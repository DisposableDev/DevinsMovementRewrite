#include <ultra64.h>
#include <PR/os_internal_error.h>
#include <PR/os_internal_thread.h>
#include <PR/os_system.h>
#include <stdarg.h>
#include <string.h>
#include "config/config_debug.h"
#include "buffers/framebuffers.h"
#include "types.h"
#include "game/puppyprint.h"
#include "audio/external.h"
#include "farcall.h"
#include "game/game_init.h"
#include "game/main.h"
#include "game/debug.h"
#include "game/emutest.h"
#include "game/rumble_init.h"
#include "game/printf.h"

#include "crash_screen.h"
#include "map_parser.h"
#include "disasm.h"
#include "assert.h"
#include "stacktrace.h"

#include "sm64.h"

extern u16 sRenderedFramebuffer;
extern struct SequenceQueueItem sBackgroundMusicQueue[6];
extern void audio_signal_game_loop_tick(void);
extern void stop_sounds_in_continuous_banks(void);
extern void read_controller_inputs(s32 threadID);
extern char *strstr(char *, char *);

static char *crashPageNames[] = {
    [CRASH_SCREEN_PAGE_ASSERTS] = "(Assert)",
    [CRASH_SCREEN_PAGE_SIMPLE] = "(Overview)",
    [CRASH_SCREEN_PAGE_CONTEXT] = "(Context)",
    [CRASH_SCREEN_PAGE_STACKTRACE] = "(Stack Trace)",
#ifdef PUPPYPRINT_DEBUG
    [CRASH_SCREEN_PAGE_LOG] = "(Log)",
#endif
    [CRASH_SCREEN_PAGE_DISASM] = "(Disassembly)",
};

static u8 sCrashScreenCharToGlyph[128] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
};

static u32 sCrashScreenFont[CRASH_SCREEN_GLYPH_HEIGHT * CRASH_SCREEN_FONT_ROWS * 2 + 1] = {
    #include "textures/crash_custom/crash_screen_font.ia1.inc.c"
};

static u8 crashPage = CRASH_SCREEN_PAGE_SIMPLE;
static u8 updateBuffer = TRUE;
static s32 sAssertScrollPixels;
static s32 sAssertScrollMaxPixels;
static s32 sCrashScreenClipTop;
static s32 sCrashScreenClipBottom = SCREEN_HEIGHT;
static u16 sCrashScreenPageButtons;
static OSMesgQueue sCrashResetMesgQueue;
static s32 sCrashResetRedirected;
static OSMesg sCrashResetMesg;

static u8 most_recent_framebuffer = 0;
static u8 stale_framebuffers[2] = {0, 0};
static u8 crash_screen_cfb_index = 0;

static char crashScreenBuf[0x200];

/**
 * Verbose descriptions for exception causes.
 */
char *gCauseDesc[19] = {
    [EXC_INT >> 2] = "Interrupt",
    [EXC_MOD >> 2] = "TLB modification",
    [EXC_RMISS >> 2] = "TLB exception on load",
    [EXC_WMISS >> 2] = "TLB exception on store",
    [EXC_RADE >> 2] = "Address error on load",
    [EXC_WADE >> 2] = "Address error on store",
    [EXC_IBE >> 2] = "Bus error on inst.",
    [EXC_DBE >> 2] = "Bus error on data",
    [EXC_SYSCALL >> 2] = "Failed Assert: See Assert Page",
    [EXC_BREAK >> 2] = "Breakpoint exception",
    [EXC_II >> 2] = "Reserved instruction",
    [EXC_CPU >> 2] = "Coprocessor unusable",
    [EXC_OV >> 2] = "Arithmetic overflow",
    [EXC_TRAP >> 2] = "Trap exception",
    [EXC_VCEI >> 2] = "Virtual coherency on inst.",
    [EXC_FPE >> 2] = "Floating point exception",
    // These two exceptions are not enumerated like the ones above.
    //  They take values 23 and 31, respectively, so here they're just placed
    //   after the ones that do enumerate nicely.
    /*EXC_WATCH       */ "Watchpoint exception",
    /*EXC_VCED        */ "Virtual coherency on data",
    "Unknown exception",
};

char *gFpcsrDesc[6] = {
    "Unimplemented op", "Invalid operation", "Division by zero",
    "Overflow",         "Underflow",         "Inexact operation",
};

static u32 sProgramPosition = 0;
static u32 sCrashInstructionAddress;
#if defined(DEBUG_EXPORT_SYMBOLS) && defined(DEBUG_FULL_STACK_TRACE)
static u32 sCrashScreenStackTraceCount = 0;
#endif // defined(DEBUG_EXPORT_SYMBOLS) && defined(DEBUG_FULL_STACK_TRACE)

static u16 gCrashScreenTextColor = 0xFFFF;
static s32 sCrashScreenPrintRow_Pixels = 0;
static u32 sCrashScreenPrintLnHeight_Pixels = CRASH_SCREEN_GLYPH_HEIGHT;

struct FaultInfo gCrashScreen;

/**
 * Splits a path string by the containing folder and the name of the file itself.
 */
char *crash_screen_ellide_string(char *str, u32 truncateLength) {
    u32 length = strlen(str);

    if (length <= truncateLength) {
        return str;
    }
    if (truncateLength < 5) {
        str[truncateLength] = '\0';
        return str;
    }
    // Keep the suffix and include the entire marker in the requested width.
    for (u32 i = 0; i < truncateLength - 5; i++) {
        str[5 + i] = str[length - (truncateLength - 5) + i];
    }
    memcpy(str, "(...)", 5);
    str[truncateLength] = '\0';
    return str;
}

