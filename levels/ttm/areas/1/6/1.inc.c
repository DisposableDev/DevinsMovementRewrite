// 0x0700B6D0 - 0x0700B6E8

// 0x0700B6E8 - 0x0700B7D8
static const Vtx ttm_seg7_vertex_0700B6E8[] = {
#include "assets/rom/generated/1251b53ea8183c2c61b3.inc.c"














};

// 0x0700B7D8 - 0x0700B8D8
static const Vtx ttm_seg7_vertex_0700B7D8[] = {
#include "assets/rom/generated/bafabadb1fdb70e54378.inc.c"















};

// 0x0700B8D8 - 0x0700B998
static const Vtx ttm_seg7_vertex_0700B8D8[] = {
#include "assets/rom/generated/ed42eb3eefaac37a5d8f.inc.c"











};

// 0x0700B998 - 0x0700BAD0
static const Gfx ttm_seg7_dl_0700B998[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09004800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(ttm_seg7_vertex_0700B6E8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12,  0, 13, 0x0, 12, 14,  0, 0x0),
    gsSP1Triangle( 0,  2, 13, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700B7D8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  8,  4,  9, 0x0),
    gsSP2Triangles( 8,  5,  4, 0x0,  4,  7,  9, 0x0),
    gsSP2Triangles(10, 11,  0, 0x0, 11,  1,  0, 0x0),
    gsSP2Triangles( 0,  2, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700B8D8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 2,  4,  3, 0x0,  2,  5,  4, 0x0),
    gsSP2Triangles( 2,  6,  5, 0x0,  4,  7,  3, 0x0),
    gsSP2Triangles( 4,  5,  8, 0x0,  3,  9, 10, 0x0),
    gsSP2Triangles( 3, 10,  0, 0x0,  3,  7,  9, 0x0),
    gsSP1Triangle( 0, 11,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700BAD0 - 0x0700BB30
const Gfx ttm_seg7_dl_0700BAD0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ttm_seg7_dl_0700B998),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
