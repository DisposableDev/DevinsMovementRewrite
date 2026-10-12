#ifndef SURFACE_COLLISION_H
#define SURFACE_COLLISION_H

#include <PR/ultratypes.h>

#include "types.h"

#include "config/config_world.h"

// The y coord is moved upward by this amount when finding floors.
// Vanilla value is 78.
#define FIND_FLOOR_BUFFER 78

#define CELL_HEIGHT_LIMIT            20000
#define FLOOR_LOWER_LIMIT           -11000
#define FLOOR_LOWER_LIMIT_MISC      (FLOOR_LOWER_LIMIT + 1000)

#define is_outside_level_bounds(xPos, zPos) \
    (((xPos) <= -LEVEL_BOUNDARY_MAX) ||     \
     ((xPos) >=  LEVEL_BOUNDARY_MAX) ||     \
     ((zPos) <= -LEVEL_BOUNDARY_MAX) ||     \
     ((zPos) >=  LEVEL_BOUNDARY_MAX))

#define get_surface_height_at_location(xPos, zPos, surf) (-(((xPos) * (surf)->normal.x) + ((zPos) * (surf)->normal.z) + (surf)->originOffset) / (surf)->normal.y)

#define SURFACE_YAW(s) (atan2s(((s)->normal.z), ((s)->normal.x)))

enum RaycastFlags {
    RAYCAST_FIND_FLOOR = (1 << 0),
    RAYCAST_FIND_WALL = (1 << 1),
    RAYCAST_FIND_CEIL = (1 << 2),
    RAYCAST_FIND_WATER = (1 << 3),
    RAYCAST_FIND_ALL =
        (RAYCAST_FIND_FLOOR | RAYCAST_FIND_WALL | RAYCAST_FIND_CEIL | RAYCAST_FIND_WATER),
    // Match Mario's physical wall filtering instead of the camera's filtering.
    RAYCAST_MARIO_WALL = (1 << 4),
    RAYCAST_IGNORE_VANISH_CAP_WALL = (1 << 5),
    RAYCAST_IGNORE_INTANGIBLE = (1 << 6)
};

struct WallCollisionData {
    /*0x00*/ f32 x, y, z;
    /*0x0C*/ f32 offsetY;
    /*0x10*/ f32 radius;
    /*0x14*/ u8 filler[2];
    /*0x16*/ s16 numWalls;
    /*0x18*/ struct Surface *walls[MAX_REFERENCED_WALLS];
};

struct SurfaceNode;

// A movement-local view of wall lists in the same cell and surface order as
// the normal partitions. The caller owns the lists for the duration of a query.
struct WallCollisionCacheCell {
    struct SurfaceNode *dynamicWalls;
    struct SurfaceNode *staticWalls;
};

struct WallCollisionCache {
    struct WallCollisionCacheCell *cells;
    s32 minCellX, minCellZ;
    s32 cellCountX, cellCountZ;
    f32 minY, maxY;
    // All walls able to reach a query center inside this rectangle are cached.
    // A wall push outside it reruns the query against the full partitions.
    f32 minX, maxX, minZ, maxZ;
    f32 maxRadius;
};

s32 f32_find_wall_collision(f32 *xPtr, f32 *yPtr, f32 *zPtr, f32 offsetY, f32 radius);
s32 find_wall_collisions(struct WallCollisionData *colData);
void resolve_and_return_wall_collisions(Vec3f pos, f32 offset, f32 radius, struct WallCollisionData *collisionData);
void resolve_and_return_wall_collisions_cached(Vec3f pos, f32 offset, f32 radius,
                                               struct WallCollisionData *collisionData,
                                               struct WallCollisionCache *cache);
f32 find_ceil(f32 posX, f32 posY, f32 posZ, struct Surface **pceil);
// Fractional triangle selection and height for swept movement. These retain the
// ordinary surface filters and consume the same one-shot collision flags.
f32 find_floor_precise(f32 x, f32 y, f32 z, struct Surface **floor);
// Exact query results owned by one movement call. Reset count/next to zero
// before use; a negative count bypasses reuse until the owner activates it.
// Geometry and collision eligibility must stay fixed during use.
#define SURFACE_QUERY_CACHE_SIZE 8
struct SurfaceQueryCacheEntry {
    Vec3f position;
    struct Surface *surface;
    f32 height;
    s16 flags, remainingFlags;
    s32 ceiling;
};
struct SurfaceQueryCache {
    struct SurfaceQueryCacheEntry entries[SURFACE_QUERY_CACHE_SIZE];
    s32 count, next;
};
f32 find_floor_precise_cached(f32 x, f32 y, f32 z, struct Surface **floor,
                              struct SurfaceQueryCache *cache);
f32 find_ceil_precise_cached(f32 x, f32 y, f32 z, struct Surface **ceil,
                             struct SurfaceQueryCache *cache);
// Grounded air-query origin corrected for integer floor-coordinate rounding.
void get_air_step_floor_start(const Vec3f pos, struct Surface *floor, f32 floorHeight, Vec3f start);
// Buffered airborne lookup requires a plane crossing before an upward floor snap.
f32 find_floor_for_air_step(const Vec3f startPos, const Vec3f nextPos, struct Surface **floor);
// Also retain a rejected buffered top for Mario's finite low-riser check.
f32 find_floor_for_air_step_with_obstacle(const Vec3f startPos, const Vec3f nextPos,
                                          struct Surface **floor, struct Surface **obstacle);
// Earliest finite floor crossing; preserves flags. Outputs are valid only on TRUE.
s32 find_air_step_floor_contact(const Vec3f start, const Vec3f next, Vec3f contact,
                                struct Surface **floor, s16 queryFlags);
f32 find_ceil_precise(f32 x, f32 y, f32 z, struct Surface **ceil);

// Finds the ceiling from a vec3f and a minimum height (with 3 unit vertical buffer).
ALWAYS_INLINE f32 find_mario_ceil(Vec3f pos, f32 height, struct Surface **ceil) {
    return find_ceil(pos[0], MAX(height, pos[1]) + 3.0f, pos[2], ceil);
}

f32 find_floor_height(f32 x, f32 y, f32 z);
f32 find_floor(f32 xPos, f32 yPos, f32 zPos, struct Surface **pfloor);
// Physical object support query; excludes owner geometry without changing query flags.
f32 find_floor_excluding_objects(f32 x, f32 y, f32 z, struct Surface **floor, struct Object *owner,
                                 struct Object *attachment);
f32 find_room_floor(f32 x, f32 y, f32 z, struct Surface **pfloor);
s32 get_room_at_pos(f32 x, f32 y, f32 z);
s32 find_water_level_and_floor(s32 x, s32 y, s32 z, struct Surface **pfloor);
s32 find_water_level(s32 x, s32 z);
s32 find_poison_gas_level(s32 x, s32 z);
#ifdef VANILLA_DEBUG
void debug_surface_list_info(f32 xPos, f32 zPos);
#endif

#endif // SURFACE_COLLISION_H
