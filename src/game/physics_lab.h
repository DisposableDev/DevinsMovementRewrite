#ifndef PHYSICS_LAB_H
#define PHYSICS_LAB_H

#include "types.h"

struct GraphNode;
struct GraphNodeObject;
struct Object;

struct PhysicsLabRenderSection {
    Vec3s min;
    Vec3s max;
    const Gfx *displayList;
};

struct PhysicsLabTerrainBatch {
    const Gfx *near;
    const Gfx *distant;
    u8 lodMask;
};

#define LAB_SCULPTURE_ORIGIN_X -8000
#define LAB_SCULPTURE_ORIGIN_Z 10500
#define LAB_SCULPTURE_SCALE 0.25f

struct PhysicsLabSculpture {
    struct PhysicsLabRenderSection bounds; // Precomputed world-space envelope.
    const Gfx *distant;
    u8 slot;
};

// Copy the camera view for this area update. horizontal/vertical are half-FOV
// tangents, aspect is width/height, and roll uses SM64 signed-angle units.
void physics_lab_prepare_view(Mat4 camera, f32 horizontal, f32 vertical, f32 aspect, s16 roll);
// Distant cosmetic emitters request a snapshot; camera rendering consumes it.
extern s32 gWaterEffectViewRequested;
s32 object_water_effect_in_view(const struct PhysicsLabRenderSection *bounds);
Gfx *geo_physics_lab_terrain(s32 callContext, struct GraphNode *node, Mat4 mtx);
s32 physics_lab_object_in_view(struct GraphNodeObject *node);
s32 physics_lab_activity_in_view(const struct PhysicsLabRenderSection *bounds);
s32 physics_lab_enemy_in_view(const struct PhysicsLabRenderSection *bounds);
// Shared audited enemy sleeping; extra aggressive policies remain Lab-only.
void enemy_sleep_init(struct Object *obj);
s32 enemy_should_sleep(struct Object *obj);
// Refresh the validated owner's visibility history; TRUE means this model must stay hidden.
// Call before enemy_should_sleep so its reveal guards read current history.
s32 physics_lab_update_enemy_hidden_state(struct Object *obj);
// Mark eligible distant Lab effects for deletion after updating tracked spawner bookkeeping.
// TRUE tells the caller to skip native behavior; object unloading happens later.
s32 physics_lab_discard_distant_effect(struct Object *obj);
s32 physics_lab_monty_mole_can_sleep(void);
s32 physics_lab_needs_mario_angle(struct Object *obj);
// TRUE skips a settled Lab box update after refreshing drawing distance and render visibility.
s32 physics_lab_skip_box_update(struct Object *obj);
// Decide whether callers should allocate cosmetic waves. Distant ordinary-level checks
// may request a camera snapshot; this helper does not spawn objects.
s32 physics_lab_should_emit_water_effect(struct Object *obj);
// Latches LOD hysteresis per stable slot (0..31). switchDistance is in world
// units; zero derives the threshold from bounds. FALSE retains the near mesh.
s32 physics_lab_use_distant_mesh(const struct PhysicsLabRenderSection *bounds, u32 slot,
                                 f32 switchDistance);
Gfx *geo_physics_lab_sculptures(s32 callContext, struct GraphNode *node, Mat4 mtx);

// behavior must be a virtual pointer; asset is an unconverted animation/collision reference.
// Lab matches use its private bank. Unmatched requests pass through the Bowser adapter,
// which returns the original reference/model when it has no match.
void *physics_lab_actor_asset(const BehaviorScript *behavior, const void *asset);
ModelID32 physics_lab_actor_model(const BehaviorScript *behavior, ModelID32 model);
// Tests only X/Z bounds in Lab area 1, strictly inside the cold pool.
// Callers decide whether Mario is in water; this query does not inspect Y or water level.
s32 physics_lab_is_cold_pool(s32 level, s32 area, f32 x, f32 z);

#endif
