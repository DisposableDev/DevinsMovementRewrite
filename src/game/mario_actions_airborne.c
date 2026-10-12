#include "player_stats.h"
#include <PR/ultratypes.h>

#include "sm64.h"
#include "mario_actions_airborne.h"
#include "mario_actions_automatic.h"
#include "mario_actions_cutscene.h"
#include "mario_actions_moving.h"
#include "mario_actions_stationary.h"
#include "area.h"
#include "audio/external.h"
#include "simpleflips_scream.h"
#include "camera.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game_init.h"
#include "game_options.h"
#include "game_options_runtime.h"
#include "interaction.h"
#include "level_update.h"
#include "liquid_particles.h"
#include "mario.h"
#include "mario_he_thicc.h"
#include "mario_character.h"
#include "mario_step.h"
#include "mario_transition_pose.h"
#include "physics_lab.h"
#include "save_file.h"
#include "rumble_init.h"

#include "config.h"

#define JUMP_KICK_ACTIVE_FRAMES 7
#define JUMP_KICK_FOLLOWUP_FRAMES 10
#define SIDE_FLIP_JUMP_KICK_LOCK_FRAMES 1
#define SLIDE_KICK_AIR_TIMEOUT_FRAMES 40
#define SLIDE_KICK_FALL_ANIM_ACCEL 0x20000
#define SLIDE_KICK_WALL_BONK_GRACE_FRAMES 3
#define SLIDE_KICK_FULL_VISUAL_PITCH_SPEED 40.0f
#define SLIDE_KICK_NORMAL_BOUNCE_FACTOR 0.5f
#define SLIDE_KICK_HELD_A_LIQUID_BOUNCE_FACTOR 0.6f
#define HANDSTAND_FALL_GRACE_FRAMES 8
#define AIR_HIGH_SPEED_DRAG_THRESHOLD 64.0f
#define AIR_HIGH_SPEED_EXTRA_DRAG 1.5f
#define WALL_KICK_DRAG_START_SPEED 40.0f
#define WALL_KICK_BASE_AIR_DRAG 0.65f
#define WALL_KICK_DRAG_ABOVE_40 0.9f
#define WALL_KICK_SOFT_CAP 64.0f
#define WALL_KICK_DRAG_ABOVE_SOFT_CAP 2.0f
#define WALL_KICK_VERTICAL_SPEED 52.0f
#define WALL_KICK_CHAIN_SPEED_LOSS 0.05f
#define WALL_KICK_CHAIN_EXTRA_DRAG 0.01f
#define WALL_KICK_CHAIN_ZERO_SPEED_COUNT 19

enum HandstandFallState { HANDSTAND_FALL_ANIMATING, HANDSTAND_FALL_GRACE, HANDSTAND_FALL_NORMAL };

static s32 start_wall_kick_from_wall(struct MarioState *m);

s32 mario_can_wall_kick_backward_jump(struct MarioState *m) {
    if (m->actionTimer <= BACKWARD_JUMP_WALL_KICK_DELAY_FRAMES) {
        return FALSE;
    }
    if (m->action == ACT_BACKFLIP) {
        // Crouch backflips launch at -16 and slow down during the delay.
        return m->forwardVel < 0.0f;
    }
    return (m->action == ACT_TRIPLE_JUMP || m->action == ACT_SPECIAL_TRIPLE_JUMP)
           && m->actionArg == ACT_ARG_TRIPLE_JUMP_BACKWARD && m->forwardVel < 0.0f;
}

void mario_update_wall_kick_slide_input(struct MarioState *m) {
    f32 forwardStick = m->controller->stickMag * coss(m->intendedYaw - m->faceAngle[1]);
    if (m->wallKickForwardStickFrames > 0) {
        m->wallKickForwardStickFrames--;
    }
    if (m->wallKickBackwardStickFrames > 0) {
        m->wallKickBackwardStickFrames--;
    }
    if (m->input & INPUT_NONZERO_ANALOG) {
        if (forwardStick >= MARIO_STRONG_STICK_THRESHOLD) {
            m->wallKickForwardStickFrames = WALL_KICK_STICK_LOOKBACK_FRAMES + 1;
        }
        if (-forwardStick >= MARIO_STRONG_STICK_THRESHOLD) {
            m->wallKickBackwardStickFrames = WALL_KICK_STICK_LOOKBACK_FRAMES + 1;
        }
    }
}

static s32 mario_has_wall_kick_slide_input(struct MarioState *m) {
    return mario_can_wall_kick_backward_jump(m) ? m->wallKickBackwardStickFrames != 0
                                                : m->wallKickForwardStickFrames != 0;
}

static u32 wall_kick_priority_step_arg(struct MarioState *m, u32 stepArg) {
    if ((stepArg & AIR_STEP_CHECK_LEDGE_GRAB) && m->wallKickInputFrames != 0
        && (m->forwardVel > 10.0f || mario_can_wall_kick_backward_jump(m))
        && mario_has_wall_kick_slide_input(m)) {
        stepArg |= AIR_STEP_CHECK_WALL_KICK;
    }
    return stepArg;
}
static void begin_wall_kick_window(struct MarioState *m) {
    s32 bufferedKick = m->wallKickInputFrames != 0;
    if (mario_can_wall_kick_backward_jump(m)) {
        // Face along the incoming travel so the shared kick and missed-window
        // recoil both point away from the wall. Keep physical velocity intact.
        m->faceAngle[1] += 0x8000;
        m->forwardVel = -m->forwardVel;
    }
    m->wallKickStartForwardVel = m->forwardVel;
    m->wallKickContactNormal[0] = m->wall->normal.x;
    m->wallKickContactNormal[1] = m->wall->normal.y;
    m->wallKickContactNormal[2] = m->wall->normal.z;
    m->wallKickContactYaw = m->wallYaw;
    m->wallKickContactFrame = gGlobalTimer;
    m->wallKickTimer = 0;
    // A wall slide may touch another wall without a new kick. Sound only the
    // first contact of this airborne attempt, then rearm on an actual kick.
    if (!m->wallKickContactSoundPlayed) {
        mario_play_wall_bonk_sound(m);
        m->wallKickContactSoundPlayed = TRUE;
    }
    set_mario_action(m, ACT_AIR_HIT_WALL, 0);
    m->actionTimer = 1; // Contact is the first update of the eight-update window.
    if (mario_try_wall_air_twirl(m)) {
        return;
    }
    if ((m->input & INPUT_A_PRESSED) || bufferedKick) {
        start_wall_kick_from_wall(m);
    } else {
        // The source jump may have selected its pose before the wall step.
        set_mario_animation(m, MARIO_ANIM_START_WALLKICK);
    }
}

static s32 handstand_jump_protects_from_fall(struct MarioState *m) {
    return m->action == ACT_TOP_OF_POLE_JUMP && m->actionState != HANDSTAND_FALL_NORMAL;
}

void play_flip_sounds(struct MarioState *m, s16 frame1, s16 frame2, s16 frame3) {
    s32 animFrame = m->marioObj->header.gfx.animInfo.animFrame;
    if (animFrame == frame1 || animFrame == frame2 || animFrame == frame3) {
        play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
    }
}

static s32 is_large_fall_at_height(struct MarioState *m, f32 landingHeight, struct Surface *floor) {
#ifdef NO_FALL_DAMAGE
    return FALSE;
#endif
    return m->spawnFallProtection == MARIO_SPAWN_FALL_PROTECTION_NONE && floor != NULL
           && floor->type != SURFACE_BURNING && m->action != ACT_TWIRLING
           && !handstand_jump_protects_from_fall(m) && m->vel[1] < -55.0f
           && m->peakHeight - landingHeight
                  > ((m->flags & MARIO_WING_CAP) ? 3000.0f : FALL_DAMAGE_HEIGHT_LARGE);
}

static s32 is_large_fall(struct MarioState *m) {
    return is_large_fall_at_height(m, m->pos[1], m->floor);
}

void play_knockback_sound(struct MarioState *m) {
    if (game_options_damage_voice_muted()) {
        return;
    }
    if (m->actionArg == 0 && (m->forwardVel <= -28.0f || m->forwardVel >= 28.0f)) {
        play_sound_if_no_flag(m, SOUND_MARIO_DOH, MARIO_MARIO_SOUND_PLAYED);
    } else {
        play_sound_if_no_flag(m, SOUND_MARIO_UH, MARIO_MARIO_SOUND_PLAYED);
    }
}

s32 lava_boost_on_wall(struct MarioState *m) {
    s32 noKnockback = mario_blocks_harmful_knockback(m);
    if (noKnockback && m->invincTimer != 0) {
        return FALSE;
    }
    s32 soaked = mario_is_soaked(m);
    if (!noKnockback) {
        m->faceAngle[1] = m->wallYaw;
        if (m->forwardVel < 24.0f) {
            m->forwardVel = 24.0f;
        }
    }

    if (soaked) {
        mario_consume_soaked_status(m);
    }
    if (!(m->flags & MARIO_METAL_CAP) && !soaked) {
        m->hurtCounter += mario_has_cap_for_damage(m) ? 12 : 24;
    }

    play_sound(SOUND_MARIO_ON_FIRE, m->marioObj->header.gfx.cameraToObject);
    update_mario_sound_and_camera(m);
    if (noKnockback) {
        m->invincTimer = MAX(m->invincTimer, 30);
        return FALSE;
    }
    return drop_and_set_mario_action(m, ACT_LAVA_BOOST, 1);
}

// Small falls retain their normal landing behavior; dives, knockback, and real
// Heave-Ho throws lose the cap only when the queued damage is lethal.
static void lose_cap_from_small_fall_damage(struct MarioState *m, s16 capYaw) {
    if (m->heaveHoThrowPendingLanding
        && (m->action == ACT_THROWN_FORWARD || m->action == ACT_THROWN_BACKWARD)) {
        mario_lose_cap_on_lethal_damage(m, capYaw);
        return;
    }

    switch (m->action) {
        case ACT_DIVE:
        case ACT_FORWARD_AIR_KB:
        case ACT_BACKWARD_AIR_KB:
        case ACT_HARD_FORWARD_AIR_KB:
        case ACT_HARD_BACKWARD_AIR_KB:
            mario_lose_cap_on_lethal_damage(m, capYaw);
            break;
    }
}

static void play_fall_damage_impact(struct MarioState *m) {
    // A deliberate pound already emits its shockwave after the landing reaction.
    if (mario_character_palette() == MARIO_PALETTE_HE_THICC && m->action != ACT_GROUND_POUND) {
        mario_ground_pound_impact(m);
    } else {
        set_camera_shake_from_hit(SHAKE_FALL_DAMAGE);
    }
}

enum FallDamageSeverity {
    FALL_DAMAGE_NONE,
    FALL_DAMAGE_SMALL,
    FALL_DAMAGE_LARGE,
};

// Classify the landing before water decides whether damage is applied.
static enum FallDamageSeverity fall_damage_severity_at_height(struct MarioState *m, f32 landingHeight,
                                                              struct Surface *floor) {
#ifdef NO_FALL_DAMAGE
    return FALL_DAMAGE_NONE;
#endif
    if (m->spawnFallProtection != MARIO_SPAWN_FALL_PROTECTION_NONE || floor == NULL
        || floor->type == SURFACE_BURNING || m->action == ACT_TWIRLING
        || handstand_jump_protects_from_fall(m) || !(m->vel[1] < -55.0f)) {
        return FALL_DAMAGE_NONE;
    }
    if (is_large_fall_at_height(m, landingHeight, floor)) {
        return FALL_DAMAGE_LARGE;
    }
    f32 damageHeight = (m->flags & MARIO_WING_CAP) ? 1150.0f : FALL_DAMAGE_HEIGHT_SMALL;
    if (m->peakHeight - landingHeight > damageHeight) {
        return FALL_DAMAGE_SMALL;
    }
    return FALL_DAMAGE_NONE;
}

static enum FallDamageSeverity fall_damage_severity(struct MarioState *m) {
    enum FallDamageSeverity severity = fall_damage_severity_at_height(m, m->pos[1], m->floor);
    if (severity == FALL_DAMAGE_SMALL && mario_floor_is_slippery(m)) {
        return FALL_DAMAGE_NONE;
    }
    return severity;
}

s32 mario_would_take_fall_damage_on_water(struct MarioState *m, f32 waterLevel) {
    if (!game_options_get(GO_FALL_DAMAGE) || !(m->flags & MARIO_WING_CAP)
        || (m->flags & MARIO_METAL_CAP) || (m->controller->buttonDown & A_BUTTON)) {
        return FALSE;
    }
    // Cap water support has ordinary ground friction. Judge impact at the surface,
    // before wet feet suppress damage, without altering Mario's landing state.
    return fall_damage_severity_at_height(m, waterLevel, &gWaterSurfacePseudoFloor) != FALL_DAMAGE_NONE;
}

static void play_wet_fall_impact(struct MarioState *m, enum FallDamageSeverity severity) {
    if (severity == FALL_DAMAGE_LARGE) {
        m->particleFlags |= PARTICLE_WATER_SPLASH;
        stop_sound(SOUND_MARIO_WAAAOOOW, m->marioObj->header.gfx.cameraToObject);
        m->flags &= ~MARIO_FALL_SOUND_PLAYED;
        queue_rumble_data(5, 80);
    } else if (severity == FALL_DAMAGE_SMALL) {
        m->particleFlags |= PARTICLE_SHALLOW_WATER_SPLASH;
        queue_rumble_data(3, 40);
    }
}

s32 check_fall_damage(struct MarioState *m, u32 hardFallAction) {
    if (castle_secret_lobby_active()) {
        return FALSE;
    }

#ifdef NO_FALL_DAMAGE
    return FALSE;
#endif
    if (m->spawnFallProtection != MARIO_SPAWN_FALL_PROTECTION_NONE) {
        return FALSE;
    }

    // A landing below the water surface is wet even if the water is too shallow
    // to enter a swimming action. Recheck at the landing position after movement.
    if (m->pos[1] < find_water_level(m->pos[0], m->pos[2]) - 10.0f) {
        // Metal already prevents the damage independently of the water.
        if (!mario_has_current_metal_cap(m)) {
            play_wet_fall_impact(m, fall_damage_severity(m));
        }
        return FALSE;
    }

    if (mario_has_current_metal_cap(m)) {
        enum FallDamageSeverity severity = fall_damage_severity(m);
        if (severity == FALL_DAMAGE_LARGE) {
            // Keep the damage-free squash, advancing its animation twice per update.
            m->squishTimer = MAX(m->squishTimer, 30);
            m->fastMetalFallSquish = TRUE;
        }
        if (severity != FALL_DAMAGE_NONE
            && !(game_options_get(GO_WING_CAP) == 0 && (m->flags & MARIO_WING_CAP)
                 && (m->controller->buttonDown & A_BUTTON))) {
            queue_rumble_data(5, 80);
            // Deliberate pounds already own their landing sound, particles and impact.
            if (m->action != ACT_GROUND_POUND) {
                if (severity == FALL_DAMAGE_SMALL) {
                    m->fastMetalFallSquish = FALSE;
                }
                play_mario_heavy_landing_sound(m, SOUND_ACTION_TERRAIN_HEAVY_LANDING);
                m->particleFlags |= PARTICLE_MIST_CIRCLE | PARTICLE_HORIZONTAL_STAR;
                mario_ground_pound_impact(m);
                m->metalFallGroundPoundImpact = TRUE;
                m->metalFallGroundPoundFloorObject = m->floor->object;
            }
        } else if (severity == FALL_DAMAGE_LARGE) {
            // Held-A Wing protection keeps the existing heavy Metal squash feedback.
            queue_rumble_data(5, 80);
            set_camera_shake_from_hit(SHAKE_FALL_DAMAGE);
        }
        return FALSE;
    }
    m->fastMetalFallSquish = FALSE;

    if (!game_options_get(GO_FALL_DAMAGE)) {
        return FALSE;
    }

    // Physical held A also protects landings that mask ordinary action input.
    if (game_options_get(GO_WING_CAP) == 0 && (m->flags & MARIO_WING_CAP)
        && (m->controller->buttonDown & A_BUTTON)) {
        return FALSE;
    }

    enum FallDamageSeverity severity = fall_damage_severity(m);
    // Match the landing reaction, even when Mario's sliding velocity points the other way.
    s16 capYaw = m->faceAngle[1];
    if (hardFallAction == ACT_HARD_BACKWARD_GROUND_KB) {
        capYaw = (s16) (capYaw + 0x8000);
    }

    if (severity == FALL_DAMAGE_LARGE) {
        player_stats_add(PS_FALL_DAMAGE_INCIDENTS, 1);
        player_stats_damage();
        m->hurtCounter += mario_has_cap_for_damage(m) ? 16 : 28;
        // Large falls replace the airborne action with this landing reaction.
        if (hardFallAction == ACT_HARD_BACKWARD_GROUND_KB
            || hardFallAction == ACT_HARD_FORWARD_GROUND_KB) {
            mario_lose_cap_on_lethal_damage(m, capYaw);
        }
        queue_rumble_data(5, 80);
        play_fall_damage_impact(m);
        play_sound(SOUND_MARIO_ATTACKED, m->marioObj->header.gfx.cameraToObject);
        if (mario_blocks_harmful_knockback(m) && mario_is_knocked_back(hardFallAction)) {
            m->invincTimer = MAX(m->invincTimer, 30);
            return FALSE;
        }
        return drop_and_set_mario_action(m, hardFallAction, 4);
    } else if (severity == FALL_DAMAGE_SMALL) {
        player_stats_add(PS_FALL_DAMAGE_INCIDENTS, 1);
        player_stats_damage();
        m->hurtCounter += mario_has_cap_for_damage(m) ? 8 : 16;
        lose_cap_from_small_fall_damage(m, capYaw);
        if (!mario_blocks_harmful_knockback(m)) {
            m->squishTimer = 30;
        }
        queue_rumble_data(5, 80);
        play_fall_damage_impact(m);
        play_sound(SOUND_MARIO_ATTACKED, m->marioObj->header.gfx.cameraToObject);
    }

    return FALSE;
}

