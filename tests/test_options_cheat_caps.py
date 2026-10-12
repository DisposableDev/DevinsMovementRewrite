"""Actual permissions, forced cap ownership/timers and permanent wet status."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]

PRELUDE = r'''
#include <assert.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#include "game_options.c"
s16 gCurrSaveFileNum=1;
s8 gDebugLevelSelect;
void *gCurrDemoInput, *gCurrCreditsEntry;
struct MarioState {
    u32 flags,forcedCapFlags,naturalCapFlags,action;
    u16 capTimer;
    u8 soakedThisUpdate,soakedDripFrames;
    u16 soakedTimer;
};
static struct MarioState *gMarioState;
#define ACT_DEBUG_FREE_MOVE 0x0000130F
#define ACT_UNINITIALIZED 0x00000000
#include "game_options_runtime.c"
static s32 stoppedMusic, fadedMusic;
static void stop_cap_music(void) { stoppedMusic++; }
static void fadeout_cap_music(void) { fadedMusic++; }
'''

CHECKS = r'''
static u32 cap_flags(s32 bits) {
    return (bits&1 ? MARIO_WING_CAP : 0) | (bits&2 ? MARIO_METAL_CAP : 0)
           | (bits&4 ? MARIO_VANISH_CAP : 0);
}
static void force(s32 bits) {
    sValues[0][GO_FORCE_WING_CAP]=(bits&1)!=0;
    sValues[0][GO_FORCE_METAL_CAP]=(bits&2)!=0;
    sValues[0][GO_FORCE_VANISH_CAP]=(bits&4)!=0;
}
int main(void) {
    game_options_init(NULL,NULL); game_options_select_file(0);
    for (s32 forced=0;forced<8;forced++) {
        for (s32 natural=0;natural<8;natural++) {
            struct MarioState m={.flags=MARIO_NORMAL_CAP|MARIO_CAP_ON_HEAD|cap_flags(natural),
                                 .action=ACT_IDLE,.capTimer=61};
            force(forced); sValues[0][GO_CHEATS_ENABLED]=1;
            mario_update_forced_caps(&m);
            assert((m.flags&MARIO_SPECIAL_CAPS)==cap_flags(forced|natural));
            assert(m.naturalCapFlags==cap_flags(natural) && m.capTimer==61);
            assert(mario_natural_cap_powers(&m)==cap_flags(natural));
            assert((update_and_return_cap_flags(&m)&cap_flags(forced))==cap_flags(forced));
            sValues[0][GO_CHEATS_ENABLED]=0; mario_update_forced_caps(&m);
            assert((m.flags&MARIO_SPECIAL_CAPS)==cap_flags(natural) && m.capTimer==60);
            sValues[0][GO_CHEATS_ENABLED]=1; mario_update_forced_caps(&m);
            for (s32 frame=0;frame<60;frame++) {
                u32 rendered=update_and_return_cap_flags(&m);
                assert((rendered&cap_flags(forced))==cap_flags(forced));
            }
            assert(m.capTimer==0 && m.naturalCapFlags==0);
            assert((m.flags&MARIO_SPECIAL_CAPS)==cap_flags(forced));
            for (s32 frame=0;frame<2000;frame++) {
                mario_update_forced_caps(&m); update_and_return_cap_flags(&m);
            }
            assert(m.capTimer==0 && (m.flags&MARIO_SPECIAL_CAPS)==cap_flags(forced));
            // Pick up every natural combination while the same forced powers exist.
            mario_add_cap_power(&m,cap_flags(natural)); m.capTimer=123;
            mario_update_forced_caps(&m);
            assert(m.naturalCapFlags==cap_flags(natural));
            sValues[0][GO_CHEATS_ENABLED]=0; mario_update_forced_caps(&m);
            assert((m.flags&MARIO_SPECIAL_CAPS)==cap_flags(natural) && m.capTimer==123);
            // Shock/cap removal strips naturally owned powers, retaining the overlay.
            sValues[0][GO_CHEATS_ENABLED]=1; mario_update_forced_caps(&m);
            mario_clear_cap_flags(&m); m.capTimer=0;
            assert((m.flags&MARIO_SPECIAL_CAPS)==cap_flags(forced) && !m.naturalCapFlags);
            force(0); mario_update_forced_caps(&m);
            assert(!(m.flags&MARIO_SPECIAL_CAPS));
        }
    }
    struct MarioState m={0}; force(7); sValues[0][GO_CHEATS_ENABLED]=1;
    sValues[0][GO_PERMANENT_SOAKED]=1;
    mario_update_forced_caps(&m);
    assert(!(m.flags&MARIO_NORMAL_CAP) && (m.flags&MARIO_CAP_ON_HEAD));
    s32 before=stoppedMusic;
    mario_remove_cap_powers(&m);
    assert(stoppedMusic==before && mario_natural_cap_powers(&m)==0 && m.forcedCapFlags==MARIO_SPECIAL_CAPS);
    for (s32 denied=0;denied<3;denied++) {
        gCurrDemoInput=denied==0 ? &m : NULL;
        gCurrCreditsEntry=denied==1 ? &m : NULL;
        gCurrSaveFileNum=denied==2 ? 2 : 1;
        mario_update_forced_caps(&m);
        assert(!m.forcedCapFlags && !(m.flags&MARIO_CAPS) && !(m.flags&MARIO_CAP_ON_HEAD));
        assert(!mario_is_soaked(&m));
    }
    gCurrDemoInput=gCurrCreditsEntry=NULL;gCurrSaveFileNum=1;
    sValues[0][GO_PERMANENT_SOAKED]=1;
    for (s32 enabled=0;enabled<2;enabled++) {
        sValues[0][GO_SOAKED]=enabled;
        for (s32 vanish=0;vanish<2;vanish++) {
            m.flags=vanish ? MARIO_VANISH_CAP : 0;
            m.soakedThisUpdate=FALSE;m.soakedTimer=0;m.soakedDripFrames=4;
            assert(mario_is_soaked(&m));
            mario_consume_soaked_status(&m);
            assert(mario_is_soaked(&m) && m.soakedDripFrames==4);
            sValues[0][GO_CHEATS_ENABLED]=0;
            assert(!mario_is_soaked(&m));
            mario_consume_soaked_status(&m); assert(!m.soakedDripFrames);
            sValues[0][GO_CHEATS_ENABLED]=1;
        }
    }
    assert(stoppedMusic>0 && fadedMusic>0);
    return 0;
}
'''


class CheatCaps(unittest.TestCase):
    def test_cap_ownership_and_wet_override(self):
        mario = Source(ROOT / "src/game/mario.c")
        constants = Source(ROOT / "include/sm64.h")
        actual = constants.declaration("enum", "MarioFlags")
        actual += constants.declaration("enum", "MarioActionGroups")
        actual += constants.declaration("enum", "MarioActionFlags")
        actual += constants.macros("ACT_.*")
        actual += mario.macros("CAP_FLICKER_.*")
        actual += mario.region(r"^u64 sCapFlickerFrames", r"^static s32 mario_cap_flicker_is_dim")
        for name in ("mario_has_current_vanish_cap", "mario_update_forced_caps", "mario_add_cap_power",
                     "mario_clear_cap_flags", "mario_natural_cap_powers", "mario_is_soaked", "mario_consume_soaked_status",
                     "update_and_return_cap_flags"):
            actual += mario.function(name)
        actual += Source(ROOT / "src/game/interaction.c").function("mario_remove_cap_powers")
        with tempfile.TemporaryDirectory(prefix="options-cheat-caps-") as temporary:
            folder=Path(temporary);(folder/"PR").mkdir()
            (folder/"PR/ultratypes.h").write_text(
                "#include <stdint.h>\ntypedef uint8_t u8;typedef int8_t s8;typedef uint16_t u16;"
                "typedef int16_t s16;typedef uint32_t u32;typedef int32_t s32;typedef uint64_t u64;"
                "typedef float f32;\n")
            for name in ("game_options.c", "game_options.h", "game_options_runtime.c", "game_options_runtime.h"):
                (folder/name).write_text((ROOT/"src/game"/name).read_text())
            for name,text in {"area.h":"extern s16 gCurrSaveFileNum;\n",
                              "game_init.h":"extern void *gCurrDemoInput;\n",
                              "level_update.h":"extern void *gCurrCreditsEntry;\n",
                              "main.h":"extern s8 gDebugLevelSelect;\n" , "mario.h": ""}.items():
                (folder/name).write_text(text)
            path=folder/"fixture.c";path.write_text(PRELUDE+actual+CHECKS)
            for defines in ([],["-DVERSION_EU"]):
                exe=folder/"test"
                subprocess.run(["cc","-std=c11","-Wall","-Wextra","-Werror",
                                "-fsanitize=address,undefined", "-fno-sanitize-recover=all","-fno-omit-frame-pointer",*defines,
                                "-I",str(folder),str(path),"-o",str(exe)],check=True, timeout=30)
                subprocess.run([str(exe)],check=True,env={**os.environ,"ASAN_OPTIONS":"detect_leaks=0"}, timeout=10)


if __name__ == "__main__":
    unittest.main()
