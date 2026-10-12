/* Production timer/save helpers and actual audio ABI commands; supplied DMA/clock/SRAM. */
#include <ultra64.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
static int sCheating;
#define game_options_secrets_blocked() sCheating
#define ACT_DEBUG_FREE_MOVE 0x0000130F

#define NUM_SAVE_FILES 4
#define UNUSED __attribute__((unused))
#define jrb_simpleflips_room_active() 0
#define jrb_simpleflips_exit_locked() 0
#define COURSE_COUNT 25
#define COURSE_STAGES_COUNT 15
#define COURSE_NONE 0
#define COURSE_NUM_TO_INDEX(n) ((n) - 1)
#define MENU_OPT_NONE 0
#define MENU_OPT_DEFAULT 1
#define MENU_MODE_RENDER_PAUSE_SCREEN 0
#define MENU_MODE_NONE -1
#define LEVEL_ENDING 99
#define CAM_MOVE_PAUSE_SCREEN 1
#define DISABLE_DEMO
#define DISABLE_EXIT_COURSE
#define PUPPYPRINT_PAGE_RAM 1
#define PUPPYPRINT_PAGE_LEVEL_SELECT 2
#define ALIGN32(n) (((n) + 31) & ~31)
#define VIRTUAL_TO_PHYSICAL2(p) ((uintptr_t) (p) & 0x1FFFFFFF)
typedef s16 Vec3s[3];
#include "pause_egg_definitions.inc"

static struct SaveBuffer gSaveBuffer, sSaveStorage;
static s8 gSaveFileModified;
static u8 sModifiedFiles;
static u8 sUnreadFiles;
static u32 sCastleSecretSavePending;
static u8 sSimpleFlipsGameOver;
static s32 gCurrSaveFileNum = 1, gCurrLevelNum = 5, gCurrAreaIndex = 2;
static void *gCurrDemoInput, *gCurrCreditsEntry;
static s16 gMenuOptSelectIndex;
static struct {
    u16 buttonDown;
    u16 buttonPressed;
    f32 stickMag;
} sController;
static __typeof__(sController) *gPlayer1Controller = &sController;
static u8 sBackgroundNoiseReasons = 1;
static u8 gMuteMusicPlayers = TRUE;
static OSTime sClock;
static u32 sWrites, sDmaCalls, sDmaBytes;
static uintptr_t sDmaAddress;
static u16 sRandom[8];
static u32 sRandomIndex;

/* Linker symbols delimit a short, aligned synthetic PCM clip, followed by zeros. */
__asm__(".balign 16\n.global gDunkeyPausePcmStart\ngDunkeyPausePcmStart:\n"
        ".space 600,0\n.global gDunkeyPausePcmEnd\ngDunkeyPausePcmEnd:\n.space 1296,0\n");
__asm__(".balign 16\n.global gYoshiCakePcmStart\ngYoshiCakePcmStart:\n"
        ".space 800,0\n.global gYoshiCakePcmEnd\ngYoshiCakePcmEnd:\n.space 1296,0\n");
static u8 sDmaBuffer[1296] __attribute__((aligned(16)));
/* Independent protected scream fixture; the ordinary quote fixtures stay above. */
__asm__(".balign 16\n.global gSimpleFlipsScreamPcmStart\ngSimpleFlipsScreamPcmStart:\n"
        ".space 640,0\n.global gSimpleFlipsScreamPcmEnd\ngSimpleFlipsScreamPcmEnd:\n.space 1296,0\n");

