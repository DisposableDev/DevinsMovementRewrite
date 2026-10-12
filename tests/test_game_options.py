#!/usr/bin/env python3
"""Run the complete production options unit against fault-injected SRAM."""

from pathlib import Path
import os
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
FIXTURE = r'''
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define FALSE 0
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#include "game_options.h"
#define SAVE_FILE_H
static s32 sHeThiccFile = -1;
static s32 save_file_is_he_thicc(s32 file) { return file == sHeThiccFile; }
#include "game_options.c"
#include "game_options_menu.c"
#include "game_timing.h"

static u8 storage[0x8000];
static s32 writes;
static s32 failWrite;
static s32 badReadback;
static s32 failRead;
static u32 failedReadOffset = ~(u32) 0;

static s32 read_storage(u32 offset, void *data, u32 size) {
    assert(offset >= 0x800 && offset + size <= 0x1000);
    assert(size % 16 == 0);
    if (failRead || offset == failedReadOffset) { return 1; }
    memcpy(data, storage + offset, size);
    if (badReadback) { ((u8 *)data)[0] ^= 1; }
    return 0;
}

static s32 write_storage(u32 offset, const void *data, u32 size) {
    assert(offset >= 0x800 && offset + size <= 0x1000);
    writes++;
    if (failWrite) { return 1; }
    memcpy(storage + offset, data, size);
    return 0;
}

static s32 read_times(u32 offset, void *data, u32 size) {
    assert(offset >= 0x1000 && offset + size <= 0x4000);
    memcpy(data, storage + offset, size);
    return 0;
}

static void reboot(void) {
    game_options_init(read_storage, write_storage);
    game_options_select_file(0);
}

static void confirm(enum GameOptionId id, u32 frame) {
    assert(game_options_confirmation_begin(id));
    assert(!game_options_confirmation_update(frame++, 0, 0));
    for (s32 i = 0; i < OPTIONS_CONFIRM_FRAMES; i++) {
        assert(!game_options_confirmation_update(frame++, 1, 0));
    }
    assert(game_options_confirmation_update(frame, 1, 0));
}

static u32 menuFrame;
static void menu_input(u16 held, u16 pressed, u8 stick) {
    assert(!game_options_menu_step(menuFrame++, held, pressed, stick));
}
static void menu_select(enum GameOptionId id) {
    for (s32 i = 0; i < gOptionsMenu.count; i++) {
        if (gOptionsMenu.ids[i] == id) {
            gOptionsMenu.selected = i;
            return;
        }
    }
    assert(!"option missing from its category");
}
static void menu_category(u8 category) {
    while (gOptionsMenu.category != category) {
        menu_input(OM_NEXT, OM_NEXT, 0);
    }
}
static void thicc_text_preference(void) {
    memset(storage, 0, sizeof(storage));
    failRead = failWrite = badReadback = 0;
    failedReadOffset = ~(u32) 0;
    sHeThiccFile = 2;
    reboot();
    for (s32 file = 0; file < GAME_OPTIONS_FILES; file++) {
        game_options_select_file(file);
        game_options_menu_open();
        menu_input(0, 0, 0);
        menu_category(GO_ACCESSIBILITY);
        s32 found = 0;
        for (s32 i = 0; i < gOptionsMenu.count; i++) {
            found |= gOptionsMenu.ids[i] == GO_DISABLE_THICC_TEXT;
        }
        assert(found == (file == sHeThiccFile));
        assert(!game_options_get(GO_DISABLE_THICC_TEXT));
    }
    game_options_select_file(sHeThiccFile);
    game_options_menu_open();
    menu_input(0, 0, 0);
    menu_category(GO_ACCESSIBILITY);
    menu_select(GO_DISABLE_THICC_TEXT);
    menu_input(OM_CONFIRM, OM_CONFIRM, 0);
    assert(game_options_get(GO_DISABLE_THICC_TEXT) && !gOptionsMenu.warning);
    game_options_menu_close();
    reboot();
    assert(!game_options_get(GO_DISABLE_THICC_TEXT));
    game_options_select_file(sHeThiccFile);
    assert(game_options_get(GO_DISABLE_THICC_TEXT));
    game_options_menu_open();
    menu_input(0, 0, 0);
    menu_category(GO_ACCESSIBILITY);
    menu_select(GO_DISABLE_THICC_TEXT);
    menu_input(OM_RESET, OM_RESET, 0);
    assert(!game_options_get(GO_DISABLE_THICC_TEXT));
    game_options_menu_close();
    reboot();
    game_options_select_file(sHeThiccFile);
    assert(!game_options_get(GO_DISABLE_THICC_TEXT));
    sHeThiccFile = -1;
}

static void reset_tests(void) {
    for (s32 id = 0; id < GO_COUNT; id++) {
        const struct GameOptionDefinition *definition = &gGameOptionDefinitions[id];
        if (!definition->label || definition->kind == GO_ACTION) { continue; }
        game_options_init(NULL, NULL);
        game_options_select_file(0);
        game_options_set_connected_controllers(0x0F);
        sValues[0][id] = definition->defaultValue == definition->minimum
                            ? definition->maximum : definition->minimum;
        game_options_menu_open();
        menu_input(0, 0, 0);
        gOptionsMenu.category = definition->category;
        gOptionsMenu.count = 1;
        gOptionsMenu.selected = 0;
        gOptionsMenu.ids[0] = id;
        menu_input(OM_RESET, OM_RESET, 0);
        assert(game_options_get(id) == definition->defaultValue);
    }
    menu_category(GO_CHEATS);
    menu_select(GO_DEBUG_MODE);
    assert(gOptionsMenu.ids[gOptionsMenu.selected + 1] == GO_FREE_MOVE);
    menu_category(GO_PRESENTATION);
    for (s32 i = 0; i < gOptionsMenu.count; i++) {
        assert(gGameOptionDefinitions[gOptionsMenu.ids[i]].label != NULL);
        assert(gOptionsMenu.ids[i] != GO_RESERVED_PROMPTS);
    }
    menu_select(GO_MASTER_VOLUME);
    assert(game_options_set(GO_MASTER_VOLUME, 20));
    menu_input(OM_RESET, 0, 0);
    assert(game_options_get(GO_MASTER_VOLUME) == 20);
    menu_input(OM_RESET, OM_RESET, 0);
    assert(game_options_get(GO_MASTER_VOLUME) == 100);
    assert(game_options_set(GO_INPUT_DISPLAY, 1));
    menu_select(GO_CONTROLLER_DISPLAY);
    menu_input(OM_RESET, OM_RESET, 0);
    assert(!game_options_get(GO_CONTROLLER_DISPLAY));
    assert(game_options_get(GO_INPUT_DISPLAY));
    menu_category(GO_GAMEPLAY);
    menu_select(GO_BOWSER_FIGHTS);
    assert(game_options_set(GO_BOWSER_FIGHTS, 0));
    menu_input(OM_RESET, OM_RESET, 0);
    assert(!game_options_get(GO_BOWSER_FIGHTS));
}
static void restart_rules(void) {
    static const enum GameOptionId deferred[] = {
        GO_BOWSER_FIGHTS, GO_WING_CAP, GO_METAL_CAP, GO_VANISH_CAP
    };
    game_options_init(NULL, NULL);
    game_options_select_file(0);
    game_options_restart();
    assert(gGameOptionDefinitions[GO_SWITCHES].label == NULL);
    assert(game_options_get(GO_SWITCHES) == 0);
    assert(!game_options_set(GO_SWITCHES, 1));
    for (u32 i = 0; i < sizeof(deferred) / sizeof(deferred[0]); i++) {
        enum GameOptionId id = deferred[i];
        char text[32];
        assert(game_options_requires_restart(id));
        assert(game_options_set(id, 1));
        assert(game_options_get_file(0, id) == 1 && game_options_get(id) == 0);
        gOptionsMenu.file = 0;
        game_options_menu_value(id, text);
        assert(!strcmp(text, "Original"));
        assert(game_options_set(id, 0));
        assert(game_options_get(id) == 0);
        assert(game_options_set(id, 1));
    }
    assert(game_options_set(GO_TWIRLS, 0) && !game_options_get(GO_TWIRLS));
    game_options_restart();
    for (u32 i = 0; i < sizeof(deferred) / sizeof(deferred[0]); i++) {
        assert(game_options_get(deferred[i]) == 1);
    }
    // Changing files must not reuse another file's gameplay snapshot.
    game_options_select_file(1);
    assert(!game_options_get(GO_WING_CAP));
    game_options_restart();
    assert(game_options_set(GO_WING_CAP, 1));
    assert(!game_options_get(GO_WING_CAP));
    game_options_select_file(0);
    assert(game_options_get(GO_WING_CAP) == 1);
}

static void warning_stick_returns(void) {
    for (s32 requireRelease = 0; requireRelease <= 1; requireRelease++) {
        for (s32 complete = 0; complete <= 1; complete++) {
            for (s32 keptStick = 0; keptStick <= 1; keptStick++) {
                memset(storage, 0xA5, sizeof(storage));
                reboot();
                assert(game_options_set(GO_MENU_STICK_RELEASE, requireRelease));
                game_options_menu_open();
                menu_input(0, 0, 0);
                menu_category(GO_CHEATS);
                menu_select(GO_DEBUG_MODE);
                menu_input(0, 0, OM_RIGHT);
                assert(gOptionsMenu.warning && !game_options_get(GO_DEBUG_MODE));
                u8 stick = keptStick ? OM_RIGHT : 0;
                if (complete) {
                    menu_input(0, 0, stick);
                    for (s32 i = 0; i <= OPTIONS_CONFIRM_FRAMES; i++) {
                        menu_input(OM_CONFIRM, 0, stick);
                    }
                    assert(game_options_get(GO_DEBUG_MODE));
                } else {
                    menu_input(OM_CANCEL, OM_CANCEL, stick);
                    assert(!game_options_get(GO_DEBUG_MODE));
                }
                assert(gOptionsMenu.open && !gOptionsMenu.warning);
                menu_input(0, 0, keptStick ? OM_RIGHT : 0);
                if (keptStick && !requireRelease) {
                    assert(complete ? !game_options_get(GO_DEBUG_MODE) : gOptionsMenu.warning);
                } else {
                    assert(!gOptionsMenu.warning && game_options_get(GO_DEBUG_MODE) == complete);
                    menu_input(0, 0, 0);
                    menu_input(0, 0, OM_RIGHT);
                    assert(complete ? !game_options_get(GO_DEBUG_MODE) : gOptionsMenu.warning);
                }
                game_options_menu_close();
            }
        }
    }
}

static void menu_file_switch_returns(void) {
    game_options_init(NULL, NULL);
    game_options_select_file(0);
    game_options_menu_open();
    menu_input(0, 0, 0);
    menu_category(GO_CHEATS);
    menu_select(GO_DEBUG_MODE);
    game_options_menu_close();
    // A same-file course restart or re-selection keeps the remembered row.
    game_options_restart();
    game_options_select_file(0);
    game_options_menu_open();
    assert(gOptionsMenu.category == GO_CHEATS);
    assert(gOptionsMenu.ids[gOptionsMenu.selected] == GO_DEBUG_MODE);
    game_options_menu_close();
    // An intervening file change matters even if that file never opens Options.
    game_options_select_file(1);
    game_options_select_file(0);
    game_options_menu_open();
    assert(gOptionsMenu.category == GO_CONTROLS && !gOptionsMenu.selected);
    assert(!gOptionsMenu.records && !gOptionsMenu.warning);
    game_options_menu_close();
}

static void menu_tests(void) {
    game_options_init(NULL, NULL);
    game_options_select_file(0);
    game_options_menu_open();
    menuFrame = 1;
    menu_input(OM_CONFIRM | OM_NEXT, OM_CONFIRM | OM_NEXT, 0);
    assert(gOptionsMenu.category == GO_CONTROLS);
    assert(gOptionsMenu.count > 0);
    menu_input(0, 0, 0);
    menu_input(OM_PREVIOUS, OM_PREVIOUS, 0);
    assert(gOptionsMenu.category == GO_CHEATS);
    menu_input(OM_NEXT, OM_NEXT, 0);
    assert(gOptionsMenu.category == GO_CONTROLS);
    for (s32 category = GO_PRESENTATION; category <= GO_CHEATS; category++) {
        menu_input(OM_NEXT, OM_NEXT, 0);
        assert(gOptionsMenu.category == category && gOptionsMenu.count > 0);
    }
    menu_input(OM_NEXT, OM_NEXT, 0);
    assert(gOptionsMenu.category == GO_CONTROLS);
    menu_category(GO_CONTROLS);
    assert(gOptionsMenu.count == 7);
    menu_select(GO_CAMERA_SPEED);
    menu_input(OM_RIGHT, OM_RIGHT, 0);
    assert(game_options_get(GO_CAMERA_SPEED) == 3);
    assert(!game_options_menu_step(menuFrame - 1, OM_RIGHT, OM_RIGHT, 0));
    assert(game_options_get(GO_CAMERA_SPEED) == 3);
    menu_input(0, 0, 0);
    menu_input(OM_RIGHT, OM_RIGHT, 0);
    menu_input(0, 0, 0);
    menu_input(OM_RIGHT, OM_RIGHT, 0);
    assert(game_options_get(GO_CAMERA_SPEED) == 5);
    assert(gOptionsMenu.count == 8);
    assert(gOptionsMenu.ids[gOptionsMenu.selected + 1] == GO_CAMERA_CUSTOM);
    menu_input(OM_CONFIRM, OM_CONFIRM, 0);
    assert(game_options_get(GO_CAMERA_SPEED) == 0);
    assert(gOptionsMenu.count == 7);
    // Both directional changes wrap, including dependent half-step numbers.
    menu_input(0, 0, 0);
    menu_input(OM_LEFT, OM_LEFT, 0);
    assert(game_options_get(GO_CAMERA_SPEED) == 5 && gOptionsMenu.count == 8);
    menu_select(GO_CAMERA_CUSTOM);
    assert(game_options_set(GO_CAMERA_CUSTOM, 20));
    menu_input(0, 0, 0);
    menu_input(OM_RIGHT, OM_RIGHT, 0);
    assert(game_options_get(GO_CAMERA_CUSTOM) == 1);
    menu_input(0, 0, 0);
    menu_input(OM_LEFT, OM_LEFT, 0);
    assert(game_options_get(GO_CAMERA_CUSTOM) == 20);
    menu_select(GO_CAMERA_SPEED);
    menu_input(0, 0, 0);
    menu_input(OM_RIGHT, OM_RIGHT, 0);
    assert(game_options_get(GO_CAMERA_SPEED) == 0 && gOptionsMenu.count == 7);

    // Only Automatic and physically present ports can be selected.
    menu_select(GO_CONTROLLER);
    game_options_set_connected_controllers(0x0A); // Ports 2 and 4.
    assert(!game_options_set(GO_CONTROLLER, 1));
    assert(!game_options_set(GO_CONTROLLER, 3));
    s32 forward[] = {2, 4, 0};
    s32 backward[] = {4, 2, 0};
    for (s32 i = 0; i < 3; i++) {
        menu_input(0, 0, 0); menu_input(OM_RIGHT, OM_RIGHT, 0);
        assert(game_options_get(GO_CONTROLLER) == forward[i]);
    }
    for (s32 i = 0; i < 3; i++) {
        menu_input(0, 0, 0); menu_input(OM_LEFT, OM_LEFT, 0);
        assert(game_options_get(GO_CONTROLLER) == backward[i]);
    }
    menu_input(0, 0, 0); menu_input(OM_CONFIRM, OM_CONFIRM, 0);
    assert(game_options_get(GO_CONTROLLER) == 2);
    game_options_set_connected_controllers(1); // Selection disconnects; only Port 1 remains.
    assert(!game_options_set(GO_CONTROLLER, 2));
    menu_input(0, 0, 0); menu_input(OM_RIGHT, OM_RIGHT, 0);
    assert(game_options_get(GO_CONTROLLER) == 0);
    menu_input(0, 0, 0); menu_input(OM_LEFT, OM_LEFT, 0);
    assert(game_options_get(GO_CONTROLLER) == 1);
    game_options_set_connected_controllers(0);
    menu_input(0, 0, 0); menu_input(OM_RIGHT, OM_RIGHT, 0);
    assert(game_options_get(GO_CONTROLLER) == 0);
    menu_input(0, 0, 0); menu_input(OM_LEFT, OM_LEFT, 0);
    assert(game_options_get(GO_CONTROLLER) == 0);
    assert(!game_options_controller_available(-1));
    assert(!game_options_controller_available(5));
    assert(game_options_controller_available(0));

    menu_category(GO_PRESENTATION);
    menu_select(GO_MASTER_VOLUME);
    menu_input(0, 0, 0); menu_input(OM_RIGHT, OM_RIGHT, 0);
    assert(game_options_get(GO_MASTER_VOLUME) == 0);
    menu_input(0, 0, 0); menu_input(OM_LEFT, OM_LEFT, 0);
    assert(game_options_get(GO_MASTER_VOLUME) == 100);

    menu_select(GO_CONTROLLER_DISPLAY);
    assert(game_options_set(GO_INPUT_DISPLAY, 1));
    assert(game_options_get(GO_CONTROLLER_DISPLAY) == 1);
    menu_input(0, 0, 0); menu_input(OM_LEFT, OM_LEFT, 0);
    assert(game_options_get(GO_CONTROLLER_DISPLAY) == 0);
    assert(game_options_get(GO_INPUT_DISPLAY) == 1);
    menu_input(0, 0, 0); menu_input(OM_RIGHT, OM_RIGHT, 0);
    assert(game_options_get(GO_CONTROLLER_DISPLAY) == 1);
    assert(game_options_set(GO_INPUT_DISPLAY, 0));
    menu_input(0, 0, 0); menu_input(OM_LEFT, OM_LEFT, 0);
    assert(game_options_get(GO_CONTROLLER_DISPLAY) == 0);
    menu_input(0, 0, 0); menu_input(OM_LEFT, OM_LEFT, 0);
    assert(game_options_get(GO_CONTROLLER_DISPLAY) == 2);

    menu_input(0, 0, 0); menu_input(OM_DOWN, OM_DOWN, 0);
    assert(gOptionsMenu.ids[gOptionsMenu.selected] == GO_CONTROLLER_NUMBERS);
    assert(game_options_get(GO_CONTROLLER_NUMBERS) == 1);
    menu_input(0, 0, 0); menu_input(OM_CONFIRM, OM_CONFIRM, 0);
    assert(game_options_get(GO_CONTROLLER_NUMBERS) == 0);
    assert(game_options_get(GO_CONTROLLER_DISPLAY) == 2);

    menu_category(GO_ACCESSIBILITY);
    assert(game_options_set(GO_MENU_STICK_RELEASE, 1));
    menu_select(GO_HEALTH_WARNING);
    u8 selected = gOptionsMenu.selected;
    menu_input(0, 0, OM_DOWN);
    assert(gOptionsMenu.selected == selected + 1);
    for (s32 i = 0; i < 20; i++) { menu_input(0, 0, OM_DOWN); }
    assert(gOptionsMenu.selected == selected + 1);
    menu_input(0, 0, OM_UP);
    assert(gOptionsMenu.selected == selected + 1);
    menu_input(0, 0, 0);
    menu_input(0, 0, OM_UP);
    assert(gOptionsMenu.selected == selected);
    // Switching from a held stick to D-pad is not a neutral release.
    menu_input(OM_DOWN, OM_DOWN, 0);
    assert(gOptionsMenu.selected == selected);
    menu_input(0, 0, 0);
    menu_input(OM_DOWN, OM_DOWN, 0);
    assert(gOptionsMenu.selected == selected + 1);
    for (s32 i = 0; i < 20; i++) { menu_input(OM_DOWN, 0, 0); }
    assert(gOptionsMenu.selected == selected + 1);
    menu_input(OM_UP, OM_UP, 0);
    assert(gOptionsMenu.selected == selected + 1);
    menu_input(0, 0, 0); menu_input(OM_UP, OM_UP, 0);
    assert(gOptionsMenu.selected == selected);
    // Disabling release restores the existing held D-pad repeat.
    menu_input(0, 0, 0);
    assert(game_options_set(GO_MENU_STICK_RELEASE, 0));
    menu_input(OM_DOWN, OM_DOWN, 0);
    for (s32 i = 0; i < MENU_SCROLL_REPEAT_DELAY_FRAMES; i++) { menu_input(OM_DOWN, 0, 0); }
    assert(gOptionsMenu.selected == selected + 2);
    assert(game_options_set(GO_MENU_STICK_RELEASE, 1));
    menu_input(0, 0, 0);
    assert(game_options_set(GO_MENU_NAVIGATION, 0));
    selected = gOptionsMenu.selected;
    menu_input(OM_DOWN, OM_DOWN, 0);
    assert(gOptionsMenu.selected == selected);
    menu_input(0, 0, OM_DOWN);
    assert(gOptionsMenu.selected == selected + 1);
    assert(game_options_set(GO_MENU_NAVIGATION, 2));

    menu_category(GO_GAMEPLAY);
    assert(game_options_set(GO_TWIRLS, 0));
    menu_select(GO_CANNON_ANGLES);
    assert(game_options_get(GO_CANNON_ANGLES) == 0);
    menu_input(OM_CONFIRM, OM_CONFIRM, 0);
    assert(game_options_get(GO_CANNON_ANGLES) == 1);
    menu_select(GO_HELD_TWIRL);
    menu_input(OM_LEFT, OM_LEFT, 0);
    assert(game_options_get(GO_HELD_TWIRL) == 1);
    menu_input(0, 0, 0);
    menu_select(GO_BOWSER_FIGHTS);
    menu_input(OM_RIGHT, OM_RIGHT, 0);
    assert(game_options_get(GO_BOWSER_FIGHTS) == 1);

    menu_category(GO_CHEATS);
    menu_select(GO_CHEATS_ENABLED);
    menu_input(0, 0, 0); menu_input(OM_LEFT, OM_LEFT, 0);
    assert(gOptionsMenu.warning && !game_options_get(GO_CHEATS_ENABLED));
    menu_input(OM_CANCEL, OM_CANCEL, 0);
    assert(!gOptionsMenu.warning && !game_options_get(GO_CHEATS_ENABLED));
    menu_input(0, 0, 0);
    menu_input(OM_CONFIRM, OM_CONFIRM, 0);
    assert(gOptionsMenu.warning);
    for (s32 i = 0; i < 200; i++) { menu_input(OM_CONFIRM, 0, 0); }
    assert(!game_options_get(GO_CHEATS_ENABLED));
    menu_input(0, 0, 0);
    for (s32 i = 0; i < OPTIONS_CONFIRM_FRAMES; i++) { menu_input(OM_CONFIRM, 0, 0); }
    assert(!game_options_get(GO_CHEATS_ENABLED));
    menu_input(OM_CONFIRM, 0, 0);
    assert(game_options_get(GO_CHEATS_ENABLED));
    assert(!gOptionsMenu.warning);
    menu_select(GO_DEBUG_MODE);
    menu_input(OM_CONFIRM, OM_CONFIRM, 0);
    assert(gOptionsMenu.warning);
    menu_input(OM_CANCEL, OM_CANCEL, 0);
    assert(gOptionsMenu.open && !gOptionsMenu.warning);
    assert(!game_options_get(GO_DEBUG_MODE));

    // Start/unpause and B both use close; reopening retains the selected row.
    game_options_menu_close();
    game_options_menu_open();
    assert(gOptionsMenu.category == GO_CHEATS);
    assert(gOptionsMenu.ids[gOptionsMenu.selected] == GO_DEBUG_MODE);
    assert(!gOptionsMenu.warning && !gOptionsMenu.sampled);
    menu_input(OM_CONFIRM | OM_NEXT, OM_CONFIRM | OM_NEXT, 0);
    assert(!gOptionsMenu.warning && gOptionsMenu.category == GO_CHEATS);
    assert(!game_options_get(GO_DEBUG_MODE));
    menu_input(0, 0, 0);
    menu_input(OM_CONFIRM, OM_CONFIRM, 0);
    assert(gOptionsMenu.warning);
    game_options_menu_close();
    game_options_menu_open();
    assert(!gOptionsMenu.warning && !game_options_get(GO_DEBUG_MODE));
    assert(gOptionsMenu.ids[gOptionsMenu.selected] == GO_DEBUG_MODE);
    menu_input(0, 0, 0);
    menu_category(GO_CONTROLS);
    menu_select(GO_CONTROLLER);
    assert(game_options_set(GO_CAMERA_SPEED, 5));
    collect_options();
    menu_select(GO_CONTROLLER);
    game_options_menu_close();
    assert(game_options_set(GO_CAMERA_SPEED, 2));
    game_options_menu_open();
    assert(gOptionsMenu.ids[gOptionsMenu.selected] == GO_CONTROLLER);
    menu_input(0, 0, 0);
    game_options_select_file(1);
    assert(game_options_menu_step(menuFrame++, OM_CONFIRM, OM_CONFIRM, 0));
    assert(!gOptionsMenu.open);
    game_options_menu_open();
    assert(gOptionsMenu.category == GO_CONTROLS && !gOptionsMenu.selected);
    game_options_menu_close();

    char value[32];
    game_options_set(GO_CONTROLLER_DISPLAY, 1);
    game_options_menu_value(GO_CONTROLLER_DISPLAY, value);
    assert(!strcmp(value, "Max"));
    game_options_set(GO_CONTROLLER_DISPLAY, 2);
    game_options_menu_value(GO_CONTROLLER_DISPLAY, value);
    assert(!strcmp(value, "Mini"));
    for (s32 id = 0; id < GO_COUNT; id++) {
        game_options_menu_value(id, value);
        assert(strlen(value) > 0 && strlen(value) < sizeof(value));
    }
    assert(game_options_set(GO_CAMERA_CUSTOM, 20));
    game_options_menu_value(GO_CAMERA_CUSTOM, value);
    assert(strcmp(value, "10.0") == 0);
    game_options_menu_value((enum GameOptionId) -1, value);
    assert(value[0] == 0);
}

static void debug_preferences_survive_restart(void) {
    memset(storage, 0, sizeof(storage));
    failRead = failWrite = badReadback = 0;
    reboot();
    assert(game_options_save(0));
    assert(!game_options_dirty(0));
    confirm(GO_DEBUG_MODE, 10);
    assert(game_options_dirty(0));
    assert(game_options_set(GO_FREE_MOVE, 1));
    assert(game_options_save(0));
    game_options_select_file(-1);
    reboot();
    assert(game_options_get(GO_DEBUG_MODE));
    assert(game_options_get(GO_FREE_MOVE));
    assert(!game_options_get_file(1, GO_DEBUG_MODE));
    assert(!game_options_get_file(1, GO_FREE_MOVE));
    assert(game_options_set(GO_DEBUG_MODE, 0));
    assert(game_options_set(GO_FREE_MOVE, 0));
    assert(game_options_save(0));
    reboot();
    assert(!game_options_get(GO_DEBUG_MODE));
    assert(!game_options_get(GO_FREE_MOVE));
}

static void knockback_save_migration(void) {
    for (u32 version = 1; version <= OPTIONS_VERSION; version++) {
        memset(storage, 0, sizeof(storage));
        for (s32 file = 0; file < 2; file++) {
            struct OptionsRecord record = {0};
            record.magic = OPTIONS_MAGIC;
            record.version = version;
            record.file = file;
            record.count = GO_COUNT;
            record.generation = 1;
            default_values(record.values);
            record.values[GO_KNOCKBACK] = file;
            record.checksum = record_checksum(&record);
            memcpy(storage + record_offset(file, 0), &record, sizeof(record));
        }
        reboot();
        for (s32 file = 0; file < 2; file++) {
            s32 expected = version < 3 ? !file : file;
            assert(game_options_get_file(file, GO_KNOCKBACK) == expected);
            assert(game_options_save(file));
        }
        reboot();
        for (s32 file = 0; file < 2; file++) {
            assert(game_options_get_file(file, GO_KNOCKBACK) == (version < 3 ? !file : file));
        }
    }
}

static void merged_filters_and_legacy_save(void) {
    memset(storage, 0, sizeof(storage));
    failRead = failWrite = badReadback = 0;
    reboot();
    assert(game_options_get(GO_DEBOUNCE) == 0 && game_options_get(GO_MIN_HOLD) == 0);
    char text[64];
    game_options_menu_value(GO_DEBOUNCE, text); assert(!strcmp(text, "0 frames"));
    game_options_menu_value(GO_MIN_HOLD, text); assert(!strcmp(text, "0 frames"));
    assert(game_options_set(GO_BOWSER_HOLD_SPIN, 1));
    assert(gGameOptionDefinitions[GO_BOWSER_HOLD_SPIN].category == GO_CHEATS);
    assert(!game_options_cheat(GO_BOWSER_HOLD_SPIN));
    confirm(GO_CHEATS_ENABLED, 0);
    assert(game_options_cheat(GO_BOWSER_HOLD_SPIN));
    assert(game_options_set(GO_CHEATS_ENABLED, 0));
    assert(!game_options_cheat(GO_BOWSER_HOLD_SPIN));
    game_options_menu_value(GO_BOWSER_HOLD_SPIN, text); assert(!strcmp(text, "Hold analogue stick"));
    game_options_menu_value(GO_CAMERA_SHAKE, text); assert(!strcmp(text, "On"));
    assert(game_options_set(GO_CAMERA_SHAKE, 1));
    game_options_menu_value(GO_CAMERA_SHAKE, text); assert(!strcmp(text, "Off"));
    game_options_menu_open(); menu_category(GO_ACCESSIBILITY);
    s32 debounce = 0, minimumHold = 0;
    for (s32 i = 0; i < gOptionsMenu.count; i++) {
        enum GameOptionId id = gOptionsMenu.ids[i];
        debounce += id == GO_DEBOUNCE; minimumHold += id == GO_MIN_HOLD;
        assert(id != GO_DEBOUNCE_FRAMES && id != GO_MIN_HOLD_FRAMES);
    }
    assert(debounce == 1 && minimumHold == 1);
    for (s32 file = 0; file < 2; file++) {
        struct OptionsRecord legacy = {0};
        legacy.magic = OPTIONS_MAGIC; legacy.version = 1; legacy.file = file;
        legacy.count = GO_COUNT; legacy.generation = 1;
        default_values(legacy.values);
        legacy.values[GO_DEBOUNCE] = legacy.values[GO_MIN_HOLD] = file == 0;
        legacy.values[GO_DEBOUNCE_FRAMES] = 3; legacy.values[GO_MIN_HOLD_FRAMES] = 5;
        legacy.values[GO_CAMERA_SHAKE] = file == 0 ? 2 : 1;
        legacy.checksum = record_checksum(&legacy);
        memcpy(storage + record_offset(file, 0), &legacy, sizeof(legacy));
    }
    reboot();
    assert(game_options_get(GO_DEBOUNCE) == 3 && game_options_get(GO_MIN_HOLD) == 5);
    assert(game_options_get(GO_CAMERA_SHAKE) == 1);
    assert(game_options_save(0));
    reboot();
    assert(game_options_get(GO_DEBOUNCE) == 3 && game_options_get(GO_MIN_HOLD) == 5);
    game_options_select_file(1);
    assert(game_options_get(GO_DEBOUNCE) == 0 && game_options_get(GO_MIN_HOLD) == 0);
    assert(game_options_get(GO_CAMERA_SHAKE) == 0);
}

static void records_and_eligibility(void) {
    game_options_select_file(0);
    game_timing_select_file(0);
    game_timing_level_entered(COURSE_BOB, 9, 0);
    game_options_menu_open();
    menu_input(0, 0, 0);
    menu_category(GO_PRESENTATION);
    menu_select(GO_BEST_TIMES);
    menu_input(0, 0, 0); menu_input(OM_CONFIRM, OM_CONFIRM, 0);
    assert(gOptionsMenu.records && gOptionsMenu.recordCourse == COURSE_BOB);
    menu_input(0, 0, 0); menu_input(OM_LEFT, OM_LEFT, 0);
    assert(gOptionsMenu.recordCourse == COURSE_CAKE_END - 1);
    menu_input(0, 0, 0); menu_input(OM_RIGHT, OM_RIGHT, 0);
    assert(gOptionsMenu.recordCourse == COURSE_BOB);
    for (s32 navigation = 0; navigation < 3; navigation++) {
        assert(game_options_set(GO_MENU_NAVIGATION, navigation));
        menu_input(0, 0, 0); menu_input(OM_PREVIOUS, OM_PREVIOUS, 0);
        assert(gOptionsMenu.recordCourse == COURSE_CAKE_END - 1);
        menu_input(0, 0, 0); menu_input(OM_NEXT, OM_NEXT, 0);
        assert(gOptionsMenu.recordCourse == COURSE_BOB);
        menu_input(0, 0, 0);
        menu_input(OM_PREVIOUS | OM_NEXT, OM_PREVIOUS | OM_NEXT, 0);
        assert(gOptionsMenu.recordCourse == COURSE_BOB);
        menu_input(OM_NEXT, 0, 0);
        assert(gOptionsMenu.recordCourse == COURSE_BOB);
        menu_input(0, 0, 0);
    }
    assert(game_options_set(GO_MENU_NAVIGATION, 2));
    menu_input(0, 0, 0); menu_input(OM_RESET, OM_RESET, 0);
    assert(gOptionsMenu.records);
    game_options_menu_close();
    game_options_menu_open();
    assert(gOptionsMenu.records && gOptionsMenu.recordCourse == COURSE_BOB);
    menu_input(0, 0, 0); menu_input(OM_CANCEL, OM_CANCEL, 0);
    assert(!gOptionsMenu.records && gOptionsMenu.open);
    assert(gOptionsMenu.ids[gOptionsMenu.selected] == GO_BEST_TIMES);
    game_options_menu_close();

    game_options_set_record_invalidator(game_timing_disqualify_file);
    const enum GameOptionId permissions[] = {GO_CHEATS_ENABLED, GO_DEBUG_MODE};
    for (unsigned i=0;i<sizeof(permissions)/sizeof(*permissions);i++) {
        enum GameOptionId id=permissions[i];
        assert(game_options_set(id, 0));
        game_timing_select_file(0);
        game_timing_level_entered(COURSE_BOB, 9, 0);
        game_timing_update(1, 0, 1, 0);
        confirm(id, 10);
        assert(!game_timing_state()->eligible);
        assert(game_options_set(id, 0));
        assert(!game_timing_collect_star(0, COURSE_BOB, 0, 0, 0));
    }
    game_timing_select_file(0);
    game_timing_level_entered(COURSE_BOB, 9, 0);
    game_options_select_file(1);
    confirm(GO_CHEATS_ENABLED, 10);
    assert(game_timing_state()->eligible);
    game_options_select_file(0);
    game_options_restart();
    game_timing_end_course();
    game_timing_level_entered(COURSE_BOB, 9, 0);
    assert(game_timing_state()->eligible);
    // A dormant individual cheat also invalidates records immediately.
    assert(game_options_set(GO_IGNORE_WIND, 1));
    assert(game_options_set(GO_IGNORE_WIND, 0));
    assert(!game_timing_state()->eligible && game_options_visit_cheating(0));
    game_options_restart();
    game_timing_end_course();
    game_timing_level_entered(COURSE_BOB, 9, game_options_visit_cheating(0));
    assert(game_timing_state()->eligible);
}

static void explicit_replacement_after_read_failure(void) {
    memset(storage, 0xA5, sizeof(storage));
    reboot();
    assert(game_options_set(GO_MASTER_VOLUME, 40));
    assert(game_options_save(0));
    game_options_select_file(1);
    assert(game_options_set(GO_MASTER_VOLUME, 20));
    assert(game_options_save(1));
    // A failed initial read must still block ordinary saves after the device recovers.
    failedReadOffset = record_offset(1, 0);
    reboot();
    failedReadOffset = ~(u32) 0;
    s32 before = writes;
    assert(!game_options_save(1) && writes == before);
    // Copy is an explicit replacement, not an implicit write of unread defaults.
    game_options_copy(0, 1);
    assert(game_options_save(1));
    reboot();
    assert(game_options_get_file(1, GO_MASTER_VOLUME) == 40);
    failedReadOffset = record_offset(0, 0);
    reboot();
    failedReadOffset = ~(u32) 0;
    before = writes;
    assert(!game_options_save(0) && writes == before);
    game_options_copy(0, 1);
    assert(game_options_get_file(1, GO_MASTER_VOLUME) == 40);
    assert(!game_options_dirty(1));
    assert(writes == before);
    game_options_erase(0);
    failWrite = 1;
    assert(!game_options_save(0) && game_options_dirty(0));
    failWrite = 0;
    assert(game_options_save(0));
    reboot();
    assert(game_options_get(GO_MASTER_VOLUME) == 100);
    assert(game_options_get_file(1, GO_MASTER_VOLUME) == 40);
    for (s32 i = 0; i < 0x800; i++) { assert(storage[i] == 0xA5); }
    for (s32 i = 0x1000; i < 0x8000; i++) { assert(storage[i] == 0xA5); }
}

static void newer_preferences_are_preserved(void) {
    for (s32 extension = 0; extension < 2; extension++) {
        memset(storage, 0xA5, sizeof(storage));
        reboot();
        assert(game_options_set(GO_MASTER_VOLUME, 40));
        assert(game_options_save(0));
        game_options_select_file(1);
        assert(game_options_set(GO_MASTER_VOLUME, 20));
        assert(game_options_save(1));
        game_options_select_file(0);
        struct OptionsRecord record;
        memcpy(&record, storage + record_offset(0, sBank[0]), sizeof(record));
        record.generation++;
        if (extension) { record.count = GO_COUNT + 1; }
        else { record.version = OPTIONS_VERSION + 1; }
        record.checksum = record_checksum(&record);
        memcpy(storage + record_offset(0, 1 - sBank[0]), &record, sizeof(record));
        u8 before[sizeof(storage)];
        memcpy(before, storage, sizeof(before));
        reboot();
        assert(game_options_get(GO_MASTER_VOLUME) == 40); // Read the older supported backup.
        assert(game_options_set(GO_MASTER_VOLUME, 50));
        s32 beforeWrites = writes;
        assert(!game_options_save(0) && writes == beforeWrites);
        assert(memcmp(before, storage, sizeof(before)) == 0);
        game_options_copy(0, 1);
        assert(game_options_get_file(1, GO_MASTER_VOLUME) == 20);
        assert(!game_options_dirty(1));
        game_options_erase(0);
        assert(game_options_save(0));
        reboot();
        assert(game_options_get(GO_MASTER_VOLUME) == 100);
    }
}

static void held_opening_stick_is_latched(void) {
    for (s32 navigation = 0; navigation < 3; navigation++) {
        for (s32 release = 0; release < 2; release++) {
            game_options_init(NULL, NULL);
            game_options_select_file(0);
            assert(game_options_set(GO_MENU_NAVIGATION, navigation));
            assert(game_options_set(GO_MENU_STICK_RELEASE, release));
            memset(&gOptionsMenu, 0, sizeof(gOptionsMenu));
            game_options_menu_open();
            menu_select(GO_CAMERA_SPEED);
            menu_input(0, 0, OM_RIGHT);
            for (s32 i = 0; i < 40; i++) { menu_input(0, 0, OM_RIGHT); }
            assert(game_options_get(GO_CAMERA_SPEED) == 2);
            menu_input(0, 0, 0);
            menu_input(0, 0, OM_RIGHT);
            assert(game_options_get(GO_CAMERA_SPEED) == (navigation == 1 ? 2 : 3));
        }
    }
}

static void conflicting_menu_directions(void) {
    for (s32 navigation = 0; navigation < 3; navigation++) {
        for (s32 release = 0; release < 2; release++) {
            game_options_init(NULL, NULL);
            game_options_select_file(0);
            game_options_set(GO_MENU_NAVIGATION, navigation);
            game_options_set(GO_MENU_STICK_RELEASE, release);
            memset(&gOptionsMenu, 0, sizeof(gOptionsMenu));
            game_options_menu_open();
            menu_input(0, 0, 0);
            menu_select(GO_CAMERA_SPEED);
            u8 initial = gOptionsMenu.selected;
            for (u8 mask = 1; mask < 16; mask++) {
                if (!(mask & (mask - 1))) { continue; }
                for (s32 source = 0; source < 2; source++) {
                    menu_input(0, 0, 0);
                    for (s32 frame = 0; frame < 30; frame++) {
                        menu_input(source ? 0 : mask, source ? 0 : mask, source ? mask : 0);
                        assert(gOptionsMenu.selected == initial);
                        assert(game_options_get(GO_CAMERA_SPEED) == 2);
                    }
                }
            }
            // A clean Down moves once; adding Left or removing it cannot create another step.
            for (s32 source = 0; source < 2; source++) {
                s32 enabled = source ? navigation != 1 : navigation != 0;
                menu_input(0, 0, 0);
                menu_select(GO_CAMERA_SPEED);
                menu_input(source ? 0 : OM_DOWN, source ? 0 : OM_DOWN, source ? OM_DOWN : 0);
                u8 afterDown = gOptionsMenu.selected;
                assert(afterDown == initial + enabled);
                for (s32 frame = 0; frame < 30; frame++) {
                    u8 mask = OM_DOWN | OM_LEFT;
                    menu_input(source ? 0 : mask, source ? 0 : OM_LEFT, source ? mask : 0);
                    assert(gOptionsMenu.selected == afterDown);
                }
                for (s32 frame = 0; frame < 30; frame++) {
                    menu_input(source ? 0 : OM_DOWN, 0, source ? OM_DOWN : 0);
                    assert(gOptionsMenu.selected == afterDown);
                }
                menu_input(0, 0, 0);
                menu_input(source ? 0 : OM_DOWN, source ? 0 : OM_DOWN, source ? OM_DOWN : 0);
                assert(gOptionsMenu.selected == afterDown + enabled);
            }
            // Cross-source conflicts are rejected when Both is selected.
            if (navigation == 2) {
                for (u8 pad = 1; pad <= OM_RIGHT; pad <<= 1) {
                    for (u8 stick = 1; stick <= OM_RIGHT; stick <<= 1) {
                        if (pad == stick) { continue; }
                        menu_input(0, 0, 0);
                        menu_select(GO_CAMERA_SPEED);
                        for (s32 frame = 0; frame < 30; frame++) {
                            menu_input(pad, pad, stick);
                            assert(gOptionsMenu.selected == initial);
                            assert(game_options_get(GO_CAMERA_SPEED) == 2);
                        }
                    }
                }
            }
            // D-pad repeat follows the same release setting as the stick.
            if (navigation != 0) {
                menu_input(0, 0, 0);
                menu_select(GO_CAMERA_SPEED);
                menu_input(OM_RIGHT, OM_RIGHT, 0);
                assert(game_options_get(GO_CAMERA_SPEED) == 3);
                for (s32 frame = 0; frame < 10; frame++) { menu_input(OM_RIGHT, 0, 0); }
                assert(game_options_get(GO_CAMERA_SPEED) == (release ? 3 : 4));
            }
        }
    }
}

// Golden raw-byte vectors preserve the legacy serialized checksum prefix.
static void checksum_golden_vectors(void) {
    struct OptionsRecord record = { 0 };
    u8 *bytes = (u8 *) &record;

    assert(offsetof(struct OptionsRecord, checksum) == 116);
    assert(record_checksum(&record) == 0x1c990fd5u);
    for (u32 i = 0; i < 116; i++) {
        bytes[i] = (u8) i;
    }
    assert(record_checksum(&record) == 0x04889861u);
}

// A checksum failure must reject an otherwise valid newer bank.
static void corrupt_checksum_uses_backup(void) {
    struct OptionsRecord good = { 0 };
    struct OptionsRecord bad;

    failRead = failWrite = badReadback = 0;
    failedReadOffset = ~(u32) 0;
    memset(storage, 0, sizeof(storage));
    good.magic = OPTIONS_MAGIC;
    good.version = OPTIONS_VERSION;
    good.generation = 1;
    good.file = 0;
    good.count = GO_COUNT;
    default_values(good.values);
    good.values[GO_MASTER_VOLUME] = 90;
    good.checksum = record_checksum(&good);
    bad = good;
    bad.generation++;
    bad.values[GO_MASTER_VOLUME] = 80;
    bad.checksum = record_checksum(&bad) ^ 1u;
    memcpy(storage + record_offset(0, 0), &good, sizeof(good));
    memcpy(storage + record_offset(0, 1), &bad, sizeof(bad));

    reboot();
    assert(game_options_get(GO_MASTER_VOLUME) == 90);
    assert(game_options_dirty(0));
    assert(game_options_save(0));
    assert(!game_options_dirty(0));
    reboot();
    assert(game_options_get(GO_MASTER_VOLUME) == 90);
    assert(!game_options_dirty(0));
}

int main(void) {
    checksum_golden_vectors();
    memset(storage, 0xA5, sizeof(storage));
    game_timing_init(read_times, NULL, NULL);
    reboot();
    for (s32 id = 0; id < GO_COUNT; id++) {
        assert(game_options_get(id) == gGameOptionDefinitions[id].defaultValue);
    }
    assert(!game_options_set(GO_CAMERA_CUSTOM, 0));
    assert(game_options_set(GO_CAMERA_CUSTOM, 20));
    assert(!game_options_set(GO_CAMERA_CUSTOM, 21));
    assert(!game_options_set(GO_MASTER_VOLUME, 95));
    assert(game_options_set(GO_MASTER_VOLUME, 90));
    assert(game_options_set(GO_INPUT_DISPLAY, 1));
    assert(game_options_get(GO_CONTROLLER_DISPLAY) == 1);
    assert(game_options_set(GO_CONTROLLER_DISPLAY, 0));
    assert(game_options_get(GO_INPUT_DISPLAY) == 1);
    assert(game_options_set(GO_INPUT_DISPLAY, 1)); // No transition: keep hidden.
    assert(game_options_get(GO_CONTROLLER_DISPLAY) == 0);
    assert(game_options_set(GO_CONTROLLER_DISPLAY, 2));
    assert(game_options_set(GO_INPUT_DISPLAY, 0));
    assert(game_options_set(GO_INPUT_DISPLAY, 1));
    assert(game_options_get(GO_CONTROLLER_DISPLAY) == 2);
    assert(game_options_set(GO_BOWSER_FIGHTS, 1));
    assert(game_options_set(GO_BOWSER_FIGHTS, 1));
    assert(game_options_set(GO_CONTROLLER_DISPLAY, 0));
    assert(game_options_set(GO_CONTROLLER_NUMBERS, 0));
    assert(game_options_save(0));
    assert(!game_options_dirty(0));
    assert(writes == 2);
    for (s32 i = 0; i < 0x800; i++) { assert(storage[i] == 0xA5); }
    for (s32 i = 0x1000; i < 0x8000; i++) { assert(storage[i] == 0xA5); }
    reboot();
    assert(game_options_get(GO_CAMERA_CUSTOM) == 20);
    assert(game_options_get(GO_MASTER_VOLUME) == 90);
    assert(game_options_get(GO_INPUT_DISPLAY) == 1);
    assert(game_options_get(GO_CONTROLLER_DISPLAY) == 0);
    assert(game_options_get(GO_CONTROLLER_NUMBERS) == 0);
    game_options_select_file(1);
    assert(game_options_get(GO_CAMERA_CUSTOM) == 8);
    game_options_select_file(0);
    assert(game_options_set(GO_MASTER_VOLUME, 80));
    failWrite = 1;
    assert(!game_options_save(0) && game_options_dirty(0));
    failWrite = 0;
    reboot();
    assert(game_options_get(GO_MASTER_VOLUME) == 90);
    assert(game_options_set(GO_MASTER_VOLUME, 80));
    badReadback = 1;
    assert(!game_options_save(0) && game_options_dirty(0));
    badReadback = 0;
    assert(game_options_save(0));
    reboot();
    assert(game_options_get(GO_MASTER_VOLUME) == 80);
    storage[record_offset(0, sBank[0])] ^= 1;
    reboot();
    assert(game_options_get(GO_MASTER_VOLUME) == 90);
    assert(game_options_dirty(0));
    assert(game_options_save(0));

    // Valid CRC does not authorize invalid values or enable consent.
    struct OptionsRecord record;
    memcpy(&record, storage + record_offset(0, sBank[0]), sizeof(record));
    record.generation++;
    record.values[GO_DEADZONE] = 255;
    record.values[GO_CHEATS_ENABLED] = 1;
    record.checksum = record_checksum(&record);
    memcpy(storage + record_offset(0, 1 - sBank[0]), &record, sizeof(record));
    reboot();
    assert(game_options_get(GO_DEADZONE) == 6);
    assert(game_options_get(GO_CHEATS_ENABLED) == 0);
    assert(game_options_dirty(0));
    game_options_copy(0, 1);
    assert(game_options_save(1));
    game_options_erase(0);
    assert(game_options_save(0));
    reboot();
    assert(game_options_get(GO_MASTER_VOLUME) == 100);
    assert(game_options_get_file(1, GO_MASTER_VOLUME) == 90);

    // An older shorter catalog receives defaults for appended IDs, without
    // moving the CRC or overwriting its existing preferences.
    memcpy(&record, storage + record_offset(0, sBank[0]), sizeof(record));
    record.generation++;
    record.count = GO_CAMERA_CUSTOM;
    record.values[GO_CAMERA_CUSTOM] = 20;
    record.checksum = record_checksum(&record);
    memcpy(storage + record_offset(0, 1 - sBank[0]), &record, sizeof(record));
    reboot();
    assert(game_options_get(GO_CAMERA_CUSTOM) == 8);
    assert(game_options_get(GO_MASTER_VOLUME) == 100);
    assert(game_options_get(GO_CANNON_ANGLES) == 0);
    assert(game_options_get(GO_CONTROLLER_NUMBERS) == 1);
    assert(game_options_get(GO_MARIO_SILHOUETTE) == 1);
    record.file = 3;
    record.generation++;
    record.checksum = record_checksum(&record);
    memcpy(storage + record_offset(0, 1 - sBank[0]), &record, sizeof(record));
    reboot();
    assert(game_options_get(GO_MASTER_VOLUME) == 100);
    // Generations compare correctly across wrap.
    record.file = 0;
    record.count = GO_COUNT;
    record.generation = 0xFFFFFFFEu;
    record.values[GO_MASTER_VOLUME] = 70;
    record.checksum = record_checksum(&record);
    memcpy(storage + record_offset(0, 0), &record, sizeof(record));
    record.generation = 1;
    record.values[GO_MASTER_VOLUME] = 60;
    record.checksum = record_checksum(&record);
    memcpy(storage + record_offset(0, 1), &record, sizeof(record));
    reboot();
    assert(game_options_get(GO_MASTER_VOLUME) == 60);
    failRead = 1;
    reboot();
    s32 writesBefore = writes;
    assert(!game_options_save(0));
    assert(writes == writesBefore);
    failRead = 0;
    reboot();
    assert(game_options_get(GO_MASTER_VOLUME) == 60);

    assert(!game_options_set(GO_CHEATS_ENABLED, 1));
    assert(!game_options_set(GO_DEBUG_MODE, 1));
    assert(game_options_confirmation_begin(GO_CHEATS_ENABLED));
    for (u32 frame = 0; frame < 300; frame++) {
        assert(!game_options_confirmation_update(frame, 1, 0));
    }
    assert(!game_options_confirmation_update(300, 0, 0));
    for (s32 i = 1; i < OPTIONS_CONFIRM_FRAMES; i++) {
        assert(!game_options_confirmation_update(300 + i, 1, 0));
        assert(!game_options_confirmation_update(300 + i, 1, 0));
    }
    assert(!game_options_confirmation_update(300 + OPTIONS_CONFIRM_FRAMES, 0, 0));
    confirm(GO_CHEATS_ENABLED, 1000);
    assert(game_options_get(GO_CHEATS_ENABLED));
    assert(game_options_set(GO_INFINITE_HEALTH, 1));
    assert(game_options_cheat(GO_INFINITE_HEALTH));
    assert(game_options_set(GO_CHEATS_ENABLED, 0));
    assert(!game_options_cheat(GO_INFINITE_HEALTH));
    confirm(GO_DEBUG_MODE, 0xFFFFFF80u);
    assert(game_options_get(GO_DEBUG_MODE));
    assert(game_options_confirmation_begin(GO_CHEATS_ENABLED));
    assert(!game_options_confirmation_update(5, 0, 0));
    assert(!game_options_confirmation_update(6, 1, 0));
    assert(!game_options_confirmation_update(8, 1, 0));
    for (u32 f = 9; f < 200; f++) {
        assert(!game_options_confirmation_update(f, 1, 0));
    }
    game_options_select_file(1);
    assert(!game_options_confirmation_update(200, 1, 0));
    assert(game_options_get_file(0, GO_DEBUG_MODE));
    assert(!game_options_get(GO_CHEATS_ENABLED));
    assert(game_options_confirmation_begin(GO_DEBUG_MODE));
    assert(!game_options_confirmation_update(1, 0, 1));
    assert(!game_options_confirmation_update(2, 1, 0));
    game_options_select_file(-1);
    assert(!game_options_set(GO_MASTER_VOLUME, 0));
    assert(game_options_get(GO_MASTER_VOLUME) == 100);
    assert(!game_options_save(-1));
    assert(!game_options_set((enum GameOptionId) -1, 0));
    menu_file_switch_returns();
    menu_tests();
    merged_filters_and_legacy_save();
    knockback_save_migration();
    debug_preferences_survive_restart();
    restart_rules();
    reset_tests();
    records_and_eligibility();
    explicit_replacement_after_read_failure();
    newer_preferences_are_preserved();
    held_opening_stick_is_latched();
    conflicting_menu_directions();
    thicc_text_preference();
    corrupt_checksum_uses_backup();
    warning_stick_returns();
    puts("options storage, confirmation and menu checks passed");
    return 0;
}
'''


class GameOptionsTests(unittest.TestCase):
    def test_storage_and_confirmation(self):
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            (directory / "PR").mkdir()
            (directory / "PR/ultratypes.h").write_text(
                "#include <stdint.h>\n"
                "typedef uint8_t u8; typedef int8_t s8;\n"
                "typedef uint16_t u16; typedef int16_t s16;\n"
                "typedef uint32_t u32; typedef int32_t s32;\n"
                "typedef uint64_t u64;\n"
                "typedef float f32;\n"
            )
            fixture = directory / "fixture.c"
            fixture.write_text(FIXTURE)
            for region in ([], ["-DVERSION_EU"]):
                executable = directory / "options-test"
                subprocess.run([
                    "cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                    "-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-fno-omit-frame-pointer",
                    *region, "-I", str(directory), "-I", str(ROOT / "src/game"),
                    "-I", str(ROOT / "include"), "-I", str(ROOT),
                    str(fixture), str(ROOT / "src/game/game_timing.c"), "-o", str(executable),
                ], check=True, timeout=30)
                environment = dict(os.environ, ASAN_OPTIONS="detect_leaks=0")
                subprocess.run([str(executable)], check=True, env=environment, timeout=10)


if __name__ == "__main__":
    unittest.main()