static void crash_screen_update_framebuffer_indices(void) {
    most_recent_framebuffer = sRenderedFramebuffer;
    stale_framebuffers[0] = (sRenderedFramebuffer + 1) % 3;
    stale_framebuffers[1] = (sRenderedFramebuffer + 2) % 3;
}

static void set_text_color(u32 r, u32 g, u32 b) {
    gCrashScreenTextColor = GPACK_RGBA5551(r, g, b, 255);
}

static void reset_text_color(void) {
    gCrashScreenTextColor = 0xFFFF;
}

/**
 * Sets the screen row, in pixels, at which the next print
 *  will be placed on the Y axis.
 */
static void crash_screen_set_print_top(s32 row) {
    sCrashScreenPrintRow_Pixels = row;
}

static void crash_screen_set_println_height(u32 height_pixels) {
    sCrashScreenPrintLnHeight_Pixels = height_pixels;
}

static void crash_screen_reset_println_height(void) {
    sCrashScreenPrintLnHeight_Pixels = CRASH_SCREEN_GLYPH_HEIGHT;
}

#define MASK_5BIT 0x1F
#define R_SHIFT 11
#define G_SHIFT 6
#define B_SHIFT 1
#define A_SHIFT 0
void crash_screen_darken_framebuffer(const f32 opacity) {
    f32 mult = 1.0f - opacity;
    if (mult < 0.0f) {
        mult = 0.0f;
    } else if (mult >= 1.0f) {
        return;
    }

    u32 r, g, b;
    for (u16 *ptr = gCrashScreen.framebuffer; ptr < gCrashScreen.framebuffer + ARRAY_COUNT(gFramebuffers[0]); ptr++) {
        r = (((*ptr) >> R_SHIFT) & MASK_5BIT) * mult;
        g = (((*ptr) >> G_SHIFT) & MASK_5BIT) * mult;
        b = (((*ptr) >> B_SHIFT) & MASK_5BIT) * mult;

        *ptr = (r << R_SHIFT) | (g << G_SHIFT) | (b << B_SHIFT) | (1 << A_SHIFT);
    }
}
#undef A_SHIFT
#undef B_SHIFT
#undef G_SHIFT
#undef R_SHIFT
#undef MASK_5BIT

void crash_screen_draw_glyph(s32 x, s32 y, s32 glyph) {
    const u32 *data;
    u16 *ptr;
    u32 bit;
    u32 rowMask;
    s32 i, j;
    u32 startOfGlyphData = 0;

    if (glyph > 0x7F || x < 0 || y < sCrashScreenClipTop
        || x + CRASH_SCREEN_GLYPH_WIDTH > gCrashScreen.width
        || y + CRASH_SCREEN_GLYPH_HEIGHT > MIN(gCrashScreen.height, sCrashScreenClipBottom)) {
        return;
    }

    startOfGlyphData = (glyph & 0xF) * CRASH_SCREEN_GLYPH_HEIGHT * 2;
    if (glyph >= 64) {
        startOfGlyphData++;
    }

    data = &sCrashScreenFont[startOfGlyphData];

    ptr = gCrashScreen.framebuffer + gCrashScreen.width * y + x;

    u16 color = gCrashScreenTextColor;

    for (i = 0; i < CRASH_SCREEN_GLYPH_HEIGHT; i++) {
        bit = 0x80000000U >> ((glyph >> 4) * CRASH_SCREEN_GLYPH_WIDTH);
        rowMask = *data++;
        data++;

        for (j = 0; j < (CRASH_SCREEN_GLYPH_WIDTH); j++) {
            if (bit & rowMask) {
                *ptr = color;
            }
            ptr++;
            bit >>= 1;
        }
        ptr += gCrashScreen.width - (CRASH_SCREEN_GLYPH_WIDTH);
    }
    osWritebackDCacheAll();
}

typedef struct {
    char *text;
    size_t capacity;
    size_t length;
} CrashTextBuffer;

static char *write_to_buf(char *buffer, const char *data, size_t size) {
    CrashTextBuffer *output = (CrashTextBuffer *) buffer;
    size_t available = output->capacity - 1 - output->length;
    size_t count = MIN(size, available);

    memcpy(output->text + output->length, data, count);
    output->length += count;
    return buffer;
}

static s32 crash_screen_format_text(char *text, size_t capacity, const char *fmt, va_list args) {
    CrashTextBuffer output = { text, capacity, 0 };

    if (capacity == 0) {
        return 0;
    }
    _Printf(write_to_buf, (char *) &output, fmt, args);
    text[output.length] = '\0';
    return output.length;
}

void __n64Assert_format(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    crash_screen_format_text(__n64Assert_MessageBuf, sizeof(__n64Assert_MessageBuf), fmt, args);
    va_end(args);
}

