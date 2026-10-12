// 0x07004B10 - 0x07004C10
static const Vtx rr_seg7_vertex_07004B10[] = {
#include "assets/rom/generated/044d6044006d5b466a15.inc.c"















};

// 0x07004C10 - 0x07004D10
static const Vtx rr_seg7_vertex_07004C10[] = {
#include "assets/rom/generated/20f0f6b1a03545c03fc9.inc.c"















};

// 0x07004D10 - 0x07004E00
static const Vtx rr_seg7_vertex_07004D10[] = {
#include "assets/rom/generated/0bf6780a10efa384879d.inc.c"














};

// 0x07004E00 - 0x07004EE0
static const Vtx rr_seg7_vertex_07004E00[] = {
#include "assets/rom/generated/90da97a99550a2ff81d9.inc.c"













};

// 0x07004EE0 - 0x07004FD0
static const Vtx rr_seg7_vertex_07004EE0[] = {
#include "assets/rom/generated/8bcfd08a5b551ba1bdac.inc.c"














};

// 0x07004FD0 - 0x070050C0
static const Vtx rr_seg7_vertex_07004FD0[] = {
#include "assets/rom/generated/be0594515d0e79ab8f27.inc.c"














};

// 0x070050C0 - 0x070051B0
static const Vtx rr_seg7_vertex_070050C0[] = {
#include "assets/rom/generated/46e6f16991ebd92a7d8a.inc.c"














};

// 0x070051B0 - 0x070052B0
static const Vtx rr_seg7_vertex_070051B0[] = {
#include "assets/rom/generated/2cd02d121502b3956b01.inc.c"















};

// 0x070052B0 - 0x070053B0
static const Vtx rr_seg7_vertex_070052B0[] = {
#include "assets/rom/generated/179757f96d75ddfec8a8.inc.c"















};

// 0x070053B0 - 0x070054B0
static const Vtx rr_seg7_vertex_070053B0[] = {
#include "assets/rom/generated/e48b3759262963368262.inc.c"















};

// 0x070054B0 - 0x070055B0
static const Vtx rr_seg7_vertex_070054B0[] = {
#include "assets/rom/generated/6de80554dd2012504bb4.inc.c"















};

// 0x070055B0 - 0x070056B0
static const Vtx rr_seg7_vertex_070055B0[] = {
#include "assets/rom/generated/17ccc4bddd62aa5df961.inc.c"















};

// 0x070056B0 - 0x070056F0
static const Vtx rr_seg7_vertex_070056B0[] = {
#include "assets/rom/generated/65b595ab9e7a9d034062.inc.c"



};

// 0x070056F0 - 0x070057F0
static const Vtx rr_seg7_vertex_070056F0[] = {
#include "assets/rom/generated/994097decfd38d0db1b0.inc.c"















};

// 0x070057F0 - 0x07005A48
static const Gfx rr_seg7_dl_070057F0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_07004B10, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_07004C10, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0, 15,  1, 0x0),
    gsSPVertex(rr_seg7_vertex_07004D10, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 3,  0,  2, 0x0,  1,  0,  5, 0x0),
    gsSP2Triangles( 2,  1,  6, 0x0,  1,  5,  7, 0x0),
    gsSP2Triangles( 2,  8,  3, 0x0,  2,  9,  8, 0x0),
    gsSP2Triangles( 2, 10,  9, 0x0,  2, 11, 10, 0x0),
    gsSP2Triangles( 2,  6, 12, 0x0, 11,  2, 13, 0x0),
    gsSP2Triangles(14, 13,  2, 0x0, 14,  7,  5, 0x0),
    gsSPVertex(rr_seg7_vertex_07004E00, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  2, 0x0,  6,  7,  1, 0x0),
    gsSP2Triangles( 6,  1,  0, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 8, 11,  9, 0x0, 12,  8,  0, 0x0),
    gsSP2Triangles(12,  5, 13, 0x0, 12,  0,  2, 0x0),
    gsSP1Triangle( 2,  5, 12, 0x0),
    gsSPVertex(rr_seg7_vertex_07004EE0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  5,  7,  8, 0x0),
    gsSP2Triangles( 7,  9,  8, 0x0, 10, 11,  6, 0x0),
    gsSP2Triangles(10,  6,  3, 0x0,  5, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(rr_seg7_vertex_07004FD0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_070050C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  2,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x07005A48 - 0x07005C18
static const Gfx rr_seg7_dl_07005A48[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_070051B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 13, 15, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_070052B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 4,  5,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 2,  6,  0, 0x0,  6,  7,  0, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 13, 15, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_070053B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9,  6, 0x0,  4, 10, 11, 0x0),
    gsSP2Triangles( 4, 12, 10, 0x0,  7, 13, 11, 0x0),
    gsSP2Triangles( 7, 14, 13, 0x0,  5, 15,  6, 0x0),
    gsSPVertex(rr_seg7_vertex_070054B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 2,  5,  0, 0x0,  5,  3,  0, 0x0),
    gsSP2Triangles( 1,  6,  2, 0x0,  6,  7,  2, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_070055B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_070056B0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07005C18 - 0x07005C80
static const Gfx rr_seg7_dl_07005C18[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_070056F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x07005C80 - 0x07005D00
const Gfx rr_seg7_dl_07005C80[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(rr_seg7_dl_070057F0),
    gsSPDisplayList(rr_seg7_dl_07005A48),
    gsSPDisplayList(rr_seg7_dl_07005C18),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
