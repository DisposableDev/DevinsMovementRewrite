#include "game_timing_runtime.h"

#include <string.h>
#include "sm64.h"
#include "area.h"
#include "dialog_ids.h"
#include "game_init.h"
#include "game_options.h"
#include "game_options_menu.h"
#include "game_options_runtime.h"
#include "game_timing.h"
#include "ingame_menu.h"
#include "interaction.h"
#include "level_update.h"
#include "main.h"
#include "mario.h"
#include "mario_actions_automatic.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "player_stats_menu.h"
#include "segment2.h"

// Observe shared cheat state before deciding this attempt's eligibility.
static s32 timing_tainted(void) {
    return game_options_secrets_blocked();
}

void game_timing_runtime_entered(void) {
    if (game_options_runtime_allowed() && game_timing_state()->file == game_options_active_file()) {
        game_timing_level_entered(gCurrCourseNum, gCurrLevelNum, timing_tainted());
    }
}

void game_timing_runtime_update(s32 running) {
    if (!game_options_runtime_allowed()) {
        return;
    }
    s32 marioFrozen = (gTimeStopState & TIME_STOP_ACTIVE)
                      && (gTimeStopState & (TIME_STOP_MARIO_AND_DOORS | TIME_STOP_ALL_OBJECTS));
    running = running && gCurrentArea && gMarioState && gMarioState->action != ACT_UNINITIALIZED
              && !marioFrozen
              && (!(gMarioState->action & ACT_FLAG_INTANGIBLE)
                  || (gMarioState->action == ACT_IN_CANNON
                      && gMarioState->actionState == ACT_STATE_IN_CANNON_READY))
              && gMarioState->health >= 0x100 && !gWarpTransition.isActive
              && get_dialog_id() == DIALOG_NONE;
    game_timing_update(gGlobalTimer, game_options_active_file(), running, timing_tainted());
}

void game_timing_runtime_star(struct Object *obj, u32 star) {
    if (!game_options_runtime_allowed()) {
        return;
    }
    s32 excluded =
        obj_has_model(obj, MODEL_BOWSER_KEY) || (obj->oInteractionSubtype & INT_SUBTYPE_GRAND_STAR);
    s32 course = gCurrCourseNum;
#ifdef GLOBAL_STAR_IDS
    course = star / GAME_TIMING_STARS;
    star %= GAME_TIMING_STARS;
#endif
    s32 file = game_options_active_file();
    if (game_timing_collect_star(file, course, star, excluded, timing_tainted())) {
        game_timing_checkpoint(file);
    }
}

void game_timing_render_hud(u8 alpha) {
    const struct GameTimingState *state = game_timing_state();
    if (!alpha || gOptionsMenu.open || !game_options_runtime_allowed()
        || state->file != game_options_active_file()) {
        return;
    }
    s32 course = state->activeCourse && game_options_get(GO_COURSE_TIMER);
    s32 session = game_options_get(GO_SESSION_TIMER);
    if (!course && !session) {
        return;
    }
    char courseText[40], sessionText[40];
    if (course) {
        memcpy(courseText, "Course ", 7);
        game_timing_format(state->courseTicks, courseText + 7);
    }
    if (session) {
        memcpy(sessionText, "Session ", 8);
        game_timing_format(state->sessionTicks, sessionText + 8);
    }
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    // Match the cap timer's down-right shadow and share each pass between rows.
    for (s32 pass = 0; pass < 2; pass++) {
        s16 offset = pass == 0 ? 1 : 0;
        // Center both rows on the neighboring cap timer's baseline.
        s16 y = 22 + (course && session ? 7 : 0);
        gDPPipeSync(gDisplayListHead++);
        if (pass == 0) {
            gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 192 * alpha / 255);
        } else {
            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, alpha);
        }
        if (course) {
            player_stats_menu_text(160 + offset, y - offset, courseText);
            y -= 14;
        }
        if (session) {
            player_stats_menu_text(160 + offset, y - offset, sessionText);
        }
    }
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}
