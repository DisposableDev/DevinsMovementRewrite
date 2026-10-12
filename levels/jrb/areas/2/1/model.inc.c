// 0x0700D318 - 0x0700D330

// 0x0700D330 - 0x0700D348

// 0x0700D348 - 0x0700D360

// 0x0700D360 - 0x0700D378

// 0x0700D378 - 0x0700D3F8
static const Vtx jrb_seg7_vertex_0700D378[] = {
#include "assets/rom/generated/e43fbb966c8bb1d5a2a7.inc.c"







};

// 0x0700D3F8 - 0x0700D4E8
static const Vtx jrb_seg7_vertex_0700D3F8[] = {
#include "assets/rom/generated/9d0d67edbc0cbde798a1.inc.c"














};

// 0x0700D4E8 - 0x0700D5D8
static const Vtx jrb_seg7_vertex_0700D4E8[] = {
#include "assets/rom/generated/0d8e0a8d869703442ce2.inc.c"














};

// 0x0700D5D8 - 0x0700D6D8
static const Vtx jrb_seg7_vertex_0700D5D8[] = {
#include "assets/rom/generated/cab0b252ba965702e513.inc.c"















};

// 0x0700D6D8 - 0x0700D7C8
static const Vtx jrb_seg7_vertex_0700D6D8[] = {
#include "assets/rom/generated/3d868fe7b05fc4bbc53f.inc.c"














};

// 0x0700D7C8 - 0x0700D878
static const Vtx jrb_seg7_vertex_0700D7C8[] = {
#include "assets/rom/generated/9332553b9c001298753e.inc.c"










};

// 0x0700D878 - 0x0700D928
static const Vtx jrb_seg7_vertex_0700D878[] = {
#include "assets/rom/generated/84bbc627df17f7d57f1d.inc.c"










};

// 0x0700D928 - 0x0700D968
static const Vtx jrb_seg7_vertex_0700D928[] = {
#include "assets/rom/generated/29f205b85bfab9e613d1.inc.c"



};

// 0x0700D968 - 0x0700DA28
static const Vtx jrb_seg7_vertex_0700D968[] = {
#include "assets/rom/generated/01b0ae08a5594fe76395.inc.c"











};

// 0x0700DA28 - 0x0700DB28
static const Vtx jrb_seg7_vertex_0700DA28[] = {
#include "assets/rom/generated/b690264b80f0244f7989.inc.c"















};

// 0x0700DB28 - 0x0700DC08
static const Vtx jrb_seg7_vertex_0700DB28[] = {
#include "assets/rom/generated/78552e2c7a7e2536c2a2.inc.c"













};

// 0x0700DC08 - 0x0700DCF8
static const Vtx jrb_seg7_vertex_0700DC08[] = {
#include "assets/rom/generated/2a939941b10abbcb9cab.inc.c"














};

// 0x0700DCF8 - 0x0700DDD8
static const Vtx jrb_seg7_vertex_0700DCF8[] = {
#include "assets/rom/generated/53789f80f2c3017abaa5.inc.c"













};

// 0x0700DDD8 - 0x0700DED8
static const Vtx jrb_seg7_vertex_0700DDD8[] = {
#include "assets/rom/generated/aaa8612fe202d94717e0.inc.c"















};

// 0x0700DED8 - 0x0700DFD8
static const Vtx jrb_seg7_vertex_0700DED8[] = {
#include "assets/rom/generated/ed8275f8936740df8f2b.inc.c"















};

// 0x0700DFD8 - 0x0700E0C8
static const Vtx jrb_seg7_vertex_0700DFD8[] = {
#include "assets/rom/generated/bdaafb8b9c9bc25961f7.inc.c"














};

// 0x0700E0C8 - 0x0700E1C8
static const Vtx jrb_seg7_vertex_0700E0C8[] = {
#include "assets/rom/generated/4429589bed69d32a9f24.inc.c"















};

// 0x0700E1C8 - 0x0700E298
static const Vtx jrb_seg7_vertex_0700E1C8[] = {
#include "assets/rom/generated/5e01a191b78e98583bbc.inc.c"












};

// 0x0700E298 - 0x0700E398
static const Vtx jrb_seg7_vertex_0700E298[] = {
#include "assets/rom/generated/9e7521048c715cce3779.inc.c"















};

// 0x0700E398 - 0x0700E458
static const Vtx jrb_seg7_vertex_0700E398[] = {
#include "assets/rom/generated/e5cc24bcf586732d3cbd.inc.c"











};

// 0x0700E458 - 0x0700E518
static const Vtx jrb_seg7_vertex_0700E458[] = {
#include "assets/rom/generated/037c87eaf6fa7500ecea.inc.c"











};

// 0x0700E518 - 0x0700E558
static const Vtx jrb_seg7_vertex_0700E518[] = {
#include "assets/rom/generated/f10b3567caf5b5579622.inc.c"



};

// 0x0700E558 - 0x0700E658
static const Vtx jrb_seg7_vertex_0700E558[] = {
#include "assets/rom/generated/c17caad47221fd93b013.inc.c"















};

// 0x0700E658 - 0x0700E6E8
static const Vtx jrb_seg7_vertex_0700E658[] = {
#include "assets/rom/generated/635c2102df958339be8b.inc.c"








};

