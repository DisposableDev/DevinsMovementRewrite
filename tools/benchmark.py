#!/usr/bin/env python3
"""Freeze/build private trials and replay matched scenes in background Ares.

Run --help or see llm/docs/ai-benchmarking.md. Uses only the Python standard library.
"""

import argparse
import csv
import fcntl
import hashlib
import io
import json
import math
import os
from pathlib import Path
import re
import shutil
import signal
import socket
import statistics
import subprocess
import sys
import time

import capture_play as capture


ROOT = Path(__file__).resolve().parents[1]
COMPILER = Path("/opt/n64-gcc-16.2/bin/mips64-elf-gcc")
WORKSPACE_CCACHE = ROOT.parents[1] / "n64-tools/ccache/bin/ccache"
DEFAULT_SETTINGS = Path.home() / ".var/app/dev.ares.ares/data/ares/settings.bml"
LIMITS = ("Ares native CPU/RSP emulator timings; Vulkan RDP is not console RDP timing. "
          "This runner does not export calibrated cache/device transfers; those and total "
          "RAMbus traffic are unavailable. Use the calibrated diagnostic when needed. "
          "Main, audio and RSP timings overlap and must not be added.")


def save_json(path, data):
    path.write_text(json.dumps(data, indent=2, sort_keys=True) + "\n")


def load_scene(name):
    path = Path(name)
    if path.is_file():
        scene = json.loads(path.read_text())
        if not isinstance(scene, dict):
            raise ValueError("Scene must be a JSON object")
        scene.setdefault("name", path.stem)
    else:
        presets = json.loads((ROOT / "tools/benchmark_scenes.json").read_text())
        if name not in presets:
            raise ValueError("Unknown scene: " + name)
        scene = {**presets[name], "name": name}
    if not isinstance(scene, dict):
        raise ValueError("Scene must be a JSON object")
    allowed = {"description", "name", "level", "area", "node", "act", "seed", "setup_frame",
               "sample_start", "sample_frames", "setup", "mario", "anchor", "camera", "inputs",
               "events", "checks", "observe_behaviors", "debug_overlay"}
    if set(scene) - allowed:
        raise ValueError("Unknown scene keys: " + ", ".join(sorted(set(scene) - allowed)))
    for key in ("name", "level", "area", "node", "setup_frame", "sample_start", "sample_frames"):
        if key not in scene:
            raise ValueError("Scene missing " + key)
    if not isinstance(scene["name"], str) or not re.fullmatch(r"[A-Za-z0-9_-]+", scene["name"]):
        raise ValueError("Scene name must use letters, numbers, underscores or hyphens")
    for key in ("area", "node", "setup_frame", "sample_start", "sample_frames"):
        if type(scene[key]) is not int:
            raise ValueError(key + " must be an integer")
    if not 1 <= scene["area"] <= 8 or not 0 <= scene["node"] <= 255:
        raise ValueError("Invalid area or warp node")
    if type(scene.get("seed", 0)) is not int or not 0 <= scene.get("seed", 0) <= 65535:
        raise ValueError("Random seed must be in [0, 65535]")
    vectors = [scene.get("mario", {}).get(key) for key in ("position", "velocity")]
    vectors += [scene.get("camera", {}).get(key) for key in ("position", "focus")]
    for numbers in vectors:
        if numbers is not None and (len(numbers) != 3 or any(
                not isinstance(number, (int, float)) or not math.isfinite(number) for number in numbers)):
            raise ValueError("Position, velocity and camera vectors need three finite coordinates")
    if not 0 < scene["setup_frame"] < scene["sample_start"]:
        raise ValueError("Scene needs setup followed by warm-up")
    if scene["sample_start"] - scene["setup_frame"] < 15:
        raise ValueError("Warm-up must cover at least one complete 15-frame window")
    if scene["sample_frames"] < 30 or scene["sample_frames"] % 15:
        raise ValueError("Measurement must be at least 30 frames and a multiple of 15")
    for entry in scene.get("inputs", []):
        if not 0 < entry["from"] < entry["until"]:
            raise ValueError("Input interval must be positive and nonempty")
        stick = entry.get("stick", [0, 0])
        if len(stick) != 2 or any(not isinstance(x, int) or abs(x) > 64 for x in stick):
            raise ValueError("Stick coordinates must be integers in [-64, 64]")
        if sum(x * x for x in stick) > 64 * 64:
            raise ValueError("Stick magnitude must not exceed 64")
        if not 0 <= entry.get("buttons", 0) <= 65535:
            raise ValueError("Invalid controller button mask")
    end = scene["sample_start"] + scene["sample_frames"] + 15
    for event in scene.get("events", []):
        if not scene["setup_frame"] <= event["frame"] < end or not event.get("set"):
            raise ValueError("Events must execute during the prepared scene")
    return scene


