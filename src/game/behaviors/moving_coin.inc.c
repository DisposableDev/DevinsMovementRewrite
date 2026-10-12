// moving_coin.inc.c

static struct ObjectHitbox sMovingYellowCoinHitbox = {
    /* interactType:      */ INTERACT_COIN,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 1,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 100,
    /* height:            */ 64,
    /* hurtboxRadius:     */ 0,
    /* hurtboxHeight:     */ 0,
};

static struct ObjectHitbox sMovingBlueCoinHitbox = {
    /* interactType:      */ INTERACT_COIN,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 5,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 100,
    /* height:            */ 64,
    /* hurtboxRadius:     */ 0,
    /* hurtboxHeight:     */ 0,
};

// Step once, fill collision flags and apply floor-death actions using the floor from object_step.
// TRUE means grounded with nonzero integer-truncated Y velocity; it also plays the drop sound.
static s32 moving_coin_step_and_report_bounce(s16 *collisionFlagsPtr) {
    *collisionFlagsPtr = object_step();

    obj_check_floor_death(*collisionFlagsPtr, sObjFloor);

    if ((*collisionFlagsPtr & OBJ_COL_FLAG_GROUNDED)
        && !(*collisionFlagsPtr & OBJ_COL_FLAG_NO_Y_VEL)) {
        cur_obj_play_sound_2(SOUND_GENERAL_COIN_DROP);
        return TRUE;
    }

    return FALSE;
}

static void moving_coin_step_and_flicker(void) {
    s16 collisionFlags;

    moving_coin_step_and_report_bounce(&collisionFlags);
    obj_flicker_and_disappear(o, 0);
}

void coin_collected(void) {
    spawn_object(o, MODEL_SPARKLES, bhvCoinSparklesSpawnerShort);
    o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
}

void bhv_moving_yellow_coin_init(void) {
    o->oGravity = 3.0f;
    o->oFriction = 1.0f;
    o->oBuoyancy = 1.5f;

    obj_set_hitbox(o, &sMovingYellowCoinHitbox);
}

void bhv_moving_yellow_coin_loop(void) {
    s16 collisionFlags;

    switch (o->oAction) {
        case MOV_YCOIN_ACT_IDLE:
            moving_coin_step_and_report_bounce(&collisionFlags);

            cur_obj_become_tangible();

            if (o->oTimer > 300) {
                o->oAction = MOV_YCOIN_ACT_BLINKING;
            }
            break;

        case MOV_YCOIN_ACT_BLINKING:
            moving_coin_step_and_flicker();
            break;

        case OBJ_ACT_LAVA_DEATH:
#ifdef COIN_LAVA_FLICKER
            moving_coin_step_and_flicker();
#else
            obj_mark_for_deletion(o);
#endif
            break;

        case OBJ_ACT_DEATH_PLANE_DEATH:
            obj_mark_for_deletion(o);
            break;
    }

    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        coin_collected();
        o->oInteractStatus = INT_STATUS_NONE;
    }
}

void bhv_moving_blue_coin_init(void) {
    o->oGravity = 5.0f;
    o->oFriction = 1.0f;
    o->oBuoyancy = 1.5f;

    obj_set_hitbox(o, &sMovingBlueCoinHitbox);
}

void bhv_moving_blue_coin_loop(void) {
    s16 collisionFlags;

    switch (o->oAction) {
        case MOV_BCOIN_ACT_STILL:
            if (is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, 1500)) {
                o->oAction = MOV_BCOIN_ACT_MOVING;
            }
            break;

        case MOV_BCOIN_ACT_MOVING:
            collisionFlags = object_step();

            if (collisionFlags & OBJ_COL_FLAG_GROUNDED) {
                o->oForwardVel += 25.0f;
                if (!(collisionFlags & OBJ_COL_FLAG_NO_Y_VEL)) {
                    cur_obj_play_sound_2(SOUND_GENERAL_COIN_DROP);
                }
            } else {
                o->oForwardVel *= 0.98f;
            }

            if (o->oForwardVel > 75.0f) {
                o->oForwardVel = 75.0f;
            }

            obj_flicker_and_disappear(o, 600);
            break;
    }

    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        coin_collected();
        o->oInteractStatus = INT_STATUS_NONE;
    }
}

