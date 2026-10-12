/* Production object_step and 1-up final contact check against controlled
 * wall/floor queries. No level loading or emulator. */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "test_support.h"

typedef int16_t s16;
typedef int32_t s32;
typedef float f32;
#define OBJ_COL_FLAG_GROUNDED 1
#define OBJ_COL_FLAG_HIT_WALL 2
#define OBJ_COL_FLAG_UNDERWATER 4
#define OBJ_COL_FLAG_NO_Y_VEL 8
#define FLOOR_LOWER_LIMIT_MISC (-11000.0f)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct Surface {
    struct {
        f32 x, y, z;
    } normal;
};
struct Object {
    f32 oPosX, oPosY, oPosZ, oForwardVel, oVelY, oFloorHeight;
    f32 hitboxHeight, hitboxRadius;
    s16 oMoveAngleYaw;
    struct Surface *oFloor;
};
static struct Object sMushroom, *gCurrentObject;
#define o gCurrentObject
static struct Surface sFloor, *sObjFloor;
static s32 sMode, sFirstWallChecks, sFinalWallChecks, sFloorChecks;

static f32 sins(s16 angle) {
    return angle == 0x4000 ? 1.0f : 0.0f;
}
static f32 coss(s16 angle) {
    return angle == 0 ? 1.0f : 0.0f;
}
static s16 obj_find_wall(f32 x, f32 y, f32 z, f32 vx, f32 vz) {
    sFirstWallChecks++;
    if (sMode == 0 || sMode == 1 || sMode == 3) {
        o->oPosZ = 8.0f;
        if (sMode != 0) {
            o->oMoveAngleYaw = 0x4000;
        }
        return 0;
    }
    return 1;
}
static f32 find_floor(f32 x, f32 y, f32 z, struct Surface **floor) {
    sFloorChecks++;
    if (sMode == 3 && x > 20.0f) {
        *floor = NULL;
        return FLOOR_LOWER_LIMIT_MISC;
    }
    *floor = &sFloor;
    return x > 20.0f ? 0.0f : -20.0f;
}
static s16 turn_obj_away_from_steep_floor(struct Surface *floor, f32 height, f32 vx, f32 vz) {
    return 1;
}
static s32 find_water_level(f32 x, f32 z) {
    return -1000;
}
static void calc_new_obj_vel_and_pos_y(struct Surface *floor, f32 height, f32 vx, f32 vz) {
    o->oPosY += o->oVelY;
    if (o->oPosY < height) {
        o->oPosY = height;
        o->oVelY = 0.0f;
    }
}
static void calc_new_obj_vel_and_pos_y_underwater(struct Surface *floor, f32 height, f32 vx, f32 vz,
                                                  f32 water) {
}
static void obj_update_pos_vel_xz(void) {
    o->oPosX += o->oForwardVel * sins(o->oMoveAngleYaw);
    o->oPosZ += o->oForwardVel * coss(o->oMoveAngleYaw);
}
static void obj_orient_graph(struct Object *obj, f32 x, f32 y, f32 z) {
}
static void obj_splash(s32 water, s32 y) {
}
static s32 f32_find_wall_collision(f32 *x, f32 *y, f32 *z, f32 offset, f32 radius) {
    sFinalWallChecks++;
    if (sMode == 0 && *z > 20.0f) {
        *z = 20.0f;
        return 1;
    }
    return 0;
}
#include "one_up_sources.inc"

static void begin(s32 mode) {
    memset(&sMushroom, 0, sizeof(sMushroom));
    gCurrentObject = &sMushroom;
    sMode = mode;
    sFirstWallChecks = sFinalWallChecks = sFloorChecks = 0;
    sFloor.normal.y = 1.0f;
    o->oPosY = 5.0f;
    o->oVelY = -10.0f;
    o->oForwardVel = 40.0f;
    o->hitboxHeight = o->hitboxRadius = 30.0f;
}

static int wall_after_turn(void) {
    begin(0);
    s16 flags = one_up_step();
    return sFirstWallChecks == 1 && sFinalWallChecks == 1 && (flags & OBJ_COL_FLAG_HIT_WALL)
           && o->oPosZ == 20.0f;
}

static int floor_after_turn(void) {
    begin(1);
    s16 flags = one_up_step();
    return sFloorChecks == 2 && (flags & OBJ_COL_FLAG_GROUNDED) && o->oPosX == 40.0f && o->oPosY == 0.0f
           && o->oVelY == 0.0f && o->oFloor == &sFloor && o->oFloorHeight == 0.0f;
}

static int no_floor_no_snap(void) {
    begin(3);
    o->oPosY = -10.0f;
    s16 flags = one_up_step();
    return !(flags & OBJ_COL_FLAG_GROUNDED) && o->oPosY == -20.0f && o->oFloor == NULL;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    if (test_selected("one_up.wall_after_turn")) {
        test_report("one_up.wall_after_turn", wall_after_turn());
    }
    if (test_selected("one_up.floor_after_turn")) {
        test_report("one_up.floor_after_turn", floor_after_turn());
    }
    if (test_selected("one_up.no_floor")) {
        test_report("one_up.no_floor", no_floor_no_snap());
    }
    return test_finish();
}
