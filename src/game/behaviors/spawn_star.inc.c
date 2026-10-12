// spawn_star.inc.c

static struct ObjectHitbox sCollectStarHitbox = {
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

void bhv_collect_star_init(void) {
    // Saved appearance is per course, derived from the star ID in global-ID builds.
    // Collected stars retain the same hitbox; transparency alone does not prevent pickup.
    u8 starId = GET_BPARAM1(o->oBehParams);
#ifdef GLOBAL_STAR_IDS
    u8 courseStarFlags =
        save_file_get_star_flags((gCurrSaveFileNum - 1), COURSE_NUM_TO_INDEX(starId / 7));
    if (courseStarFlags & (1 << (starId % 7))) {
#else
    u8 courseStarFlags =
        save_file_get_star_flags((gCurrSaveFileNum - 1), COURSE_NUM_TO_INDEX(gCurrCourseNum));
    if (courseStarFlags & (1 << starId)) {
#endif
        o->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_TRANSPARENT_STAR];
    } else {
        o->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_STAR];
    }

    obj_set_hitbox(o, &sCollectStarHitbox);
}

void bhv_collect_star_loop(void) {
    o->oFaceAngleYaw += 0x800;

    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        obj_mark_for_deletion(o);
        o->oInteractStatus = INT_STATUS_NONE;
    }
}

