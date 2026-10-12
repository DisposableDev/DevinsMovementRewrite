#!/usr/bin/env python3
"""Sliding 1-Up pickup once per contact: native loop/query/flicker, controlled services.

Movement/distance, graphics/audio and player service effects are boundaries.
This checks current healing with lives disabled and optional lives enabled;
it does not simulate a whole slide trajectory or ROM playback.
"""
from pathlib import Path
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]

PRELUDE = r"""

#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef int32_t s32;
typedef int16_t s16;
#define TRUE 1
#define FALSE 0
#define ACTIVE_FLAG_DEACTIVATED 0
#define MUSHROOMS_HEAL
#define SOUND_GENERAL_COLLECT_1UP 1
#define PS_1_UP_MUSHROOMS_COLLECTED 19
struct Object {
    s32 oAction, oTimer, activeFlags, numCollidedObjs, oPosX, oPosY, oPosZ;
    struct Object *collidedObjs[4];
};
struct MarioState {
    s32 health, respawnHealCounter, healCounter, hurtCounter, numLives;
};
static struct Object mushroom, marioObject;
static struct Object *o = &mushroom, *gMarioObject = &marioObject;
static struct MarioState mario;
static struct MarioState *gMarioState = &mario;
static s32 gGlobalSoundSource;
static s32 rewards, sounds, cooldowns, extinguishes, rumbles;
static s32 sparkles, visibility, movement, blinks, blinkPhase;
static void player_stats_add(s32 stat, s32 count) {
    (void) stat;
    rewards += count;
}
static void play_sound(s32 sound, s32 origin) {
    (void) sound;
    (void) origin;
    sounds++;
}
static void mario_ground_twirl_reset_cooldown(void) {
    cooldowns++;
}
static void mario_extinguish_fire(struct MarioState *state) {
    (void) state;
    extinguishes++;
}
static void queue_rumble_data(s32 time, s32 strength) {
    (void) time;
    (void) strength;
    rumbles++;
}
static void one_up_spawn_sparkles(void) {
    sparkles++;
}
static void set_object_visibility(struct Object *obj, s32 distance) {
    (void) obj;
    (void) distance;
    visibility++;
}
static s32 is_point_within_radius_of_mario(s32 x, s32 y, s32 z, s32 radius) {
    (void) x;
    (void) y;
    (void) z;
    (void) radius;
    return FALSE;
}
static void sliding_1up_move(void) {
    movement++;
}
static void obj_set_blink_phase(struct Object *obj, s32 phase) {
    (void) obj;
    blinks++;
    blinkPhase = phase;
}
"""

DRIVER = r"""

int main(void) {
    const s32 actions[] = { MUSHROOM_ACT_INIT,         MUSHROOM_ACT_MOVING,
                            MUSHROOM_ACT_DISAPPEARING, MUSHROOM_ACT_DISAPPEARING,
                            MUSHROOM_ACT_DISAPPEARING, MUSHROOM_ACT_DISAPPEARING };
    const s32 timers[] = { 0, 0, 29, 30, 69, 70 };
    s32 failed = 0;
    for (s32 i = 0; i < 6; i++) {
        for (s32 contact = 0; contact <= 1; contact++) {
            memset(&mushroom, 0, sizeof(mushroom));
            memset(&mario, 0, sizeof(mario));
            mushroom.oAction = actions[i];
            mushroom.oTimer = timers[i];
            mushroom.activeFlags = 1;
            mushroom.numCollidedObjs = contact;
            mushroom.collidedObjs[0] = gMarioObject;
            mario.health = 0x880;
            mario.hurtCounter = 7;
            mario.respawnHealCounter = 9;
            mario.numLives = 4;
            rewards = sounds = cooldowns = extinguishes = rumbles = 0;
            sparkles = visibility = movement = blinks = blinkPhase = 0;
            bhv_1up_sliding_loop();
            s32 expectActive = contact || timers[i] >= 70 ? 0 : 1;
            s32 bad =
                rewards != contact || sounds != contact || cooldowns != contact
                || extinguishes != contact || rumbles != contact || mushroom.activeFlags != expectActive
                || sparkles != 1 || visibility != (actions[i] == MUSHROOM_ACT_INIT)
                || movement != (actions[i] == MUSHROOM_ACT_MOVING)
                || blinks != (timers[i] >= 30 && timers[i] < 70)
                || (blinks && blinkPhase != (timers[i] & 1)) || mario.healCounter != (contact ? 31 : 0)
                || mario.hurtCounter != (contact ? 0 : 7)
                || mario.respawnHealCounter != (contact ? 0 : 9);
#ifdef ENABLE_LIVES
            bad |= mario.numLives != 4 + contact;
#else
            bad |= mario.numLives != 4;
#endif
            if (bad) {
                fprintf(stderr,
                        "{\"action\":%d,\"timer\":%d,\"contact\":%d,\"rewards\":%d,\"sounds\":%d,"
                        "\"rumbles\":%d,\"lives\":%d,\"failed\":%d}\n",
                        actions[i], timers[i], contact, rewards, sounds, rumbles, mario.numLives, bad);
            }
            failed |= bad;
        }
    }
    return failed;
}
"""


class SlidingOneUpPickupTests(unittest.TestCase):
    def test_single_pickup_and_disappearance_boundaries(self):
        mushroom = Source(ROOT / "src/game/behaviors/mushroom_1up.inc.c")
        production = Source(ROOT / "include/object_constants.h").declaration(
            "enum", "oActions1UpMushroom")
        production += Source(ROOT / "src/game/object_helpers.c").function(
            "obj_check_if_collided_with_object")
        production += Source(ROOT / "src/game/obj_behaviors.c").function(
            "obj_flicker_and_disappear")
        production += mushroom.function("bhv_1up_interact")
        production += mushroom.function("bhv_1up_sliding_loop")
        with tempfile.TemporaryDirectory() as temporary:
            folder = Path(temporary)
            source = folder / "fixture.c"
            source.write_text(PRELUDE + production + DRIVER)
            for lives_enabled in (False, True):
                with self.subTest(lives_enabled=lives_enabled):
                    binary = folder / ("with_lives" if lives_enabled else "current")
                    args = ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=undefined", "-fno-sanitize-recover=all",
                            str(source), "-o", str(binary)]
                    if lives_enabled:
                        args.insert(1, "-DENABLE_LIVES")
                    subprocess.run(args, check=True, timeout=30)
                    subprocess.run([str(binary)], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
