// 0x0700B248 - 0x0700B260

// 0x0700B260 - 0x0700B278

// 0x0700B278 - 0x0700B290

// 0x0700B290 - 0x0700B2A8

// 0x0700B2A8 - 0x0700B3A8
static const Vtx ccm_seg7_vertex_0700B2A8[] = {
#include "assets/rom/generated/adc932ead2323f5473f0.inc.c"















};

// 0x0700B3A8 - 0x0700B4A8
static const Vtx ccm_seg7_vertex_0700B3A8[] = {
#include "assets/rom/generated/881c48a06c45b03b27e3.inc.c"















};

// 0x0700B4A8 - 0x0700B5A8
static const Vtx ccm_seg7_vertex_0700B4A8[] = {
#include "assets/rom/generated/69ad9dab0981d52baade.inc.c"















};

// 0x0700B5A8 - 0x0700B6A8
static const Vtx ccm_seg7_vertex_0700B5A8[] = {
#include "assets/rom/generated/008594245e51bc93d0d3.inc.c"















};

// 0x0700B6A8 - 0x0700B7A8
static const Vtx ccm_seg7_vertex_0700B6A8[] = {
#include "assets/rom/generated/a622c43aa93984f01901.inc.c"















};

// 0x0700B7A8 - 0x0700B8A8
static const Vtx ccm_seg7_vertex_0700B7A8[] = {
#include "assets/rom/generated/ca72db34705e020ad5a3.inc.c"















};

// 0x0700B8A8 - 0x0700B988
static const Vtx ccm_seg7_vertex_0700B8A8[] = {
#include "assets/rom/generated/3e59c2f8bbb577be7432.inc.c"













};

// 0x0700B988 - 0x0700BA08
static const Vtx ccm_seg7_vertex_0700B988[] = {
#include "assets/rom/generated/2a295e2f9fd6a5ba0180.inc.c"







};

// 0x0700BA08 - 0x0700BB08
static const Vtx ccm_seg7_vertex_0700BA08[] = {
#include "assets/rom/generated/40cbec988cd0060b18ec.inc.c"















};

// 0x0700BB08 - 0x0700BB38
static const Vtx ccm_seg7_vertex_0700BB08[] = {
#include "assets/rom/generated/b8917592a258df24b497.inc.c"


};

// 0x0700BB38 - 0x0700BB88
static const Vtx ccm_seg7_vertex_0700BB38[] = {
#include "assets/rom/generated/66cf441d0ad7d6fda3b2.inc.c"




};

// 0x0700BB88 - 0x0700BC78
static const Vtx ccm_seg7_vertex_0700BB88[] = {
#include "assets/rom/generated/aff3f3814259bcf26743.inc.c"














};

// 0x0700BC78 - 0x0700BD68
static const Vtx ccm_seg7_vertex_0700BC78[] = {
#include "assets/rom/generated/3d052c34f1e12eba915d.inc.c"














};

// 0x0700BD68 - 0x0700BE58
static const Vtx ccm_seg7_vertex_0700BD68[] = {
#include "assets/rom/generated/923ec2b326951f3c96ab.inc.c"














};

// 0x0700BE58 - 0x0700BEC8
static const Vtx ccm_seg7_vertex_0700BE58[] = {
#include "assets/rom/generated/3c34c72482833375e3b6.inc.c"






};

// 0x0700BEC8 - 0x0700BF88
static const Vtx ccm_seg7_vertex_0700BEC8[] = {
#include "assets/rom/generated/2865733e6d4a42b766a8.inc.c"











};

// 0x0700BF88 - 0x0700C000
static const Gfx ccm_seg7_dl_0700BF88[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ccm_seg7_texture_07000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 4 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ccm_seg7_vertex_0700B2A8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0,  2, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700C000 - 0x0700C220
static const Gfx ccm_seg7_dl_0700C000[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09008000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ccm_seg7_vertex_0700B3A8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700B4A8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700B5A8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700B6A8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700B7A8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 12, 10, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700B8A8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700B988, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(ccm_seg7_vertex_0700BA08, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  9,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700BB08, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0xbbbbbbff),
    gsSPLightColor(LIGHT_2, 0x5d5d5dff),
    gsSPVertex(ccm_seg7_vertex_0700BB38, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP1Triangle( 0,  4,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700C220 - 0x0700C330
static const Gfx ccm_seg7_dl_0700C220[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09001000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ccm_seg7_vertex_0700BB88, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700BC78, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700BD68, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700BE58, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  6,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700C330 - 0x0700C380
static const Gfx ccm_seg7_dl_0700C330[] = {
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(ccm_seg7_vertex_0700BEC8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700C380 - 0x0700C438
const Gfx ccm_seg7_dl_0700C380[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 2, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (4 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ccm_seg7_dl_0700BF88),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ccm_seg7_dl_0700C000),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ccm_seg7_dl_0700C220),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(ccm_seg7_dl_0700C330),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
