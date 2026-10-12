// 0x0700F8B8 - 0x0700F8D0

// 0x0700F8D0 - 0x0700F8E8

// 0x0700F8E8 - 0x0700F900

// 0x0700F900 - 0x0700F918

// 0x0700F918 - 0x0700F9F8
static const Vtx bbh_seg7_vertex_0700F918[] = {
#include "assets/rom/generated/445d9409e775780af937.inc.c"













};

// 0x0700F9F8 - 0x0700FA68
static const Vtx bbh_seg7_vertex_0700F9F8[] = {
#include "assets/rom/generated/bc8b5fac85a6d1e3df7d.inc.c"






};

// 0x0700FA68 - 0x0700FAE8
static const Vtx bbh_seg7_vertex_0700FA68[] = {
#include "assets/rom/generated/3c377a86dff8c4ba557d.inc.c"







};

// 0x0700FAE8 - 0x0700FBA8
static const Vtx bbh_seg7_vertex_0700FAE8[] = {
#include "assets/rom/generated/2697296817b5059633d8.inc.c"











};

// 0x0700FBA8 - 0x0700FBE8
static const Vtx bbh_seg7_vertex_0700FBA8[] = {
#include "assets/rom/generated/575aa440e863880f3119.inc.c"



};

// 0x0700FBE8 - 0x0700FCA8
static const Vtx bbh_seg7_vertex_0700FBE8[] = {
#include "assets/rom/generated/158193da4c060a685ed8.inc.c"











};

// 0x0700FCA8 - 0x0700FD68
static const Vtx bbh_seg7_vertex_0700FCA8[] = {
#include "assets/rom/generated/680c6141efb242e8b440.inc.c"











};

// 0x0700FD68 - 0x0700FE28
static const Vtx bbh_seg7_vertex_0700FD68[] = {
#include "assets/rom/generated/68600e6f97dda6457f33.inc.c"











};

// 0x0700FE28 - 0x0700FF28
static const Vtx bbh_seg7_vertex_0700FE28[] = {
#include "assets/rom/generated/1e74205f3fb8afc7a52b.inc.c"















};

// 0x0700FF28 - 0x07010018
static const Vtx bbh_seg7_vertex_0700FF28[] = {
#include "assets/rom/generated/d59c470d51d58b1a105b.inc.c"














};

// 0x07010018 - 0x070100C8
static const Vtx bbh_seg7_vertex_07010018[] = {
#include "assets/rom/generated/5b57963eb6259f4b34f8.inc.c"










};

// 0x070100C8 - 0x070101A8
static const Vtx bbh_seg7_vertex_070100C8[] = {
#include "assets/rom/generated/20dfd1d94d4791a5b7c9.inc.c"













};

// 0x070101A8 - 0x07010298
static const Vtx bbh_seg7_vertex_070101A8[] = {
#include "assets/rom/generated/ae7d125c3e71f95c93b2.inc.c"














};

// 0x07010298 - 0x07010388
static const Vtx bbh_seg7_vertex_07010298[] = {
#include "assets/rom/generated/eca3cc1767431a6d7f83.inc.c"














};

// 0x07010388 - 0x07010478
static const Vtx bbh_seg7_vertex_07010388[] = {
#include "assets/rom/generated/ae5a39e80372d9fae779.inc.c"














};

// 0x07010478 - 0x07010568
static const Vtx bbh_seg7_vertex_07010478[] = {
#include "assets/rom/generated/17646b1d8a2fc1fc403e.inc.c"














};

// 0x07010568 - 0x07010668
static const Vtx bbh_seg7_vertex_07010568[] = {
#include "assets/rom/generated/aa5d678ad90ee50ffbb5.inc.c"















};

// 0x07010668 - 0x07010768
static const Vtx bbh_seg7_vertex_07010668[] = {
#include "assets/rom/generated/f97e1c820a27e51df70a.inc.c"















};

// 0x07010768 - 0x07010858
static const Vtx bbh_seg7_vertex_07010768[] = {
#include "assets/rom/generated/36ee4ce0bfc63681f173.inc.c"














};

// 0x07010858 - 0x07010908
static const Vtx bbh_seg7_vertex_07010858[] = {
#include "assets/rom/generated/520c21baa73090be1c5b.inc.c"










};

// 0x07010908 - 0x07010988
static const Vtx bbh_seg7_vertex_07010908[] = {
#include "assets/rom/generated/de8fb33df2601bee8c39.inc.c"







};

