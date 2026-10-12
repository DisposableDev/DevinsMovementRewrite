#ifndef MARIO_STEP_H
#define MARIO_STEP_H

#include <PR/ultratypes.h>

#include "types.h"

struct BullyCollisionData {
  /*0x00*/ f32 conversionRatio;
  /*0x04*/ f32 radius;
  /*0x08*/ f32 posX;
  /*0x0C*/ f32 posZ;
  /*0x10*/ f32 velX;
  /*0x14*/ f32 velZ;
};

extern struct Surface gWaterSurfacePseudoFloor;

f32 mario_get_movement_time_scale(struct MarioState *m);
// Cap surface support requires forward speed >=30 and y velocity >-60; excluded actions bypass it.
s32 mario_can_walk_on_water(struct MarioState *m, f32 waterLevel, f32 ceilHeight);
void mario_tick_water_walking_cooldown(struct MarioState *m);
void mario_update_water_walking_state(struct MarioState *m, f32 waterLevel, f32 ceilHeight);

f32 get_additive_y_vel_for_jumps(void);
void stub_mario_step_1(struct MarioState *m);
void stub_mario_step_2(void);

void mario_bonk_reflection(struct MarioState *m, u32 negateSpeed);
void set_vel_from_pitch_and_yaw(struct MarioState *m);
void mario_play_bonk_sound(struct MarioState *m);
// Entered bonk recovery scales impact speed, with the existing recoil as a minimum.
struct Surface *mario_find_air_twirl_entry_wall(struct MarioState *m);
void mario_set_wall_bonk_momentum(struct MarioState *m, f32 recoilSpeed, f32 minimumSpeed);
void transfer_bully_speed(struct BullyCollisionData *obj1, struct BullyCollisionData *obj2);
void init_bully_collision_data(struct BullyCollisionData *data, f32 posX, f32 posZ,
                                          f32 forwardVel, s16 yaw, f32 conversionRatio, f32 radius);
u32 mario_update_quicksand(struct MarioState *m, f32 sinkingSpeed);
s32 mario_is_on_floor(struct MarioState *m);
u32 mario_push_off_steep_floor(struct MarioState *m, u32 action, u32 actionArg);
u32 mario_update_moving_sand(struct MarioState *m);
u32 mario_update_windy_ground(struct MarioState *m);
void stop_and_set_height_to_floor(struct MarioState *m);
s32 stationary_ground_step(struct MarioState *m);
s32 perform_ground_step(struct MarioState *m);
s32 perform_ground_step_stop_at_ledge(struct MarioState *m);
// Collision-only movement for the unused part of a landing frame.
s32 perform_ground_step_with_fraction(struct MarioState *m, f32 fraction);
s32 perform_air_step(struct MarioState *m, u32 stepArg);
s32 mario_is_in_vertical_wind(struct MarioState *m);
s32 mario_is_inside_wind_column(struct MarioState *m);
// Once per gameplay update, after floor selection and before special-floor hazards.
void mario_update_wind_death_grace(struct MarioState *m);
// Reports time left after the substep that reached the floor; no time on other exits.
s32 perform_air_step_with_remainder(struct MarioState *m, u32 stepArg, f32 *remaining);
// Collision-only rebound movement; does not repeat gravity or wind for this update.
// Exact solid/liquid contact remainder for slide-kick rebound continuation.
s32 perform_slide_kick_air_step(struct MarioState *m, f32 *remaining, s32 checkWater, f32 *waterLevel);
s32 perform_air_step_with_fraction(struct MarioState *m, u32 stepArg, f32 fraction);

#endif // MARIO_STEP_H
