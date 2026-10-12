// 0x0700BBB8 - 0x0700BCB8
static const Vtx rr_seg7_vertex_0700BBB8[] = {
#include "assets/rom/generated/8f500f147ab853aff817.inc.c"















};

// 0x0700BCB8 - 0x0700BDB8
static const Vtx rr_seg7_vertex_0700BCB8[] = {
#include "assets/rom/generated/eedfa388e31165a71146.inc.c"















};

// 0x0700BDB8 - 0x0700BEB8
static const Vtx rr_seg7_vertex_0700BDB8[] = {
#include "assets/rom/generated/1fbdceae86273a58c12d.inc.c"















};

// 0x0700BEB8 - 0x0700BF98
static const Vtx rr_seg7_vertex_0700BEB8[] = {
#include "assets/rom/generated/a976d9736cdf7d77064d.inc.c"













};

// 0x0700BF98 - 0x0700C098
static const Vtx rr_seg7_vertex_0700BF98[] = {
#include "assets/rom/generated/7cce32eef3c8292fd42b.inc.c"















};

// 0x0700C098 - 0x0700C188
static const Vtx rr_seg7_vertex_0700C098[] = {
#include "assets/rom/generated/d41e1325f92823852e30.inc.c"














};

// 0x0700C188 - 0x0700C278
static const Vtx rr_seg7_vertex_0700C188[] = {
#include "assets/rom/generated/dc809d7d3946593a59f3.inc.c"














};

// 0x0700C278 - 0x0700C358
static const Vtx rr_seg7_vertex_0700C278[] = {
#include "assets/rom/generated/d394430a3eb57272f60b.inc.c"













};

// 0x0700C358 - 0x0700C458
static const Vtx rr_seg7_vertex_0700C358[] = {
#include "assets/rom/generated/efe09eb1ffc9362697f5.inc.c"















};

// 0x0700C458 - 0x0700C558
static const Vtx rr_seg7_vertex_0700C458[] = {
#include "assets/rom/generated/02d295c3c01b9746dde4.inc.c"















};

// 0x0700C558 - 0x0700C658
static const Vtx rr_seg7_vertex_0700C558[] = {
#include "assets/rom/generated/484c175afcf6bd4278b2.inc.c"















};

// 0x0700C658 - 0x0700C698
static const Vtx rr_seg7_vertex_0700C658[] = {
#include "assets/rom/generated/2e01c6e51fe9fa015dde.inc.c"



};

// 0x0700C698 - 0x0700C820
static const Gfx rr_seg7_dl_0700C698[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_0700BBB8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_0700BCB8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  7, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_0700BDB8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0,  2, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_0700BEB8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11, 12, 0x0),
    gsSP2Triangles( 8, 10, 11, 0x0,  8, 13,  9, 0x0),
    gsSPVertex(rr_seg7_vertex_0700BF98, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700C820 - 0x0700C9B8
static const Gfx rr_seg7_dl_0700C820[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_0700C098, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_0700C188, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_0700C278, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_0700C358, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_0700C458, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 2,  4,  0, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  4,  7,  0, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  0,  7,  1, 0x0),
    gsSP2Triangles( 7,  6,  5, 0x0,  7,  5,  1, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700C9B8 - 0x0700CA38
static const Gfx rr_seg7_dl_0700C9B8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09003000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_0700C558, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_0700C658, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700CA38 - 0x0700CAB8
const Gfx rr_seg7_dl_0700CA38[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(rr_seg7_dl_0700C698),
    gsSPDisplayList(rr_seg7_dl_0700C820),
    gsSPDisplayList(rr_seg7_dl_0700C9B8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
