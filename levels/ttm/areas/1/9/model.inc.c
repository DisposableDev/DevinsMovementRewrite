// 0x0700C0E0 - 0x0700C0F8

// 0x0700C0F8 - 0x0700C110

// 0x0700C110 - 0x0700C128

// 0x0700C128 - 0x0700C228
static const Vtx ttm_seg7_vertex_0700C128[] = {
#include "assets/rom/generated/eabf07eaa82b81f035ab.inc.c"















};

// 0x0700C228 - 0x0700C2A8
static const Vtx ttm_seg7_vertex_0700C228[] = {
#include "assets/rom/generated/40cd83ba0161a7093cb1.inc.c"







};

// 0x0700C2A8 - 0x0700C2E8
static const Vtx ttm_seg7_vertex_0700C2A8[] = {
#include "assets/rom/generated/c463a011b35bbfeb6dc1.inc.c"



};

// 0x0700C2E8 - 0x0700C360
static const Gfx ttm_seg7_dl_0700C2E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09000000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ttm_seg7_vertex_0700C128, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0, 15,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700C360 - 0x0700C3D8
static const Gfx ttm_seg7_dl_0700C360[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(ttm_seg7_vertex_0700C228, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  3,  1, 0x0,  4,  1,  5, 0x0),
    gsSP2Triangles( 0,  2,  6, 0x0,  2,  7,  6, 0x0),
    gsSP2Triangles( 7,  4,  5, 0x0,  7,  5,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700C3D8 - 0x0700C408
static const Gfx ttm_seg7_dl_0700C3D8[] = {
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(ttm_seg7_vertex_0700C2A8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700C408 - 0x0700C488
const Gfx ttm_seg7_dl_0700C408[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ttm_seg7_dl_0700C2E8),
    gsSPDisplayList(ttm_seg7_dl_0700C360),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(ttm_seg7_dl_0700C3D8),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
