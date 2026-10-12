#include <PR/ultratypes.h>

#include "sm64.h"
#include "mario_character.h"
#include "mario_actions_moving.h"
#include "mario_actions_object.h"
#include "mario_punch.h"
#include "mario_actions_airborne.h"
#include "mario.h"
#include "mario_he_thicc.h"
#include "audio/external.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "game_options.h"
#include "game_options_runtime.h"
#include "mario_step.h"
#include "area.h"
#include "interaction.h"
#include "memory.h"
#include "behavior_data.h"
#include "rumble_init.h"

#include "config.h"

u8 gDebugFastWalk = FALSE;

struct LandingAction {
    s16 numFrames;
    s16 doubleJumpTimer;
    u32 verySteepAction;
    u32 endAction;
    u32 aPressedAction;
    u32 offFloorAction;
    u32 slideAction;
};

/*                                               numFrames, doubleJumpTimer,   verySteepAction,                   endAction,    aPressedAction,    offFloorAction,            slideAction, */
struct LandingAction         sJumpLandAction = {         4,               5,      ACT_FREEFALL,          ACT_JUMP_LAND_STOP,   ACT_DOUBLE_JUMP,      ACT_FREEFALL,      ACT_BEGIN_SLIDING, };
struct LandingAction     sFreefallLandAction = {         4,               5,      ACT_FREEFALL,      ACT_FREEFALL_LAND_STOP,   ACT_DOUBLE_JUMP,      ACT_FREEFALL,      ACT_BEGIN_SLIDING, };
struct LandingAction     sSideFlipLandAction = {         4,               5,      ACT_FREEFALL,     ACT_SIDE_FLIP_LAND_STOP,   ACT_DOUBLE_JUMP,      ACT_FREEFALL,      ACT_BEGIN_SLIDING, };
struct LandingAction     sHoldJumpLandAction = {         4,               5, ACT_HOLD_FREEFALL,     ACT_HOLD_JUMP_LAND_STOP,     ACT_HOLD_JUMP, ACT_HOLD_FREEFALL, ACT_HOLD_BEGIN_SLIDING, };
struct LandingAction sHoldFreefallLandAction = {         4,               5, ACT_HOLD_FREEFALL, ACT_HOLD_FREEFALL_LAND_STOP,     ACT_HOLD_JUMP, ACT_HOLD_FREEFALL, ACT_HOLD_BEGIN_SLIDING, };
struct LandingAction     sLongJumpLandAction = {         6,               5,      ACT_FREEFALL,     ACT_LONG_JUMP_LAND_STOP,     ACT_LONG_JUMP,      ACT_FREEFALL,      ACT_BEGIN_SLIDING, };
struct LandingAction   sDoubleJumpLandAction = {         4,               5,      ACT_FREEFALL,   ACT_DOUBLE_JUMP_LAND_STOP,          ACT_JUMP,      ACT_FREEFALL,      ACT_BEGIN_SLIDING, };
struct LandingAction sTripleJumpLandAction = {
    4, 0, ACT_FREEFALL, ACT_TRIPLE_JUMP_LAND_STOP, ACT_JUMP, ACT_FREEFALL, ACT_BEGIN_SLIDING,
};
struct LandingAction     sBackflipLandAction = {         4,               0,      ACT_FREEFALL,      ACT_BACKFLIP_LAND_STOP,      ACT_BACKFLIP,      ACT_FREEFALL,      ACT_BEGIN_SLIDING, };

Mat4 sFloorAlignMatrix[2];

#define RUNNING_MAX_VISUAL_PITCH DEGREES(60)

s16 tilt_body_running(struct MarioState *m) {
    f32 pitch = find_floor_slope(m, 0) * m->forwardVel / 40.0f;
    // Carried speed can greatly exceed the walking target. Limit the model
    // lean before narrowing the angle, preserving slope movement and speed.
    return CLAMP(-pitch, -RUNNING_MAX_VISUAL_PITCH, RUNNING_MAX_VISUAL_PITCH);
}

void play_step_sound(struct MarioState *m, s16 frame1, s16 frame2) {
    if (is_anim_past_frame(m, frame1) || is_anim_past_frame(m, frame2)) {
        mario_he_thicc_running_step(m);
        if (m->flags & MARIO_METAL_CAP) {
            if (m->marioObj->header.gfx.animInfo.animID == MARIO_ANIM_TIPTOE) {
                play_sound_and_spawn_particles(m, SOUND_ACTION_METAL_STEP_TIPTOE, 0);
            } else {
                play_sound_and_spawn_particles(m, SOUND_ACTION_METAL_STEP, 0);
            }
        } else if (m->quicksandDepth > 50.0f) {
            play_sound(SOUND_ACTION_QUICKSAND_STEP, m->marioObj->header.gfx.cameraToObject);
        } else if (m->marioObj->header.gfx.animInfo.animID == MARIO_ANIM_TIPTOE) {
            play_sound_and_spawn_particles(m, SOUND_ACTION_TERRAIN_STEP_TIPTOE, 0);
        } else {
            play_sound_and_spawn_particles(m, SOUND_ACTION_TERRAIN_STEP, 0);
        }
    }
}

void align_with_floor(struct MarioState *m) {
    struct Surface *floor = m->floor;
    if ((floor != NULL) && (m->pos[1] < (m->floorHeight + 80.0f))) {
        m->pos[1] = m->floorHeight;
#ifdef FAST_FLOOR_ALIGN
        if (absf(m->forwardVel) > FAST_FLOOR_ALIGN) {
            Vec3f floorNormal;
            surface_normal_to_vec3f(floorNormal, floor);
            mtxf_align_terrain_normal(sFloorAlignMatrix[m->playerID], floorNormal, m->pos, m->faceAngle[1]);
        } else {
            mtxf_align_terrain_triangle(sFloorAlignMatrix[m->playerID], m->pos, m->faceAngle[1], 40.0f);
        }
#else
        mtxf_align_terrain_triangle(sFloorAlignMatrix[m->playerID], m->pos, m->faceAngle[1], 40.0f);
#endif
        m->marioObj->header.gfx.throwMatrix = &sFloorAlignMatrix[m->playerID];
    }
}

s32 begin_walking_action(struct MarioState *m, f32 forwardVel, u32 action, u32 actionArg) {
    m->faceAngle[1] = m->intendedYaw;
    mario_set_forward_vel(m, forwardVel);
    return set_mario_action(m, action, actionArg);
}

