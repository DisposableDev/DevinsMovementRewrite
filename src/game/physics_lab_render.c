#include <ultra64.h>
#include "sm64.h"
#include "config.h"
#include "engine/behavior_script.h"
#include "engine/math_util.h"
#include "area.h"
#include "emutest.h"
#include "level_table.h"
#include "memory.h"
#include "physics_lab.h"
#include "rendering_graph_node.h"
#include "levels/physics_lab/header.h"
#include "levels/physics_lab/enemy_assets.h"
#include "model_ids.h"

// Depth/zoom for static LOD and a cheap, conservative view test ONLY for
// cosmetic waves and distant idle enemies. No wall tests.
struct PhysicsLabView {
    Mat4 camera;
    f32 viewDepth[4];
    f32 horizontalHalfFovTan, verticalHalfFovTan, detailScale;
    u16 areaUpdateStamp;
    s16 roll;
    s16 level;
    s16 area;
    s32 valid;
};
static struct PhysicsLabView sLabView;
// Shared LOD latches reset at geometry creation. Terrain owns slots 0..7;
// sculptures use their generated slots above that range.
static u32 sLabDistantMeshes;
s32 gWaterEffectViewRequested;

void physics_lab_prepare_view(Mat4 camera, f32 horizontal, f32 vertical, f32 aspect, s16 roll) {
    gWaterEffectViewRequested = FALSE;
    sLabView.valid = FALSE;
    if (!(horizontal > 0.0f && vertical > 0.0f && aspect > 0.0f)) {
        return;
    }
    mtxf_copy(sLabView.camera, camera);
    for (s32 a = 0; a < 4; a++) {
        sLabView.viewDepth[a] = -camera[a][2];
    }
    sLabView.horizontalHalfFovTan = horizontal;
    sLabView.verticalHalfFovTan = vertical;
    sLabView.detailScale = MAX(1.0f, 1.0f / aspect) / (2.41421356f * vertical);
    sLabView.roll = roll;
    sLabView.areaUpdateStamp = gAreaUpdateCounter;
    sLabView.level = gCurrLevelNum;
    sLabView.area = gCurrAreaIndex;
    sLabView.valid = TRUE;
}

static s32 object_bounds_in_view(const struct PhysicsLabRenderSection *bounds) {
    // Unknown views retain activity; accept the current or previous area update.
    // The u16 age subtraction intentionally wraps with the area counter.
    if (!sLabView.valid || sLabView.level != gCurrLevelNum || sLabView.area != gCurrAreaIndex
        || sLabView.roll != 0 || (u16) (gAreaUpdateCounter - sLabView.areaUpdateStamp) > 1) {
        return TRUE;
    }
    Vec3f center, extent, position, radius;
    for (s32 a = 0; a < 3; a++) {
        center[a] = (bounds->min[a] + bounds->max[a]) * 0.5f;
        extent[a] = (bounds->max[a] - bounds->min[a]) * 0.5f;
    }
    for (s32 axis = 0; axis < 3; axis++) {
        position[axis] = sLabView.camera[3][axis];
        radius[axis] = 32.0f;
        for (s32 a = 0; a < 3; a++) {
            position[axis] += center[a] * sLabView.camera[a][axis];
            radius[axis] += extent[a] * absf(sLabView.camera[a][axis]);
        }
        if (!(absf(position[axis]) <= 1.0e30f && radius[axis] >= 32.0f && radius[axis] <= 1.0e30f)) {
            return TRUE;
        }
    }
    f32 farDepth = -position[2] + radius[2];
    // No near/far rejection; invalid arithmetic conservatively retains updates.
    return !(absf(position[0]) > farDepth * sLabView.horizontalHalfFovTan + radius[0]
             || absf(position[1]) > farDepth * sLabView.verticalHalfFovTan + radius[1]);
}

s32 physics_lab_enemy_in_view(const struct PhysicsLabRenderSection *bounds) {
    return gCurrLevelNum != LEVEL_PHYSICS_LAB || object_bounds_in_view(bounds);
}