static u16 random_u16(void) {
    assert(sRandomIndex < 8);
    return sRandom[sRandomIndex++];
}
static OSTime osGetTimeFixture(void) {
    return sClock;
}
#define osGetTime osGetTimeFixture
static void save_main_menu_data(void) {
}
static void touch_high_score_ages(s32 fileIndex) {
    (void) fileIndex;
}
static s32 write_sram_data(void *data, size_t size) {
    size_t offset = (u8 *) data - (u8 *) &gSaveBuffer;
    assert(offset + size <= sizeof(sSaveStorage));
    memcpy((u8 *) &sSaveStorage + offset, data, size);
    sWrites++;
    return 0;
}
static s32 player_stats_checkpoint(s32 file) {
    assert(file >= 0 && file < NUM_SAVE_FILES);
    return 1;
}
static void player_stats_game_update(void) {
}
static void player_stats_erase(s32 file) {
    assert(file >= 0 && file < NUM_SAVE_FILES);
}
static void player_stats_copy(s32 source, s32 destination) {
    assert(source >= 0 && source < NUM_SAVE_FILES && destination >= 0 && destination < NUM_SAVE_FILES);
}
static s32 player_stats_can_copy(s32 source) {
    return source >= 0 && source < NUM_SAVE_FILES;
}
static void vec3s_set(Vec3s v, s16 x, s16 y, s16 z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}
static void vec3s_copy(Vec3s dest, Vec3s source) {
    memcpy(dest, source, sizeof(Vec3s));
}
static u32 save_file_get_flags(void) {
    return gSaveBuffer.files[gCurrSaveFileNum - 1][0].flags;
}
static void *dma_sample_data(uintptr_t address, u32 size, s32 flags, u8 *index) {
    assert((address & 15) == 0 && flags == 0);
    assert(size <= sizeof(sDmaBuffer));
    sDmaAddress = address;
    sDmaBytes = size;
    sDmaCalls++;
    *index = 0;
    return sDmaBuffer;
}
s32 pause_music_is_disabled(void);
s32 pause_pcm_start(void);
void pause_pcm_stop(void);
void save_file_set_flags(u32 flags);
void pause_easter_egg_begin(void);
void pause_easter_egg_end(void);
void set_play_mode(s16 playMode);

static s16 sCurrPlayMode, sTransitionTimer;
static s32 sPPDebugPage;
static s32 sDelayedWarpOp, sDialogID = -1, sTimerRunning;
static u8 sPauseTasCodeProgress;
static u16 gCameraMovementFlags;
static struct {
    s32 type;
} sWarpDest;
static struct {
    s32 isActive;
} gWarpTransition;
static struct {
    s16 timer;
} gHudDisplay;
static struct {
    u32 action;
    s16 health;
    u8 respawnHealCounter;
} sMario, *gMarioState = &sMario;
static struct {
    void *camera;
} sArea, *gCurrentArea = &sArea;
static u32 sObjectUpdates, sHudUpdates, sCameraUpdates, sWarpUpdates, sRumbleCancels, sMenuUpdates;

static s32 get_dialog_id(void) {
    return sDialogID;
}
static s32 castle_secret_lobby_active(void) {
    return FALSE;
}
#ifdef REONUCAM
static void save_reonucam_speed_on_unpause(void) {
}
#endif
static void lower_background_noise(s32 mode) {
    sBackgroundNoiseReasons |= mode;
}
static void raise_background_noise(s32 mode) {
    sBackgroundNoiseReasons &= ~mode;
}
static void cancel_rumble(void) {
    sRumbleCancels++;
}
static void set_menu_mode(s16 mode) {
    (void) mode;
    sMenuUpdates++;
}
static void warp_area(void) {
    sWarpUpdates++;
}
static void check_instant_warp(void) {
    sWarpUpdates++;
}
static void area_update_objects(void) {
    sObjectUpdates++;
    if (sController.buttonDown & (A_BUTTON | B_BUTTON | Z_TRIG | L_TRIG)) {
        sMario.action++;
    }
}
static void update_hud_values(void) {
    sHudUpdates++;
}
static void update_camera(void *camera) {
    (void) camera;
    sCameraUpdates++;
}
static void hud_update_auto_visibility(void *mario) {
    (void) mario;
    sHudUpdates++;
}
static void initiate_painting_warp(void) {
    sWarpUpdates++;
}
static void initiate_delayed_warp(void) {
    sWarpUpdates++;
}
static s32 mario_character_update_tas_code(u8 *progress, u16 buttons) {
    (void) progress;
    (void) buttons;
    return FALSE;
}
static void lvl_prepare_tas_thicc_start(void) {
}
static void lvl_restore_cap_on_file_start(s16 arg, s32 level) {
    (void) arg;
    (void) level;
}
static void lvl_init_from_save_file(s16 arg, s32 level) {
    (void) arg;
    (void) level;
}
static void reset_dialog_render_state(void) {
}
static void fade_into_special_warp(s32 warp, s32 arg) {
    (void) warp;
    (void) arg;
}
static void hud_reveal_after_pause(void) {
}
#define LEVEL_CASTLE_GROUNDS 16
#define WARP_SPECIAL_NONE 0
static void game_timing_checkpoint(s32 file) {
    (void) file;
}
static void game_timing_erase(s32 file) {
    (void) file;
}
static void game_timing_copy(s32 src, s32 dst) {
    (void) src;
    (void) dst;
}
static s32 game_timing_can_copy(s32 source) {
    return source >= 0 && source < NUM_SAVE_FILES;
}
static void update_paused_camera(void) {
}
static void game_timing_runtime_update(s32 running) {
    (void) running;
}
#define game_timing_end_course() ((void) 0)
#include "pause_egg_actual.inc"

