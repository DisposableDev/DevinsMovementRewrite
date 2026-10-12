// 0x0700DD98 - 0x0700DE98
static const Vtx bits_seg7_vertex_0700DD98[] = {
#include "assets/rom/generated/5148be0cf8c0b8f8cee9.inc.c"















};

// 0x0700DE98 - 0x0700DF98
static const Vtx bits_seg7_vertex_0700DE98[] = {
#include "assets/rom/generated/49c0bb0d7f6ed6e36ab5.inc.c"















};

// 0x0700DF98 - 0x0700E078
static const Vtx bits_seg7_vertex_0700DF98[] = {
#include "assets/rom/generated/b2fa6931bedf86c1b0f2.inc.c"













};

// 0x0700E078 - 0x0700E178
static const Vtx bits_seg7_vertex_0700E078[] = {
#include "assets/rom/generated/e8a3ea6804a3df55b425.inc.c"















};

// 0x0700E178 - 0x0700E268
static const Vtx bits_seg7_vertex_0700E178[] = {
#include "assets/rom/generated/52ca03adb5b454bfc472.inc.c"














};

// 0x0700E268 - 0x0700E358
static const Vtx bits_seg7_vertex_0700E268[] = {
#include "assets/rom/generated/afe76d72693f47e50f46.inc.c"














};

// 0x0700E358 - 0x0700E458
static const Vtx bits_seg7_vertex_0700E358[] = {
#include "assets/rom/generated/81957cf97f05218fa414.inc.c"















};

// 0x0700E458 - 0x0700E548
static const Vtx bits_seg7_vertex_0700E458[] = {
#include "assets/rom/generated/26b617cca1de2415c4a9.inc.c"














};

// 0x0700E548 - 0x0700E628
static const Vtx bits_seg7_vertex_0700E548[] = {
#include "assets/rom/generated/22aa31c697fd7db65ea6.inc.c"













};

// 0x0700E628 - 0x0700E718
static const Vtx bits_seg7_vertex_0700E628[] = {
#include "assets/rom/generated/d8d4e242c6e03fbeac9f.inc.c"














};

// 0x0700E718 - 0x0700E808
static const Vtx bits_seg7_vertex_0700E718[] = {
#include "assets/rom/generated/16b32a55659698af1844.inc.c"














};

// 0x0700E808 - 0x0700E908
static const Vtx bits_seg7_vertex_0700E808[] = {
#include "assets/rom/generated/bf5535a43d731418d912.inc.c"















};

// 0x0700E908 - 0x0700EA08
static const Vtx bits_seg7_vertex_0700E908[] = {
#include "assets/rom/generated/ef709ca46802ca93036d.inc.c"















};

// 0x0700EA08 - 0x0700EB08
static const Vtx bits_seg7_vertex_0700EA08[] = {
#include "assets/rom/generated/0f5dbde1790c0ec3cbc7.inc.c"















};

// 0x0700EB08 - 0x0700EBF8
static const Vtx bits_seg7_vertex_0700EB08[] = {
#include "assets/rom/generated/278e13eedb29619687ee.inc.c"














};

// 0x0700EBF8 - 0x0700ECD8
static const Vtx bits_seg7_vertex_0700EBF8[] = {
#include "assets/rom/generated/b111627be427e64867c1.inc.c"













};

// 0x0700ECD8 - 0x0700EDC8
static const Vtx bits_seg7_vertex_0700ECD8[] = {
#include "assets/rom/generated/73e45b5336e28de13835.inc.c"














};

// 0x0700EDC8 - 0x0700EEC8
static const Vtx bits_seg7_vertex_0700EDC8[] = {
#include "assets/rom/generated/c16cb923ef1a7fb378f6.inc.c"















};

// 0x0700EEC8 - 0x0700EFC8
static const Vtx bits_seg7_vertex_0700EEC8[] = {
#include "assets/rom/generated/b5ac99bf5fec47771840.inc.c"















};

