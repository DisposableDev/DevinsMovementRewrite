#include <PR/ultratypes.h>

#include "sm64.h"
#include "game_options.h"
#include "mario_actions_stationary.h"
#include "mario_actions_object.h"
#include "mario_actions_airborne.h"
#include "mario_actions_moving.h"
#include "mario_punch.h"
#include "area.h"
#include "audio/external.h"
#include "behavior_data.h"
#include "camera.h"
#include "castle_lobby_finale.h"
#include "engine/math_util.h"
#include "interaction.h"
#include "level_update.h"
#include "mario.h"
#include "mario_he_thicc.h"
#include "mario_character.h"
#include "mario_dab.h"
#include "mario_spin_dizzy.h"
#include "mario_step.h"
#include "memory.h"
#include "save_file.h"
#include "sound_init.h"
#include "surface_terrains.h"
#include "rumble_init.h"

s32 check_common_idle_cancels(struct MarioState *m) {
    mario_drop_held_object(m);
    if (m->floor->normal.y < COS73 && m->floor->type != SURFACE_MARIO_ONLY_GRIPPY) {
        return mario_push_off_steep_floor(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        if ((m->input & INPUT_Z_DOWN) && m->zHeldFrames == 1) {
            return set_jumping_action(m, ACT_BACKFLIP, 0);
        }
        return set_jumping_action(m, ACT_JUMP, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_FIRST_PERSON) {
        return set_mario_action(m, ACT_FIRST_PERSON, 0);
    }

    if (m->zCrouchBuffered && m->zHeldFrames >= 1) {
        m->zCrouchBuffered = FALSE;
        return set_mario_action(m, absf(m->forwardVel) >= 1.0f ? ACT_CROUCH_SLIDE : ACT_START_CROUCHING,
                                0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        m->faceAngle[1] = (s16) m->intendedYaw;
        return set_mario_action(m, ACT_WALKING, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, absf(m->forwardVel) >= 1.0f ? ACT_MOVE_PUNCHING : ACT_PUNCHING, 0);
    }

    if (((m->input & INPUT_Z_DOWN) || m->zCrouchBuffered) && m->zHeldFrames >= 1) {
        m->zCrouchBuffered = FALSE;
        return set_mario_action(m, absf(m->forwardVel) >= 1.0f ? ACT_CROUCH_SLIDE : ACT_START_CROUCHING,
                                0);
    }

    return FALSE;
}

s32 check_common_hold_idle_cancels(struct MarioState *m) {
    if (m->floor->normal.y < COS73 && m->floor->type != SURFACE_MARIO_ONLY_GRIPPY) {
        return mario_push_off_steep_floor(m, ACT_HOLD_FREEFALL, 0);
    }

    if (m->heldObj->oInteractionSubtype & INT_SUBTYPE_DROP_IMMEDIATELY) {
        m->heldObj->oInteractionSubtype =
            (s32)(m->heldObj->oInteractionSubtype & ~INT_SUBTYPE_DROP_IMMEDIATELY);
        return set_mario_action(m, ACT_PLACING_DOWN, 0);
    }

    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        return set_jumping_action(m, ACT_HOLD_JUMP, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_HOLD_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_HOLD_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        m->faceAngle[1] = (s16) m->intendedYaw;
        return set_mario_action(m, ACT_HOLD_WALKING, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_THROWING, 0);
    }

    if (m->input & INPUT_Z_DOWN) {
        return drop_and_set_mario_action(m, ACT_START_CROUCHING, 0);
    }

    return FALSE;
}

//! HACKERSM64_DO: actionArg names
s32 act_idle(struct MarioState *m) {
    if (m->quicksandDepth > 30.0f) {
        return set_mario_action(m, ACT_IN_QUICKSAND, 0);
    }

    if (m->input & INPUT_IN_POISON_GAS) {
        return set_mario_action(m, ACT_COUGHING, 0);
    }

    if (!(m->actionArg & 1) && m->health < 0x300) {
        return set_mario_action(m, ACT_PANTING, 0);
    }

    if (check_common_idle_cancels(m)) {
        return TRUE;
    }

    if (m->controller->buttonDown != 0) {
        m->actionTimer = 0;
    }

#ifndef NO_SLEEP
    if (!(m->actionArg & 1)) {
        s32 capless = !(m->flags & MARIO_CAPS);
        s32 cold = (m->area->terrainType & TERRAIN_MASK) == TERRAIN_SNOW;
        s32 sleepDelay;
        s32 heThicc = mario_character_palette() == MARIO_PALETTE_HE_THICC;
        if (heThicc) {
            sleepDelay = HE_THICC_IDLE_SLEEP_FRAMES;
            cold = FALSE;
        } else if (cold) {
            sleepDelay = capless ? IDLE_SHIVER_DELAY_CAPLESS_FRAMES : IDLE_SHIVER_DELAY_CAPPED_FRAMES;
        } else {
            sleepDelay = capless ? IDLE_SLEEP_DELAY_CAPLESS_FRAMES : IDLE_SLEEP_DELAY_CAPPED_FRAMES;
        }
        if (m->actionTimer < sleepDelay) {
            m->actionTimer++;
        }
        if (m->actionTimer >= sleepDelay && !(m->floor->flags & SURFACE_FLAG_DYNAMIC)) {
            f32 deltaYOfFloorBehindMario =
                m->pos[1] - find_floor_height_relative_polar(m, -0x8000, 60.0f);
            if (deltaYOfFloorBehindMario >= -24.0f && deltaYOfFloorBehindMario <= 24.0f) {
                set_mario_action(m, cold ? ACT_SHIVERING : ACT_START_SLEEPING, 0);
                if (heThicc) {
                    m->actionState = ACT_STATE_START_SLEEPING_YAWN;
                }
                return TRUE;
            }
        }
    }
#endif

    if (m->actionArg & 1) {
        set_mario_animation(m, MARIO_ANIM_STAND_AGAINST_WALL);
    } else {
        switch (m->actionState) {
            case ACT_STATE_IDLE_HEAD_LEFT:
                set_mario_animation(m, MARIO_ANIM_IDLE_HEAD_LEFT);
                break;

            case ACT_STATE_IDLE_HEAD_RIGHT:
                set_mario_animation(m, MARIO_ANIM_IDLE_HEAD_RIGHT);
                break;

            case ACT_STATE_IDLE_HEAD_CENTER:
                set_mario_animation(m, MARIO_ANIM_IDLE_HEAD_CENTER);
                break;
        }

        if (is_anim_at_end(m) && ++m->actionState == ACT_STATE_IDLE_RESET_OR_SLEEP) {
            m->actionState = ACT_STATE_IDLE_HEAD_LEFT;
        }
    }

    stationary_ground_step(m);

    return FALSE;
}

void play_anim_sound(struct MarioState *m, u32 actionState, s32 animFrame, u32 sound) {
    if (m->actionState == actionState && m->marioObj->header.gfx.animInfo.animFrame == animFrame) {
        play_sound(sound, m->marioObj->header.gfx.cameraToObject);
    }
}

static u32 sleep_input_during_dab_lock(struct MarioState *m, s32 consumeFrame) {
    if (m->actionArg == 0) {
        return m->input;
    }

    // actionArg counts the remaining updates of a dab-triggered sleep lock.
    // Only voluntary actions are suppressed; floor and damage exits still run.
    if (consumeFrame) {
        m->actionArg--;
    }
    m->zCrouchBuffered = FALSE;
    return m->input
           & ~(INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_DOWN | INPUT_Z_PRESSED
               | INPUT_FIRST_PERSON);
}

s32 act_start_sleeping(struct MarioState *m) {
    s32 animFrame = 0;
    u32 savedInput = m->input;

    // The finished animation redispatches to ACT_SLEEPING this update, so
    // that action owns this update's one countdown step.
    m->input = sleep_input_during_dab_lock(m, m->actionState != ACT_STATE_START_SLEEPING_SLEEP);
    s32 canceled = check_common_idle_cancels(m);
    m->input = savedInput;
    if (canceled) {
        return TRUE;
    }

    if (m->quicksandDepth > 30.0f) {
        return set_mario_action(m, ACT_IN_QUICKSAND, 0);
    }

    if (m->actionState == ACT_STATE_START_SLEEPING_SLEEP) {
        return set_mario_action(m, ACT_SLEEPING, m->actionArg);
    }

    switch (m->actionState) {
        case ACT_STATE_START_SLEEPING_IDLE:
            animFrame = set_mario_animation(m, MARIO_ANIM_START_SLEEP_IDLE);
            break;

        case ACT_STATE_START_SLEEPING_SCRATCH:
            animFrame = set_mario_animation(m, MARIO_ANIM_START_SLEEP_SCRATCH);
            break;

        case ACT_STATE_START_SLEEPING_YAWN:
            animFrame = set_mario_animation(m, MARIO_ANIM_START_SLEEP_YAWN);
            m->marioBodyState->eyeState = MARIO_EYES_HALF_CLOSED;
            break;

        case ACT_STATE_START_SLEEPING_SITTING:
            animFrame = set_mario_animation(m, MARIO_ANIM_START_SLEEP_SITTING);
            m->marioBodyState->eyeState = MARIO_EYES_HALF_CLOSED;
            break;
    }

    play_anim_sound(m, ACT_STATE_START_SLEEPING_SCRATCH, 41, SOUND_ACTION_PAT_BACK);
    play_anim_sound(m, ACT_STATE_START_SLEEPING_SCRATCH, 49, SOUND_ACTION_PAT_BACK);
    play_anim_sound(m, ACT_STATE_START_SLEEPING_SITTING, 15, m->terrainSoundAddend + SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);

    if (is_anim_at_end(m)) {
        m->actionState++;
    }

    if (m->actionState == ACT_STATE_START_SLEEPING_YAWN && animFrame == -1) {
        play_sound(SOUND_MARIO_YAWNING, m->marioObj->header.gfx.cameraToObject);
    }

    if (m->actionState == ACT_STATE_START_SLEEPING_SCRATCH && animFrame == -1) {
        play_sound(SOUND_MARIO_IMA_TIRED, m->marioObj->header.gfx.cameraToObject);
    }

    stationary_ground_step(m);
    return FALSE;
}

s32 act_sleeping(struct MarioState *m) {
    s32 animFrame;
    u32 input = sleep_input_during_dab_lock(m, TRUE);
    if (input
        & (INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE
           | INPUT_FIRST_PERSON | INPUT_STOMPED | INPUT_B_PRESSED | INPUT_Z_PRESSED)) {
        return set_mario_action(m, ACT_WAKING_UP, m->actionState);
    }

    if (m->quicksandDepth > 30.0f) {
        return set_mario_action(m, ACT_WAKING_UP, m->actionState);
    }

    if (m->pos[1] - find_floor_height_relative_polar(m, -0x8000, 60.0f) > 24.0f) {
        return set_mario_action(m, ACT_WAKING_UP, m->actionState);
    }

    m->marioBodyState->eyeState = MARIO_EYES_CLOSED;
    stationary_ground_step(m);
    switch (m->actionState) {
        case ACT_SLEEPING_STATE_IDLE:
            animFrame = set_mario_animation(m, MARIO_ANIM_SLEEP_IDLE);

            if (animFrame == -1 && !m->actionTimer) {
                lower_background_noise(2);
            }

            if (animFrame == 2) {
                play_sound(SOUND_MARIO_SNORING1, m->marioObj->header.gfx.cameraToObject);
            }

            if (animFrame == 20) {
                play_sound(SOUND_MARIO_SNORING2, m->marioObj->header.gfx.cameraToObject);
            }

            if (is_anim_at_end(m)) {
                m->actionTimer++;
                if (m->actionTimer > 45) {
                    m->actionState = ACT_SLEEPING_STATE_START_LYING;
                }
            }
            break;

        case ACT_SLEEPING_STATE_START_LYING:
            if (set_mario_animation(m, MARIO_ANIM_SLEEP_START_LYING) == 18) {
                play_mario_heavy_landing_sound(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
            }

            if (is_anim_at_end(m)) {
                m->actionState = ACT_SLEEPING_STATE_LYING;
            }
            break;

        case ACT_SLEEPING_STATE_LYING:
            animFrame = set_mario_animation(m, MARIO_ANIM_SLEEP_LYING);
            play_sound_if_no_flag(m, SOUND_MARIO_SNORING3, MARIO_ACTION_SOUND_PLAYED);
            break;
    }
    return FALSE;
}

s32 act_waking_up(struct MarioState *m) {
    if (!m->actionTimer) {
        // Keep the entry cap state throughout recovery, including cap changes.
        m->actionState = !(m->flags & MARIO_CAPS);
        stop_sound(SOUND_MARIO_SNORING1, m->marioObj->header.gfx.cameraToObject);
        stop_sound(SOUND_MARIO_SNORING2, m->marioObj->header.gfx.cameraToObject);
        stop_sound(SOUND_MARIO_SNORING3, m->marioObj->header.gfx.cameraToObject);
        raise_background_noise(2);
    }

    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    m->actionTimer++;

    if (m->actionTimer > (m->actionState ? 40 : 20)) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    stationary_ground_step(m);

    s32 animation = !m->actionArg ? MARIO_ANIM_WAKE_FROM_SLEEP : MARIO_ANIM_WAKE_FROM_LYING;
    if (m->actionState) {
        set_mario_anim_with_accel(m, animation, 0x8000);
    } else {
        set_mario_animation(m, animation);
    }

    return FALSE;
}

s32 act_shivering(struct MarioState *m) {
    s32 animFrame;

    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->input
        & (INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE
           | INPUT_FIRST_PERSON | INPUT_STOMPED | INPUT_B_PRESSED | INPUT_Z_PRESSED)) {
        m->actionState = ACT_STATE_SHIVERING_RETURN_TO_IDLE;
    }

    stationary_ground_step(m);
    switch (m->actionState) {
        case ACT_STATE_SHIVERING_WARMING_HAND:
            animFrame = set_mario_animation(m, MARIO_ANIM_SHIVERING_WARMING_HAND);
            if (animFrame == 49) {
                m->particleFlags |= PARTICLE_BREATH;
                play_sound(SOUND_MARIO_PANTING_COLD, m->marioObj->header.gfx.cameraToObject);
            }
            if (animFrame == 7 || animFrame == 81) {
                play_sound(SOUND_ACTION_CLAP_HANDS_COLD, m->marioObj->header.gfx.cameraToObject);
            }
            if (is_anim_past_end(m)) {
                m->actionState = ACT_STATE_SHIVERING_SHAKE;
            }
            break;

        case ACT_STATE_SHIVERING_SHAKE:
            animFrame = set_mario_animation(m, MARIO_ANIM_SHIVERING);
            if (animFrame == 9 || animFrame == 25 || animFrame == 44) {
                play_sound(SOUND_ACTION_CLAP_HANDS_COLD, m->marioObj->header.gfx.cameraToObject);
            }
            break;

        case ACT_STATE_SHIVERING_RETURN_TO_IDLE:
            set_mario_animation(m, MARIO_ANIM_SHIVERING_RETURN_TO_IDLE);
            if (is_anim_past_end(m)) {
                set_mario_action(m, ACT_IDLE, 0);
            }
            break;
    }
    return FALSE;
}

s32 act_coughing(struct MarioState *m) {
    s32 animFrame;
    if (m->actionArg == ACT_ARG_COUGHING_SHALLOW_WATER) {
        u32 savedInput = m->input;
        m->input &= ~(INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_DOWN
                      | INPUT_Z_PRESSED | INPUT_FIRST_PERSON);
        m->zCrouchBuffered = FALSE;
        s32 canceled = check_common_idle_cancels(m);
        m->input = savedInput;
        if (canceled) {
            return TRUE;
        }
    } else if (check_common_idle_cancels(m)) {
        return TRUE;
    }

    stationary_ground_step(m);
    animFrame = set_mario_animation(m, MARIO_ANIM_COUGHING);
    if (animFrame == 25 || animFrame == 35) {
        play_sound(SOUND_MARIO_COUGHING3, m->marioObj->header.gfx.cameraToObject);
    }

    if (animFrame == 50 || animFrame == 58) {
        play_sound(SOUND_MARIO_COUGHING2, m->marioObj->header.gfx.cameraToObject);
    }

    if (animFrame == 71 || animFrame == 80) {
        play_sound(SOUND_MARIO_COUGHING1, m->marioObj->header.gfx.cameraToObject);
    }

    if (m->actionArg == ACT_ARG_COUGHING_SHALLOW_WATER && is_anim_at_end(m)) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    return FALSE;
}

s32 act_hold_idle(struct MarioState *m) {
    if (segmented_to_virtual(&bhvJumpingBox) == m->heldObj->behavior) {
        return set_mario_action(m, ACT_CRAZY_BOX_BOUNCE, 0);
    }

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_IDLE, 0);
    }

    if (m->quicksandDepth > 30.0f) {
        return drop_and_set_mario_action(m, ACT_IN_QUICKSAND, 0);
    }

    if (check_common_hold_idle_cancels(m)) {
        return TRUE;
    }

    stationary_ground_step(m);
    set_mario_animation(m, MARIO_ANIM_IDLE_WITH_LIGHT_OBJ);
    return FALSE;
}

s32 act_hold_heavy_idle(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return drop_and_set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return drop_and_set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        return set_mario_action(m, ACT_HOLD_HEAVY_WALKING, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_HEAVY_THROW, 0);
    }

    if (coasting_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        drop_and_set_mario_action(m, ACT_FREEFALL, 0);
        return FALSE;
    }
    set_mario_animation(m, MARIO_ANIM_IDLE_HEAVY_OBJ);
    return FALSE;
}