s32 object_water_effect_in_view(const struct PhysicsLabRenderSection *bounds) {
#ifndef CULLING_ON_EMULATOR
    if (gEmulator & (EMU_PROJECT64 | EMU_PARALLEL_LAUNCHER | EMU_MUPEN)) {
        return TRUE;
    }
#endif
    // Only a distant awake emitter requests an ordinary-level snapshot. A
    // missing or stale snapshot retains waves until rendering refreshes it.
    if (gCurrLevelNum != LEVEL_PHYSICS_LAB) {
        gWaterEffectViewRequested = TRUE;
    }
    return object_bounds_in_view(bounds);
}

s32 physics_lab_activity_in_view(const struct PhysicsLabRenderSection *bounds) {
#ifndef CULLING_ON_EMULATOR
    if (gEmulator & (EMU_PROJECT64 | EMU_PARALLEL_LAUNCHER | EMU_MUPEN)) {
        return TRUE;
    }
#endif
    return physics_lab_enemy_in_view(bounds);
}

s32 physics_lab_use_distant_mesh(const struct PhysicsLabRenderSection *bounds, u32 slot,
                                 f32 switchDistance) {
    if (!sLabView.valid || slot >= 32 || !(sLabView.detailScale > 0.0f)) {
        return FALSE;
    }
    f32 depth = sLabView.viewDepth[3], span = 0.0f;
    for (s32 a = 0; a < 3; a++) {
        f32 coefficient = sLabView.viewDepth[a];
        depth += coefficient * (coefficient >= 0.0f ? bounds->min[a] : bounds->max[a]);
        span = MAX(span, bounds->max[a] - bounds->min[a]);
    }
    f32 threshold =
        (switchDistance > 0.0f ? switchDistance : MAX(2250.0f, 3.75f * span)) * sLabView.detailScale;
    u32 bit = 1u << slot;
    if (depth > threshold * 1.1f) {
        sLabDistantMeshes |= bit;
    } else if (depth < threshold * 0.9f) {
        sLabDistantMeshes &= ~bit;
    }
    return (sLabDistantMeshes & bit) != 0;
}

// Simple model-level draw limits measured along camera depth.
struct PhysicsLabObjectBound {
    u16 model;
    u16 drawDistance;
};
#include "levels/physics_lab/object_bounds.inc.c"

// Cache only the simple drawing policy, including models with no added limit.
// A NULL modelNode is an empty slot; a NULL drawPolicy under a key caches no limit.
struct PhysicsLabObjectCache {
    struct GraphNode *modelNode;
    const struct PhysicsLabObjectBound *drawPolicy;
};
// The hash and probe masks require a power-of-two capacity.
static struct PhysicsLabObjectCache sLabObjectCache[128];

static void lab_reset_object_cache(void) {
    // Keys own occupancy; each reused slot overwrites its previous policy.
    for (u32 i = 0; i < ARRAY_COUNT(sLabObjectCache); i++) {
        sLabObjectCache[i].modelNode = NULL;
    }
}

static const struct PhysicsLabObjectBound *lab_object_bound(struct GraphNode *model) {
    uintptr_t key = (uintptr_t) model;
    u32 slot = ((key >> 4) ^ (key >> 10)) & (ARRAY_COUNT(sLabObjectCache) - 1);
    struct PhysicsLabObjectCache *entry = NULL;
    // Bounded probing avoids evicting common models every frame. A saturated
    // bucket takes the ordinary renderer; visibility is never guessed.
    for (u32 probe = 0; probe < 8; probe++) {
        entry = &sLabObjectCache[(slot + probe) & (ARRAY_COUNT(sLabObjectCache) - 1)];
        if (entry->modelNode == model) {
            return entry->drawPolicy;
        }
        if (entry->modelNode == NULL) {
            break;
        }
    }
    if (entry->modelNode != NULL) {
        return NULL;
    }
    entry->modelNode = model;
    entry->drawPolicy = NULL;
    for (u32 i = 0; i < ARRAY_COUNT(sLabObjectBounds); i++) {
        if (model == gLoadedGraphNodes[sLabObjectBounds[i].model]) {
            entry->drawPolicy = &sLabObjectBounds[i];
            break;
        }
    }
    return entry->drawPolicy;
}

