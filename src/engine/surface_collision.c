#include <PR/ultratypes.h>

#include "sm64.h"
#include "game/collision_profile.h"
#include "game/debug.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/mario.h"
#include "game/object_list_processor.h"
#include "game/object_platform_motion.h"
#include "math_util.h"
#include "surface_collision.h"
#include "surface_load.h"
#include "surface_query.h"
#include "game/puppyprint.h"

/**************************************************
 *                      WALLS                     *
 **************************************************/

static s32 check_wall_vw(f32 d00, f32 d01, f32 d11, f32 d20, f32 d21, f32 mult) {
    f32 v = ((d11 * d20) - (d01 * d21));
    if (v < 0.0f || v > mult) {
        return TRUE;
    }

    f32 w = ((d00 * d21) - (d01 * d20));
    if (w < 0.0f || w > mult || v + w > mult) {
        return TRUE;
    }

    return FALSE;
}

s32 check_wall_edge(Vec3f vert, Vec3f v2, f32 *d00, f32 *d01, f32 *invDenom, f32 *offset, f32 margin_radius) {
    if (FLT_IS_NONZERO(vert[1])) {
        f32 v = (v2[1] / vert[1]);
        if (v < 0.0f || v > 1.0f) {
            return TRUE;
        }

        *d00 = ((vert[0] * v) - v2[0]);
        *d01 = ((vert[2] * v) - v2[2]);
        *invDenom = sqrtf(sqr(*d00) + sqr(*d01));
        *offset = (*invDenom - margin_radius);

        return (*offset > 0.0f);
    }

    return TRUE;
}

// Apply actor-specific pass-through rules to walls, floors and ceilings.
// Mario-only surfaces also stay invisible to camera collision queries.
static ALWAYS_INLINE s32 surface_is_passable_vanish(struct Surface *surface) {
    if (surface->type == SURFACE_MARIO_ONLY_GRIPPY) {
        return o != gMarioObject || (gCollisionFlags & COLLISION_FLAG_CAMERA);
    }
    return surface->type == SURFACE_VANISH_CAP_WALLS && !(gCollisionFlags & COLLISION_FLAG_CAMERA)
           && o != NULL
           && ((o->activeFlags & ACTIVE_FLAG_MOVE_THROUGH_GRATE)
               || (o == gMarioObject && (gMarioState->flags & MARIO_VANISH_CAP)));
}

// The two cursors share one numerical solver; ordinary scans keep their hot loop.
#define WALL_HEIGHT_GROUP_SCAN 0
#include "surface_wall.inc.c"
#if STATIC_WALL_HEIGHT_GROUPS
#undef WALL_HEIGHT_GROUP_SCAN
#define WALL_HEIGHT_GROUP_SCAN 1
#include "surface_wall.inc.c"
#endif
#undef WALL_HEIGHT_GROUP_SCAN

/**
 * Formats the position and wall search for find_wall_collisions.
 */
s32 f32_find_wall_collision(f32 *xPtr, f32 *yPtr, f32 *zPtr, f32 offsetY, f32 radius) {
    struct WallCollisionData collision;

    collision.offsetY = offsetY;
    collision.radius = radius;

    collision.x = *xPtr;
    collision.y = *yPtr;
    collision.z = *zPtr;

    collision.numWalls = 0;

    s32 numCollisions = find_wall_collisions(&collision);

    *xPtr = collision.x;
    *yPtr = collision.y;
    *zPtr = collision.z;

    return numCollisions;
}

/**
 * Find wall collisions and receive their push.
 */
static s32 find_wall_collisions_in_cells(struct WallCollisionData *colData, s32 minCellX, s32 maxCellX,
                                         s32 minCellZ, s32 maxCellZ) {
    s32 numCollisions = 0;

    for (s32 cellX = minCellX; cellX <= maxCellX; cellX++) {
        for (s32 cellZ = minCellZ; cellZ <= maxCellZ; cellZ++) {
            if (!(gCollisionFlags & COLLISION_FLAG_EXCLUDE_DYNAMIC)) {
                // Check for surfaces belonging to objects.
                numCollisions += find_wall_collisions_from_list(
                    surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_WALLS, SURFACE_QUERY_DYNAMIC),
                    colData, NULL, NULL);
            }

            // Check for surfaces that are a part of level geometry.
            numCollisions += find_wall_collisions_from_list(
                surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_WALLS, SURFACE_QUERY_STATIC),
                colData, NULL, NULL);
        }
    }
    return numCollisions;
}

#if STATIC_WALL_HEIGHT_GROUPS
static __attribute__((noinline)) s32 find_wall_collisions_in_cells_with_height_groups(
    struct WallCollisionData *colData, s32 minCellX, s32 maxCellX, s32 minCellZ, s32 maxCellZ) {
    s32 numCollisions = 0;

    for (s32 cellX = minCellX; cellX <= maxCellX; cellX++) {
        for (s32 cellZ = minCellZ; cellZ <= maxCellZ; cellZ++) {
            if (!(gCollisionFlags & COLLISION_FLAG_EXCLUDE_DYNAMIC)) {
                // Check for surfaces belonging to objects.
                numCollisions += find_wall_collisions_from_list(
                    surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_WALLS, SURFACE_QUERY_DYNAMIC),
                    colData, NULL, NULL);
            }

            struct SurfaceQueryIterator walls =
                surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_WALLS, SURFACE_QUERY_STATIC);
#if STATIC_WALL_HEIGHT_GROUPS
            s32 groupCount;
            const struct StaticWallHeightGroup *groups =
                static_wall_height_groups(cellX, cellZ, colData->y + colData->offsetY, &groupCount);
            if (groups != NULL) {
                numCollisions += find_wall_collisions_from_height_groups(walls, colData, NULL, NULL,
                                                                         groups, groupCount);
                continue;
            }
#endif
            numCollisions += find_wall_collisions_from_list(walls, colData, NULL, NULL);
        }
    }
    return numCollisions;
}

#endif

// An absent or invalidated index must retain the original cell-scan loop.
// Keep grouped bookkeeping in a separate call so it cannot widen that hot loop.
static ALWAYS_INLINE s32 dispatch_wall_collisions_in_cells(struct WallCollisionData *colData,
                                                           s32 minCellX, s32 maxCellX, s32 minCellZ,
                                                           s32 maxCellZ) {
#if STATIC_WALL_HEIGHT_GROUPS
    if (gStaticWallHeightIndex.cells != NULL
        && gStaticWallHeightIndex.generation == gStaticSurfaceGeneration) {
        return find_wall_collisions_in_cells_with_height_groups(colData, minCellX, maxCellX, minCellZ,
                                                                maxCellZ);
    }
#endif
    return find_wall_collisions_in_cells(colData, minCellX, maxCellX, minCellZ, maxCellZ);
}

