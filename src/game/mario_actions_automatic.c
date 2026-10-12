#include <PR/ultratypes.h>

#include "sm64.h"
#include "mario_actions_automatic.h"
#include "mario_actions_airborne.h"
#include "behavior_data.h"
#include "audio/external.h"
#include "area.h"
#include "mario.h"
#include "game_options.h"
#include "game_options_runtime.h"
#include "mario_step.h"
#include "engine/math_util.h"
#include "memory.h"
#include "engine/graph_node.h"
#include "save_file.h"
#include "engine/surface_collision.h"
#include "interaction.h"
#include "camera.h"
#include "level_table.h"
#include "rumble_init.h"

#include "config.h"

#define HANG_DISTANCE 144.0f
#define HANG_GRAB_MOMENTUM_RETENTION 0.5f
#define HANG_GRAB_MAX_SPEED 24.0f
#define HANG_GRAB_COAST_DECEL 2.0f
#define POLE_SPEED_MULTIPLIER 1.3f
#define POLE_DESCENT_ANGLE_VEL_CAP 0x14CD // 1.3 * vanilla's 0x1000, rounded
#define POLE_GRAB_ANIM_SPEED 0x14CCD      // 1.3 * vanilla's 0x10000, rounded
#define POLE_SLOW_GRAB_CANCEL_FRAME 2
#define POLE_FAST_GRAB_CANCEL_FRAME 10
#define POLE_HANDSTAND_FACING_ANGLE DEGREES(30)

static s32 pole_handstand_faces_stick(struct MarioState *m) {
    // The handstand pose faces opposite the yaw used while gripping the pole.
    return (m->input & INPUT_NONZERO_ANALOG)
           && abs_angle_diff(m->faceAngle[1] + 0x8000, m->intendedYaw) <= POLE_HANDSTAND_FACING_ANGLE;
}

static s32 pole_grab_can_cancel(struct MarioState *m, u32 buttonInput, s32 firstFrame) {
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    return animInfo->animFrame >= firstFrame
           && (m->input & buttonInput || m->controller->stickY > 16.0f
               || m->controller->stickY < -16.0f);
}

void add_tree_leaf_particles(struct MarioState *m) {
    if (m->usedObj->behavior == segmented_to_virtual(bhvTree)) {
        // make leaf effect spawn higher on the Shifting Sand Land palm tree
        f32 leafHeight = (obj_has_model(m->usedObj, MODEL_SSL_PALM_TREE) ? 250.0f : 100.0f);
        if (m->pos[1] - m->floorHeight > leafHeight) {
            m->particleFlags |= PARTICLE_LEAF;
        }
    }
}

void play_climbing_sounds(struct MarioState *m, s32 b) {
    s32 isOnTree = (m->usedObj->behavior == segmented_to_virtual(bhvTree));

    if (b == 1) {
        if (is_anim_past_frame(m, 1)) {
            play_sound(isOnTree ? SOUND_ACTION_CLIMB_UP_TREE : SOUND_ACTION_CLIMB_UP_POLE,
                       m->marioObj->header.gfx.cameraToObject);
        }
    } else {
        play_sound(isOnTree ? SOUND_MOVING_SLIDE_DOWN_TREE : SOUND_MOVING_SLIDE_DOWN_POLE,
                   m->marioObj->header.gfx.cameraToObject);
    }
}

s32 set_pole_position(struct MarioState *m, f32 offsetY) {
    struct Surface *floor;
    struct Surface *ceil;
    s32 result = POLE_NONE;
    f32 poleTop = m->usedObj->hitboxHeight - 100.0f;
    struct Object *marioObj = m->marioObj;

    if (marioObj->oMarioPolePos > poleTop) {
        marioObj->oMarioPolePos = poleTop;
    }

    vec3f_copy_y_off(m->pos, &m->usedObj->oPosVec, marioObj->oMarioPolePos + offsetY);

    s32 collided = f32_find_wall_collision(&m->pos[0], &m->pos[1], &m->pos[2], 60.0f, 50.0f)
                 + f32_find_wall_collision(&m->pos[0], &m->pos[1], &m->pos[2], 30.0f, 24.0f);

    f32 ceilHeight = find_mario_ceil(m->pos, m->pos[1], &ceil);
    if (m->pos[1] > ceilHeight - 160.0f) {
        m->pos[1] = ceilHeight - 160.0f;
        marioObj->oMarioPolePos = m->pos[1] - m->usedObj->oPosY;
    }

    f32 floorHeight = find_floor(m->pos[0], m->pos[1], m->pos[2], &floor);
    if (m->pos[1] < floorHeight) {
        m->pos[1] = floorHeight;
        set_mario_action(m, ACT_IDLE, 0);
        result = POLE_TOUCHED_FLOOR;
    } else if (marioObj->oMarioPolePos < -m->usedObj->hitboxDownOffset) {
        m->pos[1] = m->usedObj->oPosY - m->usedObj->hitboxDownOffset;
        set_mario_action(m, ACT_FREEFALL, 0);
        result = POLE_FELL_OFF;
    } else if (collided) {
        if (m->pos[1] > floorHeight + 20.0f) {
            m->forwardVel = -2.0f;
            set_mario_action(m, ACT_SOFT_BONK, 0);
            result = POLE_FELL_OFF;
        } else {
            set_mario_action(m, ACT_IDLE, 0);
            result = POLE_TOUCHED_FLOOR;
        }
    }

    vec3f_copy(marioObj->header.gfx.pos, m->pos);
    vec3s_set(marioObj->header.gfx.angle, m->usedObj->oMoveAnglePitch, m->faceAngle[1],
              m->usedObj->oMoveAngleRoll);

    return result;
}

