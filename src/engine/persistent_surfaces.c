#include <math.h>
#include <string.h>

#include "persistent_surfaces.h"

#if (PERSISTENT_DYNAMIC_SURFACES || DORRIE_STATIONARY_SURFACES || JRB_STATIONARY_SURFACES              \
     || BOWSER_STATIONARY_SURFACES)                                                                    \
    && defined(ALL_SURFACES_HAVE_FORCE)
#include "math_util.h"
#include "surface_load.h"
#include "surface_terrains.h"

#define MESH_SLOTS 2
#ifndef PERSISTENT_SURFACE_SLOT_BYTES
#define PERSISTENT_SURFACE_SLOT_BYTES 0x4000
#endif
#define MESH_BYTES PERSISTENT_SURFACE_SLOT_BYTES
#define MAX_MESH_VERTICES 200
#define MAX_MESH_TRIANGLES 256
#define MIN_MESH_TRIANGLES 96
#define MAX_MESH_CELLS 64
#define CELL_HASH_SIZE (MAX_MESH_CELLS * 2)

struct MeshTriangle {
    u16 vertex[3];
    u8 minX, maxX, minZ, maxZ;
    u8 partition;
    u8 valid;
};

struct MeshCell {
    u16 key;
    u16 first;
    u16 count;
    u16 capacity;
};

struct PersistentSurfaces {
    struct Object *object;
    const TerrainData *data;
    u32 publishedFrame;
    u32 usedBytes;
    Mat4 transform;
    Vec3t *localVertices;
    Vec3t *worldVertices;
    struct MeshTriangle *triangles;
    struct Surface *surfaces;
    struct MeshCell *cells;
    u16 *references;
    struct SurfaceNode *nodes;
    u16 vertexCount;
    u16 triangleCount;
    u16 cellCount;
    u16 referenceCount;
    u16 referenceUsed;
    u16 referenceCapacity;
    s16 cellHash[CELL_HASH_SIZE];
    u8 geometryReady;
    u8 membershipReady;
    union {
        u64 alignment;
        u8 bytes[MESH_BYTES];
    } storage;
};

static struct PersistentSurfaces sMeshes[MESH_SLOTS];
static u32 sFrame = 1;

#if DORRIE_STATIONARY_SURFACES
static struct {
    struct Object *object;
    const TerrainData *data;
    Mat4 transform;
    Vec3f scale;
} sDorriePose;
#endif

void persistent_surfaces_begin_frame(void) {
    if (++sFrame == 0) {
        // No old lists remain reachable at this point, including on counter wrap.
        for (u32 i = 0; i < MESH_SLOTS; i++) {
            sMeshes[i].publishedFrame = 0;
        }
        sFrame = 1;
    }
}

void persistent_surfaces_reset(void) {
    // Area/object teardown may precede the next grid clear. Retire ownership now;
    // already published storage becomes reusable only after that clear.
    for (u32 i = 0; i < MESH_SLOTS; i++) {
        sMeshes[i].object = NULL;
    }
#if DORRIE_STATIONARY_SURFACES
    sDorriePose.object = NULL;
#endif
}

void persistent_surfaces_forget(struct Object *object) {
    for (u32 i = 0; i < MESH_SLOTS; i++) {
        if (sMeshes[i].object == object) {
            // Published nodes must survive until the next clear, even after deletion.
            sMeshes[i].object = NULL;
        }
    }
#if DORRIE_STATIONARY_SURFACES
    if (sDorriePose.object == object) {
        sDorriePose.object = NULL;
    }
#endif
}

u32 persistent_surfaces_reserved_bytes(void) {
    u32 bytes = sizeof(sMeshes);
#if DORRIE_STATIONARY_SURFACES
    bytes += sizeof(sDorriePose);
#endif
    return bytes;
}