def isolated_settings(original, port, saves):
    """Update exact BML paths without changing the user's saved settings."""
    changes = {"Input/Defocus": "Allow", "Boot/AwaitGDBClient": "true",
               "General/AutoSaveMemory": "false", "General/Rewind": "false",
               "General/RunAhead": "false", "DebugServer/Port": str(port),
               "DebugServer/Enabled": "true", "DebugServer/UseIPv4": "true",
               "Paths/Saves": str(saves) + "/"}
    stack = []
    output = []
    found = set()
    for line in original.splitlines():
        match = re.fullmatch(r"( *)([^:]+?)(?::(.*))?", line)
        if not match:
            output.append(line)
            continue
        indent, key, tail = match.groups()
        while stack and stack[-1][0] >= len(indent):
            stack.pop()
        path = "/".join([entry[1] for entry in stack] + [key])
        if path in changes:
            line = indent + key + ": " + changes[path]
            found.add(path)
        elif tail and ";;" in tail:
            line = indent + key + ": ;;"
        output.append(line)
        stack.append((len(indent), key))
    if found != set(changes):
        raise ValueError("Ares settings missing: " + ", ".join(sorted(set(changes) - found)))
    return "\n".join(output) + "\n"


def source_files(root):
    """Include extracted assets and required SDK inputs, exclude trial outputs."""
    for folder, dirs, files in os.walk(root):
        dirs[:] = sorted(name for name in dirs if name != "build" and
                         not (Path(folder) == root and name.startswith("build-")) and
                         name not in {".git", ".codex", ".quality-tools", "__pycache__",
                                      "performance-captures", "benchmark-results", ".vscode"})
        for name in sorted(files):
            path = Path(folder) / name
            if path.is_symlink():
                raise ValueError("Snapshot source contains a symlink: " + str(path))
            if name.endswith((".pyc", ".eeprom", ".sav", ".state")):
                continue
            yield path.relative_to(root)


def manifest(root):
    return {str(path): capture.sha256(root / path) for path in source_files(root)}


def freeze(args):
    destination = Path(args.output).resolve()
    origin = Path(args.source).resolve()
    if destination == origin or origin in destination.parents:
        raise ValueError("Freeze outside the source tree to avoid recursive copies")
    destination.mkdir(parents=True, exist_ok=False)
    source = destination / "source"
    source.mkdir()
    before = manifest(origin)
    for name in before:
        target = source / name
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(origin / name, target)
    if manifest(origin) != before or manifest(source) != before:
        raise ValueError("Source changed during snapshot; discard this incomplete freeze")
    save_json(destination / "inputs.json", before)
    git = subprocess.run(["git", "rev-parse", "HEAD"], cwd=origin, capture_output=True, text=True)
    save_json(destination / "origin.json", {"source": str(origin), "head": git.stdout.strip(),
                                           "inputs_sha256": capture.sha256(destination / "inputs.json")})
    print("Frozen " + str(len(before)) + " inputs in " + str(destination))


