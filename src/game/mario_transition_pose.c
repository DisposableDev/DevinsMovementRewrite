#include "sm64.h"
#include "engine/graph_node.h"
#include "memory.h"
#include "mario_transition_pose.h"

#define WALL_KICK_BLEND_FRAMES 3
#define WALL_KICK_TURN_FRAMES 1
#define MARIO_POSE_CHANNELS (3 * (20 + 1))
#define MARIO_BODY_ROTATION_START 6

// Only the displayed pose is blended. The real animation and its clock remain
// available to action timing, sounds, movement and animation-based positioning.
static struct {
    struct AnimInfo *owner;
    u16 startUpdate;
    s16 from[MARIO_POSE_CHANNELS];
    s16 displayed[MARIO_POSE_CHANNELS];
    u16 index[MARIO_POSE_CHANNELS * 2];
    u8 straightenFlipBody;
    u8 quickKickTurn;
    Vec3s angle;
    Vec3s displayedAngle;
} sTransitionPose;

void mario_transition_pose_reset(void) {
    sTransitionPose.owner = NULL;
    sTransitionPose.straightenFlipBody = FALSE;
    sTransitionPose.quickKickTurn = FALSE;
}

static f32 wall_kick_blend_amount(void) {
    u16 elapsed = gAreaUpdateCounter - sTransitionPose.startUpdate;
    f32 t = MIN(elapsed, WALL_KICK_BLEND_FRAMES) / (f32) WALL_KICK_BLEND_FRAMES;

    // Ease both ends, and use gameplay updates so pausing freezes the blend.
    return t * t * (3.0f - 2.0f * t);
}

static s16 wall_kick_blend_angle(s16 from, s16 to, f32 amount) {
    // Angles wrap at 65536; interpolate over the shorter arc.
    return from + (s32) ((s16) (to - from) * amount);
}

static void wall_kick_sample_pose(struct AnimInfo *animInfo, s16 *pose) {
    struct Animation *anim = animInfo->curAnim;
    u16 *index = segmented_to_virtual((void *) anim->index);
    s16 *values = segmented_to_virtual((void *) anim->values);
    s32 frame = MAX(0, animInfo->animFrame);
    f32 scale =
        anim->animYTransDivisor == 0 ? 1.0f : (f32) animInfo->animYTrans / anim->animYTransDivisor;
    s32 i;

    for (i = 0; i < MARIO_POSE_CHANNELS; i++) {
        pose[i] = values[retrieve_animation_index(frame, &index)];
    }

    // Store the root offset as actually drawn, regardless of translation flags
    // or differing animation scale divisors on either side of the transition.
    for (i = 0; i < 3; i++) {
        if ((anim->flags & ANIM_FLAG_NO_TRANS) || ((anim->flags & ANIM_FLAG_HOR_TRANS) && i != 1)
            || ((anim->flags & ANIM_FLAG_VERT_TRANS) && i == 1)) {
            pose[i] = 0;
        } else {
            pose[i] = pose[i] * scale;
        }
    }
}

