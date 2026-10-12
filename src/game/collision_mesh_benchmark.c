#include <ultra64.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "collision_mesh_benchmark.h"
#include "area.h"
#include "level_update.h"
#include "engine/collision_mesh.h"
#include "engine/surface_collision.h"
#include "engine/surface_load.h"
#include "engine/surface_query.h"
#include "mario.h"
#include "memory.h"
#include "puppyprint.h"
#include "surface_terrains.h"

#ifdef PUPPYPRINT_DEBUG

#define MESH_BENCH_PROBES 64
#define MESH_BENCH_ROUNDS 9

enum MeshBenchmarkBackend { MESH_BENCH_STOCK, MESH_BENCH_CURRENT, MESH_BENCH_TREE, MESH_BENCH_COUNT };

static struct {
    u32 buildTicks, updateTicks, bytes, faces;
    u32 best[MESH_BENCH_COUNT], worst[MESH_BENCH_COUNT];
    u32 stockDifferences, treeDifferences;
    s32 ready;
} sMeshBenchmark;

// The level loader already trusts these compiled assets. Stop at the triangle
// terminator; macro objects, water boxes and room ownership remain outside this trial.
static u32 mesh_benchmark_asset_words(const TerrainData *data, u32 stride) {
    if (data == NULL || data[0] != TERRAIN_LOAD_VERTICES || data[1] <= 0) {
        return 0;
    }
    u32 word = 2 + 3 * data[1];
    while (data[word] != TERRAIN_LOAD_CONTINUE) {
        if (data[word] < 0 || (data[word] >= 0x40 && data[word] < 0x65) || data[word + 1] < 0) {
            return 0;
        }
        word += 2 + stride * data[word + 1];
    }
    return word + 1;
}

// Stock first-floor policy and integer edge tests from 89e86908, adapted only
// to this project's flags and static partition. This is a query reference,
// not a claim to reproduce the complete vanilla game or its loader cost.
static f32 mesh_benchmark_stock_floor(f32 xPos, f32 yPos, f32 zPos) {
    s32 x = xPos, y = yPos, z = zPos;
    if (is_outside_level_bounds(x, z)) {
        return FLOOR_LOWER_LIMIT;
    }
    struct SurfaceNode *node =
        gStaticSurfacePartition[GET_CELL_COORD(z)][GET_CELL_COORD(x)][SPATIAL_PARTITION_FLOORS];
    while (node != NULL) {
        struct Surface *surface = node->surface;
        node = node->next;
        if (surface->object != NULL || surface->type == SURFACE_CAMERA_BOUNDARY
            || surface->type == SURFACE_INTANGIBLE) {
            continue;
        }
        const s16 *a = surface->vertex1, *b = surface->vertex2, *c = surface->vertex3;
        if ((a[2] - z) * (b[0] - a[0]) - (a[0] - x) * (b[2] - a[2]) < 0
            || (b[2] - z) * (c[0] - b[0]) - (b[0] - x) * (c[2] - b[2]) < 0
            || (c[2] - z) * (a[0] - c[0]) - (c[0] - x) * (a[2] - c[2]) < 0) {
            continue;
        }
        f32 height = -(x * surface->normal.x + z * surface->normal.z + surface->originOffset)
                     / surface->normal.y;
        if (height <= y + 78.0f) {
            return height;
        }
    }
    return FLOOR_LOWER_LIMIT;
}

