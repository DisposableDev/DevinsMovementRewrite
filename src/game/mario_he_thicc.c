#include "sm64.h"
#include "mario_he_thicc.h"
#include "mario_character.h"
#include "player_stats.h"
#include "mario.h"
#include "game_options_runtime.h"
#include "mario_dab.h"
#include "mario_actions_airborne.h"
#include "mario_actions_moving.h"
#include "mario_step.h"
#include "area.h"
#include "game_init.h"
#include "audio/external.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "level_update.h"
#include "memory.h"
#include "rumble_init.h"
#include "behavior_data.h"
#include "object_helpers.h"

#define HE_THICC_WAVEDASH_HEIGHT 160.0f
#define HE_THICC_WAVEDASH_SPEED 48.0f
#define HE_THICC_SLIDE_SPEED_LIMIT 100.0f
#define HE_THICC_BELLY_SPEED 56.0f
#define HE_THICC_BELLY_JUMP_SPEED 80.0f
#define HE_THICC_BELLY_JUMP_HEIGHT 48.0f
#define HE_THICC_POUND_BOUNCE_HEIGHT 84.0f
#define HE_THICC_CLAP_DISTANCE 120.0f
#define HE_THICC_RUNNING_MIN_SPEED 24.0f
#define HE_THICC_POSE_CHANNELS 63

// Independent of actionTimer: the spring survives move transitions, but never
// advances during render passes (mirror/silhouette draws share the same pose).
static struct {
    f32 jiggle;
    f32 jiggleVelocity;
    f32 lastVerticalSpeed;
    f32 clapDistance;
    Vec3f lastPos;
    u32 lastAction;
    u8 initialized;
    u8 clapFrames;
    s16 pose[HE_THICC_POSE_CHANNELS];
    u16 index[HE_THICC_POSE_CHANNELS * 2];
} sHeThicc;

static s32 he_thicc_is_active(void) {
    return mario_character_palette() == MARIO_PALETTE_HE_THICC && gCurrDemoInput == NULL
           && gCurrCreditsEntry == NULL;
}

static s32 he_thicc_has_safe_floor(struct MarioState *m) {
    return m->floor != NULL && !SURFACE_IS_UNSAFE(m->floor->type)
           && m->floor->type != SURFACE_DEATH_PLANE && m->floor->normal.y >= COS45;
}

static s32 he_thicc_can_wavedash(struct MarioState *m) {
    if (m->heldObj != NULL || m->riddenObj != NULL) {
        return FALSE;
    }
    switch (m->action) {
        case ACT_JUMP:
        case ACT_DOUBLE_JUMP:
        case ACT_TRIPLE_JUMP:
        case ACT_BACKFLIP:
        case ACT_SIDE_FLIP:
        case ACT_LONG_JUMP:
        case ACT_WALL_KICK_AIR:
        case ACT_FREEFALL:
        case ACT_DIVE:
        case ACT_FORWARD_ROLLOUT:
        case ACT_BACKWARD_ROLLOUT:
        case ACT_JUMP_KICK:
        case ACT_SLIDE_KICK:
        case ACT_STEEP_JUMP:
        case ACT_FLYING_TRIPLE_JUMP:
            return TRUE;
    }
    return FALSE;
}

void mario_he_thicc_reset(void) {
    sHeThicc.jiggle = 0.0f;
    sHeThicc.jiggleVelocity = 0.0f;
    sHeThicc.lastVerticalSpeed = 0.0f;
    sHeThicc.clapDistance = 0.0f;
    sHeThicc.lastAction = ACT_UNINITIALIZED;
    sHeThicc.initialized = FALSE;
    sHeThicc.clapFrames = 0;
}

void mario_he_thicc_clap(struct MarioState *m, f32 strength) {
    if (!he_thicc_is_active()) {
        return;
    }
    player_stats_he_thicc_event(PS_HE_THICC_CLAPS);
    sHeThicc.jiggleVelocity = MIN(0.45f, sHeThicc.jiggleVelocity + strength);
    sHeThicc.clapFrames = 6;
    play_sound(SOUND_HE_THICC_CLAP, m->marioObj->header.gfx.cameraToObject);
    if (strength >= 0.20f) {
        queue_rumble_data(3, 50);
    }
}

