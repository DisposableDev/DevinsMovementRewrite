// 0x0700BD68 - 0x0700BD80

// 0x0700BD80 - 0x0700BE70
static const Vtx sl_seg7_vertex_0700BD80[] = {
#include "assets/rom/generated/e9cc5bb05c0ef02f7d23.inc.c"














};

// 0x0700BE70 - 0x0700BF60
static const Vtx sl_seg7_vertex_0700BE70[] = {
#include "assets/rom/generated/b800e2ab2fdb49be35f8.inc.c"














};

// 0x0700BF60 - 0x0700C050
static const Vtx sl_seg7_vertex_0700BF60[] = {
#include "assets/rom/generated/03722e572c6f68811d23.inc.c"














};

// 0x0700C050 - 0x0700C140
static const Vtx sl_seg7_vertex_0700C050[] = {
#include "assets/rom/generated/71c899fbd1fd6261037d.inc.c"














};

// 0x0700C140 - 0x0700C230
static const Vtx sl_seg7_vertex_0700C140[] = {
#include "assets/rom/generated/df7753b6d04fa9818b0c.inc.c"














};

// 0x0700C230 - 0x0700C320
static const Vtx sl_seg7_vertex_0700C230[] = {
#include "assets/rom/generated/adead8cc14bb9ada6d99.inc.c"














};

// 0x0700C320 - 0x0700C410
static const Vtx sl_seg7_vertex_0700C320[] = {
#include "assets/rom/generated/cfe5864617fda9f5f28c.inc.c"














};

// 0x0700C410 - 0x0700C500
static const Vtx sl_seg7_vertex_0700C410[] = {
#include "assets/rom/generated/1b3afd40939e1fff684f.inc.c"














};

// 0x0700C500 - 0x0700C5F0
static const Vtx sl_seg7_vertex_0700C500[] = {
#include "assets/rom/generated/782461635dd8ffdfffd2.inc.c"














};

// 0x0700C5F0 - 0x0700C6E0
static const Vtx sl_seg7_vertex_0700C5F0[] = {
#include "assets/rom/generated/cd5bd6ef326033862d43.inc.c"














};

// 0x0700C6E0 - 0x0700C720
static const Vtx sl_seg7_vertex_0700C6E0[] = {
#include "assets/rom/generated/806eb4c7919bed97b70c.inc.c"



};

// 0x0700C720 - 0x0700C9E8
static const Gfx sl_seg7_dl_0700C720[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(sl_seg7_vertex_0700BD80, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(sl_seg7_vertex_0700BE70, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(sl_seg7_vertex_0700BF60, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(sl_seg7_vertex_0700C050, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(sl_seg7_vertex_0700C140, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(sl_seg7_vertex_0700C230, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(sl_seg7_vertex_0700C320, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(sl_seg7_vertex_0700C410, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(sl_seg7_vertex_0700C500, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(sl_seg7_vertex_0700C5F0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(sl_seg7_vertex_0700C6E0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700C9E8 - 0x0700CA68
const Gfx sl_seg7_dl_0700C9E8[] = {
    gsDPPipeSync(),
    gsDPSetEnvColor(255, 255, 255, 90),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(sl_seg7_dl_0700C720),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};
