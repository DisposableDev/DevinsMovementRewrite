#include <PR/ultratypes.h>

#include "sm64.h"
#include "mario.h"
#include "game_options.h"
#include "mario_dab.h"
#include "mario_actions_stationary.h"
#include "mario_actions_moving.h"
#include "mario_step.h"
#include "surface_terrains.h"
#include "engine/math_util.h"

// Keep these markers in sync with assets/anims/anim_D1.inc.c.
#define DAB_REPEAT_START_FRAME 2
#define DAB_POSE_FRAME 7
#define DAB_RECOVERY_FRAME 8
#define DAB_RECOVERY_END_FRAME 19
#define DAB_HOLD_START_FRAME 20
#define DAB_HOLD_FRAME_COUNT 32
#define DAB_HOLD_SETTLE_STEP 4
#define DAB_ANALOG_SETTLE_STEP (DAB_ANALOG_FRAME_COUNT / DAB_HOLD_SETTLE_STEP)
#define DAB_CAP_DAMAGE_FRAMES 5
#define DAB_SLEEP_WITH_CAP_COUNT 20
#define DAB_SLEEP_CAPLESS_COUNT 10
#define DAB_SLEEP_INPUT_LOCK_FRAMES 110

enum DabState {
    DAB_STATE_ENTER,
    DAB_STATE_HOLD,
    DAB_STATE_SETTLE,
    DAB_STATE_RECOVER,
    DAB_STATE_ANALOG,
    DAB_STATE_ANALOG_SETTLE,
};

// L must precede Z by exactly one update for the initial dab.
static struct {
    u8 shortcutFrames;
    u16 sessionFrames;
    u8 count;
    s32 sleepPending;
    u8 analogFrame;
} sDab;

// Releasing the chord restores ordinary controls; nonforward stick holds the pose.
static f32 dab_analog_stick_amount(struct MarioState *m) {
    if ((m->controller->buttonDown & (L_TRIG | Z_TRIG)) != (L_TRIG | Z_TRIG)) {
        return -1.0f;
    }
    if (!(m->input & INPUT_NONZERO_ANALOG)) {
        return 0.0f;
    }
    f32 forward = m->controller->stickMag * coss((s16) (m->intendedYaw - m->faceAngle[1]));
    return CLAMP(forward / 64.0f, 0.0f, 1.0f);
}

// Return TRUE while this mode owns the displayed frame. Its release settles
// back to the full dab before joining the existing breathing/recovery poses.
static s32 update_dab_analog_pose(struct MarioState *m, f32 amount) {
    if (m->actionState != DAB_STATE_HOLD && m->actionState != DAB_STATE_ANALOG
        && m->actionState != DAB_STATE_ANALOG_SETTLE) {
        return FALSE;
    }
    if (amount >= 0.0f && m->actionState != DAB_STATE_ANALOG_SETTLE) {
        m->actionState = DAB_STATE_ANALOG;
        sDab.analogFrame = (u8) (amount * (DAB_ANALOG_FRAME_COUNT - 1) + 0.5f);
        set_anim_to_frame(m, DAB_ANALOG_START_FRAME + sDab.analogFrame);
        return TRUE;
    }
    if (m->actionState == DAB_STATE_HOLD) {
        return FALSE;
    }
    m->actionState = DAB_STATE_ANALOG_SETTLE;
    if (sDab.analogFrame != 0) {
        sDab.analogFrame = MAX(0, sDab.analogFrame - DAB_ANALOG_SETTLE_STEP);
        set_anim_to_frame(m, DAB_ANALOG_START_FRAME + sDab.analogFrame);
    } else if (m->controller->buttonDown & L_TRIG) {
        m->actionState = DAB_STATE_HOLD;
        set_anim_to_frame(m, DAB_HOLD_START_FRAME);
    } else {
        m->actionState = DAB_STATE_RECOVER;
        set_anim_to_frame(m, DAB_RECOVERY_FRAME);
    }
    return TRUE;
}

