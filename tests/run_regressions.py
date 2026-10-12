#!/usr/bin/env python3
"""Fast host regression tests. Exit 0: pass; 1: behavior failure; 2: harness error."""
import argparse
import os
from pathlib import Path
import re
import shlex
import subprocess
import sys
import tempfile
import time

from regression_support import HarnessError, Source
from regression_preparation import PREPARERS
from regression_mutations import mutations, resolve_overrides
import case_catalog

HERE = Path(__file__).resolve().parent
SUITES = {
    "zero_health": "zero_health_actions_test.c",
    "debug_warp": "debug_warp_test.c",
    "end_screen": "end_screen_test.c",
    "doors": "door_intent_test.c",
    "rotation": "animation_rotation_test.c",
    "platform": "object_platform_motion_test.c",
    "he_thicc": "he_thicc_moves_test.c",
    "rumble": "character_rumble_test.c",
    "character_save": "character_save_test.c",
    "character": "character_text_test.c",
    "punch": "punch_sequence_test.c",
    "animation": "animation_resume_test.c",
    "audio": "sound_cancellation_test.c",
    "swimming": "swimming_collision_test.c",
    "swim_animation": "swim_animation_test.c",
    "landing": "landing_movement_test.c",
    "bounce": "enemy_bounce_test.c",
    "fixes": "bugfix_transitions_test.c",
    "geometry": "geometry_transitions_test.c",
    "render": "aquarium_render_test.c",
    "wallpose": "wall_kick_animation_test.c",
    "twirl": "twirl_contact_test.c",
    "twirl_speed": "twirl_speed_test.c",
    "sideflip": "side_flip_timing_test.c",
    "air_backward": "air_backward_speed_test.c",
    "jump_kick_wall": "jump_kick_wall_test.c",
    "metal": "metal_contact_test.c",
    "bowser": "bowser_moves_test.c",
    "shell": "underwater_shell_test.c",
    "switches": "switch_reenable_test.c",
    "one_up": "one_up_collision_test.c",
    "clam": "clam_hitbox_test.c",
    "hud": "hud_visibility_test.c",
}


def prepare(root, suite, temp, overrides):
    def source(path):
        return Source(root / path, overrides.get(path))

    definitions, functions = [], []
    PREPARERS[suite](root, source, definitions, functions)
    if re.search(r"\b(?:sins|coss|sincos)_bhaskara\s*\(", "\n".join(definitions + functions)):
        trig = source("src/engine/bhaskara_trig.inc.c")
        definitions.append(trig.mapped(0, len(trig.text)))
    (temp / f"{suite}_definitions.inc").write_text("\n".join(definitions))
    (temp / f"{suite}_sources.inc").write_text("\n".join(functions))


class RunStats:
    def __init__(self):
        self.seconds = dict.fromkeys(("prepare", "compile", "execute"), 0.0)
        self.counts = dict.fromkeys(self.seconds, 0)

    def measure(self, phase, operation, *arguments, **keywords):
        started = time.monotonic()
        try:
            return operation(*arguments, **keywords)
        finally:
            self.seconds[phase] += time.monotonic() - started
            self.counts[phase] += 1

    def report(self):
        for phase in self.seconds:
            print(f"TIMING {phase}: {self.seconds[phase]:.2f}s; {self.counts[phase]} calls")


