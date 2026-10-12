"""Production audio ownership, DMA fencing, immediate scheduling and retry checks."""

import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

from regression_support import Source

ROOT = Path(__file__).resolve().parents[1]
COMMON = r'''
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
typedef int32_t s32;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint8_t u8;
typedef int16_t s16;
typedef void *OSMesg;
typedef int OSMesgQueue;
#define TRUE 1
#define FALSE 0
#define UNUSED __attribute__((unused))
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define OS_MESG_BLOCK 1
#define OS_MESG_NOBLOCK 0
#define NUMAIBUFFERS 3
#define M_AUDTASK 2
#define M_GFXTASK 1
#define AUDIO_FRAME_DMA_QUEUE_SIZE 64
#define AUDIO_LOCK_NOT_LOADING 1
#define AUDIO_LOCK_LOADING 2
#define AUDIO_LOCK_UNINITIALIZED 0
#define VERSION_US 1
#define SAMPLES_TO_OVERPRODUCE 16
#define EXTRA_BUFFERED_AI_SAMPLES_TARGET 64
#define AI_STATUS_REG 0
#define AI_STATUS_FIFO_FULL 0x80000000u
#define AI_STATUS_DMA_BUSY 0x40000000u
#define SPTASK_STATE_NOT_STARTED 0
#define SPTASK_STATE_RUNNING 1
#define SPTASK_STATE_INTERRUPTED 2
#define SPTASK_STATE_FINISHED 3
#define PROFILER_RSP_AUDIO 1
#define PROFILER_RSP_GFX 2
#define MESG_START_AUDIO_SPTASK 106
#define MESG_SP_COMPLETE 100
#define assertf(condition, ...) assert(condition)
#define stubbed_printf(...) ((void)0)
static UNUSED void audio_options_output_reset(void) {}
static UNUSED void audio_options_output_begin(s32 index) { assert(index >= 0 && index < NUMAIBUFFERS); }
static UNUSED void audio_options_output_submit(s32 index) { assert(index >= 0 && index < NUMAIBUFFERS); }
typedef struct {
    s32 type, flags, ucode_boot_size, ucode_size, ucode_data_size;
    void *ucode_boot, *ucode, *ucode_data, *dram_stack, *output_buff, *output_buff_size;
    void *data_ptr, *yield_data_ptr;
    s32 dram_stack_size, data_size, yield_data_size;
} OSTask_t;
struct SPTask {
    struct { OSTask_t t; } task;
    OSMesgQueue *msgqueue;
    OSMesg msg;
    s32 state;
};
'''
PIPELINE = r'''
static s16 storage[3][1280];
static s16 *gAiBuffers[3] = {storage[0], storage[1], storage[2]}, *gCurrAiBuffer;
static s16 gAiBufferLengths[3];
static struct SPTask gAudioTasks[2], *gAudioTask;
static u64 commands[2][128], *gAudioCmdBuffers[2] = {commands[0],commands[1]}, *gAudioCmd;
static u8 rspbootTextStart[16], aspMainTextStart[4096], aspMainDataStart[704];
#define rspbootTextEnd (rspbootTextStart+16)
#define aspMainDataEnd (aspMainDataStart+704)
static s32 gAudioFrameCount, gAudioTaskIndex, gCurrAiBufferIndex, gCurrAudioFrameDmaCount;
static s32 gAudioLoadLock = AUDIO_LOCK_NOT_LOADING, gAudioEnabled = TRUE;
static s32 gSamplesPerFrameTarget = 544, gMinAiBufferLength = 528, gMaxAudioCmds = 128;
static s32 gAiFrequency = 32006;
static u8 gEmulator;
static s32 sGameLoopTicked = 1, soundUpdates, syntheses, waits, ttls, requestedDmas;
static u32 gAudioRandom;
static OSMesgQueue gCurrAudioFrameDmaQueue;
static int hardwareQueued, aiSamples, busy, submits, submitted[16];
static u32 UNUSED IO_READ(int reg) {
    assert(reg == AI_STATUS_REG);
    return hardwareQueued == 2 ? AI_STATUS_FIFO_FULL : hardwareQueued ? AI_STATUS_DMA_BUSY : 0;
}
static s32 osAiGetLength(void) { return aiSamples * 4; }
static s32 osAiSetNextBuffer(void *buffer, int bytes) {
    assert(bytes > 0 && bytes <= 2240);
    if (busy || hardwareQueued == 2) return -1;
    for (int i = 0; i < 3; i++) if (buffer == gAiBuffers[i]) submitted[submits++] = i;
    hardwareQueued++;
    aiSamples = 100;
    return 0;
}
static void update_game_sound(void) { soundUpdates++; }
static u64 *synthesis_execute(u64 *cmd, s32 *written, s16 *buffer, int samples) {
    assert(buffer == gAiBuffers[gCurrAiBufferIndex]);
    assert(samples >= gMinAiBufferLength && samples <= gSamplesPerFrameTarget + 16);
    syntheses++;
    gCurrAudioFrameDmaCount = requestedDmas;
    *written = 1;
    return cmd+1;
}
static int osRecvMesg(OSMesgQueue *queue, OSMesg *msg, int mode) {
    assert(queue == &gCurrAudioFrameDmaQueue && msg == NULL && mode == OS_MESG_BLOCK);
    assert(waits < requestedDmas);
    waits++;
    return 0;
}
static void decrease_sample_dma_ttls(void) { ttls++; }
#include "actual.inc"
int main(void) {
    requestedDmas = 2;
    struct SPTask *first = create_next_audio_frame_task();
    assert(first && waits == 2 && gCurrAudioFrameDmaCount == 0);
    assert(first->msg == (OSMesg)2 && first->msgqueue == NULL);
    assert(sAudioBufferStates[1] == AUDIO_BUFFER_RENDERING && submits == 0);
    // A delayed RSP owns both sample-DMA and command storage until completion.
    int slot = gAudioTaskIndex;
    assert(create_next_audio_frame_task() == NULL);
    assert(gAudioTaskIndex == slot && syntheses == 1 && ttls == 1);
    assert(audio_complete_frame_task(first));
    assert(sAudioBufferStates[1] == AUDIO_BUFFER_READY && submits == 0);
    audio_submit_late_frame_buffer();
    assert(submits == 1 && sAudioBufferStates[1] == AUDIO_BUFFER_QUEUED);
    audio_submit_late_frame_buffer();
    assert(submits == 1);
    requestedDmas = 0;
    struct SPTask *second = create_next_audio_frame_task();
    assert(second && submitted[0] == 1 && submits == 1);
    assert(sAudioBufferStates[1] == AUDIO_BUFFER_QUEUED);
    assert(soundUpdates == 1 && syntheses == 2);
    audio_complete_frame_task(second);
    busy = 1;
    struct SPTask *third = create_next_audio_frame_task();
    assert(third && submits == 1 && sAudioSubmitIndex == 2);
    audio_complete_frame_task(third);
    // Busy output retains order; no reuse of any of the three owned buffers.
    assert(create_next_audio_frame_task() == NULL && submits == 1);
    assert(syntheses == 3 && sAudioBufferStates[2] == AUDIO_BUFFER_READY);
    busy = 0;
    assert(create_next_audio_frame_task() == NULL);
    assert(submits == 2 && submitted[1] == 2 && sAudioSubmitIndex == 0);
    // Consume only the oldest entry; the other AI buffer remains protected.
    hardwareQueued = 1;
    struct SPTask *fourth = create_next_audio_frame_task();
    assert(fourth && submits == 3 && submitted[2] == 0);
    assert(sAudioBufferStates[2] == AUDIO_BUFFER_QUEUED);
    assert(sAudioBufferStates[0] == AUDIO_BUFFER_QUEUED);
    assert(sAudioBufferStates[1] == AUDIO_BUFFER_RENDERING);
    audio_complete_frame_task(fourth);
    hardwareQueued = 0;
    audio_reclaim_played_buffers();
    assert(sAudioBufferStates[2] == AUDIO_BUFFER_FREE);
    assert(sAudioBufferStates[0] == AUDIO_BUFFER_FREE);
    assert(sAudioBufferStates[1] == AUDIO_BUFFER_READY);
    // Duplicate/invalid completion must not release a queued or free buffer.
    struct SPTask stale = {.msg=(OSMesg)3};
    audio_complete_frame_task(&stale);
    assert(sAudioBufferStates[2] == AUDIO_BUFFER_FREE);
    stale.msg = (OSMesg)4;
    audio_complete_frame_task(&stale);
    gAudioEnabled = FALSE;
    assert(create_next_audio_frame_task() == NULL);
    assert(syntheses == 4 && submits == 4); // Ready output can drain while disabled.
    gAudioLoadLock = 0;
    assert(create_next_audio_frame_task() == NULL && syntheses == 4);
    return 0;
}
'''
SCHEDULER = r'''
static struct SPTask *sCurrentAudioSPTask, *sCurrentDisplaySPTask, *gActiveSPTask;
static s32 gAudioEnabled = TRUE, gResetTimer;
static u32 sdkShutdown;
static int reset_pending(void) { return gResetTimer != 0 || sdkShutdown != 0; }
static OSMesgQueue gIntrMesgQueue, gSPTaskMesgQueue;
static int yields, loads, starts, completions, writebacks, sends, failQueue, yielded=1, lateCompletion;
static void osSpTaskLoad(void *task) { assert(task); loads++; }
static void osSpTaskStartGo(void *task) { assert(task); starts++; }
static void osSpTaskYield(void) { yields++; }
static int osSpTaskYielded(void *task) { assert(task); return yielded; }
static void osWritebackDCacheAll(void) { writebacks++; }
static int osSendMesg(OSMesgQueue *q, OSMesg msg, int mode) {
    assert(mode == OS_MESG_NOBLOCK); sends++;
    if (q == &gSPTaskMesgQueue) return failQueue ? -1 : 0;
    assert(q == &gIntrMesgQueue && (msg==(OSMesg)MESG_START_AUDIO_SPTASK || msg==(OSMesg)MESG_SP_COMPLETE));
    return 0;
}
static void profiler_rsp_started(int kind) { assert(kind==1 || kind==2); }
static void profiler_rsp_completed(int kind) { assert(kind==1 || kind==2); }
static void profiler_rsp_resumed(void) {}
static void profiler_rsp_yielded(void) {}
static s32 audio_complete_frame_task(struct SPTask *task) { assert(task==sCurrentAudioSPTask); completions++; return lateCompletion; }
#include "actual.inc"
int main(void) {
    struct SPTask audio={.task.t.type=M_AUDTASK}, gfx={.task.t.type=M_GFXTASK};
    sCurrentAudioSPTask=&audio;
    start_audio_task_if_ready();
    assert(gActiveSPTask==&audio && loads==1 && starts==1);
    start_audio_task_if_ready();
    assert(starts==1 && yields==0); // Repeated VBlank/wakeup never restarts audio.
    handle_sp_complete();
    assert(completions==1 && sCurrentAudioSPTask==NULL && gActiveSPTask==NULL);
    audio.state=SPTASK_STATE_NOT_STARTED;
    sCurrentAudioSPTask=&audio;
    sCurrentDisplaySPTask=&gfx;
    gActiveSPTask=&gfx; gfx.state=SPTASK_STATE_RUNNING;
    start_audio_task_if_ready();
    start_audio_task_if_ready();
    assert(yields==1 && gfx.state==SPTASK_STATE_INTERRUPTED);
    handle_sp_complete();
    assert(gActiveSPTask==&audio && starts==2);
    handle_sp_complete();
    assert(completions==2 && gActiveSPTask==&gfx && gfx.state==SPTASK_STATE_RUNNING);
    // Graphics finishing during the yield must not restart finished graphics.
    audio.state=SPTASK_STATE_NOT_STARTED; sCurrentAudioSPTask=&audio;
    start_audio_task_if_ready(); yielded=0;
    handle_sp_complete(); handle_sp_complete();
    assert(completions==3 && gfx.state==SPTASK_STATE_FINISHED && gActiveSPTask==NULL);
    // Only an overdue completion sends a callback to the sound thread.
    lateCompletion=1; audio.state=SPTASK_STATE_NOT_STARTED;
    audio.msgqueue=&gIntrMesgQueue; audio.msg=(OSMesg)MESG_SP_COMPLETE;
    sCurrentAudioSPTask=&audio;
    start_audio_task_if_ready(); handle_sp_complete();
    assert(completions==4 && sends==1 && audio.msgqueue==&gIntrMesgQueue);
    int previousSends=sends;
    failQueue=1;
    assert(!dispatch_audio_sptask(&audio) && sends==previousSends+1);
    failQueue=0;
    assert(dispatch_audio_sptask(&audio) && sends==previousSends+3 && writebacks==2);
    assert(!dispatch_audio_sptask(NULL) && sends==previousSends+3);
    return 0;
}
'''
ROUTING = r'''
#include <setjmp.h>
static OSMesgQueue sSoundMesgQueue;
static OSMesg sSoundMesgBuf[AUDIO_IMMEDIATE_TASKS ? 4 : 1];
static int sSoundVblankHandler, gResetTimer;
static int cursor, frames, dispatches, starts, ends, lateSubmissions;
static struct SPTask task;
static jmp_buf done;
static void audio_init(void) {}
static void sound_init(void) {}
static void osCreateMesgQueue(OSMesgQueue *q, OSMesg *buf, int n) {
    assert(q==&sSoundMesgQueue && buf==sSoundMesgBuf && n==(AUDIO_IMMEDIATE_TASKS ? 4 : 1));
}
static void set_vblank_handler(int i, int *h, OSMesgQueue *q, OSMesg msg) {
    assert(i==1 && h==&sSoundVblankHandler && q==&sSoundMesgQueue && msg==(OSMesg)512);
}
static void osRecvMesg(OSMesgQueue *q, OSMesg *msg, int mode) {
    assert(q==&sSoundMesgQueue && mode==OS_MESG_BLOCK);
    if (cursor==(AUDIO_IMMEDIATE_TASKS ? 4 : 3)) longjmp(done,1);
    *msg=(AUDIO_IMMEDIATE_TASKS && cursor==0) ? (OSMesg)2 : (OSMesg)512;
    cursor++;
}
static struct SPTask *create_next_audio_frame_task(void) { frames++; return &task; }
static s32 dispatch_audio_sptask(struct SPTask *p) {
    assert(p==&task); dispatches++;
    return dispatches!=1;
}
static void UNUSED audio_submit_late_frame_buffer(void) { lateSubmissions++; }
static void profiler_audio_started(void) { starts++; }
static void profiler_audio_completed(void) { ends++; }
#include "actual.inc"
int main(void) {
    if (!setjmp(done)) thread4_sound(NULL);
    assert(dispatches==3 && starts==3 && ends==3);
    assert(frames==2);
    assert(lateSubmissions==AUDIO_IMMEDIATE_TASKS);
    cursor=0; gResetTimer=25;
    if (!setjmp(done)) thread4_sound(NULL);
    assert(dispatches==3 && starts==6 && ends==6);
    return 0;
}
'''

