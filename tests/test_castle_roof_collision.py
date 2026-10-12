#!/usr/bin/env python3
"""Castle roof geometry checks; no emulator movement or rendering is exercised."""
import math
from pathlib import Path
import re
import unittest

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]
AREA = ROOT / "levels/castle_grounds/areas/1"


def normal(points):
    a, b, c = points
    u = [b[i] - a[i] for i in range(3)]
    v = [c[i] - a[i] for i in range(3)]
    n = (u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2],
         u[0] * v[1] - u[1] * v[0])
    length = math.sqrt(sum(x * x for x in n))
    assert length > 0, "Degenerate collision triangle"
    return tuple(x / length for x in n)


def collision():
    text = read_source(AREA / "collision.inc.c")
    vertices = [tuple(map(int, v)) for v in re.findall(
        r"COL_VERTEX\((-?\d+),\s*(-?\d+),\s*(-?\d+)\)", text)]
    assert len(vertices) == int(re.search(r"COL_VERTEX_INIT\((\d+)\)", text)[1])
    triangles = []
    remaining = 0
    surface = None
    for command, body in re.findall(r"(COL_TRI_INIT|COL_TRI_SPECIAL|COL_TRI)\(([^)]+)\)", text):
        args = [arg.strip() for arg in body.split(",")]
        if command == "COL_TRI_INIT":
            assert remaining == 0, "Incorrect surface batch count"
            surface, remaining = args[0], int(args[1])
        else:
            assert remaining > 0
            points = tuple(vertices[int(index)] for index in args[:3])
            normal(points)
            triangles.append((surface, points))
            remaining -= 1
    assert remaining == 0
    return triangles


def rendered_triangles(part):
    text = read_source(AREA / str(part) / "model.inc.c")
    arrays = {name: [tuple(map(int, v)) for v in re.findall(
        r"\{\s*\{\s*\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)", body)]
        for name, body in re.findall(r"const Vtx (\w+)\[\] = \{(.*?)\n\};", text, re.S)}
    cache = {}
    triangles = []
    for command, body in re.findall(r"gsSP(Vertex|1Triangle|2Triangles)\(([^)]+)\)", text):
        args = [arg.strip() for arg in body.split(",")]
        if command == "Vertex":
            name, count, start = args
            for index, vertex in enumerate(arrays[name][:int(count)]):
                cache[int(start) + index] = vertex
        else:
            indices = [int(arg, 0) for arg in args]
            for start in ([0] if command == "1Triangle" else [0, 4]):
                triangles.append(tuple(cache[index] for index in indices[start:start + 3]))
    if part == 3:
        facade = read_source(AREA / "secret_room/model.inc.c").split(
            "static const Vtx castle_secret_facade_dl_vtx[] = {", 1)[1].split("};", 1)[0]
        points = [tuple(map(int, v)) for v in re.findall(
            r"\{\s*\{\s*\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)", facade)]
        for i in range(0, len(points), 4):
            a, b, c, d = points[i:i + 4]
            triangles.extend(((a, b, c), (a, c, d)))
    return triangles


