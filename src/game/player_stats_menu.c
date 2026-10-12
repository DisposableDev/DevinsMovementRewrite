#include "player_stats_menu.h"

#include <string.h>
#include "sm64.h"
#include "config.h"
#include "audio/external.h"
#include "area.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "level_table.h"
#include "player_stats.h"
#include "save_file.h"
#include "segment2.h"
#include "input_display.h"
#include "game_options.h"

#define STATS_MENU_ROWS 5
#define STATS_MENU_TEXT_SIZE 96
#define STATS_MENU_REPEAT_DELAY_FRAMES 10
#define STATS_MENU_REPEAT_INTERVAL_FRAMES 5

#ifdef INPUT_DISPLAY
#define STATS_MENU_LEFT_X (input_display_enabled() ? INPUT_DISPLAY_CONTENT_LEFT : 24)
#define STATS_MENU_RIGHT_X (input_display_enabled() ? 216 : 164)
#define STATS_MENU_TITLE_X (input_display_enabled() ? 180 : 132)
#else
#define STATS_MENU_LEFT_X 24
#define STATS_MENU_RIGHT_X 164
#define STATS_MENU_TITLE_X 132
#endif

_Static_assert(PLAYER_STAT_COUNT <= 256, "Statistics menu IDs must fit in a byte");

static const char *const sCategoryNames[PS_CATEGORY_COUNT] = {
    [PS_CATEGORY_GENERAL] = "General",
    [PS_CATEGORY_COLLECTIBLES] = "Collectibles",
    [PS_CATEGORY_MOVES] = "Moves",
    [PS_CATEGORY_DISTANCE] = "Distance and Records",
    [PS_CATEGORY_COMBAT] = "Combat",
    [PS_CATEGORY_DAMAGE] = "Damage and Recovery",
    [PS_CATEGORY_WORLD] = "World Interactions",
    [PS_CATEGORY_HE_THICC] = "HE THICC",
};

