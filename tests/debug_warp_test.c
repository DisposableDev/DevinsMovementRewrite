/* Production routing with authored script entrance metadata; loading and rendering are supplied. */
#include <stdint.h>
#include "test_support.h"
typedef int16_t s16;
typedef int8_t s8;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef uint8_t u8;
typedef uintptr_t BehaviorScript;
typedef double f64;
typedef float f32;
#define TRUE 1
#define FALSE 0
#define PUPPYPRINT_DEBUG
#define ARRAY_COUNT(a) ((s32) (sizeof(a) / sizeof((a)[0])))
#include "debug_warp_definitions.inc"
#define LEVEL_MIN 1
#define LEVEL_MAX (LEVEL_COUNT - 1)
#define COURSE_NONE 0
#define MODEL_BOWSER_KEY 1
#define MODEL_STAR 2
#define HUD_DISPLAY_DEFAULT 1
#define HUD_DISPLAY_NONE 0
#define ACT_IDLE 1
#define ACT_INTRO_CUTSCENE 2
#define ACT_UNINITIALIZED 0
#define TIMER_CONTROL_HIDE 0
#define WARP_TRANSITION_FADE_FROM_COLOR 0
#define WARP_TRANSITION_FADE_FROM_STAR 1
#define SEQ_PLAYER_SFX 0
#define SEQ_PLAYER_ENV 1
#define SOUND_BANKS_DISABLED_DURING_INTRO_CUTSCENE 1
#define OS_CYCLES_TO_USEC(value) (value)
struct SpawnInfo {
    s32 areaIndex;
    u32 behaviorArg;
    const BehaviorScript *behaviorScript;
    struct SpawnInfo *next;
};
struct ObjectWarpNode {
    struct {
        s32 id;
    } node;
    struct ObjectWarpNode *next;
};
struct Area {
    s32 index;
    s32 musicParam, musicParam2;
    void *graphNode, *camera;
    struct SpawnInfo *objectSpawnInfos;
    struct ObjectWarpNode *warpNodes;
};
static struct Area gAreas[AREA_COUNT], *gCurrentArea;
static struct SpawnInfo gPlayerSpawnInfos[1], sSpawns[128];
static struct ObjectWarpNode sNodes[128];
static s32 sSpawnCount, sNodeCount, gCurrLevelNum, gPuppyWarp, gPuppyWarpArea;
static s16 sLevelSelectOption;
static s8 sLevelSelectOptionWarp;
static u8 sPPDebugPage;
#define U_JPAD 1
#define D_JPAD 2
#define L_JPAD 4
#define R_JPAD 8
#define A_BUTTON 16
#define L_TRIG 32
#define B_BUTTON 64
#define Z_TRIG 128
#define PROFILER_TIME_PUPPYPRINT2 0
#define PUPPYPRINT_GET_SNAPSHOT() u32 first = 0
static struct {
    u16 buttonPressed, buttonDown;
    s16 rawStickX, rawStickY;
    float stickMag;
} sController, *gPlayer1Controller = &sController;
static u8 fDebug, sDebugMenu, sDebugOption;
static s32 gDebugFastWalk;
static u32 gPPSegScroll;
static u8 gMenuHoldKeyIndex;
static s16 gMenuMode;
static void *gGlobalSoundSource;
static u32 sLastScrollSound;
static void play_sound(s32 sound, void *position) {
    sLastScrollSound = (u32) sound;
}
struct PuppyPrintPage {
    s32 unused;
};
extern struct PuppyPrintPage ppPages[PUPPYPRINT_PAGE_LEVEL_SELECT + 1];
static void profiler_update(s32 timer, u32 elapsed) {
}
static void collision_mesh_benchmark_run(void) {
}
static void collision_mesh_benchmark_reset(void) {
}
static s32 mario_character_requested_index(void) {
    return 0;
}
static s32 mario_character_request(s32 index) {
    return TRUE;
}
static void mario_character_apply_appearance(s32 locked) {
}
static s32 sHealOnCourseEntry;
static void *gCurrDemoInput, *gCurrCreditsEntry;
static u8 gLevelToCourseNumTable[LEVEL_COUNT];
static struct {
    s32 type, levelNum, areaIdx, nodeId, arg;
} sWarpDest;
static s32 sDelayedWarpOp, sTransitionTimer, sSpecialWarpDest, g100CoinStarSpawned;
static s32 gStarModelLastCollected, sTimerRunning, gDebugLevelSelect, gCurrSaveFileNum;
static u32 gInitLevelTime;
static struct {
    s32 flags;
} gHudDisplay;
static struct {
    s32 action;
} sMario, *gMarioState = &sMario;
static s32 sLoadedArea, sLoadedNode, sLoadErrors;
static void set_play_mode(s32 mode) {
}
static void hud_reset_auto_visibility(void) {
}
static void warp_credits(void) {
    sLoadErrors++;
}
static void init_mario(void) {
}
static void protect_mario_from_entry_fall(void) {
}
static void reset_camera(void *camera) {
}
static s32 save_file_he_thicc_intro_pending(s32 file) {
    return FALSE;
}
static s32 save_file_exists(s32 file) {
    return TRUE;
}
static void set_mario_action(void *m, s32 action, s32 arg) {
    sMario.action = action;
}
static void play_transition(s32 kind, s32 time, s32 red, s32 green, s32 blue) {
}
static void set_background_music(s32 param, s32 sequence, s32 fade) {
}
static void cancel_rumble(void) {
}
static void sound_banks_disable(s32 player, s32 banks) {
}
static u32 osGetCount(void) {
    return 0;
}
static void append_puppyprint_log(const char *format, f64 time) {
}
static void level_control_timer(s32 op) {
}
static void load_area(s32 area) {
    if (area <= 0 || area >= AREA_COUNT || gAreas[area].graphNode == NULL) {
        sLoadErrors++;
        gCurrentArea = &gAreas[0];
        return;
    }
    gCurrentArea = &gAreas[area];
    sLoadedArea = area;
}
static void load_mario_area(void) {
    load_area(gPlayerSpawnInfos[0].areaIndex);
}
static void init_mario_after_warp(void) {
    sLoadedNode = sWarpDest.nodeId;
    s32 found = FALSE;
    for (struct SpawnInfo *spawn = gCurrentArea->objectSpawnInfos; spawn; spawn = spawn->next) {
        if (((spawn->behaviorArg >> 16) & 0xFF) == (u32) sLoadedNode) {
            found = TRUE;
        }
    }
    if (!found || gCurrentArea->camera == NULL) {
        sLoadErrors++;
    }
    sWarpDest.type = WARP_TYPE_NOT_WARPING;
    gPuppyWarp = gPuppyWarpArea = 0;
}
static void jrb_simpleflips_reset(void) {
}
static s32 jrb_simpleflips_exit_locked(void) {
    return FALSE;
}
static void clear_metadata(void) {
    memset(gAreas, 0, sizeof(gAreas));
    memset(gPlayerSpawnInfos, 0, sizeof(gPlayerSpawnInfos));
    sSpawnCount = sNodeCount = 0;
}
static void define_area(s32 area) {
    gAreas[area].index = area;
    gAreas[area].graphNode = gAreas[area].camera = &gAreas[area];
}
static void add_node(s32 area, s32 id) {
    struct ObjectWarpNode *node = &sNodes[sNodeCount++];
    node->node.id = id;
    node->next = gAreas[area].warpNodes;
    gAreas[area].warpNodes = node;
}
static void add_spawn(s32 area, u32 arg, const BehaviorScript *behavior) {
    struct SpawnInfo *spawn = &sSpawns[sSpawnCount++];
    spawn->behaviorArg = arg;
    spawn->behaviorScript = behavior;
    spawn->next = gAreas[area].objectSpawnInfos;
    gAreas[area].objectSpawnInfos = spawn;
}
/* Secret dialogue and audio players are external services to entrance routing. */
static s32 castle_secret_lobby_rejection_dialog(void) {
    return -1;
}
static void fadeout_level_music(s32 frames) {
}
static void seq_player_fade_out(u8 player, u16 frames) {
}
// Permissions are exercised by test_options_debug.py; this fixture isolates menu input.
static void puppyprint_debug_update(void) {
}
static s32 game_options_debug_active(void) {
    return TRUE;
}
/* Timing and cheat lifecycles have dedicated fixtures; this suite isolates warp routing. */
static void game_timing_warp(s32 destination, s32 exitCourse) {
}
static void game_options_runtime_restart(void) {
}
static void game_timing_end_course(void) {
}
#include "debug_warp_sources.inc"

