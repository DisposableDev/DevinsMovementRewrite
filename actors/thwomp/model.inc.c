// Thwomp

// 0x050098E8

// 0x05009900
ALIGNED8 static const Texture thwomp_seg5_texture_05009900[] = {
#include "actors/thwomp/thwomp_face.rgba16.inc.c"
};

// 0x0500A900
ALIGNED8 static const Texture thwomp_seg5_texture_0500A900[] = {
#include "actors/thwomp/thwomp_surface.rgba16.inc.c"
};

// 0x0500B100
static const Vtx thwomp_seg5_vertex_0500B100[] = {
#include "assets/rom/generated/5754063e2d71f5f224d2.inc.c"














};

// 0x0500B1F0
static const Vtx thwomp_seg5_vertex_0500B1F0[] = {
#include "assets/rom/generated/238e4a0f8df36433485b.inc.c"















};

// 0x0500B2F0
static const Vtx thwomp_seg5_vertex_0500B2F0[] = {
#include "assets/rom/generated/669788ed9f6464f966ec.inc.c"













};

// 0x0500B3D0
static const Vtx thwomp_seg5_vertex_0500B3D0[] = {
#include "assets/rom/generated/fd7d097ba2df80b5e7af.inc.c"















};

// 0x0500B4D0
static const Vtx thwomp_seg5_vertex_0500B4D0[] = {
#include "assets/rom/generated/24eee26e914b8a52ac74.inc.c"





};

// 0x0500B530
static const Vtx thwomp_seg5_vertex_0500B530[] = {
#include "assets/rom/generated/e865f2bf111c4bd84f53.inc.c"



};

// 0x0500B570 - 0x0500B718
const Gfx thwomp_seg5_dl_0500B570[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, thwomp_seg5_texture_0500A900),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(thwomp_seg5_vertex_0500B100, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  0,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13,  9, 0x0,  9, 11, 12, 0x0),
    gsSP1Triangle( 8, 14, 11, 0x0),
    gsSPVertex(thwomp_seg5_vertex_0500B1F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 0,  2,  3, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 1,  6,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0,  8, 11, 12, 0x0),
    gsSP2Triangles( 8, 12,  9, 0x0, 11,  8, 13, 0x0),
    gsSP2Triangles(13,  8, 10, 0x0, 14, 12, 11, 0x0),
    gsSP1Triangle(14, 15, 12, 0x0),
    gsSPVertex(thwomp_seg5_vertex_0500B2F0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 4,  0,  5, 0x0,  0,  2,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  6,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(thwomp_seg5_vertex_0500B3D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 3,  8,  4, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 15, 13, 12, 0x0),
    gsSPVertex(thwomp_seg5_vertex_0500B4D0, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500B718 - 0x0500B750
const Gfx thwomp_seg5_dl_0500B718[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, thwomp_seg5_texture_05009900),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(thwomp_seg5_vertex_0500B530, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500B750 - 0x0500B7D0
const Gfx thwomp_seg5_dl_0500B750[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(thwomp_seg5_dl_0500B570),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(thwomp_seg5_dl_0500B718),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
