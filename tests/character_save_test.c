/* Real save/checksum, character, file-start and intro-completion functions. */
#include <stdint.h>
#include "test_support.h"
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef float f32;
typedef s16 Vec3s[3];
#define TRUE 1
#define FALSE 0
#define UNUSED __attribute__((unused))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define bcopy(a, b, n) memmove(b, a, n)
#define bzero(a, n) memset(a, 0, n)
#define ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
#define WARP_TYPE_NOT_WARPING 0
#define WARP_OP_NONE 0
#define CUTSCENE_NONE 0
#define CAM_MOVE_C_UP_MODE 1
#define CAM_MOVE_PAUSE_SCREEN 2
#define L_CBUTTONS 0x0002
#define R_CBUTTONS 0x0001
#define U_CBUTTONS 0x0008
#define D_CBUTTONS 0x0004
#define A_BUTTON 0x8000
#define START_BUTTON 0x1000
#define MENU_OPT_NONE 0
#define MENU_OPT_DEFAULT 1
#define MENU_MODE_NONE -1
#define MENU_MODE_RENDER_PAUSE_SCREEN 0
#define DISABLE_EXIT_COURSE
#define PLAY_MODE_NORMAL 0
#define PLAY_MODE_PAUSED 2
#define WARP_FLAG_EXIT_COURSE 2
#define WARP_SPECIAL_NONE 0
#define TEXT_FILE_MARIO_C 0x16, 0x24, 0x35, 0x2c, 0x32, 0x9e, 0x0c, 0xff
#define REONUCAM
#include <assert.h>
#include <stddef.h>
#include "character_save_definitions.inc"

typedef u32 BehaviorScript;
struct Object {
    struct {
        struct Object *next;
        struct {
            struct GraphNode *sharedChild;
        } gfx;
    } header;
    const BehaviorScript *behavior;
    u32 activeFlags;
    s32 oBooDeathStatus, oAction, oOpacity, oBooTargetOpacity;
    s32 oMenuButtonActionPhase, oMenuButtonState;
};
typedef u32 Gfx;
typedef f32 Mat4[4][4];
struct GraphNode {
    int unused;
};
struct GraphNodeSwitchCase {
    struct GraphNode node;
    s16 selectedCase;
};
#define GEO_CONTEXT_RENDER 1
static struct Object *gCurGraphNodeObject;
static struct Object *sMainMenuButtons[MENU_BUTTON_OPTION_MAX];
static struct GraphNode *gLoadedGraphNodes[256];
static s32 sAllFilesExist, sSelectedFileIndex, sFadeOutText, sMainMenuTimer;
static s32 sLastMenuSound;
static f32 gGlobalSoundSource[3];
enum { SOUND_MENU_CLICK_FILE_SELECT, SOUND_MENU_CAMERA_BUZZ, SOUND_MENU_STAR_SOUND };
static void play_sound(s32 sound, f32 *position) {
    sLastMenuSound = sound;
}
static void queue_rumble_data(s32 strength, s32 duration) {
}
static struct Object gObjectLists[NUM_OBJ_LISTS];
static const BehaviorScript bhvGhostHuntBoo[] = { 1 }, bhvBoo[] = { 2 }, bhvMerryGoRoundBoo[] = { 3 };
static const BehaviorScript bhvBooWithCage[] = { 4 }, bhvGhostHuntBigBoo[] = { 5 };
static const BehaviorScript bhvBalconyBigBoo[] = { 6 }, bhvMerryGoRoundBigBoo[] = { 7 };
static s32 obj_has_behavior(struct Object *obj, const BehaviorScript *behavior) {
    return obj->behavior == behavior;
}

struct MarioState {
    u32 action, actionArg, flags;
    s16 waterLevel;
    f32 pos[3];
    const struct MarioMovementProfile *movementProfile;
};
static struct SaveBuffer gSaveBuffer, sSaveStorage;
static s8 gMainMenuDataModified, gSaveFileModified;
static u8 sModifiedFiles;
static u8 sFailedSaveFiles;
static u8 sUnreadFiles, sMenuDataUnread;
static s32 sWrites;
static u8 sBaseWriteFailureMask;
static u32 sCastleSecretSavePending;
/* Secret encounter reset and boundary saves are tested by their own fixture. */
static void castle_secret_sign_reset(s32 fileIndex) {
}
static void yoshi_credit_sign_area_exit(void) {
}
void save_file_flush_castle_secret(void) {
}

static struct {
    u8 speed;
} gReonucamState;
static void reonucam_reset_r_tap(void) {
}
static s32 gCurrSaveFileNum = 1, gCurrLevelNum, gCurrAreaIndex, gCurrActNum;
static s32 gDebugLevelSelect, gNeverEnteredCastle, gCurrCourseNum, gSavedCourseNum;
static s32 gSpecialTripleJump, sDelayedWarpOp;
static struct {
    s32 type, levelNum, areaIdx, nodeId, arg;
} sWarpDest;
static s32 sTasThiccStartPending;
static u8 sPauseTasCodeProgress;
static s16 sCurrPlayMode, gMenuOptSelectIndex, gMenuMode = MENU_MODE_NONE;
static struct {
    u16 buttonDown;
} sController, *gPlayer1Controller = &sController;
static u8 sFileSelectTasCodeProgress;
static s32 sSelectedFileNum, sObjectUpdates, sTransitions, sDialogResets;
static s32 sCapRestores;
static s32 sHealOnCourseEntry;
static void *gCurrDemoInput, *gCurrCreditsEntry;
static u8 gDebugFastWalk;
static struct {
    s32 cutscene;
} sCamera, *gCamera = &sCamera;
static u16 gCameraMovementFlags;
static s32 write_sram_data(void *p, s32 size) {
    for (s32 file = 0; file < NUM_SAVE_FILES; file++) {
        if (p == &gSaveBuffer.files[file] && (sBaseWriteFailureMask & (1u << file))) {
            return 1;
        }
    }
    size_t offset = (u8 *) p - (u8 *) &gSaveBuffer;
    if (offset + size > sizeof(sSaveStorage)) {
        abort();
    }
    memcpy((u8 *) &sSaveStorage + offset, p, size);
    sWrites++;
    return 0;
}
static s32 read_sram_data(void *p, s32 size) {
    memcpy(p, &sSaveStorage, size);
    return 0;
}
static u8 sOptionsStorage[0x800];
static u32 sOptionsReadFailureOffset = ~0u;
static s32 sUnreadStatsFile = -1;
static s32 sUnreadTimingFile = -1;
static s32 sStatsCopies, sTimingCopies;
static s32 player_stats_storage_read(u32 offset, void *data, u32 size) {
    assert(offset >= 0x800 && offset + size <= 0x1000);
    if (offset == sOptionsReadFailureOffset) {
        return 1;
    }
    memcpy(data, sOptionsStorage + offset - 0x800, size);
    return 0;
}
static s32 player_stats_storage_write(u32 offset, const void *data, u32 size) {
    assert(offset >= 0x800 && offset + size <= 0x1000);
    memcpy(sOptionsStorage + offset - 0x800, data, size);
    return 0;
}
/* Timing persistence and runtime effects are covered by their dedicated options fixtures. */
static s32 game_timing_checkpoint(s32 file) {
    return TRUE;
}
static void game_timing_erase(s32 file) {
}
static void game_timing_copy(s32 source, s32 destination) {
    sTimingCopies++;
}
#undef player_stats_copy
static void player_stats_copy(s32 source, s32 destination) {
    sStatsCopies++;
}
static s32 game_timing_can_copy(s32 source) {
    return source != sUnreadTimingFile;
}
#undef player_stats_can_copy
static s32 player_stats_can_copy(s32 source) {
    return source != sUnreadStatsFile;
}
#undef player_stats_retire_legacy
static s32 player_stats_retire_legacy(s32 file) {
    return TRUE;
}
static void game_timing_init(s32 (*read)(u32, void *, u32), s32 (*write)(u32, const void *, u32),
                             s32 (*ready)(s32)) {
}
static void game_timing_disqualify_file(s32 file) {
}
static void game_timing_end_course(void) {
}
static void game_timing_select_file(s32 file) {
}
static s32 game_options_secrets_blocked(void) {
    return FALSE;
}
static void game_options_runtime_update(void) {
}
static void update_paused_camera(void) {
}
static void simpleflips_scream_load_file(s32 fileNum) {
    (void) fileNum;
}
static void init_mario_from_save_file(void) {
}
static void disable_warp_checkpoint(void) {
}
static void select_mario_cam_mode(void) {
}
static void set_yoshi_as_not_dead(void) {
}
static void initiate_warp(s16 level, s16 area, s16 node, s32 flags) {
    sWarpDest.type = 1;
    sWarpDest.levelNum = level;
    sWarpDest.areaIdx = area;
    sWarpDest.nodeId = node;
    sWarpDest.arg = flags;
}
static s32 lvl_restore_cap_on_file_start(s16 arg, s32 level) {
    sCapRestores++;
    return level;
}
static void reset_dialog_render_state(void) {
    sDialogResets++;
}
static void raise_background_noise(s32 arg) {
}
static void hud_reveal_after_pause(void) {
}
static void set_play_mode(s16 playMode);
static void fade_into_special_warp(u32 arg, u32 color) {
    sTransitions++;
    set_play_mode(3);
}
static void area_update_objects(void) {
    sObjectUpdates++;
}
s32 lvl_init_from_save_file(s16 initOrUpdate, s32 levelNum);
void lvl_prepare_tas_thicc_start(void);
void set_menu_mode(s16 mode);
static void stop_and_set_height_to_floor(struct MarioState *m) {
}
static s32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
    return TRUE;
}
/* The pause easter egg has its own timer/audio fixture. */
static void pause_easter_egg_begin(void) {
}
static void pause_easter_egg_end(void) {
}
static void pause_easter_egg_update(void) {
}
static void vec3s_set(Vec3s v, s16 x, s16 y, s16 z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}
static void vec3s_copy(Vec3s dest, Vec3s src) {
    memcpy(dest, src, sizeof(Vec3s));
}
#include "character_save_sources.inc"