RESET_PRELUDE = r'''
#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include <string.h>
#define AIBUFFER_LEN (0xa0 * 16)
'''
RESET_SERVICES = r'''
static struct { int enabled; } notes[2], *gNotes = notes;
static int gMaxSimultaneousNotes = 2, sAudioIsInitialized = TRUE;
static struct { int persistent, temporary; } gSeqLoadedPool, gBankLoadedPool;
static int gReverbSettings[8], sHasStartedFadeOut = TRUE;
static _Atomic int resetServices;
static int failPreload;
static uint16_t gSequenceCount = 4;
#define PRELOAD_BANKS 1
#define PRELOAD_SEQUENCE 2
#define IS_SEQ_LOAD_COMPLETE(sequence) FALSE
#define eu_stubbed_printf_1(...) ((void)0)
#define SEQ_PLAYER_SFX 2
#define SEQ_SOUND_PLAYER 0
#define SEQ_EVENT_SOLVE_PUZZLE 1
#define SEQ_EVENT_PEACH_MESSAGE 2
#define SEQ_EVENT_CUTSCENE_STAR_SPAWN 3
static void reset_service(void) {
    assert(gAudioLoadLock == AUDIO_LOCK_LOADING);
    assert(!sAudioTaskInFlight[0] && !sAudioTaskInFlight[1]);
    resetServices++;
}
static void persistent_pool_clear(int *pool) { (void)pool; reset_service(); }
static void temporary_pool_clear(int *pool) { (void)pool; reset_service(); }
static void reset_bank_and_seq_load_status(void) { reset_service(); }
static void init_reverb_us(s32 preset) { assert(preset == 0); reset_service(); }
static void disable_all_sequence_players(void) { reset_service(); }
static void sound_init(void) { reset_service(); }
static void osWritebackDCacheAll(void) { reset_service(); }
static void *load_banks_immediate(s32 id, u8 *bank) {
    assert(id >= 1 && id <= 3);
    *bank = 0;
    reset_service();
    return gReverbSettings;
}
static void *get_bank_or_seq(void *pool, int type, int id) {
    assert(pool == &gSeqLoadedPool && type == 2 && id >= 1 && id <= 3);
    reset_service();
    return NULL;
}
static void *sequence_dma_immediate(int id, int type) {
    assert(id >= 1 && id <= 3 && type == 2);
    reset_service();
    return failPreload ? NULL : gReverbSettings;
}
static void load_sequence_internal(u32 player, u32 sequence, s32 async) {
    assert(player == SEQ_PLAYER_SFX && sequence == SEQ_SOUND_PLAYER && async == 0);
    reset_service();
}
void load_sequence(u32 player, u32 sequence, s32 async);
static void seq_player_play_sequence(int player, int sequence, int fade) {
    assert(player == SEQ_PLAYER_SFX && sequence == SEQ_SOUND_PLAYER && fade == 0);
    load_sequence(player, sequence, 0);
    reset_service();
}
'''
RESET_CHECKS = r'''
static void *reset_thread(void *unused) {
    (void)unused;
    sound_reset(0);
    return NULL;
}
int main(void) {
    struct SPTask *first = create_next_audio_frame_task();
    assert(first && audio_complete_frame_task(first) == FALSE);
#if !AUDIO_IMMEDIATE_TASKS
    assert(osAiSetNextBuffer(storage[1], 528 * 4) == 0);
#endif
    struct SPTask *second = create_next_audio_frame_task();
    assert(second && hardwareQueued == 1);
    int slot = gAudioTaskIndex;
    for (int i = 0; i < AIBUFFER_LEN / 2; i++) {
        storage[1][i] = 6000;
        storage[2][i] = -7000;
    }
    notes[0].enabled = notes[1].enabled = TRUE;
    sGameLoopTicked = 1;
    pthread_t reset;
    assert(pthread_create(&reset, NULL, reset_thread, NULL) == 0);
    while (gAudioLoadLock != AUDIO_LOCK_LOADING) {
        sched_yield();
    }
    assert(resetServices == 0 && sGameLoopTicked == 1);
    assert(create_next_audio_frame_task() == NULL && syntheses == 2);
    assert(gAudioTaskIndex == slot && sAudioTaskInFlight[slot]);
    audio_complete_frame_task(second);
    assert(pthread_join(reset, NULL) == 0);
    assert(resetServices == 17 && gAudioLoadLock == AUDIO_LOCK_NOT_LOADING);
    assert(!notes[0].enabled && !notes[1].enabled && !sHasStartedFadeOut);
    assert(hardwareQueued == 1 && aiSamples == 100);
    for (int i = 0; i < AIBUFFER_LEN / 2; i++) {
        assert(storage[1][i] == 6000 && storage[2][i] == -7000);
    }
    sound_reset(0); // Idle/repeated reset keeps the same live PCM and gate ownership.
    assert(resetServices == 34 && gAudioLoadLock == AUDIO_LOCK_NOT_LOADING);
    hardwareQueued = 0;
    struct SPTask *resumed = create_next_audio_frame_task();
    assert(resumed && syntheses == 3);
    audio_complete_frame_task(resumed);
    // Standalone loads release their own gate; failed nested loads retain the outer gate.
    preload_sequence(1, PRELOAD_BANKS | PRELOAD_SEQUENCE);
    load_sequence(SEQ_PLAYER_SFX, SEQ_SOUND_PLAYER, 0);
    assert(resetServices == 37 && gAudioLoadLock == AUDIO_LOCK_NOT_LOADING);
    preload_sequence(gSequenceCount, PRELOAD_BANKS | PRELOAD_SEQUENCE);
    assert(resetServices == 37 && gAudioLoadLock == AUDIO_LOCK_NOT_LOADING);
    failPreload = TRUE;
    gAudioLoadLock = AUDIO_LOCK_LOADING;
    preload_sequence(1, PRELOAD_BANKS | PRELOAD_SEQUENCE);
    assert(resetServices == 39 && gAudioLoadLock == AUDIO_LOCK_LOADING);
    gAudioLoadLock = AUDIO_LOCK_NOT_LOADING;
    preload_sequence(1, PRELOAD_BANKS | PRELOAD_SEQUENCE);
    assert(resetServices == 41 && gAudioLoadLock == AUDIO_LOCK_NOT_LOADING);
    return 0;
}
'''
SAMPLE_DMA_PRELUDE = r'''
#include <string.h>
#include <sys/types.h>
#define OS_MESG_PRI_NORMAL 0
#define OS_READ 0
typedef struct { uintptr_t source; u8 *buffer; u32 size; } OSIoMesg;
'''
SAMPLE_DMA_STORAGE = r'''
static OSMesgQueue gCurrAudioFrameDmaQueue;
static OSIoMesg gCurrAudioFrameDmaIoMesgBufs[AUDIO_FRAME_DMA_QUEUE_SIZE];
static s32 gCurrAudioFrameDmaCount;
static struct SharedDma sSampleDmas[160];
static u8 buffers[160][1440], sSampleTTLs[160];
static u32 gSampleDmaNumListItems = 160, sSampleDmaListSize1 = 120;
static u8 sSampleDmaReuseQueue1[256], sSampleDmaReuseQueue2[256];
static u8 sSampleDmaReuseQueueTail1, sSampleDmaReuseQueueHead1;
static u8 sSampleDmaReuseQueueTail2, sSampleDmaReuseQueueHead2;
static OSIoMesg *pending[64];
static int pendingCount, pendingHead, accepted, rejected, consumed, rejectAt = -1;
static u8 inUse[64];
static void osInvalDCache(void *buffer, u32 bytes) { (void)buffer; (void)bytes; }
static int osPiStartDma(OSIoMesg *message, int priority, int direction,
                      uintptr_t source, void *buffer, u32 bytes, OSMesgQueue *queue) {
    assert(priority == OS_MESG_PRI_NORMAL && direction == OS_READ);
    assert(queue == &gCurrAudioFrameDmaQueue);
    ptrdiff_t index = message - gCurrAudioFrameDmaIoMesgBufs;
    assert(index >= 0 && index < AUDIO_FRAME_DMA_QUEUE_SIZE && !inUse[index]);
    assert(pendingCount - pendingHead == gCurrAudioFrameDmaCount);
    if (accepted == rejectAt && !rejected) {
        rejected++;
        return -1;
    }
    assert(pendingCount < 64);
    *message = (OSIoMesg) { source, buffer, bytes };
    inUse[index] = 1;
    pending[pendingCount++] = message;
    accepted++;
    return 0;
}
static int osRecvMesg(OSMesgQueue *queue, OSMesg *message, int mode) {
    assert(queue == &gCurrAudioFrameDmaQueue && message == NULL && mode == OS_MESG_BLOCK);
    assert(pendingHead < pendingCount);
    OSIoMesg *transfer = pending[pendingHead++];
    memset(transfer->buffer, (transfer->source >> 12) ^ 0xa5, transfer->size);
    inUse[transfer - gCurrAudioFrameDmaIoMesgBufs] = 0;
    consumed++;
    if (pendingHead == pendingCount) {
        pendingHead = pendingCount = 0;
    }
    return 0;
}
'''
SAMPLE_DMA_CHECKS = r'''
static void begin(void) {
    gCurrAudioFrameDmaCount = accepted = rejected = consumed = pendingCount = pendingHead = 0;
    memset(sSampleTTLs, 0, sizeof(sSampleTTLs));
    memset(inUse, 0, sizeof(inUse));
    sSampleDmaReuseQueueTail1 = sSampleDmaReuseQueueTail2 = 0;
    sSampleDmaReuseQueueHead1 = 120;
    sSampleDmaReuseQueueHead2 = 40;
    for (unsigned i = 0; i < 160; i++) {
        sSampleDmas[i] = (struct SharedDma) { buffers[i], 0, i < 120 ? 1296 : 1440,
                                            i < 120 ? i : i - 120 };
        if (i < 120) {
            sSampleDmaReuseQueue1[i] = i;
        } else {
            sSampleDmaReuseQueue2[i - 120] = i;
        }
    }
}
int main(void) {
    const int counts[] = {64, 65, 100};
    const int rejections[] = {-1, 0, 23, 64};
    for (unsigned c = 0; c < ARRAY_COUNT(counts); c++) {
        for (unsigned r = 0; r < ARRAY_COUNT(rejections); r++) {
            begin();
            rejectAt = rejections[r];
            u8 refs[40] = {0}, *samples[100];
            for (int i = 0; i < counts[c]; i++) {
                samples[i] = dma_sample_data(0x100000 + i * 0x1000, 9, 0, &refs[i % 40]);
            }
            assert(accepted == counts[c] && gCurrAudioFrameDmaCount <= 64);
            audio_wait_for_sample_dmas();
            assert(consumed == accepted && gCurrAudioFrameDmaCount == 0);
            assert(pendingCount == 0 && pendingHead == 0);
            for (int i = 0; i < counts[c]; i++) {
                for (int byte = 0; byte < 9; byte++) {
                    assert(samples[i][byte] == (u8) ((0x100 + i) ^ 0xa5));
                }
            }
        }
    }
    return 0;
}
'''

