#ifndef PERFORMANCE_CAPTURE_H
#define PERFORMANCE_CAPTURE_H

#include <ultra64.h>
#include "config/config_debug.h"

#define PERFORMANCE_CAPTURE_TIMERS 20
#define PERFORMANCE_CAPTURE_MAIN_TIMERS 14
#define PERFORMANCE_CAPTURE_CALLS 6
#define PERFORMANCE_CAPTURE_COLLISION_METRICS 16

#if COLLISION_PROFILING
#define PERFORMANCE_CAPTURE_INTERVAL 1
#else
#define PERFORMANCE_CAPTURE_INTERVAL 15
#endif

struct PerformanceCaptureSample {
    u32 vblank;
    u32 elapsedVblanks;
    u32 viMillihz;
    s32 level;
    s32 area;
    s32 playMode;
    s32 ready;
    u32 action;
    u32 flags;
    s32 position[3];
    u32 timers[PERFORMANCE_CAPTURE_TIMERS];
    u32 peaks[PERFORMANCE_CAPTURE_MAIN_TIMERS];
    u32 sampleFrames;
    u32 maxElapsedVblanks;
    u32 updatedObjects;
    u32 sleepingObjects;
    u32 calls[PERFORMANCE_CAPTURE_CALLS];
    u32 collisionValid;
    u32 collision[PERFORMANCE_CAPTURE_COLLISION_METRICS];
};

#if PERFORMANCE_CAPTURE
void performance_capture_frame(const struct PerformanceCaptureSample *sample, u32 start);
#endif

#endif
