#include <ultra64.h>

#include "actors/common1.h"
#include "area.h"
#include "audio/external.h"
#include "camera.h"
#include "course_table.h"
#include "dialog_ids.h"
#include "engine/math_util.h"
#include "eu_translation.h"
#include "segment_symbols.h"
#include "game_init.h"
#include "gfx_dimensions.h"
#include "ingame_menu.h"
#include "level_update.h"
#include "jrb_simpleflips.h"
#include "levels/castle_grounds/header.h"
#include "memory.h"
#include "print.h"
#include "player_stats_menu.h"
#include "game_options_menu.h"
#include "input_display.h"
#include "save_file.h"
#include "segment2.h"
#include "segment7.h"
#include "seq_ids.h"
#include "sm64.h"
#include "mario_character_text.h"
#include "text_strings.h"
#include "types.h"
#include "config.h"
#include "main.h"

#ifdef VERSION_EU
#undef LANGUAGE_FUNCTION
#define LANGUAGE_FUNCTION gInGameLanguage
#endif

u16 gDialogColorFadeTimer;
s8 gLastDialogLineNum;
s32 gDialogVariable;
u16 gDialogTextAlpha;
s16 gCutsceneMsgXOffset;
s16 gCutsceneMsgYOffset;
s8 gRedCoinsCollected;
s8 gRedCoinStarSpawned;
#ifdef WIDE
u8 textCurrRatio43[] = { TEXT_HUD_CURRENT_RATIO_43 };
u8 textCurrRatio169[] = { TEXT_HUD_CURRENT_RATIO_169 };
u8 textPressL[] = { TEXT_HUD_PRESS_L };
#endif

#if MULTILANG
#define seg2_course_name_table course_name_table_eu_en
#define seg2_act_name_table act_name_table_eu_en
#define seg2_dialog_table dialog_table_eu_en
#endif

s16 gInGameLanguage = LANGUAGE_ENGLISH;
s16 gLoadedLanguage = LANGUAGE_ENGLISH;
s16 gMenuMode = MENU_MODE_NONE;

void *languageTable[][3] = {
    {&seg2_dialog_table, &seg2_course_name_table, &seg2_act_name_table}, // In EU, this is just mirroring English.
#if MULTILANG
    {&dialog_table_eu_en, &course_name_table_eu_en, &act_name_table_eu_en},
    {&dialog_table_eu_fr, &course_name_table_eu_fr, &act_name_table_eu_fr},
    {&dialog_table_eu_de, &course_name_table_eu_de, &act_name_table_eu_de},
#endif
};

#ifdef REONUCAM
u8 textCamInfoSlowest[] = { TEXT_CAM_INFO_SLOWEST };
u8 textCamInfoSlow[] = { TEXT_CAM_INFO_SLOW };
u8 textCamInfoMedium[] = { TEXT_CAM_INFO_MEDIUM };
u8 textCamInfoFast[] = { TEXT_CAM_INFO_FAST };
u8 textCamInfoFastest[] = { TEXT_CAM_INFO_FASTEST };
static u8 *const sReonucamSpeedLabels[REONUCAM_SPEED_COUNT] = {
    textCamInfoSlowest, textCamInfoSlow, textCamInfoMedium, textCamInfoFast, textCamInfoFastest,
};
#endif

extern u8 gLastCompletedCourseNum;
extern u8 gLastCompletedStarNum;

enum DialogBoxState {
    DIALOG_STATE_OPENING,
    DIALOG_STATE_VERTICAL,
    DIALOG_STATE_HORIZONTAL,
    DIALOG_STATE_CLOSING
};

enum DialogBoxPageState {
    DIALOG_PAGE_STATE_NONE,
    DIALOG_PAGE_STATE_SCROLL,
    DIALOG_PAGE_STATE_END
};

enum DialogBoxType {
    DIALOG_TYPE_ROTATE, // used in NPCs and level messages
    DIALOG_TYPE_ZOOM    // used in signposts and wall signs and etc
};

#define DEFAULT_DIALOG_BOX_ANGLE 90.0f
#define DEFAULT_DIALOG_BOX_SCALE 19.0f

// Keep positional character widths aligned with the native font table.
// clang-format off
u8 gDialogCharWidths[256] = { // HACKERSM64_DO: Is there a way to auto generate this?
    7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,
    6,  6,  5,  6,  6,  5,  8,  8,  6,  6,  6,  6,  6,  5,  6,  6,
    8,  7,  6,  6,  6,  5,  5,  6,  5,  5,  6,  5,  4,  5,  5,  3,
    7,  5,  5,  5,  6,  5,  5,  5,  5,  5,  7,  7,  5,  5,  4,  4,
    8,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
#if defined(VERSION_JP) || defined(VERSION_SH)
    8,  8,  8,  8,  7,  7,  6,  7,  7,  0,  0,  0,  0,  0,  0,  0,
#else
    8,  8,  8,  8,  7,  7,  6,  7,  7,  0,  0,  0,  0,  0,  0,  6,
#endif
#ifdef VERSION_EU
    6,  6,  6,  0,  6,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0,  4,
    5,  5,  5,  5,  6,  6,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0,
    5,  5,  5,  0,  6,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  5,  5,  0,  0,  6,  6,  0,  0,  0,  0,  0,  0,  0,  5,  6,
    0,  4,  4,  0,  0,  5,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,
#else
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  6,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
#endif
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,
#ifdef VERSION_EU
    7,  5, 10,  5,  9,  8,  4,  0,  0,  0,  0,  5,  5,  6,  5,  4,
#else
    7,  5, 10,  5,  9,  8,  4,  0,  0,  0,  0,  0,  0,  0,  0,  4,
#endif
    0,  0,  5,  7,  7,  6,  6,  8,  0,  8, 10,  6,  4, 10,  0,  0
};
// clang-format on

s8 gDialogBoxState = DIALOG_STATE_OPENING;
f32 gDialogBoxOpenTimer = DEFAULT_DIALOG_BOX_ANGLE;
f32 gDialogBoxScale = DEFAULT_DIALOG_BOX_SCALE;
s16 gDialogScrollOffsetY = 0;
s8 gDialogBoxType = DIALOG_TYPE_ROTATE;
s16 gDialogID = DIALOG_NONE;
static enum MarioTextStyle sDialogTextStyle = MARIO_TEXT_STANDARD;
s16 gLastDialogPageStrPos = 0;
s16 gDialogTextPos = 0;
static u8 sDialogScrollingBack = FALSE;
s8 gDialogLineNum = 1;
s8 gLastDialogResponse = 0;
u8 gMenuHoldKeyIndex = 0;
static u8 sMenuNavigationStick;
static u8 sMenuNavigationDirection;
s32 gDialogResponse = DIALOG_RESPONSE_NONE;

void create_dl_identity_matrix(void) {
    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));

    if (matrix == NULL) {
        return;
    }

#ifndef GBI_FLOATS
    matrix->m[0][0] = 0x00010000;    matrix->m[1][0] = 0x00000000;    matrix->m[2][0] = 0x00000000;    matrix->m[3][0] = 0x00000000;
    matrix->m[0][1] = 0x00000000;    matrix->m[1][1] = 0x00010000;    matrix->m[2][1] = 0x00000000;    matrix->m[3][1] = 0x00000000;
    matrix->m[0][2] = 0x00000001;    matrix->m[1][2] = 0x00000000;    matrix->m[2][2] = 0x00000000;    matrix->m[3][2] = 0x00000000;
    matrix->m[0][3] = 0x00000000;    matrix->m[1][3] = 0x00000001;    matrix->m[2][3] = 0x00000000;    matrix->m[3][3] = 0x00000000;
#else
    guMtxIdent(matrix);
#endif

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
}

void create_dl_translation_matrix(s8 pushOp, f32 x, f32 y, f32 z) {
    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));

    if (matrix == NULL) {
        return;
    }

    guTranslate(matrix, x, y, z);

    if (pushOp == MENU_MTX_PUSH) {
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    }

    if (pushOp == MENU_MTX_NOPUSH) {
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    }
}

void create_dl_rotation_matrix(s8 pushOp, f32 a, f32 x, f32 y, f32 z) {
    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));

    if (matrix == NULL) {
        return;
    }

    guRotate(matrix, a, x, y, z);

    if (pushOp == MENU_MTX_PUSH) {
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    }

    if (pushOp == MENU_MTX_NOPUSH) {
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    }
}

void create_dl_scale_matrix(s8 pushOp, f32 x, f32 y, f32 z) {
    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));

    if (matrix == NULL) {
        return;
    }

    guScale(matrix, x, y, z);

    if (pushOp == MENU_MTX_PUSH) {
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    }

    if (pushOp == MENU_MTX_NOPUSH) {
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    }
}

void create_dl_ortho_matrix(void) {
    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));

    if (matrix == NULL) {
        return;
    }

    create_dl_identity_matrix();

    guOrtho(matrix, 0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -10.0f, 10.0f, 1.0f);

    // Should produce G_RDPHALF_1 in Fast3D
    gSPPerspNormalize(gDisplayListHead++, 0xFFFF);

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
}

// Unused
#if FALSE
static u8 *alloc_ia8_text_from_i1(u16 *in, s16 width, s16 height) {
    s32 inPos;
    u16 bitMask;
    u8 *out;
    s16 outPos = 0;

    out = (u8 *) alloc_display_list((u32) width * (u32) height);

    if (out == NULL) {
        return NULL;
    }

    for (inPos = 0; inPos < (width * height) / 16; inPos++) {
        bitMask = 0x8000;

        while (bitMask != 0) {
            if (in[inPos] & bitMask) {
                out[outPos] = 0xFF;
            } else {
                out[outPos] = 0x00;
            }

            bitMask /= 2;
            outPos++;
        }
    }

    return out;
}

void render_generic_char(u8 c) {
    void **fontLUT = segmented_to_virtual(main_font_lut);
    void *packedTexture = segmented_to_virtual(fontLUT[c]);
#if defined(VERSION_JP) || defined(VERSION_SH)
    void *unpackedTexture = alloc_ia8_text_from_i1(packedTexture, 8, 16);
#endif

#ifndef VERSION_EU
    gDPPipeSync(gDisplayListHead++);
#endif
#if defined(VERSION_JP) || defined(VERSION_SH)
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_8b, 1, VIRTUAL_TO_PHYSICAL(unpackedTexture));
#else
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_16b, 1, VIRTUAL_TO_PHYSICAL(packedTexture));
#endif
    gSPDisplayList(gDisplayListHead++, dl_ia_text_tex_settings);
#ifdef VERSION_EU
    gSPTextureRectangleFlip(gDisplayListHead++, gDialogX << 2, (gDialogY - 16) << 2,
                            (gDialogX + 8) << 2, gDialogY << 2, G_TX_RENDERTILE, 8 << 6, 4 << 6, 1 << 10, 1 << 10);
#endif
}
#endif

Texture32 *alloc_ia4_tex_from_i1(Texture *in, s16 width, s16 height) {
    u32 size = (u32) width * (u32) height;
    s32 inPos;
    s16 outPos = 0;
    u8 bitMask;

    Texture *out = (Texture *) alloc_display_list(size);

    if (out == NULL) {
        return NULL;
    }

    for (inPos = 0; inPos < (width * height) / 4; inPos++) {
        bitMask = 0x80;

        while (bitMask != 0) {
            out[outPos] = (in[inPos] & bitMask) ? 0xF0 : 0x00;
            bitMask /= 2;
            out[outPos] = (in[inPos] & bitMask) ? out[outPos] + 0x0F : out[outPos];
            bitMask /= 2;
            outPos++;
        }
    }

    return (Texture32 *)out;
}

void render_generic_char(u8 c) {
    void **fontLUT = segmented_to_virtual(main_font_lut);
    void *packedTexture = segmented_to_virtual(fontLUT[c]);
#if MULTILANG
    void *unpackedTexture = alloc_ia4_tex_from_i1(packedTexture, 8, 8);

    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_16b, 1, VIRTUAL_TO_PHYSICAL(unpackedTexture));
#else
    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_16b, 1, VIRTUAL_TO_PHYSICAL(packedTexture));
#endif

    gSPDisplayList(gDisplayListHead++, dl_ia_text_tex_settings);
}


struct MultiTextEntry {
    u8 length;
    u8 str[4];
};

#define TEXT_THE_RAW ASCII_TO_DIALOG('t'), ASCII_TO_DIALOG('h'), ASCII_TO_DIALOG('e'), 0x00
#define TEXT_YOU_RAW ASCII_TO_DIALOG('y'), ASCII_TO_DIALOG('o'), ASCII_TO_DIALOG('u'), 0x00

enum MultiStringIDs { STRING_THE, STRING_YOU };

/*
 * Place the multi-text string according to the ID passed. (US, EU)
 * 0: 'the'
 * 1: 'you'
 */
void render_multi_text_string(s8 multiTextID) {
    s8 i;
    struct MultiTextEntry textLengths[2] = {
        { 3, { TEXT_THE_RAW } },
        { 3, { TEXT_YOU_RAW } },
    };

    for (i = 0; i < textLengths[multiTextID].length; i++) {
        render_generic_char(textLengths[multiTextID].str[i]);
        create_dl_translation_matrix(MENU_MTX_NOPUSH, (f32)(gDialogCharWidths[textLengths[multiTextID].str[i]]), 0.0f, 0.0f);
    }
}

#define MAX_STRING_WIDTH 16
#define CHAR_WIDTH_SPACE (f32)(gDialogCharWidths[DIALOG_CHAR_SPACE])

/**
 * Prints a generic white string.
 * In JP/EU a IA1 texture is used but in US a IA4 texture is used.
 */