s32 act_holding_pole(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;

    if ((m->input & INPUT_Z_PRESSED) || m->health < 0x100) {
        add_tree_leaf_particles(m);
        m->forwardVel = -2.0f;
        return set_mario_action(m, ACT_SOFT_BONK, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        add_tree_leaf_particles(m);
        m->faceAngle[1] += 0x8000;
        return set_mario_action(m, ACT_WALL_KICK_AIR, 0);
    }

    if (m->controller->stickY > 16.0f) {
        f32 poleTop = m->usedObj->hitboxHeight - 100.0f;

        if (marioObj->oMarioPolePos < poleTop - 0.4f) {
            return set_mario_action(m, ACT_CLIMBING_POLE, 0);
        }

        m->faceAngle[1] = approach_angle(m->faceAngle[1], m->intendedYaw + 0x8000, 0x400);
        if (pole_handstand_faces_stick(m)) {
            return set_mario_action(m, ACT_TOP_OF_POLE_TRANSITION, 0);
        }
    }

    if (m->controller->stickY < -16.0f) {
        m->angleVel[1] -= m->controller->stickY * 2 * POLE_SPEED_MULTIPLIER;
        if (m->angleVel[1] > POLE_DESCENT_ANGLE_VEL_CAP) {
            m->angleVel[1] = POLE_DESCENT_ANGLE_VEL_CAP;
        }

        m->faceAngle[1] += m->angleVel[1];
        marioObj->oMarioPolePos -= m->angleVel[1] / 256.0f * mario_get_movement_time_scale(m);

        add_tree_leaf_particles(m);
        play_climbing_sounds(m, 2);
        reset_rumble_timers_slip();
        set_sound_moving_speed(SOUND_BANK_MOVING, m->angleVel[1] / 0x100 * 2);
    } else {
        m->angleVel[1] = 0;
        if (m->controller->stickY <= 16.0f) {
            m->faceAngle[1] -= m->controller->stickX * 16.0f;
        }
    }

    if (set_pole_position(m, 0.0f) == POLE_NONE) {
        set_mario_animation(m, MARIO_ANIM_IDLE_ON_POLE);
    }

    return FALSE;
}

s32 act_climbing_pole(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;

    if ((m->input & INPUT_Z_PRESSED) || m->health < 0x100) {
        add_tree_leaf_particles(m);
        m->forwardVel = -2.0f;
        return set_mario_action(m, ACT_SOFT_BONK, 0);
    }

    if (m->input & INPUT_A_PRESSED) {
        add_tree_leaf_particles(m);
        m->faceAngle[1] += 0x8000;
        return set_mario_action(m, ACT_WALL_KICK_AIR, 0);
    }

    if (m->controller->stickY < 8.0f) {
        return set_mario_action(m, ACT_HOLDING_POLE, 0);
    }

    f32 timeScale = mario_get_movement_time_scale(m);
    marioObj->oMarioPolePos += m->controller->stickY / 8.0f * POLE_SPEED_MULTIPLIER * timeScale;
    m->angleVel[1]  = 0;
    m->faceAngle[1] = approach_angle(m->faceAngle[1], m->intendedYaw + 0x8000, 0x400);

    if (set_pole_position(m, 0.0f) == POLE_NONE) {
        if (marioObj->oMarioPolePos >= m->usedObj->hitboxHeight - 100.0f - 0.4f) {
            if (pole_handstand_faces_stick(m)) {
                return set_mario_action(m, ACT_TOP_OF_POLE_TRANSITION, 0);
            }
            return set_mario_action(m, ACT_HOLDING_POLE, 0);
        }
        s32 animSpeed = m->controller->stickY / 4.0f * 0x10000 * POLE_SPEED_MULTIPLIER * timeScale;
        set_mario_anim_with_accel(m, MARIO_ANIM_CLIMB_UP_POLE, animSpeed);
        add_tree_leaf_particles(m);
        play_climbing_sounds(m, 1);
    }

    return FALSE;
}

s32 act_grab_pole_slow(struct MarioState *m) {
    if (set_pole_position(m, 0.0f) == POLE_NONE) {
        set_mario_anim_with_accel(m, MARIO_ANIM_GRAB_POLE_SHORT, POLE_GRAB_ANIM_SPEED);
        if (m->marioObj->header.gfx.animInfo.animFrame
                >= m->marioObj->header.gfx.animInfo.curAnim->loopEnd - 1
            || pole_grab_can_cancel(m, INPUT_A_PRESSED | INPUT_Z_PRESSED,
                                    POLE_SLOW_GRAB_CANCEL_FRAME)) {
            return set_mario_action(m, ACT_HOLDING_POLE, 0);
        }
        add_tree_leaf_particles(m);
    }

    return FALSE;
}

s32 act_grab_pole_fast(struct MarioState *m) {
    play_sound_if_no_flag(m, SOUND_MARIO_WHOA, MARIO_MARIO_SOUND_PLAYED);
    m->faceAngle[1] += m->angleVel[1];
    m->angleVel[1] = m->angleVel[1] * 8 / 10;

    if (set_pole_position(m, 0.0f) == POLE_NONE) {
        if (abss(m->angleVel[1]) > 0x800) {
            set_mario_anim_with_accel(m, MARIO_ANIM_GRAB_POLE_SWING_PART1, POLE_GRAB_ANIM_SPEED);
        } else {
            set_mario_anim_with_accel(m, MARIO_ANIM_GRAB_POLE_SWING_PART2, POLE_GRAB_ANIM_SPEED);
            if (m->marioObj->header.gfx.animInfo.animFrame
                    >= m->marioObj->header.gfx.animInfo.curAnim->loopEnd - 1
                || pole_grab_can_cancel(m,
                                        INPUT_A_DOWN | INPUT_Z_DOWN | INPUT_A_PRESSED | INPUT_Z_PRESSED,
                                        POLE_FAST_GRAB_CANCEL_FRAME)) {
                m->angleVel[1] = 0;
                return set_mario_action(m, ACT_HOLDING_POLE, 0);
            }
        }
        add_tree_leaf_particles(m);
    }

    return FALSE;
}

static s32 try_cancel_pole_handstand(struct MarioState *m) {
    if (!(m->input & INPUT_Z_PRESSED) && m->health >= 0x100) {
        return FALSE;
    }

    // Carry the last displayed orientation through startup, hold and return.
    // set_pole_position refreshes a moving pole and the current root offset,
    // but overwrites the graphics yaw with the ordinary grip orientation.
    s16 yaw = m->marioObj->header.gfx.angle[1];
    if (set_pole_position(m, return_mario_anim_y_translation(m)) != POLE_NONE) {
        return TRUE;
    }

    add_tree_leaf_particles(m);
    vec3s_set(m->faceAngle, 0, yaw, 0);
    vec3s_set(m->marioObj->header.gfx.angle, 0, yaw, 0);
    m->angleVel[1] = 0;
    m->forwardVel = -2.0f;
    return set_mario_action(m, ACT_SOFT_BONK, 0);
}

s32 act_top_of_pole_transition(struct MarioState *m) {
    if (try_cancel_pole_handstand(m)) {
        return TRUE;
    }
    m->angleVel[1] = 0;

    if (m->actionArg == 0) {
        set_mario_animation(m, MARIO_ANIM_START_HANDSTAND);
        if (is_anim_at_end(m)
            || (is_anim_past_end(m) && (m->input & (INPUT_A_DOWN | INPUT_A_PRESSED)))) {
            return set_mario_action(m, ACT_TOP_OF_POLE, 0);
        }
    } else {
        set_mario_animation(m, MARIO_ANIM_RETURN_FROM_HANDSTAND);
        if (m->marioObj->header.gfx.animInfo.animFrame <= 1) {
            return set_mario_action(m, ACT_HOLDING_POLE, 0);
        }
    }

    set_pole_position(m, return_mario_anim_y_translation(m));
    return FALSE;
}

static f32 pole_jump_anim_y_translation(struct MarioState *m) {
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    struct Animation *anim = animInfo->curAnim;
    u16 *index = segmented_to_virtual((void *) anim->index);
    s16 *values = segmented_to_virtual((void *) anim->values);
    f32 scale =
        anim->animYTransDivisor == 0 ? 1.0f : (f32) animInfo->animYTrans / anim->animYTransDivisor;

    // Sample the first rendered root Y, independent of the pending render clock.
    index += 2;
    return values[retrieve_animation_index(anim->startFrame, &index)] * scale / 4.0f;
}

s32 act_top_of_pole(struct MarioState *m) {
    if (try_cancel_pole_handstand(m)) {
        return TRUE;
    }
    if (m->input & (INPUT_A_DOWN | INPUT_A_PRESSED)) {
        if (set_pole_position(m, return_mario_anim_y_translation(m)) != POLE_NONE) {
            return TRUE;
        }
        // Carry the handstand's corrected orientation into the actual jump.
        m->faceAngle[1] += 0x8000;
        set_mario_animation(m, MARIO_ANIM_HANDSTAND_JUMP);
        // Attachment placed the hips; airborne positioning supplies the feet.
        // Transfer that origin once instead of adding the new root height again.
        m->pos[1] -= pole_jump_anim_y_translation(m);
        vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
        return set_mario_action(m, ACT_TOP_OF_POLE_JUMP, 0);
    }
    if (m->controller->stickY < -16.0f) {
        return set_mario_action(m, ACT_TOP_OF_POLE_TRANSITION, 1);
    }

    m->faceAngle[1] -= m->controller->stickX * 16.0f;

    set_mario_animation(m, MARIO_ANIM_HANDSTAND_IDLE);
    set_pole_position(m, return_mario_anim_y_translation(m));
    return FALSE;
}

s32 perform_hanging_step(struct MarioState *m, Vec3f nextPos) {
    struct Surface *ceil, *floor;
    struct WallCollisionData wallCollisionData;

    resolve_and_return_wall_collisions(nextPos, 50.0f, 50.0f, &wallCollisionData);
    set_mario_wall(m, wallCollisionData.numWalls == 0 ? NULL : wallCollisionData.walls[0]);

    f32 floorHeight = find_floor(nextPos[0], nextPos[1], nextPos[2], &floor);
    f32 ceilHeight = find_mario_ceil(nextPos, floorHeight, &ceil);

    if (floor == NULL) {
        return HANG_HIT_CEIL_OR_OOB;
    }
    if (ceil == NULL) {
        return HANG_LEFT_CEIL;
    }
    if (ceilHeight - floorHeight <= HANG_DISTANCE) {
        return HANG_HIT_CEIL_OR_OOB;
    }
    if (ceil->type != SURFACE_HANGABLE) {
        return HANG_LEFT_CEIL;
    }

    f32 ceilOffset = ceilHeight - (nextPos[1] + HANG_DISTANCE);
    if (ceilOffset < -30.0f) {
        return HANG_HIT_CEIL_OR_OOB;
    }
    if (ceilOffset > 30.0f) {
        return HANG_LEFT_CEIL;
    }

    nextPos[1] = m->ceilHeight - HANG_DISTANCE;
    vec3f_copy(m->pos, nextPos);

    set_mario_floor(m, floor, floorHeight);
    set_mario_ceil(m, ceil, ceilHeight);

    return HANG_NONE;
}

s32 update_hang_moving(struct MarioState *m) {
    Vec3f nextPos;
#ifdef BETTER_HANGING
    f32 maxSpeed = (m->intendedMag / 2.0f);
#else
    f32 maxSpeed = HANGING_SPEED;
#endif

    if (m->forwardVel > maxSpeed) {
        m->forwardVel = MAX(maxSpeed, m->forwardVel - HANG_GRAB_COAST_DECEL);
    } else {
        m->forwardVel = MIN(maxSpeed, m->forwardVel + 1.0f);
    }

#ifdef BETTER_HANGING
    s16 turnRange = 0x800;
    s16 dYaw = abs_angle_diff(m->faceAngle[1], m->intendedYaw); // 0x0 is turning forwards, 0x8000 is turning backwards

    if (m->forwardVel < 0.0f) { // Don't modify Mario's speed and turn radius if Mario is moving backwards
        // Flip controls when moving backwards so Mario still moves towards intendedYaw
        m->intendedYaw += 0x8000;
    } else if (dYaw > 0x4000) { // Only modify Mario's speed and turn radius if Mario is turning around
        // Reduce Mario's forward speed by the turn amount, so Mario won't move off sideward from the intended angle when turning around.
        m->forwardVel *= ((coss(dYaw) + 1.0f) / 2.0f); // 1.0f is turning forwards, 0.0f is turning backwards
        // Increase turn speed if forwardVel is lower and intendedMag is higher
        turnRange *= (2.0f - (absf(m->forwardVel) / MAX(m->intendedMag, NEAR_ZERO))); // 1.0f front, 2.0f back
    }
    m->faceAngle[1] = approach_angle(m->faceAngle[1], m->intendedYaw, turnRange);
#else
    m->faceAngle[1] = approach_angle(m->faceAngle[1], m->intendedYaw, 0x800);
#endif

    m->slideYaw = m->faceAngle[1];
    m->slideVelX = m->forwardVel * sins(m->faceAngle[1]);
    m->slideVelZ = m->forwardVel * coss(m->faceAngle[1]);

    m->vel[0] = m->slideVelX;
    m->vel[1] = 0.0f;
    m->vel[2] = m->slideVelZ;

    f32 timeScale = mario_get_movement_time_scale(m);
    nextPos[0] = m->pos[0] - m->ceil->normal.y * m->vel[0] * timeScale;
    nextPos[2] = m->pos[2] - m->ceil->normal.y * m->vel[2] * timeScale;
    nextPos[1] = m->pos[1];

    s32 stepResult = perform_hanging_step(m, nextPos);

    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    return stepResult;
}

void update_hang_stationary(struct MarioState *m) {
    m->forwardVel = 0.0f;
    m->slideVelX = 0.0f;
    m->slideVelZ = 0.0f;

    m->pos[1] = m->ceilHeight - HANG_DISTANCE;
    vec3f_copy(m->vel, gVec3fZero);
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
#ifdef BETTER_HANGING
    vec3s_set(m->marioObj->header.gfx.angle, 0x0, m->faceAngle[1], 0x0);
#endif
}

static s32 update_hang_grab_momentum(struct MarioState *m) {
    Vec3f nextPos;
    f32 timeScale = mario_get_movement_time_scale(m);
    mario_set_forward_vel(m, m->forwardVel);
    m->vel[1] = 0.0f;
    nextPos[0] = m->pos[0] - m->ceil->normal.y * m->vel[0] * timeScale;
    nextPos[1] = m->ceilHeight - HANG_DISTANCE;
    nextPos[2] = m->pos[2] - m->ceil->normal.y * m->vel[2] * timeScale;
    s32 step = perform_hanging_step(m, nextPos);
    if (step == HANG_LEFT_CEIL) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }
    if (step == HANG_HIT_CEIL_OR_OOB || m->wall != NULL) {
        mario_set_forward_vel(m, 0.0f);
    } else {
        mario_set_forward_vel(
            m, approach_f32(m->forwardVel, 0.0f, HANG_GRAB_COAST_DECEL, HANG_GRAB_COAST_DECEL));
    }
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    return FALSE;
}

