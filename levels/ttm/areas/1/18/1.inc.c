// 0x0700FA90 - 0x0700FAA8

// 0x0700FAA8 - 0x0700FAC0

// 0x0700FAC0 - 0x0700FAD8

// 0x0700FAD8 - 0x0700FBB8
static const Vtx ttm_seg7_vertex_0700FAD8[] = {
#include "assets/rom/generated/b5fe8d438522659bec32.inc.c"













};

// 0x0700FBB8 - 0x0700FCB8
static const Vtx ttm_seg7_vertex_0700FBB8[] = {
#include "assets/rom/generated/16f10c302bec4a81cafb.inc.c"















};

// 0x0700FCB8 - 0x0700FD48
static const Vtx ttm_seg7_vertex_0700FCB8[] = {
#include "assets/rom/generated/d21c93fea0d6018d17cc.inc.c"








};

// 0x0700FD48 - 0x0700FE38
static const Vtx ttm_seg7_vertex_0700FD48[] = {
#include "assets/rom/generated/5a4ba15e85e6977c3d24.inc.c"














};

// 0x0700FE38 - 0x0700FF38
static const Vtx ttm_seg7_vertex_0700FE38[] = {
#include "assets/rom/generated/c1488fa95cc48ef3897c.inc.c"















};

// 0x0700FF38 - 0x0700FFF8
static const Vtx ttm_seg7_vertex_0700FF38[] = {
#include "assets/rom/generated/d777528e5a528af4faef.inc.c"











};

// 0x0700FFF8 - 0x070100F8
static const Vtx ttm_seg7_vertex_0700FFF8[] = {
#include "assets/rom/generated/a03b619d24da5291cdd7.inc.c"















};

// 0x070100F8 - 0x070101E8
static const Vtx ttm_seg7_vertex_070100F8[] = {
#include "assets/rom/generated/d5e5297532011e829253.inc.c"














};

// 0x070101E8 - 0x07010278
static const Vtx ttm_seg7_vertex_070101E8[] = {
#include "assets/rom/generated/9ec789e2b37558b51c02.inc.c"








};

// 0x07010278 - 0x07010378
static const Vtx ttm_seg7_vertex_07010278[] = {
#include "assets/rom/generated/61620f79413ac79bbc90.inc.c"















};

// 0x07010378 - 0x07010468
static const Vtx ttm_seg7_vertex_07010378[] = {
#include "assets/rom/generated/737c96d9f2340162ee1c.inc.c"














};

// 0x07010468 - 0x070104A8
static const Vtx ttm_seg7_vertex_07010468[] = {
#include "assets/rom/generated/4e8e1fca564a59de5b00.inc.c"



};

// 0x070104A8 - 0x07010568
static const Vtx ttm_seg7_vertex_070104A8[] = {
#include "assets/rom/generated/7b86ca4b9922698fb2e7.inc.c"











};

// 0x07010568 - 0x07010638
static const Vtx ttm_seg7_vertex_07010568[] = {
#include "assets/rom/generated/f2e41b45a9d14e27c75d.inc.c"












};

// 0x07010638 - 0x07010830
static const Gfx ttm_seg7_dl_07010638[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ttm_seg7_vertex_0700FAD8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700FBB8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(10, 12, 11, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700FCB8, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  7,  8, 0x0),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(ttm_seg7_vertex_0700FD48, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700FE38, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700FF38, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPLightColor(LIGHT_1, 0xbbbbbbff),
    gsSPLightColor(LIGHT_2, 0x5d5d5dff),
    gsSPVertex(ttm_seg7_vertex_0700FFF8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  0, 15,  1, 0x0),
    gsSPVertex(ttm_seg7_vertex_070100F8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_070101E8, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  7,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x07010830 - 0x07010978
static const Gfx ttm_seg7_dl_07010830[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ttm_seg7_vertex_07010278, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10, 11,  8, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 15, 12, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_07010378, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle(14,  0,  2, 0x0),
    gsSPVertex(ttm_seg7_vertex_07010468, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(ttm_seg7_vertex_070104A8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPLightColor(LIGHT_1, 0xbbbbbbff),
    gsSPLightColor(LIGHT_2, 0x5d5d5dff),
    gsSPVertex(ttm_seg7_vertex_07010568, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle( 0,  2, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x07010978 - 0x070109F0
const Gfx ttm_seg7_dl_07010978[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ttm_seg7_dl_07010638),
    gsSPDisplayList(ttm_seg7_dl_07010830),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
