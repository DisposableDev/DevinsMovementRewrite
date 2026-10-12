#!/usr/bin/env python3
"""Extracted stick-direction accuracy and input preservation; no ROM or timing claims."""
from pathlib import Path
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]

FIXTURE = r'''
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
typedef float f32;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
enum { GO_DEADZONE };
static s32 game_options_get(s32 option) {
    if (option != GO_DEADZONE) abort();
    return 6;
}
s16 atan2s(f32 y, f32 x);
#define absf fabsf
#define ACT_DIVE_PICKING_UP 1
#define INPUT_NONZERO_ANALOG 1
struct Controller { s16 rawStickX, rawStickY; f32 stickX, stickY, stickMag; };
struct Camera { s16 yaw; };
struct Area { struct Camera *camera; };
struct MarioState {
    struct Controller *controller;
    struct Area *area;
    u32 action, input;
    s32 squishTimer;
    f32 intendedMag;
    s16 intendedYaw, faceAngle[3];
};
#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #value); exit(1); } } while (0)
'''

CHECKS = r'''
int main(void) {
    double worst = 0.0, oldWorst = 0.0;
    int changed = 0;
    struct Camera camera = { 0 };
    struct Area area = { &camera };
    for (int x = -128; x <= 127; x++) {
        for (int y = -128; y <= 127; y++) {
            struct Controller c = { .rawStickX = x, .rawStickY = y };
            adjust_analog_stick(&c);
            s16 angle = mario_stick_angle(-c.stickY, c.stickX);
            CHECK((u16) mario_stick_angle(-c.stickY, -c.stickX) == (u16) -angle);
            if (c.stickMag != 0.0f) {
                double reference = atan2((double) c.stickX, -(double) c.stickY)
                    * (32768.0 / 3.14159265358979323846);
                double error = fabs(remainder(angle - reference, 65536.0));
                s16 old = atan2s(-c.stickY, c.stickX);
                double oldError = fabs(remainder(old - reference, 65536.0));
                CHECK(error <= 0.53);
                if (error > worst) worst = error;
                if (oldError > oldWorst) oldWorst = oldError;
                changed += angle != old;
                if (c.stickX == 0.0f || c.stickY == 0.0f
                    || fabsf(c.stickX) == fabsf(c.stickY)) {
                    CHECK(angle == old);
                }
            } else {
                CHECK(angle == 0);
            }
            for (int context = 0; context < 4; context++) {
                struct MarioState m = { .controller = &c, .area = &area,
                    .squishTimer = context == 2,
                    .action = context == 3 ? ACT_DIVE_PICKING_UP : 0,
                    .faceAngle = { 0, 12345, 0 } };
                camera.yaw = context == 0 ? 0 : context == 1 ? 32767 : -32768;
                update_mario_joystick_inputs(&m);
                if (c.stickMag == 0.0f || context == 3) {
                    CHECK(m.intendedYaw == m.faceAngle[1]);
                    CHECK(m.intendedMag == 0.0f);
                    CHECK(m.input == 0);
                } else {
                    f32 strength = c.stickMag / 64.0f;
                    strength = strength * strength * 64.0f;
                    CHECK(m.intendedMag == strength / (context == 2 ? 8.0f : 2.0f));
                    CHECK((u16) m.intendedYaw == (u16) (angle + camera.yaw));
                    CHECK(m.input == INPUT_NONZERO_ANALOG);
                }
            }
        }
    }
    CHECK(worst < oldWorst / 10.0);
    printf("65536 raw pairs: max %.9f units (%.9f degrees), LUT %.9f units; "
           "%d directions changed; camera/neutral/strength preservation passed\n",
           worst, worst * 360.0 / 65536.0, oldWorst, changed);
    return 0;
}
'''


class MarioStickAngleTests(unittest.TestCase):
    def test_accuracy_and_input_preservation(self):
        mario = Source(ROOT / "src/game/mario.c")
        math = Source(ROOT / "src/engine/math_util.c")
        tables = Source(ROOT / "include/trig_tables.inc.c")
        start = tables.text.index("s16 gArctanTable")
        end = tables.text.index("};", start) + 2
        production = Source(ROOT / "src/game/game_init.c").function("adjust_analog_stick")
        production += mario.function("mario_stick_angle")
        production += mario.function("update_mario_joystick_inputs")
        production += tables.mapped(start, end)
        production += Source(ROOT / "src/engine/math_util.h").macros("atans")
        production += math.function("atan2_lookup") + math.function("atan2s")
        with tempfile.TemporaryDirectory(prefix="sm64-stick-angle-") as folder:
            source = Path(folder) / "test.c"
            binary = Path(folder) / "test"
            source.write_text(FIXTURE + production + CHECKS)
            result = subprocess.run(
                ["gcc", "-std=c11", "-Os", "-ffp-contract=off", str(source), "-lm", "-o", str(binary)],
                capture_output=True, text=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            result = subprocess.run([str(binary)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            print(result.stdout.strip())


if __name__ == "__main__":
    unittest.main()
