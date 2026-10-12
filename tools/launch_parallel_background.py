#!/usr/bin/env python3
"""Run a private Parallel Launcher or RetroArch capture below other X11 windows.

Usage: python3 tools/launch_parallel_background.py EXECUTABLE [arguments...]
Callers must select private settings/saves, disable fullscreen and focus-loss
pausing, and silence frontend audio. This wrapper preserves their environment
and blocks focus/raise requests in the launcher and its emulator children.
"""

import hashlib
import os
from pathlib import Path
import subprocess
import sys
import tempfile


def background_environment():
    source = Path(__file__).with_name("emulator_background.c")
    folder = source.parent.parent / "build" / "launcher"
    folder.mkdir(parents=True, exist_ok=True)
    digest = hashlib.sha256(source.read_bytes()).hexdigest()[:16]
    library = folder / f"emulator-background-{digest}.so"
    if not library.is_file():
        with tempfile.TemporaryDirectory(dir=folder) as temporary:
            output = Path(temporary) / library.name
            subprocess.run(["cc", "-shared", "-fPIC", "-O2", "-Wall", "-Wextra", "-Werror",
                            str(source), "-o", str(output), "-ldl", "-lX11", "-lxcb"], check=True)
            output.replace(library)
    environment = os.environ.copy()
    previous = environment.get("LD_PRELOAD")
    environment["LD_PRELOAD"] = str(library) + (":" + previous if previous else "")
    environment.pop("DESKTOP_STARTUP_ID", None)
    environment.pop("XDG_ACTIVATION_TOKEN", None)
    return environment


def main():
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    executable, *arguments = sys.argv[1:]
    os.execvpe(executable, [executable, *arguments], background_environment())


if __name__ == "__main__":
    main()
