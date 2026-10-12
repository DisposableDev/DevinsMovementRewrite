#!/usr/bin/env python3
"""Execute the native flag counter and glass gate with controlled gameplay state."""
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

class CastleFlagSecretTests(unittest.TestCase):
    def test_native_counter_and_window_gate(self):
        production = (ROOT / 'src/game/behaviors/castle_flag.inc.c').read_text()
        fixture = r'''
#include <assert.h>
#include <math.h>
typedef int s32; typedef unsigned int u32;
#define TRUE 1
#define FALSE 0
#define CASTLE_ROOF_COMPLETION_STARS 120
#define COURSE_MIN 1
#define COURSE_MAX 25
#define COURSE_NUM_TO_INDEX(x) ((x)-1)
#define LEVEL_CASTLE_GROUNDS 16
#define ACT_TOP_OF_POLE 1
#define ACT_TOP_OF_POLE_JUMP 2
#define ACT_SHOT_FROM_CANNON 3
#define ACT_FLYING 4
#define SOUND_MENU_COLLECT_SECRET 5
struct Object { struct { struct { struct { float animFrame; } animInfo; } gfx; } header;
 int oBehParams2ndByte; int hidden; int intangible;
 float oGraphYOffset; float hitboxHeight; } objects[5], *o;
struct MarioState { struct Object *usedObj; int action; float pos[3]; } mario, *gMarioState=&mario;
int gDebugLevelSelect, gCurrSaveFileNum=1, gCurrLevelNum=16, gCurrAreaIndex=1;
int stars, number, sounds, pushes, jingles, secretsBlocked;
static int game_options_secrets_blocked(void) { return gDebugLevelSelect || secretsBlocked; }
#define absf fabsf
static float random_float(void) { return 0.5f; }
static int save_file_get_total_star_count(int a,int b,int c) { return stars; }
static void cur_obj_hide(void) { o->hidden=1; }
static void cur_obj_unhide(void) { o->hidden=0; }
static void cur_obj_become_intangible(void) { o->intangible=1; }
static void cur_obj_become_tangible(void) { o->intangible=0; }
static void spawn_orange_number(int n,int x,int y,int z) { number=n; }
static void cur_obj_play_sound_2(int s) { sounds++; }
static void play_toads_jingle(void) { jingles++; }
static void bhv_pole_base_loop(void) { pushes++; }
'''
        checks = r'''
int main(void) {
 for(int i=0;i<5;i++) { o=&objects[i];o->oBehParams2ndByte=i+1;bhv_castle_flag_init(); }
 mario.pos[0]=0; mario.pos[1]=2500; mario.pos[2]=-1987;mario.action=ACT_FLYING;
 castle_secret_glass_check_air(&mario); assert(!castle_secret_glass_warp_ready());
 // Each ordinary flag increments once, including a handstand immediately jumped from.
 for(int i=0;i<4;i++) {
  o=&objects[i];mario.usedObj=o;mario.action=i==1?ACT_TOP_OF_POLE_JUMP:ACT_TOP_OF_POLE;
  bhv_castle_flag_loop();assert(number==i+1);bhv_castle_flag_loop();assert(sounds==i+1);
  assert(jingles==0);
 }
 o=&objects[4];mario.usedObj=o;mario.action=ACT_TOP_OF_POLE;
 bhv_castle_flag_loop();assert(o->hidden&&o->intangible&&number==4);
 stars=119;bhv_castle_flag_loop();assert(number==4);
 gDebugLevelSelect=1;bhv_castle_flag_loop();assert(o->hidden&&o->intangible&&number==4);
 gDebugLevelSelect=0;stars=120;bhv_castle_flag_loop();assert(!o->hidden&&sounds==5);
 assert(jingles==1);
 for(int action=0;action<=4;action++) {
  mario.action=action;castle_secret_glass_check_air(&mario);
  assert(castle_secret_glass_warp_ready()==(action==ACT_FLYING||action==ACT_SHOT_FROM_CANNON));
 }
 mario.action=ACT_FLYING;
 float invalid[][3]={{403,2500,-2037},{0,2200,-2037},{0,3007,-2037},{0,2500,-1936}};
 for(int i=0;i<4;i++) {
  for(int j=0;j<3;j++)mario.pos[j]=invalid[i][j];
  castle_secret_glass_check_air(&mario);assert(!castle_secret_glass_warp_ready());
 }
 mario.pos[0]=402;mario.pos[1]=3006;mario.pos[2]=-2137;
 castle_secret_glass_check_air(&mario);assert(castle_secret_glass_warp_ready());
 secretsBlocked=1;assert(!castle_secret_glass_warp_ready());
 castle_secret_glass_check_air(&mario);assert(!castle_secret_glass_warp_ready());
 secretsBlocked=0;castle_secret_glass_check_air(&mario);assert(castle_secret_glass_warp_ready());
 gCurrAreaIndex=2;assert(!castle_secret_glass_warp_ready());gCurrAreaIndex=1;
 o=&objects[0];bhv_castle_flag_init();castle_secret_glass_check_air(&mario);
 assert(!castle_secret_glass_warp_ready());
 // A fresh visit can complete with an ordinary flag last and plays once again.
 int order[5]={4,0,1,2,3};
 mario.action=ACT_TOP_OF_POLE;
 for(int i=0;i<5;i++) {
  o=&objects[order[i]];mario.usedObj=o;
  bhv_castle_flag_loop();bhv_castle_flag_loop();
  assert(number==i+1);assert(jingles==(i==4?2:1));
 }
 return 0;
}
'''
        with tempfile.TemporaryDirectory() as d:
            src = Path(d) / 'test.c'
            exe = Path(d) / 'test'
            src.write_text(fixture+production+checks)
            subprocess.run(['cc','-std=c99',str(src),'-lm','-o',str(exe)],check=True)
            subprocess.run([str(exe)],check=True)

if __name__ == '__main__':
    unittest.main()
