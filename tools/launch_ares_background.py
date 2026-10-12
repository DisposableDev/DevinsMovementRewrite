#!/usr/bin/env python3
"""Run a native ares executable silently without requesting desktop focus.

Usage: python3 tools/launch_ares_background.py /path/to/ares [ares arguments...]
The caller's runtime libraries, process group, output and debugger settings stay
intact because this wrapper replaces itself with the emulator.
"""

import os
import sys

from capture_play import ares_background_arguments, ares_background_environment


def main():
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    executable, *arguments = sys.argv[1:]
    arguments = ares_background_arguments(arguments)
    os.execvpe(executable, [executable, *arguments], ares_background_environment())


if __name__ == "__main__":
    main()
