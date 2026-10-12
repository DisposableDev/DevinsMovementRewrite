#!/usr/bin/env python3
"""Verify and copy explicitly supplied local build dependencies; never download them."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil


def import_dependencies(source, destination, lock):
    source, destination = source.resolve(), destination.resolve()
    pending = []
    records = lock['files'] + lock.get('host_toolchain_inputs', [])
    for record in records:
        name = Path(record['path'])
        if name.is_absolute() or '..' in name.parts:
            raise ValueError('Dependency path must be relative and inside the project')
        original, target = source / name, destination / name
        if not original.resolve().is_relative_to(source):
            raise ValueError(f'Dependency source escapes its folder: {name}')
        if not target.resolve().is_relative_to(destination):
            raise ValueError(f'Dependency destination escapes its folder: {name}')
        expected = record['sha256']
        if hashlib.sha256(original.read_bytes()).hexdigest() != expected:
            raise ValueError(f'Dependency checksum mismatch: {name}')
        if target.exists():
            if hashlib.sha256(target.read_bytes()).hexdigest() != expected:
                raise ValueError(f'Refusing to replace a different dependency: {name}')
        else:
            pending.append((original, target))
    # Validate every input and existing destination before making any changes.
    for original, target in pending:
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(original, target)
    return len(pending)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--from-tree', type=Path, required=True)
    parser.add_argument('--to-tree', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--lock', type=Path,
                        default=Path(__file__).with_name('build-dependencies.json'))
    args = parser.parse_args()
    count = import_dependencies(args.from_tree, args.to_tree,
                                json.loads(args.lock.read_text()))
    print(f'Verified dependencies; copied {count} missing files.')


if __name__ == '__main__':
    main()
