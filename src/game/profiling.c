#include <ultra64.h>
#include <PR/os_internal_reg.h>
#include "game_init.h"
#include "main.h"

#include "profiling.h"
#include "fasttext.h"
#include "puppyprint.h"
#include "collision_profile.h"
#include "engine/surface_load.h"
#include "performance_capture.h"
#if PERFORMANCE_CAPTURE
#include "area.h"
#include "level_update.h"
#include "mario.h"
#if !defined(USE_PROFILER) || !defined(PUPPYPRINT_DEBUG)
#error Performance capture requires USE_PROFILER and PUPPYPRINT_DEBUG
#endif
#endif

#ifdef USE_PROFILER

#define RDP_CYCLE_CONV(x) ((10 * (x)) / 625) // 62.5 million cycles per frame

ProfileTimeData all_profiling_data[PROFILER_TIME_COUNT];

int profile_buffer_index = -1;
int rsp_buffer_indices[PROFILER_RSP_COUNT];
// Holds either the start time if the task is running, or the amount of time the task has run for so far if yielded
u32 rsp_pending_times[PROFILER_RSP_COUNT];
u32 cur_start;
u32 prev_time;
u32 audio_start;
u32 audio_buffer_index;
u32 preempted_time;
u32 collision_time = 0;
#ifdef PUPPYPRINT_DEBUG
static u32 sAudioCpuTicks;
static u32 sAudioCpuStart;
#endif
static u32 prev_fps_vblanks;
static u32 fps_sample_count;
static f32 fps_vi_rate;
static u32 sUpdatedObjects;
static u32 sSleepingObjects;

#ifdef AUDIO_PROFILING
u32 audio_subset_starts[AUDIO_SUBSET_SIZE];
u32 audio_subset_tallies[AUDIO_SUBSET_SIZE];
#endif

static void buffer_update(ProfileTimeData* data, u32 new, int buffer_index) {
    u32 old = data->counts[buffer_index];
    data->total -= old;
    data->total += new;
    data->counts[buffer_index] = new;
}

void profiler_update(enum ProfilerTime which, u32 delta) {
    u32 saved = __osDisableInt();
    u32 cur_time = osGetCount();
    u32 cur_preempted_time = preempted_time;
    preempted_time = 0;
    __osRestoreInt(saved);

    u32 elapsed = cur_time - prev_time;
    ProfileTimeData *cur_data = &all_profiling_data[which];

    // Clamp overlapping exclusions instead of wrapping a negative sample to u32.
    u32 diff = (delta < elapsed) ? elapsed - delta : 0;
#ifdef PUPPYPRINT_DEBUG
    if (which == PROFILER_TIME_PUPPYPRINT1 || which == PROFILER_TIME_PUPPYPRINT2) {
        diff = delta;
    }
#endif
    if (cur_preempted_time > 0) {
        diff = (cur_preempted_time < diff) ? diff - cur_preempted_time : 0;
        cur_start += cur_preempted_time;
    }
    
    buffer_update(cur_data, diff, profile_buffer_index);
    prev_time = cur_time;
}

void profiler_rsp_started(enum ProfilerRSPTime which) {
    rsp_pending_times[which] = osGetCount();
}

void profiler_rsp_completed(enum ProfilerRSPTime which) {
    ProfileTimeData* cur_data = &all_profiling_data[PROFILER_TIME_RSP_GFX + which];
    int cur_index = rsp_buffer_indices[which];
    u32 time = osGetCount() - rsp_pending_times[which];
    rsp_pending_times[which] = 0;

    buffer_update(cur_data, time, cur_index);
    cur_index++;
    if (cur_index >= PROFILING_BUFFER_SIZE) {
        cur_index = 0;
    }
    rsp_buffer_indices[which] = cur_index;
}

void profiler_rsp_resumed() {
    rsp_pending_times[PROFILER_RSP_GFX] = osGetCount() - rsp_pending_times[PROFILER_RSP_GFX];
}

// This ends up being the same math as resumed, so we just use resumed for both
// void profiler_rsp_yielded() {
//     rsp_pending_times[PROFILER_RSP_GFX] = osGetCount() - rsp_pending_times[PROFILER_RSP_GFX];
// }

void profiler_audio_started() {
    audio_start = osGetCount();
#ifdef PUPPYPRINT_DEBUG
    sAudioCpuStart = audio_start;
#endif

#ifdef AUDIO_PROFILING
    for (s32 i = 0; i < AUDIO_SUBSET_SIZE; i++) {
        audio_subset_tallies[i] = 0;
    }

    audio_subset_starts[PROFILER_TIME_SUB_AUDIO_UPDATE - PROFILER_TIME_SUB_AUDIO_START] = audio_start;
#endif
}

