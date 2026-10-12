#include <ultra64.h>
#include <PR/os_internal_reg.h>
#include <PR/os_internal_rsp.h>
#include <PR/os_system.h>
#include <PR/os_vi.h>
#include <stdio.h>

#include "sm64.h"
#include "small_data.h"
#include "audio/external.h"
#include "debugger/assert.h"
#include "debugger/crash_screen.h"
#include "game/game_init.h"
#include "game/debug.h"
#include "game/memory.h"
#include "game/sound_init.h"
#include "buffers/buffers.h"
#include "segments.h"
#include "game/main.h"
#include "game/rumble_init.h"
#include "game/version.h"
#ifdef UNF
#include "usb/usb.h"
#include "usb/debug.h"
#endif
#include "game/puppyprint.h"
#include "game/profiling.h"
#include "game/emutest.h"

// Message IDs
enum MessageIDs {
    MESG_SP_COMPLETE = 100,
    MESG_DP_COMPLETE,
    MESG_VI_VBLANK,
    MESG_START_GFX_SPTASK,
    MESG_NMI_REQUEST,
    MESG_RCP_HUNG,
    MESG_START_AUDIO_SPTASK,
};

// OSThread gUnkThread; // unused?
OSThread gIdleThread;
OSThread gMainThread;
OSThread gGameLoopThread;
OSThread gSoundThread;

OSIoMesg gDmaIoMesg;
OSMesg gMainReceivedMesg GP_BSS(gMainReceivedMesg);

OSMesgQueue gDmaMesgQueue;
OSMesgQueue gSIEventMesgQueue;
OSMesgQueue gPIMesgQueue;
OSMesgQueue gIntrMesgQueue;
OSMesgQueue gSPTaskMesgQueue;

OSMesg gDmaMesgBuf[1];
OSMesg gPIMesgBuf[32];
OSMesg gSIEventMesgBuf[1];
OSMesg gIntrMesgBuf[16];
OSMesg gUnknownMesgBuf[16];

OSViMode VI;

struct Config gConfig GP_BSS(gConfig);

struct VblankHandler *gVblankHandler1       = NULL;
struct VblankHandler *gVblankHandler2       = NULL;
struct VblankHandler *gVblankHandler3       = NULL;
struct SPTask        *gActiveSPTask         = NULL;
struct SPTask        *sCurrentAudioSPTask   = NULL;
struct SPTask        *sCurrentDisplaySPTask = NULL;
struct SPTask        *sNextAudioSPTask      = NULL;
struct SPTask        *sNextDisplaySPTask    = NULL;
s8  gAudioEnabled      = TRUE;
u32 gNumVblanks GP_DATA(gNumVblanks) = 0;
s8 gResetTimer GP_DATA(gResetTimer) = 0;
s8 gNmiResetBarsTimer GP_DATA(gNmiResetBarsTimer) = 0;
s8 gDebugLevelSelect GP_DATA(gDebugLevelSelect) = FALSE;

// The SDK latches pre-NMI before attempting its nonblocking event delivery.
extern volatile u32 __osShutdown;
#define RESET_RSP_GRACE_US 100000
static s32 sResetCleanupStarted;
static u32 sResetCleanupStart;
#define RESET_WATCHDOG_INTERVAL_US 10000
static OSThread sResetWatchdogThread;
static u8 sResetWatchdogStack[0x800] ALIGNED16;
static OSTimer sResetWatchdogTimer;
static OSMesgQueue sResetWatchdogQueue;
static OSMesg sResetWatchdogMesg;
static s32 sResetWatchdogStarted;
static u32 sResetWatchdogStart;
static u32 sResetWatchdogVblanks;

s32 reset_pending(void) {
    return gResetTimer != 0 || __osShutdown != 0;
}

