#include <ultra64.h>

#include "config.h"

#ifdef INPUT_DISPLAY

#include "sm64.h"
#include "gfx_dimensions.h"
#include "engine/math_util.h"
#include "game_init.h"
#include "hud.h"
#include "input_display.h"
#include "game_options_menu.h"
#include "game_options.h"
#include "input_history.h"
#include "puppyprint.h"
#include "libc/stdio.h"

struct InputDisplaySprite {
    const Texture *texture;
    u16 width;
    u16 height;
};

#include "input_display_assets.inc.c"

static const struct InputDisplaySprite *const sStickArrows[8] = {
    &sInputDisplayArrow0, &sInputDisplayArrow1, &sInputDisplayArrow2, &sInputDisplayArrow3,
    &sInputDisplayArrow4, &sInputDisplayArrow5, &sInputDisplayArrow6, &sInputDisplayArrow7,
};
static const struct InputDisplaySprite *const sDirections[4] = {
    &sInputDisplayDirection0,
    &sInputDisplayDirection1,
    &sInputDisplayDirection2,
    &sInputDisplayDirection3,
};
static const struct InputDisplaySprite *const sCDirections[4] = {
    &sInputDisplayCDirection0,
    &sInputDisplayCDirection1,
    &sInputDisplayCDirection2,
    &sInputDisplayCDirection3,
};
static const struct InputDisplaySprite *const sCountDigits[10] = {
    &sInputDisplayCount0, &sInputDisplayCount1, &sInputDisplayCount2, &sInputDisplayCount3,
    &sInputDisplayCount4, &sInputDisplayCount5, &sInputDisplayCount6, &sInputDisplayCount7,
    &sInputDisplayCount8, &sInputDisplayCount9,
};
static const struct InputDisplaySprite *const sCButtons[4] = {
    &sInputDisplayC0,
    &sInputDisplayC1,
    &sInputDisplayC2,
    &sInputDisplayC3,
};
static const struct InputDisplaySprite *const sDButtons[4] = {
    &sInputDisplayD0,
    &sInputDisplayD1,
    &sInputDisplayD2,
    &sInputDisplayD3,
};
static const char *const sInputLabels[INPUT_HISTORY_CHANNELS] = {
    [INPUT_STICK] = "",   [INPUT_D_UP] = "D", [INPUT_D_RIGHT] = "D", [INPUT_D_DOWN] = "D",
    [INPUT_D_LEFT] = "D", [INPUT_B] = "B",    [INPUT_L] = "L",       [INPUT_START] = "S",
    [INPUT_A] = "A",      [INPUT_C_UP] = "C", [INPUT_C_RIGHT] = "C", [INPUT_C_DOWN] = "C",
    [INPUT_C_LEFT] = "C", [INPUT_Z] = "Z",    [INPUT_R] = "R",

};
static const u8 sInputColors[INPUT_HISTORY_CHANNELS][3] = {
    [INPUT_STICK] = { 255, 255, 255 },   [INPUT_D_UP] = { 208, 208, 208 },
    [INPUT_D_RIGHT] = { 208, 208, 208 }, [INPUT_D_DOWN] = { 208, 208, 208 },
    [INPUT_D_LEFT] = { 208, 208, 208 },  [INPUT_B] = { 56, 255, 88 },
    [INPUT_L] = { 208, 208, 208 },       [INPUT_START] = { 255, 80, 80 },
    [INPUT_A] = { 96, 152, 255 },        [INPUT_C_UP] = { 255, 255, 0 },
    [INPUT_C_RIGHT] = { 255, 255, 0 },   [INPUT_C_DOWN] = { 255, 255, 0 },
    [INPUT_C_LEFT] = { 255, 255, 0 },    [INPUT_Z] = { 208, 208, 208 },
    [INPUT_R] = { 208, 208, 208 },

};

static struct InputHistory sInputHistory;
static f32 sInputStickX;
static f32 sInputStickY;
static f32 sInputStickMagnitude;
static u16 sStickAngleBinary;
// Hundredths of a percent from the deadzone-adjusted stick magnitude.
static u16 sStickPowerPercent100;

s32 input_display_enabled(void) {
    return game_options_get(GO_INPUT_DISPLAY) || game_options_get(GO_CONTROLLER_DISPLAY);
}

