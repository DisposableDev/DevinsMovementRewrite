"""Exercise real sound requests; this does not simulate audible playback."""
from pathlib import Path
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class SwimmingSoundPriorityTests(unittest.TestCase):
    def test_splash_overlaps_swimming_and_retains_swim_transitions(self):
        audio = Source(ROOT / "src/audio/external.c")
        code = r'''
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int16_t s16;
typedef int32_t s32;
typedef float f32;
#include "sounds.h"
#define SOUND_MAX_FRESHNESS 10
#define sqr(x) ((x) * (x))
/* CHARACTERISTICS */
static struct SoundCharacteristics sSoundBanks[SOUND_BANK_COUNT][4];
static u8 sSoundBankDisabled[SOUND_BANK_COUNT], sSoundMovingSpeed[SOUND_BANK_COUNT];
static u8 sSoundBankFreeListFront[SOUND_BANK_COUNT], sSoundBankUsedListBack[SOUND_BANK_COUNT];
static void update_background_music_after_sound(u8 bank, u8 sound) {
    (void)bank; (void)sound;
}
/* REQUEST */
static f32 mario[3] = {0, 0, 500};
static void existing(u32 bits) {
    memset(sSoundBanks, 0, sizeof(sSoundBanks));
    u8 bank = bits >> SOUNDARGS_SHIFT_BANK;
    sSoundBanks[bank][0].next = 1;
    sSoundBanks[bank][1].next = 0xff;
    sSoundBanks[bank][1].x = mario;
    sSoundBanks[bank][1].soundBits = bits;
    sSoundBanks[bank][1].soundStatus = SOUND_STATUS_PLAYING;
}
static void transition(u32 previous, u32 next) {
    assert(previous >> SOUNDARGS_SHIFT_BANK == next >> SOUNDARGS_SHIFT_BANK);
    existing(previous);
    process_sound_request(next, mario, 0, 0);
    assert(sSoundBanks[next >> SOUNDARGS_SHIFT_BANK][1].soundBits == next);
}
int main(void) {
    const u32 strokes[] = { SOUND_ACTION_SWIM, SOUND_ACTION_SWIM_FAST };
    const u32 overlaps[] = { SOUND_ACTION_SWIM_SURFACE,
                            SOUND_ACTION_FLUTTER_KICK, SOUND_ACTION_METAL_STEP_WATER };
    for (unsigned stroke = 0; stroke < 2; stroke++) {
        for (unsigned overlap = 0; overlap < sizeof(overlaps)/sizeof(overlaps[0]); overlap++) {
            transition(overlaps[overlap], strokes[stroke]);
        }
        transition(strokes[stroke], SOUND_ACTION_FLUTTER_KICK);
        transition(strokes[stroke], strokes[1-stroke]);
    }
    // A late low-priority surface sound cannot overwrite the first stroke.
    // Scripted interaction cues retain precedence.
    existing(SOUND_ACTION_SWIM);
    process_sound_request(SOUND_ACTION_SWIM_SURFACE, mario, 0, 0);
    assert(sSoundBanks[SOUND_BANK_ACTION][1].soundBits == SOUND_ACTION_SWIM);
    existing(SOUND_ACTION_READ_SIGN);
    process_sound_request(SOUND_ACTION_SWIM, mario, 0, 0);
    assert(sSoundBanks[SOUND_BANK_ACTION][1].soundBits == SOUND_ACTION_READ_SIGN);
    // Keep the accepted independent twirl and entry-overlay sound channels.
    assert(SOUND_ACTION_TWIRL >> SOUNDARGS_SHIFT_BANK != SOUND_ACTION_SWIM >> SOUNDARGS_SHIFT_BANK);
    assert(SOUND_ACTION_SWIM_OVER_PLUNGE >> SOUNDARGS_SHIFT_BANK != SOUND_ACTION_SWIM >> SOUNDARGS_SHIFT_BANK);
    // Allocate real requests at Mario's shared source. All four channels survive
    // the first stroke, later strokes and flutter kicks, in either request order.
    for (int reverse = 0; reverse < 2; reverse++) {
        memset(sSoundBanks, 0, sizeof(sSoundBanks));
        memset(sSoundBankUsedListBack, 0, sizeof(sSoundBankUsedListBack));
        for (u8 bank = 0; bank < SOUND_BANK_COUNT; bank++) {
            sSoundBanks[bank][0].next = 0xff;
            sSoundBankFreeListFront[bank] = 1;
            sSoundBanks[bank][1].next = 2;
            sSoundBanks[bank][2].next = 3;
            sSoundBanks[bank][3].next = 0xff;
        }
        const u32 concurrent[] = { SOUND_ACTION_WATER_PLUNGE, SOUND_ACTION_SWIM,
                                  SOUND_ACTION_TWIRL, SOUND_ACTION_SWIM_OVER_PLUNGE };
        for (int i = 0; i < 4; i++) {
            process_sound_request(concurrent[reverse ? 3-i : i], mario, 0, 0);
        }
        for (int i = 0; i < 4; i++) {
            assert(sSoundBanks[concurrent[i] >> SOUNDARGS_SHIFT_BANK][1].soundBits == concurrent[i]);
        }
        process_sound_request(SOUND_ACTION_SWIM_FAST, mario, 0, 0);
        process_sound_request(SOUND_ACTION_FLUTTER_KICK, mario, 0, 0);
        assert(sSoundBanks[SOUND_BANK_ACTION][1].soundBits == SOUND_ACTION_FLUTTER_KICK);
        assert(sSoundBanks[SOUND_ACTION_WATER_PLUNGE >> SOUNDARGS_SHIFT_BANK][1].soundBits == SOUND_ACTION_WATER_PLUNGE);
        assert(sSoundBanks[SOUND_ACTION_TWIRL >> SOUNDARGS_SHIFT_BANK][1].soundBits == SOUND_ACTION_TWIRL);
    }
    return 0;
}
'''
        code = code.replace("/* CHARACTERISTICS */", audio.declaration("struct", "SoundCharacteristics"))
        code = code.replace("/* REQUEST */", audio.function("process_sound_request"))
        sequence = (ROOT / "sound/sequences/00_sound_player.s").read_text()
        self.assertIn("sound_ref .chan_4E5 // 0x8A:", sequence)
        self.assertIn(".set .channelD_table, .channel38_table", sequence)
        with tempfile.TemporaryDirectory(prefix="swimming-sounds-") as name:
            path = Path(name)
            (path / "test.c").write_text(code)
            subprocess.run(["cc", "-std=c99", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=undefined", "-I", str(ROOT / "include"),
                            str(path / "test.c"), "-lm", "-o", str(path / "test")], check=True)
            subprocess.run([str(path / "test")], check=True)


if __name__ == "__main__":
    unittest.main()
