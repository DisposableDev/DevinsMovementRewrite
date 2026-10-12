#!/usr/bin/env python3
"""Crash-menu fault discovery, guarded reads, decoding and peripheral failure checks."""
from pathlib import Path
import os
import re
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]
BASE = r'''
#define _GNU_SOURCE
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/mman.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
#define TRUE 1
#define FALSE 0
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ARRAY_COUNT(a) ((s32)(sizeof(a) / sizeof((a)[0])))
#define UNUSED __attribute__((unused))
#define PACKED __attribute__((packed))
#define ALIGNED16 __attribute__((aligned(16)))
#define OS_USEC_TO_CYCLES(us) ((u32)(us))
'''


def functions(path, names):
    source = Source(ROOT / path)
    return "".join(source.function(name) for name in names)


class CrashMenuSafetyTests(unittest.TestCase):
    def compile_run(self, text, *, ram=False, arguments=()):
        with tempfile.TemporaryDirectory(prefix="crash-safety-") as directory:
            folder = Path(directory)
            (folder / "test.c").write_text(BASE + text)
            sanitizers = "undefined,bounds" if ram else "address,undefined"
            subprocess.run([
                "cc", "-std=gnu99", "-O1", "-Wall", "-Wextra", "-Werror",
                "-Wno-pointer-to-int-cast", "-Wno-int-to-pointer-cast", "-Wno-format",
                f"-fsanitize={sanitizers}", "-fno-sanitize-recover=all",
                str(folder / "test.c"), "-o", str(folder / "test")], check=True)
            for argument in arguments or (None,):
                command = [str(folder / "test")]
                if argument is not None:
                    command.append(str(argument))
                subprocess.run(command, check=True, timeout=5,
                               env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})

    def test_shortening_boundaries(self):
        self.compile_run(functions("src/debugger/crash_screen.c", (
            "crash_screen_ellide_string",)) + r'''
int main(void) {
    for (u32 length = 0; length < 100; length++) {
        for (u32 width = 0; width < 40; width++) {
            char *text = malloc(length + 1);
            memset(text, 'x', length);
            text[length] = '\0';
            assert(crash_screen_ellide_string(text, width) == text);
            assert(strlen(text) == MIN(length, width));
            if (length > width && width >= 5) {
                assert(strncmp(text, "(...)", 5) == 0);
            }
            free(text);
        }
    }
    return 0;
}
''')

    def test_fault_and_breakpoint_discovery(self):
        self.compile_run(r'''
#define OS_FLAG_CPU_BREAK 1
#define OS_FLAG_FAULT 2
typedef void *OSMesg;
typedef struct OSThread {
    int priority;
    u16 flags;
    struct OSThread *tlnext;
} OSThread;
static struct { OSThread thread; } gCrashScreen;
static OSThread *faulted, *active;
static OSThread *__osGetCurrFaultedThread(void) { return faulted; }
static OSThread *__osGetActiveQueue(void) { return active; }
''' + functions("src/debugger/crash_screen.c", ("get_crashed_thread",)) + r'''
int main(void) {
    assert(get_crashed_thread((OSMesg)2) == NULL);
    assert(get_crashed_thread((OSMesg)1) == NULL);
    OSThread system = {.priority = 150, .flags = OS_FLAG_FAULT};
    faulted = &system;
    assert(get_crashed_thread((OSMesg)2) == &system);
    OSThread stopped = {.priority = 100, .flags = OS_FLAG_CPU_BREAK};
    gCrashScreen.thread.flags = OS_FLAG_CPU_BREAK;
    gCrashScreen.thread.tlnext = &stopped;
    active = &gCrashScreen.thread;
    assert(get_crashed_thread((OSMesg)1) == &stopped);
    // A stale fault pointer must not replace the new breakpoint's thread.
    assert(faulted == &system);
    stopped.flags = 0;
    assert(get_crashed_thread((OSMesg)1) == NULL);
    return 0;
}
''')

    def test_sleep_uses_elapsed_count_and_stops_on_reset(self):
        self.compile_run(r'''
static u32 count = UINT32_MAX - 1000;
static int calls, resetAfter;
static u32 osGetCount(void) { calls++; count += 1000; return count; }
static int crash_screen_reset_pending(void) { return resetAfter && calls >= resetAfter; }
''' + functions("src/debugger/crash_screen.c", ("crash_screen_sleep",)) + r'''
int main(void) {
    assert(crash_screen_sleep(30));
    assert(calls == 31);
    resetAfter = calls + 3;
    assert(!crash_screen_sleep(200));
    assert(calls == resetAfter);
    return 0;
}
''')

    def test_disassembly_and_simple_stack_guard_every_word(self):
        source = Source(ROOT / "src/debugger/stacktrace.h")
        production = source.function("is_ram_word") + source.function("is_text_addr")
        self.compile_run(r'''
#define _mainSegmentStart ((void *)0x80000000)
#define _mainSegmentTextEnd ((void *)0x80001000)
#define _engineSegmentStart ((void *)0x81000000)
#define _engineSegmentTextEnd ((void *)0x81001000)
#define _goddardSegmentStart ((void *)0x82000000)
#define _goddardSegmentTextEnd ((void *)0x82001000)
static u32 osMemSize = 4096;
''' + production + r'''
typedef struct { u32 pc, ra, sp; } __OSThreadContext;
typedef struct { __OSThreadContext context; } OSThread;
typedef union { u32 d; } InsnData;
#define STACK_LINE_COUNT 17
#define CALLSITE_OFFSET (-8)
#define CRASH_SCREEN_LEFT_MARGIN 16
static u32 sProgramPosition, sCrashInstructionAddress;
static int reads, unavailable, red, highlighted, stackWords;
static void osWritebackDCacheAll(void) {}
static void reset_text_color(void) { red = 0; }
static void set_text_color(int r, int g, int b) { red = r == 255 && g == 0 && b == 0; }
static void crash_screen_println(const char *fmt, ...) {
    if (strstr(fmt, "unavailable")) unavailable++;
    if (strcmp(fmt, "    %s") == 0 && red) highlighted++;
}
static void crash_screen_print(int x, int y, const char *fmt, ...) {
    (void)x;
    (void)y;
    if (strstr(fmt, "%08X")) stackWords++;
}
static char *insn_disasm(InsnData *addr) {
    assert(is_text_addr((u32)(uintptr_t)addr));
    volatile u32 word = addr->d;
    (void)word;
    reads++;
    return "nop";
}
''' + functions("src/debugger/crash_screen.c", ("draw_disasm", "draw_stacktrace")) + r'''
int main(void) {
    void *ram = mmap((void *)0x80000000, 8192, PROT_NONE,
                    MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    assert(ram == (void *)0x80000000);
    assert(mprotect(ram, 4096, PROT_READ | PROT_WRITE) == 0);
    OSThread thread = {{0x80000FFC, 0x80000200, 0x80000FFC}};
    draw_disasm(&thread);
    assert(reads == 10 && unavailable == 6 && highlighted == 1);
    draw_stacktrace(&thread, 0);
    assert(stackWords == 1);
    thread.context.sp++;
    draw_stacktrace(&thread, 0);
    assert(stackWords == 1);
    thread.context.pc = 0x80000101;
    sProgramPosition = 0;
    highlighted = 0;
    draw_disasm(&thread);
    assert(sCrashInstructionAddress == 0x800001F8 && highlighted == 1);
    draw_disasm(&thread);
    assert(highlighted == 2);
    // A valid PC near the beginning is usable even when PC-36 is outside text.
    sProgramPosition = 0;
    thread.context.pc = 0x80000004;
    draw_disasm(&thread);
    assert(sProgramPosition == thread.context.pc);
    thread.context.pc = thread.context.ra = 0xDEADBEEF;
    sProgramPosition = 0;
    int before = reads;
    draw_disasm(&thread);
    assert(before == reads);
    assert(!is_ram_word(0x80001000) && !is_ram_word(0xA0001000));
    assert(!is_text_addr(0x81000000) && !is_text_addr(0x80000001));
    assert(munmap(ram, 8192) == 0);
    return 0;
}
''', ram=True)

    def test_decoder_reserved_fields_and_jump_targets(self):
        header = (ROOT / "src/debugger/disasm.h").read_text().replace("#pragma once", "")
        # Adapt only bitfield storage to the host's little-endian layout, retaining
        # the same R4300 word fields. Decoder bodies and instruction tables are real.
        header = header[:header.index("typedef struct")] + r'''
typedef struct PACKED { u16 function:6, shift_amt:5, rd:5; } RTypeData;
typedef struct PACKED {
    union { RTypeData rdata; u16 immediate; };
    u16 rt:5, rs:5, opcode:6;
} Insn;
typedef struct PACKED { u32 func:6, fd:5, fs:5, ft:5, fmt:5, opcode:6; } CzInsn;
typedef struct PACKED { u16 offset; u16 sub:5, rs:5, regimm:6; } BranchInsn;
''' + header[header.index("typedef union"):]
        code = re.sub(r"^#include.*\n", "", (ROOT / "src/debugger/disasm.c").read_text(),
                      flags=re.M)
        self.compile_run(header + code + r'''
int main(void) {
    InsnData instruction = {.d = 0x000110C0};
    assert(strncmp(insn_disasm(&instruction), "sll", 3) == 0);
    instruction.d = 0xFC000021;
    assert(strncmp(insn_disasm(&instruction), "sd", 2) == 0);
    // An unsupported opcode cannot masquerade as SPECIAL via its low function bits.
    instruction.d = 0x4C000021;
    assert(strncmp(insn_disasm(&instruction), "unimpl", 6) == 0);
    for (int subtype = 0; subtype < 32; subtype++) {
        instruction.d = 0x04000000 | ((u32)subtype << 16);
        char *text = insn_disasm(&instruction);
        if (subtype >= 20 || (subtype >= 4 && subtype < 16)) {
            assert(strncmp(text, "unimpl", 6) == 0);
        } else {
            assert(text[0] == 'b');
        }
    }
    for (int opcode = 2; opcode <= 3; opcode++) {
        instruction.d = ((u32)opcode << 26) | 0x02000123;
        char expected[16];
        u32 target = (((u32)(uintptr_t)&instruction + 4) & 0xF0000000)
                   | ((instruction.d & 0x03FFFFFF) << 2);
        sprintf(expected, "%08X", target);
        assert(strstr(insn_disasm(&instruction), expected));
    }
    return 0;
}
''')

    def test_serial_failures_reset_and_pending_transfer_ownership(self):
        source = Source(ROOT / "src/game/gamecube_controller.c")
        self.compile_run(r'''
#define OS_MESG_NOBLOCK 0
#define OS_READ 0
#define OS_WRITE 1
#define CONT_CMD_READ_BUTTON 1
#define CRASH_CONTROLLER_DMA_PENDING (-2)
#define OS_IM_NONE 0
typedef int OSIntMask;
static int osSetIntMask(int mask) { return mask; }
typedef int OSMesgQueue;
static OSMesgQueue __osSiAccessQueue, completion;
static s32 __osSiAccessQueueEnabled = 1;
static u8 __osContLastCmd;
static struct { u32 ramarray[16]; } __osContPifRam;
static int mode, token = 1, acquired, released, starts, pending, reset, packed;
static u32 count;
static u32 osGetCount(void) { count += 1000; return count; }
static int reset_pending(void) { return reset; }
static int __osSiDeviceBusy(void) { return mode == 2; }
static void __osPackReadData(void) { packed++; }
static void __osSiRelAccess(void) { assert(!token); token = 1; released++; }
static int osRecvMesg(OSMesgQueue *queue, void *msg, int flags) {
    (void)msg;
    assert(flags == OS_MESG_NOBLOCK);
    if (queue == &__osSiAccessQueue) {
        if (!token) return -1;
        token = 0;
        acquired++;
        return 0;
    }
    assert(queue == &completion);
    if (pending && mode != 6) {
        if (mode == 7) { reset = 1; return -1; }
        pending = 0;
        if (mode == 5) reset = 1;
        return 0;
    }
    return -1;
}
static int __osSiRawStartDma(int direction, void *buffer) {
    assert(buffer == __osContPifRam.ramarray && !token && !pending);
    assert(!reset);
    starts++;
    if ((mode == 3 && direction == OS_WRITE) || (mode == 4 && direction == OS_READ)) {
        return -1;
    }
    pending = 1;
    return 0;
}
''' + source.macros("CONTROLLER_TIMEOUT_US") + source.function("controller_wait_bounded")
                         + source.function("controller_start_dma")
                         + source.function("osContReadDataExForCrash") + r'''
int main(int argc, char **argv) {
    assert(argc == 2);
    mode = atoi(argv[1]);
    if (mode == 1) token = 0;
    if (mode == 8) reset = 1;
    if (mode == 9) __osContLastCmd = CONT_CMD_READ_BUTTON;
    int result = osContReadDataExForCrash(&completion, reset_pending);
    if (mode == 0 || mode == 9) {
        assert(result == 0 && token && !pending && acquired == 1 && released == 1);
        assert(starts == (mode == 0 ? 2 : 1));
    } else if (mode == 6 || mode == 7) {
        assert(result == CRASH_CONTROLLER_DMA_PENDING && !token && pending && !released);
        int before = starts;
        assert(osContReadDataExForCrash(&completion, reset_pending) == result);
        assert(before == starts);
    } else {
        assert(result == -1 && !pending);
        assert(acquired == released);
        if (mode == 3 || mode == 5) assert(starts == 1);
        if (mode == 4) assert(starts == 2);
        if (mode == 1 || mode == 2 || mode == 8) assert(starts == 0);
    }
    assert(packed <= 1);
    return 0;
}
''', arguments=range(10))

    def test_symbol_read_bounds_missing_data_and_busy_pi(self):
        header = (ROOT / "src/debugger/symtable.h").read_text().replace("#pragma once", "")
        parser_header = re.search(r"typedef struct \{.*?\} symtable_info_t;",
                                 (ROOT / "src/debugger/map_parser.h").read_text(), re.S)[0]
        source = Source(ROOT / "src/debugger/symtable.c")
        parser = Source(ROOT / "src/debugger/map_parser.c")
        self.compile_run(header + parser_header + r'''
#define _mapDataSegmentRomStart ((void *)0x1000)
#define _mapDataSegmentRomEnd ((void *)0x1400)
#define PI_STATUS_REG 0xFFFF0000
#define PI_STATUS_IO_BUSY 1
#define PI_STATUS_DMA_BUSY 2
#define OS_IM_NONE 0
#define osRomBase 0xB0000000
typedef int OSIntMask;
static u8 rom[1024];
static u32 SYMT_ROM = 0x1000, count;
static int sSymbolReadFailed, piBusy, reset, masked, reads;
static u32 osGetCount(void) { count += 1000; return count; }
static int crash_screen_reset_pending(void) { return reset; }
static int osSetIntMask(int value) {
    int previous = masked ? OS_IM_NONE : 1;
    masked = value == OS_IM_NONE;
    return previous;
}
static u32 io_read(u32 addr) {
    assert(masked);
    if (addr == PI_STATUS_REG) return piBusy ? PI_STATUS_DMA_BUSY : 0;
    assert(!piBusy && (addr & 3) == 0);
    addr = (addr & 0x0FFFFFFF) - 0x1000;
    assert(addr <= sizeof(rom) - 4);
    reads++;
    return ((u32)rom[addr] << 24) | ((u32)rom[addr + 1] << 16)
           | ((u32)rom[addr + 2] << 8) | rom[addr + 3];
}
#define IO_READ(addr) io_read(addr)
#define is_in_exception(addr) ((addr) == 0x80000000)
char *parse_map(char *buf, u32 size, u32 addr, u32 andOffset);
''' + source.macros("SYMBOL_READ_TIMEOUT_US") + parser.macros("UNKNOWN_SYMBOL")
                         + "".join(source.function(name) for name in (
                             "symt_rom_read", "symt_table_fits", "symt_open",
                             "symt_addrtab_entry", "symt_addrtab_search", "symt_string",
                             "symt_entry_fetch", "symt_entry_func", "symt_entry_file"))
                         + "".join(parser.function(name) for name in (
                             "__symbolize", "parse_map", "get_start_of_func", "get_symbol_info"))
                         + r'''
static void populate(void) {
    symtable_header_t header = {.head = {'S', 'Y', 'M', 'T'}, .version = 2,
        .addrtab_off = 32, .addrtab_size = 2, .symtab_off = 40, .symtab_size = 2,
        .strtab_off = 72, .strtab_size = 12};
    u32 addresses[2] = {0x80000101, 0x80000104};
    symtable_entry_t symbols[2] = {{.func_len = 4, .file_sidx = 4, .file_len = 8},
        {.func_len = 4, .file_sidx = 4, .file_len = 8, .line = 123, .func_off = 4}};
    memcpy(rom, &header, sizeof(header));
    memcpy(rom + 32, addresses, sizeof(addresses));
    memcpy(rom + 40, symbols, sizeof(symbols));
    memcpy(rom + 72, "funcsrc/%n.c", 12);
    sSymbolReadFailed = piBusy = reset = 0;
}
int main(void) {
    populate();
    symtable_header_t header = symt_open();
    assert(header.head[0] == 'S');
    char text[64];
    assert(strcmp(parse_map(text, sizeof(text), 0x80000104, 0), "func") == 0);
    symtable_info_t info = get_symbol_info(0x80000104);
    assert(info.line == 123 && strcmp(info.file, "src/%n.c") == 0);
    assert(get_start_of_func(0x80000104) == 0x80000100);
    int index;
    assert(symt_addrtab_search(&header, 0x80000000, &index) == 0 && index == -1);
    assert(strcmp(parse_map(text, sizeof(text), 0x80000000, 0), "???") == 0);
    assert(get_start_of_func(0x80000000) == 0x80000000);
    char shortName[2];
    symt_string(&header, 1, 3, shortName, sizeof(shortName));
    assert(strcmp(shortName, "u") == 0);
    int before = reads;
    symt_string(&header, 11, 3, shortName, sizeof(shortName));
    assert(reads == before && shortName[0] == 0);
    assert(symt_addrtab_entry(&header, -1) == 0 && reads == before);
    // Missing, empty and corrupt metadata must use the numeric/unknown fallback.
    rom[0] = 0;
    assert(get_symbol_info(0x80000104).line == -1);
    populate();
    memset(rom + offsetof(symtable_header_t, addrtab_size), 0, sizeof(u32));
    assert(symt_open().head[0] == 0);
    populate();
    memset(rom + offsetof(symtable_header_t, strtab_size), 0xFF, sizeof(u32));
    assert(symt_open().head[0] == 0);
    populate();
    piBusy = 1;
    before = reads;
    assert(symt_open().head[0] == 0 && reads == before && !masked);
    assert(count < 1000000 && sSymbolReadFailed);
    piBusy = 0;
    assert(get_symbol_info(0x80000104).line == -1 && reads == before);
    populate();
    reset = 1;
    before = reads;
    assert(symt_open().head[0] == 0 && reads == before && !masked);
    // A table without any function-start marker cannot walk before entry zero.
    populate();
    u32 address = 0x80000100;
    memcpy(rom + 32, &address, sizeof(address));
    assert(strcmp(parse_map(text, sizeof(text), 0x80000104, 0), "???") == 0);
    return 0;
}
''')


if __name__ == "__main__":
    unittest.main()
