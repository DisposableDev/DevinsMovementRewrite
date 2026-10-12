#include "PR/os_internal.h"
#include "engine/math_util.h"

#include "game_init.h"
#include "main.h"

/////////////////////////////////////////////////
// Libultra structs and macros (from ultralib) //
/////////////////////////////////////////////////

#define ARRLEN(x) ((s32)(sizeof(x) / sizeof(x[0])))
#define CHNL_ERR(format) (((format).rxsize & CHNL_ERR_MASK) >> 4)

#define CHNL_ERR_MASK		0xC0	/* Bit 6-7: channel errors */

typedef struct
{
    /* 0x0 */ u32 ramarray[15];
    /* 0x3C */ u32 pifstatus;
} OSPifRam;

typedef struct
{
    /* 0x0 */ u8 dummy;
    /* 0x1 */ u8 txsize;
    /* 0x2 */ u8 rxsize;
    /* 0x3 */ u8 cmd;
    /* 0x4 */ u16 button;
    /* 0x6 */ s8 stick_x;
    /* 0x7 */ s8 stick_y;
} __OSContReadFormat;

typedef struct
{
    /* 0x0 */ u8 dummy;
    /* 0x1 */ u8 txsize;
    /* 0x2 */ u8 rxsize;
    /* 0x3 */ u8 cmd;
    /* 0x4 */ u8 typeh;
    /* 0x5 */ u8 typel;
    /* 0x6 */ u8 status;
    /* 0x7 */ u8 dummy1;
} __OSContRequesFormat;

typedef struct
{
    /* 0x0 */ u8 txsize;
    /* 0x1 */ u8 rxsize;
    /* 0x2 */ u8 cmd;
    /* 0x3 */ u8 typeh;
    /* 0x4 */ u8 typel;
    /* 0x5 */ u8 status;
} __OSContRequesFormatShort;

typedef struct
{
    /* 0x0 */ u8 dummy;
    /* 0x1 */ u8 txsize;
    /* 0x2 */ u8 rxsize;
    /* 0x3 */ u8 cmd;
    /* 0x4 */ u8 addrh;
    /* 0x5 */ u8 addrl;
    /* 0x6 */ u8 data[BLOCKSIZE];
    /* 0x26 */ u8 datacrc;
} __OSContRamReadFormat;

extern OSPifRam __osContPifRam;
extern u8 __osMaxControllers;

#define CONT_CMD_READ_BUTTON    1

// Controller accessory addresses
// https://github.com/joeldipops/TransferBoy/blob/master/docs/TransferPakReference.md

// Accesory detection
#define CONT_ADDR_DETECT    0x8000
// Rumble
#define CONT_ADDR_RUMBLE    0xC000
// Controller Pak
// Transfer Pak
#define CONT_ADDR_GB_POWER  0x8000 // Same as the detection address, but semantically different
#define CONT_ADDR_GB_BANK   0xA000
#define CONT_ADDR_GB_STATUS 0xB000

// Addresses sent to controller accessories are in blocks, not bytes
#define CONT_BLOCKS(x) ((x) / BLOCKSIZE)

// Block addresses of the above
#define CONT_BLOCK_DETECT    CONT_BLOCKS(CONT_ADDR_DETECT)
#define CONT_BLOCK_RUMBLE    CONT_BLOCKS(CONT_ADDR_RUMBLE)
#define CONT_BLOCK_GB_POWER  CONT_BLOCKS(CONT_ADDR_GB_POWER)
#define CONT_BLOCK_GB_BANK   CONT_BLOCKS(CONT_ADDR_GB_BANK)
#define CONT_BLOCK_GB_STATUS CONT_BLOCKS(CONT_ADDR_GB_STATUS)

// Joybus commands
//from: http://en64.shoutwiki.com/wiki/SI_Registers_Detailed#CONT_CMD_Usage
#define CONT_CMD_REQUEST_STATUS 0
#define CONT_CMD_READ_BUTTON    1
#define CONT_CMD_READ_PAK       2
#define CONT_CMD_WRITE_PAK      3
#define CONT_CMD_READ36_VOICE   9
#define CONT_CMD_WRITE20_VOICE  10
#define CONT_CMD_READ2_VOICE    11
#define CONT_CMD_WRITE4_VOICE   12
#define CONT_CMD_SWRITE_VOICE   13
#define CONT_CMD_CHANNEL_RESET  0xFD
#define CONT_CMD_RESET          0xFF
#define CONT_CMD_GCN_SHORTPOLL  0x40

