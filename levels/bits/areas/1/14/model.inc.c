// 0x0700BA88 - 0x0700BB88
static const Vtx bits_seg7_vertex_0700BA88[] = {
#include "assets/rom/generated/9c7b4cf7e5845cd8fefe.inc.c"















};

// 0x0700BB88 - 0x0700BC08
static const Vtx bits_seg7_vertex_0700BB88[] = {
#include "assets/rom/generated/9f46cce27d0595994d03.inc.c"







};

// 0x0700BC08 - 0x0700BD08
static const Vtx bits_seg7_vertex_0700BC08[] = {
#include "assets/rom/generated/8019c2981182e022aa0b.inc.c"















};

// 0x0700BD08 - 0x0700BE08
static const Vtx bits_seg7_vertex_0700BD08[] = {
#include "assets/rom/generated/529a0ffabb79d7894023.inc.c"















};

// 0x0700BE08 - 0x0700BE48
static const Vtx bits_seg7_vertex_0700BE08[] = {
#include "assets/rom/generated/fb88da8d7eee3f239296.inc.c"



};

// 0x0700BE48 - 0x0700BF48
static const Vtx bits_seg7_vertex_0700BE48[] = {
#include "assets/rom/generated/7a036de7d44790217d6d.inc.c"















};

// 0x0700BF48 - 0x0700BF88
static const Vtx bits_seg7_vertex_0700BF48[] = {
#include "assets/rom/generated/772133ee2faa445828d7.inc.c"



};

// 0x0700BF88 - 0x0700C088
static const Vtx bits_seg7_vertex_0700BF88[] = {
#include "assets/rom/generated/6919772d7ad214fe281a.inc.c"















};

// 0x0700C088 - 0x0700C188
static const Vtx bits_seg7_vertex_0700C088[] = {
#include "assets/rom/generated/9ab6e1f37596f8d07598.inc.c"















};

// 0x0700C188 - 0x0700C288
static const Vtx bits_seg7_vertex_0700C188[] = {
#include "assets/rom/generated/1b3d6588b39af1a8e06e.inc.c"















};

// 0x0700C288 - 0x0700C378
static const Vtx bits_seg7_vertex_0700C288[] = {
#include "assets/rom/generated/48d1f0199ef5b0bb177b.inc.c"














};

// 0x0700C378 - 0x0700C468
static const Vtx bits_seg7_vertex_0700C378[] = {
#include "assets/rom/generated/454101136da98d989c97.inc.c"














};

// 0x0700C468 - 0x0700C558
static const Vtx bits_seg7_vertex_0700C468[] = {
#include "assets/rom/generated/318ba55eb1d9cc826a90.inc.c"














};

// 0x0700C558 - 0x0700C648
static const Vtx bits_seg7_vertex_0700C558[] = {
#include "assets/rom/generated/c5b45ece32ed92fd23c5.inc.c"














};

// 0x0700C648 - 0x0700C738
static const Vtx bits_seg7_vertex_0700C648[] = {
#include "assets/rom/generated/143d598432b372f093c1.inc.c"














};

// 0x0700C738 - 0x0700C828
static const Vtx bits_seg7_vertex_0700C738[] = {
#include "assets/rom/generated/083cf864dcff827662f5.inc.c"














};

// 0x0700C828 - 0x0700C8C8
static const Vtx bits_seg7_vertex_0700C828[] = {
#include "assets/rom/generated/aad33818a3338672d808.inc.c"









};

// 0x0700C8C8 - 0x0700C908
static const Vtx bits_seg7_vertex_0700C8C8[] = {
#include "assets/rom/generated/db472f0c3026a67a2791.inc.c"



};

// 0x0700C908 - 0x0700C998
static const Gfx bits_seg7_dl_0700C908[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_0700BA88, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700BB88, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700C998 - 0x0700CA70
static const Gfx bits_seg7_dl_0700C998[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09008000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_0700BC08, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  2,  3, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  6,  8, 0x0),
    gsSP2Triangles(10,  9,  8, 0x0, 11, 10,  8, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700BD08, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700BE08, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700CA70 - 0x0700CAF0
static const Gfx bits_seg7_dl_0700CA70[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_0700BE48, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700BF48, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700CAF0 - 0x0700CDA0
static const Gfx bits_seg7_dl_0700CAF0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_0700BF88, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0,  2, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700C088, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0,  2, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700C188, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0,  2, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700C288, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bits_seg7_vertex_0700C378, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(bits_seg7_vertex_0700C468, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bits_seg7_vertex_0700C558, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 4,  6, 14, 0x0),
    gsSPVertex(bits_seg7_vertex_0700C648, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10,  9,  6, 0x0, 10, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bits_seg7_vertex_0700C738, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bits_seg7_vertex_0700C828, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700CDA0 - 0x0700CDC0
static const Gfx bits_seg7_dl_0700CDA0[] = {
    gsSPVertex(bits_seg7_vertex_0700C8C8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700CDC0 - 0x0700CE50
const Gfx bits_seg7_dl_0700CDC0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bits_seg7_dl_0700C908),
    gsSPDisplayList(bits_seg7_dl_0700C998),
    gsSPDisplayList(bits_seg7_dl_0700CA70),
    gsSPDisplayList(bits_seg7_dl_0700CAF0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(bits_seg7_dl_0700CDA0),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
