#include <math.h>
#include <stddef.h>
#include <string.h>

#include "collision_mesh.h"
#include "surface_terrains.h"

#define MESH_LEAF_TRIANGLES 4
#define MESH_MAX_TRIANGLES 16384
#define MESH_ALIGN(bytes) (((bytes) + 7u) & ~7u)

struct MeshLayout {
    u32 vertices, triangles, nodes, bytes;
};

// The game uses -ffinite-math-only; inspect bits so rejected transforms still
// invalidate the instance in target builds, not only in host tests.
static s32 mesh_finite(f32 value) {
    u32 bits;
    memcpy(&bits, &value, sizeof(bits));
    return (bits & 0x7f800000u) != 0x7f800000u;
}

static s32 mesh_layout(const TerrainData *data, u32 words, u32 stride, struct MeshLayout *layout) {
    memset(layout, 0, sizeof(*layout));
    if (data == NULL || words < 3 || (stride != 3 && stride != 4) || data[0] != TERRAIN_LOAD_VERTICES
        || data[1] <= 0) {
        return 0;
    }
    layout->vertices = data[1];
    u32 cursor = 2 + 3 * layout->vertices;
    if (cursor >= words) {
        return 0;
    }
    while (data[cursor] != TERRAIN_LOAD_CONTINUE) {
        s32 type = data[cursor];
        if (cursor + 1 >= words || type < 0 || (type >= 0x40 && type < 0x65)) {
            return 0;
        }
        s32 count = data[cursor + 1];
        cursor += 2;
        if (count < 0 || (u32) count > (words - cursor) / stride
            || layout->triangles + count > MESH_MAX_TRIANGLES) {
            return 0;
        }
        for (s32 i = 0; i < count; i++) {
            for (s32 v = 0; v < 3; v++) {
                if (data[cursor + v] < 0 || (u32) data[cursor + v] >= layout->vertices) {
                    return 0;
                }
            }
            cursor += stride;
        }
        layout->triangles += count;
        if (cursor >= words) {
            return 0;
        }
    }
    if (layout->triangles == 0) {
        return 0;
    }
    u32 leaves = 1;
    while (leaves < (layout->triangles + MESH_LEAF_TRIANGLES - 1) / MESH_LEAF_TRIANGLES) {
        leaves *= 2;
    }
    layout->nodes = 2 * leaves - 1;
    layout->bytes = MESH_ALIGN(layout->vertices * sizeof(Vec3s))
                    + MESH_ALIGN(layout->triangles * sizeof(struct CollisionMeshTriangle))
                    + MESH_ALIGN(layout->triangles * sizeof(u16))
                    + layout->nodes * sizeof(struct CollisionMeshNode);
    return 1;
}

u32 collision_mesh_storage_size(const TerrainData *data, u32 wordCount, u32 stride) {
    struct MeshLayout layout;
    return mesh_layout(data, wordCount, stride, &layout) ? layout.bytes : 0;
}

static s32 mesh_centroid(const struct CollisionMesh *mesh, u16 triangle, s32 axis) {
    const struct CollisionMeshTriangle *face = &mesh->triangles[triangle];
    return mesh->vertices[face->vertices[0]][axis] + mesh->vertices[face->vertices[1]][axis]
           + mesh->vertices[face->vertices[2]][axis];
}

static s32 mesh_reference_before(const struct CollisionMesh *mesh, u16 a, u16 b, s32 axis) {
    s32 first = mesh_centroid(mesh, a, axis), second = mesh_centroid(mesh, b, axis);
    return first < second || (first == second && a < b);
}

