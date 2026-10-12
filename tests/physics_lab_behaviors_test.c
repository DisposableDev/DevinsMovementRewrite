#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#define WIDE
#define REONUCAM
#include <stdlib.h>
#include <string.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
u32 gGlobalTimer;
typedef float f32;
typedef short Collision;
const Collision physics_lab_shuttle_collision[] = { 1 };
const Collision physics_lab_disk_collision[] = { 2 };
const Collision physics_lab_bridge_collision[] = { 3 };
const Collision physics_lab_closing_wall_collision[] = { 4 };
struct Object {
    int oBehParams2ndByte, oTimer, oAction, oMoveAngleYaw, oInteractStatus;
    int oTreasureChestAboveWater, oTreasureChestNumOpenedChests, oTreasureChestDoCloseChests;
    float oPosX, oPosY, oPosZ, oHomeX, oHomeY, oVelX, oVelY, oDistanceToMario;
    struct Object *parentObj;
    struct {
        struct {
            int angle[3];
        } gfx;
    } header;
    int oFaceAngleRoll, oFaceAngleYaw, oAngleVelYaw, oAngleVelRoll;
    float oCollisionDistance, oDrawingDistance;
    const Collision *collisionData;
} fixture, *o = &fixture;
static const void *segmented_to_virtual(const void *p) {
    return p;
}
static float sins(s16 a) {
    return sinf(a * (6.283185307179586f / 65536));
}
static float coss(s16 a) {
    return cosf(a * (6.283185307179586f / 65536));
}