static void print_generic_string_styled(s16 x, s16 y, const u8 *str, enum MarioTextStyle style) {
    u8 namedText[mario_character_text_name_size(style, MARIO_TEXT_DIALOG, str)];
    str = (u8 *) mario_character_text_replace_name(style, MARIO_TEXT_DIALOG, str, namedText);
    u8 previous = 0;
    u8 glyph;
    s8 mark = DIALOG_MARK_NONE; // unused in EU
    s32 strPos = 0;
    u8 lineNum = 1;

    s16 colorLoop;
    ColorRGBA rgbaColors = { 0x00, 0x00, 0x00, 0x00 };
    u8 customColor = 0;
    u8 diffTmp     = 0;

    create_dl_translation_matrix(MENU_MTX_PUSH, x, y, 0.0f);

    while (str[strPos] != DIALOG_CHAR_TERMINATOR) {
        glyph = mario_character_text_glyph(style, MARIO_TEXT_DIALOG, &previous, str[strPos],
                                           str + strPos + 1);
        switch (str[strPos]) {
            case DIALOG_CHAR_COLOR:
                customColor = 1;
                strPos++;
                for (colorLoop = (strPos + 8); strPos < colorLoop; ++strPos) {
                    diffTmp = 0;
                    if ((str[strPos] >= 0x24)
                     && (str[strPos] <= 0x29)) {
                        diffTmp = 0x1A;
                    } else if (str[strPos] >= 0x10) {
                        customColor = 2;
                        strPos = (colorLoop - 8);
                        for (diffTmp = 0; diffTmp < 8; ++diffTmp) {
                            if (str[strPos + diffTmp] != 0x9F) {
                                break;
                            }
                        }
                        if (diffTmp == 8) {
                            strPos += diffTmp;
                        }
                        break;
                    }
                    if (((8 - (colorLoop - strPos)) % 2) == 0) {
                        rgbaColors[(8 - (colorLoop - strPos)) / 2] = (((str[strPos] - diffTmp) & 0x0F) << 4);
                    } else {
                        rgbaColors[(8 - (colorLoop - strPos)) / 2] += ((str[strPos] - diffTmp) & 0x0F);
                    }
                }
                strPos--;
                if (customColor == 1) {
                    gDPSetEnvColor(gDisplayListHead++, rgbaColors[0],
                                                    rgbaColors[1],
                                                    rgbaColors[2],
                                                    rgbaColors[3]);
                } else if (customColor == 2) {
                    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255); // HACKERSM64_DO: Is it possible to retrieve the original color that was set before print_generic_string was called?
                    customColor = 0;
                }
                break;
            case DIALOG_CHAR_DAKUTEN:
                mark = DIALOG_MARK_DAKUTEN;
                break;
            case DIALOG_CHAR_PERIOD_OR_HANDAKUTEN:
                mark = DIALOG_MARK_HANDAKUTEN;
                break;
            case DIALOG_CHAR_NEWLINE:
                gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
                create_dl_translation_matrix(MENU_MTX_PUSH, x, y - (lineNum * MAX_STRING_WIDTH), 0.0f);
                lineNum++;
                break;
            case DIALOG_CHAR_PERIOD:
                create_dl_translation_matrix(MENU_MTX_PUSH, -2.0f, -5.0f, 0.0f);
                render_generic_char(DIALOG_CHAR_PERIOD_OR_HANDAKUTEN);
                gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
                break;
            case DIALOG_CHAR_SLASH:
                create_dl_translation_matrix(MENU_MTX_NOPUSH, (f32)(gDialogCharWidths[DIALOG_CHAR_SPACE] * 2), 0.0f, 0.0f);
                break;
            case DIALOG_CHAR_MULTI_THE:
                render_multi_text_string(STRING_THE);
                break;
            case DIALOG_CHAR_MULTI_YOU:
                render_multi_text_string(STRING_YOU);
                break;
            case DIALOG_CHAR_SPACE:
                // create_dl_translation_matrix(MENU_MTX_NOPUSH, (f32)(gDialogCharWidths[DIALOG_CHAR_SPACE]), 0.0f, 0.0f);
                create_dl_translation_matrix(MENU_MTX_NOPUSH, CHAR_WIDTH_SPACE, 0.0f, 0.0f);
                break;
            default:
                if (glyph == MARIO_TEXT_SKIP) {
                    break;
                }
                render_generic_char(glyph);
                if (mark != DIALOG_MARK_NONE) {
                    create_dl_translation_matrix(MENU_MTX_PUSH, 5.0f, 5.0f, 0.0f);
                    render_generic_char(DIALOG_CHAR_MARK_START + mark);
                    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
                    mark = DIALOG_MARK_NONE;
                }

                create_dl_translation_matrix(MENU_MTX_NOPUSH, (f32) gDialogCharWidths[glyph], 0.0f,
                                             0.0f);
                break;
        }

        strPos++;
    }

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void print_generic_string(s16 x, s16 y, const u8 *str) {
    print_generic_string_styled(x, y, str, mario_character_text_style());
}

/**
 * Prints a hud string depending of the hud table list defined.
 */
void print_hud_lut_string(s8 hudLUT, s16 x, s16 y, const u8 *str) {
    u8 previous = 0;
    u8 glyph;
    enum MarioTextStyle style = mario_character_text_style();
    u8 namedText[mario_character_text_name_size(
        style, hudLUT == HUD_LUT_GLOBAL ? MARIO_TEXT_HUD : MARIO_TEXT_LITERAL, str)];
    str = (u8 *) mario_character_text_replace_name(
        style, hudLUT == HUD_LUT_GLOBAL ? MARIO_TEXT_HUD : MARIO_TEXT_LITERAL, str, namedText);
    s32 strPos = 0;
    void **hudLUT1 = segmented_to_virtual(menu_hud_lut); // Japanese Menu HUD Color font
    void **hudLUT2 = segmented_to_virtual(main_hud_lut); // 0-9 A-Z HUD Color Font
    u32 curX = x;
    u32 curY = y;

    u32 xStride; // X separation

    if (hudLUT == HUD_LUT_JPMENU) {
        xStride = 16;
    } else { // HUD_LUT_GLOBAL
        xStride = 12; //? Shindou uses this.
    }

    while (str[strPos] != GLOBAR_CHAR_TERMINATOR) {
        glyph = mario_character_text_glyph(
            style, hudLUT == HUD_LUT_GLOBAL ? MARIO_TEXT_HUD : MARIO_TEXT_LITERAL, &previous,
            str[strPos], str + strPos + 1);
        switch (str[strPos]) {
            case GLOBAL_CHAR_SPACE:
                curX += 8;
                break;
            default:
                if (glyph == MARIO_TEXT_SKIP) {
                    break;
                }
                gDPPipeSync(gDisplayListHead++);

                if (hudLUT == HUD_LUT_JPMENU) {
                    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1,
                                       hudLUT1[glyph]);
                }

                if (hudLUT == HUD_LUT_GLOBAL) {
                    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1,
                                       hudLUT2[glyph]);
                }

                gSPDisplayList(gDisplayListHead++, dl_rgba16_load_tex_block);
                gSPTextureRectangle(gDisplayListHead++, curX << 2, curY << 2, (curX + 16) << 2,
                                    (curY + 16) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

                curX += xStride;
        }
        strPos++;
    }
}


void print_menu_generic_string(s16 x, s16 y, const u8 *str) {
    u8 previous = 0;
    u8 glyph;
    enum MarioTextStyle style = mario_character_text_style();
    u8 namedText[mario_character_text_name_size(style, MARIO_TEXT_DIALOG, str)];
    str = (u8 *) mario_character_text_replace_name(style, MARIO_TEXT_DIALOG, str, namedText);
    UNUSED s8 mark = DIALOG_MARK_NONE; // unused in EU
    s32 strPos = 0;
    u32 curX = x;
    u32 curY = y;
    void **fontLUT = segmented_to_virtual(menu_font_lut);

    while (str[strPos] != DIALOG_CHAR_TERMINATOR) {
        glyph = mario_character_text_glyph(style, MARIO_TEXT_DIALOG, &previous, str[strPos],
                                           str + strPos + 1);
        switch (str[strPos]) {
            case DIALOG_CHAR_DAKUTEN:
                mark = DIALOG_MARK_DAKUTEN;
                break;
            case DIALOG_CHAR_PERIOD_OR_HANDAKUTEN:
                mark = DIALOG_MARK_HANDAKUTEN;
                break;
            case DIALOG_CHAR_SPACE:
                curX += 4;
                break;
            default:
                if (glyph == MARIO_TEXT_SKIP) {
                    break;
                }
                gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_8b, 1, fontLUT[glyph]);
                gDPLoadSync(gDisplayListHead++);
                gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 8 * 8 - 1, CALC_DXT(8, G_IM_SIZ_8b_BYTES));
                gSPTextureRectangle(gDisplayListHead++, curX << 2, curY << 2, (curX + 8) << 2,
                                    (curY + 8) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

                if (mark != DIALOG_MARK_NONE) {
                    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_IA, G_IM_SIZ_8b, 1, fontLUT[DIALOG_CHAR_MARK_START + mark]);
                    gDPLoadSync(gDisplayListHead++);
                    gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 8 * 8 - 1, CALC_DXT(8, G_IM_SIZ_8b_BYTES));
                    gSPTextureRectangle(gDisplayListHead++, (curX + 6) << 2, (curY - 7) << 2,
                                        (curX + 6 + 8) << 2, (curY - 7 + 8) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

                    mark = DIALOG_MARK_NONE;
                }
                curX += gDialogCharWidths[glyph];
        }
        strPos++;
    }
}

void print_credits_string(s16 x, s16 y, const u8 *str) {
    u8 previous = 0;
    u8 glyph;
    enum MarioTextStyle style = mario_character_text_style();
    u8 namedText[mario_character_text_name_size(style, MARIO_TEXT_HUD, str)];
    str = (u8 *) mario_character_text_replace_name(style, MARIO_TEXT_HUD, str, namedText);
    s32 strPos = 0;
    void **fontLUT = segmented_to_virtual(main_credits_font_lut);
    u32 curX = x;
    u32 curY = y;

    gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0,
                G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD);
    gDPTileSync(gDisplayListHead++);
    gDPSetTile(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 2, 0, G_TX_RENDERTILE, 0,
                G_TX_CLAMP, 3, G_TX_NOLOD, G_TX_CLAMP, 3, G_TX_NOLOD);
    gDPSetTileSize(gDisplayListHead++, G_TX_RENDERTILE, 0, 0, (8 - 1) << G_TEXTURE_IMAGE_FRAC, (8 - 1) << G_TEXTURE_IMAGE_FRAC);

    while (str[strPos] != GLOBAR_CHAR_TERMINATOR) {
        glyph =
            mario_character_text_glyph(style, MARIO_TEXT_HUD, &previous, str[strPos], str + strPos + 1);
        switch (str[strPos]) {
            case GLOBAL_CHAR_SPACE:
                curX += 4;
                break;
            default:
                if (glyph == MARIO_TEXT_SKIP) {
                    break;
                }
                gDPPipeSync(gDisplayListHead++);
                gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fontLUT[glyph]);
                gDPLoadSync(gDisplayListHead++);
                gDPLoadBlock(gDisplayListHead++, G_TX_LOADTILE, 0, 0, 8 * 8 - 1, CALC_DXT(8, G_IM_SIZ_16b_BYTES));
                gSPTextureRectangle(gDisplayListHead++, curX << 2, curY << 2, (curX + 8) << 2,
                                    (curY + 8) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                curX += 7;
                break;
        }
        strPos++;
    }
}

u8 menu_scrolling_input(u8 index, u8 *heldIndex, u8 *timer) {
    u8 pressed = (index ^ *heldIndex) & index;
    if (index == 0) {
        *timer = 0;
    } else if (index != *heldIndex) {
        *timer = MENU_SCROLL_REPEAT_DELAY_FRAMES;
    } else {
        if (*timer > 0) {
            (*timer)--;
        }
        if (*timer == 0) {
            pressed = index;
            *timer = MENU_SCROLL_REPEAT_INTERVAL_FRAMES;
        }
    }
    *heldIndex = index;
    return pressed;
}

void handle_menu_scrolling(s8 scrollDirection, s8 *currentIndex, s8 minIndex, s8 maxIndex) {
    u8 index = 0;
    u16 dpad = game_options_menu_dpad(gPlayer1Controller->buttonDown);
    u8 stick =
        (gPlayer1Controller->rawStickY > 60 ? 1 : 0) | (gPlayer1Controller->rawStickY < -60 ? 2 : 0)
        | (gPlayer1Controller->rawStickX < -60 ? 4 : 0) | (gPlayer1Controller->rawStickX > 60 ? 8 : 0);
    u8 allowed = game_options_menu_direction(
        (game_options_get(GO_MENU_NAVIGATION) == 1 ? 0 : stick) | ((dpad & U_JPAD) ? 1 : 0)
            | ((dpad & D_JPAD) ? 2 : 0) | ((dpad & L_JPAD) ? 4 : 0) | ((dpad & R_JPAD) ? 8 : 0),
        &sMenuNavigationDirection);
    stick = game_options_menu_stick(stick, &sMenuNavigationStick) & allowed;
    if (!allowed) {
        dpad = 0;
    }
    u32 scrollSound = gMenuMode == MENU_MODE_RENDER_PAUSE_SCREEN ? SOUND_MENU_PAUSE_CHANGE_SELECT
                                                                 : SOUND_MENU_CHANGE_SELECT;

    if (scrollDirection == MENU_SCROLL_VERTICAL || scrollDirection == MENU_SCROLL_VERTICAL_WRAP) {
        index = (stick & 3) | ((dpad & U_JPAD) ? 1 : 0) | ((dpad & D_JPAD) ? 2 : 0);
    } else if (scrollDirection == MENU_SCROLL_HORIZONTAL
               || scrollDirection == MENU_SCROLL_HORIZONTAL_DPAD) {
        // Z reserves horizontal D-pad input for camera speed.
        if (gMenuMode == MENU_MODE_RENDER_PAUSE_SCREEN && (gPlayer1Controller->buttonDown & Z_TRIG)) {
            dpad = 0;
        }
        index = ((stick & 4) ? 1 : 0) | ((stick & 8) ? 2 : 0) | ((dpad & L_JPAD) ? 1 : 0)
                | ((dpad & R_JPAD) ? 2 : 0);
    }

    u8 pressed = (index ^ gMenuHoldKeyIndex) & index;
    gMenuHoldKeyIndex = index;
    if (pressed == 2) {
        if (*currentIndex != maxIndex || scrollDirection == MENU_SCROLL_VERTICAL_WRAP) {
            play_sound(scrollSound, gGlobalSoundSource);
            *currentIndex = *currentIndex == maxIndex ? minIndex : *currentIndex + 1;
        }
    }

    if (pressed == 1) {
        if (*currentIndex != minIndex || scrollDirection == MENU_SCROLL_VERTICAL_WRAP) {
            play_sound(scrollSound, gGlobalSoundSource);
            *currentIndex = *currentIndex == minIndex ? maxIndex : *currentIndex - 1;
        }
    }
}

s32 get_str_x_pos_from_center(s16 centerPos, u8 *str, UNUSED f32 scale) {
    u8 previous = 0;
    u8 colorBytes = 0;
    enum MarioTextStyle style = mario_character_text_style();
    u8 namedText[mario_character_text_name_size(style, MARIO_TEXT_DIALOG, str)];
    str = (u8 *) mario_character_text_replace_name(style, MARIO_TEXT_DIALOG, str, namedText);
    s16 strPos = 0;
    f32 spacesWidth = 0.0f;

    while (str[strPos] != DIALOG_CHAR_TERMINATOR) {
        u8 glyph = str[strPos];
        // Preserve the existing width calculation for control bytes, but never
        // let colour payloads participate in the spelling rule.
        if (colorBytes != 0) {
            colorBytes--;
        } else {
            glyph = mario_character_text_glyph(style, MARIO_TEXT_DIALOG, &previous, glyph,
                                               str + strPos + 1);
            if (str[strPos] == DIALOG_CHAR_COLOR) {
                colorBytes = 8;
            }
        }
        if (glyph != MARIO_TEXT_SKIP) {
            spacesWidth += gDialogCharWidths[glyph];
        }
        strPos++;
    }
    // return the x position of where the string starts as half the string's
    // length from the position of the provided center.
    return (s16)(centerPos - (s16)(spacesWidth / 2.0f));
}

