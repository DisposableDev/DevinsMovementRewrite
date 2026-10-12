#include "game_options_runtime.h"
#include "simpleflips_scream.h"

#include "audio/pause_pcm.h"
#include "engine/math_util.h"
#include "game_init.h"
#include "save_file.h"

#define SIMPLEFLIPS_SCREAM_ODDS 8192

static s32 sSimpleFlipsScreamFile = -1;
static s32 sSimpleFlipsScreamPlayed;

void simpleflips_scream_load_file(s32 fileNum) {
    if (fileNum >= 1 && fileNum <= NUM_SAVE_FILES && fileNum != sSimpleFlipsScreamFile) {
        sSimpleFlipsScreamFile = fileNum;
        sSimpleFlipsScreamPlayed = FALSE;
    }
}

s32 simpleflips_scream_try_play(void) {
    if (game_options_secrets_blocked() || sSimpleFlipsScreamFile < 1 || sSimpleFlipsScreamPlayed
        || gCurrDemoInput != NULL) {
        return FALSE;
    }
    if ((random_u16() & (SIMPLEFLIPS_SCREAM_ODDS - 1)) != 0 || !simpleflips_scream_pcm_start()) {
        return FALSE;
    }
    sSimpleFlipsScreamPlayed = TRUE;
    return TRUE;
}
