#include <ultra64.h>

#include "sm64.h"
#include "mario_character.h"
#include "gfx_dimensions.h"
#include "audio/external.h"
#include "buffers/buffers.h"
#include "buffers/gfx_output_buffer.h"
#include "buffers/framebuffers.h"
#include "buffers/zbuffer.h"
#include "debugger/assert.h"
#include "engine/level_script.h"
#include "engine/math_util.h"
#include "game_init.h"
#include "game_options.h"
#include "game_options_runtime.h"
#include "game_options_input.h"
#include "ingame_menu.h"
#include "dialog_ids.h"
#include "object_list_processor.h"
#include "level_table.h"
#include "main.h"
#include "memory.h"
#include "save_file.h"
#include "seq_ids.h"
#include "sound_init.h"
#include "print.h"
#include "segment2.h"
#include "segment_symbols.h"
#include "rumble_init.h"
#ifdef SRAM
#include "sram.h"
#endif
#include "puppyprint.h"
#ifdef REONUCAM
#include "camera.h"
#endif
#include "debug_box.h"
#include "profiling.h"
#include "debug.h"
#include "emutest.h"
#include "area.h"
#include "level_update.h"
#include "gcc_profile.h"
#include "input_display.h"

// Gfx handlers
struct SPTask *gGfxSPTask;
Gfx *gDisplayListHead;
u8 *gGfxPoolEnd;
struct GfxPool *gGfxPool;

// OS Controllers
struct Controller gControllers[MAXCONTROLLERS];
OSContStatus gControllerStatuses[MAXCONTROLLERS];
OSContPadEx gControllerPads[MAXCONTROLLERS];
u8 gControllerBits = 0b0000;
u8 gBorderHeight;
#ifdef VANILLA_STYLE_CUSTOM_DEBUG
u8 gCustomDebugMode;
#endif
#ifdef SRAM
s8 gSramProbe;
#endif
OSMesgQueue gGameVblankQueue;
OSMesgQueue gGfxVblankQueue;
OSMesg gGameMesgBuf[1];
OSMesg gGfxMesgBuf[1];

// Vblank Handler
struct VblankHandler gGameVblankHandler;

// Buffers
uintptr_t gPhysicalFramebuffers[3];
uintptr_t gPhysicalZBuffer;

// Mario Anims and Demo allocation
void *gMarioAnimsMemAlloc;
void *gDemoInputsMemAlloc;
struct DmaHandlerList gMarioAnimsBuf;
struct DmaHandlerList gDemoInputsBuf;

// General timer that runs as the game starts
u32 gGlobalTimer = 0;
u8 *gAreaSkyboxStart[AREA_COUNT];
u8 *gAreaSkyboxEnd[AREA_COUNT];

// Framebuffer rendering values (max 3)
u16 sRenderedFramebuffer = 0;
u16 sRenderingFramebuffer = 0;

// Goddard Vblank Function Caller
void (*gGoddardVblankCallback)(void) = NULL;

// Defined player slots. Anything above MAX_NUM_PLAYERS should not be used.
struct Controller* const gPlayer1Controller = &gControllers[0];
struct Controller* const gPlayer2Controller = &gControllers[1];
struct Controller* const gPlayer3Controller = &gControllers[2];
struct Controller* const gPlayer4Controller = &gControllers[3];

// Title Screen Demo Handler
struct DemoInput *gCurrDemoInput = NULL;
u16 gDemoInputListID = 0;
struct DemoInput gRecordedDemoInput = { 0 };

// Display
// ----------------------------------------------------------------------------------------------------

/**
 * Sets the initial RDP (Reality Display Processor) rendering settings.
 */
const Gfx init_rdp[] = {
    gsDPPipeSync(),
    gsDPPipelineMode(G_PM_NPRIMITIVE),

    gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),

    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetTextureDetail(G_TD_CLAMP),
    gsDPSetTexturePersp(G_TP_PERSP),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetTextureConvert(G_TC_FILT),

    gsDPSetCombineKey(G_CK_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetBlendColor(0, 0, 0, 0),
    gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
    gsDPSetColorDither(G_CD_MAGICSQ),
    gsDPSetCycleType(G_CYC_FILL),
    gsDPSetAlphaDither(G_AD_PATTERN),
    gsSPEndDisplayList(),
};

/**
 * Sets the initial RSP (Reality Signal Processor) settings.
 */
const Gfx init_rsp[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_CULL_FRONT | G_FOG | G_LIGHTING
#ifdef G_LIGHTING_POSITIONAL
                          | G_LIGHTING_POSITIONAL
#endif
                          | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD),
    gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK | G_LIGHTING),
    gsSPTexture(0, 0, 0, G_TX_RENDERTILE, G_OFF),
#ifdef USE_FRUSTRATIO2
    gsSPClipRatio(FRUSTRATIO_2),
#else
    gsSPClipRatio(FRUSTRATIO_1),
#endif
    gsSPEndDisplayList(),
};

#ifdef S2DEX_TEXT_ENGINE
void my_rdp_init(void) {
    gSPDisplayList(gDisplayListHead++, init_rdp);
}

void my_rsp_init(void) {
    gSPDisplayList(gDisplayListHead++, init_rsp);
}
#endif

/**
 * Initialize the z buffer for the current frame.
 */
void init_z_buffer(s32 resetZB) {
    Gfx *tempGfxHead = gDisplayListHead;

    gDPPipeSync(tempGfxHead++);

    gDPSetDepthSource(tempGfxHead++, G_ZS_PIXEL);
    gDPSetDepthImage(tempGfxHead++, gPhysicalZBuffer);

    gDPSetColorImage(tempGfxHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH, gPhysicalZBuffer);
    if (!resetZB)
        return;
    gDPSetFillColor(tempGfxHead++,
                    GPACK_ZDZ(G_MAXFBZ, 0) << 16 | GPACK_ZDZ(G_MAXFBZ, 0));

    gDPFillRectangle(tempGfxHead++, 0, gBorderHeight, SCREEN_WIDTH - 1,
                     SCREEN_HEIGHT - 1 - gBorderHeight);

    gDisplayListHead = tempGfxHead;
}

/**
 * Tells the RDP which of the three framebuffers it shall draw to.
 */
void select_framebuffer(void) {
    Gfx *tempGfxHead = gDisplayListHead;

    gDPPipeSync(tempGfxHead++);

    gDPSetCycleType(tempGfxHead++, G_CYC_1CYCLE);
    gDPSetColorImage(tempGfxHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH,
                     gPhysicalFramebuffers[sRenderingFramebuffer]);
    gDPSetScissor(tempGfxHead++, G_SC_NON_INTERLACE, 0, gBorderHeight, SCREEN_WIDTH,
                  SCREEN_HEIGHT - gBorderHeight);

    gDisplayListHead = tempGfxHead;
}

/**
 * Clear the framebuffer and fill it with a 32-bit color.
 * Information about the color argument: https://jrra.zone/n64/doc/n64man/gdp/gDPSetFillColor.htm
 */