s32 act_standing_against_wall(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & (INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE)) {
        return check_common_action_exits(m);
    }

    if (m->input & INPUT_FIRST_PERSON) {
        return set_mario_action(m, ACT_FIRST_PERSON, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_PUNCHING, 0);
    }

    set_mario_animation(m, MARIO_ANIM_STAND_AGAINST_WALL);
    stationary_ground_step(m);
    return FALSE;
}

s32 act_in_quicksand(struct MarioState *m) {
    if (m->quicksandDepth < 30.0f) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    if (check_common_idle_cancels(m)) {
        return TRUE;
    }

    if (m->quicksandDepth > 70.0f) {
        set_mario_animation(m, MARIO_ANIM_DYING_IN_QUICKSAND);
    } else {
        set_mario_animation(m, MARIO_ANIM_IDLE_IN_QUICKSAND);
    }

    stationary_ground_step(m);
    return FALSE;
}

static s32 start_stationary_crouch_attack(struct MarioState *m) {
    if (m->controller->stickMag >= MARIO_STRONG_STICK_THRESHOLD) {
        return set_mario_action(m, ACT_SLIDE_KICK, 0);
    }
    return set_mario_action(m, ACT_PUNCHING, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
}

s32 act_crouching(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        if (m->backwardLongJumpCrouchFrames > 0 && m->backwardLongJumpChain
            && m->controller->stickMag > 0.0f && (m->input & INPUT_Z_DOWN)) {
            return set_jumping_action(m, ACT_LONG_JUMP, 0);
        }
        return set_jumping_action(m, ACT_BACKFLIP, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_FIRST_PERSON) {
        return set_mario_action(m, ACT_STOP_CROUCHING, 0);
    }

    if (!(m->input & INPUT_Z_DOWN)) {
        return set_mario_action(m, ACT_STOP_CROUCHING, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return start_stationary_crouch_attack(m);
    }

    // Keep the brief chain window in crouching instead of immediately crawling.
    if ((m->input & INPUT_NONZERO_ANALOG) && m->backwardLongJumpCrouchFrames == 0) {
        return set_mario_action(m, ACT_START_CRAWLING, 0);
    }

    if (coasting_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        set_mario_action(m, ACT_FREEFALL, 0);
        return FALSE;
    }
    set_mario_animation(m, MARIO_ANIM_CROUCHING);
    return FALSE;
}

s32 act_panting(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->health >= 0x500) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    if (check_common_idle_cancels(m)) {
        return TRUE;
    }

    if (set_mario_animation(m, MARIO_ANIM_WALK_PANTING) == 1) {
        play_sound(SOUND_MARIO_PANTING + ((gAudioRandom % 3U) << 0x10),
                   m->marioObj->header.gfx.cameraToObject);
    }

    stationary_ground_step(m);
    m->marioBodyState->eyeState = MARIO_EYES_HALF_CLOSED;
    return FALSE;
}

s32 act_hold_panting_unused(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_PANTING, 0);
    }

    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->health >= 0x500) {
        return set_mario_action(m, ACT_HOLD_IDLE, 0);
    }

    if (check_common_hold_idle_cancels(m)) {
        return TRUE;
    }

    set_mario_animation(m, MARIO_ANIM_WALK_PANTING);
    stationary_ground_step(m);
    m->marioBodyState->eyeState = MARIO_EYES_HALF_CLOSED;
    return FALSE;
}

