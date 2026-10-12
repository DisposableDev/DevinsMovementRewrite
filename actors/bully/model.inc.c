// Bully

// 0x05000000
static const Vtx bully_seg5_vertex_05000000[] = {
#include "assets/rom/generated/a2b26c09ec9d43b4b613.inc.c"













};

// 0x050000E0
ALIGNED8 static const Texture bully_seg5_texture_050000E0[] = {
#include "actors/bully/bully_horn.rgba16.inc.c"
};

// 0x050002E0 - 0x05000398
const Gfx bully_seg5_dl_050002E0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bully_seg5_texture_050000E0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 16 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bully_seg5_vertex_05000000, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  6,  7,  5, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  5,  8,  3, 0x0),
    gsSP2Triangles( 3,  8,  9, 0x0,  3,  9,  6, 0x0),
    gsSP2Triangles( 6,  4,  3, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11,  2, 0x0,  0, 12, 10, 0x0),
    gsSP2Triangles(10, 12, 11, 0x0,  2,  1, 10, 0x0),
    gsSP2Triangles(10,  1,  0, 0x0,  2, 11, 13, 0x0),
    gsSP2Triangles( 0, 13, 12, 0x0,  2, 13,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x05000398 - 0x05000408
const Gfx bully_seg5_dl_05000398[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_DECALRGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 4, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bully_seg5_dl_050002E0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x05000408

// 0x05000420

// Unreferenced light group

// 0x05000450

// 0x05000468
ALIGNED8 static const Texture bully_seg5_texture_05000468[] = {
#include "actors/bully/bully_left_side.rgba16.inc.c"
};

// 0x05001468
ALIGNED8 static const Texture bully_seg5_texture_05001468[] = {
#include "actors/bully/bully_right_side.rgba16.inc.c"
};

// 0x05002468
ALIGNED8 static const Texture bully_seg5_texture_05002468[] = {
#include "actors/bully/bully_eye.rgba16.inc.c"
};

// 0x05002C68
static const Vtx bully_seg5_vertex_05002C68[] = {
#include "assets/rom/generated/8b61028e944705a7c368.inc.c"













};

// 0x05002D48
static const Vtx bully_seg5_vertex_05002D48[] = {
#include "assets/rom/generated/7696f8bf836fa15aaa89.inc.c"



};

// 0x05002D88
static const Vtx bully_seg5_vertex_05002D88[] = {
#include "assets/rom/generated/463db328b048b4dcd9c0.inc.c"













};

// 0x05002E68
static const Vtx bully_seg5_vertex_05002E68[] = {
#include "assets/rom/generated/576950c763943d1aa0a4.inc.c"



};

// 0x05002F08
static const Vtx bully_seg5_vertex_05002F08[] = {
#include "assets/rom/generated/58306d66f8772f9f5325.inc.c"















};

// 0x05003008
static const Vtx bully_seg5_vertex_05003008[] = {
#include "assets/rom/generated/1219e32523c093405ca5.inc.c"















};

// 0x05003108
static const Vtx bully_seg5_vertex_05003108[] = {
#include "assets/rom/generated/e618d0918185d2096576.inc.c"















};

// 0x05003208
static const Vtx bully_seg5_vertex_05003208[] = {
#include "assets/rom/generated/08d4f32e7ff906143005.inc.c"















};

// 0x05003308
static const Vtx bully_seg5_vertex_05003308[] = {
#include "assets/rom/generated/ba7baf1a9db60c2ea095.inc.c"















};

// 0x05003408
static const Vtx bully_seg5_vertex_05003408[] = {
#include "assets/rom/generated/f797780845dfe5790241.inc.c"















};

// 0x05003508
static const Vtx bully_seg5_vertex_05003508[] = {
#include "assets/rom/generated/4634d7941c1991d6ce2b.inc.c"















};

// 0x05003608
static const Vtx bully_seg5_vertex_05003608[] = {
#include "assets/rom/generated/d367f4465f96d0a4591c.inc.c"















};

// 0x05003708 - 0x050037A0
const Gfx bully_seg5_dl_05003708[] = {
    gsSPLightColor(LIGHT_1, 0xe300ff),
    gsSPLightColor(LIGHT_2, 0x3800ff),
    gsSPVertex(bully_seg5_vertex_05002C68, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  1,  5,  6, 0x0),
    gsSP2Triangles( 1,  6,  7, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 0,  2,  8, 0x0,  8,  2,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11,  8, 12, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0,  0,  8, 11, 0x0),
    gsSPVertex(bully_seg5_vertex_05002D48, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x050037A0 - 0x05003838
const Gfx bully_seg5_dl_050037A0[] = {
    gsSPLightColor(LIGHT_1, 0xff00ff),
    gsSPLightColor(LIGHT_2, 0x3f00ff),
    gsSPVertex(bully_seg5_vertex_05002D88, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 3,  1,  0, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 5,  7,  1, 0x0,  1,  7,  2, 0x0),
    gsSP2Triangles( 8,  0,  2, 0x0,  9, 10,  0, 0x0),
    gsSP2Triangles( 9,  0,  8, 0x0, 11, 12,  8, 0x0),
    gsSP2Triangles(11,  8, 13, 0x0, 13,  8,  2, 0x0),
    gsSPVertex(bully_seg5_vertex_05002E68, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x05003878 - 0x05003C50
const Gfx bully_seg5_dl_05003878[] = {
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(bully_seg5_vertex_05002F08, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(bully_seg5_vertex_05003008, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(bully_seg5_vertex_05003108, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(bully_seg5_vertex_05003208, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(bully_seg5_vertex_05003308, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(bully_seg5_vertex_05003408, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(bully_seg5_vertex_05003508, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(bully_seg5_vertex_05003608, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x05003C50
static const Vtx bully_seg5_vertex_05003C50[] = {
#include "assets/rom/generated/9f0ec5fe1ac2d37f37d4.inc.c"



};

// 0x05003C90
static const Vtx bully_seg5_vertex_05003C90[] = {
#include "assets/rom/generated/129fd4e82c58bc28fe34.inc.c"



};

// 0x05003CD0 - 0x05003D08
const Gfx bully_seg5_dl_05003CD0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bully_seg5_texture_05000468),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bully_seg5_vertex_05003C50, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x05003D08 - 0x05003D40
const Gfx bully_seg5_dl_05003D08[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bully_seg5_texture_05001468),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bully_seg5_vertex_05003C90, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x05003D40 - 0x05003DB8
const Gfx bully_seg5_dl_05003D40[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bully_seg5_dl_05003CD0),
    gsSPDisplayList(bully_seg5_dl_05003D08),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x05003DB8
static const Vtx bully_seg5_vertex_05003DB8[] = {
#include "assets/rom/generated/9065651eb4283f6e939a.inc.c"



};

// 0x05003DF8
static const Vtx bully_seg5_vertex_05003DF8[] = {
#include "assets/rom/generated/bb8d2c5a68ab36a48682.inc.c"



};

// 0x05003E38 - 0x05003E70
const Gfx bully_seg5_dl_05003E38[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bully_seg5_texture_05000468),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bully_seg5_vertex_05003DB8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x05003E70 - 0x05003EA8
const Gfx bully_seg5_dl_05003E70[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bully_seg5_texture_05001468),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bully_seg5_vertex_05003DF8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x05003EA8 - 0x05003F20
const Gfx bully_seg5_dl_05003EA8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bully_seg5_dl_05003E38),
    gsSPDisplayList(bully_seg5_dl_05003E70),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x05003F20
static const Vtx bully_seg5_vertex_05003F20[] = {
#include "assets/rom/generated/d460faac6c2f81ee0a2d.inc.c"





};

// 0x05003F80 - 0x05003FC8
const Gfx bully_seg5_dl_05003F80[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bully_seg5_texture_05002468),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bully_seg5_vertex_05003F20, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  3,  5,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x05003FC8 - 0x05004038
const Gfx bully_seg5_dl_05003FC8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bully_seg5_dl_05003F80),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