// 0x0700E6E8 - 0x0700E8C8
static const Gfx jrb_seg7_dl_0700E6E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09005800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x32786eff),
    gsSPLightColor(LIGHT_2, 0x14302cff),
    gsSPVertex(jrb_seg7_vertex_0700D378, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  5,  3,  4, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  7,  6,  1, 0x0),
    gsSP2Triangles( 7,  5,  6, 0x0,  0,  7,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0x97cdbbff),
    gsSPLightColor(LIGHT_2, 0x3c524aff),
    gsSPVertex(jrb_seg7_vertex_0700D3F8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  3, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 6,  8,  4, 0x0,  5,  7,  9, 0x0),
    gsSP2Triangles( 5,  9, 10, 0x0, 10,  8,  6, 0x0),
    gsSP2Triangles(10,  9,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700D4E8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700D5D8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(12, 14, 13, 0x0,  0,  2, 15, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700D6D8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle( 9, 14, 10, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700D7C8, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP1Triangle( 8,  9, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700E8C8 - 0x0700E998
static const Gfx jrb_seg7_dl_0700E8C8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_0900A000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x32786eff),
    gsSPLightColor(LIGHT_2, 0x14302cff),
    gsSPVertex(jrb_seg7_vertex_0700D878, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP1Triangle(10,  0,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0x97cdbbff),
    gsSPLightColor(LIGHT_2, 0x3c524aff),
    gsSPVertex(jrb_seg7_vertex_0700D928, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(jrb_seg7_vertex_0700D968, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700E998 - 0x0700ECB8
static const Gfx jrb_seg7_dl_0700E998[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xc8ffdcff),
    gsSPLightColor(LIGHT_2, 0x506658ff),
    gsSPVertex(jrb_seg7_vertex_0700DA28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  5,  2,  1, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP2Triangles(13, 12, 14, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700DB28, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  1,  6, 0x0,  3,  7,  1, 0x0),
    gsSP2Triangles( 1,  8,  2, 0x0,  9,  2,  8, 0x0),
    gsSP2Triangles( 7,  3,  5, 0x0,  9,  8, 10, 0x0),
    gsSP1Triangle(11, 12, 13, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700DC08, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  5,  4, 0x0),
    gsSP2Triangles( 7,  8,  5, 0x0,  9, 10,  5, 0x0),
    gsSP2Triangles(11,  5, 10, 0x0, 11,  6,  5, 0x0),
    gsSP2Triangles( 8,  9,  5, 0x0, 12,  6, 11, 0x0),
    gsSP1Triangle( 9, 13, 14, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700DCF8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  2,  1, 0x0,  4,  5,  2, 0x0),
    gsSP2Triangles( 2,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  7,  9, 0x0, 11, 12,  9, 0x0),
    gsSP2Triangles(11,  9, 13, 0x0, 12, 10,  9, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700DDD8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  4,  5,  2, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8,  7, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(14, 15, 13, 0x0, 14, 13, 12, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700DED8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 1,  4,  3, 0x0,  5,  1,  6, 0x0),
    gsSP2Triangles( 5,  4,  1, 0x0,  7,  3,  4, 0x0),
    gsSP2Triangles( 8,  4,  5, 0x0,  7,  4,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9, 12, 10, 0x0),
    gsSP2Triangles( 9, 13, 14, 0x0,  9, 14, 12, 0x0),
    gsSP1Triangle(15, 11, 10, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700DFD8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle(10, 14, 13, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700E0C8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  8,  9, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700E1C8, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(11, 10, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700ECB8 - 0x0700EE28
static const Gfx jrb_seg7_dl_0700ECB8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_0900A800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(jrb_seg7_vertex_0700E298, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  7,  3,  5, 0x0),
    gsSP2Triangles( 5,  8,  6, 0x0,  5,  4,  8, 0x0),
    gsSP2Triangles( 4,  9,  8, 0x0, 10,  7, 11, 0x0),
    gsSP2Triangles( 7, 10,  3, 0x0,  7, 12, 11, 0x0),
    gsSP2Triangles( 7,  6, 12, 0x0,  1, 13,  2, 0x0),
    gsSP2Triangles(13, 14,  2, 0x0, 15,  0,  2, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700E398, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  4,  7, 0x0),
    gsSP2Triangles( 8,  9,  6, 0x0, 10,  8,  6, 0x0),
    gsSP2Triangles( 9,  4,  6, 0x0, 11,  5,  7, 0x0),
    gsSPLightColor(LIGHT_1, 0x32786eff),
    gsSPLightColor(LIGHT_2, 0x14302cff),
    gsSPVertex(jrb_seg7_vertex_0700E458, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  2,  4,  5, 0x0),
    gsSP2Triangles( 2,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  7,  9, 0x0,  3, 10,  9, 0x0),
    gsSP2Triangles( 3, 11, 10, 0x0,  0, 11,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(jrb_seg7_vertex_0700E518, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700EE28 - 0x0700EF00
static const Gfx jrb_seg7_dl_0700EE28[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09002800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xc8ffdcff),
    gsSPLightColor(LIGHT_2, 0x506658ff),
    gsSPVertex(jrb_seg7_vertex_0700E558, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  5,  3, 0x0,  0,  6,  5, 0x0),
    gsSP2Triangles( 0,  7,  6, 0x0,  0,  8,  7, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  0,  9, 10, 0x0),
    gsSP2Triangles( 0, 11,  9, 0x0,  0, 12, 11, 0x0),
    gsSP2Triangles( 0, 13, 12, 0x0,  0,  2, 13, 0x0),
    gsSP2Triangles( 0, 10, 14, 0x0,  0, 14, 15, 0x0),
    gsSPVertex(jrb_seg7_vertex_0700E658, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  7, 0x0,  3,  5,  6, 0x0),
    gsSP1Triangle( 3,  8,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700EF00 - 0x0700EF98
const Gfx jrb_seg7_dl_0700EF00[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_0700E6E8),
    gsSPDisplayList(jrb_seg7_dl_0700E8C8),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_0700E998),
    gsSPDisplayList(jrb_seg7_dl_0700ECB8),
    gsSPDisplayList(jrb_seg7_dl_0700EE28),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