void set_play_mode(s16 playMode) {
    sCurrPlayMode = playMode;
    if (playMode == PLAY_MODE_PAUSED) {
        pause_easter_egg_begin();
    } else {
        pause_easter_egg_end();
    }
}

static s32 read_empty_options(u32 offset, void *data, u32 size) {
    (void) offset;
    memset(data, 0, size);
    return 0;
}

static void reset(void) {
    // The fixture has readable default preferences and no auxiliary persistence.
    game_options_init(read_empty_options, NULL);
    memset(&gSaveBuffer, 0, sizeof(gSaveBuffer));
    memset(&sSaveStorage, 0, sizeof(sSaveStorage));
    memset(&sController, 0, sizeof(sController));
    memset(sRandom, 0, sizeof(sRandom));
    sRandomIndex = sWrites = sDmaCalls = 0;
    sModifiedFiles = sCastleSecretSavePending = gSaveFileModified = 0;
    sClock = 0;
    sCheating = 0;
    sBackgroundNoiseReasons = 1;
    gMuteMusicPlayers = TRUE;
    gCurrDemoInput = gCurrCreditsEntry = NULL;
    gCurrSaveFileNum = 1;
    gMenuOptSelectIndex = MENU_OPT_NONE;
    pause_easter_egg_end();
    sStreamingClip = FALSE;
}

static void test_timing(void) {
    reset();
    pause_easter_egg_begin();
    assert(sRandomIndex == 2 && (sPauseEasterEggState != PAUSE_EASTER_EGG_INACTIVE));
    sController.buttonDown = START_BUTTON;
    sClock = OS_USEC_TO_CYCLES(10000000ULL);
    pause_easter_egg_update();
    assert((sPauseEasterEggState == PAUSE_EASTER_EGG_WAITING_FOR_RELEASE) && sWrites == 0);
    sController.buttonDown = 0;
    pause_easter_egg_update();
    sClock += OS_USEC_TO_CYCLES(60000000ULL) - 1;
    pause_easter_egg_update();
    assert(sWrites == 0);
    sClock++;
    pause_easter_egg_update();
    assert(sWrites == 1 && sRequestedClip && (sPauseEasterEggState == PAUSE_EASTER_EGG_INACTIVE));
    for (int i = 0; i < 100; i++) {
        pause_easter_egg_update();
    }
    assert(sWrites == 1 && sRandomIndex == 2);
    pause_easter_egg_end();
    assert(!sRequestedClip);
    /* Restart with only persisted bytes, then try another pause. */
    memcpy(&gSaveBuffer, &sSaveStorage, sizeof(gSaveBuffer));
    pause_easter_egg_begin();
    assert((sPauseEasterEggState == PAUSE_EASTER_EGG_INACTIVE) && sRandomIndex == 2);

    reset();
    sRandom[1] = 180;
    pause_easter_egg_begin();
    pause_easter_egg_update();
    sClock = OS_USEC_TO_CYCLES(240000000ULL) - 1;
    pause_easter_egg_update();
    assert(sWrites == 0);
    sClock++;
    pause_easter_egg_update();
    assert(sWrites == 1);
}