enum {
    FALSE = 0,
    TRUE = 1,
    COURSE_NONE = 0,
    COURSE_MIN = 1,
    COURSE_CAKE_END = 25,
    LEVEL_PHYSICS_LAB = 32,
    LEVEL_CASTLE_GROUNDS = 16,
    LEVEL_CASTLE = 6,
    LEVEL_BOB = 9,
    MENU_OPT_NONE = 0,
    MENU_OPT_DEFAULT = 1,
    MENU_OPT_EXIT_COURSE = 2,
    MENU_OPT_STATISTICS = 4,
    MENU_OPT_OPTIONS = 5,
    MENU_MODE_NONE = -1,
    MENU_MODE_RENDER_PAUSE_SCREEN = 0,
    DIALOG_STATE_OPENING = 0,
    DIALOG_STATE_VERTICAL = 1,
    DIALOG_STATE_HORIZONTAL = 2,
    SOUND_MENU_PAUSE_OPEN = 1,
    SOUND_MENU_PAUSE_CLOSE = 2,
    SOUND_MENU_PAUSE_CHANGE_SELECT = 3,
    A_BUTTON = 0x8000,
    B_BUTTON = 0x4000,
    START_BUTTON = 0x1000,
    CAM_MOVE_PAUSE_SCREEN = 1,
    PLAY_MODE_NORMAL = 0,
    PLAY_MODE_CHANGE_AREA = 1,
    WARP_SPECIAL_LEVEL_SELECT = -9,
    WARP_SPECIAL_NONE = 0,
    WARP_NODE_DEATH = 0xF1,
    WARP_FLAGS_NONE = 0,
    WARP_FLAG_EXIT_COURSE = 0x10,
    WARP_TYPE_CHANGE_LEVEL = 1,
    EXIT_COURSE_LEVEL = LEVEL_CASTLE,
    EXIT_COURSE_AREA = 1,
    EXIT_COURSE_NODE = 0x1F,
};
enum {
    MODEL_TREASURE_CHEST_BASE = 1,
    TREASURE_CHEST_BOTTOM_ACT_CLOSE = 0,
    TREASURE_CHEST_BOTTOM_ACT_OPENING,
    TREASURE_CHEST_BOTTOM_ACT_OPENED,
    SOUND_GENERAL2_RIGHT_ANSWER,
    SOUND_MENU_CAMERA_BUZZ,
    INT_STATUS_NONE = 0
};
#define TREASURE_CHEST_OPEN_RADIUS 150.0f
static int bhvTreasureChestBottom[1], chestSpawns, jingles, shocks, tangible, nearChest;
static struct Object chests[2], marioObject, *gMarioObject = &marioObject;
static struct Object *spawn_object_relative(int param, int x, int y, int z, struct Object *parent,
                                            int model, const int *behavior) {
    assert(chestSpawns < 2 && model == MODEL_TREASURE_CHEST_BASE && behavior == bhvTreasureChestBottom);
    chests[chestSpawns++] = (struct Object){ .oBehParams2ndByte = param,
                                             .parentObj = parent,
                                             .oAction = TREASURE_CHEST_BOTTOM_ACT_CLOSE,
                                             .oPosX = parent->oPosX + x,
                                             .oPosY = parent->oPosY + y,
                                             .oPosZ = parent->oPosZ + z };
    return &chests[chestSpawns - 1];
}
static void play_puzzle_jingle(void) {
    jingles++;
}
static int is_point_within_radius_of_mario(float x, float y, float z, float r) {
    return nearChest;
}
static int obj_check_if_facing_toward_angle(int a, int b, int c) {
    return TRUE;
}
static void cur_obj_become_tangible(void) {
    tangible = TRUE;
    shocks++;
}
static void cur_obj_become_intangible(void) {
    tangible = FALSE;
}
static void cur_obj_push_mario_away_from_cylinder(float a, float b) {
}
struct Controller {
    unsigned buttonPressed;
    unsigned buttonDown;
    s16 rawStickX, rawStickY;
} controller, *gPlayer1Controller = &controller;
static void *gCurrDemoInput, *gCurrCreditsEntry;
static u8 sPauseTasCodeProgress;
// TAS shortcut services are outside this fixture's motion/pause-exit coverage.
static s32 mario_character_update_tas_code(u8 *progress, u16 buttons) {
    return FALSE;
}
static void lvl_prepare_tas_thicc_start(void) {
    assert(FALSE);
}
static void lvl_restore_cap_on_file_start(s32 arg, s32 level) {
    assert(FALSE);
}
static void lvl_init_from_save_file(s32 arg, s32 level) {
    assert(FALSE);
}
static void reset_dialog_render_state(void) {
    assert(FALSE);
}
struct ObjectWarpNode {
    struct {
        int destLevel, destArea, destNode;
    } node;
} deathNode = { { LEVEL_BOB, 2, 10 } };
struct {
    int type, level, area, node, flags;
} sWarpDest;
int gCurrCourseNum, gCurrLevelNum, gSavedCourseNum, gDialogBoxState, gDialogLineNum;
int gDialogTextAlpha, gMenuMode, gMenuOptSelectIndex, gDebugLevelSelect, gCameraMovementFlags;
int gGlobalSoundSource, scoreReads, redCoinReads, lastFade, playMode, castleReads, openingTextAlpha;
static u8 pauseShadeAlpha;
static int settingsDraws, settingsInputs;
static u32 sPauseMenuOpenedFrame;
static s32 sPauseMenuStartRepressed;
static s32 sPauseMenuARepressed;
static s32 sPauseStatisticsOpen;
static s8 sPauseCastleSelection;
static u8 sPauseCastleOptionsHeld;
static u8 sPauseCastleOptionsStick;
static u8 sMenuNavigationStick, sMenuNavigationDirection, gMenuHoldKeyIndex;
static u32 sPauseCastleOptionsFrame;
static int statisticsOpened, statisticsDrawn, chooseCastleStatistics;
// Options/storage/camera rendering have dedicated production fixtures. Keep
// their service boundaries observable here without replacing pause routing.
static struct {
    s32 open, saveFailed;
} gOptionsMenu;
static void game_options_menu_close(void) {
    gOptionsMenu.open = FALSE;
}
static void game_options_menu_open(void) {
    gOptionsMenu.open = TRUE;
}
static s32 game_options_menu_update_controller(void) {
    if (controller.buttonPressed & B_BUTTON) {
        game_options_menu_close();
        return TRUE;
    }
    return FALSE;
}
static void game_options_menu_render(void) {
}
static s16 pause_menu_center_x(void) {
    return 160;
}
#define gSPDisplayList(...) ((void) 0)
#define gDPSetEnvColor(...) ((void) 0)
static void player_stats_menu_text(s16 x, s16 y, const char *text) {
    assert(gOptionsMenu.saveFailed && strcmp(text, "Settings not saved - B: Retry") == 0);
}
static void update_paused_camera(void) {
}
static s32 game_options_secrets_blocked(void) {
    return FALSE;
}
static s32 game_options_active_file(void) {
    return 0;
}
static void game_options_save(s32 file) {
    // The TAS-code stub rejects activation, so it must not save a file here.
    assert(FALSE);
}
static s32 endedCourseTimers;
static void game_timing_end_course(void) {
    endedCourseTimers++;
}
static void player_stats_menu_open(void) {
    statisticsOpened++;
}
static s32 player_stats_menu_update(void) {
    return (controller.buttonPressed & B_BUTTON) != 0;
}
static void player_stats_menu_render(void) {
    statisticsDrawn++;
}
static void player_stats_warp(int level, int physicsLab) {
}
static void render_pause_castle_options(void) {
    if (chooseCastleStatistics) {
        sPauseCastleSelection = MENU_OPT_STATISTICS;
    }
}
static s32 level_is_ending(void) {
    return FALSE;
}
static void level_warp_to_physics_lab(void) {
    assert(FALSE);
}
static void render_widescreen_setting(s32 allowInput) {
    settingsDraws++;
    settingsInputs += allowInput;
}
static void render_reonucam_speed_setting(s32 allowInput) {
    settingsDraws++;
    settingsInputs += allowInput;
}
static void level_set_transition(int a, void *b) {
}
static void play_sound(int a, int b) {
}
static void change_dialog_camera_angle(void) {
}
static void highlight_last_course_complete_stars(void) {
}
static void shade_screen(void) {
    pauseShadeAlpha = 127;
}
static void render_pause_my_score_coins(void) {
    assert(gCurrCourseNum >= COURSE_MIN);
    scoreReads++;
}
static void render_pause_red_coins(void) {
    redCoinReads++;
}
static void print_hud_pause_colorful_str(void) {
}
static void render_pause_course_options(int a, int b, int *c, int d) {
    openingTextAlpha = gDialogTextAlpha;
}
static void render_pause_castle_menu_box(int a, int b) {
}
static void render_pause_castle_main_strings(int a, int b) {
    castleReads++;
}
static void set_menu_mode(int mode) {
    gMenuMode = mode;
}
static void hud_reveal_after_pause(void) {
}
// Camera persistence is an engine service outside these menu-routing checks.
static void save_reonucam_speed_on_unpause(void) {
}
static void raise_background_noise(int a) {
}
static void set_play_mode(int mode) {
    playMode = mode;
}
static void fade_into_special_warp(int dest, int arg) {
    lastFade = dest;
}
static void initiate_warp(int level, int area, int node, int flags) {
    sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
    sWarpDest.level = level;
    sWarpDest.area = area;
    sWarpDest.node = node;
    sWarpDest.flags = flags;
}
#ifdef DEATH_ON_EXIT_COURSE
static struct ObjectWarpNode *area_get_warp_node(int id) {
    assert(id == WARP_NODE_DEATH);
    return &deathNode;
}
#define assertf(test, ...) assert(test)
#endif

