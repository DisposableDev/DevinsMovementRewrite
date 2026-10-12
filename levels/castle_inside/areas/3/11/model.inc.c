// 0x07067010 - 0x07067028

// 0x07067028 - 0x07067040

// 0x07067040 - 0x07067058

// 0x07067058 - 0x07067070

// 0x07067070 - 0x070670F0
static const Vtx inside_castle_seg7_vertex_07067070[] = {
#include "assets/rom/generated/34af008cf89bcfff7dea.inc.c"







};

// 0x070670F0 - 0x07067190
static const Vtx inside_castle_seg7_vertex_070670F0[] = {
#include "assets/rom/generated/98df1bd1a8aa8d885b76.inc.c"









};

// 0x07067190 - 0x07067280
static const Vtx inside_castle_seg7_vertex_07067190[] = {
#include "assets/rom/generated/c917a81f32f993de1658.inc.c"














};

// 0x07067280 - 0x07067370
static const Vtx inside_castle_seg7_vertex_07067280[] = {
#include "assets/rom/generated/1aaf44b1048caee307cc.inc.c"














};

// 0x07067370 - 0x07067460
static const Vtx inside_castle_seg7_vertex_07067370[] = {
#include "assets/rom/generated/c315f561b0a1df2bbbca.inc.c"














};

// 0x07067460 - 0x070674D0
static const Vtx inside_castle_seg7_vertex_07067460[] = {
#include "assets/rom/generated/9ae53e87302f059c22f3.inc.c"






};

// 0x070674D0 - 0x070675D0
static const Vtx inside_castle_seg7_vertex_070674D0[] = {
#include "assets/rom/generated/20c0f67899fc55669d0b.inc.c"















};

// 0x070675D0 - 0x070676D0
static const Vtx inside_castle_seg7_vertex_070675D0[] = {
#include "assets/rom/generated/1d8d8bb0ab449d899249.inc.c"















};

// 0x070676D0 - 0x070677B0
static const Vtx inside_castle_seg7_vertex_070676D0[] = {
#include "assets/rom/generated/34dc0e4af34cffb27769.inc.c"













};

// 0x070677B0 - 0x070678B0
static const Vtx inside_castle_seg7_vertex_070677B0[] = {
#include "assets/rom/generated/2af2c471445cc6f8ab56.inc.c"















};

// 0x070678B0 - 0x070679B0
static const Vtx inside_castle_seg7_vertex_070678B0[] = {
#include "assets/rom/generated/27a60b8f230949f9c1f3.inc.c"















};

// 0x070679B0 - 0x07067AB0
static const Vtx inside_castle_seg7_vertex_070679B0[] = {
#include "assets/rom/generated/3e9bdde9abd2951a2e4e.inc.c"















};

// 0x07067AB0 - 0x07067BA0
static const Vtx inside_castle_seg7_vertex_07067AB0[] = {
#include "assets/rom/generated/e80d046701120122c4e7.inc.c"














};

// 0x07067BA0 - 0x07067C90
static const Vtx inside_castle_seg7_vertex_07067BA0[] = {
#include "assets/rom/generated/2d8324ec025d196ee09a.inc.c"














};

// 0x07067C90 - 0x07067D90
static const Vtx inside_castle_seg7_vertex_07067C90[] = {
#include "assets/rom/generated/54104d15cf988b5ae236.inc.c"















};

// 0x07067D90 - 0x07067E90
static const Vtx inside_castle_seg7_vertex_07067D90[] = {
#include "assets/rom/generated/b1da0991d6b5854a358d.inc.c"















};

// 0x07067E90 - 0x07067F90
static const Vtx inside_castle_seg7_vertex_07067E90[] = {
#include "assets/rom/generated/c2fe9eca870dd194aa74.inc.c"















};

// 0x07067F90 - 0x07068090
static const Vtx inside_castle_seg7_vertex_07067F90[] = {
#include "assets/rom/generated/faf17caab2acd3af746d.inc.c"















};

// 0x07068090 - 0x070680D0
static const Vtx inside_castle_seg7_vertex_07068090[] = {
#include "assets/rom/generated/a0c0d68c4d43b3cf8810.inc.c"



};

// 0x070680D0 - 0x070681D0
static const Vtx inside_castle_seg7_vertex_070680D0[] = {
#include "assets/rom/generated/40e9e0d42080a6e14981.inc.c"















};

// 0x070681D0 - 0x07068210
static const Vtx inside_castle_seg7_vertex_070681D0[] = {
#include "assets/rom/generated/249be2c945aa35b06021.inc.c"



};

// 0x07068210 - 0x07068288
static const Gfx inside_castle_seg7_dl_07068210[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_0900B000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x393939ff),
    gsSPVertex(inside_castle_seg7_vertex_07067070, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 2,  1,  3, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 6,  3,  7, 0x0,  6,  4,  3, 0x0),
    gsSP2Triangles( 6,  7,  5, 0x0,  6,  5,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x07068288 - 0x070682F0
static const Gfx inside_castle_seg7_dl_07068288[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x5f5f5fff),
    gsSPVertex(inside_castle_seg7_vertex_070670F0, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  8, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  4,  9,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x070682F0 - 0x070683F0
static const Gfx inside_castle_seg7_dl_070682F0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09003000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07067190, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07067280, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07067370, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07067460, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  6,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x070683F0 - 0x070684E8
static const Gfx inside_castle_seg7_dl_070683F0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_070674D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  0,  3, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 6,  4,  5, 0x0,  6,  7,  4, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070675D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11,  0,  2, 0x0),
    gsSP2Triangles( 0, 12,  1, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070676D0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x070684E8 - 0x070687A8
static const Gfx inside_castle_seg7_dl_070684E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xcdb297ff),
    gsSPLightColor(LIGHT_2, 0x4c4238ff),
    gsSPVertex(inside_castle_seg7_vertex_070677B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0,  4, 11,  5, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070678B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070679B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07067AB0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  3,  7,  4, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07067BA0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 4, 10,  5, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07067C90, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07067D90, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07067E90, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07067F90, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07068090, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x070687A8 - 0x07068820
static const Gfx inside_castle_seg7_dl_070687A8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09009000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x5f5f5fff),
    gsSPVertex(inside_castle_seg7_vertex_070680D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0,  2, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x07068820 - 0x07068850
static const Gfx inside_castle_seg7_dl_07068820[] = {
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(inside_castle_seg7_vertex_070681D0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07068850 - 0x07068908
const Gfx inside_castle_seg7_dl_07068850[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07068210),
    gsSPDisplayList(inside_castle_seg7_dl_07068288),
    gsSPDisplayList(inside_castle_seg7_dl_070682F0),
    gsSPDisplayList(inside_castle_seg7_dl_070683F0),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_070684E8),
    gsSPDisplayList(inside_castle_seg7_dl_070687A8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(inside_castle_seg7_dl_07068820),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
