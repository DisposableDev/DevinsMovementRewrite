#include <ultra64.h>
#include "macros.h"

#include "buffers/buffers.h"
#include "main.h"
#include "game_init.h"
#include "rumble_init.h"
#include "config.h"
#include "sounds.h"
#include "mario.h"
#include "mario_character.h"
#include "level_update.h"
#include "game_options.h"

#ifdef ENABLE_RUMBLE

#define HEALTH_WARNING_RUMBLE_DELAY_VBLANKS 4
#define SOUND_RUMBLE_ID_MASK (SOUNDARGS_MASK_BANK | SOUNDARGS_MASK_SOUNDID)

OSThread gRumblePakThread;

OSPfs gRumblePakPfs;

OSMesg gRumblePakSchedulerMesgBuf[1];
OSMesgQueue gRumblePakSchedulerMesgQueue;
OSMesg gRumbleThreadVIMesgBuf[1];
OSMesgQueue gRumbleThreadVIMesgQueue;

struct RumbleData gRumbleDataQueue[3];
struct RumbleSettings gCurrRumbleSettings;

s32 sRumblePakThreadActive = FALSE;
s32 sRumblePakActive = FALSE;
s32 sRumblePakErrorCount = 0;
s32 gRumblePakTimer = 0;
static s32 sCharacterMovementRumbleVblanks;
static s32 sCharacterMovementRumbleLevel;
static s32 sRumblePakResetStopped;

// Refreshed by gameplay, not queued: sustained moves cannot fill the effect queue.
// Expiration also stops the motor if Mario stops updating during a transition.
void set_character_movement_rumble(s32 level) {
    OSIntMask mask = osSetIntMask(OS_IM_NONE);
    sCharacterMovementRumbleVblanks = level != CHARACTER_RUMBLE_NONE && gCurrDemoInput == NULL ? 6 : 0;
    sCharacterMovementRumbleLevel = level;
    osSetIntMask(mask);
}

void init_rumble_pak_scheduler_queue(void) {
    osCreateMesgQueue(&gRumblePakSchedulerMesgQueue, gRumblePakSchedulerMesgBuf, 1);
    osSendMesg(&gRumblePakSchedulerMesgQueue, (OSMesg) 0, OS_MESG_NOBLOCK);
}

void block_until_rumble_pak_free(void) {
    OSMesg msg;
    osRecvMesg(&gRumblePakSchedulerMesgQueue, &msg, OS_MESG_BLOCK);
}

void release_rumble_pak_control(void) {
    osSendMesg(&gRumblePakSchedulerMesgQueue, (OSMesg) 0, OS_MESG_NOBLOCK);
}

s32 try_acquire_rumble_pak(void) {
    return osRecvMesg(&gRumblePakSchedulerMesgQueue, NULL, OS_MESG_NOBLOCK) == 0;
}

void stop_rumble_for_reset(void) {
    sCharacterMovementRumbleVblanks = 0;
    if (sRumblePakResetStopped) {
        return;
    }
    sRumblePakResetStopped = TRUE;
    // A faulted thread may own the controller interface; reset must not wait for it.
    if (sRumblePakActive && try_acquire_rumble_pak()) {
        if (osMotorStopExForReset(&gRumblePakPfs)) {
            release_rumble_pak_control();
        }
    }
}

static void start_rumble(void) {
    if (!sRumblePakActive) {
        return;
    }

    block_until_rumble_pak_free();

    if (gResetTimer != 0) {
        release_rumble_pak_control();
        return;
    }

    if (!osMotorStart(&gRumblePakPfs)) {
        sRumblePakErrorCount = 0;
    } else {
        sRumblePakErrorCount++;
    }

    release_rumble_pak_control();
}

static void stop_rumble(void) {
    if (!sRumblePakActive) {
        return;
    }

    block_until_rumble_pak_free();

    if (gResetTimer != 0) {
        release_rumble_pak_control();
        return;
    }

    if (!osMotorStop(&gRumblePakPfs)) {
        sRumblePakErrorCount = 0;
    } else {
        sRumblePakErrorCount++;
    }

    release_rumble_pak_control();
}

