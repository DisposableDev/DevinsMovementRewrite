#include <PR/ultratypes.h>
#include <string.h>

#include "sm64.h"
#include "game/collision_profile.h"
#include "game/ingame_menu.h"
#include "graph_node.h"
#include "behavior_script.h"
#include "behavior_data.h"
#include "debugger/assert.h"
#include "game/memory.h"
#include "game/object_helpers.h"
#include "game/macro_special_objects.h"
#include "surface_collision.h"
#include "math_util.h"
#include "game/mario.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "game/object_list_processor.h"
#include "game/object_platform_motion.h"
#include "surface_load.h"
#include "persistent_surfaces.h"
#include "surface_query.h"
#include "game/puppyprint.h"
#include "game/debug.h"

#include "config.h"

/**
 * Partitions for course and object surfaces. The arrays represent
 * the 16x16 cells that each level is split into.
 */
SpatialPartitionCell gStaticSurfacePartition[NUM_CELLS][NUM_CELLS];
SpatialPartitionCell gDynamicSurfacePartition[NUM_CELLS][NUM_CELLS];
u32 gStaticSurfaceGeneration;
#if STATIC_WALL_HEIGHT_GROUPS
struct StaticWallHeightIndex gStaticWallHeightIndex;
#endif
#if STATIC_SURFACE_INDEX
struct StaticSurfaceIndex gStaticSurfaceIndex;
#endif
#if DYNAMIC_SURFACE_INDEX
struct DynamicSurfaceIndex gDynamicSurfaceIndex = { .generation = 1 };
static union {
    struct Surface *alignment;
    u8 bytes[DYNAMIC_SURFACE_INDEX_BYTES];
} sDynamicSurfaceIndexData;
#endif
static u8 sWaterCellMayContainWater[NUM_CELLS][NUM_CELLS];
static s32 sWaterCellMapReady;

s32 water_cell_may_contain_water(s32 x, s32 z) {
    // Terrain loading and out-of-bounds queries keep the original lookup path.
    if (!sWaterCellMapReady || is_outside_level_bounds(x, z)) {
        return TRUE;
    }

    return sWaterCellMayContainWater[GET_CELL_COORD(z)][GET_CELL_COORD(x)];
}

static void build_water_cell_map(void) {
    TerrainData *region = gEnvironmentRegions;

    for (s32 cellZ = 0; cellZ < NUM_CELLS; cellZ++) {
        for (s32 cellX = 0; cellX < NUM_CELLS; cellX++) {
            sWaterCellMayContainWater[cellZ][cellX] =
                gStaticSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_WATER] != NULL;
        }
    }

    if (region != NULL) {
        s32 numRegions = *region++;

        for (s32 i = 0; i < numRegions; i++, region += 6) {
            // Water box heights can move, but their X/Z bounds do not.
            if (region[0] >= 50) {
                continue;
            }

            for (s32 cellZ = 0; cellZ < NUM_CELLS; cellZ++) {
                s32 minZ = cellZ * CELL_SIZE - LEVEL_BOUNDARY_MAX;
                s32 maxZ = minZ + CELL_SIZE - 1;
                if (region[4] < minZ || region[2] > maxZ) {
                    continue;
                }

                for (s32 cellX = 0; cellX < NUM_CELLS; cellX++) {
                    s32 minX = cellX * CELL_SIZE - LEVEL_BOUNDARY_MAX;
                    s32 maxX = minX + CELL_SIZE - 1;
                    // Include box edges: find_water_level and the shadow lookup
                    // use different inclusive/strict edge rules.
                    if (region[3] >= minX && region[1] <= maxX) {
                        sWaterCellMayContainWater[cellZ][cellX] = TRUE;
                    }
                }
            }
        }
    }

    sWaterCellMapReady = TRUE;
}

struct CellCoords {
    u8 z;
    u8 x;
    u8 partition;
};
// Sixteen slots cover a 4x4-cell footprint. Other cells reuse these slots
// and fall back to the ordered scan when their list is not remembered.
struct DynamicWallTail {
    struct SurfaceNode **list;
    struct SurfaceNode *tail;
};
static struct DynamicWallTail sDynamicWallTails[16];

// Each occupied cell can have a separate list for every surface partition.
struct CellCoords sCellsUsed[NUM_CELLS * NUM_SPATIAL_PARTITIONS];
u16 sNumCellsUsed;
u8 sClearAllCells;

/**
 * Pools of data that can contain either surface nodes or surfaces.
 * The static surface pool is resized to be exactly the amount of memory needed for the level geometry.
 * The dynamic surface pool is set at a fixed length and cleared every frame.
 */
void *gCurrStaticSurfacePool;
void *gDynamicSurfacePool;

/**
 * The end of the data currently allocated to the surface pools.
 */
void *gCurrStaticSurfacePoolEnd;
void *gDynamicSurfacePoolEnd;

/**
 * The amount of data currently allocated to static surfaces.
 */
u32 gTotalStaticSurfaceData;

#if STATIC_SURFACE_INDEX || DYNAMIC_SURFACE_INDEX
static ALWAYS_INLINE void extend_surface_query_bounds(struct SurfaceQueryBounds *bounds,
                                                      struct Surface *surface, s32 initialize) {
    for (s32 axis = 0; axis < 3; axis++) {
        s16 low = axis == 1 ? surface->lowerY
                            : MIN(surface->vertex1[axis],
                                  MIN(surface->vertex2[axis], surface->vertex3[axis]));
        s16 high = axis == 1 ? surface->upperY
                             : MAX(surface->vertex1[axis],
                                   MAX(surface->vertex2[axis], surface->vertex3[axis]));
        if (initialize) {
            bounds->minimum[axis] = low;
            bounds->maximum[axis] = high;
        } else {
            bounds->minimum[axis] = MIN(bounds->minimum[axis], low);
            bounds->maximum[axis] = MAX(bounds->maximum[axis], high);
        }
    }
}
#endif

#if DYNAMIC_SURFACE_INDEX
// Publication makes every previously obtained cursor obsolete. No index storage
// is recycled between publications, including when a different cell is queried.
void invalidate_dynamic_surface_index(void) {
    if (!gDynamicSurfaceIndex.active) {
        return;
    }
    gDynamicSurfaceIndex.active = FALSE;
    gDynamicSurfaceIndex.used = 0;
    gDynamicSurfaceIndex.generation++;
    if (gDynamicSurfaceIndex.generation == 0) {
        bzero(gDynamicSurfaceIndex.cells, sizeof(gDynamicSurfaceIndex.cells));
        gDynamicSurfaceIndex.generation = 1;
    }
}