static s32 dab_can_start(struct MarioState *m) {
    if (!game_options_get(GO_DAB)) {
        return FALSE;
    }
    switch (m->action) {
        case ACT_DAB:
        case ACT_IDLE:
        case ACT_PANTING:
        case ACT_WALKING:
        case ACT_TURNING_AROUND:
        case ACT_FINISH_TURNING_AROUND:
        case ACT_BRAKING:
        case ACT_BRAKING_STOP:
        case ACT_DECELERATING:
        case ACT_GROUND_TWIRL:
            break;
        case ACT_FREEFALL_LAND_STOP:
            if (m->prevAction != ACT_FORWARD_ROLLOUT && m->prevAction != ACT_BACKWARD_ROLLOUT) {
                return FALSE;
            }
            break;
        default:
            return FALSE;
    }
    return m->floor != NULL && m->heldObj == NULL && m->riddenObj == NULL
           && (m->health >= 0x300 || m->action == ACT_PANTING || m->action == ACT_DAB)
           && m->health >= 0x100 && m->hurtCounter == 0 && m->quicksandDepth == 0.0f
           && !SURFACE_IS_UNSAFE(m->floor->type) && !mario_floor_is_slippery(m)
           && !(m->input
                & (INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE | INPUT_IN_POISON_GAS | INPUT_STOMPED
                   | INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_FIRST_PERSON | INPUT_SQUISHED));
}

s32 mario_dab_can_start_from_neutral(struct MarioState *m) {
    return dab_can_start(m) && !(m->input & INPUT_NONZERO_ANALOG);
}

s32 mario_dab_loses_cap_on_damage(struct MarioState *m) {
    return game_options_get(GO_DAB) && m->action == ACT_DAB && (m->controller->buttonDown & L_TRIG)
           && sDab.sessionFrames >= DAB_CAP_DAMAGE_FRAMES;
}

static s32 start_dab(struct MarioState *m) {
    u32 neutralAction;
    // Movement input takes priority; do not interrupt walking just to cancel the dab.
    if (m->input & INPUT_NONZERO_ANALOG) {
        return FALSE;
    }
    if (m->action != ACT_DAB) {
        sDab.sessionFrames = 0;
        sDab.count = 0;
    }
    sDab.count++;
    sDab.analogFrame = 0;
    sDab.sleepPending = sDab.count >= ((m->flags & MARIO_CAP_ON_HEAD) ? DAB_SLEEP_WITH_CAP_COUNT
                                                                      : DAB_SLEEP_CAPLESS_COUNT);
    // Preserve the neutral-stick braking state when restarting a dab in any phase.
    if (m->action == ACT_DAB) {
        neutralAction = m->actionArg;
        set_mario_action(m, ACT_DAB, neutralAction);
        set_mario_animation(m, MARIO_ANIM_DAB);
        set_anim_to_frame(m, DAB_REPEAT_START_FRAME);
        play_sound_if_no_flag(m, SOUND_MARIO_PUNCH_HOO, MARIO_MARIO_SOUND_PLAYED);
        return TRUE;
    } else if (m->action == ACT_BRAKING || m->action == ACT_DECELERATING) {
        neutralAction = m->action;
    } else if (m->action == ACT_IDLE || m->action == ACT_PANTING || m->action == ACT_BRAKING_STOP) {
        neutralAction = ACT_IDLE;
    } else {
        // Same selection as begin_braking_action when the movement stick is released.
        neutralAction =
            (m->forwardVel >= 16.0f && m->floor->normal.y >= COS80) ? ACT_BRAKING : ACT_DECELERATING;
    }
    // The initial dab plays the full wind-up.
    m->marioObj->header.gfx.animInfo.animID = -1;
    set_mario_action(m, ACT_DAB, neutralAction);
    play_sound_if_no_flag(m, SOUND_MARIO_PUNCH_HOO, MARIO_MARIO_SOUND_PLAYED);
    return TRUE;
}

s32 mario_dab_update_shortcut(struct MarioState *m) {
    if (!game_options_get(GO_DAB)) {
        mario_dab_reset();
        return FALSE;
    }
    u16 pressed = m->controller->buttonPressed;
    u16 down = m->controller->buttonDown;
    if (m->action != ACT_DAB || !(down & L_TRIG)) {
        sDab.sessionFrames = 0;
        sDab.count = 0;
        sDab.sleepPending = FALSE;
    }
    if (!dab_can_start(m)) {
        sDab.shortcutFrames = 0;
        return FALSE;
    }
    if (m->action == ACT_DAB) {
        sDab.shortcutFrames = 0;
        if ((down & L_TRIG) && !(pressed & L_TRIG) && (pressed & Z_TRIG) && !sDab.sleepPending) {
            return start_dab(m);
        }
        return FALSE;
    }
    if (pressed & L_TRIG) {
        sDab.shortcutFrames = (down & Z_TRIG) ? 0 : 1;
        return FALSE;
    }
    if (!(down & L_TRIG)) {
        sDab.shortcutFrames = 0;
        return FALSE;
    }
    if (sDab.shortcutFrames != 0) {
        if (pressed & Z_TRIG) {
            sDab.shortcutFrames = 0;
            return start_dab(m);
        } else {
            sDab.shortcutFrames--;
        }
    }
    return FALSE;
}

