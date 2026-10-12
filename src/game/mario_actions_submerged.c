#include <PR/ultratypes.h>

#include "sm64.h"
#include "mario_actions_submerged.h"
#include "mario_actions_airborne.h"
#include "mario_actions_moving.h"
#include "mario_actions_stationary.h"
#include "level_update.h"
#include "game_init.h"
#include "game_options.h"
#include "game_options_runtime.h"
#include "memory.h"
#include "engine/math_util.h"
#include "engine/graph_node.h"
#include "area.h"
#include "save_file.h"
#include "sound_init.h"
#include "engine/surface_collision.h"
#include "interaction.h"
#include "mario.h"
#include "player_stats.h"
#include "mario_character.h"
#include "mario_he_thicc.h"
#include "mario_step.h"
#include "mario_water_collision.h"
#include "camera.h"
#include "audio/external.h"
#include "behavior_data.h"
#include "level_table.h"
#include "rumble_init.h"

// Stroke strength is stored in tenths of a speed unit, as in vanilla swimming.
#define MIN_SWIM_STRENGTH ((s16) (MIN_SWIM_SPEED * 10.0f))
#define MAX_SWIM_STRENGTH ((s16) (SWIM_SOFT_SPEED_LIMIT * 10.0f))
#define BREASTSTROKE_DURATION 14
#define BREASTSTROKE_RECOVERY_MIN 7
#define HE_THICC_WATER_SINK_SPEED 3.0f
#define SWIMMING_ROLL_FRAMES 16
#define SWIMMING_ROLL_BOUNCE_BOOST 8.0f
#define SWIMMING_ROLL_EARLY_BOOST 9.0f
#define SWIMMING_ROLL_EARLY_FRAMES 3
#define SWIMMING_ROLL_SPEED_MULTIPLIER 1.3f
#define SWIMMING_ROLL_SPEED_CAP 64.0f
#define SWIMMING_ROLL_BOUNCE_SPEED_LOSS 0.06f
#define SWIMMING_ROLL_ZERO_SPEED_COUNT 18

// Persistent swimming controls and particle timing live here.
// Individual actions keep their phase and duration in actionState/actionTimer.
static struct {
    u32 swimButton; // Last pressed swim button; uses INPUT_A_DOWN / INPUT_B_DOWN.
    u16 aTapFrames;
    u8 aTapPending;
    u8 aTapHolding;
    u8 strokeQueued;
    u8 strokeBuffered;
    u8 waterDownRepeatFrames;
    u8 entryTapFrames;
    u8 entryTapPending;
    u8 entryStrokeReady;
    u16 strokeCooldown; // Earliest next stroke, independent of A/B action changes.
    u16 bTapFrames;
    u16 punchCooldown;
    u8 bTapPending;
    u8 punchQueued;
    u8 burstRecovering;
    u16 waterDownImpactCooldown; // Survives action changes and early slope contacts.
    f32 surfaceExitVelocityY;
    u8 surfaceSplashPlayed; // Reused only by an automatic jump during the same ascent.
    f32 waterDownBubbleDistance;
    u8 rollFrame;
    u8 rollCooldown;
    s32 rollStartAngle;
    Vec3f rollFloorNormal;
    u8 rollFloorContact;
    s16 rollWallYaw;
    u8 rollWallContact;
    u8 rollBounceCount;
} sSwimming;

static s16 sWasAtSurface = FALSE;
static s16 sSwimStrength = MIN_SWIM_STRENGTH;
static s16 sWaterCurrentSpeeds[] = { 28, 12, 8, 4 };

static s16 sBobTimer;
static s16 sBobIncrement;
static f32 sBobHeight;

void mario_swimming_reset(void) {
    bzero(&sSwimming, sizeof(sSwimming));
    sSwimStrength = MIN_SWIM_STRENGTH;
    sWasAtSurface = FALSE;
    sBobTimer = 0;
    sBobIncrement = 0;
    sBobHeight = 0.0f;
}

s32 mario_swimming_roll_is_active(void) {
    return game_options_get(GO_TWIRLS) && sSwimming.rollFrame != 0;
}

static s32 is_normal_swimming_action(struct MarioState *m) {
    switch (m->action) {
        case ACT_WATER_IDLE:
        case ACT_WATER_ACTION_END:
        case ACT_BREASTSTROKE:
        case ACT_SWIMMING_END:
        case ACT_FLUTTER_KICK:
            return TRUE;
    }
    return FALSE;
}

static void cancel_swimming_roll(struct MarioState *m) {
    if (sSwimming.rollFrame != 0) {
        sSwimming.rollFrame = 0;
        sSwimming.rollCooldown = MARIO_TWIRL_COOLDOWN_FRAMES;
        m->marioObj->header.gfx.angle[2] = m->faceAngle[2];
    }
}

static void update_swimming_roll(struct MarioState *m) {
    if (!game_options_get(GO_TWIRLS)) {
        cancel_swimming_roll(m);
        sSwimming.rollCooldown = 0;
        sSwimming.rollWallContact = FALSE;
        sSwimming.rollFloorContact = FALSE;
        sSwimming.rollBounceCount = 0;
        return;
    }
    if (sSwimming.rollCooldown > 0) {
        sSwimming.rollCooldown--;
    }
    if (!is_normal_swimming_action(m) || m->floor == NULL || m->heldObj != NULL
        || (m->flags & MARIO_METAL_CAP) || m->health < 0x100) {
        cancel_swimming_roll(m);
        return;
    }
    if (sSwimming.rollFrame != 0) {
        if (++sSwimming.rollFrame > SWIMMING_ROLL_FRAMES) {
            cancel_swimming_roll(m);
        }
        return;
    }
    if (sSwimming.rollCooldown == 0 && (m->controller->buttonPressed & L_TRIG)
        && !(m->input & INPUT_Z_PRESSED)) {
        sSwimming.rollFrame = 1;
        sSwimming.rollStartAngle = 0;
        sSwimming.rollWallContact = FALSE;
        sSwimming.rollFloorContact = FALSE;
        sSwimming.rollBounceCount = 0;
        mario_play_air_twirl_sound(m);
    }
}

// Swimming animations place the hips above the object origin. Keep that
// animated point fixed while banking, instead of orbiting it around the origin.
static void center_swimming_roll(struct MarioState *m, s16 bank) {
    struct AnimInfo *info = &m->marioObj->header.gfx.animInfo;
    struct Animation *animation = info->curAnim;
    if (animation == NULL || (animation->flags & (ANIM_FLAG_NO_TRANS | ANIM_FLAG_VERT_TRANS))) {
        return;
    }
    u16 *attribute = segmented_to_virtual((void *) animation->index);
    s16 *values = segmented_to_virtual((void *) animation->values);
    attribute += 2; // Root Y translation; swimming root X/Z translations are zero.
    s16 frame = geo_update_animation_frame(info, NULL);
    f32 height = values[retrieve_animation_index(frame, &attribute)] * 0.25f;
    if (animation->animYTransDivisor != 0) {
        height *= (f32) info->animYTrans / animation->animYTransDivisor;
    }
    height *= m->marioObj->header.gfx.scale[1];
    s16 *angle = m->marioObj->header.gfx.angle;
    f32 sideways = height * (sins(angle[2]) - sins(bank));
    f32 upward = height * (coss(angle[2]) - coss(bank));
    f32 pitchSin = sins(angle[0]);
    f32 yawSin = sins(angle[1]);
    f32 yawCos = coss(angle[1]);
    m->marioObj->header.gfx.pos[0] += -yawCos * sideways + yawSin * pitchSin * upward;
    m->marioObj->header.gfx.pos[1] += coss(angle[0]) * upward;
    m->marioObj->header.gfx.pos[2] += yawSin * sideways + yawCos * pitchSin * upward;
}

// Apply after swimming has produced its ordinary pitch and bank, once dispatch settles.
static void apply_swimming_roll(struct MarioState *m) {
    if (sSwimming.rollFrame != 0) {
        s32 roll = sSwimming.rollStartAngle
                   + (0x10000 - sSwimming.rollStartAngle) * sSwimming.rollFrame / SWIMMING_ROLL_FRAMES;
        s16 bank = m->faceAngle[2] + roll;
        center_swimming_roll(m, bank);
        m->marioObj->header.gfx.angle[2] = bank;
        if (sSwimming.rollFrame == SWIMMING_ROLL_FRAMES) {
            sSwimming.rollFrame = 0;
            sSwimming.rollCooldown = MARIO_TWIRL_COOLDOWN_FRAMES;
        }
    }
}

// Only the first A press outside the A-swimming sequence uses release timing.
static s32 is_a_swimming_action(struct MarioState *m) {
    switch (m->action) {
        case ACT_BREASTSTROKE:
        case ACT_SWIMMING_END:
        case ACT_HOLD_BREASTSTROKE:
        case ACT_HOLD_SWIMMING_END:
        case ACT_HOLD_FLUTTER_KICK:
            return TRUE;
        case ACT_FLUTTER_KICK:
            return m->actionArg == ACT_ARG_FLUTTER_KICK_A_SWIM;
    }
    return FALSE;
}

static s32 can_control_swimming(struct MarioState *m) {
    if (m->floor == NULL) {
        return FALSE;
    }
    if (m->heldObj != NULL) {
        return m->action == ACT_HOLD_WATER_IDLE || m->action == ACT_HOLD_WATER_ACTION_END
               || m->action == ACT_HOLD_BREASTSTROKE || m->action == ACT_HOLD_SWIMMING_END
               || m->action == ACT_HOLD_FLUTTER_KICK;
    }
    return is_normal_swimming_action(m)
           || (m->action == ACT_WATER_PUNCH && m->actionState != ACT_STATE_WATER_PUNCH_PICK_UP);
}

static s32 b_swim_has_priority(struct MarioState *m) {
    return (m->input & INPUT_B_DOWN)
           && (!(m->input & INPUT_A_DOWN) || sSwimming.swimButton == INPUT_B_DOWN);
}

static void update_swim_button_priority(struct MarioState *m) {
    u32 pressed = m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED);
    u32 held = m->input & (INPUT_A_DOWN | INPUT_B_DOWN);

    if (m->heldObj != NULL || (m->flags & MARIO_METAL_CAP) || m->floor == NULL
        || (!can_control_swimming(m) && m->action != ACT_WATER_PLUNGE
            && m->action != ACT_WATER_DOWN_BURST)) {
        sSwimming.swimButton = 0;
        return;
    }

    if (held != (INPUT_A_DOWN | INPUT_B_DOWN)) {
        // Releasing the winner falls back to the button still held.
        sSwimming.swimButton = held;
    } else if (pressed == INPUT_A_PRESSED) {
        sSwimming.swimButton = INPUT_A_DOWN;
    } else if (pressed == INPUT_B_PRESSED) {
        sSwimming.swimButton = INPUT_B_DOWN;
    } else if (sSwimming.swimButton == 0) {
        // Simultaneous presses have no ordering: retain the current swim mode.
        sSwimming.swimButton = is_a_swimming_action(m) ? INPUT_A_DOWN : INPUT_B_DOWN;
    }
}

static void clear_initial_a_tap(void) {
    sSwimming.aTapFrames = 0;
    sSwimming.aTapPending = FALSE;
    sSwimming.strokeQueued = FALSE;
}

static void update_initial_a_tap(struct MarioState *m) {
    // A released tap is available only during this update's action loop.
    sSwimming.strokeQueued = FALSE;
    if (!can_control_swimming(m) || (m->heldObj == NULL && b_swim_has_priority(m))) {
        clear_initial_a_tap();
        return;
    }
    if (sSwimming.aTapPending && sSwimming.aTapHolding != (m->heldObj != NULL)) {
        clear_initial_a_tap();
    }

    if ((m->input & INPUT_A_PRESSED) && !is_a_swimming_action(m)) {
        sSwimming.aTapFrames = 0;
        sSwimming.aTapPending = TRUE;
        sSwimming.aTapHolding = m->heldObj != NULL;
    } else if (sSwimming.aTapPending) {
        sSwimming.aTapFrames++;
    }

    if (sSwimming.aTapPending) {
        if (sSwimming.aTapFrames > A_SWIM_STROKE_TAP_FRAMES) {
            clear_initial_a_tap();
        } else if (!(m->input & INPUT_A_DOWN)) {
            sSwimming.strokeQueued = TRUE;
            sSwimming.aTapPending = FALSE;
        }
    }
}

static void clear_air_entry_stroke(void) {
    sSwimming.entryTapFrames = 0;
    sSwimming.entryTapPending = FALSE;
    sSwimming.entryStrokeReady = FALSE;
}

