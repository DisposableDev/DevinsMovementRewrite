// 0x07005990 - 0x070059A8

// 0x070059A8 - 0x070059C0

// 0x070059C0 - 0x070059D8

// 0x070059D8 - 0x07005AC8
static const Vtx jrb_seg7_vertex_070059D8[] = {
#include "assets/rom/generated/07ca1b372c24c1d2ff22.inc.c"














};

// 0x07005AC8 - 0x07005B78
static const Vtx jrb_seg7_vertex_07005AC8[] = {
#include "assets/rom/generated/7371944fbbbf789771a0.inc.c"










};

// 0x07005B78 - 0x07005BF8
static const Vtx jrb_seg7_vertex_07005B78[] = {
#include "assets/rom/generated/6c3ab1f531ad908038eb.inc.c"







};

// 0x07005BF8 - 0x07005CE8
static const Vtx jrb_seg7_vertex_07005BF8[] = {
#include "assets/rom/generated/c2a50b74f815f65098d8.inc.c"














};

// 0x07005CE8 - 0x07005DD8
static const Vtx jrb_seg7_vertex_07005CE8[] = {
#include "assets/rom/generated/ff03f6c02d9b6e86f4cc.inc.c"














};

// 0x07005DD8 - 0x07005EC8
static const Vtx jrb_seg7_vertex_07005DD8[] = {
#include "assets/rom/generated/0cd5e7d0fcaa78667b2b.inc.c"














};

// 0x07005EC8 - 0x07005FB8
static const Vtx jrb_seg7_vertex_07005EC8[] = {
#include "assets/rom/generated/3325075b8cafafaf17ba.inc.c"














};

// 0x07005FB8 - 0x070060B8
static const Vtx jrb_seg7_vertex_07005FB8[] = {
#include "assets/rom/generated/cb85235c50aca0ffb32b.inc.c"















};

// 0x070060B8 - 0x07006138
static const Vtx jrb_seg7_vertex_070060B8[] = {
#include "assets/rom/generated/1415c009ddea6311ada5.inc.c"







};

// 0x07006138 - 0x07006238
static const Vtx jrb_seg7_vertex_07006138[] = {
#include "assets/rom/generated/2894bd9f103c261709ee.inc.c"















};

// 0x07006238 - 0x07006318
static const Vtx jrb_seg7_vertex_07006238[] = {
#include "assets/rom/generated/075b2e9840962f8219c4.inc.c"













};

// 0x07006318 - 0x07006418
static const Vtx jrb_seg7_vertex_07006318[] = {
#include "assets/rom/generated/560163f141bac60a3257.inc.c"















};

// 0x07006418 - 0x070064C8
static const Vtx jrb_seg7_vertex_07006418[] = {
#include "assets/rom/generated/f9f289ef590b3024e41a.inc.c"










};

// 0x070064C8 - 0x07006508
static const Vtx jrb_seg7_vertex_070064C8[] = {
#include "assets/rom/generated/2bb9042ba56882a2a49b.inc.c"



};

// 0x07006508 - 0x070065A8
static const Gfx jrb_seg7_dl_07006508[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_0900A000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(jrb_seg7_vertex_070059D8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(jrb_seg7_vertex_07005AC8, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP1Triangle( 0, 10,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x070065A8 - 0x070067F0
static const Gfx jrb_seg7_dl_070065A8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x262626ff),
    gsSPVertex(jrb_seg7_vertex_07005B78, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 7,  6,  5, 0x0,  7,  5,  0, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(jrb_seg7_vertex_07005BF8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(jrb_seg7_vertex_07005CE8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10,  7,  6, 0x0, 10, 11,  7, 0x0),
    gsSP2Triangles( 9,  8, 12, 0x0,  9, 12, 13, 0x0),
    gsSP2Triangles(13, 11, 10, 0x0, 13, 12, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(jrb_seg7_vertex_07005DD8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles( 0, 12,  1, 0x0,  0, 13, 12, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(jrb_seg7_vertex_07005EC8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 8,  7, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 0,  2, 12, 0x0, 12,  2, 13, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(jrb_seg7_vertex_07005FB8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 2,  1,  3, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP2Triangles(13, 10, 14, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(jrb_seg7_vertex_070060B8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x070067F0 - 0x07006980
static const Gfx jrb_seg7_dl_070067F0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09002800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(jrb_seg7_vertex_07006138, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 3,  1,  4, 0x0,  4,  1,  0, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  7,  6, 0x0),
    gsSP2Triangles( 9, 10,  7, 0x0,  9,  7,  8, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 14, 12, 15, 0x0),
    gsSP2Triangles(15, 12, 11, 0x0, 12, 14, 13, 0x0),
    gsSPVertex(jrb_seg7_vertex_07006238, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  6,  9, 0x0),
    gsSP2Triangles( 6,  5,  9, 0x0,  7,  6,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(jrb_seg7_vertex_07006318, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 6,  2,  7, 0x0,  7,  2,  1, 0x0),
    gsSP2Triangles( 3,  2,  8, 0x0,  2,  9,  8, 0x0),
    gsSP2Triangles( 3, 10, 11, 0x0, 12,  6,  5, 0x0),
    gsSP2Triangles(12, 13,  6, 0x0, 14, 13, 12, 0x0),
    gsSP1Triangle(15, 13, 14, 0x0),
    gsSPVertex(jrb_seg7_vertex_07006418, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  6,  8, 0x0),
    gsSP2Triangles( 9, 10,  4, 0x0, 10,  2,  4, 0x0),
    gsSP1Triangle( 2,  1,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x07006980 - 0x070069B0
static const Gfx jrb_seg7_dl_07006980[] = {
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(jrb_seg7_vertex_070064C8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x070069B0 - 0x07006A98
const Gfx jrb_seg7_dl_070069B0[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(15, 65, 100, 255),
    gsSPFogFactor(0x0724, 0xF9DC), // This isn't gsSPFogPosition since there is no valid min/max pair that corresponds to 0x0724F9DC
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_07006508),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_070065A8),
    gsSPDisplayList(jrb_seg7_dl_070067F0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_PASS2),
    gsSPDisplayList(jrb_seg7_dl_07006980),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