s32 find_wall_collisions(struct WallCollisionData *colData) {
    s32 numCollisions = 0;
    s32 x = colData->x;
    s32 z = colData->z;
    PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.collision_wall);
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();

    colData->numWalls = 0;

    if (is_outside_level_bounds(x, z)) {
        profiler_collision_update(first);
        return numCollisions;
    }

    // World (level) consists of a 16x16 grid. Find where the collision is on the grid (round toward -inf)
    s32 minCellX = GET_CELL_COORD(x - colData->radius);
    s32 minCellZ = GET_CELL_COORD(z - colData->radius);
    s32 maxCellX = GET_CELL_COORD(x + colData->radius);
    s32 maxCellZ = GET_CELL_COORD(z + colData->radius);

    numCollisions = dispatch_wall_collisions_in_cells(colData, minCellX, maxCellX, minCellZ, maxCellZ);

    gCollisionFlags &= ~(COLLISION_FLAG_RETURN_FIRST | COLLISION_FLAG_EXCLUDE_DYNAMIC | COLLISION_FLAG_INCLUDE_INTANGIBLE);
#ifdef VANILLA_DEBUG
    // Increment the debug tracker.
    gNumCalls.wall++;
#endif

    profiler_collision_update(first);
    return numCollisions;
}

// Mario can reuse height-filtered wall lists over several movement substeps.
// Caller-owned lists already have their own conservative height bounds.
static s32 find_wall_collisions_with_cache(struct WallCollisionData *colData,
                                           struct WallCollisionCache *cache) {
    s32 numCollisions = 0;
    s32 leftCache = FALSE;
    s32 x = colData->x;
    s32 z = colData->z;
    f32 originalX = colData->x;
    f32 originalZ = colData->z;
    s32 minCellX, minCellZ, maxCellX, maxCellZ;
    f32 queryY = colData->y + colData->offsetY;
    if (is_outside_level_bounds(x, z) || queryY < cache->minY || queryY > cache->maxY
        || colData->radius > cache->maxRadius || colData->x < cache->minX || colData->x > cache->maxX
        || colData->z < cache->minZ || colData->z > cache->maxZ) {
        return find_wall_collisions(colData);
    }

    minCellX = GET_CELL_COORD(x - colData->radius);
    minCellZ = GET_CELL_COORD(z - colData->radius);
    maxCellX = GET_CELL_COORD(x + colData->radius);
    maxCellZ = GET_CELL_COORD(z + colData->radius);
    // This is also checked by Mario, but keep the engine entry point safe if a
    // wall push moves the next probe beyond the cached cells.
    if (minCellX < cache->minCellX || maxCellX >= cache->minCellX + cache->cellCountX
        || minCellZ < cache->minCellZ || maxCellZ >= cache->minCellZ + cache->cellCountZ) {
        return find_wall_collisions(colData);
    }

    PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.collision_wall);
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();
    colData->numWalls = 0;

    for (s32 cellX = minCellX; cellX <= maxCellX; cellX++) {
        for (s32 cellZ = minCellZ; cellZ <= maxCellZ; cellZ++) {
            struct WallCollisionCacheCell *cell =
                &cache
                     ->cells[(cellX - cache->minCellX) * cache->cellCountZ + (cellZ - cache->minCellZ)];
            if (!(gCollisionFlags & COLLISION_FLAG_EXCLUDE_DYNAMIC)) {
                numCollisions += find_wall_collisions_from_list(surface_query_list(cell->dynamicWalls),
                                                                colData, cache, &leftCache);
                if (leftCache) {
                    break;
                }
            }
            numCollisions += find_wall_collisions_from_list(surface_query_list(cell->staticWalls),
                                                            colData, cache, &leftCache);
            if (leftCache) {
                break;
            }
        }
        if (leftCache) {
            break;
        }
    }

    if (leftCache) {
        // The filtered lists omitted walls outside the region. Redo this
        // query from its original position and in the original wall order.
        colData->x = originalX;
        colData->z = originalZ;
        colData->numWalls = 0;
        numCollisions =
            dispatch_wall_collisions_in_cells(colData, minCellX, maxCellX, minCellZ, maxCellZ);
    }

    gCollisionFlags &= ~(COLLISION_FLAG_RETURN_FIRST | COLLISION_FLAG_EXCLUDE_DYNAMIC
                         | COLLISION_FLAG_INCLUDE_INTANGIBLE);
#ifdef VANILLA_DEBUG
    gNumCalls.wall++;
#endif
    profiler_collision_update(first);
    return numCollisions;
}

/**
 * Collides with walls and returns the most recent wall.
 */
void resolve_and_return_wall_collisions(Vec3f pos, f32 offset, f32 radius, struct WallCollisionData *collisionData) {
    collisionData->x = pos[0];
    collisionData->y = pos[1];
    collisionData->z = pos[2];
    collisionData->radius = radius;
    collisionData->offsetY = offset;

    find_wall_collisions(collisionData);

    pos[0] = collisionData->x;
    pos[1] = collisionData->y;
    pos[2] = collisionData->z;
}

void resolve_and_return_wall_collisions_cached(Vec3f pos, f32 offset, f32 radius,
                                               struct WallCollisionData *collisionData,
                                               struct WallCollisionCache *cache) {
    collisionData->x = pos[0];
    collisionData->y = pos[1];
    collisionData->z = pos[2];
    collisionData->radius = radius;
    collisionData->offsetY = offset;

    find_wall_collisions_with_cache(collisionData, cache);

    pos[0] = collisionData->x;
    pos[1] = collisionData->y;
    pos[2] = collisionData->z;
}

/**************************************************
 *                     CEILINGS                   *
 **************************************************/

void add_ceil_margin(s32 *x, s32 *z, Vec3s target1, Vec3s target2, f32 margin) {
    register f32 diff_x = target1[0] - *x + target2[0] - *x;
    register f32 diff_z = target1[2] - *z + target2[2] - *z;
    register f32 invDenom = margin / sqrtf(sqr(diff_x) + sqr(diff_z));

    *x += diff_x * invDenom;
    *z += diff_z * invDenom;
}

static s32 check_within_ceil_triangle_bounds(s32 x, s32 z, struct Surface *surf, f32 margin) {
    s32 addMargin = surf->type != SURFACE_HANGABLE && !FLT_IS_NONZERO(margin);
    Vec3i vx, vz;
    vx[0] = surf->vertex1[0];
    vz[0] = surf->vertex1[2];
    if (addMargin) add_ceil_margin(&vx[0], &vz[0], surf->vertex2, surf->vertex3, margin);

    vx[1] = surf->vertex2[0];
    vz[1] = surf->vertex2[2];
    if (addMargin) add_ceil_margin(&vx[1], &vz[1], surf->vertex3, surf->vertex1, margin);

    // Checking if point is in bounds of the triangle laterally.
    if (((vz[0] - z) * (vx[1] - vx[0]) - (vx[0] - x) * (vz[1] - vz[0])) > 0) return FALSE;

    // Slight optimization by checking these later.
    vx[2] = surf->vertex3[0];
    vz[2] = surf->vertex3[2];
    if (addMargin) add_ceil_margin(&vx[2], &vz[2], surf->vertex1, surf->vertex2, margin);

    if (((vz[1] - z) * (vx[2] - vx[1]) - (vx[1] - x) * (vz[2] - vz[1])) > 0) return FALSE;
    if (((vz[2] - z) * (vx[0] - vx[2]) - (vx[2] - x) * (vz[0] - vz[2])) > 0) return FALSE;

    return TRUE;
}

/**
 * Iterate through the list of ceilings and find the first ceiling over a given point.
 */