static void test_inputs_and_odds(void) {
    for (u32 input = 0; input < 4; input++) {
        reset();
        pause_easter_egg_begin();
        pause_easter_egg_update();
        if (input == 0) {
            sController.buttonDown = A_BUTTON;
        }
        if (input == 1) {
            sController.buttonDown = START_BUTTON;
        }
        if (input == 2) {
            sController.stickMag = 1.0f;
        }
        if (input == 3) {
            gMenuOptSelectIndex = 1;
        }
        pause_easter_egg_update();
        memset(&sController, 0, sizeof(sController));
        gMenuOptSelectIndex = MENU_OPT_NONE;
        sClock = OS_USEC_TO_CYCLES(240000000ULL);
        pause_easter_egg_update();
        assert((sPauseEasterEggState == PAUSE_EASTER_EGG_INACTIVE) && sWrites == 0
               && sRandomIndex == 2);
    }
    reset();
    sRandom[0] = 1;
    pause_easter_egg_begin();
    assert((sPauseEasterEggState == PAUSE_EASTER_EGG_INACTIVE) && sRandomIndex == 1);
    for (int i = 0; i < 100; i++) {
        pause_easter_egg_update();
    }
    assert(sRandomIndex == 1);
    sRandom[1] = 0;
    pause_easter_egg_begin();
    assert((sPauseEasterEggState != PAUSE_EASTER_EGG_INACTIVE) && sRandomIndex == 3);
    /* Every 16-bit value: exactly 8,192 of 65,536 win. */
    u32 winners = 0;
    for (u32 value = 0; value < 65536; value++) {
        sRandomIndex = 0;
        sRandom[0] = value;
        winners += pause_easter_egg_random(PAUSE_EASTER_EGG_ODDS) == 0;
    }
    assert(winners == 8192);
    sRandomIndex = 0;
    sRandom[0] = 65535;
    sRandom[1] = 181;
    /* The non-power-of-two delay range still rejects biased tail values. */
    assert(pause_easter_egg_random(181) == 0 && sRandomIndex == 2);
    for (int excluded = 0; excluded < 6; excluded++) {
        reset();
        if (excluded == 0) {
            sBackgroundNoiseReasons = 0;
        }
        if (excluded == 1) {
            gCurrDemoInput = &sController;
        }
        if (excluded == 2) {
            gCurrCreditsEntry = &sController;
        }
        if (excluded == 3) {
            gCurrSaveFileNum = 0;
        }
        if (excluded == 4) {
            gCurrSaveFileNum = 5;
        }
        if (excluded == 5) {
            gMuteMusicPlayers = FALSE;
        }
        pause_easter_egg_begin();
        assert((sPauseEasterEggState == PAUSE_EASTER_EGG_INACTIVE) && sRandomIndex == 0);
    }
    reset();
    pause_easter_egg_begin();
    pause_easter_egg_update();
    sBackgroundNoiseReasons = 0;
    pause_easter_egg_update();
    assert((sPauseEasterEggState == PAUSE_EASTER_EGG_INACTIVE) && sWrites == 0);
}

static void test_save_and_cap(void) {
    Vec3s cap;
    reset();
    assert(sizeof(struct SaveFile) == 56);
    gSaveBuffer.files[0][0].flags = SAVE_FLAG_GAME_COMPLETED | SAVE_FLAG_COLLECTED_MIPS_STAR_2;
    save_file_set_cap_pos(10, 20, 30);
    save_file_mark_pause_easter_egg();
    assert(sWrites == 1 && save_file_has_pause_easter_egg(0));
    assert(!save_file_has_pause_easter_egg(1) && !save_file_has_pause_easter_egg(-1));
    assert(verify_save_block_signature(&sSaveStorage.files[0][0], 56, SAVE_FILE_MAGIC));
    assert(verify_save_block_signature(&sSaveStorage.files[0][1], 56, SAVE_FILE_MAGIC));
    assert(save_file_get_cap_pos(cap) && cap[0] == 10 && cap[1] == 20 && cap[2] == 30);
    gCurrAreaIndex = 3;
    assert(!save_file_get_cap_pos(cap));
    save_file_set_cap_pos(40, 50, 60);
    assert(save_file_has_pause_easter_egg(0) && save_file_get_cap_pos(cap) && cap[0] == 40);
    save_file_do_save(0);
    assert(gSaveBuffer.files[0][0].flags & SAVE_FLAG_GAME_COMPLETED);
    assert(gSaveBuffer.files[0][0].flags & SAVE_FLAG_COLLECTED_MIPS_STAR_2);
    save_file_copy(0, 1);
    assert(save_file_has_pause_easter_egg(1));
    save_file_erase(1);
    assert(!save_file_has_pause_easter_egg(1) && save_file_has_pause_easter_egg(0));
    gCurrSaveFileNum = 2;
    gCurrDemoInput = &cap;
    save_file_mark_pause_easter_egg();
    assert(!save_file_has_pause_easter_egg(1));
}

