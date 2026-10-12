#!/usr/bin/env python3
"""Exercise Yoshi's conversation, gift, footsteps and departure with host services."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class YoshiEncounterTests(unittest.TestCase):
    def test_gift_and_departure(self):
        source = Source(ROOT / 'src/game/behaviors/yoshi.inc.c')
        code = r'''
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>
typedef int16_t s16;
typedef int32_t s32;
typedef uint32_t u32;
#define UNUSED __attribute__((unused))
struct Object {
    struct { struct { struct { s16 animFrame; } animInfo; } gfx; } header;
    float oPosX, oPosY, oPosZ, oHomeX, oHomeZ, oForwardVel, oVelY;
    int oAction, oTimer, oMoveAngleYaw, oYoshiTargetYaw, oInteractStatus, oAngleToMario;
    int activeFlags, oCoinSpawnConfigured, oInteractionSubtype, oDamageOrCoinValue;
};
struct MarioState {
    int action, input, numCoins, healCounter, hurtCounter, burnCoinJumpPending;
    int health, yoshiCoinDeathPending, framesSinceA;
};
static struct Object yoshi, mario, coin;
static struct Object *o = &yoshi, *gMarioObject = &mario;
static int sounds, spawned, fail_spawn, dialog_stopped, gObjCutsceneDone, sYoshiDead;
static int bhvSingleCoinGetsSpawned, bhvYoshi;
static int jingles, dialog_state, dialog_done, dialog_calls;
static int animation;
#define MIN(a,b) ((a)<(b)?(a):(b))
#define GO_INFINITE_HEALTH 1
static int secretsBlocked;
static int game_options_secrets_blocked(void) { return secretsBlocked; }
static int game_options_cheat_active(int option) {
    assert(option == GO_INFINITE_HEALTH); return 0;
}
static void player_stats_coin(struct Object *obj, int total) { (void)obj; (void)total; }
static void player_stats_damage(void) {}
static void mario_air_twirl_reset_on_coin(struct MarioState *m) { (void)m; }
#define YOSHI_ANIM_IDLE 0
#define FALSE 0
#define ACT_BURNING_GROUND 10
#define ACT_WALKING 11
#define INPUT_A_DOWN 1
#define INPUT_A_PRESSED 2
#define TRUE 1
#define MODEL_YELLOW_COIN 1
#define MODEL_YOSHI 2
#define SOUND_GENERAL_YOSHI_WALK 3
#define SOUND_GENERAL_ENEMY_ALERT1 4
#define CUTSCENE_STAR_SPAWN 5
#define MARIO_DIALOG_STOP 6
#define MARIO_DIALOG_LOOK_FRONT 7
#define MARIO_DIALOG_STATUS_START 1
#define MARIO_DIALOG_STATUS_SPEAK 2
#define ACTIVE_FLAG_INITIATED_TIME_STOP 2
#define CUTSCENE_DIALOG 8
#define DIALOG_161 161
#define INT_STATUS_NONE 0
#define INT_STATUS_INTERACTED 7
#define ACTIVE_FLAG_DEACTIVATED 0
#define YOSHI_ACT_IDLE 0
#define YOSHI_ACT_WALK 1
#define YOSHI_ACT_TALK 2
#define YOSHI_ACT_WALK_JUMP_OFF_ROOF 3
#define YOSHI_ACT_FINISH_JUMPING_AND_DESPAWN 4
#define YOSHI_ACT_GIVE_PRESENT 5
static void object_step(void) {}
static void cur_obj_init_animation(int i) { animation = i; }
static int is_point_close_to_object(struct Object *obj, float x, float y, float z, int radius) {
    (void)obj; (void)x; (void)y; (void)z; (void)radius; return 0;
}
#define approach_s16_symmetric(a, b, c) approach_s16((a), (b), (c), (c))
/* TURN APPROACH */
static s16 atan2s(float z, float x) {
    (void)z;
    (void)x;
    return 0x4000;
}
static float dist_between_objects(struct Object *a, struct Object *b) {
    return sqrtf((a->oPosX-b->oPosX)*(a->oPosX-b->oPosX)
                 +(a->oPosY-b->oPosY)*(a->oPosY-b->oPosY)
                 +(a->oPosZ-b->oPosZ)*(a->oPosZ-b->oPosZ));
}
static int obj_angle_to_object(struct Object *a, struct Object *b) {
    assert(a == &yoshi && b == &mario); return 0x4000;
}
static void cur_obj_play_sound_2(int sound) { assert(sound == SOUND_GENERAL_YOSHI_WALK); sounds++; }
static void create_respawner(int model, int behavior, int range) {
    (void)model; (void)behavior; (void)range; assert(0);
}
static void cutscene_object(int scene, struct Object *obj) { (void)scene; (void)obj; }
static struct Object *spawn_object_relative(int param, int x, int y, int z,
                                           struct Object *owner, int model, int behavior) {
    assert(param == 0 && x == 0 && y == 120 && z == 0);
    assert(owner == &yoshi && model == MODEL_YELLOW_COIN && behavior == bhvSingleCoinGetsSpawned);
    spawned++; return fail_spawn ? NULL : &coin;
}
static void cur_obj_extend_animation_if_at_end(void) {}
static void obj_move_xyz_using_fvel_and_yaw(struct Object *obj) { (void)obj; }
static int set_mario_npc_dialog(int state) {
    if (state == MARIO_DIALOG_STOP) {
        dialog_stopped++;
        return 0;
    }
    assert(state == MARIO_DIALOG_LOOK_FRONT);
    return dialog_state;
}
static void play_puzzle_jingle(void) {
    jingles++;
}
static int hud_reveals, twirl_resets, extinguishes;
static void hud_reveal_counters(void) { hud_reveals++; }
static void mario_ground_twirl_reset_cooldown(void) { twirl_resets++; }
static void mario_extinguish_fire(struct MarioState *m) {
    extinguishes++;
    if (m->action == ACT_BURNING_GROUND) m->action = ACT_WALKING;
}
static void queue_rumble_data(int duration, int strength) {
    assert(duration == 5 && strength == 80);
}
static int cutscene_object_with_dialog(int scene, struct Object *obj, int dialog) {
    assert(scene == CUTSCENE_DIALOG && obj == &yoshi && dialog == DIALOG_161);
    dialog_calls++;
    return dialog_done;
}
/* PRODUCTION */
int main(void) {
    yoshi.oPosY = mario.oPosY = 3174;
    void (*walks[])(void) = { yoshi_walk_loop, yoshi_walk_and_jump_off_roof_loop };
    for (int mode=0; mode<2; mode++) {
        for (int frame=0; frame<30; frame++) {
            for (int boundary=-1; boundary<=1; boundary++) {
                yoshi.header.gfx.animInfo.animFrame = frame;
                yoshi.oTimer = 10;
                mario.oPosX = 1000.0f + boundary;
                sounds=0; walks[mode]();
                assert(sounds == ((frame == 0 || frame == 15) && boundary <= 0));
            }
        }
        // Vertical separation counts too, rather than just horizontal distance.
        mario.oPosX=0; mario.oPosY=4175; sounds=0;
        yoshi.header.gfx.animInfo.animFrame=0; walks[mode](); assert(sounds == 0);
        mario.oPosY=3174;
    }
    // Long turns, already aligned, and signed-angle wrap all play once per entry.
    s16 headings[] = {0x4000, -0x8000, 0, 0x7f00};
    for (unsigned heading = 0; heading < sizeof(headings) / sizeof(headings[0]); heading++) {
        for (int conversation = 0; conversation < 2; conversation++) {
            yoshi.oAction = YOSHI_ACT_TALK;
            yoshi.oTimer = 0;
            yoshi.oMoveAngleYaw = headings[heading];
            yoshi.oAngleToMario = heading == 3 ? -0x7f00 : 0;
            yoshi.activeFlags = 1;
            yoshi.oInteractStatus = INT_STATUS_INTERACTED;
            jingles = dialog_done = dialog_calls = 0;
            dialog_state = MARIO_DIALOG_STATUS_START;
            while ((s16)yoshi.oMoveAngleYaw != (s16)yoshi.oAngleToMario) {
                yoshi_talk_loop();
                yoshi.oTimer++;
                assert(yoshi.oTimer <= 26 && jingles == 1 && dialog_calls == 0);
            }
            // Mario can still be turning after Yoshi faces him.
            for (int tick = 0; tick < 8; tick++) {
                yoshi_talk_loop();
                yoshi.oTimer++;
                assert(jingles == 1 && dialog_calls == 0 && yoshi.activeFlags == 1);
            }
            dialog_state = MARIO_DIALOG_STATUS_SPEAK;
            for (int tick = 0; tick < 5; tick++) {
                yoshi_talk_loop();
                yoshi.oTimer++;
                assert(jingles == 1 && yoshi.oAction == YOSHI_ACT_TALK);
                assert(yoshi.activeFlags == (1 | ACTIVE_FLAG_INITIATED_TIME_STOP));
            }
            dialog_done = 1;
            yoshi_talk_loop();
            assert(jingles == 1 && dialog_calls == 6);
            assert(yoshi.oAction == YOSHI_ACT_GIVE_PRESENT && yoshi.activeFlags == 1);
            assert(yoshi.oInteractStatus == INT_STATUS_NONE);
            assert(yoshi.oHomeX == -1364 && yoshi.oHomeZ == -5912);
        }
    }
    // Interrupting a turn does not prevent a later fresh conversation's jingle.
    yoshi.oMoveAngleYaw = -0x8000;
    yoshi.oAngleToMario = 0;
    yoshi.oTimer = 0;
    jingles = 0;
    yoshi_talk_loop();
    yoshi.oTimer++;
    yoshi_talk_loop();
    assert(jingles == 1);
    yoshi.oTimer = 0;
    yoshi_talk_loop();
    assert(jingles == 2);
    mario.oPosX=300; yoshi.oAction=YOSHI_ACT_GIVE_PRESENT;
    yoshi.oForwardVel = 10;
    for (int tick=0; tick<45; tick++) {
        yoshi.oTimer = tick;
        yoshi_give_present_loop();
        assert(yoshi.oAction == YOSHI_ACT_GIVE_PRESENT && spawned == 1);
        assert(yoshi.oForwardVel == 0 && animation == YOSHI_ANIM_IDLE);
    }
    yoshi.oTimer = 45;
    yoshi_give_present_loop();
    assert(spawned == 1 && yoshi.oAction == YOSHI_ACT_WALK_JUMP_OFF_ROOF);
    assert(coin.oCoinSpawnConfigured && coin.oMoveAngleYaw == 0x4000);
    assert(coin.oForwardVel == 15 && coin.oVelY == 30);
    assert(coin.oInteractionSubtype & INT_SUBTYPE_YOSHI_GIFT_COIN);
    // Gift collection queues exactly one wedge of damage, preserving other queues.
    struct MarioState m = { .numCoins=7, .healCounter=2, .hurtCounter=3, .health=0x580 };
    coin.oDamageOrCoinValue = 1;
    assert(interact_coin(&m, 0, &coin) == FALSE);
    assert(m.numCoins == 8 && m.healCounter == 2 && m.hurtCounter == 7);
    assert(coin.oInteractStatus == INT_STATUS_INTERACTED);
    assert(hud_reveals == 1 && twirl_resets == 1 && extinguishes == 1);
    // Ordinary yellow, red and blue coins still heal by their coin value.
    struct Object ordinary = {0};
    int values[] = {1, 2, 5};
    for (int i=0; i<3; i++) {
        m = (struct MarioState){ .health=0x580 };
        ordinary.oDamageOrCoinValue = values[i];
        interact_coin(&m, 0, &ordinary);
        assert(m.numCoins == values[i] && m.healCounter == 4 * values[i]);
        assert(m.hurtCounter == 0 && ordinary.oInteractStatus == INT_STATUS_INTERACTED);
    }
    yoshi.oPosY=2200; yoshi.activeFlags=1;
    yoshi_finish_jumping_and_despawn_loop();
    assert(!sYoshiDead && !dialog_stopped && yoshi.activeFlags == 1);
    yoshi.oPosY=2099; yoshi_finish_jumping_and_despawn_loop();
    assert(sYoshiDead && gObjCutsceneDone && dialog_stopped == 1 && !yoshi.activeFlags);
    // Allocation failure waits the same duration without retrying the coin.
    fail_spawn=1; yoshi.oAction=YOSHI_ACT_GIVE_PRESENT;
    for (int tick=0; tick<45; tick++) {
        yoshi.oTimer = tick;
        yoshi_give_present_loop();
        assert(yoshi.oAction == YOSHI_ACT_GIVE_PRESENT && spawned == 2);
    }
    yoshi.oTimer = 45;
    yoshi_give_present_loop();
    assert(yoshi.oAction == YOSHI_ACT_WALK_JUMP_OFF_ROOF && spawned == 2);
    return 0;
}
'''
        names = ['yoshi_talk_loop', 'yoshi_walk_loop', 'yoshi_walk_and_jump_off_roof_loop',
                 'yoshi_give_present_loop', 'yoshi_finish_jumping_and_despawn_loop']
        code = code.replace('/* TURN APPROACH */',
                            Source(ROOT / 'src/engine/math_util.c').function('approach_s16'))
        homes = source.region(r'^static s16 sYoshiHomeLocations', r'^void bhv_yoshi_init')
        code = code.replace('/* PRODUCTION */', source.macros('YOSHI_PRESENT_PAUSE_UPDATES')
                            + Source(ROOT / 'src/game/interaction.h').macros('INT_SUBTYPE_YOSHI_GIFT_COIN')
                            + homes + '\n'.join(source.function(n) for n in names)
                            + Source(ROOT / 'src/game/mario.h').macros('MARIO_BURN_COIN_JUMP_BUFFER_FRAMES')
                            + Source(ROOT / 'src/game/mario.c').function('mario_health_after_pending_damage')
                            + Source(ROOT / 'src/game/interaction.c').function('interact_coin'))
        with tempfile.TemporaryDirectory(prefix='yoshi-encounter-') as folder:
            path = Path(folder)
            (path / 'test.c').write_text(code)
            subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                            '-fsanitize=address,undefined', '-fno-pie', '-no-pie',
                            str(path / 'test.c'), '-lm', '-o', str(path / 'test')], check=True)
            subprocess.run([str(path / 'test')], check=True,
                           env={**os.environ, 'ASAN_OPTIONS': 'detect_leaks=0'})


if __name__ == '__main__':
    unittest.main()
