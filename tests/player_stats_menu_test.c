#include "stats_menu_world.h"
#include "player_stats.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

s32 game_options_is_cheating(void) {
    return FALSE;
}

struct Controller sController;
struct Controller *gPlayer1Controller = &sController;
u32 gGlobalTimer;
s16 gCurrSaveFileNum = 3;
static s32 sHeThiccSave = TRUE;
#ifdef INPUT_DISPLAY
static s32 sInputEnabled = TRUE;
#endif

s32 save_file_is_he_thicc(s32 fileIndex) {
    return fileIndex == 2 && sHeThiccSave;
}
f32 gGlobalSoundSource[3];
u16 gDialogTextAlpha = 250;
static int sDisplayList[16384];
int *gDisplayListHead = sDisplayList;
int dl_ia_text_begin, dl_ia_text_end, dl_draw_triangle;
static s32 sTextCount;
static s32 sTextPipeline;
static s32 sSounds;
static s32 sRecordLocation;
static s32 sLotsTextCount;
static f32 sTextScale = 1.0f;
static s16 sTextOriginX, sTextOriginY;

#include "game_options.h"
#include "menu_services.inc.c"
#include "player_stats_menu.c"

s32 input_display_enabled(void) {
#ifdef INPUT_DISPLAY
    return sInputEnabled;
#else
    return FALSE;
#endif
}

void stats_menu_display_list(int *command, int *list) {
    (void) command;
    if (list == &dl_ia_text_begin) {
        sTextPipeline = TRUE;
    } else if (list == &dl_ia_text_end || list == &dl_draw_triangle) {
        sTextPipeline = FALSE;
    }
}

void create_dl_translation_matrix(s8 op, f32 x, f32 y, f32 z) {
    (void) op;
    (void) z;
    if (sTextPipeline) {
        sTextOriginX = x;
        sTextOriginY = y;
    }
}

void create_dl_scale_matrix(s8 op, f32 x, f32 y, f32 z) {
    (void) op;
    assert(y == 1.0f && z == 1.0f && x > 0.0f && x < 1.0f);
    sTextScale = x;
}

void stats_menu_pop_matrix(void) {
    sTextScale = 1.0f;
    sTextOriginX = sTextOriginY = 0;
}

s32 get_string_width(u8 *str) {
    s32 width = 0;
    while (*str != DIALOG_CHAR_TERMINATOR) {
        width += gDialogCharWidths[*str++];
    }
    return width;
}

void play_sound(u32 sound, f32 *source) {
    (void) sound;
    (void) source;
    sSounds++;
}

void print_generic_string(s16 x, s16 y, const u8 *text) {
    assert(sTextPipeline);
    x += sTextOriginX;
    y += sTextOriginY;
    s32 width = 0;
    s32 index = 0;
    while (text[index] != DIALOG_CHAR_TERMINATOR) {
        assert(index < STATS_MENU_TEXT_SIZE - 1);
        width += gDialogCharWidths[text[index++]];
    }
    if (index >= 4 && text[0] == ASCII_TO_DIALOG('L') && text[1] == ASCII_TO_DIALOG('o')
        && text[2] == ASCII_TO_DIALOG('t') && text[3] == ASCII_TO_DIALOG('s')) {
        assert(index == 5 && text[4] == 0xF2); // Native exclamation mark, not the digit zero.
        sLotsTextCount++;
    }
    if (x < 8 || x + width * sTextScale > 312.01f) {
        fprintf(stderr, "Statistics text overflow: x=%d y=%d width=%d glyphs=%d\n", x, y, width, index);
        assert(FALSE);
    }
    assert(y >= 8 && y <= 226);
    if (y == 22 || y == 8) {
        assert(x == STATS_MENU_LEFT_X || x == STATS_MENU_RIGHT_X);
        assert(x + width * sTextScale <= (x == STATS_MENU_LEFT_X ? STATS_MENU_RIGHT_X - 8 : 312));
        for (s32 glyph = 0; glyph < index; glyph++) {
            assert(text[glyph] != DIALOG_CHAR_SLASH);
        }
    }
    if (y == 36) {
        sRecordLocation++;
    }
    sTextCount++;
}

