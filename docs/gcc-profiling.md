# GCC execution profiling

Use this tool to find how often collision functions and branches execute on two
repeatable routes. It collects counts, not a speed benchmark.

```sh
python3 tools/profile_gcc.py --jobs 4
```

Use `--scene lab` or `--scene jrb-swim` to select one route. By default both run.
Each launch incrementally builds the separate `GCC_PROFILE=1` ROM with GCC 16.2,
G16 addressing and LTO disabled. The first run or a compiler/build-recipe change
forces that mode's full rebuild. The ordinary ROM remains separate.

## Requirements and collection

This runner currently targets the project's Linux desktop setup. It expects
GCC 16.2 in `/opt/n64-gcc-16.2/bin`, a native ares executable and its runtime
libraries, `gdb-multiarch`, desktop access and a local debugger socket. The default
runtime-library path is workspace-specific and is not included in the source
package. Use `--ares /path/to/ares` and `--runtime /path/to/runtime-libraries` on
another setup. See `--help` for timeout and other options.

It launches ares in the background with private settings/saves and silent speaker
output. The routes walk in Physics Lab and remain underwater in Jolly Roger Bay.
Each warms up for 89 updates, clears counters and records 166 updates. The game
exports on its own thread; the runner verifies sequence, length and checksum,
then decodes with the matching GCC tools. It stops only its own emulator process.

The runner freezes source, route and build identities. Changes during collection
fail the run; launch again to rebuild and collect from a consistent snapshot.

## Reports

The output folder is under
`../../research-archive/<project-folder>/evidence/performance-captures/gcc-profile/`
by default. Set `SM64_RESEARCH_DIR` to change the parent directory.

- `summary.md`: collision function counts.
- Per-scene `reports/`: compressed GCC JSON with line and branch counts.
- `route.json`: location, action and movement evidence.
- Metadata: source/build settings, compiler, ROM/ELF hashes and emulator identity.

Only `surface_collision.c`, including its wall-query include, is instrumented.
The counts describe the two saved routes and source snapshot. Instrumentation and
debugger stops affect timing; use [ordinary performance captures](performance-capture.md)
for speed comparisons. This does not automatically enable production PGO.

For a manual profiling session:

```sh
python3 tools/capture_play.py ares --gcc-profile
```

Hold L+R and press Start to export. Add `--no-build` to reuse an existing profiling
ROM. The format follows GCC's [freestanding profiling workflow](https://gcc.gnu.org/onlinedocs/gcc/Freestanding-Environments.html).
