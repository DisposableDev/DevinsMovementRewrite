#!/usr/bin/env python3
"""Actual options/controller units, with supplied physical pads and no OS I/O."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]
FIXTURE = r'''
#include <assert.h>
#include <math.h>
#include <stdio.h>
#define TRUE 1
#define FALSE 0
#define UNUSED __attribute__((unused))
#include "game_options.c"
#include "game_options_input.c"
struct Controller gControllers[MAXCONTROLLERS];
OSContStatus gControllerStatuses[MAXCONTROLLERS];
OSContPadEx gControllerPads[MAXCONTROLLERS];
u8 gControllerBits;
struct Controller *gPlayer1Controller = &gControllers[0];
struct Camera { s32 unused; };
static s16 sCUpCameraPitch, sModeOffsetYaw;
static struct { s16 headRotation[2]; } marioCam, *sMarioCamState = &marioCam;
'''
CHECKS = r'''
static void sample(s32 menu) {
    game_options_input_select(menu, FALSE);
    game_options_input_read(menu, FALSE);
}
static void reset(void) {
    game_options_init(NULL, NULL);
    game_options_select_file(0);
    game_options_input_init();
    memset(gControllers, 0, sizeof(gControllers));
    memset(gControllerPads, 0, sizeof(gControllerPads));
    memset(gControllerStatuses, 0, sizeof(gControllerStatuses));
    for (s32 i = 0; i < MAXCONTROLLERS; i++) { gControllerPads[i].errno = 1; }
    gControllerPads[0].errno = 0;
    sample(FALSE);
}
static void scalar_checks(void) {
    reset();
    assert(!game_options_has_record(0));
    for (s32 dz = 0; dz <= 63; dz++) {
        assert(game_options_set(GO_DEADZONE, dz));
        struct Controller c = {0};
        c.rawStickX = dz; c.rawStickY = -dz;
        adjust_analog_stick(&c);
        assert(c.stickX == 0 && c.stickY == 0 && c.stickMag == 0);
        c.rawStickX++; c.rawStickY--;
        adjust_analog_stick(&c);
        assert(c.stickX == 1 && c.stickY == -1);
        c.rawStickX = 127; c.rawStickY = -128;
        adjust_analog_stick(&c);
        assert(c.stickMag == 64 && c.stickX > 0 && c.stickY < 0);
    }
    f32 expected[] = {2,3,4,6,8};
    for (s32 i = 0; i < 5; i++) {
        assert(game_options_set(GO_CAMERA_SPEED, i));
        assert(game_options_camera_speed() == expected[i]);
    }
    assert(game_options_set(GO_CAMERA_SPEED, 5));
    for (s32 i = 1; i <= 20; i++) {
        assert(game_options_set(GO_CAMERA_CUSTOM, i));
        assert(game_options_camera_speed() == i * 0.5f);
    }
    for (u32 buttons = 0; buttons <= 0xFFFF; buttons++) {
        u16 swapped = game_options_swap_buttons(buttons, L_CBUTTONS, R_CBUTTONS);
        assert(game_options_swap_buttons(swapped, L_CBUTTONS, R_CBUTTONS) == buttons);
        assert((swapped & ~(L_CBUTTONS | R_CBUTTONS)) ==
               (buttons & ~(L_CBUTTONS | R_CBUTTONS)));
    }
    for (s32 setting = 0; setting < 3; setting++) {
        assert(game_options_set(GO_RUMBLE, setting));
        s32 on = 0;
        for (u32 frame = 0; frame < 100; frame++) {
            assert(!game_options_rumble_motor(FALSE));
            on += game_options_rumble_motor(TRUE);
        }
        assert(on == (setting == 0 ? 0 : setting == 1 ? 50 : 100));
    }
    game_options_set(GO_RUMBLE, 1);
    s32 sparse = 0;
    for (s32 i = 0; i < 32; i++) {
        sparse += game_options_rumble_motor(i % 8 == 0);
    }
    assert(sparse == 2);
    game_options_set(GO_CAMERA_SPEED, 2);
    for (s32 x = 0; x < 2; x++) {
        for (s32 y = 0; y < 2; y++) {
            game_options_set(GO_CAMERA_INVERT_X, x);
            game_options_set(GO_CAMERA_INVERT_Y, y);
            sCUpCameraPitch = sModeOffsetYaw = 0;
            gPlayer1Controller->stickX = 2; gPlayer1Controller->stickY = 3;
            move_mario_head_c_up(NULL);
            assert(sCUpCameraPitch == (y ? -30 : 30));
            assert(sModeOffsetYaw == (x ? 20 : -20));
            assert(gPlayer1Controller->stickX == 2 && gPlayer1Controller->stickY == 3);
        }
    }
}
static void dialogue_hold(void) {
    // Dialogue input context is marked separately from ordinary menu input.
    const s32 dialogue = 3;
    reset();
    gControllerPads[0].button = A_BUTTON | B_BUTTON;
    sample(FALSE); sample(FALSE);
    assert(gPlayer1Controller->buttonDown & A_BUTTON);
    sample(dialogue);
    assert(gPlayer1Controller->buttonDown == A_BUTTON);
    assert(!(gPlayer1Controller->buttonPressed & A_BUTTON));
    // Star exit switches between gameplay and transition polling before the save menu.
    sample(GAME_OPTIONS_INPUT_DIALOGUE);
    assert(gPlayer1Controller->buttonDown == A_BUTTON && !gPlayer1Controller->buttonPressed);
    sample(dialogue);
    assert(gPlayer1Controller->buttonDown == A_BUTTON && !gPlayer1Controller->buttonPressed);
    for (int frame = 0; frame < 40; frame++) {
        sample(dialogue);
        assert(gPlayer1Controller->buttonDown == A_BUTTON);
        assert(!gPlayer1Controller->buttonPressed);
    }
    gControllerPads[0].button = 0; sample(dialogue);
    assert(gPlayer1Controller->buttonReleased == A_BUTTON);
    gControllerPads[0].button = A_BUTTON; sample(dialogue);
    assert(gPlayer1Controller->buttonPressed == A_BUTTON);
    // A partial gameplay minimum hold becomes an ordinary hold, never a fresh
    // confirmation, when the dialogue opens.
    reset(); game_options_set(GO_MIN_HOLD, 3); sample(FALSE);
    gControllerPads[0].button = A_BUTTON; sample(FALSE);
    assert(!gPlayer1Controller->buttonDown);
    sample(dialogue);
    assert(gPlayer1Controller->buttonDown == A_BUTTON && !gPlayer1Controller->buttonPressed);
    // A concurrent file/controller change and a reconnect still quarantine A.
    reset(); gControllerPads[0].button = A_BUTTON; sample(FALSE);
    game_options_select_file(1); sample(dialogue);
    assert(!gPlayer1Controller->buttonDown);
    reset(); gControllerPads[0].button = A_BUTTON; sample(FALSE);
    gControllerPads[1].errno = 0; gControllerPads[1].button = A_BUTTON;
    sample(FALSE);
    assert(game_options_set(GO_CONTROLLER, 2)); sample(dialogue);
    assert(!gPlayer1Controller->buttonDown);
    reset(); gControllerPads[0].errno = 1; sample(FALSE);
    gControllerPads[0].errno = 0; gControllerPads[0].button = A_BUTTON;
    sample(dialogue); assert(!gPlayer1Controller->buttonDown);
}

static void ports(void) {
    reset();
    for (s32 i = 0; i < MAXCONTROLLERS; i++) { gControllerPads[i].errno = 0; }
    sample(FALSE); // Availability comes from the completed physical sample.
    for (s32 port = 0; port < MAXCONTROLLERS; port++) {
        assert(game_options_controller_available(port + 1));
        game_options_set(GO_CONTROLLER, port + 1);
        gControllerPads[port].button = A_BUTTON;
        sample(FALSE);
        assert(gPlayer1Controller->port == port);
        assert(gPlayer1Controller->buttonDown == 0); // Held switch input is quarantined.
        gControllerPads[port].button = 0; sample(FALSE);
        gControllerPads[port].button = A_BUTTON; sample(FALSE);
        assert(gPlayer1Controller->buttonPressed == A_BUTTON);
        assert(gControllers[1].port != port);
        gControllerPads[port].button = 0;
    }
    game_options_set(GO_CONTROLLER, 0);
    sample(FALSE); assert(gPlayer1Controller->port == 0);
    game_options_set(GO_PAUSE_DISCONNECT, 1);
    sample(FALSE);
    gControllerPads[0].errno = 1; sample(FALSE);
    assert(gPlayer1Controller->port == 1);
    assert(gPlayer1Controller->buttonPressed == START_BUTTON);
    assert(!game_options_input_take_disconnect());
    sample(FALSE); assert(!game_options_input_take_disconnect());
    assert(!game_options_controller_available(1));
    assert(!game_options_set(GO_CONTROLLER, 1));
    assert(game_options_get(GO_CONTROLLER) == 0);
    // Unplugging an explicit selection updates the preference and quarantines the replacement.
    gControllerPads[0].errno = 0; sample(FALSE);
    assert(game_options_set(GO_CONTROLLER, 1)); sample(FALSE);
    gControllerPads[0].errno = 1;
    gControllerPads[1].button = A_BUTTON; sample(FALSE);
    assert(game_options_get(GO_CONTROLLER) == 2 && gPlayer1Controller->port == 1);
    assert(gPlayer1Controller->buttonPressed == START_BUTTON);
    assert(!(gPlayer1Controller->buttonDown & A_BUTTON));
    sample(FALSE); assert(!gPlayer1Controller->buttonPressed);
    gControllerPads[1].button = 0; sample(TRUE);
    assert(gPlayer1Controller->port == 1);
    gControllerPads[1].button = A_BUTTON; sample(TRUE);
    assert(gPlayer1Controller->buttonPressed == A_BUTTON);
    sample(FALSE); assert(gPlayer1Controller->buttonDown == 0);
    gControllerPads[0].errno = 0; gControllerPads[0].button = A_BUTTON;
    sample(FALSE); assert(gPlayer1Controller->port == 1 && !gPlayer1Controller->buttonDown);
    assert(game_options_get(GO_CONTROLLER) == 2); // Replugging the old pad does not steal control.
    gControllerPads[1].button = 0; sample(FALSE);
    gControllerPads[1].button = A_BUTTON; sample(FALSE);
    assert(gPlayer1Controller->buttonPressed == A_BUTTON);
    for (s32 port = 0; port < MAXCONTROLLERS; port++) { gControllerPads[port].errno = 1; }
    sample(FALSE);
    assert(game_options_get(GO_CONTROLLER) == 0 && gPlayer1Controller->port == -1);
    gControllerPads[0].errno = 0; sample(FALSE);
    assert(gPlayer1Controller->port == 0 && !(gPlayer1Controller->buttonDown & A_BUTTON));
    // Remapping never mutates shared physical samples, even when recovery also reads them.
    gControllerPads[0].button = 0; sample(FALSE);
    gControllerStatuses[0].type = CONT_CONSOLE_GCN;
    gControllerPads[0].button = Z_TRIG; gControllerPads[0].l_trig = 86;
    sample(FALSE); assert(gPlayer1Controller->buttonDown == (Z_TRIG | L_TRIG));
    assert(gControllerPads[0].button == Z_TRIG);
    sample(FALSE); assert(gPlayer1Controller->buttonDown == (Z_TRIG | L_TRIG));
}
static void filters(void) {
    reset();
    assert(game_options_get(GO_DEBOUNCE) == 0 && game_options_get(GO_MIN_HOLD) == 0);
    game_options_set(GO_DEBOUNCE, 3);
    sample(FALSE);
    gControllerPads[0].button = A_BUTTON; sample(FALSE);
    assert(gPlayer1Controller->buttonPressed == A_BUTTON);
    sample(FALSE); assert(!gPlayer1Controller->buttonPressed);
    gControllerPads[0].button = 0; sample(FALSE);
    assert(gPlayer1Controller->buttonReleased == A_BUTTON);
    gControllerPads[0].button = A_BUTTON; sample(FALSE);
    assert(!gPlayer1Controller->buttonDown);
    gControllerPads[0].button = 0;
    sample(FALSE); sample(FALSE); sample(FALSE);
    gControllerPads[0].button = A_BUTTON; sample(FALSE);
    assert(gPlayer1Controller->buttonPressed == A_BUTTON);
    reset();
    game_options_set(GO_MIN_HOLD, 3);
    sample(FALSE);
    gControllerPads[0].button = A_BUTTON;
    sample(FALSE); sample(FALSE); assert(!gPlayer1Controller->buttonDown);
    gControllerPads[0].button = 0; sample(FALSE); assert(!gPlayer1Controller->buttonReleased);
    gControllerPads[0].button = A_BUTTON;
    sample(FALSE); sample(FALSE); sample(FALSE);
    assert(gPlayer1Controller->buttonPressed == A_BUTTON);
    sample(FALSE); assert(!gPlayer1Controller->buttonPressed && gPlayer1Controller->buttonDown);
    gControllerPads[0].button = 0; sample(FALSE);
    assert(gPlayer1Controller->buttonReleased == A_BUTTON);
    gControllerPads[0].button = START_BUTTON; sample(FALSE);
    assert(gPlayer1Controller->buttonPressed == START_BUTTON);
    gControllerPads[0].button = A_BUTTON; sample(FALSE); // Pending hold is discarded on pause.
    sample(TRUE); assert(!gPlayer1Controller->buttonDown);
    gControllerPads[0].button = 0; sample(TRUE);
    gControllerPads[0].button = B_BUTTON; sample(TRUE);
    assert(gPlayer1Controller->buttonPressed == B_BUTTON);
    game_options_select_file(1); sample(TRUE);
    assert(!gPlayer1Controller->buttonDown);
    reset();
    game_options_set(GO_MIN_HOLD, 2); game_options_set(GO_DEBOUNCE, 2);
    sample(FALSE);
    gControllerPads[0].button = A_BUTTON | B_BUTTON;
    sample(FALSE); sample(FALSE); assert(gPlayer1Controller->buttonPressed == (A_BUTTON | B_BUTTON));
    gControllerPads[0].button = B_BUTTON; sample(FALSE);
    assert(gPlayer1Controller->buttonReleased == A_BUTTON);
    gControllerPads[0].button = A_BUTTON | B_BUTTON; sample(FALSE);
    assert(gPlayer1Controller->buttonDown == B_BUTTON);
    gControllerPads[0].button = B_BUTTON; sample(FALSE); sample(FALSE);
    gControllerPads[0].button = A_BUTTON | B_BUTTON; sample(FALSE); sample(FALSE);
    assert(gPlayer1Controller->buttonPressed == A_BUTTON && !gPlayer1Controller->buttonReleased);
    reset();
    gControllerPads[0].errno = 1;
    game_options_input_select(TRUE, TRUE);
    gControllerPads[0].button = A_BUTTON; // Virtual demo pad replaces disconnected input.
    game_options_input_read(TRUE, TRUE);
    assert(gPlayer1Controller->buttonDown == A_BUTTON && gPlayer1Controller->buttonPressed == A_BUTTON);
}
int main(void) { scalar_checks(); ports(); filters(); dialogue_hold(); puts("options controls checks passed"); }
'''


class OptionsControlsTests(unittest.TestCase):
    def test_controls_and_filters(self):
        with tempfile.TemporaryDirectory() as temporary:
            folder = Path(temporary)
            (folder / "ultra64.h").write_text("")
            (folder / "PR").mkdir()
            (folder / "PR/ultratypes.h").write_text(
                "#pragma once\n#include <stdint.h>\n"
                "typedef uint8_t u8; typedef int8_t s8; typedef uint16_t u16;\n"
                "typedef int16_t s16; typedef uint32_t u32; typedef int32_t s32; typedef float f32;\n")
            buttons = Source(ROOT / "include/n64/PR/os_cont.h").macros(
                "CONT_.*|A_BUTTON|B_BUTTON|START_BUTTON|L_TRIG|Z_TRIG|L_CBUTTONS|R_CBUTTONS")
            controller = Source(ROOT / "include/types.h").region(
                r"^typedef struct Controller", r"^typedef u8\s+Bool8")
            (folder / "game_init.h").write_text(
                "#include <PR/ultratypes.h>\n#define MAXCONTROLLERS 4\n#define MAX_NUM_PLAYERS 2\n"
                "typedef struct { u16 type; } OSContStatus;\n"
                "typedef struct { u16 button; s8 stick_x,stick_y; u8 errno,l_trig; } OSContPadEx;\n"
                + buttons + controller + "\n"
                "extern struct Controller gControllers[MAXCONTROLLERS];\n"
                "extern OSContStatus gControllerStatuses[MAXCONTROLLERS];\n"
                "extern OSContPadEx gControllerPads[MAXCONTROLLERS];\nextern u8 gControllerBits;\n"
                "void adjust_analog_stick(struct Controller *controller);\n")
            # Local source quoted includes resolve next to their production files.
            for name in ("game_options_input.c", "game_options_input.h"):
                (folder / name).write_text((ROOT / "src/game" / name).read_text())
            fixture = FIXTURE + Source(ROOT / "src/game/game_init.c").function("adjust_analog_stick")
            fixture += Source(ROOT / "src/game/camera.c").function("move_mario_head_c_up") + CHECKS
            (folder / "fixture.c").write_text(fixture)
            subprocess.run([
                "cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-fsanitize=address,undefined", "-fno-sanitize-recover=all",
                "-fno-omit-frame-pointer", "-I", str(folder), "-I", str(ROOT / "src/game"),
                str(folder / "fixture.c"), "-lm", "-o", str(folder / "test"),
            ], check=True, timeout=30)
            subprocess.run([str(folder / "test")], check=True,
                           env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"}, timeout=10)


if __name__ == "__main__":
    unittest.main()
