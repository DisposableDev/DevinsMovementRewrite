#include "game_options_menu.h"
#include "game_timing.h"
#include "save_file.h"
#include "config/config_menu.h"

#include <string.h>

#define OPTIONS_MENU_DIRECTIONS (OM_UP | OM_DOWN | OM_LEFT | OM_RIGHT)

struct GameOptionsMenuState gOptionsMenu;

static void collect_options(void) {
    gOptionsMenu.count = 0;
    for (s32 id = 0; id < GO_COUNT; id++) {
        if (gGameOptionDefinitions[id].label == NULL
            || gGameOptionDefinitions[id].category != gOptionsMenu.category || id == GO_FREE_MOVE
            || id == GO_CAMERA_CUSTOM || id == GO_CONTROLLER_NUMBERS
            || (id == GO_DISABLE_THICC_TEXT && !save_file_is_he_thicc(gOptionsMenu.file))) {
            continue;
        }
        gOptionsMenu.ids[gOptionsMenu.count++] = id;
        if (id == GO_DEBUG_MODE) {
            gOptionsMenu.ids[gOptionsMenu.count++] = GO_FREE_MOVE;
        } else if (id == GO_CONTROLLER_DISPLAY) {
            gOptionsMenu.ids[gOptionsMenu.count++] = GO_CONTROLLER_NUMBERS;
        } else if (id == GO_CAMERA_SPEED && game_options_get(id) == 5) {
            gOptionsMenu.ids[gOptionsMenu.count++] = GO_CAMERA_CUSTOM;
        }
    }
    if (gOptionsMenu.selected >= gOptionsMenu.count) {
        gOptionsMenu.selected = gOptionsMenu.count ? gOptionsMenu.count - 1 : 0;
    }
}

void game_options_menu_open(void) {
    s32 file = game_options_active_file();
    u32 fileSelectionSerial = game_options_file_selection_serial();
    u8 category = GO_CONTROLS;
    u8 selected = 0;
    enum GameOptionId selectedId = GO_COUNT;
    u8 records = 0;
    u8 recordCourse = COURSE_MIN;
    if (gOptionsMenu.file == file && gOptionsMenu.fileSelectionSerial == fileSelectionSerial
        && gOptionsMenu.count && gOptionsMenu.category > GO_SETTINGS
        && gOptionsMenu.category < GO_CATEGORY_COUNT && gOptionsMenu.selected < gOptionsMenu.count) {
        category = gOptionsMenu.category;
        selected = gOptionsMenu.selected;
        selectedId = gOptionsMenu.ids[selected];
        records = gOptionsMenu.records;
        recordCourse = gOptionsMenu.recordCourse;
    }
    memset(&gOptionsMenu, 0, sizeof(gOptionsMenu));
    gOptionsMenu.open = 1;
    gOptionsMenu.category = category;
    gOptionsMenu.selected = selected;
    gOptionsMenu.file = file;
    gOptionsMenu.fileSelectionSerial = fileSelectionSerial;
    gOptionsMenu.records = records;
    gOptionsMenu.recordCourse = recordCourse;
    game_options_confirmation_cancel();
    collect_options();
    for (s32 i = 0; i < gOptionsMenu.count; i++) {
        if (gOptionsMenu.ids[i] == selectedId) {
            gOptionsMenu.selected = i;
            break;
        }
    }
}

void game_options_menu_close(void) {
    game_options_confirmation_cancel();
    gOptionsMenu.open = 0;
    gOptionsMenu.warning = 0;
    gOptionsMenu.saveFailed = !game_options_save(gOptionsMenu.file);
    if (!game_timing_checkpoint(gOptionsMenu.file)) {
        gOptionsMenu.saveFailed = 1;
    }
}

static s32 setting_enabled(enum GameOptionId id) {
    return id != GO_HELD_TWIRL || game_options_get(GO_TWIRLS);
}

static void change_value(enum GameOptionId id, s32 delta) {
    const struct GameOptionDefinition *definition = &gGameOptionDefinitions[id];
    s32 value = game_options_get_file(game_options_active_file(), id);
    if (!setting_enabled(id) || definition->kind == GO_ACTION) {
        return;
    }
    do {
        value += delta * definition->step;
        if (value < definition->minimum) {
            value = definition->maximum;
        } else if (value > definition->maximum) {
            value = definition->minimum;
        }
    } while (id == GO_CONTROLLER && !game_options_controller_available(value));
    if ((id == GO_CHEATS_ENABLED || id == GO_DEBUG_MODE) && value != 0 && !game_options_get(id)) {
        if (game_options_confirmation_begin(id)) {
            gOptionsMenu.warning = 1;
            gOptionsMenu.warningId = id;
        }
    } else {
        game_options_set(id, value);
    }
    collect_options();
}

