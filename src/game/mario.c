#include "player_stats.h"
#include <PR/ultratypes.h>

#include "sm64.h"
#include "mario_character.h"
#include "area.h"
#include "audio/external.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "camera.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "engine/surface_load.h"
#include "engine/surface_query.h"
#include "game_init.h"
#include "game_options.h"
#include "game_options_runtime.h"
#include "game_timing_runtime.h"
#include "interaction.h"
#include "jrb_simpleflips.h"
#include "level_table.h"
#include "level_update.h"
#include "liquid_particles.h"
#include "main.h"
#include "mario.h"
#include "mario_dab.h"
#include "mario_he_thicc.h"
#include "mario_spin_dizzy.h"
#include "mario_actions_airborne.h"
#include "mario_actions_automatic.h"
#include "mario_actions_cutscene.h"
#include "mario_actions_moving.h"
#include "mario_actions_object.h"
#include "mario_punch.h"
#include "mario_actions_stationary.h"
#include "mario_actions_submerged.h"
#include "mario_misc.h"
#include "mario_step.h"
#include "mario_water_collision.h"
#include "mario_transition_pose.h"
#include "memory.h"
#include "object_fields.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "print.h"
#include "puppyprint.h"
#include "physics_lab.h"
#include "save_file.h"
#include "sound_init.h"
#include "rumble_init.h"


/**************************************************
 *                    ANIMATIONS                  *
 **************************************************/

/**
 * Checks if Mario's animation has reached its end point.
 */
s32 is_anim_at_end(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;

    return (marioObj->header.gfx.animInfo.animFrame + 1) == marioObj->header.gfx.animInfo.curAnim->loopEnd;
}

/**
 * Checks if Mario's animation has surpassed 2 frames before its end point.
 */
s32 is_anim_past_end(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;

    return marioObj->header.gfx.animInfo.animFrame >= (marioObj->header.gfx.animInfo.curAnim->loopEnd - 2);
}

/**
 * Sets Mario's animation without any acceleration, running at its default rate.
 */
s16 set_mario_animation(struct MarioState *m, s32 targetAnimID) {
    struct Object *marioObj = m->marioObj;
    struct Animation *targetAnim = m->animList->bufTarget;

    mario_transition_pose_begin(m, targetAnimID);
    if (load_patchable_table(m->animList, targetAnimID)) {
        targetAnim->values = (void *) VIRTUAL_TO_PHYSICAL((u8 *) targetAnim + (uintptr_t) targetAnim->values);
        targetAnim->index  = (void *) VIRTUAL_TO_PHYSICAL((u8 *) targetAnim + (uintptr_t) targetAnim->index);
    }

    if (marioObj->header.gfx.animInfo.animID != targetAnimID) {
        marioObj->header.gfx.animInfo.animID = targetAnimID;
        marioObj->header.gfx.animInfo.curAnim = targetAnim;
        marioObj->header.gfx.animInfo.animAccel = 0;
        marioObj->header.gfx.animInfo.animYTrans = m->animYTrans;

        if (targetAnim->flags & ANIM_FLAG_NO_ACCEL) {
            marioObj->header.gfx.animInfo.animFrame = targetAnim->startFrame;
        } else {
            if (targetAnim->flags & ANIM_FLAG_FORWARD) {
                marioObj->header.gfx.animInfo.animFrame = targetAnim->startFrame + 1;
            } else {
                marioObj->header.gfx.animInfo.animFrame = targetAnim->startFrame - 1;
            }
        }
    }

    return marioObj->header.gfx.animInfo.animFrame;
}

/**
 * Sets Mario's animation where the animation is sped up or
 * slowed down via acceleration.
 */
s16 set_mario_anim_with_accel(struct MarioState *m, s32 targetAnimID, s32 accel) {
    struct Object *marioObj = m->marioObj;
    struct Animation *targetAnim = m->animList->bufTarget;

    mario_transition_pose_begin(m, targetAnimID);
    if (load_patchable_table(m->animList, targetAnimID)) {
        targetAnim->values = (void *) VIRTUAL_TO_PHYSICAL((u8 *) targetAnim + (uintptr_t) targetAnim->values);
        targetAnim->index = (void *) VIRTUAL_TO_PHYSICAL((u8 *) targetAnim + (uintptr_t) targetAnim->index);
    }

    if (marioObj->header.gfx.animInfo.animID != targetAnimID) {
        marioObj->header.gfx.animInfo.animID = targetAnimID;
        marioObj->header.gfx.animInfo.curAnim = targetAnim;
        marioObj->header.gfx.animInfo.animYTrans = m->animYTrans;

        if (targetAnim->flags & ANIM_FLAG_NO_ACCEL) {
            marioObj->header.gfx.animInfo.animFrameAccelAssist = (targetAnim->startFrame << 0x10);
        } else {
            if (targetAnim->flags & ANIM_FLAG_FORWARD) {
                marioObj->header.gfx.animInfo.animFrameAccelAssist = (targetAnim->startFrame << 0x10) + accel;
            } else {
                marioObj->header.gfx.animInfo.animFrameAccelAssist = (targetAnim->startFrame << 0x10) - accel;
            }
        }

        marioObj->header.gfx.animInfo.animFrame = (marioObj->header.gfx.animInfo.animFrameAccelAssist >> 0x10);
    }

    marioObj->header.gfx.animInfo.animAccel = accel;

    return marioObj->header.gfx.animInfo.animFrame;
}

/**
 * Sets the animation to a specific "next" frame from the frame given.
 */
void set_anim_to_frame(struct MarioState *m, s16 animFrame) {
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    struct Animation *curAnim = animInfo->curAnim;

    if (animInfo->animAccel) {
        if (curAnim->flags & ANIM_FLAG_FORWARD) {
            animInfo->animFrameAccelAssist = (animFrame << 0x10) + animInfo->animAccel;
        } else {
            animInfo->animFrameAccelAssist = (animFrame << 0x10) - animInfo->animAccel;
        }
    } else {
        if (curAnim->flags & ANIM_FLAG_FORWARD) {
            animInfo->animFrame = animFrame + 1;
        } else {
            animInfo->animFrame = animFrame - 1;
        }
    }
}

s32 is_anim_past_frame(struct MarioState *m, s16 animFrame) {
    s32 isPastFrame;
    s32 acceleratedFrame = animFrame << 0x10;
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    struct Animation *curAnim = animInfo->curAnim;

    if (animInfo->animAccel) {
        if (curAnim->flags & ANIM_FLAG_FORWARD) {
            isPastFrame =
                (animInfo->animFrameAccelAssist > acceleratedFrame)
                && (acceleratedFrame >= (animInfo->animFrameAccelAssist - animInfo->animAccel));
        } else {
            isPastFrame =
                (animInfo->animFrameAccelAssist < acceleratedFrame)
                && (acceleratedFrame <= (animInfo->animFrameAccelAssist + animInfo->animAccel));
        }
    } else {
        if (curAnim->flags & ANIM_FLAG_FORWARD) {
            isPastFrame = (animInfo->animFrame == (animFrame + 1));
        } else {
            isPastFrame = ((animInfo->animFrame + 1) == animFrame);
        }
    }

    return isPastFrame;
}

/**
 * Rotates the animation's translation into the global coordinate system
 * and returns the animation's flags.
 */
s16 find_mario_anim_flags_and_translation(struct Object *obj, s32 yaw, Vec3s translation) {
    f32 dx, dz;

    struct Animation *curAnim = (void *) obj->header.gfx.animInfo.curAnim;
    s16 animFrame = geo_update_animation_frame(&obj->header.gfx.animInfo, NULL);
    u16 *animIndex = segmented_to_virtual((void *) curAnim->index);
    s16 *animValues = segmented_to_virtual((void *) curAnim->values);

    f32 s = (f32) sins(yaw);
    f32 c = (f32) coss(yaw);

    dx = *(animValues + (retrieve_animation_index(animFrame, &animIndex))) / 4.0f;
    translation[1] = *(animValues + (retrieve_animation_index(animFrame, &animIndex))) / 4.0f;
    dz = *(animValues + (retrieve_animation_index(animFrame, &animIndex))) / 4.0f;

    translation[0] = ( dx * c) + (dz * s);
    translation[2] = (-dx * s) + (dz * c);

    return curAnim->flags;
}

/**
 * Updates Mario's position from his animation's translation.
 */
void update_mario_pos_for_anim(struct MarioState *m) {
    Vec3s translation;
    s16 flags;

    flags = find_mario_anim_flags_and_translation(m->marioObj, m->faceAngle[1], translation);

    if (flags & (ANIM_FLAG_HOR_TRANS | ANIM_FLAG_NO_TRANS)) {
        m->pos[0] += (f32) translation[0];
        m->pos[2] += (f32) translation[2];
    }

    if (flags & (ANIM_FLAG_VERT_TRANS | ANIM_FLAG_NO_TRANS)) {
        m->pos[1] += (f32) translation[1];
    }
}

/**
 * Finds the vertical translation from Mario's animation.
 */
s16 return_mario_anim_y_translation(struct MarioState *m) {
    Vec3s translation;
    find_mario_anim_flags_and_translation(m->marioObj, 0, translation);

    return translation[1];
}

/**************************************************
 *                      AUDIO                     *
 **************************************************/

/**
 * Plays a sound if if Mario doesn't have the flag being checked.
 */
void play_sound_if_no_flag(struct MarioState *m, u32 soundBits, u32 flags) {
    if (!(m->flags & flags)) {
        play_sound(soundBits, m->marioObj->header.gfx.cameraToObject);
        m->flags |= flags;
    }
}

/**
 * Plays a jump sound if one has not been played since the last action change.
 */
void play_mario_jump_sound(struct MarioState *m) {
    if (!(m->flags & MARIO_MARIO_SOUND_PLAYED)) {
        if (m->action == ACT_TRIPLE_JUMP) {
            play_sound(SOUND_MARIO_YAHOO_WAHA_YIPPEE + ((gAudioRandom % 5) << 16),
                       m->marioObj->header.gfx.cameraToObject);
        } else {
            play_sound(SOUND_MARIO_YAH_WAH_HOO + ((gAudioRandom % 3) << 16),
                       m->marioObj->header.gfx.cameraToObject);
        }
        m->flags |= MARIO_MARIO_SOUND_PLAYED;
    }
}

/**
 * Adjusts the volume/pitch of sounds from Mario's speed.
 */
void adjust_sound_for_speed(struct MarioState *m) {
    s32 absForwardVel = (m->forwardVel > 0.0f) ? m->forwardVel : -m->forwardVel;
    set_sound_moving_speed(SOUND_BANK_MOVING, (absForwardVel > 100) ? 100 : absForwardVel);
}

/**
 * Spawns particles if the step sound says to, then either plays a step sound or relevant other sound.
 */
void play_sound_and_spawn_particles(struct MarioState *m, u32 soundBits, u32 waveParticleType) {
    // Surface-supported steps disturb the water even when Vanish keeps dry-floor sounds.
    if (m->terrainSoundAddend == (SOUND_TERRAIN_WATER << 16)
        || (mario_has_current_vanish_cap(m) && m->floor == &gWaterSurfacePseudoFloor)) {
        if (waveParticleType != 0) {
            m->particleFlags |= PARTICLE_SHALLOW_WATER_SPLASH;
        } else {
            m->particleFlags |= PARTICLE_SHALLOW_WATER_WAVE;
        }
    } else {
        if (m->terrainSoundAddend == (SOUND_TERRAIN_SAND << 16)) {
            m->particleFlags |= PARTICLE_DIRT;
        } else if (m->terrainSoundAddend == (SOUND_TERRAIN_SNOW << 16)) {
            m->particleFlags |= PARTICLE_SNOW;
        }
    }

    if ((m->flags & MARIO_METAL_CAP) || soundBits == SOUND_ACTION_UNSTUCK_FROM_GROUND
        || soundBits == SOUND_MARIO_PUNCH_HOO) {
        play_sound(soundBits, m->marioObj->header.gfx.cameraToObject);
    } else {
        play_sound(m->terrainSoundAddend + soundBits, m->marioObj->header.gfx.cameraToObject);
    }
}

/**
 * Plays an environmental sound if one has not been played since the last action change.
 */
void play_mario_action_sound(struct MarioState *m, u32 soundBits, u32 waveParticleType) {
    if (!(m->flags & MARIO_ACTION_SOUND_PLAYED)) {
        play_sound_and_spawn_particles(m, soundBits, waveParticleType);
        m->flags |= MARIO_ACTION_SOUND_PLAYED;
    }
}

/**
 * Plays a landing sound, accounting for metal cap.
 */
void play_mario_landing_sound(struct MarioState *m, u32 soundBits) {
    if (m->metalFallGroundPoundImpact) {
        m->flags |= MARIO_ACTION_SOUND_PLAYED;
        return;
    }
    play_sound_and_spawn_particles(
        m, (m->flags & MARIO_METAL_CAP) ? SOUND_ACTION_METAL_LANDING : soundBits, 1);
}

/**
 * Plays a landing sound, accounting for metal cap. Unlike play_mario_landing_sound,
 * this function uses play_mario_action_sound, making sure the sound is only
 * played once per action.
 */
void play_mario_landing_sound_once(struct MarioState *m, u32 soundBits) {
    if (m->metalFallGroundPoundImpact) {
        m->flags |= MARIO_ACTION_SOUND_PLAYED;
        return;
    }
    play_mario_action_sound(m, (m->flags & MARIO_METAL_CAP) ? SOUND_ACTION_METAL_LANDING : soundBits,
                            1);
}

/**
 * Plays a heavy landing (ground pound, etc.) sound, accounting for metal cap.
 */
void play_mario_heavy_landing_sound(struct MarioState *m, u32 soundBits) {
    play_sound_and_spawn_particles(
        m, (m->flags & MARIO_METAL_CAP) ? SOUND_ACTION_METAL_HEAVY_LANDING : soundBits, 1);
}

/**
 * Plays a heavy landing (ground pound, etc.) sound, accounting for metal cap.
 * Unlike play_mario_heavy_landing_sound, this function uses play_mario_action_sound,
 * making sure the sound is only played once per action.
 */
void play_mario_heavy_landing_sound_once(struct MarioState *m, u32 soundBits) {
    play_mario_action_sound(
        m, (m->flags & MARIO_METAL_CAP) ? SOUND_ACTION_METAL_HEAVY_LANDING : soundBits, 1);
}

/**
 * Plays action and Mario sounds relevant to what was passed into the function.
 */
void play_mario_sound(struct MarioState *m, s32 actionSound, s32 marioSound) {
    if (actionSound == SOUND_ACTION_TERRAIN_JUMP) {
        play_mario_action_sound(m, (m->flags & MARIO_METAL_CAP) ? (s32) SOUND_ACTION_METAL_JUMP
                                                                : (s32) SOUND_ACTION_TERRAIN_JUMP, 1);
    } else {
        play_sound_if_no_flag(m, actionSound, MARIO_ACTION_SOUND_PLAYED);
    }

    if (marioSound == 0) {
        play_mario_jump_sound(m);
    }

    if (marioSound != -1) {
        play_sound_if_no_flag(m, marioSound, MARIO_MARIO_SOUND_PLAYED);
    }
}

/**************************************************
 *                     ACTIONS                    *
 **************************************************/

// Limit each signed movement component independently.
void mario_limit_velocity_vector(Vec3f velocity) {
    f32 limit = MARIO_ACTIVE_SPEED_CAP;
    velocity[0] = CLAMP(velocity[0], -limit, limit);
    velocity[1] = CLAMP(velocity[1], -limit, limit);
    velocity[2] = CLAMP(velocity[2], -limit, limit);
}

void mario_limit_velocity(struct MarioState *m) {
    mario_limit_velocity_vector(m->vel);
    f32 limit = MARIO_ACTIVE_SPEED_CAP;
    m->forwardVel = CLAMP(m->forwardVel, -limit, limit);

    // Some actions hold slide speed separately from the velocity used this frame.
    m->slideVelX = CLAMP(m->slideVelX, -limit, limit);
    m->slideVelZ = CLAMP(m->slideVelZ, -limit, limit);
}

/**
 * Sets Mario's other velocities from his forward speed.
 */
void mario_set_forward_vel(struct MarioState *m, f32 forwardVel) {
    f32 limit = MARIO_ACTIVE_SPEED_CAP;
    m->forwardVel = CLAMP(forwardVel, -limit, limit);

    m->slideVelX = sins(m->faceAngle[1]) * m->forwardVel;
    m->slideVelZ = coss(m->faceAngle[1]) * m->forwardVel;

    m->vel[0] = (f32) m->slideVelX;
    m->vel[2] = (f32) m->slideVelZ;
}

/**
 * Returns the slipperiness class of Mario's floor.
 */
s32 mario_get_floor_class(struct MarioState *m) {
    s32 floorClass;

    if (m->floor == &gWaterSurfacePseudoFloor
        && (m->flags & (MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP))
        && !(m->action & ACT_FLAG_RIDING_SHELL)) {
        return SURFACE_CLASS_DEFAULT;
    }

    if (m->floor != NULL && m->floor->object != NULL
        && m->floor->object->behavior == segmented_to_virtual(bhvTiltingBowserLavaPlatform)
        && m->floor->object->oAction != 0) {
        return game_options_cheat_active(GO_IGNORE_SLIPPERY) ? SURFACE_CLASS_DEFAULT
                                                             : SURFACE_CLASS_VERY_SLIPPERY;
    }

    // The slide terrain type defaults to slide slipperiness.
    // This doesn't matter too much since normally the slide terrain
    // is checked for anyways.
    if ((m->area->terrainType & TERRAIN_MASK) == TERRAIN_SLIDE) {
        floorClass = SURFACE_CLASS_VERY_SLIPPERY;
    } else {
        floorClass = SURFACE_CLASS_DEFAULT;
    }

    if (m->floor != NULL) {
        switch (m->floor->type) {
            case SURFACE_MARIO_ONLY_GRIPPY:
            case SURFACE_GRIPPY:
            case SURFACE_NOT_SLIPPERY:
            case SURFACE_HARD_NOT_SLIPPERY:
            case SURFACE_SWITCH:
                floorClass = SURFACE_CLASS_NOT_SLIPPERY;
                break;

            case SURFACE_SLIPPERY:
            case SURFACE_NOISE_SLIPPERY:
            case SURFACE_HARD_SLIPPERY:
            case SURFACE_NO_CAM_COL_SLIPPERY:
                floorClass = SURFACE_CLASS_SLIPPERY;
                break;

            case SURFACE_SUPER_SLIPPERY:
            case SURFACE_VERY_SLIPPERY:
            case SURFACE_ICE:
            case SURFACE_HARD_VERY_SLIPPERY:
            case SURFACE_NOISE_VERY_SLIPPERY_73:
            case SURFACE_NOISE_VERY_SLIPPERY_74:
            case SURFACE_NOISE_VERY_SLIPPERY:
            case SURFACE_NO_CAM_COL_VERY_SLIPPERY:
                floorClass = SURFACE_CLASS_VERY_SLIPPERY;
                break;
        }
    }

    if (game_options_cheat_active(GO_IGNORE_SLIPPERY)
        && (floorClass == SURFACE_CLASS_SLIPPERY || floorClass == SURFACE_CLASS_VERY_SLIPPERY)) {
        floorClass = SURFACE_CLASS_DEFAULT;
    }

    // Crawling allows Mario to not slide on certain steeper surfaces.
    if (m->action == ACT_CRAWLING && m->floor->normal.y > 0.5f && floorClass == SURFACE_CLASS_DEFAULT) {
        floorClass = SURFACE_CLASS_NOT_SLIPPERY;
    }

    return floorClass;
}

// clang-format off
s8 sTerrainSounds[7][6] = {
    // default,              hard,                 slippery,
    // very slippery,        noisy default,        noisy slippery
    { SOUND_TERRAIN_DEFAULT, SOUND_TERRAIN_STONE,  SOUND_TERRAIN_GRASS,
      SOUND_TERRAIN_GRASS,   SOUND_TERRAIN_GRASS,  SOUND_TERRAIN_DEFAULT }, // TERRAIN_GRASS
    { SOUND_TERRAIN_STONE,   SOUND_TERRAIN_STONE,  SOUND_TERRAIN_STONE,
      SOUND_TERRAIN_STONE,   SOUND_TERRAIN_GRASS,  SOUND_TERRAIN_GRASS }, // TERRAIN_STONE
    { SOUND_TERRAIN_SNOW,    SOUND_TERRAIN_ICE,    SOUND_TERRAIN_SNOW,
      SOUND_TERRAIN_ICE,     SOUND_TERRAIN_STONE,  SOUND_TERRAIN_STONE }, // TERRAIN_SNOW
    { SOUND_TERRAIN_SAND,    SOUND_TERRAIN_STONE,  SOUND_TERRAIN_SAND,
      SOUND_TERRAIN_SAND,    SOUND_TERRAIN_STONE,  SOUND_TERRAIN_STONE }, // TERRAIN_SAND
    { SOUND_TERRAIN_SPOOKY,  SOUND_TERRAIN_SPOOKY, SOUND_TERRAIN_SPOOKY,
      SOUND_TERRAIN_SPOOKY,  SOUND_TERRAIN_STONE,  SOUND_TERRAIN_STONE }, // TERRAIN_SPOOKY
    { SOUND_TERRAIN_DEFAULT, SOUND_TERRAIN_STONE,  SOUND_TERRAIN_GRASS,
      SOUND_TERRAIN_ICE,     SOUND_TERRAIN_STONE,  SOUND_TERRAIN_ICE }, // TERRAIN_WATER
    { SOUND_TERRAIN_STONE,   SOUND_TERRAIN_STONE,  SOUND_TERRAIN_STONE,
      SOUND_TERRAIN_STONE,   SOUND_TERRAIN_ICE,    SOUND_TERRAIN_ICE }, // TERRAIN_SLIDE
};
// clang-format on

/**
 * Computes a value that should be added to terrain sounds before playing them.
 * This depends on surfaces and terrain.
 */
