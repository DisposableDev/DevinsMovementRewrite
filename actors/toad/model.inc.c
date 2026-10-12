// Toad

// 0x06005908

// 0x06005920
ALIGNED8 static const Texture toad_seg6_texture_06005920[] = {
#include "actors/toad/toad_face.rgba16.inc.c"
};

// 0x06006120
ALIGNED8 static const Texture toad_seg6_texture_06006120[] = {
#include "actors/toad/toad_head.rgba16.inc.c"
};

// 0x06006920
static const Vtx toad_seg6_vertex_06006920[] = {
#include "assets/rom/generated/ee6d12046877b4311985.inc.c"














};

// 0x06006A10
static const Vtx toad_seg6_vertex_06006A10[] = {
#include "assets/rom/generated/df4f7b5daf7154be8359.inc.c"















};

// 0x06006B10
static const Vtx toad_seg6_vertex_06006B10[] = {
#include "assets/rom/generated/1afbf6fa37138c509c96.inc.c"











};

// 0x06006BD0
static const Vtx toad_seg6_vertex_06006BD0[] = {
#include "assets/rom/generated/e02384c8947893645081.inc.c"














};

// 0x06006CC0
static const Vtx toad_seg6_vertex_06006CC0[] = {
#include "assets/rom/generated/87b01fadc09964d7e724.inc.c"















};

// 0x06006DC0
static const Vtx toad_seg6_vertex_06006DC0[] = {
#include "assets/rom/generated/827bb2bd667691798e3a.inc.c"















};

// 0x06006EC0
static const Vtx toad_seg6_vertex_06006EC0[] = {
#include "assets/rom/generated/f659bb114979bd0b574a.inc.c"














};

// 0x06006FB0
static const Vtx toad_seg6_vertex_06006FB0[] = {
#include "assets/rom/generated/742cfb0f3cd7e224030f.inc.c"















};

// 0x060070B0
static const Vtx toad_seg6_vertex_060070B0[] = {
#include "assets/rom/generated/4bc24f9bd9fc5ba32760.inc.c"















};

// 0x060071B0
static const Vtx toad_seg6_vertex_060071B0[] = {
#include "assets/rom/generated/96748246a8dd9627ffc5.inc.c"




};

// 0x06007200
static const Vtx toad_seg6_vertex_06007200[] = {
#include "assets/rom/generated/fee98f6a31c86a2cc148.inc.c"















};

