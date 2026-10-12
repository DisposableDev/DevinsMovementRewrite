#include <PR/ultratypes.h>

#include "sm64.h"
#include "mario_actions_object.h"
#include "mario_punch.h"
#include "types.h"
#include "mario_step.h"
#include "mario.h"
#include "game_options_runtime.h"
#include "mario_actions_stationary.h"
#include "mario_actions_moving.h"
#include "audio/external.h"
#include "interaction.h"
#include "engine/math_util.h"
#include "rumble_init.h"
#include "bowser_throw.h"

#define THROW_CARRY_SPEED_LIMIT 12.0f
#define THROW_GROUND_DECEL 1.5f
#define DIVE_PICKUP_CANCEL_EARLY_FRAMES 10

void animated_stationary_ground_step(struct MarioState *m, s32 animation, u32 endAction) {
    stationary_ground_step(m);
    set_mario_animation(m, animation);
    if (is_anim_at_end(m)) {
        set_mario_action(m, endAction, 0);
    }
}

s32 act_punching(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (mario_update_punch_sequence(m)) {
        return TRUE;
    }
    apply_punch_lunge(m);
    apply_punch_ground_drag(m);
    perform_ground_step(m);
    if (mario_check_punch_lunge_grab(m)) {
        return TRUE;
    }
    return FALSE;
}

s32 act_picking_up(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return drop_and_set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->actionState == ACT_STATE_PICKING_UP_GRAB && m->heldObj == NULL
        && (m->usedObj == NULL || !(m->usedObj->activeFlags & ACTIVE_FLAG_ACTIVE))) {
        m->usedObj = NULL;
        return set_mario_action(m, ACT_IDLE, 0);
    }

    if (m->actionState == ACT_STATE_PICKING_UP_GRAB && is_anim_at_end(m)) {
        mario_grab_used_object(m);
        play_sound_if_no_flag(m, SOUND_MARIO_HRMM, MARIO_MARIO_SOUND_PLAYED);
        m->actionState = ACT_STATE_PICKING_UP_HOLDING;
    }

    if (m->actionState == ACT_STATE_PICKING_UP_HOLDING) {
        if (m->heldObj->oInteractionSubtype & INT_SUBTYPE_GRABS_MARIO) {
            m->marioBodyState->grabPos = GRAB_POS_HEAVY_OBJ;
            set_mario_animation(m, MARIO_ANIM_GRAB_HEAVY_OBJECT);
            if (is_anim_at_end(m)) {
                set_mario_action(m, ACT_HOLD_HEAVY_IDLE, 0);
            }
        } else {
            m->marioBodyState->grabPos = GRAB_POS_LIGHT_OBJ;
            set_mario_animation(m, MARIO_ANIM_PICK_UP_LIGHT_OBJ);
            if (is_anim_at_end(m)) {
                set_mario_action(m, ACT_HOLD_IDLE, 0);
            }
        }
    }

    stationary_ground_step(m);
    return FALSE;
}

s32 act_dive_picking_up(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

#ifdef HANDS_FREE_HOLDING_FIX
    if (m->input & INPUT_OFF_FLOOR) {
        return drop_and_set_mario_action(m, ACT_FREEFALL, 0);
    }
    if (m->input & INPUT_ABOVE_SLIDE) {
        return drop_and_set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }
#else
    //! Hands-free holding. Landing on a slope or being pushed off a ledge while
    // landing from a dive grab sets Mario's action to a non-holding action
    // without dropping the object, causing the hands-free holding glitch.
    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }
#endif

    set_mario_animation(m, MARIO_ANIM_STOP_SLIDE_LIGHT_OBJ);
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    if (m->heldObj != NULL && animInfo->curAnim != NULL
        && animInfo->animFrame >= animInfo->curAnim->loopEnd - 1 - DIVE_PICKUP_CANCEL_EARLY_FRAMES) {
        // Stick input was suppressed while this action was sampled. Let it
        // select held walking without turning Mario on this same-frame exit.
        if (m->controller->stickMag > 0.0f) {
            m->input = (m->input | INPUT_NONZERO_ANALOG) & ~INPUT_IDLE;
            m->intendedMag = 0.0f;
            m->intendedYaw = m->faceAngle[1];
        }
        if (m->objectRecoveryAFrames > 0) {
            m->input |= INPUT_A_PRESSED;
        }
        if (m->objectRecoveryBFrames > 0) {
            m->input |= INPUT_B_PRESSED;
        }
        if ((m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_DOWN | INPUT_NONZERO_ANALOG))
            && check_common_hold_idle_cancels(m)) {
            return TRUE;
        }
    }

    // A dive grab loses ground speed at the same rate as an ordinary dive slide.
    update_sliding(m, 0.0f);
    if (perform_ground_step(m) == GROUND_STEP_HIT_WALL) {
        mario_set_forward_vel(m, 0.0f);
    }
    if (is_anim_at_end(m)) {
        return set_mario_action(m, m->forwardVel != 0.0f ? ACT_HOLD_DECELERATING : ACT_HOLD_IDLE, 0);
    }
    return FALSE;
}