static int selector_destinations(void) {
    for (s32 i = 0; i < LEVEL_SELECT_COUNT; i++) {
        s32 level = sLevelSelectOrder[i];
        if (level == LEVEL_ENDING) {
            EXPECT_EQ(level_select_warp_count(level), 1);
            continue;
        }
        for (s32 act = 1; act <= 6; act++) {
            load_shipped_metadata(level, act);
            s32 count = 0;
            for (s32 area = 1; area < AREA_COUNT; area++) {
                /* Secret finale and SimpleFlips rooms are outside the authored selector. */
                if ((level == LEVEL_CASTLE && area == 4) || (level == LEVEL_JRB && area == 3)) {
                    continue;
                }
                if (debug_warp_entry_node(area) >= 0) {
                    EXPECT_EQ(level_select_warp_area(level, count), area);
                    count++;
                }
            }
            EXPECT_EQ(level_select_warp_count(level), count);
        }
    }
    return TRUE;
}

static void debug_input(u16 pressed, u16 held) {
    sController.buttonPressed = pressed;
    sController.buttonDown = held;
    puppyprint_profiler_process();
}

static int menu_scroll_timing(void) {
    EXPECT_EQ(SOUND_MENU_PAUSE_OPEN & SOUNDARGS_MASK_BANK, SOUND_BANK_MENU << SOUNDARGS_SHIFT_BANK);
    EXPECT_EQ(SOUND_MENU_PAUSE_CLOSE & SOUNDARGS_MASK_BANK, SOUND_BANK_MENU << SOUNDARGS_SHIFT_BANK);
    gMenuMode = MENU_MODE_NONE;
    sController.buttonDown = 0;
    for (s32 useDpad = 0; useDpad < 2; useDpad++) {
        for (s32 forward = 0; forward < 2; forward++) {
            gMenuHoldKeyIndex = 0;
            sMenuNavigationStick = 0;
            sController.rawStickX = 0;
            sController.rawStickY = useDpad ? 0 : (forward ? -64 : 64);
            sController.buttonDown = useDpad ? (forward ? D_JPAD : U_JPAD) : 0;
            s8 selected = forward ? 4 : 1;
            handle_menu_scrolling(MENU_SCROLL_VERTICAL_WRAP, &selected, 1, 4);
            EXPECT_EQ(selected, forward ? 1 : 4);
        }
    }
    sController.buttonDown = 0;
    for (int axis = MENU_SCROLL_VERTICAL; axis <= MENU_SCROLL_HORIZONTAL; axis++) {
        gMenuHoldKeyIndex = 0;
        sController.rawStickX = axis == MENU_SCROLL_HORIZONTAL ? 64 : 0;
        sController.rawStickY = axis == MENU_SCROLL_VERTICAL ? -64 : 0;
        s8 selected = 0;
        handle_menu_scrolling(axis, &selected, 0, 3);
        EXPECT_EQ(selected, 1);
        EXPECT_EQ(sLastScrollSound, SOUND_MENU_CHANGE_SELECT);
        for (int frame = 0; frame < 30; frame++) {
            handle_menu_scrolling(axis, &selected, 0, 3);
            EXPECT_EQ(selected, 1);
        }
        sController.rawStickX = sController.rawStickY = 0;
        handle_menu_scrolling(axis, &selected, 0, 3);
        sController.rawStickX = axis == MENU_SCROLL_HORIZONTAL ? 64 : 0;
        sController.rawStickY = axis == MENU_SCROLL_VERTICAL ? -64 : 0;
        handle_menu_scrolling(axis, &selected, 0, 3);
        EXPECT_EQ(selected, 2);
        EXPECT_EQ(sLastScrollSound, SOUND_MENU_CHANGE_SELECT);
        // A direct reversal is another deliberate navigation step.
        sController.rawStickX = axis == MENU_SCROLL_HORIZONTAL ? -64 : 0;
        sController.rawStickY = axis == MENU_SCROLL_VERTICAL ? 64 : 0;
        handle_menu_scrolling(axis, &selected, 0, 3);
        EXPECT_EQ(selected, 1);
        for (int frame = 0; frame < 30; frame++) {
            handle_menu_scrolling(axis, &selected, 0, 3);
            EXPECT_EQ(selected, 1);
        }
    }
    for (int axis = MENU_SCROLL_VERTICAL; axis <= MENU_SCROLL_HORIZONTAL; axis++) {
        u16 forward = axis == MENU_SCROLL_VERTICAL ? D_JPAD : R_JPAD;
        u16 backward = axis == MENU_SCROLL_VERTICAL ? U_JPAD : L_JPAD;
        s8 selected = 1;
        sController.rawStickX = sController.rawStickY = 0;
        gMenuHoldKeyIndex = 0;
        sController.buttonDown = forward;
        handle_menu_scrolling(axis, &selected, 0, 3);
        EXPECT_EQ(selected, 2); // Both now includes dialogue and save-menu choices.
        gMenuMode = MENU_MODE_RENDER_PAUSE_SCREEN;
        sController.buttonDown = 0;
        handle_menu_scrolling(axis, &selected, 0, 3);
        selected = 1;
        sController.buttonDown = forward;
        handle_menu_scrolling(axis, &selected, 0, 3);
        EXPECT_EQ(selected, 2);
        EXPECT_EQ(sLastScrollSound & SOUNDARGS_MASK_BANK,
                  (u32) SOUND_BANK_COINS << SOUNDARGS_SHIFT_BANK);
        EXPECT_EQ(sLastScrollSound & SOUND_MENU_STYLE, SOUND_MENU_STYLE);
        for (int frame = 0; frame < 30; frame++) {
            handle_menu_scrolling(axis, &selected, 0, 3);
            EXPECT_EQ(selected, 2);
        }
        sController.buttonDown = backward;
        handle_menu_scrolling(axis, &selected, 0, 3);
        EXPECT_EQ(selected, 1);
        EXPECT_EQ(sLastScrollSound, SOUND_MENU_PAUSE_CHANGE_SELECT);
        sController.buttonDown = 0;
        handle_menu_scrolling(axis, &selected, 0, 3);
        sController.buttonDown = forward;
        handle_menu_scrolling(axis, &selected, 0, 3);
        EXPECT_EQ(selected, 2);
        sController.buttonDown = 0;
        handle_menu_scrolling(axis, &selected, 0, 3);
        sController.buttonDown = forward | Z_TRIG;
        handle_menu_scrolling(axis, &selected, 0, 3);
        EXPECT_EQ(selected, axis == MENU_SCROLL_VERTICAL ? 3 : 2);
        gMenuMode = MENU_MODE_NONE;
    }
    sController.buttonDown = 0;
    // Star select explicitly accepts horizontal D-pad input, with its normal sound and bounds.
    gMenuMode = MENU_MODE_NONE;
    gMenuHoldKeyIndex = 0;
    sController.rawStickX = sController.rawStickY = 0;
    s8 star = 0;
    sController.buttonDown = R_JPAD;
    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 1);
    EXPECT_EQ(star, 1);
    EXPECT_EQ(sLastScrollSound, SOUND_MENU_CHANGE_SELECT);
    for (int frame = 0; frame < 30; frame++) {
        handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 1);
        EXPECT_EQ(star, 1);
    }
    sController.buttonDown = L_JPAD | Z_TRIG;
    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 1);
    EXPECT_EQ(star, 0);
    sController.buttonDown = 0;
    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 1);
    sController.buttonDown = L_JPAD;
    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 1);
    EXPECT_EQ(star, 0);
    sController.buttonDown = 0;
    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 1);
    sController.buttonDown = R_JPAD | L_JPAD;
    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 1);
    EXPECT_EQ(star, 0);
    // Navigation policy affects dialogue/save/star choices as well as pause.
    for (sNavigationPolicy = 0; sNavigationPolicy < 3; sNavigationPolicy++) {
        gMenuHoldKeyIndex = sMenuNavigationStick = 0;
        sController.rawStickX = 0;
        sController.buttonDown = R_JPAD;
        star = 0;
        handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 3);
        EXPECT_EQ(star, sNavigationPolicy == 0 ? 0 : 1);
        gMenuHoldKeyIndex = sMenuNavigationStick = 0;
        sController.rawStickX = 64;
        sController.buttonDown = 0;
        star = 0;
        handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 3);
        EXPECT_EQ(star, sNavigationPolicy == 1 ? 0 : 1);
    }
    sNavigationPolicy = 2;
    sStickReleasePolicy = 1;
    gMenuHoldKeyIndex = sMenuNavigationStick = 0;
    star = 1;
    sController.rawStickX = 64;
    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 3);
    EXPECT_EQ(star, 2);
    sController.rawStickX = -64;
    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 3);
    EXPECT_EQ(star, 2);
    sController.rawStickX = 0;
    sController.rawStickY = 64; // An unused axis is not stick release.
    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 3);
    sController.rawStickX = -64;
    sController.rawStickY = 0;
    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 3);
    EXPECT_EQ(star, 2);
    sController.buttonDown = L_JPAD;
    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 3);
    EXPECT_EQ(star, 1); // Neutral-only stick navigation never blocks D-pad.
    sController.buttonDown = 0;
    sController.rawStickX = 0;
    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 3);
    sController.rawStickX = -64;
    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL_DPAD, &star, 0, 3);
    EXPECT_EQ(star, 0);
    sStickReleasePolicy = 0;
    sController.buttonDown = 0;
    // The debug boot selector still uses the repeating input helper.
    u8 held = 0;
    u8 timer = 0;
    EXPECT_EQ(menu_scrolling_input(2, &held, &timer), 2);
    for (int frame = 1; frame < 10; frame++) {
        EXPECT_EQ(menu_scrolling_input(2, &held, &timer), 0);
    }
    EXPECT_EQ(menu_scrolling_input(2, &held, &timer), 2);
    for (int frame = 0; frame < 4; frame++) {
        EXPECT_EQ(menu_scrolling_input(2, &held, &timer), 0);
    }
    EXPECT_EQ(menu_scrolling_input(2, &held, &timer), 2);
    EXPECT_EQ(menu_scrolling_input(1, &held, &timer), 1);
    EXPECT_EQ(menu_scrolling_input(0, &held, &timer), 0);
    EXPECT_EQ(timer, 0);
    EXPECT_EQ(menu_scrolling_input(1, &held, &timer), 1);
    fDebug = TRUE;
    sDebugMenu = FALSE;
    sPPDebugPage = PUPPYPRINT_PAGE_RAM;
    gPPSegScroll = 4;
    sDebugHeldDpad = 0;
    debug_input(U_JPAD, U_JPAD);
    EXPECT_EQ(gPPSegScroll, 1);
    for (int frame = 1; frame < 10; frame++) {
        debug_input(0, U_JPAD);
        EXPECT_EQ(gPPSegScroll, 1);
    }
    debug_input(0, U_JPAD);
    EXPECT_EQ(gPPSegScroll, 0);
    gPPSegScroll = 0;
    debug_input(D_JPAD, D_JPAD);
    EXPECT_EQ(gPPSegScroll, 3);
    for (int frame = 1; frame < 10; frame++) {
        debug_input(0, D_JPAD);
        EXPECT_EQ(gPPSegScroll, 3);
    }
    debug_input(0, D_JPAD);
    EXPECT_EQ(gPPSegScroll, 6);
    for (int frame = 1; frame < 5; frame++) {
        debug_input(0, D_JPAD);
        EXPECT_EQ(gPPSegScroll, 6);
    }
    debug_input(0, D_JPAD);
    EXPECT_EQ(gPPSegScroll, 9);
    gPPSegScroll = 12 * 32 - 1;
    debug_input(0, 0);
    debug_input(D_JPAD, D_JPAD);
    EXPECT_EQ(gPPSegScroll, 12 * 32);
    return TRUE;
}