// Raw assertion text can exceed the formatting buffer; render it without copying.
static s32 crash_screen_print_wrapped_text(s32 x, s32 y, s32 xNewline, const char *text, s32 size) {
    s32 xOffset = x;
    s32 numNewlines = 0;

    while (size-- > 0 && *text != '\0') {
        if (xOffset >= SCREEN_WIDTH - (xNewline + CRASH_SCREEN_X_KERNING)) {
            y += 10;
            xOffset = xNewline;
            numNewlines++;
        }
        if (*text == '\n') {
            y += 10;
            xOffset = x;
            numNewlines++;
        } else if (*text == '\t') {
            xOffset += CRASH_SCREEN_GLYPH_WIDTH * (CRASH_SCREEN_TAB_WIDTH_CHARS - 1);
        } else {
            u32 glyph = sCrashScreenCharToGlyph[(u8) *text & 0x7f];

            if (glyph != 0xff) {
                crash_screen_draw_glyph(xOffset, y, glyph);
            }
            xOffset += CRASH_SCREEN_X_KERNING;
        }
        text++;
    }
    return numNewlines;
}

/**
 * crash_screen_print, but automatically places a newline
 *  if text is xNewline units away from the right edge of the screen.
 *  Also respects newline characters in the evaluated string.
 * 
 * Returns the number of newlines that were in the string, or created.
 */
int crash_screen_print_with_newlines(s32 x, s32 y, const s32 xNewline, const char *fmt, ...) {
    s32 size;
    s32 numNewlines;

    va_list args;
    va_start(args, fmt);

    size = crash_screen_format_text(crashScreenBuf, sizeof(crashScreenBuf), fmt, args);

    numNewlines = crash_screen_print_wrapped_text(x, y, xNewline, crashScreenBuf, size);

    va_end(args);

    return numNewlines;
}

void crash_screen_print(s32 x, s32 y, const char *fmt, ...) {
    char *ptr;
    u32 glyph;
    s32 size;

    va_list args;
    va_start(args, fmt);

    size = crash_screen_format_text(crashScreenBuf, sizeof(crashScreenBuf), fmt, args);

    if (size > 0) {
        ptr = crashScreenBuf;

        while (*ptr && size-- > 0) {
            if (*ptr == '\t') {
                ptr++;
                x += CRASH_SCREEN_GLYPH_WIDTH * (CRASH_SCREEN_TAB_WIDTH_CHARS - 1);
                continue;
            }
            glyph = sCrashScreenCharToGlyph[*ptr & 0x7f];

            if (glyph != 0xff) {
                crash_screen_draw_glyph(x, y, glyph);
            }

            ptr++;
            x += CRASH_SCREEN_X_KERNING;
        }
    }

    va_end(args);
}

void crash_screen_println(const char *fmt, ...) {
    char *ptr;
    u32 glyph;
    s32 size;
    s32 x = CRASH_SCREEN_LEFT_MARGIN;
    s32 y = sCrashScreenPrintRow_Pixels;

    va_list args;
    va_start(args, fmt);

    size = crash_screen_format_text(crashScreenBuf, sizeof(crashScreenBuf), fmt, args);

    if (size > 0) {
        ptr = crashScreenBuf;

        while (*ptr && size-- > 0) {
            if (*ptr == '\t') {
                ptr++;
                x += CRASH_SCREEN_GLYPH_WIDTH * (CRASH_SCREEN_TAB_WIDTH_CHARS - 1);
                continue;
            }

            glyph = sCrashScreenCharToGlyph[*ptr & 0x7f];

            if (glyph != 0xff) {
                crash_screen_draw_glyph(x, y, glyph);
            }

            ptr++;
            x += CRASH_SCREEN_X_KERNING;
        }
    }

    va_end(args);
    sCrashScreenPrintRow_Pixels += sCrashScreenPrintLnHeight_Pixels;
}

s32 crash_screen_sleep(s32 ms) {
    u32 start = osGetCount();
    u32 cycles = OS_USEC_TO_CYCLES(ms * 1000);

    do {
        if (crash_screen_reset_pending()) {
            return FALSE;
        }
    } while ((u32) (osGetCount() - start) < cycles);
    return TRUE;
}

void crash_screen_print_float_reg(s32 x, s32 y, s32 regNum, void *addr) {
    u32 bits = *(u32 *) addr;
    s32 exponent = ((bits & 0x7f800000U) >> 0x17) - 0x7F;

    if ((exponent >= -0x7E && exponent <= 0x7F) || bits == 0x0) {
        crash_screen_print(x, y, "F%02d:%.3e",  regNum, *(f32 *) addr);
    } else {
        crash_screen_print(x, y, "F%02d:%08XD", regNum, *(u32 *) addr);
    }
}

void crash_screen_print_fpcsr(u32 fpcsr) {
    s32 i;
    u32 bit = BIT(17);

    crash_screen_print(CRASH_SCREEN_LEFT_MARGIN + 90, 220, "FPCSR:%08X", fpcsr);
    for (i = 0; i < ARRAY_COUNT(gFpcsrDesc); i++) {
        if (fpcsr & bit) {
            crash_screen_print(192, 220, "(%s)", gFpcsrDesc[i]);
            return;
        }
        bit >>= 1;
    }
}

