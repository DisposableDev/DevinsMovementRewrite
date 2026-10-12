#!/usr/bin/env python3
"""Audit/export present Git source paths without touching the index or history.

A reviewed policy must resolve every blocker before an archive can be written.
The audit does not establish redistribution rights.
"""
import argparse
import gzip
import hashlib
import io
import json
import os
from pathlib import Path, PurePosixPath
import posixpath
import stat
import subprocess
import tarfile


def git(root, *args, data=None):
    return subprocess.check_output(["git", *args], cwd=root, input=data)


def valid_name(name):
    path = PurePosixPath(name)
    if path.is_absolute() or ".." in path.parts or not name or "\\" in name:
        raise ValueError(f"unsafe archive path: {name!r}")
    if any(ord(char) < 32 for char in name):
        raise ValueError(f"control character in path: {name!r}")
    return path


def compare_inventories(previous, current, baseline_sha256):
    """Describe package changes without choosing tests or rebuilding the game."""
    if previous.get("schema") != current["schema"] or not isinstance(previous.get("include"), dict):
        raise ValueError("previous inventory has an unsupported schema or no included-file map")
    before, after = previous["include"], current["include"]
    shared = before.keys() & after.keys()
    changed = sorted(name for name in shared if before[name] != after[name])
    excluded_before, excluded_after = previous.get("exclude", {}), current["exclude"]
    metadata = ("gitlinks", "optional_llm", "inherited_dependencies", "provenance_notes", "publication_blockers")
    return {"baseline_sha256": baseline_sha256, "baseline_git_head": previous.get("git_head"),
            "added": sorted(after.keys() - before.keys()),
            "changed": changed, "removed": sorted(before.keys() - after.keys()),
            "unchanged": len(shared) - len(changed),
            "exclusion_changes": sorted(name for name in excluded_before.keys() | excluded_after.keys()
                                        if excluded_before.get(name) != excluded_after.get(name)),
            "metadata_changes": [key for key in metadata if previous.get(key) != current.get(key)]}


