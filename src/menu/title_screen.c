#include <PR/ultratypes.h>

#include "string.h"

#include "audio/external.h"
#include "engine/math_util.h"
#include "game/area.h"
#include "game/game_init.h"
#include "game/game_options_runtime.h"
#include "game/ingame_menu.h"
#include "game/level_update.h"
#include "game/jrb_simpleflips.h"
#include "game/main.h"
#include "game/memory.h"
#include "game/print.h"
#include "game/save_file.h"
#include "game/sound_init.h"
#include "game/rumble_init.h"
#include "level_table.h"
#include "level_select.h"
#include "seq_ids.h"
#include "sm64.h"
#include "title_screen.h"

/**
 * @file title_screen.c
 * This file implements how title screen functions.
 * That includes playing demo sequences, introduction screens
 * and a level select used for testing purposes.
 */

#define STUB_LEVEL(textname, _1, _2, _3, _4, _5, _6, _7, _8) textname,
#define DEFINE_LEVEL(textname, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) textname,

static const char sLevelSelectStageNames[][32] = {
#include "levels/level_defines.h"
};
#undef STUB_LEVEL
#undef DEFINE_LEVEL

#ifdef KEEP_MARIO_HEAD
#ifndef DISABLE_DEMO
static u16 sDemoCountdown = 0;
#endif
static s16 sPlayMarioGreeting = TRUE;
static s16 sPlayMarioGameOver = TRUE;

#ifndef DISABLE_DEMO
#define PRESS_START_DEMO_TIMER 800

/**
 * Run the demo timer on the PRESS START screen after a number of frames.
 * This function returns the level ID from the first byte of a demo file.
 * It also returns the level ID from intro_regular (file select or level select menu)
 */
s32 run_level_id_or_demo(s32 level) {
    gCurrDemoInput = NULL;

    if (level == LEVEL_NONE) {
        if (!gPlayer1Controller->buttonDown && !gPlayer1Controller->stickMag) {
            // start the demo. 800 frames has passed while
            // player is idle on PRESS START screen.
            if ((++sDemoCountdown) == PRESS_START_DEMO_TIMER) {

                // start the Mario demo animation for the demo list.
                load_patchable_table(&gDemoInputsBuf, gDemoInputListID);

                // if the next demo sequence ID is the count limit, reset it back to
                // the first sequence.
                if (++gDemoInputListID == gDemoInputsBuf.dmaTable->count) {
                    gDemoInputListID = 0;
                }

                // add 1 (+4) to the pointer to skip the first 4 bytes
                // Use the first 4 bytes to store level ID,
                // then use the rest of the values for inputs
                gCurrDemoInput = ((struct DemoInput *) gDemoInputsBuf.bufTarget) + 1;
                level = (s8)((struct DemoInput *) gDemoInputsBuf.bufTarget)->timer;
                gCurrSaveFileNum = 1;
                gCurrActNum = 1;
            }
        } else { // activity was detected, so reset the demo countdown.
            sDemoCountdown = 0;
        }
    }
    return level;
}
#endif
#endif


u8 gLevelSelectHoldKeyIndex = 0;
u8 gLevelSelectHoldKeyTimer = 0;

#define LEVEL_SELECT_NAME_LINE_LENGTH 20

static void print_level_select_stage_name(const char *name) {
    char firstLine[LEVEL_SELECT_NAME_LINE_LENGTH + 1];
    s32 split = LEVEL_SELECT_NAME_LINE_LENGTH;
    s32 next;

    if (strlen(name) <= LEVEL_SELECT_NAME_LINE_LENGTH) {
        print_text_centered(SCREEN_WIDTH / 2, 60, name);
        return;
    }

    while (split > 0 && name[split] != ' ') {
        split--;
    }
    next = split > 0 ? split + 1 : LEVEL_SELECT_NAME_LINE_LENGTH;
    if (split == 0) {
        split = LEVEL_SELECT_NAME_LINE_LENGTH;
    }
    memcpy(firstLine, name, split);
    firstLine[split] = '\0';
    print_text_centered(SCREEN_WIDTH / 2, 60, firstLine);
    print_text_centered(SCREEN_WIDTH / 2, 40, name + next);
}

/**
 * Level select intro function, updates the selected stage
 * count if an input was received. signals the stage to be started
 * or the level select to be exited if start or the quit combo is pressed.
 */
