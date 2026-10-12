#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <PR/mbi.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#include "gfx_dimensions.h"
#include "../src/game/input_display.h"
#define TRUE 1
#define FALSE 0
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define LEVEL_MAX 40
#define CAM_MOVE_PAUSE_SCREEN 0x8000

typedef f32 Vec3f[3];
struct GraphNodeCamera {
    Vec3f pos, focus;
    s16 rollScreen;
};
static struct {
    f32 moveSpeed;
} mario, *gMarioState = &mario;
static struct {
    f32 areaCenX, areaCenY, areaCenZ;
} cameraCenter = { 100, 200, 300 }, *gCamera = &cameraCenter;
static struct {
    Vec3f pos;
} marioCamera = { { 30, 400, 50 } }, *sMarioCamState = &marioCamera;
static struct {
    Vec3f pos, focus;
    s16 roll;
} gLakituState = { { 10, 20, 30 }, { 40, 50, 60 }, 100 };
static s32 gCurrLevelArea, gCameraMovementFlags, overviewCalls;
static void vec3f_copy(Vec3f dest, Vec3f source) {
    memcpy(dest, source, sizeof(Vec3f));
}
static void vec3f_get_yaw(Vec3f from, Vec3f to, s16 *yaw) {
    assert(from[0] == 100 && from[1] == 300 && from[2] == 300);
    assert(to == sMarioCamState->pos);
    *yaw = 0;
}
static void vec3f_set_dist_and_angle(Vec3f from, Vec3f to, f32 distance, s16 pitch, s16 yaw) {
    assert(distance == 6000 && pitch == 0x1000 && yaw == 0);
    assert(from == sMarioCamState->pos);
    overviewCalls++;
    to[0] = from[0];
    to[1] = from[1];
    to[2] = from[2] + distance;
}

struct Area {
    void *graphNode;
} area = { &area }, otherArea = { &otherArea };
struct Area *gCurrentArea = &area;
struct {
    int isActive, pauseRendering;
} gWarpTransition;
void *gViewportOverride, *gViewportClip;
s16 gCurrLevelNum = 1;
u32 gGlobalTimer;
u32 gEmulator;
u16 sRenderingFramebuffer;
u8 gBorderHeight = 8;
s16 sCurrPlayMode;
uintptr_t gPhysicalFramebuffers[] = { 0x100000, 0x140000, 0x180000 };
const Gfx init_rdp[] = { gsSPEndDisplayList() };
static Gfx root[4096], storage[2048];
static s32 inputEnabled = TRUE;
s32 input_display_enabled(void) {
    return inputEnabled;
}
Gfx *gDisplayListHead = root;
u8 *gGfxPoolEnd = (u8 *) (root + 4096);
static u32 allocationSize;
static s32 failAllocation;
static void *alloc_display_list(u32 size) {
    assert(size <= sizeof(storage));
    allocationSize = size;
    return failAllocation ? NULL : storage;
}

#include "pause_blend_actual.inc.c"

static void prepare_frame(void) {
    prepare_pause_screen_blend(gViewportOverride == NULL && gViewportClip == NULL);
}

static int opcode(Gfx command) {
    return command.words.w0 >> 24;
}