void build_dynamic_surface_index_cell(struct DynamicSurfaceIndexCell *cell, struct SurfaceNode *head) {
    u32 start = ALIGN(gDynamicSurfaceIndex.used, sizeof(struct Surface *));
    u32 capacity = (DYNAMIC_SURFACE_INDEX_BYTES - start) / sizeof(struct Surface *);
    u32 count = 0;
    // Cache failed/sparse attempts until publication; never consume arena space.
    cell->count = -1;
    for (struct SurfaceNode *node = head; node != NULL; node = node->next) {
        if (count == capacity) {
            return;
        }
        count++;
    }
    if (count < STATIC_SURFACE_GROUP_SIZE) {
        return;
    }
    u32 groupCount = (count + STATIC_SURFACE_GROUP_SIZE - 1) / STATIC_SURFACE_GROUP_SIZE;
    u32 bytes = count * sizeof(struct Surface *) + groupCount * sizeof(struct SurfaceQueryBounds);
    if (bytes > DYNAMIC_SURFACE_INDEX_BYTES - start) {
        return;
    }
    struct Surface **surfaces = (struct Surface **) (sDynamicSurfaceIndexData.bytes + start);
    struct SurfaceQueryBounds *bounds = (struct SurfaceQueryBounds *) (surfaces + count);
    u32 offset = 0;
    for (struct SurfaceNode *node = head; node != NULL; node = node->next) {
        surfaces[offset] = node->surface;
        extend_surface_query_bounds(&bounds[offset / STATIC_SURFACE_GROUP_SIZE], node->surface,
                                    offset % STATIC_SURFACE_GROUP_SIZE == 0);
        offset++;
    }
    gDynamicSurfaceIndex.used = start + bytes;
    cell->surfaces = surfaces;
    cell->bounds = bounds;
    cell->count = count;
}
#endif

#if STATIC_SURFACE_INDEX
// Append the index to the terrain's own pool before it is shrunk. Retain the
// original lists for diagnostics, late static additions and low-memory fallback.
static void build_static_surface_index(u32 poolCapacity) {
    u32 used = (uintptr_t) gCurrStaticSurfacePoolEnd - (uintptr_t) gCurrStaticSurfacePool;
    u32 offsetBytes = ALIGN((NUM_CELLS * NUM_CELLS * NUM_SPATIAL_PARTITIONS + 1) * sizeof(u32),
                            sizeof(struct Surface *));
    gStaticSurfaceIndex.offsets = NULL;
    if (used > poolCapacity || offsetBytes > poolCapacity - used) {
        return;
    }

    u32 capacity = (poolCapacity - used - offsetBytes) / sizeof(struct Surface *);
    u32 count = 0;
    for (s32 z = 0; z < NUM_CELLS; z++) {
        for (s32 x = 0; x < NUM_CELLS; x++) {
            for (s32 partition = 0; partition < NUM_SPATIAL_PARTITIONS; partition++) {
                for (struct SurfaceNode *node = gStaticSurfacePartition[z][x][partition]; node != NULL;
                     node = node->next) {
                    if (count == capacity) {
                        return;
                    }
                    count++;
                }
            }
        }
    }

    u32 groupCount = (count + STATIC_SURFACE_GROUP_SIZE - 1) / STATIC_SURFACE_GROUP_SIZE;
    u32 boundsBytes = groupCount * sizeof(struct SurfaceQueryBounds);
    if (boundsBytes > poolCapacity - used - offsetBytes - count * sizeof(struct Surface *)) {
        return;
    }

    u32 *offsets = gCurrStaticSurfacePoolEnd;
    struct Surface **surfaces = (struct Surface **) ((u8 *) offsets + offsetBytes);
    struct SurfaceQueryBounds *bounds = (struct SurfaceQueryBounds *) (surfaces + count);
    u32 cell = 0;
    count = 0;
    for (s32 z = 0; z < NUM_CELLS; z++) {
        for (s32 x = 0; x < NUM_CELLS; x++) {
            for (s32 partition = 0; partition < NUM_SPATIAL_PARTITIONS; partition++) {
                offsets[cell++] = count;
                for (struct SurfaceNode *node = gStaticSurfacePartition[z][x][partition]; node != NULL;
                     node = node->next) {
                    struct Surface *surface = node->surface;
                    struct SurfaceQueryBounds *group = &bounds[count / STATIC_SURFACE_GROUP_SIZE];
                    extend_surface_query_bounds(group, surface, count % STATIC_SURFACE_GROUP_SIZE == 0);
                    surfaces[count++] = surface;
                }
            }
        }
    }
    offsets[cell] = count;
    gCurrStaticSurfacePoolEnd = (u8 *) bounds + boundsBytes;
    // Publish only after cell offsets, references and group bounds are complete.
    gStaticSurfaceIndex.surfaces = surfaces;
    gStaticSurfaceIndex.bounds = bounds;
    gStaticSurfaceIndex.generation = gStaticSurfaceGeneration;
    gStaticSurfaceIndex.offsets = offsets;
}
#endif

// Publication is cleared before the terrain pool can be reused. Late static
// additions keep live linked scans until the next complete terrain load.
void invalidate_static_wall_height_index(void) {
#if STATIC_WALL_HEIGHT_GROUPS
    gStaticWallHeightIndex.cells = NULL;
#endif
}

#if STATIC_WALL_HEIGHT_GROUPS
// Lists whose groups all overlap at one height seldom help movement there.
// Index only lists with disjoint group ranges, preserving the cheap live scan
// for dense single-height geometry as well as short lists.
static u32 static_wall_height_list_group_count(struct SurfaceNode *node) {
    u32 nodes = 0, groups = 0;
    s32 minimumUpperY = 32767, maximumLowerY = -32768;
    while (node != NULL) {
        s32 lowerY = 32767, upperY = -32768;
        for (s32 i = 0; i < STATIC_WALL_HEIGHT_GROUP_SIZE && node != NULL; i++) {
            lowerY = MIN(lowerY, node->surface->lowerY);
            upperY = MAX(upperY, node->surface->upperY);
            nodes++;
            node = node->next;
        }
        minimumUpperY = MIN(minimumUpperY, upperY);
        maximumLowerY = MAX(maximumLowerY, lowerY);
        groups++;
    }
    return nodes >= STATIC_WALL_HEIGHT_MIN_NODES && maximumLowerY > minimumUpperY ? groups : 0;
}