def fork(args):
    origin = Path(args.control).resolve()
    destination = Path(args.output).resolve()
    if destination == origin or origin in destination.parents:
        raise ValueError("Fork outside the control directory")
    inputs_path = origin / "built-inputs.json"
    if not inputs_path.exists():
        inputs_path = origin / "inputs.json"
    inputs = json.loads(inputs_path.read_text())
    if manifest(origin / "source") != inputs:
        raise ValueError("Control inputs changed; freeze a new control")
    # Copy rather than hard-link: editing the candidate must never mutate control.
    shutil.copytree(origin, destination, ignore=shutil.ignore_patterns("runs", "__pycache__"))
    print("Candidate copied with reusable assets/objects to " + str(destination))


def build(args):
    trial = Path(args.trial).resolve()
    source = trial / "source"
    if not (trial / "inputs.json").is_file() or not source.is_dir():
        raise ValueError("Build requires a frozen trial")
    flags = ["VERSION=us", "CONSOLE=n64", "COMPILER=gcc", "PERFORMANCE_CAPTURE=1",
             "COLLISION_PROFILING=0", "CROSS=" + str(COMPILER).removesuffix("gcc")]
    for option in args.make_option:
        if not re.fullmatch(r"[A-Z_][A-Z_0-9]*=[^\n\r]*", option):
            raise ValueError("Use KEY=VALUE Makefile options")
        if option.split("=", 1)[0] in {"VERSION", "CONSOLE", "COMPILER", "CC", "CXX", "CROSS",
                                      "AS", "LD", "AR", "CPP", "OBJCOPY", "OBJDUMP", "PYTHON",
                                      "N64_TOOLCHAIN_PREFIX", "PERFORMANCE_CAPTURE",
                                      "COLLISION_PROFILING", "BUILD_DIR", "BUILD_DIR_BASE"}:
            raise ValueError("This option would change the runner's build contract: " + option)
        flags.append(option)
    environment = os.environ.copy()
    environment["PATH"] = str(COMPILER.parent) + ":" + environment.get("PATH", "")
    environment["N64_TOOLCHAIN_PREFIX"] = str(COMPILER).removesuffix("gcc")
    cache = shutil.which("ccache")
    if not cache and WORKSPACE_CCACHE.is_file():
        cache = str(WORKSPACE_CCACHE)
    if cache:
        environment["CCACHE_DIR"] = str(ROOT / "build/benchmark-ccache")
        environment["CCACHE_COMPILERCHECK"] = "content"
        flags += ["CC=" + cache + " " + str(COMPILER)]
    version = subprocess.check_output([str(COMPILER), "--version"], text=True).splitlines()[0]
    tools = {name: subprocess.check_output([str(COMPILER).removesuffix("gcc") + name,
                                          "--version"], text=True).splitlines()[0]
             for name in ("as", "ld")}
    signature = {"flags": flags, "compiler_sha256": capture.sha256(COMPILER), "binutils": tools,
                 "makefile_sha256": capture.sha256(source / "Makefile")}
    previous = trial / "build.json"
    if (source / "build").exists():
        if not previous.exists():
            raise ValueError("Existing build has no benchmark provenance; freeze a fresh trial")
        if json.loads(previous.read_text())["signature"] != signature:
            # Make does not track command-line flag changes. Never reuse stale objects.
            shutil.rmtree(source / "build")
    started = time.monotonic()
    with (trial / "build.log").open("w") as log:
        subprocess.run(["make", "-j" + str(args.jobs), *flags], cwd=source, env=environment,
                       stdout=log, stderr=subprocess.STDOUT, check=True)
    output = subprocess.check_output(["make", "-s", "print-build-paths", *flags],
                                     cwd=source, env=environment, text=True)
    paths = dict(line[6:].split("=", 1) for line in output.splitlines()
                 if line.startswith("BUILD_") and "=" in line)
    rom = source / paths["ROM"]
    elf = source / paths["ELF"]
    save_json(trial / "built-inputs.json", manifest(source))
    save_json(previous, {"signature": signature, "compiler": version, "ccache": cache,
                         "elapsed_seconds": time.monotonic() - started, "options": paths,
                         "rom": str(rom), "elf": str(elf), "rom_sha256": capture.sha256(rom),
                         "elf_sha256": capture.sha256(elf),
                         "inputs_sha256": capture.sha256(trial / "built-inputs.json")})
    print("Built " + str(rom) + " (" + version + ")")