// Median partition bounds tree depth even for coincident or extremely thin faces.
static void mesh_partition(struct CollisionMesh *mesh, u32 first, u32 end, u32 middle, s32 axis) {
    while (end - first > 1) {
        u32 pivotIndex = first + (end - first) / 2;
        u16 pivot = mesh->references[pivotIndex];
        mesh->references[pivotIndex] = mesh->references[end - 1];
        u32 split = first;
        for (u32 i = first; i + 1 < end; i++) {
            if (mesh_reference_before(mesh, mesh->references[i], pivot, axis)) {
                u16 swap = mesh->references[split];
                mesh->references[split++] = mesh->references[i];
                mesh->references[i] = swap;
            }
        }
        mesh->references[end - 1] = mesh->references[split];
        mesh->references[split] = pivot;
        if (split == middle) {
            return;
        }
        if (split < middle) {
            first = split + 1;
        } else {
            end = split;
        }
    }
}

static void mesh_build_node(struct CollisionMesh *mesh, u32 first, u32 count) {
    struct CollisionMeshNode *node = &mesh->nodes[mesh->nodeCount++];
    for (s32 axis = 0; axis < 3; axis++) {
        node->minimum[axis] = 32767;
        node->maximum[axis] = -32768;
    }
    for (u32 i = first; i < first + count; i++) {
        const struct CollisionMeshTriangle *face = &mesh->triangles[mesh->references[i]];
        for (s32 v = 0; v < 3; v++) {
            for (s32 axis = 0; axis < 3; axis++) {
                s16 value = mesh->vertices[face->vertices[v]][axis];
                if (value < node->minimum[axis]) {
                    node->minimum[axis] = value;
                }
                if (value > node->maximum[axis]) {
                    node->maximum[axis] = value;
                }
            }
        }
    }
    if (count <= MESH_LEAF_TRIANGLES) {
        node->first = first;
        node->count = count;
        return;
    }
    s32 axis = 0;
    for (s32 i = 1; i < 3; i++) {
        if (node->maximum[i] - node->minimum[i] > node->maximum[axis] - node->minimum[axis]) {
            axis = i;
        }
    }
    u32 left = count / 2;
    mesh_partition(mesh, first, first + count, first + left, axis);
    mesh_build_node(mesh, first, left);
    mesh_build_node(mesh, first + left, count - left);
    node->first = mesh->nodeCount;
    node->count = 0;
}

static void mesh_triangle_plane(struct CollisionMesh *mesh, struct CollisionMeshTriangle *face) {
    const s16 *a = mesh->vertices[face->vertices[0]];
    const s16 *b = mesh->vertices[face->vertices[1]];
    const s16 *c = mesh->vertices[face->vertices[2]];
    Vec3f ab, ac;
    for (s32 axis = 0; axis < 3; axis++) {
        ab[axis] = (f32) b[axis] - a[axis];
        ac[axis] = (f32) c[axis] - a[axis];
    }
    face->normal[0] = ab[1] * ac[2] - ab[2] * ac[1];
    face->normal[1] = ab[2] * ac[0] - ab[0] * ac[2];
    face->normal[2] = ab[0] * ac[1] - ab[1] * ac[0];
    f32 squared = face->normal[0] * face->normal[0] + face->normal[1] * face->normal[1]
                  + face->normal[2] * face->normal[2];
    face->valid = squared > 0.0f;
    if (face->valid) {
        f32 inverse = 1.0f / sqrtf(squared);
        for (s32 axis = 0; axis < 3; axis++) {
            face->normal[axis] *= inverse;
        }
    }
}