// Carry the ordinary A tap across an airborne water entry. The
// swimming action loop may begin on the same update as the crossing.
static void update_air_entry_stroke(struct MarioState *m) {
    s32 airborne = (m->action & ACT_FLAG_AIR) != 0;
    s32 swimming = is_normal_swimming_action(m);

    if (m->floor == NULL || m->heldObj != NULL || (m->flags & MARIO_METAL_CAP)
        || (!airborne && m->action != ACT_WATER_PLUNGE && !swimming)) {
        clear_air_entry_stroke();
        return;
    }

    if (airborne && (m->input & INPUT_A_PRESSED)) {
        sSwimming.entryTapFrames = 0;
        sSwimming.entryTapPending = TRUE;
        sSwimming.entryStrokeReady = FALSE;
    } else if (!airborne && (m->input & INPUT_A_PRESSED)) {
        // A new underwater press uses the existing swimming input path.
        clear_air_entry_stroke();
    } else if (sSwimming.entryTapPending || sSwimming.entryStrokeReady) {
        if (++sSwimming.entryTapFrames > AIR_ENTRY_SWIM_STROKE_TAP_FRAMES) {
            clear_air_entry_stroke();
        } else if (sSwimming.entryTapPending && !(m->input & INPUT_A_DOWN)) {
            sSwimming.entryTapPending = FALSE;
            sSwimming.entryStrokeReady = TRUE;
        }
    }

    if (!airborne && b_swim_has_priority(m)) {
        clear_air_entry_stroke();
    } else if (swimming && sSwimming.entryStrokeReady) {
        sSwimming.strokeQueued = TRUE;
        clear_air_entry_stroke();
    }
}

// Discard custom controls on interruption, including transitions within the action loop.
// Return whether this action can record B taps. Both cooldowns continue during other actions.
static s32 clear_interrupted_swimming_controls(struct MarioState *m) {
    s32 normalSwimming = is_normal_swimming_action(m);
    s32 canBuffer =
        normalSwimming
        || (m->action == ACT_WATER_PUNCH && m->actionState != ACT_STATE_WATER_PUNCH_PICK_UP);

    if (m->floor == NULL || m->heldObj != NULL || (m->flags & MARIO_METAL_CAP)) {
        normalSwimming = FALSE;
        canBuffer = FALSE;
    }
    if (!can_control_swimming(m)
        || (sSwimming.aTapPending && sSwimming.aTapHolding != (m->heldObj != NULL))) {
        clear_initial_a_tap();
    }
    if (!normalSwimming) {
        cancel_swimming_roll(m);
        sSwimming.strokeBuffered = FALSE;
        sSwimming.burstRecovering = FALSE;
    }
    if (!canBuffer) {
        sSwimming.bTapFrames = 0;
        sSwimming.bTapPending = FALSE;
        sSwimming.punchQueued = FALSE;
    }
    return canBuffer;
}

// B releases can queue a punch through cooldown; holding A/B again cancels it.
static void update_swimming_b_tap(struct MarioState *m) {
    if (!clear_interrupted_swimming_controls(m)) {
        return;
    }

    // Resuming held-button swimming cancels a previously completed tap.
    if (m->input & (INPUT_A_DOWN | INPUT_B_DOWN)) {
        sSwimming.punchQueued = FALSE;
    }
    if (m->input & INPUT_B_PRESSED) {
        sSwimming.bTapFrames = 0;
        sSwimming.bTapPending = TRUE;
    } else if (sSwimming.bTapPending) {
        sSwimming.bTapFrames++;
    }

    // Include releases exactly on the final allowed frame. A long hold never attacks.
    if (sSwimming.bTapPending) {
        if (sSwimming.bTapFrames > B_SWIM_ATTACK_TAP_FRAMES) {
            sSwimming.bTapPending = FALSE;
        } else if (!(m->input & INPUT_B_DOWN)) {
            sSwimming.punchQueued = !(m->input & INPUT_A_DOWN);
            sSwimming.bTapPending = FALSE;
        }
    }
}

// Called once per Mario update, before the action loop. Pause/time stop does not age inputs,
// and changing actions several times in one update cannot age this buffer twice.
static void update_water_down_repeat_buffer(struct MarioState *m) {
    if (m->action != ACT_WATER_DOWN_BURST) {
        sSwimming.waterDownRepeatFrames = 0;
    } else if (sSwimming.waterDownRepeatFrames > 0) {
        sSwimming.waterDownRepeatFrames--;
    }
}

void mario_swimming_update(struct MarioState *m) {
    update_swimming_roll(m);
    update_water_down_repeat_buffer(m);
    if (!(m->action & ACT_FLAG_SWIMMING) || m->pos[1] < m->waterLevel - 130) {
        sWasAtSurface = FALSE;
        sSwimming.surfaceSplashPlayed = FALSE;
    } else if (m->vel[1] <= WATER_JUMP_MIN_UPWARD_SPEED) {
        sSwimming.surfaceSplashPlayed = FALSE;
    }

    update_swim_button_priority(m);
    update_initial_a_tap(m);
    update_air_entry_stroke(m);
    if (sSwimming.strokeCooldown > 0) {
        sSwimming.strokeCooldown--;
    }
    if (b_swim_has_priority(m)) {
        // Choosing B cancels a pending stroke, but cannot shorten its cooldown.
        sSwimming.strokeBuffered = FALSE;
    }
    if (sSwimming.waterDownImpactCooldown > 0) {
        sSwimming.waterDownImpactCooldown--;
    }
    if (sSwimming.punchCooldown > 0) {
        sSwimming.punchCooldown--;
    }
    update_swimming_b_tap(m);
}

static void set_swimming_at_surface_particles(struct MarioState *m, u32 particleFlag) {
    s16 atSurface = m->pos[1] >= m->waterLevel - 130;

    if (atSurface) {
        if (particleFlag != PARTICLE_WAVE_TRAIL || gGlobalTimer % 3 == 0) {
            m->particleFlags |= particleFlag;
        }
        if (atSurface ^ sWasAtSurface) {
            // A surface clamp may already have cleared this update's upward velocity.
            if (MAX(m->vel[1], sSwimming.surfaceExitVelocityY) > WATER_JUMP_MIN_UPWARD_SPEED) {
                play_sound(SOUND_ACTION_WATER_JUMP, m->marioObj->header.gfx.cameraToObject);
                m->particleFlags |= PARTICLE_WATER_SPLASH;
                sSwimming.surfaceSplashPlayed = TRUE;
            } else {
                play_sound(SOUND_ACTION_SWIM_SURFACE, m->marioObj->header.gfx.cameraToObject);
            }
        }
    }

    sWasAtSurface = atSurface;
}

static s32 swimming_near_surface(struct MarioState *m) {
    if (m->flags & MARIO_METAL_CAP) {
        return FALSE;
    }

    return (m->waterLevel - 80) - m->pos[1] < 400.0f;
}

static f32 get_buoyancy(struct MarioState *m) {
    f32 buoyancy = 0.0f;

    if (m->flags & MARIO_METAL_CAP) {
        if (m->action & ACT_FLAG_INVULNERABLE) {
            buoyancy = -2.0f;
        } else {
            buoyancy = -18.0f;
        }
    } else if (swimming_near_surface(m)) {
        buoyancy = 1.25f;
    } else if (!(m->action & ACT_FLAG_MOVING)) {
        buoyancy = -2.0f;
    }

    if (mario_character_palette() == MARIO_PALETTE_HE_THICC && !swimming_near_surface(m)
        && !(m->action & ACT_FLAG_INVULNERABLE)) {
        buoyancy -= HE_THICC_WATER_SINK_SPEED;
    }
    return buoyancy;
}

static void apply_water_current(struct MarioState *m, Vec3f step) {
    if (game_options_cheat_active(GO_IGNORE_CURRENTS)) {
        return;
    }
    s32 i;
    f32 whirlpoolRadius = 2000.0f;
    f32 distance, strength;
    s16 pitchToWhirlpool, yawToWhirlpool;

    if (m->floor->type == SURFACE_FLOWING_WATER) {
        s16 currentAngle = m->floor->force << 8;
        f32 currentSpeed = sWaterCurrentSpeeds[m->floor->force >> 8];

        step[0] += currentSpeed * sins(currentAngle);
        step[2] += currentSpeed * coss(currentAngle);
    }

    for (i = 0; i < ARRAY_COUNT(gCurrentArea->whirlpools); i++) {
        struct Whirlpool *whirlpool = gCurrentArea->whirlpools[i];
        if (whirlpool != NULL) {
            strength = 0.0f;

            vec3_get_dist_and_angle(m->pos, whirlpool->pos, &distance, &pitchToWhirlpool, &yawToWhirlpool);

            yawToWhirlpool -= (s16)(0x2000 * 1000.0f / (distance + 1000.0f));

            if (whirlpool->strength >= 0) {
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
                if (gCurrLevelNum == LEVEL_DDD && gCurrAreaIndex == 2) {
                    whirlpoolRadius = 4000.0f;
                }
#endif
                if (distance >= 26.0f && distance < whirlpoolRadius) {
                    strength = whirlpool->strength * (1.0f - distance / whirlpoolRadius);
                }
            } else if (distance < 2000.0f) {
                strength = whirlpool->strength * (1.0f - distance / 2000.0f);
            }

            f32 cosPitch = strength * coss(pitchToWhirlpool);
            step[0] += cosPitch * sins(yawToWhirlpool);
            step[1] += strength * sins(pitchToWhirlpool);
            step[2] += cosPitch * coss(yawToWhirlpool);
        }
    }
}

// Use incoming momentum: the water solver has already clipped the contact-normal velocity.
static void bounce_swimming_roll(struct MarioState *m, Vec3f incoming, u32 stepResult,
                                 Vec3f floorNormal) {
    if (sSwimming.rollFrame == 0) {
        return;
    }
    s32 hitFloor = stepResult == WATER_STEP_HIT_FLOOR && floorNormal[1] > 0.0f;
    Vec3f direction;
    if (hitFloor) {
        vec3f_copy(direction, incoming);
        f32 approach = direction[0] * floorNormal[0] + direction[1] * floorNormal[1]
                       + direction[2] * floorNormal[2];
        if (approach >= 0.0f) {
            // A stopped swimmer can still be aiming into the bottom.
            vec3f_set(direction, coss(m->faceAngle[0]) * sins(m->faceAngle[1]), sins(m->faceAngle[0]),
                      coss(m->faceAngle[0]) * coss(m->faceAngle[1]));
            approach = direction[0] * floorNormal[0] + direction[1] * floorNormal[1]
                       + direction[2] * floorNormal[2];
        }
        if (approach >= 0.0f) {
            hitFloor = FALSE;
        } else {
            if (sSwimming.rollFloorContact
                && absf(sSwimming.rollFloorNormal[0] - floorNormal[0]) < 0.0001f
                && absf(sSwimming.rollFloorNormal[1] - floorNormal[1]) < 0.0001f
                && absf(sSwimming.rollFloorNormal[2] - floorNormal[2]) < 0.0001f) {
                return;
            }
            direction[0] -= 2.0f * approach * floorNormal[0];
            direction[1] -= 2.0f * approach * floorNormal[1];
            direction[2] -= 2.0f * approach * floorNormal[2];
            f32 length = sqrtf(vec3f_dot(direction, direction));
            for (s32 axis = 0; axis < 3; axis++) {
                direction[axis] /= length;
            }
            f32 horizontal = sqrtf(sqr(direction[0]) + sqr(direction[2]));
            m->faceAngle[0] = atan2s(horizontal, direction[1]);
            if (horizontal > 0.0001f) {
                m->faceAngle[1] = atan2s(direction[2], direction[0]);
            }
        }
    }
    if (!hitFloor) {
        sSwimming.rollFloorContact = FALSE;
        if (m->wall == NULL) {
            sSwimming.rollWallContact = FALSE;
            return;
        }
        if (sSwimming.rollWallContact && sSwimming.rollWallYaw == m->wallYaw) {
            return;
        }
        f32 approach = incoming[0] * m->wall->normal.x + incoming[2] * m->wall->normal.z;
        f32 facingApproach =
            sins(m->faceAngle[1]) * m->wall->normal.x + coss(m->faceAngle[1]) * m->wall->normal.z;
        // A blocked swimmer can have no incoming momentum but still face into the wall.
        if (approach < 0.0f || (approach == 0.0f && facingApproach < 0.0f)) {
            m->faceAngle[1] = 2 * m->wallYaw - m->faceAngle[1] + 0x8000;
        }
    }
    mario_play_wall_bonk_sound(m);
    m->particleFlags |= PARTICLE_VERTICAL_STAR;
    if (sSwimming.rollBounceCount < SWIMMING_ROLL_ZERO_SPEED_COUNT) {
        sSwimming.rollBounceCount++;
    }
    f32 boost = SWIMMING_ROLL_BOUNCE_BOOST;
    if (sSwimming.rollBounceCount == 1 && sSwimming.rollFrame - 1 < SWIMMING_ROLL_EARLY_FRAMES) {
        boost = SWIMMING_ROLL_EARLY_BOOST;
    }
    f32 retained = 1.0f - (sSwimming.rollBounceCount - 1) * SWIMMING_ROLL_BOUNCE_SPEED_LOSS;
    if (sSwimming.rollBounceCount == SWIMMING_ROLL_ZERO_SPEED_COUNT) {
        retained = 0.0f;
    }
    m->forwardVel = (absf(m->forwardVel) + boost) * SWIMMING_ROLL_SPEED_MULTIPLIER * retained;
    if (!game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        m->forwardVel = MIN(m->forwardVel, SWIMMING_ROLL_SPEED_CAP);
    }
    if (hitFloor) {
        for (s32 axis = 0; axis < 3; axis++) {
            m->vel[axis] = m->forwardVel * direction[axis];
        }
        vec3f_set(sSwimming.rollFloorNormal, floorNormal[0], floorNormal[1], floorNormal[2]);
        sSwimming.rollFloorContact = TRUE;
        sSwimming.rollWallContact = FALSE;
    } else {
        f32 horizontal = m->forwardVel * coss(m->faceAngle[0]);
        m->vel[0] = horizontal * sins(m->faceAngle[1]);
        m->vel[2] = horizontal * coss(m->faceAngle[1]);
        // Wall rebounds preserve the solver's vertical momentum and surface clamp.
        sSwimming.rollWallYaw = m->wallYaw;
        sSwimming.rollWallContact = TRUE;
    }
    sSwimming.rollStartAngle +=
        (0x10000 - sSwimming.rollStartAngle) * (sSwimming.rollFrame - 1) / SWIMMING_ROLL_FRAMES;
    sSwimming.rollFrame = 1;
}