static s32 check_ledge_climb_down_internal(struct MarioState *m, s32 allowFast, s32 slideEdge) {
    struct WallCollisionData wallCols;
    struct Surface *floor, *bestFloor = NULL, *bestWall = NULL;
    Vec3f bestPos;
    f32 bestSpeed = 0.0f;
    s32 i;

    if (allowFast || m->forwardVel < 10.0f) {
        wallCols.x = m->pos[0];
        wallCols.y = m->pos[1];
        wallCols.z = m->pos[2];
        // A stopped slide retains the support preceding its rejected substep.
        // Cover that substep's bounded horizontal travel, including fast slides.
        wallCols.radius = slideEdge ? 26.0f : 10.0f;
        wallCols.offsetY = -10.0f;

        if (find_wall_collisions(&wallCols) != 0) {
            f32 floorHeight = find_floor(wallCols.x, wallCols.y, wallCols.z, &floor);
            if (floor != NULL && (wallCols.y - floorHeight) > 160.0f) {
                for (i = 0; i < wallCols.numWalls; i++) {
                    struct Surface *wall = wallCols.walls[i];
                    struct Surface *anchorFloor, *ceil;
                    Vec3f anchorPos;
                    s16 wallAngle = SURFACE_YAW(wall);
                    f32 outwardSpeed = m->vel[0] * wall->normal.x + m->vel[2] * wall->normal.z;

                    if ((!slideEdge && abs_angle_diff(wallAngle, m->faceAngle[1]) >= 0x4000)
                        || (slideEdge && outwardSpeed <= 0.0f)) {
                        continue;
                    }
                    // Wall resolution pushes to the probe radius outside the edge.
                    // Keep the same two-unit inset when a slide uses a wider probe.
                    f32 anchorOffset = slideEdge ? wallCols.radius + 2.0f : 20.0f;
                    anchorPos[0] = wallCols.x - anchorOffset * wall->normal.x;
                    anchorPos[2] = wallCols.z - anchorOffset * wall->normal.z;
                    anchorPos[1] = find_floor(anchorPos[0], m->pos[1], anchorPos[2], &anchorFloor);
                    // Match ordinary ground-step support: a drop over 100 units
                    // is a fall, not a ledge anchor. Hanging also needs a gentle top.
                    if (anchorFloor == NULL || anchorFloor->normal.y < COS25
                        || absf(anchorPos[1] - m->pos[1]) > 100.0f
                        || find_mario_ceil(anchorPos, anchorPos[1], &ceil) - anchorPos[1] < 160.0f) {
                        continue;
                    }
                    // Keep the first valid candidate on a tie; rejected walls
                    // cannot overwrite its position or supporting floor.
                    if (bestWall == NULL || outwardSpeed > bestSpeed) {
                        bestWall = wall;
                        bestFloor = anchorFloor;
                        bestSpeed = outwardSpeed;
                        vec3f_copy(bestPos, anchorPos);
                    }
                }
                if (bestWall != NULL) {
                    vec3f_copy(m->pos, bestPos);
                    set_mario_floor(m, bestFloor, bestPos[1]);
                    m->faceAngle[0] = 0;
                    m->faceAngle[1] = SURFACE_YAW(bestWall) + 0x8000;
                    set_mario_action(m, ACT_LEDGE_CLIMB_DOWN, 0);
                    set_mario_animation(m, MARIO_ANIM_CLIMB_DOWN_LEDGE);
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

s32 check_ledge_climb_down(struct MarioState *m, s32 allowFast) {
    return check_ledge_climb_down_internal(m, allowFast, FALSE);
}

void slide_bonk(struct MarioState *m, u32 fastAction, u32 slowAction) {
    if (m->forwardVel > 16.0f) {
        mario_bonk_reflection(m, TRUE);
        drop_and_set_mario_action(m, fastAction, 0);
        mario_set_wall_bonk_momentum(m, m->forwardVel, 0.0f);
    } else {
        mario_set_forward_vel(m, 0.0f);
        set_mario_action(m, slowAction, 0);
    }
}

s32 set_triple_jump_action(struct MarioState *m, UNUSED u32 action, UNUSED u32 actionArg) {
    if (m->flags & MARIO_WING_CAP) {
        return set_mario_action(m, ACT_FLYING_TRIPLE_JUMP, 0);
    } else if (absf(m->forwardVel) > TRIPLE_JUMP_SPEED_THRESHOLD) {
        return set_mario_action(m, ACT_TRIPLE_JUMP,
                                m->forwardVel < 0.0f ? ACT_ARG_TRIPLE_JUMP_BACKWARD
                                                     : ACT_ARG_TRIPLE_JUMP_FORWARD);
    } else {
        return set_mario_action(m, ACT_JUMP, 0);
    }

    return FALSE;
}

#define GROUND_SLIDE_SPEED_CAP 100.0f

void update_sliding_angle(struct MarioState *m, f32 accel, f32 lossFactor) {
    s32 newFacingDYaw;
    s16 facingDYaw;

    struct Surface *floor = m->floor;
    s16 slopeAngle = atan2s(floor->normal.z, floor->normal.x);
    f32 steepness = sqrtf(floor->normal.x * floor->normal.x + floor->normal.z * floor->normal.z);

    m->slideVelX += accel * steepness * sins(slopeAngle);
    m->slideVelZ += accel * steepness * coss(slopeAngle);

    m->slideVelX *= lossFactor;
    m->slideVelZ *= lossFactor;

    m->slideYaw = atan2s(m->slideVelZ, m->slideVelX);

    facingDYaw = m->faceAngle[1] - m->slideYaw;
    newFacingDYaw = facingDYaw;

    // The dive-grab stand-up retains its slide but should not turn Mario to
    // follow that slide, including when the object is a crazy box.
    if (m->action != ACT_DIVE_PICKING_UP) {
        if (newFacingDYaw > 0 && newFacingDYaw <= 0x4000) {
            if ((newFacingDYaw -= 0x200) < 0) {
                newFacingDYaw = 0;
            }
        } else if (newFacingDYaw >= -0x4000 && newFacingDYaw < 0) {
            if ((newFacingDYaw += 0x200) > 0) {
                newFacingDYaw = 0;
            }
        } else if (newFacingDYaw > 0x4000 && newFacingDYaw < 0x8000) {
            if ((newFacingDYaw += 0x200) > 0x8000) {
                newFacingDYaw = 0x8000;
            }
        } else if (newFacingDYaw > -0x8000 && newFacingDYaw < -0x4000) {
            if ((newFacingDYaw -= 0x200) < -0x8000) {
                newFacingDYaw = -0x8000;
            }
        }

        m->faceAngle[1] = m->slideYaw + newFacingDYaw;
    }

    m->forwardVel = sqrtf(sqr(m->slideVelX) + sqr(m->slideVelZ));
    if (m->forwardVel > GROUND_SLIDE_SPEED_CAP && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        m->slideVelX = m->slideVelX * GROUND_SLIDE_SPEED_CAP / m->forwardVel;
        m->slideVelZ = m->slideVelZ * GROUND_SLIDE_SPEED_CAP / m->forwardVel;
        m->forwardVel = GROUND_SLIDE_SPEED_CAP;
    }

    m->vel[0] = m->slideVelX;
    m->vel[1] = 0.0f;
    m->vel[2] = m->slideVelZ;

    mario_update_moving_sand(m);
    mario_update_windy_ground(m);

    if (newFacingDYaw < -0x4000 || newFacingDYaw > 0x4000) {
        m->forwardVel *= -1.0f;
    }
}

s32 update_sliding(struct MarioState *m, f32 stopSpeed) {
    f32 lossFactor;
    f32 accel;
    f32 oldSpeed;
    f32 newSpeed;
    f32 steeringMag = m->action == ACT_DIVE_PICKING_UP ? 0.0f : m->intendedMag;

    s32 stopped = FALSE;

    s16 intendedDYaw = m->intendedYaw - m->slideYaw;
    f32 forward = coss(intendedDYaw);
    f32 sideward = sins(intendedDYaw);

    //! 10k glitch
    if (forward < 0.0f && m->forwardVel >= 0.0f) {
        forward *= 0.5f + 0.5f * m->forwardVel / 100.0f;
    }

    switch (mario_get_floor_class(m)) {
        case SURFACE_CLASS_VERY_SLIPPERY:
            accel = 10.0f;
            lossFactor = steeringMag / 32.0f * forward * 0.02f + 0.98f;
            break;

        case SURFACE_CLASS_SLIPPERY:
            accel = 8.0f;
            lossFactor = steeringMag / 32.0f * forward * 0.02f + 0.96f;
            break;

        default:
            accel = 7.0f;
            lossFactor = steeringMag / 32.0f * forward * 0.02f + 0.92f;
            break;

        case SURFACE_CLASS_NOT_SLIPPERY:
            accel = 5.0f;
            lossFactor = steeringMag / 32.0f * forward * 0.02f + 0.92f;
            break;
    }

    if (m->action == ACT_SLIDE_KICK_SLIDE) {
        lossFactor = 1.0f - (1.0f - lossFactor) / 1.2f;
    } else if (m->action == ACT_DIVE_SLIDE || m->action == ACT_DIVE_PICKING_UP) {
        lossFactor = 1.0f - (1.0f - lossFactor) * 0.9f;
    }

    oldSpeed = sqrtf(m->slideVelX * m->slideVelX + m->slideVelZ * m->slideVelZ);

    //! This is attempting to use trig derivatives to rotate Mario's speed.
    // It is slightly off/asymmetric since it uses the new X speed, but the old
    // Z speed.
    m->slideVelX += m->slideVelZ * (steeringMag / 32.0f) * sideward * 0.05f;
    m->slideVelZ -= m->slideVelX * (steeringMag / 32.0f) * sideward * 0.05f;

    newSpeed = sqrtf(m->slideVelX * m->slideVelX + m->slideVelZ * m->slideVelZ);

    if (oldSpeed > 0.0f && newSpeed > 0.0f) {
        m->slideVelX = m->slideVelX * oldSpeed / newSpeed;
        m->slideVelZ = m->slideVelZ * oldSpeed / newSpeed;
    }

    update_sliding_angle(m, accel, lossFactor);

    if (!mario_floor_is_slope(m) && m->forwardVel * m->forwardVel < stopSpeed * stopSpeed) {
        mario_set_forward_vel(m, 0.0f);
        stopped = TRUE;
    }

    return stopped;
}

void apply_slope_accel(struct MarioState *m) {
    f32 slopeAccel;

    struct Surface *floor = m->floor;
    f32 steepness = sqrtf(sqr(floor->normal.x) + sqr(floor->normal.z));

    s16 floorDYaw = abs_angle_diff(m->floorYaw, m->faceAngle[1]);

    if (mario_floor_is_slope(m)) {
        s16 slopeClass = 0;

        if (m->action != ACT_SOFT_BACKWARD_GROUND_KB && m->action != ACT_SOFT_FORWARD_GROUND_KB) {
            slopeClass = mario_get_floor_class(m);
        }

        switch (slopeClass) {
            case SURFACE_CLASS_VERY_SLIPPERY:
                slopeAccel = 5.3f;
                break;
            case SURFACE_CLASS_SLIPPERY:
                slopeAccel = 2.7f;
                break;
            default:
                slopeAccel = 1.7f;
                break;
            case SURFACE_CLASS_NOT_SLIPPERY:
                slopeAccel = 0.0f;
                break;
        }

        if (floorDYaw < 0x4000) {
            m->forwardVel += slopeAccel * steepness;
        } else {
            if (mario_has_current_metal_cap(m) && (m->area->terrainType & TERRAIN_MASK) != TERRAIN_SLIDE
                && m->floor->type != SURFACE_SUPER_SLIPPERY
                && (m->action == ACT_WALKING || m->action == ACT_HOLD_WALKING
                    || m->action == ACT_METAL_WATER_WALKING)) {
                slopeAccel *= 0.5f;
            }
            m->forwardVel -= slopeAccel * steepness;
        }
    }

    m->slideYaw = m->faceAngle[1];

    m->slideVelX = m->forwardVel * sins(m->faceAngle[1]);
    m->slideVelZ = m->forwardVel * coss(m->faceAngle[1]);

    m->vel[0] = m->slideVelX;
    m->vel[1] = 0.0f;
    m->vel[2] = m->slideVelZ;

    mario_update_moving_sand(m);
    mario_update_windy_ground(m);
}

s32 apply_landing_accel(struct MarioState *m, f32 frictionFactor) {
    s32 stopped = FALSE;

    apply_slope_accel(m);

    if (!mario_floor_is_slope(m)) {
        m->forwardVel *= frictionFactor;
        if (m->forwardVel * m->forwardVel < 1.0f) {
            mario_set_forward_vel(m, 0.0f);
            stopped = TRUE;
        }
    }

    return stopped;
}

void update_shell_speed(struct MarioState *m) {
    f32 maxTargetSpeed;
    f32 targetSpeed;

    if (m->floorHeight < m->waterLevel) {
        set_mario_floor(m, &gWaterSurfacePseudoFloor, m->waterLevel);
        m->floor->originOffset = -m->waterLevel;
        // m->floor->originOffset = m->waterLevel; //! (Original code) Negative origin offset
    }

    if (m->floor != NULL && m->floor->type == SURFACE_SLOW
        && !game_options_cheat_active(GO_IGNORE_SAND)) {
        maxTargetSpeed = 48.0f;
    } else {
        maxTargetSpeed = 96.0f;
    }

    targetSpeed = m->intendedMag * 3.0f;
    if (targetSpeed > maxTargetSpeed) {
        targetSpeed = maxTargetSpeed;
    }
    if (targetSpeed < 24.0f) {
        targetSpeed = 24.0f;
    }

    if (m->forwardVel <= 0.0f) {
        m->forwardVel += 1.1f;
    } else if (m->forwardVel <= targetSpeed) {
        m->forwardVel += 1.1f - m->forwardVel / 87.0f;
    } else if (m->floor->normal.y >= 0.95f) {
        m->forwardVel -= 1.0f;
    }

    m->faceAngle[1] =
        m->intendedYaw - approach_s32((s16)(m->intendedYaw - m->faceAngle[1]), 0, 0x800, 0x800);

    apply_slope_accel(m);
}

s32 apply_slope_decel(struct MarioState *m, f32 decelCoef) {
    f32 decel;
    s32 stopped = FALSE;

    switch (mario_get_floor_class(m)) {
        case SURFACE_CLASS_VERY_SLIPPERY:
            decel = decelCoef * 0.2f;
            break;
        case SURFACE_CLASS_SLIPPERY:
            decel = decelCoef * 0.7f;
            break;
        default:
            decel = decelCoef * 2.0f;
            break;
        case SURFACE_CLASS_NOT_SLIPPERY:
            decel = decelCoef * 3.0f;
            break;
    }

    if ((m->forwardVel = approach_f32(m->forwardVel, 0.0f, decel, decel)) == 0.0f) {
        stopped = TRUE;
    }

    apply_slope_accel(m);
    return stopped;
}

s32 update_decelerating_speed(struct MarioState *m) {
    s32 stopped = FALSE;
    f32 drag = (m->action == ACT_DECELERATING && m->actionArg == ACT_ARG_GROUND_SPEED_FROM_ROLLOUT)
                   ? GROUND_NEUTRAL_ROLLOUT_COAST_DECEL
                   : GROUND_NEUTRAL_COAST_DECEL;

    if ((m->forwardVel = approach_f32(m->forwardVel, 0.0f, drag, drag)) == 0.0f) {
        stopped = TRUE;
    }

    mario_set_forward_vel(m, m->forwardVel);
    mario_update_moving_sand(m);
    mario_update_windy_ground(m);

    return stopped;
}

// Recovery poses can still carry ground speed. Keep their motion synchronized
// and coast to rest without routing through stationary physics while moving.
s32 coasting_ground_step(struct MarioState *m) {
    if (m->forwardVel == 0.0f) {
        return stationary_ground_step(m);
    }

    update_decelerating_speed(m);
    m->vel[1] = 0.0f;
    s32 step = perform_ground_step(m);
    if (step == GROUND_STEP_HIT_WALL) {
        mario_set_forward_vel(m, 0.0f);
    }
    return step;
}

static s32 should_brake_negative_ground_speed(struct MarioState *m) {
    return m->forwardVel < 0.0f && (m->input & INPUT_NONZERO_ANALOG) && m->action != ACT_GROUND_TWIRL
           && m->quicksandDepth <= 10.0f
           && (m->floor == NULL || m->floor->type != SURFACE_SLOW
               || game_options_cheat_active(GO_IGNORE_SAND));
}

void update_walking_speed_with_minimum(struct MarioState *m, f32 minimumSpeed, f32 maximumSpeed) {
    f32 maxTargetSpeed;
    f32 targetSpeed = m->intendedMag;
    f32 acceleration = 1.1f;
    s32 normalWalk = m->action == ACT_WALKING || m->action == ACT_METAL_WATER_WALKING;
    s32 trialWalk = normalWalk && gDebugFastWalk && game_options_debug_active();

    if (m->floor != NULL && m->floor->type == SURFACE_SLOW
        && !game_options_cheat_active(GO_IGNORE_SAND)) {
        maxTargetSpeed = 24.0f;
        trialWalk = FALSE;
    } else if (normalWalk) {
        s32 currentWing = (m->flags & MARIO_WING_CAP) && game_options_get(GO_WING_CAP) == 0;
        s32 wading =
            !mario_has_current_vanish_cap(m) && !currentWing && m->pos[1] < m->waterLevel - 10.0f;
        s32 onSand = !game_options_cheat_active(GO_IGNORE_SAND)
                     && m->terrainSoundAddend == (SOUND_TERRAIN_SAND << 16);
        f32 walkTarget = currentWing ? WALK_WING_TARGET_SPEED : m->movementProfile->walkTarget;
        if (wading || onSand) {
            // Running targets change with the floor; carried speed still brakes normally.
            maxTargetSpeed = MIN(walkTarget, WALK_SLOW_TERRAIN_TARGET_SPEED);
            trialWalk = FALSE;
        } else {
            maxTargetSpeed = trialWalk ? WALK_DEBUG_TARGET_SPEED : walkTarget;
        }
        targetSpeed *= maxTargetSpeed / 32.0f;
        acceleration = trialWalk ? 1.375f : m->movementProfile->walkAcceleration;
    } else if (m->action == ACT_GROUND_TWIRL) {
        maxTargetSpeed = GROUND_TWIRL_TARGET_SPEED;
        targetSpeed *= maxTargetSpeed / 32.0f;
    } else {
        maxTargetSpeed = 32.0f;
    }

    targetSpeed = MIN(targetSpeed, maxTargetSpeed);

    if (m->quicksandDepth > 10.0f && !game_options_cheat_active(GO_IGNORE_SAND)) {
        targetSpeed *= 6.25f / m->quicksandDepth;
        acceleration = 1.1f;
    }

    if (m->action == ACT_CRAWLING) {
        // Settle on the crawl target without alternating above and below it.
        m->forwardVel = approach_f32(m->forwardVel, targetSpeed, 1.5f, 0.5f);
    } else if (should_brake_negative_ground_speed(m)) {
        // Reversing facing must not redirect remaining backward carry.
        if (abs_angle_diff(m->faceAngle[1], m->intendedYaw) > 0x4000) {
            m->forwardVel = 0.0f;
        } else {
            m->forwardVel = MIN(0.0f, m->forwardVel + WALK_STICK_NEGATIVE_DECEL);
        }
    } else if (m->forwardVel <= 0.0f) {
        // Slow down if moving backwards
        m->forwardVel += acceleration;
    } else if (m->forwardVel <= targetSpeed) {
        // If accelerating
        if (trialWalk) {
            // Preserve the original acceleration curve relative to the new
            // full-stick target, and settle exactly at that target.
            f32 dragDivisor = 43.0f * WALK_DEBUG_TARGET_SPEED / WALK_TARGET_SPEED;
            m->forwardVel =
                MIN(targetSpeed, m->forwardVel + acceleration - m->forwardVel / dragDivisor);
        } else {
            m->forwardVel = MIN(targetSpeed, m->forwardVel + acceleration - m->forwardVel / 43.0f);
        }
    } else if (m->floor->normal.y >= 0.95f
               || (normalWalk && mario_get_floor_class(m) == SURFACE_CLASS_NOT_SLIPPERY)) {
        // Grippy slopes add no downhill acceleration, so carried running
        // speed must still return to the stick's walking target here.
        m->forwardVel = MAX(targetSpeed, m->forwardVel - 1.0f);
    }

    // Walking can carry speed gained in the air into the next jump. The
    // deceleration above targetSpeed already brings it back down gradually.
    if (!normalWalk && m->forwardVel > maximumSpeed && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        m->forwardVel = maximumSpeed;
    }
    if (m->forwardVel >= 0.0f && m->forwardVel < minimumSpeed) {
        m->forwardVel = MIN(minimumSpeed, maximumSpeed);
    }

#ifdef VELOCITY_BASED_TURN_SPEED
    if ((m->heldObj == NULL) && !(m->action & ACT_FLAG_SHORT_HITBOX)) {
        if (m->forwardVel >= 16.0f) {
            s16 turnRange = abs_angle_diff(m->faceAngle[1], m->intendedYaw);
            f32 fac = (m->forwardVel + m->intendedMag);
            turnRange *= (1.0f - (CLAMP(fac, 0.0f, 32.0f) / 32.0f));
            turnRange = MAX(turnRange, 0x800);

            approach_angle_bool(&m->faceAngle[1], m->intendedYaw, turnRange);
        } else if (m->action == ACT_WALKING && m->actionArg == ACT_ARG_WALKING_TURN_FROM_PUNCH) {
            m->faceAngle[1] = approach_s16(m->faceAngle[1], m->intendedYaw, 0x800, 0x800);
        } else {
            m->faceAngle[1] = m->intendedYaw;
        }
    } else {
        m->faceAngle[1] = m->intendedYaw - approach_s32((s16)(m->intendedYaw - m->faceAngle[1]), 0, 0x800, 0x800);
    }
#else
    // Vanilla
    m->faceAngle[1] =
        m->intendedYaw - approach_s32((s16)(m->intendedYaw - m->faceAngle[1]), 0, 0x800, 0x800);
#endif
    if (m->action == ACT_WALKING && m->actionArg == ACT_ARG_WALKING_TURN_FROM_PUNCH
        && m->faceAngle[1] == m->intendedYaw) {
        m->actionArg = 0;
    }
    apply_slope_accel(m);
}

void update_walking_speed(struct MarioState *m) {
    // Normal walking keeps above-target speed; only other actions use this limit.
    update_walking_speed_with_minimum(m, 0.0f, 48.0f);
}

s32 should_begin_sliding(struct MarioState *m) {
    if (m->input & INPUT_ABOVE_SLIDE) {
        s32 superSlippery = (m->floor != NULL) && (m->floor->type == SURFACE_SUPER_SLIPPERY);
        s32 slideLevel = (m->area->terrainType & TERRAIN_MASK) == TERRAIN_SLIDE;
        s32 movingBackward = m->forwardVel <= -1.0f;

        if (((superSlippery || slideLevel) && !game_options_cheat_active(GO_IGNORE_SLIPPERY))
            || movingBackward || mario_facing_downhill(m, FALSE)) {
            return TRUE;
        }
    }

    return FALSE;
}

#define analog_stick_held_back(m) (abs_angle_diff((m)->intendedYaw, (m)->faceAngle[1]) > 0x471C)

static s32 start_moving_crouch_attack(struct MarioState *m) {
    if (m->controller->stickMag >= MARIO_STRONG_STICK_THRESHOLD && !analog_stick_held_back(m)) {
        return set_mario_action(m, ACT_SLIDE_KICK, 0);
    }
    return set_mario_action(m, ACT_MOVE_PUNCHING, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
}

s32 check_ground_dive_or_punch(struct MarioState *m) {
    if (m->zBPressed) {
        return start_moving_crouch_attack(m);
    }

    if (m->input & INPUT_B_PRESSED) {
        if (!(m->input & INPUT_Z_DOWN) && m->controller->stickMag >= MARIO_STRONG_STICK_THRESHOLD
            && !analog_stick_held_back(m)) {
            m->vel[1] = 25.0f;
            return set_mario_action(m, ACT_DIVE, 1);
        }

        return set_mario_action(m, ACT_MOVE_PUNCHING, 0);
    }

    return FALSE;
}

s32 begin_braking_action(struct MarioState *m) {
    mario_drop_held_object(m);

    if (m->actionState == ACT_STATE_WALKING_REACH_WALL) {
        m->faceAngle[1] = m->actionArg;
        return set_mario_action(m, ACT_STANDING_AGAINST_WALL, 0);
    }

    u32 rolloutArg = (!(m->input & INPUT_NONZERO_ANALOG) && m->action == ACT_WALKING
                      && m->actionArg == ACT_ARG_GROUND_SPEED_FROM_ROLLOUT)
                         ? ACT_ARG_GROUND_SPEED_FROM_ROLLOUT
                         : 0;
    if (m->forwardVel >= 16.0f && m->floor->normal.y >= COS80) {
        return set_mario_action(m, ACT_BRAKING, rolloutArg);
    }

    return set_mario_action(m, ACT_DECELERATING, rolloutArg);
}

void anim_and_audio_for_walk(struct MarioState *m) {
    s32 animSpeed;
    struct Object *marioObj = m->marioObj;
    s32 inLoop = TRUE;
    s16 targetPitch = 0;

    f32 intendedSpeed = MAX(m->intendedMag, m->forwardVel);

    if (intendedSpeed < 4.0f) {
        intendedSpeed = 4.0f;
    }

    if (m->quicksandDepth > 50.0f) {
        animSpeed = (s32)(intendedSpeed / 4.0f * 0x10000);
        set_mario_anim_with_accel(m, MARIO_ANIM_MOVE_IN_QUICKSAND, animSpeed);
        play_step_sound(m, 19, 93);
        m->actionTimer = WALK_SPEED_START_TIPTOE;
    } else {
        while (inLoop) {
            switch (m->actionTimer) {
                case WALK_SPEED_START_TIPTOE:
                    if (intendedSpeed > 8.0f) {
                        m->actionTimer = WALK_SPEED_WALKING;
                    } else {
                        //! (Speed Crash) If Mario's speed is more than 2^17.
                        animSpeed = (s32)(intendedSpeed / 4.0f * 0x10000);
                        if (animSpeed < 0x1000) {
                            animSpeed = 0x1000;
                        }
                        set_mario_anim_with_accel(m, MARIO_ANIM_START_TIPTOE, animSpeed);
                        play_step_sound(m, 7, 22);
                        if (is_anim_past_frame(m, 23)) {
                            m->actionTimer = WALK_SPEED_WALKING;
                        }

                        inLoop = FALSE;
                    }
                    break;

                case WALK_SPEED_TIPTOE:
                    if (intendedSpeed > 8.0f) {
                        m->actionTimer = WALK_SPEED_WALKING;
                    } else {
                        //! (Speed Crash) If Mario's speed is more than 2^17.
                        animSpeed = (s32)(intendedSpeed * 0x10000);
                        if (animSpeed < 0x1000) {
                            animSpeed = 0x1000;
                        }
                        set_mario_anim_with_accel(m, MARIO_ANIM_TIPTOE, animSpeed);
                        play_step_sound(m, 14, 72);

                        inLoop = FALSE;
                    }
                    break;

                case WALK_SPEED_WALKING:
                    if (intendedSpeed < 5.0f) {
                        m->actionTimer = WALK_SPEED_TIPTOE;
                    } else if (intendedSpeed > 22.0f) {
                        m->actionTimer = WALK_SPEED_RUNNING;
                    } else {
                        //! (Speed Crash) If Mario's speed is more than 2^17.
                        animSpeed = (s32)(intendedSpeed / 4.0f * 0x10000);
                        set_mario_anim_with_accel(m, MARIO_ANIM_WALKING, animSpeed);
                        play_step_sound(m, 10, 49);

                        inLoop = FALSE;
                    }
                    break;

                case WALK_SPEED_RUNNING:
                    if (intendedSpeed < 18.0f) {
                        m->actionTimer = WALK_SPEED_WALKING;
                    } else {
                        //! (Speed Crash) If Mario's speed is more than 2^17.
                        animSpeed = (s32)(intendedSpeed / 4.0f * 0x10000);
                        set_mario_anim_with_accel(m, MARIO_ANIM_RUNNING, animSpeed);
                        play_step_sound(m, 9, 45);
                        targetPitch = tilt_body_running(m);

                        inLoop = FALSE;
                    }
                    break;
            }
        }
    }

    marioObj->oMarioWalkingPitch = approach_s32_symmetric(marioObj->oMarioWalkingPitch, targetPitch, 0x800);
    marioObj->header.gfx.angle[0] = marioObj->oMarioWalkingPitch;
}

void anim_and_audio_for_hold_walk(struct MarioState *m) {
    s32 animSpeed;
    s32 inLoop = TRUE;

    f32 intendedSpeed = MAX(m->intendedMag, m->forwardVel);

    if (intendedSpeed < 2.0f) {
        intendedSpeed = 2.0f;
    }

    while (inLoop) {
        switch (m->actionTimer) {
            case HOLD_WALK_SPEED_SLOW:
                if (intendedSpeed > 6.0f) {
                    m->actionTimer = HOLD_WALK_SPEED_WALKING;
                } else {
                    //! (Speed Crash) Crashes if Mario's speed exceeds or equals 2^15.
                    animSpeed = (s32)(intendedSpeed * 0x10000);
                    set_mario_anim_with_accel(m, MARIO_ANIM_SLOW_WALK_WITH_LIGHT_OBJ, animSpeed);
                    play_step_sound(m, 12, 62);

                    inLoop = FALSE;
                }
                break;

            case HOLD_WALK_SPEED_WALKING:
                if (intendedSpeed < 3.0f) {
                    m->actionTimer = HOLD_WALK_SPEED_SLOW;
                } else if (intendedSpeed > 11.0f) {
                    m->actionTimer = HOLD_WALK_SPEED_RUN;
                } else {
                    //! (Speed Crash) Crashes if Mario's speed exceeds or equals 2^15.
                    animSpeed = (s32)(intendedSpeed * 0x10000);
                    set_mario_anim_with_accel(m, MARIO_ANIM_WALK_WITH_LIGHT_OBJ, animSpeed);
                    play_step_sound(m, 12, 62);

                    inLoop = FALSE;
                }
                break;

            case HOLD_WALK_SPEED_RUN:
                if (intendedSpeed < 8.0f) {
                    m->actionTimer = HOLD_WALK_SPEED_WALKING;
                } else {
                    //! (Speed Crash) Crashes if Mario's speed exceeds or equals 2^16.
                    animSpeed = (s32)(intendedSpeed / 2.0f * 0x10000);
                    set_mario_anim_with_accel(m, MARIO_ANIM_RUN_WITH_LIGHT_OBJ, animSpeed);
                    play_step_sound(m, 10, 49);

                    inLoop = FALSE;
                }
                break;
        }
    }
}

void anim_and_audio_for_heavy_walk(struct MarioState *m) {
    s32 animSpeed = (s32)(m->intendedMag * 0x10000);
    set_mario_anim_with_accel(m, MARIO_ANIM_WALK_WITH_HEAVY_OBJ, animSpeed);
    play_step_sound(m, 26, 79);
}

void push_or_sidle_wall(struct MarioState *m, Vec3f startPos) {
    s16 wallAngle;
    s16 dWallAngle;
    f32 dx = m->pos[0] - startPos[0];
    f32 dz = m->pos[2] - startPos[2];
    f32 movedDistance = sqrtf(sqr(dx) + sqr(dz));
    // Leave signed fixed-point headroom for the current animation frame.
    s32 animSpeed = (s32) (MIN(movedDistance, MARIO_SPEED_SAFETY_CAP * 0.5f) * 2.0f * 0x10000);

    if (m->wall != NULL
        && (m->intendedMag <= 0.0f
            || abs_angle_diff(m->intendedYaw, m->wallYaw + 0x8000) > WALK_WALL_HUG_MAX_ANGLE)) {
        // Glancing stick inputs keep walking rather than forcing the wall-hug pose and speed cap.
        anim_and_audio_for_walk(m);
        return;
    }

    if (m->wall != NULL && m->wall->object != NULL
        && (m->wall->object->oInteractType & (INTERACT_DOOR | INTERACT_WARP_DOOR))) {
        struct Object *door = m->wall->object;
        if (door->oInteractType & INTERACT_WARP_DOOR) {
            interact_warp_door(m, INTERACT_WARP_DOOR, door);
        } else {
            interact_door(m, INTERACT_DOOR, door);
        }
        if (m->action == ACT_WALKING) {
            anim_and_audio_for_walk(m);
        }
        return;
    }

    if (m->forwardVel > 6.0f && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        mario_set_forward_vel(m, 6.0f);
    }

    if (m->wall != NULL) {
        wallAngle = m->wallYaw;
        dWallAngle = wallAngle - m->faceAngle[1];
    }

    if (m->wall == NULL || dWallAngle <= -DEGREES(160) || dWallAngle >= DEGREES(160)) {
        m->flags |= MARIO_PUSHING;
        set_mario_animation(m, MARIO_ANIM_PUSHING);
        play_step_sound(m, 6, 18);
    } else {
        if (dWallAngle < 0) {
            set_mario_anim_with_accel(m, MARIO_ANIM_SIDESTEP_RIGHT, animSpeed);
        } else {
            set_mario_anim_with_accel(m, MARIO_ANIM_SIDESTEP_LEFT, animSpeed);
        }

        if (m->marioObj->header.gfx.animInfo.animFrame < 20) {
            play_sound((SOUND_MOVING_TERRAIN_SLIDE + m->terrainSoundAddend), m->marioObj->header.gfx.cameraToObject);
            m->particleFlags |= PARTICLE_DUST;
        }

        m->actionState = ACT_STATE_PUSH_OR_SIDLE_WALL_SIDLING;
        m->actionArg = wallAngle + 0x8000;
        m->marioObj->header.gfx.angle[1] = wallAngle + 0x8000;
        m->marioObj->header.gfx.angle[2] = find_floor_slope(m, 0x4000);
    }
    
}

void tilt_body_walking(struct MarioState *m, s16 startYaw) {
    struct MarioBodyState *marioBodyState = m->marioBodyState;
    s16 animID = m->marioObj->header.gfx.animInfo.animID;

    if (animID == MARIO_ANIM_WALKING || animID == MARIO_ANIM_RUNNING) {
        s16 dYaw = m->faceAngle[1] - startYaw;
        s32 unlimited = game_options_cheat_active(GO_NO_SPEED_LIMIT);
        s16 nextBodyRoll = unlimited
                               ? (s16) CLAMP(-dYaw * m->forwardVel / 12.0f, -DEGREES(30), DEGREES(30))
                               : -(s16) (dYaw * m->forwardVel / 12.0f);
        s16 nextBodyPitch = unlimited ? (s16) CLAMP(m->forwardVel * 170.0f, 0.0f, DEGREES(30))
                                      : (s16) (m->forwardVel * 170.0f);

        nextBodyRoll  = CLAMP(nextBodyRoll, -DEGREES(30), DEGREES(30));
        nextBodyPitch = CLAMP(nextBodyPitch,         0x0, DEGREES(30));

        marioBodyState->torsoAngle[2] = approach_s32(marioBodyState->torsoAngle[2], nextBodyRoll, 0x400, 0x400);
        marioBodyState->torsoAngle[0] = approach_s32(marioBodyState->torsoAngle[0], nextBodyPitch, 0x400, 0x400);
    } else {
        marioBodyState->torsoAngle[2] = 0;
        marioBodyState->torsoAngle[0] = 0;
    }
}

void tilt_body_ground_shell(struct MarioState *m, s16 startYaw) {
    struct MarioBodyState *marioBodyState = m->marioBodyState;
    struct Object *marioObj = m->marioObj;
    s16 dYaw = m->faceAngle[1] - startYaw;
    s32 unlimited = game_options_cheat_active(GO_NO_SPEED_LIMIT);
    s16 nextBodyRoll = unlimited ? (s16) CLAMP(-dYaw * m->forwardVel / 12.0f, -0x1800, 0x1800)
                                 : -(s16) (dYaw * m->forwardVel / 12.0f);
    s16 nextBodyPitch =
        unlimited ? (s16) CLAMP(m->forwardVel * 170.0f, 0.0f, 0x1000) : (s16) (m->forwardVel * 170.0f);

    nextBodyRoll  = CLAMP(nextBodyRoll, -0x1800, 0x1800);
    nextBodyPitch = CLAMP(nextBodyPitch,      0, 0x1000);

    marioBodyState->torsoAngle[2] = approach_s32_symmetric(marioBodyState->torsoAngle[2], nextBodyRoll,  0x200);
    marioBodyState->torsoAngle[0] = approach_s32_symmetric(marioBodyState->torsoAngle[0], nextBodyPitch, 0x200);
    marioBodyState->headAngle[2] = -marioBodyState->torsoAngle[2];

    marioObj->header.gfx.angle[2] = marioBodyState->torsoAngle[2];
    marioObj->header.gfx.pos[1] += 45.0f;
}

static s32 try_burning_coin_recovery_jump(struct MarioState *m) {
    if (!m->burnCoinJumpPending) {
        return FALSE;
    }
    m->burnCoinJumpPending = FALSE;
    return set_jumping_action(m, ACT_JUMP, 0);
}

s32 act_walking(struct MarioState *m) {
    Vec3f startPos;
    s16 startYaw = m->faceAngle[1];

    mario_drop_held_object(m);

    if (should_begin_sliding(m)) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (try_burning_coin_recovery_jump(m)) {
        return TRUE;
    }

    if (m->input & INPUT_A_PRESSED) {
#ifdef EASIER_LONG_JUMPS
        if (m->input & INPUT_Z_PRESSED && m->controller->stickMag >= MARIO_STRONG_STICK_THRESHOLD) {
            return set_mario_action(m, ACT_CROUCH_SLIDE, 0);
        }
#endif
        if ((m->input & INPUT_Z_DOWN) && m->zHeldFrames == 1
            && m->controller->stickMag >= MARIO_STRONG_STICK_THRESHOLD) {
            return set_jumping_action(m, ACT_LONG_JUMP, 0);
        }
        return set_jump_from_landing(m);
    }

    if (check_ground_dive_or_punch(m)) {
        return TRUE;
    }

    if (m->zCrouchBuffered && m->zHeldFrames >= 1) {
        m->zCrouchBuffered = FALSE;
        return set_mario_action(m, ACT_CROUCH_SLIDE, 0);
    }

    if ((m->input & INPUT_IDLE) && !m->zCrouchBuffered) {
        return begin_braking_action(m);
    }

#ifdef SIDE_FLIP_AT_LOW_SPEEDS
    if (!m->zCrouchBuffered && analog_stick_held_back(m) && m->forwardVel >= 0.0f) {
#else
    if (!m->zCrouchBuffered && analog_stick_held_back(m) && m->forwardVel >= 16.0f) {
#endif
        return set_mario_action(m, ACT_TURNING_AROUND, 0);
    }

    if (((m->input & INPUT_Z_DOWN) || m->zCrouchBuffered) && m->zHeldFrames >= 1) {
        m->zCrouchBuffered = FALSE;
        return set_mario_action(m, ACT_CROUCH_SLIDE, 0);
    }

    m->actionState = ACT_STATE_WALKING_NO_WALL;

    vec3f_copy(startPos, m->pos);
    update_walking_speed(m);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 0);
            set_mario_animation(m, MARIO_ANIM_GENERAL_FALL);
            break;

        case GROUND_STEP_NONE:
            anim_and_audio_for_walk(m);
            if (m->intendedMag - m->forwardVel > 16.0f) {
                m->particleFlags |= PARTICLE_DUST;
            }
            break;

        case GROUND_STEP_HIT_WALL:
            push_or_sidle_wall(m, startPos);
            if (m->action != ACT_WALKING) {
                return TRUE;
            }
            m->actionTimer = 0;
            break;
    }

    check_ledge_climb_down(m, FALSE);
    tilt_body_walking(m, startYaw);
    return FALSE;
}

s32 act_move_punching(struct MarioState *m) {
    if (mario_update_punch_sequence(m)) {
        return TRUE;
    }
    apply_punch_lunge(m);
    apply_punch_ground_drag(m);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 0);
            break;

        case GROUND_STEP_NONE:
            if (absf(m->forwardVel) > 0.5f) {
                m->particleFlags |= PARTICLE_DUST;
            }
            break;
    }

    if (mario_check_punch_lunge_grab(m)) {
        return TRUE;
    }

    return FALSE;
}

s32 act_hold_walking(struct MarioState *m) {
    if (m->heldObj->behavior == segmented_to_virtual(bhvJumpingBox)) {
        return set_mario_action(m, ACT_CRAZY_BOX_BOUNCE, 0);
    }

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_WALKING, 0);
    }

    if (should_begin_sliding(m)) {
        return set_mario_action(m, ACT_HOLD_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_THROWING, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        return set_jumping_action(m, ACT_HOLD_JUMP, 0);
    }

    if (m->input & INPUT_IDLE) {
        return set_mario_action(m, ACT_HOLD_DECELERATING, 0);
    }

    if (m->input & INPUT_Z_PRESSED) {
        return drop_and_set_mario_action(m, ACT_CROUCH_SLIDE, 0);
    }

    m->intendedMag *= HOLD_LIGHT_TARGET_SPEED / 32.0f;

    update_walking_speed(m);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_HOLD_FREEFALL, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            if (m->forwardVel > 16.0f) {
                mario_set_forward_vel(m, 16.0f);
            }
            break;
    }

    anim_and_audio_for_hold_walk(m);

    if (0.4f * m->intendedMag - m->forwardVel > 10.0f) {
        m->particleFlags |= PARTICLE_DUST;
    }

    return FALSE;
}

s32 act_hold_heavy_walking(struct MarioState *m) {
    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_HEAVY_THROW, 0);
    }

    if (should_begin_sliding(m)) {
        return drop_and_set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_IDLE) {
        return set_mario_action(m, ACT_HOLD_HEAVY_IDLE, 0);
    }

    m->intendedMag *= HOLD_HEAVY_TARGET_SPEED / 32.0f;

    update_walking_speed(m);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            drop_and_set_mario_action(m, ACT_FREEFALL, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            if (m->forwardVel > HOLD_HEAVY_TARGET_SPEED
                && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
                mario_set_forward_vel(m, HOLD_HEAVY_TARGET_SPEED);
            }
            break;
    }

    anim_and_audio_for_heavy_walk(m);
    return FALSE;
}

