#!/usr/bin/env python3
"""Build, launch, and record the game's ISViewer frame stream using only stdlib."""

import argparse
from collections import deque
import csv
from datetime import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import selectors
import signal
import subprocess
import sys
import time
import uuid

ROOT = Path(__file__).resolve().parents[1]


def research_directory():
    """Keep captures outside the game checkout, with an optional destination override."""
    configured = os.environ.get("SM64_RESEARCH_DIR")
    if configured:
        return Path(configured).expanduser().resolve()
    return ROOT.parents[1] / "research-archive" / ROOT.name


def default_build_jobs():
    """Respect the process's CPU allocation when choosing parallel builds."""
    try:
        return max(1, len(os.sched_getaffinity(0)))
    except (AttributeError, OSError):
        return max(1, os.cpu_count() or 1)


APPS = {"rmg": "com.github.Rosalie241.RMG", "ares": "dev.ares.ares"}
MARKER = "SM64PERF1,"
PASSIVE_MARKER = "SM64PERF2,"
TIMERS = (
    "cpu_main", "input", "spawner", "dynamic", "platform_setup", "platform_prepare",
    "object_collision", "behavior_before", "mario", "behavior_after", "object_cleanup",
    "graph", "collision", "camera", "audio_avg", "rsp_graph_avg", "rsp_audio_avg",
)
PASSIVE_TIMERS = TIMERS + ("rdp_tmem", "rdp_pipe", "rdp_cmd")
MAIN_TIMERS = TIMERS[:14]
COLLISION = (
    "floor_nodes", "wall_nodes", "ceil_nodes", "water_nodes", "ray_nodes",
    "mario_prepare_nodes", "water_edge_nodes", "object_sweep_nodes", "dynamic_models",
    "dynamic_surfaces", "dynamic_nodes", "dynamic_build_ticks", "dynamic_clear_ticks",
    "collision_ticks", "cpu_at_collision_end_ticks", "dynamic_bytes",
)
CONTEXT_FIELDS = (
    "sequence", "vblank", "elapsed_vblanks", "vi_millihz", "level", "area", "play_mode",
    "player_ready", "action", "flags", "x", "y", "z",
)
COUNTER_FIELDS = (
    "updated_objects", "sleeping_objects", "floor_calls", "wall_calls", "ceil_calls",
    "water_calls", "ray_calls", "matrix_calls",
)
FIELDS = CONTEXT_FIELDS + tuple(name + "_ticks" for name in TIMERS) + COUNTER_FIELDS + (
    "collision_detail_valid",
) + tuple("detail_" + name for name in COLLISION) + ("capture_ticks",)
PASSIVE_FIELDS = CONTEXT_FIELDS + tuple(name + "_ticks" for name in PASSIVE_TIMERS) + tuple(
    name + "_peak_ticks" for name in MAIN_TIMERS
) + COUNTER_FIELDS + ("sample_frames", "max_elapsed_vblanks", "capture_ticks")
ALL_FIELDS = tuple(dict.fromkeys(FIELDS + PASSIVE_FIELDS))
MICROSECOND_FIELDS = tuple(name + "_us" for name in PASSIVE_TIMERS) + tuple(
    name + "_peak_us" for name in MAIN_TIMERS
) + ("capture_us",)
CSV_FIELDS = ("segment", "protocol") + ALL_FIELDS + ("fps", "fps_15_frames") + MICROSECOND_FIELDS
SIGNED_FIELDS = {"level", "area", "play_mode", "player_ready", "x", "y", "z"}
TICKS_PER_MICROSECOND = 46.875
RDP_CYCLES_PER_MICROSECOND = 62.5
PHASES = {
    "Rendering": ("graph",), "Object behavior": ("behavior_before", "behavior_after"),
    "Collision": ("collision",), "Mario": ("mario",), "Dynamic surfaces": ("dynamic",),
    "Platforms": ("platform_setup", "platform_prepare"), "Object pairs": ("object_collision",),
    "Object cleanup": ("object_cleanup",), "Camera": ("camera",), "Input": ("input",),
    "Spawning": ("spawner",),
}


def largest_phase(row):
    phase = max(PHASES, key=lambda name: sum(row[timer + "_us"] for timer in PHASES[name]))
    return phase if any(row[name + "_us"] for name in PHASES[phase]) else "Unresolved"


