#ifndef MARIO_HE_THICC_H
#define MARIO_HE_THICC_H

#include "types.h"
#include "mario_render_pose.h"

#define HE_THICC_BELLY_JUMP_FRAMES 6
#define HE_THICC_IDLE_SLEEP_FRAMES (3 * 30 + 1)
#define HE_THICC_HARD_LANDING_SPEED 55.0f
#define HE_THICC_BELLY_SLAM_HEIGHT 600.0f
#define HE_THICC_PUNCH_EXPLOSION 2
#define HE_THICC_POUND_SPEED 100.0f
#define ACT_ARG_WALL_KICK_HE_THICC 0x10
#define ACT_ARG_FORWARD_ROLLOUT_HE_THICC_BOUNCE 4

void mario_he_thicc_reset(void);
// Called once after movement. Rendering only reads the spring and clap pose.
void mario_he_thicc_update(struct MarioState *m);
void mario_he_thicc_clap(struct MarioState *m, f32 strength);
s32 mario_he_thicc_try_wavedash(struct MarioState *m);
s32 mario_he_thicc_wavedash(struct MarioState *m);
s32 mario_he_thicc_try_belly_landing(struct MarioState *m);
s32 mario_he_thicc_belly_slide(struct MarioState *m);
s32 mario_he_thicc_try_pound_bounce(struct MarioState *m);
void mario_he_thicc_wall_kick(struct MarioState *m);
void mario_he_thicc_slide_kick_bounce(struct MarioState *m);
// Contact dispatch owns once-per-strike gating; this applies visual/stun feedback.
void mario_he_thicc_attack_impact(struct MarioState *m);
void mario_he_thicc_running_step(struct MarioState *m);
void mario_he_thicc_breaststroke(struct MarioState *m);
// Samples the displayed animation, preserving gameplay animation data/clocks.
// FALSE leaves pose unchanged. TRUE installs shared scratch buffers, overwritten
// by the next successful call; consume the pose before sampling another draw.
s32 mario_he_thicc_render_pose(struct AnimInfo *info, struct MarioRenderPose *pose);

#endif