// Presentation order is independent of the permanent IDs used in saved totals.
static const u8 sStatDisplayOrder[] = {
    // General: time, then course visits.
    PS_TOTAL_PLAY_TIME,
    PS_TIME_SPENT_IN_PEACH_S_CASTLE,
    PS_TIME_SPENT_IN_COURSES,
    PS_TIME_SPENT_IN_DIALOGUE,
    PS_COURSE_ENTRIES,
    PS_COURSE_EXITS_WITHOUT_COLLECTING_A_STAR,

    // Collectibles: stars, coins, lives and power caps.
    PS_TOTAL_POWER_STARS_COLLECTED,
    PS_UNIQUE_POWER_STARS_COLLECTED,
    PS_PREVIOUSLY_COLLECTED_STARS_RECOLLECTED,
    PS_STARS_COLLECTED_WITHOUT_DAMAGE,
    PS_100_COIN_STARS_COLLECTED,
    PS_RED_COIN_STARS_COLLECTED,
    PS_CASTLE_SECRET_STARS_COLLECTED,
    PS_TOTAL_COIN_VALUE_COLLECTED,
    PS_YELLOW_COINS_COLLECTED,
    PS_RED_COINS_COLLECTED,
    PS_BLUE_COINS_COLLECTED,
    PS_HIGHEST_COIN_TOTAL_IN_A_SINGLE_COURSE_VISIT,
    PS_1_UP_MUSHROOMS_COLLECTED,
    PS_WING_CAPS_COLLECTED,
    PS_METAL_CAPS_COLLECTED,
    PS_VANISH_CAPS_COLLECTED,
    PS_TIME_USING_WING_CAP,
    PS_TIME_USING_METAL_CAP,
    PS_TIME_USING_VANISH_CAP,

    // Moves: jumps, attacks, traversal, twirls, breakdance and dabs.
    PS_TOTAL_JUMPS_PERFORMED,
    PS_DOUBLE_JUMPS,
    PS_TRIPLE_JUMPS,
    PS_LONG_JUMPS,
    PS_BACKFLIPS,
    PS_SIDE_SOMERSAULTS,
    PS_WALL_KICKS,
    PS_GROUND_POUNDS,
    PS_DIVES,
    PS_DIVE_ROLLOUTS,
    PS_JUMP_KICKS,
    PS_SLIDE_KICKS,
    PS_SLIDE_KICK_LIQUID_BOUNCES,
    PS_PUNCHES_THROWN,
    PS_GROUND_KICKS_THROWN,
    PS_CRAWLS_STARTED,
    PS_LEDGE_GRABS,
    PS_POLE_GRABS,
    PS_SWIMMING_STROKES,
    PS_DOWNWARD_SWIMMING_BURSTS,
    PS_WATER_JUMPS,
    PS_GROUND_TWIRLS,
    PS_AIR_TWIRLS,
    PS_TWIRL_WALL_REBOUNDS,
    PS_GROUND_TWIRL_WALL_REBOUNDS,
    PS_AIR_TWIRL_WALL_REBOUNDS,
    PS_LONGEST_TWIRL_WALL_REBOUND_CHAIN,
    PS_TIME_HOLDING_TWIRL_POSE,
    PS_BREAKDANCE_SPINS,
    PS_POWER_CAPS_SHED_BY_BREAKDANCE,
    PS_DABS_PERFORMED,
    PS_TIME_SPENT_DABBING,
    PS_DAB_INDUCED_SLEEPS,

    // Distance and Records: paired distance totals and related movement records.
    PS_TOTAL_DISTANCE_TRAVELED,
    PS_TOTAL_DISTANCE_TRAVELED_3D,
    PS_DISTANCE_TRAVELED_ON_FOOT,
    PS_DISTANCE_TRAVELED_ON_FOOT_3D,
    PS_FASTEST_GROUND_SPEED,
    PS_DISTANCE_TRAVELED_IN_THE_AIR,
    PS_DISTANCE_TRAVELED_IN_THE_AIR_3D,
    PS_FASTEST_AIRBORNE_SPEED,
    PS_LONGEST_SINGLE_LONG_JUMP,
    PS_FARTHEST_CANNON_FLIGHT,
    PS_LONGEST_AIR_TIME_WITHOUT_WING_CAP,
    PS_LONGEST_WALL_KICK_CHAIN,
    PS_GREATEST_FALL_SURVIVED,
    PS_DISTANCE_CRAWLED,
    PS_DISTANCE_CRAWLED_3D,
    PS_DISTANCE_SWUM,
    PS_DISTANCE_SWUM_3D,
    PS_DISTANCE_WATER_WALKED,
    PS_DISTANCE_WATER_WALKED_3D,
    PS_DISTANCE_TRAVELED_WHILE_SOAKED,
    PS_TIME_WATER_WALKING,
    PS_LONGEST_CONTINUOUS_WATER_WALK,
    PS_DISTANCE_SLID,
    PS_DISTANCE_SLID_3D,
    PS_LONGEST_CONTINUOUS_SLIDE,
    PS_DISTANCE_FLOWN,
    PS_DISTANCE_FLOWN_3D,
    PS_DISTANCE_TRAVELED_RIDING_SHELLS,
    PS_DISTANCE_TRAVELED_RIDING_SHELLS_3D,
    PS_LONGEST_CONTINUOUS_SHELL_RIDE,

    // Combat: defeat methods, enemy types, then all Bowser results.
    PS_TOTAL_ENEMIES_DEFEATED,
    PS_MOST_ENEMIES_DEFEATED_IN_ONE_VISIT,
    PS_ENEMIES_STOMPED,
    PS_ENEMY_BOUNCE_STREAK,
    PS_ENEMIES_PUNCHED,
    PS_ENEMIES_KICKED,
    PS_BREAKDANCE_DEFEATS,
    PS_THROWN_OBJECT_DEFEATS,
    PS_ENEMIES_GROUND_POUNDED,
    PS_ENEMIES_DEFEATED_WHILE_RIDING_SHELLS,
    PS_TWIRL_ENEMY_HITS,
    PS_ENEMIES_STUNNED_BY_TWIRLS,
    PS_ENEMIES_DEFEATED_BY_PASSIVE_METAL_CONTACT,
    PS_GOOMBAS_DEFEATED,
    PS_BOOS_DEFEATED,
    PS_BULLIES_KNOCKED_INTO_LAVA,
    PS_BOWSER_TAIL_GRABS,
    PS_BOWSER_THROWS,
    PS_BOWSER_BOMB_HITS,
    PS_MISSED_BOWSER_THROWS,
    PS_BOWSER_TIMED_ESCAPES,
    PS_DEFEATED_BY_BOWSER_1,
    PS_DEFEATED_BY_BOWSER_2,
    PS_DEFEATED_BY_BOWSER_3,

    // Damage and Recovery: health, hazards, then deaths and game overs.
    PS_HITS_TAKEN,
    PS_LONGEST_DAMAGE_FREE_STREAK,
    PS_SECONDS_SOAKED,
    PS_POWER_METER_WEDGES_LOST,
    PS_POWER_METER_WEDGES_RESTORED,
    PS_CAP_POWERS_LOST_TO_DAMAGE,
    PS_WALL_BONKS,
    PS_FALL_DAMAGE_INCIDENTS,
    PS_LAVA_BURNS,
    PS_FIRE_BURNS,
    PS_ELECTRIC_SHOCKS,
    PS_TIMES_CRUSHED,
    PS_LIVES_LOST,
    PS_DROWNING_DEATHS,
    PS_FALLS_INTO_BOTTOMLESS_PITS,
    PS_QUICKSAND_DEATHS,
    PS_GAME_OVERS,

    // World Interactions: travel, objects, conversations, progress and cap recovery.
    PS_CANNON_LAUNCHES,
    PS_HIDDEN_TELEPORTERS_USED,
    PS_WARP_PIPES_ENTERED,
    PS_KOOPA_SHELLS_MOUNTED,
    PS_TREES_CLIMBED,
    PS_OBJECTS_PICKED_UP,
    PS_TIME_HOLDING_OBJECTS,
    PS_OBJECTS_THROWN,
    PS_EXCLAMATION_BLOCKS_BROKEN,
    PS_HIDDEN_SECRETS_ACTIVATED,
    PS_SIGNS_READ,
    PS_TOAD_CONVERSATIONS,
    PS_MIPS_CAUGHT,
    PS_BABY_PENGUINS_RETURNED,
    PS_STAR_DOORS_UNLOCKED,
    PS_RACES_ENTERED,
    PS_RACES_WON,
    PS_TIMES_MARIO_S_CAP_LOST,
    PS_TIMES_MARIO_S_CAP_RECOVERED,
    PS_TIMES_MARIO_FELL_ASLEEP,

    // HE THICC: character time, ground moves, bounces and swimming.
    PS_HE_THICC_PLAY_TIME,
    PS_HE_THICC_CLAPS,
    PS_HE_THICC_WAVEDASHES,
    PS_HE_THICC_BELLY_SLAMS,
    PS_HE_THICC_BELLY_BOUNCES,
    PS_HE_THICC_POUND_BOUNCES,
    PS_HE_THICC_WALL_KICKS,
    PS_HE_THICC_SLIDE_KICK_BOUNCES,
    PS_HE_THICC_EXPLOSIVE_STRIKES,
    PS_HE_THICC_SWIMMING_STROKES,
};