static struct Surface *find_ceil_from_list(struct SurfaceQueryIterator candidates, s32 x, s32 y, s32 z,
                                           f32 *pheight) {
    register struct Surface *surf, *ceil = NULL;
    register f32 height;
    SurfaceType type = SURFACE_DEFAULT;
    *pheight = CELL_HEIGHT_LIMIT;
    // Stay in this loop until out of ceilings.
    while ((surf = surface_query_next(&candidates)) != NULL) {
#if COLLISION_PROFILING
        COLLISION_PROFILE_COUNT(COLLISION_PROFILE_CEIL_NODES);
#endif
        type = surf->type;

        // Sloped grates can be classified as ceilings instead of walls.
        if (surface_is_passable_vanish(surf)) {
            continue;
        }

        // Exclude all ceilings below the point
        if (y > surf->upperY) continue;

        // Determine if checking for the camera or not
        if (gCollisionFlags & COLLISION_FLAG_CAMERA) {
            if (surf->flags & SURFACE_FLAG_NO_CAM_COLLISION) {
                continue;
            }
        } else if (type == SURFACE_CAMERA_BOUNDARY) {
            // Ignore camera only surfaces
            continue;
        }

        // Check that the point is within the triangle bounds
        if (!check_within_ceil_triangle_bounds(x, z, surf, 1.5f)) continue;

        // Find the height of the ceil at the given location
        height = get_surface_height_at_location(x, z, surf);

        // Exclude ceilings above the previous lowest ceiling
        if (height > *pheight) continue;

        // Checks for ceiling interaction
        if (y > height) continue;


        // Use the current ceiling
        *pheight = height;
        ceil = surf;

        // Exit the loop if it's not possible for another ceiling to be closer
        // to the original point, or if COLLISION_FLAG_RETURN_FIRST.
        if (height == y || (gCollisionFlags & COLLISION_FLAG_RETURN_FIRST)) break;
    }
    return ceil;
}

/**
 * Find the lowest ceiling above a given position and return the height.
 */
f32 find_ceil(f32 posX, f32 posY, f32 posZ, struct Surface **pceil) {
    f32 height        = CELL_HEIGHT_LIMIT;
    f32 dynamicHeight = CELL_HEIGHT_LIMIT;
    PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.collision_ceil);
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();
    s32 x = posX;
    s32 y = posY;
    s32 z = posZ;
    *pceil = NULL;

    if (is_outside_level_bounds(x, z)) {
        profiler_collision_update(first);
        return height;
    }

    // Each level is split into cells to limit load, find the appropriate cell.
    s32 cellX = GET_CELL_COORD(x);
    s32 cellZ = GET_CELL_COORD(z);

    struct SurfaceQueryIterator surfaceList;
    struct Surface *ceil = NULL;
    struct Surface *dynamicCeil = NULL;

    s32 includeDynamic = !(gCollisionFlags & COLLISION_FLAG_EXCLUDE_DYNAMIC);

    if (includeDynamic) {
        // Check for surfaces belonging to objects.
        surfaceList = surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_CEILS, SURFACE_QUERY_DYNAMIC);
        dynamicCeil = find_ceil_from_list(surfaceList, x, y, z, &dynamicHeight);

        // In the next check, only check for ceilings lower than the previous check.
        height = dynamicHeight;
    }

    // Check for surfaces that are a part of level geometry.
    surfaceList = surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_CEILS, SURFACE_QUERY_STATIC);
    ceil = find_ceil_from_list(surfaceList, x, y, z, &height);

    // Use the lower ceiling.
    if (includeDynamic && height >= dynamicHeight) {
        ceil   = dynamicCeil;
        height = dynamicHeight;
    }

    // To prevent accidentally leaving the floor tangible, stop checking for it.
    gCollisionFlags &= ~(COLLISION_FLAG_RETURN_FIRST | COLLISION_FLAG_EXCLUDE_DYNAMIC | COLLISION_FLAG_INCLUDE_INTANGIBLE);

    // Return the ceiling.
    *pceil = ceil;
#ifdef VANILLA_DEBUG
    // Increment the debug tracker.
    gNumCalls.ceil++;
#endif

    profiler_collision_update(first);
    return height;
}

/**************************************************
 *                     FLOORS                     *
 **************************************************/

static s32 check_within_floor_triangle_bounds(s32 x, s32 z, struct Surface *surf) {
    Vec3i vx, vz;
    vx[0] = surf->vertex1[0];
    vz[0] = surf->vertex1[2];
    vx[1] = surf->vertex2[0];
    vz[1] = surf->vertex2[2];

    if (((vz[0] - z) * (vx[1] - vx[0]) - (vx[0] - x) * (vz[1] - vz[0])) < 0) return FALSE;

    vx[2] = surf->vertex3[0];
    vz[2] = surf->vertex3[2];

    if (((vz[1] - z) * (vx[2] - vx[1]) - (vx[1] - x) * (vz[2] - vz[1])) < 0) return FALSE;
    if (((vz[2] - z) * (vx[0] - vx[2]) - (vx[2] - x) * (vz[0] - vz[2])) < 0) return FALSE;
    return TRUE;
}

// Round upward once per selected floor, keeping the sorted-list cutoff integral.
// Extreme seeds conservatively retain candidates without an out-of-range cast.
static ALWAYS_INLINE s32 floor_height_cutoff(f32 height) {
    if (!(height > -2147483648.0f)) {
        return (-2147483647 - 1);
    }
    if (height >= 2147483648.0f) {
        return 2147483647;
    }
    s32 cutoff = (s32) height;
    if ((f32) cutoff < height) {
        cutoff++;
    }
    return cutoff;
}

/**
 * Find the highest eligible floor under a given point.
 */
static struct Surface *find_floor_from_list(struct SurfaceQueryIterator candidates, s32 x, s32 y, s32 z,
                                            f32 *pheight) {
    register struct Surface *surf, *floor = NULL;
    register SurfaceType type = SURFACE_DEFAULT;
    register f32 height;
    register s32 bufferY = y + FIND_FLOOR_BUFFER;
    s32 minimumUpperY = floor_height_cutoff(*pheight);

    // Iterate through the list of floors until there are no more floors.
    while ((surf = surface_query_next(&candidates)) != NULL) {
#if COLLISION_PROFILING
        COLLISION_PROFILE_COUNT(COLLISION_PROFILE_FLOOR_NODES);
#endif
        // Reject floors above the query before loading their other fields.
        if (bufferY < surf->lowerY) {
            continue;
        }
        // Floors are sorted by upperY. No later floor can beat the height
        // already found, including a floor found in the dynamic partition.
        if (surf->upperY < minimumUpperY) {
            break;
        }
        type        = surf->type;

        if (surface_is_passable_vanish(surf)) {
            continue;
        }

        // To prevent the Merry-Go-Round room from loading when Mario passes above the hole that leads
        // there, SURFACE_INTANGIBLE is used. This prevent the wrong room from loading, but can also allow
        // Mario to pass through.
        if (!(gCollisionFlags & COLLISION_FLAG_INCLUDE_INTANGIBLE) && (type == SURFACE_INTANGIBLE)) {
            continue;
        }

        // Determine if we are checking for the camera or not.
        if (gCollisionFlags & COLLISION_FLAG_CAMERA) {
            if (surf->flags & SURFACE_FLAG_NO_CAM_COLLISION) {
                continue;
            }
        } else if (type == SURFACE_CAMERA_BOUNDARY) {
            continue; // If we are not checking for the camera, ignore camera only floors.
        }

        // Check that the point is within the triangle bounds.
        if (!check_within_floor_triangle_bounds(x, z, surf)) continue;

        // Get the height of the floor under the current location.
        height = get_surface_height_at_location(x, z, surf);

        // Exclude floors lower than the previous highest floor.
        if (height <= *pheight) continue;

        // Checks for floor interaction with a FIND_FLOOR_BUFFER unit buffer.
        if (bufferY < height) continue;

        // Use the current floor
        *pheight = height;
        floor = surf;
        minimumUpperY = floor_height_cutoff(height);

        // Exit the loop if it's not possible for another floor to be closer
        // to the original point, or if COLLISION_FLAG_RETURN_FIRST.
        if ((height == bufferY) || (gCollisionFlags & COLLISION_FLAG_RETURN_FIRST)) break;
    }
    return floor;
}

