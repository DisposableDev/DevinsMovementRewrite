#!/usr/bin/env python3
"""Compatibility entry point; see run_regressions.py for all suites/options."""
import sys
from run_regressions import main

if __name__ == "__main__":
    raise SystemExit(main(["--suite", "punch", *sys.argv[1:]]))
