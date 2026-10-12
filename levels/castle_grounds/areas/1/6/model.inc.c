// 0x0700B240 - 0x0700B340
static const Vtx castle_grounds_seg7_vertex_0700B240[] = {
#include "assets/rom/generated/f678151c8a75a304583d.inc.c"















};

// 0x0700B340 - 0x0700B430
static const Vtx castle_grounds_seg7_vertex_0700B340[] = {
#include "assets/rom/generated/17c2a333a2d0194cbf83.inc.c"














};

// 0x0700B430 - 0x0700B520
static const Vtx castle_grounds_seg7_vertex_0700B430[] = {
#include "assets/rom/generated/6c462f5766bc74177b24.inc.c"














};

// 0x0700B520 - 0x0700B610
static const Vtx castle_grounds_seg7_vertex_0700B520[] = {
#include "assets/rom/generated/27d648656b403d6bdbe4.inc.c"














};

// 0x0700B610 - 0x0700B6F0
static const Vtx castle_grounds_seg7_vertex_0700B610[] = {
#include "assets/rom/generated/e7b314d96897bfed0433.inc.c"













};

// 0x0700B6F0 - 0x0700B7A0
static const Vtx castle_grounds_seg7_vertex_0700B6F0[] = {
#include "assets/rom/generated/98e9a150e0a24d02bc1d.inc.c"










};

// 0x0700B7A0 - 0x0700B7E0
static const Vtx castle_grounds_seg7_vertex_0700B7A0[] = {
#include "assets/rom/generated/9073f39ad4e27528e803.inc.c"



};

// 0x0700B7E0 - 0x0700B820
static const Vtx castle_grounds_seg7_vertex_0700B7E0[] = {
#include "assets/rom/generated/f492909e179fd8fdfa3d.inc.c"



};

// 0x0700B820 - 0x0700B9B0
// Preserve the original exported display-list layout.
// clang-format off
static const Gfx castle_grounds_seg7_dl_0700B820[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, outside_0900A800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(castle_grounds_seg7_vertex_0700B240, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(castle_grounds_seg7_vertex_0700B340, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(castle_grounds_seg7_vertex_0700B430, 15, 0),
    gsSP1Triangle( 3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP1Triangle(11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(castle_grounds_seg7_vertex_0700B520, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(castle_grounds_seg7_vertex_0700B610, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(castle_grounds_seg7_vertex_0700B6F0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP1Triangle( 8,  9, 10, 0x0),
    gsSPEndDisplayList(),
};
// clang-format on

// 0x0700B9B0 - 0x0700B9E8
static const Gfx castle_grounds_seg7_dl_0700B9B0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, castle_grounds_seg7_texture_07001000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(castle_grounds_seg7_vertex_0700B7A0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700B9E8 - 0x0700BA20
static const Gfx castle_grounds_seg7_dl_0700B9E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, castle_grounds_seg7_texture_07002000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(castle_grounds_seg7_vertex_0700B7E0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700BA20 - 0x0700BAB8
const Gfx castle_grounds_seg7_dl_0700BA20[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(castle_grounds_seg7_dl_0700B820),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(castle_grounds_seg7_dl_0700B9B0),
    gsSPDisplayList(castle_grounds_seg7_dl_0700B9E8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
