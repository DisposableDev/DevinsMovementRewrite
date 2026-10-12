// yoshi.inc.c

#define YOSHI_PRESENT_PAUSE_UPDATES 45

// X/Z coordinates of Yoshi's homes that he switches between.
// Note that this doesn't contain the Y coordinate since the castle roof is flat,
// so o->oHomeY is never updated.
static s16 sYoshiHomeLocations[] = { 0, -5625, -1364, -5912, -1403, -4609, -1004, -5308 };

void bhv_yoshi_init(void) {
    if (game_options_secrets_blocked() && gCurrCreditsEntry == NULL) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        return;
    }
    o->oGravity = 2.0f;
    o->oFriction = 0.9f;
    o->oBuoyancy = 1.3f;
    o->oInteractionSubtype = INT_SUBTYPE_NPC;

#if !defined(UNLOCK_ALL) && !defined(COMPLETE_SAVE_FILE)
    if (gCurrCreditsEntry == NULL && !gDebugLevelSelect
        && save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(COURSE_MIN),
                                          COURSE_NUM_TO_INDEX(COURSE_MAX))
               < CASTLE_ROOF_COMPLETION_STARS) {
        if (!(save_file_get_castle_secret(gCurrSaveFileNum - 1) & CASTLE_SECRET_MOVED)) {
            spawn_object_relative(DIALOG_YOSHI_CREDIT, 0, 0, 0, o, MODEL_YOSHI_CREDIT_SIGN,
                                  bhvYoshiCreditSign);
        }
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        return;
    }
#endif
    if (sYoshiDead == TRUE) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}

void yoshi_walk_loop(void) {
    s16 animFrame = o->header.gfx.animInfo.animFrame;

    o->oForwardVel = 10.0f;
    object_step();
    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oYoshiTargetYaw, 0x500);

    if (is_point_close_to_object(o, o->oHomeX, 3174.0f, o->oHomeZ, 200)) {
        o->oAction = YOSHI_ACT_IDLE;
    }

    cur_obj_init_animation(1);

    if ((animFrame == 0 || animFrame == 15) && dist_between_objects(o, gMarioObject) <= 1000.0f) {
        cur_obj_play_sound_2(SOUND_GENERAL_YOSHI_WALK);
    }

    if (o->oInteractStatus == INT_STATUS_INTERACTED) {
        o->oAction = YOSHI_ACT_TALK;
    }

    if (o->oPosY < 2100.0f) {
        create_respawner(MODEL_YOSHI, bhvYoshi, 3000);
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}

void yoshi_idle_loop(void) {
    if (o->oTimer > 90) {
        s16 chosenHome = random_float() * 3.99f;

        if (o->oYoshiChosenHome == chosenHome) {
            return;
        } else {
            o->oYoshiChosenHome = chosenHome;
        }

        o->oHomeX = sYoshiHomeLocations[o->oYoshiChosenHome * 2];
        o->oHomeZ = sYoshiHomeLocations[o->oYoshiChosenHome * 2 + 1];
        o->oYoshiTargetYaw = atan2s(o->oHomeZ - o->oPosZ, o->oHomeX - o->oPosX);
        o->oAction = YOSHI_ACT_WALK;
    }

    cur_obj_init_animation(0);

    if (o->oInteractStatus == INT_STATUS_INTERACTED) {
        o->oAction = YOSHI_ACT_TALK;
    }

    // Credits; Yoshi appears at this position overlooking the castle near the end of the credits
    if (gPlayerCameraState->cameraEvent == CAM_EVENT_START_ENDING ||
        gPlayerCameraState->cameraEvent == CAM_EVENT_START_END_WAVING) {
        o->oAction = YOSHI_ACT_CREDITS;
        o->oPosX = -1798.0f;
        o->oPosY = 3174.0f;
        o->oPosZ = -3644.0f;
    }
}

void yoshi_talk_loop(void) {
    if (o->oTimer == 0) {
        play_puzzle_jingle();
    }

    if ((s16) o->oMoveAngleYaw == (s16) o->oAngleToMario) {
        cur_obj_init_animation(0);
        if (set_mario_npc_dialog(MARIO_DIALOG_LOOK_FRONT) == MARIO_DIALOG_STATUS_SPEAK) {
            o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
            if (cutscene_object_with_dialog(CUTSCENE_DIALOG, o, DIALOG_161) != 0) {
                o->activeFlags &= ~ACTIVE_FLAG_INITIATED_TIME_STOP;
                o->oInteractStatus = INT_STATUS_NONE;
                o->oHomeX = sYoshiHomeLocations[2];
                o->oHomeZ = sYoshiHomeLocations[3];
                o->oYoshiTargetYaw = atan2s(o->oHomeZ - o->oPosZ, o->oHomeX - o->oPosX);
                o->oAction = YOSHI_ACT_GIVE_PRESENT;
            }
        }
    } else {
        cur_obj_init_animation(1);
        o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x500);
    }
}