s32 reset_rsp_for_pre_nmi(void) {
    if (!reset_pending()) {
        return FALSE;
    }
    if (!sResetCleanupStarted) {
        sResetCleanupStart = osGetCount();
        sResetCleanupStarted = TRUE;
    }
    if (osAfterPreNMI() == 0) {
        return TRUE;
    }
    // A broken task cannot be allowed to keep its PC across the hardware reset.
    if ((u32) (osGetCount() - sResetCleanupStart) >= OS_USEC_TO_CYCLES(RESET_RSP_GRACE_US)) {
        __osSpSetStatus(SP_SET_HALT);
        return osAfterPreNMI() == 0;
    }
    return FALSE;
}

static s32 reset_watchdog_update(void) {
    if (!reset_pending()) {
        return FALSE;
    }
    if (!sResetWatchdogStarted || sResetWatchdogVblanks != gNumVblanks) {
        sResetWatchdogStarted = TRUE;
        sResetWatchdogStart = osGetCount();
        sResetWatchdogVblanks = gNumVblanks;
    }
    if ((u32) (osGetCount() - sResetWatchdogStart) < OS_USEC_TO_CYCLES(RESET_RSP_GRACE_US)) {
        return FALSE;
    }
    // A responsive scheduler keeps its reset animation; a stuck task loader is stopped.
    if (gMainThread.state == OS_STATE_RUNNING || gMainThread.state == OS_STATE_RUNNABLE
        || gMainThread.state == OS_STATE_WAITING) {
        osStopThread(&gMainThread);
    }
    if (gResetTimer == 0) {
        gResetTimer = 1;
        gNmiResetBarsTimer = 0;
    }
    stop_rumble_for_reset();
    __osSpSetStatus(SP_SET_HALT);
    return TRUE;
}

static void thread_reset_watchdog(UNUSED void *arg) {
    do {
        osRecvMesg(&sResetWatchdogQueue, NULL, OS_MESG_BLOCK);
    } while (!reset_watchdog_update());

    // Stay runnable above game/audio/debug work until NMI; none may restart devices.
    while (TRUE) {
        reset_rsp_for_pre_nmi();
    }
}

static void init_reset_watchdog(void) {
    osCreateMesgQueue(&sResetWatchdogQueue, &sResetWatchdogMesg, 1);
    osCreateThread(&sResetWatchdogThread, THREAD_10_RESET, thread_reset_watchdog, NULL,
                   sResetWatchdogStack + sizeof(sResetWatchdogStack), OS_PRIORITY_APPMAX);
    osSetTimer(&sResetWatchdogTimer, 0, OS_USEC_TO_CYCLES(RESET_WATCHDOG_INTERVAL_US),
               &sResetWatchdogQueue, NULL);
    osStartThread(&sResetWatchdogThread);
}

#ifdef VANILLA_DEBUG
s8 gShowDebugText GP_DATA(gShowDebugText) = FALSE;

// unused
UNUSED static u16 sDebugTextKeySequence[] = {
    D_JPAD, D_JPAD, U_JPAD, U_JPAD, L_JPAD, R_JPAD, L_JPAD, R_JPAD
};
static s16 sDebugTextKey = 0;
UNUSED void handle_debug_key_sequences(void) {
    if (gPlayer1Controller->buttonPressed != 0) {
        if (sDebugTextKeySequence[sDebugTextKey++] == gPlayer1Controller->buttonPressed) {
            if (sDebugTextKey == ARRAY_COUNT(sDebugTextKeySequence)) {
                sDebugTextKey = 0;
                gShowDebugText ^= 1;
            }
        } else {
            sDebugTextKey = 0;
        }
    }
}
#endif

