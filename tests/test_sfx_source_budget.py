#!/usr/bin/env python3
"""Production SFX budgeting; synthesis and audible transitions are not simulated."""
from pathlib import Path
import subprocess
import tempfile
import unittest
from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class SfxBudgetTests(unittest.TestCase):
    def test_budget_selection_and_cleanup(self):
        source = Source(ROOT / 'src/audio/external.c')
        definitions = Source(ROOT / 'include/config/config_audio.h').macros(
            'SFX_SOURCE_BUDGET|SFX_LOOP_PRIORITY_BONUS')
        production = '\n'.join(source.function(name) for name in (
            'sound_budget_can_drop', 'sound_budget_priority', 'apply_sound_source_budget'))
        code = r'''
#include <stdint.h>
#include <string.h>
#include <assert.h>
typedef int32_t s32;
typedef uint32_t u32;
typedef uint8_t u8;
#include "sounds.h"
#include "level_table.h"
#define TRUE 1
#define FALSE 0
#define EMU_CONSOLE 1
#define SEQ_PLAYER_SFX 0
static u8 gEmulator;
static int16_t gCurrLevelNum;
static float gGlobalSoundSource[3], sources[SOUND_BANK_COUNT][3];
struct SoundCharacteristics {
    s32 soundBits;
    u32 priority;
    float *x;
    u8 soundStatus;
};
static struct SoundCharacteristics sSoundBanks[SOUND_BANK_COUNT][2];
static u8 sCurrentSound[SOUND_BANK_COUNT][1], sMaxChannelsForSoundBank[SOUND_BANK_COUNT];
struct Channel {
    int soundScriptIO[8];
} channels[SOUND_BANK_COUNT];
static struct {
    struct Channel *channels[SOUND_BANK_COUNT];
} gSequencePlayers[1];
// Observe cleanup service calls; the doubles do not delete native list nodes or restore music.
static int deletionCalls, backgroundRestoreCalls;
static void delete_sound_from_bank(u8 bank, u8 index) {
    assert(sSoundBanks[bank][index].soundStatus == SOUND_STATUS_STOPPED);
    deletionCalls++;
}
static void update_background_music_after_sound(u8 bank, u8 index) {
    (void) bank;
    (void) index;
    backgroundRestoreCalls++;
}
/* PRODUCTION */
static void reset_budget_fixture(void) {
    memset(sSoundBanks, 0, sizeof(sSoundBanks));
    memset(sCurrentSound, 0xff, sizeof(sCurrentSound));
    memset(channels, 0, sizeof(channels));
    gEmulator = EMU_CONSOLE;
    gCurrLevelNum = LEVEL_BOB;
    deletionCalls = backgroundRestoreCalls = 0;
    for (int bank = 0; bank < SOUND_BANK_COUNT; bank++) {
        sMaxChannelsForSoundBank[bank] = 1;
        gSequencePlayers[0].channels[bank] = &channels[bank];
        channels[bank].soundScriptIO[0] = 1;
    }
}
// Supply one native bank winner; this fixture does not run selection or freshness ticks.
static void select_bank_sound(int bank, u32 bits, u32 priority, int playing) {
    struct SoundCharacteristics *sound = &sSoundBanks[bank][1];
    sCurrentSound[bank][0] = 1;
    sound->soundBits =
        (bits & ~SOUNDARGS_MASK_STATUS) | (playing ? SOUND_STATUS_PLAYING : SOUND_STATUS_WAITING);
    sound->soundStatus = playing ? SOUND_STATUS_PLAYING : SOUND_STATUS_WAITING;
    sound->priority = priority;
    sound->x = sources[bank];
}
static int selected_bank_count(void) {
    int result = 0;
    for (int bank = 0; bank < SOUND_BANK_COUNT; bank++) {
        result += sCurrentSound[bank][0] != 0xff;
    }
    return result;
}
// Seed five selected protected winners and four droppable winners.
// Two additional protected IDs are classified, then removed from selection.
static void seed_crowded_bank_selections(void) {
    reset_budget_fixture();
    const u32 protected[] = { SOUND_ACTION_BONK,     SOUND_MOVING_FLYING,    SOUND_MARIO_YAH_WAH_HOO,
                              SOUND_MENU_STAR_SOUND, SOUND_OBJ_GOOMBA_ALERT, SOUND_AIR_BOBOMB_LIT_FUSE,
                              SOUND_ENV_STAR };
    for (int bank = 0; bank < 7; bank++) {
        select_bank_sound(bank, protected[bank], 10000, 1);
        assert(!sound_budget_can_drop(&sSoundBanks[bank][1]));
        if (bank >= 5) {
            sCurrentSound[bank][0] = 0xff;
        }
    }
    select_bank_sound(8, SOUND_ENV_ELEVATOR1, 2000, 1);
    select_bank_sound(9, SOUND_ENV_WATERFALL2, 1600, 0);
    select_bank_sound(10, SOUND_OBJ_GOOMBA_WALK, 3000, 1);
    select_bank_sound(11, SOUND_OBJ_KOOPA_WALK, 4000, 0);
}
int main(void) {
    reset_budget_fixture();
    select_bank_sound(0, SOUND_ENV_ELEVATOR1, 2000, 1);
    apply_sound_source_budget();
    assert(selected_bank_count() == 1 && channels[0].soundScriptIO[0] == 1);
    reset_budget_fixture();
    for (int bank = 0; bank < 6; bank++) {
        select_bank_sound(bank, SOUND_OBJ_GOOMBA_ALERT, 10000, 1);
    }
    select_bank_sound(8, SOUND_ENV_MERRY_GO_ROUND_CREAKING, 0, 1);
    assert(sound_budget_can_drop(&sSoundBanks[8][1]));
    apply_sound_source_budget();
    assert(selected_bank_count() == 6 && channels[8].soundScriptIO[0] == 0);
    seed_crowded_bank_selections();
    gEmulator = 0;
    apply_sound_source_budget();
    assert(selected_bank_count() == 9 && deletionCalls == 0);
    seed_crowded_bank_selections();
    apply_sound_source_budget();
    assert(selected_bank_count() == 6
           && sCurrentSound[8][0] == 1); // Retain close-priority playing loop.
    assert(channels[9].soundScriptIO[0] == 0 && channels[10].soundScriptIO[0] == 0);
    assert(deletionCalls == 1 && backgroundRestoreCalls == 1);
    assert(sSoundBanks[10][1].soundBits == NO_SOUND); // Evicted playing one-shot cannot restart.
    assert(sSoundBanks[11][1].soundStatus == SOUND_STATUS_WAITING); // Pending cue retains expiry.
    for (int bank = 0; bank < 5; bank++) {
        assert(sCurrentSound[bank][0] == 1);
    }
    // Continued refresh and near-equal priorities must not churn the retained loop.
    for (int frame = 0; frame < 20; frame++) {
        select_bank_sound(9, SOUND_ENV_WATERFALL2, 1600 + frame, 0);
        apply_sound_source_budget();
        assert(sCurrentSound[8][0] == 1 && channels[8].soundScriptIO[0] == 1);
    }
    select_bank_sound(9, SOUND_ENV_WATERFALL2, 1000, 0); // Significantly better source may replace it.
    apply_sound_source_budget();
    assert(sCurrentSound[9][0] == 1 && sCurrentSound[8][0] == 0xff);
    assert(sSoundBanks[8][1].soundStatus == SOUND_STATUS_WAITING);
    assert((sSoundBanks[8][1].soundBits & SOUNDARGS_MASK_STATUS) == SOUND_STATUS_WAITING);
    assert(channels[8].soundScriptIO[0] == 0);
    // Supply a later bank selection of the retained loop after load falls.
    for (int bank = 0; bank < 7; bank++) {
        sCurrentSound[bank][0] = 0xff;
    }
    select_bank_sound(8, SOUND_ENV_ELEVATOR1, 2000, 0);
    apply_sound_source_budget();
    assert(sCurrentSound[8][0] == 1);
    reset_budget_fixture();
    for (int bank = 0; bank < 10; bank++) {
        select_bank_sound(bank, SOUND_OBJ_GOOMBA_ALERT, 10000, 1);
    }
    select_bank_sound(10, SOUND_ENV_ELEVATOR1, 0, 1);
    apply_sound_source_budget();
    assert(selected_bank_count() == 10); // Essential feedback may exceed the budget.
    reset_budget_fixture();
    for (int bank = 0; bank < 8; bank++) {
        select_bank_sound(bank, SOUND_OBJ_GOOMBA_ALERT, 10000, 1);
    }
    select_bank_sound(8, SOUND_ENV_ELEVATOR1, 0, 1);
    sSoundBanks[8][1].x = gGlobalSoundSource;
    apply_sound_source_budget();
    assert(selected_bank_count() == 9); // Global feedback is protected even with a droppable sound ID.
    reset_budget_fixture();
    gCurrLevelNum = LEVEL_CASTLE_GROUNDS;
    for (int bank = 0; bank < 7; bank++) {
        select_bank_sound(bank, SOUND_OBJ_GOOMBA_ALERT, 10000, 1);
    }
    select_bank_sound(8, SOUND_ENV_WATERFALL2, 100000, 1);
    select_bank_sound(9, SOUND_ENV_WATERFALL1, 0, 1);
    assert(!sound_budget_can_drop(&sSoundBanks[8][1]));
    assert(sound_budget_can_drop(&sSoundBanks[9][1]));
    apply_sound_source_budget();
    assert(selected_bank_count() == 8 && channels[8].soundScriptIO[0] == 1);
    assert(channels[9].soundScriptIO[0] == 0);
    gCurrLevelNum = LEVEL_BOB;
    assert(sound_budget_can_drop(&sSoundBanks[8][1]));
    // Even a full console budget cannot let Amp buzz evict Bowser's footsteps.
    reset_budget_fixture();
    for (int bank = 0; bank < 5; bank++) {
        select_bank_sound(bank, SOUND_OBJ_GOOMBA_ALERT, 10000, 1);
    }
    select_bank_sound(SOUND_BANK_OBJ, SOUND_OBJ_BOWSER_WALK, 10000, 1);
    select_bank_sound(SOUND_BANK_AIR, SOUND_AIR_AMP_BUZZ, 0, 1);
    apply_sound_source_budget();
    assert(sCurrentSound[SOUND_BANK_OBJ][0] == 1);
    assert(channels[SOUND_BANK_OBJ].soundScriptIO[0] == 1);
    assert(sCurrentSound[SOUND_BANK_AIR][0] == 0xff);
    return 0;
}
'''
        code = code.replace('/* PRODUCTION */', definitions + '\n' + production)
        with tempfile.TemporaryDirectory(prefix='sfx-budget-') as directory:
            path = Path(directory)
            (path / 'test.c').write_text(code)
            subprocess.run(['cc', '-std=c99', '-O2', '-Wall', '-Wextra', '-Werror',
                            '-I', str(ROOT / 'include'), '-I', str(ROOT), str(path / 'test.c'),
                            '-o', str(path / 'test')], check=True)
            subprocess.run([str(path / 'test')], check=True)


if __name__ == '__main__':
    unittest.main()