SAMPLE_BUDGET_STORAGE = r'''
#include <string.h>
static u8 gEmulator;
static s32 gAiFrequency;
static s32 gAudioFrameCount, gSamplesPerFrameTarget, gMinAiBufferLength;
#include "actual.inc"
static void set_frequency(u32 frequency) {
    gAiFrequency = frequency;
    gSamplesPerFrameTarget = ((frequency / 60) + 15) & ~15;
    gMinAiBufferLength = gSamplesPerFrameTarget - 16;
}
'''

SAMPLE_BUDGET_STEADY = r'''
int main(void) {
    const u32 frequencies[] = {29994, 32006};
    gEmulator = EMU_MUPEN;
    for (unsigned rate = 0; rate < ARRAY_COUNT(frequencies); rate++) {
        set_frequency(frequencies[rate]);
        memset(&sAudioSampleBudget, 0, sizeof(sAudioSampleBudget));
        u64 produced = 0;
        for (gAudioFrameCount = 1; gAudioFrameCount <= 18000; gAudioFrameCount++) {
            // AI length must not change the nominal playback clock.
            s32 samples = audio_get_frame_sample_count((gAudioFrameCount % 7) * 100);
            assert((samples & 15) == 0);
            assert(samples >= gMinAiBufferLength && samples <= gSamplesPerFrameTarget + 16);
            produced += samples;
            u64 due = (u64) gAiFrequency * gAudioFrameCount / 60;
            assert(produced <= due && due - produced < 16);
        }
    }
    return 0;
}
'''

