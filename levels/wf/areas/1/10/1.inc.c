// 0x07007590 - 0x070075A8

// 0x070075A8 - 0x070076A8
static const Vtx wf_seg7_vertex_070075A8[] = {
#include "assets/rom/generated/c2a70724f1c8d6bfbafe.inc.c"















};

// 0x070076A8 - 0x070077A8
static const Vtx wf_seg7_vertex_070076A8[] = {
#include "assets/rom/generated/ee98b25044caf936436b.inc.c"















};

// 0x070077A8 - 0x07007898
static const Vtx wf_seg7_vertex_070077A8[] = {
#include "assets/rom/generated/83ad380afad8cd45179a.inc.c"














};

// 0x07007898 - 0x07007988
static const Vtx wf_seg7_vertex_07007898[] = {
#include "assets/rom/generated/d423c6e45e4b019de5f3.inc.c"














};

// 0x07007988 - 0x07007A78
static const Vtx wf_seg7_vertex_07007988[] = {
#include "assets/rom/generated/5cf83656d0c554e01eaa.inc.c"














};

// 0x07007A78 - 0x07007B78
static const Vtx wf_seg7_vertex_07007A78[] = {
#include "assets/rom/generated/804407a14502b1b19e97.inc.c"















};

// 0x07007B78 - 0x07007C68
static const Vtx wf_seg7_vertex_07007B78[] = {
#include "assets/rom/generated/cff82188de88e2411da0.inc.c"














};

// 0x07007C68 - 0x07007D48
static const Vtx wf_seg7_vertex_07007C68[] = {
#include "assets/rom/generated/00493a4056191b3e807a.inc.c"













};

// 0x07007D48 - 0x07007E38
static const Vtx wf_seg7_vertex_07007D48[] = {
#include "assets/rom/generated/b7e01a2ca0bad885263e.inc.c"














};

// 0x07007E38 - 0x07007F28
static const Vtx wf_seg7_vertex_07007E38[] = {
#include "assets/rom/generated/13f9fc4e2d4a4a89123c.inc.c"














};

// 0x07007F28 - 0x07007FA8
static const Vtx wf_seg7_vertex_07007F28[] = {
#include "assets/rom/generated/86d9e601afdb0484fc0b.inc.c"







};

// 0x07007FA8 - 0x070080A8
static const Vtx wf_seg7_vertex_07007FA8[] = {
#include "assets/rom/generated/dbfb7073848b070723e1.inc.c"















};

// 0x070080A8 - 0x07008198
static const Vtx wf_seg7_vertex_070080A8[] = {
#include "assets/rom/generated/c6aacbf63e4000bac9cc.inc.c"














};

// 0x07008198 - 0x07008288
static const Vtx wf_seg7_vertex_07008198[] = {
#include "assets/rom/generated/246386188c829a518012.inc.c"














};

// 0x07008288 - 0x07008368
static const Vtx wf_seg7_vertex_07008288[] = {
#include "assets/rom/generated/e3d659527a4dee98c6db.inc.c"













};

// 0x07008368 - 0x070083E0
static const Gfx wf_seg7_dl_07008368[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, wf_seg7_texture_07001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(wf_seg7_vertex_070075A8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0,  2, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x070083E0 - 0x07008468
static const Gfx wf_seg7_dl_070083E0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09007800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(wf_seg7_vertex_070076A8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  3,  6,  7, 0x0),
    gsSP2Triangles( 3,  7,  8, 0x0,  3,  8,  9, 0x0),
    gsSP2Triangles( 3,  9, 10, 0x0,  0,  2, 11, 0x0),
    gsSP2Triangles( 0, 11, 12, 0x0,  0, 12, 13, 0x0),
    gsSP2Triangles( 0, 13, 14, 0x0,  0, 15,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008468 - 0x07008690
static const Gfx wf_seg7_dl_07008468[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09009000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(wf_seg7_vertex_070077A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(wf_seg7_vertex_07007898, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(wf_seg7_vertex_07007988, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(wf_seg7_vertex_07007A78, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 0, 12,  1, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(wf_seg7_vertex_07007B78, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(wf_seg7_vertex_07007C68, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(wf_seg7_vertex_07007D48, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(wf_seg7_vertex_07007E38, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(wf_seg7_vertex_07007F28, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008690 - 0x070087B0
static const Gfx wf_seg7_dl_07008690[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(wf_seg7_vertex_07007FA8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles( 0, 12,  1, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(wf_seg7_vertex_070080A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(wf_seg7_vertex_07008198, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 9, 10,  7, 0x0,  9, 11, 10, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(wf_seg7_vertex_07008288, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x070087B0 - 0x07008838
const Gfx wf_seg7_dl_070087B0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(wf_seg7_dl_07008368),
    gsSPDisplayList(wf_seg7_dl_070083E0),
    gsSPDisplayList(wf_seg7_dl_07008468),
    gsSPDisplayList(wf_seg7_dl_07008690),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
