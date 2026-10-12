# Build dependencies and provenance

The source-release package retains the SDK-derived archives, headers, RSP
microcode, runtime libraries and audio encoding tools inherited from HackerSM64,
along with its bundled GNU 2.30 linker. The [prerequisite manifest](../tools/build-dependencies.json) pins
all 92 files; the [release policy](../tools/source-release-policy.json) records
the inclusion decision and upstream attribution. Existing notices are retained.

The normal build uses these bundled dependencies. The sections below identify
their versions, source routes and reproduction limits. Hashes establish file
identity; credits and inclusion do not change inherited rights or notices. See
[licence.md](../licence.md) for the project's licensing scope.

## Dependency overview

| Input | How the build obtains it |
| --- | --- |
| GCC 16.2 and cross Binutils | Installed separately; configure the prefix in [building](building.md) |
| GNU 2.30 linker and `libbfd` | Bundled, with identities pinned in the manifest |
| SDK headers, target archives and RSP inputs | Bundled inherited dependencies, with existing notices |
| SDK audio-tool source | Bundled; Make builds the required host tools |
| Goddard library | Built from repository source into the build directory |

The remaining sections provide exact versions, source recipes, hashes and known
provenance limits for maintainers reproducing or replacing these inputs.

## Compiler and linker

The game compiler is GCC **16.2.0**, target `mips64-elf`, configured for VR4300
with multilib support. The observed cross assembler, archive tool, objcopy and
objdump are GNU Binutils **2.45**. The production linker is separately pinned:
`tools/mips64-elf-ld` is GNU Binutils **2.30**, reporting `elf32ebmip` as its only
supported emulation. It requires `tools/libbfd-2.30.so`; the Makefile supplies
`LD_LIBRARY_PATH=./tools`.

Preserve the current Makefile's compiler and linker settings, linker scripts,
and library order. Do not substitute the cross toolchain's Binutils 2.45 linker
and claim the result is the same build.

The observed GCC configure options, apart from the installation prefix, are:

```
--with-system-zlib --target=mips64-elf --with-arch=vr4300 --with-tune=vr4300
--enable-languages=c,c++ --without-headers --disable-libssp --enable-multilib
--disable-shared --with-gcc --with-newlib --disable-win32-registry
--disable-nls --disable-werror
```

### Pinned compiler source recipe