s32 get_string_width(u8 *str) {
    u8 previous = 0;
    u8 colorBytes = 0;
    enum MarioTextStyle style = mario_character_text_style();
    u8 namedText[mario_character_text_name_size(style, MARIO_TEXT_DIALOG, str)];
    str = (u8 *) mario_character_text_replace_name(style, MARIO_TEXT_DIALOG, str, namedText);
    s16 strPos = 0;
    s16 width = 0;

    while (str[strPos] != DIALOG_CHAR_TERMINATOR) {
        u8 glyph = str[strPos];
        // Preserve the existing width calculation for control bytes, but never
        // let colour payloads participate in the spelling rule.
        if (colorBytes != 0) {
            colorBytes--;
        } else {
            glyph = mario_character_text_glyph(style, MARIO_TEXT_DIALOG, &previous, glyph,
                                               str + strPos + 1);
            if (str[strPos] == DIALOG_CHAR_COLOR) {
                colorBytes = 8;
            }
        }
        if (glyph != MARIO_TEXT_SKIP) {
            width += gDialogCharWidths[glyph];
        }
        strPos++;
    }
    return width;
}

u8 gHudSymCoin[] = { GLYPH_COIN, GLYPH_SPACE };
u8 gHudSymX[] = { GLYPH_MULTIPLY, GLYPH_SPACE };

void print_hud_my_score_coins(s32 useCourseCoinScore, s8 fileIndex, s8 courseIndex, s16 x, s16 y) {
    u8 strNumCoins[4];
    s16 numCoins;

    if (!useCourseCoinScore) {
        numCoins = (u16)(save_file_get_max_coin_score(courseIndex) & 0xFFFF);
    } else {
        numCoins = save_file_get_course_coin_score(fileIndex, courseIndex);
    }

    if (numCoins != 0) {
        print_hud_lut_string(HUD_LUT_GLOBAL, x +  0, y, gHudSymCoin);
        print_hud_lut_string(HUD_LUT_GLOBAL, x + 16, y, gHudSymX);
        int_to_str(numCoins, strNumCoins);
        print_hud_lut_string(HUD_LUT_GLOBAL, x + 32, y, strNumCoins);
    }
}

void print_hud_my_score_stars(s8 fileIndex, s8 courseIndex, s16 x, s16 y) {
    u8 strStarCount[4];
    u8 textSymStar[] = { GLYPH_STAR, GLYPH_SPACE };
    u8 textSymX[] = { GLYPH_MULTIPLY, GLYPH_SPACE };

    s16 starCount = save_file_get_course_star_count(fileIndex, courseIndex);

    if (starCount != 0) {
        print_hud_lut_string(HUD_LUT_GLOBAL, x +  0, y, textSymStar);
        print_hud_lut_string(HUD_LUT_GLOBAL, x + 16, y, textSymX);
        int_to_str(starCount, strStarCount);
        print_hud_lut_string(HUD_LUT_GLOBAL, x + 32, y, strStarCount);
    }
}

void int_to_str(s32 num, u8 *dst) {
    s32 digit[3];

    s8 pos = 0;

    if (num > 999) {
        dst[0] = 0x00;
        dst[1] = DIALOG_CHAR_TERMINATOR;
        return;
    }

    digit[0] = (num / 100);
    digit[1] = ((num - (digit[0] * 100)) / 10);
    digit[2] = ((num - (digit[0] * 100)) - (digit[1] * 10));

    if (digit[0] != 0) {
        dst[pos++] = digit[0];
    }

    if ((digit[1] != 0)
     || (digit[0] != 0)) {
        dst[pos++] = digit[1];
    }

    dst[pos++] = digit[2];
    dst[pos] = DIALOG_CHAR_TERMINATOR;
}

s32 get_dialog_id(void) {
    return gDialogID;
}

static void reset_menu_navigation(void) {
    // Neutral input during gameplay must not leave the next menu blocked.
    sMenuNavigationStick = 0;
    sMenuNavigationDirection = 0;
    gMenuHoldKeyIndex = 0;
}

void create_dialog_box(s16 dialog) {
    if (gDialogID == DIALOG_NONE) {
        reset_menu_navigation();
        gDialogID = dialog;
        sDialogTextStyle = mario_character_text_style();
        gDialogBoxType = DIALOG_TYPE_ROTATE;
    }
}

void create_dialog_box_with_var(s16 dialog, s32 dialogVar) {
    if (gDialogID == DIALOG_NONE) {
        reset_menu_navigation();
        gDialogID = dialog;
        sDialogTextStyle = mario_character_text_style();
        gDialogVariable = dialogVar;
        gDialogBoxType = DIALOG_TYPE_ROTATE;
    }
}

void create_dialog_inverted_box(s16 dialog) {
    if (gDialogID == DIALOG_NONE) {
        reset_menu_navigation();
        gDialogID = dialog;
        sDialogTextStyle = mario_character_text_style();
        gDialogBoxType = DIALOG_TYPE_ZOOM;
    }
}

void create_dialog_box_with_response(s16 dialog) {
    if (gDialogID == DIALOG_NONE) {
        reset_menu_navigation();
        gDialogID = dialog;
        sDialogTextStyle = mario_character_text_style();
        gDialogBoxType = DIALOG_TYPE_ROTATE;
        gLastDialogResponse = 1;
    }
}

void reset_dialog_render_state(void) {
    reset_menu_navigation();
    level_set_transition(0, NULL);

    gDialogBoxScale = 19.0f;
    gDialogBoxOpenTimer = 90.0f;
    gDialogBoxState = DIALOG_STATE_OPENING;
    gDialogID = DIALOG_NONE;
    sDialogTextStyle = MARIO_TEXT_STANDARD;
    gDialogTextPos = 0;
    gLastDialogResponse = 0;
    gLastDialogPageStrPos = 0;
    gDialogResponse = DIALOG_RESPONSE_NONE;
}

#ifdef INPUT_DISPLAY
static f32 dialog_input_display_scale(s16 textWidth) {
    return input_display_enabled()
               ? MIN(1.0f, (SCREEN_WIDTH - INPUT_DISPLAY_CONTENT_LEFT - 8.0f) / (textWidth + 4.0f))
               : 1.0f;
}
#endif

void render_dialog_box_type(struct DialogEntry *dialog, s8 linesPerBox, s16 textWidth) {
    create_dl_translation_matrix(MENU_MTX_NOPUSH, dialog->leftOffset, dialog->width, 0);
#ifdef INPUT_DISPLAY
    f32 horizontalScale = dialog_input_display_scale(textWidth);
    if (horizontalScale < 1.0f) {
        // Keep authored line/page breaks, including intentionally long two-line dialogue.
        create_dl_scale_matrix(MENU_MTX_NOPUSH, horizontalScale, 1.0f, 1.0f);
    }
#endif

    switch (gDialogBoxType) {
        case DIALOG_TYPE_ROTATE: // Renders a dialog black box with zoom and rotation
            if ((gDialogBoxState == DIALOG_STATE_OPENING)
                || (gDialogBoxState == DIALOG_STATE_CLOSING)) {
                create_dl_scale_matrix(MENU_MTX_NOPUSH, (1.0f / gDialogBoxScale),
                                       (1.0f / gDialogBoxScale), 1.0f);
                // convert the speed into angle
                create_dl_rotation_matrix(MENU_MTX_NOPUSH, (gDialogBoxOpenTimer * 4.0f), 0, 0, 1.0f);
            }
            gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 150);
            break;
        case DIALOG_TYPE_ZOOM: // Renders a dialog white box with zoom
            if (gDialogBoxState == DIALOG_STATE_OPENING || gDialogBoxState == DIALOG_STATE_CLOSING) {
                create_dl_translation_matrix(
                    MENU_MTX_NOPUSH, ((textWidth - 2.0f) * 0.5f * (1.0f - 1.0f / gDialogBoxScale)),
                    ((40.0f / gDialogBoxScale) - 40), 0);
                create_dl_scale_matrix(MENU_MTX_NOPUSH, (1.0f / gDialogBoxScale),
                                       (1.0f / gDialogBoxScale), 1.0f);
            }
            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 150);
            break;
    }

    create_dl_translation_matrix(MENU_MTX_PUSH, -7.0f, 5.0f, 0);
    create_dl_scale_matrix(MENU_MTX_NOPUSH, (textWidth + 11.0f) / 130.0f,
                           (((f32) linesPerBox / 5.0f) + 0.1f), 1.0f);

    gSPDisplayList(gDisplayListHead++, dl_draw_text_bg_box);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void change_and_flash_dialog_text_color_lines(s8 colorMode, s8 lineNum, u8 *customColor) {
    u8 colorFade;

    if (colorMode == 1) {
        if (lineNum == 1) {
            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
        } else {
            if (lineNum == gDialogLineNum) {
                colorFade = (sins(gDialogColorFadeTimer) * 50.0f) + 200.0f;
                gDPSetEnvColor(gDisplayListHead++, colorFade, colorFade, colorFade, 255);
            } else {
                gDPSetEnvColor(gDisplayListHead++, 200, 200, 200, 255);
            }
        }
    } else {
        switch (gDialogBoxType) {
            case DIALOG_TYPE_ROTATE:
                if (*customColor == 2) {
                    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
                    *customColor = 0;
                }
                break;
            case DIALOG_TYPE_ZOOM:
                gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
                break;
        }
    }
}

#define X_VAL3 0.0f
#define Y_VAL3 16

void handle_dialog_scroll_page_state(s8 lineNum, s8 totalLines, s8 *pageState, s8 *xMatrix, s16 *linePos) {
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    if (lineNum == totalLines) {
        *pageState = DIALOG_PAGE_STATE_SCROLL;
        return;
    }
    create_dl_translation_matrix(MENU_MTX_PUSH, 0.0f, 2 - (lineNum * 16), 0);

    *linePos = 0;
    *xMatrix = 1;
}

void render_star_count_dialog_text(s8 *xMatrix, s16 *linePos) {
    s8 tensDigit = gDialogVariable / 10;
    s8 onesDigit = gDialogVariable - (tensDigit * 10); // remainder

    if (tensDigit != 0) {
        if (*xMatrix != 1) {
            create_dl_translation_matrix(MENU_MTX_NOPUSH, (f32)(gDialogCharWidths[DIALOG_CHAR_SPACE] * *xMatrix), 0, 0);
        }

        render_generic_char(tensDigit);
        create_dl_translation_matrix(MENU_MTX_NOPUSH, (f32) gDialogCharWidths[tensDigit], 0, 0);
        *xMatrix = 1;
        (*linePos)++;
    }

    if (*xMatrix != 1) {
        create_dl_translation_matrix(MENU_MTX_NOPUSH, (f32)(gDialogCharWidths[DIALOG_CHAR_SPACE] * (*xMatrix - 1)), 0, 0);
    }

    render_generic_char(onesDigit);
    create_dl_translation_matrix(MENU_MTX_NOPUSH, (f32) gDialogCharWidths[onesDigit], 0, 0);
    (*linePos)++;
    *xMatrix = 1;
}

void render_multi_text_string_lines(s8 multiTextId, s8 lineNum, s16 *linePos, s8 linesPerBox, s8 xMatrix, s8 lowerBound) {
    s8 i;
    struct MultiTextEntry textLengths[2] = {
        { 3, { TEXT_THE_RAW } },
        { 3, { TEXT_YOU_RAW } },
    };

    if (lineNum >= lowerBound && lineNum <= (lowerBound + linesPerBox)) {
        if (*linePos != 0 || xMatrix != 1) {
            create_dl_translation_matrix(MENU_MTX_NOPUSH, (gDialogCharWidths[DIALOG_CHAR_SPACE] * (xMatrix - 1)), 0, 0);
        }
        for (i = 0; i < textLengths[multiTextId].length; i++) {
            render_generic_char(textLengths[multiTextId].str[i]);
            create_dl_translation_matrix(MENU_MTX_NOPUSH, (gDialogCharWidths[textLengths[multiTextId].str[i]]), 0, 0);
        }
    }
    linePos += textLengths[multiTextId].length;
}

u32 ensure_nonnegative(s16 value) {
    return ((value < 0) ? 0 : value);
}

void handle_dialog_text_and_pages(s8 colorMode, struct DialogEntry *dialog, s8 lowerBound,
                                  enum MarioTextStyle style) {
    u8 previous = 0;
    u8 glyph;
    u8 strChar;
    s16 colorLoop;
    ColorRGBA rgbaColors = { 0x00, 0x00, 0x00, 0x00 };
    u8 customColor = 0;
    u8 diffTmp = 0;
    u8 *str = segmented_to_virtual(dialog->str);
    u8 namedText[mario_character_text_name_size(style, MARIO_TEXT_DIALOG, str)];
    str = (u8 *) mario_character_text_replace_name(style, MARIO_TEXT_DIALOG, str, namedText);
    s8 lineNum = 1;
    s8 totalLines;
    s8 pageState = DIALOG_PAGE_STATE_NONE;
    UNUSED s8 mark = DIALOG_MARK_NONE; // unused in US and EU
    s8 xMatrix = 1;
    s8 linesPerBox = dialog->linesPerBox;
    s16 strIdx;
    s16 linePos = 0;

    if (gDialogBoxState == DIALOG_STATE_HORIZONTAL) {
        // If scrolling, consider the number of lines for both
        // the current page and the page being scrolled to.
        totalLines = linesPerBox * 2 + 1;
    } else {
        totalLines = linesPerBox + 1;
    }

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    strIdx = gDialogTextPos;

    if (gDialogBoxState == DIALOG_STATE_HORIZONTAL) {
        create_dl_translation_matrix(MENU_MTX_NOPUSH, 0, (f32) gDialogScrollOffsetY, 0);
    }

    create_dl_translation_matrix(MENU_MTX_PUSH, X_VAL3, 2 - lineNum * Y_VAL3, 0);

    while (pageState == DIALOG_PAGE_STATE_NONE) {
        if (customColor == 1) {
            gDPSetEnvColor(gDisplayListHead++, rgbaColors[0], rgbaColors[1], rgbaColors[2], rgbaColors[3]);
        } else {
            change_and_flash_dialog_text_color_lines(colorMode, lineNum, &customColor);
        }
        strChar = str[strIdx];
        glyph =
            mario_character_text_glyph(style, MARIO_TEXT_DIALOG, &previous, strChar, str + strIdx + 1);

        switch (strChar) {
            case DIALOG_CHAR_TERMINATOR:
                pageState = DIALOG_PAGE_STATE_END;
                gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
                break;
            case DIALOG_CHAR_COLOR:
                customColor = 1;
                strIdx++;
                for (colorLoop = (strIdx + 8); strIdx < colorLoop; ++strIdx) {
                    diffTmp = 0;
                    if ((str[strIdx] >= 0x24) && (str[strIdx] <= 0x29)) {
                        diffTmp = 0x1A;
                    } else if (str[strIdx] >= 0x10) {
                        customColor = 2;
                        strIdx = (colorLoop - 8);
                        for (diffTmp = 0; diffTmp < 8; ++diffTmp) {
                            if (str[strIdx + diffTmp] != 0x9F) {
                                break;
                            }
                        }
                        if (diffTmp == 8) {
                            strIdx += diffTmp;
                        }
                        break;
                    }
                    if (((8 - (colorLoop - strIdx)) % 2) == 0) {
                        rgbaColors[(8 - (colorLoop - strIdx)) / 2] = (((str[strIdx] - diffTmp) & 0x0F) << 4);
                    } else {
                        rgbaColors[(8 - (colorLoop - strIdx)) / 2] += ((str[strIdx] - diffTmp) & 0x0F);
                    }
                }
                strIdx--;
                break;
            case DIALOG_CHAR_NEWLINE:
                lineNum++;
                handle_dialog_scroll_page_state(lineNum, totalLines, &pageState, &xMatrix, &linePos);
                break;
            case DIALOG_CHAR_DAKUTEN:
                mark = DIALOG_MARK_DAKUTEN;
                break;
            case DIALOG_CHAR_PERIOD_OR_HANDAKUTEN:
                mark = DIALOG_MARK_HANDAKUTEN;
                break;
            case DIALOG_CHAR_SPACE:
                xMatrix++;
                linePos++;
                break;
            case DIALOG_CHAR_SLASH:
                xMatrix += 2;
                linePos += 2;
                break;
            case DIALOG_CHAR_MULTI_THE:
                render_multi_text_string_lines(STRING_THE, lineNum, &linePos, linesPerBox, xMatrix, lowerBound);
                xMatrix = 1;
                break;
            case DIALOG_CHAR_MULTI_YOU:
                render_multi_text_string_lines(STRING_YOU, lineNum, &linePos, linesPerBox, xMatrix, lowerBound);
                xMatrix = 1;
                break;
            case DIALOG_CHAR_STAR_COUNT:
                render_star_count_dialog_text(&xMatrix, &linePos);
                break;
            default: // any other character
                if (glyph == MARIO_TEXT_SKIP) {
                    break;
                }
                if ((lineNum >= lowerBound) && (lineNum <= (lowerBound + linesPerBox))) {
                    if (linePos || xMatrix != 1) {
                        create_dl_translation_matrix(
                            MENU_MTX_NOPUSH, (f32)(gDialogCharWidths[DIALOG_CHAR_SPACE] * (xMatrix - 1)), 0, 0);
                    }

                    render_generic_char(glyph);
                    create_dl_translation_matrix(MENU_MTX_NOPUSH, (f32) (gDialogCharWidths[glyph]), 0,
                                                 0);
                    xMatrix = 1;
                    linePos++;
                }
        }


        strIdx++;
    }

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

    if (gDialogBoxState == DIALOG_STATE_VERTICAL) {
        if (pageState == DIALOG_PAGE_STATE_END) {
            gLastDialogPageStrPos = -1;
        } else {
            gLastDialogPageStrPos = strIdx;
        }
    }

    gLastDialogLineNum = lineNum;
}

