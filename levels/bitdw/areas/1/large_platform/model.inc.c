// 0x07002928 - 0x07002A28
static const Vtx bitdw_seg7_vertex_07002928[] = {
#include "assets/rom/generated/7c17837fb0562ca13436.inc.c"















};

// 0x07002A28 - 0x07002B28
static const Vtx bitdw_seg7_vertex_07002A28[] = {
#include "assets/rom/generated/c89d3ea24ae9c26df73b.inc.c"















};

// 0x07002B28 - 0x07002BA8
static const Vtx bitdw_seg7_vertex_07002B28[] = {
#include "assets/rom/generated/30b8d55509fdef33eef2.inc.c"







};

// 0x07002BA8 - 0x07002CA8
static const Vtx bitdw_seg7_vertex_07002BA8[] = {
#include "assets/rom/generated/935bf451a3565bad86e0.inc.c"















};

// 0x07002CA8 - 0x07002D98
static const Vtx bitdw_seg7_vertex_07002CA8[] = {
#include "assets/rom/generated/0ddfd78ae35710efb023.inc.c"














};

// 0x07002D98 - 0x07002E88
static const Vtx bitdw_seg7_vertex_07002D98[] = {
#include "assets/rom/generated/adf550dbb7e89c233c4b.inc.c"














};

// 0x07002E88 - 0x07002F88
static const Vtx bitdw_seg7_vertex_07002E88[] = {
#include "assets/rom/generated/d4adfae75a1703cb3e3a.inc.c"















};

// 0x07002F88 - 0x07003088
static const Vtx bitdw_seg7_vertex_07002F88[] = {
#include "assets/rom/generated/1a8892a3ae1add11a394.inc.c"















};

// 0x07003088 - 0x07003160
static const Gfx bitdw_seg7_dl_07003088[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitdw_seg7_vertex_07002928, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07002A28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07002B28, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x07003160 - 0x070032F8
static const Gfx bitdw_seg7_dl_07003160[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitdw_seg7_vertex_07002BA8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0, 15,  1, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07002CA8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14, 11, 13, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07002D98, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  0, 0x0,  2,  6,  0, 0x0),
    gsSP2Triangles( 8,  5,  9, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles(11,  9,  5, 0x0,  1, 12, 13, 0x0),
    gsSP2Triangles( 2,  1, 13, 0x0,  4, 14, 11, 0x0),
    gsSP1Triangle( 4, 11,  5, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07002E88, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07002F88, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x070032F8 - 0x07003370
const Gfx bitdw_seg7_dl_070032F8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bitdw_seg7_dl_07003088),
    gsSPDisplayList(bitdw_seg7_dl_07003160),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
