"""Host checks for the flight pitch curve and its movement integration."""
from pathlib import Path
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class FlyingPitchCurveTests(unittest.TestCase):
    def test_smooth_curve_and_flight_update(self):
        source = Source(ROOT / "src/game/mario_actions_airborne.c")
        functions = (Source(ROOT / "include/config/config_movement.h").macros("WING_FLIGHT_.*")
                     + Source(ROOT / "src/engine/math_util.c").function("approach_f32")
                     + source.function("flying_speed_pitch_change") + source.function("update_flying"))
        fixture = r'''
#include <assert.h>
#include <math.h>
#include <stdint.h>
typedef float f32;
typedef int16_t s16;
#define CLAMP(x,lo,hi) fminf(fmaxf((x),(lo)),(hi))
#define DEGREES(x) ((s16)((x) * 65536.0f / 360.0f))
struct MarioState {
    f32 forwardVel, vel[3], slideVelX, slideVelZ;
    s16 faceAngle[3], angleVel[3];
};
static int steering;
static void update_flying_pitch(struct MarioState *m) { (void)m; steering++; }
static void update_flying_yaw(struct MarioState *m) { (void)m; steering++; }
static f32 sins(s16 angle) { return sinf(angle * (6.28318530718f / 65536)); }
static f32 coss(s16 angle) { return cosf(angle * (6.28318530718f / 65536)); }
/* FUNCTIONS */
int main(void) {
    assert(flying_speed_pitch_change(0) == -1024);
    assert(flying_speed_pitch_change(4) == -1024);
    assert(flying_speed_pitch_change(10) == -512);
    for (int i = 160; i <= 640; i++) {
        assert(flying_speed_pitch_change(i / 10.0f) == 0);
    }
    f32 previous = -1024;
    for (int i = 0; i <= 12800; i++) {
        f32 value = flying_speed_pitch_change(i / 100.0f);
        assert(isfinite(value) && value >= previous - 0.001f);
        assert(value - previous < 1.3f); // No discontinuities at either old threshold.
        previous = value;
    }
    const f32 joins[] = {4, 16, 64, 80};
    for (int i = 0; i < 4; i++) {
        f32 speed = joins[i], h = 0.02f;
        f32 center = flying_speed_pitch_change(speed);
        f32 left = (center - flying_speed_pitch_change(speed - h)) / h;
        f32 right = (flying_speed_pitch_change(speed + h) - center) / h;
        assert(fabsf(left - right) < 0.5f); // Slopes join as well as values.
    }
    for (int speed = 80; speed <= 128; speed++) {
        assert(flying_speed_pitch_change(speed) == (speed - 64) * 6);
    }
    // Run the real flight update: speed targeting precedes the curve, steering follows it,
    // pitch stays bounded and published velocity follows the resulting pose.
    for (int speed = 0; speed <= 128; speed++) {
        struct MarioState m = {.forwardVel = speed, .angleVel = {37, 0, 0}};
        steering = 0;
        update_flying(&m);
        f32 expectedSpeed = fminf(speed < 64 ? speed + 1 : speed > 64 ? speed - 1 : 64, 100);
        assert(steering == 2 && m.forwardVel == expectedSpeed);
        assert(m.faceAngle[0] == (s16)flying_speed_pitch_change(expectedSpeed) + 37);
        assert(fabsf(m.vel[1] - expectedSpeed * sins(m.faceAngle[0])) < 0.0001f);
        assert(m.slideVelX == m.vel[0] && m.slideVelZ == m.vel[2]);
    }
    // Fixed climb/level/dive attitudes converge to their requested targets,
    // including slow launches and initially excessive cannon speed.
    const s16 pitches[] = {DEGREES(60), 0, -DEGREES(60)};
    const f32 targets[] = {32, 64, 100};
    for (int p = 0; p < 3; p++) {
        for (int start = 0; start <= 150; start += 30) {
            struct MarioState m = {.forwardVel = start};
            for (int frame = 0; frame < 160; frame++) {
                m.faceAngle[0] = pitches[p];
                update_flying(&m);
                assert(m.forwardVel >= 0 && m.forwardVel <= 100);
                assert(m.faceAngle[0] >= -DEGREES(60) && m.faceAngle[0] <= DEGREES(60));
            }
            assert(m.forwardVel == targets[p]);
        }
    }
    struct MarioState level = {.forwardVel = 64};
    for (int frame = 0; frame < 120; frame++) update_flying(&level);
    assert(level.forwardVel == 64 && level.faceAngle[0] == 0);
    return 0;
}
'''.replace("/* FUNCTIONS */", functions)
        with tempfile.TemporaryDirectory(prefix="flying-pitch-") as directory:
            path = Path(directory)
            (path / "test.c").write_text(fixture)
            result = subprocess.run(
                ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror", "-fsanitize=undefined",
                 str(path / "test.c"), "-lm", "-o", str(path / "test")],
                capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stderr)
            result = subprocess.run([str(path / "test")], capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
