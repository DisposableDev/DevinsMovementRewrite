// Bobomb

// 0x0801DA60
ALIGNED8 static const Texture bobomb_seg8_texture_0801DA60[] = {
#include "actors/bobomb/bob-omb_left_side.rgba16.inc.c"
};

// 0x0801EA60
ALIGNED8 static const Texture bobomb_seg8_texture_0801EA60[] = {
#include "actors/bobomb/bob-omb_right_side.rgba16.inc.c"
};

// 0x0801FA60
ALIGNED8 static const Texture bobomb_seg8_texture_0801FA60[] = {
#include "actors/bobomb/bob-omb_buddy_left_side.rgba16.inc.c"
};

// 0x08020A60
ALIGNED8 static const Texture bobomb_seg8_texture_08020A60[] = {
#include "actors/bobomb/bob-omb_buddy_right_side.rgba16.inc.c"
};

// 0x08021A60
ALIGNED8 static const Texture bobomb_seg8_texture_08021A60[] = {
#include "actors/bobomb/bob-omb_eyes.rgba16.inc.c"
};

// 0x08022260
ALIGNED8 static const Texture bobomb_seg8_texture_08022260[] = {
#include "actors/bobomb/bob-omb_eyes_blink.rgba16.inc.c"
};

// 0x08022A60
static const Vtx bobomb_seg8_vertex_08022A60[] = {
#include "assets/rom/generated/09647480caec8f3d9e08.inc.c"





};

// 0x08022AC0 - 0x08022B08
const Gfx bobomb_seg8_dl_08022AC0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPEndDisplayList(),
};

// 0x08022B08 - 0x08022B58
const Gfx bobomb_seg8_dl_08022B08[] = {
    gsSPVertex(bobomb_seg8_vertex_08022A60, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  3,  5,  1, 0x0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x08022B58 - 0x08022B88
const Gfx bobomb_seg8_dl_08022B58[] = {
    gsSPDisplayList(bobomb_seg8_dl_08022AC0),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bobomb_seg8_texture_08021A60),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(bobomb_seg8_dl_08022B08),
    gsSPEndDisplayList(),
};

// 0x08022B88 - 0x08022BB8
const Gfx bobomb_seg8_dl_08022B88[] = {
    gsSPDisplayList(bobomb_seg8_dl_08022AC0),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bobomb_seg8_texture_08022260),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(bobomb_seg8_dl_08022B08),
    gsSPEndDisplayList(),
};

// 0x08022BB8
static const Vtx bobomb_seg8_vertex_08022BB8[] = {
#include "assets/rom/generated/4aed14a35911b566a96e.inc.c"



};

// 0x08022BF8
static const Vtx bobomb_seg8_vertex_08022BF8[] = {
#include "assets/rom/generated/147032eddefd7ebccd95.inc.c"



};

