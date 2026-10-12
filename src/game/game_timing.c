#include "game_timing.h"

#include <stddef.h>
#include <string.h>
#include "player_stats.h"

#define TIMING_MAGIC 0x54494D31u
#define TIMING_VERSION 1u
#define TIMING_BOTH_BANKS_MASK 3
#define TIMING_RECORD_COUNT (COURSE_COUNT * GAME_TIMING_STARS)

struct TimingRecord {
    u32 magic;
    u32 version;
    u32 generation;
    u32 file;
    u32 count;
    u32 times[TIMING_RECORD_COUNT];
    u32 checksum;
    u32 reserved[2];
} __attribute__((aligned(16)));

_Static_assert(GAME_TIMING_FILES == PLAYER_STATS_SAVE_FILES, "Timing file layout changed");
_Static_assert(sizeof(struct TimingRecord) <= PLAYER_STATS_LEGACY_SAVE_STRIDE,
               "Times exceed the retired statistics bank");
_Static_assert(sizeof(struct TimingRecord) % 16 == 0, "Timing DMA alignment changed");

static GameTimingRead sRead;
static GameTimingWrite sWrite;
static GameTimingStorageReady sStorageReady;
static struct GameTimingState sTiming;
static u32 sBest[GAME_TIMING_FILES][TIMING_RECORD_COUNT];
static u32 sGeneration[GAME_TIMING_FILES];
// Latest validated/verified bank; -1 means neither bank was selected.
static s8 sSelectedBank[GAME_TIMING_FILES];
static u8 sDirty[GAME_TIMING_FILES];
// Bit 0/1 mark replacements still awaiting a verified write.
static u8 sBanksToReplace[GAME_TIMING_FILES];
// Read failures/future versions preserve readable values but block implicit writes.
static u8 sLoadBlocked[GAME_TIMING_FILES];
static u8 sSaveFailed[GAME_TIMING_FILES];
// Shared only by synchronous main-thread load/save operations.
static struct TimingRecord sScratch[2] __attribute__((aligned(16)));

static s32 valid_file(s32 file) {
    return file >= 0 && file < GAME_TIMING_FILES;
}

static s32 valid_course(s32 course) {
    return course >= COURSE_MIN && course <= COURSE_MAX && course != COURSE_CAKE_END;
}

s32 game_timing_star_count(s32 course) {
    if (!valid_course(course)) {
        return 0;
    }
    return course <= COURSE_STAGES_MAX ? GAME_TIMING_STARS : course == COURSE_PSS ? 2 : 1;
}

static u32 record_offset(s32 file, s32 bank) {
    return PLAYER_STATS_LEGACY_SRAM_OFFSET + (file * 2 + bank) * PLAYER_STATS_LEGACY_SAVE_STRIDE;
}

static u32 record_checksum(const struct TimingRecord *record) {
    const u8 *bytes = (const u8 *) record;
    u32 crc = ~0u;
    for (u32 i = 0; i < offsetof(struct TimingRecord, checksum); i++) {
        crc ^= bytes[i];
        for (s32 bit = 0; bit < 8; bit++) {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320u : 0);
        }
    }
    return ~crc;
}

static s32 valid_record(const struct TimingRecord *record, s32 file) {
    if (record->magic != TIMING_MAGIC || record->version != TIMING_VERSION || record->file != (u32) file
        || record->count != TIMING_RECORD_COUNT || record->checksum != record_checksum(record)) {
        return 0;
    }
    for (s32 i = 0; i < TIMING_RECORD_COUNT; i++) {
        if (record->times[i] > GAME_TIMING_MAX_TICKS) {
            return 0;
        }
    }
    return 1;
}

