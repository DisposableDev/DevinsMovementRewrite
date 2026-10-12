// 0x07018270 - 0x07018360
static const Vtx hmc_seg7_vertex_07018270[] = {
#include "assets/rom/generated/41e55eef820446bcbd4f.inc.c"














};

// 0x07018360 - 0x07018450
static const Vtx hmc_seg7_vertex_07018360[] = {
#include "assets/rom/generated/4a56092e030685adf96b.inc.c"














};

// 0x07018450 - 0x07018550
static const Vtx hmc_seg7_vertex_07018450[] = {
#include "assets/rom/generated/cdc29376b9435a2e5d54.inc.c"















};

// 0x07018550 - 0x07018650
static const Vtx hmc_seg7_vertex_07018550[] = {
#include "assets/rom/generated/dca4f38372032963586e.inc.c"















};

// 0x07018650 - 0x07018750
static const Vtx hmc_seg7_vertex_07018650[] = {
#include "assets/rom/generated/23191c5c6bac78fed8a7.inc.c"















};

// 0x07018750 - 0x07018850
static const Vtx hmc_seg7_vertex_07018750[] = {
#include "assets/rom/generated/8fe3187ba772a4f766b6.inc.c"















};

// 0x07018850 - 0x07018930
static const Vtx hmc_seg7_vertex_07018850[] = {
#include "assets/rom/generated/647f8745273896162bb6.inc.c"













};

// 0x07018930 - 0x07018A20
static const Vtx hmc_seg7_vertex_07018930[] = {
#include "assets/rom/generated/bfe8bd267ebcddcc9a68.inc.c"














};

// 0x07018A20 - 0x07018B20
static const Vtx hmc_seg7_vertex_07018A20[] = {
#include "assets/rom/generated/f2314feef3b16571042f.inc.c"















};

// 0x07018B20 - 0x07018C00
static const Vtx hmc_seg7_vertex_07018B20[] = {
#include "assets/rom/generated/5b834b54467b44d3a948.inc.c"













};

// 0x07018C00 - 0x07018D00
static const Vtx hmc_seg7_vertex_07018C00[] = {
#include "assets/rom/generated/f9c0866db0de2c5ab4e7.inc.c"















};

// 0x07018D00 - 0x07018E00
static const Vtx hmc_seg7_vertex_07018D00[] = {
#include "assets/rom/generated/307675015c712d26f301.inc.c"















};

// 0x07018E00 - 0x07018E80
static const Vtx hmc_seg7_vertex_07018E00[] = {
#include "assets/rom/generated/3860cae97d76e9934b16.inc.c"







};

// 0x07018E80 - 0x07019248
static const Gfx hmc_seg7_dl_07018E80[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, cave_0900A000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(hmc_seg7_vertex_07018270, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  6,  5, 0x0),
    gsSP2Triangles( 7,  5,  8, 0x0,  9,  1,  0, 0x0),
    gsSP2Triangles( 9, 10,  1, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(hmc_seg7_vertex_07018360, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  3,  0, 0x0,  4,  5,  3, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12,  7, 13, 0x0, 12, 14,  7, 0x0),
    gsSPVertex(hmc_seg7_vertex_07018450, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  2,  5, 0x0,  4,  3,  2, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(11, 10, 12, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14,  7, 15, 0x0),
    gsSPVertex(hmc_seg7_vertex_07018550, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 8, 10, 11, 0x0, 11, 10, 12, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 14,  6,  5, 0x0),
    gsSP1Triangle(14, 15,  6, 0x0),
    gsSPVertex(hmc_seg7_vertex_07018650, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles( 8,  7, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 0, 12,  1, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(hmc_seg7_vertex_07018750, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9,  7, 0x0,  8,  7,  4, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(hmc_seg7_vertex_07018850, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 6,  5,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(10, 12, 13, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(hmc_seg7_vertex_07018930, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  5,  4, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hmc_seg7_vertex_07018A20, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 1,  6,  2, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  3, 15,  4, 0x0),
    gsSPVertex(hmc_seg7_vertex_07018B20, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12,  1, 0x0),
    gsSP2Triangles(11,  1,  0, 0x0,  0,  2, 13, 0x0),
    gsSPVertex(hmc_seg7_vertex_07018C00, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 12, 0x0),
    gsSP2Triangles( 9, 12, 10, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(hmc_seg7_vertex_07018D00, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 3,  5,  4, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle(12, 15, 13, 0x0),
    gsSPVertex(hmc_seg7_vertex_07018E00, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  2,  5, 0x0,  4,  5,  6, 0x0),
    gsSP1Triangle( 7,  2,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07019248 - 0x070192B8
const Gfx hmc_seg7_dl_07019248[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(hmc_seg7_dl_07018E80),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};