#define X_VAL4_1 56
#define X_VAL4_2 47
#define Y_VAL4_1  2
#define Y_VAL4_2 16

void render_dialog_triangle_choice(void) {
    if (gDialogBoxState == DIALOG_STATE_VERTICAL) {
        handle_menu_scrolling(MENU_SCROLL_HORIZONTAL, &gDialogLineNum, 1, 2);
    }

    create_dl_translation_matrix(MENU_MTX_NOPUSH, (gDialogLineNum * X_VAL4_1) - X_VAL4_2, Y_VAL4_1 - (gLastDialogLineNum * Y_VAL4_2), 0);

    if (gDialogBoxType == DIALOG_TYPE_ROTATE) {
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    } else {
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
    }

    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
}

#define X_VAL5 118.0f
#define Y_VAL5_1 -16
#define Y_VAL5_2 5
#define X_Y_VAL6 0.8f

void render_dialog_triangle_next(s8 linesPerBox) {
    s32 globalTimer = gGlobalTimer;

    if (globalTimer & 0x8) {
        return;
    }

    create_dl_translation_matrix(MENU_MTX_PUSH, X_VAL5, (linesPerBox * Y_VAL5_1) + Y_VAL5_2, 0);
    create_dl_scale_matrix(MENU_MTX_NOPUSH, X_Y_VAL6, X_Y_VAL6, 1.0f);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, -DEFAULT_DIALOG_BOX_ANGLE, 0, 0, 1.0f);

    if (gDialogBoxType == DIALOG_TYPE_ROTATE) { // White Text
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    } else { // Black Text
        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
    }

    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void handle_special_dialog_text(s16 dialogID) { // dialog ID tables, in order
    // King Bob-omb (Start), Whomp (Start), King Bob-omb (throw him out), Eyerock (Start), Wiggler (Start)
    s16 dialogBossStart[] = { DIALOG_017, DIALOG_114, DIALOG_128, DIALOG_117, DIALOG_150 };
    // Koopa the Quick (BoB), Koopa the Quick (THI), Penguin Race, Fat Penguin Race (120 stars)
    s16 dialogRaceSound[] = { DIALOG_005, DIALOG_009, DIALOG_055, DIALOG_164             };
    // Red Switch, Green Switch, Blue Switch, 100 coins star, Bowser Red Coin Star
    s16 dialogStarSound[] = { DIALOG_010, DIALOG_011, DIALOG_012, DIALOG_013, DIALOG_014 };
    // King Bob-omb (Start), Whomp (Defeated), King Bob-omb (Defeated, missing in JP), Eyerock (Defeated), Wiggler (Defeated)
    s16 dialogBossStop[]  = { DIALOG_017, DIALOG_115, DIALOG_116, DIALOG_118, DIALOG_152 };
    s16 i;

    for (i = 0; i < (s16) ARRAY_COUNT(dialogBossStart); i++) {
        if (dialogBossStart[i] == dialogID) {
            seq_player_unlower_volume(SEQ_PLAYER_LEVEL, 60);
            play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, SEQ_EVENT_BOSS), 0);
            return;
        }
    }

    for (i = 0; i < (s16) ARRAY_COUNT(dialogRaceSound); i++) {
        if (dialogRaceSound[i] == dialogID && gDialogLineNum == 1) {
            play_race_fanfare();
            return;
        }
    }

    for (i = 0; i < (s16) ARRAY_COUNT(dialogStarSound); i++) {
        if (dialogStarSound[i] == dialogID && gDialogLineNum == 1) {
            play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
            return;
        }
    }

    for (i = 0; i < (s16) ARRAY_COUNT(dialogBossStop); i++) {
        if (dialogBossStop[i] == dialogID) {
            seq_player_fade_out(SEQ_PLAYER_LEVEL, 1);
            return;
        }
    }
}

u8 gEndCutsceneStrEn0[] = { TEXT_FILE_MARIO_EXCLAMATION };
u8 gEndCutsceneStrEn1[] = { TEXT_POWER_STARS_RESTORED };
u8 gEndCutsceneStrEn2[] = { TEXT_THANKS_TO_YOU };
u8 gEndCutsceneStrEn3[] = { TEXT_THANK_YOU_MARIO };
u8 gEndCutsceneStrEn4[] = { TEXT_SOMETHING_SPECIAL };
u8 gEndCutsceneStrEn5[] = { TEXT_LISTEN_EVERYBODY };
u8 gEndCutsceneStrEn6[] = { TEXT_LETS_HAVE_CAKE };
u8 gEndCutsceneStrEn7[] = { TEXT_FOR_MARIO };
u8 gEndCutsceneStrEn8[] = { TEXT_FILE_MARIO_QUESTION };

u8 *gEndCutsceneStringsEn[] = {
    gEndCutsceneStrEn0,
    gEndCutsceneStrEn1,
    gEndCutsceneStrEn2,
    gEndCutsceneStrEn3,
    gEndCutsceneStrEn4,
    gEndCutsceneStrEn5,
    gEndCutsceneStrEn6,
    gEndCutsceneStrEn7,
    // This [8] string is actually unused. In the cutscene handler, the developers do not
    // set the 8th one, but use the first string again at the very end, so Peach ends up
    // saying "Mario!" twice. It is likely that she was originally meant to say "Mario?" at
    // the end but the developers changed their mind, possibly because the line recorded
    // sounded more like an exclamation than a question.
    gEndCutsceneStrEn8,
    NULL
};


u16 gCutsceneMsgFade        =  0;
s16 gCutsceneMsgIndex       = -1;
s16 gCutsceneMsgDuration = -1;
s16 gCutsceneMsgTimer = 0;
s8 gDialogCameraAngleIndex = CAM_SELECTION_MARIO;
s8 gDialogCourseActNum = 1;

#define DIAG_VAL1 16
#define DIAG_VAL3 132 // US & EU
#define DIAG_VAL4 5
#define DIAG_VAL2 240 // JP & US

// Name expansion keeps authored pages, but may require a wider text window.
static s16 get_named_dialog_width(const u8 *source, const u8 *text, enum MarioTextStyle style) {
    s32 originalSize = 1;
    s32 namedSize = 1;
    while (source[originalSize - 1] != DIALOG_CHAR_TERMINATOR) {
        originalSize++;
    }
    while (text[namedSize - 1] != DIALOG_CHAR_TERMINATOR) {
        namedSize++;
    }
    if (namedSize <= originalSize) {
        return DIAG_VAL3;
    }

    s32 width = DIAG_VAL3;
    s32 x = 0;
    u8 previous = 0;
    for (s32 i = 0; text[i] != DIALOG_CHAR_TERMINATOR; i++) {
        u8 character = text[i];
        if (character == DIALOG_CHAR_COLOR) {
            // Match the renderer: malformed controls leave their payload as text.
            s32 payload = 0;
            s32 reset = TRUE;
            while (payload < 8 && text[i + 1 + payload] == 0x9F) {
                payload++;
            }
            if (payload != 8) {
                reset = FALSE;
                payload = 0;
                while (payload < 8) {
                    u8 component = text[i + 1 + payload];
                    if (component >= 0x10 && (component < 0x24 || component > 0x29)) {
                        break;
                    }
                    payload++;
                }
            }
            i += reset ? 8 : payload;
            continue;
        }
        u8 glyph =
            mario_character_text_glyph(style, MARIO_TEXT_DIALOG, &previous, character, text + i + 1);
        if (character == DIALOG_CHAR_NEWLINE) {
            x = 0;
        } else if (character == DIALOG_CHAR_SPACE || character == DIALOG_CHAR_SLASH) {
            x += gDialogCharWidths[DIALOG_CHAR_SPACE] * (character == DIALOG_CHAR_SLASH ? 2 : 1);
        } else if (character == DIALOG_CHAR_MULTI_THE || character == DIALOG_CHAR_MULTI_YOU) {
            const u8 words[2][3] = { { 0x37, 0x2B, 0x28 }, { 0x3C, 0x32, 0x38 } };
            const u8 *word = words[character == DIALOG_CHAR_MULTI_YOU];
            for (s32 j = 0; j < 3; j++) {
                width = MAX(width, x + 16);
                x += gDialogCharWidths[word[j]];
            }
        } else if (character == DIALOG_CHAR_STAR_COUNT) {
            if (gDialogVariable / 10 != 0) {
                width = MAX(width, x + 16);
                x += gDialogCharWidths[gDialogVariable / 10];
            }
            width = MAX(width, x + 16);
            x += gDialogCharWidths[gDialogVariable % 10];
        } else if (character != DIALOG_CHAR_DAKUTEN && character != DIALOG_CHAR_PERIOD_OR_HANDAKUTEN
                   && glyph != MARIO_TEXT_SKIP) {
            width = MAX(width, x + 16);
            x += gDialogCharWidths[glyph];
        }
    }
    return MIN(width, SCREEN_WIDTH - 16);
}

// Page boundaries are separated by linesPerBox newline characters.
static s16 get_previous_dialog_page(struct DialogEntry *dialog) {
    const u8 *str = segmented_to_virtual(dialog->str);
    s16 pos = gDialogTextPos - 2; // Skip the newline immediately before this page.
    s32 lines = 0;

    for (; pos >= 0; pos--) {
        if (str[pos] == DIALOG_CHAR_NEWLINE && ++lines == dialog->linesPerBox) {
            return pos + 1;
        }
    }
    return 0;
}

// Keep the authored page boundaries, but omit unused lines from the background.
static s8 get_dialog_page_line_count(struct DialogEntry *dialog, s16 textPos, s16 *nextPagePos) {
    const u8 *str = segmented_to_virtual(dialog->str);
    s8 line = 1;
    s8 lastTextLine = 1;
    *nextPagePos = -1;
    while (str[textPos] != DIALOG_CHAR_TERMINATOR) {
        u8 character = str[textPos++];
        if (character == DIALOG_CHAR_NEWLINE) {
            if (line == dialog->linesPerBox) {
                *nextPagePos = textPos;
                break;
            }
            line++;
        } else if (character == DIALOG_CHAR_COLOR) {
            // Colour commands carry eight encoded bytes, not text or newlines.
            for (s32 i = 0; i < 8 && str[textPos] != DIALOG_CHAR_TERMINATOR; i++) {
                textPos++;
            }
        } else if (character != DIALOG_CHAR_SPACE && character != DIALOG_CHAR_SLASH
                   && character != DIALOG_CHAR_DAKUTEN
                   && character != DIALOG_CHAR_PERIOD_OR_HANDAKUTEN) {
            lastTextLine = line;
        }
    }
    return lastTextLine;
}

static s8 get_dialog_box_line_count(struct DialogEntry *dialog) {
    s16 nextPagePos;
    s8 lines = get_dialog_page_line_count(dialog, gDialogTextPos, &nextPagePos);
    if (gDialogBoxState == DIALOG_STATE_HORIZONTAL && nextPagePos != -1) {
        // Forward and backward scrolling both draw this page and its successor.
        s8 nextLines = get_dialog_page_line_count(dialog, nextPagePos, &nextPagePos);
        lines = MAX(lines, nextLines);
    }
    return lines;
}

