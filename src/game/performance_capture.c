#include "performance_capture.h"

#if PERFORMANCE_CAPTURE

// Four worst-case rows fit below Mupen's 4 KiB ISViewer message limit.
#define CAPTURE_ROW_BYTES 768
#if COLLISION_PROFILING
#define CAPTURE_BATCH_FRAMES 4
#else
#define CAPTURE_BATCH_FRAMES 1
#endif
#define ISVIEWER_BUFFER 0xB3FF0020
#define ISVIEWER_LENGTH 0xB3FF0014

static char sCaptureBuffer[CAPTURE_ROW_BYTES * CAPTURE_BATCH_FRAMES];
static u32 sCaptureBytes;
static u32 sCaptureFrames;
static u32 sCaptureSequence;
static u32 sLastCaptureTicks;

static char *performance_capture_value(char *next, u32 value) {
    static const char digits[] = "0123456789abcdef";
    for (s32 shift = 28; shift >= 0; shift -= 4) {
        *next++ = digits[(value >> shift) & 15];
    }
    return next;
}

static void performance_capture_flush(void) {
    for (u32 offset = 0; offset < sCaptureBytes; offset += 4) {
        u32 word = 0;
        for (u32 byte = 0; byte < 4; byte++) {
            word <<= 8;
            if (offset + byte < sCaptureBytes) {
                word |= (u8) sCaptureBuffer[offset + byte];
            }
        }
        IO_WRITE(ISVIEWER_BUFFER + offset, word);
    }
    IO_WRITE(ISVIEWER_LENGTH, sCaptureBytes);
    sCaptureBytes = 0;
    sCaptureFrames = 0;
}

void performance_capture_frame(const struct PerformanceCaptureSample *sample, u32 start) {
    char *row = sCaptureBuffer + sCaptureBytes;
    char *next = row;
#if COLLISION_PROFILING
    static const char prefix[] = "SM64PERF1,";
#else
    static const char prefix[] = "SM64PERF2,";
#endif

    // Fixed-width hexadecimal avoids expensive per-frame printf/decimal division.
    for (s32 i = 0; i < (s32) sizeof(prefix) - 1; i++) {
        *next++ = prefix[i];
    }
    next = performance_capture_value(next, sCaptureSequence++);
    next = performance_capture_value(next, sample->vblank);
    next = performance_capture_value(next, sample->elapsedVblanks);
    next = performance_capture_value(next, sample->viMillihz);
    next = performance_capture_value(next, sample->level);
    next = performance_capture_value(next, sample->area);
    next = performance_capture_value(next, sample->playMode);
    next = performance_capture_value(next, sample->ready);
    next = performance_capture_value(next, sample->action);
    next = performance_capture_value(next, sample->flags);
    for (s32 i = 0; i < 3; i++) {
        next = performance_capture_value(next, sample->position[i]);
    }
#if COLLISION_PROFILING
    s32 timerCount = 17;
#else
    s32 timerCount = PERFORMANCE_CAPTURE_TIMERS;
#endif
    for (s32 i = 0; i < timerCount; i++) {
        next = performance_capture_value(next, sample->timers[i]);
    }
#if !COLLISION_PROFILING
    for (s32 i = 0; i < PERFORMANCE_CAPTURE_MAIN_TIMERS; i++) {
        next = performance_capture_value(next, sample->peaks[i]);
    }
#endif
    next = performance_capture_value(next, sample->updatedObjects);
    next = performance_capture_value(next, sample->sleepingObjects);
    for (s32 i = 0; i < PERFORMANCE_CAPTURE_CALLS; i++) {
        next = performance_capture_value(next, sample->calls[i]);
    }
#if COLLISION_PROFILING
    next = performance_capture_value(next, sample->collisionValid);
    for (s32 i = 0; i < PERFORMANCE_CAPTURE_COLLISION_METRICS; i++) {
        next = performance_capture_value(next, sample->collision[i]);
    }
#else
    next = performance_capture_value(next, sample->sampleFrames);
    next = performance_capture_value(next, sample->maxElapsedVblanks);
#endif
    // The preceding export ran at the start of this completed game frame.
    next = performance_capture_value(next, sLastCaptureTicks);
    *next++ = '\n';
    sCaptureBytes += next - row;
    sCaptureFrames++;
    if (sCaptureFrames == CAPTURE_BATCH_FRAMES) {
        performance_capture_flush();
    }
    sLastCaptureTicks = osGetCount() - start;
}

#endif
