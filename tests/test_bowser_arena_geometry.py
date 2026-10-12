"""Safety margins for Bowser Fight 3's collision optimizations."""
from pathlib import Path
import math
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]


def mesh(path):
    text = path.read_text()
    vertices = [tuple(map(int, values)) for values in
                re.findall(r"COL_VERTEX\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", text)]
    triangles = [tuple(map(int, values)) for values in
                 re.findall(r"COL_TRI\(\s*(\d+),\s*(\d+),\s*(\d+)\)", text)]
    return vertices, triangles


def edge_distance(a, b):
    dx, dz = b[0] - a[0], b[2] - a[2]
    length = dx * dx + dz * dz
    t = max(0, min(1, -(a[0] * dx + a[2] * dz) / length)) if length else 0
    return math.hypot(a[0] + t * dx, a[2] + t * dz)


class BowserArenaGeometry(unittest.TestCase):
    def test_fixed_center_has_no_moving_geometry(self):
        paths = sorted((ROOT / "levels/bowser_3").glob("**/collision.inc.c"))
        moving = [path for path in paths if "/areas/" not in str(path)]
        self.assertEqual(len(moving), 10)
        for path in moving:
            vertices, triangles = mesh(path)
            for triangle in triangles:
                points = [vertices[index] for index in triangle]
                # Every projected edge is outside the fixed-center disk.
                for a, b in zip(points, points[1:] + points[:1]):
                    self.assertGreater(edge_distance(a, b), 1000, str(path))
                crosses = [a[0] * b[2] - b[0] * a[2]
                           for a, b in zip(points, points[1:] + points[:1])]
                if any(crosses):
                    self.assertFalse(all(c >= 0 for c in crosses) or
                                     all(c <= 0 for c in crosses), str(path))
            # The collision radius covers every vertex, plus a fast Mario step.
            self.assertLess(max(math.sqrt(sum(c * c for c in v)) for v in vertices)
                            + 128 + 50, 4000)

    def test_fixed_center_is_inside_static_top(self):
        vertices, triangles = mesh(ROOT / "levels/bowser_3/areas/1/collision.inc.c")
        edges = {}
        for triangle in triangles:
            if all(vertices[index][1] == 307 for index in triangle):
                for a, b in zip(triangle, triangle[1:] + triangle[:1]):
                    key = tuple(sorted((a, b)))
                    edges[key] = edges.get(key, 0) + 1
        boundary = [edge for edge, count in edges.items() if count == 1]
        self.assertEqual(len(boundary), 10)
        for a, b in boundary:
            self.assertGreater(edge_distance(vertices[a], vertices[b]), 1000)


if __name__ == "__main__":
    unittest.main()