static void begin(void) {
    sBaseWriteFailureMask = 0;
    sOptionsReadFailureOffset = ~0u;
    sUnreadStatsFile = sUnreadTimingFile = -1;
    sStatsCopies = sTimingCopies = 0;
    for (int i = 0; i < NUM_OBJ_LISTS; i++) {
        gObjectLists[i].header.next = &gObjectLists[i];
    }
    memset(&sSaveStorage, 0, sizeof(sSaveStorage));
    save_file_load_all();
    mario_character_init();
    gCurrDemoInput = gCurrCreditsEntry = NULL;
    gCurrSaveFileNum = 1;
    gCurrAreaIndex = 1;
    gDebugLevelSelect = FALSE;
    gDebugFastWalk = FALSE;
    sWrites = 0;
    sTasThiccStartPending = FALSE;
    sPauseTasCodeProgress = sFileSelectTasCodeProgress = 0;
    sSelectedFileNum = sObjectUpdates = sTransitions = sDialogResets = sCapRestores = 0;
    gMenuOptSelectIndex = MENU_OPT_NONE;
    gMenuMode = MENU_MODE_NONE;
    sController.buttonDown = 0;
}
static int failed_save_reload(void) {
    begin();
    s32 course = COURSE_NUM_TO_INDEX(COURSE_BOB);
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS);
    save_file_do_save(SAVE_FILE_A);
    save_file_set_flags(SAVE_FLAG_HAVE_KEY_1);
    save_file_set_star_flags(SAVE_FILE_A, course, 1);
    sBaseWriteFailureMask = 1u << SAVE_FILE_A;
    save_file_do_save(SAVE_FILE_A);
    EXPECT_EQ(gSaveFileModified, TRUE);
    EXPECT_EQ(sSaveStorage.files[SAVE_FILE_A][0].courseStars[course], 0);
    // Later never-saved progress rolls back, but the failed explicit save still needs retry.
    save_file_set_star_flags(SAVE_FILE_A, course, 2);
    save_file_reload();
    EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_A][0].courseStars[course], 1);
    EXPECT_EQ(gSaveFileModified, TRUE);
    save_file_do_save(SAVE_FILE_A); // The transport is still unavailable.
    save_file_reload();
    EXPECT_EQ(gSaveFileModified, TRUE);
    sBaseWriteFailureMask = 0;
    save_file_do_save(SAVE_FILE_A);
    EXPECT_EQ(gSaveFileModified, FALSE);
    save_file_load_all();
    EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_A][0].courseStars[course], 1);
    EXPECT_EQ((save_file_get_flags() & SAVE_FLAG_HAVE_KEY_1) != 0, TRUE);

    // Game Over still discards this file's genuinely unsaved changes, preserving other owners.
    save_file_set_flags(SAVE_FLAG_HAVE_KEY_2);
    save_file_set_star_flags(SAVE_FILE_B, course, 2);
    save_file_reload();
    EXPECT_EQ((save_file_get_flags() & SAVE_FLAG_HAVE_KEY_2) != 0, FALSE);
    EXPECT_EQ(gSaveFileModified, TRUE);
    save_file_do_save(SAVE_FILE_B);
    EXPECT_EQ(gSaveFileModified, FALSE);
    save_file_load_all();
    EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_B][0].courseStars[course], 2);

    // Failed copies reject without replacing the destination; a healthy retry performs the copy.
    gCurrSaveFileNum = SAVE_FILE_B + 1;
    save_file_set_flags(SAVE_FLAG_HAVE_KEY_2);
    save_file_do_save(SAVE_FILE_B);
    struct SaveFile destinationBefore[2];
    memcpy(destinationBefore, gSaveBuffer.files[SAVE_FILE_A], sizeof(destinationBefore));
    sBaseWriteFailureMask = 1u << SAVE_FILE_A;
    EXPECT_EQ(save_file_copy(SAVE_FILE_B, SAVE_FILE_A), FALSE);
    EXPECT_EQ(memcmp(destinationBefore, gSaveBuffer.files[SAVE_FILE_A], sizeof(destinationBefore)), 0);
    gCurrSaveFileNum = SAVE_FILE_A + 1;
    save_file_reload();
    EXPECT_EQ(gSaveFileModified, FALSE);
    sBaseWriteFailureMask = 0;
    EXPECT_EQ(save_file_copy(SAVE_FILE_B, SAVE_FILE_A), TRUE);
    save_file_load_all();
    EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_A][0].courseStars[course], 2);
    EXPECT_EQ((save_file_get_flags() & SAVE_FLAG_HAVE_KEY_2) != 0, TRUE);
    EXPECT_EQ((save_file_get_flags() & SAVE_FLAG_HAVE_KEY_1) != 0, FALSE);

    // Erase replaces the pending contents and retains its own failed-write retry.
    sBaseWriteFailureMask = 1u << SAVE_FILE_A;
    save_file_erase(SAVE_FILE_A);
    save_file_reload();
    EXPECT_EQ(save_file_exists(SAVE_FILE_A), FALSE);
    EXPECT_EQ(gSaveFileModified, TRUE);
    sBaseWriteFailureMask = 0;
    save_file_do_save(SAVE_FILE_A);
    save_file_load_all();
    EXPECT_EQ(save_file_exists(SAVE_FILE_A), FALSE);
    EXPECT_EQ((gSaveBuffer.files[SAVE_FILE_B][0].flags & SAVE_FLAG_HAVE_KEY_2) != 0, TRUE);
    EXPECT_EQ(gSaveFileModified, FALSE);

    // A cold load selects actual persisted data and cannot resurrect an old failed attempt.
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HAVE_KEY_1);
    sBaseWriteFailureMask = 1u << SAVE_FILE_A;
    save_file_do_save(SAVE_FILE_A);
    sBaseWriteFailureMask = 0;
    save_file_load_all();
    EXPECT_EQ(save_file_exists(SAVE_FILE_A), FALSE);
    EXPECT_EQ(gSaveFileModified, FALSE);
    save_file_do_save(SAVE_FILE_A);
    EXPECT_EQ((sSaveStorage.files[SAVE_FILE_A][0].flags & SAVE_FLAG_HAVE_KEY_1) != 0, FALSE);
    return TRUE;
}

