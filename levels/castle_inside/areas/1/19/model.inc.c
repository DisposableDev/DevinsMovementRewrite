// 0x07036E68 - 0x07036E80

// 0x07036E80 - 0x07036F70
static const Vtx inside_castle_seg7_vertex_07036E80[] = {
#include "assets/rom/generated/8c369d8868ecf052db64.inc.c"














};

// 0x07036F70 - 0x07037060
static const Vtx inside_castle_seg7_vertex_07036F70[] = {
#include "assets/rom/generated/915852c7ceb1a2c56069.inc.c"














};

// 0x07037060 - 0x07037150
static const Vtx inside_castle_seg7_vertex_07037060[] = {
#include "assets/rom/generated/e58dcdbbcf0e109c584b.inc.c"














};

// 0x07037150 - 0x07037240
static const Vtx inside_castle_seg7_vertex_07037150[] = {
#include "assets/rom/generated/f0076451da4b2bfa329e.inc.c"














};

// 0x07037240 - 0x07037320
static const Vtx inside_castle_seg7_vertex_07037240[] = {
#include "assets/rom/generated/d9f16a0295a6fb974ebd.inc.c"













};

// 0x07037320 - 0x07037420
static const Vtx inside_castle_seg7_vertex_07037320[] = {
#include "assets/rom/generated/df555b6981bce654b6d0.inc.c"















};

// 0x07037420 - 0x07037520
static const Vtx inside_castle_seg7_vertex_07037420[] = {
#include "assets/rom/generated/1f2d72d7e761a32891f2.inc.c"















};

// 0x07037520 - 0x07037620
static const Vtx inside_castle_seg7_vertex_07037520[] = {
#include "assets/rom/generated/47eb7ee8a102eedb453b.inc.c"















};

// 0x07037620 - 0x07037710
static const Vtx inside_castle_seg7_vertex_07037620[] = {
#include "assets/rom/generated/d726ef573d5f0355dd5f.inc.c"














};

// 0x07037710 - 0x07037988
static const Gfx inside_castle_seg7_dl_07037710[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09003000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0xccccccff),
    gsSPVertex(inside_castle_seg7_vertex_07036E80, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07036F70, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07037060, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07037150, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07037240, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(10,  9, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07037320, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07037420, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07037520, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07037620, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 6, 10, 11, 0x0, 12,  9, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  7,  9, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x07037988 - 0x070379F8
const Gfx inside_castle_seg7_dl_07037988[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07037710),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
