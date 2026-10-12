#ifndef OBJECT_PLATFORM_MOTION_H
#define OBJECT_PLATFORM_MOTION_H

#include "types.h"

// Per-frame visitation, including objects whose native update is skipped.
enum EnemyPlatformUpdateStatus {
    ENEMY_PLATFORM_UPDATE_PENDING,
    ENEMY_PLATFORM_UPDATE_RUNNING,
    ENEMY_PLATFORM_UPDATE_COMPLETE
};

// Discard external support/momentum and invalidate riders' references to this object.
// Preserve behavior classification, but reopen visitation and forget published poses.
void object_platform_motion_reset(struct Object *obj);
// Track pool membership only; allocation initializes history separately via reset.
void object_platform_motion_allocated(struct Object *obj);
void object_platform_motion_freed(struct Object *obj);
// Clear all history and tracked slots when clearing the object lists.
void object_platform_motion_init(void);
// Once per object-list update, before clearing and republishing dynamic terrain.
void object_platform_motion_begin_frame(void);
void object_platform_motion_apply_all(void);
void object_platform_motion_apply(struct Object *obj);
// Record post-behavior support/departure and this update's history.
// May publish Whomp collision; that path requires obj == gCurrentObject.
void object_platform_motion_finish(struct Object *obj);
void object_platform_motion_before_step(struct Object *obj);
s32 object_platform_motion_is_active(struct Object *obj);
// Script eligibility, including bodies whose current movement mode is suspended.
// With carry enabled, changed scripts refresh classification and clear old motion.
s32 object_platform_motion_is_rider(struct Object *obj);
// Feet within the strict support tolerance in world units, with no upward velocity.
// The floor query excludes self/parent and accepts static or dynamic terrain.
s32 object_platform_motion_supported(struct Object *obj);
// Pool platforms only. TRUE requests collision for eligible enemies.
// Unknown/unsafe bounds retain nearby demand; the loader combines this with Mario's
// range and applies its time-stop/room gates.
s32 object_platform_motion_needs_collision(struct Object *platform);
// Mark a pool object's completed collision publication for this frame's history.
void object_platform_motion_publish(struct Object *platform);
// Refresh eligibility and return a live saved provider, or NULL.
// Stale, self or cyclic support clears the rider's external motion.
struct Object *object_platform_motion_provider(struct Object *obj);
// Claim PENDING as RUNNING before refreshing script eligibility into rider.
// Return a borrowed status byte; the caller must mark COMPLETE on every exit.
// NULL means outside the pool or already visited; rider is then unchanged.
u8 *object_platform_motion_start_update(struct Object *obj, s32 *rider);
// Before collision loading, refresh Whomp/post transforms handled by this frame's carry phase.
void object_platform_motion_refresh_transform(struct Object *obj);
// Verified pure vertical rise this update, in world units; zero for stale/discontinuous poses.
f32 object_platform_motion_floor_rise(struct Object *obj);
// Conservative rise in world units at this footprint; zero for invalid motion.
// point lies on the current floor plane; normal is its world-space normal.
// Gradual tilt includes one unit of mesh rounding and can exceed FIND_FLOOR_BUFFER.
f32 object_platform_motion_floor_rise_at(struct Object *obj, const Vec3f point, const Vec3f normal);

#endif