def inventory(root, policy, optional_llm=False):
    inheritance = policy.get("inherited_dependencies", {})
    dependencies = {}
    if inheritance:
        if not inheritance.get("decision") or not inheritance.get("origin"):
            raise ValueError("inherited dependencies need an explicit inclusion decision and origin")
        manifest = valid_name(inheritance["manifest"])
        dependencies = {item["path"]: item for item in
                        json.loads((root / str(manifest)).read_text())["files"]}
        for name in dependencies:
            valid_name(name)
    raw = git(root, "ls-files", "--cached", "--others", "--exclude-standard", "-z")
    names = sorted(set(os.fsdecode(name) for name in raw.split(b"\0") if name))
    attrs = git(root, "check-attr", "-z", "--stdin", "export-ignore",
                data=b"\0".join(os.fsencode(name) for name in names) + b"\0")
    fields = attrs.split(b"\0")
    ignored = {os.fsdecode(fields[i]) for i in range(0, len(fields)-1, 3)
               if fields[i+2] == b"set"}
    registry = json.loads((root / "assets.json").read_text())
    originals = {name for name in registry if not name.startswith("@")}
    audio = root / "assets/rom/audio.json"
    if audio.is_file():
        originals.update(recipe["path"] for recipe in json.loads(audio.read_text())["recipes"])
    decision = {kind: {} for kind in ("include", "exclude", "block")}
    staged = git(root, "ls-files", "--stage", "-z")
    links = {}
    for item in staged.split(b"\0"):
        if item:
            metadata, name = item.split(b"\t", 1)
            mode, oid, stage = metadata.decode().split()
            if stage != "0":
                raise ValueError("unresolved index conflicts")
            if mode == "160000":
                links[os.fsdecode(name)] = oid
    for name in names:
        path = valid_name(name)
        source = root / name
        if name in links:
            decision["exclude"][name] = "gitlink: fetch pinned external source via .gitmodules"
            continue
        try:
            info = source.lstat()
        except FileNotFoundError:
            decision["exclude"][name] = "deleted in selected working tree"
            continue
        if name in ignored:
            reason = "gitattributes export-ignore"
        elif name == "SOURCE-MANIFEST.json":
            reason = "generated export identity metadata; recreated for each archive"
        elif name in originals:
            reason = "original extracted asset registry"
        elif name.startswith("assets/rom/generated/"):
            reason = "ROM-reconstructed generated initializer output"
        elif not optional_llm and (name.startswith("llm/") or name in {"AGENTS.md", ".codex/config.toml"}):
            reason = "optional agent documentation/configuration distribution"
        elif path.parts[0] in {".git", ".quality-tools", "build", "output", "performance-captures", "benchmark-results", ".vscode"}:
            reason = "local/build/capture output"
        elif "__pycache__" in path.parts or path.suffix.lower() in {".z64", ".v64", ".n64", ".rom", ".eeprom", ".sram", ".sav", ".state", ".pyc", ".log"}:
            reason = "ROM/save/generated log or local output"
        elif path.suffix.lower() in {".o", ".a", ".elf", ".bin", ".exe", ".dll", ".so", ".dylib"} and name not in dependencies:
            reason = "ROM/save/generated binary/log or unresolved SDK binary"
        elif name in policy.get("exclude", {}):
            reason = policy["exclude"][name]
        else:
            reason = None
        if reason:
            decision["exclude"][name] = reason
            continue
        if not (stat.S_ISREG(info.st_mode) or stat.S_ISLNK(info.st_mode)):
            decision["block"][name] = "unsupported source entry"
        elif stat.S_ISLNK(info.st_mode):
            if name in dependencies:
                decision["block"][name] = "hash-pinned inherited dependency must be a regular file"
                continue
            target = os.readlink(source)
            resolved = posixpath.normpath(posixpath.join(posixpath.dirname(name), target))
            if PurePosixPath(target).is_absolute() or resolved == ".." or resolved.startswith("../"):
                decision["block"][name] = "symlink escapes archive root"
            else:
                decision["include"][name] = {"symlink": target, "target": resolved}
        elif path.suffix.lower() in {".png", ".aiff", ".wav", ".s16be", ".mp3", ".ogg", ".mp4", ".m64"} and name not in policy.get("allow_media", {}):
            decision["block"][name] = "media provenance needs explicit reviewed allow or exclude entry"
        else:
            payload = source.read_bytes()
            digest = hashlib.sha256(payload).hexdigest()
            if payload[:4] in {b"\x80\x37\x12\x40", b"\x37\x80\x40\x12", b"\x40\x12\x37\x80"}:
                decision["exclude"][name] = "ROM detected by header"
            elif name in dependencies and digest != dependencies[name]["sha256"]:
                decision["block"][name] = "inherited dependency differs from its reviewed hash"
            elif name not in dependencies and (payload[:4] in {b"\x7fELF", b"\xcf\xfa\xed\xfe", b"\xfe\xed\xfa\xcf"} or payload[:2] == b"MZ"):
                decision["exclude"][name] = "compiled executable or ROM detected by header"
            else:
                decision["include"][name] = {"sha256": digest,
                                             "mode": "0755" if info.st_mode & 0o111 else "0644"}
                if name in dependencies:
                    decision["include"][name]["inherited_dependency"] = dependencies[name]["category"]
    for name, record in list(decision["include"].items()):
        if "target" in record and record["target"] not in decision["include"]:
            decision["block"][name] = "symlink target is absent from export"
            del decision["include"][name]
    for name in dependencies:
        if name not in decision["include"]:
            decision["block"].setdefault(name, "required inherited dependency is missing or excluded")
    publication_blockers = []
    restricted_headers = [name for name in decision["include"] if name.startswith("include/n64/")
                          and "proprietary" in (root / name).read_text(errors="replace").lower()]
    sdk_tools = [name for name in decision["include"] if name.startswith("tools/sdk-tools/")]
    if restricted_headers:
        publication_blockers.append({"reason": "Required SDK headers carry proprietary/disclosure restrictions; redistribution permission or replacement is unresolved",
                                     "paths": sorted(restricted_headers)})
    if sdk_tools:
        publication_blockers.append({"reason": "Required audio-tool sources are identified as Nintendo SDK decompilation; no redistribution license is recorded",
                                     "paths": sorted(sdk_tools)})
    provenance_notes = publication_blockers if inheritance else []
    if inheritance:
        publication_blockers = []
    return {"schema": 1, "git_head": git(root, "rev-parse", "HEAD").decode().strip(),
            "gitlinks": links, "optional_llm": optional_llm,
            "inherited_dependencies": inheritance, "provenance_notes": provenance_notes,
            "publication_blockers": publication_blockers, **decision}