s32 act_placing_down(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return drop_and_set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (++m->actionTimer == 8) {
        mario_drop_held_object(m);
    }

    animated_stationary_ground_step(m, MARIO_ANIM_PLACE_LIGHT_OBJ, ACT_IDLE);
    return FALSE;
}

static s32 cancel_throw_recovery(struct MarioState *m, s32 animation, s32 cancelFrames) {
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    if (m->heldObj != NULL || animInfo->animID != animation || animInfo->curAnim == NULL
        || animInfo->animFrame < animInfo->curAnim->loopEnd - cancelFrames) {
        return FALSE;
    }

    if (m->objectRecoveryAFrames > 0) {
        m->input |= INPUT_A_PRESSED;
    }
    if (m->objectRecoveryBFrames > 0) {
        m->input |= INPUT_B_PRESSED;
    }
    if (m->input
        & (INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_DOWN | INPUT_NONZERO_ANALOG | INPUT_OFF_FLOOR
           | INPUT_ABOVE_SLIDE | INPUT_FIRST_PERSON)) {
        return check_common_idle_cancels(m);
    }
    return FALSE;
}

static s32 throw_ground_step(struct MarioState *m, s32 animation) {
    if (m->actionTimer == 1 && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        mario_set_forward_vel(m,
                              CLAMP(m->forwardVel, -THROW_CARRY_SPEED_LIMIT, THROW_CARRY_SPEED_LIMIT));
    }

    mario_set_forward_vel(m, approach_f32(m->forwardVel, 0.0f, THROW_GROUND_DECEL, THROW_GROUND_DECEL));
    if (m->forwardVel == 0.0f) {
        stationary_ground_step(m);
    } else {
        mario_update_moving_sand(m);
        mario_update_windy_ground(m);
        switch (perform_ground_step(m)) {
            case GROUND_STEP_LEFT_GROUND:
                return drop_and_set_mario_action(m, ACT_FREEFALL, 0);
            case GROUND_STEP_HIT_WALL:
                mario_set_forward_vel(m, 0.0f);
                break;
        }
    }

    set_mario_animation(m, animation);
    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_IDLE, 0);
    }
    return FALSE;
}

s32 act_throwing(struct MarioState *m) {
    if (m->heldObj && (m->heldObj->oInteractionSubtype & INT_SUBTYPE_HOLDABLE_NPC)) {
        return set_mario_action(m, ACT_PLACING_DOWN, 0);
    }

    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return drop_and_set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->actionTimer == 0 && m->heldObj != NULL) {
        // Save carry speed before the throw animation slows Mario down.
        m->heldObj->oForwardVel = MAX(0.0f, m->forwardVel);
    }

    if (++m->actionTimer == 7) {
        mario_throw_held_object(m);
        play_sound_if_no_flag(m, SOUND_MARIO_WAH2, MARIO_MARIO_SOUND_PLAYED);
        play_sound_if_no_flag(m, SOUND_ACTION_THROW, MARIO_ACTION_SOUND_PLAYED);
        queue_rumble_data(3, 50);
    }

    if (cancel_throw_recovery(m, MARIO_ANIM_GROUND_THROW, 7)) {
        return TRUE;
    }
    return throw_ground_step(m, MARIO_ANIM_GROUND_THROW);
}

s32 act_heavy_throw(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return drop_and_set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (++m->actionTimer == 13) {
        mario_drop_held_object(m);
        play_sound_if_no_flag(m, SOUND_MARIO_WAH2, MARIO_MARIO_SOUND_PLAYED);
        play_sound_if_no_flag(m, SOUND_ACTION_THROW, MARIO_ACTION_SOUND_PLAYED);
        queue_rumble_data(3, 50);
    }

    if (cancel_throw_recovery(m, MARIO_ANIM_HEAVY_THROW, 7)) {
        return TRUE;
    }
    return throw_ground_step(m, MARIO_ANIM_HEAVY_THROW);
}

s32 act_stomach_slide_stop(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (check_common_landing_cancels(m, 0)) {
        return TRUE;
    }

    if (coasting_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }
    set_mario_animation(m, MARIO_ANIM_SLOW_LAND_FROM_DIVE);
    if (is_anim_at_end(m)) {
        set_mario_action(m, m->forwardVel != 0.0f ? ACT_DECELERATING : ACT_IDLE, 0);
    }
    return FALSE;
}

s32 act_picking_up_bowser(struct MarioState *m) {
    if (m->actionState == ACT_STATE_PICKING_UP_GRAB) {
        m->actionState = ACT_STATE_PICKING_UP_HOLDING;
        m->angleVel[1] = 0;
        bowser_throw_reset_history();
        m->marioBodyState->grabPos = GRAB_POS_BOWSER;
        mario_grab_used_object(m);
        queue_rumble_data(5, 80);
        play_sound(SOUND_MARIO_HRMM, m->marioObj->header.gfx.cameraToObject);
    }

    set_mario_animation(m, MARIO_ANIM_GRAB_BOWSER);
    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_HOLDING_BOWSER, 0);
    }

    stationary_ground_step(m);
    return FALSE;
}

