#include <ultra64.h>

#include "area.h"
#include "audio/external.h"
#include "audio/effects.h"
#include "audio/load.h"
#include "audio/synthesis.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "game_init.h"
#include "game_options.h"
#include "game_options_audio.h"
#include "level_table.h"
#include "level_update.h"
#include "main.h"
#include "paintings.h"
#include "print.h"
#include "save_file.h"
#include "seq_ids.h"
#include "sm64.h"
#include "sound_init.h"
#include "rumble_init.h"
#include "puppyprint.h"
#include "profiling.h"

#include "config/config_audio.h"

#define MUSIC_NONE 0xFFFF

static OSMesgQueue sSoundMesgQueue;
static OSMesg sSoundMesgBuf[AUDIO_IMMEDIATE_TASKS ? 4 : 1];
static struct VblankHandler sSoundVblankHandler;

// Game-thread-owned reason bits: 1 for pause, 2 for sleeping/first-person attenuation.
// Pause uses an independent linear gain read by the sound thread; native fades keep running.
static u8 sBackgroundNoiseReasons = 0;
static u32 sPauseMusicFadeStartFrame;
static f32 sPauseMusicFadeStartScale = 1.0f;
static f32 sPauseMusicFadeTargetScale = 1.0f;
static s32 sPauseMusicFading;

static void update_pause_music_volume(void) {
    u32 elapsed;
    f32 progress;

    if (!sPauseMusicFading) {
        return;
    }
    elapsed = (u32) (gGlobalTimer - sPauseMusicFadeStartFrame);
    if (elapsed >= PAUSE_BLEND_FRAMES) {
        gPauseMusicVolumeScale = sPauseMusicFadeTargetScale;
        sPauseMusicFading = FALSE;
        return;
    }
    // Include the entry-frame step; the final target follows the intermediate screen blends.
    progress = (f32) (elapsed + 1) / (PAUSE_BLEND_FRAMES + 1);
    gPauseMusicVolumeScale =
        sPauseMusicFadeStartScale + (sPauseMusicFadeTargetScale - sPauseMusicFadeStartScale) * progress;
}

static void fade_pause_music_volume(f32 targetScale) {
    sPauseMusicFadeStartFrame = gGlobalTimer;
    // Reversals and Options changes start from the last applied gain.
    sPauseMusicFadeStartScale = gPauseMusicVolumeScale;
    sPauseMusicFadeTargetScale = targetScale;
    sPauseMusicFading = TRUE;
    update_pause_music_volume();
}

static u8 sBgMusicDisabled = FALSE;
static u16 sCurrentMusic = MUSIC_NONE;
static u16 sCurrentShellMusic = MUSIC_NONE;
static u16 sCurrentCapMusic = MUSIC_NONE;
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
static u8 sPlayingInfiniteStairs = FALSE;
#endif
// Only the 20th array element is used.
static u32 sMenuSoundsExtra[] = {
    SOUND_MOVING_TERRAIN_SLIDE + (0 << 16),
    SOUND_MOVING_TERRAIN_SLIDE + (1 << 16),
    SOUND_MOVING_TERRAIN_SLIDE + (2 << 16),
    SOUND_MOVING_TERRAIN_SLIDE + (3 << 16),
    SOUND_MOVING_TERRAIN_SLIDE + (4 << 16),
    SOUND_MOVING_TERRAIN_SLIDE + (5 << 16),
    SOUND_MOVING_TERRAIN_SLIDE + (6 << 16),
    SOUND_MOVING_LAVA_BURN,
    SOUND_MOVING_SLIDE_DOWN_TREE,
    SOUND_MOVING_SLIDE_DOWN_POLE,
    SOUND_MOVING_QUICKSAND_DEATH,
    SOUND_MOVING_TERRAIN_RIDING_SHELL,
    NO_SOUND,
    SOUND_ENV_BOAT_ROCKING1,
    SOUND_ENV_ELEVATOR3,
    SOUND_ENV_BOWLING_BALL_ROLL,
    SOUND_ENV_WATERFALL1,
    SOUND_ENV_WATERFALL2,
    SOUND_ENV_ELEVATOR1,
    SOUND_ENV_DRONING1,
    SOUND_ENV_DRONING2,
    SOUND_ENV_ELEVATOR2,
    SOUND_ENV_WIND1,
    SOUND_ENV_WATER,
    SOUND_AIR_BOWSER_SPIT_FIRE,
    SOUND_MOVING_AIM_CANNON,
    SOUND_AIR_LAKITU_FLY,
    SOUND_AIR_AMP_BUZZ,
    SOUND_AIR_CASTLE_OUTDOORS_AMBIENT,
    SOUND_OBJ2_BIRD_CHIRP1,
    SOUND_GENERAL2_BIRD_CHIRP2,
    SOUND_OBJ_BIRD_CHIRP3,
    SOUND_ENV_ELEVATOR2,
    SOUND_AIR_BLOW_WIND,
    SOUND_AIR_BLOW_FIRE,
    SOUND_ENV_ELEVATOR4,
};
static s8 sPaintingEjectSoundPlayed = FALSE;

