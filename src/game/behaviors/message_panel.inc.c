#define SIGNPOST_POUND_DEPTH 30.0f
#define SIGNPOST_MAX_SINK_DEPTH (2 * SIGNPOST_POUND_DEPTH)
#define SIGNPOST_SINK_SPEED 6.0f
#define CASTLE_LOBBY_SIGN_REVEAL_DISTANCE 300.0f
#define PHYSICS_LAB_SIGN_REVEAL_DISTANCE 150.0f

void bhv_castle_lobby_rejection_sign_init(void) {
    s32 dialog = castle_secret_lobby_rejection_dialog();
    if (dialog < 0) {
        obj_mark_for_deletion(o);
    } else {
        o->oBehParams2ndByte = dialog;
    }
}

void bhv_castle_lobby_floor_sign_loop(void) {
    if (castle_secret_lobby_rejection_dialog() >= 0) {
        obj_mark_for_deletion(o);
        return;
    }
    // act_reading_sign sets action 1 after the dialogue finishes.
    if (o->oAction == 1) {
        save_file_set_castle_secret(gCurrSaveFileNum - 1, CASTLE_SECRET_LOBBY_SIGN_READ);
    }
    if (save_file_get_castle_secret(gCurrSaveFileNum - 1) & CASTLE_SECRET_LOBBY_SIGN_READ) {
        o->oInteractType = 0;
        cur_obj_become_intangible();
        cur_obj_hide();
        obj_mark_for_deletion(o);
        return;
    }
    if (o->oDistanceToMario > CASTLE_LOBBY_SIGN_REVEAL_DISTANCE) {
        o->oInteractType = 0;
        cur_obj_become_intangible();
        cur_obj_hide();
        return;
    }
    cur_obj_unhide();
    o->oInteractType = INTERACT_TEXT;
    bhv_message_panel_loop();
}

void bhv_message_panel_loop(void) {
    s32 pounding = cur_obj_is_mario_ground_pounding_platform();
    // Latch contact so one landing adds only one sinking stage and impact sound.
    if (pounding && !o->oSignpostMarioPounding
        && o->oSignpostTargetSinkDepth < SIGNPOST_MAX_SINK_DEPTH) {
        o->oSignpostTargetSinkDepth += SIGNPOST_POUND_DEPTH;
        cur_obj_play_sound_2(SOUND_GENERAL_POUND_WOOD_POST);
    }
    o->oSignpostMarioPounding = pounding;

    f32 descent = MIN(SIGNPOST_SINK_SPEED, o->oSignpostTargetSinkDepth - o->oSignpostSinkDepth);
    o->oSignpostSinkDepth += descent;
    o->oPosY -= descent;
    // Keep the reading volume's top at its original height as the post sinks.
    o->hitboxHeight = 80.0f + o->oSignpostSinkDepth;
    o->oIntangibleTimer = 0;
    o->oInteractStatus = INT_STATUS_NONE;
    load_object_collision_model();
}

void bhv_physics_lab_hidden_sign_loop(void) {
    // Visibility gates text interaction; sinking and solid collision stay active.
    bhv_message_panel_loop();
    if (o->oDistanceToMario <= PHYSICS_LAB_SIGN_REVEAL_DISTANCE) {
        cur_obj_unhide();
        o->oInteractType = INTERACT_TEXT;
    } else {
        cur_obj_hide();
        o->oInteractType = 0;
    }
}

void bhv_physics_lab_welcome_sign_init(void) {
#if defined(UNLOCK_ALL) || defined(COMPLETE_SAVE_FILE)
    s32 completed = FALSE;
#else
    s32 completed =
        !game_options_secrets_blocked()
        && save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(COURSE_MIN),
                                          COURSE_NUM_TO_INDEX(COURSE_MAX))
               >= CASTLE_ROOF_COMPLETION_STARS;
#endif
    o->oBehParams2ndByte =
        completed ? DIALOG_PHYSICS_LAB_WELCOME_COMPLETE : DIALOG_PHYSICS_LAB_WELCOME_EARLY;
}
