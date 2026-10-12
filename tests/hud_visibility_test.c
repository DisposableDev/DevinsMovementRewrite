/* Production HUD visibility clocks and opacity, with controlled input and camera status. */
#include <stdint.h>
#include <math.h>
#include "test_support.h"
typedef int32_t s32;
typedef int8_t s8;
typedef int16_t s16;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef float f32;
typedef u8 Texture;
typedef int Gfx;
#define WIDESCREEN 1
#define INPUT_DISPLAY
#define GFX_DIMENSIONS_FROM_LEFT_EDGE(x) (x)
static int sInputDisplayEnabled;
static int input_display_enabled(void) {
    return sInputDisplayEnabled;
}
#define TRUE 1
#define FALSE 0
#define HUD_FLASH_STARS 1
#define GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(x) (320 - (x))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define TIME_STOP_ACTIVE 1
#define INPUT_NONZERO_ANALOG 1
#define DIALOG_NONE 0
#define CUTSCENE_NONE 0
#define ACT_IDLE 1
#define ACT_HOLD_IDLE 2
#define ACT_HOLD_HEAVY_IDLE 3
#define ACT_START_SLEEPING 4
#define ACT_SLEEPING 5
#define ACT_WALKING 6
#define CAM_STATUS_NONE 0
#define CAM_STATUS_MODE_GROUP 0xF
#define CAM_STATUS_MARIO 1
#define CAM_STATUS_LAKITU 2
#define CAM_STATUS_C_UP 0x10
#define CAM_STATUS_C_DOWN 0x20
#define G_IM_FMT_RGBA 0
#define G_IM_SIZ_16b 0
#define G_TX_LOADTILE 0
#define G_TX_RENDERTILE 0
#define G_TX_WRAP 0
#define G_TX_NOMIRROR 0
#define G_TX_NOMASK 0
#define G_TX_NOLOD 0
#define G_TX_CLAMP 0
#define G_TEXTURE_IMAGE_FRAC 2
#define CALC_DXT(a, b) 0
#define G_IM_SIZ_16b_BYTES 2
static s32 sPipeSyncCount, sEnvColorCount;
#define gDPPipeSync(p) ((void) (p), sPipeSyncCount++)
#define gDPSetTextureImage(p, ...) ((void) (p))
#define gSPDisplayList(p, list) ((void) (p), record_display_list(list))
#define gDPSetTile(p, ...) ((void) (p))
#define gDPTileSync(p) ((void) (p))
#define gDPSetTileSize(p, ...) ((void) (p))
#define gDPLoadSync(p) ((void) (p))
#define gDPLoadBlock(p, ...) ((void) (p))
static s32 sRectCount, sRectDsdx, sRectDtdy;
static void record_rect(s32 dsdx, s32 dtdy) {
    sRectCount++;
    sRectDsdx = dsdx;
    sRectDtdy = dtdy;
}
#define gSPTextureRectangle(p, xl, yl, xh, yh, tile, s, t, dsdx, dtdy)                                 \
    ((void) (p), (void) (xl), (void) (yl), (void) (xh), (void) (yh), record_rect(dsdx, dtdy))
static Gfx sDisplayList[128], *gDisplayListHead = sDisplayList;
static Gfx dl_hud_img_load_tex_block;
struct Controller {
    s32 buttonDown;
};
struct MarioState {
    u32 flags;
    u16 capTimer;
    s32 action, input;
    void *floor;
    struct Controller *controller;
    f32 lateralSpeed;
};
struct Camera {
    s32 cutscene;
};
struct Area {
    struct Camera *camera;
};
struct CameraHUD {
    s16 status;
};
static struct CameraHUD sCameraHUD;
static u16 sCapTimerPeak, sCapTimerPrevious;
static u8 sHudIdleUpdates, sCounterRevealUpdates, sCameraRevealUpdates, sCounterAlpha, sCameraAlpha,
    sCapTimerAlpha, sCapTimerRevealUpdates;