s32 mario_has_dive_input(struct MarioState *m) {
    s16 stickDYaw = m->intendedYaw - m->faceAngle[1];
    return (m->input & INPUT_NONZERO_ANALOG) && m->controller->stickMag >= MARIO_STRONG_STICK_THRESHOLD
           && m->controller->stickMag * coss(stickDYaw) >= 64.0f * 0.78f;
}

s32 check_kick_or_dive_in_air(struct MarioState *m) {
    if (m->prevAction == ACT_BUTT_SLIDE && (m->action == ACT_JUMP || m->action == ACT_STEEP_JUMP)) {
        if (!(m->actionArg & ACT_ARG_BUTT_SLIDE_JUMP_DIVE_READY)) {
            m->actionArg |= ACT_ARG_BUTT_SLIDE_JUMP_DIVE_READY;
            if ((m->input & INPUT_B_PRESSED) && mario_has_dive_input(m)) {
                m->actionArg |= ACT_ARG_BUTT_SLIDE_JUMP_DIVE_BUFFERED;
                return FALSE;
            }
        } else if (m->actionArg & ACT_ARG_BUTT_SLIDE_JUMP_DIVE_BUFFERED) {
            return set_mario_action(m, ACT_DIVE, 0);
        }
    }
    if (m->input & INPUT_B_PRESSED) {
        s32 dive = mario_has_dive_input(m);
        u32 kickArg = 0;
        if ((m->action == ACT_JUMP || m->action == ACT_HOLD_JUMP)
            && (m->actionArg & ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE)) {
            kickArg = ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE;
            if (m->actionArg & ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL) {
                kickArg |= ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL;
            }
        } else if (m->action == ACT_STEEP_JUMP
                   && (m->actionArg & ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL)) {
            kickArg =
                ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE | ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL;
        } else if (m->action == ACT_JUMP_KICK) {
            kickArg = m->actionArg
                      & (ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE
                         | ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL);
        }
        if (dive) {
            return set_mario_action(m, ACT_DIVE, 0);
        }
        return set_mario_action(m, ACT_JUMP_KICK, kickArg);
    }
    return FALSE;
}

#ifdef NO_GETTING_BURIED
s32 should_get_stuck_in_ground(UNUSED struct MarioState *m) {
    return FALSE;
}
#else
s32 should_get_stuck_in_ground(struct MarioState *m) {
    u32 terrainType = m->area->terrainType & TERRAIN_MASK;
    struct Surface *floor = m->floor;

    // Use held buttons even when the landing action masks ordinary A input.
    if (floor == NULL
        || (game_options_get(GO_WING_CAP) == 0 && (m->flags & MARIO_WING_CAP)
            && (m->controller->buttonDown & A_BUTTON))) {
        return FALSE;
    }

    s32 flags = floor->flags;
    s32 type = floor->type;

    if (terrainType == TERRAIN_SAND && game_options_cheat_active(GO_IGNORE_SAND)) {
        return FALSE;
    }

    if (mario_has_current_vanish_cap(m) && terrainType == TERRAIN_SAND && !SURFACE_IS_QUICKSAND(type)) {
        return FALSE;
    }

    if ((terrainType == TERRAIN_SNOW || terrainType == TERRAIN_SAND) && type != SURFACE_BURNING
        && SURFACE_IS_NOT_HARD(type)) {
        if (!(flags & SURFACE_FLAG_DYNAMIC) && m->peakHeight - m->pos[1] > 1000.0f
            && floor->normal.y >= COS30) {
            return TRUE;
        }
    }

    return FALSE;
}
#endif

void play_far_fall_sound(struct MarioState *m) {
    if ((m->flags & MARIO_FALL_SOUND_PLAYED) || !is_large_fall(m)) {
        return;
    }

    // Only warn in actions whose landing actually checks fall damage.
    switch (m->action) {
        case ACT_JUMP:
        case ACT_PUTTING_ON_CAP_AIR:
        case ACT_DOUBLE_JUMP:
        case ACT_TRIPLE_JUMP:
        case ACT_BACKFLIP:
        case ACT_FREEFALL:
        case ACT_HOLD_JUMP:
        case ACT_HOLD_FREEFALL:
        case ACT_SIDE_FLIP:
        case ACT_WALL_KICK_AIR:
        case ACT_LONG_JUMP:
        case ACT_DIVE:
        case ACT_AIR_THROW:
        case ACT_STEEP_JUMP:
        case ACT_GROUND_POUND:
        case ACT_JUMP_KICK:
        case ACT_FLYING_TRIPLE_JUMP:
        case ACT_TOP_OF_POLE_JUMP:
            break;
        default:
            return;
    }

    // Water entry and soft-ground burial replace the damaging landing.
    if (m->waterLevel - 100.0f > m->floorHeight
        || (should_get_stuck_in_ground(m)
            && (m->action != ACT_DIVE || m->faceAngle[0] == -DEGREES(60)))) {
        return;
    }

    if (!simpleflips_scream_try_play()) {
        play_sound(SOUND_MARIO_WAAAOOOW, m->marioObj->header.gfx.cameraToObject);
    }
    m->flags |= MARIO_FALL_SOUND_PLAYED;
}

s32 check_fall_damage_or_get_stuck(struct MarioState *m, u32 hardFallAction) {
    if (mario_he_thicc_try_belly_landing(m)) {
        return TRUE;
    }
    // A handstand jump keeps its safe landing through its brief recovery.
    if (handstand_jump_protects_from_fall(m)) {
        return FALSE;
    }

    if (should_get_stuck_in_ground(m)) {
        play_sound(SOUND_MARIO_OOOF2, m->marioObj->header.gfx.cameraToObject);
        m->particleFlags |= PARTICLE_MIST_CIRCLE;
        drop_and_set_mario_action(m, ACT_FEET_STUCK_IN_GROUND, 0);
        queue_rumble_data(5, 80);
        return TRUE;
    }

    return check_fall_damage(m, hardFallAction);
}

s32 check_horizontal_wind(struct MarioState *m) {
    struct Surface *floor = m->floor;
    f32 speed;
    s16 pushAngle;

    if (!game_options_cheat_active(GO_IGNORE_WIND) && !mario_has_current_metal_cap(m)
        && floor->type == SURFACE_HORIZONTAL_WIND) {
        pushAngle = floor->force << 8;

        m->slideVelX += 1.2f * sins(pushAngle);
        m->slideVelZ += 1.2f * coss(pushAngle);

        speed = (sqr(m->slideVelX) + sqr(m->slideVelZ));

        if (speed > sqr(48.0f)) {
            speed = sqrtf(speed);
            m->slideVelX = m->slideVelX * 48.0f / speed;
            m->slideVelZ = m->slideVelZ * 48.0f / speed;
            speed = 48.0f;
        } else if (speed > 32.0f) {
            speed = 32.0f;
        }

        m->vel[0] = m->slideVelX;
        m->vel[2] = m->slideVelZ;
        m->slideYaw = atan2s(m->slideVelZ, m->slideVelX);
        m->forwardVel = speed * coss(m->faceAngle[1] - m->slideYaw);
        return TRUE;
    }

    return FALSE;
}

static f32 backward_air_extra_drag(f32 forwardVel, f32 previousForwardVel, f32 backwardInput,
                                   f32 dragScale) {
    f32 backwardSpeed = -forwardVel;
    f32 extraDrag;
    if (forwardVel >= BACKWARD_AIR_VANILLA_DRAG_THRESHOLD) {
        return 0.0f;
    }
    if (backwardSpeed <= BACKWARD_AIR_DRAG_BLEND_START) {
        extraDrag = BACKWARD_AIR_VANILLA_EXTRA_DRAG;
    } else if (forwardVel <= BACKWARD_AIR_DRAG_THRESHOLD) {
        extraDrag = BACKWARD_AIR_EXTRA_DRAG;
    } else {
        extraDrag = BACKWARD_AIR_VANILLA_EXTRA_DRAG
                    + (BACKWARD_AIR_EXTRA_DRAG - BACKWARD_AIR_VANILLA_EXTRA_DRAG)
                          * (backwardSpeed - BACKWARD_AIR_DRAG_BLEND_START)
                          / (-BACKWARD_AIR_DRAG_THRESHOLD - BACKWARD_AIR_DRAG_BLEND_START);
    }
    extraDrag *= dragScale;
    if (backwardInput <= 0.0f) {
        return extraDrag;
    }
    if (previousForwardVel <= BACKWARD_AIR_DRAG_THRESHOLD) {
        // Carried speed above 32 slows normally, without snapping to the limit.
        return forwardVel < BACKWARD_AIR_DRAG_THRESHOLD
                   ? MIN(extraDrag, BACKWARD_AIR_DRAG_THRESHOLD - forwardVel)
                   : extraDrag;
    }

    extraDrag *= 1.0f - backwardInput;
    if (previousForwardVel <= -BACKWARD_AIR_DRAG_BLEND_START) {
        f32 distanceToLimit = previousForwardVel - BACKWARD_AIR_DRAG_THRESHOLD;
        f32 maxGain = BACKWARD_AIR_HELD_GAIN_AT_BLEND_START * distanceToLimit
                      / (-BACKWARD_AIR_DRAG_THRESHOLD - BACKWARD_AIR_DRAG_BLEND_START);
        extraDrag = MAX(extraDrag, previousForwardVel - maxGain - forwardVel);
    }
    return extraDrag;
}

static void apply_backward_butt_slide_jump_drag(struct MarioState *m) {
    s32 marked =
        ((m->action == ACT_JUMP || m->action == ACT_HOLD_JUMP)
         && (m->actionArg & ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE))
        || (m->action == ACT_JUMP_KICK && (m->actionArg & ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE))
        || (m->action == ACT_STEEP_JUMP
            && (m->actionArg & ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL));
    if (marked && m->forwardVel < 0.0f) {
        // Apply after steering so held backward input cannot restore speed
        // after the drag has brought it to rest. Never push through zero.
        m->forwardVel = approach_f32(m->forwardVel, 0.0f, BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG,
                                     BACKWARD_BUTT_SLIDE_JUMP_AIR_DRAG);
    }
}

static s32 backward_butt_slide_jump_blocks_uphill(struct MarioState *m) {
    return ((m->action == ACT_JUMP || m->action == ACT_HOLD_JUMP)
            && (m->actionArg & ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL))
           || (m->action == ACT_JUMP_KICK
               && ((m->actionArg & ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL)
                   || m->forwardVel < 0.0f
                   || m->vel[0] * sins(m->faceAngle[1]) + m->vel[2] * coss(m->faceAngle[1]) < 0.0f))
           || (m->action == ACT_STEEP_JUMP
               && (m->actionArg & ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL));
}

s32 mario_restrict_air_uphill(struct MarioState *m, struct Surface *floor) {
    if (!backward_butt_slide_jump_blocks_uphill(m) || floor == NULL) {
        return FALSE;
    }
    // Classify the candidate with the same terrain rules as the current floor.
    // Restore ownership before any movement or action change.
    struct Surface *savedFloor = m->floor;
    m->floor = floor;
    s32 slope = mario_floor_is_slope(m);
    m->floor = savedFloor;
    if (!slope) {
        return FALSE;
    }
    if (m->action == ACT_JUMP_KICK) {
        // The following butt-slide entry must use impact speed, not the
        // reduced speed left by this contact's uphill braking.
        m->slopeJumpKickImpactSpeed = m->forwardVel;
        // Keep contact braking eligible at low speed and across follow-up kicks.
        m->actionArg |= ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL;
    }

    f32 slopeX = floor->normal.x;
    f32 slopeZ = floor->normal.z;
    f32 slopeLengthSq = sqr(slopeX) + sqr(slopeZ);
    f32 uphillDot = m->vel[0] * slopeX + m->vel[2] * slopeZ;
    if (slopeLengthSq > 0.0001f && uphillDot < 0.0f) {
        // Kicks brake hard once per landing contact instead of snapping to a stop.
        // Other marked slide jumps keep their existing uphill restriction.
        if (m->action == ACT_JUMP_KICK) {
            f32 uphillSpeed = -uphillDot / sqrtf(slopeLengthSq);
            uphillDot *= MIN(1.0f, BACKWARD_JUMP_KICK_UPHILL_DECEL / uphillSpeed);
        }
        // Keep cross-slope/downhill travel and synchronize landing carry.
        m->vel[0] -= uphillDot * slopeX / slopeLengthSq;
        m->vel[2] -= uphillDot * slopeZ / slopeLengthSq;
        m->slideVelX = m->vel[0];
        m->slideVelZ = m->vel[2];
        m->forwardVel = m->vel[0] * sins(m->faceAngle[1]) + m->vel[2] * coss(m->faceAngle[1]);
    }
    return TRUE;
}

static void backward_butt_slide_jump_restrict_uphill(struct MarioState *m) {
    if (m->action != ACT_JUMP_KICK) {
        mario_restrict_air_uphill(m, m->floor);
    }
}

void update_air_with_turn(struct MarioState *m) {
    f32 controlScale = mario_air_twirl_control_scale(m);
    f32 dragThreshold;
    s16 intendedDYaw;
    f32 intendedMag;
    f32 backwardInput = 0.0f;
    f32 previousForwardVel = m->forwardVel;

    if (!check_horizontal_wind(m)) {
        dragThreshold = m->action == ACT_LONG_JUMP ? 48.0f : 32.0f;
        if (controlScale == 1.0f) {
            m->forwardVel = approach_f32(m->forwardVel, 0.0f, 0.35f, 0.35f);
        }

        if (m->input & INPUT_NONZERO_ANALOG) {
            intendedDYaw = m->intendedYaw - m->faceAngle[1];
            intendedMag = m->intendedMag / 32.0f;
            if (controlScale == 1.0f) {
                backwardInput = MIN(1.0f, MAX(0.0f, -coss(intendedDYaw) * intendedMag));
                m->forwardVel += 1.5f * coss(intendedDYaw) * intendedMag;
            }
            m->faceAngle[1] += 512.0f * sins(intendedDYaw) * intendedMag * controlScale;
        }

        // Faster carried speed decays gradually instead of being clamped.
        if (controlScale == 1.0f && m->forwardVel > dragThreshold) {
            m->forwardVel -=
                m->forwardVel > AIR_HIGH_SPEED_DRAG_THRESHOLD ? AIR_HIGH_SPEED_EXTRA_DRAG : 1.0f;
        }
        if (controlScale == 1.0f) {
            m->forwardVel +=
                backward_air_extra_drag(m->forwardVel, previousForwardVel, backwardInput, 1.0f);
        }

        apply_backward_butt_slide_jump_drag(m);

        m->vel[0] = m->slideVelX = m->forwardVel * sins(m->faceAngle[1]);
        m->vel[2] = m->slideVelZ = m->forwardVel * coss(m->faceAngle[1]);
    }
    mario_air_twirl_limit_control_speed(m);
}

