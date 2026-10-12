# Nintendo 64 performance

This reference explains the production configuration and how to interpret
performance measurements. Use the [capture guide](performance-capture.md) to
record a session and [GCC profiling](gcc-profiling.md) for execution counts.

## Production configuration

The current Makefile uses GCC 16.2, `-Os`, engine G16 small-data addressing and
`--param=logical-op-non-short-circuit=1`. Resident game/engine C units also use
`-mframe-header-opt` and `-fmin-function-alignment=32`; boot/audio retain their
own rules. F3DZEX, current audio settings and the pinned GNU 2.30 linker are part
of the build configuration. Read the Makefile for exact per-file flags.

Use `make -s print-build-paths VERSION=us` to inspect the selected build without
compiling. For comparisons, keep the compiler, linker and production settings
the same and record any measurement-only switches in both builds.

## What the tools measure

| Evidence | Useful for | Limitation |
| --- | --- | --- |
| Game phase timers | Finding expensive CPU work and slow scenes | Timers and task windows can overlap |
| Ares native CPU/RSP execution | Matched CPU, audio and microcode comparisons | Emulator results do not establish console performance |
| Ares cache/device-transfer counters | Investigating exposed memory traffic | Coverage is partial; emulator-only accesses need filtering |
| Ares Vulkan paraLLEl-RDP | Graphics correctness and renderer workload | Does not reproduce console RDP timing or complete RDRAM contention |
| RMG with GLideN64/RSP HLE | Playing and checking the rendered result | HLE bypasses native microcode; host rendering is not RDP timing |
| GCC branch profiles | Finding frequently executed functions/branches | Instrumentation changes timing; counts are not elapsed cost |
| Host regressions | Checking logic and numerical equivalence | Do not measure N64 speed or rendered/controller behavior |

Verify the actual emulator backend for each run. A capped FPS value cannot reveal
headroom, and a zero unsupported counter means unavailable. CPU, audio and RSP
averages may cover different windows; do not add overlapping timers into a
fictitious frame total.

Exposed memory bytes are not total RAMbus usage. RDP command-fetch counts exclude
texture/framebuffer traffic; Vulkan RDP/VI traffic is unavailable in the current
accounting. Cache refill/writeback, uncached access and device DMA are distinct
channels. Emulator coherence or JIT accesses must not be counted as console work.

## Compare a change

1. Reproduce a slow scene and identify the costly phase before choosing a change.
2. Record source/ROM identity, build settings, scene, camera, audio workload and
   emulator version/backends. Keep them matched between control and candidate.
3. Test one small change, allow warm-up and repeat the comparison.
4. Measure the full cost, including setup, retained paths, allocations, cache
   traffic, DMA and extra draws. Fewer operations alone do not establish a saving.
5. Check representative scenes and preserve gameplay, visuals and sound before
   accepting the result. Report measurement coverage and unresolved limitations.

Nintendo 64 resources share deadlines and memory. CPU cache locality, graphics
and audio RSP tasks, DMA, RDP pixel coverage, blending and synchronization can
move the cost elsewhere. Reduced opacity or fewer vertices need not reduce the
limiting hardware work. Use console measurements when claiming console speedups.

## Hardware references

The Nintendo manuals mirrored at Ultra64 provide these starting points:

- [CPU specifications and caches](https://ultra64.ca/files/documentation/online-manuals/man/kantan/step1/2-2.html),
  [memory, cache writeback and DMA alignment](https://ultra64.ca/files/documentation/online-manuals/man-v5-1/pro-man/pro03/03-06.htm).
- [RSP units and local memory](https://ultra64.ca/files/documentation/online-manuals/man/pro-man/pro03/03-03.html),
  [RSP-to-RDP command passing](https://ultra64.ca/files/documentation/online-manuals/man/pro-man/pro25/25-02.html).
- [RDP synchronization and framebuffer work](https://ultra64.ca/files/documentation/online-manuals/man-v5-1/pro-man/pro12/12-02.htm),
  [texture memory and reuse](https://ultra64.ca/files/documentation/online-manuals/man-v5-1/pro-man/pro13/13-08.htm).
- [Audio/graphics scheduling and deadlines](https://ultra64.ca/files/documentation/online-manuals/man/pro-man/pro22/22-01.html).

Use the section relevant to the resource being changed and original emulator or
microcode source for implementation-specific details.