// Current precise scan, restricted to the same terrain mesh as the trial.
// Appended object geometry is excluded in both references rather than letting
// the prototype win by searching less of the scene.
static f32 mesh_benchmark_current_floor(const Vec3f position) {
    f32 x = position[0], z = position[2], limit = position[1] + FIND_FLOOR_BUFFER;
    f32 highest = FLOOR_LOWER_LIMIT;
    if (is_outside_level_bounds(x, z)) {
        return highest;
    }
    struct SurfaceQueryIterator iterator = surface_query_cell(
        GET_CELL_COORD(x), GET_CELL_COORD(z), SPATIAL_PARTITION_FLOORS, SURFACE_QUERY_STATIC);
    Vec3f minimum = { x - 1, -32768, z - 1 }, maximum = { x + 1, limit, z + 1 };
    for (struct Surface *surface = surface_query_next_in_bounds(&iterator, minimum, maximum);
         surface != NULL; surface = surface_query_next_in_bounds(&iterator, minimum, maximum)) {
        if (surface->upperY < highest) {
            break;
        }
        if (surface->object != NULL || limit < surface->lowerY
            || surface->type == SURFACE_CAMERA_BOUNDARY || surface->type == SURFACE_INTANGIBLE) {
            continue;
        }
        const s16 *a = surface->vertex1, *b = surface->vertex2, *c = surface->vertex3;
        if ((a[2] - z) * (b[0] - a[0]) - (a[0] - x) * (b[2] - a[2]) < 0
            || (b[2] - z) * (c[0] - b[0]) - (b[0] - x) * (c[2] - b[2]) < 0
            || (c[2] - z) * (a[0] - c[0]) - (c[0] - x) * (a[2] - c[2]) < 0) {
            continue;
        }
        f32 height = surface->vertex1[1]
                     - ((x - surface->vertex1[0]) * surface->normal.x
                        + (z - surface->vertex1[2]) * surface->normal.z)
                           / surface->normal.y;
        if (height <= limit && height > highest) {
            highest = height;
        }
    }
    return highest;
}

static f32 mesh_benchmark_floor(s32 backend, const struct CollisionMeshInstance *instance,
                                const Vec3f position) {
    if (backend == MESH_BENCH_STOCK) {
        return mesh_benchmark_stock_floor(position[0], position[1], position[2]);
    }
    if (backend == MESH_BENCH_CURRENT) {
        return mesh_benchmark_current_floor(position);
    }
    struct CollisionMeshHit hit;
    Vec3f start = { position[0], position[1] + FIND_FLOOR_BUFFER, position[2] };
    Vec3f end = { position[0], FLOOR_LOWER_LIMIT, position[2] };
    return collision_mesh_raycast(instance, start, end, COLLISION_MESH_FLOORS, &hit)
               ? hit.position[1]
               : FLOOR_LOWER_LIMIT;
}

