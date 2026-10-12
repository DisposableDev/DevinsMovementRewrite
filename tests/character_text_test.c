/* Production character registry and text parsers; graphics calls record glyphs. */
#include <stdint.h>
#include "test_support.h"
typedef uint8_t u8;
typedef uint16_t u16;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
typedef u8 ColorRGBA[4];
typedef int Gfx;
typedef int Mtx;
typedef f32 Mat4[4][4];
#define TRUE 1
#define FALSE 0
#define UNUSED __attribute__((unused))
#define DIALOG_NONE -1
#define ASCII_TO_DIALOG(c) ((c) - 'a' + 0x24)
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define GEO_CONTEXT_RENDER 1
#include "character_definitions.inc"

struct MarioState {
    const struct MarioMovementProfile *movementProfile;
};
struct GraphNode {
    s32 type;
};
struct GraphNodeSwitchCase {
    struct GraphNode node;
    s16 selectedCase;
};
static u8 gDebugFastWalk;
static s32 gCurrSaveFileNum = 3;
static s16 gMenuMode = MENU_MODE_NONE;
static u8 sDisableThiccText;
static u8 game_options_get(enum GameOptionId id) {
    return id == GO_DISABLE_THICC_TEXT && sDisableThiccText;
}
static s32 sHeThiccFile;
static s32 save_file_is_he_thicc(s32 file) {
    return file == 2 && sHeThiccFile;
}
static s8 gDialogBoxState, gDialogBoxType, gLastDialogResponse, gLastDialogLineNum;
static s16 gDialogID, gDialogTextPos, gLastDialogPageStrPos, gDialogScrollOffsetY;
static s32 gDialogResponse, gDialogVariable;
static f32 gDialogBoxScale, gDialogBoxOpenTimer;
static enum MarioTextStyle sDialogTextStyle;
static struct TextLabel *sTextLabels[52];
static s32 sTextLabelsCount;
static u8 sTextLabelAlpha = 255;
static void *gEffectsMemoryPool;
static Gfx sDisplayList[4096], *gDisplayListHead;
static void *menu_hud_lut[256], *main_hud_lut[256], *menu_font_lut[256], *main_credits_font_lut[256];
static u8 sGlyphs[1024];
static int sGlyphCount, sColorCount, sRectCount, sLastRectPos;
static s32 sLastRectTranslucent;
static u8 sMinColorAlpha = 255;
static ColorRGBA sLastColor;
static f32 sAdvance;
static void record_glyph(u8 glyph) {
    sGlyphs[sGlyphCount++] = glyph;
}
static void record_color(u8 r, u8 g, u8 b, u8 a) {
    sColorCount++;
    sLastColor[0] = r;
    sLastColor[1] = g;
    sLastColor[2] = b;
    sLastColor[3] = a;
    if (a < sMinColorAlpha) {
        sMinColorAlpha = a;
    }
}
static void record_rect(u32 x, u32 y) {
}
#define gSPDisplayList(p, ...) ((void) (p))
#define gSPPopMatrix(p, ...) ((void) (p))
#define gDPPipeSync(p) ((void) (p))
#define gDPSetCycleType(p, ...) ((void) (p))
#define gDPSetAlphaCompare(p, ...) ((void) (p))
#define gDPSetCombineMode(p, ...) ((void) (p))
#define gDPSetRenderMode(p, ...) ((void) (p))
#define G_CYC_1CYCLE 1
#define G_AC_NONE 0
#define G_CC_FADEA 1
#define G_RM_AA_XLU_SURF 1
#define G_RM_AA_XLU_SURF2 1
#define gDPLoadSync(p) ((void) (p))
#define gDPLoadBlock(p, ...) ((void) (p))
#define gDPSetTile(p, ...) ((void) (p))
#define gDPTileSync(p) ((void) (p))
#define gDPSetTileSize(p, ...) ((void) (p))
#define gDPSetTextureImage(p, f, s, w, t) record_glyph((uintptr_t) (t))
#define gDPSetEnvColor(p, r, g, b, a) record_color(r, g, b, a)
#define gSPTextureRectangle(p, x, y, ...) record_rect(x, y)
#define gSPPerspNormalize(p, ...) ((void) (p))
#define gSPMatrix(p, ...) ((void) (p))
static void *segmented_to_virtual(const void *p) {
    return (void *) p;
}
static void render_generic_char(u8 glyph) {
    record_glyph(glyph);
}
static void create_dl_translation_matrix(s8 push, f32 x, f32 y, f32 z) {
    if (push == MENU_MTX_NOPUSH) {
        sAdvance += x;
    }
}
static void level_set_transition(s32 timer, void *fn) {
}
static void change_and_flash_dialog_text_color_lines(s8 mode, s8 line, u8 *custom) {
}
static void *mem_pool_alloc(void *pool, u32 size) {
    static struct TextLabel label;
    return &label;
}
static void *alloc_display_list(u32 size) {
    static Mtx matrix;
    return &matrix;
}
static void guOrtho(Mtx *m, f32 a, f32 b, f32 c, f32 d, f32 e, f32 f, f32 g) {
}
static void mem_pool_free(void *pool, void *p) {
}
static void add_glyph_texture(s8 glyph) {
    record_glyph(glyph);
}
static void render_textrect(s32 x, s32 y, s32 pos, s32 translucent) {
    sRectCount++;
    sLastRectPos = pos;
    sLastRectTranslucent = translucent;
}
#include "character_sources.inc"