void stopping_step(struct MarioState *m, s32 animID, u32 action) {
    stationary_ground_step(m);
    set_mario_animation(m, animID);
    if (is_anim_at_end(m)) {
        set_mario_action(m, action, 0);
    }
}

s32 act_braking_stop(struct MarioState *m) {
    if (check_common_landing_cancels(m, ACT_JUMP)) {
        return TRUE;
    }

    stopping_step(m, MARIO_ANIM_STOP_SKID, ACT_IDLE);
    return FALSE;
}

s32 act_butt_slide_stop(struct MarioState *m) {
    if (check_common_landing_cancels(m, ACT_JUMP)) {
        return TRUE;
    }

    stopping_step(m, MARIO_ANIM_STOP_SLIDE, ACT_IDLE);
    if (m->marioObj->header.gfx.animInfo.animFrame == 6) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
    }

    return FALSE;
}

s32 act_hold_butt_slide_stop(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_IDLE, 0);
    }

    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & (INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE)) {
        return check_common_hold_action_exits(m);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_THROWING, 0);
    }

    stopping_step(m, MARIO_ANIM_STAND_UP_FROM_SLIDING_WITH_LIGHT_OBJ, ACT_HOLD_IDLE);
    return FALSE;
}

s32 act_slide_kick_slide_stop(struct MarioState *m) {
    if (check_common_landing_cancels(m, ACT_JUMP)) {
        return TRUE;
    }

    stopping_step(m, MARIO_ANIM_CROUCH_FROM_SLIDE_KICK, ACT_CROUCHING);
    return FALSE;
}

