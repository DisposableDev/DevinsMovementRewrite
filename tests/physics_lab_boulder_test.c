// Native boulder motion, hitbox, trig tables and floor/wall queries on the
// shipped rolling-lane mesh. Rendering, audio and particles are stubs; this
// checks sustained travel and containment, not emulator presentation.
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef float f32;
typedef int32_t s32;
typedef uint32_t u32;
typedef int16_t s16;
typedef int8_t s8;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int TerrainData;
typedef int SurfaceType;
typedef float Vec3f[3];
typedef int16_t Vec3s[3];
typedef int32_t Vec3i[3];
#define UNUSED
#define TRUE 1
#define FALSE 0
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define sqr(x) ((x) * (x))
#define absf fabsf
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define NEAR_ZERO __FLT_EPSILON__
#define FLT_IS_NONZERO(x) (absf(x) > NEAR_ZERO)
#define MAX_REFERENCED_WALLS 4
#define LEVEL_BOUNDARY_MAX 16384
#define CELL_SIZE 1024
#define NUM_CELLS 32
#define GET_CELL_COORD(p) ((((s32) (p) + LEVEL_BOUNDARY_MAX) / CELL_SIZE) & (NUM_CELLS - 1))
#define PUPPYPRINT_GET_SNAPSHOT() ((void) 0)
#define PUPPYPRINT_ADD_COUNTER(x) ((void) 0)
#define profiler_collision_update(x) ((void) 0)
struct Normal {
    f32 x, y, z;
};
struct Surface {
    TerrainData type;
    s16 flags, lowerY, upperY, room;
    Vec3s vertex1, vertex2, vertex3;
    struct Normal normal;
    f32 originOffset;
};
#define oPosVec location[0]
#define oPosX location[0]
#define oPosY location[1]
#define oPosZ location[2]
struct SurfaceNode {
    struct SurfaceNode *next;
    struct Surface *surface;
};
struct Object {
    Vec3f location, home;
    f32 oFriction, oGravity, oBuoyancy, oForwardVel, oVelY, oFloorHeight;
    f32 hitboxRadius, hitboxHeight, hurtboxRadius, hurtboxHeight, hitboxDownOffset;
    f32 oGraphYOffset, oSnowmansBottomScale;
    int activeFlags, oFlags, oInteractType, oDamageOrCoinValue, oHealth, oNumLootCoins;
    int oAction, oFaceAnglePitch;
    u32 oInteractStatus;
    s16 oMoveAngleYaw;
    struct Surface *oFloor;
    struct {
        struct {
            Vec3f scale;
        } gfx;
    } header;
};
struct MarioState {
    u32 flags;
};
#define oHomeVec home[0]

#include "lab_boulder_definitions.inc.c"

static struct SurfaceNode *gStaticSurfacePartition[NUM_CELLS][NUM_CELLS][NUM_SPATIAL_PARTITIONS];
static struct SurfaceNode *gDynamicSurfacePartition[NUM_CELLS][NUM_CELLS][NUM_SPATIAL_PARTITIONS];
static struct MarioState mario, *gMarioState = &mario;
static struct Object object, marioObject, *o = &object, *gMarioObject = &marioObject;
enum { LEVEL_PHYSICS_LAB = 32, LEVEL_BOB = 9 };
static s16 gCurrLevelNum;
static s16 gCollisionFlags;
static int gNumFindFloorMisses;
static struct Surface *sObjFloor;
static s8 sOrientObjWithFloor = TRUE;
enum {
    OBJ_FLAG_HITBOX_WAS_SET = 1,
    INTERACT_DAMAGE = 1,
    INT_STATUS_INTERACTED = 1,
    INT_STATUS_NONE = 0,
    SOUND_GENERAL_GRINDEL_ROLL = 1,
    SOUND_ENV_BOWLING_BALL_ROLL = 2
};
#define vec3f_set(a, x, y, z)                                                                          \
    do {                                                                                               \
        (a)[0] = (x);                                                                                  \
        (a)[1] = (y);                                                                                  \
        (a)[2] = (z);                                                                                  \
    } while (0)
#define vec3_diff(a, b, c)                                                                             \
    do {                                                                                               \
        for (int j_ = 0; j_ < 3; j_++)                                                                 \
            (a)[j_] = (b)[j_] - (c)[j_];                                                               \
    } while (0)
#define vec3_dot(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])
#define vec3f_normalize(a)                                                                             \
    do {                                                                                               \
        f32 mag_ = sqrtf(vec3_dot(a, a));                                                              \
        for (int j_ = 0; j_ < 3; j_++)                                                                 \
            (a)[j_] /= mag_;                                                                           \
    } while (0)
