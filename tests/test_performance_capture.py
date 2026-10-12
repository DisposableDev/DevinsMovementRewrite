"""Frame transport bounds, stream parsing, reset handling, and recorder shutdown."""

import importlib.util
import io
import os
from pathlib import Path
import re
import signal
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location("capture_play", ROOT / "tools/capture_play.py")
capture = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(capture)


def sample(sequence=0, delta=2, passive=False, **changes):
    fields = capture.PASSIVE_FIELDS if passive else capture.FIELDS
    marker = capture.PASSIVE_MARKER if passive else capture.MARKER
    row = dict.fromkeys(fields, 0)
    row.update(sequence=sequence, elapsed_vblanks=delta, vi_millihz=59940,
               player_ready=1, level=12, area=1, action=0x300024D6,
               collision_ticks=46875, x=-5118)
    if passive:
        row.update(sample_frames=15, max_elapsed_vblanks=2)
    row.update(changes)
    return marker + "".join(f"{row[name] & 0xffffffff:08x}" for name in fields) + "\n"


class PerformanceCaptureTests(unittest.TestCase):
    def test_research_destination_is_external_and_can_be_overridden(self):
        with tempfile.TemporaryDirectory() as directory:
            workspace = Path(directory)
            root = workspace / "sm64/hack"
            with patch.object(capture, "ROOT", root), patch.dict(os.environ, {}, clear=True):
                destination = capture.research_directory()
                self.assertEqual(destination, workspace / "research-archive/hack")
                self.assertFalse(destination.is_relative_to(root))
                self.assertFalse(destination.exists())
            custom = workspace / "custom research"
            with patch.dict(os.environ, {"SM64_RESEARCH_DIR": str(custom)}):
                self.assertEqual(capture.research_directory(), custom.resolve())
                self.assertFalse(custom.exists())

    def test_background_audio_mute_preserves_other_launch_arguments(self):
        arguments = ["--fullscreen", "--settings-file", "private.bml",
                     "--setting", "Audio/Mute=false", "--pseudofullscreen", "game.z64"]
        self.assertEqual(capture.ares_background_arguments(arguments),
                         ["--settings-file", "private.bml", "--setting", "Audio/Mute=false",
                          "game.z64", "--setting", "Audio/Mute=true"])
        with patch.object(capture, "ares_background_library", return_value=Path("/helper.so")):
            background = capture.launch_command("ares", Path("/game.z64"), True, True)
            foreground = capture.launch_command("ares", Path("/game.z64"), True, False)
        self.assertEqual(background[-3:], ["--setting", "Audio/Mute=true", "/game.z64"])
        self.assertNotIn("--pseudofullscreen", background)
        self.assertNotIn("Audio/Mute=true", foreground)
        self.assertIn("--pseudofullscreen", foreground)

    def test_configured_build_paths(self):
        for audio, suffix in (("10", ""), ("0", "_audio_trial_0")):
            for capture_mode, collision, mode in ((0, 0, ""), (1, 0, "_capture"),
                                                  (1, 1, "_capture_collision")):
                flags = ["VERSION=us", "CONSOLE=n64", f"AUDIO_TRIAL={audio}",
                         "AUDIO_QUALITY_TRIAL=0", "AUDIO_LINEAR_ENVELOPES=0", "AUDIO_LOW_LATENCY=1",
                         f"PERFORMANCE_CAPTURE={capture_mode}", f"COLLISION_PROFILING={collision}"]
                rom, elf, options = capture.build_paths(flags)
                directory = rom.parent
                self.assertEqual(directory.name, "us_n64" + suffix + mode)
                self.assertEqual(directory.parent, ROOT / "build")
                self.assertEqual(rom, directory / "sm64.z64")
                self.assertEqual(elf, directory / "sm64.elf")
                self.assertEqual(options["AUDIO_TRIAL"], audio)

    def test_no_build_launch_uses_resolved_rom(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            rom = root / "build/us_n64_audio_trial_10/sm64.z64"
            rom.parent.mkdir(parents=True)
            rom.write_bytes(b"existing ROM")
            with patch.object(capture, "ROOT", root), patch.object(
                    capture, "build_paths", return_value=(rom, rom.with_suffix(".elf"), {})) as paths, \
                    patch.object(capture, "ares_background_library", return_value=root / "background.so"), \
                    patch.object(capture.subprocess, "call", return_value=0) as launch:
                self.assertEqual(capture.main(["ares", "--no-capture", "--no-build"]), 0)
            self.assertIn("PERFORMANCE_CAPTURE=0", paths.call_args.args[0])
            self.assertEqual(launch.call_args.args[0][-1], str(rom))

    def test_build_path_resolution_failure(self):
        with patch.object(capture.subprocess, "run", return_value=subprocess.CompletedProcess(
                [], 2, stdout="", stderr="invalid build option")):
            with self.assertRaisesRegex(ValueError, "invalid build option"):
                capture.build_paths([])
        with patch.object(capture.subprocess, "run", return_value=subprocess.CompletedProcess(
                [], 0, stdout="unrelated output", stderr="")):
            with self.assertRaisesRegex(ValueError, "did not report"):
                capture.build_paths([])

    def test_ares_save_is_shared_without_overwriting_existing_files(self):
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            normal = directory / "normal/sm64.z64"
            normal.parent.mkdir()
            normal.with_suffix(".ram").write_bytes(b"original progress")
            rom = directory / "capture/sm64.z64"
            rom.parent.mkdir()
            target = capture.ares_save_path(rom, normal, "sram")
            self.assertEqual(target, str(normal.with_suffix(".ram")))
            save = rom.with_suffix(".ram")
            self.assertEqual(save.read_bytes(), b"original progress")
            save.write_bytes(b"new progress")
            self.assertEqual(normal.with_suffix(".ram").read_bytes(), b"new progress")
            save.unlink()
            save.write_bytes(b"separate existing save")
            capture.ares_save_path(rom, normal, "sram")
            self.assertEqual(save.read_bytes(), b"separate existing save")
            self.assertEqual(normal.with_suffix(".ram").read_bytes(), b"new progress")

    def test_sram_save_and_capture_only_progress(self):
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            normal = directory / "normal/sm64.z64"
            rom = directory / "capture/sm64.z64"
            rom.parent.mkdir()
            save = rom.with_suffix(".ram")
            save.write_bytes(b"capture-only progress")
            target = capture.ares_save_path(rom, normal, "sram")
            self.assertEqual(target, str(normal.with_suffix(".ram")))
            self.assertTrue(save.is_symlink())
            self.assertEqual(save.read_bytes(), b"capture-only progress")
            save.write_bytes(b"shared update")
            self.assertEqual(normal.with_suffix(".ram").read_bytes(), b"shared update")
            save.unlink()
            save.write_bytes(b"shared update")
            capture.ares_save_path(rom, normal, "sram")
            self.assertTrue(save.is_symlink())
            save.unlink()
            save.write_bytes(b"divergent progress")
            capture.ares_save_path(rom, normal, "sram")
            self.assertFalse(save.is_symlink())
            self.assertEqual(save.read_bytes(), b"divergent progress")
            self.assertEqual(normal.with_suffix(".ram").read_bytes(), b"shared update")

    def test_ares_rejects_unsupported_save_formats(self):
        for save_type in ("eep4k", "eep16k", "flashram"):
            with self.subTest(save_type=save_type), tempfile.TemporaryDirectory() as temporary:
                directory = Path(temporary)
                with self.assertRaisesRegex(ValueError, "require SRAM"):
                    capture.ares_save_path(directory / "capture/sm64.z64",
                                           directory / "normal/sm64.z64", save_type)
                self.assertEqual(list(directory.iterdir()), [])

    def test_completed_frame_is_exported_before_reset(self):
        profiler = Source(ROOT / "src/game/profiling.c")
        header = Source(ROOT / "src/game/profiling.h")
        definitions = (header.macros("PROFILING_BUFFER_SIZE|AUDIO_SUBSET_ENTRIES")
                       + header.declaration("enum", "ProfilerTime")
                       + re.search(r"typedef struct \{[^}]+\} ProfileTimeData;", header.text)[0])
        capture_header = (ROOT / "src/game/performance_capture.h").read_text()
        capture_header = re.sub(r"^#include.*\n", "", capture_header, flags=re.M)
        functions = "\n".join(profiler.function(name) for name in (
            "buffer_update", "profiler_clear_main_thread_samples", "profiler_capture_frame", "profiler_frame_setup"))
        fixture = r'''
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
typedef uint32_t u32;
typedef int32_t s32;
typedef float f32;
#define ARRAY_COUNT(a) (sizeof(a) / sizeof(*(a)))
#define PLAY_MODE_NORMAL 0
/* DEFINITIONS */
static ProfileTimeData all_profiling_data[PROFILER_TIME_COUNT];
static int profile_buffer_index = -1;
static u32 prev_fps_vblanks, fps_sample_count, gNumVblanks;
static f32 fps_vi_rate;
static u32 preempted_time, cur_start, prev_time, sUpdatedObjects, sSleepingObjects;
static u32 osGetCount(void) { return 100; }
static u32 __osDisableInt(void) { return 0; }
static void __osRestoreInt(u32 saved) { (void) saved; }
static f32 get_vi_refresh_rate(void) { return 60.0f; }
static int gCurrLevelNum = 12, gCurrAreaIndex = 1, sCurrPlayMode;
static void *gCurrDemoInput, *gCurrCreditsEntry;
static struct {
    u32 action, flags;
    f32 pos[3];
    void *marioObj;
} player, *gMarioState = &player;
static struct {
    u32 collision_floor, collision_wall, collision_ceil, collision_water, collision_raycast, matrix;
} gPuppyCallCounter;
static struct PerformanceCaptureSample captured;
static unsigned captures;
void performance_capture_frame(const struct PerformanceCaptureSample *sample, u32 start) {
    (void) start;
    captured = *sample;
    captures++;
}
/* FUNCTIONS */
int main(void) {
    profiler_frame_setup();
    assert(captures == 0);
    all_profiling_data[PROFILER_TIME_AUDIO].total = 300;
    all_profiling_data[PROFILER_TIME_TMEM].total = 9375;
    player.pos[0] = -5118.75f;
    player.marioObj = &player;
    player.action = 123;
    for (unsigned frame = 0; frame < 15; frame++) {
        buffer_update(&all_profiling_data[PROFILER_TIME_TOTAL], 777 + frame, profile_buffer_index);
        buffer_update(&all_profiling_data[PROFILER_TIME_COLLISION], 555, profile_buffer_index);
        sUpdatedObjects = 19;
        gPuppyCallCounter.collision_floor = 20;
        gNumVblanks += frame == 7 ? 5 : 2;
        profiler_frame_setup();
        assert(captures == (frame == 14));
    }
    assert(captures == 1 && profile_buffer_index == 0);
    assert(captured.timers[0] == 784 && captured.timers[12] == 555);
    assert(captured.peaks[0] == 791 && captured.peaks[12] == 555);
    assert(captured.timers[14] == 20);
    assert(captured.updatedObjects == 19 && captured.calls[0] == 20);
    assert(captured.ready == 1 && captured.position[0] == -5118 && captured.action == 123);
    assert(captured.elapsedVblanks == 33 && captured.viMillihz == 60000);
    assert(captured.maxElapsedVblanks == 5 && captured.sampleFrames == 15);
    assert(captured.timers[17] == 625);
    assert(sUpdatedObjects == 0);
    gCurrDemoInput = &player;
    for (unsigned frame = 0; frame < 15; frame++) {
        gNumVblanks += 2;
        profiler_frame_setup();
        gCurrDemoInput = NULL; // One demo frame invalidates the complete window.
    }
    assert(captures == 2 && captured.ready == 0);
    assert(captured.timers[0] == 0 && captured.updatedObjects == 0);
    return 0;
}
'''
        fixture = fixture.replace("/* DEFINITIONS */", definitions + "\n" + capture_header)
        fixture = fixture.replace("/* FUNCTIONS */", functions)
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            source = directory / "profiler.c"
            source.write_text(fixture)
            exe = directory / "profiler"
            subprocess.run([os.environ.get("CC", "cc"), "-std=c99", "-Wall", "-Wextra", "-Werror",
                            "-DPERFORMANCE_CAPTURE=1", "-DPUPPYPRINT_DEBUG", str(source), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)

    def test_parse_and_windows(self):
        self.assertIsNone(capture.parse_sample("unrelated emulator output"))
        row = capture.parse_sample("[CORE] IS64: " + sample())
        self.assertEqual(row["x"], -5118)
        self.assertEqual(capture.parse_sample(sample()), row)
        for line in ("SM64PERF1,1,2", sample(vi_millihz=0), sample(player_ready=2)):
            with self.assertRaises(ValueError):
                capture.parse_sample(line)
        output = io.StringIO()
        recorder = capture.Recording(output)
        for sequence in range(15):
            recorder.consume(sample(sequence))
        self.assertEqual(recorder.slow_frames[-1]["fps_15_frames"], 29.97)
        self.assertEqual(recorder.slow_frames[-1]["collision_us"], 1000)
        recorder.consume(sample(15, delta=20, play_mode=2))
        self.assertFalse(recorder.window)
        recorder.consume(sample(16, delta=0))
        self.assertFalse(recorder.window)
        recorder.consume(sample(18))
        self.assertEqual(recorder.gaps, 1)
        recorder.consume(sample(0))
        self.assertEqual(recorder.segment, 1)
        self.assertEqual(len(recorder.window), 1)
        recorder.consume("SM64PERF1,broken")
        self.assertEqual(recorder.malformed, 1)
        self.assertIn("29.97", recorder.summary())
        self.assertNotIn("2.997", recorder.summary())

    def test_passive_general_report_and_peak_retention(self):
        output = io.StringIO()
        recorder = capture.Recording(output)
        recorder.consume(sample(0, 33, passive=True, graph_ticks=468750,
                                graph_peak_ticks=1406250, cpu_main_ticks=703125,
                                cpu_main_peak_ticks=1875000, max_elapsed_vblanks=5,
                                rdp_pipe_ticks=62500, capture_ticks=46875))
        self.assertEqual((recorder.count, recorder.frame_count), (1, 15))
        row = recorder.slow_frames[0]
        self.assertAlmostEqual(row["fps"], 27.245, places=3)
        self.assertEqual(row["graph_peak_us"], 30000)
        self.assertEqual(row["rdp_pipe_us"], 1000)
        self.assertEqual(capture.largest_phase(row), "Rendering")
        self.assertEqual(capture.largest_phase(dict.fromkeys(capture.MICROSECOND_FIELDS, 0)), "Unresolved")
        self.assertEqual(recorder.gameplay_count, 15)
        report = recorder.summary()
        self.assertIn("Rendering", report)
        self.assertIn("30000.0", report)
        self.assertIn("66.7 us/frame", report)
        self.assertNotIn("Optional detailed collision", report)
        recorder.consume(sample(1, 30, passive=True, player_ready=0))
        self.assertEqual(recorder.gameplay_count, 15)

    def test_native_transport_bounds_and_order(self):
        header = (ROOT / "src/game/performance_capture.h").read_text()
        source = (ROOT / "src/game/performance_capture.c").read_text()
        header = re.sub(r"^#include.*\n", "", header, flags=re.M)
        source = re.sub(r"^#include.*\n", "", source, flags=re.M)
        fixture = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint32_t u32;
typedef int32_t s32;
typedef unsigned char u8;
static u32 osGetCount(void) { return 100; }
static unsigned char packet[4096];
static unsigned packets;
static void write_word(u32 address, u32 word) {
    if (address == 0xB3FF0014) {
        assert(word < sizeof(packet));
        packet[word] = 0;
        fputs((char *) packet, stdout);
        packets++;
    } else {
        u32 offset = address - 0xB3FF0020;
        assert(offset + 4 <= sizeof(packet));
        for (unsigned byte = 0; byte < 4; byte++) {
            packet[offset + byte] = word >> (24 - byte * 8);
        }
    }
}
#define IO_WRITE(address, word) write_word(address, word)
/* PRODUCTION */
int main(void) {
    struct PerformanceCaptureSample sample;
    memset(&sample, 0xff, sizeof(sample));
    sample.level = sample.area = sample.playMode = sample.ready = INT32_MIN;
    sample.position[0] = sample.position[1] = sample.position[2] = INT32_MIN;
    for (u32 i = 0; i < 8; i++) {
        performance_capture_frame(&sample, 90);
        assert(sCaptureBytes <= CAPTURE_ROW_BYTES * CAPTURE_BATCH_FRAMES);
        assert(sCaptureFrames == (i + 1) % CAPTURE_BATCH_FRAMES);
    }
    assert(packets == 8 / CAPTURE_BATCH_FRAMES);
    return 0;
}
'''
        fixture = fixture.replace("/* PRODUCTION */", header + source)
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            c_file = directory / "capture.c"
            c_file.write_text(fixture)
            exe = directory / "capture"
            for collision in (0, 1):
                subprocess.run([os.environ.get("CC", "cc"), "-std=c99", "-Wall", "-Wextra", "-Werror",
                                "-DPERFORMANCE_CAPTURE=1", f"-DCOLLISION_PROFILING={collision}",
                                "-fsanitize=address,undefined", str(c_file), "-o", str(exe)], check=True)
                result = subprocess.run([str(exe)], check=True, capture_output=True, text=True,
                                        env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})
                rows = []
                marker = capture.MARKER if collision else capture.PASSIVE_MARKER
                fields = capture.FIELDS if collision else capture.PASSIVE_FIELDS
                for line in result.stdout.splitlines():
                    # RMG's 512-byte formatter includes an "IS64: " prefix and NUL.
                    self.assertLess(len(line) + len("IS64: "), 512)
                    payload = line.split(marker)[1]
                    rows.append([payload[offset:offset + 8] for offset in range(0, len(payload), 8)])
                self.assertEqual(len(rows), 8)
                self.assertTrue(all(len(row) == len(fields) for row in rows))
                self.assertEqual([int(row[0], 16) for row in rows], list(range(8)))
                self.assertEqual(int(rows[0][-1], 16), 0)
                self.assertEqual(int(rows[1][-1], 16), 10)
                self.assertEqual(int(rows[0][10], 16), 2 ** 31)
                self.assertEqual(int(rows[0][13], 16), 2 ** 32 - 1)

    def test_stream_partial_lines_and_empty_capture(self):
        with tempfile.TemporaryDirectory() as temporary:
            folder = Path(temporary)
            child = folder / "child.py"
            child.write_text(
                "import os, sys\n"
                "fd = int(sys.argv[1].split('=')[1])\n"
                "os.close(fd)\n"
                f"os.write(1, {('noise\n[CORE] IS64: ' + sample(0) + sample(1).rstrip()).encode()!r})\n"
            )
            recorder, code, reason = capture.record_session([sys.executable, str(child)], folder, None)
            self.assertEqual((recorder.count, code, reason), (2, 0, "window_closed"))
            self.assertIn("frames", (folder / "summary.md").read_text())
            self.assertEqual(len((folder / "frames.csv").read_text().splitlines()), 3)
            child.write_text("import os, sys\nos.close(int(sys.argv[1].split('=')[1]))\n")
            recorder, code, reason = capture.record_session([sys.executable, str(child)], folder, None)
            self.assertEqual(recorder.count, 0)
            self.assertIn("No frame data", (folder / "summary.md").read_text())

    def test_duration_stops_owned_process(self):
        with tempfile.TemporaryDirectory() as temporary:
            folder = Path(temporary)
            child = folder / "child.py"
            child.write_text("import os, sys, time\n"
                             "os.close(int(sys.argv[1].split('=')[1]))\n"
                             f"os.write(1, {sample().encode()!r})\n"
                             "time.sleep(30)\n")
            recorder, code, reason = capture.record_session([sys.executable, str(child)], folder, 0.3)
            self.assertEqual(recorder.count, 1)
            self.assertLess(code, 0)
            self.assertEqual(reason, "duration")

    def test_instance_id_without_newline_is_used_before_pipe_closes(self):
        with tempfile.TemporaryDirectory() as temporary:
            folder = Path(temporary)
            child = folder / "child.py"
            child.write_text("import os, sys, time\n"
                             "os.write(int(sys.argv[1].split('=')[1]), str(os.getpid()).encode())\n"
                             f"os.write(1, {sample().encode()!r})\n"
                             "time.sleep(30)\n")
            kills = []

            def kill_instance(arguments, **kwargs):
                self.assertEqual(arguments[:2], ["flatpak", "kill"])
                kills.append(arguments[2])
                os.kill(int(arguments[2]), signal.SIGTERM)
                return subprocess.CompletedProcess(arguments, 0)

            with patch.object(capture.subprocess, "run", side_effect=kill_instance):
                recorder, code, reason = capture.record_session([sys.executable, str(child)], folder, 0.3)
            self.assertEqual(len(kills), 1)
            self.assertEqual(recorder.count, 1)
            self.assertLess(code, 0)
            self.assertEqual(reason, "duration")


if __name__ == "__main__":
    unittest.main()