def archive(root, report, output, private_checkpoint=False):
    if report["block"]:
        raise ValueError("unresolved release blockers; archive was not written")
    if report.get("publication_blockers") and not private_checkpoint:
        raise ValueError("unresolved source provenance; only an explicit --private-checkpoint archive can be written")
    if output.exists():
        raise ValueError("refusing to overwrite an existing archive")
    with output.open("xb") as stream:
        with gzip.GzipFile(fileobj=stream, mode="wb", filename="", mtime=0) as compressed:
            with tarfile.open(fileobj=compressed, mode="w", format=tarfile.PAX_FORMAT) as tar:
                for name, record in sorted(report["include"].items()):
                    item = tarfile.TarInfo("my-first-hack/" + name)
                    item.mtime = item.uid = item.gid = 0
                    item.uname = item.gname = ""
                    if "symlink" in record:
                        item.type, item.linkname, item.mode = tarfile.SYMTYPE, record["symlink"], 0o777
                        tar.addfile(item)
                    else:
                        payload = (root / name).read_bytes()
                        if hashlib.sha256(payload).hexdigest() != record["sha256"]:
                            raise ValueError(f"source changed since audit: {name}")
                        item.mode, item.size = int(record["mode"], 8), len(payload)
                        tar.addfile(item, io.BytesIO(payload))
                # Tar exports have no Git objects; retain dependency pins and
                # the exact working-source hashes inside the artifact itself.
                manifest = json.dumps({"schema": report["schema"], "git_head": report["git_head"],
                                       "gitlinks": report["gitlinks"], "files": report["include"],
                                       "private_checkpoint": private_checkpoint,
                                       "inherited_dependencies": report.get("inherited_dependencies", {}),
                                       "provenance_notes": report.get("provenance_notes", []),
                                       "changes_from_previous": report.get("changes_from_previous"),
                                       "publication_blockers": report.get("publication_blockers", [])},
                                      indent=2, sort_keys=True).encode() + b"\n"
                item = tarfile.TarInfo("my-first-hack/SOURCE-MANIFEST.json")
                item.mtime = item.uid = item.gid = 0
                item.uname = item.gname = ""
                item.mode, item.size = 0o644, len(manifest)
                tar.addfile(item, io.BytesIO(manifest))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--policy", type=Path, required=True)
    parser.add_argument("--report", type=Path, required=True)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--optional-llm", action="store_true")
    baseline = parser.add_mutually_exclusive_group()
    baseline.add_argument("--previous-inventory", type=Path,
                          help="compare against the previous package inventory; required for re-packaging")
    baseline.add_argument("--initial-package", action="store_true",
                          help="explicit first package when no previous inventory exists")
    parser.add_argument("--private-checkpoint", action="store_true",
                        help="preserve unresolved required sources in a private review archive; publication remains blocked")
    args = parser.parse_args()
    if args.output and not (args.previous_inventory or args.initial_package):
        parser.error("re-packaging requires --previous-inventory; use --initial-package only for the first package")
    root = args.root.resolve()
    if args.previous_inventory and args.report.resolve() == args.previous_inventory.resolve():
        parser.error("preserve the previous inventory; write the new report to a different path")
    for output in (args.report, args.output):
        if output and output.resolve().is_relative_to(root):
            parser.error("write audit reports and archives outside the source tree")
    report = inventory(root, json.loads(args.policy.read_text()), args.optional_llm)
    if args.previous_inventory:
        payload = args.previous_inventory.read_bytes()
        report["changes_from_previous"] = compare_inventories(
            json.loads(payload), report, hashlib.sha256(payload).hexdigest())
    args.report.write_text(json.dumps(report, indent=2, sort_keys=True) + "\n")
    print(json.dumps({key: len(report[key]) for key in ("include", "exclude", "block", "publication_blockers")}))
    if "changes_from_previous" in report:
        changes = report["changes_from_previous"]
        print(json.dumps({"package_changes": {key: len(changes[key]) for key in ("added", "changed", "removed")},
                          "unchanged": changes["unchanged"]}))
    if args.output:
        archive(root, report, args.output, private_checkpoint=args.private_checkpoint)


if __name__ == "__main__":
    main()
