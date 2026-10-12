#include <PR/ultratypes.h>

#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "level_update.h"
#include "object_fields.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "platform_displacement.h"
#include "types.h"
#include "sm64.h"
#include "behavior_data.h"
#include "game_init.h"

#include "config.h"

struct Object *gMarioPlatform = NULL;
static struct PlatformDisplacementInfo sMarioDisplacementInfo;
static Vec3f sMarioAmountDisplaced;

/**
 * Determine if Mario is standing on a platform object, meaning that he is
 * within 4 units of the floor. Set his referenced platform object accordingly.
 */
void update_mario_platform(void) {
    struct Surface *floor;
    f32 marioX, marioY, marioZ;
    f32 floorHeight;
    u32 awayFromFloor;

    if (gMarioObject == NULL) {
        return;
    }

    //! If Mario moves onto a rotating platform in a PU, the find_floor call
    //  will detect the platform and he will end up receiving a large amount
    //  of displacement since he is considered to be far from the platform's
    //  axis of rotation.

    marioX = gMarioObject->oPosX;
    marioY = gMarioObject->oPosY;
    marioZ = gMarioObject->oPosZ;
    floorHeight = find_floor(marioX, marioY, marioZ, &floor);

    awayFromFloor =  absf(marioY - floorHeight) >= 4.0f;

    if (awayFromFloor) {
        gMarioPlatform = NULL;
        gMarioObject->platform = NULL;
    } else {
        if (floor != NULL && floor->object != NULL) {
            gMarioPlatform = floor->object;
            gMarioObject->platform = floor->object;
            // If this is the first frame of Mario standing on the platform,
            // then initialise his platform displacement info struct
            if ((floor->object != sMarioDisplacementInfo.prevPlatform)
                || (gGlobalTimer != sMarioDisplacementInfo.prevTimer)) {
                update_platform_displacement_info(&sMarioDisplacementInfo, gMarioState->pos,
                                                  gMarioState->faceAngle[1], floor->object);
            }
        } else {
            gMarioPlatform = NULL;
            gMarioObject->platform = NULL;
        }
    }
}

/**
 * Set the values in the platform displacement struct for use next frame
 */
void update_platform_displacement_info(struct PlatformDisplacementInfo *displaceInfo, Vec3f pos,
                                       s16 yaw, struct Object *platform) {
    Vec3f scaledLocalPos, yawVec, worldOffset;

    // Avoid a crash if the platform unloaded its collision while stood on or is static
    if (platform->header.gfx.throwMatrix == NULL) {
        return;
    }

    // Update position
    vec3_diff(worldOffset, pos, (*platform->header.gfx.throwMatrix)[3]);
    linear_mtxf_transpose_mul_vec3(*platform->header.gfx.throwMatrix, scaledLocalPos, worldOffset);
    vec3_quot(displaceInfo->prevTransformedPos, scaledLocalPos, platform->header.gfx.scale);
    vec3_copy(displaceInfo->prevPos, pos);

    // Update yaw
    vec3_set(yawVec, sins(yaw), 0, coss(yaw));
    linear_mtxf_transpose_mul_vec3(*platform->header.gfx.throwMatrix,
                                   displaceInfo->prevTransformedYawVec, yawVec);
    displaceInfo->prevYaw = yaw;

    // Update platform and timer
    displaceInfo->prevPlatform = platform;
    displaceInfo->prevTimer = gGlobalTimer;
}

/**
 * Apply one frame of platform displacement to Mario or an object using the given
 * platform.
 */
