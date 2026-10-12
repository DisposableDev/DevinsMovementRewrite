/* Production carry, inertia, policies, attachment and update ordering. Floor
 * results and contact correction are supplied services; finite terrain sweeps
 * are checked by swimming.platform_collision. Native AI and rendering are not simulated. */
#include <stdint.h>
#include <math.h>
#include "test_support.h"
typedef float f32;
typedef int32_t s32;
typedef uint32_t u32;
typedef int16_t s16;
typedef s16 TerrainData;
typedef uint16_t u16;
typedef uint8_t u8;
typedef uintptr_t BehaviorScript;
typedef float Vec3f[3];
typedef int16_t Vec3s[3];
typedef float Mat4[4][4];
#define FLOOMBAS
#define TRUE 1
#define FALSE 0
#define absf fabsf
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define sqr(x) ((x) * (x))
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define bzero(p, n) memset(p, 0, n)
#define PUPPYPRINT_ADD_COUNTER(x) ((void) 0)
#define MTXF_END(m)                                                                                    \
    do {                                                                                               \
        (m)[0][3] = (m)[1][3] = (m)[2][3] = 0;                                                         \
        (m)[3][3] = 1;                                                                                 \
    } while (0)
struct Object;
#include "platform_definitions.inc"
struct ObjectNode {
    struct ObjectNode *next;
};
struct Surface {
    struct Object *object;
    struct {
        f32 x, y, z;
    } normal;
};
struct Object {
    struct ObjectNode node;
    struct {
        struct {
            Mat4 *throwMatrix;
            Vec3f scale, pos;
            struct {
                u32 flags;
            } node;
        } gfx;
    } header;
    Vec3f location, home;
    const BehaviorScript *behavior;
    void *collisionData;
    Mat4 transform;
    struct Object *parentObj, *platform, *oMontyMoleCurrentHole;
    struct ChainSegment *oWigglerSegments, *oChainChompSegments;
    s32 oTimer, oPrevAction, oSubAction, oInteractStatus, numCollidedObjs;
    f32 oDrawingDistance, oDistanceToMario;
    s32 activeFlags, oFlags, oHeldState, oAction, oMoveAngleYaw, oFaceAngleYaw, oBehParams2ndByte;
    s32 oFaceAnglePitch, oFaceAngleRoll;
    s32 oGroundTwirlStunTimer, oGroundTwirlContactRebound, oChainChompReleaseStatus,
        oChainChompRestrictedByChain;
    f32 oVelX, oVelY, oVelZ, oGravity, oFriction, oForwardVel, hitboxRadius, oCollisionDistance,
        oGraphYOffset;
    f32 oWoodenPostOffsetY, oChainChompMaxDistFromPivotPerChainPart, oWallHitboxRadius;
    u32 oMoveFlags;
    struct Surface floor;
    f32 floorHeight;
    s32 hasFloor, updates;
};
#define oVelVec oVelX
#define o gCurrentObject
#define NEAR_ZERO __FLT_EPSILON__
#define oPosVec location[0]
#define oPosX location[0]
#define oPosY location[1]
#define oPosZ location[2]
#define oHomeVec home[0]
#define oHomeY home[1]
static struct Object gObjectPool[OBJECT_POOL_CAPACITY];
static u32 gGlobalTimer, gTimeStopState;
static struct Object *gCurrentObject;
static struct MarioState {
    Vec3f pos;
} mario;
static struct MarioState *gMarioState;
static Vec3f sMarioAmountDisplaced;
static f32 clipX;
static s32 updateOrder[OBJECT_POOL_CAPACITY], updateCount;
static s32 behaviorChecks;
static s32 floorQueries, dynamicFloorsPresent, platformMotionQueries;
static s32 obj_has_behavior(struct Object *obj, const BehaviorScript *behavior) {
    behaviorChecks++;
    return obj->behavior == behavior;
}
static f32 find_floor_excluding_objects(f32 x, f32 y, f32 z, struct Surface **floor,
                                        struct Object *owner, struct Object *attachment) {
    floorQueries++;
    *floor = owner->hasFloor && owner->floor.object != owner
                     && (attachment == NULL || owner->floor.object != attachment)
                 ? &owner->floor
                 : NULL;
    return *floor != NULL ? owner->floorHeight : -11000;
}
static s32 surface_query_cell_is_empty(s32 cellX, s32 cellZ, s32 partition,
                                       enum SurfaceQueryStorage storage) {
    return !dynamicFloorsPresent;
}
static u32 object_resolve_platform_motion(struct Object *obj, Vec3f start, Vec3f target,
                                          struct Object *support) {
    platformMotionQueries++;
    if (target[0] > clipX) {
        target[0] = clipX;
    }
    return 0;
}
static void obj_build_transform_from_pos_and_angle(struct Object *obj, s32 posIndex, s32 angleIndex);
#define O_POS_INDEX 0
#define O_FACE_ANGLE_INDEX 0
static void load_object_collision_model(void) {
    object_platform_motion_publish(gCurrentObject);
    dynamicFloorsPresent = TRUE;
}
static s32 physics_lab_discard_distant_effect(struct Object *obj) {
    (void) obj;
    return FALSE;
}
// Visibility policy is a separate service; these fixtures keep objects visible.
static s32 physics_lab_update_enemy_hidden_state(struct Object *obj) {
    (void) obj;
    return FALSE;
}
static s32 sleepingVisibilityUpdates;
static void cur_obj_update_sleeping_visibility(void) {
    sleepingVisibilityUpdates++;
}
static s32 bbh_merry_go_round_update_dormancy(struct Object *obj) {
    (void) obj;
    return FALSE;
}
static s32 sleepRequested;
static const BehaviorScript sNonRiderBehavior[] = { 0 };
static struct Object *changeBehaviorObject;
static const BehaviorScript *changeBehaviorTo;
static s32 enemy_should_sleep(struct Object *obj) {
    (void) obj;
    return sleepRequested;
}
#define profiler_count_object(sleeping)

static void update_object_with_ground_twirl_stun(void) {
    struct Object *obj = gCurrentObject;
    obj->updates++;
    if (obj == changeBehaviorObject) {
        obj->behavior = changeBehaviorTo;
        changeBehaviorObject = NULL;
    }
    updateOrder[updateCount++] = obj - gObjectPool;
    if (obj->collisionData != NULL) {
        object_platform_motion_publish(obj);
    }
    if (obj->oGroundTwirlStunTimer > 0) {
        obj->header.gfx.node.flags &= ~GRAPH_RENDER_HAS_ANIMATION;
    }
}
#define LEVEL_PHYSICS_LAB 32
#define INT_STATUS_NONE 0
#define COND_BIT(c, f, b)                                                                              \
    do {                                                                                               \
        if (c) {                                                                                       \
            (f) |= (b);                                                                                \
        } else {                                                                                       \
            (f) &= ~(b);                                                                               \
        }                                                                                              \
    } while (0)
static struct Object *gMarioObject;
static s32 gCurrLevelNum, boxActionCalls, sExclamationBoxActions[6];
static void cur_obj_scale(f32 scale) {
    for (int i = 0; i < 3; i++) {
        o->header.gfx.scale[i] = scale;
    }
}
static void cur_obj_call_action_function(s32 *actions) {
    boxActionCalls++;
    load_object_collision_model(); // The active box's collision-bearing service.
}
#include "platform_sources.inc"
static void obj_build_transform_from_pos_and_angle(struct Object *obj, s32 posIndex, s32 angleIndex) {
    Vec3s angle = { 0, obj->oFaceAngleYaw, 0 };
    mtxf_rotate_zxy_and_translate(obj->transform, obj->location, angle);
}
#define NEAR(a, b)                                                                                     \
    do {                                                                                               \
        if (!(fabsf((a) - (b)) <= .04f)) {                                                             \
            fprintf(stderr, "%s:%d: expected %.4f, got %.4f\n", sCurrentCase, __LINE__, (double) (b),  \
                    (double) (a));                                                                     \
            return FALSE;                                                                              \
        }                                                                                              \
    } while (0)
