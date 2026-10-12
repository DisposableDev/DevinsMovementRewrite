// 0x070050F8 - 0x070051F8
static const Vtx bitdw_seg7_vertex_070050F8[] = {
#include "assets/rom/generated/e9e98739b33304a611f2.inc.c"















};

// 0x070051F8 - 0x070052F8
static const Vtx bitdw_seg7_vertex_070051F8[] = {
#include "assets/rom/generated/65b7a439a8be77b6ac60.inc.c"















};

// 0x070052F8 - 0x07005378
static const Vtx bitdw_seg7_vertex_070052F8[] = {
#include "assets/rom/generated/adb313f54d7c74fd4c63.inc.c"







};

// 0x07005378 - 0x07005478
static const Vtx bitdw_seg7_vertex_07005378[] = {
#include "assets/rom/generated/a00a20989bbf230ca2ff.inc.c"















};

// 0x07005478 - 0x07005568
static const Vtx bitdw_seg7_vertex_07005478[] = {
#include "assets/rom/generated/cd95da0447cf25639c95.inc.c"














};

// 0x07005568 - 0x07005668
static const Vtx bitdw_seg7_vertex_07005568[] = {
#include "assets/rom/generated/34a9c1e531efdb9dde9f.inc.c"















};

// 0x07005668 - 0x07005748
static const Vtx bitdw_seg7_vertex_07005668[] = {
#include "assets/rom/generated/91f138b40bbf622e4838.inc.c"













};

// 0x07005748 - 0x070057F8
static const Vtx bitdw_seg7_vertex_07005748[] = {
#include "assets/rom/generated/a1f2cdd264c4903195e1.inc.c"










};

// 0x070057F8 - 0x070058F8
static const Vtx bitdw_seg7_vertex_070057F8[] = {
#include "assets/rom/generated/0035b5c857787938010d.inc.c"















};

// 0x070058F8 - 0x070059D0
static const Gfx bitdw_seg7_dl_070058F8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitdw_seg7_vertex_070050F8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bitdw_seg7_vertex_070051F8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 15, 12, 14, 0x0),
    gsSPVertex(bitdw_seg7_vertex_070052F8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x070059D0 - 0x07005B58
static const Gfx bitdw_seg7_dl_070059D0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitdw_seg7_vertex_07005378, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 0,  2,  5, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 15, 0x0,  9, 11, 13, 0x0),
    gsSP1Triangle(11, 15, 12, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07005478, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  2,  6,  4, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 7, 12, 13, 0x0,  7, 13,  8, 0x0),
    gsSP1Triangle( 7, 14, 12, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07005568, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07005668, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  7,  6, 0x0,  8,  6,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07005748, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 4,  8,  9, 0x0, 10,  4,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x07005B58 - 0x07005BC0
static const Gfx bitdw_seg7_dl_07005B58[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bitdw_seg7_texture_07000000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitdw_seg7_vertex_070057F8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x07005BC0 - 0x07005C40
const Gfx bitdw_seg7_dl_07005BC0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bitdw_seg7_dl_070058F8),
    gsSPDisplayList(bitdw_seg7_dl_070059D0),
    gsSPDisplayList(bitdw_seg7_dl_07005B58),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