static void input(u16 down, u16 pressed) {
    sController.buttonDown = down;
    sController.buttonPressed = pressed;
    gGlobalTimer++;
    assert(!player_stats_menu_update());
}

static void formats(void) {
    assert(menu_glyph('+') == DIALOG_CHAR_PLUS);
    assert(gDialogCharWidths[DIALOG_CHAR_PLUS] == 7);
    char output[STATS_MENU_TEXT_SIZE];
    format_value(PS_TOTAL_JUMPS_PERFORMED, ~(u64) 0 - 1, output);
    assert(strcmp(output, "18,446,744,073,709,551,614") == 0);
    format_value(PS_TOTAL_PLAY_TIME, (u64) PLAYER_STATS_FRAMES_PER_SECOND * 3661, output);
    assert(strcmp(output, "1:01:01") == 0);
    format_value(PS_TOTAL_PLAY_TIME, PLAYER_STATS_FRAMES_PER_SECOND - 1, output);
    assert(strcmp(output, "0:00:00") == 0);
    format_value(PS_SECONDS_SOAKED, PLAYER_STATS_FRAMES_PER_SECOND * 5, output);
    assert(strcmp(output, "5 s") == 0);
    format_value(PS_SECONDS_SOAKED, PLAYER_STATS_FRAMES_PER_SECOND - 1, output);
    assert(strcmp(output, "0 s") == 0);
    format_value(PS_TOTAL_DISTANCE_TRAVELED, 1234 * 256 + 128, output);
    assert(strcmp(output, "1,234.5 units") == 0);
    format_value(PS_FASTEST_GROUND_SPEED, 33 * 256, output);
    assert(strcmp(output, "33.0 units per frame") == 0);
    format_value(PS_POWER_METER_WEDGES_LOST, 64, output);
    assert(strcmp(output, "0.25 wedges") == 0);
    for (s32 id = 1; id < PLAYER_STAT_COUNT; id++) {
        format_value(id, ~(u64) 0, output);
        assert(strcmp(output, "Lots!") == 0);
    }
    player_stats_select_file(0);
    gPlayerStats[0].values[PS_TOTAL_JUMPS_PERFORMED] = ~(u64) 0 - 2;
    player_stats_add(PS_TOTAL_JUMPS_PERFORMED, 1);
    format_value(PS_TOTAL_JUMPS_PERFORMED, gPlayerStats[0].values[PS_TOTAL_JUMPS_PERFORMED], output);
    assert(strcmp(output, "18,446,744,073,709,551,614") == 0);
    player_stats_add(PS_TOTAL_JUMPS_PERFORMED, 1);
    format_value(PS_TOTAL_JUMPS_PERFORMED, gPlayerStats[0].values[PS_TOTAL_JUMPS_PERFORMED], output);
    assert(strcmp(output, "Lots!") == 0);
    gPlayerStats[0].values[PS_TOTAL_JUMPS_PERFORMED] = ~(u64) 0 - 2;
    player_stats_add(PS_TOTAL_JUMPS_PERFORMED, 3);
    format_value(PS_TOTAL_JUMPS_PERFORMED, gPlayerStats[0].values[PS_TOTAL_JUMPS_PERFORMED], output);
    assert(strcmp(output, "Lots!") == 0);
}