u32 mario_get_terrain_sound_addend(struct MarioState *m) {
    s16 floorSoundType;
    s16 terrainType = m->area->terrainType & TERRAIN_MASK;
    s32 ret = SOUND_TERRAIN_DEFAULT << 16;
    s32 floorType;

    if (m->floor != NULL) {
        floorType = m->floor->type;

        if (!mario_has_current_vanish_cap(m) && (gCurrLevelNum != LEVEL_LLL)
            && (m->floorHeight < (m->waterLevel - 10)
                || ((m->flags & (MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP))
                    && m->floor == &gWaterSurfacePseudoFloor))) {
            // Water terrain sound, excluding LLL since it uses water in the volcano.
            ret = SOUND_TERRAIN_WATER << 16;
        } else if (SURFACE_IS_QUICKSAND(floorType)) {
            ret = SOUND_TERRAIN_SAND << 16;
        } else {
            switch (floorType) {
                default:
                    floorSoundType = 0;
                    break;

                case SURFACE_MARIO_ONLY_GRIPPY:
                case SURFACE_GRIPPY:
                case SURFACE_NOT_SLIPPERY:
                case SURFACE_HARD:
                case SURFACE_HARD_NOT_SLIPPERY:
                case SURFACE_SWITCH:
                    floorSoundType = 1;
                    break;

                case SURFACE_SLIPPERY:
                case SURFACE_HARD_SLIPPERY:
                case SURFACE_NO_CAM_COL_SLIPPERY:
                    floorSoundType = 2;
                    break;

                case SURFACE_SUPER_SLIPPERY:
                case SURFACE_VERY_SLIPPERY:
                case SURFACE_ICE:
                case SURFACE_HARD_VERY_SLIPPERY:
                case SURFACE_NOISE_VERY_SLIPPERY_73:
                case SURFACE_NOISE_VERY_SLIPPERY_74:
                case SURFACE_NOISE_VERY_SLIPPERY:
                case SURFACE_NO_CAM_COL_VERY_SLIPPERY:
                    floorSoundType = 3;
                    break;

                case SURFACE_NOISE_DEFAULT:
                    floorSoundType = 4;
                    break;

                case SURFACE_NOISE_SLIPPERY:
                    floorSoundType = 5;
                    break;
            }

            ret = sTerrainSounds[terrainType][floorSoundType] << 16;
        }
    }

    return ret;
}

/**
 * Determines if Mario is facing "downhill."
 */
s32 mario_facing_downhill(struct MarioState *m, s32 turnYaw) {
    // Forces Mario to do a belly slide rather than a butt slide when on a super slippery floor, no
    // matter his angle, so that the player can't jump.
    if (!game_options_cheat_active(GO_IGNORE_SLIPPERY) && m->floor
        && m->floor->type == SURFACE_SUPER_SLIPPERY) {
        return FALSE;
    }

    s16 faceAngleYaw = m->faceAngle[1];

    // This is never used in practice, as turnYaw is
    // always passed as zero.
    if (turnYaw && m->forwardVel < 0.0f) {
        faceAngleYaw += 0x8000;
    }

    faceAngleYaw = m->floorYaw - faceAngleYaw;

    return (-0x4000 < faceAngleYaw) && (faceAngleYaw < 0x4000);
}

/**
 * Determines if a surface is slippery based on the surface class.
 */
u32 mario_floor_is_slippery(struct MarioState *m) {
    f32 normY;
    s32 metalTraction =
        mario_has_current_metal_cap(m) && (m->area->terrainType & TERRAIN_MASK) != TERRAIN_SLIDE;

    if (!game_options_cheat_active(GO_IGNORE_SLIPPERY)
        && (((m->area->terrainType & TERRAIN_MASK) == TERRAIN_SLIDE && m->floor->normal.y < COS1)
            || m->floor->type == SURFACE_SUPER_SLIPPERY)) {
        return TRUE;
    }

    switch (mario_get_floor_class(m)) {
        case SURFACE_CLASS_VERY_SLIPPERY:
            normY = metalTraction ? COS15 : COS10;
            break;
        case SURFACE_CLASS_SLIPPERY:
            normY = metalTraction ? COS25 : COS20;
            break;
        default:
            normY = metalTraction ? COS43 : COS38;
            break;
        case SURFACE_CLASS_NOT_SLIPPERY:  normY = 0.0f;  break;
    }

    return m->floor->normal.y <= normY;
}

/**
 * Determines if a surface is a slope based on the surface class.
 */
s32 mario_floor_is_slope(struct MarioState *m) {
    if (m->floor->type == SURFACE_GRIPPY || m->floor->type == SURFACE_MARIO_ONLY_GRIPPY) {
        return FALSE;
    }
    f32 normY;

    if (!game_options_cheat_active(GO_IGNORE_SLIPPERY)
        && (((m->area->terrainType & TERRAIN_MASK) == TERRAIN_SLIDE && m->floor->normal.y < COS1)
            || m->floor->type == SURFACE_SUPER_SLIPPERY)) {
        return TRUE;
    }

    switch (mario_get_floor_class(m)) {
        case SURFACE_CLASS_VERY_SLIPPERY:
            normY = COS5;
            break;
        case SURFACE_CLASS_SLIPPERY:
            normY = COS10;
            break;
        default:
            normY = COS15;
            break;
        case SURFACE_CLASS_NOT_SLIPPERY:
            normY = COS20;
            break;
    }

    return m->floor->normal.y <= normY;
}

/**
 * Determines if a surface is steep based on the surface class.
 */
s32 mario_floor_is_steep(struct MarioState *m) {
    if (m->floor->type == SURFACE_GRIPPY || m->floor->type == SURFACE_MARIO_ONLY_GRIPPY) {
        return FALSE;
    }
    f32 normY;
    s32 metalTraction =
        mario_has_current_metal_cap(m) && (m->area->terrainType & TERRAIN_MASK) != TERRAIN_SLIDE;
    if (!game_options_cheat_active(GO_IGNORE_SLIPPERY) && m->floor->type == SURFACE_SUPER_SLIPPERY) {
        return TRUE;
    }

#ifdef JUMP_KICK_FIX
    if (m->floor->type == SURFACE_NOT_SLIPPERY) {
        return FALSE;
    }
#endif

    // Interestingly, this function does not check for the
    // slide terrain type. This means that steep behavior persists for
    // non-slippery and slippery surfaces.
    // This does not matter in vanilla game practice.
    if (!mario_facing_downhill(m, FALSE)) {
        switch (mario_get_floor_class(m)) {
            case SURFACE_CLASS_VERY_SLIPPERY:
                normY = metalTraction ? COS20 : COS15;
                break;
            case SURFACE_CLASS_SLIPPERY:
                normY = metalTraction ? COS25 : COS20;
                break;
            default:
                normY = metalTraction ? COS35 : COS30;
                break;
            case SURFACE_CLASS_NOT_SLIPPERY:
                normY = metalTraction ? COS35 : COS30;
                break;
        }

        return m->floor->normal.y <= normY;
    }

    return FALSE;
}

/**
 * Finds the floor height relative from Mario given polar displacement.
 */
f32 find_floor_height_relative_polar(struct MarioState *m, s16 angleFromMario, f32 distFromMario) {
    struct Surface *floor;

    f32 y = sins(m->faceAngle[1] + angleFromMario) * distFromMario;
    f32 x = coss(m->faceAngle[1] + angleFromMario) * distFromMario;

    return find_floor(m->pos[0] + y, m->pos[1] + 100.0f, m->pos[2] + x, &floor);
}

/**
 * Returns the slope of the floor based off points around Mario.
 */
s16 find_floor_slope(struct MarioState *m, s16 yawOffset) {
    struct Surface *floor = m->floor;
    f32 forwardFloorY, backwardFloorY;
    f32 forwardYDelta, backwardYDelta;
    s16 result;

    f32 x = sins(m->faceAngle[1] + yawOffset) * 5.0f;
    f32 z = coss(m->faceAngle[1] + yawOffset) * 5.0f;
#ifdef FAST_FLOOR_ALIGN
    if (absf(m->forwardVel) > FAST_FLOOR_ALIGN) {
        forwardFloorY  = get_surface_height_at_location(m->pos[0] + x, m->pos[2] + z, floor);
        backwardFloorY = get_surface_height_at_location(m->pos[0] - x, m->pos[2] - z, floor);
    } else {
        forwardFloorY  = find_floor(m->pos[0] + x, m->pos[1] + 100.0f, m->pos[2] + z, &floor);
        if (floor == NULL)  forwardFloorY = m->floorHeight; // handle OOB slopes
        backwardFloorY = find_floor(m->pos[0] - x, m->pos[1] + 100.0f, m->pos[2] - z, &floor);
        if (floor == NULL) backwardFloorY = m->floorHeight; // handle OOB slopes
    }
#else
    forwardFloorY  = find_floor(m->pos[0] + x, m->pos[1] + 100.0f, m->pos[2] + z, &floor);
    if (floor == NULL)  forwardFloorY = m->floorHeight; // handle OOB slopes
    backwardFloorY = find_floor(m->pos[0] - x, m->pos[1] + 100.0f, m->pos[2] - z, &floor);
    if (floor == NULL) backwardFloorY = m->floorHeight; // handle OOB slopes
#endif

    forwardYDelta = forwardFloorY - m->pos[1];
    backwardYDelta = m->pos[1] - backwardFloorY;

    if (sqr(forwardYDelta) < sqr(backwardYDelta)) {
        result = atan2s(5.0f, forwardYDelta);
    } else {
        result = atan2s(5.0f, backwardYDelta);
    }

    return result;
}

Bool32 set_mario_wall(struct MarioState *m, struct Surface *wall) {
    if (m->wall != wall) {
        m->wall = wall;
        if (m->wall != NULL) m->wallYaw = SURFACE_YAW(wall);
    }
    return (m->wall != NULL);
}

Bool32 set_mario_ceil(struct MarioState *m, struct Surface *ceil, f32 ceilHeight) {
    if (m->ceil != ceil) {
        m->ceil = ceil;
        if (m->ceil != NULL) m->ceilYaw = SURFACE_YAW(ceil);
    }
    m->ceilHeight = ceilHeight;
    return (m->ceil != NULL);
}

Bool32 set_mario_floor(struct MarioState *m, struct Surface *floor, f32 floorHeight) {
    if (m->floor != floor) {
        m->floor = floor;
        if (m->floor != NULL) m->floorYaw = SURFACE_YAW(floor);
    }
    m->floorHeight = floorHeight;
    return (m->floor != NULL);
}

/**
 * Adjusts Mario's camera and sound based on his action status.
 */
void update_mario_sound_and_camera(struct MarioState *m) {
    u32 action = m->action;
    s32 camPreset = m->area->camera->mode;

    if (action == ACT_FIRST_PERSON) {
        raise_background_noise(2);
        gCameraMovementFlags &= ~CAM_MOVE_C_UP_MODE;
        // Go back to the last camera mode
        set_camera_mode(m->area->camera, -1, 1);
    } else if (action == ACT_SLEEPING) {
        raise_background_noise(2);
    }

    if (!(action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER)) && (camPreset == DEEP_WATER_CAMERA_MODE || camPreset == WATER_SURFACE_CAMERA_MODE)) {
        set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
    }
}

/**
 * Transitions Mario to a steep jump action.
 */
void set_steep_jump_action(struct MarioState *m) {
    m->marioObj->oMarioSteepJumpYaw = m->faceAngle[1];

    if (m->forwardVel > 0.0f) {
        //! ((s16)0x8000) has undefined behavior. Therefore, this downcast has
        // undefined behavior if m->floorYaw >= 0.
        s16 angleTemp = m->floorYaw + 0x8000;
        s16 faceAngleTemp = m->faceAngle[1] - angleTemp;

        f32 y = sins(faceAngleTemp) * m->forwardVel;
        f32 x = coss(faceAngleTemp) * m->forwardVel * 0.75f;

        m->forwardVel = sqrtf(sqr(y) + sqr(x));
        m->faceAngle[1] = atan2s(x, y) + angleTemp;
    }

    drop_and_set_mario_action(m, ACT_STEEP_JUMP, 0);
}

/**
 * Sets Mario's vertical speed from his forward speed.
 */
void set_mario_y_vel_based_on_fspeed(struct MarioState *m, f32 initialVelY, f32 multiplier) {
    // get_additive_y_vel_for_jumps is always 0 and a stubbed function.
    // It was likely trampoline related based on code location.
    m->vel[1] = initialVelY + get_additive_y_vel_for_jumps() + m->forwardVel * multiplier;

    if (m->squishTimer != 0 || m->quicksandDepth > 1.0f) {
        m->vel[1] *= 0.5f;
    }
}

/**
 * Transitions for a variety of airborne actions.
 */
static f32 ground_dive_launch_velocity(f32 velocity) {
    s32 frames = 1;
    f32 height;
    velocity = CLAMP(velocity, 0.0f, MARIO_ACTIVE_SPEED_CAP);
    // Movement precedes gravity: sum v, v-4, ... through the apex.
    while (velocity > 4.0f * frames) {
        frames++;
    }
    height = GROUND_DIVE_HEIGHT_RETENTION * (frames * velocity - 2.0f * frames * (frames - 1));
    frames = 1;
    while (height > 2.0f * frames * (frames + 1)) {
        frames++;
    }
    return (height + 2.0f * frames * (frames - 1)) / frames;
}

u32 set_mario_action_airborne(struct MarioState *m, u32 action, u32 actionArg) {
    if ((m->squishTimer != 0 || m->quicksandDepth >= 1.0f)
        && (action == ACT_DOUBLE_JUMP || action == ACT_TWIRLING)) {
        action = ACT_JUMP;
    }

    switch (action) {
        case ACT_DOUBLE_JUMP:
            set_mario_y_vel_based_on_fspeed(m, 52.0f, m->movementProfile->jumpSpeedContribution);
            m->vel[1] *= m->movementProfile->jumpScale;
            m->forwardVel *= m->movementProfile->jumpRetention;
            break;

        case ACT_BACKFLIP:
            m->marioObj->header.gfx.animInfo.animID = -1;
            m->forwardVel = -16.0f;
            set_mario_y_vel_based_on_fspeed(m, 62.0f, 0.0f);
            m->vel[1] *= m->movementProfile->jumpScale;
            break;

        case ACT_TRIPLE_JUMP:
            set_mario_y_vel_based_on_fspeed(m, 69.0f, 0.0f);
            m->vel[1] *= m->movementProfile->jumpScale;
            m->forwardVel *= m->movementProfile->jumpRetention;
            break;

        case ACT_FLYING_TRIPLE_JUMP:
            set_mario_y_vel_based_on_fspeed(m, 82.0f, 0.0f);
            m->vel[1] *= m->movementProfile->jumpScale;
            break;

        case ACT_WATER_JUMP:
        case ACT_HOLD_WATER_JUMP:
            // Preserve horizontal velocity; swimming forward speed also includes pitch.
            m->forwardVel = sqrtf(sqr(m->vel[0]) + sqr(m->vel[2]));
            m->slideVelX = m->vel[0];
            m->slideVelZ = m->vel[2];
            if (actionArg == 0) {
                set_mario_y_vel_based_on_fspeed(m, 42.0f, 0.0f);
            }
            break;

        case ACT_BURNING_JUMP:
            m->vel[1] = 31.5f;
            m->forwardVel = 8.0f;
            break;

        case ACT_RIDING_SHELL_JUMP:
            set_mario_y_vel_based_on_fspeed(m, 42.0f, 0.25f);
            break;

        case ACT_JUMP:
        case ACT_HOLD_JUMP:
            m->marioObj->header.gfx.animInfo.animID = -1;
            set_mario_y_vel_based_on_fspeed(
                m, 42.0f, action == ACT_JUMP ? m->movementProfile->jumpSpeedContribution : 0.25f);
            m->vel[1] *= m->movementProfile->jumpScale;
            m->forwardVel *= m->movementProfile->jumpRetention;
            break;

        case ACT_WALL_KICK_AIR:
            set_mario_y_vel_based_on_fspeed(m, 62.0f, 0.0f);
            if (m->forwardVel < WALL_KICK_MIN_FORWARD_SPEED) {
                m->forwardVel = WALL_KICK_MIN_FORWARD_SPEED;
            }
            m->wallKickTimer = 0;
            break;

        case ACT_TOP_OF_POLE_JUMP:
            set_mario_y_vel_based_on_fspeed(m, 62.0f, 0.0f);
            if (m->forwardVel < 24.0f) {
                m->forwardVel = 24.0f;
            }
            m->wallKickTimer = 0;
            break;

        case ACT_SIDE_FLIP:
            set_mario_y_vel_based_on_fspeed(m, 62.0f, 0.0f);
            m->forwardVel = 8.0f;
            m->faceAngle[1] = m->intendedYaw;
            break;

        case ACT_STEEP_JUMP:
            m->marioObj->header.gfx.animInfo.animID = -1;
            set_mario_y_vel_based_on_fspeed(m, 42.0f, 0.25f);
            m->faceAngle[0] = -0x2000;
            break;

        case ACT_LAVA_BOOST:
            m->vel[1] = 84.0f;
            if (actionArg == ACT_ARG_LAVA_BOOST_FLOOR
                || actionArg == ACT_ARG_LAVA_BOOST_METAL_GROUND_POUND) {
                m->forwardVel = 0.0f;
            }
            break;

        case ACT_DIVE:
            if (m->groundTakeoffThisFrame
                && (m->action == ACT_JUMP || m->action == ACT_DOUBLE_JUMP
                    || m->action == ACT_TRIPLE_JUMP)
                && (m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED))
                       == (INPUT_A_PRESSED | INPUT_B_PRESSED)) {
                m->vel[1] = ground_dive_launch_velocity(m->vel[1]);
            }
            m->groundTakeoffThisFrame = FALSE;
            mario_set_forward_vel(m, m->forwardVel + FORWARD_DIVE_SPEED_BOOST);
            break;

        case ACT_GROUND_POUND:
            // Forward momentum brakes over the startup instead of snapping down on entry.
            if (m->forwardVel <= 0.0f) {
                mario_set_forward_vel(m, MAX(-8.0f, m->forwardVel * 0.25f));
            }
            break;

        case ACT_LONG_JUMP: {
            f32 entrySpeed = m->forwardVel;
            m->marioObj->header.gfx.animInfo.animID = -1;
            set_mario_y_vel_based_on_fspeed(m, 30.0f, 0.0f);
            m->marioObj->oMarioLongJumpIsSlow = m->forwardVel > 16.0f ? FALSE : TRUE;

            // Cap the boost at 52 without slowing faster entries. Backward
            // long jumps use the same 1.2x boost.
            m->forwardVel =
                MIN(entrySpeed * LONG_JUMP_SPEED_MULTIPLIER, MAX(LONG_JUMP_BOOST_CAP, entrySpeed));
            break;
        }

        case ACT_SLIDE_KICK:
            // Start just above the floor; the launch and gravity give about 3.5x vanilla rise.
            m->pos[1] = MAX(m->pos[1], m->floorHeight + 4.0f);
            m->vel[1] = 25.0f;
            break;

        case ACT_JUMP_KICK:
            // Kicking straight after takeoff should not carry the full jump launch.
            if (m->framesSinceA <= 3
                && (m->action == ACT_JUMP || m->action == ACT_HOLD_JUMP || m->action == ACT_DOUBLE_JUMP
                    || m->action == ACT_TRIPLE_JUMP || m->action == ACT_STEEP_JUMP)) {
                m->vel[1] *= 0.75f;
            }
            m->vel[1] = MAX(m->vel[1], 20.0f);
            break;
    }

    m->peakHeight = m->pos[1];
    m->flags |= MARIO_JUMPING;

    return action;
}

/**
 * Transitions for a variety of moving actions.
 */
u32 set_mario_action_moving(struct MarioState *m, u32 action, UNUSED u32 actionArg) {
    s16 floorClass = mario_get_floor_class(m);
    f32 forwardVel = m->forwardVel;
    f32 mag = MIN(m->intendedMag, 8.0f);

    switch (action) {
        case ACT_WALKING:
            if (floorClass != SURFACE_CLASS_VERY_SLIPPERY) {
                if (0.0f <= forwardVel && forwardVel < mag) {
                    m->forwardVel = mag;
                }
            }

            m->marioObj->oMarioWalkingPitch = 0;
            break;

        case ACT_HOLD_WALKING:
            if (0.0f <= forwardVel && forwardVel < mag / 2.0f) {
                m->forwardVel = mag / 2.0f;
            }
            break;

        case ACT_BEGIN_SLIDING:
            if (mario_facing_downhill(m, FALSE)) {
                action = ACT_BUTT_SLIDE;
            } else {
                action = ACT_STOMACH_SLIDE;
            }
            break;

        case ACT_HOLD_BEGIN_SLIDING:
            if (mario_facing_downhill(m, FALSE)) {
                action = ACT_HOLD_BUTT_SLIDE;
            } else {
                action = ACT_HOLD_STOMACH_SLIDE;
            }
            break;
    }

    f32 slideEntrySpeed = m->forwardVel;
    if ((m->action == ACT_JUMP_KICK || m->action == ACT_FREEFALL_LAND)
        && m->slopeJumpKickImpactSpeed < -16.0f) {
        slideEntrySpeed = m->slopeJumpKickImpactSpeed;
    }
    if ((action == ACT_BUTT_SLIDE || action == ACT_HOLD_BUTT_SLIDE) && m->action != ACT_BUTT_SLIDE
        && m->action != ACT_HOLD_BUTT_SLIDE && slideEntrySpeed < -16.0f && m->floor != NULL
        && mario_floor_is_slope(m)) {
        // Stop the first entry, launch the second slide forward at 10, then
        // reverse the incoming backward speed on later entries.
        // Use incoming speed, before jump-kick contact braking.
        f32 slideSpeed;
        if (m->backwardSlopeSlideCount == 0) {
            slideSpeed = 0.0f;
        } else if (m->backwardSlopeSlideCount == 1) {
            slideSpeed = 10.0f;
        } else {
            slideSpeed = MAX(-slideEntrySpeed, 20.0f);
        }
        mario_set_forward_vel(m, slideSpeed);
        if (m->backwardSlopeSlideCount < 3) {
            m->backwardSlopeSlideCount++;
        }
    }

    return action;
}

/**
 * Transition for certain submerged actions, which is actually just the metal jump actions.
 */
u32 set_mario_action_submerged(struct MarioState *m, u32 action, UNUSED u32 actionArg) {
    if (action == ACT_METAL_WATER_JUMP || action == ACT_HOLD_METAL_WATER_JUMP) {
        set_mario_y_vel_based_on_fspeed(
            m, 42.0f,
            action == ACT_METAL_WATER_JUMP ? m->movementProfile->jumpSpeedContribution : 0.25f);
        m->vel[1] *= m->movementProfile->jumpScale;
        m->forwardVel *= m->movementProfile->jumpRetention;
    }

    return action;
}

/**
 * Transitions for a variety of cutscene actions.
 */
u32 set_mario_action_cutscene(struct MarioState *m, u32 action, UNUSED u32 actionArg) {
    switch (action) {
        case ACT_DEBUG_FREE_MOVE:
            if (m->action == ACT_IN_CANNON) {
                if (m->usedObj != NULL) {
                    m->usedObj->oAction = OPENED_CANNON_ACT_SHOOT;
                }
                m->usedObj = NULL;
                m->statusForCamera->usedObj = NULL;
                m->statusForCamera->cameraEvent = CAM_EVENT_NONE;
                m->area->camera->cutscene = CUTSCENE_NONE;
            }
            // Flight interrupts cap animations, including their put-on cleanup.
            if (m->flags & MARIO_CAP_IN_HAND) {
                m->flags &= ~MARIO_CAP_IN_HAND;
                m->flags |= MARIO_CAP_ON_HEAD;
            }
            break;
        case ACT_EMERGE_FROM_PIPE:
            m->vel[1] = 52.0f;
            break;

        case ACT_FALL_AFTER_STAR_GRAB:
            mario_set_forward_vel(m, 0.0f);
            break;

        case ACT_SPAWN_SPIN_AIRBORNE:
            mario_set_forward_vel(m, 2.0f);
            break;

        case ACT_SPECIAL_EXIT_AIRBORNE:
        case ACT_SPECIAL_DEATH_EXIT:
            m->vel[1] = 64.0f;
            break;
    }

    return action;
}