static void he_thicc_start_belly_slide(struct MarioState *m, f32 speed, s32 heavy) {
    // Project travel into a positive facing before choosing the new slide.
    if (m->forwardVel < 0.0f) {
        m->faceAngle[1] += 0x8000;
    }
    set_mario_action(m, ACT_HE_THICC_BELLY_SLIDE, heavy);
    mario_set_forward_vel(m, game_options_cheat_active(GO_NO_SPEED_LIMIT)
                                 ? speed
                                 : MIN(speed, HE_THICC_SLIDE_SPEED_LIMIT));
    m->slideYaw = m->faceAngle[1];
    m->faceAngle[0] = 0;
    m->vel[1] = 0.0f;
    set_mario_animation(m, MARIO_ANIM_SLIDE_DIVE);
    m->particleFlags |= PARTICLE_MIST_CIRCLE | PARTICLE_DUST | PARTICLE_HORIZONTAL_STAR;
    play_mario_heavy_landing_sound(m, SOUND_ACTION_TERRAIN_BODY_HIT_GROUND);
    mario_he_thicc_clap(m, heavy ? 0.40f : 0.25f);
    if (heavy) {
        player_stats_he_thicc_event(PS_HE_THICC_BELLY_SLAMS);
        mario_ground_pound_impact(m);
    }
    // Contact-frame input belongs to the new slide even when its airborne caller
    // completes without redispatching the grounded action this update.
    if (m->input & INPUT_A_PRESSED) {
        mario_he_thicc_belly_slide(m);
    }
}

s32 mario_he_thicc_try_belly_landing(struct MarioState *m) {
    if (!he_thicc_is_active() || m->action != ACT_DIVE || m->forwardVel <= 0.0f || m->heldObj != NULL
        || m->riddenObj != NULL || !he_thicc_has_safe_floor(m)
        || m->peakHeight - m->pos[1] < HE_THICC_BELLY_SLAM_HEIGHT || m->pos[1] > m->floorHeight + 4.0f
        || m->vel[1] > -HE_THICC_HARD_LANDING_SPEED
        || m->pos[1] < find_water_level(m->pos[0], m->pos[2]) - 10.0f) {
        return FALSE;
    }
    f32 speed = MAX(HE_THICC_BELLY_SPEED, absf(m->forwardVel) + -m->vel[1] * 0.25f);
    he_thicc_start_belly_slide(m, speed, TRUE);
    return TRUE;
}

s32 mario_he_thicc_try_wavedash(struct MarioState *m) {
    if (!he_thicc_is_active() || !he_thicc_can_wavedash(m) || !he_thicc_has_safe_floor(m)
        || m->vel[1] > 0.0f || m->pos[1] < m->floorHeight
        || m->pos[1] > m->floorHeight + HE_THICC_WAVEDASH_HEIGHT || m->pos[1] < m->waterLevel - 10.0f
        || (m->input & (INPUT_Z_PRESSED | INPUT_NONZERO_ANALOG))
               != (INPUT_Z_PRESSED | INPUT_NONZERO_ANALOG)
        || (m->input & (INPUT_A_PRESSED | INPUT_B_PRESSED)) || (m->controller->buttonDown & L_TRIG)) {
        return FALSE;
    }
    f32 speed = MAX(HE_THICC_WAVEDASH_SPEED, absf(m->forwardVel) + 16.0f);
    if (!game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        speed = MIN(speed, HE_THICC_SLIDE_SPEED_LIMIT);
    }
    m->faceAngle[1] = m->intendedYaw;
    set_mario_action(m, ACT_HE_THICC_WAVEDASH, 0);
    player_stats_he_thicc_event(PS_HE_THICC_WAVEDASHES);
    mario_set_forward_vel(m, speed);
    m->vel[1] = -32.0f;
    m->input &= ~INPUT_Z_PRESSED;
    m->particleFlags |= PARTICLE_SPARKLES;
    play_sound(SOUND_ACTION_SPIN, m->marioObj->header.gfx.cameraToObject);
    mario_he_thicc_clap(m, 0.20f);
    return TRUE;
}

s32 mario_he_thicc_wavedash(struct MarioState *m) {
    set_mario_animation(m, MARIO_ANIM_SLIDE_KICK);
    switch (perform_air_step(m, AIR_STEP_CHECK_NONE)) {
        case AIR_STEP_LANDED:
            if (!he_thicc_has_safe_floor(m)) {
                set_mario_action(m, ACT_FREEFALL_LAND, 0);
                return FALSE;
            }
            he_thicc_start_belly_slide(m, absf(m->forwardVel), FALSE);
            break;
        case AIR_STEP_HIT_WALL:
            mario_bonk_reflection(m, TRUE);
            set_mario_action(m, ACT_SOFT_BONK, 0);
            return FALSE;
        case AIR_STEP_HIT_LAVA_WALL:
            lava_boost_on_wall(m);
            return FALSE;
    }
    return FALSE;
}

