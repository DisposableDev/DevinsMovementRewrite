#ifndef MARIO_ACTIONS_MOVING
#define MARIO_ACTIONS_MOVING

#include <PR/ultratypes.h>

#include "types.h"

void common_slide_action(struct MarioState *m, u32 endAction, u32 airAction, s32 animation);

enum WalkSpeeds {
	WALK_SPEED_START_TIPTOE,
	WALK_SPEED_TIPTOE,
	WALK_SPEED_WALKING,
	WALK_SPEED_RUNNING,
};

enum HoldWalkSpeeds {
	HOLD_WALK_SPEED_SLOW,
	HOLD_WALK_SPEED_WALKING,
	HOLD_WALK_SPEED_RUN,
};

enum ActionStatesWalking { // act_walking
    ACT_STATE_WALKING_NO_WALL,
    ACT_STATE_WALKING_REACH_WALL
};

#define GROUND_TWIRL_IMMUNE_REBOUND_STATE 1 // Faster harmless soft knockback.
#define GROUND_TWIRL_TARGET_SPEED 40.0f
#define DIVE_SLIDE_ATTACK_FRAMES 2
#define ACT_ARG_GROUND_SPEED_FROM_ROLLOUT 1
// Walking also stores 16-bit wall-facing angles in its argument.
#define ACT_ARG_WALKING_TURN_FROM_PUNCH 0x10000

extern u8 gDebugFastWalk;

enum ActionStatesPushOrSidleWall { // push_or_sidle_wall
    ACT_STATE_PUSH_OR_SIDLE_WALL_PUSHING,
    ACT_STATE_PUSH_OR_SIDLE_WALL_SIDLING
};

// Shared slope and neutral-stick speed updates and braking collision response.
void apply_slope_accel(struct MarioState *m);
s32 apply_slope_decel(struct MarioState *m, f32 decelCoef);
s32 update_sliding(struct MarioState *m, f32 stopSpeed);
s32 update_decelerating_speed(struct MarioState *m);
s32 coasting_ground_step(struct MarioState *m);
void update_walking_speed(struct MarioState *m);
void update_walking_speed_with_minimum(struct MarioState *m, f32 minimumSpeed, f32 maximumSpeed);
void slide_bonk(struct MarioState *m, u32 fastAction, u32 slowAction);
s32 check_ground_dive_or_punch(struct MarioState *m);

void play_step_sound(struct MarioState *m, s16 frame1, s16 frame2);
s32 mario_execute_moving_action(struct MarioState *m);

#endif // MARIO_ACTIONS_MOVING
