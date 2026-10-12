#!/usr/bin/env python3
"""Exercise production statistics rendering, navigation and formatting with host services."""
from pathlib import Path
import json
import shutil
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


def prepare_fixture(directory):
    (directory / "PR").mkdir(exist_ok=True)
    (directory / "PR/ultratypes.h").write_text("""#ifndef MENU_TYPES_H
#define MENU_TYPES_H
#define FALSE 0
#include <stdint.h>
typedef int8_t s8; typedef uint8_t u8;
typedef int16_t s16; typedef uint16_t u16;
typedef int32_t s32; typedef uint32_t u32;
typedef uint64_t u64; typedef float f32;
#endif
""")
    menu = Source(ROOT / "src/game/ingame_menu.c")
    definitions = Source(ROOT / "src/game/ingame_menu.h")
    world = """#ifndef STATS_MENU_WORLD_H
#define STATS_MENU_WORLD_H
#include <PR/ultratypes.h>
#include "include/level_table.h"
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define TRUE 1
#define SCREEN_WIDTH 320
#define FALSE 0
#define A_BUTTON 0x8000
#define B_BUTTON 0x4000
#define START_BUTTON 0x1000
#define U_JPAD 0x0800
#define D_JPAD 0x0400
#define L_JPAD 0x0200
#define R_JPAD 0x0100
#define L_TRIG 0x0020
#define R_TRIG 0x0010
#define Z_TRIG 0x2000
#define SOUND_MENU_CHANGE_SELECT 2
#define SOUND_MENU_PAUSE_CHANGE_SELECT 1
#define G_MTX_MODELVIEW 0
struct Controller { u16 buttonDown, buttonPressed; s16 rawStickX, rawStickY; };
extern struct Controller *gPlayer1Controller;
extern u32 gGlobalTimer;
extern s16 gCurrSaveFileNum;
s32 save_file_is_he_thicc(s32 fileIndex);
extern f32 gGlobalSoundSource[3];
extern u16 gDialogTextAlpha;
extern int *gDisplayListHead;
extern int dl_ia_text_begin, dl_ia_text_end, dl_draw_triangle;
void stats_menu_display_list(int *command, int *list);
#define gSPDisplayList(ptr,list) stats_menu_display_list((ptr), &(list))
void stats_menu_pop_matrix(void);
#define gSPPopMatrix(ptr,flag) ((void)(ptr), (void)(flag), stats_menu_pop_matrix())
#define gDPSetEnvColor(ptr,r,g,b,a) ((void)(ptr), (void)(r), (void)(g), (void)(b), (void)(a))
#define gDPPipeSync(ptr) ((void)(ptr))
void create_dl_translation_matrix(s8 op, f32 x, f32 y, f32 z);
void create_dl_scale_matrix(s8 op, f32 x, f32 y, f32 z);
s32 get_string_width(u8 *str);
void print_generic_string(s16 x, s16 y, const u8 *text);
void play_sound(u32 sound, f32 *source);
u8 menu_scrolling_input(u8 index, u8 *held, u8 *timer);
"""
    world += definitions.declaration("enum", "MenuScrollAxis")
    world += definitions.declaration("enum", "MenuMode")
    world += menu.declaration("enum", "DialogBoxType")
    world += menu.declaration("enum", "DialogBoxState")
    world += definitions.declaration("enum", "DialogResponseDefines")
    world += Source(ROOT / "src/game/mario_character.h").declaration("enum", "MarioTextStyle")
    world += '#include "include/dialog_ids.h"\n'
    world += definitions.macros("ASCII_TO_DIALOG|DIALOG_CHAR_PLUS|DIALOG_CHAR_FORWARD_SLASH")
    world += definitions.declaration("enum", "DialogMark")
    world += definitions.declaration("enum", "DialogSpecialChars")
    world += definitions.declaration("enum", "MenuMtxPushOp")
    world += "\nstatic const s8 gLevelToCourseNumTable[LEVEL_COUNT - 1] = {0};\n"
    world += Source(ROOT / "include/config/config_menu.h").macros("MENU_SCROLL_REPEAT_.*")
    world += "\ns32 game_options_is_cheating(void);\n#endif\n"
    (directory / "stats_menu_world.h").write_text(world)
    for name in ("sm64.h", "config.h", "game_init.h", "ingame_menu.h", "level_table.h",
                 "segment2.h", "audio/external.h", "area.h", "save_file.h", "game_options_runtime.h"):
        path = directory / name
        path.parent.mkdir(exist_ok=True, parents=True)
        path.write_text('#include "stats_menu_world.h"\n')
    for name in ("player_stats.c", "player_stats.h", "player_stats_menu.c", "player_stats_menu.h",
                 "input_display.h", "game_options.h", "game_options.c"):
        shutil.copyfile(ROOT / "src/game" / name, directory / name)
    widths = menu.region(r"^u8 gDialogCharWidths", r"^s8 gDialogBoxState")
    lifecycle = """
static s32 gDialogID = DIALOG_NONE, gDialogBoxType, gDialogVariable;
static s32 gDialogBoxState, gDialogTextPos, gLastDialogResponse, gLastDialogPageStrPos;
static s32 gDialogResponse;
static f32 gDialogBoxScale, gDialogBoxOpenTimer;
static enum MarioTextStyle sDialogTextStyle;
static enum MarioTextStyle mario_character_text_style(void) { return MARIO_TEXT_STANDARD; }
static void level_set_transition(s32 frames, void *callback) {
    (void) frames;
    (void) callback;
}
"""
    for name in ("reset_menu_navigation", "create_dialog_box", "create_dialog_box_with_var",
                 "create_dialog_inverted_box", "create_dialog_box_with_response",
                 "reset_dialog_render_state", "set_menu_mode"):
        lifecycle += menu.function(name)
    (directory / "menu_services.inc.c").write_text("static u8 gMenuHoldKeyIndex, sMenuNavigationStick, sMenuNavigationDirection;\nstatic s32 gMenuMode = MENU_MODE_RENDER_PAUSE_SCREEN;\n"
        + widths + menu.function("menu_scrolling_input") + menu.function("handle_menu_scrolling")
        + lifecycle)
    fixture = ROOT / "tests/player_stats_menu_test.c"
    (directory / "compile_commands.json").write_text(json.dumps([{
        "directory": str(ROOT), "file": str(fixture),
        "arguments": ["clang", "-std=c11", "-I", str(directory), "-I", str(ROOT),
                      "-c", str(fixture)],
    }]))


