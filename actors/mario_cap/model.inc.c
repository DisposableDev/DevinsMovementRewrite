// Shared cap meshes and palette-independent Metal/wing materials.

// 0x0301CF50
ALIGNED8 static const Texture mario_cap_seg3_texture_0301CF50[] = {
#include "actors/mario_cap/mario_cap_metal.rgba16.inc.c"
};

// 0x0301E750
ALIGNED8 static const Texture mario_cap_seg3_texture_0301E750[] = {
#include "actors/mario_cap/mario_cap_wing.rgba16.inc.c"
};

// 0x0301F750
ALIGNED8 static const Texture mario_cap_seg3_texture_0301F750[] = {
#include "actors/mario_cap/mario_cap_wing_tip.rgba16.inc.c"
};

// 0x03020750
ALIGNED8 static const Texture mario_cap_seg3_texture_03020750[] = {
#include "actors/mario_cap/mario_cap_metal_wing_unused.rgba16.inc.c"
};

// 0x03021750
ALIGNED8 static const Texture mario_cap_seg3_texture_03021750[] = {
#include "actors/mario_cap/mario_cap_metal_wing_tip_unused.rgba16.inc.c"
};

// 0x03022750
static const Vtx mario_cap_seg3_vertex_03022750[] = {
#include "assets/rom/generated/c68e9d8081863f100a65.inc.c"






};

// 0x030227C0
static const Vtx mario_cap_seg3_vertex_030227C0[] = {
#include "assets/rom/generated/d9244412116774b371ec.inc.c"















};

// 0x030228C0
static const Vtx mario_cap_seg3_vertex_030228C0[] = {
#include "assets/rom/generated/29c964cd907610185a96.inc.c"














};

// 0x030229B0
static const Vtx mario_cap_seg3_vertex_030229B0[] = {
#include "assets/rom/generated/000b3af172a599540dc8.inc.c"














};

// 0x03022AA0
static const Vtx mario_cap_seg3_vertex_03022AA0[] = {
#include "assets/rom/generated/8225f100032be4ea8890.inc.c"








};