static void test_simpleflips_save(void) {
    Vec3s cap;
    u32 writes;
    u32 progress = SAVE_FLAG_GAME_COMPLETED | SAVE_FLAG_COLLECTED_MIPS_STAR_2;

    reset();
    gSaveBuffer.files[0][0].flags = progress;
    gSaveBuffer.files[0][0].capArea = SAVE_CAP_AREA_PAUSE_EASTER_EGG;
    save_file_set_cap_pos(10, 20, 30);
    save_file_mark_simpleflips_encounter();
    assert(sWrites == 1 && save_file_has_simpleflips_encounter(0));
    assert(!save_file_has_simpleflips_encounter(1));
    assert(!save_file_has_simpleflips_encounter(-1) && !save_file_has_simpleflips_encounter(4));
    assert(gSaveBuffer.files[0][0].flags
           == (progress | SAVE_FLAG_FILE_EXISTS | SAVE_FLAG_CAP_ON_GROUND));
    assert(save_file_has_pause_easter_egg(0));
    assert(save_file_get_cap_pos(cap) && cap[0] == 10 && cap[1] == 20 && cap[2] == 30);
    assert(verify_save_block_signature(&sSaveStorage.files[0][0], 56, SAVE_FILE_MAGIC));
    assert(verify_save_block_signature(&sSaveStorage.files[0][1], 56, SAVE_FILE_MAGIC));
    assert(sSaveStorage.files[0][1].capLevel & SAVE_CAP_LEVEL_SIMPLEFLIPS);
    save_file_mark_simpleflips_encounter();
    assert(sWrites == 1);
    gCurrLevelNum = 12;
    save_file_set_cap_pos(40, 50, 60);
    assert(save_file_has_simpleflips_encounter(0) && save_file_get_cap_pos(cap) && cap[0] == 40);
    save_file_do_save(0);
    memset(&gSaveBuffer, 0, sizeof(gSaveBuffer));
    memcpy(&gSaveBuffer, &sSaveStorage, sizeof(gSaveBuffer));
    assert(save_file_has_simpleflips_encounter(0) && save_file_get_cap_pos(cap) && cap[0] == 40);
    save_file_copy(0, 1);
    assert(save_file_has_simpleflips_encounter(1));
    save_file_erase(0);
    assert(!save_file_has_simpleflips_encounter(0) && save_file_has_simpleflips_encounter(1));
    writes = sWrites;
    for (s32 excluded = 0; excluded < 4; excluded++) {
        gCurrSaveFileNum = 1;
        if (excluded == 2) {
            gCurrSaveFileNum = 0;
        } else if (excluded == 3) {
            gCurrSaveFileNum = 5;
        }
        gCurrDemoInput = excluded == 0 ? &cap : NULL;
        gCurrCreditsEntry = excluded == 1 ? &cap : NULL;
        save_file_mark_simpleflips_encounter();
        assert(sWrites == writes && !save_file_has_simpleflips_encounter(0));
    }
    gCurrSaveFileNum = 1;
    gCurrDemoInput = gCurrCreditsEntry = NULL;
    save_file_mark_simpleflips_encounter();
    assert(sWrites == writes + 1 && save_file_has_simpleflips_encounter(0));
    assert(save_file_has_simpleflips_encounter(1));
    puts("PASS SimpleFlips save: immediate persistence, cap compatibility, independent files, copy and "
         "erase");
}

