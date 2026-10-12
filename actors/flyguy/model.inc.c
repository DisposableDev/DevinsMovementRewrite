// Flyguy

// ???
UNUSED static const u64 flyguy_unused_1 = 0;

// 0x0800E088
ALIGNED8 static const Texture flyguy_seg8_texture_0800E088[] = {
#include "actors/flyguy/flyguy_cloth_wrinkle.rgba16.inc.c"
};

// 0x0800F088
ALIGNED8 static const Texture flyguy_seg8_texture_0800F088[] = {
#include "actors/flyguy/flyguy_face.custom.rgba16.inc.c"
};

// 0x0800F888
ALIGNED8 static const Texture flyguy_seg8_texture_0800F888[] = {
#include "actors/flyguy/flyguy_propeller.ia16.inc.c"
};

// Unreferenced light group

// 0x080100A0

// 0x080100B8

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// 0x08010130
static const Vtx flyguy_seg8_vertex_08010130[] = {
#include "assets/rom/generated/6a7111987f6433849981.inc.c"















};

// 0x08010230
static const Vtx flyguy_seg8_vertex_08010230[] = {
#include "assets/rom/generated/4852b9eaece607390557.inc.c"















};

// 0x08010330
static const Vtx flyguy_seg8_vertex_08010330[] = {
#include "assets/rom/generated/c864a12c32d71ac132be.inc.c"















};

// 0x08010430
static const Vtx flyguy_seg8_vertex_08010430[] = {
#include "assets/rom/generated/3df00284a935fce5f797.inc.c"


};

// 0x08010460
static const Vtx flyguy_seg8_vertex_08010460[] = {
#include "assets/rom/generated/1938dccb10bae20396f8.inc.c"















};

// 0x08010560
static const Vtx flyguy_seg8_vertex_08010560[] = {
#include "assets/rom/generated/2d336201d9fa306b185b.inc.c"













};

// 0x08010640
static const Vtx flyguy_seg8_vertex_08010640[] = {
#include "assets/rom/generated/9c84fcccd0772547f9b7.inc.c"















};

// 0x08010740
static const Vtx flyguy_seg8_vertex_08010740[] = {
#include "assets/rom/generated/80668ddacd7ec8845d7d.inc.c"


};

// 0x08010770
static const Vtx flyguy_seg8_vertex_08010770[] = {
#include "assets/rom/generated/fbd3f270f4332f349883.inc.c"












};

