#include <PR/ultratypes.h>

#include "sm64.h"
#include "collision_profile.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "engine/surface_query.h"
#include "mario.h"
#include "game_options.h"
#include "game_options_runtime.h"
#include "level_update.h"
#include "level_table.h"
#include "mario_water_collision.h"
#include "object_list_processor.h"

#define WATER_COLLISION_STEP_DISTANCE 16.0f
#define WATER_COLLISION_MAX_STEPS 4
#define WATER_COLLISION_MAX_SLIDES 4
#define WATER_COLLISION_SKIN 0.01f
#define WATER_COLLISION_MAX_RECOVERY 32.0f
#define WATER_COLLISION_WALL_OFFSET 10.0f
#define WATER_COLLISION_WALL_RADIUS 110.0f

struct WaterPreparationGeometry {
    struct Surface *floor, *ceil;
    f32 floorHeight, ceilHeight;
};

// Remove inward movement, retaining the component along a solid. Mario's own
// momentum is projected separately from displacement, which includes currents.
static void project_water_velocity(Vec3f velocity, struct Surface *surface) {
    f32 intoSurface = velocity[0] * surface->normal.x + velocity[1] * surface->normal.y
                      + velocity[2] * surface->normal.z;

    if (intoSurface < 0.0f) {
        velocity[0] -= intoSurface * surface->normal.x;
        velocity[1] -= intoSurface * surface->normal.y;
        velocity[2] -= intoSurface * surface->normal.z;
    }
}

// Projecting onto one face can send movement back into an earlier face. Keep
// both constraints by moving along their crease; a closed corner stops travel.
// Normals belong to this sweep only.
static void project_water_contact_velocity(Vec3f velocity, struct Surface *surface, Vec3f normals[],
                                           s32 normalCount) {
    Vec3f original;
    vec3f_copy(original, velocity);
    project_water_velocity(velocity, surface);
    for (s32 i = 0; i < normalCount; i++) {
        if (vec3f_dot(velocity, normals[i]) >= -0.0001f) {
            continue;
        }
        Vec3f normal = { surface->normal.x, surface->normal.y, surface->normal.z };
        Vec3f crease;
        vec3f_cross(crease, normals[i], normal);
        f32 lengthSquared = vec3f_dot(crease, crease);
        if (lengthSquared < NEAR_ZERO) {
            vec3f_set(velocity, 0.0f, 0.0f, 0.0f);
            return;
        }
        f32 alongCrease = vec3f_dot(original, crease) / lengthSquared;
        for (s32 axis = 0; axis < 3; axis++) {
            velocity[axis] = crease[axis] * alongCrease;
        }
        for (s32 previous = 0; previous < normalCount; previous++) {
            if (vec3f_dot(velocity, normals[previous]) < -0.0001f) {
                vec3f_set(velocity, 0.0f, 0.0f, 0.0f);
                return;
            }
        }
        return;
    }
}

// Floor contact must survive later wall/ceiling contacts so plunges and downward
// bursts can end. WaterStep is an enum, not a set of contact bits.
static u32 merge_water_step_result(u32 previous, u32 current) {
    if (previous == WATER_STEP_HIT_FLOOR || current == WATER_STEP_HIT_FLOOR) {
        return WATER_STEP_HIT_FLOOR;
    }
    if (previous == WATER_STEP_CANCELLED || current == WATER_STEP_CANCELLED) {
        return WATER_STEP_CANCELLED;
    }
    if (previous == WATER_STEP_HIT_CEILING || current == WATER_STEP_HIT_CEILING) {
        return WATER_STEP_HIT_CEILING;
    }
    if (previous == WATER_STEP_HIT_WALL || current == WATER_STEP_HIT_WALL) {
        return WATER_STEP_HIT_WALL;
    }
    return WATER_STEP_NONE;
}

// Sweep the uncorrected path. The one-unit extension catches motion starting
// on a plane; it is subtracted back out of the contact fraction. All positions
// and plane tests remain fractional, unlike the engine's floor lookup cells.
static s32 find_water_surface_edge(Vec3f start, Vec3f end, struct Surface *surface, f32 *fraction,
                                   struct Surface *contact) {
    f32 height;
    f32 entry = -1.0f, exit = 1.0f;
    if (MIN(start[1], end[1]) > surface->upperY || MAX(start[1], end[1]) + 160.0f < surface->lowerY) {
        return FALSE;
    }
    // The cell lists can contain large faces far from this short swim segment.
    // Reject their bounds before the edge divisions and normal calculation.
    for (s32 axis = 0; axis <= 2; axis += 2) {
        s16 minimum = MIN(surface->vertex1[axis], MIN(surface->vertex2[axis], surface->vertex3[axis]));
        s16 maximum = MAX(surface->vertex1[axis], MAX(surface->vertex2[axis], surface->vertex3[axis]));
        if (MIN(start[axis], end[axis]) > maximum || MAX(start[axis], end[axis]) < minimum) {
            return FALSE;
        }
    }
    Vec3f normal = { 0.0f, 0.0f, 0.0f };
    Vec3s *vertices[3] = { &surface->vertex1, &surface->vertex2, &surface->vertex3 };
    for (s32 i = 0; i < 3; i++) {
        s16 *a = *vertices[i], *b = *vertices[(i + 1) % 3];
        f32 nx = -(b[2] - a[2]), nz = b[0] - a[0];
        // Floor/ceiling winding is already encoded in the geometric normal.
        if (surface->normal.y > 0.0f) {
            nx = -nx;
            nz = -nz;
        }
        f32 from = nx * (start[0] - a[0]) + nz * (start[2] - a[2]);
        f32 to = nx * (end[0] - a[0]) + nz * (end[2] - a[2]);
        if (from < 0.0f && to < 0.0f) {
            return FALSE;
        }
        if (from >= 0.0f && to < 0.0f) {
            exit = MIN(exit, from / (from - to));
        }
        if (from < 0.0f && to >= 0.0f) {
            f32 crossing = from / (from - to);
            if (crossing > entry) {
                entry = crossing;
                vec3f_set(normal, -nx, 0.0f, -nz);
            }
        }
    }
    if (entry < 0.0f || entry > exit || entry >= *fraction) {
        return FALSE;
    }
    f32 x = start[0] + (end[0] - start[0]) * entry;
    f32 y = start[1] + (end[1] - start[1]) * entry;
    f32 z = start[2] + (end[2] - start[2]) * entry;
    height = surface->vertex1[1]
             - ((x - surface->vertex1[0]) * surface->normal.x
                + (z - surface->vertex1[2]) * surface->normal.z)
                   / surface->normal.y;
    // A body below the edge can pass underneath. In particular, neighboring
    // triangles on the same underside must not turn their seam into a wall.
    if (y > height || y + 160.0f <= height + WATER_COLLISION_SKIN) {
        return FALSE;
    }
    vec3f_normalize(normal);
    *contact = *surface;
    contact->normal.x = normal[0];
    contact->normal.y = normal[1];
    contact->normal.z = normal[2];
    *fraction = entry;
    return TRUE;
}

