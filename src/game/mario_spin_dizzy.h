#ifndef MARIO_SPIN_DIZZY_H
#define MARIO_SPIN_DIZZY_H

#include "types.h"

// Call once per Mario update before movement; rejected updates discard the gesture.
void mario_spin_dizzy_update(struct MarioState *m);
void mario_spin_dizzy_reset(void);
s32 mario_spin_dizzy_action(struct MarioState *m);

#endif // MARIO_SPIN_DIZZY_H
