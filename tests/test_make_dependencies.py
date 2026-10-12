"""Check that missing dependency files cannot leave stale model objects cached."""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class MissingDependenciesTest(unittest.TestCase):
    def test_cached_object_rebuilds_once_and_tracks_includes(self):
        makefile = (ROOT / "Makefile").read_text()
        start = makefile.index("# A cached object without its dependency file")
        end = makefile.index("\n#==============================================================================#", start)
        recovery = makefile[start:end]
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            build = work / "build"
            build.mkdir()
            header = work / "model.h"
            header.write_text("#define MODEL_VALUE 7\n")
            (work / "model.c").write_text('#include "model.h"\nint model = MODEL_VALUE;\n')
            (work / "Makefile").write_text(
                "default: $(BUILD_DIR)/model.o\n"
                "BUILD_DIR := build\n"
                "O_FILES := build/model.o\n"
                "GODDARD_O_FILES :=\n"
                "DEP_FILES := build/model.d\n" + recovery +
                "\n$(BUILD_DIR)/%.o: %.c\n"
                "\t$(CC) -c -MMD -MF $(BUILD_DIR)/$*.d -o $@ $<\n"
                "-include $(DEP_FILES)\n"
            )

            def run_make():
                subprocess.run(["make", "build/model.o"], cwd=work,
                               check=True, capture_output=True)

            run_make()
            obj = build / "model.o"
            original = obj.read_bytes()
            # Reproduce a cached object whose include dependencies were lost.
            (build / "model.d").unlink()
            header.write_text("#define MODEL_VALUE 19\n")
            os.utime(header, (1, 1))
            run_make()
            self.assertNotEqual(obj.read_bytes(), original)
            self.assertIn("model.h", (build / "model.d").read_text())
            stamp = obj.stat().st_mtime_ns
            run_make()
            self.assertEqual(obj.stat().st_mtime_ns, stamp)
            # Once regenerated, the normal dependency must notice model changes.
            header.write_text("#define MODEL_VALUE 23\n")
            os.utime(obj, (1, 1))
            cached = obj.read_bytes()
            run_make()
            self.assertNotEqual(obj.read_bytes(), cached)


if __name__ == "__main__":
    unittest.main()