// Bytes transmitted for each joybus command
#define CONT_CMD_REQUEST_STATUS_TX 1
#define CONT_CMD_READ_BUTTON_TX    1
#define CONT_CMD_READ_PAK_TX       3
#define CONT_CMD_WRITE_PAK_TX      35
#define CONT_CMD_READ36_VOICE_TX   3
#define CONT_CMD_WRITE20_VOICE_TX  23
#define CONT_CMD_READ2_VOICE_TX    3
#define CONT_CMD_WRITE4_VOICE_TX   7
#define CONT_CMD_SWRITE_VOICE_TX   3
#define CONT_CMD_RESET_TX          1
#define CONT_CMD_GCN_SHORTPOLL_TX  3

// Bytes received for each joybus command
#define CONT_CMD_REQUEST_STATUS_RX 3
#define CONT_CMD_READ_BUTTON_RX    4
#define CONT_CMD_READ_PAK_RX       33
#define CONT_CMD_WRITE_PAK_RX      1
#define CONT_CMD_READ36_VOICE_RX   37
#define CONT_CMD_WRITE20_VOICE_RX  1
#define CONT_CMD_READ2_VOICE_RX    3
#define CONT_CMD_WRITE4_VOICE_RX   1
#define CONT_CMD_SWRITE_VOICE_RX   1
#define CONT_CMD_RESET_RX          3
#define CONT_CMD_GCN_SHORTPOLL_RX  8

#define CONT_CMD_NOP 0xff
#define CONT_CMD_END 0xfe //indicates end of a command
#define CONT_CMD_EXE 1    //set pif ram status byte to this to do a command

void __osSiGetAccess(void);
void __osSiRelAccess(void);

////////////////////////
// Gamecube additions //
////////////////////////

typedef struct
{
    /* 0x0 */ u8 dummy;
    /* 0x1 */ u8 txsize;
    /* 0x2 */ u8 rxsize;
    /* 0x3 */ u8 cmd;
    /* 0x4 */ u8 analog_mode;
    /* 0x5 */ u8 rumble;
    /* 0x6 */ u16 button;
    /* 0x8 */ u8 stick_x;
    /* 0x9 */ u8 stick_y;
    /* 0xA */ u8 c_stick_x;
    /* 0xB */ u8 c_stick_y;
    /* 0xC */ u8 l_trig;
    /* 0xD */ u8 r_trig;
} __OSContGCNShortPollFormat;
extern u8 __osContLastCmd;
u8 __osGamecubeRumbleEnabled[MAXCONTROLLERS];

typedef struct
{
    s8 initialized;
    u8 stick_x;
    u8 stick_y;
    u8 c_stick_x;
    u8 c_stick_y;
} ControllerCenters;

#define GCN_C_STICK_THRESHOLD 38

static void __osPackReadData(void);
extern void __osPackRequestData(u8 command);
static u16 __osTranslateGCNButtons(u16, s32, s32);

////////////////////
// contreaddata.c //
////////////////////

static s32 controller_start_dma(s32 direction, void *buffer) {
    OSIntMask mask = osSetIntMask(OS_IM_NONE);
    s32 result = -1;

    // No pre-NMI can arrive between the last shutdown check and the register write.
    if (!reset_pending()) {
        result = __osSiRawStartDma(direction, buffer);
    }
    osSetIntMask(mask);
    return result;
}

s32 osContStartReadDataEx(OSMesgQueue* mq) {
    s32 ret = 0;

    __osSiGetAccess();

    if (__osContLastCmd != CONT_CMD_READ_BUTTON) {
        __osPackReadData();
        ret = controller_start_dma(OS_WRITE, __osContPifRam.ramarray);
        if (ret != 0) {
            __osSiRelAccess();
            return ret;
        }
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
    }

    ret = controller_start_dma(OS_READ, __osContPifRam.ramarray);
    __osContLastCmd = CONT_CMD_READ_BUTTON;
    __osSiRelAccess();

    return ret;
}

