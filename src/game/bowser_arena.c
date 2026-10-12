#include "sm64.h"
#include "area.h"
#include "behavior_data.h"
#include "bowser_arena.h"
#include "game_options.h"
#include "engine/math_util.h"
#include "engine/behavior_script.h"
#include "memory.h"
#include "interaction.h"
#include "mario.h"
#include "level_update.h"
#include "engine/surface_collision.h"
#include "level_table.h"
#include "model_ids.h"
#include "object_constants.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "levels/bowser_3/arena_enemy_assets.h"

extern const struct Animation *const bully_seg5_anims_0500470C[];
extern const struct Animation *const lakitu_enemy_seg5_anims_050144D4[];
extern const struct Animation *const spiny_egg_seg5_anims_050157E4[];
extern const struct Animation *const spiny_seg5_anims_05016EAC[];

// Inset from the five surviving star tips so ground enemies have room to stand.
static const Vec3s sArenaPoints[] = { { 0, 307, -2100 },
                                      { -1950, 307, -650 },
                                      { -1230, 307, 1680 },
                                      { 1230, 307, 1680 },
                                      { 1950, 307, -650 } };
// Inner star corners inset by 20% to leave room for each Mr. I.
static const Vec3s sArenaInnerPoints[] = {
    { -630, 307, -819 }, { 630, 307, -819 }, { 946, 307, 315 }, { 0, 307, 1010 }, { -946, 307, 315 }
};
static const Vec3s sArenaBombs[] = { { -2122, 512, -2912 },
                                     { -3362, 512, 1121 },
                                     { 0, 512, 3584 },
                                     { 3363, 512, 1121 },
                                     { 2123, 512, -2912 } };
static const u8 sArenaSections[5][2] = { { 1, 10 }, { 2, 3 }, { 4, 5 }, { 6, 7 }, { 8, 9 } };

static struct Object *bowser_arena_spawn(struct Object *bowser, ModelID32 model,
                                         const BehaviorScript *behavior, f32 x, f32 y, f32 z) {
    struct Object *child = spawn_object(bowser, model, behavior);
    if (child == NULL) {
        return NULL;
    }
    child->bowserArenaOwner = bowser;
    child->bowserArenaFade = BOWSER_ARENA_FADE_FRAMES;
    child->oIntangibleTimer = -1;
    child->oHomeX = x;
    child->oPosX = child->oHomeX;
    child->oHomeY = y;
    child->oPosY = child->oHomeY;
    child->oHomeZ = z;
    child->oPosZ = child->oHomeZ;
    child->oMoveAngleYaw = atan2s(-z, -x);
    child->oFaceAngleYaw = child->oMoveAngleYaw;
    if (behavior == bhvGoomba) {
        // Encounter ownership is separate from native triplet-spawner lifetime.
        child->parentObj = child;
    } else if (behavior == bhvBoo) {
        // The encounter fade owns its arrival; native Boo fading resumes afterward.
        child->oOpacity = 255;
        child->oBehParams |= (u32) COIN_INSIDE_BOO_BP_YELLOW_COIN << 24;
        child->oMoveAnglePitch = 0;
        child->oFaceAnglePitch = 0;
        child->oFaceAngleRoll = 0;
    }
    return child;
}

static void bowser_arena_fire(struct Object *bowser, s32 point) {
    u8 mask = 1 << point;
    if (!(bowser->bowserArenaFireMask & mask)) {
        const s16 *pos = sArenaPoints[point];
        bowser_arena_spawn(bowser, MODEL_BOWLING_BALL, bhvFireSpitter, pos[0], pos[1] + 80, pos[2]);
        bowser->bowserArenaFireMask |= mask;
    }
}

static void bowser_arena_collapse(struct Object *bowser, s32 section) {
    struct Object *head = (struct Object *) &gObjectLists[OBJ_LIST_SURFACE];
    for (struct Object *obj = (struct Object *) head->header.next; obj != head;
         obj = (struct Object *) obj->header.next) {
        if (obj_has_behavior(obj, bhvFallingBowserPlatform)
            && (section < 0 || obj->oBehParams2ndByte == sArenaSections[section][0]
                || obj->oBehParams2ndByte == sArenaSections[section][1])) {
            obj->bowserArenaCollapse = TRUE;
        }
    }
    if (section < 0) {
        bowser->bowserArenaCollapse = TRUE;
    }
    struct Object *wave = spawn_object(bowser, MODEL_BOWSER_WAVE, bhvBowserShockWave);
    if (wave != NULL) {
        wave->oBehParams2ndByte = BOWSER_SHOCKWAVE_LIGHT;
        wave->oPosY = 307;
        if (section >= 0) {
            wave->oPosX = sArenaPoints[section][0];
            wave->oPosZ = sArenaPoints[section][2];
        }
        mario_queue_bowser_impact(wave);
    }
}