static struct Object *spawn(s32 slot, const BehaviorScript *behavior, f32 x, f32 y, f32 z) {
    struct Object *obj = &gObjectPool[slot];
    memset(obj, 0, sizeof(*obj));
    obj->activeFlags = ACTIVE_FLAG_ACTIVE;
    obj->parentObj = obj;
    obj->behavior = behavior;
    obj->oPosX = x;
    obj->oPosY = y;
    obj->oPosZ = z;
    obj->oForwardVel = 7;
    for (s32 axis = 0; axis < 3; axis++) {
        obj->header.gfx.scale[axis] = 1;
    }
    object_platform_motion_reset(obj);
    object_platform_motion_allocated(obj);
    return obj;
}
static void reset(void) {
    memset(gObjectPool, 0, sizeof(gObjectPool));
    object_platform_motion_init();
    gGlobalTimer = 10;
    gTimeStopState = 0;
    clipX = 100000;
    updateCount = 0;
    sleepRequested = FALSE;
    changeBehaviorObject = NULL;
    floorQueries = 0;
    platformMotionQueries = 0;
    dynamicFloorsPresent = TRUE;
    gMarioState = &mario;
}
static void carrier_pose(struct Object *platform, f32 x, f32 y, f32 z, s16 yaw) {
    Vec3f position = { x, y, z };
    Vec3s rotation = { 0, yaw, 0 };
    mtxf_rotate_zxy_and_translate(platform->transform, position, rotation);
    platform->header.gfx.throwMatrix = &platform->transform;
    platform->oPosX = x;
    platform->oPosY = y;
    platform->oPosZ = z;
    platform->collisionData = platform;
    platform->oCollisionDistance = 1000;
    object_platform_motion_publish(platform);
}
static void support(struct Object *obj, struct Object *platform, f32 height) {
    obj->hasFloor = TRUE;
    obj->floor.object = platform;
    obj->floorHeight = height;
    object_platform_motion_finish(obj);
}
static void advance(struct Object *platform, f32 x, f32 y, f32 z, s16 yaw) {
    gGlobalTimer++;
    object_platform_motion_begin_frame();
    carrier_pose(platform, x, y, z, yaw);
}
static int inactive_boss_and_home_reset(void) {
    reset();
    struct Object *boss = spawn(0, bhvKingBobomb, 0, 0, 0);
    struct Object *platform = spawn(1, sNonRiderBehavior, 0, 0, 0);
    carrier_pose(platform, 0, 0, 0, 0);
    object_platform_motion_begin_frame();
    EXPECT_EQ(sPlatformRiderCount, 0);
    EXPECT_EQ(object_platform_motion_needs_collision(platform), FALSE);

    // The inactive boss holds home every update. That resets motion, but
    // must not repeat the behavior search or lose slot-generation protection.
    behaviorChecks = 0;
    for (s32 frame = 0; frame < 30; frame++) {
        u32 generation = sEnemyPlatformMotion[0].generation;
        object_platform_motion_reset(boss);
        gGlobalTimer++;
        object_platform_motion_begin_frame();
        EXPECT_EQ(sEnemyPlatformMotion[0].generation, generation + 1);
        EXPECT_EQ(sPlatformRiderCount, 0);
    }
    EXPECT_EQ(behaviorChecks, 0);

    // Dialogue leaves the home-hold phase, so the same boss can acquire
    // support immediately and retain ordinary platform carry in combat.
    boss->oSubAction = KING_BOBOMB_SUB_ACT_INACTIVE_DIALOG;
    carrier_pose(platform, 0, 0, 0, 0);
    support(boss, platform, 0);
    advance(platform, 10, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(boss->oPosX, 10);
    boss->oAction = KING_BOBOMB_ACT_ACTIVE;
    support(boss, platform, 0);
    advance(platform, 20, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(boss->oPosX, 20);
    return TRUE;
}

static int static_terrain_work(void) {
    reset();
    struct Object *enemy = spawn(0, bhvGoomba, 0, 0, 0);
    struct Object *platform = spawn(1, sNonRiderBehavior, 0, 0, 0);
    dynamicFloorsPresent = FALSE;
    for (s32 frame = 0; frame < 30; frame++) {
        gGlobalTimer++;
        object_platform_motion_begin_frame();
        gCurrentObject = enemy;
        update_object_with_platform_motion();
        EXPECT_EQ(enemy->updates, frame + 1);
    }
    EXPECT_EQ(floorQueries, 0);

    // A newly published floor must allow support in the same update.
    dynamicFloorsPresent = TRUE;
    carrier_pose(platform, 0, 0, 0, 0);
    support(enemy, platform, 0);
    EXPECT_EQ(floorQueries, 1);
    EXPECT_EQ(object_platform_motion_provider(enemy), platform);
    advance(platform, 10, 5, 0, 0);
    object_platform_motion_apply_all();
    dynamicFloorsPresent = FALSE;
    enemy->hasFloor = FALSE;
    object_platform_motion_finish(enemy);
    EXPECT_EQ(floorQueries, 2);
    EXPECT_EQ(sEnemyPlatformMotion[0].departing, TRUE);
    enemy->oVelY = 0;
    object_platform_motion_before_step(enemy);
    NEAR(enemy->oVelY, 5);

    // Static landing must clear the departure even with no dynamic floors.
    enemy->oVelY = 0;
    support(enemy, NULL, enemy->oPosY);
    EXPECT_EQ(floorQueries, 3);
    EXPECT_EQ(sEnemyPlatformMotion[0].departing, FALSE);
    NEAR(sEnemyPlatformMotion[0].velocity[0], 0);
    object_platform_motion_finish(enemy);
    EXPECT_EQ(floorQueries, 3);

    // A stunned Whomp publishes before checking the current cell's presence.
    struct Object *whomp = spawn(2, bhvSmallWhomp, 0, 0, 0);
    whomp->collisionData = whomp;
    whomp->oGroundTwirlStunTimer = 10;
    gCurrentObject = whomp;
    object_platform_motion_finish(whomp);
    EXPECT_EQ(sPlatformPublished[2], TRUE);
    EXPECT_EQ(floorQueries, 4);
    return TRUE;
}

static int unrelated_object_work(void) {
    reset();
    for (s32 i = 0; i < 100; i++) {
        spawn(i, sNonRiderBehavior, 0, 0, 0);
    }
    object_platform_motion_begin_frame();
    for (s32 i = 0; i < 100; i++) {
        gCurrentObject = &gObjectPool[i];
        update_object_with_platform_motion();
    }
    s32 historyChanges = 0;
    for (s32 frame = 0; frame < 30; frame++) {
        gGlobalTimer++;
        updateCount = 0;
        object_platform_motion_begin_frame();
        for (s32 i = 0; i < 100; i++) {
            struct EnemyPlatformMotion before = sEnemyPlatformMotion[i];
            gCurrentObject = &gObjectPool[i];
            object_platform_motion_before_step(gCurrentObject);
            update_object_with_platform_motion();
            historyChanges += memcmp(&before, &sEnemyPlatformMotion[i], sizeof(before)) != 0;
            EXPECT_EQ(gCurrentObject->updates, frame + 2);
        }
        EXPECT_EQ(updateCount, 100);
    }
    printf("WORK non-rider motion history: %d changed histories across 3000 native updates\n",
           historyChanges);
    EXPECT_EQ(historyChanges, 0);

    // Native scripts can turn an unrelated object into a rider in this update.
    struct Object *obj = spawn(110, sNonRiderBehavior, 0, 0, 0);
    struct Object *platform = spawn(111, sNonRiderBehavior, 0, 0, 0);
    carrier_pose(platform, 0, 0, 0, 0);
    gGlobalTimer++;
    object_platform_motion_begin_frame();
    carrier_pose(platform, 0, 0, 0, 0);
    obj->hasFloor = TRUE;
    obj->floor.object = platform;
    changeBehaviorObject = obj;
    changeBehaviorTo = bhvGoomba;
    gCurrentObject = obj;
    update_object_with_platform_motion();
    EXPECT_EQ(object_platform_motion_provider(obj), platform);
    advance(platform, 10, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(obj->oPosX, 10);
    changeBehaviorObject = obj;
    changeBehaviorTo = sNonRiderBehavior;
    gCurrentObject = obj;
    update_object_with_platform_motion();
    EXPECT_EQ(object_platform_motion_provider(obj), NULL);
    advance(platform, 20, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(obj->oPosX, 10);
    return TRUE;
}
static int rendered_floor_history(void) {
    for (s32 speed = 200; speed <= 600; speed += 400) {
        for (s32 direction = -1; direction <= 1; direction += 2) {
            reset();
            struct Object *p = spawn(0, sNonRiderBehavior, 0, 0, 0);
            carrier_pose(p, 0, 0, 0, 0);
            for (s32 frame = 1; frame <= 12; frame++) {
                Mat4 oldTransform;
                mtxf_copy(oldTransform, p->transform);
                // Normal object rendering clears this temporary pointer.
                p->header.gfx.throwMatrix = NULL;
                gGlobalTimer++;
                object_platform_motion_begin_frame();
                EXPECT_EQ(sPlatformPoseValid[0], TRUE);
                EXPECT_EQ(p->header.gfx.throwMatrix == NULL, TRUE);
                Vec3f position = { 0, 0, 0 };
                Vec3s rotation = { direction * speed * frame, 0, 0 };
                mtxf_rotate_zxy_and_translate(p->transform, position, rotation);
                p->header.gfx.throwMatrix = &p->transform;
                object_platform_motion_publish(p);
                Vec3f normal = { p->transform[1][0], p->transform[1][1], p->transform[1][2] };
                // The original spinner's broad top wings have local Y=26.
                for (s32 z = -200; z <= 200; z += 400) {
                    f32 height = (26 - z * normal[2]) / normal[1];
                    f32 previousHeight = (26 - z * oldTransform[1][2]) / oldTransform[1][1];
                    Vec3f point = { 0, height, z };
                    f32 rise = object_platform_motion_floor_rise_at(p, point, normal);
                    NEAR(rise, height > previousHeight ? height - previousHeight + 1 : 0);
                }
            }
            // A frame without published collision still invalidates history.
            p->header.gfx.throwMatrix = NULL;
            object_platform_motion_begin_frame();
            object_platform_motion_begin_frame();
            EXPECT_EQ(sPlatformPoseValid[0], FALSE);
        }
    }
    return TRUE;
}

static int floor_rise(void) {
    reset();
    struct Object *p = spawn(0, NULL, 2867, -1279, 307);
    carrier_pose(p, 2867, -1279, 307, 0);
    NEAR(object_platform_motion_floor_rise(p), 0);
    for (int frame = 1; frame <= 12; frame++) {
        advance(p, 2867, -1279 + 10 * frame, 307, 0);
        NEAR(object_platform_motion_floor_rise(p), 10);
    }
    const f32 changes[][4] = { { 0, 0, 0, 0 },  { 0, -10, 0, 0 },    { 1, 10, 0, 0 },
                               { 0, 10, 1, 0 }, { 0, 10, 0, 0x100 }, { 0, 79, 0, 0 } };
    for (unsigned i = 0; i < sizeof(changes) / sizeof(changes[0]); i++) {
        carrier_pose(p, 2867, -1279, 307, 0);
        advance(p, 2867 + changes[i][0], -1279 + changes[i][1], 307 + changes[i][2],
                (s16) changes[i][3]);
        NEAR(object_platform_motion_floor_rise(p), 0);
    }
    carrier_pose(p, 2867, -1279, 307, 0);
    advance(p, 2867, -1201, 307, 0);
    NEAR(object_platform_motion_floor_rise(p), 78);
    p->header.gfx.scale[0] = 2;
    NEAR(object_platform_motion_floor_rise(p), 0);
    p->header.gfx.scale[0] = 1;
    object_platform_motion_begin_frame();
    NEAR(object_platform_motion_floor_rise(p), 0);
    carrier_pose(p, 2867, -1191, 307, 0);
    NEAR(object_platform_motion_floor_rise(p), 10);
    object_platform_motion_reset(p);
    NEAR(object_platform_motion_floor_rise(p), 0);
    carrier_pose(p, 2867, -1191, 307, 0);
    gTimeStopState = TIME_STOP_ACTIVE | TIME_STOP_ALL_OBJECTS;
    object_platform_motion_begin_frame();
    NEAR(object_platform_motion_floor_rise(p), 0);
    p->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    NEAR(object_platform_motion_floor_rise(p), 0);
    struct Object foreign = { 0 };
    NEAR(object_platform_motion_floor_rise(&foreign), 0);
    NEAR(object_platform_motion_floor_rise(NULL), 0);
    return TRUE;
}
static int tilting_floor_rise(void) {
    Vec3f position = { 0, -1000, 0 };
    for (int pitch = -390; pitch <= 390; pitch += 195) {
        for (int roll = -390; roll <= 390; roll += 195) {
            reset();
            struct Object *p = spawn(0, NULL, 0, -1000, 0);
            carrier_pose(p, 0, -1000, 0, 0);
            gGlobalTimer++;
            object_platform_motion_begin_frame();
            Vec3s rotation = { pitch, 0, roll };
            mtxf_rotate_zxy_and_translate(p->transform, position, rotation);
            object_platform_motion_publish(p);
            Vec3f normal = { p->transform[1][0], p->transform[1][1], p->transform[1][2] };
            for (int x = -2400; x <= 2400; x += 1200) {
                for (int z = -2400; z <= 2400; z += 1200) {
                    f32 height = -1000 - (x * normal[0] + z * normal[2]) / normal[1];
                    Vec3f point = { x, height, z };
                    f32 rise = object_platform_motion_floor_rise_at(p, point, normal);
                    NEAR(rise, height > -1000 ? height + 1001 : 0);
                    NEAR(object_platform_motion_floor_rise(p), 0);
                }
            }
            Vec3f point = { 2400, -1000, 2400 };
            p->header.gfx.scale[0] = 2;
            NEAR(object_platform_motion_floor_rise_at(p, point, normal), 0);
            p->header.gfx.scale[0] = 1;
            object_platform_motion_begin_frame();
            NEAR(object_platform_motion_floor_rise_at(p, point, normal), 0);
            object_platform_motion_publish(p);
            NEAR(object_platform_motion_floor_rise_at(p, point, normal), 0);
            object_platform_motion_reset(p);
            NEAR(object_platform_motion_floor_rise_at(p, point, normal), 0);
        }
    }
    reset();
    struct Object *p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    Vec3f point = { 0, 10, 0 }, normal = { 0, 1, 0 };
    advance(p, 0, 10, 0, 0);
    NEAR(object_platform_motion_floor_rise_at(p, point, normal), 10);
    advance(p, 0, 89, 0, 0);
    point[1] = 89;
    NEAR(object_platform_motion_floor_rise_at(p, point, normal), 0);
    advance(p, 0, 90, 0, 0x4000);
    point[1] = 90;
    NEAR(object_platform_motion_floor_rise_at(p, point, normal), 0);
    struct Object foreign = { 0 };
    NEAR(object_platform_motion_floor_rise_at(&foreign, point, normal), 0);
    NEAR(object_platform_motion_floor_rise_at(NULL, point, normal), 0);
    // A returning tilt republishes a flat top. Its exact reconstructed old
    // plane is nearly a unit above the previous integer collision triangle.
    reset();
    p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    Vec3f origin = { 0, 0, 0 };
    Vec3s oldAngle = { -80, 0, 0 }, published[3];
    mtxf_rotate_zxy_and_translate(p->transform, origin, oldAngle);
    for (int vertex = 0; vertex < 3; vertex++) {
        linear_mtxf_mul_vec3_and_translate(p->transform, published[vertex],
                                           bowserPlaneVertices[vertex]);
    }
    Vec3f a, b, n;
    vec3f_diff(a, published[1], published[0]);
    vec3f_diff(b, published[2], published[0]);
    n[0] = a[1] * b[2] - a[2] * b[1];
    n[1] = a[2] * b[0] - a[0] * b[2];
    n[2] = a[0] * b[1] - a[1] * b[0];
    vec3f_set(point, -2000, 1229, -1000);
    f32 oldHeight =
        published[0][1]
        - ((point[0] - published[0][0]) * n[0] + (point[2] - published[0][2]) * n[2]) / n[1];
    advance(p, 0, 0, 0, 0);
    f32 rise = object_platform_motion_floor_rise_at(p, point, normal);
    EXPECT_EQ(oldHeight + 0.01f >= point[1] - rise, TRUE);
    EXPECT_EQ(oldHeight - 2.0f < point[1] - rise, TRUE);
    return TRUE;
}
static int translation(void) {
    reset();
    struct Object *p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    struct Object *a = spawn(1, bhvGoomba, 40, 0, 30), *b = spawn(2, bhvBobomb, -40, 0, -30);
    support(a, p, 0);
    support(b, p, 0);
    for (int cycle = 0; cycle < 4; cycle++) {
        advance(p, 10 * (cycle + 1), 3 * (cycle + 1), -4 * (cycle + 1), 0);
        object_platform_motion_apply_all();
        object_platform_motion_apply_all();
        NEAR(a->oPosX, 40 + 10 * (cycle + 1));
        NEAR(b->oPosX, -40 + 10 * (cycle + 1));
        NEAR(a->oPosY, 3 * (cycle + 1));
        NEAR(a->oPosZ, 30 - 4 * (cycle + 1));
        NEAR(a->oForwardVel, 7);
        NEAR(b->oForwardVel, 7);
        support(a, p, a->oPosY);
        support(b, p, b->oPosY);
    }
    return TRUE;
}
static int rotation(void) {
    reset();
    struct Object *p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    struct Object *a = spawn(1, bhvGoomba, 100, 0, 0);
    a->oFaceAngleYaw = 0x1000;
    support(a, p, 0);
    advance(p, 20, 0, 0, 0x4000);
    object_platform_motion_apply_all();
    NEAR(a->oPosX, 20);
    NEAR(a->oPosZ, -100);
    EXPECT_EQ((s16) a->oMoveAngleYaw, 0x4000);
    EXPECT_EQ((s16) a->oFaceAngleYaw, 0x5000);
    return TRUE;
}
static int departure(void) {
    reset();
    struct Object *p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    struct Object *a = spawn(1, bhvGoomba, 0, 0, 0);
    support(a, p, 0);
    advance(p, 10, 5, 0, 0);
    object_platform_motion_apply_all();
    a->oVelY = 20;
    object_platform_motion_before_step(a);
    NEAR(a->oVelY, 25);
    object_platform_motion_before_step(a);
    NEAR(a->oVelY, 25);
    a->oPosY += 25;
    a->hasFloor = FALSE;
    object_platform_motion_finish(a);
    f32 x = a->oPosX, velocity = 10;
    for (s32 i = 0; i < 4; i++) {
        advance(p, 20 + i * 10, 10 + i * 5, 0, 0);
        object_platform_motion_apply_all();
        x += velocity;
        velocity *= .97f;
        NEAR(a->oPosX, x);
        NEAR(a->oVelY, 25);
        object_platform_motion_before_step(a);
        NEAR(a->oVelY, 25);
        object_platform_motion_finish(a);
    }
    a->oVelY = 0;
    support(a, NULL, a->oPosY);
    advance(p, 90, 25, 0, 0);
    object_platform_motion_apply_all();
    NEAR(a->oPosX, x);
    return TRUE;
}
static int loss_and_downward(void) {
    for (s32 direction = -1; direction <= 1; direction += 2) {
        reset();
        struct Object *p = spawn(0, NULL, 0, 0, 0);
        carrier_pose(p, 0, 0, 0, 0);
        struct Object *a = spawn(1, bhvSmallBully, 0, 0, 0);
        support(a, p, 0);
        advance(p, 12, 6 * direction, 0, 0);
        object_platform_motion_apply_all();
        a->hasFloor = FALSE;
        object_platform_motion_finish(a);
        advance(p, 24, 12 * direction, 0, 0);
        object_platform_motion_apply_all();
        a->oVelY = 0;
        object_platform_motion_before_step(a);
        NEAR(a->oVelY, MAX(0, 6 * direction));
        object_platform_motion_before_step(a);
        NEAR(a->oVelY, MAX(0, 6 * direction));
        NEAR(a->oPosX, 24);
    }
    return TRUE;
}
static int conveyor_and_clip(void) {
    reset();
    struct Object *p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    p->oFlags = OBJ_FLAG_NO_AUTO_DISPLACEMENT;
    p->oVelX = 10;
    struct Object *a = spawn(1, bhvGoomba, 0, 0, 0);
    support(a, p, 0);
    advance(p, 0, 0, 0, 0);
    clipX = 4;
    object_platform_motion_apply_all();
    NEAR(a->oPosX, 4);
    a->hasFloor = FALSE;
    object_platform_motion_finish(a);
    clipX = 10000;
    advance(p, 0, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(a->oPosX, 4);
    return TRUE;
}
static int lifecycle(void) {
    reset();
    struct Object *p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    struct Object *a = spawn(1, bhvGoomba, 0, 0, 0);
    support(a, p, 0);
    advance(p, 10, 0, 0, 0);
    object_platform_motion_apply_all();
    support(a, p, 0);
    object_platform_motion_reset(p);
    advance(p, 1000, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(a->oPosX, 10);
    support(a, p, 0);
    gGlobalTimer += 10;
    object_platform_motion_begin_frame();
    carrier_pose(p, 2000, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(a->oPosX, 10);
    support(a, p, 0);
    a->oFlags = OBJ_FLAG_HOLDABLE;
    a->oHeldState = HELD_HELD;
    advance(p, 3000, 0, 0, 0);
    object_platform_motion_apply(a);
    NEAR(a->oPosX, 10);
    // Free a carrier, reuse its slot as an enemy, and attach another rider.
    // Allocation order differs from pool order; reset must retain live slots.
    object_platform_motion_reset(p);
    p->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    object_platform_motion_freed(p);
    object_platform_motion_begin_frame();
    EXPECT_EQ(object_platform_motion_provider(a), NULL);
    struct Object *next = spawn(5, NULL, 0, 0, 0);
    carrier_pose(next, 0, 0, 0, 0);
    p = spawn(0, bhvGoomba, 0, 0, 0);
    support(p, next, 0);
    advance(next, 10, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(p->oPosX, 10);
    object_platform_motion_reset(p);
    support(p, next, 0);
    advance(next, 20, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(p->oPosX, 20);
    return TRUE;
}
static int freeze(void) {
    reset();
    struct Object *p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    struct Object *a = spawn(1, bhvGoomba, 0, 0, 0);
    support(a, p, 0);
    gTimeStopState = TIME_STOP_ACTIVE | TIME_STOP_ALL_OBJECTS;
    advance(p, 10, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(a->oPosX, 0);
    gTimeStopState = 0;
    advance(p, 20, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(a->oPosX, 0);
    support(a, p, 0);
    a->activeFlags |= ACTIVE_FLAG_FAR_AWAY;
    advance(p, 30, 0, 0, 0);
    object_platform_motion_apply(a);
    NEAR(a->oPosX, 0);
    a->activeFlags = ACTIVE_FLAG_ACTIVE;
    support(a, p, 0);
    advance(p, 40, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(a->oPosX, 10);
    return TRUE;
}
static int policies(void) {
    reset();
    struct Object *p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    struct Object *fly = spawn(1, bhvSnufit, 0, 0, 0), *coin = spawn(2, NULL, 0, 0, 0),
                  *boss = spawn(3, bhvBowser, 0, 0, 0);
    coin->oGroundTwirlStunTimer = 50;
    support(fly, p, 0);
    support(coin, p, 0);
    support(boss, p, 0);
    EXPECT_EQ(boss->platform, p);
    advance(p, 10, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(fly->oPosX, 0);
    NEAR(coin->oPosX, 0);
    NEAR(boss->oPosX, 10);
    fly->oGroundTwirlStunTimer = 20;
    support(fly, p, 0);
    boss->oAction = BOWSER_ACT_TELEPORT;
    boss->oGroundTwirlStunTimer = 20;
    object_platform_motion_finish(boss);
    EXPECT_EQ(boss->platform, NULL);
    advance(p, 20, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(fly->oPosX, 10);
    NEAR(boss->oPosX, 10);
    fly->oGroundTwirlStunTimer = 0;
    object_platform_motion_finish(fly);
    advance(p, 30, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(fly->oPosX, 10);
    return TRUE;
}
static int boss_inertia_immunity(void) {
    const BehaviorScript *const bosses[] = {
        bhvBowser,        bhvKingBobomb,  bhvWhompKingBoss, bhvBigBully,        bhvBigBullyWithMinions,
        bhvBigChillBully, bhvWigglerHead, bhvBalconyBigBoo, bhvGhostHuntBigBoo, bhvMerryGoRoundBigBoo
    };
    for (u32 kind = 0; kind < ARRAY_COUNT(bosses); kind++) {
        for (int jumping = 0; jumping < 2; jumping++) {
            for (int cycle = 0; cycle < 3; cycle++) {
                reset();
                struct Object *p = spawn(0, NULL, 0, 0, 0);
                carrier_pose(p, 0, 0, 0, 0);
                struct Object *boss = spawn(1, bosses[kind], 50, 0, 0);
                if (bosses[kind] == bhvWigglerHead) {
                    boss->oAction = WIGGLER_ACT_WALK;
                }
                boss->oGroundTwirlStunTimer = 20;
                support(boss, p, 0);
                advance(p, 10, 5, 7, 0);
                object_platform_motion_apply_all();
                NEAR(boss->oPosX, 60);
                NEAR(boss->oPosY, 5);
                NEAR(boss->oPosZ, 7);
                boss->hasFloor = FALSE;
                boss->oVelY = jumping ? 20 : -3;
                if (jumping) {
                    object_platform_motion_before_step(boss);
                }
                object_platform_motion_finish(boss);
                for (int frame = 0; frame < 4; frame++) {
                    advance(p, 20 + frame * 10, 10 + frame * 5, 14 + frame * 7, 0);
                    object_platform_motion_apply_all();
                    object_platform_motion_before_step(boss);
                    NEAR(boss->oPosX, 60);
                    NEAR(boss->oPosZ, 7);
                    NEAR(boss->oVelY, jumping ? 20 : -3);
                    NEAR(boss->oForwardVel, 7);
                    object_platform_motion_finish(boss);
                }
                boss->oVelY = 0;
                boss->oPosY = p->oPosY;
                support(boss, p, p->oPosY);
                advance(p, p->oPosX + 10, p->oPosY, p->oPosZ + 7, 0);
                object_platform_motion_apply_all();
                NEAR(boss->oPosX, 70);
                NEAR(boss->oPosZ, 14);
            }
        }
    }
    return TRUE;
}
static int foot_contact(void) {
    reset();
    struct Object *p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    struct Object *a = spawn(1, bhvGoomba, 0, 5, 0), *mole = spawn(2, bhvMontyMole, 10, 50, 0);
    mole->oGroundTwirlStunTimer = 20;
    support(a, p, 0);
    support(mole, p, 0);
    advance(p, 10, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(a->oPosX, 0);
    NEAR(mole->oPosX, 20);
    EXPECT_EQ(object_platform_motion_supported(mole), TRUE);
    a->oPosY = 4;
    EXPECT_EQ(object_platform_motion_supported(a), FALSE);
    return TRUE;
}
static int attachments(void) {
    reset();
    struct Object *p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    struct Object *a = spawn(1, bhvWigglerHead, 100, 0, 0),
                  *child = spawn(2, bhvWigglerBody, 200, 0, 0);
    struct ChainSegment segments[WIGGLER_NUM_SEGMENTS] = { 0 };
    a->oWigglerSegments = segments;
    a->oAction = WIGGLER_ACT_WALK;
    segments[0].pos[0] = 100;
    segments[1].pos[0] = 200;
    child->parentObj = a;
    support(a, p, 0);
    advance(p, 20, 0, 0, 0x4000);
    object_platform_motion_apply_all();
    NEAR(child->oPosX, 20);
    NEAR(child->oPosZ, -200);
    NEAR(segments[1].pos[0], 20);
    NEAR(segments[1].pos[2], -200);
    object_platform_motion_apply(child);
    NEAR(child->oPosZ, -200);
    return TRUE;
}
static int detached_pokey(void) {
    reset();
    struct Object *p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    struct Object *a = spawn(1, bhvPokey, 0, 0, 0), *child = spawn(2, bhvPokeyBodyPart, 0, 100, 0);
    a->oAction = POKEY_ACT_WANDER;
    child->parentObj = a;
    support(a, p, 0);
    object_platform_motion_finish(child);
    advance(p, 10, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(child->oPosX, 10);
    child->oAction = OBJ_ACT_LAVA_DEATH;
    object_platform_motion_before_step(child);
    object_platform_motion_finish(child);
    support(a, p, 0);
    advance(p, 20, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(child->oPosX, 20);
    object_platform_motion_finish(child);
    support(a, p, 0);
    advance(p, 30, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(child->oPosX, 29.7f);
    return TRUE;
}
static int post_and_hole(void) {
    reset();
    struct Object *p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    struct Object *chomp = spawn(1, bhvChainChomp, 100, 0, 0),
                  *post = spawn(2, bhvWoodenPost, 0, -40, 0),
                  *pivot = spawn(3, bhvChainChompChainPart, 0, 0, 0);
    chomp->oAction = CHAIN_CHOMP_ACT_MOVE;
    chomp->oChainChompMaxDistFromPivotPerChainPart = 20;
    chomp->parentObj = pivot;
    post->parentObj = chomp;
    pivot->parentObj = chomp;
    post->oWoodenPostOffsetY = -40;
    support(chomp, p, 0);
    support(post, p, 0);
    advance(p, 10, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(post->oPosX, 10);
    NEAR(post->home[0], 10);
    NEAR(post->oPosY, -40);
    NEAR(pivot->oPosX, 10);
    NEAR(chomp->oPosX, 110);
    struct Object *hole = spawn(4, bhvMontyMoleHole, 0, 0, 0), *mole = spawn(5, bhvMontyMole, 0, 70, 0);
    mole->oMontyMoleCurrentHole = hole;
    support(hole, p, 0);
    support(chomp, p, 0);
    support(post, p, 0);
    advance(p, 20, 0, 0, 0);
    object_platform_motion_apply_all();
    NEAR(mole->oPosX, 10);
    NEAR(mole->oPosY, 70);
    return TRUE;
}
static int publication_and_order(void) {
    reset();
    struct Object *p = spawn(1, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    struct Object *a = spawn(0, bhvSmallWhomp, 0, 0, 0);
    support(a, p, 0);
    advance(p, 10, 0, 0, 0);
    sPlatformPublished[1] = FALSE;
    struct ObjectNode head = { &a->node };
    a->node.next = &p->node;
    p->node.next = &head;
    gCurrentObject = a;
    p->oGroundTwirlStunTimer = 4;
    update_objects_starting_at(&head, head.next);
    EXPECT_EQ(p->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION, 0);
    EXPECT_EQ(updateCount, 2);
    EXPECT_EQ(updateOrder[0], 1);
    EXPECT_EQ(updateOrder[1], 0);
    EXPECT_EQ(a->updates, 1);
    EXPECT_EQ(p->updates, 1);
    NEAR(a->oPosX, 10);
    object_platform_motion_refresh_transform(a);
    NEAR(a->transform[3][0], 10);
    return TRUE;
}
static int native_integrators(void) {
    for (s32 boss = 0; boss < 2; boss++) {
        for (s32 legacy = 0; legacy < 2; legacy++) {
            reset();
            struct Object *p = spawn(0, NULL, 0, 0, 0);
            carrier_pose(p, 0, 0, 0, 0);
            const BehaviorScript *behavior = legacy ? bhvBobomb : bhvGoomba;
            if (boss) {
                behavior = bhvKingBobomb;
            }
            struct Object *a = spawn(1, behavior, 0, 0, 0);
            if (boss) {
                a->oAction = KING_BOBOMB_ACT_ACTIVE;
            }
            support(a, p, 0);
            advance(p, 10, 5, 0, 0);
            object_platform_motion_apply_all();
            a->oVelY = 20;
            a->oGravity = legacy ? 4 : -4;
            a->oFriction = 1;
            gCurrentObject = a;
            struct Surface floor = { .normal = { 0, 1, 0 } };
            if (legacy) {
                calc_new_obj_vel_and_pos_y(&floor, -1000, 0, 0);
            } else {
                cur_obj_move_using_vel_and_gravity();
            }
            NEAR(a->oVelY, boss ? 16 : 21);
            NEAR(a->oPosY, boss ? 21 : 26);
            NEAR(a->oForwardVel, 7);
            object_platform_motion_before_step(a);
            NEAR(a->oVelY, boss ? 16 : 21);
        }
    }
    return TRUE;
}
static int cycles_and_publication(void) {
    reset();
    struct Object *a = spawn(0, bhvSmallWhomp, 0, 0, 0), *b = spawn(1, bhvSmallWhomp, 0, 0, 0);
    carrier_pose(a, 0, 0, 0, 0);
    carrier_pose(b, 0, 0, 0, 0);
    support(a, b, 0);
    support(b, a, 0);
    advance(a, 0, 0, 0, 0);
    sPlatformPublished[0] = FALSE;
    struct ObjectNode head = { &a->node };
    a->node.next = &b->node;
    b->node.next = &head;
    update_objects_starting_at(&head, head.next);
    EXPECT_EQ(updateCount, 2);
    EXPECT_EQ(a->updates, 1);
    EXPECT_EQ(b->updates, 1);
    // Finalizing a grounded stun must restore collision that its skipped native
    // Whomp loop would normally publish, without a second native update.
    gCurrentObject = a;
    a->oGroundTwirlStunTimer = 4;
    sPlatformPublished[0] = FALSE;
    object_platform_motion_finish(a);
    EXPECT_EQ(sPlatformPublished[0], TRUE);
    EXPECT_EQ(a->updates, 1);
    return TRUE;
}
static int collision_mesh_requests(void) {
    // The shipped BoB bridge's vertex prefix: long Z span, narrow X span.
    static const TerrainData bridge[] = {
        TERRAIN_LOAD_VERTICES,      8,
        COL_VERTEX(-180, 51, -904), COL_VERTEX(-180, 0, -904),
        COL_VERTEX(-180, 51, 905),  COL_VERTEX(181, 0, -904),
        COL_VERTEX(-180, 0, 905),   COL_VERTEX(181, 0, 905),
        COL_VERTEX(181, 51, -904),  COL_VERTEX(181, 51, 905),
    };
    reset();
    struct Object *platform = spawn(0, sNonRiderBehavior, -2303, 717, 1024);
    carrier_pose(platform, -2303, 717, 1024, 0);
    platform->collisionData = (void *) bridge;
    platform->oCollisionDistance = 1024;
    struct Object *enemy = spawn(1, bhvBobomb, -2815, 0, 1874);
    enemy->hitboxRadius = 65;
    enemy->oWallHitboxRadius = 50;
    enemy->oForwardVel = 3;
    object_platform_motion_begin_frame();
    EXPECT_EQ(object_platform_motion_needs_collision(platform), FALSE);
    enemy->oPosX = -2303;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), TRUE);
    enemy->oPosX = -2555;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), TRUE);
    enemy->oPosX = -2556;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), FALSE);
    enemy->oWallHitboxRadius = 120;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), TRUE);
    enemy->oWallHitboxRadius = 50;
    enemy->oPosX = -2815;
    enemy->oForwardVel = 300;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), TRUE);
    enemy->oForwardVel = 3;
    sEnemyPlatformMotion[1].velocity[0] = 300;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), TRUE);
    sEnemyPlatformMotion[1].velocity[0] = 0;

    TerrainData mutableBridge[ARRAY_COUNT(bridge)];
    memcpy(mutableBridge, bridge, sizeof(bridge));
    platform->collisionData = mutableBridge;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), FALSE);
    for (s32 i = 0; i < 8; i++) {
        mutableBridge[2 + i * 3] *= 4;
    }
    EXPECT_EQ(object_platform_motion_needs_collision(platform), TRUE);
    platform->collisionData = (void *) bridge;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), FALSE);

    carrier_pose(platform, -2303, 717, 1024, 0x2000);
    platform->collisionData = (void *) bridge;
    enemy->oPosX = -2815;
    enemy->oPosZ = 1874;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), FALSE);
    enemy->oPosX = -2303 + 140;
    enemy->oPosZ = 1024 + 140;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), TRUE);

    platform->oFaceAngleYaw = 0x2000;
    platform->header.gfx.throwMatrix = NULL;
    platform->transform[3][0] = 10000; // Stale offscreen pose must be ignored.
    Mat4 stale;
    memcpy(stale, platform->transform, sizeof(stale));
    enemy->oPosX = -2815;
    enemy->oPosZ = 1874;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), FALSE);
    enemy->oPosX = -2303 + 140;
    enemy->oPosZ = 1024 + 140;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), TRUE);
    EXPECT_EQ(platform->header.gfx.throwMatrix == NULL, TRUE);
    EXPECT_EQ(memcmp(stale, platform->transform, sizeof(stale)), 0);

    carrier_pose(platform, 0, 0, 0, 0x4000);
    platform->collisionData = (void *) bridge;
    enemy->oPosX = 800;
    enemy->oPosZ = 500;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), FALSE);
    enemy->oPosZ = 0;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), TRUE);
    platform->header.gfx.scale[0] = -0.5f;
    platform->header.gfx.scale[2] = 2.0f;
    struct EnemyPlatformCollisionBounds bounds;
    EXPECT_EQ(enemy_platform_collision_bounds(platform, &bounds), TRUE);
    Mat4 scaled;
    mtxf_scale_vec3f(scaled, platform->transform, platform->header.gfx.scale);
    for (s32 i = 0; i < 8; i++) {
        Vec3f local = { bridge[2 + i * 3], bridge[3 + i * 3], bridge[4 + i * 3] };
        Vec3s vertex;
        linear_mtxf_mul_vec3_and_translate(scaled, vertex, local);
        for (s32 axis = 0; axis < 3; axis += 2) {
            EXPECT_EQ(vertex[axis] >= bounds.minimum[axis] && vertex[axis] <= bounds.maximum[axis],
                      TRUE);
        }
        enemy->oPosX = vertex[0];
        enemy->oPosZ = vertex[2];
        EXPECT_EQ(enemy_platform_overlaps_collision_bounds(&bounds, enemy, 0), TRUE);
    }
    // Tilted/reflected boxes must still enclose every loader-quantized vertex.
    for (s32 pose = 0; pose < 6; pose++) {
        Vec3s rotation = { pose * 0x1000, 0x2000, pose * 0x0800 };
        Vec3f position = { -2303.5f, 717.25f, 1024.75f };
        mtxf_rotate_zxy_and_translate(platform->transform, position, rotation);
        EXPECT_EQ(enemy_platform_collision_bounds(platform, &bounds), TRUE);
        mtxf_scale_vec3f(scaled, platform->transform, platform->header.gfx.scale);
        for (s32 i = 0; i < 8; i++) {
            Vec3f local = { bridge[2 + i * 3], bridge[3 + i * 3], bridge[4 + i * 3] };
            Vec3s vertex;
            linear_mtxf_mul_vec3_and_translate(scaled, vertex, local);
            enemy->oPosX = vertex[0];
            enemy->oPosZ = vertex[2];
            EXPECT_EQ(enemy_platform_overlaps_collision_bounds(&bounds, enemy, 0), TRUE);
        }
    }
    // Retained support and transforms awaiting carry publication bypass rejection.
    support(enemy, platform, enemy->oPosY);
    enemy->oPosX = 10000;
    EXPECT_EQ(object_platform_motion_needs_collision(platform), TRUE);
    sEnemyPlatformMotion[0].appliedFrame = gGlobalTimer;
    sEnemyPlatformMotion[0].departing = TRUE;
    EXPECT_EQ(enemy_platform_collision_bounds(platform, &bounds), FALSE);
    sEnemyPlatformMotion[0].departing = FALSE;
    platform->header.gfx.throwMatrix = NULL;
    EXPECT_EQ(enemy_platform_collision_bounds(platform, &bounds), TRUE);
    EXPECT_EQ(platform->header.gfx.throwMatrix == NULL, TRUE);
    platform->header.gfx.throwMatrix = &platform->transform;
    platform->transform[3][0] = 40000;
    EXPECT_EQ(enemy_platform_collision_bounds(platform, &bounds), FALSE);
    return TRUE;
}