#define SIDE_FLIP_END_CUTOFF_FRAMES 4

static s32 turning_side_flip_window_open(struct MarioState *m) {
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;

    // Only the final part of the turn has an ending window to shorten.
    return animInfo->animID != MARIO_ANIM_TURNING_PART2 || animInfo->curAnim == NULL
           || animInfo->animFrame < animInfo->curAnim->loopEnd - SIDE_FLIP_END_CUTOFF_FRAMES - 1;
}

s32 act_turning_around(struct MarioState *m) {
    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (turning_side_flip_window_open(m)
        && (m->input & (INPUT_Z_DOWN | INPUT_A_DOWN)) == (INPUT_Z_DOWN | INPUT_A_DOWN)
        && m->heldObj == NULL && m->controller->stickMag < MARIO_STRONG_STICK_THRESHOLD) {
        return set_jumping_action(m, ACT_BACKFLIP, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        return set_jumping_action(m, turning_side_flip_window_open(m) ? ACT_SIDE_FLIP : ACT_JUMP, 0);
    }

    if (check_ground_dive_or_punch(m)) {
        return TRUE;
    }
    if (m->input & INPUT_Z_DOWN) {
        return set_mario_action(m, ACT_CROUCH_SLIDE, 0);
    }

    if (m->input & INPUT_IDLE) {
        return set_mario_action(m, ACT_BRAKING, 0);
    }

    if (!analog_stick_held_back(m)) {
        return set_mario_action(m, ACT_WALKING, 0);
    }

    if (apply_slope_decel(m, 2.0f)) {
        return begin_walking_action(m, 8.0f, ACT_FINISH_TURNING_AROUND, 0);
    }

    play_sound(SOUND_MOVING_TERRAIN_SLIDE + m->terrainSoundAddend, m->marioObj->header.gfx.cameraToObject);

    adjust_sound_for_speed(m);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 0);
            break;

        case GROUND_STEP_NONE:
            m->particleFlags |= PARTICLE_DUST;
            break;
    }

    if (m->forwardVel >= 18.0f) {
        set_mario_animation(m, MARIO_ANIM_TURNING_PART1);
    } else {
        set_mario_animation(m, MARIO_ANIM_TURNING_PART2);
        if (is_anim_at_end(m)) {
            if (m->forwardVel > 0.0f) {
                begin_walking_action(m, -m->forwardVel, ACT_WALKING, 0);
            } else {
                begin_walking_action(m, 8.0f, ACT_WALKING, 0);
            }
        }
    }

    return FALSE;
}