// Append only useful-length wall lists, sized exactly to this terrain. Failure
// leaves both the pool end and the ordinary collision path untouched.
static void build_static_wall_height_index(u32 poolCapacity) {
    u32 groupCount = 0;
    u32 cellBytes = NUM_CELLS * NUM_CELLS * sizeof(struct StaticWallHeightCell);
    u32 used = (uintptr_t) gCurrStaticSurfacePoolEnd - (uintptr_t) gCurrStaticSurfacePool;
    u32 start = ALIGN(used, sizeof(struct SurfaceNode *));
    invalidate_static_wall_height_index();
    if (cellBytes >= STATIC_WALL_HEIGHT_MAX_BYTES) {
        return;
    }

    for (s32 z = 0; z < NUM_CELLS; z++) {
        for (s32 x = 0; x < NUM_CELLS; x++) {
            groupCount += static_wall_height_list_group_count(
                gStaticSurfacePartition[z][x][SPATIAL_PARTITION_WALLS]);
            if (groupCount
                > (STATIC_WALL_HEIGHT_MAX_BYTES - cellBytes) / sizeof(struct StaticWallHeightGroup)) {
                return;
            }
        }
    }
    u32 bytes = cellBytes + groupCount * sizeof(struct StaticWallHeightGroup);
    if (groupCount == 0 || start > poolCapacity || bytes > poolCapacity - start) {
        return;
    }

    struct StaticWallHeightCell *cells = (void *) ((u8 *) gCurrStaticSurfacePool + start);
    struct StaticWallHeightGroup *groups = (void *) ((u8 *) cells + cellBytes);
    groupCount = 0;
    for (s32 z = 0; z < NUM_CELLS; z++) {
        for (s32 x = 0; x < NUM_CELLS; x++) {
            struct StaticWallHeightCell *cell = &cells[z * NUM_CELLS + x];
            struct SurfaceNode *node = gStaticSurfacePartition[z][x][SPATIAL_PARTITION_WALLS];
            cell->start = groupCount;
            cell->count = 0;
            if (static_wall_height_list_group_count(node) == 0) {
                continue;
            }
            while (node != NULL) {
                struct StaticWallHeightGroup *group = &groups[groupCount++];
                group->first = node;
                group->lowerY = 32767;
                group->upperY = -32768;
                group->count = 0;
                for (s32 i = 0; i < STATIC_WALL_HEIGHT_GROUP_SIZE && node != NULL; i++) {
                    group->lowerY = MIN(group->lowerY, node->surface->lowerY);
                    group->upperY = MAX(group->upperY, node->surface->upperY);
                    group->count++;
                    node = node->next;
                }
                cell->count++;
            }
        }
    }
    gCurrStaticSurfacePoolEnd = (u8 *) gCurrStaticSurfacePool + start + bytes;
    gStaticWallHeightIndex.groups = groups;
    gStaticWallHeightIndex.generation = gStaticSurfaceGeneration;
    gStaticWallHeightIndex.cells = cells;
}
#endif

/**
 * Allocate the part of the surface node pool to contain a surface node.
 */
static struct SurfaceNode *alloc_surface_node(u32 dynamic) {
    struct SurfaceNode **poolEnd = (struct SurfaceNode **)(dynamic ? &gDynamicSurfacePoolEnd : &gCurrStaticSurfacePoolEnd);

    struct SurfaceNode *node = *poolEnd;
    (*poolEnd)++;
    gSurfaceNodesAllocated++;
#if COLLISION_PROFILING
    if (dynamic) {
        COLLISION_PROFILE_COUNT(COLLISION_PROFILE_DYNAMIC_NODES);
    }
#endif

    node->next = NULL;

    return node;
}

/**
 * Allocate the part of the surface pool to contain a surface and
 * initialize the surface.
 */
static struct Surface *alloc_surface(u32 dynamic) {
    struct Surface **poolEnd = (struct Surface **)(dynamic ? &gDynamicSurfacePoolEnd : &gCurrStaticSurfacePoolEnd);
    
    struct Surface *surface = *poolEnd;
    (*poolEnd)++;
    gSurfacesAllocated++;
#if COLLISION_PROFILING
    if (dynamic) {
        COLLISION_PROFILE_COUNT(COLLISION_PROFILE_DYNAMIC_SURFACES);
    }
#endif

    surface->type = SURFACE_DEFAULT;
    surface->force = 0;
    surface->flags = SURFACE_FLAGS_NONE;
    surface->room = 0;
    surface->object = NULL;

    return surface;
}

/**
 * Add a surface to the correct cell list of surfaces.
 * @param dynamic Determines whether the surface is static or dynamic
 * @param cellX The X position of the cell in which the surface resides
 * @param cellZ The Z position of the cell in which the surface resides
 * @param surface The surface to add
 */
static void add_surface_to_cell(s32 dynamic, s32 cellX, s32 cellZ, struct Surface *surface) {
    struct SurfaceNode **list;
    struct DynamicWallTail *wallTail = NULL;
    s32 priority;
    s32 sortDir = 1; // highest to lowest, then insertion order (water and floors)
    s32 listIndex;

    if (SURFACE_IS_NEW_WATER(surface->type)) {
        listIndex = SPATIAL_PARTITION_WATER;
    } else if (surface->normal.y > NORMAL_FLOOR_THRESHOLD) {
        listIndex = SPATIAL_PARTITION_FLOORS;
    } else if (surface->normal.y < NORMAL_CEIL_THRESHOLD) {
        listIndex = SPATIAL_PARTITION_CEILS;
        sortDir = -1; // lowest to highest, then insertion order
    } else {
        listIndex = SPATIAL_PARTITION_WALLS;
        sortDir = 0; // insertion order
    }

    s32 surfacePriority = surface->upperY * sortDir;

    struct SurfaceNode *newNode = alloc_surface_node(dynamic);
    newNode->surface = surface;

    if (dynamic) {
        list = &gDynamicSurfacePartition[cellZ][cellX][listIndex];
        if (*list == NULL) {
            if (sNumCellsUsed < sizeof(sCellsUsed) / sizeof(sCellsUsed[0])) {
                sCellsUsed[sNumCellsUsed].z = cellZ;
                sCellsUsed[sNumCellsUsed].x = cellX;
                sCellsUsed[sNumCellsUsed].partition = listIndex;
                sNumCellsUsed++;
            } else {
                sClearAllCells = TRUE;
            }
        }
    } else {
        list = &gStaticSurfacePartition[cellZ][cellX][listIndex];
        // Static object collision can be added after the terrain map is built.
        if (sWaterCellMapReady && listIndex == SPATIAL_PARTITION_WATER) {
            sWaterCellMayContainWater[cellZ][cellX] = TRUE;
        }
    }

    if (dynamic && listIndex == SPATIAL_PARTITION_WALLS) {
        wallTail = &sDynamicWallTails[(cellX & 3) | ((cellZ & 3) << 2)];
        // Persistent publication can append after this tail in the same frame.
        if (wallTail->list == list && wallTail->tail->next == NULL) {
            wallTail->tail->next = newNode;
            wallTail->tail = newNode;
            return;
        }
    }

    if (*list == NULL) {
        if (wallTail != NULL) {
            wallTail->list = list;
            wallTail->tail = newNode;
        }
        *list = newNode;
        return;
    }

    struct SurfaceNode *curNode = *list;

    // Check if surface should be placed at the beginning of the list.
    priority = curNode->surface->upperY * sortDir;
    if (surfacePriority > priority) {
        *list = newNode;
        newNode->next = curNode;
        return;
    }

    // Loop until we find the appropriate place for the surface in the list.
    while (curNode->next != NULL) {
        priority = curNode->next->surface->upperY * sortDir;

        if (surfacePriority > priority) {
            break;
        }

        curNode = curNode->next;
    }

    newNode->next = curNode->next;
    curNode->next = newNode;
    if (wallTail != NULL) {
        wallTail->list = list;
        wallTail->tail = newNode;
    }
}