// 0x08022C38 - 0x08022CA0
const Gfx bobomb_seg8_dl_08022C38[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bobomb_seg8_texture_0801DA60),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bobomb_seg8_vertex_08022BB8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bobomb_seg8_texture_0801EA60),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bobomb_seg8_vertex_08022BF8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x08022CA0 - 0x08022D08
const Gfx bobomb_seg8_dl_08022CA0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bobomb_seg8_texture_0801FA60),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bobomb_seg8_vertex_08022BB8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bobomb_seg8_texture_08020A60),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bobomb_seg8_vertex_08022BF8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x08022D08 - 0x08022D78
const Gfx bobomb_seg8_dl_08022D08[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bobomb_seg8_dl_08022C38),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x08022D78 - 0x08022DE8
const Gfx bobomb_seg8_dl_08022D78[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bobomb_seg8_dl_08022CA0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x08022DE8

// 0x08022E00

// Unreferenced light group

// 0x08022E30
static const Vtx bobomb_seg8_vertex_08022E30[] = {
#include "assets/rom/generated/6e3b8cde80bf6a198699.inc.c"















};

// 0x08022F30
static const Vtx bobomb_seg8_vertex_08022F30[] = {
#include "assets/rom/generated/095f1be1ab64a65b49f6.inc.c"



};

// 0x08022F70
static const Vtx bobomb_seg8_vertex_08022F70[] = {
#include "assets/rom/generated/150ac713947a493fd727.inc.c"















};

// 0x08023070
static const Vtx bobomb_seg8_vertex_08023070[] = {
#include "assets/rom/generated/75462779d274968545aa.inc.c"



};

// 0x080230B0
static const Vtx bobomb_seg8_vertex_080230B0[] = {
#include "assets/rom/generated/3c3db7d5ba5604ab1f70.inc.c"













};

// 0x08023190
static const Vtx bobomb_seg8_vertex_08023190[] = {
#include "assets/rom/generated/5234963d13cfbc7457d9.inc.c"













};

// 0x08023270 - 0x08023378
const Gfx bobomb_seg8_dl_08023270[] = {
    gsSPLightColor(LIGHT_1, 0xff9912ff),
    gsSPLightColor(LIGHT_2, 0x3f2604ff),
    gsSPVertex(bobomb_seg8_vertex_08022E30, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12,  8, 0x0),
    gsSP2Triangles(11,  8,  7, 0x0,  6, 13, 14, 0x0),
    gsSP2Triangles( 6, 14,  3, 0x0,  9,  8, 12, 0x0),
    gsSP2Triangles( 9, 12,  1, 0x0, 10,  9, 15, 0x0),
    gsSP2Triangles( 2,  1, 12, 0x0,  2, 12, 11, 0x0),
    gsSP2Triangles(10,  6,  5, 0x0, 10,  5,  7, 0x0),
    gsSP2Triangles( 0, 14, 13, 0x0,  0, 13, 15, 0x0),
    gsSP2Triangles(11,  4,  3, 0x0, 11,  3,  2, 0x0),
    gsSP2Triangles( 2,  3, 14, 0x0,  2, 14,  0, 0x0),
    gsSP2Triangles( 7,  5,  4, 0x0,  7,  4, 11, 0x0),
    gsSP2Triangles(15, 13,  6, 0x0, 15,  6, 10, 0x0),
    gsSPVertex(bobomb_seg8_vertex_08022F30, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x08023378 - 0x08023480
const Gfx bobomb_seg8_dl_08023378[] = {
    gsSPLightColor(LIGHT_1, 0xff9912ff),
    gsSPLightColor(LIGHT_2, 0x3f2604ff),
    gsSPVertex(bobomb_seg8_vertex_08022F70, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  6, 13, 14, 0x0),
    gsSP2Triangles( 6, 14,  3, 0x0,  1, 11,  9, 0x0),
    gsSP2Triangles( 1,  9,  8, 0x0, 15,  8,  7, 0x0),
    gsSP2Triangles(12, 11,  1, 0x0, 12,  1,  0, 0x0),
    gsSP2Triangles(10,  4,  3, 0x0, 10,  3,  7, 0x0),
    gsSP2Triangles(15, 14, 13, 0x0, 15, 13,  2, 0x0),
    gsSP2Triangles( 0,  6,  5, 0x0,  0,  5, 12, 0x0),
    gsSP2Triangles( 2, 13,  6, 0x0,  2,  6,  0, 0x0),
    gsSP2Triangles(12,  5,  4, 0x0, 12,  4, 10, 0x0),
    gsSP2Triangles( 7,  3, 14, 0x0,  7, 14, 15, 0x0),
    gsSPVertex(bobomb_seg8_vertex_08023070, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x08023480 - 0x08023528
const Gfx bobomb_seg8_dl_08023480[] = {
    gsSPLightColor(LIGHT_1, 0xb2b2b2ff),
    gsSPLightColor(LIGHT_2, 0x2c2c2cff),
    gsSPVertex(bobomb_seg8_vertex_080230B0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  0,  3, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(bobomb_seg8_vertex_08023190, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 8, 11, 12, 0x0,  8, 12, 13, 0x0),
    gsSPEndDisplayList(),
};