// 0x08010840 - 0x08010968
const Gfx flyguy_seg8_dl_08010840[] = {
    gsSPLightColor(LIGHT_1, 0x27fccff),
    gsSPLightColor(LIGHT_2, 0x1f33ff),
    gsSPVertex(flyguy_seg8_vertex_08010130, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14, 15,  1, 0x0),
    gsSPVertex(flyguy_seg8_vertex_08010230, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  5, 0x0),
    gsSP2Triangles( 7,  5,  9, 0x0, 10,  8, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(flyguy_seg8_vertex_08010330, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 2,  6,  7, 0x0,  3,  8,  4, 0x0),
    gsSP2Triangles( 1,  9,  5, 0x0,  0,  9,  1, 0x0),
    gsSP2Triangles( 0,  7, 10, 0x0,  0, 10, 11, 0x0),
    gsSP2Triangles( 3,  5, 12, 0x0,  3, 12, 13, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0, 14, 12,  5, 0x0),
    gsSP2Triangles(14,  5,  9, 0x0,  6,  8, 15, 0x0),
    gsSPVertex(flyguy_seg8_vertex_08010430, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x08010968 - 0x08010A90
const Gfx flyguy_seg8_dl_08010968[] = {
    gsSPLightColor(LIGHT_1, 0x27fccff),
    gsSPLightColor(LIGHT_2, 0x1f33ff),
    gsSPVertex(flyguy_seg8_vertex_08010460, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11,  6,  5, 0x0),
    gsSP2Triangles(11,  5, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(flyguy_seg8_vertex_08010560, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 5,  4,  3, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 1,  9, 10, 0x0,  5,  6,  8, 0x0),
    gsSP2Triangles(10,  3,  2, 0x0,  2,  4, 11, 0x0),
    gsSP2Triangles( 2, 11,  0, 0x0,  8,  7, 12, 0x0),
    gsSP2Triangles(10,  9,  7, 0x0, 10,  7,  6, 0x0),
    gsSP1Triangle(13,  9,  1, 0x0),
    gsSPVertex(flyguy_seg8_vertex_08010640, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  3, 10, 12, 0x0),
    gsSP2Triangles( 3, 12,  0, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(flyguy_seg8_vertex_08010740, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x08010A90 - 0x08010AE0
const Gfx flyguy_seg8_dl_08010A90[] = {
    gsSPLightColor(LIGHT_1, 0xffc823ff),
    gsSPLightColor(LIGHT_2, 0x3f3208ff),
    gsSPVertex(flyguy_seg8_vertex_08010770, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  0,  2, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 1,  6,  2, 0x0, 10, 11, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x08010AE0

// 0x08010AF8
static const Vtx flyguy_seg8_vertex_08010AF8[] = {
#include "assets/rom/generated/dc1dd72e9cd4606e5f39.inc.c"



};

// 0x08010B38 - 0x08010B80
const Gfx flyguy_seg8_dl_08010B38[] = {
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, flyguy_seg8_texture_0800F888),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(flyguy_seg8_vertex_08010AF8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x08010B80 - 0x08010BF0
const Gfx flyguy_seg8_dl_08010B80[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(flyguy_seg8_dl_08010B38),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x08010BF0

// 0x08010C08

// 0x08010C20

// 0x08010C38
static const Vtx flyguy_seg8_vertex_08010C38[] = {
#include "assets/rom/generated/7095406af3fa5e900ee5.inc.c"








};

// 0x08010CC8
static const Vtx flyguy_seg8_vertex_08010CC8[] = {
#include "assets/rom/generated/6c9645ef5df5f86c686c.inc.c"





};

// 0x08010D28
static const Vtx flyguy_seg8_vertex_08010D28[] = {
#include "assets/rom/generated/18b3cffcc5111c209901.inc.c"













};

// 0x08010E08
static const Vtx flyguy_seg8_vertex_08010E08[] = {
#include "assets/rom/generated/beff9b4d32c73d33f39a.inc.c"















};

// 0x08010F08
static const Vtx flyguy_seg8_vertex_08010F08[] = {
#include "assets/rom/generated/3904f1b874eff87b80c5.inc.c"















};

// 0x08011008
static const Vtx flyguy_seg8_vertex_08011008[] = {
#include "assets/rom/generated/fe3a7e2a5ef93693c4d2.inc.c"














};

// 0x080110F8
static const Vtx flyguy_seg8_vertex_080110F8[] = {
#include "assets/rom/generated/d8ec541c746efbb28389.inc.c"















};

// 0x080111F8
static const Vtx flyguy_seg8_vertex_080111F8[] = {
#include "assets/rom/generated/e5a40c7ee3a8a4187555.inc.c"













};

// 0x080112D8
static const Vtx flyguy_seg8_vertex_080112D8[] = {
#include "assets/rom/generated/f1884e08aec58e15fc25.inc.c"






};

// 0x08011348
static const Vtx flyguy_seg8_vertex_08011348[] = {
#include "assets/rom/generated/a9e7d34d0abc7fc9b05d.inc.c"





};

// 0x080113A8 - 0x08011420
const Gfx flyguy_seg8_dl_080113A8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, flyguy_seg8_texture_0800F088),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(flyguy_seg8_vertex_08010C38, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  1,  0, 0x0,  2,  5,  0, 0x0),
    gsSP2Triangles( 0,  6,  4, 0x0,  6,  0,  7, 0x0),
    gsSP2Triangles( 7,  8,  6, 0x0,  0,  5,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x08011420 - 0x080116D0
const Gfx flyguy_seg8_dl_08011420[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, flyguy_seg8_texture_0800E088),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(flyguy_seg8_vertex_08010CC8, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xc40026ff),
    gsSPLightColor(LIGHT_2, 0x620013ff),
    gsSPVertex(flyguy_seg8_vertex_08010D28, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 1,  5,  3, 0x0,  1,  4,  2, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 2,  4,  7, 0x0,  2,  8,  9, 0x0),
    gsSP2Triangles( 2,  7,  8, 0x0,  0,  2,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10,  3,  5, 0x0),
    gsSP2Triangles(10, 12,  3, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(flyguy_seg8_vertex_08010E08, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  5,  4,  7, 0x0),
    gsSP2Triangles( 7,  8,  5, 0x0,  8,  9,  5, 0x0),
    gsSP2Triangles( 1, 10, 11, 0x0, 11,  2,  1, 0x0),
    gsSP2Triangles( 0,  2, 12, 0x0, 13,  1, 14, 0x0),
    gsSP1Triangle( 1,  0, 15, 0x0),
    gsSPVertex(flyguy_seg8_vertex_08010F08, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  2,  5, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 6,  8,  9, 0x0, 10,  7,  6, 0x0),
    gsSP2Triangles(11,  6, 12, 0x0, 13, 12,  6, 0x0),
    gsSP2Triangles( 6, 11, 10, 0x0, 14,  9,  1, 0x0),
    gsSP1Triangle(10, 11, 15, 0x0),
    gsSPVertex(flyguy_seg8_vertex_08011008, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  2,  5, 0x0,  0,  6,  7, 0x0),
    gsSP2Triangles( 5,  8,  0, 0x0,  0,  7,  3, 0x0),
    gsSP2Triangles( 5,  9, 10, 0x0,  5, 10,  8, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 13, 12, 14, 0x0),
    gsSPVertex(flyguy_seg8_vertex_080110F8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(12, 11, 14, 0x0, 15, 12, 14, 0x0),
    gsSPVertex(flyguy_seg8_vertex_080111F8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 2,  5,  6, 0x0,  7,  3,  2, 0x0),
    gsSP2Triangles( 6,  7,  2, 0x0,  3,  7,  8, 0x0),
    gsSP2Triangles( 5,  9,  6, 0x0, 10, 11, 12, 0x0),
    gsSP1Triangle(11, 13, 12, 0x0),
    gsSPVertex(flyguy_seg8_vertex_080112D8, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 4,  6,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x080116D0 - 0x08011710
const Gfx flyguy_seg8_dl_080116D0[] = {
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(flyguy_seg8_vertex_08011348, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  1,  5,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x08011710 - 0x08011798
const Gfx flyguy_seg8_dl_08011710[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(flyguy_seg8_dl_080113A8),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(flyguy_seg8_dl_08011420),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(flyguy_seg8_dl_080116D0),
    gsSPEndDisplayList(),
};
