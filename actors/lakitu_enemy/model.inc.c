// Lakitu Enemy

// Unreferenced light group

// 0x0500ECB0

// Unreferenced light group

// Unreferenced texture
// 0x0500ECE0
UNUSED ALIGNED8 static const Texture lakitu_enemy_seg5_texture_0500ECE0[] = {
#include "actors/lakitu_enemy/lakitu_enemy_cloud_face_unused.rgba16.inc.c"
};

// 0x0500F4E0
ALIGNED8 static const Texture lakitu_enemy_seg5_texture_0500F4E0[] = {
#include "actors/lakitu_enemy/lakitu_enemy_eyes_open.rgba16.inc.c"
};

// 0x050104E0
ALIGNED8 static const Texture lakitu_enemy_seg5_texture_050104E0[] = {
#include "actors/lakitu_enemy/lakitu_enemy_eyes_closed.rgba16.inc.c"
};

// 0x050114E0
ALIGNED8 static const Texture lakitu_enemy_seg5_texture_050114E0[] = {
#include "actors/lakitu_enemy/lakitu_enemy_shell.rgba16.inc.c"
};

// 0x05011CE0
ALIGNED8 static const Texture lakitu_enemy_seg5_texture_05011CE0[] = {
#include "actors/lakitu_enemy/lakitu_enemy_frown.rgba16.inc.c"
};

// 0x050124E0

// 0x050124F8

// 0x05012510
static const Vtx lakitu_enemy_seg5_vertex_05012510[] = {
#include "assets/rom/generated/ede7f68c116fd40a7c98.inc.c"















};

// 0x05012610
static const Vtx lakitu_enemy_seg5_vertex_05012610[] = {
#include "assets/rom/generated/bbfb7fcaa835778fe62b.inc.c"









};

// 0x050126B0
static const Vtx lakitu_enemy_seg5_vertex_050126B0[] = {
#include "assets/rom/generated/bc1674aacf60dce4da0d.inc.c"










};