static u32 perform_water_step(struct MarioState *m) {
    Vec3f step, velocity, floorNormal;
    s32 surfaceClamped;
    u32 stepResult;

    mario_limit_velocity(m);
    vec3f_copy(velocity, m->vel);
    vec3f_copy(step, velocity);
    if (m->action & ACT_FLAG_SWIMMING) {
        apply_water_current(m, step);
    }
    mario_limit_velocity_vector(step);
    surfaceClamped = m->pos[1] + step[1] > m->waterLevel - 80;

    // Clip actual movement against solids without consuming stored swim speed.
    stepResult = mario_water_collision_step_with_floor(m, step, velocity, floorNormal);
    vec3f_copy(m->vel, velocity);
    if (surfaceClamped) {
        // The water surface itself preserves launch speed; a solid above it
        // still removes any momentum directed into that solid.
        sSwimming.surfaceExitVelocityY = velocity[1];
        m->vel[1] = 0.0f;
    }

    if (stepResult != WATER_STEP_CANCELLED) {
        bounce_swimming_roll(m, step, stepResult, floorNormal);
    }
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, -m->faceAngle[0], m->faceAngle[1], m->faceAngle[2]);
    return stepResult;
}

// Apply pose-only pitch/height adjustments after water movement.
static void adjust_swimming_render_pose(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;

    if (marioObj->header.gfx.angle[0] > 0) {
        marioObj->header.gfx.pos[1] +=
            60.0f * sins(marioObj->header.gfx.angle[0]) * sins(marioObj->header.gfx.angle[0]);
    }

    if (marioObj->header.gfx.angle[0] < 0) {
        marioObj->header.gfx.angle[0] = marioObj->header.gfx.angle[0] * 6 / 10;
    }

    if (marioObj->header.gfx.angle[0] > 0) {
        marioObj->header.gfx.angle[0] = marioObj->header.gfx.angle[0] * 10 / 8;
    }
}

static void update_swimming_pitch(struct MarioState *m);

static void stationary_slow_down(struct MarioState *m, f32 forwardDecel, f32 riseAccel, f32 sinkAccel) {
    f32 buoyancy = get_buoyancy(m);

    m->angleVel[0] = 0;
    m->angleVel[1] = 0;
    m->forwardVel = approach_f32(m->forwardVel, 0.0f, forwardDecel, forwardDecel);
    m->vel[1] = approach_f32(m->vel[1], buoyancy, riseAccel, sinkAccel);

    // Respond to held vertical input from the first normal water-entry update.
    if (m->action == ACT_WATER_PLUNGE && !(m->flags & MARIO_METAL_CAP)
        && m->controller->stickY != 0.0f) {
        if (m->actionTimer == 1
            && ((m->input & INPUT_A_DOWN) || (m->heldObj == NULL && (m->input & INPUT_B_DOWN)))) {
            // Start a held-button swim at 30 degrees; normal turning resumes next frame.
            m->faceAngle[0] = m->controller->stickY > 0.0f ? -DEGREES(30) : DEGREES(30);
        } else {
            update_swimming_pitch(m);
        }
    } else {
        m->faceAngle[0] = approach_s32(m->faceAngle[0], 0, 0x200, 0x200);
    }
    m->faceAngle[2] = approach_s32(m->faceAngle[2], 0, 0x100, 0x100);

    m->vel[0] = m->forwardVel * coss(m->faceAngle[0]) * sins(m->faceAngle[1]);
    m->vel[2] = m->forwardVel * coss(m->faceAngle[0]) * coss(m->faceAngle[1]);
}

// Recover from a completed ground pound without replacing its vertical velocity instantly.
// An attack, held object, or other interruption resumes that action's ordinary physics.
static void update_swimming_vertical_velocity(struct MarioState *m, f32 targetVelocityY) {
    if (sSwimming.burstRecovering && is_normal_swimming_action(m) && m->heldObj == NULL
        && !(m->flags & MARIO_METAL_CAP)) {
        m->vel[1] = approach_f32(m->vel[1], targetVelocityY, WATER_DOWN_BURST_VERTICAL_BRAKE,
                                 WATER_DOWN_BURST_VERTICAL_BRAKE);
        sSwimming.burstRecovering = m->vel[1] != targetVelocityY;
    } else {
        sSwimming.burstRecovering = FALSE;
        m->vel[1] = targetVelocityY;
    }
}

// Actions supply their own drag. Held-button swimming passes zero after applying its target.
static void update_swimming_speed(struct MarioState *m, f32 decelThreshold, f32 drag) {
    if (m->forwardVel < 0.0f) {
        m->forwardVel = 0.0f;
    }
    if (m->forwardVel > MAX_SWIM_SPEED && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        m->forwardVel = MAX_SWIM_SPEED;
    }
    if (m->forwardVel > MIN(decelThreshold, SWIM_SOFT_SPEED_LIMIT)) {
        m->forwardVel -= drag;
    }

    m->vel[0] = m->forwardVel * coss(m->faceAngle[0]) * sins(m->faceAngle[1]);
    m->vel[2] = m->forwardVel * coss(m->faceAngle[0]) * coss(m->faceAngle[1]);
    update_swimming_vertical_velocity(m, m->forwardVel * sins(m->faceAngle[0]) + get_buoyancy(m));
}

static void update_swimming_yaw(struct MarioState *m) {
    f32 yawSpeed =
        ((m->input & INPUT_B_DOWN) && m->forwardVel < A_SWIM_SPEED) ? B_SWIM_YAW_SPEED : SWIM_YAW_SPEED;
    s16 targetYawVel = -(s16) (yawSpeed * m->controller->stickX);

    if (targetYawVel > 0) {
        if (m->angleVel[1] < 0) {
            m->angleVel[1] += 0x40;
            if (m->angleVel[1] > 0x10) {
                m->angleVel[1] = 0x10;
            }
        } else {
            m->angleVel[1] = approach_s32(m->angleVel[1], targetYawVel, 0x10, 0x20);
        }
    } else if (targetYawVel < 0) {
        if (m->angleVel[1] > 0) {
            m->angleVel[1] -= 0x40;
            if (m->angleVel[1] < -0x10) {
                m->angleVel[1] = -0x10;
            }
        } else {
            m->angleVel[1] = approach_s32(m->angleVel[1], targetYawVel, 0x20, 0x10);
        }
    } else {
        m->angleVel[1] = approach_s32(m->angleVel[1], 0, 0x40, 0x40);
    }

    m->faceAngle[1] += m->angleVel[1];
    m->faceAngle[2] = -m->angleVel[1] * 8;
}

static void update_swimming_pitch(struct MarioState *m) {
    s16 targetPitch = -(s16) (DEGREES(SWIM_MAX_PITCH_DEGREES) * m->controller->stickY / 64.0f);
    s16 pitchVel = (s16) (DEGREES(SWIM_PITCH_SPEED_DEGREES) / 30.0f + 0.5f);

    if (m->faceAngle[0] < targetPitch) {
        if ((m->faceAngle[0] += pitchVel) > targetPitch) {
            m->faceAngle[0] = targetPitch;
        }
    } else if (m->faceAngle[0] > targetPitch) {
        if ((m->faceAngle[0] -= pitchVel) < targetPitch) {
            m->faceAngle[0] = targetPitch;
        }
    }
}

static void common_idle_step(struct MarioState *m, s32 animation, s32 animAccel) {
    s16 *headAngle = &m->marioBodyState->headAngle[0];

    update_swimming_yaw(m);
    update_swimming_pitch(m);
    m->forwardVel -= 2.0f * WATER_IDLE_DRAG_SCALE;
    update_swimming_speed(m, MIN_SWIM_SPEED, SWIM_DRAG * WATER_IDLE_DRAG_SCALE);
    perform_water_step(m);
    adjust_swimming_render_pose(m);

    if (m->faceAngle[0] > 0) {
        *headAngle = approach_s32(*headAngle, m->faceAngle[0] / 2, 0x80, 0x200);
    } else {
        *headAngle = approach_s32(*headAngle, 0, 0x200, 0x200);
    }

    if (animAccel == 0) {
        set_mario_animation(m, animation);
    } else {
        set_mario_anim_with_accel(m, animation, animAccel);
    }

    set_swimming_at_surface_particles(m, PARTICLE_IDLE_WATER_WAVE);
}

// B starts swimming immediately; only a completed short tap can attack.
// Input capture and cooldown ticking happen once in mario_swimming_update().
static s32 check_swimming_b_input(struct MarioState *m) {
    if (b_swim_has_priority(m)) {
        if (m->action != ACT_FLUTTER_KICK) {
            return set_mario_action(m, ACT_FLUTTER_KICK, ACT_ARG_FLUTTER_KICK_B_SWIM);
        }
        m->actionArg = ACT_ARG_FLUTTER_KICK_B_SWIM;
    } else if (sSwimming.punchQueued && !(m->input & INPUT_A_DOWN) && sSwimming.punchCooldown == 0) {
        sSwimming.punchQueued = FALSE;
        return set_mario_action(m, ACT_WATER_PUNCH, 0);
    }
    return FALSE;
}

static s32 check_water_jump(struct MarioState *m);

static s32 check_initial_swim_stroke(struct MarioState *m) {
    if (m->heldObj == NULL) {
        if (sSwimming.strokeQueued) {
            sSwimming.strokeBuffered = TRUE;
            sSwimming.strokeQueued = FALSE;
        }
        if (!sSwimming.strokeBuffered || sSwimming.strokeCooldown != 0) {
            return FALSE;
        }
        sSwimming.strokeBuffered = FALSE;
        return set_mario_action(m, ACT_BREASTSTROKE, 0);
    }
    if (sSwimming.strokeQueued) {
        sSwimming.strokeQueued = FALSE;
        return set_mario_action(m, ACT_HOLD_BREASTSTROKE, 0);
    }
    return FALSE;
}

static s32 start_a_swimming(struct MarioState *m) {
    if (check_initial_swim_stroke(m)) {
        return TRUE;
    }
    if (m->input & INPUT_A_PRESSED) {
        // Surface jumps still respond to the press, not the tap release.
        if (check_water_jump(m)) {
            return TRUE;
        }
        return set_mario_action(m, m->heldObj == NULL ? ACT_FLUTTER_KICK : ACT_HOLD_FLUTTER_KICK,
                                ACT_ARG_FLUTTER_KICK_A_SWIM);
    }
    return FALSE;
}

static s32 act_water_idle(struct MarioState *m) {
    u32 animAccel = 0x10000;

    if (m->flags & MARIO_METAL_CAP) {
        return set_mario_action(m, ACT_METAL_WATER_FALLING, 1);
    }

    if (check_swimming_b_input(m)) {
        return TRUE;
    }

    if (start_a_swimming(m)) {
        return TRUE;
    }

    if (m->faceAngle[0] < -0x1000) {
        animAccel = 0x30000;
    }

    common_idle_step(m, MARIO_ANIM_WATER_IDLE, animAccel);
    return FALSE;
}

static s32 act_hold_water_idle(struct MarioState *m) {
    if (m->flags & MARIO_METAL_CAP) {
        return set_mario_action(m, ACT_HOLD_METAL_WATER_FALLING, 0);
    }

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_WATER_IDLE, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_WATER_THROW, 0);
    }

    if (start_a_swimming(m)) {
        return TRUE;
    }

    common_idle_step(m, MARIO_ANIM_WATER_IDLE_WITH_OBJ, 0);
    return FALSE;
}

static s32 water_ground_pound_landing(struct MarioState *m) {
    m->forwardVel = 0.0f;
    m->vel[2] = 0.0f;
    m->vel[1] = 0.0f;
    m->vel[0] = 0.0f;
    set_mario_animation(m, MARIO_ANIM_GROUND_POUND_LANDING);
    perform_water_step(m);
    if (is_anim_at_end(m) || m->floor == NULL || m->pos[1] > m->floorHeight + 1.0f) {
        if (b_swim_has_priority(m)) {
            set_mario_action(m, ACT_FLUTTER_KICK, ACT_ARG_FLUTTER_KICK_B_SWIM);
            return FALSE;
        }
        if (m->input & INPUT_A_DOWN) {
            set_mario_action(m, ACT_FLUTTER_KICK, ACT_ARG_FLUTTER_KICK_A_SWIM);
            return FALSE;
        }
        set_mario_action(m, ACT_WATER_IDLE, 0);
    }
    return FALSE;
}

