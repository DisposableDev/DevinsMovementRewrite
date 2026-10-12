"""Collision capture accounting and production query hooks; no target timing claims."""
from pathlib import Path
import os
import re
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class CollisionProfileTests(unittest.TestCase):
    def test_capture(self):
        header = (ROOT / "src/game/collision_profile.h").read_text()
        header = re.sub(r'^#include[^\n]*\n', '', header, flags=re.M)
        query_header = (ROOT / "src/engine/surface_query.h").read_text()
        query_header = re.sub(r'^#include[^\n]*\n', '', query_header, flags=re.M)
        profile = Source(ROOT / "src/game/collision_profile.c")
        profile_source = re.sub(r'^#include[^\n]*\n', '', profile.text, flags=re.M)
        profiler = Source(ROOT / "src/game/profiling.c")
        profiler_header = Source(ROOT / "src/game/profiling.h")
        time_type = re.search(r'typedef struct \{[^}]+\} ProfileTimeData;', profiler_header.text)[0]
        time_definitions = (profiler_header.macros("PROFILING_BUFFER_SIZE|AUDIO_SUBSET_ENTRIES")
                            + profiler_header.declaration("enum", "ProfilerTime") + time_type)
        surface = Source(ROOT / "src/engine/surface_collision.c")
        math = Source(ROOT / "src/engine/math_util.c")
        loading = Source(ROOT / "src/engine/surface_load.c")
        display = Source(ROOT / "src/game/puppyprint.c")
        functions = '\n'.join([
            *(profiler.function(name) for name in (
                "buffer_update", "profiler_clear_main_thread_samples", "profiler_frame_setup",
                "profiler_collision_completed")),
            surface.function("check_within_floor_triangle_bounds"),
            surface.function("floor_height_cutoff"), surface.function("find_floor_from_list"), math.function("find_surface_on_ray_candidates_internal"),
            math.function("find_surface_on_ray_candidates"),
            loading.function("alloc_surface"), loading.function("alloc_surface_node"),
        ])
        code = r'''
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef int32_t s32;
typedef int16_t s16;
typedef float f32;
typedef int SurfaceType;
typedef f32 Vec3f[3];
typedef s16 Vec3s[3];
typedef s32 Vec3i[3];
#define TRUE 1
#define FALSE 0
#define ARRAY_COUNT(a) (sizeof(a) / sizeof(*(a)))
#define bzero(p, n) memset(p, 0, n)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PUPPYPRINT_GET_COLLISION_SNAPSHOT() u32 first = osGetCount()
#define OS_CYCLES_TO_USEC(x) ((u64) (x) * 64 / 3000)
/* HEADER */
/* TIME_DEFINITIONS */
static ProfileTimeData all_profiling_data[PROFILER_TIME_COUNT];
static int profile_buffer_index = -1;
static u32 preempted_time, prev_time, cur_start, ticks;
static u32 prev_fps_vblanks, fps_sample_count, gNumVblanks;
static f32 fps_vi_rate;
static u32 sUpdatedObjects, sSleepingObjects, collision_time;
u32 gStaticSurfaceGeneration = 1;
int fDebug, sPPDebugPage, sDebugMenu;
#define PUPPYPRINT_PAGE_COLLISION 0
static u32 osGetCount(void) { return ticks; }
static u32 __osDisableInt(void) { return 0; }
static void __osRestoreInt(u32 saved) { (void) saved; }
static f32 get_vi_refresh_rate(void) { return 60.0f; }
static void profiler_collision_update(u32 start) { collision_time += ticks - start; }

struct Surface {
    SurfaceType type;
    s32 force, flags, room, lowerY, upperY;
    s16 vertex1[3], vertex2[3], vertex3[3];
    struct { f32 x, y, z; } normal;
    f32 originOffset;
    void *object;
};
struct SurfaceNode { struct SurfaceNode *next; struct Surface *surface; };
#define ALWAYS_INLINE inline __attribute__((always_inline))
static struct SurfaceNode *gStaticSurfacePartition[1][1][1];
static struct SurfaceNode *gDynamicSurfacePartition[1][1][1];
#define STATIC_SURFACE_INDEX 1
#define DYNAMIC_SURFACE_INDEX 0
#define NUM_CELLS 1
#define NUM_SPATIAL_PARTITIONS 1
/* QUERY_HEADER */
struct StaticSurfaceIndex gStaticSurfaceIndex;
static union { struct Surface surface; unsigned char bytes[4096]; } dynamicPool, staticPool;
void *gDynamicSurfacePool = dynamicPool.bytes;
static void *gDynamicSurfacePoolEnd = dynamicPool.bytes;
static void *gCurrStaticSurfacePoolEnd = staticPool.bytes;
static u32 gSurfaceNodesAllocated, gSurfacesAllocated;
#define SURFACE_DEFAULT 0
#define SURFACE_INTANGIBLE 1
#define SURFACE_CAMERA_BOUNDARY 2
#define SURFACE_FLAGS_NONE 0
#define SURFACE_FLAG_NO_CAM_COLLISION 1
#define FIND_FLOOR_BUFFER 0
#define COLLISION_FLAG_INCLUDE_INTANGIBLE 1
#define COLLISION_FLAG_CAMERA 2
#define COLLISION_FLAG_RETURN_FIRST 4
static s32 gCollisionFlags;
static s32 surface_is_passable_vanish(struct Surface *surface) { (void) surface; return FALSE; }
#define get_surface_height_at_location(x, z, s) (-((x)*(s)->normal.x + (z)*(s)->normal.z + (s)->originOffset) / (s)->normal.y)
#define RAYCAST_MARIO_WALL 1
static int rayTests;
static s32 ray_surface_intersect(Vec3f orig, Vec3f dir, f32 length, struct Surface *s,
                                Vec3f hit, f32 *distance, s32 flags) {
    (void) orig; (void) dir; (void) length; (void) s;
    (void) hit; (void) distance; (void) flags;
    rayTests++;
    return FALSE;
}
#define vec3f_copy(dest, src) memcpy(dest, src, sizeof(Vec3f))
/* PROFILE_SOURCE */
/* FUNCTIONS */

static void test_queries_and_allocations(void) {
    fDebug = TRUE;
    profiler_frame_setup();
    struct Surface surfaces[4] = {
        { .type = SURFACE_INTANGIBLE, .lowerY = 115, .upperY = 125 },
        { .lowerY = 95, .upperY = 105, .normal.y = 1, .originOffset = -100,
          .vertex1 = {-10,100,-10}, .vertex2 = {-10,100,10}, .vertex3 = {10,100,-10} },
        { .lowerY = -5, .upperY = 5 },
        { .lowerY = -55, .upperY = -45 }
    };
    struct SurfaceNode nodes[4];
    for (int i = 0; i < 4; i++) {
        nodes[i] = (struct SurfaceNode) {i < 3 ? &nodes[i+1] : NULL, &surfaces[i]};
    }
    f32 height = -11000;
    assert(find_floor_from_list(surface_query_list(nodes), 0, 200, 0, &height) == &surfaces[1]);
    assert(height == 100);
    Vec3f origin = {0,0,0}, direction = {1,0,0}, hit;
    struct Surface *hitSurface = NULL;
    f32 length = 20;
    gStaticSurfacePartition[0][0][0] = nodes;
    find_surface_on_ray_candidates(surface_query_cell(0, 0, 0, SURFACE_QUERY_STATIC),
                                   origin, direction, 20, &hitSurface, hit, &length, 0);
    assert(rayTests == 1 && hitSurface == NULL && length == 20);
    assert(alloc_surface(TRUE) == (struct Surface *) dynamicPool.bytes);
    alloc_surface_node(TRUE);
    alloc_surface(FALSE);
    alloc_surface_node(FALSE);
#if COLLISION_PROFILING && defined(PUPPYPRINT_DEBUG) && defined(USE_PROFILER)
    assert(gCollisionProfile.current[COLLISION_PROFILE_FLOOR_NODES] == 3);
    assert(gCollisionProfile.current[COLLISION_PROFILE_RAY_NODES] == 4);
    assert(gCollisionProfile.current[COLLISION_PROFILE_DYNAMIC_SURFACES] == 1);
    assert(gCollisionProfile.current[COLLISION_PROFILE_DYNAMIC_NODES] == 1);
    all_profiling_data[PROFILER_TIME_TOTAL].counts[profile_buffer_index] = 500;
    collision_time = 600;
    // A stale collision ring slot must never become this frame's captured timing.
    all_profiling_data[PROFILER_TIME_COLLISION].counts[profile_buffer_index] = 999;
    profiler_collision_completed();
    assert(collision_profile_average(COLLISION_PROFILE_COLLISION_TICKS) == 600);
    assert(collision_profile_average(COLLISION_PROFILE_MAIN_CPU_TICKS) == 500);
    assert(collision_profile_average(COLLISION_PROFILE_DYNAMIC_BYTES)
           == sizeof(struct Surface) + sizeof(struct SurfaceNode));
    fDebug = FALSE;
    profiler_frame_setup();
    int amountCalls = 0;
    COLLISION_PROFILE_ADD(COLLISION_PROFILE_FLOOR_NODES, ++amountCalls);
    assert(amountCalls == 0);
    profiler_collision_completed();
    assert(gCollisionProfile.frameCount == 1);
    fDebug = TRUE;
    profiler_frame_setup();
    assert(gCollisionProfile.frameCount == 0);
    assert(collision_profile_average(COLLISION_PROFILE_FLOOR_NODES) == 0);
    sDebugMenu = TRUE;
    profiler_frame_setup();
    assert(!gCollisionProfile.active);
    sDebugMenu = FALSE;
#else
    int amountCalls = 0;
    COLLISION_PROFILE_ADD(COLLISION_PROFILE_FLOOR_NODES, ++amountCalls);
    assert(amountCalls == 0);
#endif
}

static void test_floor_cutoff(void) {
    // This is exactly the old float comparison for integral surface bounds,
    // including negative fractions and a height seeded by a dynamic floor.
    const f32 seeds[] = {-11000, -5.75f, -1, -0.25f, 0, 0.25f, 5, 5.75f,
                         32766.5f, -INFINITY, INFINITY, NAN};
    for (unsigned i = 0; i < ARRAY_COUNT(seeds); i++) {
        for (int upper = -32768; upper <= 32767; upper++) {
            assert((upper < floor_height_cutoff(seeds[i])) == (upper < seeds[i]));
        }
    }
    struct Surface surfaces[] = {
        { .lowerY = 95, .upperY = 105, .normal.y = 1, .originOffset = -100.75f,
          .vertex1 = {-10,100,-10}, .vertex2 = {-10,100,10}, .vertex3 = {10,100,-10} },
        { .lowerY = 95, .upperY = 105, .normal.y = 1, .originOffset = -100.75f,
          .vertex1 = {-10,100,-10}, .vertex2 = {-10,100,10}, .vertex3 = {10,100,-10} },
        { .lowerY = -5, .upperY = 5, .normal.y = 1, .originOffset = 0.25f,
          .vertex1 = {-10,0,-10}, .vertex2 = {-10,0,10}, .vertex3 = {10,0,-10} }
    };
    struct SurfaceNode nodes[3];
    for (int i = 0; i < 3; i++) {
        nodes[i] = (struct SurfaceNode) {i < 2 ? &nodes[i + 1] : NULL, &surfaces[i]};
    }
    gCollisionFlags = 0;
    f32 height = 100.5f;
    assert(find_floor_from_list(surface_query_list(nodes), 0, 200, 0, &height) == &surfaces[0]);
    assert(height == 100.75f); // Equal-height floors retain the first winner.
    assert(find_floor_from_list(surface_query_list(nodes), 0, 200, 0, &height) == NULL);
    assert(height == 100.75f); // Preserve an equally high incoming dynamic floor.
    height = -0.5f;
    assert(find_floor_from_list(surface_query_list(nodes), 0, 0, 0, &height) == &surfaces[2]);
    assert(height == -0.25f); // Above-query rejection must leave lower floors reachable.
    height = 0;
    assert(find_floor_from_list(surface_query_list(nodes), 0, 0, 0, &height) == NULL);
    assert(height == 0);
}

#if COLLISION_PROFILING && defined(PUPPYPRINT_DEBUG) && defined(USE_PROFILER)
static void test_ring_and_generation(void) {
    collision_profile_begin_frame(FALSE, 1);
    collision_profile_begin_frame(TRUE, 1);
    assert(collision_profile_average(COLLISION_PROFILE_FLOOR_NODES) == 0);
    for (u32 value = 1; value <= 20; value++) {
        collision_profile_begin_frame(TRUE, 1);
        COLLISION_PROFILE_ADD(COLLISION_PROFILE_FLOOR_NODES, value);
        collision_profile_end_frame(1, value, value * 2, value * 3);
    }
    assert(gCollisionProfile.frameCount == 15);
    assert(collision_profile_average(COLLISION_PROFILE_FLOOR_NODES) == 13);
    assert(gCollisionProfile.peak[COLLISION_PROFILE_FLOOR_NODES] == 20);
    for (int i = 0; i < 15; i++) {
        collision_profile_begin_frame(TRUE, 1);
        collision_profile_end_frame(1, 0, 0, 0);
    }
    assert(collision_profile_average(COLLISION_PROFILE_FLOOR_NODES) == 0);
    assert(gCollisionProfile.peak[COLLISION_PROFILE_FLOOR_NODES] == 20);
    collision_profile_begin_frame(TRUE, 1);
    COLLISION_PROFILE_ADD(COLLISION_PROFILE_FLOOR_NODES, 100);
    collision_profile_end_frame(2, 100, 100, 100);
    assert(gCollisionProfile.frameCount == 0);
    assert(gCollisionProfile.peak[COLLISION_PROFILE_FLOOR_NODES] == 0);
    for (int i = 0; i < 15; i++) {
        collision_profile_begin_frame(TRUE, 2);
        COLLISION_PROFILE_ADD(COLLISION_PROFILE_FLOOR_NODES, UINT32_MAX);
        collision_profile_end_frame(2, UINT32_MAX, UINT32_MAX, UINT32_MAX);
    }
    assert(collision_profile_average(COLLISION_PROFILE_FLOOR_NODES) == UINT32_MAX);
    collision_profile_begin_frame(TRUE, 3);
    assert(gCollisionProfile.frameCount == 0);
}

static u32 gTotalStaticSurfaceData = UINT32_MAX;
#define DYNAMIC_SURFACE_POOL_SIZE 32768U
#define SCREEN_WIDTH 320
#define FONT_OUTLINE 1
#define PRINT_ALL -1
#define PRINT_TEXT_ALIGN_LEFT 0
#define PRINT_TEXT_ALIGN_RIGHT 1
static int printed;
static f32 profiler_get_fps(void) { return 30.0f; }
static u32 persistent_surfaces_published_meshes(void) { return 0; }
static u32 persistent_surfaces_used_bytes(void) { return 0; }
static u32 persistent_surfaces_reserved_bytes(void) { return 0; }
static void print_small_text_light(int x, int y, const char *text, int align, int count, int font) {
    (void) align; (void) count; (void) font;
    assert(x >= 0 && x < 320 && y >= 0 && y < 240);
    assert(strlen(text) < 256);
    int lines = 1;
    for (const char *p = text; *p; p++) {
        if (*p == '\n') lines++;
    }
    assert(y + lines * 12 < 198); // Leave the existing visual-debug footer clear.
    printed++;
}
/* DISPLAY */
static void test_display(void) {
    for (int metric = 0; metric < COLLISION_PROFILE_METRIC_COUNT; metric++) {
        gCollisionProfile.peak[metric] = UINT32_MAX;
        gCollisionProfile.total[metric] = (u64) UINT32_MAX * 15;
    }
    gCollisionProfile.frameCount = 15;
    puppyprint_render_collision_profile();
    assert(printed == 14);
}
#endif
int main(void) {
    test_queries_and_allocations();
    test_floor_cutoff();
#if COLLISION_PROFILING && defined(PUPPYPRINT_DEBUG) && defined(USE_PROFILER)
    test_ring_and_generation();
    test_display();
#endif
    puts("collision profile: accounting checked; floor/ray results preserved");
    return 0;
}
'''
        code = (code.replace('/* HEADER */', header)
                .replace('/* TIME_DEFINITIONS */', time_definitions)
                .replace('/* QUERY_HEADER */', query_header)
                .replace('/* PROFILE_SOURCE */', profile_source)
                .replace('/* FUNCTIONS */', functions)
                .replace('/* DISPLAY */', display.function("puppyprint_render_collision_profile")))
        with tempfile.TemporaryDirectory(prefix="collision-profile-") as directory:
            path = Path(directory)
            (path / "test.c").write_text(code)
            for enabled, debug in ((1, True), (0, True), (1, False)):
                with self.subTest(enabled=enabled, debug=debug):
                    subprocess.run([
                        "cc", "-std=c99", "-O1", "-Wall", "-Wextra", "-Werror",
                        "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                        f"-DCOLLISION_PROFILING={enabled}", "-DUSE_PROFILER",
                        *(["-DPUPPYPRINT_DEBUG"] if debug else []),
                        str(path / "test.c"), "-o", str(path / "test"),
                    ], check=True)
                    subprocess.run([str(path / "test")], check=True,
                                   env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})


if __name__ == "__main__":
    unittest.main()