static int menu_held_navigation(void) {
    fDebug = TRUE;
    sDebugMenu = FALSE;
    sPPDebugPage = PUPPYPRINT_PAGE_GENERAL;
    sMario.action = ACT_IDLE;
    sController.stickMag = 2.0f;
    debug_input(L_TRIG, L_TRIG);
    EXPECT_EQ(sDebugMenu, FALSE);
    sController.stickMag = 0.0f;
    debug_input(0, L_TRIG);
    EXPECT_EQ(sDebugMenu, FALSE);
    sMario.action = ACT_FLAG_AIR;
    debug_input(L_TRIG, L_TRIG);
    EXPECT_EQ(sDebugMenu, FALSE);
    const u32 waterActions[] = { ACT_WATER_IDLE, ACT_BREASTSTROKE, ACT_FLUTTER_KICK,
                                 ACT_WATER_SHELL_SWIMMING, ACT_HOLD_WATER_IDLE };
    for (unsigned int i = 0; i < sizeof(waterActions) / sizeof(waterActions[0]); i++) {
        sMario.action = waterActions[i];
        debug_input(L_TRIG, L_TRIG);
        EXPECT_EQ(sDebugMenu, FALSE);
        sDebugMenu = TRUE;
        debug_input(L_TRIG, L_TRIG);
        EXPECT_EQ(sDebugMenu, FALSE);
    }
    sMario.action = ACT_METAL_WATER_WALKING;
    debug_input(L_TRIG, L_TRIG);
    EXPECT_EQ(sDebugMenu, TRUE);
    sMario.action = ACT_FLAG_AIR;
    sController.stickMag = 64.0f;
    debug_input(L_TRIG, L_TRIG);
    EXPECT_EQ(sDebugMenu, FALSE);
    sMario.action = ACT_IDLE;
    sController.stickMag = 0.0f;
    for (int cycle = 0; cycle < 3; cycle++) {
        fDebug = sDebugMenu = TRUE;
        sDebugOption = sPPDebugPage = PUPPYPRINT_PAGE_GENERAL;
        sDebugHeldDpad = sDebugRepeatTimer = 0;
        debug_input(U_JPAD, U_JPAD);
        EXPECT_EQ(sDebugOption, ARRAY_COUNT(ppPages) - 1);
        for (int frame = 1; frame < 10; frame++) {
            debug_input(0, U_JPAD);
            EXPECT_EQ(sDebugOption, ARRAY_COUNT(ppPages) - 1);
        }
        debug_input(0, U_JPAD);
        EXPECT_EQ(sDebugOption, ARRAY_COUNT(ppPages) - 2);
        for (int step = 0; step < ARRAY_COUNT(ppPages) * 2; step++) {
            u8 previous = sDebugOption;
            for (int frame = 0; frame < 4; frame++) {
                debug_input(0, U_JPAD);
                EXPECT_EQ(sDebugOption, previous);
            }
            debug_input(0, U_JPAD);
            EXPECT_EQ(sDebugOption, MAX(previous - 1, 0));
        }
        u8 previous = sDebugOption;
        for (int frame = 0; frame < 12; frame++) {
            debug_input(0, 0);
            EXPECT_EQ(sDebugOption, previous);
        }
        debug_input(D_JPAD, D_JPAD);
        EXPECT_EQ(sDebugOption, (previous + 1) % ARRAY_COUNT(ppPages));
        previous = sDebugOption;
        for (int frame = 1; frame < 10; frame++) {
            debug_input(0, D_JPAD);
            EXPECT_EQ(sDebugOption, previous);
        }
        debug_input(0, D_JPAD);
        EXPECT_EQ(sDebugOption, (previous + 1) % ARRAY_COUNT(ppPages));
        // Leaving and reopening cannot carry a rapid repeat into the next menu.
        sDebugOption = PUPPYPRINT_PAGE_GENERAL;
        debug_input(L_TRIG, L_TRIG | D_JPAD);
        EXPECT_EQ(sDebugMenu, FALSE);
        EXPECT_EQ(sDebugHeldDpad, 0);
        debug_input(L_TRIG, L_TRIG | D_JPAD);
        EXPECT_EQ(sDebugMenu, TRUE);
        EXPECT_EQ(sDebugOption, PUPPYPRINT_PAGE_GENERAL);
        for (int frame = 1; frame < 10; frame++) {
            debug_input(0, D_JPAD);
            EXPECT_EQ(sDebugOption, PUPPYPRINT_PAGE_GENERAL);
        }
        debug_input(0, D_JPAD);
        EXPECT_EQ(sDebugOption, PUPPYPRINT_PAGE_GENERAL + 1);
        debug_input(L_TRIG | U_JPAD, L_TRIG | U_JPAD);
        EXPECT_EQ(fDebug, FALSE);
        EXPECT_EQ(sDebugMenu, FALSE);
        EXPECT_EQ(sDebugHeldDpad, 0);
    }
    fDebug = sDebugMenu = TRUE;
    sDebugOption = 0;
    debug_input(0, 0);
    const s16 stickDirections[][2] = { { 0, 80 }, { 0, -80 }, { 80, 0 }, { -80, 0 } };
    for (unsigned int i = 0; i < sizeof(stickDirections) / sizeof(stickDirections[0]); i++) {
        sController.rawStickX = stickDirections[i][0];
        sController.rawStickY = stickDirections[i][1];
        for (int frame = 0; frame < 120; frame++) {
            debug_input(0, 0);
            EXPECT_EQ(sDebugOption, 0);
        }
    }
    // D-pad still steps with the analogue stick held in the opposite direction.
    sController.rawStickY = 80;
    debug_input(D_JPAD, D_JPAD);
    EXPECT_EQ(sDebugOption, 1);
    sController.rawStickX = sController.rawStickY = 0;
    // Frozen pages must let Mario return to the chooser or leave debug entirely.
    const u8 frozenPages[] = { PUPPYPRINT_PAGE_RAM, PUPPYPRINT_PAGE_LEVEL_SELECT };
    const u32 restrictedActions[] = { ACT_FLAG_AIR, ACT_WATER_IDLE, ACT_WATER_SHELL_SWIMMING };
    for (unsigned int page = 0; page < ARRAY_COUNT(frozenPages); page++) {
        for (unsigned int action = 0; action < ARRAY_COUNT(restrictedActions); action++) {
            fDebug = TRUE;
            sDebugMenu = FALSE;
            sPPDebugPage = frozenPages[page];
            sMario.action = restrictedActions[action];
            sController.stickMag = 64.0f;
            debug_input(L_TRIG, L_TRIG);
            EXPECT_EQ(sDebugMenu, TRUE);
            debug_input(L_TRIG | U_JPAD, L_TRIG | U_JPAD);
            EXPECT_EQ(fDebug, FALSE);
            EXPECT_EQ(sDebugMenu, FALSE);
            EXPECT_EQ(sPPDebugPage, PUPPYPRINT_PAGE_GENERAL);
            fDebug = TRUE;
            sPPDebugPage = frozenPages[page];
            debug_input(L_TRIG | U_JPAD, L_TRIG | U_JPAD);
            EXPECT_EQ(fDebug, FALSE);
            EXPECT_EQ(sPPDebugPage, PUPPYPRINT_PAGE_GENERAL);
        }
    }
    sMario.action = ACT_IDLE;
    sController.stickMag = 0.0f;
    return TRUE;
}

