static void original_bhv_bowser_flame_spawn_loop(void);
static s32 original_bowser_spawn_shockwave(void);
static void original_bowser_bounce_effects(s32 *timer);
static void original_bowser_bitdw_actions(void);
static void original_bowser_bitfs_actions(void);
static void original_bowser_bits_action_list(void);
static void original_bowser_act_breath_fire(void);
static void original_bowser_act_walk_to_mario(void);
static void original_bowser_act_teleport(void);
static void original_bowser_act_spit_fire_into_sky(void);
static s32 original_bowser_set_anim_jump(void);
static s32 original_bowser_land(void);
static void original_bowser_act_big_jump(void);
static void original_bowser_act_quick_jump(void);
static void original_bowser_act_hit_edge(void);
static void original_bowser_act_spit_fire_onto_floor(void);
static void original_bowser_act_turn_from_edge(void);
static void original_bowser_act_charge_mario(void);

#include "game/bowser_arena.h"
#include "game/bowser_throw.h"
// bowser.inc.c

/**
 * Behavior for Bowser and it's actions (Tail, Flame, Body)
 */

// Bowser's Tail

/**
 * Checks whenever the Bowser and his tail should be intangible or not
 * By default it starts tangible
 */
void bowser_tail_anchor_act_default(void) {
    struct Object *bowser = o->parentObj;
    cur_obj_become_tangible();
    cur_obj_scale(1.0f);

    if (bowser->oBowserGrabCooldown != 0 || bowser->oHeldState != HELD_FREE
        || bowser->oAction == BOWSER_ACT_THROWN || bowser->oAction == BOWSER_ACT_HIT_MINE
        || bowser->oAction == BOWSER_ACT_JUMP_ONTO_STAGE
        || bowser->oAction == BOWSER_ACT_TILT_LAVA_PLATFORM || bowser->oAction == BOWSER_ACT_TELEPORT) {
        // Keep the grab region closed throughout release and recovery.
        bowser->oIntangibleTimer = -1;
    } else if (obj_check_if_collided_with_object(o, gMarioObject)) {
        // Tail overlap opens Bowser's native grab region.
        bowser->oIntangibleTimer = 0;
        // Keep checking overlap on every update; leaving the tail closes the grab region.
    } else {
        bowser->oIntangibleTimer = -1;
    }
}

/**
 * Reopen tail overlap checks after Bowser has recovered and faced Mario.
 */
void bowser_tail_anchor_thrown(void) {
    if (o->parentObj->oBowserGrabCooldown == 0) {
        o->oAction = BOWSER_ACT_TAIL_DEFAULT;
    }
}

/**
 * Makes the tail intangible so Mario can grab it
 */
void bowser_tail_anchor_act_touched_mario(void) {
    // Return to main action when Bowser tilts BitFS platform
    if (o->parentObj->oAction == BOWSER_ACT_TILT_LAVA_PLATFORM) {
        o->parentObj->oIntangibleTimer = -1;
        o->oAction = BOWSER_ACT_TAIL_DEFAULT;
    }
    cur_obj_become_intangible();
}

ObjActionFunc sBowserTailAnchorActions[] = {
    bowser_tail_anchor_act_default,
    bowser_tail_anchor_thrown,
    bowser_tail_anchor_act_touched_mario,
};

/**
 * Bowser's tail main loop
 */
void bhv_bowser_tail_anchor_loop(void) {
    // Call its actions
    cur_obj_call_action_function(sBowserTailAnchorActions);
    // Position the tail
    o->oParentRelativePosX = 90.0f;

    // Make it intangible while Bowser is dead
    if (o->parentObj->oAction == BOWSER_ACT_DEAD) {
        o->parentObj->oIntangibleTimer = -1;
    }

    o->oInteractStatus = INT_STATUS_NONE;
}

// Bowser's Flame

/**
 * Bowser's Flame spawn main loop
 */
void bhv_bowser_flame_spawn_loop(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bhv_bowser_flame_spawn_loop();
        return;
    }

    struct Object *bowser = o->parentObj;
    s32 animFrame;
    f32 posX;
    f32 posZ;
    f32 cossYaw = coss(bowser->oMoveAngleYaw);
    f32 sinsYaw = sins(bowser->oMoveAngleYaw);
    s16 *data = segmented_to_virtual(dBowserFlamesOrientationValues);

    // Check for Bowser breathing animation
    if (bowser->oSoundStateID == BOWSER_ANIM_BREATH) {

        // Start counting anim frames then reset it when it ends
        animFrame = bowser->header.gfx.animInfo.animFrame + 1.0f;
        if (bowser->header.gfx.animInfo.curAnim->loopEnd == animFrame) {
            animFrame = 0;
        }

        s32 fightThree = bowser->oBehParams2ndByte == BOWSER_BP_BITS;
        if (fightThree) {
            animFrame = bowser->oBowserTimer >= 16 && bowser->oBowserTimer <= 45
                            ? 46 + bowser->oBowserTimer - 16
                            : 0;
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
            if ((fightThree || !(animFrame & 1)) && o->oSubAction != animFrame) {
                o->oSubAction = animFrame;
                if (fightThree && (bowser->oBowserTimer - 15) % 4 == 0) {
                    struct Object *flame = spawn_object(o, MODEL_BLUE_FLAME, bhvBlueBowserFlame);
                    flame->oBehParams2ndByte = 1;
                    flame->oBehParams |= BOWSER_FLAME_NO_SPLIT;
                } else {
                    spawn_object(o, MODEL_RED_FLAME, bhvFlameMovingForwardGrowing);
                }
            }
        } else {
            o->oSubAction = -1;
        }
    } else {
        o->oSubAction = -1;
    }
}

/**
 * Bowser's Body main loop
 */
void bhv_bowser_body_anchor_loop(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        o->oInteractionSubtype &= ~INT_SUBTYPE_BOWSER_ATTACK;
        o->oDamageOrCoinValue = 2;
    }
    // Copy position and angles from Bowser itself
    obj_copy_pos_and_angle(o, o->parentObj);
    // If Bowser is dead, set interaction type to text
    // so that he can be ready to speak his dialog
    if (o->parentObj->oAction == BOWSER_ACT_DEAD) {
        // Clear interaction type at the last sub action in BitS
        // Fixes collision coliding after defeating him
        if (o->parentObj->oSubAction == BOWSER_SUB_ACT_DEAD_FINAL_END_OVER) {
            o->oInteractType = 0;
        } else {
            o->oInteractType = INTERACT_TEXT;
        }
    } else {
        // Do damage if Mario touches Bowser
        o->oInteractType = INTERACT_DAMAGE;
        // Make body intangible while is transparent
        // in BitFS (Teleporting)
        if (o->parentObj->oOpacity < 100) {
            cur_obj_become_intangible();
        } else {
            cur_obj_become_tangible();
        }
    }
    // Make body intangible while Bowser is getting grabbed
    if (o->parentObj->oHeldState != HELD_FREE) {
        cur_obj_become_intangible();
    }
    o->oInteractStatus = INT_STATUS_NONE;
}

/**
 * Bowser's shockwave attack, spawns only in BitS
 */
s32 bowser_spawn_shockwave(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        return original_bowser_spawn_shockwave();
    }

    if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
        s32 waveCount = 1;
        if (o->oAction == BOWSER_ACT_BIG_JUMP) {
            waveCount = 3;
        } else if (o->oAction == BOWSER_ACT_JUMP_ONTO_STAGE) {
            waveCount = 2;
        }
        for (s32 waveIndex = 0; waveIndex < waveCount; waveIndex++) {
            struct Object *wave = spawn_object(o, MODEL_BOWSER_WAVE, bhvBowserShockWave);
            if (wave != NULL) {
                wave->oPosY = o->oFloorHeight;
                if (waveCount > 1) {
                    s32 lightCount = 1;
                    if (waveCount == 3 && !(o->oBowserStatus & BOWSER_STATUS_BIG_JUMP)) {
                        lightCount = 2;
                    }
                    wave->oBehParams2ndByte =
                        waveIndex < lightCount ? BOWSER_SHOCKWAVE_LIGHT : BOWSER_SHOCKWAVE_HEAVY;
                    wave->oBowserTimer = waveIndex * BOWSER_SHOCKWAVE_DELAY_FRAMES;
                }
            }
        }
        return TRUE;
    }
    return FALSE;
}

/**
 * Misc effects that Bowser plays when he lands with drastic actions
 * Plays step sound, spawns particles and changes camera event
 */
void bowser_bounce_effects(s32 *timer) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_bounce_effects(timer);
        return;
    }

    if (o->oMoveFlags & OBJ_MOVE_LANDED) {
        if (*timer == 0 && (o->oAction == BOWSER_ACT_HIT_MINE || o->oAction == BOWSER_ACT_DEAD)) {
            bowser_arena_landed(o);
            if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
                mario_queue_bowser_impact(o);
            }
        }
        if (*timer == 0 && o->oBehParams2ndByte != BOWSER_BP_BITDW
            && (o->oAction == BOWSER_ACT_HIT_MINE || o->oAction == BOWSER_ACT_DEAD)) {
            s32 waveCount = o->oBehParams2ndByte == BOWSER_BP_BITS ? 2 : 1;
            for (s32 waveIndex = 0; waveIndex < waveCount; waveIndex++) {
                struct Object *wave = spawn_object(o, MODEL_BOWSER_WAVE, bhvBowserShockWave);
                if (wave != NULL) {
                    wave->oBehParams2ndByte =
                        waveIndex == 0 ? BOWSER_SHOCKWAVE_LIGHT : BOWSER_SHOCKWAVE_DELAYED;
                    wave->oPosY = o->oFloorHeight;
                }
            }
        }
        (*timer)++;
        if (*timer < 4) {
            cur_obj_start_cam_event(o, CAM_EVENT_BOWSER_THROW_BOUNCE);
            spawn_mist_particles_variable(o->oBehParams2ndByte == BOWSER_BP_BITS ? 1 : 0, 0, 60.0f);
            cur_obj_play_sound_2(SOUND_OBJ_BOWSER_WALK);
        }
    }
}

/**
 * Makes Bowser look up and walk on an specific animation frame
 * Returns TRUE if the animation is almost over
 */
s32 bowser_set_anim_look_up_and_walk(void) {
    cur_obj_init_animation_with_sound(BOWSER_ANIM_LOOK_UP_START_WALK);
    if (cur_obj_check_anim_frame(21)) {
        o->oForwardVel = 3.0f;
    }
    return cur_obj_check_if_near_animation_end();
}

/**
 * Makes Bowser do a slow gait (or slow walk)
 * Returns TRUE if the animation is almost over
 */
s32 bowser_set_anim_slow_gait(void) {
    o->oForwardVel = 3.0f;
    cur_obj_init_animation_with_sound(BOWSER_ANIM_SLOW_GAIT);
    return cur_obj_check_if_near_animation_end();
}

/**
 * Makes Bowser look down and stop on an specific animation frame
 * Returns TRUE if the animation is almost over
 */
s32 bowser_set_anim_look_down_stop_walk(void) {
    cur_obj_init_animation_with_sound(BOWSER_ANIM_LOOK_DOWN_STOP_WALK);
    if (cur_obj_check_anim_frame(20)) {
        o->oForwardVel = 0.0f;
    }
    return cur_obj_check_if_near_animation_end();
}

/**
 * Set Bowser an action depending of the CamAct value
 * CamAct changes value on the cutscene itself (cutscene_bowser_arena)
 */
