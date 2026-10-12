/* Native Fire Spitter action logic; engine collision and frame clocks are boundaries. */
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "../include/object_constants.h"

typedef int32_t s32;
typedef float f32;
#ifndef FIRE_SPITTER_SLEEP_TRIAL
#define FIRE_SPITTER_SLEEP_TRIAL 0
#endif
#define FALSE 0
#define TRUE 1
#define INT_STATUS_NONE 0
#define SURFACE_FLAG_DYNAMIC 1
#define SOUND_OBJ_FLAME_BLOWN 1
#define MODEL_RED_FLAME_SHADOW 1
#define absf fabsf

struct Surface {
    s32 flags;
};
struct Object {
    struct {
        struct {
            f32 scale[3];
        } gfx;
    } header;
    s32 oAction, oPrevAction, oTimer, oInteractStatus, numCollidedObjs, oMoveFlags;
    s32 oMoveAngleYaw, oAngleToMario;
    f32 oForwardVel, oVelX, oVelY, oVelZ, oGravity, oBuoyancy;
    f32 oPosX, oPosY, oPosZ, oFloorHeight;
    f32 oDistanceToMario, oFireSpitterScaleVel, oGraphYOffset;
    struct Surface *oFloor;
};
static struct Object actor, mario, *o = &actor, *gMarioObject = &mario;
static struct Surface floorSurface;
static s32 floorCalls, moveCalls, waterCalls, scaleCalls, fireCalls;
static f32 waterHeight;
static void approach_f32_ptr(f32 *value, f32 goal, f32 step) {
    *value = *value < goal ? fminf(*value + step, goal) : fmaxf(*value - step, goal);
}
static s32 obj_grow_then_shrink(f32 *velocity, f32 threshold, f32 goal) {
    (void) velocity;
    (void) threshold;
    (void) goal;
    return 1;
}
static void cur_obj_play_sound_2(s32 sound) {
    (void) sound;
}
static void obj_spit_fire(s32 a, s32 b, s32 c, f32 scale, s32 model, f32 speed, f32 y, s32 angle) {
    (void) a;
    (void) b;
    (void) c;
    (void) scale;
    (void) model;
    (void) speed;
    (void) y;
    (void) angle;
    fireCalls++;
}
static void cur_obj_scale(f32 scale) {
    scaleCalls++;
    for (s32 axis = 0; axis < 3; axis++) {
        o->header.gfx.scale[axis] = scale;
    }
}
static void cur_obj_update_floor_and_walls(void) {
    floorCalls++;
}
static void cur_obj_move_standard(s32 angle) {
    assert(angle == 78);
    moveCalls++;
}
#if FIRE_SPITTER_SLEEP_TRIAL
static f32 find_water_level(f32 x, f32 z) {
    (void) x;
    (void) z;
    waterCalls++;
    return waterHeight;
}
#endif
void bhv_fire_spitter_update(void);
/* SOURCES */

static void reset(void) {
    memset(&actor, 0, sizeof(actor));
    memset(&mario, 0, sizeof(mario));
    gMarioObject = &mario;
    floorSurface.flags = 0;
    actor.oFloor = &floorSurface;
    actor.oPosX = 2501;
    actor.oTimer = 151;
    actor.oMoveFlags = OBJ_MOVE_ON_GROUND;
    actor.oDistanceToMario = 2501;
    for (s32 axis = 0; axis < 3; axis++) {
        actor.header.gfx.scale[axis] = 0.2f;
    }
    waterHeight = -10000;
    floorCalls = moveCalls = waterCalls = scaleCalls = fireCalls = 0;
}
static void expect_physics(s32 expected) {
    actor.oDistanceToMario =
        sqrtf(actor.oPosX * actor.oPosX + (actor.oPosY - mario.oPosY) * (actor.oPosY - mario.oPosY)
              + actor.oPosZ * actor.oPosZ);
    bhv_fire_spitter_update();
    assert(floorCalls == expected && moveCalls == expected);
    assert(scaleCalls == 1);
}
int main(void) {
    reset();
    expect_physics(!FIRE_SPITTER_SLEEP_TRIAL);
    assert(waterCalls == FIRE_SPITTER_SLEEP_TRIAL);
    // Actual native idle dispatch continues; wake does not start a new cooldown.
    actor.oPosX = actor.oDistanceToMario = 500;
    floorCalls = moveCalls = scaleCalls = 0;
    expect_physics(1);
    assert(actor.oAction == FIRE_SPITTER_ACT_SPIT_FIRE && actor.oTimer == 151);
    floorCalls = moveCalls = scaleCalls = 0;
    expect_physics(1);
    assert(fireCalls == 1);

    reset();
    actor.oPosX = actor.oDistanceToMario = 2500;
    expect_physics(1);
    reset();
    actor.oPosX = actor.oDistanceToMario = 0;
    mario.oPosY = 1000;
    expect_physics(1);
    reset();
    actor.oPosX = actor.oDistanceToMario = 0;
    mario.oPosY = 1001;
    expect_physics(!FIRE_SPITTER_SLEEP_TRIAL);
    reset();
    actor.oPosX = actor.oDistanceToMario = 0;
    mario.oPosY = -1001;
    expect_physics(!FIRE_SPITTER_SLEEP_TRIAL);
    reset();
    actor.oPosX = 2000;
    actor.oPosZ = 2000;
    expect_physics(!FIRE_SPITTER_SLEEP_TRIAL);

    reset();
    gMarioObject = NULL;
    expect_physics(1);
    reset();
    actor.oTimer = 150;
    expect_physics(1);
    reset();
    actor.oPrevAction = FIRE_SPITTER_ACT_SPIT_FIRE;
    expect_physics(1);
    reset();
    actor.oInteractStatus = 1;
    expect_physics(1);
    reset();
    actor.numCollidedObjs = 1;
    expect_physics(1);
    reset();
    actor.oForwardVel = 1;
    expect_physics(1);
    reset();
    actor.oVelY = -1;
    expect_physics(1);
    reset();
    actor.oVelX = 1;
    expect_physics(1);
    reset();
    actor.oVelZ = 1;
    expect_physics(1);
    reset();
    actor.oGravity = -4;
    expect_physics(1);
    reset();
    actor.oBuoyancy = 1;
    expect_physics(1);
    reset();
    actor.oFloor = NULL;
    expect_physics(1);
    reset();
    floorSurface.flags = SURFACE_FLAG_DYNAMIC;
    expect_physics(1);
    reset();
    actor.oMoveFlags = OBJ_MOVE_IN_AIR;
    actor.oPosY = 20;
    expect_physics(!FIRE_SPITTER_SLEEP_TRIAL);
    reset();
    actor.oMoveFlags |= OBJ_MOVE_MASK_IN_WATER;
    expect_physics(1);
    reset();
    actor.oMoveFlags |= OBJ_MOVE_ABOVE_LAVA;
    expect_physics(1);
    reset();
    actor.oMoveFlags |= OBJ_MOVE_ABOVE_DEATH_BARRIER;
    expect_physics(1);
    reset();
    actor.oPosY = -2;
    expect_physics(1);
    reset();
    waterHeight = actor.oPosY;
    expect_physics(1);
    reset();
    waterHeight = actor.oPosY + 1;
    expect_physics(1);
    reset();
    actor.header.gfx.scale[0] = 0.1f;
    expect_physics(!FIRE_SPITTER_SLEEP_TRIAL);
    assert(actor.header.gfx.scale[0] > 0.1f); // Idle growth remains live.
    return 0;
}
