#ifndef MARIO_WATER_COLLISION_H
#define MARIO_WATER_COLLISION_H

#include <PR/ultratypes.h>
#include "types.h"

struct MarioState;

// Resolve the geometry-input wall pushes without pushing through other solids.
void mario_water_collision_prepare(struct MarioState *m);

// Preparation plus the immediate geometry-input floor/ceiling queries. Reuses
// exact results within this call and publishes metadata at the final position.
void mario_water_collision_prepare_geometry(struct MarioState *m);

// Move one frame's displacement, including currents. If supplied, project the
// velocity along contacted solids to discard blocked momentum. Publishes the
// first contacted wall and yaw, or clears them when no wall was contacted.
// Displacement uses world units; velocity uses world units/frame. Returns a WATER_STEP_* result.
u32 mario_water_collision_step(struct MarioState *m, Vec3f displacement, Vec3f velocity);

// Also retains the first floor-contact normal independently of endpoint support.
// floorNormal[1] is zero when no floor was struck; temporary edge normals are copied.
u32 mario_water_collision_step_with_floor(struct MarioState *m, Vec3f displacement, Vec3f velocity,
                                          Vec3f floorNormal);

#endif