void bowser_init_camera_actions(void) {
    if (o->oBowserCamAct == BOWSER_CAM_ACT_IDLE) {
        o->oAction = BOWSER_ACT_WAIT;
    } else if (o->oBowserCamAct == BOWSER_CAM_ACT_WALK) {
        o->oAction = BOWSER_ACT_INTRO_WALK;
    // Start with a big jump in BitFS to do a platform tilt
    } else if (o->oBehParams2ndByte == BOWSER_BP_BITFS) {
        o->oAction = BOWSER_ACT_BIG_JUMP;
    } else {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

/**
 * Bowser's idle action that plays when he is initialized
 * or the CamAct gets in idle mode
 */
void bowser_act_wait(void) {
    o->oForwardVel = 0.0f;
    cur_obj_init_animation_with_sound(BOWSER_ANIM_IDLE);
    bowser_init_camera_actions();
}

/**
 * Bowser's cutscene walk that last a few seconds to introduce itself
 * Do subactions until the animation ends, then go to next subaction
 */
void bowser_act_intro_walk(void) {
    // First look up and walk
    if (o->oSubAction == 0) {
        if (bowser_set_anim_look_up_and_walk()) {
            o->oSubAction++;
        }
    // Then slowly walk
    } else if (o->oSubAction == 1) {
        if (bowser_set_anim_slow_gait()) {
            o->oSubAction++;
        }
    // And finally stop, and set to wait mode
    } else if (bowser_set_anim_look_down_stop_walk()) {
        if (o->oBowserCamAct == BOWSER_CAM_ACT_WALK) {
            o->oBowserCamAct = BOWSER_CAM_ACT_IDLE;
        }
        bowser_init_camera_actions();
    }
}

/**
 * List of actions to debug Bowser
 */
s8 sBowserDebugActions[] = {
    BOWSER_ACT_CHARGE_MARIO,
    BOWSER_ACT_SPIT_FIRE_INTO_SKY,
    BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR,
    BOWSER_ACT_HIT_MINE,
    BOWSER_ACT_BIG_JUMP,
    BOWSER_ACT_WALK_TO_MARIO,
    BOWSER_ACT_BREATH_FIRE,
    BOWSER_ACT_DEAD,
    BOWSER_ACT_DANCE,
    BOWSER_ACT_TELEPORT,
    BOWSER_ACT_QUICK_JUMP,
    BOWSER_ACT_TILT_LAVA_PLATFORM,
    BOWSER_ACT_DANCE,
    BOWSER_ACT_DANCE,
    BOWSER_ACT_DANCE,
    BOWSER_ACT_DANCE,
};

/**
 * Debug function that allows to change Bowser's actions (most of them)
 */
UNUSED static void bowser_debug_actions(void) {
    if (gDebugInfo[DEBUG_PAGE_ENEMYINFO][1] != 0) {
        o->oAction = sBowserDebugActions[gDebugInfo[DEBUG_PAGE_ENEMYINFO][2] & 0x0F];
        gDebugInfo[DEBUG_PAGE_ENEMYINFO][1] = 0;
    }
}

/**
 * Set actions (and attacks) for Bowser in "Bowser in the Dark World"
 */
void bowser_bitdw_actions(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_bitdw_actions();
        return;
    }

    // Generate random float
    f32 rand = random_float();
    // Set attacks when Bowser Reacts
    if (!o->oBowserIsReacting) {
        if (o->oBowserStatus & BOWSER_STATUS_ANGLE_MARIO) {
            if (o->oDistanceToMario < 1500.0f) { // nearby
                o->oAction = rand < 0.7f ? BOWSER_ACT_BREATH_FIRE : BOWSER_ACT_QUICK_JUMP;
            } else { // far away
                o->oAction = rand < 0.7f ? BOWSER_ACT_QUICK_JUMP : BOWSER_ACT_TELEPORT;
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
            if (rand < 0.05f) {
                o->oAction = BOWSER_ACT_DANCE; // 5% chance
            } else if (rand < 0.60f) {
                o->oAction = BOWSER_ACT_WALK_TO_MARIO; // 55% chance
            } else {
                o->oAction = BOWSER_ACT_TELEPORT; // 40% chance
            }
        } else {
            o->oAction = BOWSER_ACT_WALK_TO_MARIO;
        }
    }
}

/**
 * Set actions (and attacks) for Bowser in "Bowser in the Fire Sea"
 */
void bowser_bitfs_actions(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_bitfs_actions();
        return;
    }

    f32 rand = random_float();
    if (!o->oBowserIsReacting) {
        if (o->oBowserStatus & BOWSER_STATUS_ANGLE_MARIO) {
            if (o->oDistanceToMario < 1300.0f) {
                if (rand < 0.4f) {
                    o->oAction = BOWSER_ACT_TELEPORT;
                } else if (rand < 0.6f) {
                    o->oAction = BOWSER_ACT_BREATH_FIRE;
                } else if (rand < 0.9f) {
                    o->oAction = BOWSER_ACT_QUICK_JUMP;
                } else {
                    o->oAction = BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR;
                }
            } else if (500.0f < o->oBowserDistToCenter && o->oBowserDistToCenter < 1500.0f) {
                if (rand < 0.4f) {
                    o->oAction = BOWSER_ACT_BIG_JUMP;
                } else if (rand < 0.6f) {
                    o->oAction = BOWSER_ACT_CHARGE_MARIO;
                } else if (rand < 0.7f) {
                    o->oAction = BOWSER_ACT_TELEPORT;
                } else if (rand < 0.8f) {
                    o->oAction = BOWSER_ACT_QUICK_JUMP;
                } else {
                    o->oAction = BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR;
                }
            } else {
                if (rand < 0.6f) {
                    o->oAction = BOWSER_ACT_CHARGE_MARIO;
                } else if (rand < 0.75f) {
                    o->oAction = BOWSER_ACT_BIG_JUMP;
                } else {
                    o->oAction = BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR;
                }
            }
        } else {
            o->oAction = BOWSER_ACT_WALK_TO_MARIO;
        }
        o->oBowserIsReacting = TRUE;
    } else {
        o->oBowserIsReacting = FALSE;
        if (rand < 0.55f) {
            o->oAction = BOWSER_ACT_TELEPORT;
        } else if (rand < 0.95f) {
            o->oAction = BOWSER_ACT_WALK_TO_MARIO;
        } else {
            o->oAction = BOWSER_ACT_DANCE;
        }
    }
}

/**
 * List of actions (and attacks) for "Bowser in the Sky"
 */
void bowser_bits_action_list(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_bits_action_list();
        return;
    }

    f32 rand = random_float();
    if (o->oBowserStatus & BOWSER_STATUS_ANGLE_MARIO) {
        if (o->oDistanceToMario < 1000.0f) {
            if (rand < 0.60f) {
                o->oAction = BOWSER_ACT_TELEPORT;
            } else if (rand < 0.65f) {
                o->oAction = BOWSER_ACT_BREATH_FIRE;
            } else if (rand < 0.85f) {
                o->oAction = BOWSER_ACT_QUICK_JUMP;
            } else if (rand < 0.90f) {
                o->oAction = BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR;
            } else {
                o->oAction = BOWSER_ACT_SPIT_FIRE_INTO_SKY;
            }
        } else if (rand < 0.35f) {
            o->oAction = BOWSER_ACT_TELEPORT;
        } else if (rand < 0.40f) {
            o->oAction = BOWSER_ACT_QUICK_JUMP;
        } else if (rand < 0.80f) {
            o->oAction = BOWSER_ACT_CHARGE_MARIO;
        } else {
            o->oAction = BOWSER_ACT_BIG_JUMP;
        }
    } else {
        // Keep walking
        o->oAction = BOWSER_ACT_WALK_TO_MARIO;
    }
}

/**
 * Sets big jump action, not much to say
 * Never gets called since oBowserBitsJustJump is always FALSE
 */
void bowser_set_act_big_jump(void) {
    o->oAction = BOWSER_ACT_BIG_JUMP;
}

/**
 * Set actions (and attacks) for Bowser in "Bowser in the Sky"
 */
void bowser_bits_actions(void) {
    switch (o->oBowserIsReacting) {
        case FALSE:
            // oBowserBitsJustJump never changes value,
            // so its always FALSE, maybe a debug define
            if (o->oBowserBitsJustJump == FALSE) {
                bowser_bits_action_list();
            } else {
                bowser_set_act_big_jump();
            }
            o->oBowserIsReacting = TRUE;
            break;

        case TRUE:
            o->oBowserIsReacting = FALSE;
            o->oAction = BOWSER_ACT_WALK_TO_MARIO;
            break;
    }
}

/**
 * Reset Bowser position and speed if he wasn't able to land properly on stage
 */
void bowser_reset_fallen_off_stage(void) {
    if (o->oVelY < 0.0f && o->oPosY < (o->oHomeY - 300.0f)) {
        o->oPosX = o->oPosZ = 0.0f;
        o->oPosY = o->oHomeY + 2000.0f;
        o->oVelY = 0.0f;
        o->oForwardVel = 0.0f;
    }
}

/**
 * Unused, makes Bowser be in idle and after it returns to default action
 */
void bowser_act_idle(void) {
    if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_IDLE)) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

/**
 * Default Bowser act that doesn't last very long
 */
void bowser_act_default(void) {
    // Set eye state
    o->oBowserEyesShut = FALSE;
    // Set idle animation
    cur_obj_init_animation_with_sound(BOWSER_ANIM_IDLE);
    // Stop him still
    o->oAngleVelYaw = 0;
    o->oForwardVel = 0.0f;
    o->oVelY = 0.0f;
    // Set level specific actions
    if (o->oBehParams2ndByte == BOWSER_BP_BITDW) {
        bowser_bitdw_actions();
    } else if (o->oBehParams2ndByte == BOWSER_BP_BITFS) {
        bowser_bitfs_actions();
    } else { // BOWSER_BP_BITS
        bowser_bits_actions();
    }
}

/**
 * Makes Bowser play breath animation and sound effect
 * The actual breath attack is in bhv_bowser_flame_spawn_loop
 * called as a child obj behavior in Bowser
 */
void bowser_act_breath_fire(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_act_breath_fire();
        return;
    }

    o->oForwardVel = 0.0f;
    if (o->oTimer == 0) {
        cur_obj_play_sound_2(SOUND_OBJ_BOWSER_INHALING);
    }
    if (o->oBehParams2ndByte == BOWSER_BP_BITFS) {
        if (o->oTimer == 0) {
            o->oBowserTimer = 0;
        }
        s32 frame = ++o->oBowserTimer;
        cur_obj_init_animation_with_accel_and_sound(BOWSER_ANIM_BREATH, 1.0f);
        if (frame <= 28) {
            o->header.gfx.animInfo.animFrame = frame * 45 / 28 - 1;
        } else if (frame <= 67) {
            // Preserve all twenty shots on animation frames 45, 47, ... 83.
            o->header.gfx.animInfo.animFrame = frame + 16;
        } else {
            o->header.gfx.animInfo.animFrame = 84 + (frame - 68) * 6 / 4;
            if (frame >= 72) {
                o->oAction = BOWSER_ACT_DEFAULT;
            }
        }
        o->header.gfx.animInfo.animFrameAccelAssist = o->header.gfx.animInfo.animFrame * 0x10000;
        return;
    }
    if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
        if (o->oTimer == 0) {
            o->oBowserTimer = 0;
        }
        s32 frame = ++o->oBowserTimer;
        cur_obj_init_animation_with_accel_and_sound(BOWSER_ANIM_BREATH, 1.0f);
        if (frame <= 15) {
            o->header.gfx.animInfo.animFrame = frame * 45 / 15 - 1;
        } else if (frame <= 45) {
            o->header.gfx.animInfo.animFrame = 45 + (frame - 16) * 38 / 29;
        } else {
            o->header.gfx.animInfo.animFrame = 84 + (frame - 46) * 3;
            if (frame >= 48) {
                o->oAction = BOWSER_ACT_DEFAULT;
            }
        }
        o->header.gfx.animInfo.animFrameAccelAssist = o->header.gfx.animInfo.animFrame * 0x10000;
        return;
    }
    // Init animation and return to default act after it ends
    s32 animFrame = o->header.gfx.animInfo.animFrame;
    f32 animSpeed = 1.0f;
    if (o->oTimer != 0 && animFrame >= 45 && animFrame < 85) {
        animSpeed = BOWSER_BREATH_FIRE_ANIM_SPEED;
    }
    cur_obj_init_animation_with_accel_and_sound(BOWSER_ANIM_BREATH, animSpeed);
    if (cur_obj_check_if_near_animation_end()) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

/**
 * Makes Bowser walk towards Mario
 */
