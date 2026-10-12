// 0x0702E490 - 0x0702E4A8

// 0x0702E4A8 - 0x0702E4C0

// 0x0702E4C0 - 0x0702E4D8

// 0x0702E4D8 - 0x0702E4F0

// 0x0702E4F0 - 0x0702E508

// 0x0702E508 - 0x0702E608
static const Vtx inside_castle_seg7_vertex_0702E508[] = {
#include "assets/rom/generated/ac934e5a033bc25f3149.inc.c"















};

// 0x0702E608 - 0x0702E708
static const Vtx inside_castle_seg7_vertex_0702E608[] = {
#include "assets/rom/generated/534848459015b2cc44ad.inc.c"















};

// 0x0702E708 - 0x0702E808
static const Vtx inside_castle_seg7_vertex_0702E708[] = {
#include "assets/rom/generated/b4b4e54cbb86f9fa6442.inc.c"















};

// 0x0702E808 - 0x0702E8F8
static const Vtx inside_castle_seg7_vertex_0702E808[] = {
#include "assets/rom/generated/33394c41aaf7a3d7d2f4.inc.c"














};

// 0x0702E8F8 - 0x0702E9E8
static const Vtx inside_castle_seg7_vertex_0702E8F8[] = {
#include "assets/rom/generated/6b402d03ed3fa3520a52.inc.c"














};

// 0x0702E9E8 - 0x0702EA68
static const Vtx inside_castle_seg7_vertex_0702E9E8[] = {
#include "assets/rom/generated/939b71ec483b44fe3b57.inc.c"







};

// 0x0702EA68 - 0x0702EB58
static const Vtx inside_castle_seg7_vertex_0702EA68[] = {
#include "assets/rom/generated/bd9b37cb3b2061ff7006.inc.c"














};

// 0x0702EB58 - 0x0702EC48
static const Vtx inside_castle_seg7_vertex_0702EB58[] = {
#include "assets/rom/generated/8be397de8fcd2086edf1.inc.c"














};

// 0x0702EC48 - 0x0702ED48
static const Vtx inside_castle_seg7_vertex_0702EC48[] = {
#include "assets/rom/generated/d708895e50a22fb7d68a.inc.c"















};

// 0x0702ED48 - 0x0702EE48
static const Vtx inside_castle_seg7_vertex_0702ED48[] = {
#include "assets/rom/generated/5afd06b633033910690c.inc.c"















};

// 0x0702EE48 - 0x0702EE78
static const Vtx inside_castle_seg7_vertex_0702EE48[] = {
#include "assets/rom/generated/ce3c94e7bfdc73282647.inc.c"


};

// 0x0702EE78 - 0x0702EF68
static const Vtx inside_castle_seg7_vertex_0702EE78[] = {
#include "assets/rom/generated/ba372a771240b5a39a41.inc.c"














};

// 0x0702EF68 - 0x0702F048
static const Vtx inside_castle_seg7_vertex_0702EF68[] = {
#include "assets/rom/generated/dc6ef2ac6e41e15f718d.inc.c"













};

// 0x0702F048 - 0x0702F138
static const Vtx inside_castle_seg7_vertex_0702F048[] = {
#include "assets/rom/generated/97864d5b64a9ba930919.inc.c"














};

// 0x0702F138 - 0x0702F228
static const Vtx inside_castle_seg7_vertex_0702F138[] = {
#include "assets/rom/generated/f66d7ad380436f66f745.inc.c"














};

// 0x0702F228 - 0x0702F328
static const Vtx inside_castle_seg7_vertex_0702F228[] = {
#include "assets/rom/generated/06474fad6d77f30a061d.inc.c"















};

// 0x0702F328 - 0x0702F418
static const Vtx inside_castle_seg7_vertex_0702F328[] = {
#include "assets/rom/generated/e5b97678d4266a9ff5b8.inc.c"














};

// 0x0702F418 - 0x0702F508
static const Vtx inside_castle_seg7_vertex_0702F418[] = {
#include "assets/rom/generated/8a764f1b3b9532005dca.inc.c"














};

// 0x0702F508 - 0x0702F5F8
static const Vtx inside_castle_seg7_vertex_0702F508[] = {
#include "assets/rom/generated/684c48075b4e8f0865e8.inc.c"














};

// 0x0702F5F8 - 0x0702F638
static const Vtx inside_castle_seg7_vertex_0702F5F8[] = {
#include "assets/rom/generated/2d8e8f0247deb861f68c.inc.c"



};

// 0x0702F638 - 0x0702F6B8
static const Vtx inside_castle_seg7_vertex_0702F638[] = {
#include "assets/rom/generated/870ff4b91f4d2be4ef6d.inc.c"







};

// 0x0702F6B8 - 0x0702F738
static const Vtx inside_castle_seg7_vertex_0702F6B8[] = {
#include "assets/rom/generated/cf5119fe0de9bb6dc901.inc.c"







};

// 0x0702F738 - 0x0702F8D0
static const Gfx inside_castle_seg7_dl_0702F738[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xddddddff),
    gsSPLightColor(LIGHT_2, 0x525252ff),
    gsSPVertex(inside_castle_seg7_vertex_0702E508, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(13, 12, 14, 0x0,  0,  2, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702E608, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(10,  9, 14, 0x0, 15, 12, 11, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x5f5f5fff),
    gsSPVertex(inside_castle_seg7_vertex_0702E708, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0,  2, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702E808, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(10,  9, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702E8F8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0702F8D0 - 0x0702F918
static const Gfx inside_castle_seg7_dl_0702F8D0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_0702E9E8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0702F918 - 0x0702FA80
static const Gfx inside_castle_seg7_dl_0702F918[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_0702EA68, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 4,  7,  8, 0x0,  9,  7, 10, 0x0),
    gsSP2Triangles( 9,  8,  7, 0x0,  3,  5,  9, 0x0),
    gsSP2Triangles( 9, 10,  3, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14, 11, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702EB58, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702EC48, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  3,  8, 0x0),
    gsSP2Triangles( 3,  9,  8, 0x0, 10, 11, 12, 0x0),
    gsSP2Triangles(11, 13, 12, 0x0, 14,  0,  2, 0x0),
    gsSP1Triangle( 0, 15,  1, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702ED48, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 3,  0,  2, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 5,  8,  6, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  9, 11, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702EE48, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0702FA80 - 0x0702FC90
static const Gfx inside_castle_seg7_dl_0702FA80[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_0702EE78, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14, 11, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702EF68, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702F048, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702F138, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702F228, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702F328, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702F418, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0702F508, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  0,  2,  9, 0x0),
    gsSP2Triangles( 6, 10,  7, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0702FC90 - 0x0702FD30
static const Gfx inside_castle_seg7_dl_0702FC90[] = {
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(inside_castle_seg7_vertex_0702F5F8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0x131313ff),
    gsSPLightColor(LIGHT_2, 0x70707ff),
    gsSPVertex(inside_castle_seg7_vertex_0702F638, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0x292929ff),
    gsSPLightColor(LIGHT_2, 0xf0f0fff),
    gsSPVertex(inside_castle_seg7_vertex_0702F6B8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0702FD30 - 0x0702FDD8
const Gfx inside_castle_seg7_dl_0702FD30[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_0702F738),
    gsSPDisplayList(inside_castle_seg7_dl_0702F8D0),
    gsSPDisplayList(inside_castle_seg7_dl_0702F918),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_0702FA80),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(inside_castle_seg7_dl_0702FC90),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