_Static_assert(sizeof(sStatDisplayOrder) == PLAYER_STAT_COUNT - 1,
               "Statistics display order must include every counter");

#define DEFINE_LEVEL(name, level, ...) [level] = name,
#define STUB_LEVEL(name, level, ...) [level] = name,
static const char *const sLevelNames[LEVEL_COUNT] = {
#include "levels/level_defines.h"
};
#undef DEFINE_LEVEL
#undef STUB_LEVEL

const char *player_stats_menu_course_name(s32 course) {
    for (s32 level = LEVEL_MIN; level <= LEVEL_MAX; level++) {
        if (gLevelToCourseNumTable[level - 1] == course && sLevelNames[level]
            && sLevelNames[level][0]) {
            return sLevelNames[level];
        }
    }
    return "Course";
}

static struct {
    u8 category;
    u8 selected;
    u8 count;
    u8 ids[PLAYER_STAT_COUNT];
    u8 heldCategoryButtons;
    u8 heldRowDirections;
    u8 heldStick;
    u8 heldDirection;
    u8 rowRepeatTimer;
    u32 inputFrame;
} sStatsMenu;

static void select_category(u8 category) {
    s32 index;
    sStatsMenu.category = category;
    sStatsMenu.selected = 0;
    sStatsMenu.count = 0;
    for (index = 0; index < PLAYER_STAT_COUNT - 1; index++) {
        u8 id = sStatDisplayOrder[index];
        if (gPlayerStatInfo[id].category == category && gPlayerStatInfo[id].label != NULL) {
            sStatsMenu.ids[sStatsMenu.count++] = id;
        }
    }
}