void collision_mesh_benchmark_run(void) {
    memset(&sMeshBenchmark, 0, sizeof(sMeshBenchmark));
    if (gCurrentArea == NULL || gMarioState == NULL) {
        return;
    }
#ifdef ALL_SURFACES_HAVE_FORCE
    u32 stride = 4;
#else
    u32 stride = 3;
#endif
    const TerrainData *data = gCurrentArea->terrainData;
    u32 words = mesh_benchmark_asset_words(data, stride);
    u32 bytes = collision_mesh_storage_size(data, words, stride);
    if (bytes == 0) {
        sMeshBenchmark.ready = -1;
        return;
    }
    void *storage = main_pool_alloc(bytes, MEMORY_POOL_LEFT);
    if (storage == NULL) {
        sMeshBenchmark.ready = -2;
        return;
    }
    struct CollisionMesh mesh;
    struct CollisionMeshInstance instance;
    Mat4 transform = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };
    u32 begin = osGetCount();
    if (!collision_mesh_build(&mesh, data, words, stride, storage, bytes)) {
        main_pool_free(storage);
        sMeshBenchmark.ready = -1;
        return;
    }
    sMeshBenchmark.buildTicks = osGetCount() - begin;
    collision_mesh_instance_update(&instance, &mesh, transform);
    sMeshBenchmark.bytes = bytes + sizeof(mesh) + sizeof(instance);
    sMeshBenchmark.faces = mesh.triangleCount;
    Vec3f probes[MESH_BENCH_PROBES];
    for (s32 i = 0; i < MESH_BENCH_PROBES; i++) {
        // Fractional, repeatable local patch; all backends receive identical probes.
        probes[i][0] = gMarioState->pos[0] + (i % 8 - 3.5f) * 27.25f;
        probes[i][1] = gMarioState->pos[1];
        probes[i][2] = gMarioState->pos[2] + (i / 8 - 3.5f) * 27.25f;
    }
    f32 heights[MESH_BENCH_COUNT][MESH_BENCH_PROBES];
    for (s32 backend = 0; backend < MESH_BENCH_COUNT; backend++) {
        sMeshBenchmark.best[backend] = ~0u;
    }
    for (s32 round = 0; round < MESH_BENCH_ROUNDS; round++) {
        for (s32 order = 0; order < MESH_BENCH_COUNT; order++) {
            s32 backend = (round + order) % MESH_BENCH_COUNT;
            begin = osGetCount();
            for (s32 i = 0; i < MESH_BENCH_PROBES; i++) {
                heights[backend][i] = mesh_benchmark_floor(backend, &instance, probes[i]);
            }
            u32 ticks = osGetCount() - begin;
            if (ticks < sMeshBenchmark.best[backend]) {
                sMeshBenchmark.best[backend] = ticks;
            }
            if (ticks > sMeshBenchmark.worst[backend]) {
                sMeshBenchmark.worst[backend] = ticks;
            }
        }
    }
    for (s32 i = 0; i < MESH_BENCH_PROBES; i++) {
        if (ABS2(heights[MESH_BENCH_STOCK][i] - heights[MESH_BENCH_CURRENT][i]) > 0.1f) {
            sMeshBenchmark.stockDifferences++;
        }
        if (ABS2(heights[MESH_BENCH_TREE][i] - heights[MESH_BENCH_CURRENT][i]) > 0.1f) {
            sMeshBenchmark.treeDifferences++;
        }
    }
    begin = osGetCount();
    for (s32 i = 0; i < 256; i++) {
        transform[3][0] = i * 0.125f;
        transform[3][1] = i * 0.25f;
        collision_mesh_instance_update(&instance, &mesh, transform);
    }
    sMeshBenchmark.updateTicks = (osGetCount() - begin) / 256;
    main_pool_free(storage);
    sMeshBenchmark.ready = 1;
}

void collision_mesh_benchmark_reset(void) {
    sMeshBenchmark.ready = 0;
}

int collision_mesh_benchmark_render(void) {
    char text[400];
    if (sMeshBenchmark.ready <= 0) {
        const char *status;
        if (sMeshBenchmark.ready == -2) {
            status = "Mesh trial: insufficient temporary memory";
        } else if (sMeshBenchmark.ready == -1) {
            status = "Mesh trial: unsupported collision stream";
        } else {
            status = "Press Z: compare collision backends here";
        }
        print_small_text_light(16, 120, status, PRINT_TEXT_ALIGN_LEFT, PRINT_ALL, FONT_OUTLINE);
        return 0;
    }
    sprintf(text,
            "64 terrain floors: best / worst us\nStock: %u / %u\nCurrent: %u / %u\nMesh: %u / %u\n"
            "Build: %uus  Update: %uus\nMesh: %u faces, %u bytes\nDifferent: stock %u, mesh %u\nZ "
            "reruns here; B closes trial",
            (u32) OS_CYCLES_TO_USEC(sMeshBenchmark.best[0]),
            (u32) OS_CYCLES_TO_USEC(sMeshBenchmark.worst[0]),
            (u32) OS_CYCLES_TO_USEC(sMeshBenchmark.best[1]),
            (u32) OS_CYCLES_TO_USEC(sMeshBenchmark.worst[1]),
            (u32) OS_CYCLES_TO_USEC(sMeshBenchmark.best[2]),
            (u32) OS_CYCLES_TO_USEC(sMeshBenchmark.worst[2]),
            (u32) OS_CYCLES_TO_USEC(sMeshBenchmark.buildTicks),
            (u32) OS_CYCLES_TO_USEC(sMeshBenchmark.updateTicks), sMeshBenchmark.faces,
            sMeshBenchmark.bytes, sMeshBenchmark.stockDifferences, sMeshBenchmark.treeDifferences);
    print_small_text_light(16, 60, text, PRINT_TEXT_ALIGN_LEFT, PRINT_ALL, FONT_OUTLINE);
    return 1;
}

#endif