void game_timing_init(GameTimingRead read, GameTimingWrite write, GameTimingStorageReady ready) {
    sRead = read;
    sWrite = write;
    sStorageReady = ready;
    memset(sBest, 0, sizeof(sBest));
    memset(sGeneration, 0, sizeof(sGeneration));
    memset(sDirty, 0, sizeof(sDirty));
    memset(sBanksToReplace, 0, sizeof(sBanksToReplace));
    memset(sLoadBlocked, 0, sizeof(sLoadBlocked));
    memset(sSaveFailed, 0, sizeof(sSaveFailed));
    memset(sSelectedBank, -1, sizeof(sSelectedBank));
    memset(&sTiming, 0, sizeof(sTiming));
    sTiming.file = -1;
    for (s32 file = 0; file < GAME_TIMING_FILES; file++) {
        for (s32 bank = 0; bank < 2; bank++) {
            struct TimingRecord *record = &sScratch[bank];
            if (sRead == NULL || sRead(record_offset(file, bank), record, sizeof(*record)) != 0) {
                sLoadBlocked[file] = 1;
                continue;
            }
            if (record->magic == TIMING_MAGIC && record->version > TIMING_VERSION
                && record->file == (u32) file) {
                sLoadBlocked[file] = 1;
                continue;
            }
            if (!valid_record(record, file)
                || (sSelectedBank[file] >= 0 && (s32) (record->generation - sGeneration[file]) <= 0)) {
                continue;
            }
            memcpy(sBest[file], record->times, sizeof(sBest[file]));
            sGeneration[file] = record->generation;
            sSelectedBank[file] = bank;
        }
    }
}

void game_timing_select_file(s32 file) {
    memset(&sTiming, 0, sizeof(sTiming));
    sTiming.file = valid_file(file) ? file : -1;
    sTiming.needsCourseReset = 1;
}

void game_timing_disqualify_file(s32 file) {
    if (valid_file(file) && file == sTiming.file) {
        sTiming.eligible = 0;
    }
}

void game_timing_level_entered(s32 course, s32 level, s32 tainted) {
    if (!valid_file(sTiming.file)) {
        return;
    }
    if (sTiming.needsCourseReset || course != sTiming.course || level != sTiming.level) {
        sTiming.course = course;
        sTiming.level = level;
        sTiming.courseTicks = 0;
        sTiming.submittedStarMask = 0;
        sTiming.activeCourse = valid_course(course);
        sTiming.eligible = sTiming.activeCourse && !tainted;
        sTiming.needsCourseReset = 0;
    } else if (tainted) {
        sTiming.eligible = 0;
    }
}

void game_timing_end_course(void) {
    sTiming.activeCourse = 0;
    sTiming.eligible = 0;
    sTiming.needsCourseReset = 1;
}

void game_timing_warp(s32 destination, s32 exitCourse) {
    if (exitCourse || destination != sTiming.level) {
        game_timing_end_course();
    }
}

static u32 advance_ticks(u32 ticks) {
    return ticks > GAME_TIMING_MAX_TICKS - GAME_TIMING_TICKS_PER_UPDATE
               ? GAME_TIMING_MAX_TICKS
               : ticks + GAME_TIMING_TICKS_PER_UPDATE;
}

void game_timing_update(u32 frame, s32 file, s32 running, s32 tainted) {
    if (file != sTiming.file) {
        return;
    }
    // Paused/repeated samples still disqualify the current attempt.
    if (tainted) {
        game_timing_disqualify_file(file);
    }
    if (!valid_file(file) || (sTiming.hasSampledFrame && frame == sTiming.sampledFrame)) {
        return;
    }
    sTiming.sampledFrame = frame;
    sTiming.hasSampledFrame = 1;
    if (!running) {
        return;
    }
    sTiming.sessionTicks = advance_ticks(sTiming.sessionTicks);
    if (sTiming.activeCourse) {
        sTiming.courseTicks = advance_ticks(sTiming.courseTicks);
        if (sTiming.courseTicks == GAME_TIMING_MAX_TICKS) {
            sTiming.eligible = 0;
        }
    }
}

s32 game_timing_collect_star(s32 file, s32 course, s32 star, s32 excluded, s32 tainted) {
    if (tainted) {
        game_timing_disqualify_file(file);
    }
    if (!valid_file(file) || file != sTiming.file || !valid_course(course) || course != sTiming.course
        || !sTiming.activeCourse || !sTiming.eligible || sTiming.courseTicks == 0 || excluded
        || star < 0 || star >= game_timing_star_count(course)
        || (sTiming.submittedStarMask & (1 << star))) {
        return 0;
    }
    // A collection consumes its visit allowance even when it does not improve the best.
    sTiming.submittedStarMask |= 1 << star;
    u32 *best = &sBest[file][(course - COURSE_MIN) * GAME_TIMING_STARS + star];
    if (*best == 0 || sTiming.courseTicks < *best) {
        *best = sTiming.courseTicks;
        sDirty[file] = 1;
        return 1;
    }
    return 0;
}