s32 act_finish_turning_around(struct MarioState *m) {
    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (turning_side_flip_window_open(m)
        && (m->input & (INPUT_Z_DOWN | INPUT_A_DOWN)) == (INPUT_Z_DOWN | INPUT_A_DOWN)
        && m->heldObj == NULL && m->controller->stickMag < MARIO_STRONG_STICK_THRESHOLD) {
        return set_jumping_action(m, ACT_BACKFLIP, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        return set_jumping_action(m, turning_side_flip_window_open(m) ? ACT_SIDE_FLIP : ACT_JUMP, 0);
    }

    if (check_ground_dive_or_punch(m)) {
        return TRUE;
    }
    if (m->input & INPUT_Z_DOWN) {
        return set_mario_action(m, ACT_CROUCH_SLIDE, 0);
    }

#ifdef RESET_DIRECTION_WHEN_TURNING_AROUND
    if (analog_stick_held_back(m)) {
        return set_mario_action(m, ACT_TURNING_AROUND, 0);
    }
#endif

    update_walking_speed(m);
    set_mario_animation(m, MARIO_ANIM_TURNING_PART2);

    if (perform_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_WALKING, 0);
    }

#ifdef RESET_DIRECTION_WHEN_TURNING_AROUND
    if (m->input & INPUT_NONZERO_ANALOG) {
        m->marioObj->header.gfx.angle[1] += 0x8000;
    } else {
        m->faceAngle[1] = m->intendedYaw;
        m->marioObj->header.gfx.angle[1] = m->faceAngle[1] + 0x8000;
    }
#else
    m->marioObj->header.gfx.angle[1] += 0x8000;
#endif
    return FALSE;
}

