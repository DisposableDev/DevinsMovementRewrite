#ifndef BOWSER_THROW_H
#define BOWSER_THROW_H

#include "types.h"

s32 bowser_check_fallen_off_stage(void);
void bowser_throw_velocity(s16 pitch, s16 spin, f32 *forwardVel, f32 *velY);
struct Object *bowser_throw_nearest_mine(struct Object *bowser);
void bowser_update_spin(struct MarioState *m);
void bowser_throw_reset_history(void);
void bowser_throw_record(struct MarioState *m);
void bowser_throw_assist(struct MarioState *m);

#endif
