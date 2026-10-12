#ifndef COLLISION_MESH_H
#define COLLISION_MESH_H

#include "types.h"

// Experimental backend. Storage belongs to the caller and outlives every instance.
// No Surface/Object pointers are retained; triangle IDs survive spatial reordering.
struct CollisionMeshTriangle {
    u16 vertices[3];
    s16 type, force;
    u16 valid;
    Vec3f normal;
};

struct CollisionMeshNode {
    Vec3s minimum, maximum;
    u16 first; // Leaf reference offset, or the node after an internal subtree.
    u16 count; // Zero for internal nodes.
};

struct CollisionMesh {
    Vec3s *vertices;
    struct CollisionMeshTriangle *triangles;
    struct CollisionMeshNode *nodes;
    u16 *references;
    u16 vertexCount, triangleCount, nodeCount, nodeCapacity;
};

struct CollisionMeshInstance {
    const struct CollisionMesh *mesh;
    Mat4 transform, inverse;
    Vec3f minimum, maximum;
    f32 orientation;
};

enum CollisionMeshQueryFlags {
    COLLISION_MESH_FLOORS = (1 << 0),
    COLLISION_MESH_CAMERA = (1 << 1),
    COLLISION_MESH_INCLUDE_INTANGIBLE = (1 << 2),
    COLLISION_MESH_TWO_SIDED = (1 << 3)
};

struct CollisionMeshHit {
    f32 fraction;
    Vec3f position, normal;
    u16 triangle;
    s16 type, force;
};

// Reads only the s16 vertex/triangle prefix. wordCount is the accessible length;
// stride is uniformly 3 or 4 words per triangle. Storage must not overlap input.
// Returns required bytes, or zero for an invalid/empty mesh.
u32 collision_mesh_storage_size(const TerrainData *data, u32 wordCount, u32 stride);
// Storage is eight-byte aligned. Returns TRUE on success; FALSE clears mesh.
s32 collision_mesh_build(struct CollisionMesh *mesh, const TerrainData *data, u32 wordCount, u32 stride,
                         void *storage, u32 storageBytes);
// Copies the effective affine transform, including scale. A singular transform
// disables the instance rather than leaving an older pose queryable.
s32 collision_mesh_instance_update(struct CollisionMeshInstance *instance,
                                   const struct CollisionMesh *mesh, Mat4 transform);
// Endpoints and hit positions use world units. TRUE writes the nearest hit,
// including a segment fraction in [0, 1] and a unit normal; FALSE leaves hit unchanged.
s32 collision_mesh_raycast(const struct CollisionMeshInstance *instance, const Vec3f start,
                           const Vec3f end, u32 flags, struct CollisionMeshHit *hit);

#endif
