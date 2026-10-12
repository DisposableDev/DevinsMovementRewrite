/* Run shipped cake scripts and callback; loading, sound and pool operations are supplied services. */
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "test_support.h"
#define POWER_STARS_HEAL

typedef int16_t s16;
typedef int32_t s32;
typedef uint32_t u32;
typedef int8_t s8;
typedef uint8_t u8;
typedef uint16_t u16;
typedef float f32;
#define TRUE 1
#define FALSE 0
#define jrb_simpleflips_room_active() 0
#define jrb_simpleflips_exit_locked() 0
#define jrb_simpleflips_is_game_over() 0
#define UNUSED
#define PUPPYPRINT_DEBUG
#define HUD_DISPLAY_NONE 0
#include "end_screen_definitions.inc"
static s32 save_file_is_he_thicc(s32 file) {
    return FALSE;
}

static struct {
    s32 type, levelNum, areaIdx, nodeId, arg;
} sWarpDest;
static s32 sHealOnCourseEntry, sDelayedWarpOp, gPuppyWarp, gPuppyWarpArea;
#define START_BUTTON 0x1000
#ifndef Z_TRIG
#define Z_TRIG 0x2000
#endif
#define A_BUTTON 0x8000
#define PUPPYPRINT_PAGE_RAM 1
#define PUPPYPRINT_PAGE_LEVEL_SELECT 2
#define CAM_MOVE_PAUSE_SCREEN 1
#define COURSE_NONE 0
#define COURSE_MIN 1
#define COURSE_CAKE_END 25
#define MENU_OPT_NONE 0
#define MENU_OPT_DEFAULT 1
#define MENU_OPT_EXIT_COURSE 2
#define MENU_OPT_CAMERA_ANGLE_R 3
#define MENU_OPT_STATISTICS 4
#define MENU_OPT_OPTIONS 5
#define MENU_MODE_NONE -1
#define MENU_MODE_RENDER_PAUSE_SCREEN 0
#define MENU_SCROLL_VERTICAL 0
#define MENU_SCROLL_VERTICAL_WRAP 3
#define MENU_MTX_PUSH 0
#define G_MTX_MODELVIEW 0
#define DIALOG_STATE_OPENING 0
#define DIALOG_STATE_VERTICAL 1
#define DIALOG_STATE_HORIZONTAL 2
#define SOUND_MENU_PAUSE_OPEN 1
#define SOUND_MENU_PAUSE_CLOSE 2
#define SEQ_PLAYER_SFX 0
#define SOUND_BANKS_ALL 0xFFFF
#define SOUND_BANKS_DISABLED_AFTER_CREDITS 0x10
#define SOUND_MENU_THANK_YOU_PLAYING_MY_GAME 12
#define WARP_TRANSITION_FADE_INTO_COLOR 0
#define EXIT_COURSE_LEVEL LEVEL_CASTLE_GROUNDS
#define EXIT_COURSE_AREA 1
#define EXIT_COURSE_NODE 0x32
#define TEXT_CONTINUE 1
#define TEXT_EXIT_COURSE 2
#define TEXT_CAMERA_ANGLE_R 3
#define LANGUAGE_ARRAY(x) (x)
#define gSPDisplayList(...) ((void) 0)
#define gDPSetEnvColor(...) ((void) 0)
#define gSPPopMatrix(...) ((void) 0)
#define assertf(expr, ...)                                                                             \
    do {                                                                                               \
        if (!(expr))                                                                                   \
            abort();                                                                                   \
    } while (0)
