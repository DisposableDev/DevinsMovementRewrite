#ifndef MARIO_DITHER_ALPHA_H
#define MARIO_DITHER_ALPHA_H

#include "emutest.h"

// GLideN64 on Mupen tests alpha before applying the ordered 0..7 pattern.
// Use its standard noise comparison; native RDP implementations keep the pattern.
static inline s32 mario_dither_uses_noise(void) {
    return (gEmulator & EMU_MUPEN) != 0;
}

static inline s32 mario_dither_alpha(s32 alpha) {
    return mario_dither_uses_noise() ? alpha : (alpha + 16) >> 5;
}

#endif
