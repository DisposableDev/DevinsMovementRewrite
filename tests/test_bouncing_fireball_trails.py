"""Lava fireball hazard routing, cosmetic lifetime and emitter random consumption."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class BouncingFireballTrailTests(unittest.TestCase):
    def test_hazard_and_trails(self):
        source = Source(ROOT / "src/game/behaviors/bouncing_fireball.inc.c")
        functions = "\n".join(source.function(name) for name in (
            "bouncing_fireball_trail_loop", "bhv_bouncing_fireball_flame_loop",
            "bhv_bouncing_fireball_spawner_loop"))
        code = r'''
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
typedef float f32;
#define ACTIVE_FLAG_IGNORE_ENV_BOXES 1
#define ACTIVE_FLAG_FAR_AWAY 2
#define ACTIVE_FLAG_IN_DIFFERENT_ROOM 4
#define OBJ_MOVE_LANDED 1
#define OBJ_MOVE_ON_GROUND 2
#define OBJ_MOVE_AT_WATER_SURFACE 4
#define OBJ_MOVE_UNDERWATER_ON_GROUND 8
#define INT_STATUS_NONE 0
#define MODEL_RED_FLAME 1
static const int bhvBouncingFireballFlame[] = {0};
struct Object {
    int activeFlags, oTimer, oAction, oAnimState, oMoveFlags, oInteractStatus;
    int oBouncingFireballIsTrail, oIntangibleTimer, oBouncingFireBallSpawnerRandomCooldown;
    int deleted, oMoveAngleYaw;
    f32 oPosX, oPosY, oPosZ, oVelY, oForwardVel, oGravity, oBounciness;
    f32 oBouncingFireballTrailFloorY, oDistanceToMario;
    struct { struct { f32 scale[3]; } gfx; } header;
};
static struct Object emitter, flames[12], *o;
static int spawned, randomCalls, fullCollisionCalls, fullMovementCalls;
static f32 random_float(void) { randomCalls++; return 0.5f; }
static f32 sins(int angle) { return sinf(angle * 6.28318530718f / 65536); }
static f32 coss(int angle) { return cosf(angle * 6.28318530718f / 65536); }
static void obj_scale(struct Object *obj, f32 scale) {
    for (int axis = 0; axis < 3; axis++) {
        obj->header.gfx.scale[axis] = scale;
    }
}
static void cur_obj_scale(f32 scale) { obj_scale(o, scale); }
static void obj_become_tangible(struct Object *obj) { obj->oIntangibleTimer = 0; }
static void obj_mark_for_deletion(struct Object *obj) { obj->deleted = 1; }
static void cur_obj_update_floor_and_walls(void) { fullCollisionCalls++; }
static void cur_obj_move_standard(int angle) {
    assert(angle == 78);
    fullMovementCalls++;
}
static struct Object *spawn_object(struct Object *parent, int model, const int *behavior) {
    assert(model == MODEL_RED_FLAME && behavior == bhvBouncingFireballFlame);
    assert(spawned < 12);
    struct Object *obj = &flames[spawned++];
    memset(obj, 0, sizeof(*obj));
    obj->oPosX = parent->oPosX;
    obj->oPosY = parent->oPosY;
    obj->oPosZ = parent->oPosZ;
    obj->oMoveAngleYaw = parent->oMoveAngleYaw;
    obj->oIntangibleTimer = -1;
    obj->oGravity = -4;
    obj->oBounciness = -0.7f;
    return obj;
}
/* FUNCTIONS */
static void update_flame(struct Object *obj) {
    o = obj;
    int action = o->oAction;
    o->oInteractStatus = 123;
    bhv_bouncing_fireball_flame_loop();
    assert(o->oInteractStatus == INT_STATUS_NONE);
    assert(o->activeFlags & ACTIVE_FLAG_IGNORE_ENV_BOXES);
    if (o->oAction != action) {
        o->oTimer = 0;
    } else {
        o->oTimer++;
    }
}
static void test_burst(void) {
    // Nonzero emission height also covers the Physics Lab emitter.
    emitter = (struct Object) {.oAction = 1, .oPosY = 123, .oMoveAngleYaw = 0x4000};
    for (int timer = 0; timer < 12; timer++) {
        o = &emitter;
        o->oTimer = timer;
        bhv_bouncing_fireball_spawner_loop();
        struct Object *obj = &flames[timer];
        assert(obj->oBouncingFireballIsTrail == (timer != 0));
        assert(obj->oBouncingFireballTrailFloorY == 123);
        assert(obj->oIntangibleTimer == (timer == 0 ? 0 : -1));
        assert(obj->header.gfx.scale[0] == (10 - timer) * 0.5f);
        update_flame(obj);
    }
    assert(emitter.oAction == 2 && spawned == 12 && randomCalls == 12);
    assert(fullCollisionCalls == 1 && fullMovementCalls == 1);
    o = &emitter;
    o->oTimer = 0;
    bhv_bouncing_fireball_spawner_loop();
    assert(randomCalls == 13 && emitter.oBouncingFireBallSpawnerRandomCooldown == 50);
    o->oTimer = 150;
    bhv_bouncing_fireball_spawner_loop();
    assert(o->oAction == 2);
    o->oTimer = 151;
    bhv_bouncing_fireball_spawner_loop();
    assert(o->oAction == 0);

    for (int i = 1; i < 12; i++) {
        struct Object *obj = &flames[i];
        f32 initialScale = obj->header.gfx.scale[0];
        int frames = 0, moved = 0;
        while (!obj->deleted && frames++ < 120) {
            update_flame(obj);
            assert(obj->oIntangibleTimer == -1);
            assert(obj->oPosY >= 123 && isfinite(obj->oPosY));
            if (obj->oPosX > 0) {
                moved = 1;
            }
        }
        assert(obj->deleted && moved && frames <= 120);
        assert(fabsf(obj->header.gfx.scale[0]) <= fabsf(initialScale));
    }
    assert(randomCalls == 13); // No extra random draws when the trail starts bouncing.
    assert(fullCollisionCalls == 1 && fullMovementCalls == 1);
}
static void test_hazard_stages(void) {
    struct Object *head = &flames[0];
    head->oMoveFlags = OBJ_MOVE_LANDED;
    update_flame(head);
    assert(head->oAction == 1 && head->oTimer == 0);
    update_flame(head);
    assert(head->oVelY == 50 && head->oForwardVel == 30 && !head->deleted);
    head->oTimer = 101;
    head->oMoveFlags = 0;
    update_flame(head);
    assert(!head->deleted); // Airborne hazards retain the longer native lifetime.
    head->oMoveFlags = OBJ_MOVE_ON_GROUND;
    update_flame(head);
    assert(head->deleted && head->oIntangibleTimer == 0);
    assert(fullCollisionCalls == 5 && fullMovementCalls == 5);
}
static void test_distant_cleanup(void) {
    struct Object trail = {.activeFlags = ACTIVE_FLAG_FAR_AWAY, .oBouncingFireballIsTrail = 1,
                           .oPosY = 321, .oGravity = -4, .oIntangibleTimer = -1};
    for (int frame = 0; frame < 102; frame++) {
        update_flame(&trail);
        assert(trail.oPosY == 321);
    }
    assert(trail.deleted && trail.oIntangibleTimer == -1);
    assert(fullCollisionCalls == 5 && fullMovementCalls == 5);
}
int main(void) {
    test_burst();
    test_hazard_stages();
    test_distant_cleanup();
    puts("lava fireballs: hazards retain full movement; trails skip queries and expire");
}
'''
        with tempfile.TemporaryDirectory(prefix="lava-trails-") as directory:
            path = Path(directory)
            (path / "test.c").write_text(code.replace("/* FUNCTIONS */", functions))
            subprocess.run(["cc", "-std=c99", "-O1", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie",
                            str(path / "test.c"), "-lm", "-o", str(path / "test")], check=True)
            subprocess.run([str(path / "test")], check=True,
                           env=dict(os.environ, ASAN_OPTIONS="detect_leaks=0"))


if __name__ == "__main__":
    unittest.main()
