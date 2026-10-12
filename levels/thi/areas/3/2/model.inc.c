// 0x07007C90 - 0x07007CA8

// 0x07007CA8 - 0x07007CC0

// 0x07007CC0 - 0x07007CD8

// 0x07007CD8 - 0x07007CF0

// 0x07007CF0 - 0x07007D08

// 0x07007D08 - 0x07007D88
static const Vtx thi_seg7_vertex_07007D08[] = {
#include "assets/rom/generated/8e6634f22ce91c453ca6.inc.c"







};

// 0x07007D88 - 0x07007DC8
static const Vtx thi_seg7_vertex_07007D88[] = {
#include "assets/rom/generated/01b6e6873fcd6e9bf4c1.inc.c"



};

// 0x07007DC8 - 0x07007E08
static const Vtx thi_seg7_vertex_07007DC8[] = {
#include "assets/rom/generated/8eff14f8a8f43360c9ad.inc.c"



};

// 0x07007E08 - 0x07007EE8
static const Vtx thi_seg7_vertex_07007E08[] = {
#include "assets/rom/generated/018748035960ce5dd3b5.inc.c"













};

// 0x07007EE8 - 0x07007FD8
static const Vtx thi_seg7_vertex_07007EE8[] = {
#include "assets/rom/generated/562d84adb77a0042007d.inc.c"














};

// 0x07007FD8 - 0x070080D8
static const Vtx thi_seg7_vertex_07007FD8[] = {
#include "assets/rom/generated/8591f583f5629264ebe5.inc.c"















};

// 0x070080D8 - 0x070081C8
static const Vtx thi_seg7_vertex_070080D8[] = {
#include "assets/rom/generated/e61b843ecc40a7e4870b.inc.c"














};

// 0x070081C8 - 0x070082A8
static const Vtx thi_seg7_vertex_070081C8[] = {
#include "assets/rom/generated/bbc6303ab949b62279e9.inc.c"













};

// 0x070082A8 - 0x07008388
static const Vtx thi_seg7_vertex_070082A8[] = {
#include "assets/rom/generated/c7924dd65af2ba9f3c2e.inc.c"













};

// 0x07008388 - 0x07008478
static const Vtx thi_seg7_vertex_07008388[] = {
#include "assets/rom/generated/4f4906fb974d931514f4.inc.c"














};

// 0x07008478 - 0x07008568
static const Vtx thi_seg7_vertex_07008478[] = {
#include "assets/rom/generated/02098f5b6d083fd50ed0.inc.c"














};

// 0x07008568 - 0x07008658
static const Vtx thi_seg7_vertex_07008568[] = {
#include "assets/rom/generated/0ba4d4180e9efd5b7981.inc.c"














};

// 0x07008658 - 0x07008748
static const Vtx thi_seg7_vertex_07008658[] = {
#include "assets/rom/generated/dc70f988a6145c0ec16c.inc.c"














};

// 0x07008748 - 0x07008838
static const Vtx thi_seg7_vertex_07008748[] = {
#include "assets/rom/generated/11eca835336f57a9e7e9.inc.c"














};

// 0x07008838 - 0x07008928
static const Vtx thi_seg7_vertex_07008838[] = {
#include "assets/rom/generated/6f6ecfb607c0bfd9a9b9.inc.c"














};

// 0x07008928 - 0x07008A28
static const Vtx thi_seg7_vertex_07008928[] = {
#include "assets/rom/generated/e3f7906874ff8ea15c0c.inc.c"















};

// 0x07008A28 - 0x07008B18
static const Vtx thi_seg7_vertex_07008A28[] = {
#include "assets/rom/generated/70bb2e441070efc4a593.inc.c"














};

// 0x07008B18 - 0x07008B78
static const Vtx thi_seg7_vertex_07008B18[] = {
#include "assets/rom/generated/e663f3c4c6d0a0f81515.inc.c"





};

// 0x07008B78 - 0x07008C58
static const Vtx thi_seg7_vertex_07008B78[] = {
#include "assets/rom/generated/d506fe59a0d55adaca7f.inc.c"













};

// 0x07008C58 - 0x07008D38
static const Vtx thi_seg7_vertex_07008C58[] = {
#include "assets/rom/generated/2b1e83eb3f99a9f1c014.inc.c"













};