void record_persistent_surface_counts(u32 surfaces, u32 nodes) {
    gSurfacesAllocated += surfaces;
    gSurfaceNodesAllocated += nodes;
#if COLLISION_PROFILING
    COLLISION_PROFILE_ADD(COLLISION_PROFILE_DYNAMIC_SURFACES, surfaces);
    COLLISION_PROFILE_ADD(COLLISION_PROFILE_DYNAMIC_NODES, nodes);
#endif
}

void publish_dynamic_surface_cell(s32 cellX, s32 cellZ, s32 partition, struct SurfaceNode *head) {
#if DYNAMIC_SURFACE_INDEX
    if (gDynamicSurfaceIndex.active) {
        invalidate_dynamic_surface_index();
    }
#endif
    struct SurfaceNode **list = &gDynamicSurfacePartition[cellZ][cellX][partition];
    if (*list == NULL) {
        if (sNumCellsUsed < sizeof(sCellsUsed) / sizeof(sCellsUsed[0])) {
            sCellsUsed[sNumCellsUsed].z = cellZ;
            sCellsUsed[sNumCellsUsed].x = cellX;
            sCellsUsed[sNumCellsUsed].partition = partition;
            sNumCellsUsed++;
        } else {
            sClearAllCells = TRUE;
        }
    }
    s32 direction = partition == SPATIAL_PARTITION_CEILS ? -1 : 1;
    if (partition == SPATIAL_PARTITION_WALLS) {
        direction = 0;
    }
    // Both lists are ordered. Existing faces win ties, just as repeated insertion did.
    while (*list != NULL && head != NULL) {
        if ((*list)->surface->upperY * direction >= head->surface->upperY * direction) {
            list = &(*list)->next;
        } else {
            struct SurfaceNode *next = head->next;
            head->next = *list;
            *list = head;
            list = &head->next;
            head = next;
        }
    }
    if (head != NULL) {
        *list = head;
    }
}

/**
 * Every level is split into CELL_SIZE * CELL_SIZE cells of surfaces (to limit computing
 * time). This function determines the lower cell for a given x/z position.
 * @param coord The coordinate to test
 */
static s32 lower_cell_index(s32 coord) {
    // Move from range [-LEVEL_BOUNDARY_MAX, LEVEL_BOUNDARY_MAX) to [0, 2 * LEVEL_BOUNDARY_MAX)
    coord += LEVEL_BOUNDARY_MAX;
    if (coord < 0) {
        coord = 0;
    }

    // [0, NUM_CELLS)
    s32 index = coord / CELL_SIZE;

    // Potentially > NUM_CELLS - 1, but since the upper index is <= NUM_CELLS - 1, not exploitable
    return MAX(0, index);
}

/**
 * Every level is split into CELL_SIZE * CELL_SIZE cells of surfaces (to limit computing
 * time). This function determines the upper cell for a given x/z position.
 * @param coord The coordinate to test
 */
static s32 upper_cell_index(s32 coord) {
    // Move from range [-LEVEL_BOUNDARY_MAX, LEVEL_BOUNDARY_MAX) to [0, 2 * LEVEL_BOUNDARY_MAX)
    coord += LEVEL_BOUNDARY_MAX;
    if (coord < 0) {
        coord = 0;
    }

    // [0, NUM_CELLS)
    s32 index = coord / CELL_SIZE;

    // Potentially < 0, but since lower index is >= 0, not exploitable
    return MIN((NUM_CELLS - 1), index);
}

/**
 * Every level is split into 16x16 cells, this takes a surface, finds
 * the appropriate cells (with a buffer), and adds the surface to those
 * cells.
 * @param surface The surface to check
 * @param dynamic Boolean determining whether the surface is static or dynamic
 */
static void add_surface(struct Surface *surface, s32 dynamic) {
    s32 cellZ, cellX;
    s32 minX, maxX, minZ, maxZ;

#if DYNAMIC_SURFACE_INDEX
    if (dynamic && gDynamicSurfaceIndex.active) {
        invalidate_dynamic_surface_index();
    }
#endif

    min_max_3i(surface->vertex1[0], surface->vertex2[0], surface->vertex3[0], &minX, &maxX);
    min_max_3i(surface->vertex1[2], surface->vertex2[2], surface->vertex3[2], &minZ, &maxZ);

    s32 minCellX = lower_cell_index(minX);
    s32 maxCellX = upper_cell_index(maxX);
    s32 minCellZ = lower_cell_index(minZ);
    s32 maxCellZ = upper_cell_index(maxZ);

    for (cellZ = minCellZ; cellZ <= maxCellZ; cellZ++) {
        for (cellX = minCellX; cellX <= maxCellX; cellX++) {
            add_surface_to_cell(dynamic, cellX, cellZ, surface);
        }
    }
}

/**
 * Initializes a Surface struct using the given vertex data
 * @param vertexData The raw data containing vertex positions
 * @param vertexIndices Helper which tells positions in vertexData to start reading vertices
 * @param dynamic If the surface belongs to an object or not
 */
static struct Surface *read_surface_data(TerrainData *vertexData, TerrainData **vertexIndices, u32 dynamic) {
    Vec3t v[3];
    Vec3f n;
    Vec3t offset;
    s16 min, max;

    vec3_scale_dest(offset, (*vertexIndices), 3);

    vec3s_copy(v[0], (vertexData + offset[0]));
    vec3s_copy(v[1], (vertexData + offset[1]));
    vec3s_copy(v[2], (vertexData + offset[2]));

    find_vector_perpendicular_to_plane(n, v[0], v[1], v[2]);

    f32 mag = (sqr(n[0]) + sqr(n[1]) + sqr(n[2]));
    // This will never need to be run for custom levels because Fast64 does this step before exporting.
    // assert(mag >= NEAR_ZERO, "Denorm tri was found.");
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if (mag < NEAR_ZERO) {
        return NULL;
    }
#endif
    mag = 1.0f / sqrtf(mag);
    vec3_scale(n, mag);

