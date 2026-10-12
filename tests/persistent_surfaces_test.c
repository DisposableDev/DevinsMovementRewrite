#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "persistent_surfaces.h"
#include "surface_load.h"
#include "surface_query.h"
#include "surface_terrains.h"
#include "math_util.h"

struct Object {
    Mat4 transform;
    struct {
        struct {
            Mat4 *throwMatrix;
            Vec3f scale;
        } gfx;
    } header;
    const void *behavior;
};

struct Asset {
    const char *name;
    const Collision *data;
    u32 words;
};
#include "mesh_assets.h"
#include "../actors/dorrie/collision.inc.c"
#include "../levels/bowser_3/falling_platform_1/collision.inc.c"
#include "../levels/bowser_3/falling_platform_2/collision.inc.c"
#include "../levels/bowser_3/falling_platform_3/collision.inc.c"
#include "../levels/bowser_3/falling_platform_4/collision.inc.c"
#include "../levels/bowser_3/falling_platform_5/collision.inc.c"
#include "../levels/bowser_3/falling_platform_6/collision.inc.c"
#include "../levels/bowser_3/falling_platform_7/collision.inc.c"
#include "../levels/bowser_3/falling_platform_8/collision.inc.c"
#include "../levels/bowser_3/falling_platform_9/collision.inc.c"
#include "../levels/bowser_3/falling_platform_10/collision.inc.c"

static struct Object objects[4];
static struct Object *o;
static const u8 bhvDddWarp[1];
static const u8 bhvDorrie[1];
#define segmented_to_virtual(pointer) (pointer)
#define O_POS_INDEX 0
#define O_FACE_ANGLE_INDEX 0
#define obj_build_transform_from_pos_and_angle(object, pos, angle) assert(!"unexpected transform build")
#define TIME_STOP_ACTIVE 1
#define PUPPYPRINT_GET_COLLISION_SNAPSHOT() u32 first = 0
#define profiler_collision_update(first) ((void) (first))
#define bzero(pointer, bytes) memset(pointer, 0, bytes)

SpatialPartitionCell gStaticSurfacePartition[NUM_CELLS][NUM_CELLS];
SpatialPartitionCell gDynamicSurfacePartition[NUM_CELLS][NUM_CELLS];
static union {
    u64 alignment;
    u8 bytes[1024 * 1024];
} pool;
void *gDynamicSurfacePool = pool.bytes;
void *gDynamicSurfacePoolEnd = pool.bytes;
void *gCurrStaticSurfacePoolEnd;
static u32 gSurfacesAllocated, gSurfaceNodesAllocated;
static u32 gNumStaticSurfaces, gNumStaticSurfaceNodes;
static s32 gTimeStopState;
static s16 gCollisionFlags;
static s32 sWaterCellMapReady;
static u8 sWaterCellMayContainWater[NUM_CELLS][NUM_CELLS];
static void clear_dynamic_surface_references(void) {
}
static s32 surface_is_passable_vanish(struct Surface *surface) {
    (void) surface;
    return FALSE;
}
#include "legacy_loader.inc"

static struct Surface reference[65536];
static u32 cellCounts[NUM_CELLS][NUM_CELLS][NUM_SPATIAL_PARTITIONS];
static u32 referenceFaces, referenceNodes;
static u32 comparisons;
static volatile f32 resultSink;

static void set_pose(struct Object *object, u32 frame) {
    f32 angle = sinf(frame * 0.17f) * 0.08f;
    memset(object->transform, 0, sizeof(Mat4));
    object->transform[0][0] = cosf(angle);
    object->transform[0][1] = sinf(angle);
    object->transform[1][0] = -sinf(angle);
    object->transform[1][1] = cosf(angle);
    object->transform[2][2] = 1;
    object->transform[3][0] = 0.37f;
    object->transform[3][1] = 150.61f + sinf(frame * 0.19f) * 2;
    object->transform[3][2] = -0.59f;
    object->transform[3][3] = 1;
    object->header.gfx.throwMatrix = &object->transform;
    object->header.gfx.scale[0] = object->header.gfx.scale[1] = object->header.gfx.scale[2] = 1;
}

static void reset(void) {
    gTimeStopState = 0;
    persistent_surfaces_reset();
    sClearAllCells = TRUE;
    clear_dynamic_surfaces();
}

