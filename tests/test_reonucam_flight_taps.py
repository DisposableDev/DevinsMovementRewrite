"""Flight camera R presses must not change the ground camera selection."""
from pathlib import Path
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class ReonucamFlightTapTests(unittest.TestCase):
    def test_flight_taps_carried_holds_and_ground_controls(self):
        camera = Source(ROOT / "src/game/camera.c")
        functions = camera.macros("REONUCAM_R_.*") + camera.function("reonucam_reset_r_tap")
        functions += camera.function("reonucam_handler") + camera.function("update_flying_camera_mode")
        fixture = r'''
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
typedef float f32;
typedef int32_t s32;
typedef int16_t s16;
#define REONUCAM
#define DEGREES(x) ((s16)((x) * 65536.0f / 360))
#define R_TRIG 1
#define L_CBUTTONS 2
#define R_CBUTTONS 4
#define CBUTTON_MASK 6
#define D_JPAD 8
#define ACT_FLAG_SWIMMING_OR_FLYING 0x100
#define ACT_FLYING (ACT_FLAG_SWIMMING_OR_FLYING | 1)
#define ACT_FLYING_TRIPLE_JUMP (ACT_FLAG_SWIMMING_OR_FLYING | 2)
#define ACT_SHOT_FROM_CANNON 3
#define CAM_ANGLE_MARIO 1
#define CAM_ANGLE_LAKITU 2
#define CAMERA_MODE_8_DIRECTIONS 8
#define FLYING_CAMERA_MODE 9
struct Controller {int buttonDown, buttonPressed;};
struct Camera {s16 mode, yaw;};
struct Area {struct Camera *camera;};
struct MarioState {int action; s16 faceAngle[3]; struct Area *area;};
static struct Controller controller, *gPlayer1Controller = &controller;
static struct Camera camera, *gCamera = &camera;
static struct Area area = {&camera};
static struct MarioState mario = {.area = &area}, *gMarioState = &mario;
static struct {int flyingCamOverride, rHoldFrames, rTapFrames;} gReonucamState;
static s16 s8DirModeBaseYaw, s8DirModeYawOffset;
static int selection = CAM_ANGLE_LAKITU, changes;
static f32 reonucam_rotation_speed(void) {return 4;}
static s16 snap_to_45_degrees(s16 yaw) {return yaw;}
static void play_sound_rbutton_changed(void) {}
static int set_cam_angle(int mode) {
    if (mode) {selection = mode; changes++;}
    return selection;
}
static void set_camera_mode(struct Camera *c, s16 mode, s16 frames) {
    (void)frames; c->mode = mode;
}
/* FUNCTIONS */
static void frame(int action, int down) {
    mario.action = action;
    controller.buttonPressed = down & ~controller.buttonDown;
    controller.buttonDown = down;
    if (action == ACT_FLYING) update_flying_camera_mode(&mario, 1);
    reonucam_handler(1);
}
int main(void) {
    const int flights[] = {ACT_FLYING, ACT_FLYING_TRIPLE_JUMP, ACT_SHOT_FROM_CANNON};
    for (int i = 0; i < 3; i++) {
        selection = CAM_ANGLE_LAKITU; changes = 0;
        controller.buttonDown = 0; reonucam_reset_r_tap();
        frame(flights[i], R_TRIG); frame(flights[i], 0);
        frame(flights[i], R_TRIG); frame(flights[i], 0);
        frame(0, 0);
        assert(selection == CAM_ANGLE_LAKITU && changes == 0);
        frame(flights[i], R_TRIG);
        frame(0, R_TRIG); frame(0, 0);
        assert(changes == 0);
        s16 yaw = s8DirModeBaseYaw;
        frame(0, L_CBUTTONS); assert(s8DirModeBaseYaw == (s16)(yaw - DEGREES(4)));
        frame(0, 0);
        frame(0, R_TRIG); frame(0, 0);
        frame(0, R_TRIG); frame(0, 0);
        assert(selection == CAM_ANGLE_MARIO && changes == 1);
    }
    assert(gReonucamState.flyingCamOverride == 1);
    return 0;
}
'''.replace("/* FUNCTIONS */", functions)
        with tempfile.TemporaryDirectory(prefix="reonucam-flight-") as name:
            directory = Path(name)
            source = directory / "test.c"
            source.write_text(fixture)
            result = subprocess.run(
                ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-fsanitize=undefined",
                 str(source), "-o", str(directory / "test")],
                capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stderr)
            result = subprocess.run([str(directory / "test")], capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
