// coin.inc.c

static struct ObjectHitbox sYellowCoinHitbox = {
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

/**
 *     3
 *   4   6
 * 5   2   7
 *
 *     1
 *
 *     0
 **/
// Parent-local X/Z offsets; array indices also identify each coin in the collected-bit mask.
static const s16 sCoinArrowOffsets[][2] = {
    { 0, -150 },  { 0, -50 },   { 0, 50 },   { 0, 150 },
    { -50, 100 }, { -100, 50 }, { 50, 100 }, { 100, 50 },
};

// Awarding belongs to interact_coin. TRUE emits pickup sparkles and marks deletion, retaining status.
// FALSE clears status, including Bob-omb touches excluded from this pickup path.
static s32 coin_handle_collection(void) {
    if (o->oInteractStatus & INT_STATUS_INTERACTED
        && !(o->oInteractStatus & INT_STATUS_TOUCHED_BOB_OMB)) {
        spawn_object(o, MODEL_SPARKLES, bhvCoinSparklesSpawnerShort);
        obj_mark_for_deletion(o);
        return TRUE;
    }

    o->oInteractStatus = INT_STATUS_NONE;

    return FALSE;
}

void bhv_yellow_coin_init(void) {
    // Normalize lookup identity without changing the running script (including temporary coins).
    cur_obj_set_behavior(bhvYellowCoin);
    obj_set_hitbox(o, &sYellowCoinHitbox);
    cur_obj_update_floor_height();

    if (500.0f < absf(o->oPosY - o->oFloorHeight)) {
        if (cur_obj_has_model(MODEL_YELLOW_COIN)) {
            cur_obj_set_model(MODEL_YELLOW_COIN_NO_SHADOW);
        } else if (cur_obj_has_model(MODEL_BLUE_COIN)) {
            cur_obj_set_model(MODEL_BLUE_COIN_NO_SHADOW);
        } else if (cur_obj_has_model(MODEL_RED_COIN)) {
            cur_obj_set_model(MODEL_RED_COIN_NO_SHADOW);
#ifdef IA8_30FPS_COINS
        } else if (cur_obj_has_model(MODEL_SILVER_COIN)) {
            cur_obj_set_model(MODEL_SILVER_COIN_NO_SHADOW);
#endif
        }
    }

    if (o->oFloorHeight < FLOOR_LOWER_LIMIT_MISC) {
        obj_mark_for_deletion(o);
    }
}

void bhv_yellow_coin_loop(void) {
    coin_handle_collection();
    o->oAnimState++;
}

void bhv_temp_coin_loop(void) {
    o->oAnimState++;

    // Expiration precedes pickup handling; keep that handling on the final update too.
    if (cur_obj_wait_then_blink(200, 20)) {
        obj_mark_for_deletion(o);
    }

    coin_handle_collection();
}

void bhv_coin_spawn_ring(void) {
    obj_spawn_coin_ring(o, o->oNumLootCoins, 0.0f);
}

void bhv_coin_init(void) {
    // Ring drops and Yoshi gifts already supply velocity and yaw; keep their launch setup.
    if (!o->oCoinSpawnConfigured) {
        o->oVelY = random_float() * 10.0f + 30 + o->oCoinBaseYVel;
        o->oForwardVel = 10.0f;
        o->oMoveAngleYaw = obj_angle_to_object(o, gMarioObject);
    }

    cur_obj_set_behavior(bhvYellowCoin);
    obj_set_hitbox(o, &sYellowCoinHitbox);
    cur_obj_become_tangible();
}

void bhv_coin_loop(void) {
    cur_obj_update_floor_and_walls();
    cur_obj_if_hit_wall_bounce_away();
    cur_obj_move_standard(-62);

    struct Surface *floor = o->oFloor;

    if (floor != NULL) {
        switch (o->oAction) {
            case BOUNCING_COIN_ACT_FALLING:
                if (o->oTimer == 0) {
                    cur_obj_play_sound_2(SOUND_GENERAL_COIN_SPURT);
                }
                if (o->oMoveFlags & OBJ_MOVE_LANDED) {
                    o->oAction = BOUNCING_COIN_ACT_BOUNCING;
                }

                break;

            case BOUNCING_COIN_ACT_BOUNCING:
                o->oBounciness = 0;
                if (floor->normal.y < 0.9f) {
                    s16 targetYaw = SURFACE_YAW(floor);
                    cur_obj_rotate_yaw_toward(targetYaw, 0x400);
                }
                break;

            case OBJ_ACT_LAVA_DEATH:
#ifdef COIN_LAVA_FLICKER
                obj_flicker_and_disappear(o, 0);
#else
                obj_mark_for_deletion(o);
#endif
                break;
        }
    }

    if (o->oMoveFlags & OBJ_MOVE_LANDED) {
        if (o->oMoveFlags & OBJ_MOVE_ABOVE_DEATH_BARRIER) {
            obj_mark_for_deletion(o);
        } else if (o->oMoveFlags & OBJ_MOVE_ABOVE_LAVA) {
            o->oAction = OBJ_ACT_LAVA_DEATH;
        }
    }

    if (o->oMoveFlags & OBJ_MOVE_BOUNCE) {
        if (o->oCoinBounceCount < 5) {
            cur_obj_play_sound_2(SOUND_GENERAL_COIN_DROP);
        }
        o->oCoinBounceCount++;
    }

    if (cur_obj_wait_then_blink(400, 20)) {
        obj_mark_for_deletion(o);
    }

    // Hazard and expiration deletion marks do not skip this update's pickup handling.
    coin_handle_collection();
}

void bhv_coin_formation_spawned_coin_loop(void) {
    if (o->oTimer == 0) {
        cur_obj_set_behavior(bhvYellowCoin);
        obj_set_hitbox(o, &sYellowCoinHitbox);
        if (o->oCoinSnapToGround) {
            o->oPosY += 300.0f;
            cur_obj_update_floor_height();

            if (o->oPosY + FIND_FLOOR_BUFFER < o->oFloorHeight || o->oFloorHeight < FLOOR_LOWER_LIMIT_MISC) {
                obj_mark_for_deletion(o);
            } else {
                o->oPosY = o->oFloorHeight;
            }
        } else {
            cur_obj_update_floor_height();

            if (absf(o->oPosY - o->oFloorHeight) > 250.0f) {
                cur_obj_set_model(MODEL_YELLOW_COIN_NO_SHADOW);
            }
        }
    } else {
        if (coin_handle_collection()) {
            // The child parameter is its slot index; the parent owns the collected-bit mask.
            o->parentObj->oCoinRespawnBits |= (1 << o->oBehParams2ndByte);
        }
        o->oAnimState++;
    }
    if (o->parentObj->oAction == COIN_FORMATION_ACT_DEACTIVATE) {
        obj_mark_for_deletion(o);
    }
}

// Slots 0..7 are stable collection-bit positions; line shapes use only slots 0..4.
// The shape and flying flag share one parameter byte. Offsets follow the parent transform.
static void coin_formation_spawn_coin(s32 coinIndex, s32 formationParams) {
    Vec3i relativePos = { 0, 0, 0 };
    s32 spawnCoin    = TRUE;
    s32 snapToGround = TRUE;

    switch (formationParams & COIN_FORMATION_BP_SHAPE_MASK) {
        case COIN_FORMATION_BP_SHAPE_HORIZONTAL_LINE:
            relativePos[2] = 160 * (coinIndex - 2);
            if (coinIndex > 4) {
                spawnCoin = FALSE;
            }
            break;
        case COIN_FORMATION_BP_SHAPE_VERTICAL_LINE:
            snapToGround = FALSE;
            relativePos[1] = coinIndex << 7;
            if (coinIndex > 4) {
                spawnCoin = FALSE;
            }
            break;
        case COIN_FORMATION_BP_SHAPE_HORIZONTAL_RING:
            relativePos[0] = sins(coinIndex << 13) * 300.0f;
            relativePos[2] = coss(coinIndex << 13) * 300.0f;
            break;
        case COIN_FORMATION_BP_SHAPE_VERTICAL_RING:
            snapToGround = FALSE;
            relativePos[0] = coss(coinIndex << 13) * 200.0f;
            relativePos[1] = sins(coinIndex << 13) * 200.0f + 200.0f;
            break;
        case COIN_FORMATION_BP_SHAPE_ARROW:
            relativePos[0] = sCoinArrowOffsets[coinIndex][0];
            relativePos[2] = sCoinArrowOffsets[coinIndex][1];
            break;
    }

    if (formationParams & COIN_FORMATION_BP_FLYING) {
        snapToGround = FALSE;
    }

    if (spawnCoin) {
        struct Object *coin =
            spawn_object_relative(coinIndex, relativePos[0], relativePos[1], relativePos[2], o,
                                  MODEL_YELLOW_COIN, bhvCoinFormationSpawnedCoin);
        coin->oCoinSnapToGround = snapToGround;
    }
}

void bhv_coin_formation_init(void) {
    // Restore the slots already collected before this formation was unloaded.
    o->oCoinRespawnBits = GET_BPARAM3(o->oBehParams);
}

void bhv_coin_formation_loop(void) {
    s32 coinIndex;

    switch (o->oAction) {
        case COIN_FORMATION_ACT_INACTIVE:
            if (o->oDistanceToMario < COIN_FORMATION_DISTANCE) {
                for (coinIndex = 0; coinIndex < 8; coinIndex++) {
                    if (!(o->oCoinRespawnBits & (1 << coinIndex))) {
                        coin_formation_spawn_coin(coinIndex, o->oBehParams2ndByte);
                    }
                }
                o->oAction = COIN_FORMATION_ACT_ACTIVE;
            }
            break;
        case COIN_FORMATION_ACT_ACTIVE:
            if (o->oDistanceToMario > (COIN_FORMATION_DISTANCE + 100.0f)) {
                o->oAction = COIN_FORMATION_ACT_DEACTIVATE;
            }
            break;
        case COIN_FORMATION_ACT_DEACTIVATE:
            // Spawners update before level objects: children saw this state in the previous frame.
            o->oAction = COIN_FORMATION_ACT_INACTIVE;
            break;
    }

    // OR the collected low byte into the area respawn record, separate from save-file coin totals.
    set_object_respawn_info_bits(o, o->oCoinRespawnBits & RESPAWN_INFO_DONT_RESPAWN);
}

static void coin_inside_boo_act_dropped(void) {
    cur_obj_update_floor_and_walls();
    cur_obj_if_hit_wall_bounce_away();

    if (o->oMoveFlags & OBJ_MOVE_BOUNCE) {
        cur_obj_play_sound_2(SOUND_GENERAL_COIN_DROP);
    }

    obj_set_hitbox(o, &sYellowCoinHitbox);
    cur_obj_become_tangible();
    cur_obj_set_behavior(bhvYellowCoin);

    cur_obj_move_standard(-30);
    coin_handle_collection();

    if (cur_obj_has_model(MODEL_BLUE_COIN)) {
        o->oDamageOrCoinValue = 5;
    }

    if (cur_obj_wait_then_blink(400, 20)) {
        obj_mark_for_deletion(o);
    }
}

static void coin_inside_boo_act_carried(void) {
    struct Object *parent = o->parentObj;

    cur_obj_become_intangible();
    if (o->oTimer == 0 && GET_BPARAM1(parent->oBehParams) != COIN_INSIDE_BOO_BP_YELLOW_COIN) {
        cur_obj_set_model(MODEL_BLUE_COIN);
        cur_obj_scale(0.7f);
    }

    obj_copy_pos(o, parent);

    // Release when death begins, before the parent finishes its death animation or is deleted.
    if (parent->oBooDeathStatus == BOO_DEATH_STATUS_DYING) {
        o->oAction = COIN_INSIDE_BOO_ACT_DROPPED;
        // Released rewards outlive the encounter cleanup; carried coins do not.
        o->bowserArenaOwner = NULL;
        o->oMoveAngleYaw = obj_angle_to_object(o, gMarioObject);
        o->oForwardVel = 10.0f;
        o->oVelY = 35.0f;
        // Set reward value in the release handoff, before the first dropped update.
        obj_set_hitbox(o, &sYellowCoinHitbox);
        if (cur_obj_has_model(MODEL_BLUE_COIN)) {
            o->oDamageOrCoinValue = 5;
        }
        cur_obj_become_tangible();
    }
}

static ObjActionFunc sCoinInsideBooActions[] = { coin_inside_boo_act_carried,
                                                 coin_inside_boo_act_dropped };

void bhv_coin_inside_boo_loop(void) {
    cur_obj_call_action_function(sCoinInsideBooActions);
}

void bhv_coin_sparkles_loop(void) {
    cur_obj_scale(0.6f);
}

void bhv_short_coin_sparkles_loop(void) {
    // Play the pickup's eight animation frames over five updates.
    o->oAnimState = (o->oTimer * 7) / 4;
    if (o->oTimer == 4) {
        cur_obj_scale(0.6f);
    }
}

void bhv_golden_coin_sparkles_loop(void) {
    const BehaviorScript *behavior =
        obj_has_behavior(o, bhvCoinSparklesSpawnerShort) ? bhvCoinSparklesShort : bhvCoinSparkles;
    struct Object *sparkleObj = spawn_object(o, MODEL_SPARKLES, behavior);
    sparkleObj->oPosX += random_float() * 30.0f - 15.0f;
    sparkleObj->oPosZ += random_float() * 30.0f - 15.0f;
}