/* Pause audio/timing is exercised separately by the pause easter egg fixture. */
static void pause_easter_egg_update(void) {
}
#include "lab_actual.inc.c"

static void test_chest_cycles(void) {
    for (int wet = 0; wet < 2; wet++) {
        o = &fixture;
        memset(o, 0, sizeof(*o));
        o->oBehParams2ndByte = !wet;
        o->oPosX = 4500;
        o->oPosY = 120;
        o->oPosZ = 13100;
        chestSpawns = jingles = shocks = 0;
        bhv_physics_lab_chests_init();
        assert(chestSpawns == 2);
        for (int i = 0; i < 2; i++) {
            assert((uint16_t) chests[i].oFaceAngleYaw == 0x8000);
            assert((uint16_t) chests[i].oMoveAngleYaw == 0x8000);
            assert(chests[i].oPosX == fixture.oPosX + (i == 0 ? -400 : 400));
        }
        assert(fixture.oTreasureChestAboveWater == !wet);
        assert(chests[0].oPosX == 4100 && chests[1].oPosX == 4900);
        for (int cycle = 0; cycle < 3; cycle++) {
            // Wrong order produces one shock; leaving unlocks the pair again.
            nearChest = TRUE;
            o = &chests[1];
            bhv_treasure_chest_bottom_loop();
            assert(tangible && fixture.oTreasureChestDoCloseChests);
            assert(fixture.oTreasureChestNumOpenedChests == 1);
            nearChest = FALSE;
            bhv_treasure_chest_bottom_loop();
            assert(!tangible && !fixture.oTreasureChestDoCloseChests);
            nearChest = TRUE;
            for (int i = 0; i < 2; i++) {
                o = &chests[i];
                bhv_treasure_chest_bottom_loop();
            }
            assert(fixture.oTreasureChestNumOpenedChests == 3);
            o = &fixture;
            bhv_physics_lab_chests_loop();
            assert(fixture.oAction == 1);
            fixture.oTimer = 120;
            fixture.oDistanceToMario = 0;
            bhv_physics_lab_chests_loop();
            assert(fixture.oAction == 1); // no reset while Mario is still using it
            fixture.oDistanceToMario = 1200;
            bhv_physics_lab_chests_loop();
            assert(fixture.oAction == 2 && fixture.oTreasureChestDoCloseChests);
            nearChest = FALSE;
            for (int i = 0; i < 2; i++) {
                o = &chests[i];
                bhv_treasure_chest_bottom_loop();
                assert(o->oAction == TREASURE_CHEST_BOTTOM_ACT_CLOSE);
            }
            o = &fixture;
            fixture.oTimer = 31;
            bhv_physics_lab_chests_loop();
            assert(fixture.oAction == 0 && !fixture.oTreasureChestDoCloseChests);
        }
        assert(jingles == 3 && shocks == 3);
    }
    o = &fixture;
}

