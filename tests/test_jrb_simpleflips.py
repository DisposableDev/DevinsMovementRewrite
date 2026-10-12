#!/usr/bin/env python3
"""Encounter lifecycle and shipped dialogue/animation boundaries, using production C."""
from pathlib import Path
import re
import json
import subprocess
import tempfile
import unittest

from regression_support import Source

try:
    from .source_asset import read_text as read_source
except ImportError:
    from source_asset import read_text as read_source

ROOT = Path(__file__).resolve().parents[1]


def read_us_dialog_widths():
    table = read_source(ROOT / "src/game/ingame_menu.c").split(
        "u8 gDialogCharWidths[256] = {")[1].split("\n};")[0]
    # Use the native conditional selection; regex branch removal can combine regions.
    selected = subprocess.run(
        ["cc", "-E", "-P", "-x", "c", "-DVERSION_US", "-"],
        input=table, text=True, capture_output=True, check=True).stdout
    return [int(number) for number in re.findall(r"\b\d+\b", selected)]


def prepare_head_fixture(folder):
    source = Source(ROOT / "src/menu/simpleflips_head.c")
    header = Source(ROOT / "src/menu/simpleflips_head.h")
    definitions = header.macros("SIMPLEFLIPS_HEAD_.*")
    definitions += header.declaration("struct", "SimpleFlipsHeadVertexGroup")
    definitions += header.declaration("struct", "SimpleFlipsHeadMesh")
    asset = read_source(ROOT / "levels/intro/simpleflips_head.inc.c")
    vertices = "\n".join(re.findall(
        r"static const Vtx intro_simpleflips_toad_\w+\[\] = \{.*?\n\};", asset, re.S))
    vertices += "\n" + re.search(
        r"static const struct SimpleFlipsHeadVertexGroup \w+\[\] = \{.*?\n\};", asset, re.S)[0]
    (folder / "head_definitions.inc").write_text(definitions)
    (folder / "head_vertices.inc").write_text(vertices)
    (folder / "head_actual.inc").write_text(source.region(r"^#define HEAD_GRAB_RADIUS_PIXELS", r"\Z"))


def prepare_npc_dialog_fixture(folder, motos=False):
    source = Source(ROOT / "src/game/jrb_simpleflips.c")
    cutscene = Source(ROOT / "src/game/mario_actions_cutscene.c")
    helpers = Source(ROOT / "src/game/object_helpers.c")
    actions = Source(ROOT / "include/sm64.h")
    definitions = actions.declaration("enum", "MarioFlags")
    definitions += actions.macros(
        "ACT_READING_NPC_DIALOG|ACT_IDLE|ACT_HOLD_IDLE|ACT_PUTTING_ON_CAP")
    for path, names in (
        ("include/object_constants.h", ("ObjActiveFlags", "DialogState", "DialogFlags")),
        ("src/game/mario_actions_cutscene.h", ("ActionArgsMarioDialog", "DialogStates",
                                                  "ActionStatesReadingNpcDialog")),
        ("src/game/ingame_menu.h", ("DialogResponseDefines",)),
        ("src/game/object_list_processor.h", ("gTimeStopStateFlags",)),
        ("src/game/interaction.h", ("ObjInteractTypes", "InteractStatus")),
    ):
        header = Source(ROOT / path)
        definitions += "".join(header.declaration("enum", name) for name in names)
    production = cutscene.function("set_mario_npc_dialog")
    production += cutscene.function("act_reading_npc_dialog")
    production += helpers.function("cur_obj_end_dialog")
    production += helpers.function("cur_obj_update_dialog")
    if motos:
        definitions += Source(ROOT / "src/game/save_file.h").declaration("enum", "SaveProgressFlags")
        definitions += Source(ROOT / "src/game/object_helpers.h").declaration("enum", "WaterDropletFlags")
        definitions += Source(ROOT / "src/game/object_helpers.h").declaration("struct", "WaterDropletParams")
        definitions += Source(ROOT / "src/game/particle_budget.h").declaration("enum", "ParticlePriority")
        definitions += Source(ROOT / "src/game/save_file.h").macros("CASTLE_SECRET_MOTOS_TALKED")
        production += source.function("simpleflips_motos_disappear")
        production += source.function("bhv_jrb_simpleflips_motos_loop")
        production += source.function("jrb_simpleflips_restore_cap")
    else:
        production += source.function("bhv_jrb_simpleflips_mips_loop")
    (folder / "dialog_definitions.inc").write_text(definitions)
    (folder / "dialog_actual.inc").write_text(production)


def prepare_one_up_fixture(folder):
    source = Source(ROOT / "src/game/jrb_simpleflips.c")
    mushrooms = Source(ROOT / "src/game/behaviors/mushroom_1up.inc.c")
    production = source.macros("SIMPLEFLIPS_ONE_UP_TWIRL_FRAMES")
    production += mushrooms.macros("ONE_UP_SPARKLE_EMIT_UPDATES|ONE_UP_SPARKLE_EMISSION_PERIOD_UPDATES")
    production += mushrooms.function("one_up_spawn_sparkles")
    production += mushrooms.function("bhv_1up_common_init")
    production += mushrooms.function("one_up_loop_in_air")
    production += source.function("bhv_jrb_simpleflips_one_up_loop")
    (folder / "one_up_actual.inc").write_text(production)