static s32 act_water_action_end(struct MarioState *m) {
    if (m->flags & MARIO_METAL_CAP) {
        return set_mario_action(m, ACT_METAL_WATER_FALLING, 1);
    }

    if (m->actionArg == ACT_ARG_WATER_ACTION_END_GROUND_POUND) {
        return water_ground_pound_landing(m);
    }

    if (check_swimming_b_input(m)) {
        return TRUE;
    }

    if (start_a_swimming(m)) {
        return TRUE;
    }

    common_idle_step(m, MARIO_ANIM_WATER_ACTION_END, 0);
    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_WATER_IDLE, 0);
    }
    return FALSE;
}

static s32 act_hold_water_action_end(struct MarioState *m) {
    if (m->flags & MARIO_METAL_CAP) {
        return set_mario_action(m, ACT_HOLD_METAL_WATER_FALLING, 0);
    }

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_WATER_IDLE, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_WATER_THROW, 0);
    }

    if (start_a_swimming(m)) {
        return TRUE;
    }

    common_idle_step(
        m, m->actionArg == 0 ? MARIO_ANIM_WATER_ACTION_END_WITH_OBJ : MARIO_ANIM_STOP_GRAB_OBJ_WATER,
        0);
    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_HOLD_WATER_IDLE, 0);
    }
    return FALSE;
}

static void reset_bob_variables(struct MarioState *m) {
    sBobTimer = 0;
    sBobIncrement = 0x800;
    sBobHeight = m->faceAngle[0] / 256.0f + 20.0f;
}

/**
 * Controls the bobbing that happens when you swim near the surface.
 */
static void surface_swim_bob(struct MarioState *m) {
    if (sBobIncrement != 0 && m->pos[1] > m->waterLevel - 85 && m->faceAngle[0] >= 0) {
        if ((sBobTimer += sBobIncrement) >= 0) {
            m->marioObj->header.gfx.pos[1] += sBobHeight * sins(sBobTimer);
            return;
        }
    }

    sBobIncrement = 0;
}

static void common_swimming_step(struct MarioState *m, s16 swimStrength, f32 drag) {
    s16 previousPitch = m->faceAngle[0];

    update_swimming_yaw(m);
    update_swimming_pitch(m);
    update_swimming_speed(m, swimStrength / 10.0f, drag);

    switch (perform_water_step(m)) {
        case WATER_STEP_HIT_FLOOR:
            // Follow the bottom automatically unless the player deliberately
            // holds downward input. Keep swept collision and speed handling.
            if (m->controller->stickY <= 0.0f
                && !(sSwimming.rollFrame != 0 && sSwimming.rollFloorContact)) {
                // Use the contacted plane, avoiding rounded nearby floor samples
                // that can select a different triangle at seams.
                s16 floorPitch =
                    atan2s(m->floor->normal.y, -(m->floor->normal.x * sins(m->faceAngle[1])
                                                 + m->floor->normal.z * coss(m->faceAngle[1])));
                // Ease from the previous frame so neutral pitch steering cannot
                // cancel the assistance. Never pull an upward input back down.
                s16 assistedPitch = MIN(previousPitch + 0x200, floorPitch);
                m->faceAngle[0] = MAX(m->faceAngle[0], assistedPitch);
            }
            break;

        case WATER_STEP_HIT_CEILING:
            if (m->faceAngle[0] > -0x3000) {
                m->faceAngle[0] -= 0x100;
            }
            break;

        case WATER_STEP_HIT_WALL:
            // A wall redirects travel, not the player's swimming pitch.
            break;
    }

    adjust_swimming_render_pose(m);
    m->marioBodyState->headAngle[0] = approach_s32(m->marioBodyState->headAngle[0], 0, 0x200, 0x200);

    surface_swim_bob(m);
    set_swimming_at_surface_particles(m, PARTICLE_WAVE_TRAIL);
}

static void play_swimming_noise(struct MarioState *m) {
    s16 animFrame = m->marioObj->header.gfx.animInfo.animFrame;

    if (animFrame == 0 || animFrame == 12) {
        play_sound(SOUND_ACTION_FLUTTER_KICK, m->marioObj->header.gfx.cameraToObject);
    }
}

static s32 check_water_jump(struct MarioState *m) {
    s32 probe = (s32)(m->pos[1] + 1.5f);

    if (m->input & INPUT_A_PRESSED) {
        if (probe >= m->waterLevel - 80 && m->faceAngle[0] >= 0 && m->controller->stickY < -60.0f) {
            mario_stop_underwater_shell(m);
            vec3_zero(m->angleVel);

            m->vel[1] = 62.0f;

            if (m->heldObj == NULL) {
                return set_mario_action(m, ACT_WATER_JUMP, 0);
            } else {
                return set_mario_action(m, ACT_HOLD_WATER_JUMP, 0);
            }
        }
    }

    return FALSE;
}

static s32 act_breaststroke(struct MarioState *m) {
    f32 strokeGain = 0.0f;
    if (m->actionTimer == 0) {
        // Reserve the full stroke and recovery even if B interrupts the animation.
        // Timer 1 runs now; timer 14 enters recovery on the same update.
        sSwimming.strokeCooldown = BREASTSTROKE_DURATION - 1 + BREASTSTROKE_RECOVERY_MIN;
        sSwimming.strokeBuffered = FALSE;
    }
    if (m->actionArg == 0) {
        sSwimStrength = MIN_SWIM_STRENGTH;
    }

    if (m->flags & MARIO_METAL_CAP) {
        return set_mario_action(m, ACT_METAL_WATER_FALLING, 1);
    }

    if (check_swimming_b_input(m)) {
        return TRUE;
    }

    // A fresh press during this stroke queues one more; ignore the press that started it.
    if (m->actionTimer > 0 && (m->input & INPUT_A_PRESSED)) {
        m->actionState = ACT_STATE_BREASTSTROKE_CONTINUE;
    }

    if (++m->actionTimer == BREASTSTROKE_DURATION) {
        // A queued stroke follows manual timing: finish this stroke, then wait
        // through swimming-end recovery. Its normal frame-7 check starts the next stroke.
        if (m->actionState == ACT_STATE_BREASTSTROKE_CONTINUE && (m->input & INPUT_A_DOWN)) {
            // Match releasing A on the first flutter-kick frame.
            if (sSwimStrength < MAX_SWIM_STRENGTH) {
                sSwimStrength += 10;
            }
            return set_mario_action(m, ACT_SWIMMING_END, 0);
        }
        return set_mario_action(m, ACT_FLUTTER_KICK, ACT_ARG_FLUTTER_KICK_A_SWIM);
    }

    if (check_water_jump(m)) {
        return TRUE;
    }

    // Original stroke timing, with 1.5 times the original acceleration per frame.
    if (m->actionTimer < 6) {
        strokeGain = BREASTSTROKE_INITIAL_ACCEL;
    }

    if (m->actionTimer >= 9) {
        strokeGain = BREASTSTROKE_FINISH_ACCEL;
    }
    m->forwardVel += strokeGain;

    if (m->actionTimer == 1) {
        mario_he_thicc_breaststroke(m);
        play_sound_if_no_flag(
            m, sSwimStrength == MIN_SWIM_STRENGTH ? SOUND_ACTION_SWIM : SOUND_ACTION_SWIM_FAST,
            MARIO_ACTION_SOUND_PLAYED);
        reset_bob_variables(m);
    }

    if (m->actionTimer < 6) {
        queue_rumble_submerged();
    }

    set_mario_animation(m, MARIO_ANIM_SWIM_PART1);
    common_swimming_step(m, sSwimStrength,
                         m->forwardVel > SWIM_SOFT_SPEED_LIMIT ? BREASTSTROKE_ABOVE_SOFT_DRAG
                                                               : BREASTSTROKE_DRAG);

    return FALSE;
}

static s32 act_swimming_end(struct MarioState *m) {
    if (m->flags & MARIO_METAL_CAP) {
        return set_mario_action(m, ACT_METAL_WATER_FALLING, 1);
    }

    if (check_swimming_b_input(m)) {
        return TRUE;
    }

    if (m->actionTimer >= 15) {
        return set_mario_action(m, ACT_WATER_ACTION_END, 0);
    }

    if (check_water_jump(m)) {
        return TRUE;
    }

    if (check_initial_swim_stroke(m)) {
        return TRUE;
    }

    if ((m->input & INPUT_A_DOWN) && m->actionTimer >= BREASTSTROKE_RECOVERY_MIN) {
        // Late input resumes cruising instead of restarting a nearly recovered stroke.
        if (m->actionTimer > 9) {
            return set_mario_action(m, ACT_FLUTTER_KICK, ACT_ARG_FLUTTER_KICK_A_SWIM);
        }
        if (sSwimming.strokeCooldown == 0) {
            if (m->actionTimer == BREASTSTROKE_RECOVERY_MIN && sSwimStrength < MAX_SWIM_STRENGTH) {
                sSwimStrength += 10;
            }
            return set_mario_action(m, ACT_BREASTSTROKE, 1);
        }
    }

    if (m->actionTimer >= BREASTSTROKE_RECOVERY_MIN) {
        sSwimStrength = MIN_SWIM_STRENGTH;
    }

    m->actionTimer++;

    m->forwardVel -= SWIM_RECOVERY_DECEL;
    set_mario_animation(m, MARIO_ANIM_SWIM_PART2);
    common_swimming_step(m, sSwimStrength, SWIM_DRAG);

    return FALSE;
}

static s32 act_flutter_kick(struct MarioState *m) {
    if (m->flags & MARIO_METAL_CAP) {
        return set_mario_action(m, ACT_METAL_WATER_FALLING, 1);
    }

    if (check_swimming_b_input(m)) {
        return TRUE;
    }

    if (check_initial_swim_stroke(m)) {
        return TRUE;
    }

    if (b_swim_has_priority(m)) {
        // The most recently pressed held button selects the swimming mechanics.
        m->forwardVel = approach_f32(m->forwardVel, B_SWIM_SPEED, B_SWIM_ACCEL, B_SWIM_DECEL);
    } else {
        m->actionArg = ACT_ARG_FLUTTER_KICK_A_SWIM;
        if (!(m->input & INPUT_A_DOWN)) {
            if (m->actionTimer == 0 && sSwimStrength < MAX_SWIM_STRENGTH) {
                sSwimStrength += 10;
            }
            return set_mario_action(m, ACT_SWIMMING_END, 0);
        }

        // Preserve faster strokes up to the soft limit, then ease down to it.
        if (m->forwardVel > SWIM_SOFT_SPEED_LIMIT) {
            m->forwardVel = approach_f32(m->forwardVel, SWIM_SOFT_SPEED_LIMIT, SWIM_DRAG, SWIM_DRAG);
        } else if (m->forwardVel < A_SWIM_SPEED) {
            m->forwardVel = approach_f32(m->forwardVel, A_SWIM_SPEED, A_SWIM_ACCEL, A_SWIM_ACCEL);
        }
    }
    m->actionTimer = 1;
    sSwimStrength = MIN_SWIM_STRENGTH;

    // Keep the current animation playing until A is confirmed as a hold.
    // A short tap can then enter breaststroke without flashing the flutter pose.
    if (!sSwimming.aTapPending) {
        play_swimming_noise(m);
        set_mario_animation(m, MARIO_ANIM_FLUTTERKICK);
    }

    common_swimming_step(m, sSwimStrength, 0.0f);
    return FALSE;
}

static s32 act_hold_breaststroke(struct MarioState *m) {
    f32 strokeGain = 0.0f;
    if (m->flags & MARIO_METAL_CAP) {
        return set_mario_action(m, ACT_HOLD_METAL_WATER_FALLING, 0);
    }

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_WATER_IDLE, 0);
    }

    if (++m->actionTimer == 17) {
        return set_mario_action(m, ACT_HOLD_FLUTTER_KICK, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_WATER_THROW, 0);
    }

    if (check_water_jump(m)) {
        return TRUE;
    }

    if (m->actionTimer < 6) {
        strokeGain = 0.5f;
    }

    if (m->actionTimer >= 9) {
        strokeGain = 1.5f;
    }
    m->forwardVel += strokeGain;

    if (m->actionTimer >= 2) {
        if (m->actionTimer < 6 && (m->input & INPUT_A_PRESSED)) {
            m->actionState = ACT_STATE_BREASTSTROKE_CONTINUE;
        }

        if (m->actionTimer == 9 && m->actionState == ACT_STATE_BREASTSTROKE_CONTINUE) {
            set_anim_to_frame(m, 0);
            m->actionState = ACT_STATE_BREASTSTROKE_START;
            m->actionTimer = 1;
            player_stats_add(PS_SWIMMING_STROKES, 1);
        }
    }

    if (m->actionTimer == 1) {
        mario_he_thicc_breaststroke(m);
        play_sound(SOUND_ACTION_SWIM, m->marioObj->header.gfx.cameraToObject);
        reset_bob_variables(m);
    }

    set_mario_animation(m, MARIO_ANIM_SWIM_WITH_OBJ_PART1);
    common_swimming_step(m, 160, SWIM_DRAG);
    return FALSE;
}