void render_dialog_entries(void) {
    enum MarioTextStyle style = sDialogTextStyle;
    s8 lowerBound = 0;
    void **dialogTable = segmented_to_virtual(languageTable[gInGameLanguage][0]);
    struct DialogEntry *dialog = segmented_to_virtual(dialogTable[gDialogID]);

    // if the dialog entry is invalid, set the ID to DIALOG_NONE.
    if (segmented_to_virtual(NULL) == dialog) {
        gDialogID = DIALOG_NONE;
        sDialogTextStyle = MARIO_TEXT_STANDARD;
        return;
    }

    u8 *sourceText = segmented_to_virtual(dialog->str);
    u8 namedText[mario_character_text_name_size(style, MARIO_TEXT_DIALOG, sourceText)];
    struct DialogEntry namedDialog = *dialog;
    namedDialog.str =
        (u8 *) mario_character_text_replace_name(style, MARIO_TEXT_DIALOG, sourceText, namedText);
    dialog = &namedDialog;

    switch (gDialogBoxState) {
        case DIALOG_STATE_OPENING:
            if (gDialogBoxOpenTimer == DEFAULT_DIALOG_BOX_ANGLE) {
                sDialogScrollingBack = FALSE;
                play_dialog_sound(gDialogID);
                play_sound(SOUND_MENU_MESSAGE_APPEAR, gGlobalSoundSource);
            }

            if (gDialogBoxType == DIALOG_TYPE_ROTATE) {
                gDialogBoxOpenTimer -= 7.5f;
                gDialogBoxScale -= 1.5f;
            } else {
                gDialogBoxOpenTimer -= 10.0f;
                gDialogBoxScale -= 2.0f;
            }

            if (gDialogBoxOpenTimer == 0.0f) {
                gDialogBoxState = DIALOG_STATE_VERTICAL;
                gDialogLineNum = 1;
            }
            lowerBound = 1;
            break;

        case DIALOG_STATE_VERTICAL:
            gDialogBoxOpenTimer = 0.0f;

            // Holding Z scrolls backward and takes priority over holding A.
            if (gPlayer1Controller->buttonDown & Z_TRIG) {
                if (gDialogTextPos > 0) {
                    gDialogTextPos = get_previous_dialog_page(dialog);
                    gDialogScrollOffsetY = dialog->linesPerBox * DIAG_VAL1;
                    sDialogScrollingBack = TRUE;
                    gDialogBoxState = DIALOG_STATE_HORIZONTAL;
                    play_sound(SOUND_MENU_MESSAGE_NEXT_PAGE, gGlobalSoundSource);
                }
            } else if ((gPlayer1Controller->buttonPressed & (A_BUTTON | B_BUTTON))
                       || (!jrb_simpleflips_room_active()
                           && (gPlayer1Controller->buttonDown & A_BUTTON))) {
                if (gLastDialogPageStrPos == -1) {
                    s32 holdToSaveCapSwitch =
                        gLastDialogResponse && gDialogID >= DIALOG_010 && gDialogID <= DIALOG_012
                        && (gPlayer1Controller->buttonDown & A_BUTTON)
                        && !(gPlayer1Controller->buttonPressed & (A_BUTTON | B_BUTTON));
                    // A continued hold saves at cap switches; fresh presses
                    // still confirm the selected response, including No.
                    if (!gLastDialogResponse || gDialogID == DIALOG_013 || gDialogID == DIALOG_014
                        || holdToSaveCapSwitch
                        || (gPlayer1Controller->buttonPressed & (A_BUTTON | B_BUTTON))) {
                        if (holdToSaveCapSwitch) {
                            gDialogLineNum = DIALOG_RESPONSE_YES;
                        }
                        handle_special_dialog_text(gDialogID);
                        gDialogBoxState = DIALOG_STATE_CLOSING;
                    }
                } else {
                    gDialogBoxState = DIALOG_STATE_HORIZONTAL;
                    play_sound(SOUND_MENU_MESSAGE_NEXT_PAGE, gGlobalSoundSource);
                }
            }
            // Reverse scrolling starts with a full-page offset, so draw the visible page.
            lowerBound = (gDialogScrollOffsetY / DIAG_VAL1) + 1;
            break;
        case DIALOG_STATE_HORIZONTAL: // scrolling
            if (sDialogScrollingBack) {
                gDialogScrollOffsetY -= dialog->linesPerBox * 2;
                if (gDialogScrollOffsetY <= 0) {
                    gDialogScrollOffsetY = 0;
                    sDialogScrollingBack = FALSE;
                    gDialogBoxState = DIALOG_STATE_VERTICAL;
                }
            } else {
                gDialogScrollOffsetY += dialog->linesPerBox * 2;
            }

            if (gDialogScrollOffsetY >= dialog->linesPerBox * DIAG_VAL1) {
                gDialogTextPos = gLastDialogPageStrPos;
                gDialogBoxState = DIALOG_STATE_VERTICAL;
                gDialogScrollOffsetY = 0;
            }
            lowerBound = (gDialogScrollOffsetY / DIAG_VAL1) + 1;
            break;

        case DIALOG_STATE_CLOSING:
            if (gDialogBoxOpenTimer == 20.0f) {
                level_set_transition(0, NULL);
                play_sound(SOUND_MENU_MESSAGE_DISAPPEAR, gGlobalSoundSource);

                gDialogResponse = gDialogLineNum;
            }

            gDialogBoxOpenTimer = gDialogBoxOpenTimer + 10.0f;
            gDialogBoxScale = gDialogBoxScale + 2.0f;

            if (gDialogBoxOpenTimer == DEFAULT_DIALOG_BOX_ANGLE) {
                gDialogBoxState = DIALOG_STATE_OPENING;
                gDialogID = DIALOG_NONE;
                sDialogTextStyle = MARIO_TEXT_STANDARD;
                gDialogTextPos = 0;
                gLastDialogResponse = 0;
                gLastDialogPageStrPos = 0;
                gDialogResponse = DIALOG_RESPONSE_NONE;
            }
            lowerBound = 1;
            break;
    }

    s8 boxLines = get_dialog_box_line_count(dialog);
    s16 boxWidth = get_named_dialog_width(sourceText, dialog->str, style);
#ifdef INPUT_DISPLAY
    dialog->leftOffset = input_display_enabled() ? INPUT_DISPLAY_CONTENT_LEFT
                                                 : MIN(dialog->leftOffset, SCREEN_WIDTH - boxWidth - 8);
#else
    dialog->leftOffset = MIN(dialog->leftOffset, SCREEN_WIDTH - boxWidth - 8);
#endif
    render_dialog_box_type(dialog, boxLines, boxWidth);

    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE,
    // Horizontal scissoring isn't really required and can potentially mess up widescreen enhancements.
#ifdef WIDESCREEN
                  0,
#else
                  ensure_nonnegative(dialog->leftOffset),
#endif
                  ensure_nonnegative(DIAG_VAL2 - dialog->width),
#ifdef WIDESCREEN
                  SCREEN_WIDTH,
#else
#ifdef INPUT_DISPLAY
                  ensure_nonnegative(dialog->leftOffset
                                     + (s16) (boxWidth * dialog_input_display_scale(boxWidth))),
#else
                  ensure_nonnegative(boxWidth + dialog->leftOffset),
#endif
#endif
                  ensure_nonnegative(240 + ((boxLines * 80) / DIAG_VAL4) - dialog->width));
    handle_dialog_text_and_pages(0, dialog, lowerBound, style);

    if (gLastDialogPageStrPos == -1 && gLastDialogResponse == 1) {
        // The response cursor follows the final visible line rather than padding.
        gLastDialogLineNum = boxLines;
        render_dialog_triangle_choice();
    }
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 2, 2, SCREEN_WIDTH - gBorderHeight / 2, SCREEN_HEIGHT - gBorderHeight / 2);
    if (gLastDialogPageStrPos != -1 && gDialogBoxState == DIALOG_STATE_VERTICAL) {
        render_dialog_triangle_next(boxLines);
    }
}

// Calls a gMenuMode value defined by render_menus_and_dialogs cases
void set_menu_mode(s16 mode) {
    if (mode == MENU_MODE_NONE || gMenuMode == MENU_MODE_NONE) {
        reset_menu_navigation();
        gMenuMode = mode;
    }
}

void reset_cutscene_msg_fade(void) {
    gCutsceneMsgFade = 0;
}

void clear_cutscene_message(void) {
    gCutsceneMsgIndex = -1;
    gCutsceneMsgTimer = 0;
    gCutsceneMsgFade = 0;
}

void dl_rgba16_begin_cutscene_msg_fade(void) {
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gCutsceneMsgFade);
}

void dl_rgba16_stop_cutscene_msg_fade(void) {
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    if (gCutsceneMsgFade < 250) {
        gCutsceneMsgFade += 25;
    } else {
        gCutsceneMsgFade = 255;
    }
}

u32 ascii_to_credits_char(u8 c) {
    if (c >= 'A' && c <= 'Z') return (c - ('A' - 0xA));
    if (c >= 'a' && c <= 'z') return (c - ('a' - 0xA)); // remap lower to upper case
    if (c == ' ') return GLOBAL_CHAR_SPACE;
    if (c == '.') return 0x24;
    if (c == '3') return ASCII_TO_DIALOG('3');
    if (c == '4') return ASCII_TO_DIALOG('4');
    if (c == '6') return ASCII_TO_DIALOG('6');

    return GLOBAL_CHAR_SPACE;
}

void print_credits_str_ascii(s16 x, s16 y, const char *str) {
    s32 pos = 0;
    u8 c = str[pos];
    u8 creditStr[100];

    while (c != 0) {
        creditStr[pos++] = ascii_to_credits_char(c);
        c = str[pos];
    }

    creditStr[pos] = GLOBAR_CHAR_TERMINATOR;

    print_credits_string(x, y, creditStr);
}

void set_cutscene_message(s16 xOffset, s16 yOffset, s16 msgIndex, s16 msgDuration) {
    // is message done printing?
    if (gCutsceneMsgIndex == -1) {
        gCutsceneMsgIndex = msgIndex;
        gCutsceneMsgDuration = msgDuration;
        gCutsceneMsgTimer = 0;
        gCutsceneMsgXOffset = xOffset;
        gCutsceneMsgYOffset = yOffset;
        gCutsceneMsgFade = 0;
    }
}

void do_cutscene_handler(void) {
    // is a cutscene playing? do not perform this handler's actions if so.
    if (gCutsceneMsgIndex == -1 || (level_is_ending() && sCurrPlayMode == PLAY_MODE_PAUSED)) {
        return;
    }

    create_dl_ortho_matrix();

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gCutsceneMsgFade);

    // get the x coordinate of where the cutscene string starts.
    s16 x = get_str_x_pos_from_center(gCutsceneMsgXOffset, gEndCutsceneStringsEn[gCutsceneMsgIndex], 10.0f);
    print_generic_string(x, 240 - gCutsceneMsgYOffset, gEndCutsceneStringsEn[gCutsceneMsgIndex]);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

    // if the timing variable is less than 5, increment
    // the fade until we are at full opacity.
    if (gCutsceneMsgTimer < 5) {
        gCutsceneMsgFade += 50;
    }

    // if the cutscene frame length + the fade-in counter is
    // less than the timer, it means we have exceeded the
    // time that the message is supposed to remain on
    // screen. if (message_duration = 50) and (msg_timer = 55)
    // then after the first 5 frames, the message will remain
    // on screen for another 50 frames until it starts fading.
    if (gCutsceneMsgDuration + 5 < gCutsceneMsgTimer) {
        gCutsceneMsgFade -= 50;
    }

    // like the first check, it takes 5 frames to fade out, so
    // perform a + 10 to account for the earlier check (10-5=5).
    if (gCutsceneMsgDuration + 10 < gCutsceneMsgTimer) {
        gCutsceneMsgIndex = -1;
        gCutsceneMsgFade = 0;
        gCutsceneMsgTimer = 0;
        return;
    }

    gCutsceneMsgTimer++;
}

#define PEACH_MESSAGE_TIMER 250

#define STR_X  38
#define STR_Y 142

// "Dear Mario" message handler
void print_peach_letter_message(void) {
#ifdef VERSION_EU
    void **dialogTable;
    gInGameLanguage = eu_get_language();
    switch (gInGameLanguage) {
        default:
        case LANGUAGE_ENGLISH: dialogTable = segmented_to_virtual(dialog_table_eu_en); break;
        case LANGUAGE_FRENCH:  dialogTable = segmented_to_virtual(dialog_table_eu_fr); break;
        case LANGUAGE_GERMAN:  dialogTable = segmented_to_virtual(dialog_table_eu_de); break;
    }
#else
    void **dialogTable = segmented_to_virtual(seg2_dialog_table);
#endif
    struct DialogEntry *dialog = segmented_to_virtual(dialogTable[gDialogID]);
    u8 *str = segmented_to_virtual(dialog->str);

    create_dl_translation_matrix(MENU_MTX_PUSH, 97.0f, 118.0f, 0);

    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gCutsceneMsgFade);
    gSPDisplayList(gDisplayListHead++, castle_grounds_seg7_dl_0700EA58);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 20, 20, 20, gCutsceneMsgFade);

    print_generic_string_styled(STR_X, STR_Y, str, MARIO_TEXT_STANDARD);
#ifdef VERSION_JP
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
#else
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    gDPSetEnvColor(gDisplayListHead++, 200, 80, 120, gCutsceneMsgFade);
    gSPDisplayList(gDisplayListHead++, castle_grounds_seg7_us_dl_0700F2E8);
#endif

    // at the start/end of message, reset the fade.
    if (gCutsceneMsgTimer == 0) {
        gCutsceneMsgFade = 0;
    }

    // we're less than 20 increments, so increase the fade.
    if (gCutsceneMsgTimer < 20) {
        gCutsceneMsgFade += 10;
    }

    // we're after PEACH_MESSAGE_TIMER increments, so decrease the fade.
    if (gCutsceneMsgTimer > PEACH_MESSAGE_TIMER) {
        gCutsceneMsgFade -= 10;
    }

    // 20 increments after the start of the decrease, we're
    // back where we are, so reset everything at the end.
    if (gCutsceneMsgTimer > (PEACH_MESSAGE_TIMER + 20)) {
        gCutsceneMsgIndex = -1;
        gCutsceneMsgFade = 0; //! uselessly reset since the next execution will just set it to 0 again.
        gDialogID = DIALOG_NONE;
        sDialogTextStyle = MARIO_TEXT_STANDARD;
        gCutsceneMsgTimer = 0;
        return; // return to avoid incrementing the timer
    }

    gCutsceneMsgTimer++;
}

/**
 * Renders the cannon reticle when Mario is inside a cannon.
 * Formed by four triangles.
 */
