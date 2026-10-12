// 0x0701FD60 - 0x0701FD78

// 0x0701FD78 - 0x0701FD90

// 0x0701FD90 - 0x0701FE50
static const Vtx ssl_seg7_vertex_0701FD90[] = {
#include "assets/rom/generated/020e8657e023eb8ff863.inc.c"











};

// 0x0701FE50 - 0x0701FF40
static const Vtx ssl_seg7_vertex_0701FE50[] = {
#include "assets/rom/generated/03484223af1917589c6d.inc.c"














};

// 0x0701FF40 - 0x07020030
static const Vtx ssl_seg7_vertex_0701FF40[] = {
#include "assets/rom/generated/037a5236ea749573246e.inc.c"














};

// 0x07020030 - 0x07020120
static const Vtx ssl_seg7_vertex_07020030[] = {
#include "assets/rom/generated/a1d8662f1d8b1ba83b93.inc.c"














};

// 0x07020120 - 0x07020210
static const Vtx ssl_seg7_vertex_07020120[] = {
#include "assets/rom/generated/a2ec02ebeb5ba59f6528.inc.c"














};

// 0x07020210 - 0x07020300
static const Vtx ssl_seg7_vertex_07020210[] = {
#include "assets/rom/generated/187d0bc5e12f67809f96.inc.c"














};

// 0x07020300 - 0x070203F0
static const Vtx ssl_seg7_vertex_07020300[] = {
#include "assets/rom/generated/f17c33a47a8530b1fbad.inc.c"














};

// 0x070203F0 - 0x070204E0
static const Vtx ssl_seg7_vertex_070203F0[] = {
#include "assets/rom/generated/edc521f5150a4bfddc3f.inc.c"














};

// 0x070204E0 - 0x07020550
static const Vtx ssl_seg7_vertex_070204E0[] = {
#include "assets/rom/generated/5e53b4473ce91b28d7b5.inc.c"






};

// 0x07020550 - 0x070205D0
static const Vtx ssl_seg7_vertex_07020550[] = {
#include "assets/rom/generated/e0a63b68cb6d871f5149.inc.c"







};

// 0x070205D0 - 0x070206C0
static const Vtx ssl_seg7_vertex_070205D0[] = {
#include "assets/rom/generated/4c72d9bc7d4787afc512.inc.c"














};

// 0x070206C0 - 0x070207B0
static const Vtx ssl_seg7_vertex_070206C0[] = {
#include "assets/rom/generated/8ac511934914db89dc9f.inc.c"














};

// 0x070207B0 - 0x07020870
static const Vtx ssl_seg7_vertex_070207B0[] = {
#include "assets/rom/generated/1496567c4a21f97ba22f.inc.c"











};

// 0x07020870 - 0x07020950
static const Vtx ssl_seg7_vertex_07020870[] = {
#include "assets/rom/generated/e539278629d3c1d8cce2.inc.c"













};

// 0x07020950 - 0x07020A20
static const Vtx ssl_seg7_vertex_07020950[] = {
#include "assets/rom/generated/e8702a97193ece2dfd00.inc.c"












};

// 0x07020A20 - 0x07020B20
static const Vtx ssl_seg7_vertex_07020A20[] = {
#include "assets/rom/generated/118f6dab1eaea24b6d35.inc.c"















};

// 0x07020B20 - 0x07020C20
static const Vtx ssl_seg7_vertex_07020B20[] = {
#include "assets/rom/generated/1916de32f32e6ec1ead4.inc.c"















};

// 0x07020C20 - 0x07020D10
static const Vtx ssl_seg7_vertex_07020C20[] = {
#include "assets/rom/generated/34c1a8fad681a6495037.inc.c"














};

// 0x07020D10 - 0x07020E00
static const Vtx ssl_seg7_vertex_07020D10[] = {
#include "assets/rom/generated/30d8d73ada124e59a7a3.inc.c"














};

// 0x07020E00 - 0x07020EB0
static const Vtx ssl_seg7_vertex_07020E00[] = {
#include "assets/rom/generated/064c1dd01d5f8082b568.inc.c"










};

// 0x07020EB0 - 0x07020FB0
static const Vtx ssl_seg7_vertex_07020EB0[] = {
#include "assets/rom/generated/9bdd61a3bbc826279443.inc.c"















};

// 0x07020FB0 - 0x07021090
static const Vtx ssl_seg7_vertex_07020FB0[] = {
#include "assets/rom/generated/f1f610da3fa43a547339.inc.c"













};