static int selector_held_navigation(void) {
    sDebugHeldDpad = 0;
    sDebugRepeatTimer = DEBUG_DPAD_REPEAT_DELAY_FRAMES;
    sLevelSelectOption = 0;
    sLevelSelectOptionWarp = 0;
    u16 buttons = puppyprint_debug_dpad_buttons(D_JPAD, D_JPAD);
    puppyprint_update_level_select(buttons);
    EXPECT_EQ(sLevelSelectOption, 1);
    for (s32 i = 1; i < DEBUG_DPAD_REPEAT_DELAY_FRAMES; i++) {
        puppyprint_update_level_select(puppyprint_debug_dpad_buttons(0, D_JPAD));
        EXPECT_EQ(sLevelSelectOption, 1);
    }
    puppyprint_update_level_select(puppyprint_debug_dpad_buttons(0, D_JPAD));
    EXPECT_EQ(sLevelSelectOption, 2);
    for (s32 repeat = 0; repeat < LEVEL_SELECT_COUNT * 3; repeat++) {
        s32 previous = sLevelSelectOption;
        for (s32 i = 1; i < DEBUG_DPAD_REPEAT_INTERVAL_FRAMES; i++) {
            puppyprint_update_level_select(puppyprint_debug_dpad_buttons(0, D_JPAD));
            EXPECT_EQ(sLevelSelectOption, previous);
        }
        puppyprint_update_level_select(puppyprint_debug_dpad_buttons(0, D_JPAD));
        EXPECT_EQ(sLevelSelectOption, MIN(previous + 1, LEVEL_SELECT_COUNT - 1));
    }
    puppyprint_update_level_select(puppyprint_debug_dpad_buttons(0, 0));
    puppyprint_update_level_select(puppyprint_debug_dpad_buttons(D_JPAD, D_JPAD));
    EXPECT_EQ(sLevelSelectOption, 0);
    puppyprint_update_level_select(puppyprint_debug_dpad_buttons(U_JPAD, U_JPAD));
    EXPECT_EQ(sLevelSelectOption, LEVEL_SELECT_COUNT - 1);
    s32 previous = sLevelSelectOption;
    puppyprint_update_level_select(puppyprint_debug_dpad_buttons(0, 0));
    for (s32 i = 0; i < 30; i++) {
        puppyprint_update_level_select(puppyprint_debug_dpad_buttons(0, 0));
        EXPECT_EQ(sLevelSelectOption, previous);
    }
    puppyprint_update_level_select(puppyprint_debug_dpad_buttons(U_JPAD, U_JPAD));
    EXPECT_EQ(sLevelSelectOption, (previous + LEVEL_SELECT_COUNT - 1) % LEVEL_SELECT_COUNT);
    previous = sLevelSelectOption;
    for (s32 i = 1; i < DEBUG_DPAD_REPEAT_DELAY_FRAMES; i++) {
        puppyprint_update_level_select(puppyprint_debug_dpad_buttons(0, U_JPAD));
        EXPECT_EQ(sLevelSelectOption, previous);
    }
    puppyprint_update_level_select(puppyprint_debug_dpad_buttons(0, U_JPAD));
    EXPECT_EQ(sLevelSelectOption, (previous + LEVEL_SELECT_COUNT - 1) % LEVEL_SELECT_COUNT);

    sLevelSelectOption = level_select_index(LEVEL_TTM);
    sLevelSelectOptionWarp = 0;
    puppyprint_update_level_select(puppyprint_debug_dpad_buttons(R_JPAD, R_JPAD));
    EXPECT_EQ(sLevelSelectOptionWarp, 1);
    for (s32 i = 0; i < DEBUG_DPAD_REPEAT_DELAY_FRAMES; i++) {
        puppyprint_update_level_select(puppyprint_debug_dpad_buttons(0, R_JPAD));
    }
    EXPECT_EQ(sLevelSelectOptionWarp, 2);
    for (s32 i = 0; i < DEBUG_DPAD_REPEAT_INTERVAL_FRAMES; i++) {
        puppyprint_update_level_select(puppyprint_debug_dpad_buttons(0, R_JPAD));
    }
    EXPECT_EQ(sLevelSelectOptionWarp, 2);
    puppyprint_update_level_select(puppyprint_debug_dpad_buttons(0, 0));
    puppyprint_update_level_select(puppyprint_debug_dpad_buttons(R_JPAD, R_JPAD));
    EXPECT_EQ(sLevelSelectOptionWarp, 0);
    puppyprint_update_level_select(puppyprint_debug_dpad_buttons(L_JPAD, L_JPAD));
    EXPECT_EQ(sLevelSelectOptionWarp, 2);
    EXPECT_EQ(puppyprint_debug_dpad_buttons(A_BUTTON, L_JPAD) & A_BUTTON, A_BUTTON);
    EXPECT_EQ(puppyprint_debug_dpad_buttons(0, L_JPAD) & A_BUTTON, 0);
    return TRUE;
}

