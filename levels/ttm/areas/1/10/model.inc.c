// 0x0700C488 - 0x0700C4A0

// 0x0700C4A0 - 0x0700C4B8

// 0x0700C4B8 - 0x0700C4D0

// 0x0700C4D0 - 0x0700C5D0
static const Vtx ttm_seg7_vertex_0700C4D0[] = {
#include "assets/rom/generated/284f1e6fe714e424bff7.inc.c"















};

// 0x0700C5D0 - 0x0700C600
static const Vtx ttm_seg7_vertex_0700C5D0[] = {
#include "assets/rom/generated/fc8acd083004367e82db.inc.c"


};

// 0x0700C600 - 0x0700C690
static const Vtx ttm_seg7_vertex_0700C600[] = {
#include "assets/rom/generated/b6a08dd45cc8dc26c8aa.inc.c"








};

// 0x0700C690 - 0x0700C780
static const Vtx ttm_seg7_vertex_0700C690[] = {
#include "assets/rom/generated/83fa5c7694dd0c958313.inc.c"














};

// 0x0700C780 - 0x0700C7B0
static const Vtx ttm_seg7_vertex_0700C780[] = {
#include "assets/rom/generated/89219599c1e68e3a2991.inc.c"


};

// 0x0700C7B0 - 0x0700C810
static const Vtx ttm_seg7_vertex_0700C7B0[] = {
#include "assets/rom/generated/170966a375b22f63f74c.inc.c"





};

// 0x0700C810 - 0x0700C850
static const Vtx ttm_seg7_vertex_0700C810[] = {
#include "assets/rom/generated/a89c45e0692c75192021.inc.c"



};

// 0x0700C850 - 0x0700C8C0
static const Vtx ttm_seg7_vertex_0700C850[] = {
#include "assets/rom/generated/9ce0d26ecab251ad3081.inc.c"






};

// 0x0700C8C0 - 0x0700C900
static const Vtx ttm_seg7_vertex_0700C8C0[] = {
#include "assets/rom/generated/37df0b5f7facbfe39fc7.inc.c"



};

// 0x0700C900 - 0x0700C9F8
static const Gfx ttm_seg7_dl_0700C900[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ttm_seg7_vertex_0700C4D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700C5D0, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(ttm_seg7_vertex_0700C600, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  7,  8, 0x0),
    gsSPLightColor(LIGHT_1, 0xbbbbbbff),
    gsSPLightColor(LIGHT_2, 0x5d5d5dff),
    gsSPVertex(ttm_seg7_vertex_0700C690, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700C780, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700C9F8 - 0x0700CA78
static const Gfx ttm_seg7_dl_0700C9F8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ttm_seg7_vertex_0700C7B0, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  5,  3, 0x0,  0,  2,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xbbbbbbff),
    gsSPLightColor(LIGHT_2, 0x5d5d5dff),
    gsSPVertex(ttm_seg7_vertex_0700C810, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700CA78 - 0x0700CAE0
static const Gfx ttm_seg7_dl_0700CA78[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ttm_seg7_texture_07001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ttm_seg7_vertex_0700C850, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 3,  6,  4, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ttm_seg7_vertex_0700C8C0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700CAE0 - 0x0700CB60
const Gfx ttm_seg7_dl_0700CAE0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ttm_seg7_dl_0700C900),
    gsSPDisplayList(ttm_seg7_dl_0700C9F8),
    gsSPDisplayList(ttm_seg7_dl_0700CA78),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
