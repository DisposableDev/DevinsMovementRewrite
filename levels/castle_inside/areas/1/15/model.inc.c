// 0x070331C8 - 0x070331E0

// 0x070331E0 - 0x070331F8

// 0x070331F8 - 0x070332F8
static const Vtx inside_castle_seg7_vertex_070331F8[] = {
#include "assets/rom/generated/4415c7d3f7d3ba5d93f9.inc.c"















};

// 0x070332F8 - 0x070333F8
static const Vtx inside_castle_seg7_vertex_070332F8[] = {
#include "assets/rom/generated/296b82adfe27d24bf21c.inc.c"















};

// 0x070333F8 - 0x070334F8
static const Vtx inside_castle_seg7_vertex_070333F8[] = {
#include "assets/rom/generated/570cacfaf4fac4307105.inc.c"















};

// 0x070334F8 - 0x070335D8
static const Vtx inside_castle_seg7_vertex_070334F8[] = {
#include "assets/rom/generated/ddbb7e88601d7009758d.inc.c"













};

// 0x070335D8 - 0x070336C8
static const Vtx inside_castle_seg7_vertex_070335D8[] = {
#include "assets/rom/generated/9d97ff49d4f8f3acce2f.inc.c"














};

// 0x070336C8 - 0x07033748
static const Vtx inside_castle_seg7_vertex_070336C8[] = {
#include "assets/rom/generated/6c03b80cc37cdeb1ca57.inc.c"







};

// 0x07033748 - 0x07033838
static const Vtx inside_castle_seg7_vertex_07033748[] = {
#include "assets/rom/generated/2d350dc57a84b5caecd5.inc.c"














};

// 0x07033838 - 0x07033928
static const Vtx inside_castle_seg7_vertex_07033838[] = {
#include "assets/rom/generated/3ac84edb34456e23adac.inc.c"














};

// 0x07033928 - 0x07033A18
static const Vtx inside_castle_seg7_vertex_07033928[] = {
#include "assets/rom/generated/865f96911ae2cb8c60f6.inc.c"














};

// 0x07033A18 - 0x07033AF8
static const Vtx inside_castle_seg7_vertex_07033A18[] = {
#include "assets/rom/generated/2e1f6ef1cd069d3b68d8.inc.c"













};

// 0x07033AF8 - 0x07033BE8
static const Vtx inside_castle_seg7_vertex_07033AF8[] = {
#include "assets/rom/generated/d7728c62b630bc6a8fa1.inc.c"














};

// 0x07033BE8 - 0x07033CD8
static const Vtx inside_castle_seg7_vertex_07033BE8[] = {
#include "assets/rom/generated/6f8364e5cb21590cbf01.inc.c"














};

// 0x07033CD8 - 0x07033DC8
static const Vtx inside_castle_seg7_vertex_07033CD8[] = {
#include "assets/rom/generated/9bb5cf794fb39c3e3f84.inc.c"














};

// 0x07033DC8 - 0x07033EB8
static const Vtx inside_castle_seg7_vertex_07033DC8[] = {
#include "assets/rom/generated/79de9dbcf637cc02e537.inc.c"














};

// 0x07033EB8 - 0x07033FA8
static const Vtx inside_castle_seg7_vertex_07033EB8[] = {
#include "assets/rom/generated/2c2eec84c806c7c61c8c.inc.c"














};

// 0x07033FA8 - 0x07033FE8
static const Vtx inside_castle_seg7_vertex_07033FA8[] = {
#include "assets/rom/generated/b2d5a98e20ca037f5e8e.inc.c"



};

// 0x07033FE8 - 0x070340E8
static const Vtx inside_castle_seg7_vertex_07033FE8[] = {
#include "assets/rom/generated/de35e3a2ea2cc36e013a.inc.c"















};

// 0x070340E8 - 0x070341D8
static const Vtx inside_castle_seg7_vertex_070340E8[] = {
#include "assets/rom/generated/d12779b2a89d3baddd29.inc.c"














};

// 0x070341D8 - 0x070342B8
static const Vtx inside_castle_seg7_vertex_070341D8[] = {
#include "assets/rom/generated/e75ae29be2f7e9216fd8.inc.c"













};