s32 act_start_hanging(struct MarioState *m) {
    if (m->actionTimer == 0) {
        queue_rumble_data(5, 80);
    }
    m->actionTimer++;
#ifdef BETTER_HANGING
    // immediately go into hanging if controller stick is pointed far enough in
    // any direction, and it has been at least a frame
    if (
        (m->input & INPUT_NONZERO_ANALOG)
        && m->intendedMag > 16.0f
        && m->actionTimer > 1) {
        return set_mario_action(m, ACT_HANGING, 0);
    }

    // Only let go if A or B has been pressed
    if (m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED)) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }
#else
    if ((m->input & INPUT_NONZERO_ANALOG) && m->actionTimer >= 31) {
        return set_mario_action(m, ACT_HANGING, 0);
    }

    if (!(m->input & INPUT_A_DOWN)) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }
#endif

    if (m->input & INPUT_Z_PRESSED) {
        return set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    // HackerSM64 fix: check if ceil is NULL to prevent crashing
    if (m->ceil == NULL || m->ceil->type != SURFACE_HANGABLE) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->actionTimer == 1) {
        m->forwardVel *= HANG_GRAB_MOMENTUM_RETENTION;
        if (!game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
            m->forwardVel = CLAMP(m->forwardVel, -HANG_GRAB_MAX_SPEED, HANG_GRAB_MAX_SPEED);
        }
    }
    set_mario_animation(m, MARIO_ANIM_HANG_ON_CEILING);
    play_sound_if_no_flag(m, SOUND_ACTION_HANGING_STEP, MARIO_ACTION_SOUND_PLAYED);
    if (m->forwardVel != 0.0f) {
        if (update_hang_grab_momentum(m)) {
            return TRUE;
        }
    } else {
        update_hang_stationary(m);
    }

    if (is_anim_at_end(m)) {
        set_mario_action(m, ACT_HANGING, 0);
    }

    return FALSE;
}