SAMPLE_BUDGET_RECOVERY = r'''
int main(void) {
    gEmulator = EMU_MUPEN;
    set_frequency(29994);
    gAudioFrameCount = 1;
    u64 produced = audio_get_frame_sample_count(0);
    assert(produced == 496); // No startup reserve.
    // One omitted preparation is repaid within the existing output-length bounds.
    for (gAudioFrameCount = 3; gAudioFrameCount <= 60; gAudioFrameCount++) {
        s32 samples = audio_get_frame_sample_count(0);
        assert(samples >= 496 && samples <= 528 && (samples & 15) == 0);
        produced += samples;
    }
    assert(produced <= (u32) gAiFrequency && (u32) gAiFrequency - produced < 16);

    memset(&sAudioSampleBudget, 0, sizeof(sAudioSampleBudget));
    gAudioFrameCount = 1;
    audio_get_frame_sample_count(0);
    gAudioFrameCount = 60;
    assert(audio_get_frame_sample_count(0) == 528);
    // A long but bounded interruption cannot accumulate unrestricted debt.
    assert(sAudioSampleBudget.sampleNumerator <= 512 * 60 + 959);
    gAudioFrameCount = 121;
    assert(audio_get_frame_sample_count(0) == 496); // >1 second starts fresh.
    assert(audio_get_frame_sample_count(0) == 496); // Repeated/reset counter.
    gAudioFrameCount = 0;
    assert(audio_get_frame_sample_count(0) == 496);

    set_frequency(32006);
    gAudioFrameCount = 1;
    assert(audio_get_frame_sample_count(0) == 528);
    assert(sAudioSampleBudget.sampleNumerator < 960);
    set_frequency(29994);
    gAudioFrameCount = (s32) 0xfffffffeu;
    assert(audio_get_frame_sample_count(0) == 496);
    gAudioFrameCount = 1; // Unsigned elapsed time survives counter wrap.
    assert(audio_get_frame_sample_count(0) == 528);
    assert(sAudioSampleBudget.lastFrame == 1);
    return 0;
}
'''