void update_air_without_turn(struct MarioState *m) {
    f32 controlScale = mario_air_twirl_control_scale(m);
    f32 sidewaysSpeed = 0.0f;
    f32 dragThreshold;
    f32 momentumRetention = m->action == ACT_JUMP_KICK ? 1.2f : 1.0f;
    s16 intendedDYaw;
    f32 intendedMag;
    f32 backwardInput = 0.0f;
    f32 previousForwardVel = m->forwardVel;

    if (!check_horizontal_wind(m)) {
        dragThreshold = m->action == ACT_LONG_JUMP ? 48.0f : 32.0f;
        if (m->action == ACT_WALL_KICK_AIR) {
            dragThreshold = WALL_KICK_SOFT_CAP;
        }
        f32 airDrag = 0.35f / momentumRetention;
        if (m->action == ACT_DIVE) {
            dragThreshold = FORWARD_DIVE_EXTRA_DRAG_THRESHOLD;
            airDrag = FORWARD_DIVE_BASE_AIR_DRAG;
        } else if (m->action == ACT_SLIDE_KICK) {
            dragThreshold = SLIDE_KICK_EXTRA_DRAG_THRESHOLD;
            airDrag = SLIDE_KICK_BASE_AIR_DRAG;
        } else if (m->action == ACT_WALL_KICK_AIR) {
            airDrag = WALL_KICK_BASE_AIR_DRAG
                      + MAX(0, m->wallKickChainCount - 1) * WALL_KICK_CHAIN_EXTRA_DRAG;
        } else if (m->action == ACT_PUTTING_ON_CAP_AIR && m->actionArg == ACT_ARG_CAP_PICKUP_CAPLESS) {
            airDrag = CAPLESS_CAP_PICKUP_AIR_DRAG;
        }
        // Wing twirl steering preserves momentum without acceleration or passive drag.
        if (controlScale == 1.0f) {
            m->forwardVel = approach_f32(m->forwardVel, 0.0f, airDrag, airDrag);
        }

        if (m->input & INPUT_NONZERO_ANALOG) {
            intendedDYaw = m->intendedYaw - m->faceAngle[1];
            intendedMag = m->intendedMag / 32.0f;
            if (controlScale > 1.0f) {
                // Wing twirl redirects existing momentum instead of adding lateral speed.
                m->faceAngle[1] += 512.0f * sins(intendedDYaw) * intendedMag * controlScale;
            } else {
                backwardInput = MIN(1.0f, MAX(0.0f, -coss(intendedDYaw) * intendedMag));
                f32 airAccel = 1.5f;
                if (m->action == ACT_SLIDE_KICK) {
                    airAccel = SLIDE_KICK_AIR_ACCEL;
                } else if (m->action == ACT_SIDE_FLIP) {
                    airAccel = SIDE_FLIP_AIR_ACCEL;
                }
                m->forwardVel += intendedMag * coss(intendedDYaw) * airAccel;
                sidewaysSpeed = intendedMag * sins(intendedDYaw) * 10.0f;
            }
        }

        // The threshold adds drag; the side flip's separate limit is applied below.
        if (controlScale == 1.0f && m->forwardVel > dragThreshold) {
            f32 extraDrag = 1.0f / momentumRetention;
            if (m->action == ACT_WALL_KICK_AIR) {
                extraDrag = WALL_KICK_DRAG_ABOVE_SOFT_CAP;
            } else if (m->forwardVel > AIR_HIGH_SPEED_DRAG_THRESHOLD) {
                extraDrag = AIR_HIGH_SPEED_EXTRA_DRAG;
            }
            m->forwardVel -= extraDrag;
        }
        if (m->action == ACT_WALL_KICK_AIR && m->forwardVel > WALL_KICK_DRAG_START_SPEED) {
            m->forwardVel -= WALL_KICK_DRAG_ABOVE_40;
        }
        if (controlScale == 1.0f && m->action != ACT_SLIDE_KICK) {
            m->forwardVel += backward_air_extra_drag(m->forwardVel, previousForwardVel, backwardInput,
                                                     1.0f / momentumRetention);
        }
        if (m->action == ACT_SIDE_FLIP && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
            m->forwardVel = MIN(m->forwardVel, SIDE_FLIP_HORIZONTAL_SPEED_CAP);
        }

        apply_backward_butt_slide_jump_drag(m);

        m->slideVelX = m->forwardVel * sins(m->faceAngle[1]);
        m->slideVelZ = m->forwardVel * coss(m->faceAngle[1]);

        m->slideVelX += sidewaysSpeed * sins(m->faceAngle[1] + 0x4000);
        m->slideVelZ += sidewaysSpeed * coss(m->faceAngle[1] + 0x4000);

        if ((m->action == ACT_SLIDE_KICK || m->action == ACT_SIDE_FLIP)
            && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
            f32 speed = sqrtf(sqr(m->slideVelX) + sqr(m->slideVelZ));
            f32 maxSpeed =
                m->action == ACT_SIDE_FLIP ? SIDE_FLIP_HORIZONTAL_SPEED_CAP : absf(m->forwardVel);
            if (speed > maxSpeed) {
                m->slideVelX *= maxSpeed / speed;
                m->slideVelZ *= maxSpeed / speed;
            }
        }

        m->vel[0] = m->slideVelX;
        m->vel[2] = m->slideVelZ;
    }
    mario_air_twirl_limit_control_speed(m);
    backward_butt_slide_jump_restrict_uphill(m);
}

void update_lava_boost_or_twirling(struct MarioState *m) {
    s16 intendedDYaw;
    f32 intendedMag;

    if (m->input & INPUT_NONZERO_ANALOG) {
        intendedDYaw = m->intendedYaw - m->faceAngle[1];
        intendedMag = m->intendedMag / 32.0f;

        m->forwardVel += coss(intendedDYaw) * intendedMag;
        m->faceAngle[1] += sins(intendedDYaw) * intendedMag * 1024.0f;

        if (m->forwardVel < 0.0f) {
            m->faceAngle[1] += 0x8000;
            m->forwardVel *= -1.0f;
        }

        if (m->forwardVel > 32.0f) {
            m->forwardVel -= 2.0f;
        }
    }

    m->vel[0] = m->slideVelX = m->forwardVel * sins(m->faceAngle[1]);
    m->vel[2] = m->slideVelZ = m->forwardVel * coss(m->faceAngle[1]);
}

void update_flying_yaw(struct MarioState *m) {
    s16 targetYawVel =
        -(s16) CLAMP(m->controller->stickX * (m->forwardVel / 4.0f), -32767.0f, 32767.0f);

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
    m->faceAngle[2] = 20 * -m->angleVel[1];
}

void update_flying_pitch(struct MarioState *m) {
    s16 targetPitchVel =
        -(s16) CLAMP(m->controller->stickY * (m->forwardVel / 5.0f), -32767.0f, 32767.0f);

    if (targetPitchVel > 0) {
        if (m->angleVel[0] < 0) {
            m->angleVel[0] += 0x40;
            if (m->angleVel[0] > 0x20) {
                m->angleVel[0] = 0x20;
            }
        } else {
            m->angleVel[0] = approach_s32(m->angleVel[0], targetPitchVel, 0x20, 0x40);
        }
    } else if (targetPitchVel < 0) {
        if (m->angleVel[0] > 0) {
            m->angleVel[0] -= 0x40;
            if (m->angleVel[0] < -0x20) {
                m->angleVel[0] = -0x20;
            }
        } else {
            m->angleVel[0] = approach_s32(m->angleVel[0], targetPitchVel, 0x40, 0x20);
        }
    } else {
        m->angleVel[0] = approach_s32(m->angleVel[0], 0, 0x40, 0x40);
    }
}

// Automatic pitch change in angle units per update, separate from stick steering.
static f32 flying_speed_pitch_change(f32 speed) {
    if (speed <= 4.0f) {
        return -1024.0f;
    }
    if (speed < 16.0f) {
        f32 t = (speed - 4.0f) / 12.0f;
        // Smoothstep eases out of the stall without the old threshold jumps.
        return -1024.0f * (1.0f - t * t * (3.0f - 2.0f * t));
    }
    if (speed <= WING_FLIGHT_CRUISE_SPEED) {
        return 0.0f;
    }
    if (speed < WING_FLIGHT_CRUISE_SPEED + 16.0f) {
        f32 t = (speed - WING_FLIGHT_CRUISE_SPEED) / 16.0f;
        // Ease out of level cruising into the existing six-unit slope.
        return 96.0f * t * t * (2.0f - t);
    }
    return (speed - WING_FLIGHT_CRUISE_SPEED) * 6.0f;
}

void update_flying(struct MarioState *m) {
    update_flying_pitch(m);
    update_flying_yaw(m);

    if (game_options_get(GO_WING_CAP) != 0) {
        // Original flight loses speed while climbing and turning, and stalls
        // toward the ground at low speed. Steering and collision stay shared.
        m->forwardVel -= 2.0f * ((f32) m->faceAngle[0] / 0x4000) + 0.1f;
        m->forwardVel -= 0.5f * (1.0f - coss(m->angleVel[1]));
        m->forwardVel = MAX(m->forwardVel, 0.0f);
        if (m->forwardVel > 16.0f) {
            m->faceAngle[0] =
                CLAMP(m->faceAngle[0] + (m->forwardVel - 32.0f) * 6.0f, -32767.0f, 32767.0f);
        } else if (m->forwardVel > 4.0f) {
            m->faceAngle[0] =
                CLAMP(m->faceAngle[0] + (m->forwardVel - 32.0f) * 10.0f, -32767.0f, 32767.0f);
        } else {
            m->faceAngle[0] -= 0x400;
        }
    } else {
        f32 pitch = CLAMP((f32) m->faceAngle[0] / DEGREES(60), -1.0f, 1.0f);
        f32 targetSpeed = WING_FLIGHT_CRUISE_SPEED;
        if (pitch >= 0.0f) {
            targetSpeed -= pitch * (WING_FLIGHT_CRUISE_SPEED - WING_FLIGHT_MIN_CRUISE_SPEED);
        } else {
            targetSpeed -= pitch * (WING_FLIGHT_MAX_SPEED - WING_FLIGHT_CRUISE_SPEED);
        }
        m->forwardVel = approach_f32(m->forwardVel, targetSpeed, WING_FLIGHT_SPEED_APPROACH,
                                     WING_FLIGHT_SPEED_APPROACH);
        m->forwardVel = MAX(m->forwardVel, 0.0f);
        if (!game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
            m->forwardVel = MIN(m->forwardVel, WING_FLIGHT_MAX_SPEED);
        }
        m->faceAngle[0] =
            CLAMP(m->faceAngle[0] + flying_speed_pitch_change(m->forwardVel), -32767.0f, 32767.0f);
    }

    m->faceAngle[0] = CLAMP((s32) m->faceAngle[0] + m->angleVel[0], -32767, 32767);

    if (m->faceAngle[0] > DEGREES(60)) {
        m->faceAngle[0] = DEGREES(60);
    }
    if (m->faceAngle[0] < -DEGREES(60)) {
        m->faceAngle[0] = -DEGREES(60);
    }

    m->vel[0] = m->forwardVel * coss(m->faceAngle[0]) * sins(m->faceAngle[1]);
    m->vel[1] = m->forwardVel * sins(m->faceAngle[0]);
    m->vel[2] = m->forwardVel * coss(m->faceAngle[0]) * coss(m->faceAngle[1]);

    m->slideVelX = m->vel[0];
    m->slideVelZ = m->vel[2];
}

u32 common_air_action_step(struct MarioState *m, u32 landAction, s32 animation, u32 stepArg) {
    u32 stepResult;
    u32 jumpBuffer = 0;
    u32 ledgeGrabArg;
    f32 remaining;

    update_air_without_turn(m);

    // Gravity after the collision step must not change the grab voice decision.
    ledgeGrabArg = m->vel[1] <= -LEDGE_GRAB_VOICE_MIN_FALL_SPEED ? 0 : ACT_ARG_LEDGE_GRAB_GENTLE;
    stepResult =
        perform_air_step_with_remainder(m, wall_kick_priority_step_arg(m, stepArg), &remaining);
    switch (stepResult) {
        case AIR_STEP_NONE:
            if (animation == MARIO_ANIM_FALL_FROM_SLIDE_KICK) {
                set_mario_anim_with_accel(m, animation, SLIDE_KICK_FALL_ANIM_ACCEL);
            } else {
                set_mario_animation(m, animation);
            }
            break;

        case AIR_STEP_LANDED:
            if (!check_fall_damage_or_get_stuck(m, ACT_HARD_BACKWARD_GROUND_KB)) {
                if ((m->action == ACT_JUMP || m->action == ACT_DOUBLE_JUMP) && m->actionState == 2
                    && m->framesSinceA < MARIO_LANDING_JUMP_BUFFER_FRAMES) {
                    jumpBuffer = MARIO_LANDING_JUMP_BUFFER_FRAMES - m->framesSinceA;
                } else if (m->landingJumpInputFrames != 0) {
                    jumpBuffer = m->landingJumpInputFrames;
                }
                set_mario_action(m, landAction, jumpBuffer);
                // Sound belongs to contact, even if recovery immediately jumps
                // or the remaining movement carries Mario off this floor.
                play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_LANDING);
                // Contact used only part of this frame's movement. Spend the
                // rest on the floor without advancing landing recovery/input.
                // Damage and burial above must see the original contact first.
                if (remaining > 0.0f) {
                    m->vel[1] = 0.0f;
                    if (perform_ground_step_with_fraction(m, remaining) == GROUND_STEP_LEFT_GROUND) {
                        u32 airAction =
                            (landAction == ACT_HOLD_JUMP_LAND || landAction == ACT_HOLD_FREEFALL_LAND)
                                ? ACT_HOLD_FREEFALL
                                : ACT_FREEFALL;
                        set_mario_action(m, airAction, 0);
                        set_mario_animation(m, airAction == ACT_HOLD_FREEFALL
                                                   ? MARIO_ANIM_FALL_WITH_LIGHT_OBJ
                                                   : MARIO_ANIM_GENERAL_FALL);
                    }
                }
            }
            break;

        case AIR_STEP_HIT_WALL:
            set_mario_animation(m, animation);

            if (m->wall == NULL) {
                // A missing floor can block the footprint without a wall impact.
                // Preserve ascent and A-release height control while stopping travel.
                mario_set_forward_vel(m, 0.0f);
            } else if (m->forwardVel > 10.0f || mario_can_wall_kick_backward_jump(m)) {
                if (mario_has_wall_kick_slide_input(m)) {
                    begin_wall_kick_window(m);
                } else {
                    mario_set_forward_vel(m, 0.0f);
                }
            } else {
                mario_set_forward_vel(m, 0.0f);
            }
            break;

        case AIR_STEP_GRABBED_LEDGE:
            set_mario_animation(m, MARIO_ANIM_IDLE_ON_LEDGE);
            set_anim_to_frame(m, 1);
            drop_and_set_mario_action(m, ACT_LEDGE_GRAB, ledgeGrabArg);
            break;

        case AIR_STEP_GRABBED_CEILING:
            set_mario_action(m, ACT_START_HANGING, 0);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    return stepResult;
}

s32 act_jump(struct MarioState *m) {
    if (check_kick_or_dive_in_air(m)) {
        return TRUE;
    }

    if (m->input & INPUT_Z_PRESSED) {
        return set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    // The takeoff A press is not a request for the next jump.
    if (m->actionState == 0) {
        m->actionState = 1;
    } else if (m->input & INPUT_A_PRESSED) {
        m->actionState = 2;
    }

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, 0);
    common_air_action_step(m, ACT_JUMP_LAND, MARIO_ANIM_SINGLE_JUMP,
                           AIR_STEP_CHECK_LEDGE_GRAB | AIR_STEP_CHECK_HANG);
    return FALSE;
}

s32 act_double_jump(struct MarioState *m) {
    s32 animation = (m->vel[1] >= 0.0f)
        ? MARIO_ANIM_DOUBLE_JUMP_RISE
        : MARIO_ANIM_DOUBLE_JUMP_FALL;

    if (check_kick_or_dive_in_air(m)) {
        return TRUE;
    }

    if (m->input & INPUT_Z_PRESSED) {
        return set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    if (m->actionState == 0) {
        m->actionState = 1;
    } else if (m->input & INPUT_A_PRESSED) {
        m->actionState = 2;
    }

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, SOUND_MARIO_HOOHOO);
    common_air_action_step(m, ACT_DOUBLE_JUMP_LAND, animation,
                           AIR_STEP_CHECK_LEDGE_GRAB | AIR_STEP_CHECK_HANG);
    return FALSE;
}

s32 act_triple_jump(struct MarioState *m) {
    if (gSpecialTripleJump) {
        return set_mario_action(m, ACT_SPECIAL_TRIPLE_JUMP, m->actionArg);
    }

    if (check_kick_or_dive_in_air(m)) {
        return TRUE;
    }

    if (m->input & INPUT_Z_PRESSED) {
        return set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, 0);

    s32 backwards = m->actionArg == ACT_ARG_TRIPLE_JUMP_BACKWARD;
    s32 animation = backwards ? MARIO_ANIM_BACKFLIP : MARIO_ANIM_TRIPLE_JUMP;
    if (backwards && m->actionTimer <= BACKWARD_JUMP_WALL_KICK_DELAY_FRAMES) {
        m->actionTimer++;
    }
    common_air_action_step(m, ACT_TRIPLE_JUMP_LAND, animation, 0);
    if (m->action == ACT_TRIPLE_JUMP_LAND) {
        queue_rumble_data(5, 40);
    }
    if (m->action != ACT_AIR_HIT_WALL && m->action != ACT_WALL_KICK_AIR) {
        if (backwards) {
            play_flip_sounds(m, 2, 3, 17);
        } else {
            play_flip_sounds(m, 2, 8, 20);
        }
    }
    return FALSE;
}

s32 act_backflip(struct MarioState *m) {
    if (m->input & INPUT_Z_PRESSED) {
        return set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, SOUND_MARIO_YAH_WAH_HOO);
    if (m->actionTimer <= BACKWARD_JUMP_WALL_KICK_DELAY_FRAMES) {
        m->actionTimer++;
    }
    common_air_action_step(m, ACT_BACKFLIP_LAND, MARIO_ANIM_BACKFLIP, 0);
    if (m->action == ACT_BACKFLIP_LAND) {
        queue_rumble_data(5, 40);
    }
    if (m->action != ACT_AIR_HIT_WALL && m->action != ACT_WALL_KICK_AIR) {
        play_flip_sounds(m, 2, 3, 17);
    }
    return FALSE;
}

