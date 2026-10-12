// 0x0700BB50 - 0x0700BB68

// 0x0700BB68 - 0x0700BC58
static const Vtx ddd_seg7_vertex_0700BB68[] = {
#include "assets/rom/generated/76c21b62ec95710dbd61.inc.c"














};

// 0x0700BC58 - 0x0700BC98
static const Vtx ddd_seg7_vertex_0700BC58[] = {
#include "assets/rom/generated/1552d425b852779d0f34.inc.c"



};

// 0x0700BC98 - 0x0700BD88
static const Vtx ddd_seg7_vertex_0700BC98[] = {
#include "assets/rom/generated/78cde7d669772f40b899.inc.c"














};

// 0x0700BD88 - 0x0700BE78
static const Vtx ddd_seg7_vertex_0700BD88[] = {
#include "assets/rom/generated/197e5d2a28e00a9f83fd.inc.c"














};

// 0x0700BE78 - 0x0700BF68
static const Vtx ddd_seg7_vertex_0700BE78[] = {
#include "assets/rom/generated/5efc206acbf9ba395536.inc.c"














};

// 0x0700BF68 - 0x0700C058
static const Vtx ddd_seg7_vertex_0700BF68[] = {
#include "assets/rom/generated/0a2132ecc79a1138ac96.inc.c"














};

// 0x0700C058 - 0x0700C148
static const Vtx ddd_seg7_vertex_0700C058[] = {
#include "assets/rom/generated/707f42184fc1de530b21.inc.c"














};

// 0x0700C148 - 0x0700C238
static const Vtx ddd_seg7_vertex_0700C148[] = {
#include "assets/rom/generated/97e45cce9a9f955ab659.inc.c"














};

// 0x0700C238 - 0x0700C328
static const Vtx ddd_seg7_vertex_0700C238[] = {
#include "assets/rom/generated/a8c89b2cc6100d63fea4.inc.c"














};

// 0x0700C328 - 0x0700C398
static const Vtx ddd_seg7_vertex_0700C328[] = {
#include "assets/rom/generated/bbc1b3b247ee53b31359.inc.c"






};

// 0x0700C398 - 0x0700C488
static const Vtx ddd_seg7_vertex_0700C398[] = {
#include "assets/rom/generated/1fa487bda3339f459134.inc.c"














};

// 0x0700C488 - 0x0700C578
static const Vtx ddd_seg7_vertex_0700C488[] = {
#include "assets/rom/generated/aec3bde77548bb816624.inc.c"














};

// 0x0700C578 - 0x0700C668
static const Vtx ddd_seg7_vertex_0700C578[] = {
#include "assets/rom/generated/79f60801d1e2dee0ea54.inc.c"














};

// 0x0700C668 - 0x0700C768
static const Vtx ddd_seg7_vertex_0700C668[] = {
#include "assets/rom/generated/603a071be3d30b20713f.inc.c"















};

// 0x0700C768 - 0x0700C858
static const Vtx ddd_seg7_vertex_0700C768[] = {
#include "assets/rom/generated/29bb95c5f1f77f300022.inc.c"














};

// 0x0700C858 - 0x0700C958
static const Vtx ddd_seg7_vertex_0700C858[] = {
#include "assets/rom/generated/c85f72c15ec75511dfe5.inc.c"















};

// 0x0700C958 - 0x0700C9D8
static const Vtx ddd_seg7_vertex_0700C958[] = {
#include "assets/rom/generated/b0f6e8a193414ed538df.inc.c"







};

// 0x0700C9D8 - 0x0700CA70
static const Gfx ddd_seg7_dl_0700C9D8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_0900A000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(ddd_seg7_vertex_0700BB68, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10, 11, 0x0, 11, 12,  7, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 13,  0,  2, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700BC58, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700CA70 - 0x0700CC70
static const Gfx ddd_seg7_dl_0700CA70[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ddd_seg7_texture_07001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ddd_seg7_vertex_0700BC98, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700BD88, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700BE78, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700BF68, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700C058, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700C148, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700C238, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700C328, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  6,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700CC70 - 0x0700CE48
static const Gfx ddd_seg7_dl_0700CC70[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09007800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ddd_seg7_vertex_0700C398, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700C488, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  6,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700C578, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700C668, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 6, 10,  9, 0x0,  6,  8, 11, 0x0),
    gsSP2Triangles( 2, 12,  0, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700C768, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700C858, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  8,  6, 0x0),
    gsSP2Triangles( 3,  4,  9, 0x0, 10, 11, 12, 0x0),
    gsSP2Triangles(13, 11, 14, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700C958, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700CE48 - 0x0700CEE0
const Gfx ddd_seg7_dl_0700CE48[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ddd_seg7_dl_0700C9D8),
    gsSPDisplayList(ddd_seg7_dl_0700CA70),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ddd_seg7_dl_0700CC70),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