def run_suite(root, suite, case=None, listing=False, overrides=None, quiet=False, stats=None):
    """Compile once, then run selected cases or list their names.

    Executing case=None queries the suite, then runs each case with its own deadline.
    Temporary sources and the executable are removed on return or failure.
    """
    stats = stats or RunStats()
    cases = [case] if isinstance(case, str) or case is None else list(dict.fromkeys(case))
    if not cases:
        raise HarnessError(f"{suite}: no cases selected")
    if "" in cases:
        raise HarnessError(f"{suite}: empty case selector")
    with tempfile.TemporaryDirectory(prefix=f"sm64-{suite}-tests-") as directory:
        temp = Path(directory)
        stats.measure("prepare", prepare, root, suite, temp, overrides or {})
        executable = temp / "test"
        try:
            compiler = shlex.split(os.environ.get("CC", "cc"))
        except ValueError as error:
            raise HarnessError(f"Invalid CC setting: {error}") from error
        if not compiler:
            raise HarnessError("CC must name a host C compiler")
        command = compiler + [
            "-std=c99", "-Wall", "-Wextra", "-Werror", "-Wno-unused-parameter",
            "-I", str(temp), "-I", str(HERE), "-I", str(root),
            str(HERE / SUITES[suite]),
            "-o", str(executable), "-lm"]
        compiled = stats.measure("compile", subprocess.run, command,
                                 capture_output=True, text=True, timeout=30)
        if compiled.returncode:
            raise HarnessError(f"{suite}: host compilation failed (not a gameplay verdict)\n{compiled.stderr}")
        if not listing and cases == [None]:
            available = stats.measure("execute", subprocess.run, [str(executable), "--list"],
                                      capture_output=True, text=True, timeout=10)
            cases = available.stdout.splitlines()
            if available.returncode or not cases or any(
                    not name.startswith(suite + ".") for name in cases):
                raise HarnessError(f"{suite}: invalid case list\n{available.stderr}")
        results = []
        for selected_case in ([None] if listing else cases):
            if listing:
                arguments = ["--list"]
            elif selected_case:
                arguments = ["--case", selected_case]
            else:
                arguments = []
            result = stats.measure("execute", subprocess.run, [str(executable), *arguments],
                                   capture_output=True, text=True, timeout=10)
            if result.returncode not in (0, 1):
                raise HarnessError(f"{suite}: test adapter/runtime error ({result.returncode})\n{result.stderr}")
            if not quiet:
                print(result.stdout, end="", flush=True)
                print(result.stderr, end="", file=sys.stderr)
            results.append(result)
        return subprocess.CompletedProcess(
            [str(executable)], max(result.returncode for result in results),
            "".join(result.stdout for result in results), "".join(result.stderr for result in results))


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--suite", choices=SUITES, action="append", help="select a suite; repeatable")
    parser.add_argument("--case", action="append", help="exact case name; repeat to batch cases per suite")
    parser.add_argument("--list", action="store_true", help="list stable case names")
    parser.add_argument("--match", help="with --list, show only names containing all query words")
    parser.add_argument("--refresh-case-catalog", action="store_true",
                        help="refresh case names from fixture source without compiling")
    parser.add_argument("--check-case-catalog", action="store_true",
                        help="check catalog freshness without compiling")
    parser.add_argument("--verify-case-catalog", action="store_true",
                        help="compare catalog names with compiled fixture listings")
    parser.add_argument("--self-check", action="store_true", help="verify tests reject known defects in temporary copies")
    parser.add_argument("--mutation", action="append", help="select an exact mutation name; repeatable")
    parser.add_argument("--list-mutations", action="store_true", help="list mutation names without reading source or compiling")
    parser.add_argument("--timings", action="store_true", help="report preparation, compilation and execution costs")
    parser.add_argument("--source-root", type=Path, default=HERE.parent)
    args = parser.parse_args(argv)
    if args.match is not None and not args.list:
        parser.error("--match requires --list")
    catalog_modes = [args.refresh_case_catalog, args.check_case_catalog, args.verify_case_catalog]
    if sum(catalog_modes) > 1 or (any(catalog_modes) and (args.list or args.self_check
                                                       or args.list_mutations or args.mutation)):
        parser.error("catalog modes cannot combine with another discovery or mutation mode")
    if args.list and (args.self_check or args.list_mutations):
        parser.error("--list cannot combine with --self-check or --list-mutations")
    if args.mutation and not (args.self_check or args.list_mutations):
        parser.error("--mutation requires --self-check or --list-mutations")
    suites = list(dict.fromkeys(args.suite or SUITES))
    cases = list(dict.fromkeys(args.case or []))
    for case in cases:
        if case.split(".")[0] not in suites:
            parser.error("case prefix must match a selected suite; use --list")
    if cases:
        suites = list(dict.fromkeys(case.split(".")[0] for case in cases))
    root = args.source_root.resolve()
    started = time.monotonic()
    failures = passes = 0
    stats = RunStats()
    try:
        if args.refresh_case_catalog:
            case_catalog.refresh(HERE, SUITES)
            print("PASS: case catalog refreshed from source; no compilation.")
            return 0
        if args.list or args.check_case_catalog or args.verify_case_catalog:
            available = case_catalog.read(HERE, SUITES, suites)
            missing = set(cases) - {name for names in available.values() for name in names}
            if missing:
                raise HarnessError("no case matched: " + ", ".join(sorted(missing)))
            if args.verify_case_catalog:
                for suite in suites:
                    result = run_suite(root, suite, listing=True, quiet=True, stats=stats)
                    if result.returncode or result.stdout.splitlines() != available[suite]:
                        raise HarnessError(f"{suite}: compiled listing differs from the case catalog")
                print("PASS: catalog matches compiled fixture listings.")
            elif args.check_case_catalog:
                print("PASS: case catalog is fresh; no compilation.")
            else:
                terms = args.match.lower().split() if args.match else []
                shown = 0
                for names in available.values():
                    for name in names:
                        if (not cases or name in cases) and all(term in name.lower() for term in terms):
                            print(name)
                            shown += 1
                if not shown:
                    raise HarnessError("no catalog case matched the supplied filters")
            return 0
        selected_mutations = []
        if args.self_check or args.list_mutations:
            selected_mutations = list(mutations(root, suites, cases=cases or None,
                                                names=args.mutation, materialize=False))
            missing = set(args.mutation or []) - {item[1] for item in selected_mutations}
            if missing:
                raise HarnessError("no selected mutation matched: " + ", ".join(sorted(missing)))
            if not selected_mutations:
                raise HarnessError("no mutations matched the selected suites/cases")
            if args.list_mutations:
                for _, name, case, _ in selected_mutations:
                    print(f"{name}: {case}")
                return 0
            if args.mutation and not cases:
                cases = list(dict.fromkeys(item[2] for item in selected_mutations))
                suites = list(dict.fromkeys(item[0] for item in selected_mutations))
        for suite in suites:
            suite_cases = [case for case in cases if case.split(".")[0] == suite] or None
            result = run_suite(root, suite, suite_cases, args.list, stats=stats)
            failures += result.returncode != 0
            passes += sum(line.startswith("PASS ") for line in result.stdout.splitlines())
        if args.self_check and not failures:
            for suite, name, case, overrides in selected_mutations:
                overrides = stats.measure("prepare", resolve_overrides, overrides)
                result = run_suite(root, suite, case, overrides=overrides, quiet=True, stats=stats)
                if result.returncode == 1 and f"FAIL {case}\n" in result.stdout:
                    print(f"DETECTED {name}: {case}")
                else:
                    print(f"SELF-CHECK FAILED {name}: expected {case} to reject the mutation")
                    failures += 1
        if not args.list:
            print(f"{passes} cases passed; {failures} suite/self-check failures; "
                  f"{time.monotonic() - started:.2f}s. Host tests only; no ROM built.")
        return 1 if failures else 0
    except (HarnessError, OSError, subprocess.TimeoutExpired, UnicodeError) as error:
        print(f"HARNESS ERROR: {error}", file=sys.stderr)
        return 2
    finally:
        if args.timings:
            stats.report()


if __name__ == "__main__":
    raise SystemExit(main())
