// 0x0701E948 - 0x0701E960

// 0x0701E960 - 0x0701E978

// 0x0701E978 - 0x0701E9F8
static const Vtx bbh_seg7_vertex_0701E978[] = {
#include "assets/rom/generated/904ccd5af6692b4ed833.inc.c"







};

// 0x0701E9F8 - 0x0701EAE8
static const Vtx bbh_seg7_vertex_0701E9F8[] = {
#include "assets/rom/generated/dd36cf9a2356fb69e63b.inc.c"














};

// 0x0701EAE8 - 0x0701EBD8
static const Vtx bbh_seg7_vertex_0701EAE8[] = {
#include "assets/rom/generated/17de2d5a0bffcc543fca.inc.c"














};

// 0x0701EBD8 - 0x0701EC18
static const Vtx bbh_seg7_vertex_0701EBD8[] = {
#include "assets/rom/generated/53fb325df711746560ca.inc.c"



};

// 0x0701EC18 - 0x0701ED18
static const Gfx bbh_seg7_dl_0701EC18[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09006000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x5a2c1dff),
    gsSPLightColor(LIGHT_2, 0x24110bff),
    gsSPVertex(bbh_seg7_vertex_0701E978, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0x969696ff),
    gsSPLightColor(LIGHT_2, 0x3c3c3cff),
    gsSPVertex(bbh_seg7_vertex_0701E9F8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bbh_seg7_vertex_0701EAE8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(bbh_seg7_vertex_0701EBD8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0701ED18 - 0x0701ED88
const Gfx bbh_seg7_dl_0701ED18[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_0701EC18),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
