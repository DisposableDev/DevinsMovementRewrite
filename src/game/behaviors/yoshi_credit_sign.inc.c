#define YOSHI_CREDIT_SIGN_FADE_STEP 8
// Zero-based file awaiting exit effects; -1 means there is no pending visit.
static s32 sYoshiCreditSignPendingExitFileIndex = -1;

void yoshi_credit_sign_track_visit(void) {
    if (save_file_get_castle_secret(gCurrSaveFileNum - 1) & CASTLE_SECRET_MOVED) {
        sYoshiCreditSignPendingExitFileIndex = gCurrSaveFileNum - 1;
    }
}

void yoshi_credit_sign_area_exit(void) {
    // Both area unload and save reload call here; consume the visit only once.
    if (sYoshiCreditSignPendingExitFileIndex >= 0) {
        save_file_unlock_castle_wing_cap(sYoshiCreditSignPendingExitFileIndex);
        if (save_file_get_castle_secret(sYoshiCreditSignPendingExitFileIndex)
            & CASTLE_SECRET_INTRO_READ) {
            save_file_set_castle_secret(sYoshiCreditSignPendingExitFileIndex, CASTLE_SECRET_AWAY);
        }
        sYoshiCreditSignPendingExitFileIndex = -1;
    }
}

// Complete the sign's attack: release its time stop before entering death and warping.
static void yoshi_credit_sign_finish_crush(void) {
    cur_obj_play_sound_2(SOUND_OBJ_WHOMP);
    cur_obj_shake_screen(SHAKE_POS_SMALL);
    mario_stop_riding_and_holding(gMarioState);
    mario_extinguish_fire(gMarioState);
    player_stats_damage();
    player_stats_health(gMarioState->health, 0xFF);
    gMarioState->health = 0xFF;
    gMarioState->hurtCounter = 0;
    gMarioState->healCounter = 0;
    gMarioState->respawnHealCounter = 0;
    gMarioState->yoshiCoinDeathPending = FALSE;
    gMarioState->poisonMushroomDeathPending = FALSE;
    gMarioState->numLives = 0;
    gMarioState->squishTimer = 0xFF;
    vec3f_set(gMarioState->marioObj->header.gfx.scale, 1.8f, 0.05f, 1.8f);
    disable_time_stop_including_mario();
    set_mario_action(gMarioState, ACT_STANDING_DEATH, 0);
    save_file_set_castle_secret(gCurrSaveFileNum - 1, CASTLE_SECRET_MOVED);
    save_file_unlock_castle_wing_cap(gCurrSaveFileNum - 1);
    cur_obj_hide();
    obj_mark_for_deletion(o);
    queue_rumble_data(10, 80);
    // This cake warp bypasses the ordinary death-warp accounting.
    player_stats_death(TRUE, FALSE);
    level_warp_to_castle_board_cake();
    o->oAction = YOSHI_CREDIT_SIGN_ACT_CRUSHED;
}

void bhv_yoshi_credit_sign_loop(void) {
    if (game_options_secrets_blocked() && o->oAction == YOSHI_CREDIT_SIGN_ACT_READ
        && gMarioState->action != ACT_READING_SIGN) {
        o->oInteractType = 0;
        cur_obj_become_intangible();
        return;
    }
    switch (o->oAction) {
        case YOSHI_CREDIT_SIGN_ACT_READ:
            o->oBehParams2ndByte =
                castle_secret_window_is_broken() ? DIALOG_CASTLE_BOARD_INTRUDER : DIALOG_YOSHI_CREDIT;
            bhv_message_panel_loop();
            break;

        case YOSHI_CREDIT_SIGN_ACT_SPAWN_MUSHROOM:
            cur_obj_become_intangible();
            if (o->oBehParams2ndByte == DIALOG_CASTLE_BOARD_INTRUDER) {
                enable_time_stop_including_mario();
                o->oAction = YOSHI_CREDIT_SIGN_ACT_CRUSH;
                break;
            }
            if (try_to_spawn_object(100, 1.0f, o, MODEL_POISON_MUSHROOM, bhvPoisonMushroom) != NULL) {
                cur_obj_play_sound_2(SOUND_MENU_MARIO_CASTLE_WARP);
                o->oAction = YOSHI_CREDIT_SIGN_ACT_FADE;
            }
            break;

        case YOSHI_CREDIT_SIGN_ACT_CRUSH:
            cur_obj_become_intangible();
            o->oInteractType = 0;
            if (o->oTimer == 0) {
                o->oVelY = 40.0f;
                o->oYoshiCreditAttackHeight = 0.0f;
                o->oFaceAnglePitch = 0;
                o->oAngleVelPitch = 0;
                cur_obj_scale(3.0f);
            }
            // Height is relative to Mario; oTimer counts updates from zero on state entry.
            // Whomp's jump and accelerating quarter-turn, aimed at Mario throughout.
            if (o->oTimer >= 8) {
                o->oAngleVelPitch += 0x100;
                o->oFaceAnglePitch = MIN(0x4000, o->oFaceAnglePitch + o->oAngleVelPitch);
            }
            o->oVelY -= 4.0f;
            o->oYoshiCreditAttackHeight += o->oVelY;
            o->oPosX = gMarioState->pos[0];
            o->oPosZ = gMarioState->pos[2];
            o->oPosY = gMarioState->pos[1] + MAX(0.0f, o->oYoshiCreditAttackHeight);
            // The timeout also kills if the expected landing is missed or interrupted.
            if ((o->oTimer >= 8 && o->oYoshiCreditAttackHeight <= 0.0f && o->oFaceAnglePitch == 0x4000)
                || o->oTimer >= 60) {
                yoshi_credit_sign_finish_crush();
            }
            break;

        case YOSHI_CREDIT_SIGN_ACT_CRUSHED:
            break;

        case YOSHI_CREDIT_SIGN_ACT_FADE:
            o->oOpacity = MAX(0, o->oOpacity - YOSHI_CREDIT_SIGN_FADE_STEP);
            if (o->oOpacity == 0) {
                sYoshiCreditSignPendingExitFileIndex = gCurrSaveFileNum - 1;
                save_file_set_castle_secret(sYoshiCreditSignPendingExitFileIndex, CASTLE_SECRET_MOVED);
                obj_mark_for_deletion(o);
            }
            break;
    }
}
