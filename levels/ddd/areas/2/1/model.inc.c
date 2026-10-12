// 0x07005FC8 - 0x07005FE0

// 0x07005FE0 - 0x07005FF8

// 0x07005FF8 - 0x07006010

// 0x07006010 - 0x07006028

// 0x07006028 - 0x07006040

// 0x07006040 - 0x07006080
static const Vtx ddd_seg7_vertex_07006040[] = {
#include "assets/rom/generated/1fc03fda687d6cc234af.inc.c"



};

// 0x07006080 - 0x070060C0
static const Vtx ddd_seg7_vertex_07006080[] = {
#include "assets/rom/generated/6f080d300fbce818fd81.inc.c"



};

// 0x070060C0 - 0x07006100
static const Vtx ddd_seg7_vertex_070060C0[] = {
#include "assets/rom/generated/78f84d5e37165da874af.inc.c"



};

// 0x07006100 - 0x07006140
static const Vtx ddd_seg7_vertex_07006100[] = {
#include "assets/rom/generated/4f1d85b2f22ba8a1490c.inc.c"



};

// 0x07006140 - 0x070061C0
static const Vtx ddd_seg7_vertex_07006140[] = {
#include "assets/rom/generated/c247f9d38d2523c6a8f3.inc.c"







};

// 0x070061C0 - 0x07006240
static const Vtx ddd_seg7_vertex_070061C0[] = {
#include "assets/rom/generated/360993a5ed3aec0733e5.inc.c"







};

// 0x07006240 - 0x070062C0
static const Vtx ddd_seg7_vertex_07006240[] = {
#include "assets/rom/generated/aad69c345a86320a3c6e.inc.c"







};

// 0x070062C0 - 0x070063B0
static const Vtx ddd_seg7_vertex_070062C0[] = {
#include "assets/rom/generated/62e9bc208163b9edb65e.inc.c"














};

// 0x070063B0 - 0x070064A0
static const Vtx ddd_seg7_vertex_070063B0[] = {
#include "assets/rom/generated/5c15dca171a3a8813d41.inc.c"














};

// 0x070064A0 - 0x07006520
static const Vtx ddd_seg7_vertex_070064A0[] = {
#include "assets/rom/generated/fcb3e68288cece195f30.inc.c"







};

// 0x07006520 - 0x07006560
static const Vtx ddd_seg7_vertex_07006520[] = {
#include "assets/rom/generated/5946b3e2f443047c6054.inc.c"



};

// 0x07006560 - 0x070065A0
static const Vtx ddd_seg7_vertex_07006560[] = {
#include "assets/rom/generated/a837c75417fcb57f5136.inc.c"



};

// 0x070065A0 - 0x070065E0
static const Vtx ddd_seg7_vertex_070065A0[] = {
#include "assets/rom/generated/62df785d0f895ab77e33.inc.c"



};

// 0x070065E0 - 0x070066E0
static const Vtx ddd_seg7_vertex_070065E0[] = {
#include "assets/rom/generated/ad498f629e8a4a411940.inc.c"















};

// 0x070066E0 - 0x070067E0
static const Vtx ddd_seg7_vertex_070066E0[] = {
#include "assets/rom/generated/ba83d89ba454ce8fa3b4.inc.c"















};

// 0x070067E0 - 0x070068E0
static const Vtx ddd_seg7_vertex_070067E0[] = {
#include "assets/rom/generated/12105f019fe81740bf76.inc.c"















};

// 0x070068E0 - 0x070069B0
static const Vtx ddd_seg7_vertex_070068E0[] = {
#include "assets/rom/generated/38972a4c41a2186bbe19.inc.c"












};

// 0x070069B0 - 0x07006AA0
static const Vtx ddd_seg7_vertex_070069B0[] = {
#include "assets/rom/generated/f5d9b16104af6a2c96dc.inc.c"














};

// 0x07006AA0 - 0x07006BA0
static const Vtx ddd_seg7_vertex_07006AA0[] = {
#include "assets/rom/generated/62fb58fc8b34e3fd2f58.inc.c"















};

// 0x07006BA0 - 0x07006C80
static const Vtx ddd_seg7_vertex_07006BA0[] = {
#include "assets/rom/generated/85f272b81bf42ff3028c.inc.c"













};

// 0x07006C80 - 0x07006D70
static const Vtx ddd_seg7_vertex_07006C80[] = {
#include "assets/rom/generated/b421df3440581bf65626.inc.c"














};