Use the [libdragon toolchain script at commit e356bf3f56f7afbf7e5246329562f145965cfdfc](https://github.com/DragonMinded/libdragon/blob/e356bf3f56f7afbf7e5246329562f145965cfdfc/tools/build-toolchain.sh).
The script's SHA-256 is:

```
5e1a604d44acd5bf6d92fd0bb5eba0bfaf1b2a8446378227200cf1a9bf2f55de
```

That recipe selects GCC 16.2.0 and Binutils 2.45 and applies the `mips/t-n64` /
`mips/n64.h` multilib and VR4300 driver patch. Configure options alone omit that
patch. Check out the exact commit, verify the script hash, create a private build
directory, and set `N64_INST` and `BUILD_PATH` to private locations before running
the script. Preserve its [license](https://github.com/DragonMinded/libdragon/blob/e356bf3f56f7afbf7e5246329562f145965cfdfc/LICENSE.md).

The script is pinned and hashed. The compiler source archives and a rebuilt
toolchain have not been independently pinned and verified by this project.
Record and verify those archive identities before claiming a newly built
toolchain is reproducible. Installing this toolchain's newlib/runtime does not
remove the game's separately bundled SDK and GCC runtime dependencies.

### Pinned GNU 2.30 linker source route

The official [GNU Binutils repository](https://sourceware.org/git/binutils-gdb.git)
tag `binutils-2_30` has tag object
`9f23e6cb9534f793ad12bb174be3d091ddb753f3`, resolving to source commit
`8db5daf9efe8a6174d3b10ac7bba8c178836e9ce`. Its
[target configuration](https://sourceware.org/git/?p=binutils-gdb.git;a=blob_plain;f=ld/configure.tgt;hb=8db5daf9efe8a6174d3b10ac7bba8c178836e9ce)
selects `elf32ebmip` for `mips*-*-elf*`, matching the current linker.

An out-of-tree build route is to configure that source with
`--target=mips64-elf --enable-shared --disable-nls --disable-werror`, then run
`make all-ld all-bfd`. This establishes the intended source version and MIPS
emulation; the original bundled binaries' complete configure and patch history
has not been recovered. Keep a rebuilt linker private and require fresh ROM
equality before adopting it. No linker replacement was built or adopted here.

The current host linker identities are:

| File | SHA-256 |
| --- | --- |
| `tools/mips64-elf-ld` | `5ae1a0e0ef17ee759381508d32988e4f4c9ea2e6070b8201dbfe965bf2f686e5` |
| `tools/libbfd-2.30.so` | `aea75a293e569aac340fb88b8ee1c93389c702733e0ae254e592a8ffb632d8ec` |

Preserve GNU source packages' `COPYING`, `COPYING3`, and file notices. The
[GPLv3](https://www.gnu.org/licenses/gpl-3.0.html) and
[GCC Runtime Library Exception](https://www.gnu.org/licenses/gcc-exception-3.1.en.html)
are the official notice routes. The exception applies where a file's notice
invokes it; a recipe link does not establish a redistributed binary's complete
corresponding-source provenance.

## Inherited prerequisites

The manifest contains 90 surviving SDK/runtime prerequisites: three target
archives, three opaque SDK object files, five RSP wrappers, ten `.incbin` inputs,
58 SDK headers consumed by fresh build dependencies, and eleven required SDK
audio-tool source/header files. It also locks the two production host
linker/runtime files, `tools/mips64-elf-ld` and `tools/libbfd-2.30.so`, for 92 files
in total. Those two GNU files are separate from the SDK permission question; the
confirmed exact route uses the original bundled binaries. A linker
rebuilt from GNU source has not yet passed a ROM equality check. The manifest
excludes retired HVQM, VC and Puppycam dependencies. The audio wrapper also
preserves its region-specific generated-microcode branch.

The three opaque objects are `lib/rspboot.o`,
`lib/PR/f3dex2/fifo/gspF3DEX2.fifo.o`, and `lib/PR/audio/n_aspMain.o`. The linker
script demands these files even when a microcode alternative contributes no
allocated section. Fresh exported-source builds exposed that requirement; a
list of allocated sections alone misses it. Goddard's archive is built from
checkout source into the build directory and is not an imported cached archive.

These exact files are included as inherited HackerSM64 build dependencies.
Builders supply the three supported baseroms and the compiler separately; they
do not need to acquire another SDK copy for this package. The importer is an
optional local recovery tool, not an SDK download route. The following sections identify known origins and unresolved provenance.

`include/n64/ultra64.h`, `PR/ultratypes.h`, `PR/abi.h`, `PR/mbi.h`, `PR/os.h`, and
several `PR/os_internal*.h` files retain SGI's unpublished proprietary notices
restricting third-party disclosure/copying without prior written permission;
some also retain Nintendo notices. Graphics headers retain SGI notices too.
The consumed set includes the umbrella `include/n64/ultra64.h` as well as 57 PR
headers. Excluding compiled SDK binaries does not clear these source files, or
other notice-bearing headers shipped alongside them. Their original acquisition and redistribution route remains unresolved. They
are retained under the owner's explicit inclusion decision with those notices
and this provenance record unchanged.
The public
[hackerlibultra source](https://github.com/HackerN64/hackerlibultra/tree/590b8faafb85a387d6be557fe31b9a2fbd4907c4)
builds an archive with GCC 16.2 without a reference SDK archive, but its output
already differs from the bundled library's machine code. No redistribution
grant for the inherited SDK files was established from that source tree.

### libnustd and libgcc

The current link uses four libnustd functions: `strncmp`, `strcat`, `memcmp`,
and `memset`. Independent source implementations or selected licensed libc
source could remove the NuSystem archive dependency, but require behavior
validation and would not automatically preserve the current ROM bytes.
Preserve the project ABI: `strncmp` takes a **signed `int n`** in
`include/libc/string.h`; the memory functions' byte counts use 32-bit `size_t`.
Do not silently replace that declaration with an unsigned count.

The bundled divide-break `lib/gcclib/divbreak/libgcc.a` identifies GCC
**11.1.0** in its object comments, despite the game now being compiled with GCC
16.2. Its exact source/configuration history and complete notices remain a
provenance gap. Silently linking GCC 16.2's runtime does not establish exact
reproduction. The [official exception FAQ](https://www.gnu.org/licenses/gcc-exception-3.1-faq.en.html)
distinguishes eligible linked programs from distribution of a standalone
runtime library.

### Audio build tools

The current tools Makefile requires these SDK-tool sources:

```
tools/sdk-tools/adpcm/{vadpcm_enc.c,vpredictor.c,quant.c,util.c,vencode.c,vadpcm.h}
tools/sdk-tools/tabledesign/{codebook.c,estimate.c,print.c,tabledesign.c,tabledesign.h}
```

They feed `vadpcm_enc`, `tabledesign`, and `aiff_extract_codebook`. No license
grant was found in those bounded files; the bundled README and
[upstream repository](https://github.com/n64decomp/sdk-tools) describe a Nintendo
SDK developer-tools decompilation. Public hosting does not supply permission.
Keep their current encoding behavior until any replacement is validated.

The separate `tools/audiofile` headers have LGPL 2.1-or-later notices, and
`audiofile.cpp` contains the LGPL 2.1 text. Retain those notices and the
[LGPL 2.1 license](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html) when
distributing it. That grant does not clear the neighboring SDK-tool sources.
The bundled armips source contains MIT notices; retain notices for any newer
assembler and its declared submodules as well.

## Exact graphics microcode source generation

The [f3dex2 source at commit bd31393fd02b89e024b043c8c50c9a5a10143fb6](https://github.com/Mr-Wiseguy/f3dex2/tree/bd31393fd02b89e024b043c8c50c9a5a10143fb6),
assembled with [armips at commit 62adab4ef30da765f5cf22a451eb08a59c54dc8b](https://github.com/Kingcom/armips/tree/62adab4ef30da765f5cf22a451eb08a59c54dc8b),
reproduces all four bundled F3DZEX code/data blobs byte for byte. Include the
assembler's declared filesystem submodule. The host assembler was built with
GCC 13; the game's N64 compiler was unchanged. Older bundled armips and upstream
0.11 reject the required vector-register macro arguments.

The active `F3DZEX_NoN.bin` / `F3DZEX_NoN_data.bin` pair is upstream
**`F3DZEX_NoN_2.08I`**, despite the Makefile's 2.08J comment. The clipping pair is
**`F3DZEX_2.08J`** with a custom identification string: replace `.NoN` by four
spaces. There are six spaces between `F3DZEX` and `fifo` below. An upstream
`custom.mk` configuration that reproduces the clipping pair is:

```
NAME := F3DZEX_2.08J_header_match
DESCRIPTION := Bundled clipping ucode ID string
ID_STR := RSP Gfx ucode F3DZEX      fifo 2.08J Yoshitaka Yasumoto/Kawasedo 1999.
OPTIONS := CFG_G_BRANCH_W CFG_POINT_LIGHTING
```

Build `F3DZEX_NoN_2.08I` and the custom target with the pinned assembler, then
check generated `.code` / `.data` files against the manifest's corresponding
binary hashes. F3DEX2 point-light and L3DZEX alternatives were not
source-reproduced during this task.

The upstream [CC0 dedication](https://github.com/Mr-Wiseguy/f3dex2/blob/bd31393fd02b89e024b043c8c50c9a5a10143fb6/LICENSE)
does not clear others' underlying rights; paragraph 4(c) explicitly disclaims
that responsibility. Exact generation proves bytes, not Nintendo/SGI
permission. The SDK and provenance questions remain separate from successful
local reconstruction.