static int inspect_image_commands(Gfx *dl, u16 source, int copy, u8 alpha, int setup) {
    int rectangles = 0, loads = 0, images = 0, prim = 0, combine = 0;
    int count = 0;
    while (opcode(dl[count]) != (G_ENDDL & 0xff)) {
        Gfx command = dl[count];
        assert(count < PAUSE_BLEND_DRAW_COMMANDS);
        if (opcode(command) == (G_SETTIMG & 0xff)) {
            assert(command.words.w1 == gPhysicalFramebuffers[source]);
            assert((command.words.w0 & 0xfff) + 1 == SCREEN_WIDTH);
            images++;
        } else if (opcode(command) == (G_LOADTILE & 0xff)) {
            int y = loads * 6;
            assert(((command.words.w0 >> 12) & 0xfff) == 0);
            assert((command.words.w0 & 0xfff) == (u32) (y << 2));
            assert(((command.words.w1 >> 12) & 0xfff) == (SCREEN_WIDTH - 1) << 2);
            assert((command.words.w1 & 0xfff) == (u32) ((y + 5) << 2));
            assert(SCREEN_WIDTH * 6 * 2 <= 4096);
            loads++;
        } else if (opcode(command) == (G_TEXRECT & 0xff)) {
            int y = rectangles * 6;
            int left = 0;
#ifdef INPUT_DISPLAY
            left = copy || !sPauseBlend.inputBlendSourceValid ? 0 : 112;
#endif
            assert(((command.words.w0 >> 12) & 0xfff) == (u32) ((SCREEN_WIDTH - copy) << 2));
            assert((command.words.w0 & 0xfff) == (u32) ((y + 6 - copy) << 2));
            assert(((command.words.w1 >> 12) & 0xfff) == (u32) (left << 2));
            assert((command.words.w1 & 0xfff) == (u32) (y << 2));
            assert(dl[count + 1].words.w1 == (u32) ((left << 21) | (y << 5)));
            assert(dl[count + 2].words.w1 == (u32) (((copy ? 4 : 1) << 26) | (1 << 10)));
            rectangles++;
        } else if (opcode(command) == (G_SETPRIMCOLOR & 0xff) && !prim) {
            assert(command.words.w1 == (0xffffff00 | alpha));
            prim++;
        } else if (opcode(command) == (G_SETCOMBINE & 0xff)) {
            // RGB comes directly from the old image. Alpha comes directly from
            // the constant primitive weight, independently of texture coverage.
            assert(((command.words.w1 >> 15) & 7) == G_CCMUX_TEXEL0);
            assert(((command.words.w0 >> 15) & 31) == G_CCMUX_0);
            assert(((command.words.w1 >> 9) & 7) == G_ACMUX_PRIMITIVE);
            assert(((command.words.w0 >> 9) & 7) == G_ACMUX_0);
            combine++;
        }
        count++;
    }
    assert(rectangles == 40 && loads == 40 && images == 40 && prim == 1 && combine == 1);
    assert(dl[setup + 2].words.w1 == (copy ? G_CYC_COPY : G_CYC_1CYCLE));
    assert((dl[setup + 3].words.w1 & (IM_RD | FORCE_BL)) == (IM_RD | FORCE_BL));
    assert(!(dl[setup + 3].words.w1 & (Z_CMP | Z_UPD | ALPHA_CVG_SEL)));
    assert(dl[setup + 5].words.w1 == G_TF_POINT);
    // The final commands restore the default RDP state, color, cycle and scissor.
    assert(opcode(dl[count - 4]) == (G_DL & 0xff));
    assert(opcode(dl[count - 3]) == (G_SETPRIMCOLOR & 0xff));
    assert(dl[count - 3].words.w1 == 0xffffffff);
    assert(opcode(dl[count - 1]) == (G_SETSCISSOR & 0xff));
    return count + 1;
}

static int inspect_image(Gfx *dl, u16 source, int copy, u8 alpha) {
    return inspect_image_commands(dl, source, copy, alpha, copy ? 2 : 0);
}

static void reset_history(void) {
    memset(&sPauseBlend, 0, sizeof(sPauseBlend));
    inputEnabled = TRUE;
    sPauseBlend.historyValid = FALSE;
    sPauseBlend.displayList = NULL;
    sPauseBlend.pending = FALSE;
    sPauseBlend.active = FALSE;
    sPauseBlend.copyCall = NULL;
    gGlobalTimer = UINT32_MAX - 2;
    sRenderingFramebuffer = 0;
    sCurrPlayMode = PLAY_MODE_NORMAL;
    gCurrentArea = &area;
    gCurrLevelNum = 1;
    gWarpTransition.isActive = FALSE;
    gWarpTransition.pauseRendering = FALSE;
    gViewportClip = gViewportOverride = NULL;
    failAllocation = FALSE;
    allocationSize = 0;
    gDisplayListHead = root;
    gGfxPoolEnd = (u8 *) (root + 4096);
}