static void update_rumble_pak(void) {
    s32 motorOn = FALSE;
    if (gResetTimer > 0) {
        stop_rumble_for_reset();
        return;
    }
    if (sRumblePakActive && gRumblePakPfs.channel != gPlayer1Controller->port) {
        stop_rumble();
        sRumblePakActive = FALSE;
    }

    if (gCurrRumbleSettings.delay > 0) {
        gCurrRumbleSettings.delay--;
    } else if (gCurrRumbleSettings.start > 0) {
        gCurrRumbleSettings.start--;
        motorOn = TRUE;
    } else if (gCurrRumbleSettings.timer > 0) {
        gCurrRumbleSettings.timer--;

        gCurrRumbleSettings.level -= gCurrRumbleSettings.decay;
        if (gCurrRumbleSettings.level < 0) {
            gCurrRumbleSettings.level = 0;
        }

        if (gCurrRumbleSettings.event == 1) {
            motorOn = TRUE;
        } else if (gCurrRumbleSettings.count >= 0x100) {
            gCurrRumbleSettings.count -= 0x100;
            motorOn = TRUE;
        } else {
            gCurrRumbleSettings.count +=
                ((gCurrRumbleSettings.level * gCurrRumbleSettings.level * gCurrRumbleSettings.level) / (1 << 9)) + 4;
        }
    } else {
        gCurrRumbleSettings.timer = 0;
        gCurrRumbleSettings.soundSource = NULL;
        gCurrRumbleSettings.soundBits = NO_SOUND;

        if (gCurrRumbleSettings.slip >= 5) {
            motorOn = TRUE;
        } else if ((gCurrRumbleSettings.slip >= 2) && (gNumVblanks % gCurrRumbleSettings.vibrate == 0)) {
            motorOn = TRUE;
        }
    }

    if (gCurrRumbleSettings.slip > 0) {
        gCurrRumbleSettings.slip--;
    }
    // Keep ordinary effects advancing beneath character movement feedback.
    if (sCharacterMovementRumbleVblanks > 0) {
        sCharacterMovementRumbleVblanks--;
        if ((s32) (gNumVblanks % 8) < sCharacterMovementRumbleLevel) {
            motorOn = TRUE;
        }
    }
    if (game_options_rumble_motor(motorOn)) {
        start_rumble();
    } else {
        stop_rumble();
    }
}

static s32 rumble_matches_sound(u32 queuedBits, f32 *queuedSource, u32 soundBits, f32 *source) {
    return queuedSource == source
           && (soundBits == NO_SOUND
               || (queuedBits & SOUND_RUMBLE_ID_MASK) == (soundBits & SOUND_RUMBLE_ID_MASK));
}

static void set_rumble_request(struct RumbleData *request, s16 time, s16 level, u32 soundBits,
                               f32 *source) {
    request->level = level;
    request->time = time;
    request->decay = 0;
    request->soundBits = soundBits;
    request->soundSource = source;
    request->comm = level > 70 ? 1 : 2;
}

static void update_rumble_data_queue(void) {
    // A weaker effect must not interrupt a sound-linked pulse (or vice versa).
    if (gRumbleDataQueue[0].comm
        && (!(gCurrRumbleSettings.start + gCurrRumbleSettings.timer > 0
              && (gCurrRumbleSettings.soundSource != NULL || gRumbleDataQueue[0].soundSource != NULL))
            || gRumbleDataQueue[0].level >= gCurrRumbleSettings.level)) {
        gCurrRumbleSettings.count = 0;
        gCurrRumbleSettings.start = 4;
        gCurrRumbleSettings.delay = (gRumbleDataQueue[0].soundBits & SOUND_RUMBLE_ID_MASK)
                                            == (SOUND_HEALTH_WARNING_PULSE & SOUND_RUMBLE_ID_MASK)
                                        ? HEALTH_WARNING_RUMBLE_DELAY_VBLANKS
                                        : 0;
        gCurrRumbleSettings.event  = gRumbleDataQueue[0].comm;
        gCurrRumbleSettings.timer = gRumbleDataQueue[0].time;
        gCurrRumbleSettings.level = gRumbleDataQueue[0].level;
        gCurrRumbleSettings.decay = gRumbleDataQueue[0].decay;
        gCurrRumbleSettings.soundBits = gRumbleDataQueue[0].soundBits;
        gCurrRumbleSettings.soundSource = gRumbleDataQueue[0].soundSource;
    }

    gRumbleDataQueue[0] = gRumbleDataQueue[1];
    gRumbleDataQueue[1] = gRumbleDataQueue[2];

    gRumbleDataQueue[2].comm = 0;
}

