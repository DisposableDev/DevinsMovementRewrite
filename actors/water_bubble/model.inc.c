// Water Bubble

// 0x0500FE68

// 0x0500FE80
ALIGNED8 static const Texture water_bubble_seg5_texture_0500FE80[] = {
#include "actors/water_bubble/water_bubble.rgba16.inc.c"
};

// 0x05010680
static const Vtx water_bubble_seg5_vertex_05010680[] = {
#include "assets/rom/generated/56853d44168b4795c7e2.inc.c"















};

// 0x05010780
static const Vtx water_bubble_seg5_vertex_05010780[] = {
#include "assets/rom/generated/0d448dbe704a5fb3665e.inc.c"














};

// 0x05010870
static const Vtx water_bubble_seg5_vertex_05010870[] = {
#include "assets/rom/generated/2069129a67d2b3402122.inc.c"













};

// 0x05010950
static const Vtx water_bubble_seg5_vertex_05010950[] = {
#include "assets/rom/generated/4b2a803ef4a05c1a694a.inc.c"















};

// 0x05010A50
static const Vtx water_bubble_seg5_vertex_05010A50[] = {
#include "assets/rom/generated/524c10c8b4d372734122.inc.c"















};

// 0x05010B50
static const Vtx water_bubble_seg5_vertex_05010B50[] = {
#include "assets/rom/generated/a2e010313afdbecc06e2.inc.c"















};

// 0x05010C50
static const Vtx water_bubble_seg5_vertex_05010C50[] = {
#include "assets/rom/generated/a1fef7d253b17a4e3234.inc.c"













};

// 0x05010D30 - 0x05011000
const Gfx water_bubble_seg5_dl_05010D30[] = {
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0xbfbfbfff),
    gsSPVertex(water_bubble_seg5_vertex_05010680, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  1,  5,  3, 0x0),
    gsSP2Triangles( 6,  0,  7, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles(10,  6,  8, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 12, 14, 13, 0x0),
    gsSP2Triangles(12, 15, 14, 0x0, 13, 14,  5, 0x0),
    gsSPVertex(water_bubble_seg5_vertex_05010780, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  5,  0,  3, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  6, 13,  7, 0x0),
    gsSP1Triangle(14,  6,  8, 0x0),
    gsSPVertex(water_bubble_seg5_vertex_05010870, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  4,  2,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10,  6, 0x0),
    gsSP2Triangles(10, 11,  7, 0x0, 12,  9, 13, 0x0),
    gsSP2Triangles( 3, 12,  0, 0x0, 12, 13,  0, 0x0),
    gsSP1Triangle( 0, 13,  1, 0x0),
    gsSPVertex(water_bubble_seg5_vertex_05010950, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 6,  4,  7, 0x0,  8,  6,  9, 0x0),
    gsSP2Triangles( 6,  7,  9, 0x0,  9,  7, 10, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 12, 14, 13, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(water_bubble_seg5_vertex_05010A50, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  5,  7, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 8,  3,  6, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(10, 12, 11, 0x0, 11, 12,  1, 0x0),
    gsSP2Triangles(10, 13, 12, 0x0, 14,  9, 15, 0x0),
    gsSP2Triangles(14, 15,  3, 0x0,  3, 15,  4, 0x0),
    gsSP1Triangle( 8, 14,  3, 0x0),
    gsSPVertex(water_bubble_seg5_vertex_05010B50, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  1,  5,  3, 0x0),
    gsSP2Triangles( 6,  0,  7, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles(10,  6,  8, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles(11, 12,  5, 0x0, 13, 12, 11, 0x0),
    gsSP2Triangles(14, 13, 11, 0x0, 13, 15, 12, 0x0),
    gsSPVertex(water_bubble_seg5_vertex_05010C50, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  4,  2,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles(10,  6,  8, 0x0,  0, 11,  1, 0x0),
    gsSP2Triangles( 6, 12,  7, 0x0, 13, 10, 11, 0x0),
    gsSP2Triangles( 3, 13,  0, 0x0, 13, 11,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x05011000 - 0x05011098
const Gfx water_bubble_seg5_dl_05011000[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetEnvColor(255, 255, 255, 205),
    gsDPLoadTextureBlock(water_bubble_seg5_texture_0500FE80, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0x07C0, 0x07C0, 0, G_TX_RENDERTILE, G_ON),
    gsSPDisplayList(water_bubble_seg5_dl_05010D30),
    gsSPTexture(0x07C0, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};