static struct {
    s32 stars;
} gHudDisplay;
static s32 gHudFlash;
static u32 gGlobalTimer;
static u8 sTextLabelAlpha = 255;
static s32 sStarGlyphCount;
static u8 sStarGlyphAlpha[3];
static void print_text_set_alpha(u8 alpha);
static void print_text(s32 x, s32 y, const char *text) {
    if (sStarGlyphCount >= 3) {
        fputs("HARNESS ERROR: too many star counter glyphs\n", stderr);
        exit(2);
    }
    sStarGlyphAlpha[sStarGlyphCount++] = sTextLabelAlpha;
}
static void print_text_fmt_int(s32 x, s32 y, const char *format, s32 value) {
    print_text(x, y, format);
}
static s32 gTimeStopState, sDialog;
static struct Camera camera;
static struct Area area = { &camera }, *gCurrentArea = &area;
static struct Controller controller;
static struct MarioState mario, *gMarioState = &mario;
#define MENU_MTX_PUSH 1
#define MENU_MTX_NOPUSH 0
#define G_MTX_MODELVIEW 0
#define GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(x) (x)
#define DIALOG_CHAR_TERMINATOR 255
#define TEXT_HUD_CAP_WING 'W', 'I', 'N', 'G', 255
#define TEXT_HUD_CAP_METAL 'M', 'E', 'T', 'A', 'L', 255
#define TEXT_HUD_CAP_VANISH 'V', 'A', 'N', 'I', 'S', 'H', 255
#define TEXT_HUD_CAP_COMBINED 'C', 'A', 'P', 255
#define TEXT_HUD_CAP_SECONDS 's', 255
static Gfx dl_draw_text_bg_box[1], dl_ia_text_begin[1], dl_ia_text_end[1];
static u8 sEnv[4], sAccent[4], sPrinted[6][8], sPrintedColors[6][4];
static s32 sAccentCount, sTextCount;
static s16 sPrintedX[6], sPrintedY[6];
static f32 sMatrixX, sMatrixY, sScaleX, sScaleY, sAccentX, sAccentY, sAccentWidth, sAccentHeight;
static void set_env(u8 r, u8 g, u8 b, u8 a) {
    sEnv[0] = r;
    sEnv[1] = g;
    sEnv[2] = b;
    sEnv[3] = a;
    sEnvColorCount++;
}
#define gDPSetEnvColor(p, r, g, b, a) ((void) (p), set_env(r, g, b, a))
#define gSPPopMatrix(p, m) ((void) (p), (void) (m))
static void create_dl_translation_matrix(s32 mode, f32 x, f32 y, f32 z) {
    sMatrixX = x;
    sMatrixY = y;
}
static void create_dl_scale_matrix(s32 mode, f32 x, f32 y, f32 z) {
    sScaleX = x;
    sScaleY = y;
}
static void record_display_list(const Gfx *list) {
    if (list == dl_draw_text_bg_box) {
        sAccentCount++;
        memcpy(sAccent, sEnv, 4);
        sAccentX = sMatrixX;
        sAccentY = sMatrixY;
        sAccentWidth = sScaleX * 130;
        sAccentHeight = sScaleY * 80;
    }
}
static void print_generic_string(s16 x, s16 y, const u8 *text) {
    if (sTextCount >= 6) {
        return;
    }
    int i = 0;
    do {
        sPrinted[sTextCount][i] = text[i];
    } while (text[i++] != 255 && i < 8);
    sPrintedX[sTextCount] = x;
    sPrintedY[sTextCount] = y;
    memcpy(sPrintedColors[sTextCount], sEnv, 4);
    sTextCount++;
}
static void reset_cap_draw(void) {
    gDisplayListHead = sDisplayList;
    sAccentCount = sTextCount = 0;
    sPipeSyncCount = sEnvColorCount = 0;
    memset(sAccent, 0, sizeof(sAccent));
    memset(sPrinted, 0, sizeof(sPrinted));
}

static s32 get_dialog_id(void) {
    return sDialog;
}
#include "hud_definitions.inc"
#include "hud_sources.inc"