void bowser_arena_platform_destroyed(struct Object *bowser) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        return;
    }
    if (bowser->oBehParams2ndByte != BOWSER_BP_BITS) {
        return;
    }
    struct Object *wave = spawn_object(bowser, MODEL_BOWSER_WAVE, bhvBowserShockWave);
    if (wave != NULL) {
        wave->oBehParams2ndByte = BOWSER_SHOCKWAVE_LIGHT;
        wave->oPosY = 307;
        mario_queue_bowser_impact(wave);
    }
}

void bowser_arena_bomb_destroyed(struct Object *bomb) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        return;
    }
    if (gCurrLevelNum != LEVEL_BOWSER_3) {
        return;
    }
    struct Object *bowser = cur_obj_nearest_object_with_behavior(bhvBowser);
    if (bowser == NULL || bowser->oBehParams2ndByte != BOWSER_BP_BITS) {
        return;
    }
    s32 section = 0;
    f32 best = 1e30f;
    for (s32 i = 0; i < 5; i++) {
        f32 dx = bomb->oPosX - sArenaBombs[i][0];
        f32 dz = bomb->oPosZ - sArenaBombs[i][2];
        f32 distance = dx * dx + dz * dz;
        if (distance < best) {
            best = distance;
            section = i;
        }
    }
    bowser_arena_fire(bowser, section);
    bowser_arena_collapse(bowser, section);
    // Collapse already emits the wave at this bomb's spitter; pulse the others once.
    for (s32 list = 0; list < NUM_OBJ_LISTS; list++) {
        struct Object *head = (struct Object *) &gObjectLists[list];
        for (struct Object *obj = (struct Object *) head->header.next; obj != head;) {
            struct Object *next = (struct Object *) obj->header.next;
            if (obj->bowserArenaOwner == bowser && obj->activeFlags != ACTIVE_FLAG_DEACTIVATED
                && obj_has_behavior(obj, bhvFireSpitter)
                && (obj->oPosX != sArenaPoints[section][0] || obj->oPosZ != sArenaPoints[section][2])) {
                struct Object *wave = spawn_object(obj, MODEL_BOWSER_WAVE, bhvBowserShockWave);
                if (wave != NULL) {
                    wave->oBehParams2ndByte = BOWSER_SHOCKWAVE_LIGHT;
                    wave->oPosX = obj->oPosX;
                    wave->oPosY = 307;
                    wave->oPosZ = obj->oPosZ;
                }
            }
            obj = next;
        }
    }
}

void bowser_arena_landed(struct Object *bowser) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        return;
    }
    if (bowser->oBehParams2ndByte != BOWSER_BP_BITS) {
        return;
    }
    s32 stage = 5 - bowser->oHealth;
    if (stage <= bowser->bowserArenaStage || stage < 1 || stage > 5) {
        return;
    }
    bowser->bowserArenaStage = stage;
    if (stage == 5) {
        if (gMarioState->heldObj != NULL && gMarioState->heldObj->bowserArenaOwner == bowser) {
            if (gMarioState->action & ACT_FLAG_INTANGIBLE) {
                // Dialogues and ending scripts accept empty hands without interruption.
                mario_drop_held_object(gMarioState);
            } else {
                // Retire the holding action before its arena actor is unloaded.
                drop_and_set_mario_action(
                    gMarioState, (gMarioState->action & ACT_FLAG_AIR) ? ACT_FREEFALL : ACT_IDLE, 0);
            }
        }
        for (s32 list = 0; list < NUM_OBJ_LISTS; list++) {
            struct Object *head = (struct Object *) &gObjectLists[list];
            for (struct Object *obj = (struct Object *) head->header.next; obj != head;
                 obj = (struct Object *) obj->header.next) {
                if (obj->bowserArenaOwner == bowser) {
                    obj_mark_for_deletion(obj);
                }
            }
        }
        return;
    }
    if (stage <= 2 || stage == 4) {
        const ModelID32 models[] = { MODEL_GOOMBA, MODEL_BLACK_BOBOMB, MODEL_NONE, MODEL_BULLY };
        const BehaviorScript *behaviors[] = { bhvGoomba, bhvBobomb, NULL, bhvSmallBully };
        for (s32 point = 0; point < 5; point++) {
            const s16 *pos = sArenaPoints[point];
            f32 inset = stage == 4 ? 0.75f : 1.0f;
            bowser_arena_spawn(bowser, models[stage - 1], behaviors[stage - 1], pos[0] * inset, pos[1],
                               pos[2] * inset);
        }
    }
    if (stage == 3) {
        for (s32 point = 0; point < 5; point++) {
            const s16 *pos = sArenaInnerPoints[point];
            bowser_arena_spawn(bowser, MODEL_MR_I_BODY, bhvMrI, pos[0], pos[1], pos[2]);
        }
        bowser_arena_spawn(bowser, MODEL_BOO, bhvBoo, 0, 507, 0);
        bowser_arena_collapse(bowser, -1);
        bowser->bowserArenaBooTimer = 0;
    } else if (stage == 4) {
        for (s32 point = 0; point < 5; point++) {
            const s16 *a = sArenaInnerPoints[point];
            const s16 *b = sArenaInnerPoints[(point + 1) % 5];
            bowser_arena_spawn(bowser, MODEL_SNUFIT, bhvSnufit, (a[0] + b[0]) * 0.5f, 757,
                               (a[2] + b[2]) * 0.5f);
            bowser_arena_fire(bowser, point);
        }
        struct Object *amp = bowser_arena_spawn(bowser, MODEL_AMP, bhvCirclingAmp, 0, 457, 0);
        if (amp != NULL) {
            amp->oBehParams2ndByte = AMP_BP_ROT_RADIUS_400;
        }
    }
}