static int stationary_motion(void) {
    reset();
    struct Object *platform = spawn(0, sNonRiderBehavior, 0, 0, 0);
    struct Object *enemy = spawn(1, bhvGoomba, 50, 0, 0);
    carrier_pose(platform, 0, 0, 0, 0);
    support(enemy, platform, 0);
    struct EnemyPlatformMotion *state = enemy_platform_state(enemy);
    vec3f_set(state->velocity, 7, 8, 9);
    advance(platform, 0, 0, 0, 0);
    object_platform_motion_apply(enemy);
    EXPECT_EQ(platformMotionQueries, 0);
    NEAR(enemy->oPosX, 50);
    NEAR(state->velocity[0], 0);
    NEAR(state->velocity[1], 0);
    NEAR(state->velocity[2], 0);
    object_platform_motion_finish(enemy);
    EXPECT_EQ(state->displacement.prevPlatform == platform, TRUE);
    // Even small travel keeps its collision sweep; there is no epsilon cutoff.
    advance(platform, 0.03125f, 0, 0, 0);
    object_platform_motion_apply(enemy);
    EXPECT_EQ(platformMotionQueries, 1);
    NEAR(enemy->oPosX, 50.03125f);
    // An exhausted horizontal departure still retains its one upward credit.
    state->displacement.prevPlatform = NULL;
    state->departing = state->upwardPending = TRUE;
    vec3f_set(state->velocity, 0, 12, 0);
    state->lastUpdate = gGlobalTimer;
    gGlobalTimer++;
    platformMotionQueries = 0;
    object_platform_motion_apply(enemy);
    EXPECT_EQ(platformMotionQueries, 0);
    object_platform_motion_before_step(enemy);
    NEAR(enemy->oVelY, 12);
    object_platform_motion_before_step(enemy);
    NEAR(enemy->oVelY, 12);
    // Rotation about a stationary head must still carry its segments and body.
    reset();
    platform = spawn(0, sNonRiderBehavior, 0, 0, 0);
    struct Object *head = spawn(1, bhvWigglerHead, 0, 0, 0);
    struct Object *body = spawn(2, bhvWigglerBody, 20, 0, 0);
    struct ChainSegment segments[WIGGLER_NUM_SEGMENTS] = { 0 };
    head->oAction = WIGGLER_ACT_WALK;
    head->oWigglerSegments = segments;
    segments[0].pos[0] = 20;
    body->parentObj = head;
    carrier_pose(platform, 0, 0, 0, 0);
    support(head, platform, 0);
    advance(platform, 0, 0, 0, 0x4000);
    object_platform_motion_apply(head);
    EXPECT_EQ(platformMotionQueries, 0);
    NEAR(body->oPosX, 0);
    NEAR(body->oPosZ, -20);
    NEAR(segments[0].pos[0], 0);
    NEAR(segments[0].pos[2], -20);
    return TRUE;
}

