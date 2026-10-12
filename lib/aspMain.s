#include "macros.inc"
#include "config/config_audio.h"
.set UCODE_SIZE, 0x800

.section .text


/* Audio Bins */

/* Normal audio uses the original binary; experiments use assembled microcode. */

.balign 16
glabel aspMainTextStart
#if VERSION_SH == 1 || (AUDIO_TRIAL >= 1 && AUDIO_TRIAL <= 7) || AUDIO_TRIAL == 10 || AUDIO_QUALITY_TRIAL || AUDIO_LINEAR_ENVELOPES
    .incbin "rsp/audio.bin"
#else
    .incbin "lib/PR/audio/aspMain.bin"
#endif
glabel aspMainTextEnd

/* DATA SECTION START */

.section .data

/* Audio Data */

.balign 16
glabel aspMainDataStart
#if VERSION_SH == 1 || (AUDIO_TRIAL >= 1 && AUDIO_TRIAL <= 7) || AUDIO_TRIAL == 10 || AUDIO_QUALITY_TRIAL || AUDIO_LINEAR_ENVELOPES
    .incbin "rsp/audio_data.bin"
#else
    .incbin "lib/PR/audio/aspMain_data.bin"
#endif
glabel aspMainDataEnd
