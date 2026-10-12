// 0x070379F8 - 0x07037A10

// 0x07037A10 - 0x07037B10
static const Vtx inside_castle_seg7_vertex_07037A10[] = {
#include "assets/rom/generated/82a11cacc00ef20b6c26.inc.c"















};

// 0x07037B10 - 0x07037BF8
static const Gfx inside_castle_seg7_dl_07037B10[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09008000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x5f5f5fff),
    gsSPVertex(inside_castle_seg7_vertex_07037A10, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  3, 0x0,  6,  7,  4, 0x0),
    gsSP2Triangles( 4,  8,  9, 0x0,  4,  1,  8, 0x0),
    gsSP2Triangles( 4,  2,  1, 0x0,  4, 10,  2, 0x0),
    gsSP2Triangles( 4,  7, 10, 0x0,  4,  9,  5, 0x0),
    gsSP2Triangles(11,  7,  6, 0x0, 11, 10,  7, 0x0),
    gsSP2Triangles(12, 10, 11, 0x0, 12,  2, 10, 0x0),
    gsSP2Triangles( 0,  2, 12, 0x0,  3,  5, 13, 0x0),
    gsSP2Triangles(14,  1,  0, 0x0, 14,  8,  1, 0x0),
    gsSP2Triangles( 9,  8, 14, 0x0, 13,  5,  9, 0x0),
    gsSP2Triangles(15,  9, 14, 0x0, 13,  9, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x07037BF8 - 0x07037C58
const Gfx inside_castle_seg7_dl_07037BF8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07037B10),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
