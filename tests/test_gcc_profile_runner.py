"""Automatic profiling must isolate saves/data and reject incomplete routes."""

from pathlib import Path
import sys
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import profile_gcc as profile
import capture_play as capture


class ProfileRunnerTests(unittest.TestCase):
    def test_private_settings_preserve_audio_processing_and_clear_input(self):
        original = ("Audio\n  Driver: SDL\n  Mute: false\n  Volume: 1.0\n"
                    "Input\n  Defocus: Pause\n  A: controller/0/1/Hi;;\n"
                    "General\n  AutoSaveMemory: true\n  AwaitGDBClient: false\n"
                    "  HomebrewMode: false\nPaths\n  Saves: /normal/saves\n"
                    "DebugServer\n  Port: 9123\n  Enabled: false\n  UseIPv4: false\n")
        changed = profile.private_settings(original, 12345, Path("/private/saves"))
        self.assertIn("  Driver: SDL\n  Mute: true\n  Volume: 1.0", changed)
        self.assertIn("  Saves: /private/saves", changed)
        self.assertIn("  Port: 12345\n  Enabled: true\n  UseIPv4: true", changed)
        self.assertIn("  A: ;;", changed)
        self.assertIn("/normal/saves", original)

    def test_missing_settings_fail_before_launch(self):
        with self.assertRaises(ValueError):
            profile.private_settings("Audio\n", 12345, Path("/private"))

    def test_route_requires_full_window_and_real_movement(self):
        rows = [{"age": age, "area": 1, "position": [0, 0, (age - 90) * 3]}
                for age in range(90, 256)]
        profile.validate_route("lab", rows)
        with self.assertRaises(RuntimeError):
            profile.validate_route("lab", rows[:-1])
        for row in rows:
            row["position"] = [0, 0, 0]
        with self.assertRaises(RuntimeError):
            profile.validate_route("lab", rows)

    def test_jrb_rejects_land_or_surface_route(self):
        rows = [{"age": age, "area": 1, "action": 0x380022c0,
                 "position": [0, 600, 5000], "water_level": 1000}
                for age in range(90, 256)]
        profile.validate_route("jrb-swim", rows)
        rows[10]["action"] = 0x0c400201
        with self.assertRaises(RuntimeError):
            profile.validate_route("jrb-swim", rows)
        rows[10]["action"] = 0x380022c0
        rows[10]["position"][1] = 950
        with self.assertRaises(RuntimeError):
            profile.validate_route("jrb-swim", rows)

    def test_changed_source_is_never_marked_current(self):
        with patch.object(profile, "source_identity", return_value={"collision.c": "new"}):
            with self.assertRaises(RuntimeError):
                profile.require_unchanged({"collision.c": "old"})

    def test_freshness_tracks_build_inputs_and_ignores_unrelated_docs(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            (root / "src").mkdir()
            (root / "tools").mkdir()
            files = {"Makefile": "tools/textconv.py", "src/collision.c": "code",
                     "tools/textconv.py": "converter", "tools/benchmark.py": "other task",
                     "AGENTS.md": "instructions"}
            for name, text in files.items():
                (root / name).write_text(text)
            names = b"\0".join(name.encode() for name in files) + b"\0"
            with patch.object(profile, "ROOT", root), \
                    patch.object(profile.subprocess, "check_output", return_value=names):
                first = profile.source_identity()
                self.assertEqual(set(first), {"Makefile", "src/collision.c", "tools/textconv.py"})
                (root / "AGENTS.md").write_text("new instructions")
                self.assertEqual(profile.source_identity(), first)
                (root / "src/collision.c").write_text("changed code")
                self.assertNotEqual(profile.source_identity(), first)

    def test_compiler_settings_changes_invalidate_incremental_build(self):
        with tempfile.TemporaryDirectory() as temporary:
            stamp = Path(temporary) / "build.json"
            settings = {"compiler": "16.2", "flags": ["GCC_PROFILE=1"]}
            self.assertTrue(profile.needs_full_rebuild(stamp, settings))
            stamp.write_text('{"compiler": "16.2", "flags": ["GCC_PROFILE=1"]}')
            self.assertFalse(profile.needs_full_rebuild(stamp, settings))
            self.assertTrue(profile.needs_full_rebuild(stamp, {**settings, "compiler": "other"}))
            stamp.write_text("incomplete")
            self.assertTrue(profile.needs_full_rebuild(stamp, settings))

    def test_report_relocation_cannot_write_to_the_live_build(self):
        expected = Path("/source/build/collision.gcda")
        filename = str(expected).encode() + b"\0"
        payload = b"gcfnB62*" + len(filename).to_bytes(4, "big") + filename
        with tempfile.TemporaryDirectory() as temporary:
            reports = Path(temporary)
            target = profile.prepare_report_directory(payload, reports, expected)
            self.assertEqual(target, reports / "source/build/collision.gcda")
            self.assertTrue(target.parent.is_dir())
            with self.assertRaises(ValueError):
                profile.prepare_report_directory(payload, reports, Path("/wrong/collision.gcda"))

    def test_parallel_jobs_respect_available_cpus(self):
        with patch.object(capture.os, "sched_getaffinity", return_value={2, 3}):
            self.assertEqual(capture.default_build_jobs(), 2)
        with patch.object(capture.os, "sched_getaffinity", side_effect=OSError), \
                patch.object(capture.os, "cpu_count", return_value=None):
            self.assertEqual(capture.default_build_jobs(), 1)

    def test_manual_launch_rebuilds_with_the_requested_job_limit(self):
        with tempfile.TemporaryDirectory() as temporary:
            rom = Path(temporary) / "sm64.z64"
            rom.write_bytes(b"ROM")
            with patch.object(capture, "build_paths", return_value=(rom, rom.with_suffix(".elf"), {})), \
                    patch.object(capture, "launch_command", return_value=["emulator"]), \
                    patch.object(capture.subprocess, "run", return_value=SimpleNamespace(returncode=0)) as build, \
                    patch.object(capture.subprocess, "call", return_value=0):
                self.assertEqual(capture.main(["ares", "--no-capture", "--jobs", "4"]), 0)
            self.assertEqual(build.call_args.args[0][:2], ["make", "-j4"])

    def test_export_rejects_missing_chunks_and_incorrect_checksum(self):
        payload = b"profile bytes"
        checksum = 2166136261
        for byte in payload:
            checksum = ((checksum ^ byte) * 16777619) & 0xffffffff
        lines = ["SM64GCOV1,BEGIN", f"SM64GCOV1,DATA,00000000,{payload.hex()}",
                 f"SM64GCOV1,END,{len(payload):08x},{checksum:08x}"]
        self.assertEqual(profile.decode(lines), payload)
        with self.assertRaises(ValueError):
            profile.decode([lines[0], lines[1].replace("00000000", "00000001"), lines[2]])
        with self.assertRaises(ValueError):
            profile.decode(lines[:2] + [f"SM64GCOV1,END,{len(payload):08x},00000000"])


if __name__ == "__main__":
    unittest.main()
