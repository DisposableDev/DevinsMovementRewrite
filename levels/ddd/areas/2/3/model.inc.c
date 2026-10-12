// 0x07007D18 - 0x07007D30

// 0x07007D30 - 0x07007E20
static const Vtx ddd_seg7_vertex_07007D30[] = {
#include "assets/rom/generated/90d82cf5544fc3b383a3.inc.c"














};

// 0x07007E20 - 0x07007F10
static const Vtx ddd_seg7_vertex_07007E20[] = {
#include "assets/rom/generated/b60bdf18b8121fc1ea0f.inc.c"














};

// 0x07007F10 - 0x07008000
static const Vtx ddd_seg7_vertex_07007F10[] = {
#include "assets/rom/generated/7e848e3c86d4793160a8.inc.c"














};

// 0x07008000 - 0x070080F0
static const Vtx ddd_seg7_vertex_07008000[] = {
#include "assets/rom/generated/00e7634cec47cebdd74c.inc.c"














};

// 0x070080F0 - 0x070081E0
static const Vtx ddd_seg7_vertex_070080F0[] = {
#include "assets/rom/generated/980df8b7fbe6e37700f5.inc.c"














};

// 0x070081E0 - 0x070082D0
static const Vtx ddd_seg7_vertex_070081E0[] = {
#include "assets/rom/generated/2a52dc50c7d3e37f5c10.inc.c"














};

// 0x070082D0 - 0x070083C0
static const Vtx ddd_seg7_vertex_070082D0[] = {
#include "assets/rom/generated/f35875d56694bac50b48.inc.c"














};

// 0x070083C0 - 0x070084C0
static const Vtx ddd_seg7_vertex_070083C0[] = {
#include "assets/rom/generated/4f7c70f0346ca2e967ce.inc.c"















};

// 0x070084C0 - 0x070085A0
static const Vtx ddd_seg7_vertex_070084C0[] = {
#include "assets/rom/generated/0b026597a623f33f7548.inc.c"













};

// 0x070085A0 - 0x070086A0
static const Vtx ddd_seg7_vertex_070085A0[] = {
#include "assets/rom/generated/7f0aebe57a68cacbc1b0.inc.c"















};

// 0x070086A0 - 0x07008780
static const Vtx ddd_seg7_vertex_070086A0[] = {
#include "assets/rom/generated/1c41eee1ca9f0df052bb.inc.c"













};

// 0x07008780 - 0x07008870
static const Vtx ddd_seg7_vertex_07008780[] = {
#include "assets/rom/generated/35bd02784ad47ba6a8ae.inc.c"














};

// 0x07008870 - 0x070088F0
static const Vtx ddd_seg7_vertex_07008870[] = {
#include "assets/rom/generated/5231094759f3dae2f179.inc.c"







};

// 0x070088F0 - 0x07008C48
static const Gfx ddd_seg7_dl_070088F0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ddd_seg7_texture_07001000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(ddd_seg7_vertex_07007D30, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ddd_seg7_vertex_07007E20, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(ddd_seg7_vertex_07007F10, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_07008000, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_070080F0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 7, 14,  8, 0x0),
    gsSPVertex(ddd_seg7_vertex_070081E0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ddd_seg7_vertex_070082D0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 0, 10,  1, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ddd_seg7_vertex_070083C0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP2Triangles(10, 13, 14, 0x0, 10, 14, 15, 0x0),
    gsSPVertex(ddd_seg7_vertex_070084C0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(ddd_seg7_vertex_070085A0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  2,  5,  0, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(ddd_seg7_vertex_070086A0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(ddd_seg7_vertex_07008780, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ddd_seg7_vertex_07008870, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008C48 - 0x07008CB8
const Gfx ddd_seg7_dl_07008C48[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ddd_seg7_dl_070088F0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