s32 mario_he_thicc_belly_slide(struct MarioState *m) {
    // A must be a new press. Holding the takeoff button cannot auto-chain boosts.
    if (m->input & INPUT_A_PRESSED) {
        s32 timed = m->actionTimer < HE_THICC_BELLY_JUMP_FRAMES;
        f32 speed = absf(m->forwardVel);
        set_mario_action(m, ACT_FORWARD_ROLLOUT, timed ? ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE : 0);
        f32 launchSpeed = timed ? MAX(HE_THICC_BELLY_JUMP_SPEED, speed + 24.0f) : speed;
        if (timed && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
            launchSpeed = MIN(launchSpeed, HE_THICC_SLIDE_SPEED_LIMIT);
        }
        mario_set_forward_vel(m, launchSpeed);
        if (timed) {
            player_stats_he_thicc_event(PS_HE_THICC_BELLY_BOUNCES);
            m->vel[1] = HE_THICC_BELLY_JUMP_HEIGHT;
            m->particleFlags |= PARTICLE_VERTICAL_STAR | PARTICLE_DUST;
            play_sound(SOUND_HE_THICC_BOUNCE, m->marioObj->header.gfx.cameraToObject);
            mario_he_thicc_clap(m, 0.35f);
        }
        return TRUE;
    }
    if (m->input & INPUT_B_PRESSED) {
        return set_mario_action(m, ACT_FORWARD_ROLLOUT, 0);
    }
    m->actionTimer = MIN(m->actionTimer + 1, HE_THICC_BELLY_JUMP_FRAMES);
    // Use the native floor friction, steering and wall/ledge collision response.
    if (update_sliding(m, 4.0f)) {
        return set_mario_action(m, ACT_STOMACH_SLIDE_STOP, 0);
    }
    common_slide_action(m, ACT_STOMACH_SLIDE_STOP, ACT_FREEFALL, MARIO_ANIM_SLIDE_DIVE);
    return FALSE;
}

s32 mario_he_thicc_try_pound_bounce(struct MarioState *m) {
    if (!he_thicc_is_active() || m->action != ACT_GROUND_POUND_LAND || !(m->input & INPUT_A_PRESSED)
        || m->actionTimer >= HE_THICC_BELLY_JUMP_FRAMES || !he_thicc_has_safe_floor(m)
        || !mario_is_on_floor(m)) {
        return FALSE;
    }
    f32 speed = MAX(HE_THICC_WAVEDASH_SPEED, absf(m->forwardVel));
    set_mario_action(m, ACT_TRIPLE_JUMP, 0);
    mario_set_forward_vel(m, speed);
    m->vel[1] = HE_THICC_POUND_BOUNCE_HEIGHT;
    player_stats_he_thicc_event(PS_HE_THICC_POUND_BOUNCES);
    m->input &= ~INPUT_Z_PRESSED;
    m->particleFlags |= PARTICLE_VERTICAL_STAR | PARTICLE_SPARKLES;
    play_sound(SOUND_HE_THICC_BOUNCE, m->marioObj->header.gfx.cameraToObject);
    mario_he_thicc_clap(m, 0.40f);
    return TRUE;
}

void mario_he_thicc_wall_kick(struct MarioState *m) {
    if (he_thicc_is_active()) {
        m->actionArg = ACT_ARG_WALL_KICK_HE_THICC;
        player_stats_he_thicc_event(PS_HE_THICC_WALL_KICKS);
        m->particleFlags |= PARTICLE_HORIZONTAL_STAR;
        mario_he_thicc_clap(m, 0.35f);
    }
}

void mario_he_thicc_slide_kick_bounce(struct MarioState *m) {
    if (!he_thicc_is_active()) {
        return;
    }
    player_stats_he_thicc_event(PS_HE_THICC_SLIDE_KICK_BOUNCES);
    mario_ground_pound_impact(m);
    m->particleFlags |= PARTICLE_MIST_CIRCLE | PARTICLE_HORIZONTAL_STAR;
    play_sound(SOUND_HE_THICC_BOUNCE, m->marioObj->header.gfx.cameraToObject);
    mario_he_thicc_clap(m, 0.35f);
}