s32 act_freefall(struct MarioState *m) {
    s32 animation = MARIO_ANIM_GENERAL_FALL;

    if (check_kick_or_dive_in_air(m)) {
        return TRUE;
    }

    if (m->input & INPUT_Z_PRESSED) {
        return set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    switch (m->actionArg) {
        case ACT_ARG_FREEFALL_GENERAL:
            animation = MARIO_ANIM_GENERAL_FALL;
            break;
        case ACT_ARG_FREEFALL_FROM_SLIDE:
            animation = MARIO_ANIM_FALL_FROM_SLIDE;
            break;
        case ACT_ARG_FREEFALL_FROM_SLIDE_KICK:
            animation = MARIO_ANIM_FALL_FROM_SLIDE_KICK;
            break;
    }

    common_air_action_step(m, ACT_FREEFALL_LAND, animation, AIR_STEP_CHECK_LEDGE_GRAB);
    return FALSE;
}

s32 act_hold_jump(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_FREEFALL, 0);
    }

    if ((m->input & INPUT_B_PRESSED) && !(m->heldObj->oInteractionSubtype & INT_SUBTYPE_HOLDABLE_NPC)) {
        return set_mario_action(m, ACT_AIR_THROW, 0);
    }

    if (m->input & INPUT_Z_PRESSED) {
        return drop_and_set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, 0);
    common_air_action_step(m, ACT_HOLD_JUMP_LAND, MARIO_ANIM_JUMP_WITH_LIGHT_OBJ,
                           AIR_STEP_CHECK_LEDGE_GRAB);
    return FALSE;
}

s32 act_hold_freefall(struct MarioState *m) {
    s32 animation;
    if (m->actionArg == 0) {
        animation = MARIO_ANIM_FALL_WITH_LIGHT_OBJ;
    } else {
        animation = MARIO_ANIM_FALL_FROM_SLIDING_WITH_LIGHT_OBJ;
    }

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_FREEFALL, 0);
    }

    if ((m->input & INPUT_B_PRESSED) && !(m->heldObj->oInteractionSubtype & INT_SUBTYPE_HOLDABLE_NPC)) {
        return set_mario_action(m, ACT_AIR_THROW, 0);
    }

    if (m->input & INPUT_Z_PRESSED) {
        return drop_and_set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    common_air_action_step(m, ACT_HOLD_FREEFALL_LAND, animation, AIR_STEP_CHECK_LEDGE_GRAB);
    return FALSE;
}

s32 act_side_flip(struct MarioState *m) {
    if ((m->actionTimer >= SIDE_FLIP_JUMP_KICK_LOCK_FRAMES || mario_has_dive_input(m))
        && check_kick_or_dive_in_air(m)) {
        m->marioObj->header.gfx.angle[1] += 0x8000;
        return TRUE;
    }

    if (m->input & INPUT_Z_PRESSED) {
        m->marioObj->header.gfx.angle[1] += 0x8000;
        return set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    if (m->actionTimer < SIDE_FLIP_JUMP_KICK_LOCK_FRAMES) {
        // Action-local state remembers a kick until the startup restriction ends.
        if (m->input & INPUT_B_PRESSED) {
            m->actionState = TRUE;
        }
        m->actionTimer++;
    } else if (m->actionState) {
        m->marioObj->header.gfx.angle[1] += 0x8000;
        return set_mario_action(m, ACT_JUMP_KICK, 0);
    }

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, 0);

    if (common_air_action_step(m, ACT_SIDE_FLIP_LAND, MARIO_ANIM_SLIDEFLIP, AIR_STEP_CHECK_LEDGE_GRAB)
        != AIR_STEP_GRABBED_LEDGE) {
        m->marioObj->header.gfx.angle[1] += 0x8000;
    }

    if (m->marioObj->header.gfx.animInfo.animFrame == 6) {
        play_sound(SOUND_ACTION_SIDE_FLIP_UNK, m->marioObj->header.gfx.cameraToObject);
    }
    return FALSE;
}

s32 act_wall_kick_air(struct MarioState *m) {
    if (check_kick_or_dive_in_air(m)) {
        return TRUE;
    }

    if (m->input & INPUT_Z_PRESSED) {
        return set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    play_mario_jump_sound(m);
    common_air_action_step(m, ACT_JUMP_LAND,
                           m->actionArg == ACT_ARG_WALL_KICK_HE_THICC ? MARIO_ANIM_SLIDE
                                                                      : MARIO_ANIM_SLIDEJUMP,
                           AIR_STEP_CHECK_LEDGE_GRAB);
    return FALSE;
}

s32 act_long_jump(struct MarioState *m) {
    s32 animation;
    if (!m->marioObj->oMarioLongJumpIsSlow) {
        animation = MARIO_ANIM_FAST_LONGJUMP;
    } else {
        animation = MARIO_ANIM_SLOW_LONGJUMP;
    }

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, SOUND_MARIO_YAHOO);

    if (m->floor->type == SURFACE_VERTICAL_WIND && m->actionState == 0) {
        play_sound(SOUND_MARIO_HERE_WE_GO, m->marioObj->header.gfx.cameraToObject);
        m->actionState = 1;
    }

    common_air_action_step(m, ACT_LONG_JUMP_LAND, animation, AIR_STEP_CHECK_LEDGE_GRAB);
    if (m->action == ACT_LONG_JUMP_LAND) {
        queue_rumble_data(5, 40);
    }
    return FALSE;
}

s32 act_riding_shell_air(struct MarioState *m) {
    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, 0);
    set_mario_animation(m, MARIO_ANIM_JUMP_RIDING_SHELL);

    update_air_without_turn(m);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_RIDING_SHELL_GROUND, 1);
            break;

        case AIR_STEP_HIT_WALL:
            mario_set_forward_vel(m, 0.0f);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    m->marioObj->header.gfx.pos[1] += 42.0f;
    return FALSE;
}

s32 act_twirling(struct MarioState *m) {
    s16 startTwirlYaw = m->twirlYaw;
    s16 yawVelTarget;

#ifdef Z_TWIRL
    if (m->input & INPUT_Z_DOWN) {
        yawVelTarget = 0x2800;
    } else
#endif

    if (m->input & INPUT_A_DOWN) {
        yawVelTarget = 0x2000;
    } else {
        yawVelTarget = 0x1800;
    }

    m->angleVel[1] = approach_s32_symmetric(m->angleVel[1], yawVelTarget, 0x200);
    m->twirlYaw += m->angleVel[1];

    set_mario_animation(m, m->actionArg == 0 ? MARIO_ANIM_START_TWIRL : MARIO_ANIM_TWIRL);
    if (is_anim_past_end(m)) {
        m->actionArg = 1;
    }

    if (startTwirlYaw > m->twirlYaw) {
        play_sound(SOUND_ACTION_TWIRL, m->marioObj->header.gfx.cameraToObject);
    }

    update_lava_boost_or_twirling(m);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_TWIRL_LAND, 0);
            break;

        case AIR_STEP_HIT_WALL:
            mario_bonk_reflection(m, FALSE);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    m->marioObj->header.gfx.angle[1] += m->twirlYaw;
    reset_rumble_timers_slip();
    return FALSE;
}

s32 act_dive(struct MarioState *m) {
    s32 wallBonkGrace = m->actionState < DIVE_WALL_BONK_GRACE_FRAMES;
    s32 startingDive = m->actionState == 0;

    // Remember a new rollout press shortly before landing, but not the B press that began the dive.
    if (m->actionState == 0) {
        // Rollout contact selected this dive on the previous update without
        // dispatching it. A new press now is a follow-up, not its initiating B.
        if ((m->actionArg & ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT)
            && (m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED))) {
            m->actionTimer = 6;
        }
        m->actionArg &= ~ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT;
        m->actionState = 1;
    } else {
        if (m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED)) {
            m->actionTimer = 6;
        } else if (m->actionTimer > 0) {
            m->actionTimer--;
        }
    }

    if (m->actionArg == 0) {
        play_mario_sound(m, SOUND_ACTION_THROW, SOUND_MARIO_HOOHOO);
    } else {
        play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, 0);
    }

    set_mario_animation(m, MARIO_ANIM_DIVE);
    if (mario_check_object_grab(m)) {
        mario_grab_used_object(m);
        m->marioBodyState->grabPos = GRAB_POS_LIGHT_OBJ;
        if (m->action != ACT_DIVE) {
            return TRUE;
        }
    }

    // Keep the starting frame age through the grab check, including dive frame three.
    if (!startingDive
        && m->actionState < MAX(DIVE_STARTUP_VULNERABLE_FRAMES, DIVE_WALL_BONK_GRACE_FRAMES)) {
        m->actionState++;
    }

    update_air_without_turn(m);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_NONE:
            if (m->vel[1] < 0.0f && m->faceAngle[0] > -DEGREES(60)) {
                m->faceAngle[0] -= 0x200;
                if (m->faceAngle[0] < -DEGREES(60)) {
                    m->faceAngle[0] = -DEGREES(60);
                }
            }
            m->marioObj->header.gfx.angle[0] = -m->faceAngle[0];
            break;

        case AIR_STEP_LANDED:
            if (mario_he_thicc_try_belly_landing(m)) {
                m->faceAngle[0] = 0;
                break;
            }
            if (should_get_stuck_in_ground(m) && m->faceAngle[0] == -DEGREES(60)) {
                queue_rumble_data(5, 80);
                play_sound(SOUND_MARIO_OOOF2, m->marioObj->header.gfx.cameraToObject);
                m->particleFlags |= PARTICLE_MIST_CIRCLE;
                drop_and_set_mario_action(m, ACT_HEAD_STUCK_IN_GROUND, 0);
            } else if (!check_fall_damage(m, ACT_HARD_FORWARD_GROUND_KB)) {
                if (m->heldObj == NULL) {
                    set_mario_action(m, ACT_DIVE_SLIDE, m->actionTimer);
                } else {
                    set_mario_action(m, ACT_DIVE_PICKING_UP, 0);
                }
            }
            m->faceAngle[0] = 0;
            break;

        case AIR_STEP_HIT_WALL:
            // Breakable contact remains active during startup protection.
            if (m->wall != NULL && m->wall->object != NULL
                && (m->wall->object->oInteractType & INTERACT_BREAKABLE)
                && m->wall->object->oIntangibleTimer == 0) {
                m->wall->object->oInteractStatus =
                    INT_STATUS_INTERACTED | INT_STATUS_WAS_ATTACKED | ATTACK_FAST_ATTACK;
            }
            if (wallBonkGrace) {
                // Air stepping still separates Mario from walls. Keep only tangential travel.
                if (m->wall != NULL) {
                    f32 normalX = m->wall->normal.x;
                    f32 normalZ = m->wall->normal.z;
                    f32 normalLengthSquared = normalX * normalX + normalZ * normalZ;
                    f32 inwardSpeed = m->vel[0] * normalX + m->vel[2] * normalZ;
                    if (inwardSpeed < 0.0f && normalLengthSquared > 0.0f) {
                        inwardSpeed /= normalLengthSquared;
                        m->vel[0] -= inwardSpeed * normalX;
                        m->vel[2] -= inwardSpeed * normalZ;
                        m->forwardVel =
                            m->vel[0] * sins(m->faceAngle[1]) + m->vel[2] * coss(m->faceAngle[1]);
                    }
                }
                break;
            }
            mario_bonk_reflection(m, TRUE);
            m->faceAngle[0] = 0;

            if (m->vel[1] > 0.0f) {
                m->vel[1] = 0.0f;
            }

            m->particleFlags |= PARTICLE_VERTICAL_STAR;
            drop_and_set_mario_action(m, ACT_BACKWARD_AIR_KB, 0);
            mario_set_wall_bonk_momentum(m, m->forwardVel, 16.0f);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    return FALSE;
}

s32 act_air_throw(struct MarioState *m) {
    f32 remaining;

    if (++(m->actionTimer) == 4) {
        mario_throw_held_object(m);
    }

    play_sound_if_no_flag(m, SOUND_MARIO_WAH2, MARIO_MARIO_SOUND_PLAYED);
    set_mario_animation(m, MARIO_ANIM_THROW_LIGHT_OBJECT);
    update_air_without_turn(m);

    switch (perform_air_step_with_remainder(m, 0, &remaining)) {
        case AIR_STEP_LANDED:
            if (!check_fall_damage_or_get_stuck(m, ACT_HARD_BACKWARD_GROUND_KB)) {
                // Continue the throw clock and pose across touchdown.
                m->action = ACT_AIR_THROW_LAND;
                if (remaining > 0.0f) {
                    m->vel[1] = 0.0f;
                    if (perform_ground_step_with_fraction(m, remaining) == GROUND_STEP_LEFT_GROUND) {
                        m->action = ACT_AIR_THROW;
                    }
                }
            }
            break;

        case AIR_STEP_HIT_WALL:
            mario_set_forward_vel(m, 0.0f);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    return FALSE;
}

// Use the normal wall-kick entry without changing water-jump momentum in open air.
static void water_jump_hit_wall(struct MarioState *m) {
    // Count airborne updates, starting after the completed surface movement.
    if (m->actionTimer <= WATER_JUMP_WALL_KICK_LOCK_FRAMES) {
        return;
    }

    if (m->wall != NULL && m->forwardVel > 10.0f && mario_has_wall_kick_slide_input(m)) {
        begin_wall_kick_window(m);
        set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
    } else {
        mario_set_forward_vel(m, 15.0f);
    }
}

static void accelerate_steep_water_jump(struct MarioState *m) {
    if (m->actionTimer <= WATER_JUMP_WALL_KICK_LOCK_FRAMES && m->vel[1] > 0.0f
        && m->faceAngle[0] >= WATER_JUMP_RIM_ASSIST_PITCH
        && m->controller->stickY <= WATER_JUMP_RIM_ASSIST_STICK_Y
        && m->forwardVel < WATER_JUMP_RIM_ASSIST_SPEED) {
        mario_set_forward_vel(
            m, MIN(m->forwardVel + WATER_JUMP_RIM_ASSIST_ACCEL, WATER_JUMP_RIM_ASSIST_SPEED));
    }
}

static void update_water_jump_air_control(struct MarioState *m) {
    f32 previousSpeed = m->forwardVel;
    accelerate_steep_water_jump(m);
    if (!(m->input & INPUT_NONZERO_ANALOG)) {
        return;
    }

    s16 intendedDYaw = m->intendedYaw - m->faceAngle[1];
    f32 forwardInput = (m->intendedMag / 32.0f) * coss(intendedDYaw);
    if (forwardInput <= 0.0f) {
        return;
    }

    // The stronger steep-exit assist owns forward acceleration when it applies.
    if (m->forwardVel == previousSpeed) {
        f32 speed = m->forwardVel;
        speed = MIN(speed + WATER_JUMP_AIR_ACCEL * forwardInput, MAX(speed, WATER_JUMP_AIR_SPEED));
        mario_set_forward_vel(m, speed);
    }
}

void finish_water_jump_pose(struct MarioState *m) {
    m->faceAngle[2] = 0;
    m->faceAngle[0] = 0;
    m->marioObj->header.gfx.angle[2] = 0;
    m->marioObj->header.gfx.angle[0] = 0;
    set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
}

s32 act_water_jump(struct MarioState *m) {
    // Emit the exit splash once, even if the jump immediately changes action.
    if (!(m->flags & MARIO_ACTION_SOUND_PLAYED)) {
        m->particleFlags |= PARTICLE_WATER_SPLASH;
    }
    play_mario_sound(m, SOUND_ACTION_WATER_JUMP, 0);

    if (check_kick_or_dive_in_air(m)) {
        if (m->action == ACT_DIVE && m->vel[1] > 0.0f) {
            f32 forwardSpeed =
                MIN(m->vel[1] * WATER_JUMP_DIVE_FORWARD_RATIO, WATER_JUMP_DIVE_FORWARD_CAP);
            if (m->forwardVel < forwardSpeed) {
                mario_set_forward_vel(m, forwardSpeed);
            }
        }
        finish_water_jump_pose(m);
        return TRUE;
    }

    if ((m->input & INPUT_Z_PRESSED) || (m->actionTimer == 0 && (m->input & INPUT_Z_DOWN))) {
        finish_water_jump_pose(m);
        return set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    if (m->actionTimer <= WATER_JUMP_WALL_KICK_LOCK_FRAMES) {
        m->actionTimer++;
    }

    update_water_jump_air_control(m);

    set_mario_animation(m, MARIO_ANIM_SINGLE_JUMP);

    u32 ledgeGrabArg = m->vel[1] <= -LEDGE_GRAB_VOICE_MIN_FALL_SPEED ? 0 : ACT_ARG_LEDGE_GRAB_GENTLE;
    u32 stepArg = AIR_STEP_CHECK_LEDGE_GRAB;
    if (m->actionTimer > WATER_JUMP_WALL_KICK_LOCK_FRAMES) {
        stepArg = wall_kick_priority_step_arg(m, stepArg);
    }
    switch (perform_air_step(m, stepArg)) {
        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_JUMP_LAND, 0);
            finish_water_jump_pose(m);
            break;

        case AIR_STEP_HIT_WALL:
            water_jump_hit_wall(m);
            if (m->action != ACT_WATER_JUMP) {
                finish_water_jump_pose(m);
            }
            break;

        case AIR_STEP_GRABBED_LEDGE:
            set_mario_animation(m, MARIO_ANIM_IDLE_ON_LEDGE);
            set_anim_to_frame(m, 1);
            set_mario_action(m, ACT_LEDGE_GRAB, ledgeGrabArg);
            finish_water_jump_pose(m);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            finish_water_jump_pose(m);
            break;
    }

    return FALSE;
}

s32 act_hold_water_jump(struct MarioState *m) {
    // Emit the exit splash once, even if the jump immediately changes action.
    if (!(m->flags & MARIO_ACTION_SOUND_PLAYED)) {
        m->particleFlags |= PARTICLE_WATER_SPLASH;
    }
    play_mario_sound(m, SOUND_ACTION_WATER_JUMP, 0);

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        finish_water_jump_pose(m);
        return drop_and_set_mario_action(m, ACT_FREEFALL, 0);
    }

    if ((m->input & INPUT_B_PRESSED) && !(m->heldObj->oInteractionSubtype & INT_SUBTYPE_HOLDABLE_NPC)) {
        finish_water_jump_pose(m);
        return set_mario_action(m, ACT_AIR_THROW, 0);
    }

    if ((m->input & INPUT_Z_PRESSED) || (m->actionTimer == 0 && (m->input & INPUT_Z_DOWN))) {
        finish_water_jump_pose(m);
        return drop_and_set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    if (m->actionTimer <= WATER_JUMP_WALL_KICK_LOCK_FRAMES) {
        m->actionTimer++;
    }

    update_water_jump_air_control(m);

    set_mario_animation(m, MARIO_ANIM_JUMP_WITH_LIGHT_OBJ);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_HOLD_JUMP_LAND, 0);
            finish_water_jump_pose(m);
            break;

        case AIR_STEP_HIT_WALL:
            water_jump_hit_wall(m);
            if (m->action != ACT_HOLD_WATER_JUMP) {
                finish_water_jump_pose(m);
            }
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            finish_water_jump_pose(m);
            break;
    }

    return FALSE;
}

