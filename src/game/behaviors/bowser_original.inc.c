// Original encounter mechanics, adapted from local origin/master
// 8953c07aa4592d65ffc974b596c97ae42778a221. Shared safety/ownership code stays in the callers.

static void original_bhv_bowser_flame_spawn_loop(void) {
    struct Object *bowser = o->parentObj;
    s32 animFrame;
    f32 posX;
    f32 posZ;
    f32 cossYaw;
    f32 sinsYaw;
    s16 *data = segmented_to_virtual(dBowserFlamesOrientationValues);

    if (bowser == NULL || bowser->header.gfx.animInfo.curAnim == NULL) {
        return;
    }
    cossYaw = coss(bowser->oMoveAngleYaw);
    sinsYaw = sins(bowser->oMoveAngleYaw);

    // Check for Bowser breathing animation
    if (bowser->oSoundStateID == BOWSER_ANIM_BREATH) {

        // Start counting anim frames then reset it when it ends
        animFrame = bowser->header.gfx.animInfo.animFrame + 1.0f;
        if (bowser->header.gfx.animInfo.curAnim->loopEnd == animFrame) {
            animFrame = 0;
        }

        // Bowser is breathing, play sound and adjust flame position
        // each animFrame based off the orientantion data
        if (animFrame > 45 && animFrame < 85) {
            cur_obj_play_sound_1(SOUND_AIR_BOWSER_SPIT_FIRE);
            posX = data[5 * animFrame];
            posZ = data[5 * animFrame + 2];
            o->oPosX = bowser->oPosX + (posZ * sinsYaw + posX * cossYaw);
            o->oPosY = bowser->oPosY + data[5 * animFrame + 1];
            o->oPosZ = bowser->oPosZ + (posZ * cossYaw - posX * sinsYaw);
            o->oMoveAnglePitch = data[5 * animFrame + 4] + 0xC00;
            o->oMoveAngleYaw = data[5 * animFrame + 3] + (s16) bowser->oMoveAngleYaw;
            // Spawns the flames on a non-odd animFrame value
            if (!(animFrame & 1)) {
                spawn_object(o, MODEL_RED_FLAME, bhvFlameMovingForwardGrowing);
            }
        }
    }
}

static s32 original_bowser_spawn_shockwave(void) {
    if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
        struct Object *wave = spawn_object(o, MODEL_BOWSER_WAVE, bhvBowserShockWave);
        if (wave != NULL) {
            wave->oPosY = o->oFloorHeight;
        }
        return TRUE;
    }
    return FALSE;
}

static void original_bowser_bounce_effects(s32 *timer) {
    if (o->oMoveFlags & OBJ_MOVE_LANDED) {
        (*timer)++;
        if (*timer < 4) {
            cur_obj_start_cam_event(o, CAM_EVENT_BOWSER_THROW_BOUNCE);
            spawn_mist_particles_variable(0, 0, 60.0f);
            cur_obj_play_sound_2(SOUND_OBJ_BOWSER_WALK);
        }
    }
}

static void original_bowser_bitdw_actions(void) {
    // Generate random float
    f32 rand = random_float();
    // Set attacks when Bowser Reacts
    if (!o->oBowserIsReacting) {
        if (o->oBowserStatus & BOWSER_STATUS_ANGLE_MARIO) {
            if (o->oDistanceToMario < 1500.0f) { // nearby
                o->oAction = BOWSER_ACT_BREATH_FIRE;
            } else { // far away
                o->oAction = BOWSER_ACT_QUICK_JUMP;
            }
        } else {
            // Keep walking
            o->oAction = BOWSER_ACT_WALK_TO_MARIO;
        }
        o->oBowserIsReacting++;
    } else {
        o->oBowserIsReacting = FALSE;
        // Set starting Bowser level actions, randomly he can also start
        // dancing after the introduction
        if (!gCurrDemoInput) { // demo check because entry exits post JP
            if (rand < 0.1f) {
                o->oAction = BOWSER_ACT_DANCE; // 10% chance
            } else {
                o->oAction = BOWSER_ACT_WALK_TO_MARIO; // common
            }
        } else {
            o->oAction = BOWSER_ACT_WALK_TO_MARIO;
        }
    }
}