/**
 * Puts Mario into a given action, putting Mario through the appropriate
 * specific function if needed.
 */
static void prepare_ground_takeoff(struct MarioState *m, u32 action) {
    u32 group = m->action & ACT_GROUP_MASK;
    if ((group == ACT_GROUP_STATIONARY || group == ACT_GROUP_MOVING || group == ACT_GROUP_OBJECT)
        && mario_is_on_floor(m)
        && (action == ACT_JUMP || action == ACT_DOUBLE_JUMP || action == ACT_TRIPLE_JUMP
            || action == ACT_HOLD_JUMP || action == ACT_BACKFLIP || action == ACT_LONG_JUMP
            || action == ACT_SIDE_FLIP || action == ACT_STEEP_JUMP)) {
        if (mario_has_current_vanish_cap(m) && mario_is_head_underwater(m)
            && m->pos[1] < m->waterLevel - 100.0f) {
            m->particleFlags |= PARTICLE_VANISH_WATER_RING;
        }
        m->input &= ~INPUT_Z_PRESSED;
        m->zCrouchBuffered = FALSE;
        m->groundTakeoffThisFrame =
            (m->input & INPUT_A_PRESSED)
            && (action == ACT_JUMP || action == ACT_DOUBLE_JUMP || action == ACT_TRIPLE_JUMP);
    } else if (!(action & ACT_FLAG_AIR)) {
        m->groundTakeoffThisFrame = FALSE;
    }
}

// Use the initialized destination while m->action still names the action being left.
static void reset_mario_action_state(struct MarioState *m, u32 action) {
    if (action == ACT_LONG_JUMP) {
        m->backwardLongJumpChain = m->forwardVel < 0.0f && m->controller->stickMag > 0.0f;
    } else if (action != ACT_LONG_JUMP_LAND && action != ACT_LONG_JUMP_LAND_STOP
               && action != ACT_CROUCH_SLIDE && action != ACT_START_CROUCHING
               && action != ACT_CROUCHING) {
        m->backwardLongJumpChain = FALSE;
    }
    m->backwardLongJumpCrouchFrames = action == ACT_CROUCHING && m->backwardLongJumpChain
                                          ? MARIO_BACKWARD_LONG_JUMP_CROUCH_FRAMES
                                          : 0;
    // Bonk recovery explicitly carries this across its own air/ground transitions.
    m->wallBonkMomentum = FALSE;
    // Only a successful, uninterrupted double-jump wall kick upgrades the next jump.
    switch (action) {
        case ACT_AIR_HIT_WALL:
            m->wallKickFromDoubleJump =
                m->action == ACT_DOUBLE_JUMP
                || (m->action == ACT_WALL_KICK_AIR && m->wallKickFromDoubleJump);
            break;
        case ACT_WALL_KICK_AIR:
            m->wallKickFromDoubleJump = m->action == ACT_AIR_HIT_WALL && m->wallKickFromDoubleJump;
            break;
        case ACT_JUMP_LAND:
            m->wallKickFromDoubleJump = m->action == ACT_WALL_KICK_AIR && m->wallKickFromDoubleJump;
            break;
        case ACT_JUMP_LAND_STOP:
        case ACT_WALKING:
        case ACT_DECELERATING:
            if (m->doubleJumpTimer == 0
                || (m->action != ACT_JUMP_LAND && m->action != ACT_JUMP_LAND_STOP
                    && m->action != ACT_WALKING && m->action != ACT_DECELERATING)) {
                m->wallKickFromDoubleJump = FALSE;
            }
            break;
        default:
            m->wallKickFromDoubleJump = FALSE;
            break;
    }

    // Only a continuous slide/jump/landing chain counts as another attempt.
    switch (action) {
        case ACT_BUTT_SLIDE:
        case ACT_HOLD_BUTT_SLIDE:
        case ACT_BUTT_SLIDE_AIR:
        case ACT_HOLD_BUTT_SLIDE_AIR:
        case ACT_JUMP:
        case ACT_HOLD_JUMP:
        case ACT_STEEP_JUMP:
        case ACT_JUMP_KICK:
        case ACT_FREEFALL:
        case ACT_HOLD_FREEFALL:
        case ACT_JUMP_LAND:
        case ACT_HOLD_JUMP_LAND:
        case ACT_FREEFALL_LAND:
        case ACT_HOLD_FREEFALL_LAND:
            if (!(action & ACT_FLAG_AIR) && (m->floor == NULL || !mario_floor_is_slope(m))) {
                m->backwardSlopeSlideCount = 0;
            }
            break;
        default:
            m->backwardSlopeSlideCount = 0;
            m->backwardButtSlideJumpCount = 0;
            break;
    }

    // Keep the contact sample across precisely the jump-kick landing handoff.
    // Slide setup above consumes it; any other exit discards it.
    if (m->action != ACT_JUMP_KICK || action != ACT_FREEFALL_LAND) {
        m->slopeJumpKickImpactSpeed = 0.0f;
    }

    if (!(action & ACT_FLAG_AIR)) {
        m->wallKickChainCount = 0;
        m->wallKickContactSoundPlayed = FALSE;
    }
    if (!(action & ACT_FLAG_AIR) || action == ACT_GROUND_POUND) {
        m->windCatchActive = FALSE;
    }
    mario_air_twirl_transition(m, action);
    if (m->action == ACT_GROUND_TWIRL && action != ACT_GROUND_TWIRL) {
        mario_ground_twirl_reset_wall_bounces();
    }
    if (m->action == ACT_SLIDE_KICK && action != ACT_SLIDE_KICK) {
        m->marioObj->header.gfx.angle[0] = 0;
    }
    if (m->action == ACT_PUNCHING || m->action == ACT_MOVE_PUNCHING || action == ACT_PUNCHING
        || action == ACT_MOVE_PUNCHING) {
        // Action changes end/start a sequence; animation follow-ups keep its mode.
        mario_reset_punch_sequence();
    }
    if (m->bullyKnockbackActive && !mario_is_knocked_back(action)) {
        // Recovery can finish while the Bully is still touching Mario. Give
        // him two interaction updates to move away before another knockback.
        m->bullyKnockbackImmunityFrames = 2;
        m->bullyKnockbackActive = FALSE;
    }
    if (!mario_is_knockback_recovery(m->action) || !mario_is_knockback_recovery(action)) {
        m->bullyKnockbackBuffer = 0;
    }
    if ((m->action == ACT_THROWN_FORWARD || m->action == ACT_THROWN_BACKWARD)
        && action != ACT_THROWN_FORWARD && action != ACT_THROWN_BACKWARD) {
        m->heaveHoThrowPendingLanding = FALSE;
    }
    if (m->action != action) {
        m->objectRecoveryBFrames = 0;
        m->objectRecoveryAFrames = 0;
    }
    s32 airTwirlLanding = m->action == ACT_L_AIR_TWIRL && action == ACT_FREEFALL_LAND;
    s32 leavingAirTwirlLanding =
        m->action == ACT_FREEFALL_LAND && m->prevAction == ACT_L_AIR_TWIRL && action != m->action;
    if ((!airTwirlLanding && mario_move_buffer_family(m->action) != mario_move_buffer_family(action))
        || leavingAirTwirlLanding) {
        m->automaticZFrames = 0;
        m->automaticBFrames = 0;
        m->automaticAFrames = 0;
        m->objectRecoveryBFrames = 0;
        m->objectRecoveryAFrames = 0;
    }
    if (!(action & ACT_FLAG_AIR)) {
        m->airTwirlInputFrames = 0;
    }
    if ((action & ACT_GROUP_MASK) != ACT_GROUP_AIRBORNE || action == ACT_GROUND_POUND
        || (action & ACT_FLAG_INVULNERABLE)) {
        m->groundPoundInputFrames = 0;
    }
    m->crouchSlideNeutral = FALSE;
    m->crouchSlideFrames = 0;
    m->crouchSlideBackflip = FALSE;
    m->rolloutWallKickFrames = 0;
    m->rolloutFrames = 0;
    m->waterWalkTwirl = action == ACT_GROUND_TWIRL && m->floor == &gWaterSurfacePseudoFloor;
}

u32 set_mario_action(struct MarioState *m, u32 action, u32 actionArg) {
    if (!game_options_get(GO_TWIRLS)) {
        if (action == ACT_TWIRLING) {
            action = ACT_FREEFALL;
        } else if (action == ACT_TWIRL_LAND) {
            action = ACT_FREEFALL_LAND;
        }
    }
    if (m->landingJumpBufferedEdge && (action & ACT_FLAG_AIR)) {
        m->input &= ~INPUT_A_PRESSED;
        m->landingJumpBufferedEdge = FALSE;
    } else if (!mario_is_buffered_landing(action)) {
        m->landingJumpBufferedEdge = FALSE;
    }
    prepare_ground_takeoff(m, action);
    if (m->landingJumpInputFrames > 0 && mario_is_buffered_landing(action)
        && (m->action & ACT_GROUP_MASK) == ACT_GROUP_AIRBORNE) {
        m->input |= INPUT_A_PRESSED;
        m->landingJumpBufferedEdge = TRUE;
    }
    m->landingJumpInputFrames = 0;
    m->wallKickInputFrames = 0;
    if (action != ACT_WALKING) {
        m->burnCoinJumpPending = FALSE;
    }
    if (action == ACT_GROUND_POUND && mario_has_current_vanish_cap(m) && mario_is_head_underwater(m)
        && m->pos[1] < m->waterLevel - 100.0f) {
        m->particleFlags |= PARTICLE_VANISH_WATER_RING;
    }
    if (action == ACT_WAITING_FOR_DIALOG || action == ACT_READING_SIGN
        || action == ACT_READING_NPC_DIALOG) {
        mario_set_forward_vel(m, 0.0f);
        m->slideVelZ = 0.0f;
        m->slideVelX = 0.0f;
    }
    switch (action & ACT_GROUP_MASK) {
        case ACT_GROUP_MOVING:
            action = set_mario_action_moving(m, action, actionArg);
            break;
        case ACT_GROUP_AIRBORNE:
            action = set_mario_action_airborne(m, action, actionArg);
            break;
        case ACT_GROUP_SUBMERGED:
            action = set_mario_action_submerged(m, action, actionArg);
            break;
        case ACT_GROUP_CUTSCENE:
            action = set_mario_action_cutscene(m, action, actionArg);
            break;
    }

    reset_mario_action_state(m, action);

    // Resets the sound played flags, meaning Mario can play those sound types again.
    m->flags &= ~(MARIO_ACTION_SOUND_PLAYED | MARIO_MARIO_SOUND_PLAYED);

    if (!(m->action & ACT_FLAG_AIR)) {
        m->flags &= ~MARIO_FALL_SOUND_PLAYED;
    }

    player_stats_action(m, action, actionArg);

    // Initialize the action information.
    m->prevAction = m->action;
    m->action = action;
    m->actionArg = actionArg;
    m->actionState = 0;
    m->actionTimer = 0;

    return TRUE;
}

/**
 * Puts Mario into a specific jumping action from a landing action.
 */
s32 set_jump_from_landing(struct MarioState *m) {
    if (m->quicksandDepth >= 11.0f) {
        if (m->heldObj == NULL) {
            return set_mario_action(m, ACT_QUICKSAND_JUMP_LAND, 0);
        } else {
            return set_mario_action(m, ACT_HOLD_QUICKSAND_JUMP_LAND, 0);
        }
    }

    if (mario_floor_is_steep(m)) {
        set_steep_jump_action(m);
    } else {
        if ((m->doubleJumpTimer == 0) || (m->squishTimer != 0)) {
            set_mario_action(m, ACT_JUMP, 0);
        } else {
            u32 landingAction = m->wallKickFromDoubleJump ? ACT_DOUBLE_JUMP_LAND : m->prevAction;
            switch (landingAction) {
                case ACT_JUMP_LAND:
                    set_mario_action(m, ACT_DOUBLE_JUMP, 0);
                    break;

                case ACT_FREEFALL_LAND:
                    set_mario_action(m, ACT_DOUBLE_JUMP, 0);
                    break;

                case ACT_SIDE_FLIP_LAND_STOP:
                    set_mario_action(m, ACT_DOUBLE_JUMP, 0);
                    break;

                case ACT_DOUBLE_JUMP_LAND:
                    // If Mario has a wing cap, he ignores the typical speed
                    // requirement for a triple jump.
                    if (m->flags & MARIO_WING_CAP) {
                        set_mario_action(m, ACT_FLYING_TRIPLE_JUMP, 0);
                    } else if (absf(m->forwardVel) > TRIPLE_JUMP_SPEED_THRESHOLD) {
                        set_mario_action(m, ACT_TRIPLE_JUMP,
                                         m->forwardVel < 0.0f ? ACT_ARG_TRIPLE_JUMP_BACKWARD
                                                              : ACT_ARG_TRIPLE_JUMP_FORWARD);
                    } else {
                        set_mario_action(m, ACT_JUMP, 0);
                    }
                    break;

                default:
                    set_mario_action(m, ACT_JUMP, 0);
                    break;
            }
        }
    }

    m->doubleJumpTimer = 0;

    return TRUE;
}

/**
 * Puts Mario in a given action, as long as it is not overruled by
 * either a quicksand or steep jump.
 */
s32 set_jumping_action(struct MarioState *m, u32 action, u32 actionArg) {
    if (m->quicksandDepth >= 11.0f) {
        // Checks whether Mario is holding an object or not.
        if (m->heldObj == NULL) {
            return set_mario_action(m, ACT_QUICKSAND_JUMP_LAND, 0);
        } else {
            return set_mario_action(m, ACT_HOLD_QUICKSAND_JUMP_LAND, 0);
        }
    }

    if (mario_floor_is_steep(m)) {
        set_steep_jump_action(m);
        if ((action == ACT_JUMP || action == ACT_HOLD_JUMP)
            && (actionArg & ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL)) {
            m->actionArg = ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL;
        }
    } else {
        set_mario_action(m, action, actionArg);
    }

    return TRUE;
}

/**
 * Drop anything Mario is holding and set a new action.
 */
s32 drop_and_set_mario_action(struct MarioState *m, u32 action, u32 actionArg) {
    mario_stop_riding_and_holding(m);

    return set_mario_action(m, action, actionArg);
}

/**
 * Increment Mario's hurt counter and set a new action.
 */
s32 hurt_and_set_mario_action(struct MarioState *m, u32 action, u32 actionArg, s16 hurtCounter) {
    m->hurtCounter = hurtCounter;

    return set_mario_action(m, action, actionArg);
}

/**
 * Checks a variety of inputs for common transitions between many different
 * actions. A common variant of the below function.
 */
s32 check_common_action_exits(struct MarioState *m) {
    if (m->input & INPUT_A_PRESSED) {
        return set_mario_action(m, ACT_JUMP, 0);
    }
    if (m->input & INPUT_OFF_FLOOR) {
        return set_mario_action(m, ACT_FREEFALL, 0);
    }
    if (m->input & INPUT_NONZERO_ANALOG) {
        return set_mario_action(m, ACT_WALKING, 0);
    }
    if (m->input & INPUT_ABOVE_SLIDE) {
        return set_mario_action(m, ACT_BEGIN_SLIDING, 0);
    }

    return FALSE;
}

/**
 * Checks a variety of inputs for common transitions between many different
 * object holding actions. A holding variant of the above function.
 */
s32 check_common_hold_action_exits(struct MarioState *m) {
    if (m->input & INPUT_A_PRESSED     ) return set_mario_action(m, ACT_HOLD_JUMP,          0);
    if (m->input & INPUT_OFF_FLOOR     ) return set_mario_action(m, ACT_HOLD_FREEFALL,      0);
    if (m->input & INPUT_NONZERO_ANALOG) return set_mario_action(m, ACT_HOLD_WALKING,       0);
    if (m->input & INPUT_ABOVE_SLIDE   ) return set_mario_action(m, ACT_HOLD_BEGIN_SLIDING, 0);
    return FALSE;
}

/**
 * Transitions Mario from a submerged action to a walking action.
 */
u32 landing_jump_action(struct MarioState *m, u32 ordinaryAction) {
    if (m->action == ACT_LAVA_BOOST_LAND) {
        return ordinaryAction;
    }
    if (m->heldObj == NULL && (m->input & INPUT_Z_DOWN)) {
        // A backward long jump already landed: any held stick continues its
        // chain. Neutral stick still selects the ordinary backflip.
        if (ordinaryAction == ACT_LONG_JUMP && m->forwardVel < 0.0f && m->controller->stickMag > 0.0f) {
            return ACT_LONG_JUMP;
        }
        return (m->controller->stickMag >= MARIO_STRONG_STICK_THRESHOLD) ? ACT_LONG_JUMP : ACT_BACKFLIP;
    }
    return ordinaryAction == ACT_LONG_JUMP || ordinaryAction == ACT_BACKFLIP ? ACT_JUMP
                                                                             : ordinaryAction;
}

s32 check_landing_attack_or_crouch(struct MarioState *m) {
    if (m->action == ACT_LAVA_BOOST_LAND) {
        return FALSE;
    }
    if ((m->input & INPUT_B_PRESSED) || m->zBPressed) {
        if (m->heldObj != NULL) {
            return set_mario_action(m, ACT_THROWING, 0);
        }
        if (m->input & INPUT_Z_DOWN) {
            // Restore B if simultaneous Z masked it, so the crouch handler
            // receives the remembered attack on this update's redispatch.
            m->input |= INPUT_B_PRESSED;
            return set_mario_action(m, absf(m->forwardVel) >= 1.0f ? ACT_CROUCH_SLIDE : ACT_CROUCHING,
                                    0);
        }
        return set_mario_action(m,
                                absf(m->forwardVel) >= 1.0f || (m->input & INPUT_NONZERO_ANALOG)
                                    ? ACT_MOVE_PUNCHING
                                    : ACT_PUNCHING,
                                0);
    }
    if (m->input & INPUT_Z_DOWN) {
        m->zCrouchBuffered = FALSE;
        s32 neutralSlideStop =
            !(m->input & INPUT_NONZERO_ANALOG)
            && (m->action == ACT_BUTT_SLIDE_STOP || m->action == ACT_SLIDE_KICK_SLIDE_STOP
                || m->action == ACT_STOMACH_SLIDE_STOP);
        // Residual slide momentum can coast through crouching without locking A.
        return drop_and_set_mario_action(
            m,
            !neutralSlideStop && absf(m->forwardVel) >= 1.0f ? ACT_CROUCH_SLIDE : ACT_START_CROUCHING,
            0);
    }
    return FALSE;
}

void mario_stop_underwater_shell(struct MarioState *m) {
    if (m->action == ACT_WATER_SHELL_SWIMMING && m->heldObj != NULL) {
        m->heldObj->oInteractStatus = INT_STATUS_STOP_RIDING;
        m->heldObj = NULL;
        stop_shell_music();
    }
}

s32 transition_submerged_to_walking(struct MarioState *m) {
    mario_stop_underwater_shell(m);
    f32 horizontalSpeed = sqrtf(sqr(m->vel[0]) + sqr(m->vel[2]));
    s32 backwards = m->vel[0] * sins(m->faceAngle[1]) + m->vel[2] * coss(m->faceAngle[1]) < 0.0f;
    u32 action;
    set_camera_mode(m->area->camera, m->area->camera->defMode, 1);

    vec3_zero(m->angleVel);
    m->faceAngle[2] = 0;
    m->faceAngle[0] = 0;
    m->marioObj->header.gfx.angle[2] = 0;
    m->marioObj->header.gfx.angle[0] = 0;
    if (horizontalSpeed > 0.0f) {
        m->faceAngle[1] = atan2s(m->vel[2], m->vel[0]) + (backwards ? 0x8000 : 0);
    }
    if (m->input & INPUT_NONZERO_ANALOG) {
        action = m->heldObj == NULL ? ACT_WALKING : ACT_HOLD_WALKING;
    } else {
        action = m->heldObj == NULL ? ACT_DECELERATING : ACT_HOLD_DECELERATING;
    }
    set_mario_action(m, action, 0);
    mario_set_forward_vel(m, backwards ? -horizontalSpeed : horizontalSpeed);
    m->slideVelX = m->vel[0];
    m->slideVelZ = m->vel[2];
    return TRUE;
}

/**
 * Transitions Mario from a submerged action to an airborne action.
 * You may want to change these actions to fit your hack
 */
s32 transition_submerged_to_airborne(struct MarioState *m) {
    mario_stop_underwater_shell(m);
    set_camera_mode(m->area->camera, m->area->camera->defMode, 1);

    vec3_zero(m->angleVel);
    m->faceAngle[2] = 0;
    m->faceAngle[0] = 0;
    m->marioObj->header.gfx.angle[2] = 0;
    m->marioObj->header.gfx.angle[0] = 0;

    if (m->heldObj == NULL) {
        if ((m->input & INPUT_A_DOWN) && mario_has_dive_input(m)) {
            return set_mario_action(m, ACT_DIVE, 0);
        }
        return set_mario_action(m, ACT_FREEFALL, 0);
    } else {
        if (m->input & INPUT_A_DOWN) return set_mario_action(m, ACT_HOLD_JUMP, 0);
        else return set_mario_action(m, ACT_HOLD_FREEFALL, 0);
    }
}

s32 mario_has_current_metal_cap(struct MarioState *m) {
    return (m->flags & MARIO_METAL_CAP) && game_options_get(GO_METAL_CAP) == 0;
}

s32 mario_has_current_vanish_cap(struct MarioState *m) {
    return (m->flags & MARIO_VANISH_CAP) && game_options_get(GO_VANISH_CAP) == 0;
}

s32 mario_has_water_damage_protection(struct MarioState *m) {
    // Original Metal retains native underwater immunity; Current Metal can drown.
    return mario_has_current_vanish_cap(m)
           || ((m->flags & MARIO_METAL_CAP) && game_options_get(GO_METAL_CAP) == 1);
}

s32 mario_has_land_movement_underwater(struct MarioState *m) {
    // Either Current cap grants land controls; Original Vanish swims, Metal sinks.
    return mario_has_current_metal_cap(m) || mario_has_current_vanish_cap(m);
}

s32 mario_keeps_shell(struct MarioState *m) {
    return game_options_cheat_active(GO_KEEP_SHELL)
           && (((m->action & ACT_FLAG_RIDING_SHELL) && m->riddenObj != NULL
                && (m->riddenObj->behavior == segmented_to_virtual(bhvKoopaShell)
                    || m->riddenObj->behavior == segmented_to_virtual(bhvKoopaShellUnderwater)))
               || (m->action == ACT_WATER_SHELL_SWIMMING && m->heldObj != NULL
                   && m->heldObj->behavior == segmented_to_virtual(bhvKoopaShellUnderwater)));
}