static void begin(void) {
    sInputDisplayEnabled = FALSE;
    memset(&mario, 0, sizeof(mario));
    memset(&controller, 0, sizeof(controller));
    mario.action = ACT_WALKING;
    mario.floor = &area;
    mario.controller = &controller;
    camera.cutscene = CUTSCENE_NONE;
    gTimeStopState = sDialog = 0;
    hud_reset_auto_visibility();
}

static int fade_timing(void) {
    begin();
    EXPECT_EQ(sCounterAlpha, 255);
    EXPECT_EQ(sCameraAlpha, 0);
    for (int i = 0; i < 89; i++) {
        hud_update_auto_visibility(&mario);
    }
    EXPECT_EQ(sCounterAlpha, 255);
    hud_update_auto_visibility(&mario);
    EXPECT_EQ(sCounterAlpha, 255 - HUD_FADE_STEP);
    for (int i = 0; i < 5; i++) {
        hud_update_auto_visibility(&mario);
    }
    EXPECT_EQ(sCounterAlpha, 0);
    hud_reveal_counters();
    hud_update_auto_visibility(&mario);
    EXPECT_EQ(sCounterAlpha, HUD_FADE_STEP);
    for (int i = 0; i < 5; i++) {
        hud_update_auto_visibility(&mario);
    }
    EXPECT_EQ(sCounterAlpha, 255);
    return 1;
}

static int idle_and_camera_rotation(void) {
    begin();
    set_hud_camera_status(CAM_STATUS_LAKITU);
    mario.action = ACT_IDLE;
    for (int i = 0; i < 59; i++) {
        hud_update_auto_visibility(&mario);
    }
    EXPECT_EQ(sCameraAlpha, 0);
    hud_update_auto_visibility(&mario);
    EXPECT_EQ(sCameraAlpha, HUD_FADE_STEP);
    for (int i = 0; i < 5; i++) {
        hud_update_auto_visibility(&mario);
    }
    EXPECT_EQ(sCameraAlpha, 255);
    mario.action = ACT_WALKING;
    set_hud_camera_status(CAM_STATUS_LAKITU | CAM_STATUS_C_DOWN);
    hud_update_auto_visibility(&mario);
    EXPECT_EQ(sCameraRevealUpdates, 0);
    EXPECT_EQ(sCameraAlpha, 255 - HUD_FADE_STEP);
    set_hud_camera_status(CAM_STATUS_LAKITU | CAM_STATUS_C_DOWN);
    for (int i = 0; i < 5; i++) {
        hud_update_auto_visibility(&mario);
    }
    EXPECT_EQ(sCameraAlpha, 0);
    set_hud_camera_status(CAM_STATUS_MARIO);
    hud_update_auto_visibility(&mario);
    EXPECT_EQ(sCameraRevealUpdates, HUD_REVEAL_UPDATES - 1);
    EXPECT_EQ(sCameraAlpha, HUD_FADE_STEP);
    return 1;
}

static int c_up_reveal(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin();
        set_hud_camera_status(CAM_STATUS_LAKITU);
        for (int update = 0; update < HUD_REVEAL_UPDATES + HUD_FADE_UPDATES; update++) {
            hud_update_auto_visibility(&mario);
        }
        EXPECT_EQ(sCounterAlpha, 0);
        EXPECT_EQ(sCameraAlpha, 0);
        set_hud_camera_status(CAM_STATUS_LAKITU | CAM_STATUS_C_UP);
        EXPECT_EQ(sCameraRevealUpdates, HUD_REVEAL_UPDATES);
        for (int update = 0; update < HUD_FADE_UPDATES; update++) {
            hud_update_auto_visibility(&mario);
            set_hud_camera_status(CAM_STATUS_LAKITU | CAM_STATUS_C_UP);
            EXPECT_EQ(sCameraRevealUpdates, HUD_REVEAL_UPDATES - update - 1);
        }
        EXPECT_EQ(sCameraAlpha, 255);
        EXPECT_EQ(sCounterAlpha, 0);
        for (int update = HUD_FADE_UPDATES; update < HUD_REVEAL_UPDATES + HUD_FADE_UPDATES; update++) {
            hud_update_auto_visibility(&mario);
        }
        EXPECT_EQ(sCameraAlpha, 0);
        // Exiting keeps the same Mario/Lakitu selection, but reveals the icon again.
        set_hud_camera_status(CAM_STATUS_LAKITU);
        EXPECT_EQ(sCameraRevealUpdates, HUD_REVEAL_UPDATES);
        for (int update = 0; update < HUD_FADE_UPDATES; update++) {
            hud_update_auto_visibility(&mario);
        }
        EXPECT_EQ(sCameraAlpha, 255);
        EXPECT_EQ(sCounterAlpha, 0);
        set_hud_camera_status(CAM_STATUS_LAKITU | CAM_STATUS_C_DOWN);
        EXPECT_EQ(sCameraRevealUpdates, HUD_REVEAL_UPDATES - HUD_FADE_UPDATES);
        set_hud_camera_status(CAM_STATUS_MARIO | CAM_STATUS_C_UP);
        EXPECT_EQ(sCameraRevealUpdates, HUD_REVEAL_UPDATES);
        set_hud_camera_status(CAM_STATUS_NONE); // Forced camera exit also removes C-Up.
        EXPECT_EQ(sCameraRevealUpdates, HUD_REVEAL_UPDATES);
    }
    return TRUE;
}