static void opening_category_buttons(void) {
    const u16 held[] = { L_TRIG, R_TRIG, L_TRIG | R_TRIG };
    gCurrSaveFileNum = 3;
    sHeThiccSave = TRUE;
    game_options_select_file(2);
    player_stats_select_file(2);
    game_options_set(GO_MENU_NAVIGATION, 2);
    sController.rawStickX = sController.rawStickY = 0;
    for (u32 i = 0; i < sizeof(held) / sizeof(held[0]); i++) {
        sController.buttonDown = held[i];
        sController.buttonPressed = 0;
        player_stats_menu_open();
        s32 sounds = sSounds;
        input(held[i], 0);
        assert(sStatsMenu.category == PS_CATEGORY_GENERAL && sSounds == sounds);
        input(0, 0);
        input(L_TRIG, L_TRIG);
        assert(sStatsMenu.category == PS_CATEGORY_COUNT - 1 && sSounds == sounds + 1);
        input(L_TRIG, 0);
        input(L_TRIG | R_TRIG, R_TRIG);
        input(L_TRIG, 0);
        assert(sStatsMenu.category == PS_CATEGORY_COUNT - 1 && sSounds == sounds + 1);
        input(0, 0);
        input(R_TRIG, R_TRIG);
        assert(sStatsMenu.category == PS_CATEGORY_GENERAL && sSounds == sounds + 2);
        sController.buttonDown = B_BUTTON | R_TRIG;
        sController.buttonPressed = B_BUTTON;
        gGlobalTimer++;
        assert(player_stats_menu_update());
        player_stats_menu_open();
        input(R_TRIG, 0);
        assert(sStatsMenu.category == PS_CATEGORY_GENERAL && sSounds == sounds + 2);
    }
    for (s32 release = 0; release <= 1; release++) {
        game_options_set(GO_MENU_STICK_RELEASE, release);
        sController.rawStickY = -80;
        sController.buttonDown = sController.buttonPressed = 0;
        player_stats_menu_open();
        input(0, 0);
        assert(sStatsMenu.selected == 0);
        sController.rawStickY = 0;
        input(0, 0);
        sController.rawStickY = -80;
        input(0, 0);
        assert(sStatsMenu.selected == 1);
        sController.rawStickY = 0;
        input(0, 0);
    }
}

