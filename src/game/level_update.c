#include "player_stats.h"
#include "game_options.h"
#include "game_options_runtime.h"
#include "game_timing.h"
#include "game_timing_runtime.h"
#include <ultra64.h>

#include "sm64.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "audio/external.h"
#include "audio/synthesis.h"
#include "audio/pause_pcm.h"
#include "debugger/assert.h"
#include "level_update.h"
#include "jrb_simpleflips.h"
#include "game_init.h"
#include "hud.h"
#include "level_update.h"
#include "main.h"
#include "engine/math_util.h"
#include "engine/graph_node.h"
#include "engine/surface_collision.h"
#include "behavior_data.h"
#include "area.h"
#include "save_file.h"
#include "sound_init.h"
#include "mario.h"
#include "mario_geo_switch_case_ids.h"
#include "mario_character.h"
#include "simpleflips_scream.h"
#include "camera.h"
#include "castle_lobby_finale.h"
#include "object_list_processor.h"
#include "object_platform_motion.h"
#include "ingame_menu.h"
#include "obj_behaviors.h"
#include "save_file.h"
#if MULTILANG
#include "memory.h"
#include "eu_translation.h"
#include "segment_symbols.h"
#endif
#include "level_table.h"
#include "course_table.h"
#include "rumble_init.h"
#include "puppyprint.h"
#include "level_commands.h"
#include "debug.h"
#include "pause_easter_egg.h"

#include "config.h"

// HACKERSM64_DO: Make these ifdefs better
const char *credits01[] = { "1GAME DIRECTOR", "SHIGERU MIYAMOTO" };
const char *credits02[] = { "2ASSISTANT DIRECTORS", "YOSHIAKI KOIZUMI", "TAKASHI TEZUKA" };
const char *credits03[] = { "2SYSTEM PROGRAMMERS", "YASUNARI NISHIDA", "YOSHINORI TANIMOTO" };
const char *credits04[] = { "3PROGRAMMERS", "HAJIME YAJIMA", "DAIKI IWAMOTO", "TOSHIO IWAWAKI" };

#if defined(VERSION_JP) || defined(VERSION_SH)

const char *credits05[] = { "1CAMERA PROGRAMMER", "TAKUMI KAWAGOE" };
const char *credits06[] = { "1MARIO FACE PROGRAMMER", "GILES GODDARD" };
const char *credits07[] = { "2COURSE DIRECTORS", "YOICHI YAMADA", "YASUHISA YAMAMURA" };
const char *credits08[] = { "2COURSE DESIGNERS", "KENTA USUI", "NAOKI MORI" };
const char *credits09[] = { "3COURSE DESIGNERS", "YOSHIKI HARUHANA", "MAKOTO MIYANAGA", "KATSUHIKO KANNO" };
const char *credits10[] = { "1SOUND COMPOSER", "KOJI KONDO" };

// Shindou combines sound effects and sound programmer in order to make room for Mario voice and Peach voice
const char *credits11[] = { "4SOUND EFFECTS", "SOUND PROGRAMMER", "YOJI INAGAKI", "HIDEAKI SHIMIZU" };
const char *credits12[] = { "23D ANIMATORS", "YOSHIAKI KOIZUMI", "SATORU TAKIZAWA" };
const char *credits13[] = { "1CG DESIGNER", "MASANAO ARIMOTO" };
const char *credits14[] = { "3TECHNICAL SUPPORT", "TAKAO SAWANO", "HIROHITO YOSHIMOTO", "HIROTO YADA" };
const char *credits15[] = { "1TECHNICAL SUPPORT", "SGI. 64PROJECT STAFF" };
const char *credits16[] = { "2PROGRESS MANAGEMENT", "KIMIYOSHI FUKUI", "KEIZO KATO" };

#else // VERSION_US || VERSION_EU

// US and EU combine camera programmer and Mario face programmer...
const char *credits05[] = { "4CAMERA PROGRAMMER", "MARIO FACE PROGRAMMER", "TAKUMI KAWAGOE", "GILES GODDARD" };
const char *credits06[] = { "2COURSE DIRECTORS", "YOICHI YAMADA", "YASUHISA YAMAMURA" };
const char *credits07[] = { "2COURSE DESIGNERS", "KENTA USUI", "NAOKI MORI" };
const char *credits08[] = { "3COURSE DESIGNERS", "YOSHIKI HARUHANA", "MAKOTO MIYANAGA", "KATSUHIKO KANNO" };

#ifdef VERSION_US
const char *credits09[] = { "1SOUND COMPOSER", "KOJI KONDO" };
// ...as well as sound effects and sound programmer in order to make room for screen text writer, Mario voice, and Peach voice
const char *credits10[] = { "4SOUND EFFECTS", "SOUND PROGRAMMER", "YOJI INAGAKI", "HIDEAKI SHIMIZU" };
const char *credits11[] = { "23-D ANIMATORS", "YOSHIAKI KOIZUMI", "SATORU TAKIZAWA" };
const char *credits12[] = { "1ADDITIONAL GRAPHICS", "MASANAO ARIMOTO" };
const char *credits13[] = { "3TECHNICAL SUPPORT", "TAKAO SAWANO", "HIROHITO YOSHIMOTO", "HIROTO YADA" };
const char *credits14[] = { "1TECHNICAL SUPPORT", "SGI N64 PROJECT STAFF" };
const char *credits15[] = { "2PROGRESS MANAGEMENT", "KIMIYOSHI FUKUI", "KEIZO KATO" };
const char *credits16[] = { "5SCREEN TEXT WRITER", "TRANSLATION", "LESLIE SWAN", "MINA AKINO",
                            "HIRO YAMADA" };
#else // VERSION_EU
// ...as well as sound composer, sound effects, and sound programmer, and...
const char *credits09[] = { "7SOUND COMPOSER", "SOUND EFFECTS", "SOUND PROGRAMMER", "KOJI KONDO", "YOJI INAGAKI", "HIDEAKI SHIMIZU" };
// ...3D animators and additional graphics in order to make room for screen text writer(s), Mario voice, and Peach voice
const char *credits10[] = { "63-D ANIMATORS", "ADDITIONAL GRAPHICS", "YOSHIAKI KOIZUMI", "SATORU TAKIZAWA", "MASANAO ARIMOTO" };
const char *credits11[] = { "3TECHNICAL SUPPORT", "TAKAO SAWANO", "HIROHITO YOSHIMOTO", "HIROTO YADA" };
const char *credits12[] = { "1TECHNICAL SUPPORT", "SGI N64 PROJECT STAFF" };
const char *credits13[] = { "2PROGRESS MANAGEMENT", "KIMIYOSHI FUKUI", "KEIZO KATO" };
const char *credits14[] = { "5SCREEN TEXT WRITER", "ENGLISH TRANSLATION", "LESLIE SWAN", "MINA AKINO", "HIRO YAMADA" };
const char *credits15[] = { "4SCREEN TEXT WRITER", "FRENCH TRANSLATION", "JULIEN BARDAKOFF", "KENJI HARAGUCHI" };
const char *credits16[] = { "4SCREEN TEXT WRITER", "GERMAN TRANSLATION", "THOMAS GOERG", "THOMAS SPINDLER" };
#endif

#endif

const char *credits17[] = { "4MARIO VOICE", "PEACH VOICE", "CHARLES MARTINET", "LESLIE SWAN" };

#if defined(VERSION_JP) || defined(VERSION_SH)
const char *credits18[] = { "3SPECIAL THANKS TO", "JYOHO KAIHATUBU", "ALL NINTENDO", "MARIO CLUB STAFF" };
#else // VERSION_US | VERSION_EU
const char *credits18[] = { "3SPECIAL THANKS TO", "EAD STAFF", "ALL NINTENDO PERSONNEL", "SUPER MARIO CLUB STAFF" };
#endif

const char *credits19[] = { "1PRODUCER", "SHIGERU MIYAMOTO" };
const char *credits20[] = { "1EXECUTIVE PRODUCER", "HIROSHI YAMAUCHI" };


struct CreditsEntry sCreditsSequence[] = {
    { LEVEL_CASTLE_GROUNDS, 1, 1, -128, { 0, 8000, 0 }, NULL },
    { LEVEL_BOB, 1, 1, 117, { 713, 3918, -3889 }, credits01 },
    { LEVEL_WF, 1, 50, 46, { 347, 5376, 326 }, credits02 },
    { LEVEL_JRB, 1, 18, 22, { 3800, -4840, 2727 }, credits03 },
    { LEVEL_CCM, 2, 34, 25, { -5464, 6656, -6575 }, credits04 },
    { LEVEL_BBH, 1, 1, 60, { 257, 1922, 2580 }, credits05 },
    { LEVEL_HMC, 1, -15, 123, { -6469, 1616, -6054 }, credits06 },
    { LEVEL_THI, 3, 17, -32, { 508, 1024, 1942 }, credits07 },
    { LEVEL_LLL, 2, 33, 124, { -73, 82, -1467 }, credits08 },
    { LEVEL_SSL, 1, 65, 98, { -5906, 1024, -2576 }, credits09 },
    { LEVEL_DDD, 1, 50, 47, { -4884, -4607, -272 }, credits10 },
    { LEVEL_SL, 1, 17, -34, { 1925, 3328, 563 }, credits11 },
    { LEVEL_WDW, 1, 33, 105, { -537, 1850, 1818 }, credits12 },
    { LEVEL_TTM, 1, 2, -33, { 2613, 313, 1074 }, credits13 },
    { LEVEL_THI, 1, 51, 54, { -2609, 512, 856 }, credits14 },
    { LEVEL_TTC, 1, 17, -72, { -1304, -71, -967 }, credits15 },
    { LEVEL_RR, 1, 33, 64, { 1565, 1024, -148 }, credits16 },
    { LEVEL_SA, 1, 1, 24, { -1050, -1330, -1559 }, credits17 },
    { LEVEL_COTMC, 1, 49, -16, { -254, 415, -6045 }, credits18 },
    { LEVEL_DDD, 2, -111, -64, { 3948, 1185, -104 }, credits19 },
    { LEVEL_CCM, 1, 33, 31, { 3169, -4607, 5240 }, credits20 },
    { LEVEL_CASTLE_GROUNDS, 1, 1, -128, { 0, 906, -1200 }, NULL },
    { LEVEL_NONE, 0, 1, 0, { 0, 0, 0 }, NULL },
};

struct MarioState gMarioStates[1];
struct HudDisplay gHudDisplay;
s16 sCurrPlayMode;
static u8 sPauseTasCodeProgress;
static s32 sTasThiccStartPending;
s16 sTransitionTimer;
void (*sTransitionUpdate)(s16 *);
struct WarpDest sWarpDest;
static s32 sHealOnCourseEntry;
s16 sSpecialWarpDest;
s16 sDelayedWarpOp;
s16 sDelayedWarpTimer;
s16 sSourceWarpNodeId;
s32 sDelayedWarpArg;
s8 sTimerRunning;
s8 gNeverEnteredCastle;
// Prevent multiple 100 coin stars from spawning
u8 g100CoinStarSpawned = FALSE;

