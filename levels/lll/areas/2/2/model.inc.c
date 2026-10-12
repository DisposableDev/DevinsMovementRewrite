// 0x070236E8 - 0x07023700

// 0x07023700 - 0x070237E0
static const Vtx lll_seg7_vertex_07023700[] = {
#include "assets/rom/generated/449546459063c54b4617.inc.c"













};

// 0x070237E0 - 0x070238E0
static const Vtx lll_seg7_vertex_070237E0[] = {
#include "assets/rom/generated/05d51fa69fe9bb4faa91.inc.c"















};

// 0x070238E0 - 0x070239E0
static const Vtx lll_seg7_vertex_070238E0[] = {
#include "assets/rom/generated/8b8afe7969df28b43f7a.inc.c"















};

// 0x070239E0 - 0x07023AE0
static const Vtx lll_seg7_vertex_070239E0[] = {
#include "assets/rom/generated/d7c1fac3b80d0f7bda85.inc.c"















};

// 0x07023AE0 - 0x07023BE0
static const Vtx lll_seg7_vertex_07023AE0[] = {
#include "assets/rom/generated/6931b206db0aef60e4fb.inc.c"















};

// 0x07023BE0 - 0x07023CD0
static const Vtx lll_seg7_vertex_07023BE0[] = {
#include "assets/rom/generated/4b531fdc06da773901e4.inc.c"














};

// 0x07023CD0 - 0x07023DC0
static const Vtx lll_seg7_vertex_07023CD0[] = {
#include "assets/rom/generated/5967feb158c6d3f3888b.inc.c"














};

// 0x07023DC0 - 0x07023EB0
static const Vtx lll_seg7_vertex_07023DC0[] = {
#include "assets/rom/generated/7492a4f783238fd23014.inc.c"














};

// 0x07023EB0 - 0x07023FB0
static const Vtx lll_seg7_vertex_07023EB0[] = {
#include "assets/rom/generated/59fad60c87621d4ff623.inc.c"















};

// 0x07023FB0 - 0x070240B0
static const Vtx lll_seg7_vertex_07023FB0[] = {
#include "assets/rom/generated/71ec673d9157ce570360.inc.c"















};

// 0x070240B0 - 0x070241B0
static const Vtx lll_seg7_vertex_070240B0[] = {
#include "assets/rom/generated/3333a120d185e0326552.inc.c"















};

// 0x070241B0 - 0x07024290
static const Vtx lll_seg7_vertex_070241B0[] = {
#include "assets/rom/generated/e6313514f0824f7ce343.inc.c"













};

// 0x07024290 - 0x07024350
static const Vtx lll_seg7_vertex_07024290[] = {
#include "assets/rom/generated/f572d599370ff5a250c7.inc.c"











};

// 0x07024350 - 0x07024390
static const Vtx lll_seg7_vertex_07024350[] = {
#include "assets/rom/generated/94b2fa645a443126cc3a.inc.c"



};

// 0x07024390 - 0x07024480
static const Vtx lll_seg7_vertex_07024390[] = {
#include "assets/rom/generated/8e42270709737bc783d0.inc.c"














};

// 0x07024480 - 0x07024570
static const Vtx lll_seg7_vertex_07024480[] = {
#include "assets/rom/generated/3f7ebf1ea4412e842790.inc.c"














};

// 0x07024570 - 0x07024630
static const Vtx lll_seg7_vertex_07024570[] = {
#include "assets/rom/generated/3d484b1d21dced978da6.inc.c"











};

// 0x07024630 - 0x070247C8
static const Gfx lll_seg7_dl_07024630[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lll_seg7_texture_0700D400),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(lll_seg7_vertex_07023700, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  0,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(lll_seg7_vertex_070237E0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles( 8, 12, 11, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(lll_seg7_vertex_070238E0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14, 15,  6, 0x0),
    gsSPVertex(lll_seg7_vertex_070239E0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  3,  2, 0x0,  8,  9,  3, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(lll_seg7_vertex_07023AE0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 10, 0x0),
    gsSP2Triangles(11, 13, 12, 0x0, 14,  7,  6, 0x0),
    gsSP1Triangle(14, 15,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x070247C8 - 0x07024B08
static const Gfx lll_seg7_dl_070247C8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fire_0900B000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(lll_seg7_vertex_07023BE0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  1,  5,  4, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(lll_seg7_vertex_07023CD0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  2,  5, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  9,  8, 0x0),
    gsSP2Triangles( 7, 10,  9, 0x0, 11,  6,  8, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(lll_seg7_vertex_07023DC0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 3,  2,  5, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 6,  9,  7, 0x0, 10, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(lll_seg7_vertex_07023EB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  5,  9, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 14, 15,  2, 0x0),
    gsSPVertex(lll_seg7_vertex_07023FB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 0,  5,  3, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 4,  1,  7, 0x0,  4,  2,  1, 0x0),
    gsSP2Triangles( 3,  5,  7, 0x0,  3,  7,  1, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 11, 14, 12, 0x0),
    gsSP2Triangles(14, 15, 12, 0x0,  8, 15,  9, 0x0),
    gsSP2Triangles(11, 10,  9, 0x0, 11,  9, 14, 0x0),
    gsSP2Triangles( 8, 12, 15, 0x0,  8, 13, 12, 0x0),
    gsSP1Triangle(14,  9, 15, 0x0),
    gsSPVertex(lll_seg7_vertex_070240B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 0,  5,  1, 0x0,  0,  6,  5, 0x0),
    gsSP2Triangles( 4,  2,  7, 0x0,  4,  3,  2, 0x0),
    gsSP2Triangles( 1,  5,  7, 0x0,  1,  7,  2, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSP2Triangles( 8, 13, 11, 0x0,  8, 14, 13, 0x0),
    gsSPVertex(lll_seg7_vertex_070241B0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  6,  5, 0x0),
    gsSP2Triangles( 7,  8,  6, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 12, 10, 0x0, 12,  1, 13, 0x0),
    gsSP2Triangles( 0, 13,  1, 0x0,  2,  1, 12, 0x0),
    gsSP2Triangles( 2, 12,  9, 0x0, 11, 13,  0, 0x0),
    gsSP2Triangles(11, 10, 13, 0x0, 12, 13, 10, 0x0),
    gsSPVertex(lll_seg7_vertex_07024290, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  0,  7,  8, 0x0),
    gsSP2Triangles( 0,  8,  9, 0x0, 10,  2,  1, 0x0),
    gsSP2Triangles(10, 11,  2, 0x0, 10,  9,  8, 0x0),
    gsSP2Triangles( 0,  2,  7, 0x0, 10,  8, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x07024B08 - 0x07024B40
static const Gfx lll_seg7_dl_07024B08[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fire_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(lll_seg7_vertex_07024350, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07024B40 - 0x07024C18
static const Gfx lll_seg7_dl_07024B40[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fire_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(lll_seg7_vertex_07024390, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(lll_seg7_vertex_07024480, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(lll_seg7_vertex_07024570, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x07024C18 - 0x07024CD8
const Gfx lll_seg7_dl_07024C18[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogFactor(0x0855, 0xF8AB), // This isn't gsSPFogPosition since there is no valid min/max pair that corresponds to 0x0855F8AB
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(lll_seg7_dl_07024630),
    gsSPDisplayList(lll_seg7_dl_070247C8),
    gsSPDisplayList(lll_seg7_dl_07024B08),
    gsSPDisplayList(lll_seg7_dl_07024B40),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