// 0x07006D70 - 0x07006E20
static const Vtx ddd_seg7_vertex_07006D70[] = {
#include "assets/rom/generated/fc70ac116a13c8763175.inc.c"










};

// 0x07006E20 - 0x07006EE0
static const Gfx ddd_seg7_dl_07006E20[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09007800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(ddd_seg7_vertex_07006040, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0x444444ff),
    gsSPLightColor(LIGHT_2, 0x1b1b1bff),
    gsSPVertex(ddd_seg7_vertex_07006080, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0x777777ff),
    gsSPLightColor(LIGHT_2, 0x2f2f2fff),
    gsSPVertex(ddd_seg7_vertex_070060C0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0xaaaaaaff),
    gsSPLightColor(LIGHT_2, 0x444444ff),
    gsSPVertex(ddd_seg7_vertex_07006100, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07006EE0 - 0x07007080
static const Gfx ddd_seg7_dl_07006EE0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09006800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x444444ff),
    gsSPLightColor(LIGHT_2, 0x1b1b1bff),
    gsSPVertex(ddd_seg7_vertex_07006140, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPLightColor(LIGHT_1, 0x777777ff),
    gsSPLightColor(LIGHT_2, 0x2f2f2fff),
    gsSPVertex(ddd_seg7_vertex_070061C0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPLightColor(LIGHT_1, 0xaaaaaaff),
    gsSPLightColor(LIGHT_2, 0x444444ff),
    gsSPVertex(ddd_seg7_vertex_07006240, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(ddd_seg7_vertex_070062C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  3, 0x0,  3,  5,  7, 0x0),
    gsSP2Triangles( 7,  5,  8, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 6, 10,  4, 0x0,  9,  8,  6, 0x0),
    gsSP2Triangles( 8, 10,  6, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_070063B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(ddd_seg7_vertex_070064A0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x07007080 - 0x07007298
static const Gfx ddd_seg7_dl_07007080[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x444444ff),
    gsSPLightColor(LIGHT_2, 0x1b1b1bff),
    gsSPVertex(ddd_seg7_vertex_07006520, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0x777777ff),
    gsSPLightColor(LIGHT_2, 0x2f2f2fff),
    gsSPVertex(ddd_seg7_vertex_07006560, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0xaaaaaaff),
    gsSPLightColor(LIGHT_2, 0x444444ff),
    gsSPVertex(ddd_seg7_vertex_070065A0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(ddd_seg7_vertex_070065E0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  6,  8, 0x0),
    gsSP2Triangles( 6,  9,  8, 0x0,  6,  7,  5, 0x0),
    gsSP2Triangles(10,  3,  5, 0x0, 11, 10, 12, 0x0),
    gsSP2Triangles(11,  3, 10, 0x0,  2,  1, 11, 0x0),
    gsSP2Triangles( 2, 11, 12, 0x0, 13, 10, 14, 0x0),
    gsSP1Triangle( 0, 15,  1, 0x0),
    gsSPVertex(ddd_seg7_vertex_070066E0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  0,  3, 0x0,  4,  5,  0, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  9,  8, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 10, 12, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 14,  2,  6, 0x0),
    gsSP2Triangles( 2,  7,  6, 0x0, 15,  2, 14, 0x0),
    gsSP1Triangle(15,  3,  2, 0x0),
    gsSPVertex(ddd_seg7_vertex_070067E0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  1,  0, 0x0,  4,  0,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11, 12, 0x0),
    gsSP2Triangles(10,  9, 13, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(ddd_seg7_vertex_070068E0, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 5,  6,  4, 0x0,  5,  7,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9, 12, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x07007298 - 0x07007408
static const Gfx ddd_seg7_dl_07007298[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09009000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ddd_seg7_vertex_070069B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  0,  3, 0x0,  5,  4,  3, 0x0),
    gsSP2Triangles( 6,  7,  4, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 8, 11, 12, 0x0,  8, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ddd_seg7_vertex_07006AA0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  7,  6,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ddd_seg7_vertex_07006BA0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(ddd_seg7_vertex_07006C80, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_07006D70, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP1Triangle( 0, 10,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07007408 - 0x070074C8
const Gfx ddd_seg7_dl_07007408[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ddd_seg7_dl_07006E20),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ddd_seg7_dl_07006EE0),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ddd_seg7_dl_07007080),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ddd_seg7_dl_07007298),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