class PlayerStatsMenuTests(unittest.TestCase):
    def test_navigation_formats_catalog_and_read_only_rendering(self):
        with tempfile.TemporaryDirectory(prefix="sm64-stats-menu-") as name:
            directory = Path(name)
            prepare_fixture(directory)
            for region, overlay, debug in (("us", False, False), ("eu", False, False),
                                           ("us", True, False), ("eu", True, False),
                                           ("us", False, True)):
                with self.subTest(region=region, overlay=overlay, debug=debug):
                    executable = directory / (region + str(overlay))
                    command = ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                               "-fsanitize=undefined", "-I", str(directory), "-I", str(ROOT),
                               str(directory / "player_stats.c"),
                               str(directory / "game_options.c"),
                               str(ROOT / "tests/player_stats_menu_test.c"), "-o", str(executable)]
                    if debug:
                        command.insert(1, "-DDEBUG_ALL")
                        command.insert(1, "-DPUPPYPRINT_DEBUG")
                    if region == "eu":
                        command.insert(1, "-DVERSION_EU")
                    if overlay:
                        command.insert(1, "-DINPUT_DISPLAY")
                    result = subprocess.run(command, capture_output=True, text=True, timeout=30)
                    self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                    result = subprocess.run([str(executable)], capture_output=True,
                                            text=True, timeout=10)
                    self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
