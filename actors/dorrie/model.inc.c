// Dorrie

// 0x06009BA0
ALIGNED8 static const Texture dorrie_seg6_texture_06009BA0[] = {
#include "actors/dorrie/dorrie_eye.rgba16.inc.c"
};

// 0x06009DA0
ALIGNED8 static const Texture dorrie_seg6_texture_06009DA0[] = {
#include "actors/dorrie/dorrie_skin.rgba16.inc.c"
};

// 0x0600ADA0
ALIGNED8 static const Texture dorrie_seg6_texture_0600ADA0[] = {
#include "actors/dorrie/dorrie_tongue.rgba16.inc.c"
};

// 0x0600B5A0

// 0x0600B5B8
static const Vtx dorrie_seg6_vertex_0600B5B8[] = {
#include "assets/rom/generated/3b2c1a2152299d25534f.inc.c"





};

// 0x0600B618
static const Vtx dorrie_seg6_vertex_0600B618[] = {
#include "assets/rom/generated/5723562133dca785024b.inc.c"















};

// 0x0600B718
static const Vtx dorrie_seg6_vertex_0600B718[] = {
#include "assets/rom/generated/ed1b52e65d5a13136b8f.inc.c"














};

// 0x0600B808
static const Vtx dorrie_seg6_vertex_0600B808[] = {
#include "assets/rom/generated/8520f06ccd2ce5e3979c.inc.c"













};