static void next_frame(int fixedBuffer, int paused) {
    gGlobalTimer++;
    if (!fixedBuffer) {
        sRenderingFramebuffer = (sRenderingFramebuffer + 1) % 3;
    }
    sCurrPlayMode = paused ? PLAY_MODE_PAUSED : PLAY_MODE_NORMAL;
    gDisplayListHead = root;
    gGfxPoolEnd = (u8 *) (root + 4096);
    allocationSize = 0;
}

static void verify_transition(int fixedBuffer, u16 oldBuffer, int step) {
    const u8 weights[] = { 191, 127, 63 };
    u16 source = fixedBuffer && step > 0 ? 1 : oldBuffer;
    int copy = source == sRenderingFramebuffer;
    u16 savedBuffer = (sRenderingFramebuffer + 1) % 3;
    prepare_frame();
    assert(sPauseBlend.pending && allocationSize == 0);
    assert(gDisplayListHead == root + copy);
    render_pause_screen_blend();
    assert(sPauseBlend.displayList != NULL);
    assert(allocationSize == (copy ? 2 : 1) * PAUSE_BLEND_COPY_COMMANDS * sizeof(Gfx));
    assert(gDisplayListHead == root + copy + 1);
    if (copy) {
        assert(opcode(storage[1]) == (G_SETCIMG & 0xff));
        assert(storage[1].words.w1 == gPhysicalFramebuffers[savedBuffer]);
        int count = inspect_image(storage, sRenderingFramebuffer, TRUE, 255);
        assert(opcode(storage[count - 6]) == (G_SETCIMG & 0xff));
        assert(storage[count - 6].words.w1 == gPhysicalFramebuffers[sRenderingFramebuffer]);
    }
    inspect_image(sPauseBlend.displayList, copy ? savedBuffer : source, FALSE, weights[step]);
    Gfx *saved = sPauseBlend.displayList;
    u32 savedSize = allocationSize;
    for (int draw = 0; draw < 3; draw++) {
        prepare_frame();
        assert(sPauseBlend.displayList == saved && allocationSize == savedSize);
        assert(gDisplayListHead == root + copy + 1 + draw);
        render_pause_screen_blend();
    }
}

static void test_cycles(void) {
    for (int fixedBuffer = 0; fixedBuffer < 2; fixedBuffer++) {
        reset_history();
        prepare_frame();
        assert(sPauseBlend.displayList == NULL);
        for (int cycle = 0; cycle < 3; cycle++) {
            u16 oldBuffer = sRenderingFramebuffer;
            for (int step = 0; step < 3; step++) {
                next_frame(fixedBuffer, TRUE);
                verify_transition(fixedBuffer, oldBuffer, step);
            }
            for (int frame = 3; frame <= 8; frame++) {
                next_frame(fixedBuffer, TRUE);
                prepare_frame();
                render_pause_screen_blend();
                assert(sPauseBlend.displayList == NULL && allocationSize == 0);
                assert(gDisplayListHead == root);
            }
            oldBuffer = sRenderingFramebuffer;
            for (int step = 0; step < 3; step++) {
                next_frame(fixedBuffer, FALSE);
                verify_transition(fixedBuffer, oldBuffer, step);
            }
            next_frame(fixedBuffer, FALSE);
            prepare_frame();
            render_pause_screen_blend();
            assert(sPauseBlend.displayList == NULL && allocationSize == 0);
        }
    }
}