// 0x06007300 - 0x06007498
const Gfx toad_seg6_dl_06007300[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, toad_seg6_texture_06005920),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(toad_seg6_vertex_06006920, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 2,  4,  0, 0x0,  0,  5,  6, 0x0),
    gsSP2Triangles( 3,  5,  0, 0x0,  6,  1,  0, 0x0),
    gsSP2Triangles( 6,  5,  7, 0x0,  3,  7,  5, 0x0),
    gsSP2Triangles( 1,  6,  8, 0x0,  8,  6,  9, 0x0),
    gsSP2Triangles( 7,  9,  6, 0x0, 10,  9,  7, 0x0),
    gsSP2Triangles( 3, 11,  7, 0x0,  7, 11, 10, 0x0),
    gsSP2Triangles( 8, 12,  1, 0x0, 13, 14,  8, 0x0),
    gsSP2Triangles( 9, 13,  8, 0x0, 12,  8, 14, 0x0),
    gsSPVertex(toad_seg6_vertex_06006A10, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  3, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 9, 10,  7, 0x0,  1, 11, 12, 0x0),
    gsSP2Triangles(13, 11,  1, 0x0,  1, 12,  2, 0x0),
    gsSP2Triangles(13,  1,  0, 0x0,  2, 12, 14, 0x0),
    gsSP1Triangle( 2, 14, 15, 0x0),
    gsSPVertex(toad_seg6_vertex_06006B10, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 4,  0,  5, 0x0,  4,  5,  6, 0x0),
    gsSP2Triangles( 2,  3,  7, 0x0,  5,  0,  2, 0x0),
    gsSP2Triangles( 5,  2,  8, 0x0,  8,  2,  7, 0x0),
    gsSP2Triangles( 8,  7,  9, 0x0,  6,  5,  8, 0x0),
    gsSP2Triangles( 6,  8, 10, 0x0, 10,  8,  9, 0x0),
    gsSP1Triangle(10,  9, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x06007498 - 0x060076C0
const Gfx toad_seg6_dl_06007498[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, toad_seg6_texture_06006120),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(toad_seg6_vertex_06006BD0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  2,  4,  5, 0x0),
    gsSP2Triangles( 3,  2,  6, 0x0,  6,  2,  5, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  8, 10,  9, 0x0),
    gsSP2Triangles( 8, 11, 10, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(toad_seg6_vertex_06006CC0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 10,  7, 0x0),
    gsSP2Triangles(11, 12, 10, 0x0, 13, 12, 11, 0x0),
    gsSP2Triangles(14,  1,  0, 0x0, 15, 14,  0, 0x0),
    gsSPVertex(toad_seg6_vertex_06006DC0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  7,  6, 0x0),
    gsSP2Triangles( 9, 10,  7, 0x0, 11, 10,  9, 0x0),
    gsSP2Triangles(12,  4,  3, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(toad_seg6_vertex_06006EC0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  3,  6, 0x0),
    gsSP2Triangles( 7,  6,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12,  7,  8, 0x0, 11, 10, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(toad_seg6_vertex_06006FB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  3,  0, 0x0,  4,  5,  3, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(toad_seg6_vertex_060070B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(11, 10, 12, 0x0,  9, 13, 10, 0x0),
    gsSP2Triangles(14, 13,  9, 0x0, 14, 15, 13, 0x0),
    gsSPVertex(toad_seg6_vertex_060071B0, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP1Triangle( 4,  3,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x060076C0 - 0x06007710
const Gfx toad_seg6_dl_060076C0[] = {
    gsSPVertex(toad_seg6_vertex_06007200, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0, 15,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x06007710 - 0x06007788
const Gfx toad_seg6_dl_06007710[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(toad_seg6_dl_06007300),
    gsSPDisplayList(toad_seg6_dl_06007498),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(toad_seg6_dl_060076C0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

// 0x06007788 - 0x06007808
const Gfx toad_seg6_us_dl_06007788[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetRenderMode(G_RM_CUSTOM_AA_ZB_XLU_SURF, G_RM_NOOP2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(toad_seg6_dl_06007300),
    gsSPDisplayList(toad_seg6_dl_06007498),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(toad_seg6_dl_060076C0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

// 0x06007788

// 0x060077A0
static const Vtx toad_seg6_vertex_060077A0[] = {
#include "assets/rom/generated/2bdefc0136b84f57e6bc.inc.c"















};

// 0x060078A0
static const Vtx toad_seg6_vertex_060078A0[] = {
#include "assets/rom/generated/9cf3a00d5f3520a14b87.inc.c"











};

// 0x06007960 - 0x06007A48
const Gfx toad_seg6_dl_06007960[] = {
    gsSPLightColor(LIGHT_1, 0x4227b5ff),
    gsSPLightColor(LIGHT_2, 0x21135aff),
    gsSPVertex(toad_seg6_vertex_060077A0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  4,  7,  8, 0x0),
    gsSP2Triangles( 8,  5,  4, 0x0,  9,  7,  6, 0x0),
    gsSP2Triangles(10,  8,  7, 0x0, 10,  7,  9, 0x0),
    gsSP2Triangles(10, 11,  8, 0x0, 12,  8, 11, 0x0),
    gsSP2Triangles(12,  5,  8, 0x0, 10,  9, 13, 0x0),
    gsSP2Triangles( 0, 14,  1, 0x0, 15,  0,  2, 0x0),
    gsSPVertex(toad_seg6_vertex_060078A0, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 4,  3,  2, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles( 5,  9,  8, 0x0,  4,  2, 10, 0x0),
    gsSP2Triangles( 8,  1,  5, 0x0,  8,  7, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x06007A48 - 0x06007A80
const Gfx toad_seg6_dl_06007A48[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPDisplayList(toad_seg6_dl_06007960),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x06007B00 - 0x06007B28
const Gfx toad_seg6_us_dl_06007B00[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(toad_seg6_dl_06007960),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

// 0x06007A80

// 0x06007A98

// 0x06007AB0
static const Vtx toad_seg6_vertex_06007AB0[] = {
#include "assets/rom/generated/52d787909f7c4f0fcc85.inc.c"













};

// 0x06007B90
static const Vtx toad_seg6_vertex_06007B90[] = {
#include "assets/rom/generated/16c44804c5fa1d4876c0.inc.c"











};

// 0x06007C50
static const Vtx toad_seg6_vertex_06007C50[] = {
#include "assets/rom/generated/3babbef84df0ec825b52.inc.c"











};

// 0x06007D10 - 0x06007EB0
const Gfx toad_seg6_dl_06007D10[] = {
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(toad_seg6_vertex_06007AB0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  3,  6,  7, 0x0),
    gsSP2Triangles( 7,  8,  3, 0x0,  3,  9,  4, 0x0),
    gsSP2Triangles( 3,  8,  9, 0x0, 10,  7,  6, 0x0),
    gsSP2Triangles(10,  6, 11, 0x0, 11,  6,  5, 0x0),
    gsSP2Triangles( 7,  0,  8, 0x0,  0,  7, 10, 0x0),
    gsSP2Triangles( 2, 12, 13, 0x0,  2,  1, 12, 0x0),
    gsSP2Triangles( 2,  8,  0, 0x0,  2, 13,  9, 0x0),
    gsSP1Triangle( 2,  9,  8, 0x0),
    gsSPVertex(toad_seg6_vertex_06007B90, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  1, 0x0),
    gsSP2Triangles( 7,  1,  9, 0x0,  7, 10,  8, 0x0),
    gsSP2Triangles( 7,  9,  4, 0x0,  5,  4,  9, 0x0),
    gsSP2Triangles( 5,  9,  0, 0x0,  0,  9,  1, 0x0),
    gsSP2Triangles(11,  6,  0, 0x0, 11,  0,  2, 0x0),
    gsSP1Triangle( 0,  6,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xfed5a1ff),
    gsSPLightColor(LIGHT_2, 0x7f6a50ff),
    gsSPVertex(toad_seg6_vertex_06007C50, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 4,  2,  1, 0x0,  4,  1,  5, 0x0),
    gsSP2Triangles( 6,  7,  2, 0x0,  6,  2,  4, 0x0),
    gsSP2Triangles( 2,  7,  3, 0x0,  5,  8,  4, 0x0),
    gsSP2Triangles( 6,  8,  9, 0x0,  6,  4,  8, 0x0),
    gsSP2Triangles(10,  3,  7, 0x0,  7, 11, 10, 0x0),
    gsSP2Triangles( 7,  6, 11, 0x0,  9, 11,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x06007EB0 - 0x06007ED8
const Gfx toad_seg6_dl_06007EB0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(toad_seg6_dl_06007D10),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

// 0x06007ED8

// 0x06007EF0
static const Vtx toad_seg6_vertex_06007EF0[] = {
#include "assets/rom/generated/9ff7dcc05d5563925758.inc.c"















};

// 0x06007FF0
static const Vtx toad_seg6_vertex_06007FF0[] = {
#include "assets/rom/generated/1297a029aed29bc9d1bd.inc.c"












};

// 0x060080C0 - 0x060081F8
const Gfx toad_seg6_dl_060080C0[] = {
    gsSPLightColor(LIGHT_1, 0xfed5a1ff),
    gsSPLightColor(LIGHT_2, 0x7f6a50ff),
    gsSPVertex(toad_seg6_vertex_06007EF0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  4,  3,  0, 0x0),
    gsSP2Triangles( 2,  5,  6, 0x0,  6,  7,  2, 0x0),
    gsSP2Triangles( 4,  2,  7, 0x0,  5,  2,  1, 0x0),
    gsSP2Triangles( 7,  8,  4, 0x0,  4,  9,  3, 0x0),
    gsSP2Triangles( 4,  8, 10, 0x0, 11,  9,  4, 0x0),
    gsSP2Triangles( 4, 10, 11, 0x0,  1,  3, 12, 0x0),
    gsSP2Triangles(11,  3,  9, 0x0, 12,  3, 11, 0x0),
    gsSP2Triangles( 7, 13,  8, 0x0,  7,  6, 13, 0x0),
    gsSP2Triangles(10, 14, 15, 0x0, 10, 15, 11, 0x0),
    gsSPVertex(toad_seg6_vertex_06007FF0, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 2,  4,  0, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 7,  8,  5, 0x0,  2,  8,  4, 0x0),
    gsSP2Triangles( 2,  5,  8, 0x0,  5,  2,  9, 0x0),
    gsSP2Triangles( 5,  9,  6, 0x0,  9,  2,  1, 0x0),
    gsSP2Triangles(10,  9,  1, 0x0, 10,  1, 11, 0x0),
    gsSP2Triangles( 9, 12,  6, 0x0,  9, 10, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x060081F8 - 0x06008220
const Gfx toad_seg6_dl_060081F8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(toad_seg6_dl_060080C0),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

// 0x06008220

// 0x06008238
static const Vtx toad_seg6_vertex_06008238[] = {
#include "assets/rom/generated/1bf2589ad609d53b98ee.inc.c"















};

// 0x06008338
static const Vtx toad_seg6_vertex_06008338[] = {
#include "assets/rom/generated/a447edb9868388849f73.inc.c"










};

// 0x060083E8 - 0x06008520
const Gfx toad_seg6_dl_060083E8[] = {
    gsSPLightColor(LIGHT_1, 0xfed5a1ff),
    gsSPLightColor(LIGHT_2, 0x7f6a50ff),
    gsSPVertex(toad_seg6_vertex_06008238, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 5,  1,  0, 0x0,  0,  4,  6, 0x0),
    gsSP2Triangles( 0,  7,  8, 0x0,  8,  3,  0, 0x0),
    gsSP2Triangles( 2,  7,  0, 0x0,  6,  5,  0, 0x0),
    gsSP2Triangles( 9, 10,  7, 0x0,  7, 11,  8, 0x0),
    gsSP2Triangles( 7,  2,  9, 0x0, 11,  7, 10, 0x0),
    gsSP2Triangles(12,  3,  8, 0x0, 12,  8, 11, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  6,  3, 12, 0x0),
    gsSP2Triangles( 1,  9,  2, 0x0, 12, 11, 10, 0x0),
    gsSP2Triangles( 6, 12, 13, 0x0,  6, 14, 15, 0x0),
    gsSP2Triangles( 6, 15,  5, 0x0, 13, 14,  6, 0x0),
    gsSPVertex(toad_seg6_vertex_06008338, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 2,  4,  0, 0x0,  4,  5,  0, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  0,  5, 0x0),
    gsSP2Triangles( 8,  7,  5, 0x0,  3,  1,  9, 0x0),
    gsSP2Triangles( 1,  0,  7, 0x0,  9,  1,  7, 0x0),
    gsSP2Triangles( 8, 10,  7, 0x0,  9,  7, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x06008520 - 0x06008560
const Gfx toad_seg6_dl_06008520[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(toad_seg6_dl_060083E8),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};

// US: 8608
// 0x06008608 - 0x06008650
const Gfx toad_seg6_us_dl_06008608[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(toad_seg6_dl_060083E8),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetRenderMode(G_RM_AA_ZB_XLU_SURF, G_RM_NOOP2),
    gsSPEndDisplayList(),
};

// 0x06008560

// 0x06008578
static const Vtx toad_seg6_vertex_06008578[] = {
#include "assets/rom/generated/4d538baa6f42f4538fd7.inc.c"















};

// 0x06008678
static const Vtx toad_seg6_vertex_06008678[] = {
#include "assets/rom/generated/c28eafeec8ccf94e63be.inc.c"












};

// 0x06008748 - 0x06008890
const Gfx toad_seg6_dl_06008748[] = {
    gsSPLightColor(LIGHT_1, 0x68401bff),
    gsSPLightColor(LIGHT_2, 0x34200dff),
    gsSPVertex(toad_seg6_vertex_06008578, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  2,  5,  6, 0x0),
    gsSP2Triangles( 2,  7,  3, 0x0,  2,  8,  7, 0x0),
    gsSP2Triangles( 2,  9,  8, 0x0,  2,  6,  9, 0x0),
    gsSP2Triangles( 2,  1, 10, 0x0,  2, 10,  4, 0x0),
    gsSP2Triangles( 7, 11,  0, 0x0, 12, 11,  7, 0x0),
    gsSP2Triangles(12,  7,  8, 0x0,  7,  0,  3, 0x0),
    gsSP2Triangles(12,  8,  9, 0x0, 10,  5,  4, 0x0),
    gsSP2Triangles(12,  9, 13, 0x0, 14,  9,  6, 0x0),
    gsSP2Triangles(14, 13,  9, 0x0,  5, 14,  6, 0x0),
    gsSP1Triangle(10, 15,  5, 0x0),
    gsSPVertex(toad_seg6_vertex_06008678, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  4,  0, 0x0,  2,  5,  6, 0x0),
    gsSP2Triangles( 2,  6,  3, 0x0,  2,  1,  7, 0x0),
    gsSP2Triangles( 7,  5,  2, 0x0,  8,  4,  3, 0x0),
    gsSP2Triangles( 8,  3,  6, 0x0,  9,  7,  1, 0x0),
    gsSP2Triangles( 7, 10,  5, 0x0,  5, 10, 11, 0x0),
    gsSP2Triangles(11,  6,  5, 0x0, 11, 12,  6, 0x0),
    gsSP1Triangle( 6, 12,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x06008890 - 0x060088B8
const Gfx toad_seg6_dl_06008890[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(toad_seg6_dl_06008748),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

// 0x060088B8

// 0x060088D0
static const Vtx toad_seg6_vertex_060088D0[] = {
#include "assets/rom/generated/63d198a1217c31576cf3.inc.c"















};

// 0x060089D0
static const Vtx toad_seg6_vertex_060089D0[] = {
#include "assets/rom/generated/ef3e6a59e3f889542d43.inc.c"











};

// 0x06008A90 - 0x06008BD8
const Gfx toad_seg6_dl_06008A90[] = {
    gsSPLightColor(LIGHT_1, 0x68401bff),
    gsSPLightColor(LIGHT_2, 0x34200dff),
    gsSPVertex(toad_seg6_vertex_060088D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  0,  2,  5, 0x0),
    gsSP2Triangles( 0,  5,  6, 0x0,  0,  6,  7, 0x0),
    gsSP2Triangles( 0,  8,  3, 0x0,  0,  9,  8, 0x0),
    gsSP2Triangles( 0, 10,  9, 0x0,  0,  7, 10, 0x0),
    gsSP2Triangles( 8, 11, 12, 0x0,  8,  9, 11, 0x0),
    gsSP2Triangles( 8, 12,  4, 0x0,  8,  4,  3, 0x0),
    gsSP2Triangles(10, 11,  9, 0x0,  6,  5,  2, 0x0),
    gsSP2Triangles(10, 13, 14, 0x0, 10,  7, 13, 0x0),
    gsSP2Triangles(10, 14, 11, 0x0, 13,  7,  6, 0x0),
    gsSP2Triangles(13,  6, 15, 0x0,  6,  2, 15, 0x0),
    gsSPVertex(toad_seg6_vertex_060089D0, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 1,  0,  4, 0x0,  0,  3,  5, 0x0),
    gsSP2Triangles( 0,  5,  6, 0x0,  6,  4,  0, 0x0),
    gsSP2Triangles( 7,  5,  3, 0x0,  3,  8,  7, 0x0),
    gsSP2Triangles( 3,  2,  8, 0x0,  1,  4,  9, 0x0),
    gsSP2Triangles(10,  8,  2, 0x0,  2, 11, 10, 0x0),
    gsSP2Triangles( 2,  1, 11, 0x0,  9, 11,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x06008BD8 - 0x06008C00
const Gfx toad_seg6_dl_06008BD8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(toad_seg6_dl_06008A90),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};