void input_display_update(const struct Controller *controller) {
    u8 sector = INPUT_STICK_NEUTRAL;

    // Raw direction remains visible even when gameplay's deadzone ignores it.
    sInputStickX = controller->rawStickX;
    sInputStickY = controller->rawStickY;
    sInputStickMagnitude = sqrtf(sInputStickX * sInputStickX + sInputStickY * sInputStickY);
    sStickAngleBinary = 0;
    if (sInputStickMagnitude > 0.0f) {
        // SM64 atan2s measures from its first argument; right is zero for this readout.
        sStickAngleBinary = (u16) atan2s(sInputStickX, sInputStickY);
        sector = ((sStickAngleBinary + 0x1000) >> 13) & 7;
    }
    sStickPowerPercent100 = (u16) (controller->stickMag * (10000.0f / 64.0f) + 0.5f);
    input_history_update(&sInputHistory, controller->buttonDown, sector);
}

static s32 input_display_left(void) {
    return GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(1);
}

static void input_display_sprite(const struct InputDisplaySprite *sprite, s32 x, s32 y, s32 width,
                                 s32 height) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetRenderMode(gDisplayListHead++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineLERP(gDisplayListHead++, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0,
                      0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0);
    draw_sprite(&gDisplayListHead, sprite->texture, G_IM_FMT_RGBA, G_IM_SIZ_16b, FALSE, sprite->width,
                sprite->height, input_display_left() + x, y - 2, width, height);
}

static void input_display_texture(const struct InputDisplaySprite *sprite, s32 x, s32 y) {
    input_display_sprite(sprite, x, y, sprite->width, sprite->height);
}

static void input_display_text(const char *text, s32 x, s32 y) {
    print_small_text_light(input_display_left() + x, y - 2, text, PRINT_TEXT_ALIGN_LEFT, PRINT_ALL,
                           FONT_OUTLINE);
}

static void input_display_count(const char *text, s32 x, s32 y) {
    while (*text != '\0') {
        input_display_texture(sCountDigits[*text++ - '0'], x, y);
        x += 7; // Match the outline font's six-pixel digit kerning plus its gap.
    }
}

static void input_display_entry(const struct InputHistoryEntry *entry, u8 channel, s32 x, s32 countX,
                                s32 y) {
    char frames[INPUT_HISTORY_COUNT_TEXT_SIZE];
    const u8 *color = sInputColors[channel];

    print_set_envcolour(color[0], color[1], color[2], 255);
    if (channel == INPUT_STICK) {
        input_display_texture(sStickArrows[entry->sector], x - 1, y - 2);
    } else {
        if (channel == INPUT_A) {
            input_display_texture(&sInputDisplayLabelA, x, y);
        } else {
            input_display_text(sInputLabels[channel], x, y);
        }
        // Leave a clear pixel after the eight-pixel letter, including its outline.
        // Arrow colours are baked so their gray borders stay independent of the letter tint.
        print_set_envcolour(255, 255, 255, 255);
        if (channel >= INPUT_D_UP && channel <= INPUT_D_LEFT) {
            input_display_texture(sDirections[channel - INPUT_D_UP], x + 9, y + 1);
        } else if (channel >= INPUT_C_UP && channel <= INPUT_C_LEFT) {
            input_display_texture(sCDirections[channel - INPUT_C_UP], x + 9, y + 1);
        }
    }
    print_set_envcolour(255, 255, 255, 255);
    input_history_format_count(entry->held[channel], frames);
    input_display_count(frames, MAX(x + 20, countX - MAX(0, get_text_width(frames, FONT_OUTLINE) - 14)),
                        y);
}