static struct Surface *find_water_crossed_surface(struct MarioState *m, Vec3f start, Vec3f nextPos,
                                                  f32 *fraction, u32 *result, f32 wallOffset,
                                                  struct Surface *edgeContact) {
    const f32 offsets[3] = { 0.0f, wallOffset, 160.0f };
    const s32 kinds[3] = { RAYCAST_FIND_FLOOR | RAYCAST_IGNORE_INTANGIBLE, RAYCAST_FIND_WALL,
                           RAYCAST_FIND_CEIL };
    const u32 results[3] = { WATER_STEP_HIT_FLOOR, WATER_STEP_HIT_WALL, WATER_STEP_HIT_CEILING };
    Vec3f direction, origin, hitPos;
    struct Surface *nearest = NULL;
    f32 nearestDistance = CELL_HEIGHT_LIMIT * 2.0f;
    f32 length;
    s32 wallFlags = 0;

    vec3f_diff(direction, nextPos, start);
    length = sqrtf(vec3f_dot(direction, direction));
    if (length < 0.0001f) {
        return NULL;
    }
    if ((m->flags & MARIO_VANISH_CAP) || (m->marioObj->activeFlags & ACTIVE_FLAG_MOVE_THROUGH_GRATE)) {
        wallFlags = RAYCAST_IGNORE_VANISH_CAP_WALL;
    }
    for (s32 i = 0; i < 3; i++) {
        origin[i] = start[i] - direction[i] / length;
        direction[i] *= (length + 1.0f) / length;
    }
    for (s32 i = 0; i < 3; i++) {
        struct Surface *surface;
        f32 distance;
        origin[1] += offsets[i];
        distance = find_surface_on_ray(origin, direction, &surface, hitPos,
                                       RAYCAST_MARIO_WALL | kinds[i] | wallFlags);
        origin[1] -= offsets[i];
        if (surface != NULL && distance < nearestDistance) {
            nearest = surface;
            nearestDistance = distance;
            *fraction = CLAMP((distance - 1.0f) / length, 0.0f, 1.0f);
            *result = results[i];
        }
    }
    // Sweep the body through the cells touched by this segment. Checking all
    // candidates handles adjacent faces hidden by the nearest vertical query.
    // Two clear endpoints do not rule out a finite face between feet and head.
    // Use the full swept body bounds, retaining the ordinary ray filters.
    if (*fraction > 0.0f && !is_outside_level_bounds(start[0], start[2])
        && !is_outside_level_bounds(nextPos[0], nextPos[2])) {
        // Match the edge test's existing bounds, including the full body height.
        // The segment stays fixed while contacts compete for the first crossing.
        Vec3f minimum = { MIN(start[0], nextPos[0]), MIN(start[1], nextPos[1]),
                          MIN(start[2], nextPos[2]) };
        Vec3f maximum = { MAX(start[0], nextPos[0]), MAX(start[1], nextPos[1]) + 160.0f,
                          MAX(start[2], nextPos[2]) };
        s32 minX = GET_CELL_COORD(MIN(start[0], nextPos[0]));
        s32 maxX = GET_CELL_COORD(MAX(start[0], nextPos[0]));
        s32 minZ = GET_CELL_COORD(MIN(start[2], nextPos[2]));
        s32 maxZ = GET_CELL_COORD(MAX(start[2], nextPos[2]));
        for (s32 z = minZ; z <= maxZ; z++) {
            for (s32 x = minX; x <= maxX; x++) {
                for (s32 partition = SPATIAL_PARTITION_FLOORS; partition <= SPATIAL_PARTITION_CEILS;
                     partition++) {
                    struct SurfaceQueryIterator lists[2] = {
                        surface_query_cell(x, z, partition, SURFACE_QUERY_STATIC),
                        surface_query_cell(x, z, partition, SURFACE_QUERY_DYNAMIC)
                    };
                    for (s32 list = 0; list < 2; list++) {
                        for (struct Surface *surface =
                                 surface_query_next_in_bounds(&lists[list], minimum, maximum);
                             surface != NULL;
                             surface = surface_query_next_in_bounds(&lists[list], minimum, maximum)) {
#if COLLISION_PROFILING
                            COLLISION_PROFILE_COUNT(COLLISION_PROFILE_WATER_EDGE_NODES);
#endif
                            if (surface->type == SURFACE_CAMERA_BOUNDARY
                                || (partition == SPATIAL_PARTITION_FLOORS
                                    && surface->type == SURFACE_INTANGIBLE)
                                || (wallFlags && surface->type == SURFACE_VANISH_CAP_WALLS)) {
                                continue;
                            }
                            if (find_water_surface_edge(start, nextPos, surface, fraction,
                                                        edgeContact)) {
                                nearest = edgeContact;
                                *result = partition == SPATIAL_PARTITION_FLOORS
                                              ? WATER_STEP_HIT_FLOOR
                                              : WATER_STEP_HIT_CEILING;
                            }
                        }
                    }
                }
            }
        }
    }
    return nearest;
}