struct Object {
    u32 oInteractionSubtype, oInteractStatus, oBehParams;
    struct {
        struct {
            s32 cameraToObject;
        } gfx;
    } header;
} starObject, marioObject;
struct MarioState {
    u32 action, flags;
    s32 actionState, health, hurtCounter, healCounter, capTimer, numCoins, numStars;
    u8 yoshiCoinDeathPending;
    u8 poisonMushroomDeathPending;
    s16 respawnHealCounter;
    struct Object *marioObj, *interactObj, *usedObj;
} mario, *gMarioState = &mario;
#define MODEL_NONE 0
#define SOUND_MENU_STAR_SOUND 3
#define COURSE_MAX 25
static s32 bhvStarKeyCollectionPuffSpawner, gStarModelLastCollected, gCurrSaveFileNum;
static s32 sCompletionWrites;
static void mario_stop_riding_and_holding(struct MarioState *m) {
}
static void queue_rumble_data(s32 frames, s32 strength) {
}
static s32 mario_is_head_underwater(struct MarioState *m) {
    return FALSE;
}
static void spawn_object(struct Object *obj, s32 model, s32 behavior) {
}
static s32 obj_get_model_id(struct Object *obj) {
    return MODEL_NONE;
}
static void save_file_collect_star_or_key(s32 coins, u32 star) {
}
static s32 save_file_get_total_star_count(s32 file, s32 min, s32 max) {
    return 0;
}
static void drop_queued_background_music(void) {
}
static void fadeout_level_music(s32 frames) {
}
static void update_mario_sound_and_camera(struct MarioState *m) {
}
static void save_file_mark_game_completed(void) {
    sCompletionWrites++;
}
static u32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    return TRUE;
}
struct Controller {
    u16 buttonPressed, buttonDown;
    s16 rawStickX, rawStickY;
} controller, *gPlayer1Controller = &controller;
struct CreditsEntry {
    s32 levelNum, areaIndex, actNum;
} sCreditsSequence[3];
struct ObjectWarpNode {
    struct {
        s32 destLevel, destArea, destNode;
    } node;
} warpNode;
static struct CreditsEntry *gCurrCreditsEntry, *sDispCreditsEntry;
static void *gCurrDemoInput;
static s32 gCurrLevelNum, gCurrCourseNum, gCurrActNum, gSavedCourseNum, gSpecialTripleJump;
static s32 gDebugLevelSelect, sPPDebugPage, gCameraMovementFlags, gMenuOptSelectIndex;
static s32 gMenuMode, gDialogBoxState, gDialogTextAlpha;
static s8 gDialogLineNum;
static s32 gCutsceneMsgIndex, gCutsceneMsgTimer, gCutsceneMsgFade, gCutsceneMsgDuration;
static s32 gCutsceneMsgXOffset, gCutsceneMsgYOffset, gDialogID;
static u8 message[] = { 0 }, *gEndCutsceneStringsEn[] = { message };
static s32 sCurrPlayMode, sSpecialWarpDest, sDelayedWarpTimer, sSourceWarpNodeId, sDelayedWarpArg;
static s16 sTransitionTimer;
static void (*sTransitionUpdate)(s16 *);
static u8 sPauseTasCodeProgress;
static u32 gGlobalTimer;
static struct {
    s32 isActive;
} gWarpTransition;
static s32 gGlobalSoundSource, sFadeCount, sWarpCount, sSoundBanks, sCaptionDraws;
static s32 sCameraChanges, sScoreDraws, sMenuMax;
static void set_play_mode(s16 mode) {
    sCurrPlayMode = mode;
}
static void set_menu_mode(s16 mode) {
    gMenuMode = mode;
}
static s32 get_dialog_id(void) {
    return gDialogID;
}
static void level_set_transition(s16 length, void (*fn)(s16 *)) {
    sTransitionTimer = length;
    sTransitionUpdate = fn;
}
static void reset_dialog_render_state(void) {
    level_set_transition(0, NULL);
    gDialogID = -1;
    gDialogBoxState = DIALOG_STATE_OPENING;
}
static void sound_banks_enable(s32 player, s32 banks) {
    sSoundBanks |= banks;
}
static void sound_banks_disable(s32 player, s32 banks) {
    sSoundBanks &= ~banks;
}
static void fadeout_music(s32 duration) {
    sFadeCount++;
}
static void play_transition(s32 type, s32 frames, s32 r, s32 g, s32 b) {
}
static void lower_background_noise(s32 value) {
}
static void raise_background_noise(s32 value) {
}
static void cancel_rumble(void) {
}
static void hud_reveal_after_pause(void) {
}
static s32 mario_character_update_tas_code(u8 *progress, u16 buttons) {
    return FALSE;
}
static s32 sUnlockDeath;
static s32 mario_character_is_unlock_death(struct MarioState *m) {
    return sUnlockDeath;
}
static void lvl_prepare_tas_thicc_start(void) {
    abort();
}
static void lvl_restore_cap_on_file_start(s32 arg, s32 level) {
    abort();
}
static void lvl_init_from_save_file(s32 arg, s32 level) {
    abort();
}
static s32 sSaveReloads;
static void save_file_reload(void) {
    sSaveReloads++;
}
static void save_file_arm_he_thicc(void) {
}
static struct ObjectWarpNode *area_get_warp_node(s32 id) {
    return &warpNode;
}
static void check_if_should_set_warp_checkpoint(void *node) {
}
static void create_dl_ortho_matrix(void) {
}
static s16 get_str_x_pos_from_center(s32 center, u8 *text, float width) {
    return center;
}
static void print_generic_string(s32 x, s32 y, u8 *text) {
    sCaptionDraws++;
}
static void create_dl_translation_matrix(s32 mode, s32 x, s32 y, s32 z) {
}
static void handle_menu_scrolling(s32 mode, s8 *index, s8 min, s8 max) {
    sMenuMax = max;
    if (*index > max) {
        *index = max;
    }
    if (*index < min) {
        *index = min;
    }
}
static void update_paused_camera(void) {
}
static void change_dialog_camera_angle(void) {
    sCameraChanges++;
}
static void highlight_last_course_complete_stars(void) {
}
static void shade_screen(void) {
}
static void print_hud_pause_colorful_str(void) {
}
static void render_pause_my_score_coins(void) {
    sScoreDraws++;
}
static void render_pause_red_coins(void) {
    sScoreDraws++;
}
static void render_pause_castle_menu_box(s32 x, s32 y) {
}
static void render_pause_castle_main_strings(s32 x, s32 y) {
}
static s32 sThankYouSounds, sCakeClipStarts, sCakeClipPlaying, sCakeClipStops;
static void play_sound(s32 sound, s32 source) {
    if (sound == SOUND_MENU_THANK_YOU_PLAYING_MY_GAME) {
        sThankYouSounds++;
    }
}
static s32 yoshi_cake_pcm_start(void) {
    sCakeClipStarts++;
    sCakeClipPlaying = TRUE;
    return TRUE;
}
static s32 yoshi_cake_pcm_is_playing(void) {
    return sCakeClipPlaying;
}
static void pause_pcm_stop(void) {
    sCakeClipStops++;
    sCakeClipPlaying = FALSE;
}

static struct {
    s32 flags;
} gHudDisplay;
static s32 sVolumeResets, sSoundEnables;
static void reset_volume(void) {
    sVolumeResets++;
}
static void enable_background_sound(void) {
    sSoundEnables++;
}
static void initiate_warp(s16 level, s16 area, s16 node, s32 flags) {
    // Include same-cake destinations, which must reload instead of using an in-area warp.
    sWarpCount++;
    sWarpDest.type = level == gCurrLevelNum ? WARP_TYPE_SAME_AREA : WARP_TYPE_CHANGE_LEVEL;
    if (flags & WARP_FLAG_EXIT_COURSE) {
        sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
    }
    sWarpDest.levelNum = level;
    sWarpDest.areaIdx = area;
    sWarpDest.nodeId = node;
    sWarpDest.arg = flags;
}

typedef struct LevelScript LevelScript;
struct LevelScript {
    s32 op, arg;
    const LevelScript *target;
    s32 (*callback)(s16, s32);
};
enum {
    INIT,
    FREE_POOL,
    LOAD_AREA_OP,
    SLEEP_OP,
    CALL_OP,
    CALL_LOOP_OP,
    LINK_OP,
    RETURN_OP,
    JUMP_OP,
    CLEAR_OP,
    EXIT_OP,
    NOOP
};
#define CMD(op, arg)                                                                                   \
    { op, arg, NULL, NULL }
