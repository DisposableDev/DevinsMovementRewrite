#!/usr/bin/env python3
"""Run complete timing/storage units for NTSC and PAL with controlled cartridge I/O."""
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]

FIXTURE = r'''
#include <assert.h>
#include <stdio.h>

static u8 cartridge[0x8000];
static s32 ready, writes, failAfter;
static u32 failedRead;

static s32 read_storage(u32 offset, void *data, u32 size) {
    assert(offset + size <= sizeof(cartridge));
    if (offset == failedRead) {
        return 1;
    }
    memcpy(data, cartridge + offset, size);
    return 0;
}

static s32 write_storage(u32 offset, const void *data, u32 size) {
    assert(offset >= PLAYER_STATS_LEGACY_SRAM_OFFSET && offset + size <= PLAYER_STATS_SRAM_OFFSET);
    if (failAfter == writes++) {
        return 1;
    }
    memcpy(cartridge + offset, data, size);
    return 0;
}

static s32 retired(s32 file) {
    assert(file >= 0 && file < GAME_TIMING_FILES);
    return ready;
}

static void reboot(void) {
    game_timing_init(read_storage, write_storage, retired);
}

static void visit(s32 file) {
    game_timing_select_file(file);
    game_timing_level_entered(COURSE_BOB, 9, 0);
}

static void timing_boundaries(void) {
    for (s32 course = COURSE_MIN; course < COURSE_CAKE_END; course++) {
        s32 stars = course <= COURSE_STAGES_MAX ? 7 : course == COURSE_PSS ? 2 : 1;
        assert(game_timing_star_count(course) == stars);
        game_timing_select_file(0);
        game_timing_level_entered(course, 9, 0);
        game_timing_update(1, 0, 1, 0);
        assert(!game_timing_collect_star(0, course, stars, 0, 0));
    }
    assert(!game_timing_star_count(COURSE_NONE));
    assert(!game_timing_star_count(COURSE_CAKE_END));
    assert(!game_timing_star_count(-1));
    visit(0);
    u32 unit = GAME_TIMING_TICKS_PER_UPDATE;
    game_timing_update(~0u, 0, 1, 0);
    game_timing_update(~0u, 0, 1, 0);
    game_timing_update(0, 0, 1, 0);
    assert(sTiming.sessionTicks == unit * 2 && sTiming.courseTicks == unit * 2);
    game_timing_update(1, 0, 0, 0);
    game_timing_update(2, 1, 1, 0);
    assert(sTiming.courseTicks == unit * 2);
    game_timing_level_entered(COURSE_BOB, 9, 0);
    assert(sTiming.courseTicks == unit * 2);
    game_timing_warp(9, 0);
    assert(sTiming.activeCourse);
    game_timing_disqualify_file(1);
    assert(sTiming.eligible);
    game_timing_disqualify_file(0);
    game_timing_update(3, 0, 1, 0);
    assert(!game_timing_collect_star(0, COURSE_BOB, 0, 0, 0));
    game_timing_end_course();
    u32 session = sTiming.sessionTicks;
    game_timing_level_entered(COURSE_BOB, 9, 0);
    assert(sTiming.eligible && sTiming.courseTicks == 0 && sTiming.sessionTicks == session);
    game_timing_update(4, 0, 1, 0);
    assert(!game_timing_collect_star(0, COURSE_BOB, 0, 1, 0));
    assert(!game_timing_collect_star(1, COURSE_BOB, 0, 0, 0));
    assert(!game_timing_collect_star(0, COURSE_WF, 0, 0, 0));
    assert(!game_timing_collect_star(0, COURSE_BOB, 7, 0, 0));
    assert(game_timing_collect_star(0, COURSE_BOB, 0, 0, 0));
    assert(game_timing_best(0, COURSE_BOB, 0) == unit);
    assert(!game_timing_collect_star(0, COURSE_BOB, 0, 0, 0));
    game_timing_update(5, 0, 0, 1);
    game_timing_update(5, 0, 1, 0);
    assert(!sTiming.eligible && !game_timing_collect_star(0, COURSE_BOB, 1, 0, 0));
    game_timing_level_entered(COURSE_WF, 24, 0);
    assert(sTiming.eligible && !sTiming.courseTicks);
    game_timing_update(6, 0, 1, 0);
    assert(game_timing_collect_star(0, COURSE_WF, 1, 0, 0));
    game_timing_warp(6, 1);
    assert(!sTiming.activeCourse && !sTiming.eligible);
    game_timing_select_file(0);
    assert(!sTiming.sessionTicks && !sTiming.courseTicks);
    game_timing_level_entered(COURSE_BOB, 9, 1);
    assert(!sTiming.eligible);
    game_timing_select_file(-1);
    game_timing_update(7, -1, 1, 0);
    assert(!sTiming.sessionTicks);
    visit(0);
    sTiming.sessionTicks = sTiming.courseTicks = GAME_TIMING_MAX_TICKS - 1;
    game_timing_update(8, 0, 1, 0);
    assert(sTiming.sessionTicks == GAME_TIMING_MAX_TICKS && sTiming.courseTicks == GAME_TIMING_MAX_TICKS
           && !sTiming.eligible);
    char text[24];
    game_timing_format(150u * (3600 + 60 + 1) + 149, text);
    assert(!strcmp(text, "1:01:01.99"));
    game_timing_format(GAME_TIMING_MAX_TICKS, text);
    assert(strlen(text) < sizeof(text));
}

static void backup_boundaries(void) {
    assert(!game_timing_checkpoint(0) && writes == 0);
    ready = 1;
    assert(game_timing_checkpoint(0) && writes == 2);
    reboot();
    assert(game_timing_best(0, COURSE_BOB, 0) == GAME_TIMING_TICKS_PER_UPDATE);
    // Both initial backups work independently.
    for (s32 bank = 0; bank < 2; bank++) {
        u32 offset = record_offset(0, bank);
        u8 byte = cartridge[offset];
        cartridge[offset] ^= 1;
        reboot();
        assert(game_timing_best(0, COURSE_BOB, 0) == GAME_TIMING_TICKS_PER_UPDATE);
        cartridge[offset] = byte;
    }
    reboot();
    game_timing_copy(0, 1);
    failAfter = writes + 1;
    assert(!game_timing_checkpoint(1));
    assert(sBanksToReplace[1] != 0 && sDirty[1]);
    failAfter = -1;
    assert(game_timing_checkpoint(1));
    reboot();
    assert(game_timing_best(1, COURSE_WF, 1) == GAME_TIMING_TICKS_PER_UPDATE);
    game_timing_erase(1);
    assert(game_timing_checkpoint(1));
    cartridge[record_offset(1, sSelectedBank[1])] ^= 1;
    reboot();
    assert(!game_timing_best(1, COURSE_BOB, 0));
    // A read error must not turn unread data into an empty replacement.
    failedRead = record_offset(0, 0);
    reboot();
    assert(game_timing_storage_failed(0));
    visit(0);
    game_timing_update(10, 0, 1, 0);
    assert(game_timing_collect_star(0, COURSE_BOB, 2, 0, 0));
    s32 before = writes;
    assert(!game_timing_checkpoint(0) && writes == before);
    failedRead = ~0u;
    reboot();
    assert(!game_timing_storage_failed(0) && !game_timing_best(0, COURSE_BOB, 2));
    // Sequence comparison must survive wrapping from UINT_MAX to zero.
    for (s32 bank = 0; bank < 2; bank++) {
        struct TimingRecord *r = &sScratch[bank];
        read_storage(record_offset(0, bank), r, sizeof(*r));
        r->generation = bank ? 0 : ~0u;
        r->times[0] = bank ? 150 : 300;
        r->checksum = record_checksum(r);
        write_storage(record_offset(0, bank), r, sizeof(*r));
    }
    reboot();
    assert(game_timing_best(0, COURSE_BOB, 0) == 150);
    struct TimingRecord *future = &sScratch[0];
    read_storage(record_offset(0, 1), future, sizeof(*future));
    future->version = TIMING_VERSION + 1;
    future->checksum = record_checksum(future);
    write_storage(record_offset(0, 1), future, sizeof(*future));
    reboot();
    assert(game_timing_storage_failed(0));
    visit(0); game_timing_update(20, 0, 1, 0);
    assert(game_timing_collect_star(0, COURSE_BOB, 2, 0, 0));
    before = writes;
    assert(!game_timing_checkpoint(0) && writes == before);
    game_timing_erase(0);
    assert(game_timing_checkpoint(0));
    reboot();
    assert(!game_timing_storage_failed(0) && !game_timing_best(0, COURSE_BOB, 0));
    assert(!game_timing_checkpoint(-1));
    printf("Timing/storage boundaries PASS; %u records/file, %zu bytes/bank, range 0x1000-0x4000.\n",
           TIMING_RECORD_COUNT, sizeof(struct TimingRecord));
}

static void copy_source_boundaries(void) {
    memset(cartridge, 0xFF, sizeof(cartridge));
    reboot();
    for (s32 file = 0; file < 2; file++) {
        visit(file);
        for (u32 frame = 1; frame <= (u32) file + 1; frame++) {
            game_timing_update(frame, file, 1, 0);
        }
        assert(game_timing_collect_star(file, COURSE_BOB, 0, 0, 0));
        assert(game_timing_checkpoint(file));
    }
    u8 before[sizeof(cartridge)];
    for (s32 fault = 0; fault < 2; fault++) {
        if (fault == 0) {
            failedRead = record_offset(0, 0);
        } else {
            struct TimingRecord future;
            read_storage(record_offset(0, 0), &future, sizeof(future));
            future.version = TIMING_VERSION + 1;
            future.checksum = record_checksum(&future);
            write_storage(record_offset(0, 0), &future, sizeof(future));
        }
        reboot();
        failedRead = ~0u;
        memcpy(before, cartridge, sizeof(before));
        s32 beforeWrites = writes;
        game_timing_copy(0, 1);
        assert(game_timing_best(1, COURSE_BOB, 0) == 2 * GAME_TIMING_TICKS_PER_UPDATE);
        assert(game_timing_checkpoint(1));
        assert(writes == beforeWrites);
        assert(memcmp(before, cartridge, sizeof(before)) == 0);
    }
    game_timing_erase(0);
    assert(game_timing_checkpoint(0));
    reboot();
    game_timing_copy(0, 1);
    assert(game_timing_checkpoint(1));
    reboot();
    assert(game_timing_best(1, COURSE_BOB, 0) == 0);
}

int main(void) {
    memset(cartridge, 0xFF, sizeof(cartridge));
    ready = writes = 0;
    failedRead = ~0u;
    failAfter = -1;
    reboot();
    timing_boundaries();
    backup_boundaries();
    copy_source_boundaries();
    return 0;
}
'''