static int pending_and_occupied_file(void) {
    begin();
    gSaveBuffer.files[SAVE_FILE_C][0].flags = SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HAVE_KEY_1;
    gSaveBuffer.files[SAVE_FILE_C][0].courseStars[0] = 3;
    mark_save_file_modified(SAVE_FILE_C);
    save_file_do_save(SAVE_FILE_C);
    struct SaveFile original = gSaveBuffer.files[SAVE_FILE_C][0];
    save_file_arm_he_thicc();
    EXPECT_EQ(save_file_he_thicc_pending(), TRUE);
    EXPECT_EQ(lvl_should_skip_file_select(0, 0), FALSE);
    EXPECT_EQ(save_file_is_he_thicc(SAVE_FILE_C), FALSE);
    save_file_start_he_thicc(SAVE_FILE_C);
    EXPECT_EQ(save_file_is_he_thicc(SAVE_FILE_C), FALSE);
    EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_C][0].courseStars[0], original.courseStars[0]);
    EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_C][0].flags, original.flags | SAVE_FLAG_HE_THICC_PENDING);
    save_file_load_all();
    EXPECT_EQ(save_file_he_thicc_pending(), TRUE);
    save_file_erase(SAVE_FILE_A);
    EXPECT_EQ(save_file_he_thicc_pending(), TRUE);
    save_file_erase(SAVE_FILE_C);
    EXPECT_EQ(save_file_exists(SAVE_FILE_C), FALSE);
    EXPECT_EQ(save_file_he_thicc_pending(), TRUE);
    return 1;
}
static int new_file_and_full_intro(void) {
    begin();
    save_file_arm_he_thicc();
    save_file_load_all();
    gCurrSaveFileNum = 1;
    lvl_init_from_save_file(0, LEVEL_PHYSICS_LAB);
    EXPECT_EQ(save_file_he_thicc_pending(), TRUE);
    EXPECT_EQ(mario_character_requested_index(), 0);
    gCurrSaveFileNum = 3;
    gCurrDemoInput = &gSaveBuffer;
    lvl_init_from_save_file(0, LEVEL_PHYSICS_LAB);
    EXPECT_EQ(save_file_he_thicc_pending(), TRUE);
    gCurrDemoInput = NULL;
    gDebugLevelSelect = TRUE;
    EXPECT_EQ(lvl_init_from_save_file(0, LEVEL_PHYSICS_LAB), LEVEL_CASTLE_GROUNDS);
    EXPECT_EQ(gDebugLevelSelect, FALSE);
    EXPECT_EQ(gNeverEnteredCastle, TRUE);
    EXPECT_EQ(save_file_is_he_thicc(SAVE_FILE_C), TRUE);
    EXPECT_EQ(save_file_he_thicc_pending(), FALSE);
    EXPECT_EQ(save_file_exists(SAVE_FILE_C), TRUE);
    EXPECT_EQ(save_file_he_thicc_intro_pending(SAVE_FILE_C), TRUE);
    EXPECT_EQ(file_c_name() == textHeThicc, TRUE);
    save_file_load_all();
    EXPECT_EQ(save_file_he_thicc_intro_pending(SAVE_FILE_C), TRUE);
    struct MarioState m = { 0 };
    mario_character_commit(&m, FALSE);
    EXPECT_EQ(mario_character_palette(), MARIO_PALETTE_HE_THICC);
    EXPECT_EQ(m.movementProfile->walkTarget, 40);
    m.action = ACT_INTRO_CUTSCENE;
    sCamera.cutscene = 1;
    intro_cutscene_set_mario_to_idle(&m);
    EXPECT_EQ(save_file_he_thicc_intro_pending(SAVE_FILE_C), TRUE);
    sCamera.cutscene = CUTSCENE_NONE;
    intro_cutscene_set_mario_to_idle(&m);
    EXPECT_EQ(m.action, ACT_IDLE);
    EXPECT_EQ(save_file_he_thicc_intro_pending(SAVE_FILE_C), FALSE);
    save_file_load_all();
    EXPECT_EQ(save_file_is_he_thicc(SAVE_FILE_C), TRUE);
    EXPECT_EQ(save_file_he_thicc_intro_pending(SAVE_FILE_C), FALSE);
    return 1;
}
static int failed_copy_menu_retry(void) {
    static struct Object buttons[MENU_BUTTON_OPTION_MAX];
    static struct GraphNode emptyModel, copiedModel;
    struct Object copyButton = { 0 };
    begin();
    for (s32 button = 0; button < MENU_BUTTON_OPTION_MAX; button++) {
        sMainMenuButtons[button] = &buttons[button];
        buttons[button].header.gfx.sharedChild = &emptyModel;
    }
    gLoadedGraphNodes[MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE] = &copiedModel;
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HAVE_KEY_1);
    save_file_set_star_flags(SAVE_FILE_A, COURSE_NUM_TO_INDEX(COURSE_BOB), 1);
    save_file_do_save(SAVE_FILE_A);
    game_options_select_file(SAVE_FILE_A);
    EXPECT_EQ(game_options_set(GO_MASTER_VOLUME, 30), TRUE);
    EXPECT_EQ(game_options_save(SAVE_FILE_A), TRUE);
    struct SaveBuffer before = gSaveBuffer;
    u8 optionsBefore[sizeof(sOptionsStorage)];
    memcpy(optionsBefore, sOptionsStorage, sizeof(optionsBefore));
    u8 modifiedBefore = sModifiedFiles, failedBefore = sFailedSaveFiles, unreadBefore = sUnreadFiles;
    s32 statsBefore = sStatsCopies, timingBefore = sTimingCopies;
    sSelectedFileIndex = SAVE_FILE_A;
    copyButton.oMenuButtonActionPhase = COPY_PHASE_COPY_WHERE;
    sFadeOutText = FALSE;
    sBaseWriteFailureMask = 1u << SAVE_FILE_B;
    copy_action_file_button(&copyButton, MENU_BUTTON_COPY_FILE_B);
    EXPECT_EQ(copyButton.oMenuButtonActionPhase, COPY_PHASE_COPY_WHERE);
    EXPECT_EQ(sLastMenuSound, SOUND_MENU_CAMERA_BUZZ);
    EXPECT_EQ(sFadeOutText, FALSE);
    EXPECT_EQ(buttons[MENU_BUTTON_COPY_FILE_B].header.gfx.sharedChild == &emptyModel, TRUE);
    EXPECT_EQ(buttons[SAVE_FILE_B].header.gfx.sharedChild == &emptyModel, TRUE);
    EXPECT_EQ(save_file_exists(SAVE_FILE_B), FALSE);
    EXPECT_EQ(memcmp(&before, &gSaveBuffer, sizeof(before)), 0);
    EXPECT_EQ(sModifiedFiles, modifiedBefore);
    EXPECT_EQ(sFailedSaveFiles, failedBefore);
    EXPECT_EQ(sUnreadFiles, unreadBefore);
    EXPECT_EQ(sStatsCopies, statsBefore);
    EXPECT_EQ(sTimingCopies, timingBefore);
    EXPECT_EQ(memcmp(optionsBefore, sOptionsStorage, sizeof(optionsBefore)), 0);
    EXPECT_EQ(game_options_get_file(SAVE_FILE_B, GO_MASTER_VOLUME), 100);

    // The same empty target remains reachable for a healthy retry in the real menu.
    sBaseWriteFailureMask = 0;
    copy_action_file_button(&copyButton, MENU_BUTTON_COPY_FILE_B);
    EXPECT_EQ(copyButton.oMenuButtonActionPhase, COPY_PHASE_COPY_COMPLETE);
    EXPECT_EQ(sLastMenuSound, SOUND_MENU_STAR_SOUND);
    EXPECT_EQ(buttons[MENU_BUTTON_COPY_FILE_B].header.gfx.sharedChild == &copiedModel, TRUE);
    EXPECT_EQ(save_file_exists(SAVE_FILE_B), TRUE);
    EXPECT_EQ(sStatsCopies, statsBefore + 1);
    EXPECT_EQ(sTimingCopies, timingBefore + 1);
    EXPECT_EQ(game_options_get_file(SAVE_FILE_B, GO_MASTER_VOLUME), 30);

    // Completion is safe to follow with erasing the original and restarting the game.
    save_file_erase(SAVE_FILE_A);
    save_file_load_all();
    EXPECT_EQ(save_file_exists(SAVE_FILE_A), FALSE);
    EXPECT_EQ(save_file_exists(SAVE_FILE_B), TRUE);
    EXPECT_EQ((gSaveBuffer.files[SAVE_FILE_B][0].flags & SAVE_FLAG_HAVE_KEY_1) != 0, TRUE);
    EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_B][0].courseStars[COURSE_NUM_TO_INDEX(COURSE_BOB)], 1);
    EXPECT_EQ(game_options_get_file(SAVE_FILE_B, GO_MASTER_VOLUME), 30);
    return TRUE;
}

