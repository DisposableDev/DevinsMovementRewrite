"""Compare persistent publication with the actual native loader and floor resolver."""
from pathlib import Path
import os
import re
import subprocess
import tempfile
import unittest

from regression_support import Source
from test_collision_mesh import prepare_fixture as prepare_assets

ROOT = Path(__file__).resolve().parents[1]


def prepare_fixture(temporary):
    prepare_assets(temporary)
    types = Source(ROOT / "include/types.h")
    (temporary / "types.h").write_text("""
#pragma once
#include <stdint.h>
typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef s16 TerrainData;
typedef s16 Collision;
typedef s16 MacroObject;
typedef s16 SurfaceType;
typedef s8 RoomData;
typedef s16 Vec3t[3];
typedef s16 Vec3s[3];
typedef s32 Vec3i[3];
typedef f32 Vec3f[3];
typedef f32 Mat4[4][4];
struct Object;
""" + types.declaration("struct", "Normal") + types.declaration("struct", "Surface"))
    (temporary / "config.h").write_text("""
#pragma once
#define ALL_SURFACES_HAVE_FORCE
#ifndef PERSISTENT_DYNAMIC_SURFACES
#define PERSISTENT_DYNAMIC_SURFACES 1
#endif
#define DORRIE_STATIONARY_SURFACES 1
#define JRB_STATIONARY_SURFACES 1
#define BOWSER_STATIONARY_SURFACES 1
// Host pointers widen Surface and SurfaceNode; allow equivalent reference coverage.
#define PERSISTENT_SURFACE_SLOT_BYTES 0x5800
#define STATIC_SURFACE_INDEX 0
#define DYNAMIC_SURFACE_INDEX 0
#define COLLISION_PROFILING 0
#define TRUE 1
#define FALSE 0
#define LEVEL_BOUNDARY_MAX 16384
#define CELL_SIZE 1024
#define NUM_CELLS 32
#define ALWAYS_INLINE inline __attribute__((always_inline))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define sqr(a) ((a)*(a))
#define PUPPYPRINT_ADD_COUNTER(counter) ((void)0)
""")
    math = Source(ROOT / "src/engine/math_util.h")
    math_functions = Source(ROOT / "src/engine/math_util.c")
    (temporary / "math_util.h").write_text(
        '#pragma once\n#include "types.h"\n#include "config.h"\n'
        + math.macros("NEAR_ZERO|find_vector_perpendicular_to_plane|vec3_copy|vec3s_copy|"
                      "vec3s_to_vec3f|vec3_sum|vec3_add|vec3_scale_dest|vec3_scale|vec2_dot|"
                      "vec3_dot|linear_mtxf_mul_vec3|linear_mtxf_mul_vec3_and_translate")
        + "\nvoid min_max_3s(s16,s16,s16,s16*,s16*);\n"
        + "void min_max_3i(s32,s32,s32,s32*,s32*);\n"
        + "void mtxf_scale_vec3f(Mat4,Mat4,Vec3f);\n")
    loading = Source(ROOT / "src/engine/surface_load.c")
    load_header = Source(ROOT / "src/engine/surface_load.h")
    header = load_header.text
    header = re.sub(r'^#include[^\n]*\n', '', header, flags=re.M)
    (temporary / "surface_load.h").write_text(
        '#include "types.h"\n#include "config.h"\n' + header)
    query = (ROOT / "src/engine/surface_query.h").read_text()
    query = re.sub(r'^#include[^\n]*\n', '', query, flags=re.M)
    (temporary / "surface_query.h").write_text(query)
    collision = Source(ROOT / "src/engine/surface_collision.c")
    collision_header = Source(ROOT / "src/engine/surface_collision.h")
    object_header = Source(ROOT / "src/game/object_list_processor.h")
    functions = (math_functions.macros("min_max_3_func")
                 + math_functions.function("min_max_3i")
                 + math_functions.function("min_max_3s")
                 + math_functions.function("mtxf_scale_vec3f"))
    functions += loading.declaration("struct", "CellCoords")
    functions += loading.declaration("struct", "DynamicWallTail")
    functions += """
static struct DynamicWallTail sDynamicWallTails[16];
static struct CellCoords sCellsUsed[NUM_CELLS * NUM_SPATIAL_PARTITIONS];
static u16 sNumCellsUsed;
static u8 sClearAllCells;
"""
    functions += '\n'.join(loading.function(name) for name in (
        "alloc_surface_node", "alloc_surface", "add_surface_to_cell",
        "record_persistent_surface_counts", "publish_dynamic_surface_cell", "lower_cell_index",
        "upper_cell_index", "add_surface", "read_surface_data", "surf_has_no_cam_collision",
        "transform_dorrie_collision_vertices", "transform_object_vertices",
        "load_object_surfaces", "load_bowser_platform_top", "clear_dynamic_surfaces"))
    functions += collision_header.macros("FIND_FLOOR_BUFFER|get_surface_height_at_location")
    functions += object_header.declaration("enum", "CollisionFlags")
    functions += collision.function("check_within_floor_triangle_bounds")
    functions += collision.function("floor_height_cutoff")
    functions += collision.function("find_floor_from_list")
    (temporary / "legacy_loader.inc").write_text(functions)
    # Compile the production body beside the host headers; no algorithm is rewritten.
    (temporary / "persistent_surfaces.c").write_text(
        (ROOT / "src/engine/persistent_surfaces.c").read_text())