static s32 act_hold_swimming_end(struct MarioState *m) {
    if (m->flags & MARIO_METAL_CAP) {
        return set_mario_action(m, ACT_HOLD_METAL_WATER_FALLING, 0);
    }

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_WATER_IDLE, 0);
    }

    if (m->actionTimer >= 15) {
        return set_mario_action(m, ACT_HOLD_WATER_ACTION_END, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_WATER_THROW, 0);
    }

    if (check_water_jump(m)) {
        return TRUE;
    }

    if ((m->input & INPUT_A_DOWN) && m->actionTimer >= 7) {
        return set_mario_action(m, ACT_HOLD_BREASTSTROKE, 0);
    }

    m->actionTimer++;

    m->forwardVel -= 0.25f;
    set_mario_animation(m, MARIO_ANIM_SWIM_WITH_OBJ_PART2);
    common_swimming_step(m, 160, SWIM_DRAG);
    return FALSE;
}

static s32 act_hold_flutter_kick(struct MarioState *m) {
    if (m->flags & MARIO_METAL_CAP) {
        return set_mario_action(m, ACT_HOLD_METAL_WATER_FALLING, 0);
    }

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_WATER_IDLE, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_WATER_THROW, 0);
    }

    if (check_initial_swim_stroke(m)) {
        return TRUE;
    }

    if (!(m->input & INPUT_A_DOWN)) {
        return set_mario_action(m, ACT_HOLD_SWIMMING_END, 0);
    }

    m->forwardVel = approach_f32(m->forwardVel, 12.0f, 0.1f, 0.15f);
    if (!sSwimming.aTapPending && m->forwardVel < 14.0f) {
        play_swimming_noise(m);
        set_mario_animation(m, MARIO_ANIM_FLUTTERKICK_WITH_OBJ);
    }
    common_swimming_step(m, 160, SWIM_DRAG);
    return FALSE;
}

static s32 act_water_shell_swimming(struct MarioState *m) {
    if ((m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) && !mario_keeps_shell(m)) {
        return drop_and_set_mario_action(m, ACT_WATER_IDLE, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_WATER_THROW, 0);
    }

    if (check_water_jump(m)) {
        return TRUE;
    }

    m->forwardVel = approach_f32(m->forwardVel, 30.0f, 2.0f, 1.0f);

    play_swimming_noise(m);
    set_mario_animation(m, MARIO_ANIM_FLUTTERKICK_WITH_OBJ);
    common_swimming_step(m, 300, SWIM_DRAG);

    return FALSE;
}

static s32 check_water_grab(struct MarioState *m) {
    //! Heave hos have the grabbable interaction type but are not normally
    // grabbable. Since water grabbing doesn't check the appropriate input flag,
    // you can use water grab to pick up heave ho.
    if (m->marioObj->collidedObjInteractTypes & INTERACT_GRABBABLE) {
        struct Object *object = mario_get_collided_object(m, INTERACT_GRABBABLE);
        f32 dx = object->oPosX - m->pos[0];
        f32 dz = object->oPosZ - m->pos[2];
        s16 dAngleToObject = atan2s(dz, dx) - m->faceAngle[1];

        if (dAngleToObject >= -DEGREES(60) && dAngleToObject <= DEGREES(60)) {
            m->usedObj = object;
            mario_grab_used_object(m);
            m->marioBodyState->grabPos = GRAB_POS_LIGHT_OBJ;
            return TRUE;
        }
    }

    return FALSE;
}

static s32 act_water_throw(struct MarioState *m) {
    update_swimming_yaw(m);
    update_swimming_pitch(m);
    update_swimming_speed(m, MIN_SWIM_SPEED, SWIM_DRAG);
    perform_water_step(m);
    adjust_swimming_render_pose(m);

    set_mario_animation(m, MARIO_ANIM_WATER_THROW_OBJ);
    play_sound_if_no_flag(m, SOUND_ACTION_SWIM, MARIO_ACTION_SOUND_PLAYED);

    m->marioBodyState->headAngle[0] = approach_s32(m->marioBodyState->headAngle[0], 0, 0x200, 0x200);

    if (m->actionTimer++ == 5) {
        mario_throw_held_object(m);
        queue_rumble_data(3, 50);
    }

    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_WATER_IDLE, 0);
    }

    return FALSE;
}

static s32 act_water_punch(struct MarioState *m) {
    if (m->forwardVel < 7.0f) {
        m->forwardVel += 1.0f;
    }

    update_swimming_yaw(m);
    update_swimming_pitch(m);
    update_swimming_speed(m, MIN_SWIM_SPEED, SWIM_DRAG);
    perform_water_step(m);
    adjust_swimming_render_pose(m);

    m->marioBodyState->headAngle[0] = approach_s32(m->marioBodyState->headAngle[0], 0, 0x200, 0x200);

    play_sound_if_no_flag(m, SOUND_ACTION_SWIM, MARIO_ACTION_SOUND_PLAYED);

    switch (m->actionState) {
        case ACT_STATE_WATER_PUNCH_START:
            set_mario_animation(m, MARIO_ANIM_WATER_GRAB_OBJ_PART1);
            if (is_anim_at_end(m)) {
                m->actionState = check_water_grab(m) + 1;
            }
            break;

        case ACT_STATE_WATER_PUNCH_MISSED:
            set_mario_animation(m, MARIO_ANIM_WATER_GRAB_OBJ_PART2);
            // Let held swim buttons cancel the final recovery frames.
            if ((m->input & (INPUT_A_DOWN | INPUT_B_DOWN))
                && m->marioObj->header.gfx.animInfo.animFrame
                       >= m->marioObj->header.gfx.animInfo.curAnim->loopEnd
                              - WATER_PUNCH_SWIM_CANCEL_FRAMES) {
                sSwimming.punchCooldown = WATER_PUNCH_SWIM_CANCEL_FRAMES;
                set_mario_action(m, ACT_FLUTTER_KICK,
                                 b_swim_has_priority(m) ? ACT_ARG_FLUTTER_KICK_B_SWIM
                                                        : ACT_ARG_FLUTTER_KICK_A_SWIM);
            } else if (is_anim_at_end(m)) {
                sSwimming.punchCooldown = WATER_PUNCH_SWIM_CANCEL_FRAMES;
                set_mario_action(m, ACT_WATER_ACTION_END, 0);
            }
            break;

        case ACT_STATE_WATER_PUNCH_PICK_UP:
            set_mario_animation(m, MARIO_ANIM_WATER_PICK_UP_OBJ);
            if (is_anim_at_end(m)) {
                if (m->heldObj->behavior == segmented_to_virtual(bhvKoopaShellUnderwater)) {
                    play_shell_music();
                    set_mario_action(m, ACT_WATER_SHELL_SWIMMING, 0);
                } else {
                    set_mario_action(m, ACT_HOLD_WATER_ACTION_END, 1);
                }
            }
            break;
    }

    return FALSE;
}

static void common_water_knockback_step(struct MarioState *m, s32 animation, u32 endAction, s32 actionArg) {
    stationary_slow_down(m, 1.0f, 2.0f, 1.0f);
    perform_water_step(m);
    set_mario_animation(m, animation);

    m->marioBodyState->headAngle[0] = 0;

    if (is_anim_at_end(m)) {
        if (actionArg > 0) {
            m->invincTimer = 30;
        }

        if (m->health < 0x100) {
            set_mario_action(m, ACT_WATER_DEATH, 0);
        } else if (!(m->flags & MARIO_METAL_CAP) && m->heldObj == NULL
                   && (m->bullyKnockbackBuffer & INPUT_A_PRESSED)) {
            set_mario_action(m, ACT_BREASTSTROKE, 0);
        } else if (!(m->flags & MARIO_METAL_CAP) && m->heldObj == NULL
                   && (m->bullyKnockbackBuffer & INPUT_B_PRESSED)) {
            set_mario_action(m, ACT_WATER_PUNCH, 0);
        } else {
            set_mario_action(m, endAction, 0);
        }
    }
}

static s32 act_backward_water_kb(struct MarioState *m) {
    common_water_knockback_step(m, MARIO_ANIM_BACKWARDS_WATER_KB, ACT_WATER_IDLE, m->actionArg);
    return FALSE;
}

static s32 act_forward_water_kb(struct MarioState *m) {
    common_water_knockback_step(m, MARIO_ANIM_WATER_FORWARD_KB, ACT_WATER_IDLE, m->actionArg);
    return FALSE;
}

static s32 act_water_shocked(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_WAAAOOOW, MARIO_ACTION_SOUND_PLAYED);
    play_sound(SOUND_MOVING_SHOCKED, m->marioObj->header.gfx.cameraToObject);
    set_camera_shake_from_hit(SHAKE_SHOCK);

    if (set_mario_animation(m, MARIO_ANIM_SHOCKED) == 0) {
        m->actionTimer++;
    }

    if (m->actionTimer >= 6) {
        m->invincTimer = 30;
        set_mario_action(m, m->health < 0x100 ? ACT_WATER_DEATH : ACT_WATER_IDLE, 0);
    }

    stationary_slow_down(m, 1.0f, 2.0f, 1.0f);
    perform_water_step(m);
    m->marioBodyState->headAngle[0] = 0;
    return FALSE;
}

static s32 act_drowning(struct MarioState *m) {
    switch (m->actionState) {
        case ACT_STATE_DROWNING_EYES_HALF_CLOSED:
            set_mario_animation(m, MARIO_ANIM_DROWNING_PART1);
            m->marioBodyState->eyeState = MARIO_EYES_HALF_CLOSED;
            if (is_anim_at_end(m)) {
                m->actionState = ACT_STATE_DROWNING_EYES_DEAD;
            }
            break;

        case ACT_STATE_DROWNING_EYES_DEAD:
            set_mario_animation(m, MARIO_ANIM_DROWNING_PART2);
            m->marioBodyState->eyeState = MARIO_EYES_DEAD;
            if (m->marioObj->header.gfx.animInfo.animFrame == 30) {
                level_trigger_warp(m, WARP_OP_DEATH);
            }
            break;
    }

    play_sound_if_no_flag(m, SOUND_MARIO_DROWNING, MARIO_ACTION_SOUND_PLAYED);
    stationary_slow_down(m, 1.0f, 2.0f, 1.0f);
    perform_water_step(m);

    return FALSE;
}

static s32 act_water_death(struct MarioState *m) {
    stationary_slow_down(m, 1.0f, 2.0f, 1.0f);
    perform_water_step(m);

    m->marioBodyState->eyeState = MARIO_EYES_DEAD;

    set_mario_animation(m, MARIO_ANIM_WATER_DYING);
    if (set_mario_animation(m, MARIO_ANIM_WATER_DYING) == 35) {
        level_trigger_warp(m, WARP_OP_DEATH);
    }

    return FALSE;
}

