#!/usr/bin/env python3
"""Decode one complete SM64GCOV1 export from an emulator capture log."""

import argparse
from pathlib import Path
import re


def decode(lines):
    payload = bytearray()
    sequence = 0
    started = False
    ended = False
    checksum = 2166136261
    for line in lines:
        offset = line.find("SM64GCOV1,")
        if offset < 0:
            continue
        fields = line[offset:].strip().split(",")
        if fields == ["SM64GCOV1", "BEGIN"]:
            if started:
                raise ValueError("Multiple exports: use a fresh boot and one export per log")
            started = True
        elif len(fields) == 4 and fields[1] == "DATA":
            if not started or ended:
                raise ValueError("Data outside the export")
            if not re.fullmatch(r"[0-9a-f]{8}", fields[2]):
                raise ValueError("Invalid chunk sequence")
            if int(fields[2], 16) != sequence:
                raise ValueError("Missing or out-of-order chunk")
            if not re.fullmatch(r"(?:[0-9a-f]{2}){1,256}", fields[3]):
                raise ValueError("Invalid hexadecimal payload")
            chunk = bytes.fromhex(fields[3])
            for byte in chunk:
                checksum = ((checksum ^ byte) * 16777619) & 0xffffffff
            payload.extend(chunk)
            sequence += 1
        elif len(fields) == 4 and fields[1] == "END":
            if not started or ended:
                raise ValueError("Unexpected end marker")
            if not all(re.fullmatch(r"[0-9a-f]{8}", field) for field in fields[2:]):
                raise ValueError("Invalid export totals")
            if int(fields[2], 16) != len(payload) or int(fields[3], 16) != checksum:
                raise ValueError("Export length or checksum mismatch")
            ended = True
        else:
            raise ValueError("Unrecognized or malformed profile record")
    if not started or not ended or not payload:
        raise ValueError("No complete nonempty GCC profile export found")
    return bytes(payload)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    try:
        with args.log.open(encoding="utf-8", errors="replace") as source:
            payload = decode(source)
    except (OSError, ValueError) as error:
        parser.error(str(error))
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(payload)
    print(f"Decoded {len(payload)} bytes to {args.output}")


if __name__ == "__main__":
    main()