static int selector_navigation(void) {
    sDebugManualDirections = U_JPAD | D_JPAD | L_JPAD | R_JPAD;
    for (s32 i = 0; i < LEVEL_SELECT_COUNT; i++) {
        s32 level = sLevelSelectOrder[i];
        s32 count = level_select_warp_count(level);
        sLevelSelectOption = i;
        sLevelSelectOptionWarp = 0;
        for (s32 cycle = 0; cycle < 3; cycle++) {
            for (s32 warp = 0; warp < count; warp++) {
                sPPDebugPage = 1;
                puppyprint_update_level_select(A_BUTTON);
                EXPECT_EQ(sPPDebugPage, 0);
                EXPECT_EQ(gPuppyWarp, level);
                EXPECT_EQ(gPuppyWarpArea, level_select_warp_area(level, warp));
                puppyprint_update_level_select(R_JPAD);
                EXPECT_EQ(sLevelSelectOptionWarp, (warp + 1) % count);
                puppyprint_update_level_select(0); // Releasing the button keeps the choice.
                EXPECT_EQ(sLevelSelectOptionWarp, (warp + 1) % count);
            }
            puppyprint_update_level_select(L_JPAD);
            EXPECT_EQ(sLevelSelectOptionWarp, count - 1);
            puppyprint_update_level_select(R_JPAD);
            EXPECT_EQ(sLevelSelectOptionWarp, 0);
        }
        // Moving to a shorter list cannot confirm the previous level's extra destination.
        sLevelSelectOptionWarp = count - 1;
        puppyprint_update_level_select(D_JPAD | A_BUTTON);
        s32 next = sLevelSelectOrder[(i + 1) % LEVEL_SELECT_COUNT];
        EXPECT_EQ(gPuppyWarp, next);
        EXPECT_EQ(sLevelSelectOptionWarp < level_select_warp_count(next), TRUE);
        EXPECT_EQ(gPuppyWarpArea, level_select_warp_area(next, sLevelSelectOptionWarp));
        puppyprint_update_level_select(U_JPAD);
        EXPECT_EQ(sLevelSelectOption, i);
    }
    sLevelSelectOption = level_select_index(LEVEL_TTM);
    sLevelSelectOptionWarp = 2;
    puppyprint_update_level_select(A_BUTTON);
    EXPECT_EQ(gPuppyWarpArea, 4);
    return TRUE;
}

