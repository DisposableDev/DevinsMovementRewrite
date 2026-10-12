#ifndef COLLISION_PROFILE_H
#define COLLISION_PROFILE_H

#include <ultra64.h>
#include "config/config_debug.h"
#include "config/config_safeguards.h"

#define COLLISION_PROFILE_FRAMES 15

enum CollisionProfileMetric {
    COLLISION_PROFILE_FLOOR_NODES,
    COLLISION_PROFILE_WALL_NODES,
    COLLISION_PROFILE_CEIL_NODES,
    COLLISION_PROFILE_WATER_NODES,
    COLLISION_PROFILE_RAY_NODES,
    COLLISION_PROFILE_MARIO_PREPARE_NODES,
    COLLISION_PROFILE_WATER_EDGE_NODES,
    COLLISION_PROFILE_OBJECT_SWEEP_NODES,
    COLLISION_PROFILE_DYNAMIC_MODELS,
    COLLISION_PROFILE_DYNAMIC_SURFACES,
    COLLISION_PROFILE_DYNAMIC_NODES,
    COLLISION_PROFILE_DYNAMIC_BUILD_TICKS,
    COLLISION_PROFILE_DYNAMIC_CLEAR_TICKS,
    COLLISION_PROFILE_COLLISION_TICKS,
    COLLISION_PROFILE_MAIN_CPU_TICKS,
    COLLISION_PROFILE_DYNAMIC_BYTES,
    COLLISION_PROFILE_METRIC_COUNT
};

#if COLLISION_PROFILING && defined(PUPPYPRINT_DEBUG) && defined(USE_PROFILER)
struct CollisionProfile {
    s32 active;
    u32 generation;
    u32 frameCount;
    u32 nextFrame;
    u32 current[COLLISION_PROFILE_METRIC_COUNT];
    u64 total[COLLISION_PROFILE_METRIC_COUNT];
    u32 peak[COLLISION_PROFILE_METRIC_COUNT];
    u32 frames[COLLISION_PROFILE_FRAMES][COLLISION_PROFILE_METRIC_COUNT];
};

extern struct CollisionProfile gCollisionProfile;

// Count examined list entries, including rejected entries and repeated/cache-copy visits.
#define COLLISION_PROFILE_ADD(metric, amount)                                                          \
    do {                                                                                               \
        if (gCollisionProfile.active) {                                                                \
            gCollisionProfile.current[metric] += (amount);                                             \
        }                                                                                              \
    } while (0)
#define COLLISION_PROFILE_COUNT(metric) COLLISION_PROFILE_ADD(metric, 1)

// Capture on the Collision debug page or throughout a PERFORMANCE_CAPTURE session.
// Terrain changes discard the transition frame.
void collision_profile_begin_frame(s32 active, u32 generation);
// Tick arguments are raw CPU counter ticks; dynamicBytes counts allocated terrain storage.
void collision_profile_end_frame(u32 generation, u32 collisionTicks, u32 cpuTicks, u32 dynamicBytes);
// Truncated mean in the selected metric's units; zero before any frame is recorded.
u32 collision_profile_average(enum CollisionProfileMetric metric);
#else
#define COLLISION_PROFILE_ADD(metric, amount)
#define COLLISION_PROFILE_COUNT(metric)
#define collision_profile_begin_frame(active, generation)
#define collision_profile_end_frame(generation, collisionTicks, cpuTicks, dynamicBytes)
#endif

#endif