s32 collision_mesh_build(struct CollisionMesh *mesh, const TerrainData *data, u32 wordCount, u32 stride,
                         void *storage, u32 storageBytes) {
    struct MeshLayout layout;
    memset(mesh, 0, sizeof(*mesh));
    if (!mesh_layout(data, wordCount, stride, &layout) || storage == NULL
        || ((uintptr_t) storage & 7u) != 0 || storageBytes < layout.bytes) {
        return 0;
    }
    u8 *cursor = storage;
    mesh->vertices = (Vec3s *) cursor;
    cursor += MESH_ALIGN(layout.vertices * sizeof(Vec3s));
    mesh->triangles = (struct CollisionMeshTriangle *) cursor;
    cursor += MESH_ALIGN(layout.triangles * sizeof(struct CollisionMeshTriangle));
    mesh->references = (u16 *) cursor;
    cursor += MESH_ALIGN(layout.triangles * sizeof(u16));
    mesh->nodes = (struct CollisionMeshNode *) cursor;
    mesh->vertexCount = layout.vertices;
    mesh->triangleCount = layout.triangles;
    mesh->nodeCapacity = layout.nodes;
    for (u32 i = 0; i < layout.vertices; i++) {
        for (s32 axis = 0; axis < 3; axis++) {
            mesh->vertices[i][axis] = data[2 + 3 * i + axis];
        }
    }
    u32 word = 2 + 3 * layout.vertices;
    u32 triangle = 0;
    while (data[word] != TERRAIN_LOAD_CONTINUE) {
        s16 type = data[word++];
        s32 count = data[word++];
        for (s32 i = 0; i < count; i++, triangle++) {
            struct CollisionMeshTriangle *face = &mesh->triangles[triangle];
            for (s32 v = 0; v < 3; v++) {
                face->vertices[v] = data[word++];
            }
            face->force = stride == 4 ? data[word++] : 0;
            face->type = type;
            mesh->references[triangle] = triangle;
            mesh_triangle_plane(mesh, face);
        }
    }
    mesh_build_node(mesh, 0, mesh->triangleCount);
    return 1;
}

static void mesh_transform_point(const Mat4 matrix, const Vec3f input, Vec3f output) {
    for (s32 axis = 0; axis < 3; axis++) {
        output[axis] = matrix[3][axis] + matrix[0][axis] * input[0] + matrix[1][axis] * input[1]
                       + matrix[2][axis] * input[2];
    }
}

s32 collision_mesh_instance_update(struct CollisionMeshInstance *instance,
                                   const struct CollisionMesh *mesh, Mat4 transform) {
    instance->mesh = NULL;
    if (mesh == NULL || mesh->nodeCount == 0) {
        return 0;
    }
    for (s32 column = 0; column < 4; column++) {
        for (s32 row = 0; row < 4; row++) {
            if (!mesh_finite(transform[column][row])) {
                return 0;
            }
        }
    }
    if (transform[0][3] != 0.0f || transform[1][3] != 0.0f || transform[2][3] != 0.0f
        || transform[3][3] != 1.0f) {
        return 0;
    }
    f32 a = transform[0][0], b = transform[1][0], c = transform[2][0];
    f32 d = transform[0][1], e = transform[1][1], f = transform[2][1];
    f32 g = transform[0][2], h = transform[1][2], i = transform[2][2];
    f32 determinant = a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
    if (!mesh_finite(determinant) || determinant == 0.0f) {
        return 0;
    }
    f32 reciprocal = 1.0f / determinant;
    instance->orientation = determinant < 0.0f ? -1.0f : 1.0f;
    memcpy(instance->transform, transform, sizeof(Mat4));
    memset(instance->inverse, 0, sizeof(Mat4));
    instance->inverse[0][0] = (e * i - f * h) * reciprocal;
    instance->inverse[1][0] = (c * h - b * i) * reciprocal;
    instance->inverse[2][0] = (b * f - c * e) * reciprocal;
    instance->inverse[0][1] = (f * g - d * i) * reciprocal;
    instance->inverse[1][1] = (a * i - c * g) * reciprocal;
    instance->inverse[2][1] = (c * d - a * f) * reciprocal;
    instance->inverse[0][2] = (d * h - e * g) * reciprocal;
    instance->inverse[1][2] = (b * g - a * h) * reciprocal;
    instance->inverse[2][2] = (a * e - b * d) * reciprocal;
    instance->inverse[3][3] = 1.0f;
    for (s32 axis = 0; axis < 3; axis++) {
        instance->inverse[3][axis] = -(instance->inverse[0][axis] * transform[3][0]
                                       + instance->inverse[1][axis] * transform[3][1]
                                       + instance->inverse[2][axis] * transform[3][2]);
        instance->minimum[axis] = 1e30f;
        instance->maximum[axis] = -1e30f;
    }
    for (s32 column = 0; column < 4; column++) {
        for (s32 row = 0; row < 3; row++) {
            if (!mesh_finite(instance->inverse[column][row])) {
                return 0;
            }
        }
    }
    for (s32 corner = 0; corner < 8; corner++) {
        Vec3f local, world;
        for (s32 axis = 0; axis < 3; axis++) {
            local[axis] =
                (corner & (1 << axis)) ? mesh->nodes[0].maximum[axis] : mesh->nodes[0].minimum[axis];
        }
        mesh_transform_point(transform, local, world);
        for (s32 axis = 0; axis < 3; axis++) {
            if (!mesh_finite(world[axis])) {
                return 0;
            }
            if (world[axis] < instance->minimum[axis]) {
                instance->minimum[axis] = world[axis];
            }
            if (world[axis] > instance->maximum[axis]) {
                instance->maximum[axis] = world[axis];
            }
        }
    }
    instance->mesh = mesh;
    return 1;
}

