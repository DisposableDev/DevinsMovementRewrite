// 0x0700FD88 - 0x0700FDA0

// 0x0700FDA0 - 0x0700FDB8

// 0x0700FDB8 - 0x0700FDD0

// 0x0700FDD0 - 0x0700FE10
static const Vtx ccm_seg7_vertex_0700FDD0[] = {
#include "assets/rom/generated/29b69e86b05269fc02e9.inc.c"



};

// 0x0700FE10 - 0x0700FEF0
static const Vtx ccm_seg7_vertex_0700FE10[] = {
#include "assets/rom/generated/6e618c324536908b2910.inc.c"













};

// 0x0700FEF0 - 0x0700FF70
static const Vtx ccm_seg7_vertex_0700FEF0[] = {
#include "assets/rom/generated/c6a80295b0531f98d133.inc.c"







};

// 0x0700FF70 - 0x0700FFB0
static const Vtx ccm_seg7_vertex_0700FF70[] = {
#include "assets/rom/generated/4e971c2f13bd5630fa22.inc.c"



};

// 0x0700FFB0 - 0x07010070
static const Vtx ccm_seg7_vertex_0700FFB0[] = {
#include "assets/rom/generated/a31067ee1c303920b5ff.inc.c"











};

// 0x07010070 - 0x07010160
static const Vtx ccm_seg7_vertex_07010070[] = {
#include "assets/rom/generated/d316a81a078e77890f81.inc.c"














};

// 0x07010160 - 0x070101A0
static const Vtx ccm_seg7_vertex_07010160[] = {
#include "assets/rom/generated/84408793b8b6a38459c5.inc.c"



};

// 0x070101A0 - 0x07010280
static const Vtx ccm_seg7_vertex_070101A0[] = {
#include "assets/rom/generated/acd4efbdd9803426cbc7.inc.c"













};

// 0x07010280 - 0x07010320
static const Vtx ccm_seg7_vertex_07010280[] = {
#include "assets/rom/generated/567a50f8be394f64ee1b.inc.c"









};

// 0x07010320 - 0x07010390
static const Vtx ccm_seg7_vertex_07010320[] = {
#include "assets/rom/generated/9c195ef908c7d5ea02e8.inc.c"






};

// 0x07010390 - 0x07010480
static const Gfx ccm_seg7_dl_07010390[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09003000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ccm_seg7_vertex_0700FDD0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(ccm_seg7_vertex_0700FE10, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 10,  2, 0x0),
    gsSP2Triangles( 2, 10,  0, 0x0, 12,  2, 13, 0x0),
    gsSP2Triangles(13,  2,  1, 0x0, 12, 11,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0xbbbbbbff),
    gsSPLightColor(LIGHT_2, 0x5d5d5dff),
    gsSPVertex(ccm_seg7_vertex_0700FEF0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  4,  3, 0x0,  3,  1,  0, 0x0),
    gsSP1Triangle( 5,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x07010480 - 0x07010518
static const Gfx ccm_seg7_dl_07010480[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09006000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(ccm_seg7_vertex_0700FF70, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSPLightColor(LIGHT_1, 0xbbbbbbff),
    gsSPLightColor(LIGHT_2, 0x5d5d5dff),
    gsSPVertex(ccm_seg7_vertex_0700FFB0, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  4,  7, 0x0),
    gsSP2Triangles( 8,  7,  9, 0x0, 10,  8,  9, 0x0),
    gsSP1Triangle( 0, 11,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07010518 - 0x07010610
static const Gfx ccm_seg7_dl_07010518[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09002000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(ccm_seg7_vertex_07010070, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  7,  4, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ccm_seg7_vertex_07010160, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0xbbbbbbff),
    gsSPLightColor(LIGHT_2, 0x5d5d5dff),
    gsSPVertex(ccm_seg7_vertex_070101A0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(ccm_seg7_vertex_07010280, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x07010610 - 0x07010660
static const Gfx ccm_seg7_dl_07010610[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09002800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ccm_seg7_vertex_07010320, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 3,  5,  4, 0x0,  0,  4,  6, 0x0),
    gsSP1Triangle( 1,  0,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x07010660 - 0x070106D8
const Gfx ccm_seg7_dl_07010660[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ccm_seg7_dl_07010390),
    gsSPDisplayList(ccm_seg7_dl_07010480),
    gsSPDisplayList(ccm_seg7_dl_07010518),
    gsSPDisplayList(ccm_seg7_dl_07010610),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
