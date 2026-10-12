#!/usr/bin/env python3
"""Exercise production sound selection; this does not establish audible playback."""
from pathlib import Path
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


class BowserSoundPriorityTests(unittest.TestCase):
    def test_wave_sounds_do_not_suppress_footsteps(self):
        audio = Source(ROOT / 'src/audio/external.c')
        code = r'''
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int16_t s16;
typedef float f32;
#include "sounds.h"
#define SOUND_MAX_FRESHNESS 10
#define sqr(x) ((x) * (x))
/* CHARACTERISTICS */
static struct SoundCharacteristics sSoundBanks[SOUND_BANK_COUNT][40];
static u8 sCurrentSound[SOUND_BANK_COUNT][16];
static u8 sMaxChannelsForSoundBank[SOUND_BANK_COUNT];
static u8 sUsedChannelsForSoundBank[SOUND_BANK_COUNT];
static u8 sNumSoundsInBank[SOUND_BANK_COUNT];
static f32 positions[SOUND_BANK_COUNT][40][3];
static void delete_sound_from_bank(u8 bank, u8 sound) {
    (void)bank; (void)sound; assert(0 && "Unexpected stale sound");
}
static void update_background_music_after_sound(u8 bank, u8 sound) {
    (void)bank; (void)sound;
}
/* SELECT */
static u8 add_sound(u32 bits, f32 z) {
    u8 bank = bits >> SOUNDARGS_SHIFT_BANK;
    u8 index = ++sNumSoundsInBank[bank];
    struct SoundCharacteristics *sound = &sSoundBanks[bank][index];
    positions[bank][index][2] = z;
    sound->x = &positions[bank][index][0];
    sound->y = &positions[bank][index][1];
    sound->z = &positions[bank][index][2];
    sound->soundBits = bits;
    sound->soundStatus = SOUND_STATUS_WAITING;
    sound->freshness = SOUND_MAX_FRESHNESS;
    sound->next = 0xff;
    sSoundBanks[bank][index - 1].next = index;
    return index;
}
static void reset(void) {
    memset(sSoundBanks, 0, sizeof(sSoundBanks));
    memset(sNumSoundsInBank, 0, sizeof(sNumSoundsInBank));
    memset(sCurrentSound, 0xff, sizeof(sCurrentSound));
    memset(sMaxChannelsForSoundBank, 1, sizeof(sMaxChannelsForSoundBank));
    for (int bank = 0; bank < SOUND_BANK_COUNT; bank++) {
        sSoundBanks[bank][0].next = 0xff;
    }
}
int main(void) {
    // Amp buzz alone can play alongside Bowser on its separate channel.
    reset();
    u8 bowser = add_sound(SOUND_OBJ_BOWSER_WALK, 1000);
    u8 amp = add_sound(SOUND_AIR_AMP_BUZZ, 100);
    select_current_sounds(SOUND_BANK_OBJ);
    select_current_sounds(SOUND_BANK_AIR);
    assert(sCurrentSound[SOUND_BANK_OBJ][0] == bowser);
    assert(sCurrentSound[SOUND_BANK_AIR][0] == amp);
    // Waves appear on recovery landings. Nearby footsteps must not drown out Bowser.
    for (int side = -1; side <= 1; side += 2) {
        for (int distance = 500; distance <= 6000; distance += 500) {
            reset();
            u8 bowser = add_sound(SOUND_OBJ_BOWSER_WALK, side * distance);
            add_sound(SOUND_OBJ_BOBOMB_WALK, 100);
            for (int bully = 0; bully < 5; bully++) {
                add_sound(SOUND_OBJ_BULLY_WALK_LARGE, 100 + bully * 50);
            }
            u8 amp = add_sound(SOUND_AIR_AMP_BUZZ, 100);
            select_current_sounds(SOUND_BANK_OBJ);
            select_current_sounds(SOUND_BANK_AIR);
            assert(sCurrentSound[SOUND_BANK_OBJ][0] == bowser);
            assert(sCurrentSound[SOUND_BANK_AIR][0] == amp);
            // A playing footstep survives refreshed wave footsteps and Amp buzz.
            sSoundBanks[SOUND_BANK_OBJ][bowser].soundStatus = SOUND_STATUS_PLAYING;
            sSoundBanks[SOUND_BANK_OBJ][bowser].soundBits =
                (SOUND_OBJ_BOWSER_WALK & ~SOUNDARGS_MASK_STATUS) | SOUND_STATUS_PLAYING;
            select_current_sounds(SOUND_BANK_OBJ);
            select_current_sounds(SOUND_BANK_AIR);
            assert(sCurrentSound[SOUND_BANK_OBJ][0] == bowser);
            assert(sCurrentSound[SOUND_BANK_AIR][0] == amp);
        }
    }
    // Pickup/spin/breath cues must beat the recently promoted boss footsteps,
    // as well as nearby wave enemies, both before and after playback begins.
    const u32 cues[] = { SOUND_OBJ_BOWSER_TAIL_PICKUP, SOUND_OBJ_BOWSER_SPINNING,
                        SOUND_OBJ_BOWSER_INHALING, SOUND_OBJ2_BOWSER_ROAR,
                        SOUND_OBJ_BOWSER_DEFEATED, SOUND_AIR_BOWSER_SPIT_FIRE,
                        SOUND_GENERAL_BOWSER_BOMB_EXPLOSION, SOUND_OBJ_BOWSER_LAUGH,
                        SOUND_OBJ_BOWSER_INTRO_LAUGH, SOUND_OBJ2_BOWSER_TELEPORT };
    for (unsigned cue = 0; cue < sizeof(cues) / sizeof(cues[0]); cue++) {
        for (int distance = 500; distance <= 6000; distance += 500) {
            reset();
            add_sound(SOUND_OBJ_BOWSER_WALK, 100);
            add_sound(SOUND_OBJ_BULLY_WALK_LARGE, 100);
            add_sound(SOUND_OBJ_BULLY_EXPLODE_LAVA, 100);
            add_sound(SOUND_AIR_AMP_BUZZ, 100);
            add_sound(SOUND_GENERAL_BOWSER_PLATFORM_FALL, 100);
            u8 bank = cues[cue] >> SOUNDARGS_SHIFT_BANK;
            u8 boss = add_sound(cues[cue], distance);
            select_current_sounds(bank);
            assert(sCurrentSound[bank][0] == boss);
            sSoundBanks[bank][boss].soundStatus = SOUND_STATUS_PLAYING;
            sSoundBanks[bank][boss].soundBits =
                (cues[cue] & ~SOUNDARGS_MASK_STATUS) | SOUND_STATUS_PLAYING;
            select_current_sounds(bank);
            assert(sCurrentSound[bank][0] == boss);
        }
    }
    // Preserve precedence for higher-priority object effects.
    reset();
    add_sound(SOUND_OBJ_BOWSER_WALK, 500);
    u8 critical = add_sound(SOUND_OBJ_RELEASE_MARIO, 100);
    select_current_sounds(SOUND_BANK_OBJ);
    assert(sCurrentSound[SOUND_BANK_OBJ][0] == critical);
    return 0;
}
'''
        code = code.replace('/* CHARACTERISTICS */',
                            audio.declaration('struct', 'SoundCharacteristics'))
        code = code.replace('/* SELECT */', audio.function('select_current_sounds'))
        with tempfile.TemporaryDirectory(prefix='bowser-sound-priority-') as directory:
            path = Path(directory)
            (path / 'test.c').write_text(code)
            subprocess.run(['cc', '-std=c99', '-O2', '-Wall', '-Wextra', '-Werror',
                            '-I', str(ROOT / 'include'), str(path / 'test.c'),
                            '-lm', '-o', str(path / 'test')], check=True)
            subprocess.run([str(path / 'test')], check=True)


if __name__ == '__main__':
    unittest.main()