s32 act_braking(struct MarioState *m) {
    if (!(m->input & INPUT_FIRST_PERSON)
        && (m->input
            & (INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE))) {
        return check_common_action_exits(m);
    }

    if (m->zBPressed) {
        return start_moving_crouch_attack(m);
    }

    if (m->input & INPUT_Z_PRESSED) {
        return set_mario_action(m, ACT_CROUCH_SLIDE, 0);
    }

    f32 drag = m->actionArg == ACT_ARG_GROUND_SPEED_FROM_ROLLOUT ? GROUND_NEUTRAL_ROLLOUT_BRAKE_DECEL
                                                                 : GROUND_NEUTRAL_BRAKE_DECEL;
    if (apply_slope_decel(m, drag)) {
        return set_mario_action(m, ACT_BRAKING_STOP, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_MOVE_PUNCHING, 0);
    }

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 0);
            break;

        case GROUND_STEP_NONE:
            m->particleFlags |= PARTICLE_DUST;
            break;

        case GROUND_STEP_HIT_WALL:
            slide_bonk(m, ACT_BACKWARD_GROUND_KB, ACT_BRAKING_STOP);
            break;
    }

    play_sound(SOUND_MOVING_TERRAIN_SLIDE + m->terrainSoundAddend, m->marioObj->header.gfx.cameraToObject);
    adjust_sound_for_speed(m);
    set_mario_animation(m, MARIO_ANIM_SKID_ON_GROUND);
    return FALSE;
}

s32 act_decelerating(struct MarioState *m) {
    s16 slopeClass = mario_get_floor_class(m);

    if (!(m->input & INPUT_FIRST_PERSON)) {
        if (should_begin_sliding(m)) {
            return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
        }

        if (m->input & INPUT_A_PRESSED) {
            return set_jump_from_landing(m);
        }

        if (check_ground_dive_or_punch(m)) {
            return TRUE;
        }

        if (m->input & INPUT_NONZERO_ANALOG) {
            return set_mario_action(m, ACT_WALKING, 0);
        }

        if (m->input & INPUT_Z_PRESSED) {
            return set_mario_action(m, ACT_CROUCH_SLIDE, 0);
        }
    }

    if (update_decelerating_speed(m)) {
        return set_mario_action(m, ACT_IDLE, 0);
    }

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            if (slopeClass == SURFACE_CLASS_VERY_SLIPPERY) {
                mario_bonk_reflection(m, TRUE);
            } else {
                mario_set_forward_vel(m, 0.0f);
            }
            break;
    }

    if (slopeClass == SURFACE_CLASS_VERY_SLIPPERY) {
        set_mario_animation(m, MARIO_ANIM_IDLE_HEAD_LEFT);
        play_sound(SOUND_MOVING_TERRAIN_SLIDE + m->terrainSoundAddend, m->marioObj->header.gfx.cameraToObject);
        adjust_sound_for_speed(m);
        m->particleFlags |= PARTICLE_DUST;
    } else {
        // (Speed Crash) Crashes if speed exceeds 2^17.
        s32 animSpeed = (s32) (absf(m->forwardVel) / 4.0f * 0x10000);
        if (animSpeed < 0x1000) {
            animSpeed = 0x1000;
        }

        set_mario_anim_with_accel(m, MARIO_ANIM_WALKING, animSpeed);
        play_step_sound(m, 10, 49);
    }

    return FALSE;
}

s32 act_hold_decelerating(struct MarioState *m) {
    s16 slopeClass = mario_get_floor_class(m);

    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_WALKING, 0);
    }

    if (should_begin_sliding(m)) {
        return set_mario_action(m, ACT_HOLD_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_THROWING, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        return set_jumping_action(m, ACT_HOLD_JUMP, 0);
    }

    if (m->input & INPUT_Z_PRESSED) {
        return drop_and_set_mario_action(m, ACT_CROUCH_SLIDE, 0);
    }

    if (m->input & INPUT_NONZERO_ANALOG) {
        return set_mario_action(m, ACT_HOLD_WALKING, 0);
    }

    if (update_decelerating_speed(m)) {
        return set_mario_action(m, ACT_HOLD_IDLE, 0);
    }

    m->intendedMag *= 0.4f;

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_HOLD_FREEFALL, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            if (slopeClass == SURFACE_CLASS_VERY_SLIPPERY) {
                mario_bonk_reflection(m, TRUE);
            } else {
                mario_set_forward_vel(m, 0.0f);
            }
            break;
    }

    if (slopeClass == SURFACE_CLASS_VERY_SLIPPERY) {
        set_mario_animation(m, MARIO_ANIM_IDLE_WITH_LIGHT_OBJ);
        play_sound(SOUND_MOVING_TERRAIN_SLIDE + m->terrainSoundAddend, m->marioObj->header.gfx.cameraToObject);
        adjust_sound_for_speed(m);
        m->particleFlags |= PARTICLE_DUST;
    } else {
        //! (Speed Crash) This crashes if Mario has more speed than 2^15 speed.
        s32 animSpeed = (s32)(m->forwardVel * 0x10000);
        if (animSpeed < 0x1000) {
            animSpeed = 0x1000;
        }

        set_mario_anim_with_accel(m, MARIO_ANIM_WALK_WITH_LIGHT_OBJ, animSpeed);
        play_step_sound(m, 12, 62);
    }

    return FALSE;
}

s32 act_riding_shell_ground(struct MarioState *m) {
    s16 startYaw = m->faceAngle[1];

    if (m->input & INPUT_A_PRESSED) {
        return set_mario_action(m, ACT_RIDING_SHELL_JUMP, 0);
    }

    if (m->input & INPUT_Z_PRESSED) {
        mario_stop_riding_object(m);
        if (m->forwardVel < 24.0f) {
            mario_set_forward_vel(m, 24.0f);
        }
        return set_mario_action(m, ACT_CROUCH_SLIDE, 0);
    }

    update_shell_speed(m);
    set_mario_animation(m, m->actionArg == 0 ? MARIO_ANIM_START_RIDING_SHELL : MARIO_ANIM_RIDING_SHELL);

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_RIDING_SHELL_FALL, 0);
            break;

        case GROUND_STEP_HIT_WALL:
            if (mario_keeps_shell(m)) {
                mario_set_forward_vel(m, 0.0f);
                break;
            }
            mario_stop_riding_object(m);
            mario_play_bonk_sound(m);
            m->particleFlags |= PARTICLE_VERTICAL_STAR;
            set_mario_action(m, ACT_BACKWARD_GROUND_KB, 0);
            mario_set_wall_bonk_momentum(m, -m->forwardVel, 0.0f);
            break;
    }

    tilt_body_ground_shell(m, startYaw);
    if (m->floor->type == SURFACE_BURNING) {
        play_sound(SOUND_MOVING_RIDING_SHELL_LAVA, m->marioObj->header.gfx.cameraToObject);
    } else {
        play_sound(SOUND_MOVING_TERRAIN_RIDING_SHELL + m->terrainSoundAddend,
                   m->marioObj->header.gfx.cameraToObject);
    }

    adjust_sound_for_speed(m);
    reset_rumble_timers_slip();
    return FALSE;
}

s32 act_crawling(struct MarioState *m) {
    if (m->floor->normal.y < COS73 && m->floor->type != SURFACE_MARIO_ONLY_GRIPPY) {
        return mario_push_off_steep_floor(m, ACT_FREEFALL, 0);
    }

    if (should_begin_sliding(m)) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    if (m->input & INPUT_FIRST_PERSON) {
        return set_mario_action(m, ACT_STOP_CRAWLING, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        return set_jumping_action(m, ACT_JUMP, 0);
    }

    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_MOVE_PUNCHING, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
    }

    if (m->input & INPUT_IDLE) {
        return set_mario_action(m, ACT_STOP_CRAWLING, 0);
    }

    if (!(m->input & INPUT_Z_DOWN)) {
        return set_mario_action(m, ACT_STOP_CRAWLING, 0);
    }

    m->intendedMag *= CRAWL_TARGET_SPEED / 32.0f;

    update_walking_speed(m);

    if (check_ledge_climb_down(m, TRUE)) {
        return TRUE;
    }

    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            if (check_ledge_climb_down(m, TRUE)) {
                return TRUE;
            }
            return set_mario_action(m, ACT_FREEFALL, 0);

        case GROUND_STEP_HIT_WALL:
            if (m->forwardVel > 10.0f) {
                mario_set_forward_vel(m, 10.0f);
            }
            //! Possibly unintended missing break
            FALL_THROUGH;

        case GROUND_STEP_NONE:
            align_with_floor(m);
            break;
    }

    // One crawl cycle takes about 17 frames at the target speed. Follow the
    // actual velocity so acceleration and slopes do not make the pose race.
    s32 animSpeed =
        (s32) (CLAMP(m->forwardVel, CRAWL_TARGET_SPEED * 0.3f, CRAWL_TARGET_SPEED) * 0.6f * 0x10000);
    set_mario_anim_with_accel(m, MARIO_ANIM_CRAWLING, animSpeed);
    play_step_sound(m, 26, 79);
    return FALSE;
}

s32 act_burning_ground(struct MarioState *m) {
    if (m->input & INPUT_A_PRESSED) {
        return set_mario_action(m, ACT_BURNING_JUMP, 0);
    }

    m->marioObj->oMarioBurnTimer += 2;
    if (m->marioObj->oMarioBurnTimer > MARIO_BURN_TIMER_LIMIT) {
        mario_apply_burning_damage(m, 2);
        return set_mario_action(m, m->health < 0x100 ? ACT_STANDING_DEATH : ACT_WALKING, 0);
    }

    if (m->waterLevel - m->floorHeight > 50.0f) {
        play_sound(SOUND_GENERAL_FLAME_OUT, m->marioObj->header.gfx.cameraToObject);
        return set_mario_action(m, ACT_WALKING, 0);
    }

    if (m->forwardVel < 8.0f) {
        m->forwardVel = 8.0f;
    }
    if (m->forwardVel > 48.0f && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        m->forwardVel = 48.0f;
    }

    m->forwardVel = approach_f32(m->forwardVel, 32.0f, 4.0f, 1.0f);

    if (m->input & INPUT_NONZERO_ANALOG) {
        m->faceAngle[1] = approach_angle(m->faceAngle[1], m->intendedYaw, 0x600);
    }

    apply_slope_accel(m);

    if (perform_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        set_mario_action(m, ACT_BURNING_FALL, 0);
    }

    set_mario_anim_with_accel(m, MARIO_ANIM_RUNNING, (s32) (m->forwardVel / 2.0f * 0x10000));
    play_step_sound(m, 9, 45);

    m->particleFlags |= PARTICLE_FIRE;
    play_sound(SOUND_MOVING_LAVA_BURN, m->marioObj->header.gfx.cameraToObject);

    mario_apply_burning_damage(m, 2);
    if (m->health < 0x100) {
        set_mario_action(m, ACT_STANDING_DEATH, 0);
    }

    m->marioBodyState->eyeState = MARIO_EYES_DEAD;
    reset_rumble_timers_slip();
    return FALSE;
}