static void near(float actual, float expected) {
    assert(fabsf(actual - expected) < .02f);
}
static void init_mover(int type) {
    memset(o, 0, sizeof(*o));
    o->oBehParams2ndByte = type;
    o->oPosX = o->oHomeX = 9800;
    o->oPosY = o->oHomeY = 140;
    bhv_physics_lab_mover_init();
}

static void test_motion(void) {
    // Check four complete cycles, including wrap frames, extrema and velocity.
    for (int type = LAB_MOVER_SHUTTLE; type <= LAB_MOVER_WALL_RIGHT; type++) {
        init_mover(type);
        const Collision *expected = physics_lab_shuttle_collision;
        if (type == LAB_MOVER_DISK) {
            expected = physics_lab_disk_collision;
        }
        if (type == LAB_MOVER_BRIDGE) {
            expected = physics_lab_bridge_collision;
        }
        if (type >= LAB_MOVER_WALL_LEFT) {
            expected = physics_lab_closing_wall_collision;
        }
        assert(o->collisionData == expected);
        for (int frame = 0; frame <= 1024; frame++) {
            float oldX = o->oPosX, oldY = o->oPosY;
            o->oTimer = frame;
            bhv_physics_lab_mover_loop();
            near(o->oVelX, o->oPosX - oldX);
            near(o->oVelY, o->oPosY - oldY);
            if (frame > 0) {
                assert(fabsf(o->oVelX) < 23);
                assert(fabsf(o->oVelY) < 9);
            }
            if (type == LAB_MOVER_SHUTTLE) {
                assert(o->oPosX >= 8900 && o->oPosX <= 10700);
                if (frame % 256 == 64) {
                    near(o->oPosX, 10700);
                }
                if (frame % 256 == 192) {
                    near(o->oPosX, 8900);
                }
                if (frame % 128 == 0) {
                    near(o->oPosX, 9800);
                }
            } else if (type == LAB_MOVER_DISK) {
                assert(o->oAngleVelYaw == 128);
                if (frame % 512 == 0) {
                    assert((uint16_t) o->oFaceAngleYaw == 0);
                }
                if (frame % 512 == 256) {
                    assert((uint16_t) o->oFaceAngleYaw == 32768);
                }
            } else if (type == LAB_MOVER_BRIDGE) {
                assert(abs(o->oFaceAngleRoll) <= 1792);
                assert(abs(o->oAngleVelRoll) <= 45);
                if (frame % 256 == 64) {
                    assert(o->oFaceAngleRoll == 1792);
                }
                if (frame % 256 == 192) {
                    assert(o->oFaceAngleRoll == -1792);
                }
            } else if (type == LAB_MOVER_LIFT) {
                assert(o->oPosY >= 140 && o->oPosY <= 820);
                if (frame % 256 == 0) {
                    near(o->oPosY, 140);
                }
                if (frame % 256 == 128) {
                    near(o->oPosY, 820);
                }
            } else {
                float offset = o->oPosX - o->oHomeX;
                assert(type == LAB_MOVER_WALL_LEFT ? offset < 0 : offset > 0);
                // Mirror pair leaves a 100..1100-unit gap between inner faces.
                float gap = 2 * fabsf(offset) - 100;
                assert(gap >= 100 && gap <= 1100);
                if (frame % 256 == 0) {
                    near(gap, 1100);
                }
                if (frame % 256 == 128) {
                    near(gap, 100);
                }
            }
        }
    }
}

