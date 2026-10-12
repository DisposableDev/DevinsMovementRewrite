#!/usr/bin/env python3
"""Rebuild and automatically collect GCC collision profiles in Lab and JRB.

Run from any directory: python3 tools/profile_gcc.py [--jobs N]. No game input
is needed. Every invocation rebuilds incrementally and boots each route afresh.
"""

import argparse
from datetime import datetime
import fcntl
import gzip
import json
import os
from pathlib import Path
import re
import shutil
import signal
import socket
import subprocess
import sys
import time
import uuid

from capture_play import ROOT, build_paths, default_build_jobs, emulator_info, research_directory, sha256
from decode_gcov_capture import decode

TOOLCHAIN = Path("/opt/n64-gcc-16.2/bin")
PREFIX = str(TOOLCHAIN / "mips64-elf-")
ARES = Path("/var/lib/flatpak/app/dev.ares.ares/x86_64/stable/active/files/bin/ares")
RUNTIME = ROOT.parents[1] / "audio-benchmark-2026-10-02/runtime-libs"
SETTINGS = Path.home() / ".var/app/dev.ares.ares/data/ares/settings.bml"
FLAGS = ["VERSION=us", "CONSOLE=n64", "COMPILER=gcc", "ENGINE_LTO=0", "ENGINE_G16=1",
         "PERFORMANCE_CAPTURE=1", "COLLISION_PROFILING=0", "GCC_PROFILE=1", f"CROSS={PREFIX}"]


def source_identity():
    """Track build inputs, including uncommitted and untracked source changes."""
    names = subprocess.check_output(
        ["git", "ls-files", "-cz", "--others", "--exclude-standard"], cwd=ROOT).split(b"\0")
    directories = {"src", "include", "asm", "data", "actors", "levels", "bin", "assets",
                   "sound", "textures", "lib"}
    makefile = (ROOT / "Makefile").read_text()
    runner_files = {"tools/profile_gcc.py", "tools/gcc_profile_gdb.py", "tools/capture_play.py",
                    "tools/launch_ares_background.py", "tools/ares_background.c"}
    result = {}
    for name in names:
        if not name:
            continue
        path = Path(os.fsdecode(name))
        build_tool = (path.parts[0] == "tools"
                      and (path.suffix in {".c", ".h", ".sh"} or str(path) in makefile))
        build_recipe = path.name == "Makefile" or path.suffix in {".mk", ".ld"}
        root_input = len(path.parts) == 1 and str(path) in makefile
        if not (path.parts[0] in directories or build_tool or build_recipe
                or root_input or str(path) in runner_files):
            continue
        source = ROOT / path
        if source.is_file():
            result[str(path)] = sha256(source)
    return result


def require_unchanged(identity):
    if source_identity() != identity:
        raise RuntimeError("Source changed during profiling. Launch again to rebuild and collect fresh counts.")


def build_settings_identity(identity):
    return {"compiler_sha256": sha256(TOOLCHAIN / "mips64-elf-gcc"), "flags": FLAGS,
            "recipes": {name: digest for name, digest in identity.items()
                        if Path(name).name == "Makefile" or name.endswith(".mk")}}


def needs_full_rebuild(stamp, settings):
    try:
        return json.loads(stamp.read_text()) != settings
    except (OSError, ValueError):
        return True


def private_settings(text, port, saves):
    """Keep the user's settings and saves intact; leave audio synthesis running."""
    for name, replacement in [("Defocus", "Allow"), ("AutoSaveMemory", "false"),
                              ("Mute", "true"), ("AwaitGDBClient", "true"),
                              ("HomebrewMode", "true")]:
        text, count = re.subn(rf"(?m)^  {name}: .*?$", f"  {name}: {replacement}", text)
        if count != 1:
            raise ValueError(f"Expected one ares {name} setting")
    text, count = re.subn(r"DebugServer\n  Port: \d+\n  Enabled: (?:true|false)\n  UseIPv4: (?:true|false)",
                         f"DebugServer\n  Port: {port}\n  Enabled: true\n  UseIPv4: true", text)
    if count != 1:
        raise ValueError("Missing ares debugger settings")
    text = re.sub(r"(: )[^\n]*;;[^\n]*", r"\1;;", text)
    text, count = re.subn(r"(?m)^  Saves(?:: .*?)?$", f"  Saves: {saves}", text)
    if count != 1:
        raise ValueError("Missing ares save directory setting")
    return text