static s32 mesh_node_intersects(const struct CollisionMeshNode *node, const Vec3f start,
                                const Vec3f direction, const Vec3f reciprocal, f32 limit) {
    f32 near = 0.0f, far = limit;
    for (s32 axis = 0; axis < 3; axis++) {
        if (direction[axis] == 0.0f) {
            if (start[axis] < node->minimum[axis] || start[axis] > node->maximum[axis]) {
                return 0;
            }
        } else {
            f32 first = (node->minimum[axis] - start[axis]) * reciprocal[axis];
            f32 second = (node->maximum[axis] - start[axis]) * reciprocal[axis];
            if (first > second) {
                f32 swap = first;
                first = second;
                second = swap;
            }
            if (first > near) {
                near = first;
            }
            if (second < far) {
                far = second;
            }
            if (near > far) {
                return 0;
            }
        }
    }
    return 1;
}

static void mesh_world_normal(const struct CollisionMeshInstance *instance, const Vec3f local,
                              Vec3f world) {
    for (s32 axis = 0; axis < 3; axis++) {
        world[axis] = instance->orientation
                      * (instance->inverse[axis][0] * local[0] + instance->inverse[axis][1] * local[1]
                         + instance->inverse[axis][2] * local[2]);
    }
}

static s32 mesh_triangle_eligible(const struct CollisionMeshInstance *instance,
                                  const struct CollisionMeshTriangle *face, u32 flags) {
    if (!face->valid || SURFACE_IS_NEW_WATER(face->type)) {
        return 0;
    }
    if (flags & COLLISION_MESH_CAMERA) {
        if (face->type == SURFACE_NO_CAM_COLLISION || face->type == SURFACE_NO_CAM_COLLISION_77
            || face->type == SURFACE_NO_CAM_COL_VERY_SLIPPERY || face->type == SURFACE_SWITCH) {
            return 0;
        }
    } else if (face->type == SURFACE_CAMERA_BOUNDARY) {
        return 0;
    }
    if (face->type == SURFACE_INTANGIBLE && !(flags & COLLISION_MESH_INCLUDE_INTANGIBLE)) {
        return 0;
    }
    if (flags & COLLISION_MESH_FLOORS) {
        Vec3f normal;
        mesh_world_normal(instance, face->normal, normal);
        f32 squared = normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2];
        return normal[1] > 0.0f && normal[1] * normal[1] > 0.0001f * squared;
    }
    return 1;
}

