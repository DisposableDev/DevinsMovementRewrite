#include "gcc_profile.h"

#if GCC_PROFILE

#include <ultra64.h>
#include <gcov.h>
#include "memory.h"

#define GCOV_ROW_CAPACITY 600
#define GCOV_CHUNK_BYTES 256
#define GCOV_ISVIEWER_BUFFER 0xB3FF0020
#define GCOV_ISVIEWER_LENGTH 0xB3FF0014

extern const struct gcov_info *const __gcov_info_start[];
extern const struct gcov_info *const __gcov_info_end[];

static char sRow[GCOV_ROW_CAPACITY];
static u8 sChunk[GCOV_CHUNK_BYTES];
static u32 sChunkBytes;
static u32 sSequence;
static u32 sTotalBytes;
static u32 sChecksum;
static s32 sDumped;
volatile s32 gGccProfileDumpState;

static char *gcov_text(char *next, const char *text) {
    while (*text != '\0') {
        *next++ = *text++;
    }
    return next;
}

static char *gcov_hex(char *next, u32 value) {
    static const char digits[] = "0123456789abcdef";
    for (s32 shift = 28; shift >= 0; shift -= 4) {
        *next++ = digits[(value >> shift) & 15];
    }
    return next;
}

static void gcov_send(u32 length) {
    for (u32 offset = 0; offset < length; offset += 4) {
        u32 word = 0;
        for (u32 byte = 0; byte < 4; byte++) {
            word <<= 8;
            if (offset + byte < length) {
                word |= (u8) sRow[offset + byte];
            }
        }
        IO_WRITE(GCOV_ISVIEWER_BUFFER + offset, word);
    }
    IO_WRITE(GCOV_ISVIEWER_LENGTH, length);
}

static void gcov_flush(void) {
    static const char digits[] = "0123456789abcdef";
    if (sChunkBytes == 0) {
        return;
    }
    char *next = gcov_text(sRow, "SM64GCOV1,DATA,");
    next = gcov_hex(next, sSequence++);
    *next++ = ',';
    for (u32 i = 0; i < sChunkBytes; i++) {
        *next++ = digits[sChunk[i] >> 4];
        *next++ = digits[sChunk[i] & 15];
    }
    *next++ = '\n';
    gcov_send(next - sRow);
    sChunkBytes = 0;
}

static void gcov_bytes(const void *data, unsigned length, void *arg) {
    (void) arg;
    const u8 *bytes = data;
    for (unsigned i = 0; i < length; i++) {
        sChunk[sChunkBytes++] = bytes[i];
        sTotalBytes++;
        sChecksum = (sChecksum ^ bytes[i]) * 16777619u;
        if (sChunkBytes == GCOV_CHUNK_BYTES) {
            gcov_flush();
        }
    }
}

static void gcov_filename(const char *filename, void *arg) {
    __gcov_filename_to_gcfn(filename, gcov_bytes, arg);
}

static void *gcov_allocate(unsigned length, void *arg) {
    (void) arg;
    return main_pool_alloc(length, MEMORY_POOL_LEFT);
}

void gcc_profile_dump(void) {
    if (sDumped) {
        return;
    }
    sDumped = TRUE;
    sChecksum = 2166136261u;
    char *next = gcov_text(sRow, "SM64GCOV1,BEGIN\n");
    gcov_send(next - sRow);

    // Collision counters are owned by this thread and stay stable during export.
    main_pool_push_state();
    for (const struct gcov_info *const *info = __gcov_info_start; info != __gcov_info_end; info++) {
        __gcov_info_to_gcda(*info, gcov_filename, gcov_bytes, gcov_allocate, NULL);
    }
    main_pool_pop_state();
    gcov_flush();
    next = gcov_text(sRow, "SM64GCOV1,END,");
    next = gcov_hex(next, sTotalBytes);
    *next++ = ',';
    next = gcov_hex(next, sChecksum);
    *next++ = '\n';
    gcov_send(next - sRow);
    gGccProfileDumpState = 2;
}

#endif
