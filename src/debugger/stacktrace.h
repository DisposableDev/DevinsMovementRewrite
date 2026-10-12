#pragma once

#include <PR/os_thread.h>

#include "segment_symbols.h"

typedef struct {
    u32 func;
    u32 offset;
    u32 ra;
    int line;
    char funcname[100];
} StackFrame;

#define STACK_TRAVERSAL_LIMIT (100)
#define STACK_LINE_COUNT (17)
// RA points to 2 instructions past any given callsite
#define CALLSITE_OFFSET (-8)

extern u32 stackTraceGenerated;

// libultra import
extern void __osCleanupThread();

extern far u32 generate_stack(OSThread *);
extern far char *get_stack_entry(u32 idx);

static inline u8 is_ram_word(u32 addr) {
    return (addr & 0xDF000000) == 0x80000000 && (addr & 3) == 0 && osMemSize >= sizeof(u32)
           && (addr & 0x1FFFFFFF) <= osMemSize - sizeof(u32);
}

static inline u8 is_text_addr(u32 addr) {
    if (!is_ram_word(addr)) {
        return FALSE;
    }
    return (addr >= (u32) _mainSegmentStart && addr < (u32) _mainSegmentTextEnd)
           || (addr >= (u32) _engineSegmentStart && addr < (u32) _engineSegmentTextEnd)
           || (addr >= (u32) _goddardSegmentStart && addr < (u32) _goddardSegmentTextEnd);
}