class SimpleFlipsTests(unittest.TestCase):
    def test_room_dialog_requires_fresh_press(self):
        menu = Source(ROOT / "src/game/ingame_menu.c")
        header = Source(ROOT / "src/game/ingame_menu.h")
        switch = menu.function("render_dialog_entries").split(
            "    switch (gDialogBoxState) {", 1)[1].split(
            "\n    s8 boxLines", 1)[0]
        code = r'''
#include <assert.h>
#define game_options_secrets_blocked() 0
#include <stddef.h>
#include <stdint.h>
#include "dialog_ids.h"
typedef int32_t s32;
typedef int16_t s16;
typedef uint8_t u8;
#define TRUE 1
#define FALSE 0
#define A_BUTTON 0x8000
#define B_BUTTON 0x4000
#define Z_TRIG 0x2000
#define MARIO_TEXT_STANDARD 0
#define SOUND_MENU_MESSAGE_APPEAR 1
#define SOUND_MENU_MESSAGE_NEXT_PAGE 2
#define SOUND_MENU_MESSAGE_DISAPPEAR 3
#define LEVEL_JRB 12
#define JRB_SIMPLEFLIPS_AREA 3
/* DEFINITIONS */
struct DialogEntry { int linesPerBox; const u8 *str; };
static int gCurrLevelNum = LEVEL_JRB, gCurrAreaIndex = JRB_SIMPLEFLIPS_AREA;
static int area, *gCurrentArea = &area;
static int gDialogBoxState, gDialogBoxType, gDialogLineNum, gDialogTextPos;
static int gLastDialogPageStrPos, gLastDialogResponse, gDialogID;
static int gDialogScrollOffsetY, sDialogScrollingBack, gDialogResponse, sDialogTextStyle;
static float gDialogBoxOpenTimer, gDialogBoxScale;
static int gGlobalSoundSource, completions;
static struct { int buttonDown, buttonPressed; } controller, *gPlayer1Controller = &controller;
static const void *segmented_to_virtual(const void *ptr) { return ptr; }
static void play_dialog_sound(int id) { (void)id; }
static void play_sound(int sound, int source) { (void)sound; (void)source; }
static void level_set_transition(int frames, void *callback) { (void)frames; (void)callback; }
static void handle_special_dialog_text(int id) { assert(id == gDialogID); completions++; }
/* PRODUCTION */
static void update(struct DialogEntry *dialog) {
    int lowerBound;
    switch (gDialogBoxState) {
/* SWITCH */
    (void)lowerBound;
}
int main(void) {
    const u8 words[] = {1, 2, 3, DIALOG_CHAR_NEWLINE, 4, 5, 6, DIALOG_CHAR_NEWLINE, 7};
    struct DialogEntry dialog = {1, words};
    const int ids[] = {DIALOG_SIMPLEFLIPS_TOAD, DIALOG_SIMPLEFLIPS_SIGN,
                      DIALOG_SIMPLEFLIPS_MIPS, DIALOG_SIMPLEFLIPS_MOTOS};
    for (unsigned i=0; i<sizeof(ids)/sizeof(ids[0]); i++) {
        gDialogID = ids[i]; gDialogBoxState = DIALOG_STATE_VERTICAL;
        gDialogTextPos = gDialogScrollOffsetY = sDialogScrollingBack = 0;
        gLastDialogPageStrPos = 4; gLastDialogResponse = 0;
        controller.buttonDown = A_BUTTON; controller.buttonPressed = 0;
        for (int frame=0; frame<40; frame++) update(&dialog);
        assert(gDialogBoxState == DIALOG_STATE_VERTICAL && gDialogTextPos == 0);
        controller.buttonPressed = A_BUTTON; update(&dialog);
        assert(gDialogBoxState == DIALOG_STATE_HORIZONTAL);
        controller.buttonPressed = 0;
        for (int frame=0; frame<8; frame++) update(&dialog);
        assert(gDialogBoxState == DIALOG_STATE_VERTICAL && gDialogTextPos == 4);
        gLastDialogPageStrPos = 8;
        for (int frame=0; frame<40; frame++) update(&dialog);
        assert(gDialogBoxState == DIALOG_STATE_VERTICAL && gDialogTextPos == 4);
        // Backward scrolling keeps priority even with a fresh forward press.
        controller.buttonDown = Z_TRIG | A_BUTTON; controller.buttonPressed = A_BUTTON;
        update(&dialog);
        assert(sDialogScrollingBack && gDialogTextPos == 0);
        controller.buttonDown = controller.buttonPressed = 0;
        for (int frame=0; frame<8; frame++) update(&dialog);
        assert(gDialogBoxState == DIALOG_STATE_VERTICAL && !sDialogScrollingBack);
        controller.buttonPressed = B_BUTTON; update(&dialog);
        assert(gDialogBoxState == DIALOG_STATE_HORIZONTAL);
        controller.buttonDown = A_BUTTON; controller.buttonPressed = 0;
        for (int frame=0; frame<8; frame++) update(&dialog);
        gLastDialogPageStrPos = -1;
        int before = completions;
        for (int frame=0; frame<40; frame++) update(&dialog);
        assert(gDialogBoxState == DIALOG_STATE_VERTICAL && completions == before);
        controller.buttonPressed = A_BUTTON; update(&dialog);
        assert(gDialogBoxState == DIALOG_STATE_CLOSING && completions == before + 1);
    }
    // A held before ordinary dialogue opens advances every page without a new edge.
    for (int type=DIALOG_TYPE_ROTATE; type<=DIALOG_TYPE_ZOOM; type++) {
        gCurrAreaIndex = 2; controller.buttonDown = A_BUTTON; controller.buttonPressed = 0;
        gDialogID = DIALOG_001; gDialogBoxState = DIALOG_STATE_OPENING;
        gDialogBoxType = type;
        gDialogBoxOpenTimer = DEFAULT_DIALOG_BOX_ANGLE; gDialogBoxScale = 19.0f;
        gDialogTextPos = gDialogScrollOffsetY = sDialogScrollingBack = 0;
        gLastDialogPageStrPos = 4; gLastDialogResponse = 0;
        for (int frame=0; frame<30 && gDialogBoxState == DIALOG_STATE_OPENING; frame++) update(&dialog);
        assert(gDialogBoxState == DIALOG_STATE_VERTICAL);
        for (int page=1; page<=2; page++) {
            gLastDialogPageStrPos = page * 4;
            update(&dialog);
            assert(gDialogBoxState == DIALOG_STATE_HORIZONTAL);
            for (int frame=0; frame<30 && gDialogBoxState == DIALOG_STATE_HORIZONTAL; frame++) update(&dialog);
            assert(gDialogBoxState == DIALOG_STATE_VERTICAL && gDialogTextPos == page * 4);
            assert(controller.buttonPressed == 0);
        }
        gLastDialogPageStrPos = -1;
        int beforeClose = completions;
        update(&dialog);
        assert(gDialogBoxState == DIALOG_STATE_CLOSING && completions == beforeClose + 1);
    }
    int beforeClose = completions - 1;
    // A carried hold must not accidentally confirm a normal Yes/No choice.
    gDialogBoxState = DIALOG_STATE_VERTICAL; gLastDialogResponse = 1;
    gDialogLineNum = DIALOG_RESPONSE_NO;
    update(&dialog);
    assert(gDialogBoxState == DIALOG_STATE_VERTICAL && completions == beforeClose + 1);
    controller.buttonPressed = A_BUTTON;
    update(&dialog);
    assert(gDialogBoxState == DIALOG_STATE_CLOSING && gDialogLineNum == DIALOG_RESPONSE_NO);
    // Ordinary dialogue and held-A cap-switch saving retain their controls.
    gCurrAreaIndex = 2; controller.buttonDown = A_BUTTON; controller.buttonPressed = 0;
    gDialogID = DIALOG_SIMPLEFLIPS_TOAD; gDialogBoxState = DIALOG_STATE_VERTICAL;
    gLastDialogPageStrPos = 4; update(&dialog);
    assert(gDialogBoxState == DIALOG_STATE_HORIZONTAL);
    gDialogID = DIALOG_010; gDialogBoxState = DIALOG_STATE_VERTICAL;
    gLastDialogPageStrPos = -1; gLastDialogResponse = 1; gDialogLineNum = DIALOG_RESPONSE_NO;
    update(&dialog);
    assert(gDialogBoxState == DIALOG_STATE_CLOSING && gDialogLineNum == DIALOG_RESPONSE_YES);
    return 0;
}
'''
        definitions = menu.declaration("enum", "DialogBoxState")
        definitions += menu.declaration("enum", "DialogBoxType")
        definitions += menu.macros("DEFAULT_DIALOG_BOX_ANGLE|DIAG_VAL1")
        definitions += header.declaration("enum", "DialogResponseDefines")
        definitions += header.declaration("enum", "DialogMark")
        definitions += header.declaration("enum", "DialogSpecialChars")
        code = code.replace("/* DEFINITIONS */", definitions)
        code = code.replace("/* PRODUCTION */", menu.function("get_previous_dialog_page")
                            + Source(ROOT / "src/game/jrb_simpleflips.c").function(
                                "jrb_simpleflips_room_active"))
        code = code.replace("/* SWITCH */", switch)
        with tempfile.TemporaryDirectory(prefix="simpleflips-dialog-input-") as directory:
            folder = Path(directory)
            (folder / "test.c").write_text(code)
            subprocess.run(["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=undefined", "-I", str(ROOT / "include"),
                            str(folder / "test.c"), "-o", str(folder / "test")], check=True)
            subprocess.run([str(folder / "test")], check=True)

    def test_head_grab_release(self):
        with tempfile.TemporaryDirectory(prefix="simpleflips-head-") as directory:
            folder = Path(directory)
            prepare_head_fixture(folder)
            for flags in ([], ["-DHEAD_TEST_NO_WIDE"]):
                subprocess.run(["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror",
                                "-fsanitize=undefined", *flags, "-I", str(folder),
                                str(ROOT / "tests/simpleflips_head_test.c"), "-lm",
                                "-o", str(folder / "test")], check=True)
                subprocess.run([str(folder / "test")], check=True)

    def test_encounter_lifecycle(self):
        source = Source(ROOT / "src/game/jrb_simpleflips.c")
        actions = Source(ROOT / "include/sm64.h")
        code = r'''
#include <assert.h>
static int cheating;
#define game_options_secrets_blocked() cheating
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <string.h>
#include "seq_ids.h"
#include "dialog_ids.h"
typedef int32_t s32;
typedef uint32_t u32;
typedef int16_t s16;
typedef uint8_t u8;
typedef float f32;
typedef int Gfx;
#define TRUE 1
#define FALSE 0
#define UNUSED __attribute__((unused))
#define MODEL_LEVEL_GEOMETRY_03 3
#define MODEL_LEVEL_GEOMETRY_04 4
#define MODEL_LEVEL_GEOMETRY_05 5
#define MODEL_LEVEL_GEOMETRY_06 6
#define MODEL_EXPLOSION 7
#define MODEL_1UP 8
#define STATIC_ASSERT(c,m) _Static_assert(c,m)
#define ARRAY_COUNT(a) (sizeof(a)/sizeof((a)[0]))
#define SEQ_PLAYER_LEVEL 0
#define SEQ_PLAYER_ENV 1
#define SEQ_PLAYER_SFX 2
#define SEQ_BASE_ID 0x7f
#define SEQUENCE_ARGS(p,s) (((p)<<8)|(s))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define INTERACT_TEXT 1
#define INT_SUBTYPE_SIGN 1
#define LEVEL_JRB 12
#define GEO_CONTEXT_RENDER 1
#define INT_STATUS_NONE 0
#define INT_STATUS_INTERACTED 1
#define INT_SUBTYPE_NPC 2
#define INTERACT_NONE 0
#define DIALOG_FLAG_TEXT_DEFAULT 1
#define DIALOG_FLAG_TIME_STOP_ENABLED 2
#define MARIO_DIALOG_LOOK_DOWN 3
#define DIALOG_SIMPLEFLIPS_TOAD 171
#define SOUND_ACTION_HIT 1
#define START_BUTTON 0x1000
#define WARP_OP_NONE 0
#define WARP_OP_DEATH 1
/* DEFINITIONS */
struct GraphNode { int unused; };
struct GraphNodeSwitchCase { struct GraphNode node; int selectedCase; };
struct Object {
    struct { struct { struct { void *curAnim; int animFrame, animYTrans; } animInfo; } gfx; } header;
    void *oAnimations;
    int oInteractionSubtype, oInteractStatus, oInteractType, oMoveAngleYaw;
    int oAction, oTimer, oOpacity, model, intangible, hidden, oBehParams2ndByte;
    const void *behavior;
    float hitboxRadius, hitboxHeight, oPosX, oPosY, oPosZ;
};
struct MarioState {
    int health, hurtCounter, healCounter, respawnHealCounter, invincTimer;
    u32 action;
    float pos[3], vel[3], forwardVel;
    s16 faceAngle[3];
    struct Object *usedObj;
};
static struct Object object;
static struct Object *o = &object;
static struct MarioState mario;
static struct MarioState *gMarioState = &mario;
static int gCurrLevelNum, gCurrAreaIndex, gCurrActNum, area;
static int gCurrSaveFileNum = 1;
static int visited[4];
static int save_file_has_simpleflips_encounter(int file) {
    return file >= 0 && file < 4 && visited[file];
}
static int *gCurrentArea;
static s16 shipWater[7] = { 1, 0, -4095, -4095, 4096, 4096, 2760 };
static s16 *gEnvironmentRegions = shipWater;
static int groundPound, collisionLoads, dialogs, sounds, actionChanges;
static int jrb_simpleflips_anims[2];
static const int bhvJrbSimpleFlipsSign[1], bhvJrbSimpleFlipsMips[1], bhvJrbSimpleFlipsExplosion[1], bhvJrbSimpleFlipsOneUp[1];
static struct Object mips;
static int spawnCount[3], musicStarts, musicStops, musicFades, crashes, pendingGameOvers;
static int statHits,statHealthLost,statToadConversations;
#define PS_TOAD_CONVERSATIONS 92
static void player_stats_damage(void) {statHits++;}
static void player_stats_health(int before,int after) {statHealthLost += before-after;}
static void player_stats_add(int id,int amount) {
    assert(id == PS_TOAD_CONVERSATIONS);statToadConversations += amount;
}
static void save_file_mark_simpleflips_game_over(void) { pendingGameOvers++; }
static char __n64Assert_MessageBuf[513];
static void __n64Assert(const char *file, u32 line, const char *condition) {
    assert(pendingGameOvers == crashes + 1);
    assert(file && line && strncmp(condition,"Nathanial scuttle bug",20)==0);
    assert(strstr(condition,"Japanese video game development bandy")); crashes++;
}
static int obj_has_behavior(struct Object *obj, const void *behavior) { return obj->behavior == behavior; }
static struct Object *spawn_object(struct Object *parent, int model, const void *behavior) {
    assert(parent == o);
    if (behavior == bhvJrbSimpleFlipsMips) { assert(model == MODEL_JRB_SIMPLEFLIPS_MIPS); spawnCount[0]++; }
    else { assert(behavior == bhvJrbSimpleFlipsExplosion && model == MODEL_EXPLOSION); spawnCount[1]++; }
    mips = *o; return &mips;
}
static void spawn_object_relative(int param, int x, int y, int z, struct Object *parent, int model, const void *behavior) {
    assert(param == 0 && x == 0 && y == 200 && z == 0 && parent == o);
    assert(model == MODEL_1UP && behavior == bhvJrbSimpleFlipsOneUp); spawnCount[2]++;
}
static void obj_mark_for_deletion(struct Object *obj) { assert(obj == &mips); obj->hidden = 1; }
static void play_music(int player, int sequence, int fade) {
    assert(player == SEQ_PLAYER_LEVEL && sequence == SEQUENCE_ARGS(4,SEQ_LEVEL_PHYSICS_LAB) && fade == 0); musicStarts++;
}
static void stop_background_music(int sequence) { assert(sequence == SEQUENCE_ARGS(4,SEQ_LEVEL_PHYSICS_LAB)); musicStops++; }
static void fadeout_music(int frames) { assert(frames == 0); musicFades++; }
/* GLOBALS */
static void cur_obj_become_intangible(void) { o->intangible = 1; }
static void cur_obj_become_tangible(void) { o->intangible = 0; }
static void cur_obj_hide(void) { o->hidden = 1; }
static void cur_obj_unhide(void) { o->hidden = 0; }
static void cur_obj_set_model(int model) { o->model = model; }
static void cur_obj_init_animation(int anim) { o->header.gfx.animInfo.curAnim = &jrb_simpleflips_anims[anim]; }
static void load_object_collision_model(void) { collisionLoads++; }
static int cur_obj_is_mario_ground_pounding_platform(void) { return groundPound; }
static int cur_obj_update_dialog(int look, int flags, int dialog, int unused) {
    assert(look == MARIO_DIALOG_LOOK_DOWN && unused == 0);
    assert(dialog == DIALOG_SIMPLEFLIPS_TOAD || dialog == DIALOG_SIMPLEFLIPS_MIPS);
    assert(flags == (dialog == DIALOG_SIMPLEFLIPS_TOAD
                     ? DIALOG_FLAG_TEXT_DEFAULT | DIALOG_FLAG_TIME_STOP_ENABLED
                     : DIALOG_FLAG_TEXT_DEFAULT));
    return ++dialogs == (dialog == DIALOG_SIMPLEFLIPS_TOAD ? 30 : 3);
}
static s16 atan2s(float z, float x) { return (s16)(atan2f(x,z) * 32768.0f / 3.14159265f); }
static float sins(s16 yaw) { return sinf(yaw * 3.14159265f / 32768.0f); }
static float coss(s16 yaw) { return cosf(yaw * 3.14159265f / 32768.0f); }
static int drop_and_set_mario_action(struct MarioState *m, u32 action, int arg) {
    assert(arg == 0); actionChanges++; m->action = action; return 1;
}
static void mario_set_forward_vel(struct MarioState *m, float velocity) { m->forwardVel = velocity; }
static void cur_obj_play_sound_2(int sound) { assert(sound == SOUND_ACTION_HIT); sounds++; }
static struct { int buttonPressed; } controller, *gPlayer1Controller = &controller;
static struct { int isActive; } gWarpTransition;
static int sDelayedWarpOp;
static int get_dialog_id(void) { return -1; }
static int level_is_ending(void) { return 0; }
static int castle_secret_lobby_active(void) { return 0; }
/* PRODUCTION */
int main(void) {
    gCurrLevelNum = LEVEL_JRB; gCurrentArea = &area;
    gCurrAreaIndex = 2;
    for (gCurrActNum=1; gCurrActNum<=6; gCurrActNum++) {
        bhv_jrb_simpleflips_window_loop();
        assert(o->intangible == (gCurrActNum != 1));
        assert(!jrb_simpleflips_room_active());
    }
    gCurrActNum = 1;
    // Flooded or unavailable water keeps the warp locked, even inside the window.
    shipWater[6] = 2765;
    bhv_jrb_simpleflips_window_loop();
    assert(o->intangible);
    gEnvironmentRegions = NULL;
    bhv_jrb_simpleflips_window_loop();
    assert(o->intangible);
    gEnvironmentRegions = shipWater;
    shipWater[6] = 2760;
    bhv_jrb_simpleflips_window_loop();
    assert(!o->intangible);
    cheating=1; bhv_jrb_simpleflips_window_loop(); assert(o->intangible);
    cheating=0;
    shipWater[6] = -335;
    bhv_jrb_simpleflips_window_loop();
    assert(!o->intangible);
    gCurrAreaIndex = 1;
    bhv_jrb_simpleflips_window_loop();
    assert(o->intangible);
    gCurrAreaIndex = 2;
    gCurrActNum = 1; visited[0] = 1;
    bhv_jrb_simpleflips_window_loop();
    assert(o->intangible);
    gCurrSaveFileNum = 2;
    bhv_jrb_simpleflips_window_loop();
    assert(!o->intangible);
    gCurrSaveFileNum = 1; visited[0] = 0;
    bhv_jrb_simpleflips_sign_loop();
    assert(o->hidden && o->intangible && collisionLoads == 0);
    *o = (struct Object){0};
    gCurrAreaIndex = JRB_SIMPLEFLIPS_AREA;
    assert(jrb_simpleflips_room_active());
    cheating=1; bhv_jrb_simpleflips_sign_loop();
    assert(o->hidden && o->intangible && o->oInteractType==INTERACT_NONE);
    controller.buttonPressed = START_BUTTON;
    mario.health = 0x880;
    assert(pressed_pause());
    cheating=0; *o=(struct Object){0};
    controller.buttonPressed = START_BUTTON;
    for (int health=0; health<=0x880; health+=0x80) {
        mario.health = health;
        assert(!pressed_pause());
    }
    gCurrAreaIndex = 2; mario.action = 0; mario.health = 0x880;
    assert(pressed_pause());
    gCurrAreaIndex = JRB_SIMPLEFLIPS_AREA;
    bhv_jrb_simpleflips_sign_loop();
    assert(collisionLoads == 1 && o->oAction == SIMPLEFLIPS_SIGN);
    // Actual support/impact qualification is supplied at this boundary.
    groundPound = 1;
    bhv_jrb_simpleflips_sign_loop();
    assert(o->oAction == SIMPLEFLIPS_TOAD && o->model == MODEL_JRB_SIMPLEFLIPS_TOAD);
    assert(o->oInteractionSubtype == INT_SUBTYPE_NPC && o->oOpacity == 255);
    assert(collisionLoads == 1);
    mario.health = 0x880; mario.hurtCounter = 5; mario.healCounter = 20;
    mario.pos[2] = 105;
    o->oInteractStatus = INT_STATUS_INTERACTED;
    bhv_jrb_simpleflips_sign_loop();
    assert(o->oAction == SIMPLEFLIPS_DIALOG);
    for (int i=0; i<29; i++) bhv_jrb_simpleflips_sign_loop();
    assert(o->oAction == SIMPLEFLIPS_DIALOG && mario.health == 0x880);
    assert(statToadConversations == 0 && statHits == 0);
    bhv_jrb_simpleflips_sign_loop();
    assert(o->oAction == SIMPLEFLIPS_PUNCH && o->model == MODEL_JRB_SIMPLEFLIPS_PUNCH);
    assert(statToadConversations == 1);
    assert(o->header.gfx.animInfo.animYTrans == 189 && o->intangible);
    assert(fabsf(o->oPosZ - 5.0f) < 0.001f);
    for (o->oTimer=0; o->oTimer<SIMPLEFLIPS_PUNCH_CONTACT; o->oTimer++) {
        bhv_jrb_simpleflips_sign_loop();
        assert(mario.health == 0x880 && !jrb_simpleflips_is_game_over());
    }
    bhv_jrb_simpleflips_sign_loop();
    assert(o->oAction == SIMPLEFLIPS_FINISHED && mario.health == 0xFF);
    assert(mario.hurtCounter == 0 && mario.healCounter == 0);
    assert(mario.action == ACT_HARD_BACKWARD_AIR_KB);
    assert(mario.forwardVel == -60.0f && mario.vel[1] == 40.0f);
    assert(jrb_simpleflips_is_game_over());
    for (o->oTimer=0; o->oTimer<100; o->oTimer++) {
        bhv_jrb_simpleflips_sign_loop();
        assert(o->model == (o->oTimer < SIMPLEFLIPS_FRAMES_PER_SECOND/2 ? MODEL_JRB_SIMPLEFLIPS_PUNCH : MODEL_JRB_SIMPLEFLIPS_TOAD));
    }
    assert(o->header.gfx.animInfo.animYTrans == 0);
    assert(actionChanges == 1 && sounds == 1);
    assert(statHits == 1 && statHealthLost == 0x781 && statToadConversations == 1);
    // The native wave pose must face the punch direction at every heading,
    // including yaw wraparound, without turning again on subsequent updates.
    const s16 headings[] = {0, 0x4000, -32768, -0x4000, 12345, -30000};
    for (unsigned i=0; i<ARRAY_COUNT(headings); i++) {
        o->oAction = SIMPLEFLIPS_FINISHED; o->oMoveAngleYaw = headings[i];
        for (o->oTimer=0; o->oTimer<100; o->oTimer++) {
            bhv_jrb_simpleflips_sign_loop();
            s16 visibleYaw = o->oMoveAngleYaw;
            if (o->oTimer >= SIMPLEFLIPS_FRAMES_PER_SECOND/2) visibleYaw += SIMPLEFLIPS_TOAD_IDLE_YAW_OFFSET;
            assert(visibleYaw == headings[i]);
        }
    }
    struct GraphNodeSwitchCase head = {0};
    geo_switch_simpleflips_head(GEO_CONTEXT_RENDER, &head.node, NULL);
    assert(head.selectedCase == 1);
    jrb_simpleflips_reset();
    geo_switch_simpleflips_head(GEO_CONTEXT_RENDER, &head.node, NULL);
    assert(!jrb_simpleflips_is_game_over() && head.selectedCase == 0);
    musicStops = 0;
    musicFades = 0;
    groundPound = 0; *o = (struct Object){.behavior=bhvJrbSimpleFlipsSign, .oOpacity=255, .oInteractType=INTERACT_TEXT};
    // Every read is committed once; effects cannot precede the closing dialogue.
    for (int read=1; read<=128; read++) {
        mario.usedObj = o; mario.action = ACT_READING_SIGN;
        jrb_simpleflips_begin_sign_read(o);
        assert(sSimpleFlipsReadCount == read);
        int chosen = DIALOG_SIMPLEFLIPS_SIGN;
        for (unsigned i=0; i<ARRAY_COUNT(sSimpleFlipsMessages); i++)
            if (sSimpleFlipsMessages[i].read == read) chosen = sSimpleFlipsMessages[i].dialog;
        assert(o->oBehParams2ndByte == chosen);
        jrb_simpleflips_begin_sign_read(o); assert(sSimpleFlipsReadCount == read);
        bhv_jrb_simpleflips_sign_loop(); assert(sSimpleFlipsReadEffectsPending);
        if (read==26) assert(!sSimpleFlipsRoomRed);
        if (read==55) assert(!sSimpleFlipsRoomMusicAllowed);
        if (read==70) assert(!sSimpleFlipsMips);
        if (read==128) assert(!sSimpleFlipsFinalReadComplete);
        if (read==95) o->oTimer=1234; // A long-lived sign must still vanish for three seconds.
        mario.action = 0; bhv_jrb_simpleflips_sign_loop(); assert(!sSimpleFlipsReadEffectsPending);
        assert(sSimpleFlipsRoomRed == (read>=26 && read<128));
        assert(jrb_simpleflips_music_allowed(SEQ_PLAYER_SFX,SEQ_EVENT_POWERUP));
        assert(!jrb_simpleflips_music_allowed(SEQ_PLAYER_ENV,SEQ_LEVEL_PHYSICS_LAB));
        assert(!jrb_simpleflips_music_allowed(SEQ_PLAYER_LEVEL,SEQ_EVENT_POWERUP));
        assert(jrb_simpleflips_music_allowed(SEQ_PLAYER_LEVEL,SEQ_LEVEL_PHYSICS_LAB) == (read>=55 && read<128));
        if (read==70) {
            struct Object *sign = o; o = &mips; dialogs=0;
            o->oAction=0; o->oInteractStatus=INT_STATUS_INTERACTED;
            for (int i=0; i<3; i++) bhv_jrb_simpleflips_mips_loop();
            assert(o->oAction==2 && o->oInteractType==INTERACT_NONE && o->intangible);
            for (int i=0; i<10; i++) { o->oInteractStatus=INT_STATUS_INTERACTED; bhv_jrb_simpleflips_mips_loop(); }
            assert(dialogs==3); o=sign;
        }
        if (read==95) {
            assert(o->hidden && o->intangible && o->oAction==SIMPLEFLIPS_SIGN_HIDDEN);
            int before = collisionLoads;
            for (o->oTimer=0; o->oTimer<SIMPLEFLIPS_FRAMES_PER_SECOND*5; o->oTimer++) {
                bhv_jrb_simpleflips_sign_loop(); assert(o->hidden && collisionLoads==before);
            }
            bhv_jrb_simpleflips_sign_loop(); assert(!o->hidden && o->oOpacity==0);
            for (int i=1; i<=32; i++) {
                bhv_jrb_simpleflips_sign_loop(); assert(o->oOpacity==MIN(i*8,255));
            }
            assert(o->oAction==SIMPLEFLIPS_SIGN && o->oInteractType==INTERACT_TEXT);
        }
    }
    assert(spawnCount[0]==1 && spawnCount[1]==1 && spawnCount[2]==1);
    assert(musicStarts==1 && musicStops==1 && musicFades==1 && !sSimpleFlipsMips && mips.hidden);
    assert(o->oInteractType==INTERACT_NONE && sSimpleFlipsFinalReadComplete);
    jrb_simpleflips_begin_sign_read(o); assert(sSimpleFlipsReadCount==128 && !sSimpleFlipsReadEffectsPending);
    assert(pendingGameOvers==0);
    groundPound=1; bhv_jrb_simpleflips_sign_loop(); assert(crashes==1 && o->oAction==SIMPLEFLIPS_SIGN);
    jrb_simpleflips_reset(); assert(sSimpleFlipsReadCount==0 && !sSimpleFlipsFinalReadComplete);
    // Ground-pounding at different milestones clears whichever effects exist.
    const int milestones[] = {0, 26, 55, 70, 105};
    for (unsigned i = 0; i < ARRAY_COUNT(milestones); i++) {
        jrb_simpleflips_reset();
        *o = (struct Object){.behavior = bhvJrbSimpleFlipsSign,
                             .oInteractType = INTERACT_TEXT};
        sSimpleFlipsReadCount = milestones[i];
        sSimpleFlipsRoomRed = milestones[i] >= 26;
        sSimpleFlipsRoomMusicAllowed = milestones[i] >= 55;
        sSimpleFlipsMips = milestones[i] >= 70 ? &mips : NULL;
        mips.hidden = 0;
        int stops = musicStops;
        int fades = musicFades;
        bhv_jrb_simpleflips_sign_loop();
        assert(o->oAction == SIMPLEFLIPS_TOAD && o->model == MODEL_JRB_SIMPLEFLIPS_TOAD);
        assert(!sSimpleFlipsRoomRed && !sSimpleFlipsRoomMusicAllowed && !sSimpleFlipsMips);
        assert(mips.hidden == (milestones[i] >= 70));
        assert(!sSimpleFlipsFinalReadComplete && o->oInteractType == INTERACT_TEXT);
        assert(!jrb_simpleflips_music_allowed(SEQ_PLAYER_LEVEL, SEQ_LEVEL_PHYSICS_LAB));
        assert(jrb_simpleflips_music_allowed(SEQ_PLAYER_SFX, SEQ_EVENT_POWERUP));
        for (int frame = 0; frame < 10; frame++) {
            bhv_jrb_simpleflips_sign_loop();
        }
        assert(musicStops == stops + 1 && musicFades == fades + 1);
    }
    assert(pendingGameOvers==1);
    gCurrLevelNum = 1; assert(!jrb_simpleflips_room_active());
    gCurrLevelNum = LEVEL_JRB; gCurrentArea = NULL; assert(!jrb_simpleflips_room_active());
    return 0;
}
'''
        definitions = Source(ROOT / "src/game/jrb_simpleflips.h").macros(
            "JRB_SIMPLEFLIPS_.*|MODEL_JRB_SIMPLEFLIPS_.*")
        definitions += source.declaration("enum", "SimpleFlipsActions")
        definitions += source.macros("SIMPLEFLIPS_.*").replace("#define SIMPLEFLIPS_FRAMES_PER_SECOND 25", "")
        definitions += actions.declaration("enum", "MarioActionGroups")
        definitions += actions.declaration("enum", "MarioActionFlags")
        definitions += actions.macros("ACT_HARD_BACKWARD_AIR_KB|ACT_READING_SIGN|ACT_DEBUG_FREE_MOVE")
        names = ("jrb_simpleflips_room_active", "jrb_simpleflips_is_game_over",
                 "jrb_simpleflips_exit_locked",
                 "jrb_simpleflips_reset", "geo_switch_simpleflips_head",
                 "bhv_jrb_simpleflips_window_loop", "simpleflips_restore_black_room",
                 "simpleflips_transform_sign",
                 "simpleflips_start_punch", "simpleflips_punch_mario",
                 "jrb_simpleflips_music_allowed", "jrb_simpleflips_begin_sign_read",
                 "simpleflips_finish_sign_read",
                 "bhv_jrb_simpleflips_sign_loop",
                 "bhv_jrb_simpleflips_mips_loop")
        code = code.replace("/* DEFINITIONS */", definitions)
        code = code.replace("/* GLOBALS */", source.region(r"^static u8 sSimpleFlipsGameOver;", r"^s32 jrb_simpleflips_room_active"))
        code = code.replace("/* PRODUCTION */", "\n".join(source.function(name) for name in names)
                            + Source(ROOT / "src/game/level_update.c").function("pressed_pause"))
        with tempfile.TemporaryDirectory(prefix="simpleflips-test-") as directory:
            folder = Path(directory)
            (folder / "test.c").write_text(code)
            subprocess.run(["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=undefined", "-I", str(ROOT / "include"), str(folder / "test.c"), "-lm",
                            "-o", str(folder / "test")], check=True)
            subprocess.run([str(folder / "test")], check=True)

    def test_mips_dialog_releases_mario(self):
        self.check_npc_dialog(False)

    def test_motos_dialog_releases_mario(self):
        self.check_npc_dialog(True)

    def check_npc_dialog(self, motos):
        with tempfile.TemporaryDirectory(
                prefix="simpleflips-motos-dialog-" if motos else "simpleflips-mips-dialog-") as directory:
            folder = Path(directory)
            prepare_npc_dialog_fixture(folder, motos)
            subprocess.run(["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror",
                            *(["-DTEST_MOTOS"] if motos else []),
                            "-fsanitize=undefined", "-I", str(folder),
                            "-I", str(ROOT / "include"),
                            str(ROOT / "tests/simpleflips_mips_dialog_test.c"),
                            "-o", str(folder / "test")], check=True)
            subprocess.run([str(folder / "test")], check=True)

    def test_one_up_spawn_twirl(self):
        with tempfile.TemporaryDirectory(prefix="simpleflips-one-up-") as directory:
            folder = Path(directory)
            prepare_one_up_fixture(folder)
            subprocess.run(["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=undefined", "-I", str(folder),
                            str(ROOT / "tests/simpleflips_one_up_test.c"), "-lm",
                            "-o", str(folder / "test")], check=True)
            subprocess.run([str(folder / "test")], check=True)

    def test_milestone_text_layout(self):
        source = read_source(ROOT / "include/simpleflips_messages.inc.h")
        entries = re.findall(r'SIMPLEFLIPS_MESSAGE\((\d+), (".*")\)', source)
        self.assertEqual([int(read) for read, _ in entries], [
            3, 5, 10, 11, 15, 16, 17, 20, 21, 25, 26, 27, 30, 31, 32, 33, 34, 35,
            36, 40, 45, 46, 47, 50, 55, 60, 61, 64, 65, 66, 70, 71, 74, 75, 80, 81,
            85, 86, 87, 88, 92, 95, 96, 98, 99, 100, 101, 102, 105, 111, 112,
            113, 114, 115, 116, 117, 118, 119, 120, 123, 125, 126, 127, 128])
        widths = read_us_dialog_widths()
        self.assertEqual(len(widths), 256)
        chars = {}
        for line in read_source(ROOT / "charmap.txt").splitlines():
            match = re.match(r"'(.*?)' = (0x[\dA-Fa-f]+)", line)
            if match and len(match[1]) == 1:
                chars[match[1]] = int(match[2], 16)
        chars["'"] = 0x3E
        # Expand the shipped definitions so the fallback and milestone messages
        # are checked with the actual dialogue box settings they will use.
        preprocessed = subprocess.run(
            ["cc", "-E", "-P", "-x", "c", "-I", str(ROOT / "include"), "-"],
            input='#define _(text) text\n'
                  '#define DEFINE_DIALOG(id, unused, lines, left, top, text) '
                  'TEST_DIALOG(id, lines, left, top, text)\n'
                  f'#include "{ROOT / "text/simpleflips_dialogs.h"}"\n',
            text=True, capture_output=True, check=True).stdout
        dialogs = re.findall(
            r'TEST_DIALOG\((\w+), (\d+), (\d+), (\d+), ("(?:\\.|[^"\\])*")\)',
            preprocessed)
        signs = {name: (int(lines), int(left), int(top), json.loads(literal))
                 for name, lines, left, top, literal in dialogs
                 if name == "DIALOG_SIMPLEFLIPS_SIGN" or name.startswith("DIALOG_SIMPLEFLIPS_READ_")}
        self.assertEqual(set(signs), {"DIALOG_SIMPLEFLIPS_SIGN"}
                         | {f"DIALOG_SIMPLEFLIPS_READ_{read}" for read, _ in entries})
        for name, (capacity, left, top, text) in signs.items():
            self.assertEqual((capacity, left, top), (2, 30, 200), name)
            lines = text.split("\n")
            # Longer messages use the normal continuation page, within one read.
            self.assertLessEqual(len(lines), capacity * 2, name)
            for line in lines:
                self.assertLessEqual(sum(widths[chars[char]] for char in line), 120, (name, line))
        continuation_messages = {name for name, (capacity, _, _, text) in signs.items()
                                 if len(text.split("\n")) > capacity}
        self.assertEqual(continuation_messages, set())
        self.assertEqual(signs["DIALOG_SIMPLEFLIPS_SIGN"][3].replace("\n", " "),
                         "Shout-outs to SimpleFlips")
        self.assertEqual(signs["DIALOG_SIMPLEFLIPS_READ_17"][3].replace("\n", " "),
                         "Shout-outs to SimpleFlips ;)")

    def test_shipped_words_and_animation_channels(self):
        text = read_source(ROOT / "text/simpleflips_dialogs.h")
        speech = text.split('_("\\\n')[1].split('"))')[0]
        words = speech.replace("\\n\\\n", " ").strip()
        self.assertEqual(words, "Now You Done Messed Up I Will Count To One Hundred And When I End "
                         "I Will Count To One Hundred More Are You Ready One Two Oatmeal.")
        self.assertIn("DIALOG_SIMPLEFLIPS_TOAD, 1, 1,", text)
        rig = read_source(ROOT / "levels/jrb/simpleflips/geo.inc.c")
        self.assertEqual(rig.count("GEO_ANIMATED_PART("), 20)
        animation = read_source(ROOT / "assets/anims/anim_67.inc.c")
        indices = re.search(r"anim_67_indices\[\] = \{(.*?)\};", animation, re.S)[1]
        # Three translation channels followed by three per animated joint.
        self.assertEqual(len(re.findall(r"0x[0-9A-F]+", indices)) // 6 - 1, 20)
        subprocess.run(["python3", str(ROOT / "tools/generate_simpleflips_assets.py"),
                        "--check"], check=True)

    def test_dialog_table_preserves_out_of_order_entries(self):
        table = read_source(ROOT / "text/define_text.inc.c").split("// == courses ==")[0]
        # Motos is defined before the milestones, but its ID follows them. The
        # sentinel must not overwrite that entry after the last milestone.
        with tempfile.TemporaryDirectory(prefix="simpleflips-dialog-table-") as directory:
            folder = Path(directory)
            source = folder / "table.c"
            source.write_text(
                '#include <assert.h>\n#define game_options_secrets_blocked() 0\n#include <stddef.h>\n'
                'typedef unsigned char u8;\n'
                'struct DialogEntry { int unused, lines, left, top; const u8 *text; };\n'
                '#define _(text) 0xFF\n' + table +
                '\nint main(void) {\n'
                '    for (int i = 0; i < DIALOG_COUNT; i++) assert(seg2_dialog_table[i]);\n'
                '    assert(seg2_dialog_table[DIALOG_SIMPLEFLIPS_MOTOS]->lines == 3);\n'
                '    assert(seg2_dialog_table[DIALOG_COUNT] == NULL);\n'
                '    assert(sizeof(seg2_dialog_table) / sizeof(*seg2_dialog_table) == DIALOG_COUNT + 1);\n'
                '    return 0;\n}\n')
            executable = folder / "test"
            subprocess.run(["cc", "-std=gnu17", "-Wall", "-Wextra", "-Werror", "-Wno-trigraphs",
                            "-I", str(ROOT / "include"), "-I", str(ROOT / "text"),
                            "-I", str(ROOT / "text/us"), str(source), "-o", str(executable)],
                           check=True)
            subprocess.run([str(executable)], check=True)

    def test_motos_pages_and_animation_channels(self):
        text = read_source(ROOT / "text/simpleflips_dialogs.h")
        speech = re.search(r'DEFINE_DIALOG\(DIALOG_SIMPLEFLIPS_MOTOS, 1, 3, 30, 200,\s*_\((.*?)\)\)',
                           text, re.S)[1]
        lines = "".join(json.loads(literal) for literal in re.findall(
            r'"(?:\\.|[^"\\])*"', speech)).split("\n")
        expected = [
            "Mario! You found me!",
            "You got rid of your Cap of Anti-visibility.",
            "Who am I?",
            "I'm the AI, Mario!",
            "I was personalizing you, all along!",
            "I don't have much time. I must be quick!",
            "There are too many SO RETRO discoveries -",
            "No! I can feel it!",
            "It's the phantom of BETA hacks!",
            "It's taking me back to the LARP zone!",
            "Must... resist... LIMINAL...",
            "YEARRRGGGGGGGGGGGGHHHHHHHHHHHHHHHHHHHHHH",
        ]
        pages = [lines[i:i + 3] for i in range(0, len(lines), 3)]
        self.assertEqual(len(pages), len(expected))
        for index, (page, sentence) in enumerate(zip(pages, expected)):
            joiner = "" if index == 11 else " "
            self.assertEqual(joiner.join(line for line in page if line), sentence)
        widths = read_us_dialog_widths()
        chars = {}
        for line in read_source(ROOT / "charmap.txt").splitlines():
            match = re.match(r"'(.*?)' = (0x[\dA-Fa-f]+)", line)
            if match and len(match[1]) == 1:
                chars[match[1]] = int(match[2], 16)
        chars["'"] = 0x3E
        for line in lines:
            self.assertLessEqual(sum(widths[chars[char]] for char in line), 120, line)

        self.assertEqual(len(pages[-1]), 2)
        for line in pages[-1]:
            self.assertEqual(sum(widths[chars[char]] for char in line), 120)

        rig = read_source(ROOT / "actors/motos/geo.inc.c")
        self.assertEqual(rig.count("GEO_ANIMATED_PART("), 36)
        animation = read_source(ROOT / "actors/motos/anims/anim.inc.c")
        records = re.findall(r"AnimeRecord (\w+)=\{(.*?)\};", animation, re.S)
        self.assertEqual(len(records), 10)
        for name, record in records:
            fields = re.sub(r"/\*.*?\*/", "", record, flags=re.S).split(",")
            self.assertEqual(int(fields[5]), 36, name)
            samples = re.search(r"\b" + fields[6].strip() + r"\[\]=\{(.*?)\};",
                                animation, re.S)[1]
            indices = re.search(r"\b" + fields[7].strip() + r"\[\]=\s*\{(.*?)\};",
                                animation, re.S)[1]
            indices = re.sub(r"/\*.*?\*/", "", indices, flags=re.S)
            tracks = [int(number) for number in re.findall(r"-?\d+", indices)]
            self.assertEqual(len(tracks), 37 * 6, name)
            sample_count = len(re.findall(r"-?\d+", samples))
            for count, start in zip(tracks[::2], tracks[1::2]):
                self.assertGreater(count, 0, name)
                self.assertLessEqual(start + count, sample_count, name)


if __name__ == "__main__":
    unittest.main()
