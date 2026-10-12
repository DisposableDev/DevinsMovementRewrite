#ifndef GAME_TIMING_H
#define GAME_TIMING_H

#include <PR/ultratypes.h>
#include "course_table.h"

#define GAME_TIMING_FILES 4
#define GAME_TIMING_STARS 7
#define GAME_TIMING_TICKS_PER_SECOND 150u
#define GAME_TIMING_MAX_TICKS 0xFFFFFFFEu

// Common units preserve comparisons between 25 Hz PAL and 30 Hz NTSC saves.
#ifdef VERSION_EU
#define GAME_TIMING_TICKS_PER_UPDATE 6u
#else
#define GAME_TIMING_TICKS_PER_UPDATE 5u
#endif

// Offsets/sizes are bytes; read/write callbacks return zero on success.
typedef s32 (*GameTimingRead)(u32 offset, void *data, u32 size);
typedef s32 (*GameTimingWrite)(u32 offset, const void *data, u32 size);
// Nonzero permits writes to retired statistics banks; may retire them first.
typedef s32 (*GameTimingStorageReady)(s32 file);

struct GameTimingState {
    s32 file;
    s32 course;
    s32 level;
    u32 sessionTicks;
    u32 courseTicks;
    u32 sampledFrame;
    u8 hasSampledFrame;
    u8 activeCourse;
    // Best-time eligibility stays cleared until a fresh course attempt.
    u8 eligible;
    u8 needsCourseReset;
    u8 submittedStarMask;
};

void game_timing_init(GameTimingRead read, GameTimingWrite write, GameTimingStorageReady ready);
// Zero-based file; selection starts a fresh session, invalid indices disable it.
void game_timing_select_file(s32 file);
// Same course/level retains the attempt unless a prior exit/death requested a reset.
void game_timing_level_entered(s32 course, s32 level, s32 tainted);
// For the selected file, a stopped sample also consumes its frame key.
// Taint disqualifies before deduplication; only running samples advance clocks.
void game_timing_update(u32 frame, s32 file, s32 running, s32 tainted);
void game_timing_disqualify_file(s32 file);
void game_timing_end_course(void);
void game_timing_warp(s32 destination, s32 exitCourse);
// Star indices are zero-based; TRUE means a new/improved best, marked dirty.
s32 game_timing_collect_star(s32 file, s32 course, s32 star, s32 excluded, s32 tainted);
// Borrowed live state; later timing operations update this same object.
const struct GameTimingState *game_timing_state(void);
u32 game_timing_best(s32 file, s32 course, s32 star);
// Main courses have six missions plus 100 coins; bonus courses have fewer stars.
s32 game_timing_star_count(s32 course);
// TRUE means clean/unblocked or saved/read back; failure retains dirty work.
s32 game_timing_checkpoint(s32 file);
// Copy/erase queue both-bank replacement; checkpoint performs the writes.
void game_timing_erase(s32 file);
// Readable blank files qualify; unread/unsupported source records forbid copying.
s32 game_timing_can_copy(s32 source);
void game_timing_copy(s32 source, s32 destination);
s32 game_timing_storage_failed(s32 file);
// Format 1/150-second ticks as h:mm:ss.cc, truncating fractional hundredths.
void game_timing_format(u32 ticks, char output[24]);

#endif
