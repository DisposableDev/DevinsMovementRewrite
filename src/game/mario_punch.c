#include "player_stats.h"
#include <PR/ultratypes.h>

#include "sm64.h"
#include "mario_punch.h"
#include "types.h"
#include "mario_step.h"
#include "mario.h"
#include "mario_actions_stationary.h"
#include "mario_actions_moving.h"
#include "audio/external.h"
#include "behavior_data.h"
#include "interaction.h"
#include "engine/math_util.h"
#include "object_helpers.h"
#include "rumble_init.h"
#include "sound_init.h"

/**
 * Forward-velocity pattern used by the punch lunge during each animation frame.
 */
s8 sPunchingForwardVelocities[8] = { 0, 1, 1, 2, 3, 5, 7, 10 };

#define PUNCH_RECOVERY_CANCEL_EARLY_FRAMES 5
#define GROUND_KICK_CANCEL_EARLY_FRAMES 10
#define AUTO_PUNCH_RECOVERY_EARLY_FRAMES (PUNCH_RECOVERY_CANCEL_EARLY_FRAMES + 1)
#define AUTO_KICK_RECOVERY_EARLY_FRAMES (GROUND_KICK_CANCEL_EARLY_FRAMES + 1)
#define HELD_B_PUNCH_FOLLOWUP_FRAME 2
#define GROUND_KICK_RECOVERY_START_FRAME 8
#define FIRST_PUNCH_LUNGE_SCALE 1.5f
#define SECOND_PUNCH_LUNGE_SCALE 2.25f
#define GROUND_KICK_LUNGE_SCALE 3.0f
#define PUNCH_MAX_TURN 0x80
#define BREAKDANCE_GROUND_DRAG 0.10f
#define BREAKDANCE_SLIDE_MAX_TURN 0x100

enum PunchMode {
    PUNCH_MODE_COMBO,
    PUNCH_MODE_REPEAT_SINGLE,
    PUNCH_MODE_REPEAT_PAIR,
};

// The mode belongs to the whole sequence, not to an individual animation.
static struct {
    enum PunchMode mode;
    // Release/re-press latch, cleared at sequence reset and each follow-up.
    s32 bReleased;
} sPunchSequence;

void mario_reset_punch_sequence(void) {
    sPunchSequence.mode = PUNCH_MODE_COMBO;
    sPunchSequence.bReleased = FALSE;
}

s32 mario_is_first_punch(struct MarioState *m) {
    if (m->action != ACT_PUNCHING && m->action != ACT_MOVE_PUNCHING) {
        return FALSE;
    }
    switch (m->actionArg) {
        case ACT_ARG_PUNCH_SEQUENCE_YAH:
        case ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH:
        case ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST:
            return TRUE;
        default:
            return FALSE;
    }
}

static void update_punch_input(struct MarioState *m) {
    if (!(m->input & INPUT_B_DOWN)) {
        sPunchSequence.bReleased = TRUE;
        sPunchSequence.mode = PUNCH_MODE_COMBO;
    } else if (sPunchSequence.bReleased && (m->input & INPUT_B_PRESSED)) {
        sPunchSequence.mode =
            mario_is_first_punch(m) ? PUNCH_MODE_REPEAT_SINGLE : PUNCH_MODE_REPEAT_PAIR;
        sPunchSequence.bReleased = FALSE;
    }
}

static void start_punch_followup(struct MarioState *m, u32 actionArg) {
    m->actionArg = actionArg;
    sPunchSequence.bReleased = FALSE;
    // A repeated first punch must be able to hit the same object/wall again.
    if (actionArg == ACT_ARG_PUNCH_SEQUENCE_YAH) {
        mario_set_punch_hit_phase(0);
        // A chain begun while moving can lunge once its earlier hits slow Mario.
        if (m->actionState == ACT_STATE_PUNCH_LUNGE_INACTIVE) {
            m->actionState = ACT_STATE_PUNCH_LUNGE_UNDECIDED;
        }
    } else if (actionArg == ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK) {
        player_stats_punch(actionArg);
        mario_set_punch_hit_phase(3);
        // Follow-ups stay in the same Mario action, so allow this kick's sound.
        m->flags &= ~MARIO_ACTION_SOUND_PLAYED;
    }
    if (m->actionState == ACT_STATE_PUNCH_LUNGE_ACTIVE && actionArg != ACT_ARG_PUNCH_SEQUENCE_WAH
        && actionArg != ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK) {
        m->actionTimer = 7;
    }
}

