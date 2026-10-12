// 0x07007910 - 0x07007928

// 0x07007928 - 0x07007A18
static const Vtx sl_seg7_vertex_07007928[] = {
#include "assets/rom/generated/b8d728911477838e57b8.inc.c"














};

// 0x07007A18 - 0x07007B08
static const Vtx sl_seg7_vertex_07007A18[] = {
#include "assets/rom/generated/47b5dc2a5a34baf2a5b3.inc.c"














};

// 0x07007B08 - 0x07007BF8
static const Vtx sl_seg7_vertex_07007B08[] = {
#include "assets/rom/generated/1b0e0d5494cc67b330ea.inc.c"














};

// 0x07007BF8 - 0x07007CF8
static const Vtx sl_seg7_vertex_07007BF8[] = {
#include "assets/rom/generated/d43679de40b697f52baa.inc.c"















};

// 0x07007CF8 - 0x07007DE8
static const Vtx sl_seg7_vertex_07007CF8[] = {
#include "assets/rom/generated/0c74d90a4b27ad66bd34.inc.c"














};

// 0x07007DE8 - 0x07007EC8
static const Vtx sl_seg7_vertex_07007DE8[] = {
#include "assets/rom/generated/a271fe017670ef2b913f.inc.c"













};

// 0x07007EC8 - 0x07007FB8
static const Vtx sl_seg7_vertex_07007EC8[] = {
#include "assets/rom/generated/2f5c096f56bc19a1efa3.inc.c"














};

// 0x07007FB8 - 0x070080A8
static const Vtx sl_seg7_vertex_07007FB8[] = {
#include "assets/rom/generated/2cbc94cefdaed1192ac3.inc.c"














};

// 0x070080A8 - 0x07008198
static const Vtx sl_seg7_vertex_070080A8[] = {
#include "assets/rom/generated/d20621b41eb63667c8b9.inc.c"














};

// 0x07008198 - 0x07008288
static const Vtx sl_seg7_vertex_07008198[] = {
#include "assets/rom/generated/39b0698b706b90309c14.inc.c"














};

// 0x07008288 - 0x07008378
static const Vtx sl_seg7_vertex_07008288[] = {
#include "assets/rom/generated/faa46b7b13dc8ffb84ad.inc.c"














};

// 0x07008378 - 0x07008478
static const Vtx sl_seg7_vertex_07008378[] = {
#include "assets/rom/generated/4dbdbefa0a76f2ca3ddf.inc.c"















};

// 0x07008478 - 0x07008528
static const Vtx sl_seg7_vertex_07008478[] = {
#include "assets/rom/generated/c1601a8f9c82332131de.inc.c"










};

// 0x07008528 - 0x070088B0
static const Gfx sl_seg7_dl_07008528[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(sl_seg7_vertex_07007928, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14, 11, 13, 0x0),
    gsSPVertex(sl_seg7_vertex_07007A18, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 0, 10,  1, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(sl_seg7_vertex_07007B08, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(sl_seg7_vertex_07007BF8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 10, 11,  7, 0x0),
    gsSP2Triangles( 0, 12,  1, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(sl_seg7_vertex_07007CF8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11,  8, 10, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(sl_seg7_vertex_07007DE8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(sl_seg7_vertex_07007EC8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 10, 11,  7, 0x0),
    gsSP2Triangles( 8, 12,  9, 0x0,  8, 13, 12, 0x0),
    gsSP2Triangles(10, 14, 11, 0x0, 14, 12, 11, 0x0),
    gsSP1Triangle(12, 13, 11, 0x0),
    gsSPVertex(sl_seg7_vertex_07007FB8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(sl_seg7_vertex_070080A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(sl_seg7_vertex_07008198, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(sl_seg7_vertex_07008288, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(12, 11, 14, 0x0),
    gsSPVertex(sl_seg7_vertex_07008378, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 9, 10,  7, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles( 0,  2, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(sl_seg7_vertex_07008478, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP1Triangle( 7, 10,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x070088B0 - 0x07008930
const Gfx sl_seg7_dl_070088B0[] = {
    gsDPPipeSync(),
    gsDPSetEnvColor(255, 255, 255, 100),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(sl_seg7_dl_07008528),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};