void bhv_star_spawn_init(void) {
    s16 yaw;
    vec3f_get_yaw(&o->oPosVec, &o->oHomeVec, &yaw);
    vec3f_get_lateral_dist(&o->oPosVec, &o->oHomeVec, &o->oStarSpawnLateralDistance);
    o->oMoveAngleYaw = yaw;
    o->oVelY = (o->oHomeY - o->oPosY) / 30.0f;
    o->oForwardVel = o->oStarSpawnLateralDistance / 30.0f;
    // Track the straight-line height separately from the sine arc added during travel.
    o->oStarSpawnBaseY = o->oPosY;

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if (o->oBehParams2ndByte == SPAWN_STAR_ARC_CUTSCENE_BP_DEFAULT_STAR || gCurrCourseNum == COURSE_BBH) {
#else
    if (o->oBehParams2ndByte == SPAWN_STAR_ARC_CUTSCENE_BP_DEFAULT_STAR) {
#endif
        cutscene_object(CUTSCENE_STAR_SPAWN, o);
    } else {
        cutscene_object(CUTSCENE_RED_COIN_STAR_SPAWN, o);
    }

    set_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
    o->activeFlags |= ACTIVE_FLAG_INITIATED_TIME_STOP;
    cur_obj_become_intangible();
}

void bhv_star_spawn_loop(void) {
    switch (o->oAction) {
        case SPAWN_STAR_ARC_CUTSCENE_ACT_START:
            o->oFaceAngleYaw += 0x1000;
            if (o->oTimer > 20) {
                o->oAction = SPAWN_STAR_ARC_CUTSCENE_ACT_GO_TO_HOME;
            }
            break;

        case SPAWN_STAR_ARC_CUTSCENE_ACT_GO_TO_HOME:
            obj_move_xyz_using_fvel_and_yaw(o);
            o->oStarSpawnBaseY += o->oVelY;
            o->oPosY = o->oStarSpawnBaseY + sins((o->oTimer * 0x8000) / 30) * 400.0f;
            o->oFaceAngleYaw += 0x1000;
            try_spawn_particle(o, MODEL_NONE, bhvSparkleSpawn, PARTICLE_PRIORITY_FEEDBACK);
            cur_obj_play_sound_1(SOUND_ENV_STAR);
            if (o->oTimer == 29) {
                o->oAction = SPAWN_STAR_ARC_CUTSCENE_ACT_BOUNCE;
                o->oForwardVel = 0;
                // Set to exact home coordinates
                vec3f_copy(&o->oPosVec, &o->oHomeVec);
                play_power_star_jingle();
            }
            break;

        case SPAWN_STAR_ARC_CUTSCENE_ACT_BOUNCE:
            if (o->oTimer < 20) {
                o->oVelY = 20 - o->oTimer;
            } else {
                o->oVelY = -10.0f;
            }

            try_spawn_particle(o, MODEL_NONE, bhvSparkleSpawn, PARTICLE_PRIORITY_FEEDBACK);
            obj_move_xyz_using_fvel_and_yaw(o);
            o->oFaceAngleYaw = o->oFaceAngleYaw - o->oTimer * 0x10 + 0x1000;
            cur_obj_play_sound_1(SOUND_ENV_STAR);

            if (o->oPosY < o->oHomeY) {
                cur_obj_play_sound_2(SOUND_GENERAL_STAR_APPEARS);
                // Enable pickup at landing; the END phase retains time stop until timer 20.
                cur_obj_become_tangible();
                o->oPosY = o->oHomeY;
                o->oAction = SPAWN_STAR_ARC_CUTSCENE_ACT_END;
            }
            break;

        case SPAWN_STAR_ARC_CUTSCENE_ACT_END:
            o->oFaceAngleYaw += 0x800;
            if (o->oTimer == 20) {
                gObjCutsceneDone = TRUE;
                clear_time_stop_flags(TIME_STOP_ENABLED | TIME_STOP_MARIO_AND_DOORS);
                o->activeFlags &= ~ACTIVE_FLAG_INITIATED_TIME_STOP;
            }

            if (o->oInteractStatus & INT_STATUS_INTERACTED) {
                obj_mark_for_deletion(o);
                o->oInteractStatus = INT_STATUS_NONE;
            }
            break;
    }
}

// Start at the current emitter and fly to these world-space home coordinates.
static struct Object *spawn_star_to_home(f32 homeX, f32 homeY, f32 homeZ) {
    struct Object *starObj = spawn_object_abs_with_rot(o, 0, MODEL_STAR, bhvStarSpawnCoordinates,
                                                       o->oPosX, o->oPosY, o->oPosZ, 0, 0, 0);
    starObj->oBehParams = o->oBehParams;
    vec3f_set(&starObj->oHomeVec, homeX, homeY, homeZ);
    starObj->oFaceAnglePitch = 0;
    starObj->oFaceAngleRoll = 0;
    return starObj;
}

void spawn_default_star(f32 x, f32 y, f32 z) {
    struct Object *starObj = spawn_star_to_home(x, y, z);
    starObj->oBehParams2ndByte = SPAWN_STAR_ARC_CUTSCENE_BP_DEFAULT_STAR;
}

void spawn_red_coin_cutscene_star(f32 x, f32 y, f32 z) {
    struct Object *starObj = spawn_star_to_home(x, y, z);
    starObj->oBehParams2ndByte = SPAWN_STAR_ARC_CUTSCENE_BP_HIDDEN_STAR;
    player_stats_tag_star(starObj, obj_has_behavior(o, bhvHiddenRedCoinStar)
                                       || obj_has_behavior(o, bhvBowserCourseRedCoinStar));
}

void spawn_no_exit_star(f32 x, f32 y, f32 z) {
    struct Object *starObj = spawn_star_to_home(x, y, z);
    starObj->oBehParams2ndByte = SPAWN_STAR_ARC_CUTSCENE_BP_HIDDEN_STAR;
    player_stats_tag_star(starObj, obj_has_behavior(o, bhvHiddenRedCoinStar)
                                       || obj_has_behavior(o, bhvBowserCourseRedCoinStar));
    starObj->oInteractionSubtype |= INT_SUBTYPE_NO_EXIT;
}

// A goal satisfied on entry uses an ordinary stationary star instead of a reveal cutscene.
static void hidden_red_coin_spawn_stationary_star(void) {
    struct Object *starObj =
        spawn_object_abs_with_rot(o, 0, MODEL_STAR, bhvStar, o->oPosX, o->oPosY, o->oPosZ, 0, 0, 0);
    starObj->oBehParams = o->oBehParams;
    player_stats_tag_star(starObj, TRUE);
    gRedCoinStarSpawned = TRUE;
    o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
}

void bhv_hidden_red_coin_star_init(void) {
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    if (gCurrCourseNum != COURSE_JRB) {
        spawn_object(o, MODEL_TRANSPARENT_STAR, bhvRedCoinStarMarker);
    }
#else
    spawn_object(o, MODEL_TRANSPARENT_STAR, bhvRedCoinStarMarker);
#endif

    // Nonzero byte 2 specifies the goal; zero derives it from listed coins plus prior progress.
    if (o->oBehParams2ndByte != 0) {
        o->oHiddenStarTriggerTotal = o->oBehParams2ndByte;
        o->oHiddenStarTriggerCounter = gRedCoinsCollected;
        if (o->oHiddenStarTriggerCounter >= o->oHiddenStarTriggerTotal) {
            hidden_red_coin_spawn_stationary_star();
        }
    }
    else {
        s16 numRedCoinsRemaining = count_objects_with_behavior(bhvRedCoin);
        if (numRedCoinsRemaining == 0) {
            hidden_red_coin_spawn_stationary_star();
        }
        o->oHiddenStarTriggerTotal = numRedCoinsRemaining + gRedCoinsCollected;
        o->oHiddenStarTriggerCounter = o->oHiddenStarTriggerTotal - numRedCoinsRemaining;
    }
    
}

void bhv_hidden_red_coin_star_loop(void) {
    // Coins increment this spawner's counter; publish it for the HUD and later initialization.
    gRedCoinsCollected = o->oHiddenStarTriggerCounter;

    switch (o->oAction) {
        case HIDDEN_STAR_ACT_INACTIVE:
            if (o->oHiddenStarTriggerCounter == o->oHiddenStarTriggerTotal) {
                o->oAction = HIDDEN_STAR_ACT_ACTIVE;
            }
            break;

        case HIDDEN_STAR_ACT_ACTIVE:
            if (o->oTimer > 2) {
                spawn_red_coin_cutscene_star(o->oPosX, o->oPosY, o->oPosZ);
                gRedCoinStarSpawned = TRUE;
                spawn_mist_particles();
                o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
            }
            break;
    }
}