static int pause_reveal_and_freeze(void) {
    begin();
    for (int i = 0; i < 96; i++) {
        hud_update_auto_visibility(&mario);
    }
    EXPECT_EQ(sCounterAlpha, 0);
    EXPECT_EQ(sCameraAlpha, 0);
    hud_reveal_after_pause();
    EXPECT_EQ(sCounterAlpha, 255);
    EXPECT_EQ(sCameraAlpha, 255);
    gTimeStopState = TIME_STOP_ACTIVE;
    for (int i = 0; i < 20; i++) {
        hud_update_auto_visibility(&mario);
    }
    EXPECT_EQ(sCounterRevealUpdates, HUD_REVEAL_UPDATES);
    EXPECT_EQ(sCameraRevealUpdates, HUD_REVEAL_UPDATES);
    gTimeStopState = 0;
    for (int i = 0; i < 89; i++) {
        hud_update_auto_visibility(&mario);
    }
    EXPECT_EQ(sCounterAlpha, 255);
    EXPECT_EQ(sCameraAlpha, 255);
    hud_update_auto_visibility(&mario);
    EXPECT_EQ(sCounterAlpha, 255 - HUD_FADE_STEP);
    EXPECT_EQ(sCameraAlpha, 255 - HUD_FADE_STEP);
    return 1;
}

static int glyph_texture_steps(void) {
    Texture texture = 0;
    gDisplayListHead = sDisplayList;
    sRectCount = 0;
    render_hud_tex_lut(0, 0, &texture, FALSE);
    EXPECT_EQ(sRectCount, 1);
    EXPECT_EQ(sRectDsdx, 4 << 10);
    EXPECT_EQ(sRectDtdy, 1 << 10);
    render_hud_tex_lut(0, 0, &texture, TRUE);
    EXPECT_EQ(sRectDsdx, 1 << 10);
    EXPECT_EQ(sRectDtdy, 1 << 10);
    render_hud_small_tex_lut(0, 0, &texture, FALSE);
    EXPECT_EQ(sRectDsdx, 4 << 10);
    render_hud_small_tex_lut(0, 0, &texture, TRUE);
    EXPECT_EQ(sRectDsdx, 1 << 10);
    render_textrect(0, 0, 0, FALSE);
    EXPECT_EQ(sRectDsdx, 4 << 10);
    render_textrect(0, 0, 0, TRUE);
    EXPECT_EQ(sRectDsdx, 1 << 10);
    EXPECT_EQ(sRectDtdy, 1 << 10);
    EXPECT_EQ(sRectCount, 6);
    return 1;
}

