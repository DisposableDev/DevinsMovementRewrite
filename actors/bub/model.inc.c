// Bub

// 0x0600E278

// 0x0600E290

// 0x0600E2A8
ALIGNED8 static const Texture bub_seg6_texture_0600E2A8[] = {
#include "actors/bub/bub_eye_border.rgba16.inc.c"
};

// 0x0600EAA8
ALIGNED8 static const Texture bub_seg6_texture_0600EAA8[] = {
#include "actors/bub/bub_fins.rgba16.inc.c"
};

// 0x0600F2A8
ALIGNED8 static const Texture bub_seg6_texture_0600F2A8[] = {
#include "actors/bub/bub_eyes.rgba16.inc.c"
};

// 0x060102A8
ALIGNED8 static const Texture bub_seg6_texture_060102A8[] = {
#include "actors/bub/bub_scales.rgba16.inc.c"
};

// 0x060112A8
static const Vtx bub_seg6_vertex_060112A8[] = {
#include "assets/rom/generated/a8b6e7d320feed6eae56.inc.c"













};

// 0x06011388
static const Vtx bub_seg6_vertex_06011388[] = {
#include "assets/rom/generated/5dee45a900950a6fcca9.inc.c"


};

// 0x060113B8
static const Vtx bub_seg6_vertex_060113B8[] = {
#include "assets/rom/generated/d507775c1b24f68b3b08.inc.c"





};

// 0x06011418
static const Vtx bub_seg6_vertex_06011418[] = {
#include "assets/rom/generated/daeab959ee99b0774d19.inc.c"






};

// 0x06011488
static const Vtx bub_seg6_vertex_06011488[] = {
#include "assets/rom/generated/2978174858e38e31bce3.inc.c"














};

// 0x06011578
static const Vtx bub_seg6_vertex_06011578[] = {
#include "assets/rom/generated/af3b4df5866d70eb8e2f.inc.c"













};

// 0x06011658
static const Vtx bub_seg6_vertex_06011658[] = {
#include "assets/rom/generated/1d28b72014c7c8c0714c.inc.c"











};

// 0x06011718
static const Vtx bub_seg6_vertex_06011718[] = {
#include "assets/rom/generated/6466e109ab692ded11cd.inc.c"











};

// 0x060117D8
static const Vtx bub_seg6_vertex_060117D8[] = {
#include "assets/rom/generated/816d57096ed6fddc093b.inc.c"






};

// 0x06011848 - 0x060118C0
const Gfx bub_seg6_dl_06011848[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bub_seg6_texture_0600E2A8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(bub_seg6_vertex_060112A8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bub_seg6_vertex_06011388, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x060118C0 - 0x06011918
const Gfx bub_seg6_dl_060118C0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bub_seg6_texture_0600EAA8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bub_seg6_vertex_060113B8, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  5,  2,  1, 0x0),
    gsSP2Triangles( 5,  3,  2, 0x0,  4,  3,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011918 - 0x06011968
const Gfx bub_seg6_dl_06011918[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bub_seg6_texture_0600F2A8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bub_seg6_vertex_06011418, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  6,  2,  4, 0x0),
    gsSP1Triangle( 6,  0,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011968 - 0x06011A50
const Gfx bub_seg6_dl_06011968[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bub_seg6_texture_060102A8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bub_seg6_vertex_06011488, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  7,  5,  4, 0x0),
    gsSP2Triangles( 7,  6,  8, 0x0,  9, 10,  0, 0x0),
    gsSP2Triangles( 9,  0, 11, 0x0, 11, 12,  9, 0x0),
    gsSP2Triangles(10, 13,  0, 0x0, 13,  1,  0, 0x0),
    gsSP2Triangles( 2, 11,  0, 0x0, 11, 14, 12, 0x0),
    gsSPVertex(bub_seg6_vertex_06011578, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  5,  4,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bub_seg6_vertex_06011658, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011A50 - 0x06011B28
const Gfx bub_seg6_dl_06011A50[] = {
    gsSPLightColor(LIGHT_1, 0xff7594ff),
    gsSPLightColor(LIGHT_2, 0x3f1d25ff),
    gsSPVertex(bub_seg6_vertex_06011718, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 3,  5,  4, 0x0,  0,  5,  3, 0x0),
    gsSP2Triangles( 6,  7,  4, 0x0,  4,  5,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8,  5,  0, 0x0),
    gsSP2Triangles( 0, 11,  8, 0x0,  8, 10,  5, 0x0),
    gsSP2Triangles( 5, 10,  6, 0x0, 10,  9,  6, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(bub_seg6_vertex_060117D8, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 2,  1,  5, 0x0,  2,  5,  6, 0x0),
    gsSP2Triangles( 2,  6,  3, 0x0,  0,  2,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011B28 - 0x06011BD8
const Gfx bub_seg6_dl_06011B28[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bub_seg6_dl_06011848),
    gsSPDisplayList(bub_seg6_dl_060118C0),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bub_seg6_dl_06011918),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bub_seg6_dl_06011968),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(bub_seg6_dl_06011A50),
    gsSPEndDisplayList(),
};

// 0x06011BD8
static const Vtx bub_seg6_vertex_06011BD8[] = {
#include "assets/rom/generated/ae8b78984bb38ab2ddce.inc.c"







};

// 0x06011C58 - 0x06011CF0
const Gfx bub_seg6_dl_06011C58[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bub_seg6_texture_0600EAA8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(bub_seg6_vertex_06011BD8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  0, 0x0,  0,  5,  6, 0x0),
    gsSP2Triangles( 6,  1,  0, 0x0,  4,  0,  3, 0x0),
    gsSP2Triangles( 6,  5,  7, 0x0,  5,  4,  7, 0x0),
    gsSP2Triangles( 7,  1,  6, 0x0,  2,  7,  3, 0x0),
    gsSP2Triangles( 3,  7,  4, 0x0,  1,  7,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011CF0 - 0x06011D50
const Gfx bub_seg6_dl_06011CF0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bub_seg6_dl_06011C58),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x06011D50
static const Vtx bub_seg6_vertex_06011D50[] = {
#include "assets/rom/generated/c0c49e6d07f8ac9301f6.inc.c"






};

// 0x06011DC0 - 0x06011E48
const Gfx bub_seg6_dl_06011DC0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bub_seg6_texture_0600EAA8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(bub_seg6_vertex_06011D50, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 2,  4,  0, 0x0,  3,  2,  1, 0x0),
    gsSP2Triangles( 3,  0,  4, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 6,  5,  0, 0x0,  3,  6,  0, 0x0),
    gsSP2Triangles( 1,  5,  6, 0x0,  6,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011E48 - 0x06011EA8
const Gfx bub_seg6_dl_06011E48[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bub_seg6_dl_06011DC0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x06011EA8
static const Vtx bub_seg6_vertex_06011EA8[] = {
#include "assets/rom/generated/c1b2e3dda2230206cfa6.inc.c"






};

// 0x06011F18 - 0x06011FA0
const Gfx bub_seg6_dl_06011F18[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bub_seg6_texture_0600EAA8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(bub_seg6_vertex_06011EA8, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  3,  0, 0x0,  0,  5,  6, 0x0),
    gsSP2Triangles( 5,  0,  2, 0x0,  6,  4,  0, 0x0),
    gsSP2Triangles( 6,  5,  2, 0x0,  4,  6,  2, 0x0),
    gsSP2Triangles( 4,  2,  1, 0x0,  1,  3,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011FA0 - 0x06012000
const Gfx bub_seg6_dl_06011FA0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bub_seg6_dl_06011F18),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