void mario_he_thicc_attack_impact(struct MarioState *m) {
    if (!he_thicc_is_active()) {
        return;
    }
    player_stats_he_thicc_event(PS_HE_THICC_EXPLOSIVE_STRIKES);
    struct Object *explosion = try_to_spawn_object(0, 1.0f, m->marioObj, MODEL_EXPLOSION, bhvExplosion);
    if (explosion != NULL) {
        explosion->oPosX = m->pos[0] + sins(m->faceAngle[1]) * 80.0f;
        explosion->oPosY = m->pos[1] + 80.0f;
        explosion->oPosZ = m->pos[2] + coss(m->faceAngle[1]) * 80.0f;
        // Damage belongs to the strike; the blast adds the existing harmless AOE stun.
        explosion->oBehParams2ndByte = HE_THICC_PUNCH_EXPLOSION;
        explosion->oInteractType = 0;
        explosion->oIntangibleTimer = -1;
    } else {
        play_sound(SOUND_GENERAL2_BOBOMB_EXPLOSION, m->marioObj->header.gfx.cameraToObject);
    }
    mario_ground_pound_impact(m);
    m->particleFlags |= PARTICLE_MIST_CIRCLE | PARTICLE_HORIZONTAL_STAR;
    mario_he_thicc_clap(m, 0.30f);
}

void mario_he_thicc_running_step(struct MarioState *m) {
    if (he_thicc_is_active() && (m->action == ACT_WALKING || m->action == ACT_HOLD_WALKING)
        && absf(m->forwardVel) >= HE_THICC_RUNNING_MIN_SPEED && mario_is_on_floor(m)
        && m->quicksandDepth <= 50.0f) {
        play_sound(SOUND_HE_THICC_RUNNING_STEP, m->marioObj->header.gfx.cameraToObject);
    }
}

void mario_he_thicc_breaststroke(struct MarioState *m) {
    if (he_thicc_is_active()) {
        player_stats_he_thicc_event(PS_HE_THICC_SWIMMING_STROKES);
        // This expanding particle ring needs no course-specific water-ring model.
        m->particleFlags |= PARTICLE_VANISH_WATER_RING;
    }
}

void mario_he_thicc_update(struct MarioState *m) {
    if (!he_thicc_is_active() || (m->action & ACT_GROUP_MASK) == ACT_GROUP_CUTSCENE) {
        mario_he_thicc_reset();
        return;
    }
    if (sHeThicc.initialized) {
        f32 verticalChange = m->vel[1] - sHeThicc.lastVerticalSpeed;
        sHeThicc.jiggleVelocity += MAX(-0.12f, MIN(0.12f, verticalChange * 0.004f));
        if (mario_is_on_floor(m) && !(m->action & ACT_FLAG_IDLE)) {
            f32 dx = m->pos[0] - sHeThicc.lastPos[0];
            f32 dz = m->pos[2] - sHeThicc.lastPos[2];
            // Exclude externally scripted teleports from the footstep cadence.
            f32 distance = sqrtf(dx * dx + dz * dz);
            if (distance <= HE_THICC_SLIDE_SPEED_LIMIT) {
                sHeThicc.clapDistance += distance;
            }
            if (sHeThicc.clapDistance >= HE_THICC_CLAP_DISTANCE) {
                sHeThicc.clapDistance -= HE_THICC_CLAP_DISTANCE;
                if ((m->action != ACT_WALKING && m->action != ACT_HOLD_WALKING)
                    || absf(m->forwardVel) < HE_THICC_RUNNING_MIN_SPEED) {
                    mario_he_thicc_clap(m, 0.12f);
                }
            }
        } else {
            sHeThicc.clapDistance = 0.0f;
        }
    }
    if (m->action == ACT_GROUND_POUND_LAND && sHeThicc.lastAction != ACT_GROUND_POUND_LAND) {
        mario_he_thicc_clap(m, 0.45f);
    }
    sHeThicc.jiggleVelocity = (sHeThicc.jiggleVelocity - sHeThicc.jiggle * 0.22f) * 0.78f;
    sHeThicc.jiggle = MAX(-1.0f, MIN(1.0f, sHeThicc.jiggle + sHeThicc.jiggleVelocity));
    // The native squash can retain its scale across updates (timer 0xFF).
    // Compose only with the ordinary scale freshly reset by squish_mario_model.
    if (m->squishTimer == 0 && m->action != ACT_SQUISHED) {
        m->marioObj->header.gfx.scale[0] *= 1.0f + sHeThicc.jiggle * 0.14f;
        m->marioObj->header.gfx.scale[1] *= 1.0f - sHeThicc.jiggle * 0.20f;
        m->marioObj->header.gfx.scale[2] *= 1.0f + sHeThicc.jiggle * 0.14f;
    }
    if (sHeThicc.clapFrames != 0) {
        sHeThicc.clapFrames--;
    }
    sHeThicc.lastVerticalSpeed = m->vel[1];
    sHeThicc.lastAction = m->action;
    vec3f_copy(sHeThicc.lastPos, m->pos);
    sHeThicc.initialized = TRUE;
}