SAMPLE_BUDGET_NATIVE = r'''
int main(void) {
    const u8 emulators[] = {0, EMU_CONSOLE, EMU_WIIVC, EMU_PROJECT64_1_OR_2,
                           EMU_PROJECT64_3, EMU_PARALLEL_LAUNCHER, EMU_OTHER,
#if !AUDIO_MUPEN_SAMPLE_CLOCK
                           EMU_MUPEN,
#endif
    };
    set_frequency(32006);
    for (unsigned emulator = 0; emulator < ARRAY_COUNT(emulators); emulator++) {
        gEmulator = emulators[emulator];
        for (u32 remaining = 0; remaining <= 65536; remaining += 16) {
            s32 expected = (s16) (((gSamplesPerFrameTarget - remaining + 64) & ~15) + 16);
            if (expected < gMinAiBufferLength) {
                expected = gMinAiBufferLength;
            }
            if (expected > gSamplesPerFrameTarget + 16) {
                expected = gSamplesPerFrameTarget + 16;
            }
            gAudioFrameCount++;
            assert(audio_get_frame_sample_count(remaining) == expected);
        }
    }
#if AUDIO_MUPEN_SAMPLE_CLOCK
    assert(sAudioSampleBudget.frequency == 0 && sAudioSampleBudget.sampleNumerator == 0);
#endif
    return 0;
}
'''

