#ifndef SIMPLEFLIPS_SCREAM_H
#define SIMPLEFLIPS_SCREAM_H

#include "types.h"

// Game-thread state: loading a different file rearms the one-time replacement.
void simpleflips_scream_load_file(s32 fileNum);
s32 simpleflips_scream_try_play(void);

#endif