enum {
    DORRIE_STATIONARY_PUBLICATION = 2,
    JRB_STATIONARY_PUBLICATION = 3,
    BOWSER_STATIONARY_PUBLICATION = 4
};

static s32 publish(struct Object *object, const Collision *data, s32 retained) {
    o = object;
    s32 handled = FALSE;
    if (retained == BOWSER_STATIONARY_PUBLICATION) {
        handled = persistent_surfaces_publish_bowser_platform(object, data, object->transform,
                                                              object->header.gfx.scale);
    } else if (retained == JRB_STATIONARY_PUBLICATION) {
        handled = persistent_surfaces_publish_jrb_ship(object, data, object->transform,
                                                       object->header.gfx.scale);
    } else if (retained == DORRIE_STATIONARY_PUBLICATION) {
        handled = persistent_surfaces_publish_dorrie(object, data, object->transform,
                                                     object->header.gfx.scale);
    } else if (retained && data[1] >= 64) {
        Mat4 scaled;
        mtxf_scale_vec3f(scaled, object->transform, object->header.gfx.scale);
        handled =
            persistent_surfaces_publish(object, data, scaled, object->behavior == &bhvDddWarp ? 5 : 0);
    }
    if (!handled) {
        TerrainData vertices[600];
        TerrainData *cursor = (TerrainData *) data + 1;
        transform_object_vertices(&cursor, vertices);
        while (*cursor != TERRAIN_LOAD_CONTINUE) {
            load_object_surfaces(&cursor, vertices, TRUE);
        }
    }
    assert((u8 *) gDynamicSurfacePoolEnd <= pool.bytes + sizeof(pool.bytes));
    return handled;
}

static void capture(void) {
    u32 count = 0;
    for (u32 z = 0; z < NUM_CELLS; z++) {
        for (u32 x = 0; x < NUM_CELLS; x++) {
            for (u32 p = 0; p < NUM_SPATIAL_PARTITIONS; p++) {
                u32 first = count;
                for (struct SurfaceNode *node = gDynamicSurfacePartition[z][x][p]; node != NULL;
                     node = node->next) {
                    assert(count < sizeof(reference) / sizeof(reference[0]));
                    reference[count++] = *node->surface;
                }
                cellCounts[z][x][p] = count - first;
            }
        }
    }
    referenceFaces = gSurfacesAllocated;
    referenceNodes = gSurfaceNodesAllocated;
}

static void compare_surface(const struct Surface *a, const struct Surface *b) {
    assert(a->type == b->type && a->force == b->force && a->flags == b->flags && a->room == b->room);
    assert(a->object == b->object && a->lowerY == b->lowerY && a->upperY == b->upperY);
    assert(memcmp(a->vertex1, b->vertex1, sizeof(Vec3t)) == 0);
    assert(memcmp(a->vertex2, b->vertex2, sizeof(Vec3t)) == 0);
    assert(memcmp(a->vertex3, b->vertex3, sizeof(Vec3t)) == 0);
    assert(memcmp(&a->normal, &b->normal, sizeof(struct Normal)) == 0);
    assert(memcmp(&a->originOffset, &b->originOffset, sizeof(f32)) == 0);
    comparisons++;
}

static void compare_grid(void) {
    u32 count = 0;
    for (u32 z = 0; z < NUM_CELLS; z++) {
        for (u32 x = 0; x < NUM_CELLS; x++) {
            for (u32 p = 0; p < NUM_SPATIAL_PARTITIONS; p++) {
                u32 first = count;
                for (struct SurfaceNode *node = gDynamicSurfacePartition[z][x][p]; node != NULL;
                     node = node->next) {
                    assert(count - first < cellCounts[z][x][p]);
                    compare_surface(node->surface, &reference[count++]);
                }
                assert(count - first == cellCounts[z][x][p]);
            }
        }
    }
    assert(gSurfacesAllocated == referenceFaces && gSurfaceNodesAllocated == referenceNodes);
}

static f32 queries(u32 frame) {
    f32 sum = 0;
    for (s32 i = 0; i < 16; i++) {
        s32 x = (i % 4) * 180 - 270;
        s32 z = (i / 4) * 500 - 750 + (s32) (frame % 13);
        f32 height = -11000;
        gCollisionFlags = 0;
        find_floor_from_list(
            surface_query_list(gDynamicSurfacePartition[(z + LEVEL_BOUNDARY_MAX) / CELL_SIZE]
                                                       [(x + LEVEL_BOUNDARY_MAX) / CELL_SIZE]
                                                       [SPATIAL_PARTITION_FLOORS]),
            x, 2000, z, &height);
        sum += height;
    }
    return sum;
}

