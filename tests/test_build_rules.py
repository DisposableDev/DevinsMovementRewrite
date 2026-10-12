#!/usr/bin/env python3
"""Check build dependencies and setup failures in private host-only fixtures."""

from pathlib import Path
import shutil
import shlex
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class BuildRuleTests(unittest.TestCase):
    def test_production_dry_run_is_read_only_and_complete(self):
        compiler = Path("/opt/n64-gcc-16.2/bin/mips64-elf-gcc")
        if not compiler.is_file():
            self.skipTest("GCC 16.2 is needed for read-only level-rule preprocessing")
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            for path in ROOT.iterdir():
                if path.name in {".git", "build", ".quality-tools", "tools", "Makefile", "Makefile.split"}:
                    continue
                (root / path.name).symlink_to(path, target_is_directory=path.is_dir())
            (root / "tools").mkdir()
            for path in (ROOT / "tools").iterdir():
                (root / "tools" / path.name).symlink_to(path, target_is_directory=path.is_dir())
            for name in ("Makefile", "Makefile.split"):
                shutil.copyfile(ROOT / name, root / name)
            before = {path.relative_to(root) for path in root.rglob("*")}
            prefix = str(compiler).removesuffix("gcc")
            cases = (
                ["-n", "VERSION=us"], ["--dry-run", "VERSION=eu"], ["-n", "VERSION=sh"],
                ["-n", "-j2", "all"], ["-n", "patch"], ["-n", "clean"],
                ["-n", "rebuildtools"],
            )
            for arguments in cases:
                with self.subTest(arguments=arguments):
                    result = subprocess.run(
                        ["make", "--no-print-directory", "N64_TOOLCHAIN_PREFIX=" + prefix,
                         "PYTHON=/bin/false", *arguments],
                        cwd=root, capture_output=True, text=True)
                    self.assertEqual(result.returncode, 0, result.stderr)
                    self.assertNotIn("warning:", result.stderr)
                    self.assertNotIn("DRY RUNS ARE DISABLED", result.stdout)
                    self.assertEqual(before, {path.relative_to(root) for path in root.rglob("*")})
                    if "clean" not in arguments and "rebuildtools" not in arguments:
                        self.assertIn(str(compiler), result.stdout)
                        self.assertIn(" -c ", result.stdout)
                        pack = [shlex.split(line) for line in result.stdout.splitlines()
                                if "mips64-elf-objcopy --pad-to=" in line]
                        self.assertEqual(len(pack), 1)
                        self.assertTrue(any(argument.endswith("sm64.elf") for argument in pack[0]))
                        self.assertTrue(any(argument.endswith("sm64.z64") for argument in pack[0]))
                    if "patch" in arguments:
                        self.assertIn("$(/bin/false tools/detect_baseroms.py us)", result.stdout)

    def test_host_tool_dependencies(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            tools = root / "tools"
            tools.mkdir()
            shutil.copyfile(ROOT / "util.mk", root / "util.mk")
            shutil.copyfile(ROOT / "tools/Makefile", tools / "Makefile")
            (tools / "audiofile").mkdir()
            (tools / "audiofile/Makefile").write_text("clean:\n\t@true\n")
            sources = {
                "lz4tpack.c": '#include "lz4hc.c"\nint main(void) { return value(); }\n',
                "lz4hc.c": '#include "lz4.h"\nstatic int value(void) { return VALUE; }\n',
                "lz4.h": '#define VALUE 0\n',
                "n64cksum.c": '#include "n64cksum.h"\nint main(void) { return VALUE; }\n',
                "n64cksum.h": '#define VALUE 0\n',
                "utils.c": '#include "utils.h"\nint utility(void) { return VALUE; }\n',
                "utils.h": '#define VALUE 0\n',
                "textconv.c": '#include "hashtable.h"\nint main(void) { return VALUE; }\n',
                "hashtable.c": '#include "hashtable.h"\nint table(void) { return VALUE; }\n',
                "hashtable.h": '#define VALUE 0\n',
                "utf8.c": '#include "utf8.h"\nint utf8(void) { return VALUE; }\n',
                "utf8.h": '#define VALUE 0\n',
            }
            for name, content in sources.items():
                (tools / name).write_text(content)
            targets = ["lz4tpack", "n64cksum", "textconv"]
            command = ["make", "--no-print-directory", "-C", str(tools)]
            built = subprocess.run(command + ["-j2", *targets], capture_output=True, text=True)
            self.assertEqual(built.returncode, 0, built.stderr)
            self.assertNotIn("jobserver unavailable", built.stderr)
            self.assertEqual(subprocess.run(command + ["-q", *targets]).returncode, 0)
            for target, dependency in (
                ("lz4tpack", "lz4hc.c"), ("lz4tpack", "lz4.h"),
                ("n64cksum", "n64cksum.h"), ("n64cksum", "utils.h"),
                ("textconv", "hashtable.h"), ("textconv", "utf8.h"),
            ):
                with self.subTest(target=target, dependency=dependency):
                    self.assertEqual(subprocess.run(
                        command + ["-q", "-W", dependency, target]).returncode, 1)
            # Existing binaries from before dependency tracking migrate on the next build.
            (tools / "lz4tpack.d").unlink()
            self.assertEqual(subprocess.run(command + ["-q", "lz4tpack"]).returncode, 1)
            cleaned = subprocess.run(command + ["clean"], capture_output=True, text=True)
            self.assertEqual(cleaned.returncode, 0, cleaned.stderr)
            self.assertFalse(list(tools.glob("*.d")))

    def test_setup_failures_stop_make(self):
        source = (ROOT / "Makefile").read_text()
        directory_start = source.index("# Make sure build directory exists")
        directory_end = source.index("$(BUILD_DIR)/include/text_strings.h:", directory_start)
        lighting_end = source.index("$(BUILD_DIR)/%.o: %.c")
        lighting_start = source.rindex("ifeq ($(QUALITY_CONFIG_ONLY)$(DRY_RUN),)", 0, lighting_end)
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            (root / "blocked").write_text("not a directory")
            (root / "lighting.py").write_text(
                "import sys\nprint('conversion output')\n"
                "sys.exit(3 if sys.argv[1] == sys.argv[2] else 0)\n")
            for failure, expected in (
                ("directories", "Failed to create build directories"),
                ("actors", "Failed to convert actor lighting"),
                ("levels", "Failed to convert level lighting"),
            ):
                with self.subTest(failure=failure):
                    fixture = "ALL_DIRS := blocked/us\n" + source[directory_start:directory_end]
                    if failure != "directories":
                        fixture = ("FIXLIGHTS := 1\nPYTHON := python3\n"
                                   f"FIXLIGHTS_PY := lighting.py {failure}\n"
                                   + source[lighting_start:lighting_end])
                    (root / "Makefile").write_text(fixture + "all:\n\t@true\n")
                    result = subprocess.run(["make", "-s"], cwd=root,
                                            capture_output=True, text=True)
                    self.assertNotEqual(result.returncode, 0)
                    self.assertIn(expected, result.stderr)

    def test_lighting_search_distinguishes_empty_from_failure(self):
        with tempfile.TemporaryDirectory() as temporary:
            empty = Path(temporary) / "empty"
            empty.mkdir()
            command = ["python3", str(ROOT / "tools/fixlights.py")]
            self.assertEqual(subprocess.run(command + [str(empty)],
                                           capture_output=True).returncode, 0)
            self.assertNotEqual(subprocess.run(command + [str(empty / "missing")],
                                              capture_output=True).returncode, 0)


if __name__ == "__main__":
    unittest.main()