// Borrowed floor/ceiling faces survive only within this movement call.
// Generated edge planes stay local to their sweep; object geometry stays fixed.
struct WaterContactState {
    struct Surface *surfaces[WATER_COLLISION_MAX_SLIDES];
    u32 results[WATER_COLLISION_MAX_SLIDES];
    s32 count;
};

static s32 water_contact_contains(struct Surface *surface, Vec3f position) {
    Vec3s *vertices[3] = { &surface->vertex1, &surface->vertex2, &surface->vertex3 };
    for (s32 i = 0; i < 3; i++) {
        s16 *a = *vertices[i];
        s16 *b = *vertices[(i + 1) % 3];
        f32 edge = (a[2] - position[2]) * (b[0] - a[0]) - (a[0] - position[0]) * (b[2] - a[2]);
        if (surface->normal.y > 0.0f ? edge < 0.0f : edge > 0.0f) {
            return FALSE;
        }
    }
    return TRUE;
}

static void remember_water_contact(struct WaterContactState *contacts, struct Surface *surface,
                                   u32 result) {
    if (contacts == NULL || (result != WATER_STEP_HIT_FLOOR && result != WATER_STEP_HIT_CEILING)) {
        return;
    }
    for (s32 i = 0; i < contacts->count; i++) {
        if (contacts->surfaces[i] == surface) {
            return;
        }
    }
    if (contacts->count < WATER_COLLISION_MAX_SLIDES) {
        contacts->surfaces[contacts->count] = surface;
        contacts->results[contacts->count++] = result;
    }
}

// Reuse only contacts still touching the body and covering the whole projected
// segment. Finite edges and outward input take the ordinary sweep again.
static s32 reuse_water_contacts(struct WaterContactState *contacts, Vec3f position, Vec3f remaining,
                                Vec3f velocity, Vec3f normals[], u32 *result, Vec3f floorNormal) {
    if (contacts == NULL) {
        return 0;
    }
    struct WaterContactState previous = *contacts;
    Vec3f projected, target;
    s32 normalCount = 0;
    contacts->count = 0;
    if (is_outside_level_bounds(position[0], position[2])) {
        return 0;
    }
    vec3f_copy(projected, remaining);
    for (s32 i = 0; i < previous.count; i++) {
        struct Surface *surface = previous.surfaces[i];
        f32 offset = previous.results[i] == WATER_STEP_HIT_CEILING ? 160.0f : 0.0f;
        f32 separation = (position[0] - surface->vertex1[0]) * surface->normal.x
                         + (position[1] + offset - surface->vertex1[1]) * surface->normal.y
                         + (position[2] - surface->vertex1[2]) * surface->normal.z;
        f32 inward = remaining[0] * surface->normal.x + remaining[1] * surface->normal.y
                     + remaining[2] * surface->normal.z;
        if (separation < 0.0f || separation > WATER_COLLISION_SKIN * 3.0f || inward > 0.0001f
            || !water_contact_contains(surface, position)) {
            continue;
        }
        project_water_contact_velocity(projected, surface, normals, normalCount);
        vec3f_set(normals[normalCount], surface->normal.x, surface->normal.y, surface->normal.z);
        normalCount++;
        remember_water_contact(contacts, surface, previous.results[i]);
    }
    vec3f_sum(target, position, projected);
    if (is_outside_level_bounds(target[0], target[2])) {
        contacts->count = 0;
        return 0;
    }
    for (s32 i = 0; i < contacts->count; i++) {
        if (!water_contact_contains(contacts->surfaces[i], target)) {
            contacts->count = 0;
            return 0;
        }
    }
    for (s32 i = 0; i < contacts->count; i++) {
        if (velocity != NULL) {
            project_water_contact_velocity(velocity, contacts->surfaces[i], normals, i);
        }
        *result = merge_water_step_result(*result, contacts->results[i]);
        if (floorNormal != NULL && floorNormal[1] == 0.0f
            && contacts->results[i] == WATER_STEP_HIT_FLOOR) {
            struct Surface *surface = contacts->surfaces[i];
            vec3f_set(floorNormal, surface->normal.x, surface->normal.y, surface->normal.z);
        }
    }
    vec3f_copy(remaining, projected);
    return normalCount;
}

