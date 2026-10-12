#include "player_stats.h"
#include <PR/ultratypes.h>

#include "sm64.h"
#include "audio/external.h"
#include "engine/math_util.h"
#include "game_options.h"
#include "game_options_runtime.h"
#include "mario.h"
#include "mario_animation_ids.h"
#include "mario_actions_airborne.h"
#include "mario_step.h"
#include "rumble_init.h"

#define AIR_TWIRL_FRAMES 8
#define AIR_TWIRL_INPUT_BUFFER_FRAMES 6
#define AIR_TWIRL_FLIGHT_ROLL_FRAMES 16
#define AIR_TWIRL_WALL_INITIAL_BOOST 8.0f
#define AIR_TWIRL_EARLY_BOUNCE_FRAMES 3
#define AIR_TWIRL_EARLY_BOUNCE_BOOST 9.0f
#define AIR_TWIRL_WALL_SPEED_MULTIPLIER 1.3f
#define AIR_TWIRL_WALL_SPEED_CAP 64.0f
#define AIR_TWIRL_WALL_BOUNCE_SPEED_LOSS 0.06f
#define AIR_TWIRL_WALL_BOUNCE_ZERO_SPEED_COUNT 18
#define AIR_TWIRL_GRAVITY_SCALE 0.02f
#define AIR_TWIRL_FALL_SPEED_SCALE 0.3f
#define AIR_TWIRL_WING_GRAVITY_SCALE 0.01f
#define AIR_TWIRL_WING_UPWARD_BOOST 20.0f
#define AIR_TWIRL_WING_CONTROL_FRAMES 30
#define AIR_TWIRL_WING_CONTROL_SCALE 4.0f

static void mario_air_twirl_reset_allowances(struct MarioState *m) {
    m->airTwirlUsed = FALSE;
    m->airTwirlWingResetUsed = FALSE;
    m->airTwirlGravity = FALSE;
    m->airTwirlContinue = FALSE;
    m->airTwirlWallContact = FALSE;
    m->airTwirlWallBounceCount = 0;
    m->airTwirlWingControlTimer = 0;
    m->airTwirlWingSpeedLimit = 0.0f;
    m->airTwirlSourceAction = ACT_FREEFALL;
    m->airTwirlStartAngle = 0;
    m->airTwirlWallYaw = 0;
    m->airTwirlFlightRollFrame = 0;
}

void mario_air_twirl_init(struct MarioState *m) {
    mario_air_twirl_reset_allowances(m);
    m->airTwirlCooldown = 0;
    m->airTwirlCooldownResetByCoin = FALSE;
    m->wallTwirlSoundTimer = 0;
    m->airTwirlJumpQueued = FALSE;
    m->airTwirlLPressed = FALSE;
    m->airTwirlInputFrames = 0;
}

void mario_air_twirl_reset_on_coin(struct MarioState *m) {
    m->airTwirlUsed = FALSE;
    m->airTwirlWingResetUsed = FALSE;
    m->airTwirlCooldown = 0;
    // A coin collected mid-spin also waives the cooldown when that spin finishes.
    m->airTwirlCooldownResetByCoin = TRUE;
}

s32 mario_air_twirl_is_active(struct MarioState *m) {
    return game_options_get(GO_TWIRLS)
           && (m->action == ACT_L_AIR_TWIRL
               || (m->action == ACT_FLYING && m->airTwirlFlightRollFrame != 0
                   && (m->flags & MARIO_WING_CAP)));
}