void bowser_arena_update(struct Object *bowser) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        return;
    }
    if (bowser->oBehParams2ndByte != BOWSER_BP_BITS
        || (bowser->bowserArenaStage != 3 && bowser->bowserArenaStage != 4)) {
        return;
    }
    struct Object *head = (struct Object *) &gObjectLists[OBJ_LIST_GENACTOR];
    for (struct Object *obj = (struct Object *) head->header.next; obj != head;
         obj = (struct Object *) obj->header.next) {
        if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED && obj->bowserArenaOwner == bowser
            && obj_has_behavior(obj, bhvBoo)) {
            bowser->bowserArenaBooTimer = 0;
            return;
        }
    }
    s32 respawnDelay = bowser->bowserArenaStage == 4 ? 900 : 1800;
    if (++bowser->bowserArenaBooTimer >= respawnDelay) {
        bowser->bowserArenaBooTimer = 0;
        bowser_arena_spawn(bowser, MODEL_BOO, bhvBoo, 0, 507, 0);
    }
}

void bowser_arena_recover_enemy(struct Object *obj) {
    if (obj->bowserArenaOwner == NULL || obj->activeFlags == ACTIVE_FLAG_DEACTIVATED
        || obj->oHeldState != HELD_FREE
        || !(obj_has_behavior(obj, bhvGoomba) || obj_has_behavior(obj, bhvBobomb)
             || obj_has_behavior(obj, bhvSpiny) || obj_has_behavior(obj, bhvSmallBully)
             || obj_has_behavior(obj, bhvHomingAmp))) {
        return;
    }
    s32 attacked = (obj->oInteractStatus & INT_STATUS_WAS_ATTACKED)
                   || (obj_has_behavior(obj, bhvSmallBully) && obj->oAction == BULLY_ACT_KNOCKBACK)
                   || (obj_has_behavior(obj, bhvBobomb) && obj->oAction == BOBOMB_ACT_LAUNCHED)
                   || obj->oGroundTwirlStunTimer != 0 || obj->oGroundTwirlContactRebound != 0
                   || obj->oGroundTwirlStunDuration != 0;
    if (attacked) {
        obj->bowserArenaFallAllowed = TRUE;
    }
    struct Surface *floor;
    f32 height = find_floor(obj->oPosX, 1307.0f, obj->oPosZ, &floor);
    if (floor != NULL && floor->type != SURFACE_BURNING && floor->type != SURFACE_DEATH_PLANE
        && height >= 287.0f && obj->oPosY >= height - 20.0f) {
        if (!attacked && (obj->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND)) {
            obj->bowserArenaFallAllowed = FALSE;
        }
        return;
    }
    // Recover after clearing the arena floor by 200 units, before distance culling.
    // Attack knockback stays unprotected until the enemy safely lands again.
    if (obj->oPosY > 107.0f) {
        return;
    }
    if (obj->bowserArenaFallAllowed) {
        obj_mark_for_deletion(obj);
        return;
    }
    f32 x = obj->oHomeX;
    f32 z = obj->oHomeZ;
    height = find_floor(x, 1307.0f, z, &floor);
    if (floor == NULL || floor->type == SURFACE_BURNING || floor->type == SURFACE_DEATH_PLANE
        || height < 287.0f) {
        x = 600.0f;
        z = 0.0f;
        height = find_floor(x, 1307.0f, z, &floor);
        if (floor == NULL || floor->type == SURFACE_BURNING || floor->type == SURFACE_DEATH_PLANE
            || height < 287.0f) {
            return;
        }
        obj->oHomeX = x;
        obj->oHomeZ = z;
    }
    obj->oPosX = x;
    obj->oPosY = height + (obj_has_behavior(obj, bhvHomingAmp) ? 100.0f : 0.0f);
    obj->oPosZ = z;
    obj->oMoveAngleYaw = atan2s(-z, -x);
    obj->oFaceAngleYaw = obj->oMoveAngleYaw;
    obj->oForwardVel = 0.0f;
    obj->oVelZ = 0.0f;
    obj->oVelY = 0.0f;
    obj->oVelX = 0.0f;
    obj->oFloor = floor;
    obj->oFloorHeight = height;
    obj->oFloorType = floor->type;
    obj->oMoveFlags = OBJ_MOVE_ON_GROUND;
    if (obj->oAction == OBJ_ACT_LAVA_DEATH || obj->oAction == OBJ_ACT_DEATH_PLANE_DEATH) {
        obj->oTimer = 0;
        obj->oSubAction = 0;
        obj->oAction = 0;
    }
}

