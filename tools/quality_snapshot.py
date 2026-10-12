"""Conservative, index-only dependencies for isolated quality checks."""

import os
from pathlib import PurePosixPath
import posixpath
import re
import subprocess


class SnapshotError(Exception):
    pass


def index_entries(data):
    entries = {}
    for item in data.split(b"\0"):
        if not item:
            continue
        metadata, name = item.split(b"\t", 1)
        mode, oid, stage = metadata.decode().split()
        if stage != "0":
            raise SnapshotError("resolve index conflicts before checking a staged snapshot")
        entries[os.fsdecode(name)] = (mode, oid)
    return entries


def blobs(repository, entries, names):
    """Read one batch of immutable Git objects, never working-tree dependencies."""
    oids = list(dict.fromkeys(entries[name][1] for name in names))
    result = subprocess.run(["git", "cat-file", "--batch"], cwd=repository,
                            input="".join(oid + "\n" for oid in oids).encode(), capture_output=True)
    if result.returncode:
        raise SnapshotError(result.stderr.decode(errors="replace"))
    output, offset, content = result.stdout, 0, {}
    for oid in oids:
        end = output.index(b"\n", offset)
        header = output[offset:end].split()
        if len(header) != 3 or header[0].decode() != oid or header[1] != b"blob":
            raise SnapshotError("invalid index dependency object")
        size = int(header[2])
        content[oid] = output[end + 1:end + 1 + size]
        offset = end + size + 2
    return {name: content[entries[name][1]] for name in names}


def dependencies(repository, entries, selected, parsed):
    needed = set(selected) | {".quality.json", ".clang-format", ".clang-tidy"}
    if not parsed:
        return needed & entries.keys()
    # Owner discovery and extraction adapters inspect source across translation
    # units. Keep that conservative input universe, but omit build products/media.
    source_suffixes = {".c", ".cpp", ".h", ".hpp", ".in", ".inc", ".def", ".s", ".S", ".lds"}
    for name in entries:
        path = PurePosixPath(name)
        support = path.parts[0] in {"tools", "tests"} and path.suffix == ".py"
        build = path.name.startswith("Makefile") or path.suffix == ".mk"
        data = path.suffix in {".json", ".txt", ".csv"} or name == "assets/rom/arrays.json.gz"
        texture = path.parts[0] in {"actors", "levels", "textures", "assets"} and path.suffix == ".png"
        if path.suffix in source_suffixes or support or build or data or texture:
            needed.add(name)
    # Git searches immutable index blobs in C, avoiding Python scans of large
    # generated arrays. Literal includes also retain unusually named input files.
    result = subprocess.run(["git", "grep", "--cached", "-I", "-z", "-E", "-e",
                             r'^[[:space:]]*#[[:space:]]*include[[:space:]]+', "--",
                             "*.c", "*.cpp", "*.h", "*.hpp", "*.in"],
                            cwd=repository, capture_output=True)
    if result.returncode not in {0, 1}:
        raise SnapshotError(result.stderr.decode(errors="replace"))
    output, offset, macros = result.stdout, 0, set()
    include_dirs = ("", "include", "src", "src/game", "src/engine", "src/audio",
                    "src/menu", "src/goddard", "lib", "lib/libpl", "lib/ultralib/include")
    while offset < len(output):
        end = output.index(b"\0", offset)
        name = os.fsdecode(output[offset:end])
        finish = output.index(b"\n", end + 1)
        line = output[end + 1:finish]
        offset = finish + 1
        match = re.match(rb'\s*#\s*include\s*["<]([^">]+)[">]', line)
        if not match:
            macros.add(name)
            continue
        target = os.fsdecode(match[1])
        for directory in (posixpath.dirname(name), *include_dirs):
            candidate = posixpath.normpath(posixpath.join(directory, target))
            if candidate in entries:
                needed.add(candidate)
                break
    # A macro can construct any filename. Known S2DEX macros have a bounded font
    # family already in the source universe; unknown forms need a full snapshot.
    names = sorted(macros | ({"src/s2d_engine/s2d_config.h"} & entries.keys()))
    for name in names:
        if entries[name][0] not in {"100644", "100755"}:
            raise SnapshotError(f"expected a regular index dependency: {name!r}")
    contents = blobs(repository, entries, names)
    config = contents.get("src/s2d_engine/s2d_config.h", b"")
    font_macros = all(re.search(pattern, config) for pattern in (
        rb'#define\s+__headername\(hd\)\s+fonts/hd\.h\s*(?:\n|$)',
        rb'#define\s+__fontinclude\(f\)\s+src/s2d_engine/fonts/f\.c\s*(?:\n|$)',
        rb'#define\s+_FONT_HEADER\s+STR2\(__headername\(FONTNAME\)\)\s*(?:\n|$)',
        rb'#define\s+FONT_C_FILE\s+STR2\(__fontinclude\(FONTNAME\)\)\s*(?:\n|$)'))
    for name in macros:
        content = re.sub(rb'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|//[^\n]*|/\*.*?\*/',
                         lambda m: b" " if m[0].startswith((b"//", b"/*")) else m[0],
                         contents[name], flags=re.S)
        for match in re.finditer(rb'^\s*#\s*include\s*([^\n]+)', content, re.M):
            if not re.match(rb'["<]', match[1]):
                if not font_macros or match[1].strip() not in {b"_FONT_HEADER", b"FONT_C_FILE"}:
                    return set(entries)
    return needed & entries.keys()


def link_destination(name, content):
    """Accept relative file links without allowing a dependency outside the snapshot."""
    target = os.fsdecode(content)
    destination = posixpath.normpath(posixpath.join(posixpath.dirname(name), target))
    if (not target or "\0" in target or posixpath.isabs(target)
            or destination == ".." or destination.startswith("../")):
        raise SnapshotError(f"expected an internal relative file link: {name!r}")
    return destination


def materialize(repository, snapshot, index, selected, parsed=False, complete=False):
    entries = index_entries(index)
    names = set(entries) if complete else dependencies(repository, entries, selected, parsed)
    links = [name for name in names if entries[name][0] == "120000"]
    for name, content in blobs(repository, entries, links).items():
        target = link_destination(name, content)
        if target not in entries or entries[target][0] not in {"100644", "100755"}:
            raise SnapshotError(f"expected a regular index link target: {name!r}")
        names.add(target)
    for name in names:
        path = PurePosixPath(name)
        if (path.is_absolute() or ".." in path.parts
                or entries[name][0] not in {"100644", "100755", "120000"}):
            raise SnapshotError(f"expected a regular index dependency: {name!r}")
    result = subprocess.run(["git", "checkout-index", "-z", "--stdin",
                             "--prefix=" + str(snapshot) + os.sep], cwd=repository,
                            input=b"".join(os.fsencode(name) + b"\0" for name in sorted(names)),
                            capture_output=True)
    if result.returncode:
        raise SnapshotError(result.stderr.decode(errors="replace"))
    current = subprocess.check_output(["git", "ls-files", "--stage", "-z"], cwd=repository)
    if current != index:
        raise SnapshotError("index changed while copying dependencies; rerun the check")
    return len(names), len(entries)