static u8 menu_navigation_buttons(u16 held) {
    u8 directions = game_options_menu_dpad(held & OPTIONS_MENU_DIRECTIONS);
    if (gOptionsMenu.records) {
        if (held & OM_PREVIOUS) {
            directions |= OM_LEFT;
        }
        if (held & OM_NEXT) {
            directions |= OM_RIGHT;
        }
    }
    return directions;
}

// Consume direction holds once per menu sample and emit fresh or repeated steps.
static u8 sample_navigation_step(u32 frame, u16 held, u8 stickDirection) {
    u8 pad = menu_navigation_buttons(held);
    u8 allowed = game_options_menu_direction(
        pad | (game_options_get(GO_MENU_NAVIGATION) == 1 ? 0 : stickDirection),
        &gOptionsMenu.heldDirection);
    u8 stick = game_options_menu_stick(stickDirection, &gOptionsMenu.stickDirection);
    u8 direction = (pad | stick) & allowed;
    if (direction != gOptionsMenu.direction) {
        gOptionsMenu.direction = direction;
        gOptionsMenu.directionStart = frame;
        return direction;
    }
    if (direction) {
        u32 elapsed = frame - gOptionsMenu.directionStart;
        if (elapsed >= MENU_SCROLL_REPEAT_DELAY_FRAMES
            && (elapsed - MENU_SCROLL_REPEAT_DELAY_FRAMES) % MENU_SCROLL_REPEAT_INTERVAL_FRAMES == 0) {
            return direction;
        }
    }
    return 0;
}

s32 game_options_menu_step(u32 frame, u16 held, u16 pressed, u8 stickDirection) {
    if (!gOptionsMenu.open) {
        return 0;
    }
    if (gOptionsMenu.file != game_options_active_file()
        || gOptionsMenu.fileSelectionSerial != game_options_file_selection_serial()) {
        game_options_confirmation_cancel();
        gOptionsMenu.open = 0;
        return 1;
    }
    if (gOptionsMenu.sampled && gOptionsMenu.frame == frame) {
        return 0;
    }
    gOptionsMenu.frame = frame;
    if (!gOptionsMenu.sampled) {
        gOptionsMenu.sampled = 1;
        gOptionsMenu.stickDirection = stickDirection ? GO_MENU_WAIT_FOR_NEUTRAL : 0;
        gOptionsMenu.direction = game_options_menu_direction(
            menu_navigation_buttons(held)
                | (game_options_get(GO_MENU_NAVIGATION) == 1 ? 0 : stickDirection),
            &gOptionsMenu.heldDirection);
        gOptionsMenu.directionStart = frame;
        return 0;
    }
    if (gOptionsMenu.warning) {
        // Track holds while the warning owns input so exiting cannot fabricate a fresh stick choice.
        sample_navigation_step(frame, held, stickDirection);
        s32 cancelled = (pressed & OM_CANCEL) != 0;
        if (game_options_confirmation_update(frame, held & OM_CONFIRM, cancelled) || cancelled) {
            gOptionsMenu.warning = 0;
            gOptionsMenu.direction = 0;
        }
        return 0;
    }
    if (pressed & OM_CANCEL) {
        if (gOptionsMenu.records) {
            gOptionsMenu.records = 0;
            gOptionsMenu.direction = 0;
            return 0;
        }
        game_options_menu_close();
        return 1;
    }
    if (!gOptionsMenu.records
        && ((pressed & (OM_PREVIOUS | OM_NEXT)) == OM_PREVIOUS
            || (pressed & (OM_PREVIOUS | OM_NEXT)) == OM_NEXT)) {
        s32 category = gOptionsMenu.category;
        do {
            category =
                (category + ((pressed & OM_NEXT) ? 1 : GO_CATEGORY_COUNT - 1)) % GO_CATEGORY_COUNT;
        } while (category == GO_SETTINGS);
        gOptionsMenu.category = category;
        gOptionsMenu.selected = 0;
        gOptionsMenu.direction = 0;
        collect_options();
        return 0;
    }

    u8 step = sample_navigation_step(frame, held, stickDirection);
    if (!gOptionsMenu.count) {
        return 0;
    }
    if (gOptionsMenu.records) {
        if (step & (OM_LEFT | OM_RIGHT)) {
            s32 course = gOptionsMenu.recordCourse + ((step & OM_RIGHT) ? 1 : -1);
            if (course < COURSE_MIN) {
                course = COURSE_CAKE_END - 1;
            } else if (course >= COURSE_CAKE_END) {
                course = COURSE_MIN;
            }
            gOptionsMenu.recordCourse = course;
        }
        return 0;
    }
    if (pressed & OM_RESET) {
        enum GameOptionId id = gOptionsMenu.ids[gOptionsMenu.selected];
        const struct GameOptionDefinition *definition = &gGameOptionDefinitions[id];
        if (definition->kind != GO_ACTION) {
            game_options_set(id, definition->defaultValue);
            collect_options();
        }
    } else if (step & (OM_UP | OM_DOWN)) {
        s32 selected = gOptionsMenu.selected + ((step & OM_DOWN) ? 1 : -1);
        gOptionsMenu.selected = (selected + gOptionsMenu.count) % gOptionsMenu.count;
    } else if (pressed & OM_CONFIRM) {
        enum GameOptionId id = gOptionsMenu.ids[gOptionsMenu.selected];
        if (id == GO_BEST_TIMES) {
            s32 course = game_timing_state()->course;
            gOptionsMenu.records = 1;
            gOptionsMenu.recordCourse =
                course >= COURSE_MIN && course < COURSE_CAKE_END ? course : COURSE_MIN;
            gOptionsMenu.direction = 0;
        } else {
            change_value(id, 1);
        }
    } else if (step & (OM_LEFT | OM_RIGHT)) {
        change_value(gOptionsMenu.ids[gOptionsMenu.selected], (step & OM_RIGHT) ? 1 : -1);
    }
    return 0;
}

