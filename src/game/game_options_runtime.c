#include "game_options_runtime.h"

#include "area.h"
#include "game_init.h"
#include "level_update.h"
#include "main.h"
#include "mario.h"

s32 game_options_is_cheating(void) {
#if defined(UNLOCK_ALL) || defined(COMPLETE_SAVE_FILE)
    return TRUE;
#else
    if (gDebugLevelSelect || (gMarioState && gMarioState->action == ACT_DEBUG_FREE_MOVE)) {
        return TRUE;
    }
    // Enabling a cheat preference counts even before its master permission is on.
    for (s32 id = 0; id < GO_COUNT; id++) {
        if (gGameOptionDefinitions[id].category == GO_CHEATS && game_options_get(id)) {
            return TRUE;
        }
    }
    return FALSE;
#endif
}

s32 game_options_secrets_blocked(void) {
    return game_options_visit_cheating(game_options_is_cheating());
}

void game_options_runtime_restart(void) {
    // The Grand Star and ending screens continue the Bowser visit's eligibility.
    s32 endingCheated =
        (gCurrCreditsEntry != NULL || gCurrLevelNum == LEVEL_ENDING) && game_options_secrets_blocked();
    // A full reload ends the old flight before timers and objects check the new visit.
    if (gMarioState && gMarioState->action == ACT_DEBUG_FREE_MOVE) {
        gMarioState->action = ACT_UNINITIALIZED;
    }
    game_options_restart();
    game_options_visit_cheating(endingCheated);
    game_options_secrets_blocked();
}

s32 game_options_runtime_allowed(void) {
    s32 file = game_options_active_file();
    return file >= 0 && file < GAME_OPTIONS_FILES && file == gCurrSaveFileNum - 1
           && gCurrDemoInput == NULL && gCurrCreditsEntry == NULL;
}

s32 game_options_debug_active(void) {
    return game_options_runtime_allowed() && game_options_get(GO_DEBUG_MODE);
}

s32 game_options_free_move_active(void) {
    return game_options_debug_active() && game_options_get(GO_FREE_MOVE);
}

s32 game_options_cheat_active(enum GameOptionId id) {
    if (id == GO_FREE_MOVE) {
        return game_options_free_move_active();
    }
    if (!game_options_runtime_allowed() || !game_options_get(GO_CHEATS_ENABLED)) {
        return FALSE;
    }
    return game_options_cheat(id);
}

s32 game_options_damage_voice_muted(void) {
    return game_options_cheat_active(GO_INFINITE_HEALTH) && game_options_cheat_active(GO_KNOCKBACK);
}

void game_options_runtime_update(void) {
    // Observe legacy debug flight/selection before permission cleanup clears them.
    game_options_secrets_blocked();
    // The legacy title selector also grants health/cap and progression shortcuts.
    if (!game_options_debug_active()) {
        gDebugLevelSelect = FALSE;
    }
}