// 0x05012760 - 0x05012890
const Gfx lakitu_enemy_seg5_dl_05012760[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lakitu_enemy_seg5_texture_050114E0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(lakitu_enemy_seg5_vertex_05012510, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  8,  7,  1, 0x0),
    gsSP2Triangles( 6,  1,  7, 0x0,  5,  8,  9, 0x0),
    gsSP2Triangles( 5,  9, 10, 0x0,  3,  5, 10, 0x0),
    gsSP2Triangles( 6,  2,  1, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles( 8, 12, 11, 0x0,  8,  1, 12, 0x0),
    gsSP2Triangles(12,  1,  0, 0x0, 13, 11, 12, 0x0),
    gsSP2Triangles(13, 12,  0, 0x0, 14, 13,  0, 0x0),
    gsSP2Triangles( 0,  2, 14, 0x0, 14, 15, 13, 0x0),
    gsSPVertex(lakitu_enemy_seg5_vertex_05012610, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  4,  2,  5, 0x0),
    gsSP2Triangles( 1,  5,  2, 0x0,  1,  6,  5, 0x0),
    gsSP2Triangles( 4,  5,  7, 0x0,  7,  5,  8, 0x0),
    gsSP2Triangles( 8,  5,  6, 0x0,  8,  6,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x05012890 - 0x05012910
const Gfx lakitu_enemy_seg5_dl_05012890[] = {
    gsSPLightColor(LIGHT_1, 0xf2ab00ff),
    gsSPLightColor(LIGHT_2, 0x795500ff),
    gsSPVertex(lakitu_enemy_seg5_vertex_050126B0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 1,  5,  2, 0x0,  4,  0,  2, 0x0),
    gsSP2Triangles( 5,  3,  2, 0x0,  4,  6,  0, 0x0),
    gsSP2Triangles( 7,  8,  1, 0x0,  9,  7,  0, 0x0),
    gsSP2Triangles( 1,  0,  7, 0x0,  8, 10,  1, 0x0),
    gsSP2Triangles( 1, 10,  5, 0x0,  0,  6,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x05012910 - 0x05012978
const Gfx lakitu_enemy_seg5_dl_05012910[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(lakitu_enemy_seg5_dl_05012760),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(lakitu_enemy_seg5_dl_05012890),
    gsSPEndDisplayList(),
};

// 0x05012978

// 0x05012990
static const Vtx lakitu_enemy_seg5_vertex_05012990[] = {
#include "assets/rom/generated/a13fb96ac0b145c5d55d.inc.c"















};

// 0x05012A90
static const Vtx lakitu_enemy_seg5_vertex_05012A90[] = {
#include "assets/rom/generated/adc88ce0896cca669b10.inc.c"














};

// 0x05012B80
static const Vtx lakitu_enemy_seg5_vertex_05012B80[] = {
#include "assets/rom/generated/fd1ba0595cd76b7b6066.inc.c"















};

// 0x05012C80
static const Vtx lakitu_enemy_seg5_vertex_05012C80[] = {
#include "assets/rom/generated/89fcd629615304e8c990.inc.c"














};

// 0x05012D70
static const Vtx lakitu_enemy_seg5_vertex_05012D70[] = {
#include "assets/rom/generated/425fc915c4eb49745565.inc.c"















};

// 0x05012E70
static const Vtx lakitu_enemy_seg5_vertex_05012E70[] = {
#include "assets/rom/generated/a68287bb8ce34a6d6bad.inc.c"







};

// 0x05012EF0 - 0x05013160
const Gfx lakitu_enemy_seg5_dl_05012EF0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lakitu_enemy_seg5_texture_05011CE0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(lakitu_enemy_seg5_vertex_05012990, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  4, 0x0),
    gsSP2Triangles( 3,  5,  1, 0x0,  1,  6,  7, 0x0),
    gsSP2Triangles( 1,  5,  6, 0x0,  1,  0,  4, 0x0),
    gsSP2Triangles( 1,  7,  2, 0x0,  8,  7,  6, 0x0),
    gsSP2Triangles( 9,  7,  8, 0x0,  2,  7,  9, 0x0),
    gsSP2Triangles(10,  0,  2, 0x0,  2, 11, 10, 0x0),
    gsSP2Triangles( 2, 12, 11, 0x0,  2,  9, 12, 0x0),
    gsSP2Triangles( 9, 13, 12, 0x0,  9,  8, 13, 0x0),
    gsSP2Triangles(14,  5,  3, 0x0,  5, 15,  6, 0x0),
    gsSP1Triangle( 5, 14, 15, 0x0),
    gsSPVertex(lakitu_enemy_seg5_vertex_05012A90, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  6,  0,  3, 0x0),
    gsSP2Triangles( 0,  5,  1, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  6, 11, 0x0, 11,  6,  3, 0x0),
    gsSP2Triangles( 2, 11,  3, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(lakitu_enemy_seg5_vertex_05012B80, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 2,  1,  5, 0x0,  4,  6,  2, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10,  7,  9, 0x0,  7, 12,  8, 0x0),
    gsSP2Triangles(13,  8, 12, 0x0,  9,  8, 14, 0x0),
    gsSP1Triangle(15,  8, 13, 0x0),
    gsSPVertex(lakitu_enemy_seg5_vertex_05012C80, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 5,  6,  2, 0x0,  2,  6,  4, 0x0),
    gsSP2Triangles( 5,  2,  1, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle(14, 10, 13, 0x0),
    gsSPVertex(lakitu_enemy_seg5_vertex_05012D70, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 1,  0,  5, 0x0,  0,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10,  7, 0x0),
    gsSP2Triangles( 3,  7, 10, 0x0,  9,  7,  6, 0x0),
    gsSP2Triangles(11, 12, 10, 0x0, 10, 13, 11, 0x0),
    gsSP2Triangles( 3, 10, 12, 0x0,  1, 14,  2, 0x0),
    gsSP1Triangle(13,  9, 15, 0x0),
    gsSPVertex(lakitu_enemy_seg5_vertex_05012E70, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x05013160 - 0x050131C0
const Gfx lakitu_enemy_seg5_dl_05013160[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(lakitu_enemy_seg5_dl_05012EF0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x050131C0

// 0x050131D8
static const Vtx lakitu_enemy_seg5_vertex_050131D8[] = {
#include "assets/rom/generated/47a306ccbe0d1809e12c.inc.c"











};

// 0x05013298 - 0x050132D8
const Gfx lakitu_enemy_seg5_dl_05013298[] = {
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(lakitu_enemy_seg5_vertex_050131D8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x050132D8 - 0x05013320
const Gfx lakitu_enemy_seg5_dl_050132D8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPEndDisplayList(),
};

// 0x05013320 - 0x05013350
const Gfx lakitu_enemy_seg5_dl_05013320[] = {
    gsSPDisplayList(lakitu_enemy_seg5_dl_05013298),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x05013350 - 0x05013378
const Gfx lakitu_enemy_seg5_dl_05013350[] = {
    gsSPDisplayList(lakitu_enemy_seg5_dl_050132D8),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lakitu_enemy_seg5_texture_0500F4E0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(lakitu_enemy_seg5_dl_05013320),
};

// 0x05013378 - 0x050133A0
const Gfx lakitu_enemy_seg5_dl_05013378[] = {
    gsSPDisplayList(lakitu_enemy_seg5_dl_050132D8),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lakitu_enemy_seg5_texture_050104E0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(lakitu_enemy_seg5_dl_05013320),
};

// 0x050133A0

// 0x050133B8
static const Vtx lakitu_enemy_seg5_vertex_050133B8[] = {
#include "assets/rom/generated/ae0edce221ac3d8585db.inc.c"














};

// 0x050134A8 - 0x05013598
const Gfx lakitu_enemy_seg5_dl_050134A8[] = {
    gsSPLightColor(LIGHT_1, 0xf2ab00ff),
    gsSPLightColor(LIGHT_2, 0x795500ff),
    gsSPVertex(lakitu_enemy_seg5_vertex_050133B8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  3,  7,  4, 0x0),
    gsSP2Triangles( 6,  1,  3, 0x0,  1,  7,  3, 0x0),
    gsSP2Triangles( 8,  7,  0, 0x0,  7,  8,  4, 0x0),
    gsSP2Triangles( 0,  7,  1, 0x0,  9,  4,  8, 0x0),
    gsSP2Triangles( 4,  9,  5, 0x0, 10,  8, 11, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0, 11,  8,  0, 0x0),
    gsSP2Triangles( 0,  2, 11, 0x0,  2, 12, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 12,  5, 0x0),
    gsSP2Triangles( 9, 10, 13, 0x0,  9, 13,  5, 0x0),
    gsSP2Triangles(10, 12, 13, 0x0,  1, 14,  2, 0x0),
    gsSP2Triangles( 2, 14, 12, 0x0,  6, 14,  1, 0x0),
    gsSP2Triangles( 5, 14,  6, 0x0,  5, 12, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x05013598

// 0x050135B0
static const Vtx lakitu_enemy_seg5_vertex_050135B0[] = {
#include "assets/rom/generated/96b861e8f8107b12130b.inc.c"














};

// 0x050136A0 - 0x05013790
const Gfx lakitu_enemy_seg5_dl_050136A0[] = {
    gsSPLightColor(LIGHT_1, 0xf2ab00ff),
    gsSPLightColor(LIGHT_2, 0x795500ff),
    gsSPVertex(lakitu_enemy_seg5_vertex_050135B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 6,  4,  8, 0x0,  3,  8,  4, 0x0),
    gsSP2Triangles( 9,  7,  6, 0x0, 10,  7,  9, 0x0),
    gsSP2Triangles( 5,  7, 10, 0x0,  3,  5, 11, 0x0),
    gsSP2Triangles( 5, 10, 11, 0x0,  1,  6,  2, 0x0),
    gsSP2Triangles( 2,  6,  8, 0x0,  9,  6,  1, 0x0),
    gsSP2Triangles( 1, 12,  9, 0x0, 12, 10,  9, 0x0),
    gsSP2Triangles( 1,  0, 12, 0x0, 11, 13,  3, 0x0),
    gsSP2Triangles(14,  0,  2, 0x0, 14,  2,  8, 0x0),
    gsSP2Triangles( 3, 14,  8, 0x0, 13,  0,  3, 0x0),
    gsSP2Triangles( 3,  0, 14, 0x0,  0, 13, 12, 0x0),
    gsSP2Triangles(10, 13, 11, 0x0, 12, 13, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x05013790
static const Vtx lakitu_enemy_seg5_vertex_05013790[] = {
#include "assets/rom/generated/e5eb24f3b7b071f7cf22.inc.c"












};

// 0x05013860 - 0x050138B0
const Gfx lakitu_enemy_seg5_dl_05013860[] = {
    gsSPLightColor(LIGHT_1, 0xa54f1bff),
    gsSPLightColor(LIGHT_2, 0x291306ff),
    gsSPVertex(lakitu_enemy_seg5_vertex_05013790, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  0,  2, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 1,  6,  2, 0x0, 10, 11, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x050138B0 - 0x050138C0
const Gfx lakitu_enemy_seg5_dl_050138B0[] = {
    gsSPNumLights(NUMLIGHTS_1),
    gsSPEndDisplayList(),
};