static void test_pause_and_exit(void) {
    gCurrLevelNum = LEVEL_PHYSICS_LAB;
    gCurrCourseNum = COURSE_NONE;
    gDialogBoxState = DIALOG_STATE_OPENING;
    controller.buttonPressed = START_BUTTON;
    controller.buttonDown = START_BUTTON;
    assert(render_pause_courses_and_castle() == MENU_OPT_NONE);
    assert(gDialogBoxState == DIALOG_STATE_VERTICAL);
    assert(openingTextAlpha == 250 && pauseShadeAlpha == 127);
    gDialogLineNum = MENU_OPT_EXIT_COURSE;
    gGlobalTimer += 8;
    controller.buttonPressed = START_BUTTON;
    assert(render_pause_courses_and_castle() == MENU_OPT_DEFAULT);
    assert(scoreReads == 0 && redCoinReads == 0);
    controller.buttonPressed = 0;
    controller.buttonDown = 0;
    render_pause_courses_and_castle();
    gGlobalTimer += 8;
    gDialogLineNum = MENU_OPT_EXIT_COURSE;
    controller.buttonPressed = A_BUTTON;
    assert(render_pause_courses_and_castle() == MENU_OPT_EXIT_COURSE);
    for (int debug = 0; debug < 2; debug++) {
        gDebugLevelSelect = debug;
        gSavedCourseNum = 12;
        gMenuOptSelectIndex = MENU_OPT_EXIT_COURSE;
        gCameraMovementFlags = CAM_MOVE_PAUSE_SCREEN;
        play_mode_paused();
        assert(sWarpDest.level == LEVEL_CASTLE_GROUNDS && sWarpDest.area == 1
               && sWarpDest.node == 0x32);
        assert(sWarpDest.flags == WARP_FLAG_EXIT_COURSE);
        assert(lastFade == WARP_SPECIAL_NONE && gSavedCourseNum == COURSE_NONE);
        assert(!(gCameraMovementFlags & CAM_MOVE_PAUSE_SCREEN));
    }
    // Ordinary courses retain their existing score menu and exit configuration.
    gCurrLevelNum = LEVEL_BOB;
    gCurrCourseNum = 1;
    gDialogBoxState = DIALOG_STATE_OPENING;
    controller.buttonPressed = 0;
    render_pause_courses_and_castle();
    assert(gDialogBoxState == DIALOG_STATE_VERTICAL && scoreReads == 1 && redCoinReads == 1);
    gDebugLevelSelect = 1;
    int endedTimers = endedCourseTimers;
    play_mode_paused();
    assert(lastFade == WARP_SPECIAL_LEVEL_SELECT);
    assert(endedCourseTimers == endedTimers + 1);
    gDebugLevelSelect = 0;
    play_mode_paused();
#ifdef DEATH_ON_EXIT_COURSE
    assert(sWarpDest.level == LEVEL_BOB && sWarpDest.area == 2 && sWarpDest.node == 10
           && sWarpDest.flags == 0);
#else
    assert(sWarpDest.level == EXIT_COURSE_LEVEL && sWarpDest.node == EXIT_COURSE_NODE);
#endif
    gCurrLevelNum = LEVEL_CASTLE;
    gCurrCourseNum = COURSE_NONE;
    gDialogBoxState = DIALOG_STATE_OPENING;
    render_pause_courses_and_castle();
    assert(gDialogBoxState == DIALOG_STATE_HORIZONTAL && castleReads == 1);
}

