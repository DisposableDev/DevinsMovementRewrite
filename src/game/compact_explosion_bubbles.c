#include "compact_explosion_bubbles.h"

#include "area.h"
#include "behavior_data.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "level_update.h"
#include "mario.h"
#include "model_ids.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "particle_budget.h"

// The ordinary bubble needs a full Object, a behavior script and a graph node
// for a position, two animated scales and a short lifetime. Keep the same
// animation in compact slots; the renderer submits their quads in one run.
struct CompactExplosionBubble {
    Vec3f pos;
    f32 velY;
    f32 scaleX;
    f32 scaleY;
    s16 phaseX;
    s16 phaseY;
    s16 rateX;
    s16 rateY;
    s16 areaIndex;
    u8 timer;
    u8 delayed;
    u8 active;
};

static struct CompactExplosionBubble sBubbles[COMPACT_EXPLOSION_BUBBLE_CAPACITY];
static s32 sBubbleCount;
static s32 sBubbleHighWater;

void compact_explosion_bubbles_reset(void) {
    for (s32 i = 0; i < sBubbleHighWater; i++) {
        sBubbles[i].active = FALSE;
    }
    sBubbleCount = 0;
    sBubbleHighWater = 0;
}

s32 compact_explosion_bubble_spawn(struct Object *parent) {
    s32 index;
    struct CompactExplosionBubble *bubble;
    if (sBubbleHighWater < COMPACT_EXPLOSION_BUBBLE_CAPACITY) {
        index = sBubbleHighWater++;
    } else {
        for (index = 0; index < sBubbleHighWater && sBubbles[index].active; index++) {
        }
        if (index == COMPACT_EXPLOSION_BUBBLE_CAPACITY) {
            return FALSE;
        }
    }
    bubble = &sBubbles[index];
    bubble->active = TRUE;
    bubble->delayed = TRUE;
    bubble->areaIndex = parent->header.gfx.areaIndex;
    bubble->scaleX = 2.0f;
    bubble->scaleY = 2.0f;
    bubble->phaseX = 0;
    bubble->phaseY = 0;
    bubble->rateX = (s32) (random_float() * 2048.0f) + 0x800;
    bubble->rateY = (s32) (random_float() * 2048.0f) + 0x800;
    bubble->timer = random_float() * 10.0f;
    bubble->velY = (s32) (random_float() * 4.0f) + 4;
    bubble->pos[0] = parent->oPosX + random_float() * 100.0f - 50.0f;
    bubble->pos[1] = parent->oPosY + random_float() * 100.0f - 50.0f;
    bubble->pos[2] = parent->oPosZ + random_float() * 100.0f - 50.0f;
    bubble->pos[0] += random_float() * 4.0f - 2.0f;
    bubble->pos[2] += random_float() * 4.0f - 2.0f;
    sBubbleCount++;
    return TRUE;
}

void compact_explosion_bubbles_update(void) {
    s16 areaIndex = gCurrentArea != NULL ? gCurrentArea->index : -1;
    f32 waterY = gMarioState != NULL ? gMarioState->waterLevel : FLOOR_LOWER_LIMIT_MISC;
    if (sBubbleCount == 0) {
        return;
    }
    for (s32 i = 0; i < sBubbleHighWater; i++) {
        struct CompactExplosionBubble *bubble = &sBubbles[i];
        if (!bubble->active) {
            continue;
        }
        if (bubble->areaIndex != areaIndex) {
            bubble->active = FALSE;
            sBubbleCount--;
            continue;
        }
        if (bubble->delayed) {
            bubble->delayed = FALSE;
            bubble->timer++;
            continue;
        }
        bubble->pos[0] += random_float() * 4.0f - 2.0f;
        bubble->pos[2] += random_float() * 4.0f - 2.0f;
        bubble->scaleX = sins(bubble->phaseX) * 0.5f + 2.0f;
        bubble->phaseX += bubble->rateX;
        bubble->scaleY = sins(bubble->phaseY) * 0.5f + 2.0f;
        bubble->phaseY += bubble->rateY;
        if (bubble->pos[1] > waterY) {
            struct Object *splash = try_spawn_particle(gMarioObject, MODEL_SMALL_WATER_SPLASH,
                                                       bhvObjectWaterSplash, PARTICLE_PRIORITY_DROPLET);
            if (splash != NULL) {
                splash->oPosX = bubble->pos[0];
                splash->oPosY = bubble->pos[1] + 5.0f;
                splash->oPosZ = bubble->pos[2];
            }
        }
        if (bubble->pos[1] > waterY || bubble->timer > 60) {
            bubble->active = FALSE;
            sBubbleCount--;
            continue;
        }
        bubble->pos[1] += bubble->velY;
        bubble->timer += 2; // Native loop and object update both increment oTimer.
    }
    while (sBubbleHighWater > 0 && !sBubbles[sBubbleHighWater - 1].active) {
        sBubbleHighWater--;
    }
}

s32 compact_explosion_bubbles_count(void) {
    return sBubbleCount;
}

s32 compact_explosion_bubble_view(s32 index, Vec3f pos, Vec3f scale, s16 *areaIndex) {
    struct CompactExplosionBubble *bubble;
    if (index < 0 || index >= sBubbleHighWater) {
        return FALSE;
    }
    bubble = &sBubbles[index];
    if (!bubble->active) {
        return FALSE;
    }
    vec3_copy(pos, bubble->pos);
    vec3f_set(scale, bubble->scaleX, bubble->scaleY, 1.0f);
    *areaIndex = bubble->areaIndex;
    return TRUE;
}