static const struct Asset *ship_asset(void) {
    for (u32 i = 0; i < sizeof(assets) / sizeof(assets[0]); i++) {
        if (strcmp(assets[i].name, "jrb_seg7_collision_in_sunken_ship_3") == 0) {
            return &assets[i];
        }
    }
    assert(!"ship asset changed");
    return NULL;
}

static u32 target_ship_reference_capacity(void) {
    // Match the N64's 48-byte Surface, 8-byte node and aligned 16 KiB slot.
    u32 vertices = ship_asset()->data[1];
    u32 offset = vertices * 6;
    offset = ((offset + 7) & ~7u) + vertices * 6;
    offset = ((offset + 7) & ~7u) + 140 * 12;
    offset = ((offset + 7) & ~7u) + 140 * 48;
    offset = ((offset + 7) & ~7u) + 64 * 8;
    offset = (offset + 7) & ~7u;
    return (0x4000 - offset - 8) / 10;
}

static void test_assets(void) {
    const struct Asset *ship = ship_asset();
    for (u32 asset = 0; asset < sizeof(assets) / sizeof(assets[0]); asset++) {
        const Collision *data = assets[asset].data;
        if (data[1] > 200) {
            continue;
        }
        reset();
        for (u32 frame = 0; frame < 90; frame++) {
            set_pose(&objects[0], frame);
            // Exercise signed/nonuniform scale, shear, cell changes and room metadata.
            if (frame >= 30) {
                objects[0].header.gfx.scale[0] = -0.7f;
                objects[0].header.gfx.scale[1] = 1.3f;
                objects[0].transform[2][0] = 0.15f;
                objects[0].transform[3][0] += (frame - 30) * 31;
                objects[0].behavior = &bhvDddWarp;
            }
            if (frame >= 60) {
                // Frozen poses should skip all vertex and plane recomputation.
                set_pose(&objects[0], 60);
            }
            clear_dynamic_surfaces();
            publish(&objects[0], data, FALSE);
            capture();
            f32 expected = queries(frame);
            clear_dynamic_surfaces();
            s32 handled = publish(&objects[0], data, TRUE);
            compare_grid();
            assert(queries(frame) == expected);
            if (data == ship->data) {
                assert(handled);
                assert(referenceNodes <= target_ship_reference_capacity());
            }
        }
    }
}

static void test_order_and_lifetime(void) {
    const Collision *ship = ship_asset()->data;
    reset();
    for (u32 frame = 0; frame < 80; frame++) {
        for (u32 i = 0; i < 4; i++) {
            set_pose(&objects[i], frame);
        }
        clear_dynamic_surfaces();
        for (u32 i = 0; i < 4; i++) {
            publish(&objects[i], ship, FALSE);
        }
        capture();
        clear_dynamic_surfaces();
        publish(&objects[0], ship, FALSE);
        assert(publish(&objects[1], ship, TRUE));
        assert(publish(&objects[2], ship, TRUE));
        assert(!publish(&objects[3], ship, TRUE));
        compare_grid();
        // A second pose in the same epoch must not overwrite already published faces.
        objects[1].transform[3][1] += 100;
        publish(&objects[1], ship, FALSE);
        capture();
        clear_dynamic_surfaces();
        objects[1].transform[3][1] -= 100;
        publish(&objects[0], ship, FALSE);
        assert(publish(&objects[1], ship, TRUE));
        assert(publish(&objects[2], ship, TRUE));
        publish(&objects[3], ship, FALSE);
        objects[1].transform[3][1] += 100;
        assert(!publish(&objects[1], ship, TRUE));
        compare_grid();
        capture();
        persistent_surfaces_forget(&objects[1]);
        gTimeStopState = TIME_STOP_ACTIVE;
        clear_dynamic_surfaces();
        compare_grid();
        assert(persistent_surfaces_published_meshes() == 2);
        // Released bytes cannot be reused while the current grid still points into them.
        Mat4 scaled;
        mtxf_scale_vec3f(scaled, objects[3].transform, objects[3].header.gfx.scale);
        assert(!persistent_surfaces_publish(&objects[3], ship, scaled, 0));
        compare_grid();
        gTimeStopState = 0;
        clear_dynamic_surfaces();
        assert(persistent_surfaces_published_meshes() == 0);
        assert(publish(&objects[1], ship, TRUE));
    }
    // Reset also retires storage until clear, protecting an old area's still-linked grid.
    capture();
    persistent_surfaces_reset();
    compare_grid();
    clear_dynamic_surfaces();
    assert(persistent_surfaces_used_bytes() == 0);
    assert(publish(&objects[0], ship, TRUE));
}