static s16 bowser_walk_turn_speed(void) {
    s32 fight = o->oBehParams2ndByte;
    f32 base;
    s32 initialHealth;
    f32 gain = 0.1f;
    switch (fight) {
        case BOWSER_BP_BITFS:
            base = 6.0f;
            initialHealth = 4;
            break;
        case BOWSER_BP_BITS:
            base = 6.4f;
            initialHealth = 5;
            break;
        default:
            base = 5.625f;
            initialHealth = 3;
            break;
    }
    return DEGREES(base + MAX(0, initialHealth - o->oHealth) * gain);
}

void bowser_act_walk_to_mario(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_act_walk_to_mario();
        return;
    }

    if ((o->oBehParams2ndByte == BOWSER_BP_BITFS && random_float() < 1.0f / 300.0f)
        || (o->oBehParams2ndByte == BOWSER_BP_BITS && !(o->oBowserStatus & BOWSER_STATUS_ANGLE_MARIO)
            && random_float() < 1.0f / 600.0f)) {
        o->oForwardVel = 0.0f;
        o->oAction = o->oBehParams2ndByte == BOWSER_BP_BITFS || random_float() < 0.9f
                         ? BOWSER_ACT_TELEPORT
                         : BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR;
        return;
    }
    s16 turnSpeed = bowser_walk_turn_speed();
    s16 angleFromMario = abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario);

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
    if (o->oForwardVel > 0.0f) {
        f32 multiplier = 1.0f;
        if (angleFromMario == 0) {
            if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
                multiplier = 3.0f;
            } else if (o->oBehParams2ndByte == BOWSER_BP_BITFS) {
                multiplier = 2.0f;
            }
        }
        o->oForwardVel = 3.0f * multiplier;
    }
}

/**
 * Makes Bowser teleport while invisible
 */
static s32 bowser_teleport_to_mario(void) {
    f32 choice = random_float();
    f32 offset;
    if (o->oBehParams2ndByte == BOWSER_BP_BITFS) {
        if (ABS(gMarioState->forwardVel) < 25.0f) {
            if (choice < 0.6f) {
                offset = 0.0f;
            } else if (choice < 0.9f) {
                offset = 400.0f;
            } else {
                offset = -400.0f;
            }
        } else {
            offset = choice < 0.6f ? 400.0f : -400.0f;
        }
    } else if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
        // Thirteen hidden updates plus three fade updates before the body can damage Mario.
        f32 distance = MAX(200.0f, ABS(gMarioState->forwardVel) * 16.0f);
        if (ABS(gMarioState->forwardVel) < 30.0f) {
            if (choice < 0.6f) {
                offset = 0.0f;
            } else if (choice < 0.9f) {
                offset = distance;
            } else {
                offset = -distance;
            }
        } else {
            if (choice < 0.2f) {
                offset = 0.0f;
            } else if (choice < 0.7f) {
                offset = distance;
            } else {
                offset = -distance;
            }
        }
    } else if (ABS(gMarioState->forwardVel) < 20.0f) {
        if (choice < 0.5f) {
            offset = 0.0f;
        } else if (choice < 0.75f) {
            offset = 300.0f;
        } else {
            offset = -300.0f;
        }
    } else {
        offset = choice < 0.5f ? 300.0f : -300.0f;
    }
    f32 x = gMarioState->pos[0] + sins(gMarioState->faceAngle[1]) * offset;
    f32 z = gMarioState->pos[2] + coss(gMarioState->faceAngle[1]) * offset;
    struct Surface *floor;
    f32 y = find_floor(x, gMarioState->pos[1] + 1000.0f, z, &floor);
    // Unsafe offsets fall back to Mario's support, rather than a pit or lava.
    if (floor == NULL || floor->type == SURFACE_BURNING || floor->type == SURFACE_DEATH_PLANE) {
        x = gMarioState->pos[0];
        z = gMarioState->pos[2];
        y = find_floor(x, gMarioState->pos[1] + 1000.0f, z, &floor);
    }
    if (floor != NULL && floor->type != SURFACE_BURNING && floor->type != SURFACE_DEATH_PLANE) {
        o->oPosX = x;
        o->oPosY = y;
        o->oPosZ = z;
        o->oVelY = 0.0f;
        o->oMoveFlags = OBJ_MOVE_ON_GROUND;
        o->oFloor = floor;
        o->oFloorHeight = y;
        o->oFloorType = floor->type;
        o->oMoveAngleYaw = obj_angle_to_object(o, gMarioObject);
        return TRUE;
    }
    return FALSE;
}

void bowser_act_teleport(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_act_teleport();
        return;
    }

    o->oForwardVel = 0.0f;
    // Ground bounciness leaves a small positive Y speed even while standing.
    if (o->oFloor == NULL || !(o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) || o->oVelY > 5.0f
        || ABS(o->oPosY - o->oFloorHeight) > 1.0f) {
        o->oAction = BOWSER_ACT_DEFAULT;
        o->oBowserTargetOpacity = 255;
        cur_obj_become_tangible();
        return;
    }
    switch (o->oSubAction) {
        case BOWSER_SUB_ACT_TELEPORT_START:
            cur_obj_become_intangible();
            o->oBowserTargetOpacity = 0;
            if (o->oBehParams2ndByte == BOWSER_BP_BITFS) {
                o->oBowserTimer = 14;
            } else if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
                o->oBowserTimer = 13;
            } else {
                o->oBowserTimer = 15;
            }
            if (o->oTimer == 0) {
                cur_obj_play_sound_2(SOUND_OBJ2_BOWSER_TELEPORT);
            }
            if (o->oOpacity == 0) {
                if (bowser_teleport_to_mario()) {
                    o->oSubAction = BOWSER_SUB_ACT_TELEPORT_MOVE;
                } else {
                    o->oAction = BOWSER_ACT_DEFAULT;
                    o->oBowserTargetOpacity = 255;
                    cur_obj_become_tangible();
                }
            }
            break;

        case BOWSER_SUB_ACT_TELEPORT_MOVE:
            if (--o->oBowserTimer <= 0) {
                o->oSubAction = BOWSER_SUB_ACT_TELEPORT_STOP;
                cur_obj_play_sound_2(SOUND_OBJ2_BOWSER_TELEPORT);
            }
            break;

        case BOWSER_SUB_ACT_TELEPORT_STOP:
            o->oBowserTargetOpacity = 255;
            if (o->oOpacity == 255) {
                o->oAction = BOWSER_ACT_DEFAULT;
            }
            cur_obj_become_tangible();
            break;
    }
}

/**
 * Makes Bowser do a fire split into the sky
 */
void bowser_act_spit_fire_into_sky(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_act_spit_fire_into_sky();
        return;
    }

    s32 animFrame;
    if (o->oTimer == 0) {
        o->oBowserTimer = -1;
    }
    if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
        s32 frame = o->oTimer + 1;
        cur_obj_init_animation_with_accel_and_sound(BOWSER_ANIM_BREATH_UP, 1.0f);
        if (frame <= 14) {
            animFrame = frame * 25 / 14 - 1;
        } else if (frame <= 69) {
            s32 shot = (frame - 15) / 6;
            animFrame = 25 + shot;
            if ((frame - 15) % 6 == 0 && o->oBowserTimer != shot) {
                o->oBowserTimer = shot;
                cur_obj_play_sound_1(SOUND_AIR_BOWSER_SPIT_FIRE);
                spawn_object_relative(shot == 9, 0, 400, 100, o, MODEL_RED_FLAME, bhvBlueBowserFlame);
            }
        } else {
            animFrame = 36 + (frame - 70) * 22 / 9;
            if (frame >= 79) {
                o->oAction = BOWSER_ACT_DEFAULT;
            }
        }
        o->header.gfx.animInfo.animFrame = animFrame;
        o->header.gfx.animInfo.animFrameAccelAssist = animFrame * 0x10000;
        o->oBowserStatus |= BOWSER_STATUS_FIRE_SKY;
        return;
    }
    animFrame = o->header.gfx.animInfo.animFrame;
    f32 animSpeed = 1.0f;
    if (o->oTimer != 0 && animFrame > 24 && animFrame < 36) {
        animSpeed = BOWSER_SKY_FIRE_ANIM_SPEED;
    }
    cur_obj_init_animation_with_accel_and_sound(BOWSER_ANIM_BREATH_UP, animSpeed);
    animFrame = o->header.gfx.animInfo.animFrame;
    // Spawn flames in the middle of the animation
    if (animFrame > 24 && animFrame < 36) {
        cur_obj_play_sound_1(SOUND_AIR_BOWSER_SPIT_FIRE);
        // Slower animation holds each frame; emit only once on that frame.
        if (o->oBowserTimer != animFrame) {
            o->oBowserTimer = animFrame;
            s32 blueFlame = animFrame == 35;
            spawn_object_relative(blueFlame, 0, 400, 100, o, MODEL_RED_FLAME, bhvBlueBowserFlame);
        }
    }
    // Return to default act once the animation is over
    if (cur_obj_check_if_near_animation_end()) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
    // Set fire sky status
    o->oBowserStatus |= BOWSER_STATUS_FIRE_SKY;
}

/**
 * Flips Bowser back on stage if he hits a mine with more than 1 health
 */
void bowser_act_hit_mine(void) {
    // Similar vel values from bowser_fly_back_dead
    if (o->oTimer == 0) {
        o->oForwardVel = -400.0f;
        o->oVelY = 100.0f;
        o->oMoveAngleYaw = o->oBowserAngleToCenter + 0x8000;
        o->oBowserEyesShut = TRUE; // close eyes
    }
    // Play flip animation
    if (o->oSubAction == BOWSER_SUB_ACT_HIT_MINE_START) {
        cur_obj_init_animation_with_sound(BOWSER_ANIM_FLIP);
        o->oSubAction++;
        o->oBowserTimer = 0;
    // Play flip animation again, extend it and play bounce effects
    } else if (o->oSubAction == BOWSER_SUB_ACT_HIT_MINE_FALL) {
        cur_obj_init_animation_with_sound(BOWSER_ANIM_FLIP);
        cur_obj_extend_animation_if_at_end();
        bowser_bounce_effects(&o->oBowserTimer);
        // Reset vel and stand up
        if (o->oBowserTimer > 2) {
            cur_obj_init_animation_with_sound(BOWSER_ANIM_STAND_UP_FROM_FLIP);
            o->oVelY = 0.0f;
            o->oForwardVel = 0.0f;
            o->oSubAction++;
        }
    // Play these actions once he is stand up
    } else if (o->oSubAction == BOWSER_SUB_ACT_HIT_MINE_STOP) {
        if (cur_obj_check_if_near_animation_end()) {
            // Makes Bowser dance at one health (in BitS)
            if (o->oHealth == 1) {
                o->oAction = BOWSER_ACT_DANCE;
            } else {
                o->oAction = BOWSER_ACT_DEFAULT;
            }
            o->oBowserEyesShut = FALSE; // open eyes
        }
    }
}

/**
 * Makes Bowser do his jump start animation
 * Returns TRUE on the middle of the jump
 */
s32 bowser_set_anim_jump(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        return original_bowser_set_anim_jump();
    }

    cur_obj_init_animation_with_sound(BOWSER_ANIM_JUMP_START);
    return cur_obj_check_anim_frame(o->oBehParams2ndByte == BOWSER_BP_BITS ? 8 : 10);
}

/**
 * Reset speed, play jump stop animation and do attacks in BitDW
 * Returns TRUE when Bowser lands
 */