def validate_route(scene, rows):
    if len(rows) != 166 or [row["age"] for row in rows] != list(range(90, 256)):
        raise RuntimeError(f"{scene}: incomplete route window")
    if any(row["area"] != 1 for row in rows):
        raise RuntimeError(f"{scene}: wrong area")
    if scene == "lab":
        positions = [row["position"] for row in rows[:120]]
        if max(sum((a - b) ** 2 for a, b in zip(position, positions[0]))
               for position in positions) < 100 ** 2:
            raise RuntimeError("Lab route did not walk")
    elif any(not row["action"] & 0x2000 or row["position"][1] >= row["water_level"] - 80
             for row in rows):
        raise RuntimeError("JRB route did not remain swimming underwater")


def stop_emulator(process):
    if process.poll() is None:
        os.killpg(process.pid, signal.SIGTERM)
        try:
            process.wait(timeout=10)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGKILL)
            process.wait(timeout=5)


def prepare_report_directory(payload, reports, expected):
    # This cross-configured gcov-tool does not create missing directories.
    # Check the single serialized filename before letting it write any data.
    if payload[:8] != b"gcfnB62*" or len(payload) < 12:
        raise ValueError("Expected a GCC 16.2 big-endian filename record")
    length = int.from_bytes(payload[8:12], "big")
    name = payload[12:12 + length]
    if name != os.fsencode(expected) + b"\0":
        raise ValueError("Profile filename does not match the instrumented object")
    target = reports / expected.relative_to(expected.anchor)
    target.parent.mkdir(parents=True, exist_ok=True)
    return target


def run_scene(scene, folder, frozen, counters, water_idle, gcda_path, args):
    work = folder / scene
    work.mkdir()
    saves = work / "saves"
    saves.mkdir()
    with socket.socket() as sock:
        sock.bind(("127.0.0.1", 0))
        port = sock.getsockname()[1]
    (work / "settings.bml").write_text(private_settings(SETTINGS.read_text(), port, saves))
    config = {"work": str(work), "scene": scene, "counters": counters, "water_idle": water_idle}
    commands = ("set pagination off\nset confirm off\nset remotetimeout 5\n"
                f"file {json.dumps(str(frozen / 'sm64.elf'))}\n"
                f"target remote 127.0.0.1:{port}\npython\nCONFIG = {config!r}\n"
                + (frozen / "gcc_profile_gdb.py").read_text()
                + "\nend\ncontinue\ndetach\n")
    (work / "gdb.commands").write_text(commands)
    environment = dict(os.environ, LD_LIBRARY_PATH=str(args.runtime))
    command = [sys.executable, str(ROOT / "tools/launch_ares_background.py"), str(args.ares),
               "--settings-file", str(work / "settings.bml"), "--no-file-prompt", str(frozen / "sm64.z64")]
    print(f"Collecting {scene} automatically…", flush=True)
    with (work / "emulator.log").open("w") as log:
        process = subprocess.Popen(command, stdout=log, stderr=subprocess.STDOUT,
                                   env=environment, start_new_session=True)
        try:
            # Give the debugger server time to start without opening an extra
            # connection that would consume ares's single waiting client.
            time.sleep(2)
            with (work / "gdb.log").open("w") as debug:
                result = subprocess.run(["gdb-multiarch", "-q", "-nx", "-batch", "-x",
                                         str(work / "gdb.commands")], stdout=debug,
                                        stderr=subprocess.STDOUT, timeout=args.timeout)
            if result.returncode or not (work / "route.json").is_file():
                raise RuntimeError(f"{scene} route failed; see {work / 'gdb.log'}")
            deadline = time.monotonic() + 5
            while "SM64GCOV1,END," not in (work / "emulator.log").read_text(errors="replace"):
                if time.monotonic() > deadline:
                    raise RuntimeError(f"{scene}: profile export did not finish")
                time.sleep(0.1)
        finally:
            stop_emulator(process)
    route = json.loads((work / "route.json").read_text())
    validate_route(scene, route["rows"])
    payload = decode((work / "emulator.log").read_text(errors="replace").splitlines())
    stream = work / "profile.gcov-stream"
    stream.write_bytes(payload)
    reports = work / "reports"
    reports.mkdir()
    target = prepare_report_directory(payload, reports, gcda_path)
    # libgcov also honors these variables in merge-stream. Keep every .gcda
    # under a fresh scene directory rather than merging into the working build.
    environment.update(GCOV_PREFIX=str(reports), GCOV_PREFIX_STRIP="0")
    merged = subprocess.run([PREFIX + "gcov-tool", "merge-stream", str(stream)], cwd=reports,
                            env=environment, text=True, capture_output=True, check=True)
    (reports / "merge.log").write_text(merged.stdout + merged.stderr)
    data = list(reports.rglob("*.gcda"))
    if data != [target]:
        raise RuntimeError(f"Expected one collision profile, found {len(data)}")
    shutil.copy2(frozen / "surface_collision.gcno", data[0].with_suffix(".gcno"))
    report = subprocess.run([PREFIX + "gcov", "--branch-probabilities", "--branch-counts",
                             "--function-summaries", "--json-format", str(data[0])],
                            cwd=reports, text=True, capture_output=True, check=True)
    (reports / "gcov.log").write_text(report.stdout + report.stderr)
    json_reports = list(reports.glob("*.gcov.json.gz"))
    if len(json_reports) != 1:
        raise RuntimeError("GCC did not produce the expected coverage report")
    with gzip.open(json_reports[0], "rt") as source:
        coverage = json.load(source)
    functions = sorted([{"file": file["file"], "name": function["name"],
                         "calls": function["execution_count"]}
                        for file in coverage["files"] for function in file["functions"]],
                       key=lambda item: item["calls"], reverse=True)
    if not functions or not any(function["calls"] for function in functions):
        raise RuntimeError("Profile has no executed collision functions")
    if "Vulkan Enabled: using paraLLEl-RDP" not in (work / "emulator.log").read_text(errors="replace"):
        raise RuntimeError("Expected ares Vulkan paraLLEl-RDP backend was not confirmed")
    result = {"scene": scene, "frames": len(route["rows"]), "stream_bytes": len(payload),
              "functions": functions, "command": command,
              "backends": {"cpu": "native VR4300", "rsp": "native RSP",
                           "rdp": "Vulkan paraLLEl-RDP (confirmed in emulator.log)"}}
    (work / "summary.json").write_text(json.dumps(result, indent=2) + "\n")
    return result