static void test_fallback_and_mesh_switch(void) {
    const struct Asset *ship = ship_asset();
    Collision *copy = malloc(ship->words * sizeof(Collision));
    assert(copy != NULL);
    memcpy(copy, ship->data, ship->words * sizeof(Collision));
    reset();
    set_pose(&objects[0], 0);
    assert(publish(&objects[0], ship->data, TRUE));
    // A mesh switch in the same epoch preserves the old mesh and falls back.
    publish(&objects[0], copy, FALSE);
    capture();
    clear_dynamic_surfaces();
    publish(&objects[0], ship->data, TRUE);
    assert(!publish(&objects[0], copy, TRUE));
    compare_grid();
    clear_dynamic_surfaces();
    assert(publish(&objects[0], copy, TRUE));
    persistent_surfaces_reset();
    clear_dynamic_surfaces();
    // An area can reuse the same segmented source address with different vertices.
    copy[2] += 20;
    publish(&objects[0], copy, FALSE);
    capture();
    clear_dynamic_surfaces();
    assert(publish(&objects[0], copy, TRUE));
    compare_grid();
    clear_dynamic_surfaces();
    // Omitted/culled owners leave no cached faces in the current grid.
    assert(gSurfaceNodesAllocated == 0 && persistent_surfaces_published_meshes() == 0);
    // Moving far outside the grid must retain native clipping semantics.
    objects[0].transform[3][0] = 20000;
    publish(&objects[0], copy, FALSE);
    capture();
    clear_dynamic_surfaces();
    publish(&objects[0], copy, TRUE);
    compare_grid();
    reset();
    free(copy);
}

static void test_metadata_and_capacity(void) {
    const struct Asset *ship = ship_asset();
    Collision *copy = malloc(ship->words * sizeof(Collision));
    assert(copy != NULL);
    memcpy(copy, ship->data, ship->words * sizeof(Collision));
    u32 group = 2 + copy[1] * 3;
    const TerrainData types[] = {
        SURFACE_NEW_WATER,          SURFACE_NEW_WATER_BOTTOM, SURFACE_SWITCH,
        SURFACE_NO_CAM_COLLISION,   SURFACE_INTANGIBLE,       SURFACE_CAMERA_BOUNDARY,
        SURFACE_NO_CAM_COL_SLIPPERY
    };
    for (u32 type = 0; type < sizeof(types) / sizeof(types[0]); type++) {
        reset();
        copy[group] = types[type];
        for (u32 face = 0; face < 140; face++) {
            copy[group + 2 + face * 4 + 3] = face - 70;
        }
        set_pose(&objects[0], type);
        objects[0].behavior = bhvDddWarp;
        publish(&objects[0], copy, FALSE);
        capture();
        clear_dynamic_surfaces();
        assert(publish(&objects[0], copy, TRUE));
        compare_grid();
    }
    // A large footprint exceeds private cell/reference capacity. Its fallback must
    // preserve geometry already published by another owner, without partial links.
    reset();
    set_pose(&objects[0], 0);
    set_pose(&objects[1], 0);
    objects[0].header.gfx.scale[0] = objects[0].header.gfx.scale[2] = 4;
    publish(&objects[1], ship->data, FALSE);
    publish(&objects[0], copy, FALSE);
    capture();
    clear_dynamic_surfaces();
    assert(publish(&objects[1], ship->data, TRUE));
    assert(!publish(&objects[0], copy, TRUE));
    compare_grid();
    // A later fitting pose can recover from the same owner's failed publication.
    clear_dynamic_surfaces();
    set_pose(&objects[0], 0);
    assert(publish(&objects[0], copy, TRUE));
    reset();
    free(copy);
}

static double now(void) {
    struct timespec time;
    assert(clock_gettime(CLOCK_MONOTONIC, &time) == 0);
    return time.tv_sec + time.tv_nsec * 1e-9;
}

