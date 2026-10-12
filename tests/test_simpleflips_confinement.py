#!/usr/bin/env python3
"""Production room bounds and exit routing, with host loading/audio boundaries."""
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


def prepare_confinement_fixture(folder):
    room = Source(ROOT / "src/game/jrb_simpleflips.c")
    level = Source(ROOT / "src/game/level_update.c")
    header = Source(ROOT / "src/game/level_update.h")
    mario = Source(ROOT / "include/sm64.h")
    definitions = "".join(header.declaration("enum", name) for name in (
        "WarpOperation", "SpecialWarpDestinations", "WarpFlags", "PlayModes",
        "WarpTypes", "WarpNodes"))
    definitions += header.declaration("struct", "WarpDest")
    definitions += header.macros("WARP_FLAG_YOSHI_COIN_CAKE")
    definitions += mario.declaration("enum", "MarioFlags")
    definitions += mario.macros("ACT_IDLE|ACT_DEBUG_FREE_MOVE|ACT_HARD_BACKWARD_AIR_KB")
    definitions += Source(ROOT / "src/game/jrb_simpleflips.h").macros("JRB_SIMPLEFLIPS_AREA")
    production = room.function("jrb_simpleflips_room_active")
    production += room.function("jrb_simpleflips_is_game_over")
    production += room.function("jrb_simpleflips_exit_locked")
    production += room.function("jrb_simpleflips_confine_mario")
    production += "".join(level.function(name) for name in (
        "warp_special", "fade_into_special_warp", "initiate_warp",
        "level_trigger_warp", "initiate_delayed_warp"))
    (folder / "confinement_definitions.inc").write_text(definitions)
    (folder / "confinement_actual.inc").write_text(production)


class ConfinementTests(unittest.TestCase):
    def test_bounds_and_exit_routes(self):
        with tempfile.TemporaryDirectory(prefix="simpleflips-confinement-") as directory:
            folder = Path(directory)
            prepare_confinement_fixture(folder)
            subprocess.run(["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=undefined", "-fno-sanitize-recover=all", "-I", str(folder), "-I", str(ROOT),
                            "-I", str(ROOT / "include"),
                            str(ROOT / "tests/simpleflips_confinement_test.c"),
                            "-o", str(folder / "test")], check=True, timeout=30)
            subprocess.run([str(folder / "test")], check=True, timeout=10)

    def test_shipped_box_is_closed(self):
        mesh = (ROOT / "levels/jrb/simpleflips/room.inc.c").read_text().split(
            "static const Vtx")[0]
        vertices = [tuple(map(int, point)) for point in re.findall(
            r"COL_VERTEX\((-?\d+), (-?\d+), (-?\d+)\)", mesh)]
        faces = [tuple(map(int, face)) for face in re.findall(
            r"COL_TRI\((\d+), (\d+), (\d+)\)", mesh)]
        self.assertEqual(len(vertices), 8)
        self.assertEqual(len(faces), 12)
        edges = {}
        for face in faces:
            a, b, c = [vertices[i] for i in face]
            u = [b[i] - a[i] for i in range(3)]
            v = [c[i] - a[i] for i in range(3)]
            normal = (u[1] * v[2] - u[2] * v[1],
                      u[2] * v[0] - u[0] * v[2],
                      u[0] * v[1] - u[1] * v[0])
            self.assertGreater(sum(normal[i] * ((0, 500, 0)[i] - a[i])
                                   for i in range(3)), 0)
            for edge in zip(face, face[1:] + face[:1]):
                key = tuple(sorted(edge))
                edges.setdefault(key, []).append(edge)
        for pair in edges.values():
            self.assertEqual(len(pair), 2)
            self.assertEqual(pair[0], pair[1][::-1])


if __name__ == "__main__":
    unittest.main()