#define INIT_LEVEL() CMD(INIT, 0)
#define LOAD_LEVEL_DATA(...) CMD(NOOP, 0)
#define ALLOC_LEVEL_POOL() CMD(NOOP, 0)
#define AREA(...) CMD(NOOP, 0)
#define END_AREA() CMD(NOOP, 0)
#define FREE_LEVEL_POOL() CMD(FREE_POOL, 0)
#define SLEEP(frames) CMD(SLEEP_OP, frames)
#define SLEEP_BEFORE_EXIT(frames) SLEEP(frames)
#define BLACKOUT(...) CMD(NOOP, 0)
#define LOAD_AREA(area) CMD(LOAD_AREA_OP, area)
#define TRANSITION(...) CMD(NOOP, 0)
#define CALL(arg, func)                                                                                \
    { CALL_OP, arg, NULL, func }
#define CALL_LOOP(arg, func)                                                                           \
    { CALL_LOOP_OP, arg, NULL, func }
#define JUMP_LINK(target)                                                                              \
    { LINK_OP, 0, target, NULL }
#define RETURN() CMD(RETURN_OP, 0)
#define JUMP(target)                                                                                   \
    { JUMP_OP, 0, target, NULL }
#define CLEAR_LEVEL() CMD(CLEAR_OP, 0)
#define EXIT() CMD(EXIT_OP, 0)
#define EXIT_AND_EXECUTE_WITH_CODE(seg, start, end, entry, bssStart, bssEnd) JUMP(entry)

/* The pause easter egg has its own timer/audio fixture. */
static void pause_easter_egg_update(void) {
}
/* Secret encounter and statistics rendering have independent fixtures. */
static s32 castle_secret_lobby_active(void) {
    return FALSE;
}
static u32 sPauseMenuOpenedFrame;
static s32 sPauseMenuStartRepressed;
static s32 sPauseMenuARepressed;
static s32 sPauseStatisticsOpen;
static s8 sPauseCastleSelection;
static u8 sPauseCastleOptionsHeld;
static u8 sPauseCastleOptionsStick;
static u8 sMenuNavigationStick;
static u8 sMenuNavigationDirection;
static u8 gMenuHoldKeyIndex;
static u32 sPauseCastleOptionsFrame;
#define SOUND_MENU_PAUSE_CHANGE_SELECT 3
static s32 sCloseStatistics;
static s32 player_stats_menu_update(void) {
    return sCloseStatistics;
}
static void player_stats_menu_render(void) {
}
void game_options_menu_render(void) {
}
static void player_stats_menu_open(void) {
}
static void render_pause_castle_options(void) {
}
static void player_stats_menu_text(s16 x, s16 y, const char *text) {
}
static s16 player_stats_menu_text_centered(s16 centerX, s16 y, const char *text) {
    return centerX;
}
static s16 pause_menu_center_x(void) {
    return 160;
}
#include "end_screen_sources.inc"