static int unpublished_pose(void) {
    reset();
    struct Object *platform = spawn(0, sNonRiderBehavior, 0, 0, 0);
    carrier_pose(platform, 0, 0, 0, 0);
    gGlobalTimer++;
    object_platform_motion_begin_frame();
    EXPECT_EQ(sPlatformPoseValid[0], TRUE);
    // The following frame has no published terrain to retain.
    gGlobalTimer++;
    object_platform_motion_begin_frame();
    EXPECT_EQ(sPlatformPoseValid[0], FALSE);
    carrier_pose(platform, 0, 20, 0, 0);
    NEAR(object_platform_motion_floor_rise(platform), 0);
    gGlobalTimer++;
    gTimeStopState = TIME_STOP_ACTIVE | TIME_STOP_ALL_OBJECTS;
    object_platform_motion_begin_frame();
    EXPECT_EQ(sPlatformPoseValid[0], TRUE);
    EXPECT_EQ(sPlatformPublished[0], TRUE);
    object_platform_motion_reset(platform);
    gGlobalTimer++;
    object_platform_motion_begin_frame();
    EXPECT_EQ(sPlatformPoseValid[0], FALSE);
    EXPECT_EQ(sPlatformPublished[0], FALSE);
    return TRUE;
}

static int collision_requests(void) {
    reset();
    struct Object *p = spawn(0, NULL, 0, 0, 0);
    carrier_pose(p, 0, 0, 0, 0);
    struct Object *a = spawn(1, bhvGoomba, 200, 0, 0);
    object_platform_motion_begin_frame();
    EXPECT_EQ(object_platform_motion_needs_collision(p), TRUE);
    a->oPosX = 5000;
    EXPECT_EQ(object_platform_motion_needs_collision(p), FALSE);
    a->oPosX = 1200;
    EXPECT_EQ(object_platform_motion_needs_collision(p), FALSE);
    sEnemyPlatformMotion[1].velocity[0] = -300;
    EXPECT_EQ(object_platform_motion_needs_collision(p), TRUE);
    a->oPosX = 200;
    a->activeFlags |= ACTIVE_FLAG_IN_DIFFERENT_ROOM;
    object_platform_motion_begin_frame();
    EXPECT_EQ(object_platform_motion_needs_collision(p), FALSE);
    return TRUE;
}
static int sleeping_update_lifecycle(void) {
    reset();
    sleepingVisibilityUpdates = 0;
    struct Object *enemy = spawn(1, bhvGoomba, 0, 0, 0);
    struct EnemyPlatformMotion *state = enemy_platform_state(enemy);
    enemy->oFlags |= OBJ_FLAG_ALLOW_ENEMY_SLEEP;
    enemy->activeFlags |= ACTIVE_FLAG_BLINKING;
    enemy->header.gfx.node.flags |= GRAPH_RENDER_HAS_ANIMATION;
    sleepRequested = TRUE;
    gCurrentObject = enemy;
    object_platform_motion_begin_frame();
    update_object_with_platform_motion();
    EXPECT_EQ(enemy->updates, 0);
    EXPECT_EQ(sleepingVisibilityUpdates, 1);
    EXPECT_EQ(sEnemyPlatformClass[enemy - gObjectPool].updateStatus, ENEMY_PLATFORM_UPDATE_COMPLETE);
    EXPECT_EQ(enemy->activeFlags & ACTIVE_FLAG_BLINKING, 0);
    EXPECT_EQ(enemy->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION, 0);
    // A recursive/list revisit must not restore animation or run the object.
    update_object_with_platform_motion();
    EXPECT_EQ(enemy->updates, 0);
    EXPECT_EQ(sleepingVisibilityUpdates, 1);
    EXPECT_EQ(enemy->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION, 0);
    gGlobalTimer++;
    object_platform_motion_begin_frame();
    sleepRequested = FALSE;
    update_object_with_platform_motion();
    EXPECT_EQ(enemy->updates, 1);
    EXPECT_EQ(state->lastUpdate, gGlobalTimer);
    EXPECT_EQ(enemy->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION, GRAPH_RENDER_HAS_ANIMATION);
    update_object_with_platform_motion();
    EXPECT_EQ(enemy->updates, 1);
    return TRUE;
}

