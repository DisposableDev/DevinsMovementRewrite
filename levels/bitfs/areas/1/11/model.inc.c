// 0x070079C8 - 0x07007AC8
static const Vtx bitfs_seg7_vertex_070079C8[] = {
#include "assets/rom/generated/1515496384416bf886cf.inc.c"















};

// 0x07007AC8 - 0x07007B08
static const Vtx bitfs_seg7_vertex_07007AC8[] = {
#include "assets/rom/generated/a80f3f294e6fcd5d2301.inc.c"



};

// 0x07007B08 - 0x07007BF8
static const Vtx bitfs_seg7_vertex_07007B08[] = {
#include "assets/rom/generated/ec64041fe27ea26ad7fc.inc.c"














};

// 0x07007BF8 - 0x07007CE8
static const Vtx bitfs_seg7_vertex_07007BF8[] = {
#include "assets/rom/generated/c9a644e5f24f24c6eaf7.inc.c"














};

// 0x07007CE8 - 0x07007DE8
static const Vtx bitfs_seg7_vertex_07007CE8[] = {
#include "assets/rom/generated/8235b7e57d1e96f8da26.inc.c"















};

// 0x07007DE8 - 0x07007EE8
static const Vtx bitfs_seg7_vertex_07007DE8[] = {
#include "assets/rom/generated/657c4931315a5ebc2c24.inc.c"















};

// 0x07007EE8 - 0x07007FD8
static const Vtx bitfs_seg7_vertex_07007EE8[] = {
#include "assets/rom/generated/bbad8171f91b4d7e1d33.inc.c"














};

// 0x07007FD8 - 0x070080B8
static const Vtx bitfs_seg7_vertex_07007FD8[] = {
#include "assets/rom/generated/a75084b4769d396da286.inc.c"













};

// 0x070080B8 - 0x070081B8
static const Vtx bitfs_seg7_vertex_070080B8[] = {
#include "assets/rom/generated/727e75afda4570cf04ad.inc.c"















};

// 0x070081B8 - 0x070082A8
static const Vtx bitfs_seg7_vertex_070081B8[] = {
#include "assets/rom/generated/44b19b557616ea1b329d.inc.c"














};

// 0x070082A8 - 0x07008368
static const Vtx bitfs_seg7_vertex_070082A8[] = {
#include "assets/rom/generated/d759b87f608b7bd2c980.inc.c"











};

// 0x07008368 - 0x07008468
static const Vtx bitfs_seg7_vertex_07008368[] = {
#include "assets/rom/generated/48a26b861e424b893169.inc.c"















};

// 0x07008468 - 0x07008548
static const Vtx bitfs_seg7_vertex_07008468[] = {
#include "assets/rom/generated/9a96792a37ad11718a0f.inc.c"













};

// 0x07008548 - 0x070085A8
static const Vtx bitfs_seg7_vertex_07008548[] = {
#include "assets/rom/generated/7ed27284b401ae1a65ea.inc.c"





};

// 0x070085A8 - 0x070086A8
static const Vtx bitfs_seg7_vertex_070085A8[] = {
#include "assets/rom/generated/97e226ac2a03f6e5dc9f.inc.c"















};

// 0x070086A8 - 0x07008728
static const Vtx bitfs_seg7_vertex_070086A8[] = {
#include "assets/rom/generated/fdc3bc34c99915ee5244.inc.c"







};

// 0x07008728 - 0x07008828
static const Vtx bitfs_seg7_vertex_07008728[] = {
#include "assets/rom/generated/18cc78ab5c2d2a133278.inc.c"















};

// 0x07008828 - 0x07008928
static const Vtx bitfs_seg7_vertex_07008828[] = {
#include "assets/rom/generated/19b03a405593b5eed576.inc.c"















};

// 0x07008928 - 0x070089E8
static const Vtx bitfs_seg7_vertex_07008928[] = {
#include "assets/rom/generated/560b950f3f7c874ac749.inc.c"











};

// 0x070089E8 - 0x07008A68
static const Gfx bitfs_seg7_dl_070089E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitfs_seg7_vertex_070079C8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07007AC8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008A68 - 0x07008CE0
static const Gfx bitfs_seg7_dl_07008A68[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitfs_seg7_vertex_07007B08, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07007BF8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07007CE8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  2,  3, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 0,  6,  7, 0x0,  0,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07007DE8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(12, 14, 13, 0x0,  0,  2, 15, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07007EE8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  0,  5,  6, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  0,  7,  1, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07007FD8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bitfs_seg7_vertex_070080B8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bitfs_seg7_vertex_070081B8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bitfs_seg7_vertex_070082A8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 3,  1,  0, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008CE0 - 0x07008DB8
static const Gfx bitfs_seg7_dl_07008CE0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitfs_seg7_vertex_07008368, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07008468, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10, 11,  8, 0x0),
    gsSP2Triangles(10, 12, 11, 0x0,  8, 13,  9, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07008548, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  3,  5,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008DB8 - 0x07008E20
static const Gfx bitfs_seg7_dl_07008DB8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitfs_seg7_vertex_070085A8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008E20 - 0x07008E68
static const Gfx bitfs_seg7_dl_07008E20[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitfs_seg7_vertex_070086A8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008E68 - 0x07008F18
static const Gfx bitfs_seg7_dl_07008E68[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bitfs_seg7_texture_07001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitfs_seg7_vertex_07008728, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  0,  2, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(bitfs_seg7_vertex_07008828, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 15, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008F18 - 0x07008F48
static const Gfx bitfs_seg7_dl_07008F18[] = {
    gsSPVertex(bitfs_seg7_vertex_07008928, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008F48 - 0x07009000
const Gfx bitfs_seg7_dl_07008F48[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bitfs_seg7_dl_070089E8),
    gsSPDisplayList(bitfs_seg7_dl_07008A68),
    gsSPDisplayList(bitfs_seg7_dl_07008CE0),
    gsSPDisplayList(bitfs_seg7_dl_07008DB8),
    gsSPDisplayList(bitfs_seg7_dl_07008E20),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bitfs_seg7_dl_07008E68),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(bitfs_seg7_dl_07008F18),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