struct MarioState *gMarioState = &gMarioStates[0];
s8 sWarpCheckpointActive = FALSE;

u16 level_control_timer(s32 timerOp) {
    switch (timerOp) {
        case TIMER_CONTROL_SHOW:
            gHudDisplay.flags |= HUD_DISPLAY_FLAG_TIMER;
            sTimerRunning = FALSE;
            gHudDisplay.timer = 0;
            break;

        case TIMER_CONTROL_START:
            sTimerRunning = TRUE;
            break;

        case TIMER_CONTROL_STOP:
            sTimerRunning = FALSE;
            break;

        case TIMER_CONTROL_HIDE:
            gHudDisplay.flags &= ~HUD_DISPLAY_FLAG_TIMER;
            sTimerRunning = FALSE;
            gHudDisplay.timer = 0;
            break;
    }

    return gHudDisplay.timer;
}

s32 level_is_ending(void) {
    return gCurrLevelNum == LEVEL_ENDING || gMarioState->action == ACT_END_PEACH_CUTSCENE;
}

s32 castle_secret_lobby_active(void) {
    return gCurrLevelNum == LEVEL_CASTLE && gCurrAreaIndex == 4;
}

// Check actual saved stars before debug eligibility; debug never supplies missing stars here.
s32 castle_secret_lobby_rejection_dialog(void) {
    if (!castle_secret_lobby_active()) {
        return -1;
    }
    if (save_file_get_saved_star_count(gCurrSaveFileNum - 1) < CASTLE_ROOF_COMPLETION_STARS) {
        return DIALOG_CASTLE_LOBBY_UNEARNED_ENDING;
    }
#if defined(UNLOCK_ALL) || defined(COMPLETE_SAVE_FILE)
    return DIALOG_CASTLE_LOBBY_DEBUG_STARS;
#else
    return game_options_secrets_blocked() ? DIALOG_CASTLE_LOBBY_DEBUG_STARS : -1;
#endif
}

u32 pressed_pause(void) {
    if (jrb_simpleflips_exit_locked() || castle_secret_lobby_active()) {
        return FALSE;
    }

    u32 dialogActive = get_dialog_id() >= 0;
    u32 intangible =
        (gMarioState->action & ACT_FLAG_INTANGIBLE) != 0 && gMarioState->action != ACT_DEBUG_FREE_MOVE;
    u32 zeroHealth = gMarioState->health < 0x100;

#ifdef PUPPYPRINT_DEBUG
    if (sPPDebugPage == PUPPYPRINT_PAGE_RAM || sPPDebugPage == PUPPYPRINT_PAGE_LEVEL_SELECT
#ifdef BETTER_REVERB
        || sPPDebugPage == PUPPYPRINT_PAGE_BETTER_REVERB
#endif
    ) {
        return FALSE;
    }
#endif

    if ((!intangible || zeroHealth || level_is_ending()) && !dialogActive && !gWarpTransition.isActive
        && (sDelayedWarpOp == WARP_OP_NONE || (zeroHealth && sDelayedWarpOp == WARP_OP_DEATH))
        && (gPlayer1Controller->buttonPressed & START_BUTTON)) {
        return TRUE;
    }

    return FALSE;
}

void set_play_mode(s16 playMode) {
    if (sCurrPlayMode == PLAY_MODE_PAUSED && playMode != PLAY_MODE_PAUSED) {
        game_options_save(game_options_active_file());
    }
    sPauseTasCodeProgress = 0;
#ifdef REONUCAM
    if (playMode != PLAY_MODE_NORMAL) {
        reonucam_reset_r_tap();
    }
#endif
    sCurrPlayMode = playMode;
    if (playMode == PLAY_MODE_PAUSED) {
        pause_easter_egg_begin();
    } else {
        pause_easter_egg_end();
    }
}

void warp_special(s32 arg) {
    if (jrb_simpleflips_exit_locked()
        && !(jrb_simpleflips_is_game_over() && arg == WARP_SPECIAL_MARIO_HEAD_DIZZY)) {
        return;
    }
    set_play_mode(PLAY_MODE_CHANGE_LEVEL);
    sSpecialWarpDest = arg;
}

void fade_into_special_warp(u32 arg, u32 color) {
    if (jrb_simpleflips_exit_locked()) {
        return;
    }
    if (color != 0) {
        color = 0xFF;
    }

    fadeout_music(190);
    play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 0x10, color, color, color);
    level_set_transition(30, NULL);

    warp_special(arg);
}

void load_level_init_text(u32 arg) {
    s32 gotAchievement;
    u32 dialogID = gCurrentArea->dialog[arg];

    switch (dialogID) {
        case DIALOG_129:
            gotAchievement = save_file_get_flags() & SAVE_FLAG_HAVE_VANISH_CAP;
            break;

        case DIALOG_130:
            gotAchievement = save_file_get_flags() & SAVE_FLAG_HAVE_METAL_CAP;
            break;

        case DIALOG_131:
            gotAchievement = save_file_get_flags() & SAVE_FLAG_HAVE_WING_CAP;
            break;

        case (u8)DIALOG_NONE: // 255, cast value to u8 to match (-1)
            gotAchievement = TRUE;
            break;

        default:
            gotAchievement =
                save_file_get_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(gCurrCourseNum));
            break;
    }

    if (!gotAchievement) {
        level_set_transition(-1, NULL);
        create_dialog_box(dialogID);
    }
}

void init_door_warp(struct SpawnInfo *spawnInfo, u32 warpDestFlags) {
    if (warpDestFlags & WARP_FLAG_DOOR_FLIP_MARIO) {
        spawnInfo->startAngle[1] += 0x8000;
    }

    spawnInfo->startPos[0] += 300.0f * sins(spawnInfo->startAngle[1]);
    spawnInfo->startPos[2] += 300.0f * coss(spawnInfo->startAngle[1]);
}

void set_mario_initial_cap_powerup(struct MarioState *m) {
    u32 capCourseIndex = gCurrCourseNum - COURSE_CAP_COURSES;

    switch (capCourseIndex) {
        case COURSE_COTMC - COURSE_CAP_COURSES:
            mario_add_cap_power(m, MARIO_METAL_CAP | MARIO_CAP_ON_HEAD);
            m->capTimer = 600;
            break;

        case COURSE_TOTWC - COURSE_CAP_COURSES:
            mario_add_cap_power(m, MARIO_WING_CAP | MARIO_CAP_ON_HEAD);
            m->capTimer = 1200;
            break;

        case COURSE_VCUTM - COURSE_CAP_COURSES:
            mario_add_cap_power(m, MARIO_VANISH_CAP | MARIO_CAP_ON_HEAD);
            m->capTimer = 600;
            break;
    }
}

void set_mario_initial_action(struct MarioState *m, u32 spawnType, u32 actionArg) {
    switch (spawnType) {
        case MARIO_SPAWN_DOOR_WARP:
            set_mario_action(m, ACT_WARP_DOOR_SPAWN, actionArg);
            break;
        case MARIO_SPAWN_IDLE:
            set_mario_action(m, ACT_IDLE, 0);
            break;
        case MARIO_SPAWN_PIPE:
            set_mario_action(m, ACT_EMERGE_FROM_PIPE, 0);
            break;
        case MARIO_SPAWN_TELEPORT:
            set_mario_action(m, ACT_TELEPORT_FADE_IN, 0);
            break;
        case MARIO_SPAWN_INSTANT_ACTIVE:
            set_mario_action(m, ACT_IDLE, 0);
            break;
        case MARIO_SPAWN_AIRBORNE:
            set_mario_action(m, ACT_SPAWN_NO_SPIN_AIRBORNE, 0);
            break;
        case MARIO_SPAWN_HARD_AIR_KNOCKBACK:
            set_mario_action(m, ACT_HARD_BACKWARD_AIR_KB, 0);
            break;
        case MARIO_SPAWN_SPIN_AIRBORNE_CIRCLE:
            set_mario_action(m, ACT_SPAWN_SPIN_AIRBORNE, 0);
            break;
        case MARIO_SPAWN_DEATH:
            set_mario_action(m, ACT_FALLING_DEATH_EXIT, 0);
            break;
        case MARIO_SPAWN_SPIN_AIRBORNE:
            set_mario_action(m, ACT_SPAWN_SPIN_AIRBORNE, 0);
            break;
        case MARIO_SPAWN_FLYING:
            set_mario_action(m, ACT_FLYING, 2);
            break;
        case MARIO_SPAWN_SWIMMING:
            set_mario_action(m, ACT_WATER_IDLE, 1);
            break;
        case MARIO_SPAWN_PAINTING_STAR_COLLECT:
            set_mario_action(m, ACT_EXIT_AIRBORNE, 0);
            break;
        case MARIO_SPAWN_PAINTING_DEATH:
            set_mario_action(m, ACT_DEATH_EXIT, 0);
            break;
        case MARIO_SPAWN_AIRBORNE_STAR_COLLECT:
            set_mario_action(m, ACT_FALLING_EXIT_AIRBORNE, 0);
            break;
        case MARIO_SPAWN_AIRBORNE_DEATH:
            set_mario_action(m, ACT_UNUSED_DEATH_EXIT, 0);
            break;
        case MARIO_SPAWN_LAUNCH_STAR_COLLECT:
            set_mario_action(m, ACT_SPECIAL_EXIT_AIRBORNE, 0);
            break;
        case MARIO_SPAWN_LAUNCH_DEATH:
            set_mario_action(m, ACT_SPECIAL_DEATH_EXIT, 0);
            break;
    }

#ifdef PREVENT_DEATH_LOOP
    if (m->isDead) {
        m->health = 0x880;
        m->isDead = FALSE;
    }
#endif

    set_mario_initial_cap_powerup(m);
}

static void protect_mario_from_entry_fall(void) {
    if (gCurrDemoInput == NULL && gMarioState->pos[1] > gMarioState->floorHeight) {
        gMarioState->spawnFallProtection = MARIO_SPAWN_FALL_PROTECTION_ACTIVE;
    }
}