    struct Surface *surface = alloc_surface(dynamic);

    vec3s_copy(surface->vertex1, v[0]);
    vec3s_copy(surface->vertex2, v[1]);
    vec3s_copy(surface->vertex3, v[2]);

    surface->normal.x = n[0];
    surface->normal.y = n[1];
    surface->normal.z = n[2];

    surface->originOffset = -vec3_dot(n, v[0]);

    min_max_3s(v[0][1], v[1][1], v[2][1], &min, &max);
    surface->lowerY = (min - SURFACE_VERTICAL_BUFFER);
    surface->upperY = (max + SURFACE_VERTICAL_BUFFER);

    return surface;
}

#ifndef ALL_SURFACES_HAVE_FORCE
/**
 * Returns whether a surface has exertion/moves Mario
 * based on the surface type.
 */
static s32 surface_has_force(s32 surfaceType) {
    s32 hasForce = FALSE;

    switch (surfaceType) {
        case SURFACE_0004: // Unused
        case SURFACE_FLOWING_WATER:
        case SURFACE_DEEP_MOVING_QUICKSAND:
        case SURFACE_SHALLOW_MOVING_QUICKSAND:
        case SURFACE_MOVING_QUICKSAND:
        case SURFACE_HORIZONTAL_WIND:
        case SURFACE_INSTANT_MOVING_QUICKSAND:
            hasForce = TRUE;
            break;

        default:
            break;
    }
    return hasForce;
}
#endif

/**
 * Returns whether a surface should have the
 * SURFACE_FLAG_NO_CAM_COLLISION flag.
 */
s32 surf_has_no_cam_collision(s32 surfaceType) {
    s32 flags = SURFACE_FLAGS_NONE;

    switch (surfaceType) {
        case SURFACE_NO_CAM_COLLISION:
        case SURFACE_NON_INTERACTIVE_BOUNDARY:
        case SURFACE_NO_CAM_COL_VERY_SLIPPERY:
        case SURFACE_SWITCH:
            flags = SURFACE_FLAG_NO_CAM_COLLISION;
            break;

        default:
            break;
    }

    return flags;
}

/**
 * Load in the surfaces for a given surface type. This includes setting the flags,
 * exertion, and room.
 */
static void load_static_surfaces(TerrainData **data, TerrainData *vertexData, s32 surfaceType, RoomData **surfaceRooms) {
    s32 i;
    struct Surface *surface;
    RoomData room = 0;
#ifndef ALL_SURFACES_HAVE_FORCE
    s16 hasForce = surface_has_force(surfaceType);
#endif
    s32 flags = surf_has_no_cam_collision(surfaceType);

    s32 numSurfaces = *(*data)++;

    for (i = 0; i < numSurfaces; i++) {
        if (*surfaceRooms != NULL) {
            room = *(*surfaceRooms)++;
        }

        surface = read_surface_data(vertexData, data, FALSE);
        if (surface != NULL) {
            surface->room = room;
            surface->type = surfaceType;
            surface->flags = flags;

#ifdef ALL_SURFACES_HAVE_FORCE
            surface->force = *(*data + 3);
#else
            if (hasForce) {
                surface->force = *(*data + 3);
            } else {
                surface->force = 0;
            }
#endif

            add_surface(surface, FALSE);
        }

#ifdef ALL_SURFACES_HAVE_FORCE
        *data += 4;
#else
        *data += 3;
        if (hasForce) {
            (*data)++;
        }
#endif
    }
}

/**
 * Read the data for vertices for reference by triangles.
 */
static TerrainData *read_vertex_data(TerrainData **data) {
    s32 numVertices = *(*data)++;

    TerrainData *vertexData = *data;
    *data += 3 * numVertices;

    return vertexData;
}

/**
 * Loads in special environmental regions, such as water, poison gas, and JRB fog.
 */
static void load_environmental_regions(TerrainData **data) {
    s32 i;

    gEnvironmentRegions = *data;
    s32 numRegions = *(*data)++;

    for (i = 0; i < numRegions; i++) {
        *data += 5;
        gEnvironmentLevels[i] = *(*data)++;
    }
}

/**
 * Allocate the dynamic surface pool for object collision.
 */
void alloc_surface_pools(void) {
#if DYNAMIC_SURFACE_INDEX
    invalidate_dynamic_surface_index();
#endif
    gDynamicSurfacePool = main_pool_alloc(DYNAMIC_SURFACE_POOL_SIZE, MEMORY_POOL_LEFT);
    gDynamicSurfacePoolEnd = gDynamicSurfacePool;

    gCCMEnteredSlide = FALSE;
    reset_red_coins_collected();
}

#ifdef NO_SEGMENTED_MEMORY
/**
 * Get the size of the terrain data, to get the correct size when copying later.
 */
u32 get_area_terrain_size(TerrainData *data) {
    TerrainData *startPos = data;
    s32 end = FALSE;
    TerrainData terrainLoadType;
    s32 numVertices;
    s32 numRegions;
    s32 numSurfaces;
#ifndef ALL_SURFACES_HAVE_FORCE
    TerrainData hasForce;
#endif

    while (!end) {
        terrainLoadType = *data++;

        switch (terrainLoadType) {
            case TERRAIN_LOAD_VERTICES:
                numVertices = *data++;
                data += 3 * numVertices;
                break;

            case TERRAIN_LOAD_OBJECTS:
                data += get_special_objects_size(data);
                break;

            case TERRAIN_LOAD_ENVIRONMENT:
                numRegions = *data++;
                data += 6 * numRegions;
                break;

            case TERRAIN_LOAD_CONTINUE:
                continue;

            case TERRAIN_LOAD_END:
                end = TRUE;
                break;

            default:
                numSurfaces = *data++;
#ifdef ALL_SURFACES_HAVE_FORCE
                data += 4 * numSurfaces;
#else
                hasForce = surface_has_force(terrainLoadType);
                data += (3 + hasForce) * numSurfaces;
#endif
                break;
        }
    }

    return data - startPos;
}
#endif


/**
 * Process the level file, loading in vertices, surfaces, some objects, and environmental
 * boxes (water, gas, JRB fog).
 */