static void vec3f_cross(Vec3f a, Vec3f b, Vec3f c) {
    vec3f_set(a, b[1] * c[2] - b[2] * c[1], b[2] * c[0] - b[0] * c[2], b[0] * c[1] - b[1] * c[0]);
}
static void vec3f_copy(float *a, const float *b) {
    memcpy(a, b, sizeof(Vec3f));
}
static s32 find_water_level(s32 x, s32 z) {
    return -11000;
}
static void obj_orient_graph(struct Object *obj, f32 x, f32 y, f32 z) {
}
static void obj_splash(s32 water, s32 y) {
}
static void cur_obj_play_sound_1(int sound) {
}
static void cur_obj_play_sound_2(int sound) {
}
static void spawn_mist_particles(void) {
}
static void cur_obj_become_tangible(void) {
}
static void cur_obj_scale(f32 scale) {
    vec3f_set(o->header.gfx.scale, scale, scale, scale);
}

static struct Surface surfaces[600];
static struct SurfaceNode nodes[600 * NUM_CELLS * NUM_CELLS];
static int surfaceCount, nodeCount;
static struct {
    int x, z, partition;
} sCellsUsed[NUM_CELLS * NUM_CELLS * NUM_SPATIAL_PARTITIONS];
static unsigned sNumCellsUsed;
static int sClearAllCells, sWaterCellMapReady;
static u8 sWaterCellMayContainWater[NUM_CELLS][NUM_CELLS];
#define NORMAL_FLOOR_THRESHOLD 0.01f
#define NORMAL_CEIL_THRESHOLD (-0.01f)
static struct SurfaceNode *alloc_surface_node(u32 dynamic) {
    assert(nodeCount < (int) ARRAY_COUNT(nodes));
    return &nodes[nodeCount++];
}
#include "lab_boulder_actual.inc.c"
#include "lab_boulder_mesh.inc.c"
// Supply normalized triangles; production code inserts them into cells and
// preserves its own floor/wall ordering. The area loader is not simulated.
static struct Surface *triangle(Vec3s a, Vec3s b, Vec3s c, int dynamic, int type) {
    assert(surfaceCount < 600);
    struct Surface *s = &surfaces[surfaceCount++];
    Vec3f ab, ac, n;
    memcpy(s->vertex1, a, sizeof(Vec3s));
    memcpy(s->vertex2, b, sizeof(Vec3s));
    memcpy(s->vertex3, c, sizeof(Vec3s));
    vec3_diff(ab, b, a);
    vec3_diff(ac, c, a);
    vec3f_cross(n, ab, ac);
    vec3f_normalize(n);
    s->normal = (struct Normal){ n[0], n[1], n[2] };
    s->originOffset = -(n[0] * a[0] + n[1] * a[1] + n[2] * a[2]);
    s->lowerY = MIN(a[1], MIN(b[1], c[1])) - 5;
    s->upperY = MAX(a[1], MAX(b[1], c[1])) + 5;
    s->type = type;
    add_surface(s, dynamic);
    return s;
}

static void start_boulder(int level, float x, float y, float z, s16 yaw) {
    object = (struct Object){ .activeFlags = 1 };
    o = &object;
    gCurrLevelNum = level;
    vec3f_set(o->location, x, y, z);
    o->oMoveAngleYaw = yaw;
    bhv_big_boulder_init();
    bhv_big_boulder_loop();
}

static int inside_lane(void) {
    return o->activeFlags && o->oPosX >= 13700 && o->oPosX <= 14900 && o->oPosZ >= 11400
           && o->oPosZ <= 14300;
}

static void restore_flat_floor(void) {
    o->oFloorHeight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &o->oFloor);
    assert(o->oFloor && o->oFloor->normal.y > .999f);
}

static void check_unreported_corner_push(void) {
    const float targetX = 13733.604492f, targetZ = 12726.328125f;
    const float velocityX = -35.629959f, velocityZ = -18.179821f;
    start_boulder(LEVEL_PHYSICS_LAB, 14300, 0, 12850, 0);
    struct WallCollisionData wall = {
        .x = targetX,
        .y = 0,
        .z = targetZ,
        .offsetY = o->hitboxHeight / 2,
        .radius = o->hitboxRadius,
    };
    // The failing gate-corner contact pushes without an interactive wall.
    assert(find_wall_collisions(&wall) == 0);
    assert(wall.x > targetX && wall.z > targetZ);
    assert(!obj_find_wall(targetX, 0, targetZ, velocityX, velocityZ));
    assert(o->oPosX == wall.x && o->oPosZ == wall.z);
    float pushX = wall.x - targetX, pushZ = wall.z - targetZ;
    assert(pushX * sins(o->oMoveAngleYaw) + pushZ * coss(o->oMoveAngleYaw) > 0);

    // A sphere already travelling out must keep its direction after separation.
    s16 awayYaw = atan2s(-velocityZ, -velocityX);
    o->oMoveAngleYaw = awayYaw;
    assert(!obj_find_wall(targetX, 0, targetZ, -velocityX, -velocityZ));
    assert(o->oPosX == wall.x && o->oPosZ == wall.z);
    assert(o->oMoveAngleYaw == awayYaw);
}

