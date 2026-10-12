// 0x0700CE50 - 0x0700CF50
static const Vtx bits_seg7_vertex_0700CE50[] = {
#include "assets/rom/generated/3cda0cce627e570c9c2b.inc.c"















};

// 0x0700CF50 - 0x0700D040
static const Vtx bits_seg7_vertex_0700CF50[] = {
#include "assets/rom/generated/709b9289b122b74f4a21.inc.c"














};

// 0x0700D040 - 0x0700D0C0
static const Vtx bits_seg7_vertex_0700D040[] = {
#include "assets/rom/generated/66bfbeb7f3fa5842a641.inc.c"







};

// 0x0700D0C0 - 0x0700D140
static const Vtx bits_seg7_vertex_0700D0C0[] = {
#include "assets/rom/generated/10fcd022de750f9af0b7.inc.c"







};

// 0x0700D140 - 0x0700D228
static const Gfx bits_seg7_dl_0700D140[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_0700CE50, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11,  8, 12, 0x0),
    gsSP2Triangles(12, 13, 11, 0x0, 12, 14, 13, 0x0),
    gsSP1Triangle(14, 15, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_0700CF50, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 2,  4,  3, 0x0,  2,  5,  4, 0x0),
    gsSP2Triangles( 6,  1,  0, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 6, 10,  1, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bits_seg7_vertex_0700D040, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700D228 - 0x0700D278
static const Gfx bits_seg7_dl_0700D228[] = {
    gsSPVertex(bits_seg7_vertex_0700D0C0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 7,  5,  0, 0x0,  7,  6,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700D278 - 0x0700D2F0
const Gfx bits_seg7_dl_0700D278[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bits_seg7_dl_0700D140),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(bits_seg7_dl_0700D228),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