void tilt_body_butt_slide(struct MarioState *m) {
    s16 intendedDYaw = m->intendedYaw - m->faceAngle[1];
    f32 mag = DEGREES(30) * m->intendedMag / 32.0f;
    m->marioBodyState->torsoAngle[0] = (s32)(mag * coss(intendedDYaw));
    m->marioBodyState->torsoAngle[2] = (s32)(-(mag * sins(intendedDYaw)));
}

static s32 check_crouch_slide_ledge_grab(struct MarioState *m) {
    if ((m->input & INPUT_NONZERO_ANALOG) && analog_stick_held_back(m)
        && check_ledge_climb_down_internal(m, TRUE, TRUE)) {
        mario_set_forward_vel(m, 0.0f);
        m->slideVelZ = 0.0f;
        m->slideVelX = 0.0f;
        return TRUE;
    }
    return FALSE;
}

void common_slide_action(struct MarioState *m, u32 endAction, u32 airAction, s32 animation) {
    play_sound(SOUND_MOVING_TERRAIN_SLIDE + m->terrainSoundAddend, m->marioObj->header.gfx.cameraToObject);

    reset_rumble_timers_slip();

    adjust_sound_for_speed(m);

    s32 guardCrouchLedge = m->action == ACT_CROUCH_SLIDE
                           && (!(m->input & INPUT_NONZERO_ANALOG) || analog_stick_held_back(m));
    s32 stepResult = guardCrouchLedge ? perform_ground_step_stop_at_ledge(m) : perform_ground_step(m);

    switch (stepResult) {
        case GROUND_STEP_LEFT_GROUND:
            if (guardCrouchLedge) {
                if (check_crouch_slide_ledge_grab(m)) {
                    return;
                }
                mario_set_forward_vel(m, 0.0f);
                m->slideVelZ = 0.0f;
                m->slideVelX = 0.0f;
                set_mario_action(m, endAction, 0);
                return;
            }
            set_mario_action(m, airAction, 0);
            if (m->forwardVel < -50.0f || 50.0f < m->forwardVel) {
                play_sound(SOUND_MARIO_HOOHOO, m->marioObj->header.gfx.cameraToObject);
            }
            break;

        case GROUND_STEP_NONE:
            set_mario_animation(m, animation);
            align_with_floor(m);
            m->particleFlags |= PARTICLE_DUST;
            break;

        case GROUND_STEP_HIT_WALL:
            if (!mario_floor_is_slippery(m)) {
                if (m->forwardVel > 16.0f) {
                    m->particleFlags |= PARTICLE_VERTICAL_STAR;
                }
                slide_bonk(m, ACT_GROUND_BONK, endAction);
            } else if (m->wall != NULL) {
                s16 wallAngle = m->wallYaw;
                f32 slideSpeed = sqrtf(sqr(m->slideVelX) + sqr(m->slideVelZ)) * 0.9f;

                if (slideSpeed < 4.0f) {
                    slideSpeed = 4.0f;
                }

                m->slideYaw = wallAngle - (s16)(m->slideYaw - wallAngle) + 0x8000;

                m->vel[0] = m->slideVelX = slideSpeed * sins(m->slideYaw);
                m->vel[2] = m->slideVelZ = slideSpeed * coss(m->slideYaw);
            }

            align_with_floor(m);
            break;
    }
}

s32 common_slide_action_with_jump(struct MarioState *m, u32 stopAction, u32 jumpAction, u32 airAction,
                                  s32 animation, f32 stopSpeed) {
    s32 buttSlide = m->action == ACT_BUTT_SLIDE || m->action == ACT_HOLD_BUTT_SLIDE;
    s32 backwardButtSlideJump = buttSlide && m->forwardVel < 0.0f;
    u32 jumpDelay = 6;
    s32 slope = m->floor != NULL && mario_floor_is_slope(m);
    if (!slope) {
        m->backwardSlopeSlideCount = 0;
    }
    if (backwardButtSlideJump) {
        jumpDelay = m->backwardButtSlideJumpCount >= 2 ? 9 : 3 * (m->backwardButtSlideJumpCount + 1);
    } else if (slope) {
        jumpDelay = buttSlide ? 9 : 6;
    }
    u32 jumpArg = backwardButtSlideJump ? ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE : 0;
    if (jumpArg != 0 && slope) {
        // A backward jump from a slide may turn uphill after takeoff even
        // when its initial slide velocity points downhill.
        jumpArg |= ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL;
    }
#ifdef SLOPE_BUFFER
    if (m->input & INPUT_A_PRESSED) {
        m->actionState = 1;
    } else if (m->framesSinceA >= 6) {
        m->actionState = 0;
    }
    if (m->actionTimer >= jumpDelay) {
        if (m->actionState == 1) {
            if (backwardButtSlideJump && m->backwardButtSlideJumpCount < 3) {
                m->backwardButtSlideJumpCount++;
            }
            return set_jumping_action(m, jumpAction, jumpArg);
        }
    } else {
        m->actionTimer++;
    }
#else
    if (m->actionTimer >= jumpDelay) {
        if (m->input & INPUT_A_PRESSED) {
            if (backwardButtSlideJump && m->backwardButtSlideJumpCount < 3) {
                m->backwardButtSlideJumpCount++;
            }
            return set_jumping_action(m, jumpAction, jumpArg);
        }
    } else {
        m->actionTimer++;
    }
#endif

    // Crawling probes before moving; give a held-back crouch slide the same
    // chance before friction can end the slide on the last supported floor.
    if (m->action == ACT_CROUCH_SLIDE && check_crouch_slide_ledge_grab(m)) {
        return TRUE;
    }

    if (update_sliding(m, stopSpeed)) {
        return set_mario_action(m, stopAction, 0);
    }

    common_slide_action(m, stopAction, airAction, animation);
    return FALSE;
}

s32 act_butt_slide(struct MarioState *m) {
    s32 cancel = common_slide_action_with_jump(m, ACT_BUTT_SLIDE_STOP, ACT_JUMP, ACT_BUTT_SLIDE_AIR,
                                               MARIO_ANIM_SLIDE, 4.0f);
    tilt_body_butt_slide(m);
    return cancel;
}

s32 act_hold_butt_slide(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_BUTT_SLIDE, 0);
    }

    s32 cancel = common_slide_action_with_jump(m, ACT_HOLD_BUTT_SLIDE_STOP, ACT_HOLD_JUMP,
                                               ACT_HOLD_BUTT_SLIDE_AIR,
                                               MARIO_ANIM_SLIDING_ON_BOTTOM_WITH_LIGHT_OBJ, 4.0f);
    tilt_body_butt_slide(m);
    return cancel;
}

s32 act_crouch_slide(struct MarioState *m) {
    s32 backflipPressed =
        (m->input & INPUT_A_PRESSED) && (m->input & INPUT_NONZERO_ANALOG) && analog_stick_held_back(m);

    if (backflipPressed) {
        m->crouchSlideBackflip = TRUE;
    } else if ((m->input & INPUT_A_PRESSED) || !(m->input & INPUT_A_DOWN)) {
        // Releasing A cancels the queue; a fresh jump request replaces it.
        m->crouchSlideBackflip = FALSE;
    }
    if (m->crouchSlideFrames >= 1
        && (backflipPressed
            || (m->crouchSlideBackflip
                && !(m->input & (INPUT_B_PRESSED | INPUT_FIRST_PERSON | INPUT_ABOVE_SLIDE))))) {
        return set_jumping_action(m, ACT_BACKFLIP, 0);
    }

    if (m->input & INPUT_ABOVE_SLIDE) {
        if ((m->input & INPUT_A_PRESSED) && !mario_has_dive_input(m)) {
            m->input &= ~INPUT_A_PRESSED;
        }
        return set_mario_action(m, ACT_BUTT_SLIDE, 0);
    }

    if (m->actionTimer < 30) {
        m->actionTimer++;
        if (m->input & INPUT_A_PRESSED) {
            if (mario_has_dive_input(m)) {
                return set_jumping_action(m, ACT_LONG_JUMP, 0);
            }
        }
    }

    if (m->input & INPUT_B_PRESSED) {
        return start_moving_crouch_attack(m);
    }

    if (m->input & INPUT_A_PRESSED) {
        if (backflipPressed) {
            // Keep the startup backflip queue without letting the shared slide
            // handler turn the same A press into an ordinary jump.
            m->input &= ~INPUT_A_PRESSED;
        } else {
            // Neutral held-Z crouching keeps its backflip while momentum remains.
            u32 jumpAction = !(m->input & INPUT_NONZERO_ANALOG) && (m->input & INPUT_Z_DOWN)
                                 ? ACT_BACKFLIP
                                 : ACT_JUMP;
            return set_jumping_action(m, jumpAction, 0);
        }
    }

    if (m->input & INPUT_FIRST_PERSON) {
        return set_mario_action(m, ACT_BRAKING, 0);
    }

    if (!(m->input & INPUT_Z_DOWN)) {
        m->zCrouchBuffered = FALSE;
        return set_mario_action(m, ACT_STOP_CROUCHING, 0);
    }

    // actionTimer also advances in the shared slide handler. Count completed
    // updates separately for the backflip startup gate.
    if (m->crouchSlideFrames < 5) {
        m->crouchSlideFrames++;
    }

    f32 stopSpeed = m->prevAction == ACT_MOVE_PUNCHING ? 0.5f : 4.0f;
    // A short slide can stop before the shared handler selects its animation.
    // Start it now, then carry its frame into stationary entry if unfinished.
    set_mario_animation(m, MARIO_ANIM_START_CROUCHING);
    u32 stopAction = is_anim_at_end(m) ? ACT_CROUCHING : ACT_START_CROUCHING;
    return common_slide_action_with_jump(m, stopAction, ACT_JUMP, ACT_FREEFALL,
                                         MARIO_ANIM_START_CROUCHING, stopSpeed);
}

s32 act_slide_kick_slide(struct MarioState *m) {
    if (m->actionArg > 0 || (m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED))) {
        queue_rumble_data(5, 80);
        return set_jumping_action(m, ACT_FORWARD_ROLLOUT, ACT_ARG_FORWARD_ROLLOUT_FROM_SLIDE_KICK);
    }

    // Grounded recovery starts after the initial bounce; rollout inputs still take priority.
    if (should_begin_sliding(m)) {
        return set_mario_action(m, ACT_BUTT_SLIDE, 0);
    }

    set_mario_animation(m, MARIO_ANIM_SLIDE_KICK);
    if (is_anim_at_end(m) && absf(m->forwardVel) < 1.0f) {
        return set_mario_action(m, ACT_SLIDE_KICK_SLIDE_STOP, 0);
    }

    update_sliding(m, 1.0f);
    switch (perform_ground_step(m)) {
        case GROUND_STEP_LEFT_GROUND:
            set_mario_action(m, ACT_FREEFALL, 2);
            break;

        case GROUND_STEP_HIT_WALL:
            mario_attack_breakable_box_wall(m);
            mario_bonk_reflection(m, TRUE);
            m->particleFlags |= PARTICLE_VERTICAL_STAR;
            set_mario_action(m, ACT_BACKWARD_GROUND_KB, 0);
            mario_set_wall_bonk_momentum(m, m->forwardVel, 0.0f);
            break;
    }

    play_sound(SOUND_MOVING_TERRAIN_SLIDE + m->terrainSoundAddend, m->marioObj->header.gfx.cameraToObject);
    m->particleFlags |= PARTICLE_DUST;
    return FALSE;
}

static s32 can_rollout_from_belly_slide(struct MarioState *m) {
    if (!(m->input & INPUT_ABOVE_SLIDE)) {
        return TRUE;
    }
    if (m->floor == NULL || m->forwardVel <= 0.0f) {
        return FALSE;
    }

    f32 facingX = sins(m->faceAngle[1]);
    f32 facingZ = coss(m->faceAngle[1]);
    f32 forwardTravel = m->slideVelX * facingX + m->slideVelZ * facingZ;
    f32 downhillTravel = m->slideVelX * m->floor->normal.x + m->slideVelZ * m->floor->normal.z;
    // Rollout air movement follows facing, so both the slide and launch must
    // head downhill. A sideways slide while facing uphill cannot bypass this.
    f32 downhillFacing = facingX * m->floor->normal.x + facingZ * m->floor->normal.z;
    return forwardTravel > 0.0f && downhillTravel > 0.0f && downhillFacing > 0.0f;
}

