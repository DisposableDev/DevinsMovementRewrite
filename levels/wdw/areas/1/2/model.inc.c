// 0x07009B78 - 0x07009B90

// 0x07009B90 - 0x07009C70
static const Vtx wdw_seg7_vertex_07009B90[] = {
#include "assets/rom/generated/ca2bc8374b4317259713.inc.c"













};

// 0x07009C70 - 0x07009D50
static const Vtx wdw_seg7_vertex_07009C70[] = {
#include "assets/rom/generated/c01b7d29956b774aac1f.inc.c"













};

// 0x07009D50 - 0x07009E40
static const Vtx wdw_seg7_vertex_07009D50[] = {
#include "assets/rom/generated/9a5886ced57f919f9424.inc.c"














};

// 0x07009E40 - 0x07009EB0
static const Vtx wdw_seg7_vertex_07009E40[] = {
#include "assets/rom/generated/73864676b338aa8922ec.inc.c"






};

// 0x07009EB0 - 0x07009FB0
static const Vtx wdw_seg7_vertex_07009EB0[] = {
#include "assets/rom/generated/4fb9738048d7f7b9be11.inc.c"















};

// 0x07009FB0 - 0x0700A0D0
static const Gfx wdw_seg7_dl_07009FB0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09000000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x999999ff),
    gsSPVertex(wdw_seg7_vertex_07009B90, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10, 11,  8, 0x0),
    gsSP2Triangles(10, 12, 11, 0x0, 10, 13, 12, 0x0),
    gsSPVertex(wdw_seg7_vertex_07009C70, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10, 11,  8, 0x0),
    gsSP2Triangles( 8, 12,  9, 0x0,  8, 13, 12, 0x0),
    gsSPVertex(wdw_seg7_vertex_07009D50, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(wdw_seg7_vertex_07009E40, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  2,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700A0D0 - 0x0700A138
static const Gfx wdw_seg7_dl_0700A0D0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_0900A000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(wdw_seg7_vertex_07009EB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0, 15,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700A138 - 0x0700A1B0
const Gfx wdw_seg7_dl_0700A138[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(wdw_seg7_dl_07009FB0),
    gsSPDisplayList(wdw_seg7_dl_0700A0D0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