void mario_air_twirl_transition(struct MarioState *m, u32 action) {
    if (m->action == ACT_L_AIR_TWIRL && action != ACT_L_AIR_TWIRL) {
        m->marioObj->header.gfx.angle[0] = 0;
        m->marioObj->header.gfx.angle[1] = m->faceAngle[1];
        m->marioObj->header.gfx.angle[2] = 0;
        m->airTwirlGravity = FALSE;
        m->airTwirlWallContact = FALSE;
        if (m->airTwirlUsed) {
            m->airTwirlCooldown = m->airTwirlCooldownResetByCoin ? 0 : MARIO_TWIRL_COOLDOWN_FRAMES;
        }
    }
    if (m->action == ACT_FLYING && action != ACT_FLYING) {
        if (m->airTwirlFlightRollFrame != 0 && m->airTwirlUsed) {
            m->airTwirlCooldown = m->airTwirlCooldownResetByCoin ? 0 : MARIO_TWIRL_COOLDOWN_FRAMES;
        }
        m->airTwirlFlightRollFrame = 0;
        m->airTwirlContinue = FALSE;
        m->airTwirlWallContact = FALSE;
    }
    if (action != ACT_L_AIR_TWIRL && action != ACT_GROUND_TWIRL && action != ACT_FREEFALL
        && action != ACT_FREEFALL_LAND && action != ACT_FLYING) {
        m->airTwirlContinue = FALSE;
    }
    if ((action & ACT_GROUP_MASK) == ACT_GROUP_SUBMERGED) {
        mario_air_twirl_reset_allowances(m);
    }
}

static s32 mario_air_twirl_bounce_off_wall(struct MarioState *m);

s32 mario_start_air_twirl(struct MarioState *m, s32 continuation) {
    if (!game_options_get(GO_TWIRLS)) {
        return FALSE;
    }
    u32 sourceAction = m->action;
    f32 peakHeight = m->peakHeight;
    s16 visualYaw = m->marioObj->header.gfx.angle[1] - m->faceAngle[1];
    u32 jumping = m->flags & MARIO_JUMPING;

    if (!continuation) {
        player_stats_twirl_start(TRUE, continuation);
        mario_twirl_begin_contacts();
    }
    if (sourceAction == ACT_WATER_JUMP) {
        finish_water_jump_pose(m);
    }
    m->airTwirlInputFrames = 0;
    mario_ground_twirl_cancel_queued_input();
    if (sourceAction == ACT_FLYING) {
        // Flight keeps its action, movement and camera; only its displayed bank rolls.
        m->airTwirlFlightRollFrame = 1;
    } else {
        set_mario_action(m, ACT_L_AIR_TWIRL, 0);
        // Ordinary air entry must not erase fall height or relaunch a jump.
        m->peakHeight = peakHeight;
        m->flags = (m->flags & ~MARIO_JUMPING) | jumping;
    }
    m->airTwirlSourceAction = sourceAction == ACT_GROUND_TWIRL ? ACT_FREEFALL : sourceAction;
    if (!m->airTwirlUsed && sourceAction != ACT_FLYING && (m->flags & MARIO_WING_CAP)) {
        m->airTwirlWingControlTimer = AIR_TWIRL_WING_CONTROL_FRAMES;
        m->airTwirlWingSpeedLimit = sqrtf(sqr(m->vel[0]) + sqr(m->vel[2]));
    }
    m->airTwirlCooldownResetByCoin = FALSE;
    m->airTwirlGravity = !m->airTwirlUsed;
    m->airTwirlUsed = TRUE;
    m->airTwirlContinue = (m->controller->buttonDown & L_TRIG) != 0;
    m->airTwirlWallContact = FALSE;
    m->airTwirlWallBounceCount = 0;
    m->airTwirlStartAngle = continuation ? visualYaw : 0;
    m->wallKickTimer = 0;
    m->wallKickInputFrames = 0;
    m->wallKickFromDoubleJump = FALSE;
    if (sourceAction != ACT_FLYING && (m->flags & MARIO_WING_CAP) && !m->airTwirlWingResetUsed) {
        if (m->vel[1] < 0.0f) {
            m->vel[1] = 0.0f;
            m->peakHeight = m->pos[1];
        }
        m->vel[1] += AIR_TWIRL_WING_UPWARD_BOOST;
        m->airTwirlWingResetUsed = TRUE;
    } else if (sourceAction != ACT_FLYING && m->airTwirlGravity && m->vel[1] < 0.0f) {
        m->vel[1] *= AIR_TWIRL_FALL_SPEED_SCALE;
        // Fall damage must measure the new fall after this midair braking.
        m->peakHeight = m->pos[1];
    }
    if (sourceAction != ACT_FLYING && (m->flags & MARIO_WING_CAP)
        && (m->input & INPUT_NONZERO_ANALOG)) {
        struct Surface *wall = mario_find_air_twirl_entry_wall(m);
        f32 speed = sqrtf(sqr(m->vel[0]) + sqr(m->vel[2]));
        if (wall != NULL) {
            // Award the incoming impact before stick redirection can erase its approach.
            m->faceAngle[1] = atan2s(m->vel[2], m->vel[0]);
            mario_set_forward_vel(m, speed);
            set_mario_wall(m, wall);
            mario_air_twirl_bounce_off_wall(m);
            speed = sqrtf(sqr(m->vel[0]) + sqr(m->vel[2]));
        }
        m->faceAngle[1] = m->intendedYaw;
        mario_set_forward_vel(m, speed);
    }
    mario_play_air_twirl_sound(m);
    return TRUE;
}