class PersistentSurfacesTests(unittest.TestCase):
    def test_geometry_lifetime_and_full_pipeline(self):
        with tempfile.TemporaryDirectory(prefix="sm64-persistent-") as directory:
            temporary = Path(directory)
            prepare_fixture(temporary)
            common = [os.environ.get("CC", "cc"), "-std=gnu17", "-Wall", "-Wextra", "-Werror",
                      "-fwrapv", "-I" + directory, "-I" + str(ROOT / "src/engine"),
                      "-I" + str(ROOT / "include"), str(temporary / "persistent_surfaces.c"),
                      str(ROOT / "tests/persistent_surfaces_test.c"), "-lm"]
            sanitized = temporary / "sanitized"
            subprocess.run([*common, "-O1", "-g", "-fsanitize=address,undefined",
                            "-fno-omit-frame-pointer", "-no-pie", "-o", str(sanitized)], check=True)
            environment = dict(os.environ)
            environment["ASAN_OPTIONS"] = environment.get("ASAN_OPTIONS", "") + ":detect_leaks=0"
            result = subprocess.run([str(sanitized)], env=environment, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            print(result.stdout, end="")
            standalone = temporary / "dorrie-only"
            subprocess.run([*common, "-DPERSISTENT_DYNAMIC_SURFACES=0", "-O1", "-g",
                            "-fsanitize=address,undefined", "-fno-omit-frame-pointer", "-no-pie",
                            "-o", str(standalone)], check=True)
            result = subprocess.run([str(standalone)], env=environment, capture_output=True,
                                    text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            print("Dorrie-only configuration:\n" + result.stdout, end="")
            size_optimized = temporary / "size-optimized"
            subprocess.run([*common, "-Os", "-fno-inline-functions", "-finline-limit=1",
                            "-ffinite-math-only", "-fno-signed-zeros", "-fno-associative-math",
                            "-o", str(size_optimized)], check=True)
            result = subprocess.run([str(size_optimized), "benchmark"], capture_output=True,
                                    text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            print("Host with ROM-like size optimization:\n" + result.stdout, end="")
            optimized = temporary / "optimized"
            subprocess.run([*common, "-O2", "-ffinite-math-only", "-fno-signed-zeros",
                            "-fno-associative-math", "-o", str(optimized)], check=True)
            result = subprocess.run([str(optimized), "benchmark"], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            print(result.stdout, end="")


if __name__ == "__main__":
    unittest.main()
