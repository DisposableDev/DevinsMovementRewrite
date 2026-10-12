#ifndef GAME_OPTIONS_MENU_H
#define GAME_OPTIONS_MENU_H

#include "game_options.h"

enum OptionsMenuButton {
    OM_UP = 1 << 0,
    OM_DOWN = 1 << 1,
    OM_LEFT = 1 << 2,
    OM_RIGHT = 1 << 3,
    OM_CONFIRM = 1 << 4,
    OM_CANCEL = 1 << 5,
    OM_PREVIOUS = 1 << 6,
    OM_NEXT = 1 << 7,
    OM_RESET = 1 << 8
};

struct GameOptionsMenuState {
    s32 open;
    s32 file;
    u32 fileSelectionSerial;
    u8 category;
    u8 selected;
    u8 count;
    u8 ids[GO_COUNT];
    s32 warning;
    enum GameOptionId warningId;
    u32 frame;
    u32 directionStart;
    u16 direction;
    u8 stickDirection;
    u8 heldDirection;
    u8 sampled;
    u8 saveFailed;
    u8 records;
    u8 recordCourse;
};

extern struct GameOptionsMenuState gOptionsMenu;
void game_options_menu_open(void);
void game_options_menu_close(void);
// Directions are held states; buttonPressed is a fresh-edge mask. Stick
// directions remain separate so neutral-only navigation never suppresses D-pad.
// Returns TRUE when this subpage closes; gameplay remains paused.
s32 game_options_menu_step(u32 frame, u16 held, u16 pressed, u8 stickDirection);
// Write a null-terminated label to the 32-byte output; invalid IDs produce empty text.
void game_options_menu_value(enum GameOptionId id, char output[32]);
// Render the current subpage in the caller's menu transform; owns the IA text pipeline.
void game_options_menu_render(void);
// Translate native controller state, then sample through the frame-guarded menu step.
s32 game_options_menu_update_controller(void);

#endif