static int cap_timer_indicator(void) {
    const u16 times[] = { 1, 29, 30, 31, 149, 150, 151, 299, 300, 301, 2550, 6570, 65535 };
    const u8 expectedColors[][3] = { { 255, 80, 80 },   { 255, 80, 80 },   { 255, 80, 80 },
                                     { 255, 80, 80 },   { 255, 80, 80 },   { 255, 224, 96 },
                                     { 255, 224, 96 },  { 255, 224, 96 },  { 255, 224, 96 },
                                     { 128, 255, 128 }, { 128, 255, 128 }, { 128, 255, 128 },
                                     { 128, 255, 128 } };
    begin();
    for (int cycle = 0; cycle < 4; cycle++) {
        sInputDisplayEnabled = cycle & 1;
        for (int caps = 1; caps <= 7; caps++) {
            for (unsigned time = 0; time < sizeof(times) / sizeof(*times); time++) {
                hud_reset_auto_visibility();
                mario.flags = (u32) caps << 1;
                mario.capTimer = times[time];
                hud_update_cap_timer(&mario);
                reset_cap_draw();
                render_hud_cap_timer(sCapTimerAlpha);
                EXPECT_EQ(sAccentCount, 1);
                EXPECT_EQ(sTextCount, 4);
                EXPECT_EQ(mario.capTimer, times[time]);
                EXPECT_EQ(sPipeSyncCount, 3);
                EXPECT_EQ(sEnvColorCount, 3);
                for (int rgb = 0; rgb < 3; rgb++) {
                    EXPECT_EQ(sAccent[rgb], expectedColors[time][rgb]);
                }
                EXPECT_EQ(sAccent[3], 255);
                EXPECT_EQ(fabsf(sAccentHeight - 2) < 0.001f, TRUE);
                EXPECT_EQ(sAccentX + sAccentWidth <= sPrintedX[3] + 6.001f, TRUE);
                EXPECT_EQ(sAccentWidth >= 0, TRUE);
                EXPECT_EQ(sAccentY, 20);
                // Countdown and suffix keep white foreground with a dark shadow.
                for (int text = 0; text < 4; text++) {
                    for (int rgb = 0; rgb < 3; rgb++) {
                        EXPECT_EQ(sPrintedColors[text][rgb], text < 2 ? 0 : 255);
                    }
                }
                for (int part = 0; part < 2; part++) {
                    EXPECT_EQ(sPrintedX[part], sPrintedX[part + 2] + 1);
                    EXPECT_EQ(sPrintedY[part], sPrintedY[part + 2] - 1);
                    EXPECT_EQ(sPrintedColors[part][3], 192);
                    EXPECT_EQ(sPrintedColors[part + 2][3], 255);
                }
                EXPECT_EQ(sPrinted[3][0], 's');
                int seconds = (times[time] + 29) / 30;
                int digits;
                if (seconds >= 1000) {
                    digits = 4;
                } else if (seconds >= 100) {
                    digits = 3;
                } else if (seconds >= 10) {
                    digits = 2;
                } else {
                    digits = 1;
                }
                EXPECT_EQ(sPrintedX[3] - sPrintedX[2], digits * 7 + 2);
                if (sInputDisplayEnabled) {
                    EXPECT_EQ(fabsf((sPrintedX[2] + sPrintedX[3] + 6.0f) / 2 - HUD_POWER_METER_X)
                                  <= 0.5f,
                              TRUE);
                } else {
                    EXPECT_EQ(sPrintedX[3], 62);
                }
                for (int digit = digits - 1; digit >= 0; digit--) {
                    EXPECT_EQ(sPrinted[2][digit], seconds % 10);
                    seconds /= 10;
                }
                EXPECT_EQ(sPrinted[2][digits], 255);
            }
        }
    }
    // Extensions return to green, and expiry/removal hide both text and accent.
    mario.flags = MARIO_VANISH_CAP;
    mario.capTimer = 149;
    reset_cap_draw();
    render_hud_cap_timer(sCapTimerAlpha);
    EXPECT_EQ(sAccent[1], 80);
    mario.capTimer = 451;
    reset_cap_draw();
    render_hud_cap_timer(sCapTimerAlpha);
    EXPECT_EQ(sAccent[1], 255);
    mario.capTimer = 0;
    reset_cap_draw();
    render_hud_cap_timer(sCapTimerAlpha);
    EXPECT_EQ(sAccentCount, 0);
    EXPECT_EQ(sTextCount, 0);
    mario.flags = 0;
    mario.capTimer = 300;
    reset_cap_draw();
    render_hud_cap_timer(sCapTimerAlpha);
    EXPECT_EQ(sAccentCount, 0);
    EXPECT_EQ(sTextCount, 0);
    return TRUE;
}