static void clear_output(void) {
    sGlyphCount = sColorCount = sRectCount = 0;
    sMinColorAlpha = 255;
    sLastRectPos = -1;
    sAdvance = 0;
    gDisplayListHead = sDisplayList;
}
static void begin(int selection) {
    struct MarioState m;
    gMenuMode = MENU_MODE_NONE;
    sDisableThiccText = 0;
    mario_character_init();
    sHeThiccFile = selection & 1;
    mario_character_request(selection);
    mario_character_commit(&m, FALSE);
    reset_dialog_render_state();
    clear_output();
    for (int i = 0; i < 256; i++) {
        menu_hud_lut[i] = main_hud_lut[i] = menu_font_lut[i] = main_credits_font_lut[i] =
            (void *) (uintptr_t) i;
    }
}
static int expect_glyphs(const u8 *expected, int count) {
    EXPECT_EQ(sGlyphCount, count);
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(sGlyphs[i], expected[i]);
    }
    return 1;
}
static int pause_menu_literal_text(void) {
    // Mario Kick se red: name expansion, both spelling rules and the final d.
    u8 text[] = { 0x16, 0x24, 0x35, 0x2C, 0x32, 0x9E, 0x14, 0x2C, 0x26,
                  0x2E, 0x9E, 0x36, 0x28, 0x9E, 0x35, 0x28, 0x27, 0xFF };
    const u8 expected[] = { 0x16, 0x24, 0x35, 0x2C, 0x32, 0x14, 0x2C,
                            0x26, 0x2E, 0x36, 0x28, 0x35, 0x28, 0x27 };
    begin(0);
    s32 width = get_string_width(text);
    for (int choice = 0; choice < MARIO_CHARACTER_COUNT; choice++) {
        begin(choice);
        enum MarioTextStyle style = mario_character_text_style();
        create_dialog_box(1);
        gMenuMode = MENU_MODE_RENDER_PAUSE_SCREEN;
        EXPECT_EQ(mario_character_palette(), choice & 1);
        print_generic_string(0, 0, text);
        if (!expect_glyphs(expected, sizeof(expected))) {
            return 0;
        }
        EXPECT_EQ(get_string_width(text), width);
        EXPECT_EQ(get_str_x_pos_from_center(100, text, 1), 100 - width / 2);
        EXPECT_EQ(sDialogTextStyle, style);
        clear_output();
        print_menu_generic_string(0, 0, text);
        if (!expect_glyphs(expected, sizeof(expected))) {
            return 0;
        }
        gMenuMode = MENU_MODE_NONE;
        EXPECT_EQ(mario_character_text_style(), style);
        EXPECT_EQ(sDialogTextStyle, style);
    }
    return 1;
}

