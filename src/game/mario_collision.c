#include <ultra64.h>

#include "sm64.h"
#include "game/collision_profile.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "engine/surface_load.h"
#include "engine/surface_query.h"
#include "mario_collision.h"
#include "object_list_processor.h"

// Reject empty stretches of the movement corridor before using the more
// expensive triangle raycasts. The margin covers the normal wall push.
static s32 mario_wall_overlaps_xz_box(struct Surface *wall, f32 minX, f32 maxX, f32 minZ, f32 maxZ) {
    f32 wallMinX = MIN(wall->vertex1[0], MIN(wall->vertex2[0], wall->vertex3[0]));
    f32 wallMaxX = MAX(wall->vertex1[0], MAX(wall->vertex2[0], wall->vertex3[0]));
    f32 wallMinZ = MIN(wall->vertex1[2], MIN(wall->vertex2[2], wall->vertex3[2]));
    f32 wallMaxZ = MAX(wall->vertex1[2], MAX(wall->vertex2[2], wall->vertex3[2]));
    return wallMaxX >= minX && wallMinX <= maxX && wallMaxZ >= minZ && wallMinZ <= maxZ;
}

static s32 mario_wall_list_near_path(struct SurfaceQueryIterator candidates, f32 minX, f32 maxX,
                                     f32 minY, f32 maxY, f32 minZ, f32 maxZ, s32 ignoreVanishWalls) {
    struct Surface *wall;
    while ((wall = surface_query_next(&candidates)) != NULL) {
#if COLLISION_PROFILING
        COLLISION_PROFILE_COUNT(COLLISION_PROFILE_MARIO_PREPARE_NODES);
#endif

        if (wall->type == SURFACE_CAMERA_BOUNDARY
            || (ignoreVanishWalls && wall->type == SURFACE_VANISH_CAP_WALLS)) {
            continue;
        }
        if (wall->upperY < minY || wall->lowerY > maxY) {
            continue;
        }
        if (mario_wall_overlaps_xz_box(wall, minX, maxX, minZ, maxZ)) {
            return TRUE;
        }
    }
    return FALSE;
}