def parse_sample(line):
    """Accept the same payload with RMG log prefixes or ares terminal output."""
    if PASSIVE_MARKER in line:
        marker, fields = PASSIVE_MARKER, PASSIVE_FIELDS
    elif MARKER in line:
        marker, fields = MARKER, FIELDS
    else:
        return None
    payload = line.split(marker, 1)[1].strip()
    if len(payload) != len(fields) * 8 or not re.fullmatch(r"[0-9a-fA-F]+", payload):
        raise ValueError("malformed performance row")
    values = [payload[offset:offset + 8] for offset in range(0, len(payload), 8)]
    row = dict.fromkeys(ALL_FIELDS, 0)
    row.update(zip(fields, (int(value, 16) for value in values)))
    row["protocol"] = marker.rstrip(",")
    if marker == MARKER:
        row["sample_frames"] = 1
        row["max_elapsed_vblanks"] = row["elapsed_vblanks"]
        for name in MAIN_TIMERS:
            row[name + "_peak_ticks"] = row[name + "_ticks"]
    if row["sample_frames"] not in (1, 15):
        raise ValueError("invalid sample window")
    for name in SIGNED_FIELDS:
        if row[name] >= 2 ** 31:
            row[name] -= 2 ** 32
    if row["player_ready"] not in (0, 1) or row["collision_detail_valid"] not in (0, 1):
        raise ValueError("invalid validity flag")
    if not 40000 <= row["vi_millihz"] <= 70000:
        raise ValueError("invalid video refresh rate")
    return row