static void navigation(void) {
    player_stats_menu_open();
    s32 initialSounds = sSounds;
    input(L_JPAD, L_JPAD);
    input(0, 0);
    input(R_JPAD, R_JPAD);
    assert(sStatsMenu.category == 0 && sSounds == initialSounds);
    for (s32 direction = -1; direction <= 1; direction += 2) {
        sController.rawStickX = direction * 80;
        for (s32 frame = 0; frame < 40; frame++) {
            input(0, 0);
            assert(sStatsMenu.category == 0 && sSounds == initialSounds);
        }
    }
    sController.rawStickX = 0;
    input(0, 0);
    input(L_TRIG, L_TRIG);
    assert(sStatsMenu.category == PS_CATEGORY_COUNT - 1);
    assert(sSounds == initialSounds + 1);
    for (s32 frame = 0; frame < 80; frame++) {
        input(L_TRIG, 0);
        assert(sStatsMenu.category == PS_CATEGORY_COUNT - 1);
        assert(sSounds == initialSounds + 1);
    }
    s32 sounds = sSounds;
    assert(!player_stats_menu_update());
    assert(sSounds == sounds);
    input(0, 0);
    input(R_TRIG, R_TRIG);
    assert(sStatsMenu.category == 0);
    assert(sSounds == initialSounds + 2);
    for (s32 frame = 0; frame < 80; frame++) {
        input(R_TRIG, 0);
        assert(sStatsMenu.category == 0 && sSounds == initialSounds + 2);
    }
    sounds = sSounds;
    input(0, 0);
    input(D_JPAD, D_JPAD);
    assert(sStatsMenu.selected == 1);
    for (s32 frame = 0; frame < STATS_MENU_REPEAT_DELAY_FRAMES - 1; frame++) {
        input(D_JPAD, 0);
        assert(sStatsMenu.selected == 1);
    }
    input(D_JPAD, 0);
    assert(sStatsMenu.selected == 2);
    for (s32 frame = 0; frame < STATS_MENU_REPEAT_INTERVAL_FRAMES - 1; frame++) {
        input(D_JPAD, 0);
        assert(sStatsMenu.selected == 2);
    }
    input(D_JPAD, 0);
    assert(sStatsMenu.selected == 3);
    for (s32 frame = 0; frame < 160; frame++) {
        input(D_JPAD, 0);
    }
    assert(sStatsMenu.selected == sStatsMenu.count - 1);
    input(0, 0);
    input(D_JPAD, D_JPAD);
    assert(sStatsMenu.selected == 0);
    input(0, 0);
    input(U_JPAD, U_JPAD);
    assert(sStatsMenu.selected == sStatsMenu.count - 1);
    input(0, 0);
    input(U_JPAD, U_JPAD);
    assert(sStatsMenu.selected == sStatsMenu.count - 2);
    input(0, 0);
    input(D_JPAD, D_JPAD);
    assert(sSounds == sounds);
    input(0, 0);
    input(L_TRIG | R_TRIG | U_JPAD | D_JPAD, L_TRIG | R_TRIG);
    assert(sStatsMenu.category == 0 && sStatsMenu.selected == sStatsMenu.count - 1);
    for (s32 frame = 0; frame < 80; frame++) {
        input(L_TRIG | R_TRIG, 0);
        assert(sStatsMenu.category == 0 && sSounds == sounds);
    }
    input(R_TRIG, 0);
    assert(sStatsMenu.category == 0 && sSounds == sounds);
    input(0, 0);
    input(R_TRIG | D_JPAD, R_TRIG);
    assert(sStatsMenu.category == 1 && sStatsMenu.selected == 0);
    assert(sSounds == sounds + 1);
    sounds = sSounds;
    sController.buttonDown = sController.buttonPressed = B_BUTTON;
    gGlobalTimer++;
    assert(player_stats_menu_update());
    assert(sSounds == sounds);
    assert(!player_stats_menu_update());
}

static void stick_browsing(void) {
    player_stats_menu_open();
    sController.rawStickY = -80;
    input(0, 0);
    assert(sStatsMenu.selected == 1);
    for (s32 frame = 0; frame < STATS_MENU_REPEAT_DELAY_FRAMES - 1; frame++) {
        input(0, 0);
        assert(sStatsMenu.selected == 1);
    }
    input(0, 0);
    assert(sStatsMenu.selected == 2);
    for (s32 frame = 0; frame < STATS_MENU_REPEAT_INTERVAL_FRAMES - 1; frame++) {
        input(0, 0);
        assert(sStatsMenu.selected == 2);
    }
    input(0, 0);
    assert(sStatsMenu.selected == 3);
    sController.rawStickY = 0;
    input(0, 0);
    sController.rawStickY = 80;
    input(0, 0);
    assert(sStatsMenu.selected == 2);
    for (s32 frame = 0; frame < 160; frame++) {
        input(0, 0);
    }
    assert(sStatsMenu.selected == 0);
    sController.rawStickY = 0;
    input(0, 0);
    sController.rawStickY = 80;
    input(0, 0);
    assert(sStatsMenu.selected == sStatsMenu.count - 1);
    sController.rawStickY = -80;
    input(0, 0);
    assert(sStatsMenu.selected == 0);
    for (s32 frame = 0; frame < 160; frame++) {
        input(0, 0);
    }
    assert(sStatsMenu.selected == sStatsMenu.count - 1);
    // A fresh D-pad press still wraps when the stick holds the same direction.
    input(D_JPAD, D_JPAD);
    assert(sStatsMenu.selected == 0);
    sController.rawStickY = 0;
}

