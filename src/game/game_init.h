#ifndef GAME_INIT_H
#define GAME_INIT_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include <PR/os_cont.h>
#include <PR/os_message.h>

#include "types.h"
#include "memory.h"
#include "config.h"
#include "emutest.h"

// Crash polling returns -1 without an outstanding transfer, or -2 retaining SI ownership.
#define CRASH_CONTROLLER_DMA_PENDING (-2)
s32 osContReadDataExForCrash(OSMesgQueue *mq, s32 (*resetPending)(void));
s32 osContStartQueryEx(OSMesgQueue *mq);
// TRUE releases outer rumble ownership; FALSE retains it for unfinished motor DMA.
s32 osMotorStopExForReset(OSPfs *pfs);

#define PAUSE_BLEND_FRAMES 3

#define MARIO_ANIMS_POOL_SIZE 0x4000
#define DEMO_INPUTS_POOL_SIZE 0x800

// Emulators that the Instant Input patch should be applied to
#define INSTANT_INPUT_WHITELIST (EMU_PARALLEL_LAUNCHER | EMU_PROJECT64 | EMU_MUPEN)

struct GfxPool {
    Gfx buffer[GFX_POOL_SIZE];
    struct SPTask spTask;
};

struct DemoInput {
    u8 timer; // time until next input. if this value is 0, it means the demo is over
    s8 rawStickX;
    s8 rawStickY;
    u8 buttonMask;
};

enum ZBmodes {
    KEEP_ZBUFFER = 0,
    CLEAR_ZBUFFER = 1,
};

extern struct Controller gControllers[MAXCONTROLLERS];
extern OSContStatus gControllerStatuses[MAXCONTROLLERS];
extern OSContPadEx gControllerPads[MAXCONTROLLERS];
extern OSMesgQueue gGameVblankQueue;
extern OSMesgQueue gGfxVblankQueue;
extern OSMesg gGameMesgBuf[1];
extern OSMesg gGfxMesgBuf[1];
extern struct VblankHandler gGameVblankHandler;
extern uintptr_t gPhysicalFramebuffers[3];
extern uintptr_t gPhysicalZBuffer;
extern void *gMarioAnimsMemAlloc;
extern void *gDemoInputsMemAlloc;
extern struct SPTask *gGfxSPTask;
extern Gfx *gDisplayListHead;
extern u8 *gGfxPoolEnd;
extern struct GfxPool *gGfxPool;
extern u8 gControllerBits;
extern u8 gBorderHeight;
#ifdef VANILLA_STYLE_CUSTOM_DEBUG
extern u8 gCustomDebugMode;
#endif
extern u8 *gAreaSkyboxStart[AREA_COUNT];
extern u8 *gAreaSkyboxEnd[AREA_COUNT];
#ifdef SRAM
extern s8 gSramProbe;
#endif

extern void (*gGoddardVblankCallback)(void);
extern struct Controller* const gPlayer1Controller;
extern struct Controller* const gPlayer2Controller;
extern struct Controller* const gPlayer3Controller;
extern struct Controller* const gPlayer4Controller;
void adjust_analog_stick(struct Controller *controller);
extern struct DemoInput *gCurrDemoInput;
extern u16 gDemoInputListID;
extern struct DemoInput gRecordedDemoInput;

// this area is the demo input + the header. when the demo is loaded in, there is a header the size
// of a single word next to the input list. this word is the current ID count.
extern struct DmaHandlerList gMarioAnimsBuf;
extern struct DmaHandlerList gDemoInputsBuf;

extern u8 gMarioAnims[];
extern u8 gDemoInputs[];

extern u16 sRenderingFramebuffer;
void game_present_completed_frame(u16 framebuffer);
extern u32 gGlobalTimer;

void setup_game_memory(void);
void thread5_game_loop(UNUSED void *arg);
void clear_framebuffer(s32 color);
void clear_viewport(Vp *viewport, s32 color);
void make_viewport_clip_rect(Vp *viewport);
void init_rcp(s32 resetZB);
// Call before scene drawing and after all HUD/menu/debug drawing, respectively.
void prepare_pause_screen_blend(s32 normalViewport);
void render_pause_screen_blend(void);
void render_pause_world_native(s32 normalViewport);
void prepare_debug_overlay(void);
void end_master_display_list(void);
void render_init(void);
void select_gfx_pool(void);
void display_and_vsync(void);

#endif // GAME_INIT_H