void setup_mesg_queues(void) {
    osCreateMesgQueue(&gDmaMesgQueue, gDmaMesgBuf, ARRAY_COUNT(gDmaMesgBuf));
    osCreateMesgQueue(&gSIEventMesgQueue, gSIEventMesgBuf, ARRAY_COUNT(gSIEventMesgBuf));
    osSetEventMesg(OS_EVENT_SI, &gSIEventMesgQueue, NULL);

    osCreateMesgQueue(&gSPTaskMesgQueue, gUnknownMesgBuf, ARRAY_COUNT(gUnknownMesgBuf));
    osCreateMesgQueue(&gIntrMesgQueue, gIntrMesgBuf, ARRAY_COUNT(gIntrMesgBuf));
    osViSetEvent(&gIntrMesgQueue, (OSMesg) MESG_VI_VBLANK, 1);

    osSetEventMesg(OS_EVENT_SP, &gIntrMesgQueue, (OSMesg) MESG_SP_COMPLETE);
    osSetEventMesg(OS_EVENT_DP, &gIntrMesgQueue, (OSMesg) MESG_DP_COMPLETE);
    osSetEventMesg(OS_EVENT_PRENMI, &gIntrMesgQueue, (OSMesg) MESG_NMI_REQUEST);
}

void alloc_pool(void) {
    void *start = (void *) SEG_POOL_START;
    void *end = (void *) (SEG_POOL_START + POOL_SIZE);

    main_pool_init(start, end);
    gEffectsMemoryPool = mem_pool_init(EFFECTS_MEMORY_POOL, MEMORY_POOL_LEFT);
}

void create_thread(OSThread *thread, OSId id, void (*entry)(void *), void *arg, void *sp, OSPri pri) {
    thread->next = NULL;
    thread->queue = NULL;
    osCreateThread(thread, id, entry, arg, sp, pri);
}

#ifdef VERSION_SH
extern void func_sh_802f69cc(void);
#endif

void handle_nmi_request(void) {
    gResetTimer = 1;
    gNmiResetBarsTimer = 0;
    stop_rumble_for_reset();
    stop_sounds_in_continuous_banks();
    sound_banks_disable(SEQ_PLAYER_SFX, SOUND_BANKS_BACKGROUND);
    fadeout_music(90);
#ifdef VERSION_SH
    func_sh_802f69cc();
#endif
}

void redirect_pre_nmi_to_crash(OSMesgQueue *queue) {
    OSMesg msg;
    OSIntMask mask = osSetIntMask(OS_IM_NONE);

    osSetEventMesg(OS_EVENT_PRENMI, queue, NULL);
    // The stopped scheduler cannot drain its queue, including an earlier reset request.
    while (osRecvMesg(&gIntrMesgQueue, &msg, OS_MESG_NOBLOCK) == 0) {
        if (msg == (OSMesg) MESG_NMI_REQUEST) {
            osSendMesg(queue, NULL, OS_MESG_NOBLOCK);
        }
    }
    osSetIntMask(mask);
}

void receive_new_tasks(void) {
    struct SPTask *spTask;

    while (osRecvMesg(&gSPTaskMesgQueue, (OSMesg *) &spTask, OS_MESG_NOBLOCK) != -1) {
        spTask->state = SPTASK_STATE_NOT_STARTED;
        switch (spTask->task.t.type) {
            case M_AUDTASK:
                sNextAudioSPTask = spTask;
                break;
            case M_GFXTASK:
                sNextDisplaySPTask = spTask;
                break;
        }
    }

    if (sCurrentAudioSPTask == NULL && sNextAudioSPTask != NULL) {
        sCurrentAudioSPTask = sNextAudioSPTask;
        sNextAudioSPTask = NULL;
    }

    if (sCurrentDisplaySPTask == NULL && sNextDisplaySPTask != NULL) {
        sCurrentDisplaySPTask = sNextDisplaySPTask;
        sNextDisplaySPTask = NULL;
    }
}

void start_sptask(s32 taskType) {
    // Pre-NMI must leave the RSP halted for the boot code.
    if (reset_pending()) {
        return;
    }
    if (taskType == M_AUDTASK) {
        gActiveSPTask = sCurrentAudioSPTask;
    } else {
        gActiveSPTask = sCurrentDisplaySPTask;
    }

    osSpTaskLoad(&gActiveSPTask->task);
    if (reset_pending()) {
        return;
    }
    osSpTaskStartGo(&gActiveSPTask->task);
    gActiveSPTask->state = SPTASK_STATE_RUNNING;
}

