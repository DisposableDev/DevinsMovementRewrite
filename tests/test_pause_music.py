#!/usr/bin/env python3
"""Host checks of pause fade timing and music/SFX volume separation."""
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]

PRELUDE = r'''
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>
typedef uint8_t u8;
typedef int8_t s8;
typedef int16_t s16;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t s32;
typedef float f32;
#define TRUE 1
#define FALSE 0
#define LAYERS_MAX 4
#define MUTE_BEHAVIOR_SOFTEN 1
#define SEQ_PLAYER_LEVEL 0
#define SEQ_PLAYER_ENV 1
#define SEQ_PLAYER_SFX 2
#include <string.h>
#define CHANNELS_MAX 16
#define IS_SEQUENCE_CHANNEL_VALID(channel) ((channel) != NULL)
#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : (v) > (hi) ? (hi) : (v))
struct SequencePlayer {
    f32 appliedFadeVolume, fadeVolume, muteVolumeScale, fadeVelocity, fadeVolumeScale;
    s32 muted, fadeRemainingFrames, state, recalculateVolume;
    struct SequenceChannel *channels[16];
};
static void sequence_player_disable(struct SequencePlayer *player) { (void) player; }
struct SequenceChannelLayer {
    s32 enabled, notePropertiesNeedInit;
    void *note;
    f32 freqScale, noteFreqScale, velocitySquare, noteVelocity;
    s32 pan, notePan;
};
struct SequenceChannel {
    f32 volume, volumeScale, appliedVolume, freqScale;
#if defined(VERSION_EU) || defined(VERSION_SH)
    s32 panChannelWeight;
#else
    f32 panChannelWeight;
#endif
    s32 pan, newPan, muteBehavior, enabled;
    struct SequencePlayer *seqPlayer;
    struct SequenceChannelLayer *layers[4];
    union { struct { unsigned volume:1, pan:1, freqScale:1; } as_bitfields; u8 as_u8; } changes;
};
static struct SequencePlayer gSequencePlayers[3];
static f32 gPauseMusicVolumeScale = 1.0f;
static u8 gMuteMusicPlayers, sBackgroundNoiseReasons;
static u32 gGlobalTimer, sPauseMusicFadeStartFrame;
static f32 sPauseMusicFadeStartScale = 1.0f, sPauseMusicFadeTargetScale = 1.0f;
static s32 sPauseMusicFading, sSfxMuted;
static s32 modeSelected;
static void set_sound_mode(u16 mode) { modeSelected = mode; }
static void set_pause_sfx_muted(u8 muted) { sSfxMuted = muted; }
static void seq_player_lower_volume(s32 player, s32 frames, s32 percentage) {
    (void) player; (void) frames; (void) percentage;
}
static void seq_player_unlower_volume(s32 player, s32 frames) { (void) player; (void) frames; }
static void audio_signal_game_loop_tick(void) {}
static void near(f32 actual, f32 expected) { assert(fabsf(actual - expected) < 0.00001f); }
'''

CHECKS = r'''
static void check_channels(f32 scale) {
    for (s32 player = 0; player < 3; player++) {
        struct SequenceChannelLayer layer = {0};
        struct SequenceChannel channel = {0};
        gSequencePlayers[player].fadeVolume = 0.5f;
        gSequencePlayers[player].appliedFadeVolume = 0.5f;
        channel.seqPlayer = &gSequencePlayers[player];
        channel.volume = channel.volumeScale = channel.freqScale = 1.0f;
        channel.layers[0] = &layer;
        layer.enabled = TRUE;
        layer.note = &layer;
        layer.velocitySquare = 1.0f;
#if defined(VERSION_EU) || defined(VERSION_SH)
        sequence_channel_process_sound(&channel, TRUE);
#else
        sequence_channel_process_sound(&channel);
#endif
#ifdef VERSION_SH
        f32 normal = 0.25f;
#else
        f32 normal = 0.5f;
#endif
        near(layer.noteVelocity, normal * (player == SEQ_PLAYER_SFX ? 1.0f : scale));
        assert(!gSequencePlayers[player].muted);
    }
}
int main(void) {
    for (s32 cycle = 0; cycle < 3; cycle++) {
        gGlobalTimer = cycle == 2 ? UINT32_MAX - 1 : (u32) cycle * 20;
        lower_background_noise(1);
        assert(sSfxMuted && !pause_music_is_disabled());
        for (s32 frame = 0; frame < 4; frame++) {
            f32 expected = 0.8f - 0.2f * frame;
            audio_game_loop_tick();
            near(gPauseMusicVolumeScale, expected);
            check_channels(expected);
            audio_game_loop_tick(); // Repeated updates never advance the fade.
            near(gPauseMusicVolumeScale, expected);
            gGlobalTimer++;
        }
        gGlobalTimer += 100;
        audio_game_loop_tick();
        near(gPauseMusicVolumeScale, 0.2f);
        raise_background_noise(1);
        assert(!sSfxMuted);
        for (s32 frame = 0; frame < 4; frame++) {
            f32 expected = 0.4f + 0.2f * frame;
            audio_game_loop_tick();
            near(gPauseMusicVolumeScale, expected);
            check_channels(expected);
            gGlobalTimer++;
        }
    }
    lower_background_noise(1);
    raise_background_noise(1); // Interruption starts from the current volume.
    near(gPauseMusicVolumeScale, 0.85f);
    reset_volume();
    assert(!sSfxMuted && !sPauseMusicFading);
    near(gPauseMusicVolumeScale, 1.0f);
    lower_background_noise(2);
    audio_game_loop_tick();
    near(gPauseMusicVolumeScale, 1.0f);
    raise_background_noise(2);
    return 0;
}
'''