void render_hud_cannon_reticle(void) {
    create_dl_translation_matrix(MENU_MTX_PUSH, 160.0f, 120.0f, 0);

    gDPSetEnvColor(gDisplayListHead++, 50, 50, 50, 180);
    create_dl_translation_matrix(MENU_MTX_PUSH, -20.0f, -8.0f, 0);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    create_dl_translation_matrix(MENU_MTX_PUSH, 20.0f, 8.0f, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, 180.0f, 0, 0, 1.0f);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    create_dl_translation_matrix(MENU_MTX_PUSH, 8.0f, -20.0f, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, DEFAULT_DIALOG_BOX_ANGLE, 0, 0, 1.0f);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    create_dl_translation_matrix(MENU_MTX_PUSH, -8.0f, 20.0f, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, -DEFAULT_DIALOG_BOX_ANGLE, 0, 0, 1.0f);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void reset_red_coins_collected(void) {
    gRedCoinsCollected = 0;
    gRedCoinStarSpawned = FALSE;
}

#ifdef REONUCAM
void render_reonucam_speed_setting(s32 allowInput) {
    u8 speed = game_options_get(GO_CAMERA_SPEED);

    if (allowInput && (gPlayer1Controller->buttonDown & Z_TRIG)) {
        if (gPlayer1Controller->buttonPressed & R_JPAD) {
            speed = (speed + 1) % REONUCAM_SPEED_COUNT;
            gReonucamState.speed = speed;
            game_options_set(GO_CAMERA_SPEED, speed);
        } else if (gPlayer1Controller->buttonPressed & L_JPAD) {
            speed = (speed + REONUCAM_SPEED_COUNT - 1) % REONUCAM_SPEED_COUNT;
            gReonucamState.speed = speed;
            game_options_set(GO_CAMERA_SPEED, speed);
        }
    }

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
#ifdef INPUT_DISPLAY
    s16 footerX = input_display_enabled() ? 220 : 164;
#else
    s16 footerX = 164;
#endif
    if (speed < REONUCAM_SPEED_COUNT) {
        print_generic_string(footerX, 22, sReonucamSpeedLabels[speed]);
    } else {
        player_stats_menu_text(footerX, 22, "Custom camera speed");
    }
    u8 textCamSpeedControls[] = { TEXT_CAM_SPEED_CONTROLS };
    print_generic_string(footerX, 8, textCamSpeedControls);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

#endif

void change_dialog_camera_angle(void) {
    if (cam_select_alt_mode(0) == CAM_SELECTION_MARIO) {
        gDialogCameraAngleIndex = CAM_SELECTION_MARIO;
    } else {
        gDialogCameraAngleIndex = CAM_SELECTION_FIXED;
    }
}

void shade_screen(void) {
    Gfx *dlHead = gDisplayListHead;

    gSPDisplayList(dlHead++, dl_shade_screen_begin);
    gDPFillRectangle(dlHead++, GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(0), gBorderHeight,
        (GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(0) - 1), ((SCREEN_HEIGHT - gBorderHeight) - 1));
    gSPDisplayList(dlHead++, dl_shade_screen_end);

    gDisplayListHead = dlHead;
}

void print_animated_red_coin(s16 x, s16 y) {
    s32 globalTimer = gGlobalTimer;
    u8 alpha = gRedCoinStarSpawned ? 96 : 255;

    create_dl_translation_matrix(MENU_MTX_PUSH, x, y, 0);
    create_dl_scale_matrix(MENU_MTX_NOPUSH, 0.2f, 0.2f, 1.0f);
    gDPPipeSync(gDisplayListHead++);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, alpha);
    if (gRedCoinStarSpawned) {
        gDPSetRenderMode(gDisplayListHead++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);
    } else {
        gDPSetRenderMode(gDisplayListHead++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
    }

#ifdef IA8_30FPS_COINS
    static const Gfx *const frames[] = { coin_seg3_dl_red_0,    coin_seg3_dl_red_22_5,
                                         coin_seg3_dl_red_45,   coin_seg3_dl_red_67_5,
                                         coin_seg3_dl_red_90,   coin_seg3_dl_red_67_5_r,
                                         coin_seg3_dl_red_45_r, coin_seg3_dl_red_22_5_r };
    static const Gfx *const fadedFrames[] = {
        coin_seg3_dl_red_pause_0,    coin_seg3_dl_red_pause_22_5,  coin_seg3_dl_red_pause_45,
        coin_seg3_dl_red_pause_67_5, coin_seg3_dl_red_pause_90,    coin_seg3_dl_red_pause_67_5_r,
        coin_seg3_dl_red_pause_45_r, coin_seg3_dl_red_pause_22_5_r
    };
    s32 frame = globalTimer & 0x7;
#else
    static const Gfx *const frames[] = { coin_seg3_dl_red_front, coin_seg3_dl_red_tilt_right,
                                         coin_seg3_dl_red_side, coin_seg3_dl_red_tilt_left };
    static const Gfx *const fadedFrames[] = { coin_seg3_dl_red_pause_front,
                                              coin_seg3_dl_red_pause_tilt_right,
                                              coin_seg3_dl_red_pause_side,
                                              coin_seg3_dl_red_pause_tilt_left };
    s32 frame = (globalTimer & 0x6) >> 1;
#endif
    gSPDisplayList(gDisplayListHead++, gRedCoinStarSpawned ? fadedFrames[frame] : frames[frame]);

    gDPPipeSync(gDisplayListHead++);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_pause_red_coins(void) {
    s8 x;
    s8 redCoinCount;
    Mtx *mtx;

    if (gRedCoinsCollected <= 9) {
        for (x = 0; x < gRedCoinsCollected; x++) {
            print_animated_red_coin(GFX_DIMENSIONS_FROM_RIGHT_EDGE(30) - x * 20, 16);
        }
        return;
    }

    print_animated_red_coin(GFX_DIMENSIONS_FROM_RIGHT_EDGE(108), 16);
    // Keep the numeric count visible after the star has spawned.
    mtx = alloc_display_list(sizeof(*mtx));
    if (mtx == NULL) {
        return;
    }
    guOrtho(mtx, 0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -10.0f, 10.0f, 1.0f);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx),
              G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);

    redCoinCount = gRedCoinsCollected;
    if (redCoinCount > 99) {
        redCoinCount = 99;
    }

    add_glyph_texture(GLYPH_MULTIPLY);
    render_textrect(GFX_DIMENSIONS_FROM_RIGHT_EDGE(100), 16, 0, FALSE);
    add_glyph_texture(char_to_glyph_index((char) (48 + (redCoinCount / 10))));
    render_textrect(GFX_DIMENSIONS_FROM_RIGHT_EDGE(86), 16, 0, FALSE);
    add_glyph_texture(char_to_glyph_index((char) (48 + (redCoinCount % 10))));
    render_textrect(GFX_DIMENSIONS_FROM_RIGHT_EDGE(86), 16, 1, FALSE);

    gSPDisplayList(gDisplayListHead++, dl_hud_img_end);
}

#ifdef WIDE
void render_widescreen_setting(s32 allowInput) {
#ifdef INPUT_DISPLAY
    const s16 footerX = input_display_enabled() ? INPUT_DISPLAY_CONTENT_LEFT : 24;
#else
    const s16 footerX = 24;
#endif
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    gConfig.widescreen = game_options_get(GO_ASPECT_RATIO);
    if (!gConfig.widescreen) {
        print_generic_string(footerX, 22, textCurrRatio43);
    } else {
        print_generic_string(footerX, 22, textCurrRatio169);
    }
    print_generic_string(footerX, 8, textPressL);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    if (allowInput && (gPlayer1Controller->buttonDown & Z_TRIG)
        && (gPlayer1Controller->buttonPressed & L_TRIG)) {
        gConfig.widescreen ^= 1;
        game_options_set(GO_ASPECT_RATIO, gConfig.widescreen);
        save_file_set_widescreen_mode(gConfig.widescreen);
    }
}
#endif

#if defined(VERSION_JP) || defined(VERSION_SH)
    #define CRS_NUM_X1 93
#elif defined(VERSION_US)
    #define CRS_NUM_X1 100
#elif defined(VERSION_EU)
    #define CRS_NUM_X1 get_string_width(LANGUAGE_ARRAY(textCourse)) + 51
#endif

#ifdef VERSION_EU
    #define TXT_COURSE_X      48
    #define TXT_STAR_X        89
    #define ACT_NAME_X        107
    #define LVL_NAME_X        108
    #define SECRET_LVL_NAME_X get_str_x_pos_from_center(159, &courseName[3], 10.0f)
    #define MYSCORE_X         48
#else
    #define TXT_COURSE_X      63
    #define TXT_STAR_X        98
    #define ACT_NAME_X        116
    #define LVL_NAME_X        117
    #define SECRET_LVL_NAME_X 94
    #define MYSCORE_X         62
#endif

void render_pause_my_score_coins(void) {
#ifdef INPUT_DISPLAY
    const s16 offsetX = input_display_enabled() ? 50 : 0;
#else
    const s16 offsetX = 0;
#endif
    u8 textCourse[] = { TEXT_COURSE };
    u8 textMyScore[] = { TEXT_MY_SCORE };
    u8 textStar[] = { TEXT_STAR };
    u8 textUnfilledStar[] = { TEXT_UNFILLED_STAR };

    u8 strCourseNum[4];

    void **courseNameTbl = segmented_to_virtual(languageTable[gInGameLanguage][1]);
    void    **actNameTbl = segmented_to_virtual(languageTable[gInGameLanguage][2]);

    u8 courseIndex = COURSE_NUM_TO_INDEX(gCurrCourseNum);
    u8 starFlags = courseIndex <= COURSE_NUM_TO_INDEX(COURSE_STAGES_MAX)
                       ? save_file_get_star_flags(gCurrSaveFileNum - 1, courseIndex)
                       : 0;

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);

    if (courseIndex <= COURSE_NUM_TO_INDEX(COURSE_STAGES_MAX)) {
        print_hud_my_score_coins(1, gCurrSaveFileNum - 1, courseIndex, 178 + offsetX, 103);
        print_hud_my_score_stars(gCurrSaveFileNum - 1, courseIndex, 118 + offsetX, 103);
    }

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);

    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);

    if (courseIndex <= COURSE_NUM_TO_INDEX(COURSE_STAGES_MAX)
        && (save_file_get_course_star_count(gCurrSaveFileNum - 1, courseIndex) != 0)) {
        print_generic_string(MYSCORE_X + offsetX, 121, LANGUAGE_ARRAY(textMyScore));
    }

    u8 *courseName = segmented_to_virtual(courseNameTbl[courseIndex]);

    if (courseIndex <= COURSE_NUM_TO_INDEX(COURSE_STAGES_MAX)) {
        print_generic_string(TXT_COURSE_X + offsetX, 157, LANGUAGE_ARRAY(textCourse));
        int_to_str(gCurrCourseNum, strCourseNum);
        print_generic_string(CRS_NUM_X1 + offsetX, 157, strCourseNum);

        u8 *actName = segmented_to_virtual(actNameTbl[COURSE_NUM_TO_INDEX(gCurrCourseNum) * 6 + gDialogCourseActNum - 1]);

        if (starFlags & (1 << (gDialogCourseActNum - 1))) {
            print_generic_string(TXT_STAR_X + offsetX, 140, textStar);
        } else {
            print_generic_string(TXT_STAR_X + offsetX, 140, textUnfilledStar);
        }

        print_generic_string(ACT_NAME_X + offsetX, 140, actName);
        print_generic_string(LVL_NAME_X + offsetX, 157, &courseName[3]);
    } else {
        print_generic_string(SECRET_LVL_NAME_X + offsetX, 157, &courseName[3]);
    }

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

#define TXT1_X 3
#define TXT2_X 119
#define Y_VAL7 2

void render_pause_camera_options(s16 x, s16 y, s8 *index, s16 xIndex) {
    u8 textLakituMario[] = { TEXT_LAKITU_MARIO };
    u8 textLakituStop[] = { TEXT_LAKITU_STOP };
    u8 textNormalUpClose[] = { TEXT_NORMAL_UPCLOSE };
    u8 textNormalFixed[] = { TEXT_NORMAL_FIXED };

    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL, index, 1, 2);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);

    print_generic_string(x +     14, y +  2, textLakituMario);
    print_generic_string(x + TXT1_X, y - 13, LANGUAGE_ARRAY(textNormalUpClose));
    print_generic_string(x +    124, y +  2, textLakituStop);
    print_generic_string(x + TXT2_X, y - 13, LANGUAGE_ARRAY(textNormalFixed));

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    create_dl_translation_matrix(MENU_MTX_PUSH, ((*index - 1) * xIndex) + x, y + Y_VAL7, 0);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    switch (*index) {
        case CAM_SELECTION_MARIO:
            cam_select_alt_mode(CAM_SELECTION_MARIO);
            break;
        case CAM_SELECTION_FIXED:
            cam_select_alt_mode(CAM_SELECTION_FIXED);
            break;
    }
}

#define Y_VAL8 2

static s16 pause_menu_center_x(void) {
#ifdef INPUT_DISPLAY
    if (input_display_enabled()) {
        return (INPUT_DISPLAY_CONTENT_LEFT + SCREEN_WIDTH) / 2;
    }
#endif
    return SCREEN_WIDTH / 2;
}

void render_pause_course_options(s16 centerX, s16 y, s8 *index, s16 yIndex) {
    u8 textContinue[] = { TEXT_CONTINUE };
    u8 textExitCourse[] = { TEXT_EXIT_COURSE };
    s8 options[4];
    s8 selected = 1;
    s8 count = 0;
    s32 row;
    s16 optionY;
    s16 optionX;
    s16 selectedX = centerX;

    options[count++] = MENU_OPT_DEFAULT;
#ifndef DISABLE_EXIT_COURSE
    options[count++] = MENU_OPT_EXIT_COURSE;
#endif
    options[count++] = MENU_OPT_STATISTICS;
    options[count++] = MENU_OPT_OPTIONS;
    for (row = 0; row < count; row++) {
        if (options[row] == *index) {
            selected = row + 1;
        }
    }
    handle_menu_scrolling(MENU_SCROLL_VERTICAL_WRAP, &selected, 1, count);
    *index = options[selected - 1];

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    for (row = 0; row < count; row++) {
        optionY = y - row * yIndex - 2;
        optionX = centerX;
        switch (options[row]) {
            case MENU_OPT_DEFAULT:
                optionX = get_str_x_pos_from_center(centerX, LANGUAGE_ARRAY(textContinue), 1.0f);
                print_generic_string(optionX, optionY, LANGUAGE_ARRAY(textContinue));
                break;
            case MENU_OPT_EXIT_COURSE:
                optionX = get_str_x_pos_from_center(centerX, LANGUAGE_ARRAY(textExitCourse), 1.0f);
                print_generic_string(optionX, optionY, LANGUAGE_ARRAY(textExitCourse));
                break;
            case MENU_OPT_STATISTICS:
                optionX = player_stats_menu_text_centered(centerX, optionY, "STATISTICS");
                break;
            case MENU_OPT_OPTIONS:
                optionX = player_stats_menu_text_centered(centerX, optionY, "OPTIONS");
                break;
        }
        if (row == selected - 1) {
            selectedX = optionX;
        }
    }
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    create_dl_translation_matrix(MENU_MTX_PUSH, selectedX - 14, y - (selected - 1) * yIndex - Y_VAL8,
                                 0);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void render_pause_castle_menu_box(s16 x, s16 y) {
    create_dl_translation_matrix(MENU_MTX_PUSH, x - 78, y - 32, 0);
    create_dl_scale_matrix(MENU_MTX_NOPUSH, 1.2f, 0.8f, 1.0f);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 105);
    gSPDisplayList(gDisplayListHead++, dl_draw_text_bg_box);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    create_dl_translation_matrix(MENU_MTX_PUSH, x - 86, y - 56, 0);
    create_dl_rotation_matrix(MENU_MTX_NOPUSH, 180.0f, 0, 0, 1.0f);
    gDPPipeSync(gDisplayListHead++);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    create_dl_translation_matrix(MENU_MTX_PUSH, x + 86, y - 72, 0);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

void highlight_last_course_complete_stars(void) {
    u8 doneCourseIndex;

    if (gLastCompletedCourseNum == COURSE_NONE) {
        doneCourseIndex = COURSE_NUM_TO_INDEX(COURSE_MIN);
    } else {
        doneCourseIndex = COURSE_NUM_TO_INDEX(gLastCompletedCourseNum);

        if (doneCourseIndex >= COURSE_NUM_TO_INDEX(COURSE_BONUS_STAGES)) {
            doneCourseIndex = COURSE_NUM_TO_INDEX(COURSE_BONUS_STAGES);
        }
    }

    gDialogLineNum = doneCourseIndex;
}

void print_hud_pause_colorful_str(void) {
    u8 textPause[] = { TEXT_PAUSE };

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);

    // Five 16-pixel HUD glyphs at a 12-pixel stride occupy 64 pixels.
    const s16 pauseX = pause_menu_center_x() - 32;
    print_hud_lut_string(HUD_LUT_GLOBAL, pauseX, gDialogBoxState == DIALOG_STATE_HORIZONTAL ? 51 : 71,
                         textPause);

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
}