static void navigation_policies(void) {
    for (s32 policy = 0; policy < 3; policy++) {
        game_options_set(GO_MENU_NAVIGATION, policy);
        sController.rawStickX = sController.rawStickY = 0;
        player_stats_menu_open();
        input(D_JPAD, D_JPAD);
        assert(sStatsMenu.selected == (policy == 0 ? 0 : 1));
        input(0, 0);
        player_stats_menu_open();
        sController.rawStickY = -64;
        input(0, 0);
        assert(sStatsMenu.selected == (policy == 1 ? 0 : 1));
    }
    game_options_set(GO_MENU_NAVIGATION, 2);
    game_options_set(GO_MENU_STICK_RELEASE, 1);
    sController.rawStickY = 0;
    player_stats_menu_open();
    sController.rawStickY = -64;
    input(0, 0);
    assert(sStatsMenu.selected == 1);
    sController.rawStickY = 64;
    for (s32 frame = 0; frame < 30; frame++) {
        input(0, 0);
        assert(sStatsMenu.selected == 1);
    }
    sController.rawStickY = 0;
    sController.rawStickX = 64;
    input(0, 0);
    sController.rawStickX = 0;
    sController.rawStickY = 64;
    input(0, 0);
    assert(sStatsMenu.selected == 1);
    input(U_JPAD, U_JPAD);
    assert(sStatsMenu.selected == 1); // A held stick cannot hand off directly to D-pad.
    sController.rawStickY = 0;
    input(0, 0);
    input(U_JPAD, U_JPAD);
    assert(sStatsMenu.selected == 0);
    for (s32 frame = 0; frame < 30; frame++) {
        input(U_JPAD, 0);
        assert(sStatsMenu.selected == 0);
    }
    input(D_JPAD, D_JPAD);
    assert(sStatsMenu.selected == 0);
    input(0, 0);
    sController.rawStickY = -64;
    input(0, 0);
    assert(sStatsMenu.selected == 1);
    sController.rawStickY = 0;
    game_options_set(GO_MENU_STICK_RELEASE, 0);
}

static void save_category_visibility(void) {
    for (s32 committed = FALSE; committed <= TRUE; committed++) {
        sHeThiccSave = committed;
        for (gCurrSaveFileNum = 1; gCurrSaveFileNum <= 4; gCurrSaveFileNum++) {
            s32 categoryCount = committed && gCurrSaveFileNum == 3 ? 8 : 7;
            player_stats_menu_open();
            input(L_TRIG, L_TRIG);
            assert(sStatsMenu.category == categoryCount - 1);
            input(0, 0);
            input(R_TRIG, R_TRIG);
            assert(sStatsMenu.category == 0);
            for (s32 category = 1; category <= categoryCount; category++) {
                input(0, 0);
                input(R_TRIG, R_TRIG);
                assert(sStatsMenu.category == category % categoryCount);
                for (s32 index = 0; index < sStatsMenu.count; index++) {
                    assert(categoryCount == 8 || gPlayerStatInfo[sStatsMenu.ids[index]].category != 7);
                }
            }
        }
    }
    gCurrSaveFileNum = 3;
    sHeThiccSave = TRUE;
}

static void new_titles_fit(void) {
    // Use the narrow column even in non-overlay builds, with the native font widths.
    sTextPipeline = TRUE;
    for (s32 id = PS_LONGEST_DAMAGE_FREE_STREAK; id <= PS_TIME_USING_VANISH_CAP; id++) {
        assert(gPlayerStatInfo[id].label != NULL);
        player_stats_menu_text(INPUT_DISPLAY_CONTENT_LEFT, 174, gPlayerStatInfo[id].label);
        assert(sTextScale == 1.0f);
    }
    sTextPipeline = FALSE;
}

