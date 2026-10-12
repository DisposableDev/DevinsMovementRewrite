#!/usr/bin/env python3
"""Crash-page input, raw long-text layout, and framebuffer clipping host checks."""
from pathlib import Path
import os
import re
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


def prepare_crash_fixture(folder):
    source = Source(ROOT / "src/debugger/crash_screen.c")
    header = Source(ROOT / "src/debugger/crash_screen.h")
    encounter = Source(ROOT / "src/game/jrb_simpleflips.c")
    definitions = header.macros("CRASH_SCREEN_.*")
    definitions += header.declaration("enum", "CrashPages")
    definitions += Source(ROOT / "src/debugger/assert.h").macros(
        "errorf|error|aggressf|__assert_wrapper")
    definitions += re.search(
        r"static u8 sCrashScreenCharToGlyph\[128\] = \{.*?\n\};",
        source.text, re.S)[0]
    definitions += re.search(
        r"static char sSimpleFlipsEnding\[\] =\n.*?;", encounter.text, re.S)[0]
    definitions += re.search(r"typedef struct \{[^}]*\} CrashTextBuffer;", source.text, re.S)[0]
    # Rasterize a solid test glyph; its shape is a boundary, clipping is real.
    production = source.function("crash_screen_draw_glyph").replace(
        "void crash_screen_draw_glyph(", "void draw_glyph_actual(", 1)
    production += "".join(source.function(name) for name in (
        "write_to_buf", "crash_screen_format_text", "__n64Assert_format", "crash_screen_set_print_top", "crash_screen_print_wrapped_text",
        "crash_screen_print_with_newlines", "crash_screen_print", "crash_screen_println",
        "draw_assert", "crash_screen_scroll_assert", "crash_screen_change_page", "crash_screen_page_number"))
    production += "\n#ifdef PUPPYPRINT_DEBUG\n" + source.function("draw_crash_log") + "\n#endif\n"
    (folder / "crash_definitions.inc").write_text(definitions)
    (folder / "crash_actual.inc").write_text(production)


def prepare_stack_fixture(folder):
    source = Source(ROOT / "src/debugger/stacktrace.c")
    header = Source(ROOT / "src/debugger/stacktrace.h")
    production = "".join(source.function(name) for name in (
        "add_entry_to_stack", "get_stack_entry", "generate_stack"))
    definitions = re.search(r"typedef struct \{.*?\} StackFrame;", header.text, re.S)[0] + "\n"
    definitions += header.macros("STACK_.*|CALLSITE_OFFSET")
    definitions += source.macros("STACK_END_STR")
    (folder / "stack_definitions.inc").write_text(definitions)
    (folder / "stack_actual.inc").write_text(production)


