#!/usr/bin/env python3
"""Exercise actual game-facing clock and star wrappers with production timing state."""
from pathlib import Path
import subprocess
import tempfile
import unittest
from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class TimingRuntimeTests(unittest.TestCase):
    def test_best_times_lists_only_real_course_stars(self):
        render = Source(ROOT / "src/game/game_options_menu_render.c").function("render_best_times")
        fixture = r'''
#include <assert.h>
#include <string.h>
#include "game_timing.h"
#include "game_options_menu.h"
struct GameOptionsMenuState gOptionsMenu;
static s32 rows;
static char labels[7][24];
static void render_version(void) {}
static const char *player_stats_menu_course_name(s32 course) {(void)course;return "Course";}
static void player_stats_menu_text(s16 x,s16 y,const char *text) {
    if (x==24 && y>=63 && y<=165) {assert(rows<7);strcpy(labels[rows++],text);}
}
'''+render+r'''
int main(void) {
    game_timing_init(NULL,NULL,NULL);
    for (s32 course=COURSE_MIN;course<COURSE_CAKE_END;course++) {
        gOptionsMenu.recordCourse=course;rows=0;
        render_best_times();
        if (course<=COURSE_STAGES_MAX) {
            assert(rows==7 && strcmp(labels[6],"100 coins")==0);
        } else if (course==COURSE_PSS) {
            assert(rows==2 && strcmp(labels[0],"Slide box")==0);
            assert(strcmp(labels[1],"Fast slide")==0);
        } else {
            assert(rows==1 && strcmp(labels[0],"8 red coins")==0);
        }
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix="sm64-record-course-stars-") as temporary:
            d=Path(temporary);(d/"PR").mkdir()
            (d/"PR/ultratypes.h").write_text("""#ifndef RECORD_TYPES
#define RECORD_TYPES
#include <stdint.h>
typedef int8_t s8;typedef uint8_t u8;typedef int16_t s16;typedef uint16_t u16;
typedef int32_t s32;typedef uint32_t u32;typedef uint64_t u64;typedef float f32;
#endif
""")
            (d/"fixture.c").write_text(fixture)
            subprocess.run(["cc","-std=c11","-Wall","-Wextra","-Werror",
                            "-fsanitize=undefined","-fno-sanitize-recover=all",
                            "-I",str(d),"-I",str(ROOT/"src/game"),"-I",str(ROOT/"include"),
                            "-I",str(ROOT),str(d/"fixture.c"),str(ROOT/"src/game/game_timing.c"),
                            "-o",str(d/"fixture")],check=True)
            subprocess.run([str(d/"fixture")],check=True)

    def test_world_interruptions_identity_and_permissions(self):
        sm64 = Source(ROOT / "include/sm64.h")
        runtime = Source(ROOT / "src/game/game_timing_runtime.c")
        fixture = r'''
#include <assert.h>
#include <string.h>
#include "game_timing.h"
#include "game_options.h"
#define DIALOG_NONE -1
#define TRUE 1
#define FALSE 0
struct MarioState { u32 action; s16 health; u16 actionState; } mario, *gMarioState=&mario;
struct Object { u32 oInteractionSubtype; s32 model; };
struct { u8 isActive; } gWarpTransition;
void *gCurrentArea=(void *)1, *gCurrDemoInput, *gCurrCreditsEntry;
s16 gCurrSaveFileNum=1,gCurrCourseNum=COURSE_BOB,gCurrLevelNum=9;
s8 gDebugLevelSelect;
u32 gGlobalTimer, gTimeStopState;
static s32 activeFile,dialog=-1;
static u8 values[GO_COUNT];
s32 game_options_active_file(void) { return activeFile; }
u8 game_options_get(enum GameOptionId id) { return values[id]; }
s32 get_dialog_id(void) { return dialog; }
s32 obj_has_model(struct Object *obj,s32 model) { return obj->model==model; }
'''
        fixture += sm64.declaration("enum", "MarioActionGroups")
        fixture += sm64.declaration("enum", "MarioActionFlags")
        fixture += sm64.macros("ACT_.*")
        fixture += Source(ROOT / "src/game/mario_actions_automatic.h").declaration(
            "enum", "ActionStatesInCannon")
        fixture += Source(ROOT / "src/game/object_list_processor.h").declaration(
            "enum", "gTimeStopStateFlags")
        fixture += Source(ROOT / "include/model_ids.h").macros("MODEL_BOWSER_KEY")
        fixture += Source(ROOT / "src/game/interaction.h").declaration("enum", "InteractSubtypes")
        fixture += """
#define OPTION_DEF(id,category,kind,label,minimum,maximum,step,defaultValue) \
    [GO_##id]={label,GO_##category,GO_##kind,minimum,maximum,step,defaultValue},
const struct GameOptionDefinition gGameOptionDefinitions[GO_COUNT]={GAME_OPTION_CATALOG(OPTION_DEF)};
#undef OPTION_DEF
"""
        fixture += Source(ROOT / "src/game/game_options_runtime.c").function("game_options_is_cheating")
        fixture += Source(ROOT / "src/game/game_options_runtime.c").function("game_options_runtime_allowed")
        fixture += "\ns32 game_options_secrets_blocked(void) { return game_options_is_cheating(); }\n"
        fixture += "\n".join(runtime.function(n) for n in (
            "timing_tainted", "game_timing_runtime_entered", "game_timing_runtime_update",
            "game_timing_runtime_star"))
        fixture += r'''