static int cap_bar_phase_progress(void) {
    const u16 frames[] = { 3600, 1950, 301, 300, 225, 151, 150, 149, 75, 1 };
    const f32 fractions[] = { 1, .5f, 1.0f / 3300, 1, .5f, 1.0f / 150, 0, 1, 75.0f / 149, 1.0f / 149 };
    for (int cycle = 0; cycle < 3; cycle++) {
        begin();
        mario.flags = MARIO_METAL_CAP;
        for (unsigned i = 0; i < sizeof(frames) / sizeof(*frames); i++) {
            mario.capTimer = frames[i];
            hud_update_auto_visibility(&mario);
            reset_cap_draw();
            render_hud_cap_timer(sCapTimerAlpha);
            EXPECT_EQ(fabsf(sAccentWidth - 29 * fractions[i]) < .001f, TRUE);
            EXPECT_EQ(sAccentX, 39);
            EXPECT_EQ(sCapTimerPeak, 3600);
            EXPECT_EQ(mario.capTimer, frames[i]);
            // Repeated renders and a frozen timer do not drain the bar themselves.
            f32 width = sAccentWidth;
            reset_cap_draw();
            render_hud_cap_timer(sCapTimerAlpha);
            EXPECT_EQ(sAccentWidth, width);
            gTimeStopState = TIME_STOP_ACTIVE;
            hud_update_auto_visibility(&mario);
            reset_cap_draw();
            render_hud_cap_timer(sCapTimerAlpha);
            EXPECT_EQ(sAccentWidth, width);
            gTimeStopState = 0;
        }
        // Timer gains refill the green phase, then expiry clears its old denominator.
        mario.capTimer = 900;
        hud_update_auto_visibility(&mario);
        reset_cap_draw();
        render_hud_cap_timer(sCapTimerAlpha);
        EXPECT_EQ(sCapTimerPeak, 900);
        EXPECT_EQ(fabsf(sAccentWidth - 22) < .001f, TRUE);
        mario.capTimer = 0;
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerPeak, 0);
        mario.capTimer = 1800;
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerPeak, 1800);
        mario.flags = 0;
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerPeak, 0);
    }
    return TRUE;
}

