// Heave Ho

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// 0x0500E968

// 0x0500E980

// Unreferenced light group

// Unreferenced light group

// 0x0500E9C8
ALIGNED8 static const Texture heave_ho_seg5_texture_0500E9C8[] = {
#include "actors/heave_ho/heave-ho_face.rgba16.inc.c"
};

// 0x0500F1C8
ALIGNED8 static const Texture heave_ho_seg5_texture_0500F1C8[] = {
#include "actors/heave_ho/heave-ho_platform.rgba16.inc.c"
};

// 0x0500F9C8
ALIGNED8 static const Texture heave_ho_seg5_texture_0500F9C8[] = {
#include "actors/heave_ho/heave-ho_logo.rgba16.inc.c"
};

// 0x050109C8
ALIGNED8 static const Texture heave_ho_seg5_texture_050109C8[] = {
#include "actors/heave_ho/heave-ho_arm_ornament.rgba16.inc.c"
};

// 0x050111C8
ALIGNED8 static const Texture heave_ho_seg5_texture_050111C8[] = {
#include "actors/heave_ho/heave-ho_roller.rgba16.inc.c"
};

// 0x050113C8
ALIGNED8 static const Texture heave_ho_seg5_texture_050113C8[] = {
#include "actors/heave_ho/heave-ho_turnkey.rgba16.inc.c"
};

// 0x05011BC8

// 0x05011BE0

// 0x05011BF8
static const Vtx heave_ho_seg5_vertex_05011BF8[] = {
#include "assets/rom/generated/9d9e723481a316321d2e.inc.c"



};

// 0x05011C38
static const Vtx heave_ho_seg5_vertex_05011C38[] = {
#include "assets/rom/generated/20b2c64c950f18c72794.inc.c"





};

// 0x05011C98
static const Vtx heave_ho_seg5_vertex_05011C98[] = {
#include "assets/rom/generated/2086d0dddadd6fa24a0c.inc.c"









};

// 0x05011D38
static const Vtx heave_ho_seg5_vertex_05011D38[] = {
#include "assets/rom/generated/fed5686b27615689b5bd.inc.c"





};