void interrupt_gfx_sptask(void) {
    if (gActiveSPTask->task.t.type == M_GFXTASK) {
        gActiveSPTask->state = SPTASK_STATE_INTERRUPTED;
        osSpTaskYield();
    }
}

void start_gfx_sptask(void) {
    if (gActiveSPTask == NULL
     && sCurrentDisplaySPTask != NULL
     && sCurrentDisplaySPTask->state == SPTASK_STATE_NOT_STARTED) {
        start_sptask(M_GFXTASK);
        profiler_rsp_started(PROFILER_RSP_GFX);
    }
}

void pretend_audio_sptask_done(void) {
    gActiveSPTask = sCurrentAudioSPTask;
    gActiveSPTask->state = SPTASK_STATE_RUNNING;
    osSendMesg(&gIntrMesgQueue, (OSMesg) MESG_SP_COMPLETE, OS_MESG_NOBLOCK);
}

#if AUDIO_IMMEDIATE_TASKS
static void start_audio_task_if_ready(void) {
    if (sCurrentAudioSPTask == NULL || sCurrentAudioSPTask->state != SPTASK_STATE_NOT_STARTED) {
        return;
    }
    if (gActiveSPTask != NULL) {
        if (gActiveSPTask->task.t.type == M_GFXTASK
            && gActiveSPTask->state != SPTASK_STATE_INTERRUPTED) {
            interrupt_gfx_sptask();
        }
        return;
    }
    if (gAudioEnabled) {
        start_sptask(M_AUDTASK);
    } else {
        pretend_audio_sptask_done();
    }
    profiler_rsp_started(PROFILER_RSP_AUDIO);
}
#endif

void handle_vblank(void) {
    gNumVblanks++;
    if (reset_pending()) {
        if (gResetTimer == 0) {
            handle_nmi_request();
        }
        reset_rsp_for_pre_nmi();
        if (gResetTimer < 100) {
            gResetTimer++;
        }
    } else {
        receive_new_tasks();

#if AUDIO_IMMEDIATE_TASKS
        start_audio_task_if_ready();
        if (sCurrentAudioSPTask == NULL) {
            start_gfx_sptask();
        }
#else
        // First try to kick off an audio task. If the gfx task is currently
        // running, we need to asynchronously interrupt it -- handle_sp_complete
        // will pick up on what we're doing and start the audio task for us.
        // If there is already an audio task running, there is nothing to do.
        // If there is no audio task available, try a gfx task instead.
        if (sCurrentAudioSPTask != NULL) {
            if (gActiveSPTask != NULL) {
                interrupt_gfx_sptask();
            } else {
                if (gAudioEnabled) {
                    start_sptask(M_AUDTASK);
                } else {
                    pretend_audio_sptask_done();
                }
                profiler_rsp_started(PROFILER_RSP_AUDIO);
            }
        } else {
            if (gActiveSPTask == NULL && sCurrentDisplaySPTask != NULL
                && sCurrentDisplaySPTask->state != SPTASK_STATE_FINISHED) {
                start_sptask(M_GFXTASK);
                profiler_rsp_started(PROFILER_RSP_GFX);
            }
        }
#endif
    }
    rumble_thread_update_vi();

    // Notify the game loop about the vblank.
    if (gVblankHandler1 != NULL) {
        osSendMesg(gVblankHandler1->queue, gVblankHandler1->msg, OS_MESG_NOBLOCK);
    }
    if (gVblankHandler2 != NULL) {
        osSendMesg(gVblankHandler2->queue, gVblankHandler2->msg, OS_MESG_NOBLOCK);
    }
    if (gVblankHandler3 != NULL) {
        osSendMesg(gVblankHandler3->queue, gVblankHandler3->msg, OS_MESG_NOBLOCK);
    }
}