// Advance to contact, then spend only the remaining movement along the solid.
// Recheck each redirected segment for a second surface. A wall-separation push
// stops at contact instead of converting that horizontal correction into a lift.
static u32 sweep_water_movement(struct MarioState *m, Vec3f start, Vec3f nextPos, Vec3f velocity,
                                s32 slide, f32 wallOffset, struct WaterContactState *contacts,
                                Vec3f floorNormal) {
    Vec3f position, remaining;
    // Retained contact normals plus at most one new normal per slide attempt.
    Vec3f normals[WATER_COLLISION_MAX_SLIDES * 2];
    u32 result = WATER_STEP_NONE;
    vec3f_copy(position, start);
    vec3f_diff(remaining, nextPos, start);
    // Keep substeps, but avoid rediscovering the same nearby finite faces.
    s32 normalCount =
        reuse_water_contacts(contacts, position, remaining, velocity, normals, &result, floorNormal);
    if (normalCount != 0 && vec3f_dot(remaining, remaining) < 0.0001f) {
        vec3f_copy(nextPos, position);
        return result;
    }

    for (s32 attempt = 0; attempt < WATER_COLLISION_MAX_SLIDES; attempt++) {
        struct Surface edgeContact;
        u32 hitResult = WATER_STEP_NONE;
        f32 fraction = 1.0f;
        struct Surface *surface;
        vec3f_sum(nextPos, position, remaining);
        surface = find_water_crossed_surface(m, position, nextPos, &fraction, &hitResult, wallOffset,
                                             &edgeContact);
        if (surface == NULL) {
            return result;
        }

        if (hitResult == WATER_STEP_HIT_WALL && surface != &edgeContact && m->wall == NULL
            && surface->type != SURFACE_NON_INTERACTIVE_BOUNDARY) {
            set_mario_wall(m, surface);
        }
        result = merge_water_step_result(result, hitResult);
        if (floorNormal != NULL && floorNormal[1] == 0.0f && hitResult == WATER_STEP_HIT_FLOOR) {
            // The winning face can be a temporary finite-edge plane. Copy its
            // normal now; the endpoint floor may change later in this movement.
            vec3f_set(floorNormal, surface->normal.x, surface->normal.y, surface->normal.z);
        }
        if (velocity != NULL) {
            project_water_contact_velocity(velocity, surface, normals, normalCount);
        }
        if (!slide) {
            // Separation is not requested travel. Stop just before contact on
            // its original path; a normal offset here can ratchet Mario uphill
            // every time a wall pushes him against a slope.
            fraction =
                MAX(0.0f, fraction - WATER_COLLISION_SKIN / sqrtf(vec3f_dot(remaining, remaining)));
            for (s32 axis = 0; axis < 3; axis++) {
                position[axis] += remaining[axis] * fraction;
            }
            break;
        }
        for (s32 axis = 0; axis < 3; axis++) {
            position[axis] += remaining[axis] * fraction;
            remaining[axis] *= 1.0f - fraction;
        }
        // Keep numerical error from putting the next ray behind the contact.
        position[0] += surface->normal.x * WATER_COLLISION_SKIN;
        position[1] += surface->normal.y * WATER_COLLISION_SKIN;
        position[2] += surface->normal.z * WATER_COLLISION_SKIN;
        project_water_contact_velocity(remaining, surface, normals, normalCount);
        vec3f_set(normals[normalCount], surface->normal.x, surface->normal.y, surface->normal.z);
        normalCount++;
        if (surface != &edgeContact) {
            remember_water_contact(contacts, surface, hitResult);
        }
        if (vec3f_dot(remaining, remaining) < 0.0001f) {
            break;
        }
    }
    // Keep the reached contact if a crowded corner uses up the slide budget.
    if (slide && velocity != NULL && vec3f_dot(remaining, remaining) >= 0.0001f) {
        vec3f_set(velocity, 0.0f, 0.0f, 0.0f);
    }
    vec3f_copy(nextPos, position);
    return result;
}

static s32 water_position_has_clearance(Vec3f pos, struct Surface **floor, f32 *floorHeight,
                                        struct SurfaceQueryCache *cache) {
    struct Surface *ceil;
    *floorHeight = find_floor_precise_cached(pos[0], pos[1], pos[2], floor, cache);
    f32 ceilHeight = find_ceil_precise_cached(pos[0], pos[1] + 3.0f, pos[2], &ceil, cache);
    return *floor != NULL && pos[1] >= *floorHeight - WATER_COLLISION_SKIN
           && ceilHeight - pos[1] >= 160.0f - WATER_COLLISION_SKIN;
}

// An already embedded swimmer must be able to steer out over several substeps.
// Requiring full clearance on the first small step otherwise traps every input.
// Keep both overlaps from growing, and require outward travel from each solid
// already intersecting the body. The plane test also prevents "escaping" by
// moving deeper until that surface drops out of the vertical query's range.
static s32 water_movement_reduces_overlap(Vec3f start, Vec3f target, struct Surface *targetFloor,
                                          f32 targetFloorHeight, struct SurfaceQueryCache *cache) {
    s16 collisionFlags = gCollisionFlags;
    struct Surface *floor, *ceil, *targetCeil;
    f32 floorHeight = find_floor_precise_cached(start[0], start[1], start[2], &floor, cache);
    f32 ceilHeight = find_ceil_precise_cached(start[0], start[1] + 3.0f, start[2], &ceil, cache);
    f32 targetCeilHeight =
        find_ceil_precise_cached(target[0], target[1] + 3.0f, target[2], &targetCeil, cache);
    f32 targetFloorOverlap = MAX(0.0f, targetFloorHeight - target[1]);
    f32 targetCeilOverlap = MAX(0.0f, target[1] + 160.0f - targetCeilHeight);
    f32 floorOverlap, ceilOverlap;
    Vec3f travel;
    s32 allowed = FALSE;

    // A deeper overlap can hide a face outside the ordinary vertical query's
    // buffer. Confirm that the newly found face also covers the starting body
    // before treating it as an existing overlap rather than a new obstacle.
    if (targetFloorOverlap > WATER_COLLISION_SKIN && floor != targetFloor) {
        struct Surface *bodyFloor;
        f32 height = find_floor_precise_cached(start[0], start[1] + 160.0f - FIND_FLOOR_BUFFER,
                                               start[2], &bodyFloor, cache);
        if (bodyFloor == targetFloor) {
            floor = bodyFloor;
            floorHeight = height;
        }
    }
    if (targetCeilOverlap > WATER_COLLISION_SKIN && ceil != targetCeil) {
        struct Surface *bodyCeil;
        f32 height = find_ceil_precise_cached(start[0], start[1] - 160.0f, start[2], &bodyCeil, cache);
        if (bodyCeil == targetCeil) {
            ceil = bodyCeil;
            ceilHeight = height;
        }
    }
    floorOverlap = MAX(0.0f, floorHeight - start[1]);
    ceilOverlap = MAX(0.0f, start[1] + 160.0f - ceilHeight);
    vec3f_diff(travel, target, start);
    if (floor == NULL || targetFloor == NULL || targetFloorOverlap > floorOverlap + WATER_COLLISION_SKIN
        || targetCeilOverlap > ceilOverlap + WATER_COLLISION_SKIN
        || targetFloorOverlap + targetCeilOverlap
               >= floorOverlap + ceilOverlap - WATER_COLLISION_SKIN) {
        goto done;
    }
    if (floorOverlap > WATER_COLLISION_SKIN
        && travel[0] * floor->normal.x + travel[1] * floor->normal.y + travel[2] * floor->normal.z
               <= WATER_COLLISION_SKIN) {
        goto done;
    }
    if (ceilOverlap > WATER_COLLISION_SKIN && ceil != NULL
        && travel[0] * ceil->normal.x + travel[1] * ceil->normal.y + travel[2] * ceil->normal.z
               <= WATER_COLLISION_SKIN) {
        goto done;
    }
    allowed = TRUE;
done:
    gCollisionFlags = collisionFlags;
    return allowed;
}