s32 bowser_land(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        return original_bowser_land();
    }

    if (o->oMoveFlags & OBJ_MOVE_LANDED) {
        o->oForwardVel = 0.0f;
        o->oVelY = 0.0f;
        spawn_mist_particles_variable(0, 0, 60.0f);
        cur_obj_init_animation_with_sound(BOWSER_ANIM_JUMP_STOP);
        o->header.gfx.animInfo.animFrame = 0;
        cur_obj_start_cam_event(o, CAM_EVENT_BOWSER_JUMP);
        // The landing hits immediately; BitS shockwaves carry it outward later.
        mario_queue_bowser_impact(o);
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * Makes Bowser do a second hop speed only in BitS
 */
void bowser_short_second_hop(void) {
    if (o->oBehParams2ndByte == BOWSER_BP_BITS && o->oBowserStatus & BOWSER_STATUS_BIG_JUMP) {
        if (o->oBowserDistToCenter > 1000.0f) {
            o->oForwardVel = 60.0f;
        }
    }
}

// Steer center-return jumps; ordinary attack jumps keep their native travel.
static void bowser_center_jump_update(void) {
    // Solve the arc to a safe landing point; Fight 2 avoids the tilt pivot.
    f32 gravity = o->oGravity < 0.0f ? o->oGravity : -4.0f;
    f32 velocity = o->oVelY + gravity * 0.5f;
    f32 height = o->oPosY - o->oHomeY;
    f32 discriminant = MAX(0.0f, velocity * velocity - 2.0f * gravity * height);
    s32 remaining = MAX(1, (s32) ((velocity + sqrtf(discriminant)) / -gravity + 0.999f));
    f32 targetX = o->oBehParams2ndByte == BOWSER_BP_BITFS ? -800.0f : 0.0f;
    f32 dx = targetX - o->oPosX;
    f32 dz = -o->oPosZ;
    o->oMoveAngleYaw = atan2s(dz, dx);
    o->oForwardVel = sqrtf(dx * dx + dz * dz) / remaining;
    o->oDragStrength = 0.0f;
}

static void bowser_start_center_jump(void) {
    cur_obj_change_action(BOWSER_ACT_BIG_JUMP);
    o->oSubAction = 3;
    o->oBowserStatus &= ~BOWSER_STATUS_BIG_JUMP;
    o->oVelY = 80.0f;
    bowser_center_jump_update();
    o->oBowserTimer = 0;
    cur_obj_init_animation_with_sound(BOWSER_ANIM_JUMP_START);
    o->header.gfx.animInfo.animFrame = 11;
    cur_obj_play_sound_2(SOUND_OBJ2_BOWSER_ROAR);
}

static s32 bowser_action_is_attack(s32 action) {
    switch (action) {
        case BOWSER_ACT_BREATH_FIRE:
        case BOWSER_ACT_SPIT_FIRE_INTO_SKY:
        case BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR:
        case BOWSER_ACT_CHARGE_MARIO:
        case BOWSER_ACT_BIG_JUMP:
        case BOWSER_ACT_QUICK_JUMP:
        case BOWSER_ACT_TELEPORT:
        case BOWSER_ACT_TILT_LAVA_PLATFORM:
            return TRUE;
    }
    return FALSE;
}

static void bowser_update_post_attack_facing(s32 previousAction, s32 previousSubAction) {
    if (o->oHeldState != HELD_FREE || o->oHealth <= 0) {
        o->oBowserPostAttackFacingTimer = -1;
        return;
    }
    if (bowser_action_is_attack(previousAction) && o->oAction == BOWSER_ACT_DEFAULT
        && (previousAction != BOWSER_ACT_TELEPORT
            || (previousSubAction == BOWSER_SUB_ACT_TELEPORT_STOP && o->oOpacity == 255))) {
        o->oBowserGrabCooldown = 0;
        o->oBowserPostAttackFacingTimer = 0;
        return;
    }
    // Only the interval after an attack qualifies; introductions and damage do not.
    if (o->oAction != BOWSER_ACT_DEFAULT && o->oAction != BOWSER_ACT_WALK_TO_MARIO
        && o->oAction != BOWSER_ACT_DANCE && o->oAction != BOWSER_ACT_UNUSED_SLOW_WALK
        && o->oAction != BOWSER_ACT_HIT_EDGE && o->oAction != BOWSER_ACT_TURN_FROM_EDGE) {
        o->oBowserPostAttackFacingTimer = -1;
        return;
    }
    if (o->oBowserPostAttackFacingTimer < 0) {
        return;
    }
    if (o->oBowserStatus & BOWSER_STATUS_ANGLE_MARIO) {
        o->oBowserPostAttackFacingTimer = 0;
        return;
    }
    s32 facingTimeout = o->oBehParams2ndByte == BOWSER_BP_BITS ? 120 : 150;
    if (o->oBowserPostAttackFacingTimer < facingTimeout) {
        o->oBowserPostAttackFacingTimer++;
    }
    if (o->oBowserPostAttackFacingTimer == facingTimeout && o->oFloor != NULL
        && (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) && o->oVelY <= 5.0f
        && ABS(o->oPosY - o->oFloorHeight) <= 1.0f) {
        o->oBowserPostAttackFacingTimer = -1;
        bowser_start_center_jump();
    }
}

static void bowser_spawn_center_return_flames(void) {
    if (o->oBehParams2ndByte != BOWSER_BP_BITFS) {
        return;
    }
    for (s32 flameIndex = 0; flameIndex < 8; flameIndex++) {
        s16 angle = flameIndex * 0x2000;
        struct Object *flame =
            spawn_object_relative_with_scale(0, sins(angle) * 300.0f, 200, coss(angle) * 300.0f, 5.0f,
                                             o, MODEL_RED_FLAME, bhvFlameFloatingLanding);
        flame->oBehParams |= BOWSER_FLAME_DOUBLE_LIFETIME;
    }
}

void bowser_act_big_jump(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_act_big_jump();
        return;
    }

    if (o->oSubAction == 0) {
        // Set jump animation
        if (bowser_set_anim_jump()) {
            // Set vel depending of the stage and status
            if (o->oBehParams2ndByte == BOWSER_BP_BITS && o->oBowserStatus & BOWSER_STATUS_BIG_JUMP) {
                o->oVelY = 60.0f;
            } else {
                o->oVelY = o->oBehParams2ndByte == BOWSER_BP_BITS ? 60.0f : 80.0f;
            }
            o->oBowserTimer = 0;
            bowser_short_second_hop();
            o->oSubAction++;
        }
    } else if (o->oSubAction == 1 || o->oSubAction == 3) {
        if (o->oSubAction == 3) {
            bowser_center_jump_update();
        }
        // Reset Bowser back on stage in BitS if he doesn't land properly
        if (o->oBehParams2ndByte == BOWSER_BP_BITS && o->oBowserStatus & BOWSER_STATUS_BIG_JUMP) {
            bowser_reset_fallen_off_stage();
        }
        // Land on stage, reset status jump and velocity
        if (bowser_land()) {
            if (o->oSubAction == 3) {
                bowser_spawn_center_return_flames();
                o->oDragStrength = 10.0f;
                o->oSubAction = 1;
            }
            o->oForwardVel = 0.0f;
            o->oSubAction++;
            // Spawn shockwave (BitS only) if is not on a platform
            bowser_spawn_shockwave();
            o->oBowserStatus &= ~BOWSER_STATUS_BIG_JUMP;
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

/**
 * Makes Bowser do a "quick" jump in BitDW
 */
void bowser_act_quick_jump(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_act_quick_jump();
        return;
    }

    if (o->oSubAction == 0) {
        if (o->oTimer == 0) {
            o->oBowserTimer = 0;
        }
        s32 startup;
        if (o->oBehParams2ndByte == BOWSER_BP_BITFS) {
            startup = 8;
        } else if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
            startup = 6;
        } else {
            startup = 10;
        }
        cur_obj_init_animation_with_accel_and_sound(BOWSER_ANIM_JUMP_START, 12.5f / startup);
        if (++o->oBowserTimer >= startup) {
            // At gravity -4, level-floor arcs last 25/22/19 updates in Fights 1/2/3.
            if (o->oBehParams2ndByte == BOWSER_BP_BITFS) {
                o->oVelY = 44.0f;
            } else if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
                o->oVelY = 38.0f;
            } else {
                o->oVelY = 50.0f;
            }
            o->oForwardVel = 50.0f;
            o->oBowserTimer = 0;
            o->oSubAction++;
        }
    } else if (o->oSubAction == 1) {
        if (bowser_land()) {
            s32 waveCount = o->oBehParams2ndByte == BOWSER_BP_BITS ? 2 : 1;
            for (s32 waveIndex = 0; waveIndex < waveCount; waveIndex++) {
                struct Object *wave = spawn_object(o, MODEL_BOWSER_WAVE, bhvBowserShockWave);
                if (wave != NULL) {
                    wave->oBehParams2ndByte =
                        waveIndex == 0 ? BOWSER_SHOCKWAVE_LIGHT : BOWSER_SHOCKWAVE_DELAYED;
                    wave->oPosY = o->oFloorHeight;
                }
            }
            o->oBowserTimer = 0;
            cur_obj_init_animation_with_accel_and_sound(BOWSER_ANIM_JUMP_STOP, 1.2f);
            o->oSubAction++;
        }
    } else if (++o->oBowserTimer >= 15) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

/**
 * Makes Bowser moving around if he is on an edge floor
 */
void bowser_act_hit_edge(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_act_hit_edge();
        return;
    }

    if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
        o->oForwardVel = 0.0f;
        if (o->oTimer == 0) {
            o->oBowserTimer = 0;
        }
        s32 frame = ++o->oBowserTimer;
        s32 animation = frame <= 10 ? BOWSER_ANIM_EDGE_MOVE : BOWSER_ANIM_EDGE_STOP;
        cur_obj_init_animation_with_accel_and_sound(animation, frame <= 10 ? 27.0f / 10 : 15.0f / 10);
        if (frame >= 20) {
            o->oAction = BOWSER_ACT_TURN_FROM_EDGE;
        }
        return;
    }
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

/**
 * Makes Bowser do a fire split attack
 */
void bowser_act_spit_fire_onto_floor(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_act_spit_fire_onto_floor();
        return;
    }

    s32 shotFrame = o->oBehParams2ndByte == BOWSER_BP_BITS ? 2 : 4;
    s32 cycleFrames = o->oBehParams2ndByte == BOWSER_BP_BITS ? 10 : 15;
    if (o->oTimer == 0) {
        o->oBowserRandSplitFloor = 3 + (s32) (random_float() * 7.0f);
        o->oBowserTimer = 0;
        o->oSubAction = 0;
    }
    o->oForwardVel = 0.0f;
    cur_obj_init_animation_with_accel_and_sound(BOWSER_ANIM_BREATH_QUICK, 20.0f / cycleFrames);
    if (o->oBowserTimer == 0) {
        o->header.gfx.animInfo.animFrame = 0;
        o->header.gfx.animInfo.animFrameAccelAssist = 0;
    }
    ++o->oBowserTimer;
    if (o->oBowserTimer == shotFrame) {
        struct Object *flame = spawn_object_relative_with_scale(1, 0, 200, 180, 3.0f, o,
                                                                MODEL_BLUE_FLAME, bhvBlueBowserFlame);
        flame->oBehParams |= BOWSER_FLAME_NO_SPLIT;
        cur_obj_play_sound_1(SOUND_AIR_BOWSER_SPIT_FIRE);
    }
    if (o->oBowserTimer >= shotFrame) {
        s16 turnSpeed = o->oBehParams2ndByte == BOWSER_BP_BITS ? DEGREES(10) : 0x1000;
        cur_obj_rotate_yaw_toward(o->oAngleToMario, turnSpeed);
    }
    if (o->oBowserTimer >= cycleFrames) {
        o->oBowserTimer = 0;
        if (++o->oSubAction >= o->oBowserRandSplitFloor) {
            // Start the next attack directly, with the current target and no walk turn.
            o->oMoveAngleYaw = o->oAngleToMario;
            o->oBowserStatus |= BOWSER_STATUS_ANGLE_MARIO;
            o->oBowserIsReacting = FALSE;
            o->oBowserGrabCooldown = 0;
            if (o->oBehParams2ndByte == BOWSER_BP_BITDW) {
                bowser_bitdw_actions();
            } else if (o->oBehParams2ndByte == BOWSER_BP_BITFS) {
                bowser_bitfs_actions();
            } else {
                bowser_bits_action_list();
                o->oBowserIsReacting = TRUE;
            }
            // Let the script reset entry clocks, even if it picks another spit burst.
            o->oPrevAction = -1;
        }
    }
}

