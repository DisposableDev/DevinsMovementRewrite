"""Production profiler ring samples, paused frames and object counters on the host."""
from pathlib import Path
import os
import re
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class ProfilerSamplesTests(unittest.TestCase):
    def test_samples(self):
        header = Source(ROOT / "src/game/profiling.h")
        source = Source(ROOT / "src/game/profiling.c")
        profile_type = re.search(r"typedef struct \{[^}]+\} ProfileTimeData;", header.text)
        self.assertIsNotNone(profile_type)
        definitions = (header.macros("PROFILING_BUFFER_SIZE|AUDIO_SUBSET_ENTRIES|AUDIO_SUBSET_SIZE")
                       + header.declaration("enum", "ProfilerTime")
                       + header.declaration("enum", "ProfilerDeltaTime") + profile_type[0])
        functions = "\n".join(source.function(name) for name in (
            "buffer_update", "profiler_update", "profiler_get_delta", "profiler_count_object",
            "profiler_clear_main_thread_samples", "profiler_frame_setup", "profiler_get_fps", "update_total_timer",
            "profiler_audio_started", "profiler_audio_waited", "profiler_audio_completed"))
        functions += "\n#ifdef PUPPYPRINT_DEBUG\n" + "\n".join(
            source.function(name) for name in (
                "profiler_collision_clock", "profiler_audio_wait_started",
                "profiler_collision_reset", "profiler_collision_update")) + "\n#endif\n"
        object_updates = Source(ROOT / "src/game/object_list_processor.c").function("update_non_terrain_objects")
        print_body = source.region(r"^void profiler_print_times", r"^void profiler_count_object")
        print_size = re.search(r"char text_buffer\[(\d+)\];", print_body)[1]
        object_size = re.search(r"char object_buffer\[(\d+)\];", print_body)[1]
        print_start = print_body.index("sprintf(text_buffer,")
        print_end = print_body.index("Gfx* dlHead", print_start)
        print_call = print_body[print_start:print_end]
        code = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint32_t u32;