static char *unsigned_text(char *output, u32 value) {
    char digits[10];
    s32 count = 0;
    do {
        digits[count++] = '0' + value % 10;
        value /= 10;
    } while (value);
    while (count) {
        *output++ = digits[--count];
    }
    *output = '\0';
    return output;
}

static void copy_value_text(char *output, const char *text) {
    memcpy(output, text, strlen(text) + 1);
}

void game_options_menu_value(enum GameOptionId id, char output[32]) {
    static const char *const camera[] = {
        "Very slow", "Slow", "Medium", "Fast", "Very fast", "Custom"
    };
    static const char *const rumble[] = { "Off", "Low", "Normal" };
    static const char *const controllers[] = { "Automatic", "Port 1", "Port 2", "Port 3", "Port 4" };
    static const char *const hud[] = { "Always", "Automatic", "Hidden" };
    static const char *const pause[] = { "Normal", "Quiet", "Silent" };
    static const char *const navigation[] = { "Stick", "D-pad", "Both" };
    static const char *const shake[] = { "On", "Off" };
    if (id < 0 || id >= GO_COUNT) {
        output[0] = '\0';
        return;
    }
    const char *text = NULL;
    u8 value = game_options_get_file(game_options_active_file(), id);
    switch (id) {
        case GO_CAMERA_SPEED:
            text = camera[value];
            break;
        case GO_RUMBLE:
            text = rumble[value];
            break;
        case GO_CONTROLLER:
            text = controllers[value];
            break;
        case GO_HUD:
            text = hud[value];
            break;
        case GO_PAUSE_MUSIC:
            text = pause[value];
            break;
        case GO_MENU_NAVIGATION:
            text = navigation[value];
            break;
        case GO_CAMERA_SHAKE:
            text = shake[value];
            break;
        case GO_CAMERA_INVERT_X:
        case GO_CAMERA_INVERT_Y:
            text = value ? "Inverted" : "Normal";
            break;
        case GO_SOUND_OUTPUT:
            text = value ? "Mono" : "Stereo";
            break;
        case GO_CONTROLLER_DISPLAY:
            if (value == 2) {
                text = "Mini";
            } else if (value) {
                text = "Max";
            } else {
                text = "Off";
            }
            break;
        case GO_ASPECT_RATIO:
            text = value ? "Widescreen" : "4:3";
            break;
        case GO_HUD_WIDE:
            text = value ? "Screen edges" : "Near centre";
            break;
        case GO_BOWSER_HOLD_SPIN:
            text = value ? "Hold analogue stick" : "Off";
            break;
        case GO_FLASHING:
            text = value ? "Reduced" : "Normal";
            break;
        case GO_BEST_TIMES:
            text = "View records";
            break;
        case GO_SWITCHES:
        case GO_WING_CAP:
        case GO_METAL_CAP:
        case GO_VANISH_CAP:
        case GO_BOWSER_FIGHTS:
        case GO_CANNON_ANGLES:
            text = value ? "Original" : "Current";
            break;
        default:
            break;
    }
    if (text != NULL) {
        copy_value_text(output, text);
    } else if (id == GO_CAMERA_CUSTOM) {
        char *end = unsigned_text(output, value / 2);
        copy_value_text(end, value % 2 ? ".5" : ".0");
    } else if (id == GO_AUDIO_BALANCE) {
        if (value == 10) {
            copy_value_text(output, "Centre");
        } else {
            copy_value_text(output, value < 10 ? "Left " : "Right ");
            unsigned_text(output + strlen(output), (value < 10 ? 10 - value : value - 10) * 10);
        }
    } else if (gGameOptionDefinitions[id].kind == GO_NUMBER) {
        char *end = unsigned_text(output, value);
        if (id >= GO_MASTER_VOLUME && id <= GO_VOICE_VOLUME) {
            copy_value_text(end, "%");
        } else if (id == GO_DEBOUNCE || id == GO_MIN_HOLD) {
            copy_value_text(end, " frames");
        }
    } else {
        copy_value_text(output, value ? "On" : "Off");
    }
}