s32 act_hanging(struct MarioState *m) {
    if (m->input & INPUT_NONZERO_ANALOG) {
        return set_mario_action(m, ACT_HANG_MOVING, m->actionArg);
    }

#ifdef BETTER_HANGING
    // Only let go if A or B is pressed
    if (m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED)) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }
#else
    if (!(m->input & INPUT_A_DOWN)) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }
#endif

    if (m->input & INPUT_Z_PRESSED) {
        return set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    // HackerSM64 fix: check if ceil is NULL to prevent crashing
    if (m->ceil == NULL || m->ceil->type != SURFACE_HANGABLE) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    if (m->actionArg & 1) {
        set_mario_animation(m, MARIO_ANIM_HANDSTAND_LEFT);
    } else {
        set_mario_animation(m, MARIO_ANIM_HANDSTAND_RIGHT);
    }

    update_hang_stationary(m);

    return FALSE;
}

s32 act_hang_moving(struct MarioState *m) {
#ifdef BETTER_HANGING
    // Only let go if A or B is pressed
    if (m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED)) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }
#else
    if (!(m->input & INPUT_A_DOWN)) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }
#endif

    if (m->input & INPUT_Z_PRESSED) {
        return set_mario_action(m, ACT_GROUND_POUND, 0);
    }

    // HackerSM64 fix: check if ceil is NULL to prevent crashing
    if (m->ceil == NULL || m->ceil->type != SURFACE_HANGABLE) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

