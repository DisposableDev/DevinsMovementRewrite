#include "game_options_menu.h"
#include "game_timing.h"

#include <string.h>
#include "sm64.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "player_stats_menu.h"
#include "segment2.h"
#include "area.h"
#include "level_table.h"

#define OPTIONS_ROWS 4

// Native input/graphics adapters keep the menu state independent of engine services.
s32 game_options_menu_update_controller(void) {
    static const u16 nativeButtons[] = { U_JPAD,   D_JPAD, L_JPAD, R_JPAD, A_BUTTON,
                                         B_BUTTON, L_TRIG, R_TRIG, Z_TRIG };
    u16 held = 0;
    u16 pressed = 0;
    u8 stick = 0;
    for (s32 i = 0; i < 9; i++) {
        if (gPlayer1Controller->buttonDown & nativeButtons[i]) {
            held |= 1 << i;
        }
        if (gPlayer1Controller->buttonPressed & nativeButtons[i]) {
            pressed |= 1 << i;
        }
    }
    if (gPlayer1Controller->rawStickX < -60) {
        stick |= OM_LEFT;
    } else if (gPlayer1Controller->rawStickX > 60) {
        stick |= OM_RIGHT;
    }
    if (gPlayer1Controller->rawStickY < -60) {
        stick |= OM_DOWN;
    } else if (gPlayer1Controller->rawStickY > 60) {
        stick |= OM_UP;
    }
    return game_options_menu_step(gGlobalTimer, held, pressed, stick);
}

static void render_version(void) {
    player_stats_menu_text(228, 214, "v" GAME_OPTIONS_VERSION_TEXT);
}

static void render_warning(void) {
    s32 debug = gOptionsMenu.warningId == GO_DEBUG_MODE;
    player_stats_menu_text(24, 214, debug ? "Enable debug mode?" : "Enable cheats?");
    render_version();
    if (debug) {
        player_stats_menu_text(24, 181, "While enabled, press D-pad Up + L.");
        player_stats_menu_text(24, 157, "Records and statistics are not");
        player_stats_menu_text(24, 143, "available while in debug mode.");
        player_stats_menu_text(24, 119, "Secrets stay blocked until restart.");
    } else {
        player_stats_menu_text(24, 181, "Records and statistics are not");
        player_stats_menu_text(24, 167, "available while using cheats.");
        player_stats_menu_text(24, 143, "Secrets stay blocked until restart.");
    }
    player_stats_menu_text(24, 31, "Release A, then hold A for 5.0 seconds.");
    player_stats_menu_text(24, 17, "B: Cancel");
    char progress[] = "Holding: 0.0 of 5 seconds";
    u32 frames = game_options_confirmation_progress();
    u32 tenths = frames * 50 / game_options_confirmation_duration();
    progress[9] = '0' + tenths / 10;
    progress[11] = '0' + tenths % 10;
    player_stats_menu_text(138, 17, progress);
}

static void render_best_times(void) {
    s32 course = gOptionsMenu.recordCourse;
    player_stats_menu_text(24, 214, "Best times per star");
    render_version();
    player_stats_menu_text(24, 190, player_stats_menu_course_name(course));
    for (s32 star = 0; star < game_timing_star_count(course); star++) {
        char label[] = "Star 1";
        char time[24];
        label[5] += star;
        u32 ticks = game_timing_best(gOptionsMenu.file, course, star);
        s16 y = 165 - star * 17;
        const char *name = label;
        if (course == COURSE_PSS) {
            name = star ? "Fast slide" : "Slide box";
        } else if (course > COURSE_STAGES_MAX) {
            name = "8 red coins";
        } else if (star == 6) {
            name = "100 coins";
        }
        player_stats_menu_text(24, y, name);
        if (ticks) {
            game_timing_format(ticks, time);
        } else {
            memcpy(time, "--", 3);
        }
        player_stats_menu_text(156, y, time);
    }
    player_stats_menu_text(24, 42,
                           game_timing_storage_failed(gOptionsMenu.file)
                               ? "Record storage unavailable"
                               : "Times exclude pauses and dialogue.");
    player_stats_menu_text(24, 25, "Left-Right: Course   B: Back");
    player_stats_menu_text(24, 9, "Cheat/debug attempts are excluded.");
}

void game_options_menu_render(void) {
    static const char *const categories[GO_CATEGORY_COUNT] = { "Settings",     "Controls",
                                                               "Presentation", "Accessibility",
                                                               "Gameplay",     "Cheats" };
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    if (gOptionsMenu.warning) {
        render_warning();
    } else if (gOptionsMenu.records) {
        render_best_times();
    } else {
        player_stats_menu_text(24, 214, categories[gOptionsMenu.category]);
        render_version();
        s32 first = gOptionsMenu.selected / OPTIONS_ROWS * OPTIONS_ROWS;
        for (s32 row = 0; row < OPTIONS_ROWS && first + row < gOptionsMenu.count; row++) {
            enum GameOptionId id = gOptionsMenu.ids[first + row];
            s32 selected = first + row == gOptionsMenu.selected;
            s32 disabled = id == GO_HELD_TWIRL && !game_options_get(GO_TWIRLS);
            s16 y = 179 - row * 36;
            char value[32];
            game_options_menu_value(id, value);
            if (disabled) {
                gDPSetEnvColor(gDisplayListHead++, 140, 140, 140, gDialogTextAlpha);
            } else if (selected) {
                gDPSetEnvColor(gDisplayListHead++, 255, 225, 96, gDialogTextAlpha);
            } else {
                gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
            }
            player_stats_menu_text(24, y, gGameOptionDefinitions[id].label);
            player_stats_menu_text(40, y - 16, disabled ? "Unavailable here" : value);
            if (selected) {
                player_stats_menu_text(14, y, "*");
            }
        }
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
        enum GameOptionId selectedId = gOptionsMenu.ids[gOptionsMenu.selected];
        if (game_options_requires_restart(selectedId)
            && game_options_get(selectedId) != game_options_get_file(gOptionsMenu.file, selectedId)) {
            player_stats_menu_text(24, 43, "Change applies on course restart.");
        }
        player_stats_menu_text(24, 25, "L-R: Category  Z: Default  B: Back");
        player_stats_menu_text(24, 9, "Up-Down: Select    Left-Right or A: Change");
    }
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}