s32 act_steep_jump(struct MarioState *m) {
    if (check_kick_or_dive_in_air(m)) {
        return TRUE;
    }

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, 0);
    if (m->actionArg & ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL) {
        m->forwardVel *= 0.98f;
        apply_backward_butt_slide_jump_drag(m);
        mario_set_forward_vel(m, m->forwardVel);
        backward_butt_slide_jump_restrict_uphill(m);
    } else {
        mario_set_forward_vel(m, 0.98f * m->forwardVel);
    }

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            if (!check_fall_damage_or_get_stuck(m, ACT_HARD_BACKWARD_GROUND_KB)) {
                m->faceAngle[0] = 0;
                set_mario_action(m, m->forwardVel < 0.0f ? ACT_BEGIN_SLIDING : ACT_JUMP_LAND, 0);
            }
            break;

        case AIR_STEP_HIT_WALL:
            mario_set_forward_vel(m, 0.0f);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    set_mario_animation(m, MARIO_ANIM_SINGLE_JUMP);
    m->marioObj->header.gfx.angle[1] = m->marioObj->oMarioSteepJumpYaw;
    return FALSE;
}

static void update_ground_pound_horizontal_speed(struct MarioState *m) {
    // Keep Mario's facing fixed while allowing a small sideways drift from the stick.
    f32 steerSpeed = (m->input & INPUT_NONZERO_ANALOG) ? MIN(m->intendedMag * 0.1f, 3.2f) : 0.0f;
    m->forwardVel *= m->forwardVel > 0.0f ? 0.80f : 0.92f;
    mario_set_forward_vel(m, m->forwardVel);
    m->vel[0] += steerSpeed * sins(m->intendedYaw);
    m->vel[2] += steerSpeed * coss(m->intendedYaw);
    m->slideVelX = m->vel[0];
    m->slideVelZ = m->vel[2];
}

s32 act_ground_pound(struct MarioState *m) {
    u32 stepResult;
    f32 yOffset;

    play_sound_if_no_flag(m, SOUND_ACTION_THROW, MARIO_ACTION_SOUND_PLAYED);

    if (m->actionState == 0) {
        // Resolve the moving Whomp against Mario's original position before
        // the spin animation raises him and can skip the contact.
        update_ground_pound_horizontal_speed(m);
        m->vel[1] = 0.0f;
        stepResult = perform_air_step(m, AIR_STEP_CHECK_NONE);

        if (stepResult == AIR_STEP_NONE && m->actionTimer < 10) {
            yOffset = (20 - 2 * m->actionTimer) * mario_get_movement_time_scale(m);
            if (m->pos[1] + yOffset + 160.0f < m->ceilHeight) {
                m->pos[1] += yOffset;
                m->peakHeight = m->pos[1];
                vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
            }
        }

        m->vel[1] =
            mario_character_palette() == MARIO_PALETTE_HE_THICC ? -HE_THICC_POUND_SPEED : -50.0f;

        set_mario_animation(m, m->actionArg == ACT_ARG_GROUND_POUND_NORMAL ? MARIO_ANIM_START_GROUND_POUND
                                                                           : MARIO_ANIM_TRIPLE_JUMP_GROUND_POUND);
        if (m->actionTimer == 0) {
            play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
        }

        m->actionTimer++;
        if (m->actionTimer >= m->marioObj->header.gfx.animInfo.curAnim->loopEnd + 4) {
            play_sound(SOUND_MARIO_GROUND_POUND_WAH, m->marioObj->header.gfx.cameraToObject);
            m->actionState = ACT_STATE_GROUND_POUND_FALL;
        }
    } else {
        set_mario_animation(m, MARIO_ANIM_GROUND_POUND);
        update_ground_pound_horizontal_speed(m);
        if (mario_character_palette() == MARIO_PALETTE_HE_THICC) {
            m->vel[1] = -HE_THICC_POUND_SPEED;
        }
        stepResult = perform_air_step(m, 0);
    }

    // A pound started above water still bursts when Vanish Mario crosses under.
    // Check before the landing action replaces ACT_GROUND_POUND this update.
    if (mario_has_current_vanish_cap(m) && mario_is_head_underwater(m)
        && m->prevPos[1] + 140.0f >= m->waterLevel) {
        m->particleFlags |= PARTICLE_VANISH_WATER_RING;
    }

    if (stepResult == AIR_STEP_LANDED) {
        if (mario_has_current_metal_cap(m) && m->floor != NULL && m->floor->type == SURFACE_BURNING) {
            // A deliberate ground pound keeps the lava bounce without burning Metal.
            play_mario_heavy_landing_sound(m, SOUND_ACTION_TERRAIN_HEAVY_LANDING);
            queue_rumble_data(5, 80);
            set_mario_action(m, ACT_LAVA_BOOST, ACT_ARG_LAVA_BOOST_METAL_GROUND_POUND);
        } else if (should_get_stuck_in_ground(m)) {
            queue_rumble_data(5, 80);
            play_sound(SOUND_MARIO_OOOF2, m->marioObj->header.gfx.cameraToObject);
            m->particleFlags |= PARTICLE_MIST_CIRCLE;
            set_mario_action(m, ACT_BUTT_STUCK_IN_GROUND, 0);
        } else {
            play_mario_heavy_landing_sound(m, SOUND_ACTION_TERRAIN_HEAVY_LANDING);
            if (!check_fall_damage(m, ACT_HARD_BACKWARD_GROUND_KB)) {
                m->particleFlags |= PARTICLE_MIST_CIRCLE | PARTICLE_HORIZONTAL_STAR;
                set_mario_action(m, ACT_GROUND_POUND_LAND, 0);
            }
        }
        mario_ground_pound_impact(m);
        mario_he_thicc_try_pound_bounce(m);
#ifndef DISABLE_GROUNDPOUND_BONK
    } else if (stepResult == AIR_STEP_HIT_WALL) {
        if (m->vel[1] > 0.0f) {
            m->vel[1] = 0.0f;
        }

        m->particleFlags |= PARTICLE_VERTICAL_STAR;
        set_mario_action(m, ACT_BACKWARD_AIR_KB, 0);
        mario_set_wall_bonk_momentum(m, -m->forwardVel, 16.0f);
#endif
    }

    return FALSE;
}

s32 act_burning_jump(struct MarioState *m) {
    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, m->actionArg == 0 ? 0 : -1);
    mario_set_forward_vel(m, m->forwardVel);

    if (perform_air_step(m, 0) == AIR_STEP_LANDED) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
        set_mario_action(m, ACT_BURNING_GROUND, 0);
    }

    set_mario_animation(m, m->actionArg == 0 ? MARIO_ANIM_SINGLE_JUMP : MARIO_ANIM_FIRE_LAVA_BURN);
    m->particleFlags |= PARTICLE_FIRE;
    play_sound(SOUND_MOVING_LAVA_BURN, m->marioObj->header.gfx.cameraToObject);

    m->marioObj->oMarioBurnTimer += 3;

    mario_apply_burning_damage(m, 3);
    if (m->health < 0x100) {
        player_stats_health(m->health, 0xFF);
        m->health = 0xFF;
    }
    reset_rumble_timers_slip();
    return FALSE;
}

s32 act_burning_fall(struct MarioState *m) {
    mario_set_forward_vel(m, m->forwardVel);

    if (perform_air_step(m, 0) == AIR_STEP_LANDED) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
        set_mario_action(m, ACT_BURNING_GROUND, 0);
    }

    set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
    m->particleFlags |= PARTICLE_FIRE;
    m->marioObj->oMarioBurnTimer += 3;

    mario_apply_burning_damage(m, 3);
    if (m->health < 0x100) {
        player_stats_health(m->health, 0xFF);
        m->health = 0xFF;
    }
    reset_rumble_timers_slip();
    return FALSE;
}

s32 act_crazy_box_bounce(struct MarioState *m) {
    f32 minSpeed = 0.0f;

    if (m->actionTimer == 0) {
        switch (m->actionArg) {
            case 0:
                m->vel[1] = 54.0f;
                minSpeed = 32.0f;
                break;

            case 1:
                m->vel[1] = 72.0f;
                minSpeed = 36.0f;
                break;

            case 2:
                m->vel[1] = 120.0f;
                minSpeed = 48.0f;
                break;
        }

        play_sound(minSpeed < 40.0f ? SOUND_GENERAL_CRAZY_BOX_BOING_SLOW : SOUND_GENERAL_CRAZY_BOX_BOING_FAST,
                   m->marioObj->header.gfx.cameraToObject);

        if (m->forwardVel < minSpeed) {
            mario_set_forward_vel(m, minSpeed);
        }

        m->actionTimer = 1;
    }

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, 0);
    set_mario_animation(m, MARIO_ANIM_DIVE);

    update_air_without_turn(m);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            if (m->actionArg < 2) {
                set_mario_action(m, ACT_CRAZY_BOX_BOUNCE, m->actionArg + 1);
            } else {
                m->heldObj->oInteractStatus = INT_STATUS_STOP_RIDING;
                m->heldObj = NULL;
                set_mario_action(m, ACT_STOMACH_SLIDE, 0);
            }
            queue_rumble_data(5, 80);
            m->particleFlags |= PARTICLE_MIST_CIRCLE;
            break;

        case AIR_STEP_HIT_WALL:
            mario_bonk_reflection(m, FALSE);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    // Reversing travel keeps the dive pose facing Mario's gameplay yaw. A
    // signed horizontal speed would instead rotate his body through 180 degrees.
    m->marioObj->header.gfx.angle[0] = atan2s(absf(m->forwardVel), -m->vel[1]);
    return FALSE;
}

u32 common_air_knockback_step(struct MarioState *m, u32 landAction, u32 hardFallAction, s32 animation,
                              f32 speed) {
    u32 stepResult;
    s32 wallBonkMomentum = m->wallBonkMomentum;
    s32 bowserMineLaunch =
        m->action == ACT_HARD_BACKWARD_AIR_KB && (m->actionArg & BOWSER_MINE_LAUNCH_ARG);

    if (wallBonkMomentum || bowserMineLaunch) {
        speed = m->forwardVel;
    }
    mario_set_forward_vel(m, speed);
    set_mario_animation(m, animation);

    stepResult = perform_air_step(m, 0);
    switch (stepResult) {
        case AIR_STEP_LANDED:
            if (m->action != ACT_SOFT_BONK) {
                queue_rumble_data(5, 40);
            }

            if (m->heaveHoThrowPendingLanding
                && (m->action == ACT_THROWN_FORWARD || m->action == ACT_THROWN_BACKWARD)) {
                m->heaveHoThrowRecoveryPending = TRUE;
            }
            if (!check_fall_damage_or_get_stuck(m, hardFallAction)) {
                if (m->action == ACT_THROWN_FORWARD || m->action == ACT_THROWN_BACKWARD) {
                    set_mario_action(m, landAction, m->hurtCounter);
                } else {
                    u32 actionArg = m->actionArg;
                    if (bowserMineLaunch) {
                        actionArg &= ~BOWSER_MINE_LAUNCH_ARG;
                    }
                    if (m->action == ACT_SOFT_BONK) {
                        m->input |= m->bullyKnockbackBuffer & INPUT_B_PRESSED;
                        m->input |= m->bullyKnockbackBuffer & INPUT_A_PRESSED;
                        if (m->bullyKnockbackBuffer & INPUT_A_PRESSED) {
                            actionArg = 6 - m->framesSinceA;
                        }
                    }
                    set_mario_action(m, landAction, actionArg);
                }
                if (landAction == ACT_BACKWARD_GROUND_KB || landAction == ACT_FORWARD_GROUND_KB) {
                    m->wallBonkMomentum = wallBonkMomentum;
                }
            }
            break;

        case AIR_STEP_HIT_WALL:
            set_mario_animation(m, MARIO_ANIM_BACKWARD_AIR_KB);
            mario_bonk_reflection(m, FALSE);

            if (m->vel[1] > 0.0f) {
                m->vel[1] = 0.0f;
            }

            mario_set_forward_vel(m, -speed);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    return stepResult;
}

static s32 start_wall_kick_from_wall(struct MarioState *m) {
    f32 retention = WALL_KICK_MOMENTUM_RETENTION;
    f32 forwardVel;

    if (m->wallKickChainCount >= 1) {
        retention = MAX(0.0f, retention - m->wallKickChainCount * WALL_KICK_CHAIN_SPEED_LOSS);
    }
    forwardVel = MAX(m->wallKickStartForwardVel, WALL_KICK_MIN_FORWARD_SPEED) * retention;
    if (m->wallKickChainCount < WALL_KICK_CHAIN_ZERO_SPEED_COUNT) {
        m->wallKickChainCount++;
    }
    m->wallKickContactSoundPlayed = FALSE;

    // A contact-update kick can skip START_WALLKICK entirely. Blend from the
    // incoming pose in that case too, before changing gameplay facing.
    mario_transition_pose_begin(m, MARIO_ANIM_START_WALLKICK);
    m->faceAngle[1] = m->wallKickContactYaw - (s16) (m->faceAngle[1] - m->wallKickContactYaw) + 0x8000;
    set_mario_action(m, ACT_WALL_KICK_AIR, 0);
    set_mario_animation(m, MARIO_ANIM_SLIDEJUMP);
    queue_rumble_data(5, 40);
    // Pole jumps share this action but retain their normal 62-speed launch.
    set_mario_y_vel_based_on_fspeed(m, WALL_KICK_VERTICAL_SPEED, 0.0f);
    // The generic wall-kick entry has a 28-speed minimum for pole jumps.
    mario_set_forward_vel(m, forwardVel);
    mario_he_thicc_wall_kick(m);
    return TRUE;
}

s32 check_wall_kick(struct MarioState *m) {
    if (mario_try_wall_air_twirl(m)) {
        return TRUE;
    }
    if ((m->input & INPUT_A_PRESSED) && m->wallKickTimer != 0 && m->prevAction == ACT_AIR_HIT_WALL) {
        return start_wall_kick_from_wall(m);
    }

    return FALSE;
}

s32 act_backward_air_kb(struct MarioState *m) {
    if (check_wall_kick(m)) {
        return TRUE;
    }

    play_knockback_sound(m);
    common_air_knockback_step(m, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB, MARIO_ANIM_BACKWARD_AIR_KB, -16.0f);
    return FALSE;
}

s32 act_forward_air_kb(struct MarioState *m) {
    if (check_wall_kick(m)) {
        return TRUE;
    }

    play_knockback_sound(m);
    common_air_knockback_step(m, ACT_FORWARD_GROUND_KB, ACT_HARD_FORWARD_GROUND_KB, MARIO_ANIM_AIR_FORWARD_KB, 16.0f);
    return FALSE;
}

s32 act_hard_backward_air_kb(struct MarioState *m) {
    play_knockback_sound(m);
    common_air_knockback_step(m, ACT_HARD_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB, MARIO_ANIM_BACKWARD_AIR_KB, -16.0f);
    return FALSE;
}

s32 act_hard_forward_air_kb(struct MarioState *m) {
    play_knockback_sound(m);
    common_air_knockback_step(m, ACT_HARD_FORWARD_GROUND_KB, ACT_HARD_FORWARD_GROUND_KB, MARIO_ANIM_AIR_FORWARD_KB, 16.0f);
    return FALSE;
}

s32 act_thrown_backward(struct MarioState *m) {
    u32 landAction;
    if (m->actionArg != 0) {
        landAction = ACT_HARD_BACKWARD_GROUND_KB;
    } else {
        landAction = ACT_BACKWARD_GROUND_KB;
    }

    play_sound_if_no_flag(m, SOUND_MARIO_WAAAOOOW, MARIO_MARIO_SOUND_PLAYED);

    common_air_knockback_step(m, landAction, ACT_HARD_BACKWARD_GROUND_KB, MARIO_ANIM_BACKWARD_AIR_KB, m->forwardVel);

    m->forwardVel *= 0.98f;
    return FALSE;
}

s32 act_thrown_forward(struct MarioState *m) {
    s16 pitch;

    u32 landAction;
    if (m->actionArg != 0) {
        landAction = ACT_HARD_FORWARD_GROUND_KB;
    } else {
        landAction = ACT_FORWARD_GROUND_KB;
    }

    play_sound_if_no_flag(m, SOUND_MARIO_WAAAOOOW, MARIO_MARIO_SOUND_PLAYED);

    if (common_air_knockback_step(m, landAction, ACT_HARD_FORWARD_GROUND_KB, MARIO_ANIM_AIR_FORWARD_KB, m->forwardVel) == AIR_STEP_NONE) {
        pitch = atan2s(m->forwardVel, -m->vel[1]);
        if (pitch > 0x1800) {
            pitch = 0x1800;
        }

        m->marioObj->header.gfx.angle[0] = pitch + 0x1800;
    }

    m->forwardVel *= 0.98f;
    return FALSE;
}

s32 act_soft_bonk(struct MarioState *m) {
    if (check_wall_kick(m)) {
        return TRUE;
    }

    play_knockback_sound(m);

    common_air_knockback_step(m, ACT_FREEFALL_LAND, ACT_HARD_BACKWARD_GROUND_KB, MARIO_ANIM_GENERAL_FALL, m->forwardVel);
    return FALSE;
}

s32 act_getting_blown(struct MarioState *m) {
    if (m->actionState == ACT_STATE_GETTING_BLOWN_ACCEL_BACKWARDS) {
        if (m->forwardVel > -60.0f) {
            m->forwardVel -= 6.0f;
        } else {
            m->actionState = ACT_STATE_GETTING_BLOWN_SLOW_DOWN;
        }
    } else {
        if (m->forwardVel < -16.0f) {
            m->forwardVel += 0.8f;
        }

        if (m->vel[1] < 0.0f && m->windGravity < 4.0f) {
            m->windGravity += 0.05f;
        }
    }
#ifndef PREVENT_CAP_LOSS
    if (++(m->actionTimer) == 20) {
        mario_blow_off_cap_from_wind(m, 50.0f);
    }
#endif

    mario_set_forward_vel(m, m->forwardVel);
    set_mario_animation(m, MARIO_ANIM_BACKWARD_AIR_KB);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_HARD_BACKWARD_AIR_KB, 0);
            break;

        case AIR_STEP_HIT_WALL:
            set_mario_animation(m, MARIO_ANIM_AIR_FORWARD_KB);
            mario_bonk_reflection(m, FALSE);

            if (m->vel[1] > 0.0f) {
                m->vel[1] = 0.0f;
            }

            mario_set_forward_vel(m, -m->forwardVel);
            break;
    }

    return FALSE;
}