u32 persistent_surfaces_used_bytes(void) {
    u32 bytes = 0;
    for (u32 i = 0; i < MESH_SLOTS; i++) {
        if (sMeshes[i].object != NULL || sMeshes[i].publishedFrame == sFrame) {
            bytes += sMeshes[i].usedBytes;
        }
    }
    return bytes;
}

u32 persistent_surfaces_published_meshes(void) {
    u32 meshes = 0;
    for (u32 i = 0; i < MESH_SLOTS; i++) {
        meshes += sMeshes[i].publishedFrame == sFrame;
    }
    return meshes;
}

static void *mesh_storage(struct PersistentSurfaces *mesh, u32 *offset, u32 bytes) {
    u32 start = (*offset + 7) & ~7u;
    if (start > MESH_BYTES || bytes > MESH_BYTES - start) {
        return NULL;
    }
    *offset = start + bytes;
    return mesh->storage.bytes + start;
}

static s32 decode_mesh(struct PersistentSurfaces *mesh, struct Object *object, const TerrainData *data,
                       s32 smallMesh) {
    u32 vertices = data[1];
    if (data[0] != TERRAIN_LOAD_VERTICES || vertices < (smallMesh ? 1u : 64u)
        || vertices > MAX_MESH_VERTICES) {
        return FALSE;
    }

    const TerrainData *groups = data + 2 + vertices * 3;
    const TerrainData *cursor = groups;
    u32 triangles = 0;
    for (u32 group = 0; *cursor != TERRAIN_LOAD_CONTINUE; group++) {
        s32 count = cursor[1];
        if (group == MAX_MESH_TRIANGLES || count < 0 || (u32) count > MAX_MESH_TRIANGLES - triangles) {
            return FALSE;
        }
        triangles += count;
        cursor += 2 + count * 4;
    }
    if (triangles < (smallMesh ? 1u : MIN_MESH_TRIANGLES)) {
        return FALSE;
    }

    u32 offset = 0;
    mesh->localVertices = mesh_storage(mesh, &offset, vertices * sizeof(Vec3t));
    mesh->worldVertices = mesh_storage(mesh, &offset, vertices * sizeof(Vec3t));
    mesh->triangles = mesh_storage(mesh, &offset, triangles * sizeof(struct MeshTriangle));
    mesh->surfaces = mesh_storage(mesh, &offset, triangles * sizeof(struct Surface));
    mesh->cells = mesh_storage(mesh, &offset, MAX_MESH_CELLS * sizeof(struct MeshCell));
    if (mesh->cells == NULL || mesh->surfaces == NULL) {
        return FALSE;
    }
    offset = (offset + 7) & ~7u;
    u32 capacity = (MESH_BYTES - offset - 8) / (sizeof(u16) + sizeof(struct SurfaceNode));
    if (capacity < triangles) {
        return FALSE;
    }
    mesh->references = mesh_storage(mesh, &offset, capacity * sizeof(u16));
    mesh->nodes = mesh_storage(mesh, &offset, capacity * sizeof(struct SurfaceNode));
    if (mesh->nodes == NULL) {
        return FALSE;
    }

    memcpy(mesh->localVertices, data + 2, vertices * sizeof(Vec3t));
    memset(mesh->triangles, 0, triangles * sizeof(struct MeshTriangle));
    cursor = groups;
    u32 triangle = 0;
    while (*cursor != TERRAIN_LOAD_CONTINUE) {
        TerrainData type = *cursor++;
        s32 count = *cursor++;
        for (s32 i = 0; i < count; i++, triangle++, cursor += 4) {
            struct MeshTriangle *topology = &mesh->triangles[triangle];
            for (u32 corner = 0; corner < 3; corner++) {
                if (cursor[corner] < 0 || cursor[corner] >= (s32) vertices) {
                    return FALSE;
                }
                topology->vertex[corner] = cursor[corner];
            }
            struct Surface *surface = &mesh->surfaces[triangle];
            surface->type = type;
            surface->force = cursor[3];
            surface->flags = surf_has_no_cam_collision(type) | SURFACE_FLAG_DYNAMIC;
            surface->object = object;
        }
    }
    mesh->object = object;
    mesh->data = data;
    mesh->vertexCount = vertices;
    mesh->triangleCount = triangles;
    mesh->referenceCapacity = capacity;
    mesh->usedBytes = offset;
    mesh->geometryReady = FALSE;
    mesh->membershipReady = FALSE;
    return TRUE;
}

