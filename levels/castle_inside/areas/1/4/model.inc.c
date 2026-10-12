// 0x070295E8 - 0x07029600

// 0x07029600 - 0x07029618

// 0x07029618 - 0x07029630

// 0x07029630 - 0x07029648

// 0x07029648 - 0x07029660

// 0x07029660 - 0x07029678

// 0x07029678 - 0x070296D8
static const Vtx inside_castle_seg7_vertex_07029678[] = {
#include "assets/rom/generated/8a59f4b5f0dff69c6124.inc.c"





};

// 0x070296D8 - 0x07029718
static const Vtx inside_castle_seg7_vertex_070296D8[] = {
#include "assets/rom/generated/118e6195cdf2cd98adf3.inc.c"



};

// 0x07029718 - 0x070297F8
static const Vtx inside_castle_seg7_vertex_07029718[] = {
#include "assets/rom/generated/eb2afec4789d880eee96.inc.c"













};

// 0x070297F8 - 0x070298D8
static const Vtx inside_castle_seg7_vertex_070297F8[] = {
#include "assets/rom/generated/211508edbc669a4a951b.inc.c"













};

// 0x070298D8 - 0x07029938
static const Vtx inside_castle_seg7_vertex_070298D8[] = {
#include "assets/rom/generated/787ca4f273f6cfe6df23.inc.c"





};

// 0x07029938 - 0x07029978
static const Vtx inside_castle_seg7_vertex_07029938[] = {
#include "assets/rom/generated/628556c0b87bf8877b7e.inc.c"



};

// 0x07029978 - 0x070299B8
static const Vtx inside_castle_seg7_vertex_07029978[] = {
#include "assets/rom/generated/acfd8be0bf1693fd57cd.inc.c"



};

// 0x070299B8 - 0x070299F8
static const Vtx inside_castle_seg7_vertex_070299B8[] = {
#include "assets/rom/generated/7c4984c2cb352da3c313.inc.c"



};

// 0x070299F8 - 0x07029A38
static const Vtx inside_castle_seg7_vertex_070299F8[] = {
#include "assets/rom/generated/f6e5913e4b3ab6dfc780.inc.c"



};

// 0x07029A38 - 0x07029A78
static const Vtx inside_castle_seg7_vertex_07029A38[] = {
#include "assets/rom/generated/8ddd9b526009836349eb.inc.c"



};

// 0x07029A78 - 0x07029B78
static const Vtx inside_castle_seg7_vertex_07029A78[] = {
#include "assets/rom/generated/4b56030de971e7c51570.inc.c"















};

// 0x07029B78 - 0x07029C78
static const Vtx inside_castle_seg7_vertex_07029B78[] = {
#include "assets/rom/generated/98d62c2b999e8171f29a.inc.c"















};

// 0x07029C78 - 0x07029D58
static const Vtx inside_castle_seg7_vertex_07029C78[] = {
#include "assets/rom/generated/dac86367399784573eaf.inc.c"













};

// 0x07029D58 - 0x07029E58
static const Vtx inside_castle_seg7_vertex_07029D58[] = {
#include "assets/rom/generated/79d2a00b5d631f36cefb.inc.c"















};

// 0x07029E58 - 0x07029EA8
static const Vtx inside_castle_seg7_vertex_07029E58[] = {
#include "assets/rom/generated/7a4933914407c8be0b2d.inc.c"




};

// 0x07029EA8 - 0x07029F98
static const Vtx inside_castle_seg7_vertex_07029EA8[] = {
#include "assets/rom/generated/ab471aa92e341dc3e4ad.inc.c"














};

// 0x07029F98 - 0x0702A088
static const Vtx inside_castle_seg7_vertex_07029F98[] = {
#include "assets/rom/generated/4c278ea67ca73669b389.inc.c"














};

// 0x0702A088 - 0x0702A188
static const Vtx inside_castle_seg7_vertex_0702A088[] = {
#include "assets/rom/generated/342845cc3f0a14795a40.inc.c"















};

// 0x0702A188 - 0x0702A300
static const Gfx inside_castle_seg7_dl_0702A188[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09008000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x666666ff),
    gsSPLightColor(LIGHT_2, 0x262626ff),
    gsSPVertex(inside_castle_seg7_vertex_07029678, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0x888888ff),
    gsSPLightColor(LIGHT_2, 0x333333ff),
    gsSPVertex(inside_castle_seg7_vertex_070296D8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x5f5f5fff),
    gsSPVertex(inside_castle_seg7_vertex_07029718, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  3,  7,  6, 0x0),
    gsSP2Triangles( 7,  8,  6, 0x0,  8,  2,  6, 0x0),
    gsSP2Triangles( 6,  9, 10, 0x0, 11,  4,  6, 0x0),
    gsSP2Triangles(11,  6, 10, 0x0,  6,  2,  9, 0x0),
    gsSP2Triangles( 8, 12,  2, 0x0, 12,  0,  2, 0x0),
    gsSP1Triangle(13,  2,  1, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070297F8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  5,  4,  7, 0x0),
    gsSP2Triangles( 4,  8,  7, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 2, 10,  9, 0x0,  2,  1, 10, 0x0),
    gsSP2Triangles(12, 10,  1, 0x0, 12,  1, 13, 0x0),
    gsSP1Triangle( 0, 13,  1, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070298D8, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 3,  4,  1, 0x0,  5,  4,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0702A300 - 0x0702A548
static const Gfx inside_castle_seg7_dl_0702A300[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09001000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x555555ff),
    gsSPLightColor(LIGHT_2, 0x1f1f1fff),
    gsSPVertex(inside_castle_seg7_vertex_07029938, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0x666666ff),
    gsSPLightColor(LIGHT_2, 0x262626ff),
    gsSPVertex(inside_castle_seg7_vertex_07029978, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0x888888ff),
    gsSPLightColor(LIGHT_2, 0x333333ff),
    gsSPVertex(inside_castle_seg7_vertex_070299B8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0xaaaaaaff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(inside_castle_seg7_vertex_070299F8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0xccccccff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(inside_castle_seg7_vertex_07029A38, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x5f5f5fff),
    gsSPVertex(inside_castle_seg7_vertex_07029A78, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  4,  2,  5, 0x0),
    gsSP2Triangles( 6,  7,  4, 0x0,  6,  4,  5, 0x0),
    gsSP2Triangles( 2,  1,  8, 0x0,  1,  9,  8, 0x0),
    gsSP2Triangles( 8, 10, 11, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 10, 15, 11, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07029B78, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 2,  5,  3, 0x0,  5,  6,  3, 0x0),
    gsSP2Triangles( 7,  8,  6, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07029C78, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 5,  8,  7, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 8, 12,  7, 0x0,  8, 13, 12, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07029D58, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle( 8, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07029E58, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x0702A548 - 0x0702A650
static const Gfx inside_castle_seg7_dl_0702A548[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09000000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07029EA8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0,  8, 11, 12, 0x0),
    gsSP2Triangles( 8, 12,  9, 0x0, 11, 13, 14, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07029F98, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle(12, 14, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702A088, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0,  8, 11, 12, 0x0),
    gsSP2Triangles( 8, 12,  9, 0x0, 11, 13, 12, 0x0),
    gsSP2Triangles( 0, 14,  1, 0x0, 11, 15, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x0702A650 - 0x0702A6D8
const Gfx inside_castle_seg7_dl_0702A650[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_0702A188),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_0702A300),
    gsSPDisplayList(inside_castle_seg7_dl_0702A548),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
