#!/usr/bin/env python3
"""Production input history: simultaneous holds, direction changes, resets and bounds."""
import re
import subprocess
import tempfile
import unittest
from pathlib import Path

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class InputDisplayTests(unittest.TestCase):
    def test_history(self):
        with tempfile.TemporaryDirectory() as directory:
            directory = Path(directory)
            (directory / 'PR').mkdir()
            (directory / 'PR/ultratypes.h').write_text('''
#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t s32;
typedef int16_t s16;
typedef float f32;
''')
            (directory / 'config.h').write_text('#define INPUT_DISPLAY\n')
            (directory / 'ultra64.h').write_text('''
#include <PR/ultratypes.h>
#define A_BUTTON 0x8000
#define B_BUTTON 0x4000
#define Z_TRIG 0x2000
#define START_BUTTON 0x1000
#define U_JPAD 0x0800
#define D_JPAD 0x0400
#define L_JPAD 0x0200
#define R_JPAD 0x0100
#define L_TRIG 0x0020
#define R_TRIG 0x0010
#define U_CBUTTONS 0x0008
#define D_CBUTTONS 0x0004
#define L_CBUTTONS 0x0002
#define R_CBUTTONS 0x0001
''')
            display = Source(ROOT / 'src/game/input_display.c')
            math_source = Source(ROOT / 'src/engine/math_util.c')
            game_source = Source(ROOT / 'src/game/game_init.c')
            table = re.search(r's16 gArctanTable\[0x401\] = \{.*?\};',
                              (ROOT / 'include/trig_tables.inc.c').read_text(), re.S).group()
            production = table + '\n#define atans(x) gArctanTable[(s32)(((x)*1024)+0.5f)]\n'
            production += math_source.function('atan2_lookup') + math_source.function('atan2s')
            production += game_source.function('adjust_analog_stick') + display.function('input_display_update')
            production += display.function('input_display_enabled')
            production += display.function('input_display_history')
            initial_state = r'''
enum { GO_INPUT_DISPLAY, GO_CONTROLLER_DISPLAY, GO_DEADZONE };
static s32 sDisplayOptions[3] = {0,0,6};
static s32 game_options_get(s32 id) { return sDisplayOptions[id]; }
static s32 game_options_set(s32 id,s32 value) {
    sDisplayOptions[id] = value;
    if (id == GO_INPUT_DISPLAY && value && !sDisplayOptions[GO_CONTROLLER_DISPLAY]) {
        sDisplayOptions[GO_CONTROLLER_DISPLAY] = 1;
    }
    return TRUE;
}
'''
            check = directory / 'check.c'
            check.write_text(r'''
#include <assert.h>
#include <math.h>
#include <string.h>
#include <ultra64.h>
#include "input_history.h"
#define TRUE 1
#define FALSE 0
static u32 gGlobalTimer;

struct Controller {
    s16 rawStickX, rawStickY;
    f32 stickX, stickY, stickMag;
    u16 buttonDown;
};
static struct InputHistory sInputHistory;
static f32 sInputStickX, sInputStickY, sInputStickMagnitude;
static u16 sStickAngleBinary, sStickPowerPercent100;

#define MAX(a,b) ((a) > (b) ? (a) : (b))
struct InputDisplaySprite { u8 unused; };
static const struct InputDisplaySprite sInputDisplayNeutral, sInputDisplayBracketVertical, sInputDisplayBracketCap;
static s32 headerCount, rightColumnValues, visibleRows;
static void print_set_envcolour(u8 r, u8 g, u8 b, u8 a) {
    (void)r; (void)g; (void)b; (void)a;
}
static void input_display_text(const char *text, s32 x, s32 y) {
    (void)x; (void)y;
    if (strcmp(text, "FR") == 0) headerCount++;
}
static void input_display_count(const char *text, s32 x, s32 y) {
    visibleRows++;
    input_display_text(text, x, y);
}
static void input_display_texture(const struct InputDisplaySprite *sprite, s32 x, s32 y) {
    (void)sprite; (void)x; (void)y;
}
static void input_display_sprite(const struct InputDisplaySprite *sprite, s32 x, s32 y, s32 w, s32 h) {
    (void)sprite; (void)x; (void)y; (void)w; (void)h;
}
static void input_display_entry(const struct InputHistoryEntry *entry, u8 channel, s32 x, s32 countX, s32 y) {
    (void)entry; (void)channel; (void)x; (void)y;
    if (countX > 50) rightColumnValues++;
}
''' + initial_state + '\n' + production + r'''
static void check_visible_columns(s32 expectedHeaders, s32 usesRightColumn) {
    headerCount = rightColumnValues = 0;
    input_display_history();
    assert(headerCount == expectedHeaders);
    assert((rightColumnValues != 0) == usesRightColumn);
}
int main(void) {
    struct InputHistory h;
    u8 channels[INPUT_HISTORY_CHANNELS];
    char text[INPUT_HISTORY_COUNT_TEXT_SIZE];
    assert(!input_display_enabled());
    assert(game_options_set(GO_INPUT_DISPLAY, 1) && input_display_enabled());
    struct Controller held = { .buttonDown=A_BUTTON };
    input_display_update(&held);
    gGlobalTimer++;
    input_display_update(&held);
    assert(sInputHistory.entries[0].held[INPUT_A] == 2);
    assert(game_options_set(GO_INPUT_DISPLAY, 0) && input_display_enabled());
    assert(!game_options_get(GO_INPUT_DISPLAY) && game_options_get(GO_CONTROLLER_DISPLAY));
    input_history_reset(&h);
    assert(h.length == 0);
    for (int frame = 0; frame < 27; frame++) {
        input_history_update(&h, 0, INPUT_STICK_NEUTRAL);
    }
    assert(h.length == 1 && h.entries[0].neutralFrames == 27);
    input_history_update(&h, A_BUTTON, 1);
    assert(h.length == 2 && h.entries[1].neutralFrames == 27);
    assert(h.entries[0].held[INPUT_A] == 1 && h.entries[0].held[INPUT_STICK] == 1);
    for (int frame = 0; frame < 11; frame++) {
        input_history_update(&h, A_BUTTON, 1);
    }
    input_history_update(&h, A_BUTTON | L_JPAD, 1);
    assert(h.entries[0].held[INPUT_A] == 13 && h.entries[0].held[INPUT_STICK] == 13);
    assert(h.entries[0].held[INPUT_D_LEFT] == 1);
    assert(h.entries[1].held[INPUT_A] == 12 && h.entries[1].held[INPUT_STICK] == 12);
    input_history_update(&h, A_BUTTON | L_JPAD, 2);
    assert(h.entries[0].held[INPUT_A] == 14 && h.entries[0].held[INPUT_D_LEFT] == 2);
    assert(h.entries[0].held[INPUT_STICK] == 1);
    input_history_update(&h, A_BUTTON, INPUT_STICK_NEUTRAL);
    assert(h.entries[0].held[INPUT_A] == 15);
    assert(h.entries[0].held[INPUT_D_LEFT] == 0 && h.entries[0].held[INPUT_STICK] == 0);
    input_history_update(&h, 0, INPUT_STICK_NEUTRAL);
    input_history_update(&h, A_BUTTON, INPUT_STICK_NEUTRAL);
    assert(h.entries[0].held[INPUT_A] == 1);
    for (int sector = 0; sector < 8; sector++) {
        input_history_update(&h, 0xFF3F, sector);
        assert(input_history_channels(&h.entries[0], channels) == INPUT_HISTORY_CHANNELS);
        for (int channel = 0; channel < INPUT_HISTORY_CHANNELS; channel++) {
            assert(channels[channel] == channel && h.entries[0].held[channel] != 0);
        }
        assert(h.entries[0].sector == sector);
    }
    assert(h.length == INPUT_HISTORY_LENGTH);
    for (unsigned combination = 0; combination < (1u << 14); combination++) {
        unsigned buttons = 0;
        unsigned expected = 0;
        input_history_reset(&h);
        for (int channel = 1; channel < INPUT_HISTORY_CHANNELS; channel++) {
            if (combination & (1u << (channel - 1))) {
                buttons |= gInputHistoryButtons[channel];
                expected++;
            }
        }
        input_history_update(&h, buttons, INPUT_STICK_NEUTRAL);
        assert(input_history_channels(&h.entries[0], channels) == expected);
        for (int channel = 1; channel < INPUT_HISTORY_CHANNELS; channel++) {
            assert(h.entries[0].held[channel] == !!(buttons & gInputHistoryButtons[channel]));
        }
    }
    input_history_update(&h, 0xFF3F, 0);
    for (int channel = 0; channel < INPUT_HISTORY_CHANNELS; channel++) {
        h.entries[0].held[channel] = 0xFFFFFFFFu;
    }
    input_history_update(&h, 0xFF3F, 0);
    for (int channel = 0; channel < INPUT_HISTORY_CHANNELS; channel++) {
        assert(h.entries[0].held[channel] == 0xFFFFFFFFu);
    }
    input_history_reset(&h);
    input_history_update(&h, 0, INPUT_STICK_NEUTRAL);
    h.entries[0].neutralFrames = 0xFFFFFFFFu;
    input_history_update(&h, 0, INPUT_STICK_NEUTRAL);
    assert(h.entries[0].neutralFrames == 0xFFFFFFFFu);
    input_history_reset(&h);
    input_history_update(&h, 0x00C0, 255);
    assert(h.length == 1 && h.entries[0].sector == INPUT_STICK_NEUTRAL);
    assert(h.entries[0].buttons == 0 && h.entries[0].neutralFrames == 1);
    assert(input_history_channels(&h.entries[0], channels) == 0);
    for (int x = -128; x <= 127; x++) {
        for (int y = -128; y <= 127; y++) {
            struct Controller pad = { .rawStickX=x, .rawStickY=y, .buttonDown=A_BUTTON|L_JPAD };
            adjust_analog_stick(&pad);
            struct Controller before = pad;
            input_display_update(&pad);
            assert(memcmp(&pad, &before, sizeof(pad)) == 0);
            assert(sInputHistory.entries[0].buttons == (A_BUTTON|L_JPAD));
            assert(sInputHistory.entries[0].held[INPUT_A] != 0 && sInputHistory.entries[0].held[INPUT_D_LEFT] != 0);
            assert(sStickPowerPercent100 <= 10000);
            assert(sInputStickX == x && sInputStickY == y);
            if (x == 0 && y == 0) {
                assert(sInputHistory.entries[0].sector == INPUT_STICK_NEUTRAL);
            } else {
                assert(sInputHistory.entries[0].sector < 8);
                assert(sInputHistory.entries[0].held[INPUT_STICK] != 0);
                if (x >= -6 && x <= 6 && y >= -6 && y <= 6) {
                    assert(sStickPowerPercent100 == 0); // ignored by gameplay, still visible in raw history
                }
            }
        }
    }
    struct Controller right = { .rawStickX=64 };
    struct Controller up = { .rawStickY=64 };
    struct Controller left = { .rawStickX=-64 };
    struct Controller down = { .rawStickY=-64 };
    input_display_update(&right); assert(sStickAngleBinary == 0);
    input_display_update(&up); assert(sStickAngleBinary == 0x4000);
    input_display_update(&left); assert(sStickAngleBinary == 0x8000);
    input_display_update(&down); assert(sStickAngleBinary == 0xC000);
    input_history_reset(&sInputHistory);
    input_history_update(&sInputHistory, B_BUTTON, INPUT_STICK_NEUTRAL);
    check_visible_columns(0, 0);
    input_history_update(&sInputHistory, A_BUTTON | B_BUTTON | Z_TRIG | L_TRIG, INPUT_STICK_NEUTRAL);
    check_visible_columns(0, 1);
    // Both headings stay absent; visible frame counts still use the correct columns.
    input_history_update(&sInputHistory, A_BUTTON | B_BUTTON | Z_TRIG, INPUT_STICK_NEUTRAL);
    input_history_update(&sInputHistory, A_BUTTON | B_BUTTON | L_TRIG, INPUT_STICK_NEUTRAL);
    input_history_update(&sInputHistory, A_BUTTON | B_BUTTON | Z_TRIG, INPUT_STICK_NEUTRAL);
    check_visible_columns(0, 0);
    input_history_reset(&sInputHistory);
    input_history_update(&sInputHistory, A_BUTTON | B_BUTTON | Z_TRIG | L_TRIG, INPUT_STICK_NEUTRAL);
    check_visible_columns(0, 1);
    sInputHistory.entries[0].held[INPUT_A] = 1001;
    check_visible_columns(0, 0);
    memset(&sInputHistory, 0, sizeof(sInputHistory));
    sInputHistory.length = INPUT_HISTORY_LENGTH;
    for (s32 i = 0; i < INPUT_HISTORY_LENGTH; i++) {
        sInputHistory.entries[i].sector = INPUT_STICK_NEUTRAL;
        sInputHistory.entries[i].neutralFrames = i + 1;
    }
    for (s32 mode = 0; mode <= 2; mode++) {
        sDisplayOptions[GO_CONTROLLER_DISPLAY] = mode;
        visibleRows = 0;
        input_display_history();
        assert(visibleRows == (mode == 0 ? 15 : mode == 2 ? 12 : 10));
    }
    const unsigned counts[] = {0, 1, 999, 1000, 1001, 2345, 10000, 100000, 1000000, 100000000, 0xFFFFFFFFu};
    const char *expected[] = {"0", "1", "999", "1000", "1001", "2345", "10000", "100000", "1000000", "100000000", "4294967295"};
    for (unsigned i = 0; i < sizeof(counts) / sizeof(counts[0]); i++) {
        input_history_format_count(counts[i], text);
        assert(strcmp(text, expected[i]) == 0);
    }
    input_history_reset(&h);
    assert(h.length == 0 && h.entries[0].held[INPUT_A] == 0);
    input_history_update(&h, A_BUTTON, INPUT_STICK_NEUTRAL);
    assert(h.entries[0].held[INPUT_A] == 1);
    return 0;
}
''')
            exe = directory / 'check'
            subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror', '-I', str(directory),
                            '-I', str(ROOT / 'src/game'), str(check),
                            str(ROOT / 'src/game/input_history.c'), '-lm', '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)

    def test_generated_assets(self):
        subprocess.run(['python3', str(ROOT / 'tools/generate_input_display.py'), '--check'], check=True)


if __name__ == '__main__':
    unittest.main()