static s32 mario_air_twirl_input_allowed(struct MarioState *m) {
    // Held Z is compatible with air twirls; fresh Z keeps its native action priority.
    return game_options_get(GO_TWIRLS) && m->heldObj == NULL && m->riddenObj == NULL
           && m->health >= 0x100
           && (!(m->input & INPUT_Z_DOWN)
               || ((m->action & ACT_FLAG_AIR) && !(m->input & INPUT_Z_PRESSED)))
           && !(m->input & (INPUT_STOMPED | INPUT_SQUISHED | INPUT_FIRST_PERSON));
}

s32 mario_try_wall_air_twirl(struct MarioState *m) {
    s32 wallContact = m->action == ACT_AIR_HIT_WALL;
    s32 wallBonk = m->prevAction == ACT_AIR_HIT_WALL
                   && (m->action == ACT_SOFT_BONK || m->action == ACT_BACKWARD_AIR_KB
                       || m->action == ACT_FORWARD_AIR_KB);
    s32 validWindow = wallContact && m->actionTimer < WALL_KICK_GRACE_FRAMES;
    s32 validBonk = wallBonk && m->wallKickTimer != 0;
    // A fresh Wing twirl remains available after the wall-kick grace expires.
    s32 wingPress = (m->flags & MARIO_WING_CAP) && (m->controller->buttonPressed & L_TRIG)
                    && (wallContact || wallBonk);

    if ((validWindow || validBonk || wingPress) && (m->controller->buttonDown & L_TRIG)
        && m->airTwirlCooldown == 0 && mario_air_twirl_input_allowed(m)) {
        return mario_start_air_twirl(m, FALSE);
    }
    return FALSE;
}

// Input and cooldown policy are checked by the caller; this is action eligibility only.
static s32 mario_air_twirl_action_allows_entry(struct MarioState *m) {
    switch (m->action) {
        case ACT_JUMP_KICK:
            return mario_jump_kick_can_cancel(m);
        case ACT_JUMP:
        case ACT_DOUBLE_JUMP:
        case ACT_TRIPLE_JUMP:
        case ACT_BACKFLIP:
        case ACT_SIDE_FLIP:
        case ACT_LONG_JUMP:
        case ACT_WALL_KICK_AIR:
        case ACT_STEEP_JUMP:
        case ACT_FREEFALL:
        case ACT_TWIRLING:
            return TRUE;
        case ACT_WATER_JUMP:
            return m->actionTimer > WATER_JUMP_WALL_KICK_LOCK_FRAMES;
        case ACT_FLYING:
            return (m->flags & MARIO_WING_CAP) != 0;
    }
    return FALSE;
}