// Match libultra's status query, with the same reset-safe DMA entry used for pads.
s32 osContStartQueryEx(OSMesgQueue *mq) {
    s32 result;
    __osSiGetAccess();
    if (__osContLastCmd != CONT_CMD_REQUEST_STATUS) {
        __osPackRequestData(CONT_CMD_REQUEST_STATUS);
        result = controller_start_dma(OS_WRITE, __osContPifRam.ramarray);
        if (result != 0) {
            __osSiRelAccess();
            return result;
        }
        osRecvMesg(mq, NULL, OS_MESG_BLOCK);
    }
    result = controller_start_dma(OS_READ, __osContPifRam.ramarray);
    __osContLastCmd = CONT_CMD_REQUEST_STATUS;
    __osSiRelAccess();
    return result;
}

extern OSMesgQueue __osSiAccessQueue;
extern s32 __osSiAccessQueueEnabled;
extern s32 __osSiDeviceBusy(void);

#define CONTROLLER_TIMEOUT_US 20000

static s32 controller_wait_bounded(OSMesgQueue *mq, s32 (*resetPending)(void)) {
    u32 start = osGetCount();

    do {
        if (osRecvMesg(mq, NULL, OS_MESG_NOBLOCK) == 0) {
            return 0;
        }
        if (resetPending != NULL && resetPending()) {
            return CRASH_CONTROLLER_DMA_PENDING;
        }
    } while ((u32) (osGetCount() - start) < OS_USEC_TO_CYCLES(CONTROLLER_TIMEOUT_US));
    return CRASH_CONTROLLER_DMA_PENDING;
}

s32 osContReadDataExForCrash(OSMesgQueue *mq, s32 (*resetPending)(void)) {
    static s32 dmaPending;
    s32 result;

    if (dmaPending) {
        return CRASH_CONTROLLER_DMA_PENDING;
    }
    if (resetPending() || !__osSiAccessQueueEnabled
        || osRecvMesg(&__osSiAccessQueue, NULL, OS_MESG_NOBLOCK) != 0) {
        return -1;
    }
    // A transfer owned by the interrupted game may still be finishing.
    if (__osSiDeviceBusy()) {
        __osSiRelAccess();
        return -1;
    }
    while (osRecvMesg(mq, NULL, OS_MESG_NOBLOCK) == 0) {
    }
    if (__osContLastCmd != CONT_CMD_READ_BUTTON) {
        __osPackReadData();
        result = controller_start_dma(OS_WRITE, __osContPifRam.ramarray);
        if (result != 0) {
            __osSiRelAccess();
            return -1;
        }
        result = controller_wait_bounded(mq, resetPending);
        if (result == CRASH_CONTROLLER_DMA_PENDING) {
            dmaPending = TRUE;
            return result;
        }
    }
    if (resetPending()) {
        __osSiRelAccess();
        return -1;
    }
    result = controller_start_dma(OS_READ, __osContPifRam.ramarray);
    if (result != 0) {
        __osSiRelAccess();
        return -1;
    }
    __osContLastCmd = CONT_CMD_READ_BUTTON;
    result = controller_wait_bounded(mq, resetPending);
    if (result == CRASH_CONTROLLER_DMA_PENDING) {
        dmaPending = TRUE;
        return result;
    }
    __osSiRelAccess();
    return 0;
}

ControllerCenters gGamecubeControllerCenters[MAXCONTROLLERS] = { 0 };

