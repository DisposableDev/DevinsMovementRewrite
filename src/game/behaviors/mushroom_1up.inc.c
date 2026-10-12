// mushroom_1up.inc.c

#define ONE_UP_SPARKLE_EMIT_UPDATES 2
#define ONE_UP_SPARKLE_EMISSION_PERIOD_UPDATES 5

void one_up_spawn_sparkles(void) {
    // Direct emission avoids the shared spawner's three children per request.
    if ((o->oTimer % ONE_UP_SPARKLE_EMISSION_PERIOD_UPDATES) >= ONE_UP_SPARKLE_EMIT_UPDATES) {
        return;
    }
    struct Object *sparkle =
        try_spawn_particle(o, MODEL_SPARKLES_ANIMATION, bhvSparkleShort, PARTICLE_PRIORITY_FEEDBACK);
    if (sparkle != NULL) {
        obj_translate_xyz_random(sparkle, 90.0f);
        obj_scale_random(sparkle, 1.0f, 0.0f);
    }
}

void bhv_1up_interact(void) {
    if (obj_check_if_collided_with_object(o, gMarioObject)) {
        player_stats_add(PS_1_UP_MUSHROOMS_COLLECTED, 1);
        play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
        mario_ground_twirl_reset_cooldown();
        mario_extinguish_fire(gMarioState);
#ifdef MUSHROOMS_HEAL
        if (gMarioState->health >= 0x100) {
            // Use the level-exit refill so wedges return gradually.
            gMarioState->respawnHealCounter = 0;
            gMarioState->healCounter = 31;
            gMarioState->hurtCounter = 0;
#ifdef BREATH_METER
            gMarioState->breathCounter = 31;
#endif
        }
#endif
#ifdef ENABLE_LIVES
        gMarioState->numLives++;
#endif
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        queue_rumble_data(5, 80);
    }
}

void bhv_1up_common_init(void) {
    o->oMoveAnglePitch = -0x4000;
    o->oGravity = 3.0f;
    o->oFriction = 1.0f;
    o->oBuoyancy = 1.0f;
}

void bhv_1up_init(void) {
    bhv_1up_common_init();
    if (o->oBehParams2ndByte == MUSHROOM_BP_REQUIRES_CASTLE_COMPLETION
        && game_options_secrets_blocked()) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        return;
    }
#if !defined(UNLOCK_ALL) && !defined(COMPLETE_SAVE_FILE)
    if (o->oBehParams2ndByte == MUSHROOM_BP_REQUIRES_CASTLE_COMPLETION && !gDebugLevelSelect
        && save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(COURSE_MIN),
                                          COURSE_NUM_TO_INDEX(COURSE_MAX))
               < CASTLE_ROOF_COMPLETION_STARS) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        return;
    }
#endif
#ifndef UNLOCK_ALL
    if (o->oBehParams2ndByte == MUSHROOM_BP_REQUIRES_BOWSER_1) {
        if (!(save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_1 | SAVE_FLAG_UNLOCKED_BASEMENT_DOOR))) {
            o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        }
    } else if (o->oBehParams2ndByte == MUSHROOM_BP_REQUIRES_BOWSER_2) {
        if (!(save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_2 | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR))) {
            o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        }
    }
#endif
}