void handle_sp_complete(void) {
    struct SPTask *curSPTask = gActiveSPTask;

    gActiveSPTask = NULL;

    if (curSPTask->state == SPTASK_STATE_INTERRUPTED) {
        // handle_vblank tried to start an audio task while there was already a gfx task
        // running, so it had to interrupt the gfx task. That interruption just finished.
        if (osSpTaskYielded(&curSPTask->task) == 0) {
            // The gfx task completed before we had time to interrupt it.
            // Mark it finished, just like below.
            curSPTask->state = SPTASK_STATE_FINISHED;
            profiler_rsp_completed(PROFILER_RSP_GFX);
        } else {
            profiler_rsp_yielded();
        }

        // Start the audio task, as expected by handle_vblank.
        if (gAudioEnabled) {
            start_sptask(M_AUDTASK);
        } else {
            pretend_audio_sptask_done();
        }
        profiler_rsp_started(PROFILER_RSP_AUDIO);
    } else {
        curSPTask->state = SPTASK_STATE_FINISHED;
        if (curSPTask->task.t.type == M_AUDTASK) {
            profiler_rsp_completed(PROFILER_RSP_AUDIO);
#if defined(VERSION_JP) || defined(VERSION_US)
            if (!audio_complete_frame_task(curSPTask)) {
                curSPTask->msgqueue = NULL;
            }
#endif
            // After audio tasks come gfx tasks.
            if ((sCurrentDisplaySPTask != NULL)
             && (sCurrentDisplaySPTask->state != SPTASK_STATE_FINISHED)) {
                if (sCurrentDisplaySPTask->state == SPTASK_STATE_INTERRUPTED) {
                    profiler_rsp_resumed();
                } else {
                    profiler_rsp_started(PROFILER_RSP_GFX);
                }
                start_sptask(M_GFXTASK);
            }
            sCurrentAudioSPTask = NULL;
            if (curSPTask->msgqueue != NULL) {
                osSendMesg(curSPTask->msgqueue, curSPTask->msg, OS_MESG_NOBLOCK);
            }
        } else {
            // The SP process is done, but there is still a Display Processor notification
            // that needs to arrive before we can consider the task completely finished and
            // null out sCurrentDisplaySPTask. That happens in handle_dp_complete.
            profiler_rsp_completed(PROFILER_RSP_GFX);
        }
    }
}

void handle_dp_complete(void) {
    // FullSync means the RDP has finished every write to this task's framebuffer.
    // Request its next-retrace presentation before waking the game loop.
    if (sCurrentDisplaySPTask->presentOnComplete && !reset_pending()) {
        game_present_completed_frame(sCurrentDisplaySPTask->framebuffer);
    }
    if (sCurrentDisplaySPTask->msgqueue != NULL) {
        osSendMesg(sCurrentDisplaySPTask->msgqueue, sCurrentDisplaySPTask->msg, OS_MESG_NOBLOCK);
    }
    sCurrentDisplaySPTask->state = SPTASK_STATE_FINISHED_DP;
    sCurrentDisplaySPTask = NULL;
}

OSTimerEx RCPHangTimer;
void start_rcp_hang_timer(void) {
    if (RCPHangTimer.started == FALSE) {
        osSetTimer(&RCPHangTimer.timer, OS_USEC_TO_CYCLES(3000000), (OSTime) 0, &gIntrMesgQueue, (OSMesg) MESG_RCP_HUNG);
        RCPHangTimer.started = TRUE;
    }
}

void stop_rcp_hang_timer(void) {
    osStopTimer(&RCPHangTimer.timer);
    RCPHangTimer.started = FALSE;
}

void alert_rcp_hung_up(void) {
    if (gActiveSPTask && gActiveSPTask->task.t.type == M_AUDTASK) {
        error("Timeout while waiting for hardware SP\n"
              "task! This was probably caused by a broken\n"
              "command somewhere in the audio pipeline.\n\n"
              "RCP is HUNG UP!! Oh! MY GOD!!"
        );
    } else {
        error("Timeout while waiting for hardware SP\n"
              "task! This was probably caused by a broken\n"
              "display list somewhere (i.e. bad model data).\n\n"
              "RCP is HUNG UP!! Oh! MY GOD!!"
        );
    }
}