void draw_crash_overview(OSThread *thread, s32 cause) {
    __OSThreadContext *tc = &thread->context;

    set_text_color(0xFF, 0, 0);
    crash_screen_println("Game crashed!");
    reset_text_color();

    set_text_color(241, 196, 15);
    crash_screen_println("Thread:");
    crash_screen_println("Cause:");
    reset_text_color();
    sCrashScreenPrintRow_Pixels -= (2 * CRASH_SCREEN_GLYPH_HEIGHT);
    crash_screen_println("        %d", thread->id);
    crash_screen_println("       %s", gCauseDesc[cause]);

#ifdef DEBUG_EXPORT_SYMBOLS
    int numNewlines = 0;

    symtable_info_t info = get_symbol_info(tc->pc);

    if (info.line != -1) {
        char file_line[CRASH_SCREEN_MAX_PATH];

        set_text_color(241, 196, 15);
#ifdef DEBUG_EXPORT_ALL_LINES
        sprintf(file_line, "      %s:%d", info.file, info.line);
#else  // DEBUG_EXPORT_ALL_LINES
        sprintf(file_line, "      %s", info.file);
#endif // DEBUG_EXPORT_ALL_LINES
        sCrashScreenPrintRow_Pixels += 5;
        crash_screen_println("File: ");
        reset_text_color();

        numNewlines = crash_screen_print_with_newlines(
            CRASH_SCREEN_LEFT_MARGIN, sCrashScreenPrintRow_Pixels - CRASH_SCREEN_GLYPH_HEIGHT,
            CRASH_SCREEN_LEFT_MARGIN, "%s", file_line);
        sCrashScreenPrintRow_Pixels += (numNewlines * CRASH_SCREEN_GLYPH_HEIGHT);
    }
#endif // DEBUG_EXPORT_SYMBOLS

#if defined(DEBUG_EXPORT_SYMBOLS) && defined(DEBUG_FULL_STACK_TRACE)
    if (stackTraceGenerated) {
        set_text_color(241, 196, 15);
        crash_screen_println("Stack Trace: ");
        reset_text_color();

        // print current func
        crash_screen_println("%08X: %s", tc->pc, info.func);
        // print last func
        u32 ret_addr = tc->ra;
        symtable_info_t ra_info = get_symbol_info(ret_addr);
        crash_screen_println("%08X: %s:%d", ret_addr, ra_info.func, ra_info.line);
        // print up to 3 more
        for (u32 i = 0; i < MIN(3, sCrashScreenStackTraceCount); i++) {
            crash_screen_println("%s", get_stack_entry(i));
        }
    }
#else // defined(DEBUG_EXPORT_SYMBOLS) && defined(DEBUG_FULL_STACK_TRACE)
    crash_screen_println("Address: 0x%08X", tc->pc);
#endif // defined(DEBUG_EXPORT_SYMBOLS) && defined(DEBUG_FULL_STACK_TRACE)
}

void draw_crash_context(OSThread *thread, s32 cause) {
#ifdef DEBUG_EXPORT_SYMBOLS
    char symbolname_scratch[64];
#endif // DEBUG_EXPORT_SYMBOLS

    __OSThreadContext *tc = &thread->context;

    crash_screen_println("Thread:%d (%s)", thread->id, gCauseDesc[cause]);
    crash_screen_set_println_height(10);
    crash_screen_println("PC:%08XH   SR:%08XH   VA:%08XH", tc->pc, tc->sr, tc->badvaddr);
    osWritebackDCacheAll();
#ifdef DEBUG_EXPORT_SYMBOLS
    char *fname = parse_map(symbolname_scratch, sizeof(symbolname_scratch), tc->pc, TRUE);
    crash_screen_println("Crash at: %s", fname == NULL ? "Unknown" : fname);
#endif // DEBUG_EXPORT_SYMBOLS
    crash_screen_println("AT:%08XH   V0:%08XH   V1:%08XH", (u32) tc->at, (u32) tc->v0, (u32) tc->v1);
    crash_screen_println("A0:%08XH   A1:%08XH   A2:%08XH", (u32) tc->a0, (u32) tc->a1, (u32) tc->a2);
    crash_screen_println("A3:%08XH   T0:%08XH   T1:%08XH", (u32) tc->a3, (u32) tc->t0, (u32) tc->t1);
    crash_screen_println("T2:%08XH   T3:%08XH   T4:%08XH", (u32) tc->t2, (u32) tc->t3, (u32) tc->t4);
    crash_screen_println("T5:%08XH   T6:%08XH   T7:%08XH", (u32) tc->t5, (u32) tc->t6, (u32) tc->t7);
    crash_screen_println("S0:%08XH   S1:%08XH   S2:%08XH", (u32) tc->s0, (u32) tc->s1, (u32) tc->s2);
    crash_screen_println("S3:%08XH   S4:%08XH   S5:%08XH", (u32) tc->s3, (u32) tc->s4, (u32) tc->s5);
    crash_screen_println("S6:%08XH   S7:%08XH   T8:%08XH", (u32) tc->s6, (u32) tc->s7, (u32) tc->t8);
    crash_screen_println("T9:%08XH   GP:%08XH   SP:%08XH", (u32) tc->t9, (u32) tc->gp, (u32) tc->sp);
    crash_screen_println("S8:%08XH   RA:%08XH",            (u32) tc->s8, (u32) tc->ra);
#ifdef DEBUG_EXPORT_SYMBOLS
    fname = parse_map(symbolname_scratch, sizeof(symbolname_scratch), tc->ra, TRUE);
    crash_screen_println("RA at: %s", fname == NULL ? "Unknown" : fname);
#endif // DEBUG_EXPORT_SYMBOLS

    crash_screen_print_fpcsr(tc->fpcsr);

    osWritebackDCacheAll();
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN +   0, 170,  0, &tc->fp0.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN +  90, 170,  2, &tc->fp2.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN + 180, 170,  4, &tc->fp4.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN +   0, 180,  6, &tc->fp6.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN +  90, 180,  8, &tc->fp8.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN + 180, 180, 10, &tc->fp10.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN +   0, 190, 12, &tc->fp12.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN +  90, 190, 14, &tc->fp14.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN + 180, 190, 16, &tc->fp16.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN +   0, 200, 18, &tc->fp18.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN +  90, 200, 20, &tc->fp20.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN + 180, 200, 22, &tc->fp22.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN +   0, 210, 24, &tc->fp24.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN +  90, 210, 26, &tc->fp26.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN + 180, 210, 28, &tc->fp28.f.f_even);
    crash_screen_print_float_reg(CRASH_SCREEN_LEFT_MARGIN +   0, 220, 30, &tc->fp30.f.f_even);

    crash_screen_reset_println_height();
}


