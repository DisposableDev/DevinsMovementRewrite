#!/usr/bin/env python3
"""Host fish school/steering/lifecycle checks; these do not establish target performance."""
from pathlib import Path
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class FishBehaviorTests(unittest.TestCase):
    def test_school_and_steering(self):
        code = r'''
#include <assert.h>
#include <math.h>
#include <stdint.h>
typedef int32_t s32;
typedef int16_t s16;
typedef float f32;
typedef int ModelID16;
struct Animation { int unused; };
struct Object {
    struct Object *parentObj;
    struct { struct { struct { s16 animFrame; } animInfo; } gfx; } header;
    s32 oBehParams2ndByte, oAction, oTimer, oFishYawVel;
    s32 oAngleToMario;
    f32 oDistanceToMario, oPosY, oPosX, oPosZ, oFishWaterLevel, oFishGoalY;
    f32 oFishHeightOffset, oFishRoamDistance, oFishActiveDistance, oFishGoalVel;
    f32 oFishDepthDistance, oForwardVel, oWallHitboxRadius;
};
typedef void (*ObjActionFunc)(void);
static struct Object mario, parent, pool[16], object;
static struct Object *o = &object, *gMarioObject = &mario;
static unsigned gGlobalTimer;
static int gCurrLevelNum, spawned, selectedModel, angleCalls;
static int movements, animations, walls, deletions, sounds, waterQueries;
static const struct Animation *blue_fish_seg3_anims_0301C2B0[1];
static const struct Animation *cyan_fish_seg6_anims_0600E264[1];
#define MODEL_FISH 1
#define MODEL_CYAN_FISH 2
#define LEVEL_SA 3
#define LEVEL_JRB 12
#define LEVEL_DDD 9
#define FLOOR_LOWER_LIMIT_MISC -11000.0f
#define SOUND_GENERAL_MOVING_WATER 1
static int bhvFish;
/* CONSTANTS */
static struct Object *spawn_object(struct Object *owner, ModelID16 model, int behavior) {
    (void) behavior;
    assert(spawned < 16);
    selectedModel = model;
    pool[spawned].parentObj = owner;
    return &pool[spawned++];
}
static void obj_init_animation_with_sound(struct Object *fish,
                                        const struct Animation *const *animation, int index) {
    (void) fish; (void) index;
    assert(animation == (selectedModel == MODEL_FISH
                         ? blue_fish_seg3_anims_0301C2B0 : cyan_fish_seg6_anims_0600E264));
}
static void obj_translate_xyz_random(struct Object *fish, f32 range) {
    (void) fish;
    assert(range == 700.0f);
}
static f32 random_float(void) { return 0.5f; }
static f32 absf(f32 value) { return fabsf(value); }
static f32 approach_f32_symmetric(f32 value, f32 target, f32 step) {
    return value < target ? fminf(value + step, target) : fmaxf(value - step, target);
}
static void cur_obj_init_animation_with_accel_and_sound(int index, f32 speed) {
    (void) index; (void) speed; animations++;
}
static void cur_obj_rotate_yaw_toward(s32 angle, s32 step) {
    (void) angle; (void) step;
}
static s32 obj_angle_to_object(struct Object *fish, struct Object *target) {
    assert(fish == o && target == &mario);
    angleCalls++;
    return 100 + gGlobalTimer;
}
static void cur_obj_play_sound_2(int sound) { (void) sound; sounds++; }
static void cur_obj_scale(f32 scale) { (void) scale; }
static void cur_obj_resolve_wall_collisions(void) { walls++; }
static f32 find_water_level(f32 x, f32 z) {
    (void) x; (void) z; waterQueries++;
    return -12000.0f;
}
static void obj_mark_for_deletion(struct Object *fish) { assert(fish == o); deletions++; }
static void cur_obj_move_using_fvel_and_gravity(void) { movements++; o->oPosX += o->oForwardVel; }
static void cur_obj_call_action_function(ObjActionFunc *actions) { actions[o->oAction](); }
#include "src/game/behaviors/fish.inc.c"
static void update_fish(void) {
    // Model the angle flag's pre-behavior engine update.
    o->oAngleToMario = obj_angle_to_object(o, gMarioObject);
    bhv_fish_loop();
}
static void reset(void) {
    object = (struct Object) {0};
    parent = (struct Object) {0};
    mario = (struct Object) {0};
    o = &object;
    object.parentObj = &parent;
    object.oBehParams2ndByte = FISH_BP_FLAG_IN_FULL_WATER_LEVEL;
    object.oAction = FISH_ACT_ROAM;
    object.oTimer = 20;
    object.oDistanceToMario = 1000.0f;
    object.oPosY = -1000.0f;
    object.oForwardVel = 4.0f;
    object.oFishRoamDistance = 400.0f;
    parent.oAction = FISH_SPAWNER_ACT_IDLE;
    angleCalls = movements = animations = walls = deletions = sounds = waterQueries = 0;
}
int main(void) {
    // The full-water flag changes the spawn gate, never the school type or child flags.
    const int levels[4] = {0, LEVEL_SA, LEVEL_JRB, LEVEL_DDD};
    const int schoolCounts[4][4] = {
        {12, 5, 12, 5}, {12, 5, 12, 5}, {10, 4, 10, 4}, {10, 4, 10, 4}
    };
    for (int level = 0; level < 4; level++) {
        gCurrLevelNum = levels[level];
        for (int fullWater = 0; fullWater < 2; fullWater++) {
            for (int kind = 0; kind < 4; kind++) {
                reset(); spawned = 0;
                object.oAction = FISH_SPAWNER_ACT_SPAWN;
                int params = kind | (fullWater ? FISH_BP_FLAG_IN_FULL_WATER_LEVEL : 0);
                object.oBehParams2ndByte = params;
                object.oDistanceToMario = fullWater ? 5000.0f : 1500.0f;
                bhv_fish_spawner_loop();
                if (!fullWater) {
                    assert(spawned == 0);
                    object.oDistanceToMario = 1499.0f;
                    bhv_fish_spawner_loop();
                }
                assert(spawned == schoolCounts[level][kind]);
                assert(selectedModel == (kind < 2 ? MODEL_FISH : MODEL_CYAN_FISH));
                for (int i = 0; i < spawned; i++) {
                    assert(pool[i].oBehParams2ndByte == params);
                }
                assert(object.oAction == FISH_SPAWNER_ACT_IDLE);
            }
        }
    }
    // Unknown school types still reject spawning, with or without the water flag.
    for (int fullWater = 0; fullWater < 2; fullWater++) {
        reset(); spawned = 0;
        object.oAction = FISH_SPAWNER_ACT_SPAWN;
        object.oBehParams2ndByte = 4 | (fullWater ? FISH_BP_FLAG_IN_FULL_WATER_LEVEL : 0);
        bhv_fish_spawner_loop();
        assert(spawned == 0 && object.oAction == FISH_SPAWNER_ACT_SPAWN);
    }
    // Refresh targets and keep movement/animation running on every update.
    reset();
    for (gGlobalTimer = 0; gGlobalTimer < 24; gGlobalTimer++) {
        mario.oPosY = -1200.0f + gGlobalTimer;
        update_fish();
        assert(angleCalls == (int) gGlobalTimer + 1);
        assert(object.oFishGoalY == mario.oPosY + object.oFishHeightOffset);
        assert(object.oAngleToMario == (s32) (100 + gGlobalTimer));
        object.oTimer++;
    }
    assert(angleCalls == 24 && movements == 24 && animations == 24 && walls == 24);
    assert(object.oPosX == 96.0f && object.oAction == FISH_ACT_ROAM);
    // Close approach refreshes immediately, then flees at full rate.
    reset(); gGlobalTimer = 1; object.oDistanceToMario = 549.0f;
    update_fish();
    assert(angleCalls == 1 && object.oAction == FISH_ACT_FLEE);
    object.oTimer = 0;
    for (int i = 0; i < 6; i++) {
        update_fish(); object.oTimer++; gGlobalTimer++;
    }
    assert(angleCalls == 7 && movements == 7 && animations == 7 && sounds == 1);
    // Action entry refreshes after its random height offset has been initialized.
    reset(); gGlobalTimer = 1; object.oTimer = 0; mario.oPosY = -1400.0f;
    update_fish();
    assert(angleCalls == 1 && object.oFishGoalY == -1050.0f);
    // Flee-to-roam entry also refreshes.
    reset(); object.oAction = FISH_ACT_FLEE; object.oFishActiveDistance = 0.0f;
    update_fish(); assert(object.oAction == FISH_ACT_ROAM);
    object.oTimer = 0; gGlobalTimer = 2;
    update_fish(); assert(angleCalls == 2);
    // Missing water and spawner replacement still delete through the native loop.
    reset(); object.oBehParams2ndByte = 0;
    update_fish(); assert(deletions == 1 && movements == 0 && waterQueries == 1);
    reset(); parent.oAction = FISH_SPAWNER_ACT_RESPAWN;
    update_fish(); assert(deletions == 1 && movements == 1);
    // Spawner lifecycle retains its existing vertical respawn rule and Aquarium exemption.
    reset(); object.oAction = FISH_SPAWNER_ACT_IDLE;
    mario.oPosY = 2001.0f; gCurrLevelNum = 0;
    bhv_fish_spawner_loop(); assert(object.oAction == FISH_SPAWNER_ACT_RESPAWN);
    bhv_fish_spawner_loop(); assert(object.oAction == FISH_SPAWNER_ACT_SPAWN);
    object.oAction = FISH_SPAWNER_ACT_IDLE; gCurrLevelNum = LEVEL_SA;
    bhv_fish_spawner_loop(); assert(object.oAction == FISH_SPAWNER_ACT_IDLE);
    return 0;
}
'''
        constants = Source(ROOT / 'include/object_constants.h').macros('FISH_.*')
        code = code.replace('/* CONSTANTS */', constants)
        # The shared engine owns the full-rate angle calculation before wall resolution.
        behavior = Source(ROOT / 'data/behavior_data.c').text.split(
            'const BehaviorScript bhvFish[] = {', 1)[1].split('};', 1)[0]
        self.assertIn('OBJ_FLAG_COMPUTE_ANGLE_TO_MARIO', behavior)
        self.assertIn('OBJ_FLAG_COMPUTE_DIST_TO_MARIO', behavior)
        with tempfile.TemporaryDirectory(prefix='fish-behavior-') as directory:
            path = Path(directory)
            (path / 'test.c').write_text(code)
            subprocess.run(['cc', '-std=c99', '-O2', '-Wall', '-Wextra', '-Werror',
                            '-I', str(ROOT), str(path / 'test.c'), '-lm',
                            '-o', str(path / 'test')], check=True)
            subprocess.run([str(path / 'test')], check=True)


if __name__ == '__main__':
    unittest.main()
