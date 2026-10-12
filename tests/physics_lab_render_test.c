#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef float f32;
typedef int32_t s32;
typedef int16_t s16;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef f32 Mat4[4][4];
typedef f32 Vec3f[3];
typedef s16 Vec3s[3];
typedef struct Gfx {
    int op;
    const struct Gfx *target;
} Gfx;
struct GraphNode {
    int flags;
    struct GraphNode *children;
};
// No animation or transform envelope is available to the distance cutoff.
struct GraphNodeObject {
    struct GraphNode node;
    struct GraphNode *sharedChild;
    Vec3f cameraToObject;
};
struct Object {
    struct {
        struct GraphNodeObject gfx;
    } header;
    u32 cameraDistanceCulled;
};
#define TRUE 1
#define FALSE 0
#define LEVEL_PHYSICS_LAB 32
#define EMU_PROJECT64 1
#define EMU_PARALLEL_LAUNCHER 2
#define EMU_MUPEN 4
#define GEO_CONTEXT_CREATE 0
#define GEO_CONTEXT_RENDER 1
#define LAYER_OPAQUE 1
#define SET_GRAPH_NODE_LAYER(flags, layer) ((flags) = (layer))
#define ARRAY_COUNT(a) (sizeof(a) / sizeof(*(a)))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define absf fabsf
#define segmented_to_virtual(p) ((void *) (p))
#define mtxf_copy(a, b) memcpy((a), (b), sizeof(Mat4))
#define gSPDisplayList(p, t)                                                                           \
    do {                                                                                               \
        Gfx *cmd = (p);                                                                                \
        cmd->op = 1;                                                                                   \
        cmd->target = (t);                                                                             \
    } while (0)
#define gSPEndDisplayList(p)                                                                           \
    do {                                                                                               \
        Gfx *cmd = (p);                                                                                \
        cmd->op = 0;                                                                                   \
        cmd->target = NULL;                                                                            \
    } while (0)
static u16 gAreaUpdateCounter;
static int gCurrLevelNum = LEVEL_PHYSICS_LAB, gEmulator;
static int gCurrAreaIndex = 1;
static struct GraphNode camera, *gCurGraphNodeCamera = &camera, *gCurGraphNodeCamFrustum = &camera;
static struct GraphNode models[128], *gLoadedGraphNodes[128];
static const Gfx physics_lab_terrain_dl[] = { { 0, NULL } }, physics_lab_ground_dl[] = { { 0, NULL } },
                 physics_lab_terrain_begin[] = { { 0, NULL } },
                 physics_lab_terrain_end[] = { { 0, NULL } },
                 physics_lab_sculptures_dl[] = { { 0, NULL } };