#ifdef PUPPYPRINT_DEBUG
void draw_crash_log(void) {
    s32 i;
    osWritebackDCacheAll();
    for (i = 0; i < LOG_BUFFER_SIZE; i++) {
        crash_screen_println("%s", consoleLogTable[LOG_BUFFER_SIZE - 1 - i]);
    }
}
#endif

void draw_stacktrace(OSThread *thread, UNUSED s32 cause) {
#ifdef DEBUG_EXPORT_SYMBOLS
    char symbolname_scratch[64];
#endif // DEBUG_EXPORT_SYMBOLS

    __OSThreadContext *tc = &thread->context;

    crash_screen_println("Stack Trace from %08X:", (u32) tc->sp);

#if defined(DEBUG_EXPORT_SYMBOLS) && defined(DEBUG_FULL_STACK_TRACE)
    // Current Func (EPC)
    crash_screen_println("%08X (%s)", tc->pc, parse_map(symbolname_scratch, sizeof(symbolname_scratch), tc->pc, TRUE));

    // Previous Func (RA)
    u32 ra = tc->ra;
    symtable_info_t ra_info = get_symbol_info(ra);

    crash_screen_println("%08X (%s:%d)", ra, ra_info.func, ra_info.line);

    osWritebackDCacheAll();

    for (u32 i = 0; i < sCrashScreenStackTraceCount; i++) {
        crash_screen_println("%s", get_stack_entry(i));
    }
#else // defined(DEBUG_EXPORT_SYMBOLS) && defined(DEBUG_FULL_STACK_TRACE)
    // simple stack trace
    u32 sp = tc->sp;

    for (int i = 0; i < STACK_LINE_COUNT; i++) {
        for (int column = 0; column < 2; column++) {
            int index = i + column * STACK_LINE_COUNT;
            u32 addr = sp + index * sizeof(u32);
            s32 x = column == 0 ? CRASH_SCREEN_LEFT_MARGIN : 120;

            if (addr >= sp && is_ram_word(addr)) {
                crash_screen_print(x, 55 + i * 10, "%3d: %08X", index, *(u32 *) addr);
            } else {
                crash_screen_print(x, 55 + i * 10, "%3d: --------", index);
            }
        }
    }
#endif // defined(DEBUG_EXPORT_SYMBOLS) && defined(DEBUG_FULL_STACK_TRACE)
}

void draw_disasm(OSThread *thread) {
    __OSThreadContext *tc = &thread->context;
    if (sProgramPosition == 0) {
        sCrashInstructionAddress = tc->pc;
        if (!is_text_addr(sCrashInstructionAddress)) {
            sCrashInstructionAddress = tc->ra + CALLSITE_OFFSET;
        }
        if (!is_text_addr(sCrashInstructionAddress)) {
            crash_screen_println("Corrupted crash location!");
            crash_screen_println("Disassembly will not work!");
            crash_screen_println("PC: %08X", tc->pc);
            crash_screen_println("RA: %08X", tc->ra);
            osWritebackDCacheAll();
            return;
        }
        sProgramPosition = sCrashInstructionAddress;
        if (is_text_addr(sCrashInstructionAddress - 36)) {
            sProgramPosition -= 36;
        }
    }
    crash_screen_println("Program Counter: %08X", sProgramPosition);
    osWritebackDCacheAll();

    for (int i = 0; i < 16; i++) {
        u32 addr = (sProgramPosition + (i * 4));

        if (!is_text_addr(addr)) {
            crash_screen_println("    %08X: unavailable", addr);
            continue;
        }
        char *disasm = insn_disasm((InsnData *) addr);

        if (disasm[0] == 0) {
            crash_screen_println("    %08X", addr);
        } else {
#ifdef DEBUG_EXPORT_SYMBOLS
            symtable_info_t info = get_symbol_info(addr);

            if (info.func_offset == 0 && info.distance == 0) {
                set_text_color(239, 196, 15);
                crash_screen_println("<%s:>", info.func);
                reset_text_color();
            }
#ifndef DEBUG_EXPORT_ALL_LINES
            // catch `jal` and `jalr` callsites
            if (disasm[0] == 'j' && disasm[1] == 'a') {
#endif // DEBUG_EXPORT_ALL_LINES
                if (info.line != -1) {
                    set_text_color(200, 200, 200);
                    crash_screen_println("%d:", info.line);

                    reset_text_color();
                    sCrashScreenPrintRow_Pixels -= sCrashScreenPrintLnHeight_Pixels;
                    if (addr == sCrashInstructionAddress) {
                        set_text_color(255, 0, 0);
                    } else {
                        reset_text_color();
                    }
                    crash_screen_println("    %s", disasm);
                }
#ifndef DEBUG_EXPORT_ALL_LINES
            }
#endif // DEBUG_EXPORT_ALL_LINES
            else {
#endif // DEBUG_EXPORT_SYMBOLS
                if (addr == sCrashInstructionAddress) {
                    set_text_color(255, 0, 0);
                } else {
                    reset_text_color();
                }
                crash_screen_println("    %s", disasm);
#ifdef DEBUG_EXPORT_SYMBOLS
            }
#endif // DEBUG_EXPORT_SYMBOLS
        }
    }

    reset_text_color();
    osWritebackDCacheAll();
}