#ifdef PUPPYPRINT_DEBUG

u32 profiler_collision_clock(void) {
    u32 saved = __osDisableInt();
    u32 time = osGetCount() - sAudioCpuTicks;
    __osRestoreInt(saved);
    return time;
}

u32 profiler_audio_wait_started(void) {
    u32 time = osGetCount();
    // Publish the completed CPU segment before the game can run during DMA.
    sAudioCpuTicks += time - sAudioCpuStart;
    return time;
}

void profiler_collision_reset() {
    collision_time = 0;
}

void profiler_collision_update(u32 time) {
    collision_time += profiler_collision_clock() - time;
}

void profiler_collision_completed() {
    ProfileTimeData* cur_data = &all_profiling_data[PROFILER_TIME_COLLISION];
    buffer_update(cur_data, collision_time, profile_buffer_index);
#if COLLISION_PROFILING
    collision_profile_end_frame(gStaticSurfaceGeneration, collision_time,
                                all_profiling_data[PROFILER_TIME_TOTAL].counts[profile_buffer_index],
                                (uintptr_t) gDynamicSurfacePoolEnd - (uintptr_t) gDynamicSurfacePool);
#endif
}

#endif

u32 profiler_get_delta(enum ProfilerDeltaTime which) {
    if (which == PROFILER_DELTA_COLLISION) {
        return collision_time;
    } else {
        return 0;
    }
}

void profiler_audio_waited(u32 ticks) {
    // During sample DMA waits the game thread can run; it was not preempted.
    audio_start += ticks;
#ifdef PUPPYPRINT_DEBUG
    sAudioCpuStart = osGetCount();
#endif
#ifdef AUDIO_PROFILING
    audio_subset_starts[PROFILER_TIME_SUB_AUDIO_UPDATE - PROFILER_TIME_SUB_AUDIO_START] += ticks;
#endif
}

void profiler_audio_completed() {
    ProfileTimeData* cur_data = &all_profiling_data[PROFILER_TIME_AUDIO];
    u32 time = osGetCount();
    u32 cur_index = audio_buffer_index;
#ifdef PUPPYPRINT_DEBUG
    sAudioCpuTicks += time - sAudioCpuStart;
#endif

    // Several audio updates may interrupt one main-thread interval.
    preempted_time += time - audio_start;
    buffer_update(cur_data, time - audio_start, cur_index);

#ifdef AUDIO_PROFILING
    audio_subset_tallies[PROFILER_TIME_SUB_AUDIO_UPDATE - PROFILER_TIME_SUB_AUDIO_START] += time - audio_subset_starts[PROFILER_TIME_SUB_AUDIO_UPDATE - PROFILER_TIME_SUB_AUDIO_START];

    for (s32 i = 0; i < AUDIO_SUBSET_SIZE; i++) {
        cur_data = &all_profiling_data[i + PROFILER_TIME_SUB_AUDIO_START];
        buffer_update(cur_data, audio_subset_tallies[i], cur_index);
    }
#endif

    cur_index++;
    if (cur_index >= PROFILING_BUFFER_SIZE) {
        cur_index = 0;
    }

    audio_buffer_index = cur_index;
}

static void update_total_timer() {
    u32 saved = __osDisableInt();
    u32 cur_preempted_time = preempted_time;
    preempted_time = 0;
    __osRestoreInt(saved);

    cur_start += cur_preempted_time;
    prev_time = cur_start;
    u32 debug_time = 0;
#ifdef PUPPYPRINT_DEBUG
    debug_time = all_profiling_data[PROFILER_TIME_PUPPYPRINT1].counts[profile_buffer_index]
                 + all_profiling_data[PROFILER_TIME_PUPPYPRINT2].counts[profile_buffer_index];
#endif
    profiler_update(PROFILER_TIME_TOTAL, debug_time);
}

#ifdef PUPPYPRINT_DEBUG
extern u8 sPPDebugPage;
extern u8 fDebug;
#endif