static void tick(s32 running) { gGlobalTimer++; game_timing_runtime_update(running); }
int main(void) {
    game_timing_init(NULL,NULL,NULL);game_timing_select_file(0);
    mario.action=ACT_IDLE;mario.health=0x880;
    game_timing_runtime_entered();
    tick(1);u32 time=game_timing_state()->sessionTicks;
    assert(time==GAME_TIMING_TICKS_PER_UPDATE && time==game_timing_state()->courseTicks);
    game_timing_runtime_update(1);
    assert(game_timing_state()->sessionTicks==time);
    // Cannon entry is scripted; READY aiming is gameplay and reaches the star record below.
    mario.action=ACT_IN_CANNON;mario.actionState=ACT_STATE_IN_CANNON_INIT;tick(1);
    mario.actionState=ACT_STATE_IN_CANNON_WAIT_FOR_CANNON;tick(1);
    assert(game_timing_state()->sessionTicks==time && game_timing_state()->courseTicks==time);
    mario.actionState=ACT_STATE_IN_CANNON_READY;tick(1);time+=GAME_TIMING_TICKS_PER_UPDATE;
    assert(game_timing_state()->sessionTicks==time && game_timing_state()->courseTicks==time);
    tick(0);dialog=4;tick(1);dialog=-1;
    mario.health=0xFF;tick(1);mario.health=0x880;
    gWarpTransition.isActive=1;tick(1);gWarpTransition.isActive=0;
    assert(game_timing_state()->sessionTicks==time && game_timing_state()->courseTicks==time);
    mario.action=ACT_IDLE;
    // Star reveals can freeze Mario without changing his ordinary action.
    gTimeStopState=TIME_STOP_ACTIVE|TIME_STOP_MARIO_AND_DOORS;tick(1);
    gTimeStopState=TIME_STOP_ACTIVE|TIME_STOP_ALL_OBJECTS;tick(1);
    assert(game_timing_state()->sessionTicks==time && game_timing_state()->courseTicks==time);
    // Object-only stops and requested-but-inactive stops still let Mario update.
    gTimeStopState=TIME_STOP_ACTIVE;tick(1);
    gTimeStopState=TIME_STOP_MARIO_AND_DOORS;tick(1);
    gTimeStopState=0;time+=2*GAME_TIMING_TICKS_PER_UPDATE;
    assert(game_timing_state()->sessionTicks==time && game_timing_state()->courseTicks==time);
    // Pause/loading caller policy, dialogue, fades, scripted actions and death all freeze clocks.
    tick(0);dialog=4;tick(1);dialog=-1;
    gWarpTransition.isActive=1;tick(1);gWarpTransition.isActive=0;
    mario.action=ACT_DISAPPEARED;tick(1);mario.action=ACT_IDLE;
    mario.health=0xFF;tick(1);mario.health=0x880;
    gCurrentArea=NULL;tick(1);gCurrentArea=(void *)1;
    gMarioState=NULL;tick(1);gMarioState=&mario;
    gCurrDemoInput=(void *)1;tick(1);gCurrDemoInput=NULL;
    gCurrCreditsEntry=(void *)1;tick(1);gCurrCreditsEntry=NULL;
    activeFile=1;tick(1);activeFile=0;
    assert(game_timing_state()->sessionTicks==time);
    game_timing_runtime_entered();
    assert(game_timing_state()->courseTicks==time);
    // Timer visibility never controls collection.
    assert(!values[GO_COURSE_TIMER] && !values[GO_SESSION_TIMER]);
    struct Object obj={.model=MODEL_BOWSER_KEY};
    game_timing_runtime_star(&obj,0);
    assert(!game_timing_best(0,COURSE_BOB,0));
    obj.model=0;obj.oInteractionSubtype=INT_SUBTYPE_GRAND_STAR;
    game_timing_runtime_star(&obj,0);
    assert(!game_timing_best(0,COURSE_BOB,0));
    obj.oInteractionSubtype=0;game_timing_runtime_star(&obj,6);
    assert(game_timing_best(0,COURSE_BOB,6)==time);
    assert(!game_timing_best(0,COURSE_BOB,0));
    values[GO_IGNORE_WIND]=1;tick(0);values[GO_IGNORE_WIND]=0;
    assert(!game_timing_state()->eligible);
    // Late enablement at collection and later disablement cannot restore eligibility.
    values[GO_CHEATS_ENABLED]=1;game_timing_runtime_star(&obj,0);
    values[GO_CHEATS_ENABLED]=0;game_timing_runtime_star(&obj,0);
    assert(!game_timing_best(0,COURSE_BOB,0));
    game_timing_end_course();game_timing_runtime_entered();
    assert(game_timing_state()->sessionTicks==time && !game_timing_state()->courseTicks);
    values[GO_DEBUG_MODE]=1;tick(1);values[GO_DEBUG_MODE]=0;
    game_timing_runtime_star(&obj,1);assert(!game_timing_best(0,COURSE_BOB,1));
    game_timing_end_course();game_timing_runtime_entered();
    gDebugLevelSelect=1;tick(1);gDebugLevelSelect=0;
    assert(!game_timing_state()->eligible);
    game_timing_end_course();mario.action=ACT_DEBUG_FREE_MOVE;game_timing_runtime_entered();
    assert(!game_timing_state()->eligible);mario.action=ACT_IDLE;
    game_timing_end_course();game_timing_runtime_entered();tick(1);
    game_timing_warp(9,0);game_timing_runtime_entered();
    assert(game_timing_state()->courseTicks==GAME_TIMING_TICKS_PER_UPDATE);
    gCurrCourseNum=COURSE_WF;gCurrLevelNum=24;game_timing_runtime_entered();
    assert(!game_timing_state()->courseTicks && game_timing_state()->eligible);
    game_timing_select_file(0);game_timing_runtime_entered();
    assert(!game_timing_state()->sessionTicks);
    return 0;
}
'''
        for pal in (False, True):
            with self.subTest(pal=pal), tempfile.TemporaryDirectory(prefix="sm64-time-world-") as temporary:
                d = Path(temporary); (d / "PR").mkdir()
                (d / "PR/ultratypes.h").write_text("""#ifndef WORLD_TYPES
#define WORLD_TYPES
#include <stdint.h>
typedef int8_t s8; typedef uint8_t u8; typedef int16_t s16; typedef uint16_t u16;
typedef int32_t s32; typedef uint32_t u32; typedef uint64_t u64; typedef float f32;
#endif
""")
                (d / "fixture.c").write_text(fixture)
                command = ["cc", "-std=c11", "-Wall", "-Wextra", "-Werror",
                           "-fsanitize=undefined", "-fno-sanitize-recover=all", "-I", str(d),
                           "-I", str(ROOT / "src/game"), "-I", str(ROOT / "include"),
                           "-I", str(ROOT), str(d / "fixture.c"),
                           str(ROOT / "src/game/game_timing.c"), "-o", str(d / "fixture")]
                if pal: command.insert(1, "-DVERSION_EU")
                subprocess.run(command, check=True)
                subprocess.run([str(d / "fixture")], check=True)


if __name__ == "__main__":
    unittest.main()