void draw_assert(OSThread *thread) {
    __OSThreadContext *tc = &thread->context;
    sCrashScreenClipTop = 35;
    sCrashScreenClipBottom = gCrashScreen.height - CRASH_SCREEN_GLYPH_HEIGHT;
    crash_screen_set_print_top(35 - sAssertScrollPixels);

    set_text_color(0xFF, 0, 0);
    crash_screen_println("Assert Failed!");
    reset_text_color();


    if (__n64Assert_Filename != NULL) {
        set_text_color(241, 196, 15);
        crash_screen_println("File/Line:");
        reset_text_color();
        int numNewlines = crash_screen_print_wrapped_text(
            CRASH_SCREEN_LEFT_MARGIN + 11 * CRASH_SCREEN_X_KERNING,
            sCrashScreenPrintRow_Pixels - CRASH_SCREEN_GLYPH_HEIGHT, CRASH_SCREEN_LEFT_MARGIN,
            __n64Assert_Filename, strlen(__n64Assert_Filename));

        sCrashScreenPrintRow_Pixels += numNewlines * CRASH_SCREEN_GLYPH_HEIGHT;
        crash_screen_println("Line: %u", __n64Assert_LineNum);

        // Print the assert condition that failed.
        set_text_color(241, 196, 15);
        crash_screen_println("Condition:");
        reset_text_color();
        numNewlines = crash_screen_print_wrapped_text(
            CRASH_SCREEN_LEFT_MARGIN + 11 * CRASH_SCREEN_X_KERNING,
            sCrashScreenPrintRow_Pixels - CRASH_SCREEN_GLYPH_HEIGHT, CRASH_SCREEN_LEFT_MARGIN,
            __n64Assert_Condition, strlen(__n64Assert_Condition));

        sCrashScreenPrintRow_Pixels += (numNewlines * 10);

        // Print the message, if assertf/aggressf/errorf were used.
        numNewlines = 0;
        if (__n64Assert_MessageBuf[0] != 0) {
            set_text_color(241, 196, 15);
            crash_screen_println("Message:");
            reset_text_color();
            numNewlines = crash_screen_print_wrapped_text(
                CRASH_SCREEN_LEFT_MARGIN + 9 * CRASH_SCREEN_X_KERNING,
                sCrashScreenPrintRow_Pixels - CRASH_SCREEN_GLYPH_HEIGHT, CRASH_SCREEN_LEFT_MARGIN,
                __n64Assert_MessageBuf, strlen(__n64Assert_MessageBuf));
        }
        sCrashScreenPrintRow_Pixels += (numNewlines * 10);
#if defined(DEBUG_EXPORT_SYMBOLS) && defined(DEBUG_FULL_STACK_TRACE)
        if (stackTraceGenerated) {
            set_text_color(241, 196, 15);
            crash_screen_println("Stack Trace:");
            reset_text_color();

            // Print last func (we know the current func is __n64Assert)
            u32 ret_addr = tc->ra;
            symtable_info_t ra_info = get_symbol_info(ret_addr);
            crash_screen_println("%08X: %s:%d", ret_addr, ra_info.func, ra_info.line);
            // Print up to 3 more
            for (u32 i = 0; i < MIN(3, sCrashScreenStackTraceCount); i++) {
                crash_screen_println("%s", get_stack_entry(i));
            }
        }
#else // defined(DEBUG_EXPORT_SYMBOLS) && defined(DEBUG_FULL_STACK_TRACE)
        // Print address of last func (we know the current func is __n64Assert)
        u32 ret_addr = tc->ra;
        set_text_color(241, 196, 15);
        crash_screen_println("Called From:");
        reset_text_color();
        crash_screen_println("\t0x%08X", ret_addr);
#endif // defined(DEBUG_EXPORT_SYMBOLS) && defined(DEBUG_FULL_STACK_TRACE)
    } else {
        crash_screen_println("No failed assert to report.");
    }

    sAssertScrollMaxPixels = MAX(0, sCrashScreenPrintRow_Pixels + sAssertScrollPixels
                                        - (gCrashScreen.height - CRASH_SCREEN_GLYPH_HEIGHT));
    sCrashScreenClipTop = 0;
    sCrashScreenClipBottom = gCrashScreen.height;
    reset_text_color();
    if (sAssertScrollMaxPixels > 0) {
        crash_screen_print(CRASH_SCREEN_LEFT_MARGIN, gCrashScreen.height - CRASH_SCREEN_GLYPH_HEIGHT,
                           "C-Up/Down: Scroll  Z: Faster");
    }

    osWritebackDCacheAll();
}