static void update_rdp_timers() {
    u32 tmem = IO_READ(DPC_TMEM_REG);
    u32 cmd =  IO_READ(DPC_BUFBUSY_REG);
    u32 pipe = IO_READ(DPC_PIPEBUSY_REG);
    
    if (gGlobalTimer > 5) {
        IO_WRITE(DPC_STATUS_REG, (DPC_CLR_CLOCK_CTR | DPC_CLR_CMD_CTR | DPC_CLR_PIPE_CTR | DPC_CLR_TMEM_CTR));
    }

    buffer_update(&all_profiling_data[PROFILER_TIME_TMEM], tmem, profile_buffer_index);
    buffer_update(&all_profiling_data[PROFILER_TIME_CMD], cmd, profile_buffer_index);
    buffer_update(&all_profiling_data[PROFILER_TIME_PIPE], pipe, profile_buffer_index);
}

float profiler_get_fps() {
    u32 vblanks = all_profiling_data[PROFILER_TIME_FPS].total;
    return vblanks ? fps_vi_rate * fps_sample_count / vblanks : 0.0f;
}

u32 profiler_get_cpu_cycles() {
    u32 cpu_normal_time = all_profiling_data[PROFILER_TIME_TOTAL].total / PROFILING_BUFFER_SIZE;
    u32 cpu_audio_time = all_profiling_data[PROFILER_TIME_AUDIO].total / PROFILING_BUFFER_SIZE;
    return cpu_normal_time + cpu_audio_time * 2;
}

u32 profiler_get_rsp_cycles() {
    u32 rsp_graphics_time = all_profiling_data[PROFILER_TIME_RSP_GFX].total / PROFILING_BUFFER_SIZE;
    u32 rsp_audio_time = all_profiling_data[PROFILER_TIME_RSP_AUDIO].total / PROFILING_BUFFER_SIZE;
    return rsp_graphics_time + rsp_audio_time;
}

u32 profiler_get_rdp_cycles() {
    u32 rdp_pipe_cycles = all_profiling_data[PROFILER_TIME_PIPE].total;
    u32 rdp_tmem_cycles = all_profiling_data[PROFILER_TIME_TMEM].total;
    u32 rdp_cmd_cycles = all_profiling_data[PROFILER_TIME_CMD].total;

    u32 rdp_max_cycles = MAX(MAX(rdp_pipe_cycles, rdp_tmem_cycles), rdp_cmd_cycles);

    return rdp_max_cycles / PROFILING_BUFFER_SIZE;
}

u32 profiler_get_cpu_microseconds() {
    u32 cpu_normal_time = OS_CYCLES_TO_USEC(all_profiling_data[PROFILER_TIME_TOTAL].total / PROFILING_BUFFER_SIZE);
    u32 cpu_audio_time = OS_CYCLES_TO_USEC(all_profiling_data[PROFILER_TIME_AUDIO].total / PROFILING_BUFFER_SIZE);
    return cpu_normal_time + cpu_audio_time * 2;
}

u32 profiler_get_rsp_microseconds() {
    u32 rsp_graphics_time = OS_CYCLES_TO_USEC(all_profiling_data[PROFILER_TIME_RSP_GFX].total / PROFILING_BUFFER_SIZE);
    u32 rsp_audio_time = OS_CYCLES_TO_USEC(all_profiling_data[PROFILER_TIME_RSP_AUDIO].total / PROFILING_BUFFER_SIZE);
    return rsp_graphics_time + rsp_audio_time;
}

u32 profiler_get_rdp_microseconds() {
    u32 rdp_pipe_cycles = all_profiling_data[PROFILER_TIME_PIPE].total;
    u32 rdp_tmem_cycles = all_profiling_data[PROFILER_TIME_TMEM].total;
    u32 rdp_cmd_cycles = all_profiling_data[PROFILER_TIME_CMD].total;

    u32 rdp_max_cycles = MAX(MAX(rdp_pipe_cycles, rdp_tmem_cycles), rdp_cmd_cycles);

    return RDP_CYCLE_CONV(rdp_max_cycles / PROFILING_BUFFER_SIZE);
}