/**
 * Increment the first and last values of the stack.
 * If they're different, that means an error has occured, so trigger a crash.
*/
#ifdef DEBUG_ASSERTIONS
void check_stack_validity(void) {
    gIdleThreadStack[0]++;
    gIdleThreadStack[THREAD1_STACK - 1]++;
    assertf(gIdleThreadStack[0] == gIdleThreadStack[THREAD1_STACK - 1], "Thread 1 stack overflow.");
    gThread3Stack[0]++;
    gThread3Stack[THREAD3_STACK - 1]++;
    assertf(gThread3Stack[0] == gThread3Stack[THREAD3_STACK - 1], "Thread 3 stack overflow.");
    gThread4Stack[0]++;
    gThread4Stack[THREAD4_STACK - 1]++;
    assertf(gThread4Stack[0] == gThread4Stack[THREAD4_STACK - 1], "Thread 4 stack overflow.");
    gThread5Stack[0]++;
    gThread5Stack[THREAD5_STACK - 1]++;
    assertf(gThread5Stack[0] == gThread5Stack[THREAD5_STACK - 1], "Thread 5 stack overflow.");
#ifdef ENABLE_RUMBLE
    gThread6Stack[0]++;
    gThread6Stack[THREAD6_STACK - 1]++;
    assertf(gThread6Stack[0] == gThread6Stack[THREAD6_STACK - 1], "Thread 6 stack overflow.");
#endif
}
#endif


extern OSViMode VI;
void thread3_main(UNUSED void *arg) {
    setup_mesg_queues();
    alloc_pool();
    load_engine_code_segment();
    gEmulator = detect_emulator();
#ifndef UNF
    crash_screen_init();
#endif

#ifdef UNF
    debug_initialize();
#endif

#ifdef DEBUG
    osSyncPrintf("Super Mario 64\n");
#if 0 // if your PC username isn't your real name feel free to uncomment
    osSyncPrintf("Built by: %s\n", __username__);
#endif
    osSyncPrintf("Compiler: %s\n", __compiler__);
    osSyncPrintf("Linker  : %s\n", __linker__);
#endif

    if (!(gEmulator & EMU_CONSOLE)) {
        gBorderHeight = BORDER_HEIGHT_EMULATOR;
#ifdef RCVI_HACK
        VI.comRegs.vSync = 525*20;   
        change_vi(&VI, SCREEN_WIDTH, SCREEN_HEIGHT);
        osViSetMode(&VI);
        osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);
        osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
#endif
    } else {
        gBorderHeight = BORDER_HEIGHT_CONSOLE;
    }
#ifdef DEBUG_ASSERTIONS
    gIdleThreadStack[0] = 0;
    gIdleThreadStack[THREAD1_STACK - 1] = 0;
    gThread3Stack[0] = 0;
    gThread3Stack[THREAD3_STACK - 1] = 0;
    gThread4Stack[0] = 0;
    gThread4Stack[THREAD4_STACK - 1] = 0;
    gThread5Stack[0] = 0;
    gThread5Stack[THREAD5_STACK - 1] = 0;
#ifdef ENABLE_RUMBLE
    gThread6Stack[0] = 0;
    gThread6Stack[THREAD6_STACK - 1] = 0;
