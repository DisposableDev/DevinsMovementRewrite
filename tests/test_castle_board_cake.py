"""Native cake routing checks; screen rendering and audio are controlled boundaries."""
from pathlib import Path
import subprocess
import tempfile
import unittest
from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class CastleBoardCakeTests(unittest.TestCase):
    def test_crush_uses_normal_cake_then_game_over(self):
        source = Source(ROOT/'src/game/level_update.c')
        code = r'''
#include <assert.h>
#include <stddef.h>
typedef int s32;
typedef short s16;
#define UNUSED __attribute__((unused))
#define TRUE 1
#define FALSE 0
#define WARP_FLAGS_NONE 0
#define WARP_FLAG_YOSHI_COIN_CAKE 16
#define WARP_FLAG_CASTLE_BOARD_CAKE 32
#define WARP_FLAG_SECRET_LOBBY_CAKE 64
#define LEVEL_CASTLE_GROUNDS 16
#define LEVEL_ENDING 99
#define WARP_NODE_MAIN_ENTRY 10
#define WARP_TRANSITION_FADE_INTO_COLOR 1
#define WARP_TYPE_NOT_WARPING 0
#define WARP_TYPE_CHANGE_LEVEL 1
#define WARP_OP_NONE 0
#define HUD_DISPLAY_NONE 0
#define MENU_MODE_NONE -1
#define MENU_OPT_NONE 0
#define ACT_IDLE 0
#define PLAY_MODE_NORMAL 0
#define PLAY_MODE_PAUSED 1
#define PLAY_MODE_CHANGE_LEVEL 2
#define CAM_MOVE_PAUSE_SCREEN 1
#define MENU_MODE_RENDER_PAUSE_SCREEN 1
#define WARP_SPECIAL_MARIO_HEAD_DIZZY -3
#define SOUND_MENU_THANK_YOU_PLAYING_MY_GAME 77
static struct { int type,arg,levelNum; } sWarpDest;
static struct { int action,yoshiCoinDeathPending; } mario,*gMarioState=&mario;
static struct { int flags; } gHudDisplay;
static void *gCurrCreditsEntry,*sDispCreditsEntry;
static int sDelayedWarpOp,sHealOnCourseEntry,sCurrPlayMode,gMenuOptSelectIndex;
static int sYoshiCoinCakeActive,sCastleBoardCakeActive,sSecretLobbyCakeActive,gCameraMovementFlags,gGlobalSoundSource;
#ifdef PUPPYPRINT_DEBUG
static int gPuppyWarp,gPuppyWarpArea;
static void pause_pcm_stop(void) {}
#endif
static int departure,warps,reloads,gameOvers,labEntries,thanks,quotes,pcmPlaying,pausePressed=1;
static void reset_dialog_render_state(void) {}
static void clear_cutscene_message(void) {}
static void basic_update(void) {}
static void initiate_warp(int level,int area,int node,int arg) {
 assert((level==LEVEL_ENDING || level==LEVEL_CASTLE_GROUNDS) && area==1 && node==10); sWarpDest.arg=arg;sWarpDest.levelNum=level; warps++;
}
static void play_transition(int type,int time,int r,int g,int b) {assert(type==1 && time==30 && r==0 && g==0 && b==0);}
static void level_set_transition(int frames,void(*update)(void)) {assert(frames==30 && update==basic_update);}
static void yoshi_cake_pcm_start(void) {quotes++;}
static int yoshi_cake_pcm_is_playing(void) {return pcmPlaying;}
static void play_sound(int sound,int pos) {(void)pos; assert(sound==77); thanks++;}
static void fadeout_music(int frames) {assert(frames==1);}
static void set_play_mode(int mode) {sCurrPlayMode=mode;}
static void set_menu_mode(int mode) {assert(mode==MENU_MODE_NONE || mode==MENU_MODE_RENDER_PAUSE_SCREEN);}
static int pressed_pause(void) {return pausePressed;}
static void lower_background_noise(int x) {(void)x;}
static void cancel_rumble(void) {}
static void save_file_reload(void) {reloads++;}
static void fade_into_special_warp(int dest,int arg) {assert(dest==-3 && arg==0); gameOvers++;}
static void level_warp_to_physics_lab(void) {labEntries++;}
static void play_mode_paused(void) {}
static int play_mode_change_level(void) {return departure;}
static void reset_volume(void) {}
static void enable_background_sound(void) {}
/* PRODUCTION */
int main(void) {
 level_warp_to_castle_board_cake(); assert(warps==1 && sWarpDest.arg==32);
 lvl_end_screen(0,0); lvl_play_the_end_screen_sound(0,0);
 assert(sCastleBoardCakeActive && !sYoshiCoinCakeActive && thanks==1 && quotes==0);
 for(int frame=0;frame<299;frame++) lvl_end_screen(1,0);
 assert(reloads==0 && gameOvers==0 && sCurrPlayMode==PLAY_MODE_NORMAL);
 lvl_end_screen(1,0); assert(reloads==1 && gameOvers==1 && labEntries==0);
 // Ordinary completion cake still returns to the Lab.
 sWarpDest.arg=0; lvl_end_screen(0,0);
 pausePressed=0;
 assert(!sYoshiCoinCakeActive && !sCastleBoardCakeActive);
 for(int frame=0;frame<300;frame++)lvl_end_screen(1,0);
 assert(labEntries==1 && gameOvers==1 && reloads==1);
 // Existing Yoshi cake retains its quote and waits for it before Game Over.
 sWarpDest.arg=16; lvl_end_screen(0,0);lvl_play_the_end_screen_sound(0,0);
 pcmPlaying=1;for(int frame=0;frame<310;frame++)lvl_end_screen(1,0);
 assert(gameOvers==1 && quotes==1);
 pcmPlaying=0;lvl_end_screen(1,0);assert(gameOvers==2 && reloads==2);
 // The earned finale cake ignores time and pause, including an externally forced pause.
 sWarpDest.arg=64;lvl_end_screen(0,0);pausePressed=1;
 for(int frame=0;frame<100000;frame++)assert(!lvl_end_screen(1,0));
 assert(sCurrPlayMode==PLAY_MODE_NORMAL && labEntries==1 && gameOvers==2);
 sCurrPlayMode=PLAY_MODE_PAUSED;assert(!lvl_end_screen(1,0));assert(sCurrPlayMode==PLAY_MODE_NORMAL);
 sCurrPlayMode=PLAY_MODE_CHANGE_LEVEL;departure=LEVEL_ENDING;
 assert(lvl_end_screen(1,0)==LEVEL_CASTLE_GROUNDS && sWarpDest.levelNum==LEVEL_CASTLE_GROUNDS);
#ifdef PUPPYPRINT_DEBUG
 sWarpDest.arg=64;lvl_end_screen(0,0);gPuppyWarp=LEVEL_ENDING;gPuppyWarpArea=7;
 assert(lvl_end_screen(1,0)==LEVEL_CASTLE_GROUNDS && sWarpDest.levelNum==LEVEL_CASTLE_GROUNDS);
#endif
 // A later ordinary cake visit does not inherit the indefinite finale flag.
 sWarpDest.arg=0;lvl_end_screen(0,0);assert(!sSecretLobbyCakeActive);
}
'''
        functions = source.function('level_warp_to_castle_board_cake')
        functions += source.function('lvl_play_the_end_screen_sound')
        functions += source.macros('END_SCREEN_IDLE_FRAMES') + source.function('lvl_end_screen')
        code = code.replace('/* PRODUCTION */',functions)
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            (folder / 'test.c').write_text(code)
            for flags in ([], ['-DPUPPYPRINT_DEBUG']):
                subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror','-fsanitize=undefined',
                            *flags,str(folder/'test.c'),'-o',str(folder/'test')],check=True)
                subprocess.run([str(folder/'test')],check=True)


if __name__ == '__main__':
    unittest.main()