void init_mario_after_warp(void) {
    struct Object *object = get_destination_warp_object(sWarpDest.nodeId);
    assertf(object, "No dest warp object found for: 0x%02X", sWarpDest.nodeId);

    u32 marioSpawnType = get_mario_spawn_type(object);

    if (gMarioState->action != ACT_UNINITIALIZED) {
        gPlayerSpawnInfos[0].startPos[0] = (s16) object->oPosX;
        gPlayerSpawnInfos[0].startPos[1] = (s16) object->oPosY;
        gPlayerSpawnInfos[0].startPos[2] = (s16) object->oPosZ;

        gPlayerSpawnInfos[0].startAngle[0] = 0;
        gPlayerSpawnInfos[0].startAngle[1] = object->oMoveAngleYaw;
        gPlayerSpawnInfos[0].startAngle[2] = 0;

        struct Surface *floor;
        gMarioState->lastSafePos[0] = object->oPosX;
        gMarioState->lastSafePos[1] = find_floor(object->oPosX, object->oPosY, object->oPosZ, &floor);
        gMarioState->lastSafePos[2] = object->oPosZ;

        if (marioSpawnType == MARIO_SPAWN_DOOR_WARP) {
            init_door_warp(&gPlayerSpawnInfos[0], sWarpDest.arg);
        }

        if (sWarpDest.type == WARP_TYPE_CHANGE_LEVEL || sWarpDest.type == WARP_TYPE_CHANGE_AREA) {
            gPlayerSpawnInfos[0].areaIndex = sWarpDest.areaIdx;
            load_mario_area();
        }

        init_mario();
        if (jrb_simpleflips_room_active()) {
            jrb_simpleflips_restore_cap(gMarioState);
            save_file_mark_simpleflips_encounter();
        }
        if (sHealOnCourseEntry || jrb_simpleflips_room_active()) {
            gMarioState->health = 0x880;
            gMarioState->hurtCounter = 0;
            gMarioState->healCounter = 0;
            gMarioState->respawnHealCounter = 0;
            mario_reset_health_regeneration(gMarioState);
        }
        if (sWarpDest.type == WARP_TYPE_CHANGE_LEVEL) {
            protect_mario_from_entry_fall();
        }
        set_mario_initial_action(gMarioState, marioSpawnType, sWarpDest.arg);

        gMarioState->interactObj = object;
        gMarioState->usedObj = object;
    }

    reset_camera(gCurrentArea->camera);
    sHealOnCourseEntry = FALSE;
    sWarpDest.type = WARP_TYPE_NOT_WARPING;
    sDelayedWarpOp = WARP_OP_NONE;

    switch (marioSpawnType) {
        case MARIO_SPAWN_DOOR_WARP:
        case MARIO_SPAWN_SPIN_AIRBORNE_CIRCLE:
            play_transition(WARP_TRANSITION_FADE_FROM_CIRCLE, 0x10, 0x00, 0x00, 0x00);
            break;
        case MARIO_SPAWN_TELEPORT:
            play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 0x14, 0xFF, 0xFF, 0xFF);
            break;
        case MARIO_SPAWN_SPIN_AIRBORNE:
            play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 0x1A, 0xFF, 0xFF, 0xFF);
            break;
        case MARIO_SPAWN_FADE_FROM_BLACK:
            play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 0x10, 0x00, 0x00, 0x00);
            break;
        default:
            play_transition(WARP_TRANSITION_FADE_FROM_STAR, 0x10, 0x00, 0x00, 0x00);
            break;
    }

    if (gCurrDemoInput == NULL) {
#ifdef BETTER_REVERB
        gBetterReverbPresetValue = gCurrentArea->betterReverbPreset;
#endif
        if (castle_secret_lobby_rejection_dialog() >= 0) {
            fadeout_music(1);
        } else {
            set_background_music(gCurrentArea->musicParam, gCurrentArea->musicParam2, 0);
        }

        if (castle_secret_lobby_rejection_dialog() < 0 && (gMarioState->flags & MARIO_VANISH_CAP)) {
            play_cap_music(SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP));
        } else {
            if (castle_secret_lobby_rejection_dialog() < 0 && (gMarioState->flags & MARIO_METAL_CAP)) {
                play_cap_music(SEQUENCE_ARGS(4, SEQ_EVENT_METAL_CAP));
            }
            if (castle_secret_lobby_rejection_dialog() < 0 && (gMarioState->flags & MARIO_WING_CAP)) {
                play_cap_music(SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP));
            }
        }

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
        if (gCurrLevelNum == LEVEL_BOB
            && get_current_background_music() != SEQUENCE_ARGS(4, SEQ_LEVEL_SLIDE) && sTimerRunning) {
            play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, SEQ_LEVEL_SLIDE), 0);
        }

        if (sWarpDest.levelNum == LEVEL_CASTLE && sWarpDest.areaIdx == 1
            && (sWarpDest.nodeId == 32)
        ) {
            play_sound(SOUND_MENU_MARIO_CASTLE_WARP, gGlobalSoundSource);
        }

        if (sWarpDest.levelNum == LEVEL_CASTLE_GROUNDS && sWarpDest.areaIdx == 1
            && (sWarpDest.nodeId == 7 || sWarpDest.nodeId == 10 || sWarpDest.nodeId == 20
                || sWarpDest.nodeId == 30)) {
            play_sound(SOUND_MENU_MARIO_CASTLE_WARP, gGlobalSoundSource);
        }
#endif
#ifndef DISABLE_EXIT_COURSE
       if (sWarpDest.arg == WARP_FLAG_EXIT_COURSE) {
            play_sound(SOUND_MENU_MARIO_CASTLE_WARP, gGlobalSoundSource);
        }
#endif
    }
#ifdef PUPPYPRINT_DEBUG
    gPuppyWarp = 0;
    gLastWarpID = sWarpDest.nodeId;
    gPuppyWarpArea = 0;
#endif
}

// used for warps inside one level
void warp_area(void) {
    if (sWarpDest.type != WARP_TYPE_NOT_WARPING) {
        if (sWarpDest.type == WARP_TYPE_CHANGE_AREA) {
            level_control_timer(TIMER_CONTROL_HIDE);
            unload_mario_area();
            load_area(sWarpDest.areaIdx);
        }

        init_mario_after_warp();
    }
}

// used for warps between levels
void warp_level(void) {
    gCurrLevelNum = sWarpDest.levelNum;

    level_control_timer(TIMER_CONTROL_HIDE);

    load_area(sWarpDest.areaIdx);
    init_mario_after_warp();
}

void warp_credits(void) {
    s32 marioAction = ACT_UNINITIALIZED;

    switch (sWarpDest.nodeId) {
        case WARP_NODE_CREDITS_START:
            marioAction = ACT_END_PEACH_CUTSCENE;
            break;

        case WARP_NODE_CREDITS_NEXT:
            sEndCutsceneVp.vp.vscale[0] = 640;
            sEndCutsceneVp.vp.vscale[1] = 360;
            sEndCutsceneVp.vp.vtrans[0] = 640;
            sEndCutsceneVp.vp.vtrans[1] = 480;
            marioAction = ACT_CREDITS_CUTSCENE;
            break;

        case WARP_NODE_CREDITS_END:
            marioAction = ACT_END_WAVING_CUTSCENE;
            break;
    }

    gCurrLevelNum = sWarpDest.levelNum;

    load_area(sWarpDest.areaIdx);

    vec3s_set(gPlayerSpawnInfos[0].startPos, gCurrCreditsEntry->marioPos[0],
              gCurrCreditsEntry->marioPos[1], gCurrCreditsEntry->marioPos[2]);

    vec3s_set(gPlayerSpawnInfos[0].startAngle, 0, 0x100 * gCurrCreditsEntry->marioAngle, 0);

    gPlayerSpawnInfos[0].areaIndex = sWarpDest.areaIdx;

    load_mario_area();
    init_mario();

    set_mario_action(gMarioState, marioAction, 0);

    reset_camera(gCurrentArea->camera);

    sWarpDest.type = WARP_TYPE_NOT_WARPING;
    sDelayedWarpOp = WARP_OP_NONE;

    play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 0x14, 0x00, 0x00, 0x00);

    if (gCurrCreditsEntry == NULL || gCurrCreditsEntry == sCreditsSequence) {
#ifdef BETTER_REVERB
        gBetterReverbPresetValue = gCurrentArea->betterReverbPreset;
#endif
        if (castle_secret_lobby_rejection_dialog() >= 0) {
            fadeout_music(1);
        } else {
            set_background_music(gCurrentArea->musicParam, gCurrentArea->musicParam2, 0);
        }
    }
}

static void move_owned_object_through_instant_warp(struct Object *obj, const struct InstantWarp *warp) {
    if (obj == NULL) {
        return;
    }

    obj->header.gfx.areaIndex = warp->area;
    obj->header.gfx.activeAreaIndex = warp->area;
    vec3f_add(&obj->oPosVec, warp->displacement);
    vec3f_add(&obj->oHomeVec, warp->displacement);
    vec3f_add(obj->header.gfx.pos, warp->displacement);
    if (warp->area != gCurrAreaIndex) {
        // Source-area geometry and support history disappear during change_area.
        obj->oFloor = NULL;
        obj->oFloorHeight = FLOOR_LOWER_LIMIT_MISC;
        obj->oFloorType = SURFACE_DEFAULT;
        obj->oMoveFlags = 0;
        obj->platform = NULL;
        object_platform_motion_reset(obj);
    }
}

static void move_owned_objects_through_instant_warp(const struct InstantWarp *warp) {
    struct Object *held = gMarioState->heldObj;
    struct Object *ridden = gMarioState->riddenObj;

    move_owned_object_through_instant_warp(held, warp);
    if (held != NULL) {
        // A throw/drop can run before the destination's first hand transform renders.
        vec3f_add(gMarioState->marioBodyState->heldObjLastPosition, warp->displacement);
        if (held->behavior == segmented_to_virtual(bhvChuckya)) {
            struct ObjectNode *list = &gObjectLists[OBJ_LIST_GENACTOR];
            for (struct ObjectNode *node = list->next; node != list; node = node->next) {
                struct Object *child = (struct Object *) node;
                if (child->parentObj == held && child->activeFlags != ACTIVE_FLAG_DEACTIVATED
                    && child->behavior == segmented_to_virtual(bhvChuckyaAnchorMario)) {
                    move_owned_object_through_instant_warp(child, warp);
                }
            }
        }
    }
    if (ridden != held) {
        move_owned_object_through_instant_warp(ridden, warp);
    }
    if (ridden != NULL && warp->area != gCurrAreaIndex
        && (ridden->behavior == segmented_to_virtual(bhvKoopaShell)
            || ridden->behavior == segmented_to_virtual(bhvKoopaShellUnderwater))
        && ridden->parentObj != NULL
        && ridden->parentObj->header.gfx.activeAreaIndex == gCurrAreaIndex) {
        // A riding shell no longer needs the source box's idle-expiry ownership.
        ridden->parentObj = gMarioObject;
    }
}