#endif
#endif

    create_thread(&gSoundThread, THREAD_4_SOUND, thread4_sound, NULL, gThread4Stack + THREAD4_STACK, 20);
    osStartThread(&gSoundThread);

    create_thread(&gGameLoopThread, THREAD_5_GAME_LOOP, thread5_game_loop, NULL, gThread5Stack + THREAD5_STACK, 10);
    osStartThread(&gGameLoopThread);

    while (TRUE) {
        OSMesg msg;
        osRecvMesg(&gIntrMesgQueue, &msg, OS_MESG_BLOCK);
#ifdef DEBUG_ASSERTIONS
        check_stack_validity();
#endif
        switch ((uintptr_t) msg) {
            case MESG_VI_VBLANK:
                handle_vblank();
                break;
            case MESG_SP_COMPLETE:
                handle_sp_complete();
                break;
            case MESG_DP_COMPLETE:
                stop_rcp_hang_timer();
                handle_dp_complete();
                break;
            case MESG_START_GFX_SPTASK:
                start_rcp_hang_timer();
                start_gfx_sptask();
                break;
#if AUDIO_IMMEDIATE_TASKS
            case MESG_START_AUDIO_SPTASK:
                receive_new_tasks();
                start_audio_task_if_ready();
                break;
#endif
            case MESG_NMI_REQUEST:
                handle_nmi_request();
                break;
            case MESG_RCP_HUNG:
                alert_rcp_hung_up();
                break;
        }
    }
}

void set_vblank_handler(s32 index, struct VblankHandler *handler, OSMesgQueue *queue, OSMesg *msg) {
    handler->queue = queue;
    handler->msg = msg;

    switch (index) {
        case 1:
            gVblankHandler1 = handler;
            break;
        case 2:
            gVblankHandler2 = handler;
            break;
        case 3:
            gVblankHandler3 = handler;
            break;
    }
}

void send_sp_task_message(OSMesg *msg) {
    osWritebackDCacheAll();
    osSendMesg(&gSPTaskMesgQueue, msg, OS_MESG_NOBLOCK);
}

s32 dispatch_audio_sptask(struct SPTask *spTask) {
    if (!gAudioEnabled || spTask == NULL) {
        return FALSE;
    }
    osWritebackDCacheAll();
    if (osSendMesg(&gSPTaskMesgQueue, spTask, OS_MESG_NOBLOCK) != 0) {
        return FALSE;
    }
#if AUDIO_IMMEDIATE_TASKS
    // A full interrupt queue falls back to receiving the task at VBlank.
    osSendMesg(&gIntrMesgQueue, (OSMesg) MESG_START_AUDIO_SPTASK, OS_MESG_NOBLOCK);
#endif
    return TRUE;
}

void exec_display_list(struct SPTask *spTask) {
    if (spTask != NULL) {
        osWritebackDCacheAll();
        spTask->state = SPTASK_STATE_NOT_STARTED;
        if (sCurrentDisplaySPTask == NULL) {
            sCurrentDisplaySPTask = spTask;
            sNextDisplaySPTask = NULL;
            osSendMesg(&gIntrMesgQueue, (OSMesg) MESG_START_GFX_SPTASK, OS_MESG_NOBLOCK);
        } else {
            sNextDisplaySPTask = spTask;
        }
    }
}

void turn_on_audio(void) {
    gAudioEnabled = TRUE;
}

void turn_off_audio(void) {
    gAudioEnabled = FALSE;
    while (sCurrentAudioSPTask != NULL) {
        ;
    }
}

void change_vi(OSViMode *mode, int width, int height) {
    mode->comRegs.width  = width;
    mode->comRegs.xScale = ((width * 512) / 320);
    if (height > 240) {
        mode->comRegs.ctrl     |= 0x40;
        mode->fldRegs[0].origin = (width * 2);
        mode->fldRegs[1].origin = (width * 4);
        mode->fldRegs[0].yScale = (0x2000000 | ((height * 1024) / 240));
        mode->fldRegs[1].yScale = (0x2000000 | ((height * 1024) / 240));
        mode->fldRegs[0].vStart = (mode->fldRegs[1].vStart - 0x20002);
    } else {
        mode->fldRegs[0].origin = (width * 2);
        mode->fldRegs[1].origin = (width * 4);
        mode->fldRegs[0].yScale = ((height * 1024) / 240);
        mode->fldRegs[1].yScale = ((height * 1024) / 240);
    }
}

