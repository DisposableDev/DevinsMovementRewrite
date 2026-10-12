// 0x070589C0 - 0x070589D8

// 0x070589D8 - 0x070589F0

// 0x070589F0 - 0x07058AF0
static const Vtx inside_castle_seg7_vertex_070589F0[] = {
#include "assets/rom/generated/b97dd3ec42ad0f55d456.inc.c"















};

// 0x07058AF0 - 0x07058BD0
static const Vtx inside_castle_seg7_vertex_07058AF0[] = {
#include "assets/rom/generated/ffd974f8eb9580c6191d.inc.c"













};

// 0x07058BD0 - 0x07058CD0
static const Vtx inside_castle_seg7_vertex_07058BD0[] = {
#include "assets/rom/generated/22112d0fec031476fd88.inc.c"















};

// 0x07058CD0 - 0x07058DB0
static const Vtx inside_castle_seg7_vertex_07058CD0[] = {
#include "assets/rom/generated/09d5ffd17ec2b142dd8a.inc.c"













};

// 0x07058DB0 - 0x07058E50
static const Vtx inside_castle_seg7_vertex_07058DB0[] = {
#include "assets/rom/generated/36ebee266fd14d625794.inc.c"









};

// 0x07058E50 - 0x07058F50
static const Vtx inside_castle_seg7_vertex_07058E50[] = {
#include "assets/rom/generated/8d4c0904eed6616d30c3.inc.c"















};

// 0x07058F50 - 0x07058FC0
static const Vtx inside_castle_seg7_vertex_07058F50[] = {
#include "assets/rom/generated/1bfce949992f9d65d9bb.inc.c"






};

// 0x07058FC0 - 0x07059190
static const Gfx inside_castle_seg7_dl_07058FC0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09004800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffcc65ff),
    gsSPLightColor(LIGHT_2, 0x3f3319ff),
    gsSPVertex(inside_castle_seg7_vertex_070589F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  4,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles( 8, 10, 14, 0x0, 11, 15, 12, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07058AF0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 3, 10,  4, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07058BD0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  5,  4, 13, 0x0),
    gsSP2Triangles(10, 12, 14, 0x0,  5, 13, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07058CD0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 4,  6, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07058DB0, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  7,  8,  9, 0x0),
    gsSPLightColor(LIGHT_1, 0xff0000ff),
    gsSPLightColor(LIGHT_2, 0x3f0000ff),
    gsSPVertex(inside_castle_seg7_vertex_07058E50, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  3, 0x0,  3,  7,  4, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07058F50, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 3,  6,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x07059190 - 0x07059200
const Gfx inside_castle_seg7_dl_07059190[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07058FC0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
