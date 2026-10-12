#include "collision_profile.h"

#if COLLISION_PROFILING && defined(PUPPYPRINT_DEBUG) && defined(USE_PROFILER)

struct CollisionProfile gCollisionProfile;

static void collision_profile_reset(u32 generation) {
    bzero(&gCollisionProfile, sizeof(gCollisionProfile));
    gCollisionProfile.active = TRUE;
    gCollisionProfile.generation = generation;
}

void collision_profile_begin_frame(s32 active, u32 generation) {
    if (!active) {
        gCollisionProfile.active = FALSE;
        return;
    }
    if (!gCollisionProfile.active || gCollisionProfile.generation != generation) {
        collision_profile_reset(generation);
    }
    bzero(gCollisionProfile.current, sizeof(gCollisionProfile.current));
}

void collision_profile_end_frame(u32 generation, u32 collisionTicks, u32 cpuTicks, u32 dynamicBytes) {
    if (!gCollisionProfile.active) {
        return;
    }
    if (gCollisionProfile.generation != generation) {
        collision_profile_reset(generation);
        return;
    }

    gCollisionProfile.current[COLLISION_PROFILE_COLLISION_TICKS] = collisionTicks;
    gCollisionProfile.current[COLLISION_PROFILE_MAIN_CPU_TICKS] = cpuTicks;
    gCollisionProfile.current[COLLISION_PROFILE_DYNAMIC_BYTES] = dynamicBytes;
    for (s32 metric = 0; metric < COLLISION_PROFILE_METRIC_COUNT; metric++) {
        u32 value = gCollisionProfile.current[metric];
        u32 *previous = &gCollisionProfile.frames[gCollisionProfile.nextFrame][metric];
        gCollisionProfile.total[metric] -= *previous;
        gCollisionProfile.total[metric] += value;
        *previous = value;
        if (value > gCollisionProfile.peak[metric]) {
            gCollisionProfile.peak[metric] = value;
        }
    }
    gCollisionProfile.nextFrame = (gCollisionProfile.nextFrame + 1) % COLLISION_PROFILE_FRAMES;
    if (gCollisionProfile.frameCount < COLLISION_PROFILE_FRAMES) {
        gCollisionProfile.frameCount++;
    }
}

u32 collision_profile_average(enum CollisionProfileMetric metric) {
    if (gCollisionProfile.frameCount == 0) {
        return 0;
    }
    return gCollisionProfile.total[metric] / gCollisionProfile.frameCount;
}

#endif
