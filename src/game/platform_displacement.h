#ifndef PLATFORM_DISPLACEMENT_H
#define PLATFORM_DISPLACEMENT_H

#include <PR/ultratypes.h>

#include "types.h"

#include "config.h"

struct PlatformDisplacementInfo {
    // World position, then platform-basis position with object scale removed.
    Vec3f prevPos;
    Vec3f prevTransformedPos;
    // Platform-basis heading vector and the rider's world-space yaw.
    Vec3f prevTransformedYawVec;
    s16 prevYaw;
    // Borrowed support and gGlobalTimer at capture.
    struct Object *prevPlatform;
    u32 prevTimer;
};

// After native movement, select Mario's floor-object support and capture new contact.
void update_mario_platform(void);
// Capture position/yaw history; a missing matrix leaves the record unchanged.
void update_platform_displacement_info(struct PlatformDisplacementInfo *displaceInfo, Vec3f pos,
                                       s16 yaw, struct Object *platform);
// Carry mutable world position/yaw for the same platform on consecutive updates.
// Refresh history even on a continuity miss; a missing matrix leaves it unchanged.
// Mario's actual position buffer also records displacement for departure inertia.
void apply_platform_displacement(struct PlatformDisplacementInfo *displaceInfo, Vec3f pos, s16 *yaw,
                                 struct Object *platform);
// Before Mario's update, apply carry or armed departure inertia unless time is stopped.
void apply_mario_platform_displacement(void);
// Clear gMarioPlatform, retaining gMarioObject->platform, saved history and inertia state.
void clear_mario_platform(void);

#endif // PLATFORM_DISPLACEMENT_H
