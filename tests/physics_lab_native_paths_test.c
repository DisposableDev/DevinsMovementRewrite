// Native checkerboard and Spindel paths, observed through their real meshes.
// Sound/collision publication are stubs; no Mario or renderer is simulated.
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int16_t s16;
typedef int32_t s32;
typedef float f32;
typedef float Vec3f[3];
typedef s16 Vec3s[3];
typedef float Mat4[4][4];
struct Object {
    uint32_t oBehParams;
    int oBehParams2ndByte, oAction, oTimer, oAngleVelPitch;
    int oMoveAnglePitch, oMoveAngleYaw, oFaceAnglePitch, oFaceAngleYaw;
    int oCheckerBoardPlatformHeight, oCheckerBoardPlatformRotateAction;
    int oSpindelMoveTimer, oSpindelMoveDirection;
    float oPosX, oPosY, oPosZ, oHomeY, oForwardVel, oVelY, oVelZ;
    float oDistanceToMario, oCheckerBoardPlatformRadius;
    struct Object *parentObj;
    struct {
        struct {
            Vec3f scale;
        } gfx;
    } header;
} parent, children[2], spindel, *o;
static int spawned;
static const int bhvCheckerboardPlatformSub = 1;
enum {
    MODEL_CHECKERBOARD_PLATFORM = 1,
    SOUND_ENV_ELEVATOR4,
    SOUND_GENERAL2_SPINDEL_ROLL,
    SHAKE_POS_SMALL
};
#define GET_BPARAM1(v) (((v) >> 24) & 255)
#define abss abs
#define absi abs
#define PUPPYPRINT_ADD_COUNTER(v) ((void) 0)
#define MTXF_END(m) ((m)[0][3] = 0, (m)[1][3] = 0, (m)[2][3] = 0, (m)[3][3] = 1)
static float sins(s16 angle) {
    return sinf(angle * (6.283185307179586f / 65536));
}
static float coss(s16 angle) {
    return cosf(angle * (6.283185307179586f / 65536));
}
static float absf(float value) {
    return fabsf(value);
}
static void vec3f_copy(float *dst, const float *src) {
    memcpy(dst, src, sizeof(Vec3f));
}
static void cur_obj_play_sound_1(int sound) {
}
static void cur_obj_play_sound_2(int sound) {
}
static void set_camera_shake_from_point(int type, float x, float y, float z) {
}
static void load_object_collision_model(void) {
}
static void cur_obj_move_using_fvel_and_gravity(void) {
    // These fixtures have zero gravity; the engine integrates free velocity.
    o->oPosX += o->oForwardVel * sins(o->oMoveAngleYaw);
    o->oPosZ += o->oForwardVel * coss(o->oMoveAngleYaw);
    o->oPosY += o->oVelY;
}
static struct Object *spawn_object_relative(int param, int x, int y, int z, struct Object *p, int model,
                                            int behavior) {
    assert(spawned < 2 && model == MODEL_CHECKERBOARD_PLATFORM
           && behavior == bhvCheckerboardPlatformSub);
    struct Object *child = &children[spawned++];
    *child =
        (struct Object){ .oBehParams2ndByte = param,
                         .parentObj = p,
                         .oMoveAngleYaw = p->oMoveAngleYaw,
                         .oPosX = p->oPosX + x * coss(p->oMoveAngleYaw) + z * sins(p->oMoveAngleYaw),
                         .oPosY = p->oPosY + y,
                         .oPosZ = p->oPosZ + z * coss(p->oMoveAngleYaw) - x * sins(p->oMoveAngleYaw) };
    return child;
}

#include "lab_native_actual.inc.c"
#include "lab_native_vertices.inc.c"

static float bounds[3][2][3];
static void observe(int id, const float vertices[][3], int count, int pitch, int yaw) {
    Mat4 transform;
    Vec3f position = { o->oPosX, o->oPosY, o->oPosZ };
    Vec3s rotation = { pitch, yaw, 0 };
    mtxf_rotate_zxy_and_translate(transform, position, rotation);
    for (int v = 0; v < count; v++) {
        for (int axis = 0; axis < 3; axis++) {
            float value = position[axis];
            for (int local = 0; local < 3; local++) {
                value += vertices[v][local] * o->header.gfx.scale[local] * transform[local][axis];
            }
            bounds[id][0][axis] = fminf(bounds[id][0][axis], value);
            bounds[id][1][axis] = fmaxf(bounds[id][1][axis], value);
        }
    }
}

int main(int argc, char **argv) {
    assert(argc == 9);
    parent = (struct Object){ .oPosX = atof(argv[1]),
                              .oPosY = atof(argv[2]),
                              .oPosZ = atof(argv[3]),
                              .oMoveAngleYaw = atoi(argv[4]) * 65536 / 360,
                              .oBehParams = strtoul(argv[5], NULL, 0) };
    parent.oBehParams2ndByte = (parent.oBehParams >> 16) & 255;
    o = &parent;
    bhv_checkerboard_elevator_group_init();
    for (int i = 0; i < 2; i++) {
        o = &children[i];
        bhv_checkerboard_platform_init();
    }
    spindel = (struct Object){ .oPosX = atof(argv[6]),
                               .oPosY = atof(argv[7]),
                               .oPosZ = atof(argv[8]),
                               .header.gfx.scale = { 1, 1, 1 } };
    o = &spindel;
    bhv_spindel_init();
    for (int id = 0; id < 3; id++) {
        for (int a = 0; a < 3; a++) {
            bounds[id][0][a] = INFINITY;
            bounds[id][1][a] = -INFINITY;
        }
    }
    for (int frame = 0; frame < 3000; frame++) {
        for (int id = 0; id < 2; id++) {
            o = &children[id];
            int action = o->oAction;
            bhv_checkerboard_platform_loop();
            observe(id, checker_vertices, sizeof(checker_vertices) / sizeof(*checker_vertices),
                    o->oFaceAnglePitch, o->oFaceAngleYaw);
            o->oTimer = (o->oAction == action) ? o->oTimer + 1 : 0;
        }
        o = &spindel;
        bhv_spindel_loop();
        observe(2, spindel_vertices, sizeof(spindel_vertices) / sizeof(*spindel_vertices),
                o->oMoveAnglePitch, 0);
        o->oTimer++;
    }
    for (int id = 0; id < 3; id++) {
        for (int side = 0; side < 2; side++) {
            for (int a = 0; a < 3; a++) {
                printf("%.4f ", bounds[id][side][a]);
            }
        }
        puts("");
    }
}