static int disable_thicc_text(void) {
    u8 text[] = { 0x16, 0x24, 0x35, 0x2C, 0x32, 0x9E, 0x26, 0x2E,
                  0x9E, 0x36, 0x28, 0x9E, 0x35, 0x28, 0x27, 0xFF };
    const u8 expected[] = { 0x16, 0x24, 0x35, 0x2C, 0x32, 0x26, 0x2E, 0x36, 0x28, 0x35, 0x28, 0x27 };
    begin(0);
    s32 width = get_string_width(text);
    for (int choice = 0; choice < MARIO_CHARACTER_COUNT; choice++) {
        begin(choice);
        const struct MarioMovementProfile *movement = mario_character_movement();
        sDisableThiccText = 1;
        EXPECT_EQ(mario_character_text_style(), MARIO_TEXT_STANDARD);
        EXPECT_EQ(mario_character_palette(), choice & 1);
        EXPECT_EQ(mario_character_movement() == movement, TRUE);
        print_generic_string(0, 0, text);
        if (!expect_glyphs(expected, sizeof(expected))) {
            return 0;
        }
        EXPECT_EQ(get_string_width(text), width);
        create_dialog_box(1);
        EXPECT_EQ(sDialogTextStyle, MARIO_TEXT_STANDARD);
        sDisableThiccText = 0;
        EXPECT_EQ(mario_character_text_style(),
                  (choice & 1) ? MARIO_TEXT_HE_THICC : MARIO_TEXT_STANDARD);
        EXPECT_EQ(sDialogTextStyle, MARIO_TEXT_STANDARD);
    }
    return 1;
}
static int spelling_cases(void) {
    const char *input = "Kick CK Ck cK ckk c-k c k ccKK Please SE Se sE see s-e s e ssee ckse";
    const char *expected = "Kicc CC Cc cC cck c-k c k ccCK Pleass SS Ss sS sse s-e s e ssse ccss";
    for (int choice = 0; choice < 4; choice++) {
        begin(choice);
        u8 previous = 0;
        for (int i = 0; input[i]; i++) {
            u8 glyph = mario_character_text_glyph(mario_character_text_style(), MARIO_TEXT_ASCII,
                                                  &previous, input[i], (const u8 *) input + i + 1);
            EXPECT_EQ(glyph, (choice & 1) ? expected[i] : input[i]);
        }
    }
    return 1;
}
static int encoded_renderers(void) {
    u8 text[] = { 0x14, 0x2c, 0x26, 0x2e, 0x36, 0x28, 0xff }; // Kickse
    u8 expected[] = { 0x14, 0x2c, 0x26, 0x26, 0x36, 0x36 };
    u8 upper[] = { 0x0c, 0x14, 0x1c, 0x0e, 0xff }, upperExpected[] = { 0x0c, 0x0c, 0x1c, 0x1c };
    for (int choice = 0; choice < 4; choice++) {
        begin(choice);
        const u8 *want = (choice & 1) ? expected : text;
        print_generic_string(0, 0, text);
        if (!expect_glyphs(want, 6)) {
            return 0;
        }
        EXPECT_EQ(sAdvance, 30);
        EXPECT_EQ(get_string_width(text), 30);
        EXPECT_EQ(get_str_x_pos_from_center(100, text, 1), 85);
        clear_output();
        print_menu_generic_string(0, 0, text);
        if (!expect_glyphs(want, 6)) {
            return 0;
        }
        clear_output();
        print_hud_lut_string(HUD_LUT_GLOBAL, 0, 0, upper);
        if (!expect_glyphs((choice & 1) ? upperExpected : upper, 4)) {
            return 0;
        }
        clear_output();
        print_credits_string(0, 0, upper);
        if (!expect_glyphs((choice & 1) ? upperExpected : upper, 4)) {
            return 0;
        }
        clear_output();
        print_hud_lut_string(HUD_LUT_JPMENU, 0, 0, upper);
        if (!expect_glyphs(upper, 4)) {
            return 0;
        }
        EXPECT_EQ(text[3], 0x2e); // Source and control IDs remain immutable.
        clear_output();
        u8 slash[] = { DIALOG_CHAR_FORWARD_SLASH, DIALOG_CHAR_TERMINATOR };
        print_generic_string(0, 0, slash);
        if (!expect_glyphs(slash, 1)) {
            return 0;
        }
        EXPECT_EQ(sAdvance, 6);
        EXPECT_EQ(get_string_width(slash), 6);
    }
    return 1;
}
static int color_and_controls(void) {
    u8 text[] = { 0x26, 0xdf, 0x26, 0, 1, 2, 3, 4, 5, 6,    0x2e, 0xfe, 0x2e, 0x26, 0x54, 0x2e, 0x36,
                  0xdf, 0x28, 0,    1, 2, 3, 4, 5, 6, 0x28, 0xfe, 0x28, 0x36, 0x54, 0x28, 0xff };
    u8 expected[] = { 0x26, 0x26, 0x2e, 0x26, 0x54, 0x2e, 0x36, 0x36, 0x28, 0x36, 0x54, 0x28 };
    begin(1);
    print_generic_string(0, 0, text);
    if (!expect_glyphs(expected, sizeof(expected))) {
        return 0;
    }
    EXPECT_EQ(sColorCount, 2);
    EXPECT_EQ(sLastColor[0], 0xe0);
    EXPECT_EQ(sLastColor[3], 0x56);
    clear_output();
    struct DialogEntry dialog = { .linesPerBox = 4, .str = text };
    gDialogBoxState = DIALOG_STATE_VERTICAL;
    handle_dialog_text_and_pages(0, &dialog, 0, MARIO_TEXT_HE_THICC);
    if (!expect_glyphs(expected, sizeof(expected))) {
        return 0;
    }
    EXPECT_EQ(gLastDialogPageStrPos, -1);
    EXPECT_EQ(gLastDialogLineNum, 3);
    // End of one independently printed string must not affect the next.
    clear_output();
    print_generic_string(0, 0, (u8[]){ 0x26, 0xff });
    print_generic_string(0, 0, (u8[]){ 0x2e, 0xff });
    print_generic_string(0, 0, (u8[]){ 0x36, 0xff });
    print_generic_string(0, 0, (u8[]){ 0x28, 0xff });
    return expect_glyphs((u8[]){ 0x26, 0x2e, 0x36, 0x28 }, 4);
}
static int dialogue_snapshot_and_pages(void) {
    u8 text[] = { 0x26, 0x2e, 0x36, 0x28, 0xfe, 0x26, 0x2e, 0x36, 0x28, 0xff };
    struct DialogEntry dialog = { .linesPerBox = 1, .str = text };
    for (int api = 0; api < 4; api++) {
        begin(1);
        switch (api) {
            case 0:
                create_dialog_box(7);
                break;
            case 1:
                create_dialog_box_with_var(7, 42);
                break;
            case 2:
                create_dialog_inverted_box(7);
                break;
            case 3:
                create_dialog_box_with_response(7);
                break;
        }
        struct MarioState m;
        sHeThiccFile = FALSE;
        mario_character_request(0);
        mario_character_commit(&m, FALSE);
        create_dialog_box(8);
        EXPECT_EQ(gDialogID, 7);
        EXPECT_EQ(sDialogTextStyle, MARIO_TEXT_HE_THICC);
        gDialogBoxState = DIALOG_STATE_VERTICAL;
        handle_dialog_text_and_pages(0, &dialog, 0, sDialogTextStyle);
        if (!expect_glyphs((u8[]){ 0x26, 0x26, 0x36, 0x36 }, 4)) {
            return 0;
        }
        EXPECT_EQ(gLastDialogPageStrPos, 5);
        gDialogTextPos = gLastDialogPageStrPos;
        clear_output();
        handle_dialog_text_and_pages(0, &dialog, 0, sDialogTextStyle);
        if (!expect_glyphs((u8[]){ 0x26, 0x26, 0x36, 0x36 }, 4)) {
            return 0;
        }
        EXPECT_EQ(gLastDialogPageStrPos, -1);
        EXPECT_EQ(gDialogID, 7);
        reset_dialog_render_state();
        EXPECT_EQ(sDialogTextStyle, MARIO_TEXT_STANDARD);
        create_dialog_box(8);
        EXPECT_EQ(sDialogTextStyle, MARIO_TEXT_STANDARD);
    }
    return 1;
}
static int ascii_labels_and_geo(void) {
    for (int choice = 0; choice < 4; choice++) {
        begin(choice);
        struct TextLabel label = { .length = 6, .buffer = "Kickse" };
        sTextLabels[0] = &label;
        sTextLabelsCount = 1;
        render_text_labels();
        if (!expect_glyphs((u8[]){ 20, 18, 12, (choice & 1) ? 12 : 20, 28, (choice & 1) ? 28 : 14 },
                           6)) {
            return 0;
        }
        EXPECT_EQ(sTextLabelsCount, 0);
        EXPECT_EQ(label.buffer[3], 'k');
        struct GraphNodeSwitchCase node = { .selectedCase = -1 };
        geo_switch_mario_character(0, &node.node, NULL);
        EXPECT_EQ(node.selectedCase, -1);
        geo_switch_mario_character(GEO_CONTEXT_RENDER, &node.node, NULL);
        EXPECT_EQ(node.selectedCase, choice);
        geo_switch_mario_palette(GEO_CONTEXT_RENDER, &node.node, NULL);
        EXPECT_EQ(node.selectedCase, choice & 1);
    }
    return 1;
}
static int live_appearance(void) {
    for (int palette = 0; palette < 2; palette++) {
        begin(palette);
        gDebugFastWalk = TRUE;
        const struct MarioMovementProfile *profile = mario_character_movement();
        enum MarioTextStyle style = mario_character_text_style();
        for (int cycle = 0; cycle < 3; cycle++) {
            for (int simple = 1; simple >= 0; simple--) {
                int choice = palette + 2 * simple;
                EXPECT_EQ(mario_character_request(choice), TRUE);
                mario_character_apply_appearance(FALSE);
                EXPECT_EQ(mario_character_active_index(), choice);
                struct GraphNodeSwitchCase node = { .selectedCase = -1 };
                geo_switch_mario_character(GEO_CONTEXT_RENDER, &node.node, NULL);
                EXPECT_EQ(node.selectedCase, choice);
                EXPECT_EQ(mario_character_palette(), palette);
                EXPECT_EQ(mario_character_movement() == profile, TRUE);
                EXPECT_EQ(mario_character_text_style(), style);
                EXPECT_EQ(gDebugFastWalk, TRUE);
                struct MarioState m;
                mario_character_commit(&m, FALSE);
                EXPECT_EQ(mario_character_active_index(), choice);
                EXPECT_EQ(gDebugFastWalk, TRUE);
            }
        }
        mario_character_request(palette + 2);
        mario_character_apply_appearance(TRUE);
        EXPECT_EQ(mario_character_active_index(), palette);
        // File loading may request a different palette; an appearance preview cannot apply it.
        sHeThiccFile = !palette;
        mario_character_load_file(sHeThiccFile ? 2 : 0);
        mario_character_apply_appearance(FALSE);
        EXPECT_EQ(mario_character_palette(), palette);
    }
    return 1;
}
static int final_d_words(void) {
    const char *input = "red END Dad's Dad' d2 2d dD odd... d-d d!";
    const char *expected = "re EN Dad's Da' d2 2 d od... - !";
    for (int choice = 0; choice < 4; choice++) {
        begin(choice);
        u8 previous = 0;
        int out = 0;
        const char *want = (choice & 1) ? expected : input;
        for (int i = 0; input[i]; i++) {
            u8 glyph = mario_character_text_glyph(mario_character_text_style(), MARIO_TEXT_ASCII,
                                                  &previous, input[i], (const u8 *) input + i + 1);
            if (glyph != MARIO_TEXT_SKIP) {
                EXPECT_EQ(glyph, want[out++]);
            }
        }
        EXPECT_EQ(out, strlen(want));
        u8 text[] = { 0x35, 0x28, 0x27, 0xff }; // red
        print_generic_string(0, 0, text);
        if (!expect_glyphs(text, (choice & 1) ? 2 : 3)) {
            return 0;
        }
        EXPECT_EQ(get_string_width(text), (choice & 1) ? 10 : 16);
        EXPECT_EQ(get_str_x_pos_from_center(100, text, 1), (choice & 1) ? 95 : 92);
        clear_output();
        print_menu_generic_string(0, 0, text);
        if (!expect_glyphs(text, (choice & 1) ? 2 : 3)) {
            return 0;
        }
        clear_output();
        print_hud_lut_string(HUD_LUT_GLOBAL, 0, 0, (u8[]){ 0x0e, 0x17, 0x0d, 0xff });
        if (!expect_glyphs((u8[]){ 0x0e, 0x17, 0x0d }, (choice & 1) ? 2 : 3)) {
            return 0;
        }
        clear_output();
        print_credits_string(0, 0, (u8[]){ 0x0e, 0x17, 0x0d, 0xff });
        if (!expect_glyphs((u8[]){ 0x0e, 0x17, 0x0d }, (choice & 1) ? 2 : 3)) {
            return 0;
        }
        clear_output();
        print_hud_lut_string(HUD_LUT_JPMENU, 0, 0, (u8[]){ 0x0d, 0xff });
        if (!expect_glyphs((u8[]){ 0x0d }, 1)) {
            return 0;
        }
        clear_output();
        sTextLabelsCount = 0;
        print_text_centered(100, 0, "red END dad's");
        EXPECT_EQ(sTextLabels[0]->length, 13);
        EXPECT_EQ(sTextLabels[0]->x, (choice & 1) ? 34 : 22);
        render_text_labels();
        EXPECT_EQ(sRectCount, (choice & 1) ? 8 : 10);
        EXPECT_EQ(sLastRectPos, (choice & 1) ? 10 : 12);
        clear_output();
        struct TextLabel unterminated = { .length = 3, .buffer = "ENDa" };
        sTextLabels[0] = &unterminated;
        sTextLabelsCount = 1;
        render_text_labels();
        EXPECT_EQ(sRectCount, (choice & 1) ? 2 : 3);
        EXPECT_EQ(sLastRectPos, (choice & 1) ? 1 : 2);
    }
    begin(1);
    u8 text[] = { 0x27, 0xdf, 0, 0, 0, 0, 0, 0, 0, 0, 0x28, 0x9e,
                  0x27, 0xdf, 0, 0, 0, 0, 0, 0, 0, 0, 0xff };
    print_generic_string(0, 0, text);
    if (!expect_glyphs((u8[]){ 0x27, 0x28 }, 2)) {
        return 0;
    }
    clear_output();
    u8 pages[] = { 0x35, 0x28, 0x27, 0xfe, 0x35, 0x28, 0x27, 0xff };
    struct DialogEntry dialog = { .linesPerBox = 1, .str = pages };
    gDialogBoxState = DIALOG_STATE_VERTICAL;
    handle_dialog_text_and_pages(0, &dialog, 0, MARIO_TEXT_HE_THICC);
    if (!expect_glyphs((u8[]){ 0x35, 0x28 }, 2)) {
        return 0;
    }
    EXPECT_EQ(gLastDialogPageStrPos, 4);
    gDialogTextPos = 4;
    clear_output();
    handle_dialog_text_and_pages(0, &dialog, 0, MARIO_TEXT_HE_THICC);
    if (!expect_glyphs((u8[]){ 0x35, 0x28 }, 2)) {
        return 0;
    }
    EXPECT_EQ(gLastDialogPageStrPos, -1);
    return 1;
}

