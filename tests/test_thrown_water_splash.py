#!/usr/bin/env python3
"""Run shared throw entry tracking with controlled movement and particle allocation."""
import subprocess
import tempfile
import unittest
from pathlib import Path

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class ThrownWaterSplashTests(unittest.TestCase):
    def test_shared_entry_and_reset(self):
        helpers = Source(ROOT / 'src/game/object_helpers.c')
        constants = Source(ROOT / 'include/object_constants.h')
        flags = constants.declaration('enum', 'ObjFlags') + constants.macros('OBJ_FLAG_THROWN_.*')
        production = helpers.function('obj_set_held_state') + helpers.function('obj_update_thrown_water_splash')
        fixture = r'''
#include <assert.h>
#include <stddef.h>
typedef int s32; typedef unsigned int u32; typedef float f32;
typedef int BehaviorScript;
#define TRUE 1
#define FALSE 0
#define FLOOR_LOWER_LIMIT -11000
#define ACTIVE_FLAG_IGNORE_ENV_BOXES 2
#define HELD_FREE 0
#define HELD_HELD 1
#define HELD_THROWN 2
#define HELD_DROPPED 3
#define MODEL_WATER_SPLASH 4
#define PARTICLE_PRIORITY_DROPLET 5
static const BehaviorScript bhvCarrySomethingHeld[1], bhvCarrySomethingThrown[1],
    bhvCarrySomethingDropped[1], bhvThrownObjectWaterSplash[1];
struct Object {
    u32 oFlags; s32 activeFlags, oHeldState, bhvStackIndex;
    f32 oPosX, oPosY, oPosZ, oFloorHeight, oVelY;
    struct Object *parentObj;
    const BehaviorScript *curBhvCommand;
} parent, splash, *o = &parent;
static int attempts, allocationFails;
static f32 water = 100;
static void object_platform_motion_reset(struct Object *obj) {}
static const BehaviorScript *segmented_to_virtual(const BehaviorScript *b) { return b; }
static f32 find_water_level(f32 x, f32 z) { return x < 1000 ? water : FLOOR_LOWER_LIMIT; }
static struct Object *try_spawn_particle(struct Object *obj, int model,
                                         const BehaviorScript *behavior, int priority) {
    assert(model == MODEL_WATER_SPLASH && behavior == bhvThrownObjectWaterSplash);
    assert(priority == PARTICLE_PRIORITY_DROPLET);
    attempts++;
    splash = *obj;
    return allocationFails ? NULL : &splash;
}
'''
        checks = r'''
int main(void) {
    struct Object obj = { .oFlags = OBJ_FLAG_HOLDABLE, .oPosY = 200 };
    for (int cycle = 0; cycle < 3; cycle++) {
        obj.oPosY = 200;
        obj_set_held_state(&obj, bhvCarrySomethingThrown);
        assert(obj.oHeldState == HELD_THROWN);
        obj_update_thrown_water_splash(&obj, TRUE);
        obj.oHeldState = HELD_FREE; // Native actor initialization consumes held state.
        obj.oPosY = 90;
        obj_update_thrown_water_splash(&obj, TRUE);
        assert(attempts == cycle + 1 && splash.oPosY == 100);
        for (int frame = 0; frame < 20; frame++) obj_update_thrown_water_splash(&obj, TRUE);
        assert(attempts == cycle + 1);
    }
    // Dry horizontal travel into a pool, even if release had no water beneath it.
    obj.oPosX = 2000; obj.oPosY = 50;
    obj_set_held_state(&obj, bhvCarrySomethingThrown);
    obj_update_thrown_water_splash(&obj, TRUE);
    obj.oPosX = 25; obj_update_thrown_water_splash(&obj, TRUE);
    assert(attempts == 4 && splash.oPosX == 25 && splash.oPosY == water);
    // Already-submerged throws wait for a genuine exit and re-entry.
    obj.oPosY = 50;
    obj_set_held_state(&obj, bhvCarrySomethingThrown);
    obj_update_thrown_water_splash(&obj, TRUE); assert(attempts == 4);
    obj.oPosY = 150; obj_update_thrown_water_splash(&obj, TRUE);
    obj.oPosY = 100; obj_update_thrown_water_splash(&obj, TRUE); assert(attempts == 5);
    for (int held = 0; held < 2; held++) {
        obj.oPosY = 200;
        obj_set_held_state(&obj, bhvCarrySomethingThrown);
        obj_update_thrown_water_splash(&obj, TRUE);
        obj_set_held_state(&obj, held ? bhvCarrySomethingHeld : bhvCarrySomethingDropped);
        obj.oPosY = 50; obj_update_thrown_water_splash(&obj, TRUE); assert(attempts == 5);
    }
    // Particle exhaustion consumes entry instead of spawning a delayed effect.
    obj.oPosY = 200; obj_set_held_state(&obj, bhvCarrySomethingThrown);
    obj_update_thrown_water_splash(&obj, TRUE); allocationFails = 1;
    obj.oPosY = 50; obj_update_thrown_water_splash(&obj, TRUE); assert(attempts == 6);
    allocationFails = 0; obj_update_thrown_water_splash(&obj, TRUE); assert(attempts == 6);
    // Environment-ignoring and dry objects do not splash.
    obj.oPosY = 200; obj_set_held_state(&obj, bhvCarrySomethingThrown);
    obj.activeFlags = ACTIVE_FLAG_IGNORE_ENV_BOXES;
    obj_update_thrown_water_splash(&obj, TRUE); obj.oPosY = 50;
    obj_update_thrown_water_splash(&obj, TRUE); assert(attempts == 6);
    obj.activeFlags = 0; water = FLOOR_LOWER_LIMIT;
    obj_update_thrown_water_splash(&obj, TRUE); assert(attempts == 6);
    // Non-holdable actors use carry scripts and share the same tracking.
    obj.oFlags = 0; water = 100; obj.oPosY = 200;
    obj_set_held_state(&obj, bhvCarrySomethingThrown);
    assert(obj.curBhvCommand == bhvCarrySomethingThrown);
    obj_update_thrown_water_splash(&obj, TRUE); obj.oPosY = 50;
    obj_update_thrown_water_splash(&obj, TRUE); assert(attempts == 7);
    // A dry landing ends the throw; later walking into water is not a throw entry.
    obj.oPosY = 200; obj_set_held_state(&obj, bhvCarrySomethingThrown);
    obj_update_thrown_water_splash(&obj, FALSE);
    obj.oPosY = obj.oFloorHeight = 150; obj.oVelY = 0;
    obj_update_thrown_water_splash(&obj, TRUE);
    obj.oPosY = 50; obj_update_thrown_water_splash(&obj, TRUE); assert(attempts == 7);
    // Do not cancel the throw on its floor-level release before actor launch runs.
    obj.oPosY = obj.oFloorHeight = 150;
    obj_set_held_state(&obj, bhvCarrySomethingThrown);
    obj_update_thrown_water_splash(&obj, FALSE);
    assert(obj.oFlags & OBJ_FLAG_THROWN_WATER_SPLASH_PENDING);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            src = Path(directory) / 'entry.c'
            exe = Path(directory) / 'entry'
            src.write_text(flags + fixture + production + checks)
            subprocess.run(['cc', '-std=c99', '-Wall', '-Werror=implicit-function-declaration',
                            str(src), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)

    def test_entry_observations_surround_all_behavior_movement(self):
        update = Source(ROOT / 'src/engine/behavior_script.c').function('cur_obj_update')
        first = update.index('obj_update_thrown_water_splash(o, FALSE);')
        last = update.index('obj_update_thrown_water_splash(o, TRUE);')
        self.assertLess(first, update.index('bhvProcResult = bhvCmdProc();'))
        self.assertGreater(last, update.index('cur_obj_move_y_with_terminal_vel();'))
        self.assertGreater(last, update.index('obj_build_transform_relative_to_parent(o);'))


if __name__ == '__main__':
    unittest.main()
