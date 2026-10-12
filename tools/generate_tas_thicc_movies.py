#!/usr/bin/env python3
"""Create two deterministic Mupen input recipes and isolated EEPROM fixtures.

These are generated inputs, not emulator recordings. See the accompanying README
for the target configuration and the emulator verification still required.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "docs/verification/tas-thicc"
BOOT_SAMPLES = 300
TAIL_SAMPLES = 180
CODE = [3, 12] * 32  # C-Left+Right, then C-Up+Down; ordinary N64 button bits.


def signed_block(data, magic):
    struct.pack_into(">H", data, len(data) - 4, magic)
    struct.pack_into(">H", data, len(data) - 2, sum(data[:-2]) & 0xFFFF)
    return data


def eeprom(file_select):
    # Current 25-course, 15-stage SaveFile: 56 bytes, two copies per file.
    # Main menu data is deliberately invalid so the game initializes its settings.
    data = bytearray(512)
    for file_index in range(4):
        block = bytearray(56)
        flags = 0
        if file_select and file_index == 2:
            # Pending unlock keeps file select reachable with SKIP_FILE_SELECT.
            # Occupied C witnesses that the shortcut preserves existing progress.
            flags = (1 << 21) | (1 << 5) | 1
            block[12] = 5  # BOB stars 1 and 3.
            block[37] = 83  # BOB coin record.
        struct.pack_into(">I", block, 8, flags)
        signed_block(block, 0x4441)
        for copy in range(2):
            offset = (file_index * 2 + copy) * 56
            data[offset:offset + 56] = block
    return data


def movie(name, inputs, rom=None):
    header = bytearray(1024)
    header[:4] = b"M64\x1a"
    struct.pack_into("<I", header, 4, 3)
    header[8:12] = hashlib.sha256(name.encode()).digest()[:4]
    struct.pack_into("<I", header, 12, len(inputs) * 2)
    header[20:22] = bytes((60, 1))
    struct.pack_into("<I", header, 24, len(inputs))
    struct.pack_into("<H", header, 28, 4)  # Reset while retaining prepared EEPROM.
    struct.pack_into("<I", header, 32, 1)  # Controller 1 present.
    if rom is not None:
        header[196:216] = rom[32:52]
        struct.pack_into("<I", header, 228, struct.unpack_from(">I", rom, 16)[0])
        struct.pack_into("<H", header, 232, rom[62])
    else:
        header[196:216] = b"HackerSM64          "
    author = b"Generated verification input recipe"
    header[546:546 + len(author)] = author
    description = (
        f"{name}: generated, NOT emulator-verified. Install matching EEPROM fixture; "
        "current US debug configuration. Alternate CL+CR / CU+CD for 32 pairs."
    ).encode()
    header[768:768 + len(description)] = description
    return header + b"".join(struct.pack("<I", buttons) for buttons in inputs)


def artifacts(rom=None):
    result = {}
    scenarios = {
        "file-select": [0] * BOOT_SAMPLES,
        "pause-menu": [0] * BOOT_SAMPLES + [0x1000] + [0] * 30,
    }
    manifest = {
        "status": "generated input recipes; host replay checked, emulator playback not verified",
        "format": "Mupen M64 v3, start from prepared EEPROM, US 60 VI/s",
        "rom_sha256": hashlib.sha256(rom).hexdigest() if rom is not None else None,
        "rom_binding": "unbound until regenerated with --rom" if rom is None else "header and hash recorded",
        "movies": {},
    }
    for name, lead in scenarios.items():
        inputs = lead + CODE + [0] * TAIL_SAMPLES
        result[f"{name}.m64"] = movie(name, inputs, rom)
        result[f"{name}.eep"] = eeprom(name == "file-select")
        manifest["movies"][name] = {
            "samples": len(inputs),
            "code_first_sample_zero_based": len(lead),
            "unlock_sample_zero_based": len(lead) + 63,
            "expected": "File C, TAS THICC, castle grounds area 1 node 0x32, no intro",
        }
    result["manifest.json"] = (json.dumps(manifest, indent=2) + "\n").encode()
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rom", type=Path, help="bind movies to a newly built big-endian .z64 ROM")
    parser.add_argument("--check", action="store_true", help="check generated artifacts without writing")
    args = parser.parse_args()
    rom = args.rom.read_bytes() if args.rom else None
    if rom is not None and (len(rom) < 64 or rom[:4] != bytes.fromhex("80371240")):
        parser.error("--rom must be a big-endian N64 .z64 ROM")
    expected = artifacts(rom)
    if args.check:
        stale = [name for name, data in expected.items()
                 if not (OUTPUT / name).exists() or (OUTPUT / name).read_bytes() != data]
        if stale:
            parser.exit(1, "Stale/missing artifacts: " + ", ".join(stale) + "\n")
        print("Both movie recipes and EEPROM fixtures match the generator.")
    else:
        OUTPUT.mkdir(parents=True, exist_ok=True)
        for name, data in expected.items():
            (OUTPUT / name).write_bytes(data)
        print(f"Generated two movie recipes in {OUTPUT}; emulator playback is not verified.")


if __name__ == "__main__":
    main()