static const LevelScript *sCommand, *sReturn;
static s32 sRegister, sDelay, sPoolDepth, sClears, sLoadedArea;
static void frame(void) {
    for (s32 budget = 0; sCommand != NULL && budget < 40; budget++) {
        const LevelScript *cmd = sCommand;
        switch (cmd->op) {
            case INIT:
                sPoolDepth++;
                break;
            case FREE_POOL:
                sPoolDepth++;
                break;
            case LOAD_AREA_OP:
                sLoadedArea = cmd->arg;
                break;
            case SLEEP_OP:
                if (sDelay == 0) {
                    sDelay = cmd->arg;
                    return;
                }
                if (--sDelay != 0) {
                    return;
                }
                break;
            case CALL_OP:
            case CALL_LOOP_OP:
                sRegister = cmd->callback(cmd->arg, sRegister);
                if (cmd->op == CALL_LOOP_OP && sRegister == 0) {
                    return;
                }
                break;
            case LINK_OP:
                sReturn = cmd + 1;
                sCommand = cmd->target;
                continue;
            case RETURN_OP:
                sCommand = sReturn;
                continue;
            case JUMP_OP:
                sCommand = cmd->target;
                continue;
            case CLEAR_OP:
                sPoolDepth -= 2;
                sLoadedArea = 0;
                sClears++;
                break;
            case EXIT_OP:
                sCommand = NULL;
                return;
        }
        sCommand++;
    }
}
static void begin(const LevelScript *entry) {
    sUnlockDeath = FALSE;
    gDebugLevelSelect = FALSE;
    gCurrLevelNum = LEVEL_ENDING;
    mario.health = 0x880;
    controller = (struct Controller){ 0 };
    game_options_init(NULL, NULL);
    game_options_select_file(0);
    game_options_restart();
    gOptionsMenu.open = 0;
    gWarpTransition.isActive = FALSE;
    sPPDebugPage = 0;
    gMenuOptSelectIndex = MENU_OPT_NONE;
    sCommand = entry;
    sRegister = sDelay = sPoolDepth = sClears = sLoadedArea = 0;
    sThankYouSounds = sVolumeResets = sSoundEnables = 0;
    sCakeClipStarts = sCakeClipPlaying = sCakeClipStops = 0;
    sWarpDest.arg = WARP_FLAGS_NONE;
}
static void advance(s32 frames) {
    for (s32 i = 0; i < frames; i++) {
        frame();
    }
}
static int debug_departure(void) {
    const s32 levels[] = { LEVEL_BOB, LEVEL_ENDING, LEVEL_PHYSICS_LAB };
    for (s32 cycle = 0; cycle < 3; cycle++) {
        for (s32 area = 1; area <= 4; area++) {
            for (s32 i = 0; i < 3; i++) {
                gPuppyWarp = LEVEL_ENDING;
                gPuppyWarpArea = area;
                sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
                sHealOnCourseEntry = TRUE;
                sDelayedWarpOp = WARP_OP_CREDITS_END;
                gCurrCreditsEntry = &sCreditsSequence[0];
                gHudDisplay.flags = 0xFF;
                begin(level_ending_entry);
                advance(200);
                EXPECT_EQ(gPuppyWarp, 0);
                EXPECT_EQ(gPuppyWarpArea, 0);
                EXPECT_EQ(sWarpDest.type, WARP_TYPE_NOT_WARPING);
                EXPECT_EQ(sHealOnCourseEntry, FALSE);
                EXPECT_EQ(sDelayedWarpOp, WARP_OP_NONE);
                EXPECT_EQ(gCurrCreditsEntry == NULL, TRUE);
                EXPECT_EQ(gHudDisplay.flags, HUD_DISPLAY_NONE);
                EXPECT_EQ(sLoadedArea, 1);
                EXPECT_EQ(sThankYouSounds, 1);
                EXPECT_EQ(sClears, 0);
                EXPECT_EQ(sCommand != NULL, TRUE);
                gPuppyWarp = levels[i];
                gPuppyWarpArea = area;
                advance(3);
                EXPECT_EQ(sCommand == NULL, TRUE);
                EXPECT_EQ(sRegister, levels[i]);
                EXPECT_EQ(sWarpDest.levelNum, levels[i]);
                EXPECT_EQ(sWarpDest.areaIdx, area);
                EXPECT_EQ(sWarpDest.nodeId, 0x0A);
                EXPECT_EQ(sWarpDest.arg, WARP_FLAGS_NONE);
                EXPECT_EQ(sWarpDest.type, WARP_TYPE_CHANGE_LEVEL);
                EXPECT_EQ(sClears, 1);
                EXPECT_EQ(sPoolDepth, 0);
                EXPECT_EQ(sLoadedArea, 0);
                EXPECT_EQ(sVolumeResets, 1);
                EXPECT_EQ(sSoundEnables, 1);
            }
        }
    }
    return TRUE;
}
static int automatic_departure(void) {
    for (s32 cycle = 0; cycle < 3; cycle++) {
        begin(level_ending_entry);
        if (cycle != 0) {
            game_options_set(GO_INFINITE_HEALTH, 1);
            if (cycle == 2) {
                game_options_set(GO_INFINITE_HEALTH, 0);
            }
        }
        s32 destination = cycle ? LEVEL_CASTLE_GROUNDS : LEVEL_PHYSICS_LAB;
        // Loading cake must retain a cheat used during the preceding Bowser visit.
        game_options_runtime_restart();
        while (sThankYouSounds == 0) {
            frame();
        }
        EXPECT_EQ(sCurrPlayMode, PLAY_MODE_NORMAL);
        // The first callback runs on the thank-you update.
        advance(298);
        EXPECT_EQ(sCurrPlayMode, PLAY_MODE_NORMAL);
        EXPECT_EQ(sClears, 0);
        advance(1);
        EXPECT_EQ(sCurrPlayMode, PLAY_MODE_CHANGE_LEVEL);
        EXPECT_EQ(sWarpDest.levelNum, destination);
        EXPECT_EQ(sWarpDest.areaIdx, 1);
        EXPECT_EQ(sWarpDest.nodeId, cycle ? 0x32 : WARP_NODE_MAIN_ENTRY);
        EXPECT_EQ(sWarpDest.arg, WARP_FLAG_EXIT_COURSE);
        advance(31);
        EXPECT_EQ(sClears, 1);
        advance(2);
        EXPECT_EQ(sRegister, destination);
        EXPECT_EQ(sCommand == NULL, TRUE);
        EXPECT_EQ(sLoadedArea, 0);
        EXPECT_EQ(sPoolDepth, 0);
        EXPECT_EQ(sVolumeResets, 1);
        EXPECT_EQ(sSoundEnables, 1);
    }
    return TRUE;
}
static int pause_countdown(void) {
    begin(level_ending_entry);
    lvl_end_screen(0, 0);
    for (s32 i = 0; i < 299; i++) {
        lvl_end_screen(1, 0);
    }
    controller.buttonPressed = START_BUTTON;
    lvl_end_screen(1, 0);
    EXPECT_EQ(sCurrPlayMode, PLAY_MODE_PAUSED);
    controller.buttonPressed = 0;
    for (s32 i = 0; i < 600; i++) {
        lvl_end_screen(1, 0);
    }
    EXPECT_EQ(sCurrPlayMode, PLAY_MODE_PAUSED);
    EXPECT_EQ(sWarpDest.type, WARP_TYPE_NOT_WARPING);
    gMenuOptSelectIndex = MENU_OPT_DEFAULT;
    lvl_end_screen(1, 0);
    EXPECT_EQ(sCurrPlayMode, PLAY_MODE_NORMAL);
    EXPECT_EQ(gCameraMovementFlags & CAM_MOVE_PAUSE_SCREEN, 0);
    lvl_end_screen(1, 0);
    EXPECT_EQ(sCurrPlayMode, PLAY_MODE_CHANGE_LEVEL);
    EXPECT_EQ(sWarpDest.levelNum, LEVEL_PHYSICS_LAB);
    // A fresh cake has a fresh clock; Exit Course can leave it immediately.
    lvl_end_screen(0, 0);
    lvl_end_screen(1, 0);
    EXPECT_EQ(sCurrPlayMode, PLAY_MODE_NORMAL);
    controller.buttonPressed = START_BUTTON;
    lvl_end_screen(1, 0);
    gMenuOptSelectIndex = MENU_OPT_EXIT_COURSE;
    lvl_end_screen(1, 0);
    EXPECT_EQ(sCurrPlayMode, PLAY_MODE_CHANGE_LEVEL);
    EXPECT_EQ(sWarpDest.levelNum, LEVEL_PHYSICS_LAB);
    for (s32 debug = 0; debug < 2; debug++) {
        begin(level_ending_entry);
        gDebugLevelSelect = debug;
        if (!debug) {
            game_options_set(GO_INFINITE_HEALTH, 1);
        }
        lvl_end_screen(0, 0);
        controller.buttonPressed = START_BUTTON;
        lvl_end_screen(1, 0);
        EXPECT_EQ(sCurrPlayMode, PLAY_MODE_PAUSED);
        gMenuOptSelectIndex = MENU_OPT_EXIT_COURSE;
        lvl_end_screen(1, 0);
        EXPECT_EQ(sWarpDest.levelNum, LEVEL_CASTLE_GROUNDS);
        EXPECT_EQ(sWarpDest.nodeId, 0x32);
    }
    return TRUE;
}
static int ending_pause_policy(void) {
    begin(level_ending_entry);
    gCurrLevelNum = LEVEL_CASTLE_GROUNDS;
    gDialogID = -1;
    sDelayedWarpOp = WARP_OP_NONE;
    controller.buttonPressed = START_BUTTON;
    const u32 locked[] = { ACT_JUMBO_STAR_CUTSCENE, ACT_CREDITS_CUTSCENE, ACT_INTRO_CUTSCENE };
    for (s32 i = 0; i < 3; i++) {
        mario.action = locked[i];
        EXPECT_EQ(pressed_pause(), FALSE);
    }
    mario.action = ACT_DEBUG_FREE_MOVE;
    EXPECT_EQ(pressed_pause(), TRUE);
    gDialogID = 4;
    EXPECT_EQ(pressed_pause(), FALSE);
    gDialogID = -1;
    gWarpTransition.isActive = TRUE;
    EXPECT_EQ(pressed_pause(), FALSE);
    gWarpTransition.isActive = FALSE;
    sDelayedWarpOp = WARP_OP_CREDITS_NEXT;
    EXPECT_EQ(pressed_pause(), FALSE);
    sDelayedWarpOp = WARP_OP_NONE;
    mario.action = ACT_END_PEACH_CUTSCENE;
    EXPECT_EQ(pressed_pause(), TRUE);
    gDialogID = 4;
    EXPECT_EQ(pressed_pause(), FALSE);
    gDialogID = -1;
    gWarpTransition.isActive = TRUE;
    EXPECT_EQ(pressed_pause(), FALSE);
    gWarpTransition.isActive = FALSE;
    sDelayedWarpOp = WARP_OP_CREDITS_NEXT;
    EXPECT_EQ(pressed_pause(), FALSE);
    sDelayedWarpOp = WARP_OP_NONE;
    sPPDebugPage = PUPPYPRINT_PAGE_LEVEL_SELECT;
    EXPECT_EQ(pressed_pause(), FALSE);
    sPPDebugPage = 0;
    gCurrLevelNum = LEVEL_ENDING;
    mario.action = ACT_UNINITIALIZED;
    EXPECT_EQ(pressed_pause(), TRUE);
    controller.buttonPressed = 0;
    EXPECT_EQ(pressed_pause(), FALSE);
    return TRUE;
}
static int peach_departure(void) {
    for (s32 debug = 0; debug < 5; debug++) {
        for (s32 natural = 0; natural < 2; natural++) {
            begin(level_ending_entry);
            gCurrLevelNum = LEVEL_CASTLE_GROUNDS;
            gDebugLevelSelect = debug == 1;
            if (debug == 2 || debug == 3) {
                game_options_set(GO_INFINITE_HEALTH, 1);
                if (debug == 3) {
                    game_options_set(GO_INFINITE_HEALTH, 0);
                }
            }
            if (debug == 4) {
                gDebugLevelSelect = TRUE;
                EXPECT_EQ(game_options_secrets_blocked(), TRUE);
                gDebugLevelSelect = FALSE;
            }
            mario.action = ACT_END_PEACH_CUTSCENE;
            mario.actionState = ACT_STATE_END_PEACH_CUTSCENE_FADE_OUT_WARP;
            gCurrCreditsEntry = sDispCreditsEntry = &sCreditsSequence[0];
            // Grand Star departure reloads castle grounds for Peach's scene.
            game_options_runtime_restart();
            gCutsceneMsgIndex = 0;
            gCutsceneMsgTimer = 20;
            gCutsceneMsgFade = 250;
            gSavedCourseNum = 8;
            gCurrActNum = 6;
            gSpecialTripleJump = TRUE;
            sSoundBanks = 0;
            sWarpCount = sFadeCount = 0;
            if (natural) {
                end_peach_cutscene_fade_out(&mario);
                end_peach_cutscene_fade_out(&mario);
                EXPECT_EQ(mario.actionState, ACT_STATE_END_PEACH_CUTSCENE_FADE_OUT_END);
            } else {
                sCurrPlayMode = PLAY_MODE_PAUSED;
                gMenuOptSelectIndex = MENU_OPT_EXIT_COURSE;
                play_mode_paused();
            }
            EXPECT_EQ(sWarpCount, 1);
            EXPECT_EQ(sFadeCount, 1);
            EXPECT_EQ(sWarpDest.levelNum, debug ? LEVEL_CASTLE_GROUNDS : LEVEL_PHYSICS_LAB);
            EXPECT_EQ(sWarpDest.areaIdx, 1);
            EXPECT_EQ(sWarpDest.nodeId, debug ? 0x32 : WARP_NODE_MAIN_ENTRY);
            EXPECT_EQ(sWarpDest.arg, WARP_FLAG_EXIT_COURSE);
            EXPECT_EQ(sCurrPlayMode, PLAY_MODE_CHANGE_LEVEL);
            EXPECT_EQ(sTransitionTimer, 30);
            EXPECT_EQ(gCurrCreditsEntry == NULL && sDispCreditsEntry == NULL, TRUE);
            EXPECT_EQ(gCutsceneMsgIndex, -1);
            EXPECT_EQ(gCutsceneMsgTimer, 0);
            EXPECT_EQ(gCutsceneMsgFade, 0);
            EXPECT_EQ(gSavedCourseNum, COURSE_NONE);
            EXPECT_EQ(gCurrActNum, 1);
            EXPECT_EQ(gSpecialTripleJump, FALSE);
            EXPECT_EQ(sSoundBanks, SOUND_BANKS_ALL);
        }
    }
    // Preserve ordinary debug and Lab exit destinations.
    mario.action = ACT_IDLE;
    gCurrLevelNum = LEVEL_BOB;
    game_options_runtime_restart();
    EXPECT_EQ(game_options_secrets_blocked(), FALSE);
    gMenuOptSelectIndex = MENU_OPT_EXIT_COURSE;
    gDebugLevelSelect = TRUE;
    play_mode_paused();
    EXPECT_EQ(sSpecialWarpDest, WARP_SPECIAL_LEVEL_SELECT);
    gCurrLevelNum = LEVEL_PHYSICS_LAB;
    play_mode_paused();
    EXPECT_EQ(sWarpDest.levelNum, LEVEL_CASTLE_GROUNDS);
    EXPECT_EQ(sWarpDest.nodeId, 0x32);
    return TRUE;
}
static int yoshi_coin_cake_route(void) {
    const s32 operations[] = { WARP_OP_DEATH, WARP_OP_GAME_OVER };
    for (s32 debug = 0; debug < 2; debug++) {
        for (unsigned i = 0; i < sizeof(operations) / sizeof(*operations); i++) {
            begin(level_ending_entry);
            gCurrLevelNum = LEVEL_CASTLE_GROUNDS;
            gDebugLevelSelect = debug;
            gCurrDemoInput = NULL;
            gCurrCreditsEntry = &sCreditsSequence[0];
            mario.health = 0xff;
            mario.yoshiCoinDeathPending = TRUE;
            sDelayedWarpOp = operations[i];
            sDelayedWarpTimer = 2;
            sWarpDest.type = WARP_TYPE_NOT_WARPING;
            gPuppyWarp = 0;
            initiate_delayed_warp();
            EXPECT_EQ(sWarpDest.type, WARP_TYPE_NOT_WARPING);
            EXPECT_EQ(mario.yoshiCoinDeathPending, TRUE);
            initiate_delayed_warp();
            EXPECT_EQ(sWarpDest.type, WARP_TYPE_CHANGE_LEVEL);
            EXPECT_EQ(sWarpDest.levelNum, LEVEL_ENDING);
            EXPECT_EQ(sWarpDest.areaIdx, 1);
            EXPECT_EQ(mario.yoshiCoinDeathPending, FALSE);
            EXPECT_EQ(sWarpDest.arg, WARP_FLAG_YOSHI_COIN_CAKE);
            EXPECT_EQ(gCurrCreditsEntry == NULL, TRUE);
            frame();
            controller.buttonPressed = START_BUTTON;
            advance(520);
            EXPECT_EQ(sCakeClipStarts, 1);
            EXPECT_EQ(sThankYouSounds, 0);
            EXPECT_EQ(sCurrPlayMode, PLAY_MODE_NORMAL);
            EXPECT_EQ(sCommand != NULL, TRUE);
            EXPECT_EQ(sYoshiCoinCakeActive, TRUE);
            sCakeClipPlaying = FALSE;
            s32 reloads = sSaveReloads;
            advance(520);
            EXPECT_EQ(sRegister, WARP_SPECIAL_MARIO_HEAD_DIZZY);
            EXPECT_EQ(sSaveReloads, reloads + 1);
            EXPECT_EQ(sClears, 1);
            EXPECT_EQ(sYoshiCoinCakeActive, FALSE);
        }
    }
    // An ordinary game over retains its existing menu route.
    begin(level_ending_entry);
    gDebugLevelSelect = FALSE;
    gCurrDemoInput = NULL;
    mario.health = 0xff;
    mario.yoshiCoinDeathPending = FALSE;
    sDelayedWarpOp = WARP_OP_GAME_OVER;
    sDelayedWarpTimer = 1;
    gPuppyWarp = 0;
    initiate_delayed_warp();
    EXPECT_EQ(sSpecialWarpDest, WARP_SPECIAL_MARIO_HEAD_DIZZY);
    sUnlockDeath = TRUE;
    sDelayedWarpOp = WARP_OP_GAME_OVER;
    sDelayedWarpTimer = 1;
    initiate_delayed_warp();
    EXPECT_EQ(sSpecialWarpDest, WARP_SPECIAL_INTRO_SPLASH_SCREEN);
    sUnlockDeath = FALSE;
    // Debug departure stops the clip; the next normal cake can pause again.
    begin(level_ending_entry);
    sWarpDest.arg = WARP_FLAG_YOSHI_COIN_CAKE;
    frame();
    advance(200);
    EXPECT_EQ(sCakeClipPlaying, TRUE);
    gPuppyWarp = LEVEL_CASTLE_GROUNDS;
    gPuppyWarpArea = 1;
    advance(3);
    EXPECT_EQ(sCakeClipStops, 1);
    EXPECT_EQ(sYoshiCoinCakeActive, FALSE);
    begin(level_ending_entry);
    frame();
    advance(200);
    EXPECT_EQ(sThankYouSounds, 1);
    controller.buttonPressed = START_BUTTON;
    frame();
    EXPECT_EQ(sCurrPlayMode, PLAY_MODE_PAUSED);
    return TRUE;
}

