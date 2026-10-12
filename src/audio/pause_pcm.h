#ifndef AUDIO_PAUSE_PCM_H
#define AUDIO_PAUSE_PCM_H

#include <PR/ultratypes.h>

/* Game-thread requests share one pause/cake stream; the audio thread owns its byte cursor.
 * Start selects a clip: TRUE when accepted, FALSE for the unsupported Shindou ABI.
 * Repeating that clip continues it; a clip change or mixer-observed stop resets the cursor. */
s32 pause_pcm_start(void);
// Stop the shared pause/cake request; the independent scream survives.
void pause_pcm_stop(void);
s32 yoshi_cake_pcm_start(void);
// Query the request, through final-chunk generation; not completion of queued audible output.
s32 yoshi_cake_pcm_is_playing(void);
// Independent one-shot stream; ordinary sound/quote stops cannot cancel it.
// Returns FALSE if already requested or using the unsupported Shindou ABI.
s32 simpleflips_scream_pcm_start(void);
/* Mix mono PCM into both dry channels. byteCount is a multiple of 16;
 * scratch/left/right are DMEM byte offsets. Returns the next command slot. */
u64 *pause_pcm_mix(u64 *cmd, u32 byteCount, u16 scratch, u16 left, u16 right);

#endif