class Recording:
    def __init__(self, stream):
        self.writer = csv.DictWriter(stream, CSV_FIELDS)
        self.writer.writeheader()
        self.count = 0
        self.malformed = 0
        self.gaps = 0
        self.segment = 0
        self.previous = None
        self.protocols = set()
        self.window = deque(maxlen=15)
        self.slow_frames = []
        self.collision_frames = []
        self.gameplay_count = 0
        self.frame_count = 0
        self.gameplay_seconds = 0.0
        self.timer_totals = dict.fromkeys(PASSIVE_TIMERS, 0.0)
        self.timer_peaks = dict.fromkeys(PASSIVE_TIMERS, 0.0)
        self.export_us_total = 0.0
        self.export_us_max = 0.0

    def consume(self, line):
        try:
            row = parse_sample(line)
        except ValueError:
            self.malformed += 1
            return
        if row is None:
            return
        self.protocols.add(row["protocol"])
        if self.previous is not None:
            if row["sequence"] <= self.previous:
                self.segment += 1
                self.window.clear()
            elif row["sequence"] != self.previous + 1:
                self.gaps += row["sequence"] - self.previous - 1
                self.window.clear()
        self.previous = row["sequence"]
        row["segment"] = self.segment
        delta = row["elapsed_vblanks"]
        frames = row["sample_frames"]
        row["fps"] = round(row["vi_millihz"] * frames / (1000 * delta), 3) if delta else ""
        gameplay = row["player_ready"] == 1 and row["play_mode"] == 0
        # Zero-delta/reset, pause and area transitions cannot form a gameplay window.
        valid = gameplay and 0 < delta <= 120 * frames
        row["fps_15_frames"] = ""
        if frames == 15:
            self.window.clear()
            if valid:
                row["fps_15_frames"] = row["fps"]
        elif not valid:
            self.window.clear()
        else:
            location = (row["level"], row["area"], row["vi_millihz"])
            if self.window and location != self.window[-1][0]:
                self.window.clear()
            self.window.append((location, delta))
        if len(self.window) == 15:
            row["fps_15_frames"] = round(row["vi_millihz"] * 15 / (1000 * sum(v[1] for v in self.window)), 3)
        for name in PASSIVE_TIMERS:
            clock = RDP_CYCLES_PER_MICROSECOND if name.startswith("rdp_") else TICKS_PER_MICROSECOND
            row[name + "_us"] = round(row[name + "_ticks"] / clock, 3)
        for name in MAIN_TIMERS:
            row[name + "_peak_us"] = round(row[name + "_peak_ticks"] / TICKS_PER_MICROSECOND, 3)
        row["capture_us"] = round(row["capture_ticks"] / TICKS_PER_MICROSECOND, 3)
        self.export_us_total += row["capture_us"]
        self.export_us_max = max(self.export_us_max, row["capture_us"])
        self.writer.writerow(row)
        self.count += 1
        self.frame_count += frames
        if valid:
            self.gameplay_count += frames
            self.gameplay_seconds += delta * 1000 / row["vi_millihz"]
            for name in PASSIVE_TIMERS:
                value = row[name + "_us"]
                self.timer_totals[name] += value * frames
                peak = row[name + "_peak_us"] if name in MAIN_TIMERS else value
                self.timer_peaks[name] = max(self.timer_peaks[name], peak)
            if row["collision_detail_valid"]:
                self.collision_frames.append(row)
                self.collision_frames.sort(key=lambda r: r["collision_ticks"], reverse=True)
                del self.collision_frames[10:]
            if row["fps_15_frames"] != "":
                self.slow_frames.append(row)
                self.slow_frames.sort(key=lambda r: (r["fps_15_frames"], -r["cpu_main_ticks"]))
                del self.slow_frames[150:]

    def summary(self):
        if not self.count:
            return "No frame data received. This session did not produce a usable performance capture.\n"
        lines = [
            f"Recorded {self.count} samples covering {self.frame_count} frames; {self.gameplay_count} gameplay frames.",
            f"Missing sequence samples: {self.gaps}; malformed rows: {self.malformed}; resets: {self.segment}.",
            f"Measured export cost: mean {self.export_us_total / self.count:.1f} us/sample; "
            f"{self.export_us_total / self.frame_count:.1f} us/frame amortized; max {self.export_us_max:.1f} us/sample.",
            "", "FPS uses video refreshes. Timings use the emulator's N64 clock, not host CPU utilization.",
            "Main CPU excludes audio/preemption and debug drawing. Audio/RSP values are task-ring averages.",
            "Export timing covers collection, serialization and ISViewer writes; host logging overhead needs a no-capture comparison.",
            "Closing can lose an incomplete 15-frame passive window, or three buffered detailed frames.",
        ]
        if self.gameplay_count:
            lines += ["", f"Gameplay frame pacing: {self.gameplay_count / self.gameplay_seconds:.2f} FPS overall.",
                      "", "Largest measured CPU phases (mean and single-frame peak, in us):", "",
                      "| Phase | Mean | Peak |", "| --- | --- | --- |"]
            phases = sorted(PHASES, key=lambda phase: sum(self.timer_totals[name] for name in PHASES[phase]), reverse=True)
            for phase in phases:
                mean = sum(self.timer_totals[name] for name in PHASES[phase]) / self.gameplay_count
                peak = sum(self.timer_peaks[name] for name in PHASES[phase])
                label = "Peak bound" if len(PHASES[phase]) > 1 else "Peak"
                lines.append(f"| {phase} | {mean:.1f} | {peak:.1f} ({label.lower()}) |")
            lines += ["", "Audio and graphics processors (task/window averages, in us):", "",
                      "| Measure | Mean | Highest average |", "| --- | --- | --- |"]
            for name in ("audio_avg", "rsp_graph_avg", "rsp_audio_avg", "rdp_tmem", "rdp_pipe", "rdp_cmd"):
                if not self.timer_peaks[name]:
                    lines.append(f"| {name} | unavailable/zero | unavailable/zero |")
                else:
                    lines.append(f"| {name} | {self.timer_totals[name] / self.gameplay_count:.1f} | {self.timer_peaks[name]:.1f} |")
        lines += [
            "", "Slowest gameplay windows (15 frames; pause and transitions excluded):", "",
            "| Segment/sample | FPS | Worst frame VBlanks | Main CPU mean/peak us | Largest CPU phase | Level/area | Position |",
            "| --- | --- | --- | --- | --- | --- | --- |",
        ]
        selected = []
        for row in self.slow_frames:
            if any(row["sample_frames"] == 1 and row["segment"] == other["segment"]
                   and abs(row["sequence"] - other["sequence"]) < 15 for other in selected):
                continue
            selected.append(row)
            lines.append(
                f'| {row["segment"]}/{row["sequence"]} | {row["fps_15_frames"]:.2f} | '
                f'{row["max_elapsed_vblanks"]} | {row["cpu_main_us"]:.1f}/{row["cpu_main_peak_us"]:.1f} | '
                f'{largest_phase(row)} | {row["level"]}/{row["area"]} | '
                f'{row["x"]}, {row["y"]}, {row["z"]} |'
            )
            if len(selected) == 10:
                break
        if not selected:
            lines.append("No complete gameplay window recorded. Title/menu frames remain in frames.csv.")
        lines += ["", "The largest phase is a lead, not proof of the cause. Peaks of combined phases are upper bounds.",
                  "Zero processor counters do not establish that rendering is cheap."]
        if self.collision_frames:
            lines += ["", "Optional detailed collision samples:", ""]
        for row in self.collision_frames[:5]:
            detail = ""
            if row["collision_detail_valid"]:
                detail = (f'; candidate visits: floors {row["detail_floor_nodes"]}, '
                          f'walls {row["detail_wall_nodes"]}, ceilings {row["detail_ceil_nodes"]}')
            lines.append(f'- {row["segment"]}/{row["sequence"]}: {row["collision_us"]:.1f} us; '
                         f'level {row["level"]}/{row["area"]}; '
                         f'position {row["x"]}, {row["y"]}, {row["z"]}{detail}')
        return "\n".join(lines) + "\n"