s32 act_start_crouching(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        return set_jumping_action(m, ACT_BACKFLIP, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return start_stationary_crouch_attack(m);
    }

    if (!(m->input & INPUT_Z_DOWN) || (m->input & INPUT_FIRST_PERSON)) {
        return set_mario_action(m, ACT_STOP_CROUCHING, 0);
    }

    if (coasting_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        set_mario_action(m, ACT_FREEFALL, 0);
        return FALSE;
    }
    // Skip only a fresh standing entry; a stopped slide retains its current pose.
    s32 startingAnimation = m->marioObj->header.gfx.animInfo.animID != MARIO_ANIM_START_CROUCHING;
    set_mario_animation(m, MARIO_ANIM_START_CROUCHING);
    if (startingAnimation) {
        set_anim_to_frame(m, 2);
    }
    if (is_anim_at_end(m)) {
        // Finish lowering from standing before the crouch-to-crawl animation.
        set_mario_action(m, (m->input & INPUT_NONZERO_ANALOG) ? ACT_START_CRAWLING : ACT_CROUCHING, 0);
    }
    return FALSE;
}

s32 act_stop_crouching(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if ((m->input & INPUT_Z_DOWN)
        && !(m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_FIRST_PERSON | INPUT_ABOVE_SLIDE))) {
        // Re-pressing Z reverses directly, without idle's delayed crouch input.
        return set_mario_action(m, ACT_START_CROUCHING, 0);
    }

    set_mario_animation(m, MARIO_ANIM_STOP_CROUCHING);
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    s32 standingInput = m->input
                        & (INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_NONZERO_ANALOG | INPUT_FIRST_PERSON
                           | INPUT_Z_DOWN);
    if (is_anim_past_end(m)
        || (standingInput && animInfo->animFrame >= animInfo->curAnim->loopEnd - 7)) {
        // Standing inputs open at animation frame 3 (player frame 4), without
        // shortening an uninterrupted stand-up or taking a second ground step.
        return set_mario_action(m, m->forwardVel != 0.0f ? ACT_DECELERATING : ACT_IDLE, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        return set_jumping_action(m, ACT_BACKFLIP, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if ((m->input & INPUT_Z_DOWN) && !(m->input & INPUT_FIRST_PERSON)) {
        return set_mario_action(m, ACT_START_CROUCHING, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_PUNCHING, 0);
    }

    if (coasting_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        set_mario_action(m, ACT_FREEFALL, 0);
        return FALSE;
    }
    return FALSE;
}

s32 act_start_crawling(struct MarioState *m) {
    if (m->input & INPUT_FIRST_PERSON) {
        return set_mario_action(m, ACT_STOP_CROUCHING, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    if ((m->input & INPUT_A_PRESSED) && m->controller->stickMag >= MARIO_STRONG_STICK_THRESHOLD
        && animInfo->animID == MARIO_ANIM_START_CRAWLING && animInfo->animFrame >= 0
        && animInfo->animFrame < 4) {
        return set_jumping_action(m, ACT_LONG_JUMP, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_PUNCHING, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
    }

    if (coasting_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        set_mario_action(m, ACT_FREEFALL, 0);
        return FALSE;
    }
    set_mario_animation(m, MARIO_ANIM_START_CRAWLING);
    if (is_anim_past_end(m)) {
        set_mario_action(m, ACT_CRAWLING, 0);
    }

    return FALSE;
}

s32 act_stop_crawling(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    set_mario_animation(m, MARIO_ANIM_STOP_CRAWLING);
    if (m->marioObj->header.gfx.animInfo.animFrame >= 2
        && ((m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_NONZERO_ANALOG | INPUT_FIRST_PERSON))
            || !(m->input & INPUT_Z_DOWN))) {
        // Redispatch through the legal crouching or standing controls on this update.
        u32 action = ACT_CROUCHING;
        if (!(m->input & INPUT_Z_DOWN) || (m->input & INPUT_FIRST_PERSON)) {
            action = m->forwardVel != 0.0f ? ACT_DECELERATING : ACT_IDLE;
        }
        return set_mario_action(m, action, 0);
    }

    if (coasting_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        set_mario_action(m, ACT_FREEFALL, 0);
        return FALSE;
    }
    if (is_anim_past_end(m)) {
        set_mario_action(m, ACT_CROUCHING, 0);
    }
    return FALSE;
}

s32 act_shockwave_bounce(struct MarioState *m) {
    u32 impact =
        m->marioObj->oInteractStatus
        & (INT_STATUS_MARIO_STUNNED | INT_STATUS_MARIO_KNOCKBACK_DMG | INT_STATUS_MARIO_SHOCKWAVE);
    if (game_options_get(GO_BOWSER_FIGHTS) == 1) {
        if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_SHOCKWAVE) {
            queue_rumble_data(70, 40);
            return hurt_and_set_mario_action(m, ACT_SHOCKED, 0, 4);
        }
        if (m->actionTimer == 0 && (m->marioObj->oInteractStatus & INT_STATUS_MARIO_KNOCKBACK_DMG)) {
            queue_rumble_data(70, 40);
            return hurt_and_set_mario_action(m, ACT_BACKWARD_GROUND_KB, 0, 12);
        }
    } else if (impact && m->actionTimer == 0) {
        return mario_bowser_impact_knockback(m, FALSE);
    }

    if (++m->actionTimer == 48) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    s16 bounceTimer = (m->actionTimer % 16) << 12;
    f32 bounceAmt = (f32) (((f32) (6 - m->actionTimer / 8) * 8.0f) + 4.0f);
    mario_set_forward_vel(m, 0);
    vec3_zero(m->vel);
    if (sins(bounceTimer) >= 0.0f) {
        m->pos[1] = sins(bounceTimer) * bounceAmt + m->floorHeight;
    } else {
        m->pos[1] = m->floorHeight - sins(bounceTimer) * bounceAmt;
    }

    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    set_mario_animation(m, MARIO_ANIM_A_POSE);
    return FALSE;
}

s32 landing_step(struct MarioState *m, s32 animID, u32 action) {
    stationary_ground_step(m);
    set_mario_animation(m, animID);
    if (is_anim_at_end(m)) {
        return set_mario_action(m, action, 0);
    }
    return FALSE;
}

s32 check_common_landing_cancels(struct MarioState *m, u32 action) {
    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }
    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->input & INPUT_FIRST_PERSON) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        action = landing_jump_action(m, action);
        if (!action) {
            return set_jump_from_landing(m);
        } else {
            return set_jumping_action(m, action, 0);
        }
    }
    if (check_landing_attack_or_crouch(m)) {
        return TRUE;
    }

    if (m->input & (INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE)) {
        return check_common_action_exits(m);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_PUNCHING, 0);
    }

    return FALSE;
}