void check_instant_warp(void) {
    s16 cameraAngle;
    struct Surface *floor;

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
 #ifdef UNLOCK_ALL
    if (gCurrLevelNum == LEVEL_CASTLE) {
 #else // !UNLOCK_ALL
    if (gCurrLevelNum == LEVEL_CASTLE
        && save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) >= 70) {
 #endif // !UNLOCK_ALL
        return;
    }
#endif // ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS

    if ((floor = gMarioState->floor) != NULL) {
        s32 index = floor->type - SURFACE_INSTANT_WARP_1B;
        if (index >= INSTANT_WARP_INDEX_START && index < INSTANT_WARP_INDEX_STOP
            && gCurrentArea->instantWarps != NULL) {
            struct InstantWarp *warp = &gCurrentArea->instantWarps[index];

            if (warp->id != 0) {
                player_stats_reset_movement();
                move_owned_objects_through_instant_warp(warp);
                gMarioState->pos[0] += warp->displacement[0];
                gMarioState->pos[1] += warp->displacement[1];
                gMarioState->pos[2] += warp->displacement[2];

                gMarioState->marioObj->oPosX = gMarioState->pos[0];
                gMarioState->marioObj->oPosY = gMarioState->pos[1];
                gMarioState->marioObj->oPosZ = gMarioState->pos[2];

                // Fix instant warp offset not working when warping across different areas
                gMarioObject->header.gfx.pos[0] = gMarioState->pos[0];
                gMarioObject->header.gfx.pos[1] = gMarioState->pos[1];
                gMarioObject->header.gfx.pos[2] = gMarioState->pos[2];

                cameraAngle = gMarioState->area->camera->yaw;

                change_area(warp->area);
                gMarioState->area = gCurrentArea;

                warp_camera(warp->displacement[0], warp->displacement[1], warp->displacement[2]);

                gMarioState->area->camera->yaw = cameraAngle;
            }
        }
    }
}

s16 music_unchanged_through_warp(s16 arg) {
    struct ObjectWarpNode *warpNode = area_get_warp_node(arg);
    assertf(warpNode, "No source warp node found for: 0x%02X", (u8) arg);

    s16 levelNum = warpNode->node.destLevel & 0x7F;

    s16 destArea = warpNode->node.destArea;
    s16 unchanged = TRUE;

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    s16 currBgMusic;
    if (levelNum == LEVEL_BOB && levelNum == gCurrLevelNum && destArea == gCurrAreaIndex) {
        currBgMusic = get_current_background_music();
        if (currBgMusic == SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP | SEQ_VARIATION)
            || currBgMusic == SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP)) {
            unchanged = FALSE;
        }
    } else {
#endif
        u16 destParam1 = gAreas[destArea].musicParam;
        u16 destParam2 = gAreas[destArea].musicParam2;
#ifdef BETTER_REVERB
        u16 destParam3 = gAreas[destArea].betterReverbPreset;
        unchanged = levelNum == gCurrLevelNum && destParam1 == gCurrentArea->musicParam
               && destParam2 == gCurrentArea->musicParam2 && destParam3 == gCurrentArea->betterReverbPreset;
#else
        unchanged = levelNum == gCurrLevelNum && destParam1 == gCurrentArea->musicParam
               && destParam2 == gCurrentArea->musicParam2;
#endif

        if (get_current_background_music() != destParam2) {
            unchanged = FALSE;
        }
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    }
#endif
    return unchanged;
}

/**
 * Set the current warp type and destination level/area/node.
 */
void initiate_warp(s16 destLevel, s16 destArea, s16 destWarpNode, s32 warpFlags) {
    if (jrb_simpleflips_exit_locked()) {
        return;
    }
    player_stats_warp(destLevel, (warpFlags & WARP_FLAG_EXIT_COURSE) != 0);
    game_timing_warp(destLevel, (warpFlags & WARP_FLAG_EXIT_COURSE) != 0);
    s32 refillEntry = gCurrLevelNum == LEVEL_CASTLE || gCurrLevelNum == LEVEL_CASTLE_COURTYARD
                      || gCurrLevelNum == LEVEL_CASTLE_GROUNDS;
    s32 debugEntry = FALSE;
#ifdef PUPPYPRINT_DEBUG
    debugEntry = gPuppyWarp != 0 && gPuppyWarp == destLevel;
#endif
    sHealOnCourseEntry = (refillEntry || debugEntry) && destLevel >= LEVEL_MIN && destLevel <= LEVEL_MAX
                         && (debugEntry || gLevelToCourseNumTable[destLevel - 1] != COURSE_NONE)
                         && destWarpNode < WARP_NODE_CREDITS_MIN && gCurrDemoInput == NULL
                         && gCurrCreditsEntry == NULL;
    if (destWarpNode >= WARP_NODE_CREDITS_MIN) {
        sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
    } else if (warpFlags == WARP_FLAG_EXIT_COURSE) {
        sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
#ifdef PUPPYPRINT_DEBUG
    } else if (gPuppyWarp != 0 && gPuppyWarp == destLevel) {
        // Reload even the current level so debug entrance validation always runs.
        sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
#endif
    } else if (destLevel != gCurrLevelNum) {
        sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
    } else if (destArea != gCurrentArea->index) {
        sWarpDest.type = WARP_TYPE_CHANGE_AREA;
    } else {
        sWarpDest.type = WARP_TYPE_SAME_AREA;
    }

    sWarpDest.levelNum = destLevel;
    sWarpDest.areaIdx = destArea;
    sWarpDest.nodeId = destWarpNode;
    sWarpDest.arg = warpFlags;
}

// From Surface 0xD3 to 0xFC
#define PAINTING_WARP_INDEX_START 0x00 // Value greater than or equal to Surface 0xD3
#define PAINTING_WARP_INDEX_FA 0x2A    // THI Huge Painting index left
#define PAINTING_WARP_INDEX_END 0x2D   // Value less than Surface 0xFD

/**
 * Check if Mario is above and close to a painting warp floor, and return the
 * corresponding warp node.
 */
struct WarpNode *get_painting_warp_node(void) {
    struct WarpNode *warpNode = NULL;
    s32 paintingIndex = gMarioState->floor->type - SURFACE_PAINTING_WARP_D3;

    if (castle_secret_glass_warp_ready()) {
        // Slots 0-2 belong to the rooftop Physics Lab portrait.
        return &gCurrentArea->paintingWarpNodes[3];
    }

    if (paintingIndex >= PAINTING_WARP_INDEX_START && paintingIndex < PAINTING_WARP_INDEX_END) {
        if (paintingIndex < PAINTING_WARP_INDEX_FA
            || gMarioState->pos[1] - gMarioState->floorHeight < 80.0f) {
            warpNode = &gCurrentArea->paintingWarpNodes[paintingIndex];
        }
    }

    return warpNode;
}

/**
 * Check is Mario has entered a painting, and if so, initiate a warp.
 */
void initiate_painting_warp(void) {
    if (gCurrentArea->paintingWarpNodes != NULL && gMarioState->floor != NULL) {
        struct WarpNode warpNode;
        struct WarpNode *pWarpNode = get_painting_warp_node();

        if (pWarpNode != NULL) {
            if (gMarioState->action & ACT_FLAG_INTANGIBLE) {
                play_painting_eject_sound();
            } else if (pWarpNode->id != 0) {
                warpNode = *pWarpNode;

                if (!(warpNode.destLevel & WARP_NO_CHECKPOINT)) {
                    sWarpCheckpointActive = check_warp_checkpoint(&warpNode);
                }

                initiate_warp(warpNode.destLevel & 0x7F, warpNode.destArea, warpNode.destNode, WARP_FLAGS_NONE);
                check_if_should_set_warp_checkpoint(&warpNode);

                play_transition_after_delay(WARP_TRANSITION_FADE_INTO_COLOR, 30, 255, 255, 255, 45);
                level_set_transition(74, basic_update);

                set_mario_action(gMarioState, ACT_DISAPPEARED, 0);

                gMarioState->marioObj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;

                play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
                fadeout_music(398);

                queue_rumble_data(80, 70);
                queue_rumble_decay(1);
            }
        }
    }
}

/**
 * If there is not already a delayed warp, schedule one. The source node is
 * based on the warp operation and sometimes Mario's used object.
 * Return the time left until the delayed warp is initiated.
 */