def collect(args, folder):
    version = subprocess.check_output([PREFIX + "gcc", "-dumpfullversion"], text=True).strip()
    if version != "16.2.0":
        raise RuntimeError(f"Expected GCC 16.2.0, found {version}")
    environment = dict(os.environ, N64_TOOLCHAIN_PREFIX=PREFIX,
                       PATH=str(TOOLCHAIN) + os.pathsep + os.environ.get("PATH", ""))
    # build_paths uses the same explicit CROSS setting even in an older shell.
    rom, elf, options = build_paths(FLAGS)
    identity = source_identity()
    settings = build_settings_identity(identity)
    stamp = elf.parent / "auto-profile-build.json"
    force = needs_full_rebuild(stamp, settings)
    command = ["make", f"-j{args.jobs}", *FLAGS]
    if force:
        command.append("--always-make")
        print("Compiler/build settings changed or unverified; rebuilding this profiling mode fully.", flush=True)
    print(f"Updating the separate profiling ROM with GCC {version} ({args.jobs} build jobs)…", flush=True)
    with (folder / "build.log").open("w") as log:
        subprocess.run(command, cwd=ROOT, env=environment,
                       stdout=log, stderr=subprocess.STDOUT, check=True)
    require_unchanged(identity)
    stamp.write_text(json.dumps(settings, indent=2) + "\n")
    frozen = folder / "snapshot"
    frozen.mkdir()
    notes = elf.parent / "src/engine/surface_collision.gcno"
    for source in [rom, elf, notes, ROOT / "tools/gcc_profile_gdb.py"]:
        shutil.copy2(source, frozen / source.name)
    shutil.copytree(ROOT / "src/engine", frozen / "src/engine")
    require_unchanged(identity)
    metadata = {"compiler": version, "build_flags": FLAGS, "build_options": options,
                "build_jobs": args.jobs, "source_sha256": identity,
                "full_rebuild": force, "build_settings_identity": settings,
                "rom_sha256": sha256(frozen / "sm64.z64"), "elf_sha256": sha256(frozen / "sm64.elf"),
                "notes_sha256": sha256(frozen / notes.name), "status": "collecting",
                "git_commit": subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=ROOT, text=True).strip(),
                "emulator_executable": str(args.ares), "emulator_sha256": sha256(args.ares),
                **emulator_info("dev.ares.ares"),
                "evidence": "GCC arc/function counts on ares native CPU/RSP with Vulkan paraLLEl-RDP; not timing or console speed evidence."}
    (folder / "metadata.json").write_text(json.dumps(metadata, indent=2) + "\n")
    symbols = subprocess.check_output([PREFIX + "nm", "-S", "--defined-only", str(frozen / "sm64.elf")], text=True)
    counters = [[int(fields[0], 16), int(fields[1], 16)] for line in symbols.splitlines()
                if len(fields := line.split()) == 4 and fields[3].startswith("__gcov0.")]
    if not counters or any(size % 8 for _, size in counters):
        raise RuntimeError("Missing or invalid GCC arc counter arrays in this ELF")
    water_idle = int(re.search(r"(?m)^#define ACT_WATER_IDLE\s+(0x[0-9a-fA-F]+)",
                              (ROOT / "include/sm64.h").read_text())[1], 16)
    results = [run_scene(scene, folder, frozen, counters, water_idle,
                         notes.with_suffix(".gcda"), args) for scene in args.scene]
    require_unchanged(identity)
    metadata.update(status="complete", scenes=[result["scene"] for result in results])
    (folder / "metadata.json").write_text(json.dumps(metadata, indent=2) + "\n")
    lines = ["# GCC collision execution profiles", "",
             "GCC 16.2; warm-up excluded; each scene starts from a fresh boot.",
             "Counts describe these scripted routes. They are not performance timings.", ""]
    for result in results:
        lines += [f"## {result['scene']} ({result['frames']} frames)", "",
                  "| Function | Calls |", "| --- | ---: |"]
        lines += [f"| `{item['name']}` | {item['calls']} |" for item in result["functions"][:20]]
        lines += ["", f"Full branch/line data: `{result['scene']}/reports/`.", ""]
    (folder / "summary.md").write_text("\n".join(lines))


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-j", "--jobs", type=int, default=default_build_jobs())
    parser.add_argument("--scene", action="append", choices=["lab", "jrb-swim"],
                        help="select a route; default: both")
    parser.add_argument("--timeout", type=float, default=120, help="debugger time limit per route, in seconds")
    parser.add_argument("--ares", type=Path, default=ARES)
    parser.add_argument("--runtime", type=Path, default=RUNTIME)
    args = parser.parse_args(argv)
    if args.jobs < 1 or not 0 < args.timeout <= 600:
        parser.error("Use positive jobs and a route timeout between 0 and 600 seconds")
    args.scene = list(dict.fromkeys(args.scene or ["lab", "jrb-swim"]))
    if not args.ares.is_file() or not args.runtime.is_dir() or not shutil.which("gdb-multiarch"):
        parser.error("The native ares runtime and gdb-multiarch must be installed")
    parent = research_directory() / "evidence/performance-captures/gcc-profile"
    parent.mkdir(parents=True, exist_ok=True)
    with (parent / "runner.lock").open("w") as lock:
        try:
            fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except BlockingIOError:
            parser.error("A GCC profiling run is already active")
        stamp = datetime.now().astimezone().strftime("%Y-%m-%d_%H-%M-%S")
        folder = parent / f"{stamp}-{uuid.uuid4().hex[:8]}"
        folder.mkdir()
        print(f"Profile folder: {folder}", flush=True)
        try:
            collect(args, folder)
        except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
            path = folder / "metadata.json"
            metadata = json.loads(path.read_text()) if path.exists() else {}
            metadata.update(status="failed", error=str(error))
            path.write_text(json.dumps(metadata, indent=2) + "\n")
            print(f"Profiling failed: {error}\nDetails: {folder}", file=sys.stderr)
            return 1
    print(f"Profiles complete: {folder / 'summary.md'}", flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