static s32 he_thicc_uses_native_upper_body(s16 animID) {
    if (animID >= MARIO_ANIM_START_SLEEP_IDLE && animID <= MARIO_ANIM_SLEEP_LYING) {
        return TRUE;
    }
    switch (animID) {
        case MARIO_ANIM_IDLE_HEAD_LEFT:
        case MARIO_ANIM_IDLE_HEAD_RIGHT:
        case MARIO_ANIM_IDLE_HEAD_CENTER:
        case MARIO_ANIM_IDLE_WITH_LIGHT_OBJ:
        case MARIO_ANIM_IDLE_HEAVY_OBJ:
        case MARIO_ANIM_IDLE_IN_QUICKSAND:
        case MARIO_ANIM_SHIVERING_WARMING_HAND:
        case MARIO_ANIM_SHIVERING:
        case MARIO_ANIM_SHIVERING_RETURN_TO_IDLE:
        case MARIO_ANIM_WALK_PANTING:
        case MARIO_ANIM_WAKE_FROM_SLEEP:
        case MARIO_ANIM_WALKING:
        case MARIO_ANIM_RUNNING:
        case MARIO_ANIM_RUNNING_UNUSED:
        case MARIO_ANIM_TIPTOE:
        case MARIO_ANIM_WALK_WITH_LIGHT_OBJ:
        case MARIO_ANIM_RUN_WITH_LIGHT_OBJ:
        case MARIO_ANIM_SLOW_WALK_WITH_LIGHT_OBJ:
        case MARIO_ANIM_WALK_WITH_HEAVY_OBJ:
        case MARIO_ANIM_SKID_ON_GROUND:
        case MARIO_ANIM_STOP_SKID:
        case MARIO_ANIM_SINGLE_JUMP:
        case MARIO_ANIM_DOUBLE_JUMP_RISE:
        case MARIO_ANIM_DOUBLE_JUMP_FALL:
        case MARIO_ANIM_TRIPLE_JUMP:
        case MARIO_ANIM_GENERAL_FALL:
        case MARIO_ANIM_LAND_FROM_SINGLE_JUMP:
        case MARIO_ANIM_LAND_FROM_DOUBLE_JUMP:
        case MARIO_ANIM_TRIPLE_JUMP_LAND:
        case MARIO_ANIM_GENERAL_LAND:
        case MARIO_ANIM_JUMP_WITH_LIGHT_OBJ:
        case MARIO_ANIM_FALL_WITH_LIGHT_OBJ:
        case MARIO_ANIM_JUMP_LAND_WITH_LIGHT_OBJ:
        case MARIO_ANIM_FALL_LAND_WITH_LIGHT_OBJ:
            return TRUE;
    }
    return FALSE;
}