s32 act_jump_land_stop(struct MarioState *m) {
    if (check_common_landing_cancels(m, 0)) {
        return TRUE;
    }

    landing_step(m, MARIO_ANIM_LAND_FROM_SINGLE_JUMP, ACT_IDLE);
    return FALSE;
}

s32 act_double_jump_land_stop(struct MarioState *m) {
    if (check_common_landing_cancels(m, 0)) {
        return TRUE;
    }

    landing_step(m, MARIO_ANIM_LAND_FROM_DOUBLE_JUMP, ACT_IDLE);
    return FALSE;
}

s32 act_side_flip_land_stop(struct MarioState *m) {
    if (check_common_landing_cancels(m, 0)) {
        return TRUE;
    }

    landing_step(m, MARIO_ANIM_SLIDEFLIP_LAND, ACT_IDLE);
    m->marioObj->header.gfx.angle[1] += 0x8000;
    return FALSE;
}

s32 act_freefall_land_stop(struct MarioState *m) {
    if (m->prevAction == ACT_FORWARD_ROLLOUT || m->prevAction == ACT_BACKWARD_ROLLOUT) {
        // Rollout touchdown only selects the next action. Its air step already
        // moved Mario this update, so there is no separate recovery movement.
        if (m->input & (INPUT_STOMPED | INPUT_OFF_FLOOR | INPUT_FIRST_PERSON | INPUT_A_PRESSED)) {
            return check_common_landing_cancels(m, 0);
        }
        if (m->input & INPUT_ABOVE_SLIDE) {
            return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
        }
        if (m->actionArg == ACT_ARG_FORWARD_ROLLOUT_QUEUED_SLIDE_KICK) {
            return set_mario_action(m, ACT_SLIDE_KICK, 0);
        }
        if ((m->input & INPUT_B_PRESSED) && !(m->input & INPUT_Z_DOWN)
            && m->controller->stickMag >= MARIO_STRONG_STICK_THRESHOLD
            && m->prevAction == ACT_BACKWARD_ROLLOUT) {
            mario_set_forward_vel(m, absf(m->forwardVel));
        }
        if (check_ground_dive_or_punch(m) || check_common_landing_cancels(m, 0)) {
            return TRUE;
        }
        return set_mario_action(m, absf(m->forwardVel) >= 1.0f ? ACT_DECELERATING : ACT_IDLE, 0);
    }

    if (check_common_landing_cancels(m, 0)) {
        return TRUE;
    }

    landing_step(m, MARIO_ANIM_GENERAL_LAND, ACT_IDLE);
    return FALSE;
}

