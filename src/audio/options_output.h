#ifndef AUDIO_OPTIONS_OUTPUT_H
#define AUDIO_OPTIONS_OUTPUT_H

#include <PR/ultratypes.h>

void audio_options_output_reset(void);
void audio_options_output_begin(s32 index);
void audio_options_output_submit(s32 index);

#endif