void osContGetReadDataEx(OSContPadEx* data) {
    u8* ptr = (u8*)__osContPifRam.ramarray;
    __OSContReadFormat readformat;
    __OSContGCNShortPollFormat readformatgcn;
    int i;

    for (i = 0; i < __osMaxControllers; i++, data++) {
        if ((gControllerStatuses[i].type & CONT_CONSOLE_MASK) == CONT_CONSOLE_GCN) {
            s32 stick_x, stick_y, c_stick_x, c_stick_y;
            readformatgcn = *(__OSContGCNShortPollFormat*)ptr;
            data->errno = CHNL_ERR(readformatgcn);
            if (data->errno == 0) {
                if (!gGamecubeControllerCenters[i].initialized) {
                    gGamecubeControllerCenters[i].initialized = TRUE;
                    gGamecubeControllerCenters[i].stick_x   = readformatgcn.stick_x;
                    gGamecubeControllerCenters[i].stick_y   = readformatgcn.stick_y;
                    gGamecubeControllerCenters[i].c_stick_x = readformatgcn.c_stick_x;
                    gGamecubeControllerCenters[i].c_stick_y = readformatgcn.c_stick_y;
                }

                stick_x = CLAMP_S8(((s32)readformatgcn.stick_x) - gGamecubeControllerCenters[i].stick_x);
                stick_y = CLAMP_S8(((s32)readformatgcn.stick_y) - gGamecubeControllerCenters[i].stick_y);
                data->stick_x = stick_x;
                data->stick_y = stick_y;
                c_stick_x = CLAMP_S8(((s32)readformatgcn.c_stick_x) - gGamecubeControllerCenters[i].c_stick_x);
                c_stick_y = CLAMP_S8(((s32)readformatgcn.c_stick_y) - gGamecubeControllerCenters[i].c_stick_y);
                data->c_stick_x = c_stick_x;
                data->c_stick_y = c_stick_y;
                data->button = __osTranslateGCNButtons(readformatgcn.button, c_stick_x, c_stick_y);
                data->l_trig = readformatgcn.l_trig;
                data->r_trig = readformatgcn.r_trig;
            } else {
                gGamecubeControllerCenters[i].initialized = FALSE;
            }
            ptr += sizeof(__OSContGCNShortPollFormat);
        } else {
            readformat = *(__OSContReadFormat*)ptr;
            data->errno = CHNL_ERR(readformat);
            
            if (data->errno == 0) {
                data->stick_x = readformat.stick_x;
                data->stick_y = readformat.stick_y;
                data->button = readformat.button;
                data->c_stick_x = 0;
                data->c_stick_y = 0;
                data->l_trig = 0;
                data->r_trig = 0;
            }
            ptr += sizeof(__OSContReadFormat);
        }
    }
}

static void __osPackReadData(void) {
    u8* ptr = (u8*)__osContPifRam.ramarray;
    __OSContReadFormat readformat;
    __OSContGCNShortPollFormat readformatgcn;
    int i;

    for (i = 0; i < ARRLEN(__osContPifRam.ramarray); i++) {
        __osContPifRam.ramarray[i] = 0;
    }

    __osContPifRam.pifstatus = CONT_CMD_EXE;
    readformat.dummy = CONT_CMD_NOP;
    readformat.txsize = CONT_CMD_READ_BUTTON_TX;
    readformat.rxsize = CONT_CMD_READ_BUTTON_RX;
    readformat.cmd = CONT_CMD_READ_BUTTON;
    readformat.button = 0xFFFF;
    readformat.stick_x = -1;
    readformat.stick_y = -1;

    readformatgcn.dummy = CONT_CMD_NOP;
    readformatgcn.txsize = CONT_CMD_GCN_SHORTPOLL_TX;
    readformatgcn.rxsize = CONT_CMD_GCN_SHORTPOLL_RX;
    readformatgcn.cmd = CONT_CMD_GCN_SHORTPOLL;
    readformatgcn.analog_mode = 3;
    readformatgcn.rumble = 0;
    readformatgcn.button = 0xFFFF;
    readformatgcn.stick_x = -1;
    readformatgcn.stick_y = -1;

    for (i = 0; i < __osMaxControllers; i++) {
        if ((gControllerStatuses[i].type & CONT_CONSOLE_MASK) == CONT_CONSOLE_GCN) {
            readformatgcn.rumble = __osGamecubeRumbleEnabled[i];
            *(__OSContGCNShortPollFormat*)ptr = readformatgcn;
            ptr += sizeof(__OSContGCNShortPollFormat);
        } else {
            *(__OSContReadFormat*)ptr = readformat;
            ptr += sizeof(__OSContReadFormat);
        }
    }
    
    *ptr = CONT_CMD_END;
}