static s32 mario_wall_near_movement_path(struct MarioState *m, f32 dx, f32 dy, f32 dz, f32 lowerOffset,
                                         f32 upperOffset, f32 verticalMargin) {
    const f32 margin = 50.0f;
    f32 minX = MIN(m->pos[0], m->pos[0] + dx) - margin;
    f32 maxX = MAX(m->pos[0], m->pos[0] + dx) + margin;
    f32 minY = MIN(m->pos[1], m->pos[1] + dy) + lowerOffset - verticalMargin;
    f32 maxY = MAX(m->pos[1], m->pos[1] + dy) + upperOffset + verticalMargin;
    f32 minZ = MIN(m->pos[2], m->pos[2] + dz) - margin;
    f32 maxZ = MAX(m->pos[2], m->pos[2] + dz) + margin;
    s32 ignoreVanishWalls =
        (m->flags & MARIO_VANISH_CAP) || (m->marioObj->activeFlags & ACTIVE_FLAG_MOVE_THROUGH_GRATE);
    s32 minCellX, maxCellX, minCellZ, maxCellZ;
    s32 cellX, cellZ;

    // Keep the normal wall checks and raycast near the level boundary.
    if (is_outside_level_bounds(minX, minZ) || is_outside_level_bounds(maxX, maxZ)) {
        return TRUE;
    }
    // Shift before converting to an integer so negative fractional positions
    // beside a cell boundary are assigned to the correct cell.
    minCellX = (s32) ((minX + LEVEL_BOUNDARY_MAX) / CELL_SIZE);
    maxCellX = (s32) ((maxX + LEVEL_BOUNDARY_MAX) / CELL_SIZE);
    minCellZ = (s32) ((minZ + LEVEL_BOUNDARY_MAX) / CELL_SIZE);
    maxCellZ = (s32) ((maxZ + LEVEL_BOUNDARY_MAX) / CELL_SIZE);

    for (cellX = minCellX; cellX <= maxCellX; cellX++) {
        for (cellZ = minCellZ; cellZ <= maxCellZ; cellZ++) {
            if (mario_wall_list_near_path(
                    surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_WALLS, SURFACE_QUERY_STATIC),
                    minX, maxX, minY, maxY, minZ, maxZ, ignoreVanishWalls)
                || mario_wall_list_near_path(
                    surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_WALLS, SURFACE_QUERY_DYNAMIC),
                    minX, maxX, minY, maxY, minZ, maxZ, ignoreVanishWalls)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

#define MARIO_WALL_CACHE_MAX_CELLS 4
#define MARIO_WALL_CACHE_MAX_NODES 256
// Four ordinary walking steps can issue eight wall probes; the candidate
// ratio check below still rejects lists that are too costly to copy.
#define MARIO_WALL_CACHE_MIN_STEPS 4
#define MARIO_WALL_CACHE_Y_MARGIN 64.0f
#define MARIO_WALL_CACHE_XZ_MARGIN 50.0f
#define MARIO_WALL_CACHE_MAX_RADIUS 50.0f
// The edge solver grows its effective reach by 0.01 per hit. A cache holds
// at most 256 nodes, so two extra units cover every push in one cached query.
#define MARIO_WALL_CACHE_EDGE_MARGIN 2.0f

// This is reset for each ground or air movement pass. Keeping the copied nodes
// here avoids putting a large temporary buffer on Mario's N64 call stack.
#if MARIO_WALL_CANDIDATE_CACHE
static struct {
    struct WallCollisionCache view;
    struct WallCollisionCacheCell cells[MARIO_WALL_CACHE_MAX_CELLS];
    struct SurfaceNode nodes[MARIO_WALL_CACHE_MAX_NODES];
    f32 lowerOffset, upperOffset, travelX, travelZ;
    s32 nodeCount, totalNodes, eligibleNodes;
    s32 enabled, initialized, valid;
} sMarioWallCandidateCache;
#endif

static void mario_reset_wall_candidate_cache(s32 enabled, f32 lowerOffset, f32 upperOffset, f32 travelX,
                                             f32 travelZ) {
#if MARIO_WALL_CANDIDATE_CACHE
    sMarioWallCandidateCache.enabled = enabled;
    sMarioWallCandidateCache.initialized = FALSE;
    sMarioWallCandidateCache.lowerOffset = lowerOffset;
    sMarioWallCandidateCache.upperOffset = upperOffset;
    sMarioWallCandidateCache.travelX = travelX;
    sMarioWallCandidateCache.travelZ = travelZ;
#else
    (void) enabled;
    (void) lowerOffset;
    (void) upperOffset;
    (void) travelX;
    (void) travelZ;
#endif
}

#if MARIO_WALL_CANDIDATE_CACHE
static s32 mario_wall_in_candidate_region(struct Surface *wall) {
    struct WallCollisionCache *view = &sMarioWallCandidateCache.view;
    f32 reach = view->maxRadius + MARIO_WALL_CACHE_EDGE_MARGIN;

    return wall->upperY >= view->minY && wall->lowerY <= view->maxY
           && mario_wall_overlaps_xz_box(wall, view->minX - reach, view->maxX + reach,
                                         view->minZ - reach, view->maxZ + reach);
}

static s32 mario_cache_wall_list(struct SurfaceQueryIterator candidates, struct SurfaceNode **head) {
    struct SurfaceNode **tail = head;
    *head = NULL;
    struct Surface *wall;
    while ((wall = surface_query_next(&candidates)) != NULL) {
        struct SurfaceNode *copy;
#if COLLISION_PROFILING
        COLLISION_PROFILE_COUNT(COLLISION_PROFILE_MARIO_PREPARE_NODES);
#endif
        sMarioWallCandidateCache.totalNodes++;
        // The engine retries the full query if a wall push leaves this region.
        if (!mario_wall_in_candidate_region(wall)) {
            continue;
        }
        sMarioWallCandidateCache.eligibleNodes++;
        if (sMarioWallCandidateCache.nodeCount == MARIO_WALL_CACHE_MAX_NODES) {
            return FALSE;
        }
        copy = &sMarioWallCandidateCache.nodes[sMarioWallCandidateCache.nodeCount++];
        copy->surface = wall;
        copy->next = NULL;
        *tail = copy;
        tail = &copy->next;
    }
    return TRUE;
}

static void mario_build_wall_candidate_cache(Vec3f pos) {
    s32 x = pos[0];
    s32 z = pos[2];
    s32 minCellX = GET_CELL_COORD(x - 50.0f);
    s32 maxCellX = GET_CELL_COORD(x + 50.0f);
    s32 minCellZ = GET_CELL_COORD(z - 50.0f);
    s32 maxCellZ = GET_CELL_COORD(z + 50.0f);
    s32 cellX, cellZ;

    sMarioWallCandidateCache.initialized = TRUE;
    sMarioWallCandidateCache.valid = FALSE;
    sMarioWallCandidateCache.nodeCount = 0;
    sMarioWallCandidateCache.totalNodes = 0;
    sMarioWallCandidateCache.eligibleNodes = 0;
    sMarioWallCandidateCache.view.minY =
        pos[1] + sMarioWallCandidateCache.lowerOffset - MARIO_WALL_CACHE_Y_MARGIN;
    sMarioWallCandidateCache.view.maxY =
        pos[1] + sMarioWallCandidateCache.upperOffset + MARIO_WALL_CACHE_Y_MARGIN;
    sMarioWallCandidateCache.view.minX =
        MIN(pos[0], pos[0] + sMarioWallCandidateCache.travelX) - MARIO_WALL_CACHE_XZ_MARGIN;
    sMarioWallCandidateCache.view.maxX =
        MAX(pos[0], pos[0] + sMarioWallCandidateCache.travelX) + MARIO_WALL_CACHE_XZ_MARGIN;
    sMarioWallCandidateCache.view.minZ =
        MIN(pos[2], pos[2] + sMarioWallCandidateCache.travelZ) - MARIO_WALL_CACHE_XZ_MARGIN;
    sMarioWallCandidateCache.view.maxZ =
        MAX(pos[2], pos[2] + sMarioWallCandidateCache.travelZ) + MARIO_WALL_CACHE_XZ_MARGIN;
    sMarioWallCandidateCache.view.maxRadius = MARIO_WALL_CACHE_MAX_RADIUS;
    sMarioWallCandidateCache.view.minCellX = minCellX;
    sMarioWallCandidateCache.view.minCellZ = minCellZ;
    sMarioWallCandidateCache.view.cellCountX = maxCellX - minCellX + 1;
    sMarioWallCandidateCache.view.cellCountZ = maxCellZ - minCellZ + 1;
    sMarioWallCandidateCache.view.cells = sMarioWallCandidateCache.cells;

    if (is_outside_level_bounds(x - 50, z - 50) || is_outside_level_bounds(x + 50, z + 50)
        || sMarioWallCandidateCache.view.cellCountX <= 0
        || sMarioWallCandidateCache.view.cellCountZ <= 0
        || sMarioWallCandidateCache.view.cellCountX * sMarioWallCandidateCache.view.cellCountZ
               > MARIO_WALL_CACHE_MAX_CELLS) {
        return;
    }

    for (cellX = minCellX; cellX <= maxCellX; cellX++) {
        for (cellZ = minCellZ; cellZ <= maxCellZ; cellZ++) {
            struct WallCollisionCacheCell *cell =
                &sMarioWallCandidateCache
                     .cells[(cellX - minCellX) * sMarioWallCandidateCache.view.cellCountZ
                            + (cellZ - minCellZ)];
            if (!mario_cache_wall_list(
                    surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_WALLS, SURFACE_QUERY_DYNAMIC),
                    &cell->dynamicWalls)
                || !mario_cache_wall_list(
                    surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_WALLS, SURFACE_QUERY_STATIC),
                    &cell->staticWalls)) {
                return;
            }
        }
    }
    // Keep the copied lists only when filtering repays their preparation.
    sMarioWallCandidateCache.valid =
        sMarioWallCandidateCache.totalNodes != 0
        && sMarioWallCandidateCache.eligibleNodes * 4 <= sMarioWallCandidateCache.totalNodes * 3;
}