#if COLLISION_RESULT_CACHES
#define STATIC_FLOOR_CACHE_SIZE 8

struct StaticFloorCacheEntry {
    s32 x, y, z;
    u32 frame, generation;
    const void *list;
    struct Surface *floor;
    f32 minimumHeight, height;
    s32 ignoreGrates;
};

static struct StaticFloorCacheEntry sStaticFloorCache[STATIC_FLOOR_CACHE_SIZE];
#endif

static struct Surface *find_static_floor_from_list(struct SurfaceQueryIterator candidates, s32 x, s32 y,
                                                   s32 z, f32 *height) {
#if COLLISION_RESULT_CACHES
    // Short lists already cost little. Special queries retain their native
    // ordering and one-shot flag behavior, without populating this cache.
    if (!surface_query_has_multiple(candidates) || gCollisionFlags != 0) {
        return find_floor_from_list(candidates, x, y, z, height);
    }

    const void *list = surface_query_identity(candidates);
    s32 ignoreGrates = o != NULL
                       && ((o->activeFlags & ACTIVE_FLAG_MOVE_THROUGH_GRATE)
                           || (o == gMarioObject && (gMarioState->flags & MARIO_VANISH_CAP)));
    ignoreGrates |= (o == gMarioObject) << 1;
    // Fold higher coordinate bits into the slot: aligned placements should
    // not repeatedly evict each other merely because their low bits match.
    u32 hash = (u32) x ^ ((u32) z << 1) ^ ((u32) y << 2);
    hash ^= hash >> 7;
    hash ^= hash >> 3;
    u32 slot = hash & (STATIC_FLOOR_CACHE_SIZE - 1);
    struct StaticFloorCacheEntry *entry = &sStaticFloorCache[slot];

    // Dynamic floors are still searched every time. Their winning height is
    // part of the key because it changes how the static list is searched.
    // Terrain loads and late static object publication advance the generation.
    if (entry->list == list && entry->frame == gGlobalTimer
        && entry->generation == gStaticSurfaceGeneration && entry->x == x && entry->y == y
        && entry->z == z && entry->minimumHeight == *height && entry->ignoreGrates == ignoreGrates) {
        *height = entry->height;
        return entry->floor;
    }

    entry->minimumHeight = *height;
    entry->floor = find_floor_from_list(candidates, x, y, z, height);
    entry->height = *height;
    entry->list = list;
    entry->frame = gGlobalTimer;
    entry->generation = gStaticSurfaceGeneration;
    entry->x = x;
    entry->y = y;
    entry->z = z;
    entry->ignoreGrates = ignoreGrates;
    return entry->floor;
#else
    return find_floor_from_list(candidates, x, y, z, height);
#endif
}

// Generic triangle bounds func
ALWAYS_INLINE static s32 check_within_bounds_y_norm(s32 x, s32 z, struct Surface *surf) {
    if (surf->normal.y >= NORMAL_FLOOR_THRESHOLD) return check_within_floor_triangle_bounds(x, z, surf);
    return check_within_ceil_triangle_bounds(x, z, surf, 0);
}

/**
 * Iterate through the list of water floors and find the first water floor under a given point.
 */
static struct Surface *find_water_floor_from_list(struct SurfaceQueryIterator candidates, s32 x, s32 y,
                                                  s32 z, f32 *pheight) {
    register struct Surface *surf;
    struct Surface *floor = NULL;
    struct SurfaceQueryIterator tops = candidates;
    struct SurfaceQueryIterator bottoms = candidates;
    f32 height = FLOOR_LOWER_LIMIT;
    f32 curHeight = FLOOR_LOWER_LIMIT;
    f32 bottomHeight = FLOOR_LOWER_LIMIT;
    f32 curBottomHeight = FLOOR_LOWER_LIMIT;
    f32 buffer = FIND_FLOOR_BUFFER;

    // Iterate through the list of water floors until there are no more water floors.
    // SURFACE_NEW_WATER_BOTTOM
    while ((surf = surface_query_next(&bottoms)) != NULL) {
#if COLLISION_PROFILING
        COLLISION_PROFILE_COUNT(COLLISION_PROFILE_WATER_NODES);
#endif

        // skip wall angled water
        if (surf->type != SURFACE_NEW_WATER_BOTTOM || absf(surf->normal.y) < NORMAL_FLOOR_THRESHOLD) continue;

        if (!check_within_bounds_y_norm(x, z, surf)) continue;

        curBottomHeight = get_surface_height_at_location(x, z, surf);

        if (curBottomHeight < y + buffer) {
            continue;
        } else {
            bottomHeight = curBottomHeight;
        }
    }

    // Iterate through the list of water tops until there are no more water tops.
    // SURFACE_NEW_WATER
    while ((surf = surface_query_next(&tops)) != NULL) {
#if COLLISION_PROFILING
        COLLISION_PROFILE_COUNT(COLLISION_PROFILE_WATER_NODES);
#endif

        // skip water tops or wall angled water bottoms
        if (surf->type == SURFACE_NEW_WATER_BOTTOM || absf(surf->normal.y) < NORMAL_FLOOR_THRESHOLD) continue;

        if (!check_within_bounds_y_norm(x, z, surf)) continue;

        curHeight = get_surface_height_at_location(x, z, surf);

        if (bottomHeight != FLOOR_LOWER_LIMIT && curHeight > bottomHeight) continue;

        if (curHeight > height) {
            height = curHeight;
            *pheight = curHeight;
            floor = surf;
        }
    }

    return floor;
}

/**
 * Find the height of the highest floor below a point.
 */
f32 find_floor_excluding_objects(f32 x, f32 y, f32 z, struct Surface **floor, struct Object *owner,
                                 struct Object *attachment) {
    PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.collision_floor);
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();
    f32 height = FLOOR_LOWER_LIMIT;
    *floor = NULL;
    if (is_outside_level_bounds(x, z)) {
        profiler_collision_update(first);
        return height;
    }
    s32 cellX = GET_CELL_COORD(x);
    s32 cellZ = GET_CELL_COORD(z);
    for (s32 dynamic = 0; dynamic < 2; dynamic++) {
        struct SurfaceQueryIterator candidates =
            surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_FLOORS,
                               dynamic ? SURFACE_QUERY_DYNAMIC : SURFACE_QUERY_STATIC);
        struct Surface *surface;
        while ((surface = surface_query_next(&candidates)) != NULL) {
#if COLLISION_PROFILING
            COLLISION_PROFILE_COUNT(COLLISION_PROFILE_FLOOR_NODES);
#endif
            if ((owner != NULL && surface->object == owner)
                || (attachment != NULL && surface->object == attachment)
                || surface->type == SURFACE_INTANGIBLE || surface->type == SURFACE_CAMERA_BOUNDARY
                || (surface->type == SURFACE_VANISH_CAP_WALLS && owner != NULL
                    && (owner->activeFlags & ACTIVE_FLAG_MOVE_THROUGH_GRATE))
                || !check_within_floor_triangle_bounds((s32) x, (s32) z, surface)) {
                continue;
            }
            f32 candidate = get_surface_height_at_location(x, z, surface);
            if (candidate > height && candidate <= y + FIND_FLOOR_BUFFER) {
                height = candidate;
                *floor = surface;
            }
        }
    }
    profiler_collision_update(first);
    return height;
}