// Recovery is separate from requested movement and bounded once per call. A
// slightly embedded starting point must not make every small escape step fail.
// Do not snap across a narrow gap or perform unbounded upward floor correction.
static u32 recover_water_overlap(struct MarioState *m, s32 *overlapping,
                                 struct SurfaceQueryCache *cache,
                                 struct WaterPreparationGeometry *geometry) {
    s16 collisionFlags = gCollisionFlags;
    struct Surface *floor, *ceil;
    f32 floorHeight = find_floor_precise_cached(m->pos[0], m->pos[1], m->pos[2], &floor, cache);
    f32 ceilHeight = find_ceil_precise_cached(m->pos[0], m->pos[1] + 3.0f, m->pos[2], &ceil, cache);
    if (geometry != NULL) {
        geometry->floor = floor;
        geometry->ceil = ceil;
        geometry->floorHeight = floorHeight;
        geometry->ceilHeight = ceilHeight;
    }
    Vec3f target;
    u32 result = WATER_STEP_NONE;
    if (overlapping != NULL) {
        *overlapping = floor != NULL
                       && (m->pos[1] < floorHeight - WATER_COLLISION_SKIN
                           || ceilHeight - m->pos[1] < 160.0f - WATER_COLLISION_SKIN);
    }
    if (floor == NULL || ceilHeight - floorHeight < 160.0f) {
        goto done;
    }
    vec3f_copy(target, m->pos);
    if (target[1] < floorHeight - WATER_COLLISION_SKIN) {
        target[1] = floorHeight + WATER_COLLISION_SKIN;
        result = WATER_STEP_HIT_FLOOR;
    } else if (ceilHeight - target[1] < 160.0f - WATER_COLLISION_SKIN) {
        target[1] = ceilHeight - 160.0f - WATER_COLLISION_SKIN;
        result = WATER_STEP_HIT_CEILING;
    }
    if (result == WATER_STEP_NONE || absf(target[1] - m->pos[1]) > WATER_COLLISION_MAX_RECOVERY) {
        result = WATER_STEP_NONE;
        goto done;
    }
    sweep_water_movement(m, m->pos, target, NULL, FALSE, 10.0f, NULL, NULL);
    if (!water_position_has_clearance(target, &floor, &floorHeight, cache)) {
        result = WATER_STEP_NONE;
        goto done;
    }
    vec3f_copy(m->pos, target);
    set_mario_floor(m, floor, floorHeight);
    if (overlapping != NULL) {
        *overlapping = FALSE;
    }
done:
    // These are auxiliary queries. The first ordinary wall query still owns
    // RETURN_FIRST/EXCLUDE_DYNAMIC/INCLUDE_INTANGIBLE, including early exits.
    gCollisionFlags = collisionFlags;
    return result;
}

// A vertical face can constrain later increments only while the wall probe's
// projected point stays strictly inside that finite triangle. Edges, slopes
// and outward travel keep the ordinary separation path.
static s32 water_wall_contact_contains(struct Surface *surface, Vec3f position) {
    s32 axis = absf(surface->normal.x) > absf(surface->normal.z) ? 2 : 0;
    f32 direction = axis == 2 ? -surface->normal.x : surface->normal.z;
    f32 coordinate =
        position[axis]
        - (axis == 2 ? surface->normal.z : surface->normal.x) * WATER_COLLISION_WALL_RADIUS;
    f32 height = position[1] + WATER_COLLISION_WALL_OFFSET;
    Vec3s *vertices[3] = { &surface->vertex1, &surface->vertex2, &surface->vertex3 };
    for (s32 i = 0; i < 3; i++) {
        s16 *a = *vertices[i], *b = *vertices[(i + 1) % 3];
        f32 edge = (b[axis] - a[axis]) * (height - a[1]) - (b[1] - a[1]) * (coordinate - a[axis]);
        if (direction > 0.0f ? edge <= 1.0f : edge >= -1.0f) {
            return FALSE;
        }
    }
    return TRUE;
}

