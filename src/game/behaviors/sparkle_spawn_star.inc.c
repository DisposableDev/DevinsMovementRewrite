// sparkle_spawn_star.inc.c

static struct ObjectHitbox sSpawnedStarHitbox = {
    /* interactType:      */ INTERACT_STAR_OR_KEY,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 0,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 80,
    /* height:            */ 50,
    /* hurtboxRadius:     */ 0,
    /* hurtboxHeight:     */ 0,
};

void bhv_spawned_star_init(void) {
    // No-exit callers already supplied the star ID; ordinary spawned stars inherit the parent's bytes.
    if (!(o->oInteractionSubtype & INT_SUBTYPE_NO_EXIT)) {
        o->oBehParams = o->parentObj->oBehParams;
    }
    u8 starId = GET_BPARAM1(o->oBehParams);
#ifdef GLOBAL_STAR_IDS
    if ((1 << (starId % 7))
        & save_file_get_star_flags((gCurrSaveFileNum - 1), COURSE_NUM_TO_INDEX(starId / 7))) {
#else
    if ((1 << starId)
        & save_file_get_star_flags((gCurrSaveFileNum - 1), COURSE_NUM_TO_INDEX(gCurrCourseNum))) {
#endif
        cur_obj_set_model(MODEL_TRANSPARENT_STAR);
    }

    cur_obj_play_sound_2(SOUND_GENERAL2_STAR_APPEARS);
}

// interact_star_or_key owns awards/save updates; this behavior enables pickup and consumes the result.
static void spawned_star_update_collection(void) {
    obj_set_hitbox(o, &sSpawnedStarHitbox);
    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        obj_mark_for_deletion(o);
        o->oInteractStatus = INT_STATUS_NONE;
    }
}

static void spawned_star_prepare_flight_to_mario(void) {
    vec3f_copy_y_off(&o->oHomeVec, &gMarioObject->oPosVec, 250.0f);

    // Check that the star isn't clipping inside the ceiling
    if (gMarioState->ceil != NULL) {
        if (o->oHomeY > (gMarioState->ceilHeight - 50)) {
            o->oHomeY = gMarioState->ceilHeight - 50;
        }
    }

    // Start at target height; the launch arc below is relative to that height.
    o->oPosY = o->oHomeY;
    f32 lateralDist;
    vec3f_get_lateral_dist(&o->oPosVec, &o->oHomeVec, &lateralDist);

    o->oForwardVel = lateralDist / 23.0f;
}

static void spawned_star_keep_spawn_height(void) {
    // Keep horizontal motion stopped while launching vertically over the spawn position.
    o->oForwardVel = 0.0f;
    o->oHomeY = o->oPosY;
}

static void spawned_star_slow_rotation(void) {
    if (o->oAngleVelYaw > 0x400) {
        o->oAngleVelYaw -= 0x40;
    }
}

void bhv_spawned_star_loop(void) {
    if (o->oAction == SPAWN_STAR_POS_CUTSCENE_ACT_START) {
        if (o->oTimer == 0) {
            cutscene_object(CUTSCENE_STAR_SPAWN, o);
            set_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
            o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
            o->oAngleVelYaw = 0x800;
            if (o->oBehParams2ndByte == SPAWN_STAR_POS_CUTSCENE_BP_SPAWN_AT_MARIO) {
                spawned_star_prepare_flight_to_mario();
            } else {
                spawned_star_keep_spawn_height();
            }
            o->oMoveAngleYaw = cur_obj_angle_to_home();
            o->oVelY = 50.0f;
            o->oGravity = -4.0f;
            spawn_mist_particles();
        }
        cur_obj_play_sound_1(SOUND_ENV_STAR);
        try_spawn_particle(o, MODEL_NONE, bhvSparkleSpawn, PARTICLE_PRIORITY_FEEDBACK);
        if (o->oVelY < 0 && o->oPosY < o->oHomeY) {
            o->oAction++; // SPAWN_STAR_POS_CUTSCENE_ACT_BOUNCE
            o->oForwardVel = 0;
            o->oVelY = 20.0f;
            o->oGravity = -1.0f;
            play_power_star_jingle();
        }
    } else if (o->oAction == SPAWN_STAR_POS_CUTSCENE_ACT_BOUNCE) {
        if (o->oVelY < -4.0f) {
            o->oVelY = -4.0f;
        }
        if (o->oVelY < 0 && o->oPosY < o->oHomeY) {
            gObjCutsceneDone = TRUE;
            o->oVelY = 0;
            o->oGravity = 0;
            o->oAction++; // SPAWN_STAR_POS_CUTSCENE_ACT_END
        }
        try_spawn_particle(o, MODEL_NONE, bhvSparkleSpawn, PARTICLE_PRIORITY_FEEDBACK);
    } else if (o->oAction == SPAWN_STAR_POS_CUTSCENE_ACT_END) {
        // Motion completion signals the camera; release time stop only after both camera markers clear.
        if (gCamera->cutscene == 0 && gRecentCutscene == 0) {
            clear_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
            o->activeFlags &= ~ACTIVE_FLAG_INITIATED_TIME_STOP;
            o->oAction++; // SPAWN_STAR_POS_CUTSCENE_ACT_SLOW_STAR_ROTATION
        }
    } else { // SPAWN_STAR_POS_CUTSCENE_ACT_SLOW_STAR_ROTATION
        spawned_star_update_collection();
        spawned_star_slow_rotation();
    }

    cur_obj_move_using_fvel_and_gravity();
    o->oFaceAngleYaw += o->oAngleVelYaw;
    o->oInteractStatus = INT_STATUS_NONE;
}

void bhv_spawn_star_no_level_exit(u32 starId) {
    struct Object *starObj = spawn_object(o, MODEL_STAR, bhvSpawnedStarNoLevelExit);
    SET_BPARAM1(starObj->oBehParams, starId);
    starObj->oInteractionSubtype = INT_SUBTYPE_NO_EXIT;
    obj_set_angle(starObj, 0x0, 0x0, 0x0);
}