// 0x07010988 - 0x07010A08
static const Vtx bbh_seg7_vertex_07010988[] = {
#include "assets/rom/generated/842ed084c56707789798.inc.c"







};

// 0x07010A08 - 0x07010A48
static const Vtx bbh_seg7_vertex_07010A08[] = {
#include "assets/rom/generated/7f08e6efa80ff5d5a472.inc.c"



};

// 0x07010A48 - 0x07010B58
static const Gfx bbh_seg7_dl_07010A48[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09008800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xfefe8cff),
    gsSPLightColor(LIGHT_2, 0x656538ff),
    gsSPVertex(bbh_seg7_vertex_0700F918, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  3, 0x0,  5,  7,  8, 0x0),
    gsSP2Triangles( 5,  9,  7, 0x0, 10, 11, 12, 0x0),
    gsSP1Triangle(10, 13, 11, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700F9F8, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  6,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0x505050ff),
    gsSPLightColor(LIGHT_2, 0x202020ff),
    gsSPVertex(bbh_seg7_vertex_0700FA68, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(bbh_seg7_vertex_0700FAE8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x07010B58 - 0x07010C30
static const Gfx bbh_seg7_dl_07010B58[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xfefe8cff),
    gsSPLightColor(LIGHT_2, 0x656538ff),
    gsSPVertex(bbh_seg7_vertex_0700FBA8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(bbh_seg7_vertex_0700FBE8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSPLightColor(LIGHT_1, 0x807052ff),
    gsSPLightColor(LIGHT_2, 0x332c20ff),
    gsSPVertex(bbh_seg7_vertex_0700FCA8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x07010C30 - 0x07010D00
static const Gfx bbh_seg7_dl_07010C30[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bbh_seg7_texture_07000000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xfefe8cff),
    gsSPLightColor(LIGHT_2, 0x656538ff),
    gsSPVertex(bbh_seg7_vertex_0700FD68, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(bbh_seg7_vertex_0700FE28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  5,  3, 0x0,  0,  4,  1, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 6, 10,  9, 0x0,  6,  8, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x07010D00 - 0x07010D90
static const Gfx bbh_seg7_dl_07010D00[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_0700FF28, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_07010018, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP1Triangle( 0, 10,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07010D90 - 0x07011058
static const Gfx bbh_seg7_dl_07010D90[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_070100C8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  0, 0x0,  4,  0,  2, 0x0),
    gsSP2Triangles( 6,  1,  3, 0x0,  7,  6,  3, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12,  8, 11, 0x0, 12, 13,  8, 0x0),
    gsSPVertex(bbh_seg7_vertex_070101A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bbh_seg7_vertex_07010298, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  9,  8, 0x0),
    gsSP2Triangles( 7, 10,  9, 0x0,  7, 11, 10, 0x0),
    gsSP2Triangles(12, 10, 11, 0x0, 12, 11, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_07010388, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bbh_seg7_vertex_07010478, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_07010568, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_07010668, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  3,  7,  6, 0x0),
    gsSP2Triangles( 8,  3,  5, 0x0,  9,  4,  6, 0x0),
    gsSP2Triangles( 9, 10,  4, 0x0, 11,  5, 12, 0x0),
    gsSP2Triangles(11,  8,  5, 0x0, 13, 11, 12, 0x0),
    gsSP2Triangles(13, 12, 10, 0x0,  9, 13, 10, 0x0),
    gsSP2Triangles( 9, 14, 13, 0x0,  0, 15,  1, 0x0),
    gsSPVertex(bbh_seg7_vertex_07010768, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  2,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle( 8, 10, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_07010858, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP1Triangle( 0, 10,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07011058 - 0x070110A0
static const Gfx bbh_seg7_dl_07011058[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09002800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_07010908, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x070110A0 - 0x070110E8
static const Gfx bbh_seg7_dl_070110A0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bbh_seg7_texture_07002000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_07010988, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x070110E8 - 0x07011120
static const Gfx bbh_seg7_dl_070110E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09009000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_07010A08, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07011120 - 0x07011228
const Gfx bbh_seg7_dl_07011120[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_07010A48),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_07010B58),
    gsSPDisplayList(bbh_seg7_dl_07010C30),
    gsSPDisplayList(bbh_seg7_dl_07010D00),
    gsSPDisplayList(bbh_seg7_dl_07010D90),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_07011058),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_070110A0),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_070110E8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