void player_stats_menu_open(void) {
    memset(&sStatsMenu, 0, sizeof(sStatsMenu));
    sStatsMenu.inputFrame = gGlobalTimer;
    sStatsMenu.heldCategoryButtons = ((gPlayer1Controller->buttonDown & L_TRIG) ? 1 : 0)
                                     | ((gPlayer1Controller->buttonDown & R_TRIG) ? 2 : 0);
    if (gPlayer1Controller->rawStickX < -60 || gPlayer1Controller->rawStickX > 60
        || gPlayer1Controller->rawStickY < -60 || gPlayer1Controller->rawStickY > 60) {
        sStatsMenu.heldStick = GO_MENU_WAIT_FOR_NEUTRAL;
    }
    select_category(PS_CATEGORY_GENERAL);
}

s32 player_stats_menu_update(void) {
    u8 horizontal = 0;
    u8 vertical = 0;
    u8 pressed;
    u8 manual;
    u8 stick = 0;
    u16 buttons = gPlayer1Controller->buttonDown;
    u16 dpad = game_options_menu_dpad(buttons);
    if (sStatsMenu.inputFrame == gGlobalTimer) {
        return FALSE;
    }
    sStatsMenu.inputFrame = gGlobalTimer;
    if (gPlayer1Controller->buttonPressed & B_BUTTON) {
        return TRUE;
    }
    if (buttons & L_TRIG) {
        horizontal |= 1;
    }
    if (buttons & R_TRIG) {
        horizontal |= 2;
    }
    if (gPlayer1Controller->rawStickY > 60) {
        stick = 1;
    } else if (gPlayer1Controller->rawStickY < -60) {
        stick = 2;
    }
    stick |=
        (gPlayer1Controller->rawStickX < -60 ? 4 : 0) | (gPlayer1Controller->rawStickX > 60 ? 8 : 0);
    u8 allowed = game_options_menu_direction(
        (game_options_get(GO_MENU_NAVIGATION) == 1 ? 0 : stick) | ((dpad & U_JPAD) ? 1 : 0)
            | ((dpad & D_JPAD) ? 2 : 0) | ((dpad & L_JPAD) ? 4 : 0) | ((dpad & R_JPAD) ? 8 : 0),
        &sStatsMenu.heldDirection);
    u8 previousStick = sStatsMenu.heldStick;
    stick = game_options_menu_stick(stick, &sStatsMenu.heldStick) & allowed & 3;
    vertical = (stick | ((dpad & U_JPAD) ? 1 : 0) | ((dpad & D_JPAD) ? 2 : 0)) & allowed;
    manual = stick & ~previousStick;
    pressed = horizontal & ~sStatsMenu.heldCategoryButtons;
    sStatsMenu.heldCategoryButtons = horizontal;
    if (horizontal == 3) {
        pressed = 0;
    }
    if (pressed == 1 || pressed == 2) {
        // HE THICC is the final category, gated by the selected save.
        u8 categoryCount = PS_CATEGORY_COUNT - !save_file_is_he_thicc(gCurrSaveFileNum - 1);
        select_category((sStatsMenu.category + (pressed == 1 ? categoryCount - 1 : 1)) % categoryCount);
        play_sound(SOUND_MENU_PAUSE_CHANGE_SELECT, gGlobalSoundSource);
        // A diagonal category change does not also skip the new category's first row.
        sStatsMenu.heldRowDirections = vertical;
        sStatsMenu.rowRepeatTimer = STATS_MENU_REPEAT_DELAY_FRAMES;
        return FALSE;
    }
    pressed = ((vertical ^ sStatsMenu.heldRowDirections) & vertical) | manual;
    if ((allowed & 1) && (game_options_menu_dpad(gPlayer1Controller->buttonPressed) & U_JPAD)) {
        pressed |= 1;
    }
    if ((allowed & 2) && (game_options_menu_dpad(gPlayer1Controller->buttonPressed) & D_JPAD)) {
        pressed |= 2;
    }
    manual = pressed;
    if (vertical == 0) {
        sStatsMenu.rowRepeatTimer = 0;
    } else if (manual || vertical != sStatsMenu.heldRowDirections) {
        sStatsMenu.rowRepeatTimer = STATS_MENU_REPEAT_DELAY_FRAMES;
    } else {
        if (sStatsMenu.rowRepeatTimer > 0) {
            sStatsMenu.rowRepeatTimer--;
        }
        if (sStatsMenu.rowRepeatTimer == 0) {
            pressed = vertical;
            sStatsMenu.rowRepeatTimer = STATS_MENU_REPEAT_INTERVAL_FRAMES;
        }
    }
    sStatsMenu.heldRowDirections = vertical;
    // Only fresh input may cross a list boundary; repeats stop there.
    if (vertical != 3 && pressed == 1) {
        if (sStatsMenu.selected > 0) {
            sStatsMenu.selected--;
        } else if (manual) {
            sStatsMenu.selected = sStatsMenu.count - 1;
        }
    } else if (vertical != 3 && pressed == 2) {
        if (sStatsMenu.selected + 1 < sStatsMenu.count) {
            sStatsMenu.selected++;
        } else if (manual) {
            sStatsMenu.selected = 0;
        }
    }
    return FALSE;
}