static int copy_menu_refusal(void) {
    static struct Object buttons[MENU_BUTTON_OPTION_MAX];
    static struct GraphNode emptyModel, copiedModel;
    struct Object copyButton = { 0 };
    begin();
    for (s32 button = 0; button < MENU_BUTTON_OPTION_MAX; button++) {
        sMainMenuButtons[button] = &buttons[button];
        buttons[button].header.gfx.sharedChild = &emptyModel;
    }
    gLoadedGraphNodes[MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE] = &copiedModel;
    save_file_set_flags(SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HAVE_KEY_1);
    save_file_do_save(SAVE_FILE_A);
    sSelectedFileIndex = SAVE_FILE_A;
    copyButton.oMenuButtonActionPhase = COPY_PHASE_COPY_WHERE;
    sUnreadStatsFile = SAVE_FILE_A;
    sMainMenuTimer = BUZZ_TIMER;
    sFadeOutText = FALSE;
    s32 writes = sWrites;
    copy_action_file_button(&copyButton, MENU_BUTTON_COPY_FILE_B);
    EXPECT_EQ(copyButton.oMenuButtonActionPhase, COPY_PHASE_COPY_WHERE);
    EXPECT_EQ(sLastMenuSound, SOUND_MENU_CAMERA_BUZZ);
    EXPECT_EQ(sFadeOutText, FALSE);
    EXPECT_EQ(sMainMenuTimer, BUZZ_TIMER);
    EXPECT_EQ(buttons[MENU_BUTTON_COPY_FILE_B].oMenuButtonState, MENU_BUTTON_STATE_ZOOM_IN_OUT);
    EXPECT_EQ(buttons[MENU_BUTTON_COPY_FILE_B].header.gfx.sharedChild, &emptyModel);
    EXPECT_EQ(buttons[MENU_BUTTON_PLAY_FILE_B].header.gfx.sharedChild, &emptyModel);
    EXPECT_EQ(save_file_exists(SAVE_FILE_B), FALSE);
    EXPECT_EQ(sWrites, writes);

    sUnreadStatsFile = -1;
    save_file_load_all();
    copy_action_file_button(&copyButton, MENU_BUTTON_COPY_FILE_B);
    EXPECT_EQ(copyButton.oMenuButtonActionPhase, COPY_PHASE_COPY_COMPLETE);
    EXPECT_EQ(sLastMenuSound, SOUND_MENU_STAR_SOUND);
    EXPECT_EQ(buttons[MENU_BUTTON_COPY_FILE_B].header.gfx.sharedChild, &copiedModel);
    EXPECT_EQ(buttons[MENU_BUTTON_PLAY_FILE_B].header.gfx.sharedChild, &copiedModel);
    EXPECT_EQ(save_file_exists(SAVE_FILE_B), TRUE);

    // Clicking the selected source still cancels with the ordinary refusal feedback.
    copyButton.oMenuButtonActionPhase = COPY_PHASE_COPY_WHERE;
    copy_action_file_button(&copyButton, MENU_BUTTON_COPY_FILE_A);
    EXPECT_EQ(copyButton.oMenuButtonActionPhase, COPY_PHASE_MAIN);
    EXPECT_EQ(sLastMenuSound, SOUND_MENU_CAMERA_BUZZ);
    EXPECT_EQ(buttons[MENU_BUTTON_COPY_FILE_A].oMenuButtonState, MENU_BUTTON_STATE_ZOOM_OUT);
    return TRUE;
}