#ifdef BETTER_HANGING
    // determine animation speed from forward velocity
    set_mario_anim_with_accel(
        m,
        (m->actionArg & 0x1) ? MARIO_ANIM_MOVE_ON_WIRE_NET_RIGHT : MARIO_ANIM_MOVE_ON_WIRE_NET_LEFT,
        (m->forwardVel + 1.0f) * 0x2000
    );
#else
    if (m->actionArg & 1) {
        set_mario_animation(m, MARIO_ANIM_MOVE_ON_WIRE_NET_RIGHT);
    } else {
        set_mario_animation(m, MARIO_ANIM_MOVE_ON_WIRE_NET_LEFT);
    }
#endif

    if (m->marioObj->header.gfx.animInfo.animFrame == 12) {
        play_sound(SOUND_ACTION_HANGING_STEP, m->marioObj->header.gfx.cameraToObject);
        queue_rumble_data(1, 30);
    }

#ifdef BETTER_HANGING
    if (m->input & INPUT_IDLE) {
        if (m->marioObj->header.gfx.animInfo.animFrame > 6) m->actionArg ^= 1;
        set_mario_action(m, ACT_HANGING, m->actionArg);
    } else if (is_anim_past_end(m)) {
        m->actionArg ^= 1;
    }
    update_hang_moving(m);
#else
    if (is_anim_past_end(m)) {
        m->actionArg ^= 1;
        if (m->input & INPUT_IDLE) {
            return set_mario_action(m, ACT_HANGING, m->actionArg);
        }
    }

    if (update_hang_moving(m) == HANG_LEFT_CEIL) {
        set_mario_action(m, ACT_FREEFALL, 0);
    }
#endif

    return FALSE;
}

s32 let_go_of_ledge(struct MarioState *m) {
    struct Surface *floor;

    m->vel[1] = 0.0f;
    m->forwardVel = -8.0f;
    m->pos[0] -= 60.0f * sins(m->faceAngle[1]);
    m->pos[2] -= 60.0f * coss(m->faceAngle[1]);

    f32 floorHeight = find_floor(m->pos[0], m->pos[1], m->pos[2], &floor);
    if (floorHeight < m->pos[1] - 100.0f) {
        m->pos[1] -= 100.0f;
    } else {
        m->pos[1] = floorHeight;
    }

    return set_mario_action(m, ACT_SOFT_BONK, 0);
}

void climb_up_ledge(struct MarioState *m) {
    set_mario_animation(m, MARIO_ANIM_IDLE_HEAD_LEFT);
    m->pos[0] += 14.0f * sins(m->faceAngle[1]);
    m->pos[2] += 14.0f * coss(m->faceAngle[1]);
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
}

void update_ledge_climb_camera(struct MarioState *m) {
    f32 dist = MIN(m->actionTimer, 14.0f);

    m->statusForCamera->pos[0] = m->pos[0] + dist * sins(m->faceAngle[1]);
    m->statusForCamera->pos[2] = m->pos[2] + dist * coss(m->faceAngle[1]);
    m->statusForCamera->pos[1] = m->pos[1];
    m->actionTimer++;
    m->flags |= MARIO_LEDGE_CLIMB_CAMERA;
}