f32 find_floor_height(f32 x, f32 y, f32 z) {
    struct Surface *floor;
    return find_floor(x, y, z, &floor);
}

/**
 * Find the highest dynamic floor under a given position. Perhaps originally static
 * and dynamic floors were checked separately.
 */
f32 unused_find_dynamic_floor(f32 xPos, f32 yPos, f32 zPos, struct Surface **pfloor) {
    f32 floorHeight = FLOOR_LOWER_LIMIT;

    // Would normally cause PUs, but dynamic floors unload at that range.
    s32 x = xPos;
    s32 y = yPos;
    s32 z = zPos;

    // Each level is split into cells to limit load, find the appropriate cell.
    s32 cellX = GET_CELL_COORD(x);
    s32 cellZ = GET_CELL_COORD(z);

    struct SurfaceQueryIterator surfaceList =
        surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_FLOORS, SURFACE_QUERY_DYNAMIC);

    *pfloor = find_floor_from_list(surfaceList, x, y, z, &floorHeight);

    return floorHeight;
}

/**
 * Find the highest floor under a given position and return the height.
 */
f32 find_floor(f32 xPos, f32 yPos, f32 zPos, struct Surface **pfloor) {
    PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.collision_floor);
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();

    f32 height        = FLOOR_LOWER_LIMIT;
    f32 dynamicHeight = FLOOR_LOWER_LIMIT;

    //! (Parallel Universes) Because position is casted to an s16, reaching higher
    //  float locations can return floors despite them not existing there.
    //  (Dynamic floors will unload due to the range.)
    s32 x = xPos;
    s32 y = yPos;
    s32 z = zPos;

    *pfloor = NULL;

    if (is_outside_level_bounds(x, z)) {
        profiler_collision_update(first);
        return height;
    }
    // Each level is split into cells to limit load, find the appropriate cell.
    s32 cellX = GET_CELL_COORD(x);
    s32 cellZ = GET_CELL_COORD(z);

    struct SurfaceQueryIterator surfaceList;
    struct Surface *floor = NULL;
    struct Surface *dynamicFloor = NULL;

    s32 includeDynamic = !(gCollisionFlags & COLLISION_FLAG_EXCLUDE_DYNAMIC);

    if (includeDynamic) {
        // Check for surfaces belonging to objects.
        surfaceList = surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_FLOORS, SURFACE_QUERY_DYNAMIC);
        dynamicFloor = find_floor_from_list(surfaceList, x, y, z, &dynamicHeight);

        // In the next check, only check for floors higher than the previous check.
        height = dynamicHeight;
    }

    // Check for surfaces that are a part of level geometry.
    surfaceList = surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_FLOORS, SURFACE_QUERY_STATIC);
    floor = find_static_floor_from_list(surfaceList, x, y, z, &height);

    // Use the higher floor.
    if (includeDynamic && height <= dynamicHeight) {
        floor  = dynamicFloor;
        height = dynamicHeight;
    }

    // To prevent accidentally leaving the floor tangible, stop checking for it.
    gCollisionFlags &= ~(COLLISION_FLAG_RETURN_FIRST | COLLISION_FLAG_EXCLUDE_DYNAMIC | COLLISION_FLAG_INCLUDE_INTANGIBLE);
    // If a floor was missed, increment the debug counter.
    if (floor == NULL) {
        gNumFindFloorMisses++;
    }

    // Return the floor.
    *pfloor = floor;
#ifdef VANILLA_DEBUG
    // Increment the debug tracker.
    gNumCalls.floor++;
#endif

    profiler_collision_update(first);
    return height;
}

// Swept contacts are fractional. Rounding only the triangle selection and then
// evaluating its plane at the real position can extend a neighboring slope
// beyond its edge, making a downhill seam act like an invisible step.
static s32 check_within_surface_triangle_bounds_f32(f32 x, f32 z, struct Surface *surface) {
    Vec3s *vertices[3] = { &surface->vertex1, &surface->vertex2, &surface->vertex3 };
    for (s32 i = 0; i < 3; i++) {
        s16 *a = *vertices[i];
        s16 *b = *vertices[(i + 1) % 3];
        f32 edge = (a[2] - z) * (b[0] - a[0]) - (a[0] - x) * (b[2] - a[2]);
        if (surface->normal.y > 0.0f ? edge < 0.0f : edge > 0.0f) {
            return FALSE;
        }
    }
    return TRUE;
}

static f32 floor_height_at_xz(f32 x, f32 z, const struct Surface *surface) {
    // Anchor at a vertex to avoid cancellation at large world coordinates.
    return surface->vertex1[1]
           - ((x - surface->vertex1[0]) * surface->normal.x
              + (z - surface->vertex1[2]) * surface->normal.z)
                 / surface->normal.y;
}

void get_air_step_floor_start(const Vec3f pos, struct Surface *floor, f32 floorHeight, Vec3f start) {
    vec3f_copy(start, pos);
    if (floor == NULL || floor->normal.y <= 0.0f || pos[1] != floorHeight
        || !check_within_surface_triangle_bounds_f32((s32) pos[0], (s32) pos[2], floor)) {
        return;
    }
    // Ground queries round X/Z to integers. Use the actual support plane for
    // collision queries only; do not move Mario or grant a below-ledge upwarp.
    // The rounded footprint also preserves contact across coplanar seams.
    start[1] = MAX(start[1], floor_height_at_xz(pos[0], pos[2], floor));
}

