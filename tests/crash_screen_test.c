#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef int32_t s32;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define TRUE 1
#define FALSE 0
#define L_TRIG 0x20
#define R_TRIG 0x10
#define Z_TRIG 0x2000
#define B_BUTTON 0x4000
#define U_CBUTTONS 8
#define D_CBUTTONS 4
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) MIN(MAX(x, lo), hi)
#define DEBUG_EXPORT_SYMBOLS
#define DEBUG_FULL_STACK_TRACE

#include "crash_definitions.inc"

typedef struct {
    u32 ra;
} __OSThreadContext;
typedef struct {
    __OSThreadContext context;
} OSThread;
typedef struct {
    const char *func;
    int line;
} symtable_info_t;

static struct {
    u16 *framebuffer;
    int width, height;
} gCrashScreen;
static struct {
    u16 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    u16 guard[32];
} framebuffer;
static u32 sCrashScreenFont[CRASH_SCREEN_GLYPH_HEIGHT * CRASH_SCREEN_FONT_ROWS * 2 + 1];
static u16 gCrashScreenTextColor = 0xffff;
static s32 sCrashScreenPrintRow_Pixels;
static u32 sCrashScreenPrintLnHeight_Pixels = CRASH_SCREEN_GLYPH_HEIGHT;
static s32 sCrashScreenClipTop, sCrashScreenClipBottom = SCREEN_HEIGHT;
static s32 sAssertScrollPixels, sAssertScrollMaxPixels;
static u16 sCrashScreenPageButtons;
static u8 crashPage, updateBuffer;
static char crashScreenBuf[0x200];
#ifdef PUPPYPRINT_DEBUG
#define LOG_BUFFER_SIZE 16
static char consoleLogTable[LOG_BUFFER_SIZE][255];
static int logLines;
#endif
static char *__n64Assert_Filename = "src/game/jrb_simpleflips.c";
static u32 __n64Assert_LineNum = 300;
static char *__n64Assert_Condition;
static char __n64Assert_MessageBuf[513];
static int assertionCalls;
static void __n64Assert(char *filename, u32 line, char *condition) {
    assert(filename != NULL && line > 0 && condition != NULL);
    assertionCalls++;
}

static int stackTraceGenerated = TRUE;
static u32 sCrashScreenStackTraceCount = 3;
static struct {
    u16 buttonDown, buttonPressed;
} controller, *gPlayer1Controller = &controller;
static char visible[4096];
static int visibleCount, glyphCount, sleeps;

static void osWritebackDCacheAll(void) {
}

static void set_text_color(u32 r, u32 g, u32 b) {
    (void) r;
    (void) g;
    (void) b;
}

static void reset_text_color(void) {
}

static int _Printf(char *(*writer)(char *, const char *, size_t), char *dest, const char *fmt,
                   va_list args) {
    char text[8192];
    int size = vsnprintf(text, sizeof(text), fmt, args);
    assert(size >= 0 && size < (int) sizeof(text));
#ifdef PUPPYPRINT_DEBUG
    if (logLines < LOG_BUFFER_SIZE && strncmp(text, "log-", 4) == 0) {
        char expected[64];
        sprintf(expected, "log-%d: 100%% %%s %%n", LOG_BUFFER_SIZE - 1 - logLines);
        assert(strcmp(text, expected) == 0);
        logLines++;
    }
#endif
    writer(dest, text, size);
    return size;
}

static symtable_info_t get_symbol_info(u32 address) {
    (void) address;
    return (symtable_info_t){ "bhv_jrb_simpleflips_sign_loop", 300 };
}

static const char *get_stack_entry(u32 index) {
    assert(index < 3);
    return "80100000: example_call:123";
}

static void crash_screen_sleep(s32 milliseconds) {
    assert(milliseconds == 30);
    sleeps++;
}

void draw_glyph_actual(s32 x, s32 y, s32 glyph);
static void crash_screen_draw_glyph(s32 x, s32 y, s32 glyph) {
    glyphCount++;
    if (x >= 0 && x + CRASH_SCREEN_GLYPH_WIDTH <= gCrashScreen.width && y >= sCrashScreenClipTop
        && y + CRASH_SCREEN_GLYPH_HEIGHT <= sCrashScreenClipBottom) {
        assert(visibleCount < (int) sizeof(visible) - 1);
        visible[visibleCount++] = glyph + 32;
    }
    draw_glyph_actual(x, y, 0);
}

#include "crash_actual.inc"

static void render_assert(OSThread *thread) {
    visibleCount = 0;
    glyphCount = 0;
    draw_assert(thread);
    visible[visibleCount] = '\0';
    for (int i = 0; i < 32; i++) {
        assert(framebuffer.guard[i] == 0x1234);
    }
}