static int hud_label_alpha(void) {
    begin(0);
    print_text_set_alpha(128);
    print_text(10, 10, "A");
    EXPECT_EQ(sTextLabels[0]->alpha, 128);
    print_text_set_alpha(255);
    render_text_labels();
    EXPECT_EQ(sMinColorAlpha, 128);
    EXPECT_EQ(sLastColor[3], 255);
    EXPECT_EQ(sLastRectTranslucent, TRUE);
    clear_output();
    print_text(10, 10, "A");
    EXPECT_EQ(sTextLabels[0]->alpha, 255);
    render_text_labels();
    EXPECT_EQ(sColorCount, 0);
    EXPECT_EQ(sLastRectTranslucent, FALSE);
    return 1;
}

static int name_replacement(void) {
    const u8 ascii[] = "Mario, MARIO! Mario's SuperMario marioland mario.";
    const char expected[] = "HE THICC, HE THICC! HE THICC's SuperMario marioland HE THICC.";
    u8 output[128];
    EXPECT_EQ(mario_character_text_name_size(MARIO_TEXT_HE_THICC, MARIO_TEXT_ASCII, ascii),
              sizeof(expected));
    const u8 *result =
        mario_character_text_replace_name(MARIO_TEXT_HE_THICC, MARIO_TEXT_ASCII, ascii, output);
    EXPECT_EQ(strcmp((const char *) result, expected), 0);
    EXPECT_EQ(strcmp((const char *) ascii, "Mario, MARIO! Mario's SuperMario marioland mario."), 0);
    EXPECT_EQ(mario_character_text_name_size(MARIO_TEXT_STANDARD, MARIO_TEXT_ASCII, ascii), 1);
    EXPECT_EQ(mario_character_text_replace_name(MARIO_TEXT_STANDARD, MARIO_TEXT_ASCII, ascii, output)
                  == ascii,
              TRUE);
    u8 name[] = { 0x16, 0x24, 0x35, 0x2C, 0x32, 0xFF };
    u8 named[] = { 0x11, 0x0E, 0x9E, 0x1D, 0x11, 0x12, 0x0C, 0x0C, 0xFF };
    u8 glyphs[] = { 0x11, 0x0E, 0x1D, 0x11, 0x12, 0x0C, 0x0C };
    begin(1);
    print_generic_string(0, 0, name);
    if (!expect_glyphs(glyphs, sizeof(glyphs))) {
        return 0;
    }
    s32 width = 0;
    for (s32 i = 0; i < 8; i++) {
        width += gDialogCharWidths[named[i]];
    }
    EXPECT_EQ(get_string_width(name), width);
    EXPECT_EQ(get_str_x_pos_from_center(100, name, 1), 100 - width / 2);
    clear_output();
    print_generic_string_styled(0, 0, name, MARIO_TEXT_STANDARD);
    if (!expect_glyphs(name, 5)) {
        return 0;
    }
    u8 pages[] = { 0x16, 0x24, 0x35, 0x2C, 0x32, 0xFE, 0x16, 0x24, 0x35, 0x2C, 0x32, 0xFF };
    struct DialogEntry dialog = { .linesPerBox = 1, .str = pages };
    clear_output();
    gDialogBoxState = DIALOG_STATE_VERTICAL;
    gDialogTextPos = 0;
    handle_dialog_text_and_pages(0, &dialog, 0, MARIO_TEXT_HE_THICC);
    EXPECT_EQ(gLastDialogPageStrPos, 9);
    if (!expect_glyphs(glyphs, sizeof(glyphs))) {
        return 0;
    }
    clear_output();
    gDialogTextPos = gLastDialogPageStrPos;
    handle_dialog_text_and_pages(0, &dialog, 0, MARIO_TEXT_HE_THICC);
    EXPECT_EQ(gLastDialogPageStrPos, -1);
    if (!expect_glyphs(glyphs, sizeof(glyphs))) {
        return 0;
    }
    u8 upper[] = { 0x16, 0x0A, 0x1B, 0x12, 0x18, 0xFF };
    clear_output();
    print_hud_lut_string(HUD_LUT_GLOBAL, 0, 0, upper);
    if (!expect_glyphs(glyphs, sizeof(glyphs))) {
        return 0;
    }
    clear_output();
    sTextLabelsCount = 0;
    print_text_centered(100, 0, "Mario");
    EXPECT_EQ(sTextLabels[0]->x, 52);
    EXPECT_EQ(sTextLabels[0]->length, 5);
    render_text_labels();
    EXPECT_EQ(sRectCount, 7);
    EXPECT_EQ(sLastRectPos, 7);
    return 1;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
#define RUN(name, fn)                                                                                  \
    if (test_selected("character." name))                                                              \
    test_report("character." name, fn())
    RUN("name_replacement", name_replacement);
    RUN("pause_menu_literal_text", pause_menu_literal_text);
    RUN("disable_thicc_text", disable_thicc_text);
    RUN("spelling_cases", spelling_cases);
    RUN("final_d_words", final_d_words);
    RUN("hud_label_alpha", hud_label_alpha);
    RUN("encoded_renderers", encoded_renderers);
    RUN("color_and_controls", color_and_controls);
    RUN("dialogue_snapshot_and_pages", dialogue_snapshot_and_pages);
    RUN("ascii_labels_and_geo", ascii_labels_and_geo);
    RUN("live_appearance", live_appearance);
    return test_finish();
}