void mario_transition_pose_begin(struct MarioState *m, s32 targetAnimID) {
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    s32 flipBody = animInfo->animID == MARIO_ANIM_SLIDEFLIP || animInfo->animID == MARIO_ANIM_BACKFLIP
                   || animInfo->animID == MARIO_ANIM_TRIPLE_JUMP
                   || animInfo->animID == MARIO_ANIM_BACKWARD_SPINNING;
    s32 straightenFlipBody =
        flipBody
        && (targetAnimID == MARIO_ANIM_START_WALLKICK
            || (sTransitionPose.owner == animInfo && sTransitionPose.straightenFlipBody));
    s32 quickKickTurn =
        (animInfo->animID == MARIO_ANIM_START_WALLKICK && targetAnimID == MARIO_ANIM_SLIDEJUMP
         && (sTransitionPose.owner != animInfo || wall_kick_blend_amount() >= 1.0f))
        || (targetAnimID == MARIO_ANIM_SLIDE && sTransitionPose.owner == animInfo
            && sTransitionPose.quickKickTurn);
    // Diving retains its separately requested pose transition; handstand takeoff
    // and landing now use matching animation/attachment origins without a blend.
    s32 handstandTransition =
        animInfo->animID == MARIO_ANIM_HANDSTAND_JUMP && targetAnimID == MARIO_ANIM_DIVE;
    s32 i;

    if (animInfo->animID == targetAnimID) {
        return;
    }
    if (animInfo->animID != MARIO_ANIM_START_WALLKICK && targetAnimID != MARIO_ANIM_START_WALLKICK
        && sTransitionPose.owner != animInfo && !handstandTransition) {
        return;
    }
    if (animInfo->curAnim == NULL || animInfo->curAnim->unusedBoneCount != 20
        || (animInfo->curAnim->flags & ANIM_FLAG_DISABLED)) {
        mario_transition_pose_reset();
        return;
    }

    // Mario's next animation overwrites the same DMA buffer. Capture now;
    // interrupted blends resume from the last visible pose, including multiple
    // animation changes within one gameplay update.
    if (sTransitionPose.owner != animInfo) {
        wall_kick_sample_pose(animInfo, sTransitionPose.displayed);
        for (i = 0; i < 3; i++) {
            sTransitionPose.displayedAngle[i] = m->marioObj->header.gfx.angle[i];
        }
    }
    for (i = 0; i < MARIO_POSE_CHANNELS; i++) {
        sTransitionPose.from[i] = sTransitionPose.displayed[i];
        sTransitionPose.index[i * 2] = 1;
        sTransitionPose.index[i * 2 + 1] = i;
    }
    for (i = 0; i < 3; i++) {
        sTransitionPose.angle[i] = sTransitionPose.displayedAngle[i];
    }
    sTransitionPose.owner = animInfo;
    sTransitionPose.straightenFlipBody = straightenFlipBody;
    // A settled wall pose should turn promptly into the actual launch, rather
    // than spinning sideways while already traveling away. Carry that turn
    // through the same-update replacement with HE THICC's seated kick pose.
    sTransitionPose.quickKickTurn = quickKickTurn;
    sTransitionPose.startUpdate = gAreaUpdateCounter;
}

void mario_transition_pose_update_gfx(struct MarioState *m) {
    struct AnimInfo *animInfo = &m->marioObj->header.gfx.animInfo;
    f32 amount;
    f32 turnAmount;
    s32 i;

    if (sTransitionPose.owner != animInfo) {
        return;
    }
    amount = wall_kick_blend_amount();
    turnAmount =
        sTransitionPose.quickKickTurn
            ? MIN((u16) (gAreaUpdateCounter - sTransitionPose.startUpdate), WALL_KICK_TURN_FRAMES)
                  / (f32) WALL_KICK_TURN_FRAMES
            : amount;
    for (i = 0; i < 3; i++) {
        m->marioObj->header.gfx.angle[i] = wall_kick_blend_angle(
            sTransitionPose.angle[i], m->marioObj->header.gfx.angle[i], i == 1 ? turnAmount : amount);
        sTransitionPose.displayedAngle[i] = m->marioObj->header.gfx.angle[i];
    }
}

s32 mario_transition_pose_render(struct AnimInfo *animInfo, struct MarioRenderPose *pose) {
    s16 target[MARIO_POSE_CHANNELS];
    f32 amount;
    s32 i;

    if (sTransitionPose.owner != animInfo) {
        return FALSE;
    }
    amount = wall_kick_blend_amount();
    if (amount >= 1.0f || animInfo->curAnim->unusedBoneCount != 20
        || (animInfo->curAnim->flags & ANIM_FLAG_DISABLED)) {
        mario_transition_pose_reset();
        return FALSE;
    }
    wall_kick_sample_pose(animInfo, target);
    for (i = 0; i < MARIO_POSE_CHANNELS; i++) {
        if (sTransitionPose.straightenFlipBody && i >= MARIO_BODY_ROTATION_START
            && i < MARIO_BODY_ROTATION_START + 3) {
            // Flipping jump animations spin the butt joint and the whole body.
            // Bring that joint upright at contact; blend the limbs as usual.
            sTransitionPose.displayed[i] = target[i];
        } else {
            sTransitionPose.displayed[i] =
                i < 3 ? sTransitionPose.from[i] + (s32) ((target[i] - sTransitionPose.from[i]) * amount)
                      : wall_kick_blend_angle(sTransitionPose.from[i], target[i], amount);
        }
    }
    pose->index = sTransitionPose.index;
    pose->values = sTransitionPose.displayed;
    pose->frame = 0;
    pose->rootScale = 1.0f;
    pose->rootSpace = MARIO_POSE_RENDER_ROOT;
    return TRUE;
}