def parse_capture(log, result, scene):
    stream = io.StringIO()
    recording = capture.Recording(stream)
    expected = result["sequence_start"]
    count = 0
    for line in log.splitlines():
        row = capture.parse_sample(line)
        if row is None or not result["sequence_start"] <= row["sequence"] < result["sequence_end"]:
            continue
        if row["sequence"] != expected:
            raise ValueError("Capture has missing, duplicate or reset measurement windows")
        if (row["protocol"] != "SM64PERF2" or row["level"] != result["level"] or
                row["area"] != scene["area"] or row["play_mode"] != 0 or not row["player_ready"]):
            raise ValueError("Capture contains an invalid scene window")
        expected += 1
        count += row["sample_frames"]
        recording.consume(line)
    if expected != result["sequence_end"] or count != scene["sample_frames"]:
        raise ValueError("Capture did not contain the complete requested measurement")
    rows = list(csv.DictReader(io.StringIO(stream.getvalue())))
    metrics = {name + "_us": statistics.mean(float(row[name + "_us"]) for row in rows)
               for name in capture.TIMERS}
    metrics["cpu_main_peak_us"] = max(float(row["cpu_main_peak_us"]) for row in rows)
    metrics["worst_frame_vblanks"] = max(int(row["max_elapsed_vblanks"]) for row in rows)
    metrics["fps"] = (int(rows[0]["vi_millihz"]) * count /
                      (1000 * sum(int(row["elapsed_vblanks"]) for row in rows)))
    metrics["capture_us"] = statistics.mean(float(row["capture_us"]) for row in rows)
    if metrics["cpu_main_us"] <= 0:
        raise ValueError("Main CPU timings are unavailable")
    return stream.getvalue(), metrics


def native_ares(executable):
    if executable:
        return str(Path(executable).resolve())
    location = subprocess.check_output(["flatpak", "info", "--show-location", capture.APPS["ares"]],
                                       text=True).strip()
    return str(Path(location) / "files/bin/ares")


def run_one(rom, elf, scene, work, args):
    work.mkdir()
    saves = work / "saves"
    saves.mkdir()
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        port = sock.getsockname()[1]
    original = Path(args.settings).read_text()
    (work / "settings.bml").write_text(isolated_settings(original, port, saves))
    save_json(work / "scene.json", scene)
    commands = ("set pagination off\nset confirm off\nset remotetimeout 5\n"
                "set tcp auto-retry on\nset tcp connect-timeout 15\n"
                "python\ngdb.execute('file ' + " + repr(json.dumps(str(elf))) + ")\nend\n"
                "target remote 127.0.0.1:" + str(port) + "\npython\n"
                "BENCHMARK_SCENE = " + repr(scene) + "\nBENCHMARK_WORK = " + repr(str(work)) + "\n"
                "exec(compile(open(" + repr(str(work.parent / "scene-driver.py")) +
                ").read(), 'benchmark_scene.py', 'exec'))\nend\ncontinue\ndetach\n")
    (work / "gdb.commands").write_text(commands)
    environment = os.environ.copy()
    if args.runtime_libs:
        environment["LD_LIBRARY_PATH"] = args.runtime_libs + ":" + environment.get("LD_LIBRARY_PATH", "")
    executable = native_ares(args.ares)
    command = [sys.executable, str(ROOT / "tools/launch_ares_background.py"), executable,
               "--settings-file", str(work / "settings.bml"), "--no-file-prompt", str(rom)]
    metadata = {"rom_sha256": capture.sha256(rom), "elf_sha256": capture.sha256(elf),
                "scene_sha256": capture.sha256(work / "scene.json"), "command": command,
                "emulator": capture.emulator_info(capture.APPS["ares"]), "limits": LIMITS,
                "settings_sha256": capture.sha256(work / "settings.bml"),
                "scene_driver_sha256": capture.sha256(work.parent / "scene-driver.py"),
                "emulator_binary_sha256": capture.sha256(Path(executable))}
    started = time.monotonic()
    save_json(work / "metadata.json", {**metadata, "status": "running"})
    with (work / "ares.log").open("w") as log:
        process = subprocess.Popen(command, env=environment, stdout=log, stderr=subprocess.STDOUT,
                                   start_new_session=True)
        try:
            with (work / "gdb.log").open("w") as debug:
                subprocess.run(["gdb-multiarch", "-q", "-nx", "-batch", "-x", str(work / "gdb.commands")],
                               stdout=debug, stderr=subprocess.STDOUT, timeout=args.timeout, check=True)
        finally:
            if process.poll() is None:
                os.killpg(process.pid, signal.SIGTERM)
                try:
                    process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    os.killpg(process.pid, signal.SIGKILL)
                    process.wait()
    if (work / "scene-error.json").exists():
        raise ValueError((work / "scene-error.json").read_text().strip())
    result = json.loads((work / "scene-result.json").read_text())
    log = (work / "ares.log").read_text(errors="replace")
    if "Vulkan Enabled: using paraLLEl-RDP" not in log:
        raise ValueError("Could not verify the expected Ares Vulkan rendering backend")
    text, metrics = parse_capture(log, result, scene)
    (work / "frames.csv").write_text(text)
    result["metrics"] = metrics
    result["state_sha256"] = hashlib.sha256(json.dumps(result["observations"],
                                                       sort_keys=True).encode()).hexdigest()
    save_json(work / "results.json", result)
    save_json(work / "metadata.json", {**metadata, "status": "complete",
                                       "elapsed_seconds": time.monotonic() - started,
                                       "backend": "Ares N64 CPU/native RSP; Vulkan paraLLEl-RDP"})
    print(scene["name"] + ": " + work.name + " complete", flush=True)
    return result


