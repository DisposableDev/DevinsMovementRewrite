// 0x0700DB48 - 0x0700DB88
static const Vtx pss_seg7_vertex_0700DB48[] = {
#include "assets/rom/generated/c29f5fb4c6eff233b0e1.inc.c"



};

// 0x0700DB88 - 0x0700DC48
static const Vtx pss_seg7_vertex_0700DB88[] = {
#include "assets/rom/generated/a8031fcdc0759a83a333.inc.c"











};

// 0x0700DC48 - 0x0700DD38
static const Vtx pss_seg7_vertex_0700DC48[] = {
#include "assets/rom/generated/49c69a0e50af9db2f03f.inc.c"














};

// 0x0700DD38 - 0x0700DE28
static const Vtx pss_seg7_vertex_0700DD38[] = {
#include "assets/rom/generated/c98c3b7b23994cf63b47.inc.c"














};

// 0x0700DE28 - 0x0700DF18
static const Vtx pss_seg7_vertex_0700DE28[] = {
#include "assets/rom/generated/bbeacb9be498436927c3.inc.c"














};

// 0x0700DF18 - 0x0700DFF8
static const Vtx pss_seg7_vertex_0700DF18[] = {
#include "assets/rom/generated/a2ff7dafd2b62f17fe41.inc.c"













};

// 0x0700DFF8 - 0x0700E0A8
static const Vtx pss_seg7_vertex_0700DFF8[] = {
#include "assets/rom/generated/1de1971882c1083eb57b.inc.c"










};

// 0x0700E0A8 - 0x0700E0E0
static const Gfx pss_seg7_dl_0700E0A8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(pss_seg7_vertex_0700DB48, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700E0E0 - 0x0700E138
static const Gfx pss_seg7_dl_0700E0E0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, pss_seg7_texture_07000000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(pss_seg7_vertex_0700DB88, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700E138 - 0x0700E2B0
static const Gfx pss_seg7_dl_0700E138[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09005800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(pss_seg7_vertex_0700DC48, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 4,  8,  7, 0x0,  5,  7,  9, 0x0),
    gsSP2Triangles( 5,  9, 10, 0x0, 11,  0, 12, 0x0),
    gsSP2Triangles(11, 13,  0, 0x0, 14,  0, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(pss_seg7_vertex_0700DD38, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700DE28, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  7, 0x0,  3,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  6,  8, 0x0),
    gsSP2Triangles( 9, 10,  6, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700DF18, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(pss_seg7_vertex_0700DFF8, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP1Triangle( 8,  9, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700E2B0 - 0x0700E348
const Gfx pss_seg7_dl_0700E2B0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(pss_seg7_dl_0700E0A8),
    gsSPDisplayList(pss_seg7_dl_0700E0E0),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(pss_seg7_dl_0700E138),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
