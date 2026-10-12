#include "game/bowser_arena.h"
#include "sm64.h"
#include "behavior_data.h"
#include "level_table.h"
#include "model_ids.h"
#include "area.h"
#include "memory.h"
#include "physics_lab.h"
#include "levels/physics_lab/enemy_assets.h"

struct LabAssetRemap {
    const BehaviorScript *behavior;
    const void *original;
    const void *replacement;
};
struct LabModelRemap {
    const BehaviorScript *behavior;
    ModelID32 original;
    ModelID32 replacement;
};
#include "levels/physics_lab/enemy_remap.inc.c"

// Original behavior identities remain intact for combat, child spawning and
// object searches. Only their asset references change in this one course.
void *physics_lab_actor_asset(const BehaviorScript *behavior, const void *asset) {
    if (gCurrLevelNum == LEVEL_PHYSICS_LAB) {
        for (u32 i = 0; i < ARRAY_COUNT(sLabAssets); i++) {
            if (asset == sLabAssets[i].original
                && behavior == segmented_to_virtual(sLabAssets[i].behavior)) {
                return (void *) sLabAssets[i].replacement;
            }
        }
    }
    return bowser_arena_actor_asset(behavior, asset);
}

ModelID32 physics_lab_actor_model(const BehaviorScript *behavior, ModelID32 model) {
    if (gCurrLevelNum == LEVEL_PHYSICS_LAB) {
        for (u32 i = 0; i < ARRAY_COUNT(sLabModels); i++) {
            if (model == sLabModels[i].original
                && behavior == segmented_to_virtual(sLabModels[i].behavior)) {
                return sLabModels[i].replacement;
            }
        }
    }
    return bowser_arena_actor_model(behavior, model);
}

s32 physics_lab_is_cold_pool(s32 level, s32 area, f32 x, f32 z) {
    // Matches water box 3; the other pools retain ordinary water behavior.
    return level == LEVEL_PHYSICS_LAB && area == 1 && x > 3300.0f && x < 5700.0f && z > 11400.0f
           && z < 14300.0f;
}