#define BELLY_SLIDE_FORWARD_STICK_DEADZONE 4.0f

static s32 belly_slide_forward_stick_held(struct MarioState *m) {
    return (m->input & INPUT_NONZERO_ANALOG)
           && m->controller->stickMag * coss(m->intendedYaw - m->faceAngle[1])
                  > BELLY_SLIDE_FORWARD_STICK_DEADZONE;
}

s32 stomach_slide_action(struct MarioState *m, u32 stopAction, u32 airAction, s32 animation) {
    if ((m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED)) && can_rollout_from_belly_slide(m)) {
        queue_rumble_data(5, 80);
        return drop_and_set_mario_action(
            m, m->forwardVel >= 0.0f ? ACT_FORWARD_ROLLOUT : ACT_BACKWARD_ROLLOUT, 0);
    }

    // Stand up at the existing threshold without discarding the remaining slide.
    update_sliding(m, 0.0f);
    if (!mario_floor_is_slope(m) && absf(m->forwardVel) < 4.0f && !belly_slide_forward_stick_held(m)) {
        return set_mario_action(m, stopAction, 0);
    }

    common_slide_action(m, stopAction, airAction, animation);
    return FALSE;
}

s32 act_stomach_slide(struct MarioState *m) {
    return stomach_slide_action(m, ACT_STOMACH_SLIDE_STOP, ACT_FREEFALL, MARIO_ANIM_SLIDE_DIVE);
}

s32 act_hold_stomach_slide(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_STOMACH_SLIDE, 0);
    }

    return stomach_slide_action(m, ACT_DIVE_PICKING_UP, ACT_HOLD_FREEFALL, MARIO_ANIM_SLIDE_DIVE);
}

s32 act_dive_slide(struct MarioState *m) {
    if (m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED)) {
        m->actionArg = 6;
    }

    if (m->actionArg > 0 && can_rollout_from_belly_slide(m)) {
        queue_rumble_data(5, 80);
        return set_mario_action(m, m->forwardVel > 0.0f ? ACT_FORWARD_ROLLOUT : ACT_BACKWARD_ROLLOUT,
                                0);
    }

    if (m->actionArg > 0) {
        m->actionArg--;
    }

    // Interactions run before actions each frame, so timer values 0 and 1
    // cover the first two slide contacts after landing.
    if (m->actionTimer < DIVE_SLIDE_ATTACK_FRAMES) {
        m->actionTimer++;
    }

    play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);

    //! If the dive slide ends on the same frame that we pick up on object,
    // Mario will not be in the dive slide action for the call to
    // mario_check_object_grab, and so will end up in the regular picking action,
    // rather than the picking up after dive action.

    update_sliding(m, 0.0f);
    if (!mario_floor_is_slope(m) && absf(m->forwardVel) < 8.0f && is_anim_at_end(m)
        && !belly_slide_forward_stick_held(m)) {
        set_mario_action(m, ACT_STOMACH_SLIDE_STOP, 0);
    }

    if (mario_check_object_grab(m)) {
        mario_grab_used_object(m);
        m->marioBodyState->grabPos = GRAB_POS_LIGHT_OBJ;
        return TRUE;
    }

    common_slide_action(m, ACT_STOMACH_SLIDE_STOP, ACT_FREEFALL, MARIO_ANIM_DIVE);
    return FALSE;
}

#define GROUND_TWIRL_IMMUNE_REBOUND_ANIM_SPEED 0x18000 // 1.5x animation and recovery speed.

s32 common_ground_knockback_action(struct MarioState *m, s32 animation, s32 checkFrame, s32 playLandingSound, s32 actionArg) {
    s32 quickTwirlRebound =
        (m->action == ACT_SOFT_BACKWARD_GROUND_KB || m->action == ACT_SOFT_FORWARD_GROUND_KB)
        && m->actionState == GROUND_TWIRL_IMMUNE_REBOUND_STATE;
    s32 earlyBonkRecovery = m->health >= 0x100 && (m->action == ACT_GROUND_BONK || m->wallBonkMomentum);
    if (playLandingSound) {
        play_mario_heavy_landing_sound_once(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
    }

    if (actionArg > 0) {
        play_sound_if_no_flag(m, SOUND_MARIO_ATTACKED, MARIO_MARIO_SOUND_PLAYED);
    } else {
        play_sound_if_no_flag(m, SOUND_MARIO_OOOF2, MARIO_MARIO_SOUND_PLAYED);
    }

    if (!m->wallBonkMomentum && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        if (m->forwardVel > 32.0f) {
            m->forwardVel = 32.0f;
        }
        if (m->forwardVel < -32.0f) {
            m->forwardVel = -32.0f;
        }
    }

    s32 animFrame =
        quickTwirlRebound
            ? set_mario_anim_with_accel(m, animation, GROUND_TWIRL_IMMUNE_REBOUND_ANIM_SPEED)
            : set_mario_animation(m, animation);
    if (animFrame < checkFrame || m->wallBonkMomentum) {
        apply_landing_accel(m, 0.9f);
    } else if (m->forwardVel >= 0.0f) {
        mario_set_forward_vel(m, 0.1f);
    } else {
        mario_set_forward_vel(m, -0.1f);
    }

    if (perform_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        s32 wallBonkMomentum = m->wallBonkMomentum;
        if (m->forwardVel >= 0.0f) {
            set_mario_action(m, ACT_FORWARD_AIR_KB, actionArg);
        } else {
            set_mario_action(m, ACT_BACKWARD_AIR_KB, actionArg);
        }
        m->wallBonkMomentum = wallBonkMomentum;
    } else if ((quickTwirlRebound || earlyBonkRecovery) ? is_anim_past_end(m) : is_anim_at_end(m)) {
        if (m->health < 0x100) {
            set_mario_action(m, ACT_STANDING_DEATH, 0);
        } else {
            if (actionArg > 0) {
                m->invincTimer = 30;
            }
            if (m->bullyKnockbackBuffer & INPUT_A_PRESSED) {
                set_jumping_action(m, ACT_JUMP, 0);
            } else if (m->bullyKnockbackBuffer & INPUT_B_PRESSED) {
                set_mario_action(m, ACT_PUNCHING, 0);
            } else {
                set_mario_action(m, ACT_IDLE, 0);
            }
        }
    }

    return animFrame;
}

s32 act_hard_backward_ground_kb(struct MarioState *m) {
    s32 animFrame =
        common_ground_knockback_action(m, MARIO_ANIM_FALL_OVER_BACKWARDS, 43, TRUE, m->actionArg);
    if (animFrame == 43 && m->health < 0x100) {
        set_mario_action(m, ACT_DEATH_ON_BACK, 0);
    }

    if (animFrame == 54 && m->prevAction == ACT_SPECIAL_DEATH_EXIT) {
        play_sound(SOUND_MARIO_MAMA_MIA, m->marioObj->header.gfx.cameraToObject);
    }

    if (animFrame == 69) {
        play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_LANDING);
    }

    return FALSE;
}

s32 act_hard_forward_ground_kb(struct MarioState *m) {
    s32 animFrame =
        common_ground_knockback_action(m, MARIO_ANIM_LAND_ON_STOMACH, 21, TRUE, m->actionArg);
    if (animFrame == 23 && m->health < 0x100) {
        set_mario_action(m, ACT_DEATH_ON_STOMACH, 0);
    }

    return FALSE;
}

s32 act_backward_ground_kb(struct MarioState *m) {
    common_ground_knockback_action(m, MARIO_ANIM_BACKWARD_KB, 22, TRUE, m->actionArg);
    return FALSE;
}

s32 act_forward_ground_kb(struct MarioState *m) {
    common_ground_knockback_action(m, MARIO_ANIM_FORWARD_KB, 20, TRUE, m->actionArg);
    return FALSE;
}

s32 act_soft_backward_ground_kb(struct MarioState *m) {
    common_ground_knockback_action(m, MARIO_ANIM_SOFT_BACK_KB, 100, FALSE, m->actionArg);
    return FALSE;
}

s32 act_soft_forward_ground_kb(struct MarioState *m) {
    common_ground_knockback_action(m, MARIO_ANIM_SOFT_FRONT_KB, 100, FALSE, m->actionArg);
    return FALSE;
}

s32 act_ground_bonk(struct MarioState *m) {
    s32 animFrame =
        common_ground_knockback_action(m, MARIO_ANIM_GROUND_BONK, 32, TRUE, m->actionArg);
    if (animFrame == 32) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
    }
    return FALSE;
}

s32 act_death_exit_land(struct MarioState *m) {
    s32 animFrame;

    apply_landing_accel(m, 0.9f);
    play_mario_heavy_landing_sound_once(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);

    animFrame = set_mario_animation(m, MARIO_ANIM_FALL_OVER_BACKWARDS);

    if (animFrame == 54) {
        play_sound(SOUND_MARIO_MAMA_MIA, m->marioObj->header.gfx.cameraToObject);
    }
    if (animFrame == 68) {
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
    }

    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_IDLE, 0);
    }

    return FALSE;
}

u32 common_landing_action(struct MarioState *m, s16 animation, u32 airAction) {
    u32 stepResult;

    if (m->input & INPUT_NONZERO_ANALOG) {
        if (should_brake_negative_ground_speed(m)) {
            m->forwardVel = MIN(0.0f, m->forwardVel + WALK_STICK_NEGATIVE_DECEL);
        }
        apply_landing_accel(m, 0.98f);
    } else if (absf(m->forwardVel) >= 16.0f) {
        apply_slope_decel(m, GROUND_NEUTRAL_BRAKE_DECEL);
    } else if (m->forwardVel != 0.0f) {
        update_decelerating_speed(m);
        m->vel[1] = 0.0f;
    } else {
        m->vel[1] = 0.0f;
    }

    stepResult = perform_ground_step(m);
    if (stepResult == GROUND_STEP_LEFT_GROUND) {
        set_mario_action(m, airAction, 0);
        set_mario_animation(m, airAction == ACT_HOLD_FREEFALL ? MARIO_ANIM_FALL_WITH_LIGHT_OBJ
                                                              : MARIO_ANIM_GENERAL_FALL);
        return stepResult;
    }

    if (m->forwardVel > 16.0f) {
        m->particleFlags |= PARTICLE_DUST;
    }

    set_mario_animation(m, animation);
    play_mario_landing_sound_once(m, SOUND_ACTION_TERRAIN_LANDING);

    if (!game_options_cheat_active(GO_IGNORE_SAND) && m->floor->type >= SURFACE_SHALLOW_QUICKSAND
        && m->floor->type <= SURFACE_MOVING_QUICKSAND) {
        m->quicksandDepth += (4 - m->actionTimer) * 3.5f - 0.5f;
    }

    return stepResult;
}

s32 common_landing_cancels(struct MarioState *m, struct LandingAction *landingAction,
                           s32 (*setAPressAction)(struct MarioState *m, u32 action, u32 actionArg)) {
    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, landingAction->offFloorAction, 0);
    }
    if (m->input & INPUT_STOMPED) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->floor->normal.y < COS73 && m->floor->type != SURFACE_MARIO_ONLY_GRIPPY) {
        return mario_push_off_steep_floor(m, landingAction->verySteepAction, 0);
    }

    m->doubleJumpTimer = landingAction->doubleJumpTimer;

    if (should_begin_sliding(m)) {
        return set_mario_action(m, landingAction->slideAction, 0);
    }

    if (m->input & INPUT_FIRST_PERSON) {
        return set_mario_action(m, landingAction->endAction, 0);
    }

    if (m->action == ACT_FREEFALL_LAND && m->prevAction == ACT_L_AIR_TWIRL) {
        if (m->automaticAFrames > 0) {
            m->input |= INPUT_A_PRESSED;
        }
        if (m->automaticBFrames > 0) {
            m->input |= INPUT_B_PRESSED;
        }
        if (m->automaticZFrames > 0) {
            m->input |= INPUT_Z_DOWN;
        }
    }

    if ((m->input & INPUT_A_PRESSED)
        || ((m->action == ACT_JUMP_LAND || m->action == ACT_DOUBLE_JUMP_LAND
             || m->action == ACT_TRIPLE_JUMP_LAND || m->action == ACT_SIDE_FLIP_LAND
             || m->action == ACT_LONG_JUMP_LAND || m->action == ACT_FREEFALL_LAND
             || m->action == ACT_HOLD_FREEFALL_LAND || m->action == ACT_HOLD_JUMP_LAND)
            && m->actionArg > 0)) {
        u32 jump = landing_jump_action(m, landingAction->aPressedAction);
        // A crouch jump takes priority over a double/triple-jump chain.
        if (jump != landingAction->aPressedAction) {
            return set_jumping_action(m, jump, 0);
        }
        return setAPressAction(m, jump, 0);
    }
    if (check_landing_attack_or_crouch(m)) {
        return TRUE;
    }

    if (++m->actionTimer >= landingAction->numFrames) {
        if (!(m->input & INPUT_NONZERO_ANALOG) && absf(m->forwardVel) >= 1.0f) {
            u32 decelAction = (landingAction->endAction == ACT_HOLD_JUMP_LAND_STOP
                               || landingAction->endAction == ACT_HOLD_FREEFALL_LAND_STOP)
                                  ? ACT_HOLD_DECELERATING
                                  : ACT_DECELERATING;
            return set_mario_action(m, decelAction, 0);
        }
        return set_mario_action(m, landingAction->endAction, 0);
    }

    return FALSE;
}