static u8 menu_glyph(u8 character) {
    switch (character) {
        case ' ':
            return DIALOG_CHAR_SPACE;
        case '.':
            return 0x3F;
        case ',':
            return DIALOG_CHAR_COMMA;
        case '\'':
            return 0x3E;
        case '-':
            return 0x9F;
        case '+':
            return DIALOG_CHAR_PLUS;
        case '(':
            return 0xE1;
        case ')':
            return 0xE3;
        case ':':
            return 0xE6;
        case '/':
            return DIALOG_CHAR_FORWARD_SLASH;
        case '!':
            return 0xF2;
        case '%':
            return 0xF3;
        case '*':
            return DIALOG_CHAR_STAR_FILLED;
    }
    if ((character >= '0' && character <= '9') || (character >= 'A' && character <= 'Z')
        || (character >= 'a' && character <= 'z')) {
        return ASCII_TO_DIALOG(character);
    }
    return DIALOG_CHAR_SPACE;
}

static void menu_encode_text(u8 *encoded, const char *text) {
    s32 index = 0;
    while (text[index] != '\0' && index < STATS_MENU_TEXT_SIZE - 1) {
        encoded[index] = menu_glyph(text[index]);
        index++;
    }
    encoded[index] = DIALOG_CHAR_TERMINATOR;
}

s16 player_stats_menu_text_centered(s16 centerX, s16 y, const char *text) {
    u8 encoded[STATS_MENU_TEXT_SIZE];
    menu_encode_text(encoded, text);
    s16 x = centerX - get_string_width(encoded) / 2;
    print_generic_string(x, y, encoded);
    return x;
}