static f32 find_vertical_surface_precise(f32 x, f32 y, f32 z, struct Surface **result, s32 ceiling,
                                         const Vec3f airStart) {
    f32 height = ceiling ? CELL_HEIGHT_LIMIT : FLOOR_LOWER_LIMIT;
    f32 limit = ceiling ? y : y + FIND_FLOOR_BUFFER;
    s32 partition = ceiling ? SPATIAL_PARTITION_CEILS : SPATIAL_PARTITION_FLOORS;
    *result = NULL;
    if (is_outside_level_bounds(x, z)) {
        return height;
    }

    s32 cellX = GET_CELL_COORD(x);
    s32 cellZ = GET_CELL_COORD(z);
    struct SurfaceQueryIterator lists[2] = {
        surface_query_cell(cellX, cellZ, partition,
                           (gCollisionFlags & COLLISION_FLAG_EXCLUDE_DYNAMIC) ? SURFACE_QUERY_EMPTY
                                                                              : SURFACE_QUERY_DYNAMIC),
        surface_query_cell(cellX, cellZ, partition, SURFACE_QUERY_STATIC)
    };
    // Match the existing height rejection; X/Z padding keeps edge rounding
    // conservative. Exact fractional triangle tests still select every contact.
    Vec3f minimum = { x - 1.0f, ceiling ? limit : -32768.0f, z - 1.0f };
    Vec3f maximum = { x + 1.0f, ceiling ? 32767.0f : limit, z + 1.0f };
    for (s32 i = 0; i < 2; i++) {
        for (struct Surface *surface = surface_query_next_in_bounds(&lists[i], minimum, maximum);
             surface != NULL; surface = surface_query_next_in_bounds(&lists[i], minimum, maximum)) {
#if COLLISION_PROFILING
            COLLISION_PROFILE_COUNT(ceiling ? COLLISION_PROFILE_CEIL_NODES
                                            : COLLISION_PROFILE_FLOOR_NODES);
#endif
            if (!ceiling && surface->upperY < height) {
                break;
            }
            if (ceiling ? limit > surface->upperY : limit < surface->lowerY) {
                continue;
            }
            if (surface_is_passable_vanish(surface)) {
                continue;
            }
            if (!ceiling && surface->type == SURFACE_INTANGIBLE
                && !(gCollisionFlags & COLLISION_FLAG_INCLUDE_INTANGIBLE)) {
                continue;
            }
            if (gCollisionFlags & COLLISION_FLAG_CAMERA) {
                if (surface->flags & SURFACE_FLAG_NO_CAM_COLLISION) {
                    continue;
                }
            } else if (surface->type == SURFACE_CAMERA_BOUNDARY) {
                continue;
            }
            if (!check_within_surface_triangle_bounds_f32(x, z, surface)) {
                continue;
            }
            f32 candidate = floor_height_at_xz(x, z, surface);
            // A floor already below the destination feet is safe to reference.
            // Its extended plane can be above the takeoff point at a crest.
            // Only a possible landing needs the below-plane rejection.
            if (airStart != NULL && candidate >= y) {
                f32 startPlaneY = floor_height_at_xz(airStart[0], airStart[2], surface);
                if (airStart[1] < startPlaneY) {
                    Vec3f point = { airStart[0], startPlaneY, airStart[2] };
                    Vec3f normal = { surface->normal.x, surface->normal.y, surface->normal.z };
                    f32 rise = object_platform_motion_floor_rise_at(surface->object, point, normal);
                    if (airStart[1] < startPlaneY - rise) {
                        continue;
                    }
                }
            }

            if (ceiling ? (candidate < limit || candidate >= height)
                        : (candidate > limit || candidate <= height)) {
                continue;
            }
            height = candidate;
            *result = surface;
            if (height == limit || (gCollisionFlags & COLLISION_FLAG_RETURN_FIRST)) {
                break;
            }
        }
    }
    gCollisionFlags &= ~(COLLISION_FLAG_RETURN_FIRST | COLLISION_FLAG_EXCLUDE_DYNAMIC
                         | COLLISION_FLAG_INCLUDE_INTANGIBLE);
    return height;
}

static f32 find_vertical_surface_cache_entry(f32 x, f32 y, f32 z, struct Surface **result, s32 ceiling,
                                             struct SurfaceQueryCache *cache) {
    s16 flags = gCollisionFlags;
    for (s32 i = 0; i < cache->count; i++) {
        struct SurfaceQueryCacheEntry *entry = &cache->entries[i];
        if (entry->ceiling == ceiling && entry->flags == flags && entry->position[0] == x
            && entry->position[1] == y && entry->position[2] == z) {
            *result = entry->surface;
            gCollisionFlags = entry->remainingFlags;
            return entry->height;
        }
    }
    f32 height = find_vertical_surface_precise(x, y, z, result, ceiling, NULL);
    struct SurfaceQueryCacheEntry *entry = &cache->entries[cache->next];
    vec3f_set(entry->position, x, y, z);
    entry->surface = *result;
    entry->height = height;
    entry->flags = flags;
    entry->remainingFlags = gCollisionFlags;
    entry->ceiling = ceiling;
    cache->next = (cache->next + 1) % SURFACE_QUERY_CACHE_SIZE;
    cache->count = MIN(cache->count + 1, SURFACE_QUERY_CACHE_SIZE);
    return height;
}

// Keep uncached requests out of the cache lookup's register-saving frame.
static ALWAYS_INLINE f32 find_vertical_surface_cached(f32 x, f32 y, f32 z, struct Surface **result,
                                                      s32 ceiling, struct SurfaceQueryCache *cache) {
    if (cache == NULL || cache->count < 0) {
        return find_vertical_surface_precise(x, y, z, result, ceiling, NULL);
    }
    return find_vertical_surface_cache_entry(x, y, z, result, ceiling, cache);
}

f32 find_floor_precise_cached(f32 x, f32 y, f32 z, struct Surface **floor,
                              struct SurfaceQueryCache *cache) {
    PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.collision_floor);
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();
    f32 height = find_vertical_surface_cached(x, y, z, floor, FALSE, cache);
    if (*floor == NULL) {
        gNumFindFloorMisses++;
    }
#ifdef VANILLA_DEBUG
    gNumCalls.floor++;
#endif
    profiler_collision_update(first);
    return height;
}

f32 find_floor_precise(f32 x, f32 y, f32 z, struct Surface **floor) {
    return find_floor_precise_cached(x, y, z, floor, NULL);
}