s32 act_air_hit_wall(struct MarioState *m) {
    f32 inward, normalLengthSq;
    s32 stepResult;
    s32 expired;

    // An action cancellation can redispatch within the contact update. Its
    // movement and gravity were already spent by the action that hit the wall.
    if (m->wallKickContactFrame == gGlobalTimer) {
        return FALSE;
    }
    if (mario_try_wall_air_twirl(m)) {
        return TRUE;
    }
    m->wallKickContactFrame = gGlobalTimer;

    if (m->heldObj != NULL) {
        mario_drop_held_object(m);
    }

    expired = ++(m->actionTimer) > WALL_KICK_GRACE_FRAMES;
    if (!expired) {
        if (m->input & INPUT_A_PRESSED) {
            return start_wall_kick_from_wall(m);
        }
    }

    update_air_without_turn(m);
    // Preserve stored incoming momentum, while movement stays along the wall.
    normalLengthSq = sqr(m->wallKickContactNormal[0]) + sqr(m->wallKickContactNormal[2]);
    inward = m->vel[0] * m->wallKickContactNormal[0] + m->vel[2] * m->wallKickContactNormal[2];
    if (inward < 0.0f && normalLengthSq > 0.0001f) {
        m->vel[0] -= inward * m->wallKickContactNormal[0] / normalLengthSq;
        m->vel[2] -= inward * m->wallKickContactNormal[2] / normalLengthSq;
    }
    u32 ledgeGrabArg = m->vel[1] <= -LEDGE_GRAB_VOICE_MIN_FALL_SPEED ? 0 : ACT_ARG_LEDGE_GRAB_GENTLE;
    // Destination movement belongs to the next update once this step has been spent.
    stepResult = perform_air_step(m, AIR_STEP_CHECK_LEDGE_GRAB);
    // Display preparation only while this step leaves a live kick window.
    s32 canStillKick =
        !expired && m->wall != NULL && (stepResult == AIR_STEP_NONE || stepResult == AIR_STEP_HIT_WALL);
    set_mario_animation(m, canStillKick ? MARIO_ANIM_START_WALLKICK : MARIO_ANIM_GENERAL_FALL);
    if (stepResult == AIR_STEP_LANDED) {
        set_mario_action(m, ACT_FREEFALL_LAND, 0);
        return FALSE;
    }
    if (stepResult == AIR_STEP_GRABBED_LEDGE) {
        drop_and_set_mario_action(m, ACT_LEDGE_GRAB, ledgeGrabArg);
        return FALSE;
    }
    if (stepResult == AIR_STEP_GRABBED_CEILING) {
        set_mario_action(m, ACT_START_HANGING, 0);
        return FALSE;
    }
    if (stepResult == AIR_STEP_HIT_LAVA_WALL) {
        lava_boost_on_wall(m);
        return FALSE;
    }
    if (m->wall == NULL) {
        // Walking past a wall edge ends contact without a recoil or bonk.
        m->wallKickTimer = 0;
        set_mario_action(m, ACT_FREEFALL, 0);
        return FALSE;
    }
    if (expired) {
        // Only a continuing wall contact can turn an expired kick window into a bonk.
        queue_rumble_data(5, 40);
        m->faceAngle[1] = m->wallKickContactYaw - (s16) (m->faceAngle[1] - m->wallKickContactYaw);
        m->wallKickTimer = 0;
        if (m->vel[1] > 0.0f) {
            m->vel[1] = 0.0f;
        }
        if (m->forwardVel >= 38.0f) {
            m->particleFlags |= PARTICLE_VERTICAL_STAR;
            set_mario_action(m, ACT_BACKWARD_AIR_KB, 0);
            mario_set_wall_bonk_momentum(m, -m->wallKickStartForwardVel, 16.0f);
        } else {
            set_mario_action(m, ACT_SOFT_BONK, 0);
            mario_set_wall_bonk_momentum(m, -m->wallKickStartForwardVel, 8.0f);
        }
        return FALSE;
    }
    return FALSE;
}

static void finish_rollout_landing(struct MarioState *m, s32 starting, u32 actionArg) {
    set_mario_action(m, ACT_FREEFALL_LAND_STOP, actionArg);
    // These geometry flags were sampled before the air step reached this floor.
    m->input &= ~(INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE);
    if (m->pos[1] > m->waterLevel - 40 && mario_floor_is_slippery(m)) {
        m->input |= INPUT_ABOVE_SLIDE;
    }
    if (starting) {
        // A/B that launched the rollout must not trigger a second action.
        m->input &= ~(INPUT_A_PRESSED | INPUT_B_PRESSED);
        m->zBPressed = FALSE;
    }
    m->vel[1] = 0.0f;
    play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
    if (!check_common_stationary_cancels(m)) {
        act_freefall_land_stop(m);
    }
    if (m->action == ACT_WALKING || m->action == ACT_DECELERATING) {
        // Keep the rollout's neutral-stick drag when the stick is released
        // after touchdown as well as when it was already neutral.
        m->actionArg = ACT_ARG_GROUND_SPEED_FROM_ROLLOUT;
    }
    if (m->action == ACT_DIVE) {
        m->actionArg |= ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT;
    }
    // The caller returns FALSE: choose controls now, move the new action next update.
}

s32 act_forward_rollout(struct MarioState *m) {
    s32 starting = m->actionState == 0;
    if (m->actionState == 0) {
        if (!(m->actionArg & ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE)) {
            m->vel[1] = 30.0f;
        }
        m->actionState = 1;
    }

    if (m->rolloutWallKickFrames <= FORWARD_ROLLOUT_WALL_KICK_DELAY_FRAMES) {
        m->rolloutWallKickFrames++;
    }

    if (m->actionTimer > 0) {
        m->actionTimer--;
    }
    // The press that began the rollout cannot queue the next slide kick.
    if (!starting && (m->actionArg & ACT_ARG_FORWARD_ROLLOUT_FROM_SLIDE_KICK) && m->zBPressed
        && m->controller->stickMag >= MARIO_STRONG_STICK_THRESHOLD) {
        m->actionTimer = 6;
    }

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, 0);

    update_air_without_turn(m);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_NONE:
            if (m->actionState == 1) {
                if (set_mario_animation(m, MARIO_ANIM_FORWARD_SPINNING) == 4) {
                    play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
                }
            } else {
                set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
            }
            break;

        case AIR_STEP_LANDED:
            finish_rollout_landing(m, starting,
                                   (m->actionArg & ACT_ARG_FORWARD_ROLLOUT_FROM_SLIDE_KICK)
                                           && m->actionTimer > 0
                                       ? ACT_ARG_FORWARD_ROLLOUT_QUEUED_SLIDE_KICK
                                       : 0);
            return FALSE;

        case AIR_STEP_HIT_WALL:
            if (m->wall != NULL && m->forwardVel > 16.0f) {
                if (m->rolloutWallKickFrames > FORWARD_ROLLOUT_WALL_KICK_DELAY_FRAMES) {
                    if (mario_has_wall_kick_slide_input(m)) {
                        begin_wall_kick_window(m);
                    } else {
                        mario_set_forward_vel(m, 0.0f);
                    }
                } else {
                    queue_rumble_data(5, 40);
                    mario_bonk_reflection(m, FALSE);
                    m->faceAngle[1] += 0x8000;
                    if (m->vel[1] > 0.0f) {
                        m->vel[1] = 0.0f;
                    }
                    m->particleFlags |= PARTICLE_VERTICAL_STAR;
                    set_mario_action(m, ACT_BACKWARD_AIR_KB, 0);
                    mario_set_wall_bonk_momentum(m, -m->forwardVel, 16.0f);
                }
            } else {
                mario_set_forward_vel(m, 0.0f);
            }
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    if (m->actionState == 1 && is_anim_past_end(m)) {
        m->actionState = 2;
    }
    if (m->action == ACT_FORWARD_ROLLOUT && m->rolloutFrames < ROLLOUT_WATER_WALK_DELAY_FRAMES) {
        m->rolloutFrames++;
    }
    return FALSE;
}

s32 act_backward_rollout(struct MarioState *m) {
    s32 starting = m->actionState == 0;
    if (m->actionState == 0) {
        m->vel[1] = 30.0f;
        m->actionState = 1;
    }

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, 0);

    update_air_without_turn(m);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_NONE:
            if (m->actionState == 1) {
                if (set_mario_animation(m, MARIO_ANIM_BACKWARD_SPINNING) == 4) {
                    play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
                }
            } else {
                set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
            }
            break;

        case AIR_STEP_LANDED:
            finish_rollout_landing(m, starting, 0);
            return FALSE;

        case AIR_STEP_HIT_WALL:
            mario_set_forward_vel(m, 0.0f);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    if (m->actionState == 1 && m->marioObj->header.gfx.animInfo.animFrame == 2) {
        m->actionState = 2;
    }
    if (m->action == ACT_BACKWARD_ROLLOUT && m->rolloutFrames < ROLLOUT_WATER_WALK_DELAY_FRAMES) {
        m->rolloutFrames++;
    }
    return FALSE;
}

s32 act_butt_slide_air(struct MarioState *m) {
    if (++(m->actionTimer) > 30 && m->pos[1] - m->floorHeight > 500.0f) {
        return set_mario_action(m, ACT_FREEFALL, 1);
    }

    update_air_with_turn(m);

    switch (perform_air_step(m, AIR_STEP_CHECK_NONE)) {
        case AIR_STEP_LANDED:
            if (m->actionState == 0 && m->vel[1] < 0.0f && m->floor->normal.y >= COS10) {
                m->vel[1] = -m->vel[1] / 2.0f;
                m->actionState = 1;
            } else {
                set_mario_action(m, ACT_BUTT_SLIDE, 0);
            }
            play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
            break;

        case AIR_STEP_HIT_WALL:
            if (m->vel[1] > 0.0f) {
                m->vel[1] = 0.0f;
            }
            m->particleFlags |= PARTICLE_VERTICAL_STAR;
            set_mario_action(m, ACT_BACKWARD_AIR_KB, 0);
            mario_set_wall_bonk_momentum(m, -m->forwardVel, 16.0f);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    set_mario_animation(m, MARIO_ANIM_SLIDE);
    return FALSE;
}

s32 act_hold_butt_slide_air(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_HOLD_FREEFALL, 1);
    }

    if (++m->actionTimer > 30 && m->pos[1] - m->floorHeight > 500.0f) {
        return set_mario_action(m, ACT_HOLD_FREEFALL, 1);
    }

    update_air_with_turn(m);

    switch (perform_air_step(m, AIR_STEP_CHECK_NONE)) {
        case AIR_STEP_LANDED:
            if (m->actionState == ACT_STATE_BUTT_SLIDE_AIR_SMALL_BOUNCE && m->vel[1] < 0.0f && m->floor->normal.y >= COS10) {
                m->vel[1] = -m->vel[1] / 2.0f;
                m->actionState = 1;
            } else {
                set_mario_action(m, ACT_HOLD_BUTT_SLIDE, 0);
            }
            play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
            break;

        case AIR_STEP_HIT_WALL:
            if (m->vel[1] > 0.0f) {
                m->vel[1] = 0.0f;
            }

            mario_drop_held_object(m);
            m->particleFlags |= PARTICLE_VERTICAL_STAR;
            set_mario_action(m, ACT_BACKWARD_AIR_KB, 0);
            mario_set_wall_bonk_momentum(m, -m->forwardVel, 16.0f);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    set_mario_animation(m, MARIO_ANIM_SLIDING_ON_BOTTOM_WITH_LIGHT_OBJ);
    return FALSE;
}

