"""Host allocation, topology and vertex-attribute checks; no rasterization or timing."""

from pathlib import Path
import os
import re
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class VertexGroupTests(unittest.TestCase):
    def test_snow_batches_and_painting_maps(self):
        snow = Source(ROOT / "src/game/envfx_snow.c")
        painting = Source(ROOT / "src/game/paintings.c")
        maps = (ROOT / "levels/castle_inside/painting.inc.c").read_text()
        maps = "\n".join(re.findall(
            r"static const PaintingData inside_castle_seg7_painting_texture_map_\w+\[\] = \{.*?\};",
            maps, re.S))
        declarations = painting.declaration("struct", "PaintingVertexGroup")
        declarations += painting.declaration("struct", "PaintingVertexGroups")
        groups = (ROOT / "src/game/painting_vertex_groups.inc.c").read_text()
        production = "\n".join(snow.function(name) for name in (
            "append_snowflake_vertex_buffer", "envfx_update_snow"))
        production += "\n".join(painting.function(name) for name in (
            "painting_vertex_groups", "render_painting_vertex_groups", "render_painting"))
        fixture = r'''
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
typedef int16_t s16;
typedef int32_t s32;
typedef uint8_t u8;
typedef int16_t PaintingData;
typedef s16 Vec3s[3];
typedef struct { struct { s16 ob[3]; uint16_t flag; s16 tc[2]; u8 cn[4]; } v; } Vtx;
typedef struct { uint32_t command, argument; } Gfx;
#define PAINTING_CACHES 0
#define F3DEX_GBI_2 1
#define ENVFX_SNOW_NORMAL 1
#define ENVFX_SNOW_WATER 2
#define ENVFX_SNOW_BLIZZARD 3
#define LEVEL_CASTLE 6
#define G_IM_FMT_RGBA 0
#define ARRAY_COUNT(a) ((s32)(sizeof(a) / sizeof((a)[0])))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
static union { uint64_t align; u8 bytes[65536]; } arena;
static size_t used, starts[4], sizes[4];
static int allocations, failAllocation, unbatched, packetAllocation;
static void reset(void) { used = 0; allocations = 0; failAllocation = 0; packetAllocation = 0; }
static void *alloc_display_list(size_t size) {
    allocations++;
    if (allocations == failAllocation) return NULL;
    assert(allocations <= 4 && used + size <= sizeof(arena.bytes));
    size_t start = used;
    starts[allocations - 1] = start;
    sizes[allocations - 1] = size;
    used = (used + size + 15) & ~(size_t)15;
    memset(arena.bytes + start, 0xa5, size);
    return arena.bytes + start;
}
static void emit(Gfx *packet, uint32_t kind, uint32_t argument) {
    size_t offset = (u8 *)packet - arena.bytes;
    assert(allocations > packetAllocation);
    assert(offset >= starts[packetAllocation]
           && offset + sizeof(*packet) <= starts[packetAllocation] + sizes[packetAllocation]);
    *packet = (Gfx){kind, argument};
}
#define VIRTUAL_TO_PHYSICAL(v) ((uint32_t)((u8 *)(v) - arena.bytes))
#define gSPVertex(p, v, n, first) emit((p), 0x100 + (n), (v))
#define TRI(a, b, c) ((uint32_t)(a) | ((uint32_t)(b) << 5) | ((uint32_t)(c) << 10))
#define gSP1Triangle(p, a, b, c, flag) emit((p), 2, TRI(a, b, c))
#define gSP2Triangles(p, a, b, c, f, d, e, g, h) emit((p), 3, TRI(a, b, c) | (TRI(d, e, g) << 15))
#define gSPDisplayList(p, list) { emit((p), 4, 0); }
#define gSPEndDisplayList(p) { emit((p), 5, 0); }
#define gLoadBlockTexture(p, width, height, format, image) do { \
    for (int load = 0; load < 5; load++) emit((p), 6, 0); \
    (void)(width); (void)(height); (void)(format); (void)(image); \
} while (0)
struct SnowFlakeVertex { s16 x, y, z; };
struct EnvFxParticle { s32 xPos, yPos, zPos; } particles[140], *gEnvFxBuffer = particles;
static s16 gSnowParticleCount;
static Vtx gSnowTempVtx[3];
static struct SnowFlakeVertex gSnowFlakeVertex1 = {-5, 5, 0};
static struct SnowFlakeVertex gSnowFlakeVertex2 = {-5, -5, 0};
static struct SnowFlakeVertex gSnowFlakeVertex3 = {5, 5, 0};
static void envfx_update_snowflake_count(s32 mode, Vec3s position) { (void)mode; (void)position; }
static void orbit_from_positions(Vec3s a, Vec3s b, s16 *r, s16 *p, s16 *y) {
    (void)a; (void)b; *r = 600; *p = 0; *y = 0;
}
static void pos_from_orbit(Vec3s a, Vec3s b, s16 r, s16 p, s16 y) {
    (void)a; (void)r; (void)p; (void)y; memset(b, 0, sizeof(Vec3s));
}
static void rotate_triangle_vertices(Vec3s a, Vec3s b, Vec3s c, s16 p, s16 y) {
    (void)a; (void)b; (void)c; (void)p; (void)y;
}
static void envfx_update_snow_normal(s32 x, s32 y, s32 z) { (void)x; (void)y; (void)z; }
static void envfx_update_snow_water(s32 x, s32 y, s32 z) { (void)x; (void)y; (void)z; }
static void envfx_update_snow_blizzard(s32 x, s32 y, s32 z) { (void)x; (void)y; (void)z; }
struct PaintingMeshVertex { s16 pos[3]; u8 norm[3]; } mesh[256], *gPaintingMesh = mesh;
struct Painting { const s16 *const *textureMaps; } bob_painting;
static s16 gCurrLevelNum;
static void *segmented_to_virtual(const void *pointer) { return (void *)pointer; }
static void make_vertex(Vtx *verts, int i, s16 x, s16 y, s16 z, s16 u, s16 v,
                        u8 r, u8 g, u8 b, u8 a) {
    verts[i] = (Vtx){{{x, y, z}, 0, {u, v}, {r, g, b, a}}};
}
static Gfx fallback;
static Gfx *render_painting_unbatched(u8 *img, s16 w, s16 h, s16 *map,
                                     s16 verts, s16 tris, u8 alpha) {
    (void)img; (void)w; (void)h; (void)map; (void)verts; (void)tris; (void)alpha;
    unbatched++;
    return &fallback;
}
/* DECLARATIONS */
/* MAPS */
/* GROUPS */
/* PRODUCTION */
static void verify_triangles(Gfx *list, const s16 *map, int count) {
    Vtx *cache = NULL;
    int cacheCount = 0, triangle = 0;
    for (int command = 0; list[command].command != 5; command++) {
        assert((u8 *)&list[command + 1] <= arena.bytes + used);
        Gfx packet = list[command];
        if (packet.command >= 0x100) {
            cacheCount = packet.command - 0x100;
            assert(cacheCount > 0 && cacheCount <= 32);
            assert(packet.argument + cacheCount * sizeof(Vtx) <= used);
            cache = (Vtx *)(arena.bytes + packet.argument);
        } else if (packet.command == 2 || packet.command == 3) {
            int triangles = packet.command == 3 ? 2 : 1;
            for (int t = 0; t < triangles; t++, triangle++) {
                for (int corner = 0; corner < 3; corner++) {
                    int local = (packet.argument >> (15 * t + 5 * corner)) & 31;
                    assert(cache && local < cacheCount && triangle < count);
                    Vtx expected;
                    if (map) {
                        int mapping = map[map[0] * 3 + 2 + triangle * 3 + corner];
                        int vertex = map[mapping * 3 + 1];
                        make_vertex(&expected, 0, mesh[vertex].pos[0], mesh[vertex].pos[1],
                                    mesh[vertex].pos[2], map[mapping * 3 + 2], map[mapping * 3 + 3],
                                    mesh[vertex].norm[0], mesh[vertex].norm[1], mesh[vertex].norm[2], 173);
                    } else {
                        struct SnowFlakeVertex offsets[] = {
                            gSnowFlakeVertex1, gSnowFlakeVertex2, gSnowFlakeVertex3};
                        expected = gSnowTempVtx[corner];
                        expected.v.ob[0] = particles[triangle].xPos + offsets[corner].x;
                        expected.v.ob[1] = particles[triangle].yPos + offsets[corner].y;
                        expected.v.ob[2] = particles[triangle].zPos + offsets[corner].z;
                    }
                    assert(memcmp(&cache[local], &expected, sizeof(Vtx)) == 0);
                }
            }
        }
    }
    assert(triangle == count);
}
int main(void) {
    for (int i = 0; i < 140; i++) particles[i] = (struct EnvFxParticle){i * 10, i * 11, i * 12};
    for (int i = 0; i < 3; i++) make_vertex(gSnowTempVtx, i, 0, 0, 0, i * 50, i * 60, 127, 126, 125, 255);
    int counts[] = {0, 5, 10, 15, 25, 30, 105, 110};
    Vec3s position = {0};
    for (int mode = ENVFX_SNOW_NORMAL; mode <= ENVFX_SNOW_BLIZZARD; mode++) {
        for (int i = 0; i < ARRAY_COUNT(counts); i++) {
            reset(); gSnowParticleCount = counts[i];
            verify_triangles(envfx_update_snow(mode, position, position, position), NULL, counts[i]);
        }
    }
    for (int failure = 1; failure <= 2; failure++) {
        reset(); failAllocation = failure;
        assert(envfx_update_snow(ENVFX_SNOW_NORMAL, position, position, position) == NULL);
    }
    const s16 *maps[] = {inside_castle_seg7_painting_texture_map_bottom_07021AE0,
                        inside_castle_seg7_painting_texture_map_top_07021FFC};
    bob_painting.textureMaps = maps; gCurrLevelNum = LEVEL_CASTLE;
    for (int i = 0; i < 256; i++) {
        mesh[i] = (struct PaintingMeshVertex){{i * 3, i * 4, i * 5}, {i, i / 2, i / 3}};
    }
    for (int image = 0; image < 2; image++) {
        s16 *map = (s16 *)maps[image];
        int tris = map[map[0] * 3 + 1];
        reset(); packetAllocation = 1;
        verify_triangles(render_painting(NULL, 64, 32, map, map[0], tris, 173), map, tris);
        for (int failure = 1; failure <= 2; failure++) {
            reset(); failAllocation = failure;
            assert(render_painting(NULL, 64, 32, map, map[0], tris, 173) == NULL);
        }
        assert(painting_vertex_groups(map, map[0] - 1, tris) == NULL);
        assert(painting_vertex_groups(map, map[0], tris - 1) == NULL);
    }
    s16 replacement[sizeof(inside_castle_seg7_painting_texture_map_bottom_07021AE0) / sizeof(s16)];
    memcpy(replacement, maps[0], sizeof(replacement)); maps[0] = replacement;
    reset(); packetAllocation = 1;
    verify_triangles(render_painting(NULL, 64, 32, replacement, 85, 132, 173), replacement, 132);
    maps[0] = inside_castle_seg7_painting_texture_map_bottom_07021AE0;
    assert(painting_vertex_groups(replacement, 85, 132) == NULL);
    gCurrLevelNum = 7;
    assert(render_painting(NULL, 64, 32, (s16 *)maps[0], 85, 132, 173) == &fallback);
    assert(unbatched == 1);
    return 0;
}
'''
        for marker, text in (("DECLARATIONS", declarations), ("MAPS", maps),
                             ("GROUPS", groups), ("PRODUCTION", production)):
            fixture = fixture.replace("/* " + marker + " */", text)
        with tempfile.TemporaryDirectory(prefix="vertex-groups-") as directory:
            path = Path(directory)
            (path / "test.c").write_text(fixture)
            subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=address,undefined", str(path / "test.c"),
                            "-o", str(path / "test")], check=True)
            subprocess.run([str(path / "test")], check=True,
                           env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})


if __name__ == "__main__":
    unittest.main()
