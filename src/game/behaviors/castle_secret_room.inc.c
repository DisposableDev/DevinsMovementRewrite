#define CASTLE_SECRET_SIGN_FADE_STEP 8
// Per-file runtime state: survives area reloads, clears on save reload or initialization.
static u32 sCompletionSignDismissedFileMask;

void castle_secret_sign_reset(s32 fileIndex) {
    if (fileIndex >= 0 && fileIndex < NUM_SAVE_FILES) {
        sCompletionSignDismissedFileMask &= ~(1U << fileIndex);
    }
}

enum CastleSecretPortraitState {
    CASTLE_SECRET_PORTRAIT_SMUG,
    CASTLE_SECRET_PORTRAIT_PHYSICS_LAB,
    CASTLE_SECRET_PORTRAIT_BOW,
    CASTLE_SECRET_PORTRAIT_ABSENT,
};

enum CastleSecretRoomSpawnState {
    CASTLE_SECRET_ROOM_NOT_SPAWNED,
    CASTLE_SECRET_ROOM_SPAWNED,
    CASTLE_SECRET_ROOM_COLLISION_READY,
};

enum CastleSecretWindowState {
    CASTLE_SECRET_WINDOW_INTACT = 0,
    CASTLE_SECRET_WINDOW_BREAK_REQUESTED = 1,
    CASTLE_SECRET_WINDOW_BROKEN = 2,
};

static s32 castle_secret_room_has_full_stars(void) {
    return !game_options_secrets_blocked()
           && save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(COURSE_MIN),
                                             COURSE_NUM_TO_INDEX(COURSE_MAX))
                  >= CASTLE_ROOF_COMPLETION_STARS;
}

static s32 castle_secret_room_portrait_state(void) {
    if (game_options_secrets_blocked()) {
        return CASTLE_SECRET_PORTRAIT_ABSENT;
    }
    if (castle_secret_room_has_full_stars()) {
        return CASTLE_SECRET_PORTRAIT_PHYSICS_LAB;
    }
    u32 progressFlags = save_file_get_castle_secret(gCurrSaveFileNum - 1);
    if (!(progressFlags & CASTLE_SECRET_MOVED)) {
        return CASTLE_SECRET_PORTRAIT_BOW;
    }
    if (progressFlags & CASTLE_SECRET_AWAY) {
        return CASTLE_SECRET_PORTRAIT_ABSENT;
    }
    return CASTLE_SECRET_PORTRAIT_SMUG;
}

s32 castle_secret_window_is_broken(void) {
    struct Object *window = cur_obj_nearest_object_with_behavior(bhvCastleSecretWindow);
    return window != NULL && window->oAction == CASTLE_SECRET_WINDOW_BROKEN;
}

// The room is created only after its entrance has been broken open.
void castle_secret_window_bonk(struct MarioState *m) {
    if (!game_options_secrets_blocked() && m->wall != NULL && m->wall->object != NULL
        && obj_has_behavior(m->wall->object, bhvCastleSecretWindow)
        && m->wall->object->oAction == CASTLE_SECRET_WINDOW_INTACT) {
        m->wall->object->oAction = CASTLE_SECRET_WINDOW_BREAK_REQUESTED;
    }
}

void bhv_castle_secret_window_loop(void) {
    u32 progressFlags = save_file_get_castle_secret(gCurrSaveFileNum - 1);
    if (progressFlags & CASTLE_SECRET_MOVED) {
        yoshi_credit_sign_track_visit();
    }
    if (o->oAction == CASTLE_SECRET_WINDOW_BREAK_REQUESTED) {
        // Keep the glass solid until the replacement room can be created.
        if (!o->oCastleSecretChildSpawned) {
            if (try_to_spawn_object(0, 1.0f, o, MODEL_LEVEL_GEOMETRY_05, bhvCastleSecretRoom) == NULL) {
                load_object_collision_model();
                return;
            }
            o->oCastleSecretChildSpawned = CASTLE_SECRET_ROOM_SPAWNED;
        }
        // The child publishes its floor before the glass releases Mario.
        if (o->oCastleSecretChildSpawned != CASTLE_SECRET_ROOM_COLLISION_READY) {
            load_object_collision_model();
            return;
        }
        cur_obj_play_sound_2(SOUND_GENERAL_BREAK_BOX);
        spawn_triangle_break_particles(12, MODEL_DIRT_ANIMATION, 1.0f, 0);
        o->oAction = CASTLE_SECRET_WINDOW_BROKEN;
    }
    if (o->oAction == CASTLE_SECRET_WINDOW_BROKEN) {
        cur_obj_hide();
        // Commit only a successful spawn; allocation failure retries next update.
        if (!o->oCastleSecretChildSpawned
            && try_to_spawn_object(0, 1.0f, o, MODEL_LEVEL_GEOMETRY_05, bhvCastleSecretRoom) != NULL) {
            o->oCastleSecretChildSpawned = TRUE;
        }
    } else {
        load_object_collision_model();
    }
}

