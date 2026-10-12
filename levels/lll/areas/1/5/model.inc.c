// 0x07013DB8 - 0x07013EA8
static const Vtx lll_seg7_vertex_07013DB8[] = {
#include "assets/rom/generated/c8336c540d88fe16a0da.inc.c"














};

// 0x07013EA8 - 0x07013F98
static const Vtx lll_seg7_vertex_07013EA8[] = {
#include "assets/rom/generated/8867588fe922ea1a072a.inc.c"














};

// 0x07013F98 - 0x07014088
static const Vtx lll_seg7_vertex_07013F98[] = {
#include "assets/rom/generated/8d5f27ebf9bfd9a44ddd.inc.c"














};

// 0x07014088 - 0x07014178
static const Vtx lll_seg7_vertex_07014088[] = {
#include "assets/rom/generated/6b7630bd6cd444b8d6a2.inc.c"














};

// 0x07014178 - 0x07014278
static const Vtx lll_seg7_vertex_07014178[] = {
#include "assets/rom/generated/8a76e48dc47342a21676.inc.c"















};

// 0x07014278 - 0x07014378
static const Vtx lll_seg7_vertex_07014278[] = {
#include "assets/rom/generated/a4ee42ba21fd6d7c64fb.inc.c"















};

// 0x07014378 - 0x07014478
static const Vtx lll_seg7_vertex_07014378[] = {
#include "assets/rom/generated/c3f13a99ebea6ec45649.inc.c"















};

// 0x07014478 - 0x070144F8
static const Vtx lll_seg7_vertex_07014478[] = {
#include "assets/rom/generated/25619ae9fe82c16e8116.inc.c"







};

// 0x070144F8 - 0x070146B8
static const Gfx lll_seg7_dl_070144F8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fire_09007800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(lll_seg7_vertex_07013DB8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(lll_seg7_vertex_07013EA8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(lll_seg7_vertex_07013F98, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(lll_seg7_vertex_07014088, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(lll_seg7_vertex_07014178, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(lll_seg7_vertex_07014278, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x070146B8 - 0x07014788
static const Gfx lll_seg7_dl_070146B8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fire_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(lll_seg7_vertex_07014378, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  4,  7,  6, 0x0),
    gsSP2Triangles( 4,  8,  7, 0x0,  3,  5,  9, 0x0),
    gsSP2Triangles(10,  9,  0, 0x0, 10,  3,  9, 0x0),
    gsSP2Triangles( 9, 11,  0, 0x0,  0, 12, 13, 0x0),
    gsSP2Triangles( 0, 13, 10, 0x0,  0,  2, 12, 0x0),
    gsSP2Triangles( 0, 14,  1, 0x0, 10, 15,  3, 0x0),
    gsSPVertex(lll_seg7_vertex_07014478, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  1,  5,  4, 0x0),
    gsSP2Triangles( 1,  6,  5, 0x0,  2,  7,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07014788 - 0x07014818
const Gfx lll_seg7_dl_07014788[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(lll_seg7_dl_070144F8),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(lll_seg7_dl_070146B8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
