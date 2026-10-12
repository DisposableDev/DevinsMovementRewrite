#include "game_options_runtime.h"
#include <PR/ultratypes.h>

#include "sm64.h"
#include "area.h"
#include "level_table.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "interaction.h"
#include "game_options.h"
#include "mario.h"
#include "mario_spin_dizzy.h"
#include "mario_step.h"
#include "memory.h"
#include "surface_terrains.h"

// Turn Mario through the configured number of rotations, then release to fall backwards.
// These durations count game updates (normally 30 per second).
#define SPIN_DIZZY_TURNS 3
#define SPIN_DIZZY_WINDOW 150
#define SPIN_DIZZY_RELEASE_WINDOW 30
#define SPIN_DIZZY_MIN_STICK 50.0f

// A gesture belongs to uninterrupted ordinary locomotion; every rejected update resets it.
static struct {
    s32 rotation;
    u16 frames;
    u16 releaseFrames;
    s16 lastYaw;
    u8 tracking;
} sSpinDizzy;

void mario_spin_dizzy_reset(void) {
    bzero(&sSpinDizzy, sizeof(sSpinDizzy));
}

// Action and support policy only; the caller owns permissions and arena exclusions.
static s32 mario_spin_dizzy_can_track(struct MarioState *m) {
    switch (m->action) {
        case ACT_IDLE:
        case ACT_WALKING:
        case ACT_TURNING_AROUND:
        case ACT_FINISH_TURNING_AROUND:
        case ACT_BRAKING:
        case ACT_BRAKING_STOP:
        case ACT_DECELERATING:
            break;
        case ACT_FREEFALL_LAND_STOP:
            if (m->prevAction != ACT_FORWARD_ROLLOUT && m->prevAction != ACT_BACKWARD_ROLLOUT) {
                return FALSE;
            }
            break;
        default:
            return FALSE;
    }
    return !(m->floor == NULL || m->heldObj != NULL || m->riddenObj != NULL || m->health < 0x300
             || m->hurtCounter != 0 || m->quicksandDepth > 0.0f
             || (m->floor->flags & SURFACE_FLAG_DYNAMIC) || SURFACE_IS_UNSAFE(m->floor->type)
             || mario_floor_is_slippery(m)
             || (m->input
                 & (INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE | INPUT_IN_POISON_GAS | INPUT_STOMPED
                    | INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_DOWN | INPUT_FIRST_PERSON)));
}

void mario_spin_dizzy_update(struct MarioState *m) {
    s16 yaw;
    s32 delta;
    f32 floorBehind;

    if (game_options_secrets_blocked() || !game_options_get(GO_DIZZINESS)
        || gCurrLevelNum == LEVEL_BOWSER_1 || gCurrLevelNum == LEVEL_BOWSER_2
        || gCurrLevelNum == LEVEL_BOWSER_3) {
        mario_spin_dizzy_reset();
        return;
    }
    if (!mario_spin_dizzy_can_track(m)) {
        mario_spin_dizzy_reset();
        return;
    }

    if (sSpinDizzy.releaseFrames == 0) {
        yaw = m->faceAngle[1];
        if (!sSpinDizzy.tracking) {
            if (m->controller->stickMag >= SPIN_DIZZY_MIN_STICK) {
                sSpinDizzy.tracking = TRUE;
                sSpinDizzy.lastYaw = yaw;
            }
            return;
        }
        if (++sSpinDizzy.frames > SPIN_DIZZY_WINDOW) {
            mario_spin_dizzy_reset();
            return;
        }

        // This runs before movement. Count the previous update's turn before checking
        // today's stick release, so releasing on the final rotation can trigger the fall.
        delta = (s16) (yaw - sSpinDizzy.lastYaw);
        sSpinDizzy.lastYaw = yaw;
        // Reject abrupt facing flips, rather than counting them as spins.
        if (delta > DEGREES(90) || delta < -DEGREES(90)) {
            mario_spin_dizzy_reset();
            return;
        }
        // Signed accumulation makes back-and-forth movement cancel itself out.
        sSpinDizzy.rotation += delta;
        if (sSpinDizzy.rotation >= SPIN_DIZZY_TURNS * 0x10000
            || sSpinDizzy.rotation <= -SPIN_DIZZY_TURNS * 0x10000) {
            sSpinDizzy.releaseFrames = SPIN_DIZZY_RELEASE_WINDOW;
            // Keep the full release window when the stick is still held.
            if (m->input & INPUT_NONZERO_ANALOG) {
                return;
            }
        } else {
            if (m->controller->stickMag < SPIN_DIZZY_MIN_STICK) {
                mario_spin_dizzy_reset();
            }
            return;
        }
    }

    if (!(m->input & INPUT_NONZERO_ANALOG)) {
        // Mario needs level ground behind him for the fall reaction.
        floorBehind = find_floor_height_relative_polar(m, -0x8000, 60.0f);
        if (absf(m->pos[1] - floorBehind) <= 24.0f) {
            mario_set_forward_vel(m, 0.0f);
            set_mario_action(m, ACT_SPIN_DIZZY, 0);
            mario_blow_off_cap(m, 15.0f);
        }
        mario_spin_dizzy_reset();
    } else if (--sSpinDizzy.releaseFrames == 0) {
        mario_spin_dizzy_reset();
    }
}

// Play a grounded gesture once, then return control to the player.
static s32 act_grounded_gesture(struct MarioState *m, s32 animation) {
    if (m->input & INPUT_STOMPED) {
        return set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }
    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }
    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    stationary_ground_step(m);
    set_mario_animation(m, animation);
    if (is_anim_at_end(m)) {
        return set_mario_action(m, ACT_IDLE, 0);
    }
    return FALSE;
}

// Fall backwards after spinning; play the hurt voice once without dealing damage.
s32 mario_spin_dizzy_action(struct MarioState *m) {
    if (act_grounded_gesture(m, MARIO_ANIM_FALL_OVER_BACKWARDS)) {
        return TRUE;
    }

    play_sound_if_no_flag(m, SOUND_MARIO_ATTACKED, MARIO_MARIO_SOUND_PLAYED);
    return FALSE;
}
