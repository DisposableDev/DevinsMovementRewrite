// 0x070103B0 - 0x070104A0
static const Vtx bitfs_seg7_vertex_070103B0[] = {
#include "assets/rom/generated/cf4633c11ceb03915eb3.inc.c"














};

// 0x070104A0 - 0x07010590
static const Vtx bitfs_seg7_vertex_070104A0[] = {
#include "assets/rom/generated/9a3dbdf8c70cc8df0b62.inc.c"














};

// 0x07010590 - 0x07010680
static const Vtx bitfs_seg7_vertex_07010590[] = {
#include "assets/rom/generated/f758fb2f4e9971d13222.inc.c"














};

// 0x07010680 - 0x07010770
static const Vtx bitfs_seg7_vertex_07010680[] = {
#include "assets/rom/generated/8b58fa8b2895ca48af64.inc.c"














};

// 0x07010770 - 0x07010850
static const Vtx bitfs_seg7_vertex_07010770[] = {
#include "assets/rom/generated/67064814c0eb5cdd398d.inc.c"













};

// 0x07010850 - 0x07010940
static const Vtx bitfs_seg7_vertex_07010850[] = {
#include "assets/rom/generated/0d4ce4b78158b52a710f.inc.c"














};

// 0x07010940 - 0x07010A40
static const Vtx bitfs_seg7_vertex_07010940[] = {
#include "assets/rom/generated/c750193ece66b0519b44.inc.c"















};

// 0x07010A40 - 0x07010B30
static const Vtx bitfs_seg7_vertex_07010A40[] = {
#include "assets/rom/generated/ecb40af01efdb9acb383.inc.c"














};

// 0x07010B30 - 0x07010C30
static const Vtx bitfs_seg7_vertex_07010B30[] = {
#include "assets/rom/generated/9ff3404ed39abbee7764.inc.c"















};

// 0x07010C30 - 0x07010D20
static const Vtx bitfs_seg7_vertex_07010C30[] = {
#include "assets/rom/generated/551236a6a4c4f8ff3b9a.inc.c"














};

// 0x07010D20 - 0x07010E20
static const Vtx bitfs_seg7_vertex_07010D20[] = {
#include "assets/rom/generated/0d128c9742cabdf7f590.inc.c"















};

// 0x07010E20 - 0x07011138
static const Gfx bitfs_seg7_dl_07010E20[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitfs_seg7_vertex_070103B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  7,  9, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bitfs_seg7_vertex_070104A0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle(14,  0,  2, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07010590, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07010680, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07010770, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07010850, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07010940, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 14, 0x0),
    gsSP2Triangles(10, 15, 13, 0x0, 10, 12, 15, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07010A40, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07010B30, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  5,  6,  3, 0x0),
    gsSP2Triangles( 5,  3,  7, 0x0,  5,  4,  0, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0, 11, 15, 14, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07010C30, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  3,  7,  6, 0x0),
    gsSP2Triangles( 3,  8,  7, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07010D20, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(10, 12, 13, 0x0, 12, 14, 13, 0x0),
    gsSP2Triangles(12, 15, 14, 0x0, 14, 15, 11, 0x0),
    gsSP1Triangle(14, 11,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x07011138 - 0x070111A8
const Gfx bitfs_seg7_dl_07011138[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bitfs_seg7_dl_07010E20),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