void update_ledge_climb(struct MarioState *m, s32 animation, u32 endAction) {
    stop_and_set_height_to_floor(m);

    s32 startingAnimation = m->marioObj->header.gfx.animInfo.animID != animation;
    set_mario_animation(m, animation);
    if (startingAnimation && animation != MARIO_ANIM_CLIMB_DOWN_LEDGE) {
        set_anim_to_frame(m, 3);
    }
    if (is_anim_at_end(m)) {
        set_mario_action(m, endAction, 0);
        if (endAction == ACT_IDLE) {
            climb_up_ledge(m);
        }
    }
}

s32 act_ledge_grab(struct MarioState *m) {
    f32 heightAboveFloor;
    s16 intendedDYaw = m->intendedYaw - m->faceAngle[1];
    s32 hasSpaceForMario = (m->ceilHeight - m->floorHeight >= 160.0f);

    if (m->actionTimer < 8) {
        m->actionTimer++;
    }
    if (m->floor->normal.y < COS25) {
        return let_go_of_ledge(m);
    }
    if ((m->input & INPUT_OFF_FLOOR) || ((m->input & INPUT_Z_PRESSED) && m->actionTimer >= 8)) {
        return let_go_of_ledge(m);
    }

    if ((m->input & INPUT_A_PRESSED) && hasSpaceForMario) {
        return set_mario_action(m, ACT_LEDGE_CLIMB_FAST, 0);
    }

    if (m->input & INPUT_STOMPED) {
        if (m->marioObj->oInteractStatus
            & (INT_STATUS_MARIO_STUNNED | INT_STATUS_MARIO_KNOCKBACK_DMG
               | INT_STATUS_MARIO_SHOCKWAVE)) {
            return mario_bowser_impact_knockback(m, TRUE);
        }
        return let_go_of_ledge(m);
    }
#ifdef VERSION_EU
    // On EU, you can't slow climb up ledges while holding A.
    if (m->actionTimer == 8 && (m->input & INPUT_NONZERO_ANALOG) && !(m->input & INPUT_A_DOWN))
#else
    if (m->actionTimer == 8 && (m->input & INPUT_NONZERO_ANALOG))
#endif
    {
        if (intendedDYaw >= -0x4000 && intendedDYaw <= 0x4000) {
            if (hasSpaceForMario) {
                return set_mario_action(m, ACT_LEDGE_CLIMB_SLOW_1, 0);
            }
        }
    }

    heightAboveFloor = m->pos[1] - find_floor_height_relative_polar(m, -0x8000, 30.0f);
    if (hasSpaceForMario && heightAboveFloor < 100.0f) {
        return set_mario_action(m, ACT_LEDGE_CLIMB_FAST, 0);
    }

    if (m->actionArg != ACT_ARG_LEDGE_GRAB_SILENT) {
        play_sound_if_no_flag(
            m, m->actionArg == ACT_ARG_LEDGE_GRAB_GENTLE ? SOUND_MARIO_HRMM : SOUND_MARIO_WHOA,
            MARIO_MARIO_SOUND_PLAYED);
    }

    stop_and_set_height_to_floor(m);
    s32 startingAnimation = m->marioObj->header.gfx.animInfo.animID != MARIO_ANIM_IDLE_ON_LEDGE;
    set_mario_animation(m, MARIO_ANIM_IDLE_ON_LEDGE);
    if (startingAnimation) {
        set_anim_to_frame(m, 1);
    }

    return FALSE;
}

s32 act_ledge_climb_slow(struct MarioState *m) {
    if (m->input & (INPUT_Z_PRESSED | INPUT_OFF_FLOOR)) {
        return let_go_of_ledge(m);
    }

    if (m->actionTimer >= 24
        && (m->input
            & (INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_OFF_FLOOR | INPUT_ABOVE_SLIDE))) {
        climb_up_ledge(m);
        return check_common_action_exits(m);
    }

    if (m->actionTimer == 7) {
        play_sound_if_no_flag(m, SOUND_MARIO_EEUH, MARIO_MARIO_SOUND_PLAYED);
    }

    update_ledge_climb(m, MARIO_ANIM_SLOW_LEDGE_GRAB, ACT_IDLE);

    update_ledge_climb_camera(m);
    if (m->marioObj->header.gfx.animInfo.animFrame == 17) {
        m->action = ACT_LEDGE_CLIMB_SLOW_2;
    }

    return FALSE;
}

s32 act_ledge_climb_down(struct MarioState *m) {
    if (m->input & (INPUT_Z_PRESSED | INPUT_OFF_FLOOR)) {
        return let_go_of_ledge(m);
    }

    play_sound_if_no_flag(m, SOUND_MARIO_HRMM, MARIO_MARIO_SOUND_PLAYED);

    update_ledge_climb(m, MARIO_ANIM_CLIMB_DOWN_LEDGE, ACT_LEDGE_GRAB);
    m->actionArg = ACT_ARG_LEDGE_GRAB_SILENT;

    return FALSE;
}

s32 act_ledge_climb_fast(struct MarioState *m) {
    if (m->input & (INPUT_Z_PRESSED | INPUT_OFF_FLOOR)) {
        return let_go_of_ledge(m);
    }

    update_ledge_climb(m, MARIO_ANIM_FAST_LEDGE_GRAB, ACT_IDLE);

    if (m->action == ACT_LEDGE_CLIMB_FAST && m->marioObj->header.gfx.animInfo.animFrame >= 5) {
        // Replay requests before changing families clears their timers.
        if (m->automaticAFrames > 0) {
            m->input |= INPUT_A_PRESSED;
        }
        if (m->automaticBFrames > 0) {
            m->input |= INPUT_B_PRESSED;
        }
        s32 boostForward = mario_has_dive_input(m);
        play_mario_landing_sound(m, SOUND_ACTION_TERRAIN_LANDING);
        set_mario_action(m, ACT_IDLE, 0);
        if (boostForward) {
            mario_set_forward_vel(m, m->forwardVel + 20.0f);
        }
        climb_up_ledge(m);
        update_ledge_climb_camera(m);
        return TRUE;
    }
    update_ledge_climb_camera(m);

    return FALSE;
}