static int credits_cake_route(void) {
    for (s32 debug = 0; debug < 2; debug++) {
        begin(level_ending_entry);
        gCurrLevelNum = LEVEL_CASTLE_GROUNDS;
        gDebugLevelSelect = debug;
        gCurrCreditsEntry = &sCreditsSequence[0];
        gCurrDemoInput = NULL;
        sDelayedWarpOp = WARP_OP_CREDITS_END;
        sDelayedWarpTimer = 1;
        sWarpDest.type = WARP_TYPE_NOT_WARPING;
        sSpecialWarpDest = WARP_SPECIAL_NONE;
        gPuppyWarp = 0;
        initiate_delayed_warp();
        EXPECT_EQ(sWarpDest.type, WARP_TYPE_CHANGE_LEVEL);
        EXPECT_EQ(sWarpDest.levelNum, LEVEL_ENDING);
        EXPECT_EQ(sWarpDest.areaIdx, 1);
        EXPECT_EQ(sSpecialWarpDest, WARP_SPECIAL_NONE);
        EXPECT_EQ(gCurrCreditsEntry == NULL, TRUE);
        frame();
        EXPECT_EQ(sDelayedWarpOp, WARP_OP_NONE);
        advance(520);
        EXPECT_EQ(sCommand == NULL, TRUE);
        EXPECT_EQ(sRegister, debug ? LEVEL_CASTLE_GROUNDS : LEVEL_PHYSICS_LAB);
        EXPECT_EQ(sClears, 1);
    }
    return TRUE;
}
static int ending_menu_and_captions(void) {
    for (s32 cake = 0; cake < 2; cake++) {
        begin(level_ending_entry);
        gCurrLevelNum = cake ? LEVEL_ENDING : LEVEL_CASTLE_GROUNDS;
        gCurrCourseNum = cake ? COURSE_CAKE_END : COURSE_NONE;
        mario.action = cake ? ACT_IDLE : ACT_END_PEACH_CUTSCENE;
        sCurrPlayMode = PLAY_MODE_PAUSED;
        gDialogBoxState = DIALOG_STATE_OPENING;
        sCameraChanges = sScoreDraws = sMenuMax = 0;
        gGlobalTimer = 100;
        controller.buttonPressed = controller.buttonDown = START_BUTTON;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        EXPECT_EQ(gDialogBoxState, DIALOG_STATE_VERTICAL);
        EXPECT_EQ(sCameraChanges, 0);
        EXPECT_EQ(sScoreDraws, 0);
        EXPECT_EQ(sMenuMax, 4); /* Continue, Exit Course, Statistics and Options. */
        controller.buttonPressed = controller.buttonDown = 0;
        gGlobalTimer++;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        gDialogLineNum = MENU_OPT_CAMERA_ANGLE_R;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        EXPECT_EQ(gDialogLineNum, MENU_OPT_DEFAULT);
        gDialogLineNum = MENU_OPT_EXIT_COURSE;
        gGlobalTimer = 100 + PAUSE_MENU_UNPAUSE_DELAY_FRAMES;
        controller.buttonPressed = controller.buttonDown = A_BUTTON;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_EXIT_COURSE);
        EXPECT_EQ(sCameraChanges, 0);
        EXPECT_EQ(sScoreDraws, 0);
    }
    gCurrLevelNum = LEVEL_CASTLE_GROUNDS;
    mario.action = ACT_END_PEACH_CUTSCENE;
    gCutsceneMsgIndex = 0;
    gCutsceneMsgTimer = 10;
    gCutsceneMsgFade = 250;
    gCutsceneMsgDuration = 50;
    sCaptionDraws = 0;
    sCurrPlayMode = PLAY_MODE_PAUSED;
    for (s32 i = 0; i < 100; i++) {
        do_cutscene_handler();
    }
    EXPECT_EQ(gCutsceneMsgTimer, 10);
    EXPECT_EQ(gCutsceneMsgFade, 250);
    EXPECT_EQ(sCaptionDraws, 0);
    sCurrPlayMode = PLAY_MODE_NORMAL;
    // Pre-held A does not fast-forward timed cutscene captions.
    controller.buttonDown = A_BUTTON;
    controller.buttonPressed = 0;
    do_cutscene_handler();
    EXPECT_EQ(gCutsceneMsgTimer, 11);
    EXPECT_EQ(sCaptionDraws, 1);
    for (s32 frame = 0; frame < 5; frame++) {
        do_cutscene_handler();
    }
    EXPECT_EQ(gCutsceneMsgTimer, 16);
    EXPECT_EQ(gCutsceneMsgIndex, 0);
    s32 captionDraws = sCaptionDraws;
    clear_cutscene_message();
    do_cutscene_handler();
    EXPECT_EQ(sCaptionDraws, captionDraws);
    return TRUE;
}
static int final_grand_star(void) {
    const s32 levels[] = { LEVEL_BOWSER_1, LEVEL_BOWSER_2, LEVEL_BOWSER_3, LEVEL_BOB };
    for (s32 i = 0; i < 4; i++) {
        for (s32 grand = 0; grand < 2; grand++) {
            for (s32 alive = 0; alive < 2; alive++) {
                mario = (struct MarioState){ .health = alive ? 0x880 : 0xFF,
                                             .action = ACT_IDLE,
                                             .marioObj = &marioObject };
                starObject =
                    (struct Object){ .oInteractionSubtype = grand ? INT_SUBTYPE_GRAND_STAR : 0 };
                gCurrLevelNum = levels[i];
                sCompletionWrites = 0;
                EXPECT_EQ(interact_star_or_key(&mario, 0, &starObject), alive);
                EXPECT_EQ(mario.health, alive ? 0x880 : 0xff);
                EXPECT_EQ(mario.healCounter, alive ? 31 : 0);
                EXPECT_EQ(sCompletionWrites, alive && grand && levels[i] == LEVEL_BOWSER_3);
                if (alive && grand) {
                    EXPECT_EQ(mario.action, ACT_JUMBO_STAR_CUTSCENE);
                }
            }
        }
    }
    return TRUE;
}
static int options_pause_navigation(void) {
    for (s32 location = 0; location < 3; location++) {
        begin(level_ending_entry);
        gCurrLevelNum = location == 0 ? LEVEL_BOB : location == 1 ? LEVEL_CASTLE_GROUNDS : LEVEL_ENDING;
        gCurrCourseNum = location == 0 ? COURSE_MIN : location == 1 ? COURSE_NONE : COURSE_CAKE_END;
        mario.action = ACT_IDLE;
        gMenuMode = MENU_MODE_RENDER_PAUSE_SCREEN;
        gDialogBoxState = DIALOG_STATE_OPENING;
        gGlobalTimer = 100;
        controller.buttonPressed = controller.buttonDown = START_BUTTON;
        controller.rawStickY = location == 0 ? -80 : 0;
        controller.rawStickX = location == 1 ? 80 : 0;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        EXPECT_EQ(sMenuNavigationStick, location == 2 ? 0 : GO_MENU_WAIT_FOR_NEUTRAL);
        EXPECT_EQ(sPauseCastleOptionsStick, location == 2 ? 0 : GO_MENU_WAIT_FOR_NEUTRAL);
        controller.rawStickX = controller.rawStickY = 0;
        gGlobalTimer += PAUSE_MENU_UNPAUSE_DELAY_FRAMES;
        controller.buttonPressed = controller.buttonDown = B_BUTTON;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        EXPECT_EQ(gOptionsMenu.open, TRUE);
        EXPECT_EQ(gMenuMode, MENU_MODE_RENDER_PAUSE_SCREEN);
        gGlobalTimer++;
        controller.buttonPressed = controller.buttonDown = A_BUTTON;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        EXPECT_EQ(gOptionsMenu.category, GO_CONTROLS);
        EXPECT_EQ(gMenuMode, MENU_MODE_RENDER_PAUSE_SCREEN);
        EXPECT_EQ(game_options_get(GO_PAUSE_DISCONNECT), TRUE);
        gGlobalTimer++;
        controller.buttonPressed = controller.buttonDown = R_TRIG;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        EXPECT_EQ(gOptionsMenu.category, GO_PRESENTATION);
        gOptionsMenu.selected = 0;
        EXPECT_EQ(gOptionsMenu.ids[0], GO_MASTER_VOLUME);
        EXPECT_EQ(game_options_set(GO_MASTER_VOLUME, 20), TRUE);
        gGlobalTimer++;
        controller.buttonPressed = controller.buttonDown = Z_TRIG;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        EXPECT_EQ(game_options_get(GO_MASTER_VOLUME), 100);
        gGlobalTimer++;
        controller.buttonPressed = controller.buttonDown = L_TRIG;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        EXPECT_EQ(gOptionsMenu.category, GO_CONTROLS);
        gGlobalTimer++;
        controller.buttonPressed = controller.buttonDown = A_BUTTON;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        EXPECT_EQ(game_options_get(GO_PAUSE_DISCONNECT), FALSE);
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        EXPECT_EQ(game_options_get(GO_PAUSE_DISCONNECT), FALSE);
        gGlobalTimer++;
        controller.buttonPressed = controller.buttonDown = B_BUTTON;
        // Old pause input was released while Options owned navigation.
        sMenuNavigationDirection = GO_MENU_WAIT_FOR_NEUTRAL | 2;
        sPauseCastleOptionsHeld = gMenuHoldKeyIndex = 2;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        EXPECT_EQ(gOptionsMenu.open, FALSE);
        EXPECT_EQ(gMenuMode, MENU_MODE_RENDER_PAUSE_SCREEN);
        EXPECT_EQ(game_options_menu_direction(2, &sMenuNavigationDirection), 2);
        EXPECT_EQ(sPauseCastleOptionsHeld | gMenuHoldKeyIndex, 0);
        sPauseStatisticsOpen = TRUE;
        sCloseStatistics = TRUE;
        sMenuNavigationDirection = GO_MENU_WAIT_FOR_NEUTRAL | 2;
        sPauseCastleOptionsHeld = gMenuHoldKeyIndex = 2;
        gGlobalTimer++;
        controller.rawStickY = -80;
        controller.buttonPressed = controller.buttonDown = B_BUTTON;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        EXPECT_EQ(sPauseStatisticsOpen, FALSE);
        EXPECT_EQ(game_options_menu_direction(2, &sMenuNavigationDirection), 2);
        EXPECT_EQ(sPauseCastleOptionsHeld | gMenuHoldKeyIndex, 0);
        // Returning with the stick held cannot move either pause layout until release.
        EXPECT_EQ(game_options_menu_stick(2, &sMenuNavigationStick), 0);
        EXPECT_EQ(game_options_menu_stick(2, &sPauseCastleOptionsStick), 0);
        EXPECT_EQ(game_options_menu_stick(0, &sMenuNavigationStick), 0);
        EXPECT_EQ(game_options_menu_stick(0, &sPauseCastleOptionsStick), 0);
        EXPECT_EQ(game_options_menu_stick(2, &sMenuNavigationStick), 2);
        EXPECT_EQ(game_options_menu_stick(2, &sPauseCastleOptionsStick), 2);
        controller.rawStickY = 0;
        sCloseStatistics = FALSE;
        gDialogLineNum = MENU_OPT_OPTIONS;
        sPauseCastleSelection = MENU_OPT_OPTIONS;
        gGlobalTimer++;
        controller.buttonPressed = controller.buttonDown = A_BUTTON;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_NONE);
        EXPECT_EQ(gOptionsMenu.open, TRUE);
        EXPECT_EQ(gMenuMode, MENU_MODE_RENDER_PAUSE_SCREEN);
        gGlobalTimer++;
        controller.buttonPressed = controller.buttonDown = START_BUTTON;
        EXPECT_EQ(render_pause_courses_and_castle(), MENU_OPT_DEFAULT);
        EXPECT_EQ(gMenuMode, MENU_MODE_NONE);
    }
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
#define RUN(name, fn)                                                                                  \
    do {                                                                                               \
        if (test_selected("end_screen." name)) {                                                       \
            test_report("end_screen." name, fn());                                                     \
        }                                                                                              \
    } while (0)
    RUN("debug_departure", debug_departure);
    RUN("automatic_departure", automatic_departure);
    RUN("pause_countdown", pause_countdown);
    RUN("pause_policy", ending_pause_policy);
    RUN("peach_departure", peach_departure);
    RUN("credits_cake_route", credits_cake_route);
    RUN("yoshi_coin_cake_route", yoshi_coin_cake_route);
    RUN("menu_captions", ending_menu_and_captions);
    RUN("options_pause", options_pause_navigation);
    RUN("final_grand_star", final_grand_star);
    return test_finish();
}