/**
 * Turns around Bowser from an specific yaw angle
 * Returns TRUE once the timer is bigger than the time set
 */
s32 bowser_turn_on_timer(s32 time, s16 yaw) {
    if (o->oSubAction == 0) {
        if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_LOOK_UP_START_WALK)) {
            o->oSubAction++;
        }
    } else if (o->oSubAction == 1) {
        if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_LOOK_DOWN_STOP_WALK)) {
            o->oSubAction++;
        }
    } else {
        cur_obj_init_animation_with_sound(BOWSER_ANIM_IDLE);
    }

    o->oForwardVel = 0.0f;
    o->oMoveAngleYaw += yaw;
    return o->oTimer >= time;
}

/**
 * Makes Bowser turn around after hitting the edge
 */
void bowser_act_turn_from_edge(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_act_turn_from_edge();
        return;
    }

    if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
        o->oForwardVel = 0.0f;
        cur_obj_init_animation_with_sound(BOWSER_ANIM_IDLE);
        cur_obj_rotate_yaw_toward(o->oAngleToMario, bowser_walk_turn_speed());
        if (abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario) == 0) {
            o->oAction = BOWSER_ACT_DEFAULT;
        }
        return;
    }
    if (bowser_turn_on_timer(63, 0x200)) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

/**
 * Makes Bowser charge (run) to Mario
 */
void bowser_act_charge_mario(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        original_bowser_act_charge_mario();
        return;
    }

    s32 fightThree = o->oBehParams2ndByte == BOWSER_BP_BITS;
    s32 startup = fightThree ? 9 : 14;
    // Reset Speed to prepare charge
    if (o->oTimer == 0) {
        o->oForwardVel = 0.0f;
        o->oBowserTimer = 0;
    }

    switch (o->oSubAction) {
        case BOWSER_SUB_ACT_CHARGE_START:
            o->oForwardVel = 0.0f;
            cur_obj_init_animation_with_accel_and_sound(BOWSER_ANIM_RUN_START, 23.0f / startup);
            if (++o->oBowserTimer >= startup) {
                o->oBowserTimer = 0;
                o->oSubAction = BOWSER_SUB_ACT_CHARGE_RUN;
            }
            break;

        case BOWSER_SUB_ACT_CHARGE_RUN: {
            f32 initialSpeed = fightThree ? 65.0f : 60.0f;
            f32 acceleration = fightThree ? 0.4f : 0.2f;
            // Use elapsed running time so movement drag cannot erase acceleration.
            o->oForwardVel = initialSpeed + MAX(0, o->oTimer - startup) * acceleration;
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
            cur_obj_rotate_yaw_toward(o->oAngleToMario, fightThree ? DEGREES(3.2f) : 0x200);
            break;
        }

        case BOWSER_SUB_ACT_CHARGE_SLIP:
            // Spawn smoke puff while slipping
            o->oBowserTimer = 0;
            cur_obj_init_animation_with_sound(BOWSER_ANIM_RUN_SLIP);
            // Keep the trail continuous with fewer cosmetic puffs in the final fight.
            if (o->oBehParams2ndByte != BOWSER_BP_BITS || !(o->oTimer & 1)) {
                try_spawn_particle_relative(0, 100, -50, 0, 3.0f, o, MODEL_SMOKE, bhvWhitePuffSmoke2,
                                            PARTICLE_PRIORITY_AMBIENT);
                try_spawn_particle_relative(0, -100, -50, 0, 3.0f, o, MODEL_SMOKE, bhvWhitePuffSmoke2,
                                            PARTICLE_PRIORITY_AMBIENT);
            }
            // End Charge once Bowser stops running
            if (approach_f32_signed(&o->oForwardVel, 0, fightThree ? -3.5f : -2.5f)) {
                o->oSubAction = BOWSER_SUB_ACT_CHARGE_END;
            }
            cur_obj_extend_animation_if_at_end();
            break;

        case BOWSER_SUB_ACT_CHARGE_END:
            // Stop running
            o->oForwardVel = 0.0f;
            cur_obj_init_animation_with_sound(BOWSER_ANIM_RUN_STOP);
            if (++o->oBowserTimer >= (fightThree ? 15 : 35)) {
                o->oAction = BOWSER_ACT_DEFAULT;
            }
            cur_obj_extend_animation_if_at_end();
            break;
    }

    // Bowser is close to falling so set hit edge action
    if (o->oMoveFlags & OBJ_MOVE_HIT_EDGE) {
        o->oAction = BOWSER_ACT_HIT_EDGE;
    }
}

/**
 * Checks if Bowser hits a mine from a distance, returns TRUE if so
 */
s32 bowser_check_hit_mine(void) {
    if (game_options_get(GO_BOWSER_FIGHTS) == 0 && !(o->oBowserStatus & BOWSER_STATUS_VALID_THROW)) {
        return FALSE;
    }

    struct Object *nearest = bowser_throw_nearest_mine(o);
    if (nearest != NULL) {
        nearest->oInteractStatus |= INT_STATUS_HIT_MINE;
        return TRUE;
    }
    return FALSE;
}

// Resolve each player throw once, including weak throws and off-stage recovery.
static void bowser_finish_throw(s32 hitMine) {
    if (o->oBowserStatus & BOWSER_STATUS_THROW_PENDING) {
        o->oBowserStatus &= ~BOWSER_STATUS_THROW_PENDING;
        if (!hitMine) {
            player_stats_add(PS_MISSED_BOWSER_THROWS, 1);
        }
    }
}

/** Bowser's thrown act that gets called after Mario releases him. */
void bowser_act_thrown(void) {
    s32 hitMine = bowser_check_hit_mine();
    if (hitMine || (o->oMoveFlags & OBJ_MOVE_ON_GROUND)) {
        bowser_finish_throw(hitMine);
    }
    if (!(o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) && o->bowserThrowAirFrames <= 30) {
        o->bowserThrowAirFrames++;
    }
    // Keep Bowser's timer at 0 unless he lands
    if (o->oTimer < 2) {
        o->oBowserTimer = 0;
    }
    if (o->oSubAction == 0) {
        // Play shake animations and do bounce effects
        cur_obj_init_animation_with_sound(BOWSER_ANIM_SHAKING);
        if (game_options_get(GO_BOWSER_FIGHTS) == 0 && o->oBehParams2ndByte == BOWSER_BP_BITS
            && !hitMine && o->oBowserTimer == 0 && (o->oMoveFlags & OBJ_MOVE_LANDED)
            && o->bowserThrowAirFrames > 30) {
            mario_queue_bowser_impact(o);
            struct Object *wave = spawn_object(o, MODEL_BOWSER_WAVE, bhvBowserShockWave);
            if (wave != NULL) {
                wave->oBehParams2ndByte = BOWSER_SHOCKWAVE_LIGHT;
                wave->oPosY = o->oFloorHeight;
            }
        }
        bowser_bounce_effects(&o->oBowserTimer);
        // Reset speed when he moves on ground
        if (o->oMoveFlags & OBJ_MOVE_ON_GROUND) {
            o->oBowserStatus &= ~BOWSER_STATUS_VALID_THROW;
            o->oForwardVel = 0.0f;
            o->oSubAction++; // stops this current subaction
        }
        // Stand up and after play, set to default act
    } else if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_STAND_UP)) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
    // Hit mine check, reduce health and set specific action depending of it
    if (hitMine) {
        o->oBowserStatus &= ~BOWSER_STATUS_VALID_THROW;
        player_stats_add(PS_BOWSER_BOMB_HITS, 1);
        player_stats_attack(o, ATTACK_NONE);
        o->oHealth--;
        if (o->oHealth <= 0) {
            player_stats_enemy_defeated(o);
            o->oAction = BOWSER_ACT_DEAD;
        } else {
            o->oAction = BOWSER_ACT_HIT_MINE;
        }
    }
}

/**
 * Set Bowser invisible and stops him (after falling)
 */
void bowser_set_goal_invisible(void) {
    o->oBowserTargetOpacity = 0;
    if (o->oOpacity == 0) {
        o->oForwardVel = 0.0f;
        o->oVelY = 0.0f;
        o->oPosY = (o->oHomeY - 1000.0f);
    }
}

/**
 * Makes Bowser jump back on stage after falling
 */
void bowser_act_jump_onto_stage(void) {
    struct Surface *floor = o->oFloor;

    // Set dynamic floor check (Object platforms)
    s32 onDynamicFloor = floor != NULL && floor->flags & SURFACE_FLAG_DYNAMIC;
    // Set status Jump
    o->oBowserStatus |= BOWSER_STATUS_BIG_JUMP;

    switch (o->oSubAction) {
        // Stops Bowser and makes him invisible
        case BOWSER_SUB_ACT_JUMP_ON_STAGE_IDLE:
            if (o->oTimer == 0) {
                o->oFaceAnglePitch = 0;
                o->oFaceAngleRoll = 0;
            } //? missing else
            o->oFaceAnglePitch += 0x800;
            o->oFaceAngleRoll += 0x800;
            if (!(o->oFaceAnglePitch & 0xFFFF)) {
                o->oBowserTimer = 0;
                o->oSubAction++;
            }
            bowser_set_goal_invisible();
            break;

        // Start the return windup, independently of the preceding jump's animation.
        case BOWSER_SUB_ACT_JUMP_ON_STAGE_START:
            cur_obj_init_animation_with_sound(BOWSER_ANIM_JUMP_START);
            // Quick jumps can retain this same animation at a faster speed or
            // past frame 11. An exact animation-frame check can never launch.
            if (++o->oBowserTimer >= 12) {
                o->oMoveAngleYaw = o->oBowserAngleToCenter;
                o->oVelY = 150.0f;
                o->oBowserTargetOpacity = 255;
                o->oBowserTimer = 0;
                o->oSubAction++;
            } else {
                bowser_set_goal_invisible();
            }
            break;

        // Approach him back on stage
        case BOWSER_SUB_ACT_JUMP_ON_STAGE_LAND:
            if (o->oPosY > o->oHomeY) {
                o->oDragStrength = 0.0f;
                if (o->oBowserDistToCenter < 2500.0f) {
                    if (absf(o->oFloorHeight - o->oHomeY) < 100.0f) {
                        approach_f32_signed(&o->oForwardVel, 0, -5.0f);
                    } else {
                        cur_obj_forward_vel_approach_upward(150.0f, 2.0f);
                    }
                } else {
                    cur_obj_forward_vel_approach_upward(150.0f, 2.0f);
                }
            }
            // Land on stage
            if (bowser_land()) {
                // An offstage mine hit may have bypassed the hit-mine bounce.
                bowser_arena_landed(o);
                o->oDragStrength = 10.0f;
                o->oSubAction++;
                // Spawn shockwave (BitS only) if is not on a platform
                if (onDynamicFloor == FALSE) {
                    bowser_spawn_shockwave();
                // If is on a dynamic floor in BitS, then jump
                // because of the falling platform
                } else if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
                    o->oAction = BOWSER_ACT_BIG_JUMP;
                }
                // If is on a dynamic floor in BitFS, then tilt platform
                if (o->oBehParams2ndByte == BOWSER_BP_BITFS) {
                    o->oAction = BOWSER_ACT_TILT_LAVA_PLATFORM;
                }
            }
            // Reset him back on stage if he still didn't landed yet
            // Post-JP made this check as a separate function
            bowser_reset_fallen_off_stage();
            break;

        // Bowser landed, so reset action after he's done jumping
        case BOWSER_SUB_ACT_JUMP_ON_STAGE_STOP:
            if (cur_obj_check_if_near_animation_end()) {
                o->oAction = BOWSER_ACT_DEFAULT;
                o->oBowserStatus &= ~BOWSER_STATUS_BIG_JUMP;
                cur_obj_extend_animation_if_at_end();
            }
            break;
    }

    print_debug_bottom_up("sp %d", o->oForwardVel);
}

/**
 * The frames of the Bowser's timer on which to play a "stomp" sound
 */
s8 sBowserDanceStepNoises[] = { 24, 42, 60, -1 };