// 0x0600B8E8 - 0x0600B940
const Gfx dorrie_seg6_dl_0600B8E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_0600ADA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(dorrie_seg6_vertex_0600B5B8, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  1,  3,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600B940 - 0x0600BAF8
const Gfx dorrie_seg6_dl_0600B940[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_06009DA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(dorrie_seg6_vertex_0600B618, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  1,  0,  5, 0x0),
    gsSP2Triangles( 5,  3,  1, 0x0,  5,  0,  6, 0x0),
    gsSP2Triangles( 6,  0,  2, 0x0,  7,  5,  6, 0x0),
    gsSP2Triangles( 8,  9,  4, 0x0, 10,  4,  9, 0x0),
    gsSP2Triangles( 3, 11,  4, 0x0,  4, 11,  8, 0x0),
    gsSP2Triangles( 2,  4, 10, 0x0,  8, 12,  9, 0x0),
    gsSP2Triangles(12,  8, 13, 0x0, 10,  9, 12, 0x0),
    gsSP2Triangles(13, 14, 12, 0x0, 12, 15, 10, 0x0),
    gsSPVertex(dorrie_seg6_vertex_0600B718, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 2,  5,  0, 0x0,  0,  6,  3, 0x0),
    gsSP2Triangles( 0,  5,  6, 0x0,  3,  7,  4, 0x0),
    gsSP2Triangles( 8,  9,  3, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0,  7,  3,  9, 0x0),
    gsSP2Triangles( 3,  6,  8, 0x0, 12,  2,  1, 0x0),
    gsSP1Triangle(13, 14,  2, 0x0),
    gsSPVertex(dorrie_seg6_vertex_0600B808, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 1,  0,  4, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 3,  2,  5, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 2,  1,  8, 0x0,  4,  9,  1, 0x0),
    gsSP2Triangles( 8,  1,  9, 0x0, 10,  9,  4, 0x0),
    gsSP2Triangles(11,  7,  8, 0x0,  2,  8,  5, 0x0),
    gsSP2Triangles( 8,  9, 12, 0x0,  8,  7,  5, 0x0),
    gsSP2Triangles(13,  9, 10, 0x0, 13, 12,  9, 0x0),
    gsSP1Triangle(11, 12, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600BAF8 - 0x0600BBC0
const Gfx dorrie_seg6_dl_0600BAF8[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(960, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600B8E8),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600B940),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0600BBC0

// 0x0600BBD8
static const Vtx dorrie_seg6_vertex_0600BBD8[] = {
#include "assets/rom/generated/83c6654d4ef7e0e3dde8.inc.c"











};

// 0x0600BC98
static const Vtx dorrie_seg6_vertex_0600BC98[] = {
#include "assets/rom/generated/8e097e73e345d1600fb6.inc.c"















};

// 0x0600BD98
static const Vtx dorrie_seg6_vertex_0600BD98[] = {
#include "assets/rom/generated/6a21c846374d933859c8.inc.c"












};

// 0x0600BE68 - 0x0600BF10
const Gfx dorrie_seg6_dl_0600BE68[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_0600ADA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(dorrie_seg6_vertex_0600BBD8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  0,  3, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 0,  6,  5, 0x0,  4,  6,  0, 0x0),
    gsSP2Triangles( 1,  5,  7, 0x0,  8,  1,  7, 0x0),
    gsSP2Triangles( 8,  2,  1, 0x0,  8,  9,  2, 0x0),
    gsSP2Triangles( 2, 10,  3, 0x0,  2,  9, 10, 0x0),
    gsSP2Triangles( 3, 10, 11, 0x0,  4,  3, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600BF10 - 0x0600C030
const Gfx dorrie_seg6_dl_0600BF10[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_06009DA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(dorrie_seg6_vertex_0600BC98, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  7,  5,  8, 0x0),
    gsSP2Triangles( 7,  9,  5, 0x0,  5,  9,  6, 0x0),
    gsSP2Triangles( 5, 10,  8, 0x0,  5,  4, 10, 0x0),
    gsSP2Triangles( 0,  6,  9, 0x0,  7,  0,  9, 0x0),
    gsSP2Triangles(11,  3,  6, 0x0,  6, 12, 11, 0x0),
    gsSP2Triangles( 6,  0,  2, 0x0,  6,  2, 12, 0x0),
    gsSP2Triangles(13,  1, 14, 0x0, 14,  7,  8, 0x0),
    gsSP2Triangles( 4,  3, 11, 0x0, 11, 15,  4, 0x0),
    gsSPVertex(dorrie_seg6_vertex_0600BD98, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 5,  6,  2, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 4,  3,  7, 0x0,  8,  1,  0, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 5,  1,  8, 0x0,  8, 11,  5, 0x0),
    gsSP2Triangles( 7,  3, 12, 0x0, 12,  3,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600C030 - 0x0600C0F8
const Gfx dorrie_seg6_dl_0600C030[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(960, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600BE68),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600BF10),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0600C0F8

// 0x0600C110
static const Vtx dorrie_seg6_vertex_0600C110[] = {
#include "assets/rom/generated/456b5829aa333cf4b5bd.inc.c"















};

// 0x0600C210
static const Vtx dorrie_seg6_vertex_0600C210[] = {
#include "assets/rom/generated/b719e421382322a2a8fd.inc.c"















};

// 0x0600C310 - 0x0600C468
const Gfx dorrie_seg6_dl_0600C310[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_06009DA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(dorrie_seg6_vertex_0600C110, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  7, 0x0,  4,  8,  7, 0x0),
    gsSP2Triangles( 4,  3,  8, 0x0,  6,  5,  4, 0x0),
    gsSP2Triangles( 7,  8,  0, 0x0,  3,  1,  8, 0x0),
    gsSP2Triangles( 0,  8,  1, 0x0,  6,  7,  9, 0x0),
    gsSP2Triangles( 7, 10,  9, 0x0,  7,  0, 10, 0x0),
    gsSP2Triangles( 3, 11,  1, 0x0, 12, 11,  3, 0x0),
    gsSP2Triangles(12,  3,  5, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(dorrie_seg6_vertex_0600C210, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  4,  5,  0, 0x0),
    gsSP2Triangles( 6,  4,  2, 0x0,  2,  3,  6, 0x0),
    gsSP2Triangles( 6,  3,  7, 0x0,  6,  8,  4, 0x0),
    gsSP2Triangles( 6,  7,  9, 0x0,  6,  9,  8, 0x0),
    gsSP2Triangles(10,  4,  8, 0x0,  4, 10,  5, 0x0),
    gsSP2Triangles( 5, 10, 11, 0x0, 12, 13,  9, 0x0),
    gsSP2Triangles( 9, 13,  8, 0x0, 12,  9, 14, 0x0),
    gsSP2Triangles(11, 10, 15, 0x0, 15, 13, 12, 0x0),
    gsSP1Triangle(15, 10, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600C468 - 0x0600C510
const Gfx dorrie_seg6_dl_0600C468[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(960, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600C310),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0600C510

// 0x0600C528
static const Vtx dorrie_seg6_vertex_0600C528[] = {
#include "assets/rom/generated/772bc8157c8801451370.inc.c"















};

// 0x0600C628
static const Vtx dorrie_seg6_vertex_0600C628[] = {
#include "assets/rom/generated/8985ef74c511da07c529.inc.c"















};

// 0x0600C728
static const Vtx dorrie_seg6_vertex_0600C728[] = {
#include "assets/rom/generated/28bdc1083dd8e9c34e80.inc.c"


};

// 0x0600C758 - 0x0600C8B8
const Gfx dorrie_seg6_dl_0600C758[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_06009DA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(dorrie_seg6_vertex_0600C528, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  4,  6, 0x0),
    gsSP2Triangles( 8,  3,  5, 0x0,  7,  6,  9, 0x0),
    gsSP2Triangles( 4,  1,  6, 0x0,  6,  1, 10, 0x0),
    gsSP2Triangles( 9,  6, 10, 0x0,  2,  4,  3, 0x0),
    gsSP2Triangles( 4,  2,  1, 0x0,  0, 11, 10, 0x0),
    gsSP2Triangles( 0, 10,  1, 0x0, 12, 13, 14, 0x0),
    gsSP2Triangles(11, 12, 14, 0x0,  2,  3, 15, 0x0),
    gsSPVertex(dorrie_seg6_vertex_0600C628, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  2,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  5,  4,  6, 0x0),
    gsSP2Triangles( 0,  2,  7, 0x0,  3,  8,  6, 0x0),
    gsSP2Triangles( 5,  6,  9, 0x0,  9,  6, 10, 0x0),
    gsSP2Triangles(10,  6,  8, 0x0, 11,  9, 10, 0x0),
    gsSP2Triangles( 1,  0, 12, 0x0, 13, 11, 10, 0x0),
    gsSP2Triangles(10,  8, 13, 0x0, 13,  8, 14, 0x0),
    gsSP2Triangles(14,  8,  3, 0x0, 12, 15,  1, 0x0),
    gsSPVertex(dorrie_seg6_vertex_0600C728, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600C8B8 - 0x0600C960
const Gfx dorrie_seg6_dl_0600C8B8[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(960, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600C758),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0600C960

// 0x0600C978
static const Vtx dorrie_seg6_vertex_0600C978[] = {
#include "assets/rom/generated/661dbc0187ba2b29e8c1.inc.c"














};

// 0x0600CA68
static const Vtx dorrie_seg6_vertex_0600CA68[] = {
#include "assets/rom/generated/b97f9054017bfcf32eab.inc.c"














};

// 0x0600CB58
static const Vtx dorrie_seg6_vertex_0600CB58[] = {
#include "assets/rom/generated/989c207d8e1d103f3ffa.inc.c"















};

// 0x0600CC58
static const Vtx dorrie_seg6_vertex_0600CC58[] = {
#include "assets/rom/generated/bf4ee9937a73c5ada562.inc.c"















};

// 0x0600CD58
static const Vtx dorrie_seg6_vertex_0600CD58[] = {
#include "assets/rom/generated/b5e05fcc5aee3ca4fd3c.inc.c"








};

// 0x0600CDE8 - 0x0600CFD0
const Gfx dorrie_seg6_dl_0600CDE8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_06009DA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(dorrie_seg6_vertex_0600C978, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  2,  5,  0, 0x0),
    gsSP2Triangles( 3,  6,  2, 0x0,  6,  4,  2, 0x0),
    gsSP2Triangles( 7,  0,  5, 0x0,  5,  8,  7, 0x0),
    gsSP2Triangles( 5,  4,  8, 0x0,  0,  9,  1, 0x0),
    gsSP2Triangles( 7, 10,  0, 0x0, 11,  9,  0, 0x0),
    gsSP2Triangles(11,  0, 10, 0x0, 12,  3, 13, 0x0),
    gsSP2Triangles( 1, 14,  3, 0x0,  3, 14, 13, 0x0),
    gsSPVertex(dorrie_seg6_vertex_0600CA68, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  5,  4, 0x0),
    gsSP2Triangles( 8,  2,  6, 0x0,  6,  5,  9, 0x0),
    gsSP2Triangles( 8,  6,  9, 0x0,  3,  1, 10, 0x0),
    gsSP2Triangles( 8, 11, 12, 0x0,  8, 12, 13, 0x0),
    gsSP2Triangles( 8, 13,  0, 0x0,  8,  0,  2, 0x0),
    gsSP2Triangles( 9, 11,  8, 0x0,  9,  5, 14, 0x0),
    gsSPVertex(dorrie_seg6_vertex_0600CB58, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 4,  3,  0, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 8,  1,  3, 0x0,  8,  9,  1, 0x0),
    gsSP2Triangles(10,  8,  3, 0x0, 10, 11,  8, 0x0),
    gsSP2Triangles( 4, 10,  3, 0x0, 10, 12, 11, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(dorrie_seg6_vertex_0600CC58, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  3, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(dorrie_seg6_vertex_0600CD58, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  7,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600CFD0 - 0x0600D078
const Gfx dorrie_seg6_dl_0600CFD0[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(960, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600CDE8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0600D078

// 0x0600D090
static const Vtx dorrie_seg6_vertex_0600D090[] = {
#include "assets/rom/generated/fde8de52cfe995b97899.inc.c"














};

// 0x0600D180
static const Vtx dorrie_seg6_vertex_0600D180[] = {
#include "assets/rom/generated/401b8286867e229f8680.inc.c"















};

// 0x0600D280
static const Vtx dorrie_seg6_vertex_0600D280[] = {
#include "assets/rom/generated/efc334bcedf94c19b221.inc.c"







};

// 0x0600D300 - 0x0600D440
const Gfx dorrie_seg6_dl_0600D300[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_06009DA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(dorrie_seg6_vertex_0600D090, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  1, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  2,  5,  0, 0x0),
    gsSP2Triangles( 3,  4,  2, 0x0,  0,  5,  6, 0x0),
    gsSP2Triangles( 0,  7,  1, 0x0,  0,  6,  8, 0x0),
    gsSP2Triangles( 0,  8,  7, 0x0,  3,  9,  4, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 3,  1, 14, 0x0),
    gsSPVertex(dorrie_seg6_vertex_0600D180, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 2,  3,  0, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 7,  6,  8, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0, 10,  8, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 10, 0x0),
    gsSP2Triangles(13, 10, 12, 0x0, 12, 11, 15, 0x0),
    gsSPVertex(dorrie_seg6_vertex_0600D280, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 5,  6,  1, 0x0,  1,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600D440 - 0x0600D4E8
const Gfx dorrie_seg6_dl_0600D440[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(960, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600D300),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0600D4E8

// 0x0600D500
static const Vtx dorrie_seg6_vertex_0600D500[] = {
#include "assets/rom/generated/5abc155a5bdb08d11755.inc.c"














};

// 0x0600D5F0
static const Vtx dorrie_seg6_vertex_0600D5F0[] = {
#include "assets/rom/generated/9fd93e4703c7829c79be.inc.c"


};

// 0x0600D620 - 0x0600D6D8
const Gfx dorrie_seg6_dl_0600D620[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_06009DA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(dorrie_seg6_vertex_0600D500, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 1,  5,  2, 0x0,  6,  4,  3, 0x0),
    gsSP2Triangles( 6,  7,  4, 0x0,  2,  5,  8, 0x0),
    gsSP2Triangles( 0,  2,  9, 0x0,  2,  8,  9, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0, 11,  7,  6, 0x0),
    gsSP2Triangles( 0,  6,  3, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(dorrie_seg6_vertex_0600D5F0, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600D6D8 - 0x0600D780
const Gfx dorrie_seg6_dl_0600D6D8[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(960, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600D620),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0600D780

// 0x0600D798
static const Vtx dorrie_seg6_vertex_0600D798[] = {
#include "assets/rom/generated/7a88f9b12f165f7e903d.inc.c"







};

// 0x0600D818 - 0x0600D8B0
const Gfx dorrie_seg6_dl_0600D818[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_06009DA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(dorrie_seg6_vertex_0600D798, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 2,  1,  5, 0x0,  2,  6,  3, 0x0),
    gsSP2Triangles( 5,  6,  2, 0x0,  2,  4,  0, 0x0),
    gsSP2Triangles( 6,  5,  0, 0x0,  0,  3,  6, 0x0),
    gsSP2Triangles( 0,  4,  3, 0x0,  1,  7,  5, 0x0),
    gsSP2Triangles( 5,  7,  0, 0x0,  0,  7,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600D8B0 - 0x0600D958
const Gfx dorrie_seg6_dl_0600D8B0[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(960, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600D818),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0600D958

// 0x0600D970
static const Vtx dorrie_seg6_vertex_0600D970[] = {
#include "assets/rom/generated/abc117711f28c80eb0d6.inc.c"







};

// 0x0600D9F0 - 0x0600DA88
const Gfx dorrie_seg6_dl_0600D9F0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_06009DA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(dorrie_seg6_vertex_0600D970, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 1,  5,  3, 0x0,  2,  1,  4, 0x0),
    gsSP2Triangles( 0,  5,  1, 0x0,  3,  5,  0, 0x0),
    gsSP2Triangles( 4,  3,  0, 0x0,  6,  7,  2, 0x0),
    gsSP2Triangles( 4,  6,  2, 0x0,  2,  7,  0, 0x0),
    gsSP2Triangles( 6,  4,  0, 0x0,  0,  7,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600DA88 - 0x0600DB30
const Gfx dorrie_seg6_dl_0600DA88[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(960, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600D9F0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0600DB30

// 0x0600DB48
static const Vtx dorrie_seg6_vertex_0600DB48[] = {
#include "assets/rom/generated/f295f4ce2d516c713f1f.inc.c"







};

// 0x0600DBC8 - 0x0600DC60
const Gfx dorrie_seg6_dl_0600DBC8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_06009DA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(dorrie_seg6_vertex_0600DB48, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  2,  1, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 4,  3,  5, 0x0,  0,  5,  3, 0x0),
    gsSP2Triangles( 5,  6,  4, 0x0,  0,  2,  4, 0x0),
    gsSP2Triangles( 4,  6,  0, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 0,  7,  5, 0x0,  6,  7,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600DC60 - 0x0600DD08
const Gfx dorrie_seg6_dl_0600DC60[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(960, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600DBC8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0600DD08

// 0x0600DD20
static const Vtx dorrie_seg6_vertex_0600DD20[] = {
#include "assets/rom/generated/001f1bcdde1bf85a826d.inc.c"







};

// 0x0600DDA0 - 0x0600DE38
const Gfx dorrie_seg6_dl_0600DDA0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_06009DA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(dorrie_seg6_vertex_0600DD20, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  0,  6,  3, 0x0),
    gsSP2Triangles( 5,  6,  0, 0x0,  2,  4,  0, 0x0),
    gsSP2Triangles( 2,  6,  5, 0x0,  2,  3,  6, 0x0),
    gsSP2Triangles( 1,  3,  2, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 5,  7,  2, 0x0,  2,  7,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600DE38 - 0x0600DEE0
const Gfx dorrie_seg6_dl_0600DE38[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(960, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600DDA0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0600DEE0
static const Vtx dorrie_seg6_vertex_0600DEE0[] = {
#include "assets/rom/generated/f857d56f490f874e114d.inc.c"







};

// 0x0600DF60 - 0x0600DFA8
const Gfx dorrie_seg6_dl_0600DF60[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dorrie_seg6_texture_06009BA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 16 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(dorrie_seg6_vertex_0600DEE0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600DFA8 - 0x0600E060
const Gfx dorrie_seg6_dl_0600DFA8[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_TEX_EDGE2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(960, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_CLAMP, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dorrie_seg6_dl_0600DF60),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_TEX_EDGE, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
