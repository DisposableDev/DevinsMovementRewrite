// 0x07005730 - 0x07005748

// 0x07005748 - 0x07005760

// 0x07005760 - 0x070057D0
static const Vtx sl_seg7_vertex_07005760[] = {
#include "assets/rom/generated/6355a0c5fc4b1982b0c5.inc.c"






};

// 0x070057D0 - 0x07005810
static const Vtx sl_seg7_vertex_070057D0[] = {
#include "assets/rom/generated/b52e14592e1c590ed10a.inc.c"



};

// 0x07005810 - 0x07005900
static const Vtx sl_seg7_vertex_07005810[] = {
#include "assets/rom/generated/f57fb8576d00d985e19f.inc.c"














};

// 0x07005900 - 0x07005A00
static const Vtx sl_seg7_vertex_07005900[] = {
#include "assets/rom/generated/8ff6cbb45e14a3ab782f.inc.c"















};

// 0x07005A00 - 0x07005A30
static const Vtx sl_seg7_vertex_07005A00[] = {
#include "assets/rom/generated/568502d6ed0c3bbb84fe.inc.c"


};

// 0x07005A30 - 0x07005B30
static const Vtx sl_seg7_vertex_07005A30[] = {
#include "assets/rom/generated/7d8bc214f4344b298fd6.inc.c"















};

// 0x07005B30 - 0x07005C30
static const Vtx sl_seg7_vertex_07005B30[] = {
#include "assets/rom/generated/9abd9f4304667d3f5992.inc.c"















};

// 0x07005C30 - 0x07005D30
static const Vtx sl_seg7_vertex_07005C30[] = {
#include "assets/rom/generated/2286ae41714ea93b2fef.inc.c"















};

// 0x07005D30 - 0x07005E10
static const Vtx sl_seg7_vertex_07005D30[] = {
#include "assets/rom/generated/eecb55169838adc12134.inc.c"













};

// 0x07005E10 - 0x07005EF0
static const Vtx sl_seg7_vertex_07005E10[] = {
#include "assets/rom/generated/1a7905b7e974b513b184.inc.c"













};

// 0x07005EF0 - 0x07005FF0
static const Vtx sl_seg7_vertex_07005EF0[] = {
#include "assets/rom/generated/7606c3fb849d9e8cf4f2.inc.c"















};

// 0x07005FF0 - 0x070060E0
static const Vtx sl_seg7_vertex_07005FF0[] = {
#include "assets/rom/generated/632809cd96de2cd5cd6d.inc.c"














};

// 0x070060E0 - 0x070061E0
static const Vtx sl_seg7_vertex_070060E0[] = {
#include "assets/rom/generated/b0d4754412d120ebc9e9.inc.c"















};

// 0x070061E0 - 0x07006250
static const Vtx sl_seg7_vertex_070061E0[] = {
#include "assets/rom/generated/8e07bb416b1cff627ce9.inc.c"






};

// 0x07006250 - 0x07006350
static const Vtx sl_seg7_vertex_07006250[] = {
#include "assets/rom/generated/90c3f44a03544b719d74.inc.c"















};

// 0x07006350 - 0x070063B0
static const Vtx sl_seg7_vertex_07006350[] = {
#include "assets/rom/generated/ba59b38985f5119ecb80.inc.c"





};

// 0x070063B0 - 0x070064B0
static const Vtx sl_seg7_vertex_070063B0[] = {
#include "assets/rom/generated/587c3ebb1fc1dd385908.inc.c"















};

// 0x070064B0 - 0x07006590
static const Vtx sl_seg7_vertex_070064B0[] = {
#include "assets/rom/generated/3207b9f20d3a526d5ede.inc.c"













};

// 0x07006590 - 0x07006690
static const Vtx sl_seg7_vertex_07006590[] = {
#include "assets/rom/generated/2cce132aeb976af66885.inc.c"















};

// 0x07006690 - 0x07006790
static const Vtx sl_seg7_vertex_07006690[] = {
#include "assets/rom/generated/75d9c8de1313c115d058.inc.c"















};