static void crash_screen_scroll_assert(void) {
    s32 scroll = sAssertScrollPixels;
    s32 step = (gPlayer1Controller->buttonDown & Z_TRIG) ? 40 : 10;

    if (gPlayer1Controller->buttonDown & D_CBUTTONS) {
        scroll += step;
    }
    if (gPlayer1Controller->buttonDown & U_CBUTTONS) {
        scroll -= step;
    }
    scroll = MAX(0, MIN(scroll, sAssertScrollMaxPixels));
    if (scroll != sAssertScrollPixels) {
        if (!(gPlayer1Controller->buttonDown & Z_TRIG)) {
            crash_screen_sleep(30);
        }
        sAssertScrollPixels = scroll;
        updateBuffer = TRUE;
    }
}

static void crash_screen_change_page(s32 assertAvailable) {
    u16 buttons = gPlayer1Controller->buttonDown & (L_TRIG | R_TRIG);
    u16 pressed = buttons & ~sCrashScreenPageButtons;
    s32 direction;
    s32 page;

    sCrashScreenPageButtons = buttons;
    if (pressed == L_TRIG) {
        direction = -1;
    } else if (pressed == R_TRIG) {
        direction = 1;
    } else {
        return;
    }
    page = crashPage + direction;
    if (page < 0) {
        page = CRASH_SCREEN_PAGE_COUNT - 1;
    } else if (page >= CRASH_SCREEN_PAGE_COUNT) {
        page = 0;
    }
    if (page == CRASH_SCREEN_PAGE_ASSERTS && !assertAvailable) {
        page = direction > 0 ? CRASH_SCREEN_PAGE_SIMPLE : CRASH_SCREEN_PAGE_COUNT - 1;
    }
    crashPage = page;
    updateBuffer = TRUE;
}

static s32 crash_screen_page_number(s32 page, s32 assertAvailable) {
    return page + 1 - !assertAvailable;
}

void draw_crash_screen(OSThread *thread) {
    __OSThreadContext *tc = &thread->context;

    s32 assertAvailable = (tc->cause & 0x7C) == EXC_SYSCALL;
    s32 cause = ((tc->cause >> 2) & 0x1F);
    if (cause == 23) { // EXC_WATCH
        cause = 16;
    }
    if (cause == 31) { // EXC_VCED
        cause = 17;
    }

    if (cause >= ARRAY_COUNT(gCauseDesc)) {
        cause = ARRAY_COUNT(gCauseDesc) - 1;
    }

    crash_screen_change_page(assertAvailable);

    if (crashPage == CRASH_SCREEN_PAGE_DISASM) {
        u32 sNewProgramPosition = sProgramPosition;
        if (gPlayer1Controller->buttonDown & D_CBUTTONS) {
            sNewProgramPosition += 4;
        }
        if (gPlayer1Controller->buttonDown & U_CBUTTONS) {
            sNewProgramPosition -= 4;
        }

        if (is_text_addr(sNewProgramPosition) && (sNewProgramPosition != sProgramPosition)) {
            // Hold Z to speed up scrolling
            if (!(gPlayer1Controller->buttonDown & Z_TRIG)) {
                crash_screen_sleep(30);
            }
            sProgramPosition = sNewProgramPosition;
            updateBuffer = TRUE;
        }
    }

    if (crashPage == CRASH_SCREEN_PAGE_ASSERTS) {
        crash_screen_scroll_assert();
    }
    if (updateBuffer) {
        memcpy(
            gCrashScreen.framebuffer,
            gFramebuffers[most_recent_framebuffer],
            sizeof(gFramebuffers[most_recent_framebuffer])
        );
        osWritebackDCacheAll();

        crash_screen_set_print_top(CRASH_SCREEN_TOP_MARGIN);
        crash_screen_println("Page:%02d/%02d %-16s L: Left   R: Right",
                             crash_screen_page_number(crashPage, assertAvailable),
                             CRASH_SCREEN_PAGE_COUNT - !assertAvailable, crashPageNames[crashPage]);
        switch (crashPage) {
            case CRASH_SCREEN_PAGE_SIMPLE:     draw_crash_overview(thread, cause); break;
            case CRASH_SCREEN_PAGE_CONTEXT:    draw_crash_context(thread, cause); break;
            case CRASH_SCREEN_PAGE_STACKTRACE: draw_stacktrace(thread, cause); break;
#ifdef PUPPYPRINT_DEBUG
            case CRASH_SCREEN_PAGE_LOG:        draw_crash_log(); break;
#endif
            case CRASH_SCREEN_PAGE_DISASM:     draw_disasm(thread); break;
            case CRASH_SCREEN_PAGE_ASSERTS:    draw_assert(thread); break;
            default: break;
        }

        osWritebackDCacheAll();
        osViBlack(FALSE);
        osViSwapBuffer(gCrashScreen.framebuffer);
        crash_screen_cfb_index = (crash_screen_cfb_index + 1) % CRASH_SCREEN_NUM_CFBs;
        gCrashScreen.framebuffer = (RGBA16 *) gFramebuffers[stale_framebuffers[crash_screen_cfb_index]];
        updateBuffer = FALSE;
    }
}

OSThread *get_crashed_thread(OSMesg event) {
    OSThread *thread;
    u16 flag = event == (OSMesg) 1 ? OS_FLAG_CPU_BREAK : OS_FLAG_FAULT;

    if (flag == OS_FLAG_FAULT) {
        thread = __osGetCurrFaultedThread();
        if (thread != NULL && thread != &gCrashScreen.thread && (thread->flags & flag)) {
            return thread;
        }
    }
    // Breakpoints do not set the SDK's faulted-thread pointer; the active list ends at NULL.
    for (thread = __osGetActiveQueue(); thread != NULL; thread = thread->tlnext) {
        if (thread != &gCrashScreen.thread && (thread->flags & flag)) {
            return thread;
        }
    }
    return NULL;
}