static void benchmark(void) {
    const Collision *ship = ship_asset()->data;
    for (u32 stationary = 0; stationary < 2; stationary++) {
        double best[2] = { 1000, 1000 };
        for (u32 round = 0; round < 7; round++) {
            for (u32 order = 0; order < 2; order++) {
                u32 retained = (round + order) % 2;
                reset();
                double start = now();
                for (u32 frame = 0; frame < 4000; frame++) {
                    set_pose(&objects[0], stationary ? 0 : frame);
                    clear_dynamic_surfaces();
                    publish(&objects[0], ship, retained);
                    resultSink = queries(frame);
                }
                double elapsed = now() - start;
                best[retained] = MIN(best[retained], elapsed);
            }
        }
        printf("140-face ship %s, 4000 clears/publications + 64000 native floor queries: "
               "original %.3f ms, retained %.3f ms (%.2fx)\n",
               stationary ? "stationary" : "rocking", best[0] * 1000, best[1] * 1000,
               best[0] / best[1]);
    }
}

static void test_dorrie_yaw_transform(void) {
    const Collision *meshes[] = {
        dorrie_seg6_collision_0600F644,
        dorrie_seg6_collision_0600FBB8,
    };
    struct Object *object = &objects[0];
    for (u32 frame = 0; frame < 64; frame++) {
        set_pose(object, frame);
        object->behavior = bhvDorrie;
        // Include fractional translation: rotation must round before translation.
        f32 angle = frame * 0.17f;
        memset(object->transform, 0, sizeof(Mat4));
        object->transform[0][0] = cosf(angle);
        object->transform[0][2] = -sinf(angle);
        object->transform[1][1] = 1.0f;
        object->transform[2][0] = sinf(angle);
        object->transform[2][2] = cosf(angle);
        object->transform[3][0] = -3000.37f + frame * 0.61f;
        object->transform[3][1] = -4500.61f + frame * 0.19f;
        object->transform[3][2] = 4000.59f - frame * 0.31f;
        object->transform[3][3] = 1.0f;
        if (frame >= 32) {
            // Tilted and vertically scaled poses must use the ordinary transform.
            if (frame & 1) {
                object->transform[1][0] = 0.05f;
            } else {
                object->header.gfx.scale[1] = 1.5f;
            }
        }
        for (u32 mesh = 0; mesh < sizeof(meshes) / sizeof(meshes[0]); mesh++) {
            reset();
            // The retained publisher supplies the original full-matrix reference.
            assert(publish(object, meshes[mesh], TRUE));
            capture();
            clear_dynamic_surfaces();
            publish(object, meshes[mesh], FALSE);
            compare_grid();
        }
    }
    object->behavior = NULL;
}

static void test_jrb_stationary(void) {
    reset();
    set_pose(&objects[0], 0);
    set_pose(&objects[1], 0);
    const Collision *meshes[2] = { jrb_seg7_collision_in_sunken_ship,
                                   jrb_seg7_collision_in_sunken_ship_2 };
    for (u32 frame = 0; frame < 32; frame++) {
        if (frame == 5) {
            objects[0].transform[3][0] += 1024.0f;
        }
        if (frame == 9) {
            objects[1].header.gfx.scale[1] = 0.75f;
        }
        if (frame == 13) {
            persistent_surfaces_forget(&objects[0]);
        }
        if (frame == 17) {
            persistent_surfaces_reset();
        }
        if (frame == 21) {
            meshes[0] = meshes[1];
        }
        clear_dynamic_surfaces();
        for (u32 i = 0; i < 2; i++) {
            publish(&objects[i], meshes[i], FALSE);
        }
        capture();
        f32 height = queries(frame);
        clear_dynamic_surfaces();
        for (u32 i = 0; i < 2; i++) {
            assert(publish(&objects[i], meshes[i], JRB_STATIONARY_PUBLICATION));
        }
        compare_grid();
        assert(queries(frame) == height);
    }
    // The same-frame fallback cannot corrupt a retained mesh still in the grid.
    assert(!publish(&objects[0], meshes[0], JRB_STATIONARY_PUBLICATION));
}