s32 mario_dab_action(struct MarioState *m) {
    u16 savedInput = m->input;
    s32 cancel;
    s32 canContinue;
    f32 analogAmount = dab_analog_stick_amount(m);

    // Held L+Z owns voluntary controls, while forced exits still run before physics.
    // Z belongs to the dab only while L is held; otherwise it can crouch normally.
    if (m->controller->buttonDown & L_TRIG) {
        m->input &= ~(INPUT_Z_DOWN | INPUT_Z_PRESSED);
    }
    if (analogAmount >= 0.0f) {
        m->input &= ~(INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_FIRST_PERSON);
    }
    cancel = check_common_idle_cancels(m);
    canContinue = !cancel && dab_can_start(m);
    // Restore the caller's input even when cancellation changed the action.
    m->input = savedInput;
    if (cancel) {
        return TRUE;
    }
    if (!canContinue) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    if (m->controller->buttonDown & L_TRIG) {
        if (sDab.sessionFrames < DAB_CAP_DAMAGE_FRAMES) {
            sDab.sessionFrames++;
        }
    }

    // Use the actual neutral-stick speed routines, including slope, wind and sand.
    if (m->actionArg == ACT_BRAKING) {
        if (apply_slope_decel(m, 2.0f)) {
            m->actionArg = ACT_IDLE;
        }
    } else if (m->actionArg == ACT_DECELERATING) {
        if (update_decelerating_speed(m)) {
            m->actionArg = ACT_IDLE;
        }
    }

    switch (m->actionArg == ACT_IDLE ? stationary_ground_step(m) : perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            return set_mario_action(m, ACT_FREEFALL, 0);
        case GROUND_STEP_HIT_WALL:
            if (m->actionArg == ACT_BRAKING) {
                slide_bonk(m, ACT_BACKWARD_GROUND_KB, ACT_BRAKING_STOP);
                return TRUE;
            }
            mario_set_forward_vel(m, 0.0f);
            break;
    }
    set_mario_animation(m, MARIO_ANIM_DAB);
    if (update_dab_analog_pose(m, analogAmount)) {
        return FALSE;
    }
    switch (m->actionState) {
        case DAB_STATE_ENTER: {
            s16 frame = m->marioObj->header.gfx.animInfo.animFrame;

            // Finish the short settle even on early release, avoiding a pose jump.
            if (frame < DAB_POSE_FRAME) {
                break;
            }
            if (sDab.sleepPending) {
                sDab.sleepPending = FALSE;
                return set_mario_action(m, ACT_START_SLEEPING, DAB_SLEEP_INPUT_LOCK_FRAMES);
            }
            m->actionState = DAB_STATE_HOLD;
            if (update_dab_analog_pose(m, analogAmount)) {
                return FALSE;
            }
        }
        // fall through
        case DAB_STATE_HOLD:
            if (m->controller->buttonDown & L_TRIG) {
                s16 frame = m->marioObj->header.gfx.animInfo.animFrame;

                if (frame < DAB_HOLD_START_FRAME
                    || frame >= DAB_HOLD_START_FRAME + DAB_HOLD_FRAME_COUNT - 1) {
                    set_anim_to_frame(m, DAB_HOLD_START_FRAME);
                }
                break;
            }
            m->actionState = DAB_STATE_SETTLE;
            // fall through
        case DAB_STATE_SETTLE: {
            s16 phase = m->marioObj->header.gfx.animInfo.animFrame - DAB_HOLD_START_FRAME;

            // The breathing loop is symmetric: either half can settle along its
            // matching first-half pose, reaching neutral in at most four frames.
            if (phase > DAB_HOLD_FRAME_COUNT / 2) {
                phase = DAB_HOLD_FRAME_COUNT - phase;
            }
            if (phase > 0) {
                phase -= DAB_HOLD_SETTLE_STEP;
                if (phase < 0) {
                    phase = 0;
                }
                set_anim_to_frame(m, DAB_HOLD_START_FRAME + phase);
                break;
            }
            m->actionState = DAB_STATE_RECOVER;
            set_anim_to_frame(m, DAB_RECOVERY_FRAME);
            break;
        }
        case DAB_STATE_RECOVER:
            if (m->marioObj->header.gfx.animInfo.animFrame >= DAB_RECOVERY_END_FRAME) {
                return set_mario_action(m, m->actionArg, 0);
            }
            break;
    }
    return FALSE;
}

void mario_dab_reset(void) {
    sDab.shortcutFrames = 0;
    sDab.sessionFrames = 0;
    sDab.count = 0;
    sDab.sleepPending = FALSE;
    sDab.analogFrame = 0;
}