static s32 act_water_plunge(struct MarioState *m) {
    u32 stepResult;
    s32 stateFlags = m->heldObj != NULL;

    f32 endVSpeed;
    if (swimming_near_surface(m)) {
        endVSpeed = 0.0f;
    } else {
        endVSpeed = -5.0f;
    }

    if (m->flags & MARIO_METAL_CAP) {
        stateFlags |= PLUNGE_FLAG_METAL_CAP;
    } else if ((m->prevAction & ACT_FLAG_DIVING) || (m->input & INPUT_A_DOWN)
               || (m->heldObj == NULL && (m->input & INPUT_B_DOWN))) {
        stateFlags |= PLUNGE_FLAG_DIVING;
    }

    m->actionTimer++;

    // Entry preserves forward speed and uses lighter vertical drag.
    stationary_slow_down(m, WATER_PLUNGE_FORWARD_DRAG, 2.0f * WATER_PLUNGE_DRAG_SCALE,
                         WATER_PLUNGE_DRAG_SCALE);

    stepResult = perform_water_step(m);

    if (m->actionState == ACT_STATE_WATER_PLUNGE_FALL) {
        play_sound(SOUND_ACTION_WATER_PLUNGE, m->marioObj->header.gfx.cameraToObject);
        if (m->peakHeight - m->pos[1] > FALL_DAMAGE_HEIGHT_SMALL) {
            play_sound(SOUND_MARIO_HAHA_WATER, m->marioObj->header.gfx.cameraToObject);
        }

        m->particleFlags |= PARTICLE_WATER_SPLASH;
        m->actionState = ACT_STATE_WATER_PLUNGE_SINK;
        if (m->prevAction & ACT_FLAG_AIR) {
            queue_rumble_data(5, 80);
        }
    }

    if (!(m->flags & MARIO_METAL_CAP) && m->heldObj == NULL
        && ((m->input & INPUT_A_PRESSED) || sSwimming.entryStrokeReady) && !b_swim_has_priority(m)) {
        // A fresh or buffered stroke input takes over the plunge on this update. The
        // plunge has already taken its movement step and played its entry splash.
        clear_air_entry_stroke();
        sSwimming.strokeCooldown = 0;
        sBobIncrement = 0;
        set_mario_action(m, ACT_BREASTSTROKE, 0);
        // Use a separate playback channel so the stroke and entry splash begin together.
        play_sound_if_no_flag(m, SOUND_ACTION_SWIM_OVER_PLUNGE, MARIO_ACTION_SOUND_PLAYED);
        set_mario_animation(m, MARIO_ANIM_SWIM_PART1);
        m->particleFlags |= PARTICLE_WATER_ENTRY_BUBBLE;
        return FALSE;
    }

    if (stepResult == WATER_STEP_HIT_FLOOR || m->vel[1] >= endVSpeed || m->actionTimer > 20) {
        switch (stateFlags) {
            case PLUNGE_FLAGS_NONE:
                set_mario_action(m, ACT_WATER_ACTION_END, 0);
                break;
            case PLUNGE_FLAG_HOLDING_OBJ:
                set_mario_action(m, ACT_HOLD_WATER_ACTION_END, 0);
                break;
            case PLUNGE_FLAG_DIVING:
                set_mario_action(m, ACT_FLUTTER_KICK,
                                 b_swim_has_priority(m) ? ACT_ARG_FLUTTER_KICK_B_SWIM
                                                        : ACT_ARG_FLUTTER_KICK_A_SWIM);
                break;
            case (PLUNGE_FLAG_DIVING | PLUNGE_FLAG_HOLDING_OBJ):
                set_mario_action(m, ACT_HOLD_FLUTTER_KICK, 0);
                break;
            case PLUNGE_FLAG_METAL_CAP:
                set_mario_action(m, ACT_METAL_WATER_FALLING, 0);
                break;
            case (PLUNGE_FLAG_METAL_CAP | PLUNGE_FLAG_HOLDING_OBJ):
                set_mario_action(m, ACT_HOLD_METAL_WATER_FALLING, 0);
                break;
        }
        sBobIncrement = 0;
    }

    switch (stateFlags) {
        case PLUNGE_FLAGS_NONE:
            set_mario_animation(m, MARIO_ANIM_WATER_ACTION_END);
            break;
        case PLUNGE_FLAG_HOLDING_OBJ:
            set_mario_animation(m, MARIO_ANIM_WATER_ACTION_END_WITH_OBJ);
            break;
        case PLUNGE_FLAG_DIVING:
            set_mario_animation(m, MARIO_ANIM_FLUTTERKICK);
            break;
        case (PLUNGE_FLAG_DIVING | PLUNGE_FLAG_HOLDING_OBJ):
            set_mario_animation(m, MARIO_ANIM_FLUTTERKICK_WITH_OBJ);
            break;
        case PLUNGE_FLAG_METAL_CAP:
            set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
            break;
        case (PLUNGE_FLAG_METAL_CAP | PLUNGE_FLAG_HOLDING_OBJ):
            set_mario_animation(m, MARIO_ANIM_FALL_WITH_LIGHT_OBJ);
            break;
    }

    m->particleFlags |= PARTICLE_WATER_ENTRY_BUBBLE;
    return FALSE;
}

static s32 act_caught_in_whirlpool(struct MarioState *m) {
    f32 sinAngleChange;
    f32 cosAngleChange;
    f32 newDistance;
    s16 angleChange;

    struct Object *marioObj = m->marioObj;
    struct Object *whirlpool = m->usedObj;

    f32 dx = m->pos[0] - whirlpool->oPosX;
    f32 dz = m->pos[2] - whirlpool->oPosZ;
    f32 distance = sqrtf(sqr(dx) + sqr(dz));

    if ((marioObj->oMarioWhirlpoolPosY += m->vel[1]) < 0.0f) {
        marioObj->oMarioWhirlpoolPosY = 0.0f;
        if (distance < 16.1f && m->actionTimer++ == 16) {
            level_trigger_warp(m, WARP_OP_DEATH);
        }
    }

    if (distance <= 28.0f) {
        newDistance = 16.0f;
        angleChange = 0x1800;
    } else if (distance < 256.0f) {
        newDistance = distance - (12.0f - distance / 32.0f);
        angleChange = (s16)(0x1C00 - distance * 20.0f);
    } else {
        newDistance = distance - 4.0f;
        angleChange = 0x800;
    }

    m->vel[1] = -640.0f / (newDistance + 16.0f);

    sinAngleChange = sins(angleChange);
    cosAngleChange = coss(angleChange);

    if (distance < 1.0f) {
        dx = newDistance * sins(m->faceAngle[1]);
        dz = newDistance * coss(m->faceAngle[1]);
    } else {
        dx *= newDistance / distance;
        dz *= newDistance / distance;
    }

    m->pos[0] = whirlpool->oPosX + dx * cosAngleChange + dz * sinAngleChange;
    m->pos[2] = whirlpool->oPosZ - dx * sinAngleChange + dz * cosAngleChange;
    m->pos[1] = whirlpool->oPosY + marioObj->oMarioWhirlpoolPosY;

    m->faceAngle[1] = atan2s(dz, dx) + 0x8000;

    set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    reset_rumble_timers_slip();

    return FALSE;
}

static void play_metal_water_jumping_sound(struct MarioState *m, u32 landing) {
    if (!(m->flags & MARIO_ACTION_SOUND_PLAYED)) {
        m->particleFlags |= PARTICLE_MIST_CIRCLE;
    }

    play_sound_if_no_flag(m, landing ? SOUND_ACTION_METAL_LAND_WATER : SOUND_ACTION_METAL_JUMP_WATER,
                          MARIO_ACTION_SOUND_PLAYED);
}

static void play_metal_water_walking_sound(struct MarioState *m) {
    if (is_anim_past_frame(m, 10) || is_anim_past_frame(m, 49)) {
        play_sound(SOUND_ACTION_METAL_STEP_WATER, m->marioObj->header.gfx.cameraToObject);
        m->particleFlags |= PARTICLE_DUST;
    }
}

static void update_metal_water_walking_speed(struct MarioState *m) {
    if (game_options_get(GO_METAL_CAP) == 0) {
        update_walking_speed(m);
        return;
    }

    f32 targetSpeed = m->intendedMag / 1.5f;

    if (m->forwardVel <= 0.0f) {
        m->forwardVel += 1.1f;
    } else if (m->forwardVel <= targetSpeed) {
        m->forwardVel += 1.1f - m->forwardVel / 43.0f;
    } else if (m->floor->normal.y >= 0.95f) {
        m->forwardVel -= 1.0f;
    }

    if (m->forwardVel > 32.0f && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        m->forwardVel = 32.0f;
    }

    m->faceAngle[1] = approach_angle(m->faceAngle[1], m->intendedYaw, 0x800);

    m->slideVelX = m->forwardVel * sins(m->faceAngle[1]);
    m->slideVelZ = m->forwardVel * coss(m->faceAngle[1]);

    m->vel[0] = m->slideVelX;
    m->vel[1] = 0.0f;
    m->vel[2] = m->slideVelZ;
}

static s32 update_metal_water_jump_speed(struct MarioState *m) {
    if (game_options_get(GO_METAL_CAP) == 0) {
        if (m->vel[1] > 0.0f && m->pos[1] > m->waterLevel - 100.0f) {
            return TRUE;
        }
        update_air_without_turn(m);
        return FALSE;
    }

    f32 waterSurface = m->waterLevel - 100;

    if (m->vel[1] > 0.0f && m->pos[1] > waterSurface) {
        return TRUE;
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        s16 intendedDYaw = m->intendedYaw - m->faceAngle[1];
        m->forwardVel += 0.8f * coss(intendedDYaw);
        m->faceAngle[1] += 0x200 * sins(intendedDYaw);
    } else {
        m->forwardVel = approach_f32(m->forwardVel, 0.0f, 0.25f, 0.25f);
    }

    if (m->forwardVel > 16.0f) {
        m->forwardVel -= 1.0f;
    }

    if (m->forwardVel < 0.0f) {
        m->forwardVel += 2.0f;
    }

    m->vel[0] = m->slideVelX = m->forwardVel * sins(m->faceAngle[1]);
    m->vel[2] = m->slideVelZ = m->forwardVel * coss(m->faceAngle[1]);
    return FALSE;
}

static void update_metal_water_falling_speed(struct MarioState *m) {
    if (game_options_get(GO_METAL_CAP) == 1) {
        if (m->input & INPUT_NONZERO_ANALOG) {
            m->faceAngle[1] += 0x400 * sins(m->intendedYaw - m->faceAngle[1]);
        }
        stationary_slow_down(m, 1.0f, 2.0f, 1.0f);
        return;
    }
    // Keep the metal water sink rate while using ordinary airborne steering.
    vec3_zero(m->angleVel);
    m->vel[1] = approach_f32(m->vel[1], get_buoyancy(m), 2.0f, 1.0f);
    m->faceAngle[0] = approach_s32(m->faceAngle[0], 0, 0x200, 0x200);
    m->faceAngle[2] = approach_s32(m->faceAngle[2], 0, 0x100, 0x100);
    update_air_without_turn(m);
}

static s32 act_metal_water_standing(struct MarioState *m) {
    if (!(m->flags & MARIO_METAL_CAP)) {
        return set_mario_action(m, ACT_WATER_IDLE, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        return set_mario_action(m, ACT_METAL_WATER_JUMP, 0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        return set_mario_action(m, ACT_METAL_WATER_WALKING, 0);
    }

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

    stop_and_set_height_to_floor(m);
    if (m->pos[1] >= m->waterLevel - 150) {
        m->particleFlags |= PARTICLE_IDLE_WATER_WAVE;
    }

    return FALSE;
}

static s32 act_hold_metal_water_standing(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_METAL_WATER_STANDING, 0);
    }

    if (!(m->flags & MARIO_METAL_CAP)) {
        return set_mario_action(m, ACT_HOLD_WATER_IDLE, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        return set_mario_action(m, ACT_HOLD_METAL_WATER_JUMP, 0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        return set_mario_action(m, ACT_HOLD_METAL_WATER_WALKING, 0);
    }

    stop_and_set_height_to_floor(m);
    set_mario_animation(m, MARIO_ANIM_IDLE_WITH_LIGHT_OBJ);
    return FALSE;
}

static s32 act_metal_water_walking(struct MarioState *m) {
    s32 animSpeed;

    if (!(m->flags & MARIO_METAL_CAP)) {
        return set_mario_action(m, ACT_WATER_IDLE, 0);
    }

    if (m->input & INPUT_FIRST_PERSON) {
        return set_mario_action(m, ACT_METAL_WATER_STANDING, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        return set_mario_action(m, ACT_METAL_WATER_JUMP, 0);
    }

    if (m->input & INPUT_IDLE) {
        return set_mario_action(m, ACT_METAL_WATER_STANDING, 0);
    }

    if ((animSpeed = (s32)(m->forwardVel / 4.0f * 0x10000)) < 0x1000) {
        animSpeed = 0x1000;
    }

    set_mario_anim_with_accel(m, MARIO_ANIM_WALKING, animSpeed);
    play_metal_water_walking_sound(m);
    update_metal_water_walking_speed(m);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_METAL_WATER_FALLING, 1);
            break;

        case GROUND_STEP_HIT_WALL:
            m->forwardVel = 0.0f;
            break;
    }

    return FALSE;
}

static s32 act_hold_metal_water_walking(struct MarioState *m) {
    s32 animSpeed;

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_METAL_WATER_WALKING, 0);
    }

    if (!(m->flags & MARIO_METAL_CAP)) {
        return set_mario_action(m, ACT_HOLD_WATER_IDLE, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        return set_mario_action(m, ACT_HOLD_METAL_WATER_JUMP, 0);
    }

    if (m->input & INPUT_IDLE) {
        return set_mario_action(m, ACT_HOLD_METAL_WATER_STANDING, 0);
    }

    m->intendedMag *= 0.4f;

    if ((animSpeed = (s32)(m->forwardVel / 2.0f * 0x10000)) < 0x1000) {
        animSpeed = 0x1000;
    }

    set_mario_anim_with_accel(m, MARIO_ANIM_RUN_WITH_LIGHT_OBJ, animSpeed);
    play_metal_water_walking_sound(m);
    update_metal_water_walking_speed(m);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_HOLD_METAL_WATER_FALLING, 1);
            break;

        case GROUND_STEP_HIT_WALL:
            m->forwardVel = 0.0f;
            break;
    }

    return FALSE;
}

static s32 act_metal_water_jump(struct MarioState *m) {
    if (!(m->flags & MARIO_METAL_CAP)) {
        return set_mario_action(m, ACT_WATER_IDLE, 0);
    }

    if (update_metal_water_jump_speed(m)) {
        return set_mario_action(m, ACT_WATER_JUMP, 1);
    }

    play_metal_water_jumping_sound(m, FALSE);
    set_mario_animation(m, MARIO_ANIM_SINGLE_JUMP);

    switch (perform_air_step(m, AIR_STEP_CHECK_NONE)) {
        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_METAL_WATER_JUMP_LAND, 0);
            break;

        case AIR_STEP_HIT_WALL:
            m->forwardVel = 0.0f;
            break;
    }

    return FALSE;
}