static void original_bowser_bitfs_actions(void) {
    // Generate random float
    f32 rand = random_float();
    // Set attacks when Bowser Reacts
    if (!o->oBowserIsReacting) {
        if (o->oBowserStatus & BOWSER_STATUS_ANGLE_MARIO) {
            if (o->oDistanceToMario < 1300.0f) { // nearby
                if (rand < 0.5f) {               // 50% chance
                    o->oAction = BOWSER_ACT_TELEPORT;
                } else {
                    o->oAction = BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR;
                }
            } else { // far away
                o->oAction = BOWSER_ACT_CHARGE_MARIO;
                if (500.0f < o->oBowserDistToCenter && o->oBowserDistToCenter < 1500.0f
                    && rand < 0.5f) { // 50% chance
                    o->oAction = BOWSER_ACT_BIG_JUMP;
                }
            }
        } else {
            // Keep walking
            o->oAction = BOWSER_ACT_WALK_TO_MARIO;
        }
        o->oBowserIsReacting++;
    } else {
        // Keep walking
        o->oBowserIsReacting = FALSE;
        o->oAction = BOWSER_ACT_WALK_TO_MARIO;
    }
}

static void original_bowser_bits_action_list(void) {
    f32 rand = random_float();
    if (o->oBowserStatus & BOWSER_STATUS_ANGLE_MARIO) {
        if (o->oDistanceToMario < 1000.0f) { // nearby
            if (rand < 0.4f) {
                o->oAction = BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR; // 40% chance
            } else if (rand < 0.8f) {
                o->oAction = BOWSER_ACT_SPIT_FIRE_INTO_SKY; // 80% chance
            } else {
                o->oAction = BOWSER_ACT_BREATH_FIRE;
            } // far away
        } else if (rand < 0.5f) {
            o->oAction = BOWSER_ACT_BIG_JUMP; // 50% chance
        } else {
            o->oAction = BOWSER_ACT_CHARGE_MARIO;
        }
    } else {
        // Keep walking
        o->oAction = BOWSER_ACT_WALK_TO_MARIO;
    }
}

static void original_bowser_act_breath_fire(void) {
    o->oForwardVel = 0.0f;
    if (o->oTimer == 0) {
        cur_obj_play_sound_2(SOUND_OBJ_BOWSER_INHALING);
    }
    // Init animation and return to default act after it ends
    if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_BREATH)) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

static void original_bowser_act_walk_to_mario(void) {
    s16 turnSpeed;
    s16 angleFromMario = abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario);

    // Set turning speed depending of the health
    // Also special case for BitFS
    if (o->oBehParams2ndByte == BOWSER_BP_BITFS) {
        turnSpeed = 0x400;
    } else { // BOWSER_BP_BitDW or BOWSER_BP_BitS
        if (o->oHealth >= 3) {
            turnSpeed = 0x400;
        } else if (o->oHealth == 2) {
            turnSpeed = 0x300;
        } else { // 1 health
            turnSpeed = 0x200;
        }
    }

    cur_obj_rotate_yaw_toward(o->oAngleToMario, turnSpeed);

    if (o->oSubAction == 0) {
        o->oBowserTimer = 0;
        // Start walking
        if (bowser_set_anim_look_up_and_walk()) {
            o->oSubAction++;
        }
    } else if (o->oSubAction == 1) {
        // Keep walking slowly
        if (bowser_set_anim_slow_gait()) {
            o->oBowserTimer++;
            // Reset fire sky status
            if (o->oBowserStatus & BOWSER_STATUS_FIRE_SKY) {
                if (o->oBowserTimer > 4) {
                    o->oBowserStatus &= ~BOWSER_STATUS_FIRE_SKY;
                }
                // Do subaction below if angles is less than 0x2000
            } else if (angleFromMario < 0x2000) {
                o->oSubAction++;
            }
        }
        // Stop walking and set to default action
    } else if (bowser_set_anim_look_down_stop_walk()) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

static void original_bowser_act_teleport(void) {
    switch (o->oSubAction) {
        // Set opacity target to invisible and become intangible
        case BOWSER_SUB_ACT_TELEPORT_START:
            cur_obj_become_intangible();
            o->oBowserTargetOpacity = 0;
            o->oBowserTimer = 30; // set timer value
            // Play sound effect
            if (o->oTimer == 0) {
                cur_obj_play_sound_2(SOUND_OBJ2_BOWSER_TELEPORT);
            }
            // Bowser is invisible, move angle to face Mario
            if (o->oOpacity == 0) {
                o->oSubAction++;
                o->oMoveAngleYaw = o->oAngleToMario;
            }
            break;

        case BOWSER_SUB_ACT_TELEPORT_MOVE:
            // reduce timer and set velocity teleport while at it
            if (o->oBowserTimer--) {
                o->oForwardVel = 100.0f;
            } else {
                o->oSubAction = BOWSER_SUB_ACT_TELEPORT_STOP;
                o->oMoveAngleYaw = o->oAngleToMario; // update angle
            }

            if (abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario) > 0x4000) {
                if (o->oDistanceToMario > 500.0f) {
                    o->oSubAction = BOWSER_SUB_ACT_TELEPORT_STOP;
                    o->oMoveAngleYaw = o->oAngleToMario; // update angle
                    cur_obj_play_sound_2(SOUND_OBJ2_BOWSER_TELEPORT);
                }
            }
            break;

        // Set opacity target to visible and become tangible
        case BOWSER_SUB_ACT_TELEPORT_STOP:
            o->oForwardVel = 0.0f; // reset velocity
            o->oBowserTargetOpacity = 255;
            // Set to default action once visible
            if (o->oOpacity == 255) {
                o->oAction = BOWSER_ACT_DEFAULT;
            }
            cur_obj_become_tangible();
            break;
    }
}