/**
 * Makes Bowser's dance as a "taunt"
 */
void bowser_act_dance(void) {
    // Play a stomp sound effect on certain frames
    if (is_item_in_array(o->oTimer, sBowserDanceStepNoises)) {
        cur_obj_play_sound_2(SOUND_OBJ_BOWSER_WALK);
    }
    // Play dance animation and after that return to default action
    if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_DANCE)) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

/**
 * Spawn collectable that Bowser spawns after despawning
 * Spawns a Key in BitDW/BitFS or Grand Star in BitS
 */
void bowser_spawn_collectable(void) {
    if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
        gSecondCameraFocus = spawn_object(o, MODEL_STAR, bhvGrandStar);
    } else {
        gSecondCameraFocus = spawn_object(o, MODEL_BOWSER_KEY, bhvBowserKey);
        cur_obj_play_sound_2(SOUND_GENERAL2_BOWSER_KEY);
    }
    gSecondCameraFocus->oAngleVelYaw = o->oAngleVelYaw;
}

/**
 * Makes Bowser fly back on stage defeated
 */
void bowser_fly_back_dead(void) {
    cur_obj_init_animation_with_sound(BOWSER_ANIM_FLIP_DOWN);
    // More knockback in BitS
    if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
        o->oForwardVel = -400.0f;
    } else {
        o->oForwardVel = -200.0f;
    }
    o->oVelY = 100.0f;
    o->oMoveAngleYaw = o->oBowserAngleToCenter + 0x8000;
    o->oBowserTimer = 0;
    o->oSubAction++; // BOWSER_SUB_ACT_DEAD_BOUNCE
}

/**
 * Plays bounce effects after landing upside down
 */
void bowser_dead_bounce(void) {
    o->oBowserEyesShut = TRUE; // close eyes
    bowser_bounce_effects(&o->oBowserTimer);
    if (o->oMoveFlags & OBJ_MOVE_LANDED) {
        cur_obj_play_sound_2(SOUND_OBJ_BOWSER_WALK);
    }
    if (o->oMoveFlags & OBJ_MOVE_ON_GROUND) {
        o->oForwardVel = 0.0f;
        o->oSubAction++; // BOWSER_SUB_ACT_DEAD_WAIT
    }
}

/**
 * Wait for Mario to get close while Bowser is defeated
 * Returns TRUE if he is close enough
 */
s32 bowser_dead_wait_for_mario(void) {
    s32 ret = FALSE;
    cur_obj_become_intangible();
    if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_LAY_DOWN) && o->oDistanceToMario < 700.0f
        && abs_angle_diff(gMarioObject->oMoveAngleYaw, o->oAngleToMario) > 0x6000) {
        ret = TRUE;
    }
    cur_obj_extend_animation_if_at_end();
    o->oBowserTimer = 0;
    return ret;
}

/**
 * Makes Bowser twirl up by changing his scale
 * Returns TRUE once done
 */
s32 bowser_dead_twirl_up(void) {
    s32 ret = FALSE;

    // Set angle rotation once he has low X scale value
    if (o->header.gfx.scale[0] < 0.8f) {
        o->oAngleVelYaw += 0x80;
    }
    // Slowly scale down his X and Z value
    if (o->header.gfx.scale[0] > 0.2f) {
        o->header.gfx.scale[0] = o->header.gfx.scale[0] - 0.02f;
        o->header.gfx.scale[2] = o->header.gfx.scale[2] - 0.02f;
    } else {
        // Now scale down his Y value (and send Bowser up)
        o->header.gfx.scale[1] = o->header.gfx.scale[1] - 0.01f;
        o->oVelY = 20.0f;
        o->oGravity = 0.0f;
    }
    // At half Y scale value, he is high enough, so we are done
    if (o->header.gfx.scale[1] < 0.5f) {
        ret = TRUE;
    }
    // Copy angle rotation to moving rotation
    o->oMoveAngleYaw += o->oAngleVelYaw;
    // Slowly fade out
    if (o->oOpacity >= 3) {
        o->oOpacity -= 2;
    }

    return ret;
}

/**
 * Hides Bowser after his death sequence is done
 */
void bowser_dead_hide(void) {
    cur_obj_scale(0);
    o->oForwardVel = 0;
    o->oVelY = 0;
    o->oGravity = 0;
}

/**
 * Dialog values that are set on each stage Bowser's is defeated
 */
s16 sBowserDefeatedDialogText[3] = { DIALOG_119, DIALOG_120, DIALOG_121 };

/**
 * Bowser's dead sequence that plays in BitDW/BitFS
 * Returns TRUE once done
 */
s32 bowser_dead_default_stage_ending(void) {
    if (o->oBowserTimer < 2) {
        // Lower music volume
        if (o->oBowserTimer == 0) {
            seq_player_lower_volume(SEQ_PLAYER_LEVEL, 60, 40);
            o->oBowserTimer++;
        }
        // Play Bowser defeated dialog
        if (cur_obj_update_dialog(MARIO_DIALOG_LOOK_UP,
            (DIALOG_FLAG_TEXT_DEFAULT | DIALOG_FLAG_TIME_STOP_ENABLED),
            sBowserDefeatedDialogText[o->oBehParams2ndByte], 0)) {
            // Dialog is done, fade out music and play explode sound effect
            o->oBowserTimer++;
            cur_obj_play_sound_2(SOUND_GENERAL2_BOWSER_EXPLODE);
            seq_player_unlower_volume(SEQ_PLAYER_LEVEL, 60);
            seq_player_fade_out(SEQ_PLAYER_LEVEL, 1);
        }
    // Hide Bowser and spawn collectable once done twirling
    } else if (bowser_dead_twirl_up()) {
        bowser_dead_hide();
        spawn_triangle_break_particles(20, MODEL_YELLOW_COIN, 1.0f, 0);
        bowser_spawn_collectable();
        set_mario_npc_dialog(MARIO_DIALOG_STOP);
        return TRUE;
    }
    return FALSE;
}

/**
 * Bowser's dead sequence that plays in BitS
 * Returns TRUE once done
 */
s32 bowser_dead_final_stage_ending(void) {
    s32 dialogID;

    if (o->oBowserTimer < 2) {
        // Set dialog whenever you have 120 stars or not
        if (gHudDisplay.stars < 120) {
            dialogID = DIALOG_121;
        } else {
            dialogID = DIALOG_163;
        }
        // Lower music volume
        if (o->oBowserTimer == 0) {
            seq_player_lower_volume(SEQ_PLAYER_LEVEL, 60, 40);
            o->oBowserTimer++;
        }
        // Play Bowser defeated dialog
        if (cur_obj_update_dialog(MARIO_DIALOG_LOOK_UP,
            (DIALOG_FLAG_TEXT_DEFAULT | DIALOG_FLAG_TIME_STOP_ENABLED), dialogID, 0)) {
            // Dialog is done, fade out music and spawn grand star
            cur_obj_set_model(MODEL_BOWSER_NO_SHADOW);
            seq_player_unlower_volume(SEQ_PLAYER_LEVEL, 60);
            seq_player_fade_out(SEQ_PLAYER_LEVEL, 1);
            bowser_spawn_collectable();
            o->oBowserTimer++;
        }
    // Slowly fade him out
    } else if (o->oOpacity > 4) {
        o->oOpacity -= 4;
    } else {
        // And at last, hide him
        bowser_dead_hide();
        return TRUE;
    }
    return FALSE;
}

/**
 * Bowser's dead action, plays when he has no health left
 * This action is divided in subaction functions
 */
void bowser_act_dead(void) {
    switch (o->oSubAction) {
        case BOWSER_SUB_ACT_DEAD_FLY_BACK:
            bowser_fly_back_dead();
            break;

        case BOWSER_SUB_ACT_DEAD_BOUNCE:
            bowser_dead_bounce();
            break;

        case BOWSER_SUB_ACT_DEAD_WAIT:
            // Check if Mario is close to Bowser
            if (bowser_dead_wait_for_mario()) {
                o->oBowserTimer = 0;
                // Set different (final) subaction in BitS
                // Non-BitS Bowser uses default subaction and sets dithering
                if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
                    o->oSubAction = BOWSER_SUB_ACT_DEAD_FINAL_END;
                } else {
                    o->activeFlags |= ACTIVE_FLAG_DITHERED_ALPHA;
                    o->oSubAction++; // BOWSER_SUB_ACT_DEAD_DEFAULT_END
                }
            }
            break;

        case BOWSER_SUB_ACT_DEAD_DEFAULT_END:
            if (bowser_dead_default_stage_ending()) {
                o->oSubAction++; // BOWSER_SUB_ACT_DEAD_DEFAULT_END_OVER
            }
            break;

        case BOWSER_SUB_ACT_DEAD_DEFAULT_END_OVER:
            break;

        case BOWSER_SUB_ACT_DEAD_FINAL_END:
            if (bowser_dead_final_stage_ending()) {
                o->oSubAction++; // BOWSER_SUB_ACT_DEAD_FINAL_END_OVER
            }
            break;

        case BOWSER_SUB_ACT_DEAD_FINAL_END_OVER:
            break;
    }
}

/**
 * Sets values for the BitFS platform to tilt
 */
void bowser_tilt_platform(struct Object *platform, s16 angSpeed) {
    s16 angle = o->oBowserAngleToCenter + 0x8000;
    platform->oAngleVelPitch = coss(angle) * angSpeed;
    platform->oAngleVelRoll = -sins(angle) * angSpeed;
}

/**
 * Struct for the BitFS tilt platform
 */
struct BowserTiltPlatformInfo {
    // Flag value to make sure platform moves smoothly
    //  0 = Don't move
    //  1 = Move angle behind Bowser
    // -1 = Move angle in front of Bowser
    s16 flag;
    // Sets platform's tilt angle speed (pattern: positive then negative)
    s16 angSpeed;
    // Sets how much time the platform can tilt, increases each move
    s16 time;
};

/**
 * Data for the BitFS tilt Platform
 */
struct BowserTiltPlatformInfo sBowsertiltPlatformData[] = {
    {  1,   10,  40 },
    {  0,    0,  74 },
    { -1,  -10, 114 },
    {  1,  -20, 134 },
    { -1,   20, 154 },
    {  1,   40, 164 },
    { -1,  -40, 174 },
    {  1,  -80, 179 },
    { -1,   80, 184 },
    {  1,  160, 186 },
    { -1, -160, 186 },
    {  1,    0,   0 },
};

/**
 * Makes the platform in BitFS tilt from left to right
 */
void bowser_act_tilt_lava_platform(void) {
    // Set platform object
    struct Object *platform = cur_obj_nearest_object_with_behavior(bhvTiltingBowserLavaPlatform);
    s16 angSpeed;
    // If there's not platform, return to default action
    if (platform == NULL) {
        o->oAction = BOWSER_ACT_DEFAULT;
    } else {
        s32 i = 0;
        s32 isNotTilting = TRUE;
        // Active platform tilting if the timer is not 0
        while (sBowsertiltPlatformData[i].time != 0) {
            // Move if the time values is more than the timer
            if (o->oTimer < sBowsertiltPlatformData[i].time) {
                // Set angle speed
                angSpeed = sBowsertiltPlatformData[i].angSpeed;
                // Move angle behind Bowser
                if (sBowsertiltPlatformData[i].flag > 0) {
                    angSpeed = (sBowsertiltPlatformData[i].time - o->oTimer - 1) * angSpeed;
                } else { // Move angle in front of Bowser
                    angSpeed = (o->oTimer - sBowsertiltPlatformData[i - 1].time) * angSpeed;
                }
                // Set angle values to the platform
                bowser_tilt_platform(platform, angSpeed);
                // Play sound effect
                if (angSpeed != 0) {
                    play_sound(SOUND_ENV_MOVING_BIG_PLATFORM, platform->header.gfx.cameraToObject);
                }
                isNotTilting = FALSE;
                break;
            }
            i++;
        }
        platform->oAction = !isNotTilting;
        // If Bowser is done tilting, reset platform angles
        // and set Bowser to default action
        if (isNotTilting) {
            o->oAction = BOWSER_ACT_DEFAULT;
            platform->oAngleVelPitch = 0;
            platform->oAngleVelRoll = 0;
            platform->oFaceAnglePitch = 0;
            platform->oFaceAngleRoll = 0;
        }
    }
    // Extend "idle" animation
    cur_obj_extend_animation_if_at_end();
}

