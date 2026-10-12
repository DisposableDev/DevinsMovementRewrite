"""Scene validity, isolated settings, complete-window comparisons and snapshots."""

import argparse
import contextlib
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import benchmark


def sample(sequence, **changes):
    fields = benchmark.capture.PASSIVE_FIELDS
    row = dict.fromkeys(fields, 0)
    row.update(sequence=sequence, sample_frames=15, elapsed_vblanks=30, vi_millihz=59940,
               max_elapsed_vblanks=2, level=9, area=1, player_ready=1,
               cpu_main_ticks=46875, cpu_main_peak_ticks=93750)
    row.update(changes)
    return benchmark.capture.PASSIVE_MARKER + "".join(
        f"{row[key] & 0xffffffff:08x}" for key in fields)


class BenchmarkTests(unittest.TestCase):
    def test_scene_presets_and_invalid_input(self):
        for name in json.loads((ROOT / "tools/benchmark_scenes.json").read_text()):
            self.assertEqual(benchmark.load_scene(name)["name"], name)
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "invalid.json"
            scene = benchmark.load_scene("bob-standing")
            scene["inputs"] = [{"from": 1, "until": 10, "stick": [64, 64]}]
            path.write_text(json.dumps(scene))
            with self.assertRaisesRegex(ValueError, "magnitude"):
                benchmark.load_scene(str(path))

    def test_private_settings_preserve_audio_and_clear_live_inputs(self):
        original = ("Input\n  Defocus: Pause\n  Device: pad;;\nBoot\n"
                    "  AwaitGDBClient: false\nGeneral\n  AutoSaveMemory: true\n"
                    "  Rewind: false\n  RunAhead: false\n  ForceInterpreter: false\n"
                    "Audio\n  Blocking: true\n  Mute: false\nPaths\n  Saves\n"
                    "DebugServer\n  Port: 9123\n  Enabled: false\n  UseIPv4: false\n")
        output = benchmark.isolated_settings(original, 1234, Path("/private/saves"))
        self.assertIn("  Defocus: Allow", output)
        self.assertIn("  Device: ;;", output)
        self.assertIn("  Blocking: true", output)
        self.assertIn("  Mute: false", output)
        self.assertIn("  Saves: /private/saves/", output)
        self.assertIn("  ForceInterpreter: false", output)
        self.assertIn("  Defocus: Pause", original)
        with self.assertRaisesRegex(ValueError, "missing"):
            benchmark.isolated_settings("Audio\n  Mute: false\n", 1234, Path("/private"))

    def test_complete_capture_and_window_rejection(self):
        scene = {"area": 1, "sample_frames": 30}
        result = {"level": 9, "sequence_start": 1, "sequence_end": 3}
        text, metrics = benchmark.parse_capture("\n".join(sample(i) for i in range(4)), result, scene)
        self.assertEqual(len(text.splitlines()), 3)
        self.assertEqual(metrics["cpu_main_us"], 1000)
        self.assertEqual(metrics["cpu_main_peak_us"], 2000)
        self.assertAlmostEqual(metrics["fps"], 29.97)
        with self.assertRaisesRegex(ValueError, "complete"):
            benchmark.parse_capture(sample(1), result, scene)
        with self.assertRaisesRegex(ValueError, "duplicate"):
            benchmark.parse_capture(sample(1) + "\n" + sample(1), result, scene)
        with self.assertRaisesRegex(ValueError, "scene window"):
            benchmark.parse_capture(sample(1, play_mode=2) + "\n" + sample(2), result, scene)

    def test_state_mismatch_blocks_performance_claim(self):
        control = {"state_sha256": "a", "image_sha256": "x", "metrics": {"cpu_main_us": 1000}}
        candidate = {"state_sha256": "b", "image_sha256": "x", "metrics": {"cpu_main_us": 500}}
        comparison = benchmark.compare([(control, candidate)])
        self.assertFalse(comparison["matched"])
        self.assertIn("mismatch", comparison["decision"])
        candidate["state_sha256"] = "a"
        self.assertTrue(benchmark.compare([(control, candidate)])["matched"])

    def test_freeze_and_fork_do_not_share_writable_files(self):
        with tempfile.TemporaryDirectory() as folder, contextlib.redirect_stdout(io.StringIO()):
            root = Path(folder)
            source = root / "live"
            source.mkdir()
            (source / "Makefile").write_text("source\n")
            (source / "baserom.us.z64").write_bytes(b"input ROM")
            (source / "build").mkdir()
            (source / "build/output").write_text("must not freeze")
            control, candidate = root / "control", root / "candidate"
            benchmark.freeze(argparse.Namespace(source=str(source), output=str(control)))
            self.assertFalse((control / "source/build").exists())
            self.assertEqual((control / "source/baserom.us.z64").read_bytes(), b"input ROM")
            (control / "source/build").mkdir()
            (control / "source/build/reusable.o").write_bytes(b"cached object")
            benchmark.fork(argparse.Namespace(control=str(control), output=str(candidate)))
            self.assertEqual((candidate / "source/build/reusable.o").read_bytes(), b"cached object")
            (candidate / "source/Makefile").write_text("candidate\n")
            self.assertEqual((control / "source/Makefile").read_text(), "source\n")
            (control / "source/Makefile").write_text("changed control\n")
            with self.assertRaisesRegex(ValueError, "changed"):
                benchmark.fork(argparse.Namespace(control=str(control), output=str(root / "bad")))

    def test_flag_change_invalidates_cached_build(self):
        with tempfile.TemporaryDirectory() as folder, contextlib.redirect_stdout(io.StringIO()):
            trial = Path(folder)
            source = trial / "source"
            source.mkdir()
            (trial / "inputs.json").write_text("{}")
            (source / "Makefile").write_text("fixture\n")
            compiler = trial / "mips64-elf-gcc"
            compiler.write_bytes(b"compiler identity")
            calls = []

            def make(command, **kwargs):
                output = source / "build"
                calls.append((output / "stale.o").exists())
                output.mkdir(exist_ok=True)
                (output / "sm64.z64").write_bytes(b"ROM")
                (output / "sm64.elf").write_bytes(b"ELF")

            def information(command, **kwargs):
                if command[0] == "make":
                    return "BUILD_ROM=build/sm64.z64\nBUILD_ELF=build/sm64.elf\n"
                return "tool version\n"

            args = argparse.Namespace(trial=str(trial), jobs=1, make_option=[])
            with patch.object(benchmark, "COMPILER", compiler), \
                    patch.object(benchmark, "WORKSPACE_CCACHE", trial / "absent"), \
                    patch.object(benchmark.shutil, "which", return_value=None), \
                    patch.object(benchmark.subprocess, "run", side_effect=make), \
                    patch.object(benchmark.subprocess, "check_output", side_effect=information):
                benchmark.build(args)
                (source / "build/stale.o").write_bytes(b"old flags")
                benchmark.build(args)
                args.make_option = ["GENERAL_OPTIMIZATION_LEVEL=-O2"]
                benchmark.build(args)
            self.assertEqual(calls, [False, True, False])


if __name__ == "__main__":
    unittest.main()