static int deletion_and_copy_isolation(void) {
    EXPECT_EQ(copy_menu_refusal(), TRUE);
    begin();
    save_file_arm_he_thicc();
    save_file_start_he_thicc(SAVE_FILE_C);
    save_file_copy(SAVE_FILE_C, SAVE_FILE_A);
    save_file_copy(SAVE_FILE_C, SAVE_FILE_D);
    u32 secret = SAVE_FLAG_HE_THICC | SAVE_FLAG_HE_THICC_PENDING | SAVE_FLAG_HE_THICC_INTRO;
    EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_A][0].flags & secret, 0);
    EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_D][0].flags & secret, 0);
    save_file_arm_he_thicc();
    EXPECT_EQ(save_file_he_thicc_pending(), FALSE);
    save_file_copy(SAVE_FILE_A, SAVE_FILE_C);
    EXPECT_EQ(save_file_is_he_thicc(SAVE_FILE_C), TRUE);
    save_file_erase(SAVE_FILE_C);
    save_file_load_all();
    EXPECT_EQ(save_file_is_he_thicc(SAVE_FILE_C), FALSE);
    EXPECT_EQ(save_file_he_thicc_pending(), FALSE);
    EXPECT_EQ(file_c_name() == textMarioC, TRUE);
    EXPECT_EQ(lvl_should_skip_file_select(0, 0), TRUE);
    save_file_start_he_thicc(SAVE_FILE_C);
    EXPECT_EQ(save_file_is_he_thicc(SAVE_FILE_C), FALSE);
    save_file_arm_he_thicc();
    save_file_copy(SAVE_FILE_A, SAVE_FILE_C);
    EXPECT_EQ(save_file_he_thicc_pending(), TRUE);
    EXPECT_EQ(save_file_is_he_thicc(SAVE_FILE_C), FALSE);
    // Each auxiliary source must be readable before any destination state changes.
    for (s32 fault = 0; fault < 3; fault++) {
        begin();
        save_file_set_flags(SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HAVE_KEY_1);
        game_options_select_file(SAVE_FILE_A);
        EXPECT_EQ(game_options_set(GO_MASTER_VOLUME, 40), TRUE);
        save_file_do_save(SAVE_FILE_A);
        gCurrSaveFileNum = SAVE_FILE_B + 1;
        save_file_set_flags(SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HAVE_KEY_2);
        game_options_select_file(SAVE_FILE_B);
        EXPECT_EQ(game_options_set(GO_MASTER_VOLUME, 20), TRUE);
        save_file_do_save(SAVE_FILE_B);
        struct SaveBuffer persisted = sSaveStorage;
        struct SaveBuffer cached = gSaveBuffer;
        u8 options[sizeof(sOptionsStorage)];
        memcpy(options, sOptionsStorage, sizeof(options));
        s32 writes = sWrites;
        s32 statsCopies = sStatsCopies, timingCopies = sTimingCopies;
        u8 modified = sModifiedFiles;
        if (fault == 0) {
            sUnreadStatsFile = SAVE_FILE_A;
        } else if (fault == 1) {
            sUnreadTimingFile = SAVE_FILE_A;
        } else {
            sOptionsReadFailureOffset = record_offset(SAVE_FILE_A, 0);
            game_options_init(player_stats_storage_read, player_stats_storage_write);
            sOptionsReadFailureOffset = ~0u;
        }
        EXPECT_EQ(save_file_copy(SAVE_FILE_A, SAVE_FILE_B), FALSE);
        EXPECT_EQ(sWrites, writes);
        EXPECT_EQ(sStatsCopies, statsCopies);
        EXPECT_EQ(sTimingCopies, timingCopies);
        EXPECT_EQ(sModifiedFiles, modified);
        EXPECT_EQ(memcmp(&cached, &gSaveBuffer, sizeof(cached)), 0);
        EXPECT_EQ(memcmp(&persisted, &sSaveStorage, sizeof(persisted)), 0);
        EXPECT_EQ(memcmp(options, sOptionsStorage, sizeof(options)), 0);
        EXPECT_EQ(game_options_get_file(SAVE_FILE_B, GO_MASTER_VOLUME), 20);
        sUnreadStatsFile = sUnreadTimingFile = -1;
        save_file_load_all();
        EXPECT_EQ(save_file_copy(SAVE_FILE_A, SAVE_FILE_B), TRUE);
        EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_B][0].flags & SAVE_FLAG_HAVE_KEY_1, SAVE_FLAG_HAVE_KEY_1);
        EXPECT_EQ(game_options_get_file(SAVE_FILE_B, GO_MASTER_VOLUME), 40);
    }
    return 1;
}
static int backup_recovery_and_invalid_save(void) {
    begin();
    save_file_arm_he_thicc();
    save_file_start_he_thicc(SAVE_FILE_C);
    sSaveStorage.files[SAVE_FILE_C][0].signature.magic = 0;
    save_file_load_all();
    EXPECT_EQ(save_file_is_he_thicc(SAVE_FILE_C), TRUE);
    sSaveStorage.files[SAVE_FILE_C][1].signature.magic = 0;
    save_file_load_all();
    EXPECT_EQ(save_file_is_he_thicc(SAVE_FILE_C), TRUE);
    memset(&sSaveStorage.files[SAVE_FILE_C], 0xff, sizeof(sSaveStorage.files[SAVE_FILE_C]));
    save_file_load_all();
    EXPECT_EQ(save_file_he_thicc_pending(), FALSE);
    EXPECT_EQ(save_file_is_he_thicc(SAVE_FILE_C), FALSE);
    return 1;
}
static int palette_lock_and_death_reason(void) {
    begin();
    struct MarioState m = { 0 };
    EXPECT_EQ(mario_character_request(1), FALSE);
    EXPECT_EQ(mario_character_request(2), TRUE);
    save_file_arm_he_thicc();
    save_file_start_he_thicc(SAVE_FILE_C);
    gCurrSaveFileNum = 3;
    mario_character_load_file(SAVE_FILE_C);
    mario_character_commit(&m, FALSE);
    EXPECT_EQ(mario_character_request(0), FALSE);
    EXPECT_EQ(mario_character_request(3), TRUE);
    mario_character_commit(&m, FALSE);
    EXPECT_EQ(mario_character_active_index(), 3);
    mario_character_commit(&m, TRUE);
    EXPECT_EQ(mario_character_active_index(), 0);
    mario_character_commit(&m, FALSE);
    EXPECT_EQ(mario_character_active_index(), 3);
    gCurrSaveFileNum = 1;
    mario_character_load_file(SAVE_FILE_A);
    mario_character_commit(&m, FALSE);
    EXPECT_EQ(mario_character_active_index(), 0);
    EXPECT_EQ(mario_character_request(3), FALSE);
    m.action = ACT_QUICKSAND_DEATH;
    m.actionArg = ACT_ARG_QUICKSAND_DEATH_SHALLOW_WATER;
    EXPECT_EQ(mario_character_is_unlock_death(&m), FALSE);
    m.actionArg = ACT_ARG_QUICKSAND_DEATH_HE_THICC_UNLOCK;
    EXPECT_EQ(mario_character_is_unlock_death(&m), TRUE);
    return 1;
}
static int courtyard_boos(void) {
    begin();
    gCurrLevelNum = LEVEL_CASTLE_COURTYARD;
    struct MarioState m = { .action = ACT_DAB, .waterLevel = 51, .pos = { 0, 0, -1200 } };
    EXPECT_EQ(mario_character_can_unlock(&m), FALSE); // Cage carrier has not spawned.
    struct Object *head = &gObjectLists[OBJ_LIST_GENACTOR];
    struct Object first = { .behavior = bhvGhostHuntBoo, .activeFlags = ACTIVE_FLAG_ACTIVE };
    struct Object last = { .activeFlags = ACTIVE_FLAG_ACTIVE };
    struct Object cage = { .behavior = bhvBooWithCage,
                           .activeFlags = ACTIVE_FLAG_ACTIVE,
                           .oOpacity = 255,
                           .oBooTargetOpacity = 255 };
    head->header.next = &first;
    first.header.next = &last;
    last.header.next = &cage;
    cage.header.next = head;
    const BehaviorScript *ordinary[] = { bhvGhostHuntBoo, bhvBoo, bhvMerryGoRoundBoo };
    for (int type = 0; type < 3; type++) {
        last.behavior = ordinary[type];
        first.oAction = 3; // Already defeated, even before the death animation's first update.
        for (int action = 0; action <= 2; action++) {
            last.oAction = action;
            last.oBooDeathStatus = BOO_DEATH_STATUS_ALIVE;
            EXPECT_EQ(mario_character_can_unlock(&m), FALSE); // Dormant, chasing, or bounced/stunned.
        }
        last.oAction = 3;
        EXPECT_EQ(mario_character_can_unlock(&m), TRUE);
        last.oBooDeathStatus = BOO_DEATH_STATUS_DYING;
        EXPECT_EQ(mario_character_can_unlock(&m), TRUE);
        last.oAction = 4;
        last.oBooDeathStatus = BOO_DEATH_STATUS_DEAD;
        EXPECT_EQ(mario_character_can_unlock(&m), TRUE);
        last.oAction = 0;
        last.oBooDeathStatus = BOO_DEATH_STATUS_ALIVE;
        last.activeFlags = ACTIVE_FLAG_DEACTIVATED;
        EXPECT_EQ(mario_character_can_unlock(&m), TRUE);
        last.activeFlags = ACTIVE_FLAG_ACTIVE;
        EXPECT_EQ(mario_character_can_unlock(&m), FALSE);
    }
    const BehaviorScript *big[] = { bhvBooWithCage, bhvGhostHuntBigBoo, bhvBalconyBigBoo,
                                    bhvMerryGoRoundBigBoo };
    for (int type = 0; type < 4; type++) {
        last.behavior = big[type];
        EXPECT_EQ(mario_character_can_unlock(&m), TRUE);
        first.oAction = 1;
        EXPECT_EQ(mario_character_can_unlock(&m), FALSE);
        first.oAction = 3;
    }
    head->header.next = &cage;
    for (int type = 0; type < 4; type++) {
        cage.behavior = big[type];
        EXPECT_EQ(mario_character_can_unlock(&m), type == 0);
    }
    cage.behavior = bhvBooWithCage;
    cage.oAction = 2;
    EXPECT_EQ(mario_character_can_unlock(&m), TRUE); // Bounced, still alive.
    cage.oAction = 3;
    EXPECT_EQ(mario_character_can_unlock(&m), FALSE); // Fatal hit before death update.
    cage.oAction = 0;
    cage.oBooDeathStatus = BOO_DEATH_STATUS_DYING;
    EXPECT_EQ(mario_character_can_unlock(&m), FALSE);
    cage.oBooDeathStatus = BOO_DEATH_STATUS_DEAD;
    EXPECT_EQ(mario_character_can_unlock(&m), FALSE);
    cage.oBooDeathStatus = BOO_DEATH_STATUS_ALIVE;
    cage.activeFlags = ACTIVE_FLAG_DEACTIVATED;
    EXPECT_EQ(mario_character_can_unlock(&m), FALSE);
    cage.activeFlags = ACTIVE_FLAG_ACTIVE;
    EXPECT_EQ(mario_character_can_unlock(&m), TRUE);
    m.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD;
    EXPECT_EQ(mario_character_can_unlock(&m), FALSE);
    m.flags = MARIO_NORMAL_CAP | MARIO_CAP_IN_HAND;
    EXPECT_EQ(mario_character_can_unlock(&m), FALSE);
    m.flags = 0;
    EXPECT_EQ(mario_character_can_unlock(&m), TRUE);
    const s32 opacity[] = { 0, 40, 180, 181, 210, 255 };
    for (unsigned i = 0; i < sizeof(opacity) / sizeof(*opacity); i++) {
        cage.oOpacity = opacity[i];
        cage.oBooTargetOpacity = 255;
        EXPECT_EQ(mario_character_can_unlock(&m), opacity[i] > 180);
        cage.oBooTargetOpacity = 40;
        EXPECT_EQ(mario_character_can_unlock(&m), FALSE);
    }
    head->header.next = head;
    EXPECT_EQ(mario_character_can_unlock(&m), FALSE);
    return 1;
}
static int file_face_palette(void) {
    static struct Object buttons[MENU_BUTTON_OPTION_MAX], unrelated;
    struct GraphNodeSwitchCase node = { 0 };
    begin();
    for (int i = 0; i < MENU_BUTTON_OPTION_MAX; i++) {
        sMainMenuButtons[i] = &buttons[i];
    }
    for (int cycle = 0; cycle < 3; cycle++) {
        save_file_arm_he_thicc();
        save_file_start_he_thicc(SAVE_FILE_C);
        EXPECT_EQ(save_file_is_he_thicc(SAVE_FILE_C), TRUE);
        // Gameplay can still be Standard before the selected file is entered.
        EXPECT_EQ(mario_character_palette(), MARIO_PALETTE_STANDARD);
        for (int i = 0; i < MENU_BUTTON_OPTION_MAX; i++) {
            gCurGraphNodeObject = &buttons[i];
            node.selectedCase = -1;
            EXPECT_EQ(geo_switch_file_select_face(GEO_CONTEXT_RENDER, &node.node, NULL), NULL);
            EXPECT_EQ(node.selectedCase, i == MENU_BUTTON_PLAY_FILE_C || i == MENU_BUTTON_SCORE_FILE_C
                                             || i == MENU_BUTTON_COPY_FILE_C
                                             || i == MENU_BUTTON_ERASE_FILE_C);
        }
        gCurGraphNodeObject = NULL;
        node.selectedCase = 1;
        geo_switch_file_select_face(GEO_CONTEXT_RENDER, &node.node, NULL);
        EXPECT_EQ(node.selectedCase, 0);
        gCurGraphNodeObject = &unrelated;
        node.selectedCase = 1;
        geo_switch_file_select_face(GEO_CONTEXT_RENDER, &node.node, NULL);
        EXPECT_EQ(node.selectedCase, 0);
        gCurGraphNodeObject = &buttons[MENU_BUTTON_PLAY_FILE_C];
        node.selectedCase = 0;
        geo_switch_file_select_face(0, &node.node, NULL);
        EXPECT_EQ(node.selectedCase, 0);
        save_file_copy(SAVE_FILE_C, SAVE_FILE_A);
        gCurGraphNodeObject = &buttons[MENU_BUTTON_PLAY_FILE_A];
        geo_switch_file_select_face(GEO_CONTEXT_RENDER, &node.node, NULL);
        EXPECT_EQ(node.selectedCase, 0);
        save_file_erase(SAVE_FILE_C);
        for (int i = 0; i < MENU_BUTTON_OPTION_MAX; i++) {
            gCurGraphNodeObject = &buttons[i];
            node.selectedCase = 1;
            geo_switch_file_select_face(GEO_CONTEXT_RENDER, &node.node, NULL);
            EXPECT_EQ(node.selectedCase, 0);
        }
    }
    return 1;
}