static void test_bowser_top_floors(void) {
    const Collision *meshes[] = { bowser_3_seg7_collision_07004B94, bowser_3_seg7_collision_07004C18,
                                  bowser_3_seg7_collision_07004C9C, bowser_3_seg7_collision_07004D20,
                                  bowser_3_seg7_collision_07004DA4, bowser_3_seg7_collision_07004E28,
                                  bowser_3_seg7_collision_07004EAC, bowser_3_seg7_collision_07004F30,
                                  bowser_3_seg7_collision_07004FB4, bowser_3_seg7_collision_07005038 };
    for (u32 mesh = 0; mesh < sizeof(meshes) / sizeof(meshes[0]); mesh++) {
        reset();
        struct Object *object = &objects[0];
        set_pose(object, 0);
        memset(object->transform, 0, sizeof(Mat4));
        for (s32 i = 0; i < 4; i++) {
            object->transform[i][i] = 1;
        }
        publish(object, meshes[mesh], FALSE);
        // Capture native floors in their original cell/list order.
        u32 floorCount = 0;
        for (u32 z = 0; z < NUM_CELLS; z++) {
            for (u32 x = 0; x < NUM_CELLS; x++) {
                u32 start = floorCount;
                for (struct SurfaceNode *node =
                         gDynamicSurfacePartition[z][x][SPATIAL_PARTITION_FLOORS];
                     node != NULL; node = node->next) {
                    assert(node->surface->vertex1[1] == 307);
                    reference[floorCount++] = *node->surface;
                }
                cellCounts[z][x][SPATIAL_PARTITION_FLOORS] = floorCount - start;
            }
        }
        assert(floorCount > 0);
        f32 expectedHeight = queries(0);
        clear_dynamic_surfaces();
        TerrainData vertices[600];
        TerrainData *cursor = (TerrainData *) meshes[mesh] + 1;
        transform_object_vertices(&cursor, vertices);
        while (*cursor != TERRAIN_LOAD_CONTINUE) {
            load_bowser_platform_top(&cursor, vertices);
        }
        assert(gSurfacesAllocated == 2);
        u32 index = 0;
        for (u32 z = 0; z < NUM_CELLS; z++) {
            for (u32 x = 0; x < NUM_CELLS; x++) {
                assert(gDynamicSurfacePartition[z][x][SPATIAL_PARTITION_WALLS] == NULL);
                assert(gDynamicSurfacePartition[z][x][SPATIAL_PARTITION_CEILS] == NULL);
                u32 start = index;
                for (struct SurfaceNode *node =
                         gDynamicSurfacePartition[z][x][SPATIAL_PARTITION_FLOORS];
                     node != NULL; node = node->next) {
                    assert(index < floorCount);
                    compare_surface(node->surface, &reference[index++]);
                }
                assert(index - start == cellCounts[z][x][SPATIAL_PARTITION_FLOORS]);
            }
        }
        assert(index == floorCount && queries(0) == expectedHeight);
        // Switching back to the full loader restores side and underside faces.
        clear_dynamic_surfaces();
        publish(object, meshes[mesh], FALSE);
        assert(gSurfacesAllocated == 12);
    }
}

static void test_bowser_stationary(void) {
    const Collision *meshes[] = { bowser_3_seg7_collision_07004B94, bowser_3_seg7_collision_07004C18,
                                  bowser_3_seg7_collision_07004B94 };
    reset();
    for (u32 i = 0; i < 3; i++) {
        set_pose(&objects[i], 0);
    }
    for (u32 frame = 0; frame < 16; frame++) {
        if (frame == 8) {
            persistent_surfaces_forget(&objects[0]);
        }
        if (frame >= 8) {
            objects[0].transform[3][1] -= 64.0f;
        }
        clear_dynamic_surfaces();
        for (u32 i = 0; i < 3; i++) {
            publish(&objects[i], meshes[i], FALSE);
        }
        capture();
        f32 height = queries(frame);
        clear_dynamic_surfaces();
        for (u32 i = 0; i < 3; i++) {
            publish(&objects[i], meshes[i],
                    i == 0 && frame >= 8 ? FALSE : BOWSER_STATIONARY_PUBLICATION);
        }
        compare_grid();
        assert(queries(frame) == height);
    }
}

