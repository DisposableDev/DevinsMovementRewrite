#ifndef GAME_OPTIONS_AUDIO_H
#define GAME_OPTIONS_AUDIO_H

#include <PR/ultratypes.h>

f32 game_options_music_gain(void);
f32 game_options_effects_gain(s32 voice);
f32 game_options_health_warning_gain(void);
f32 game_options_pause_gain(void);
s32 game_options_audio_needs_output(void);
void game_options_audio_reset(void);
// Interleaved signed PCM; caller owns a completed, unqueued buffer and cache I/O.
s32 game_options_audio_process(s16 *samples, u32 frames, s32 frequency);

#endif