// 0x05011D98 - 0x05011DE0
const Gfx heave_ho_seg5_dl_05011D98[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, heave_ho_seg5_texture_0500E9C8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(heave_ho_seg5_vertex_05011BF8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x05011DE0 - 0x05011E28
const Gfx heave_ho_seg5_dl_05011DE0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, heave_ho_seg5_texture_050109C8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(heave_ho_seg5_vertex_05011C38, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  2,  5,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x05011E28 - 0x05011E80
const Gfx heave_ho_seg5_dl_05011E28[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, heave_ho_seg5_texture_0500F9C8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(heave_ho_seg5_vertex_05011C98, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 8,  6,  5, 0x0,  6,  9,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x05011E80 - 0x05011EC0
const Gfx heave_ho_seg5_dl_05011E80[] = {
    gsSPLightColor(LIGHT_1, 0x81011aff),
    gsSPLightColor(LIGHT_2, 0x40000dff),
    gsSPVertex(heave_ho_seg5_vertex_05011D38, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  3,  5,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x05011EC0 - 0x05011F50
const Gfx heave_ho_seg5_dl_05011EC0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(heave_ho_seg5_dl_05011D98),
    gsSPDisplayList(heave_ho_seg5_dl_05011DE0),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(heave_ho_seg5_dl_05011E28),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(heave_ho_seg5_dl_05011E80),
    gsSPEndDisplayList(),
};

// 0x05011F50

// 0x05011F68

// 0x05011F80
static const Vtx heave_ho_seg5_vertex_05011F80[] = {
#include "assets/rom/generated/9450ad13b02f94ec0b58.inc.c"










};

// 0x05012030
static const Vtx heave_ho_seg5_vertex_05012030[] = {
#include "assets/rom/generated/902703af9557db32792f.inc.c"






};

// 0x050120A0 - 0x05012100
const Gfx heave_ho_seg5_dl_050120A0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, heave_ho_seg5_texture_050109C8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(heave_ho_seg5_vertex_05011F80, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  3, 0x0,  7,  8,  9, 0x0),
    gsSP1Triangle( 9, 10,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x05012100 - 0x05012148
const Gfx heave_ho_seg5_dl_05012100[] = {
    gsSPLightColor(LIGHT_1, 0x99002dff),
    gsSPLightColor(LIGHT_2, 0x4c0016ff),
    gsSPVertex(heave_ho_seg5_vertex_05012030, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 4,  0,  3, 0x0,  3,  5,  4, 0x0),
    gsSP1Triangle( 6,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x05012148 - 0x050121B0
const Gfx heave_ho_seg5_dl_05012148[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(heave_ho_seg5_dl_050120A0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(heave_ho_seg5_dl_05012100),
    gsSPEndDisplayList(),
};

// 0x050121B0

// 0x050121C8

// 0x050121E0
static const Vtx heave_ho_seg5_vertex_050121E0[] = {
#include "assets/rom/generated/c0ddb24b2450c434cf42.inc.c"










};

// 0x05012290
static const Vtx heave_ho_seg5_vertex_05012290[] = {
#include "assets/rom/generated/c84cc19f1296c12c0a0c.inc.c"






};

// 0x05012300 - 0x05012360
const Gfx heave_ho_seg5_dl_05012300[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, heave_ho_seg5_texture_050109C8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(heave_ho_seg5_vertex_050121E0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  3, 0x0,  7,  8,  9, 0x0),
    gsSP1Triangle( 9, 10,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x05012360 - 0x050123A8
const Gfx heave_ho_seg5_dl_05012360[] = {
    gsSPLightColor(LIGHT_1, 0x99002dff),
    gsSPLightColor(LIGHT_2, 0x4c0016ff),
    gsSPVertex(heave_ho_seg5_vertex_05012290, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  2,  4, 0x0,  4,  3,  6, 0x0),
    gsSP1Triangle( 2,  6,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x050123A8 - 0x05012410
const Gfx heave_ho_seg5_dl_050123A8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(heave_ho_seg5_dl_05012300),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(heave_ho_seg5_dl_05012360),
    gsSPEndDisplayList(),
};

// 0x05012410

// 0x05012428

// 0x05012440
static const Vtx heave_ho_seg5_vertex_05012440[] = {
#include "assets/rom/generated/96a3486b22bb55272931.inc.c"



};

// 0x05012480
static const Vtx heave_ho_seg5_vertex_05012480[] = {
#include "assets/rom/generated/acc5dfde25e31a237f1c.inc.c"













};

// 0x05012560 - 0x050125A8
const Gfx heave_ho_seg5_dl_05012560[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, heave_ho_seg5_texture_0500F1C8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(heave_ho_seg5_vertex_05012440, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x050125A8 - 0x050125F8
const Gfx heave_ho_seg5_dl_050125A8[] = {
    gsSPLightColor(LIGHT_1, 0xbbdacbff),
    gsSPLightColor(LIGHT_2, 0x5d6d65ff),
    gsSPVertex(heave_ho_seg5_vertex_05012480, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 0,  2, 10, 0x0, 11, 12, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x050125F8 - 0x05012670
const Gfx heave_ho_seg5_dl_050125F8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(heave_ho_seg5_dl_05012560),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(heave_ho_seg5_dl_050125A8),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x05012670
static const Vtx heave_ho_seg5_vertex_05012670[] = {
#include "assets/rom/generated/d3d371f9dd129fd46936.inc.c"



};

// 0x050126B0 - 0x050126E8
const Gfx heave_ho_seg5_dl_050126B0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, heave_ho_seg5_texture_050111C8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 16 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(heave_ho_seg5_vertex_05012670, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x050126E8 - 0x05012758
const Gfx heave_ho_seg5_dl_050126E8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_CLAMP, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(heave_ho_seg5_dl_050126B0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x05012758

// 0x05012770
static const Vtx heave_ho_seg5_vertex_05012770[] = {
#include "assets/rom/generated/ffae3e690a5f197da24b.inc.c"



};

// 0x050127B0 - 0x050127F8
const Gfx heave_ho_seg5_dl_050127B0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, heave_ho_seg5_texture_050113C8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x93737cff),
    gsSPVertex(heave_ho_seg5_vertex_05012770, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x050127F8 - 0x05012868
const Gfx heave_ho_seg5_dl_050127F8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(heave_ho_seg5_dl_050127B0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x05012868
static const Vtx heave_ho_seg5_vertex_05012868[] = {
#include "assets/rom/generated/8f6010d79055e2e518c8.inc.c"















};

// 0x05012968
static const Vtx heave_ho_seg5_vertex_05012968[] = {
#include "assets/rom/generated/08a9ba345d577c53b8ff.inc.c"















};

// 0x05012A68
static const Vtx heave_ho_seg5_vertex_05012A68[] = {
#include "assets/rom/generated/880a26c4eaf489b43a01.inc.c"















};

// 0x05012B68
static const Vtx heave_ho_seg5_vertex_05012B68[] = {
#include "assets/rom/generated/a965c2c440cf186654dd.inc.c"















};

// 0x05012C68
static const Vtx heave_ho_seg5_vertex_05012C68[] = {
#include "assets/rom/generated/0e12e3e430b00cfb46f2.inc.c"















};

// 0x05012D68
static const Vtx heave_ho_seg5_vertex_05012D68[] = {
#include "assets/rom/generated/eb4130dd98fc981d5be8.inc.c"















};

// 0x05012E68
static const Vtx heave_ho_seg5_vertex_05012E68[] = {
#include "assets/rom/generated/32c3623426a97c04f407.inc.c"















};

// 0x05012F68
static const Vtx heave_ho_seg5_vertex_05012F68[] = {
#include "assets/rom/generated/b765633e5caae66ce4d4.inc.c"















};

// 0x05013068
static const Vtx heave_ho_seg5_vertex_05013068[] = {
#include "assets/rom/generated/ee9810f9c858e4ccc13c.inc.c"











};

// 0x05013128
static const Vtx heave_ho_seg5_vertex_05013128[] = {
#include "assets/rom/generated/11673dc6c47b75361099.inc.c"















};

// 0x05013228
static const Vtx heave_ho_seg5_vertex_05013228[] = {
#include "assets/rom/generated/78640ba5dc2d64e28778.inc.c"















};

// 0x05013328
static const Vtx heave_ho_seg5_vertex_05013328[] = {
#include "assets/rom/generated/0a34d5e07a28586c682b.inc.c"















};

// 0x05013428
static const Vtx heave_ho_seg5_vertex_05013428[] = {
#include "assets/rom/generated/57659b2c7e594a52670e.inc.c"















};

// 0x05013528
static const Vtx heave_ho_seg5_vertex_05013528[] = {
#include "assets/rom/generated/9da2def84f6bdea65dcb.inc.c"















};

// 0x05013628
static const Vtx heave_ho_seg5_vertex_05013628[] = {
#include "assets/rom/generated/028ff5e759a9d5c65d11.inc.c"















};

// 0x05013728
static const Vtx heave_ho_seg5_vertex_05013728[] = {
#include "assets/rom/generated/376d49b2edfde847b9ad.inc.c"















};

// 0x05013828
static const Vtx heave_ho_seg5_vertex_05013828[] = {
#include "assets/rom/generated/0039e428bc2a4e3129e5.inc.c"















};

// 0x05013928
static const Vtx heave_ho_seg5_vertex_05013928[] = {
#include "assets/rom/generated/e6bacdb2de7c42f7ce24.inc.c"















};

// 0x05013A28
static const Vtx heave_ho_seg5_vertex_05013A28[] = {
#include "assets/rom/generated/55299344ea24a130aea4.inc.c"















};

// 0x05013B28
static const Vtx heave_ho_seg5_vertex_05013B28[] = {
#include "assets/rom/generated/8ef20dac949dd6e15c9d.inc.c"















};

// 0x05013C28
static const Vtx heave_ho_seg5_vertex_05013C28[] = {
#include "assets/rom/generated/0883e5886151063326c7.inc.c"















};

// 0x05013D28
static const Vtx heave_ho_seg5_vertex_05013D28[] = {
#include "assets/rom/generated/ed203c32105de7aade6e.inc.c"















};

// 0x05013E28
static const Vtx heave_ho_seg5_vertex_05013E28[] = {
#include "assets/rom/generated/aec909b7ad71b3012fef.inc.c"















};

// 0x05013F28
static const Vtx heave_ho_seg5_vertex_05013F28[] = {
#include "assets/rom/generated/65ad4e90dcadabbe51a2.inc.c"















};

// 0x05014028
static const Vtx heave_ho_seg5_vertex_05014028[] = {
#include "assets/rom/generated/4bc3f33efeac1f4b4d13.inc.c"















};

// 0x05014128
static const Vtx heave_ho_seg5_vertex_05014128[] = {
#include "assets/rom/generated/6aed39cebadd34436567.inc.c"





};

// 0x05014188 - 0x05014560
const Gfx heave_ho_seg5_dl_05014188[] = {
    gsSPLightColor(LIGHT_1, 0x505054ff),
    gsSPLightColor(LIGHT_2, 0x141415ff),
    gsSPVertex(heave_ho_seg5_vertex_05012868, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05012968, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05012A68, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05012B68, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05012C68, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05012D68, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05012E68, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05012F68, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x05014560 - 0x050145D0
const Gfx heave_ho_seg5_dl_05014560[] = {
    gsSPLightColor(LIGHT_1, 0xffe085ff),
    gsSPLightColor(LIGHT_2, 0x733f32ff),
    gsSPVertex(heave_ho_seg5_vertex_05013068, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  5,  7,  3, 0x0),
    gsSP2Triangles( 8,  9,  7, 0x0, 10,  6,  9, 0x0),
    gsSP2Triangles( 2, 11,  0, 0x0,  7,  5,  8, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0,  6, 10,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x050145D0 - 0x050149A8
const Gfx heave_ho_seg5_dl_050145D0[] = {
    gsSPLightColor(LIGHT_1, 0x505054ff),
    gsSPLightColor(LIGHT_2, 0x141415ff),
    gsSPVertex(heave_ho_seg5_vertex_05013128, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05013228, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05013328, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05013428, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05013528, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05013628, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05013728, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05013828, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x050149A8 - 0x05014D80
const Gfx heave_ho_seg5_dl_050149A8[] = {
    gsSPLightColor(LIGHT_1, 0x505054ff),
    gsSPLightColor(LIGHT_2, 0x141415ff),
    gsSPVertex(heave_ho_seg5_vertex_05013928, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05013A28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05013B28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05013C28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05013D28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05013E28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05013F28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(heave_ho_seg5_vertex_05014028, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x05014D80 - 0x05014DD0
const Gfx heave_ho_seg5_dl_05014D80[] = {
    gsSPLightColor(LIGHT_1, 0xffe085ff),
    gsSPLightColor(LIGHT_2, 0x733f32ff),
    gsSPVertex(heave_ho_seg5_vertex_05014128, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 4,  3,  5, 0x0,  0,  2,  5, 0x0),
    gsSP2Triangles( 0,  3,  1, 0x0,  2,  4,  5, 0x0),
    gsSPEndDisplayList(),
};