void player_stats_menu_text(s16 x, s16 y, const char *text) {
    u8 encoded[STATS_MENU_TEXT_SIZE];
    menu_encode_text(encoded, text);
    s32 width = get_string_width(encoded);
    s32 available = SCREEN_WIDTH - x - 8;
    if (width > available && available > 0) {
        // Keep complete labels and maximum-width counters within the right edge.
        create_dl_translation_matrix(MENU_MTX_PUSH, x, y, 0);
        create_dl_scale_matrix(MENU_MTX_NOPUSH, (f32) available / width, 1.0f, 1.0f);
        print_generic_string(0, 0, encoded);
        gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
        return;
    }
    print_generic_string(x, y, encoded);
}

/* Maximum u64 values fit without narrowing, floating point or printf support. */
static char *menu_number(char *output, u64 value, s32 groupThousands) {
    char digits[20];
    s32 count = 0;
    do {
        digits[count++] = '0' + value % 10;
        value /= 10;
    } while (value != 0);
    while (count > 0) {
        *output++ = digits[--count];
        if (groupThousands && count > 0 && count % 3 == 0) {
            *output++ = ',';
        }
    }
    *output = '\0';
    return output;
}

static void format_value(enum PlayerStatId id, u64 value, char *output) {
    char *cursor;
    u64 seconds;
    u32 fraction;
    if (value == ~(u64) 0) {
        memcpy(output, "Lots!", sizeof("Lots!"));
        return;
    }
    switch (gPlayerStatInfo[id].format) {
        case PLAYER_STATS_SECONDS:
            cursor = menu_number(output, value / PLAYER_STATS_FRAMES_PER_SECOND, TRUE);
            memcpy(cursor, " s", sizeof(" s"));
            break;
        case PLAYER_STATS_TIME:
            seconds = value / PLAYER_STATS_FRAMES_PER_SECOND;
            cursor = menu_number(output, seconds / 3600, FALSE);
            *cursor++ = ':';
            *cursor++ = '0' + (seconds / 60 % 60) / 10;
            *cursor++ = '0' + seconds / 60 % 10;
            *cursor++ = ':';
            *cursor++ = '0' + (seconds % 60) / 10;
            *cursor++ = '0' + seconds % 10;
            *cursor = '\0';
            break;
        case PLAYER_STATS_DISTANCE:
        case PLAYER_STATS_SPEED:
        case PLAYER_STATS_WEDGES:
            cursor = menu_number(output, value / PLAYER_STATS_FIXED_SCALE, TRUE);
            *cursor++ = '.';
            // Display fixed-point fractions with integer truncation.
            fraction = value % PLAYER_STATS_FIXED_SCALE;
            if (gPlayerStatInfo[id].format == PLAYER_STATS_WEDGES) {
                fraction = fraction * 100 / PLAYER_STATS_FIXED_SCALE;
                *cursor++ = '0' + fraction / 10;
                *cursor++ = '0' + fraction % 10;
                memcpy(cursor, " wedges", sizeof(" wedges"));
            } else {
                *cursor++ = '0' + fraction * 10 / PLAYER_STATS_FIXED_SCALE;
                if (gPlayerStatInfo[id].format == PLAYER_STATS_SPEED) {
                    memcpy(cursor, " units per frame", sizeof(" units per frame"));
                } else {
                    memcpy(cursor, " units", sizeof(" units"));
                }
            }
            break;
        default:
            menu_number(output, value, TRUE);
            break;
    }
}

static void render_record_location(const struct PlayerStatsData *data, enum PlayerStatId id) {
    char text[STATS_MENU_TEXT_SIZE];
    u16 level = data->levels[id];
    if (!gPlayerStatInfo[id].record) {
        return;
    }
    if (data->values[id] == 0) {
        player_stats_menu_text(STATS_MENU_LEFT_X, 36, "No record yet");
        return;
    }
    if (level > LEVEL_NONE && level < LEVEL_COUNT && sLevelNames[level] != NULL
        && sLevelNames[level][0] != '\0') {
        u32 length = strlen(sLevelNames[level]);
        length = MIN(length, STATS_MENU_TEXT_SIZE - 1);
        memcpy(text, sLevelNames[level], length);
        text[length] = '\0';
    } else {
        memcpy(text, "Level ", sizeof("Level "));
        menu_number(text + 6, level, FALSE);
    }
    player_stats_menu_text(STATS_MENU_LEFT_X, 36, text);
}