void load_area_terrain(s32 index, TerrainData *data, RoomData *surfaceRooms, s16 *macroObjects) {
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();
    s32 terrainLoadType;
    TerrainData *vertexData = NULL;
    u32 surfacePoolData;

    persistent_surfaces_reset();

    invalidate_static_wall_height_index();
    gStaticSurfaceGeneration++;
#if DYNAMIC_SURFACE_INDEX
    invalidate_dynamic_surface_index();
#endif
#if STATIC_SURFACE_INDEX
    gStaticSurfaceIndex.offsets = NULL;
#endif
    // Initialize the data for this.
    sWaterCellMapReady = FALSE;
    gEnvironmentRegions = NULL;
    gSurfaceNodesAllocated = 0;
    gSurfacesAllocated = 0;
    bzero(sDynamicWallTails, sizeof(sDynamicWallTails));
    bzero(&sCellsUsed, sizeof(sCellsUsed));
    sNumCellsUsed = 0;
    sClearAllCells = TRUE;

    // Clear the static (level) surface partitions for new use.
    bzero(gStaticSurfacePartition, sizeof(gStaticSurfacePartition));
    gTotalStaticSurfaceData = 0;

    // Initialise a new surface pool for this block of static surface data
    u32 surfacePoolCapacity = main_pool_available() - 0x10;
    gCurrStaticSurfacePool = main_pool_alloc(surfacePoolCapacity, MEMORY_POOL_LEFT);
    gCurrStaticSurfacePoolEnd = gCurrStaticSurfacePool;

    // A while loop iterating through each section of the level data. Sections of data
    // are prefixed by a terrain "type." This type is reused for surfaces as the surface
    // type.
    while (TRUE) {
        terrainLoadType = *data++;

        if (TERRAIN_LOAD_IS_SURFACE_TYPE_LOW(terrainLoadType)) {
            load_static_surfaces(&data, vertexData, terrainLoadType, &surfaceRooms);
        } else if (terrainLoadType == TERRAIN_LOAD_VERTICES) {
            vertexData = read_vertex_data(&data);
        } else if (terrainLoadType == TERRAIN_LOAD_OBJECTS) {
            spawn_special_objects(index, &data);
        } else if (terrainLoadType == TERRAIN_LOAD_ENVIRONMENT) {
            load_environmental_regions(&data);
        } else if (terrainLoadType == TERRAIN_LOAD_CONTINUE) {
            continue;
        } else if (terrainLoadType == TERRAIN_LOAD_END) {
            break;
        } else if (TERRAIN_LOAD_IS_SURFACE_TYPE_HIGH(terrainLoadType)) {
            load_static_surfaces(&data, vertexData, terrainLoadType, &surfaceRooms);
            continue;
        }
    }

    build_water_cell_map();

    if (macroObjects != NULL && *macroObjects != -1) {
        // If the first macro object presetID is within the range [0, 29].
        // Generally an early spawning method, every object is in BBH (the first level).
        if (0 <= *macroObjects && *macroObjects < 30) {
            spawn_macro_objects_hardcoded(index, macroObjects);
        }
        // A more general version that can spawn more objects.
        else {
            spawn_macro_objects(index, macroObjects);
        }
    }

#if STATIC_SURFACE_INDEX
    build_static_surface_index(surfacePoolCapacity);
#endif
#if STATIC_WALL_HEIGHT_GROUPS
    build_static_wall_height_index(surfacePoolCapacity);
#endif
    surfacePoolData = (uintptr_t)gCurrStaticSurfacePoolEnd - (uintptr_t)gCurrStaticSurfacePool;
    gTotalStaticSurfaceData += surfacePoolData;
    main_pool_realloc(gCurrStaticSurfacePool, surfacePoolData);

    gNumStaticSurfaceNodes = gSurfaceNodesAllocated;
    gNumStaticSurfaces = gSurfacesAllocated;
    profiler_collision_update(first);
}

/**
 * If not in time stop, clear the surface partitions.
 */
void clear_dynamic_surfaces(void) {
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();
    if (!(gTimeStopState & TIME_STOP_ACTIVE)) {
#if DYNAMIC_SURFACE_INDEX
        invalidate_dynamic_surface_index();
#endif
        bzero(sDynamicWallTails, sizeof(sDynamicWallTails));
        clear_dynamic_surface_references();

        gSurfacesAllocated = gNumStaticSurfaces;
        gSurfaceNodesAllocated = gNumStaticSurfaceNodes;
        gDynamicSurfacePoolEnd = gDynamicSurfacePool;
        if (sClearAllCells) {
            bzero(gDynamicSurfacePartition, sizeof(gDynamicSurfacePartition));
        } else {
            for (u32 i = 0; i < sNumCellsUsed; i++) {
                gDynamicSurfacePartition[sCellsUsed[i].z][sCellsUsed[i].x][sCellsUsed[i].partition] = NULL;
            }
        }
        sNumCellsUsed = 0;
        sClearAllCells = FALSE;
        persistent_surfaces_begin_frame();
    }
#if COLLISION_PROFILING
    COLLISION_PROFILE_ADD(COLLISION_PROFILE_DYNAMIC_CLEAR_TICKS, osGetCount() - first);
#endif
    profiler_collision_update(first);
}

static s32 transform_dorrie_collision_vertices(Mat4 transform, TerrainData *vertices,
                                               TerrainData *vertexData, s32 numVertices) {
    // Preserve the ordinary transform for tilted or vertically scaled poses.
    if (transform[0][1] != 0.0f || transform[2][1] != 0.0f || transform[1][0] != 0.0f
        || transform[1][2] != 0.0f || transform[1][1] != 1.0f) {
        return FALSE;
    }
    while (numVertices--) {
        PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.matrix);
        // Match the original integer rotation before adding translation.
        vertexData[0] = transform[0][0] * vertices[0] + transform[2][0] * vertices[2];
        vertexData[1] = vertices[1];
        vertexData[2] = transform[0][2] * vertices[0] + transform[2][2] * vertices[2];
        vec3_add(vertexData, transform[3]);
        vertices += 3;
        vertexData += 3;
    }
    return TRUE;
}

/**
 * Applies an object's transformation to the object's vertices.
 */
void transform_object_vertices(TerrainData **data, TerrainData *vertexData) {
    Mat4 *objectTransform = &o->transform;

    register s32 numVertices = *(*data)++;

    register TerrainData *vertices = *data;

    if (o->header.gfx.throwMatrix == NULL) {
        o->header.gfx.throwMatrix = objectTransform;
        obj_build_transform_from_pos_and_angle(o, O_POS_INDEX, O_FACE_ANGLE_INDEX);
    }

    Mat4 transform;
    mtxf_scale_vec3f(transform, *objectTransform, o->header.gfx.scale);

    if (o->behavior == segmented_to_virtual(bhvDorrie)
        && transform_dorrie_collision_vertices(transform, vertices, vertexData, numVertices)) {
        *data = vertices + numVertices * 3;
        return;
    }

    // Go through all vertices, rotating and translating them to transform the object.
    Vec3f pos;
    while (numVertices--) {
        vec3s_to_vec3f(pos, vertices);
        vertices += 3;

        //! No bounds check on vertex data
        linear_mtxf_mul_vec3_and_translate(transform, vertexData, pos);

        vertexData += 3;
    }

    *data = vertices;
}

