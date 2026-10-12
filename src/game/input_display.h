#ifndef INPUT_DISPLAY_H
#define INPUT_DISPLAY_H

#include <PR/ultratypes.h>

struct Controller;

// Display-list command reserve for history, controller highlights and readouts.
#define INPUT_DISPLAY_GFX_RESERVE 2048
#define INPUT_DISPLAY_CONTENT_LEFT 120
#define INPUT_DISPLAY_BACKGROUND_WIDTH 112
#define INPUT_DISPLAY_BACKGROUND_GFX_RESERVE 512

// Sample physical controller input once per game-loop frame, before gameplay remapping.
// stickMag is deadzone-adjusted; raw axes determine angle and history sector.
void input_display_update(const struct Controller *controller);
void render_input_display(void);
// TRUE when either overlay drawing preference is enabled.
s32 input_display_enabled(void);

#endif