typedef int32_t s32;
typedef float f32;
#define ARRAY_COUNT(a) (sizeof(a) / sizeof(*(a)))
/* DEFINITIONS */
static ProfileTimeData all_profiling_data[PROFILER_TIME_COUNT];
static int profile_buffer_index = -1;
static u32 prev_fps_vblanks, fps_sample_count, gNumVblanks;
static f32 fps_vi_rate;
static u32 preempted_time, cur_start, prev_time, ticks, collision_time;
static u32 audio_start, audio_buffer_index;
#ifdef PUPPYPRINT_DEBUG
static u32 sAudioCpuTicks, sAudioCpuStart;
#endif
#ifdef AUDIO_PROFILING
static u32 audio_subset_starts[AUDIO_SUBSET_SIZE], audio_subset_tallies[AUDIO_SUBSET_SIZE];
#endif
static u32 interrupt_audio_ticks;
static u32 sUpdatedObjects, sSleepingObjects;
static u32 osGetCount(void) { return ticks; }
static u32 __osDisableInt(void) { return 0; }
static void __osRestoreInt(u32 saved) {
    (void) saved;
    if (interrupt_audio_ticks != 0) {
        u32 duration = interrupt_audio_ticks;
        interrupt_audio_ticks = 0;
        audio_start = ticks;
        ticks += duration;
        preempted_time += duration;
#ifdef PUPPYPRINT_DEBUG
        sAudioCpuTicks += duration;
#endif
    }
}
static f32 get_vi_refresh_rate(void) { return 60.0f; }
/* FUNCTIONS */
#define USE_PROFILER
#define PROFILER_GET_SNAPSHOT_TYPE(type) u32 first = profiler_get_delta(type)
#define OBJ_LIST_PLAYER 1
#define OBJ_LIST_DEFAULT 2
#define TIME_STOP_ACTIVE 1
static const int sObjectListUpdateOrder[] = {0, 0, 0, OBJ_LIST_PLAYER, OBJ_LIST_DEFAULT, -1};
static int gObjectLists[3], gObjectCounter, gTimeStopState;
static int update_objects_in_list(int *list) {
    int index = list - gObjectLists;
    ticks += (index + 1) * 100 + 30;
    collision_time += 30;
    return 1;
}
static void compact_explosion_bubbles_update(void) { ticks += 25; }
/* OBJECT_UPDATES */
static void test_object_attribution(void) {
    memset(all_profiling_data, 0, sizeof(all_profiling_data));
    profile_buffer_index = 0;
    ticks = prev_time = 1000;
    preempted_time = 0;
    collision_time = 90; // Earlier terrain work must not affect these intervals.
    update_non_terrain_objects();
    assert(all_profiling_data[PROFILER_TIME_BEHAVIOR_BEFORE_MARIO].counts[0] == 100);
    assert(all_profiling_data[PROFILER_TIME_MARIO].counts[0] == 200);
    assert(all_profiling_data[PROFILER_TIME_BEHAVIOR_AFTER_MARIO].counts[0] == 325);
    assert(collision_time == 180);
    profile_buffer_index = -1;
}
static void fill_sample(enum ProfilerTime which, u32 value) {
    for (int i = 0; i < PROFILING_BUFFER_SIZE; i++) {
        all_profiling_data[which].counts[i] = value;
    }
    all_profiling_data[which].total = value * PROFILING_BUFFER_SIZE;
}
static void test_pause_resume(void) {
    for (int which = PROFILER_TIME_CONTROLLERS; which < PROFILER_TIME_COUNT; which++) {
        fill_sample(which, 100 + which);
    }
    u32 audio_total = all_profiling_data[PROFILER_TIME_AUDIO].total;
    u32 rsp_total = all_profiling_data[PROFILER_TIME_RSP_GFX].total;
    u32 rdp_total = all_profiling_data[PROFILER_TIME_PIPE].total;
    profiler_frame_setup();
    assert(all_profiling_data[PROFILER_TIME_MARIO].counts[0] == 0);
    assert(all_profiling_data[PROFILER_TIME_BEHAVIOR_BEFORE_MARIO].counts[0] == 0);
    assert(all_profiling_data[PROFILER_TIME_BEHAVIOR_AFTER_MARIO].counts[0] == 0);
    assert(all_profiling_data[PROFILER_TIME_OBJECT_PLATFORM_SETUP].counts[0] == 0);
    assert(all_profiling_data[PROFILER_TIME_OBJECT_CLEANUP].counts[0] == 0);
    profiler_count_object(0);
    profiler_count_object(0);
    profiler_count_object(1);
    assert(sUpdatedObjects == 2 && sSleepingObjects == 1);
    for (int frame = 0; frame < PROFILING_BUFFER_SIZE * 2; frame++) {
        gNumVblanks += 2;
        ticks += 33333;
        profiler_frame_setup();
        assert(sUpdatedObjects == 0 && sSleepingObjects == 0);
    }
    assert(all_profiling_data[PROFILER_TIME_MARIO].total == 0);
    assert(all_profiling_data[PROFILER_TIME_BEHAVIOR_BEFORE_MARIO].total == 0);
    assert(all_profiling_data[PROFILER_TIME_BEHAVIOR_AFTER_MARIO].total == 0);
    assert(all_profiling_data[PROFILER_TIME_OBJECT_PLATFORM_SETUP].total == 0);
    assert(all_profiling_data[PROFILER_TIME_OBJECT_CLEANUP].total == 0);
    assert(all_profiling_data[PROFILER_TIME_AUDIO].total == audio_total);
    assert(all_profiling_data[PROFILER_TIME_RSP_GFX].total == rsp_total);
    assert(all_profiling_data[PROFILER_TIME_PIPE].total == rdp_total);
    assert(profiler_get_fps() == 30.0f);
#ifdef AUDIO_PROFILING
    assert(all_profiling_data[PROFILER_TIME_SUB_AUDIO_SYNTHESIS].total != 0);
#endif
    ticks += 1000;
    profiler_update(PROFILER_TIME_BEHAVIOR_BEFORE_MARIO, 0);
    assert(all_profiling_data[PROFILER_TIME_BEHAVIOR_BEFORE_MARIO].total == 1000);
    // A second pause flushes the resumed sample instead of preserving it forever.
    for (int frame = 0; frame < PROFILING_BUFFER_SIZE; frame++) {
        gNumVblanks += 2;
        ticks += 33333;
        profiler_frame_setup();
    }
    assert(all_profiling_data[PROFILER_TIME_BEHAVIOR_BEFORE_MARIO].total == 0);
}
static void test_total_timer(void) {
    profile_buffer_index = 0;
    cur_start = prev_time = 1000;
    preempted_time = 0;
    ticks = 1100;
    u32 excluded = 0;
#ifdef PUPPYPRINT_DEBUG
    // These sections supply their measured duration, not an enum identifier.
    profiler_update(PROFILER_TIME_PUPPYPRINT1, 80);
    ticks = 1300;
    preempted_time = 50;
    profiler_update(PROFILER_TIME_PUPPYPRINT2, 120);
    assert(all_profiling_data[PROFILER_TIME_PUPPYPRINT1].counts[0] == 80);
    assert(all_profiling_data[PROFILER_TIME_PUPPYPRINT2].counts[0] == 70);
    excluded = 150;
#endif
    ticks = 2000;
    update_total_timer();
    assert(all_profiling_data[PROFILER_TIME_TOTAL].counts[0] == ticks - cur_start - excluded);
    // Re-reading a total has no effect on recorded debug durations.
    ticks += 100;
    update_total_timer();
    assert(all_profiling_data[PROFILER_TIME_TOTAL].counts[0] == ticks - cur_start - excluded);
}
static void audio_interval(u32 start, u32 end, u32 wait) {
    ticks = start;
    profiler_audio_started();
#ifdef PUPPYPRINT_DEBUG
    profiler_audio_wait_started();
#endif
    ticks += wait;
    profiler_audio_waited(wait);
    ticks = end;
    profiler_audio_completed();
}
static void test_collision_audio_clock(void) {
#ifdef PUPPYPRINT_DEBUG
    ticks = 1000;
    sAudioCpuTicks = 0;
    profiler_collision_reset();
    u32 start = profiler_collision_clock();
    audio_interval(1100, 1400, 0);
    audio_interval(1450, 1550, 0);
    ticks = 2000;
    profiler_collision_update(start);
    assert(collision_time == 600);

    // A collision query may start during a sample-DMA wait. The CPU segment
    // before that query must already be excluded from both clock snapshots.
    ticks = 2100;
    profiler_audio_started();
    ticks = 2200;
    u32 wait_start = profiler_audio_wait_started();
    ticks = 2300;
    start = profiler_collision_clock();
    ticks = 2400;
    profiler_audio_waited(ticks - wait_start);
    ticks = 2500;
    profiler_audio_completed();
    ticks = 2600;
    profiler_collision_reset();
    profiler_collision_update(start);
    assert(collision_time == 200);

    // Delivery just after restoring interrupts belongs inside the query.
    ticks = 3000;
    interrupt_audio_ticks = 100;
    start = profiler_collision_clock();
    ticks += 150;
    profiler_collision_reset();
    profiler_collision_update(start);
    assert(collision_time == 150);

    ticks = UINT32_MAX - 100;
    sAudioCpuTicks = UINT32_MAX - 50;
    start = profiler_collision_clock();
    audio_interval(UINT32_MAX - 80, 19, 0);
    ticks = 99;
    profiler_collision_reset();
    profiler_collision_update(start);
    assert(collision_time == 100);
#endif
}
static void test_audio_preemptions(void) {
    memset(all_profiling_data, 0, sizeof(all_profiling_data));
    profile_buffer_index = audio_buffer_index = 0;
    cur_start = prev_time = 1000;
    preempted_time = 0;
    audio_interval(1100, 1300, 40);
    audio_interval(1400, 1700, 60);
    assert(preempted_time == 400);
    assert(all_profiling_data[PROFILER_TIME_AUDIO].counts[0] == 160);
    assert(all_profiling_data[PROFILER_TIME_AUDIO].counts[1] == 240);
    ticks = 2000;
    profiler_update(PROFILER_TIME_GFX, 0);
    assert(all_profiling_data[PROFILER_TIME_GFX].counts[0] == 600);
    assert(preempted_time == 0 && cur_start == 1400);
    ticks = 2100;
    profiler_update(PROFILER_TIME_CAMERA, 0);
    assert(all_profiling_data[PROFILER_TIME_CAMERA].counts[0] == 100);
    ticks = 2200;
    update_total_timer();
    assert(all_profiling_data[PROFILER_TIME_TOTAL].counts[0] == 800);

    // An audio interrupt after the atomic snapshot belongs to the next interval.
    ticks = 2300;
    interrupt_audio_ticks = 50;
    profiler_update(PROFILER_TIME_CAMERA, 0);
    assert(preempted_time == 50);
    ticks = 2500;
    profiler_update(PROFILER_TIME_CAMERA, 0);
    assert(all_profiling_data[PROFILER_TIME_CAMERA].counts[0] == 150);
    assert(cur_start == 1450);

    // Total-timer handoff must also consume a newly delivered interrupt once.
    audio_interval(2510, 2560, 0);
    interrupt_audio_ticks = 30;
    ticks = 2700;
    update_total_timer();
    assert(all_profiling_data[PROFILER_TIME_TOTAL].counts[0] == 1200);
    assert(preempted_time == 0 && cur_start == 1530);
    ticks = 2800;
    update_total_timer();
    assert(all_profiling_data[PROFILER_TIME_TOTAL].counts[0] == 1270);

    // Unsigned subtraction preserves intervals across the Count register wrap.
    preempted_time = 0;
    cur_start = prev_time = UINT32_MAX - 100;
    audio_interval(UINT32_MAX - 80, 19, 20);
    ticks = 99;
    profiler_update(PROFILER_TIME_GFX, 0);
    assert(all_profiling_data[PROFILER_TIME_GFX].counts[0] == 120);
    assert(preempted_time == 0);
    preempted_time = 50;
    profiler_frame_setup();
    assert(preempted_time == 0 && prev_time == ticks && cur_start == ticks);
    interrupt_audio_ticks = 40;
    profiler_frame_setup();
    assert(preempted_time == 40 && prev_time == ticks - 40);
    ticks += 60;
    profiler_update(PROFILER_TIME_CAMERA, 0);
    assert(all_profiling_data[PROFILER_TIME_CAMERA].counts[profile_buffer_index] == 60);
}
static void test_text_capacity(void) {
    char text_buffer[/* PRINT_SIZE */];
    char object_buffer[/* OBJECT_SIZE */];
    u32 microseconds[PROFILER_TIME_COUNT];
    for (int i = 0; i < PROFILER_TIME_COUNT; i++) {
        microseconds[i] = 1073741823;
    }
    u32 total_cpu = 2147483647, total_rsp = 2147483647, max_rdp = 2147483647;
    sUpdatedObjects = sSleepingObjects = 240;
    /* PRINT_CALL */
    assert(strlen(text_buffer) < sizeof(text_buffer));
    assert(strlen(object_buffer) < sizeof(object_buffer));
    // The fast font advances 12 pixels per line, with 12-pixel glyphs.
    // Count occupied lines; the trailing newline does not draw another row.
    int lines = 0;
    for (const char *p = text_buffer; *p; p++) {
        if (*p == '\n') lines++;
    }
    assert(8 + lines * 12 <= 240);
    assert(strstr(text_buffer, "Awake/Sleep") == NULL);
    assert(strstr(object_buffer, "Awake/Sleep") != NULL);
    memset(microseconds, 0, sizeof(microseconds));
    microseconds[PROFILER_TIME_SPAWNER] = 13;
    microseconds[PROFILER_TIME_DYNAMIC] = 17;
    microseconds[PROFILER_TIME_BEHAVIOR_BEFORE_MARIO] = 7;
    microseconds[PROFILER_TIME_BEHAVIOR_AFTER_MARIO] = 11;
    microseconds[PROFILER_TIME_OBJECT_COLLISION] = 888;
    microseconds[PROFILER_TIME_OBJECT_PLATFORM_SETUP] = 123;
    microseconds[PROFILER_TIME_OBJECT_PLATFORM_PREPARE] = 456;
    microseconds[PROFILER_TIME_OBJECT_CLEANUP] = 777;
    /* PRINT_CALL */
#ifdef PUPPYPRINT_DEBUG
    assert(strstr(text_buffer, "Behavior\t\t48\n") != NULL);
#else
    assert(strstr(text_buffer, "Behavior\t\t31\n") != NULL);
#endif
    assert(strstr(object_buffer, "Obj pairs: 888\nPlatforms: 579\nCleanup: 777\n") != NULL);
}
int main(void) {
    test_object_attribution();
    test_pause_resume();
    test_total_timer();
    test_audio_preemptions();
    test_collision_audio_clock();
    test_text_capacity();
    puts("profiler: skipped samples expire, audio/RSP clocks survive, counters reset, FPS stays separate");
    return 0;
}
'''
        code = (code.replace("/* DEFINITIONS */", definitions)
                .replace("/* FUNCTIONS */", functions)
                .replace("/* OBJECT_UPDATES */", object_updates)
                .replace("/* PRINT_SIZE */", print_size)
                .replace("/* OBJECT_SIZE */", object_size)
                .replace("/* PRINT_CALL */", print_call))
        with tempfile.TemporaryDirectory(prefix="profiler-samples-") as directory:
            path = Path(directory)
            (path / "test.c").write_text(code)
            for debug in (False, True):
                subprocess.run(["cc", "-std=c99", "-O1", "-Wall", "-Wextra", "-Werror",
                                "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                                *(["-DPUPPYPRINT_DEBUG", "-DAUDIO_PROFILING"] if debug else []),
                                str(path / "test.c"), "-o", str(path / "test")], check=True)
                subprocess.run([str(path / "test")], check=True,
                               env=dict(os.environ, ASAN_OPTIONS="detect_leaks=0"))


if __name__ == "__main__":
    unittest.main()