// Convert VI counts to nominal seconds without relying on the emulator's CPU timer scale.
f32 get_vi_refresh_rate(void) {
    f32 vi_clock;

    switch (gConfig.tvType) {
        case MODE_PAL:
            vi_clock = 49656530.0f;
            break;
        case MODE_MPAL:
            vi_clock = 48628322.0f;
            break;
        default:
            vi_clock = 48681818.0f;
            break;
    }

    // VI horizontal timing uses the low 12 bits; vSync counts half-lines.
    return (2.0f * vi_clock) / (((VI.comRegs.hSync & 0xfff) + 1) * (VI.comRegs.vSync + 1));
}

void get_audio_frequency(void) {
    switch (gConfig.tvType) {
#if defined(VERSION_JP) || defined(VERSION_US)
    case MODE_NTSC: gConfig.audioFrequency = 1.0f;    break;
    case MODE_MPAL: gConfig.audioFrequency = 0.9915f; break;
    case MODE_PAL:  gConfig.audioFrequency = 0.9876f; break;
#else
    case MODE_NTSC: gConfig.audioFrequency = 1.0126f; break;
    case MODE_MPAL: gConfig.audioFrequency = 1.0086f; break;
    case MODE_PAL:  gConfig.audioFrequency = 1.0f;    break;
#endif
    }
}

/**
 * Initialize hardware, start main thread, then idle.
 */
void thread1_idle(UNUSED void *arg) {
    osCreateViManager(OS_PRIORITY_VIMGR);
    switch (osTvType) {
        case OS_TV_NTSC:
            //osViSetMode(&osViModeTable[OS_VI_NTSC_LAN1]);
            VI = osViModeTable[OS_VI_NTSC_LAN1];
            gConfig.tvType = MODE_NTSC;
            break;
        case OS_TV_MPAL:
            //osViSetMode(&osViModeTable[OS_VI_MPAL_LAN1]);
            VI = osViModeTable[OS_VI_NTSC_LAN1];
            gConfig.tvType = MODE_MPAL;
            break;
        case OS_TV_PAL:
            //osViSetMode(&osViModeTable[OS_VI_PAL_LAN1]);
            VI = osViModeTable[OS_VI_NTSC_LAN1];
            gConfig.tvType = MODE_PAL;
            break;
    }
    get_audio_frequency();
    change_vi(&VI, SCREEN_WIDTH, SCREEN_HEIGHT);
    osViSetMode(&VI);
    osViBlack(TRUE);
    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
    osCreatePiManager(OS_PRIORITY_PIMGR, &gPIMesgQueue, gPIMesgBuf, ARRAY_COUNT(gPIMesgBuf));
    init_reset_watchdog();
    create_thread(&gMainThread, THREAD_3_MAIN, thread3_main, NULL, gThread3Stack + THREAD3_STACK, 100);
    osStartThread(&gMainThread);

    osSetThreadPri(NULL, 0);

    // halt
    while (TRUE) {
        ;
    }
}

// Clear RAM on boot
void ClearRAM(void) {
    bzero(_mainSegmentEnd, (size_t)osMemSize - (size_t)OS_K0_TO_PHYSICAL(_mainSegmentEnd));
}

#ifdef ISVPRINT
extern u32 gISVDbgPrnAdrs;
extern u32 gISVFlag;

void osInitialize_fakeisv() {
    /* global flag to skip `__checkHardware_isv` from being called. */
    gISVFlag = 0x49533634;  // 'IS64'

    /* printf writes go to this address, cen64(1) has this hardcoded. */
    gISVDbgPrnAdrs = 0x13FF0000;

    /* `__printfunc`, used by `osSyncPrintf` will be set. */
    __osInitialize_isv();
}
#endif

void main_func(void) {
    ClearRAM();
    __osInitialize_common();
#ifdef ISVPRINT
    osInitialize_fakeisv();
#endif
    create_thread(&gIdleThread, THREAD_1_IDLE, thread1_idle, NULL, gIdleThreadStack + THREAD1_STACK, 100);
    osStartThread(&gIdleThread);
}