static void catalog_layout(void) {
    u8 seen[PLAYER_STAT_COUNT] = { 0 };
    struct PlayerStatsData before[PLAYER_STATS_SAVE_FILES];
    s32 category;
    s32 index;
    s32 id;
    for (id = 1; id < PLAYER_STAT_COUNT; id++) {
        gPlayerStats[2].values[id] = ~(u64) 0;
        gPlayerStats[2].levels[id] = LEVEL_PHYSICS_LAB;
        gPlayerStats[2].areas[id] = 2;
    }
    player_stats_select_file(2);
    memcpy(before, gPlayerStats, sizeof(before));
    for (category = 0; category < PS_CATEGORY_COUNT; category++) {
        select_category(category);
        assert(sStatsMenu.count > 0);
        for (index = 0; index < sStatsMenu.count; index++) {
            id = sStatsMenu.ids[index];
            assert(!seen[id]);
            seen[id] = TRUE;
            sStatsMenu.selected = index;
            player_stats_menu_render();
        }
    }
    for (id = 1; id < PLAYER_STAT_COUNT; id++) {
        assert(seen[id]);
    }
    assert(sRecordLocation > 0 && sTextCount > PLAYER_STAT_COUNT);
    assert(sLotsTextCount > PLAYER_STAT_COUNT);
    assert(memcmp(before, gPlayerStats, sizeof(before)) == 0);
    assert(!player_stats_is_dirty(2));
    player_stats_select_file(-1);
    player_stats_menu_render();
    assert(memcmp(before, gPlayerStats, sizeof(before)) == 0);
}

static void conflicting_and_opening_directions(void) {
    game_options_set(GO_MENU_NAVIGATION, 2);
    game_options_set(GO_MENU_STICK_RELEASE, 0);
    sController = (struct Controller){ 0 };
    player_stats_menu_open();
    input(D_JPAD | L_JPAD, D_JPAD | L_JPAD);
    assert(sStatsMenu.selected == 0);
    input(D_JPAD, 0);
    assert(sStatsMenu.selected == 0);
    input(0, 0);
    input(D_JPAD, D_JPAD);
    assert(sStatsMenu.selected == 1);
    for (s32 frame = 0; frame < 40; frame++) {
        input(D_JPAD | L_JPAD, L_JPAD);
        assert(sStatsMenu.selected == 1);
    }
    input(0, 0);
    sController.rawStickY = -80;
    sController.rawStickX = 80;
    input(0, 0);
    assert(sStatsMenu.selected == 1);
    sController.rawStickX = 0;
    input(0, 0);
    assert(sStatsMenu.selected == 1);
    sController.rawStickY = 0;
    input(0, 0);
    sController.rawStickY = -80;
    player_stats_menu_open();
    for (s32 frame = 0; frame < 40; frame++) {
        input(0, 0);
        assert(sStatsMenu.selected == 0);
    }
    sController.rawStickY = 0;
    input(0, 0);
    sController.rawStickY = -80;
    input(0, 0);
    assert(sStatsMenu.selected == 1);

    // Production pause/dialog/star-selection scrolling uses the same conflict policy.
    for (s32 axis = MENU_SCROLL_VERTICAL; axis <= MENU_SCROLL_HORIZONTAL; axis++) {
        s8 selected = 1;
        sController = (struct Controller){ 0 };
        sMenuNavigationStick = GO_MENU_WAIT_FOR_NEUTRAL;
        sMenuNavigationDirection = gMenuHoldKeyIndex = 0;
        if (axis == MENU_SCROLL_VERTICAL) {
            sController.rawStickY = -80;
        } else {
            sController.rawStickX = 80;
        }
        for (s32 frame = 0; frame < 40; frame++) {
            handle_menu_scrolling(axis, &selected, 1, 3);
            assert(selected == 1);
        }
        sController = (struct Controller){ 0 };
        handle_menu_scrolling(axis, &selected, 1, 3);
        sController.buttonDown = axis == MENU_SCROLL_VERTICAL ? D_JPAD : R_JPAD;
        handle_menu_scrolling(axis, &selected, 1, 3);
        assert(selected == 2);
        sController.buttonDown |= axis == MENU_SCROLL_VERTICAL ? L_JPAD : D_JPAD;
        handle_menu_scrolling(axis, &selected, 1, 3);
        assert(selected == 2);
        sController.buttonDown = axis == MENU_SCROLL_VERTICAL ? D_JPAD : R_JPAD;
        handle_menu_scrolling(axis, &selected, 1, 3);
        assert(selected == 2);
        sController = (struct Controller){ 0 };
        handle_menu_scrolling(axis, &selected, 1, 3);
        sController.rawStickX = sController.rawStickY = -80;
        handle_menu_scrolling(axis, &selected, 1, 3);
        assert(selected == 2);
        sController = (struct Controller){ 0 };
        handle_menu_scrolling(axis, &selected, 1, 3);
        sController.buttonDown = axis == MENU_SCROLL_VERTICAL ? D_JPAD : R_JPAD;
        handle_menu_scrolling(axis, &selected, 1, 3);
        assert(selected == 3);
    }
    sController = (struct Controller){ 0 };
}