// 0x070342B8 - 0x070343A8
static const Vtx inside_castle_seg7_vertex_070342B8[] = {
#include "assets/rom/generated/105e567c3cdcc486a54b.inc.c"














};

// 0x070343A8 - 0x07034428
static const Vtx inside_castle_seg7_vertex_070343A8[] = {
#include "assets/rom/generated/6237df8059fee5ba5796.inc.c"







};

// 0x07034428 - 0x07034528
static const Vtx inside_castle_seg7_vertex_07034428[] = {
#include "assets/rom/generated/09fa5c4e74a8c5349da5.inc.c"















};

// 0x07034528 - 0x07034628
static const Vtx inside_castle_seg7_vertex_07034528[] = {
#include "assets/rom/generated/3323f29f665cbc982dad.inc.c"















};

// 0x07034628 - 0x07034688
static const Vtx inside_castle_seg7_vertex_07034628[] = {
#include "assets/rom/generated/183bfe8d83996cd0ef07.inc.c"





};

// 0x07034688 - 0x070347D8
static const Gfx inside_castle_seg7_dl_07034688[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xddddddff),
    gsSPLightColor(LIGHT_2, 0x525252ff),
    gsSPVertex(inside_castle_seg7_vertex_070331F8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0,  2, 15, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x5f5f5fff),
    gsSPVertex(inside_castle_seg7_vertex_070332F8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070333F8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11,  7, 10, 0x0),
    gsSP2Triangles(11, 10, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070334F8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 10,  9, 0x0,  3,  5, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x070347D8 - 0x07034888
static const Gfx inside_castle_seg7_dl_070347D8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_070335D8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  7, 0x0,  3,  8,  6, 0x0),
    gsSP2Triangles( 3,  7,  9, 0x0,  3,  5,  8, 0x0),
    gsSP2Triangles( 8, 10,  6, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070336C8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  7,  1, 0x0),
    gsSP2Triangles( 4,  6,  3, 0x0,  3,  0,  2, 0x0),
    gsSP1Triangle( 0,  5,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07034888 - 0x07034B28
static const Gfx inside_castle_seg7_dl_07034888[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07033748, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  3,  7,  6, 0x0),
    gsSP2Triangles( 3,  8,  7, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(11, 12,  9, 0x0, 11, 13, 14, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07033838, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 7,  8,  5, 0x0,  5,  9,  6, 0x0),
    gsSP2Triangles( 5, 10,  9, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(12, 14, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07033928, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 4,  6, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle( 7, 14,  8, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07033A18, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07033AF8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07033BE8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  1,  5,  2, 0x0),
    gsSP2Triangles( 2,  6,  0, 0x0,  2,  7,  6, 0x0),
    gsSP2Triangles( 2,  8,  7, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07033CD8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07033DC8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles( 3, 10,  4, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07033EB8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 3, 10,  4, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07033FA8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07034B28 - 0x07034B90
static const Gfx inside_castle_seg7_dl_07034B28[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09003000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07033FE8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0, 15,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07034B90 - 0x07034C90
static const Gfx inside_castle_seg7_dl_07034B90[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_070340E8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 3, 10,  4, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070341D8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  0,  9,  1, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070342B8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070343A8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x07034C90 - 0x07034D88
static const Gfx inside_castle_seg7_dl_07034C90[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_0900A000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07034428, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  4, 0x0),
    gsSP2Triangles( 7,  4,  6, 0x0,  7,  6,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP2Triangles(10, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07034528, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  1,  4,  5, 0x0),
    gsSP2Triangles( 1,  5,  6, 0x0,  3,  2,  7, 0x0),
    gsSP2Triangles( 5,  3,  7, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(14,  8, 10, 0x0, 14, 10, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07034628, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x07034D88 - 0x07034E50
const Gfx inside_castle_seg7_dl_07034D88[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07034688),
    gsSPDisplayList(inside_castle_seg7_dl_070347D8),
    gsSPDisplayList(inside_castle_seg7_dl_07034888),
    gsSPDisplayList(inside_castle_seg7_dl_07034B28),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07034B90),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07034C90),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
