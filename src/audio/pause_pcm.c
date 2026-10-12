#include <ultra64.h>

#include "pause_pcm.h"
#include "load.h"
#include "macros.h"
#include "game/game_options_audio.h"

#ifndef VERSION_SH
extern u8 gDunkeyPausePcmStart[];
extern u8 gDunkeyPausePcmEnd[];
extern u8 gYoshiCakePcmStart[];
extern u8 gYoshiCakePcmEnd[];
extern u8 gSimpleFlipsScreamPcmStart[];
extern u8 gSimpleFlipsScreamPcmEnd[];

enum StreamedPcmClip { STREAMED_PCM_NONE, STREAMED_PCM_PAUSE, STREAMED_PCM_YOSHI_CAKE };

// The game thread selects the shared pause/cake clip; the audio thread owns its cursor
// and clears the request when its final chunk is generated.
static volatile u32 sRequestedClip;
static u32 sStreamByteOffset;
static u8 sStreamDmaIndex;
// Last clip observed by the mixer; repeating the same request does not restart it.
static u8 sStreamingClip;

static volatile u32 sSimpleFlipsScreamRequested;
static u32 sSimpleFlipsScreamPosition;
static u8 sSimpleFlipsScreamDmaIndex;
static u8 sSimpleFlipsScreamStarted;
#endif

s32 simpleflips_scream_pcm_start(void) {
#ifndef VERSION_SH
    if (sSimpleFlipsScreamRequested) {
        return FALSE;
    }
    sSimpleFlipsScreamRequested = TRUE;
    return TRUE;
#else
    return FALSE;
#endif
}

static u64 *simpleflips_scream_pcm_mix(u64 *cmd, u32 byteCount, u16 scratch, u16 left, u16 right) {
#ifndef VERSION_SH
    u8 *samples;
    u32 length = (uintptr_t) gSimpleFlipsScreamPcmEnd - (uintptr_t) gSimpleFlipsScreamPcmStart;

    if (!sSimpleFlipsScreamRequested) {
        sSimpleFlipsScreamStarted = FALSE;
        return cmd;
    }
    if (!sSimpleFlipsScreamStarted) {
        sSimpleFlipsScreamPosition = 0;
        sSimpleFlipsScreamDmaIndex = 0;
        sSimpleFlipsScreamStarted = TRUE;
    }

    // A separate stream survives voice stealing, landings and pause/quote stops.
    samples = dma_sample_data((uintptr_t) gSimpleFlipsScreamPcmStart + sSimpleFlipsScreamPosition,
                              ALIGN32(byteCount), 0, &sSimpleFlipsScreamDmaIndex);
    aSetBuffer(cmd++, 0, scratch, 0, ALIGN32(byteCount));
    aLoadBuffer(cmd++, VIRTUAL_TO_PHYSICAL2(samples));
    s16 gain = (s16) (0x7FFF * game_options_effects_gain(TRUE));
    aMix(cmd++, 0, gain, scratch, left);
    aMix(cmd++, 0, gain, scratch, right);
    sSimpleFlipsScreamPosition += byteCount;
    if (sSimpleFlipsScreamPosition >= length) {
        sSimpleFlipsScreamRequested = FALSE;
        sSimpleFlipsScreamStarted = FALSE;
    }
#endif
    return cmd;
}

s32 pause_pcm_start(void) {
#ifndef VERSION_SH
    sRequestedClip = STREAMED_PCM_PAUSE;
    return TRUE;
#else
    /* Shindou uses a different audio ABI. */
    return FALSE;
#endif
}

s32 yoshi_cake_pcm_start(void) {
#ifndef VERSION_SH
    sRequestedClip = STREAMED_PCM_YOSHI_CAKE;
    return TRUE;
#else
    return FALSE;
#endif
}

s32 yoshi_cake_pcm_is_playing(void) {
#ifndef VERSION_SH
    return sRequestedClip == STREAMED_PCM_YOSHI_CAKE;
#else
    return FALSE;
#endif
}

void pause_pcm_stop(void) {
#ifndef VERSION_SH
    sRequestedClip = STREAMED_PCM_NONE;
#endif
}

u64 *pause_pcm_mix(u64 *cmd, u32 byteCount, u16 scratch, u16 left, u16 right) {
#ifndef VERSION_SH
    u32 clip = sRequestedClip;
    u8 *start = clip == STREAMED_PCM_YOSHI_CAKE ? gYoshiCakePcmStart : gDunkeyPausePcmStart;
    u8 *end = clip == STREAMED_PCM_YOSHI_CAKE ? gYoshiCakePcmEnd : gDunkeyPausePcmEnd;
    u32 length = (uintptr_t) end - (uintptr_t) start;
    u8 *samples;

    if (clip == STREAMED_PCM_NONE) {
        sStreamingClip = STREAMED_PCM_NONE;
        return simpleflips_scream_pcm_mix(cmd, byteCount, scratch, left, right);
    }
    if (sStreamingClip != clip) {
        sStreamByteOffset = 0;
        sStreamDmaIndex = 0;
        sStreamingClip = clip;
    }

    /* The ROM sample has a zero tail covering a complete DMA-cache fetch.
     * Mixer commands round up to 32 bytes; load that entire range too.
     * The selected ROM asset matches the output rate: 30 kHz in the trial,
     * 32 kHz otherwise. Both play one PCM sample per output sample. */
    samples =
        dma_sample_data((uintptr_t) start + sStreamByteOffset, ALIGN32(byteCount), 0, &sStreamDmaIndex);
    aSetBuffer(cmd++, 0, scratch, 0, ALIGN32(byteCount));
    aLoadBuffer(cmd++, VIRTUAL_TO_PHYSICAL2(samples));
    s16 gain = (s16) (0x7FFF * game_options_effects_gain(TRUE));
    aMix(cmd++, 0, gain, scratch, left);
    aMix(cmd++, 0, gain, scratch, right);
    sStreamByteOffset += byteCount;
    if (sStreamByteOffset >= length) {
        sRequestedClip = STREAMED_PCM_NONE;
        sStreamingClip = STREAMED_PCM_NONE;
    }
#endif
    return simpleflips_scream_pcm_mix(cmd, byteCount, scratch, left, right);
}