static u16 tas_chord(s32 frame) {
    return (frame & 1) ? U_CBUTTONS | D_CBUTTONS : L_CBUTTONS | R_CBUTTONS;
}

static int tas_sequence_rejection(void) {
    begin();
    u8 progress = 0;
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int frame = 0; frame < 100; frame++) {
            EXPECT_EQ(mario_character_update_tas_code(&progress, tas_chord(0)), FALSE);
        }
        EXPECT_EQ(progress, 1);
        EXPECT_EQ(mario_character_update_tas_code(&progress, 0), FALSE);
        EXPECT_EQ(progress, 0);
        for (int frame = 0; frame < 63; frame++) {
            EXPECT_EQ(mario_character_update_tas_code(&progress, tas_chord(frame)), FALSE);
        }
        EXPECT_EQ(mario_character_update_tas_code(&progress, tas_chord(63) | A_BUTTON), FALSE);
        EXPECT_EQ(progress, 0);
        EXPECT_EQ(mario_character_update_tas_code(&progress, tas_chord(1)), FALSE);
        for (int frame = 0; frame < 64; frame++) {
            EXPECT_EQ(mario_character_update_tas_code(&progress, tas_chord(frame)), frame == 63);
        }
        EXPECT_EQ(progress, 0);
    }
    return 1;
}

static int tas_file_select_start(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin();
        gDebugLevelSelect = TRUE;
        gSaveBuffer.files[SAVE_FILE_A][0].flags = SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HAVE_KEY_1;
        struct SaveFile fileA = gSaveBuffer.files[SAVE_FILE_A][0];
        gSaveBuffer.files[SAVE_FILE_C][0].flags = SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HAVE_KEY_2
                                                  | SAVE_FLAG_HE_THICC_PENDING
                                                  | SAVE_FLAG_HE_THICC_INTRO;
        gSaveBuffer.files[SAVE_FILE_C][0].courseStars[0] = 5;
        gSaveBuffer.files[SAVE_FILE_C][0].courseCoinScores[0] = 83;
        for (int frame = 0; frame < 64; frame++) {
            sController.buttonDown = tas_chord(frame);
            EXPECT_EQ(lvl_update_obj_and_load_file_selected(0, 0), frame == 63 ? 3 : 0);
            EXPECT_EQ(save_file_is_tas_thicc(SAVE_FILE_C), frame == 63);
        }
        EXPECT_EQ(sObjectUpdates, 63);
        EXPECT_EQ(gCurrSaveFileNum, 3);
        EXPECT_EQ(lvl_init_from_save_file(0, LEVEL_BOB), LEVEL_CASTLE_GROUNDS);
        EXPECT_EQ(gDebugLevelSelect, FALSE);
        EXPECT_EQ(sWarpDest.levelNum, LEVEL_CASTLE_GROUNDS);
        EXPECT_EQ(sWarpDest.areaIdx, 1);
        EXPECT_EQ(sWarpDest.nodeId, 0x32);
        EXPECT_EQ(sWarpDest.arg, WARP_FLAG_EXIT_COURSE);
        EXPECT_EQ(mario_character_requested_index(), 1);
        EXPECT_EQ(save_file_he_thicc_intro_pending(SAVE_FILE_C), FALSE);
        EXPECT_EQ(save_file_he_thicc_pending(), FALSE);
        EXPECT_EQ(gNeverEnteredCastle, FALSE);
        EXPECT_EQ(file_c_name() == textTasThicc, TRUE);
        EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_C][0].courseStars[0], 5);
        EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_C][0].courseCoinScores[0], 83);
        EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_C][0].flags & SAVE_FLAG_HAVE_KEY_2, SAVE_FLAG_HAVE_KEY_2);
        EXPECT_EQ(memcmp(&fileA, &gSaveBuffer.files[SAVE_FILE_A][0], sizeof(fileA)), 0);
        save_file_load_all();
        EXPECT_EQ(save_file_is_tas_thicc(SAVE_FILE_C), TRUE);
        EXPECT_EQ(lvl_init_from_save_file(0, LEVEL_BOB), LEVEL_BOB);
        EXPECT_EQ(sWarpDest.type, WARP_TYPE_NOT_WARPING);
    }
    return 1;
}