static void test_simpleflips_reboot(void) {
    Vec3s cap;

    reset();
    sSimpleFlipsGameOver = FALSE;
    assert(!lvl_simpleflips_boot(0, 0) && !jrb_simpleflips_is_game_over());
    gCurrSaveFileNum = 3;
    save_file_mark_simpleflips_encounter();
    assert(!lvl_simpleflips_boot(0, 0));
    save_file_set_cap_pos(11, 22, 33);
    save_file_mark_simpleflips_game_over();
    assert(!jrb_simpleflips_is_game_over());
    assert(save_file_get_cap_pos(cap) && cap[0] == 11);
    assert(verify_save_block_signature(&sSaveStorage.files[2][0], 56, SAVE_FILE_MAGIC));
    assert(verify_save_block_signature(&sSaveStorage.files[2][1], 56, SAVE_FILE_MAGIC));
    // A later cap save must preserve both encounter metadata bits.
    save_file_set_cap_pos(44, 55, 66);
    save_file_do_save(2);
    assert(sSaveStorage.files[2][1].capLevel & SAVE_CAP_LEVEL_SIMPLEFLIPS_GAME_OVER);
    memset(&gSaveBuffer, 0, sizeof(gSaveBuffer));
    memcpy(&gSaveBuffer, &sSaveStorage, sizeof(gSaveBuffer));
    gCurrSaveFileNum = 1;
    assert(lvl_simpleflips_boot(0, 0) && jrb_simpleflips_is_game_over());
    assert(gCurrSaveFileNum == 3 && save_file_has_simpleflips_encounter(2));
    assert(save_file_get_cap_pos(cap) && cap[0] == 44);
    for (s32 slot = 0; slot < 2; slot++) {
        assert(!(sSaveStorage.files[2][slot].capLevel & SAVE_CAP_LEVEL_SIMPLEFLIPS_GAME_OVER));
        assert(verify_save_block_signature(&sSaveStorage.files[2][slot], 56, SAVE_FILE_MAGIC));
    }
    // A second reset cannot replay this crash's game-over screen.
    sSimpleFlipsGameOver = FALSE;
    memcpy(&gSaveBuffer, &sSaveStorage, sizeof(gSaveBuffer));
    assert(!lvl_simpleflips_boot(0, 0) && !jrb_simpleflips_is_game_over());
    assert(save_file_has_simpleflips_encounter(2));
    u32 writes = sWrites;
    for (s32 excluded = 0; excluded < 4; excluded++) {
        gCurrSaveFileNum = excluded == 2 ? 0 : 1;
        if (excluded == 3) {
            gCurrSaveFileNum = NUM_SAVE_FILES + 1;
        }
        gCurrDemoInput = excluded == 0 ? &cap : NULL;
        gCurrCreditsEntry = excluded == 1 ? &cap : NULL;
        save_file_mark_simpleflips_game_over();
        assert(sWrites == writes && !save_file_consume_simpleflips_game_over());
    }
    gCurrDemoInput = gCurrCreditsEntry = NULL;
    puts("PASS SimpleFlips reboot: durable one-shot Toad game over, backup checksums and cap data");
}

static void test_pcm_commands(void) {
    Acmd commands[16];
    Acmd *start = commands;
    reset();
    assert(pause_pcm_mix(start, 272, 0, 0x4C0, 0x600) == start);
    assert(pause_pcm_start());
    Acmd *end = pause_pcm_mix(start, 272, 0, 0x4C0, 0x600);
    assert(end == start + 4 && sDmaCalls == 1 && sDmaBytes == 288);
    assert(sDmaAddress == (uintptr_t) gDunkeyPausePcmStart);
    assert(commands[0].words.w0 >> 24 == A_SETBUFF && (commands[0].words.w1 & 0xFFFF) == 288);
    assert(commands[1].words.w0 >> 24 == A_LOADBUFF);
    assert(commands[2].words.w0 >> 24 == A_MIXER && commands[2].words.w1 == 0x4C0);
    assert(commands[3].words.w0 >> 24 == A_MIXER && commands[3].words.w1 == 0x600);
    assert((commands[2].words.w0 & 0xFFFF) == 0x7FFF);
    assert(sStreamByteOffset == 272 && sRequestedClip);
    pause_pcm_mix(start, 288, 0, 0x4C0, 0x600);
    assert(sDmaAddress == (uintptr_t) gDunkeyPausePcmStart + 272 && sRequestedClip);
    pause_pcm_mix(start, 256, 0, 0x4C0, 0x600);
    assert(!sRequestedClip && sStreamByteOffset == 816);
    assert(sDmaAddress + sDmaBytes < (uintptr_t) gDunkeyPausePcmEnd + 1296);
    assert(pause_pcm_mix(start, 272, 0, 0x4C0, 0x600) == start);
    pause_pcm_start();
    pause_pcm_mix(start, 272, 0, 0x4C0, 0x600);
    assert(sDmaAddress == (uintptr_t) gDunkeyPausePcmStart);
    pause_pcm_stop();
    assert(pause_pcm_mix(start, 272, 0, 0x4C0, 0x600) == start);
}