s32 act_lava_boost(struct MarioState *m) {
    if (!(m->flags & MARIO_MARIO_SOUND_PLAYED)) {
        play_sound_if_no_flag(m, SOUND_MARIO_ON_FIRE, MARIO_MARIO_SOUND_PLAYED);
        queue_rumble_data(5, 80);
    }

    if (!(m->input & INPUT_NONZERO_ANALOG)) {
        m->forwardVel = approach_f32(m->forwardVel, 0.0f, 0.35f, 0.35f);
    }

    update_lava_boost_or_twirling(m);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED: {
            s32 soaked = m->floor->type == SURFACE_BURNING
                         && (m->area->terrainType & TERRAIN_MASK) != TERRAIN_SNOW && mario_is_soaked(m);
            if (soaked) {
                mario_consume_soaked_status(m);
            }
            if (m->floor->type == SURFACE_BURNING
                && (!mario_has_current_metal_cap(m) || (m->input & INPUT_A_DOWN))) {
                m->actionState = ACT_STATE_LAVA_BOOST_HIT_LAVA;
                if (!(m->flags & MARIO_METAL_CAP) && !soaked) {
                    m->hurtCounter += mario_has_cap_for_damage(m) ? 12 : 24;
                }
                m->vel[1] = 84.0f;
                play_sound(SOUND_MARIO_ON_FIRE, m->marioObj->header.gfx.cameraToObject);
                queue_rumble_data(5, 80);
            } else {
                play_mario_heavy_landing_sound(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
                if (m->actionState < ACT_STATE_LAVA_BOOST_SET_LANDING_ACTION && m->vel[1] < 0.0f) {
                    m->vel[1] = -m->vel[1] * 0.4f;
                    mario_set_forward_vel(m, m->forwardVel * 0.5f);
                    m->actionState++;
                } else {
                    set_mario_action(m, ACT_LAVA_BOOST_LAND, 0);
                }
            }
            break;
        }

        case AIR_STEP_HIT_WALL:
            mario_bonk_reflection(m, FALSE);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    set_mario_animation(m, MARIO_ANIM_FIRE_LAVA_BURN);
    if ((m->area->terrainType & TERRAIN_MASK) != TERRAIN_SNOW && !(m->flags & MARIO_METAL_CAP)
        && m->vel[1] > 0.0f) {
        m->particleFlags |= PARTICLE_FIRE;
        if (m->actionState == ACT_STATE_LAVA_BOOST_HIT_LAVA) {
            play_sound(SOUND_MOVING_LAVA_BURN, m->marioObj->header.gfx.cameraToObject);
        }
    }

    if (m->health < 0x100) {
        level_trigger_warp(m, WARP_OP_DEATH);
    }

    m->marioBodyState->eyeState = MARIO_EYES_DEAD;
    reset_rumble_timers_slip();
    return FALSE;
}

static void slide_kick_handle_wall(struct MarioState *m, s32 stepResult) {
    if (stepResult == AIR_STEP_HIT_WALL) {
        mario_attack_breakable_box_wall(m);
        if (m->slideKickStartupFrames <= SLIDE_KICK_WALL_BONK_GRACE_FRAMES) {
            return;
        }
        if (m->vel[1] > 0.0f) {
            m->vel[1] = 0.0f;
        }
        m->particleFlags |= PARTICLE_VERTICAL_STAR;
        set_mario_action(m, ACT_BACKWARD_AIR_KB, 0);
        mario_set_wall_bonk_momentum(m, -m->forwardVel, 16.0f);
    } else if (stepResult == AIR_STEP_HIT_LAVA_WALL) {
        lava_boost_on_wall(m);
    }
}

static void slide_kick_continue_bounce(struct MarioState *m, f32 remaining) {
    s32 stepResult = perform_air_step_with_fraction(m, 0, remaining);
    if (stepResult == AIR_STEP_LANDED) {
        set_mario_action(m, ACT_SLIDE_KICK_SLIDE, m->actionArg);
    }
    slide_kick_handle_wall(m, stepResult);
}

s32 act_slide_kick(struct MarioState *m) {
    f32 waterLevel;
    f32 remaining;
    f32 incomingVelY;
    s32 stepResult;
    if (m->actionState == ACT_STATE_SLIDE_KICK_SLIDING && m->actionTimer == 0) {
        m->slideKickStartupFrames = 0;
        play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, SOUND_MARIO_HOOHOO);
        set_mario_animation(m, MARIO_ANIM_SLIDE_KICK);
    } else if (m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED)) {
        m->actionArg = 6;
    } else if (m->actionArg > 0) {
        m->actionArg--;
    }

    // Bounce phases reset actionTimer; only a fresh slide kick renews this grace.
    if (m->slideKickStartupFrames <= SLIDE_KICK_WALL_BONK_GRACE_FRAMES) {
        m->slideKickStartupFrames++;
    }
    if (++(m->actionTimer) > SLIDE_KICK_AIR_TIMEOUT_FRAMES && m->pos[1] - m->floorHeight > 500.0f) {
        return set_mario_action(m, ACT_FREEFALL, ACT_ARG_FREEFALL_FROM_SLIDE_KICK);
    }

    update_air_without_turn(m);
    incomingVelY = m->vel[1];

    s32 checkWater =
        m->actionState != ACT_STATE_SLIDE_KICK_WATER_SKIPPED && !mario_has_land_movement_underwater(m);
    stepResult = perform_slide_kick_air_step(m, &remaining, checkWater, &waterLevel);
    if (waterLevel != FLOOR_LOWER_LIMIT) {
        m->pos[1] = waterLevel;
        m->marioObj->header.gfx.pos[1] = waterLevel;
        m->vel[1] *= -(m->input & INPUT_A_DOWN ? SLIDE_KICK_HELD_A_LIQUID_BOUNCE_FACTOR
                                               : SLIDE_KICK_NORMAL_BOUNCE_FACTOR);
        m->actionState = ACT_STATE_SLIDE_KICK_WATER_SKIPPED;
        m->actionTimer = 0;
        player_stats_add(PS_SLIDE_KICK_LIQUID_BOUNCES, 1);
        s32 coldWater =
            (m->area->terrainType & TERRAIN_MASK) == TERRAIN_SNOW
            || physics_lab_is_cold_pool(gCurrLevelNum, gCurrAreaIndex, m->pos[0], m->pos[2]);
        if (coldWater) {
            m->particleFlags &= ~(PARTICLE_WATER_SPLASH | PARTICLE_SHALLOW_WATER_SPLASH);
            spawn_slide_kick_liquid_particles(m, waterLevel, LIQUID_PARTICLE_COLD_WATER);
        } else {
            m->particleFlags |= PARTICLE_WATER_SPLASH;
        }
        mario_he_thicc_slide_kick_bounce(m);
        play_sound(SOUND_ACTION_WATER_JUMP, m->marioObj->header.gfx.cameraToObject);
        slide_kick_continue_bounce(m, remaining);
        return FALSE;
    }

    switch (stepResult) {
        case AIR_STEP_NONE:
            if (m->actionState == ACT_STATE_SLIDE_KICK_SLIDING) {
                s16 pitch = atan2s(m->forwardVel, -m->vel[1]);
                if (pitch > 0x1800) {
                    pitch = 0x1800;
                }
                // The angle approaches vertical when a slide kick has almost
                // no forward speed. Ease only the model tilt into that pose.
                f32 pitchScale = CLAMP(m->forwardVel / SLIDE_KICK_FULL_VISUAL_PITCH_SPEED, 0.0f, 1.0f);
                m->marioObj->header.gfx.angle[0] = (s16) (pitch * pitchScale);
            }
            break;

        case AIR_STEP_LANDED: {
            s32 lavaBounce = FALSE;
            s32 bounced = FALSE;
            // The step applies gravity after contact. It must not turn an
            // ascending ramp contact into a new downward-speed rebound.
            if (m->actionState == ACT_STATE_SLIDE_KICK_SLIDING && incomingVelY < 0.0f
                && m->vel[1] < 0.0f) {
                f32 bounceFactor = (m->input & INPUT_A_DOWN) && !(m->flags & MARIO_METAL_CAP)
                                           && m->floor != NULL && m->floor->type == SURFACE_BURNING
                                       ? SLIDE_KICK_HELD_A_LIQUID_BOUNCE_FACTOR
                                       : SLIDE_KICK_NORMAL_BOUNCE_FACTOR;
                m->vel[1] *= -bounceFactor;
                m->actionState = ACT_STATE_SLIDE_KICK_END;
                m->actionTimer = 0;
                bounced = TRUE;
                lavaBounce = m->floor != NULL && m->floor->type == SURFACE_BURNING;
                mario_he_thicc_slide_kick_bounce(m);
            } else {
                set_mario_action(m, ACT_SLIDE_KICK_SLIDE, m->actionArg);
            }
            play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
            if (lavaBounce) {
                player_stats_add(PS_SLIDE_KICK_LIQUID_BOUNCES, 1);
                // Keep the landing sound, but avoid a second white liquid splash.
                m->particleFlags &= ~(PARTICLE_WATER_SPLASH | PARTICLE_SHALLOW_WATER_SPLASH);
                spawn_slide_kick_liquid_particles(m, m->floorHeight, LIQUID_PARTICLE_LAVA);
            }
            if (bounced) {
                slide_kick_continue_bounce(m, remaining);
            }
            break;
        }
    }

    slide_kick_handle_wall(m, stepResult);
    return FALSE;
}

s32 mario_jump_kick_can_cancel(struct MarioState *m) {
    // Air-twirl checks run before this update's jump-kick handler.
    return m->actionState == ACT_STATE_JUMP_KICK_RECOVERY
           || m->actionTimer + 1 >= JUMP_KICK_CANCEL_FRAMES;
}

s32 act_jump_kick(struct MarioState *m) {
    s32 canFollowup = mario_jump_kick_can_cancel(m);
    m->actionTimer++;
    // A fresh B expires after three updates; releasing it preserves the request.
    if (m->actionState != ACT_STATE_JUMP_KICK_PLAY_SOUND_AND_ANIM && (m->input & INPUT_B_PRESSED)) {
        m->actionArg |= ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED;
    }
    if (m->framesSinceB >= JUMP_KICK_BUFFER_FRAMES || (m->input & INPUT_Z_PRESSED)) {
        m->actionArg &= ~ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED;
    }

    if (m->actionState == ACT_STATE_JUMP_KICK_PLAY_SOUND_AND_ANIM) {
        // The attack is new even when a buffered B follow-up keeps the same action.
        mario_reset_attack_hit_record();
        play_sound_if_no_flag(m, SOUND_MARIO_PUNCH_HOO, MARIO_ACTION_SOUND_PLAYED);
        m->marioObj->header.gfx.animInfo.animID = -1;
        set_mario_animation(m, MARIO_ANIM_AIR_KICK);
        m->actionState = ACT_STATE_JUMP_KICK_KICKING;
    }

    if (m->actionState == ACT_STATE_JUMP_KICK_KICKING) {
        s32 animFrame = m->marioObj->header.gfx.animInfo.animFrame;
        if (animFrame == 0) {
            m->marioBodyState->punchState = (PUNCH_STATE_TYPE_KICK | 0x6);
        }
        if (animFrame >= 0 && animFrame < JUMP_KICK_ACTIVE_FRAMES) {
            m->flags |= MARIO_KICKING;
        }
        if (is_anim_at_end(m)) {
            m->actionState = ACT_STATE_JUMP_KICK_RECOVERY;
            m->actionTimer = 0;
        }
    } else {
        set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
    }

    if (canFollowup) {
        if (m->input & INPUT_Z_PRESSED) {
            return set_mario_action(m, ACT_GROUND_POUND, 0);
        }
        if (m->actionArg & ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED) {
            s32 dive = mario_has_dive_input(m);
            if (dive) {
                m->actionArg &= ~ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED;
                return set_mario_action(m, ACT_DIVE, 0);
            }
            if (m->actionState == ACT_STATE_JUMP_KICK_RECOVERY
                && m->actionTimer >= JUMP_KICK_FOLLOWUP_FRAMES) {
                return set_mario_action(m, ACT_JUMP_KICK,
                                        m->actionArg
                                            & (ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE
                                               | ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL));
            }
        }
    }

    update_air_without_turn(m);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            if (!check_fall_damage_or_get_stuck(m, ACT_HARD_BACKWARD_GROUND_KB)) {
                set_mario_action(m, ACT_FREEFALL_LAND, 0);
                // A jump can cancel the recovery on the next update. Show the
                // grounded pose now, before a chained double jump replaces it.
                set_mario_animation(m, MARIO_ANIM_GENERAL_LAND);
            }
            break;

        case AIR_STEP_HIT_WALL:
            mario_set_forward_vel(m, 0.0f);
            break;
    }

    return FALSE;
}

s32 act_shot_from_cannon(struct MarioState *m) {
    if (m->area->camera->mode != CAMERA_MODE_BEHIND_MARIO) {
        m->statusForCamera->cameraEvent = CAM_EVENT_SHOT_FROM_CANNON;
    }

    mario_set_forward_vel(m, m->forwardVel);

    play_sound_if_no_flag(m, SOUND_MARIO_YAHOO, MARIO_MARIO_SOUND_PLAYED);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_NONE:
            set_mario_animation(m, MARIO_ANIM_AIRBORNE_ON_STOMACH);
            m->faceAngle[0] = atan2s(m->forwardVel, m->vel[1]);
            m->marioObj->header.gfx.angle[0] = -m->faceAngle[0];
            break;

        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_DIVE_SLIDE, 0);
            m->faceAngle[0] = 0;
            set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
            queue_rumble_data(5, 80);
            break;

        case AIR_STEP_HIT_WALL:
            m->faceAngle[0] = 0;
            if (m->vel[1] > 0.0f) {
                m->vel[1] = 0.0f;
            }

            m->particleFlags |= PARTICLE_VERTICAL_STAR;
            set_mario_action(m, ACT_BACKWARD_AIR_KB, 0);
            mario_set_wall_bonk_momentum(m, -m->forwardVel, 16.0f);
            set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    if (m->action != ACT_SHOT_FROM_CANNON) {
        // Keep impact recovery and its recoil; flight and launch-speed maintenance
        // must not replace the collision response on this same update.
        reset_rumble_timers_slip();
        return FALSE;
    }

    if ((m->flags & MARIO_WING_CAP) && m->vel[1] < 0.0f) {
        set_mario_action(m, ACT_FLYING, 0);
    }

    if ((m->forwardVel -= 0.05f) < 10.0f) {
        mario_set_forward_vel(m, 10.0f);
    }

    if (m->vel[1] > 0.0f) {
        m->particleFlags |= PARTICLE_DUST;
    }
    reset_rumble_timers_slip();
    return FALSE;
}

static s32 cancel_wing_cap_flight(struct MarioState *m) {
    if (m->area->camera->mode == FLYING_CAMERA_MODE) {
        set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
    }

    if (game_options_get(GO_WING_CAP) != 0) {
        return set_mario_action(m, ACT_GROUND_POUND, m->action == ACT_FLYING ? 1 : 0);
    }

    // Freefall uses horizontal speed; flight's forward speed includes its pitch.
    m->forwardVel = sqrtf(sqr(m->vel[0]) + sqr(m->vel[2]));
    m->faceAngle[0] = 0;
    m->faceAngle[2] = 0;
    // The same-frame freefall dispatch must not reuse this press for a pound.
    m->input &= ~INPUT_Z_PRESSED;
    return set_mario_action(m, ACT_FREEFALL, 0);
}

s32 act_flying(struct MarioState *m) {
    s16 startPitch = m->faceAngle[0];

    update_flying_camera_mode(m, !(m->input & INPUT_Z_PRESSED) && (m->flags & MARIO_WING_CAP));

    if (m->input & INPUT_Z_PRESSED) {
        return cancel_wing_cap_flight(m);
    }

    if (!(m->flags & MARIO_WING_CAP)) {
        if (m->area->camera->mode == FLYING_CAMERA_MODE) {
            set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
        }
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->actionState == ACT_STATE_FLYING_SPIN) {
        if (m->actionArg == ACT_ARG_FLYING_FROM_CANNON) {
            set_mario_animation(m, MARIO_ANIM_FLY_FROM_CANNON);
        } else {
            set_mario_animation(m, MARIO_ANIM_FORWARD_SPINNING_FLIP);
            if (m->marioObj->header.gfx.animInfo.animFrame == 1) {
                play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
            }
        }

        if (m->airTwirlFlightRollFrame != 0 || is_anim_at_end(m)) {
            if (m->actionArg == ACT_ARG_FLYING_TOTWC) {
                load_level_init_text(0);
                m->actionArg = ACT_ARG_FLYING_DEFAULT;
            }

            set_mario_animation(m, MARIO_ANIM_WING_CAP_FLY);
            m->actionState = ACT_STATE_FLYING_FLY;
        }
    }

    update_flying(m);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_NONE:
            m->marioObj->header.gfx.angle[0] = -m->faceAngle[0];
            m->marioObj->header.gfx.angle[2] = m->faceAngle[2];
            m->actionTimer = 0;
            break;

        case AIR_STEP_LANDED:
            if ((mario_air_twirl_is_active(m) || m->airTwirlContinue)
                && (m->controller->buttonDown & L_TRIG) && m->quicksandDepth == 0.0f) {
                play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_LANDING);
                mario_start_ground_twirl(m, TRUE);
            } else {
                set_mario_action(m, ACT_DIVE_SLIDE, 0);
                set_mario_animation(m, MARIO_ANIM_DIVE);
                set_anim_to_frame(m, 7);
            }

            m->faceAngle[0] = 0;
            set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
            queue_rumble_data(5, 60);
            break;

        case AIR_STEP_HIT_WALL:
            if (mario_air_twirl_flying_wall_bounce(m)) {
                break;
            }
            if (m->wall != NULL) {
                m->faceAngle[0] = 0;

                if (m->vel[1] > 0.0f) {
                    m->vel[1] = 0.0f;
                }

                mario_play_bonk_sound(m);

                m->particleFlags |= PARTICLE_VERTICAL_STAR;
                set_mario_action(m, ACT_BACKWARD_AIR_KB, 0);
                mario_set_wall_bonk_momentum(m, -m->forwardVel, 16.0f);
                set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
            } else {
                if (m->actionTimer++ == 0) {
                    play_sound(SOUND_ACTION_HIT, m->marioObj->header.gfx.cameraToObject);
                }

                if (m->actionTimer == 30) {
                    m->actionTimer = 0;
                }

                m->faceAngle[0] -= 0x200;
                if (m->faceAngle[0] < -DEGREES(60)) {
                    m->faceAngle[0] = -DEGREES(60);
                }

                m->marioObj->header.gfx.angle[0] = -m->faceAngle[0];
                m->marioObj->header.gfx.angle[2] = m->faceAngle[2];
            }
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    if (m->faceAngle[0] > 0x800 && m->forwardVel >= 48.0f) {
        m->particleFlags |= PARTICLE_DUST;
    }

    if (startPitch <= 0 && m->faceAngle[0] > 0 && m->forwardVel >= 48.0f) {
        play_sound(SOUND_ACTION_FLYING_FAST, m->marioObj->header.gfx.cameraToObject);
        play_sound(SOUND_MARIO_YAHOO_WAHA_YIPPEE + ((gAudioRandom % 5) << 16),
                   m->marioObj->header.gfx.cameraToObject);
        if (mario_character_palette() != MARIO_PALETTE_HE_THICC) {
            queue_rumble_data(50, 40);
        }
    }

    if (m->action == ACT_FLYING) {
        mario_air_twirl_flying_action(m);
    }
    play_sound(SOUND_MOVING_FLYING, m->marioObj->header.gfx.cameraToObject);
    adjust_sound_for_speed(m);
    return FALSE;
}

