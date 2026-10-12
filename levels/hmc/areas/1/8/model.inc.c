// 0x0700F458 - 0x0700F548
static const Vtx hmc_seg7_vertex_0700F458[] = {
#include "assets/rom/generated/62546653748a77d7b999.inc.c"














};

// 0x0700F548 - 0x0700F628
static const Vtx hmc_seg7_vertex_0700F548[] = {
#include "assets/rom/generated/2803dd35ec0fb8c16572.inc.c"













};

// 0x0700F628 - 0x0700F728
static const Vtx hmc_seg7_vertex_0700F628[] = {
#include "assets/rom/generated/0edc1b9771e0c323cd29.inc.c"















};

// 0x0700F728 - 0x0700F808
static const Vtx hmc_seg7_vertex_0700F728[] = {
#include "assets/rom/generated/712fbfb393f0632b884f.inc.c"













};

// 0x0700F808 - 0x0700F8B8
static const Vtx hmc_seg7_vertex_0700F808[] = {
#include "assets/rom/generated/9d6ab1becf531c1bdf68.inc.c"










};

// 0x0700F8B8 - 0x0700FA40
static const Gfx hmc_seg7_dl_0700F8B8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, cave_0900A000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(hmc_seg7_vertex_0700F458, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  5,  4,  6, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 8, 11,  9, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle( 0,  2,  4, 0x0),
    gsSPVertex(hmc_seg7_vertex_0700F548, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 1, 12,  2, 0x0,  1, 13, 12, 0x0),
    gsSPVertex(hmc_seg7_vertex_0700F628, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13,  2, 0x0),
    gsSP1Triangle(14, 15,  6, 0x0),
    gsSPVertex(hmc_seg7_vertex_0700F728, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 8, 10, 11, 0x0,  1, 12, 13, 0x0),
    gsSP1Triangle( 1, 13,  2, 0x0),
    gsSPVertex(hmc_seg7_vertex_0700F808, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  5,  1,  4, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP1Triangle( 0, 10,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700FA40 - 0x0700FAB0
const Gfx hmc_seg7_dl_0700FA40[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(hmc_seg7_dl_0700F8B8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};