static void project_water_wall_increment(struct Surface *surface, Vec3f position, Vec3f increment) {
    if (surface == NULL || surface->object != NULL || surface->normal.y != 0.0f) {
        return;
    }
    f32 separation =
        position[0] * surface->normal.x + position[2] * surface->normal.z + surface->originOffset;
    f32 inward = increment[0] * surface->normal.x + increment[2] * surface->normal.z;
    if (absf(separation - WATER_COLLISION_WALL_RADIUS) > WATER_COLLISION_SKIN || inward >= 0.0f
        || !water_wall_contact_contains(surface, position)) {
        return;
    }
    Vec3f projected, target;
    vec3f_copy(projected, increment);
    project_water_velocity(projected, surface);
    vec3f_sum(target, position, projected);
    if (water_wall_contact_contains(surface, target)) {
        vec3f_copy(increment, projected);
    }
}

#define WATER_COLLISION_CACHE_CELLS 4
#define WATER_COLLISION_CACHE_NODES 96

// These lists live only during one movement call. Preserve cell and face order;
// retain the normal resolver when the bounded storage or movement region is exceeded.
struct WaterWallCandidates {
    struct WallCollisionCache view;
    struct WallCollisionCacheCell cells[WATER_COLLISION_CACHE_CELLS];
    struct SurfaceNode nodes[WATER_COLLISION_CACHE_NODES];
    s32 nodeCount;
    s32 wallQueryCount; // Build the filtered view on the second wall query.
    struct WallCollisionCache *activeView;
};

static s32 filter_water_wall_list(struct WaterWallCandidates *cache, struct SurfaceQueryIterator list,
                                  struct SurfaceNode **head) {
    struct SurfaceNode **tail = head;
    struct Surface *surface;
    *head = NULL;
    while ((surface = surface_query_next(&list)) != NULL) {
        if (surface->upperY < cache->view.minY || surface->lowerY > cache->view.maxY) {
            continue;
        }
        if (cache->nodeCount == WATER_COLLISION_CACHE_NODES) {
            return FALSE;
        }
        struct SurfaceNode *node = &cache->nodes[cache->nodeCount++];
        node->surface = surface;
        node->next = NULL;
        *tail = node;
        tail = &node->next;
    }
    return TRUE;
}

static struct WallCollisionCache *prepare_water_wall_candidates(struct WaterWallCandidates *cache,
                                                                Vec3f pos, Vec3f displacement) {
    struct WallCollisionCache *view = &cache->view;
    f32 reach = WATER_COLLISION_WALL_RADIUS + 2.0f;
    view->minX = pos[0] - absf(displacement[0]) - reach;
    view->maxX = pos[0] + absf(displacement[0]) + reach;
    view->minZ = pos[2] - absf(displacement[2]) - reach;
    view->maxZ = pos[2] + absf(displacement[2]) + reach;
    if (is_outside_level_bounds(view->minX - reach, view->minZ - reach)
        || is_outside_level_bounds(view->maxX + reach, view->maxZ + reach)) {
        return NULL;
    }
    view->minY = pos[1] - absf(displacement[1]) - WATER_COLLISION_MAX_RECOVERY;
    view->maxY = pos[1] + absf(displacement[1]) + WATER_COLLISION_MAX_RECOVERY + 60.0f;
    view->maxRadius = WATER_COLLISION_WALL_RADIUS;
    view->minCellX = GET_CELL_COORD((s32) pos[0] - reach);
    view->minCellZ = GET_CELL_COORD((s32) pos[2] - reach);
    view->cellCountX = GET_CELL_COORD((s32) pos[0] + reach) - view->minCellX + 1;
    view->cellCountZ = GET_CELL_COORD((s32) pos[2] + reach) - view->minCellZ + 1;
    if (view->cellCountX * view->cellCountZ > WATER_COLLISION_CACHE_CELLS) {
        return NULL;
    }
    view->cells = cache->cells;
    cache->nodeCount = 0;
    for (s32 x = 0; x < view->cellCountX; x++) {
        for (s32 z = 0; z < view->cellCountZ; z++) {
            struct WallCollisionCacheCell *cell = &cache->cells[x * view->cellCountZ + z];
            if (!filter_water_wall_list(cache,
                                        surface_query_cell(view->minCellX + x, view->minCellZ + z,
                                                           SPATIAL_PARTITION_WALLS,
                                                           SURFACE_QUERY_DYNAMIC),
                                        &cell->dynamicWalls)
                || !filter_water_wall_list(cache,
                                           surface_query_cell(view->minCellX + x, view->minCellZ + z,
                                                              SPATIAL_PARTITION_WALLS,
                                                              SURFACE_QUERY_STATIC),
                                           &cell->staticWalls)) {
                return NULL;
            }
        }
    }
    return view;
}

