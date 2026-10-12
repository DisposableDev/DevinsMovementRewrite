# Optional components

The normal game build uses Nintendo 64, GCC 16.2, F3DZEX graphics microcode,
LZ4T compression and 32 KB SRAM. This guide describes optional routes retained
in the source; an available setting is not a claim that every combination has
been built or tested.

## Build and platform options

| Component | Use and status |
| --- | --- |
| Graphics microcode alternatives | Retained Makefile routes; F3DZEX is the production choice |
| Parallel Launcher library, `LIBPL=1` | Optional capability queries through `lib/libpl`; off by default |
| ARM host linker | Bundled host route checks for `arm`; general ARM64 compatibility is unverified |
| Division runtime, `LIBGCCDIR` | Selects divide-break, trap or unchecked behavior; `divbreak` is current |
| SDK diagnostic libraries | IS-Viewer/USB printing alternatives to the current `ultra_rom` library |
| USB/UNFLoader, `UNF=1` | Compatible flashcart loading and debugging; off by default |
| IS-Viewer, `ISVPRINT=1` | Diagnostic output in compatible emulators; separate from crash symbols |
| S2DEX text, `TEXT_ENGINE=s2dex_text_engine` | Retained selector; required microcode linking is incomplete/unverified |
| LTO, `ENGINE_LTO=1` | Experimental route; off and separate from production G16 addressing |
| GCC profiling, `GCC_PROFILE=1` | Collision execution-count collection in a separate diagnostic build |
| Better Reverb | Retained source option, disabled; configuration safeguards limit it to US/JP |

Check [Makefile](../Makefile), [host-tool rules](../tools/Makefile),
[linker script](../sm64.ld), [RSP selections](../rsptext.inc.ld) and
[audio configuration](../include/config/config_audio.h) before enabling a route.
The current HUD/menus already use their existing renderer; enabling an S2DEX
selector alone does not integrate it or improve text quality.

## Authoring and development tools

| Tool | Purpose |
| --- | --- |
| Fast64 and `fast64.json` | Model/level export and asset authoring |
| Editor configuration | Navigation, completion and include paths; does not set production flags |
| Clang quality tools | Formatting and parsed source checks; separate from N64 GCC compilation |
| Regression tests | Host checks of behavior, assets and tool contracts |
| Capture/benchmark tools | Emulator measurements and matched development experiments |
| Physics Lab preview tool | Source-geometry maps using Pillow/NumPy |
| `llm/` | Optional agent instructions, plans and development history |

These tools are not required merely to link the ROM. See [contributing](contributing.md),
[quality checks](source-quality-standards.md), [captures](performance-capture.md)
and [profiling](gcc-profiling.md) for their use.

## Retired configurations

iQue, Clang game compilation, non-LZ4T ROM compression, HVQM playback, EEPROM
saves and Puppycam are retired. Clang remains the source-checking frontend;
`COMPILER=clang quality-flags` is an analysis-only query, not a Clang ROM build.
Host MIO0 decoding remains necessary for baserom extraction.

ReonuCam, first-person controls, player options, Debug Mode, Physics Lab and
PuppyPrint remain available. A retired developer configuration does not remove
its unrelated player-facing features.
