#!/usr/bin/env python3
"""Check the frozen first public import without rewriting inherited C style."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys

from export_source import inventory

ROOT = Path(__file__).resolve().parents[1]
SCOPE = "tools/source-release-first-import.json"
C_SUFFIXES = {".c", ".h", ".cpp", ".hpp"}


def verify(root, scope):
    policy = json.loads((root / "tools/source-release-policy.json").read_text())
    report = inventory(root, policy)
    if report["block"]:
        raise ValueError("source export has unresolved package decisions")
    expected = (set(scope["files"]) | set(scope["legacy_c"]) | set(scope["empty"])
                | set(scope.get("binary_dependencies", {})) | {SCOPE})
    if set(report["include"]) != expected:
        raise ValueError("first-import source inventory changed; review and refresh the frozen scope")
    for name, item in scope["legacy_c"].items():
        if Path(name).suffix not in C_SUFFIXES or not item.get("reason"):
            raise ValueError("invalid inherited C style exemption")
    for group in ("files", "legacy_c", "empty", "binary_dependencies"):
        for name, item in scope.get(group, {}).items():
            if hashlib.sha256((root / name).read_bytes()).hexdigest() != item["sha256"]:
                raise ValueError(f"first-import source changed: {name}; review and refresh the frozen scope")
    return len(scope["legacy_c"])


def refresh(root):
    """Freeze a reviewed release snapshot; never run this automatically in CI."""
    path = root / SCOPE
    previous = json.loads(path.read_text())
    exceptions = previous.get("preserved_legacy_lines", {})
    policy = json.loads((root / "tools/source-release-policy.json").read_text())
    report = inventory(root, policy)
    if report["block"]:
        raise ValueError("source export has unresolved package decisions")
    scope = {"base": "4b825dc642cb6eb9a060e54bf8d69288fbee4904", "files": {}, "legacy_c": {}, "empty": {}, "binary_dependencies": {},
             "policy": previous["policy"], "preserved_legacy_lines": exceptions}
    for name, record in report["include"].items():
        if name == SCOPE:
            continue
        payload = (root / name).read_bytes()
        digest = hashlib.sha256(payload).hexdigest()
        item = {"sha256": digest}
        if record.get("inherited_dependency") and (Path(name).suffix in {".a", ".o", ".bin", ".so"}
                                                   or name == "tools/mips64-elf-ld"):
            item["reason"] = "Hash-pinned inherited HackerSM64 build prerequisite."
            scope["binary_dependencies"][name] = item
        elif Path(name).suffix in C_SUFFIXES:
            item["reason"] = "Initial import only: preserve exact inherited C style; no C style pass claimed."
            scope["legacy_c"][name] = item
        elif not payload:
            scope["empty"][name] = item
        else:
            excluded = exceptions.get(name, {}).get("lines", [])
            if excluded and previous["files"][name]["sha256"] != digest:
                raise ValueError(f"legacy text exception changed: {name}; review its exact lines before refreshing")
            ranges = []
            for line in range(1, len(payload.splitlines()) + 1):
                if line in excluded:
                    continue
                if ranges and line == ranges[-1][1] + 1:
                    ranges[-1][1] = line
                else:
                    ranges.append([line, line])
            item.update(lines=ranges, reason="Initial import: check all other source text, preserving only documented legacy whitespace lines.")
            scope["files"][name] = item
    path.write_text(json.dumps(scope, indent=2, sort_keys=True) + "\n")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--verify-only", action="store_true")
    parser.add_argument("--refresh-reviewed", action="store_true",
                        help="explicitly freeze the reviewed final import; never used by CI or future changed-range checks")
    args = parser.parse_args()
    try:
        if args.refresh_reviewed:
            refresh(ROOT)
        scope = json.loads((ROOT / SCOPE).read_text())
        count = verify(ROOT, scope)
    except (OSError, KeyError, ValueError) as error:
        print(f"UNAVAILABLE: {error}", file=sys.stderr)
        return 2
    print(f"FIRST IMPORT: {count} exact C files preserve inherited style; no C style pass is claimed.", flush=True)
    if args.verify_only:
        return 0
    return subprocess.call([sys.executable, str(ROOT / "tools/check_quality.py"),
                            "--base", "EMPTY_TREE", "--frontend-only",
                            "--task-scope", str(ROOT / SCOPE)], cwd=ROOT)


if __name__ == "__main__":
    sys.exit(main())