void queue_rumble_data(s16 time, s16 level) {
    OSIntMask mask;
    if (gCurrDemoInput != NULL) {
        return;
    }

    mask = osSetIntMask(OS_IM_NONE);
    set_rumble_request(&gRumbleDataQueue[2], time, level, NO_SOUND, NULL);
    osSetIntMask(mask);
}

// Audio calls this only when a sound is selected for playback. Start at the
// next rumble update instead of adding the normal three-slot queue delay.
void queue_rumble_for_sound(s16 time, s16 level, u32 soundBits, f32 *source) {
    OSIntMask mask;
    if (gCurrDemoInput != NULL || time <= 0) {
        return;
    }

    mask = osSetIntMask(OS_IM_NONE);
    if (gRumbleDataQueue[0].comm == 0 || level > gRumbleDataQueue[0].level
        || (level == gRumbleDataQueue[0].level && time >= gRumbleDataQueue[0].time)) {
        set_rumble_request(&gRumbleDataQueue[0], time, level, soundBits, source);
    }
    osSetIntMask(mask);
}

// NO_SOUND cancels every sound-linked rumble from this source.
void cancel_sound_rumble(u32 soundBits, f32 *source) {
    OSIntMask mask = osSetIntMask(OS_IM_NONE);
    s32 i;

    for (i = 0; i < 3; i++) {
        if (gRumbleDataQueue[i].comm != 0
            && rumble_matches_sound(gRumbleDataQueue[i].soundBits, gRumbleDataQueue[i].soundSource,
                                    soundBits, source)) {
            gRumbleDataQueue[i].comm = 0;
        }
    }
    if (rumble_matches_sound(gCurrRumbleSettings.soundBits, gCurrRumbleSettings.soundSource, soundBits,
                             source)) {
        gCurrRumbleSettings.start = 0;
        gCurrRumbleSettings.delay = 0;
        gCurrRumbleSettings.timer = 0;
        gCurrRumbleSettings.soundSource = NULL;
        gCurrRumbleSettings.soundBits = NO_SOUND;
    }
    osSetIntMask(mask);
}

void queue_rumble_decay(s16 decay) {
    gRumbleDataQueue[2].decay = decay;
}

u32 is_rumble_finished_and_queue_empty(void) {
    if (gCurrRumbleSettings.delay + gCurrRumbleSettings.start + gCurrRumbleSettings.timer >= 4) {
        return FALSE;
    }

    if (gRumbleDataQueue[0].comm != 0) return FALSE;
    if (gRumbleDataQueue[1].comm != 0) return FALSE;
    if (gRumbleDataQueue[2].comm != 0) return FALSE;

    return TRUE;
}

void reset_rumble_timers_slip(void) {
    if (gCurrDemoInput != NULL) {
        return;
    }

    if (gCurrRumbleSettings.slip == 0) {
        gCurrRumbleSettings.slip = 7;
    }

    if (gCurrRumbleSettings.slip < 4) {
        gCurrRumbleSettings.slip = 4;
    }

    gCurrRumbleSettings.vibrate = 7;
}

