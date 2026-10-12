#include <PR/ultratypes.h>

#include "sm64.h"
#include "behavior_data.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "engine/surface_load.h"
#include "engine/surface_query.h"
#include "object_constants.h"
#include "surface_terrains.h"
#include "object_fields.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "object_platform_motion.h"
#include "platform_displacement.h"
#include "game_init.h"
#include "config.h"

#define ENEMY_PLATFORM_SUPPORT_TOLERANCE 4.0f
#define ENEMY_PLATFORM_INERTIA_DRAG 0.97f

enum EnemyPlatformPolicy {
    ENEMY_PLATFORM_NONE,
    ENEMY_PLATFORM_FREE,
    ENEMY_PLATFORM_ROOTED,
    ENEMY_PLATFORM_CHILD,
    ENEMY_PLATFORM_BOWSER,
    ENEMY_PLATFORM_KING_BOBOMB,
    ENEMY_PLATFORM_CHOMP,
    ENEMY_PLATFORM_WIGGLER,
    ENEMY_PLATFORM_POKEY,
    ENEMY_PLATFORM_MOLE,
    ENEMY_PLATFORM_STUN_ONLY
};

struct EnemyPlatformMotion {
    struct PlatformDisplacementInfo displacement;
    Vec3f velocity;
    // Reset epoch, checked against a rider's saved providerGeneration.
    u32 generation;
    u32 providerGeneration;
    u32 lastUpdate;
    u32 appliedFrame;
    u32 stepFrame;
    u8 departing;
    u8 upwardPending;
    u8 detached;
};

// Keep cached script policy and per-frame visitation separate from motion history.
// Cached non-riders can check eligibility without touching the larger history.
struct EnemyPlatformClassification {
    const BehaviorScript *behavior;
    u8 policy;
    u8 inertiaDisabled;
    u8 updateStatus;
};

// Provider poses are shared by every rider, rather than copied per enemy.
struct EnemyPlatformPose {
    Mat4 transform;
    Vec3f scale;
};

static struct EnemyPlatformMotion sEnemyPlatformMotion[OBJECT_POOL_CAPACITY];
static struct EnemyPlatformClassification sEnemyPlatformClass[OBJECT_POOL_CAPACITY];
static struct EnemyPlatformPose sEnemyPlatformPose[OBJECT_POOL_CAPACITY];
static struct Object *sPlatformRiders[OBJECT_POOL_CAPACITY];
static s32 sPlatformRiderCount;
// Pack frame flags together; resetting them must not dirty every motion/pose.
static u8 sPlatformPoseValid[OBJECT_POOL_CAPACITY];
static u8 sPlatformPublished[OBJECT_POOL_CAPACITY];
// Sorted live slots preserve the old pool order without visiting free Objects.
static u16 sPlatformObjectSlots[OBJECT_POOL_CAPACITY];
static s32 sPlatformObjectCount;

static ALWAYS_INLINE s32 enemy_platform_slot(struct Object *obj) {
    if (obj == NULL || (uintptr_t) obj < (uintptr_t) gObjectPool
        || (uintptr_t) obj >= (uintptr_t) (gObjectPool + OBJECT_POOL_CAPACITY)) {
        return -1;
    }
    return obj - gObjectPool;
}

static ALWAYS_INLINE struct EnemyPlatformMotion *enemy_platform_state(struct Object *obj) {
    s32 slot = enemy_platform_slot(obj);
    return slot >= 0 ? &sEnemyPlatformMotion[slot] : NULL;
}

void object_platform_motion_allocated(struct Object *obj) {
    if (enemy_platform_state(obj) == NULL) {
        return;
    }
    u16 slot = obj - gObjectPool;
    s32 i = 0;
    while (i < sPlatformObjectCount && sPlatformObjectSlots[i] < slot) {
        i++;
    }
    if (i < sPlatformObjectCount && sPlatformObjectSlots[i] == slot) {
        return;
    }
    for (s32 j = sPlatformObjectCount; j > i; j--) {
        sPlatformObjectSlots[j] = sPlatformObjectSlots[j - 1];
    }
    sPlatformObjectSlots[i] = slot;
    sPlatformObjectCount++;
}

void object_platform_motion_freed(struct Object *obj) {
    if (enemy_platform_state(obj) == NULL) {
        return;
    }
    u16 slot = obj - gObjectPool;
    for (s32 i = 0; i < sPlatformObjectCount; i++) {
        if (sPlatformObjectSlots[i] == slot) {
            for (s32 j = i + 1; j < sPlatformObjectCount; j++) {
                sPlatformObjectSlots[j - 1] = sPlatformObjectSlots[j];
            }
            sPlatformObjectCount--;
            return;
        }
    }
}

// Sleeping must not pause support tracking or unconsumed departure momentum.
s32 object_platform_motion_is_active(struct Object *obj) {
    if (!ENABLE_ENEMY_PLATFORM_MOTION) {
        return FALSE;
    }
    struct EnemyPlatformMotion *state = enemy_platform_state(obj);
    return state != NULL
           && (state->displacement.prevPlatform != NULL || state->departing || state->upwardPending);
}

void object_platform_motion_reset(struct Object *obj) {
    struct EnemyPlatformMotion *state = enemy_platform_state(obj);
    if (state != NULL) {
        u32 generation = state->generation + 1;
        bzero(state, sizeof(*state));
        // Motion resets do not change the behavior-keyed classification.
        state->generation = generation;
        // Start outside this/previous frame so reset history cannot be reused.
        state->lastUpdate = gGlobalTimer - 2;
        state->stepFrame = state->lastUpdate;
        state->appliedFrame = state->stepFrame;
        sEnemyPlatformClass[obj - gObjectPool].updateStatus = ENEMY_PLATFORM_UPDATE_PENDING;
        sPlatformPoseValid[obj - gObjectPool] = FALSE;
        sPlatformPublished[obj - gObjectPool] = FALSE;
    }
}

void object_platform_motion_init(void) {
    bzero(sEnemyPlatformMotion, sizeof(sEnemyPlatformMotion));
    bzero(sEnemyPlatformClass, sizeof(sEnemyPlatformClass));
    bzero(sEnemyPlatformPose, sizeof(sEnemyPlatformPose));
    bzero(sPlatformPoseValid, sizeof(sPlatformPoseValid));
    bzero(sPlatformPublished, sizeof(sPlatformPublished));
    sPlatformObjectCount = 0;
    sPlatformRiderCount = 0;
}

