#include "game_options_audio.h"

#include "game_options.h"

static f32 sRangeGain = 1.0f;
static s32 sRangeFile = -2;

f32 game_options_music_gain(void) {
    return game_options_get(GO_MUSIC_VOLUME) * 0.01f;
}

f32 game_options_effects_gain(s32 voice) {
    (void) voice;
    return game_options_get(GO_SFX_VOLUME) * 0.01f;
}

f32 game_options_health_warning_gain(void) {
    return game_options_get(GO_HEALTH_WARNING) ? game_options_effects_gain(0) : 0.0f;
}

f32 game_options_pause_gain(void) {
    static const f32 gains[] = { 1.0f, 0.2f, 0.0f };
    return gains[game_options_get(GO_PAUSE_MUSIC)];
}

void game_options_audio_reset(void) {
    sRangeGain = 1.0f;
    sRangeFile = -2;
}

s32 game_options_audio_needs_output(void) {
    if (!game_options_get(GO_AUDIO_RANGE)) {
        sRangeGain = 1.0f;
    }
    return game_options_get(GO_MASTER_VOLUME) != 100 || game_options_get(GO_AUDIO_BALANCE) != 10
           || game_options_get(GO_AUDIO_RANGE);
}

s32 game_options_audio_process(s16 *samples, u32 frames, s32 frequency) {
    s32 master = game_options_get(GO_MASTER_VOLUME);
    s32 balance = game_options_get(GO_AUDIO_BALANCE);
    s32 reduced = game_options_get(GO_AUDIO_RANGE);
    s32 file = game_options_active_file();
    if (file != sRangeFile) {
        sRangeGain = 1.0f;
        sRangeFile = file;
    }
    if (!frames || !game_options_audio_needs_output()) {
        return 0;
    }
    if (!master) {
        for (u32 i = 0; i < frames * 2; i++) {
            samples[i] = 0;
        }
        sRangeGain = 1.0f;
        return 1;
    }
    s32 left = master * (balance <= 10 ? 100 : (20 - balance) * 10);
    s32 right = master * (balance >= 10 ? 100 : balance * 10);
    f32 target = 1.0f;
    f32 ceiling = 2.0f;
    if (reduced) {
        s32 peak = 0;
        for (u32 i = 0; i < frames * 2; i++) {
            s32 value = samples[i];
            if (value < 0) {
                value = -value;
            }
            if (value > peak) {
                peak = value;
            }
        }
        // Stereo-linked peak compression: +6 dB for quiet audio, then a 2:1
        // slope above 1/8 full scale. A known block peak bounds makeup gain.
        target = peak <= 4096 ? 2.0f : (8192.0f + (peak - 4096) * 0.5f) / peak;
        if (peak) {
            ceiling = 32767.0f / peak;
        }
    }
    f32 attack = (frequency > 0 ? frequency : 32000) * 0.002f;
    f32 release = (frequency > 0 ? frequency : 32000) * 0.064f;
    if (attack < 1.0f) {
        attack = 1.0f;
    }
    if (release < 1.0f) {
        release = 1.0f;
    }
    for (u32 i = 0; i < frames; i++) {
        f32 gain = 1.0f;
        if (reduced) {
            sRangeGain += (target - sRangeGain) / (target < sRangeGain ? attack : release);
            if (sRangeGain > ceiling) {
                sRangeGain = ceiling;
            }
            gain = sRangeGain;
        }
        s32 l = reduced ? (s32) (samples[i * 2] * gain) : samples[i * 2];
        s32 r = reduced ? (s32) (samples[i * 2 + 1] * gain) : samples[i * 2 + 1];
        // Integer user gain is last: compression cannot raise muted output.
        samples[i * 2] = l * left / 10000;
        samples[i * 2 + 1] = r * right / 10000;
    }
    return 1;
}
