#!/usr/bin/env python3
"""Native stick mapping and graphics-completion scheduling, with controlled VI/DP events."""
import subprocess
import tempfile
import unittest
from pathlib import Path

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]


def compile_run(code, defines=()):
    with tempfile.TemporaryDirectory() as directory:
        src = Path(directory) / 'check.c'
        exe = Path(directory) / 'check'
        src.write_text(code)
        subprocess.run(['cc', '-std=c99', '-Werror=implicit-function-declaration', *defines,
                        str(src), '-lm', '-o', str(exe)], check=True)
        subprocess.run([str(exe)], check=True)


class InputVideoLatencyTests(unittest.TestCase):
    def test_all_raw_stick_pairs(self):
        game = Source(ROOT / 'src/game/game_init.c')
        compile_run(r'''
#include <assert.h>
#include <math.h>
typedef int s32;
struct Controller { int rawStickX, rawStickY; float stickX, stickY, stickMag; };
enum { GO_DEADZONE };
static s32 game_options_get(s32 option) {
    assert(option == GO_DEADZONE);
    return 6;
}
''' + game.function('adjust_analog_stick') + r'''
int main(void) {
    struct Controller pad;
    for (int x = -128; x <= 127; x++) for (int y = -128; y <= 127; y++) {
        pad.rawStickX = x; pad.rawStickY = y;
        adjust_analog_stick(&pad);
        float expectedX = x > 6 ? x - 6 : x < -6 ? x + 6 : 0;
        float expectedY = y > 6 ? y - 6 : y < -6 ? y + 6 : 0;
        float magnitude = sqrtf(expectedX * expectedX + expectedY * expectedY);
        if (magnitude > 64) { expectedX *= 64/magnitude; expectedY *= 64/magnitude; magnitude = 64; }
        assert(fabsf(pad.stickX - expectedX) < 0.0001f);
        assert(fabsf(pad.stickY - expectedY) < 0.0001f);
        assert(fabsf(pad.stickMag - magnitude) < 0.0001f);
    }
    for (int axis = 0; axis < 2; axis++) for (int sign = -1; sign <= 1; sign += 2) {
        pad.rawStickX = axis ? 0 : sign * 7;
        pad.rawStickY = axis ? sign * 7 : 0;
        adjust_analog_stick(&pad);
        assert(pad.stickMag == 1 && (axis ? pad.stickY : pad.stickX) == sign);
    }
    return 0;
}
''')

    def test_completed_frame_publication_and_legacy_paths(self):
        game = Source(ROOT / 'src/game/game_init.c')
        boot = Source(ROOT / 'src/boot/main.c')
        production = ''.join(game.function(name) for name in (
            'video_low_latency_enabled', 'game_present_completed_frame', 'display_and_vsync'))
        production += boot.function('handle_dp_complete')
        code = r'''
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
typedef int s32; typedef unsigned short u16; typedef unsigned int u32;
typedef uintptr_t OSMesg;
typedef struct { int messages; } OSMesgQueue;
#define TRUE 1
#define FALSE 0
#define OS_MESG_BLOCK 1
#define OS_MESG_NOBLOCK 0
#define SPTASK_STATE_FINISHED_DP 4
#define PHYSICAL_TO_VIRTUAL(x) ((uintptr_t)(x))
#define INSTANT_INPUT_WHITELIST 1
#ifndef VIDEO_LOW_LATENCY
#define VIDEO_LOW_LATENCY 1
#endif
struct SPTask { OSMesgQueue *msgqueue; OSMesg msg; s32 state; u16 framebuffer, presentOnComplete; };
struct GfxPool { struct SPTask spTask; } pools[2], *gGfxPool;
static struct SPTask *sCurrentDisplaySPTask;
static OSMesgQueue gGfxVblankQueue, gGameVblankQueue;
static OSMesg gMainReceivedMesg;
static uintptr_t gPhysicalFramebuffers[3] = { 100, 200, 300 };
static u16 sRenderedFramebuffer, sRenderingFramebuffer;
static u32 gGlobalTimer;
static int gEmulator, reset, tick, finishTick, renderDelay, inDp, swaps, displayed, queued;
static int gameWaits, callbackCalls;
static void (*gGoddardVblankCallback)(void);
static s32 video_low_latency_enabled(void);
static void handle_dp_complete(void);
static int reset_pending(void) { return reset; }
static void goddard(void) { callbackCalls++; }
static void osViSwapBuffer(void *pointer) {
    if (video_low_latency_enabled()) assert(inDp);
    queued = (uintptr_t)pointer; swaps++;
}
static int osSendMesg(OSMesgQueue *queue, OSMesg msg, int flags) {
    assert(flags == OS_MESG_NOBLOCK); queue->messages = 1; return 0;
}
static void advance_vi(void) {
    tick++;
    if (sCurrentDisplaySPTask && tick >= finishTick) {
        inDp = 1; handle_dp_complete(); inDp = 0;
    }
    displayed = queued;
    gGameVblankQueue.messages = 1;
}
static int osRecvMesg(OSMesgQueue *queue, OSMesg *msg, int flags) {
    assert(flags == OS_MESG_BLOCK);
    if (queue == &gGameVblankQueue) gameWaits++;
    while (!queue->messages) advance_vi();
    queue->messages = 0; return 0;
}
static void exec_display_list(struct SPTask *task) {
    assert(!sCurrentDisplaySPTask);
    if (video_low_latency_enabled()) {
        assert((int)gPhysicalFramebuffers[task->framebuffer] != displayed);
        assert((int)gPhysicalFramebuffers[task->framebuffer] != queued);
    }
    sCurrentDisplaySPTask = task; finishTick = tick + renderDelay;
}
''' + production + r'''
int main(void) {
    for (int emulator = 0; emulator < 2; emulator++) {
        for (int delay = 1; delay <= 6; delay += 5) {
            gEmulator = emulator;
            displayed = queued = 100;
            sRenderedFramebuffer = 0; sRenderingFramebuffer = emulator ? 0 : 1;
            gGlobalTimer = 1; tick = swaps = gameWaits = callbackCalls = 0;
            renderDelay = delay; gGameVblankQueue.messages = 0;
            gGfxVblankQueue.messages = 1; sCurrentDisplaySPTask = NULL;
            for (int frame = 0; frame < 24; frame++) {
                gGfxPool = &pools[gGlobalTimer % 2];
                gGfxPool->spTask = (struct SPTask) { .msgqueue = &gGfxVblankQueue,
                    .framebuffer = sRenderingFramebuffer,
                    .presentOnComplete = video_low_latency_enabled() };
                gGoddardVblankCallback = goddard;
                display_and_vsync();
#ifndef UNLOCK_FPS
                assert(gameWaits == (frame + 1) * 2);
                if (frame == 0 && delay == 1 && !emulator) {
                    assert(displayed == (video_low_latency_enabled() ? 200 : 100));
                }
#endif
                assert(callbackCalls == frame + 1 && !gGoddardVblankCallback);
                assert(gGlobalTimer == (u32)frame + 2);
                assert(sRenderingFramebuffer == (emulator ? 0 : (frame + 2) % 3));
            }
            while (sCurrentDisplaySPTask) advance_vi();
            assert(swaps == 24);
        }
    }
    // Completion suppresses presentation during reset but still releases its waiter.
    gEmulator = 0; reset = 1;
    struct SPTask resetTask = { .msgqueue = &gGfxVblankQueue, .framebuffer = 2,
                               .presentOnComplete = 1 };
    sCurrentDisplaySPTask = &resetTask; int before = swaps;
    handle_dp_complete();
    assert(!sCurrentDisplaySPTask && resetTask.state == SPTASK_STATE_FINISHED_DP);
    assert(swaps == before && gGfxVblankQueue.messages == 1);
    return 0;
}
'''
        for defines in ((), ('-DVIDEO_LOW_LATENCY=0',), ('-DUNLOCK_FPS',)):
            compile_run(code, defines)

    def test_task_snapshot_and_reset_buffer_ownership(self):
        game = Source(ROOT / 'src/game/game_init.c')
        task = game.function('create_gfx_task_structure')
        self.assertIn('gGfxSPTask->framebuffer = sRenderingFramebuffer;', task)
        self.assertIn('gGfxSPTask->presentOnComplete = video_low_latency_enabled();', task)
        reset = game.function('draw_reset_bars')
        self.assertIn('fbNum = sRenderedFramebuffer;', reset)
        self.assertIn('sRenderingFramebuffer++;', game.function('render_init'))


if __name__ == '__main__':
    unittest.main()