static u8 lower_mesh_cell(s32 coordinate) {
    return MIN(MAX(coordinate + LEVEL_BOUNDARY_MAX, 0) / CELL_SIZE, NUM_CELLS);
}

static u8 upper_mesh_cell(s32 coordinate) {
    return MIN(MAX(coordinate + LEVEL_BOUNDARY_MAX, 0) / CELL_SIZE, NUM_CELLS - 1);
}

static s32 change_membership(struct PersistentSurfaces *mesh, u16 face,
                             const struct MeshTriangle *previous);

static s32 update_surface(struct PersistentSurfaces *mesh, u32 index) {
    struct MeshTriangle *triangle = &mesh->triangles[index];
    struct Surface *surface = &mesh->surfaces[index];
    Vec3t *vertices = mesh->worldVertices;
    Vec3f normal;
    find_vector_perpendicular_to_plane(normal, vertices[triangle->vertex[0]],
                                       vertices[triangle->vertex[1]], vertices[triangle->vertex[2]]);
    f32 magnitude = sqr(normal[0]) + sqr(normal[1]) + sqr(normal[2]);
    u8 valid = TRUE;
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    valid = magnitude >= NEAR_ZERO;
#endif
    s32 changed = !mesh->geometryReady || triangle->valid != valid;
    triangle->valid = valid;
    if (!valid) {
        return changed;
    }
    magnitude = 1.0f / sqrtf(magnitude);
    vec3_scale(normal, magnitude);
    vec3s_copy(surface->vertex1, vertices[triangle->vertex[0]]);
    vec3s_copy(surface->vertex2, vertices[triangle->vertex[1]]);
    vec3s_copy(surface->vertex3, vertices[triangle->vertex[2]]);
    surface->normal.x = normal[0];
    surface->normal.y = normal[1];
    surface->normal.z = normal[2];
    surface->originOffset = -vec3_dot(normal, surface->vertex1);
    s16 minY, maxY;
    min_max_3s(surface->vertex1[1], surface->vertex2[1], surface->vertex3[1], &minY, &maxY);
    surface->lowerY = minY - SURFACE_VERTICAL_BUFFER;
    surface->upperY = maxY + SURFACE_VERTICAL_BUFFER;

    s32 minX, maxX, minZ, maxZ;
    min_max_3i(surface->vertex1[0], surface->vertex2[0], surface->vertex3[0], &minX, &maxX);
    min_max_3i(surface->vertex1[2], surface->vertex2[2], surface->vertex3[2], &minZ, &maxZ);
    u8 lowerX = lower_mesh_cell(minX), upperX = upper_mesh_cell(maxX);
    u8 lowerZ = lower_mesh_cell(minZ), upperZ = upper_mesh_cell(maxZ);
    u8 partition;
    if (SURFACE_IS_NEW_WATER(surface->type)) {
        partition = SPATIAL_PARTITION_WATER;
    } else if (normal[1] > NORMAL_FLOOR_THRESHOLD) {
        partition = SPATIAL_PARTITION_FLOORS;
    } else if (normal[1] < NORMAL_CEIL_THRESHOLD) {
        partition = SPATIAL_PARTITION_CEILS;
    } else {
        partition = SPATIAL_PARTITION_WALLS;
    }
    changed |= triangle->minX != lowerX || triangle->maxX != upperX || triangle->minZ != lowerZ
               || triangle->maxZ != upperZ || triangle->partition != partition;
    triangle->minX = lowerX;
    triangle->maxX = upperX;
    triangle->minZ = lowerZ;
    triangle->maxZ = upperZ;
    triangle->partition = partition;
    return changed;
}

