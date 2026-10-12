// 0x0700A8B8 - 0x0700A978
static const Vtx pss_seg7_vertex_0700A8B8[] = {
#include "assets/rom/generated/daf320198fe1b33631c9.inc.c"











};

// 0x0700A978 - 0x0700AA78
static const Vtx pss_seg7_vertex_0700A978[] = {
#include "assets/rom/generated/d9a0cdc9ff3bfc621600.inc.c"















};

// 0x0700AA78 - 0x0700AB68
static const Vtx pss_seg7_vertex_0700AA78[] = {
#include "assets/rom/generated/6a9be1849498bd20f1ee.inc.c"














};

// 0x0700AB68 - 0x0700AC58
static const Vtx pss_seg7_vertex_0700AB68[] = {
#include "assets/rom/generated/a13bdc983691a12c8a4b.inc.c"














};

// 0x0700AC58 - 0x0700AD48
static const Vtx pss_seg7_vertex_0700AC58[] = {
#include "assets/rom/generated/3348af611899a0f4b68d.inc.c"














};

// 0x0700AD48 - 0x0700ADA8
static const Vtx pss_seg7_vertex_0700AD48[] = {
#include "assets/rom/generated/969a16d5f1ef63d5473a.inc.c"





};

// 0x0700ADA8 - 0x0700AE00
static const Gfx pss_seg7_dl_0700ADA8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_0900C000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(pss_seg7_vertex_0700A8B8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700AE00 - 0x0700AF60
static const Gfx pss_seg7_dl_0700AE00[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09000000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(pss_seg7_vertex_0700A978, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  3,  2, 0x0,  4,  5,  3, 0x0),
    gsSP2Triangles( 6,  5,  4, 0x0,  6,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11, 12, 0x0),
    gsSP2Triangles( 8, 13,  9, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 9, 13, 14, 0x0,  9, 14, 15, 0x0),
    gsSPVertex(pss_seg7_vertex_0700AA78, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 5,  7,  6, 0x0,  3,  4,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700AB68, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(12, 14, 13, 0x0, 14,  5, 13, 0x0),
    gsSPVertex(pss_seg7_vertex_0700AC58, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700AF60 - 0x0700AFA8
static const Gfx pss_seg7_dl_0700AF60[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(pss_seg7_vertex_0700AD48, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  5,  0,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700AFA8 - 0x0700B070
const Gfx pss_seg7_dl_0700AFA8[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(980, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(pss_seg7_dl_0700ADA8),
    gsSPDisplayList(pss_seg7_dl_0700AE00),
    gsSPDisplayList(pss_seg7_dl_0700AF60),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