/**
 * Return an offstage defeated Bowser to safe support without restarting the fight.
 */
static void bowser_recover_defeated(void) {
    if (o->oAction != BOWSER_ACT_DEAD || o->oSubAction > BOWSER_SUB_ACT_DEAD_WAIT) {
        return;
    }
    s32 belowArena = o->oVelY <= 0.0f && o->oPosY < o->oHomeY - 300.0f;
    s32 lethalLanding = (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND)
                        && (o->oFloorType == SURFACE_BURNING || o->oFloorType == SURFACE_DEATH_PLANE);
    if (!belowArena && !lethalLanding) {
        return;
    }
    f32 x = o->oBehParams2ndByte == BOWSER_BP_BITFS ? -800.0f : 0.0f;
    struct Surface *floor;
    f32 height = find_floor(x, o->oHomeY + 2000.0f, 0.0f, &floor);
    if (floor == NULL || floor->type == SURFACE_BURNING || floor->type == SURFACE_DEATH_PLANE) {
        return;
    }
    o->oPosX = x;
    o->oPosY = height + 2000.0f;
    o->oPosZ = 0.0f;
    o->oForwardVel = 0.0f;
    o->oVelX = o->oVelY = o->oVelZ = 0.0f;
    o->oDragStrength = 10.0f;
    o->oMoveFlags = OBJ_MOVE_IN_AIR;
    o->oFloor = floor;
    o->oFloorHeight = height;
    o->oFloorType = floor->type;
    o->platform = NULL;
    if (o->oSubAction == BOWSER_SUB_ACT_DEAD_WAIT) {
        // Resume landing without repeating the already-completed defeat impact.
        o->oSubAction = BOWSER_SUB_ACT_DEAD_BOUNCE;
        o->oBowserTimer = 1;
    }
}

/** Check whether a living Bowser needs his normal offstage return. */
s32 bowser_check_fallen_off_stage(void) {
    // Defeat recovery preserves this action and the pending reward sequence.
    if (o->oAction == BOWSER_ACT_DEAD) {
        return FALSE;
    }
    if (game_options_get(GO_BOWSER_FIGHTS) == 0 && o->oBehParams2ndByte == BOWSER_BP_BITS) {
        if (o->oAction == BOWSER_ACT_JUMP_ONTO_STAGE
            && o->oSubAction <= BOWSER_SUB_ACT_JUMP_ON_STAGE_START) {
            // The return windup deliberately waits below the arena while fading.
            return FALSE;
        }
        if (o->oVelY <= 0.0f && o->oPosY < o->oHomeY - 300.0f) {
            return TRUE;
        }
        return (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND)
               && (o->oFloorType == SURFACE_BURNING || o->oFloorType == SURFACE_DEATH_PLANE);
    }

    if (o->oAction != BOWSER_ACT_JUMP_ONTO_STAGE && o->oAction != BOWSER_ACT_TILT_LAVA_PLATFORM) {
        if (o->oPosY < o->oHomeY - 1000.0f) {
            return TRUE;
        }
        if (o->oMoveFlags & OBJ_MOVE_LANDED) {
            // Check for Fire Sea
            if (o->oFloorType == SURFACE_BURNING) {
                return TRUE;
            }
            // Check for Dark World - Sky
            if (o->oFloorType == SURFACE_DEATH_PLANE) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/**
 * Set Bowser's actions
 */
#if !ENABLE_ENEMY_PLATFORM_MOTION
static struct PlatformDisplacementInfo sBowserDisplacementInfo;
#endif

ObjActionFunc sBowserActions[] = {
    bowser_act_default,
    bowser_act_thrown,
    bowser_act_jump_onto_stage,
    bowser_act_dance,
    bowser_act_dead,
    bowser_act_wait,
    bowser_act_intro_walk,
    bowser_act_charge_mario,
    bowser_act_spit_fire_into_sky,
    bowser_act_spit_fire_onto_floor,
    bowser_act_hit_edge,
    bowser_act_turn_from_edge,
    bowser_act_hit_mine,
    bowser_act_big_jump,
    bowser_act_walk_to_mario,
    bowser_act_breath_fire,
    bowser_act_teleport,
    bowser_act_quick_jump,
    bowser_act_idle,
    bowser_act_tilt_lava_platform,
};

/**
 * Set Bowser's sound animations
 */
struct SoundState sBowserSoundStates[] = {
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 1, 0, -1, SOUND_OBJ_BOWSER_WALK },
    { 1, 0, -1, SOUND_OBJ2_BOWSER_ROAR },
    { 1, 0, -1, SOUND_OBJ2_BOWSER_ROAR },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 1, 20, 40, SOUND_OBJ_BOWSER_WALK },
    { 1, 20, -1, SOUND_OBJ_BOWSER_WALK },
    { 1, 20, 40, SOUND_OBJ_BOWSER_WALK },
    { 1, 0, -1, SOUND_OBJ_BOWSER_TAIL_PICKUP },
    { 1, 0, -1, SOUND_OBJ_BOWSER_DEFEATED },
    { 1, 8, -1, SOUND_OBJ_BOWSER_WALK },
    { 1, 8, 17, SOUND_OBJ_BOWSER_WALK },
    { 1, 8, -10, SOUND_OBJ_BOWSER_WALK },
    { 0, 0, 0, NO_SOUND },
    { 1, 5, -1, SOUND_OBJ_FLAME_BLOWN },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 1, 0, -1, SOUND_OBJ_BOWSER_TAIL_PICKUP },
    { 1, 0, -1, SOUND_OBJ2_BOWSER_ROAR },
};

/**
 * Set whenever Bowser should have rainbow light or not on each stage
 */
s8 sBowserRainbowLight[] = { FALSE, FALSE, TRUE };

/**
 * Set how much health Bowser has on each stage
 */
s8 sBowserHealth[] = { 3, 4, 5 };

static void bowser_update_grab_cooldown(void) {
    // Count recovery on the arena, then wait until Bowser has faced Mario.
    if (o->oBowserGrabCooldown > 0 && (o->oMoveFlags & OBJ_MOVE_ON_GROUND)
        && o->oAction != BOWSER_ACT_THROWN && o->oAction != BOWSER_ACT_HIT_MINE
        && o->oAction != BOWSER_ACT_JUMP_ONTO_STAGE) {
        if (o->oBowserGrabCooldown > 1 || abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario) < 0x2000) {
            o->oBowserGrabCooldown--;
        }
    }
}

/**
 * Update Bowser's actions when he's hands free
 */
void bowser_free_update(void) {
#if !ENABLE_ENEMY_PLATFORM_MOTION
    // Retain Bowser's native carry when the shared enemy system is disabled.
    struct Object *platform = o->platform;
    if (platform != NULL) {
        s16 yaw = (s16) o->oFaceAngleYaw;
        apply_platform_displacement(&sBowserDisplacementInfo, &o->oPosVec, &yaw, platform);
        o->oFaceAngleYaw = yaw;
    }
#endif
    // Reset grabbed status
    o->oBowserGrabbedStatus = BOWSER_GRAB_STATUS_NONE;
    // Update positions and actions (default action)
    cur_obj_update_floor_and_walls();
    s32 previousAction = o->oAction;
    s32 previousSubAction = o->oSubAction;
    cur_obj_call_action_function(sBowserActions);
    if (game_options_get(GO_BOWSER_FIGHTS) == 0) {
        bowser_update_post_attack_facing(previousAction, previousSubAction);
    }
    cur_obj_move_standard(-78);
    bowser_recover_defeated();
    // Jump on stage if Bowser has fallen off
    if (bowser_check_fallen_off_stage()) {
        bowser_finish_throw(FALSE);
        cur_obj_change_action(BOWSER_ACT_JUMP_ONTO_STAGE);
    }
#if !ENABLE_ENEMY_PLATFORM_MOTION
    o->platform = o->oFloor != NULL ? o->oFloor->object : NULL;
#endif
    if (game_options_get(GO_BOWSER_FIGHTS) == 0) {
        bowser_update_grab_cooldown();
    }
    // Sound states for Bowser Animations
    exec_anim_sound_state(sBowserSoundStates);
}

/**
 * Update Bowser's actions when he's getting held
 */
void bowser_held_update(void) {
    bowser_finish_throw(FALSE);
    // Reset fire sky status and make him intangible
    o->oBowserStatus &= ~(BOWSER_STATUS_FIRE_SKY | BOWSER_STATUS_VALID_THROW);
    cur_obj_become_intangible();

    switch (o->oBowserGrabbedStatus) {
        // Play pickup sound, start grabbed animation, and set throw action
        // Throw action won't be played until he's actually released
        case BOWSER_GRAB_STATUS_NONE:
            o->oBowserTimer = 0;
            // Grabbing can interrupt a center-return jump before its landing cleanup.
            o->oDragStrength = 10.0f;
            cur_obj_play_sound_2(SOUND_OBJ_BOWSER_TAIL_PICKUP);
            cur_obj_unrender_set_action_and_anim(BOWSER_ANIM_GRABBED, BOWSER_ACT_THROWN);
            o->oBowserGrabbedStatus++;
            break;
        // After the grabbed animation ends, play shaking animation in a loop
        case BOWSER_GRAB_STATUS_GRABBED:
            if (cur_obj_check_if_near_animation_end()) {
                cur_obj_init_animation_with_sound(BOWSER_ANIM_SHAKING);
                o->oBowserGrabbedStatus++;
            }
            break;
        case BOWSER_GRAB_STATUS_HOLDING:
            break;
    }

    // Count the whole grab, including pickup and continuous spinning.
    s32 holdTimeout = BOWSER_HOLD_TIMEOUT_FRAMES;
    if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
        holdTimeout -= 30;
    }
    if (game_options_get(GO_BOWSER_FIGHTS) == 0 && ++o->oBowserTimer > holdTimeout
        && gMarioState->heldObj == o) {
        player_stats_add(PS_BOWSER_TIMED_ESCAPES, 1);
        mario_drop_held_object(gMarioState);
        set_mario_action(gMarioState, ACT_RELEASING_BOWSER, 1);
        mario_blow_off_cap(gMarioState, 25.0f);
        o->oBowserGrabbedStatus = BOWSER_GRAB_STATUS_BREAKING_FREE;
        return;
    }

    // Reset move flags
    o->oMoveFlags = OBJ_MOVE_NONE;
    // Copy angle values from Mario
    o->oBowserHeldAnglePitch = gMarioObject->oMoveAnglePitch;
    o->oBowserHeldAngleVelYaw = gMarioObject->oAngleVelYaw;
    o->oMoveAngleYaw = gMarioObject->oMoveAngleYaw;
}

/**
 * Update Bowser's actions when he's thrown and dropped
 */
void bowser_thrown_dropped_update(void) {
    s32 breakingFree = o->oBowserGrabbedStatus == BOWSER_GRAB_STATUS_BREAKING_FREE;
    s32 thrown = o->oHeldState == HELD_THROWN;
    o->oBowserStatus &= ~BOWSER_STATUS_THROW_PENDING;
    if (thrown && !breakingFree) {
        o->oBowserStatus |= BOWSER_STATUS_THROW_PENDING;
    }
    o->bowserThrowAirFrames = 0;
    o->oBowserGrabCooldown = game_options_get(GO_BOWSER_FIGHTS) == 0 ? BOWSER_GRAB_COOLDOWN_FRAMES : 0;
    // Reset grabbed status
    o->oBowserGrabbedStatus = BOWSER_GRAB_STATUS_NONE;
    // Set throw action and vel values
    cur_obj_get_thrown_or_placed(1.0f, 1.0f, BOWSER_ACT_THROWN);
    o->oDragStrength = 10.0f;
    bowser_throw_velocity(o->oBowserHeldAnglePitch, o->oBowserHeldAngleVelYaw, &o->oForwardVel,
                          &o->oVelY);
    o->oBowserStatus &= ~BOWSER_STATUS_VALID_THROW;
    if (!breakingFree && ABS(o->oForwardVel) >= BOWSER_MINE_MIN_THROW_SPEED) {
        o->oBowserStatus |= BOWSER_STATUS_VALID_THROW;
    }
    cur_obj_become_intangible();

    // Reset timer and subactions
    o->prevObj->oAction = BOWSER_ACT_TAIL_THROWN; // prevObj is Bowser's Tail
    o->prevObj->oTimer = 0;
    o->prevObj->oSubAction = 0; //! Tail doesn't have sub actions

    o->oTimer = 0;
    o->oSubAction = 0;

    if (breakingFree) {
        // Leave Mario's hands visibly, then use the normal jump landing/recovery.
        bowser_start_center_jump();
    }

    if (thrown) {
        // Start the flight on release instead of waiting for the next free update.
        cur_obj_update_floor_and_walls();
        cur_obj_move_standard(-78);
    }
}