static s32 act_hold_metal_water_jump(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_METAL_WATER_FALLING, 0);
    }

    if (!(m->flags & MARIO_METAL_CAP)) {
        return set_mario_action(m, ACT_HOLD_WATER_IDLE, 0);
    }

    if (update_metal_water_jump_speed(m)) {
        return set_mario_action(m, ACT_HOLD_WATER_JUMP, 1);
    }

    play_metal_water_jumping_sound(m, FALSE);
    set_mario_animation(m, MARIO_ANIM_JUMP_WITH_LIGHT_OBJ);

    switch (perform_air_step(m, AIR_STEP_CHECK_NONE)) {
        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_HOLD_METAL_WATER_JUMP_LAND, 0);
            break;

        case AIR_STEP_HIT_WALL:
            m->forwardVel = 0.0f;
            break;
    }

    return FALSE;
}

static s32 act_metal_water_falling(struct MarioState *m) {
    if (!(m->flags & MARIO_METAL_CAP)) {
        return set_mario_action(m, ACT_WATER_IDLE, 0);
    }

    set_mario_animation(m, m->actionArg == 0 ? MARIO_ANIM_GENERAL_FALL : MARIO_ANIM_FALL_FROM_WATER);
    update_metal_water_falling_speed(m);

    if (perform_water_step(m) & WATER_STEP_HIT_FLOOR) { // hit floor or cancelled
        set_mario_action(m, ACT_METAL_WATER_FALL_LAND, 0);
    }

    return FALSE;
}

static s32 act_hold_metal_water_falling(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_METAL_WATER_FALLING, 0);
    }

    if (!(m->flags & MARIO_METAL_CAP)) {
        return set_mario_action(m, ACT_HOLD_WATER_IDLE, 0);
    }

    set_mario_animation(m, MARIO_ANIM_FALL_WITH_LIGHT_OBJ);
    update_metal_water_falling_speed(m);

    if (perform_water_step(m) & WATER_STEP_HIT_FLOOR) { // hit floor or cancelled
        set_mario_action(m, ACT_HOLD_METAL_WATER_FALL_LAND, 0);
    }

    return FALSE;
}

static s32 act_metal_water_jump_land(struct MarioState *m) {
    play_metal_water_jumping_sound(m, TRUE);

    if (!(m->flags & MARIO_METAL_CAP)) {
        return set_mario_action(m, ACT_WATER_IDLE, 0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        return set_mario_action(m, ACT_METAL_WATER_WALKING, 0);
    }

    stop_and_set_height_to_floor(m);
    set_mario_animation(m, MARIO_ANIM_LAND_FROM_SINGLE_JUMP);

    if (is_anim_at_end(m)) {
        return set_mario_action(m, ACT_METAL_WATER_STANDING, 0);
    }

    return FALSE;
}

static s32 act_hold_metal_water_jump_land(struct MarioState *m) {
    play_metal_water_jumping_sound(m, TRUE);

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_METAL_WATER_STANDING, 0);
    }

    if (!(m->flags & MARIO_METAL_CAP)) {
        return set_mario_action(m, ACT_HOLD_WATER_IDLE, 0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        return set_mario_action(m, ACT_HOLD_METAL_WATER_WALKING, 0);
    }

    stop_and_set_height_to_floor(m);
    set_mario_animation(m, MARIO_ANIM_JUMP_LAND_WITH_LIGHT_OBJ);

    if (is_anim_at_end(m)) {
        return set_mario_action(m, ACT_HOLD_METAL_WATER_STANDING, 0);
    }

    return FALSE;
}

static s32 act_metal_water_fall_land(struct MarioState *m) {
    play_metal_water_jumping_sound(m, TRUE);

    if (!(m->flags & MARIO_METAL_CAP)) {
        return set_mario_action(m, ACT_WATER_IDLE, 0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        return set_mario_action(m, ACT_METAL_WATER_WALKING, 0);
    }

    stop_and_set_height_to_floor(m);
    set_mario_animation(m, MARIO_ANIM_GENERAL_LAND);

    if (is_anim_at_end(m)) {
        return set_mario_action(m, ACT_METAL_WATER_STANDING, 0);
    }

    return FALSE;
}

static s32 act_hold_metal_water_fall_land(struct MarioState *m) {
    play_metal_water_jumping_sound(m, TRUE);

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_METAL_WATER_STANDING, 0);
    }

    if (!(m->flags & MARIO_METAL_CAP)) {
        return set_mario_action(m, ACT_HOLD_WATER_IDLE, 0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        return set_mario_action(m, ACT_HOLD_METAL_WATER_WALKING, 0);
    }

    stop_and_set_height_to_floor(m);
    set_mario_animation(m, MARIO_ANIM_FALL_LAND_WITH_LIGHT_OBJ);

    if (is_anim_at_end(m)) {
        return set_mario_action(m, ACT_HOLD_METAL_WATER_STANDING, 0);
    }

    return FALSE;
}

// End the pose and ease remaining vertical momentum into ordinary swimming.
static void finish_water_down_burst(struct MarioState *m, s32 hitFloor) {
    if (hitFloor && sSwimming.waterDownImpactCooldown == 0) {
        sSwimming.waterDownImpactCooldown = WATER_DOWN_IMPACT_COOLDOWN_FRAMES;
        m->waterGroundPoundImpact = TRUE;
        m->waterGroundPoundFloorObject = m->floor != NULL ? m->floor->object : NULL;
        mario_attack_cap_box_surface(m, m->floor, INT_GROUND_POUND_OR_TWIRL);
        m->particleFlags |= PARTICLE_MIST_CIRCLE | PARTICLE_HORIZONTAL_STAR;
        // The land sound helper also spawns shallow-water droplets at the surface.
        // Keep this impact's sound and floor particles together underwater.
        u32 sound = SOUND_ACTION_TERRAIN_HEAVY_LANDING + m->terrainSoundAddend;
        if (m->flags & MARIO_METAL_CAP) {
            sound = SOUND_ACTION_METAL_HEAVY_LANDING;
        }
        // Splash requests use priority 0xC0 in this bank; the impact must win.
        sound = (sound & ~SOUNDARGS_MASK_PRIORITY) | (0xD0 << SOUNDARGS_SHIFT_PRIORITY);
        play_sound(sound, m->marioObj->header.gfx.cameraToObject);
        mario_ground_pound_impact(m);
    }
    if (hitFloor) {
        sSwimming.burstRecovering = FALSE;
        set_mario_action(m, ACT_WATER_ACTION_END, ACT_ARG_WATER_ACTION_END_GROUND_POUND);
        return;
    }
    sSwimming.burstRecovering = !hitFloor && m->actionState == ACT_STATE_WATER_DOWN_BURST_SINK;
    if (b_swim_has_priority(m)) {
        set_mario_action(m, ACT_FLUTTER_KICK, ACT_ARG_FLUTTER_KICK_B_SWIM);
    } else if (m->input & INPUT_A_DOWN) {
        set_mario_action(m, ACT_FLUTTER_KICK, ACT_ARG_FLUTTER_KICK_A_SWIM);
    } else {
        set_mario_action(m, ACT_WATER_ACTION_END, 0);
    }
}

// Update the delayed ring and distance-spaced trail after collision, before exit/repeat.
static void update_water_down_burst_particles(struct MarioState *m, s32 burstStarted, u32 stepResult) {
    // Wait two updates after the impulse; landing during the wait cancels the ring.
    if (m->actionState == ACT_STATE_WATER_DOWN_BURST_SINK && m->actionTimer == 2
        && stepResult != WATER_STEP_HIT_FLOOR && m->floor != NULL
        && m->pos[1] - m->floorHeight >= 150.0f) {
        m->particleFlags |= PARTICLE_WATER_DOWN_MIST;
    }

    // Keep the initial puff; space subsequent puffs by downward travel so the
    // trail thins as Mario slows. Full burst speed gives one batch per 4 updates.
    if (m->actionState == ACT_STATE_WATER_DOWN_BURST_SINK) {
        if (burstStarted || (m->actionTimer == 0 && m->vel[1] < 0.0f)) {
            m->particleFlags |= PARTICLE_PLUNGE_BUBBLE;
        } else if (m->vel[1] < 0.0f) {
            sSwimming.waterDownBubbleDistance -= m->vel[1];
            if (sSwimming.waterDownBubbleDistance >= WATER_DOWN_BURST_SPEED * 4.0f) {
                sSwimming.waterDownBubbleDistance = 0.0f;
                m->particleFlags |= PARTICLE_PLUNGE_BUBBLE;
            }
        }
    }
}

// Animation phases control when the impulse fires and how long the pose lasts.
// Movement remains steerable; releasing the pose begins vertical recovery.
static s32 act_water_down_burst(struct MarioState *m) {
    u32 stepResult;
    s32 burstStarted = FALSE;
    f32 targetVerticalSpeed;
    s32 fromAir = (m->actionArg & ACT_ARG_WATER_DOWN_BURST_FROM_AIR) != 0;
    s32 weakBurst = !fromAir && !(m->input & INPUT_Z_DOWN);
    f32 burstStrength = weakBurst ? WATER_DOWN_WEAK_STRENGTH : 1.0f;

    if (m->flags & MARIO_METAL_CAP) {
        sSwimming.burstRecovering = FALSE;
        return set_mario_action(m, ACT_METAL_WATER_FALLING, 1);
    }

    if (m->actionState == ACT_STATE_WATER_DOWN_BURST_STARTUP && m->actionTimer == 0) {
        sSwimming.waterDownBubbleDistance = 0.0f;
        sSwimming.waterDownRepeatFrames = 0;
        // Match plunge priority so swimming sounds cannot suppress startup,
        // while the later plunge sound can still replace it when the burst fires.
        if (!fromAir) {
            play_sound_with_rumble((SOUND_ACTION_SPIN & ~SOUNDARGS_MASK_PRIORITY)
                                       | (0xC0 << SOUNDARGS_SHIFT_PRIORITY),
                                   m->marioObj->header.gfx.cameraToObject, 3, 30);
        }
        sSwimming.burstRecovering = FALSE;
    }

    switch (m->actionState) {
        case ACT_STATE_WATER_DOWN_BURST_STARTUP:
            if (fromAir) {
                // Continue the airborne pound: no repeated windup, delay, or impulse.
                m->actionState = ACT_STATE_WATER_DOWN_BURST_SINK;
                m->actionTimer = 0;
                player_stats_add(PS_DOWNWARD_SWIMMING_BURSTS, 1);
                set_mario_anim_with_accel(m, MARIO_ANIM_GROUND_POUND,
                                          (s32) (0x10000 * WATER_DOWN_ANIM_SPEED));
                m->particleFlags |= PARTICLE_WATER_SPLASH;
                play_sound(SOUND_ACTION_WATER_PLUNGE, m->marioObj->header.gfx.cameraToObject);
                break;
            }
            set_mario_anim_with_accel(m, MARIO_ANIM_START_GROUND_POUND,
                                      (s32) (0x10000 * WATER_DOWN_ANIM_SPEED));
            m->actionTimer++;
            if (is_anim_at_end(m)) {
                m->actionState = ACT_STATE_WATER_DOWN_BURST_DELAY;
                m->actionTimer = 0;
                play_sound(SOUND_ACTION_WATER_PLUNGE, m->marioObj->header.gfx.cameraToObject);
            }
            break;

        case ACT_STATE_WATER_DOWN_BURST_DELAY:
            set_mario_anim_with_accel(m, MARIO_ANIM_GROUND_POUND,
                                      (s32) (0x10000 * WATER_DOWN_ANIM_SPEED));
            m->actionTimer++;
            if (m->actionTimer >= WATER_DOWN_BURST_DELAY_FRAMES) {
                m->actionState = ACT_STATE_WATER_DOWN_BURST_SINK;
                m->actionTimer = 0;
                burstStarted = TRUE;
                player_stats_add(PS_DOWNWARD_SWIMMING_BURSTS, 1);
                if (!weakBurst) {
                    m->actionArg |= ACT_ARG_WATER_DOWN_BURST_STARTED_STRONG;
                    // Strength is settled here, after Z can be released during startup.
                    queue_rumble_data(5, 80);
                }
                // Choose the initial impulse once; preserve faster incoming descents.
                if (m->vel[1] > -WATER_DOWN_BURST_SPEED * burstStrength) {
                    m->vel[1] = -WATER_DOWN_BURST_SPEED * burstStrength;
                }
            }
            break;

        case ACT_STATE_WATER_DOWN_BURST_SINK:
            // A fresh press after release queues one repeat, never continuous holding.
            if ((m->input & INPUT_Z_PRESSED)
                && (m->actionArg
                    & (ACT_ARG_WATER_DOWN_BURST_FROM_AIR | ACT_ARG_WATER_DOWN_BURST_STARTED_STRONG))) {
                sSwimming.waterDownRepeatFrames = 6;
            }
            set_mario_anim_with_accel(m, MARIO_ANIM_GROUND_POUND,
                                      (s32) (0x10000 * WATER_DOWN_ANIM_SPEED));
            m->actionTimer++;
            break;
    }

    // Preserve pitch and use normal steering instead of snapping the swimmer horizontal.
    update_swimming_yaw(m);
    update_swimming_pitch(m);
    m->forwardVel = approach_f32(m->forwardVel, 0.0f, WATER_DOWN_BURST_BRAKE, WATER_DOWN_BURST_BRAKE);
    m->vel[0] = m->forwardVel * coss(m->faceAngle[0]) * sins(m->faceAngle[1]);
    m->vel[2] = m->forwardVel * coss(m->faceAngle[0]) * coss(m->faceAngle[1]);
    if (!burstStarted) {
        if (m->actionState == ACT_STATE_WATER_DOWN_BURST_SINK) {
            // Brake excess downward speed without accelerating or adding another impulse.
            targetVerticalSpeed = -WATER_DOWN_HOLD_SPEED * burstStrength;
            if (m->vel[1] < targetVerticalSpeed) {
                m->vel[1] += WATER_DOWN_SINK_DECEL;
                if (m->vel[1] > targetVerticalSpeed) {
                    m->vel[1] = targetVerticalSpeed;
                }
            }
        } else {
            targetVerticalSpeed = m->forwardVel * sins(m->faceAngle[0]) + get_buoyancy(m);
            m->vel[1] = approach_f32(m->vel[1], targetVerticalSpeed, 2.0f, 1.0f);
        }
    }
    m->marioBodyState->headAngle[0] = 0;
    stepResult = perform_water_step(m);

    update_water_down_burst_particles(m, burstStarted, stepResult);

    // Count the impulse or airborne-entry frame toward the shared minimum.
    // Airborne entry skips the delay, so it does not consume any of the hold cap.
    if (stepResult == WATER_STEP_HIT_FLOOR
        || (m->actionState == ACT_STATE_WATER_DOWN_BURST_SINK
            && (m->actionTimer + (fromAir ? 0 : WATER_DOWN_BURST_DELAY_FRAMES)
                    >= WATER_DOWN_BURST_MAX_HOLD_FRAMES
                || (!(m->input & INPUT_Z_DOWN) && m->actionTimer + 1 >= WATER_DOWN_BURST_FRAMES)))) {
        finish_water_down_burst(m, stepResult == WATER_STEP_HIT_FLOOR);
    } else if (m->actionState == ACT_STATE_WATER_DOWN_BURST_SINK && sSwimming.waterDownRepeatFrames > 0
               && m->actionTimer + 1 >= WATER_DOWN_BURST_FRAMES) {
        // Restart next update so this frame does not perform water movement twice.
        set_mario_action(m, ACT_WATER_DOWN_BURST, ACT_ARG_WATER_DOWN_BURST_NORMAL);
    }
    return FALSE;
}