void player_stats_menu_render(void) {
    s32 file = player_stats_active_file();
    s32 first = (sStatsMenu.selected / STATS_MENU_ROWS) * STATS_MENU_ROWS;
    s32 row;
    s32 id;
    s16 y;
    char text[STATS_MENU_TEXT_SIZE];
    char *cursor;
    const struct PlayerStatsData *data;
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 225, 96, gDialogTextAlpha);
    player_stats_menu_text(STATS_MENU_TITLE_X, 214, "STATISTICS");
    gDPPipeSync(gDisplayListHead++);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    player_stats_menu_text(STATS_MENU_LEFT_X, 192, sCategoryNames[sStatsMenu.category]);
    memcpy(text, "Page ", sizeof("Page "));
    cursor = menu_number(text + 5, first / STATS_MENU_ROWS + 1, FALSE);
    memcpy(cursor, " of ", sizeof(" of "));
    cursor += 4;
    menu_number(cursor, (sStatsMenu.count + STATS_MENU_ROWS - 1) / STATS_MENU_ROWS, FALSE);
    player_stats_menu_text(240, 192, text);
    if (file < 0 || file >= PLAYER_STATS_SAVE_FILES) {
        player_stats_menu_text(STATS_MENU_LEFT_X, 174, "No save file selected");
    } else {
        memcpy(text, "File A", sizeof("File A"));
        text[5] += file;
        player_stats_menu_text(264, 214, text);
        data = &gPlayerStats[file];
        for (row = 0; row < STATS_MENU_ROWS && first + row < sStatsMenu.count; row++) {
            id = sStatsMenu.ids[first + row];
            y = 174 - row * 28;
            gDPPipeSync(gDisplayListHead++);
            if (first + row == sStatsMenu.selected) {
                gDPSetEnvColor(gDisplayListHead++, 255, 225, 96, gDialogTextAlpha);
            } else {
                gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
            }
            player_stats_menu_text(STATS_MENU_LEFT_X, y, gPlayerStatInfo[id].label);
            format_value(id, data->values[id], text);
            gDPPipeSync(gDisplayListHead++);
            gDPSetEnvColor(gDisplayListHead++, 190, 215, 255, gDialogTextAlpha);
            player_stats_menu_text(STATS_MENU_LEFT_X + 8, y - 12, text);
        }
        gDPPipeSync(gDisplayListHead++);
        gDPSetEnvColor(gDisplayListHead++, 210, 210, 210, gDialogTextAlpha);
        if (sStatsMenu.count != 0) {
            render_record_location(data, sStatsMenu.ids[sStatsMenu.selected]);
        }
    }
    gDPPipeSync(gDisplayListHead++);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    player_stats_menu_text(STATS_MENU_LEFT_X, 22, "L-R: Category");
    player_stats_menu_text(STATS_MENU_RIGHT_X, 22, "Up-Down: Browse");
    player_stats_menu_text(STATS_MENU_LEFT_X, 8, "B: Back");
    player_stats_menu_text(STATS_MENU_RIGHT_X, 8, "Start: Resume");
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    // The cursor restores the triangle pipeline, so draw it after all text.
    if (file >= 0 && file < PLAYER_STATS_SAVE_FILES && sStatsMenu.count != 0) {
        y = 175 - (sStatsMenu.selected - first) * 28;
        create_dl_translation_matrix(MENU_MTX_PUSH, STATS_MENU_LEFT_X - 12, y, 0);
        gDPSetEnvColor(gDisplayListHead++, 255, 225, 96, gDialogTextAlpha);
        gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
        gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    }
}