void yoshi_walk_and_jump_off_roof_loop(void) {
    s16 animFrame = o->header.gfx.animInfo.animFrame;

    o->oForwardVel = 10.0f;
    object_step();
    cur_obj_init_animation(1);

    if (o->oTimer == 0) {
        cutscene_object(CUTSCENE_STAR_SPAWN, o);
    }

    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oYoshiTargetYaw, 0x500);

    if (is_point_close_to_object(o, o->oHomeX, 3174.0f, o->oHomeZ, 200)) {
        cur_obj_init_animation(2);
        cur_obj_play_sound_2(SOUND_GENERAL_ENEMY_ALERT1);
        o->oForwardVel = 50.0f;
        o->oVelY = 40.0f;
        o->oMoveAngleYaw = -0x3FFF;
        o->oAction = YOSHI_ACT_FINISH_JUMPING_AND_DESPAWN;
    }

    if ((animFrame == 0 || animFrame == 15) && dist_between_objects(o, gMarioObject) <= 1000.0f) {
        cur_obj_play_sound_2(SOUND_GENERAL_YOSHI_WALK);
    }
}

void yoshi_finish_jumping_and_despawn_loop(void) {
    cur_obj_extend_animation_if_at_end();
    obj_move_xyz_using_fvel_and_yaw(o);

    o->oVelY -= 2.0f;

    if (o->oPosY < 2100.0f) {
        set_mario_npc_dialog(MARIO_DIALOG_STOP);
        gObjCutsceneDone = TRUE;
        sYoshiDead = TRUE;
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}

void yoshi_give_present_loop(void) {
    cur_obj_init_animation(YOSHI_ANIM_IDLE);
    o->oForwardVel = 0.0f;

    if (o->oTimer == 0 && !game_options_secrets_blocked()) {
        struct Object *coin =
            spawn_object_relative(0, 0, 120, 0, o, MODEL_YELLOW_COIN, bhvSingleCoinGetsSpawned);

        if (coin != NULL) {
            // Keep the aimed throw when the bouncing coin initializes next update.
            coin->oCoinSpawnConfigured = TRUE;
            coin->oInteractionSubtype |= INT_SUBTYPE_YOSHI_GIFT_COIN;
            coin->oMoveAngleYaw = obj_angle_to_object(o, gMarioObject);
            coin->oForwardVel = dist_between_objects(o, gMarioObject) / 20.0f;
            coin->oVelY = 30.0f;
        }
    }

    if (o->oTimer >= YOSHI_PRESENT_PAUSE_UPDATES) {
        o->oAction = YOSHI_ACT_WALK_JUMP_OFF_ROOF;
    }
}

void bhv_yoshi_loop(void) {
    // Finish an existing conversation, but do not start another while cheating.
    if (game_options_secrets_blocked() && gCurrCreditsEntry == NULL
        && (o->oAction == YOSHI_ACT_IDLE || o->oAction == YOSHI_ACT_WALK)) {
        cur_obj_hide();
        cur_obj_become_intangible();
        o->oInteractStatus = INT_STATUS_NONE;
        return;
    }
    if (o->oAction == YOSHI_ACT_IDLE || o->oAction == YOSHI_ACT_WALK) {
        cur_obj_unhide();
        cur_obj_become_tangible();
    }
    switch (o->oAction) {
        case YOSHI_ACT_IDLE:
            yoshi_idle_loop();
            break;

        case YOSHI_ACT_WALK:
            yoshi_walk_loop();
            break;

        case YOSHI_ACT_TALK:
            yoshi_talk_loop();
            break;

        case YOSHI_ACT_WALK_JUMP_OFF_ROOF:
            yoshi_walk_and_jump_off_roof_loop();
            break;

        case YOSHI_ACT_FINISH_JUMPING_AND_DESPAWN:
            yoshi_finish_jumping_and_despawn_loop();
            break;

        case YOSHI_ACT_GIVE_PRESENT:
            yoshi_give_present_loop();
            break;

        case YOSHI_ACT_CREDITS:
            cur_obj_init_animation(0);
            break;
    }

    curr_obj_random_blink(&o->oYoshiBlinkTimer);
}
