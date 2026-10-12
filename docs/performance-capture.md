# Recording performance captures

The recorder builds a separate diagnostic ROM, launches an emulator and writes
performance samples while you play. The launcher currently uses a Linux desktop
and the Flatpak apps `dev.ares.ares` or `com.github.Rosalie241.RMG`; install the
selected app first. It does not attach to an already-running instance.

## Record a session

Run one of these from the repository root:

```sh
python3 tools/capture_play.py ares
python3 tools/capture_play.py rmg
```

Close another instance of the chosen emulator first. Close the recorder's emulator
window, press Ctrl+C, or use `--duration SECONDS` to finish. The recorder stops
only the instance it launched and writes the available report.

If your shell has the project launcher functions installed, `aplay` selects ares
and `play` selects RMG. Direct commands above do not require those functions.
Ordinary ares launches open fullscreen and take focus. Keep the window focused
while playing if the emulator pauses when unfocused.

| Option | Purpose |
| --- | --- |
| `--no-build` | Reuse this mode's existing ROM |
| `--no-capture` | Build/play the ordinary ROM to compare recording overhead |
| `--collision` | Add per-frame collision candidate counts; increases overhead |
| `--background` | Open ares behind other windows, without taking focus and with speaker output muted |
| `--duration SECONDS` | End the owned emulator session after a time limit |
| `--jobs 4` | Limit build parallelism |

The Makefile supplies the ROM/ELF paths, including configuration suffixes.
Different modes keep separate outputs. The first build of a mode may take longer.
For default ares SRAM locations, capture/ordinary launches reconcile compatible
saves and preserve differing existing files; custom save directories retain
the emulator's behavior.

## Output

The session prints its output directory. By default it is under
`../../research-archive/<project-folder>/evidence/performance-captures/`, outside
the checkout. Set `SM64_RESEARCH_DIR` to choose another parent directory.

| File | Contents |
| --- | --- |
| `summary.md` | Ranked CPU phases and slow gameplay windows |
| `frames.csv` | Frame pacing, phase averages/peaks, available RSP/RDP measurements and scene state |
| `metadata.json` | ROM/ELF hashes, build settings, emulator identity, status and missing samples |
| `emulator.log` | Original output for diagnosing capture failures |

Keep the complete folder when comparing runs or reporting a performance issue.

## Interpretation

Passive recording exports one window per 15 completed frames, without extra
collision candidate hooks. CPU peaks retain brief stalls. The largest measured
phase in a slow window suggests where to investigate; it does not establish
the cause by itself.

Main CPU excludes audio/preemption and debug drawing. Audio/RSP averages use
their own task rings, so the timers overlap and cannot simply be summed. Combined
phase peaks are sums of individual peaks and are labelled upper bounds. RDP
counters use a separate 62.5 MHz clock; unsupported zero counters are unavailable.
The end-of-window action/position and object counts are snapshots, not averages.

Title/demo/credits, pauses and transitional windows remain in the CSV but are
excluded from slowdown ranking. Missing/reset sequences are recorded. Closing
can lose an incomplete passive window of up to 14 completed frames; detailed
mode can lose up to three buffered samples. Sessions without frame data report
`no_data` and return a nonzero exit code.

Use only save states made with the same capture ROM. Recording and collision
instrumentation add overhead; compare the same scene with `--no-capture` when
assessing its impact. These are emulator measurements. See
[N64 performance](n64-performance.md) for backend and hardware limits.
