
/**
 * Shared Tweester tornado actions and the sand particles emitted while chasing.
 */

struct ObjectHitbox sTweesterHitbox = {
    /* interactType:      */ INTERACT_TORNADO,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 0,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 1500,
    /* height:            */ 4000,
    /* hurtboxRadius:     */ 0,
    /* hurtboxHeight:     */ 0,
};

/**
 * Apply the growth/shrink multiplier to the oscillating shape and advance spin.
 * Also reset forward velocity to 14; chase calls this after its movement step.
 */
void tweester_scale_and_move(f32 preScale) {
    s16 dYaw  = 0x2C00;
    f32 scale = preScale * 0.4f;

    o->header.gfx.scale[0]
        = (( coss(o->oTweesterScaleTimer) + 1.0) * 0.5 * 0.3 + 1.0) * scale;
    o->header.gfx.scale[1]
        = ((-coss(o->oTweesterScaleTimer) + 1.0) * 0.5 * 0.5 + 0.5) * scale;
    o->header.gfx.scale[2]
        = (( coss(o->oTweesterScaleTimer) + 1.0) * 0.5 * 0.3 + 1.0) * scale;

    o->oTweesterScaleTimer += 0x200;
    o->oForwardVel = 14.0f;
    o->oFaceAngleYaw += dYaw;
}

/**
 * Wait at home with zero scale and no interaction until Mario is within 1500 units.
 * Growth remains intangible at timer zero, becomes tangible at positive timers,
 * and switches to chasing at timer 60.
 */
void tweester_act_idle(void) {
    if (o->oSubAction == TWEESTER_SUB_ACT_WAIT) {
        cur_obj_become_intangible();
        cur_obj_set_pos_to_home();
        cur_obj_scale(0.0f);

        // Retain the legacy field reset while waiting.
        o->oTweesterUnused = 0;

        // If Mario is within range, change to the growth sub-action.
        if (o->oDistanceToMario < 1500.0f) {
            o->oSubAction++;
        }

        o->oTimer = 0;
    } else {
        cur_obj_play_sound_1(SOUND_ENV_WIND1);
        tweester_scale_and_move(o->oTimer / 60.0f);
        if (o->oTimer > 0) {
            cur_obj_become_tangible();
        }
        if (o->oTimer >= 60) {
            o->oAction = TWEESTER_ACT_CHASE;
        }
    }
}

/**
 * Chase Mario only inside the home activation radius and in the chase sub-action.
 * Otherwise steer home; twirling during chase selects return-home for later updates.
 * Selecting the hide action still completes this update's movement and emission.
 */
void tweester_act_chase(void) {
    f32 activationRadius = o->oBehParams2ndByte * 100;

    o->oAngleToHome = cur_obj_angle_to_home();
    cur_obj_play_sound_1(SOUND_ENV_WIND1);

    if (cur_obj_lateral_dist_from_mario_to_home() < activationRadius
        && o->oSubAction == TWEESTER_SUB_ACT_CHASE_MARIO) {

        o->oForwardVel = 20.0f;
        cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x200);
        print_debug_top_down_objectinfo("off ", 0);

        if (gMarioStates[0].action == ACT_TWIRLING) {
            o->oSubAction++;
        }
    } else {
        o->oForwardVel = 20.0f;
        cur_obj_rotate_yaw_toward(o->oAngleToHome, 0x200);

        if (cur_obj_lateral_dist_to_home() < 200.0f) {
            o->oAction = TWEESTER_ACT_HIDE;
        }
    }

    if (o->oDistanceToMario > 3000.0f) {
        o->oAction = TWEESTER_ACT_HIDE;
    }

    cur_obj_update_floor_and_walls();
    if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
        o->oMoveAngleYaw = o->oWallAngle;
    }

    cur_obj_move_standard(60);
    tweester_scale_and_move(1.0f);
    try_spawn_particle(o, MODEL_SAND_DUST, bhvTweesterSandParticle, PARTICLE_PRIORITY_AMBIENT);
}

/**
 * Shrink through timer 60, becoming intangible at zero size.
 * After shrinking, return to idle when Mario's horizontal distance from home
 * exceeds 2500 units or the action timer exceeds 360.
 */
void tweester_act_hide(void) {
    f32 shrinkTimer = 60.0f - o->oTimer;

    if (shrinkTimer >= 0.0f) {
        tweester_scale_and_move(shrinkTimer / 60.0f);
        if (shrinkTimer == 0.0f) {
            cur_obj_become_intangible();
        }
    } else {
        cur_obj_become_intangible();
        if (cur_obj_lateral_dist_from_mario_to_home() > 2500.0f) {
            o->oAction = TWEESTER_ACT_IDLE;
        }
        if (o->oTimer > 360) {
            o->oAction = TWEESTER_ACT_IDLE;
        }
    }
}

// Array of Tweester action functions.
ObjActionFunc sTweesterActions[] = {
    tweester_act_idle,
    tweester_act_chase,
    tweester_act_hide,
};

/**
 * Refresh the hitbox before action dispatch, then clear this update's interactions.
 */
void bhv_tweester_loop(void) {
    obj_set_hitbox(o, &sTweesterHitbox);
    cur_obj_call_action_function(sTweesterActions);
    o->oInteractStatus = INT_STATUS_NONE;
}

/**
 * Spin, accelerate horizontally and rise; behavior flags integrate XZ movement.
 * Randomize size each update and initial position/facing on timer zero.
 * Mark the particle for deletion once its timer exceeds 15.
 */
void bhv_tweester_sand_particle_loop(void) {
    o->oMoveAngleYaw += 0x3700;
    o->oForwardVel += 15.0f;
    o->oPosY += 22.0f;

    cur_obj_scale(random_float() + 1.0f);

    if (o->oTimer == 0) {
        obj_translate_xz_random(o, 100.0f);
        o->oFaceAnglePitch = random_u16();
        o->oFaceAngleYaw = random_u16();
    }

    if (o->oTimer > 15) {
        obj_mark_for_deletion(o);
    }
}