static int cap_timer_fade(void) {
    for (int caps = 1; caps <= 7; caps++) {
        begin();
        mario.flags = (u32) caps << 1;
        mario.capTimer = 900;
        for (int update = 0; update < HUD_REVEAL_UPDATES - 1; update++) {
            hud_update_auto_visibility(&mario);
            EXPECT_EQ(sCapTimerAlpha, 255);
        }
        for (int update = 0; update < HUD_FADE_UPDATES; update++) {
            hud_update_auto_visibility(&mario);
            EXPECT_EQ(sCapTimerAlpha, MAX(0, 255 - (update + 1) * HUD_FADE_STEP));
            EXPECT_EQ(sCapTimerAlpha, sCounterAlpha);
        }
        reset_cap_draw();
        render_hud_cap_timer(sCapTimerAlpha);
        EXPECT_EQ(sTextCount, 0);
        EXPECT_EQ(sAccentCount, 0);
        EXPECT_EQ(gDisplayListHead == sDisplayList, TRUE);

        // Green stays hidden; amber begins its gradual reveal at ten seconds.
        mario.capTimer = 301;
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerAlpha, 0);
        mario.capTimer = 300;
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerAlpha, HUD_FADE_STEP);
        EXPECT_EQ(sCounterAlpha, 0);
        reset_cap_draw();
        render_hud_cap_timer(sCapTimerAlpha);
        EXPECT_EQ(sTextCount, 4);
        EXPECT_EQ(sAccent[3], HUD_FADE_STEP);
        EXPECT_EQ(sPrintedColors[0][3], 192 * HUD_FADE_STEP / 255);
        EXPECT_EQ(sPrintedColors[2][3], HUD_FADE_STEP);
        gTimeStopState = TIME_STOP_ACTIVE;
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerAlpha, HUD_FADE_STEP);
        gTimeStopState = 0;
        for (int update = 1; update < HUD_FADE_UPDATES; update++) {
            hud_update_auto_visibility(&mario);
            EXPECT_EQ(sCapTimerAlpha, MIN(255, (update + 1) * HUD_FADE_STEP));
        }
        mario.capTimer = 149;
        for (int update = 0; update < HUD_REVEAL_UPDATES; update++) {
            hud_update_auto_visibility(&mario);
            EXPECT_EQ(sCapTimerAlpha, 255);
            EXPECT_EQ(sCounterAlpha, 0);
        }
        // An extension into green returns to the normal fade-out rule.
        mario.capTimer = 900;
        for (int update = 0; update < HUD_FADE_UPDATES; update++) {
            hud_update_auto_visibility(&mario);
            EXPECT_EQ(sCapTimerAlpha, MAX(0, 255 - (update + 1) * HUD_FADE_STEP));
        }
        hud_reveal_counters();
        for (int update = 0; update < HUD_FADE_UPDATES; update++) {
            hud_update_auto_visibility(&mario);
            EXPECT_EQ(sCapTimerAlpha, sCounterAlpha);
        }
        EXPECT_EQ(sCapTimerAlpha, 255);

        // A pause draw overrides hidden gameplay alpha without ticking the fade.
        sCapTimerAlpha = 0;
        reset_cap_draw();
        render_hud_cap_timer(255);
        EXPECT_EQ(sTextCount, 4);
        EXPECT_EQ(sPrintedColors[2][3], 255);
        EXPECT_EQ(sCapTimerAlpha, 0);
        hud_reveal_after_pause();
        EXPECT_EQ(sCapTimerAlpha, 255);
        gTimeStopState = TIME_STOP_ACTIVE;
        for (int update = 0; update < 20; update++) {
            hud_update_auto_visibility(&mario);
        }
        EXPECT_EQ(sCapTimerAlpha, 255);
        EXPECT_EQ(sCounterRevealUpdates, HUD_REVEAL_UPDATES);
    }
    begin();
    mario.flags = MARIO_WING_CAP;
    mario.capTimer = 900;
    for (int update = 0; update < HUD_REVEAL_UPDATES + HUD_FADE_UPDATES; update++) {
        hud_update_auto_visibility(&mario);
    }
    mario.action = ACT_IDLE;
    for (int update = 0; update < HUD_IDLE_UPDATES - 1; update++) {
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerAlpha, 0);
    }
    for (int update = 0; update < HUD_FADE_UPDATES; update++) {
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerAlpha, sCounterAlpha);
    }
    EXPECT_EQ(sCapTimerAlpha, 255);
    mario.action = ACT_WALKING;
    for (int update = 0; update < HUD_FADE_UPDATES; update++) {
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerAlpha, sCounterAlpha);
    }
    EXPECT_EQ(sCapTimerAlpha, 0);
    begin();
    mario.capTimer = 149;
    for (int update = 0; update < HUD_REVEAL_UPDATES + HUD_FADE_UPDATES; update++) {
        hud_update_auto_visibility(&mario);
    }
    EXPECT_EQ(sCapTimerAlpha, 0); // No active power must not force urgency visibility.
    return TRUE;
}

