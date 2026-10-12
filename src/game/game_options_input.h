#ifndef GAME_OPTIONS_INPUT_H
#define GAME_OPTIONS_INPUT_H

#include <PR/ultratypes.h>

#define GAME_OPTIONS_INPUT_DIALOGUE 2

void game_options_input_init(void);
// Select after a completed physical poll, before demos replace any pad data.
void game_options_input_select(s32 menu, s32 demo);
void game_options_input_read(s32 menu, s32 demo);
s32 game_options_input_take_disconnect(void);

#endif