void play_menu_sounds_extra(s32 a, void *b);

/**
 * Called from threads: thread5_game_loop
 */
void reset_volume(void) {
    sBackgroundNoiseReasons = 0;
    sPauseMusicFading = FALSE;
    gPauseMusicVolumeScale = 1.0f;
    set_pause_sfx_muted(FALSE);
}

/**
 * Called from threads: thread5_game_loop
 */
void lower_background_noise(s32 reason) {
    switch (reason) {
        case 1:
            set_pause_sfx_muted(TRUE);
            fade_pause_music_volume(game_options_pause_gain());
            break;
        case 2:
            seq_player_lower_volume(SEQ_PLAYER_LEVEL, 60, 40);
            break;
    }
    sBackgroundNoiseReasons |= reason;
}

/**
 * Called from threads: thread5_game_loop
 */
void raise_background_noise(s32 reason) {
    switch (reason) {
        case 1:
            set_pause_sfx_muted(FALSE);
            fade_pause_music_volume(1.0f);
            break;
        case 2:
            seq_player_unlower_volume(SEQ_PLAYER_LEVEL, 60);
            break;
    }
    sBackgroundNoiseReasons &= ~reason;
}

// This legacy predicate checks sequence-player muting, not the Pause Music option gain.
s32 pause_music_is_disabled(void) {
#ifdef MUTE_MUSIC_PLAYERS
    return (sBackgroundNoiseReasons & 1) != 0;
#else
    return (sBackgroundNoiseReasons & 1) != 0 && gMuteMusicPlayers;
#endif
}

/**
 * Called from threads: thread5_game_loop
 */
void disable_background_sound(void) {
    if (sBgMusicDisabled == FALSE) {
        sBgMusicDisabled = TRUE;
        sound_banks_disable(SEQ_PLAYER_SFX, SOUND_BANKS_BACKGROUND);
    }
}

/**
 * Called from threads: thread5_game_loop
 */
void enable_background_sound(void) {
    if (sBgMusicDisabled == TRUE) {
        sBgMusicDisabled = FALSE;
        sound_banks_enable(SEQ_PLAYER_SFX, SOUND_BANKS_BACKGROUND);
    }
}

/**
 * Sets the sound mode
 *
 * Called from threads: thread5_game_loop
 */
void set_sound_mode(u16 soundMode) {
    if (soundMode < SOUND_MODE_COUNT) {
        gSoundMode = soundMode;
    }
}

/**
 * Wrapper method by menu used to set the sound via flags.
 *
 * Called from threads: thread5_game_loop
 */
void play_menu_sounds(s16 soundMenuFlags) {
    if (soundMenuFlags & SOUND_MENU_FLAG_HANDAPPEAR) {
        play_sound(SOUND_MENU_HAND_APPEAR, gGlobalSoundSource);
    } else if (soundMenuFlags & SOUND_MENU_FLAG_HANDISAPPEAR) {
        play_sound(SOUND_MENU_HAND_DISAPPEAR, gGlobalSoundSource);
    } else if (soundMenuFlags & SOUND_MENU_FLAG_UNKNOWN1) {
        play_sound(SOUND_MENU_UNK0C_FLAG_UNKNOWN1, gGlobalSoundSource);
    } else if (soundMenuFlags & SOUND_MENU_FLAG_PINCHMARIOFACE) {
        play_sound(SOUND_MENU_PINCH_MARIO_FACE, gGlobalSoundSource);
    } else if (soundMenuFlags & SOUND_MENU_FLAG_PINCHMARIOFACE2) {
        play_sound(SOUND_MENU_PINCH_MARIO_FACE, gGlobalSoundSource);
    } else if (soundMenuFlags & SOUND_MENU_FLAG_LETGOMARIOFACE) {
        play_sound(SOUND_MENU_LET_GO_MARIO_FACE, gGlobalSoundSource);
    } else if (soundMenuFlags & SOUND_MENU_FLAG_CAMERAZOOMIN) {
        play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
    } else if (soundMenuFlags & SOUND_MENU_FLAG_CAMERAZOOMOUT) {
        play_sound(SOUND_MENU_CAMERA_ZOOM_OUT, gGlobalSoundSource);
    }

    if (soundMenuFlags & SOUND_MENU_FLAG_EXTRA) {
        play_menu_sounds_extra(20, NULL);
    }
    if (soundMenuFlags & SOUND_MENU_FLAG_LETGOMARIOFACE) {
        queue_rumble_data(10, 60);
    }
}