def sha256(path):
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def emulator_info(app_id):
    output = subprocess.check_output(["flatpak", "info", app_id], text=True,
                                     env={**os.environ, "LC_ALL": "C"})
    version = re.search(r"^\s*Version:\s*(.+)$", output, flags=re.M)
    return {"emulator_version": version[1] if version else "unknown", "flatpak_info": output}


def ares_save_path(rom, normal_rom, save_type="sram"):
    """Share the configured save format, preserving independently diverged saves."""
    if save_type != "sram":
        raise ValueError("Game captures require SRAM saves")
    extension = ".ram"
    save = rom.with_suffix(extension)
    normal_save = normal_rom.with_suffix(extension)
    if save == normal_save:
        return str(save.resolve())
    normal_save.parent.mkdir(parents=True, exist_ok=True)
    if save.is_file() and not save.is_symlink():
        if not normal_save.exists() and normal_save.is_symlink():
            return str(save.resolve())
        if not normal_save.exists():
            # Adopt capture-only progress without copying or overwriting its data.
            normal_save.hardlink_to(save)
        elif save.read_bytes() != normal_save.read_bytes():
            return str(save.resolve())
        # Both paths now retain the same data; replace only the capture pathname.
        save.unlink()
    if not save.exists() and not save.is_symlink():
        save.symlink_to(os.path.relpath(normal_save, save.parent))
    return str(save.resolve())


def ares_background_library():
    """Build the small desktop-only GTK shim once per source revision."""
    source = ROOT / "tools/ares_background.c"
    folder = ROOT / "build/launcher"
    library = folder / f"ares-background-{sha256(source)[:16]}.so"
    if not library.is_file():
        folder.mkdir(parents=True, exist_ok=True)
        temporary = folder / f".{library.name}-{uuid.uuid4().hex}"
        try:
            subprocess.run(["cc", "-shared", "-fPIC", "-O2", "-Wall", "-Wextra", "-Werror",
                            str(source), "-o", str(temporary), "-ldl"], check=True)
            temporary.replace(library)
        finally:
            temporary.unlink(missing_ok=True)
    return library


def ares_background_environment():
    """Protect native benchmark launches while retaining their runtime setup."""
    environment = os.environ.copy()
    library = str(ares_background_library())
    previous = environment.get("LD_PRELOAD")
    environment["LD_PRELOAD"] = library + (":" + previous if previous else "")
    environment.pop("DESKTOP_STARTUP_ID", None)
    environment.pop("XDG_ACTIVATION_TOKEN", None)
    return environment


def ares_background_arguments(arguments):
    """Keep automated runs windowed and mute only ares's final audio output."""
    arguments = [argument for argument in arguments
                 if argument not in ("--fullscreen", "--pseudofullscreen")]
    # Ares restores CLI setting overrides on exit; ordinary playback keeps its
    # saved volume. The mute leaves emulated synthesis and audio pacing active.
    return arguments + ["--setting", "Audio/Mute=true"]