static int sleep_motion_guard(void) {
    reset();
    struct Object *enemy = spawn(1, bhvGoomba, 0, 0, 0);
    struct EnemyPlatformMotion *state = enemy_platform_state(enemy);
    EXPECT_EQ(object_platform_motion_is_active(enemy), FALSE);
    state->departing = TRUE;
    EXPECT_EQ(object_platform_motion_is_active(enemy), TRUE);
    state->departing = FALSE;
    state->upwardPending = TRUE;
    EXPECT_EQ(object_platform_motion_is_active(enemy), TRUE);
    state->upwardPending = FALSE;
    state->displacement.prevPlatform = &gObjectPool[2];
    EXPECT_EQ(object_platform_motion_is_active(enemy), TRUE);
    object_platform_motion_reset(enemy);
    EXPECT_EQ(object_platform_motion_is_active(enemy), FALSE);
    EXPECT_EQ(object_platform_motion_is_active(NULL), FALSE);
    return TRUE;
}

static int box_support(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int policy = 0; policy < 5; policy++) {
            reset();
            struct Object *box = spawn(0, NULL, 4000, 0, 0);
            struct Object *enemy = spawn(1, bhvGoomba, 4000, 200, 0);
            gMarioObject = spawn(2, NULL, 0, 0, 0);
            gCurrLevelNum = LEVEL_PHYSICS_LAB;
            box->oAction = box->oPrevAction = EXCLAMATION_BOX_ACT_ACTIVE;
            box->oTimer = 10;
            box->oCollisionDistance = 300;
            box->oDrawingDistance = 4000;
            box->oDistanceToMario = 19000;
            box->collisionData = box;
            for (int i = 0; i < 3; i++) {
                box->header.gfx.scale[i] = 2;
            }
            if (policy == 0 || policy == 2) {
                support(enemy, box, 200);
            }
            if (policy == 1) {
                enemy->oPosX += 250; // Approaching, without prior attachment.
            }
            if (policy == 2) {
                enemy->oGroundTwirlStunTimer = 60;
            }
            if (policy == 3) {
                enemy->oPosX += 2000;
            }
            if (policy == 4) {
                enemy->activeFlags |= ACTIVE_FLAG_IN_DIFFERENT_ROOM;
            }
            object_platform_motion_begin_frame();
            EXPECT_EQ(object_platform_motion_needs_collision(box), policy < 3);
            sPlatformPublished[box - gObjectPool] = FALSE;
            gCurrentObject = box;
            boxActionCalls = 0;
            bhv_exclamation_box_loop();
            EXPECT_EQ(boxActionCalls, policy < 3);
            EXPECT_EQ(sPlatformPublished[box - gObjectPool], policy < 3);
            gMarioObject->oPosX = box->oPosX;
            bhv_exclamation_box_loop();
            EXPECT_EQ(boxActionCalls, (policy < 3) + 1);
            // Respawn and transition states must still run regardless of distance.
            gMarioObject->oPosX = 0;
            box->oAction = EXCLAMATION_BOX_ACT_WAIT_FOR_RESPAWN;
            EXPECT_EQ(physics_lab_skip_box_update(box), FALSE);
            if (policy == 0) {
                gTimeStopState = TIME_STOP_ACTIVE | TIME_STOP_ALL_OBJECTS;
                f32 x = enemy->oPosX;
                box->oPosX += 100;
                object_platform_motion_apply_all();
                NEAR(enemy->oPosX, x);
            }
        }
    }
    return TRUE;
}

