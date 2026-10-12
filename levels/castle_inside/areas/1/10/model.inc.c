// 0x0702FDD8 - 0x0702FDF0

// 0x0702FDF0 - 0x0702FE70
static const Vtx inside_castle_seg7_vertex_0702FDF0[] = {
#include "assets/rom/generated/57d0f364437f07c0ceb7.inc.c"







};

// 0x0702FE70 - 0x0702FF70
static const Vtx inside_castle_seg7_vertex_0702FE70[] = {
#include "assets/rom/generated/05c0b8285addd9d53fe9.inc.c"














};

// 0x0702FF70 - 0x0702FFF0
static const Vtx inside_castle_seg7_vertex_0702FF70[] = {
#include "assets/rom/generated/741de2a1aed755694a26.inc.c"






};

// 0x0702FFF0 - 0x070300E0
static const Vtx inside_castle_seg7_vertex_0702FFF0[] = {
#include "assets/rom/generated/191903de9921e9f3fe64.inc.c"














};

// 0x070300E0 - 0x070301D0
static const Vtx inside_castle_seg7_vertex_070300E0[] = {
#include "assets/rom/generated/5f28b28675f0d4473f63.inc.c"














};

// 0x070301D0 - 0x070302B0
static const Vtx inside_castle_seg7_vertex_070301D0[] = {
#include "assets/rom/generated/9cace9bfca885cea82b4.inc.c"













};

// 0x070302B0 - 0x070303B0
static const Vtx inside_castle_seg7_vertex_070302B0[] = {
#include "assets/rom/generated/3703bf2618b9a7a1ef6a.inc.c"















};

// 0x070303B0 - 0x07030490
static const Vtx inside_castle_seg7_vertex_070303B0[] = {
#include "assets/rom/generated/27b9fc86e05911b0b90d.inc.c"













};

// 0x07030490 - 0x07030590
static const Vtx inside_castle_seg7_vertex_07030490[] = {
#include "assets/rom/generated/5452ea98c08d11a64613.inc.c"















};

// 0x07030590 - 0x07030670
static const Vtx inside_castle_seg7_vertex_07030590[] = {
#include "assets/rom/generated/444445b39c149de1f7e2.inc.c"














};

// 0x07030670 - 0x07030760
static const Vtx inside_castle_seg7_vertex_07030670[] = {
#include "assets/rom/generated/5cbe72f71c51c818369c.inc.c"













};

// 0x07030760 - 0x07030860
static const Vtx inside_castle_seg7_vertex_07030760[] = {
#include "assets/rom/generated/3c403943882d4d65be3c.inc.c"













};

// 0x07030860 - 0x07030940
static const Vtx inside_castle_seg7_vertex_07030860[] = {
#include "assets/rom/generated/9eb3e614e8bffefc6dba.inc.c"














};

// 0x07030940 - 0x07030A40
const Vtx inside_castle_seg7_vertex_07030940[] = {
#include "assets/rom/generated/27b8d42746f9e1739666.inc.c"













};

// 0x07030A40 - 0x07030B30
static const Vtx inside_castle_seg7_vertex_07030A40[] = {
#include "assets/rom/generated/0447ca71d38fb46b4363.inc.c"















};

// 0x07030B30 - 0x07030C20
static const Vtx inside_castle_seg7_vertex_07030B30[] = {
#include "assets/rom/generated/74cc192584267780d99b.inc.c"















};

// 0x07030C20 - 0x07030D20
static const Vtx inside_castle_seg7_vertex_07030C20[] = {
#include "assets/rom/generated/b6ea0d749a92e8dd8b31.inc.c"













};

// 0x07030D20 - 0x07030E20
static const Vtx inside_castle_seg7_vertex_07030D20[] = {
#include "assets/rom/generated/412c501f333a0aaaf8dd.inc.c"













};

// 0x07030E20 - 0x07030F10
static const Vtx inside_castle_seg7_vertex_07030E20[] = {
#include "assets/rom/generated/4d1ef02ff7a98e98acb6.inc.c"













};

// 0x07030F10 - 0x07030FF0
static const Vtx inside_castle_seg7_vertex_07030F10[] = {
#include "assets/rom/generated/3fc9e7e4e1006664205d.inc.c"













};

// 0x07030FF0 - 0x07031070
static const Vtx inside_castle_seg7_vertex_07030FF0[] = {
#include "assets/rom/generated/d22c800722470867d34e.inc.c"















};

// 0x07031070 - 0x070310D8
static const Gfx inside_castle_seg7_dl_07031070[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x5f5f5fff),
    gsSPVertex(inside_castle_seg7_vertex_0702FDF0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  5,  3, 0x0,  0,  2,  6, 0x0),
    gsSP2Triangles( 0,  4,  7, 0x0,  0,  6,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x070310D8 - 0x07031168
static const Gfx inside_castle_seg7_dl_070310D8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_0702FE70, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702FF70, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  6,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07031168 - 0x07031588
const Gfx inside_castle_seg7_dl_07031168[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09003000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_0702FFF0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles( 3, 10,  4, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070300E0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070301D0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070302B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070303B0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07030490, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07030590, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 4, 12,  5, 0x0,  9, 13, 14, 0x0),
    gsSP1Triangle( 9, 14, 10, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07030670, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07030760, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07030860, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07030940, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  8, 13,  9, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07030A40, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07030B30, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(10, 12, 11, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07030C20, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07030D20, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  7,  9, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07030E20, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07030F10, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  4, 13,  5, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07030FF0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x07031588 - 0x07031608
const Gfx inside_castle_seg7_dl_07031588[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07031070),
    gsSPDisplayList(inside_castle_seg7_dl_070310D8),
    gsSPDisplayList(inside_castle_seg7_dl_07031168),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