static void check_pages(s32 assertAvailable) {
    int firstPage = assertAvailable ? CRASH_SCREEN_PAGE_ASSERTS : CRASH_SCREEN_PAGE_SIMPLE;
    crashPage = firstPage;
    sCrashScreenPageButtons = 0;
    int pageCount = CRASH_SCREEN_PAGE_COUNT - !assertAvailable;
    for (int page = firstPage; page < CRASH_SCREEN_PAGE_COUNT; page++) {
        assert(crash_screen_page_number(page, assertAvailable) == page - firstPage + 1);
    }
    for (int page = 1; page <= pageCount; page++) {
        controller.buttonDown = R_TRIG;
        // Repeated/stale shared pressed bits must not repeat page transitions.
        controller.buttonPressed = R_TRIG;
        crash_screen_change_page(assertAvailable);
        int expected = firstPage + page % pageCount;
        assert(crashPage == expected);
        for (int hold = 0; hold < 30; hold++) {
            updateBuffer = FALSE;
            crash_screen_change_page(assertAvailable);
            assert(crashPage == expected && !updateBuffer);
        }
        controller.buttonDown = 0;
        crash_screen_change_page(assertAvailable);
    }
    controller.buttonDown = L_TRIG;
    crash_screen_change_page(assertAvailable);
    assert(crashPage == CRASH_SCREEN_PAGE_COUNT - 1);
    controller.buttonDown = 0;
    crash_screen_change_page(assertAvailable);
    controller.buttonDown = Z_TRIG;
    crash_screen_change_page(assertAvailable);
    assert(crashPage == CRASH_SCREEN_PAGE_COUNT - 1);
    controller.buttonDown = L_TRIG | R_TRIG;
    crash_screen_change_page(assertAvailable);
    assert(crashPage == CRASH_SCREEN_PAGE_COUNT - 1);
}

int main(void) {
    // Formatting longer than either destination must stop at its own terminator.
    char message[700];
    memset(message, 'x', sizeof(message) - 1);
    message[sizeof(message) - 1] = '\0';
    aggressf(FALSE, "%s", message);
    assert(assertionCalls == 1);
    assert(strlen(__n64Assert_MessageBuf) == sizeof(__n64Assert_MessageBuf) - 1);
    error("100% %n %s");
    assert(strcmp(__n64Assert_MessageBuf, "100% %n %s") == 0);
    errorf("value=%u", 123U);
    assert(strcmp(__n64Assert_MessageBuf, "value=123") == 0 && assertionCalls == 3);
    __n64Assert_MessageBuf[0] = '\0';

    OSThread thread = { { 0x80100000 } };
    gCrashScreen.framebuffer = framebuffer.pixels;
    gCrashScreen.width = SCREEN_WIDTH;
    gCrashScreen.height = SCREEN_HEIGHT;
    memset(sCrashScreenFont, 0xff, sizeof(sCrashScreenFont));
    for (int i = 0; i < 32; i++) {
        framebuffer.guard[i] = 0x1234;
    }
    __n64Assert_Condition = sSimpleFlipsEnding;
    assert(strlen(sSimpleFlipsEnding) == 1671);
    render_assert(&thread);
    assert(glyphCount > 1671 && sAssertScrollMaxPixels > 200);
    assert(strstr(visible, "Nathanial scuttle bug"));
    assert(!strstr(visible, "development bandy"));
    assert(strstr(visible, "Z: Faster") && !strstr(visible, "B: Faster"));
    int maxScroll = sAssertScrollMaxPixels;
    controller.buttonDown = D_CBUTTONS;
    crash_screen_scroll_assert();
    assert(sAssertScrollPixels == 10 && sleeps == 1);
    controller.buttonDown = D_CBUTTONS | B_BUTTON;
    crash_screen_scroll_assert();
    assert(sAssertScrollPixels == 20 && sleeps == 2);
    controller.buttonDown = D_CBUTTONS | Z_TRIG;
    crash_screen_scroll_assert();
    assert(sAssertScrollPixels == 60 && sleeps == 2);
    for (int i = 0; i < 100; i++) {
        crash_screen_scroll_assert();
    }
    assert(sAssertScrollPixels == maxScroll);
    render_assert(&thread);
    assert(sAssertScrollMaxPixels == maxScroll);
    assert(strstr(visible, "Japanese video game development bandy"));
    assert(strstr(visible, "Stack Trace:"));
    controller.buttonDown = U_CBUTTONS | Z_TRIG;
    for (int i = 0; i < 100; i++) {
        crash_screen_scroll_assert();
    }
    assert(sAssertScrollPixels == 0);
    render_assert(&thread);
    assert(strstr(visible, "Nathanial scuttle bug"));
#ifdef PUPPYPRINT_DEBUG
    for (int i = 0; i < LOG_BUFFER_SIZE; i++) {
        sprintf(consoleLogTable[i], "log-%d: 100%% %%s %%n", i);
    }
    draw_crash_log();
    assert(logLines == LOG_BUFFER_SIZE);
#endif
    check_pages(FALSE);
    check_pages(TRUE);
    // Paths are literal text and can exceed the former 256-byte path buffer.
    char path[400];
    memset(path, 'p', sizeof(path) - 1);
    path[sizeof(path) - 1] = '\0';
    memcpy(path + 30, "%n/%s/100%", 10);
    __n64Assert_Filename = path;
    sAssertScrollPixels = 0;
    render_assert(&thread);
    crash_screen_println("%s", message);
    assert(strlen(crashScreenBuf) == sizeof(crashScreenBuf) - 1);
    return 0;
}
