"""Actual option permissions and debug input handlers with supplied engine services."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]

FIXTURE = r'''
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#define PUPPYPRINT_DEBUG
#define USE_PROFILER
#define VISUAL_DEBUG
#define ARRAY_COUNT(a) (sizeof(a)/sizeof((a)[0]))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(a) ((a)<0?-(a):(a))
#define U_JPAD 0x0800
#define D_JPAD 0x0400
#define L_JPAD 0x0200
#define R_JPAD 0x0100
#define A_BUTTON 0x8000
#define B_BUTTON 0x4000
#define Z_TRIG 0x2000
#define L_TRIG 0x0020
#define R_TRIG 0x0010
#define ACT_FLAG_AIR 0x0800
#define ACT_FLAG_SWIMMING 0x2000
#define MARIO_CHARACTER_COUNT 2
#define DEBUG_DPAD_REPEAT_DELAY_FRAMES 10
#define DEBUG_DPAD_REPEAT_INTERVAL_FRAMES 3
#define DEADZONE 8
#define PROFILER_TIME_PUPPYPRINT2 2
#define PUPPYPRINT_GET_SNAPSHOT() u32 first = 0
#include "game_options.c"
s16 gCurrSaveFileNum = 1;
s8 gDebugLevelSelect;
void *gCurrDemoInput, *gCurrCreditsEntry;
struct Controller {u16 buttonPressed,buttonDown; f32 stickMag; s32 rawStickX,rawStickY;};
static struct Controller controller, *gPlayer1Controller=&controller;
struct Object {f32 oPosVec[3]; s32 oAction,oInteractStatus,oYoshiBlinkTimer,hidden,intangible;};
struct MarioState {u32 action; f32 pos[3];};
static struct MarioState mario, *gMarioState=&mario;
#ifndef ACT_DEBUG_FREE_MOVE
#define ACT_DEBUG_FREE_MOVE 0x0000130F
#endif
#define ACT_UNINITIALIZED 0x00000000
#include "game_options_runtime.c"
static u8 fDebug,sDebugMenu,sPPDebugPage,sDebugOption,logViewer,gDebugFastWalk;
static u8 viewCycle,hitboxView,surfaceView,sDebugRepeatTimer,gPuppyWarp,gPuppyWarpArea;
static u16 sDebugHeldDpad,sDebugManualDirections;
static s32 sLevelSelectOption,sLevelSelectOptionWarp,gCurrLevelNum,gPPSegScroll;
static s32 warps,characterChanges,benchmarks,resets,appearances;
static struct {int unused;} ppPages[16];
static u32 osGetCount(void) {return 0;}
static void profiler_update(s32 id,u32 value) {(void)id;(void)value;}
static s32 mario_character_requested_index(void) {return 0;}
static s32 mario_character_request(s32 index) {(void)index;characterChanges++;return TRUE;}
static void mario_character_apply_appearance(s32 demo) {(void)demo;appearances++;}
static void puppyprint_update_level_select(u16 buttons) {(void)buttons;warps++;}
static s32 level_select_index(s32 level) {(void)level;return 0;}
static s32 level_select_warp_count(s32 level) {(void)level;return 4;}
static void collision_mesh_benchmark_run(void) {benchmarks++;}
static void collision_mesh_benchmark_reset(void) {resets++;}
'''

CHECKS = r'''
static void confirm(enum GameOptionId id) {
    assert(game_options_confirmation_begin(id));
    assert(!game_options_confirmation_update(0,0,0));
    for(u32 frame=1;frame<=game_options_confirmation_duration();frame++) {
        assert(!game_options_confirmation_update(frame,1,0));
    }
    assert(game_options_confirmation_update(game_options_confirmation_duration()+1,1,0));
}
static void fresh(void) {
    game_options_init(NULL,NULL); game_options_select_file(0);
    gCurrSaveFileNum=1; gCurrDemoInput=gCurrCreditsEntry=NULL;
    memset(&controller,0,sizeof(controller));
    warps=characterChanges=benchmarks=resets=appearances=0;
    puppyprint_debug_update();
}
static void cheating(void) {
    fresh(); gDebugLevelSelect=0; mario.action=0;
    assert(!game_options_is_cheating());
    for(s32 id=0;id<GO_COUNT;id++) {
        fresh(); gDebugLevelSelect=0; mario.action=0;
        if(gGameOptionDefinitions[id].category!=GO_CHEATS) continue;
        if(id==GO_DEBUG_MODE || id==GO_CHEATS_ENABLED) confirm(id);
        else assert(game_options_set(id,1));
        assert(game_options_is_cheating());
        assert(game_options_set(id,0));
        assert(!game_options_is_cheating());
    }
    fresh(); gDebugLevelSelect=1; assert(game_options_is_cheating());
    gDebugLevelSelect=0; mario.action=ACT_DEBUG_FREE_MOVE;
    assert(game_options_is_cheating());
    mario.action=0;
    game_options_set(GO_INPUT_DISPLAY,1); game_options_set(GO_DEBOUNCE,3);
    assert(!game_options_is_cheating());
}

#define YOSHI_ACT_IDLE 0
#define YOSHI_ACT_WALK 1
#define YOSHI_ACT_TALK 2
#define YOSHI_ACT_WALK_JUMP_OFF_ROOF 3
#define YOSHI_ACT_FINISH_JUMPING_AND_DESPAWN 4
#define YOSHI_ACT_GIVE_PRESENT 5
#define YOSHI_ACT_CREDITS 6
#define INT_STATUS_NONE 0
static struct Object yoshiObject, *o = &yoshiObject;
static s32 yoshiUpdates;
static void cur_obj_hide(void) { o->hidden = 1; }
static void cur_obj_unhide(void) { o->hidden = 0; }
static void cur_obj_become_intangible(void) { o->intangible = 1; }
static void cur_obj_become_tangible(void) { o->intangible = 0; }
static void yoshi_idle_loop(void) { yoshiUpdates++; }
static void yoshi_walk_loop(void) { yoshiUpdates++; }
static void yoshi_talk_loop(void) { yoshiUpdates++; }
static void yoshi_walk_and_jump_off_roof_loop(void) { yoshiUpdates++; }
static void yoshi_finish_jumping_and_despawn_loop(void) { yoshiUpdates++; }
static void yoshi_give_present_loop(void) { yoshiUpdates++; }
static void cur_obj_init_animation(s32 id) { (void) id; }
static void curr_obj_random_blink(s32 *timer) { (void) timer; }
void bhv_yoshi_loop(void);
static void yoshi_course_eligibility(void) {
    gDebugLevelSelect = 0; mario.action = 0; fresh();
    game_options_runtime_restart();
    for (s32 action = YOSHI_ACT_IDLE; action <= YOSHI_ACT_WALK; action++) {
        *o = (struct Object){ .oAction = action };
        yoshiUpdates = 0;
        bhv_yoshi_loop();
        assert(yoshiUpdates == 1 && !o->hidden && !o->intangible);
        assert(game_options_set(GO_FREE_MOVE, 1));
        assert(game_options_set(GO_FREE_MOVE, 0));
        bhv_yoshi_loop();
        assert(yoshiUpdates == 1 && o->hidden && o->intangible);
        game_options_runtime_update();
        bhv_yoshi_loop();
        assert(yoshiUpdates == 1 && o->hidden);
        o->oAction = YOSHI_ACT_TALK;
        bhv_yoshi_loop(); // Already-started conversation can finish.
        assert(yoshiUpdates == 2);
        game_options_runtime_restart();
        *o = (struct Object){ .oAction = action };
        bhv_yoshi_loop();
        assert(yoshiUpdates == 3 && !o->hidden && !o->intangible);
    }
}
static void visit_eligibility(void) {
    for (s32 id = 0; id < GO_COUNT; id++) {
        if (gGameOptionDefinitions[id].category != GO_CHEATS) continue;
        gDebugLevelSelect = 0; mario.action = 0; fresh();
        game_options_runtime_restart();
        assert(!game_options_secrets_blocked());
        if (id == GO_DEBUG_MODE || id == GO_CHEATS_ENABLED) confirm(id);
        else assert(game_options_set(id, 1));
        // Disable before a world update: even a brief enable taints the visit.
        assert(game_options_set(id, 0));
        assert(!game_options_is_cheating() && game_options_secrets_blocked());
        game_options_runtime_update();
        assert(game_options_secrets_blocked());
        game_options_runtime_restart();
        assert(!game_options_secrets_blocked());
    }
    gDebugLevelSelect = 0; mario.action = 0; fresh();
    game_options_runtime_restart();
    mario.action = ACT_DEBUG_FREE_MOVE;
    game_options_runtime_update();
    mario.action = 0;
    assert(!game_options_is_cheating() && game_options_secrets_blocked());
    game_options_runtime_restart();
    assert(!game_options_secrets_blocked());
    gDebugLevelSelect = 1;
    game_options_runtime_update(); // Revocation must not erase the visit history.
    assert(!gDebugLevelSelect && game_options_secrets_blocked());
    game_options_runtime_restart();
    assert(!game_options_secrets_blocked());
    confirm(GO_DEBUG_MODE);
    game_options_runtime_restart();
    assert(game_options_secrets_blocked());
    assert(game_options_set(GO_DEBUG_MODE, 0));
    assert(game_options_secrets_blocked());
    game_options_select_file(1);
    game_options_runtime_restart();
    assert(!game_options_secrets_blocked());
}

static void restart_from_debug_flight(void) {
    gDebugLevelSelect = 0; mario.action = 0; fresh();
    game_options_runtime_restart();
    confirm(GO_DEBUG_MODE);
    assert(game_options_set(GO_FREE_MOVE, 1));
    mario.action = ACT_DEBUG_FREE_MOVE;
    game_options_runtime_update();
    assert(game_options_set(GO_FREE_MOVE, 0));
    assert(game_options_set(GO_DEBUG_MODE, 0));
    assert(game_options_secrets_blocked());

    // The full reload starts before Mario/object initialization in the new level.
    game_options_runtime_restart();
    assert(mario.action == ACT_UNINITIALIZED);
    assert(!game_options_is_cheating() && !game_options_secrets_blocked());
    *o = (struct Object){ .oAction = YOSHI_ACT_IDLE };
    yoshiUpdates = 0;
    bhv_yoshi_loop();
    assert(yoshiUpdates == 1 && !o->hidden && !o->intangible);

    // Clearing the outgoing action must not bypass a still-selected cheat.
    assert(game_options_set(GO_FORCE_WING_CAP, 1));
    mario.action = ACT_DEBUG_FREE_MOVE;
    game_options_runtime_restart();
    assert(mario.action == ACT_UNINITIALIZED && game_options_secrets_blocked());
    assert(game_options_set(GO_FORCE_WING_CAP, 0));
    gMarioState = NULL;
    game_options_runtime_restart();
    assert(!game_options_secrets_blocked());
    gMarioState = &mario;
}

static void permissions(void) {
    fresh();
    assert(game_options_runtime_allowed());
    assert(!game_options_debug_active() && !game_options_free_move_active());
    assert(!game_options_set(GO_DEBUG_MODE,1));
    assert(!game_options_set(GO_CHEATS_ENABLED,1));
    game_options_set(GO_FREE_MOVE,1); game_options_set(GO_INFINITE_HEALTH,1);
    assert(!game_options_get(GO_KNOCKBACK));
    game_options_set(GO_KNOCKBACK,1);
    assert(!game_options_cheat_active(GO_KNOCKBACK));
    confirm(GO_CHEATS_ENABLED);
    assert(game_options_cheat_active(GO_INFINITE_HEALTH));
    assert(game_options_cheat_active(GO_KNOCKBACK));
    game_options_set(GO_KNOCKBACK,0);
    assert(!game_options_cheat_active(GO_KNOCKBACK));
    assert(!game_options_cheat_active(GO_FREE_MOVE));
    assert(!game_options_cheat_active(GO_DEBUG_MODE));
    assert(!game_options_cheat_active(GO_CHEATS_ENABLED));
    assert(!game_options_cheat_active((enum GameOptionId)-1));
    confirm(GO_DEBUG_MODE); game_options_set(GO_CHEATS_ENABLED,0);
    assert(game_options_free_move_active());
    assert(game_options_cheat_active(GO_FREE_MOVE));
    assert(!game_options_cheat_active(GO_INFINITE_HEALTH));
    void *sentinel=&mario;
    for(s32 state=0;state<3;state++) {
        gCurrDemoInput=state==0?sentinel:NULL;
        gCurrCreditsEntry=state==1?sentinel:NULL;
        gCurrSaveFileNum=state==2?2:1;
        assert(!game_options_runtime_allowed());
        assert(!game_options_debug_active() && !game_options_free_move_active());
        gDebugLevelSelect=TRUE; game_options_runtime_update();
        assert(!gDebugLevelSelect);
    }
    gCurrDemoInput=gCurrCreditsEntry=NULL;gCurrSaveFileNum=1;
    assert(game_options_free_move_active());
    game_options_select_file(1);gCurrSaveFileNum=2;
    assert(!game_options_debug_active());
    game_options_select_file(0);gCurrSaveFileNum=1;
    assert(game_options_get(GO_FREE_MOVE));
    assert(game_options_debug_active() && game_options_free_move_active());
}
static void debug_inputs(void) {
    fresh();
    controller.buttonDown=L_TRIG|U_JPAD;
    controller.buttonPressed=L_TRIG|U_JPAD;
    puppyprint_profiler_process();
#ifndef ENABLE_CREDITS_BENCHMARK
    assert(!fDebug);
#endif
    assert(!sDebugMenu);
    confirm(GO_DEBUG_MODE);
    fDebug=FALSE; puppyprint_profiler_process();
    assert(fDebug && !sDebugMenu);
    controller.buttonDown=controller.buttonPressed=R_JPAD;
    sPPDebugPage=PUPPYPRINT_PAGE_WALK_SPEED;
    puppyprint_profiler_process();assert(gDebugFastWalk);
    sPPDebugPage=PUPPYPRINT_PAGE_LEVEL_SELECT;
    puppyprint_profiler_process();assert(warps==1);
    fDebug=FALSE;puppyprint_profiler_process();assert(warps==1);
    fDebug=TRUE;sDebugMenu=TRUE;puppyprint_profiler_process();assert(warps==1);
    sDebugMenu=FALSE;sPPDebugPage=PUPPYPRINT_PAGE_CHARACTER;
    puppyprint_profiler_process();assert(characterChanges==1 && appearances==1);
    sPPDebugPage=PUPPYPRINT_PAGE_COLLISION;controller.buttonPressed=Z_TRIG;
    puppyprint_profiler_process();assert(benchmarks==1);
    controller.buttonPressed=B_BUTTON;puppyprint_profiler_process();assert(resets==1);
    // Revoke consent with active tools and held shortcuts. No world callbacks run.
    game_options_set(GO_DEBUG_MODE,0);
    sDebugMenu=fDebug=logViewer=gDebugFastWalk=TRUE;
    sPPDebugPage=sDebugOption=PUPPYPRINT_PAGE_LEVEL_SELECT;
    sDebugHeldDpad=sDebugManualDirections=R_JPAD;sDebugRepeatTimer=0;
    viewCycle=hitboxView=surfaceView=3;gPuppyWarp=5;gPuppyWarpArea=2;
    controller.buttonPressed=A_BUTTON|L_TRIG|R_JPAD;
    controller.buttonDown=L_TRIG|U_JPAD;
    puppyprint_profiler_process();
    assert(warps==1 && characterChanges==1 && benchmarks==1 && resets==1);
    assert(!sDebugMenu && !gDebugFastWalk && !logViewer);
    assert(sPPDebugPage==PUPPYPRINT_PAGE_GENERAL && sDebugOption==PUPPYPRINT_PAGE_GENERAL);
    assert(!sDebugHeldDpad && !sDebugManualDirections);
    assert(sDebugRepeatTimer==DEBUG_DPAD_REPEAT_DELAY_FRAMES);
    assert(!viewCycle && !hitboxView && !surfaceView);
    assert(!gPuppyWarp && !gPuppyWarpArea);
#ifdef ENABLE_CREDITS_BENCHMARK
    assert(fDebug);
#else
    assert(!fDebug);
#endif
    debug_box_input();assert(!viewCycle);
    confirm(GO_DEBUG_MODE); controller.buttonPressed=R_JPAD;
    debug_box_input();assert(viewCycle==1 && hitboxView && !surfaceView);
    gCurrDemoInput=&mario;puppyprint_profiler_process();
    assert(!gDebugFastWalk && !sDebugMenu);
}
int main(void) {
    cheating();visit_eligibility();yoshi_course_eligibility();restart_from_debug_flight();
    permissions();debug_inputs();
    puts("debug permissions, revocation and overlay controls checks passed");
    return 0;
}
'''


class OptionsDebugTests(unittest.TestCase):
    def test_permissions_and_controls(self):
        puppy = Source(ROOT / 'src/game/puppyprint.c')
        sources = Source(ROOT / 'src/game/puppyprint.h').declaration('enum', 'PPPages')
        sources += '\n'.join(puppy.function(name) for name in (
            'puppyprint_debug_dpad_buttons', 'puppyprint_debug_move',
            'puppyprint_debug_update', 'puppyprint_profiler_process'))
        sources += Source(ROOT / 'src/game/debug_box.c').function('debug_box_input')
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            (directory / 'PR').mkdir()
            (directory / 'PR/ultratypes.h').write_text(
                '#include <stdint.h>\n'
                'typedef uint8_t u8;typedef int8_t s8;typedef uint16_t u16;typedef int16_t s16;\n'
                'typedef uint32_t u32;typedef int32_t s32;typedef float f32;\n')
            for name in ('game_options.c', 'game_options.h',
                         'game_options_runtime.c', 'game_options_runtime.h'):
                (directory / name).write_text((ROOT / 'src/game' / name).read_text())
            (directory / 'area.h').write_text('extern s16 gCurrSaveFileNum;\n')
            (directory / 'game_init.h').write_text('extern void *gCurrDemoInput;\n')
            (directory / 'level_update.h').write_text('extern void *gCurrCreditsEntry;\n')
            (directory / 'main.h').write_text('extern s8 gDebugLevelSelect;\n')
            (directory / 'mario.h').write_text('')
            fixture = directory / 'fixture.c'
            fixture.write_text(FIXTURE + sources + CHECKS
                               + Source(ROOT / 'src/game/behaviors/yoshi.inc.c').function('bhv_yoshi_loop'))
            for defines in ([], ['-DVERSION_EU'], ['-DENABLE_CREDITS_BENCHMARK']):
                executable = directory / 'debug-test'
                subprocess.run([
                    'cc', '-std=c11', '-Wall', '-Wextra', '-Werror',
                    '-fsanitize=address,undefined', "-fno-sanitize-recover=all", '-fno-omit-frame-pointer', *defines,
                    '-I', str(directory), str(fixture), '-lm', '-o', str(executable),
                ], check=True, timeout=30)
                subprocess.run([str(executable)], check=True,
                               env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0'), timeout=10)


if __name__ == '__main__':
    unittest.main()