s32 act_triple_jump_land_stop(struct MarioState *m) {
    if (check_common_landing_cancels(m, ACT_JUMP)) {
        return TRUE;
    }

    landing_step(m, MARIO_ANIM_TRIPLE_JUMP_LAND, ACT_IDLE);
    return FALSE;
}

s32 act_backflip_land_stop(struct MarioState *m) {
    if ((m->input & INPUT_Z_DOWN) && m->framesSinceA < MARIO_BACKFLIP_LANDING_BUFFER_FRAMES
        && !(m->input & (INPUT_OFF_FLOOR | INPUT_STOMPED | INPUT_FIRST_PERSON))) {
        m->framesSinceA = 0xFF;
        return set_jumping_action(m, ACT_BACKFLIP, 0);
    }
    if (check_common_landing_cancels(m, ACT_JUMP)) {
        return TRUE;
    }

    landing_step(m, MARIO_ANIM_TRIPLE_JUMP_LAND, ACT_IDLE);
    return FALSE;
}

s32 act_lava_boost_land(struct MarioState *m) {
    m->input &= ~(INPUT_FIRST_PERSON | INPUT_B_PRESSED);

    if (check_common_landing_cancels(m, 0)) {
        return TRUE;
    }

    landing_step(m, MARIO_ANIM_STAND_UP_FROM_LAVA_BOOST, ACT_IDLE);
    return FALSE;
}

s32 act_long_jump_land_stop(struct MarioState *m) {
    if (check_common_landing_cancels(m, ACT_JUMP)) {
        return TRUE;
    }

    landing_step(m, !m->marioObj->oMarioLongJumpIsSlow ? MARIO_ANIM_CROUCH_FROM_FAST_LONGJUMP
                                                       : MARIO_ANIM_CROUCH_FROM_SLOW_LONGJUMP,
                 ACT_CROUCHING);
    return FALSE;
}

s32 act_hold_jump_land_stop(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_IDLE, 0);
    }

    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_HOLD_FREEFALL, 0);
    }

    if (!(m->input & (INPUT_A_PRESSED | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE))
        && check_landing_attack_or_crouch(m)) {
        return TRUE;
    }

    if (m->input & (INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE)) {
        return check_common_hold_action_exits(m);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_THROWING, 0);
    }

    landing_step(m, MARIO_ANIM_JUMP_LAND_WITH_LIGHT_OBJ, ACT_HOLD_IDLE);
    return FALSE;
}

