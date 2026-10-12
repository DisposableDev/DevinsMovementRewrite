// Bullet Bill

// 0x0500BA90

// 0x0500BAA8
ALIGNED8 static const Texture bullet_bill_seg5_texture_0500BAA8[] = {
#include "actors/bullet_bill/bullet_bill_eye.rgba16.inc.c"
};

// 0x0500CAA8
ALIGNED8 static const Texture bullet_bill_seg5_texture_0500CAA8[] = {
#include "actors/bullet_bill/bullet_bill_mouth.rgba16.inc.c"
};

// 0x0500DAA8
static const Vtx bullet_bill_seg5_vertex_0500DAA8[] = {
#include "assets/rom/generated/c11700d07b45e59e4974.inc.c"














};

// 0x0500DB98
static const Vtx bullet_bill_seg5_vertex_0500DB98[] = {
#include "assets/rom/generated/9db1a42f6724060c8ab8.inc.c"








};

// 0x0500DC28
static const Vtx bullet_bill_seg5_vertex_0500DC28[] = {
#include "assets/rom/generated/e2cdd63e8cef8ebec1f8.inc.c"














};

// 0x0500DD18
static const Vtx bullet_bill_seg5_vertex_0500DD18[] = {
#include "assets/rom/generated/d46573e05e112913bfdb.inc.c"














};

// 0x0500DE08
static const Vtx bullet_bill_seg5_vertex_0500DE08[] = {
#include "assets/rom/generated/303117236afc906348e0.inc.c"











};

// 0x0500DEC8
static const Vtx bullet_bill_seg5_vertex_0500DEC8[] = {
#include "assets/rom/generated/5263da268c1fb924e668.inc.c"














};

// 0x0500DFB8
static const Vtx bullet_bill_seg5_vertex_0500DFB8[] = {
#include "assets/rom/generated/9e999f947bc4c887b152.inc.c"














};

// 0x0500E0A8
static const Vtx bullet_bill_seg5_vertex_0500E0A8[] = {
#include "assets/rom/generated/e94c296d32391665ee60.inc.c"














};

// 0x0500E198
static const Vtx bullet_bill_seg5_vertex_0500E198[] = {
#include "assets/rom/generated/3ad40f669dfc5aceb77c.inc.c"














};

// 0x0500E288
static const Vtx bullet_bill_seg5_vertex_0500E288[] = {
#include "assets/rom/generated/4bb1df2368cf7f204ace.inc.c"














};

// 0x0500E378
static const Vtx bullet_bill_seg5_vertex_0500E378[] = {
#include "assets/rom/generated/5d82c0fffa38e206c95d.inc.c"














};

// 0x0500E468
static const Vtx bullet_bill_seg5_vertex_0500E468[] = {
#include "assets/rom/generated/140d04382b6a03d03c7a.inc.c"














};

// 0x0500E558
static const Vtx bullet_bill_seg5_vertex_0500E558[] = {
#include "assets/rom/generated/5015fa6f814d1bf89842.inc.c"








};

// 0x0500E5E8 - 0x0500E678
const Gfx bullet_bill_seg5_dl_0500E5E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bullet_bill_seg5_texture_0500BAA8),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x191c52ff),
    gsSPLightColor(LIGHT_2, 0x60714ff),
    gsSPVertex(bullet_bill_seg5_vertex_0500DAA8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bullet_bill_seg5_vertex_0500DB98, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  7,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500E678 - 0x0500E730
const Gfx bullet_bill_seg5_dl_0500E678[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bullet_bill_seg5_texture_0500CAA8),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bullet_bill_seg5_vertex_0500DC28, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bullet_bill_seg5_vertex_0500DD18, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bullet_bill_seg5_vertex_0500DE08, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500E730 - 0x0500E8A8
const Gfx bullet_bill_seg5_dl_0500E730[] = {
    gsSPVertex(bullet_bill_seg5_vertex_0500DEC8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bullet_bill_seg5_vertex_0500DFB8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bullet_bill_seg5_vertex_0500E0A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bullet_bill_seg5_vertex_0500E198, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bullet_bill_seg5_vertex_0500E288, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bullet_bill_seg5_vertex_0500E378, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bullet_bill_seg5_vertex_0500E468, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bullet_bill_seg5_vertex_0500E558, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  7,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500E8A8 - 0x0500E918
const Gfx bullet_bill_seg5_dl_0500E8A8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBA, G_CC_BLENDRGBA),
    gsSPNumLights(NUMLIGHTS_1),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bullet_bill_seg5_dl_0500E5E8),
    gsSPDisplayList(bullet_bill_seg5_dl_0500E678),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(bullet_bill_seg5_dl_0500E730),
    gsSPEndDisplayList(),
};