s32 act_grabbed(struct MarioState *m) {
    if (m->marioObj->oInteractStatus & INT_STATUS_MARIO_THROWN_BY_OBJ) {
        s32 thrown = (m->marioObj->oInteractStatus & INT_STATUS_MARIO_DROPPED_BY_OBJ) == 0;

        m->heaveHoThrowPendingLanding =
            thrown && m->usedObj->behavior == segmented_to_virtual(bhvHeaveHo);
        m->faceAngle[1] = m->usedObj->oMoveAngleYaw;
        vec3f_copy(m->pos, m->marioObj->header.gfx.pos);
        queue_rumble_data(5, 60);

        return set_mario_action(m, (m->forwardVel >= 0.0f) ? ACT_THROWN_FORWARD : ACT_THROWN_BACKWARD,
                                thrown);
    }

    set_mario_animation(m, MARIO_ANIM_BEING_GRABBED);
    return FALSE;
}

s32 act_in_cannon(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;
    s16 startFacePitch = m->faceAngle[0];
    s16 startFaceYaw = m->faceAngle[1];

    if (m->health < 0x100) {
        marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
        m->usedObj->oAction = OPENED_CANNON_ACT_SHOOT;
        m->statusForCamera->cameraEvent = CAM_EVENT_NONE;
        m->area->camera->cutscene = CUTSCENE_NONE;
        set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
        return set_mario_action(m, ACT_FREEFALL, 0);
    }

    switch (m->actionState) {
        case ACT_STATE_IN_CANNON_INIT:
            marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
            m->usedObj->oInteractStatus = INT_STATUS_INTERACTED;

            m->statusForCamera->cameraEvent = CAM_EVENT_CANNON;
            m->statusForCamera->usedObj = m->usedObj;

            vec3_zero(m->vel);
            vec3f_copy_y_off(m->pos, &m->usedObj->oPosVec, 350.0f);

            m->forwardVel = 0.0f;

            m->actionState = ACT_STATE_IN_CANNON_WAIT_FOR_CANNON;
            break;

        case ACT_STATE_IN_CANNON_WAIT_FOR_CANNON:
            if (m->usedObj->oAction == OPENED_CANNON_ACT_READY) {
                m->faceAngle[0] = m->usedObj->oMoveAnglePitch;
                m->faceAngle[1] = m->usedObj->oMoveAngleYaw;

                marioObj->oMarioCannonObjectYaw = m->usedObj->oMoveAngleYaw;
                marioObj->oMarioCannonInputYaw  = 0x0;

                m->actionState = ACT_STATE_IN_CANNON_READY;
            }
            break;

        case ACT_STATE_IN_CANNON_READY:
            m->faceAngle[0] -= (s16) (m->controller->stickY * 10.0f);
            s32 inputYaw = marioObj->oMarioCannonInputYaw - (s16) (m->controller->stickX * 10.0f);
            marioObj->oMarioCannonInputYaw = game_options_get(GO_CANNON_ANGLES) == 1
                                                 ? CLAMP(inputYaw, -0x4000, 0x4000)
                                                 : (s16) inputYaw;

            m->faceAngle[0] = CLAMP(m->faceAngle[0], 0, DEGREES(80));

            m->faceAngle[1] = (marioObj->oMarioCannonObjectYaw + marioObj->oMarioCannonInputYaw);
            if (m->input & INPUT_A_PRESSED) {
                f32 cosPitch = coss(m->faceAngle[0]);
                m->forwardVel = 100.0f * cosPitch;

                f32 sinPitch = sins(m->faceAngle[0]);
                m->vel[1] = 100.0f * sinPitch;

                cosPitch *= 120.0f;

                m->pos[0] += cosPitch * sins(m->faceAngle[1]);
                m->pos[1] += 120.0f * sinPitch;
                m->pos[2] += cosPitch * coss(m->faceAngle[1]);

                play_sound(SOUND_ACTION_FLYING_FAST, marioObj->header.gfx.cameraToObject);
                play_sound(SOUND_OBJ_POUNDING_CANNON, marioObj->header.gfx.cameraToObject);

                marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;

                set_mario_action(m, ACT_SHOT_FROM_CANNON, 0);
                queue_rumble_data(60, 70);
                m->usedObj->oAction = OPENED_CANNON_ACT_SHOOT;
                return FALSE;
            } else if (m->faceAngle[0] != startFacePitch || m->faceAngle[1] != startFaceYaw) {
                play_sound(SOUND_MOVING_AIM_CANNON, marioObj->header.gfx.cameraToObject);
                reset_rumble_timers_vibrate(0);
            }
    }

    vec3f_copy(marioObj->header.gfx.pos, m->pos);
    vec3s_set(marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    set_mario_animation(m, MARIO_ANIM_DIVE);

    return FALSE;
}

s32 act_tornado_twirling(struct MarioState *m) {
    struct Surface *floor;
    Vec3f nextPos;
    f32 sinAngleVel;
    f32 cosAngleVel;
    f32 floorHeight;
    struct Object *marioObj = m->marioObj;
    struct Object *usedObj = m->usedObj;
    s16 prevTwirlYaw = m->twirlYaw;

    f32 dx = (m->pos[0] - usedObj->oPosX) * 0.95f;
    f32 dz = (m->pos[2] - usedObj->oPosZ) * 0.95f;

    if (m->vel[1] < 60.0f) {
        m->vel[1] += 1.0f;
    }

    if ((marioObj->oMarioTornadoPosY += m->vel[1]) < 0.0f) {
        marioObj->oMarioTornadoPosY = 0.0f;
    }
    if (marioObj->oMarioTornadoPosY > usedObj->hitboxHeight) {
        if (m->vel[1] < 20.0f) {
            m->vel[1] = 20.0f;
        }
        return set_mario_action(m, ACT_TWIRLING, 1);
    }

    if (m->angleVel[1] < 0x3000) {
        m->angleVel[1] += 0x100;
    }

    if (marioObj->oMarioTornadoYawVel < 0x1000) {
        marioObj->oMarioTornadoYawVel += 0x100;
    }

    m->twirlYaw += m->angleVel[1];

    sinAngleVel = sins(marioObj->oMarioTornadoYawVel);
    cosAngleVel = coss(marioObj->oMarioTornadoYawVel);

    nextPos[0] = usedObj->oPosX + dx * cosAngleVel + dz * sinAngleVel;
    nextPos[2] = usedObj->oPosZ - dx * sinAngleVel + dz * cosAngleVel;
    nextPos[1] = usedObj->oPosY + marioObj->oMarioTornadoPosY;

    f32_find_wall_collision(&nextPos[0], &nextPos[1], &nextPos[2], 60.0f, 50.0f);

    floorHeight = find_floor(nextPos[0], nextPos[1], nextPos[2], &floor);
    if (floor != NULL) {
        set_mario_floor(m, floor, floorHeight);
        vec3f_copy(m->pos, nextPos);
    } else {
        if (nextPos[1] >= m->floorHeight) {
            m->pos[1] = nextPos[1];
        } else {
            m->pos[1] = m->floorHeight;
        }
    }

    m->actionTimer++;

    set_mario_animation(m, (m->actionArg == 0) ? MARIO_ANIM_START_TWIRL : MARIO_ANIM_TWIRL);

    if (is_anim_past_end(m)) {
        m->actionArg = 1;
    }

    // Play sound on angle overflow
    if (prevTwirlYaw > m->twirlYaw) {
        play_sound(SOUND_ACTION_TWIRL, marioObj->header.gfx.cameraToObject);
    }

    vec3f_copy(marioObj->header.gfx.pos, m->pos);
    vec3s_set(marioObj->header.gfx.angle, 0, m->faceAngle[1] + m->twirlYaw, 0);
    reset_rumble_timers_slip();

    return FALSE;
}

s32 check_common_automatic_cancels(struct MarioState *m) {
    if (mario_has_current_metal_cap(m) && m->action == ACT_TORNADO_TWIRLING) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }
    if (m->pos[1] < m->waterLevel - 100 && !mario_has_land_movement_underwater(m)) {
        return set_water_plunge_action(m);
    }

    return FALSE;
}

