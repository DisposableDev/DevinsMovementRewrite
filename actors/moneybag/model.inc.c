// Moneybag

// 0x060039B0
ALIGNED8 static const Texture moneybag_seg6_texture_060039B0[] = {
#include "actors/moneybag/moneybag_mouth.rgba16.inc.c"
};

// 0x060049B0
ALIGNED8 static const Texture moneybag_seg6_texture_060049B0[] = {
#include "actors/moneybag/moneybag_eyes.rgba16.inc.c"
};

// 0x060051B0

// 0x060051C8

// 0x060051E0
static const Vtx moneybag_seg6_vertex_060051E0[] = {
#include "assets/rom/generated/71732e0a8e7d4f186f84.inc.c"









};

// 0x06005280
static const Vtx moneybag_seg6_vertex_06005280[] = {
#include "assets/rom/generated/6e5171ffba34eadcd5f7.inc.c"







};

// 0x06005300 - 0x06005358
const Gfx moneybag_seg6_dl_06005300[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, moneybag_seg6_texture_060039B0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(moneybag_seg6_vertex_060051E0, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  0, 0x0,  8,  4,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x06005358 - 0x060053B8
const Gfx moneybag_seg6_dl_06005358[] = {
    gsSPLightColor(LIGHT_1, 0x7f47ff),
    gsSPLightColor(LIGHT_2, 0x3f23ff),
    gsSPVertex(moneybag_seg6_vertex_06005280, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 3,  0,  4, 0x0,  3,  5,  1, 0x0),
    gsSP2Triangles( 6,  2,  1, 0x0,  1,  5,  6, 0x0),
    gsSP2Triangles( 5,  3,  7, 0x0,  4,  7,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x060053B8 - 0x06005428
const Gfx moneybag_seg6_dl_060053B8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(moneybag_seg6_dl_06005300),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(moneybag_seg6_dl_06005358),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x06005428

// 0x06005440
static const Vtx moneybag_seg6_vertex_06005440[] = {
#include "assets/rom/generated/d489ffb20ea8b6874f53.inc.c"













};

// 0x06005520
static const Vtx moneybag_seg6_vertex_06005520[] = {
#include "assets/rom/generated/c11d52eaa036e6d58a54.inc.c"






};

// 0x06005590 - 0x06005618
const Gfx moneybag_seg6_dl_06005590[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, moneybag_seg6_texture_060039B0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(moneybag_seg6_vertex_06005440, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10,  7, 0x0),
    gsSP2Triangles( 5, 11, 12, 0x0,  0, 13,  1, 0x0),
    gsSPVertex(moneybag_seg6_vertex_06005520, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP1Triangle( 4,  5,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x06005618 - 0x06005688
const Gfx moneybag_seg6_dl_06005618[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(moneybag_seg6_dl_06005590),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x06005688 - 0x060056A8
const Gfx moneybag_seg6_dl_06005688[] = {
    gsSPDisplayList(moneybag_seg6_dl_060053B8),
    gsSPDisplayList(moneybag_seg6_dl_06005618),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};

// 0x060056A8
static const Vtx moneybag_seg6_vertex_060056A8[] = {
#include "assets/rom/generated/abf858056169051ed768.inc.c"





};

// 0x06005708 - 0x06005750
const Gfx moneybag_seg6_dl_06005708[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, moneybag_seg6_texture_060049B0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(moneybag_seg6_vertex_060056A8, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  5,  0,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x06005750 - 0x060057C0
const Gfx moneybag_seg6_dl_06005750[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALFADE, G_CC_DECALFADE),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(moneybag_seg6_dl_06005708),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// Unreferenced light group

// Unreferenced light group

// 0x060057F0

// Unreferenced light group

// 0x06005820
static const Vtx moneybag_seg6_vertex_06005820[] = {
#include "assets/rom/generated/4a8e3784dc8e8f2abfc0.inc.c"










};

// 0x060058D0
static const Vtx moneybag_seg6_vertex_060058D0[] = {
#include "assets/rom/generated/e8fcd3d43a4137def837.inc.c"










};

// 0x06005980 - 0x060059F0
const Gfx moneybag_seg6_dl_06005980[] = {
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPVertex(moneybag_seg6_vertex_06005820, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  7,  8, 0x0),
    gsSP2Triangles( 5,  8,  9, 0x0, 10,  1,  0, 0x0),
    gsSP2Triangles( 0,  4, 10, 0x0,  2,  3,  0, 0x0),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x060059F0 - 0x06005A60
const Gfx moneybag_seg6_dl_060059F0[] = {
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPVertex(moneybag_seg6_vertex_060058D0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  5,  7, 0x0),
    gsSP2Triangles( 9,  8,  7, 0x0,  2,  1, 10, 0x0),
    gsSP2Triangles(10,  3,  2, 0x0,  2,  4,  0, 0x0),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
