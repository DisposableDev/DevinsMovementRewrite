// 0x07011940 - 0x07011958

// 0x07011958 - 0x07012158
ALIGNED8 static const Texture ccm_seg7_texture_07011958[] = {
#include "levels/ccm/12.rgba16.inc.c"
};

// 0x07012158 - 0x07012258
static const Vtx ccm_seg7_vertex_07012158[] = {
#include "assets/rom/generated/1c1985c624ac5cd30c76.inc.c"















};

// 0x07012258 - 0x07012358
static const Vtx ccm_seg7_vertex_07012258[] = {
#include "assets/rom/generated/35d94deb46b058794b0d.inc.c"















};

// 0x07012358 - 0x07012458
static const Vtx ccm_seg7_vertex_07012358[] = {
#include "assets/rom/generated/fdb329cbfe23715623df.inc.c"















};

// 0x07012458 - 0x07012558
static const Vtx ccm_seg7_vertex_07012458[] = {
#include "assets/rom/generated/64d646eab881b9604125.inc.c"















};

// 0x07012558 - 0x07012658
static const Vtx ccm_seg7_vertex_07012558[] = {
#include "assets/rom/generated/6f81dd88d313f818e9f5.inc.c"















};

// 0x07012658 - 0x07012758
static const Vtx ccm_seg7_vertex_07012658[] = {
#include "assets/rom/generated/90f8c252fa79f2695df0.inc.c"















};

// 0x07012758 - 0x07012858
static const Vtx ccm_seg7_vertex_07012758[] = {
#include "assets/rom/generated/fcf9bed9243bea2b9c57.inc.c"















};

// 0x07012858 - 0x070128E8
static const Vtx ccm_seg7_vertex_07012858[] = {
#include "assets/rom/generated/65c6613463fb6a51e025.inc.c"








};

// 0x070128E8 - 0x07012BD8
static const Gfx ccm_seg7_dl_070128E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ccm_seg7_texture_07011958),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0xccccccff),
    gsSPVertex(ccm_seg7_vertex_07012158, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  4,  7,  6, 0x0),
    gsSP2Triangles( 5,  6,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(10, 12, 11, 0x0, 13,  2, 14, 0x0),
    gsSP1Triangle( 3,  5, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_07012258, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  5,  8,  6, 0x0),
    gsSP2Triangles( 6,  8,  9, 0x0, 10, 11, 12, 0x0),
    gsSP2Triangles(11, 13, 12, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle( 2,  4, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_07012358, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 1,  4,  3, 0x0,  2,  3,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  9,  8, 0x0),
    gsSP2Triangles( 7, 10,  9, 0x0,  8,  9, 11, 0x0),
    gsSP2Triangles(10, 12, 13, 0x0, 12, 14, 13, 0x0),
    gsSP2Triangles(12,  5, 14, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_07012458, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 1,  4,  3, 0x0,  2,  3,  5, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  6,  8,  7, 0x0),
    gsSP2Triangles( 6,  9,  8, 0x0,  7,  8, 10, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 12, 14, 13, 0x0),
    gsSP1Triangle(12, 15, 14, 0x0),
    gsSPVertex(ccm_seg7_vertex_07012558, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13,  0, 14, 0x0),
    gsSP2Triangles( 0, 15, 14, 0x0,  0,  2, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_07012658, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  4,  7,  6, 0x0),
    gsSP2Triangles( 5,  6,  8, 0x0,  9,  0, 10, 0x0),
    gsSP2Triangles( 0, 11, 10, 0x0,  0,  2, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  2, 13, 14, 0x0),
    gsSP2Triangles(13, 15, 14, 0x0, 13,  8, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_07012758, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  6,  9, 10, 0x0),
    gsSP2Triangles( 9, 11, 10, 0x0,  9, 12, 11, 0x0),
    gsSP2Triangles(10, 11, 13, 0x0,  3,  5, 14, 0x0),
    gsSP2Triangles( 5, 15, 14, 0x0,  5,  8, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_07012858, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  0, 0x0,  7,  1,  0, 0x0),
    gsSP1Triangle( 7,  8,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07012BD8 - 0x07012C38
const Gfx ccm_seg7_dl_07012BD8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ccm_seg7_dl_070128E8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
