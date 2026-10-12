"""Late crash, latched reset, bounded motor shutdown and forced RSP cleanup checks."""
from pathlib import Path
import unittest

from regression_support import Source
import test_crash_menu_safety

ROOT = Path(__file__).resolve().parents[1]


class SoftResetRecoveryTests(unittest.TestCase):
    compile_run = test_crash_menu_safety.CrashMenuSafetyTests.compile_run

    def test_timer_watchdog_recovers_without_scheduler_or_crash_thread(self):
        boot = Source(ROOT / "src/boot/main.c")
        self.compile_run(r'''
#include <setjmp.h>
#define OS_STATE_RUNNING 4
#define OS_STATE_RUNNABLE 2
#define OS_STATE_WAITING 8
#define OS_STATE_STOPPED 1
#define OS_MESG_BLOCK 1
#define SP_SET_HALT 2
typedef int OSMesgQueue;
static OSMesgQueue sResetWatchdogQueue;
static volatile u32 __osShutdown;
static struct { int state; } gMainThread;
static int gResetTimer, gNmiResetBarsTimer, sResetWatchdogStarted, sResetCleanupStarted;
static u32 sResetWatchdogStart, sResetCleanupStart, sResetWatchdogVblanks, gNumVblanks, count;
static int stopped, rumbleStops, halts, cleanupCalls, messages, running = 1, pc = 0x100;
static jmp_buf finished;
static u32 osGetCount(void) { return count; }
static void osStopThread(void *thread) {
    assert(thread == &gMainThread);
    gMainThread.state = OS_STATE_STOPPED;
    stopped++;
}
static void stop_rumble_for_reset(void) { rumbleStops++; }
static void __osSpSetStatus(int status) { assert(status == SP_SET_HALT); halts++; running = 0; }
static int osAfterPreNMI(void) {
    cleanupCalls++;
    if (cleanupCalls == 3) longjmp(finished, 1);
    if (running) return -1;
    pc = 0; return 0;
}
static int osRecvMesg(OSMesgQueue *queue, void *message, int flags) {
    assert(queue == &sResetWatchdogQueue && message == NULL && flags == OS_MESG_BLOCK);
    // Timer delivery advances independently: neither application observer runs.
    count += 10000; messages++; assert(messages <= 11); return 0;
}
''' + boot.macros("RESET_RSP_GRACE_US") + boot.function("reset_pending")
                         + boot.function("reset_rsp_for_pre_nmi")
                         + boot.function("reset_watchdog_update")
                         + boot.function("thread_reset_watchdog") + r'''
int main(int argc, char **argv) {
    assert(argc == 2); int mode = atoi(argv[1]);
    assert(!reset_watchdog_update() && !stopped && !halts);
    gMainThread.state = mode == 3 ? 0 : mode == 4 ? OS_STATE_STOPPED : OS_STATE_RUNNING;
    if (mode == 1) gMainThread.state = OS_STATE_WAITING;
    if (mode == 2) count = UINT32_MAX - 50000;
    if (mode == 5) { running = 0; pc = 0; gResetTimer = 7; }
    __osShutdown = 1; // No delivered pre-NMI message or scheduler fault flag.
    // Normal shutdown can continue beyond the deadline while retraces make progress.
    for (int i = 0; i < 40; i++) {
        count += 10000; gNumVblanks++;
        assert(!reset_watchdog_update() && !stopped && !halts && !rumbleStops);
    }
    sResetWatchdogStarted = FALSE;
    if (mode == 2) count = UINT32_MAX - 50000;
    if (!setjmp(finished)) thread_reset_watchdog(NULL);
    assert(messages == 11 && cleanupCalls == 3 && pc == 0 && halts >= 1);
    assert(stopped == (mode == 3 || mode == 4 ? 0 : 1));
    assert(rumbleStops == 1 && gResetTimer == (mode == 5 ? 7 : 1));
    return 0;
}
''', arguments=range(6))

    def test_controller_reset_between_ownership_and_each_dma_start(self):
        serial = Source(ROOT / "src/game/gamecube_controller.c")
        self.compile_run(r'''
#define OS_WRITE 1
#define OS_READ 0
#define OS_MESG_BLOCK 1
#define CONT_CMD_READ_BUTTON 1
#define CONT_CMD_REQUEST_STATUS 0
#define OS_IM_NONE 0
typedef int OSMesgQueue;
typedef int OSIntMask;
static OSMesgQueue completion;
static struct { u32 ramarray[16]; } __osContPifRam;
static int mode, reset, token = 1, starts, waits, released, pending, masked;
static int __osContLastCmd;
static int reset_pending(void) { return reset; }
static int osSetIntMask(int mask) {
    int old = masked ? OS_IM_NONE : 1;
    if (mask == OS_IM_NONE && mode == 7) reset = 1;
    masked = mask == OS_IM_NONE;
    return old;
}
static void __osSiGetAccess(void) { assert(token); token = 0; if (mode == 2) reset = 1; }
static void __osSiRelAccess(void) { assert(!token); token = 1; released++; }
static void __osPackReadData(void) { if (mode == 3) reset = 1; }
static void __osPackRequestData(int command) { assert(command == 0); __osPackReadData(); }
static int osRecvMesg(OSMesgQueue *queue, void *message, int flags) {
    assert(queue == &completion && message == NULL && flags == OS_MESG_BLOCK);
    assert(pending && !masked); pending = 0; waits++;
    if (mode == 4) reset = 1;
    return 0;
}
static int __osSiRawStartDma(int direction, void *buffer) {
    assert(!reset && masked && !token && !pending && buffer == __osContPifRam.ramarray);
    starts++;
    if ((mode == 5 && direction == OS_WRITE) || (mode == 6 && direction == OS_READ)) return -1;
    pending = 1; return 0;
}
''' + serial.function("controller_start_dma") + serial.function("osContStartReadDataEx") + serial.function("osContStartQueryEx") + r'''
int main(int argc, char **argv) {
    assert(argc == 2); mode = atoi(argv[1]);
    int query = mode >= 8;
    if (query) { mode -= 8; __osContLastCmd = CONT_CMD_READ_BUTTON; }
    if (mode == 1) reset = 1;
    int result = query ? osContStartQueryEx(&completion) : osContStartReadDataEx(&completion);
    assert(!masked && token && released == 1);
    if (!mode) {
        assert(result == 0 && starts == 2 && waits == 1 && pending);
    } else {
        assert(result != 0 && !pending);
        assert(starts == (mode == 4 || mode == 5 ? 1 : mode == 6 ? 2 : 0));
        assert(waits == (mode == 4 || mode == 6 ? 1 : 0));
    }
    return 0;
}
''', arguments=range(16))

    def test_controller_type_refresh_owns_completion_and_retains_read_failures(self):
        game = Source(ROOT / "src/game/game_init.c")
        self.compile_run(r'''
#define MAXCONTROLLERS 4
#define OS_MESG_BLOCK 1
static u32 gGlobalTimer;
static u8 gControllerBits;
static int gSIEventMesgQueue, gMainReceivedMesg, gControllerStatuses[4];
static int failed, queries, completions, decoded;
static int osContStartQueryEx(int *queue) {
    assert(queue == &gSIEventMesgQueue); queries++; return failed;
}
static int osRecvMesg(int *queue, int *message, int flags) {
    assert(queue == &gSIEventMesgQueue && message == &gMainReceivedMesg);
    assert(flags == OS_MESG_BLOCK && !failed); completions++; return 0;
}
static void osContGetQuery(int *status) {
    assert(status == gControllerStatuses && completions == queries); decoded++;
}
''' + game.function("refresh_controller_types") + r'''
int main(void) {
    assert(refresh_controller_types() && queries == 1 && completions == 1 && decoded == 1);
    gGlobalTimer = 1; assert(refresh_controller_types() && queries == 1);
    gGlobalTimer = 30; gControllerBits = 15;
    assert(refresh_controller_types() && queries == 1);
    gControllerBits = 0; failed = 1;
    assert(!refresh_controller_types() && queries == 2 && completions == 1 && decoded == 1);
    return 0;
}
''')

    def test_reset_during_rsp_load_does_not_start_loaded_task(self):
        boot = Source(ROOT / "src/boot/main.c")
        self.compile_run(r'''
#define M_AUDTASK 1
#define M_GFXTASK 2
#define SPTASK_STATE_RUNNING 1
struct SPTask { int task, state; };
static struct SPTask audio, display;
static struct SPTask *gActiveSPTask, *sCurrentAudioSPTask = &audio, *sCurrentDisplaySPTask = &display;
static int reset, mode, loads, starts;
static int reset_pending(void) { return reset; }
static void osSpTaskLoad(void *task) { assert(task); loads++; if (mode) reset = 1; }
static void osSpTaskStartGo(void *task) { assert(task && !reset); starts++; }
''' + boot.function("start_sptask") + r'''
int main(int argc, char **argv) {
    assert(argc == 2); mode = atoi(argv[1]);
    start_sptask(M_AUDTASK);
    assert(loads == 1 && starts == !mode && audio.state == !mode);
    reset = 0;
    start_sptask(M_GFXTASK);
    assert(loads == 2 && starts == (mode ? 0 : 2) && display.state == !mode);
    return 0;
}
''', arguments=range(2))

    def test_ordinary_motor_starts_stop_at_reset_and_do_not_wait_for_failed_dma(self):
        serial = Source(ROOT / "src/game/gamecube_controller.c")
        self.compile_run(r'''
#define OS_WRITE 1
#define OS_READ 0
#define OS_MESG_BLOCK 1
#define OS_IM_NONE 0
#define PFS_MOTOR_INITIALIZED 1
#define CONT_CONSOLE_MASK 255
#define CONT_CONSOLE_GCN 4
#define CONT_CMD_EXE 1
#define CONT_CMD_END 254
#define CHNL_ERR_MASK 0xc0
#define PFS_ERR_CONTRFAIL 4
#define BLOCKSIZE 32
typedef int OSMesgQueue;
typedef int OSIntMask;
typedef struct { int status, channel; OSMesgQueue *queue; } OSPfs;
typedef struct { u8 dummy, txsize, rxsize, cmd, addrh, addrl, data[32], datacrc; } Packet;
typedef struct { u32 ramarray[15]; u32 pifstatus; } OSPifRam;
#define READFORMAT(ptr) ((Packet *)(ptr))
static OSPifRam __MotorDataBuf[4];
static struct { int type; } gControllerStatuses[4];
static u8 __osGamecubeRumbleEnabled[4], __osContLastCmd;
static int mode, reset, masked, token = 1, starts, waits, pending;
static int reset_pending(void) { return reset; }
static int osSetIntMask(int mask) { int old = masked ? 0 : 1; masked = mask == 0; return old; }
static void __osSiGetAccess(void) { assert(token); token = 0; if (mode == 1) reset = 1; }
static void __osSiRelAccess(void) { assert(!token); token = 1; }
static int __osSiRawStartDma(int direction, void *buffer) {
    assert(!reset && masked && !token && !pending && buffer == &__MotorDataBuf[0]);
    starts++;
    if ((mode == 3 && direction == OS_WRITE) || (mode == 4 && direction == OS_READ)) return -1;
    pending = 1; return 0;
}
static int osRecvMesg(OSMesgQueue *queue, void *message, int flags) {
    assert(queue != NULL && message == NULL && flags == OS_MESG_BLOCK && pending && !masked);
    pending = 0; waits++;
    if (mode == 2) reset = 1;
    return 0;
}
''' + serial.function("controller_start_dma") + serial.function("__osMotorAccessEx") + r'''
int main(int argc, char **argv) {
    assert(argc == 2); mode = atoi(argv[1]);
    OSMesgQueue completion = 0;
    OSPfs pfs = {.channel = 0, .status = PFS_MOTOR_INITIALIZED, .queue = &completion};
    if (mode == 5) gControllerStatuses[0].type = CONT_CONSOLE_GCN;
    if (mode == 6) pfs.status = 0;
    int result = __osMotorAccessEx(&pfs, 0);
    assert(token && !masked && !pending);
    if (mode == 0) assert(result == 0 && starts == 2 && waits == 2);
    if (mode == 1) assert(result != 0 && !starts && !waits);
    if (mode == 2) assert(result != 0 && starts == 1 && waits == 1);
    if (mode == 3) assert(result != 0 && starts == 1 && !waits);
    if (mode == 4) assert(result != 0 && starts == 2 && waits == 1);
    if (mode == 5) assert(result == 0 && !starts && !waits && !__osGamecubeRumbleEnabled[0]);
    if (mode == 6) assert(result == 5 && !starts && !waits);
    return 0;
}
''', arguments=range(7))

    def test_game_polls_after_setup_and_skips_failed_or_reset_reads(self):
        game = Source(ROOT / "src/game/game_init.c")
        start, end = game.function_span("thread5_game_loop")
        start = game.text.index("    while (TRUE) {", start, end)
        end = game.text.index("        profiler_update(PROFILER_TIME_CONTROLLERS,", start, end)
        prefix = game.mapped(start, end)
        self.compile_run(r'''
#include <setjmp.h>
typedef int OSMesgQueue;
static OSMesgQueue gSIEventMesgQueue;
#define THREAD_5_GAME_LOOP 5
static int gResetTimer, gControllerBits = 1, mode, calls, released;
static int physicalY = 2, sampledY, consumed, setup, held, pressed, previousHeld;
static void audio_game_loop_tick(void) {
    assert(setup == 0); setup = 1;
    physicalY = 127; held = 1;
}
static void select_gfx_pool(void) { assert(setup == 1); setup = 2; }
static void read_controller_inputs(int thread) {
    assert(thread == THREAD_5_GAME_LOOP && setup == 2);
    if (gControllerBits) {
        assert(calls == 1 && sampledY == 127);
        pressed = held & ~previousHeld;
        previousHeld = held;
        assert(pressed == 1);
    } else {
        assert(calls == 1 && sampledY == 127); // Empty boot ports are still polled for hotplug.
    }
    consumed++;
}
static jmp_buf finished;
static void profiler_frame_setup(void) {
    if (mode == 2 && released) longjmp(finished, 1);
    setup = 0;
}
static void draw_reset_bars(void) { assert(gResetTimer); longjmp(finished, 1); }
static int reset_pending(void) { return gResetTimer; }
static int refresh_controller_types(void) { assert(!gResetTimer); return 1; }
static void block_until_rumble_pak_free(void) { if (mode == 1) gResetTimer = 1; }
static void release_rumble_pak_control(void) { released++; }
static int osContStartReadDataEx(OSMesgQueue *queue) {
    assert(queue == &gSIEventMesgQueue && !gResetTimer && setup == 2);
    sampledY = physicalY; calls++; return mode == 2 ? -1 : 0;
}
static void frame_prefix(void) {
''' + prefix + r'''
        return;
    }
}
int main(int argc, char **argv) {
    assert(argc == 2); mode = atoi(argv[1]);
    if (mode == 3) gControllerBits = 0;
    if (!setjmp(finished)) frame_prefix();
    assert(calls == (mode == 1 ? 0 : 1));
    assert(released == (mode == 1 || mode == 2 ? 1 : 0));
    assert(consumed == (mode == 0 || mode == 3 ? 1 : 0));
    return 0;
}
''', arguments=range(4))

    def test_sound_shutdown_handles_cycles_invalid_links_and_full_healthy_bank(self):
        audio = Source(ROOT / "src/audio/external.c")
        boot = Source(ROOT / "src/boot/main.c")
        self.compile_run(r'''
#define SOUND_DISCRETE 0x80
#define NO_SOUND 0
#define SOUND_BANK_COUNT 1
#define SEQ_PLAYER_SFX 0
#define SOUND_BANKS_BACKGROUND 1
static int gResetTimer, gNmiResetBarsTimer, rumbleStops, fades, updates;
static struct { u8 next; u32 soundBits; } sSoundBanks[1][40];
static u8 sNumProcessedSoundRequests, sSoundRequestCount;
static struct { u32 soundBits; } sSoundRequests[256];
static void update_background_music_after_sound(u8 bank, u8 index) {
    assert(bank == 0 && index > 0 && index < 40); updates++;
}
static void stop_rumble_for_reset(void) { rumbleStops++; }
static void sound_banks_disable(int player, int banks) { assert(player == 0 && banks == 1); }
static void fadeout_music(int frames) { assert(frames == 90); fades++; }
''' + audio.function("stop_continuous_sounds_in_bank")
                         + audio.function("stop_sounds_in_continuous_banks")
                         + boot.function("handle_nmi_request") + r'''
int main(int argc, char **argv) {
    assert(argc == 2); int mode = atoi(argv[1]);
    for (int i = 0; i < 40; i++) {
        sSoundBanks[0][i].next = i == 39 ? 0xff : i + 1;
        sSoundBanks[0][i].soundBits = i % 2 ? 1 : SOUND_DISCRETE;
    }
    if (mode == 1) sSoundBanks[0][1].next = 1;
    if (mode == 2) sSoundBanks[0][3].next = 1;
    if (mode == 3) sSoundBanks[0][1].next = 0;
    if (mode == 4) sSoundBanks[0][1].next = 40;
    if (mode == 5) sSoundBanks[0][0].next = 255;
    if (mode == 6) sSoundBanks[0][0].next = 254;
    sNumProcessedSoundRequests = 254; sSoundRequestCount = 1;
    sSoundRequests[254].soundBits = 1;
    sSoundRequests[255].soundBits = SOUND_DISCRETE;
    sSoundRequests[0].soundBits = 1;
    handle_nmi_request();
    assert(gResetTimer && rumbleStops == 1 && fades == 1 && updates <= 39);
    assert(!sSoundRequests[254].soundBits && !sSoundRequests[0].soundBits);
    assert(sSoundRequests[255].soundBits == SOUND_DISCRETE);
    if (mode == 0) {
        assert(updates == 20);
        for (int i = 1; i < 40; i++) assert(sSoundBanks[0][i].soundBits == (i % 2 ? NO_SOUND : SOUND_DISCRETE));
    }
    return 0;
}
''', arguments=range(7))

    def test_rsp_cleanup_grace_halt_count_wrap_and_shared_deadline(self):
        source = Source(ROOT / "src/boot/main.c")
        self.compile_run(r'''
static volatile u32 __osShutdown;
static int gResetTimer, sResetCleanupStarted, running = 1, ignoresHalt;
static u32 sResetCleanupStart, count;
static int attempts, halts, pc = 0x100;
#define SP_SET_HALT 2
static u32 osGetCount(void) { return count; }
static int osAfterPreNMI(void) {
    attempts++;
    if (running) return -1;
    pc = 0;
    return 0;
}
static void __osSpSetStatus(u32 value) {
    assert(value == SP_SET_HALT);
    halts++;
    if (!ignoresHalt) running = 0;
}
''' + source.macros("RESET_RSP_GRACE_US") + source.function("reset_pending")
                         + source.function("reset_rsp_for_pre_nmi") + r'''
int main(int argc, char **argv) {
    assert(argc == 2);
    int mode = atoi(argv[1]);
    assert(!reset_pending() && !reset_rsp_for_pre_nmi() && attempts == 0 && halts == 0);
    // The queue can be absent or full: the SDK latch is still enough to detect reset.
    __osShutdown = 1;
    assert(reset_pending());
    if (mode == 0) {
        running = 0;
        assert(reset_rsp_for_pre_nmi() && pc == 0 && !halts);
        return 0;
    }
    if (mode == 3) count = UINT32_MAX - 100;
    if (mode == 4) ignoresHalt = 1;
    u32 start = count;
    assert(!reset_rsp_for_pre_nmi() && !halts);
    count = start + RESET_RSP_GRACE_US - 1;
    assert(!reset_rsp_for_pre_nmi() && !halts && pc == 0x100);
    if (mode == 1) {
        // Finishing normally within the grace period requires no force halt.
        running = 0;
        assert(reset_rsp_for_pre_nmi() && pc == 0 && !halts);
        return 0;
    }
    count = start + RESET_RSP_GRACE_US;
    int completed = reset_rsp_for_pre_nmi();
    assert(halts == 1 && completed == !ignoresHalt);
    if (ignoresHalt) {
        assert(pc == 0x100);
        ignoresHalt = 0;
        assert(reset_rsp_for_pre_nmi() && halts == 2 && pc == 0);
    } else {
        assert(pc == 0);
        // Scheduler and crash callers share the same deadline and completed state.
        assert(reset_rsp_for_pre_nmi() && halts == 1);
    }
    return 0;
}
''', arguments=range(5))

    def test_motor_shutdown_never_blocks_and_retains_pending_dma_ownership(self):
        serial = Source(ROOT / "src/game/gamecube_controller.c")
        rumble = Source(ROOT / "src/game/rumble_init.c")
        self.compile_run(r'''
#define MAXCONTROLLERS 4
#define BLOCKSIZE 32
#define PFS_MOTOR_INITIALIZED 1
#define CONT_CONSOLE_MASK 255
#define CONT_CONSOLE_GCN 4
#define CONT_CMD_END 254
#define CONT_CMD_EXE 1
#define OS_WRITE 1
#define OS_READ 0
#define OS_MESG_NOBLOCK 0
#define CRASH_CONTROLLER_DMA_PENDING (-2)
typedef int OSMesgQueue;
typedef struct { int channel, status; OSMesgQueue *queue; } OSPfs;
typedef struct { u32 ramarray[15]; u32 pifstatus; } OSPifRam;
typedef struct {
    u8 dummy, txsize, rxsize, cmd, addrh, addrl, data[32], datacrc;
} __OSContRamReadFormat;
#define READFORMAT(ptr) ((__OSContRamReadFormat*)(ptr))
static OSPifRam __MotorDataBuf[4];
static struct { int type; } gControllerStatuses[4];
static u8 __osGamecubeRumbleEnabled[4], __osContLastCmd;
static OSMesgQueue __osSiAccessQueue, completion;
static s32 __osSiAccessQueueEnabled = 1;
static OSPfs gRumblePakPfs = {.channel = 1, .status = PFS_MOTOR_INITIALIZED, .queue = &completion};
static int mode, token = 1, starts, pending, stale, acquired, released, outerReleased;
static int sCharacterMovementRumbleVblanks = 6, sRumblePakActive = 1, sRumblePakResetStopped;
static u32 count;
static u32 osGetCount(void) { count += 1000; return count; }
static int __osSiDeviceBusy(void) { return mode == 2; }
static void __osSiRelAccess(void) { assert(!token); token = 1; released++; }
static int try_acquire_rumble_pak(void) { return 1; }
static void release_rumble_pak_control(void) { outerReleased++; }
static int osRecvMesg(OSMesgQueue *queue, void *message, int flags) {
    (void)message;
    assert(flags == OS_MESG_NOBLOCK);
    if (queue == &__osSiAccessQueue) {
        if (!token) return -1;
        token = 0; acquired++; return 0;
    }
    assert(queue == &completion);
    if (stale) { stale--; return 0; }
    if (!pending || mode == 5 || (mode == 6 && starts == 2)) return -1;
    pending = 0;
    return 0;
}
static int __osSiRawStartDma(int direction, void *buffer) {
    assert(!token && !pending && !stale);
    assert(buffer == &__MotorDataBuf[1]);
    __OSContRamReadFormat *packet = READFORMAT((u8 *)buffer + 1);
    for (int i = 0; i < 32; i++) assert(packet->data[i] == 0);
    assert(__MotorDataBuf[1].pifstatus == CONT_CMD_EXE);
    starts++;
    if ((mode == 3 && direction == OS_WRITE) || (mode == 4 && direction == OS_READ)) return -1;
    pending = 1;
    return 0;
}
''' + serial.macros("CONTROLLER_TIMEOUT_US") + serial.function("controller_wait_bounded")
                         + serial.function("osMotorStopExForReset")
                         + rumble.function("stop_rumble_for_reset") + r'''
int main(int argc, char **argv) {
    assert(argc == 2);
    mode = atoi(argv[1]);
    memset(__MotorDataBuf, 0xAA, sizeof(__MotorDataBuf));
    if (mode == 1) token = 0;
    if (mode == 7) { gControllerStatuses[1].type = CONT_CONSOLE_GCN; __osGamecubeRumbleEnabled[1] = 1; }
    if (mode == 8) gRumblePakPfs.status = 0;
    if (mode == 9) gRumblePakPfs.channel = MAXCONTROLLERS;
    if (mode == 10) __osSiAccessQueueEnabled = 0;
    if (mode == 11) stale = 2;
    if (mode == 12) count = UINT32_MAX - 100;
    stop_rumble_for_reset();
    int attempts = starts;
    stop_rumble_for_reset();
    assert(sRumblePakResetStopped && sCharacterMovementRumbleVblanks == 0 && starts == attempts);
    if (mode == 5 || mode == 6) {
        assert(pending && !token && !released && !outerReleased);
    } else {
        assert(!pending && acquired == released && outerReleased == 1);
    }
    if (mode == 0 || mode == 4 || mode == 6 || mode == 11 || mode == 12) assert(starts == 2);
    if (mode == 3 || mode == 5) assert(starts == 1);
    if (mode == 1 || mode == 2 || mode == 7 || mode == 8 || mode == 9 || mode == 10) assert(!starts);
    if (mode == 7) assert(!__osGamecubeRumbleEnabled[1] && __osContLastCmd == CONT_CMD_END);
    return 0;
}
''', arguments=range(13))


if __name__ == "__main__":
    unittest.main()