void reset_rumble_timers_vibrate(s32 level) {
    if (gCurrDemoInput != NULL) {
        return;
    }

    if (gCurrRumbleSettings.slip == 0) {
        gCurrRumbleSettings.slip = 7;
    }

    if (gCurrRumbleSettings.slip < 4) {
        gCurrRumbleSettings.slip = 4;
    }

    switch (level) {
        case 0: gCurrRumbleSettings.vibrate = 5; break;
        case 1: gCurrRumbleSettings.vibrate = 4; break;
        case 2: gCurrRumbleSettings.vibrate = 3; break;
        case 3: gCurrRumbleSettings.vibrate = 2; break;
        case 4: gCurrRumbleSettings.vibrate = 1; break;
    }
}

void queue_rumble_submerged(void) {
    if (gCurrDemoInput != NULL) {
        return;
    }

    gCurrRumbleSettings.slip = 4;
    gCurrRumbleSettings.vibrate = 4;
}

static void thread6_rumble_loop(UNUSED void *arg) {
    OSMesg msg;

	osSyncPrintf("start motor thread\n");
    cancel_rumble();

    sRumblePakThreadActive = TRUE;
	osSyncPrintf("go motor thread\n");

    while (TRUE) {
        // Block until VI
        osRecvMesg(&gRumbleThreadVIMesgQueue, &msg, OS_MESG_BLOCK);

        if (gResetTimer != 0) {
            stop_rumble_for_reset();
            sRumblePakThreadActive = FALSE;
            osStopThread(NULL);
            continue;
        }

        update_rumble_data_queue();
        update_rumble_pak();

        if (sRumblePakActive) {
            if (sRumblePakErrorCount >= 30) {
                sRumblePakActive = FALSE;
            }
        } else if (gPlayer1Controller->port >= 0
                   && (gNumVblanks % 60 == 0 || gRumblePakPfs.channel != gPlayer1Controller->port)) {
            block_until_rumble_pak_free();

            if (gResetTimer == 0) {
                sRumblePakActive =
                    osMotorInitEx(&gSIEventMesgQueue, &gRumblePakPfs, gPlayer1Controller->port) < 1;
                sRumblePakErrorCount = 0;
            }

            release_rumble_pak_control();
        }

        if (gRumblePakTimer > 0) {
            gRumblePakTimer--;
        }
    }
}

void cancel_rumble(void) {
    block_until_rumble_pak_free();
    if (gResetTimer != 0) {
        release_rumble_pak_control();
        stop_rumble_for_reset();
        return;
    }
    sCharacterMovementRumbleVblanks = 0;

    if (sRumblePakActive && gRumblePakPfs.channel != gPlayer1Controller->port) {
        osMotorStop(&gRumblePakPfs);
    }
    sRumblePakActive =
        gPlayer1Controller->port >= 0
        && osMotorInitEx(&gSIEventMesgQueue, &gRumblePakPfs, gPlayer1Controller->port) < 1;

    if (sRumblePakActive) {
        osMotorStop(&gRumblePakPfs);
    }

    gRumbleDataQueue[0].comm = 0;
    gRumbleDataQueue[1].comm = 0;
    gRumbleDataQueue[2].comm = 0;

    gCurrRumbleSettings.start = 0;
    gCurrRumbleSettings.delay = 0;
    gCurrRumbleSettings.timer = 0;
    gCurrRumbleSettings.slip  = 0;
    gCurrRumbleSettings.soundSource = NULL;
    gCurrRumbleSettings.soundBits = NO_SOUND;

    gRumblePakTimer = 0;

    release_rumble_pak_control();
}

void create_thread_6(void) {
    osCreateMesgQueue(&gRumbleThreadVIMesgQueue, gRumbleThreadVIMesgBuf, 1);
    osCreateThread(&gRumblePakThread, THREAD_6_RUMBLE, thread6_rumble_loop, NULL, gThread6Stack + THREAD6_STACK, 30);
    osStartThread(&gRumblePakThread);
}

#define VRTC 0x56525443

void rumble_thread_update_vi(void) {
    if (!sRumblePakThreadActive) {
        return;
    }

    osSendMesg(&gRumbleThreadVIMesgQueue, (OSMesg) VRTC, OS_MESG_NOBLOCK);
}

#undef VRTC

#endif
