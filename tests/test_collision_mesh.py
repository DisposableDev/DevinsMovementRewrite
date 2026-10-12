"""Persistent mesh accuracy, bounded storage, and full rebuild/query comparisons."""
from pathlib import Path
import os
import re
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


def prepare_fixture(temporary):
    (temporary / "types.h").write_text("""
#include <stdint.h>
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef uint8_t u8;
typedef float f32;
typedef s16 Vec3s[3];
typedef f32 Vec3f[3];
typedef f32 Mat4[4][4];
typedef s16 TerrainData;
typedef s16 Collision;
""")
    (temporary / "config.h").write_text("#define ALL_SURFACES_HAVE_FORCE\n")
    assets = []
    for path in ("levels/jrb/areas/1/collision.inc.c",
                 "levels/physics_lab/collision.inc.c",
                 "levels/bob/areas/1/collision.inc.c",
                 "levels/jrb/wooden_ship/collision.inc.c"):
        source = (ROOT / path).read_text()
        for match in re.finditer(
                r"const Collision (\w+)\[\] = \{(.*?)COL_TRI_STOP\(\)", source, re.S):
            assets.append((match[1], "const Collision " + match[1]
                           + "[] = {" + match[2] + "COL_TRI_STOP() };\n"))
    if len(assets) != 12:
        raise ValueError("Shipped mesh inventory changed; review the benchmark selection")
    definitions = '#include "surface_terrains.h"\n' + ''.join(body for _, body in assets)
    definitions += "static const struct Asset assets[] = {\n" + ''.join(
        '{"' + name + '", ' + name + ', sizeof(' + name + ')/sizeof(Collision)},\n'
        for name, _ in assets) + "};\n"
    (temporary / "mesh_assets.h").write_text(definitions)


class CollisionMeshTests(unittest.TestCase):
    def test_geometry_and_cost(self):
        with tempfile.TemporaryDirectory(prefix="sm64-mesh-") as directory:
            temporary = Path(directory)
            prepare_fixture(temporary)
            compiler = os.environ.get("CC", "cc")
            sources = [str(ROOT / "src/engine/collision_mesh.c"),
                       str(ROOT / "tests/collision_mesh_test.c")]
            common = [compiler, "-std=gnu17", "-Wall", "-Wextra", "-Werror",
                      "-I" + directory, "-I" + str(ROOT / "src/engine"),
                      "-I" + str(ROOT / "include"), *sources, "-lm"]
            sanitized = temporary / "sanitized"
            subprocess.run([*common, "-O1", "-g", "-fsanitize=address,undefined",
                            "-fno-omit-frame-pointer", "-no-pie", "-o", str(sanitized)],
                           check=True)
            sanitizer_environment = dict(os.environ)
            sanitizer_environment["ASAN_OPTIONS"] = (
                sanitizer_environment.get("ASAN_OPTIONS", "") + ":detect_leaks=0")
            result = subprocess.run([str(sanitized), "benchmark"], capture_output=True, text=True,
                                    env=sanitizer_environment)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertIn("accuracy and lifecycle passed", result.stdout)
            optimized = temporary / "optimized"
            subprocess.run([*common, "-O2", "-ffinite-math-only", "-fno-signed-zeros",
                            "-o", str(optimized)], check=True)
            result = subprocess.run([str(optimized), "benchmark"], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            print(result.stdout, end="")


if __name__ == "__main__":
    unittest.main()