#if !ENABLE_ENEMY_PLATFORM_MOTION
static int disabled_carry_and_inertia(void) {
    reset();
    struct Object *enemy = spawn(0, bhvGoomba, 0, 0, 0);
    struct Object *platform = spawn(1, sNonRiderBehavior, 0, 0, 0);
    carrier_pose(platform, 0, 0, 0, 0);
    // Even stale support/departure history must not carry or launch an enemy.
    struct EnemyPlatformMotion *state = &sEnemyPlatformMotion[0];
    state->displacement.prevPlatform = platform;
    state->departing = TRUE;
    state->upwardPending = TRUE;
    state->velocity[0] = 10;
    state->velocity[1] = 5;
    advance(platform, 10, 5, 0, 0x4000);
    EXPECT_EQ(object_platform_motion_is_rider(enemy), FALSE);
    EXPECT_EQ(object_platform_motion_is_active(enemy), FALSE);
    EXPECT_EQ(object_platform_motion_provider(enemy), NULL);
    EXPECT_EQ(object_platform_motion_needs_collision(platform), FALSE);
    object_platform_motion_apply_all();
    object_platform_motion_apply(enemy);
    object_platform_motion_before_step(enemy);
    object_platform_motion_finish(enemy);
    NEAR(enemy->oPosX, 0);
    NEAR(enemy->oPosY, 0);
    NEAR(enemy->oPosZ, 0);
    NEAR(enemy->oVelY, 0);
    EXPECT_EQ(enemy->oFaceAngleYaw, 0);
    EXPECT_EQ(floorQueries, 0);
    EXPECT_EQ(platformMotionQueries, 0);
    // The shared floor sweep still observes vertical carrier movement.
    advance(platform, 10, 15, 0, 0x4000);
    NEAR(object_platform_motion_floor_rise(platform), 10);
    gCurrentObject = enemy;
    update_object_with_platform_motion();
    update_object_with_platform_motion();
    EXPECT_EQ(enemy->updates, 1);
    return TRUE;
}
#endif