static void input_display_history(void) {
    u8 index;
    s32 row = 0;
    s32 controller = game_options_get(GO_CONTROLLER_DISPLAY);
    s32 rowLimit;
    if (controller == 0) {
        rowLimit = 15;
    } else if (controller == 2) {
        rowLimit = 12;
    } else {
        rowLimit = 10;
    }
    u8 channels[INPUT_HISTORY_CHANNELS];
    char frames[INPUT_HISTORY_COUNT_TEXT_SIZE];

    print_set_envcolour(255, 255, 255, 255);
    for (index = 0; index < sInputHistory.length; index++) {
        const struct InputHistoryEntry *entry = &sInputHistory.entries[index];
        u8 count = input_history_channels(entry, channels);
        s32 useTwoColumns = count > 3;
        for (u8 channel = 0; channel < count; channel++) {
            if (entry->held[channels[channel]] > 999) {
                useTwoColumns = FALSE;
            }
        }
        s32 rows = useTwoColumns ? (count + 1) / 2 : MAX(1, count);
        s32 y = 20 + row * 14;
        u8 input;

        // Only show complete entries so a partial bracket cannot imply missing simultaneous inputs.
        if (row + rows > rowLimit) {
            break;
        }
        if (count == 0) {
            input_display_texture(&sInputDisplayNeutral, 13, y - 2);
            input_history_format_count(entry->neutralFrames, frames);
            input_display_count(frames, entry->neutralFrames > 999 ? 34 : 42, y);
        } else {
            for (input = 0; input < count; input++) {
                s32 column = input / rows;
                input_display_entry(entry, channels[input], 14 + column * 50, 42 + column * 48,
                                    y + (input % rows) * 14);
            }
        }
        if (count > 1) {
            input_display_sprite(&sInputDisplayBracketVertical, 6, y - 2, 3, rows * 14 - 1);
            input_display_texture(&sInputDisplayBracketCap, 6, y - 2);
            input_display_texture(&sInputDisplayBracketCap, 6, y + rows * 14 - 6);
        }
        row += rows;
    }
}

static void input_display_mini_controller(u16 buttons) {
    static const struct InputDisplaySprite *const cButtons[4] = {
        &sInputDisplayMiniC0,
        &sInputDisplayMiniC1,
        &sInputDisplayMiniC2,
        &sInputDisplayMiniC3,
    };
    static const struct InputDisplaySprite *const dButtons[4] = {
        &sInputDisplayMiniD0,
        &sInputDisplayMiniD1,
        &sInputDisplayMiniD2,
        &sInputDisplayMiniD3,
    };
    static const s16 cPositions[4][2] = { { 55, 199 }, { 59, 203 }, { 55, 207 }, { 51, 203 } };
    f32 stickScale = 3.0f / MAX(70.0f, sInputStickMagnitude);
    s32 stickX = (s32) roundf(sInputStickX * stickScale);
    s32 stickY = (s32) roundf(sInputStickY * stickScale);
    s32 yOffset = (game_options_get(GO_CONTROLLER_NUMBERS) ? -6 : 0) - 1;

    if (sInputStickMagnitude > 0.0f && stickX == 0 && stickY == 0) {
        if (absf(sInputStickX) >= absf(sInputStickY)) {
            stickX = sInputStickX > 0.0f ? 1 : -1;
        } else {
            stickY = sInputStickY > 0.0f ? 1 : -1;
        }
    }
    print_set_envcolour(255, 255, 255, 255);
    input_display_texture(&sInputDisplayMiniBase, 19, 194 + yOffset);
    if (buttons & L_TRIG) {
        input_display_texture(&sInputDisplayMiniShoulderL, 24, 195 + yOffset);
    }
    if (buttons & R_TRIG) {
        input_display_texture(&sInputDisplayMiniShoulderR, 51, 195 + yOffset);
    }
    if (buttons & START_BUTTON) {
        input_display_texture(&sInputDisplayMiniStart, 41, 202 + yOffset);
    }
    if (buttons & B_BUTTON) {
        input_display_texture(&sInputDisplayMiniB, 46, 208 + yOffset);
    }
    if (buttons & A_BUTTON) {
        input_display_texture(&sInputDisplayMiniA, 53, 213 + yOffset);
    }
    if (buttons & Z_TRIG) {
        input_display_texture(&sInputDisplayMiniZ, 40, 221 + yOffset);
    }
    for (s32 i = 0; i < 4; i++) {
        if (buttons & gInputHistoryButtons[INPUT_C_UP + i]) {
            input_display_texture(cButtons[i], cPositions[i][0], cPositions[i][1] + yOffset);
        }
        if (buttons & gInputHistoryButtons[INPUT_D_UP + i]) {
            input_display_texture(dButtons[i], 32, 209 + yOffset);
        }
    }
    input_display_texture(&sInputDisplayMiniStick, 27 + stickX, 202 - stickY + yOffset);
}

