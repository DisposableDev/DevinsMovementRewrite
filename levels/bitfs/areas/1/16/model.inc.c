// 0x0700AC00 - 0x0700AD00
static const Vtx bitfs_seg7_vertex_0700AC00[] = {
#include "assets/rom/generated/bcd972d4fb318b81066e.inc.c"















};

// 0x0700AD00 - 0x0700AE00
static const Vtx bitfs_seg7_vertex_0700AD00[] = {
#include "assets/rom/generated/a8c8a93e18c5e5a6523a.inc.c"















};

// 0x0700AE00 - 0x0700AEE0
static const Vtx bitfs_seg7_vertex_0700AE00[] = {
#include "assets/rom/generated/caf0db16b6fdc60066bd.inc.c"













};

// 0x0700AEE0 - 0x0700AFD0
static const Vtx bitfs_seg7_vertex_0700AEE0[] = {
#include "assets/rom/generated/73f502272a06758fc39c.inc.c"














};

// 0x0700AFD0 - 0x0700B0C0
static const Vtx bitfs_seg7_vertex_0700AFD0[] = {
#include "assets/rom/generated/3376b54c990d1c87be1b.inc.c"














};

// 0x0700B0C0 - 0x0700B1B0
static const Vtx bitfs_seg7_vertex_0700B0C0[] = {
#include "assets/rom/generated/51b25dd4471eff110c4e.inc.c"














};

// 0x0700B1B0 - 0x0700B260
static const Vtx bitfs_seg7_vertex_0700B1B0[] = {
#include "assets/rom/generated/674143592d33e1a6df0d.inc.c"










};

// 0x0700B260 - 0x0700B360
static const Vtx bitfs_seg7_vertex_0700B260[] = {
#include "assets/rom/generated/6af8bfa67a3b140f4c85.inc.c"















};

// 0x0700B360 - 0x0700B450
static const Vtx bitfs_seg7_vertex_0700B360[] = {
#include "assets/rom/generated/ea09f696f57d97768bc3.inc.c"














};

// 0x0700B450 - 0x0700B4D0
static const Vtx bitfs_seg7_vertex_0700B450[] = {
#include "assets/rom/generated/cb2557be77ce8d2f0773.inc.c"







};

// 0x0700B4D0 - 0x0700B5D0
static const Vtx bitfs_seg7_vertex_0700B4D0[] = {
#include "assets/rom/generated/37239c93404c1db5a23e.inc.c"















};

// 0x0700B5D0 - 0x0700B6C0
static const Vtx bitfs_seg7_vertex_0700B5D0[] = {
#include "assets/rom/generated/dec78ad6608dc79c9b53.inc.c"














};

// 0x0700B6C0 - 0x0700B7B0
static const Vtx bitfs_seg7_vertex_0700B6C0[] = {
#include "assets/rom/generated/13b0a3bf2d84aa091f73.inc.c"














};

// 0x0700B7B0 - 0x0700B8A0
static const Vtx bitfs_seg7_vertex_0700B7B0[] = {
#include "assets/rom/generated/d08a1e5a42631383fa80.inc.c"














};

// 0x0700B8A0 - 0x0700B910
static const Vtx bitfs_seg7_vertex_0700B8A0[] = {
#include "assets/rom/generated/5e4db5ce96b3768d6de4.inc.c"






};

// 0x0700B910 - 0x0700BA10
static const Vtx bitfs_seg7_vertex_0700B910[] = {
#include "assets/rom/generated/111647c6a2c5aea144ff.inc.c"















};

// 0x0700BA10 - 0x0700BA50
static const Vtx bitfs_seg7_vertex_0700BA10[] = {
#include "assets/rom/generated/077714b914d317b3c517.inc.c"



};

// 0x0700BA50 - 0x0700BC38
static const Gfx bitfs_seg7_dl_0700BA50[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitfs_seg7_vertex_0700AC00, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0, 11, 14, 15, 0x0),
    gsSPVertex(bitfs_seg7_vertex_0700AD00, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 6,  9,  7, 0x0,  6, 10,  9, 0x0),
    gsSP2Triangles( 6,  8, 11, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle(13, 15, 14, 0x0),
    gsSPVertex(bitfs_seg7_vertex_0700AE00, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  7,  4, 0x0),
    gsSP2Triangles( 6,  8,  9, 0x0, 10, 11, 12, 0x0),
    gsSP1Triangle(10, 12, 13, 0x0),
    gsSPVertex(bitfs_seg7_vertex_0700AEE0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bitfs_seg7_vertex_0700AFD0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(bitfs_seg7_vertex_0700B0C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(bitfs_seg7_vertex_0700B1B0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP1Triangle( 8,  9, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700BC38 - 0x0700BD10
static const Gfx bitfs_seg7_dl_0700BC38[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitfs_seg7_vertex_0700B260, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(bitfs_seg7_vertex_0700B360, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  8, 0x0),
    gsSP2Triangles( 5,  8,  9, 0x0,  4,  6, 10, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 11, 13, 14, 0x0),
    gsSPVertex(bitfs_seg7_vertex_0700B450, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700BD10 - 0x0700BE58
static const Gfx bitfs_seg7_dl_0700BD10[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bitfs_seg7_texture_07001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitfs_seg7_vertex_0700B4D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bitfs_seg7_vertex_0700B5D0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(bitfs_seg7_vertex_0700B6C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 4,  6, 14, 0x0),
    gsSPVertex(bitfs_seg7_vertex_0700B7B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bitfs_seg7_vertex_0700B8A0, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  2,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700BE58 - 0x0700BED8
static const Gfx bitfs_seg7_dl_0700BE58[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitfs_seg7_vertex_0700B910, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(bitfs_seg7_vertex_0700BA10, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700BED8 - 0x0700BF90
const Gfx bitfs_seg7_dl_0700BED8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bitfs_seg7_dl_0700BA50),
    gsSPDisplayList(bitfs_seg7_dl_0700BC38),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bitfs_seg7_dl_0700BD10),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bitfs_seg7_dl_0700BE58),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