static void original_bowser_act_spit_fire_into_sky(void) {
    s32 animFrame;
    // Play animation
    cur_obj_init_animation_with_sound(BOWSER_ANIM_BREATH_UP);
    // Set frames
    animFrame = o->header.gfx.animInfo.animFrame;
    // Spawn flames in the middle of the animation
    if (animFrame > 24 && animFrame < 36) {
        cur_obj_play_sound_1(SOUND_AIR_BOWSER_SPIT_FIRE);
        if (animFrame == 35) { // Spawns Blue flames at this frame
            spawn_object_relative(1, 0, 400, 100, o, MODEL_RED_FLAME, bhvBlueBowserFlame);
        } else { // Spawns Red flames
            spawn_object_relative(0, 0, 400, 100, o, MODEL_RED_FLAME, bhvBlueBowserFlame);
        }
    }
    // Return to default act once the animation is over
    if (cur_obj_check_if_near_animation_end()) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
    // Set fire sky status
    o->oBowserStatus |= BOWSER_STATUS_FIRE_SKY;
}

static s32 original_bowser_set_anim_jump(void) {
    cur_obj_init_animation_with_sound(BOWSER_ANIM_JUMP_START);
    return cur_obj_check_anim_frame(11);
}

static s32 original_bowser_land(void) {
    if (o->oMoveFlags & OBJ_MOVE_LANDED) {
        o->oForwardVel = 0.0f;
        o->oVelY = 0.0f;
        spawn_mist_particles_variable(0, 0, 60.0f);
        cur_obj_init_animation_with_sound(BOWSER_ANIM_JUMP_STOP);
        o->header.gfx.animInfo.animFrame = 0;
        cur_obj_start_cam_event(o, CAM_EVENT_BOWSER_JUMP);
        // Set status attacks in BitDW since the other levels
        // have different attacks defined
        if (o->oBehParams2ndByte == BOWSER_BP_BITDW) {
            if (o->oDistanceToMario < 850.0f) {
                gMarioObject->oInteractStatus |= INT_STATUS_MARIO_KNOCKBACK_DMG;
            } else {
                gMarioObject->oInteractStatus |= INT_STATUS_MARIO_STUNNED;
            }
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

static void original_bowser_act_big_jump(void) {
    if (o->oSubAction == 0) {
        // Set jump animation
        if (bowser_set_anim_jump()) {
            // Set vel depending of the stage and status
            if (o->oBehParams2ndByte == BOWSER_BP_BITS && o->oBowserStatus & BOWSER_STATUS_BIG_JUMP) {
                o->oVelY = 70.0f;
            } else {
                o->oVelY = 80.0f;
            }
            o->oBowserTimer = 0;
            bowser_short_second_hop();
            o->oSubAction++;
        }
    } else if (o->oSubAction == 1) {
        // Reset Bowser back on stage in BitS if he doesn't land properly
        if (o->oBehParams2ndByte == BOWSER_BP_BITS && o->oBowserStatus & BOWSER_STATUS_BIG_JUMP) {
            bowser_reset_fallen_off_stage();
        }
        // Land on stage, reset status jump and velocity
        if (bowser_land()) {
            o->oBowserStatus &= ~BOWSER_STATUS_BIG_JUMP;
            o->oForwardVel = 0.0f;
            o->oSubAction++;
            // Spawn shockwave (BitS only) if is not on a platform
            bowser_spawn_shockwave();
            // Tilt platform in BitFS
            if (o->oBehParams2ndByte == BOWSER_BP_BITFS) {
                o->oAction = BOWSER_ACT_TILT_LAVA_PLATFORM;
            }
        }
        // Set to default action when the animation is over
    } else if (cur_obj_check_if_near_animation_end()) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

static void original_bowser_act_quick_jump(void) {
    if (o->oSubAction == 0) {
        // Set fixed val positions while jumping
        if (bowser_set_anim_jump()) {
            o->oVelY = 60.0f;
            o->oForwardVel = 50.0f;
            o->oBowserTimer = 0;
            o->oSubAction++;
        }
        // Lands then quickly returns to default action
    } else if (o->oSubAction == 1) {
        if (bowser_land()) {
            o->oSubAction++;
        }
    } else if (cur_obj_check_if_near_animation_end()) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

static void original_bowser_act_hit_edge(void) {
    // Reset speed and timer
    o->oForwardVel = 0.0f;
    if (o->oTimer == 0) {
        o->oBowserTimer = 0;
    }

    switch (o->oSubAction) {
        case 0:
            // Move on the edge
            cur_obj_init_animation_with_sound(BOWSER_ANIM_EDGE_MOVE);
            if (cur_obj_check_if_near_animation_end()) {
                o->oBowserTimer++;
            }
            if (o->oBowserTimer > 0) {
                o->oSubAction++;
            }
            break;

        case 1:
            // Stop moving on the edge
            cur_obj_init_animation_with_sound(BOWSER_ANIM_EDGE_STOP);
            // Turn around once the animation ends
            if (cur_obj_check_if_near_animation_end()) {
                o->oAction = BOWSER_ACT_TURN_FROM_EDGE;
            }
            break;
    }
}

static void original_bowser_act_spit_fire_onto_floor(void) {
    // Set fixed rand value if Mario is low health
    if (gHudDisplay.wedges < 4) {
        o->oBowserRandSplitFloor = 3;
    } else {
        o->oBowserRandSplitFloor = random_float() * 3.0f + 1.0f;
    }

    // Play animation and split fire at a specific frame
    cur_obj_init_animation_with_sound(BOWSER_ANIM_BREATH_QUICK);
    if (cur_obj_check_anim_frame(5)) {
        obj_spit_fire(0, 200, 180, 7.0f, MODEL_RED_FLAME, 30.0f, 10.0f, 0x1000);
    }
    // Use subaction as a timer when the animation is over
    if (cur_obj_check_if_near_animation_end()) {
        o->oSubAction++;
    }
    // Return to default act once we get past rand value
    if (o->oSubAction >= o->oBowserRandSplitFloor) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

static void original_bowser_act_turn_from_edge(void) {
    if (bowser_turn_on_timer(63, 0x200)) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

static void original_bowser_act_charge_mario(void) {
    s32 time;
    // Reset Speed to prepare charge
    if (o->oTimer == 0) {
        o->oForwardVel = 0.0f;
    }

    switch (o->oSubAction) {
        case BOWSER_SUB_ACT_CHARGE_START:
            // Start running
            o->oBowserTimer = 0;
            if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_RUN_START)) {
                o->oSubAction = BOWSER_SUB_ACT_CHARGE_RUN;
            }
            break;

        case BOWSER_SUB_ACT_CHARGE_RUN:
            // Set speed to run
            o->oForwardVel = 50.0f;
            if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_RUN)) {
                o->oBowserTimer++;
                // Split if 6 timer frames has passed
                if (o->oBowserTimer >= 6) {
                    o->oSubAction = BOWSER_SUB_ACT_CHARGE_SLIP;
                }
                // Slip if Mario has a differentiable angle and 2 timer frames has passed
                if (o->oBowserTimer >= 2) {
                    if (abs_angle_diff(o->oAngleToMario, o->oMoveAngleYaw) > 0x2000) {
                        o->oSubAction = BOWSER_SUB_ACT_CHARGE_SLIP;
                    }
                }
            }
            // Rotate to Mario
            cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x200);
            break;

        case BOWSER_SUB_ACT_CHARGE_SLIP:
            // Spawn smoke puff while slipping
            o->oBowserTimer = 0;
            cur_obj_init_animation_with_sound(BOWSER_ANIM_RUN_SLIP);
            spawn_object_relative_with_scale(0, 100, -50, 0, 3.0f, o, MODEL_SMOKE, bhvWhitePuffSmoke2);
            spawn_object_relative_with_scale(0, -100, -50, 0, 3.0f, o, MODEL_SMOKE, bhvWhitePuffSmoke2);
            // End Charge once Bowser stops running
            if (approach_f32_signed(&o->oForwardVel, 0, -1.0f)) {
                o->oSubAction = BOWSER_SUB_ACT_CHARGE_END;
            }
            cur_obj_extend_animation_if_at_end();
            break;

        case BOWSER_SUB_ACT_CHARGE_END:
            // Stop running
            o->oForwardVel = 0.0f;
            cur_obj_init_animation_with_sound(BOWSER_ANIM_RUN_STOP);
            if (cur_obj_check_if_near_animation_end()) {
                // Set time delay to go to default action
                if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
                    time = 10;
                } else {
                    time = 30;
                }
                if (o->oBowserTimer > time) {
                    o->oAction = BOWSER_ACT_DEFAULT;
                }
                o->oBowserTimer++;
            }
            cur_obj_extend_animation_if_at_end();
            break;
    }

    // Bowser is close to falling so set hit edge action
    if (o->oMoveFlags & OBJ_MOVE_HIT_EDGE) {
        o->oAction = BOWSER_ACT_HIT_EDGE;
    }
}