// Teleport fade durations depend on the fight; other opacity changes retain their pace.
static void bowser_update_opacity(void) {
    if (o->oHeldState == HELD_HELD) {
        o->oBowserTargetOpacity = 255;
        o->oOpacity = o->oBowserTargetOpacity;
        return;
    }
    // Adjust opacity (when not dead)
    s32 step = 20;
    if (game_options_get(GO_BOWSER_FIGHTS) == 0 && o->oAction == BOWSER_ACT_TELEPORT) {
        if (o->oBehParams2ndByte == BOWSER_BP_BITFS) {
            step = 32;
        } else if (o->oBehParams2ndByte == BOWSER_BP_BITS) {
            step = 43;
        } else {
            step = 26;
        }
    }
    if (o->oAction != BOWSER_ACT_DEAD) {
        if (o->oBowserTargetOpacity != o->oOpacity) {
            // increase opacity when oBowserTargetOpacity is 255
            if (o->oBowserTargetOpacity > o->oOpacity) {
                o->oOpacity += step;
                if (o->oOpacity > 255) {
                    o->oOpacity = 255;
                }
                // reduce opacity when oBowserTargetOpacity is 0
            } else {
                o->oOpacity -= step;
                if (o->oOpacity < 0) {
                    o->oOpacity = 0;
                }
            }
        }
    }
}

/** Bowser's main loop. */
void bhv_bowser_loop(void) {
    bowser_arena_update(o);
    s16 angleToMario;  // AngleToMario from Bowser's perspective
    s16 angleToCenter; // AngleToCenter from Bowser's perspective

    // Set distance/angle values
    o->oBowserDistToCenter = sqrtf(o->oPosX * o->oPosX + o->oPosZ * o->oPosZ);
    o->oBowserAngleToCenter = atan2s(0.0f - o->oPosZ, 0.0f - o->oPosX);
    angleToMario = abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario);
    angleToCenter = abs_angle_diff(o->oMoveAngleYaw, o->oBowserAngleToCenter);

    // Reset Status
    o->oBowserStatus &= ~0xFF;

    // Set bitflag status for distance/angle values
    // Only the first one is used
    if (angleToMario < 0x2000) {
        o->oBowserStatus |= BOWSER_STATUS_ANGLE_MARIO;
    }
    if (angleToCenter < 0x3800) {
        o->oBowserStatus |= BOWSER_STATUS_ANGLE_CENTER; // unused
    }
    if (o->oBowserDistToCenter < 1000.0f) {
        o->oBowserStatus |= BOWSER_STATUS_DIST_CENTER; // unused
    }
    if (o->oDistanceToMario < 850.0f) {
        o->oBowserStatus |= BOWSER_STATUS_DIST_MARIO; // unused
    }

    if (o->oHeldState != HELD_FREE) {
        o->oBowserPostAttackFacingTimer = -1;
    }

    // Update Held state actions
    switch (o->oHeldState) {
        case HELD_FREE:
            bowser_free_update();
            break;
        case HELD_HELD:
            bowser_held_update();
            break;
        case HELD_THROWN:
            bowser_thrown_dropped_update();
            break;
        case HELD_DROPPED:
            bowser_thrown_dropped_update();
            break;
    }
    // Adjust model to the floor
    cur_obj_align_gfx_with_floor();

    bowser_update_opacity();
}

/**
 * Bowser's initial values and actions
 */
void bhv_bowser_init(void) {
    s32 level;
    o->oBowserPostAttackFacingTimer = -1;
    // Set "reaction" value
    // It goes true when Bowser is a non-walking state
    o->oBowserIsReacting = TRUE;
    // Set no transparency opacity
    o->oOpacity = 255;
    o->oBowserTargetOpacity = 255;
    // Set Bowser B-param depending of the stage
    if (gCurrLevelNum == LEVEL_BOWSER_2) {
        level = BOWSER_BP_BITFS;
    } else if (gCurrLevelNum == LEVEL_BOWSER_3) {
        level = BOWSER_BP_BITS;
    } else { // LEVEL_BOWSER_1
        level = BOWSER_BP_BITDW;
    }
    o->oBehParams2ndByte = level;
    // Set health and rainbow light depending of the level
    o->oBowserRainbowLight = sBowserRainbowLight[level];
    static const s8 originalHealth[] = { 1, 1, 3 };
    o->oHealth = game_options_get(GO_BOWSER_FIGHTS) == 1 ? originalHealth[level] : sBowserHealth[level];
    // Start camera event, this event is not defined so maybe
    // the "start arena" cutscene was originally called this way
    cur_obj_start_cam_event(o, CAM_EVENT_BOWSER_INIT);
    o->oAction = BOWSER_ACT_WAIT;
    // Set eyes status
    o->oBowserEyesTimer = 0;
    o->oBowserEyesShut = FALSE;
}

Gfx *geo_update_body_rot_from_parent(s32 callContext, UNUSED struct GraphNode *node, Mat4 mtx) {
    if (callContext == GEO_CONTEXT_RENDER) {
        struct Object *obj = (struct Object *) gCurGraphNodeObject;
        if (obj->prevObj != NULL) {
            obj_update_pos_from_parent_transformation(mtx, obj->prevObj);
            obj_set_gfx_pos_from_pos(obj->prevObj);
        }
    }

    return NULL;
}

/**
 * Bowser's eyes Geo-Switch-Case IDs, defined from Mario's POV
 */
enum BowserEyesGSCId {
    /*0x00*/ BOWSER_EYES_OPEN,
    /*0x01*/ BOWSER_EYES_HALF_CLOSED,
    /*0x02*/ BOWSER_EYES_CLOSED,
    /*0x03*/ BOWSER_EYES_LEFT,
    /*0x04*/ BOWSER_EYES_FAR_LEFT,
    /*0x05*/ BOWSER_EYES_RIGHT,
    /*0x06*/ BOWSER_EYES_FAR_RIGHT,
    /*0x07*/ BOWSER_EYES_DERP, // unused
    /*0x08*/ BOWSER_EYES_CROSS, // unused
    /*0x08*/ BOWSER_EYES_RESET // set eyes back to open
};

/**
 * Controls Bowser's eye open stage, including blinking and look directions
 */
void bowser_open_eye_switch(struct Object *obj, struct GraphNodeSwitchCase *switchCase) {
    s32 eyeCase;
    s16 angleFromMario;

    angleFromMario = abs_angle_diff(obj->oMoveAngleYaw, obj->oAngleToMario);
    eyeCase = switchCase->selectedCase;

    switch (eyeCase) {
        case BOWSER_EYES_OPEN:
            // Mario is in Bowser's field of view
            if (angleFromMario > 0x2000) {
                if (obj->oAngleVelYaw > 0) {
                    switchCase->selectedCase = BOWSER_EYES_RIGHT;
                }
                if (obj->oAngleVelYaw < 0) {
                    switchCase->selectedCase = BOWSER_EYES_LEFT;
                }
            }
            // Half close, start blinking
            if (obj->oBowserEyesTimer > 50) {
                switchCase->selectedCase = BOWSER_EYES_HALF_CLOSED;
            }
            break;

        case BOWSER_EYES_HALF_CLOSED:
            // Close, blinking
            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_CLOSED;
            }
            break;

        case BOWSER_EYES_CLOSED:
            // Reset blinking
            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_RESET;
            }
            break;

        case BOWSER_EYES_RESET:
            // Open, no longer blinking
            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_OPEN;
            }
            break;

        case BOWSER_EYES_RIGHT:
            // Look more on the right if angle didn't change
            // Otherwise, look at the center (open)
            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_FAR_RIGHT;
                if (obj->oAngleVelYaw <= 0) {
                    switchCase->selectedCase = BOWSER_EYES_OPEN;
                }
            }
            break;

        case BOWSER_EYES_FAR_RIGHT:
            // Look close right if angle was drastically changed
            if (obj->oAngleVelYaw <= 0) {
                switchCase->selectedCase = BOWSER_EYES_RIGHT;
            }
            break;

        case BOWSER_EYES_LEFT:
            // Look more on the left if angle didn't change
            // Otherwise, look at the center (open)
            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_FAR_LEFT;
                if (obj->oAngleVelYaw >= 0) {
                    switchCase->selectedCase = BOWSER_EYES_OPEN;
                }
            }
            break;

        case BOWSER_EYES_FAR_LEFT:
            // Look close left if angle was drastically changed
            if (obj->oAngleVelYaw >= 0) {
                switchCase->selectedCase = BOWSER_EYES_LEFT;
            }
            break;

        default:
            switchCase->selectedCase = BOWSER_EYES_OPEN;
    }

    // Reset timer if eye case has changed
    if (switchCase->selectedCase != eyeCase) {
        obj->oBowserEyesTimer = -1;
    }
}

/**
 * Geo switch for controlling the state of Bowser's eye direction and open/closed
 * state. Checks whether oBowserEyesShut is TRUE and closes eyes if so and processes
 * direction otherwise.
 */
Gfx *geo_switch_bowser_eyes(s32 callContext, struct GraphNode *node, UNUSED Mat4 *mtx) {
    struct Object *obj = (struct Object *) gCurGraphNodeObject;
    struct GraphNodeSwitchCase *switchCase = (struct GraphNodeSwitchCase *) node;

    if (callContext == GEO_CONTEXT_RENDER) {
        if (gCurGraphNodeHeldObject != NULL) {
            obj = gCurGraphNodeHeldObject->objNode;
        }
        if (obj->oBowserEyesShut) { // eyes closed, blinking
            switchCase->selectedCase = BOWSER_EYES_CLOSED;
        } else { // eyes open, handle eye looking direction
            bowser_open_eye_switch(obj, switchCase);
        }

        obj->oBowserEyesTimer++;
    }

    return NULL;
}

/**
 * Geo switch that sets Bowser's Rainbow coloring (in BitS)
 */
Gfx *geo_bits_bowser_coloring(s32 callContext, struct GraphNode *node, UNUSED s32 context) {
    Gfx *gfxHead = NULL;

    if (callContext == GEO_CONTEXT_RENDER) {
        struct Object *obj = (struct Object *) gCurGraphNodeObject;
        struct GraphNodeGenerated *graphNode = (struct GraphNodeGenerated *) node;
        if (gCurGraphNodeHeldObject != NULL) {
            obj = gCurGraphNodeHeldObject->objNode;
        }

        // Set layers if object is transparent or not
        if (obj->oOpacity == 255) {
            SET_GRAPH_NODE_LAYER(graphNode->fnNode.node.flags, LAYER_OPAQUE);
        } else {
            SET_GRAPH_NODE_LAYER(graphNode->fnNode.node.flags, LAYER_TRANSPARENT);
        }
        Gfx *gfx = gfxHead = alloc_display_list(2 * sizeof(Gfx));
        // If TRUE, clear lighting to give rainbow color
        if (obj->oBowserRainbowLight) {
            gSPClearGeometryMode(gfx++, G_LIGHTING);
        }

        gSPEndDisplayList(gfx);
    }

    return gfxHead;
}

#include "bowser_original.inc.c"
