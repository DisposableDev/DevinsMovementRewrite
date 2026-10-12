#ifndef MARIO_ACTIONS_SUBMERGED_H
#define MARIO_ACTIONS_SUBMERGED_H

#include <PR/ultratypes.h>

#include "types.h"

enum ActionStatesBreaststroke { // act_breaststroke, act_hold_breaststroke
    ACT_STATE_BREASTSTROKE_START,
    ACT_STATE_BREASTSTROKE_CONTINUE
};

enum ActionArgsFlutterKick { // act_flutter_kick
    ACT_ARG_FLUTTER_KICK_A_SWIM,
    ACT_ARG_FLUTTER_KICK_B_SWIM
};

enum ActionArgsWaterActionEnd {
    ACT_ARG_WATER_ACTION_END_NORMAL,
    ACT_ARG_WATER_ACTION_END_GROUND_POUND
};

enum ActionArgsWaterDownBurst { // Bit flags, reset for each new ground pound.
    ACT_ARG_WATER_DOWN_BURST_NORMAL = 0,
    ACT_ARG_WATER_DOWN_BURST_FROM_AIR = (1 << 0),
    ACT_ARG_WATER_DOWN_BURST_STARTED_STRONG = (1 << 1),
    ACT_ARG_WATER_DOWN_BURST_REPEAT_QUEUED = (1 << 2)
};

enum ActionStatesWaterDownBurst { // act_water_down_burst
    ACT_STATE_WATER_DOWN_BURST_STARTUP,
    ACT_STATE_WATER_DOWN_BURST_DELAY,
    ACT_STATE_WATER_DOWN_BURST_SINK
};

enum ActionStatesWaterPunch { // act_water_punch
    ACT_STATE_WATER_PUNCH_START,
    ACT_STATE_WATER_PUNCH_MISSED,
    ACT_STATE_WATER_PUNCH_PICK_UP
};

enum ActionStatesDrowning { // act_drowning
    ACT_STATE_DROWNING_EYES_HALF_CLOSED,
    ACT_STATE_DROWNING_EYES_DEAD
};

enum ActionStatesWaterPlunge { // act_water_plunge
    ACT_STATE_WATER_PLUNGE_FALL,
    ACT_STATE_WATER_PLUNGE_SINK
};

enum WaterPlungeFlags { // act_water_plunge
    PLUNGE_FLAGS_NONE       = (0 << 0), // 0x0
    PLUNGE_FLAG_HOLDING_OBJ = (1 << 0), // 0x1
    PLUNGE_FLAG_DIVING      = (1 << 1), // 0x2
    PLUNGE_FLAG_METAL_CAP   = (1 << 2), // 0x4
};

void mario_swimming_reset(void);
s32 mario_swimming_roll_is_active(void);
// Sample controls and age timers once per Mario update, before the action loop.
void mario_swimming_update(struct MarioState *m);

s32 mario_execute_submerged_action(struct MarioState *m);

#endif // MARIO_ACTIONS_SUBMERGED_H