void apply_platform_displacement(struct PlatformDisplacementInfo *displaceInfo, Vec3f pos, s16 *yaw,
                                 struct Object *platform) {
    Vec3f selfMovementDelta;
    Vec3f yawVec;
    Vec3f scaledLocalPos;
    // Preserve the rider's own turn since the previous snapshot.
    s16 yawDifference = *yaw - displaceInfo->prevYaw;

    // Avoid a crash if the platform unloaded its collision while stood on or is static
    if (platform->header.gfx.throwMatrix == NULL) {
        return;
    }

    // Preserve the rider's own world-space movement since the previous snapshot.
    vec3_diff(selfMovementDelta, pos, displaceInfo->prevPos);

    if ((platform == displaceInfo->prevPlatform) && (gGlobalTimer == displaceInfo->prevTimer + 1)) {
        // For certain objects, only use velocity for displacement rather than the transform
        // E.g. TTC treadmills
        if (platform->oFlags & OBJ_FLAG_NO_AUTO_DISPLACEMENT) {
            pos[0] += platform->oVelX;
            pos[1] += platform->oVelY;
            pos[2] += platform->oVelZ;
        } else {
            // Transform from relative positions to world positions
            vec3_prod(scaledLocalPos, displaceInfo->prevTransformedPos, platform->header.gfx.scale);
            linear_mtxf_mul_vec3(*platform->header.gfx.throwMatrix, pos, scaledLocalPos);
            vec3_add(pos, (*platform->header.gfx.throwMatrix)[3]);

            // Add the rider's own movement after transforming the saved local position.
            vec3_add(pos, selfMovementDelta);

            // Calculate new yaw
            linear_mtxf_mul_vec3(*platform->header.gfx.throwMatrix, yawVec,
                                 displaceInfo->prevTransformedYawVec);
            *yaw = atan2s(yawVec[2], yawVec[0]) + yawDifference;
        }
    }

    Vec3f oldPos;
    vec3_sum(oldPos, displaceInfo->prevPos, selfMovementDelta);
    update_platform_displacement_info(displaceInfo, pos, *yaw, platform);

    // Only Mario's actual position buffer records displacement for departure inertia.
    if (pos == gMarioState->pos) {
        vec3_diff(sMarioAmountDisplaced, pos, oldPos);
    }
}

// Gate Mario's departure inertia after support is lost.
u8 gDoInertia = TRUE;

static u8 sMarioInertiaArmed = FALSE;
// First processed inertia update, which can be delayed by time stop or gDoInertia.
static u8 sMarioFirstInertiaUpdate = FALSE;

/**
 * Apply inertia based on Mario's last platform.
 */
static void apply_mario_inertia(void) {
#ifdef MARIO_INERTIA_UPWARD
    // Grant positive Y displacement on the first processed inertia update.
    if (sMarioFirstInertiaUpdate) {
        if (sMarioAmountDisplaced[1] > 0.0f) {
            gMarioState->vel[1] += sMarioAmountDisplaced[1];
        }
    }
#endif

#ifdef MARIO_INERTIA_LATERAL
    // Apply sideways inertia
    gMarioState->pos[0] += sMarioAmountDisplaced[0];
    gMarioState->pos[2] += sMarioAmountDisplaced[2];

    // Drag
    sMarioAmountDisplaced[0] *= 0.97f;
    sMarioAmountDisplaced[2] *= 0.97f;
#endif

    // After applying this update, disarm on landing or ground pound.
    if (!(gMarioState->action & ACT_FLAG_AIR) || (gMarioState->action == ACT_GROUND_POUND)) {
        sMarioInertiaArmed = FALSE;
    }
}

/**
 * Apply platform displacement or inertia if required.
 */
void apply_mario_platform_displacement(void) {
    struct Object *platform;

    platform = gMarioPlatform;
    if (!(gTimeStopState & TIME_STOP_ACTIVE) && gMarioObject != NULL) {
        if (platform != NULL) {
            apply_platform_displacement(&sMarioDisplacementInfo, gMarioState->pos,
                                        &gMarioState->faceAngle[1], platform);
            sMarioInertiaArmed = TRUE;
            sMarioFirstInertiaUpdate = TRUE;
        } else if (sMarioInertiaArmed && gDoInertia) {
            apply_mario_inertia();
            sMarioFirstInertiaUpdate = FALSE;
        }
    }
}

/**
 * Set Mario's platform to NULL.
 */
void clear_mario_platform(void) {
    gMarioPlatform = NULL;
}