VOLUME_CHECKS = r'''
static void volume_options(void) {
    game_options_init(NULL, NULL);
    game_options_select_file(0);
    gMuteMusicPlayers = FALSE;
    gPauseMusicVolumeScale = 1.0f;
    struct SequenceChannel channels[3] = {0};
    struct SequenceChannelLayer layers[3] = {0};
    for (s32 i = 0; i < 3; i++) {
        channels[i].seqPlayer = &gSequencePlayers[i == 0 ? SEQ_PLAYER_LEVEL : SEQ_PLAYER_SFX];
        channels[i].volume = channels[i].volumeScale = channels[i].freqScale = 1.0f;
        channels[i].enabled = TRUE;
        channels[i].layers[0] = &layers[i];
        layers[i].enabled = layers[i].notePropertiesNeedInit = TRUE;
        layers[i].velocitySquare = 1.0f;
        layers[i].note = &layers[i];
    }
    gSequencePlayers[SEQ_PLAYER_LEVEL].channels[0] = &channels[0];
    gSequencePlayers[SEQ_PLAYER_SFX].channels[0] = &channels[1];
    gSequencePlayers[SEQ_PLAYER_SFX].channels[2] = &channels[2];
    for (s32 i = 0; i < 3; i++) {
        gSequencePlayers[i].fadeVolume = gSequencePlayers[i].appliedFadeVolume = 1.0f;
        gSequencePlayers[i].fadeVolumeScale = 1.0f;
    }
    for (s32 volume = 0; volume <= 100; volume += 10) {
        game_options_set(GO_MUSIC_VOLUME, volume);
        game_options_set(GO_SFX_VOLUME, volume);
        game_options_set(GO_VOICE_VOLUME, 50);
        for (s32 repeat = 0; repeat < 12; repeat++) {
            sequence_player_process_sound(&gSequencePlayers[SEQ_PLAYER_LEVEL]);
            sequence_player_process_sound(&gSequencePlayers[SEQ_PLAYER_SFX]);
            near(layers[0].noteVelocity, volume * 0.01f);
            near(layers[1].noteVelocity, volume * 0.01f);
            near(layers[2].noteVelocity, volume * 0.005f);
            near(channels[0].volume, 1.0f);
            near(channels[1].volume, 1.0f);
        }
    }
    // Volume changes update sustained EU/SH notes without changing native fades.
    game_options_set(GO_VOICE_VOLUME, 0);
    sequence_player_process_sound(&gSequencePlayers[SEQ_PLAYER_SFX]);
    near(layers[2].noteVelocity, 0);
    near(layers[1].noteVelocity, 1);
    // Health pulses keep their native note lifecycle while only their gain changes.
    gSequencePlayers[SEQ_PLAYER_SFX].channels[0] = NULL;
    gSequencePlayers[SEQ_PLAYER_SFX].channels[SOUND_BANK_HEALTH] = &channels[1];
    for (s32 cycle = 0; cycle < 4; cycle++) {
        game_options_set(GO_HEALTH_WARNING, cycle & 1);
        for (s32 repeat = 0; repeat < 6; repeat++) {
            sequence_player_process_sound(&gSequencePlayers[SEQ_PLAYER_SFX]);
            near(layers[1].noteVelocity, cycle & 1);
            assert(layers[1].enabled && layers[1].note == &layers[1]);
            near(channels[1].volume, 1);
        }
    }
    // Bank ranges follow the channel allocation, including multi-channel banks.
    gSequencePlayers[SEQ_PLAYER_SFX].channels[SOUND_BANK_HEALTH] = NULL;
    sMaxChannelsForSoundBank[0] = 2;
    sMaxChannelsForSoundBank[SOUND_BANK_HEALTH] = 2;
    gSequencePlayers[SEQ_PLAYER_SFX].channels[SOUND_BANK_HEALTH + 2] = &channels[1];
    game_options_set(GO_HEALTH_WARNING, 0);
    sequence_player_process_sound(&gSequencePlayers[SEQ_PLAYER_SFX]);
    near(layers[1].noteVelocity, 0);
    game_options_set(GO_HEALTH_WARNING, 1);
    sequence_player_process_sound(&gSequencePlayers[SEQ_PLAYER_SFX]);
    near(layers[1].noteVelocity, 1);
    sMaxChannelsForSoundBank[0] = sMaxChannelsForSoundBank[SOUND_BANK_HEALTH] = 1;
    for (s32 mode = 0; mode < 3; mode++) {
        reset_volume();
        game_options_set(GO_PAUSE_MUSIC, mode);
        lower_background_noise(1);
        for (s32 i = 0; i < 4; i++) { audio_game_loop_tick(); gGlobalTimer++; }
        near(gPauseMusicVolumeScale, mode == 0 ? 1.0f : mode == 1 ? 0.2f : 0.0f);
        game_options_set(GO_PAUSE_MUSIC, (mode + 1) % 3);
        for (s32 i = 0; i < 4; i++) { audio_game_loop_tick(); gGlobalTimer++; }
        near(gPauseMusicVolumeScale, game_options_pause_gain());
        raise_background_noise(1);
        for (s32 i = 0; i < 4; i++) { audio_game_loop_tick(); gGlobalTimer++; }
        near(gPauseMusicVolumeScale, 1.0f);
    }
    game_options_set(GO_SOUND_OUTPUT, 1); audio_game_loop_tick(); assert(modeSelected == 1);
    game_options_select_file(1); audio_game_loop_tick(); assert(modeSelected == 0);
}
'''