static void test_pause_minimum_duration(void) {
    const int levels[] = { LEVEL_PHYSICS_LAB, LEVEL_BOB, LEVEL_CASTLE };
    gGlobalTimer = UINT32_MAX - 2;
    for (int page = 0; page < 3; page++) {
        gCurrLevelNum = levels[page];
        gCurrCourseNum = levels[page] == LEVEL_BOB ? COURSE_MIN : COURSE_NONE;
        for (int cycle = 0; cycle < 3; cycle++) {
            for (int scenario = 0; scenario < 8; scenario++) {
                gDialogBoxState = DIALOG_STATE_OPENING;
                gMenuMode = MENU_MODE_RENDER_PAUSE_SCREEN;
                controller.buttonPressed = START_BUTTON | (scenario == 7 ? A_BUTTON : 0);
                controller.buttonDown = START_BUTTON | (scenario >= 5 && scenario != 6 ? A_BUTTON : 0);
                assert(render_pause_courses_and_castle() == MENU_OPT_NONE);
                assert(gDialogTextAlpha == 250 && pauseShadeAlpha == 127);
                gDialogLineNum = MENU_OPT_EXIT_COURSE;
                // Repeated draws cannot arm the opening press or age the delay.
                for (int draw = 0; draw < 20; draw++) {
                    assert(render_pause_courses_and_castle() == MENU_OPT_NONE);
                    assert(gDialogTextAlpha == 250 && pauseShadeAlpha == 127);
                }
                for (int frame = 1; frame <= 8; frame++) {
                    gGlobalTimer++;
                    controller.buttonPressed = 0;
                    if (scenario == 0) {
                        // Holding the original opening press never resumes.
                        controller.buttonDown = START_BUTTON;
                    } else if (scenario == 5 || scenario == 7) {
                        // Gameplay holds and opening-update presses cannot arm A.
                        controller.buttonDown = A_BUTTON;
                    } else if (scenario == 3 || scenario == 6) {
                        // A new pause-menu press confirms at the boundary only if held.
                        controller.buttonDown = frame >= 2 ? A_BUTTON : 0;
                        controller.buttonPressed = frame == 2 ? A_BUTTON : 0;
                        if (scenario == 6 && frame >= 4) {
                            controller.buttonDown = 0;
                        }
                    } else {
                        controller.buttonDown = frame >= 2 ? START_BUTTON : 0;
                        controller.buttonPressed = frame == 2 ? START_BUTTON : 0;
                        if (scenario == 2 && frame >= 4) {
                            // Releasing a re-press cancels held-button resume.
                            controller.buttonDown = 0;
                        }
                        if (scenario == 4 && frame == 8) {
                            // Held Start still wins over a fresh A on Exit Course.
                            controller.buttonPressed = A_BUTTON;
                            controller.buttonDown |= A_BUTTON;
                        }
                    }
                    int closes = frame == 8 && (scenario == 1 || scenario == 3 || scenario == 4);
                    int expected = closes ? MENU_OPT_DEFAULT : MENU_OPT_NONE;
                    if (closes && scenario == 3 && page != 2) {
                        expected = MENU_OPT_EXIT_COURSE;
                    }
                    int previousDraws = settingsDraws;
                    int previousInputs = settingsInputs;
                    assert(render_pause_courses_and_castle() == expected);
                    assert(settingsDraws == previousDraws + 2);
                    assert(settingsInputs == previousInputs + (closes ? 0 : 2));
                    assert(gMenuMode == (closes ? MENU_MODE_NONE : MENU_MODE_RENDER_PAUSE_SCREEN));
                    assert(gDialogTextAlpha == 250 && pauseShadeAlpha == 127);
                }
                if (scenario != 1 && scenario != 3 && scenario != 4) {
                    gGlobalTimer++;
                    controller.buttonPressed = scenario >= 5 ? A_BUTTON : START_BUTTON;
                    controller.buttonDown = controller.buttonPressed;
                    int expected = page != 2 && scenario >= 5 ? MENU_OPT_EXIT_COURSE : MENU_OPT_DEFAULT;
                    int previousDraws = settingsDraws;
                    int previousInputs = settingsInputs;
                    assert(render_pause_courses_and_castle() == expected);
                    assert(settingsDraws == previousDraws + 2 && settingsInputs == previousInputs);
                    assert(gMenuMode == MENU_MODE_NONE);
                    assert(gDialogTextAlpha == 250 && pauseShadeAlpha == 127);
                }
            }
        }
    }
}

