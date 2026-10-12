"""Secret Slide awards require a current run, using the real timer and floor handlers."""
from pathlib import Path
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]

PREFIX = r'''#include <stdio.h>
#include <stdint.h>
typedef int32_t s32; typedef uint16_t u16; typedef uint8_t u8; typedef int8_t s8;
#define UNUSED __attribute__((unused))
#define FALSE 0
#define TRUE 1
struct Object { unsigned oBehParams; };
struct MarioState { struct Object *marioObj; };
struct { unsigned flags; u16 timer; } gHudDisplay;
static s8 sTimerRunning; static u8 sPssSlideStarted;
static int stars; static float starX,starY,starZ;
static void spawn_default_star(float x,float y,float z) { stars++; starX=x; starY=y; starZ=z; }
#line 15 "src/game/level_update.h"
'''
CASES = r'''#define CHECK(v) do { if (!(v)) { fprintf(stderr,"line %d: %s\n",__LINE__,#v); return 1; } } while(0)
int main(int argc,char **argv) {
 (void)argv; struct Object obj={0}; struct MarioState m={&obj};
 if(argc>1) {
  pss_begin_slide(&m); gHudDisplay.timer=400; level_control_timer(TIMER_CONTROL_HIDE);
  pss_end_slide(&m); CHECK(stars==0); CHECK(!sPssSlideStarted); return 0;
 }
 pss_end_slide(&m); CHECK(stars==0);
 pss_begin_slide(&m); CHECK(sTimerRunning); gHudDisplay.timer=629;
 pss_begin_slide(&m); CHECK(gHudDisplay.timer==629);
 pss_end_slide(&m); CHECK(stars==1); CHECK(!sTimerRunning); CHECK(gHudDisplay.timer==629);
 CHECK(gHudDisplay.flags & HUD_DISPLAY_FLAG_TIMER); CHECK(obj.oBehParams==(1u<<24));
 CHECK(starX==-6358 && starY==-4300 && starZ==4700);
 pss_end_slide(&m); CHECK(stars==1);
 for(int t=630;t<=631;t++) { level_control_timer(TIMER_CONTROL_HIDE); pss_begin_slide(&m);
  gHudDisplay.timer=t; pss_end_slide(&m); CHECK(stars==1); }
 level_control_timer(TIMER_CONTROL_HIDE); pss_begin_slide(&m); gHudDisplay.timer=400;
 level_control_timer(TIMER_CONTROL_HIDE); pss_end_slide(&m); CHECK(stars==1); CHECK(!sPssSlideStarted);
 pss_begin_slide(&m); gHudDisplay.timer=42; gHudDisplay.flags=0; sTimerRunning=0;
 pss_end_slide(&m); CHECK(stars==1); CHECK(!sPssSlideStarted);
 pss_begin_slide(&m); gHudDisplay.timer=400; level_control_timer(TIMER_CONTROL_HIDE);
 pss_begin_slide(&m); gHudDisplay.timer=400; pss_end_slide(&m); CHECK(stars==2);
 puts("PASS valid, slow, repeated, aborted and restarted slide finishes"); return 0;
}
'''


class SecretSlideTimerTests(unittest.TestCase):
    def test_cancelled_runs_and_valid_finishes(self):
        header = Source(ROOT / 'src/game/level_update.h')
        interaction = Source(ROOT / 'src/game/interaction.c')
        fixture = PREFIX
        fixture += header.declaration('enum', 'TimerControl') + '\n'
        fixture += header.declaration('enum', 'HUDDisplayFlag') + '\n'
        fixture += Source(ROOT / 'src/game/level_update.c').function('level_control_timer')
        fixture += interaction.function('pss_begin_slide')
        fixture += interaction.function('pss_end_slide')
        fixture += CASES
        with tempfile.TemporaryDirectory(prefix='secret-slide-timer-') as directory:
            source = Path(directory) / 'test.c'
            binary = Path(directory) / 'test'
            source.write_text(fixture)
            compiled = subprocess.run(
                ['cc', '-std=c99', '-Wall', '-Wextra', '-Werror', '-O2', str(source),
                 '-o', str(binary)], capture_output=True, text=True, timeout=30)
            self.assertEqual(compiled.returncode, 0, compiled.stdout + compiled.stderr)
            for arguments in (['aborted'], []):
                with self.subTest(arguments=arguments):
                    result = subprocess.run([str(binary), *arguments], capture_output=True,
                                            text=True, timeout=10)
                    self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


if __name__ == '__main__':
    unittest.main()