// Called once before interactions; air input is never queued from an excluded action.
s32 mario_try_air_twirl(struct MarioState *m) {
    if (!game_options_get(GO_TWIRLS)) {
        mario_air_twirl_init(m);
        return FALSE;
    }
    s32 jumpTwirlQueued = m->airTwirlJumpQueued;
    s32 lPressedLastFrame = m->airTwirlLPressed;
    m->airTwirlJumpQueued = FALSE;
    m->airTwirlLPressed = FALSE;
    if (m->airTwirlInputFrames > 0) {
        m->airTwirlInputFrames--;
    }
    if (m->health < 0x100 || (m->action & ACT_GROUP_MASK) == ACT_GROUP_CUTSCENE) {
        m->airTwirlInputFrames = 0;
    } else if ((m->action & ACT_FLAG_AIR) && (m->controller->buttonPressed & L_TRIG)) {
        m->airTwirlInputFrames =
            m->action == ACT_JUMP_KICK ? JUMP_KICK_BUFFER_FRAMES : AIR_TWIRL_INPUT_BUFFER_FRAMES;
    }

    if (m->wallTwirlSoundTimer > 0) {
        m->wallTwirlSoundTimer--;
    }
    if (!(m->flags & MARIO_WING_CAP)) {
        m->airTwirlWingControlTimer = 0;
    } else if (m->airTwirlWingControlTimer > 0) {
        m->airTwirlWingControlTimer--;
    }
    if (m->airTwirlCooldown > 0) {
        m->airTwirlCooldown--;
    }
    if (!(m->controller->buttonDown & L_TRIG)) {
        m->airTwirlContinue = FALSE;
    }
    if (!mario_air_twirl_input_allowed(m)) {
        m->airTwirlContinue = FALSE;
        return FALSE;
    }
    if (m->action == ACT_FREEFALL_LAND && m->airTwirlContinue && m->floor != NULL
        && m->quicksandDepth == 0.0f && !(m->input & INPUT_OFF_FLOOR)) {
        return mario_start_ground_twirl(m, TRUE);
    }
    if (m->airTwirlCooldown != 0 || mario_air_twirl_is_active(m)) {
        return FALSE;
    }
    u32 actionGroup = m->action & ACT_GROUP_MASK;
    if ((actionGroup == ACT_GROUP_STATIONARY || actionGroup == ACT_GROUP_MOVING)
        && !(m->input & INPUT_OFF_FLOOR) && m->floor != NULL && m->quicksandDepth == 0.0f) {
        m->airTwirlLPressed = (m->controller->buttonPressed & L_TRIG) != 0;
        if ((m->input & INPUT_A_PRESSED) && (m->airTwirlLPressed || lPressedLastFrame)) {
            // Input is sampled before the ground handler launches the jump.
            // Keep the completed chord for exactly the following gameplay update.
            m->airTwirlJumpQueued = TRUE;
        }
    }
    if (mario_try_wall_air_twirl(m)) {
        return TRUE;
    }
    if (!(m->controller->buttonPressed & L_TRIG) && !jumpTwirlQueued && m->airTwirlInputFrames == 0) {
        return FALSE;
    }
    if (mario_air_twirl_action_allows_entry(m)) {
        return mario_start_air_twirl(m, FALSE);
    }
    return FALSE;
}

f32 mario_air_twirl_gravity_scale(struct MarioState *m) {
    if (game_options_get(GO_TWIRLS) && m->action == ACT_L_AIR_TWIRL && m->airTwirlGravity) {
        if ((m->flags & MARIO_WING_CAP) && m->airTwirlWingControlTimer > 0 && m->vel[1] <= 0.0f) {
            return AIR_TWIRL_WING_GRAVITY_SCALE;
        }
        if (m->vel[1] < 0.0f) {
            return AIR_TWIRL_GRAVITY_SCALE;
        }
    }
    return 1.0f;
}

f32 mario_air_twirl_control_scale(struct MarioState *m) {
    return game_options_get(GO_TWIRLS) && (m->flags & MARIO_WING_CAP) && m->airTwirlWingControlTimer > 0
                   && (m->action == ACT_L_AIR_TWIRL || m->action == ACT_FREEFALL)
               ? AIR_TWIRL_WING_CONTROL_SCALE
               : 1.0f;
}