const struct GameTimingState *game_timing_state(void) {
    return &sTiming;
}

u32 game_timing_best(s32 file, s32 course, s32 star) {
    return valid_file(file) && valid_course(course) && star >= 0 && star < GAME_TIMING_STARS
               ? sBest[file][(course - COURSE_MIN) * GAME_TIMING_STARS + star]
               : 0;
}

s32 game_timing_checkpoint(s32 file) {
    if (!valid_file(file)) {
        return 0;
    }
    if (!sDirty[file]) {
        return !sLoadBlocked[file] && !sSaveFailed[file];
    }
    if (sLoadBlocked[file] || sRead == NULL || sWrite == NULL || sStorageReady == NULL
        || !sStorageReady(file)) {
        sSaveFailed[file] = 1;
        return 0;
    }
    struct TimingRecord *record = &sScratch[0];
    struct TimingRecord *verify = &sScratch[1];
    if (sSelectedBank[file] < 0) {
        sBanksToReplace[file] = TIMING_BOTH_BANKS_MASK;
    }
    do {
        s32 bank = sSelectedBank[file] == 0 ? 1 : 0;
        memset(record, 0, sizeof(*record));
        record->magic = TIMING_MAGIC;
        record->version = TIMING_VERSION;
        record->generation = sGeneration[file] + 1;
        record->file = file;
        record->count = TIMING_RECORD_COUNT;
        memcpy(record->times, sBest[file], sizeof(sBest[file]));
        record->checksum = record_checksum(record);
        if (sWrite(record_offset(file, bank), record, sizeof(*record)) != 0
            || sRead(record_offset(file, bank), verify, sizeof(*verify)) != 0
            || memcmp(record, verify, sizeof(*record)) != 0) {
            sSaveFailed[file] = 1;
            return 0;
        }
        sGeneration[file] = record->generation;
        sSelectedBank[file] = bank;
        sBanksToReplace[file] &= ~(1 << bank);
    } while (sBanksToReplace[file]);
    sDirty[file] = 0;
    sSaveFailed[file] = 0;
    return 1;
}

void game_timing_erase(s32 file) {
    if (valid_file(file)) {
        memset(sBest[file], 0, sizeof(sBest[file]));
        sDirty[file] = 1;
        sBanksToReplace[file] = TIMING_BOTH_BANKS_MASK;
        sLoadBlocked[file] = 0;
        if (file == sTiming.file) {
            game_timing_select_file(-1);
        }
    }
}

s32 game_timing_can_copy(s32 source) {
    return valid_file(source) && !sLoadBlocked[source];
}

void game_timing_copy(s32 source, s32 destination) {
    if (!game_timing_can_copy(source) || !valid_file(destination) || source == destination) {
        return;
    }
    memcpy(sBest[destination], sBest[source], sizeof(sBest[destination]));
    sDirty[destination] = 1;
    sBanksToReplace[destination] = TIMING_BOTH_BANKS_MASK;
    sLoadBlocked[destination] = 0;
    if (destination == sTiming.file) {
        game_timing_select_file(-1);
    }
}

s32 game_timing_storage_failed(s32 file) {
    return valid_file(file) && (sLoadBlocked[file] || sSaveFailed[file]);
}

void game_timing_format(u32 ticks, char output[24]) {
    u32 seconds = ticks / GAME_TIMING_TICKS_PER_SECOND;
    u32 hours = seconds / 3600;
    u32 minutes = seconds / 60 % 60;
    u32 fraction = ticks % GAME_TIMING_TICKS_PER_SECOND * 100 / GAME_TIMING_TICKS_PER_SECOND;
    char digits[10];
    s32 count = 0;
    do {
        digits[count++] = '0' + hours % 10;
        hours /= 10;
    } while (hours);
    char *p = output;
    while (count) {
        *p++ = digits[--count];
    }
    *p++ = ':';
    *p++ = '0' + minutes / 10;
    *p++ = '0' + minutes % 10;
    *p++ = ':';
    *p++ = '0' + seconds % 60 / 10;
    *p++ = '0' + seconds % 10;
    *p++ = '.';
    *p++ = '0' + fraction / 10;
    *p++ = '0' + fraction % 10;
    *p = '\0';
}
