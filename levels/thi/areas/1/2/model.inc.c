// 0x07005330 - 0x07005348

// 0x07005348 - 0x07005360

// 0x07005360 - 0x07005378

// 0x07005378 - 0x07005390

// 0x07005390 - 0x07005470
static const Vtx thi_seg7_vertex_07005390[] = {
#include "assets/rom/generated/30625b32dc4fb20283e6.inc.c"













};

// 0x07005470 - 0x07005550
static const Vtx thi_seg7_vertex_07005470[] = {
#include "assets/rom/generated/a230220d5e90293a2a52.inc.c"













};

// 0x07005550 - 0x070055D0
static const Vtx thi_seg7_vertex_07005550[] = {
#include "assets/rom/generated/f56350c61522ca6976f4.inc.c"







};

// 0x070055D0 - 0x070056C0
static const Vtx thi_seg7_vertex_070055D0[] = {
#include "assets/rom/generated/f0d9ba6189574cca29ed.inc.c"














};

// 0x070056C0 - 0x070057B0
static const Vtx thi_seg7_vertex_070056C0[] = {
#include "assets/rom/generated/836615a51e0fe168152b.inc.c"














};

// 0x070057B0 - 0x07005890
static const Vtx thi_seg7_vertex_070057B0[] = {
#include "assets/rom/generated/ecc27f363462a73cc019.inc.c"













};

// 0x07005890 - 0x07005990
static const Vtx thi_seg7_vertex_07005890[] = {
#include "assets/rom/generated/3382600778b9c6cb7f26.inc.c"















};

// 0x07005990 - 0x07005A80
static const Vtx thi_seg7_vertex_07005990[] = {
#include "assets/rom/generated/b1dff4af08b8c951acc2.inc.c"














};

// 0x07005A80 - 0x07005B70
static const Vtx thi_seg7_vertex_07005A80[] = {
#include "assets/rom/generated/244df71f7ed36321421c.inc.c"














};

// 0x07005B70 - 0x07005C30
static const Vtx thi_seg7_vertex_07005B70[] = {
#include "assets/rom/generated/f7825d64b247fbda9813.inc.c"











};

// 0x07005C30 - 0x07005D30
static const Vtx thi_seg7_vertex_07005C30[] = {
#include "assets/rom/generated/f94c820421e959337bd3.inc.c"















};

// 0x07005D30 - 0x07005DA0
static const Vtx thi_seg7_vertex_07005D30[] = {
#include "assets/rom/generated/1cd90bb71379860df899.inc.c"






};

// 0x07005DA0 - 0x07005EA0
static const Vtx thi_seg7_vertex_07005DA0[] = {
#include "assets/rom/generated/079aa491d6fea7b7a0b4.inc.c"















};

// 0x07005EA0 - 0x07005FA0
static const Vtx thi_seg7_vertex_07005EA0[] = {
#include "assets/rom/generated/14d960d3fc0d8ae5e950.inc.c"















};

// 0x07005FA0 - 0x07005FE0
static const Vtx thi_seg7_vertex_07005FA0[] = {
#include "assets/rom/generated/c1a5b747e7cb606aad3c.inc.c"



};

// 0x07005FE0 - 0x070060E0
static const Vtx thi_seg7_vertex_07005FE0[] = {
#include "assets/rom/generated/6acf0c6a2f3cfb48c992.inc.c"















};

// 0x070060E0 - 0x070061D0
static const Vtx thi_seg7_vertex_070060E0[] = {
#include "assets/rom/generated/a89e07e4fc1cb347fdca.inc.c"














};

// 0x070061D0 - 0x070062C0
static const Vtx thi_seg7_vertex_070061D0[] = {
#include "assets/rom/generated/4d8a7019dd541e336cfe.inc.c"














};

// 0x070062C0 - 0x070063C0
static const Vtx thi_seg7_vertex_070062C0[] = {
#include "assets/rom/generated/83f9c232b369efe952c2.inc.c"















};

// 0x070063C0 - 0x07006660
static const Gfx thi_seg7_dl_070063C0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09006800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(thi_seg7_vertex_07005390, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(thi_seg7_vertex_07005470, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(thi_seg7_vertex_07005550, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(thi_seg7_vertex_070055D0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(thi_seg7_vertex_070056C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(thi_seg7_vertex_070057B0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(thi_seg7_vertex_07005890, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(thi_seg7_vertex_07005990, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 12, 10, 0x0, 13, 14,  0, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(thi_seg7_vertex_07005A80, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(thi_seg7_vertex_07005B70, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  2,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0,  3, 11,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x07006660 - 0x07006770
static const Gfx thi_seg7_dl_07006660[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09008000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(thi_seg7_vertex_07005C30, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  3, 0x0),
    gsSP2Triangles( 7,  3,  6, 0x0,  4,  9, 10, 0x0),
    gsSP2Triangles( 5,  4, 10, 0x0, 11,  8,  7, 0x0),
    gsSP2Triangles(11,  9,  8, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles( 0,  2, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(thi_seg7_vertex_07005D30, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  3,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0x666666ff),
    gsSPLightColor(LIGHT_2, 0x333333ff),
    gsSPVertex(thi_seg7_vertex_07005DA0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0, 15,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07006770 - 0x07006800
static const Gfx thi_seg7_dl_07006770[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(thi_seg7_vertex_07005EA0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  3,  5,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(thi_seg7_vertex_07005FA0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07006800 - 0x07006868
static const Gfx thi_seg7_dl_07006800[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(thi_seg7_vertex_07005FE0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0,  2, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x07006868 - 0x07006908
static const Gfx thi_seg7_dl_07006868[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09003000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(thi_seg7_vertex_070060E0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(thi_seg7_vertex_070061D0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x07006908 - 0x07006968
static const Gfx thi_seg7_dl_07006908[] = {
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(thi_seg7_vertex_070062C0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0,  2, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x07006968 - 0x07006A30
const Gfx thi_seg7_dl_07006968[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(thi_seg7_dl_070063C0),
    gsSPDisplayList(thi_seg7_dl_07006660),
    gsSPDisplayList(thi_seg7_dl_07006770),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(thi_seg7_dl_07006800),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(thi_seg7_dl_07006868),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(thi_seg7_dl_07006908),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
