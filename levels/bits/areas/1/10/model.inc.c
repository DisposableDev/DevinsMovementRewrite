// 0x07009058 - 0x07009158
static const Vtx bits_seg7_vertex_07009058[] = {
#include "assets/rom/generated/c6c8caf87a931b919180.inc.c"















};

// 0x07009158 - 0x07009258
static const Vtx bits_seg7_vertex_07009158[] = {
#include "assets/rom/generated/ece03a9b9e5adad0d99f.inc.c"















};

// 0x07009258 - 0x07009338
static const Vtx bits_seg7_vertex_07009258[] = {
#include "assets/rom/generated/46a652a0b774bb2f00e3.inc.c"













};

// 0x07009338 - 0x07009438
static const Vtx bits_seg7_vertex_07009338[] = {
#include "assets/rom/generated/345e5985799cfd16de61.inc.c"















};

// 0x07009438 - 0x070094F8
static const Vtx bits_seg7_vertex_07009438[] = {
#include "assets/rom/generated/96212fbb9ec0844ecbb7.inc.c"











};

// 0x070094F8 - 0x07009690
static const Gfx bits_seg7_dl_070094F8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_07009058, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSP2Triangles( 6,  8,  7, 0x0,  8,  9,  7, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0,  4, 11,  5, 0x0),
    gsSP2Triangles( 0,  3,  5, 0x0, 11,  0,  5, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_07009158, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11,  8, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_07009258, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  3,  5,  1, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_07009338, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 15, 12, 0x0),
    gsSPVertex(bits_seg7_vertex_07009438, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  3,  5, 0x0),
    gsSP2Triangles(10,  6,  8, 0x0,  0, 11,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07009690 - 0x07009700
const Gfx bits_seg7_dl_07009690[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bits_seg7_dl_070094F8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