void mario_apply_automatic_buffer(struct MarioState *m) {
    s32 family = mario_move_buffer_family(m->action);
    if (family >= 1 && family <= 3) {
        if (m->automaticZFrames > 0) {
            m->input |= INPUT_Z_PRESSED;
        }
        if (family == 1 && m->automaticAFrames > 0) {
            m->input |= INPUT_A_PRESSED;
        }
        if (family == 2) {
            if (m->automaticAFrames > 0) {
                m->input |= INPUT_A_PRESSED;
            }
            if (m->automaticBFrames > 0) {
                m->input |= INPUT_B_PRESSED;
            }
        }
    }
}

s32 mario_execute_automatic_action(struct MarioState *m) {
    mario_apply_automatic_buffer(m);
    s32 cancel = FALSE;

    if (check_common_automatic_cancels(m)) {
        return TRUE;
    }

    m->quicksandDepth = 0.0f;

    /* clang-format off */
    switch (m->action) {
        case ACT_HOLDING_POLE:           cancel = act_holding_pole(m);           break;
        case ACT_GRAB_POLE_SLOW:         cancel = act_grab_pole_slow(m);         break;
        case ACT_GRAB_POLE_FAST:         cancel = act_grab_pole_fast(m);         break;
        case ACT_CLIMBING_POLE:          cancel = act_climbing_pole(m);          break;
        case ACT_TOP_OF_POLE_TRANSITION: cancel = act_top_of_pole_transition(m); break;
        case ACT_TOP_OF_POLE:            cancel = act_top_of_pole(m);            break;
        case ACT_START_HANGING:          cancel = act_start_hanging(m);          break;
        case ACT_HANGING:                cancel = act_hanging(m);                break;
        case ACT_HANG_MOVING:            cancel = act_hang_moving(m);            break;
        case ACT_LEDGE_GRAB:             cancel = act_ledge_grab(m);             break;
        case ACT_LEDGE_CLIMB_SLOW_1:     cancel = act_ledge_climb_slow(m);       break;
        case ACT_LEDGE_CLIMB_SLOW_2:     cancel = act_ledge_climb_slow(m);       break;
        case ACT_LEDGE_CLIMB_DOWN:       cancel = act_ledge_climb_down(m);       break;
        case ACT_LEDGE_CLIMB_FAST:       cancel = act_ledge_climb_fast(m);       break;
        case ACT_GRABBED:                cancel = act_grabbed(m);                break;
        case ACT_IN_CANNON:              cancel = act_in_cannon(m);              break;
        case ACT_TORNADO_TWIRLING:       cancel = act_tornado_twirling(m);       break;
    }
    /* clang-format on */

    return cancel;
}
