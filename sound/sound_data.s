.include "macros.inc"
#include "config/config_audio.h"

.section .data

glabel gSoundDataADSR
.incbin "sound/sound_data.ctl"
.balign 16

glabel gSoundDataRaw
.incbin "sound/sound_data.tbl"
.balign 16

glabel gMusicData
.incbin "sound/sequences.bin"
.balign 16

#ifndef VERSION_SH
glabel gBankSetsData
.incbin "sound/bank_sets"
.balign 16
#endif

#ifndef VERSION_SH
.balign 16
glabel gDunkeyPausePcmStart
#if AUDIO_OUTPUT_FREQUENCY == 30000
.incbin "sound/pcm/dunkey_pause_30k.s16be"
#else
.incbin "sound/pcm/dunkey_pause.s16be"
#endif
.global gDunkeyPausePcmEnd
gDunkeyPausePcmEnd:
// Keep the final PCM chunk and complete sample-cache DMA fetch inside zero data.
.balign 16, 0
.space (144 * 9), 0
.balign 16
glabel gYoshiCakePcmStart
#if AUDIO_OUTPUT_FREQUENCY == 30000
.incbin "sound/pcm/yoshi_coin_cake_30k.s16be"
#else
.incbin "sound/pcm/yoshi_coin_cake.s16be"
#endif
.global gYoshiCakePcmEnd
gYoshiCakePcmEnd:
.balign 16, 0
.space (144 * 9), 0
.balign 16
glabel gSimpleFlipsScreamPcmStart
#if AUDIO_OUTPUT_FREQUENCY == 30000
.incbin "sound/pcm/simpleflips_scream_30k.s16be"
#else
.incbin "sound/pcm/simpleflips_scream.s16be"
#endif
.global gSimpleFlipsScreamPcmEnd
gSimpleFlipsScreamPcmEnd:
.balign 16, 0
.space (144 * 9), 0
#endif
