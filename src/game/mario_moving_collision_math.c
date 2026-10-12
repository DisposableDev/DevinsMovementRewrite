#include <ultra64.h>
#include "mario_moving_collision_math.h"
#include "engine/math_util.h"

#define MOVING_CONTACT_SKIN 0.02f
#define MOVING_MATH_EPSILON 0.00001f

static s32 moving_triangle_normal(Vec3f vertices[3], Vec3f normal);

static f32 moving_dot(Vec3f a, Vec3f b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

static f64 moving_precise_dot(Vec3f a, Vec3f b) {
    return (f64) a[0] * b[0] + (f64) a[1] * b[1] + (f64) a[2] * b[2];
}

static void moving_sub(Vec3f result, Vec3f a, Vec3f b) {
    for (s32 i = 0; i < 3; i++) {
        result[i] = a[i] - b[i];
    }
}

// Small pivoted systems serve contact projection.
// Singular active sets are skipped; lower-dimensional sets remain candidates.
static s32 moving_solve(f64 matrix[5][6], s32 size, f64 result[5]) {
    for (s32 column = 0; column < size; column++) {
        s32 pivot = column;
        f64 value;
        for (s32 row = column + 1; row < size; row++) {
            if (ABS(matrix[row][column]) > ABS(matrix[pivot][column])) {
                pivot = row;
            }
        }
        if (ABS(matrix[pivot][column]) < 0.0000000001) {
            return FALSE;
        }
        for (s32 i = column; i <= size; i++) {
            value = matrix[pivot][i];
            matrix[pivot][i] = matrix[column][i];
            matrix[column][i] = value;
        }
        value = matrix[column][column];
        for (s32 i = column; i <= size; i++) {
            matrix[column][i] /= value;
        }
        for (s32 row = 0; row < size; row++) {
            if (row == column) {
                continue;
            }
            value = matrix[row][column];
            for (s32 i = column; i <= size; i++) {
                matrix[row][i] -= value * matrix[column][i];
            }
        }
    }
    for (s32 i = 0; i < size; i++) {
        result[i] = matrix[i][size];
    }
    return TRUE;
}

f32 moving_body_support(struct MovingBodyProfile *profile, Vec3f direction, Vec3f point) {
    f32 lateral = sqrtf(sqr(direction[0]) + sqr(direction[2]));
    f32 best = MAX(0.0f, profile->height * direction[1]);
    vec3f_set(point, 0.0f, direction[1] > 0.0f ? profile->height : 0.0f, 0.0f);
    for (s32 i = 0; i < 2; i++) {
        f32 value = profile->diskHeight[i] * direction[1] + profile->diskRadius[i] * lateral;
        if (value > best) {
            best = value;
            point[0] = lateral > 0.0f ? profile->diskRadius[i] * direction[0] / lateral : 0.0f;
            point[1] = profile->diskHeight[i];
            point[2] = lateral > 0.0f ? profile->diskRadius[i] * direction[2] / lateral : 0.0f;
        }
    }
    return best;
}

static void moving_minkowski_support(struct MovingBodyProfile *profile, Vec3f position,
                                     Vec3f vertices[3], Vec3f direction, Vec3f result) {
    Vec3f opposite, point;
    s32 vertex = 0;
    for (s32 i = 0; i < 3; i++) {
        opposite[i] = -direction[i];
    }
    moving_body_support(profile, opposite, point);
    for (s32 i = 1; i < 3; i++) {
        if (moving_dot(vertices[i], direction) > moving_dot(vertices[vertex], direction)) {
            vertex = i;
        }
    }
    for (s32 i = 0; i < 3; i++) {
        result[i] = vertices[vertex][i] - position[i] - point[i];
    }
}

static s32 moving_closest_simplex(Vec3f simplex[4], s32 *count, Vec3f closest) {
    f64 best = 1e30;
    s32 bestMask = 0;
    Vec3f candidate;
    for (s32 mask = 1; mask < (1 << *count); mask++) {
        s32 indices[4], size = 0;
        f64 weights[5];
        for (s32 i = 0; i < *count; i++) {
            if (mask & (1 << i)) {
                indices[size++] = i;
            }
        }
        // Vertices, segments and triangle faces are almost all GJK candidates.
        // Project directly in local edge coordinates instead of zeroing and
        // eliminating a double-precision matrix for every candidate subset.
        if (size == 1) {
            weights[0] = 1.0;
        } else if (size == 2) {
            f64 d00 = 0.0, a0 = 0.0;
            for (s32 axis = 0; axis < 3; axis++) {
                f64 a = simplex[indices[0]][axis];
                f64 edge0 = (f64) simplex[indices[1]][axis] - a;
                d00 += edge0 * edge0;
                a0 -= a * edge0;
            }
            if (d00 <= 0.0) {
                continue;
            }
            weights[1] = a0 / d00;
            weights[0] = 1.0 - weights[1];
        } else if (size == 3) {
            f64 a[3], edge0[3], edge1[3];
            f64 determinant = 0.0, numerator0 = 0.0, numerator1 = 0.0;
            for (s32 axis = 0; axis < 3; axis++) {
                a[axis] = simplex[indices[0]][axis];
                edge0[axis] = (f64) simplex[indices[1]][axis] - a[axis];
                edge1[axis] = (f64) simplex[indices[2]][axis] - a[axis];
            }
            // Cross products retain skinny faces near long wall edges. The
            // Gram determinant subtracts almost equal squared lengths there,
            // losing a valid projection and making GJK repeat the same simplex.
            for (s32 axis = 0; axis < 3; axis++) {
                s32 b = (axis + 1) % 3, c = (axis + 2) % 3;
                f64 normal = edge0[b] * edge1[c] - edge0[c] * edge1[b];
                determinant += normal * normal;
                numerator0 += (a[c] * edge1[b] - a[b] * edge1[c]) * normal;
                numerator1 += (edge0[c] * a[b] - edge0[b] * a[c]) * normal;
            }
            if (determinant <= 0.0) {
                continue;
            }
            weights[1] = numerator0 / determinant;
            weights[2] = numerator1 / determinant;
            weights[0] = 1.0 - weights[1] - weights[2];
        } else {
            f64 edge[3][3], cross[3][3], lengthSq[3] = { 0 }, determinant = 0.0;
            for (s32 i = 0; i < 3; i++) {
                for (s32 axis = 0; axis < 3; axis++) {
                    edge[i][axis] = (f64) simplex[indices[i + 1]][axis] - simplex[indices[0]][axis];
                    lengthSq[i] += edge[i][axis] * edge[i][axis];
                }
            }
            for (s32 i = 0; i < 3; i++) {
                s32 b = (i + 1) % 3, c = (i + 2) % 3;
                cross[i][0] = edge[b][1] * edge[c][2] - edge[b][2] * edge[c][1];
                cross[i][1] = edge[b][2] * edge[c][0] - edge[b][0] * edge[c][2];
                cross[i][2] = edge[b][0] * edge[c][1] - edge[b][1] * edge[c][0];
            }
            for (s32 axis = 0; axis < 3; axis++) {
                determinant += edge[0][axis] * cross[0][axis];
            }
            // Signed volumes give tetrahedron barycentrics. Flat tetrahedra
            // retain the face/edge candidates instead of an unstable inverse.
            if (determinant * determinant <= 1e-24 * lengthSq[0] * lengthSq[1] * lengthSq[2]) {
                continue;
            }
            weights[0] = 1.0;
            for (s32 i = 0; i < 3; i++) {
                f64 numerator = 0.0;
                for (s32 axis = 0; axis < 3; axis++) {
                    numerator -= (f64) simplex[indices[0]][axis] * cross[i][axis];
                }
                weights[i + 1] = numerator / determinant;
                weights[0] -= weights[i + 1];
            }
        }
        f64 preciseCandidate[3] = { 0, 0, 0 };
        s32 valid = TRUE;
        for (s32 i = 0; i < size; i++) {
            // A negative weight extrapolates outside the convex hull. Even a
            // tiny one can incorrectly beat every valid candidate on a long
            // edge; its boundary already has a separate subset candidate.
            if (weights[i] < 0.0) {
                valid = FALSE;
            }
            for (s32 axis = 0; axis < 3; axis++) {
                preciseCandidate[axis] += weights[i] * simplex[indices[i]][axis];
            }
        }
        for (s32 axis = 0; axis < 3; axis++) {
            candidate[axis] = preciseCandidate[axis];
        }
        // Keep small improvements until after choosing the winning subset.
        // Rounding each squared distance to float can pin the old simplex.
        f64 distance = preciseCandidate[0] * preciseCandidate[0]
                       + preciseCandidate[1] * preciseCandidate[1]
                       + preciseCandidate[2] * preciseCandidate[2];
        if (valid && distance < best) {
            best = distance;
            bestMask = mask;
            vec3f_copy(closest, candidate);
        }
    }
    if (!bestMask) {
        return FALSE;
    }
    s32 remaining = 0;
    for (s32 i = 0; i < *count; i++) {
        if (bestMask & (1 << i)) {
            // vec3f_copy evaluates its destination once per component.
            vec3f_copy(simplex[remaining], simplex[i]);
            remaining++;
        }
    }
    *count = remaining;
    return TRUE;
}

// A largest-radius disk can attain the horizontal lower bound at a finite
// vertical edge. Certify that witness before avoiding the generic distance
// solver; cones, tips, slanted edges and initial overlaps keep its diagnosis.
static f32 moving_vertical_edge_distance(struct MovingBodyProfile *profile, Vec3f position,
                                         Vec3f vertices[3], Vec3f faceNormal, Vec3f normal) {
    if (profile->height < 0.0f) {
        return -1.0f;
    }
    s32 largest = profile->diskRadius[1] > profile->diskRadius[0];
    f32 radius = profile->diskRadius[largest];
    if (radius <= 0.0f || profile->diskRadius[0] < 0.0f || profile->diskRadius[1] < 0.0f) {
        return -1.0f;
    }
    for (s32 disk = 0; disk < 2; disk++) {
        if (profile->diskHeight[disk] < 0.0f || profile->diskHeight[disk] > profile->height) {
            return -1.0f;
        }
    }
    f32 low = profile->diskHeight[largest], high = low;
    if (profile->diskRadius[0] == profile->diskRadius[1]) {
        low = MIN(profile->diskHeight[0], profile->diskHeight[1]);
        high = MAX(profile->diskHeight[0], profile->diskHeight[1]);
    }
    for (s32 edge = 0; edge < 3; edge++) {
        f32 *a = vertices[edge], *b = vertices[(edge + 1) % 3];
        f32 *other = vertices[(edge + 2) % 3];
        if (a[0] != b[0] || a[2] != b[2] || a[1] == b[1]) {
            continue;
        }
        f32 x = position[0] - a[0], z = position[2] - a[2];
        if ((f64) x * (other[0] - a[0]) + (f64) z * (other[2] - a[2]) > 0.0
            || x * faceNormal[0] + z * faceNormal[2] < 0.0f || position[1] + high < MIN(a[1], b[1])
            || position[1] + low > MAX(a[1], b[1])) {
            return -1.0f;
        }
        f32 distance = sqrtf(x * x + z * z);
        if (distance <= radius) {
            return -1.0f;
        }
        vec3f_set(normal, x / distance, 0.0f, z / distance);
        return distance - radius;
    }
    return -1.0f;
}

// GJK distance against a finite triangle includes its edges and vertices. A
// negative result means numerical uncertainty, never a certified collision.
static f32 moving_triangle_distance_with_normal(struct MovingBodyProfile *profile, Vec3f position,
                                                Vec3f vertices[3], Vec3f faceNormal, Vec3f normal) {
    Vec3f simplex[4], closest, direction, point;
    // The common face-interior case has an exact cheap witness. Edges, corners
    // and penetrations use the finite convex-distance calculation below.
    if (faceNormal != NULL) {
        Vec3f facePoint;
        vec3f_copy(normal, faceNormal);
        for (s32 axis = 0; axis < 3; axis++) {
            direction[axis] = -normal[axis];
        }
        moving_body_support(profile, direction, point);
        for (s32 axis = 0; axis < 3; axis++) {
            point[axis] += position[axis];
        }
        moving_sub(direction, point, vertices[0]);
        f32 gap = moving_dot(direction, normal);
        for (s32 axis = 0; axis < 3; axis++) {
            facePoint[axis] = point[axis] - gap * normal[axis];
        }
        s32 inside = TRUE;
        for (s32 edge = 0; edge < 3; edge++) {
            Vec3f a, b, cross;
            moving_sub(a, vertices[(edge + 1) % 3], vertices[edge]);
            moving_sub(b, facePoint, vertices[edge]);
            cross[0] = a[1] * b[2] - a[2] * b[1];
            cross[1] = a[2] * b[0] - a[0] * b[2];
            cross[2] = a[0] * b[1] - a[1] * b[0];
            if (moving_dot(cross, normal) < -0.0001f) {
                inside = FALSE;
            }
        }
        if (inside && gap >= 0.0f) {
            return gap;
        }
        if (faceNormal[1] == 0.0f && (profile->diskRadius[0] > 0.0f || profile->diskRadius[1] > 0.0f)) {
            f32 edgeDistance =
                moving_vertical_edge_distance(profile, position, vertices, faceNormal, normal);
            if (edgeDistance >= 0.0f) {
                return edgeDistance;
            }
        }
    }
    s32 count = 1;
    moving_sub(direction, vertices[0], position);
    if (moving_dot(direction, direction) < MOVING_MATH_EPSILON) {
        direction[1] = 1.0f;
    }
    moving_minkowski_support(profile, position, vertices, direction, simplex[0]);
    vec3f_copy(closest, simplex[0]);
    for (s32 attempt = 0; attempt < 32; attempt++) {
        f32 distanceSq = moving_dot(closest, closest);
        if (distanceSq < sqr(MOVING_CONTACT_SKIN * 0.25f)) {
            return 0.0f;
        }
        for (s32 i = 0; i < 3; i++) {
            direction[i] = -closest[i];
        }
        moving_minkowski_support(profile, position, vertices, direction, point);
        // The simplex distance is an upper bound; its support plane supplies
        // a lower bound. Advance using that lower bound, including when float
        // rounding prevents another simplex improvement near a curved probe.
        f32 distance = sqrtf(distanceSq);
        f32 lowerBound = MAX(0.0f, moving_precise_dot(closest, point) / distance);
        if (distance - lowerBound <= MOVING_CONTACT_SKIN * 0.125f) {
            for (s32 i = 0; i < 3; i++) {
                normal[i] = -closest[i] / distance;
            }
            return lowerBound;
        }
        s32 duplicate = FALSE;
        for (s32 i = 0; i < count; i++) {
            Vec3f difference;
            moving_sub(difference, point, simplex[i]);
            if (moving_dot(difference, difference) < 0.000001f) {
                duplicate = TRUE;
            }
        }
        if (duplicate) {
            for (s32 i = 0; i < 3; i++) {
                normal[i] = -closest[i] / distance;
            }
            return lowerBound;
        }
        if (count == 4) {
            return MOVING_CAST_INDETERMINATE;
        }
        vec3f_copy(simplex[count], point);
        count++;
        if (!moving_closest_simplex(simplex, &count, closest)) {
            return MOVING_CAST_INDETERMINATE;
        }
    }
    return MOVING_CAST_INDETERMINATE;
}

f32 moving_triangle_distance(struct MovingBodyProfile *profile, Vec3f position, Vec3f vertices[3],
                             Vec3f normal) {
    s32 valid = moving_triangle_normal(vertices, normal);
    return moving_triangle_distance_with_normal(profile, position, vertices, valid ? normal : NULL,
                                                normal);
}

static s32 moving_triangle_normal(Vec3f vertices[3], Vec3f normal) {
    Vec3f a, b;
    moving_sub(a, vertices[1], vertices[0]);
    moving_sub(b, vertices[2], vertices[0]);
    normal[0] = a[1] * b[2] - a[2] * b[1];
    normal[1] = a[2] * b[0] - a[0] * b[2];
    normal[2] = a[0] * b[1] - a[1] * b[0];
    f32 length = sqrtf(moving_dot(normal, normal));
    if (length < MOVING_MATH_EPSILON) {
        return FALSE;
    }
    for (s32 i = 0; i < 3; i++) {
        normal[i] /= length;
    }
    return TRUE;
}

s32 moving_triangle_cast(struct MovingBodyProfile *profile, Vec3f start, Vec3f end, Vec3f before[3],
                         Vec3f after[3], f32 *fraction, Vec3f normal) {
    Vec3f delta, position, movingVertices[3], faceNormal, support;
    Vec3f *vertices = before;
    s32 fixedTriangle = (before == after);
    f32 speed, time = 0.0f;
    if (!moving_triangle_normal(before, faceNormal)) {
        return MOVING_CAST_CLEAR;
    }
    moving_sub(delta, start, before[0]);
    // One-sided geometry: a body wholly behind the initial plane is not swept
    // back through it. Initial partial overlaps are handled as recovery only.
    if (moving_dot(delta, faceNormal) + moving_body_support(profile, faceNormal, support)
        < -MOVING_CONTACT_SKIN) {
        return MOVING_CAST_CLEAR;
    }
    moving_sub(delta, end, start);
    Vec3f bodyDelta;
    vec3f_copy(bodyDelta, delta);
    // Enemy sweeps pass the same current triangle for both endpoints, even
    // for dynamic surfaces. Reuse its geometry within this cast only; the
    // next update still observes any object movement or rebuilt surface.
    if (fixedTriangle) {
        speed = sqrtf(moving_dot(bodyDelta, bodyDelta));
    } else {
        vertices = movingVertices;
        speed = 0.0f;
        for (s32 vertex = 0; vertex < 3; vertex++) {
            for (s32 axis = 0; axis < 3; axis++) {
                delta[axis] = after[vertex][axis] - before[vertex][axis] - bodyDelta[axis];
            }
            speed = MAX(speed, sqrtf(moving_dot(delta, delta)));
        }
    }
    for (s32 attempt = 0; attempt < 96; attempt++) {
        for (s32 axis = 0; axis < 3; axis++) {
            position[axis] = start[axis] + time * (end[axis] - start[axis]);
            if (!fixedTriangle) {
                for (s32 vertex = 0; vertex < 3; vertex++) {
                    vertices[vertex][axis] =
                        before[vertex][axis] + time * (after[vertex][axis] - before[vertex][axis]);
                }
            }
        }
        f32 distance = fixedTriangle ? moving_triangle_distance_with_normal(profile, position, vertices,
                                                                            faceNormal, normal)
                                     : moving_triangle_distance(profile, position, vertices, normal);
        if (distance < 0.0f) {
            return MOVING_CAST_INDETERMINATE;
        }
        if (distance <= MOVING_CONTACT_SKIN) {
            if (distance == 0.0f) {
                if (fixedTriangle) {
                    vec3f_copy(normal, faceNormal);
                } else if (!moving_triangle_normal(vertices, normal)) {
                    return MOVING_CAST_INDETERMINATE;
                }
            }
            // Edge normals behind a one-sided face do not become back walls.
            if (moving_dot(normal, faceNormal) < -0.01f) {
                return MOVING_CAST_CLEAR;
            }
            *fraction = time;
            return MOVING_CAST_HIT;
        }
        if (speed < MOVING_MATH_EPSILON) {
            return MOVING_CAST_CLEAR;
        }
        // Advance by the separating plane's closing speed. Using total speed
        // stalls on a grazing edge or when Mario is already following a face.
        f32 closing = 0.0f;
        if (fixedTriangle) {
            closing = MAX(0.0f, -moving_dot(normal, bodyDelta));
        } else {
            for (s32 vertex = 0; vertex < 3; vertex++) {
                for (s32 axis = 0; axis < 3; axis++) {
                    delta[axis] = after[vertex][axis] - before[vertex][axis] - bodyDelta[axis];
                }
                closing = MAX(closing, moving_dot(normal, delta));
            }
        }
        if (closing < MOVING_MATH_EPSILON) {
            return MOVING_CAST_CLEAR;
        }
        f32 increment = (distance - MOVING_CONTACT_SKIN * 0.5f) / closing;
        if (time + increment > 1.0f) {
            return MOVING_CAST_CLEAR;
        }
        if (increment < 0.000001f) {
            return MOVING_CAST_INDETERMINATE;
        }
        time += increment;
    }
    return MOVING_CAST_INDETERMINATE;
}

static s32 moving_position_satisfies(Vec3f position, struct MovingContactPlane *planes, s32 count,
                                     f32 fraction) {
    for (s32 i = 0; i < count; i++) {
        f32 offset = planes[i].offset - (1.0f - fraction) * planes[i].support;
        if (moving_dot(planes[i].normal, position) < offset - 0.002f) {
            return FALSE;
        }
    }
    return TRUE;
}

static void moving_projection_candidate(Vec3f desired, struct MovingContactPlane *planes, s32 count,
                                        f32 fraction, s32 indices[3], s32 size, f32 *bestDistance,
                                        Vec3f result) {
    f64 matrix[5][6] = { { 0 } }, weights[5];
    Vec3f candidate;
    for (s32 i = 0; i < size; i++) {
        struct MovingContactPlane *p = &planes[indices[i]];
        for (s32 j = 0; j < size; j++) {
            matrix[i][j] = moving_dot(p->normal, planes[indices[j]].normal);
        }
        matrix[i][size] = p->offset - (1.0f - fraction) * p->support - moving_dot(p->normal, desired);
    }
    if (!moving_solve(matrix, size, weights)) {
        return;
    }
    vec3f_copy(candidate, desired);
    for (s32 i = 0; i < size; i++) {
        if (weights[i] < -0.002f) {
            return;
        }
        for (s32 axis = 0; axis < 3; axis++) {
            candidate[axis] += weights[i] * planes[indices[i]].normal[axis];
        }
    }
    if (!moving_position_satisfies(candidate, planes, count, fraction)) {
        return;
    }
    Vec3f difference;
    moving_sub(difference, candidate, desired);
    f32 distance = moving_dot(difference, difference);
    if (distance < *bestDistance) {
        *bestDistance = distance;
        vec3f_copy(result, candidate);
    }
}

s32 moving_project_position(Vec3f desired, struct MovingContactPlane *planes, s32 count,
                            f32 bodyFraction, Vec3f result) {
    f32 bestDistance = 1e30f;
    s32 indices[3];
    if (moving_position_satisfies(desired, planes, count, bodyFraction)) {
        vec3f_copy(result, desired);
        return TRUE;
    }
    // The nearest point in a nonempty 3D intersection has at most three
    // independent active planes. Validate every candidate against all planes.
    for (s32 i = 0; i < count; i++) {
        indices[0] = i;
        moving_projection_candidate(desired, planes, count, bodyFraction, indices, 1, &bestDistance,
                                    result);
        for (s32 j = i + 1; j < count; j++) {
            indices[1] = j;
            moving_projection_candidate(desired, planes, count, bodyFraction, indices, 2, &bestDistance,
                                        result);
            for (s32 k = j + 1; k < count; k++) {
                indices[2] = k;
                moving_projection_candidate(desired, planes, count, bodyFraction, indices, 3,
                                            &bestDistance, result);
            }
        }
    }
    return bestDistance < 1e30f;
}

f32 moving_contact_clearance(Vec3f desired, struct MovingContactPlane *planes, s32 count) {
    Vec3f result;
    f32 low = 0.0f, high = 1.0f;
    if (moving_project_position(desired, planes, count, 1.0f, result)) {
        return 161.0f;
    }
    if (!moving_project_position(desired, planes, count, 0.0f, result)) {
        return 0.0f;
    }
    // Monotonic feasibility yields the same maximum uniform fraction as the
    // four-variable clearance LP, without an additional fragile 4D solver.
    for (s32 iteration = 0; iteration < 18; iteration++) {
        f32 middle = 0.5f * (low + high);
        if (moving_project_position(desired, planes, count, middle, result)) {
            low = middle;
        } else {
            high = middle;
        }
    }
    return 160.0f * low;
}