static void test_statistics_pause_controls(void) {
    for (int castle = 0; castle < 2; castle++) {
        gCurrCourseNum = castle ? COURSE_NONE : COURSE_MIN;
        gCurrLevelNum = castle ? LEVEL_CASTLE : LEVEL_BOB;
        gDialogBoxState = DIALOG_STATE_OPENING;
        gMenuMode = MENU_MODE_RENDER_PAUSE_SCREEN;
        gGlobalTimer += 100;
        controller.buttonDown = controller.buttonPressed = START_BUTTON;
        chooseCastleStatistics = FALSE;
        assert(render_pause_courses_and_castle() == MENU_OPT_NONE);
        controller.buttonDown = controller.buttonPressed = 0;
        gGlobalTimer++;
        assert(render_pause_courses_and_castle() == MENU_OPT_NONE);
        gGlobalTimer += 7;
        chooseCastleStatistics = castle;
        gDialogLineNum = MENU_OPT_STATISTICS;
        controller.buttonDown = controller.buttonPressed = A_BUTTON;
        int opened = statisticsOpened;
        assert(render_pause_courses_and_castle() == MENU_OPT_NONE);
        assert(statisticsOpened == opened + 1 && sPauseStatisticsOpen);
        // Opening A stays harmless, including after the usual resume delay.
        controller.buttonPressed = 0;
        gGlobalTimer++;
        int settings = settingsDraws;
        assert(render_pause_courses_and_castle() == MENU_OPT_NONE);
        assert(sPauseStatisticsOpen && settingsDraws == settings);
        controller.buttonDown = controller.buttonPressed = B_BUTTON;
        gGlobalTimer++;
        assert(render_pause_courses_and_castle() == MENU_OPT_NONE);
        assert(!sPauseStatisticsOpen && settingsDraws == settings);
        controller.buttonDown = controller.buttonPressed = A_BUTTON;
        gGlobalTimer++;
        assert(render_pause_courses_and_castle() == MENU_OPT_NONE);
        assert(sPauseStatisticsOpen);
        // Start takes priority over A and B and keeps the statistics closing image.
        controller.buttonDown = controller.buttonPressed = START_BUTTON | A_BUTTON | B_BUTTON;
        gGlobalTimer++;
        settings = settingsDraws;
        int drawn = statisticsDrawn;
        assert(render_pause_courses_and_castle() == MENU_OPT_DEFAULT);
        assert(statisticsDrawn == drawn + 1 && settingsDraws == settings);
        assert(!sPauseStatisticsOpen && gMenuMode == MENU_MODE_NONE);
    }
}

int main(void) {
    test_chest_cycles();
    test_motion();
    test_pause_and_exit();
    test_pause_minimum_duration();
    test_statistics_pause_controls();
    puts("Lab chest cycles, motion cycles, pause controls and castle exit routing passed.");
    return 0;
}