s32 crash_screen_reset_pending(void) {
    // Keep watching the scheduler even after another thread's crash page opens.
    if (!sCrashResetRedirected && (gMainThread.flags & (OS_FLAG_FAULT | OS_FLAG_CPU_BREAK))) {
        osStopThread(&gMainThread);
        redirect_pre_nmi_to_crash(&sCrashResetMesgQueue);
        sCrashResetRedirected = TRUE;
    }
    if ((osRecvMesg(&sCrashResetMesgQueue, NULL, OS_MESG_NOBLOCK) == 0 || reset_pending())
        && gResetTimer == 0) {
        gResetTimer = 1;
        gNmiResetBarsTimer = 0;
        stop_rumble_for_reset();
    }
    return gResetTimer != 0;
}

static void crash_screen_read_controller(void) {
    s32 result;

    if (!gControllerBits || !try_acquire_rumble_pak()) {
        return;
    }
    if (crash_screen_reset_pending()) {
        release_rumble_pak_control();
        return;
    }
    result = osContReadDataExForCrash(&gSIEventMesgQueue, crash_screen_reset_pending);
    if (result == 0) {
        read_controller_inputs(THREAD_2_CRASH_SCREEN);
    } else if (result != CRASH_CONTROLLER_DMA_PENDING) {
        release_rumble_pak_control();
    }
    // An unfinished DMA retains both locks; no other caller may reuse its PIF buffer.
}

void thread2_crash_screen(UNUSED void *arg) {
    OSMesg mesg;
    OSThread *thread = NULL;

    osSetEventMesg(OS_EVENT_CPU_BREAK, &gCrashScreen.mesgQueue, (OSMesg) 1);
    osSetEventMesg(OS_EVENT_FAULT,     &gCrashScreen.mesgQueue, (OSMesg) 2);
    while (TRUE) {
        // Reset cleanup must continue even if a peripheral stopped delivering completions.
        if (crash_screen_reset_pending()) {
            // This also retries RSP cleanup when the main scheduler is the faulted thread.
            if (reset_rsp_for_pre_nmi()) {
                osStopThread(NULL);
            }
            continue;
        }
        if (thread == NULL) {
            osRecvMesg(&gCrashScreen.mesgQueue, &mesg, OS_MESG_BLOCK);
            thread = get_crashed_thread(mesg);

            if (thread) {
                if (mesg == (OSMesg) 1) {
                    osStopThread(thread);
                }
                osSetThreadPri(NULL, 15);
                if (crash_screen_reset_pending()) {
                    continue;
                }
                crash_screen_update_framebuffer_indices();
                stop_sounds_in_continuous_banks();
                stop_background_music(sBackgroundMusicQueue[0].seqId);
                audio_signal_game_loop_tick();
                if (!crash_screen_sleep(200)) {
                    continue;
                }
                play_sound(SOUND_MARIO_WAAAOOOW, gGlobalSoundSource);
                audio_signal_game_loop_tick();
                if (!crash_screen_sleep(200)) {
                    continue;
                }

                // Draw a dark overlay on the last rendered frame, so we don't have to draw rectangles again
                gCrashScreen.framebuffer = (RGBA16 *) gFramebuffers[most_recent_framebuffer];
                crash_screen_darken_framebuffer((gEmulator & EMU_CONSOLE) ? 0.667f : 0.75f);

                gCrashScreen.framebuffer = (RGBA16 *) gFramebuffers[stale_framebuffers[crash_screen_cfb_index]];

#if defined(DEBUG_EXPORT_SYMBOLS) && defined(DEBUG_FULL_STACK_TRACE)
                if (stackTraceGenerated == FALSE) {
                    sCrashScreenStackTraceCount = generate_stack(thread);
                    stackTraceGenerated = TRUE;
                }
#endif // defined(DEBUG_EXPORT_SYMBOLS) && defined(DEBUG_FULL_STACK_TRACE)
                // If an assert happened, go straight to that page
                if ((thread->context.cause & 0x7C) == EXC_SYSCALL) {
                    crashPage = CRASH_SCREEN_PAGE_ASSERTS;
                }
                sCrashScreenPageButtons = gPlayer1Controller->buttonDown & (L_TRIG | R_TRIG);
                continue;
            }
        } else {
            crash_screen_read_controller();
            draw_crash_screen(thread);
        }
    }
}

void crash_screen_init(void) {
    crash_screen_update_framebuffer_indices();
    gCrashScreen.framebuffer = (RGBA16 *) gFramebuffers[stale_framebuffers[crash_screen_cfb_index]];
    gCrashScreen.width = SCREEN_WIDTH;
    gCrashScreen.height = SCREEN_HEIGHT;
    osCreateMesgQueue(&gCrashScreen.mesgQueue, &gCrashScreen.mesg, 1);
    osCreateMesgQueue(&sCrashResetMesgQueue, &sCrashResetMesg, 1);
    osCreateThread(&gCrashScreen.thread, THREAD_2_CRASH_SCREEN, thread2_crash_screen, NULL,
                   (u8 *) gCrashScreen.stack + sizeof(gCrashScreen.stack),
                   OS_PRIORITY_APPMAX
                  );
    osStartThread(&gCrashScreen.thread);
}