static u16 __osTranslateGCNButtons(u16 input, s32 c_stick_x, s32 c_stick_y) {
    u16 ret = 0;

    // Face buttons
    if (input & CONT_GCN_A) {
        ret |= A_BUTTON;
    }
    if (input & CONT_GCN_B) {
        ret |= B_BUTTON;
    }
    if (input & CONT_GCN_START) {
        ret |= START_BUTTON;
    }
    if (input & CONT_GCN_X) {
        ret |= GCN_X_BUTTON;
    }
    if (input & CONT_GCN_Y) {
        ret |= GCN_Y_BUTTON;
    }

    // Triggers & Z
    if (input & CONT_GCN_Z) {
        ret |= Z_TRIG;
    }
    if (input & CONT_GCN_R) {
        ret |= R_TRIG;
    }
    if (input & CONT_GCN_L) {
        ret |= L_TRIG;
    }

    // D-Pad
    if (input & CONT_GCN_UP) {
        ret |= U_JPAD;
    }
    if (input & CONT_GCN_DOWN) {
        ret |= D_JPAD;
    }
    if (input & CONT_GCN_LEFT) {
        ret |= L_JPAD;
    }
    if (input & CONT_GCN_RIGHT) {
        ret |= R_JPAD;
    }

    // C-stick to C-buttons
    if (c_stick_x > GCN_C_STICK_THRESHOLD) {
        ret |= R_CBUTTONS;
    }
    if (c_stick_x < -GCN_C_STICK_THRESHOLD) {
        ret |= L_CBUTTONS;
    }
    if (c_stick_y > GCN_C_STICK_THRESHOLD) {
        ret |= U_CBUTTONS;
    }
    if (c_stick_y < -GCN_C_STICK_THRESHOLD) {
        ret |= D_CBUTTONS;
    }

    return ret;
}

//////////////////
// controller.c //
//////////////////

extern s32 __osContinitialized;

extern OSPifRam __osContPifRam;
extern u8 __osContLastCmd;
extern u8 __osMaxControllers;
extern u8 __osGamecubeRumbleEnabled[MAXCONTROLLERS];



/////////////
// motor.c //
/////////////

/* SI transfers require 8-byte alignment. Give each 64-byte packet whole
 * 16-byte cache lines too, so DMA cannot corrupt adjacent CPU-owned state. */
static OSPifRam __MotorDataBuf[MAXCONTROLLERS] ALIGNED16;

#define READFORMAT(ptr) ((__OSContRamReadFormat*)(ptr))

// FALSE retains SI and outer rumble ownership for an unfinished motor DMA.
s32 osMotorStopExForReset(OSPfs *pfs) {
    u8 *ptr;

    if (pfs->channel < 0 || pfs->channel >= MAXCONTROLLERS || !(pfs->status & PFS_MOTOR_INITIALIZED)) {
        return TRUE;
    }
    if ((gControllerStatuses[pfs->channel].type & CONT_CONSOLE_MASK) == CONT_CONSOLE_GCN) {
        __osGamecubeRumbleEnabled[pfs->channel] = FALSE;
        __osContLastCmd = CONT_CMD_END;
        return TRUE;
    }
    if (!__osSiAccessQueueEnabled || osRecvMesg(&__osSiAccessQueue, NULL, OS_MESG_NOBLOCK) != 0) {
        return TRUE;
    }
    if (__osSiDeviceBusy()) {
        __osSiRelAccess();
        return TRUE;
    }
    while (osRecvMesg(pfs->queue, NULL, OS_MESG_NOBLOCK) == 0) {
    }
    __MotorDataBuf[pfs->channel].pifstatus = CONT_CMD_EXE;
    ptr = (u8 *) &__MotorDataBuf[pfs->channel] + pfs->channel;
    for (s32 i = 0; i < BLOCKSIZE; i++) {
        READFORMAT(ptr)->data[i] = 0;
    }
    __osContLastCmd = CONT_CMD_END;
    if (__osSiRawStartDma(OS_WRITE, &__MotorDataBuf[pfs->channel]) != 0) {
        __osSiRelAccess();
        return TRUE;
    }
    // Reset is already pending here; only the bounded completion deadline applies.
    if (controller_wait_bounded(pfs->queue, NULL) == CRASH_CONTROLLER_DMA_PENDING) {
        return FALSE;
    }
    if (__osSiRawStartDma(OS_READ, &__MotorDataBuf[pfs->channel]) != 0) {
        __osSiRelAccess();
        return TRUE;
    }
    if (controller_wait_bounded(pfs->queue, NULL) == CRASH_CONTROLLER_DMA_PENDING) {
        return FALSE;
    }
    __osSiRelAccess();
    return TRUE;
}