s32 intro_level_select(void) {
    if (!game_options_debug_active()) {
        gDebugLevelSelect = FALSE;
        return LEVEL_FILE_SELECT;
    }

    u32 index = 0;
    s32 selected = level_select_index(gCurrLevelNum);
    if (gPlayer1Controller->rawStickY < -60
        || gPlayer1Controller->rawStickX < -60
        || gPlayer1Controller->buttonDown & (D_CBUTTONS | D_JPAD | L_CBUTTONS | L_JPAD)
    ) {
            index++;
    }

    if (gPlayer1Controller->rawStickY > 60
        || gPlayer1Controller->rawStickX > 60
        || gPlayer1Controller->buttonDown & (U_CBUTTONS | U_JPAD | R_CBUTTONS | R_JPAD)
    ) {
            index += 2;
    }

    u8 manual =
        (index & ~gLevelSelectHoldKeyIndex)
        || (gPlayer1Controller->buttonPressed
            & (U_JPAD | D_JPAD | L_JPAD | R_JPAD | U_CBUTTONS | D_CBUTTONS | L_CBUTTONS | R_CBUTTONS));
    if (manual) {
        gLevelSelectHoldKeyIndex = 0;
    }
    u8 pressed = menu_scrolling_input(index, &gLevelSelectHoldKeyIndex, &gLevelSelectHoldKeyTimer);
    if (pressed == 2) {
        play_sound(SOUND_GENERAL_LEVEL_SELECT_CHANGE, gGlobalSoundSource);
        selected += gPlayer1Controller->buttonDown & B_BUTTON ? 10 : 1;
    }

    if (pressed == 1) {
        play_sound(SOUND_GENERAL_LEVEL_SELECT_CHANGE, gGlobalSoundSource);
        selected -= gPlayer1Controller->buttonDown & B_BUTTON ? 10 : 1;
    }

    if (manual) {
        selected = (selected + LEVEL_SELECT_COUNT) % LEVEL_SELECT_COUNT;
    } else {
        selected = MAX(0, MIN(selected, LEVEL_SELECT_COUNT - 1));
    }
    gCurrLevelNum = sLevelSelectOrder[selected];
    // Use file 4 and last act as a test
    gCurrSaveFileNum = 4;
    gCurrActNum = 6;

    print_text_centered(160, 80, "SELECT STAGE");
    print_text_centered(160, 20, "PRESS START BUTTON");
    print_text_fmt_int((SCREEN_WIDTH / 2) - 12, 110, "%2d", gCurrLevelNum);
    print_level_select_stage_name(sLevelSelectStageNames[gCurrLevelNum - 1]);

    // start being pressed signals the stage to be started. that is, unless...
    if (gPlayer1Controller->buttonPressed & (START_BUTTON | A_BUTTON)) {
        // ... the level select quit combo is being pressed, which uses START. If this
        // is the case, quit the menu instead.
        if (gPlayer1Controller->buttonDown == (Z_TRIG | START_BUTTON | L_CBUTTONS)) { // quit level select
            gDebugLevelSelect = FALSE;
            return LEVEL_RESTART_GAME;
        }
        play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
        return gCurrLevelNum;
    }
    return LEVEL_NONE;
}

#ifdef KEEP_MARIO_HEAD
/**
 * Regular intro function that handles Mario's greeting voice and game start.
 */
s32 intro_regular(void) {
    s32 level = LEVEL_NONE;

    // When the game stars, gGlobalTimer is less than 129 frames,
    // so Mario greets the player. After that, he will always say
    // "press start to play" when it goes back to the title screen
    // (using SAVE AND QUIT)
    if (sPlayMarioGreeting) {
        if (gGlobalTimer < 129) {
            play_sound(SOUND_MARIO_HELLO, gGlobalSoundSource);
        } else {
            play_sound(SOUND_MARIO_PRESS_START_TO_PLAY, gGlobalSoundSource);
        }
        sPlayMarioGreeting = FALSE;
    }
    print_intro_text();
#ifdef DEBUG_LEVEL_SELECT
    if (game_options_debug_active() && (gPlayer1Controller->buttonDown & L_TRIG)) {
        gDebugLevelSelect = TRUE;
    }
#endif
    if (gPlayer1Controller->buttonPressed & START_BUTTON) {
        play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
        queue_rumble_data(60, 70);
        queue_rumble_decay(1);
        // calls level ID 100 (or 101 adding level select bool value)
        // defined in level_intro_mario_head_regular JUMP_IF commands
        // 100 is File Select - 101 is Level Select
        level = (LEVEL_FILE_SELECT + gDebugLevelSelect);
        sPlayMarioGreeting = TRUE;
    }
#if !defined(DISABLE_DEMO) && defined(KEEP_MARIO_HEAD)
    return run_level_id_or_demo(level);
#else
    return level;
#endif
}

/**
 * Game over intro function that handles Mario's game over voice and game start.
 */
s32 intro_game_over(void) {
    s32 level = LEVEL_NONE;

    if (sPlayMarioGameOver == TRUE) {
        play_sound(SOUND_MARIO_GAME_OVER, gGlobalSoundSource);
        sPlayMarioGameOver = FALSE;
    }

    print_intro_text();

    if (gPlayer1Controller->buttonPressed & START_BUTTON) {
        play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
        queue_rumble_data(60, 70);
        queue_rumble_decay(1);
        // same criteria as intro_regular
        level = LEVEL_FILE_SELECT + gDebugLevelSelect;
        sPlayMarioGameOver = TRUE;
        jrb_simpleflips_reset();
    }
#if !defined(DISABLE_DEMO) && defined(KEEP_MARIO_HEAD)
    return run_level_id_or_demo(level);
#else
    return level;
#endif
}

#endif

/**
 * Plays the casual "It's a me mario" when the game stars.
 */
s32 intro_play_its_a_me_mario(void) {
    play_sound(SOUND_MENU_COIN_ITS_A_ME_MARIO, gGlobalSoundSource);
    return LEVEL_NONE + 1;
}

/**
 * Update intro functions to handle title screen actions.
 * Returns a level ID after their criteria is met.
 */
s32 lvl_intro_update(s16 arg, UNUSED s32 unusedArg) {
    switch (arg) {
        case LVL_INTRO_PLAY_ITS_A_ME_MARIO: return intro_play_its_a_me_mario();
#ifdef KEEP_MARIO_HEAD
        case LVL_INTRO_REGULAR:             return intro_regular();
        case LVL_INTRO_GAME_OVER:           return intro_game_over();
#else
        case LVL_INTRO_REGULAR:
#ifdef DEBUG_LEVEL_SELECT
            if (game_options_debug_active() && (gPlayer1Controller->buttonDown & L_TRIG)) {
                gDebugLevelSelect = TRUE;
            }
#endif
            // fallthrough
        case LVL_INTRO_GAME_OVER:           return (LEVEL_FILE_SELECT + gDebugLevelSelect);
#endif
        case LVL_INTRO_LEVEL_SELECT:        return intro_level_select();
        default: return LEVEL_NONE;
    }
}