s32 mario_blocks_harmful_knockback(struct MarioState *m) {
    return game_options_cheat_active(GO_KNOCKBACK) || mario_keeps_shell(m);
}

s32 mario_is_riding_underwater_shell(struct MarioState *m) {
    return (m->action & ACT_FLAG_RIDING_SHELL) && m->riddenObj != NULL
           && m->riddenObj->behavior == segmented_to_virtual(bhvKoopaShellUnderwater);
}

s32 mario_transition_underwater_shell_to_riding(struct MarioState *m) {
    struct Object *shell;
    u32 action;

    if (m->action != ACT_WATER_SHELL_SWIMMING || m->heldObj == NULL
        || m->heldObj->behavior != segmented_to_virtual(bhvKoopaShellUnderwater)
        || !mario_has_land_movement_underwater(m) || m->health < 0x100) {
        return FALSE;
    }

    shell = m->heldObj;
    m->heldObj = NULL;
    m->riddenObj = shell;
    m->usedObj = shell;
    shell->oHeldState = HELD_FREE;
    shell->oAction = 1;
    shell->oInteractStatus = INT_STATUS_NONE;
    shell->oIntangibleTimer = -1;
    shell->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    set_camera_mode(m->area->camera, m->area->camera->defMode, 1);
    m->faceAngle[0] = 0;
    m->faceAngle[2] = 0;
    m->marioObj->header.gfx.angle[2] = 0;
    m->marioObj->header.gfx.angle[0] = 0;
    vec3_zero(m->angleVel);
    mario_set_forward_vel(m, sqrtf(sqr(m->vel[0]) + sqr(m->vel[2])));
    action = m->pos[1] <= m->floorHeight + 4.0f && m->vel[1] <= 0.0f ? ACT_RIDING_SHELL_GROUND
                                                                     : ACT_RIDING_SHELL_FALL;
    return set_mario_action(m, action, 1);
}

s32 mario_transition_underwater_shell_to_swimming(struct MarioState *m) {
    struct Object *shell;
    struct Surface *floor;
    f32 waterHeight = m->waterLevel - 80.0f;

    if (!mario_is_riding_underwater_shell(m) || mario_has_land_movement_underwater(m)
        || m->waterLevel == FLOOR_LOWER_LIMIT || m->pos[1] > m->waterLevel || m->health < 0x100) {
        return FALSE;
    }

    // A shell rider's cached floor may be the water-surface pseudo floor.
    // Check the actual floor before entering the swimming action.
    f32 floorHeight = find_floor(m->pos[0], m->pos[1], m->pos[2], &floor);
    if (floor != NULL && floorHeight >= waterHeight) {
        return FALSE;
    }

    shell = m->riddenObj;
    m->riddenObj = NULL;
    m->heldObj = shell;
    m->usedObj = shell;
    shell->oInteractStatus = INT_STATUS_NONE;
    obj_set_held_state(shell, bhvCarrySomethingHeld);
    m->marioBodyState->grabPos = GRAB_POS_LIGHT_OBJ;
    m->faceAngle[0] = 0;
    m->faceAngle[2] = 0;
    m->vel[1] = 0.0f;
    m->pos[1] = MIN(m->pos[1], waterHeight);
    return set_mario_action(m, ACT_WATER_SHELL_SWIMMING, 0);
}