SAMPLE_BUDGET_PIPELINE = r'''
int main(void) {
    gEmulator = EMU_MUPEN;
    gAiFrequency = 29994;
    gSamplesPerFrameTarget = 512;
    gMinAiBufferLength = 496;
    struct SPTask *first = create_next_audio_frame_task();
    assert(first && gAiBufferLengths[gCurrAiBufferIndex] == 496);
    struct AudioSampleBudget budget = sAudioSampleBudget;
    gAudioLoadLock = AUDIO_LOCK_LOADING;
    assert(create_next_audio_frame_task() == NULL);
    assert(sAudioSampleBudget.lastFrame == budget.lastFrame);
    assert(sAudioSampleBudget.sampleNumerator == budget.sampleNumerator);
    gAudioLoadLock = AUDIO_LOCK_NOT_LOADING;
#if !AUDIO_IMMEDIATE_TASKS
    // The buffered scheduler can prepare its second task while the first runs.
    struct SPTask *second = create_next_audio_frame_task();
    assert(second && syntheses == 2);
    budget = sAudioSampleBudget;
#endif
    assert(create_next_audio_frame_task() == NULL); // No reusable command/output slot.
    assert(sAudioSampleBudget.lastFrame == budget.lastFrame);
    assert(sAudioSampleBudget.sampleNumerator == budget.sampleNumerator);
    audio_complete_frame_task(first);
#if !AUDIO_IMMEDIATE_TASKS
    audio_complete_frame_task(second);
#endif
    struct SPTask *next = create_next_audio_frame_task();
    assert(next && syntheses == (AUDIO_IMMEDIATE_TASKS ? 2 : 3));
    assert(gAiBufferLengths[gCurrAiBufferIndex] == 528);
    audio_complete_frame_task(next);
#if AUDIO_IMMEDIATE_TASKS
    busy = 1;
    struct SPTask *third = create_next_audio_frame_task();
    assert(third && syntheses == 3);
    audio_complete_frame_task(third);
    budget = sAudioSampleBudget;
    assert(create_next_audio_frame_task() == NULL);
    assert(create_next_audio_frame_task() == NULL);
    assert(sAudioSampleBudget.lastFrame == budget.lastFrame);
    assert(sAudioSampleBudget.sampleNumerator == budget.sampleNumerator);
    assert(syntheses == 3);
    busy = 0;
    hardwareQueued = 0;
    struct SPTask *resumed = create_next_audio_frame_task();
    assert(resumed && syntheses == 4 && gAiBufferLengths[gCurrAiBufferIndex] == 528);
    audio_complete_frame_task(resumed);
#endif
    budget = sAudioSampleBudget;
    gAudioEnabled = FALSE;
    assert(create_next_audio_frame_task() == NULL);
    assert(sAudioSampleBudget.lastFrame == budget.lastFrame);
    assert(sAudioSampleBudget.sampleNumerator == budget.sampleNumerator);
    return 0;
}
'''