static s32 check_common_submerged_cancels(struct MarioState *m) {
    s16 waterHeight = m->waterLevel - 80;
    if (m->pos[1] > waterHeight) {
        if (waterHeight > m->floorHeight) {
            if (m->pos[1] - waterHeight < 50) {
                m->pos[1] = waterHeight; // lock mario to top if the falloff isn't big enough
            } else {
                // m->pos[1] = m->waterLevel - 80; // Vanilla bug: Downwarp swimming out of waterfalls
                return transition_submerged_to_airborne(m);
            }
        } else {
            return transition_submerged_to_walking(m);
        }
    }

    if (m->health < 0x100 && !(m->action & (ACT_FLAG_INTANGIBLE | ACT_FLAG_INVULNERABLE))) {
        // Current Vanish cannot drown; Original retains the native drowning action.
        set_mario_action(m, mario_has_current_vanish_cap(m) ? ACT_WATER_DEATH : ACT_DROWNING, 0);
    }

    return FALSE;
}

static s32 transition_cap_swimming_to_land(struct MarioState *m) {
    if (!mario_has_land_movement_underwater(m) || m->health < 0x100
        || (!can_control_swimming(m) && !(m->action & ACT_FLAG_METAL_WATER)
            && m->action != ACT_WATER_PLUNGE && m->action != ACT_WATER_DOWN_BURST
            && m->action != ACT_WATER_SHELL_SWIMMING)) {
        return FALSE;
    }

    if (m->action == ACT_WATER_SHELL_SWIMMING && m->heldObj != NULL) {
        return FALSE;
    }

    f32 horizontalSpeed = sqrtf(sqr(m->vel[0]) + sqr(m->vel[2]));
    s32 backwards = m->vel[0] * sins(m->faceAngle[1]) + m->vel[2] * coss(m->faceAngle[1]) < 0.0f;
    u32 action;

    set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
    m->faceAngle[0] = 0;
    m->faceAngle[2] = 0;
    m->marioObj->header.gfx.angle[2] = 0;
    m->marioObj->header.gfx.angle[0] = 0;
    vec3_zero(m->angleVel);
    // Project the actual swim velocity into land movement without turning
    // backward travel around or losing momentum to a stationary idle update.
    if (horizontalSpeed > 0.0f) {
        m->faceAngle[1] = atan2s(m->vel[2], m->vel[0]) + (backwards ? 0x8000 : 0);
    }

    if (m->pos[1] <= m->floorHeight + 4.0f && m->vel[1] <= 0.0f) {
        m->pos[1] = m->floorHeight;
        m->vel[1] = 0.0f;
        if (horizontalSpeed == 0.0f) {
            action = m->heldObj == NULL ? ACT_IDLE : ACT_HOLD_IDLE;
        } else if (m->input & INPUT_NONZERO_ANALOG) {
            action = m->heldObj == NULL ? ACT_WALKING : ACT_HOLD_WALKING;
        } else {
            action = m->heldObj == NULL ? ACT_DECELERATING : ACT_HOLD_DECELERATING;
        }
    } else {
        action = m->heldObj == NULL ? ACT_FREEFALL : ACT_HOLD_FREEFALL;
    }
    set_mario_action(m, action, 0);
    mario_set_forward_vel(m, backwards ? -horizontalSpeed : horizontalSpeed);
    m->slideVelX = m->vel[0];
    m->slideVelZ = m->vel[2];
    return TRUE;
}

static s32 try_swimming_surface_launch(struct MarioState *m) {
    // Only launch after movement actually reaches the surface, not into a ceiling.
    if (sSwimming.surfaceExitVelocityY <= WATER_JUMP_MIN_UPWARD_SPEED || !(m->input & INPUT_A_DOWN)
        || m->pos[1] < m->waterLevel - 80 || m->health < 0x100
        || (!can_control_swimming(m)
            && !(m->action == ACT_WATER_SHELL_SWIMMING && m->heldObj != NULL && m->floor != NULL))) {
        return FALSE;
    }

    mario_stop_underwater_shell(m);
    f32 swimmingVelocityY = sSwimming.surfaceExitVelocityY;
    vec3_zero(m->angleVel);
    set_mario_action(m, m->heldObj == NULL ? ACT_WATER_JUMP : ACT_HOLD_WATER_JUMP, 0);
    mario_set_forward_vel(m, (m->forwardVel + WATER_JUMP_FORWARD_BOOST) * WATER_JUMP_SPEED_SCALE);
    m->vel[1] = (swimmingVelocityY + WATER_JUMP_UPWARD_BOOST) * WATER_JUMP_SPEED_SCALE;

    if (game_options_get(GO_WING_CAP) == 0 && (m->flags & MARIO_WING_CAP) && m->heldObj == NULL) {
        f32 horizontalSpeed = m->forwardVel;
        f32 upwardSpeed = m->vel[1];
        set_mario_action(m, ACT_FLYING, ACT_ARG_FLYING_DEFAULT);
        m->actionState = ACT_STATE_FLYING_FLY;
        set_mario_animation(m, MARIO_ANIM_WING_CAP_FLY);
        m->faceAngle[0] = atan2s(horizontalSpeed, upwardSpeed);
        // Flight stores total speed rather than horizontal speed; preserve the launch vector.
        m->forwardVel = sqrtf(sqr(horizontalSpeed) + sqr(upwardSpeed));
    }
    if (sSwimming.surfaceSplashPlayed) {
        // Surfacing already emitted this ascent's splash; do not restart it at takeoff.
        m->flags |= MARIO_ACTION_SOUND_PLAYED;
    }
    return TRUE;
}

s32 mario_execute_submerged_action(struct MarioState *m) {
    s32 cancel = FALSE;

    sSwimming.surfaceExitVelocityY = 0.0f;

    if (mario_transition_underwater_shell_to_riding(m)) {
        clear_interrupted_swimming_controls(m);
        return TRUE;
    }

    if (transition_cap_swimming_to_land(m)) {
        clear_interrupted_swimming_controls(m);
        return TRUE;
    }

    if (check_common_submerged_cancels(m)) {
        clear_interrupted_swimming_controls(m);
        return TRUE;
    }

    // Only ordinary swimming and water entry accept Z; do not interrupt damage,
    // cutscenes, held-object actions, or Metal Mario's existing controls.
    if ((m->input & INPUT_Z_PRESSED) && !(m->flags & MARIO_METAL_CAP) && m->heldObj == NULL) {
        switch (m->action) {
            case ACT_WATER_IDLE:
            case ACT_WATER_ACTION_END:
            case ACT_BREASTSTROKE:
            case ACT_SWIMMING_END:
            case ACT_FLUTTER_KICK:
            case ACT_WATER_PLUNGE:
                set_mario_action(m, ACT_WATER_DOWN_BURST, 0);
                clear_interrupted_swimming_controls(m);
                return TRUE;
        }
    }

    m->quicksandDepth = 0.0f;

    m->marioBodyState->headAngle[1] = 0;
    m->marioBodyState->headAngle[2] = 0;

    /* clang-format off */
    switch (m->action) {
        case ACT_WATER_IDLE:                 cancel = act_water_idle(m);                 break;
        case ACT_HOLD_WATER_IDLE:            cancel = act_hold_water_idle(m);            break;
        case ACT_WATER_ACTION_END:           cancel = act_water_action_end(m);           break;
        case ACT_HOLD_WATER_ACTION_END:      cancel = act_hold_water_action_end(m);      break;
        case ACT_DROWNING:                   cancel = act_drowning(m);                   break;
        case ACT_BACKWARD_WATER_KB:          cancel = act_backward_water_kb(m);          break;
        case ACT_FORWARD_WATER_KB:           cancel = act_forward_water_kb(m);           break;
        case ACT_WATER_DEATH:                cancel = act_water_death(m);                break;
        case ACT_WATER_SHOCKED:              cancel = act_water_shocked(m);              break;
        case ACT_BREASTSTROKE:               cancel = act_breaststroke(m);               break;
        case ACT_SWIMMING_END:               cancel = act_swimming_end(m);               break;
        case ACT_FLUTTER_KICK:               cancel = act_flutter_kick(m);               break;
        case ACT_HOLD_BREASTSTROKE:          cancel = act_hold_breaststroke(m);          break;
        case ACT_HOLD_SWIMMING_END:          cancel = act_hold_swimming_end(m);          break;
        case ACT_HOLD_FLUTTER_KICK:          cancel = act_hold_flutter_kick(m);          break;
        case ACT_WATER_SHELL_SWIMMING:       cancel = act_water_shell_swimming(m);       break;
        case ACT_WATER_THROW:                cancel = act_water_throw(m);                break;
        case ACT_WATER_PUNCH:                cancel = act_water_punch(m);                break;
        case ACT_WATER_PLUNGE:               cancel = act_water_plunge(m);               break;
        case ACT_WATER_DOWN_BURST:
            cancel = act_water_down_burst(m);
            break;
        case ACT_CAUGHT_IN_WHIRLPOOL:        cancel = act_caught_in_whirlpool(m);        break;
        case ACT_METAL_WATER_STANDING:       cancel = act_metal_water_standing(m);       break;
        case ACT_METAL_WATER_WALKING:        cancel = act_metal_water_walking(m);        break;
        case ACT_METAL_WATER_FALLING:        cancel = act_metal_water_falling(m);        break;
        case ACT_METAL_WATER_FALL_LAND:      cancel = act_metal_water_fall_land(m);      break;
        case ACT_METAL_WATER_JUMP:           cancel = act_metal_water_jump(m);           break;
        case ACT_METAL_WATER_JUMP_LAND:      cancel = act_metal_water_jump_land(m);      break;
        case ACT_HOLD_METAL_WATER_STANDING:  cancel = act_hold_metal_water_standing(m);  break;
        case ACT_HOLD_METAL_WATER_WALKING:   cancel = act_hold_metal_water_walking(m);   break;
        case ACT_HOLD_METAL_WATER_FALLING:   cancel = act_hold_metal_water_falling(m);   break;
        case ACT_HOLD_METAL_WATER_FALL_LAND: cancel = act_hold_metal_water_fall_land(m); break;
        case ACT_HOLD_METAL_WATER_JUMP:      cancel = act_hold_metal_water_jump(m);      break;
        case ACT_HOLD_METAL_WATER_JUMP_LAND: cancel = act_hold_metal_water_jump_land(m); break;
    }
    /* clang-format on */

    if (try_swimming_surface_launch(m)) {
        // Water movement is complete. Start airborne movement on the next update.
        cancel = FALSE;
    }

    clear_interrupted_swimming_controls(m);
    if (!cancel) {
        apply_swimming_roll(m);
    }
    return cancel;
}