#endif

static void mario_resolve_wall_collisions(Vec3f pos, f32 offset, f32 radius,
                                          struct WallCollisionData *collisionData) {
#if MARIO_WALL_CANDIDATE_CACHE
    if (sMarioWallCandidateCache.enabled) {
        s32 x = pos[0];
        s32 z = pos[2];
        s32 minCellX = GET_CELL_COORD(x - radius);
        s32 maxCellX = GET_CELL_COORD(x + radius);
        s32 minCellZ = GET_CELL_COORD(z - radius);
        s32 maxCellZ = GET_CELL_COORD(z + radius);
        f32 queryY = pos[1] + offset;
        struct WallCollisionCache *view = &sMarioWallCandidateCache.view;

        if (!sMarioWallCandidateCache.initialized || queryY < view->minY || queryY > view->maxY
            || pos[0] < view->minX || pos[0] > view->maxX || pos[2] < view->minZ || pos[2] > view->maxZ
            || minCellX < view->minCellX || maxCellX >= view->minCellX + view->cellCountX
            || minCellZ < view->minCellZ || maxCellZ >= view->minCellZ + view->cellCountZ) {
            mario_build_wall_candidate_cache(pos);
        }
        if (sMarioWallCandidateCache.valid && queryY >= view->minY && queryY <= view->maxY
            && pos[0] >= view->minX && pos[0] <= view->maxX && pos[2] >= view->minZ
            && pos[2] <= view->maxZ) {
            resolve_and_return_wall_collisions_cached(pos, offset, radius, collisionData, view);
            return;
        }
    }
#endif
    resolve_and_return_wall_collisions(pos, offset, radius, collisionData);
}