// Return -1 to keep the game's ordinary culling, or 0 for a simple distance
// cutoff. Never inspect animated vertices, shadow bounds or opaque walls.
s32 physics_lab_object_in_view(struct GraphNodeObject *node) {
    if (gCurrLevelNum != LEVEL_PHYSICS_LAB || !sLabView.valid || node->sharedChild == NULL
        || node->node.children != NULL || gCurGraphNodeCamera == NULL
        || gCurGraphNodeCamFrustum == NULL) {
        return -1;
    }
#ifndef CULLING_ON_EMULATOR
    if (gEmulator & (EMU_PROJECT64 | EMU_PARALLEL_LAUNCHER | EMU_MUPEN)) {
        return -1;
    }
#endif
    const struct PhysicsLabObjectBound *bound = lab_object_bound(node->sharedChild);
    if (bound == NULL) {
        return -1;
    }
    f32 distance = bound->drawDistance * MAX(1.0f, sLabView.detailScale);
    // The 3,500-unit policies belong to enemies; mover cutoffs stay unchanged.
    if (bound->drawDistance == 3500) {
        struct Object *obj = (struct Object *) node;
        if (!obj->cameraDistanceCulled) {
            distance *= ENEMY_DRAW_DISTANCE_EXIT_SCALE;
        }
        obj->cameraDistanceCulled = -node->cameraToObject[2] > distance;
        return obj->cameraDistanceCulled ? 0 : -1;
    }
    return -node->cameraToObject[2] > distance ? 0 : -1;
}

// World-unit clearance from the camera plane for the complete checker floor.
#define LAB_GROUND_WIDE_CLIP_MIN_DEPTH 4096.0f

enum PhysicsLabTerrainView {
    LAB_TERRAIN_COMPLETE,
    LAB_TERRAIN_NORTH,
    LAB_TERRAIN_AERIAL,
};

// One whole-mesh camera classification selects guaranteed backface omissions.
// No section frustum tests or occlusion volumes are involved. Invalid, stale or
// unusual camera matrices retain the complete list.
static enum PhysicsLabTerrainView physics_lab_terrain_view(void) {
    if (!sLabView.valid || sLabView.level != gCurrLevelNum || sLabView.area != gCurrAreaIndex
        || (u16) (gAreaUpdateCounter - sLabView.areaUpdateStamp) > 1) {
        return LAB_TERRAIN_COMPLETE;
    }
    f32 eyeZ = 0.0f;
    for (s32 axis = 0; axis < 3; axis++) {
        eyeZ -= sLabView.camera[3][axis] * sLabView.camera[2][axis];
        if (sLabView.camera[axis][3] != 0.0f || !(absf(sLabView.camera[3][axis]) <= 65536.0f)) {
            return LAB_TERRAIN_COMPLETE;
        }
    }
    f32 northViewZ = *(f32 *) segmented_to_virtual(&physics_lab_north_view_z);
    if (!(eyeZ < northViewZ - 32.0f && absf(eyeZ) <= 1.0e30f) || sLabView.camera[3][3] != 1.0f) {
        return LAB_TERRAIN_COMPLETE;
    }
    // R^-1 = R^T is valid only for the ordinary orthonormal camera basis.
    for (s32 a = 0; a < 3; a++) {
        for (s32 b = a; b < 3; b++) {
            f32 dot = 0.0f;
            for (s32 axis = 0; axis < 3; axis++) {
                dot += sLabView.camera[a][axis] * sLabView.camera[b][axis];
            }
            if (!(absf(dot - (a == b ? 1.0f : 0.0f)) < 0.00001f)) {
                return LAB_TERRAIN_COMPLETE;
            }
        }
    }
    // A reflected basis reverses the RSP's winding even though it is orthonormal.
    f32 handedness = sLabView.camera[0][0]
                         * (sLabView.camera[1][1] * sLabView.camera[2][2]
                            - sLabView.camera[1][2] * sLabView.camera[2][1])
                     - sLabView.camera[0][1]
                           * (sLabView.camera[1][0] * sLabView.camera[2][2]
                              - sLabView.camera[1][2] * sLabView.camera[2][0])
                     + sLabView.camera[0][2]
                           * (sLabView.camera[1][0] * sLabView.camera[2][1]
                              - sLabView.camera[1][1] * sLabView.camera[2][0]);
    if (!(handedness > 0.0f)) {
        return LAB_TERRAIN_COMPLETE;
    }
    f32 eyeX = 0.0f, eyeY = 0.0f;
    for (s32 axis = 0; axis < 3; axis++) {
        eyeX -= sLabView.camera[3][axis] * sLabView.camera[0][axis];
        eyeY -= sLabView.camera[3][axis] * sLabView.camera[1][axis];
    }
    f32 aerialViewY = *(f32 *) segmented_to_virtual(&physics_lab_aerial_view_y);
    f32 aerialViewHalfWidth = *(f32 *) segmented_to_virtual(&physics_lab_aerial_view_half_width);
    if (absf(eyeX) < aerialViewHalfWidth - 32.0f && eyeY > aerialViewY + 32.0f) {
        return LAB_TERRAIN_AERIAL;
    }
    return LAB_TERRAIN_NORTH;
}