/**
 * Load in the surfaces for the o. This includes setting the flags, exertion, and room.
 */
void load_object_surfaces(TerrainData **data, TerrainData *vertexData, u32 dynamic) {
    s32 i;

    s32 surfaceType = *(*data)++;
    s32 numSurfaces = *(*data)++;

#ifndef ALL_SURFACES_HAVE_FORCE
    TerrainData hasForce = surface_has_force(surfaceType);
#endif

    s32 flags = surf_has_no_cam_collision(surfaceType) | (dynamic ? SURFACE_FLAG_DYNAMIC : 0);

    // The DDD warp is initially loaded at the origin and moved to the proper
    // position in paintings.c and doesn't update its room, so set it here.
    RoomData room = (o->behavior == segmented_to_virtual(bhvDddWarp)) ? 5 : 0;

    for (i = 0; i < numSurfaces; i++) {
        struct Surface *surface = read_surface_data(vertexData, data, dynamic);

        if (surface != NULL) {
            surface->object = o;
            surface->type = surfaceType;

#ifdef ALL_SURFACES_HAVE_FORCE
            surface->force = *(*data + 3);
#else
            if (hasForce) {
                surface->force = *(*data + 3);
            } else {
                surface->force = 0;
            }
#endif

            surface->flags |= flags;
            surface->room = room;
            add_surface(surface, dynamic);
        }

#ifdef ALL_SURFACES_HAVE_FORCE
        *data += 4;
#else
        if (hasForce) {
            *data += 4;
        } else {
            *data += 3;
        }
#endif
    }
}

#ifdef AUTO_COLLISION_DISTANCE
static void get_optimal_coll_dist(struct Object *obj) {
    register f32 thisVertDist, maxDist = 0.0f;
    Vec3f v;
    TerrainData *collisionData = o->collisionData;
    obj->oFlags |= OBJ_FLAG_DONT_CALC_COLL_DIST;
    collisionData++;
    register u32 vertsLeft = *(collisionData)++;
    while (vertsLeft) {
        vec3_prod(v, collisionData, obj->header.gfx.scale);
        thisVertDist = vec3_sumsq(v);
        if (thisVertDist > maxDist) maxDist = thisVertDist;
        collisionData += 3;
        vertsLeft--;
    }
    obj->oCollisionDistance = (sqrtf(maxDist) + 100.0f);
}
#endif

static TerrainData sVertexData[600];

/**
 * Transform an object's vertices, reload them, and render the object.
 */
#if BOWSER_STATIONARY_SURFACES && defined(ALL_SURFACES_HAVE_FORCE)
static s32 bowser_platform_only_needs_top(void) {
    return o->behavior == segmented_to_virtual(bhvFallingBowserPlatform)
           && o->oAction == BOWSER_BITS_PLAT_ACT_CHECK && o->oPosX == 0.0f && o->oPosY == 0.0f
           && o->oPosZ == 0.0f && o->oFaceAnglePitch == 0 && o->oFaceAngleYaw == 0
           && o->oFaceAngleRoll == 0 && o->header.gfx.scale[0] == 1.0f && o->header.gfx.scale[1] == 1.0f
           && o->header.gfx.scale[2] == 1.0f && gMarioObject->oPosY >= 307.0f && gMarioState != NULL
           && !(gMarioState->action & ACT_FLAG_AIR) && gMarioState->vel[1] >= 0.0f
           && o->oBitsPlatformBowser != NULL && o->oBitsPlatformBowser->oPosY >= 307.0f
           && o->oBitsPlatformBowser->oHeldState == HELD_FREE
           && o->oBitsPlatformBowser->oAction != BOWSER_ACT_THROWN
           && o->oBitsPlatformBowser->oAction != BOWSER_ACT_HIT_MINE
           && o->oBitsPlatformBowser->oAction != BOWSER_ACT_JUMP_ONTO_STAGE && gCamera != NULL
           && gCamera->pos[1] >= 307.0f && gCamera->focus[1] >= 307.0f;
}

static void load_bowser_platform_top(TerrainData **data, TerrainData *vertices) {
    // Shipped pieces have twelve triangles. Keep native floor construction and
    // ordering. Airborne Mario and below-floor cameras retain the full mesh.
    TerrainData filtered[2 + 12 * 4];
    s32 count = (*data)[1];
    if (count > 12 || count < 0) {
        load_object_surfaces(data, vertices, TRUE);
        return;
    }
    filtered[0] = (*data)[0];
    filtered[1] = 0;
    TerrainData *faces = *data + 2;
    for (s32 i = 0; i < count; i++, faces += 4) {
        TerrainData *a = &vertices[faces[0] * 3];
        TerrainData *b = &vertices[faces[1] * 3];
        TerrainData *c = &vertices[faces[2] * 3];
        f32 normalY = (f32) (b[2] - a[2]) * (c[0] - b[0]) - (f32) (b[0] - a[0]) * (c[2] - b[2]);
        if (a[1] == 307 && b[1] == 307 && c[1] == 307 && normalY > 0.0f) {
            memcpy(&filtered[2 + filtered[1] * 4], faces, 4 * sizeof(*faces));
            filtered[1]++;
        }
    }
    *data += 2 + count * 4;
    if (filtered[1] != 0) {
        TerrainData *cursor = filtered;
        load_object_surfaces(&cursor, vertices, TRUE);
    }
}

#endif