class CastleRoofCollision(unittest.TestCase):
    def test_outer_roof_support_matches_every_visible_face(self):
        def outer_roof(t):
            return (min(v[1] for v in t) >= 2339 and max(v[1] for v in t) <= 3174
                    and max(abs(v[0]) for v in t) < 4500 and normal(t)[1] > 0.01)
        visual = {frozenset(t) for t in rendered_triangles(1) if outer_roof(t)}
        physical = {frozenset(t) for _, t in collision() if outer_roof(t)}
        self.assertTrue(visual)
        self.assertEqual(physical, visual)

    def test_northwest_cliff_support_matches_visible_slopes(self):
        def cliff(t):
            return (max(v[0] for v in t) <= -5000 and max(v[2] for v in t) <= -4000
                    and normal(t)[1] > 0.01)
        visual = {frozenset(t) for t in rendered_triangles(2) if cliff(t)}
        physical = [(surface, t) for surface, t in collision() if cliff(t)]
        self.assertTrue(visual)
        self.assertEqual({frozenset(t) for _, t in physical}, visual)
        self.assertTrue(all(surface in ("SURFACE_VERY_SLIPPERY", "SURFACE_NOISE_SLIPPERY")
                            for surface, _ in physical))

    def test_waterfall_bank_matches_visible_slopes(self):
        def bank(t):
            return (min(v[0] for v in t) < -5000 and min(v[2] for v in t) < -4000
                    and max(v[1] for v in t) < 2000 and 0.01 < normal(t)[1] < 0.99)
        visual = {frozenset(t) for t in rendered_triangles(2) if bank(t)}
        physical = [(surface, t) for surface, t in collision()
                    if bank(t) and surface in ("SURFACE_VERY_SLIPPERY", "SURFACE_NOISE_SLIPPERY")]
        self.assertTrue(visual)
        self.assertEqual({frozenset(t) for _, t in physical}, visual)

    def test_all_four_flag_roofs_match_visible_faces(self):
        triangles = collision()
        rendered = rendered_triangles(1)
        for apex in ((-3212, 3349, -3009), (3213, 3349, -3009),
                     (-3834, 3349, -6646), (3835, 3349, -6646)):
            visual = {frozenset(t) for t in rendered if apex in t}
            physical = {frozenset(t) for _, t in triangles if apex in t}
            self.assertGreaterEqual(len(visual), 5)
            self.assertEqual(physical, visual)

    def test_highest_roof_matches_visible_mesh_and_has_full_grip(self):
        visual = {frozenset((x, y + 2867, z - 3924) for x, y, z in t)
                  for t in rendered_triangles(3)
                  if min(v[1] for v in t) >= 3430 and normal(t)[1] > 0.1}
        physical = [(surface, t) for surface, t in collision()
                    if min(v[1] for v in t) >= 6297 and normal(t)[1] > 0.1]
        self.assertEqual(len(physical), 21)
        self.assertEqual({frozenset(t) for _, t in physical}, visual)
        self.assertEqual({surface for surface, _ in physical}, {"SURFACE_GRIPPY"})
        for surface, t in collision():
            if surface in ("SURFACE_NOT_SLIPPERY", "SURFACE_GRIPPY"):
                self.assertEqual(surface == "SURFACE_GRIPPY", min(v[1] for v in t) > 2000)

    def test_central_roof_collision_matches_shadow_receiving_faces(self):
        def central_roof(points):
            return (min(p[1] for p in points) >= 2460
                    and max(p[1] for p in points) <= 4069
                    and max(abs(p[0]) for p in points) <= 1846
                    and normal(points)[1] > 0.1)

        visible = {frozenset(t) for t in rendered_triangles(1) if central_roof(t)}
        physical = {frozenset(t) for surface, t in collision()
                    if surface == "SURFACE_GRIPPY" and central_roof(t)}
        self.assertTrue(visible)
        self.assertEqual(physical, visible)

    def test_both_white_tower_tiers_are_complete_vertical_walls(self):
        triangles = collision()
        for bottom, top in ((2867, 4618), (5003, 6298)):
            walls = [(surface, t) for surface, t in triangles
                     if max(v[1] for v in t) == top and min(v[1] for v in t) >= bottom
                     and min(v[1] for v in t) < top and abs(normal(t)[1]) < 0.1]
            self.assertEqual(len(walls), 14 if bottom == 2867 else 18)
            for surface, points in walls:
                self.assertEqual(surface, "SURFACE_DEFAULT")
                self.assertAlmostEqual(normal(points)[1], 0.0)
            # Each tower ring edge must be covered by two triangles, outward-facing.
            edges = {}
            for _, points in walls:
                edge = frozenset((x, z) for x, _, z in points)
                self.assertEqual(len(edge), 2)
                edges[edge] = edges.get(edge, 0) + 1
            if bottom == 2867:
                self.assertEqual(len(edges), 7)
                self.assertEqual(set(edges.values()), {2})
            center_z = -3925
            for _, points in walls:
                n = normal(points)
                center = [sum(v[i] for v in points) / 3 for i in range(3)]
                self.assertGreater(n[0] * center[0] + n[2] * (center[2] - center_z), 0)

    def test_lower_tower_walls_match_rendered_footprint(self):
        visual = {frozenset((x, y + 2867, z - 3925) for x, y, z in t)
                  for t in rendered_triangles(3) if abs(normal(t)[1]) < 0.01
                  and min(v[1] for v in t) == 0 and max(v[1] for v in t) == 1751}
        physical = {frozenset(t) for surface, t in collision()
                    if surface == "SURFACE_DEFAULT" and max(v[1] for v in t) == 4618
                    and min(v[1] for v in t) == 2867}
        self.assertEqual(physical, visual)
        self.assertEqual(len(physical), 14)

    def test_roof_support_reaches_both_tower_wall_rings(self):
        triangles = collision()
        floors = [t for _, t in triangles if normal(t)[1] > 0.1]
        checks = 0
        for bottom, top in ((2867, 4618), (5003, 6298)):
            edges = set()
            for surface, t in triangles:
                if (surface == "SURFACE_DEFAULT" and max(v[1] for v in t) == top
                        and bottom <= min(v[1] for v in t) < top
                        and abs(normal(t)[1]) < 0.01):
                    edge = frozenset((x, z) for x, _, z in t)
                    if edge in edges:
                        continue
                    edges.add(edge)
                    n = normal(t)
                    a, b = sorted(edge)
                    for radius in (24, 50):
                        for fraction in (0.1, 0.25, 0.5, 0.75, 0.9):
                            x = a[0] + (b[0] - a[0]) * fraction + n[0] * radius
                            z = a[1] + (b[1] - a[1]) * fraction + n[2] * radius
                            supported = False
                            for floor in floors:
                                # Independent finite triangle/plane check at the
                                # ordinary lower and upper wall push distances.
                                sides = [(q[0] - p[0]) * (z - p[2])
                                         - (q[2] - p[2]) * (x - p[0])
                                         for p, q in zip(floor, floor[1:] + floor[:1])]
                                if not (all(d <= 0.01 for d in sides)
                                        or all(d >= -0.01 for d in sides)):
                                    continue
                                fn = normal(floor)
                                p = floor[0]
                                height = p[1] - (fn[0] * (x - p[0])
                                                  + fn[2] * (z - p[2])) / fn[1]
                                if bottom - 100 <= height < top - 200:
                                    supported = True
                                    break
                            self.assertTrue(supported, (bottom, edge, radius, fraction))
                            checks += 1
        self.assertGreaterEqual(checks, 140)

    def test_upper_tower_wall_rim_stays_inside_visible_stone(self):
        visual = [tuple((x, y + 2867, z - 3924) for x, y, z in t)
                  for t in rendered_triangles(3) if min(v[1] for v in t) >= 2136
                  and 3430 <= max(v[1] for v in t) <= 3431
                  and min(v[1] for v in t) < 3133 and abs(normal(t)[1]) < 0.1]
        physical = [t for surface, t in collision()
                    if surface == "SURFACE_DEFAULT" and min(v[1] for v in t) >= 5003
                    and max(v[1] for v in t) == 6298
                    and min(v[1] for v in t) < 6000]
        self.assertEqual(len(visual), 16)
        self.assertEqual(len(physical), 16)
        visible_rim = {(x, z) for t in visual for x, y, z in t if y >= 6297}
        collision_rim = {(x, z) for t in physical for x, y, z in t if y == 6298}
        self.assertEqual(collision_rim, visible_rim)
        # Every replacement is a true wall, with its lower corners directly below the rim.
        for t in physical:
            self.assertAlmostEqual(normal(t)[1], 0.0)
            self.assertTrue(all((x, z) in visible_rim for x, y, z in t))

    def test_broken_window_aperture_and_inward_room_collision(self):
        front = [t for _, t in collision() if all(z == -3421 for x, y, z in t)
                 and min(y for x, y, z in t) >= 5021]
        self.assertEqual(len(front), 8)
        for t in front:
            # No static wall crosses the open center at Mario's body height.
            self.assertTrue(max(x for x, y, z in t) <= -165
                            or min(x for x, y, z in t) >= 165
                            or max(y for x, y, z in t) <= 5780
                            or min(y for x, y, z in t) >= 6120)
        text = read_source(AREA / "secret_room/collision.inc.c").split(
            "const Collision castle_secret_room_collision[] = {", 1)[1].split("};", 1)[0]
        vertices = [tuple(map(int, v)) for v in re.findall(
            r"COL_VERTEX\((-?\d+),\s*(-?\d+),\s*(-?\d+)\)", text)]
        for body in re.findall(r"COL_TRI\(([^)]+)\)", text):
            t = tuple(vertices[int(i)] for i in body.split(","))
            n = normal(t)
            center = [sum(v[i] for v in t) / 3 for i in range(3)]
            self.assertGreater(sum(n[i] * ((0, 200, -350)[i] - center[i])
                                   for i in range(3)), 0)

    def test_empty_portrait_retains_a_frame_with_an_open_center(self):
        text = read_source(AREA / "secret_room/model.inc.c")
        vertices = text.split("static const Vtx castle_secret_portrait_frame_dl_vtx[] = {", 1)[1].split("};", 1)[0]
        points = [tuple(map(int, v)) for v in re.findall(
            r"\{\s*\{\s*\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)", vertices)]
        dl = text.split("const Gfx castle_secret_portrait_frame_dl[] = {", 1)[1].split("};", 1)[0]
        area = 0
        for body in re.findall(r"gsSP2Triangles\(([^)]+)\)", dl):
            indices = [int(v.strip()) for v in body.split(",")]
            for start in (0, 4):
                a, b, c = [points[i] for i in indices[start:start + 3]]
                self.assertGreater(normal((a, b, c))[2], 0)
                area += ((b[0] - a[0]) * (c[1] - a[1])
                         - (b[1] - a[1]) * (c[0] - a[0])) / 2
        self.assertEqual(area, 260 * 260 - 228 * 228)
        empty = text.split("const Gfx castle_secret_empty_frame_dl[] = {", 1)[1].split("};", 1)[0]
        self.assertIn("gsSPDisplayList(castle_secret_portrait_frame_dl)", empty)
        self.assertNotIn("gsDPSetTextureImage", empty)

    def test_flags_share_pole_interaction_and_keep_waving(self):
        behavior = read_source(ROOT / "data/behavior_data.c").split(
            "const BehaviorScript bhvCastleFlagWaving[] = {", 1)[1].split("};", 1)[0]
        self.assertIn("BEGIN(OBJ_LIST_POLELIKE)", behavior)
        self.assertIn("SET_INT(oInteractType, INTERACT_POLE)", behavior)
        self.assertIn("SET_INT(oIntangibleTimer, 0)", behavior)
        self.assertIn("ANIMATE(CASTLE_FLAG_ANIM_WAVE)", behavior)
        self.assertIn("CALL_NATIVE(bhv_castle_flag_loop)", behavior)
        height = int(re.search(r"SET_HITBOX\([^,]+,\s*/\*Height\*/\s*(\d+)\)", behavior)[1])
        scale = int(re.search(r"GEO_SCALE\(0x00,\s*(\d+)\)",
                              read_source(AREA / "11/geo.inc.c"))[1]) / 65536
        # The root flag panel spans +/-480 local units before the fixed geo scale.
        self.assertEqual(height, 960 * scale)
        placements = re.findall(r"OBJECT\([^\n]+bhvCastleFlagWaving\)",
                                read_source(ROOT / "levels/castle_grounds/script.c"))
        self.assertEqual(len(placements), 5)


if __name__ == "__main__":
    unittest.main()