// Catch the earliest front-side crossing that an endpoint floor lookup can miss at a ridge.
// queryFlags are captured before that lookup consumes its one-shot flags.
s32 find_air_step_floor_contact(const Vec3f start, const Vec3f next, Vec3f contact,
                                struct Surface **floor, s16 queryFlags) {
    PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.collision_floor);
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();
    s32 minX =
        GET_CELL_COORD(CLAMP(MIN(start[0], next[0]), -LEVEL_BOUNDARY_MAX + 1, LEVEL_BOUNDARY_MAX - 1));
    s32 maxX =
        GET_CELL_COORD(CLAMP(MAX(start[0], next[0]), -LEVEL_BOUNDARY_MAX + 1, LEVEL_BOUNDARY_MAX - 1));
    s32 minZ =
        GET_CELL_COORD(CLAMP(MIN(start[2], next[2]), -LEVEL_BOUNDARY_MAX + 1, LEVEL_BOUNDARY_MAX - 1));
    s32 maxZ =
        GET_CELL_COORD(CLAMP(MAX(start[2], next[2]), -LEVEL_BOUNDARY_MAX + 1, LEVEL_BOUNDARY_MAX - 1));
    f32 nearest = 2.0f;
    *floor = NULL;
    for (s32 z = minZ; z <= maxZ; z++) {
        for (s32 x = minX; x <= maxX; x++) {
            struct SurfaceQueryIterator lists[2] = {
                surface_query_cell(x, z, SPATIAL_PARTITION_FLOORS, SURFACE_QUERY_STATIC),
                surface_query_cell(x, z, SPATIAL_PARTITION_FLOORS,
                                   (queryFlags & COLLISION_FLAG_EXCLUDE_DYNAMIC)
                                       ? SURFACE_QUERY_EMPTY
                                       : SURFACE_QUERY_DYNAMIC)
            };
            for (s32 list = 0; list < 2; list++) {
                struct Surface *surface;
                while ((surface = surface_query_next(&lists[list])) != NULL) {
#if COLLISION_PROFILING
                    COLLISION_PROFILE_COUNT(COLLISION_PROFILE_FLOOR_NODES);
#endif
                    if ((list == 0 && surface->lowerY > MAX(start[1], next[1]))
                        || surface->upperY < MIN(start[1], next[1])
                        || surface_is_passable_vanish(surface)) {
                        continue;
                    }
                    if (surface->type == SURFACE_INTANGIBLE
                        && !(queryFlags & COLLISION_FLAG_INCLUDE_INTANGIBLE)) {
                        continue;
                    }
                    if (queryFlags & COLLISION_FLAG_CAMERA) {
                        if (surface->flags & SURFACE_FLAG_NO_CAM_COLLISION) {
                            continue;
                        }
                    } else if (surface->type == SURFACE_CAMERA_BOUNDARY) {
                        continue;
                    }
                    // Use the same plane evaluation as the supported start so
                    // a grounded origin is exactly on the plane, not behind it.
                    f32 startHeight = floor_height_at_xz(start[0], start[2], surface);
                    f32 from = start[1] - startHeight;
                    f32 to = next[1] - floor_height_at_xz(next[0], next[2], surface);
                    if (to > 0.0f) {
                        continue;
                    }
                    f32 fraction;
                    if (from < 0.0f) {
                        // Platforms update before Mario. Catch a top that has
                        // already risen through his feet only when its previous
                        // plane was below him and his footprint is on this face.
                        if (list == 0
                            || !check_within_surface_triangle_bounds_f32(start[0], start[2], surface)) {
                            continue;
                        }
                        Vec3f point = { start[0], startHeight, start[2] };
                        Vec3f normal = { surface->normal.x, surface->normal.y, surface->normal.z };
                        f32 rise = object_platform_motion_floor_rise_at(surface->object, point, normal);
                        if (rise <= 0.0f || from < -rise) {
                            continue;
                        }
                        fraction = 0.0f;
                    } else {
                        if (from <= to) {
                            continue;
                        }
                        fraction = from / (from - to);
                    }
                    if (fraction >= nearest) {
                        continue;
                    }
                    Vec3f candidate;
                    for (s32 axis = 0; axis < 3; axis++) {
                        candidate[axis] = start[axis] + (next[axis] - start[axis]) * fraction;
                    }
                    if (from < 0.0f) {
                        candidate[1] = startHeight;
                    }
                    if (is_outside_level_bounds(candidate[0], candidate[2])
                        || !check_within_surface_triangle_bounds_f32(candidate[0], candidate[2],
                                                                     surface)) {
                        continue;
                    }
                    vec3f_copy(contact, candidate);
                    *floor = surface;
                    nearest = fraction;
                }
            }
        }
    }
    profiler_collision_update(first);
    return *floor != NULL;
}

f32 find_floor_for_air_step_with_obstacle(const Vec3f startPos, const Vec3f nextPos,
                                          struct Surface **floor, struct Surface **obstacle) {
    s16 collisionFlags = gCollisionFlags;
    f32 height = find_floor(nextPos[0], nextPos[1], nextPos[2], floor);
    if (obstacle != NULL) {
        *obstacle = NULL;
    }
    if (*floor != NULL && height > startPos[1]) {
        struct Surface *bufferedFloor = *floor;
        if (obstacle != NULL
            && !check_within_surface_triangle_bounds_f32(nextPos[0], nextPos[2], bufferedFloor)) {
            // Integer X/Z can choose the other half of a tread. Remap only
            // this rare rejected candidate to the actual finite footprint.
            gCollisionFlags = collisionFlags;
            find_floor_precise(nextPos[0], nextPos[1], nextPos[2], &bufferedFloor);
        }
        // The height buffer can select a ledge Mario has already fallen below.
        // Keep real plane crossings (including slopes), but reject side upwarps.
        gCollisionFlags = collisionFlags;
        PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.collision_floor);
        PUPPYPRINT_GET_COLLISION_SNAPSHOT();
        height =
            find_vertical_surface_precise(nextPos[0], nextPos[1], nextPos[2], floor, FALSE, startPos);
        if (obstacle != NULL && bufferedFloor != NULL && bufferedFloor != *floor
            && check_within_surface_triangle_bounds_f32(nextPos[0], nextPos[2], bufferedFloor)
            && floor_height_at_xz(nextPos[0], nextPos[2], bufferedFloor) > height) {
            *obstacle = bufferedFloor;
        }
        if (*floor == NULL) {
            gNumFindFloorMisses++;
        }
        profiler_collision_update(first);
    }
    return height;
}

f32 find_floor_for_air_step(const Vec3f startPos, const Vec3f nextPos, struct Surface **floor) {
    return find_floor_for_air_step_with_obstacle(startPos, nextPos, floor, NULL);
}

f32 find_ceil_precise_cached(f32 x, f32 y, f32 z, struct Surface **ceil,
                             struct SurfaceQueryCache *cache) {
    PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.collision_ceil);
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();
    f32 height = find_vertical_surface_cached(x, y, z, ceil, TRUE, cache);
#ifdef VANILLA_DEBUG
    gNumCalls.ceil++;
#endif
    profiler_collision_update(first);
    return height;
}

f32 find_ceil_precise(f32 x, f32 y, f32 z, struct Surface **ceil) {
    return find_ceil_precise_cached(x, y, z, ceil, NULL);
}

f32 find_room_floor(f32 x, f32 y, f32 z, struct Surface **pfloor) {
    gCollisionFlags |= (COLLISION_FLAG_EXCLUDE_DYNAMIC | COLLISION_FLAG_INCLUDE_INTANGIBLE);

    return find_floor(x, y, z, pfloor);
}

/**
 * Get the room index at a given position.
 */
s32 get_room_at_pos(f32 x, f32 y, f32 z) {
    if (gCurrentArea->surfaceRooms != NULL) {
        struct Surface *floor;

        find_room_floor(x, y, z, &floor);

        if (floor != NULL) {
            return floor->room;
        }
    }

    return -1;
}

/**
 * Find the highest water floor under a given position and return the height.
 */
f32 find_water_floor(s32 xPos, s32 yPos, s32 zPos, struct Surface **pfloor) {
    f32 height = FLOOR_LOWER_LIMIT;

    s32 x = xPos;
    s32 y = yPos;
    s32 z = zPos;

    if (is_outside_level_bounds(x, z)) return height;

    // Each level is split into cells to limit load, find the appropriate cell.
    s32 cellX = GET_CELL_COORD(x);
    s32 cellZ = GET_CELL_COORD(z);

    // Check for surfaces that are a part of level geometry.
    struct SurfaceQueryIterator surfaceList =
        surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_WATER, SURFACE_QUERY_STATIC);
    struct Surface     *floor       = find_water_floor_from_list(surfaceList, x, y, z, &height);

    if (floor == NULL) {
        height = FLOOR_LOWER_LIMIT;
    } else {
        *pfloor = floor;
    }
#ifdef VANILLA_DEBUG
    // Increment the debug tracker.
    gNumCalls.floor++;
#endif
    return height;
}

/**************************************************
 *               ENVIRONMENTAL BOXES              *
 **************************************************/

/**
 * Finds the height of water at a given location.
 */