static void navigation_between_contexts(void) {
    for (s32 context = 0; context < 6; context++) {
        for (s32 conflict = 0; conflict < 2; conflict++) {
            s8 selected = 1;
            reset_menu_navigation();
            sController = (struct Controller){ .buttonDown = D_JPAD };
            if (conflict) {
                sController.buttonDown |= L_JPAD;
            }
            handle_menu_scrolling(MENU_SCROLL_VERTICAL, &selected, 1, 3);
            assert(selected == (conflict ? 1 : 2));

            // Release during gameplay: scrolling does not observe these neutral frames.
            sController = (struct Controller){ 0 };
            gDialogID = DIALOG_NONE;
            gMenuMode = MENU_MODE_NONE;
            switch (context) {
                case 0:
                    create_dialog_box(5);
                    break;
                case 1:
                    create_dialog_box_with_var(5, 2);
                    break;
                case 2:
                    create_dialog_inverted_box(5);
                    break;
                case 3:
                    create_dialog_box_with_response(5);
                    break;
                case 4:
                    set_menu_mode(MENU_MODE_RENDER_PAUSE_SCREEN);
                    break;
                case 5:
                    reset_dialog_render_state();
                    break;
            }
            selected = 1;
            sController.buttonDown = D_JPAD;
            handle_menu_scrolling(MENU_SCROLL_VERTICAL, &selected, 1, 3);
            assert(selected == 2);

            // The new context still blocks conflicts until every direction is released.
            sController.buttonDown |= L_JPAD;
            handle_menu_scrolling(MENU_SCROLL_VERTICAL, &selected, 1, 3);
            sController.buttonDown = D_JPAD;
            handle_menu_scrolling(MENU_SCROLL_VERTICAL, &selected, 1, 3);
            assert(selected == 2);
            sController.buttonDown = 0;
            handle_menu_scrolling(MENU_SCROLL_VERTICAL, &selected, 1, 3);
            sController.buttonDown = D_JPAD;
            handle_menu_scrolling(MENU_SCROLL_VERTICAL, &selected, 1, 3);
            assert(selected == 3);
        }
    }
    sController = (struct Controller){ 0 };
}

int main(void) {
    game_options_init(NULL, NULL);
    game_options_select_file(0);
    player_stats_init(NULL, NULL);
    formats();
    save_category_visibility();
    navigation();
#ifdef INPUT_DISPLAY
    sInputEnabled = FALSE;
    navigation();
#endif
    stick_browsing();
    navigation_policies();
    conflicting_and_opening_directions();
    navigation_between_contexts();
    new_titles_fit();
    catalog_layout();
#ifdef INPUT_DISPLAY
    sInputEnabled = TRUE;
    catalog_layout();
#endif
    opening_category_buttons();
    puts("Statistics menu formats, navigation, all labels and read-only rendering passed.");
    return 0;
}
