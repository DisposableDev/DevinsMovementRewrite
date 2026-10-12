#include <ultra64.h>

#include "options_output.h"
#include "data.h"
#include "load.h"
#include "game/game_options_audio.h"

static u8 sProcessed[NUMAIBUFFERS];

void audio_options_output_reset(void) {
    for (s32 i = 0; i < NUMAIBUFFERS; i++) {
        sProcessed[i] = 0;
    }
    game_options_audio_reset();
}

void audio_options_output_begin(s32 index) {
    sProcessed[index] = 0;
}

void audio_options_output_submit(s32 index) {
    if (sProcessed[index]) {
        return;
    }
    sProcessed[index] = 1;
    if (game_options_audio_needs_output() && gAiBufferLengths[index] > 0) {
        s32 frames = gAiBufferLengths[index];
        s32 bytes = frames * 4;
#if defined(VERSION_EU) || defined(VERSION_SH)
        s32 frequency = gAudioBufferParameters.aiFrequency;
#else
        s32 frequency = gAiFrequency;
#endif
        // The RSP completed this output. Refresh cached PCM before CPU processing,
        // then publish modifications before AI DMA can own the same buffer.
        osInvalDCache(gAiBuffers[index], bytes);
        game_options_audio_process(gAiBuffers[index], frames, frequency);
        osWritebackDCache(gAiBuffers[index], bytes);
    }
}
