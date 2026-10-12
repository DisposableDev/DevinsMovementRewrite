#include "game/bowser_arena.h"
#include "game/game_options.h"
// bowser_bomb.inc.c

void bhv_bowser_bomb_init(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        o->oInteractType = 0;
        o->oInteractionSubtype &= ~INT_SUBTYPE_BOWSER_ATTACK;
        o->oDamageOrCoinValue = 0;
        o->hitboxRadius = 40.0f;
        o->hitboxHeight = 40.0f;
        o->hitboxDownOffset = 40.0f;
    }
}

void bhv_bowser_bomb_loop(void) {
    if (o->oAction == BOWSER_BOMB_ACT_RESPAWNING) {
        // The detonation update is timer zero; restore after 60 further updates.
        if (o->oTimer < BOWSER_BOMB_RESPAWN_FRAMES - 1) {
            return;
        }
        o->oInteractStatus = 0;
        o->oAction = BOWSER_BOMB_ACT_ARMED;
        cur_obj_unhide();
        cur_obj_become_tangible();
        set_object_visibility(o, 7000);
        return;
    }

    // Bowser's cinematic explosion and mine consumption keep arena progression.
    if (o->oInteractStatus & INT_STATUS_HIT_MINE) {
        bowser_arena_bomb_destroyed(o);
        spawn_object(o, MODEL_BOWSER_FLAMES, bhvBowserBombExplosion);
        create_sound_spawner(SOUND_GENERAL_BOWSER_BOMB_EXPLOSION);
        set_camera_shake_from_point(SHAKE_POS_LARGE, o->oPosX, o->oPosY, o->oPosZ);
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        return;
    }

    if (obj_check_if_collided_with_object(o, gMarioObject) == TRUE) {
        // Damage was handled by the mine's normal Mario interaction. The child
        // supplies effects only, so it cannot deal a second explosion hit.
        struct Object *explosion = spawn_object(o, MODEL_EXPLOSION, bhvExplosion);
        if (explosion != NULL) {
            explosion->oBehParams2ndByte = game_options_get(GO_BOWSER_FIGHTS) == 0 ? 1 : 0;
        }
        o->oInteractStatus = 0;
        if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
            o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
            return;
        }
        o->oAction = BOWSER_BOMB_ACT_RESPAWNING;
        cur_obj_hide();
        cur_obj_become_intangible();
        return;
    }

    set_object_visibility(o, 7000);
}

void bhv_bowser_bomb_explosion_loop(void) {
    cur_obj_scale((f32) o->oTimer / 14.0f * 9.0f + 1.0f);

    if ((o->oTimer % 4 == 0) && (o->oTimer < 20)) {
        struct Object *mineSmoke = spawn_object(o, MODEL_BOWSER_SMOKE, bhvBowserBombSmoke);
        mineSmoke->oPosX += random_float() * 600.0f - 400.0f;
        mineSmoke->oPosZ += random_float() * 600.0f - 400.0f;
        mineSmoke->oVelY += random_float() * 10.0f;
    }

    if (o->oTimer % 2 == 0) {
        o->oAnimState++;
    }

    if (o->oTimer == 28) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}

void bhv_bowser_bomb_smoke_loop(void) {
    cur_obj_scale((f32) o->oTimer / 14.0f * 9.0 + 1.0);

    if (o->oTimer % 2 == 0) {
        o->oAnimState++;
    }

    o->oOpacity -= 10;

    if (o->oOpacity < 10) {
        o->oOpacity = 0;
    }

    o->oPosY += o->oVelY;

    if (o->oTimer == 28) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}
