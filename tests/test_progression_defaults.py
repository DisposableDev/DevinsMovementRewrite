"""Fresh-file progression under the real release and diagnostic configurations."""
from pathlib import Path
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class ProgressionDefaultsTests(unittest.TestCase):
    def test_earned_unlocks_in_release_and_diagnostics(self):
        production = (ROOT / 'src/game/behaviors/moat_drainer.inc.c').read_text()
        production += Source(ROOT / 'src/game/behaviors/water_pillar.inc.c').function(
            'bhv_water_level_pillar_init')
        production += Source(ROOT / 'src/game/behaviors/castle_cannon_grate.inc.c').function(
            'bhv_castle_cannon_grate_init')
        production += Source(ROOT / 'src/game/behaviors/exclamation_box.inc.c').function(
            'exclamation_box_cap_is_unlocked')
        production += Source(ROOT / 'src/game/save_file.c').function('save_file_is_cannon_unlocked')
        fixture = r'''
#include <assert.h>
#include "config.h"
#if defined(UNLOCK_ALL) || defined(COMPLETE_SAVE_FILE)
#error Ordinary builds must not force earned progression
#endif
#ifndef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
#error Vanilla course progression must remain active
#endif
typedef int s32;
#define TRUE 1
#define COURSE_MIN 1
#define COURSE_MAX 25
#define LEVEL_BOB 9
#define LEVEL_CASTLE_GROUNDS 16
#define SAVE_FLAG_MOAT_DRAINED 8
#define COURSE_FLAG_CANNON_UNLOCKED 128
#define ACTIVE_FLAG_DEACTIVATED 0
#define EXCLAMATION_BOX_BP1_NEEDS_SWITCH 0
#define EXCLAMATION_BOX_BP_WING_CAP 0
#define GET_BPARAM1(x) ((x) >> 24)
struct Object { int activeFlags, oWaterLevelPillarDrained, oBehParams2ndByte; unsigned oBehParams; } object, *o=&object;
struct { struct { unsigned char courseStars[26]; } files[1][1]; } gSaveBuffer;
int gCurrSaveFileNum=1, gCurrCourseNum=1, gCurrLevelNum=LEVEL_BOB, gCurrAreaIndex=1;
int flags, stars, gEnvironmentRegions[13];
const int sCapSaveFlags[3]={1,2,4};
static int save_file_get_flags(void) { return flags; }
static int save_file_get_total_star_count(int file,int min,int max) { (void)file;(void)min;(void)max;return stars; }
static int save_file_has_castle_wing_cap(int file) { (void)file;return 0; }
'''
        fixture += production
        fixture += r'''
int main(void) {
 gEnvironmentRegions[6]=gEnvironmentRegions[12]=-81;
 bhv_invisible_objects_under_bridge_init();
 assert(gEnvironmentRegions[6]==-81 && gEnvironmentRegions[12]==-81);
 bhv_water_level_pillar_init(); assert(!object.oWaterLevelPillarDrained);
 object.activeFlags=1;bhv_castle_cannon_grate_init();assert(object.activeFlags==1);
 stars=119;bhv_castle_cannon_grate_init();assert(object.activeFlags==1);
 stars=120;bhv_castle_cannon_grate_init();assert(object.activeFlags==ACTIVE_FLAG_DEACTIVATED);
 for(int cap=0;cap<3;cap++) {
  object.oBehParams2ndByte=cap;flags=0;assert(!exclamation_box_cap_is_unlocked());
  flags=sCapSaveFlags[cap];assert(exclamation_box_cap_is_unlocked());
 }
 assert(!save_file_is_cannon_unlocked());
 gSaveBuffer.files[0][0].courseStars[1]=COURSE_FLAG_CANNON_UNLOCKED;
 assert(save_file_is_cannon_unlocked());
 flags=SAVE_FLAG_MOAT_DRAINED;bhv_invisible_objects_under_bridge_init();
 assert(gEnvironmentRegions[6]==-800 && gEnvironmentRegions[12]==-800);
 bhv_water_level_pillar_init();assert(object.oWaterLevelPillarDrained);
 return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix='progression-defaults-') as directory:
            folder = Path(directory)
            source = folder / 'test.c'
            source.write_text(fixture)
            for region in ('VERSION_US', 'VERSION_EU'):
                for diagnostic in (False, True):
                    command = ['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                               '-fsanitize=undefined', '-fno-sanitize-recover=all', '-I', str(ROOT / 'include'),
                               f'-D{region}=1', str(source), '-o', str(folder / 'test')]
                    if diagnostic:
                        command.append('-DDEBUG=1')
                    subprocess.run(command, check=True, timeout=30)
                    subprocess.run([str(folder / 'test')], check=True, timeout=10)


if __name__ == '__main__':
    unittest.main()
