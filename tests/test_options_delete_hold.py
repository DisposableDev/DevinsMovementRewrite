#!/usr/bin/env python3
"""Execute the shipped erase prompt with controlled cursor/input/drawing services."""

from pathlib import Path
import os
import subprocess
import tempfile
import unittest

from regression_support import Source
from test_audit_state_fixes import compile_and_run
from run_regressions import prepare

ROOT = Path(__file__).resolve().parents[1]
PRELUDE = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
typedef int16_t s16; typedef int32_t s32; typedef uint32_t u32;
#define TRUE 1
#define FALSE 0
#define A_BUTTON 0x8000
#define B_BUTTON 0x4000
#define START_BUTTON 0x1000
#define MENU_ERASE_HOVER_YES 1
#define MENU_ERASE_HOVER_NO 2
#define MENU_ERASE_HOVER_NONE 0
#define MENU_BUTTON_ERASE 10
#define MENU_BUTTON_ERASE_MIN 11
#define ERASE_PHASE_MARIO_ERASED 2
#define ERASE_PHASE_MAIN 0
#define MENU_BUTTON_STATE_ZOOM_OUT 1
#define MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE 0
#define SOUND_MARIO_WAAAOOOW 0
#define SOUND_MENU_CLICK_FILE_SELECT 1
#define SOUND_MENU_CAMERA_BUZZ 2
#define LANGUAGE_ARRAY(x) (x)
#define gSPDisplayList(...) ((void)0)
#define gDPSetEnvColor(a,r,g,b,c) ((void)(r),(void)(g),(void)(b),(void)(c))
static struct { s32 buttonDown, buttonPressed; } pad, *gPlayer1Controller = &pad;
static struct Button {
    s32 oMenuButtonActionPhase, oMenuButtonState;
    struct { struct { void *sharedChild; } gfx; } header;
} buttons[20], *sMainMenuButtons[20];
static void *gLoadedGraphNodes[1];
static u32 gGlobalTimer;
static float sCursorPos[2];
static s32 sSelectedFileIndex, sEraseYesNoHoverState, sCursorClickingTimer;
static s32 sFadeOutText, sMainMenuTimer, sTextBaseAlpha, sYesNoColor[2];
static s32 erased, erasedFile;
static const char textYes[] = "YES", textNo[] = "NO";
static float sins(s16 x) { (void)x; return 0; }
static void *gGlobalSoundSource;
static void play_sound(s32 sound, void *source) { (void)sound; (void)source; }
static void queue_rumble_data(s32 strength, s32 duration) { (void)strength; (void)duration; }
static s32 save_file_try_erase(s32 file) { erased++; erasedFile = file; return TRUE; }
static void print_generic_string(s32 x, s32 y, const char *s) { (void)x; (void)y; (void)s; }
static void player_stats_menu_text(s32 x, s32 y, const char *s) {
    (void)x; (void)y; assert(strstr(s, "Hold A on YES") != NULL);
}
'''
CHECKS = r'''
static void begin(void) {
    memset(&sEraseHold, 0, sizeof(sEraseHold));
    memset(buttons, 0, sizeof(buttons));
    for (s32 i = 0; i < 20; i++) { sMainMenuButtons[i] = &buttons[i]; }
    sCursorPos[0] = -10; sCursorPos[1] = 80;
    gGlobalTimer = 0; sSelectedFileIndex = 0;
    erased = 0; sCursorClickingTimer = 2;
    pad.buttonDown = pad.buttonPressed = 0;
}
static void step(s32 held, s32 pressed) {
    pad.buttonDown = held; pad.buttonPressed = pressed;
    gGlobalTimer++;
    print_erase_menu_prompt(90, 190);
}
int main(void) {
    begin();
    for (s32 i = 0; i < 200; i++) { step(A_BUTTON, i == 0 ? A_BUTTON : 0); }
    assert(erased == 0);
    step(0, 0);
    for (s32 i = 0; i < ERASE_HOLD_FRAMES; i++) {
        step(A_BUTTON, i == 0 ? A_BUTTON : 0);
        print_erase_menu_prompt(90, 190);
        assert(erased == 0);
    }
    step(A_BUTTON, 0);
    assert(erased == 1 && erasedFile == 0);
    assert(buttons[MENU_BUTTON_ERASE].oMenuButtonActionPhase == ERASE_PHASE_MARIO_ERASED);
    print_erase_menu_prompt(90, 190);
    assert(erased == 1);

    begin(); step(0, 0);
    for (s32 i = 0; i < ERASE_HOLD_FRAMES - 1; i++) { step(A_BUTTON, 0); }
    step(0, 0);
    step(A_BUTTON, A_BUTTON);
    assert(erased == 0);
    sSelectedFileIndex = 1;
    for (s32 i = 0; i < 200; i++) { step(A_BUTTON, 0); }
    assert(erased == 0);
    step(0, 0);
    for (s32 i = 0; i <= ERASE_HOLD_FRAMES; i++) { step(A_BUTTON, 0); }
    assert(erased == 1 && erasedFile == 1);

    begin(); step(0, 0); step(A_BUTTON, A_BUTTON);
    sCursorPos[0] = 40; step(A_BUTTON, 0); // Leaving YES resets consent.
    sCursorPos[0] = -10;
    for (s32 i = 0; i < 200; i++) { step(A_BUTTON, 0); }
    assert(erased == 0);
    begin(); step(0, 0); step(A_BUTTON, A_BUTTON);
    step(A_BUTTON | B_BUTTON, B_BUTTON);
    assert(erased == 0);
    assert(buttons[MENU_BUTTON_ERASE].oMenuButtonActionPhase == ERASE_PHASE_MAIN);
    begin(); step(0, 0); step(A_BUTTON, A_BUTTON);
    gGlobalTimer += 100;
    for (s32 i = 0; i < 200; i++) { step(A_BUTTON, 0); }
    assert(erased == 0);
    begin(); gGlobalTimer = 0xFFFFFFF0u; step(0, 0);
    for (s32 i = 0; i <= ERASE_HOLD_FRAMES; i++) { step(A_BUTTON, 0); }
    assert(erased == 1);
    return 0;
}
'''


class DeleteHoldTests(unittest.TestCase):
    def test_failed_erase_keeps_prompt_and_retries(self):
        with tempfile.TemporaryDirectory(prefix="erase-menu-write-retry-") as temporary:
            folder = Path(temporary)
            prepare(ROOT, "character_save", folder, {})
            definitions = folder / "character_save_definitions.inc"
            constants = definitions.read_text()
            self.assertIn("#define player_stats_erase(...) ((void) 0)", constants)
            definitions.write_text(constants.replace(
                "#define player_stats_erase(...) ((void) 0)",
                "#define player_stats_erase(...) (++statsErases)"))
            original = (ROOT / "tests/character_save_test.c").read_text().replace(
                "u16 buttonDown;", "u16 buttonDown, buttonPressed;")
            self.assertIn("static void game_timing_erase(s32 file) {", original)
            original = original.replace("static void game_timing_erase(s32 file) {",
                                        "static void game_timing_erase(s32 file) { timingErases++;")
            (folder / "character_erase_world.c").write_text(original)
            source = Source(ROOT / "src/menu/file_select.c")
            header = Source(ROOT / "src/menu/file_select.h")
            declarations = header.declaration("enum", "MenuEraseButtonHoverStates")
            declarations += header.declaration("enum", "EraseMenuActionPhase")
            declarations += Source(ROOT / "include/model_ids.h").macros(
                "MODEL_LEVEL_GEOMETRY_0A|MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE")
            production = source.region(r"^#define CURSOR_X", r"^void print_erase_menu_prompt")
            production += source.function("print_erase_menu_prompt")
            code = r'''
