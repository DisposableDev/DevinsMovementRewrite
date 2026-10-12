// 0x07016B70 - 0x07016BF0
static const Vtx lll_seg7_vertex_07016B70[] = {
#include "assets/rom/generated/e4d3f78d0c1c6fd2e319.inc.c"







};

// 0x07016BF0 - 0x07016CE0
static const Vtx lll_seg7_vertex_07016BF0[] = {
#include "assets/rom/generated/436b97a89290b8982415.inc.c"














};

// 0x07016CE0 - 0x07016DD0
static const Vtx lll_seg7_vertex_07016CE0[] = {
#include "assets/rom/generated/02fda1ee32d51c6ce9e8.inc.c"














};

// 0x07016DD0 - 0x07016EC0
static const Vtx lll_seg7_vertex_07016DD0[] = {
#include "assets/rom/generated/da03a518355bf80e6304.inc.c"














};

// 0x07016EC0 - 0x07016FB0
static const Vtx lll_seg7_vertex_07016EC0[] = {
#include "assets/rom/generated/2f99148658db432a1161.inc.c"














};

// 0x07016FB0 - 0x070170A0
static const Vtx lll_seg7_vertex_07016FB0[] = {
#include "assets/rom/generated/1b8b35b2a85be217e8be.inc.c"














};

// 0x070170A0 - 0x07017190
static const Vtx lll_seg7_vertex_070170A0[] = {
#include "assets/rom/generated/296005073239b5c4397b.inc.c"














};

// 0x07017190 - 0x07017250
static const Vtx lll_seg7_vertex_07017190[] = {
#include "assets/rom/generated/0ce62a6b185e65cf7cbb.inc.c"











};

// 0x07017250 - 0x070172B8
static const Gfx lll_seg7_dl_07017250[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fire_09004800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(lll_seg7_vertex_07016B70, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  0,  5,  6, 0x0),
    gsSP2Triangles( 0,  7,  5, 0x0,  0,  6,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x070172B8 - 0x07017358
static const Gfx lll_seg7_dl_070172B8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fire_09007800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(lll_seg7_vertex_07016BF0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(lll_seg7_vertex_07016CE0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x07017358 - 0x070174E0
static const Gfx lll_seg7_dl_07017358[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lll_seg7_texture_07002000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(lll_seg7_vertex_07016DD0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(lll_seg7_vertex_07016EC0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(lll_seg7_vertex_07016FB0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(lll_seg7_vertex_070170A0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  5,  4,  6, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  6,  7,  9, 0x0),
    gsSP2Triangles( 7, 10, 11, 0x0,  7, 11,  8, 0x0),
    gsSP2Triangles( 6,  9,  5, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(lll_seg7_vertex_07017190, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  5,  1, 0x0,  2,  7,  8, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  7, 10, 11, 0x0),
    gsSP2Triangles( 7, 11,  8, 0x0,  2,  8,  0, 0x0),
    gsSP1Triangle( 0,  6,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x070174E0 - 0x07017578
const Gfx lll_seg7_dl_070174E0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(lll_seg7_dl_07017250),
    gsSPDisplayList(lll_seg7_dl_07017358),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(lll_seg7_dl_070172B8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