void render_pause_castle_course_stars(s16 x, s16 y, s16 fileIndex, s16 courseIndex) {
    s16 hasStar = 0;

    u8 str[COURSE_STAGES_COUNT * 2];

    u8 textStar[] = { TEXT_STAR };

    u8 starFlags = save_file_get_star_flags(fileIndex, courseIndex);
    u16 starCount = save_file_get_course_star_count(fileIndex, courseIndex);

    u16 nextStar = 0;

    if (starFlags & STAR_FLAG_ACT_100_COINS) {
        starCount--;
        print_generic_string(x + 89, y - 5, textStar);
    }

    while (hasStar != starCount) {
        if (starFlags & (1 << nextStar)) {
            str[nextStar * 2] = DIALOG_CHAR_STAR_FILLED;
            hasStar++;
        } else {
            str[nextStar * 2] = DIALOG_CHAR_STAR_OPEN;
        }

        str[nextStar * 2 + 1] = DIALOG_CHAR_SPACE;
        nextStar++;
    }

    if (starCount == nextStar && starCount != 6) {
        str[nextStar * 2] = DIALOG_CHAR_STAR_OPEN;
        str[nextStar * 2 + 1] = DIALOG_CHAR_SPACE;
        nextStar++;
    }

    str[nextStar * 2] = DIALOG_CHAR_TERMINATOR;

    print_generic_string(x + 14, y + 13, str);
}

void render_pause_castle_main_strings(s16 x, s16 y) {
    void **courseNameTbl = segmented_to_virtual(languageTable[gInGameLanguage][1]);

    u8 textCoin[] = { TEXT_COIN_X };

    void *courseName;

    u8 strVal[8];
    s16 prevCourseIndex = gDialogLineNum;

    handle_menu_scrolling(MENU_SCROLL_HORIZONTAL, &gDialogLineNum, COURSE_NUM_TO_INDEX(COURSE_MIN) - 1,
                          COURSE_NUM_TO_INDEX(COURSE_BONUS_STAGES) + 1);

    if (gDialogLineNum == COURSE_NUM_TO_INDEX(COURSE_BONUS_STAGES) + 1) {
        gDialogLineNum = COURSE_NUM_TO_INDEX(COURSE_MIN); // Exceeded max, set to min
    }

    if (gDialogLineNum == COURSE_NUM_TO_INDEX(COURSE_MIN) - 1) {
        gDialogLineNum = COURSE_NUM_TO_INDEX(COURSE_BONUS_STAGES); // Exceeded min, set to max
    }

    if (gDialogLineNum != COURSE_NUM_TO_INDEX(COURSE_BONUS_STAGES)) {
        while (save_file_get_course_star_count(gCurrSaveFileNum - 1, gDialogLineNum) == 0) {
            if (gDialogLineNum >= prevCourseIndex) {
                gDialogLineNum++;
            } else {
                gDialogLineNum--;
            }

            if (gDialogLineNum == COURSE_NUM_TO_INDEX(COURSE_STAGES_MAX) + 1
             || gDialogLineNum == COURSE_NUM_TO_INDEX(COURSE_MIN) - 1) {
                gDialogLineNum = COURSE_NUM_TO_INDEX(COURSE_BONUS_STAGES);
                break;
            }
        }
    }

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);

    if (gDialogLineNum <= COURSE_NUM_TO_INDEX(COURSE_STAGES_MAX)) { // Main courses
        courseName = segmented_to_virtual(courseNameTbl[gDialogLineNum]);
        render_pause_castle_course_stars(x, y, gCurrSaveFileNum - 1, gDialogLineNum);
        print_generic_string(x + 34, y - 5, textCoin);
        int_to_str(save_file_get_course_coin_score(gCurrSaveFileNum - 1, gDialogLineNum), strVal);
        print_generic_string(x + 54, y - 5, strVal);
    } else { // Castle secret stars
        u8 textStarX[] = { TEXT_STAR_X };
        courseName = segmented_to_virtual(courseNameTbl[COURSE_MAX]);
        print_generic_string(x + 40, y + 13, textStarX);
        int_to_str(save_file_get_total_star_count(gCurrSaveFileNum - 1,
                                                  COURSE_NUM_TO_INDEX(COURSE_BONUS_STAGES),
                                                  COURSE_NUM_TO_INDEX(COURSE_MAX)),
                                                  strVal);
        print_generic_string(x + 60, y + 13, strVal);
    }

    print_generic_string(x - 9, y + 30, courseName);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

s8 gCourseCompleteCoinsEqual = FALSE;
s32 gCourseDoneMenuTimer = 0;
s32 gCourseCompleteCoins = 0;
s8 gHudFlash = HUD_FLASH_NONE;

#define PAUSE_MENU_UNPAUSE_DELAY_FRAMES 8
#define PAUSE_CASTLE_OPTIONS_Y 145

static u32 sPauseMenuOpenedFrame;
static s32 sPauseMenuStartRepressed;
static s32 sPauseMenuARepressed;
static s32 sPauseStatisticsOpen;
static s8 sPauseCastleSelection;
static u8 sPauseCastleOptionsHeld;
static u8 sPauseCastleOptionsStick;
static u32 sPauseCastleOptionsFrame;

static void reset_pause_menu_navigation(void) {
    reset_menu_navigation();
    sPauseCastleOptionsHeld = 0;
    u8 openingStick = gPlayer1Controller->rawStickX < -60 || gPlayer1Controller->rawStickX > 60
                              || gPlayer1Controller->rawStickY < -60
                              || gPlayer1Controller->rawStickY > 60
                          ? GO_MENU_WAIT_FOR_NEUTRAL
                          : 0;
    sPauseCastleOptionsStick = openingStick;
    sMenuNavigationStick = openingStick;
    sPauseCastleOptionsFrame = gGlobalTimer;
}

s32 pause_camera_controls_allowed(void) {
    return gMenuMode == MENU_MODE_RENDER_PAUSE_SCREEN && !sPauseStatisticsOpen && !gOptionsMenu.open
           && gDialogBoxState != DIALOG_STATE_OPENING;
}

static void render_pause_castle_options(void) {
    const s16 centerX = pause_menu_center_x();
    s16 optionsX[3];
    u8 textContinue[] = { TEXT_CONTINUE };
    static const s8 options[] = { MENU_OPT_DEFAULT, MENU_OPT_STATISTICS, MENU_OPT_OPTIONS };
    s32 selected = 0;
    for (s32 row = 0; row < 3; row++) {
        if (options[row] == sPauseCastleSelection) {
            selected = row;
        }
    }
    u8 direction = 0;
    u8 pressed;
    if (sPauseCastleOptionsFrame != gGlobalTimer) {
        sPauseCastleOptionsFrame = gGlobalTimer;
        u8 stick = (gPlayer1Controller->rawStickY > 60 ? 1 : 0)
                   | (gPlayer1Controller->rawStickY < -60 ? 2 : 0)
                   | (gPlayer1Controller->rawStickX < -60 ? 4 : 0)
                   | (gPlayer1Controller->rawStickX > 60 ? 8 : 0);
        u8 rawStick = stick;
        stick = game_options_menu_stick(stick, &sPauseCastleOptionsStick);
        // Camera-speed chords keep their normal ownership on the main pause page.
        if (!(gPlayer1Controller->buttonDown & Z_TRIG)) {
            u16 dpad = game_options_menu_dpad(gPlayer1Controller->buttonDown);
            u8 allowed = game_options_menu_direction(
                (game_options_get(GO_MENU_NAVIGATION) == 1 ? 0 : rawStick) | ((dpad & U_JPAD) ? 1 : 0)
                    | ((dpad & D_JPAD) ? 2 : 0) | ((dpad & L_JPAD) ? 4 : 0) | ((dpad & R_JPAD) ? 8 : 0),
                &sMenuNavigationDirection);
            direction = ((stick & 3) | ((dpad & U_JPAD) ? 1 : 0) | ((dpad & D_JPAD) ? 2 : 0)) & allowed;
        }
        pressed = (direction ^ sPauseCastleOptionsHeld) & direction;
        sPauseCastleOptionsHeld = direction;
        s8 previous = sPauseCastleSelection;
        if (pressed == 1) {
            selected = (selected + 2) % 3;
        } else if (pressed == 2) {
            selected = (selected + 1) % 3;
        }
        sPauseCastleSelection = options[selected];
        if (sPauseCastleSelection != previous) {
            play_sound(SOUND_MENU_PAUSE_CHANGE_SELECT, gGlobalSoundSource);
        }
    }
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    optionsX[0] = get_str_x_pos_from_center(centerX, LANGUAGE_ARRAY(textContinue), 1.0f);
    print_generic_string(optionsX[0], PAUSE_CASTLE_OPTIONS_Y, LANGUAGE_ARRAY(textContinue));
    optionsX[1] = player_stats_menu_text_centered(centerX, PAUSE_CASTLE_OPTIONS_Y - 15, "STATISTICS");
    optionsX[2] = player_stats_menu_text_centered(centerX, PAUSE_CASTLE_OPTIONS_Y - 30, "OPTIONS");
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    create_dl_translation_matrix(MENU_MTX_PUSH, optionsX[selected] - 14,
                                 PAUSE_CASTLE_OPTIONS_Y + 1 - selected * 15, 0);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

s32 render_pause_courses_and_castle(void) {
    s16 index = MENU_OPT_NONE;
    s32 closeRequested;
    s32 startResume;
    s32 justOpened = gDialogBoxState == DIALOG_STATE_OPENING;
    s32 ending = level_is_ending();
    s32 statisticsThisFrame = sPauseStatisticsOpen && !justOpened;
    s32 optionsThisFrame = gOptionsMenu.open && !justOpened;

        if (justOpened) {
            if (gOptionsMenu.open) {
                game_options_menu_close();
            }
            sPauseMenuOpenedFrame = gGlobalTimer;
            sPauseMenuStartRepressed = FALSE;
            sPauseMenuARepressed = FALSE;
            sPauseStatisticsOpen = FALSE;
            sPauseCastleSelection = MENU_OPT_DEFAULT;
            reset_pause_menu_navigation();
            gDialogLineNum = MENU_OPT_DEFAULT;
            level_set_transition(-1, NULL);
            play_sound(SOUND_MENU_PAUSE_OPEN, gGlobalSoundSource);

            if ((gCurrCourseNum >= COURSE_MIN && gCurrCourseNum < COURSE_CAKE_END)
                || gCurrLevelNum == LEVEL_PHYSICS_LAB || ending) {
                if (!ending) {
                    change_dialog_camera_angle();
                }
                gDialogBoxState = DIALOG_STATE_VERTICAL;
            } else {
                highlight_last_course_complete_stars();
                gDialogBoxState = DIALOG_STATE_HORIZONTAL;
            }
        }

        // Only presses after opening can arm a hold; gameplay and opening inputs cannot.
        if (!(gPlayer1Controller->buttonDown & START_BUTTON)) {
            sPauseMenuStartRepressed = FALSE;
        }
        if (!(gPlayer1Controller->buttonDown & A_BUTTON)) {
            sPauseMenuARepressed = FALSE;
        }
        if (!justOpened && gGlobalTimer != sPauseMenuOpenedFrame) {
            if (gPlayer1Controller->buttonPressed & START_BUTTON) {
                sPauseMenuStartRepressed = TRUE;
            }
            if (gPlayer1Controller->buttonPressed & A_BUTTON) {
                sPauseMenuARepressed = TRUE;
            }
        }
        startResume = sPauseMenuStartRepressed && (gPlayer1Controller->buttonDown & START_BUTTON);

        if (!justOpened && !statisticsThisFrame && !optionsThisFrame && !startResume
            && (u32) (gGlobalTimer - sPauseMenuOpenedFrame) >= PAUSE_MENU_UNPAUSE_DELAY_FRAMES
            && (gPlayer1Controller->buttonPressed & B_BUTTON)) {
            game_options_menu_open();
            optionsThisFrame = TRUE;
            sPauseMenuARepressed = FALSE;
            play_sound(SOUND_MENU_PAUSE_CHANGE_SELECT, gGlobalSoundSource);
        }

        // Extra render passes cannot advance the unpause delay.
        closeRequested =
            !justOpened
            && (u32) (gGlobalTimer - sPauseMenuOpenedFrame) >= PAUSE_MENU_UNPAUSE_DELAY_FRAMES
            && (startResume
                || (!statisticsThisFrame && !optionsThisFrame
                    && ((gPlayer1Controller->buttonPressed & A_BUTTON)
                        || (sPauseMenuARepressed && (gPlayer1Controller->buttonDown & A_BUTTON)))));
        gDialogTextAlpha = 250;

        if (optionsThisFrame) {
            shade_screen();
            if (!closeRequested && game_options_menu_update_controller()) {
                sPauseMenuARepressed = FALSE;
                reset_pause_menu_navigation();
            }
            game_options_menu_render();
            if (closeRequested) {
                game_options_menu_close();
                level_set_transition(0, NULL);
                play_sound(SOUND_MENU_PAUSE_CLOSE, gGlobalSoundSource);
                gDialogBoxState = DIALOG_STATE_OPENING;
                gMenuMode = MENU_MODE_NONE;
                index = MENU_OPT_DEFAULT;
            }
        } else if (statisticsThisFrame) {
            shade_screen();
            if (!closeRequested && player_stats_menu_update()) {
                sPauseStatisticsOpen = FALSE;
                sPauseMenuARepressed = FALSE;
                reset_pause_menu_navigation();
            }
            player_stats_menu_render();
            if (closeRequested) {
                level_set_transition(0, NULL);
                play_sound(SOUND_MENU_PAUSE_CLOSE, gGlobalSoundSource);
                sPauseStatisticsOpen = FALSE;
                gDialogBoxState = DIALOG_STATE_OPENING;
                gMenuMode = MENU_MODE_NONE;
                index = MENU_OPT_DEFAULT;
            }
        } else {
            switch (gDialogBoxState) {
                case DIALOG_STATE_VERTICAL:
                    shade_screen();
                    if (gCurrLevelNum == LEVEL_PHYSICS_LAB || ending) {
                        // The playground has no course/save entry to index for scores.
                        print_hud_pause_colorful_str();
                    } else {
                        render_pause_my_score_coins();
                        render_pause_red_coins();
                    }
                    render_pause_course_options(pause_menu_center_x(), 93, &gDialogLineNum, 15);

                    if (closeRequested && !startResume && gDialogLineNum == MENU_OPT_OPTIONS) {
                        game_options_menu_open();
                        optionsThisFrame = TRUE;
                        sPauseMenuARepressed = FALSE;
                        play_sound(SOUND_MENU_PAUSE_CHANGE_SELECT, gGlobalSoundSource);
                        break;
                    }
                    if (closeRequested && !startResume && gDialogLineNum == MENU_OPT_STATISTICS) {
                        player_stats_menu_open();
                        sPauseStatisticsOpen = TRUE;
                        sPauseMenuARepressed = FALSE;
                        play_sound(SOUND_MENU_PAUSE_CHANGE_SELECT, gGlobalSoundSource);
                        break;
                    }
                    if (closeRequested) {
                        level_set_transition(0, NULL);
                        play_sound(SOUND_MENU_PAUSE_CLOSE, gGlobalSoundSource);
                        gDialogBoxState = DIALOG_STATE_OPENING;
                        gMenuMode = MENU_MODE_NONE;

                        if (!startResume && gDialogLineNum == MENU_OPT_EXIT_COURSE) {
                            index = gDialogLineNum;
                        } else { // Start always resumes, including from Exit Course.
                            index = MENU_OPT_DEFAULT;
                        }

                        break;
                    }
                    break;

                case DIALOG_STATE_HORIZONTAL:
                    shade_screen();
                    print_hud_pause_colorful_str();
#ifdef INPUT_DISPLAY
                    render_pause_castle_menu_box(input_display_enabled() ? 216 : 160, 143);
                    render_pause_castle_main_strings(input_display_enabled() ? 160 : 104, 60);
#else
                    render_pause_castle_menu_box(160, 143);
                    render_pause_castle_main_strings(104, 60);
#endif
                    render_pause_castle_options();

                    if (closeRequested && !startResume && sPauseCastleSelection == MENU_OPT_OPTIONS) {
                        game_options_menu_open();
                        optionsThisFrame = TRUE;
                        sPauseMenuARepressed = FALSE;
                        play_sound(SOUND_MENU_PAUSE_CHANGE_SELECT, gGlobalSoundSource);
                        break;
                    }
                    if (closeRequested && !startResume
                        && sPauseCastleSelection == MENU_OPT_STATISTICS) {
                        player_stats_menu_open();
                        sPauseStatisticsOpen = TRUE;
                        sPauseMenuARepressed = FALSE;
                        play_sound(SOUND_MENU_PAUSE_CHANGE_SELECT, gGlobalSoundSource);
                        break;
                    }
                    if (closeRequested) {
                        level_set_transition(0, NULL);
                        play_sound(SOUND_MENU_PAUSE_CLOSE, gGlobalSoundSource);
                        gMenuMode = MENU_MODE_NONE;
                        gDialogBoxState = DIALOG_STATE_OPENING;

                        index = MENU_OPT_DEFAULT;
                        break;
                    }
                    break;
            }
        }
        // Keep the closing statistics image intact; its L/R controls own this subpage.
        if (!statisticsThisFrame && !optionsThisFrame) {
#ifdef INPUT_DISPLAY
            s16 hintX = input_display_enabled() ? INPUT_DISPLAY_CONTENT_LEFT : 24;
#else
            s16 hintX = 24;
#endif
            const char *hint = NULL;
            if (gOptionsMenu.saveFailed) {
                hint = "Settings not saved - B: Retry";
            }
            if (hint != NULL) {
                // One row below the HUD and above the course heading; bottom rows
                // already belong to aspect/camera settings and their shortcuts.
                gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
                gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
                player_stats_menu_text(hintX, 184, hint);
                gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
            }
#ifdef WIDE
            render_widescreen_setting(!closeRequested);
#endif
#ifdef REONUCAM
            render_reonucam_speed_setting(!closeRequested);
#endif
        }
    return index;
}

#define TXT_HISCORE_X 109
#define TXT_HISCORE_Y  36
#define TXT_CONGRATS_X 70
#define TXT_CONGRATS_Y 67

enum HUDCourseCompleteStringIDs {
    HUD_PRINT_HISCORE,
    HUD_PRINT_CONGRATULATIONS
};

void print_hud_course_complete_string(s8 str) {
    u8 textHiScore[] = { TEXT_HUD_HI_SCORE };
    u8 textCongratulations[] = { TEXT_HUD_CONGRATULATIONS };

    u8 colorFade = sins(gDialogColorFadeTimer) * 50.0f + 200.0f;

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, colorFade, colorFade, colorFade, 255);

    if (str == HUD_PRINT_HISCORE) {
        print_hud_lut_string(HUD_LUT_GLOBAL, TXT_HISCORE_X,  TXT_HISCORE_Y,  LANGUAGE_ARRAY(textHiScore));
    } else { // HUD_PRINT_CONGRATULATIONS
        print_hud_lut_string(HUD_LUT_GLOBAL, TXT_CONGRATS_X, TXT_CONGRATS_Y, LANGUAGE_ARRAY(textCongratulations));
    }

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
}