s32 find_water_level_and_floor(s32 x, s32 y, s32 z, struct Surface **pfloor) {
    if (!water_cell_may_contain_water(x, z)) {
        // Match the old no-region path, which cleared the output floor.
        if (gEnvironmentRegions == NULL) {
            *pfloor = NULL;
        }
        return FLOOR_LOWER_LIMIT;
    }

    s32 val;
    s32 loX, hiX, loZ, hiZ;
    TerrainData *p = gEnvironmentRegions;
    struct Surface *floor = NULL;
    PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.collision_water);
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();
    s32 waterLevel = find_water_floor(x, y, z, &floor);

    if (p != NULL && waterLevel == FLOOR_LOWER_LIMIT) {
        s32 numRegions = *p++;

        for (s32 i = 0; i < numRegions; i++) {
            val = *p++;
            loX = *p++;
            loZ = *p++;
            hiX = *p++;
            hiZ = *p++;

            // If the location is within a water box and it is a water box.
            // Water is less than 50 val only, while above is gas and such.
            if (loX < x && x < hiX && loZ < z && z < hiZ && val < 50) {
                // Set the water height. Since this breaks, only return the first height.
                waterLevel = *p;
                break;
            }
            p++;
        }
    } else {
        *pfloor = floor;
    }

    profiler_collision_update(first);
    return waterLevel;
}

/**
 * Finds the height of water at a given location.
 */
s32 find_water_level(s32 x, s32 z) { // HACKERSM64_DO: Allow y pos
    if (!water_cell_may_contain_water(x, z)) {
        return FLOOR_LOWER_LIMIT;
    }

    s32 val;
    s32 loX, hiX, loZ, hiZ;
    TerrainData *p = gEnvironmentRegions;
    struct Surface *floor = NULL;
    PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.collision_water);
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();
    s32 waterLevel = find_water_floor(x, ((gCollisionFlags & COLLISION_FLAG_CAMERA) ? gLakituState.pos[1] : gMarioState->pos[1]), z, &floor);

    if ((p != NULL) && (waterLevel == FLOOR_LOWER_LIMIT)) {
        s32 numRegions = *p++;

        for (s32 i = 0; i < numRegions; i++) {
            val = *p++;
            loX = *p++;
            loZ = *p++;
            hiX = *p++;
            hiZ = *p++;

            // If the location is within a water box and it is a water box.
            // Water is less than 50 val only, while above is gas and such.
            if (loX <= x && x <= hiX && loZ <= z && z <= hiZ && val < 50) {
                // Set the water height. Since this breaks, only return the first height.
                waterLevel = *p;
                break;
            }
            p++;
        }
    }

    profiler_collision_update(first);

    return waterLevel;
}

/**
 * Finds the height of the poison gas (used only in HMC) at a given location.
 */
s32 find_poison_gas_level(s32 x, s32 z) {
    s32 val;
    s32 loX, hiX, loZ, hiZ;
    s32 gasLevel = FLOOR_LOWER_LIMIT;
    TerrainData *p = gEnvironmentRegions;
    PUPPYPRINT_ADD_COUNTER(gPuppyCallCounter.collision_water);
    PUPPYPRINT_GET_COLLISION_SNAPSHOT();

    if (p != NULL) {
        s32 numRegions = *p++;

        for (s32 i = 0; i < numRegions; i++) {
            val = *p;

            if (val >= 50) {
                loX = p[1];
                loZ = p[2];
                hiX = p[3];
                hiZ = p[4];

                // If the location is within a gas's box and it is a gas box.
                // Gas has a value of 50, 60, etc.
                if (loX < x && x < hiX && loZ < z && z < hiZ && val % 10 == 0) {
                    // Set the gas height. Since this breaks, only return the first height.
                    gasLevel = p[5];
                    break;
                }
            }

            p += 6;
        }
    }
    
    profiler_collision_update(first);
    return gasLevel;
}

/**************************************************
 *                      DEBUG                     *
 **************************************************/

#ifdef VANILLA_DEBUG
/**
 * Finds the length of a surface list for debug purposes.
 */
static s32 surface_list_length(struct SurfaceNode *list) {
    s32 count = 0;
    while (list != NULL) {
        list = list->next;
        count++;
    }
    return count;
}

/**
 * Print the area,number of walls, how many times they were called,
 * and some allocation information.
 */
void debug_surface_list_info(f32 xPos, f32 zPos) {
    struct SurfaceNode *list;
    s32 numFloors = 0;
    s32 numWalls  = 0;
    s32 numCeils  = 0;

    s32 cellX = GET_CELL_COORD(xPos);
    s32 cellZ = GET_CELL_COORD(zPos);

    list = gStaticSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_FLOORS];
    numFloors += surface_list_length(list);

    list = gDynamicSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_FLOORS];
    numFloors += surface_list_length(list);

    list = gStaticSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_WALLS];
    numWalls += surface_list_length(list);

    list = gDynamicSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_WALLS];
    numWalls += surface_list_length(list);

    list = gStaticSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_CEILS];
    numCeils += surface_list_length(list);

    list = gDynamicSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_CEILS];
    numCeils += surface_list_length(list);

    print_debug_top_down_mapinfo("area   %x", cellZ * NUM_CELLS + cellX);

    // Names represent ground, walls, and roofs as found in SMS.
    print_debug_top_down_mapinfo("dg %d", numFloors);
    print_debug_top_down_mapinfo("dw %d", numWalls);
    print_debug_top_down_mapinfo("dr %d", numCeils);

    set_text_array_x_y(80, -3);

    print_debug_top_down_mapinfo("%d", gNumCalls.floor);
    print_debug_top_down_mapinfo("%d", gNumCalls.wall);
    print_debug_top_down_mapinfo("%d", gNumCalls.ceil);

    set_text_array_x_y(-80, 0);

    // listal- List Allocated?, statbg- Static Background?, movebg- Moving Background?
    print_debug_top_down_mapinfo("listal %d", gSurfaceNodesAllocated);
    print_debug_top_down_mapinfo("statbg %d", gNumStaticSurfaces);
    print_debug_top_down_mapinfo("movebg %d", (gSurfacesAllocated - gNumStaticSurfaces));

    gNumCalls.floor = 0;
    gNumCalls.ceil = 0;
    gNumCalls.wall = 0;
}
#endif

/**
 * An unused function that finds and interacts with any type of surface.
 * Perhaps an original implementation of surfaces before they were more specialized.
 */
s32 unused_resolve_floor_or_ceil_collisions(s32 checkCeil, f32 *px, f32 *py, f32 *pz, f32 radius,
                                            struct Surface **psurface, f32 *surfaceHeight) {
    f32 x = *px;
    f32 y = *py;
    f32 z = *pz;

    *psurface = NULL;

    if (checkCeil) {
        *surfaceHeight = find_ceil(x, y, z, psurface);
    } else {
        *surfaceHeight = find_floor(x, y, z, psurface);
    }

    if (*psurface == NULL) return -1;

    f32 nx = (*psurface)->normal.x;
    f32 ny = (*psurface)->normal.y;
    f32 nz = (*psurface)->normal.z;
    f32 oo = (*psurface)->originOffset;

    f32 offset = absf((nx * x) + (ny * y) + (nz * z) + oo);

    // Interesting surface interaction that should be surf type independent.
    if (offset < radius) {
        offset = (radius - offset);
        *px += (nx * offset);
        *py += (ny * offset);
        *pz += (nz * offset);

        return 1;
    }

    return 0;
}
