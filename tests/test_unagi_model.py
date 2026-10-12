#!/usr/bin/env python3
"""Preserve Unagi's native draws while sharing the body material."""
from pathlib import Path
import hashlib
import json
import re
import unittest

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]
PARTS = ["unagi_seg5_dl_" + suffix for suffix in (
    "0500D268", "0500D468", "0500D668", "0500D828", "0500D050",
    "0500DEB8", "0500E258", "0500CDD0", "0500E088", "0500DD08")]


def native_draws(source):
    arrays = {name: (kind, body) for kind, name, body in re.findall(
        r"\b(Vtx|Gfx)\s+(\w+)\[\]\s*=\s*\{(.*?)\n\};", source, re.S)}
    cache = {}
    state = {}
    geometry = {"G_LIGHTING"}
    tiles = {}
    image = None
    texture = None
    draws = []

    def visit(name, part):
        nonlocal image, texture
        for command, arguments in re.findall(r"(gs\w+)\((.*?)\),", arrays[name][1], re.S):
            fields = tuple(re.sub(r"\s+", "", field) for field in arguments.split(","))
            if command == "gsSPDisplayList":
                visit(fields[0], part)
            elif command == "gsSPVertex":
                rows = re.findall(r"\{\{\{.*?\}\}\}", arrays[fields[0]][1])
                count, start = int(fields[1]), int(fields[2])
                if count > len(rows) or start + count > 32:
                    raise AssertionError("Invalid vertex-cache load")
                for index, row in enumerate(rows[:count], start):
                    cache[index] = tuple(int(value, 0) for value in re.findall(
                        r"0x[\da-fA-F]+|-?\d+", row))
            elif command in ("gsSP1Triangle", "gsSP2Triangles"):
                for offset in range(0, len(fields), 4):
                    vertices = tuple(cache[int(index)] for index in fields[offset:offset + 3])
                    draws.append((part, vertices, int(fields[offset + 3], 0), dict(state),
                                  sorted(geometry), dict(tiles), texture))
            elif command == "gsDPSetTextureImage":
                image = fields
            elif command == "gsDPLoadBlock":
                texture = (image, fields)
            elif command == "gsDPSetTile":
                tiles["tile" + fields[4]] = fields
            elif command == "gsDPSetTileSize":
                tiles["size" + fields[0]] = fields
            elif command == "gsSPSetGeometryMode":
                geometry.update(fields[0].split("|"))
            elif command == "gsSPClearGeometryMode":
                geometry.difference_update(fields[0].split("|"))
            elif command not in ("gsDPPipeSync", "gsDPTileSync", "gsDPLoadSync",
                                 "gsSPEndDisplayList"):
                state[command] = fields

    for part in PARTS:
        visit(part, part)
    return draws, (state, sorted(geometry), tiles, texture)


class UnagiModelTests(unittest.TestCase):
    def test_native_draws_and_restored_state(self):
        result = native_draws(read_source(ROOT / "actors/unagi/model.inc.c"))
        self.assertEqual(len(result[0]), 156)
        # Audited original ordered triangles, full vertex payloads, material and final state.
        self.assertEqual(hashlib.sha256(json.dumps(result).encode()).hexdigest(),
                         "662dc0dc96955174b7bdf0897059b8d0a23b28420814524bf55d976e2af14ccb")

    def test_body_parts_are_consecutive_opaque_draws(self):
        geo = read_source(ROOT / "actors/unagi/geo.inc.c")
        parts = re.findall(r"GEO_ANIMATED_PART\((LAYER_\w+),[^\n]*, (unagi_seg5_dl_\w+)\)", geo)
        self.assertEqual(parts[:4], [("LAYER_OPAQUE", name) for name in PARTS[:4]])
        first, last = geo.index(PARTS[0]), geo.index(PARTS[3])
        self.assertNotIn("GEO_SWITCH_CASE", geo[first:last])
        self.assertNotIn("GEO_DISPLAY_LIST", geo[first:last])


if __name__ == "__main__":
    unittest.main()
