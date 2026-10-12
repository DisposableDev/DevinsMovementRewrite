#!/usr/bin/env python3
"""Protect Yoshi's original torso faces while changing their materials."""
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


class YoshiModelTests(unittest.TestCase):
    def test_torso_faces_and_winding(self):
        source = read_source(ROOT / "actors/yoshi/model.inc.c")
        arrays = {name: body for name, body in re.findall(
            r"\b(?:Vtx|Gfx)\s+(\w+)\[\]\s*=\s*\{(.*?)\n\};", source, re.S)}
        vertices = {name: [tuple(map(int, row)) for row in re.findall(
            r"\{\{\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\}", body)]
            for name, body in arrays.items() if name.startswith("yoshi_seg5_vertex_")}
        cache = {}
        faces = []
        for command, arguments in re.findall(
                r"(gsSPVertex|gsSP1Triangle|gsSP2Triangles)\((.*?)\)",
                arrays["yoshi_seg5_dl_05022408"], re.S):
            fields = [field.strip() for field in arguments.split(",")]
            if command == "gsSPVertex":
                batch, count, start = fields[0], int(fields[1], 0), int(fields[2], 0)
                self.assertLessEqual(count, len(vertices[batch]))
                self.assertLessEqual(start + count, 32)
                for index, position in enumerate(vertices[batch][:count], start):
                    cache[index] = position
            else:
                for offset in range(0, len(fields), 4):
                    indices = [int(field, 0) for field in fields[offset:offset + 3]]
                    self.assertTrue(all(index in cache for index in indices),
                                    f"Unloaded vertex in {command}({arguments})")
                    triangle = tuple(cache[index] for index in indices)
                    # Ignore cyclic index order, but retain winding and duplicate faces.
                    faces.append(min(triangle, triangle[1:] + triangle[:1],
                                     triangle[2:] + triangle[:2]))

        self.assertEqual(len(faces), 50)
        digest = hashlib.sha256(json.dumps(sorted(faces), separators=(",", ":")).encode())
        # Original unmodified torso geometry, independently audited from the base model.
        self.assertEqual(digest.hexdigest(),
                         "7d009fbbba3a6071f39b655cceda0fb3fdd383d86a2d0aeb90b3f5f6ea93601a")


if __name__ == "__main__":
    unittest.main()
