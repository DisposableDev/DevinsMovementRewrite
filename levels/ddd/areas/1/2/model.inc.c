// 0x07004DA8 - 0x07004DC0

// 0x07004DC0 - 0x07004EC0
static const Vtx ddd_seg7_vertex_07004DC0[] = {
#include "assets/rom/generated/37d7e0858fb3ff38c20c.inc.c"















};

// 0x07004EC0 - 0x07004FA0
static const Vtx ddd_seg7_vertex_07004EC0[] = {
#include "assets/rom/generated/4b57214deff26be7c857.inc.c"













};

// 0x07004FA0 - 0x07005090
static const Vtx ddd_seg7_vertex_07004FA0[] = {
#include "assets/rom/generated/82974ad62aeb636d8044.inc.c"














};

// 0x07005090 - 0x07005190
static const Vtx ddd_seg7_vertex_07005090[] = {
#include "assets/rom/generated/dce242c71d70a3e713a7.inc.c"















};

// 0x07005190 - 0x07005270
static const Vtx ddd_seg7_vertex_07005190[] = {
#include "assets/rom/generated/f555bb1b832f49c9e6cc.inc.c"













};

// 0x07005270 - 0x07005350
static const Vtx ddd_seg7_vertex_07005270[] = {
#include "assets/rom/generated/5b68d0fe412ed741b73b.inc.c"













};

// 0x07005350 - 0x070053C0
static const Vtx ddd_seg7_vertex_07005350[] = {
#include "assets/rom/generated/fb710fabeacaafcfe955.inc.c"






};

// 0x070053C0 - 0x070054B0
static const Vtx ddd_seg7_vertex_070053C0[] = {
#include "assets/rom/generated/252c5ff0429de8f3ce60.inc.c"














};

// 0x070054B0 - 0x07005570
static const Vtx ddd_seg7_vertex_070054B0[] = {
#include "assets/rom/generated/78a236c991f2b2c356af.inc.c"











};

// 0x07005570 - 0x07005738
static const Gfx ddd_seg7_dl_07005570[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09007800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(ddd_seg7_vertex_07004DC0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11,  1,  0, 0x0),
    gsSP2Triangles( 0,  2, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ddd_seg7_vertex_07004EC0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(ddd_seg7_vertex_07004FA0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0,  1, 11, 12, 0x0),
    gsSP2Triangles( 1, 13, 11, 0x0,  0, 14,  1, 0x0),
    gsSPVertex(ddd_seg7_vertex_07005090, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0, 10, 11, 12, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(ddd_seg7_vertex_07005190, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 1,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(ddd_seg7_vertex_07005270, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(ddd_seg7_vertex_07005350, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP1Triangle( 4,  5,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x07005738 - 0x070057E8
static const Gfx ddd_seg7_dl_07005738[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09004800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ddd_seg7_vertex_070053C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 12, 10, 0x0, 13, 12,  9, 0x0),
    gsSP1Triangle(13, 14, 12, 0x0),
    gsSPVertex(ddd_seg7_vertex_070054B0, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  4, 0x0,  3,  5,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9,  2, 10, 0x0),
    gsSP1Triangle( 0,  2,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x070057E8 - 0x07005850
const Gfx ddd_seg7_dl_070057E8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ddd_seg7_dl_07005570),
    gsSPDisplayList(ddd_seg7_dl_07005738),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