static void test_interrupted_transition(void) {
    for (int fixedBuffer = 0; fixedBuffer < 2; fixedBuffer++) {
        reset_history();
        prepare_frame();
        next_frame(fixedBuffer, TRUE);
        verify_transition(fixedBuffer, 0, 0);
        // Reversing restarts from the latest complete image, with fresh weights.
        u16 oldBuffer = sRenderingFramebuffer;
        next_frame(fixedBuffer, FALSE);
        verify_transition(fixedBuffer, oldBuffer, 0);
        next_frame(fixedBuffer, FALSE);
        verify_transition(fixedBuffer, oldBuffer, 1);
        next_frame(fixedBuffer, FALSE);
        verify_transition(fixedBuffer, oldBuffer, 2);

        for (int scenario = 0; scenario < 4; scenario++) {
            reset_history();
            prepare_frame();
            next_frame(fixedBuffer, TRUE);
            verify_transition(fixedBuffer, 0, 0);
            if (scenario >= 2) {
                next_frame(fixedBuffer, TRUE);
                verify_transition(fixedBuffer, 0, 1);
            }
            next_frame(fixedBuffer, TRUE);
            if (scenario % 2 == 0) {
                failAllocation = TRUE;
            } else {
                gCurrentArea = &otherArea;
            }
            prepare_frame();
            render_pause_screen_blend();
            assert(sPauseBlend.displayList == NULL && !sPauseBlend.active);
            int copyPlaceholder = !fixedBuffer && scenario == 2;
            assert(gDisplayListHead == root + copyPlaceholder);
            if (copyPlaceholder) {
                assert(opcode(root[0]) == (G_NOOP & 0xff));
            }
            next_frame(fixedBuffer, TRUE);
            failAllocation = FALSE;
            prepare_frame();
            render_pause_screen_blend();
            assert(sPauseBlend.displayList == NULL && allocationSize == 0);
        }
    }
}

static void test_invalid_history_and_allocation(void) {
    for (int fixedBuffer = 0; fixedBuffer < 2; fixedBuffer++) {
        for (int scenario = 0; scenario < 10; scenario++) {
            reset_history();
            prepare_frame();
            next_frame(fixedBuffer, TRUE);
            if (scenario == 0) {
                gCurrentArea = &otherArea;
            }
            if (scenario == 1) {
                gCurrLevelNum++;
            }
            if (scenario == 2) {
                gGlobalTimer++;
            }
            if (scenario == 3) {
                gWarpTransition.isActive = TRUE;
            }
            if (scenario == 4) {
                gWarpTransition.pauseRendering = TRUE;
            }
            if (scenario == 5) {
                gViewportClip = &area;
            }
            if (scenario == 6) {
                gCurrentArea = NULL;
            }
            if (scenario == 7) {
                failAllocation = TRUE;
            }
            if (scenario == 8) {
                gViewportOverride = &area;
            }
            prepare_frame();
            if (scenario == 9) {
                // Enough space for the effect, but not for finishing the frame.
                int commands = (fixedBuffer ? 2 : 1) * PAUSE_BLEND_DRAW_COMMANDS;
                gGfxPoolEnd = (u8 *) (gDisplayListHead + commands + PAUSE_BLEND_TAIL_COMMANDS - 1);
            }
            render_pause_screen_blend();
            int copyPlaceholder = fixedBuffer && (scenario == 7 || scenario == 9);
            assert(sPauseBlend.displayList == NULL && gDisplayListHead == root + copyPlaceholder);
            if (copyPlaceholder) {
                assert(opcode(root[0]) == (G_NOOP & 0xff));
            }
            if (scenario == 9) {
                assert(allocationSize == 0);
            }
            next_frame(fixedBuffer, TRUE);
            failAllocation = FALSE;
            gWarpTransition.isActive = gWarpTransition.pauseRendering = FALSE;
            gViewportClip = gViewportOverride = NULL;
            gCurrentArea = &area;
            prepare_frame();
            assert(sPauseBlend.displayList == NULL);
        }
    }
    reset_history();
    prepare_frame();
    next_frame(FALSE, FALSE);
    sCurrPlayMode = PLAY_MODE_FRAME_ADVANCE;
    prepare_frame();
    render_pause_screen_blend();
    assert(sPauseBlend.displayList == NULL && gDisplayListHead == root);
}

