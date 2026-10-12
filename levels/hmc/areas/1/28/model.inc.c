// 0x0701F888 - 0x0701F988
static const Vtx hmc_seg7_vertex_0701F888[] = {
#include "assets/rom/generated/a71b2e84baf8555de875.inc.c"















};

// 0x0701F988 - 0x0701FA68
static const Vtx hmc_seg7_vertex_0701F988[] = {
#include "assets/rom/generated/16a37c89df8fa5238d22.inc.c"













};

// 0x0701FA68 - 0x0701FB68
static const Vtx hmc_seg7_vertex_0701FA68[] = {
#include "assets/rom/generated/6f88fbe30b316fc718c9.inc.c"















};

// 0x0701FB68 - 0x0701FC28
static const Vtx hmc_seg7_vertex_0701FB68[] = {
#include "assets/rom/generated/8f55c754d22e607fce62.inc.c"











};

// 0x0701FC28 - 0x0701FD58
static const Gfx hmc_seg7_dl_0701FC28[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, cave_0900A000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(hmc_seg7_vertex_0701F888, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 6,  9,  7, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 8, 12, 10, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(hmc_seg7_vertex_0701F988, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 1,  5,  3, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 6,  9,  7, 0x0, 10,  9,  6, 0x0),
    gsSP1Triangle(11, 12, 13, 0x0),
    gsSPVertex(hmc_seg7_vertex_0701FA68, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  2,  5, 0x0,  4,  6,  2, 0x0),
    gsSP2Triangles( 5,  1,  7, 0x0,  5,  2,  1, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14, 13, 15, 0x0),
    gsSPVertex(hmc_seg7_vertex_0701FB68, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  5,  7,  8, 0x0),
    gsSP2Triangles( 9,  1, 10, 0x0,  9, 11,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0701FD58 - 0x0701FDC8
const Gfx hmc_seg7_dl_0701FD58[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(hmc_seg7_dl_0701FC28),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};