// 0x07021090 - 0x07021150
static const Vtx ssl_seg7_vertex_07021090[] = {
#include "assets/rom/generated/06358c279a0885a19b2e.inc.c"











};

// 0x07021150 - 0x07021250
static const Vtx ssl_seg7_vertex_07021150[] = {
#include "assets/rom/generated/33c3149abc3f8f6bbf51.inc.c"















};

// 0x07021250 - 0x07021290
static const Vtx ssl_seg7_vertex_07021250[] = {
#include "assets/rom/generated/d40f28104218ff26c1eb.inc.c"



};

// 0x07021290 - 0x070214E8
static const Gfx ssl_seg7_dl_07021290[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, generic_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x8c8c8cff),
    gsSPLightColor(LIGHT_2, 0x464646ff),
    gsSPVertex(ssl_seg7_vertex_0701FD90, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ssl_seg7_vertex_0701FE50, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(ssl_seg7_vertex_0701FF40, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(ssl_seg7_vertex_07020030, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ssl_seg7_vertex_07020120, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(ssl_seg7_vertex_07020210, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(ssl_seg7_vertex_07020300, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(ssl_seg7_vertex_070203F0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(12, 14, 13, 0x0),
    gsSPVertex(ssl_seg7_vertex_070204E0, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  2,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x070214E8 - 0x07021638
static const Gfx ssl_seg7_dl_070214E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, generic_0900A800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x8c8c8cff),
    gsSPLightColor(LIGHT_2, 0x464646ff),
    gsSPVertex(ssl_seg7_vertex_07020550, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 0,  3,  1, 0x0,  1,  5,  2, 0x0),
    gsSP2Triangles( 1,  6,  5, 0x0,  3,  4,  7, 0x0),
    gsSP2Triangles( 5,  7,  4, 0x0,  5,  6,  7, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ssl_seg7_vertex_070205D0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ssl_seg7_vertex_070206C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(ssl_seg7_vertex_070207B0, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  8, 0x0),
    gsSP2Triangles( 5,  8,  6, 0x0,  4,  6,  9, 0x0),
    gsSP2Triangles(10,  4,  9, 0x0, 10,  9, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x07021638 - 0x07021870
static const Gfx ssl_seg7_dl_07021638[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, generic_09001000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x8c8c8cff),
    gsSPLightColor(LIGHT_2, 0x464646ff),
    gsSPVertex(ssl_seg7_vertex_07020870, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10, 11, 0x0,  8,  7, 10, 0x0),
    gsSP2Triangles( 7,  9, 12, 0x0, 13,  7, 12, 0x0),
    gsSP1Triangle( 0, 13,  1, 0x0),
    gsSPVertex(ssl_seg7_vertex_07020950, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  6,  7,  4, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle( 0,  2, 12, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ssl_seg7_vertex_07020A20, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  4,  7,  8, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  9,  7, 10, 0x0),
    gsSP2Triangles( 9,  8,  7, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13,  2, 0x0, 14, 15,  0, 0x0),
    gsSPVertex(ssl_seg7_vertex_07020B20, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  3,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 8, 12,  9, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ssl_seg7_vertex_07020C20, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 4,  6,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ssl_seg7_vertex_07020D10, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ssl_seg7_vertex_07020E00, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  3, 0x0,  3,  5,  7, 0x0),
    gsSP2Triangles( 7,  5,  8, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 6, 10,  4, 0x0,  9, 10,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x07021870 - 0x07021910
static const Gfx ssl_seg7_dl_07021870[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, generic_09007800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ssl_seg7_vertex_07020EB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  0,  5, 0x0,  0,  3,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 12, 10, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ssl_seg7_vertex_07020FB0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  7, 13,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x07021910 - 0x07021968
static const Gfx ssl_seg7_dl_07021910[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ssl_seg7_texture_07001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ssl_seg7_vertex_07021090, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x07021968 - 0x070219D0
static const Gfx ssl_seg7_dl_07021968[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, generic_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ssl_seg7_vertex_07021150, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0, 15,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x070219D0 - 0x07021A08
static const Gfx ssl_seg7_dl_070219D0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, generic_09002000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ssl_seg7_vertex_07021250, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07021A08 - 0x07021AC8
const Gfx ssl_seg7_dl_07021A08[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ssl_seg7_dl_07021290),
    gsSPDisplayList(ssl_seg7_dl_070214E8),
    gsSPDisplayList(ssl_seg7_dl_07021638),
    gsSPDisplayList(ssl_seg7_dl_07021870),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ssl_seg7_dl_07021910),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ssl_seg7_dl_07021968),
    gsSPDisplayList(ssl_seg7_dl_070219D0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