static u32 resolve_water_wall_push(struct MarioState *m, Vec3f pos, f32 offset, f32 radius,
                                   Vec3f velocity, struct SurfaceQueryCache *cache,
                                   struct Surface **wallContact, struct WallCollisionCache *wallCache,
                                   Vec3f floorNormal) {
    struct WallCollisionData wallData;
    struct Surface *floor;
    f32 floorHeight;
    Vec3f beforeWall;
    u32 result = WATER_STEP_NONE;
    vec3f_copy(beforeWall, pos);
    if (wallCache != NULL) {
        resolve_and_return_wall_collisions_cached(pos, offset, radius, &wallData, wallCache);
    } else {
        resolve_and_return_wall_collisions(pos, offset, radius, &wallData);
    }
    // The wall solver can separate a triangle edge without recording a wall.
    // Check those pushes too, so separation cannot cross an intervening solid
    // or worsen an existing floor/ceiling overlap.
    if (wallData.numWalls != 0 || pos[0] != beforeWall[0] || pos[2] != beforeWall[2]) {
        s32 changedPosition = pos[0] != beforeWall[0] || pos[2] != beforeWall[2];
        result = merge_water_step_result(result, WATER_STEP_HIT_WALL);
        if (velocity != NULL) {
            for (s32 i = 0; i < wallData.numWalls; i++) {
                project_water_velocity(velocity, wallData.walls[i]);
            }
            if (wallData.numWalls == 0) {
                f32 dx = pos[0] - beforeWall[0], dz = pos[2] - beforeWall[2];
                f32 intoEdge = (velocity[0] * dx + velocity[2] * dz) / (dx * dx + dz * dz);
                if (intoEdge < 0.0f) {
                    velocity[0] -= intoEdge * dx;
                    velocity[2] -= intoEdge * dz;
                }
            }
        }
        // No separation occurred at an already touching face. Final substep
        // clearance remains authoritative; auxiliary checks of an identical
        // before/after position cannot change the correction.
        if (changedPosition) {
            // Enable exact-query reuse once a wall correction changes the position.
            if (cache != NULL && cache->count < 0) {
                cache->count = 0;
            }
            s32 startedClear = water_position_has_clearance(beforeWall, &floor, &floorHeight, cache);
            result =
                merge_water_step_result(result, sweep_water_movement(m, beforeWall, pos, velocity,
                                                                     FALSE, offset, NULL, floorNormal));
            if (!water_position_has_clearance(pos, &floor, &floorHeight, cache)
                || (!startedClear
                    && !water_movement_reduces_overlap(beforeWall, pos, floor, floorHeight, cache))) {
                vec3f_copy(pos, beforeWall);
                return result;
            }
        }
        if (m->wall == NULL && wallData.numWalls != 0) {
            set_mario_wall(m, wallData.walls[0]);
        }
        if (wallContact != NULL && wallData.numWalls == 1) {
            *wallContact = wallData.walls[0];
        }
    }
    return result;
}

static void prepare_water_geometry(struct MarioState *m, struct SurfaceQueryCache *cache,
                                   struct WaterPreparationGeometry *geometry) {
    recover_water_overlap(m, NULL, cache, geometry);
    resolve_water_wall_push(m, m->pos, 60.0f, 50.0f, NULL, cache, NULL, NULL, NULL);
    resolve_water_wall_push(m, m->pos, 30.0f, 24.0f, NULL, cache, NULL, NULL, NULL);
}

void mario_water_collision_prepare(struct MarioState *m) {
#if COLLISION_RESULT_CACHES
    struct SurfaceQueryCache cache = { .count = 0, .next = 0 };
    struct SurfaceQueryCache *queryCache = &cache;
#else
    struct SurfaceQueryCache *queryCache = NULL;
#endif
    prepare_water_geometry(m, queryCache, NULL);
}

// Geometry inputs consume these results immediately, before interactions can
// change position, eligibility or object collision. The native fallback and
// floor-before-ceiling flag ownership remain the same as the ordinary caller.
void mario_water_collision_prepare_geometry(struct MarioState *m) {
#if COLLISION_RESULT_CACHES
    struct SurfaceQueryCache cache = { .count = 0, .next = 0 };
    struct SurfaceQueryCache *queryCache = &cache;
#else
    struct SurfaceQueryCache *queryCache = NULL;
#endif
    struct WaterPreparationGeometry geometry;
    Vec3f queriedPos;
    s32 ordinaryQueries = gCollisionFlags == 0;
    vec3f_copy(queriedPos, m->pos);
    prepare_water_geometry(m, queryCache, ordinaryQueries ? &geometry : NULL);
    // The auxiliary pair is reusable only at the same pose with ordinary flags.
    // Missing floors still take the graphical-position fallback below.
    if (ordinaryQueries && gCollisionFlags == 0 && geometry.floor != NULL && queriedPos[0] == m->pos[0]
        && queriedPos[1] == m->pos[1] && queriedPos[2] == m->pos[2]) {
        m->floor = geometry.floor;
        m->ceil = geometry.ceil;
        m->floorHeight = geometry.floorHeight;
        m->ceilHeight = geometry.ceilHeight;
        return;
    }
    m->floorHeight = find_floor_precise_cached(m->pos[0], m->pos[1], m->pos[2], &m->floor, queryCache);
    if (m->floor == NULL) {
        vec3f_copy(m->pos, m->marioObj->header.gfx.pos);
        m->floorHeight =
            find_floor_precise_cached(m->pos[0], m->pos[1], m->pos[2], &m->floor, queryCache);
    }
    m->ceilHeight =
        find_ceil_precise_cached(m->pos[0], m->pos[1] + 3.0f, m->pos[2], &m->ceil, queryCache);
}

static u32 perform_water_substep(struct MarioState *m, Vec3f nextPos, Vec3f velocity, s32 *overlapping,
                                 struct WaterContactState *contacts, struct SurfaceQueryCache *cache,
                                 struct Surface **wallContact,
                                 struct WaterWallCandidates *wallCandidates, Vec3f floorNormal) {
    struct WallCollisionCache *wallCache = NULL;
    struct Surface *floor;
    f32 floorHeight;
    s32 hasClearance;
    u32 result = sweep_water_movement(m, m->pos, nextPos, velocity, TRUE, WATER_COLLISION_WALL_OFFSET,
                                      contacts, floorNormal);
    if (wallCandidates != NULL) {
        if (wallCandidates->wallQueryCount++ == 1) {
            Vec3f travel = { 0.0f, 0.0f, 0.0f };
            wallCandidates->activeView = prepare_water_wall_candidates(wallCandidates, nextPos, travel);
        }
        wallCache = wallCandidates->activeView;
    }
    result = merge_water_step_result(
        result,
        resolve_water_wall_push(m, nextPos, WATER_COLLISION_WALL_OFFSET, WATER_COLLISION_WALL_RADIUS,
                                velocity, cache, wallContact, wallCache, floorNormal));
    hasClearance = water_position_has_clearance(nextPos, &floor, &floorHeight, cache);
    if ((!hasClearance || *overlapping)
        && !water_movement_reduces_overlap(m->pos, nextPos, floor, floorHeight, cache)) {
        goto cancelled;
    }

    vec3f_copy(m->pos, nextPos);
    set_mario_floor(m, floor, floorHeight);
    *overlapping = !hasClearance;
    return result;

cancelled:
    if (velocity != NULL) {
        vec3f_set(velocity, 0.0f, 0.0f, 0.0f);
    }
    return WATER_STEP_CANCELLED;
}