class AudioSubmission(unittest.TestCase):
    def compile_run(self, fixture, actual, immediate=1, extra_flags=()):
        with tempfile.TemporaryDirectory(prefix="audio-submission-") as name:
            directory = Path(name)
            (directory / "actual.inc").write_text(actual)
            (directory / "fixture.c").write_text(COMMON + fixture)
            command = shlex.split(os.environ.get("CC", "cc")) + [
                "-std=gnu99", f"-DAUDIO_IMMEDIATE_TASKS={immediate}", *extra_flags,
                "-Wall", "-Wextra", "-Werror",
                "-Wno-pointer-to-int-cast", "-Wno-int-to-pointer-cast", "-I", str(directory),
                str(directory / "fixture.c"), "-o", str(directory / "test")]
            subprocess.run(command, check=True, timeout=30)
            subprocess.run([str(directory / "test")], check=True, timeout=15)

    def sample_budget_source(self):
        source = Source(ROOT / "src/audio/external.c")
        config = Source(ROOT / "include/config/config_audio.h")
        actual = "#ifndef AUDIO_MUPEN_SAMPLE_CLOCK\n"
        actual += config.macros("AUDIO_MUPEN_SAMPLE_CLOCK") + "\n#endif\n"
        actual += Source(ROOT / "src/game/emutest.h").declaration("enum", "Emulator")
        actual += "#if AUDIO_MUPEN_SAMPLE_CLOCK\n"
        actual += source.macros("AUDIO_SAMPLE_BUDGET_.*")
        actual += source.declaration("struct", "AudioSampleBudget")
        actual += "static struct AudioSampleBudget sAudioSampleBudget;\n#endif\n"
        return actual + source.function("audio_get_frame_sample_count")

    def pipeline_source(self, atomic_tasks=False):
        source = Source(ROOT / "src/audio/external.c")
        task_type = "_Atomic u8" if atomic_tasks else "volatile u8"
        actual = f"static {task_type} sAudioTaskInFlight[2];\n"
        actual += "#if AUDIO_IMMEDIATE_TASKS\n"
        actual += source.declaration("enum", "AudioBufferState")
        actual += "\nstatic volatile u8 sAudioBufferStates[3];\n"
        actual += "static u8 sAudioAiQueue[2], sAudioAiQueueCount, sAudioSubmitIndex=1;\n"
        actual += "static volatile u8 sAudioSubmissionLate;\n"
        for name in ("audio_reclaim_played_buffers", "audio_submit_frame_buffer",
                     "audio_submit_late_frame_buffer", "audio_frame_buffer_available"):
            actual += source.function(name)
        actual += "#endif\n"
        actual += self.sample_budget_source()
        actual += source.function("audio_init_frame_tasks")
        actual += Source(ROOT / "src/audio/load.c").function("audio_wait_for_sample_dmas")
        actual += source.function("audio_wait_for_frame_tasks")
        actual += source.function("audio_complete_frame_task")
        actual += source.function("create_next_audio_frame_task")
        return actual

    def test_mupen_sample_clock_exact_aligned_totals(self):
        self.compile_run(SAMPLE_BUDGET_STORAGE + SAMPLE_BUDGET_STEADY,
                         self.sample_budget_source())

    def test_mupen_sample_budget_recovery_and_clock_epochs(self):
        self.compile_run(SAMPLE_BUDGET_STORAGE + SAMPLE_BUDGET_RECOVERY,
                         self.sample_budget_source())

    def test_native_and_disabled_sample_sizing_is_preserved(self):
        for enabled in (0, 1):
            with self.subTest(enabled=enabled):
                self.compile_run(SAMPLE_BUDGET_STORAGE + SAMPLE_BUDGET_NATIVE,
                                 self.sample_budget_source(),
                                 extra_flags=(f"-DAUDIO_MUPEN_SAMPLE_CLOCK={enabled}",))

    def test_sample_budget_waits_for_owned_buffer_preparation(self):
        fixture = PIPELINE.split('#include "actual.inc"')[0]
        fixture += '#include "actual.inc"\n' + SAMPLE_BUDGET_PIPELINE
        for immediate in (0, 1):
            with self.subTest(immediate=immediate):
                self.compile_run(fixture, self.pipeline_source(), immediate)

    def test_buffer_ownership_order_busy_retry_and_dma_fence(self):
        self.compile_run(PIPELINE, self.pipeline_source())

    def test_boot_discards_audio_ownership_retained_by_soft_reset(self):
        loader = Source(ROOT / "src/audio/load.c")
        boot = loader.function("audio_init")
        start = boot.index("    for (i = 0; i < NUMAIBUFFERS; i++) {")
        end = boot.index("    osCreateMesgQueue(&gAudioDmaMesgQueue")
        actual = self.pipeline_source()
        actual += "static void boot_audio_startup(void) {\ns32 i;\n"
        actual += boot[start:end] + "}\n"
        fixture = PIPELINE.split('#include "actual.inc"')[0]
        fixture = fixture.replace("static s32 gAiFrequency", "static s32 UNUSED gAiFrequency")
        fixture = fixture.replace("static u8 gEmulator", "static u8 UNUSED gEmulator")
        fixture += r'''
#include "actual.inc"
int main(void) {
    // Reset has discarded the hardware tasks, but low RAM still describes them.
    sAudioTaskInFlight[0] = sAudioTaskInFlight[1] = TRUE;
    gAudioFrameCount = 900;
    gAudioTaskIndex = 1;
    gCurrAiBufferIndex = 2;
    gAudioTask = &gAudioTasks[0];
    gAudioTasks[0].task.t.data_size = gAudioTasks[1].task.t.data_size = 64;
#if AUDIO_IMMEDIATE_TASKS
    sAudioBufferStates[0] = AUDIO_BUFFER_RENDERING;
    sAudioBufferStates[1] = AUDIO_BUFFER_QUEUED;
    sAudioBufferStates[2] = AUDIO_BUFFER_READY;
    sAudioAiQueue[0] = 1;
    sAudioAiQueue[1] = 2;
    sAudioAiQueueCount = 2;
    sAudioSubmitIndex = 2;
    sAudioSubmissionLate = TRUE;
#endif
#if AUDIO_MUPEN_SAMPLE_CLOCK
    sAudioSampleBudget = (struct AudioSampleBudget){899, 29994, 48000};
#endif
    boot_audio_startup();
    assert(!sAudioTaskInFlight[0] && !sAudioTaskInFlight[1]);
    assert(!gAudioTask && gAudioFrameCount == 0 && gAudioTaskIndex == 0);
    assert(gCurrAiBufferIndex == 0);
#if AUDIO_IMMEDIATE_TASKS
    assert(sAudioAiQueueCount == 0 && sAudioSubmitIndex == 1 && !sAudioSubmissionLate);
    for (unsigned i = 0; i < ARRAY_COUNT(sAudioBufferStates); i++) {
        assert(sAudioBufferStates[i] == AUDIO_BUFFER_FREE);
    }
#endif
#if AUDIO_MUPEN_SAMPLE_CLOCK
    assert(sAudioSampleBudget.lastFrame == 0 && sAudioSampleBudget.frequency == 0);
    assert(sAudioSampleBudget.sampleNumerator == 0);
    gEmulator = EMU_MUPEN;
#endif
    struct SPTask *fresh = create_next_audio_frame_task();
    assert(fresh && syntheses == 1 && gCurrAiBufferIndex == 1);
    assert(gAudioFrameCount == 1 && gAudioTaskIndex == 1);
    audio_complete_frame_task(fresh);
    audio_wait_for_frame_tasks(); // The game-over music reset can now finish.
    assert(!sAudioTaskInFlight[0] && !sAudioTaskInFlight[1]);
    return 0;
}
'''
        for immediate in (0, 1):
            for clock in (0, 1):
                with self.subTest(immediate=immediate, clock=clock):
                    self.compile_run(fixture, actual, immediate,
                                     (f"-DAUDIO_MUPEN_SAMPLE_CLOCK={clock}",))

    def test_reset_waits_for_tasks_and_preserves_playback(self):
        fixture = PIPELINE.split('#include "actual.inc"')[0]
        fixture = fixture.replace("static s32 gAudioLoadLock", "static _Atomic s32 gAudioLoadLock")
        fixture = fixture.replace("    syntheses++;", "    assert(sAudioTaskInFlight[gAudioTaskIndex]);\n    syntheses++;")
        # The task flag is declared before the synthesis boundary double here.
        fixture = fixture.replace("static s16 storage", "static _Atomic u8 sAudioTaskInFlight[2];\nstatic s16 storage", 1)
        fixture = RESET_PRELUDE + fixture + RESET_SERVICES + '#include "actual.inc"\n' + RESET_CHECKS
        heap = Source(ROOT / "src/audio/heap.c")
        start = heap.text.index("void audio_reset_session(s32 reverbPresetId) {")
        end = heap.text.index("\n#else\nvoid audio_reset_session(void)", start)
        warm_reset = heap.text[start:end] + '\nassert(!"unexpected cold init");\n}\n'
        actual = self.pipeline_source(atomic_tasks=True).replace(
            "static _Atomic u8 sAudioTaskInFlight[2];\n", "", 1)
        loader = Source(ROOT / "src/audio/load.c")
        actual += loader.function("preload_sequence")
        actual += loader.function("load_sequence")
        actual += warm_reset
        actual += Source(ROOT / "src/audio/external.c").function("sound_reset")
        for immediate in (0, 1):
            with self.subTest(immediate=immediate):
                self.compile_run(fixture, actual, immediate, ("-pthread",))

    def test_sample_dma_batch_capacity_and_rejected_submission(self):
        loader = Source(ROOT / "src/audio/load.c")
        actual = loader.declaration("struct", "SharedDma")
        actual += SAMPLE_DMA_STORAGE
        actual += "".join(loader.function(name) for name in (
            "audio_wait_for_sample_dmas", "audio_wait_for_sample_dmas_profiled", "dma_sample_data"))
        self.compile_run(SAMPLE_DMA_PRELUDE + '#include "actual.inc"\n' + SAMPLE_DMA_CHECKS, actual)

    def test_immediate_start_yield_resume_and_dispatch_failure(self):
        source = Source(ROOT / "src/boot/main.c")
        actual = "".join(source.function(function) for function in (
            "start_sptask", "interrupt_gfx_sptask", "pretend_audio_sptask_done",
            "start_audio_task_if_ready", "handle_sp_complete", "dispatch_audio_sptask"))
        self.compile_run(SCHEDULER, actual)

    def test_pre_nmi_stops_tasks_and_retries_rsp_cleanup(self):
        source = Source(ROOT / "src/boot/main.c")
        actual = "".join(source.function(function) for function in (
            "start_sptask", "interrupt_gfx_sptask", "pretend_audio_sptask_done",
            "start_audio_task_if_ready", "handle_sp_complete", "dispatch_audio_sptask",
            "handle_vblank"))
        fixture = SCHEDULER.split('#include "actual.inc"')[0] + r'''
static int gNumVblanks, cleanupAttempts, rspRunning, rspPc = 0x100, rumbleTicks;
struct VblankHandler { OSMesgQueue *queue; OSMesg msg; };
static struct VblankHandler handler = {&gIntrMesgQueue, (OSMesg)MESG_SP_COMPLETE};
static struct VblankHandler *gVblankHandler1 = &handler, *gVblankHandler2, *gVblankHandler3;
static s32 reset_rsp_for_pre_nmi(void) {
    cleanupAttempts++;
    if (rspRunning) return FALSE;
    rspPc = 0;
    return TRUE;
}
static void handle_nmi_request(void) { gResetTimer = 1; }
static void receive_new_tasks(void) {}
void start_sptask(s32 taskType);
static void UNUSED start_gfx_sptask(void) { start_sptask(M_GFXTASK); }
static void rumble_thread_update_vi(void) { rumbleTicks++; }
#include "actual.inc"
int main(void) {
    struct SPTask audio = {.task.t.type=M_AUDTASK}, gfx = {.task.t.type=M_GFXTASK};
    sCurrentAudioSPTask = &audio; sCurrentDisplaySPTask = &gfx;
    start_sptask(M_AUDTASK); rspRunning = 1;
    assert(starts == 1 && loads == 1 && gActiveSPTask == &audio);
    gResetTimer = 1;
    for (int i=0; i<4; i++) handle_vblank();
    assert(cleanupAttempts == 4 && rspPc == 0x100 && starts == 1);
    // Finishing the old task cannot launch the queued graphics task during reset.
    rspRunning = 0; handle_sp_complete();
    assert(gActiveSPTask == NULL && starts == 1 && completions == 1);
    handle_vblank();
    assert(cleanupAttempts == 5 && rspPc == 0 && starts == 1 && loads == 1);
    assert(gNumVblanks == 5 && rumbleTicks == 5 && sends == 5);
    start_sptask(M_GFXTASK);
    assert(gActiveSPTask == NULL && starts == 1);
    // A new boot restores the normal task path.
    gResetTimer = 0; start_sptask(M_GFXTASK);
    assert(gActiveSPTask == &gfx && starts == 2 && loads == 2);
    assert(dispatch_audio_sptask(&audio));
    // A full interrupt queue can lose the message, but not the SDK shutdown latch.
    gResetTimer = 0; sdkShutdown = 1;
    start_sptask(M_AUDTASK);
    assert(starts == 2);
    int previousSends = sends;
    handle_vblank();
    assert(gResetTimer == 2 && starts == 2 && rumbleTicks == 6 && sends == previousSends + 1);
    return 0;
}
'''
        for immediate in (0, 1):
            selected = actual.replace("static void start_audio_task_if_ready",
                                      "static void UNUSED start_audio_task_if_ready")
            self.compile_run(fixture, selected, immediate=immediate)

    def test_dma_wait_is_not_counted_as_preempting_the_game_thread(self):
        actual = Source(ROOT / "src/game/profiling.c").function("profiler_audio_waited")
        fixture = r'''
static u32 audio_start = 0xfffffff0u;
#define AUDIO_PROFILING
#define PROFILER_TIME_SUB_AUDIO_UPDATE 1
#define PROFILER_TIME_SUB_AUDIO_START 0
static u32 audio_subset_starts[2] = {7, 0xfffffff0u};
#include "actual.inc"
int main(void) {
    profiler_audio_waited(32);
    assert(audio_start == 16 && audio_subset_starts[1] == 16);
    assert(audio_subset_starts[0] == 7);
    assert((u32)(48 - audio_start) == 32);
    return 0;
}
'''
        self.compile_run(fixture, actual)

    def test_sound_thread_retains_failed_dispatch_without_resynthesis(self):
        actual = Source(ROOT / "src/game/sound_init.c").function("thread4_sound")
        for immediate in (0, 1):
            with self.subTest(immediate=immediate):
                self.compile_run(ROUTING, actual, immediate)


if __name__ == "__main__":
    unittest.main()
