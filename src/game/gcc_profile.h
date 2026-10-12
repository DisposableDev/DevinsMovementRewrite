#ifndef GCC_PROFILE_H
#define GCC_PROFILE_H

#if GCC_PROFILE
#include <ultra64.h>

// Debugger mailbox: 0 = idle, 1 = export requested, 2 = export complete.
extern volatile s32 gGccProfileDumpState;
void gcc_profile_dump(void);
#endif

#endif
