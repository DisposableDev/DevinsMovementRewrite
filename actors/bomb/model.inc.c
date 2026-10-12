// Bomb

// 0x06057AA8

// 0x06057AC0
ALIGNED8 static const Texture bomb_seg6_texture_06057AC0[] = {
#include "actors/bomb/bomb_left_side.rgba16.inc.c"
};

// 0x06058AC0
ALIGNED8 static const Texture bomb_seg6_texture_06058AC0[] = {
#include "actors/bomb/bomb_right_side.rgba16.inc.c"
};

// 0x06059AC0
ALIGNED8 static const Texture bomb_seg6_texture_06059AC0[] = {
#include "actors/bomb/bomb_spike.rgba16.inc.c"
};

// 0x0605A2C0
static const Vtx bomb_seg6_vertex_0605A2C0[] = {
#include "assets/rom/generated/1bc4fc9daa4b2b872a8e.inc.c"



};

// 0x0605A300
static const Vtx bomb_seg6_vertex_0605A300[] = {
#include "assets/rom/generated/3529a2fbd2c6fa8b6f0e.inc.c"



};

// 0x0605A340
static const Vtx bomb_seg6_vertex_0605A340[] = {
#include "assets/rom/generated/c45f62f51cbe009c1149.inc.c"














};

// 0x0605A430
static const Vtx bomb_seg6_vertex_0605A430[] = {
#include "assets/rom/generated/699442007c6cf4b26f0e.inc.c"














};

// 0x0605A520
static const Vtx bomb_seg6_vertex_0605A520[] = {
#include "assets/rom/generated/7dd9059d9dc186d27d52.inc.c"














};

// 0x0605A610
static const Vtx bomb_seg6_vertex_0605A610[] = {
#include "assets/rom/generated/d74f016004f11a15bd49.inc.c"














};

// 0x0605A700
static const Vtx bomb_seg6_vertex_0605A700[] = {
#include "assets/rom/generated/46a03755c5441962b598.inc.c"











};

// 0x0605A7C0 - 0x0605A7F8
const Gfx bomb_seg6_dl_0605A7C0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bomb_seg6_texture_06057AC0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bomb_seg6_vertex_0605A2C0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0605A7F8 - 0x0605A830
const Gfx bomb_seg6_dl_0605A7F8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bomb_seg6_texture_06058AC0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bomb_seg6_vertex_0605A300, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0605A830 - 0x0605A8A8
const Gfx bomb_seg6_dl_0605A830[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bomb_seg6_dl_0605A7C0),
    gsSPDisplayList(bomb_seg6_dl_0605A7F8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0605A8A8 - 0x0605A9C0
const Gfx bomb_seg6_dl_0605A8A8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bomb_seg6_texture_06059AC0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(bomb_seg6_vertex_0605A340, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bomb_seg6_vertex_0605A430, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bomb_seg6_vertex_0605A520, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bomb_seg6_vertex_0605A610, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bomb_seg6_vertex_0605A700, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0605A9C0 - 0x0605AA20
const Gfx bomb_seg6_dl_0605A9C0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bomb_seg6_dl_0605A8A8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