s32 bowser_arena_phase_in(struct Object *obj) {
    if (obj->bowserArenaOwner != NULL && obj_has_behavior(obj, bhvSpiny)
        && ++obj->bowserArenaSpinyAge >= 900) {
        obj_mark_for_deletion(obj);
        return TRUE;
    }
    if (obj->bowserArenaFade == 0) {
        return FALSE;
    }
    if (obj->curBhvCommand == obj->behavior) {
        cur_obj_update();
    }
    obj->bowserArenaFade--;
    obj->oIntangibleTimer = obj->bowserArenaFade == 0 ? 0 : -1;
    return TRUE;
}

s32 bowser_arena_try_drop_coin(struct Object *obj) {
    if (obj->bowserArenaOwner == NULL) {
        return FALSE;
    }
    if (!obj->bowserArenaCoinRolled) {
        obj->bowserArenaCoinRolled = TRUE;
        if (obj_has_behavior(obj, bhvMrI)
            || (!obj_has_behavior(obj, bhvSpiny) && random_float() < 0.2f)) {
            struct Object *coin = spawn_object(obj, MODEL_YELLOW_COIN, bhvMovingYellowCoin);
            if (coin != NULL) {
                coin->bowserArenaOwner = NULL;
                coin->oVelY = 30;
                coin->oForwardVel = 10;
            }
        }
    }
    obj->oNumLootCoins = 0;
    return TRUE;
}

void *bowser_arena_actor_asset(const BehaviorScript *behavior, const void *asset) {
    if (gCurrLevelNum == LEVEL_BOWSER_3) {
        if (behavior == segmented_to_virtual(bhvSmallBully) && asset == bully_seg5_anims_0500470C) {
            return (void *) bowser_arena_bully_seg5_anims_0500470C;
        }
        if (behavior == segmented_to_virtual(bhvEnemyLakitu)
            && asset == lakitu_enemy_seg5_anims_050144D4) {
            return (void *) bowser_arena_lakitu_enemy_seg5_anims_050144D4;
        }
        if (behavior == segmented_to_virtual(bhvSpiny)) {
            if (asset == spiny_egg_seg5_anims_050157E4) {
                return (void *) bowser_arena_spiny_egg_seg5_anims_050157E4;
            }
            if (asset == spiny_seg5_anims_05016EAC) {
                return (void *) bowser_arena_spiny_seg5_anims_05016EAC;
            }
        }
    }
    return (void *) asset;
}

ModelID32 bowser_arena_actor_model(const BehaviorScript *behavior, ModelID32 model) {
    if (gCurrLevelNum == LEVEL_BOWSER_3) {
        if (behavior == segmented_to_virtual(bhvMrI) && model == MODEL_MR_I_BODY) {
            return BOWSER_ARENA_MODEL_MR_I;
        }
        if (behavior == segmented_to_virtual(bhvMrIIris) && model == MODEL_MR_I_IRIS) {
            return BOWSER_ARENA_MODEL_MR_I_IRIS;
        }
        if (behavior == segmented_to_virtual(bhvBoo) && model == MODEL_BOO) {
            return BOWSER_ARENA_MODEL_BOO;
        }
        if (behavior == segmented_to_virtual(bhvSnufit) && model == MODEL_SNUFIT) {
            return BOWSER_ARENA_MODEL_SNUFIT;
        }

        if (behavior == segmented_to_virtual(bhvSmallBully) && model == MODEL_BULLY) {
            return BOWSER_ARENA_MODEL_BULLY;
        }
        if (behavior == segmented_to_virtual(bhvEnemyLakitu) && model == MODEL_ENEMY_LAKITU) {
            return BOWSER_ARENA_MODEL_LAKITU;
        }
        if (behavior == segmented_to_virtual(bhvSpiny)) {
            if (model == MODEL_SPINY_BALL) {
                return BOWSER_ARENA_MODEL_SPINY_BALL;
            }
            if (model == MODEL_SPINY) {
                return BOWSER_ARENA_MODEL_SPINY;
            }
        }
    }
    return model;
}
