// 0x07010AD8 - 0x07010AF0

// 0x07010AF0 - 0x07010B08

// 0x07010B08 - 0x07010B20

// 0x07010B20 - 0x07010C10
static const Vtx ttm_seg7_vertex_07010B20[] = {
#include "assets/rom/generated/fd6af41785e300f29928.inc.c"














};

// 0x07010C10 - 0x07010C40
static const Vtx ttm_seg7_vertex_07010C10[] = {
#include "assets/rom/generated/48154da92d22b5956184.inc.c"


};

// 0x07010C40 - 0x07010D30
static const Vtx ttm_seg7_vertex_07010C40[] = {
#include "assets/rom/generated/6bd1a39b8e6f32ed39bd.inc.c"














};

// 0x07010D30 - 0x07010D60
static const Vtx ttm_seg7_vertex_07010D30[] = {
#include "assets/rom/generated/5d789656df380e43f49c.inc.c"


};

// 0x07010D60 - 0x07010DA0
static const Vtx ttm_seg7_vertex_07010D60[] = {
#include "assets/rom/generated/16a72b13e480bb08fe8a.inc.c"



};

// 0x07010DA0 - 0x07010E00
static const Vtx ttm_seg7_vertex_07010DA0[] = {
#include "assets/rom/generated/8ac436cbd85d431c6208.inc.c"





};

// 0x07010E00 - 0x07010F00
static const Vtx ttm_seg7_vertex_07010E00[] = {
#include "assets/rom/generated/d8c4c67d889728d224ed.inc.c"















};

// 0x07010F00 - 0x07010F90
static const Vtx ttm_seg7_vertex_07010F00[] = {
#include "assets/rom/generated/8ee79eba9259b5855b00.inc.c"








};

// 0x07010F90 - 0x07011078
static const Gfx ttm_seg7_dl_07010F90[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(ttm_seg7_vertex_07010B20, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_07010C10, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0xbbbbbbff),
    gsSPLightColor(LIGHT_2, 0x5d5d5dff),
    gsSPVertex(ttm_seg7_vertex_07010C40, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_07010D30, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ttm_seg7_vertex_07010D60, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07011078 - 0x07011128
static const Gfx ttm_seg7_dl_07011078[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(ttm_seg7_vertex_07010DA0, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ttm_seg7_vertex_07010E00, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12,  6,  8, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ttm_seg7_vertex_07010F00, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  7,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x07011128 - 0x070111A0
const Gfx ttm_seg7_dl_07011128[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ttm_seg7_dl_07010F90),
    gsSPDisplayList(ttm_seg7_dl_07011078),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