// object_step can turn at a wall and then move again using the new yaw. Its
// floor query was made before that final move, so check the accepted position
// for the small mushrooms without changing generic object movement.
static s16 one_up_step(void) {
    f32 stepStartY = o->oPosY;
    s16 collisionFlags = object_step();
    struct Surface *postStepFloor;
    f32 postStepFloorHeight;

    if (f32_find_wall_collision(&o->oPosX, &o->oPosY, &o->oPosZ, o->hitboxHeight * 0.5f,
                                o->hitboxRadius)) {
        collisionFlags |= OBJ_COL_FLAG_HIT_WALL;
    }

    postStepFloorHeight = find_floor(o->oPosX, MAX(stepStartY, o->oPosY), o->oPosZ, &postStepFloor);
    o->oFloor = postStepFloor;
    o->oFloorHeight = postStepFloorHeight;
    // Preserve other step flags; recompute ground/vertical flags after the correction.
    collisionFlags &= ~(OBJ_COL_FLAG_GROUNDED | OBJ_COL_FLAG_NO_Y_VEL);
    if (postStepFloor != NULL && o->oPosY < postStepFloorHeight
        && postStepFloorHeight <= stepStartY + 37.0f) {
        o->oPosY = postStepFloorHeight;
        if (o->oVelY < -17.5f) {
            o->oVelY *= -0.5f;
        } else if (o->oVelY < 0.0f) {
            o->oVelY = 0.0f;
        }
    }
    // Integer-truncated comparisons match object_step's flag semantics.
    if (postStepFloor != NULL && (s32) o->oPosY == (s32) postStepFloorHeight) {
        collisionFlags |= OBJ_COL_FLAG_GROUNDED;
    }
    if ((s32) o->oVelY == 0) {
        collisionFlags |= OBJ_COL_FLAG_NO_Y_VEL;
    }
    return collisionFlags;
}

void one_up_loop_in_air(void) {
    if (o->oTimer < 5) {
        o->oVelY = 40.0f;
    } else {
        o->oAngleVelPitch = -0x1000;
        o->oMoveAnglePitch += o->oAngleVelPitch;
        o->oVelY = coss(o->oMoveAnglePitch) * 30.0f + 2.0f;
        o->oForwardVel = -sins(o->oMoveAnglePitch) * 30.0f;
    }
}

// Update orientation and velocity only; callers apply object movement separately.
static void one_up_steer_toward_mario(void) {
    f32 dx = gMarioObject->header.gfx.pos[0] - o->oPosX;
    f32 dy = gMarioObject->header.gfx.pos[1] - o->oPosY + 120.0f;
    f32 dz = gMarioObject->header.gfx.pos[2] - o->oPosZ;
    s16 targetPitch = atan2s(sqrtf(sqr(dx) + sqr(dz)), dy);

    obj_turn_toward_object(o, gMarioObject, O_MOVE_ANGLE_YAW_INDEX, 0x1000);

    o->oMoveAnglePitch = approach_s16_symmetric(o->oMoveAnglePitch, targetPitch, 0x1000);
    o->oVelY = sins(o->oMoveAnglePitch) * 30.0f;
    o->oForwardVel = coss(o->oMoveAnglePitch) * 30.0f;
}

void pole_1up_move_towards_mario(void) {
    one_up_steer_toward_mario();
    bhv_1up_interact();
}

void one_up_move_away_from_mario(s16 collisionFlags) {
    o->oForwardVel = 8.0f;
    o->oMoveAngleYaw = o->oAngleToMario + 0x8000;

    bhv_1up_interact();

    if (collisionFlags & OBJ_COL_FLAG_HIT_WALL) {
        o->oAction = MUSHROOM_ACT_DISAPPEARING;
    }

    if (!is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, 3000)) {
        o->oAction = MUSHROOM_ACT_DISAPPEARING;
    }
}

void bhv_1up_walking_loop(void) {
    one_up_step();

    switch (o->oAction) {
        case MUSHROOM_ACT_INIT:
            if (o->oTimer > 17) {
                one_up_spawn_sparkles();
            }

            if (o->oTimer == 0) {
                play_sound(SOUND_GENERAL2_1UP_APPEAR, gGlobalSoundSource);
            }

            one_up_loop_in_air();

            if (o->oTimer == 37) {
                cur_obj_become_tangible();
                o->oAction = MUSHROOM_ACT_MOVING;
                o->oForwardVel = 2.0f;
            }
            break;

        case MUSHROOM_ACT_MOVING:
            if (o->oTimer > 300) {
                o->oAction = MUSHROOM_ACT_DISAPPEARING;
            }

            bhv_1up_interact();
            break;

        case MUSHROOM_ACT_DISAPPEARING:
            obj_flicker_and_disappear(o, 30);
            bhv_1up_interact();
            break;
    }

    set_object_visibility(o, 3000);
}