s32 mario_is_low_land_pose(struct MarioState *m) {
    if ((m->action & ACT_FLAG_AIR) || (m->action & ACT_GROUP_MASK) == ACT_GROUP_SUBMERGED) {
        return FALSE;
    }
    if (m->action & ACT_FLAG_SHORT_HITBOX) {
        return TRUE;
    }
    return m->action == ACT_DAB || m->action == ACT_STOMACH_SLIDE || m->action == ACT_HOLD_STOMACH_SLIDE
           || m->action == ACT_DIVE_SLIDE || m->action == ACT_SLIDE_KICK_SLIDE
           || m->action == ACT_STOMACH_SLIDE_STOP || m->action == ACT_SLIDE_KICK_SLIDE_STOP
           || ((m->action == ACT_PUNCHING || m->action == ACT_MOVE_PUNCHING)
               && m->actionArg == ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
}

s32 mario_is_head_underwater(struct MarioState *m) {
    return m->pos[1] + (mario_is_low_land_pose(m) ? 50.0f : 140.0f) < m->waterLevel;
}

#define MARIO_SOAKED_TAIL_FRAMES (5 * 30)

s32 mario_has_water_contact(struct MarioState *m, f32 waterLevel) {
    return waterLevel > FLOOR_LOWER_LIMIT && m->pos[1] <= waterLevel;
}

// Overlay infinite powers without replacing the collected cap timer or ownership.
void mario_update_forced_caps(struct MarioState *m) {
    u32 forced = 0;
    if (game_options_cheat_active(GO_FORCE_WING_CAP)) {
        forced |= MARIO_WING_CAP;
    }
    if (game_options_cheat_active(GO_FORCE_METAL_CAP)) {
        forced |= MARIO_METAL_CAP;
    }
    if (game_options_cheat_active(GO_FORCE_VANISH_CAP)) {
        forced |= MARIO_VANISH_CAP;
    }
    m->flags = (m->flags & ~m->forcedCapFlags) | m->naturalCapFlags;
    m->naturalCapFlags = m->flags & MARIO_SPECIAL_CAPS;
    m->forcedCapFlags = forced;
    m->flags |= forced;
    if (forced && !(m->flags & MARIO_CAP_IN_HAND)) {
        m->flags |= MARIO_CAP_ON_HEAD;
    } else if (!(m->flags & MARIO_CAPS)) {
        m->flags &= ~MARIO_CAP_ON_HEAD;
    }
}

void mario_add_cap_power(struct MarioState *m, u32 flags) {
    m->naturalCapFlags |= flags & MARIO_SPECIAL_CAPS;
    m->flags |= flags;
}

u32 mario_natural_cap_powers(struct MarioState *m) {
    return (m->flags & MARIO_SPECIAL_CAPS & ~m->forcedCapFlags) | m->naturalCapFlags;
}

void mario_clear_cap_flags(struct MarioState *m) {
    m->naturalCapFlags = 0;
    m->flags = (m->flags & ~MARIO_SPECIAL_CAPS) | m->forcedCapFlags;
}

s32 mario_is_soaked(struct MarioState *m) {
    return game_options_cheat_active(GO_PERMANENT_SOAKED)
           || (game_options_get(GO_SOAKED) && !mario_has_current_vanish_cap(m) && m->soakedThisUpdate);
}

void mario_apply_soaked_status(struct MarioState *m) {
    if (!game_options_get(GO_SOAKED) || mario_has_current_vanish_cap(m)) {
        return;
    }

    m->soakedTimer = MARIO_SOAKED_TAIL_FRAMES;
    m->soakedThisUpdate = TRUE;
    m->soakedDripFrames = 0;
}

void mario_consume_soaked_status(struct MarioState *m) {
    m->soakedTimer = 0;
    m->soakedThisUpdate = FALSE;
    if (!game_options_cheat_active(GO_PERMANENT_SOAKED)) {
        m->soakedDripFrames = 0;
    }
}

static void mario_dry_from_wind(struct MarioState *m) {
    if (game_options_cheat_active(GO_IGNORE_WIND)) {
        return;
    }
    s32 windContact = (m->collidedObjInteractTypes & (INTERACT_TORNADO | INTERACT_STRONG_WIND))
                      || m->action == ACT_TORNADO_TWIRLING || m->action == ACT_GETTING_BLOWN;
    if (m->floor != NULL) {
        windContact |= (m->floor->type == SURFACE_HORIZONTAL_WIND && m->pos[1] <= m->floorHeight)
                       || (m->floor->type == SURFACE_VERTICAL_WIND
                           && m->pos[1] >= m->floorHeight + 2048.0f && m->pos[1] < 500.0f);
    }
    // Exposure dries Mario even when a cap or action blocks the wind's force.
    if (windContact) {
        mario_consume_soaked_status(m);
    }
}

static void mario_extinguish_soaked_burning(struct MarioState *m) {
    if (mario_is_soaked(m)
        && (m->action == ACT_BURNING_GROUND || m->action == ACT_BURNING_JUMP
            || m->action == ACT_BURNING_FALL)) {
        m->soakedFlameProtection = (m->collidedObjInteractTypes & INTERACT_FLAME) != 0;
        mario_consume_soaked_status(m);
        mario_extinguish_fire(m);
    }
}

void mario_update_soaked_status(struct MarioState *m) {
    s32 waterContact = mario_has_water_contact(m, m->waterLevel);

    if (!game_options_get(GO_SOAKED) && !game_options_cheat_active(GO_PERMANENT_SOAKED)) {
        mario_consume_soaked_status(m);
        m->soakedWaterContact = FALSE;
        m->soakedFlameProtection = FALSE;
        return;
    }

    // Flamethrowers reset their interaction status every update. Retain the
    // consumed status's protection until Mario actually leaves the flames.
    if (!(m->collidedObjInteractTypes & INTERACT_FLAME) || mario_has_current_vanish_cap(m)) {
        m->soakedFlameProtection = FALSE;
    }

    if (mario_has_current_vanish_cap(m)) {
        mario_consume_soaked_status(m);
        m->soakedWaterContact = FALSE;
    } else if (waterContact) {
        if (!m->soakedWaterContact) {
            m->soakedTimer = MARIO_SOAKED_TAIL_FRAMES;
            m->soakedDripFrames = 0;
        } else if (m->soakedTimer != 0) {
            m->soakedTimer = MARIO_SOAKED_TAIL_FRAMES;
        }
        m->soakedThisUpdate = m->soakedTimer != 0;
        m->soakedWaterContact = TRUE;
    } else {
        m->soakedWaterContact = FALSE;
        m->soakedThisUpdate = m->soakedTimer != 0;
        if (m->soakedTimer != 0) {
            m->soakedTimer--;
        }
    }
    mario_dry_from_wind(m);
    mario_extinguish_soaked_burning(m);
}

static void mario_refresh_soaked_after_movement(struct MarioState *m) {
    f32 waterLevel = find_water_level(m->pos[0], m->pos[2]);
    s32 waterContact = mario_has_water_contact(m, waterLevel);
    mario_update_water_walking_state(m, waterLevel, m->ceilHeight);

    if (!game_options_get(GO_SOAKED) && !game_options_cheat_active(GO_PERMANENT_SOAKED)) {
        mario_consume_soaked_status(m);
        m->soakedWaterContact = FALSE;
        m->soakedFlameProtection = FALSE;
        return;
    }

    if (mario_has_current_vanish_cap(m)) {
        m->soakedFlameProtection = FALSE;
        mario_consume_soaked_status(m);
        m->soakedWaterContact = FALSE;
    } else {
        if (waterContact && !m->soakedWaterContact) {
            m->soakedTimer = MARIO_SOAKED_TAIL_FRAMES;
            m->soakedThisUpdate = TRUE;
            m->soakedDripFrames = 0;
        }
        m->soakedWaterContact = waterContact;
    }
    mario_dry_from_wind(m);
    mario_extinguish_soaked_burning(m);
}

static void mario_emit_soaked_drips(struct MarioState *m) {
    struct Object *drop;
    f32 scale;
    f32 sideways;
    s16 cameraYaw;

    if (!mario_is_soaked(m) || m->soakedWaterContact || (m->action & ACT_FLAG_INTANGIBLE)
        || m->health < 0x100) {
        m->soakedDripFrames = 0;
        return;
    }
    if (++m->soakedDripFrames != 5 && m->soakedDripFrames != 10) {
        return;
    }
    scale = m->soakedDripFrames == 5 ? 1.2f : 2.4f;
    if (m->soakedDripFrames == 10) {
        m->soakedDripFrames = 0;
    }
    drop = spawn_object_with_scale(m->marioObj, MODEL_WHITE_PARTICLE_SMALL, bhvWaterDroplet, scale);
    if (drop == NULL) {
        return;
    }
    // Spawn beside Mario in screen space so his body does not hide the drip.
    sideways = ((random_u16() & 1) ? -60.0f : 60.0f) + random_f32_around_zero(20.0f);
    cameraYaw = m->area->camera->yaw;
    // Dry drip: carry some horizontal motion, then fall under gravity.
    drop->oAction = WATER_DROPLET_ACT_SOAKED;
    drop->oPosX = m->pos[0] + sideways * coss(cameraYaw);
    drop->oPosY = m->pos[1] + 80.0f;
    drop->oPosZ = m->pos[2] - sideways * sins(cameraYaw);
    // Remember Mario's last position so later movement only partly carries the drip.
    drop->oHomeX = m->pos[0];
    drop->oHomeZ = m->pos[2];
    drop->oForwardVel = 0.0f;
    drop->oVelY = 0.0f;
}

static s32 mario_should_drown_on_land(struct MarioState *m) {
    return !mario_has_water_damage_protection(m)
           && ((mario_has_land_movement_underwater(m) && m->pos[1] < m->waterLevel - 100)
               || (mario_is_low_land_pose(m) && mario_is_head_underwater(m)))
           && m->health < 0x100 && !(m->action & (ACT_FLAG_INTANGIBLE | ACT_FLAG_INVULNERABLE))
           && (m->action & ACT_GROUP_MASK) != ACT_GROUP_SUBMERGED
           && (m->action & ACT_GROUP_MASK) != ACT_GROUP_CUTSCENE;
}

static s32 mario_is_shallow_water_drowning(struct MarioState *m) {
    return mario_is_low_land_pose(m) && mario_is_head_underwater(m) && m->pos[1] >= m->waterLevel - 100;
}

s32 mario_action_locks_camera(u32 action) {
    return action == ACT_WAITING_FOR_DIALOG || action == ACT_READING_SIGN
           || action == ACT_READING_NPC_DIALOG || action == ACT_READING_AUTOMATIC_DIALOG;
}

/**
 * This is the transition function typically for entering a submerged action for a
 * non-submerged action. This also applies the water surface camera preset.
 */
s32 set_water_plunge_action(struct MarioState *m) {
    // Apply the one-time entry loss separately from ongoing plunge drag.
    m->forwardVel /= WATER_ENTRY_FORWARD_DIVISOR;
    m->vel[1] /= WATER_ENTRY_VERTICAL_DIVISOR;

#ifdef WATER_PLUNGE_UPWARP
    m->pos[1] = m->waterLevel - 100;
#endif

    m->faceAngle[2] = 0;

    vec3_zero(m->angleVel);

    if (!(m->action & ACT_FLAG_DIVING)) {
        m->faceAngle[0] = 0;
    }

    if (m->area->camera->mode != WATER_SURFACE_CAMERA_MODE) {
        set_camera_mode(m->area->camera, WATER_SURFACE_CAMERA_MODE, 1);
    }

    if (m->action == ACT_GROUND_POUND && !(m->flags & MARIO_METAL_CAP)) {
        return set_mario_action(m, ACT_WATER_DOWN_BURST, ACT_ARG_WATER_DOWN_BURST_FROM_AIR);
    }

    return set_mario_action(m, ACT_WATER_PLUNGE, 0);
}

/**
 * These are the scaling values for the x and z axis for Mario
 * when he is close to unsquishing.
 */
u8 sSquishScaleOverTime[16] = { 0x46, 0x32, 0x32, 0x3C, 0x46, 0x50, 0x50, 0x3C,
                                0x28, 0x14, 0x14, 0x1E, 0x32, 0x3C, 0x3C, 0x28 };

/**
 * Applies the squish to Mario's model via scaling.
 */
void squish_mario_model(struct MarioState *m) {
    u8 step;

    if (m->squishTimer == 0 || m->squishTimer == 0xFF || m->action == ACT_SQUISHED) {
        m->fastMetalFallSquish = FALSE;
    }
    step = m->fastMetalFallSquish ? 2 : 1;
    if (m->squishTimer != 0xFF) {
        // If no longer squished, scale back to default.
        if (m->squishTimer == 0) {
            vec3f_set(m->marioObj->header.gfx.scale, 1.0f, 1.0f, 1.0f);
        }
        // If timer is less than 16, rubber-band Mario's size scale up and down.
        else if (m->squishTimer <= 16) {
            m->squishTimer = m->squishTimer > step ? m->squishTimer - step : 0;

            m->marioObj->header.gfx.scale[1] =
                1.0f - ((sSquishScaleOverTime[15 - m->squishTimer] * 0.6f) / 100.0f);
            m->marioObj->header.gfx.scale[0] =
                ((sSquishScaleOverTime[15 - m->squishTimer] * 0.4f) / 100.0f) + 1.0f;

            m->marioObj->header.gfx.scale[2] = m->marioObj->header.gfx.scale[0];
        } else {
            m->squishTimer = m->squishTimer > step ? m->squishTimer - step : 0;

            vec3f_set(m->marioObj->header.gfx.scale, 1.4f, 0.4f, 1.4f);
        }
    }
}

#ifdef VANILLA_DEBUG
/**
 * Debug function that prints floor normal, velocity, and action information.
 */
void debug_print_speed_action_normal(struct MarioState *m) {
    f32 steepness;
    f32 floor_nY;

    if (gShowDebugText) {
        steepness = sqrtf(sqr(m->floor->normal.x) + sqr(m->floor->normal.z));
        floor_nY = m->floor->normal.y;

        print_text_fmt_int(210, 88, "ANG %d", (atan2s(floor_nY, steepness) * 180.0f) / 32768.0f);

        print_text_fmt_int(210, 72, "SPD %d", m->forwardVel);

        // STA short for "status," the official action name via SMS map.
        print_text_fmt_int(210, 56, "STA %x", (m->action & ACT_ID_MASK));
    }
}
#endif

/**
 * Update the button inputs for Mario.
 */
s32 mario_move_buffer_family(u32 action) {
    if (action & ACT_FLAG_ON_POLE) {
        return 1;
    }
    if (action & ACT_FLAG_HANGING) {
        return 2;
    }
    switch (action) {
        case ACT_LEDGE_GRAB:
        case ACT_LEDGE_CLIMB_SLOW_1:
        case ACT_LEDGE_CLIMB_SLOW_2:
        case ACT_LEDGE_CLIMB_DOWN:
        case ACT_LEDGE_CLIMB_FAST:
            return 3;
        case ACT_DIVE_PICKING_UP:
        case ACT_THROWING:
        case ACT_HEAVY_THROW:
            return 4;
        case ACT_L_AIR_TWIRL:
        case ACT_TWIRLING:
        case ACT_TWIRL_LAND:
            return 5;
        case ACT_RELEASING_BOWSER:
            return 6;
    }
    return 0;
}

s32 mario_is_buffered_landing(u32 action) {
    switch (action) {
        case ACT_JUMP_LAND:
        case ACT_DOUBLE_JUMP_LAND:
        case ACT_TRIPLE_JUMP_LAND:
        case ACT_SIDE_FLIP_LAND:
        case ACT_LONG_JUMP_LAND:
        case ACT_BACKFLIP_LAND:
        case ACT_FREEFALL_LAND:
        case ACT_HOLD_JUMP_LAND:
        case ACT_HOLD_FREEFALL_LAND:
        case ACT_QUICKSAND_JUMP_LAND:
        case ACT_HOLD_QUICKSAND_JUMP_LAND:
        case ACT_TWIRL_LAND:
        case ACT_GROUND_POUND_LAND:
        case ACT_FREEFALL_LAND_STOP:
        case ACT_HOLD_FREEFALL_LAND_STOP:
            return TRUE;
    }
    return FALSE;
}

// Sample once per gameplay update; action redispatch never ages or refreshes a request.
void mario_update_move_buffers(struct MarioState *m) {
    s32 family = mario_move_buffer_family(m->action);
    if (m->action == ACT_FREEFALL_LAND && m->prevAction == ACT_L_AIR_TWIRL) {
        family = 5;
    }
    if (m->objectRecoveryAFrames > 0) {
        m->objectRecoveryAFrames--;
    }
    if (m->objectRecoveryBFrames > 0) {
        m->objectRecoveryBFrames--;
    }
    if (m->automaticAFrames > 0) {
        m->automaticAFrames--;
    }
    if (m->automaticBFrames > 0) {
        m->automaticBFrames--;
    }
    if (m->automaticZFrames > 0) {
        m->automaticZFrames--;
    }
    if (m->groundPoundInputFrames > 0) {
        m->groundPoundInputFrames--;
    }
    if (family == 4) {
        if (m->input & INPUT_A_PRESSED) {
            m->objectRecoveryAFrames = 6;
        }
        if (m->input & INPUT_B_PRESSED) {
            m->objectRecoveryBFrames = 6;
        }
    } else {
        m->objectRecoveryBFrames = 0;
        m->objectRecoveryAFrames = 0;
    }
    if ((family >= 1 && family <= 3) || family == 5 || family == 6) {
        u8 bufferFrames = (family >= 5 || m->action == ACT_LEDGE_CLIMB_FAST) ? 6 : 3;
        if (m->input & INPUT_A_PRESSED) {
            m->automaticAFrames = bufferFrames;
        }
        if (m->input & INPUT_B_PRESSED) {
            m->automaticBFrames = bufferFrames;
        }
        if (m->input & INPUT_Z_PRESSED) {
            m->automaticZFrames = family >= 5 ? 6 : 3;
        }
    } else {
        m->automaticZFrames = 0;
        m->automaticBFrames = 0;
        m->automaticAFrames = 0;
    }
    if ((m->action & ACT_GROUP_MASK) == ACT_GROUP_AIRBORNE && !(m->action & ACT_FLAG_INVULNERABLE)) {
        if (m->squishTimer == 0 && (m->controller->buttonPressed & Z_TRIG)) {
            m->groundPoundInputFrames = 3;
        }
        if (m->groundPoundInputFrames > 0 && m->groundPoundBounceCooldown == 0 && m->squishTimer == 0) {
            m->input |= INPUT_Z_PRESSED;
        }
    } else {
        m->groundPoundInputFrames = 0;
    }
}

void update_mario_button_inputs(struct MarioState *m) {
    s32 groundPoundLocked = m->groundPoundBounceCooldown > 0;
    m->landingJumpBufferedEdge = FALSE;
    if (m->controller->buttonPressed & A_BUTTON) m->input |= INPUT_A_PRESSED;
    if (m->controller->buttonDown    & A_BUTTON) m->input |= INPUT_A_DOWN;

    // Don't update for these buttons if squished.
    if (m->squishTimer == 0) {
        if (m->controller->buttonDown    & B_BUTTON) m->input |= INPUT_B_DOWN;
        if (m->controller->buttonPressed & B_BUTTON) m->input |= INPUT_B_PRESSED;
        if (m->controller->buttonDown    & Z_TRIG  ) m->input |= INPUT_Z_DOWN;
        if (m->controller->buttonPressed & Z_TRIG  ) m->input |= INPUT_Z_PRESSED;
    }

    if (m->groundPoundBounceCooldown > 0) {
        if (m->action & ACT_FLAG_AIR) {
            m->input &= ~(INPUT_Z_DOWN | INPUT_Z_PRESSED);
        }
        m->groundPoundBounceCooldown--;
    }

    // Remember the slide-kick chord before Z removes a simultaneous B press.
    m->zBPressed = (m->input & (INPUT_Z_DOWN | INPUT_B_PRESSED)) == (INPUT_Z_DOWN | INPUT_B_PRESSED);
    // Z wins simultaneous presses by default.
    if ((m->input & (INPUT_B_PRESSED | INPUT_Z_PRESSED)) == (INPUT_B_PRESSED | INPUT_Z_PRESSED)) {
        m->input &= ~INPUT_B_PRESSED;
    }

    if (m->controller->stickMag == 0.0f) {
        m->backwardLongJumpChain = FALSE;
        m->backwardLongJumpCrouchFrames = 0;
    } else if (m->action == ACT_LONG_JUMP && m->forwardVel < 0.0f) {
        m->backwardLongJumpChain = TRUE;
    }
    if (m->backwardLongJumpCrouchFrames > 0) {
        m->backwardLongJumpCrouchFrames--;
    }
    if (m->landingJumpInputFrames > 0) {
        m->landingJumpInputFrames--;
    }
    if (m->wallKickInputFrames > 0) {
        m->wallKickInputFrames--;
    }
    if (m->burnCoinJumpPending > 0) {
        m->burnCoinJumpPending--;
    }
    if (m->input & INPUT_A_PRESSED) {
        if ((m->action & ACT_GROUP_MASK) == ACT_GROUP_AIRBORNE
            && !(m->action & ACT_FLAG_INVULNERABLE)) {
            m->landingJumpInputFrames = MARIO_LANDING_JUMP_BUFFER_FRAMES;
        }
        if ((m->action & ACT_FLAG_AIR) && m->action != ACT_AIR_HIT_WALL) {
            m->wallKickInputFrames = MARIO_WALL_KICK_INPUT_BUFFER_FRAMES;
        }
    }
    if (m->input & INPUT_A_PRESSED) {
        m->framesSinceA = 0;
    } else if (m->framesSinceA < 0xFF) {
        m->framesSinceA++;
    }

    if (m->input & INPUT_B_PRESSED) {
        m->framesSinceB = 0;
    } else if (m->framesSinceB < 0xFF) {
        m->framesSinceB++;
    }

    if (m->input & INPUT_Z_PRESSED) {
        m->zHeldFrames = 0;
        // Z wins a simultaneous B press in idle; keep its delayed crouch request.
        m->zCrouchBuffered = (m->action == ACT_IDLE || m->action == ACT_WALKING)
                             && !(m->controller->buttonDown & ~(Z_TRIG | B_BUTTON));
    } else if (!(m->input & INPUT_Z_DOWN) || (m->controller->buttonDown & ~Z_TRIG)
               || (m->action != ACT_IDLE && m->action != ACT_WALKING)) {
        m->zCrouchBuffered = FALSE;
    }

    if (!(m->input & INPUT_Z_PRESSED) && ((m->input & INPUT_Z_DOWN) || m->zCrouchBuffered)
        && m->zHeldFrames < 1) {
        m->zHeldFrames++;
    } else if (!(m->input & INPUT_Z_DOWN) && !m->zCrouchBuffered) {
        m->zHeldFrames = 0;
    }
    mario_update_move_buffers(m);
    if (groundPoundLocked && (m->action & ACT_FLAG_AIR)) {
        m->input &= ~(INPUT_Z_DOWN | INPUT_Z_PRESSED);
    }
}

/**
 * Table-free stick direction using libdragon's octant reduction and a higher
 * accuracy degree-11 arctangent polynomial. Finite controller inputs only.
 * Sources: libdragon preview/src/math/fmath.c (public domain), and coefficients
 * published at https://mazzo.li/posts/vectorized-atan2.html.
 * Scalar float operations suit VR4300; no SIMD, FMA, or libm call is required.
 * Round the first octant before reflecting to preserve exact axis/diagonal
 * directions and mirror symmetry in SM64's angle convention.
 */
static s16 mario_stick_angle(f32 forward, f32 sideways) {
    f32 af = absf(forward);
    f32 as = absf(sideways);
    f32 largest = af > as ? af : as;
    if (largest == 0.0f) {
        return 0;
    }
    f32 ratio = (af < as ? af : as) / largest;
    f32 square = ratio * ratio;
    f32 polynomial = -0.01172120f;
    polynomial = polynomial * square + 0.05265332f;
    polynomial = polynomial * square - 0.11643287f;
    polynomial = polynomial * square + 0.19354346f;
    polynomial = polynomial * square - 0.33262347f;
    polynomial = polynomial * square + 0.99997726f;
    u32 angle = (u32) (ratio * polynomial * 10430.37835f + 0.5f);
    if (as > af) {
        angle = 0x4000 - angle;
    }
    if (forward < 0.0f) {
        angle = 0x8000 - angle;
    }
    if (sideways < 0.0f) {
        angle = 0x10000 - angle;
    }
    return (s16) (u16) angle;
}

/**
 * Updates the joystick intended magnitude and camera-relative direction.
 */
void update_mario_joystick_inputs(struct MarioState *m) {
    struct Controller *controller = m->controller;
    f32 mag = ((controller->stickMag / 64.0f) * (controller->stickMag / 64.0f)) * 64.0f;

    // A dive-grab stand-up cannot steer, including its same-frame transition
    // into a holding action when the animation ends.
    if (m->action == ACT_DIVE_PICKING_UP) {
        m->intendedMag = 0.0f;
        m->intendedYaw = m->faceAngle[1];
        return;
    }

    if (m->squishTimer == 0) {
        m->intendedMag = mag / 2.0f;
    } else {
        m->intendedMag = mag / 8.0f;
    }

    if (m->intendedMag > 0.0f) {
        m->intendedYaw =
            mario_stick_angle(-controller->stickY, controller->stickX) + m->area->camera->yaw;
        m->input |= INPUT_NONZERO_ANALOG;
    } else {
        m->intendedYaw = m->faceAngle[1];
    }
}

static struct {
    struct MarioState *mario;
    struct Object *marioObj;
    struct Area *area;
    struct Surface *floor;
    struct Surface *ceil;
    Vec3f pos;
    f32 floorHeight;
    f32 ceilHeight;
    u32 staticSurfaceGeneration;
    s32 valid;
} sIdleGeometryCache;

// Dynamic floors, ceilings, or walls can appear even while Mario stands still.
// Reuse only the previous static result when none occupies the cells queried
// by Mario's floor, ceiling, and two wall searches.
static s32 mario_idle_geometry_is_static(struct MarioState *m) {
    s32 x = m->pos[0];
    s32 z = m->pos[2];
    s32 cellX, cellZ;

    if (m->action != ACT_IDLE || gCurrentArea == NULL || m->area != gCurrentArea || m->floor == NULL
        || (m->floor->flags & SURFACE_FLAG_DYNAMIC) || m->floor->object != NULL
        || (m->ceil != NULL && ((m->ceil->flags & SURFACE_FLAG_DYNAMIC) || m->ceil->object != NULL))
        || (m->flags & MARIO_VANISH_CAP) || m->floor == &gWaterSurfacePseudoFloor
        || (m->marioObj->activeFlags & ACTIVE_FLAG_MOVE_THROUGH_GRATE) || o != m->marioObj
        || gCollisionFlags != 0 || is_outside_level_bounds(x - 50, z - 50)
        || is_outside_level_bounds(x + 50, z + 50)) {
        return FALSE;
    }

    cellX = GET_CELL_COORD(x);
    cellZ = GET_CELL_COORD(z);
    if (!surface_query_cell_is_empty(cellX, cellZ, SPATIAL_PARTITION_FLOORS, SURFACE_QUERY_DYNAMIC)
        || !surface_query_cell_is_empty(cellX, cellZ, SPATIAL_PARTITION_CEILS, SURFACE_QUERY_DYNAMIC)) {
        return FALSE;
    }

    for (cellX = GET_CELL_COORD(x - 50); cellX <= GET_CELL_COORD(x + 50); cellX++) {
        for (cellZ = GET_CELL_COORD(z - 50); cellZ <= GET_CELL_COORD(z + 50); cellZ++) {
            if (!surface_query_cell_is_empty(cellX, cellZ, SPATIAL_PARTITION_WALLS,
                                             SURFACE_QUERY_DYNAMIC)) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

static void resolve_mario_geometry_walls(struct MarioState *m) {
    if (m->action & ACT_FLAG_SWIMMING) {
        mario_water_collision_prepare(m);
    } else {
        f32_find_wall_collision(&m->pos[0], &m->pos[1], &m->pos[2], 60.0f, 50.0f);
        f32_find_wall_collision(&m->pos[0], &m->pos[1], &m->pos[2], 30.0f, 24.0f);
    }
}

static void update_mario_poison_input(struct MarioState *m, f32 gasLevel) {
    // Any body contact counts, including the shallow edge of a poison volume.
    if (m->pos[1] < gasLevel) {
        m->input |= INPUT_IN_POISON_GAS;
    }
}

/**
 * Resolves wall collisions, and updates a variety of inputs.
 */
void update_mario_geometry_inputs(struct MarioState *m) {
    f32 gasLevel;
    f32 ceilToFloorDist;
    Vec3f startPos;
    s32 swimming = (m->action & ACT_FLAG_SWIMMING) != 0;
    s32 ordinaryCollisionFlags = gCollisionFlags == 0;
    s32 reuseIdleGeometry =
        sIdleGeometryCache.valid && sIdleGeometryCache.mario == m
        && sIdleGeometryCache.marioObj == m->marioObj && sIdleGeometryCache.area == m->area
        && sIdleGeometryCache.staticSurfaceGeneration == gStaticSurfaceGeneration
        && sIdleGeometryCache.pos[0] == m->pos[0] && sIdleGeometryCache.pos[1] == m->pos[1]
        && sIdleGeometryCache.pos[2] == m->pos[2] && sIdleGeometryCache.floor == m->floor
        && sIdleGeometryCache.ceil == m->ceil && sIdleGeometryCache.floorHeight == m->floorHeight
        && sIdleGeometryCache.ceilHeight == m->ceilHeight && mario_idle_geometry_is_static(m);

    if (!reuseIdleGeometry) {
        sIdleGeometryCache.valid = FALSE;
        vec3f_copy(startPos, m->pos);
        if (swimming) {
            mario_water_collision_prepare_geometry(m);
        } else {
            resolve_mario_geometry_walls(m);
            m->floorHeight = find_floor(m->pos[0], m->pos[1], m->pos[2], &m->floor);

            // If Mario is OOB, query at the previous graphical position. This
            // retains the existing fallback even during astral projection.
            if (m->floor == NULL) {
                vec3f_copy(m->pos, m->marioObj->header.gfx.pos);
                m->floorHeight = find_floor(m->pos[0], m->pos[1], m->pos[2], &m->floor);
            }
            m->ceilHeight = find_mario_ceil(m->pos, m->floorHeight, &m->ceil);
        }
        if (ordinaryCollisionFlags && startPos[0] == m->pos[0] && startPos[1] == m->pos[1]
            && startPos[2] == m->pos[2] && mario_idle_geometry_is_static(m)) {
            sIdleGeometryCache.mario = m;
            sIdleGeometryCache.marioObj = m->marioObj;
            sIdleGeometryCache.area = m->area;
            sIdleGeometryCache.floor = m->floor;
            sIdleGeometryCache.ceil = m->ceil;
            vec3f_copy(sIdleGeometryCache.pos, m->pos);
            sIdleGeometryCache.floorHeight = m->floorHeight;
            sIdleGeometryCache.ceilHeight = m->ceilHeight;
            sIdleGeometryCache.staticSurfaceGeneration = gStaticSurfaceGeneration;
            sIdleGeometryCache.valid = TRUE;
        }
    }
    gasLevel = find_poison_gas_level(m->pos[0], m->pos[2]);
    m->waterLevel = find_water_level(m->pos[0], m->pos[2]);
    mario_update_water_walking_state(m, m->waterLevel, m->ceilHeight);

    if (m->floor != NULL) {
        if (m->floorHeight < m->waterLevel
            && mario_can_walk_on_water(m, m->waterLevel, m->ceilHeight)) {
            m->floor = &gWaterSurfacePseudoFloor;
            m->floorHeight = m->waterLevel;
            m->floor->originOffset = -m->floorHeight;
            if (!(m->action & ACT_FLAG_AIR)) {
                m->pos[1] = MAX(m->pos[1], m->floorHeight);
            }
        }
        m->floorYaw = atan2s(m->floor->normal.z, m->floor->normal.x);
        m->terrainSoundAddend = mario_get_terrain_sound_addend(m);

        if ((m->pos[1] > m->waterLevel - 40) && mario_floor_is_slippery(m)) {
            m->input |= INPUT_ABOVE_SLIDE;
        }

        if ((m->floor->flags & SURFACE_FLAG_DYNAMIC)
            || (m->ceil && m->ceil->flags & SURFACE_FLAG_DYNAMIC)) {
            ceilToFloorDist = m->ceilHeight - m->floorHeight;

            if ((0.0f <= ceilToFloorDist) && (ceilToFloorDist <= 150.0f)) {
                m->input |= INPUT_SQUISHED;
            }
        }

        if (m->pos[1] > m->floorHeight + 100.0f) {
            m->input |= INPUT_OFF_FLOOR;
        }

        if (m->pos[1] < (m->waterLevel - 10)) {
            m->input |= INPUT_IN_WATER;
        }

        update_mario_poison_input(m, gasLevel);

    } else {
        level_trigger_warp(m, WARP_OP_DEATH);
    }
}

/** Require neutral stick for entry; in first person the stick controls looking. */
static void mario_update_c_up_input(struct MarioState *m) {
    if (gCameraMovementFlags & CAM_MOVE_C_UP_MODE) {
        // Moving the stick before entry cancels the request instead of
        // leaving it queued until Mario eventually comes to a stop.
        if ((m->action != ACT_FIRST_PERSON && m->controller->stickMag > 0.0f)
            || !(m->action & ACT_FLAG_ALLOW_FIRST_PERSON)) {
            gCameraMovementFlags &= ~CAM_MOVE_C_UP_MODE;
        } else {
            m->input |= INPUT_FIRST_PERSON;
        }
    }
}

/** Handles Mario's input flags as well as a couple timers. */
void update_mario_inputs(struct MarioState *m) {
    mario_tick_water_walking_cooldown(m);
    m->groundTakeoffThisFrame = FALSE;
    m->particleFlags = PARTICLE_NONE;
    m->input = INPUT_NONE;
    m->zBPressed = FALSE;
    m->collidedObjInteractTypes = m->marioObj->collidedObjInteractTypes;
    m->flags &= 0xFFFFFF;


    update_mario_button_inputs(m);
    update_mario_joystick_inputs(m);
    mario_update_wall_kick_slide_input(m);
    update_mario_geometry_inputs(m);
    mario_update_soaked_status(m);
#ifdef VANILLA_DEBUG
    debug_print_speed_action_normal(m);
#endif
    mario_update_c_up_input(m);

    if (!(m->input & (INPUT_NONZERO_ANALOG | INPUT_A_PRESSED))) {
        m->input |= INPUT_IDLE;
    }

    // These 3 flags are defined by Bowser stomping attacks
    if (m->marioObj->oInteractStatus
        & (INT_STATUS_MARIO_STUNNED | INT_STATUS_MARIO_KNOCKBACK_DMG | INT_STATUS_MARIO_SHOCKWAVE)) {
        m->input |= INPUT_STOMPED;
    }

    // This function is located near other unused trampoline functions,
    // perhaps logically grouped here with the timers.
    stub_mario_step_1(m);

    if (m->wallKickTimer > 0) {
        m->wallKickTimer--;
    }

    if (m->doubleJumpTimer > 0) {
        m->doubleJumpTimer--;
    }
}

static struct {
    Vec3f lastPos;
    s32 active;
} sSubmergedTrail;

static void update_mario_submerged_effects(struct MarioState *m) {
    s32 vanishUnderwater = mario_has_current_vanish_cap(m) && mario_is_head_underwater(m);

    if ((mario_has_current_vanish_cap(m) && m->pos[1] < m->waterLevel - 10.0f)
        || (mario_has_land_movement_underwater(m) && mario_is_head_underwater(m))) {
        // Vanish leaves a trail even in shallow water, without droplets or splash bursts.
        m->particleFlags &=
            ~(PARTICLE_SHALLOW_WATER_WAVE | PARTICLE_SHALLOW_WATER_SPLASH | PARTICLE_WATER_SPLASH);
    }

    // Mark Mario's position on the surface while he is out of sight below it.
    if (((m->action & ACT_GROUP_MASK) == ACT_GROUP_SUBMERGED || mario_has_land_movement_underwater(m))
        && m->pos[1] < (f32) (m->waterLevel - 130)) {
        m->particleFlags |= PARTICLE_IDLE_WATER_WAVE;
        if (m->pos[1] <= m->waterLevel - 500.0f) {
            m->particleFlags &= ~PARTICLE_WAVE_TRAIL;
            sSubmergedTrail.active = FALSE;
        } else if (!sSubmergedTrail.active) {
            vec3f_copy(sSubmergedTrail.lastPos, m->pos);
            sSubmergedTrail.active = TRUE;
        } else {
            f32 dx = m->pos[0] - sSubmergedTrail.lastPos[0];
            f32 dz = m->pos[2] - sSubmergedTrail.lastPos[2];
            if (dx * dx + dz * dz >= 100.0f * 100.0f) {
                m->particleFlags |= PARTICLE_WAVE_TRAIL;
                vec3f_copy(sSubmergedTrail.lastPos, m->pos);
            }
        }
    } else {
        sSubmergedTrail.active = FALSE;
    }

    // Bubbles are visual: Vanish keeps them even while using land actions and needing no air.
    if (!(m->action & ACT_FLAG_INTANGIBLE)
        && (vanishUnderwater
            || ((m->action & ACT_GROUP_MASK) == ACT_GROUP_SUBMERGED
                && !(m->flags & MARIO_METAL_CAP)))) {
        if (vanishUnderwater || (m->pos[1] < (f32) (m->waterLevel - 160))
            || (m->faceAngle[0] < -0x800)) {
            m->particleFlags |= PARTICLE_BUBBLE;
        }
    }
}

/**
 * Both increments and decrements Mario's HP.
 */
// Lose whole wedges at two-thirds of the original drain rate.
// Every reset starts a full wedge interval without restoring health.
#define WATER_HEALTH_DRAIN_PERIOD 3
#define WATER_HEALTH_DRAIN_TICKS 2
#define WATER_HEALTH_WEDGE 0x100
#define WATER_HEALTH_WEDGE_INTERVAL (WATER_HEALTH_WEDGE * WATER_HEALTH_DRAIN_PERIOD)
#define POISON_HEALTH_DRAIN_TICKS 4
#define POISON_HEALTH_REENTRY_RESET_FRAMES (10 * 30)
#define WATER_HEALTH_REENTRY_RESET_FRAMES 30 // One second at 30 updates/sec.
#define WATER_BREATH_DIVE_FRAMES 150         // Must spend more than five seconds underwater.
#define WATER_BREATH_SURFACE_DEPTH 130       // Match the swimming surface-particle cutoff.
#define WATER_BREATH_VOICE_PRIORITY 0x70     // Let normal jump voices take priority after surfacing.
#define HEALTH_REGEN_START_FRAMES (45 * 30)
#define HEALTH_REGEN_WEDGE_FRAMES (3 * 30)
#define HEALTH_REGEN_SHALLOW_RESET_FRAMES 90

static struct {
    u16 underwaterFrames;
    u8 voiceFrames;
    u32 surfaceVoice;
    u32 playingVoice;
} sSurfaceBreath;

static struct {
    s16 previousHealth;
    u16 drainProgress;
    u8 outOfWaterFrames;
    u8 shallowDeathPending;
} sWaterHealth = { .previousHealth = 0x880 };

static struct {
    u16 drainProgress;
    u16 cleanAirFrames;
    u8 protected;
} sPoisonHealth;

static s32 mario_has_shallow_water_death_pending(struct MarioState *m) {
    return sWaterHealth.shallowDeathPending && !game_options_secrets_blocked()
           && mario_is_shallow_water_drowning(m);
}

static void reset_environmental_health_progress(void) {
    sWaterHealth.drainProgress = 0;
    sWaterHealth.shallowDeathPending = FALSE;
    sPoisonHealth.drainProgress = 0;
}

static u16 sHealthRegenFrames;
static u16 sHealthRegenInactiveFrames;
static u8 sShallowRegenExposureFrames;

s32 mario_has_cap_for_damage(struct MarioState *m) {
    return (m->flags & MARIO_CAP_ON_HEAD) || m->action == ACT_PUTTING_ON_CAP
           || m->action == ACT_PUTTING_ON_CAP_AIR;
}

void mario_reset_health_regeneration(struct MarioState *m) {
    reset_environmental_health_progress();
    sHealthRegenFrames = 0;
    sHealthRegenInactiveFrames = 0;
    sShallowRegenExposureFrames = 0;
    sWaterHealth.previousHealth = m->health;
}

// Keep queued hits and exposure timers from becoming delayed damage after disable.
static s32 mario_update_infinite_health(struct MarioState *m) {
    if (!game_options_cheat_active(GO_INFINITE_HEALTH)) {
        return FALSE;
    }
    if (m->hurtCounter != 0) {
        m->invincTimer = MAX(m->invincTimer, 30);
    }
    m->health = 0x880;
    m->hurtCounter = 0;
    m->healCounter = 0;
    m->respawnHealCounter = 0;
    m->yoshiCoinDeathPending = FALSE;
#ifdef BREATH_METER
    m->breath = 0x880;
    m->breathCounter = 0;
#endif
    mario_reset_health_regeneration(m);
    return TRUE;
}

void mario_apply_burning_damage(struct MarioState *m, s32 timerStep) {
    if (game_options_cheat_active(GO_INFINITE_HEALTH)) {
        return;
    }
    s32 timer = m->marioObj->oMarioBurnTimer;
    s32 previous = CLAMP(timer - timerStep, 0, MARIO_BURN_TIMER_LIMIT);
    s32 current = CLAMP(timer, 0, MARIO_BURN_TIMER_LIMIT);

    // The timer retains progress, but health changes only at whole-wedge boundaries.
    s32 wedges = MARIO_BURN_DAMAGE_TOTAL / 0x100;
    s32 damage =
        (current * wedges / MARIO_BURN_TIMER_LIMIT - previous * wedges / MARIO_BURN_TIMER_LIMIT)
        * 0x100;
    player_stats_health(m->health, m->health - damage);
    m->health -= damage;
}

static void reset_water_health_and_breath(void) {
    sSurfaceBreath.underwaterFrames = 0;
    sSurfaceBreath.voiceFrames = 0;
    sSurfaceBreath.surfaceVoice = 0;
    sSurfaceBreath.playingVoice = 0;
    sWaterHealth.previousHealth = 0x880;
    sWaterHealth.drainProgress = 0;
    sWaterHealth.outOfWaterFrames = 0;
    sWaterHealth.shallowDeathPending = FALSE;
    sPoisonHealth.drainProgress = 0;
    sPoisonHealth.cleanAirFrames = 0;
    sPoisonHealth.protected = FALSE;
}

static void update_surface_breath_sound(struct MarioState *m) {
    s32 secondFrame;
    s32 lastFrame;
    s32 lowPose = mario_is_low_land_pose(m);
    s32 underwater = m->pos[1] < m->waterLevel - (lowPose ? 50 : WATER_BREATH_SURFACE_DEPTH);

    // Retain the last clip after the sequence ends so diving can also stop
    // the final cough or pant while it is still audible.
    if (underwater && sSurfaceBreath.playingVoice != 0) {
        stop_sound(sSurfaceBreath.playingVoice, m->marioObj->header.gfx.cameraToObject);
        sSurfaceBreath.playingVoice = 0;
    }

    if ((!(m->action & ACT_FLAG_SWIMMING)
         && !(mario_has_land_movement_underwater(m) && (m->input & INPUT_IN_WATER)) && !lowPose
         && sSurfaceBreath.underwaterFrames == 0 && sSurfaceBreath.surfaceVoice == 0)
        || (m->action & ACT_FLAG_INTANGIBLE)
        || (m->action == ACT_COUGHING && m->actionArg == ACT_ARG_COUGHING_SHALLOW_WATER)
        || mario_has_current_vanish_cap(m) || m->health < 0x100) {
        if (sSurfaceBreath.playingVoice != 0) {
            stop_sound(sSurfaceBreath.playingVoice, m->marioObj->header.gfx.cameraToObject);
            sSurfaceBreath.playingVoice = 0;
        }
        sSurfaceBreath.underwaterFrames = 0;
        sSurfaceBreath.surfaceVoice = 0;
        return;
    }

    if (underwater) {
        if (sSurfaceBreath.underwaterFrames <= WATER_BREATH_DIVE_FRAMES) {
            sSurfaceBreath.underwaterFrames++;
        }
        sSurfaceBreath.surfaceVoice = 0;
        return;
    }

    // Choose one sequence on surfacing, even while swimming. Staying at the
    // surface or changing health there must not restart it.
    if (sSurfaceBreath.underwaterFrames > WATER_BREATH_DIVE_FRAMES) {
        sSurfaceBreath.surfaceVoice = m->health < 0x300 ? SOUND_MARIO_COUGHING1 : SOUND_MARIO_PANTING;
        sSurfaceBreath.voiceFrames = 0;
    }
    sSurfaceBreath.underwaterFrames = 0;
    if (sSurfaceBreath.surfaceVoice == 0) {
        return;
    }

    // A pair, then one final breath. Panting needs wider spacing so its
    // longer recording finishes before the next play_sound call.
    secondFrame = sSurfaceBreath.surfaceVoice == SOUND_MARIO_PANTING ? 25 : 10;
    lastFrame = sSurfaceBreath.surfaceVoice == SOUND_MARIO_PANTING ? 65 : 35;
    if (sSurfaceBreath.voiceFrames == 0 || sSurfaceBreath.voiceFrames == secondFrame
        || sSurfaceBreath.voiceFrames == lastFrame) {
        sSurfaceBreath.playingVoice = sSurfaceBreath.surfaceVoice;
        play_sound_with_rumble((sSurfaceBreath.surfaceVoice & ~SOUNDARGS_MASK_PRIORITY)
                                   | (WATER_BREATH_VOICE_PRIORITY << SOUNDARGS_SHIFT_PRIORITY),
                               m->marioObj->header.gfx.cameraToObject,
                               sSurfaceBreath.surfaceVoice == SOUND_MARIO_PANTING ? 3 : 5,
                               sSurfaceBreath.surfaceVoice == SOUND_MARIO_PANTING ? 30 : 80);
    }
    if (sSurfaceBreath.voiceFrames++ == lastFrame) {
        sSurfaceBreath.surfaceVoice = 0;
    }
}

#ifndef BREATH_METER
static void update_low_health_warning(struct MarioState *m) {
    static s16 previousHealth = 0x880;
    static u8 warningWedges = 0;
    static u8 beepsRemaining = 0;
    static u8 gapFrames = 0;
    static u8 beepActive = FALSE;
    static u8 beepObserved = FALSE;
    static u8 requestFrames = 0;
    u8 wedges = m->health >= 0x100 ? (m->health >> 8) : 0;
    u8 previousWedges = previousHealth >= 0x100 ? (previousHealth >> 8) : 0;
    u8 playing = 0, queued = 0, soundId;
    s32 dying = FALSE;

    switch (m->action) {
        case ACT_STANDING_DEATH:
        case ACT_QUICKSAND_DEATH:
        case ACT_ELECTROCUTION:
        case ACT_SUFFOCATION:
        case ACT_DEATH_ON_STOMACH:
        case ACT_DEATH_ON_BACK:
        case ACT_DROWNING:
        case ACT_WATER_DEATH:
        case ACT_DEATH_EXIT:
        case ACT_UNUSED_DEATH_EXIT:
        case ACT_FALLING_DEATH_EXIT:
        case ACT_SPECIAL_DEATH_EXIT:
        case ACT_DEATH_EXIT_LAND:
            dying = TRUE;
            break;
    }

    if (beepActive) {
        get_currently_playing_sound(SOUND_BANK_HEALTH, &playing, &queued, &soundId);
    }

    // Damage drains over several frames. Stay silent if the queued damage
    // will kill Mario, including any queued healing in the prediction.
    if (dying || wedges == 0
        || m->health + 0x40 * (s32) m->healCounter - 0x100 * (((s32) m->hurtCounter + 3) / 4) < 0x100) {
        stop_sound(SOUND_HEALTH_WARNING_PULSE, gGlobalSoundSource);
        warningWedges = 0;
        beepsRemaining = 0;
        gapFrames = 0;
        beepActive = FALSE;
        beepObserved = FALSE;
        requestFrames = 0;
        previousHealth = m->health;
        return;
    }

    if (wedges > 2) {
        warningWedges = 0;
        beepsRemaining = 0;
        gapFrames = 0;
    } else if (m->health > previousHealth || m->healCounter != 0) {
        // Healing ends the initial sequence, but low health still gets reminders.
        // Let any current pulse finish before waiting five seconds.
        warningWedges = wedges;
        beepsRemaining = 1;
        gapFrames = 150;
    } else if (wedges < previousWedges) {
        warningWedges = wedges;
        beepsRemaining = wedges == 2 ? 5 : 10;
        // Carry an unfinished pulse into the critical warning's ten-pulse total.
        // An observed pulse whose bank is now empty has already finished.
        if (wedges == 1 && beepActive && (playing != 0 || queued != 0 || !beepObserved)) {
            beepsRemaining--;
        }
        gapFrames = 11; // Approximately 0.35 seconds at 30 updates/second.
    }
    previousHealth = m->health;

    if (beepActive) {
        if (playing != 0 || queued != 0) {
            beepObserved = TRUE;
            return;
        }
        // Requests are asynchronous. Wait to observe playback before treating
        // an empty bank as completion; abandon a suppressed request after a second.
        if (!beepObserved && ++requestFrames < 30) {
            return;
        }
        beepActive = FALSE;
        if (warningWedges != 0 && beepsRemaining == 0) {
            // After the sequence, repeat one full pulse five seconds after
            // the previous pulse finishes.
            beepsRemaining = 1;
            gapFrames = 150;
        } else if (gapFrames == 0) {
            // Preserve a newly armed initial delay or post-healing reminder.
            gapFrames = warningWedges == 2 ? 30 : 0;
        }
    }
    if (beepsRemaining == 0) {
        return;
    }
    if (gapFrames != 0) {
        gapFrames--;
        return;
    }

    // A dedicated one-shot bank keeps movement sounds from replacing the beep.
    // Wait for actual completion instead of cutting a looping sound into pieces.
    play_sound_with_rumble(SOUND_HEALTH_WARNING_PULSE, gGlobalSoundSource, 3, 30);
    beepsRemaining--;
    beepActive = TRUE;
    beepObserved = FALSE;
    requestFrames = 0;
}
#endif

// Some resting states cannot play the sleep animation (holding, crouching,
// swimming, attached holds). Give them the same inactivity limit anyway.
static s32 mario_health_regeneration_sleep_due(struct MarioState *m) {
    s32 sleepDelay =
        (m->flags & MARIO_CAPS) ? IDLE_SLEEP_DELAY_CAPPED_FRAMES : IDLE_SLEEP_DELAY_CAPLESS_FRAMES;
    if (m->action == ACT_START_SLEEPING || m->action == ACT_SLEEPING) {
        sHealthRegenInactiveFrames = 0;
        return TRUE;
    }
    if (!(m->action & ACT_FLAG_STATIONARY) || (m->action & (ACT_FLAG_ATTACKING | ACT_FLAG_INTANGIBLE))
        || (m->input & INPUT_NONZERO_ANALOG) || m->controller->buttonPressed != 0
        || absf(m->forwardVel) > 0.5f) {
        sHealthRegenInactiveFrames = 0;
        return FALSE;
    }
    // Held Z can maintain a resting crouch; fresh input or movement restarts
    // inactivity. This clock is independent of camera/HUD idle detection.
    if (sHealthRegenInactiveFrames < sleepDelay) {
        sHealthRegenInactiveFrames++;
    }
    return sHealthRegenInactiveFrames >= sleepDelay;
}

static void update_water_health_reentry(s32 inWater, s32 terrainIsSnow, s32 headUnderwater) {
    // Land actions (including shallow-water wading) and normal-water surface
    // breathing count toward a reset after WATER_HEALTH_REENTRY_RESET_FRAMES.
    // Cold water drains at the surface too, so surfacing must preserve progress.
    if (!inWater || (!terrainIsSnow && !headUnderwater)) {
        if (sWaterHealth.outOfWaterFrames < WATER_HEALTH_REENTRY_RESET_FRAMES) {
            sWaterHealth.outOfWaterFrames++;
        }
    } else {
        if (sWaterHealth.outOfWaterFrames >= WATER_HEALTH_REENTRY_RESET_FRAMES) {
            sWaterHealth.drainProgress = 0;
        }
        sWaterHealth.outOfWaterFrames = 0;
    }
}

static void update_poison_health_reentry(struct MarioState *m, s32 poisonProtected) {
    // Gaining protection restarts poison's interval; protection is not clean air.
    if (poisonProtected && !sPoisonHealth.protected) {
        sPoisonHealth.drainProgress = 0;
    }
    sPoisonHealth.protected = poisonProtected;
    if (!(m->input & INPUT_IN_POISON_GAS)) {
        if (sPoisonHealth.cleanAirFrames < POISON_HEALTH_REENTRY_RESET_FRAMES) {
            sPoisonHealth.cleanAirFrames++;
        }
        if (sPoisonHealth.cleanAirFrames >= POISON_HEALTH_REENTRY_RESET_FRAMES) {
            sPoisonHealth.drainProgress = 0;
        }
    } else {
        sPoisonHealth.cleanAirFrames = 0;
    }
}

static void finish_mario_health_update(struct MarioState *m, s16 healthAtStart, s32 waterDrainApplied,
                                       s32 poisonDrainApplied, s32 lowPoseUnderwater) {
    // Remember the final value so our own drain is not mistaken for outside damage.
    if (m->health != healthAtStart - waterDrainApplied || m->health < 0x100) {
        sWaterHealth.drainProgress = 0;
    }
    if (m->health != healthAtStart - poisonDrainApplied || m->health < 0x100) {
        sPoisonHealth.drainProgress = 0;
    }
    sWaterHealth.previousHealth = m->health;
    sWaterHealth.shallowDeathPending = waterDrainApplied != 0 && m->health < 0x100 && lowPoseUnderwater
                                       && m->pos[1] >= m->waterLevel - 100;
    if (sWaterHealth.shallowDeathPending && mario_character_can_unlock(m)) {
        m->quicksandDepth = 0.0f;
        set_mario_action(m, ACT_QUICKSAND_DEATH, ACT_ARG_QUICKSAND_DEATH_HE_THICC_UNLOCK);
    }
    if (waterDrainApplied != 0 && m->health >= 0x100 && lowPoseUnderwater
        && m->pos[1] >= m->waterLevel - 100) {
        // Lethal shallow-water loss keeps its sink death; survivable loss
        // interrupts the low pose with one complete cough.
        set_mario_action(m, ACT_COUGHING, ACT_ARG_COUGHING_SHALLOW_WATER);
        set_mario_animation(m, MARIO_ANIM_COUGHING);
    }
    update_surface_breath_sound(m);
#ifndef BREATH_METER
    update_low_health_warning(m);
#endif
}

s32 mario_health_after_pending_damage(struct MarioState *m) {
    if (game_options_cheat_active(GO_INFINITE_HEALTH)) {
        return 0x880;
    }
    s32 health = m->health;
    s32 hurtCounter = m->hurtCounter;
    s32 healCounter = m->healCounter;

    if (health < 0x100) {
        return 0xFF;
    }
    // Pending healing must keep Mario alive until its ticks actually arrive.
    // Mirror the health update's heal-before-debit order and overflow hold.
    while (hurtCounter > 0) {
        if (healCounter > 0 && (hurtCounter % 4 == 1 || health <= 0x840)) {
            health += 0x40;
            healCounter--;
        }
        hurtCounter--;
        if (hurtCounter % 4 == 0) {
            health -= 0x100;
        }
        health = MIN(health, 0x880);
        if (health < 0x100) {
            return 0xFF;
        }
    }
    return MIN(health + 0x40 * healCounter, 0x880);
}

void mario_queue_respawn_healing(struct MarioState *m) {
    m->healCounter = 31;
    m->respawnHealCounter = 31;
}

void update_mario_health(struct MarioState *m) {
    if (mario_update_infinite_health(m)) {
        return;
    }
    if (castle_secret_lobby_active()) {
        m->health = 0x880;
        m->hurtCounter = 0;
        m->healCounter = 0;
        m->respawnHealCounter = 0;
        mario_reset_health_regeneration(m);
        return;
    }

    s32 terrainIsSnow =
        (m->area->terrainType & TERRAIN_MASK) == TERRAIN_SNOW
        || physics_lab_is_cold_pool(gCurrLevelNum, gCurrAreaIndex, m->pos[0], m->pos[2]);
    s32 waterDrainAmount = 0;
    s32 waterDrainApplied = 0;
    s32 poisonDrainAmount = 0;
    s32 poisonDrainApplied = 0;
    s16 healthAtStart = m->health;
    s32 healthChangedExternally = m->health != sWaterHealth.previousHealth;
    s32 damageObserved = m->health < sWaterHealth.previousHealth || m->hurtCounter > 0;
    s32 vanishUnderwater = mario_has_land_movement_underwater(m) && m->pos[1] < m->waterLevel - 100;
    s32 headUnderwater = mario_is_head_underwater(m);
    s32 lowPoseUnderwater = mario_is_low_land_pose(m) && headUnderwater;
    s32 inWater =
        (m->action & ACT_GROUP_MASK) == ACT_GROUP_SUBMERGED || vanishUnderwater || lowPoseUnderwater;
    s32 vulnerableToExposure =
        m->health >= 0x100 && !(m->action & ACT_FLAG_INTANGIBLE) && !gDebugLevelSelect;
    s32 shallowExposure = vulnerableToExposure && !mario_has_water_damage_protection(m)
                          && mario_is_shallow_water_drowning(m);
    s32 deepExposure =
        vulnerableToExposure && !mario_has_water_damage_protection(m) && inWater && !shallowExposure;
    s32 poisonExposure =
        vulnerableToExposure && !(m->flags & MARIO_METAL_CAP) && (m->input & INPUT_IN_POISON_GAS);
    s32 poisonProtected =
        (m->flags & MARIO_METAL_CAP) || (m->action & ACT_FLAG_INTANGIBLE) || gDebugLevelSelect;
    s32 sleepDue = mario_health_regeneration_sleep_due(m);

    update_water_health_reentry(inWater, terrainIsSnow, headUnderwater);
    update_poison_health_reentry(m, poisonProtected);

    if (m->health >= 0x100) {
        // When already healing or hurting Mario, Mario's HP is not changed any more here.
        if (((u32) m->healCounter | (u32) m->hurtCounter) == 0) {
            if ((m->input & INPUT_IN_POISON_GAS) && !(m->action & ACT_FLAG_INTANGIBLE)) {
                if (!(m->flags & MARIO_METAL_CAP) && !gDebugLevelSelect) {
                    poisonDrainAmount =
                        POISON_HEALTH_DRAIN_TICKS * (mario_has_cap_for_damage(m) ? 1 : 2);
                }
            } else {
                if (((m->action & ACT_FLAG_SWIMMING) || vanishUnderwater || lowPoseUnderwater)
                    && !mario_has_water_damage_protection(m) && !(m->action & ACT_FLAG_INTANGIBLE)) {
#ifdef BREATH_METER
                    // Cold water advances the wedge timer three times as fast.
                    if (headUnderwater && terrainIsSnow) {
                        waterDrainAmount = 3;
                    }
#else
                    // Normal-water surfacing stops drain but does not restore health.
                    // Cold water still drains at the surface; debug level select disables drain.
                    if (!gDebugLevelSelect && (terrainIsSnow || headUnderwater)) {
                        waterDrainAmount = terrainIsSnow ? 3 : 1;
                    }
#endif
                }
            }
        }

        if (mario_breakdance_protects_from_damage(m)) {
            waterDrainAmount = 0;
            poisonDrainAmount = 0;
        }
        // Accumulate whole wedges: capped Mario drains at 2/3 vanilla speed;
        // hatless Mario uses the vanilla rate. Keep progress when cap status changes.
        // Damage/healing restarts the interval; other pauses retain it.
        if (waterDrainAmount != 0 && !healthChangedExternally && m->health == healthAtStart
            && m->healCounter == 0 && m->hurtCounter == 0) {
            sWaterHealth.drainProgress +=
                (mario_has_cap_for_damage(m) ? WATER_HEALTH_DRAIN_TICKS : WATER_HEALTH_DRAIN_PERIOD)
                * waterDrainAmount;
            if (sWaterHealth.drainProgress >= WATER_HEALTH_WEDGE_INTERVAL) {
                player_stats_health(m->health, m->health - WATER_HEALTH_WEDGE);
                m->health -= WATER_HEALTH_WEDGE;
                waterDrainApplied = WATER_HEALTH_WEDGE;
                sWaterHealth.drainProgress -= WATER_HEALTH_WEDGE_INTERVAL;
            }
        } else if (healthChangedExternally || m->health != healthAtStart || m->healCounter != 0
                   || m->hurtCounter != 0) {
            reset_environmental_health_progress();
        }

        // Poison has its own interval: four health units/update, doubled without the hat.
        if (poisonDrainAmount != 0 && !healthChangedExternally && m->health == healthAtStart
            && m->healCounter == 0 && m->hurtCounter == 0) {
            sPoisonHealth.drainProgress += poisonDrainAmount;
            if (sPoisonHealth.drainProgress >= WATER_HEALTH_WEDGE) {
                player_stats_health(m->health, m->health - WATER_HEALTH_WEDGE);
                m->health -= WATER_HEALTH_WEDGE;
                poisonDrainApplied = WATER_HEALTH_WEDGE;
                sPoisonHealth.drainProgress -= WATER_HEALTH_WEDGE;
            }
        }

        // Keep overflow healing queued until a pending whole-wedge debit makes room.
        // Healing on the debit update still precedes damage, allowing a living rescue.
        if (m->healCounter > 0
            && (m->hurtCounter == 0 || m->hurtCounter % 4 == 1 || m->health <= 0x840)) {
            if (m->respawnHealCounter != 0) {
                player_stats_health_non_restoring(m->health, m->health + 0x40);
                m->respawnHealCounter--;
            } else {
                player_stats_health(m->health, m->health + 0x40);
            }
            m->health += 0x40;
            m->healCounter--;
        }
        if (m->hurtCounter > 0) {
            m->hurtCounter--;
            // Keep the four-update drain cadence; each completed interval costs one wedge.
            if (m->hurtCounter % 4 == 0) {
                player_stats_health(m->health, m->health - 0x100);
                m->health -= 0x100;
            }
        }

        if (m->health > 0x880) {
            m->health = 0x880;
        }
        if (m->health < 0x100) {
            m->health = 0xFF;
        }
    }

    damageObserved |= m->health < healthAtStart;
    if (shallowExposure) {
        if (sShallowRegenExposureFrames < HEALTH_REGEN_SHALLOW_RESET_FRAMES) {
            sShallowRegenExposureFrames++;
        }
    } else {
        sShallowRegenExposureFrames = 0;
    }
    // Actual or equivalent sleep resets only automatic healing, preserving hazard drain.
    if (!game_options_get(GO_PASSIVE_HEAL) || damageObserved || m->health < 0x100 || deepExposure
        || poisonExposure || sleepDue || m->action == ACT_BURNING_GROUND
        || m->action == ACT_BURNING_JUMP || m->action == ACT_BURNING_FALL
        || sShallowRegenExposureFrames >= HEALTH_REGEN_SHALLOW_RESET_FRAMES) {
        sHealthRegenFrames = 0;
    } else if (m->health < 0x880 && !(m->action & ACT_FLAG_INTANGIBLE)
               && (!(m->input & INPUT_IN_POISON_GAS) || (m->flags & MARIO_METAL_CAP)
                   || gDebugLevelSelect)
               && !(inWater && (terrainIsSnow || headUnderwater)
                    && !mario_has_water_damage_protection(m))) {
        sHealthRegenFrames++;
        if (sHealthRegenFrames == HEALTH_REGEN_START_FRAMES
            || (sHealthRegenFrames > HEALTH_REGEN_START_FRAMES
                && (sHealthRegenFrames - HEALTH_REGEN_START_FRAMES) % HEALTH_REGEN_WEDGE_FRAMES == 0)) {
            player_stats_health(m->health, m->health + WATER_HEALTH_WEDGE);
            m->health += WATER_HEALTH_WEDGE;
            if (m->health > 0x880) {
                m->health = 0x880;
            }
        }
    }

    if (m->health < 0x100) {
        // Death discards healing collected or queued before the fatal update.
        m->healCounter = 0;
    }
    if (m->healCounter == 0) {
        m->respawnHealCounter = 0;
    }
    finish_mario_health_update(m, healthAtStart, waterDrainApplied, poisonDrainApplied,
                               lowPoseUnderwater);
    if (mario_health_after_pending_damage(m) >= 0x100) {
        m->yoshiCoinDeathPending = FALSE;
    }
}

#ifdef BREATH_METER
void update_mario_breath(struct MarioState *m) {
    if (game_options_cheat_active(GO_INFINITE_HEALTH)) {
        m->breath = 0x880;
        m->breathCounter = 0;
        return;
    }
    if (m->breath >= 0x100 && m->health >= 0x100) {
        if (mario_is_head_underwater(m) && !mario_has_water_damage_protection(m)
            && !(m->action & ACT_FLAG_INTANGIBLE)) {
            m->breath--;
            if (m->breath < 0x300) {
                // Play a noise to alert the player when Mario is close to drowning.
                play_sound(SOUND_MOVING_ALMOST_DROWNING, gGlobalSoundSource);
#ifdef ENABLE_RUMBLE
                if (gRumblePakTimer == 0) {
                    gRumblePakTimer = 36;
                    if (is_rumble_finished_and_queue_empty()) {
                        queue_rumble_data(3, 30);
                    }
                }
            } else {
                gRumblePakTimer = 0;
#endif
            }
        } else if (!(m->input & INPUT_IN_POISON_GAS)) {
            m->breath += 0x1A;
        }
        if (m->breathCounter > 0) {
            m->breath += 0x40;
            m->breathCounter--;
        }
        if (m->breath > 0x880) m->breath = 0x880;
        if (m->breath < 0x100) {
            // If breath is "zero", set health to "zero"
            m->breath =  0xFF;
            player_stats_health(m->health, 0xFF);
            m->health =  0xFF;
        }
    }
}
#endif

/**
 * Updates some basic info for camera usage.
 */
void update_mario_info_for_cam(struct MarioState *m) {
    m->marioBodyState->action = m->action;
    m->statusForCamera->action = m->action;

    vec3s_copy(m->statusForCamera->faceAngle, m->faceAngle);

    if (!(m->flags & MARIO_LEDGE_CLIMB_CAMERA)) {
        vec3f_copy(m->statusForCamera->pos, m->pos);
    }
}

/**
 * Resets Mario's model, done every time an action is executed.
 */
void mario_reset_bodystate(struct MarioState *m) {
    struct MarioBodyState *bodyState = m->marioBodyState;

    bodyState->capState = MARIO_HAS_DEFAULT_CAP_OFF;
    bodyState->eyeState = MARIO_EYES_BLINK;
    bodyState->handState = MARIO_HAND_FISTS;
    bodyState->modelState = 0;
    bodyState->wingFlutter = FALSE;
    bodyState->capBlinkBlend = MARIO_CAP_BLINK_NONE;
}

/**
 * Adjusts Mario's graphical height for quicksand.
 */
void sink_mario_in_quicksand(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;
    if (!mario_is_on_floor(m) && m->action != ACT_QUICKSAND_DEATH) {
        return;
    }

    if (marioObj->header.gfx.throwMatrix) {
        (*marioObj->header.gfx.throwMatrix)[3][1] -= m->quicksandDepth;
    }

    marioObj->header.gfx.pos[1] -= m->quicksandDepth;
}

#define CAP_FLICKER_FRAMES (5 * 30)
#define CAP_FLICKER_PHASES 63

// Stretch the existing slow-to-fast warning pattern across the final five seconds.
u64 sCapFlickerFrames = 0b100010001000100010001001001001001001001001001010101010101010101;

static s32 mario_cap_flicker_is_dim(u16 timer) {
    u32 phase = timer * CAP_FLICKER_PHASES / CAP_FLICKER_FRAMES;
    return !game_options_get(GO_FLASHING) && timer < CAP_FLICKER_FRAMES
           && ((1ULL << phase) & sCapFlickerFrames) != 0;
}

/**
 * Updates the cap flags mainly based on the cap timer.
 */
u32 update_and_return_cap_flags(struct MarioState *m) {
    // Free movement is in the cutscene group, but never owns a cap-in-hand pose.
    if ((m->flags & MARIO_CAP_IN_HAND)
        && (m->action == ACT_DEBUG_FREE_MOVE
            || (gDebugLevelSelect && (m->action & ACT_GROUP_MASK) != ACT_GROUP_CUTSCENE
                && m->action != ACT_PUTTING_ON_CAP_AIR))) {
        m->flags &= ~MARIO_CAP_IN_HAND;
        m->flags |= MARIO_CAP_ON_HEAD;
    }
    u32 flags = m->flags;
    u32 action;

    if (m->capTimer > 0) {
        action = m->action;

        if ((m->capTimer <= 60)
            || ((action != ACT_READING_AUTOMATIC_DIALOG) && (action != ACT_READING_NPC_DIALOG)
                && (action != ACT_READING_SIGN) && (action != ACT_IN_CANNON))) {
            m->capTimer -= 1;
        }

        if (m->capTimer == 0) {
            stop_cap_music();

            mario_clear_cap_flags(m);
            if (!(m->flags & MARIO_CAPS)) {
                m->flags &= ~MARIO_CAP_ON_HEAD;
            }
            flags = m->flags;
        }

        if (m->capTimer == 60) {
            fadeout_cap_music();
        }

        if (!game_options_get(GO_FLASHING) && m->capTimer < CAP_FLICKER_FRAMES) {
            u32 flickerPhase = m->capTimer * CAP_FLICKER_PHASES / CAP_FLICKER_FRAMES;
            if ((1ULL << flickerPhase) & sCapFlickerFrames) {
                flags &= ~(MARIO_SPECIAL_CAPS & ~m->forcedCapFlags);
                if (!(flags & MARIO_CAPS)) {
                    flags &= ~MARIO_CAP_ON_HEAD;
                }
            }
        }
    }

    return flags;
}

/**
 * Composes cap appearance with warp and damage-blink opacity.
 */
static void mario_set_cap_render_state(struct MarioState *m, u32 flags) {
    struct MarioBodyState *bodyState = m->marioBodyState;

    if (flags & MARIO_VANISH_CAP) {
        bodyState->modelState = MODEL_STATE_NOISE_ALPHA;
    }

    if (flags & MARIO_METAL_CAP) {
        bodyState->modelState |= MODEL_STATE_METAL;
    }

    if ((flags & MARIO_CAP_IN_HAND) && m->heldObj == NULL) {
        if (flags & MARIO_WING_CAP) {
            bodyState->handState = MARIO_HAND_HOLDING_WING_CAP;
        } else {
            bodyState->handState = MARIO_HAND_HOLDING_CAP;
        }
    }

    if (flags & MARIO_CAP_ON_HEAD) {
        if (flags & MARIO_WING_CAP) {
            bodyState->capState = MARIO_HAS_WING_CAP_ON;
        } else {
            bodyState->capState = MARIO_HAS_DEFAULT_CAP_ON;
        }
    }

    if ((m->flags & MARIO_TELEPORTING) && (m->fadeWarpOpacity != MODEL_STATE_MASK)) {
        bodyState->modelState &= ~MODEL_STATE_MASK;
        bodyState->modelState |= (MODEL_STATE_ALPHA | m->fadeWarpOpacity);
    }

    // Reduced flashing keeps cap geometry stable and warns at steady half opacity.
    // Normal wing expiry warns with the whole body, not just disappearing wings.
    s32 steadyWarning =
        game_options_get(GO_FLASHING) && (m->flags & MARIO_SPECIAL_CAPS & ~m->forcedCapFlags);
    if (m->capTimer > 0 && m->capTimer < CAP_FLICKER_FRAMES
        && (steadyWarning || ((m->flags & MARIO_WING_CAP) && !(flags & MARIO_WING_CAP)))) {
        s32 alpha = (bodyState->modelState & MODEL_STATE_ALPHA)
                        ? bodyState->modelState & MODEL_STATE_MASK
                        : MODEL_STATE_MASK;
        bodyState->modelState = (bodyState->modelState & ~MODEL_STATE_MASK) | MODEL_STATE_ALPHA
                                | MIN(alpha, steadyWarning ? 128 : 64);
    }

    // Keep the damage blink visible at 25%, without brightening a darker warp fade.
    if ((m->invincTimer >= 3) && (game_options_get(GO_FLASHING) || gGlobalTimer % 5 >= 3)) {
        s32 alpha = (bodyState->modelState & MODEL_STATE_ALPHA)
                        ? (bodyState->modelState & MODEL_STATE_MASK)
                        : MODEL_STATE_MASK;
        bodyState->modelState = (bodyState->modelState & ~MODEL_STATE_MASK) | MODEL_STATE_ALPHA
                                | MIN(alpha, 64); // 64/255 is approximately 25%.
    }
}

/** Updates the hitbox once and blends each warning edge for one gameplay update. */
void mario_update_hitbox_and_cap_model(struct MarioState *m) {
    struct MarioBodyState *bodyState = m->marioBodyState;
    u16 previousTimer = m->capTimer;
    u32 flags = update_and_return_cap_flags(m);
    s32 blend = (m->flags & MARIO_SPECIAL_CAPS & ~m->forcedCapFlags) && m->capTimer != previousTimer
                && previousTimer <= CAP_FLICKER_FRAMES
                && mario_cap_flicker_is_dim(previousTimer) != mario_cap_flicker_is_dim(m->capTimer);

    bodyState->capBlinkBlend = MARIO_CAP_BLINK_NONE;
    if (blend) {
        u32 ordinaryFlags = m->flags & ~(MARIO_SPECIAL_CAPS & ~m->forcedCapFlags);
        s16 initialModel = bodyState->modelState;
        s8 initialCap = bodyState->capState;
        s8 initialHand = bodyState->handState;
        if (!(ordinaryFlags & MARIO_CAPS)) {
            ordinaryFlags &= ~MARIO_CAP_ON_HEAD;
        }
        mario_set_cap_render_state(m, ordinaryFlags);
        bodyState->capBlinkModelState = bodyState->modelState;
        bodyState->capBlinkCapState = bodyState->capState;
        bodyState->capBlinkHandState = bodyState->handState;
        bodyState->modelState = initialModel;
        bodyState->capState = initialCap;
        bodyState->handState = initialHand;
        mario_set_cap_render_state(m, m->flags);
        bodyState->capBlinkBlend = MARIO_CAP_BLINK_GEOMETRY;

        // Vanish alone changes only opacity; one interpolated draw suffices.
        if (bodyState->capState == bodyState->capBlinkCapState
            && bodyState->handState == bodyState->capBlinkHandState
            && !((bodyState->modelState ^ bodyState->capBlinkModelState) & MODEL_STATE_METAL)) {
            bodyState->capBlinkBlend = MARIO_CAP_BLINK_OPACITY;
        }
    } else {
        mario_set_cap_render_state(m, flags);
    }

    // Short hitbox for crouching/crawling/etc.
    m->marioObj->hitboxHeight = (m->action & ACT_FLAG_SHORT_HITBOX) ? 100.0f : 160.0f;
}

/**
 * An unused and possibly a debug function. Z + another button input
 * sets Mario with a different cap.
 */
UNUSED static void debug_update_mario_cap(u16 button, s32 flags, u16 capTimer, u16 capMusic) {
    // This checks for Z_TRIG instead of Z_DOWN flag
    // (which is also what other debug functions do),
    // so likely debug behavior rather than unused behavior.
    if ((gPlayer1Controller->buttonDown & Z_TRIG) && (gPlayer1Controller->buttonPressed & button)
        && !(gMarioState->flags & flags)) {
        mario_add_cap_power(gMarioState, flags | MARIO_CAP_ON_HEAD);

        if (capTimer > gMarioState->capTimer) {
            gMarioState->capTimer = capTimer;
        }

        play_cap_music(capMusic);
    }
}

#ifdef ENABLE_RUMBLE
static void queue_rumble_particles(struct MarioState *m) {
    if (m->particleFlags & PARTICLE_HORIZONTAL_STAR) {
        queue_rumble_data(5, 80);
    } else if (m->particleFlags & PARTICLE_VERTICAL_STAR) {
        queue_rumble_data(5, 80);
    } else if (m->particleFlags & PARTICLE_TRIANGLE) {
        queue_rumble_data(5, 80);
    }
    if (m->heldObj && m->heldObj->behavior == segmented_to_virtual(bhvBobomb)) {
        reset_rumble_timers_slip();
    }
}
#else
#define queue_rumble_particles(m)
#endif

s32 mario_has_rumble_contact(struct MarioState *m) {
    if (mario_is_on_floor(m) || (m->action & (ACT_FLAG_ON_POLE | ACT_FLAG_HANGING))) {
        return TRUE;
    }
    // These actions own a real attachment; interactObj can be stale in free air.
    switch (m->action) {
        case ACT_RIDING_HOOT:
        case ACT_GRABBED:
        case ACT_IN_CANNON:
        case ACT_TORNADO_TWIRLING:
        case ACT_CAUGHT_IN_WHIRLPOOL:
        case ACT_LEDGE_GRAB:
        case ACT_LEDGE_CLIMB_SLOW_1:
        case ACT_LEDGE_CLIMB_SLOW_2:
        case ACT_LEDGE_CLIMB_DOWN:
        case ACT_LEDGE_CLIMB_FAST:
            return TRUE;
    }
    return m->heldObj != NULL && m->heldObj->behavior == segmented_to_virtual(bhvBobomb);
}

void mario_he_thicc_contact_shake(UNUSED struct MarioState *m) {
    if (mario_character_palette() == MARIO_PALETTE_HE_THICC) {
        set_camera_shake_from_hit(SHAKE_HE_THICC_CONTACT);
    }
}

static void mario_update_character_rumble(struct MarioState *m) {
    s32 active = mario_character_palette() == MARIO_PALETTE_HE_THICC && gCurrDemoInput == NULL
                 && gCurrCreditsEntry == NULL && m->action != ACT_UNINITIALIZED
                 && !(m->action & ACT_FLAG_IDLE) && (m->action & ACT_GROUP_MASK) != ACT_GROUP_CUTSCENE;
    s32 level = CHARACTER_RUMBLE_LIGHT;

    // Shared defaults cover equivalent actions, including held-object variants.
    if (m->action & ACT_FLAG_SWIMMING) {
        level = CHARACTER_RUMBLE_MEDIUM_LIGHT;
    }
    if (m->action & ACT_FLAG_ATTACKING) {
        level = CHARACTER_RUMBLE_MEDIUM;
    }
    if (m->action & ACT_FLAG_BUTT_OR_STOMACH_SLIDE) {
        level = CHARACTER_RUMBLE_STRONG;
    }
    if (m->action & ACT_FLAG_AIR) {
        level = m->vel[1] > 0.0f ? CHARACTER_RUMBLE_MEDIUM : CHARACTER_RUMBLE_FAINT;
        if (m->action & ACT_FLAG_ATTACKING) {
            level = CHARACTER_RUMBLE_STRONG;
        }
    }

    switch (m->action) {
        case ACT_SLEEPING:
        case ACT_SHIVERING:
        case ACT_HOLD_IDLE:
        case ACT_HOLD_HEAVY_IDLE:
        case ACT_HOLD_PANTING_UNUSED:
        case ACT_FIRST_PERSON:
        case ACT_WATER_IDLE:
        case ACT_HOLD_WATER_IDLE:
        case ACT_METAL_WATER_STANDING:
        case ACT_HOLD_METAL_WATER_STANDING:
        case ACT_HOLDING_POLE:
        case ACT_TOP_OF_POLE:
        case ACT_HANGING:
        case ACT_LEDGE_GRAB:
            level = CHARACTER_RUMBLE_NONE;
            break;
        case ACT_CROUCHING:
            level = CHARACTER_RUMBLE_FAINT;
            break;
        case ACT_DAB:
            // Entering the pose has a beat; holding, settling and recovery soften it.
            level = m->actionState == 0 ? CHARACTER_RUMBLE_MEDIUM : CHARACTER_RUMBLE_FAINT;
            break;
        case ACT_GROUND_TWIRL:
            level =
                mario_ground_twirl_is_held_pose(m) ? CHARACTER_RUMBLE_FAINT : CHARACTER_RUMBLE_STRONG;
            break;
        case ACT_WALKING:
        case ACT_HOLD_WALKING:
        case ACT_HOLD_HEAVY_WALKING:
        case ACT_TURNING_AROUND:
        case ACT_FINISH_TURNING_AROUND:
        case ACT_BRAKING:
        case ACT_BRAKING_STOP:
        case ACT_DECELERATING:
        case ACT_HOLD_DECELERATING:
        case ACT_METAL_WATER_WALKING:
        case ACT_HOLD_METAL_WATER_WALKING:
            level = (m->forwardVel >= 24.0f || m->forwardVel <= -24.0f) ? CHARACTER_RUMBLE_MEDIUM_LIGHT
                                                                        : CHARACTER_RUMBLE_LIGHT;
            break;
        case ACT_DIVE:
        case ACT_SLIDE_KICK:
        case ACT_HE_THICC_WAVEDASH:
        case ACT_HEAVY_THROW:
            level = CHARACTER_RUMBLE_STRONG;
            break;
        case ACT_GROUND_POUND:
            level = m->actionState == 0 ? CHARACTER_RUMBLE_MEDIUM : CHARACTER_RUMBLE_VERY_STRONG;
            break;
        case ACT_JUMP_LAND:
        case ACT_DOUBLE_JUMP_LAND:
        case ACT_TRIPLE_JUMP_LAND:
        case ACT_BACKFLIP_LAND:
        case ACT_SIDE_FLIP_LAND:
        case ACT_LONG_JUMP_LAND:
        case ACT_FREEFALL_LAND:
        case ACT_HOLD_JUMP_LAND:
        case ACT_HOLD_FREEFALL_LAND:
            level = CHARACTER_RUMBLE_VERY_STRONG;
            break;
        case ACT_GROUND_POUND_LAND:
        case ACT_GROUND_BONK:
        case ACT_HARD_BACKWARD_GROUND_KB:
        case ACT_HARD_FORWARD_GROUND_KB:
            level = CHARACTER_RUMBLE_HEAVY;
            break;
    }
    set_character_movement_rumble(active ? level : CHARACTER_RUMBLE_NONE);
}

/**
 * The upside-down handstand pose faces opposite Mario's logical yaw. Compensate
 * visually while attached to the pole. Takeoff transfers this offset to his yaw.
 */
static void update_pole_handstand_facing(struct MarioState *m) {
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    s32 yawOffset = 0x8000;
    s32 frame;
    s32 lastFrame;

    if (m->action != ACT_TOP_OF_POLE && m->action != ACT_TOP_OF_POLE_TRANSITION) {
        return;
    }

    switch (animInfo->animID) {
        case MARIO_ANIM_START_HANDSTAND:
        case MARIO_ANIM_RETURN_FROM_HANDSTAND:
            // Match the frame that will be rendered, including reverse playback.
            frame = geo_update_animation_frame(animInfo, NULL);
            lastFrame = animInfo->curAnim->loopEnd - 1;
            if (lastFrame <= 0) {
                return;
            }
            yawOffset = 0x8000 * frame / lastFrame;
            break;
        case MARIO_ANIM_HANDSTAND_IDLE:
            break;
        default:
            return;
    }

    m->marioObj->header.gfx.angle[1] = m->faceAngle[1] + yawOffset;
}

/**
 * Main function for executing Mario's behavior. Returns particleFlags.
 */
s32 mario_is_knockback_recovery(u32 action) {
    return mario_is_knocked_back(action) || action == ACT_SOFT_BONK || action == ACT_GROUND_BONK
           || action == ACT_THROWN_FORWARD || action == ACT_THROWN_BACKWARD;
}

static void update_bully_knockback_buffer(struct MarioState *m) {
    if (mario_is_knockback_recovery(m->action)) {
        m->bullyKnockbackBuffer |= m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED);
        if (m->framesSinceA >= 6) {
            m->bullyKnockbackBuffer &= ~INPUT_A_PRESSED;
        }
        if (m->framesSinceB >= 6) {
            m->bullyKnockbackBuffer &= ~INPUT_B_PRESSED;
        }
    }
}

static void mario_tick_cap_pickup_lock(struct MarioState *m) {
    if (m->capPickupLockFrames != 0 && m->capPickupLockStartFrame != gGlobalTimer) {
        m->capPickupLockFrames--;
    }
}

// Keep physics and forced reactions running, but discard fresh and buffered voluntary inputs.
static void mario_block_actions_at_zero_health(struct MarioState *m,
                                               struct Controller *neutralController) {
    if (m->health >= 0x100) {
        return;
    }
    // Only Mario sees neutral controls; the real controller remains available to pause and menus.
    m->controller = neutralController;
    m->input &= ~(INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_PRESSED | INPUT_A_DOWN | INPUT_B_DOWN
                  | INPUT_Z_DOWN | INPUT_NONZERO_ANALOG | INPUT_FIRST_PERSON);
    m->input |= INPUT_IDLE;
    m->intendedMag = 0.0f;
    m->intendedYaw = m->faceAngle[1];
    m->framesSinceA = 0xFF;
    m->framesSinceB = 0xFF;
    m->zHeldFrames = 0;
    m->zCrouchBuffered = FALSE;
    m->zBPressed = FALSE;
    m->crouchSlideBackflip = FALSE;
    m->backwardLongJumpChain = FALSE;
    m->backwardLongJumpCrouchFrames = 0;
    m->objectRecoveryBFrames = 0;
    m->objectRecoveryAFrames = 0;
    m->automaticZFrames = 0;
    m->automaticBFrames = 0;
    m->automaticAFrames = 0;
    m->airTwirlInputFrames = 0;
    m->groundPoundInputFrames = 0;
    m->landingJumpBufferedEdge = FALSE;
    m->landingJumpInputFrames = 0;
    m->wallKickInputFrames = 0;
    m->wallKickForwardStickFrames = 0;
    m->wallKickBackwardStickFrames = 0;
    m->burnCoinJumpPending = FALSE;
    m->bullyKnockbackBuffer = 0;
    mario_ground_twirl_cancel_queued_input();
}

// Clear constraints before inputs can turn them into another environmental action.
static void mario_update_environment_cheats(struct MarioState *m) {
    if (game_options_cheat_active(GO_KNOCKBACK)
        && (mario_is_knocked_back(m->action) || m->action == ACT_SHOCKED
            || m->action == ACT_WATER_SHOCKED || m->action == ACT_SHOCKWAVE_BOUNCE
            || m->action == ACT_GETTING_BLOWN || m->action == ACT_TORNADO_TWIRLING
            || m->action == ACT_THROWN_FORWARD || m->action == ACT_THROWN_BACKWARD
            || m->action == ACT_GRABBED || m->action == ACT_BURNING_GROUND
            || m->action == ACT_BURNING_JUMP || m->action == ACT_BURNING_FALL
            || (m->action == ACT_LAVA_BOOST && m->actionArg != ACT_ARG_LAVA_BOOST_METAL_GROUND_POUND)
            || m->action == ACT_LAVA_BOOST_LAND)) {
        f32 peakHeight = m->peakHeight;
        if (m->action == ACT_GRABBED && m->usedObj != NULL) {
            // Captors move Mario's held pose; release from that current location.
            vec3f_copy(m->pos, m->marioObj->header.gfx.pos);
            struct Object *owner = m->usedObj;
            owner->oInteractStatus &= ~INT_STATUS_GRABBED_MARIO;
            if (owner->behavior == segmented_to_virtual(bhvHeaveHo)) {
                owner->oHeaveHoThrowState = 0;
                owner->oAction = 1;
            } else if (owner->behavior == segmented_to_virtual(bhvChuckya)) {
                owner->oCommonAnchorAction = 0;
                owner->oAction = 3;
            } else if (owner->behavior == segmented_to_virtual(bhvKingBobomb)) {
                owner->oCommonAnchorAction = 0;
                owner->oAction = KING_BOBOMB_ACT_ACTIVE;
                owner->oKingBobombPlayerGrabEscapeCooldown = 35;
            }
            m->usedObj = NULL;
            m->marioObj->oInteractStatus &=
                ~(INT_STATUS_MARIO_THROWN_BY_OBJ | INT_STATUS_MARIO_DROPPED_BY_OBJ);
        }
        u32 action = (m->action & ACT_FLAG_SWIMMING) ? ACT_WATER_IDLE
                     : mario_is_on_floor(m)          ? ACT_IDLE
                                                     : ACT_FREEFALL;
        if (m->action == ACT_BURNING_GROUND || m->action == ACT_BURNING_JUMP
            || m->action == ACT_BURNING_FALL) {
            mario_extinguish_fire(m);
        }
        if (m->action == ACT_TORNADO_TWIRLING || m->action == ACT_GETTING_BLOWN) {
            m->usedObj = NULL;
            m->angleVel[1] = 0;
        }
        m->forwardVel = 0.0f;
        vec3_zero(m->vel);
        m->slideVelX = m->slideVelZ = 0.0f;
        m->invincTimer = MAX(m->invincTimer, 30);
        set_mario_action(m, action, 0);
        if (action & ACT_FLAG_AIR) {
            m->peakHeight = peakHeight;
        }
    }

    if (game_options_cheat_active(GO_IGNORE_SAND)) {
        m->quicksandDepth = 0.0f;
        if (m->area != NULL && (m->area->terrainType & TERRAIN_MASK) == TERRAIN_SAND
            && (m->action == ACT_HEAD_STUCK_IN_GROUND || m->action == ACT_BUTT_STUCK_IN_GROUND
                || m->action == ACT_FEET_STUCK_IN_GROUND)) {
            set_mario_action(m, ACT_IDLE, 0);
        }
        // Shallow-water and unlock deaths share this action but are not sand.
        if (m->action == ACT_QUICKSAND_DEATH && m->actionArg == 0 && m->health >= 0x100) {
            m->vel[1] = 0.0f;
            m->peakHeight = m->pos[1];
            set_mario_action(m, ACT_FREEFALL, 0);
        }
    }
    if (game_options_cheat_active(GO_IGNORE_WIND)) {
        m->windCatchActive = FALSE;
        m->windColumnActive = FALSE;
        m->windDeathGraceTimer = 0;
        if (m->action == ACT_VERTICAL_WIND || m->action == ACT_GETTING_BLOWN
            || m->action == ACT_TORNADO_TWIRLING) {
            m->usedObj = NULL;
            m->angleVel[1] = 0;
            m->windGravity = 0.0f;
            set_mario_action(m, ACT_FREEFALL, 0);
        }
    }
    if (game_options_cheat_active(GO_IGNORE_CURRENTS) && m->action == ACT_CAUGHT_IN_WHIRLPOOL) {
        m->usedObj = NULL;
        m->vel[1] = 0.0f;
        set_mario_action(m, mario_has_land_movement_underwater(m) ? ACT_FREEFALL : ACT_WATER_IDLE, 0);
    }
}

// Options can change during pause. Retire active/queued mechanics before interactions.
static void mario_update_gameplay_options(struct MarioState *m) {
    if (game_options_get(GO_METAL_CAP) == 1) {
        m->metalFallGroundPoundImpact = FALSE;
        m->metalFallGroundPoundFloorObject = NULL;
        m->fastMetalFallSquish = FALSE;
    }
    s32 endGroundPose =
        (!game_options_get(GO_TWIRLS) && (m->action == ACT_GROUND_TWIRL || m->action == ACT_TWIRL_LAND))
        || (!game_options_get(GO_DAB) && m->action == ACT_DAB)
        || (!game_options_get(GO_DIZZINESS) && m->action == ACT_SPIN_DIZZY);
    if (!game_options_get(GO_TWIRLS)) {
        s32 endAirTwirl = m->action == ACT_L_AIR_TWIRL || m->action == ACT_TWIRLING;
        if (endAirTwirl) {
            f32 peakHeight = m->peakHeight;
            set_mario_action(m, ACT_FREEFALL, 0);
            m->peakHeight = peakHeight;
        }
        if (m->airTwirlFlightRollFrame != 0 || endGroundPose || endAirTwirl) {
            m->marioObj->header.gfx.angle[0] = -m->faceAngle[0];
            m->marioObj->header.gfx.angle[1] = m->faceAngle[1];
            m->marioObj->header.gfx.angle[2] = m->faceAngle[2];
        }
        mario_air_twirl_init(m);
        mario_ground_twirl_init();
    }
    if (!game_options_get(GO_DAB)) {
        mario_dab_reset();
    }
    if (!game_options_get(GO_DIZZINESS)) {
        mario_spin_dizzy_reset();
    }
    if (endGroundPose) {
        f32 speed = m->forwardVel;
        u32 action = (m->input & INPUT_OFF_FLOOR)        ? ACT_FREEFALL
                     : (m->input & INPUT_NONZERO_ANALOG) ? ACT_WALKING
                                                         : ACT_DECELERATING;
        set_mario_action(m, action, 0);
        mario_set_forward_vel(m, speed);
    }
}

s32 execute_mario_action(UNUSED struct Object *obj) {
    s32 inLoop = TRUE;
    struct Controller *controller = gMarioState->controller;
    struct Controller neutralController = { 0 };

    player_stats_mario_begin(gMarioState);

    // Updates once per frame:
    jrb_simpleflips_confine_mario(gMarioState);
    mario_tick_cap_pickup_lock(gMarioState);
    vec3f_get_dist_and_angle(gMarioState->prevPos, gMarioState->pos, &gMarioState->moveSpeed, &gMarioState->movePitch, &gMarioState->moveYaw);
    vec3f_get_lateral_dist(gMarioState->prevPos, gMarioState->pos, &gMarioState->lateralSpeed);
    vec3f_copy(gMarioState->prevPos, gMarioState->pos);

    if (gMarioState->action) {
        mario_update_infinite_health(gMarioState);
        mario_block_actions_at_zero_health(gMarioState, &neutralController);
#ifdef ENABLE_DEBUG_FREE_MOVE
        if (
#ifdef PUPPYPRINT_DEBUG
            !sDebugMenu &&
#endif // PUPPYPRINT_DEBUG
            game_options_free_move_active() && !jrb_simpleflips_room_active()
            && (gMarioState->action != ACT_DEBUG_FREE_MOVE)
            && (gMarioState->controller->buttonDown & U_JPAD)
            && !(gMarioState->controller->buttonDown & L_TRIG)) {
            if (gMarioState->area->camera->mode != CAMERA_MODE_8_DIRECTIONS) {
                set_camera_mode(gMarioState->area->camera, CAMERA_MODE_8_DIRECTIONS, 1);
            }
            set_mario_action(gMarioState, ACT_DEBUG_FREE_MOVE, 0);
        }
#endif // ENABLE_DEBUG_FREE_MOVE
#ifdef ENABLE_CREDITS_BENCHMARK
        static s32 startedBenchmark = FALSE;
        if (!startedBenchmark) {
            set_mario_action(gMarioState, ACT_IDLE, 0);
            level_trigger_warp(gMarioState, WARP_OP_CREDITS_START);
            startedBenchmark = TRUE;
        }
#endif

        gMarioState->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
        mario_reset_bodystate(gMarioState);
        mario_update_forced_caps(gMarioState);
        mario_update_environment_cheats(gMarioState);
        update_mario_inputs(gMarioState);
        mario_update_gameplay_options(gMarioState);
        mario_update_wind_death_grace(gMarioState);
        update_bully_knockback_buffer(gMarioState);

        if (mario_should_drown_on_land(gMarioState)) {
            if (mario_has_shallow_water_death_pending(gMarioState)) {
                gMarioState->quicksandDepth = 0.0f;
                drop_and_set_mario_action(gMarioState, ACT_QUICKSAND_DEATH,
                                          ACT_ARG_QUICKSAND_DEATH_SHALLOW_WATER);
            } else {
                drop_and_set_mario_action(gMarioState, ACT_DROWNING, 0);
            }
        }

            mario_handle_special_floors(gMarioState);
        // Sample contact and shortcut state once, before action redispatch.
        if (mario_try_air_twirl(gMarioState)) {
            mario_ground_twirl_update_cooldown();
        } else {
            mario_try_ground_twirl(gMarioState);
        }
        mario_process_interactions(gMarioState);
        mario_block_actions_at_zero_health(gMarioState, &neutralController);
        // Floor impact is observed by later objects and next update's switches.
        // Retain it through this interaction pass, then clear it before movement.
        gMarioState->metalFallGroundPoundImpact = FALSE;
        gMarioState->metalFallGroundPoundFloorObject = NULL;
        gMarioState->waterGroundPoundImpact = FALSE;
        gMarioState->waterGroundPoundFloorObject = NULL;

        if (mario_dab_update_shortcut(gMarioState)) {
            mario_ground_twirl_cancel_queued_input();
            mario_spin_dizzy_reset();
        }
        mario_spin_dizzy_update(gMarioState);
        mario_swimming_update(gMarioState);

        // If Mario is OOB, stop executing actions.
        if (gMarioState->floor == NULL) {
            gMarioState->controller = controller;
            return ACTIVE_PARTICLE_NONE;
        }

        // The function can loop through many action shifts in one frame,
        // which can lead to unexpected sub-frame behavior. Could potentially hang
        // if a loop of actions were found, but there has not been a situation found.
        while (inLoop) {
            mario_block_actions_at_zero_health(gMarioState, &neutralController);
            mario_limit_velocity(gMarioState);
            switch (gMarioState->action & ACT_GROUP_MASK) {
                case ACT_GROUP_STATIONARY: inLoop = mario_execute_stationary_action(gMarioState); break;
                case ACT_GROUP_MOVING:     inLoop = mario_execute_moving_action(gMarioState);     break;
                case ACT_GROUP_AIRBORNE:   inLoop = mario_execute_airborne_action(gMarioState);   break;
                case ACT_GROUP_SUBMERGED:  inLoop = mario_execute_submerged_action(gMarioState);  break;
                case ACT_GROUP_CUTSCENE:   inLoop = mario_execute_cutscene_action(gMarioState);   break;
                case ACT_GROUP_AUTOMATIC:  inLoop = mario_execute_automatic_action(gMarioState);  break;
                case ACT_GROUP_OBJECT:     inLoop = mario_execute_object_action(gMarioState);     break;
            }
        }
        mario_limit_velocity(gMarioState);

        // Keep the entry fall safe through the landing check, then end protection.
        // A buffered jump can leave the floor again in this same update.
        if (gMarioState->spawnFallProtection == MARIO_SPAWN_FALL_PROTECTION_LANDED
            || (gMarioState->spawnFallProtection == MARIO_SPAWN_FALL_PROTECTION_ACTIVE
                && gMarioState->floor != NULL && gMarioState->pos[1] <= gMarioState->floorHeight)) {
            gMarioState->spawnFallProtection = MARIO_SPAWN_FALL_PROTECTION_NONE;
        }

        jrb_simpleflips_confine_mario(gMarioState);
        update_pole_handstand_facing(gMarioState);
        mario_transition_pose_update_gfx(gMarioState);
        sink_mario_in_quicksand(gMarioState);
        squish_mario_model(gMarioState);
        mario_he_thicc_update(gMarioState);
        mario_refresh_soaked_after_movement(gMarioState);
        mario_emit_soaked_drips(gMarioState);
        update_mario_submerged_effects(gMarioState);
        update_submerged_camera(gMarioState);
        if (!jrb_simpleflips_room_active() || jrb_simpleflips_is_game_over()) {
            s16 healthBefore = gMarioState->health;
            s32 capVulnerable = mario_pose_exposes_cap_to_damage(gMarioState);
            update_mario_health(gMarioState);
            if (capVulnerable && gMarioState->health < healthBefore) {
                mario_blow_off_cap(gMarioState, 15.0f);
            }
        }
#ifdef BREATH_METER
        update_mario_breath(gMarioState);
#endif
        update_mario_info_for_cam(gMarioState);
        mario_update_hitbox_and_cap_model(gMarioState);

        // Both of the wind handling portions play wind audio only in
        // non-Japanese releases.
        if (gMarioState->floor->type == SURFACE_HORIZONTAL_WIND) {
            spawn_wind_particles(0, (gMarioState->floor->force << 8));
            play_sound(SOUND_ENV_WIND2, gMarioState->marioObj->header.gfx.cameraToObject);
        }

        if (gMarioState->floor->type == SURFACE_VERTICAL_WIND) {
            spawn_wind_particles(1, 0);
            play_sound(SOUND_ENV_WIND2, gMarioState->marioObj->header.gfx.cameraToObject);
        }

        play_infinite_stairs_music();
        gMarioState->marioObj->oInteractStatus = INT_STATUS_NONE;
        queue_rumble_particles(gMarioState);
        mario_update_character_rumble(gMarioState);

        u32 actGroup = gMarioState->action & ACT_GROUP_MASK;
        if ((actGroup == ACT_GROUP_STATIONARY || actGroup == ACT_GROUP_MOVING)
            && gMarioState->floor != NULL && !SURFACE_IS_UNSAFE(gMarioState->floor->type)
            && !mario_floor_is_slippery(gMarioState)) {
            vec3f_copy(gMarioState->lastSafePos, gMarioState->pos);
        }

        player_stats_mario_end(gMarioState);
        gMarioState->controller = controller;
        return gMarioState->particleFlags;
    }

    return ACTIVE_PARTICLE_NONE;
}

/**************************************************
 *                  INITIALIZATION                *
 **************************************************/

static s32 mario_has_normal_cap_in_current_area(void) {
    struct Object *head = (struct Object *) &gObjectLists[OBJ_LIST_LEVEL];
    struct Object *obj = (struct Object *) head->header.next;

    for (; obj != head; obj = (struct Object *) obj->header.next) {
        if (obj->activeFlags != ACTIVE_FLAG_DEACTIVATED
            && obj->behavior == segmented_to_virtual(bhvNormalCap)
            && obj->header.gfx.activeAreaIndex == gCurrAreaIndex) {
            return TRUE;
        }
    }

    return FALSE;
}

void init_mario(void) {
    player_stats_level_entered();
    game_timing_runtime_entered();
    mario_transition_pose_reset();
    sSubmergedTrail.active = FALSE;
    mario_ground_twirl_reset_rebound_invulnerability();
    mario_reset_knockback_source();
    mario_ground_twirl_init();
    mario_air_twirl_init(gMarioState);
    mario_spin_dizzy_reset();
    mario_dab_reset();
    mario_he_thicc_reset();
    mario_swimming_reset();
    reset_water_health_and_breath();
    mario_reset_health_regeneration(gMarioState);
    gMarioState->actionTimer = 0;
    gMarioState->framesSinceA = 0xFF;
    gMarioState->framesSinceB = 0xFF;
    gMarioState->zHeldFrames = 0;
    gMarioState->wallKickChainCount = 0;
    gMarioState->wallKickFromDoubleJump = FALSE;
    gMarioState->wallBonkMomentum = FALSE;
    gMarioState->wallKickContactSoundPlayed = FALSE;
    gMarioState->rolloutWallKickFrames = 0;
    gMarioState->rolloutFrames = 0;
    gMarioState->waterWalkTwirl = FALSE;
    gMarioState->zCrouchBuffered = FALSE;
    gMarioState->zBPressed = FALSE;
    gMarioState->crouchSlideNeutral = FALSE;
    gMarioState->crouchSlideFrames = 0;
    gMarioState->crouchSlideBackflip = FALSE;
    gMarioState->backwardLongJumpChain = FALSE;
    gMarioState->backwardLongJumpCrouchFrames = 0;
    gMarioState->groundPoundBounceCooldown = 0;
    gMarioState->metalFallGroundPoundImpact = FALSE;
    gMarioState->metalFallGroundPoundFloorObject = NULL;
    gMarioState->waterGroundPoundImpact = FALSE;
    gMarioState->waterGroundPoundFloorObject = NULL;
    gMarioState->groundTakeoffThisFrame = FALSE;
    gMarioState->objectRecoveryBFrames = 0;
    gMarioState->objectRecoveryAFrames = 0;
    gMarioState->automaticZFrames = 0;
    gMarioState->automaticBFrames = 0;
    gMarioState->automaticAFrames = 0;
    gMarioState->airTwirlInputFrames = 0;
    gMarioState->groundPoundInputFrames = 0;
    gMarioState->landingJumpBufferedEdge = FALSE;
    gMarioState->landingJumpInputFrames = 0;
    gMarioState->wallKickInputFrames = 0;
    gMarioState->wallKickForwardStickFrames = 0;
    gMarioState->wallKickBackwardStickFrames = 0;
    gMarioState->burnCoinJumpPending = FALSE;
    gMarioState->spawnFallProtection = MARIO_SPAWN_FALL_PROTECTION_NONE;
    gMarioState->fastMetalFallSquish = FALSE;
    gMarioState->bullyKnockbackActive = FALSE;
    gMarioState->bullyKnockbackBuffer = 0;
    gMarioState->bullyKnockbackImmunityFrames = 0;
    gMarioState->heaveHoThrowPendingLanding = FALSE;
    gMarioState->heaveHoThrowRecoveryPending = FALSE;
    gMarioState->heaveHoLaunchImmunityFrames = 0;
    gMarioState->soakedTimer = 0;
    gMarioState->soakedThisUpdate = FALSE;
    gMarioState->soakedWaterContact = FALSE;
    gMarioState->waterWalkDisabled = FALSE;
    gMarioState->waterWalkCooldown = 0;
    gMarioState->soakedDripFrames = 0;
    gMarioState->soakedFlameProtection = FALSE;

    gMarioState->invincTimer = 0;

    if (save_file_get_flags()
        & (SAVE_FLAG_CAP_ON_GROUND | SAVE_FLAG_CAP_ON_KLEPTO | SAVE_FLAG_CAP_ON_UKIKI
           | SAVE_FLAG_CAP_ON_MR_BLIZZARD)) {
        gMarioState->flags = 0;
    } else {
        gMarioState->flags = (MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD);
    }

    gMarioState->forwardVel = 0.0f;
    gMarioState->squishTimer = 0;
    gMarioState->windCatchActive = FALSE;
    gMarioState->windColumnActive = FALSE;
    gMarioState->windDeathGraceTimer = 0;
    gMarioState->slopeJumpKickImpactSpeed = 0.0f;
    gMarioState->backwardSlopeSlideCount = 0;
    gMarioState->backwardButtSlideJumpCount = 0;

    gMarioState->hurtCounter = 0;
    gMarioState->healCounter = 0;
    gMarioState->respawnHealCounter = 0;
    gMarioState->yoshiCoinDeathPending = FALSE;
    gMarioState->poisonMushroomDeathPending = FALSE;

    gMarioState->capTimer = 0;
    gMarioState->forcedCapFlags = 0;
    gMarioState->naturalCapFlags = 0;
    gMarioState->quicksandDepth = 0.0f;

    gMarioState->heldObj = NULL;
    gMarioState->riddenObj = NULL;
    gMarioState->usedObj = NULL;

    gMarioState->waterLevel = find_water_level(gMarioSpawnInfo->startPos[0], gMarioSpawnInfo->startPos[2]);

    gMarioState->area = gCurrentArea;
    gMarioState->marioObj = gMarioObject;
    mario_character_commit(gMarioState, gCurrDemoInput != NULL || gCurrCreditsEntry != NULL);
    // Same-area respawns reuse the object hidden by ACT_DISAPPEARED.
    gMarioState->marioObj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    gMarioState->marioObj->header.gfx.animInfo.animID = -1;
    vec3s_copy(gMarioState->faceAngle, gMarioSpawnInfo->startAngle);
    vec3_zero(gMarioState->angleVel);
    vec3s_to_vec3f(gMarioState->pos, gMarioSpawnInfo->startPos);
    vec3f_copy(gMarioState->prevPos, gMarioState->pos);
    vec3_zero(gMarioState->vel);
    gMarioState->floorHeight =
        find_floor(gMarioState->pos[0], gMarioState->pos[1], gMarioState->pos[2], &gMarioState->floor);

    if (gMarioState->pos[1] < gMarioState->floorHeight) {
        gMarioState->pos[1] = gMarioState->floorHeight;
    }

    gMarioState->marioObj->header.gfx.pos[1] = gMarioState->pos[1];

    gMarioState->action =
        (gMarioState->pos[1] <= (gMarioState->waterLevel - 100)) ? ACT_WATER_IDLE : ACT_IDLE;

    mario_reset_bodystate(gMarioState);
    update_mario_info_for_cam(gMarioState);
    gMarioState->marioBodyState->punchState = 0;

    vec3f_copy(&gMarioState->marioObj->oPosVec, gMarioState->pos);
    vec3s_to_vec3i(&gMarioState->marioObj->oMoveAngleVec, gMarioState->faceAngle);

    vec3f_copy(gMarioState->marioObj->header.gfx.pos, gMarioState->pos);
    vec3s_set(gMarioState->marioObj->header.gfx.angle, 0, gMarioState->faceAngle[1], 0);

    Vec3s capPos;
    if (save_file_get_cap_pos(capPos) && !mario_has_normal_cap_in_current_area()) {
        struct Object *capObject = spawn_object(gMarioState->marioObj, MODEL_MARIOS_CAP, bhvNormalCap);
        vec3s_to_vec3f(&capObject->oPosVec, capPos);

        capObject->oForwardVel = 0;
        capObject->oMoveAngleYaw = 0;
    }
}

void init_mario_from_save_file(void) {
    gMarioState->movementProfile = &gMarioMovementProfiles[0];
    gMarioState->playerID = 0;
    gMarioState->flags = MARIO_NONE;
    gMarioState->capPickupLockFrames = 0;
    gMarioState->capPickupLockStartFrame = 0;
    gMarioState->action = ACT_UNINITIALIZED;
    gMarioState->spawnInfo = &gPlayerSpawnInfos[0];
    gMarioState->statusForCamera = &gPlayerCameraState[0];
    gMarioState->marioBodyState = &gBodyStates[0];
    gMarioState->controller = &gControllers[0];
    gMarioState->animList = &gMarioAnimsBuf;

    gMarioState->numCoins = 0;
    gMarioState->numStars = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);
    gMarioState->numKeys = 0;
#ifdef ENABLE_LIVES
    gMarioState->numLives = ENABLE_LIVES;
#else
    gMarioState->numLives = 0;
#endif
    gMarioState->health = 0x880;
#ifdef BREATH_METER
    gMarioState->breath = 0x880;
    gHudDisplay.breath = 8;
#endif
    gMarioState->prevNumStarsForDialog = gMarioState->numStars;
    gMarioState->animYTrans = 0xBD;

    gHudDisplay.coins = 0;
    gHudDisplay.wedges = 8;
}
