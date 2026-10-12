#ifndef MARIO_ACTIONS_AIRBORNE_H
#define MARIO_ACTIONS_AIRBORNE_H

#include <PR/ultratypes.h>

#include "types.h"

#define JUMP_KICK_CANCEL_FRAMES 16
#define JUMP_KICK_BUFFER_FRAMES 3
s32 mario_jump_kick_can_cancel(struct MarioState *m);

s32 lava_boost_on_wall(struct MarioState *m);
void update_air_with_turn(struct MarioState *m);
void finish_water_jump_pose(struct MarioState *m);
s32 mario_would_take_fall_damage_on_water(struct MarioState *m, f32 waterLevel);
s32 check_fall_damage_or_get_stuck(struct MarioState *m, u32 hardFallAction);

#define WALL_KICK_MOMENTUM_RETENTION 0.90f
#define WALL_KICK_MIN_FORWARD_SPEED 28.0f
#define DIVE_STARTUP_VULNERABLE_FRAMES 3
#define DIVE_WALL_BONK_GRACE_FRAMES 3
#define SLIDE_KICK_STARTUP_VULNERABLE_FRAMES 6
#define FORWARD_ROLLOUT_WALL_KICK_DELAY_FRAMES 3
#define ROLLOUT_WATER_WALK_DELAY_FRAMES 8
#define BACKWARD_JUMP_WALL_KICK_DELAY_FRAMES 13
#define WALL_KICK_STICK_LOOKBACK_FRAMES 3
#define ACT_ARG_FORWARD_ROLLOUT_FROM_SLIDE_KICK 1
#define ACT_ARG_FORWARD_ROLLOUT_QUEUED_SLIDE_KICK 2
#define ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE 1
#define ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL 2
#define ACT_ARG_BUTT_SLIDE_JUMP_DIVE_READY 0x100
#define ACT_ARG_BUTT_SLIDE_JUMP_DIVE_BUFFERED 0x200
#define ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED 1
#define ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE 2
#define ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL 4
#define ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL 1
#define ACT_ARG_DIVE_FROM_ROLLOUT_CONTACT 2

s32 mario_restrict_air_uphill(struct MarioState *m, struct Surface *floor);

void mario_update_wall_kick_slide_input(struct MarioState *m);

enum ActionStatesGroundPound { // act_ground_pound
    ACT_STATE_GROUND_POUND_SPIN,
    ACT_STATE_GROUND_POUND_FALL
};

enum ActionArgsGroundPound { // act_ground_pound
    ACT_ARG_GROUND_POUND_NORMAL,
    ACT_ARG_GROUND_POUND_FROM_TRIPLE_JUMP
};

enum ActionArgsTripleJump { // act_triple_jump, act_special_triple_jump
    ACT_ARG_TRIPLE_JUMP_FORWARD,
    ACT_ARG_TRIPLE_JUMP_BACKWARD
};

enum ActionArgsPuttingOnCapAir { // act_putting_on_cap_air
    ACT_ARG_CAP_PICKUP_REGULAR,
    ACT_ARG_CAP_PICKUP_CAPLESS
};

enum ActionStatesGettingBlown { // act_getting_blown
    ACT_STATE_GETTING_BLOWN_ACCEL_BACKWARDS,
    ACT_STATE_GETTING_BLOWN_SLOW_DOWN
};

enum ActionStatesButtSlideAir { // act_butt_slide_air, act_hold_butt_slide_air
    ACT_STATE_BUTT_SLIDE_AIR_SMALL_BOUNCE,
    ACT_STATE_BUTT_SLIDE_AIR_START_SLIDING
};

enum ActionArgsLavaBoost {
    ACT_ARG_LAVA_BOOST_FLOOR,
    ACT_ARG_LAVA_BOOST_WALL,
    ACT_ARG_LAVA_BOOST_METAL_GROUND_POUND
};

enum ActionStatesLavaBoost { // act_lava_boost
    ACT_STATE_LAVA_BOOST_HIT_LAVA,
    ACT_STATE_LAVA_BOOST_SLOW_DOWN,
    ACT_STATE_LAVA_BOOST_SET_LANDING_ACTION
};

enum ActionStatesSlideKick { // act_slide_kick
    ACT_STATE_SLIDE_KICK_SLIDING,
    ACT_STATE_SLIDE_KICK_END,
    ACT_STATE_SLIDE_KICK_WATER_SKIPPED
};

enum ActionStatesJumpKick { // act_jump_kick
    ACT_STATE_JUMP_KICK_PLAY_SOUND_AND_ANIM,
    ACT_STATE_JUMP_KICK_KICKING,
    ACT_STATE_JUMP_KICK_RECOVERY
};

enum ActionStatesFlying { // act_flying
    ACT_STATE_FLYING_SPIN,
    ACT_STATE_FLYING_FLY
};

enum ActionArgsFlying { // act_flying
    ACT_ARG_FLYING_FROM_CANNON,
    ACT_ARG_FLYING_DEFAULT,
    ACT_ARG_FLYING_TOTWC
};

enum ActionStatesRidingHoot { // act_riding_hoot
    ACT_STATE_RIDING_HOOT_GRABBING,
    ACT_STATE_RIDING_HOOT_HANGING
};

enum ActionStatesFlyingTripleJump { // act_flying_triple_jump
    ACT_STATE_FLYING_TRIPLE_JUMP_START,
    ACT_STATE_FLYING_TRIPLE_JUMP_SPIN
};

enum ActionStatesVerticalWind { // act_vertical_wind
    ACT_STATE_VERTICAL_WIND_SPINNING,
    ACT_STATE_VERTICAL_WIND_AIRBORNE
};

enum ActionStatesSpecialTripleJump { // act_special_triple_jump
    ACT_STATE_SPECIAL_TRIPLE_JUMP_SPINNING,
    ACT_STATE_SPECIAL_TRIPLE_JUMP_LANDING
};

enum ActionArgsFreefall { // act_freefall
    ACT_ARG_FREEFALL_GENERAL,
    ACT_ARG_FREEFALL_FROM_SLIDE,
    ACT_ARG_FREEFALL_FROM_SLIDE_KICK
};

s32 mario_execute_airborne_action(struct MarioState *m);
s32 mario_has_dive_input(struct MarioState *m);
s32 mario_can_wall_kick_backward_jump(struct MarioState *m);
void update_air_without_turn(struct MarioState *m);

#endif // MARIO_ACTIONS_AIRBORNE_H