static int tas_pause_start(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin();
        // A fresh C, occupied standard C, and an already unlocked HE THICC C.
        if (cycle == 1) {
            gSaveBuffer.files[SAVE_FILE_C][0].flags = SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HAVE_KEY_1;
        } else if (cycle == 2) {
            save_file_arm_he_thicc();
            save_file_start_he_thicc(SAVE_FILE_C);
        }
        gCurrLevelNum = cycle == 2 ? LEVEL_CASTLE_GROUNDS : LEVEL_BOB;
        set_play_mode(PLAY_MODE_PAUSED);
        gMenuMode = MENU_MODE_RENDER_PAUSE_SCREEN;
        gCameraMovementFlags = CAM_MOVE_PAUSE_SCREEN | CAM_MOVE_C_UP_MODE;
        for (int frame = 0; frame < 64; frame++) {
            sController.buttonDown = tas_chord(frame);
            EXPECT_EQ(play_mode_paused(), FALSE);
            if (frame < 63) {
                EXPECT_EQ(sCurrPlayMode, PLAY_MODE_PAUSED);
                EXPECT_EQ(sTransitions, 0);
                EXPECT_EQ(gMenuMode, MENU_MODE_RENDER_PAUSE_SCREEN);
            }
        }
        EXPECT_EQ(sTransitions, 1);
        EXPECT_EQ(sDialogResets, 1);
        EXPECT_EQ(sCapRestores, 1);
        EXPECT_EQ(gMenuMode, MENU_MODE_NONE);
        EXPECT_EQ(gMenuOptSelectIndex, MENU_OPT_NONE);
        EXPECT_EQ(gCameraMovementFlags, CAM_MOVE_C_UP_MODE);
        EXPECT_EQ(sPauseTasCodeProgress, 0);
        EXPECT_EQ(gCurrSaveFileNum, 3);
        EXPECT_EQ(sWarpDest.levelNum, LEVEL_CASTLE_GROUNDS);
        EXPECT_EQ(sWarpDest.nodeId, 0x32);
        EXPECT_EQ(save_file_he_thicc_intro_pending(SAVE_FILE_C), FALSE);
        EXPECT_EQ(save_file_is_tas_thicc(SAVE_FILE_C), TRUE);
        EXPECT_EQ(mario_character_requested_index(), 1);
    }
    begin();
    set_play_mode(PLAY_MODE_PAUSED);
    for (int frame = 0; frame < 32; frame++) {
        sController.buttonDown = tas_chord(frame);
        play_mode_paused();
    }
    gMenuOptSelectIndex = MENU_OPT_DEFAULT;
    play_mode_paused();
    EXPECT_EQ(sCurrPlayMode, PLAY_MODE_NORMAL);
    EXPECT_EQ(sPauseTasCodeProgress, 0);
    EXPECT_EQ(sTransitions, 0);
    gMenuOptSelectIndex = MENU_OPT_NONE;
    set_play_mode(PLAY_MODE_PAUSED);
    for (int frame = 32; frame < 64; frame++) {
        sController.buttonDown = tas_chord(frame);
        play_mode_paused();
    }
    EXPECT_EQ(save_file_is_tas_thicc(SAVE_FILE_C), FALSE);
    return 1;
}

static int tas_save_isolation(void) {
    begin();
    save_file_unlock_tas_thicc();
    save_file_copy(SAVE_FILE_C, SAVE_FILE_A);
    save_file_copy(SAVE_FILE_C, SAVE_FILE_D);
    EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_A][0].flags & SAVE_FLAG_TAS_THICC, 0);
    EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_D][0].flags & SAVE_FLAG_TAS_THICC, 0);
    save_file_copy(SAVE_FILE_A, SAVE_FILE_C);
    EXPECT_EQ(save_file_is_tas_thicc(SAVE_FILE_C), TRUE);
    save_file_load_all();
    EXPECT_EQ(file_c_name() == textTasThicc, TRUE);
    save_file_erase(SAVE_FILE_C);
    EXPECT_EQ(file_c_name() == textMarioC, TRUE);
    EXPECT_EQ(save_file_is_tas_thicc(SAVE_FILE_C), FALSE);
    save_file_arm_he_thicc();
    save_file_start_he_thicc(SAVE_FILE_C);
    EXPECT_EQ(file_c_name() == textHeThicc, TRUE);
    return 1;
}

static u32 little_word(const u8 *p) {
    return p[0] | (u32) p[1] << 8 | (u32) p[2] << 16 | (u32) p[3] << 24;
}

static int tas_movie_replays(void) {
    const char *names[] = { "file-select", "pause-menu" };
    for (int menu = 0; menu < 2; menu++) {
        char path[1024];
        u8 header[1024], sample[4], seed[512];
        begin();
        snprintf(path, sizeof(path), "%s/%s.eep", TAS_MOVIE_DIRECTORY, names[menu]);
        FILE *f = fopen(path, "rb");
        EXPECT_EQ(f != NULL, TRUE);
        EXPECT_EQ(fread(seed, 1, sizeof(seed), f), sizeof(seed));
        fclose(f);
        for (int copy = 0; copy < 8; copy++) {
            u8 *block = seed + copy * 56;
            EXPECT_EQ(block[52] * 256 + block[53], 0x4441);
            EXPECT_EQ(block[54] * 256 + block[55], calc_checksum(block, 56));
            EXPECT_EQ(block[8] & 0x20, 0); // TAS flag has not been preinstalled.
        }
        if (menu == 0) {
            // Load the target-endian fixture's witnessed progress into the host save.
            const u8 *c = seed + 224;
            gSaveBuffer.files[SAVE_FILE_C][0].flags =
                (u32) c[8] << 24 | (u32) c[9] << 16 | (u32) c[10] << 8 | c[11];
            gSaveBuffer.files[SAVE_FILE_C][0].courseStars[0] = c[12];
            gSaveBuffer.files[SAVE_FILE_C][0].courseCoinScores[0] = c[37];
            EXPECT_EQ(save_file_he_thicc_pending(), TRUE);
        }
        snprintf(path, sizeof(path), "%s/%s.m64", TAS_MOVIE_DIRECTORY, names[menu]);
        f = fopen(path, "rb");
        EXPECT_EQ(f != NULL, TRUE);
        EXPECT_EQ(fread(header, 1, sizeof(header), f), sizeof(header));
        EXPECT_EQ(memcmp(header, "M64\x1a", 4), 0);
        EXPECT_EQ(little_word(header + 4), 3);
        EXPECT_EQ(header[28], 4);
        EXPECT_EQ(little_word(header + 32), 1);
        s32 completed = FALSE, paused = FALSE;
        u32 count = little_word(header + 24);
        for (u32 frame = 0; frame < count; frame++) {
            EXPECT_EQ(fread(sample, 1, sizeof(sample), f), sizeof(sample));
            EXPECT_EQ(sample[2] | sample[3], 0); // Both axes stay neutral.
            sController.buttonDown = little_word(sample);
            if (completed) {
                EXPECT_EQ(sController.buttonDown, 0);
                continue;
            }
            if (menu == 0) {
                if (lvl_update_obj_and_load_file_selected(0, 0) != 0) {
                    EXPECT_EQ(frame, 363);
                    EXPECT_EQ(lvl_init_from_save_file(0, LEVEL_PHYSICS_LAB), LEVEL_CASTLE_GROUNDS);
                    completed = TRUE;
                }
            } else if (!paused) {
                if (sController.buttonDown == START_BUTTON) {
                    EXPECT_EQ(frame, 300);
                    paused = TRUE;
                    set_play_mode(PLAY_MODE_PAUSED);
                    gMenuMode = MENU_MODE_RENDER_PAUSE_SCREEN;
                } else {
                    EXPECT_EQ(sController.buttonDown, 0);
                }
            } else {
                play_mode_paused();
                if (sTransitions != 0) {
                    EXPECT_EQ(frame, 394);
                    EXPECT_EQ(gMenuMode, MENU_MODE_NONE);
                    completed = TRUE;
                }
            }
        }
        EXPECT_EQ(fgetc(f), EOF);
        fclose(f);
        EXPECT_EQ(completed, TRUE);
        EXPECT_EQ(save_file_is_tas_thicc(SAVE_FILE_C), TRUE);
        EXPECT_EQ(file_c_name() == textTasThicc, TRUE);
        EXPECT_EQ(save_file_he_thicc_intro_pending(SAVE_FILE_C), FALSE);
        EXPECT_EQ(sWarpDest.levelNum, LEVEL_CASTLE_GROUNDS);
        EXPECT_EQ(sWarpDest.nodeId, 0x32);
        if (menu == 0) {
            EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_C][0].courseStars[0], 5);
            EXPECT_EQ(gSaveBuffer.files[SAVE_FILE_C][0].courseCoinScores[0], 83);
        }
    }
    return 1;
}

