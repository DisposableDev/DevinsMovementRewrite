#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "collision_mesh.h"

struct Asset {
    const char *name;
    const Collision *data;
    u32 words;
};
#include "mesh_assets.h"

static Mat4 identity = { { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };
static u32 randomState = 1234567;

static f32 random_unit(void) {
    randomState = randomState * 1664525u + 1013904223u;
    return (randomState >> 8) / 16777216.0f;
}

static void world_vertex(const struct CollisionMeshInstance *instance, const s16 *vertex,
                         double world[3]) {
    for (s32 axis = 0; axis < 3; axis++) {
        world[axis] = instance->transform[3][axis];
        for (s32 local = 0; local < 3; local++) {
            world[axis] += (double) instance->transform[local][axis] * vertex[local];
        }
    }
}

static void cross(const double a[3], const double b[3], double result[3]) {
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
}

static double dot(const double a[3], const double b[3]) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

// Independent double-precision, world-space Moller-Trumbore oracle. It shares
// neither BVH traversal, local planes, nor edge tests with the prototype.
static s32 oracle_ray(const struct CollisionMeshInstance *instance, const Vec3f start, const Vec3f end,
                      u32 flags, double *nearest) {
    double direction[3];
    for (s32 axis = 0; axis < 3; axis++) {
        direction[axis] = (double) end[axis] - start[axis];
    }
    *nearest = 1.0;
    s32 found = 0;
    for (s32 i = 0; i < instance->mesh->triangleCount; i++) {
        const struct CollisionMeshTriangle *face = &instance->mesh->triangles[i];
        if (!face->valid || SURFACE_IS_NEW_WATER(face->type) || face->type == SURFACE_INTANGIBLE
            || face->type == SURFACE_CAMERA_BOUNDARY) {
            continue;
        }
        double a[3], b[3], c[3], ab[3], ac[3], from[3], p[3], q[3], normal[3];
        world_vertex(instance, instance->mesh->vertices[face->vertices[0]], a);
        world_vertex(instance, instance->mesh->vertices[face->vertices[1]], b);
        world_vertex(instance, instance->mesh->vertices[face->vertices[2]], c);
        for (s32 axis = 0; axis < 3; axis++) {
            ab[axis] = b[axis] - a[axis];
            ac[axis] = c[axis] - a[axis];
            from[axis] = start[axis] - a[axis];
        }
        cross(ab, ac, normal);
        if ((flags & COLLISION_MESH_FLOORS)
            && (normal[1] <= 0 || normal[1] * normal[1] <= 0.0001 * dot(normal, normal))) {
            continue;
        }
        cross(direction, ac, p);
        double determinant = dot(ab, p);
        if (fabs(determinant) < 1e-12 || (!(flags & COLLISION_MESH_TWO_SIDED) && determinant < 0)) {
            continue;
        }
        double u = dot(from, p) / determinant;
        cross(from, ab, q);
        double v = dot(direction, q) / determinant;
        double fraction = dot(ac, q) / determinant;
        if (u >= 0 && v >= 0 && u + v <= 1 && fraction >= 0 && fraction <= *nearest) {
            *nearest = fraction;
            found = 1;
        }
    }
    return found;
}

static void compare_ray(const struct CollisionMeshInstance *instance, const Vec3f start,
                        const Vec3f end, u32 flags) {
    double fraction;
    s32 expected = oracle_ray(instance, start, end, flags, &fraction);
    struct CollisionMeshHit hit;
    s32 actual = collision_mesh_raycast(instance, start, end, flags, &hit);
    if (actual != expected || (actual && fabs(hit.fraction - fraction) > 0.00002)) {
        fprintf(stderr, "ray mismatch %d/%d t=%g/%g start=%g,%g,%g end=%g,%g,%g\n", actual, expected,
                actual ? hit.fraction : -1, fraction, start[0], start[1], start[2], end[0], end[1],
                end[2]);
        abort();
    }
}

static void test_boundaries(void) {
    Collision data[] = { COL_INIT(),
                         COL_VERTEX_INIT(7),
                         COL_VERTEX(-100, 0, -100),
                         COL_VERTEX(-100, 0, 100),
                         COL_VERTEX(100, 0, -100),
                         COL_VERTEX(-100, 10, -100),
                         COL_VERTEX(-100, 10, 100),
                         COL_VERTEX(100, 10, -100),
                         COL_VERTEX(0, 0, 0),
                         COL_TRI_INIT(SURFACE_DEFAULT, 3),
                         COL_TRI_SPECIAL(0, 1, 2, 17),
                         COL_TRI_SPECIAL(3, 4, 5, 23),
                         COL_TRI(6, 6, 6),
                         COL_TRI_STOP() };
    u32 words = sizeof(data) / sizeof(*data);
    u32 bytes = collision_mesh_storage_size(data, words, 4);
    assert(bytes != 0);
    u8 *storage = malloc(bytes + 16);
    memset(storage, 0xa5, bytes + 16);
    struct CollisionMesh mesh;
    assert(!collision_mesh_build(&mesh, data, words, 4, storage, bytes - 1));
    assert(mesh.nodeCount == 0);
    for (u32 i = 0; i < bytes + 16; i++) {
        assert(storage[i] == 0xa5);
    }
    for (u32 truncated = 0; truncated < words; truncated++) {
        assert(collision_mesh_storage_size(data, truncated, 4) == 0);
    }
    assert(!collision_mesh_build(&mesh, data, words, 4, storage + 1, bytes));
    assert(collision_mesh_build(&mesh, data, words, 4, storage, bytes));
    memset(data, 0, sizeof(data));
    struct CollisionMeshInstance instance;
    assert(collision_mesh_instance_update(&instance, &mesh, identity));
    struct CollisionMeshHit hit;
    Vec3f start = { -25.25f, 100, -25.25f }, end = { -25.25f, -100, -25.25f };
    assert(collision_mesh_raycast(&instance, start, end, COLLISION_MESH_FLOORS, &hit));
    assert(hit.triangle == 1 && hit.force == 23 && fabsf(hit.position[1] - 10) < 0.001f);
    // Fractional position outside the finite edge must not borrow the plane.
    start[0] = end[0] = 0.25f;
    start[2] = end[2] = 0.25f;
    assert(!collision_mesh_raycast(&instance, start, end, COLLISION_MESH_FLOORS, &hit));
    start[0] = end[0] = 0;
    start[2] = end[2] = 0;
    assert(collision_mesh_raycast(&instance, start, end, COLLISION_MESH_FLOORS, &hit));
    Mat4 moved;
    memcpy(moved, identity, sizeof(moved));
    moved[0][0] = 2;
    moved[1][1] = 3;
    moved[2][2] = 0.5f;
    moved[3][0] = 200.125f;
    moved[3][1] = 30.25f;
    assert(collision_mesh_instance_update(&instance, &mesh, moved));
    start[0] = end[0] = 150;
    start[2] = end[2] = -20;
    assert(collision_mesh_raycast(&instance, start, end, COLLISION_MESH_FLOORS, &hit));
    assert(fabsf(hit.position[1] - 60.25f) < 0.001f && hit.normal[1] > 0.999f);
    moved[0][0] = -2;
    assert(collision_mesh_instance_update(&instance, &mesh, moved));
    start[0] = end[0] = 250;
    assert(!collision_mesh_raycast(&instance, start, end, COLLISION_MESH_FLOORS, &hit));
    assert(collision_mesh_raycast(&instance, start, end, COLLISION_MESH_TWO_SIDED, &hit));
    assert(hit.normal[1] < -0.999f);
    moved[0][0] = 0;
    assert(!collision_mesh_instance_update(&instance, &mesh, moved));
    assert(!collision_mesh_raycast(&instance, start, end, 0, &hit));
    moved[0][0] = INFINITY;
    assert(!collision_mesh_instance_update(&instance, &mesh, moved));
    // Mesh geometry is unchanged after updates and caller input may be released.
    assert(mesh.vertices[0][0] == -100 && mesh.triangles[1].force == 23);
    for (u32 i = bytes; i < bytes + 16; i++) {
        assert(storage[i] == 0xa5);
    }
    free(storage);
}

static void test_assets(void) {
    for (u32 asset = 0; asset < sizeof(assets) / sizeof(*assets); asset++) {
        u32 bytes = collision_mesh_storage_size(assets[asset].data, assets[asset].words, 4);
        assert(bytes != 0);
        void *storage = malloc(bytes);
        struct CollisionMesh mesh;
        assert(collision_mesh_build(&mesh, assets[asset].data, assets[asset].words, 4, storage, bytes));
        assert(mesh.nodeCount <= mesh.nodeCapacity);
        struct CollisionMeshInstance instance;
        for (s32 pose = 0; pose < 3; pose++) {
            Mat4 transform;
            memcpy(transform, identity, sizeof(transform));
            if (pose != 0) {
                f32 angle = pose * 0.31f;
                transform[0][0] = cosf(angle) * 1.3f;
                transform[0][1] = sinf(angle) * 1.3f;
                transform[1][0] = -sinf(angle) * 0.7f;
                transform[1][1] = cosf(angle) * 0.7f;
                transform[2][2] = pose == 2 ? -1.1f : 1.1f;
                transform[3][0] = 37.125f;
                transform[3][1] = -83.625f;
            }
            assert(collision_mesh_instance_update(&instance, &mesh, transform));
            for (s32 probe = 0; probe < 350; probe++) {
                Vec3f start, end;
                for (s32 axis = 0; axis < 3; axis++) {
                    f32 span = instance.maximum[axis] - instance.minimum[axis];
                    start[axis] = instance.minimum[axis] - 100 + random_unit() * (span + 200);
                    end[axis] = instance.minimum[axis] - 100 + random_unit() * (span + 200);
                }
                compare_ray(&instance, start, end, 0);
                start[1] = instance.maximum[1] + 100;
                end[0] = start[0];
                end[1] = instance.minimum[1] - 100;
                end[2] = start[2];
                compare_ray(&instance, start, end, COLLISION_MESH_FLOORS);
            }
        }
        free(storage);
    }
}

static void test_filters(void) {
    Collision data[] = { COL_INIT(),
                         COL_VERTEX_INIT(3),
                         COL_VERTEX(-100, 0, -100),
                         COL_VERTEX(-100, 0, 100),
                         COL_VERTEX(100, 0, -100),
                         COL_TRI_INIT(SURFACE_DEFAULT, 1),
                         COL_TRI(0, 1, 2),
                         COL_TRI_STOP() };
    u32 words = sizeof(data) / sizeof(*data);
    u32 bytes = collision_mesh_storage_size(data, words, 4);
    void *storage = malloc(bytes);
    struct CollisionMesh mesh;
    struct CollisionMeshInstance instance;
    struct CollisionMeshHit hit;
    Vec3f start = { -25.25f, 100, -25.25f }, end = { -25.25f, -100, -25.25f };
    const s16 kinds[] = { SURFACE_DEFAULT,
                          SURFACE_INTANGIBLE,
                          SURFACE_CAMERA_BOUNDARY,
                          SURFACE_NO_CAM_COLLISION,
                          SURFACE_NO_CAM_COLLISION_77,
                          SURFACE_NO_CAM_COL_VERY_SLIPPERY,
                          SURFACE_SWITCH,
                          SURFACE_NEW_WATER };
    for (u32 i = 0; i < sizeof(kinds) / sizeof(*kinds); i++) {
        data[11] = kinds[i];
        assert(collision_mesh_build(&mesh, data, words, 4, storage, bytes));
        assert(collision_mesh_instance_update(&instance, &mesh, identity));
        s32 physical = collision_mesh_raycast(&instance, start, end, COLLISION_MESH_FLOORS, &hit);
        s32 camera = collision_mesh_raycast(&instance, start, end,
                                            COLLISION_MESH_FLOORS | COLLISION_MESH_CAMERA, &hit);
        assert(physical == (i == 0 || (i >= 3 && i <= 6)));
        assert(camera == (i == 0 || i == 2));
        if (i == 1) {
            assert(collision_mesh_raycast(&instance, start, end,
                                          COLLISION_MESH_FLOORS | COLLISION_MESH_INCLUDE_INTANGIBLE,
                                          &hit));
        }
    }
    data[11] = SURFACE_DEFAULT;
    data[16] = COL_TRI_STOP();
    assert(collision_mesh_build(&mesh, data, words - 1, 3, storage, bytes));
    assert(mesh.triangles[0].force == 0);
    data[13] = 99;
    assert(collision_mesh_storage_size(data, words - 1, 3) == 0);
    free(storage);
}

// Isolated legacy representation: world-vertex quantization, plane rebuild,
// per-cell references and sorted insertion. Uses the project's 32x32/1024 grid.
// This benchmarks the geometry pipeline, not object scheduling or the whole game.
struct GridFace {
    Vec3s vertices[3];
    Vec3f normal;
    f32 offset;
    s32 upper, lower;
    s16 type;
};
struct GridNode {
    s32 face, next;
};
struct Grid {
    Vec3s *vertices;
    struct GridFace *faces;
    struct GridNode *nodes;
    s32 heads[32][32][3];
    u16 occupied[32 * 32 * 3];
    u32 occupiedCount;
    u32 count, capacity;
};

static s32 grid_cell(s32 coordinate) {
    s32 cell = (coordinate + 16384) / 1024;
    if (cell < 0) {
        return 0;
    }
    if (cell > 31) {
        return 31;
    }
    return cell;
}

static void grid_build(struct Grid *grid, const struct CollisionMeshInstance *instance) {
    for (u32 i = 0; i < grid->occupiedCount; i++) {
        u32 cell = grid->occupied[i];
        grid->heads[cell / 96][(cell / 3) % 32][cell % 3] = -1;
    }
    grid->occupiedCount = 0;
    grid->count = 0;
    // The real loader transforms each shared vertex once, not once per face.
    for (s32 i = 0; i < instance->mesh->vertexCount; i++) {
        const s16 *source = instance->mesh->vertices[i];
        for (s32 axis = 0; axis < 3; axis++) {
            f32 world = instance->transform[3][axis] + instance->transform[0][axis] * source[0]
                        + instance->transform[1][axis] * source[1]
                        + instance->transform[2][axis] * source[2];
            assert(world > -32768 && world < 32768);
            grid->vertices[i][axis] = world;
        }
    }
    for (s32 i = 0; i < instance->mesh->triangleCount; i++) {
        struct GridFace *face = &grid->faces[i];
        const struct CollisionMeshTriangle *local = &instance->mesh->triangles[i];
        s32 minimum[3] = { 32767, 32767, 32767 }, maximum[3] = { -32768, -32768, -32768 };
        for (s32 vertex = 0; vertex < 3; vertex++) {
            for (s32 axis = 0; axis < 3; axis++) {
                s16 value = grid->vertices[local->vertices[vertex]][axis];
                face->vertices[vertex][axis] = value;
                if (value < minimum[axis]) {
                    minimum[axis] = value;
                }
                if (value > maximum[axis]) {
                    maximum[axis] = value;
                }
            }
        }
        Vec3f ab, ac, normal;
        for (s32 axis = 0; axis < 3; axis++) {
            ab[axis] = face->vertices[1][axis] - face->vertices[0][axis];
            ac[axis] = face->vertices[2][axis] - face->vertices[0][axis];
        }
        normal[0] = ab[1] * ac[2] - ab[2] * ac[1];
        normal[1] = ab[2] * ac[0] - ab[0] * ac[2];
        normal[2] = ab[0] * ac[1] - ab[1] * ac[0];
        f32 length = sqrtf(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
        if (length == 0) {
            continue;
        }
        face->offset = 0;
        for (s32 axis = 0; axis < 3; axis++) {
            face->normal[axis] = normal[axis] / length;
            face->offset -= face->normal[axis] * face->vertices[0][axis];
        }
        face->type = local->type;
        face->upper = maximum[1] + 5;
        face->lower = minimum[1] - 5;
        s32 partition;
        if (face->normal[1] > 0.01f) {
            partition = 0;
        } else if (face->normal[1] < -0.01f) {
            partition = 1;
        } else {
            partition = 2;
        }
        for (s32 z = grid_cell(minimum[2]); z <= grid_cell(maximum[2]); z++) {
            for (s32 x = grid_cell(minimum[0]); x <= grid_cell(maximum[0]); x++) {
                assert(grid->count < grid->capacity);
                s32 *link = &grid->heads[z][x][partition];
                if (*link < 0) {
                    grid->occupied[grid->occupiedCount++] = (z * 32 + x) * 3 + partition;
                }
                while (*link >= 0) {
                    const struct GridFace *old = &grid->faces[grid->nodes[*link].face];
                    if ((partition == 0 && face->upper > old->upper)
                        || (partition == 1 && face->lower < old->lower) || partition == 2) {
                        break;
                    }
                    link = &grid->nodes[*link].next;
                }
                grid->nodes[grid->count] = (struct GridNode){ i, *link };
                *link = grid->count++;
            }
        }
    }
}

static f32 grid_floor(const struct Grid *grid, const Vec3f position) {
    f32 highest = -11000;
    s32 node = grid->heads[grid_cell(position[2])][grid_cell(position[0])][0];
    while (node >= 0) {
        const struct GridFace *face = &grid->faces[grid->nodes[node].face];
        node = grid->nodes[node].next;
        if (face->type == SURFACE_INTANGIBLE || face->type == SURFACE_CAMERA_BOUNDARY
            || SURFACE_IS_NEW_WATER(face->type)) {
            continue;
        }
        s32 inside = 1;
        for (s32 edge = 0; edge < 3; edge++) {
            const s16 *a = face->vertices[edge], *b = face->vertices[(edge + 1) % 3];
            if ((a[2] - position[2]) * (b[0] - a[0]) - (a[0] - position[0]) * (b[2] - a[2]) < 0) {
                inside = 0;
                break;
            }
        }
        if (inside) {
            f32 height = -(position[0] * face->normal[0] + position[2] * face->normal[2] + face->offset)
                         / face->normal[1];
            if (height <= position[1] + 78 && height > highest) {
                highest = height;
            }
        }
    }
    return highest;
}

static f32 tree_floor(const struct CollisionMeshInstance *instance, const Vec3f position) {
    Vec3f start = { position[0], position[1] + 78, position[2] };
    Vec3f end = { position[0], -11000, position[2] };
    struct CollisionMeshHit hit;
    return collision_mesh_raycast(instance, start, end, COLLISION_MESH_FLOORS, &hit) ? hit.position[1]
                                                                                     : -11000;
}

static f32 grid_face_ray(const struct GridFace *face, const Vec3f start, const Vec3f direction) {
    Vec3f ab, ac, from, p, q;
    for (s32 axis = 0; axis < 3; axis++) {
        ab[axis] = face->vertices[1][axis] - face->vertices[0][axis];
        ac[axis] = face->vertices[2][axis] - face->vertices[0][axis];
        from[axis] = start[axis] - face->vertices[0][axis];
    }
    p[0] = direction[1] * ac[2] - direction[2] * ac[1];
    p[1] = direction[2] * ac[0] - direction[0] * ac[2];
    p[2] = direction[0] * ac[1] - direction[1] * ac[0];
    f32 determinant = ab[0] * p[0] + ab[1] * p[1] + ab[2] * p[2];
    if (determinant <= 0.00001f) {
        return 2;
    }
    f32 u = (from[0] * p[0] + from[1] * p[1] + from[2] * p[2]) / determinant;
    if (u < 0 || u > 1) {
        return 2;
    }
    q[0] = from[1] * ab[2] - from[2] * ab[1];
    q[1] = from[2] * ab[0] - from[0] * ab[2];
    q[2] = from[0] * ab[1] - from[1] * ab[0];
    f32 v = (direction[0] * q[0] + direction[1] * q[1] + direction[2] * q[2]) / determinant;
    if (v < 0 || u + v > 1) {
        return 2;
    }
    f32 fraction = (ac[0] * q[0] + ac[1] * q[1] + ac[2] * q[2]) / determinant;
    return fraction > 0.00001f && fraction <= 1 ? fraction : 2;
}

static f32 grid_ray(const struct Grid *grid, const Vec3f start, const Vec3f end) {
    Vec3f direction;
    for (s32 axis = 0; axis < 3; axis++) {
        direction[axis] = end[axis] - start[axis];
    }
    s32 x = (s32) floorf((start[0] + 16384) / 1024);
    s32 z = (s32) floorf((start[2] + 16384) / 1024);
    s32 stepX = direction[0] < 0 ? -1 : 1, stepZ = direction[2] < 0 ? -1 : 1;
    f32 deltaX = direction[0] == 0 ? 1e30f : fabsf(1024 / direction[0]);
    f32 deltaZ = direction[2] == 0 ? 1e30f : fabsf(1024 / direction[2]);
    f32 nextX =
        direction[0] == 0 ? 1e30f : ((x + (stepX > 0)) * 1024 - 16384 - start[0]) / direction[0];
    f32 nextZ =
        direction[2] == 0 ? 1e30f : ((z + (stepZ > 0)) * 1024 - 16384 - start[2]) / direction[2];
    f32 nearest = 1;
    while (1) {
        if (x >= 0 && x < 32 && z >= 0 && z < 32) {
            for (s32 partition = 0; partition < 3; partition++) {
                for (s32 node = grid->heads[z][x][partition]; node >= 0;
                     node = grid->nodes[node].next) {
                    const struct GridFace *face = &grid->faces[grid->nodes[node].face];
                    if (face->type == SURFACE_INTANGIBLE || face->type == SURFACE_CAMERA_BOUNDARY
                        || SURFACE_IS_NEW_WATER(face->type)) {
                        continue;
                    }
                    s32 outside = 0;
                    for (s32 axis = 0; axis < 3; axis++) {
                        s32 low = face->vertices[0][axis], high = low;
                        for (s32 v = 1; v < 3; v++) {
                            if (face->vertices[v][axis] < low) {
                                low = face->vertices[v][axis];
                            }
                            if (face->vertices[v][axis] > high) {
                                high = face->vertices[v][axis];
                            }
                        }
                        f32 minimum = fminf(start[axis], end[axis]) - 1;
                        f32 maximum = fmaxf(start[axis], end[axis]) + 1;
                        if (low > maximum || high < minimum) {
                            outside = 1;
                            break;
                        }
                    }
                    if (!outside) {
                        f32 fraction = grid_face_ray(face, start, direction);
                        if (fraction < nearest) {
                            nearest = fraction;
                        }
                    }
                }
            }
        }
        if (nextX > 1 && nextZ > 1) {
            break;
        }
        if (nextX < nextZ) {
            x += stepX;
            nextX += deltaX;
        } else {
            z += stepZ;
            nextZ += deltaZ;
        }
    }
    return nearest;
}

static double milliseconds(clock_t start) {
    return 1000.0 * (clock() - start) / CLOCKS_PER_SEC;
}

static volatile f32 checksum;

static void benchmark(void) {
    puts("Host CPU timings only; grid reference is not target/vanilla performance proof.");
    for (u32 asset = 0; asset < sizeof(assets) / sizeof(*assets); asset++) {
        u32 bytes = collision_mesh_storage_size(assets[asset].data, assets[asset].words, 4);
        void *storage = malloc(bytes);
        struct CollisionMesh mesh;
        clock_t begin = clock();
        assert(collision_mesh_build(&mesh, assets[asset].data, assets[asset].words, 4, storage, bytes));
        double build = milliseconds(begin);
        struct CollisionMeshInstance instance;
        assert(collision_mesh_instance_update(&instance, &mesh, identity));
        struct Grid grid;
        memset(grid.heads, -1, sizeof(grid.heads));
        grid.occupiedCount = 0;
        grid.vertices = malloc(mesh.vertexCount * sizeof(*grid.vertices));
        grid.faces = calloc(mesh.triangleCount, sizeof(*grid.faces));
        grid.capacity = mesh.triangleCount * 1024u;
        grid.nodes = malloc(grid.capacity * sizeof(*grid.nodes));
        begin = clock();
        grid_build(&grid, &instance);
        double gridBuild = milliseconds(begin);
        Vec3f probes[2048];
        for (u32 i = 0; i < 2048; i++) {
            const struct CollisionMeshTriangle *face = &mesh.triangles[i % mesh.triangleCount];
            for (s32 axis = 0; axis < 3; axis++) {
                probes[i][axis] =
                    (mesh.vertices[face->vertices[0]][axis] + mesh.vertices[face->vertices[1]][axis]
                     + mesh.vertices[face->vertices[2]][axis])
                        / 3.0f
                    + 0.125f;
            }
            probes[i][1] += 160;
        }
        begin = clock();
        for (u32 i = 0; i < 2048; i++) {
            checksum += grid_floor(&grid, probes[i]);
        }
        double gridQueries = milliseconds(begin);
        begin = clock();
        for (u32 i = 0; i < 2048; i++) {
            checksum += tree_floor(&instance, probes[i]);
        }
        double treeQueries = milliseconds(begin);
        Vec3f rayStart[2048], rayEnd[2048];
        f32 fractions[2][2048];
        for (u32 i = 0; i < 2048; i++) {
            const struct CollisionMeshTriangle *face = &mesh.triangles[i % mesh.triangleCount];
            for (s32 axis = 0; axis < 3; axis++) {
                rayStart[i][axis] = probes[i][axis] - (axis == 1 ? 160 : 0) + face->normal[axis] * 80;
                rayEnd[i][axis] = probes[i][axis] - (axis == 1 ? 160 : 0) - face->normal[axis] * 80;
            }
        }
        begin = clock();
        for (u32 i = 0; i < 2048; i++) {
            fractions[0][i] = grid_ray(&grid, rayStart[i], rayEnd[i]);
            checksum += fractions[0][i];
        }
        double gridRays = milliseconds(begin);
        begin = clock();
        for (u32 i = 0; i < 2048; i++) {
            struct CollisionMeshHit hit;
            fractions[1][i] =
                collision_mesh_raycast(&instance, rayStart[i], rayEnd[i], 0, &hit) ? hit.fraction : 1;
            checksum += fractions[1][i];
        }
        double treeRays = milliseconds(begin);
        for (u32 i = 0; i < 2048; i++) {
            assert(fabsf(fractions[0][i] - fractions[1][i]) < 0.00005f);
        }
        // A moving mesh repeats transform + all planes + all insertions; the
        // prototype repeats inverse/bounds + queries. Include both steady costs.
        double pipeline[2];
        for (s32 backend = 0; backend < 2; backend++) {
            begin = clock();
            for (s32 frame = 0; frame < 80; frame++) {
                Mat4 transform;
                memcpy(transform, identity, sizeof(transform));
                transform[3][0] = frame * 0.125f;
                transform[3][1] = frame * 0.25f;
                if (backend == 0) {
                    memcpy(instance.transform, transform, sizeof(transform));
                    grid_build(&grid, &instance);
                } else {
                    assert(collision_mesh_instance_update(&instance, &mesh, transform));
                }
                for (s32 i = 0; i < 16; i++) {
                    checksum += backend == 0 ? grid_floor(&grid, probes[(frame * 16 + i) % 2048])
                                             : tree_floor(&instance, probes[(frame * 16 + i) % 2048]);
                }
            }
            pipeline[backend] = milliseconds(begin);
        }
        printf("%s: %u faces, mesh %u B; cold grid/tree %.3f/%.3f ms; "
               "2048 floors %.3f/%.3f ms; rays %.3f/%.3f ms; "
               "80 moving updates+1280 floors %.3f/%.3f ms\n",
               assets[asset].name, mesh.triangleCount, bytes, gridBuild, build, gridQueries,
               treeQueries, gridRays, treeRays, pipeline[0], pipeline[1]);
        free(grid.vertices);
        free(grid.faces);
        free(grid.nodes);
        free(storage);
    }
}

int main(int argc, char **argv) {
    (void) argv;
    test_boundaries();
    test_filters();
    test_assets();
    puts("accuracy and lifecycle passed: 12 shipped meshes, 25200 independent ray/floor comparisons");
    if (argc > 1) {
        benchmark();
    }
    return 0;
}