class PauseMusic(unittest.TestCase):
    def test_fade_and_channels(self):
        sound = Source(ROOT / 'src/game/sound_init.c')
        actual = Source(ROOT / 'src/game/game_init.h').macros('PAUSE_BLEND_FRAMES')
        actual += Source(ROOT / 'src/game/game_options.h').region(r'^#define GAME_OPTIONS_FILES', r'^#endif')
        actual += Source(ROOT / 'src/game/game_options.c').region(r'^#define OPTIONS_MAGIC', r'\Z')
        actual += Source(ROOT / 'src/game/game_options_audio.c').region(r'^static f32 sRangeGain', r'\Z')
        actual += Source(ROOT / 'include/sounds.h').declaration('enum', 'SoundBank')
        actual += Source(ROOT / 'include/sounds.h').macros('SOUND_BANK_HEALTH')
        actual += Source(ROOT / 'src/audio/internal.h').declaration('enum', 'SequencePlayerStates')
        actual += 'static u8 sMaxChannelsForSoundBank[SOUND_BANK_COUNT] = {[0 ... SOUND_BANK_COUNT-1] = 1};\n'
        actual += Source(ROOT / 'src/audio/external.c').function('audio_options_channel_gain')
        actual += '\n'.join(sound.function(name) for name in (
            'update_pause_music_volume', 'fade_pause_music_volume', 'reset_volume',
            'lower_background_noise', 'raise_background_noise', 'pause_music_is_disabled',
            'audio_game_loop_tick'))
        effects = (ROOT / 'src/audio/effects.c').read_text()
        actual += effects[effects.index('#if defined(VERSION_EU)'):effects.index('void sequence_player_process_sound')]
        actual += Source(ROOT / 'src/audio/effects.c').function('sequence_player_process_sound')
        with tempfile.TemporaryDirectory(prefix='pause-music-') as folder:
            folder = Path(folder)
            source = folder / 'test.c'
            source.write_text(PRELUDE + actual + VOLUME_CHECKS + CHECKS.replace(
                '    return 0;\n}', '    volume_options();\n    return 0;\n}'))
            for version in ('US', 'JP', 'EU', 'SH'):
                with self.subTest(version=version):
                    executable = folder / version
                    command = shlex.split(os.environ.get('CC', 'cc')) + [
                        '-std=gnu99', '-Wall', '-Wextra', '-Werror', '-DVERSION_' + version,
                        str(source), '-lm', '-o', str(executable)]
                    subprocess.run(command, check=True)
                    subprocess.run([str(executable)], check=True)


if __name__ == '__main__':
    unittest.main()
