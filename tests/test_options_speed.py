#!/usr/bin/env python3
"""Shared speed limits execute production functions; no ROM or collision doubles."""
from pathlib import Path
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class SpeedOptionsTests(unittest.TestCase):
    def test_master_gate_momentum_and_numeric_boundary(self):
        movement = Source(ROOT / "include/config/config_movement.h")
        options = Source(ROOT / "src/game/game_options.h")
        mario = Source(ROOT / "src/game/mario.c")
        fixture = r'''
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>
typedef float f32;
typedef float Vec3f[3];
typedef int16_t s16;
typedef int32_t s32;
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define CLAMP(x,lo,hi) MIN(MAX(x,lo),hi)
struct MarioState { Vec3f vel; f32 forwardVel,slideVelX,slideVelZ; s16 faceAngle[3]; };
static f32 sins(s16 angle) {return sinf(angle * (6.28318530718f / 65536));}
static f32 coss(s16 angle) {return cosf(angle * (6.28318530718f / 65536));}
'''
        fixture += options.macros("GAME_OPTION_CATALOG")
        fixture += options.declaration("enum", "GameOptionId")
        fixture += r'''
static int master,enabled;
static int game_options_cheat_active(enum GameOptionId id) {
    assert(id==GO_NO_SPEED_LIMIT);return master && enabled;
}
'''
        fixture += movement.macros("MARIO_GLOBAL_SPEED_CAP|MARIO_SPEED_SAFETY_CAP|MARIO_ACTIVE_SPEED_CAP|GROUND_DIVE_HEIGHT_RETENTION")
        fixture += "\n".join(mario.function(name) for name in (
            "mario_limit_velocity_vector", "mario_limit_velocity", "mario_set_forward_vel",
            "ground_dive_launch_velocity"))
        fixture += r'''
int main(void) {
    const f32 speeds[]={0,127,128,129,300,-129,-300,16384,-16384,20000,-20000,INFINITY,-INFINITY};
    for(master=0;master<2;master++) for(enabled=0;enabled<2;enabled++) {
        f32 limit=master&&enabled?16384:128;
        for(unsigned i=0;i<sizeof(speeds)/sizeof(*speeds);i++) {
            f32 expected=CLAMP(speeds[i],-limit,limit);
            struct MarioState m={.vel={speeds[i],-speeds[i],speeds[i]},
                                    .forwardVel=speeds[i],.slideVelX=speeds[i],.slideVelZ=-speeds[i]};
            mario_limit_velocity(&m);
            assert(m.forwardVel==expected && m.vel[0]==expected && m.vel[1]==-expected);
            assert(m.slideVelX==expected && m.slideVelZ==-expected);
            assert(isfinite(m.forwardVel) && isfinite(m.vel[1]));
            mario_set_forward_vel(&m,speeds[i]);
            assert(m.forwardVel==expected && m.vel[2]==expected && m.slideVelZ==expected);
            assert(m.vel[0]==0 && m.slideVelX==0);
            f32 launch=ground_dive_launch_velocity(speeds[i]);
            assert(isfinite(launch) && launch>=0 && launch<=limit);
            if(speeds[i]>128 && master&&enabled) assert(launch>128 || speeds[i]==129);
        }
    }
    master=enabled=1;
    struct MarioState m={.forwardVel=300,.slideVelX=-240,.slideVelZ=180,.vel={-240,-200,180}};
    mario_limit_velocity(&m);
    assert(m.forwardVel==300 && m.vel[0]==-240 && m.vel[1]==-200 && m.slideVelZ==180);
    master=0;
    mario_limit_velocity(&m);
    assert(m.forwardVel==128 && m.vel[0]==-128 && m.vel[1]==-128 && m.slideVelZ==128);
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix="sm64-options-speed-") as directory:
            path = Path(directory)
            (path / "fixture.c").write_text(fixture)
            subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=undefined,float-cast-overflow", "-fno-sanitize-recover=all", str(path / "fixture.c"),
                            "-lm", "-o", str(path / "fixture")], check=True, timeout=30)
            subprocess.run([str(path / "fixture")], check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