s32 act_holding_bowser(struct MarioState *m) {
    s16 spin;

    if (m->input & INPUT_B_PRESSED) {
        bowser_throw_assist(m);
        if (m->angleVel[1] <= -0xE00 || m->angleVel[1] >= 0xE00) {
            play_sound(SOUND_MARIO_SO_LONGA_BOWSER, m->marioObj->header.gfx.cameraToObject);
        } else {
            play_sound(SOUND_MARIO_HERE_WE_GO, m->marioObj->header.gfx.cameraToObject);
        }
        return set_mario_action(m, ACT_RELEASING_BOWSER, 0);
    }

    if (m->angleVel[1] == 0) {
        set_mario_animation(m, MARIO_ANIM_HOLDING_BOWSER);
    } else {
        m->actionTimer = 0;
        set_mario_animation(m, MARIO_ANIM_SWINGING_BOWSER);
    }

    bowser_throw_record(m);
    bowser_update_spin(m);

    // spin = starting yaw
    spin = m->faceAngle[1];
    m->faceAngle[1] += m->angleVel[1];

    // play sound on overflow
    if (m->angleVel[1] <= -0x100 && spin < m->faceAngle[1]) {
        queue_rumble_data(4, 20);
        play_sound(SOUND_OBJ_BOWSER_SPINNING, m->marioObj->header.gfx.cameraToObject);
    }
    if (m->angleVel[1] >= 0x100 && spin > m->faceAngle[1]) {
        queue_rumble_data(4, 20);
        play_sound(SOUND_OBJ_BOWSER_SPINNING, m->marioObj->header.gfx.cameraToObject);
    }

    stationary_ground_step(m);
    if (m->angleVel[1] >= 0) {
        m->marioObj->header.gfx.angle[0] = -m->angleVel[1];
    } else {
        m->marioObj->header.gfx.angle[0] = m->angleVel[1];
    }

    return FALSE;
}

s32 act_releasing_bowser(struct MarioState *m) {
    if (++m->actionTimer == 1) {
        if (m->actionArg == 0) {
            queue_rumble_data(5, 50);
            mario_throw_held_object(m);
        } else {
            queue_rumble_data(4, 50);
            mario_drop_held_object(m);
        }
    }

    m->angleVel[1] = 0;
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    if (animInfo->animID == MARIO_ANIM_RELEASE_BOWSER && animInfo->animFrame > 25) {
        if (m->automaticAFrames > 0) {
            m->input |= INPUT_A_PRESSED;
        }
        if (m->automaticBFrames > 0) {
            m->input |= INPUT_B_PRESSED;
        }
        if (m->automaticZFrames > 0) {
            m->input |= INPUT_Z_DOWN;
            m->zCrouchBuffered = TRUE;
            m->zHeldFrames = MAX(1, m->zHeldFrames);
        }
        if (check_common_idle_cancels(m)) {
            return TRUE;
        }
    }
    animated_stationary_ground_step(m, MARIO_ANIM_RELEASE_BOWSER, ACT_IDLE);
    return FALSE;
}

s32 check_common_object_cancels(struct MarioState *m) {
    f32 waterSurface = m->waterLevel - 100;
    if (m->pos[1] < waterSurface && !mario_has_land_movement_underwater(m)) {
        return set_water_plunge_action(m);
    }

    if (m->input & INPUT_SQUISHED) {
        return drop_and_set_mario_action(m, ACT_SQUISHED, 0);
    }

    if (m->health < 0x100) {
        return drop_and_set_mario_action(m, ACT_STANDING_DEATH, 0);
    }

    return FALSE;
}

s32 mario_execute_object_action(struct MarioState *m) {
    s32 cancel = FALSE;

    if (check_common_object_cancels(m)) {
        return TRUE;
    }

    if (mario_update_quicksand(m, 0.5f)) {
        return TRUE;
    }

    /* clang-format off */
    switch (m->action) {
        case ACT_PUNCHING:           cancel = act_punching(m);           break;
        case ACT_PICKING_UP:         cancel = act_picking_up(m);         break;
        case ACT_DIVE_PICKING_UP:    cancel = act_dive_picking_up(m);    break;
        case ACT_STOMACH_SLIDE_STOP: cancel = act_stomach_slide_stop(m); break;
        case ACT_PLACING_DOWN:       cancel = act_placing_down(m);       break;
        case ACT_THROWING:           cancel = act_throwing(m);           break;
        case ACT_HEAVY_THROW:        cancel = act_heavy_throw(m);        break;
        case ACT_PICKING_UP_BOWSER:  cancel = act_picking_up_bowser(m);  break;
        case ACT_HOLDING_BOWSER:     cancel = act_holding_bowser(m);     break;
        case ACT_RELEASING_BOWSER:   cancel = act_releasing_bowser(m);   break;
    }
    /* clang-format on */

    if (!cancel && (m->input & INPUT_IN_WATER)) {
        m->particleFlags |= PARTICLE_IDLE_WATER_WAVE;
    }

    return cancel;
}