Gfx *geo_physics_lab_terrain(s32 callContext, struct GraphNode *node, Mat4 mtx) {
    if (callContext == GEO_CONTEXT_CREATE) {
        lab_reset_object_cache();
        sLabDistantMeshes = 0;
        sLabView.valid = FALSE;
        return NULL;
    }
    if (callContext != GEO_CONTEXT_RENDER || gCurrLevelNum != LEVEL_PHYSICS_LAB) {
        return NULL;
    }
    SET_GRAPH_NODE_LAYER(node->flags, LAYER_OPAQUE);
    Gfx *fallback = segmented_to_virtual(physics_lab_terrain_dl);
    if (!sLabView.valid || gCurGraphNodeCamera == NULL || gCurGraphNodeCamFrustum == NULL) {
        return fallback;
    }
#ifndef CULLING_ON_EMULATOR
    if (gEmulator & (EMU_PROJECT64 | EMU_PARALLEL_LAUNCHER | EMU_MUPEN)) {
        return fallback;
    }
#endif
    for (s32 row = 0; row < 4; row++) {
        for (s32 col = 0; col < 4; col++) {
            if (mtx[row][col] != (row == col ? 1.0f : 0.0f)) {
                return fallback;
            }
        }
    }
    const struct PhysicsLabRenderSection *bounds = segmented_to_virtual(physics_lab_terrain_lod_bounds);
    u32 lodCount = MIN(*(u32 *) segmented_to_virtual(&physics_lab_terrain_lod_count), 8u);
    u32 distantLodMask = 0;
    for (u32 i = 0; i < lodCount; i++) {
        if (physics_lab_use_distant_mesh(&bounds[i], i, 5000.0f)) {
            distantLodMask |= 1u << i;
        }
    }
    enum PhysicsLabTerrainView variant = physics_lab_terrain_view();
    const struct PhysicsLabTerrainBatch *batches = physics_lab_terrain_batches;
    if (variant == LAB_TERRAIN_AERIAL) {
        batches = physics_lab_terrain_aerial_batches;
    } else if (variant == LAB_TERRAIN_NORTH) {
        batches = physics_lab_terrain_north_batches;
    }
    const Gfx *ground = physics_lab_ground_dl;
    if (variant == LAB_TERRAIN_AERIAL) {
        // This variant has already validated the current orthonormal camera.
        // The nearest box corner bounds every floor triangle's forward depth.
        const struct PhysicsLabRenderSection *floorBounds =
            segmented_to_virtual(&physics_lab_ground_clip_bounds);
        f32 nearestDepth = sLabView.viewDepth[3];
        for (s32 a = 0; a < 3; a++) {
            f32 depth = sLabView.viewDepth[a];
            nearestDepth += (depth >= 0.0f ? floorBounds->min[a] : floorBounds->max[a]) * depth;
        }
        if (nearestDepth > LAB_GROUND_WIDE_CLIP_MIN_DEPTH && nearestDepth <= 65536.0f) {
            ground = physics_lab_ground_wide_dl;
        }
    }
    batches = segmented_to_virtual(batches);
    u32 batchCount = *(u32 *) segmented_to_virtual(&physics_lab_terrain_batch_count);
    // Ground, material begin/end and the terminator add four commands.
    Gfx *list = alloc_display_list((batchCount + 4) * sizeof(*list));
    if (list == NULL) {
        return fallback;
    }
    Gfx *head = list;
    gSPDisplayList(head++, ground);
    gSPDisplayList(head++, physics_lab_terrain_begin);
    for (u32 i = 0; i < batchCount; i++) {
        gSPDisplayList(head++,
                       distantLodMask & batches[i].lodMask ? batches[i].distant : batches[i].near);
    }
    gSPDisplayList(head++, physics_lab_terrain_end);
    gSPEndDisplayList(head);
    return list;
}