/**
 * Plays the painting eject sound effect if it has not already been played
 *
 * Called from threads: thread5_game_loop
 */
void play_painting_eject_sound(void) {
    if (gRipplingPainting != NULL && gRipplingPainting->state == PAINTING_ENTERED) {
        // ripple when Mario enters painting
        if (!sPaintingEjectSoundPlayed) {
            play_sound(SOUND_GENERAL_PAINTING_EJECT,
                       gMarioStates[0].marioObj->header.gfx.cameraToObject);
        }
        sPaintingEjectSoundPlayed = TRUE;
    } else {
        sPaintingEjectSoundPlayed = FALSE;
    }
}

/**
 * Called from threads: thread5_game_loop
 */
void play_infinite_stairs_music(void) {
#if defined(ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS) && !defined(UNLOCK_ALL)
    u8 shouldPlay = FALSE;

    /* Infinite stairs? */
    if (gCurrLevelNum == LEVEL_CASTLE && gCurrAreaIndex == 2 && gMarioState->numStars < 70) {
        if (gMarioState->floor != NULL && gMarioState->floor->room == 6) {
            if (gMarioState->pos[2] < 2540.0f) {
                shouldPlay = TRUE;
            }
        }
    }

    if (sPlayingInfiniteStairs ^ shouldPlay) {
        sPlayingInfiniteStairs = shouldPlay;
        if (shouldPlay) {
            play_secondary_music(SEQ_EVENT_ENDLESS_STAIRS, 0, 255, 1000);
        } else {
            stop_secondary_music(500);
        }
    }
#endif
}

/**
 * Called from threads: thread5_game_loop
 */
void set_background_music(u16 a, u16 seqArgs, s16 fadeTimer) {
    if (gResetTimer == 0 && (seqArgs != sCurrentMusic
#ifdef BETTER_REVERB
        || gBetterReverbPresetValue != activeBetterReverbPreset
#endif
    )) {
        if (gCurrCreditsEntry != NULL) {
            sound_reset(7);
        } else {
            sound_reset(a);
        }

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
        if (!gNeverEnteredCastle || seqArgs != SEQ_LEVEL_INSIDE_CASTLE)
#endif
        {
            play_music(SEQ_PLAYER_LEVEL, seqArgs, fadeTimer);
            sCurrentMusic = seqArgs;
        }
    }
}

/**
 * Called from threads: thread3_main, thread5_game_loop
 */
void fadeout_music(s16 fadeOutTime) {
    func_803210D4(fadeOutTime);
    sCurrentMusic = MUSIC_NONE;
    sCurrentShellMusic = MUSIC_NONE;
    sCurrentCapMusic = MUSIC_NONE;
}

/**
 * Called from threads: thread5_game_loop
 */
void fadeout_level_music(s16 fadeTimer) {
    seq_player_fade_out(SEQ_PLAYER_LEVEL, fadeTimer);
    sCurrentMusic = MUSIC_NONE;
    sCurrentShellMusic = MUSIC_NONE;
    sCurrentCapMusic = MUSIC_NONE;
}

/**
 * Called from threads: thread5_game_loop
 */
void play_cutscene_music(u16 seqArgs) {
    play_music(SEQ_PLAYER_LEVEL, seqArgs, 0);
    sCurrentMusic = seqArgs;
}

/**
 * Called from threads: thread5_game_loop
 */