static const Gfx physics_lab_ground_wide_dl[] = { { 0, NULL } };
static Gfx commands[64];
static int failAlloc, allocationCalls;
static size_t allocationSize;
static void *alloc_display_list(size_t bytes) {
    allocationCalls++;
    allocationSize = bytes;
    assert(bytes <= sizeof(commands));
    memset(commands, 0xA5, sizeof(commands));
    return failAlloc ? NULL : commands;
}
#include "physics_lab_render_definitions.inc"
#include "physics_lab_render_tables.inc"
#include "physics_lab_render_functions.inc"
static void identity(Mat4 m) {
    memset(m, 0, sizeof(Mat4));
    for (int i = 0; i < 4; i++) {
        m[i][i] = 1;
    }
}
static void prepare(f32 z, f32 vertical) {
    Mat4 m;
    identity(m);
    m[3][2] = -z;
    physics_lab_prepare_view(m, 0.7f, vertical, 4.0f / 3.0f, 0);
}
static void wave_view(void) {
    struct PhysicsLabRenderSection b = { { -100, -100, -1100 }, { 100, 100, -900 }, NULL };
    prepare(0, 0.41421356f);
    assert(physics_lab_activity_in_view(&b));
    b.min[0] = 3000;
    b.max[0] = 3200;
    assert(!physics_lab_activity_in_view(&b));
    sLabView.roll = 1;
    assert(physics_lab_activity_in_view(&b));
    sLabView.roll = 0;
    gAreaUpdateCounter += 2;
    assert(physics_lab_activity_in_view(&b));
    gAreaUpdateCounter = 65535;
    prepare(0, .41421356f);
    gAreaUpdateCounter = 0;
    assert(!physics_lab_activity_in_view(&b));
    gAreaUpdateCounter = 1;
    assert(physics_lab_activity_in_view(&b));
    prepare(0, .41421356f);
    gEmulator = EMU_MUPEN;
    assert(physics_lab_activity_in_view(&b));
    assert(!physics_lab_enemy_in_view(&b));
    gEmulator = 0;
    sLabView.camera[0][0] = NAN;
    assert(physics_lab_enemy_in_view(&b));
    prepare(0, .41421356f);
    gCurrLevelNum = 1;
    assert(physics_lab_activity_in_view(&b));
    assert(!gWaterEffectViewRequested);
    assert(object_water_effect_in_view(&b)); // Previous level's view cannot suppress waves.
    assert(gWaterEffectViewRequested);
    prepare(0, .41421356f);
    assert(!gWaterEffectViewRequested); // Rendering consumes the request.
    assert(!object_water_effect_in_view(&b));
    assert(physics_lab_enemy_in_view(&b)); // Ordinary enemy view policy is unchanged.
    gCurrAreaIndex = 2;
    assert(object_water_effect_in_view(&b));
    prepare(0, .41421356f);
    assert(!object_water_effect_in_view(&b));
    sLabView.roll = 1;
    assert(object_water_effect_in_view(&b));
    sLabView.roll = 0;
    gAreaUpdateCounter += 2;
    assert(object_water_effect_in_view(&b));
    prepare(0, .41421356f);
    gEmulator = EMU_MUPEN;
    assert(object_water_effect_in_view(&b));
    gEmulator = 0;
    gWaterEffectViewRequested = TRUE;
    prepare(0, 0);
    assert(!gWaterEffectViewRequested && object_water_effect_in_view(&b));
    gCurrLevelNum = LEVEL_PHYSICS_LAB;
    gCurrAreaIndex = 1;
    prepare(0, .41421356f);
    b.min[0] = -10000;
    b.max[0] = 10000;
    assert(physics_lab_activity_in_view(&b));
    // Independent corner half-space reference: any box with an on-screen
    // corner must survive the cosmetic AABB rejection, at different yaw angles.
    for (int turn = 0; turn < 12; turn++) {
        Mat4 m;
        identity(m);
        f32 angle = turn * .52359878f;
        m[0][0] = cosf(angle);
        m[2][0] = -sinf(angle);
        m[0][2] = sinf(angle);
        m[2][2] = cosf(angle);
        physics_lab_prepare_view(m, .7f, .41421356f, 4.0f / 3.0f, 0);
        for (int x = -6000; x <= 6000; x += 1000) {
            for (int z = -6000; z <= 6000; z += 1000) {
                b.min[0] = x;
                b.max[0] = x + 800;
                b.min[2] = z;
                b.max[2] = z + 800;
                int visible = 0;
                for (int mask = 0; mask < 8; mask++) {
                    f32 p[3];
                    for (int a = 0; a < 3; a++) {
                        p[a] = (mask & (1 << a)) ? b.max[a] : b.min[a];
                    }
                    f32 cx = p[0] * m[0][0] + p[2] * m[2][0],
                        depth = -(p[0] * m[0][2] + p[2] * m[2][2]);
                    if (depth > 0 && fabsf(cx) <= depth * .7f && fabsf(p[1]) <= depth * .41421356f) {
                        visible = 1;
                    }
                }
                if (visible) {
                    assert(physics_lab_activity_in_view(&b));
                }
            }
        }
    }
    puts("waves: conservative view checks, roll/stale/wrapped/HLE fallbacks");
}
static void detail_switching(void) {
    struct PhysicsLabRenderSection b = { { -1000, 0, -1000 }, { 1000, 2000, 1000 }, NULL };
    sLabDistantMeshes = 0;
    prepare(9500, .41421356f);
    assert(physics_lab_use_distant_mesh(&b, 8, 0));
    prepare(8500, .41421356f);
    assert(physics_lab_use_distant_mesh(&b, 8, 0));
    prepare(7500, .41421356f);
    assert(!physics_lab_use_distant_mesh(&b, 8, 0));
    prepare(8500, .41421356f);
    assert(!physics_lab_use_distant_mesh(&b, 8, 0));
    prepare(9500, .19891237f);
    assert(!physics_lab_use_distant_mesh(&b, 8, 0));
    assert(!physics_lab_use_distant_mesh(&b, 32, 0));
    sLabView.valid = 0;
    assert(!physics_lab_use_distant_mesh(&b, 8, 0));
    puts("LOD: near/far hysteresis, zoom and invalid-view fallback");
}
static void drawing_limits(void) {
    assert(ARRAY_COUNT(sLabObjectBounds) == 14);
    lab_reset_object_cache();
    prepare(0, .41421356f);
    sLabView.detailScale = 1;
    for (u32 i = 0; i < ARRAY_COUNT(gLoadedGraphNodes); i++) {
        gLoadedGraphNodes[i] = &models[i];
    }
    struct Object object = { 0 };
    struct GraphNodeObject *view = &object.header.gfx;
    for (u32 i = 0; i < ARRAY_COUNT(sLabObjectBounds); i++) {
        const struct PhysicsLabObjectBound *b = &sLabObjectBounds[i];
        view->sharedChild = gLoadedGraphNodes[b->model];
        object.cameraDistanceCulled = TRUE;
        view->cameraToObject[2] = -b->drawDistance - 1;
        assert(physics_lab_object_in_view(view) == 0);
        view->cameraToObject[2] = -b->drawDistance;
        assert(physics_lab_object_in_view(view) == -1);
        if (b->drawDistance == 3500) {
            for (int cycle = 0; cycle < 3; cycle++) {
                for (int jitter = 0; jitter < 6; jitter++) {
                    view->cameraToObject[2] = -(b->drawDistance + (jitter & 1 ? 50 : -50));
                    assert(physics_lab_object_in_view(view) == -1);
                }
                view->cameraToObject[2] = -b->drawDistance * ENEMY_DRAW_DISTANCE_EXIT_SCALE;
                assert(physics_lab_object_in_view(view) == -1);
                view->cameraToObject[2] -= 1;
                assert(physics_lab_object_in_view(view) == 0);
                view->cameraToObject[2] = -b->drawDistance - 50;
                assert(physics_lab_object_in_view(view) == 0);
                view->cameraToObject[2] = -b->drawDistance;
                assert(physics_lab_object_in_view(view) == -1);
            }
            struct Object other = { 0 };
            other.cameraDistanceCulled = TRUE;
            other.header.gfx.sharedChild = view->sharedChild;
            other.header.gfx.cameraToObject[2] = -b->drawDistance - 50;
            assert(physics_lab_object_in_view(&other.header.gfx) == 0);
        } else {
            view->cameraToObject[2] -= 1;
            assert(physics_lab_object_in_view(view) == 0);
        }
        sLabView.detailScale = 2;
        assert(physics_lab_object_in_view(view) == -1);
        sLabView.detailScale = 1;
        view->node.children = &camera;
        assert(physics_lab_object_in_view(view) == -1);
        view->node.children = NULL;
    }
    view->cameraToObject[2] = -20000;
    view->sharedChild = &models[127];
    assert(physics_lab_object_in_view(view) == -1);
    view->sharedChild = NULL;
    assert(physics_lab_object_in_view(view) == -1);
    view->sharedChild = gLoadedGraphNodes[sLabObjectBounds[0].model];
    gEmulator = EMU_MUPEN;
    assert(physics_lab_object_in_view(view) == -1);
    gEmulator = 0;
    gCurrLevelNum = 1;
    assert(physics_lab_object_in_view(view) == -1);
    gCurrLevelNum = LEVEL_PHYSICS_LAB;
    gCurGraphNodeCamera = NULL;
    assert(physics_lab_object_in_view(view) == -1);
    gCurGraphNodeCamera = &camera;
    // Reloaded model pointers must not retain old supported/unsupported policy.
    gLoadedGraphNodes[sLabObjectBounds[0].model] = &models[127];
    lab_reset_object_cache();
    assert(physics_lab_object_in_view(view) == -1);
    view->sharedChild = &models[127];
    assert(physics_lab_object_in_view(view) == 0);
    puts("objects: enemy distance hysteresis, independent objects, unchanged movers, zoom and "
         "fallbacks");
}
static void northern_view(void) {
    const f32 limit = physics_lab_north_view_z;
    prepare(limit - 31, .41421356f);
    assert(!physics_lab_terrain_view());
    prepare(limit - 33, .41421356f);
    assert(physics_lab_terrain_view());
    for (int turn = 0; turn < 12; turn++) {
        Mat4 m;
        identity(m);
        f32 angle = turn * .52359878f;
        m[0][0] = cosf(angle);
        m[2][0] = -sinf(angle);
        m[0][2] = sinf(angle);
        m[2][2] = cosf(angle);
        Vec3f eye = { 4500, 8000, limit - 700 };
        for (int a = 0; a < 3; a++) {
            for (int b = 0; b < 3; b++) {
                m[3][a] -= eye[b] * m[b][a];
            }
        }
        physics_lab_prepare_view(m, .7f, .41421356f, 4.0f / 3.0f, 0);
        assert(physics_lab_terrain_view());
    }
    prepare(limit - 700, .41421356f);
    sLabView.roll = 1;
    assert(physics_lab_terrain_view());
    sLabView.roll = 0;
    gAreaUpdateCounter += 2;
    assert(!physics_lab_terrain_view());
    prepare(limit - 700, .41421356f);
    gCurrAreaIndex = 2;
    assert(!physics_lab_terrain_view());
    gCurrAreaIndex = 1;
    gCurrLevelNum = 1;
    assert(!physics_lab_terrain_view());
    gCurrLevelNum = LEVEL_PHYSICS_LAB;
    sLabView.camera[0][0] = 1.1f;
    assert(!physics_lab_terrain_view());
    prepare(limit - 700, .41421356f);
    sLabView.camera[0][0] = -1.0f;
    assert(physics_lab_terrain_view() == LAB_TERRAIN_COMPLETE);
    prepare(limit - 700, .41421356f);
    sLabView.camera[2][2] = NAN;
    assert(!physics_lab_terrain_view());
    prepare(limit - 700, .41421356f);
    sLabView.camera[3][3] = 0;
    assert(!physics_lab_terrain_view());
    prepare(limit - 700, .41421356f);
    sLabView.camera[0][3] = .1f;
    assert(!physics_lab_terrain_view());
    sLabView.valid = 0;
    assert(!physics_lab_terrain_view());
    Vec3f eye = { 0, physics_lab_aerial_view_y + 700, limit - 700 };
    Mat4 m;
    identity(m);
    for (int a = 0; a < 3; a++) {
        m[3][a] = -eye[a];
    }
    physics_lab_prepare_view(m, .7f, .41421356f, 4.0f / 3.0f, 123);
    // Roll does not change which side of a plane holds the eye.
    assert(physics_lab_terrain_view() == LAB_TERRAIN_AERIAL);
    for (int turn = 0; turn < 12; turn++) {
        identity(m);
        f32 angle = turn * .52359878f;
        m[0][0] = cosf(angle);
        m[1][0] = -sinf(angle);
        m[0][1] = sinf(angle);
        m[1][1] = cosf(angle);
        for (int a = 0; a < 3; a++) {
            for (int b = 0; b < 3; b++) {
                m[3][a] -= eye[b] * m[b][a];
            }
        }
        physics_lab_prepare_view(m, .7f, .41421356f, 4.0f / 3.0f, turn * 1024);
        assert(physics_lab_terrain_view() == LAB_TERRAIN_AERIAL);
    }
    identity(m);
    eye[0] = physics_lab_aerial_view_half_width - 31;
    for (int a = 0; a < 3; a++) {
        m[3][a] = -eye[a];
    }
    physics_lab_prepare_view(m, .7f, .41421356f, 4.0f / 3.0f, 0);
    assert(physics_lab_terrain_view() == LAB_TERRAIN_NORTH);
    eye[0] = 0;
    eye[1] = physics_lab_aerial_view_y + 31;
    for (int a = 0; a < 3; a++) {
        m[3][a] = -eye[a];
    }
    physics_lab_prepare_view(m, .7f, .41421356f, 4.0f / 3.0f, 0);
    assert(physics_lab_terrain_view() == LAB_TERRAIN_NORTH);
    sLabView.camera[3][0] = 70000;
    assert(physics_lab_terrain_view() == LAB_TERRAIN_COMPLETE);
    puts("north: whole-mesh camera gate, rotation, padded boundary and conservative fallbacks");
}
static void ground_clipping(void) {
    struct GraphNode node = { 0 };
    Mat4 m, basis;
    identity(basis);
    identity(m);
    m[0][0] = -1.0f;
    m[1][1] = sqrtf(0.8f);
    m[1][2] = sqrtf(0.2f);
    m[2][1] = sqrtf(0.2f);
    m[2][2] = -sqrtf(0.8f);
    f32 radius = physics_lab_ground_clip_bounds.max[2] * -m[2][2];
    for (int offset = -1; offset <= 1; offset++) {
        m[3][2] = -(radius + LAB_GROUND_WIDE_CLIP_MIN_DEPTH + offset);
        physics_lab_prepare_view(m, .7f, .41421356f, 4.0f / 3.0f, 0);
        assert(physics_lab_terrain_view() == LAB_TERRAIN_AERIAL);
        assert(geo_physics_lab_terrain(GEO_CONTEXT_RENDER, &node, basis) == commands);
        assert(commands[0].target == (offset > 0 ? physics_lab_ground_wide_dl : physics_lab_ground_dl));
    }
    sLabView.roll = 123;
    assert(geo_physics_lab_terrain(GEO_CONTEXT_RENDER, &node, basis) == commands);
    assert(commands[0].target == physics_lab_ground_wide_dl);
    gAreaUpdateCounter += 2;
    assert(geo_physics_lab_terrain(GEO_CONTEXT_RENDER, &node, basis) == commands);
    assert(commands[0].target == physics_lab_ground_dl);
    // The eye stays in the aerial region while the camera looks away.
    for (int row = 0; row < 4; row++) {
        m[row][0] = -m[row][0];
        m[row][2] = -m[row][2];
    }
    physics_lab_prepare_view(m, .7f, .41421356f, 4.0f / 3.0f, 0);
    assert(physics_lab_terrain_view() == LAB_TERRAIN_AERIAL);
    assert(geo_physics_lab_terrain(GEO_CONTEXT_RENDER, &node, basis) == commands);
    assert(commands[0].target == physics_lab_ground_dl);
    puts("ground: full-floor camera-plane clearance, strict boundary, roll and stale/away fallbacks");
}
static void submission(void) {
    struct GraphNode node = { 0 };
    Mat4 basis;
    identity(basis);
    assert(geo_physics_lab_terrain(GEO_CONTEXT_CREATE, &node, basis) == NULL);
    assert(!sLabView.valid && !sLabDistantMeshes);
    assert(geo_physics_lab_terrain(GEO_CONTEXT_RENDER, &node, basis) == physics_lab_terrain_dl);
    for (int far = 0; far < 2; far++) {
        prepare(far ? 30000 : -30000, .41421356f);
        allocationCalls = 0;
        assert(geo_physics_lab_terrain(GEO_CONTEXT_RENDER, &node, basis) == commands);
        assert(allocationCalls == 1
               && allocationSize == (physics_lab_terrain_batch_count + 4) * sizeof(Gfx));
        assert(commands[0].target == physics_lab_ground_dl
               && commands[1].target == physics_lab_terrain_begin);
        for (u32 i = 0; i < physics_lab_terrain_batch_count; i++) {
            const struct PhysicsLabTerrainBatch *b =
                far ? &physics_lab_terrain_batches[i] : &physics_lab_terrain_north_batches[i];
            assert(commands[i + 2].target == (far && b->lodMask ? b->distant : b->near));
        }
        assert(commands[physics_lab_terrain_batch_count + 2].target == physics_lab_terrain_end);
        assert(commands[physics_lab_terrain_batch_count + 3].op == 0);
    }
    failAlloc = 1;
    assert(geo_physics_lab_terrain(GEO_CONTEXT_RENDER, &node, basis) == physics_lab_terrain_dl);
    failAlloc = 0;
    basis[0][0] = 2;
    assert(geo_physics_lab_terrain(GEO_CONTEXT_RENDER, &node, basis) == physics_lab_terrain_dl);
    identity(basis);
    gEmulator = EMU_MUPEN;
    assert(geo_physics_lab_terrain(GEO_CONTEXT_RENDER, &node, basis) == physics_lab_terrain_dl);
    gEmulator = 0;
    for (int a = 0; a < 3; a++) {
        basis[a][a] = LAB_SCULPTURE_SCALE;
    }
    basis[3][0] = LAB_SCULPTURE_ORIGIN_X;
    basis[3][2] = LAB_SCULPTURE_ORIGIN_Z;
    for (int far = 0; far < 2; far++) {
        prepare(far ? 30000 : -30000, .41421356f);
        allocationCalls = 0;
        assert(geo_physics_lab_sculptures(GEO_CONTEXT_RENDER, &node, basis) == commands);
        assert(allocationCalls == 1
               && allocationSize == (physics_lab_sculpture_count + 3) * sizeof(Gfx));
        assert(commands[0].target == physics_lab_terrain_begin);
        for (u32 i = 0; i < physics_lab_sculpture_count; i++) {
            assert(commands[i + 1].target
                   == (far ? physics_lab_sculptures[i].distant
                           : physics_lab_sculptures[i].bounds.displayList));
        }
        assert(commands[physics_lab_sculpture_count + 1].target == physics_lab_terrain_end);
        assert(commands[physics_lab_sculpture_count + 2].op == 0);
    }
    failAlloc = 1;
    assert(geo_physics_lab_sculptures(GEO_CONTEXT_RENDER, &node, basis) == physics_lab_sculptures_dl);
    failAlloc = 0;
    basis[0][0] = 1;
    assert(geo_physics_lab_sculptures(GEO_CONTEXT_RENDER, &node, basis) == physics_lab_sculptures_dl);
    puts("submission: all six terrain runs and twelve statues submit in order, including behind-camera "
         "views; safe fallbacks");
}
int main(void) {
    wave_view();
    detail_switching();
    drawing_limits();
    northern_view();
    ground_clipping();
    submission();
    return 0;
}