void apply_punch_lunge(struct MarioState *m) {
    // The sequence updater can finish or cancel the action before this call.
    if ((m->action != ACT_PUNCHING && m->action != ACT_MOVE_PUNCHING)
        || m->actionArg == ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE) {
        return;
    }
    if (!(m->flags & MARIO_PUNCHING)
        && (mario_is_first_punch(m) || m->actionArg == ACT_ARG_PUNCH_SEQUENCE_WAH
            || m->actionArg == ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH)) {
        return;
    }

    if (m->actionState == ACT_STATE_PUNCH_LUNGE_UNDECIDED) {
        // Keep the starting-speed decision for the entire punch sequence.
        m->actionState =
            absf(m->forwardVel) < 10.0f ? ACT_STATE_PUNCH_LUNGE_ACTIVE : ACT_STATE_PUNCH_LUNGE_INACTIVE;
        if (m->actionState == ACT_STATE_PUNCH_LUNGE_ACTIVE) {
            m->actionTimer = 7;
        }
    }

    if (m->actionState == ACT_STATE_PUNCH_LUNGE_ACTIVE && m->actionTimer > 0) {
        f32 lungeScale = FIRST_PUNCH_LUNGE_SCALE;
        if (m->actionArg == ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH
            || m->actionArg == ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH_FAST) {
            lungeScale = SECOND_PUNCH_LUNGE_SCALE;
        } else if (m->actionArg == ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK) {
            lungeScale = GROUND_KICK_LUNGE_SCALE;
        }
        mario_set_forward_vel(m, lungeScale * sPunchingForwardVelocities[m->actionTimer]);
        m->actionTimer--;
    }
}

static void release_breakdance_power_up_cap(struct MarioState *m) {
    u32 caps = mario_natural_cap_powers(m);
    ModelID32 model = MODEL_MARIOS_CAP;
    struct Object *cap;

    if (caps == 0 || absf(m->forwardVel) >= 20.0f || m->controller->stickMag != 0.0f
        || (m->controller->buttonDown & (L_TRIG | Z_TRIG)) != (L_TRIG | Z_TRIG)) {
        return;
    }

    if (caps & MARIO_METAL_CAP) {
        model = (caps & MARIO_WING_CAP) ? MODEL_MARIOS_WINGED_METAL_CAP : MODEL_MARIOS_METAL_CAP;
    } else if (caps & MARIO_WING_CAP) {
        model = MODEL_MARIOS_WING_CAP;
    }
    cap = try_to_spawn_object(0, 1.0f, m->marioObj, model, bhvReleasedPowerUpCap);
    if (cap != NULL) {
        cap->oPosX = m->pos[0];
        cap->oPosY = m->pos[1] + 160.0f;
        cap->oPosZ = m->pos[2];
        cap->oFaceAngleRoll = 0;
        cap->oFaceAnglePitch = 0;
        cap->oFaceAngleYaw = m->faceAngle[1];
        cap->oVelY = 16.0f;
        cap->oBehParams = caps;
        cap->oOpacity = (caps & MARIO_VANISH_CAP) ? 150 : 255;
    }

    player_stats_cap_powers_removed(caps, TRUE);
    mario_clear_cap_flags(m);
    if (!(m->flags & MARIO_CAPS)) {
        m->flags &= ~MARIO_CAP_ON_HEAD;
    }
    m->capTimer = 0;
    stop_cap_music();
}

static s32 punch_recovery_is_open(struct MarioState *m, s32 earlyFrames) {
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    s32 firstRecoveryFrame = m->actionArg == ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK
                                 ? GROUND_KICK_RECOVERY_START_FRAME
                                 : HELD_B_PUNCH_FOLLOWUP_FRAME;
    return animInfo->animFrame >= firstRecoveryFrame
           && animInfo->animFrame >= animInfo->curAnim->loopEnd - 1 - earlyFrames;
}

