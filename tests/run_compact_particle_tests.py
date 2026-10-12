#!/usr/bin/env python3
"""Host lifecycle tests using the complete compact bubble implementation."""
from pathlib import Path
import os
import re
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def prepare_fixture(directory):
    """Share actual implementation inputs with the host runner and parsed checks."""
    source = (ROOT / "src/game/compact_explosion_bubbles.c").read_text()
    source = re.sub(r"^#include.*\n", "", source, flags=re.M)
    (directory / "compact_source.inc").write_text(source)


def main():
    with tempfile.TemporaryDirectory(prefix="compact-particles-") as directory:
        directory = Path(directory)
        prepare_fixture(directory)
        command = shlex.split(os.environ.get("CC", "cc")) + [
            "-std=c11", "-O1", "-Wall", "-Wextra", "-Werror",
            "-I" + str(directory), str(ROOT / "tests/compact_explosion_bubble_test.c"),
            "-lm", "-o", str(directory / "compact-test")]
        result = subprocess.run(command, text=True, capture_output=True)
        if result.returncode:
            print("HARNESS ERROR: " + result.stderr)
            return 2
        result = subprocess.run([str(directory / "compact-test")], text=True, capture_output=True)
        print(result.stdout, end="")
        if result.stderr:
            print(result.stderr, end="")
        return result.returncode


if __name__ == "__main__":
    raise SystemExit(main())