void bhv_blue_coin_sliding_jumping_init(void) {
    o->oGravity = 3.0f;
    o->oFriction = 0.98f;
    o->oBuoyancy = 1.5f;

    obj_set_hitbox(o, &sMovingBlueCoinHitbox);
}

// Sliding coins flee; jumping coins pursue. The behavior identity selects the steering direction.
static void blue_coin_move_relative_to_mario(void) {
    s16 collisionFlags;

    o->oForwardVel = 15.0f;
    o->oMoveAngleYaw = o->oAngleToMario;
    if (!cur_obj_has_behavior(bhvBlueCoinJumping)) {
        o->oMoveAngleYaw += 0x8000;
    }

    if (moving_coin_step_and_report_bounce(&collisionFlags)) {
        o->oVelY += 18.0f;
    }

    if (collisionFlags & OBJ_COL_FLAG_HIT_WALL) {
        o->oAction = MOV_BCOIN_ACT_STOPPED;
    }

    // Distance takes precedence over the wall-triggered stop when both checks apply.
    if (!is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, 1000)) {
        o->oAction = MOV_BCOIN_ACT_SLOWING_DOWN;
    }
}

static void blue_coin_update_slowdown(void) {
    s16 collisionFlags;

    moving_coin_step_and_report_bounce(&collisionFlags);

    if (is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, 500)) {
        o->oAction = MOV_BCOIN_ACT_MOVING;
    }

    // The slowdown timeout takes precedence over resuming movement near Mario in this update.
    if (o->oTimer > 150) {
        o->oAction = MOV_BCOIN_ACT_STOPPED;
    }
}

void bhv_blue_coin_sliding_loop(void) {
    s16 collisionFlags;

    switch (o->oAction) {
        case MOV_BCOIN_ACT_STILL:
            if (is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, 500)) {
                o->oAction = MOV_BCOIN_ACT_MOVING;
            }

            set_object_visibility(o, 3000);
            break;

        case MOV_BCOIN_ACT_MOVING:
            blue_coin_move_relative_to_mario();
            break;

        case MOV_BCOIN_ACT_SLOWING_DOWN:
            blue_coin_update_slowdown();
            set_object_visibility(o, 3000);
            break;

        case MOV_BCOIN_ACT_STOPPED:
            moving_coin_step_and_report_bounce(&collisionFlags);
            if (o->oTimer > 60) {
                o->oAction = MOV_BCOIN_ACT_FLICKERING;
            }
            break;

        case MOV_BCOIN_ACT_FLICKERING:
            moving_coin_step_and_flicker();
            break;

        case OBJ_ACT_LAVA_DEATH:
#ifdef COIN_LAVA_FLICKER
            o->oAction = MOV_BCOIN_ACT_FLICKERING;
#else
            obj_mark_for_deletion(o);
#endif
            break;

        case OBJ_ACT_DEATH_PLANE_DEATH:
            obj_mark_for_deletion(o);
            break;
    }

    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        coin_collected();
        o->oInteractStatus = INT_STATUS_NONE;
    }
}

void bhv_blue_coin_jumping_loop(void) {
    s16 collisionFlags;

    switch (o->oAction) {
        case MOV_BCOIN_ACT_STILL:
            if (o->oTimer == 0) {
                cur_obj_become_tangible();
                o->oVelY = 50.0f;
                o->oForwardVel = 15.0f;
                o->oMoveAngleYaw = obj_angle_to_object(o, gMarioObject);
            }

            object_step();

            if (o->oTimer == 15) {
                cur_obj_become_tangible();
                o->oAction = MOV_BCOIN_ACT_MOVING;
            }
            break;

        case MOV_BCOIN_ACT_MOVING:
            blue_coin_move_relative_to_mario();
            break;

        case MOV_BCOIN_ACT_SLOWING_DOWN:
            blue_coin_update_slowdown();
            set_object_visibility(o, 3000);
            break;

        case MOV_BCOIN_ACT_STOPPED:
            moving_coin_step_and_report_bounce(&collisionFlags);
            if (o->oTimer > 60) {
                o->oAction = MOV_BCOIN_ACT_FLICKERING;
            }
            break;

        case MOV_BCOIN_ACT_FLICKERING:
            moving_coin_step_and_flicker();
            break;
    }

    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        coin_collected();
        o->oInteractStatus = INT_STATUS_NONE;
    }
}
