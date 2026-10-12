#ifndef MARIO_GROUND_POUND_ANIMATION_H
#define MARIO_GROUND_POUND_ANIMATION_H

#include "types.h"
#include "mario_render_pose.h"

// Requires info->curAnim. TRUE supplies a render-only pose with borrowed arrays
// valid until the next call; FALSE leaves pose unchanged. Animation clocks and
// action timing are unchanged.
s32 mario_ground_pound_animation_render(struct AnimInfo *info, struct MarioState *m,
                                        struct MarioRenderPose *pose);

#endif
