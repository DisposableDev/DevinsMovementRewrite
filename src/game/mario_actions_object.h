#ifndef MARIO_ACTIONS_OBJECT_H
#define MARIO_ACTIONS_OBJECT_H

#include <PR/ultratypes.h>

#include "types.h"

enum ActionStatesPickingUp { // act_picking_up, act_picking_up_bowser
    ACT_STATE_PICKING_UP_GRAB,
    ACT_STATE_PICKING_UP_HOLDING
};

s32 mario_execute_object_action(struct MarioState *m);

#endif // MARIO_ACTIONS_OBJECT_H