void clear_framebuffer(s32 color) {
    Gfx *tempGfxHead = gDisplayListHead;

    gDPPipeSync(tempGfxHead++);

    gDPSetRenderMode(tempGfxHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCycleType(tempGfxHead++, G_CYC_FILL);

    gDPSetFillColor(tempGfxHead++, color);
    gDPFillRectangle(tempGfxHead++,
                     GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(0), gBorderHeight,
                     GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(0) - 1, SCREEN_HEIGHT - gBorderHeight - 1);

    gDPPipeSync(tempGfxHead++);

    gDPSetCycleType(tempGfxHead++, G_CYC_1CYCLE);

    gDisplayListHead = tempGfxHead;
}

/**
 * Resets the viewport, readying it for the final image.
 */
void clear_viewport(Vp *viewport, s32 color) {
    s16 vpUlx = (viewport->vp.vtrans[0] - viewport->vp.vscale[0]) / 4 + 1;
    s16 vpUly = (viewport->vp.vtrans[1] - viewport->vp.vscale[1]) / 4 + 1;
    s16 vpLrx = (viewport->vp.vtrans[0] + viewport->vp.vscale[0]) / 4 - 2;
    s16 vpLry = (viewport->vp.vtrans[1] + viewport->vp.vscale[1]) / 4 - 2;

#ifdef WIDESCREEN
    vpUlx = GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(vpUlx);
    vpLrx = GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(SCREEN_WIDTH - vpLrx);
#endif

    Gfx *tempGfxHead = gDisplayListHead;

    gDPPipeSync(tempGfxHead++);

    gDPSetRenderMode(tempGfxHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCycleType(tempGfxHead++, G_CYC_FILL);

    gDPSetFillColor(tempGfxHead++, color);
    gDPFillRectangle(tempGfxHead++, vpUlx, vpUly, vpLrx, vpLry);

    gDPPipeSync(tempGfxHead++);

    gDPSetCycleType(tempGfxHead++, G_CYC_1CYCLE);

    gDisplayListHead = tempGfxHead;
}

/**
 * Draw the horizontal screen borders.
 */
void draw_screen_borders(void) {
    Gfx *tempGfxHead = gDisplayListHead;

    gDPPipeSync(tempGfxHead++);

    gDPSetScissor(tempGfxHead++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gDPSetRenderMode(tempGfxHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCycleType(tempGfxHead++, G_CYC_FILL);

    gDPSetFillColor(tempGfxHead++, GPACK_RGBA5551(0, 0, 0, 0) << 16 | GPACK_RGBA5551(0, 0, 0, 0));

    if (gBorderHeight) {
        gDPFillRectangle(tempGfxHead++, GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(0), 0,
                        GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(0) - 1, gBorderHeight - 1);
        gDPFillRectangle(tempGfxHead++,
                        GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(0), SCREEN_HEIGHT - gBorderHeight,
                        GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(0) - 1, SCREEN_HEIGHT - 1);
    }

    gDisplayListHead = tempGfxHead;
}

/**
 * Defines the viewport scissoring rectangle.
 * Scissoring: https://jrra.zone/n64/doc/pro-man/pro12/12-03.htm#01
 */
void make_viewport_clip_rect(Vp *viewport) {
    s16 vpUlx = (viewport->vp.vtrans[0] - viewport->vp.vscale[0]) / 4 + 1;
    s16 vpPly = (viewport->vp.vtrans[1] - viewport->vp.vscale[1]) / 4 + 1;
    s16 vpLrx = (viewport->vp.vtrans[0] + viewport->vp.vscale[0]) / 4 - 1;
    s16 vpLry = (viewport->vp.vtrans[1] + viewport->vp.vscale[1]) / 4 - 1;

    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, vpUlx, vpPly, vpLrx, vpLry);
}

static s32 video_low_latency_enabled(void) {
#if VIDEO_LOW_LATENCY && !defined(UNLOCK_FPS)
    // These emulators already display graphics tasks directly through their HLE path.
    return !(gEmulator & INSTANT_INPUT_WHITELIST);
#else
    return FALSE;
#endif
}

// Only the DP-complete handler may publish a buffer that the RDP has finished.
void game_present_completed_frame(u16 framebuffer) {
    osViSwapBuffer((void *) PHYSICAL_TO_VIRTUAL(gPhysicalFramebuffers[framebuffer]));
    sRenderedFramebuffer = framebuffer;
}

/**
 * Initializes the Fast3D OSTask structure.
 * If you plan on using gSPLoadUcode, make sure to add OS_TASK_LOADABLE to the flags member.
 */
void create_gfx_task_structure(void) {
    s32 entries = gDisplayListHead - gGfxPool->buffer;

    gGfxSPTask->framebuffer = sRenderingFramebuffer;
    gGfxSPTask->presentOnComplete = video_low_latency_enabled();
    gGfxSPTask->msgqueue = &gGfxVblankQueue;
    gGfxSPTask->msg = (OSMesg) 2;
    gGfxSPTask->task.t.type = M_GFXTASK;
    gGfxSPTask->task.t.ucode_boot = rspbootTextStart;
    gGfxSPTask->task.t.ucode_boot_size = ((u8 *) rspbootTextEnd - (u8 *) rspbootTextStart);
    gGfxSPTask->task.t.flags = (OS_TASK_LOADABLE | OS_TASK_DP_WAIT);
#ifdef  L3DEX2_ALONE
    gGfxSPTask->task.t.ucode = gspL3DEX2_fifoTextStart;
    gGfxSPTask->task.t.ucode_data = gspL3DEX2_fifoDataStart;
    gGfxSPTask->task.t.ucode_size = ((u8 *) gspL3DEX2_fifoTextEnd - (u8 *) gspL3DEX2_fifoTextStart);
    gGfxSPTask->task.t.ucode_data_size = ((u8 *) gspL3DEX2_fifoDataEnd - (u8 *) gspL3DEX2_fifoDataStart);
#elif  F3DZEX_GBI_2
    gGfxSPTask->task.t.ucode = gspF3DZEX2_PosLight_fifoTextStart;
    gGfxSPTask->task.t.ucode_data = gspF3DZEX2_PosLight_fifoDataStart;
    gGfxSPTask->task.t.ucode_size = ((u8 *) gspF3DZEX2_PosLight_fifoTextEnd - (u8 *) gspF3DZEX2_PosLight_fifoTextStart);
    gGfxSPTask->task.t.ucode_data_size = ((u8 *) gspF3DZEX2_PosLight_fifoDataEnd - (u8 *) gspF3DZEX2_PosLight_fifoDataStart);
#elif  F3DZEX_NON_GBI_2
    gGfxSPTask->task.t.ucode = gspF3DZEX2_NoN_PosLight_fifoTextStart;
    gGfxSPTask->task.t.ucode_data = gspF3DZEX2_NoN_PosLight_fifoDataStart;
    gGfxSPTask->task.t.ucode_size = ((u8 *) gspF3DZEX2_NoN_PosLight_fifoTextEnd - (u8 *) gspF3DZEX2_NoN_PosLight_fifoTextStart);
    gGfxSPTask->task.t.ucode_data_size = ((u8 *) gspF3DZEX2_NoN_PosLight_fifoDataEnd - (u8 *) gspF3DZEX2_NoN_PosLight_fifoDataStart);
#elif   F3DEX2PL_GBI
    gGfxSPTask->task.t.ucode = gspF3DEX2_PosLight_fifoTextStart;
    gGfxSPTask->task.t.ucode_data = gspF3DEX2_PosLight_fifoDataStart;
    gGfxSPTask->task.t.ucode_size = ((u8 *) gspF3DEX2_PosLight_fifoTextEnd - (u8 *) gspF3DEX2_PosLight_fifoTextStart);
    gGfxSPTask->task.t.ucode_data_size = ((u8 *) gspF3DEX2_PosLight_fifoDataEnd - (u8 *) gspF3DEX2_PosLight_fifoDataStart);
#elif   F3DEX_GBI_2
    gGfxSPTask->task.t.ucode = gspF3DEX2_fifoTextStart;
    gGfxSPTask->task.t.ucode_data = gspF3DEX2_fifoDataStart;
    gGfxSPTask->task.t.ucode_size = ((u8 *) gspF3DEX2_fifoTextEnd - (u8 *) gspF3DEX2_fifoTextStart);
    gGfxSPTask->task.t.ucode_data_size = ((u8 *) gspF3DEX2_fifoDataEnd - (u8 *) gspF3DEX2_fifoDataStart);
#elif   F3DEX_GBI
    gGfxSPTask->task.t.ucode = gspF3DEX_fifoTextStart;
    gGfxSPTask->task.t.ucode_data = gspF3DEX_fifoDataStart;
    gGfxSPTask->task.t.ucode_size = ((u8 *) gspF3DEX_fifoTextEnd - (u8 *) gspF3DEX_fifoTextStart);
    gGfxSPTask->task.t.ucode_data_size = ((u8 *) gspF3DEX_fifoDataEnd - (u8 *) gspF3DEX_fifoDataStart);
#elif   SUPER3D_GBI
    gGfxSPTask->task.t.ucode = gspSuper3DTextStart;
    gGfxSPTask->task.t.ucode_data = gspSuper3DDataStart;
    gGfxSPTask->task.t.ucode_size = ((u8 *) gspSuper3DTextEnd - (u8 *) gspSuper3DTextStart);
    gGfxSPTask->task.t.ucode_data_size = ((u8 *) gspSuper3DDataEnd - (u8 *) gspSuper3DDataStart);
#else
    gGfxSPTask->task.t.ucode = gspFast3D_fifoTextStart;
    gGfxSPTask->task.t.ucode_data = gspFast3D_fifoDataStart;
    gGfxSPTask->task.t.ucode_size = ((u8 *) gspFast3D_fifoTextEnd - (u8 *) gspFast3D_fifoTextStart);
    gGfxSPTask->task.t.ucode_data_size = ((u8 *) gspFast3D_fifoDataEnd - (u8 *) gspFast3D_fifoDataStart);
#endif
    gGfxSPTask->task.t.dram_stack = (u64 *) gGfxSPTaskStack;
    gGfxSPTask->task.t.dram_stack_size = SP_DRAM_STACK_SIZE8;
    gGfxSPTask->task.t.output_buff = gGfxSPTaskOutputBuffer;
    gGfxSPTask->task.t.output_buff_size =
        (u64 *)((u8 *) gGfxSPTaskOutputBuffer + sizeof(gGfxSPTaskOutputBuffer));
    gGfxSPTask->task.t.data_ptr = (u64 *) &gGfxPool->buffer;
    gGfxSPTask->task.t.data_size = entries * sizeof(Gfx);
    gGfxSPTask->task.t.yield_data_ptr = (u64 *) gGfxSPTaskYieldBuffer;
    gGfxSPTask->task.t.yield_data_size = OS_YIELD_DATA_SIZE;

    // NOTE: 'entries' is not representative of the right-side allocations coming from the GFX pool; do not use that variable here.
    assertf((u8*) gDisplayListHead <= gGfxPoolEnd, "GFX pool exceeded: %d command(s) over!", ((s32) gGfxPoolEnd - (s32) gDisplayListHead) / sizeof(Gfx));
}

/**
 * Set default RCP (Reality Co-Processor) settings.
 */
void init_rcp(s32 resetZB) {
    move_segment_table_to_dmem();
    gSPDisplayList(gDisplayListHead++, init_rdp);
    gSPDisplayList(gDisplayListHead++, init_rsp);
    init_z_buffer(resetZB);
    select_framebuffer();
}

// Six RGBA16 rows use 3840 of TMEM's 4096 bytes at the 320-pixel screen width.
// Reserve for the longer load-tile macro variant
// and the three-command texture rectangle, plus setup/restore commands.
#define PAUSE_BLEND_STRIP_HEIGHT 6
#define PAUSE_BLEND_STRIP_COUNT                                                                        \
    ((SCREEN_HEIGHT + PAUSE_BLEND_STRIP_HEIGHT - 1) / PAUSE_BLEND_STRIP_HEIGHT)
#define PAUSE_BLEND_DRAW_COMMANDS (PAUSE_BLEND_STRIP_COUNT * 24 + 32)
#define PAUSE_BLEND_COPY_COMMANDS (PAUSE_BLEND_STRIP_COUNT * 12 + 32)
#define PAUSE_BLEND_TAIL_COMMANDS 16

static struct {
    struct Area *area;
    s16 level;
    u32 frame;
    s32 historyValid;
    s32 wasPaused;
    u16 previousFramebuffer;
    Gfx *displayList;
    Gfx *copyCall;
    u16 source;
    s32 pending;
    s32 active;
    u32 startedFrame;
    u8 alpha;
#ifdef INPUT_DISPLAY
    // Preserve the clean background under the live overlay, including interrupted fades.
    union {
        u64 alignment;
        u16 pixels[SCREEN_HEIGHT][INPUT_DISPLAY_BACKGROUND_WIDTH];
    } inputBackgrounds[2] __attribute__((aligned(16)));
    // Latest capture is distinct from the source pinned during an active fade.
    u32 inputBackgroundFrame;
    s32 latestInputBackgroundValid;
    s32 inputBlendSourceValid;
    u8 latestInputBackground;
    u8 inputBlendSource;
#endif
} sPauseBlend;

static Gfx *pause_blend_draw_image(Gfx *dl, const void *image, s32 width, s32 left, s32 right, s32 copy,
                                   u8 alpha) {
    s32 y;
    s32 bottom;

    gDPPipeSync(dl++);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gDPSetCycleType(dl++, copy ? G_CYC_COPY : G_CYC_1CYCLE);
    gDPSetRenderMode(dl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetTexturePersp(dl++, G_TP_NONE);
    gDPSetTextureFilter(dl++, G_TF_POINT);
    gDPSetTextureLOD(dl++, G_TL_TILE);
    gDPSetTextureLUT(dl++, G_TT_NONE);
    gDPSetTextureDetail(dl++, G_TD_CLAMP);
    gDPSetAlphaCompare(dl++, G_AC_NONE);
    gDPSetColorDither(dl++, G_CD_DISABLE);
    gDPSetAlphaDither(dl++, G_AD_DISABLE);
    // Framebuffer alpha stores coverage, so use a constant blend weight instead.
    gDPSetCombineLERP(dl++, 0, 0, 0, TEXEL0, 0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0, 0, 0, 0, PRIMITIVE);
    gDPSetPrimColor(dl++, 0, 0, 255, 255, 255, alpha);

    for (y = 0; y < SCREEN_HEIGHT; y += PAUSE_BLEND_STRIP_HEIGHT) {
        bottom = MIN(y + PAUSE_BLEND_STRIP_HEIGHT, SCREEN_HEIGHT);
        gDPLoadTextureTile(dl++, image, G_IM_FMT_RGBA, G_IM_SIZ_16b, width, SCREEN_HEIGHT, left, y,
                           right - 1, bottom - 1, 0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK,
                           G_TX_NOLOD, G_TX_NOLOD);
        // Copy includes its lower/right edges; one-cycle drawing excludes them.
        gSPTextureRectangle(dl++, left << 2, y << 2, (right - copy) << 2, (bottom - copy) << 2,
                            G_TX_RENDERTILE, left << 5, y << 5, (copy ? 4 : 1) << 10, 1 << 10);
    }
    gDPPipeSync(dl++);
    return dl;
}

static Gfx *pause_blend_draw_framebuffer(Gfx *dl, u16 source, s32 copy, u8 alpha) {
    s32 left = 0;
#ifdef INPUT_DISPLAY
    if (!copy && sPauseBlend.inputBlendSourceValid) {
        left = MAX(0, GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(INPUT_DISPLAY_BACKGROUND_WIDTH));
    }
#endif
    dl = pause_blend_draw_image(dl, (const void *) gPhysicalFramebuffers[source], SCREEN_WIDTH, left,
                                SCREEN_WIDTH, copy, alpha);
#ifdef INPUT_DISPLAY
    if (left > 0) {
        dl = pause_blend_draw_image(dl,
                                    sPauseBlend.inputBackgrounds[sPauseBlend.inputBlendSource].pixels,
                                    INPUT_DISPLAY_BACKGROUND_WIDTH, 0, left, FALSE, alpha);
    }
#endif
    return dl;
}

static void pause_blend_end_display_list(Gfx *dl) {
    gSPDisplayList(dl++, init_rdp);
    gDPSetPrimColor(dl++, 0, 0, 255, 255, 255, 255);
    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gDPSetScissor(dl++, G_SC_NON_INTERLACE, 0, gBorderHeight, SCREEN_WIDTH,
                  SCREEN_HEIGHT - gBorderHeight);
    gSPEndDisplayList(dl);
}

void render_pause_world_native(s32 normalViewport) {
    Gfx *dl;
    u32 size = PAUSE_BLEND_COPY_COMMANDS * sizeof(Gfx);
    if (!(gEmulator & EMU_PARALLEL_LAUNCHER) || sCurrPlayMode != PLAY_MODE_PAUSED
        || gWarpTransition.isActive || !normalViewport) {
        return;
    }
    if (gGfxPoolEnd - (u8 *) gDisplayListHead
        < (s32) (size + PAUSE_BLEND_TAIL_COMMANDS * sizeof(Gfx))) {
        return;
    }
    dl = alloc_display_list(size);
    if (dl == NULL) {
        return;
    }
    gSPDisplayList(gDisplayListHead++, dl);
    // Sampling the world back into its own framebuffer resolves ParaLLEl's
    // enhanced samples to native pixels. Draw HUD and menu only after this pass.
    dl = pause_blend_draw_image(dl, (const void *) gPhysicalFramebuffers[sRenderingFramebuffer],
                                SCREEN_WIDTH, 0, SCREEN_WIDTH, TRUE, 255);
    pause_blend_end_display_list(dl);
}

void prepare_pause_screen_blend(s32 normalViewport) {
    s32 eligible = gCurrentArea != NULL && gCurrentArea->graphNode != NULL && !gWarpTransition.isActive
                   && !gWarpTransition.pauseRendering && normalViewport;
    s32 paused = sCurrPlayMode == PLAY_MODE_PAUSED;
    s32 continuous = sPauseBlend.historyValid && eligible && sPauseBlend.area == gCurrentArea
                     && sPauseBlend.level == gCurrLevelNum
                     && (u32) (gGlobalTimer - sPauseBlend.frame) == 1;
    s32 changed = continuous && paused != sPauseBlend.wasPaused;
    u32 elapsed;
    // Extra render passes reuse the same saved image and blend, without recopying.
    if (sPauseBlend.historyValid && eligible && gGlobalTimer == sPauseBlend.frame
        && sPauseBlend.area == gCurrentArea && sPauseBlend.level == gCurrLevelNum
        && paused == sPauseBlend.wasPaused) {
        return;
    }
    sPauseBlend.displayList = NULL;
    sPauseBlend.copyCall = NULL;
    if (!continuous) {
        sPauseBlend.active = FALSE;
    }
    if (changed) {
#ifdef INPUT_DISPLAY
        sPauseBlend.inputBlendSourceValid = sPauseBlend.latestInputBackgroundValid
                                            && sPauseBlend.inputBackgroundFrame == sPauseBlend.frame;
        sPauseBlend.inputBlendSource = sPauseBlend.latestInputBackground;
#endif
        sPauseBlend.source = sPauseBlend.previousFramebuffer;
        sPauseBlend.startedFrame = gGlobalTimer;
        sPauseBlend.active = TRUE;
    }
    elapsed = (u32) (gGlobalTimer - sPauseBlend.startedFrame);
    if (elapsed >= PAUSE_BLEND_FRAMES) {
        sPauseBlend.active = FALSE;
    }
    sPauseBlend.pending = sPauseBlend.active;
    sPauseBlend.alpha =
        sPauseBlend.active ? 255 * (PAUSE_BLEND_FRAMES - elapsed) / (PAUSE_BLEND_FRAMES + 1) : 0;
    sPauseBlend.historyValid = eligible;
    sPauseBlend.area = gCurrentArea;
    sPauseBlend.level = gCurrLevelNum;
    sPauseBlend.frame = gGlobalTimer;
    sPauseBlend.wasPaused = paused;
    sPauseBlend.previousFramebuffer = sRenderingFramebuffer;
    if (!sPauseBlend.pending) {
        return;
    }

    if (sPauseBlend.source == sRenderingFramebuffer) {
        // Fill this call after scene construction, when its remaining graphics
        // space is known. The RDP will still execute the copy before scene drawing.
        sPauseBlend.copyCall = gDisplayListHead++;
        gDPNoOp(sPauseBlend.copyCall);
    }
}

void render_pause_screen_blend(void) {
    Gfx *storage;
    Gfx *dl;
    u16 source = sPauseBlend.source;
    s32 copy = sPauseBlend.copyCall != NULL;
    u32 drawCommands = PAUSE_BLEND_COPY_COMMANDS;
    u32 tailCommands = PAUSE_BLEND_TAIL_COMMANDS;
    u32 size;

#ifdef INPUT_DISPLAY
    if (input_display_enabled()) {
        tailCommands += INPUT_DISPLAY_GFX_RESERVE + INPUT_DISPLAY_BACKGROUND_GFX_RESERVE;
    }
    if (sPauseBlend.inputBlendSourceValid) {
        drawCommands = PAUSE_BLEND_DRAW_COMMANDS;
    }
#endif
    size = (drawCommands + (copy ? PAUSE_BLEND_COPY_COMMANDS : 0)) * sizeof(Gfx);

    if (!sPauseBlend.pending) {
        if (sPauseBlend.displayList != NULL) {
            gSPDisplayList(gDisplayListHead++, sPauseBlend.displayList);
        }
        return;
    }
    sPauseBlend.pending = FALSE;

    // Leave room for this call, screen borders and the master list's sync/end.
    if (gGfxPoolEnd - (u8 *) gDisplayListHead < (s32) (size + tailCommands * sizeof(Gfx))) {
        sPauseBlend.active = FALSE;
        return;
    }
    storage = alloc_display_list(size);
    if (storage == NULL) {
        sPauseBlend.active = FALSE;
        return;
    }
    if (copy) {
        // Instant-input emulators redraw one buffer. Save it with the RDP before
        // the scene clears it; this also works without CPU framebuffer emulation.
        source = (sRenderingFramebuffer + 1) % 3;
        dl = storage;
        gDPPipeSync(dl++);
        gDPSetColorImage(dl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH,
                         gPhysicalFramebuffers[source]);
        dl = pause_blend_draw_framebuffer(dl, sRenderingFramebuffer, TRUE, 255);
        gDPSetColorImage(dl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH,
                         gPhysicalFramebuffers[sRenderingFramebuffer]);
        pause_blend_end_display_list(dl);
        gSPDisplayList(sPauseBlend.copyCall, storage);
        // Retain the original image for the remaining blend frames, preserving
        // it again if framebuffer rotation reaches its buffer before we finish.
        sPauseBlend.source = source;
    }
    sPauseBlend.displayList = storage + (copy ? PAUSE_BLEND_COPY_COMMANDS : 0);
    dl = pause_blend_draw_framebuffer(sPauseBlend.displayList, source, FALSE, sPauseBlend.alpha);
    pause_blend_end_display_list(dl);
    gSPDisplayList(gDisplayListHead++, sPauseBlend.displayList);
}

#ifdef INPUT_DISPLAY
static Gfx *sDebugOverlayDisplayList;

void prepare_debug_overlay(void) {
    Gfx *skip = gDisplayListHead++;
    sDebugOverlayDisplayList = gDisplayListHead;
    // Generate this list before the blend's allocation, then execute it above live inputs.
    profiler_print_times();
    render_text_labels();
#ifdef PUPPYPRINT
    puppyprint_print_deferred();
#endif
#ifdef PUPPYPRINT_DEBUG
    puppyprint_render_profiler();
#endif
    gSPEndDisplayList(gDisplayListHead++);
    gSPBranchList(skip, gDisplayListHead);
}

static void capture_input_display_background(void) {
    s32 right = MAX(0, GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(INPUT_DISPLAY_BACKGROUND_WIDTH));
    u8 target;
    if (!input_display_enabled() || !sPauseBlend.historyValid || sPauseBlend.frame != gGlobalTimer
        || right == 0) {
        sPauseBlend.latestInputBackgroundValid = FALSE;
        return;
    }
    if (sPauseBlend.latestInputBackgroundValid && sPauseBlend.inputBackgroundFrame == gGlobalTimer) {
        return;
    }
    // Preserve the active blend source while newer clean strips are captured.
    target =
        1 - (sPauseBlend.active ? sPauseBlend.inputBlendSource : sPauseBlend.latestInputBackground);
    gDPPipeSync(gDisplayListHead++);
    gDPSetColorImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, INPUT_DISPLAY_BACKGROUND_WIDTH,
                     sPauseBlend.inputBackgrounds[target].pixels);
    gDisplayListHead = pause_blend_draw_image(
        gDisplayListHead, (const void *) gPhysicalFramebuffers[sRenderingFramebuffer], SCREEN_WIDTH, 0,
        right, TRUE, 255);
    gDPSetColorImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WIDTH,
                     gPhysicalFramebuffers[sRenderingFramebuffer]);
    gSPDisplayList(gDisplayListHead++, init_rdp);
    sPauseBlend.latestInputBackground = target;
    sPauseBlend.inputBackgroundFrame = gGlobalTimer;
    sPauseBlend.latestInputBackgroundValid = TRUE;
}
#endif
/**
 * End the master display list and initialize the graphics task structure for the next frame to be rendered.
 */
void end_master_display_list(void) {
    draw_screen_borders();

#ifdef INPUT_DISPLAY
    capture_input_display_background();
    render_input_display();
    if (sDebugOverlayDisplayList != NULL) {
        gSPDisplayList(gDisplayListHead++, sDebugOverlayDisplayList);
        sDebugOverlayDisplayList = NULL;
    }
#endif

    gDPFullSync(gDisplayListHead++);
    gSPEndDisplayList(gDisplayListHead++);

    create_gfx_task_structure();
}

/**
 * Draw the bars that appear when the N64 is soft reset.
 */
void draw_reset_bars(void) {
    s32 width, height;
    s32 fbNum;
    u64 *fbPtr;

    if (gResetTimer != 0 && gNmiResetBarsTimer < 15) {
        if (video_low_latency_enabled()) {
            fbNum = sRenderedFramebuffer;
        } else if (sRenderedFramebuffer == 0) {
            fbNum = 2;
        } else {
            fbNum = sRenderedFramebuffer - 1;
        }

        fbPtr = (u64 *) PHYSICAL_TO_VIRTUAL(gPhysicalFramebuffers[fbNum]);
        fbPtr += gNmiResetBarsTimer++ * (SCREEN_WIDTH / 4);

        for (width = 0; width < ((SCREEN_HEIGHT / 16) + 1); width++) {
            for (height = 0; height < (SCREEN_WIDTH / 4); height++) {
                *fbPtr++ = 0;
            }
            fbPtr += ((SCREEN_WIDTH / 4) * 14);
        }
    }

    osWritebackDCacheAll();
    osRecvMesg(&gGameVblankQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
    osRecvMesg(&gGameVblankQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
}

/**
 * Check if we are emulating the framebuffer
 * 
 * s32 frameIndex:
 *  0: Write to the framebuffer, wait to process displaylist
 *  1: Check whether the framebuffer write persisted
 */
static void check_fbe(s32 frameIndex) {
    // NOTE: For whatever reason, checking against pixel index 12 fails on some versions of GlideN64 (pain).
    // So apparently, this value being set to 13 actually matters...???
    const s32 fbePixelOffset = 13;
    const u16 fbePixelVal = 0xFF01;

    if (frameIndex == 0) {
        // Write pixel to the framebuffer
        gFramebuffers[sRenderingFramebuffer][fbePixelOffset] = fbePixelVal;
    } else {
        // Check if pixel persisted in the framebuffer after executing the display list
        //  that clears it (but before updating sRenderingFramebuffer!)
        if (gFramebuffers[sRenderingFramebuffer][fbePixelOffset] != fbePixelVal) {
            gSystemCapabilities |= SUPPORTS_SOFTWARE_FRAMEBUFFER;
        }
    }
}

/**
 * Initial settings for the first rendered frame.
 */
void render_init(void) {
    gGfxPool = &gGfxPools[0];
    set_segment_base_addr(SEGMENT_RENDER, gGfxPool->buffer);
    gGfxSPTask = &gGfxPool->spTask;
    gDisplayListHead = gGfxPool->buffer;
    gGfxPoolEnd = (u8 *)(gGfxPool->buffer + GFX_POOL_SIZE);
    init_rcp(CLEAR_ZBUFFER);
    clear_framebuffer(0);
    end_master_display_list();

    // Skip the FBE check if system is console,
    //  or had already been determined to support framebuffer emulation.
    if (gSystemCapabilities & SUPPORTS_SOFTWARE_FRAMEBUFFER) {
        exec_display_list(&gGfxPool->spTask);
    } else {
        check_fbe(0);

        exec_display_list(&gGfxPool->spTask);

        // Wait for frame rendering to complete to prevent race condition with FBE check
        osRecvMesg(&gGfxVblankQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
        check_fbe(1);

        // Send message back to queue to prevent locking up
        osSendMesg(&gGfxVblankQueue, gMainReceivedMesg, OS_MESG_BLOCK);
    }

    // Skip incrementing the initial framebuffer index on certain emulators so that they display immediately as the Gfx task finishes
    // This will break accurate emulators, so only enable on Project64, Parallel Launcher and Mupen.
    if (!(gEmulator & INSTANT_INPUT_WHITELIST)) {
        sRenderingFramebuffer++;
    }
    gGlobalTimer++;
}

/**
 * Selects the location of the F3D output buffer (gDisplayListHead).
 */
void select_gfx_pool(void) {
    gGfxPool = &gGfxPools[gGlobalTimer % ARRAY_COUNT(gGfxPools)];
    set_segment_base_addr(SEGMENT_RENDER, gGfxPool->buffer);
    gGfxSPTask = &gGfxPool->spTask;
    gDisplayListHead = gGfxPool->buffer;
    gGfxPoolEnd = (u8 *) (gGfxPool->buffer + GFX_POOL_SIZE);
}

/**
 * This function:
 * - Sends the current master display list out to be rendered.
 * - Legacy paths select the old framebuffer here; low-latency frames publish on DP completion.
 * - Yields to the VI framerate twice, locking the game at 30 FPS.
 * - Selects which framebuffer will be rendered and displayed to next time.
 */
void display_and_vsync(void) {
    osRecvMesg(&gGfxVblankQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
    if (gGoddardVblankCallback != NULL) {
        gGoddardVblankCallback();
        gGoddardVblankCallback = NULL;
    }
    exec_display_list(&gGfxPool->spTask);
#ifndef UNLOCK_FPS
    osRecvMesg(&gGameVblankQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
#endif
    if (!video_low_latency_enabled()) {
        osViSwapBuffer((void *) PHYSICAL_TO_VIRTUAL(gPhysicalFramebuffers[sRenderedFramebuffer]));
    }
#ifndef UNLOCK_FPS
    osRecvMesg(&gGameVblankQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
#endif
    // Skip swapping buffers on some inaccurate emulators so that they display immediately as the Gfx task finishes
    if (!(gEmulator & INSTANT_INPUT_WHITELIST)) {
        if (!video_low_latency_enabled() && ++sRenderedFramebuffer == 3) {
            sRenderedFramebuffer = 0;
        }
        if (++sRenderingFramebuffer == 3) {
            sRenderingFramebuffer = 0;
        }
    }
    gGlobalTimer++;
}

#if !defined(DISABLE_DEMO) && defined(KEEP_MARIO_HEAD)
// this function records distinct inputs over a 255-frame interval to RAM locations and was likely
// used to record the demo sequences seen in the final game. This function is unused.
UNUSED static void record_demo(void) {
    // record the player's button mask and current rawStickX and rawStickY.
    u8 buttonMask =
        ((gPlayer1Controller->buttonDown & (A_BUTTON | B_BUTTON | Z_TRIG | START_BUTTON)) >> 8)
        | (gPlayer1Controller->buttonDown & (U_CBUTTONS | D_CBUTTONS | L_CBUTTONS | R_CBUTTONS));
    s8 rawStickX = gPlayer1Controller->rawStickX;
    s8 rawStickY = gPlayer1Controller->rawStickY;

    // If the stick is in deadzone, set its value to 0 to
    // nullify the effects. We do not record deadzone inputs.
    if (rawStickX > -8 && rawStickX < 8) {
        rawStickX = 0;
    }

    if (rawStickY > -8 && rawStickY < 8) {
        rawStickY = 0;
    }

    // Rrecord the distinct input and timer so long as they are unique.
    // If the timer hits 0xFF, reset the timer for the next demo input.
    if (gRecordedDemoInput.timer == 0xFF || buttonMask != gRecordedDemoInput.buttonMask
        || rawStickX != gRecordedDemoInput.rawStickX || rawStickY != gRecordedDemoInput.rawStickY) {
        gRecordedDemoInput.timer = 0;
        gRecordedDemoInput.buttonMask = buttonMask;
        gRecordedDemoInput.rawStickX = rawStickX;
        gRecordedDemoInput.rawStickY = rawStickY;
    }
    gRecordedDemoInput.timer++;
}

/**
 * If a demo sequence exists, this will run the demo input list until it is complete.
 */
void run_demo_inputs(void) {
    // Eliminate the unused bits.
    gPlayer1Controller->controllerData->button &= VALID_BUTTONS;

    // Check if a demo inputs list exists and if so,
    // run the active demo input list.
    if (gCurrDemoInput != NULL) {
        // The timer variable being 0 at the current input means the demo is over.
        // Set the button to the END_DEMO mask to end the demo.
        if (gCurrDemoInput->timer == 0) {
            gPlayer1Controller->controllerData->stick_x = 0;
            gPlayer1Controller->controllerData->stick_y = 0;
            gPlayer1Controller->controllerData->button = END_DEMO;
        } else {
            // Backup the start button if it is pressed, since we don't want the
            // demo input to override the mask where start may have been pressed.
            u16 startPushed = (gPlayer1Controller->controllerData->button & START_BUTTON);

            // Perform the demo inputs by assigning the current button mask and the stick inputs.
            gPlayer1Controller->controllerData->stick_x = gCurrDemoInput->rawStickX;
            gPlayer1Controller->controllerData->stick_y = gCurrDemoInput->rawStickY;

            // To assign the demo input, the button information is stored in
            // an 8-bit mask rather than a 16-bit mask. this is because only
            // A, B, Z, Start, and the C-Buttons are used in a demo, as bits
            // in that order. In order to assign the mask, we need to take the
            // upper 4 bits (A, B, Z, and Start) and shift then left by 8 to
            // match the correct input mask. We then add this to the masked
            // lower 4 bits to get the correct button mask.
            gPlayer1Controller->controllerData->button =
                ((gCurrDemoInput->buttonMask & 0xF0) << 8) + ((gCurrDemoInput->buttonMask & 0xF));

            // If start was pushed, put it into the demo sequence being input to end the demo.
            gPlayer1Controller->controllerData->button |= startPushed;

            // Run the current demo input's timer down. if it hits 0, advance the demo input list.
            if (--gCurrDemoInput->timer == 0) {
                gCurrDemoInput++;
            }
        }
    }
}

#endif

/**
 * Take the updated controller struct and calculate the new x, y, and distance floats.
 */
void adjust_analog_stick(struct Controller *controller) {
    s32 deadzone = game_options_get(GO_DEADZONE);
    // Reset the controller's x and y floats.
    controller->stickX = 0;
    controller->stickY = 0;

    // Preserve the first active raw unit as +/-1, at every chosen deadzone.
    if (controller->rawStickX < -deadzone) {
        controller->stickX = controller->rawStickX + deadzone;
    }

    if (controller->rawStickX > deadzone) {
        controller->stickX = controller->rawStickX - deadzone;
    }

    if (controller->rawStickY < -deadzone) {
        controller->stickY = controller->rawStickY + deadzone;
    }

    if (controller->rawStickY > deadzone) {
        controller->stickY = controller->rawStickY - deadzone;
    }

    // Calculate f32 magnitude from the center by vector length.
    controller->stickMag =
        sqrtf(controller->stickX * controller->stickX + controller->stickY * controller->stickY);

    // Magnitude cannot exceed 64.0f: if it does, modify the values
    // appropriately to flatten the values down to the allowed maximum value.
    if (controller->stickMag > 64) {
        controller->stickX *= 64 / controller->stickMag;
        controller->stickY *= 64 / controller->stickMag;
        controller->stickMag = 64;
    }
}

/**
 * Update the controller struct with available inputs if present.
 */
static s32 refresh_controller_types(void) {
    // Missing ports may reconnect using a different protocol. Query infrequently,
    // after the previous read completed and while rumble ownership is held.
    if (gControllerBits == (1 << MAXCONTROLLERS) - 1 || gGlobalTimer % 30 != 0) {
        return TRUE;
    }
    if (osContStartQueryEx(&gSIEventMesgQueue) != 0) {
        return FALSE;
    }
    osRecvMesg(&gSIEventMesgQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
    osContGetQuery(gControllerStatuses);
    return TRUE;
}

void read_controller_inputs(s32 threadID) {
    if (threadID == THREAD_5_GAME_LOOP) {
        osRecvMesg(&gSIEventMesgQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
    }
    osContGetReadDataEx(gControllerPads);
    release_rumble_pak_control();
    s32 menuInput = threadID != THREAD_5_GAME_LOOP || sCurrPlayMode != PLAY_MODE_NORMAL
                    || gCurrLevelNum == LEVEL_UNKNOWN_1 || game_options_active_file() < 0
                    || get_dialog_id() != DIALOG_NONE || gMarioObject == NULL;
    // Preserve held A through the star exit, not just after its save prompt opens.
    s32 starSaveInput = sDelayedWarpOp == WARP_OP_STAR_EXIT
                        || gMenuMode == MENU_MODE_RENDER_COURSE_COMPLETE_SCREEN
                        || (gMarioState != NULL
                            && (gMarioState->action == ACT_EXIT_AIRBORNE
                                || gMarioState->action == ACT_FALLING_EXIT_AIRBORNE
                                || gMarioState->action == ACT_SPECIAL_EXIT_AIRBORNE
                                || gMarioState->action == ACT_EXIT_LAND_SAVE_DIALOG));
    if (get_dialog_id() != DIALOG_NONE || starSaveInput) {
        menuInput |= GAME_OPTIONS_INPUT_DIALOGUE;
    }
    game_options_input_select(menuInput, gCurrDemoInput != NULL);
#ifdef INPUT_DISPLAY
    if (threadID == THREAD_5_GAME_LOOP) {
        struct Controller displayedController = { 0 };
        const OSContPadEx *pad = gPlayer1Controller->controllerData;

        // Capture physical input before attract demos or gameplay remaps can replace it.
        if (pad != NULL && pad->errno == 0) {
            displayedController.rawStickX = pad->stick_x;
            displayedController.rawStickY = pad->stick_y;
            displayedController.buttonDown = pad->button;
            adjust_analog_stick(&displayedController);
        }
        input_display_update(&displayedController);
    }
#endif
#if !defined(DISABLE_DEMO) && defined(KEEP_MARIO_HEAD)
    run_demo_inputs();
#endif

    game_options_input_read(menuInput, gCurrDemoInput != NULL);
}

/**
 * @brief Links a controller struct to the appropriate status and pad.
 *
 * @param[out] controller The controller to link.
 * @param[in ] port The port to get the data from.
 */
static void assign_controller_data_to_port(struct Controller* controller, int port) {
    controller->statusData = &gControllerStatuses[port];
    controller->controllerData = &gControllerPads[port];
    controller->port = port;
}

/**
 * Initialize the controller structs to point at the OSCont information.
 */
void init_controllers(void) {
    int port, cont = 0;

    // Set controller 1 to point to the set of status/pads for input 1 and
    // init the controllers.
    assign_controller_data_to_port(&gControllers[0], 0);
    osContInit(&gSIEventMesgQueue, &gControllerBits, gControllerStatuses);

#ifdef SRAM
    gSramProbe = nuPiInitSram();
#endif

    // Loop over the 4 ports and link the controller structs to the appropriate status and pad.
    for (port = 0; port < MAXCONTROLLERS; port++) {
        if (cont >= MAX_NUM_PLAYERS) {
            break;
        }

        // Is controller plugged in?
        if (gControllerBits & (1 << port)) {
            // The game allows you to have just 1 controller plugged
            // into any port in order to play the game. this was probably
            // so if any of the ports didn't work, you can have controllers
            // plugged into any of them and it will work.
            assign_controller_data_to_port(&gControllers[cont], port);

            cont++;
        }
    }

#if (MAX_NUM_PLAYERS >= 2)
    //! Some flashcarts (eg. ED64p) don't let you start a ROM with a GameCube controller in port 1,
    //   so if port 1 is an N64 controller and port 2 is a GC controller, swap them.
    if (
        (gEmulator & EMU_CONSOLE) &&
        ((gControllerBits & 0b11) == 0b11) && // Only swap if the first two ports both have controllers plugged in.
        ((gControllerStatuses[0].type & CONT_CONSOLE_MASK) == CONT_CONSOLE_N64) && // If the 1st port's controller is N64.
        ((gControllerStatuses[1].type & CONT_CONSOLE_MASK) == CONT_CONSOLE_GCN)    // If the 2nd port's controller is GCN.
    ) {
        struct Controller temp = gControllers[0];
        gControllers[0] = gControllers[1];
        gControllers[1] = temp;
    }
#endif

    // Read all ports so unplug/replug and explicit port selection can recover.
    osContSetCh(MAXCONTROLLERS);
    game_options_input_init();
}

// Game thread core
// ----------------------------------------------------------------------------------------------------

/**
 * Setup main segments and framebuffers.
 */
void setup_game_memory(void) {
    mario_character_init();
    // Setup general Segment 0
    set_segment_base_addr(SEGMENT_MAIN, (void *)RAM_START);
    // Create Mesg Queues
    osCreateMesgQueue(&gGfxVblankQueue, gGfxMesgBuf, ARRAY_COUNT(gGfxMesgBuf));
    osCreateMesgQueue(&gGameVblankQueue, gGameMesgBuf, ARRAY_COUNT(gGameMesgBuf));
    // Setup z buffer and framebuffer
    gPhysicalZBuffer = VIRTUAL_TO_PHYSICAL(gZBuffer);
    gPhysicalFramebuffers[0] = VIRTUAL_TO_PHYSICAL(gFramebuffer0);
    gPhysicalFramebuffers[1] = VIRTUAL_TO_PHYSICAL(gFramebuffer1);
    gPhysicalFramebuffers[2] = VIRTUAL_TO_PHYSICAL(gFramebuffer2);
    // Setup Mario Animations
    gMarioAnimsMemAlloc = main_pool_alloc(MARIO_ANIMS_POOL_SIZE, MEMORY_POOL_LEFT);
    set_segment_base_addr(SEGMENT_MARIO_ANIMS, (void *) gMarioAnimsMemAlloc);
    setup_dma_table_list(&gMarioAnimsBuf, gMarioAnims, gMarioAnimsMemAlloc);
#ifdef PUPPYPRINT_DEBUG
    set_segment_memory_printout(SEGMENT_MARIO_ANIMS, MARIO_ANIMS_POOL_SIZE);
    set_segment_memory_printout(SEGMENT_DEMO_INPUTS, DEMO_INPUTS_POOL_SIZE);
#endif
    // Setup Demo Inputs List
    gDemoInputsMemAlloc = main_pool_alloc(DEMO_INPUTS_POOL_SIZE, MEMORY_POOL_LEFT);
    set_segment_base_addr(SEGMENT_DEMO_INPUTS, (void *) gDemoInputsMemAlloc);
    setup_dma_table_list(&gDemoInputsBuf, gDemoInputs, gDemoInputsMemAlloc);
    // Setup Level Script Entry
    load_segment(SEGMENT_LEVEL_ENTRY, _entrySegmentRomStart, _entrySegmentRomEnd, MEMORY_POOL_LEFT, NULL, NULL);
    // Setup Segment 2 (Fonts, Text, etc)
    load_segment_decompress(SEGMENT_SEGMENT2, _segment2_mio0SegmentRomStart, _segment2_mio0SegmentRomEnd);
}

/**
 * Main game loop thread. Runs forever as long as the game continues.
 */
void thread5_game_loop(UNUSED void *arg) {
    setup_game_memory();
    init_rumble_pak_scheduler_queue();
    init_controllers();
    create_thread_6();
    save_file_load_all();

    set_vblank_handler(2, &gGameVblankHandler, &gGameVblankQueue, (OSMesg) 1);

    // Point address to the entry point into the level script data.
    struct LevelCommand *addr = segmented_to_virtual(level_script_entry);

    play_music(SEQ_PLAYER_SFX, SEQUENCE_ARGS(0, SEQ_SOUND_PLAYER), 0);
    set_sound_mode(save_file_get_sound_mode());
#ifdef REONUCAM
    gReonucamState.speed = REONUCAM_DEFAULT_SPEED;
#endif
#ifdef WIDE
    gConfig.widescreen = save_file_get_widescreen_mode();
#endif
    render_init();

    while (TRUE) {
        profiler_frame_setup();
        // If the reset timer is active, run the process to reset the game.
        if (gResetTimer != 0) {
            draw_reset_bars();
            continue;
        }
#ifdef PUPPYPRINT_DEBUG
        bzero(&gPuppyCallCounter, sizeof(gPuppyCallCounter));
#endif
        audio_game_loop_tick();
        select_gfx_pool();
        // Sample after frame setup so gameplay uses the freshest available input.
        block_until_rumble_pak_free();
        if (reset_pending() || !refresh_controller_types()
            || osContStartReadDataEx(&gSIEventMesgQueue) != 0) {
            release_rumble_pak_control();
            continue;
        }

        read_controller_inputs(THREAD_5_GAME_LOOP);
#if GCC_PROFILE
        if (gGccProfileDumpState == 1
            || ((gPlayer1Controller->buttonDown & (L_TRIG | R_TRIG)) == (L_TRIG | R_TRIG)
                && (gPlayer1Controller->buttonPressed & START_BUTTON))) {
            gcc_profile_dump();
        }
#endif
        profiler_update(PROFILER_TIME_CONTROLLERS, 0);
        profiler_collision_reset();
        game_options_runtime_update();
#ifdef PUPPYPRINT_DEBUG
        puppyprint_debug_update();
#endif
        addr = level_script_execute(addr);
        profiler_collision_completed();
#if !defined(PUPPYPRINT_DEBUG) && defined(VISUAL_DEBUG)
        debug_box_input();
#endif
#ifdef PUPPYPRINT_DEBUG
        puppyprint_profiler_process();
#endif

        display_and_vsync();
#ifdef VANILLA_DEBUG
        // when debug info is enabled, print the "BUF %d" information.
        if (gShowDebugText) {
            // subtract the end of the gfx pool with the display list to obtain the
            // amount of free space remaining.
            print_text_fmt_int(180, 20, "BUF %d", gGfxPoolEnd - (u8 *) gDisplayListHead);
        }
#endif
    }
}