s32 act_hold_freefall_land_stop(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_IDLE, 0);
    }

    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_HOLD_FREEFALL, 0);
    }

    if (!(m->input & (INPUT_A_PRESSED | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE))
        && check_landing_attack_or_crouch(m)) {
        return TRUE;
    }

    if (m->input & (INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE)) {
        return check_common_hold_action_exits(m);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_THROWING, 0);
    }
    landing_step(m, MARIO_ANIM_FALL_LAND_WITH_LIGHT_OBJ, ACT_HOLD_IDLE);
    return FALSE;
}

s32 act_air_throw_land(struct MarioState *m) {
    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, m->heldObj != NULL ? ACT_HOLD_FREEFALL : ACT_FREEFALL, 0);
    }

    if (++m->actionTimer == 4) {
        mario_throw_held_object(m);
    }

    set_mario_animation(m, MARIO_ANIM_THROW_LIGHT_OBJECT);
    if (m->heldObj == NULL && m->marioObj->header.gfx.animInfo.animFrame >= 6
        && check_common_landing_cancels(m, ACT_JUMP)) {
        return TRUE;
    }

    if (coasting_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        // A very early touchdown must not reset the pending release timer.
        m->action = ACT_AIR_THROW;
        return FALSE;
    }
    if (m->heldObj == NULL && is_anim_at_end(m)) {
        set_mario_action(m, absf(m->forwardVel) >= 1.0f ? ACT_DECELERATING : ACT_IDLE, 0);
    }
    return FALSE;
}

s32 act_twirl_land(struct MarioState *m) {
    m->actionState = ACT_STATE_TWIRL_LAND_1;
    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    set_mario_animation(m, MARIO_ANIM_TWIRL_LAND);
    if (m->automaticAFrames > 0) {
        m->input |= INPUT_A_PRESSED;
    }
    if (m->automaticBFrames > 0) {
        m->input |= INPUT_B_PRESSED;
    }
    if (m->automaticZFrames > 0) {
        m->input |= INPUT_Z_DOWN;
    }
    s16 baseYaw = m->faceAngle[1];
    // A cancelled spin keeps Mario's visible facing for the next action.
    m->faceAngle[1] += m->twirlYaw;
    if (check_common_landing_cancels(m, ACT_JUMP)) {
        m->twirlYaw = 0;
        m->angleVel[1] = 0;
        m->marioObj->header.gfx.angle[1] = m->faceAngle[1];
        return TRUE;
    }
    m->faceAngle[1] = baseYaw;

    stationary_ground_step(m);
    if (m->angleVel[1] > 0) {
        m->angleVel[1] -= 0x400;
        if (m->angleVel[1] < 0) {
            m->angleVel[1] = 0;
        }

        m->twirlYaw += m->angleVel[1];
    }

    m->marioObj->header.gfx.angle[1] += m->twirlYaw;
    if (is_anim_at_end(m) && m->angleVel[1] == 0) {
        m->faceAngle[1] += m->twirlYaw;
        set_mario_action(m, ACT_IDLE, 0);
    }

    return FALSE;
}

s32 act_ground_pound_land(struct MarioState *m) {
    m->actionState = ACT_STATE_GROUND_POUND_LAND_1;
    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (mario_he_thicc_try_pound_bounce(m)) {
        return TRUE;
    }
    if (mario_character_palette() == MARIO_PALETTE_HE_THICC) {
        m->actionTimer = MIN(m->actionTimer + 1, HE_THICC_BELLY_JUMP_FRAMES);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BUTT_SLIDE, 0);
    }

    landing_step(m, MARIO_ANIM_GROUND_POUND_LANDING, ACT_BUTT_SLIDE_STOP);
    return FALSE;
}

static s32 castle_secret_lobby_looking_at_sun(struct MarioState *m) {
    s16 yaw;
    s16 yawDifference;
    if (!castle_lobby_finale_sun_ready() || castle_secret_lobby_rejection_dialog() >= 0
        || m->statusForCamera->headRotation[0] != -0x1800
        || (gCameraMovementFlags & CAM_MOVING_INTO_MODE)) {
        return FALSE;
    }
    yaw = atan2s(gLakituState.focus[2] - gLakituState.pos[2],
                 gLakituState.focus[0] - gLakituState.pos[0]);
    yawDifference = yaw - CASTLE_SECRET_LOBBY_SUN_YAW;
    return yawDifference >= -0x800 && yawDifference <= 0x800;
}

s32 act_first_person(struct MarioState *m) {
    s32 exit = (m->input & (INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE | INPUT_STOMPED)) != 0;

    if (m->actionState == ACT_STATE_FIRST_PERSON_SET_MODE) {
        lower_background_noise(2);
        set_camera_mode(m->area->camera, CAMERA_MODE_C_UP, 16);
        m->actionState = ACT_STATE_FIRST_PERSON_IDLE;
    } else if (!(m->input & INPUT_FIRST_PERSON) || exit) {
        raise_background_noise(2);
        // Go back to the last camera mode
        set_camera_mode(m->area->camera, -1, 1);
        return set_mario_action(m, ACT_IDLE, 0);
    }

    if (castle_secret_lobby_looking_at_sun(m)) {
        level_warp_to_secret_lobby_cake();
        // The transition freezes input; keep the visible pose for the synchronized fade.
        return FALSE;
    }

#ifdef UNLOCK_ALL
    if (m->floor->type == SURFACE_LOOK_UP_WARP) {
#else
    if (m->floor->type == SURFACE_LOOK_UP_WARP
        && save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) >= 10) {
#endif
        s16 headRX = m->statusForCamera->headRotation[0];
        s16 totalRY = ((m->statusForCamera->headRotation[1] * 4) / 3) + m->faceAngle[1];
        if (headRX == -0x1800 && (totalRY < -0x6FFF || totalRY >= 0x7000)) {
            level_trigger_warp(m, WARP_OP_LOOK_UP);
        }
    }

    stationary_ground_step(m);
    set_mario_animation(m, MARIO_ANIM_FIRST_PERSON);
    return FALSE;
}