void bhv_castle_secret_room_loop(void) {
    o->oAnimState = castle_secret_room_portrait_state();
    if (!o->oCastleSecretChildSpawned
        && !(sCompletionSignDismissedFileMask & (1U << (gCurrSaveFileNum - 1)))
        && (castle_secret_room_has_full_stars()
            || (save_file_get_castle_secret(gCurrSaveFileNum - 1) & CASTLE_SECRET_MOVED))) {
        struct Object *sign =
            try_to_spawn_object(0, 1.0f, o, MODEL_YOSHI_CREDIT_SIGN, bhvCastleSecretSign);
        if (sign != NULL) {
            sign->oPosZ -= 380.0f;
            sign->oFaceAngleYaw = 0;
            sign->oMoveAngleYaw = 0;
            o->oCastleSecretChildSpawned = TRUE;
        }
    }
    o->collisionData =
        segmented_to_virtual(castle_secret_room_has_full_stars() ? castle_secret_painting_room_collision
                                                                 : castle_secret_room_collision);
    load_object_collision_model();
    if (o->parentObj != NULL && obj_has_behavior(o->parentObj, bhvCastleSecretWindow)) {
        o->parentObj->oCastleSecretChildSpawned = CASTLE_SECRET_ROOM_COLLISION_READY;
    }
}

void bhv_castle_secret_sign_loop(void) {
    if (o->oAction == 0 && game_options_secrets_blocked()) {
        o->oInteractType = 0;
        cur_obj_become_intangible();
        return;
    }
    if (o->oAction == 1 && o->oBehParams2ndByte == DIALOG_CASTLE_SECRET_COMPLETE) {
        sCompletionSignDismissedFileMask |= 1U << (gCurrSaveFileNum - 1);
        o->oAction = 2;
    }
    if (o->oAction == 2) {
        o->oInteractType = 0;
        cur_obj_become_intangible();
        o->oOpacity = MAX(0, o->oOpacity - CASTLE_SECRET_SIGN_FADE_STEP);
        if (o->oOpacity == 0) {
            obj_mark_for_deletion(o);
        }
        return;
    }
    s32 fullStarCompletion = castle_secret_room_has_full_stars();
    u32 progressFlags = save_file_get_castle_secret(gCurrSaveFileNum - 1);
    if (o->oAction == 1 && !fullStarCompletion && !(progressFlags & CASTLE_SECRET_AWAY)) {
        u32 readFlag = CASTLE_SECRET_INTRO_READ;
        if (o->oBehParams2ndByte == DIALOG_CASTLE_SECRET_OW) {
            readFlag = CASTLE_SECRET_OW_READ;
        } else if (o->oBehParams2ndByte == DIALOG_CASTLE_SECRET_WHY) {
            readFlag = CASTLE_SECRET_WHY_READ;
        }
        save_file_set_castle_secret(gCurrSaveFileNum - 1, readFlag);
        progressFlags |= readFlag;
    }
    o->oAction = 0;

    s32 pounding = cur_obj_is_mario_ground_pounding_platform();
    if (!fullStarCompletion && !(progressFlags & CASTLE_SECRET_AWAY) && pounding
        && !o->oSignpostMarioPounding) {
        if ((progressFlags & CASTLE_SECRET_INTRO_READ)
            && !(progressFlags & CASTLE_SECRET_POUNDED_ONCE)) {
            save_file_set_castle_secret(gCurrSaveFileNum - 1, CASTLE_SECRET_POUNDED_ONCE);
            progressFlags |= CASTLE_SECRET_POUNDED_ONCE;
        } else if ((progressFlags & CASTLE_SECRET_OW_READ)
                   && !(progressFlags & CASTLE_SECRET_POUNDED_TWICE)) {
            save_file_set_castle_secret(gCurrSaveFileNum - 1, CASTLE_SECRET_POUNDED_TWICE);
            progressFlags |= CASTLE_SECRET_POUNDED_TWICE;
        }
    }

    s32 dialogId = -1;
    if (fullStarCompletion) {
        dialogId = DIALOG_CASTLE_SECRET_COMPLETE;
    } else if (progressFlags & CASTLE_SECRET_AWAY) {
        dialogId = DIALOG_CASTLE_SECRET_AWAY;
    } else if ((progressFlags & CASTLE_SECRET_POUNDED_TWICE)
               && !(progressFlags & CASTLE_SECRET_WHY_READ)) {
        dialogId = DIALOG_CASTLE_SECRET_WHY;
    } else if ((progressFlags & CASTLE_SECRET_POUNDED_ONCE)
               && !(progressFlags & CASTLE_SECRET_OW_READ)) {
        dialogId = DIALOG_CASTLE_SECRET_OW;
    } else if (!(progressFlags & CASTLE_SECRET_INTRO_READ)) {
        dialogId = DIALOG_CASTLE_SECRET_HOME;
    }
    // Apply ordinary post sinking before restoring this sign's dialogue availability.
    bhv_message_panel_loop();
    o->oInteractType = dialogId >= 0 ? INTERACT_TEXT : 0;
    o->oIntangibleTimer = dialogId >= 0 ? 0 : -1;
    if (dialogId >= 0) {
        o->oBehParams2ndByte = dialogId;
    }
}
