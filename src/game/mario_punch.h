#ifndef MARIO_PUNCH_H
#define MARIO_PUNCH_H

#include <PR/ultratypes.h>

#include "types.h"

enum ActionStatesPunchSequence {
    ACT_STATE_PUNCH_LUNGE_UNDECIDED,
    ACT_STATE_PUNCH_LUNGE_ACTIVE,
    ACT_STATE_PUNCH_LUNGE_INACTIVE
};

enum ActionArgsPunchSequence { // mario_update_punch_sequence
    ACT_ARG_PUNCH_SEQUENCE_YAH,
    ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH,
    // The inherited FAST names select recovery animations, not faster attacks.
    ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST,
    ACT_ARG_PUNCH_SEQUENCE_WAH,
    ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH,
    ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH_FAST,
    ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK,
    ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE = 9,
};

// Call before lunge/drag in the standing or moving punch action update.
// TRUE requests immediate action redispatch; FALSE may still finish the sequence.
s32 mario_update_punch_sequence(struct MarioState *m);
// Action entry, exit or restart clears the mode; animation follow-ups retain it.
void mario_reset_punch_sequence(void);
s32 mario_is_first_punch(struct MarioState *m);
void apply_punch_lunge(struct MarioState *m);
void apply_punch_ground_drag(struct MarioState *m);

#endif // MARIO_PUNCH_H