void load_object_collision_model(void) {
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();
    TerrainData *collisionData = o->collisionData;

    Vec3f dist;
    vec3_diff(dist, &o->oPosVec, &gMarioObject->oPosVec);
    f32 sqrLateralDist = sqr(dist[0]) + sqr(dist[2]);

    f32 verticalMarioDiff = gMarioObject->oPosY - o->oPosY;

#ifdef AUTO_COLLISION_DISTANCE
    if (!(o->oFlags & OBJ_FLAG_DONT_CALC_COLL_DIST)) {
        get_optimal_coll_dist(o);
    }
#endif

    // If the object collision is supposed to be loaded more than the
    // drawing distance, extend the drawing range.
    if (o->oCollisionDistance > o->oDrawingDistance) {
        o->oDrawingDistance = o->oCollisionDistance;
    }

    f32 collisionAbove = o->oCollisionDistance + 2000.0f;
    if (o->behavior == segmented_to_virtual(bhvTTCSpinner)) {
        // Distant spinner floors are not needed for contact. Keep the full
        // mesh nearby, padded for body clearance, rotation and this frame's speed.
        collisionAbove = o->oCollisionDistance + 200.0f + absf(gMarioState->vel[1]);
    }

    s32 inColRadius = sqrLateralDist < sqr(o->oCollisionDistance)
                      && (verticalMarioDiff > 0 || verticalMarioDiff > -o->oCollisionDistance)
                      && (verticalMarioDiff < 0 || verticalMarioDiff < collisionAbove);

    // Update if no Time Stop, in range, and in the current room.
    if (!(gTimeStopState & TIME_STOP_ACTIVE)
        && (inColRadius || object_platform_motion_needs_collision(o))
        && !(o->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM)) {
#if COLLISION_PROFILING
        COLLISION_PROFILE_COUNT(COLLISION_PROFILE_DYNAMIC_MODELS);
#endif
        object_platform_motion_refresh_transform(o);
        s32 persistent = FALSE;
        s32 onlyTop = FALSE;
#if BOWSER_STATIONARY_SURFACES && defined(ALL_SURFACES_HAVE_FORCE)
        onlyTop = bowser_platform_only_needs_top();
#endif
#if DORRIE_STATIONARY_SURFACES && defined(ALL_SURFACES_HAVE_FORCE)
        if (o->behavior == segmented_to_virtual(bhvDorrie)) {
            if (o->header.gfx.throwMatrix == NULL) {
                o->header.gfx.throwMatrix = &o->transform;
                obj_build_transform_from_pos_and_angle(o, O_POS_INDEX, O_FACE_ANGLE_INDEX);
            }
            persistent =
                persistent_surfaces_publish_dorrie(o, collisionData, o->transform, o->header.gfx.scale);
        }
#endif
#if JRB_STATIONARY_SURFACES && defined(ALL_SURFACES_HAVE_FORCE)
        if (o->behavior == segmented_to_virtual(bhvInSunkenShip)
            || o->behavior == segmented_to_virtual(bhvInSunkenShip2)) {
            if (o->header.gfx.throwMatrix == NULL) {
                o->header.gfx.throwMatrix = &o->transform;
                obj_build_transform_from_pos_and_angle(o, O_POS_INDEX, O_FACE_ANGLE_INDEX);
            }
            persistent = persistent_surfaces_publish_jrb_ship(o, collisionData, o->transform,
                                                              o->header.gfx.scale);
        }
#endif

#if BOWSER_STATIONARY_SURFACES && defined(ALL_SURFACES_HAVE_FORCE)
        if (!onlyTop && o->behavior == segmented_to_virtual(bhvFallingBowserPlatform)) {
            if (o->oAction == BOWSER_BITS_PLAT_ACT_CHECK) {
                if (o->header.gfx.throwMatrix == NULL) {
                    o->header.gfx.throwMatrix = &o->transform;
                    obj_build_transform_from_pos_and_angle(o, O_POS_INDEX, O_FACE_ANGLE_INDEX);
                }
                persistent = persistent_surfaces_publish_bowser_platform(o, collisionData, o->transform,
                                                                         o->header.gfx.scale);
            } else {
                // Falling pieces use native loading and release their waiting-pose slot.
                persistent_surfaces_forget(o);
            }
        }
#endif

#if PERSISTENT_DYNAMIC_SURFACES && defined(ALL_SURFACES_HAVE_FORCE)
        // Keep a successful specialized publication; tiny meshes stay native.
        if (!persistent && collisionData[1] >= 64
#if DORRIE_STATIONARY_SURFACES
            && o->behavior != segmented_to_virtual(bhvDorrie)
#endif
        ) {
            if (o->header.gfx.throwMatrix == NULL) {
                o->header.gfx.throwMatrix = &o->transform;
                obj_build_transform_from_pos_and_angle(o, O_POS_INDEX, O_FACE_ANGLE_INDEX);
            }
            Mat4 transform;
            mtxf_scale_vec3f(transform, o->transform, o->header.gfx.scale);
            RoomData room = (o->behavior == segmented_to_virtual(bhvDddWarp)) ? 5 : 0;
            persistent = persistent_surfaces_publish(o, collisionData, transform, room);
        }
#endif
        if (!persistent) {
            collisionData++;
            transform_object_vertices(&collisionData, sVertexData);

            // TERRAIN_LOAD_CONTINUE acts as an "end" to the terrain data.
            while (*collisionData != TERRAIN_LOAD_CONTINUE) {
#if BOWSER_STATIONARY_SURFACES && defined(ALL_SURFACES_HAVE_FORCE)
                if (onlyTop) {
                    load_bowser_platform_top(&collisionData, sVertexData);
                } else
#endif
                {
                    load_object_surfaces(&collisionData, sVertexData, TRUE);
                }
            }
        }
        object_platform_motion_publish(o);
    }

    f32 marioDist = o->oDistanceToMario;

    // On an object's first frame, the distance is set to 19000.0f.
    // If the distance hasn't been updated, update it now.
    if (marioDist == 19000.0f) {
        marioDist = dist_between_objects(o, gMarioObject);
    }

    COND_BIT((marioDist < cur_obj_drawing_distance(marioDist)), o->header.gfx.node.flags,
             GRAPH_RENDER_ACTIVE);
#if COLLISION_PROFILING
    COLLISION_PROFILE_ADD(COLLISION_PROFILE_DYNAMIC_BUILD_TICKS, osGetCount() - first);
#endif
    profiler_collision_update(first);
}

/**
 * Transform an object's vertices and add them to the static surface pool.
 */
void load_object_static_model(void) {
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();
    TerrainData *collisionData = o->collisionData;
    u32 surfacePoolData;

    invalidate_static_wall_height_index();
    gStaticSurfaceGeneration++;
#if STATIC_SURFACE_INDEX
    // This addition can change any sorted list. Keep using the live linked
    // storage until the next terrain load builds a fresh complete index.
    gStaticSurfaceIndex.offsets = NULL;
#endif
    // Initialise a new surface pool for this block of surface data
    gCurrStaticSurfacePool = main_pool_alloc(main_pool_available() - 0x10, MEMORY_POOL_LEFT);
    gCurrStaticSurfacePoolEnd = gCurrStaticSurfacePool;
    gSurfaceNodesAllocated = gNumStaticSurfaceNodes;
    gSurfacesAllocated = gNumStaticSurfaces;

    collisionData++;
    transform_object_vertices(&collisionData, sVertexData);

    // TERRAIN_LOAD_CONTINUE acts as an "end" to the terrain data.
    while (*collisionData != TERRAIN_LOAD_CONTINUE) {
        load_object_surfaces(&collisionData, sVertexData, FALSE);
    }

    surfacePoolData = (uintptr_t)gCurrStaticSurfacePoolEnd - (uintptr_t)gCurrStaticSurfacePool;
    gTotalStaticSurfaceData += surfacePoolData;
    main_pool_realloc(gCurrStaticSurfacePool, surfacePoolData);

    gNumStaticSurfaceNodes = gSurfaceNodesAllocated;
    gNumStaticSurfaces = gSurfacesAllocated;
    profiler_collision_update(first);
}
