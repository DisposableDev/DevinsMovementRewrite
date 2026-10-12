#ifndef MARIO_MOVING_COLLISION_MATH_H
#define MARIO_MOVING_COLLISION_MATH_H

#include "types.h"

// Convex envelope of the action's existing endpoint and horizontal-disk probes.
struct MovingBodyProfile {
    f32 height;
    f32 diskHeight[2];
    f32 diskRadius[2];
};

// Positions and profile dimensions use world units; plane normals are unit length.
// Full-profile origins satisfy dot(normal, position) >= offset.
struct MovingContactPlane {
    Vec3f normal;
    f32 offset;
    f32 support;
    struct Object *source;
    u8 pressing;
};

enum MovingCastResult {
    // An unresolved cast requires conservative handling; it does not prove clearance.
    MOVING_CAST_INDETERMINATE = -1,
    MOVING_CAST_CLEAR,
    MOVING_CAST_HIT
};

// Write the farthest body-relative point and return its dot product with direction.
f32 moving_body_support(struct MovingBodyProfile *profile, Vec3f direction, Vec3f point);

// Return separation in world units, zero for contact, or MOVING_CAST_INDETERMINATE.
// The outward unit normal is valid only for positive separation.
f32 moving_triangle_distance(struct MovingBodyProfile *profile, Vec3f position, Vec3f vertices[3],
                             Vec3f normal);

// Sweep the body and one-sided triangle between their respective endpoints.
// Only MOVING_CAST_HIT supplies a fraction in [0, 1] and an outward unit normal.
s32 moving_triangle_cast(struct MovingBodyProfile *profile, Vec3f start, Vec3f end, Vec3f before[3],
                         Vec3f after[3], f32 *fraction, Vec3f normal);

// Project to the nearest feasible origin for bodyFraction in [0, 1].
// Return TRUE with result written; FALSE leaves result unspecified.
s32 moving_project_position(Vec3f desired, struct MovingContactPlane *planes, s32 count,
                            f32 bodyFraction, Vec3f result);

// Uniform profile fit on a 160-unit scale: zero is blocked, 161 is fully clear.
f32 moving_contact_clearance(Vec3f desired, struct MovingContactPlane *planes, s32 count);

#endif