int main(int argc, char **argv) {
    test_init(argc, argv);
#define RUN(name, fn)                                                                                  \
    do {                                                                                               \
        if (test_selected("platform." name)) {                                                         \
            test_report("platform." name, fn());                                                       \
        }                                                                                              \
    } while (0)
#if !ENABLE_ENEMY_PLATFORM_MOTION
    RUN("disabled_carry_and_inertia", disabled_carry_and_inertia);
#endif
    RUN("sleeping_update_lifecycle", sleeping_update_lifecycle);
    RUN("sleep_motion_guard", sleep_motion_guard);
    RUN("box_support", box_support);
    RUN("translation", translation);
    RUN("floor_rise", floor_rise);
    RUN("rendered_floor_history", rendered_floor_history);
    RUN("tilting_floor_rise", tilting_floor_rise);
    RUN("rotation", rotation);
    RUN("departure", departure);
    RUN("loss_and_downward", loss_and_downward);
    RUN("conveyor_and_clip", conveyor_and_clip);
    RUN("lifecycle", lifecycle);
    RUN("unrelated_object_work", unrelated_object_work);
    RUN("static_terrain_work", static_terrain_work);
    RUN("inactive_boss_and_home_reset", inactive_boss_and_home_reset);
    RUN("freeze", freeze);
    RUN("policies", policies);
    RUN("boss_inertia_immunity", boss_inertia_immunity);
    RUN("foot_contact", foot_contact);
    RUN("attachments", attachments);
    RUN("detached_pokey", detached_pokey);
    RUN("post_and_hole", post_and_hole);
    RUN("publication_and_order", publication_and_order);
    RUN("collision_requests", collision_requests);
    RUN("collision_mesh_requests", collision_mesh_requests);
    RUN("unpublished_pose", unpublished_pose);
    RUN("stationary_motion", stationary_motion);
    RUN("native_integrators", native_integrators);
    RUN("cycles_and_publication", cycles_and_publication);
    return test_finish();
}
