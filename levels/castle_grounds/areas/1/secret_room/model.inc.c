static const Vtx castle_secret_facade_dl_vtx[] = {
    { { { -306, 2154, 551 }, 0, { 0, 990 }, { 255, 255, 255, 255 } } },
    { { { -165, 2154, 551 }, 0, { 697, 990 }, { 255, 255, 255, 255 } } },
    { { { -165, 3431, 503 }, 0, { 697, -5388 }, { 255, 255, 255, 255 } } },
    { { { -281, 3431, 503 }, 0, { 124, -5388 }, { 255, 255, 255, 255 } } },
    { { { 165, 2154, 551 }, 0, { 2330, 990 }, { 255, 255, 255, 255 } } },
    { { { 307, 2154, 551 }, 0, { 3032, 990 }, { 255, 255, 255, 255 } } },
    { { { 282, 3431, 503 }, 0, { 2908, -5388 }, { 255, 255, 255, 255 } } },
    { { { 165, 3431, 503 }, 0, { 2330, -5388 }, { 255, 255, 255, 255 } } },
    { { { -165, 2154, 551 }, 0, { 697, 990 }, { 255, 255, 255, 255 } } },
    { { { 165, 2154, 551 }, 0, { 2330, 990 }, { 255, 255, 255, 255 } } },
    { { { 165, 2913, 522 }, 0, { 2330, -2801 }, { 255, 255, 255, 255 } } },
    { { { -165, 2913, 522 }, 0, { 697, -2801 }, { 255, 255, 255, 255 } } },
    { { { -165, 3253, 510 }, 0, { 697, -4499 }, { 255, 255, 255, 255 } } },
    { { { 165, 3253, 510 }, 0, { 2330, -4499 }, { 255, 255, 255, 255 } } },
    { { { 165, 3431, 503 }, 0, { 2330, -5388 }, { 255, 255, 255, 255 } } },
    { { { -165, 3431, 503 }, 0, { 697, -5388 }, { 255, 255, 255, 255 } } },
};