static void test_pause_camera_movement(void) {
    const f32 speeds[] = { 0, 0.1f, 0.5f, 0.51f, 10, 40 };
    // BOB outdoors/second area, castle indoors, and Lab first/unused second area.
    const s32 areas[] = { 9 * 16 + 1, 9 * 16 + 2, 6 * 16 + 1, 32 * 16 + 1, 32 * 16 + 2 };
    struct GraphNodeCamera camera;
    for (int areaIndex = 0; areaIndex < 5; areaIndex++) {
        gCurrLevelArea = areas[areaIndex];
        for (int cycle = 0; cycle < 3; cycle++) {
            for (int speed = 0; speed < 6; speed++) {
                mario.moveSpeed = speeds[speed];
                gCameraMovementFlags = CAM_MOVE_PAUSE_SCREEN;
                for (int draw = 0; draw < 12; draw++) {
                    int previousCalls = overviewCalls;
                    update_graph_node_camera(&camera);
                    int overview = (areaIndex == 0 || areaIndex == 3) && speed < 3;
                    assert(overviewCalls == previousCalls + overview);
                    assert(camera.rollScreen == gLakituState.roll);
                    if (overview) {
                        assert(camera.pos[2] == 6050 && camera.focus[1] == 300);
                    } else {
                        assert(memcmp(camera.pos, gLakituState.pos, sizeof(Vec3f)) == 0);
                        assert(memcmp(camera.focus, gLakituState.focus, sizeof(Vec3f)) == 0);
                    }
                }
                // Resume restores the gameplay view regardless of prior movement.
                gCameraMovementFlags = 0;
                int previousCalls = overviewCalls;
                update_graph_node_camera(&camera);
                assert(overviewCalls == previousCalls);
                assert(memcmp(camera.pos, gLakituState.pos, sizeof(Vec3f)) == 0);
                assert(memcmp(camera.focus, gLakituState.focus, sizeof(Vec3f)) == 0);
            }
        }
    }
}

static void test_cake_pause_without_gameplay_camera(void) {
    struct GraphNodeCamera camera;
    // Cake clears gCamera without refreshing the previous outdoor area's mask.
    gCamera = NULL;
    mario.moveSpeed = 0;
    gCurrLevelArea = 32 * 16 + 1;
    int previousCalls = overviewCalls;
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int draw = 0; draw < 12; draw++) {
            gCameraMovementFlags = CAM_MOVE_PAUSE_SCREEN;
            update_graph_node_camera(&camera);
            assert(overviewCalls == previousCalls);
            assert(camera.rollScreen == gLakituState.roll);
            assert(memcmp(camera.pos, gLakituState.pos, sizeof(Vec3f)) == 0);
            assert(memcmp(camera.focus, gLakituState.focus, sizeof(Vec3f)) == 0);
        }
        gCameraMovementFlags = 0;
        update_graph_node_camera(&camera);
        assert(overviewCalls == previousCalls);
    }
    gCamera = &cameraCenter;
    gCameraMovementFlags = CAM_MOVE_PAUSE_SCREEN;
    update_graph_node_camera(&camera);
    assert(overviewCalls == previousCalls + 1);
}

