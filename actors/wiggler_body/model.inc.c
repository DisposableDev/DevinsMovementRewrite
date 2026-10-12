// Wiggler Body

// 0x05005A30
ALIGNED8 static const Texture wiggler_seg5_texture_05005A30[] = {
#include "actors/wiggler/wiggler_segment_left_side.rgba16.inc.c"
};

// 0x05006A30
ALIGNED8 static const Texture wiggler_seg5_texture_05006A30[] = {
#include "actors/wiggler/wiggler_segment_right_side.rgba16.inc.c"
};

// 0x05007A30
ALIGNED8 static const Texture wiggler_seg5_texture_05007A30[] = {
#include "actors/wiggler/wiggler_eye.rgba16.inc.c"
};

// 0x05008230
ALIGNED8 static const Texture wiggler_seg5_texture_05008230[] = {
#include "actors/wiggler/wiggler_flower.rgba16.inc.c"
};

// 0x05008A30
ALIGNED8 static const Texture wiggler_seg5_texture_05008A30[] = {
#include "actors/wiggler/wiggler_frown.rgba16.inc.c"
};

// 0x05009230
ALIGNED8 static const Texture wiggler_seg5_texture_05009230[] = {
#include "actors/wiggler/wiggler_nose_left_side.rgba16.inc.c"
};

// 0x0500A230
ALIGNED8 static const Texture wiggler_seg5_texture_0500A230[] = {
#include "actors/wiggler/wiggler_nose_right_side.rgba16.inc.c"
};

// 0x0500B230

// 0x0500B248

// Unreferenced light group

// 0x0500B278
static const Vtx wiggler_seg5_vertex_0500B278[] = {
#include "assets/rom/generated/e6edbfbd43fd9a936804.inc.c"















};

// 0x0500B378
static const Vtx wiggler_seg5_vertex_0500B378[] = {
#include "assets/rom/generated/a26f98310c223221a42f.inc.c"















};

// 0x0500B478
static const Vtx wiggler_seg5_vertex_0500B478[] = {
#include "assets/rom/generated/f749bb600c2bc085934a.inc.c"















};

// 0x0500B578
static const Vtx wiggler_seg5_vertex_0500B578[] = {
#include "assets/rom/generated/d6646c053334b1ba90fa.inc.c"



};

// 0x0500B5B8
static const Vtx wiggler_seg5_vertex_0500B5B8[] = {
#include "assets/rom/generated/0d82975e6d1ccca4cf03.inc.c"














};

// 0x0500B6A8
static const Vtx wiggler_seg5_vertex_0500B6A8[] = {
#include "assets/rom/generated/54b9910d1c8487a2766c.inc.c"














};

// 0x0500B798
static const Vtx wiggler_seg5_vertex_0500B798[] = {
#include "assets/rom/generated/8a733de050f2c36c3909.inc.c"















};

// 0x0500B898
static const Vtx wiggler_seg5_vertex_0500B898[] = {
#include "assets/rom/generated/679efb7cb8d6c97ff0f4.inc.c"















};

// 0x0500B998
static const Vtx wiggler_seg5_vertex_0500B998[] = {
#include "assets/rom/generated/a65fba5dbbb5e883d9ac.inc.c"















};

// 0x0500BA98
static const Vtx wiggler_seg5_vertex_0500BA98[] = {
#include "assets/rom/generated/d9e3efb3443582e62eac.inc.c"



};

// 0x0500BAD8
static const Vtx wiggler_seg5_vertex_0500BAD8[] = {
#include "assets/rom/generated/72b65dc31df1d2259d1a.inc.c"














};

// 0x0500BBC8
static const Vtx wiggler_seg5_vertex_0500BBC8[] = {
#include "assets/rom/generated/9e308216e91e4c8dbf83.inc.c"














};