// A front-facing ray can only hit a wall if the step moves from the front of
// its plane to the back. Use this cheap necessary condition before raycasting.
static s32 mario_wall_list_may_cross_step(struct SurfaceQueryIterator candidates, Vec3f start,
                                          Vec3f end, f32 lowerHeight, f32 upperHeight,
                                          s32 ignoreVanishWalls) {
    f32 minX = MIN(start[0], end[0]) - 1.0f;
    f32 maxX = MAX(start[0], end[0]) + 1.0f;
    f32 minZ = MIN(start[2], end[2]) - 1.0f;
    f32 maxZ = MAX(start[2], end[2]) + 1.0f;
    f32 heights[2] = { lowerHeight, upperHeight };
    s32 i;

    struct Surface *wall;
    while ((wall = surface_query_next(&candidates)) != NULL) {
#if COLLISION_PROFILING
        COLLISION_PROFILE_COUNT(COLLISION_PROFILE_MARIO_PREPARE_NODES);
#endif
        f32 planeMove;

        if (wall->type == SURFACE_CAMERA_BOUNDARY
            || (ignoreVanishWalls && wall->type == SURFACE_VANISH_CAP_WALLS)
            || !mario_wall_overlaps_xz_box(wall, minX, maxX, minZ, maxZ)) {
            continue;
        }
        planeMove = wall->normal.x * (end[0] - start[0]) + wall->normal.y * (end[1] - start[1])
                    + wall->normal.z * (end[2] - start[2]);
        if (planeMove >= 0.0f) {
            continue;
        }
        for (i = 0; i < 2; i++) {
            f32 startY = start[1] + heights[i];
            f32 endY = end[1] + heights[i];
            f32 startOffset;
            if (wall->lowerY > MAX(startY, endY) || wall->upperY < MIN(startY, endY)) {
                continue;
            }
            startOffset = wall->normal.x * start[0] + wall->normal.y * startY
                          + wall->normal.z * start[2] + wall->originOffset;
            if (startOffset > 0.0f && startOffset + planeMove <= 0.0f) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

static s32 mario_wall_step_may_cross_plane(struct MarioState *m, Vec3f nextPos, f32 lowerHeight,
                                           f32 upperHeight) {
    f32 minX = MIN(m->pos[0], nextPos[0]) - 1.0f;
    f32 maxX = MAX(m->pos[0], nextPos[0]) + 1.0f;
    f32 minZ = MIN(m->pos[2], nextPos[2]) - 1.0f;
    f32 maxZ = MAX(m->pos[2], nextPos[2]) + 1.0f;
    s32 ignoreVanishWalls =
        (m->flags & MARIO_VANISH_CAP) || (m->marioObj->activeFlags & ACTIVE_FLAG_MOVE_THROUGH_GRATE);
    s32 minCellX, maxCellX, minCellZ, maxCellZ;
    s32 cellX, cellZ;

    if (is_outside_level_bounds(minX, minZ) || is_outside_level_bounds(maxX, maxZ)) {
        return TRUE;
    }
    minCellX = (s32) ((minX + LEVEL_BOUNDARY_MAX) / CELL_SIZE);
    maxCellX = (s32) ((maxX + LEVEL_BOUNDARY_MAX) / CELL_SIZE);
    minCellZ = (s32) ((minZ + LEVEL_BOUNDARY_MAX) / CELL_SIZE);
    maxCellZ = (s32) ((maxZ + LEVEL_BOUNDARY_MAX) / CELL_SIZE);

    for (cellX = minCellX; cellX <= maxCellX; cellX++) {
        for (cellZ = minCellZ; cellZ <= maxCellZ; cellZ++) {
            if (mario_wall_list_may_cross_step(
                    surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_WALLS, SURFACE_QUERY_STATIC),
                    m->pos, nextPos, lowerHeight, upperHeight, ignoreVanishWalls)
                || mario_wall_list_may_cross_step(
                    surface_query_cell(cellX, cellZ, SPATIAL_PARTITION_WALLS, SURFACE_QUERY_DYNAMIC),
                    m->pos, nextPos, lowerHeight, upperHeight, ignoreVanishWalls)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

// The usual wall resolver examines Mario's destination. Catch a wall crossed
// between destinations, including pushes across a neighboring wall at a seam. Remove
// only the inward part of the move so Mario can still slide along it.
static s32 raycast_mario_crossed_wall(struct MarioState *m, Vec3f nextPos, f32 lowerHeight,
                                      f32 upperHeight, struct Surface **lowerHit,
                                      struct Surface **upperHit, s32 resolve) {
    Vec3f direction, origin, hitPos;
    struct Surface *wall, *nearestWall = NULL;
    struct Surface **hits[2] = { lowerHit, upperHit };
    f32 nearestDistance = 0.0f;
    f32 heights[2] = { lowerHeight, upperHeight };
    s32 flags = RAYCAST_FIND_WALL | RAYCAST_MARIO_WALL;
    s32 i;

    *lowerHit = NULL;
    *upperHit = NULL;
    vec3f_diff(direction, nextPos, m->pos);
    if (sqr(direction[0]) + sqr(direction[1]) + sqr(direction[2]) < 0.0001f) {
        return FALSE;
    }
    if ((m->flags & MARIO_VANISH_CAP) || (m->marioObj->activeFlags & ACTIVE_FLAG_MOVE_THROUGH_GRATE)) {
        flags |= RAYCAST_IGNORE_VANISH_CAP_WALL;
    }

    origin[0] = m->pos[0];
    origin[2] = m->pos[2];
    for (i = 0; i < 2; i++) {
        f32 distance;
        origin[1] = m->pos[1] + heights[i];
        distance = find_surface_on_ray(origin, direction, &wall, hitPos, flags);
        *hits[i] = (wall != NULL && wall->type == SURFACE_NON_INTERACTIVE_BOUNDARY) ? NULL : wall;
        if (wall != NULL && (nearestWall == NULL || distance < nearestDistance)) {
            nearestWall = wall;
            nearestDistance = distance;
        }
    }

    if (nearestWall != NULL && resolve) {
        f32 horizontalNormalSquared = sqr(nearestWall->normal.x) + sqr(nearestWall->normal.z);
        f32 inwardMove = nearestWall->normal.x * direction[0] + nearestWall->normal.y * direction[1]
                         + nearestWall->normal.z * direction[2];
        if (inwardMove < 0.0f && horizontalNormalSquared > 0.0001f) {
            nextPos[0] -= nearestWall->normal.x * inwardMove / horizontalNormalSquared;
            nextPos[2] -= nearestWall->normal.z * inwardMove / horizontalNormalSquared;
        } else {
            vec3f_copy(nextPos, m->pos);
        }
    }
    return nearestWall != NULL;
}

static void mario_reject_crossed_wall_push(struct MarioState *m, Vec3f nextPos, f32 lowerHeight,
                                           f32 upperHeight) {
    struct Surface *lowerHit, *upperHit;
    if (!mario_wall_step_may_cross_plane(m, nextPos, lowerHeight, upperHeight)
        || !raycast_mario_crossed_wall(m, nextPos, lowerHeight, upperHeight, &lowerHit, &upperHit,
                                       FALSE)) {
        return;
    }
    // A correction at a tight seam can cross another wall. Reject that horizontal
    // push instead of resolving the same incompatible corner over and over.
    nextPos[0] = m->pos[0];
    nextPos[2] = m->pos[2];
    // Vertical walls leave jumping/falling intact. A tilted wall may also block Y.
    if (mario_wall_step_may_cross_plane(m, nextPos, lowerHeight, upperHeight)
        && raycast_mario_crossed_wall(m, nextPos, lowerHeight, upperHeight, &lowerHit, &upperHit,
                                      FALSE)) {
        nextPos[1] = m->pos[1];
    }
}

static void mario_skip_wall_collisions(struct WallCollisionData *lowerWall,
                                       struct WallCollisionData *upperWall) {
    lowerWall->numWalls = 0;
    upperWall->numWalls = 0;
    // Normal wall queries consume these flags before the floor query runs.
    gCollisionFlags &= ~(COLLISION_FLAG_RETURN_FIRST | COLLISION_FLAG_EXCLUDE_DYNAMIC
                         | COLLISION_FLAG_INCLUDE_INTANGIBLE);
}

s32 mario_wall_collision_begin(struct MarioState *m, s32 steps, f32 lowerOffset, f32 upperOffset,
                               f32 verticalTravel, f32 verticalMargin) {
    s32 checkCrossedWalls = (gCollisionFlags & COLLISION_FLAG_CAMERA)
                            || mario_wall_near_movement_path(m, m->vel[0], verticalTravel, m->vel[2],
                                                             lowerOffset, upperOffset, verticalMargin);
    mario_reset_wall_candidate_cache(checkCrossedWalls && steps >= MARIO_WALL_CACHE_MIN_STEPS,
                                     lowerOffset, upperOffset, m->vel[0], m->vel[2]);
    return checkCrossedWalls;
}

void mario_resolve_ground_step_walls(struct MarioState *m, Vec3f nextPos, s32 checkCrossedWalls,
                                     s32 skipLowerWall, struct WallCollisionData *lowerWall,
                                     struct WallCollisionData *upperWall) {
    if (checkCrossedWalls) {
        if (skipLowerWall) {
            lowerWall->numWalls = 0;
        } else {
            mario_resolve_wall_collisions(nextPos, 30.0f, 24.0f, lowerWall);
        }
        mario_resolve_wall_collisions(nextPos, 60.0f, 50.0f, upperWall);
    } else {
        mario_skip_wall_collisions(lowerWall, upperWall);
    }

    if (checkCrossedWalls
        && mario_wall_step_may_cross_plane(m, nextPos, skipLowerWall ? 60.0f : 30.0f, 60.0f)) {
        struct Surface *lowerRayWall, *upperRayWall;
        s32 hadWallContact = lowerWall->numWalls != 0 || upperWall->numWalls != 0;
        if (raycast_mario_crossed_wall(m, nextPos, skipLowerWall ? 60.0f : 30.0f, 60.0f, &lowerRayWall,
                                       &upperRayWall, TRUE)) {
            if (!hadWallContact) {
                if (!skipLowerWall) {
                    mario_resolve_wall_collisions(nextPos, 30.0f, 24.0f, lowerWall);
                }
                mario_resolve_wall_collisions(nextPos, 60.0f, 50.0f, upperWall);
            }
            mario_reject_crossed_wall_push(m, nextPos, skipLowerWall ? 60.0f : 30.0f, 60.0f);
            if (!skipLowerWall && lowerWall->numWalls == 0 && lowerRayWall != NULL) {
                lowerWall->walls[0] = lowerRayWall;
                lowerWall->numWalls = 1;
            }
            if (upperWall->numWalls == 0 && upperRayWall != NULL) {
                upperWall->walls[0] = upperRayWall;
                upperWall->numWalls = 1;
            }
        }
    }
}

s32 mario_air_step_crosses_wall(struct MarioState *m, Vec3f nextPos) {
    struct Surface *lowerHit, *upperHit;
    return raycast_mario_crossed_wall(m, nextPos, 30.0f, 150.0f, &lowerHit, &upperHit, FALSE);
}

s32 mario_resolve_air_step_floor_side(struct MarioState *m, Vec3f nextPos,
                                      struct WallCollisionData *lowerWall) {
    struct Surface *lowerHit, *upperHit;
    // A rejected top can hide a triangular side below the usual +30 probe.
    // Sweep the feet only in that case, retaining the normal wall/grate rules.
    if (!raycast_mario_crossed_wall(m, nextPos, 0.0f, 0.0f, &lowerHit, &upperHit, TRUE)) {
        return FALSE;
    }
    mario_reject_crossed_wall_push(m, nextPos, 0.0f, 0.0f);
    if (lowerHit != NULL) {
        s32 index = MIN(lowerWall->numWalls, MAX_REFERENCED_WALLS - 1);
        lowerWall->walls[index] = lowerHit;
        lowerWall->numWalls = index + 1;
    }
    return TRUE;
}

void mario_resolve_air_step_walls(struct MarioState *m, Vec3f nextPos, s32 checkCrossedWalls,
                                  struct WallCollisionData *lowerWall,
                                  struct WallCollisionData *upperWall) {
    if (checkCrossedWalls) {
        mario_resolve_wall_collisions(nextPos, 150.0f, 50.0f, upperWall);
        mario_resolve_wall_collisions(nextPos, 30.0f, 50.0f, lowerWall);
    } else {
        mario_skip_wall_collisions(lowerWall, upperWall);
    }

    if (checkCrossedWalls && mario_wall_step_may_cross_plane(m, nextPos, 30.0f, 150.0f)) {
        struct Surface *lowerRayWall, *upperRayWall;
        s32 hadWallContact = lowerWall->numWalls != 0 || upperWall->numWalls != 0;
        if (raycast_mario_crossed_wall(m, nextPos, 30.0f, 150.0f, &lowerRayWall, &upperRayWall, TRUE)) {
            if (!hadWallContact) {
                mario_resolve_wall_collisions(nextPos, 150.0f, 50.0f, upperWall);
                mario_resolve_wall_collisions(nextPos, 30.0f, 50.0f, lowerWall);
            }
            mario_reject_crossed_wall_push(m, nextPos, 30.0f, 150.0f);
            if (lowerWall->numWalls == 0 && lowerRayWall != NULL) {
                lowerWall->walls[0] = lowerRayWall;
                lowerWall->numWalls = 1;
            }
            if (upperWall->numWalls == 0 && upperRayWall != NULL) {
                upperWall->walls[0] = upperRayWall;
                upperWall->numWalls = 1;
            }
        }
    }
}
