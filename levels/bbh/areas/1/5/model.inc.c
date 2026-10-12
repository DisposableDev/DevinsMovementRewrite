// 0x07008040 - 0x07008140
static const Vtx bbh_seg7_vertex_07008040[] = {
#include "assets/rom/generated/42c8dda9a023d11b27b8.inc.c"















};

// 0x07008140 - 0x07008240
static const Vtx bbh_seg7_vertex_07008140[] = {
#include "assets/rom/generated/7ab907b6a7d7d2ebf5c1.inc.c"















};

// 0x07008240 - 0x07008330
static const Vtx bbh_seg7_vertex_07008240[] = {
#include "assets/rom/generated/5f0466e329e04d126b75.inc.c"














};

// 0x07008330 - 0x07008430
static const Vtx bbh_seg7_vertex_07008330[] = {
#include "assets/rom/generated/c7b1838bce5b9593a78c.inc.c"















};

// 0x07008430 - 0x07008530
static const Vtx bbh_seg7_vertex_07008430[] = {
#include "assets/rom/generated/97b6d4ebbba5e1b6bd80.inc.c"















};

// 0x07008530 - 0x07008620
static const Vtx bbh_seg7_vertex_07008530[] = {
#include "assets/rom/generated/812c112a08b81086d4f9.inc.c"














};

// 0x07008620 - 0x07008720
static const Vtx bbh_seg7_vertex_07008620[] = {
#include "assets/rom/generated/9e210b0c1d1b197fe288.inc.c"















};

// 0x07008720 - 0x07008820
static const Vtx bbh_seg7_vertex_07008720[] = {
#include "assets/rom/generated/b12772065a4933a244cb.inc.c"















};

// 0x07008820 - 0x070088E0
static const Vtx bbh_seg7_vertex_07008820[] = {
#include "assets/rom/generated/742ccc0cd03134b5d28a.inc.c"











};

// 0x070088E0 - 0x07008B58
static const Gfx bbh_seg7_dl_070088E0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09006800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_07008040, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  0,  8, 0x0),
    gsSP2Triangles( 9,  1, 10, 0x0,  9, 11,  1, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(bbh_seg7_vertex_07008140, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 1,  6,  7, 0x0,  1,  7,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9, 11, 12, 0x0),
    gsSP2Triangles(13, 14, 10, 0x0, 13, 10, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_07008240, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13,  6, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_07008330, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 5, 10, 11, 0x0,  5, 11, 12, 0x0),
    gsSP2Triangles(13, 14, 10, 0x0, 13, 10, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_07008430, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 5,  2, 10, 0x0,  5, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_07008530, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  0,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_07008620, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  2,  5, 0x0,  4,  5,  6, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  7,  9, 10, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 14, 10, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_07008720, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11,  5, 0x0, 10, 12, 11, 0x0),
    gsSP2Triangles(13, 14, 15, 0x0, 13, 11, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_07008820, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  3,  8, 0x0),
    gsSP2Triangles( 9,  2, 10, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008B58 - 0x07008BC8
const Gfx bbh_seg7_dl_07008B58[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_070088E0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