static u32 perform_water_collision_step(struct MarioState *m, Vec3f displacement, Vec3f velocity,
                                        struct WaterWallCandidates *candidateCache,
                                        struct SurfaceQueryCache *queryCache, Vec3f floorNormal) {
    s32 steps = 1;
    f32 distanceSquared = vec3f_dot(displacement, displacement);
    Vec3f increment, nextPos;
    s32 overlapping;
    struct WaterContactState contacts = { .count = 0 };
    struct Surface *wallContact = NULL;
    s32 ordinaryWalls = gCollisionFlags == 0;
#if COLLISION_RESULT_CACHES
    struct SurfaceQueryCache cache = { .count = 0, .next = 0 };
    if (queryCache == NULL) {
        queryCache = &cache;
    }
#endif
    u32 result = recover_water_overlap(m, &overlapping, queryCache, NULL);

    s32 maxSteps = game_options_cheat_active(GO_NO_SPEED_LIMIT) ? MARIO_UNLIMITED_COLLISION_MAX_STEPS
                                                                : WATER_COLLISION_MAX_STEPS;
    while (steps < maxSteps && distanceSquared > sqr(steps * WATER_COLLISION_STEP_DISTANCE)) {
        steps++;
    }
    for (s32 axis = 0; axis < 3; axis++) {
        increment[axis] = displacement[axis] / steps;
    }
    for (s32 i = 0; i < steps; i++) {
        Vec3f oldPos, stepIncrement;
        u32 stepResult;
        vec3f_copy(oldPos, m->pos);
        vec3f_copy(stepIncrement, increment);
        if (ordinaryWalls) {
            project_water_wall_increment(wallContact, m->pos, stepIncrement);
        }
        wallContact = NULL;
        vec3f_sum(nextPos, m->pos, stepIncrement);
        nextPos[1] = MIN(nextPos[1], m->waterLevel - 80.0f);
        stepResult =
            perform_water_substep(m, nextPos, velocity, &overlapping, &contacts, queryCache,
                                  ordinaryWalls ? &wallContact : NULL, candidateCache, floorNormal);
        result = merge_water_step_result(result, stepResult);
        if (stepResult == WATER_STEP_CANCELLED) {
            break;
        }
        // Avoid repeating the same queries once fully blocked; tangential
        // movement still consumes all remaining substeps.
        if (stepResult != WATER_STEP_NONE
            && sqr(oldPos[0] - m->pos[0]) + sqr(oldPos[1] - m->pos[1]) + sqr(oldPos[2] - m->pos[2])
                   < 0.0001f) {
            break;
        }
    }
    return result;
}

// Keep the temporary lists off the ordinary swimming path's stack.
static u32 perform_bbh_water_collision_step(struct MarioState *m, Vec3f displacement, Vec3f velocity,
                                            Vec3f floorNormal) {
    struct WaterWallCandidates candidates;
    struct SurfaceQueryCache cache;
    cache.count = -1;
    cache.next = 0;
    candidates.wallQueryCount = 0;
    candidates.activeView = NULL;
    return perform_water_collision_step(m, displacement, velocity, &candidates, &cache, floorNormal);
}

// Keep fast-swim query storage off the slow path's stack, even with GCC's
// single-call inlining. Reuse starts only after an actual wall correction.
static __attribute__((noinline)) u32 perform_cached_water_collision_step(struct MarioState *m,
                                                                         Vec3f displacement,
                                                                         Vec3f velocity,
                                                                         Vec3f floorNormal) {
    struct SurfaceQueryCache cache;
    cache.count = -1;
    cache.next = 0;
    return perform_water_collision_step(m, displacement, velocity, NULL, &cache, floorNormal);
}

u32 mario_water_collision_step_with_floor(struct MarioState *m, Vec3f displacement, Vec3f velocity,
                                          Vec3f floorNormal) {
    if (floorNormal != NULL) {
        vec3f_set(floorNormal, 0.0f, 0.0f, 0.0f);
    }
    set_mario_wall(m, NULL);
    if (gCurrLevelNum == LEVEL_BBH
        && vec3f_dot(displacement, displacement) > sqr(2 * WATER_COLLISION_STEP_DISTANCE)) {
        return perform_bbh_water_collision_step(m, displacement, velocity, floorNormal);
    }
    if (vec3f_dot(displacement, displacement) > sqr(2 * WATER_COLLISION_STEP_DISTANCE)) {
        return perform_cached_water_collision_step(m, displacement, velocity, floorNormal);
    }
    return perform_water_collision_step(m, displacement, velocity, NULL, NULL, floorNormal);
}

u32 mario_water_collision_step(struct MarioState *m, Vec3f displacement, Vec3f velocity) {
    return mario_water_collision_step_with_floor(m, displacement, velocity, NULL);
}