void print_hud_course_complete_coins(s16 x, s16 y) {
    u8 courseCompleteCoinsStr[4];
    u8 hudTextSymCoin[] = { GLYPH_COIN, GLYPH_SPACE };
    u8 hudTextSymX[] = { GLYPH_MULTIPLY, GLYPH_SPACE };

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);

    print_hud_lut_string(HUD_LUT_GLOBAL, x +  0, y, hudTextSymCoin);
    print_hud_lut_string(HUD_LUT_GLOBAL, x + 16, y, hudTextSymX);

    int_to_str(gCourseCompleteCoins, courseCompleteCoinsStr);
    print_hud_lut_string(HUD_LUT_GLOBAL, x + 32, y, courseCompleteCoinsStr);

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    if (gCourseCompleteCoins >= gHudDisplay.coins) {
        gCourseCompleteCoinsEqual = TRUE;
        gCourseCompleteCoins = gHudDisplay.coins;

        if (gGotFileCoinHiScore) {
            print_hud_course_complete_string(HUD_PRINT_HISCORE);
        }
    } else {
        if ((gCourseDoneMenuTimer & 1) || gHudDisplay.coins > 70) {
            gCourseCompleteCoins++;
            play_sound(SOUND_MENU_YOSHI_GAIN_LIVES, gGlobalSoundSource);

#ifdef ENABLE_LIVES
            if (gCourseCompleteCoins && ((gCourseCompleteCoins % 50) == 0)) {
                play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
                gMarioState->numLives++;
            }
#endif
        }

        if ((gHudDisplay.coins == gCourseCompleteCoins) && gGotFileCoinHiScore) {
            play_sound(SOUND_MENU_HIGH_SCORE, gGlobalSoundSource);
        }
    }
}

void play_star_fanfare_and_flash_hud(s32 arg, u8 starNum) {
    if (gHudDisplay.coins == gCourseCompleteCoins && (gCurrCourseStarFlags & starNum) == 0 && gHudFlash == HUD_FLASH_NONE) {
        play_star_fanfare();
        gHudFlash = arg;
    }
}

#define TXT_NAME_X1 71
#define TXT_NAME_X2 TXT_NAME_X1 - 2
#define CRS_NUM_X2 104
#define CRS_NUM_X3 CRS_NUM_X2 - 2
#define TXT_CLEAR_X1 get_string_width(name) + 81
#define TXT_CLEAR_X2 TXT_CLEAR_X1 - 2

void render_course_complete_lvl_info_and_hud_str(void) {
    u8 textCourse[] = { TEXT_COURSE };
    u8 textClear[] = { TEXT_CLEAR };
    u8 textSymStar[] = { GLYPH_STAR, GLYPH_SPACE };

    u8 *name;

    u8 strCourseNum[4];

    void **actNameTbl    = segmented_to_virtual(languageTable[gInGameLanguage][2]);
    void **courseNameTbl = segmented_to_virtual(languageTable[gInGameLanguage][1]);

    if (gLastCompletedCourseNum == COURSE_NONE || gLastCompletedStarNum == 0) {
        // Debug arrivals can enter a star-exit warp without having collected a star.
        static u8 textBrokenRules[] = { TEXT_YOU_BROKE_THE_RULES };
        name = textBrokenRules;
        print_hud_course_complete_coins(118, 103);
    } else if (gStarModelLastCollected == MODEL_BOWSER_KEY) { // Bowser courses
        name = segmented_to_virtual(courseNameTbl[COURSE_NUM_TO_INDEX(gLastCompletedCourseNum)]);

        // Print course name and clear text
        gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);

        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, gDialogTextAlpha);
        print_generic_string(TXT_NAME_X1, 130, name);
        print_generic_string(TXT_CLEAR_X1, 130, textClear);
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
        print_generic_string(TXT_NAME_X2, 132, name);
        print_generic_string(TXT_CLEAR_X2, 132, textClear);
        gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

        print_hud_course_complete_string(HUD_PRINT_CONGRATULATIONS);
        print_hud_course_complete_coins(118, 111);
        play_star_fanfare_and_flash_hud(HUD_FLASH_KEYS, 0);
        return;
    } else if (gLastCompletedCourseNum <= COURSE_STAGES_MAX) { // Main courses
        print_hud_course_complete_coins(118, 103);
        play_star_fanfare_and_flash_hud(HUD_FLASH_STARS, (1 << (gLastCompletedStarNum - 1)));

        if (gLastCompletedStarNum == 7) {
            name = segmented_to_virtual(actNameTbl[COURSE_STAGES_MAX * 6 + 1]);
        } else {
            name = segmented_to_virtual(actNameTbl[COURSE_NUM_TO_INDEX(gLastCompletedCourseNum) * 6 + gLastCompletedStarNum - 1]);
        }

        // Print course number
        gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);

        int_to_str(gLastCompletedCourseNum, strCourseNum);

        gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, gDialogTextAlpha);
        print_generic_string(65, 165, LANGUAGE_ARRAY(textCourse));
        print_generic_string(CRS_NUM_X2, 165, strCourseNum);

        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
        print_generic_string(63, 167, LANGUAGE_ARRAY(textCourse));
        print_generic_string(CRS_NUM_X3, 167, strCourseNum);

        gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
    } else { // Castle secret stars
        name = segmented_to_virtual(actNameTbl[COURSE_STAGES_MAX * 6]);

        print_hud_course_complete_coins(118, 103);
        play_star_fanfare_and_flash_hud(HUD_FLASH_STARS, 1 << (gLastCompletedStarNum - 1));
    }

    // Print star glyph
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);

    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    print_hud_lut_string(HUD_LUT_GLOBAL, 55, 77, textSymStar);

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    // Print act name and catch text
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);

    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, gDialogTextAlpha);
    print_generic_string(76, 145, name);

    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    print_generic_string(74, 147, name);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

#define X_VAL9 x
#define TXT_SAVEOPTIONS_X x + 12
#define TXT_SAVECONT_Y 0
#define TXT_SAVEQUIT_Y 20
#define TXT_CONTNOSAVE_Y 40

void render_save_confirmation(s16 x, s16 y, s8 *index, s16 yPos) {
    u8 textSaveAndContinue[] = { TEXT_SAVE_AND_CONTINUE };
    u8 textSaveAndQuit[] = { TEXT_SAVE_AND_QUIT };
    u8 textContinueWithoutSave[] = { TEXT_CONTINUE_WITHOUT_SAVING };

    handle_menu_scrolling(MENU_SCROLL_VERTICAL, index, 1, 3);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);

    print_generic_string(TXT_SAVEOPTIONS_X, y + TXT_SAVECONT_Y, LANGUAGE_ARRAY(textSaveAndContinue));
    print_generic_string(TXT_SAVEOPTIONS_X, y - TXT_SAVEQUIT_Y, LANGUAGE_ARRAY(textSaveAndQuit));
    print_generic_string(TXT_SAVEOPTIONS_X, y - TXT_CONTNOSAVE_Y, LANGUAGE_ARRAY(textContinueWithoutSave));

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

    create_dl_translation_matrix(MENU_MTX_PUSH, X_VAL9, y - ((*index - 1) * yPos), 0);

    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, gDialogTextAlpha);
    gSPDisplayList(gDisplayListHead++, dl_draw_triangle);

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
}

s32 render_course_complete_screen(void) {
    switch (gDialogBoxState) {
        case DIALOG_STATE_OPENING:
            render_course_complete_lvl_info_and_hud_str();
            if (gCourseDoneMenuTimer > 100 && gCourseCompleteCoinsEqual) {
                gDialogBoxState = DIALOG_STATE_VERTICAL;
                level_set_transition(-1, NULL);
                gDialogTextAlpha = 0;
                gDialogLineNum = MENU_OPT_DEFAULT;
            }
            break;

        case DIALOG_STATE_VERTICAL:
            shade_screen();
            render_course_complete_lvl_info_and_hud_str();
            render_save_confirmation(100, 86, &gDialogLineNum, 20);

            if (gCourseDoneMenuTimer > 110
                && ((gDialogLineNum == MENU_OPT_DEFAULT && (gPlayer1Controller->buttonDown & A_BUTTON))
                    || (gPlayer1Controller->buttonPressed & (A_BUTTON | START_BUTTON)))) {
                level_set_transition(0, NULL);
                play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
                gDialogBoxState = DIALOG_STATE_OPENING;
                gMenuMode = MENU_MODE_NONE;
                gCourseDoneMenuTimer = 0;
                gCourseCompleteCoins = 0;
                gCourseCompleteCoinsEqual = FALSE;
                gHudFlash = HUD_FLASH_NONE;

                return gDialogLineNum;
            }
            break;
    }

    if (gDialogTextAlpha < 250) {
        gDialogTextAlpha += 25;
    }

    gCourseDoneMenuTimer++;

    return MENU_OPT_NONE;
}

s32 render_menus_and_dialogs(void) {
    s32 mode = MENU_OPT_NONE;

    create_dl_ortho_matrix();

    if (gMenuMode != MENU_MODE_NONE) {
        switch (gMenuMode) {
            case MENU_MODE_UNUSED_0:
                mode = render_pause_courses_and_castle();
                break;
            case MENU_MODE_RENDER_PAUSE_SCREEN:
                mode = render_pause_courses_and_castle();
                break;
            case MENU_MODE_RENDER_COURSE_COMPLETE_SCREEN:
                mode = render_course_complete_screen();
                break;
            case MENU_MODE_UNUSED_3:
                mode = render_course_complete_screen();
                break;
        }

        gDialogColorFadeTimer = (s16) gDialogColorFadeTimer + 0x1000;
    } else if (gDialogID != DIALOG_NONE) {
        // The Peach "Dear Mario" message needs to be repositioned separately
        if (gDialogID == DIALOG_020) {
            print_peach_letter_message();
            return mode;
        }

        render_dialog_entries();
        gDialogColorFadeTimer = (s16) gDialogColorFadeTimer + 0x1000;
    }

    return mode;
}
