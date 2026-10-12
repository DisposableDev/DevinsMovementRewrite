// 0x0700DE68 - 0x0700DE80

// 0x0700DE80 - 0x0700DE98

// 0x0700DE98 - 0x0700DEB0

// 0x0700DEB0 - 0x0700DEC8

// 0x0700DEC8 - 0x0700DF88
static const Vtx ccm_seg7_vertex_0700DEC8[] = {
#include "assets/rom/generated/9cb80901e4fe55f36789.inc.c"











};

// 0x0700DF88 - 0x0700E068
static const Vtx ccm_seg7_vertex_0700DF88[] = {
#include "assets/rom/generated/df01a02c6b4d7a4b1683.inc.c"













};

// 0x0700E068 - 0x0700E168
static const Vtx ccm_seg7_vertex_0700E068[] = {
#include "assets/rom/generated/e433663a2fcca632a6f4.inc.c"















};

// 0x0700E168 - 0x0700E268
static const Vtx ccm_seg7_vertex_0700E168[] = {
#include "assets/rom/generated/68c85acf8b1e79bcdf69.inc.c"















};

// 0x0700E268 - 0x0700E2C8
static const Vtx ccm_seg7_vertex_0700E268[] = {
#include "assets/rom/generated/ce7f4a0998da7442e4ec.inc.c"





};

// 0x0700E2C8 - 0x0700E3A8
static const Vtx ccm_seg7_vertex_0700E2C8[] = {
#include "assets/rom/generated/6800d1713f2432078103.inc.c"













};

// 0x0700E3A8 - 0x0700E428
static const Vtx ccm_seg7_vertex_0700E3A8[] = {
#include "assets/rom/generated/1f519c84941f8029c45d.inc.c"







};

// 0x0700E428 - 0x0700E468
static const Vtx ccm_seg7_vertex_0700E428[] = {
#include "assets/rom/generated/ad9a1d139547d729ebfe.inc.c"



};

// 0x0700E468 - 0x0700E4A8
static const Vtx ccm_seg7_vertex_0700E468[] = {
#include "assets/rom/generated/d26ed3aea00c2612da22.inc.c"



};

// 0x0700E4A8 - 0x0700E530
static const Gfx ccm_seg7_dl_0700E4A8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09008800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ccm_seg7_vertex_0700DEC8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10,  9,  8, 0x0, 10, 11,  9, 0x0),
    gsSP2Triangles(11, 10,  4, 0x0, 10,  5,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700E530 - 0x0700E668
static const Gfx ccm_seg7_dl_0700E530[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ccm_seg7_vertex_0700DF88, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 11, 13, 12, 0x0),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(ccm_seg7_vertex_0700E068, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700E168, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700E268, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xbbbbbbff),
    gsSPLightColor(LIGHT_2, 0x5d5d5dff),
    gsSPVertex(ccm_seg7_vertex_0700E2C8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700E668 - 0x0700E6C0
static const Gfx ccm_seg7_dl_0700E668[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09008000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ccm_seg7_vertex_0700E3A8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700E6C0 - 0x0700E708
static const Gfx ccm_seg7_dl_0700E6C0[] = {
    gsSPVertex(ccm_seg7_vertex_0700E428, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(ccm_seg7_vertex_0700E468, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700E708 - 0x0700E790
const Gfx ccm_seg7_dl_0700E708[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ccm_seg7_dl_0700E4A8),
    gsSPDisplayList(ccm_seg7_dl_0700E530),
    gsSPDisplayList(ccm_seg7_dl_0700E668),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(ccm_seg7_dl_0700E6C0),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
