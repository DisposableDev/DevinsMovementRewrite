# Building the game

Run commands from the repository root. The default build targets the US Nintendo 64 version
with GCC **16.2.0**, F3DZEX graphics microcode, LZ4T compression and 32 KB SRAM.

## Requirements

- GNU Make, a host C/C++ compiler, Python 3, Git and standard Unix build tools.
  On Debian/Ubuntu, install `build-essential`, `python3`, `git` and `bsdextrautils`.
- An N64 GCC 16.2.0 toolchain targeting `mips64-elf`.
- The three baseroms listed below.

The [pinned libdragon toolchain recipe](https://github.com/DragonMinded/libdragon/blob/e356bf3f56f7afbf7e5246329562f145965cfdfc/tools/build-toolchain.sh)
provides the compiler build route. The game uses its existing libultra-based
runtime; installing libdragon does not replace the libraries linked here.
[Build dependencies](build-dependencies.md) records the exact recipe and bundled
linker/SDK inputs. The source package already includes the pinned inherited build
prerequisites and their notices; a separate SDK copy is not required.

Set `N64_INST="$HOME/.local/n64-gcc-16.2"` when running the toolchain recipe.
The Makefile automatically checks this installation directory before searching
`PATH`. No shell configuration or compiler-selection command is needed for
ordinary builds.

Verify the installed compiler:

```sh
"$HOME/.local/n64-gcc-16.2/bin/mips64-elf-gcc" -dumpfullversion
```

The compiler should print `16.2.0`. If you installed it elsewhere, select that
compiler for an individual build:

```sh
make N64_TOOLCHAIN_PREFIX=/path/to/toolchain/bin/mips64-elf-
```

The Makefile selects the bundled GNU 2.30 linker independently of this prefix.

## Baseroms

Place your own big-endian `.z64` files in the repository root:

| File | Version | SHA-1 |
| --- | --- | --- |
| `baserom.us.z64` | US | `9bef1128717f958171a4afac3ed78ee2bb4e86ce` |
| `baserom.eu.z64` | Europe | `4ac5721683d0e0b6bbb561b58a71740845dceea9` |
| `baserom.sh.z64` | Japanese Shindou | `3f319ae697533a255a1003d09202379d78d5a2e0` |

All three are used for extraction, including shared assets. Make invokes
`extract_assets.py` when needed. Baseroms and unchanged extracted ROM assets are
excluded from source packages; custom and imported project assets remain.

## Build and output

```sh
make
```

The default outputs are `build/us_n64/sm64.z64` and `build/us_n64/sm64.elf`.
Plain `make` uses all available CPU cores and selects the US version automatically.
An explicit job limit, such as `make -j2`, takes precedence. To inspect the resolved configuration
and output paths without compiling:

```sh
make -s print-build-paths
```

To inspect the complete build commands without creating or changing build files:

```sh
make -n
```

Dry runs skip asset extraction, lighting conversion and directory creation.
Level rules are preprocessed to stdout only. The extracted assets must already
exist for the complete dependency graph to resolve.

Keep the Makefile defaults when reproducing a release. Audio, profiling and other
configuration changes can select a different output directory. See
[optional components](optional-components.md) before changing build options.

## Common problems

| Problem | What to check |
| --- | --- |
| Cross compiler cannot be found | The installation directory, `PATH`, or an explicit `N64_TOOLCHAIN_PREFIX`; verify the compiler version |
| Extraction fails or a ROM hash differs | All three filenames, big-endian format and hashes above |
| Bundled dependencies are missing or modified | The pinned manifest and recovery importer described in [build dependencies](build-dependencies.md) |
| A retired build option is rejected | Supported options in [optional components](optional-components.md) |
| Source checks need Clang | Clang is used for checks, independently of the N64 GCC compiler |

The supported game build uses `CONSOLE=n64`, `COMPILER=gcc`, `COMPRESS=lz4t`
and `SAVETYPE=sram`. iQue, Clang ROM compilation, alternate ROM packing, HVQM
playback and EEPROM saves are retired. Host MIO0 decoding remains necessary
to extract the baseroms.
