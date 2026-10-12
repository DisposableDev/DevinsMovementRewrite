#include <ultra64.h>

#if ENGINE_G16
extern u8 _gp[];

extern void __real_osCreateThread(OSThread *thread, OSId id, void (*entry)(void *), void *arg, void *sp,
                                  OSPri pri);

// Linker wrapping also covers direct calls from the SDK's PI/VI managers.
void __wrap_osCreateThread(OSThread *thread, OSId id, void (*entry)(void *), void *arg, void *sp,
                           OSPri pri) {
    __real_osCreateThread(thread, id, entry, arg, sp, pri);
    // SDK contexts restore all 64 bits; KSEG0 pointers require sign extension.
    thread->context.gp = (u64) (s64) (s32) (uintptr_t) _gp;
}
#endif