static s32 cancel_punch_recovery(struct MarioState *m, s32 earlyFrames) {
    if (!punch_recovery_is_open(m, earlyFrames)) {
        return FALSE;
    }
    // Keep Mario facing forward long enough to use the normal side-flip turn.
    if ((m->input & INPUT_NONZERO_ANALOG) && abs_angle_diff(m->intendedYaw, m->faceAngle[1]) > 0x471C
        && m->floor->normal.y >= COS73
        && !(m->input & (INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE | INPUT_FIRST_PERSON))) {
        if (m->input & INPUT_A_PRESSED) {
            return set_jumping_action(m, ACT_JUMP, 0);
        }
        return set_mario_action(m, ACT_TURNING_AROUND, 0);
    }
    if (m->input
        & (INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_DOWN | INPUT_NONZERO_ANALOG | INPUT_OFF_FLOOR
           | INPUT_ABOVE_SLIDE | INPUT_FIRST_PERSON)) {
        s32 keepFacing =
            m->actionArg != ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE || m->action == ACT_MOVE_PUNCHING;
        s16 facingYaw = m->faceAngle[1];
        s32 cancelled = check_common_idle_cancels(m);
        if (cancelled && keepFacing && m->action == ACT_WALKING) {
            // Preserve facing through both the idle cancel and low-speed walking.
            m->faceAngle[1] = facingYaw;
            m->actionArg = ACT_ARG_WALKING_TURN_FROM_PUNCH;
        }
        return cancelled;
    }
    return FALSE;
}

static void finish_punch_sequence(struct MarioState *m, u32 endAction) {
    u32 endArg = endAction == ACT_WALKING ? ACT_ARG_WALKING_TURN_FROM_PUNCH : 0;
    set_mario_action(m, endAction, endArg);
}

static s32 update_punch_recovery(struct MarioState *m, u32 endAction) {
    u32 recoveryArg = m->actionArg;
    s32 firstPunch = mario_is_first_punch(m);
    s32 repeatBoundary = firstPunch ? sPunchSequence.mode == PUNCH_MODE_REPEAT_SINGLE
                                    : sPunchSequence.mode == PUNCH_MODE_REPEAT_PAIR;

    if ((m->input & INPUT_B_DOWN) && !(m->input & INPUT_Z_PRESSED) && repeatBoundary) {
        // Finish the entire recovery before repeating a single punch or a pair.
        // Stick input alone must not cancel a held repeat during its extra wait.
        if (is_anim_at_end(m)) {
            start_punch_followup(m, ACT_ARG_PUNCH_SEQUENCE_YAH);
        } else if ((m->input
                    & (INPUT_A_PRESSED | INPUT_Z_DOWN | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE
                       | INPUT_FIRST_PERSON))
                   && cancel_punch_recovery(m, PUNCH_RECOVERY_CANCEL_EARLY_FRAMES)) {
            return TRUE;
        }
    } else if ((m->input & INPUT_B_DOWN) && !(m->input & INPUT_Z_PRESSED)
               && punch_recovery_is_open(m, AUTO_PUNCH_RECOVERY_EARLY_FRAMES)) {
        start_punch_followup(m, firstPunch ? ACT_ARG_PUNCH_SEQUENCE_WAH
                                           : ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK);
    } else if (cancel_punch_recovery(m, PUNCH_RECOVERY_CANCEL_EARLY_FRAMES)) {
        return TRUE;
    }

    if (m->actionArg == recoveryArg && is_anim_at_end(m)) {
        finish_punch_sequence(m, endAction);
    }
    return FALSE;
}

