#ifndef MARIO_COLLISION_H
#define MARIO_COLLISION_H

#include <PR/ultratypes.h>
#include "types.h"

struct MarioState;
struct WallCollisionData;

// Starts one ground or air movement pass and resets its wall candidate cache.
// Offsets, vertical travel, and margin are world units; TRUE requests swept-wall checks.
s32 mario_wall_collision_begin(struct MarioState *m, s32 steps, f32 lowerOffset, f32 upperOffset,
                               f32 verticalTravel, f32 verticalMargin);
void mario_resolve_ground_step_walls(struct MarioState *m, Vec3f nextPos, s32 checkCrossedWalls,
                                     s32 skipLowerWall, struct WallCollisionData *lowerWall,
                                     struct WallCollisionData *upperWall);
void mario_resolve_air_step_walls(struct MarioState *m, Vec3f nextPos, s32 checkCrossedWalls,
                                  struct WallCollisionData *lowerWall,
                                  struct WallCollisionData *upperWall);
// Query the lower/upper body sweeps without moving Mario or nextPos.
s32 mario_air_step_crosses_wall(struct MarioState *m, Vec3f nextPos);
// TRUE rejects a feet-level crossing, adjusts nextPos, and records any lower hit.
// FALSE leaves nextPos and lowerWall unchanged.
s32 mario_resolve_air_step_floor_side(struct MarioState *m, Vec3f nextPos,
                                      struct WallCollisionData *lowerWall);

#endif // MARIO_COLLISION_H
