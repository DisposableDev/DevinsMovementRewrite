#ifndef MARIO_TRANSITION_POSE_H
#define MARIO_TRANSITION_POSE_H

#include "types.h"
#include "mario_render_pose.h"

// Display-only blending; gameplay animation clocks are preserved.
void mario_transition_pose_reset(void);
void mario_transition_pose_begin(struct MarioState *m, s32 targetAnimID);
void mario_transition_pose_update_gfx(struct MarioState *m);
// TRUE supplies borrowed render arrays valid until the next pose operation;
// FALSE leaves pose unchanged. These arrays are not owned by the caller.
s32 mario_transition_pose_render(struct AnimInfo *animInfo, struct MarioRenderPose *pose);

#endif