static void update_geometry(struct PersistentSurfaces *mesh, Mat4 transform) {
    if (mesh->geometryReady && memcmp(mesh->transform, transform, sizeof(Mat4)) == 0) {
        return;
    }
    for (u32 i = 0; i < mesh->vertexCount; i++) {
        Vec3f local;
        vec3s_to_vec3f(local, mesh->localVertices[i]);
        // The ordinary loader truncates the linear result before adding translation.
        linear_mtxf_mul_vec3_and_translate(transform, mesh->worldVertices[i], local);
    }
    for (u32 i = 0; i < mesh->triangleCount; i++) {
        struct MeshTriangle *triangle = &mesh->triangles[i];
        struct MeshTriangle previous = *triangle;
        if (update_surface(mesh, i)) {
            if (mesh->membershipReady && !change_membership(mesh, i, &previous)) {
                // Capacity/cell changes are handled by one bounded repack below.
                mesh->membershipReady = FALSE;
            }
        }
    }
    memcpy(mesh->transform, transform, sizeof(Mat4));
    mesh->geometryReady = TRUE;
}

static u32 find_mesh_cell(struct PersistentSurfaces *mesh, u16 key) {
    u32 bucket = (key * 40503u) & (CELL_HASH_SIZE - 1);
    while (mesh->cellHash[bucket] >= 0) {
        u32 cell = mesh->cellHash[bucket];
        if (mesh->cells[cell].key == key) {
            return cell;
        }
        bucket = (bucket + 1) & (CELL_HASH_SIZE - 1);
    }
    if (mesh->cellCount == MAX_MESH_CELLS) {
        return MAX_MESH_CELLS;
    }
    u32 cell = mesh->cellCount++;
    mesh->cellHash[bucket] = cell;
    mesh->cells[cell].key = key;
    mesh->cells[cell].first = 0;
    mesh->cells[cell].count = 0;
    mesh->cells[cell].capacity = 0;
    return cell;
}

static s32 change_membership(struct PersistentSurfaces *mesh, u16 face,
                             const struct MeshTriangle *previous) {
    const struct MeshTriangle *current = &mesh->triangles[face];
    const struct MeshTriangle *ranges[2] = { previous, current };
    for (u32 pass = 0; pass < 2; pass++) {
        const struct MeshTriangle *range = ranges[pass];
        if (!range->valid) {
            continue;
        }
        for (u32 z = range->minZ; z <= range->maxZ; z++) {
            for (u32 x = range->minX; x <= range->maxX; x++) {
                // References in the unchanged overlap need no removal or insertion.
                const struct MeshTriangle *other = ranges[1 - pass];
                if (other->valid && other->partition == range->partition && x >= other->minX
                    && x <= other->maxX && z >= other->minZ && z <= other->maxZ) {
                    continue;
                }
                u16 key = (z * NUM_CELLS + x) * NUM_SPATIAL_PARTITIONS + range->partition;
                u32 index = find_mesh_cell(mesh, key);
                if (index == MAX_MESH_CELLS) {
                    return FALSE;
                }
                struct MeshCell *cell = &mesh->cells[index];
                u16 *references = mesh->references + cell->first;
                if (pass == 0) {
                    u32 position = 0;
                    while (position < cell->count && references[position] != face) {
                        position++;
                    }
                    if (position == cell->count) {
                        return FALSE;
                    }
                    cell->count--;
                    mesh->referenceCount--;
                    for (u32 j = position; j < cell->count; j++) {
                        references[j] = references[j + 1];
                    }
                } else {
                    if (cell->count == cell->capacity) {
                        u32 capacity = cell->count + 4;
                        if (capacity + mesh->referenceUsed > mesh->referenceCapacity) {
                            return FALSE;
                        }
                        u16 *destination = mesh->references + mesh->referenceUsed;
                        memcpy(destination, references, cell->count * sizeof(u16));
                        cell->first = mesh->referenceUsed;
                        cell->capacity = capacity;
                        mesh->referenceUsed += capacity;
                        references = destination;
                    }
                    // Wall lists must retain ordinal order even when membership changes.
                    u32 position = cell->count;
                    if (range->partition == SPATIAL_PARTITION_WALLS) {
                        while (position > 0 && references[position - 1] > face) {
                            references[position] = references[position - 1];
                            position--;
                        }
                    }
                    references[position] = face;
                    cell->count++;
                    mesh->referenceCount++;
                }
            }
        }
    }
    return TRUE;
}