const Gfx castle_secret_facade_dl[] = {
    gsSPVertex(castle_secret_facade_dl_vtx + 0, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPVertex(castle_secret_facade_dl_vtx + 4, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPVertex(castle_secret_facade_dl_vtx + 8, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPVertex(castle_secret_facade_dl_vtx + 12, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};

static const Vtx castle_secret_window_stone_dl_vtx[] = {
    { { { -165, 0, 19 }, 0, { 697, -2801 }, { 255, 255, 255, 255 } } },
    { { { 165, 0, 19 }, 0, { 2330, -2801 }, { 255, 255, 255, 255 } } },
    { { { 165, 340, 7 }, 0, { 2330, -4499 }, { 255, 255, 255, 255 } } },
    { { { -165, 340, 7 }, 0, { 697, -4499 }, { 255, 255, 255, 255 } } },
};

const Gfx castle_secret_window_stone_dl[] = {
    gsSPVertex(castle_secret_window_stone_dl_vtx + 0, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};

static const Vtx castle_secret_window_panel_dl_vtx[] = {
    { { { 158, 7, 29 }, 0, { 0, 990 }, { 196, 196, 208, 255 } } },
    { { { 158, 314, 18 }, 0, { 0, 0 }, { 196, 196, 208, 255 } } },
    { { { -148, 314, 18 }, 0, { 990, 0 }, { 196, 196, 208, 255 } } },
    { { { -148, 7, 29 }, 0, { 990, 990 }, { 196, 196, 208, 255 } } },
};

const Gfx castle_secret_window_panel_dl[] = {
    gsSPVertex(castle_secret_window_panel_dl_vtx + 0, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};

const Gfx castle_secret_window_dl[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, outside_09001000),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP, 0, 0, G_TX_WRAP, 0, 0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 2047, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP, 5, 0, G_TX_WRAP, 6,
                0),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(castle_secret_window_stone_dl),
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, outside_0900A800),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP, 0, 0, G_TX_WRAP, 0, 0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 1023, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, 0, G_TX_CLAMP, 5,
                0),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPDisplayList(castle_secret_window_panel_dl),
    gsSPTexture(0, 0, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

static const Vtx castle_secret_room_shell_dl_vtx[] = {
    { { { 245, 0, 0 }, 0, { 0, 0 }, { 76, 43, 29, 255 } } },
    { { { 245, 0, -700 }, 0, { 0, 0 }, { 76, 43, 29, 255 } } },
    { { { -245, 0, -700 }, 0, { 0, 0 }, { 76, 43, 29, 255 } } },
    { { { -245, 0, 0 }, 0, { 0, 0 }, { 76, 43, 29, 255 } } },
    { { { 165, 0, 19 }, 0, { 0, 0 }, { 76, 43, 29, 255 } } },
    { { { 165, 0, 0 }, 0, { 0, 0 }, { 76, 43, 29, 255 } } },
    { { { -165, 0, 0 }, 0, { 0, 0 }, { 76, 43, 29, 255 } } },
    { { { -165, 0, 19 }, 0, { 0, 0 }, { 76, 43, 29, 255 } } },
    { { { -245, 420, -700 }, 0, { 0, 0 }, { 48, 36, 55, 255 } } },
    { { { 245, 420, -700 }, 0, { 0, 0 }, { 48, 36, 55, 255 } } },
    { { { 245, 420, 0 }, 0, { 0, 0 }, { 48, 36, 55, 255 } } },
    { { { -245, 420, 0 }, 0, { 0, 0 }, { 48, 36, 55, 255 } } },
    { { { -245, 0, -700 }, 0, { 0, 0 }, { 113, 87, 127, 255 } } },
    { { { -245, 420, -700 }, 0, { 0, 0 }, { 113, 87, 127, 255 } } },
    { { { -245, 420, 0 }, 0, { 0, 0 }, { 113, 87, 127, 255 } } },
    { { { -245, 0, 0 }, 0, { 0, 0 }, { 113, 87, 127, 255 } } },
    { { { 245, 0, 0 }, 0, { 0, 0 }, { 96, 70, 111, 255 } } },
    { { { 245, 420, 0 }, 0, { 0, 0 }, { 96, 70, 111, 255 } } },
    { { { 245, 420, -700 }, 0, { 0, 0 }, { 96, 70, 111, 255 } } },
    { { { 245, 0, -700 }, 0, { 0, 0 }, { 96, 70, 111, 255 } } },
    { { { 245, 0, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { 245, 420, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { -245, 420, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { -245, 0, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
};

const Gfx castle_secret_room_shell_dl[] = {
    gsSPVertex(castle_secret_room_shell_dl_vtx + 0, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPVertex(castle_secret_room_shell_dl_vtx + 4, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPVertex(castle_secret_room_shell_dl_vtx + 8, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPVertex(castle_secret_room_shell_dl_vtx + 12, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPVertex(castle_secret_room_shell_dl_vtx + 16, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};

static const Gfx castle_secret_back_wall_dl[] = {
    gsSPVertex(castle_secret_room_shell_dl_vtx + 20, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};

static const Vtx castle_secret_painting_aperture_vtx[] = {
    { { { -114, 0, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { -114, 420, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { -245, 420, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { -245, 0, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { 245, 0, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { 245, 420, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { 114, 420, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { 114, 0, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { 114, 0, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { 114, 124, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { -114, 124, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { -114, 0, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { 114, 352, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { 114, 420, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { -114, 420, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
    { { { -114, 352, -700 }, 0, { 0, 0 }, { 123, 92, 133, 255 } } },
};
static const Gfx castle_secret_painting_aperture_dl[] = {
    gsSPVertex(castle_secret_painting_aperture_vtx + 0, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPVertex(castle_secret_painting_aperture_vtx + 4, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPVertex(castle_secret_painting_aperture_vtx + 8, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPVertex(castle_secret_painting_aperture_vtx + 12, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};

static const Vtx castle_secret_portrait_frame_dl_vtx[] = {
    { { { -130, 108, -697 }, 0, { 0, 0 }, { 177, 122, 40, 255 } } },
    { { { 130, 108, -697 }, 0, { 0, 0 }, { 177, 122, 40, 255 } } },
    { { { 130, 368, -697 }, 0, { 0, 0 }, { 220, 175, 69, 255 } } },
    { { { -130, 368, -697 }, 0, { 0, 0 }, { 220, 175, 69, 255 } } },
    { { { -114, 124, -697 }, 0, { 0, 0 }, { 180, 125, 42, 255 } } },
    { { { 114, 124, -697 }, 0, { 0, 0 }, { 180, 125, 42, 255 } } },
    { { { 114, 352, -697 }, 0, { 0, 0 }, { 217, 172, 67, 255 } } },
    { { { -114, 352, -697 }, 0, { 0, 0 }, { 217, 172, 67, 255 } } },
};

const Gfx castle_secret_portrait_frame_dl[] = {
    gsSPVertex(castle_secret_portrait_frame_dl_vtx + 0, 8, 0),
    gsSP2Triangles(0, 1, 5, 0, 0, 5, 4, 0),
    gsSP2Triangles(1, 2, 6, 0, 1, 6, 5, 0),
    gsSP2Triangles(2, 3, 7, 0, 2, 7, 6, 0),
    gsSP2Triangles(3, 0, 4, 0, 3, 4, 7, 0),
    gsSPEndDisplayList(),
};

ALIGNED8 static const Texture castle_secret_bow_portrait[] = {
#include "levels/castle_grounds/smug_bow_top.rgba16.inc.c"
};
ALIGNED8 static const Texture castle_secret_bow_portrait_bottom[] = {
#include "levels/castle_grounds/smug_bow_bottom.rgba16.inc.c"
};

static const Vtx castle_secret_portrait_dl_vtx[] = {
    { { { -114, 238, -695 }, 0, { 0, 992 }, { 255, 255, 255, 255 } } },
    { { { 114, 238, -695 }, 0, { 2016, 992 }, { 255, 255, 255, 255 } } },
    { { { 114, 352, -695 }, 0, { 2016, 0 }, { 255, 255, 255, 255 } } },
    { { { -114, 352, -695 }, 0, { 0, 0 }, { 255, 255, 255, 255 } } },
};

const Gfx castle_secret_portrait_dl[] = {
    gsSPVertex(castle_secret_portrait_dl_vtx + 0, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};

static const Vtx castle_secret_portrait_bottom_dl_vtx[] = {
    { { { -114, 124, -695 }, 0, { 0, 992 }, { 255, 255, 255, 255 } } },
    { { { 114, 124, -695 }, 0, { 2016, 992 }, { 255, 255, 255, 255 } } },
    { { { 114, 238, -695 }, 0, { 2016, 0 }, { 255, 255, 255, 255 } } },
    { { { -114, 238, -695 }, 0, { 0, 0 }, { 255, 255, 255, 255 } } },
};

const Gfx castle_secret_portrait_bottom_dl[] = {
    gsSPVertex(castle_secret_portrait_bottom_dl_vtx + 0, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};

const Gfx castle_secret_room_dl[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(castle_secret_room_shell_dl),
    gsSPTexture(0, 0, 0, G_TX_RENDERTILE, G_OFF),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

ALIGNED8 static const Texture castle_secret_regular_bow_portrait[] = {
#include "levels/castle_grounds/bow_top.rgba16.inc.c"
};
ALIGNED8 static const Texture castle_secret_regular_bow_portrait_bottom[] = {
#include "levels/castle_grounds/bow_bottom.rgba16.inc.c"
};

ALIGNED8 static const Texture castle_secret_lab_portrait[] = {
#include "levels/castle_grounds/physics_lab_portrait_top.rgba16.inc.c"
};
ALIGNED8 static const Texture castle_secret_lab_portrait_bottom[] = {
#include "levels/castle_grounds/physics_lab_portrait_bottom.rgba16.inc.c"
};

const Gfx castle_secret_smug_bow_dl[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(castle_secret_back_wall_dl),
    gsSPDisplayList(castle_secret_portrait_frame_dl),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, castle_secret_bow_portrait),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP, 0, 0, G_TX_WRAP, 0, 0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 2047, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, 0, G_TX_CLAMP, 6,
                0),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(castle_secret_portrait_dl),
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, castle_secret_bow_portrait_bottom),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP, 0, 0, G_TX_WRAP, 0, 0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 2047, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, 0, G_TX_CLAMP, 6,
                0),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(castle_secret_portrait_bottom_dl),
    gsSPTexture(0, 0, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

const Gfx castle_secret_bow_dl[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(castle_secret_back_wall_dl),
    gsSPDisplayList(castle_secret_portrait_frame_dl),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, castle_secret_regular_bow_portrait),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP, 0, 0, G_TX_WRAP, 0, 0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 2047, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, 0, G_TX_CLAMP, 6,
                0),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(castle_secret_portrait_dl),
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, castle_secret_regular_bow_portrait_bottom),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP, 0, 0, G_TX_WRAP, 0, 0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 2047, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, 0, G_TX_CLAMP, 6,
                0),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(castle_secret_portrait_bottom_dl),
    gsSPTexture(0, 0, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

const Gfx castle_secret_physics_lab_dl[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(castle_secret_painting_aperture_dl),
    gsSPDisplayList(castle_secret_portrait_frame_dl),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

static const Vtx castle_secret_portrait_backing_vtx[] = {
    { { { -114, 124, -695 }, 0, { 0, 0 }, { 32, 17, 44, 255 } } },
    { { { 114, 124, -695 }, 0, { 0, 0 }, { 32, 17, 44, 255 } } },
    { { { 114, 352, -695 }, 0, { 0, 0 }, { 32, 17, 44, 255 } } },
    { { { -114, 352, -695 }, 0, { 0, 0 }, { 32, 17, 44, 255 } } },
};

const Gfx castle_secret_empty_frame_dl[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsSPTexture(0, 0, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(castle_secret_back_wall_dl),
    gsSPDisplayList(castle_secret_portrait_frame_dl),
    gsSPVertex(castle_secret_portrait_backing_vtx, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