class CrashScreenTests(unittest.TestCase):
    def test_reset_stops_controller_polling_after_in_flight_read(self):
        source = Source(ROOT / "src/debugger/crash_screen.c")
        production = "".join(source.function(name) for name in (
            "crash_screen_reset_pending", "crash_screen_read_controller", "thread2_crash_screen"))
        fixture = r'''
#include <assert.h>
#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#define UNUSED __attribute__((unused))
#define TRUE 1
#define FALSE 0
#define OS_EVENT_CPU_BREAK 1
#define OS_EVENT_FAULT 2
#define CRASH_CONTROLLER_DMA_PENDING (-2)
#define OS_FLAG_FAULT 2
#define OS_FLAG_CPU_BREAK 1
#define OS_MESG_BLOCK 1
#define OS_MESG_NOBLOCK 0
#define THREAD_2_CRASH_SCREEN 2
#define EXC_SYSCALL 32
#define CRASH_SCREEN_PAGE_ASSERTS 0
#define CRASH_SCREEN_NUM_CFBs 2
#define EMU_CONSOLE 1
#define SOUND_MARIO_WAAAOOOW 1
#define L_TRIG 32
#define R_TRIG 16
typedef int s32;
typedef uint16_t RGBA16;
typedef void *OSMesg;
typedef int OSMesgQueue;
typedef struct { int priority, flags; struct { int cause; } context; } OSThread;
static struct { OSThread thread; OSMesgQueue mesgQueue; RGBA16 *framebuffer; } gCrashScreen;
static struct { int seqId; } sBackgroundMusicQueue[1];
static struct { int buttonDown; } controller;
#define gPlayer1Controller (&controller)
static OSThread fault = {.context.cause = EXC_SYSCALL | 0x80000800};
static OSThread gMainThread = {.context.cause = EXC_SYSCALL};
static OSThread *faultThread = &fault;
static RGBA16 gFramebuffers[3][1];
static int gEmulator, most_recent_framebuffer, stale_framebuffers[2] = {1, 2};
static int crash_screen_cfb_index, crashPage, sCrashScreenPageButtons;
static int gControllerBits = 1, gResetTimer, gNmiResetBarsTimer, gGlobalSoundSource;
static OSMesgQueue gSIEventMesgQueue;
static OSMesgQueue sCrashResetMesgQueue;
static int polls, completed, inputs, draws, owned, stopped;
static int sCrashResetRedirected, schedulerStops, sdkShutdown;
static int mode, resetQueued, redirected, cleanupBusy, cleanupCalls, releases, delayCalls;
static jmp_buf done;
static void osSetEventMesg(int event, OSMesgQueue *queue, OSMesg msg) {
    (void)event; (void)queue; (void)msg;
}
static void osStopThread(OSThread *thread) {
    if (thread == &gMainThread) { schedulerStops++; return; }
    assert(thread == NULL && gResetTimer != 0);
    stopped++; longjmp(done, 1);
}
static int osRecvMesg(OSMesgQueue *queue, OSMesg *msg, int block) {
    if (msg != NULL) {
        *msg = (OSMesg) 2;
    }
    if (queue == &sCrashResetMesgQueue) {
        assert(block == OS_MESG_NOBLOCK);
        if (!resetQueued) return -1;
        resetQueued = 0; return 0;
    }
    assert(block == OS_MESG_BLOCK);
    if (queue == &gCrashScreen.mesgQueue && faultThread == &gMainThread) {
        gMainThread.flags = OS_FLAG_FAULT;
    }
    if (queue == &gSIEventMesgQueue) {
        assert(owned && polls == completed + 1);
        completed++;
        if (completed == 2) gResetTimer = 1;
    }
    return 0;
}
static void stop_rumble_for_reset(void) {}
static int reset_pending(void) { return gResetTimer || sdkShutdown; }
static int reset_rsp_for_pre_nmi(void) {
    cleanupCalls++;
    if (cleanupBusy) { cleanupBusy--; return 0; }
    return 1;
}
static void redirect_pre_nmi_to_crash(OSMesgQueue *queue) {
    assert(queue == &sCrashResetMesgQueue && gMainThread.flags);
    redirected++;
    if (mode == 5 || mode == 6 || mode == 7) resetQueued = 1;
}
static OSThread *get_crashed_thread(OSMesg event) { (void)event; return faultThread; }
static void osSetThreadPri(OSThread *thread, int priority) { (void)thread; (void)priority; }
static void crash_screen_update_framebuffer_indices(void) {}
static void stop_sounds_in_continuous_banks(void) {}
static void stop_background_music(int sequence) { (void)sequence; }
static void audio_signal_game_loop_tick(void) {}
static int crash_screen_sleep(int delay) {
    (void)delay;
    delayCalls++;
    return !gResetTimer;
}
static void play_sound(int sound, int source) { (void)sound; (void)source; }
static void crash_screen_darken_framebuffer(float alpha) { (void)alpha; }
static int try_acquire_rumble_pak(void) {
    if (owned) return 0;
    owned = 1;
    if (mode == 4) resetQueued = 1;
    return 1;
}
static void release_rumble_pak_control(void) { assert(owned); owned = 0; releases++; }
static int osContReadDataExForCrash(OSMesgQueue *queue, int (*resetPending)(void)) {
    (void)resetPending;
    assert(queue == &gSIEventMesgQueue && owned && polls < 2);
    assert(gResetTimer == 0);
    if (mode == 3) return -1;
    polls++;
    osRecvMesg(queue, NULL, OS_MESG_BLOCK);
    return 0;
}
static void read_controller_inputs(int thread) {
    assert(thread == THREAD_2_CRASH_SCREEN && completed == polls && owned);
    inputs++; release_rumble_pak_control();
}
static void draw_crash_screen(OSThread *thread) {
    assert(thread == faultThread && inputs == polls); draws++;
    if ((mode == 1 || mode == 2 || mode == 3) && draws == 2) resetQueued = 1;
    if (draws == 2 && (mode == 6 || mode == 7)) {
        gMainThread.flags = mode == 6 ? OS_FLAG_FAULT : OS_FLAG_CPU_BREAK;
    }
    if (mode == 8 && draws == 2) sdkShutdown = 1;
}
#include "actual.inc"
static void run_loop(void) {
    if (setjmp(done) == 0) thread2_crash_screen(NULL);
}
static void reset_fixture(void) {
    polls = completed = inputs = draws = owned = stopped = 0;
    gResetTimer = resetQueued = redirected = releases = cleanupCalls = cleanupBusy = 0;
    delayCalls = 0;
    sCrashResetRedirected = schedulerStops = sdkShutdown = gMainThread.flags = 0;
    faultThread = &fault;
}
int main(void) {
    cleanupBusy = 2;
    run_loop();
    assert(stopped == 1 && polls == 2 && completed == 2 && inputs == 2 && draws == 2);
    assert(owned == 0 && cleanupCalls == 3 && crashPage == CRASH_SCREEN_PAGE_ASSERTS);
    polls = completed = inputs = draws = stopped = 0;
    run_loop();
    assert(stopped == 1 && polls == 0 && completed == 0 && inputs == 0 && draws == 0);
    // A dead owner cannot prevent the debugger from drawing or receiving reset.
    reset_fixture(); mode = 1; owned = 1;
    run_loop();
    assert(draws == 2 && polls == 0 && owned == 1 && releases == 0 && stopped == 1);
    // A scheduler fault redirects reset delivery and cleans the RSP without that thread.
    reset_fixture(); mode = 2; faultThread = &gMainThread;
    run_loop();
    assert(redirected == 1 && stopped == 1 && cleanupCalls == 1 && gResetTimer == 1);
    // Failed reads release ownership without consuming stale controller data.
    reset_fixture(); mode = 3;
    run_loop();
    assert(polls == 0 && inputs == 0 && releases == 2 && draws == 2 && !owned);
    // Reset arriving during lock acquisition prevents a new transfer.
    reset_fixture(); mode = 4;
    run_loop();
    assert(polls == 0 && inputs == 0 && releases == 1 && !owned && stopped == 1);
    // A pre-NMI already queued behind the scheduler fault bypasses both intro delays.
    reset_fixture(); mode = 5; faultThread = &gMainThread;
    run_loop();
    assert(stopped == 1 && delayCalls == 0 && draws == 0 && polls == 0);
    // A later scheduler fault or breakpoint still redirects while the first page stays open.
    for (int later = 6; later <= 7; later++) {
        reset_fixture(); mode = later; owned = 1;
        run_loop();
        assert(draws == 2 && redirected == 1 && schedulerStops == 1 && stopped == 1);
        assert(gResetTimer == 1 && faultThread == &fault);
    }
    // A dropped pre-NMI message is recovered from the SDK's sticky shutdown state.
    reset_fixture(); mode = 8; owned = 1;
    run_loop();
    assert(draws == 2 && stopped == 1 && cleanupCalls == 1 && gResetTimer == 1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix="crash-reset-") as directory:
            folder = Path(directory)
            (folder / "actual.inc").write_text(production)
            (folder / "test.c").write_text(fixture)
            subprocess.run(["cc", "-std=c99", "-O1", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=address,undefined", "-I", str(folder),
                            str(folder / "test.c"), "-o", str(folder / "test")], check=True)
            subprocess.run([str(folder / "test")], check=True,
                           env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})

    def test_scheduler_reset_redirect_preserves_pending_notification(self):
        production = Source(ROOT / "src/boot/main.c").function(
            "redirect_pre_nmi_to_crash")
        fixture = r'''