int main(void) {
    for (unsigned i = 0; i < sizeof(mesh) / sizeof(*mesh); i++) {
        triangle(mesh[i][0], mesh[i][1], mesh[i][2], 0, SURFACE_DEFAULT);
    }
    check_unreported_corner_push();

    // Checking every later window catches a boulder that moves initially but
    // settles while Mario is elsewhere. Each run is 16+ gameplay minutes.
    for (int run = 0; run < 3; run++) {
        start_boulder(LEVEL_PHYSICS_LAB, spawn[0], spawn[1], spawn[2], spawnYaw);
        if (run) {
            restore_flat_floor();
            o->oForwardVel = 0;
            o->oMoveAngleYaw = run == 1 ? 0 : (s16) 0x8000;
        }
        float minZ = 1e8f, maxZ = -1e8f;
        for (int frame = 0; frame < 30000; frame++) {
            bhv_big_boulder_loop();
            assert(inside_lane());
            minZ = fminf(minZ, o->oPosZ);
            maxZ = fmaxf(maxZ, o->oPosZ);
            if (frame % 900 == 899) {
                if (minZ > 12000 || maxZ < 13700) {
                    fprintf(stderr,
                            "Boulder stopped traversing both ramps: run %d frame %d, Z %.3f..%.3f\n",
                            run, frame, minZ, maxZ);
                    return 1;
                }
                minZ = 1e8f;
                maxZ = -1e8f;
            }
        }
    }

    // Retain natural slowing/turning on slopes and ordinary behavior elsewhere.
    for (int lab = 0; lab < 2; lab++) {
        start_boulder(lab ? LEVEL_PHYSICS_LAB : LEVEL_BOB, 14300, 0, 12850, 0);
        restore_flat_floor();
        o->oForwardVel = 20;
        bhv_big_boulder_loop();
        assert(fabsf(o->oForwardVel - (lab ? 39.96f : 19.98f)) < .01f);
    }
    start_boulder(LEVEL_PHYSICS_LAB, 14300, 200, 12850, 0);
    restore_flat_floor();
    o->oForwardVel = 20;
    bhv_big_boulder_loop();
    assert(fabsf(o->oForwardVel - 20) < .01f); // no airborne boost
    start_boulder(LEVEL_PHYSICS_LAB, 14300, 150, 14000, 0);
    o->oFloorHeight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &o->oFloor);
    assert(o->oFloor && o->oFloor->normal.y < .999f);
    o->oForwardVel = 10;
    bhv_big_boulder_loop();
    assert(o->oForwardVel < 10); // still slows while climbing the end ramp
    start_boulder(LEVEL_PHYSICS_LAB, 14300, 0, 12850, 0);
    restore_flat_floor();
    o->oForwardVel = 60;
    bhv_big_boulder_loop();
    assert(fabsf(o->oForwardVel - 59.94f) < .01f); // faster travel is preserved

    // The boost must not defeat the perimeter or the narrow walk-in gate.
    for (int angle = 0; angle < 16; angle++) {
        start_boulder(LEVEL_PHYSICS_LAB, 14300, 0, 12850, (s16) (angle * 4096));
        restore_flat_floor();
        o->oForwardVel = 70;
        for (int frame = 0; frame < 9000; frame++) {
            bhv_big_boulder_loop();
            if (!inside_lane()) {
                fprintf(stderr,
                        "Boulder escaped: angle %d frame %d pos %.3f,%.3f,%.3f speed %.3f yaw %d\n",
                        angle, frame, o->oPosX, o->oPosY, o->oPosZ, o->oForwardVel, o->oMoveAngleYaw);
                return 1;
            }
        }
    }
    for (int direction = -1; direction <= 1; direction += 2) {
        for (int offset = 30; offset <= 150; offset += 120) {
            float x = direction == 1 ? 13400 : 14000;
            for (int frame = 0; frame < 30; frame++) {
                struct WallCollisionData wall = {
                    .x = x + direction * 20, .y = 0, .z = 12850, .offsetY = offset, .radius = 50
                };
                find_wall_collisions(&wall);
                x = wall.x;
                assert(fabsf(wall.z - 12850) < .01f);
            }
            assert(fabsf(x - (direction == 1 ? 14000 : 13400)) < .01f);
        }
    }
    puts("Boulder kept traversing both ramps through 30000-frame runs, recovered from rest, and stayed "
         "contained; slope, air, other-course and entrance controls passed.");
    return 0;
}