static s32 rebuild_membership(struct PersistentSurfaces *mesh) {
    u16 cursor[MAX_MESH_CELLS];
    memset(mesh->cellHash, -1, sizeof(mesh->cellHash));
    mesh->cellCount = 0;
    mesh->referenceCount = 0;
    for (u32 pass = 0; pass < 2; pass++) {
        for (u32 i = 0; i < mesh->triangleCount; i++) {
            struct MeshTriangle *triangle = &mesh->triangles[i];
            if (!triangle->valid) {
                continue;
            }
            for (u32 z = triangle->minZ; z <= triangle->maxZ; z++) {
                for (u32 x = triangle->minX; x <= triangle->maxX; x++) {
                    u16 key = (z * NUM_CELLS + x) * NUM_SPATIAL_PARTITIONS + triangle->partition;
                    u32 cell = find_mesh_cell(mesh, key);
                    if (cell == MAX_MESH_CELLS) {
                        return FALSE;
                    }
                    if (pass == 0) {
                        if (mesh->referenceCount == mesh->referenceCapacity) {
                            return FALSE;
                        }
                        mesh->referenceCount++;
                        mesh->cells[cell].count++;
                    } else {
                        mesh->references[cursor[cell]++] = i;
                    }
                }
            }
        }
        if (pass == 0) {
            u32 spare = mesh->referenceCount + mesh->cellCount * 2 <= mesh->referenceCapacity ? 2 : 0;
            u32 first = 0;
            for (u32 i = 0; i < mesh->cellCount; i++) {
                mesh->cells[i].first = first;
                cursor[i] = first;
                mesh->cells[i].capacity = mesh->cells[i].count + spare;
                first += mesh->cells[i].capacity;
            }
            mesh->referenceUsed = first;
        }
    }
    mesh->membershipReady = TRUE;
    return TRUE;
}

static void sort_mesh_cell(struct PersistentSurfaces *mesh, struct MeshCell *cell) {
    u32 partition = cell->key % NUM_SPATIAL_PARTITIONS;
    if (partition == SPATIAL_PARTITION_WALLS) {
        return;
    }
    s32 direction = partition == SPATIAL_PARTITION_CEILS ? -1 : 1;
    u16 *references = mesh->references + cell->first;
    // Warm lists are almost sorted. Restore ordinal order when priorities become equal.
    for (u32 i = 1; i < cell->count; i++) {
        u16 face = references[i];
        s32 priority = mesh->surfaces[face].upperY * direction;
        u32 destination = i;
        while (destination > 0) {
            u16 previous = references[destination - 1];
            s32 previousPriority = mesh->surfaces[previous].upperY * direction;
            if (previousPriority > priority || (previousPriority == priority && previous < face)) {
                break;
            }
            references[destination--] = previous;
        }
        references[destination] = face;
    }
}