s32 act_jump_land(struct MarioState *m) {
    if (common_landing_cancels(m, &sJumpLandAction,
                               m->wallKickFromDoubleJump ? set_triple_jump_action
                                                         : set_jumping_action)) {
        return TRUE;
    }

    common_landing_action(m, MARIO_ANIM_LAND_FROM_SINGLE_JUMP, ACT_FREEFALL);
    return FALSE;
}

s32 act_freefall_land(struct MarioState *m) {
    if (common_landing_cancels(m, &sFreefallLandAction, set_jumping_action)) {
        return TRUE;
    }

    common_landing_action(m, MARIO_ANIM_GENERAL_LAND, ACT_FREEFALL);
    return FALSE;
}

s32 act_side_flip_land(struct MarioState *m) {
    if (common_landing_cancels(m, &sSideFlipLandAction, set_jumping_action)) {
        return TRUE;
    }

    if (common_landing_action(m, MARIO_ANIM_SLIDEFLIP_LAND, ACT_FREEFALL) != GROUND_STEP_HIT_WALL) {
        m->marioObj->header.gfx.angle[1] += 0x8000;
    }
    return FALSE;
}

s32 act_hold_jump_land(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_JUMP_LAND_STOP, 0);
    }

    if (common_landing_cancels(m, &sHoldJumpLandAction, set_jumping_action)) {
        return TRUE;
    }

    common_landing_action(m, MARIO_ANIM_JUMP_LAND_WITH_LIGHT_OBJ, ACT_HOLD_FREEFALL);
    return FALSE;
}

s32 act_hold_freefall_land(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROP_OBJECT) {
        return drop_and_set_mario_action(m, ACT_FREEFALL_LAND_STOP, 0);
    }

    if (common_landing_cancels(m, &sHoldFreefallLandAction, set_jumping_action)) {
        return TRUE;
    }

    common_landing_action(m, MARIO_ANIM_FALL_LAND_WITH_LIGHT_OBJ, ACT_HOLD_FREEFALL);
    return FALSE;
}

s32 act_long_jump_land(struct MarioState *m) {
#ifdef DISABLE_BLJ
    // BLJ (Backwards Long Jump) speed build up fix, crushing SimpleFlips's dreams since July 1997
    if (m->forwardVel < 0.0f) {
        m->forwardVel = 0.0f;
    }
#endif

    if (common_landing_cancels(m, &sLongJumpLandAction, set_jumping_action)) {
        return TRUE;
    }

    if (!(m->input & INPUT_NONZERO_ANALOG)) {
        play_sound_if_no_flag(m, SOUND_MARIO_UH_LONG_JUMP_LAND, MARIO_MARIO_SOUND_PLAYED);
    }

    common_landing_action(m,
                          !m->marioObj->oMarioLongJumpIsSlow ? MARIO_ANIM_CROUCH_FROM_FAST_LONGJUMP
                                                             : MARIO_ANIM_CROUCH_FROM_SLOW_LONGJUMP,
                          ACT_FREEFALL);
    return FALSE;
}

s32 act_double_jump_land(struct MarioState *m) {
    if (common_landing_cancels(m, &sDoubleJumpLandAction, set_triple_jump_action)) {
        return TRUE;
    }
    common_landing_action(m, MARIO_ANIM_LAND_FROM_DOUBLE_JUMP, ACT_FREEFALL);
    return FALSE;
}

s32 act_triple_jump_land(struct MarioState *m) {
    if (common_landing_cancels(m, &sTripleJumpLandAction, set_jumping_action)) {
        return TRUE;
    }

    if (!(m->input & INPUT_NONZERO_ANALOG)) {
        play_sound_if_no_flag(m, SOUND_MARIO_HAHA, MARIO_MARIO_SOUND_PLAYED);
    }

    common_landing_action(m, MARIO_ANIM_TRIPLE_JUMP_LAND, ACT_FREEFALL);
    return FALSE;
}

s32 act_backflip_land(struct MarioState *m) {
    if ((m->input & INPUT_Z_DOWN)
        && (m->framesSinceA < MARIO_BACKFLIP_LANDING_BUFFER_FRAMES || m->actionArg > 0)
        && !(m->input & (INPUT_OFF_FLOOR | INPUT_STOMPED | INPUT_FIRST_PERSON))) {
        m->framesSinceA = 0xFF;
        return set_jumping_action(m, ACT_BACKFLIP, 0);
    }
    if (m->actionArg > 0 && !(m->input & INPUT_Z_DOWN)
        && !(m->input & (INPUT_OFF_FLOOR | INPUT_STOMPED | INPUT_FIRST_PERSON))) {
        return set_jumping_action(m, ACT_JUMP, 0);
    }
    if (common_landing_cancels(m, &sBackflipLandAction, set_jumping_action)) {
        return TRUE;
    }

    if (!(m->input & INPUT_NONZERO_ANALOG)) {
        play_sound_if_no_flag(m, SOUND_MARIO_HAHA, MARIO_MARIO_SOUND_PLAYED);
    }

    common_landing_action(m, MARIO_ANIM_TRIPLE_JUMP_LAND, ACT_FREEFALL);
    return FALSE;
}

s32 quicksand_jump_land_action(struct MarioState *m, s32 animation1, s32 animation2, u32 endAction,
                               u32 airAction) {
    if (m->actionTimer++ < 6) {
        m->quicksandDepth -= (7 - m->actionTimer) * 0.8f;
        if (m->quicksandDepth < 1.0f) {
            m->quicksandDepth = 1.1f;
        }

        play_mario_jump_sound(m);
        set_mario_animation(m, animation1);
    } else {
        if (m->actionTimer >= 13) {
            return set_mario_action(m, endAction, 0);
        }

        set_mario_animation(m, animation2);
    }

    apply_landing_accel(m, 0.95f);
    if (perform_ground_step(m) == GROUND_STEP_LEFT_GROUND) {
        set_mario_action(m, airAction, 0);
    }

    return FALSE;
}

s32 act_quicksand_jump_land(struct MarioState *m) {
    return quicksand_jump_land_action(m, MARIO_ANIM_SINGLE_JUMP,
                                         MARIO_ANIM_LAND_FROM_SINGLE_JUMP,
                                         ACT_JUMP_LAND_STOP,
                                         ACT_FREEFALL);
}

s32 act_hold_quicksand_jump_land(struct MarioState *m) {
    return quicksand_jump_land_action(m, MARIO_ANIM_JUMP_WITH_LIGHT_OBJ,
                                         MARIO_ANIM_JUMP_LAND_WITH_LIGHT_OBJ,
                                         ACT_HOLD_JUMP_LAND_STOP,
                                         ACT_HOLD_FREEFALL);
}

s32 check_common_moving_cancels(struct MarioState *m) {
    s32 invulnerable = (m->action & ACT_FLAG_INVULNERABLE)
                       && (m->action != ACT_GROUND_TWIRL || mario_ground_twirl_is_active(m));
    if (m->pos[1] < m->waterLevel - 100 && !mario_has_land_movement_underwater(m)
        && !mario_is_riding_underwater_shell(m)) {
        return set_water_plunge_action(m);
    }

    if (!invulnerable && (m->input & INPUT_STOMPED)) {
        return drop_and_set_mario_action(m, ACT_SHOCKWAVE_BOUNCE, 0);
    }

    if (m->input & INPUT_SQUISHED) {
        return drop_and_set_mario_action(m, ACT_SQUISHED, 0);
    }

    if (!invulnerable) {
        if (m->health < 0x100) {
            return drop_and_set_mario_action(m, ACT_STANDING_DEATH, 0);
        }
    }

    if (mario_transition_underwater_shell_to_swimming(m)) {
        return TRUE;
    }

    return FALSE;
}

s32 mario_execute_moving_action(struct MarioState *m) {
    s32 cancel = FALSE;

    if (check_common_moving_cancels(m)) {
        return TRUE;
    }

    if (mario_update_quicksand(m, 0.25f)) {
        return TRUE;
    }

    /* clang-format off */
    switch (m->action) {
        case ACT_WALKING:                  cancel = act_walking(m);                  break;
        case ACT_GROUND_TWIRL:
            cancel = mario_ground_twirl_action(m);
            break;
        case ACT_HOLD_WALKING:             cancel = act_hold_walking(m);             break;
        case ACT_HOLD_HEAVY_WALKING:       cancel = act_hold_heavy_walking(m);       break;
        case ACT_TURNING_AROUND:           cancel = act_turning_around(m);           break;
        case ACT_FINISH_TURNING_AROUND:    cancel = act_finish_turning_around(m);    break;
        case ACT_BRAKING:                  cancel = act_braking(m);                  break;
        case ACT_RIDING_SHELL_GROUND:      cancel = act_riding_shell_ground(m);      break;
        case ACT_CRAWLING:                 cancel = act_crawling(m);                 break;
        case ACT_BURNING_GROUND:           cancel = act_burning_ground(m);           break;
        case ACT_DECELERATING:             cancel = act_decelerating(m);             break;
        case ACT_HOLD_DECELERATING:        cancel = act_hold_decelerating(m);        break;
        case ACT_BUTT_SLIDE:               cancel = act_butt_slide(m);               break;
        case ACT_STOMACH_SLIDE:            cancel = act_stomach_slide(m);            break;
        case ACT_HE_THICC_BELLY_SLIDE:
            cancel = mario_he_thicc_belly_slide(m);
            break;
        case ACT_HOLD_BUTT_SLIDE:          cancel = act_hold_butt_slide(m);          break;
        case ACT_HOLD_STOMACH_SLIDE:       cancel = act_hold_stomach_slide(m);       break;
        case ACT_DIVE_SLIDE:               cancel = act_dive_slide(m);               break;
        case ACT_MOVE_PUNCHING:            cancel = act_move_punching(m);            break;
        case ACT_CROUCH_SLIDE:             cancel = act_crouch_slide(m);             break;
        case ACT_SLIDE_KICK_SLIDE:         cancel = act_slide_kick_slide(m);         break;
        case ACT_HARD_BACKWARD_GROUND_KB:  cancel = act_hard_backward_ground_kb(m);  break;
        case ACT_HARD_FORWARD_GROUND_KB:   cancel = act_hard_forward_ground_kb(m);   break;
        case ACT_BACKWARD_GROUND_KB:       cancel = act_backward_ground_kb(m);       break;
        case ACT_FORWARD_GROUND_KB:        cancel = act_forward_ground_kb(m);        break;
        case ACT_SOFT_BACKWARD_GROUND_KB:  cancel = act_soft_backward_ground_kb(m);  break;
        case ACT_SOFT_FORWARD_GROUND_KB:   cancel = act_soft_forward_ground_kb(m);   break;
        case ACT_GROUND_BONK:              cancel = act_ground_bonk(m);              break;
        case ACT_DEATH_EXIT_LAND:          cancel = act_death_exit_land(m);          break;
        case ACT_JUMP_LAND:                cancel = act_jump_land(m);                break;
        case ACT_FREEFALL_LAND:            cancel = act_freefall_land(m);            break;
        case ACT_DOUBLE_JUMP_LAND:         cancel = act_double_jump_land(m);         break;
        case ACT_SIDE_FLIP_LAND:           cancel = act_side_flip_land(m);           break;
        case ACT_HOLD_JUMP_LAND:           cancel = act_hold_jump_land(m);           break;
        case ACT_HOLD_FREEFALL_LAND:       cancel = act_hold_freefall_land(m);       break;
        case ACT_TRIPLE_JUMP_LAND:         cancel = act_triple_jump_land(m);         break;
        case ACT_BACKFLIP_LAND:            cancel = act_backflip_land(m);            break;
        case ACT_QUICKSAND_JUMP_LAND:      cancel = act_quicksand_jump_land(m);      break;
        case ACT_HOLD_QUICKSAND_JUMP_LAND: cancel = act_hold_quicksand_jump_land(m); break;
        case ACT_LONG_JUMP_LAND:           cancel = act_long_jump_land(m);           break;
    }
    /* clang-format on */

    if (!cancel && (m->input & INPUT_IN_WATER)) {
        m->particleFlags |= PARTICLE_WAVE_TRAIL;
        m->particleFlags &= ~PARTICLE_DUST;
    }

    return cancel;
}