// 0x0500BCB8 - 0x0500BE10
const Gfx wiggler_seg5_dl_0500BCB8[] = {
    gsSPLightColor(LIGHT_1, 0xdf0000ff),
    gsSPLightColor(LIGHT_2, 0x370000ff),
    gsSPVertex(wiggler_seg5_vertex_0500B278, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(wiggler_seg5_vertex_0500B378, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  2, 0x0,  6,  2,  5, 0x0),
    gsSP2Triangles( 7,  6,  8, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12,  8, 0x0),
    gsSP2Triangles( 9, 13,  7, 0x0, 10,  8,  6, 0x0),
    gsSP2Triangles(14, 13,  9, 0x0, 14,  9, 15, 0x0),
    gsSP2Triangles( 2,  7, 13, 0x0,  2, 13,  0, 0x0),
    gsSP2Triangles( 6,  4, 10, 0x0,  6,  5,  4, 0x0),
    gsSPVertex(wiggler_seg5_vertex_0500B478, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  4,  1,  6, 0x0),
    gsSP2Triangles( 7,  6,  8, 0x0,  7,  9,  6, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP2Triangles(10, 13, 14, 0x0, 10, 14, 15, 0x0),
    gsSPVertex(wiggler_seg5_vertex_0500B578, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500BE10 - 0x0500BE98
const Gfx wiggler_seg5_dl_0500BE10[] = {
    gsSPLightColor(LIGHT_1, 0xe74700ff),
    gsSPLightColor(LIGHT_2, 0x391100ff),
    gsSPVertex(wiggler_seg5_vertex_0500B5B8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  0,  3, 0x0,  8,  3,  9, 0x0),
    gsSP2Triangles( 1,  4,  7, 0x0,  1,  7,  2, 0x0),
    gsSP2Triangles( 5,  8,  9, 0x0,  5,  9,  6, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle(10, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500BE98 - 0x0500BF20
const Gfx wiggler_seg5_dl_0500BE98[] = {
    gsSPLightColor(LIGHT_1, 0xe74700ff),
    gsSPLightColor(LIGHT_2, 0x391100ff),
    gsSPVertex(wiggler_seg5_vertex_0500B6A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  0,  3, 0x0,  8,  3,  9, 0x0),
    gsSP2Triangles( 1,  4,  7, 0x0,  1,  7,  2, 0x0),
    gsSP2Triangles( 5,  8,  9, 0x0,  5,  9,  6, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle(10, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500BF20 - 0x0500C078
const Gfx wiggler_seg5_dl_0500BF20[] = {
    gsSPLightColor(LIGHT_1, 0xdf0000ff),
    gsSPLightColor(LIGHT_2, 0x370000ff),
    gsSPVertex(wiggler_seg5_vertex_0500B798, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(wiggler_seg5_vertex_0500B898, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  0,  6, 0x0,  3,  6,  7, 0x0),
    gsSP2Triangles( 8,  9,  7, 0x0,  8,  7,  6, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9, 11, 12, 0x0),
    gsSP2Triangles( 6, 13,  8, 0x0,  7,  9, 12, 0x0),
    gsSP2Triangles(14,  8, 13, 0x0, 14, 13, 15, 0x0),
    gsSP2Triangles( 2, 13,  6, 0x0,  2,  6,  0, 0x0),
    gsSP2Triangles(12,  3,  7, 0x0, 12,  4,  3, 0x0),
    gsSPVertex(wiggler_seg5_vertex_0500B998, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  2,  5, 0x0,  4,  5,  6, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  7,  4,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP2Triangles(10, 13, 14, 0x0, 10, 14, 15, 0x0),
    gsSPVertex(wiggler_seg5_vertex_0500BA98, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C078 - 0x0500C100
const Gfx wiggler_seg5_dl_0500C078[] = {
    gsSPLightColor(LIGHT_1, 0xe74700ff),
    gsSPLightColor(LIGHT_2, 0x391100ff),
    gsSPVertex(wiggler_seg5_vertex_0500BAD8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  0,  3, 0x0,  8,  3,  9, 0x0),
    gsSP2Triangles( 1,  4,  7, 0x0,  1,  7,  2, 0x0),
    gsSP2Triangles( 5,  8,  9, 0x0,  5,  9,  6, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle(10, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C100 - 0x0500C188
const Gfx wiggler_seg5_dl_0500C100[] = {
    gsSPLightColor(LIGHT_1, 0xe74700ff),
    gsSPLightColor(LIGHT_2, 0x391100ff),
    gsSPVertex(wiggler_seg5_vertex_0500BBC8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  0,  3, 0x0,  8,  3,  9, 0x0),
    gsSP2Triangles( 1,  4,  7, 0x0,  1,  7,  2, 0x0),
    gsSP2Triangles( 5,  8,  9, 0x0,  5,  9,  6, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle(10, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C188
static const Vtx wiggler_seg5_vertex_0500C188[] = {
#include "assets/rom/generated/9fe36175fc5328af19c9.inc.c"



};

// 0x0500C1C8
static const Vtx wiggler_seg5_vertex_0500C1C8[] = {
#include "assets/rom/generated/9245cf005c3d8db5cebb.inc.c"



};

// 0x0500C208 - 0x0500C240
const Gfx wiggler_seg5_dl_0500C208[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, wiggler_seg5_texture_05005A30),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(wiggler_seg5_vertex_0500C188, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C240 - 0x0500C278
const Gfx wiggler_seg5_dl_0500C240[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, wiggler_seg5_texture_05006A30),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(wiggler_seg5_vertex_0500C1C8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C278 - 0x0500C2F0
const Gfx wiggler_seg5_dl_0500C278[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(wiggler_seg5_dl_0500C208),
    gsSPDisplayList(wiggler_seg5_dl_0500C240),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
