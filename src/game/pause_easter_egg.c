#include "game_options_runtime.h"
#include <ultra64.h>

#include "sm64.h"
#include "audio/pause_pcm.h"
#include "engine/math_util.h"
#include "game_init.h"
#include "level_update.h"
#include "area.h"
#include "ingame_menu.h"
#include "save_file.h"
#include "sound_init.h"
#include "pause_easter_egg.h"

#define PAUSE_EASTER_EGG_ODDS 8
#define PAUSE_EASTER_EGG_MIN_SECONDS (1 * 60)
#define PAUSE_EASTER_EGG_MAX_SECONDS (4 * 60)

enum PauseEasterEggState {
    PAUSE_EASTER_EGG_INACTIVE,
    PAUSE_EASTER_EGG_WAITING_FOR_RELEASE,
    PAUSE_EASTER_EGG_TIMING,
};

// Opportunity state only: INACTIVE also covers a quote already handed to the PCM stream.
static enum PauseEasterEggState sPauseEasterEggState;
static OSTime sPauseEasterEggIdleStartCycles;
static OSTime sPauseEasterEggIdleDelayCycles;

static u32 pause_easter_egg_random(u32 bound) {
    u32 value;
    u32 limit = (0x10000 / bound) * bound;

    /* Rejection sampling keeps bounded rolls uniform, including the idle delay.
     * The chance roll is called only when pause opens. */
    do {
        value = random_u16();
    } while (value >= limit);
    return value % bound;
}

void pause_easter_egg_end(void) {
    sPauseEasterEggState = PAUSE_EASTER_EGG_INACTIVE;
    pause_pcm_stop();
}

void pause_easter_egg_begin(void) {
    u32 seconds;

    pause_easter_egg_end();
    if (game_options_secrets_blocked() || !pause_music_is_disabled() || gCurrDemoInput != NULL
        || gCurrCreditsEntry != NULL || gCurrSaveFileNum < 1 || gCurrSaveFileNum > NUM_SAVE_FILES
        || save_file_has_pause_easter_egg(gCurrSaveFileNum - 1)) {
        return;
    }
    if (pause_easter_egg_random(PAUSE_EASTER_EGG_ODDS) != 0) {
        return;
    }
    seconds =
        PAUSE_EASTER_EGG_MIN_SECONDS
        + pause_easter_egg_random(PAUSE_EASTER_EGG_MAX_SECONDS - PAUSE_EASTER_EGG_MIN_SECONDS + 1);
    sPauseEasterEggIdleDelayCycles = OS_USEC_TO_CYCLES((u64) seconds * 1000000);
    sPauseEasterEggState = PAUSE_EASTER_EGG_WAITING_FOR_RELEASE;
}

void pause_easter_egg_update(void) {
    u16 buttons = gPlayer1Controller->buttonDown;

    if (gMenuOptSelectIndex != MENU_OPT_NONE) {
        pause_easter_egg_end();
        return;
    }
    if (sPauseEasterEggState == PAUSE_EASTER_EGG_INACTIVE) {
        return;
    }
    if (game_options_secrets_blocked() || !pause_music_is_disabled() || gCurrDemoInput != NULL
        || gCurrCreditsEntry != NULL || gPlayer1Controller->stickMag != 0.0f) {
        sPauseEasterEggState = PAUSE_EASTER_EGG_INACTIVE;
        return;
    }
    if (sPauseEasterEggState == PAUSE_EASTER_EGG_WAITING_FOR_RELEASE) {
        /* The Start press that opened pause is not an idle-breaking input.
         * Begin measuring idle time when it is released. */
        if (buttons & ~START_BUTTON) {
            sPauseEasterEggState = PAUSE_EASTER_EGG_INACTIVE;
        } else if (buttons == 0) {
            sPauseEasterEggState = PAUSE_EASTER_EGG_TIMING;
            sPauseEasterEggIdleStartCycles = osGetTime();
        }
        return;
    }
    if (buttons != 0) {
        sPauseEasterEggState = PAUSE_EASTER_EGG_INACTIVE;
        return;
    }
    if (osGetTime() - sPauseEasterEggIdleStartCycles >= sPauseEasterEggIdleDelayCycles) {
        sPauseEasterEggState = PAUSE_EASTER_EGG_INACTIVE;
        // Consume the opportunity even if this ABI cannot start it; save only accepted starts.
        if (pause_pcm_start()) {
            save_file_mark_pause_easter_egg();
        }
    }
}