static s32 enemy_platform_is_boss(struct Object *obj) {
    static const BehaviorScript *const bosses[] = {
        bhvBowser,          bhvKingBobomb,          bhvWhompKingBoss,
        bhvBigBully,        bhvBigBullyWithMinions, bhvBigChillBully,
        bhvWigglerHead,     bhvWigglerBody,         bhvBalconyBigBoo,
        bhvGhostHuntBigBoo, bhvMerryGoRoundBigBoo,  bhvEyerokBoss,
        bhvEyerokHand
    };
    for (s32 i = 0; i < (s32) ARRAY_COUNT(bosses); i++) {
        if (obj_has_behavior(obj, bosses[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

static u8 enemy_platform_policy(struct Object *obj) {
    static const BehaviorScript *const freeBodies[] = {
        bhvGoomba,
#ifdef FLOOMBAS
        bhvFloomba,
#endif
        bhvKoopa,           bhvSpiny,         bhvScuttlebug,   bhvSpindrift, bhvHeaveHo,
        bhvChuckya,         bhvBobomb,        bhvSmallBully,   bhvBigBully,  bhvBigBullyWithMinions,
        bhvSmallChillBully, bhvBigChillBully, bhvMoneybag,     bhvMadPiano,  bhvHauntedChair,
        bhvMrBlizzard,      bhvSmallWhomp,    bhvWhompKingBoss
    };
    static const BehaviorScript *const groundedStuns[] = { bhvSnufit,
                                                           bhvFlyGuy,
                                                           bhvSwoop,
                                                           bhvKlepto,
                                                           bhvEnemyLakitu,
                                                           bhvSkeeter,
                                                           bhvBoo,
                                                           bhvGhostHuntBoo,
                                                           bhvBooWithCage,
                                                           bhvBalconyBigBoo,
                                                           bhvGhostHuntBigBoo,
                                                           bhvMerryGoRoundBoo,
                                                           bhvMerryGoRoundBigBoo };
    for (s32 i = 0; i < (s32) ARRAY_COUNT(freeBodies); i++) {
        if (obj_has_behavior(obj, freeBodies[i])) {
            return ENEMY_PLATFORM_FREE;
        }
    }
    for (s32 i = 0; i < (s32) ARRAY_COUNT(groundedStuns); i++) {
        if (obj_has_behavior(obj, groundedStuns[i])) {
            return ENEMY_PLATFORM_STUN_ONLY;
        }
    }
    if (obj_has_behavior(obj, bhvBowser)) {
        return ENEMY_PLATFORM_BOWSER;
    }
    if (obj_has_behavior(obj, bhvKingBobomb)) {
        return ENEMY_PLATFORM_KING_BOBOMB;
    }
    if (obj_has_behavior(obj, bhvChainChomp)) {
        return ENEMY_PLATFORM_CHOMP;
    }
    if (obj_has_behavior(obj, bhvWigglerHead)) {
        return ENEMY_PLATFORM_WIGGLER;
    }
    if (obj_has_behavior(obj, bhvPokey)) {
        return ENEMY_PLATFORM_POKEY;
    }
    if (obj_has_behavior(obj, bhvMontyMole)) {
        return ENEMY_PLATFORM_MOLE;
    }
    if (obj_has_behavior(obj, bhvPiranhaPlant) || obj_has_behavior(obj, bhvWoodenPost)
        || obj_has_behavior(obj, bhvMontyMoleHole)) {
        return ENEMY_PLATFORM_ROOTED;
    }
    if (obj_has_behavior(obj, bhvWigglerBody) || obj_has_behavior(obj, bhvPokeyBodyPart)
        || obj_has_behavior(obj, bhvChainChompChainPart)) {
        return ENEMY_PLATFORM_CHILD;
    }
    return ENEMY_PLATFORM_NONE;
}

static void enemy_platform_clear_motion(struct EnemyPlatformMotion *state) {
    state->displacement.prevPlatform = NULL;
    vec3_zero(state->velocity);
    state->upwardPending = FALSE;
    state->departing = FALSE;
}

static u8 enemy_platform_mode(struct Object *obj, u8 policy) {
    if (obj->activeFlags == ACTIVE_FLAG_DEACTIVATED
        || (obj->activeFlags & (ACTIVE_FLAG_IN_DIFFERENT_ROOM | ACTIVE_FLAG_FAR_AWAY))
        || ((obj->oFlags & OBJ_FLAG_HOLDABLE) && obj->oHeldState != HELD_FREE)) {
        return ENEMY_PLATFORM_NONE;
    }
    // Authored placement/release phases own movement even with a leftover stun.
    if ((policy == ENEMY_PLATFORM_BOWSER
         && (obj->oAction == BOWSER_ACT_TELEPORT || obj->oAction == BOWSER_ACT_JUMP_ONTO_STAGE
             || obj->oAction == BOWSER_ACT_DEAD || obj->oAction == BOWSER_ACT_INTRO_WALK))
        || (policy == ENEMY_PLATFORM_KING_BOBOMB
            && ((obj->oAction == KING_BOBOMB_ACT_INACTIVE
                 && obj->oSubAction == KING_BOBOMB_SUB_ACT_INACTIVE_INIT
                 && obj->oGroundTwirlStunTimer == 0 && obj->oGroundTwirlContactRebound == 0)
                || obj->oAction == KING_BOBOMB_ACT_RETURN_HOME || obj->oAction == KING_BOBOMB_ACT_DEATH
                || obj->oAction == KING_BOBOMB_ACT_STOP_MUSIC))
        || (policy == ENEMY_PLATFORM_CHOMP
            && (obj->oChainChompReleaseStatus != CHAIN_CHOMP_NOT_RELEASED
                || obj->oAction != CHAIN_CHOMP_ACT_MOVE))
        || (policy == ENEMY_PLATFORM_WIGGLER && obj->oAction != WIGGLER_ACT_WALK
            && obj->oAction != WIGGLER_ACT_KNOCKBACK && obj->oAction != WIGGLER_ACT_JUMPED_ON)
        || (policy == ENEMY_PLATFORM_POKEY && obj->oAction != POKEY_ACT_WANDER)) {
        return ENEMY_PLATFORM_NONE;
    }
    if (obj->oGroundTwirlStunTimer != 0 || obj->oGroundTwirlContactRebound != 0) {
        // Only classified enemies use the behavior-field aliases as stun state.
        if (policy != ENEMY_PLATFORM_NONE && policy != ENEMY_PLATFORM_ROOTED
            && policy != ENEMY_PLATFORM_CHILD) {
            return ENEMY_PLATFORM_FREE;
        }
    }
    switch (policy) {
        case ENEMY_PLATFORM_BOWSER:
        case ENEMY_PLATFORM_KING_BOBOMB:
            return ENEMY_PLATFORM_FREE;
        case ENEMY_PLATFORM_CHOMP:
        case ENEMY_PLATFORM_WIGGLER:
        case ENEMY_PLATFORM_POKEY:
            return ENEMY_PLATFORM_FREE;
        case ENEMY_PLATFORM_MOLE:
            return ENEMY_PLATFORM_NONE; // Native hole-relative phases inherit the hole's carry.
        case ENEMY_PLATFORM_STUN_ONLY:
            return ENEMY_PLATFORM_NONE;
        case ENEMY_PLATFORM_CHILD:
            if (obj_has_behavior(obj, bhvPokeyBodyPart) && obj->oAction >= OBJ_ACT_LAVA_DEATH) {
                return ENEMY_PLATFORM_FREE;
            }
            return ENEMY_PLATFORM_CHILD;
    }
    return policy;
}

// Refresh cached behavior policy and attachment state before choosing how to move.
static u8 enemy_platform_refresh_mode(struct Object *obj, struct EnemyPlatformMotion *state) {
    struct EnemyPlatformClassification *classification = &sEnemyPlatformClass[obj - gObjectPool];
    if (classification->behavior != obj->behavior) {
        enemy_platform_clear_motion(state);
        state->detached = FALSE;
        state->lastUpdate = gGlobalTimer - 2;
        classification->behavior = obj->behavior;
        classification->policy = enemy_platform_policy(obj);
        classification->inertiaDisabled = enemy_platform_is_boss(obj);
    }
    if (classification->policy == ENEMY_PLATFORM_NONE) {
        return ENEMY_PLATFORM_NONE;
    }
    u8 mode = enemy_platform_mode(obj, classification->policy);
    if (mode == ENEMY_PLATFORM_CHILD) {
        state->detached = FALSE;
    }
    return mode;
}

// Script changes clear old motion before refreshing eligibility.
// Cached non-riders leave motion history untouched.
static ALWAYS_INLINE struct EnemyPlatformMotion *
enemy_platform_refresh_rider_state(struct Object *obj) {
    if (!ENABLE_ENEMY_PLATFORM_MOTION) {
        return NULL;
    }
    s32 slot = enemy_platform_slot(obj);
    if (slot < 0) {
        return NULL;
    }
    struct EnemyPlatformClassification *classification = &sEnemyPlatformClass[slot];
    if (classification->behavior != obj->behavior) {
        enemy_platform_refresh_mode(obj, &sEnemyPlatformMotion[slot]);
    }
    return classification->policy != ENEMY_PLATFORM_NONE ? &sEnemyPlatformMotion[slot] : NULL;
}

s32 object_platform_motion_is_rider(struct Object *obj) {
    return enemy_platform_refresh_rider_state(obj) != NULL;
}

static void enemy_platform_detach_part(struct Object *obj, struct EnemyPlatformMotion *state) {
    if (sEnemyPlatformClass[obj - gObjectPool].policy == ENEMY_PLATFORM_CHILD
        && obj_has_behavior(obj, bhvPokeyBodyPart) && obj->oAction >= OBJ_ACT_LAVA_DEATH
        && !state->detached) {
        struct EnemyPlatformMotion *parent = enemy_platform_state(obj->parentObj);
        state->detached = TRUE;
        if (parent != NULL) {
            vec3f_copy(state->velocity, parent->velocity);
            state->departing = TRUE;
            state->upwardPending = state->velocity[1] > 0.0f;
        }
    }
}

static f32 enemy_platform_foot_offset(struct Object *obj) {
    if (obj_has_behavior(obj, bhvMontyMole)) {
        return 50.0f;
    }
    if (obj_has_behavior(obj, bhvWoodenPost)) {
        return obj->oWoodenPostOffsetY;
    }
    return 0.0f;
}

static f32 enemy_platform_floor(struct Object *obj, struct Surface **floor) {
    return find_floor_excluding_objects(obj->oPosX, obj->oPosY - enemy_platform_foot_offset(obj),
                                        obj->oPosZ, floor, obj,
                                        obj->parentObj != obj ? obj->parentObj : NULL);
}

s32 object_platform_motion_supported(struct Object *obj) {
    struct Surface *floor;
    f32 height = enemy_platform_floor(obj, &floor);
    return floor != NULL && obj->oVelY <= 0.0f
           && absf(obj->oPosY - enemy_platform_foot_offset(obj) - height)
                  < ENEMY_PLATFORM_SUPPORT_TOLERANCE;
}

static s32 enemy_platform_unfrozen(struct Object *obj) {
    if (!(gTimeStopState & TIME_STOP_ACTIVE)) {
        return TRUE;
    }
    return !(gTimeStopState & TIME_STOP_ALL_OBJECTS)
           && (obj->activeFlags & (ACTIVE_FLAG_UNIMPORTANT | ACTIVE_FLAG_INITIATED_TIME_STOP));
}

void object_platform_motion_begin_frame(void) {
    sPlatformRiderCount = 0;
    for (s32 entry = 0; entry < sPlatformObjectCount; entry++) {
        s32 slot = sPlatformObjectSlots[entry];
        struct Object *obj = &gObjectPool[slot];
        sEnemyPlatformClass[slot].updateStatus = ENEMY_PLATFORM_UPDATE_PENDING;
        struct EnemyPlatformPose *pose = &sEnemyPlatformPose[slot];
        // Only published terrain has a previous collision pose. A mesh first
        // appearing this frame must not inherit an old offscreen transform.
        sPlatformPoseValid[slot] = sPlatformPublished[slot]
                                   && obj->activeFlags != ACTIVE_FLAG_DEACTIVATED
                                   && obj->collisionData != NULL;
        sPlatformPublished[slot] = sPlatformPoseValid[slot] && (gTimeStopState & TIME_STOP_ACTIVE);
        if (sPlatformPoseValid[slot]) {
            // Dynamic terrain is retained during time stop. Selectively active
            // riders may move on it, but the frozen carrier contributes no delta.
            // Rendering clears throwMatrix; collision vertices were built from
            // the retained object transform, whose history remains valid.
            mtxf_copy(pose->transform, obj->transform);
            vec3f_copy(pose->scale, obj->header.gfx.scale);
        }
        if (ENABLE_ENEMY_PLATFORM_MOTION && obj->activeFlags != ACTIVE_FLAG_DEACTIVATED
            && enemy_platform_unfrozen(obj)) {
            struct EnemyPlatformMotion *state = enemy_platform_refresh_rider_state(obj);
            if (state == NULL) {
                continue;
            }
            u8 mode = enemy_platform_refresh_mode(obj, state);
            if (mode == ENEMY_PLATFORM_FREE || mode == ENEMY_PLATFORM_ROOTED) {
                sPlatformRiders[sPlatformRiderCount++] = obj;
            }
        }
    }
}

// Unnormalized horizontal separating axis and its projected interval.
struct EnemyPlatformCollisionProjection {
    f32 axisX, axisZ, minimum, maximum;
};

struct EnemyPlatformCollisionBounds {
    // Horizontal bounds: only indices 0 (X) and 2 (Z) are populated/read.
    Vec3f minimum, maximum;
    struct EnemyPlatformCollisionProjection projections[3];
    s32 projectionCount;
};

// Enclose the loader's scaled transform and s16 rounding. Bounds are local to
// this request, so mutable collision data and changed transforms stay current.
static s32 enemy_platform_collision_bounds(struct Object *platform,
                                           struct EnemyPlatformCollisionBounds *bounds)
    __attribute__((noinline));
static s32 enemy_platform_collision_bounds(struct Object *platform,
                                           struct EnemyPlatformCollisionBounds *bounds) {
    const TerrainData *data = platform->collisionData;
    struct EnemyPlatformMotion *state = enemy_platform_state(platform);
    if (data == NULL || data[0] != TERRAIN_LOAD_VERTICES || data[1] <= 0 || data[1] > 600
        || (state != NULL && state->appliedFrame == gGlobalTimer
            && object_platform_motion_is_active(platform))) {
        // A carried surface may refresh its transform only after this request.
        return FALSE;
    }
    Mat4 currentTransform;
    Mat4 *transform = &platform->transform;
    if (platform->header.gfx.throwMatrix == NULL) {
        // Rendering clears this pointer for offscreen objects. Match the
        // collision loader's pending transform without changing object state.
        Vec3s rotation = { platform->oFaceAnglePitch, platform->oFaceAngleYaw,
                           platform->oFaceAngleRoll };
        mtxf_rotate_zxy_and_translate(currentTransform, &platform->oPosVec, rotation);
        transform = &currentTransform;
    }
    Vec3s minimum = { 32767, 32767, 32767 };
    Vec3s maximum = { -32768, -32768, -32768 };
    s32 count = data[1];
    data += 2;
    for (s32 i = 0; i < count; i++, data += 3) {
        for (s32 axis = 0; axis < 3; axis++) {
            minimum[axis] = MIN(minimum[axis], data[axis]);
            maximum[axis] = MAX(maximum[axis], data[axis]);
        }
    }
    // Project the local box instead of transforming every vertex twice. Keep
    // both native s16 casts inside their range, otherwise retain the old request.
    for (s32 axis = 0; axis < 3; axis += 2) {
        f32 low = 0.0f, high = 0.0f;
        for (s32 local = 0; local < 3; local++) {
            f32 coefficient = (*transform)[local][axis] * platform->header.gfx.scale[local];
            f32 first = coefficient * minimum[local];
            f32 second = coefficient * maximum[local];
            low += MIN(first, second);
            high += MAX(first, second);
        }
        if (low < -32765.0f || high > 32765.0f) {
            return FALSE;
        }
        low += (*transform)[3][axis];
        high += (*transform)[3][axis];
        if (low < -32765.0f || high > 32765.0f) {
            return FALSE;
        }
        bounds->minimum[axis] = low - 2.0f;
        bounds->maximum[axis] = high + 2.0f;
    }
    // A rotated narrow mesh leaves empty corners in its world-aligned box.
    // Test the perpendiculars of its projected local edges too. These enclose
    // the local box even with tilt, nonuniform scale or a reflected transform.
    bounds->projectionCount = 0;
    for (s32 edge = 0; edge < 3; edge++) {
        f32 axisX = -(*transform)[edge][2] * platform->header.gfx.scale[edge];
        f32 axisZ = (*transform)[edge][0] * platform->header.gfx.scale[edge];
        if (axisX == 0.0f || axisZ == 0.0f) {
            continue; // Already covered by the world-aligned bounds.
        }
        struct EnemyPlatformCollisionProjection *projection =
            &bounds->projections[bounds->projectionCount++];
        f32 low = 0.0f, high = 0.0f;
        for (s32 local = 0; local < 3; local++) {
            f32 coefficient = (axisX * (*transform)[local][0] + axisZ * (*transform)[local][2])
                              * platform->header.gfx.scale[local];
            f32 first = coefficient * minimum[local];
            f32 second = coefficient * maximum[local];
            low += MIN(first, second);
            high += MAX(first, second);
        }
        f32 translation = axisX * (*transform)[3][0] + axisZ * (*transform)[3][2];
        f32 rounding = 2.0f * (absf(axisX) + absf(axisZ));
        projection->axisX = axisX;
        projection->axisZ = axisZ;
        projection->minimum = low + translation - rounding;
        projection->maximum = high + translation + rounding;
    }
    return TRUE;
}

static s32 enemy_platform_overlaps_collision_bounds(struct EnemyPlatformCollisionBounds *bounds,
                                                    struct Object *rider, f32 padding) {
    if (rider->oPosX + padding < bounds->minimum[0] || rider->oPosX - padding > bounds->maximum[0]
        || rider->oPosZ + padding < bounds->minimum[2] || rider->oPosZ - padding > bounds->maximum[2]) {
        return FALSE;
    }
    for (s32 i = 0; i < bounds->projectionCount; i++) {
        struct EnemyPlatformCollisionProjection *projection = &bounds->projections[i];
        f32 position = projection->axisX * rider->oPosX + projection->axisZ * rider->oPosZ;
        f32 margin = padding * (absf(projection->axisX) + absf(projection->axisZ));
        if (position + margin < projection->minimum || position - margin > projection->maximum) {
            return FALSE;
        }
    }
    return TRUE;
}

s32 object_platform_motion_needs_collision(struct Object *platform) {
    if (!ENABLE_ENEMY_PLATFORM_MOTION) {
        return FALSE;
    }
    if (platform->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM) {
        return FALSE;
    }
    struct EnemyPlatformCollisionBounds bounds;
    bounds.projectionCount = 0;
    s32 boundsReady = FALSE;
    for (s32 i = 0; i < sPlatformRiderCount; i++) {
        struct Object *rider = sPlatformRiders[i];
        struct EnemyPlatformMotion *state = enemy_platform_state(rider);
        if (rider == platform || rider->parentObj == platform) {
            continue;
        }
        if (state->displacement.prevPlatform == platform
            && state->providerGeneration == sEnemyPlatformMotion[platform - gObjectPool].generation) {
            return TRUE;
        }
        // Match the carrier's existing collision range around eligible active enemies.
        // Rendering remains based on Mario's drawing distance.
        f32 dx = rider->oPosX - platform->oPosX;
        f32 dz = rider->oPosZ - platform->oPosZ;
        f32 range = platform->oCollisionDistance + absf(rider->oForwardVel) + rider->hitboxRadius
                    + absf(state->velocity[0]) + absf(state->velocity[2]);
        f32 dy = rider->oPosY - platform->oPosY;
        if (dx * dx + dz * dz < range * range && dy > -range && dy < range + 2000.0f) {
            if (!boundsReady) {
                if (!enemy_platform_collision_bounds(platform, &bounds)) {
                    return TRUE;
                }
                boundsReady = TRUE;
            }
            // Keep the old vertical range, with horizontal room for body contact,
            // the next native step, external motion and coordinate rounding.
            f32 padding = MAX(rider->hitboxRadius, rider->oWallHitboxRadius) + absf(rider->oForwardVel)
                          + absf(state->velocity[0]) + absf(state->velocity[2]) + 2.0f;
            if (enemy_platform_overlaps_collision_bounds(&bounds, rider, padding)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void object_platform_motion_publish(struct Object *platform) {
    struct EnemyPlatformMotion *state = enemy_platform_state(platform);
    if (state != NULL) {
        sPlatformPublished[platform - gObjectPool] = TRUE;
    }
}

struct Object *object_platform_motion_provider(struct Object *obj) {
    struct EnemyPlatformMotion *state = enemy_platform_refresh_rider_state(obj);
    struct Object *provider = state != NULL ? state->displacement.prevPlatform : NULL;
    struct EnemyPlatformMotion *providerState = enemy_platform_state(provider);
    // A RUNNING provider is already on the recursion chain; reject cyclic support.
    if (providerState == NULL || provider == obj || provider->activeFlags == ACTIVE_FLAG_DEACTIVATED
        || providerState->generation != state->providerGeneration
        || sEnemyPlatformClass[provider - gObjectPool].updateStatus == ENEMY_PLATFORM_UPDATE_RUNNING) {
        if (state != NULL && provider != NULL) {
            enemy_platform_clear_motion(state);
        }
        return NULL;
    }
    return provider;
}

u8 *object_platform_motion_start_update(struct Object *obj, s32 *rider) {
    s32 slot = enemy_platform_slot(obj);
    if (slot < 0) {
        return NULL;
    }
    struct EnemyPlatformClassification *classification = &sEnemyPlatformClass[slot];
    if (classification->updateStatus != ENEMY_PLATFORM_UPDATE_PENDING) {
        return NULL;
    }
    classification->updateStatus = ENEMY_PLATFORM_UPDATE_RUNNING;
    *rider = enemy_platform_refresh_rider_state(obj) != NULL;
    return &classification->updateStatus;
}

static void enemy_platform_depart(struct EnemyPlatformMotion *state) {
    if (sEnemyPlatformClass[state - sEnemyPlatformMotion].inertiaDisabled) {
        enemy_platform_clear_motion(state);
        return;
    }
    state->displacement.prevPlatform = NULL;
    state->departing = TRUE;
    state->upwardPending = state->velocity[1] > 0.0f;
}

// Carry a world point through the provider's previous/current poses, then correct it.
// A zero previous scale leaves the point unchanged.
static void enemy_platform_carry_world_point(Vec3f worldPoint, struct Object *provider,
                                             struct EnemyPlatformPose *previousPose,
                                             Vec3f bodyCorrection) {
    Vec3f local, scaled;
    vec3f_diff(local, worldPoint, previousPose->transform[3]);
    linear_mtxf_transpose_mul_vec3(previousPose->transform, scaled, local);
    for (s32 axis = 0; axis < 3; axis++) {
        if (previousPose->scale[axis] == 0.0f) {
            return;
        }
        scaled[axis] *= provider->header.gfx.scale[axis] / previousPose->scale[axis];
    }
    linear_mtxf_mul_vec3(*provider->header.gfx.throwMatrix, worldPoint, scaled);
    vec3f_add(worldPoint, (*provider->header.gfx.throwMatrix)[3]);
    vec3f_add(worldPoint, bodyCorrection);
}

static void enemy_platform_carry_attachments(struct Object *obj, struct Object *provider, Vec3f start,
                                             Vec3f desired, s16 yaw) {
    struct EnemyPlatformPose *previousPose =
        provider != NULL ? &sEnemyPlatformPose[provider - gObjectPool] : NULL;
    Vec3f delta, bodyCorrection;
    vec3f_diff(delta, &obj->oPosVec, start);
    // Body correction includes collision clipping and tether limits after desired.
    vec3f_diff(bodyCorrection, &obj->oPosVec, desired);
    s32 useProviderTransform = previousPose != NULL
                               && sPlatformPoseValid[previousPose - sEnemyPlatformPose]
                               && !(provider->oFlags & OBJ_FLAG_NO_AUTO_DISPLACEMENT);
    if (obj_has_behavior(obj, bhvWigglerHead) && obj->oWigglerSegments != NULL) {
        for (s32 i = 0; i < WIGGLER_NUM_SEGMENTS; i++) {
            if (useProviderTransform) {
                enemy_platform_carry_world_point(obj->oWigglerSegments[i].pos, provider, previousPose,
                                                 bodyCorrection);
            } else {
                vec3f_add(obj->oWigglerSegments[i].pos, delta);
            }
            obj->oWigglerSegments[i].angle[1] += yaw;
        }
    }
    // Rotate/scale pivot-relative offsets using provider origins; omit body correction.
    if (useProviderTransform && obj_has_behavior(obj, bhvChainChomp)
        && obj->oChainChompSegments != NULL) {
        for (s32 i = 0; i < CHAIN_CHOMP_NUM_SEGMENTS; i++) {
            Vec3f world;
            vec3f_sum(world, obj->oChainChompSegments[i].pos, previousPose->transform[3]);
            Vec3f noCorrection = { 0.0f, 0.0f, 0.0f };
            enemy_platform_carry_world_point(world, provider, previousPose, noCorrection);
            vec3f_diff(obj->oChainChompSegments[i].pos, world, (*provider->header.gfx.throwMatrix)[3]);
        }
    }
    for (s32 i = 0; i < sPlatformObjectCount; i++) {
        struct Object *child = &gObjectPool[sPlatformObjectSlots[i]];
        if (child == obj || child->activeFlags == ACTIVE_FLAG_DEACTIVATED) {
            continue;
        }
        s32 attached =
            child->parentObj == obj
            && (obj_has_behavior(child, bhvWigglerBody)
                || (obj_has_behavior(child, bhvPokeyBodyPart) && child->oAction < OBJ_ACT_LAVA_DEATH)
                || (obj_has_behavior(child, bhvChainChompChainPart)
                    && child->oBehParams2ndByte != CHAIN_CHOMP_CHAIN_PART_BP_PIVOT));
        if (obj_has_behavior(obj, bhvMontyMoleHole) && obj_has_behavior(child, bhvMontyMole)
            && child->oMontyMoleCurrentHole == obj && child->oGroundTwirlStunTimer == 0
            && child->oGroundTwirlContactRebound == 0) {
            attached = TRUE;
        }
        if (obj_has_behavior(obj, bhvWoodenPost) && obj->parentObj != obj
            && obj_has_behavior(obj->parentObj, bhvChainChomp)
            && obj->parentObj->oChainChompReleaseStatus == CHAIN_CHOMP_NOT_RELEASED
            && child == obj->parentObj->parentObj) {
            attached = TRUE;
        }
        if (attached) {
            if (useProviderTransform) {
                enemy_platform_carry_world_point(&child->oPosVec, provider, previousPose,
                                                 bodyCorrection);
            } else {
                vec3f_add(&child->oPosVec, delta);
            }
            child->oMoveAngleYaw += yaw;
            child->oFaceAngleYaw += yaw;
            vec3f_copy_y_off(child->header.gfx.pos, &child->oPosVec, child->oGraphYOffset);
        }
    }
}

// Save the displacement solver's registers only when there is motion to apply.
static void enemy_platform_apply_motion(struct Object *obj, struct EnemyPlatformMotion *state, u8 mode,
                                        u8 policy, struct Object *provider) __attribute__((noinline));
static void enemy_platform_apply_motion(struct Object *obj, struct EnemyPlatformMotion *state, u8 mode,
                                        u8 policy, struct Object *provider) {
    Vec3f start, target, requestedPosition;
    vec3f_copy(start, &obj->oPosVec);
    vec3f_copy(target, start);
    s16 yaw = obj->oMoveAngleYaw;
    if (provider != NULL) {
        struct EnemyPlatformPose *pose = &sEnemyPlatformPose[provider - gObjectPool];
        if (!sPlatformPublished[pose - sEnemyPlatformPose] || provider->header.gfx.throwMatrix == NULL
            || absf(provider->header.gfx.scale[0]) < 0.0001f
            || absf(provider->header.gfx.scale[1]) < 0.0001f
            || absf(provider->header.gfx.scale[2]) < 0.0001f) {
            enemy_platform_clear_motion(state);
            return;
        }
        apply_platform_displacement(&state->displacement, target, &yaw, provider);
    } else if (state->departing && mode == ENEMY_PLATFORM_FREE) {
        target[0] += state->velocity[0];
        target[2] += state->velocity[2];
    } else {
        return;
    }
    vec3f_copy(requestedPosition, target);
    f32 footOffset = enemy_platform_foot_offset(obj);
    Vec3f footStart, footTarget;
    vec3f_copy(footStart, start);
    vec3f_copy(footTarget, target);
    footStart[1] -= footOffset;
    footTarget[1] -= footOffset;
    // No external travel means no swept contact to resolve. Still update
    // velocity, yaw and attachments when the carrier turns around this point.
    if (footStart[0] != footTarget[0] || footStart[1] != footTarget[1]
        || footStart[2] != footTarget[2]) {
        object_resolve_platform_motion(obj, footStart, footTarget, provider);
    }
    footTarget[1] += footOffset;
    vec3f_copy(&obj->oPosVec, footTarget);
    if (policy == ENEMY_PLATFORM_CHOMP && obj->parentObj != obj) {
        Vec3f fromPivot;
        vec3f_diff(fromPivot, &obj->oPosVec, &obj->parentObj->oPosVec);
        f32 distance = vec3_mag(fromPivot);
        f32 limit = obj->oChainChompMaxDistFromPivotPerChainPart * CHAIN_CHOMP_NUM_SEGMENTS;
        if (limit > 0.0f && distance > limit) {
            vec3_scale(fromPivot, limit / distance);
            vec3f_sum(&obj->oPosVec, &obj->parentObj->oPosVec, fromPivot);
        }
    }
    for (s32 axis = 0; axis < 3; axis++) {
        if (provider != NULL || axis != 1) {
            state->velocity[axis] = (&obj->oPosVec)[axis] - start[axis];
        }
    }
    // Rejected travel points from the accepted position back to the requested target.
    // Do not bank that component for a later departure or tether release.
    Vec3f rejected;
    vec3f_diff(rejected, requestedPosition, &obj->oPosVec);
    f32 rejectedLengthSq = vec3f_dot(rejected, rejected);
    if (rejectedLengthSq > 0.0001f) {
        f32 rejectedVelocityScale = vec3f_dot(state->velocity, rejected) / rejectedLengthSq;
        if (rejectedVelocityScale > 0.0f) {
            for (s32 axis = 0; axis < 3; axis++) {
                state->velocity[axis] -= rejectedVelocityScale * rejected[axis];
            }
        }
    }
    s16 yawDelta = yaw - (s16) obj->oMoveAngleYaw;
    obj->oMoveAngleYaw += yawDelta;
    obj->oFaceAngleYaw += yawDelta;
    if (mode == ENEMY_PLATFORM_ROOTED) {
        for (s32 axis = 0; axis < 3; axis++) {
            (&obj->oHomeVec)[axis] += (&obj->oPosVec)[axis] - start[axis];
        }
    }
    if (policy == ENEMY_PLATFORM_WIGGLER || policy == ENEMY_PLATFORM_POKEY
        || policy == ENEMY_PLATFORM_CHOMP || mode == ENEMY_PLATFORM_ROOTED) {
        enemy_platform_carry_attachments(obj, provider, start, requestedPosition, yawDelta);
    }
    if (provider == NULL) {
        state->velocity[0] *= ENEMY_PLATFORM_INERTIA_DRAG;
        state->velocity[2] *= ENEMY_PLATFORM_INERTIA_DRAG;
    }
    vec3f_copy_y_off(obj->header.gfx.pos, &obj->oPosVec, obj->oGraphYOffset);
}

void object_platform_motion_apply(struct Object *obj) {
    struct EnemyPlatformMotion *state = enemy_platform_refresh_rider_state(obj);
    if (state == NULL || state->appliedFrame == gGlobalTimer || !enemy_platform_unfrozen(obj)) {
        return;
    }
    state->appliedFrame = gGlobalTimer;
    u8 mode = enemy_platform_refresh_mode(obj, state);
    u8 policy = sEnemyPlatformClass[obj - gObjectPool].policy;
    if (mode != ENEMY_PLATFORM_FREE && mode != ENEMY_PLATFORM_ROOTED) {
        enemy_platform_clear_motion(state);
        return;
    }
    if (state->lastUpdate + 1 != gGlobalTimer) {
        enemy_platform_clear_motion(state);
        return;
    }
    enemy_platform_detach_part(obj, state);
    if (policy == ENEMY_PLATFORM_CHOMP) {
        // The post owns the pivot's attachment; move it before tether clipping.
        for (s32 i = 0; i < sPlatformRiderCount; i++) {
            struct Object *post = sPlatformRiders[i];
            if (post->parentObj == obj && obj_has_behavior(post, bhvWoodenPost)) {
                object_platform_motion_apply(post);
            }
        }
    }
    struct Object *provider = object_platform_motion_provider(obj);
    if (provider != NULL || (state->departing && mode == ENEMY_PLATFORM_FREE)) {
        enemy_platform_apply_motion(obj, state, mode, policy, provider);
    }
}

void object_platform_motion_apply_all(void) {
    for (s32 i = 0; i < sPlatformRiderCount; i++) {
        struct Object *obj = sPlatformRiders[i];
        // Surface-list bodies already carried before their final collision publication.
        object_platform_motion_apply(obj);
    }
}

void object_platform_motion_before_step(struct Object *obj) {
    struct EnemyPlatformMotion *state = enemy_platform_refresh_rider_state(obj);
    if (state == NULL || state->stepFrame == gGlobalTimer
        || enemy_platform_refresh_mode(obj, state) != ENEMY_PLATFORM_FREE) {
        return;
    }
    state->stepFrame = gGlobalTimer;
    enemy_platform_detach_part(obj, state);
    if (state->displacement.prevPlatform != NULL && obj->oVelY > 0.0f) {
        enemy_platform_depart(state);
    }
    if (state->upwardPending) {
        obj->oVelY += MAX(0.0f, state->velocity[1]);
        state->upwardPending = FALSE;
    }
}

// Keep floor-query registers off the common path with no dynamic support.
static void enemy_platform_finish_support(struct Object *obj, struct EnemyPlatformMotion *state,
                                          u8 policy, u8 mode) __attribute__((noinline));
static void enemy_platform_finish_support(struct Object *obj, struct EnemyPlatformMotion *state,
                                          u8 policy, u8 mode) {
    struct Surface *floor;
    f32 height = enemy_platform_floor(obj, &floor);
    s32 supported = floor != NULL && obj->oVelY <= 0.0f
                    && absf(obj->oPosY - enemy_platform_foot_offset(obj) - height)
                           < ENEMY_PLATFORM_SUPPORT_TOLERANCE;
    if (supported) {
        struct Object *provider = floor->object;
        struct EnemyPlatformMotion *providerState = enemy_platform_state(provider);
        if (providerState != NULL && provider->header.gfx.throwMatrix != NULL
            && sPlatformPublished[provider - gObjectPool]
            && absf(provider->header.gfx.scale[0]) >= 0.0001f
            && absf(provider->header.gfx.scale[1]) >= 0.0001f
            && absf(provider->header.gfx.scale[2]) >= 0.0001f) {
            if (state->displacement.prevPlatform != provider) {
                vec3_zero(state->velocity);
            }
            state->upwardPending = FALSE;
            state->departing = FALSE;
            state->providerGeneration = providerState->generation;
            if (policy == ENEMY_PLATFORM_BOWSER) {
                obj->platform = provider;
            }
            update_platform_displacement_info(&state->displacement, &obj->oPosVec, obj->oMoveAngleYaw,
                                              provider);
        } else {
            enemy_platform_clear_motion(state);
        }
    } else if (state->displacement.prevPlatform != NULL) {
        if (mode == ENEMY_PLATFORM_FREE) {
            enemy_platform_depart(state);
        } else {
            enemy_platform_clear_motion(state);
        }
    }
    if (policy == ENEMY_PLATFORM_CHOMP && obj->oChainChompRestrictedByChain) {
        // Native tether rejection must not be saved for a later release.
        vec3_zero(state->velocity);
        state->upwardPending = FALSE;
    }
    if (state->displacement.prevPlatform == NULL && (obj->oMoveFlags & OBJ_MOVE_MASK_IN_WATER)) {
        state->upwardPending = FALSE;
        state->departing = FALSE;
        vec3_zero(state->velocity);
    }
}

void object_platform_motion_finish(struct Object *obj) {
    struct EnemyPlatformMotion *state = enemy_platform_refresh_rider_state(obj);
    if (state == NULL) {
        return;
    }
    u8 policy = sEnemyPlatformClass[obj - gObjectPool].policy;
    if (policy == ENEMY_PLATFORM_BOWSER) {
        obj->platform = NULL;
    }
    u8 mode = enemy_platform_refresh_mode(obj, state);
    state->lastUpdate = gGlobalTimer;
    if (mode != ENEMY_PLATFORM_FREE && mode != ENEMY_PLATFORM_ROOTED) {
        enemy_platform_clear_motion(state);
        return;
    }
    // Generic stun bypasses a Whomp's native loop, including its publication.
    // Publish its final carried/falling pose once so it remains a real platform.
    if ((obj_has_behavior(obj, bhvSmallWhomp) || obj_has_behavior(obj, bhvWhompKingBoss))
        && obj->oAction != WHOMP_ACT_STOP_BOSS_MUSIC && obj->collisionData != NULL
        && !sPlatformPublished[obj - gObjectPool]) {
        load_object_collision_model();
    }
    // Static terrain cannot acquire a carrier. Existing support or departure
    // still needs the full query to detect landing and preserve inherited motion.
    if (state->displacement.prevPlatform == NULL && !state->departing && !state->upwardPending
        && (is_outside_level_bounds(obj->oPosX, obj->oPosZ)
            || surface_query_cell_is_empty(GET_CELL_COORD(obj->oPosX), GET_CELL_COORD(obj->oPosZ),
                                           SPATIAL_PARTITION_FLOORS, SURFACE_QUERY_DYNAMIC))) {
        enemy_platform_clear_motion(state);
        return;
    }
    enemy_platform_finish_support(obj, state, policy, mode);
}

void object_platform_motion_refresh_transform(struct Object *obj) {
    if (!ENABLE_ENEMY_PLATFORM_MOTION) {
        return;
    }
    struct EnemyPlatformMotion *state = enemy_platform_state(obj);
    if (state != NULL && state->appliedFrame == gGlobalTimer
        && (obj_has_behavior(obj, bhvSmallWhomp) || obj_has_behavior(obj, bhvWhompKingBoss)
            || obj_has_behavior(obj, bhvWoodenPost))) {
        obj_build_transform_from_pos_and_angle(obj, O_POS_INDEX, O_FACE_ANGLE_INDEX);
        obj->header.gfx.throwMatrix = &obj->transform;
    }
}

// Require a frame-start collision pose and current publication of the same object.
static struct EnemyPlatformPose *enemy_platform_previous_floor_pose(struct Object *obj) {
    struct EnemyPlatformMotion *state = enemy_platform_state(obj);
    if (state == NULL || obj->header.gfx.throwMatrix == NULL) {
        return NULL;
    }
    struct EnemyPlatformPose *previousPose = &sEnemyPlatformPose[obj - gObjectPool];
    if (!sPlatformPoseValid[previousPose - sEnemyPlatformPose]
        || !sPlatformPublished[previousPose - sEnemyPlatformPose]
        || obj->activeFlags == ACTIVE_FLAG_DEACTIVATED) {
        return NULL;
    }
    for (s32 axis = 0; axis < 3; axis++) {
        if (previousPose->scale[axis] != obj->header.gfx.scale[axis]) {
            return NULL;
        }
    }
    return previousPose;
}

f32 object_platform_motion_floor_rise(struct Object *obj) {
    struct EnemyPlatformPose *previousPose = enemy_platform_previous_floor_pose(obj);
    if (previousPose == NULL) {
        return 0.0f;
    }
    Mat4 *current = obj->header.gfx.throwMatrix;
    for (s32 axis = 0; axis < 3; axis++) {
        for (s32 column = 0; column < 3; column++) {
            if (previousPose->transform[axis][column] != (*current)[axis][column]) {
                return 0.0f;
            }
        }
    }
    if (previousPose->transform[3][0] != (*current)[3][0]
        || previousPose->transform[3][2] != (*current)[3][2]) {
        return 0.0f;
    }
    f32 rise = (*current)[3][1] - previousPose->transform[3][1];
    return rise > 0.0f && rise <= FIND_FLOOR_BUFFER ? rise : 0.0f;
}

f32 object_platform_motion_floor_rise_at(struct Object *obj, const Vec3f point, const Vec3f normal) {
    struct EnemyPlatformPose *previousPose = enemy_platform_previous_floor_pose(obj);
    if (previousPose == NULL) {
        return 0.0f;
    }
    Mat4 *current = obj->header.gfx.throwMatrix;
    Vec3f localPoint, localNormal, previousPoint, previousNormal;
    Vec3f translation;
    s32 sameBasis = TRUE;
    f32 translationSquared = 0.0f;
    for (s32 axis = 0; axis < 3; axis++) {
        translation[axis] = (*current)[3][axis] - previousPose->transform[3][axis];
        translationSquared += sqr(translation[axis]);
        for (s32 column = 0; column < 3; column++) {
            if ((*current)[axis][column] != previousPose->transform[axis][column]) {
                sameBasis = FALSE;
            }
        }
    }
    if (translationSquared > sqr(FIND_FLOOR_BUFFER) || normal[1] <= NORMAL_FLOOR_THRESHOLD) {
        return 0.0f;
    }
    // Stationary platforms and translation need no inverse or plane reconstruction.
    if (sameBasis) {
        f32 rise = vec3f_dot(normal, translation) / normal[1];
        return rise > 0.0f && rise <= FIND_FLOOR_BUFFER ? rise : 0.0f;
    }
    for (s32 axis = 0; axis < 3; axis++) {
        // Transpose is an inverse only for rigid published transforms. Reject
        // abrupt turns and scale/shear changes instead of granting an upwarp.
        if (vec3f_dot((*current)[axis], previousPose->transform[axis]) < 0.99f) {
            return 0.0f;
        }
        for (s32 column = 0; column <= axis; column++) {
            f32 expected = axis == column ? 1.0f : 0.0f;
            if (absf(vec3f_dot((*current)[axis], (*current)[column]) - expected) > 0.002f
                || absf(vec3f_dot(previousPose->transform[axis], previousPose->transform[column])
                        - expected)
                       > 0.002f) {
                return 0.0f;
            }
        }
        localPoint[axis] = 0.0f;
        localNormal[axis] = 0.0f;
        for (s32 column = 0; column < 3; column++) {
            localPoint[axis] += (*current)[axis][column] * (point[column] - (*current)[3][column]);
            localNormal[axis] += (*current)[axis][column] * normal[column];
        }
    }
    for (s32 axis = 0; axis < 3; axis++) {
        previousPoint[axis] = previousPose->transform[3][axis];
        previousNormal[axis] = 0.0f;
        for (s32 column = 0; column < 3; column++) {
            previousPoint[axis] += previousPose->transform[column][axis] * localPoint[column];
            previousNormal[axis] += previousPose->transform[column][axis] * localNormal[column];
        }
    }
    if (previousNormal[1] <= NORMAL_FLOOR_THRESHOLD) {
        return 0.0f;
    }
    // Compare both planes at Mario's X/Z, not the displacement of a vertex
    // elsewhere on the arena. Only a rise above the old plane can catch him.
    f32 previousHeight = previousPoint[1]
                         - ((point[0] - previousPoint[0]) * previousNormal[0]
                            + (point[2] - previousPoint[2]) * previousNormal[2])
                               / previousNormal[1];
    f32 rise = point[1] - previousHeight;
    // Rotation republishes integer vertices. Allow one unit when that rounding
    // puts the published previous support below its reconstructed plane.
    // A small rotation can lift a large arena edge beyond the ordinary floor
    // lookup buffer. The continuous rigid-transform checks above bound motion.
    return rise > 0.0f ? rise + 1.0f : 0.0f;
}