static s32 publish_mesh(struct Object *object, const TerrainData *data, Mat4 transform, RoomData room,
                        s32 smallMesh) {
    struct PersistentSurfaces *mesh = NULL;
    for (u32 i = 0; i < MESH_SLOTS; i++) {
        if (sMeshes[i].object == object) {
            mesh = &sMeshes[i];
            break;
        }
    }
    if (mesh != NULL && mesh->publishedFrame == sFrame) {
        // Duplicate publications use independent native storage, preserving the first pose.
        return FALSE;
    }
    if (mesh == NULL) {
        for (u32 i = 0; i < MESH_SLOTS; i++) {
            if (sMeshes[i].object == NULL && sMeshes[i].publishedFrame != sFrame) {
                mesh = &sMeshes[i];
                break;
            }
        }
    }
    if (mesh == NULL) {
        return FALSE;
    }
    if (mesh->object != object || mesh->data != data) {
        mesh->object = NULL;
        if (!decode_mesh(mesh, object, data, smallMesh)) {
            return FALSE;
        }
    }
    update_geometry(mesh, transform);
    if (!mesh->membershipReady && !rebuild_membership(mesh)) {
        return FALSE;
    }
    u32 surfaces = 0;
    for (u32 i = 0; i < mesh->triangleCount; i++) {
        mesh->surfaces[i].room = room;
        surfaces += mesh->triangles[i].valid;
    }
    mesh->publishedFrame = sFrame;
    for (u32 i = 0; i < mesh->cellCount; i++) {
        struct MeshCell *cell = &mesh->cells[i];
        if (cell->count == 0) {
            continue;
        }
        sort_mesh_cell(mesh, cell);
        for (u32 j = cell->first; j < cell->first + cell->count; j++) {
            mesh->nodes[j].surface = &mesh->surfaces[mesh->references[j]];
            mesh->nodes[j].next = j + 1 < cell->first + cell->count ? &mesh->nodes[j + 1] : NULL;
        }
        u32 position = cell->key / NUM_SPATIAL_PARTITIONS;
        publish_dynamic_surface_cell(position % NUM_CELLS, position / NUM_CELLS,
                                     cell->key % NUM_SPATIAL_PARTITIONS, &mesh->nodes[cell->first]);
    }
    record_persistent_surface_counts(surfaces, mesh->referenceCount);
    return TRUE;
}

s32 persistent_surfaces_publish(struct Object *object, const TerrainData *data, Mat4 transform,
                                RoomData room) {
    return publish_mesh(object, data, transform, room, FALSE);
}

#if JRB_STATIONARY_SURFACES
s32 persistent_surfaces_publish_jrb_ship(struct Object *object, const TerrainData *data, Mat4 transform,
                                         Vec3f scale) {
    Mat4 scaled;
    mtxf_scale_vec3f(scaled, transform, scale);
    return publish_mesh(object, data, scaled, 0, TRUE);
}
#endif

#if BOWSER_STATIONARY_SURFACES
s32 persistent_surfaces_publish_bowser_platform(struct Object *object, const TerrainData *data,
                                                Mat4 transform, Vec3f scale) {
    Mat4 scaled;
    mtxf_scale_vec3f(scaled, transform, scale);
    return publish_mesh(object, data, scaled, 0, TRUE);
}
#endif

#if DORRIE_STATIONARY_SURFACES
s32 persistent_surfaces_publish_dorrie(struct Object *object, const TerrainData *data, Mat4 transform,
                                       Vec3f scale) {
    // Forward speed alone misses bobbing, scale changes and head-mesh switches.
    if (sDorriePose.object == object && sDorriePose.data == data
        && memcmp(sDorriePose.transform, transform, sizeof(Mat4)) == 0
        && memcmp(sDorriePose.scale, scale, sizeof(Vec3f)) == 0) {
        Mat4 scaled;
        mtxf_scale_vec3f(scaled, transform, scale);
        return persistent_surfaces_publish(object, data, scaled, 0);
    }

    sDorriePose.object = object;
    sDorriePose.data = data;
    memcpy(sDorriePose.transform, transform, sizeof(Mat4));
    memcpy(sDorriePose.scale, scale, sizeof(Vec3f));
    return FALSE;
}
#endif
#endif