void mario_air_twirl_limit_control_speed(struct MarioState *m) {
    if (mario_air_twirl_control_scale(m) == 1.0f || game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        return;
    }
    f32 speed = sqrtf(sqr(m->vel[0]) + sqr(m->vel[2]));
    if (speed > m->airTwirlWingSpeedLimit) {
        f32 scale = m->airTwirlWingSpeedLimit / speed;
        m->forwardVel *= scale;
        m->vel[0] *= scale;
        m->vel[2] *= scale;
        m->slideVelX = m->vel[0];
        m->slideVelZ = m->vel[2];
    }
}

// Copy only the contact yaw across updates, never a potentially recycled Surface pointer.
static s32 mario_air_twirl_bounce_off_wall(struct MarioState *m) {
    f32 approach = m->vel[0] * m->wall->normal.x + m->vel[2] * m->wall->normal.z;
    s32 bounced = FALSE;

    s32 reflectImpact =
        approach < 0.0f
        && abs_angle_diff(m->wallYaw, m->faceAngle[1]) > DEGREES(180 - WALL_KICK_DEGREES);
    if (!m->airTwirlWallContact || m->airTwirlWallYaw != m->wallYaw) {
        if (m->airTwirlWallBounceCount < AIR_TWIRL_WALL_BOUNCE_ZERO_SPEED_COUNT) {
            m->airTwirlWallBounceCount++;
        }
        f32 retainedSpeed =
            m->airTwirlWallBounceCount == AIR_TWIRL_WALL_BOUNCE_ZERO_SPEED_COUNT
                ? 0.0f
                : 1.0f - (m->airTwirlWallBounceCount - 1) * AIR_TWIRL_WALL_BOUNCE_SPEED_LOSS;
        if (reflectImpact) {
            mario_bonk_reflection(m, FALSE);
        } else {
            // Newly eligible side/rear contacts boost along Mario's current facing.
            mario_set_forward_vel(m, absf(m->forwardVel));
            mario_play_wall_bonk_sound(m);
        }
        m->particleFlags |= PARTICLE_VERTICAL_STAR;
        player_stats_wall_bonk(m);
        player_stats_twirl_wall_rebound(TRUE);
        queue_rumble_data(5, 40);
        s32 frame = m->action == ACT_FLYING ? m->airTwirlFlightRollFrame - 1 : (s32) m->actionTimer;
        f32 boost = m->airTwirlWallBounceCount == 1 && frame < AIR_TWIRL_EARLY_BOUNCE_FRAMES
                        ? AIR_TWIRL_EARLY_BOUNCE_BOOST
                        : AIR_TWIRL_WALL_INITIAL_BOOST;
        if (m->forwardVel < 0.0f) {
            m->forwardVel -= boost;
        } else {
            m->forwardVel += boost;
        }
        m->forwardVel *= AIR_TWIRL_WALL_SPEED_MULTIPLIER * retainedSpeed;
        if (!game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
            m->forwardVel = CLAMP(m->forwardVel, -AIR_TWIRL_WALL_SPEED_CAP, AIR_TWIRL_WALL_SPEED_CAP);
        }
        if (m->action == ACT_FLYING) {
            set_vel_from_pitch_and_yaw(m);
        } else {
            mario_set_forward_vel(m, m->forwardVel);
        }
        if (mario_air_twirl_control_scale(m) > 1.0f) {
            // A genuine wall boost can raise the ceiling; stick steering cannot.
            f32 speed = sqrtf(sqr(m->vel[0]) + sqr(m->vel[2]));
            m->airTwirlWingSpeedLimit = MAX(m->airTwirlWingSpeedLimit, speed);
        }
        mario_air_twirl_limit_control_speed(m);
        m->airTwirlWallYaw = m->wallYaw;
        bounced = TRUE;
    }
    m->airTwirlWallContact = TRUE;
    return bounced;
}