s16 level_trigger_warp(struct MarioState *m, s32 warpOp) {
    s32 fadeMusic = TRUE;

    if (jrb_simpleflips_exit_locked() && !(jrb_simpleflips_is_game_over() && warpOp == WARP_OP_DEATH)) {
        return 0;
    }
    if (sDelayedWarpOp == WARP_OP_NONE) {
        m->invincTimer = -1;
        sDelayedWarpArg = WARP_FLAGS_NONE;
        sDelayedWarpOp = warpOp;

        switch (warpOp) {
            case WARP_OP_DEMO_NEXT:
            case WARP_OP_DEMO_END:
                sDelayedWarpTimer = 20;
                sSourceWarpNodeId = WARP_NODE_DEFAULT;
                gSavedCourseNum = COURSE_NONE;
                fadeMusic = FALSE;
                play_transition(WARP_TRANSITION_FADE_INTO_STAR, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_CREDITS_END:
                sDelayedWarpTimer = 60;
                sSourceWarpNodeId = WARP_NODE_DEFAULT;
                fadeMusic = FALSE;
                gSavedCourseNum = COURSE_NONE;
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_STAR_EXIT:
                sDelayedWarpTimer = 32;
                sSourceWarpNodeId = WARP_NODE_DEFAULT;
                gSavedCourseNum = COURSE_NONE;
                play_transition(WARP_TRANSITION_FADE_INTO_MARIO, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_DEATH:
                if (m->poisonMushroomDeathPending) {
                    sDelayedWarpOp = WARP_OP_GAME_OVER;
                    m->numLives = 0;
                }
                if (jrb_simpleflips_is_game_over()) {
                    sDelayedWarpOp = WARP_OP_GAME_OVER;
                    m->numLives = 0;
                }
                if (mario_character_is_unlock_death(m)) {
                    sDelayedWarpOp = WARP_OP_GAME_OVER;
                    m->numLives = 0;
                }
#ifdef ENABLE_LIVES
                if (m->numLives == 0) {
                    sDelayedWarpOp = WARP_OP_GAME_OVER;
                }
#endif
                player_stats_death(sDelayedWarpOp == WARP_OP_GAME_OVER, FALSE);
                game_timing_end_course();
                sDelayedWarpTimer = 48;
                sSourceWarpNodeId = WARP_NODE_DEATH;
                play_transition(WARP_TRANSITION_FADE_INTO_BOWSER, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                play_sound(SOUND_MENU_BOWSER_LAUGH, gGlobalSoundSource);
#ifdef PREVENT_DEATH_LOOP
                m->isDead = TRUE;
#endif
                break;

            case WARP_OP_WARP_FLOOR:
                if ((m->floor) && (m->floor->force & 0xFF)) {
                    sSourceWarpNodeId = m->floor->force & 0xFF;
                } else {
                    sSourceWarpNodeId = WARP_NODE_WARP_FLOOR;
                    if (area_get_warp_node(sSourceWarpNodeId) == NULL) {
#ifdef ENABLE_LIVES
                        if (m->numLives == 0) {
                            sDelayedWarpOp = WARP_OP_GAME_OVER;
                        } else {
                            sSourceWarpNodeId = WARP_NODE_DEATH;
                        }
#else
                        sSourceWarpNodeId = WARP_NODE_DEATH;
#endif
                    }
                }

                if (sSourceWarpNodeId == WARP_NODE_DEATH || sDelayedWarpOp == WARP_OP_GAME_OVER) {
                    player_stats_death(sDelayedWarpOp == WARP_OP_GAME_OVER, TRUE);
                    game_timing_end_course();
                }
                sDelayedWarpTimer = 20;
                play_transition(WARP_TRANSITION_FADE_INTO_CIRCLE, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_LOOK_UP: // enter totwc
                sDelayedWarpTimer = 30;
                sSourceWarpNodeId = WARP_NODE_LOOK_UP;
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0xFF, 0xFF, 0xFF);
                play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
                break;

            case WARP_OP_SPIN_SHRINK: // bbh enter
                sDelayedWarpTimer = 30;
                sSourceWarpNodeId = GET_BPARAM2(m->usedObj->oBehParams);
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0xFF, 0xFF, 0xFF);
                break;

            case WARP_OP_TELEPORT:
                sDelayedWarpTimer = 20;
                sSourceWarpNodeId = GET_BPARAM2(m->usedObj->oBehParams);
                fadeMusic = !music_unchanged_through_warp(sSourceWarpNodeId);
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0xFF, 0xFF, 0xFF);
                break;

            case WARP_OP_WARP_DOOR:
                sDelayedWarpTimer = 20;
                sDelayedWarpArg = m->actionArg;
                sSourceWarpNodeId = GET_BPARAM2(m->usedObj->oBehParams);
                fadeMusic = !music_unchanged_through_warp(sSourceWarpNodeId);
                play_transition(WARP_TRANSITION_FADE_INTO_CIRCLE, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_WARP_OBJECT:
                sDelayedWarpTimer = 20;
                sSourceWarpNodeId = GET_BPARAM2(m->usedObj->oBehParams);
                fadeMusic = !music_unchanged_through_warp(sSourceWarpNodeId);
                play_transition(WARP_TRANSITION_FADE_INTO_STAR, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_CREDITS_START:
                sDelayedWarpTimer = 30;
                play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                break;

            case WARP_OP_CREDITS_NEXT:
                if (gCurrCreditsEntry == &sCreditsSequence[0]) {
                    sDelayedWarpTimer = 60;
                    play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                } else {
                    sDelayedWarpTimer = 20;
                    play_transition(WARP_TRANSITION_FADE_INTO_COLOR, sDelayedWarpTimer, 0x00, 0x00, 0x00);
                }
                fadeMusic = FALSE;
                break;
        }

        if (fadeMusic && gCurrDemoInput == NULL) {
            fadeout_music((3 * sDelayedWarpTimer / 2) * 8 - 2);
        }
    }

    return sDelayedWarpTimer;
}

/**
 * If a delayed warp is ready, initiate it.
 */
void initiate_delayed_warp(void) {
    struct ObjectWarpNode *warpNode;
    s32 destWarpNode;

    if (jrb_simpleflips_exit_locked()) {
#ifdef PUPPYPRINT_DEBUG
        gPuppyWarp = 0;
        gPuppyWarpArea = 0;
#endif
        if (!jrb_simpleflips_is_game_over()) {
            sDelayedWarpOp = WARP_OP_NONE;
            return;
        }
    }
#ifdef PUPPYPRINT_DEBUG
    if (gPuppyWarp) {
        initiate_warp(gPuppyWarp, gPuppyWarpArea, 0x0A, 0);
        sDelayedWarpOp = WARP_OP_NONE;
        reset_dialog_render_state();
        return;
    }
#endif

    if (sDelayedWarpOp != WARP_OP_NONE && --sDelayedWarpTimer == 0) {
        reset_dialog_render_state();

        if (gCurrDemoInput == NULL && !jrb_simpleflips_is_game_over()
            && gMarioState->yoshiCoinDeathPending && gMarioState->health < 0x100
            && (sDelayedWarpOp == WARP_OP_DEATH || sDelayedWarpOp == WARP_OP_GAME_OVER)) {
            gMarioState->yoshiCoinDeathPending = FALSE;
            gCurrCreditsEntry = NULL;
            initiate_warp(LEVEL_ENDING, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAG_YOSHI_COIN_CAKE);
            sound_banks_enable(SEQ_PLAYER_SFX, SOUND_BANKS_ALL & ~SOUND_BANKS_DISABLED_AFTER_CREDITS);
        } else if (gDebugLevelSelect && (sDelayedWarpOp & WARP_OP_TRIGGERS_LEVEL_SELECT)
                   && sDelayedWarpOp != WARP_OP_CREDITS_START && sDelayedWarpOp != WARP_OP_CREDITS_NEXT
                   && sDelayedWarpOp != WARP_OP_CREDITS_END
                   && !mario_character_is_unlock_death(gMarioState)
                   && !gMarioState->poisonMushroomDeathPending && !jrb_simpleflips_is_game_over()) {
            warp_special(WARP_SPECIAL_LEVEL_SELECT);
        } else if (gCurrDemoInput != NULL) {
            if (sDelayedWarpOp == WARP_OP_DEMO_END) {
                warp_special(WARP_SPECIAL_INTRO_SPLASH_SCREEN);
            } else {
                warp_special(WARP_SPECIAL_MARIO_HEAD_REGULAR);
            }
        } else {
            switch (sDelayedWarpOp) {
                case WARP_OP_GAME_OVER:
                    save_file_reload();
                    if (mario_character_is_unlock_death(gMarioState)) {
                        save_file_arm_he_thicc();
                        warp_special(WARP_SPECIAL_INTRO_SPLASH_SCREEN);
                    } else {
                        warp_special(WARP_SPECIAL_MARIO_HEAD_DIZZY);
                    }
                    break;

                case WARP_OP_CREDITS_END:
                    gCurrCreditsEntry = NULL;
                    initiate_warp(LEVEL_ENDING, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAGS_NONE);
                    sound_banks_enable(SEQ_PLAYER_SFX, SOUND_BANKS_ALL & ~SOUND_BANKS_DISABLED_AFTER_CREDITS);
                    break;

                case WARP_OP_DEMO_NEXT:
                    warp_special(WARP_SPECIAL_MARIO_HEAD_REGULAR);
                    break;

                case WARP_OP_CREDITS_START:
                    gCurrCreditsEntry = &sCreditsSequence[0];
                    initiate_warp(gCurrCreditsEntry->levelNum, gCurrCreditsEntry->areaIndex,
                                  WARP_NODE_CREDITS_START, WARP_FLAGS_NONE);
                    break;

                case WARP_OP_CREDITS_NEXT:
                    sound_banks_disable(SEQ_PLAYER_SFX, SOUND_BANKS_ALL);

                    gCurrCreditsEntry++;
                    gCurrActNum = gCurrCreditsEntry->actNum & 0x07;
                    if ((gCurrCreditsEntry + 1)->levelNum == LEVEL_NONE) {
                        destWarpNode = WARP_NODE_CREDITS_END;
                    } else {
                        destWarpNode = WARP_NODE_CREDITS_NEXT;
                    }

                    initiate_warp(gCurrCreditsEntry->levelNum, gCurrCreditsEntry->areaIndex, destWarpNode, WARP_FLAGS_NONE);
                    break;

                default:
                    warpNode = area_get_warp_node(sSourceWarpNodeId);
                    assertf(warpNode, "No source warp node found for: 0x%02X", (u8) sSourceWarpNodeId);

                    initiate_warp(warpNode->node.destLevel & 0x7F, warpNode->node.destArea,
                                  warpNode->node.destNode, sDelayedWarpArg);

                    check_if_should_set_warp_checkpoint(&warpNode->node);
                    if (sWarpDest.type != WARP_TYPE_CHANGE_LEVEL) {
                        level_set_transition(2, NULL);
                    }
                    break;
            }
        }
    }
}

void update_hud_values(void) {
    if (gCurrCreditsEntry == NULL) {
        s16 numHealthWedges = gMarioState->health > 0 ? gMarioState->health >> 8 : 0;

#ifdef BREATH_METER
        s16 numBreathWedges = gMarioState->breath > 0 ? gMarioState->breath >> 8 : 0;
#endif
        COND_BIT((gCurrCourseNum >= COURSE_MIN), gHudDisplay.flags, HUD_DISPLAY_FLAG_COIN_COUNT);

        if (gHudDisplay.coins < gMarioState->numCoins) {
            if (gGlobalTimer & 1) {
                u32 coinSound;
                if ((gMarioState->action & (ACT_FLAG_SWIMMING | ACT_FLAG_METAL_WATER))
                    || ((gMarioState->flags & MARIO_METAL_CAP)
                        && mario_is_head_underwater(gMarioState))) {
                    coinSound = SOUND_GENERAL_COIN_WATER;
                } else {
                    coinSound = SOUND_GENERAL_COIN;
                }

                gHudDisplay.coins++;
                play_sound(coinSound, gMarioState->marioObj->header.gfx.cameraToObject);
            }
        }

#ifdef ENABLE_LIVES
        if (gMarioState->numLives > MAX_NUM_LIVES) {
            gMarioState->numLives = MAX_NUM_LIVES;
        }
#endif

        if (gMarioState->numCoins > MAX_NUM_COINS) {
            gMarioState->numCoins = MAX_NUM_COINS;
        }

        if (gHudDisplay.coins > MAX_NUM_COINS) {
            gHudDisplay.coins = MAX_NUM_COINS;
        }

        gHudDisplay.stars = gMarioState->numStars;
        gHudDisplay.lives = gMarioState->numLives;
        gHudDisplay.keys = gMarioState->numKeys;

        if (numHealthWedges > gHudDisplay.wedges) {
            play_sound(SOUND_MENU_POWER_METER, gGlobalSoundSource);
        }
        gHudDisplay.wedges = numHealthWedges;

        COND_BIT((gMarioState->hurtCounter > 0), gHudDisplay.flags, HUD_DISPLAY_FLAG_EMPHASIZE_POWER);
#ifdef BREATH_METER
        gHudDisplay.breath = numBreathWedges;
        COND_BIT((gMarioState->breath > 0), gHudDisplay.flags, HUD_DISPLAY_FLAG_BREATH_METER);
#endif
    }
}

/**
 * Update objects, HUD, and camera. This update function excludes things like
 * endless staircase, warps, pausing, etc. This is used when entering a painting,
 * presumably to allow painting and camera updating while avoiding triggering the
 * warp twice.
 */
void basic_update(void) {
    area_update_objects();
    update_hud_values();

    if (gCurrentArea != NULL) {
        update_camera(gCurrentArea->camera);
    }
}

s32 play_mode_normal(void) {
    // Accept pause before objects sample this frame's action inputs or advance physics.
    // Pending transitions and demo-exit handling retain their existing priority.
    if (gCurrDemoInput == NULL && sCurrPlayMode == PLAY_MODE_NORMAL
        && sWarpDest.type != WARP_TYPE_CHANGE_LEVEL && sTransitionTimer == 0 && pressed_pause()) {
        lower_background_noise(1);
        cancel_rumble();
        gCameraMovementFlags |= CAM_MOVE_PAUSE_SCREEN;
        set_play_mode(PLAY_MODE_PAUSED);
        set_menu_mode(MENU_MODE_RENDER_PAUSE_SCREEN);
        return FALSE;
    }
#ifndef DISABLE_DEMO
    if (gCurrDemoInput != NULL) {
        print_intro_text();
        if (gPlayer1Controller->buttonPressed & END_DEMO) {
            level_trigger_warp(gMarioState, gCurrLevelNum == LEVEL_PSS ? WARP_OP_DEMO_END : WARP_OP_DEMO_NEXT);
        } else if (!gWarpTransition.isActive && sDelayedWarpOp == WARP_OP_NONE
                   && (gPlayer1Controller->buttonPressed & START_BUTTON)) {
            level_trigger_warp(gMarioState, WARP_OP_DEMO_NEXT);
        }
    }
#endif

    player_stats_game_update();
    warp_area();
    check_instant_warp();

#ifdef PUPPYPRINT_DEBUG
#ifdef BETTER_REVERB
    if (sPPDebugPage != PUPPYPRINT_PAGE_RAM && sPPDebugPage != PUPPYPRINT_PAGE_LEVEL_SELECT && sPPDebugPage != PUPPYPRINT_PAGE_BETTER_REVERB) {
#else
    if (sPPDebugPage != PUPPYPRINT_PAGE_RAM && sPPDebugPage != PUPPYPRINT_PAGE_LEVEL_SELECT) {
#endif
        if (sTimerRunning && gHudDisplay.timer < 17999) {
            gHudDisplay.timer++;
        }
        game_timing_runtime_update(sTransitionTimer == 0 && sWarpDest.type == WARP_TYPE_NOT_WARPING);
        area_update_objects();
    }
#else
    if (sTimerRunning && gHudDisplay.timer < 17999) {
        gHudDisplay.timer++;
    }
    game_timing_runtime_update(sTransitionTimer == 0 && sWarpDest.type == WARP_TYPE_NOT_WARPING);
    area_update_objects();
#endif
    update_hud_values();
    if (gCurrentArea != NULL) {
#ifdef PUPPYPRINT_DEBUG
#ifdef BETTER_REVERB
    if (sPPDebugPage != PUPPYPRINT_PAGE_RAM && sPPDebugPage != PUPPYPRINT_PAGE_LEVEL_SELECT && sPPDebugPage != PUPPYPRINT_PAGE_BETTER_REVERB) {
#else
    if (sPPDebugPage != PUPPYPRINT_PAGE_RAM && sPPDebugPage != PUPPYPRINT_PAGE_LEVEL_SELECT) {
#endif
            update_camera(gCurrentArea->camera);
        }
#else
        update_camera(gCurrentArea->camera);
#endif
    }

    hud_update_auto_visibility(gMarioState);

    initiate_painting_warp();
    initiate_delayed_warp();

    // If either initiate_painting_warp or initiate_delayed_warp initiated a
    // warp, change play mode accordingly.
    if (sCurrPlayMode == PLAY_MODE_NORMAL) {
        if (sWarpDest.type == WARP_TYPE_CHANGE_LEVEL) {
            set_play_mode(PLAY_MODE_CHANGE_LEVEL);
        } else if (sTransitionTimer != 0) {
            set_play_mode(PLAY_MODE_CHANGE_AREA);
        }
    }

    return FALSE;
}

static void secret_lobby_cake_fade_update(s16 *timer) {
    struct MarioBodyState *bodyState = gMarioState->marioBodyState;
    s32 alpha = MODEL_STATE_MASK * CLAMP(*timer, 0, 90) / 90;
    bodyState->modelState = (bodyState->modelState & ~MODEL_STATE_MASK) | MODEL_STATE_ALPHA | alpha;
    bodyState->capBlinkBlend = MARIO_CAP_BLINK_NONE;
}

static void secret_lobby_rejection_fade_update(s16 *timer) {
    struct MarioBodyState *body = gMarioState->marioBodyState;
    s32 alpha = MODEL_STATE_MASK * CLAMP(*timer, 0, 32) / 32;
    body->modelState = (body->modelState & ~MODEL_STATE_MASK) | MODEL_STATE_ALPHA | alpha;
    body->capBlinkBlend = MARIO_CAP_BLINK_NONE;
    if (*timer == 0) {
        player_stats_health(gMarioState->health, 0);
        gMarioState->health = 0;
        gMarioState->respawnHealCounter = 0;
        player_stats_death(TRUE, FALSE);
        game_timing_end_course();
        save_file_reload();
    }
}

void level_reject_secret_lobby(void) {
    sDelayedWarpOp = WARP_OP_NONE;
    reset_dialog_render_state();
    play_sound(SOUND_ACTION_TELEPORT, gGlobalSoundSource);
    // Bypass the debug level-select death route, and freeze movement through the fade.
    warp_special(WARP_SPECIAL_MARIO_HEAD_DIZZY);
    level_set_transition(32, secret_lobby_rejection_fade_update);
}

void level_warp_to_secret_lobby_cake(void) {
    if (!castle_lobby_finale_sun_ready()) {
        return;
    }
    gCurrCreditsEntry = NULL;
    sDispCreditsEntry = NULL;
    sDelayedWarpOp = WARP_OP_NONE;
    reset_dialog_render_state();
    initiate_warp(LEVEL_ENDING, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAG_SECRET_LOBBY_CAKE);
    play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 90, 255, 255, 255);
    // Freeze gameplay and C-Up aiming for the full fade.
    level_set_transition(90, secret_lobby_cake_fade_update);
}

void level_warp_to_castle_board_cake(void) {
    gCurrCreditsEntry = NULL;
    sDispCreditsEntry = NULL;
    sDelayedWarpOp = WARP_OP_NONE;
    reset_dialog_render_state();
    initiate_warp(LEVEL_ENDING, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAG_CASTLE_BOARD_CAKE);
    play_transition(WARP_TRANSITION_FADE_INTO_COLOR, 30, 0, 0, 0);
    level_set_transition(30, basic_update);
}

void level_warp_from_ending(void) {
    // Leave credits mode before the destination restores the saved character and gameplay state.
    gCurrCreditsEntry = NULL;
    sDispCreditsEntry = NULL;
    sDelayedWarpOp = WARP_OP_NONE;
    gSavedCourseNum = COURSE_NONE;
    gCurrActNum = 1;
    gSpecialTripleJump = FALSE;
    clear_cutscene_message();
    reset_dialog_render_state();
    sound_banks_enable(SEQ_PLAYER_SFX, SOUND_BANKS_ALL);
    if (game_options_secrets_blocked()) {
        initiate_warp(LEVEL_CASTLE_GROUNDS, 1, 0x32, WARP_FLAG_EXIT_COURSE);
    } else {
        initiate_warp(LEVEL_PHYSICS_LAB, 1, WARP_NODE_MAIN_ENTRY, WARP_FLAG_EXIT_COURSE);
    }
    fade_into_special_warp(WARP_SPECIAL_NONE, 0);
}

#ifdef REONUCAM
static void save_reonucam_speed_on_unpause(void) {
    gReonucamState.speed = game_options_get(GO_CAMERA_SPEED);
    if (gReonucamState.speed < REONUCAM_SPEED_COUNT
        && gReonucamState.speed != save_file_get_camera_speed()) {
        save_file_set_camera_speed(gReonucamState.speed);
    }
}
#endif

s32 play_mode_paused(void) {
    pause_easter_egg_update();
    update_paused_camera();
#ifdef REONUCAM
    if (gMenuOptSelectIndex != MENU_OPT_NONE) {
        save_reonucam_speed_on_unpause();
    }
#endif
    if (gMenuOptSelectIndex == MENU_OPT_NONE && gCurrDemoInput == NULL && gCurrCreditsEntry == NULL
        && !game_options_secrets_blocked()
        && mario_character_update_tas_code(&sPauseTasCodeProgress, gPlayer1Controller->buttonDown)) {
#ifdef REONUCAM
        // Commit the old file's preference before the shortcut reinitializes file state.
        save_reonucam_speed_on_unpause();
#endif
        game_options_save(game_options_active_file());
        lvl_prepare_tas_thicc_start();
        lvl_restore_cap_on_file_start(0, LEVEL_CASTLE_GROUNDS);
        lvl_init_from_save_file(0, LEVEL_CASTLE_GROUNDS);
        reset_dialog_render_state();
        set_menu_mode(MENU_MODE_NONE);
        gMenuOptSelectIndex = MENU_OPT_NONE;
        raise_background_noise(1);
        gCameraMovementFlags &= ~CAM_MOVE_PAUSE_SCREEN;
        fade_into_special_warp(WARP_SPECIAL_NONE, 0);
        return FALSE;
    }
    if (gMenuOptSelectIndex == MENU_OPT_NONE) {
        set_menu_mode(MENU_MODE_RENDER_PAUSE_SCREEN);
    } else if (gMenuOptSelectIndex == MENU_OPT_DEFAULT) {
        raise_background_noise(1);
        gCameraMovementFlags &= ~CAM_MOVE_PAUSE_SCREEN;
        hud_reveal_after_pause();
        set_play_mode(PLAY_MODE_NORMAL);
#ifndef DISABLE_EXIT_COURSE
    } else { // MENU_OPT_EXIT_COURSE
        if (level_is_ending()) {
            level_warp_from_ending();
        } else if (gDebugLevelSelect && gCurrLevelNum != LEVEL_PHYSICS_LAB) {
            player_stats_warp(gCurrLevelNum, TRUE);
            game_timing_end_course();
            fade_into_special_warp(WARP_SPECIAL_LEVEL_SELECT, 1);
        } else {
            if (gCurrLevelNum == LEVEL_PHYSICS_LAB) {
                // Exit to the castle entrance even with death-on-exit enabled.
                initiate_warp(LEVEL_CASTLE_GROUNDS, 1, 0x32, WARP_FLAG_EXIT_COURSE);
                gSavedCourseNum = COURSE_NONE;
            } else {
#ifdef DEATH_ON_EXIT_COURSE
                struct ObjectWarpNode *warpNode = area_get_warp_node(WARP_NODE_DEATH);
                assertf(warpNode != NULL, "No death warp node could be found!");

                initiate_warp(warpNode->node.destLevel & 0x7F, warpNode->node.destArea,
                              warpNode->node.destNode, WARP_FLAGS_NONE);
#else // DEATH_ON_EXIT_COURSE
                initiate_warp(EXIT_COURSE_LEVEL, EXIT_COURSE_AREA, EXIT_COURSE_NODE,
                              WARP_FLAG_EXIT_COURSE);
                gSavedCourseNum = COURSE_NONE;
#endif // DEATH_ON_EXIT_COURSE
            }

            fade_into_special_warp(WARP_SPECIAL_NONE, 0);
            if (sWarpDest.type != WARP_TYPE_CHANGE_LEVEL) {
                set_play_mode(PLAY_MODE_CHANGE_AREA);
            }
        }

        gCameraMovementFlags &= ~CAM_MOVE_PAUSE_SCREEN;
#endif // DISABLE_EXIT_COURSE
    }

    return FALSE;
}

/**
 * Debug mode that lets you frame advance by pressing D-pad down. Unfortunately
 * it uses the pause camera, making it basically unusable in most levels.
 */
s32 play_mode_frame_advance(void) {
    if (gPlayer1Controller->buttonPressed & D_JPAD) {
        gCameraMovementFlags &= ~CAM_MOVE_PAUSE_SCREEN;
        play_mode_normal();
    } else if (gPlayer1Controller->buttonPressed & START_BUTTON) {
        gCameraMovementFlags &= ~CAM_MOVE_PAUSE_SCREEN;
        raise_background_noise(1);
        set_play_mode(PLAY_MODE_NORMAL);
    } else {
        gCameraMovementFlags |= CAM_MOVE_PAUSE_SCREEN;
    }

    return FALSE;
}

/**
 * Set the transition, which is a period of time after the warp is initiated
 * but before it actually occurs. If updateFunction is not NULL, it will be
 * called each frame during the transition.
 */
void level_set_transition(s16 length, void (*updateFunction)()) {
    sTransitionTimer = length;
    sTransitionUpdate = (typeof(sTransitionUpdate)) updateFunction;
}

/**
 * Play the transition and then return to normal play mode.
 */
s32 play_mode_change_area(void) {
    // Course-entry dialogue freezes world updates but still counts as playtime.
    if (sTransitionTimer < 0 && get_dialog_id() != DIALOG_NONE) {
        player_stats_game_update();
    }

    // sm64ex-alo
    // Change function to have similar change_level defines
    if (sTransitionUpdate != NULL) {
        sTransitionUpdate(&sTransitionTimer);
    }

    if (--sTransitionTimer == -1) {
        update_camera(gCurrentArea->camera);
        sTransitionTimer = 0;
        sTransitionUpdate = NULL;
        set_play_mode(PLAY_MODE_NORMAL);
    }

    return FALSE;
}

/**
 * Play the transition and then return to normal play mode.
 */
s32 play_mode_change_level(void) {
    if (sTransitionUpdate != NULL) {
        sTransitionUpdate(&sTransitionTimer);
    }

    if (--sTransitionTimer == -1) {
        gHudDisplay.flags = HUD_DISPLAY_NONE;
        sTransitionTimer = 0;
        sTransitionUpdate = NULL;

        if (sWarpDest.type != WARP_TYPE_NOT_WARPING) {
            return sWarpDest.levelNum;
        } else {
            return sSpecialWarpDest;
        }
    }

    return FALSE;
}

/**
 * Unused play mode. Doesn't call transition update and doesn't reset transition at the end.
 */
UNUSED static s32 play_mode_unused(void) {
    if (--sTransitionTimer == -1) {
        gHudDisplay.flags = HUD_DISPLAY_NONE;

        if (sWarpDest.type != WARP_TYPE_NOT_WARPING) {
            return sWarpDest.levelNum;
        } else {
            return sSpecialWarpDest;
        }
    }

    return FALSE;
}

s32 update_level(void) {
    s32 changeLevel = FALSE;

    switch (sCurrPlayMode) {
        case PLAY_MODE_NORMAL:
            changeLevel = play_mode_normal();
            break;
        case PLAY_MODE_PAUSED:
            changeLevel = play_mode_paused();
            break;
        case PLAY_MODE_CHANGE_AREA:
            changeLevel = play_mode_change_area();
            break;
        case PLAY_MODE_CHANGE_LEVEL:
            changeLevel = play_mode_change_level();
            break;
        case PLAY_MODE_FRAME_ADVANCE:
            changeLevel = play_mode_frame_advance();
            break;
    }

    if (changeLevel) {
        reset_volume();
        enable_background_sound();
    }

    return changeLevel;
}

#ifdef PUPPYPRINT_DEBUG
extern u32 gInitLevelTime;
#endif

#ifdef PUPPYPRINT_DEBUG
static s32 debug_warp_entry_node(s32 areaIndex) {
    if (areaIndex <= 0 || areaIndex >= AREA_COUNT || gAreas[areaIndex].graphNode == NULL
        || gAreas[areaIndex].camera == NULL) {
        return -1;
    }

    s32 fallbackNode = -1;
    for (struct SpawnInfo *spawn = gAreas[areaIndex].objectSpawnInfos; spawn != NULL;
         spawn = spawn->next) {
        u32 type = get_mario_spawn_type_from_behavior(spawn->behaviorScript);
        if (type == MARIO_SPAWN_NONE || type == MARIO_SPAWN_DEATH || type == MARIO_SPAWN_PAINTING_DEATH
            || type == MARIO_SPAWN_AIRBORNE_DEATH || type == MARIO_SPAWN_LAUNCH_DEATH
            || type == MARIO_SPAWN_HARD_AIR_KNOCKBACK) {
            continue;
        }
        s32 id = (spawn->behaviorArg >> 16) & 0xFF;
        for (struct ObjectWarpNode *node = gAreas[areaIndex].warpNodes; node != NULL;
             node = node->next) {
            if (node->node.id == id) {
                if (id == WARP_NODE_MAIN_ENTRY) {
                    return id;
                }
                if (fallbackNode < 0) {
                    fallbackNode = id;
                }
                break;
            }
        }
    }
    return fallbackNode;
}

// Resolve against the destination script's active objects, after act filtering and before loading.
static void resolve_debug_warp_destination(void) {
    s32 area = sWarpDest.areaIdx;
    s32 node = debug_warp_entry_node(area);
    if (node < 0) {
        area = gPlayerSpawnInfos[0].areaIndex;
        node = debug_warp_entry_node(area);
    }
    if (node < 0) {
        for (area = 1; area < AREA_COUNT; area++) {
            node = debug_warp_entry_node(area);
            if (node >= 0) {
                break;
            }
        }
    }
    if (node >= 0) {
        sWarpDest.areaIdx = area;
        sWarpDest.nodeId = node;
    } else {
        // A playable script without warp objects can still use its authored MARIO_POS.
        sWarpDest.type = WARP_TYPE_NOT_WARPING;
        sHealOnCourseEntry = FALSE;
        gPuppyWarp = 0;
        gPuppyWarpArea = 0;
    }
}
#endif

s32 init_level(void) {
    s32 fadeFromColor = FALSE;

    // Apply pending rules before loading Mario and initializing arena objects.
    game_options_runtime_restart();
    game_timing_end_course();

    jrb_simpleflips_reset();

    set_play_mode(PLAY_MODE_NORMAL);

    sDelayedWarpOp = WARP_OP_NONE;
    sTransitionTimer = 0;
    sSpecialWarpDest = WARP_SPECIAL_NONE;

    g100CoinStarSpawned = FALSE;

    // NOTE: gStarModelLastCollected reset here as a safety to prevent possible UB if assigned a model used
    // in a non-global group. This checked can be removed as needed.
    if (gStarModelLastCollected != MODEL_BOWSER_KEY
#ifdef STAR_DANCE_USES_STARS_MODEL
         || gStarModelLastCollected != MODEL_TRANSPARENT_STAR
#endif
    ) {
        gStarModelLastCollected = MODEL_STAR;
    }

    if (gCurrCreditsEntry == NULL) {
        gHudDisplay.flags = HUD_DISPLAY_DEFAULT;
    } else {
        gHudDisplay.flags = HUD_DISPLAY_NONE;
    }
    hud_reset_auto_visibility();

    sTimerRunning = FALSE;

#ifdef PUPPYPRINT_DEBUG
    if (gPuppyWarp != 0 && sWarpDest.type != WARP_TYPE_NOT_WARPING) {
        resolve_debug_warp_destination();
    }
#endif

    if (sWarpDest.type != WARP_TYPE_NOT_WARPING) {
        if (sWarpDest.nodeId >= WARP_NODE_CREDITS_MIN) {
            warp_credits();
        } else {
            warp_level();
        }
    } else {
        if (gPlayerSpawnInfos[0].areaIndex >= 0) {
            load_mario_area();
            init_mario();
            protect_mario_from_entry_fall();
        }

        if (gCurrentArea != NULL) {
            reset_camera(gCurrentArea->camera);

            if (gCurrDemoInput != NULL) {
                set_mario_action(gMarioState, ACT_IDLE, 0);
            } else if (gMarioState->action != ACT_UNINITIALIZED) {
                if (gCurrLevelNum == LEVEL_CASTLE_GROUNDS
                    && save_file_he_thicc_intro_pending(gCurrSaveFileNum - 1)) {
                    set_mario_action(gMarioState, ACT_INTRO_CUTSCENE, 0);
                    fadeFromColor = TRUE;
                } else if (!gDebugLevelSelect) {
#ifndef PEACH_SKIP
                    if (!save_file_exists(gCurrSaveFileNum - 1)) {
                        set_mario_action(gMarioState, ACT_INTRO_CUTSCENE, 0);
                        fadeFromColor = TRUE;
                    } else
#endif
                    {
                        set_mario_action(gMarioState, ACT_IDLE, 0);
                    }
                }
            }
        }
        if (fadeFromColor) {
            play_transition(WARP_TRANSITION_FADE_FROM_COLOR, 0x5A, 0xFF, 0xFF, 0xFF);
        } else {
            play_transition(WARP_TRANSITION_FADE_FROM_STAR, 0x10, 0xFF, 0xFF, 0xFF);
        }

        if (gCurrDemoInput == NULL) {
#ifdef BETTER_REVERB
            gBetterReverbPresetValue = gCurrentArea->betterReverbPreset;
#endif
            if (castle_secret_lobby_rejection_dialog() >= 0) {
                fadeout_level_music(1);
                seq_player_fade_out(SEQ_PLAYER_ENV, 1);
            } else {
                set_background_music(gCurrentArea->musicParam, gCurrentArea->musicParam2, 0);
            }
        }
    }
    if (gCurrDemoInput == NULL) {
        cancel_rumble();
    }

    if (gMarioState->action == ACT_INTRO_CUTSCENE) {
        sound_banks_disable(SEQ_PLAYER_SFX, SOUND_BANKS_DISABLED_DURING_INTRO_CUTSCENE);
    }

#ifdef PUPPYPRINT_DEBUG
    if (gInitLevelTime) {
        u32 totalTime = osGetCount() - gInitLevelTime;
        append_puppyprint_log("Level loaded in %2.3fs.", (f64) OS_CYCLES_TO_USEC(totalTime) / 1000000.0f);
        gInitLevelTime = 0;
    }
#endif

    return TRUE;
}

/**
 * Initialize the current level if initOrUpdate is 0, or update the level if it is 1.
 */
s32 lvl_init_or_update(s16 initOrUpdate, UNUSED s32 unused) {
    return (initOrUpdate ? update_level() : init_level());
}

#if MULTILANG
void load_language_text(void) {
    switch (gInGameLanguage - 1) {
        case LANGUAGE_ENGLISH:
            load_segment_decompress(SEGMENT_EU_TRANSLATION, _translation_en_yay0SegmentRomStart, _translation_en_yay0SegmentRomEnd);
            break;
        case LANGUAGE_FRENCH:
            load_segment_decompress(SEGMENT_EU_TRANSLATION, _translation_fr_yay0SegmentRomStart, _translation_fr_yay0SegmentRomEnd);
            break;
        case LANGUAGE_GERMAN:
            load_segment_decompress(SEGMENT_EU_TRANSLATION, _translation_de_yay0SegmentRomStart, _translation_de_yay0SegmentRomEnd);
            break;
    }
}
#endif

void lvl_prepare_tas_thicc_start(void) {
    if (game_options_secrets_blocked()) {
        return;
    }
    // Finish records and the old visit while their file, level and area still own them.
    player_stats_warp(LEVEL_CASTLE_GROUNDS, TRUE);
    game_timing_end_course();
    save_file_unlock_tas_thicc();
    gCurrSaveFileNum = SAVE_FILE_C + 1;
    sTasThiccStartPending = TRUE;
}

s32 lvl_init_from_save_file(UNUSED s16 initOrUpdate, s32 levelNum) {
#if MULTILANG
    gInGameLanguage = eu_get_language()+1;
    load_language_text();
#endif
    sWarpDest.type = WARP_TYPE_NOT_WARPING;
    sHealOnCourseEntry = FALSE;
    sDelayedWarpOp = WARP_OP_NONE;
    game_options_select_file(gCurrDemoInput == NULL ? gCurrSaveFileNum - 1 : -1);
    game_timing_select_file(gCurrDemoInput == NULL ? gCurrSaveFileNum - 1 : -1);
    game_options_runtime_update();
    if (gCurrDemoInput == NULL) {
#ifdef REONUCAM
        if (!game_options_has_record(gCurrSaveFileNum - 1)) {
            game_options_set(GO_CAMERA_SPEED, save_file_get_camera_speed());
        }
        gReonucamState.speed = game_options_get(GO_CAMERA_SPEED);
#endif
        simpleflips_scream_load_file(gCurrSaveFileNum);
        save_file_start_he_thicc(gCurrSaveFileNum - 1);
        mario_character_load_file(gCurrSaveFileNum - 1);
        if (save_file_he_thicc_intro_pending(gCurrSaveFileNum - 1)) {
            levelNum = LEVEL_CASTLE_GROUNDS;
            gDebugLevelSelect = FALSE;
        } else if (!gDebugLevelSelect && !game_options_secrets_blocked()
                   && save_file_has_completed_game(gCurrSaveFileNum - 1)) {
            levelNum = LEVEL_PHYSICS_LAB;
            gCurrActNum = 1;
        }
    }
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
    gNeverEnteredCastle = !save_file_exists(gCurrSaveFileNum - 1)
                          || save_file_he_thicc_intro_pending(gCurrSaveFileNum - 1);
#else
    gNeverEnteredCastle = 0;
#endif
    gCurrLevelNum = levelNum;
    gCurrCourseNum = COURSE_NONE;
    gSavedCourseNum = COURSE_NONE;
    gCurrCreditsEntry = NULL;
    gSpecialTripleJump = FALSE;

    init_mario_from_save_file();
    disable_warp_checkpoint();
    select_mario_cam_mode();
    set_yoshi_as_not_dead();

    if (sTasThiccStartPending) {
        sTasThiccStartPending = FALSE;
        gCurrLevelNum = LEVEL_CASTLE_GROUNDS;
        levelNum = gCurrLevelNum;
        gDebugLevelSelect = FALSE;
        // Force a fresh load even when the code was entered in the grounds pause menu.
        initiate_warp(LEVEL_CASTLE_GROUNDS, 1, 0x32, WARP_FLAG_EXIT_COURSE);
    }

    return levelNum;
}

// Debug shortcuts must not hide an earned or already active File C.
s32 lvl_should_skip_file_select(UNUSED s16 arg, UNUSED s32 unused) {
    return !save_file_he_thicc_pending() && !save_file_is_he_thicc(SAVE_FILE_C);
}

s32 lvl_restore_cap_on_file_start(UNUSED s16 arg, s32 levelNum) {
    u32 lostCapFlags = SAVE_FLAG_CAP_ON_GROUND | SAVE_FLAG_CAP_ON_KLEPTO | SAVE_FLAG_CAP_ON_UKIKI
                       | SAVE_FLAG_CAP_ON_MR_BLIZZARD;

    if (save_file_get_flags() & lostCapFlags) {
        save_file_clear_flags(lostCapFlags);
    }
    return levelNum;
}

s32 lvl_set_current_level(UNUSED s16 initOrUpdate, s32 levelNum) {
    s32 warpCheckpointActive = sWarpCheckpointActive;

    sWarpCheckpointActive = FALSE;
    gCurrLevelNum = levelNum;
    gCurrCourseNum = gLevelToCourseNumTable[levelNum - 1];

    if (gCurrDemoInput != NULL || gCurrCreditsEntry != NULL || gCurrCourseNum == COURSE_NONE) {
        return FALSE;
    }

    if (gCurrLevelNum != LEVEL_BOWSER_1 && gCurrLevelNum != LEVEL_BOWSER_2 && gCurrLevelNum != LEVEL_BOWSER_3) {
        gMarioState->numCoins = 0;
        gHudDisplay.coins = 0;
        gCurrCourseStarFlags =
            save_file_get_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(gCurrCourseNum));
    }

    if (gSavedCourseNum != gCurrCourseNum) {
        gSavedCourseNum = gCurrCourseNum;
        disable_warp_checkpoint();
    }

    if (gCurrCourseNum > COURSE_STAGES_MAX || warpCheckpointActive) {
        return FALSE;
    }

#if defined(TEST_LEVEL) && defined(TEST_ACT)
    if (levelNum == TEST_LEVEL && !gDebugLevelSelect) {
        gCurrActNum = TEST_ACT;
        return FALSE;
    }
#endif

    return !gDebugLevelSelect;
}

/**
 * Play the "thank you so much for to playing my game" sound.
 */
static s32 sYoshiCoinCakeActive;
static s32 sCastleBoardCakeActive;
static s32 sSecretLobbyCakeActive;

s32 lvl_play_the_end_screen_sound(UNUSED s16 initOrUpdate, UNUSED s32 levelNum) {
    if (sYoshiCoinCakeActive) {
        yoshi_cake_pcm_start();
    } else {
        play_sound(SOUND_MENU_THANK_YOU_PLAYING_MY_GAME, gGlobalSoundSource);
    }
    return TRUE;
}

#define END_SCREEN_IDLE_FRAMES (10 * 30)

// The cake has no Mario or camera: service menus and transitions without gameplay updates.
s32 lvl_end_screen(s16 initOrUpdate, UNUSED s32 levelNum) {
    static s32 sEndScreenTimer;
    s32 changeLevel;
    if (!initOrUpdate) {
        sYoshiCoinCakeActive = (sWarpDest.arg & WARP_FLAG_YOSHI_COIN_CAKE) != 0;
        sCastleBoardCakeActive = (sWarpDest.arg & WARP_FLAG_CASTLE_BOARD_CAKE) != 0;
        sSecretLobbyCakeActive = (sWarpDest.arg & WARP_FLAG_SECRET_LOBBY_CAKE) != 0;
        sWarpDest.arg = WARP_FLAGS_NONE;
        if (sYoshiCoinCakeActive || sCastleBoardCakeActive) {
            fadeout_music(1);
        }
        sEndScreenTimer = 0;
        sWarpDest.type = WARP_TYPE_NOT_WARPING;
        sHealOnCourseEntry = FALSE;
        sDelayedWarpOp = WARP_OP_NONE;
        gCurrCreditsEntry = NULL;
        sDispCreditsEntry = NULL;
        gHudDisplay.flags = HUD_DISPLAY_NONE;
        clear_cutscene_message();
        set_play_mode(PLAY_MODE_NORMAL);
        reset_dialog_render_state();
        set_menu_mode(MENU_MODE_NONE);
        gMenuOptSelectIndex = MENU_OPT_NONE;
        // A direct boot into cake has no Mario; permit normal Mario initialization on departure.
        gMarioState->action = ACT_IDLE;
        gMarioState->yoshiCoinDeathPending = FALSE;
#ifdef PUPPYPRINT_DEBUG
        // Consume the request that brought us here before accepting the next selection.
        gPuppyWarp = 0;
        gPuppyWarpArea = 0;
#endif
    }
#ifdef PUPPYPRINT_DEBUG
    else if (gPuppyWarp) {
        sCastleBoardCakeActive = FALSE;
        if (sYoshiCoinCakeActive) {
            pause_pcm_stop();
            sYoshiCoinCakeActive = FALSE;
        }
        if (sSecretLobbyCakeActive) {
            initiate_warp(LEVEL_CASTLE_GROUNDS, 1, 0x0A, WARP_FLAGS_NONE);
            sSecretLobbyCakeActive = FALSE;
        } else {
            initiate_warp(gPuppyWarp, gPuppyWarpArea, 0x0A, WARP_FLAGS_NONE);
        }
        // Selecting cake again also reloads its script; it has no in-area Mario warp.
        sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
        reset_volume();
        enable_background_sound();
        return sWarpDest.levelNum;
    }
#endif
    if (initOrUpdate) {
        switch (sCurrPlayMode) {
            case PLAY_MODE_NORMAL:
                if (sSecretLobbyCakeActive) {
                    return FALSE; // Indefinite finale: no pause and no idle timer overflow.
                }
                if (!sYoshiCoinCakeActive && !sCastleBoardCakeActive && pressed_pause()) {
                    lower_background_noise(1);
                    cancel_rumble();
                    gCameraMovementFlags |= CAM_MOVE_PAUSE_SCREEN;
                    set_play_mode(PLAY_MODE_PAUSED);
                    set_menu_mode(MENU_MODE_RENDER_PAUSE_SCREEN);
                } else if (++sEndScreenTimer >= END_SCREEN_IDLE_FRAMES
                           && (!sYoshiCoinCakeActive || !yoshi_cake_pcm_is_playing())) {
                    if (sYoshiCoinCakeActive || sCastleBoardCakeActive) {
                        sYoshiCoinCakeActive = FALSE;
                        sCastleBoardCakeActive = FALSE;
                        save_file_reload();
                        fade_into_special_warp(WARP_SPECIAL_MARIO_HEAD_DIZZY, 0);
                    } else {
                        level_warp_from_ending();
                    }
                }
                break;
            case PLAY_MODE_PAUSED:
                if (sSecretLobbyCakeActive) {
                    set_menu_mode(MENU_MODE_NONE);
                    set_play_mode(PLAY_MODE_NORMAL);
                    return FALSE;
                }
                play_mode_paused();
                break;
            case PLAY_MODE_CHANGE_LEVEL:
                changeLevel = play_mode_change_level();
                if (changeLevel) {
                    if (sSecretLobbyCakeActive) {
                        initiate_warp(LEVEL_CASTLE_GROUNDS, 1, 0x0A, WARP_FLAGS_NONE);
                        sWarpDest.type = WARP_TYPE_CHANGE_LEVEL;
                        sSecretLobbyCakeActive = FALSE;
                        changeLevel = LEVEL_CASTLE_GROUNDS;
                    }
                    reset_volume();
                    enable_background_sound();
                }
                return changeLevel;
        }
    }
    return FALSE;
}