void bhv_poison_mushroom_loop(void) {
    if (game_options_secrets_blocked()) {
        obj_mark_for_deletion(o);
        return;
    }
    if (o->oAction == MUSHROOM_ACT_INIT) {
        one_up_step();
        if (o->oTimer == 0) {
            play_sound(SOUND_GENERAL2_1UP_APPEAR, gGlobalSoundSource);
        }
        if (o->oTimer > 17) {
            one_up_spawn_sparkles();
        }
        one_up_loop_in_air();
        if (o->oTimer == 37) {
            cur_obj_become_tangible();
            o->oAction = MUSHROOM_ACT_MOVING;
            o->oVelY = 0.0f;
            o->oForwardVel = 0.0f;
        }
    } else {
        one_up_steer_toward_mario();
        one_up_step();
        if (obj_check_if_collided_with_object(o, gMarioObject) && gMarioState->health >= 0x100) {
            struct MarioState *m = gMarioState;
            mario_stop_riding_and_holding(m);
            mario_extinguish_fire(m);
            player_stats_damage();
            player_stats_health(m->health, 0xFF);
            m->health = 0xFF;
            m->healCounter = 0;
            m->respawnHealCounter = 0;
            m->hurtCounter = 0;
            m->yoshiCoinDeathPending = FALSE;
            // The death warp uses this latch to force Game Over; Mario initialization clears it.
            m->poisonMushroomDeathPending = TRUE;
            save_file_unlock_castle_wing_cap(gCurrSaveFileNum - 1);
            set_mario_action(m, ACT_STANDING_DEATH, 0);
            obj_mark_for_deletion(o);
            queue_rumble_data(5, 80);
        }
    }
    set_object_visibility(o, 3000);
}

void bhv_1up_running_away_loop(void) {
    s16 collisionFlags = one_up_step();

    switch (o->oAction) {
        case MUSHROOM_ACT_INIT:
            if (o->oTimer > 17) {
                one_up_spawn_sparkles();
            }

            if (o->oTimer == 0) {
                play_sound(SOUND_GENERAL2_1UP_APPEAR, gGlobalSoundSource);
            }

            one_up_loop_in_air();

            if (o->oTimer == 37) {
                cur_obj_become_tangible();
                o->oAction = MUSHROOM_ACT_MOVING;
                o->oForwardVel = 8.0f;
            }
            break;

        case MUSHROOM_ACT_MOVING:
            one_up_spawn_sparkles();
            one_up_move_away_from_mario(collisionFlags);
            break;

        case MUSHROOM_ACT_DISAPPEARING:
            obj_flicker_and_disappear(o, 30);
            bhv_1up_interact();
            break;
    }

    set_object_visibility(o, 3000);
}

void sliding_1up_move(void) {
    s16 collisionFlags = one_up_step();

    if (collisionFlags & OBJ_COL_FLAG_GROUNDED) {
        o->oForwardVel += 25.0f;
        o->oVelY = 0.0f;
    } else {
        o->oForwardVel *= 0.98f;
    }

    if (o->oForwardVel > 40.0) {
        o->oForwardVel = 40.0f;
    }

    if (!is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, 5000)) {
        o->oAction = MUSHROOM_ACT_DISAPPEARING;
    }
}

void bhv_1up_sliding_loop(void) {
    switch (o->oAction) {
        case MUSHROOM_ACT_INIT:
            set_object_visibility(o, 3000);
            if (is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, 1000)) {
                o->oAction = MUSHROOM_ACT_MOVING;
            }
            break;

        case MUSHROOM_ACT_MOVING:
            sliding_1up_move();
            break;

        case MUSHROOM_ACT_DISAPPEARING:
            obj_flicker_and_disappear(o, 30);
            break;
    }

    bhv_1up_interact();
    one_up_spawn_sparkles();
}

void bhv_1up_loop(void) {
    if (o->oBehParams2ndByte == MUSHROOM_BP_REQUIRES_CASTLE_COMPLETION
        && game_options_secrets_blocked()) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        return;
    }
    bhv_1up_interact();
    set_object_visibility(o, 3000);
}