static int shipped_entrances(void) {
    s32 levelsChecked = 0;
    for (s32 level = 1; level < LEVEL_COUNT; level++) {
        if (level == LEVEL_ENDING || level <= 3 || level == LEVEL_UNKNOWN_35
            || level == LEVEL_UNKNOWN_37 || level == LEVEL_UNKNOWN_38) {
            continue;
        }
        levelsChecked++;
        for (s32 act = 1; act <= 6; act++) {
            for (s32 requested = 0; requested <= AREA_COUNT; requested++) {
                load_shipped_metadata(level, act);
                gCurrLevelNum = gPuppyWarp = level;
                gPuppyWarpArea = requested;
                gCurrentArea = NULL; // Reload classification must not dereference the unloaded area.
                initiate_warp(level, requested, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
                EXPECT_EQ(sWarpDest.type, WARP_TYPE_CHANGE_LEVEL);
                s32 requestedNode = debug_warp_entry_node(requested);
                resolve_debug_warp_destination();
                EXPECT_EQ(sWarpDest.type, WARP_TYPE_CHANGE_LEVEL);
                EXPECT_EQ(sWarpDest.levelNum, level);
                EXPECT_EQ(sWarpDest.areaIdx > 0 && sWarpDest.areaIdx < AREA_COUNT, TRUE);
                EXPECT_EQ(debug_warp_entry_node(sWarpDest.areaIdx), sWarpDest.nodeId);
                EXPECT_EQ(gAreas[sWarpDest.areaIdx].camera != NULL, TRUE);
                if (requestedNode >= 0) {
                    EXPECT_EQ(sWarpDest.areaIdx, requested);
                    EXPECT_EQ(sWarpDest.nodeId, requestedNode);
                }
                s32 resolvedArea = sWarpDest.areaIdx;
                s32 resolvedNode = sWarpDest.nodeId;
                // Exercise the actual initialization hook before area and Mario loading.
                sWarpDest.areaIdx = requested;
                sWarpDest.nodeId = WARP_NODE_MAIN_ENTRY;
                sLoadErrors = 0;
                init_level();
                EXPECT_EQ(sLoadErrors, 0);
                EXPECT_EQ(sLoadedArea, resolvedArea);
                EXPECT_EQ(sLoadedNode, resolvedNode);
                EXPECT_EQ(gPuppyWarp, 0);
                // Return to another level, then repeat using freshly constructed script metadata.
                gPuppyWarp = level;
                gCurrLevelNum = LEVEL_BOB;
                gCurrentArea = &gAreas[sWarpDest.areaIdx];
                initiate_warp(level, requested, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
                resolve_debug_warp_destination();
                EXPECT_EQ(sWarpDest.type, WARP_TYPE_CHANGE_LEVEL);
            }
        }
    }
    EXPECT_EQ(levelsChecked, 31);
    return TRUE;
}
static int boundaries(void) {
    clear_metadata();
    define_area(1);
    define_area(2);
    gPlayerSpawnInfos[0].areaIndex = 1;
    add_node(1, 10); // A node without its object is not an entrance.
    add_node(2, 19);
    add_spawn(2, 19u << 16, bhvDoorWarp);
    add_node(2, 10);
    add_spawn(2, 10u << 16, bhvDeathWarp); // Do not route a debug visit into death.
    gPuppyWarp = LEVEL_CASTLE;
    gPuppyWarpArea = sWarpDest.areaIdx = 1;
    sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
    resolve_debug_warp_destination();
    EXPECT_EQ(sWarpDest.areaIdx, 2);
    EXPECT_EQ(sWarpDest.nodeId, 19);
    add_spawn(2, 10u << 16, bhvAirborneWarp);
    EXPECT_EQ(debug_warp_entry_node(2), 10);
    gAreas[2].camera = NULL;
    EXPECT_EQ(debug_warp_entry_node(2), -1);
    resolve_debug_warp_destination();
    EXPECT_EQ(sWarpDest.type, WARP_TYPE_NOT_WARPING);
    EXPECT_EQ(gPuppyWarp, 0);
    EXPECT_EQ(gPuppyWarpArea, 0);
    gPuppyWarp = 0;
    gCurrLevelNum = LEVEL_BOB;
    gCurrentArea = &gAreas[1];
    initiate_warp(LEVEL_BOB, 1, 10, WARP_FLAGS_NONE);
    EXPECT_EQ(sWarpDest.type, WARP_TYPE_SAME_AREA);
    initiate_warp(LEVEL_BOB, 2, 10, WARP_FLAGS_NONE);
    EXPECT_EQ(sWarpDest.type, WARP_TYPE_CHANGE_AREA);
    return TRUE;
}
int main(int argc, char **argv) {
    test_init(argc, argv);
    if (test_selected("debug_warp.selector_destinations")) {
        test_report("debug_warp.selector_destinations", selector_destinations());
    }
    if (test_selected("debug_warp.selector_navigation")) {
        test_report("debug_warp.selector_navigation", selector_navigation());
    }
    if (test_selected("debug_warp.selector_held_navigation")) {
        test_report("debug_warp.selector_held_navigation", selector_held_navigation());
    }
    if (test_selected("debug_warp.menu_held_navigation")) {
        test_report("debug_warp.menu_held_navigation", menu_held_navigation());
    }
    if (test_selected("debug_warp.menu_scroll_timing")) {
        test_report("debug_warp.menu_scroll_timing", menu_scroll_timing());
    }
    if (test_selected("debug_warp.shipped_entrances")) {
        test_report("debug_warp.shipped_entrances", shipped_entrances());
    }
    if (test_selected("debug_warp.boundaries")) {
        test_report("debug_warp.boundaries", boundaries());
    }
    return test_finish();
}