s32 __osMotorAccessEx(OSPfs* pfs, s32 flag) {
    int i;
    s32 ret = 0;
    u8* ptr = (u8*)&__MotorDataBuf[pfs->channel];

    if (!(pfs->status & PFS_MOTOR_INITIALIZED)) {
        return 5;
    }

    if ((gControllerStatuses[pfs->channel].type & CONT_CONSOLE_MASK) == CONT_CONSOLE_GCN) {
        __osGamecubeRumbleEnabled[pfs->channel] = flag;
        __osContLastCmd = CONT_CMD_END;
    } else {
        __osSiGetAccess();
        __MotorDataBuf[pfs->channel].pifstatus = CONT_CMD_EXE;
        ptr += pfs->channel;

        for (i = 0; i < BLOCKSIZE; i++) {
            READFORMAT(ptr)->data[i] = flag;
        }

        __osContLastCmd = CONT_CMD_END;
        ret = controller_start_dma(OS_WRITE, &__MotorDataBuf[pfs->channel]);
        if (ret != 0) {
            __osSiRelAccess();
            return ret;
        }
        osRecvMesg(pfs->queue, NULL, OS_MESG_BLOCK);
        ret = controller_start_dma(OS_READ, &__MotorDataBuf[pfs->channel]);
        if (ret != 0) {
            __osSiRelAccess();
            return ret;
        }
        osRecvMesg(pfs->queue, NULL, OS_MESG_BLOCK);

        ret = READFORMAT(ptr)->rxsize & CHNL_ERR_MASK;
        if (!ret) {
            if (!flag) {
                if (READFORMAT(ptr)->datacrc != 0) {
                    ret = PFS_ERR_CONTRFAIL;
                }
            } else {
                if (READFORMAT(ptr)->datacrc != 0xEB) {
                    ret = PFS_ERR_CONTRFAIL;
                }
            }
        }
        __osSiRelAccess();
    }

    return ret;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
static void _MakeMotorData(int channel, OSPifRam *mdata) {
    u8 *ptr = (u8 *)mdata->ramarray;
    __OSContRamReadFormat ramreadformat;
    int i;

    ramreadformat.dummy = CONT_CMD_NOP;
    ramreadformat.txsize = CONT_CMD_WRITE_PAK_TX;
    ramreadformat.rxsize = CONT_CMD_WRITE_PAK_RX;
    ramreadformat.cmd = CONT_CMD_WRITE_PAK;
    ramreadformat.addrh = CONT_BLOCK_RUMBLE >> 3;
    ramreadformat.addrl = (u8)(__osContAddressCrc(CONT_BLOCK_RUMBLE) | (CONT_BLOCK_RUMBLE << 5));
    
    if (channel != 0) {
        for (i = 0; i < channel; i++) {
            *ptr++ = CONT_CMD_REQUEST_STATUS;
        }
    }

    *READFORMAT(ptr) = ramreadformat;
    ptr += sizeof(__OSContRamReadFormat);
    ptr[0] = CONT_CMD_END;
}

s32 osMotorInitEx(OSMesgQueue *mq, OSPfs *pfs, int channel)
{
    s32 ret;
    u8 temp[32];

    pfs->queue = mq;
    pfs->channel = channel;
    pfs->activebank = 0xFF;
    pfs->status = 0;

    if ((gControllerStatuses[pfs->channel].type & CONT_CONSOLE_MASK) == CONT_CONSOLE_N64) {
        ret = __osPfsSelectBank(pfs, 0xFE);
        
        if (ret == PFS_ERR_NEW_PACK) {
            ret = __osPfsSelectBank(pfs, 0x80);
        }

        if (ret != 0) {
            return ret;
        }

        ret = __osContRamRead(mq, channel, CONT_BLOCK_DETECT, temp);

        if (ret == PFS_ERR_NEW_PACK) {
            ret = PFS_ERR_CONTRFAIL;
        }

        if (ret != 0) {
            return ret;
        }

        if (temp[31] == 254) {
            return PFS_ERR_DEVICE;
        }

        ret = __osPfsSelectBank(pfs, 0x80);
        if (ret == PFS_ERR_NEW_PACK) {
            ret = PFS_ERR_CONTRFAIL;
        }
        
        if (ret != 0) {
            return ret;
        }

        ret = __osContRamRead(mq, channel, CONT_BLOCK_DETECT, temp);
        if (ret == PFS_ERR_NEW_PACK) {
            ret = PFS_ERR_CONTRFAIL;
        }
        
        if (ret != 0) {
            return ret;
        }
        
        if (temp[31] != 0x80) {
            return PFS_ERR_DEVICE;
        }

        if (!(pfs->status & PFS_MOTOR_INITIALIZED)) {
            _MakeMotorData(channel, &__MotorDataBuf[channel]);
        }
    }

    pfs->status = PFS_MOTOR_INITIALIZED;
    return 0;
}
#pragma GCC diagnostic pop