void play_shell_music(void) {
    play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP | SEQ_VARIATION), 0);
    sCurrentShellMusic = SEQUENCE_ARGS(4, SEQ_EVENT_POWERUP | SEQ_VARIATION);
}

/**
 * Called from threads: thread5_game_loop
 */
void stop_shell_music(void) {
    if (sCurrentShellMusic != MUSIC_NONE) {
        stop_background_music(sCurrentShellMusic);
        sCurrentShellMusic = MUSIC_NONE;
    }
}

/**
 * Called from threads: thread5_game_loop
 */

#ifdef PERSISTENT_CAP_MUSIC
static s8 sDoResetMusic = FALSE;
extern void stop_cap_music(void);
#endif

void play_cap_music(u16 seqArgs) {
#ifdef PERSISTENT_CAP_MUSIC
    if (sDoResetMusic) {
        sDoResetMusic = FALSE;
        stop_cap_music();
    }
#endif

    play_music(SEQ_PLAYER_LEVEL, seqArgs, 0);
    if (sCurrentCapMusic != MUSIC_NONE && sCurrentCapMusic != seqArgs) {
        stop_background_music(sCurrentCapMusic);
    }
    sCurrentCapMusic = seqArgs;
}

/**
 * Called from threads: thread5_game_loop
 */
void fadeout_cap_music(void) {
    if (sCurrentCapMusic != MUSIC_NONE) {
        fadeout_background_music(sCurrentCapMusic, 600);
#ifdef PERSISTENT_CAP_MUSIC
        sDoResetMusic = TRUE;
#endif
    }
}

/**
 * Called from threads: thread5_game_loop
 */
void stop_cap_music(void) {
    if (sCurrentCapMusic != MUSIC_NONE) {
        stop_background_music(sCurrentCapMusic);
        sCurrentCapMusic = MUSIC_NONE;
    }
}

/**
 * Called from threads: thread5_game_loop
 */
void play_menu_sounds_extra(s32 a, void *b) {
    play_sound(sMenuSoundsExtra[a], b);
}

/**
 * Called from threads: thread5_game_loop
 */
void audio_game_loop_tick(void) {
    // File select keeps its global preference even after game over retains the old file.
    set_sound_mode(game_options_active_file() < 0 || gCurrLevelNum == LEVEL_UNKNOWN_1
                       ? save_file_get_sound_mode()
                       : game_options_get(GO_SOUND_OUTPUT));
    if ((sBackgroundNoiseReasons & 1) && sPauseMusicFadeTargetScale != game_options_pause_gain()) {
        fade_pause_music_volume(game_options_pause_gain());
    }
    update_pause_music_volume();
    audio_signal_game_loop_tick();
}

/**
 * Sound processing thread. Runs at 60 FPS.
 */
void thread4_sound(UNUSED void *arg) {
#if defined(VERSION_JP) || defined(VERSION_US)
    struct SPTask *pendingTask = NULL;
#endif
    audio_init();
    sound_init();

    osCreateMesgQueue(&sSoundMesgQueue, sSoundMesgBuf, ARRAY_COUNT(sSoundMesgBuf));
    set_vblank_handler(1, &sSoundVblankHandler, &sSoundMesgQueue, (OSMesg) 512);

    while (TRUE) {
        OSMesg msg;

        osRecvMesg(&sSoundMesgQueue, &msg, OS_MESG_BLOCK);
#if AUDIO_IMMEDIATE_TASKS
        if (msg != (OSMesg) 512) {
            audio_submit_late_frame_buffer();
            continue;
        }
#endif
        profiler_audio_started(); // also starts PROFILER_TIME_SUB_AUDIO_UPDATE inside
        if (gResetTimer < 25) {
#if defined(VERSION_JP) || defined(VERSION_US)
            if (pendingTask == NULL) {
                pendingTask = create_next_audio_frame_task();
            }
            if (pendingTask != NULL) {
#if AUDIO_IMMEDIATE_TASKS
                pendingTask->msgqueue = &sSoundMesgQueue;
#endif
                if (dispatch_audio_sptask(pendingTask)) {
                    pendingTask = NULL;
                }
            }
#else
            struct SPTask *spTask = create_next_audio_frame_task();
            if (spTask != NULL) {
                dispatch_audio_sptask(spTask);
            }
#endif
        }
        profiler_audio_completed(); // also completes PROFILER_TIME_SUB_AUDIO_UPDATE inside
    }
}