s32 act_riding_hoot(struct MarioState *m) {
    if (!(m->input & INPUT_A_DOWN) || (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_FROM_HOOT)) {
        m->usedObj->oInteractStatus = 0;
        m->usedObj->oHootMarioReleaseTime = gGlobalTimer;

        play_sound_if_no_flag(m, SOUND_MARIO_UH, MARIO_MARIO_SOUND_PLAYED);
        queue_rumble_data(4, 40);
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    m->pos[0] = m->usedObj->oPosX;
    m->pos[1] = m->usedObj->oPosY - 92.5f;
    m->pos[2] = m->usedObj->oPosZ;

    m->faceAngle[1] = 0x4000 - m->usedObj->oMoveAngleYaw;

    if (m->actionState == ACT_STATE_RIDING_HOOT_GRABBING) {
        set_mario_animation(m, MARIO_ANIM_HANG_ON_CEILING);
        if (is_anim_at_end(m)) {
            set_mario_animation(m, MARIO_ANIM_HANG_ON_OWL);
            m->actionState = ACT_STATE_RIDING_HOOT_HANGING;
        }
    }

    vec3f_set(m->vel, 0.0f, 0.0f, 0.0f);
    vec3f_set(m->marioObj->header.gfx.pos, m->pos[0], m->pos[1], m->pos[2]);
    vec3s_set(m->marioObj->header.gfx.angle, 0, 0x4000 - m->faceAngle[1], 0);
    return FALSE;
}

s32 act_flying_triple_jump(struct MarioState *m) {
    if (game_options_get(GO_WING_CAP) != 0
        && ((m->input & (INPUT_B_PRESSED | INPUT_Z_PRESSED)) || m->zBPressed)) {
        if (m->area->camera->mode == FLYING_CAMERA_MODE) {
            set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
        }
        return set_mario_action(
            m, ((m->input & INPUT_B_PRESSED) || m->zBPressed) ? ACT_DIVE : ACT_GROUND_POUND, 0);
    }
    if (m->input & INPUT_Z_PRESSED) {
        return cancel_wing_cap_flight(m);
    }
    if (check_kick_or_dive_in_air(m)) {
        if (m->area->camera->mode == FLYING_CAMERA_MODE) {
            set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
        }
        return TRUE;
    }

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, SOUND_MARIO_YAHOO);
    if (m->actionState == ACT_STATE_FLYING_TRIPLE_JUMP_START) {
        set_mario_animation(m, MARIO_ANIM_TRIPLE_JUMP_FLY);

        if (m->marioObj->header.gfx.animInfo.animFrame == 7) {
            play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
        }

        if (is_anim_past_end(m)) {
            set_mario_animation(m, MARIO_ANIM_FORWARD_SPINNING);
            queue_rumble_data(8, 80);
            m->actionState = ACT_STATE_FLYING_TRIPLE_JUMP_SPIN;
        }
    }

    if (m->actionState == ACT_STATE_FLYING_TRIPLE_JUMP_SPIN && m->marioObj->header.gfx.animInfo.animFrame == 1) {
        play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
    }

    if (m->vel[1] < 4.0f) {
        if (m->area->camera->mode != FLYING_CAMERA_MODE) {
            set_camera_mode(m->area->camera, FLYING_CAMERA_MODE, 1);
        }

        if (m->forwardVel < 32.0f) {
            mario_set_forward_vel(m, 32.0f);
        }

        set_mario_action(m, ACT_FLYING, 1);
    }

    if (m->actionTimer++ == 10 && m->area->camera->mode != FLYING_CAMERA_MODE) {
        set_camera_mode(m->area->camera, FLYING_CAMERA_MODE, 1);
    }

    update_air_without_turn(m);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            if (!check_fall_damage_or_get_stuck(m, ACT_HARD_BACKWARD_GROUND_KB)) {
                set_mario_action(m, ACT_DOUBLE_JUMP_LAND, 0);
            }
            break;

        case AIR_STEP_HIT_WALL:
            mario_bonk_reflection(m, FALSE);
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            break;
    }

    return FALSE;
}

s32 act_top_of_pole_jump(struct MarioState *m) {
    play_mario_jump_sound(m);
    if (check_kick_or_dive_in_air(m)) {
        return TRUE;
    }
    if (m->input & INPUT_Z_PRESSED) {
        return set_mario_action(m, ACT_GROUND_POUND, 0);
    }
    if (common_air_action_step(m, ACT_FREEFALL_LAND, MARIO_ANIM_HANDSTAND_JUMP,
                               AIR_STEP_CHECK_LEDGE_GRAB)
            == AIR_STEP_LANDED
        && m->action == ACT_FREEFALL_LAND) {
        // Start the landing pose on contact without running recovery or input twice.
        set_mario_animation(m, MARIO_ANIM_GENERAL_LAND);
    }
    if (m->action == ACT_TOP_OF_POLE_JUMP) {
        if (m->actionState == HANDSTAND_FALL_ANIMATING && is_anim_at_end(m)) {
            m->actionState = HANDSTAND_FALL_GRACE;
        } else if (m->actionState == HANDSTAND_FALL_GRACE
                   && ++m->actionTimer >= HANDSTAND_FALL_GRACE_FRAMES) {
            m->actionState = HANDSTAND_FALL_NORMAL;
        }
    }
    return FALSE;
}

s32 act_vertical_wind(struct MarioState *m) {
    s16 intendedDYaw = m->intendedYaw - m->faceAngle[1];
    f32 intendedMag = m->intendedMag / 32.0f;

    play_sound_if_no_flag(m, SOUND_MARIO_HERE_WE_GO, MARIO_MARIO_SOUND_PLAYED);
    if (m->actionState == ACT_STATE_VERTICAL_WIND_SPINNING) {
        set_mario_animation(m, MARIO_ANIM_FORWARD_SPINNING_FLIP);
        if (m->marioObj->header.gfx.animInfo.animFrame == 1) {
            play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
            queue_rumble_data(8, 80);
        }

        if (is_anim_past_end(m)) {
            m->actionState = ACT_STATE_VERTICAL_WIND_AIRBORNE;
        }
    } else {
        set_mario_animation(m, MARIO_ANIM_AIRBORNE_ON_STOMACH);
    }

    update_air_without_turn(m);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_DIVE_SLIDE, 0);
            break;

        case AIR_STEP_HIT_WALL:
            mario_set_forward_vel(m, -16.0f);
            break;
    }

    m->marioObj->header.gfx.angle[0] = (s16)(6144.0f * intendedMag * coss(intendedDYaw));
    m->marioObj->header.gfx.angle[2] = (s16)(-4096.0f * intendedMag * sins(intendedDYaw));
    return FALSE;
}

s32 act_special_triple_jump(struct MarioState *m) {
    if (check_kick_or_dive_in_air(m)) {
        return TRUE;
    }

    if (m->input & INPUT_Z_PRESSED) {
        return set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    play_mario_sound(m, SOUND_ACTION_TERRAIN_JUMP, SOUND_MARIO_YAHOO);

    if (m->actionArg == ACT_ARG_TRIPLE_JUMP_BACKWARD
        && m->actionTimer <= BACKWARD_JUMP_WALL_KICK_DELAY_FRAMES) {
        m->actionTimer++;
    }
    update_air_without_turn(m);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            if (m->actionState++ == ACT_STATE_SPECIAL_TRIPLE_JUMP_SPINNING) {
                m->vel[1] = 42.0f;
            } else {
                set_mario_action(m, ACT_FREEFALL_LAND_STOP, 0);
            }
            play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
            break;

        case AIR_STEP_HIT_WALL:
            if (m->wall != NULL && mario_can_wall_kick_backward_jump(m)
                && mario_has_wall_kick_slide_input(m)) {
                begin_wall_kick_window(m);
                return FALSE;
            }
            mario_bonk_reflection(m, TRUE);
            break;
    }

    if (m->actionState == ACT_STATE_SPECIAL_TRIPLE_JUMP_SPINNING || m->vel[1] > 0.0f) {
        s32 animation = m->actionArg == ACT_ARG_TRIPLE_JUMP_BACKWARD ? MARIO_ANIM_BACKWARD_SPINNING
                                                                     : MARIO_ANIM_FORWARD_SPINNING;
        if (set_mario_animation(m, animation) == 0) {
            play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
        }
    } else {
        set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
    }

    m->particleFlags |= PARTICLE_SPARKLES;
    return FALSE;
}

static void finish_air_cap_pickup(struct MarioState *m) {
    if (m->action == ACT_PUTTING_ON_CAP_AIR && (m->flags & MARIO_CAP_IN_HAND)) {
        cutscene_put_cap_on(m);
    }
}

s32 check_common_airborne_cancels(struct MarioState *m) {
    if (m->pos[1] < m->waterLevel - 100 && !mario_has_land_movement_underwater(m)
        && !mario_is_riding_underwater_shell(m)) {
        finish_air_cap_pickup(m);
        return set_water_plunge_action(m);
    }

    if (m->input & INPUT_SQUISHED) {
        finish_air_cap_pickup(m);
        return drop_and_set_mario_action(m, ACT_SQUISHED, 0);
    }

    if (mario_transition_underwater_shell_to_swimming(m)) {
        return TRUE;
    }

    if (mario_has_current_metal_cap(m)
        && (m->action == ACT_VERTICAL_WIND || m->action == ACT_GETTING_BLOWN
            || (m->action == ACT_LAVA_BOOST && m->actionArg == ACT_ARG_LAVA_BOOST_FLOOR))) {
        m->windCatchActive = FALSE;
        finish_air_cap_pickup(m);
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (mario_is_in_vertical_wind(m) && (m->action & ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION)) {
        return drop_and_set_mario_action(m, ACT_VERTICAL_WIND, 0);
    }

    m->quicksandDepth = 0.0f;
    return FALSE;
}

static s32 act_putting_on_cap_air(struct MarioState *m) {
    if (m->actionArg == ACT_ARG_CAP_PICKUP_CAPLESS) {
        // Keep environmental responses active but ignore controls until the
        // cap animation has finished, including its same-frame exit.
        m->input &= ~(INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_PRESSED | INPUT_A_DOWN | INPUT_B_DOWN
                      | INPUT_Z_DOWN | INPUT_NONZERO_ANALOG | INPUT_FIRST_PERSON);
        m->intendedMag = 0.0f;
        m->intendedYaw = m->faceAngle[1];
    }
    s32 animFrame = set_mario_animation(m, MARIO_ANIM_PUT_CAP_ON);

    if (animFrame == 28 && (m->flags & MARIO_CAP_IN_HAND)) {
        cutscene_put_cap_on(m);
    }

    update_air_without_turn(m);
    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            if (check_fall_damage_or_get_stuck(m, ACT_HARD_BACKWARD_GROUND_KB)) {
                if (m->flags & MARIO_CAP_IN_HAND) {
                    cutscene_put_cap_on(m);
                }
                return FALSE;
            }
            play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_LANDING);
            set_mario_action(m, ACT_PUTTING_ON_CAP, 1);
            return FALSE;

        case AIR_STEP_HIT_WALL:
            if (m->vel[1] > 0.0f) {
                m->vel[1] = 0.0f;
            }
            break;

        case AIR_STEP_HIT_LAVA_WALL:
            finish_air_cap_pickup(m);
            lava_boost_on_wall(m);
            return FALSE;
    }

    if (is_anim_at_end(m)) {
        finish_air_cap_pickup(m);
        set_mario_action(m, m->heldObj != NULL ? ACT_HOLD_FREEFALL : ACT_FREEFALL, 0);
        return FALSE;
    }

    return FALSE;
}

s32 mario_execute_airborne_action(struct MarioState *m) {
    u32 cancel = FALSE;

    if (check_common_airborne_cancels(m)) {
        return TRUE;
    }

    if (mario_he_thicc_try_wavedash(m)) {
        return TRUE;
    }

#ifndef NO_FALL_DAMAGE_SOUND
    play_far_fall_sound(m);
#endif

    /* clang-format off */
    switch (m->action) {
        case ACT_JUMP:                 cancel = act_jump(m);                 break;
        case ACT_HE_THICC_WAVEDASH:
            cancel = mario_he_thicc_wavedash(m);
            break;
        case ACT_PUTTING_ON_CAP_AIR:
            cancel = act_putting_on_cap_air(m);
            break;
        case ACT_DOUBLE_JUMP:          cancel = act_double_jump(m);          break;
        case ACT_FREEFALL:             cancel = act_freefall(m);             break;
        case ACT_HOLD_JUMP:            cancel = act_hold_jump(m);            break;
        case ACT_HOLD_FREEFALL:        cancel = act_hold_freefall(m);        break;
        case ACT_SIDE_FLIP:            cancel = act_side_flip(m);            break;
        case ACT_WALL_KICK_AIR:        cancel = act_wall_kick_air(m);        break;
        case ACT_TWIRLING:             cancel = act_twirling(m);             break;
        case ACT_L_AIR_TWIRL:
            cancel = mario_air_twirl_action(m);
            break;
        case ACT_WATER_JUMP:           cancel = act_water_jump(m);           break;
        case ACT_HOLD_WATER_JUMP:      cancel = act_hold_water_jump(m);      break;
        case ACT_STEEP_JUMP:           cancel = act_steep_jump(m);           break;
        case ACT_BURNING_JUMP:         cancel = act_burning_jump(m);         break;
        case ACT_BURNING_FALL:         cancel = act_burning_fall(m);         break;
        case ACT_TRIPLE_JUMP:          cancel = act_triple_jump(m);          break;
        case ACT_BACKFLIP:             cancel = act_backflip(m);             break;
        case ACT_LONG_JUMP:            cancel = act_long_jump(m);            break;
        case ACT_RIDING_SHELL_JUMP:
        case ACT_RIDING_SHELL_FALL:    cancel = act_riding_shell_air(m);     break;
        case ACT_DIVE:                 cancel = act_dive(m);                 break;
        case ACT_AIR_THROW:            cancel = act_air_throw(m);            break;
        case ACT_BACKWARD_AIR_KB:      cancel = act_backward_air_kb(m);      break;
        case ACT_FORWARD_AIR_KB:       cancel = act_forward_air_kb(m);       break;
        case ACT_HARD_FORWARD_AIR_KB:  cancel = act_hard_forward_air_kb(m);  break;
        case ACT_HARD_BACKWARD_AIR_KB: cancel = act_hard_backward_air_kb(m); break;
        case ACT_SOFT_BONK:            cancel = act_soft_bonk(m);            break;
        case ACT_AIR_HIT_WALL:         cancel = act_air_hit_wall(m);         break;
        case ACT_FORWARD_ROLLOUT:      cancel = act_forward_rollout(m);      break;
        case ACT_SHOT_FROM_CANNON:     cancel = act_shot_from_cannon(m);     break;
        case ACT_BUTT_SLIDE_AIR:       cancel = act_butt_slide_air(m);       break;
        case ACT_HOLD_BUTT_SLIDE_AIR:  cancel = act_hold_butt_slide_air(m);  break;
        case ACT_LAVA_BOOST:           cancel = act_lava_boost(m);           break;
        case ACT_GETTING_BLOWN:        cancel = act_getting_blown(m);        break;
        case ACT_BACKWARD_ROLLOUT:     cancel = act_backward_rollout(m);     break;
        case ACT_CRAZY_BOX_BOUNCE:     cancel = act_crazy_box_bounce(m);     break;
        case ACT_SPECIAL_TRIPLE_JUMP:  cancel = act_special_triple_jump(m);  break;
        case ACT_GROUND_POUND:         cancel = act_ground_pound(m);         break;
        case ACT_THROWN_FORWARD:       cancel = act_thrown_forward(m);       break;
        case ACT_THROWN_BACKWARD:      cancel = act_thrown_backward(m);      break;
        case ACT_FLYING_TRIPLE_JUMP:   cancel = act_flying_triple_jump(m);   break;
        case ACT_SLIDE_KICK:           cancel = act_slide_kick(m);           break;
        case ACT_JUMP_KICK:            cancel = act_jump_kick(m);            break;
        case ACT_FLYING:               cancel = act_flying(m);               break;
        case ACT_RIDING_HOOT:          cancel = act_riding_hoot(m);          break;
        case ACT_TOP_OF_POLE_JUMP:     cancel = act_top_of_pole_jump(m);     break;
        case ACT_VERTICAL_WIND:        cancel = act_vertical_wind(m);        break;
    }
    /* clang-format on */

    return cancel;
}