// 0x03022B30 - 0x03022B68
const Gfx mario_cap_seg3_dl_03022B30[] = {
    gsSPVertex(mario_cap_seg3_vertex_03022750, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 3,  5,  4, 0x0,  2,  3,  0, 0x0),
    gsSP1Triangle( 5,  6,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x03022B68 - 0x03022CC8
const Gfx mario_cap_seg3_dl_03022B68[] = {
    gsSPVertex(mario_cap_seg3_vertex_030227C0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 3,  1,  0, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 3,  0,  5, 0x0,  5,  7,  8, 0x0),
    gsSP2Triangles( 5,  8,  6, 0x0,  0,  2,  5, 0x0),
    gsSP2Triangles( 6,  9, 10, 0x0,  8,  9,  6, 0x0),
    gsSP2Triangles(11,  8,  7, 0x0,  8, 12,  9, 0x0),
    gsSP2Triangles(12,  8, 11, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(mario_cap_seg3_vertex_030228C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 4,  2,  1, 0x0,  5,  1,  0, 0x0),
    gsSP2Triangles( 1,  6,  3, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9,  7,  6, 0x0, 10,  4,  3, 0x0),
    gsSP2Triangles( 9, 11,  7, 0x0,  7, 11,  8, 0x0),
    gsSP2Triangles(12, 11,  9, 0x0, 13, 14,  4, 0x0),
    gsSPVertex(mario_cap_seg3_vertex_030229B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 5,  3,  1, 0x0,  1,  4,  2, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  0,  2,  7, 0x0),
    gsSP2Triangles( 8,  7,  2, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 10,  9, 0x0,  7, 10,  0, 0x0),
    gsSP2Triangles(10,  7, 11, 0x0,  0, 10, 12, 0x0),
    gsSP2Triangles(11,  8, 13, 0x0, 14, 12,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x03022CC8 - 0x03022D10
const Gfx mario_cap_seg3_dl_03022CC8[] = {
    gsSPVertex(mario_cap_seg3_vertex_03022AA0, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  7,  6,  5, 0x0),
    gsSP2Triangles( 0,  7,  5, 0x0,  0,  5,  1, 0x0),
    gsSP1Triangle( 2,  8,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x03022D38
static const Vtx mario_cap_seg3_vertex_03022D38[] = {
#include "assets/rom/generated/ff3706409d5b85f47f1b.inc.c"











};

// 0x03022DF8
static const Vtx mario_cap_seg3_vertex_03022DF8[] = {
#include "assets/rom/generated/2e2816692e5d724ba7a9.inc.c"







};

// 0x03022E78 - 0x03022EA8
const Gfx mario_cap_seg3_dl_03022E78[] = {
    gsSPVertex(mario_cap_seg3_vertex_03022D38, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x03022EA8 - 0x03022ED8
const Gfx mario_cap_seg3_dl_03022EA8[] = {
    gsSPVertex(mario_cap_seg3_vertex_03022DF8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x03022ED8 - 0x03022F20
const Gfx mario_cap_seg3_dl_03022ED8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPEndDisplayList(),
};

// 0x03022F20 - 0x03022F48
const Gfx mario_cap_seg3_dl_03022F20[] = {
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x03022FF8 - 0x030230B0
const Gfx mario_cap_seg3_dl_03022FF8[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPLoadTextureBlock(mario_cap_seg3_texture_0301CF50, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0x07C0, 0x0F80, 0, G_TX_RENDERTILE, G_ON),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPDisplayList(mario_cap_seg3_dl_03022B30),
    gsSPDisplayList(mario_cap_seg3_dl_03022B68),
    gsSPDisplayList(mario_cap_seg3_dl_03022CC8),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPTexture(0x07C0, 0x0F80, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};

// 0x030230B0 - 0x03023108
const Gfx mario_cap_seg3_dl_030230B0[] = {
    gsSPDisplayList(mario_cap_seg3_dl_03022ED8),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_cap_seg3_texture_0301E750),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_cap_seg3_dl_03022E78),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_cap_seg3_texture_0301F750),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_cap_seg3_dl_03022EA8),
    gsSPDisplayList(mario_cap_seg3_dl_03022F20),
    gsSPEndDisplayList(),
};

// 0x03023108 - 0x03023160
const Gfx mario_cap_seg3_dl_03023108[] = {
    gsSPDisplayList(mario_cap_seg3_dl_03022ED8),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_cap_seg3_texture_03020750),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_cap_seg3_dl_03022E78),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_cap_seg3_texture_03021750),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_cap_seg3_dl_03022EA8),
    gsSPDisplayList(mario_cap_seg3_dl_03022F20),
    gsSPEndDisplayList(),
};

// 0x03023298 - 0x030233D0
const Gfx mario_cap_seg3_dl_03023298[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPLoadTextureBlock(mario_cap_seg3_texture_0301CF50, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0x07C0, 0x0F80, 0, G_TX_RENDERTILE, G_ON),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPDisplayList(mario_cap_seg3_dl_03022B30),
    gsSPDisplayList(mario_cap_seg3_dl_03022B68),
    gsSPDisplayList(mario_cap_seg3_dl_03022CC8),
    gsSPTexture(0x0F80, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN | G_LIGHTING | G_CULL_BACK),
    gsDPSetCombineMode(G_CC_DECALFADEA, G_CC_DECALFADEA),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_cap_seg3_texture_03020750),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_cap_seg3_dl_03022E78),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_cap_seg3_texture_03021750),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_cap_seg3_dl_03022EA8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};

#define MARIO_PALETTE_VARIANT 0
#define MARIO_PALETTE_NAME(name) name
#include "actors/mario/palette.h"
#include "actors/mario_cap/palette_materials.inc.c"
#include "actors/mario/palette_undef.h"
#undef MARIO_PALETTE_NAME
#undef MARIO_PALETTE_VARIANT

#define MARIO_PALETTE_VARIANT 1
#define MARIO_PALETTE_NAME(name) he_thicc_##name
#include "actors/mario/palette.h"
#include "actors/mario_cap/palette_materials.inc.c"
#include "actors/mario/palette_undef.h"
#undef MARIO_PALETTE_NAME
#undef MARIO_PALETTE_VARIANT