def launch_command(emulator, rom, capture, background=False):
    access = str(ROOT / "build") + (":ro" if emulator == "rmg" else "")
    command = ["flatpak", "run", "--filesystem=" + access]
    if emulator == "ares" and background:
        library = ares_background_library()
        command += [f"--env=LD_PRELOAD={library}", "--unset-env=DESKTOP_STARTUP_ID",
                    "--unset-env=XDG_ACTIVATION_TOKEN"]
    command.append(APPS[emulator])
    if emulator == "rmg":
        command += ["--fullscreen", "--quit-after-emulation"]
        if capture:
            command.append("--debug-messages")
    else:
        if not background:
            command.append("--pseudofullscreen")
        command.append("--no-file-prompt")
        if background:
            command = ares_background_arguments(command)
    return command + [str(rom)]


def record_session(command, folder, duration):
    recording = None
    read_fd, write_fd = os.pipe()
    command.insert(2, f"--instance-id-fd={write_fd}")
    instance_bytes = bytearray()
    instance_id = None
    pending = bytearray()
    started = time.monotonic()
    stopped = None
    stop_reason = "window_closed"
    warned = False
    process = None
    interrupted = False

    def interrupt(signum, frame):
        nonlocal interrupted
        interrupted = True

    previous_handler = signal.signal(signal.SIGINT, interrupt)
    try:
        with (folder / "frames.csv").open("w", newline="") as frames, (folder / "emulator.log").open("wb") as log:
            recording = Recording(frames)
            process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                       pass_fds=(write_fd,), start_new_session=True)
            os.close(write_fd)
            write_fd = None
            with selectors.DefaultSelector() as selector:
                selector.register(read_fd, selectors.EVENT_READ, "instance")
                selector.register(process.stdout, selectors.EVENT_READ, "output")
                while selector.get_map():
                    events = selector.select(0.2)
                    if interrupted:
                        stop_reason = "interrupted"
                        duration = 0
                    elapsed = time.monotonic() - started
                    if duration is not None and elapsed >= duration and stopped is None:
                        stopped = time.monotonic()
                        if stop_reason != "interrupted":
                            stop_reason = "duration"
                        if instance_id:
                            subprocess.run(["flatpak", "kill", instance_id], stdout=subprocess.DEVNULL,
                                           stderr=subprocess.DEVNULL, check=False, timeout=5)
                        else:
                            os.killpg(process.pid, signal.SIGTERM)
                    if stopped is not None and time.monotonic() - stopped > 5 and process.poll() is None:
                        os.killpg(process.pid, signal.SIGKILL)
                    for key, _ in events:
                        chunk = os.read(key.fd, 65536)
                        if not chunk:
                            selector.unregister(key.fileobj)
                            if key.data == "instance":
                                candidate = instance_bytes.decode("ascii", errors="ignore").strip()
                                if re.fullmatch(r"\d+", candidate):
                                    instance_id = candidate
                            elif pending:
                                recording.consume(pending.decode("utf-8", errors="replace"))
                                pending.clear()
                            continue
                        if key.data == "instance":
                            instance_bytes.extend(chunk)
                            # Flatpak writes its short ID atomically, without a newline.
                            candidate = instance_bytes.decode("ascii", errors="ignore").strip()
                            if re.fullmatch(r"\d+", candidate):
                                instance_id = candidate
                            continue
                        log.write(chunk)
                        pending.extend(chunk)
                        while b"\n" in pending:
                            line, _, remainder = pending.partition(b"\n")
                            pending = bytearray(remainder)
                            before = recording.count
                            decoded = line.decode("utf-8", errors="replace")
                            if "SM64GCOV1,BEGIN" in decoded:
                                print("Exporting GCC execution counts.", flush=True)
                            if "SM64GCOV1,END," in decoded:
                                print("GCC profile export finished. Close the emulator, then decode emulator.log.", flush=True)
                            recording.consume(decoded)
                            if before == 0 and recording.count:
                                print("Receiving game frame data.", flush=True)
                        if len(pending) > 1024 * 1024:
                            pending.clear()
                            recording.malformed += 1
                        frames.flush()
                        log.flush()
                    if elapsed > 15 and not warned and not recording.count:
                        print("No game data yet. Focus the emulator window and check that the capture ROM is running.", flush=True)
                        warned = True
            process.wait(timeout=10)
    finally:
        signal.signal(signal.SIGINT, previous_handler)
        os.close(read_fd)
        if write_fd is not None:
            os.close(write_fd)
        if process is not None:
            if process.poll() is None:
                if instance_id:
                    subprocess.run(["flatpak", "kill", instance_id], stdout=subprocess.DEVNULL,
                                   stderr=subprocess.DEVNULL, check=False, timeout=5)
                os.killpg(process.pid, signal.SIGTERM)
                try:
                    process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    os.killpg(process.pid, signal.SIGKILL)
                    process.wait()
            process.stdout.close()
        if recording is not None:
            (folder / "summary.md").write_text(recording.summary())
    return recording, process.returncode, stop_reason