// 0x0700EFC8 - 0x0700F0B8
static const Vtx bits_seg7_vertex_0700EFC8[] = {
#include "assets/rom/generated/0c67cf85bfee8f15c457.inc.c"














};

// 0x0700F0B8 - 0x0700F198
static const Vtx bits_seg7_vertex_0700F0B8[] = {
#include "assets/rom/generated/f91835dac7b9df897823.inc.c"













};

// 0x0700F198 - 0x0700F298
static const Vtx bits_seg7_vertex_0700F198[] = {
#include "assets/rom/generated/5dd1600ac96504ffda32.inc.c"















};

// 0x0700F298 - 0x0700F398
static const Vtx bits_seg7_vertex_0700F298[] = {
#include "assets/rom/generated/419b8503ba00340b3cfb.inc.c"















};

// 0x0700F398 - 0x0700F478
static const Vtx bits_seg7_vertex_0700F398[] = {
#include "assets/rom/generated/4cca17121cdca8cc58b6.inc.c"













};

// 0x0700F478 - 0x0700F568
static const Vtx bits_seg7_vertex_0700F478[] = {
#include "assets/rom/generated/5f014075b591473fb828.inc.c"














};

// 0x0700F568 - 0x0700F790
static const Gfx bits_seg7_dl_0700F568[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_0700DD98, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 10,  7, 0x0),
    gsSP2Triangles(11, 12, 10, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700DE98, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 3,  4,  1, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  5, 10,  6, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 11, 13, 14, 0x0),
    gsSP1Triangle(14, 13, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700DF98, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 8, 11, 12, 0x0,  9,  8, 12, 0x0),
    gsSP1Triangle( 9, 12, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_0700E078, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 7,  6,  8, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP2Triangles(14, 13, 10, 0x0, 14, 15, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_0700E178, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bits_seg7_vertex_0700E268, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bits_seg7_vertex_0700E358, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700F790 - 0x0700FC70
static const Gfx bits_seg7_dl_0700F790[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_0700E458, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14, 11, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_0700E548, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  6,  8,  7, 0x0),
    gsSP2Triangles( 9,  0,  2, 0x0, 10, 11, 12, 0x0),
    gsSP1Triangle(10, 12, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_0700E628, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11,  8, 10, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bits_seg7_vertex_0700E718, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bits_seg7_vertex_0700E808, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  1, 0x0),
    gsSP2Triangles( 3,  4,  2, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 6,  8,  7, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 15,  9, 11, 0x0),
    gsSPVertex(bits_seg7_vertex_0700E908, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  3,  9, 0x0, 10, 11, 12, 0x0),
    gsSP2Triangles(13, 14, 10, 0x0, 13, 10, 15, 0x0),
    gsSP1Triangle(15, 10, 12, 0x0),
    gsSPVertex(bits_seg7_vertex_0700EA08, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  4,  1,  6, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0, 10, 11,  7, 0x0),
    gsSP2Triangles( 9, 10,  7, 0x0, 11, 12,  7, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700EB08, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bits_seg7_vertex_0700EBF8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_0700ECD8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(bits_seg7_vertex_0700EDC8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9, 11,  0, 0x0),
    gsSP2Triangles( 0, 12,  1, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700EEC8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 7,  8,  6, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 15, 12, 14, 0x0),
    gsSPVertex(bits_seg7_vertex_0700EFC8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14, 11, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_0700F0B8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_0700F198, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0, 15, 12, 11, 0x0),
    gsSPVertex(bits_seg7_vertex_0700F298, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 3,  1,  0, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 6,  8,  7, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12,  9, 11, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700F398, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  4,  7,  8, 0x0),
    gsSP2Triangles( 4,  8,  5, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 7, 12, 13, 0x0,  7, 13,  8, 0x0),
    gsSPVertex(bits_seg7_vertex_0700F478, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle( 9, 14, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700FC70 - 0x0700FCE8
const Gfx bits_seg7_dl_0700FC70[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bits_seg7_dl_0700F568),
    gsSPDisplayList(bits_seg7_dl_0700F790),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