s32 mario_air_twirl_flying_wall_bounce(struct MarioState *m) {
    if (!game_options_get(GO_TWIRLS) || m->airTwirlFlightRollFrame == 0 || m->wall == NULL) {
        return FALSE;
    }
    if (mario_air_twirl_bounce_off_wall(m)) {
        m->airTwirlStartAngle += (0x10000 - m->airTwirlStartAngle) * (m->airTwirlFlightRollFrame - 1)
                                 / AIR_TWIRL_FLIGHT_ROLL_FRAMES;
        m->airTwirlFlightRollFrame = 1;
    }
    return TRUE;
}

void mario_air_twirl_flying_action(struct MarioState *m) {
    s32 roll;

    if (m->airTwirlFlightRollFrame == 0) {
        return;
    }
    if (m->wall == NULL) {
        m->airTwirlWallContact = FALSE;
    }
    mario_twirl_push_enemies(m);
    if (m->action != ACT_FLYING) {
        return;
    }
    set_mario_animation(m, MARIO_ANIM_WING_CAP_FLY);
    roll =
        m->airTwirlStartAngle
        + (0x10000 - m->airTwirlStartAngle) * m->airTwirlFlightRollFrame / AIR_TWIRL_FLIGHT_ROLL_FRAMES;
    m->marioObj->header.gfx.angle[0] = -m->faceAngle[0];
    m->marioObj->header.gfx.angle[1] = m->faceAngle[1];
    m->marioObj->header.gfx.angle[2] = m->faceAngle[2] + roll;
    if (++m->airTwirlFlightRollFrame > AIR_TWIRL_FLIGHT_ROLL_FRAMES) {
        m->airTwirlFlightRollFrame = 0;
        m->airTwirlCooldown = m->airTwirlCooldownResetByCoin ? 0 : MARIO_TWIRL_COOLDOWN_FRAMES;
    }
}

s32 mario_air_twirl_action(struct MarioState *m) {
    s32 step, spinFrame, yawOffset;
    s32 bounced = FALSE;
    s16 visualYaw = m->marioObj->header.gfx.angle[1];

    update_air_with_turn(m);
    step = perform_air_step(m, 0);
    if (step == AIR_STEP_LANDED) {
        if (!check_fall_damage_or_get_stuck(m, ACT_HARD_BACKWARD_GROUND_KB)) {
            play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_LANDING);
            if ((m->controller->buttonDown & L_TRIG) && m->quicksandDepth == 0.0f) {
                mario_start_ground_twirl(m, TRUE);
            } else {
                set_mario_action(m, ACT_FREEFALL_LAND, 0);
            }
        }
        return FALSE; // Landing has already spent this update's movement.
    }
    if (step == AIR_STEP_HIT_LAVA_WALL) {
        lava_boost_on_wall(m);
        return FALSE;
    }
    if (step == AIR_STEP_HIT_WALL && m->wall != NULL) {
        if (mario_air_twirl_bounce_off_wall(m)) {
            m->actionTimer = 0;
            bounced = TRUE;
            m->airTwirlGravity = TRUE;
            m->airTwirlStartAngle = visualYaw - m->faceAngle[1];
        }
    } else if (m->wall == NULL) {
        m->airTwirlWallContact = FALSE;
    }
    mario_twirl_push_enemies(m);
    if (m->action != ACT_L_AIR_TWIRL) {
        return FALSE;
    }
    set_mario_animation(m, MARIO_ANIM_TWIRL);
    // Continue from the linked/bounced pose, then finish facing the direction of travel.
    spinFrame = bounced ? 0 : m->actionTimer + 1;
    yawOffset =
        m->airTwirlStartAngle + (0x10000 - m->airTwirlStartAngle) * spinFrame / AIR_TWIRL_FRAMES;
    m->marioObj->header.gfx.angle[0] = 0;
    m->marioObj->header.gfx.angle[1] = m->faceAngle[1] + yawOffset;
    m->marioObj->header.gfx.angle[2] = 0;
    if (!bounced && ++m->actionTimer >= AIR_TWIRL_FRAMES) {
        f32 peakHeight = m->peakHeight;
        set_mario_action(m, ACT_FREEFALL, 0);
        m->peakHeight = peakHeight;
    }
    return FALSE;
}