// 0x07008D38 - 0x07008D78
static const Vtx thi_seg7_vertex_07008D38[] = {
#include "assets/rom/generated/77240600110f5af25cf6.inc.c"



};

// 0x07008D78 - 0x07008E38
static const Vtx thi_seg7_vertex_07008D78[] = {
#include "assets/rom/generated/70675b153217419665a5.inc.c"











};

// 0x07008E38 - 0x07008F18
static const Vtx thi_seg7_vertex_07008E38[] = {
#include "assets/rom/generated/2ae0fcd0eca8319b2664.inc.c"













};

// 0x07008F18 - 0x07008F98
static const Vtx thi_seg7_vertex_07008F18[] = {
#include "assets/rom/generated/c2b8e48a3d5b53fe290f.inc.c"







};

// 0x07008F98 - 0x07009010
static const Gfx thi_seg7_dl_07008F98[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_0900B800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(thi_seg7_vertex_07007D08, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 0,  2,  5, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 6,  2,  7, 0x0,  6,  5,  2, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  4,  7,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07009010 - 0x07009080
static const Gfx thi_seg7_dl_07009010[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09005800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(thi_seg7_vertex_07007D88, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(thi_seg7_vertex_07007DC8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07009080 - 0x07009530
static const Gfx thi_seg7_dl_07009080[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(thi_seg7_vertex_07007E08, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  3,  7,  6, 0x0),
    gsSP2Triangles( 8,  6,  7, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 0, 10,  1, 0x0, 11, 12, 13, 0x0),
    gsSPLightColor(LIGHT_1, 0x666666ff),
    gsSPLightColor(LIGHT_2, 0x333333ff),
    gsSPVertex(thi_seg7_vertex_07007EE8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(thi_seg7_vertex_07007FD8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  0,  7, 0x0,  0,  2,  7, 0x0),
    gsSP2Triangles( 7,  8,  6, 0x0,  9, 10,  8, 0x0),
    gsSP2Triangles( 9,  8, 11, 0x0,  8, 10,  6, 0x0),
    gsSP2Triangles(11, 12,  9, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(thi_seg7_vertex_070080D8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(thi_seg7_vertex_070081C8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(thi_seg7_vertex_070082A8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  0, 0x0),
    gsSP2Triangles( 7,  9,  8, 0x0, 10, 11, 12, 0x0),
    gsSP1Triangle(10, 13, 11, 0x0),
    gsSPVertex(thi_seg7_vertex_07008388, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(thi_seg7_vertex_07008478, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(thi_seg7_vertex_07008568, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(thi_seg7_vertex_07008658, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(thi_seg7_vertex_07008748, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 10, 12, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(thi_seg7_vertex_07008838, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(12, 14, 13, 0x0),
    gsSPVertex(thi_seg7_vertex_07008928, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10,  1,  0, 0x0, 10, 11,  1, 0x0),
    gsSP2Triangles( 0,  2, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(thi_seg7_vertex_07008A28, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(thi_seg7_vertex_07008B18, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xccccccff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(thi_seg7_vertex_07008B78, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  0,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(thi_seg7_vertex_07008C58, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(thi_seg7_vertex_07008D38, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07009530 - 0x070095A8
static const Gfx thi_seg7_dl_07009530[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(thi_seg7_vertex_07008D78, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 8, 10, 11, 0x0,  8,  7, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x070095A8 - 0x07009610
static const Gfx thi_seg7_dl_070095A8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09004800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(thi_seg7_vertex_07008E38, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 7,  6,  5, 0x0,  0,  8,  9, 0x0),
    gsSP2Triangles( 0, 10, 11, 0x0,  0, 12, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x07009610 - 0x07009670
static const Gfx thi_seg7_dl_07009610[] = {
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(thi_seg7_vertex_07008F18, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  0,  3,  5, 0x0),
    gsSP2Triangles( 6,  5,  3, 0x0,  6,  3,  7, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  4,  7,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07009670 - 0x07009740
const Gfx thi_seg7_dl_07009670[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(thi_seg7_dl_07008F98),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(thi_seg7_dl_07009010),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(thi_seg7_dl_07009080),
    gsSPDisplayList(thi_seg7_dl_07009530),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(thi_seg7_dl_070095A8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(thi_seg7_dl_07009610),
    gsSPEndDisplayList(),
};