// 0x07006790 - 0x07006870
static const Vtx sl_seg7_vertex_07006790[] = {
#include "assets/rom/generated/e3227df9e16ae524734e.inc.c"













};

// 0x07006870 - 0x07006950
static const Vtx sl_seg7_vertex_07006870[] = {
#include "assets/rom/generated/4b11c9fd320ba0d15f58.inc.c"













};

// 0x07006950 - 0x07006A40
static const Vtx sl_seg7_vertex_07006950[] = {
#include "assets/rom/generated/dae902d17c276e2d4d08.inc.c"














};

// 0x07006A40 - 0x07006B30
static const Vtx sl_seg7_vertex_07006A40[] = {
#include "assets/rom/generated/bafdd47fbce7d420bf4b.inc.c"














};

// 0x07006B30 - 0x07006C30
static const Vtx sl_seg7_vertex_07006B30[] = {
#include "assets/rom/generated/9663c84868ef2a48f523.inc.c"















};

// 0x07006C30 - 0x07006C70
static const Vtx sl_seg7_vertex_07006C30[] = {
#include "assets/rom/generated/c96ae392abb4c99432f2.inc.c"



};

// 0x07006C70 - 0x07006D70
static const Vtx sl_seg7_vertex_07006C70[] = {
#include "assets/rom/generated/dcb9c88aa4a575d0bcb4.inc.c"















};

// 0x07006D70 - 0x07006DE8
static const Gfx sl_seg7_dl_07006D70[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09008800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(sl_seg7_vertex_07005760, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 3,  6,  4, 0x0),
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(sl_seg7_vertex_070057D0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07006DE8 - 0x07006EB0
static const Gfx sl_seg7_dl_07006DE8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(sl_seg7_vertex_07005810, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(sl_seg7_vertex_07005900, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  9,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 10, 12, 0x0),
    gsSP2Triangles( 0, 14,  1, 0x0,  0, 15, 14, 0x0),
    gsSPVertex(sl_seg7_vertex_07005A00, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x07006EB0 - 0x070070A8
static const Gfx sl_seg7_dl_07006EB0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09003000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(sl_seg7_vertex_07005A30, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(sl_seg7_vertex_07005B30, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(sl_seg7_vertex_07005C30, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(sl_seg7_vertex_07005D30, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(sl_seg7_vertex_07005E10, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(sl_seg7_vertex_07005EF0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(sl_seg7_vertex_07005FF0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(sl_seg7_vertex_070060E0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(sl_seg7_vertex_070061E0, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP1Triangle( 4,  5,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x070070A8 - 0x07007118
static const Gfx sl_seg7_dl_070070A8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09006000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(sl_seg7_vertex_07006250, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 12, 10, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(sl_seg7_vertex_07006350, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x07007118 - 0x07007368
static const Gfx sl_seg7_dl_07007118[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09008000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(sl_seg7_vertex_070063B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(sl_seg7_vertex_070064B0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  3,  9,  4, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(sl_seg7_vertex_07006590, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(sl_seg7_vertex_07006690, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(sl_seg7_vertex_07006790, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(sl_seg7_vertex_07006870, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(sl_seg7_vertex_07006950, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(sl_seg7_vertex_07006A40, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(sl_seg7_vertex_07006B30, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 0, 10,  1, 0x0,  7, 11,  8, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(sl_seg7_vertex_07006C30, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07007368 - 0x070073D0
static const Gfx sl_seg7_dl_07007368[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09001000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(sl_seg7_vertex_07006C70, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0,  2, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x070073D0 - 0x07007480
const Gfx sl_seg7_dl_070073D0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(sl_seg7_dl_07006D70),
    gsSPDisplayList(sl_seg7_dl_07006DE8),
    gsSPDisplayList(sl_seg7_dl_07006EB0),
    gsSPDisplayList(sl_seg7_dl_070070A8),
    gsSPDisplayList(sl_seg7_dl_07007118),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(sl_seg7_dl_07007368),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
