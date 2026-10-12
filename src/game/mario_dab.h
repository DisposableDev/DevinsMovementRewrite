#ifndef MARIO_DAB_H
#define MARIO_DAB_H

#include "types.h"

// Appended inverse-stick poses in anim_D1: full dab through relaxed wind-up.
#define DAB_ANALOG_START_FRAME 52
#define DAB_ANALOG_FRAME_COUNT 64

// Call once per Mario update to sample the chord and maintain its session state.
// Returns TRUE when a new dab begins, so the caller can reset the spin gesture.
s32 mario_dab_update_shortcut(struct MarioState *m);
s32 mario_dab_can_start_from_neutral(struct MarioState *m);
s32 mario_dab_loses_cap_on_damage(struct MarioState *m);
s32 mario_dab_action(struct MarioState *m);
void mario_dab_reset(void);

#endif // MARIO_DAB_H