#include <assert.h>
static int statsErases, timingErases;
#define main inherited_character_main
#include "character_erase_world.c"
#undef main
#define B_BUTTON 0x4000
#define SOUND_MARIO_WAAAOOOW 99
#define LANGUAGE_ARRAY(x) (x)
#define gSPDisplayList(...) ((void)0)
#define gDPSetEnvColor(a,r,g,b,c) ((void)(r),(void)(g),(void)(b),(void)(c))
static u32 gGlobalTimer;
static f32 sCursorPos[2];
static s32 sEraseYesNoHoverState,sCursorClickingTimer,sTextBaseAlpha;
static u8 sYesNoColor[2];
static const u8 textYes[]="YES",textNo[]="NO";
static f32 sins(s16 angle) {(void)angle;return 0;}
static void print_generic_string(s32 x,s32 y,const u8 *text) {(void)x;(void)y;(void)text;}
static void player_stats_menu_text(s32 x,s32 y,const char *text) {(void)x;(void)y;(void)text;}
''' + declarations + production + r'''
static void erase_step(u16 held) {
    sController.buttonDown=held;sController.buttonPressed=0;
    gGlobalTimer++;print_erase_menu_prompt(90,190);
}
int main(void) {
    static struct Object buttons[MENU_BUTTON_OPTION_MAX];
    static struct GraphNode savedModel,emptyModel;
    begin();
    for(s32 i=0;i<MENU_BUTTON_OPTION_MAX;i++) {
        sMainMenuButtons[i]=&buttons[i];buttons[i].header.gfx.sharedChild=&savedModel;
    }
    gLoadedGraphNodes[MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE]=&emptyModel;
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS|SAVE_FLAG_HAVE_KEY_1);
    save_file_set_star_flags(SAVE_FILE_A,COURSE_NUM_TO_INDEX(COURSE_BOB),1);
    save_file_do_save(SAVE_FILE_A);
    game_options_select_file(SAVE_FILE_A);
    assert(game_options_set(GO_MASTER_VOLUME,30) && game_options_save(SAVE_FILE_A));
    gCurrSaveFileNum=SAVE_FILE_B+1;
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS|SAVE_FLAG_HAVE_KEY_2);
    save_file_do_save(SAVE_FILE_B);
    struct SaveBuffer before=gSaveBuffer;
    struct SaveFile otherBefore[2];memcpy(otherBefore,sSaveStorage.files[SAVE_FILE_B],sizeof(otherBefore));
    u8 optionsBefore[sizeof(sOptionsStorage)];memcpy(optionsBefore,sOptionsStorage,sizeof(optionsBefore));
    u8 modifiedBefore=sModifiedFiles,failedBefore=sFailedSaveFiles,unreadBefore=sUnreadFiles;
    s32 statsBefore=statsErases,timingBefore=timingErases;
    sSelectedFileIndex=SAVE_FILE_A;
    buttons[MENU_BUTTON_ERASE].oMenuButtonActionPhase=ERASE_PHASE_PROMPT;
    sCursorPos[0]=-10;sCursorPos[1]=80;sFadeOutText=FALSE;
    erase_step(0);
    sBaseWriteFailureMask=1u<<SAVE_FILE_A;
    for(s32 i=0;i<=ERASE_HOLD_FRAMES;i++) {erase_step(A_BUTTON);}
    assert(buttons[MENU_BUTTON_ERASE].oMenuButtonActionPhase==ERASE_PHASE_PROMPT);
    assert(buttons[SAVE_FILE_A].header.gfx.sharedChild==&savedModel);
    assert(buttons[MENU_BUTTON_ERASE_FILE_A].header.gfx.sharedChild==&savedModel);
    assert(sLastMenuSound==SOUND_MENU_CAMERA_BUZZ && !sFadeOutText);
    assert(save_file_exists(SAVE_FILE_A));
    assert(!memcmp(&before,&gSaveBuffer,sizeof(before)));
    assert(sModifiedFiles==modifiedBefore && sFailedSaveFiles==failedBefore && sUnreadFiles==unreadBefore);
    assert(statsErases==statsBefore && timingErases==timingBefore);
    assert(!memcmp(optionsBefore,sOptionsStorage,sizeof(optionsBefore)));
    assert(game_options_get_file(SAVE_FILE_A,GO_MASTER_VOLUME)==30);
    // Keeping A held cannot grant a fresh retry; release then confirm in the same prompt.
    sBaseWriteFailureMask=0;
    erase_step(A_BUTTON);
    assert(buttons[MENU_BUTTON_ERASE].oMenuButtonActionPhase==ERASE_PHASE_PROMPT);
    erase_step(0);
    for(s32 i=0;i<=ERASE_HOLD_FRAMES;i++) {erase_step(A_BUTTON);}
    assert(buttons[MENU_BUTTON_ERASE].oMenuButtonActionPhase==ERASE_PHASE_MARIO_ERASED);
    assert(buttons[SAVE_FILE_A].header.gfx.sharedChild==&emptyModel);
    assert(statsErases==statsBefore+1 && timingErases==timingBefore+1);
    save_file_load_all();
    assert(!save_file_exists(SAVE_FILE_A));
    assert(!(gSaveBuffer.files[SAVE_FILE_A][0].flags&SAVE_FLAG_HAVE_KEY_1));
    assert(!gSaveBuffer.files[SAVE_FILE_A][0].courseStars[0]);
    assert(!memcmp(otherBefore,sSaveStorage.files[SAVE_FILE_B],sizeof(otherBefore)));
    assert(game_options_get_file(SAVE_FILE_A,GO_MASTER_VOLUME)==100);
    // An earned unlock waiting on occupied C survives both rejection and accepted deletion.
    gCurrSaveFileNum=SAVE_FILE_C+1;
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS|SAVE_FLAG_HE_THICC_PENDING|SAVE_FLAG_HAVE_KEY_2);
    save_file_do_save(SAVE_FILE_C);
    struct SaveFile cBefore[2];memcpy(cBefore,gSaveBuffer.files[SAVE_FILE_C],sizeof(cBefore));
    sBaseWriteFailureMask=1u<<SAVE_FILE_C;
    assert(!save_file_try_erase(SAVE_FILE_C));
    assert(!memcmp(cBefore,gSaveBuffer.files[SAVE_FILE_C],sizeof(cBefore)));
    sBaseWriteFailureMask=0;
    assert(save_file_try_erase(SAVE_FILE_C));
    save_file_load_all();
    assert(!save_file_exists(SAVE_FILE_C) && save_file_he_thicc_pending());
    return 0;
}
'''
            compile_and_run(folder, code, stats=False)

    def test_erase_confirmation(self):
        source = Source(ROOT / "src/menu/file_select.c")
        production = source.region(r"^#define CURSOR_X", r"^void print_erase_menu_prompt")
        production += source.function("print_erase_menu_prompt")
        with tempfile.TemporaryDirectory() as temporary:
            directory = Path(temporary)
            fixture = directory / "fixture.c"
            fixture.write_text(PRELUDE + production + CHECKS)
            for region in ([], ["-DVERSION_EU"]):
                executable = directory / "erase-test"
                subprocess.run([
                    "cc", "-std=c11", "-Wall", "-Wextra", "-Werror", *region,
                    "-fsanitize=address,undefined", "-fno-sanitize-recover=all", "-fno-omit-frame-pointer",
                    str(fixture), "-o", str(executable),
                ], check=True, timeout=30)
                subprocess.run([str(executable)], check=True,
                               env=dict(os.environ, ASAN_OPTIONS="detect_leaks=0"), timeout=10)


if __name__ == "__main__":
    unittest.main()
