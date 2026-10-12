"""Actual current forces and runtime permission boundaries, NTSC and PAL."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]

PRELUDE = r'''
#include <assert.h>
#include <math.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#define ARRAY_COUNT(a) ((s32)(sizeof(a)/sizeof(*(a))))
#define ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
#define LEVEL_DDD 23
#include "game_options.c"
s16 gCurrSaveFileNum=1;
s8 gDebugLevelSelect;
void *gCurrDemoInput, *gCurrCreditsEntry;
s32 gCurrLevelNum, gCurrAreaIndex;
typedef f32 Vec3f[3];
struct Surface { s16 type; u16 force; };
struct MarioState { Vec3f pos; struct Surface *floor; u32 action; };
static struct MarioState *gMarioState;
#define ACT_DEBUG_FREE_MOVE 0x0000130F
#define ACT_UNINITIALIZED 0x00000000
#include "game_options_runtime.c"
struct Whirlpool { Vec3f pos; s16 strength; };
static struct { struct Whirlpool *whirlpools[2]; } area, *gCurrentArea=&area;
static f32 sins(s16 angle) { return sinf(angle*(6.283185307179586f/65536.0f)); }
static f32 coss(s16 angle) { return cosf(angle*(6.283185307179586f/65536.0f)); }
static void vec3_get_dist_and_angle(Vec3f a,Vec3f b,f32 *dist,s16 *pitch,s16 *yaw) {
    f32 x=b[0]-a[0],y=b[1]-a[1],z=b[2]-a[2];
    *dist=sqrtf(x*x+y*y+z*z);
    *pitch=(s16)(atan2f(y,sqrtf(x*x+z*z))*65536.0f/6.283185307179586f);
    *yaw=(s16)(atan2f(x,z)*65536.0f/6.283185307179586f);
}
'''

CHECKS = r'''
static void check_current(struct MarioState *m) {
    const Vec3f incoming={11,-7,19};
    Vec3f normal,ignored,restored;
    sValues[0][GO_CHEATS_ENABLED]=1;
    sValues[0][GO_IGNORE_CURRENTS]=0;
    memcpy(normal,incoming,sizeof(normal)); apply_water_current(m,normal);
    assert(memcmp(normal,incoming,sizeof(normal))!=0);
    sValues[0][GO_IGNORE_CURRENTS]=1;
    memcpy(ignored,incoming,sizeof(ignored)); apply_water_current(m,ignored);
    assert(memcmp(ignored,incoming,sizeof(ignored))==0);
    // Master Off restores identical native force evaluation, preserving momentum.
    sValues[0][GO_CHEATS_ENABLED]=0;
    memcpy(restored,incoming,sizeof(restored)); apply_water_current(m,restored);
    assert(memcmp(restored,normal,sizeof(normal))==0);
    sValues[0][GO_CHEATS_ENABLED]=1;
    for (s32 denied=0;denied<3;denied++) {
        gCurrDemoInput=denied==0 ? m : NULL;
        gCurrCreditsEntry=denied==1 ? m : NULL;
        gCurrSaveFileNum=denied==2 ? 2 : 1;
        memcpy(restored,incoming,sizeof(restored)); apply_water_current(m,restored);
        assert(memcmp(restored,normal,sizeof(normal))==0);
    }
    gCurrDemoInput=gCurrCreditsEntry=NULL; gCurrSaveFileNum=1;
}
int main(void) {
    game_options_init(NULL,NULL); game_options_select_file(0);
    for (s32 combination=0;combination<8;combination++) {
        sValues[0][GO_CHEATS_ENABLED]=(combination&1)!=0;
        sValues[0][GO_INFINITE_HEALTH]=(combination&2)!=0;
        sValues[0][GO_KNOCKBACK]=(combination&4)!=0;
        assert(game_options_damage_voice_muted()==(combination==7));
    }
    gCurrDemoInput=&area;
    assert(!game_options_damage_voice_muted());
    gCurrDemoInput=NULL; gCurrCreditsEntry=&area;
    assert(!game_options_damage_voice_muted());
    gCurrCreditsEntry=NULL; gCurrSaveFileNum=2;
    assert(!game_options_damage_voice_muted());
    gCurrSaveFileNum=1;
    game_options_init(NULL,NULL); game_options_select_file(0);
    struct Surface floor={.type=SURFACE_FLOWING_WATER};
    struct MarioState m={.floor=&floor};
    for (s32 speed=0;speed<4;speed++) {
        for (s32 angle=0;angle<256;angle+=32) {
            floor.force=(speed<<8)|angle;
            check_current(&m);
        }
    }
    floor.type=SURFACE_DEFAULT;
    struct Whirlpool whirlpool={.pos={400,200,600}};
    area.whirlpools[0]=&whirlpool;
    for (s32 sign=-1;sign<=1;sign+=2) {
        whirlpool.strength=sign*100;
        check_current(&m);
    }
    whirlpool.pos[0]=3000;whirlpool.pos[1]=whirlpool.pos[2]=0;
    whirlpool.strength=100;gCurrLevelNum=LEVEL_DDD;gCurrAreaIndex=2;
    check_current(&m); // Extended DDD radius uses the same bypass.
    // Outside all influences, enabling/disabling changes neither force nor state.
    whirlpool.pos[0]=5000;
    for (s32 enabled=0;enabled<2;enabled++) {
        sValues[0][GO_IGNORE_CURRENTS]=enabled;
        Vec3f step={11,-7,19};apply_water_current(&m,step);
        assert(step[0]==11 && step[1]==-7 && step[2]==19);
        assert(m.pos[0]==0 && m.pos[1]==0 && m.pos[2]==0 && m.floor==&floor);
    }
    return 0;
}
'''


class EnvironmentOptions(unittest.TestCase):
    def test_current_forces_and_permissions(self):
        submerged = Source(ROOT / "src/game/mario_actions_submerged.c")
        actual = Source(ROOT / "include/surface_terrains.h").declaration("enum", "SurfaceTypes")
        # Read the production table as well as the force function.
        table = next(line for line in submerged.text.splitlines()
                     if line.startswith("static s16 sWaterCurrentSpeeds"))
        actual += table + "\n" + submerged.function("apply_water_current")
        with tempfile.TemporaryDirectory(prefix="options-environment-") as temporary:
            folder = Path(temporary)
            (folder / "PR").mkdir()
            (folder / "PR/ultratypes.h").write_text(
                "#include <stdint.h>\ntypedef uint8_t u8;typedef int8_t s8;typedef uint16_t u16;"
                "typedef int16_t s16;typedef uint32_t u32;typedef int32_t s32;typedef uint64_t u64;"
                "typedef float f32;\n")
            for name in ("game_options.c", "game_options.h", "game_options_runtime.c", "game_options_runtime.h"):
                (folder / name).write_text((ROOT / "src/game" / name).read_text())
            for name, text in {"area.h": "extern s16 gCurrSaveFileNum;\n",
                               "game_init.h": "extern void *gCurrDemoInput;\n",
                               "level_update.h": "extern void *gCurrCreditsEntry;\n",
                               "main.h": "extern s8 gDebugLevelSelect;\n" , "mario.h": ""}.items():
                (folder / name).write_text(text)
            path = folder / "fixture.c"
            path.write_text(PRELUDE + actual + CHECKS)
            for defines in ([], ["-DVERSION_EU"]):
                executable = folder / "test"
                subprocess.run(["cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                                "-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-fno-omit-frame-pointer", *defines,
                                "-I", str(folder), str(path), "-lm", "-o", str(executable)], check=True, timeout=30)
                subprocess.run([str(executable)], check=True,
                               env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"}, timeout=10)


if __name__ == "__main__":
    unittest.main()
