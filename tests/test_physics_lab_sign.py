"""Host checks for sign eligibility, entrance text and dialogue pagination."""
import ast
import math
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class PhysicsLabSignTests(unittest.TestCase):
    def test_hidden_sign_reveal_boundary_and_reentry(self):
        source = Source(ROOT / 'src/game/behaviors/message_panel.inc.c')
        fixture = r'''
#include <assert.h>
struct Object { float oDistanceToMario; int oInteractType; } object, *o = &object;
#define INTERACT_TEXT 1
static int hidden, panel_updates;
void bhv_message_panel_loop(void) { panel_updates++; }
void cur_obj_hide(void) { hidden = 1; }
void cur_obj_unhide(void) { hidden = 0; }
''' + source.macros('PHYSICS_LAB_SIGN_REVEAL_DISTANCE') + source.function(
            'bhv_physics_lab_hidden_sign_loop') + r'''
int main(void) {
    const float distances[] = {1000, 150.001f, 150, 149.999f, 0, 151, 150};
    for (unsigned i = 0; i < sizeof(distances) / sizeof(distances[0]); i++) {
        object.oDistanceToMario = distances[i];
        bhv_physics_lab_hidden_sign_loop();
        assert(hidden == (distances[i] > 150));
        assert(object.oInteractType == (distances[i] <= 150 ? INTERACT_TEXT : 0));
        assert(panel_updates == (int)i + 1);
    }
}
'''
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            (folder / 'test.c').write_text(fixture)
            subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                            str(folder / 'test.c'), '-o', str(folder / 'test')], check=True)
            subprocess.run([str(folder / 'test')], check=True)

    def test_hidden_sign_reading_camera_clearance(self):
        script = (ROOT / 'levels/physics_lab/script.c').read_text()
        match = re.search(r'OBJECT\(MODEL_WOODEN_SIGNPOST,\s*([^\n]+),\s*'
                          r'bhvPhysicsLabHiddenSign\)', script)
        x, y, z, _, yaw, _, _ = map(int, match[1].split(','))
        facing = (math.sin(math.radians(yaw)), math.cos(math.radians(yaw)))
        interaction = Source(ROOT / 'src/game/interaction.c').region(
            r'^u32 check_read_sign\(', r'^u32 check_npc_talk\(')
        reading_distance = float(re.search(r'targetX = .*?\+ ([\d.]+)f', interaction)[1])
        camera = Source(ROOT / 'src/game/camera.c').function('update_c_up')
        camera_distance = float(re.search(
            r'focus_on_mario\(focus, pos, [\d.]+f, [\d.]+f, ([\d.]+)f', camera)[1])
        # Shaft interior: X=10100..10700, Z=4800..5700. Mario's radius is 50.
        mario_x = x + reading_distance * facing[0]
        mario_z = z + reading_distance * facing[1]
        self.assertTrue(10150 <= mario_x <= 10650)
        self.assertTrue(4850 <= mario_z <= 5650)
        self.assertEqual(y, -2000)
        for offset in (0, camera_distance):
            camera_x = mario_x + offset * facing[0]
            camera_z = mario_z + offset * facing[1]
            self.assertTrue(10100 < camera_x < 10700)
            self.assertTrue(4800 < camera_z < 5700)
        # Ordinary sinking still leaves the 105-unit reading stance within reveal range.
        self.assertLessEqual(math.hypot(reading_distance, 60), 150)

    def test_underwater_caps_ground_contact_and_dry_reading(self):
        interaction = Source(ROOT / 'src/game/interaction.c')
        constants = Source(ROOT / 'include/sm64.h')
        functions = '\n'.join((
            Source(ROOT / 'src/game/mario.c').function('mario_has_current_metal_cap'),
            Source(ROOT / 'src/game/mario.c').function('mario_has_current_vanish_cap'),
            Source(ROOT / 'src/game/mario.c').function('mario_has_land_movement_underwater'),
            Source(ROOT / 'src/game/mario_step.c').function('mario_is_on_floor'),
            interaction.region(r'^u32 mario_can_talk\(', r'^#define SIGN_RANGE'),
            interaction.region(r'^u32 check_read_sign\(', r'^u32 check_npc_talk\(')))
        actions = {'ACT_IDLE', 'ACT_WATER_IDLE'} | set(
            re.findall(r'\bACT_(?!FLAG_)[A-Z_]+\b', functions))
        definitions = '\n'.join((
            constants.declaration('enum', 'MarioInput'),
            constants.declaration('enum', 'MarioFlags'),
            constants.declaration('enum', 'MarioActionFlags'),
            constants.macros('|'.join(sorted(actions))),
            Source(ROOT / 'include/config/config_movement.h').macros(
                'MARIO_STRONG_STICK_THRESHOLD'),
            interaction.macros('SIGN_RANGE')))
        fixture = r'''
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mario_animation_ids.h"
typedef int16_t s16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
#define TRUE 1
#define FALSE 0
#define DEGREES(angle) ((angle) * 65536 / 360)
#define absf fabsf
#define ORANGE_NUMBER_B 1
#define MODEL_NUMBER 1
struct Controller { f32 stickMag; } controller;
struct Object {
    struct { struct { struct { s16 animID; } animInfo; } gfx; } header;
    s32 oMoveAngleYaw;
    f32 oPosX, oPosZ, oHomeX, oHomeZ;
    s16 oMarioReadingSignDYaw;
    f32 oMarioReadingSignDPosX, oMarioReadingSignDPosZ;
    const void *behavior;
} sign, mario, indicator;
struct MarioState {
    u32 flags, action, input;
    f32 pos[3], floorHeight;
    s16 waterLevel, faceAngle[3];
    void *floor;
    struct Controller *controller;
    struct Object *marioObj, *interactObj, *usedObj;
} state;
enum { GO_METAL_CAP, GO_VANISH_CAP };
static int game_options_get(int option) {
    assert(option == GO_METAL_CAP || option == GO_VANISH_CAP);
    return 0;
}
static const int bhvSignOnWall[] = {0}, bhvOrangeNumber[] = {0};
static int facing = TRUE, readings, prompts;
static f32 sins(s16 angle) { return sinf(angle * 0.0000958738f); }
static f32 coss(s16 angle) { return cosf(angle * 0.0000958738f); }
#ifdef EASIER_DIALOG_TRIGGER
static s32 abs_angle_diff(s16 a, s16 b) { return abs((s16)(a - b)); }
static s16 mario_obj_angle_to_object(struct MarioState *m, struct Object *obj) {
    (void) obj;
    return m->faceAngle[1];
}
#endif
static int object_facing_mario(struct MarioState *m, struct Object *obj, int range) {
    (void) m;
    (void) obj;
    (void) range;
    return facing;
}
#ifdef DIALOG_INDICATOR
static void *segmented_to_virtual(const void *behavior) { return (void *) behavior; }
static struct Object *spawn_object_relative(int number, int x, int y, int z,
        struct Object *obj, int model, const void *behavior) {
    (void) number;
    (void) x;
    (void) y;
    (void) z;
    (void) obj;
    (void) model;
    (void) behavior;
    prompts++;
    return &indicator;
}
#endif
static void jrb_simpleflips_begin_sign_read(struct Object *obj) {
    assert(obj == &sign);
    readings++;
}
static u32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    assert(arg == 0);
    m->action = action;
    return TRUE;
}
''' + definitions + functions + r'''
static void reset(u32 caps, f32 height, s16 water) {
    memset(&state, 0, sizeof(state));
    state.flags = caps;
    state.action = ACT_IDLE;
    state.input = INPUT_B_PRESSED;
    state.controller = &controller;
    state.marioObj = &mario;
    state.pos[1] = height;
    state.floorHeight = -2000.0f;
    state.waterLevel = water;
    state.floor = &sign;
    state.faceAngle[1] = (s16) 0x8000;
    controller.stickMag = 0;
    facing = TRUE;
    readings = prompts = 0;
}
static void assert_rejected(void) {
    u32 action = state.action;
    assert(!check_read_sign(&state, &sign));
    assert(state.action == action && state.usedObj == NULL);
    assert(readings == 0 && prompts == 0);
}
int main(void) {
    const u32 caps[] = {MARIO_METAL_CAP, MARIO_VANISH_CAP,
                       MARIO_METAL_CAP | MARIO_VANISH_CAP};
    sign.behavior = bhvSignOnWall;
    (void) bhvOrangeNumber;
    for (unsigned i = 0; i < sizeof(caps) / sizeof(caps[0]); i++) {
        for (int offset = -4; offset <= 4; offset++) {
            reset(caps[i], -2000.0f + offset, -40);
            assert(check_read_sign(&state, &sign));
            assert(state.action == ACT_READING_SIGN && readings == 1);
            assert(state.usedObj == &sign && state.interactObj == &sign);
            assert(fabsf(mario.oMarioReadingSignDPosZ - 105.0f) < 0.001f);
        }
        reset(caps[i], -1995.9f, -40);
        assert_rejected();
        reset(caps[i], -2004.1f, -40);
        assert_rejected();
        reset(caps[i], -2000, -40);
        state.floor = NULL;
        assert_rejected();
        reset(caps[i], -2000, -40);
        state.action |= ACT_FLAG_AIR;
        assert_rejected();
        reset(caps[i], -2000, -40);
        state.input = 0;
        assert(!check_read_sign(&state, &sign) && readings == 0);
        state.input = INPUT_B_PRESSED;
        assert(check_read_sign(&state, &sign) && readings == 1);
    }
    reset(0, -2000, -40);
    assert_rejected();
    reset(MARIO_WING_CAP, -2000, -40);
    assert_rejected();
#ifdef EASIER_DIALOG_TRIGGER
    reset(0, -1000, -40);
    state.action = ACT_WATER_IDLE;
    assert_rejected();
#endif
    reset(0, -2000, -2001);
    assert(check_read_sign(&state, &sign));
    reset(0, -2000, -2000);
    assert(check_read_sign(&state, &sign));
    reset(0, -2000, -1999);
    assert_rejected();
    reset(MARIO_METAL_CAP, -2000, -40);
    controller.stickMag = MARIO_STRONG_STICK_THRESHOLD;
    assert_rejected();
    reset(MARIO_VANISH_CAP, -2000, -40);
    state.faceAngle[1] = 0;
    assert_rejected();
    reset(MARIO_METAL_CAP, -2000, -40);
    facing = FALSE;
    assert_rejected();
}
'''
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            (folder / 'test.c').write_text(fixture)
            for switches in ([], ['-DEASIER_DIALOG_TRIGGER', '-DDIALOG_INDICATOR']):
                subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                                '-I', str(ROOT / 'include'), *switches,
                                str(folder / 'test.c'), '-lm', '-o', str(folder / 'test')],
                               check=True)
                subprocess.run([str(folder / 'test')], check=True)

    def test_completion_boundary_and_debug_overrides(self):
        function = Source(ROOT / 'src/game/behaviors/message_panel.inc.c').function(
            'bhv_physics_lab_welcome_sign_init')
        fixture = '''
#include <assert.h>
typedef int s32;
#define TRUE 1
#define FALSE 0
#define COURSE_NUM_TO_INDEX(n) ((n)-1)
#define COURSE_MIN 1
#define COURSE_MAX 25
#define CASTLE_ROOF_COMPLETION_STARS 120
#define DIALOG_PHYSICS_LAB_WELCOME_COMPLETE 1
#define DIALOG_PHYSICS_LAB_WELCOME_EARLY 2
struct Object { int oBehParams2ndByte; } object, *o=&object;
int gDebugLevelSelect, gCurrSaveFileNum=3, stars, secretsBlocked;
int game_options_secrets_blocked(void) { return gDebugLevelSelect || secretsBlocked; }
int save_file_get_total_star_count(int file, int min, int max) {
    assert(file==2 && min==0 && max==24); return stars;
}
''' + function + '''
int main(void) {
    for (stars=0; stars<=121; stars++) {
        for (secretsBlocked=0; secretsBlocked<=1; secretsBlocked++) {
          for (gDebugLevelSelect=0; gDebugLevelSelect<=1; gDebugLevelSelect++) {
            bhv_physics_lab_welcome_sign_init();
#if defined(UNLOCK_ALL) || defined(COMPLETE_SAVE_FILE)
            assert(object.oBehParams2ndByte==2);
#else
            assert(object.oBehParams2ndByte==((stars>=120 && !gDebugLevelSelect && !secretsBlocked)?1:2));
#endif
          }
        }
    }
}
'''
        with tempfile.TemporaryDirectory() as directory:
            folder = Path(directory)
            (folder/'test.c').write_text(fixture)
            for override in (None, 'UNLOCK_ALL', 'COMPLETE_SAVE_FILE'):
                command = ['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                           str(folder/'test.c'), '-o', str(folder/'test')]
                if override:
                    command.append('-D'+override)
                subprocess.run(command, check=True)
                subprocess.run([str(folder/'test')], check=True)

    def test_sentence_pages_and_font_width(self):
        expected = {
            'COMPLETE': ["Welcome to the Physics Lab!", "It's a secret to everybody.",
                "If you've made it here, you've finished the game.",
                "This level was made entirely by GPT-6.1 Sol.", "(I'm sure it's hard to believe.)",
                "Since you've made it this far, I have one last mission...", "Have fun!"],
            'EARLY': ["Welcome to the Physics Lab!", "Of course, you cheated to get here.",
                "(Or you're a developer.)", "This level was made by GPT-6.1 Sol.",
                "There's a legitimate way to get here, of course.",
                "If you find it, don't be a sneaky snitch!",
                "I'd put in developer comments, but it'd be way too long.",
                "Be sure to try and break the game.", "One day work will end.", "But play is forever!"]}
        expected['INTRUDER'] = ["Sorry, Mario. I can't -", "Did you just break into my house?",
            "Do you have any idea how much that cost?", "Oh man, my landlord is going to KILL me!",
            "And my beautiful wife, too!", "No, she's not just a painting, shut up.",
            "This is worse than when Bowser...", "Never mind! I gotta get that door fixed up!",
            "And you're supposed to be the hero?", "You're more like a ZERO!"]
        table = (ROOT/'src/game/ingame_menu.c').read_text().split(
            'u8 gDialogCharWidths[256] = {')[1].split('\n};')[0]
        table = re.sub(r'#ifdef VERSION_EU\n.*?#else\n(.*?)#endif', r'\1', table, flags=re.S)
        widths = [int(n) for n in re.findall(r'\b\d+\b', re.sub(r'//[^\n]*', '', table))]
        chars = {}
        for line in (ROOT/'charmap.txt').read_text().splitlines():
            match = re.match(r"'(.*?)' = (0x[\dA-Fa-f]+)", line)
            if match and len(match[1])==1:
                chars[match[1]] = int(match[2],16)
        chars["'"] = 0x3E
        for name, pages in expected.items():
            source = (ROOT/'text/simpleflips_dialogs.h').read_text().split(
                'DEFINE_DIALOG('+('DIALOG_CASTLE_BOARD_INTRUDER' if name=='INTRUDER' else 'DIALOG_PHYSICS_LAB_WELCOME_'+name)+',',1)[1].split('DEFINE_DIALOG',1)[0]
            self.assertRegex(source, r'^\s*1, 4, 30, 200,')
            text = ''.join(ast.literal_eval(s) for s in re.findall(r'"(?:\\.|[^"\\])*"',source))
            lines = text.split('\n')
            self.assertEqual([' '.join(lines[i:i+4]).strip() for i in range(0,len(lines),4)], pages)
            for line in lines:
                self.assertLessEqual(sum(widths[chars[c]] for c in line),120,line)


if __name__ == '__main__':
    unittest.main()