static void input_display_controller_numbers(void) {
    char text[16];
    u32 angleDegrees100 = ((u32) sStickAngleBinary * 36000 + 32768) >> 16;

    if (!game_options_get(GO_CONTROLLER_NUMBERS)) {
        return;
    }
    sprintf(text, "%03u.%02u", angleDegrees100 / 100, angleDegrees100 % 100);
    input_display_text(text, 6, 227);
    input_display_texture(&sInputDisplayDegree, 6 + get_text_width(text, FONT_OUTLINE) + 1, 228);
    sprintf(text, "%03u.%02u%%", sStickPowerPercent100 / 100, sStickPowerPercent100 % 100);
    print_small_text_light(input_display_left() + 104, 225, text, PRINT_TEXT_ALIGN_RIGHT, PRINT_ALL,
                           FONT_OUTLINE);
}

static void input_display_controller(s32 miniController) {
    static const s16 cPositions[4][2] = { { 78, 171 }, { 86, 179 }, { 78, 187 }, { 70, 179 } };
    const struct InputHistoryEntry *entry = &sInputHistory.entries[0];
    u16 buttons = entry->buttons;
    s32 i;
    s32 yOffset = -1;
    f32 stickScale = 6.0f / MAX(70.0f, sInputStickMagnitude);
    s32 stickX = (s32) roundf(sInputStickX * stickScale);
    s32 stickY = (s32) roundf(sInputStickY * stickScale);

    if (miniController) {
        input_display_mini_controller(buttons);
        input_display_controller_numbers();
        return;
    }
    if (sInputStickMagnitude > 0.0f && stickX == 0 && stickY == 0) {
        if (absf(sInputStickX) >= absf(sInputStickY)) {
            stickX = sInputStickX > 0.0f ? 1 : -1;
        } else {
            stickY = sInputStickY > 0.0f ? 1 : -1;
        }
    }
    print_set_envcolour(255, 255, 255, 255);
    input_display_texture(&sInputDisplayBase, 6, 160 + yOffset);
    if (buttons & L_TRIG) {
        input_display_texture(&sInputDisplayShoulderL, 16, 162 + yOffset);
    }
    if (buttons & R_TRIG) {
        input_display_texture(&sInputDisplayShoulderR, 70, 162 + yOffset);
    }
    if (buttons & START_BUTTON) {
        input_display_texture(&sInputDisplayStart, 49, 176 + yOffset);
    }
    if (buttons & B_BUTTON) {
        input_display_texture(&sInputDisplayB, 61, 189 + yOffset);
    }
    if (buttons & A_BUTTON) {
        input_display_texture(&sInputDisplayA, 74, 198 + yOffset);
    }
    if (buttons & Z_TRIG) {
        input_display_texture(&sInputDisplayZ, 49, 214 + yOffset);
    }
    for (i = 0; i < 4; i++) {
        if (buttons & gInputHistoryButtons[INPUT_C_UP + i]) {
            input_display_texture(sCButtons[i], cPositions[i][0], cPositions[i][1] + yOffset);
        }
        if (buttons & gInputHistoryButtons[INPUT_D_UP + i]) {
            input_display_texture(sDButtons[i], 32, 190 + yOffset);
        }
    }
    input_display_texture(&sInputDisplayStick, 21 + stickX, 176 - stickY + yOffset);
    input_display_controller_numbers();
}

void render_input_display(void) {
    if (gOptionsMenu.open || !input_display_enabled() || sInputHistory.length == 0) {
        return;
    }
    // Draw after menus/fades, independently of their HUD masks or text colour state.
    bzero(gCurrEnvCol, sizeof(ColorRGBA));
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (game_options_get(GO_INPUT_DISPLAY)) {
        input_display_history();
    }
    s32 controller = game_options_get(GO_CONTROLLER_DISPLAY);
    if (controller) {
        input_display_controller(controller == 2);
    }
    print_set_envcolour(255, 255, 255, 255);
    gDPPipeSync(gDisplayListHead++);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_BILERP);
    gDPSetCombineMode(gDisplayListHead++, G_CC_SHADE, G_CC_SHADE);
}

#endif