static s32 update_breakdance(struct MarioState *m, u32 crouchEndAction) {
    // Re-entering a spin during the crawl cancel can happen in the same
    // frame. Restart its animation so recovery cannot immediately
    // return to crawl again, and give the new spin fresh hit records.
    if (m->actionTimer == 0) {
        player_stats_punch(ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        release_breakdance_power_up_cap(m);
        mario_set_punch_hit_phase(0);
        // Shedding Vanish ends land movement underwater. Redispatch
        // into swimming before running the spin's ground movement.
        if (m->pos[1] < m->waterLevel - 100 && !mario_has_land_movement_underwater(m)) {
            return set_water_plunge_action(m);
        }
        m->marioObj->header.gfx.animInfo.animID = -1;
        m->actionTimer = 1;
    }
    mario_set_punch_hit_phase(4);
    play_mario_action_sound(m, SOUND_MARIO_PUNCH_HOO, 1);
    set_mario_animation(m, MARIO_ANIM_BREAKDANCE);
    s32 animFrame = m->marioObj->header.gfx.animInfo.animFrame;
    s32 cameFromCrawl = m->prevAction == ACT_CRAWLING || m->prevAction == ACT_START_CRAWLING
                        || m->prevAction == ACT_STOP_CRAWLING;
    s32 walkWithHeldZ =
        (m->input & INPUT_Z_DOWN) && m->controller->stickMag >= MARIO_STRONG_STICK_THRESHOLD;

    if (animFrame >= 2 && animFrame < 12) {
        m->flags |= MARIO_TRIPPING;
    }

    // Returning from a crawl uses the usual recovery window but skips
    // the crouch action that Z would otherwise select.
    if (cameFromCrawl && (m->input & INPUT_Z_DOWN)
        && punch_recovery_is_open(m, GROUND_KICK_CANCEL_EARLY_FRAMES)) {
        return set_mario_action(m, ACT_CRAWLING, 0);
    }

    if (animFrame >= 12 && cancel_punch_recovery(m, GROUND_KICK_CANCEL_EARLY_FRAMES)) {
        return TRUE;
    }

    if (is_anim_at_end(m)) {
        u32 endAction;
        if (cameFromCrawl && (m->input & INPUT_Z_DOWN)) {
            endAction = ACT_CRAWLING;
        } else if (walkWithHeldZ) {
            endAction = ACT_WALKING;
        } else {
            endAction = crouchEndAction;
        }
        set_mario_action(m, endAction, 0);
    }
    return FALSE;
}

s32 mario_update_punch_sequence(struct MarioState *m) {
    // actionArg selects the animation phase; actionState caches lunge eligibility.
    // actionTimer counts down lunge samples, or marks breakdance's startup.
    u32 endAction, crouchEndAction;
    s32 animFrame;

    if (m->action & ACT_FLAG_MOVING) {
        endAction = ACT_WALKING, crouchEndAction = ACT_CROUCH_SLIDE;
    } else {
        endAction = ACT_IDLE, crouchEndAction = ACT_CROUCHING;
    }

    switch (m->actionArg) {
        case ACT_ARG_PUNCH_SEQUENCE_YAH:
            player_stats_punch(m->actionArg);
            mario_set_punch_hit_phase(0);
            play_sound(SOUND_MARIO_PUNCH_YAH, m->marioObj->header.gfx.cameraToObject);
            FALL_THROUGH;
        case ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH:
            update_punch_input(m);
            mario_set_punch_hit_phase(1);
            set_mario_animation(m, MARIO_ANIM_FIRST_PUNCH);
            if (is_anim_past_end(m)) {
                m->actionArg = ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST;
            } else {
                m->actionArg = ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH;
            }

            if (m->marioObj->header.gfx.animInfo.animFrame >= 2) {
                if (mario_check_object_grab(m)) {
                    return TRUE;
                }

                m->flags |= MARIO_PUNCHING;
            }

            if (m->actionArg == ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST) {
                m->marioBodyState->punchState = (PUNCH_STATE_TYPE_FIRST_PUNCH | 0x4);
            }
            break;

        case ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST:
            mario_set_punch_hit_phase(1);
            set_mario_animation(m, MARIO_ANIM_FIRST_PUNCH_FAST);
            update_punch_input(m);

            if (mario_check_object_grab(m)) {
                return TRUE;
            }

            if (m->marioObj->header.gfx.animInfo.animFrame <= 0) {
                m->flags |= MARIO_PUNCHING;
            }

            if (update_punch_recovery(m, endAction)) {
                return TRUE;
            }
            break;

        case ACT_ARG_PUNCH_SEQUENCE_WAH:
            player_stats_punch(m->actionArg);
            play_sound(SOUND_MARIO_PUNCH_WAH, m->marioObj->header.gfx.cameraToObject);
            // fallthrough
        case ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH:
            update_punch_input(m);
            mario_set_punch_hit_phase(2);
            animFrame = set_mario_animation(m, MARIO_ANIM_SECOND_PUNCH);
            if (is_anim_past_end(m)) {
                m->actionArg = ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH_FAST;
            } else {
                m->actionArg = ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH;
            }

            if (animFrame > 0) {
                m->flags |= MARIO_PUNCHING;
            }

            if (animFrame == 1 && m->actionState == ACT_STATE_PUNCH_LUNGE_ACTIVE) {
                // Begin the lunge with the second hit, not in the first hit's recovery.
                m->actionTimer = 7;
            }

            if (m->actionArg == ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH_FAST) {
                m->marioBodyState->punchState = (PUNCH_STATE_TYPE_SECOND_PUNCH | 0x4);
            }
            break;

        case ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH_FAST:
            mario_set_punch_hit_phase(2);
            set_mario_animation(m, MARIO_ANIM_SECOND_PUNCH_FAST);
            update_punch_input(m);
            if (m->marioObj->header.gfx.animInfo.animFrame <= 0) {
                m->flags |= MARIO_PUNCHING;
            }

            if (update_punch_recovery(m, endAction)) {
                return TRUE;
            }
            break;

        case ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK:
            mario_set_punch_hit_phase(3);
            play_mario_action_sound(m, SOUND_MARIO_PUNCH_HOO, 1);
            animFrame = set_mario_animation(m, MARIO_ANIM_GROUND_KICK);
            if (animFrame == 0 && m->actionState == ACT_STATE_PUNCH_LUNGE_ACTIVE) {
                m->actionTimer = 7;
            }
            if (animFrame == 0) {
                m->marioBodyState->punchState = (PUNCH_STATE_TYPE_KICK | 0x6);
            }

            if (animFrame >= 0 && animFrame < GROUND_KICK_RECOVERY_START_FRAME) {
                m->flags |= MARIO_KICKING;
            }

            if (cancel_punch_recovery(m, AUTO_KICK_RECOVERY_EARLY_FRAMES)) {
                return TRUE;
            }

            if (is_anim_at_end(m)) {
                if ((m->input & INPUT_B_DOWN) && !(m->input & INPUT_Z_PRESSED)) {
                    start_punch_followup(m, ACT_ARG_PUNCH_SEQUENCE_YAH);
                } else {
                    finish_punch_sequence(m, endAction);
                }
            }
            break;

        case ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE:
            return update_breakdance(m, crouchEndAction);
    }

    return FALSE;
}

static void steer_ground_attack(struct MarioState *m) {
    if ((m->action != ACT_PUNCHING && m->action != ACT_MOVE_PUNCHING)
        || !(m->input & INPUT_NONZERO_ANALOG)) {
        return;
    }

    s16 targetYaw = m->intendedYaw;
    s16 maxTurn = PUNCH_MAX_TURN;
    if (m->actionArg == ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE) {
        if (m->action != ACT_MOVE_PUNCHING || absf(m->forwardVel) < 1.0f) {
            return;
        }
        maxTurn = BREAKDANCE_SLIDE_MAX_TURN;
        targetYaw += m->forwardVel < 0.0f ? 0x8000 : 0;
    }

    s16 turn = (s16) (maxTurn * CLAMP(m->intendedMag, 0.0f, 32.0f) / 32.0f);
    m->faceAngle[1] = approach_s16(m->faceAngle[1], targetYaw, turn, turn);
}

void apply_punch_ground_drag(struct MarioState *m) {
    // Punches settle fastest; the kick retains a little more travel, while
    // breakdancing carries the speed Mario already had.
    f32 groundDrag;
    if (m->actionArg == ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE) {
        groundDrag = BREAKDANCE_GROUND_DRAG;
    } else if (m->actionArg == ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK) {
        groundDrag = 0.6f;
    } else {
        groundDrag = 0.7f;
    }

    steer_ground_attack(m);

    if (m->forwardVel >= 0.0f) {
        apply_slope_decel(m, groundDrag);
    } else {
        if ((m->forwardVel += 16.0f * groundDrag) >= 0.0f) {
            m->forwardVel = 0.0f;
        }
        apply_slope_accel(m);
    }
}