void profiler_print_times() {
    u32 microseconds[PROFILER_TIME_COUNT];
    char text_buffer[512];
    char object_buffer[128];

    update_total_timer();
    update_rdp_timers();

#ifndef PUPPYPRINT_DEBUG
    static u8 show_profiler = 0;
    if ((gPlayer1Controller->buttonPressed & (L_TRIG | U_JPAD)) && (gPlayer1Controller->buttonDown & L_TRIG) && (gPlayer1Controller->buttonDown & U_JPAD)) {
        show_profiler ^= 1;
    }
#endif

#ifdef PUPPYPRINT_DEBUG
    if (fDebug && sPPDebugPage == PUPPYPRINT_PAGE_PROFILER) {
#else
    if (show_profiler) {
#endif
        for (int i = PROFILER_TIME_CONTROLLERS; i < PROFILER_TIME_COUNT; i++) {
            if (i < PROFILER_TIME_TMEM) {
                microseconds[i] = OS_CYCLES_TO_USEC(all_profiling_data[i].total / PROFILING_BUFFER_SIZE);
            } else {
                microseconds[i] = RDP_CYCLE_CONV(all_profiling_data[i].total / PROFILING_BUFFER_SIZE);
            }
        }

        // audio time is removed from the main thread profiling, so add it back here
        u32 total_cpu = microseconds[PROFILER_TIME_TOTAL] + microseconds[PROFILER_TIME_AUDIO] * 2;
        u32 total_rsp = microseconds[PROFILER_TIME_RSP_GFX] + microseconds[PROFILER_TIME_RSP_AUDIO] * 2;
        u32 max_rdp = MAX(MAX(microseconds[PROFILER_TIME_TMEM], microseconds[PROFILER_TIME_CMD]), microseconds[PROFILER_TIME_PIPE]);

        sprintf(text_buffer,
                "FPS: %5.2f\n"
                "\n"
                "CPU\t\t%d (%d%%)\n"
                " Input\t\t%d\n"
#ifdef PUPPYPRINT_DEBUG
                " Collision\t\t%d\n"
#else
                " Dynamic\t\t%d\n"
#endif
                " Mario\t\t\t%d\n"
                " Behavior\t\t%d\n"
                " Graph\t\t%d\n"
                " Audio\t\t\t%d\n"
#ifdef PUPPYPRINT_DEBUG
                " Camera\t\t%d\n"
#endif
                "\n"
                "RDP\t\t%d (%d%%)\n"
                " Tmem\t\t\t%d\n"
                " Cmd\t\t\t%d\n"
                " Pipe\t\t\t%d\n"
                "\n"
                "RSP\t\t%d (%d%%)\n"
                " Gfx\t\t\t%d\n"
                " Audio\t\t\t%d\n",
                profiler_get_fps(), total_cpu, total_cpu / 333, microseconds[PROFILER_TIME_CONTROLLERS],
#ifdef PUPPYPRINT_DEBUG
                microseconds[PROFILER_TIME_COLLISION],
#else
                microseconds[PROFILER_TIME_DYNAMIC],
#endif
                microseconds[PROFILER_TIME_MARIO],
                microseconds[PROFILER_TIME_SPAWNER]
#ifdef PUPPYPRINT_DEBUG
                    + microseconds[PROFILER_TIME_DYNAMIC]
#endif
                    + microseconds[PROFILER_TIME_BEHAVIOR_BEFORE_MARIO]
                    + microseconds[PROFILER_TIME_BEHAVIOR_AFTER_MARIO],
                microseconds[PROFILER_TIME_GFX],
                microseconds[PROFILER_TIME_AUDIO] * 2, // audio is 60Hz, so double the average
#ifdef PUPPYPRINT_DEBUG
                microseconds[PROFILER_TIME_CAMERA],
#endif
                max_rdp, max_rdp / 333, microseconds[PROFILER_TIME_TMEM],
                microseconds[PROFILER_TIME_CMD], microseconds[PROFILER_TIME_PIPE], total_rsp,
                total_rsp / 333, microseconds[PROFILER_TIME_RSP_GFX],
                microseconds[PROFILER_TIME_RSP_AUDIO] * 2);

        sprintf(object_buffer, "Obj pairs: %d\nPlatforms: %d\nCleanup: %d\nAwake/Sleep: %d/%d",
                microseconds[PROFILER_TIME_OBJECT_COLLISION],
                microseconds[PROFILER_TIME_OBJECT_PLATFORM_SETUP]
                    + microseconds[PROFILER_TIME_OBJECT_PLATFORM_PREPARE],
                microseconds[PROFILER_TIME_OBJECT_CLEANUP], sUpdatedObjects, sSleepingObjects);

        Gfx* dlHead = gDisplayListHead;
        gDPPipeSync(dlHead++);
        gDPSetCycleType(dlHead++, G_CYC_1CYCLE);
        gDPSetRenderMode(dlHead++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
        gDPSetTexturePersp(dlHead++, G_TP_NONE);
        gDPSetTextureFilter(dlHead++, G_TF_POINT);
        gDPSetTextureLUT(dlHead++, G_TT_NONE);
        drawSmallStringCol(&dlHead, 10, 8, text_buffer, 255, 255, 255);
        drawSmallStringCol(&dlHead, SCREEN_WIDTH - 136, 140, object_buffer, 255, 255, 255);
        gDisplayListHead = dlHead;
    }
}

void profiler_count_object(s32 sleeping) {
    if (sleeping) {
        sSleepingObjects++;
    } else {
        sUpdatedObjects++;
    }
}

static void profiler_clear_main_thread_samples(void) {
    static const enum ProfilerTime mainThreadTimes[] = {
        PROFILER_TIME_CONTROLLERS,
        PROFILER_TIME_SPAWNER,
        PROFILER_TIME_DYNAMIC,
        PROFILER_TIME_OBJECT_PLATFORM_SETUP,
        PROFILER_TIME_OBJECT_PLATFORM_PREPARE,
        PROFILER_TIME_OBJECT_COLLISION,
        PROFILER_TIME_BEHAVIOR_BEFORE_MARIO,
        PROFILER_TIME_MARIO,
        PROFILER_TIME_BEHAVIOR_AFTER_MARIO,
        PROFILER_TIME_OBJECT_CLEANUP,
        PROFILER_TIME_GFX,
        PROFILER_TIME_COLLISION,
        PROFILER_TIME_CAMERA,
        PROFILER_TIME_TOTAL,
#ifdef PUPPYPRINT_DEBUG
        PROFILER_TIME_PUPPYPRINT1,
        PROFILER_TIME_PUPPYPRINT2,
#endif
    };
    // A skipped subsystem contributes zero this frame, rather than retaining
    // the previous visit to this ring slot. Audio and RSP own separate clocks.
    for (s32 i = 0; i < (s32) ARRAY_COUNT(mainThreadTimes); i++) {
        buffer_update(&all_profiling_data[mainThreadTimes[i]], 0, profile_buffer_index);
    }
    sUpdatedObjects = 0;
    sSleepingObjects = 0;
}

#if PERFORMANCE_CAPTURE
static void profiler_capture_frame(u32 vblanks, u32 elapsedVblanks) {
    static u32 windowFrames;
    static u32 windowVblanks;
    static u32 maxVblanks;
    static s32 windowLevel;
    static s32 windowArea;
    static s32 windowValid;
    s32 ready = gMarioState != NULL && gMarioState->marioObj != NULL && gCurrDemoInput == NULL
                && gCurrCreditsEntry == NULL;
    if (windowFrames == 0) {
        windowLevel = gCurrLevelNum;
        windowArea = gCurrAreaIndex;
        windowValid = ready && sCurrPlayMode == PLAY_MODE_NORMAL;
    }
    windowValid &= ready && sCurrPlayMode == PLAY_MODE_NORMAL && windowLevel == gCurrLevelNum
                   && windowArea == gCurrAreaIndex;
    windowVblanks += elapsedVblanks;
    if (elapsedVblanks > maxVblanks) {
        maxVblanks = elapsedVblanks;
    }
    windowFrames++;
    if (windowFrames < PERFORMANCE_CAPTURE_INTERVAL) {
        return;
    }
    u32 start = osGetCount();
    static const enum ProfilerTime times[] = {
        PROFILER_TIME_TOTAL,
        PROFILER_TIME_CONTROLLERS,
        PROFILER_TIME_SPAWNER,
        PROFILER_TIME_DYNAMIC,
        PROFILER_TIME_OBJECT_PLATFORM_SETUP,
        PROFILER_TIME_OBJECT_PLATFORM_PREPARE,
        PROFILER_TIME_OBJECT_COLLISION,
        PROFILER_TIME_BEHAVIOR_BEFORE_MARIO,
        PROFILER_TIME_MARIO,
        PROFILER_TIME_BEHAVIOR_AFTER_MARIO,
        PROFILER_TIME_OBJECT_CLEANUP,
        PROFILER_TIME_GFX,
        PROFILER_TIME_COLLISION,
        PROFILER_TIME_CAMERA,
    };
    struct PerformanceCaptureSample sample = { 0 };
    sample.vblank = vblanks;
    sample.elapsedVblanks = windowVblanks;
    sample.maxElapsedVblanks = maxVblanks;
    sample.sampleFrames = windowFrames;
    sample.viMillihz = fps_vi_rate * 1000.0f;
    sample.level = gCurrLevelNum;
    sample.area = gCurrAreaIndex;
    sample.playMode = sCurrPlayMode;
    sample.ready = windowValid;
    if (ready) {
        sample.action = gMarioState->action;
        sample.flags = gMarioState->flags;
        for (s32 i = 0; i < 3; i++) {
            sample.position[i] = gMarioState->pos[i];
        }
    }
    for (s32 i = 0; i < (s32) ARRAY_COUNT(times); i++) {
#if COLLISION_PROFILING
        sample.timers[i] = all_profiling_data[times[i]].counts[profile_buffer_index];
#else
        ProfileTimeData *data = &all_profiling_data[times[i]];
        sample.timers[i] = data->total / PROFILING_BUFFER_SIZE;
        for (s32 frame = 0; frame < PROFILING_BUFFER_SIZE; frame++) {
            if (data->counts[frame] > sample.peaks[i]) {
                sample.peaks[i] = data->counts[frame];
            }
        }
#endif
    }
    // Audio/RSP ring slots follow their own task clocks, not game-frame indices.
    u32 saved = __osDisableInt();
    sample.timers[14] = all_profiling_data[PROFILER_TIME_AUDIO].total / PROFILING_BUFFER_SIZE;
    sample.timers[15] = all_profiling_data[PROFILER_TIME_RSP_GFX].total / PROFILING_BUFFER_SIZE;
    sample.timers[16] = all_profiling_data[PROFILER_TIME_RSP_AUDIO].total / PROFILING_BUFFER_SIZE;
    __osRestoreInt(saved);
    sample.timers[17] = all_profiling_data[PROFILER_TIME_TMEM].total / PROFILING_BUFFER_SIZE;
    sample.timers[18] = all_profiling_data[PROFILER_TIME_PIPE].total / PROFILING_BUFFER_SIZE;
    sample.timers[19] = all_profiling_data[PROFILER_TIME_CMD].total / PROFILING_BUFFER_SIZE;
    sample.updatedObjects = sUpdatedObjects;
    sample.sleepingObjects = sSleepingObjects;
    sample.calls[0] = gPuppyCallCounter.collision_floor;
    sample.calls[1] = gPuppyCallCounter.collision_wall;
    sample.calls[2] = gPuppyCallCounter.collision_ceil;
    sample.calls[3] = gPuppyCallCounter.collision_water;
    sample.calls[4] = gPuppyCallCounter.collision_raycast;
    sample.calls[5] = gPuppyCallCounter.matrix;
#if COLLISION_PROFILING
    sample.collisionValid = gCollisionProfile.active && gCollisionProfile.frameCount > 0;
    if (sample.collisionValid) {
        for (s32 i = 0; i < COLLISION_PROFILE_METRIC_COUNT; i++) {
            sample.collision[i] = gCollisionProfile.current[i];
        }
    }
#endif
    performance_capture_frame(&sample, start);
    windowFrames = 0;
    windowVblanks = 0;
    maxVblanks = 0;
}
#endif

void profiler_frame_setup() {
    s32 first_frame = profile_buffer_index < 0;
    u32 cur_vblanks = gNumVblanks;

#if PERFORMANCE_CAPTURE
    if (!first_frame) {
        profiler_capture_frame(cur_vblanks, cur_vblanks - prev_fps_vblanks);
    }
#endif

    profile_buffer_index++;

    if (profile_buffer_index >= PROFILING_BUFFER_SIZE) {
        profile_buffer_index = 0;
    }

    profiler_clear_main_thread_samples();
#if COLLISION_PROFILING && defined(PUPPYPRINT_DEBUG)
    s32 collisionCaptureActive = fDebug && !sDebugMenu && sPPDebugPage == PUPPYPRINT_PAGE_COLLISION;
#if PERFORMANCE_CAPTURE
    collisionCaptureActive = TRUE;
#endif
    collision_profile_begin_frame(collisionCaptureActive, gStaticSurfaceGeneration);
#endif

    if (first_frame) {
        prev_fps_vblanks = cur_vblanks;
        fps_vi_rate = get_vi_refresh_rate();
    } else {
        // Count elapsed video refreshes rather than emulator-dependent CPU ticks.
        buffer_update(&all_profiling_data[PROFILER_TIME_FPS], cur_vblanks - prev_fps_vblanks,
                      profile_buffer_index);
        prev_fps_vblanks = cur_vblanks;
        if (fps_sample_count < PROFILING_BUFFER_SIZE) {
            fps_sample_count++;
        }
    }

    u32 saved = __osDisableInt();
    preempted_time = 0;
    prev_time = cur_start = osGetCount();
    __osRestoreInt(saved);
}

#endif
