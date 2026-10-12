#!/bin/sh
set -eu

toolchain_prefix=${1:?Supply the cross-toolchain prefix}
compiler="${toolchain_prefix}gcc"
linker="${toolchain_prefix}ld"
compiler_version=$("$compiler" -dumpfullversion)
linker_version=$("$linker" -v)

printf '%s\n' '#ifndef VER_DATA_H' '#define VER_DATA_H'
printf '%s\n' '#define __USERNAME__  "anonymous"'
printf '#define __COMPILER__  "%s version %s"\n' "${compiler##*/}" "$compiler_version"
printf '#define __LINKER__    "%s: %s"\n' "${linker##*/}" "$linker_version"
printf '%s\n' '#endif'