// Use the displayed animation clock so paused/mirrored draws cannot advance the dab.
static void he_thicc_animate_dab(struct AnimInfo *info, s16 frame) {
    struct Animation *anim = info->curAnim;
    s32 length = MAX(1, anim->loopEnd - anim->loopStart);
    s16 phase = (MAX(0, frame - anim->loopStart) % length) * 0x10000 / length;
    f32 swing = sins(phase);
    switch (info->animID) {
        case MARIO_ANIM_BACKFLIP:
        case MARIO_ANIM_SLIDEJUMP:
        case MARIO_ANIM_FAST_LONGJUMP:
        case MARIO_ANIM_SLOW_LONGJUMP:
        case MARIO_ANIM_SLIDEFLIP:
            sHeThicc.pose[20] += 0x900 + (s16) (swing * 0x400);
            sHeThicc.pose[32] -= 0x900;
            break;
        case MARIO_ANIM_DIVE:
        case MARIO_ANIM_SLIDE_DIVE:
        case MARIO_ANIM_SLIDE_KICK:
        case MARIO_ANIM_SLIDE:
        case MARIO_ANIM_FORWARD_SPINNING:
        case MARIO_ANIM_BACKWARD_SPINNING:
        case MARIO_ANIM_GROUND_POUND:
            sHeThicc.pose[18] += 0x1000;
            sHeThicc.pose[30] -= 0x800 + (s16) (swing * 0x300);
            sHeThicc.pose[12] += 0x500;
            break;
        case MARIO_ANIM_SWIM_PART1:
        case MARIO_ANIM_SWIM_PART2:
        case MARIO_ANIM_FLUTTERKICK:
        case MARIO_ANIM_WATER_ACTION_END:
        case MARIO_ANIM_SWIM_WITH_OBJ_PART1:
        case MARIO_ANIM_SWIM_WITH_OBJ_PART2:
        case MARIO_ANIM_FLUTTERKICK_WITH_OBJ:
        case MARIO_ANIM_WATER_ACTION_END_WITH_OBJ:
            sHeThicc.pose[19] += (s16) (swing * 0x1000);
            sHeThicc.pose[31] -= (s16) (swing * 0x1000);
            sHeThicc.pose[33] += (s16) (coss(phase) * 0x600);
            break;
        case MARIO_ANIM_CRAWLING:
            sHeThicc.pose[18] -= 0x800;
            sHeThicc.pose[30] += (s16) (swing * 0x800);
            break;
    }
}

s32 mario_he_thicc_render_pose(struct AnimInfo *info, struct MarioRenderPose *pose) {
    // Dab frame 7, baked from anim_D1 on the shared 20-bone Mario skeleton.
    static const s16 dabArms[] = {
        32767, 32767, 16753, -17703, -11094, 15175,  1, 0, 0,      0, 0, 0,
        32767, 32767, 16753, 8893,   23325,  -14494, 0, 0, -19943, 0, 0, 0,
    };
    struct Animation *anim = info->curAnim;
    u16 *attributes = pose->index;
    s16 frame = pose->frame;
    if (!he_thicc_is_active() || anim == NULL || anim->unusedBoneCount != 20
        || (anim->flags & ANIM_FLAG_DISABLED)) {
        return FALSE;
    }
    for (s32 i = 0; i < HE_THICC_POSE_CHANNELS; i++) {
        sHeThicc.pose[i] = pose->values[retrieve_animation_index(MAX(0, frame), &attributes)];
        sHeThicc.index[i * 2] = 1;
        sHeThicc.index[i * 2 + 1] = i;
    }
    if (pose->rootSpace == MARIO_POSE_NATIVE_ROOT) {
        for (s32 i = 0; i < 3; i++) {
            if ((anim->flags & ANIM_FLAG_NO_TRANS) || ((anim->flags & ANIM_FLAG_HOR_TRANS) && i != 1)
                || ((anim->flags & ANIM_FLAG_VERT_TRANS) && i == 1)) {
                sHeThicc.pose[i] = 0;
            } else {
                sHeThicc.pose[i] *= pose->rootScale;
            }
        }
    }
    // The inverse-stick segment already supplies its controlled upper-body pose.
    // Ordinary locomotion stays native; special moves vary the baked dab.
    if (!he_thicc_uses_native_upper_body(info->animID)
        && (info->animID != MARIO_ANIM_DAB || frame < DAB_ANALOG_START_FRAME
            || frame >= DAB_ANALOG_START_FRAME + DAB_ANALOG_FRAME_COUNT)) {
        for (s32 i = 0; i < 24; i++) {
            sHeThicc.pose[15 + i] = dabArms[i];
        }
        sHeThicc.pose[12] = -4824;
        sHeThicc.pose[13] = -1482;
        sHeThicc.pose[14] = 2919;
        he_thicc_animate_dab(info, frame);
    }
    sHeThicc.pose[8] += (s16) (sHeThicc.jiggle * 0x900);
    sHeThicc.pose[11] -= (s16) (sHeThicc.jiggle * 0x600);
    if (sHeThicc.clapFrames != 0) {
        s16 squeeze = sins((6 - sHeThicc.clapFrames) * 0x1800) * 0x900;
        sHeThicc.pose[41] += squeeze;
        sHeThicc.pose[53] -= squeeze;
    }
    pose->values = sHeThicc.pose;
    pose->index = sHeThicc.index;
    pose->frame = 0;
    pose->rootScale = 1.0f;
    pose->rootSpace = MARIO_POSE_RENDER_ROOT;
    return TRUE;
}