void bhv_1up_jump_on_approach_loop(void) {
    s16 collisionFlags;

    switch (o->oAction) {
        case MUSHROOM_ACT_INIT:
            if (is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, 1000)) {
                o->oVelY = 40.0f;
                o->oAction = MUSHROOM_ACT_MOVING;
            }
            break;

        case MUSHROOM_ACT_MOVING:
            collisionFlags = one_up_step();
            one_up_move_away_from_mario(collisionFlags);
            one_up_spawn_sparkles();
            break;

        case MUSHROOM_ACT_DISAPPEARING:
            collisionFlags = one_up_step();
            bhv_1up_interact();
            obj_flicker_and_disappear(o, 30);
            break;
    }

    set_object_visibility(o, 3000);
}

void bhv_1up_hidden_loop(void) {
    s16 collisionFlags;

    switch (o->oAction) {
        case MUSHROOM_ACT_INIT:
            o->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
            if (o->o1UpHiddenTimesTriggered == o->oBehParams2ndByte) {
                o->oVelY = 40.0f;
                o->oAction = MUSHROOM_ACT_LOOP_IN_AIR;
                o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
                play_sound(SOUND_GENERAL2_1UP_APPEAR, gGlobalSoundSource);
            }
            break;

        case MUSHROOM_ACT_MOVING:
            collisionFlags = one_up_step();
            one_up_move_away_from_mario(collisionFlags);
            one_up_spawn_sparkles();
            break;

        case MUSHROOM_ACT_DISAPPEARING:
            collisionFlags = one_up_step();
            bhv_1up_interact();
            obj_flicker_and_disappear(o, 30);
            break;

        case MUSHROOM_ACT_LOOP_IN_AIR:
            collisionFlags = one_up_step();
            if (o->oTimer > 17) {
                one_up_spawn_sparkles();
            }

            one_up_loop_in_air();

            if (o->oTimer == 37) {
                cur_obj_become_tangible();
                o->oAction = MUSHROOM_ACT_MOVING;
                o->oForwardVel = 8.0f;
            }
            break;
    }
}

void bhv_1up_hidden_trigger_loop(void) {
    if (obj_check_if_collided_with_object(o, gMarioObject)) {
        struct Object *nearestHidden1up = cur_obj_nearest_object_with_behavior(bhvHidden1up);
        if (nearestHidden1up != NULL) {
            nearestHidden1up->o1UpHiddenTimesTriggered++;
        }

        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}

void bhv_1up_hidden_in_pole_loop(void) {
    switch (o->oAction) {
        case MUSHROOM_ACT_INIT:
            o->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
            if (o->o1UpHiddenTimesTriggered == o->oBehParams2ndByte) {
                o->oVelY = 40.0f;
                o->oAction = MUSHROOM_ACT_LOOP_IN_AIR;
                o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
                play_sound(SOUND_GENERAL2_1UP_APPEAR, gGlobalSoundSource);
            }
            break;

        case MUSHROOM_ACT_MOVING:
            pole_1up_move_towards_mario();
            one_up_step();
            break;

        case MUSHROOM_ACT_LOOP_IN_AIR:
            one_up_step();
            if (o->oTimer > 17) {
                one_up_spawn_sparkles();
            }

            one_up_loop_in_air();

            if (o->oTimer == 37) {
                cur_obj_become_tangible();
                o->oAction = MUSHROOM_ACT_MOVING;
                o->oForwardVel = 10.0f;
            }
            break;
    }
}

void bhv_1up_hidden_in_pole_trigger_loop(void) {
    if (obj_check_if_collided_with_object(o, gMarioObject)) {
        struct Object *nearestHidden1upInPole = cur_obj_nearest_object_with_behavior(bhvHidden1upInPole);
        if (nearestHidden1upInPole != NULL) {
            nearestHidden1upInPole->o1UpHiddenTimesTriggered++;
        }

        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}

void bhv_1up_hidden_in_pole_spawner_loop(void) {
    if (is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, 700)) {
        s8 i;
        spawn_object_relative(2, 0, 50, 0, o, MODEL_1UP, bhvHidden1upInPole);
        for (i = 0; i < 2; i++) {
            spawn_object_relative(0, 0, i * -200, 0, o, MODEL_NONE, bhvHidden1upInPoleTrigger);
        }

        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}
