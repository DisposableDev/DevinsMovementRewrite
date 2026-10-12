// 0x07019B68 - 0x07019B80

// 0x07019B80 - 0x07019B98

// 0x07019B98 - 0x07019BB0

// 0x07019BB0 - 0x07019BC8

// 0x07019BC8 - 0x07019BE0

// 0x07019BE0 - 0x07019CD0
static const Vtx rr_seg7_vertex_07019BE0[] = {
#include "assets/rom/generated/882c26b182cdfe6c3f1c.inc.c"














};

// 0x07019CD0 - 0x07019DD0
static const Vtx rr_seg7_vertex_07019CD0[] = {
#include "assets/rom/generated/cf5c6ab70fb0f5c0d90f.inc.c"















};

// 0x07019DD0 - 0x07019E80
static const Vtx rr_seg7_vertex_07019DD0[] = {
#include "assets/rom/generated/70576200e7fe150f445c.inc.c"










};

// 0x07019E80 - 0x07019EC0
static const Vtx rr_seg7_vertex_07019E80[] = {
#include "assets/rom/generated/72d8dc8d6682c41769ef.inc.c"



};

// 0x07019EC0 - 0x07019F40
static const Vtx rr_seg7_vertex_07019EC0[] = {
#include "assets/rom/generated/e0971ea2bcc318d511e4.inc.c"







};

// 0x07019F40 - 0x0701A040
static const Vtx rr_seg7_vertex_07019F40[] = {
#include "assets/rom/generated/6d3cbcbd330800149fc5.inc.c"















};

// 0x0701A040 - 0x0701A140
static const Vtx rr_seg7_vertex_0701A040[] = {
#include "assets/rom/generated/454d981dabbfabef44de.inc.c"















};

// 0x0701A140 - 0x0701A230
static const Vtx rr_seg7_vertex_0701A140[] = {
#include "assets/rom/generated/a922fb3054c7b322535a.inc.c"














};

// 0x0701A230 - 0x0701A2C0
static const Vtx rr_seg7_vertex_0701A230[] = {
#include "assets/rom/generated/1debeba7d30895acf0e5.inc.c"








};

// 0x0701A2C0 - 0x0701A3A8
static const Gfx rr_seg7_dl_0701A2C0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xc8c8c8ff),
    gsSPLightColor(LIGHT_2, 0x323232ff),
    gsSPVertex(rr_seg7_vertex_07019BE0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 0, 10,  1, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07019CD0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  4, 13,  5, 0x0),
    gsSP2Triangles(10, 14, 11, 0x0, 10, 15, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07019DD0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP1Triangle( 8,  9, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x0701A3A8 - 0x0701A3E0
static const Gfx rr_seg7_dl_0701A3A8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_07019E80, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0701A3E0 - 0x0701A4E8
static const Gfx rr_seg7_dl_0701A3E0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09003000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x8eac52ff),
    gsSPLightColor(LIGHT_2, 0x232b14ff),
    gsSPVertex(rr_seg7_vertex_07019EC0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0x7fac52ff),
    gsSPLightColor(LIGHT_2, 0x1f2b14ff),
    gsSPVertex(rr_seg7_vertex_07019F40, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0,  2, 15, 0x0),
    gsSPLightColor(LIGHT_1, 0xbfaa3fff),
    gsSPLightColor(LIGHT_2, 0x2f2a0fff),
    gsSPVertex(rr_seg7_vertex_0701A040, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0, 15,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0701A4E8 - 0x0701A568
static const Gfx rr_seg7_dl_0701A4E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09002000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x528eacff),
    gsSPLightColor(LIGHT_2, 0x14232bff),
    gsSPVertex(rr_seg7_vertex_0701A140, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_0701A230, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  7,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0701A568 - 0x0701A608
const Gfx rr_seg7_dl_0701A568[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(rr_seg7_dl_0701A2C0),
    gsSPDisplayList(rr_seg7_dl_0701A3A8),
    gsSPDisplayList(rr_seg7_dl_0701A3E0),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(rr_seg7_dl_0701A4E8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