def compare(results):
    pairs = []
    for control, candidate in results:
        same_state = control["state_sha256"] == candidate["state_sha256"]
        same_image = control["image_sha256"] == candidate["image_sha256"]
        deltas = {}
        for name, before in control["metrics"].items():
            after = candidate["metrics"][name]
            deltas[name] = {"control": before, "candidate": after, "difference": after - before,
                            "percent": 100 * (after - before) / before if before else None}
        pairs.append({"state_matches": same_state, "completed_image_matches": same_image,
                      "deltas": deltas})
    valid = all(pair["state_matches"] and pair["completed_image_matches"] for pair in pairs)
    return {"pairs": pairs, "matched": valid,
            "decision": ("Review repeated net savings and relevant regression scenes before adoption."
                         if valid else "Workload or image mismatch: investigate before making a speed claim.")}


def run(args):
    output = Path(args.output).resolve()
    output.mkdir(parents=True, exist_ok=False)
    scene = load_scene(args.scene)
    save_json(output / "scene.json", scene)
    shutil.copy2(ROOT / "tools/benchmark_scene.py", output / "scene-driver.py")
    artifacts = {}
    for label, rom_name, elf_name in (("control", args.rom, args.elf),
                                      ("candidate", args.candidate_rom, args.candidate_elf)):
        if not rom_name:
            continue
        if not elf_name:
            raise ValueError("Each ROM needs its own matching ELF")
        folder = output / label
        folder.mkdir()
        rom, elf = folder / "sm64.z64", folder / "sm64.elf"
        shutil.copy2(rom_name, rom)
        shutil.copy2(elf_name, elf)
        artifacts[label] = (rom, elf)
    lock_path = ROOT / "build/benchmark.lock"
    lock_path.parent.mkdir(parents=True, exist_ok=True)
    comparisons = []
    lines = ["# " + scene["name"], "", LIMITS, ""]
    with lock_path.open("w") as lock:
        wait_started = time.monotonic()
        while True:
            try:
                fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
                break
            except BlockingIOError:
                if time.monotonic() - wait_started >= args.wait_for_emulator:
                    raise ValueError("Another benchmark still owns the runner lock")
                print("Waiting for the active benchmark runner to finish", flush=True)
                time.sleep(10)
        while (not args.separate_instance
               and subprocess.run(["pgrep", "-x", "ares"], capture_output=True).returncode == 0):
            if time.monotonic() - wait_started >= args.wait_for_emulator:
                raise ValueError("Ares is still busy; retained prepared artifacts without interrupting it")
            print("Waiting for the existing Ares session to finish", flush=True)
            time.sleep(10)
        for repeat in range(args.repeats):
            order = list(artifacts)
            if repeat % 2:
                order.reverse()
            results = {}
            for label in order:
                work = output / (str(repeat + 1) + "-" + label)
                try:
                    results[label] = run_one(*artifacts[label], scene, work, args)
                except Exception as error:
                    save_json(work / "failure.json", {"error": str(error)})
                    metadata_path = work / "metadata.json"
                    if metadata_path.exists():
                        metadata = json.loads(metadata_path.read_text())
                        save_json(metadata_path, {**metadata, "status": "failed", "error": str(error)})
                    raise
                metrics = results[label]["metrics"]
                lines.append(label + " " + str(repeat + 1) + ": main CPU " +
                             f"{metrics['cpu_main_us'] / 1000:.3f} ms, audio " +
                             f"{metrics['audio_avg_us'] / 1000:.3f} ms, graphics RSP " +
                             f"{metrics['rsp_graph_avg_us'] / 1000:.3f} ms, " +
                             f"{metrics['fps']:.3f} FPS; worst frame " +
                             str(metrics["worst_frame_vblanks"]) + " video refreshes.")
            if "candidate" in results:
                comparisons.append((results["control"], results["candidate"]))
    if comparisons:
        comparison = compare(comparisons)
        save_json(output / "comparison.json", comparison)
        lines += ["", comparison["decision"]]
        for index, pair in enumerate(comparison["pairs"], 1):
            delta = pair["deltas"]["cpu_main_us"]
            lines.append(f"Pair {index}: main CPU {delta['percent']:+.3f}%; " +
                         f"state matches={pair['state_matches']}, image matches={pair['completed_image_matches']}.")
    (output / "summary.md").write_text("\n\n".join(lines) + "\n")
    print("Report: " + str(output / "summary.md"))


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    command = commands.add_parser("list", help="show reusable scene presets")
    command.set_defaults(function=lambda args: print((ROOT / "tools/benchmark_scenes.json").read_text()))
    command = commands.add_parser("freeze", help="freeze current source and extracted assets")
    command.add_argument("--source", default=str(ROOT))
    command.add_argument("--output", required=True)
    command.set_defaults(function=freeze)
    command = commands.add_parser("fork", help="copy a control with its reusable build outputs")
    command.add_argument("--control", required=True)
    command.add_argument("--output", required=True)
    command.set_defaults(function=fork)
    command = commands.add_parser("build", help="incrementally build a private frozen trial with GCC 16.2")
    command.add_argument("--trial", required=True)
    command.add_argument("--make-option", action="append", default=[])
    command.add_argument("--jobs", type=int, default=min(4, os.cpu_count() or 1))
    command.set_defaults(function=build)
    command = commands.add_parser("run", help="replay a preset or JSON scene against supplied artifacts")
    command.add_argument("--scene", required=True)
    command.add_argument("--rom", required=True)
    command.add_argument("--elf", required=True)
    command.add_argument("--candidate-rom")
    command.add_argument("--candidate-elf")
    command.add_argument("--output", required=True)
    command.add_argument("--repeats", type=int, default=2)
    command.add_argument("--timeout", type=float, default=180)
    command.add_argument("--wait-for-emulator", type=float, default=300)
    command.add_argument("--separate-instance", action="store_true",
                         help="run a private background instance alongside an existing Ares game")
    command.add_argument("--ares")
    command.add_argument("--settings", default=str(DEFAULT_SETTINGS))
    command.add_argument("--runtime-libs", default=os.environ.get("ARES_RUNTIME_LIBS"))
    command.set_defaults(function=run)
    args = parser.parse_args(argv)
    if getattr(args, "repeats", 1) < 1 or getattr(args, "jobs", 1) < 1:
        parser.error("Repeat and worker counts must be positive")
    try:
        args.function(args)
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        print("Benchmark failed: " + str(error), file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
