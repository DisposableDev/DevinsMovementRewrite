"""Ordered candidate scans compared with direct cell traversal, under sanitizers."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class SurfaceQueryTests(unittest.TestCase):
    def test_order_and_publication(self):
        query = Source(ROOT / "src/engine/surface_query.h")
        definitions = (query.declaration("enum", "SurfaceQueryStorage")
                       + query.macros("STATIC_SURFACE_GROUP_SIZE")
                       + query.declaration("struct", "SurfaceQueryBounds")
                       + query.declaration("struct", "StaticSurfaceIndex")
                       + query.declaration("struct", "SurfaceQueryIterator")
                       + Source(ROOT / "include/macros.h").macros("ALIGN"))
        functions = '\n'.join(query.function(name) for name in (
            "surface_query_list", "surface_query_is_empty", "surface_query_has_multiple",
            "surface_query_identity", "surface_query_cell_is_empty", "surface_query_cell", "surface_query_next",
            "surface_query_next_in_bounds"))
        functions += "\n#if STATIC_SURFACE_INDEX\n" + Source(
            ROOT / "src/engine/surface_load.c").function("extend_surface_query_bounds") + "\n" + Source(
            ROOT / "src/engine/surface_load.c").function("build_static_surface_index") + "\n#endif"
        code = r'''
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef int32_t s32;
typedef uint32_t u32;
typedef uint8_t u8;
typedef int16_t s16;
typedef float f32;
typedef s16 Vec3s[3];
typedef f32 Vec3f[3];
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ALWAYS_INLINE inline __attribute__((always_inline))
#define TRUE 1
#define NUM_CELLS 2
#define NUM_SPATIAL_PARTITIONS 4
#define NUM_PARTITIONS NUM_SPATIAL_PARTITIONS
struct Surface { int id; s16 lowerY, upperY; Vec3s vertex1, vertex2, vertex3; };
struct SurfaceNode { struct SurfaceNode *next; struct Surface *surface; };
static struct SurfaceNode *gStaticSurfacePartition[NUM_CELLS][NUM_CELLS][NUM_PARTITIONS];
static struct SurfaceNode *gDynamicSurfacePartition[NUM_CELLS][NUM_CELLS][NUM_PARTITIONS];
/* DEFINITIONS */
u32 gStaticSurfaceGeneration;
#if STATIC_SURFACE_INDEX
struct StaticSurfaceIndex gStaticSurfaceIndex;
static union { void *alignment; u8 bytes[65536]; } pool;
static void *gCurrStaticSurfacePool = pool.bytes;
static void *gCurrStaticSurfacePoolEnd;
#endif
/* FUNCTIONS */
static void compare_scan(int x, int z, int partition, enum SurfaceQueryStorage storage,
                         struct SurfaceNode *head) {
    struct SurfaceQueryIterator iterator = surface_query_cell(x, z, partition, storage);
#if STATIC_SURFACE_INDEX
    if (iterator.remaining < 0)
#endif
    { assert(surface_query_identity(iterator) == head); }
    assert(surface_query_is_empty(iterator) == (head == NULL));
    assert(surface_query_cell_is_empty(x, z, partition, storage) == (head == NULL));
    assert(surface_query_has_multiple(iterator) == (head != NULL && head->next != NULL));
    struct SurfaceQueryIterator filtered = surface_query_list(head);
    for (struct SurfaceNode *node = head; node != NULL; node = node->next) {
        assert(surface_query_identity(iterator) != NULL);
        assert(surface_query_next(&filtered) == node->surface);
        assert(surface_query_next(&iterator) == node->surface);
    }
    assert(surface_query_is_empty(iterator));
    assert(!surface_query_has_multiple(iterator));
    assert(surface_query_identity(iterator) == NULL);
    assert(surface_query_next(&filtered) == NULL);
    assert(surface_query_next(&iterator) == NULL);
    assert(surface_query_next(&iterator) == NULL);
}
static void test_all_cells(void) {
    struct Surface surfaces[2] = {{.id = 1}, {.id = 1}};
    // Repeated references and equal-valued surfaces must retain their original order.
    struct SurfaceNode staticNodes[3] = {
        {&staticNodes[1], &surfaces[0]}, {&staticNodes[2], &surfaces[1]}, {NULL, &surfaces[0]}
    };
    struct SurfaceNode dynamicNodes[2] = {
        {&dynamicNodes[1], &surfaces[1]}, {NULL, &surfaces[0]}
    };
    for (int z = 0; z < NUM_CELLS; z++) {
        for (int x = 0; x < NUM_CELLS; x++) {
            for (int partition = 0; partition < NUM_PARTITIONS; partition++) {
                gStaticSurfacePartition[z][x][partition] = staticNodes;
                gDynamicSurfacePartition[z][x][partition] = dynamicNodes;
                compare_scan(x, z, partition, SURFACE_QUERY_STATIC,
                             gStaticSurfacePartition[z][x][partition]);
                compare_scan(x, z, partition, SURFACE_QUERY_DYNAMIC,
                             gDynamicSurfacePartition[z][x][partition]);
                compare_scan(x, z, partition, SURFACE_QUERY_EMPTY, NULL);
                struct SurfaceQueryIterator first = surface_query_cell(x, z, partition, SURFACE_QUERY_STATIC);
                struct SurfaceQueryIterator second = surface_query_cell(x, z, partition, SURFACE_QUERY_STATIC);
                assert(surface_query_next(&first) == &surfaces[0]);
                assert(surface_query_next(&first) == &surfaces[1]);
                assert(surface_query_next(&second) == &surfaces[0]);
                gStaticSurfacePartition[z][x][partition] = NULL;
                gDynamicSurfacePartition[z][x][partition] = NULL;
                compare_scan(x, z, partition, SURFACE_QUERY_STATIC, NULL);
                compare_scan(x, z, partition, SURFACE_QUERY_DYNAMIC, NULL);
            }
        }
    }
    // EMPTY never reads a cell, including when a caller excludes dynamic geometry.
    struct SurfaceQueryIterator empty = surface_query_cell(-1, NUM_CELLS, -1, SURFACE_QUERY_EMPTY);
    assert(surface_query_next(&empty) == NULL);
    assert(surface_query_cell_is_empty(-1, NUM_CELLS, -1, SURFACE_QUERY_EMPTY));
}
static void test_fresh_publication(void) {
    struct Surface surfaces[2] = {{.id = 1}, {.id = 2}};
    struct SurfaceNode node = {NULL, &surfaces[0]};
    for (int storage = SURFACE_QUERY_STATIC; storage <= SURFACE_QUERY_DYNAMIC; storage++) {
        node.surface = &surfaces[0];
        struct SurfaceNode **head = storage == SURFACE_QUERY_STATIC
            ? &gStaticSurfacePartition[1][0][3] : &gDynamicSurfacePartition[1][0][3];
        *head = &node;
        compare_scan(0, 1, 3, storage, &node);
        *head = NULL;
        compare_scan(0, 1, 3, storage, NULL);
        node.surface = &surfaces[1];
        *head = &node;
        // A fresh scan sees reused node storage and replacement surface identity.
        struct SurfaceQueryIterator iterator = surface_query_cell(0, 1, 3, storage);
        assert(surface_query_next(&iterator) == &surfaces[1]);
        assert(surface_query_next(&iterator) == NULL);
        *head = NULL;
    }
}
#if STATIC_SURFACE_INDEX
static void test_packed_cells_and_lifetime(void) {
    struct Surface surfaces[16][2] = {0};
    struct SurfaceNode nodes[16][3];
    int cell = 0;
    for (int z = 0; z < NUM_CELLS; z++) {
        for (int x = 0; x < NUM_CELLS; x++) {
            for (int part = 0; part < NUM_PARTITIONS; part++, cell++) {
                surfaces[cell][0].id = surfaces[cell][1].id = cell;
                nodes[cell][0] = (struct SurfaceNode){&nodes[cell][1], &surfaces[cell][0]};
                nodes[cell][1] = (struct SurfaceNode){&nodes[cell][2], &surfaces[cell][1]};
                nodes[cell][2] = (struct SurfaceNode){NULL, &surfaces[cell][0]};
                gStaticSurfacePartition[z][x][part] = nodes[cell];
                gDynamicSurfacePartition[z][x][part] = &nodes[cell][1];
            }
        }
    }
    memset(pool.bytes, 0xA5, sizeof(pool.bytes));
    gCurrStaticSurfacePoolEnd = pool.bytes + 32;
    gStaticSurfaceGeneration++;
    build_static_surface_index(sizeof(pool.bytes));
    assert(gStaticSurfaceIndex.offsets != NULL);
    size_t offsets = ALIGN((16 + 1) * sizeof(u32), sizeof(struct Surface *));
    size_t bytes = 32 + offsets + 48 * sizeof(struct Surface *)
        + 6 * sizeof(struct SurfaceQueryBounds);
    assert(gCurrStaticSurfacePoolEnd == pool.bytes + bytes);
    for (size_t i = 0; i < 32; i++) assert(pool.bytes[i] == 0xA5);
    for (size_t i = bytes; i < sizeof(pool.bytes); i++) assert(pool.bytes[i] == 0xA5);
    cell = 0;
    for (int z = 0; z < NUM_CELLS; z++) {
        for (int x = 0; x < NUM_CELLS; x++) {
            for (int part = 0; part < NUM_PARTITIONS; part++, cell++) {
                struct SurfaceQueryIterator packed = surface_query_cell(x, z, part, SURFACE_QUERY_STATIC);
                assert(packed.remaining == 3);
                struct SurfaceQueryIterator second = packed;
                assert(surface_query_next(&packed) == &surfaces[cell][0]);
                assert(surface_query_next(&packed) == &surfaces[cell][1]);
                assert(!surface_query_has_multiple(packed));
                assert(surface_query_next(&second) == &surfaces[cell][0]);
                compare_scan(x, z, part, SURFACE_QUERY_STATIC, nodes[cell]);
                compare_scan(x, z, part, SURFACE_QUERY_DYNAMIC, &nodes[cell][1]);
            }
        }
    }
    // Late publication changes the live list and generation. A stale index
    // must yield to it immediately even though the index storage still exists.
    struct Surface late = {.id = 100};
    struct SurfaceNode lateNode = {nodes[0], &late};
    gStaticSurfacePartition[0][0][0] = &lateNode;
    gStaticSurfaceGeneration++;
    struct SurfaceQueryIterator lateScan = surface_query_cell(0, 0, 0, SURFACE_QUERY_STATIC);
    assert(lateScan.remaining == -1);
    compare_scan(0, 0, 0, SURFACE_QUERY_STATIC, &lateNode);
    // Rebuilding at the same addresses publishes the new first entry.
    gCurrStaticSurfacePoolEnd = pool.bytes + 32;
    build_static_surface_index(sizeof(pool.bytes));
    lateScan = surface_query_cell(0, 0, 0, SURFACE_QUERY_STATIC);
    assert(lateScan.remaining == 4);
    assert(surface_query_next(&lateScan) == &late);
    // Simulate the next area's pool reuse; an empty world has no stale entries.
    memset(gStaticSurfacePartition, 0, sizeof(gStaticSurfacePartition));
    memset(gDynamicSurfacePartition, 0, sizeof(gDynamicSurfacePartition));
    gStaticSurfaceGeneration++;
    gCurrStaticSurfacePoolEnd = pool.bytes + 32;
    build_static_surface_index(sizeof(pool.bytes));
    assert(gStaticSurfaceIndex.offsets != NULL);
    for (int z = 0; z < NUM_CELLS; z++) {
        for (int x = 0; x < NUM_CELLS; x++) {
            for (int part = 0; part < NUM_PARTITIONS; part++)
                compare_scan(x, z, part, SURFACE_QUERY_STATIC, NULL);
        }
    }
}
static void test_bounded_groups(void) {
    struct Surface surfaces[40] = {0};
    struct SurfaceNode nodes[40];
    struct SurfaceNode prefix[3] = {
        {&prefix[1], &surfaces[0]}, {&prefix[2], &surfaces[0]}, {NULL, &surfaces[0]}
    };
    memset(gStaticSurfacePartition, 0, sizeof(gStaticSurfacePartition));
    gStaticSurfacePartition[0][0][0] = prefix;
    gStaticSurfacePartition[0][0][1] = nodes;
    for (int i = 0; i < 40; i++) {
        surfaces[i].id = i;
        int axis = i >= 5 && i < 13 ? 0 : i >= 21 && i < 29 ? 2 : 1;
        int coordinate = i >= 5 && i < 13 ? 100 : i >= 21 && i < 29 ? -100
            : i >= 29 && i < 37 ? 100 : 0;
        for (int vertex = 0; vertex < 3; vertex++) {
            s16 *point = vertex == 0 ? surfaces[i].vertex1
                : vertex == 1 ? surfaces[i].vertex2 : surfaces[i].vertex3;
            point[axis] = coordinate + (vertex == 2 && coordinate != 0);
        }
        surfaces[i].lowerY = axis == 1 ? coordinate : 0;
        surfaces[i].upperY = axis == 1 && coordinate != 0 ? coordinate + 1 : 0;
        nodes[i] = (struct SurfaceNode){i == 39 ? NULL : &nodes[i + 1], &surfaces[i]};
    }
    nodes[20].surface = &surfaces[13];
    gStaticSurfaceGeneration++;
    gCurrStaticSurfacePoolEnd = pool.bytes + 32;
    build_static_surface_index(sizeof(pool.bytes));
    assert(gStaticSurfaceIndex.offsets != NULL);
    const Vec3f minimum = {-1, -1, -1}, maximum = {1, 1, 1};
    struct SurfaceQueryIterator bounded = surface_query_cell(0, 0, 1, SURFACE_QUERY_STATIC);
    struct SurfaceQueryIterator original = bounded;
    // A cell starts inside a block. Its five-entry prefix and three-entry tail
    // remain unfiltered; only its three wholly disjoint eight-entry groups skip.
    const int expected[] = {0, 1, 2, 3, 4, 13, 14, 15, 16, 17, 18, 19, 13, 37, 38, 39};
    for (size_t i = 0; i < sizeof(expected) / sizeof(*expected); i++) {
        assert(surface_query_next_in_bounds(&bounded, minimum, maximum) == &surfaces[expected[i]]);
    }
    assert(surface_query_next_in_bounds(&bounded, minimum, maximum) == NULL);
    assert(surface_query_next_in_bounds(&bounded, minimum, maximum) == NULL);
    assert(surface_query_next(&original) == &surfaces[0]);
    // Touching either group boundary is inclusive, with no lost edge contacts.
    const Vec3f touches[] = {{100, 0, 0}, {101, 0, 0}, {0, 0, -100},
                            {0, 0, -99}, {0, 100, 0}, {0, 101, 0}};
    const int first[] = {5, 5, 21, 21, 29, 29};
    for (size_t touch = 0; touch < sizeof(touches) / sizeof(*touches); touch++) {
        bounded = surface_query_cell(0, 0, 1, SURFACE_QUERY_STATIC);
        for (int i = 0; i < 5; i++) {
            assert(surface_query_next_in_bounds(&bounded, touches[touch], touches[touch]) == &surfaces[i]);
        }
        for (int i = first[touch]; i < first[touch] + 8; i++) {
            assert(surface_query_next_in_bounds(&bounded, touches[touch], touches[touch]) == &surfaces[i]);
        }
        for (int i = 37; i < 40; i++) {
            assert(surface_query_next_in_bounds(&bounded, touches[touch], touches[touch]) == &surfaces[i]);
        }
        assert(surface_query_next_in_bounds(&bounded, touches[touch], touches[touch]) == NULL);
    }
    // Caller-owned and dynamic lists never use immutable static bounds.
    bounded = surface_query_list(nodes);
    for (int i = 0; i < 40; i++) {
        assert(surface_query_next_in_bounds(&bounded, minimum, maximum) == nodes[i].surface);
    }
    gStaticSurfaceIndex.offsets = NULL;
    memset(gStaticSurfacePartition, 0, sizeof(gStaticSurfacePartition));
}
static void test_pool_capacity_fallback(void) {
    struct Surface surface = {.id = 1};
    struct SurfaceNode node = {NULL, &surface};
    gStaticSurfacePartition[0][0][0] = &node;
    gStaticSurfaceGeneration++;
    size_t offsetBytes = ALIGN((16 + 1) * sizeof(u32), sizeof(struct Surface *));
    size_t needed = 32 + offsetBytes + sizeof(struct Surface *)
        + sizeof(struct SurfaceQueryBounds);
    const size_t limits[] = {0, 31, 32, 32 + offsetBytes - 1, 32 + offsetBytes + sizeof(struct Surface *), needed - 1};
    for (size_t i = 0; i < sizeof(limits) / sizeof(*limits); i++) {
        memset(pool.bytes, 0xA5, sizeof(pool.bytes));
        gCurrStaticSurfacePoolEnd = pool.bytes + 32;
        build_static_surface_index(limits[i]);
        assert(gStaticSurfaceIndex.offsets == NULL);
        assert(gCurrStaticSurfacePoolEnd == pool.bytes + 32);
        for (size_t byte = 0; byte < sizeof(pool.bytes); byte++) assert(pool.bytes[byte] == 0xA5);
        compare_scan(0, 0, 0, SURFACE_QUERY_STATIC, &node);
    }
    build_static_surface_index(needed);
    assert(gStaticSurfaceIndex.offsets != NULL);
    assert(gCurrStaticSurfacePoolEnd == pool.bytes + needed);
    compare_scan(0, 0, 0, SURFACE_QUERY_STATIC, &node);
    gStaticSurfaceIndex.offsets = NULL;
    gStaticSurfacePartition[0][0][0] = NULL;
}
#endif
int main(void) {
    test_all_cells();
    test_fresh_publication();
#if STATIC_SURFACE_INDEX
    test_packed_cells_and_lifetime();
    test_bounded_groups();
    test_pool_capacity_fallback();
#endif
    puts("surface queries: order, duplicates, independent scans and fresh publication preserved");
    return 0;
}
'''
        code = code.replace('/* DEFINITIONS */', definitions).replace('/* FUNCTIONS */', functions)
        with tempfile.TemporaryDirectory(prefix="surface-query-") as directory:
            path = Path(directory)
            (path / "test.c").write_text(code)
            for enabled in (1, 0):
                subprocess.run([
                    "cc", f"-DSTATIC_SURFACE_INDEX={enabled}", "-DDYNAMIC_SURFACE_INDEX=0", "-std=c99", "-O1",
                    "-Wall", "-Wextra", "-Werror", "-fsanitize=address,undefined",
                    "-fno-omit-frame-pointer", str(path / "test.c"), "-o", str(path / "test"),
                ], check=True)
                subprocess.run([str(path / "test")], check=True,
                               env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})


if __name__ == "__main__":
    unittest.main()