static void test_dorrie_stationary(void) {
    const Collision *meshes[] = {
        dorrie_seg6_collision_0600F644,
        dorrie_seg6_collision_0600FBB8,
    };
    for (u32 mesh = 0; mesh < 2; mesh++) {
        for (u32 pose = 0; pose < 16; pose++) {
            reset();
            struct Object *object = &objects[0];
            set_pose(object, pose);
            object->behavior = bhvDorrie;
            const Collision *data = meshes[mesh];
            for (u32 frame = 0; frame < 16; frame++) {
                s32 expected = TRUE;
                switch (frame) {
                    case 0:
                        expected = FALSE;
                        break;
                    case 3:
                        object->transform[3][1] += 0.25f;
                        expected = FALSE;
                        break;
                    case 5:
                        object->header.gfx.scale[1] = 1.5f;
                        expected = FALSE;
                        break;
                    case 7:
                        object->transform[3][0] += 1024.0f;
                        expected = FALSE;
                        break;
                    case 9:
                        data = meshes[1 - mesh];
                        expected = FALSE;
                        break;
                    case 11:
                        objects[1] = *object;
                        object = &objects[1];
                        expected = FALSE;
                        break;
                    case 13:
                        persistent_surfaces_forget(object);
                        expected = FALSE;
                        break;
                    case 15:
                        persistent_surfaces_reset();
                        expected = FALSE;
                        break;
                }
                clear_dynamic_surfaces();
                publish(object, data, FALSE);
                capture();
                f32 heightSum = queries(frame);
                clear_dynamic_surfaces();
                assert(publish(object, data, DORRIE_STATIONARY_PUBLICATION) == expected);
                compare_grid();
                assert(queries(frame) == heightSum);
            }
        }
    }

    // Duplicate loads preserve the already-published pose, including after forget.
    reset();
    set_pose(&objects[0], 0);
    objects[0].behavior = bhvDorrie;
    const Collision *data = meshes[0];
    assert(!publish(&objects[0], data, DORRIE_STATIONARY_PUBLICATION));
    clear_dynamic_surfaces();
    assert(publish(&objects[0], data, DORRIE_STATIONARY_PUBLICATION));
    capture();
    gTimeStopState = TIME_STOP_ACTIVE;
    clear_dynamic_surfaces();
    compare_grid();
    persistent_surfaces_forget(&objects[0]);
    compare_grid();
    gTimeStopState = 0;
    clear_dynamic_surfaces();
    publish(&objects[0], data, FALSE);
    publish(&objects[0], data, FALSE);
    capture();
    clear_dynamic_surfaces();
    assert(!publish(&objects[0], data, DORRIE_STATIONARY_PUBLICATION));
    assert(publish(&objects[0], data, DORRIE_STATIONARY_PUBLICATION));
    compare_grid();
    clear_dynamic_surfaces();
    publish(&objects[0], data, FALSE);
    publish(&objects[0], data, FALSE);
    publish(&objects[0], data, FALSE);
    capture();
    persistent_surfaces_forget(&objects[0]);
    clear_dynamic_surfaces();
    assert(!publish(&objects[0], data, DORRIE_STATIONARY_PUBLICATION));
    assert(publish(&objects[0], data, DORRIE_STATIONARY_PUBLICATION));
    assert(!publish(&objects[0], data, DORRIE_STATIONARY_PUBLICATION));
    compare_grid();

    // Exhausted storage leaves Dorrie's ordinary fallback exact.
    reset();
    for (u32 i = 0; i < 3; i++) {
        set_pose(&objects[i], i);
        objects[i].behavior = bhvDorrie;
    }
    assert(publish(&objects[0], data, TRUE));
    assert(publish(&objects[1], data, TRUE));
    clear_dynamic_surfaces();
    publish(&objects[2], data, FALSE);
    capture();
    clear_dynamic_surfaces();
    assert(!publish(&objects[2], data, DORRIE_STATIONARY_PUBLICATION));
    compare_grid();
    clear_dynamic_surfaces();
    assert(!publish(&objects[2], data, DORRIE_STATIONARY_PUBLICATION));
    compare_grid();
}

int main(int argc, char **argv) {
    test_assets();
    test_order_and_lifetime();
    test_fallback_and_mesh_switch();
    test_metadata_and_capacity();
    test_dorrie_yaw_transform();
    test_dorrie_stationary();
    test_jrb_stationary();
    test_bowser_stationary();
    test_bowser_top_floors();
    printf("Persistent geometry, exact grid order, lifecycle and native queries passed: %u faces\n",
           comparisons);
    if (argc > 1 && strcmp(argv[1], "benchmark") == 0) {
        benchmark();
    }
    return 0;
}