static void test_live_input_background(void) {
#ifdef INPUT_DISPLAY
    reset_history();
    prepare_frame();
    capture_input_display_background();
    assert(sPauseBlend.latestInputBackgroundValid);
    u8 saved = sPauseBlend.latestInputBackground;
    assert(((uintptr_t) sPauseBlend.inputBackgrounds[saved].pixels & 15) == 0);
    next_frame(FALSE, TRUE);
    prepare_frame();
    assert(sPauseBlend.inputBlendSourceValid && sPauseBlend.inputBlendSource == saved);
    render_pause_screen_blend();
    int image = 0, rectangles = 0, weights = 0;
    for (Gfx *dl = sPauseBlend.displayList; opcode(*dl) != (G_ENDDL & 0xff); dl++) {
        if (opcode(*dl) == (G_SETTIMG & 0xff)) {
            s32 width = (dl->words.w0 & 0xfff) + 1;
            if (width == INPUT_DISPLAY_BACKGROUND_WIDTH) {
                assert(dl->words.w1 == (u32) (uintptr_t) sPauseBlend.inputBackgrounds[saved].pixels);
                image++;
            } else {
                assert(width == SCREEN_WIDTH);
            }
        } else if (opcode(*dl) == (G_TEXRECT & 0xff)) {
            s32 left = (dl->words.w1 >> 12) & 0xfff;
            s32 right = (dl->words.w0 >> 12) & 0xfff;
            assert((left == 112 * 4 && right == SCREEN_WIDTH * 4) || (left == 0 && right == 112 * 4));
            rectangles++;
        } else if (opcode(*dl) == (G_SETPRIMCOLOR & 0xff) && dl->words.w1 == 0xffffffbf) {
            weights++;
        }
    }
    assert(image == 40 && rectangles == 80 && weights == 2);
    capture_input_display_background();
    assert(sPauseBlend.latestInputBackground != saved && sPauseBlend.inputBlendSource == saved);
    u8 latest = sPauseBlend.latestInputBackground;
    Gfx *head = gDisplayListHead;
    capture_input_display_background();
    assert(gDisplayListHead == head);
    next_frame(FALSE, FALSE);
    prepare_frame();
    assert(sPauseBlend.inputBlendSource == latest); // reversing a fade preserves the latest clean strip
    inputEnabled = FALSE;
    capture_input_display_background();
    assert(!sPauseBlend.latestInputBackgroundValid);
    next_frame(FALSE, TRUE);
    prepare_frame();
    assert(!sPauseBlend.inputBlendSourceValid);
    u32 commands = (sPauseBlend.copyCall != NULL ? 2 : 1) * PAUSE_BLEND_COPY_COMMANDS;
    gGfxPoolEnd = (u8 *) (gDisplayListHead + commands + PAUSE_BLEND_TAIL_COMMANDS);
    render_pause_screen_blend();
    assert(sPauseBlend.displayList != NULL); // hidden inputs retain the original graphics budget
#endif
}

static void test_native_pause_world(void) {
    const u32 emulators[] = { EMU_CONSOLE, EMU_MUPEN, EMU_OTHER, EMU_PARALLEL_LAUNCHER };
    for (unsigned i = 0; i < sizeof(emulators) / sizeof(emulators[0]); i++) {
        for (int paused = 0; paused < 2; paused++) {
            for (int blocked = 0; blocked < 5; blocked++) {
                gEmulator = emulators[i];
                sCurrPlayMode = paused ? PLAY_MODE_PAUSED : PLAY_MODE_NORMAL;
                gWarpTransition.isActive = blocked == 1;
                gViewportOverride = blocked == 2 ? &area : NULL;
                gViewportClip = blocked == 3 ? &area : NULL;
                failAllocation = blocked == 4;
                gDisplayListHead = root;
                gGfxPoolEnd = (u8 *) (root + 4096);
                render_pause_world_native(gViewportOverride == NULL && gViewportClip == NULL);
                int draw = emulators[i] == EMU_PARALLEL_LAUNCHER && paused && blocked == 0;
                assert(gDisplayListHead == root + draw);
                if (draw) {
                    assert(allocationSize == PAUSE_BLEND_COPY_COMMANDS * sizeof(Gfx));
                    // A self-copy must cover the world exactly, without alpha/coverage loss.
                    inspect_image_commands(storage, sRenderingFramebuffer, TRUE, 255, 0);
                }
            }
        }
    }
    gEmulator = EMU_PARALLEL_LAUNCHER;
    sCurrPlayMode = PLAY_MODE_PAUSED;
    gWarpTransition.isActive = FALSE;
    gViewportOverride = gViewportClip = NULL;
    failAllocation = FALSE;
    gDisplayListHead = root;
    gGfxPoolEnd = (u8 *) (root + PAUSE_BLEND_COPY_COMMANDS + PAUSE_BLEND_TAIL_COMMANDS - 1);
    render_pause_world_native(gViewportOverride == NULL && gViewportClip == NULL);
    assert(gDisplayListHead == root);
}

int main(void) {
    test_cycles();
    test_live_input_background();
    test_interrupted_transition();
    test_invalid_history_and_allocation();
    test_pause_camera_movement();
    test_cake_pause_without_gameplay_camera();
    test_native_pause_world();
    puts("Pause blend framebuffer commands, repeated transitions and fallbacks passed.");
    return 0;
}