s32 check_common_stationary_cancels(struct MarioState *m) {
    if (m->pos[1] < m->waterLevel - 100 && !mario_has_land_movement_underwater(m)) {
        if (m->action == ACT_SPAWN_SPIN_LANDING) {
            load_level_init_text(0);
        }
        update_mario_sound_and_camera(m);
        return set_water_plunge_action(m);
    }

    if (m->input & INPUT_SQUISHED) {
        update_mario_sound_and_camera(m);
        return drop_and_set_mario_action(m, ACT_SQUISHED, 0);
    }

    if (m->action != ACT_NO_STANDING_DEATH) {
        if (m->health < 0x100) {
            update_mario_sound_and_camera(m);
            return drop_and_set_mario_action(m, ACT_STANDING_DEATH, 0);
        }
    }
    return FALSE;
}

s32 mario_execute_stationary_action(struct MarioState *m) {
    s32 cancel;

    if (check_common_stationary_cancels(m)) {
        return TRUE;
    }

    if (mario_update_quicksand(m, 0.5f)) {
        return TRUE;
    }

    /* clang-format off */
    switch (m->action) {
        case ACT_IDLE:                    cancel = act_idle(m);                             break;
        case ACT_DAB:
            cancel = mario_dab_action(m);
            break;
        case ACT_SPIN_DIZZY:
            cancel = mario_spin_dizzy_action(m);
            break;
        case ACT_START_SLEEPING:          cancel = act_start_sleeping(m);                   break;
        case ACT_SLEEPING:                cancel = act_sleeping(m);                         break;
        case ACT_WAKING_UP:               cancel = act_waking_up(m);                        break;
        case ACT_PANTING:                 cancel = act_panting(m);                          break;
        case ACT_HOLD_PANTING_UNUSED:     cancel = act_hold_panting_unused(m);              break;
        case ACT_HOLD_IDLE:               cancel = act_hold_idle(m);                        break;
        case ACT_HOLD_HEAVY_IDLE:         cancel = act_hold_heavy_idle(m);                  break;
        case ACT_IN_QUICKSAND:            cancel = act_in_quicksand(m);                     break;
        case ACT_STANDING_AGAINST_WALL:   cancel = act_standing_against_wall(m);            break;
        case ACT_COUGHING:                cancel = act_coughing(m);                         break;
        case ACT_SHIVERING:               cancel = act_shivering(m);                        break;
        case ACT_CROUCHING:               cancel = act_crouching(m);                        break;
        case ACT_START_CROUCHING:         cancel = act_start_crouching(m);                  break;
        case ACT_STOP_CROUCHING:          cancel = act_stop_crouching(m);                   break;
        case ACT_START_CRAWLING:          cancel = act_start_crawling(m);                   break;
        case ACT_STOP_CRAWLING:           cancel = act_stop_crawling(m);                    break;
        case ACT_SLIDE_KICK_SLIDE_STOP:   cancel = act_slide_kick_slide_stop(m);            break;
        case ACT_SHOCKWAVE_BOUNCE:        cancel = act_shockwave_bounce(m);                 break;
        case ACT_FIRST_PERSON:            cancel = act_first_person(m);                     break;
        case ACT_JUMP_LAND_STOP:          cancel = act_jump_land_stop(m);                   break;
        case ACT_DOUBLE_JUMP_LAND_STOP:   cancel = act_double_jump_land_stop(m);            break;
        case ACT_FREEFALL_LAND_STOP:      cancel = act_freefall_land_stop(m);               break;
        case ACT_SIDE_FLIP_LAND_STOP:     cancel = act_side_flip_land_stop(m);              break;
        case ACT_HOLD_JUMP_LAND_STOP:     cancel = act_hold_jump_land_stop(m);              break;
        case ACT_HOLD_FREEFALL_LAND_STOP: cancel = act_hold_freefall_land_stop(m);          break;
        case ACT_AIR_THROW_LAND:          cancel = act_air_throw_land(m);                   break;
        case ACT_LAVA_BOOST_LAND:         cancel = act_lava_boost_land(m);                  break;
        case ACT_TWIRL_LAND:              cancel = act_twirl_land(m);                       break;
        case ACT_TRIPLE_JUMP_LAND_STOP:   cancel = act_triple_jump_land_stop(m);            break;
        case ACT_BACKFLIP_LAND_STOP:      cancel = act_backflip_land_stop(m);               break;
        case ACT_LONG_JUMP_LAND_STOP:     cancel = act_long_jump_land_stop(m);              break;
        case ACT_GROUND_POUND_LAND:       cancel = act_ground_pound_land(m);                break;
        case ACT_BRAKING_STOP:            cancel = act_braking_stop(m);                     break;
        case ACT_BUTT_SLIDE_STOP:         cancel = act_butt_slide_stop(m);                  break;
        case ACT_HOLD_BUTT_SLIDE_STOP:    cancel = act_hold_butt_slide_stop(m);             break;
        default:                          cancel = TRUE;                                    break;
    }
    /* clang-format on */

    if (!cancel && (m->input & INPUT_IN_WATER)) {
        m->particleFlags |= PARTICLE_IDLE_WATER_WAVE;
    }

    return cancel;
}
