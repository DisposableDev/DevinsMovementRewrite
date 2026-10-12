#!/usr/bin/env python3
"""Check the exported textured floor against its original tiled footprint.

UV sampling here uses affine world-space interpolation and point sampling;
it is not an RSP/RDP rasterizer or an emulator timing measurement.
"""
import math
from collections import Counter
import re
import unittest

from test_physics_lab_rendering import ARRAYS, MODEL, decode, lab


def bounds(face):
    return tuple(min(p[a] for p in face.points) for a in (0, 2)) + tuple(
        max(p[a] for p in face.points) for a in (0, 2))


def area(rect):
    return (rect[2] - rect[0]) * (rect[3] - rect[1])


def overlap(a, b):
    return max(0, min(a[2], b[2]) - max(a[0], b[0])) * max(
        0, min(a[3], b[3]) - max(a[1], b[1]))


class PhysicsLabChecker(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.mesh = lab.build_terrain()
        cls.original = [f for f in cls.mesh.faces if f.checker_color is not None]
        cls.patches = lab.ground_patches(cls.mesh)
        cls.shipped = decode("physics_lab_ground_dl")
        texture = re.search(r"physics_lab_checker_texture\[\] = \{(.*?)\};", MODEL, re.S)[1]
        cls.texels = list(map(int, re.findall(r"\d+", texture)))

    def test_exact_floor_coverage_and_surface_regions(self):
        """Every old rectangle is covered once, with no added floor in holes."""
        # Paint keeps collision-only copies among the rendered ground pieces.
        # Every original ground triangle must still survive with its surface.
        ground = lab.Mesh()
        lab.add_ground(ground)
        original_collision = Counter((tri,face.surface,face.force) for face,tri in ground.triangles())
        terrain_collision = Counter((tri,face.surface,face.force) for face,tri in self.mesh.triangles()
                                    if face.surface is not None)
        self.assertFalse(original_collision-terrain_collision)
        source_bounds = [bounds(f) for f in self.original]
        patch_bounds = [bounds(f) for f in self.patches]
        source_coverage = [0] * len(self.original)
        for i, (patch, rect) in enumerate(zip(self.patches, patch_bounds)):
            self.assertTrue(all(p[1] == 0 for p in patch.points))
            self.assertGreater(lab.normal(patch.points)[1], 0)
            # Ground no longer splits at 6,000-unit list boundaries. Its
            # actual corners must still fit the native signed vertex format.
            for coordinate in rect:
                self.assertGreaterEqual(coordinate, -32768)
                self.assertLessEqual(coordinate, 32767)
            self.assertTrue(all(overlap(rect, other) == 0 for other in patch_bounds[:i]))
            covered = 0
            for j, (old, old_rect) in enumerate(zip(self.original, source_bounds)):
                common = overlap(rect, old_rect)
                if common:
                    self.assertEqual((patch.color, patch.surface, patch.force),
                                     (old.checker_color, old.surface, old.force))
                    source_coverage[j] += common
                    covered += common
            self.assertEqual(covered, area(rect))
        self.assertEqual(source_coverage, [area(rect) for rect in source_bounds])
        # Collision still uses the original faces. The render conversion cannot
        # replace, reorder or mutate them as a side effect.
        self.assertEqual(self.mesh.faces, lab.build_terrain().faces)

    def test_shipped_patch_shape_attributes_and_vertex_budget(self):
        self.assertEqual(len(self.shipped), 2 * len(self.patches))
        # Full district captions add deliberate color boundaries that cannot
        # merge. Keep at least a 70% reduction against the authored tiles.
        self.assertLess(len(self.shipped), 2 * len(self.original) * .3)
        for i, face in enumerate(self.patches):
            pair = self.shipped[2 * i:2 * i + 2]
            self.assertEqual([tuple(v[:3] for v in tri) for tri in pair],
                             [face.points[:3], tuple(face.points[j] for j in (0, 2, 3))])
            for triangle in pair:
                for v in triangle:
                    self.assertEqual(v[6:], (*face.color, 255))
                    self.assertTrue(all(-32768 <= t <= 32767 for t in v[4:6]))
                    # Test the mapping independently of checker_uv().
                    self.assertLessEqual(abs(v[4] - v[0] * 16 * 32 / 1000), .5)
                    self.assertLessEqual(abs(v[5] - v[2] * 16 * 32 / 1000), .5)

    def test_world_grid_phase_and_original_colors(self):
        self.assertEqual(len(self.texels), 1024)
        for t in range(32):
            for s in range(32):
                self.assertEqual(self.texels[t * 32 + s],
                                 235 if (s // 16 + t // 16) % 2 == 0 else 255)
        samples = 0
        for i, face in enumerate(self.patches):
            x0, z0, x1, z1 = bounds(face)
            # Includes tile centers, both sides of grid lines, and negative
            # coordinates. Avoid exact edges, where fixed-point rounding may
            # select either adjacent texel (less than one world unit away).
            xs = {x0 + 4, x1 - 4, (x0 + x1) / 2}
            zs = {z0 + 4, z1 - 4, (z0 + z1) / 2}
            xs.update(v for k in range(math.floor(x0 / 1000), math.ceil(x1 / 1000) + 1)
                      for v in (k * 1000 - 4, k * 1000 + 4, k * 1000 + 500) if x0 < v < x1)
            zs.update(v for k in range(math.floor(z0 / 1000), math.ceil(z1 / 1000) + 1)
                      for v in (k * 1000 - 4, k * 1000 + 4, k * 1000 + 500) if z0 < v < z1)
            a, b, c = self.shipped[2 * i]
            d = self.shipped[2 * i + 1][2]
            for x in xs:
                for z in zs:
                    if min(x % 1000, 1000 - x % 1000, z % 1000, 1000 - z % 1000) < 2:
                        continue
                    # Interpolate the actual exported UV endpoints along each
                    # rectangle axis. Both triangles must have the same map.
                    u = a[4] + (d[4] - a[4]) * (x - x0) / (x1 - x0)
                    v = a[5] + (b[5] - a[5]) * (z - z0) / (z1 - z0)
                    self.assertEqual((a[4], b[4], c[4], d[4]), (a[4], a[4], d[4], d[4]))
                    self.assertEqual((a[5], b[5], c[5], d[5]), (a[5], b[5], b[5], a[5]))
                    # gsSPTexture's unsigned 0.16 scale is just under unity.
                    texel = self.texels[(math.floor(v / 32 * 65535 / 65536) % 32) * 32
                                        + math.floor(u / 32 * 65535 / 65536) % 32]
                    dark = (math.floor(x / 1000) + math.floor(z / 1000)) % 2 == 0
                    self.assertEqual(texel, 235 if dark else 255)
                    samples += 1
        # The old color values are independently retained by collision/source
        # tiles. The 8-bit intensity approximates their 0.92 tint within 1/255.
        for old in self.original:
            x0, z0, x1, z1 = bounds(old)
            dark = (math.floor((x0 + x1) / 2000) + math.floor((z0 + z1) / 2000)) % 2 == 0
            tint = 235 / 255 if dark else 1
            self.assertTrue(all(abs(round(c * tint) - original) <= 1
                                for c, original in zip(old.checker_color, old.color)))
        self.assertGreater(samples, 10000)
        print(f"Checker grid: {samples} samples preserve 1000-unit spacing and world phase.")

    def test_single_texture_load_and_material_restoration(self):
        state = []
        decode("physics_lab_terrain_dl", seen=state)
        loads = [(op, args) for op, args in state if op == "gsDPLoadTextureBlock"]
        self.assertEqual(loads, [("gsDPLoadTextureBlock",
            "physics_lab_checker_texture, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0, G_TX_WRAP, G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD")])
        self.assertEqual([args for op, args in state if op == "gsDPSetTextureFilter"],
                         ["G_TF_POINT", "G_TF_BILERP"])
        opening, closing = [], []
        decode("physics_lab_ground_begin", seen=opening)
        decode("physics_lab_ground_end", seen=closing)
        self.assertIn(("gsDPSetCombineMode", "G_CC_MODULATEI, G_CC_MODULATEI"), opening)
        self.assertIn(("gsSPTexture", "0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON"), opening)
        self.assertIn(("gsSPTexture", "0, 0, 0, G_TX_RENDERTILE, G_OFF"), closing)
        self.assertIn(("gsDPSetCombineMode", "G_CC_SHADE, G_CC_SHADE"), closing)
        self.assertIn(("gsSPSetGeometryMode", "G_LIGHTING | G_CULL_BACK"), closing)
        self.assertLess(ARRAYS["physics_lab_terrain_dl"].index("physics_lab_ground_dl"),
                        ARRAYS["physics_lab_terrain_dl"].index("physics_lab_terrain_begin"))


if __name__ == "__main__":
    unittest.main()