static int cap_timer_pickup_reveal(void) {
    for (int caps = 1; caps <= 7; caps++) {
        begin();
        mario.flags = (u32) caps << 1;
        mario.capTimer = 1800;
        for (int update = 0; update < HUD_REVEAL_UPDATES + HUD_FADE_UPDATES; update++) {
            hud_update_auto_visibility(&mario);
        }
        EXPECT_EQ(sCapTimerAlpha, 0);
        hud_reveal_cap_timer();
        EXPECT_EQ(sCapTimerRevealUpdates, 150);
        for (int update = 0; update < HUD_CAP_PICKUP_REVEAL_UPDATES - 1; update++) {
            hud_update_auto_visibility(&mario);
            EXPECT_EQ(sCapTimerRevealUpdates, HUD_CAP_PICKUP_REVEAL_UPDATES - update - 1);
            EXPECT_EQ(sCapTimerAlpha, MIN(255, (update + 1) * HUD_FADE_STEP));
            EXPECT_EQ(sCounterAlpha, 0);
        }
        for (int update = 0; update < HUD_FADE_UPDATES; update++) {
            hud_update_auto_visibility(&mario);
            EXPECT_EQ(sCapTimerAlpha, MAX(0, 255 - (update + 1) * HUD_FADE_STEP));
        }
        EXPECT_EQ(sCapTimerRevealUpdates, 0);
        // Another pickup refreshes the reveal even without changing capTimer.
        hud_reveal_cap_timer();
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerRevealUpdates, 149);
        EXPECT_EQ(sCapTimerAlpha, HUD_FADE_STEP);
        hud_reveal_cap_timer();
        EXPECT_EQ(sCapTimerRevealUpdates, 150);
        gTimeStopState = TIME_STOP_ACTIVE;
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerRevealUpdates, 150);
        EXPECT_EQ(sCapTimerAlpha, HUD_FADE_STEP);
        gTimeStopState = 0;
        mario.capTimer += 60; // A coin extension does not refresh the pickup clock.
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerRevealUpdates, 149);
        mario.capTimer = 0;
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerRevealUpdates, 0);
        hud_reveal_cap_timer();
        mario.capTimer = 1800;
        mario.flags = 0;
        hud_update_auto_visibility(&mario);
        EXPECT_EQ(sCapTimerRevealUpdates, 0);
        hud_reveal_cap_timer();
        hud_reset_auto_visibility();
        EXPECT_EQ(sCapTimerRevealUpdates, 0);
    }
    return TRUE;
}

static int star_counter_blink(void) {
    const u8 alphas[] = { 255, 127, 51, 20, 0 };
    for (s32 reduced = 0; reduced < 2; reduced++) {
        sReducedFlashing = reduced;
        for (s32 cycle = 0; cycle < 3; cycle++) {
            for (s32 flashing = 0; flashing < 2; flashing++) {
                for (s32 largeCount = 0; largeCount < 2; largeCount++) {
                    for (u32 a = 0; a < sizeof(alphas); a++) {
                        gHudFlash = flashing ? HUD_FLASH_STARS : 0;
                        gHudDisplay.stars = largeCount ? 100 : 99;
                        for (gGlobalTimer = 0; gGlobalTimer < 32; gGlobalTimer++) {
                            sStarGlyphCount = 0;
                            print_text_set_alpha(alphas[a]);
                            render_hud_stars(alphas[a]);
                            EXPECT_EQ(sStarGlyphCount, largeCount ? 2 : 3);
                            u8 expected = alphas[a];
                            if (flashing && (reduced || (gGlobalTimer & 8))) {
                                expected = MIN(expected, reduced ? 128 : 51);
                            }
                            for (s32 glyph = 0; glyph < sStarGlyphCount; glyph++) {
                                EXPECT_EQ(sStarGlyphAlpha[glyph], expected);
                            }
                            EXPECT_EQ(sTextLabelAlpha, alphas[a]);
                        }
                    }
                }
            }
        }
    }
    sReducedFlashing = 0;
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
#define RUN(name, fn)                                                                                  \
    if (test_selected("hud." name))                                                                    \
    test_report("hud." name, fn())
    RUN("fade_timing", fade_timing);
    RUN("idle_and_camera_rotation", idle_and_camera_rotation);
    RUN("c_up_reveal", c_up_reveal);
    RUN("pause_reveal_and_freeze", pause_reveal_and_freeze);
    RUN("glyph_texture_steps", glyph_texture_steps);
    RUN("cap_timer_indicator", cap_timer_indicator);
    RUN("cap_bar_phase_progress", cap_bar_phase_progress);
    RUN("cap_timer_fade", cap_timer_fade);
    RUN("cap_timer_pickup_reveal", cap_timer_pickup_reveal);
    RUN("star_counter_blink", star_counter_blink);
    return test_finish();
}