static void test_pause_action_priority(void) {
    const u16 chords[] = { A_BUTTON, B_BUTTON, Z_TRIG, L_TRIG | Z_TRIG, A_BUTTON | B_BUTTON | Z_TRIG };

    for (u32 i = 0; i < sizeof(chords) / sizeof(chords[0]); i++) {
        reset();
        sMario.action = 0;
        sMario.health = 0x880;
        sDialogID = -1;
        sDelayedWarpOp = WARP_OP_NONE;
        sTransitionTimer = 0;
        sWarpDest.type = WARP_TYPE_NOT_WARPING;
        gWarpTransition.isActive = FALSE;
        sCurrPlayMode = PLAY_MODE_NORMAL;
        sObjectUpdates = sHudUpdates = sCameraUpdates = sWarpUpdates = 0;
        sMenuUpdates = sRumbleCancels = gCameraMovementFlags = 0;
        sTimerRunning = TRUE;
        gHudDisplay.timer = 100;
        sController.buttonDown = sController.buttonPressed = START_BUTTON | chords[i];
        assert(play_mode_normal() == FALSE);
        assert(sCurrPlayMode == PLAY_MODE_PAUSED && sMario.action == 0);
        assert(sObjectUpdates == 0 && sHudUpdates == 0 && sCameraUpdates == 0 && sWarpUpdates == 0);
        assert(gHudDisplay.timer == 100 && sMenuUpdates == 1 && sRumbleCancels == 1);
        assert(gCameraMovementFlags & CAM_MOVE_PAUSE_SCREEN);
        play_mode_paused();
        assert(sObjectUpdates == 0 && sMario.action == 0);
        gMenuOptSelectIndex = MENU_OPT_DEFAULT;
        play_mode_paused();
        assert(sCurrPlayMode == PLAY_MODE_NORMAL && sObjectUpdates == 0);
        gMenuOptSelectIndex = MENU_OPT_NONE;
        sController.buttonDown = sController.buttonPressed = 0;
        play_mode_normal();
        assert(sObjectUpdates == 1 && sMario.action == 0 && gHudDisplay.timer == 101);
    }
    // Blocked pause presses must still deliver simultaneous gameplay input.
    for (u32 blocked = 0; blocked < 5; blocked++) {
        reset();
        sCurrPlayMode = PLAY_MODE_NORMAL;
        sMario.action = blocked == 0 ? ACT_FLAG_INTANGIBLE : 0;
        sMario.health = 0x880;
        sDialogID = blocked == 1 ? 0 : -1;
        gWarpTransition.isActive = blocked == 2;
        sDelayedWarpOp = blocked == 3 ? WARP_OP_DEATH : WARP_OP_NONE;
        sTransitionTimer = blocked == 4 ? 1 : 0;
        sWarpDest.type = WARP_TYPE_NOT_WARPING;
        sObjectUpdates = 0;
        sController.buttonDown = sController.buttonPressed = START_BUTTON | B_BUTTON;
        u32 action = sMario.action;
        play_mode_normal();
        assert(sCurrPlayMode != PLAY_MODE_PAUSED && sObjectUpdates == 1 && sMario.action == action + 1);
    }
    puts("PASS pause priority: action chords, frozen opening update, resume and rejected pause inputs");
}

static void test_cheating_blocks_start_and_pending_quote(void) {
    reset();
    sCheating = 1;
    pause_easter_egg_begin();
    assert(sPauseEasterEggState == PAUSE_EASTER_EGG_INACTIVE && sRandomIndex == 0);
    save_file_mark_pause_easter_egg();
    assert(!save_file_has_pause_easter_egg(0) && sWrites == 0);
    sCheating = 0;
    pause_easter_egg_begin();
    assert(sPauseEasterEggState == PAUSE_EASTER_EGG_WAITING_FOR_RELEASE);
    sCheating = 1;
    pause_easter_egg_update();
    assert(sPauseEasterEggState == PAUSE_EASTER_EGG_INACTIVE && sWrites == 0);
}
int main(void) {
    test_cheating_blocks_start_and_pending_quote();
    test_timing();
    test_inputs_and_odds();
    test_save_and_cap();
    test_simpleflips_save();
    test_simpleflips_reboot();
    test_pcm_commands();
    test_pause_action_priority();
    puts("PASS pause easter egg: timing, odds, input, persistence, cap compatibility and PCM commands");
    return 0;
}