#include <assert.h>
#include <stddef.h>
typedef void *OSMesg;
typedef int OSMesgQueue;
typedef int OSIntMask;
#define OS_IM_NONE 0
#define OS_MESG_NOBLOCK 0
#define OS_EVENT_PRENMI 1
#define MESG_NMI_REQUEST 104
static OSMesgQueue gIntrMesgQueue, fallback;
static OSMesg messages[4];
static int index, count, pending, masks, routeCalls;
static OSMesgQueue *resetRoute;
static OSIntMask osSetIntMask(OSIntMask mask) {
    assert(mask == (masks % 2 == 0 ? OS_IM_NONE : 123));
    masks++; return 123;
}
static void osSetEventMesg(int event, OSMesgQueue *queue, OSMesg message) {
    assert(event == OS_EVENT_PRENMI && queue == &fallback && message == NULL);
    assert(masks % 2 == 1); resetRoute = queue; routeCalls++;
}
static int osRecvMesg(OSMesgQueue *queue, OSMesg *message, int flags) {
    assert(queue == &gIntrMesgQueue && flags == OS_MESG_NOBLOCK);
    if (index == count) return -1;
    *message = messages[index++]; return 0;
}
static int osSendMesg(OSMesgQueue *queue, OSMesg message, int flags) {
    assert(queue == &fallback && message == NULL && flags == OS_MESG_NOBLOCK);
    if (pending) return -1;
    pending = 1; return 0;
}
#include "actual.inc"
int main(void) {
    messages[0] = (OSMesg)100;
    messages[1] = (OSMesg)MESG_NMI_REQUEST;
    messages[2] = (OSMesg)102;
    count = 3;
    redirect_pre_nmi_to_crash(&fallback);
    assert(pending == 1 && index == 3 && masks == 2 && resetRoute == &fallback);
    // No old reset queued: a subsequent SDK event uses the new recipient.
    index = count = pending = 0;
    redirect_pre_nmi_to_crash(&fallback);
    assert(pending == 0 && routeCalls == 2 && masks == 4);
    assert(osSendMesg(resetRoute, NULL, OS_MESG_NOBLOCK) == 0 && pending == 1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory(prefix="crash-reset-route-") as directory:
            folder = Path(directory)
            (folder / "actual.inc").write_text(production)
            (folder / "test.c").write_text(fixture)
            subprocess.run(["cc", "-std=c99", "-O1", "-Wall", "-Wextra", "-Werror",
                            "-fsanitize=address,undefined", "-I", str(folder),
                            str(folder / "test.c"), "-o", str(folder / "test")], check=True)
            subprocess.run([str(folder / "test")], check=True,
                           env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})

    def test_stack_scan_bounds_and_first_indirect_call(self):
        with tempfile.TemporaryDirectory(prefix="crash-stack-") as directory:
            folder = Path(directory)
            prepare_stack_fixture(folder)
            subprocess.run(["cc", "-std=c99", "-O1", "-Wall", "-Wextra", "-Werror",
                            "-Wno-int-to-pointer-cast", "-fsanitize=undefined,bounds",
                            "-fno-sanitize-recover=all", "-I", str(folder),
                            str(ROOT / "tests/crash_stack_test.c"), "-o", str(folder / "test")],
                           check=True)
            subprocess.run([str(folder / "test")], check=True)

    def test_assert_scrolling_and_page_edges(self):
        with tempfile.TemporaryDirectory(prefix="crash-screen-") as directory:
            folder = Path(directory)
            prepare_crash_fixture(folder)
            for log_page in (False, True):
                command = ["cc", "-std=c99", "-O1", "-Wall", "-Wextra", "-Werror",
                           "-fsanitize=address,undefined", "-I", str(folder)]
                if log_page:
                    command.append("-DPUPPYPRINT_DEBUG")
                subprocess.run(command + [str(ROOT / "tests/crash_screen_test.c"),
                                          "-o", str(folder / "test")], check=True)
                subprocess.run([str(folder / "test")], check=True,
                               env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})


if __name__ == "__main__":
    unittest.main()