static int completed_file_start(void) {
    for (s32 file = 0; file < NUM_SAVE_FILES; file++) {
        begin();
        gCurrSaveFileNum = file + 1;
        save_file_set_flags(SAVE_FLAG_HAVE_KEY_1 | SAVE_FLAG_COLLECTED_TOAD_STAR_1);
        u32 stars = SAVE_FLAG_TO_STAR_FLAG(gSaveBuffer.files[file][0].flags);
        save_file_mark_game_completed();
        EXPECT_EQ(save_file_has_completed_game(file), TRUE);
        EXPECT_EQ(SAVE_FLAG_TO_STAR_FLAG(gSaveBuffer.files[file][0].flags), stars);
        EXPECT_EQ(save_file_has_completed_game(-1), FALSE);
        EXPECT_EQ(save_file_has_completed_game(NUM_SAVE_FILES), FALSE);
        // Both SRAM copies and the loader must retain completion.
        EXPECT_EQ(sSaveStorage.files[file][0].flags & SAVE_FLAG_GAME_COMPLETED,
                  SAVE_FLAG_GAME_COMPLETED);
        EXPECT_EQ(sSaveStorage.files[file][1].flags & SAVE_FLAG_GAME_COMPLETED,
                  SAVE_FLAG_GAME_COMPLETED);
        save_file_load_all();
        for (s32 other = 0; other < NUM_SAVE_FILES; other++) {
            gCurrSaveFileNum = other + 1;
            EXPECT_EQ(save_file_has_completed_game(other), other == file);
            gCurrActNum = 6;
            EXPECT_EQ(lvl_init_from_save_file(0, LEVEL_CASTLE_GROUNDS),
                      other == file ? LEVEL_PHYSICS_LAB : LEVEL_CASTLE_GROUNDS);
            if (other == file) {
                EXPECT_EQ(gCurrActNum, 1);
            }
        }
        // Copying progress carries completion; erasing it restores the ordinary start.
        s32 copy = (file + 1) % NUM_SAVE_FILES;
        save_file_copy(file, copy);
        save_file_load_all();
        gCurrSaveFileNum = copy + 1;
        EXPECT_EQ(lvl_init_from_save_file(0, LEVEL_CASTLE_GROUNDS), LEVEL_PHYSICS_LAB);
        save_file_erase(file);
        save_file_load_all();
        gCurrSaveFileNum = file + 1;
        EXPECT_EQ(save_file_has_completed_game(file), FALSE);
        EXPECT_EQ(lvl_init_from_save_file(0, LEVEL_CASTLE_GROUNDS), LEVEL_CASTLE_GROUNDS);
        EXPECT_EQ(save_file_has_completed_game(copy), TRUE);
    }
    return TRUE;
}

static int completion_start_priority(void) {
    begin();
    gCurrDemoInput = &sSaveStorage;
    save_file_mark_game_completed();
    EXPECT_EQ(save_file_has_completed_game(SAVE_FILE_A), FALSE);
    gCurrDemoInput = NULL;
    gCurrCreditsEntry = &sSaveStorage;
    save_file_mark_game_completed();
    EXPECT_EQ(save_file_has_completed_game(SAVE_FILE_A), FALSE);
    gCurrCreditsEntry = NULL;
    save_file_mark_game_completed();
    gDebugLevelSelect = TRUE;
    EXPECT_EQ(lvl_init_from_save_file(0, LEVEL_BOB), LEVEL_BOB);
    gDebugLevelSelect = FALSE;
    gCurrDemoInput = &sSaveStorage;
    EXPECT_EQ(lvl_init_from_save_file(0, LEVEL_BOB), LEVEL_BOB);
    gCurrDemoInput = NULL;
    gCurrSaveFileNum = SAVE_FILE_C + 1;
    save_file_mark_game_completed();
    gSaveBuffer.files[SAVE_FILE_C][0].flags |= SAVE_FLAG_HE_THICC | SAVE_FLAG_HE_THICC_INTRO;
    EXPECT_EQ(lvl_init_from_save_file(0, LEVEL_PHYSICS_LAB), LEVEL_CASTLE_GROUNDS);
    return TRUE;
}

static int camera_speed_files(void) {
    begin();
    for (s32 file = 0; file < NUM_SAVE_FILES; file++) {
        gCurrSaveFileNum = file + 1;
        EXPECT_EQ(save_file_get_camera_speed(), REONUCAM_DEFAULT_SPEED);
        gSaveBuffer.files[file][0].capArea = SAVE_CAP_AREA_PAUSE_EASTER_EGG | 1;
        gSaveBuffer.files[file][0].flags |= SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_HAVE_KEY_1;
        save_file_set_camera_speed(file);
    }
    memset(&gSaveBuffer, 0, sizeof(gSaveBuffer));
    save_file_load_all();
    for (s32 file = 0; file < NUM_SAVE_FILES; file++) {
        gCurrSaveFileNum = file + 1;
        EXPECT_EQ(save_file_get_camera_speed(), file);
        lvl_init_from_save_file(0, LEVEL_BOB);
        EXPECT_EQ(gReonucamState.speed, file);
        gCurrLevelNum = LEVEL_BOB;
        gCurrAreaIndex = 2;
        save_file_set_cap_pos(100, 200, 300);
        Vec3s pos;
        EXPECT_EQ(save_file_get_cap_pos(pos), TRUE);
        EXPECT_EQ(pos[0], 100);
        EXPECT_EQ(save_file_get_camera_speed(), file);
        EXPECT_EQ(gSaveBuffer.files[file][0].capArea & SAVE_CAP_AREA_PAUSE_EASTER_EGG,
                  SAVE_CAP_AREA_PAUSE_EASTER_EGG);
        EXPECT_EQ(gSaveBuffer.files[file][0].flags & SAVE_FLAG_HAVE_KEY_1, SAVE_FLAG_HAVE_KEY_1);
    }
    gCurrSaveFileNum = 1;
    save_file_set_camera_speed(4);
    s32 writes = sWrites;
    save_file_set_camera_speed(4);
    EXPECT_EQ(sWrites, writes);
    sSaveStorage.files[SAVE_FILE_A][0].signature.chksum ^= 1;
    save_file_load_all();
    EXPECT_EQ(save_file_get_camera_speed(), 4);
    save_file_copy(SAVE_FILE_A, SAVE_FILE_B);
    gCurrSaveFileNum = 2;
    EXPECT_EQ(save_file_get_camera_speed(), 4);
    save_file_erase(SAVE_FILE_B);
    EXPECT_EQ(save_file_get_camera_speed(), REONUCAM_DEFAULT_SPEED);
    gSaveBuffer.files[SAVE_FILE_B][0].capArea = SAVE_CAP_AREA_CAMERA_SPEED_MASK;
    EXPECT_EQ(save_file_get_camera_speed(), REONUCAM_DEFAULT_SPEED);
    save_file_set_camera_speed(255);
    EXPECT_EQ(save_file_get_camera_speed(), REONUCAM_DEFAULT_SPEED);
    gCurrSaveFileNum = 0;
    EXPECT_EQ(save_file_get_camera_speed(), REONUCAM_DEFAULT_SPEED);
    save_file_set_camera_speed(4);
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
#define RUN(name, fn)                                                                                  \
    if (test_selected("character_save." name))                                                         \
    test_report("character_save." name, fn())
    RUN("camera_speed", camera_speed_files);
    RUN("completed_start", completed_file_start);
    RUN("completion_priority", completion_start_priority);
    RUN("tas_sequence", tas_sequence_rejection);
    RUN("tas_file_select", tas_file_select_start);
    RUN("tas_pause", tas_pause_start);
    RUN("tas_save_isolation", tas_save_isolation);
    RUN("tas_movies", tas_movie_replays);
    RUN("occupied", pending_and_occupied_file);
    RUN("intro", new_file_and_full_intro);
    RUN("erase_copy", deletion_and_copy_isolation);
    RUN("recovery", backup_recovery_and_invalid_save);
    RUN("failed_save_reload", failed_save_reload);
    RUN("failed_copy_menu_retry", failed_copy_menu_retry);
    RUN("palette_lock", palette_lock_and_death_reason);
    RUN("courtyard_boos", courtyard_boos);
    RUN("file_face_palette", file_face_palette);
    return test_finish();
}
