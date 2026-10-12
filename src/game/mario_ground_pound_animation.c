#include "sm64.h"
#include "memory.h"
#include "mario_ground_pound_animation.h"

#define GROUND_POUND_POSE_CHANNELS (3 * (20 + 1))

s32 mario_ground_pound_animation_render(struct AnimInfo *info, struct MarioState *m,
                                        struct MarioRenderPose *renderPose) {
    static s16 pose[GROUND_POUND_POSE_CHANNELS];
    static u16 attributes[GROUND_POUND_POSE_CHANNELS * 2];
    struct Animation *anim = info->curAnim;
    s32 frame, nextFrame, i;
    f32 position, fraction;
    s16 *source;
    u16 *channels;

    if (m == NULL || anim->unusedBoneCount != 20 || (anim->flags & ANIM_FLAG_DISABLED)
        || (info->animID != MARIO_ANIM_START_GROUND_POUND
            && info->animID != MARIO_ANIM_TRIPLE_JUMP_GROUND_POUND)) {
        return FALSE;
    }

    position = info->animAccel != 0 ? info->animFrameAccelAssist / 65536.0f : info->animFrame;
    if (m->action == ACT_GROUND_POUND) {
        // The action already reserves four extra updates after the native
        // windup. Use that time for the displayed spin instead of a frozen pose.
        position = MAX(0, m->actionTimer - 1) * (anim->loopEnd - 1.0f) / (anim->loopEnd + 3.0f);
    }
    position = MAX(anim->startFrame, MIN(position, anim->loopEnd - 1.0f));
    frame = (s32) position;
    nextFrame = MIN(frame + 1, anim->loopEnd - 1);
    fraction = position - frame;
    source = segmented_to_virtual((void *) anim->values);
    channels = segmented_to_virtual((void *) anim->index);
    for (i = 0; i < GROUND_POUND_POSE_CHANNELS; i++) {
        s32 count = channels[i * 2];
        s32 offset = channels[i * 2 + 1];
        s16 from = source[offset + MIN(frame, count - 1)];
        s16 to = source[offset + MIN(nextFrame, count - 1)];
        // Root translations are linear; joint rotations take the short arc
        // across the signed-angle boundary during the spin.
        s32 delta = i < 3 ? (s32) to - from : (s16) (to - from);
        pose[i] = from + (s32) (delta * fraction);
        attributes[i * 2] = 1;
        attributes[i * 2 + 1] = i;
    }
    renderPose->values = pose;
    renderPose->index = attributes;
    renderPose->frame = 0;
    return TRUE;
}