class TimingTests(unittest.TestCase):
    def test_timing_and_backup_boundaries(self):
        for pal in (False, True):
            with self.subTest(pal=pal), tempfile.TemporaryDirectory(prefix="sm64-times-") as directory:
                d = Path(directory)
                (d / "PR").mkdir()
                (d / "PR/ultratypes.h").write_text("""#include <stdint.h>
typedef int8_t s8; typedef uint8_t u8; typedef int16_t s16; typedef uint16_t u16;
typedef int32_t s32; typedef uint32_t u32; typedef uint64_t u64; typedef float f32;
""")
                (d / "fixture.c").write_text('#include "src/game/game_timing.c"\n'
                                              + FIXTURE)
                command = ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                           "-fsanitize=undefined", "-fno-sanitize-recover=all",
                           "-I", str(d), "-I", str(ROOT), "-I", str(ROOT / "include"),
                           "-I", str(ROOT / "src/game"), str(d / "fixture.c"),
                           "-o", str(d / "fixture")]
                if pal:
                    command.insert(1, "-DVERSION_EU")
                subprocess.run(command, check=True, timeout=30)
                subprocess.run([str(d / "fixture")], check=True, timeout=10)


    def test_real_statistics_retirement_and_record_writes(self):
        fixture = r'''
#include <assert.h>
#include <string.h>
#include "game_timing.h"
#include "player_stats.h"
static u8 storage[0x8000];
static s32 writes,failAt=-1,legacyWrites;
static s32 read_storage(u32 offset,void *data,u32 size) {
    assert(offset+size<=sizeof(storage));memcpy(data,storage+offset,size);return 0;
}
static s32 write_storage(u32 offset,const void *data,u32 size) {
    assert(offset%16==0 && size%16==0 && offset+size<=sizeof(storage));
    if(writes++==failAt)return 1;
    if(offset<PLAYER_STATS_SRAM_OFFSET) {
        assert(offset>=PLAYER_STATS_LEGACY_SRAM_OFFSET);
        s32 file=(offset-PLAYER_STATS_LEGACY_SRAM_OFFSET)/(2*PLAYER_STATS_LEGACY_SAVE_STRIDE);
        for(s32 bank=0;bank<2;bank++) {
            u32 head[2];
            memcpy(head,storage+PLAYER_STATS_SRAM_OFFSET+(file*2+bank)*PLAYER_STATS_SAVE_STRIDE,sizeof(head));
            assert(head[0]==0x50535431u && head[1]==PLAYER_STATS_SAVE_VERSION);
        }
        legacyWrites++;
    }
    memcpy(storage+offset,data,size);return 0;
}
static u32 crc(const u8 *data,u32 size) {
    u32 value=~0u;
    for(u32 i=0;i<size;i++) {
        value^=data[i];
        for(s32 b=0;b<8;b++)value=(value>>1)^((value&1)?0xEDB88320u:0);
    }
    return ~value;
}
s32 game_options_is_cheating(void) { return 0; }
int main(void) {
    memset(storage,0xA5,sizeof(storage));
    u8 old[1312]={0};
    u32 header[]={0x50535431u,1,10,1272,0,0};
    u64 count=123;
    memcpy(old,header,sizeof(header));
    memcpy(old+24+PS_TOTAL_JUMPS_PERFORMED*8,&count,sizeof(count));
    u32 sum=crc(old,1296);memcpy(old+1296,&sum,sizeof(sum));
    memcpy(storage+PLAYER_STATS_LEGACY_SRAM_OFFSET,old,sizeof(old));
    player_stats_init(read_storage,write_storage);
    assert(gPlayerStats[0].values[PS_TOTAL_JUMPS_PERFORMED]==123);
    game_timing_init(read_storage,write_storage,player_stats_retire_legacy);
    game_timing_select_file(0);game_timing_level_entered(COURSE_BOB,9,0);
    game_timing_update(1,0,1,0);assert(game_timing_collect_star(0,COURSE_BOB,0,0,0));
    failAt=writes+1;
    assert(!game_timing_checkpoint(0) && legacyWrites==0);
    assert(!memcmp(storage+PLAYER_STATS_LEGACY_SRAM_OFFSET,old,sizeof(old)));
    failAt=-1;
    assert(game_timing_checkpoint(0) && legacyWrites==3);
    player_stats_init(read_storage,write_storage);
    game_timing_init(read_storage,write_storage,player_stats_retire_legacy);
    assert(gPlayerStats[0].values[PS_TOTAL_JUMPS_PERFORMED]==123);
    assert(game_timing_best(0,COURSE_BOB,0)==GAME_TIMING_TICKS_PER_UPDATE);
    player_stats_copy(0,2);game_timing_copy(0,2);assert(game_timing_checkpoint(2));
    assert(gPlayerStats[2].values[PS_TOTAL_JUMPS_PERFORMED]==123);
    player_stats_erase(2);game_timing_erase(2);assert(game_timing_checkpoint(2));
    player_stats_init(read_storage,write_storage);
    game_timing_init(read_storage,write_storage,player_stats_retire_legacy);
    assert(gPlayerStats[0].values[PS_TOTAL_JUMPS_PERFORMED]==123);
    assert(!gPlayerStats[2].values[PS_TOTAL_JUMPS_PERFORMED] && !game_timing_best(2,COURSE_BOB,0));
    return 0;
}
'''
        for pal in (False, True):
            with self.subTest(pal=pal), tempfile.TemporaryDirectory(prefix="sm64-times-migration-") as directory:
                d = Path(directory); (d / "PR").mkdir()
                (d / "PR/ultratypes.h").write_text("""#ifndef MIGRATION_TYPES
#define MIGRATION_TYPES
#include <stdint.h>
typedef int8_t s8; typedef uint8_t u8; typedef int16_t s16; typedef uint16_t u16;
typedef int32_t s32; typedef uint32_t u32; typedef uint64_t u64; typedef float f32;
#endif
""")
                (d / "fixture.c").write_text(fixture)
                command = ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                           "-fsanitize=undefined", "-fno-sanitize-recover=all", "-I", str(d),
                           "-I", str(ROOT / "src/game"), "-I", str(ROOT / "include"),
                           "-I", str(ROOT), str(d / "fixture.c"),
                           str(ROOT / "src/game/game_timing.c"),
                           str(ROOT / "src/game/player_stats.c"), "-o", str(d / "fixture")]
                if pal: command.insert(1, "-DVERSION_EU")
                subprocess.run(command, check=True, timeout=30)
                subprocess.run([str(d / "fixture")], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