def build_paths(build_flags):
    """Resolve configured outputs without building tools, assets, or the ROM."""
    result = subprocess.run(["make", "--no-print-directory", "-s", "print-build-paths", *build_flags],
                            cwd=ROOT, text=True, capture_output=True, check=False)
    if result.returncode:
        raise ValueError("Cannot resolve build paths: " + (result.stderr or result.stdout).strip())
    values = {}
    for line in result.stdout.splitlines():
        if line.startswith("BUILD_") and "=" in line:
            key, value = line[6:].split("=", 1)
            values[key] = value
    if not values.get("ROM") or not values.get("ELF"):
        raise ValueError("Makefile did not report ROM and ELF paths")
    return ROOT / values.pop("ROM"), ROOT / values.pop("ELF"), values


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("emulator", choices=APPS)
    parser.add_argument("--gcc-profile", action="store_true", help="use the opt-in GCC branch-counter ROM; hold L+R and press Start to export once")
    parser.add_argument("--collision", action="store_true", help="also count collision candidate visits (adds overhead)")
    parser.add_argument("--no-capture", action="store_true", help="build and play the normal ROM for comparison")
    parser.add_argument("--no-build", action="store_true", help="reuse this mode's existing ROM")
    parser.add_argument("-j", "--jobs", type=int, default=default_build_jobs(),
                        help="parallel build jobs (default: available logical CPUs)")
    focus = parser.add_mutually_exclusive_group()
    focus.add_argument("--background", action="store_true",
                       help="open ares without fullscreen or requesting desktop focus")
    focus.add_argument("--foreground", dest="background", action="store_false",
                       help="let ares take focus and open fullscreen (the default)")
    parser.set_defaults(background=False)
    parser.add_argument("--duration", type=float, help="stop this session after this many seconds")
    args = parser.parse_args(argv)
    if args.jobs < 1:
        parser.error("--jobs must be positive")
    if args.gcc_profile and args.no_capture:
        parser.error("--gcc-profile requires capture")
    if args.collision and args.no_capture:
        parser.error("--collision requires capture")
    if args.duration is not None and (not 0 < args.duration < 86400):
        parser.error("--duration must be between 0 and 86400 seconds")
    if args.duration is not None and args.no_capture:
        parser.error("--duration requires capture")
    capture = not args.no_capture
    build_flags = ["VERSION=us", "CONSOLE=n64", f"PERFORMANCE_CAPTURE={int(capture)}",
                   f"COLLISION_PROFILING={int(args.collision)}", f"GCC_PROFILE={int(args.gcc_profile)}"]
    try:
        rom, elf, build_options = build_paths(build_flags)
    except ValueError as error:
        parser.error(str(error))
    if not args.no_build:
        build = subprocess.run(["make", f"-j{args.jobs}", *build_flags], cwd=ROOT, check=False)
        if build.returncode:
            return build.returncode
    if not rom.is_file():
        parser.error(f"ROM not found: {rom}. Build this mode without --no-build.")
    command = launch_command(args.emulator, rom, capture, args.background)
    if not capture:
        return subprocess.call(command)
    if rom.stat().st_size > 0x03FF0000:
        parser.error("This ROM overlaps the emulator's ISViewer channel.")
    running = subprocess.check_output(["flatpak", "ps", "--columns=application"], text=True)
    if APPS[args.emulator] in running.splitlines():
        parser.error("Close the existing emulator first so this recorder owns its game session.")
    save_path = None
    if args.emulator == "ares":
        normal_flags = ["VERSION=us", "CONSOLE=n64", "PERFORMANCE_CAPTURE=0", "COLLISION_PROFILING=0"]
        try:
            normal_rom, _, _ = build_paths(normal_flags)
        except ValueError as error:
            parser.error(str(error))
        save_path = ares_save_path(rom, normal_rom, build_options["SAVETYPE"])
    stamp = datetime.now().astimezone().strftime("%Y-%m-%d_%H-%M-%S")
    folder = research_directory() / "evidence/performance-captures" / f"{stamp}-{args.emulator}-{uuid.uuid4().hex[:8]}"
    folder.mkdir(parents=True)
    metadata = {
        "started": datetime.now().astimezone().isoformat(), "emulator": args.emulator,
        "app_id": APPS[args.emulator],
        **emulator_info(APPS[args.emulator]),
        "rom": str(rom), "rom_bytes": rom.stat().st_size, "rom_sha256": sha256(rom),
        "build_flags": build_flags, "build_options": build_options, "build_skipped": args.no_build,
        "collision_detail": args.collision,
        "gcc_profile": args.gcc_profile,
        "expected_protocol": "SM64PERF1" if args.collision else "SM64PERF2",
        "fields": FIELDS if args.collision else PASSIVE_FIELDS,
        "ticks_per_microsecond": TICKS_PER_MICROSECOND,
        "transport": "ISViewer terminal output; fixed-width hex words",
        "sample_interval_frames": 1 if args.collision else 15,
        "buffered_tail_frames_max": 3 if args.collision else 14,
        "status": "recording", "command": command.copy(),
    }
    if save_path is not None:
        metadata["ares_default_save_path"] = save_path
        metadata["ares_save_type"] = build_options["SAVETYPE"]
    if elf.is_file():
        metadata["elf_sha256"] = sha256(elf)
    if args.gcc_profile:
        notes = elf.parent / "src/engine/surface_collision.gcno"
        if not notes.is_file():
            parser.error("Missing collision .gcno file. Rebuild the GCC_PROFILE=1 ROM.")
        profile_folder = folder / "gcc-profile"
        profile_folder.mkdir()
        (profile_folder / "surface_collision.gcno").write_bytes(notes.read_bytes())
        source = ROOT / "src/engine/surface_collision.c"
        (profile_folder / "surface_collision.c").write_bytes(source.read_bytes())
        metadata["gcc_profile_source_sha256"] = sha256(source)
        metadata["gcc_profile_notes_sha256"] = sha256(notes)
        metadata["gcc_profile_git_commit"] = subprocess.run(
            ["git", "rev-parse", "HEAD"], cwd=ROOT, text=True,
            capture_output=True, check=True).stdout.strip()
        metadata["gcc_profile_worktree_dirty"] = bool(subprocess.run(
            ["git", "status", "--porcelain"], cwd=ROOT, text=True,
            capture_output=True, check=True).stdout.strip())
    metadata_path = folder / "metadata.json"
    metadata_path.write_text(json.dumps(metadata, indent=2) + "\n")
    print(f"Capture folder: {folder}\nClose the emulator window to finish recording.", flush=True)
    exit_code = 1
    try:
        recording, emulator_exit, reason = record_session(command, folder, args.duration)
        metadata.update(samples=recording.count, frames=recording.frame_count,
                        protocols=sorted(recording.protocols), gameplay_frames=recording.gameplay_count,
                        malformed_rows=recording.malformed, missing_samples=recording.gaps,
                        emulator_exit_code=emulator_exit, stop_reason=reason)
        if not recording.count:
            metadata["status"] = "no_data"
            exit_code = 2
            print("Capture failed: no game frame data received.", file=sys.stderr)
        elif emulator_exit and reason == "window_closed":
            metadata["status"] = "emulator_error"
        else:
            metadata["status"] = "complete"
            exit_code = 0
        print(f"Saved {recording.count} samples covering {recording.frame_count} frames. Summary: {folder / 'summary.md'}")
    except (OSError, subprocess.SubprocessError) as error:
        metadata.update(status="error", error=str(error))
        print(f"Capture failed: {error}", file=sys.stderr)
    finally:
        metadata["ended"] = datetime.now().astimezone().isoformat()
        metadata_path.write_text(json.dumps(metadata, indent=2) + "\n")
    return exit_code


if __name__ == "__main__":
    try:
        result = main()
    except (OSError, subprocess.SubprocessError) as error:
        print(f"Cannot start capture: {error}", file=sys.stderr)
        result = 1
    raise SystemExit(result)