static s32 mesh_triangle_contains(const struct CollisionMesh *mesh,
                                  const struct CollisionMeshTriangle *face, const Vec3f point) {
    for (s32 edge = 0; edge < 3; edge++) {
        const s16 *a = mesh->vertices[face->vertices[edge]];
        const s16 *b = mesh->vertices[face->vertices[(edge + 1) % 3]];
        Vec3f ab, ap;
        for (s32 axis = 0; axis < 3; axis++) {
            ab[axis] = (f32) b[axis] - a[axis];
            ap[axis] = point[axis] - a[axis];
        }
        f32 side = (ab[1] * ap[2] - ab[2] * ap[1]) * face->normal[0]
                   + (ab[2] * ap[0] - ab[0] * ap[2]) * face->normal[1]
                   + (ab[0] * ap[1] - ab[1] * ap[0]) * face->normal[2];
        if (side < 0.0f) {
            return 0;
        }
    }
    return 1;
}

s32 collision_mesh_raycast(const struct CollisionMeshInstance *instance, const Vec3f start,
                           const Vec3f end, u32 flags, struct CollisionMeshHit *hit) {
    if (instance->mesh == NULL) {
        return 0;
    }
    const struct CollisionMesh *mesh = instance->mesh;
    Vec3f localStart, localEnd, direction, reciprocal;
    mesh_transform_point(instance->inverse, start, localStart);
    mesh_transform_point(instance->inverse, end, localEnd);
    for (s32 axis = 0; axis < 3; axis++) {
        if (!mesh_finite(localStart[axis]) || !mesh_finite(localEnd[axis])) {
            return 0;
        }
        direction[axis] = localEnd[axis] - localStart[axis];
        reciprocal[axis] = direction[axis] == 0.0f ? 0.0f : 1.0f / direction[axis];
        if (!mesh_finite(direction[axis]) || !mesh_finite(reciprocal[axis])) {
            return 0;
        }
    }
    f32 nearest = 1.0f;
    s32 winner = -1;
    u32 index = 0;
    while (index < mesh->nodeCount) {
        const struct CollisionMeshNode *node = &mesh->nodes[index];
        if (!mesh_node_intersects(node, localStart, direction, reciprocal, nearest)) {
            index = node->count == 0 ? node->first : index + 1;
            continue;
        }
        index++;
        for (u32 i = node->first; node->count != 0 && i < node->first + node->count; i++) {
            u16 ordinal = mesh->references[i];
            const struct CollisionMeshTriangle *face = &mesh->triangles[ordinal];
            if (!mesh_triangle_eligible(instance, face, flags)) {
                continue;
            }
            const s16 *anchor = mesh->vertices[face->vertices[0]];
            f32 denominator = 0.0f, distance = 0.0f;
            for (s32 axis = 0; axis < 3; axis++) {
                denominator += face->normal[axis] * direction[axis];
                distance += face->normal[axis] * (localStart[axis] - anchor[axis]);
            }
            if (denominator == 0.0f
                || (!(flags & COLLISION_MESH_TWO_SIDED)
                    && denominator * instance->orientation >= 0.0f)) {
                continue;
            }
            f32 fraction = -distance / denominator;
            if (fraction < 0.0f || fraction > nearest
                || (fraction == nearest && winner >= 0 && ordinal >= winner)) {
                continue;
            }
            Vec3f point;
            for (s32 axis = 0; axis < 3; axis++) {
                point[axis] = localStart[axis] + fraction * direction[axis];
            }
            if (mesh_triangle_contains(mesh, face, point)) {
                nearest = fraction;
                winner = ordinal;
            }
        }
    }
    if (winner < 0) {
        return 0;
    }
    const struct CollisionMeshTriangle *face = &mesh->triangles[winner];
    hit->fraction = nearest;
    hit->triangle = winner;
    hit->type = face->type;
    hit->force = face->force;
    mesh_world_normal(instance, face->normal, hit->normal);
    f32 inverse = 1.0f
                  / sqrtf(hit->normal[0] * hit->normal[0] + hit->normal[1] * hit->normal[1]
                          + hit->normal[2] * hit->normal[2]);
    for (s32 axis = 0; axis < 3; axis++) {
        hit->normal[axis] *= inverse;
        hit->position[axis] = start[axis] + nearest * (end[axis] - start[axis]);
    }
    return 1;
}
