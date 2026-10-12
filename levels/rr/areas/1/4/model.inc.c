// 0x07002EF8 - 0x07002FD8
static const Vtx rr_seg7_vertex_07002EF8[] = {
#include "assets/rom/generated/98da730c770719ca65ac.inc.c"













};

// 0x07002FD8 - 0x070030D8
static const Vtx rr_seg7_vertex_07002FD8[] = {
#include "assets/rom/generated/4f28853ec380f9c2ae85.inc.c"















};

// 0x070030D8 - 0x070031D8
static const Vtx rr_seg7_vertex_070030D8[] = {
#include "assets/rom/generated/656c10abc78a619ed188.inc.c"















};

// 0x070031D8 - 0x070032B8
static const Vtx rr_seg7_vertex_070031D8[] = {
#include "assets/rom/generated/563ac2ebe5daa82da540.inc.c"













};

// 0x070032B8 - 0x070033A8
static const Vtx rr_seg7_vertex_070032B8[] = {
#include "assets/rom/generated/52d4fcd10de7cd337f60.inc.c"














};

// 0x070033A8 - 0x07003488
static const Vtx rr_seg7_vertex_070033A8[] = {
#include "assets/rom/generated/13e9cab8862b385b6dca.inc.c"













};

// 0x07003488 - 0x07003568
static const Vtx rr_seg7_vertex_07003488[] = {
#include "assets/rom/generated/a06df2c46c3377ee02d5.inc.c"













};

// 0x07003568 - 0x07003668
static const Vtx rr_seg7_vertex_07003568[] = {
#include "assets/rom/generated/9ada3ffc4e1346ecc69e.inc.c"















};

// 0x07003668 - 0x07003758
static const Vtx rr_seg7_vertex_07003668[] = {
#include "assets/rom/generated/abbe5d10146597600f6c.inc.c"














};

// 0x07003758 - 0x07003858
static const Vtx rr_seg7_vertex_07003758[] = {
#include "assets/rom/generated/bb1799f31f329dc5536f.inc.c"















};

// 0x07003858 - 0x07003948
static const Vtx rr_seg7_vertex_07003858[] = {
#include "assets/rom/generated/2f413a65ee08e9e6ec43.inc.c"














};

// 0x07003948 - 0x07003A28
static const Vtx rr_seg7_vertex_07003948[] = {
#include "assets/rom/generated/b2f9822cd100bf0b5fc6.inc.c"













};

// 0x07003A28 - 0x07003B08
static const Vtx rr_seg7_vertex_07003A28[] = {
#include "assets/rom/generated/5f08670fa53a2967e995.inc.c"













};

// 0x07003B08 - 0x07003BF8
static const Vtx rr_seg7_vertex_07003B08[] = {
#include "assets/rom/generated/3b88ed52a72500175e04.inc.c"














};

// 0x07003BF8 - 0x07003CF8
static const Vtx rr_seg7_vertex_07003BF8[] = {
#include "assets/rom/generated/6102b207d84dfa4a6aa3.inc.c"















};

// 0x07003CF8 - 0x07003DE8
static const Vtx rr_seg7_vertex_07003CF8[] = {
#include "assets/rom/generated/235fbb961d4a1ac17e60.inc.c"














};

// 0x07003DE8 - 0x07003EE8
static const Vtx rr_seg7_vertex_07003DE8[] = {
#include "assets/rom/generated/fb7ef7a2cc7fa546c72d.inc.c"















};

// 0x07003EE8 - 0x07003FE8
static const Vtx rr_seg7_vertex_07003EE8[] = {
#include "assets/rom/generated/a1285c258e8cc33f3e0b.inc.c"















};

// 0x07003FE8 - 0x07004058
static const Vtx rr_seg7_vertex_07003FE8[] = {
#include "assets/rom/generated/664945d211b0f3619b1d.inc.c"






};

// 0x07004058 - 0x07004148
static const Vtx rr_seg7_vertex_07004058[] = {
#include "assets/rom/generated/cc38fc99d7523368597d.inc.c"














};

// 0x07004148 - 0x07004248
static const Vtx rr_seg7_vertex_07004148[] = {
#include "assets/rom/generated/efe5d1b139c108e3effc.inc.c"















};

// 0x07004248 - 0x07004338
static const Vtx rr_seg7_vertex_07004248[] = {
#include "assets/rom/generated/9a8f630411ccba5e393a.inc.c"














};

// 0x07004338 - 0x07004428
static const Vtx rr_seg7_vertex_07004338[] = {
#include "assets/rom/generated/3b396b7206a626f207e5.inc.c"














};

// 0x07004428 - 0x07004468
static const Vtx rr_seg7_vertex_07004428[] = {
#include "assets/rom/generated/9cbcfa76155fb543be79.inc.c"



};

// 0x07004468 - 0x07004950
static const Gfx rr_seg7_dl_07004468[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_07002EF8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07002FD8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_070030D8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 15, 12, 0x0),
    gsSPVertex(rr_seg7_vertex_070031D8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(rr_seg7_vertex_070032B8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_070033A8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07003488, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07003568, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 0,  8,  9, 0x0,  0,  9,  6, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  4, 13,  2, 0x0),
    gsSP1Triangle(12, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_07003668, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  0, 0x0),
    gsSP2Triangles( 5,  9,  2, 0x0,  9, 10,  2, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 11, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07003758, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9,  4, 0x0,  4,  6,  8, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0,  8, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_07003858, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07003948, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 10, 12, 0x0),
    gsSPVertex(rr_seg7_vertex_07003A28, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07003B08, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07003BF8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0, 15,  1, 0x0),
    gsSPVertex(rr_seg7_vertex_07003CF8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  3,  5,  7, 0x0),
    gsSP2Triangles( 3,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 12, 13, 0x0,  9, 13, 10, 0x0),
    gsSP1Triangle( 9, 11, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07003DE8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  3,  7,  6, 0x0),
    gsSP2Triangles( 0,  2,  8, 0x0,  0,  8,  9, 0x0),
    gsSP2Triangles( 0,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14, 13, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_07003EE8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 8, 11,  9, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle(12, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_07003FE8, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP1Triangle( 4,  5,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x07004950 - 0x07004A98
static const Gfx rr_seg7_dl_07004950[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_07004058, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(rr_seg7_vertex_07004148, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  3,  7,  8, 0x0),
    gsSP2Triangles( 4,  9,  5, 0x0,  4,  6, 10, 0x0),
    gsSP2Triangles( 4, 10, 11, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle(12, 15, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07004248, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07004338, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(rr_seg7_vertex_07004428, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07004A98 - 0x07004B10
const Gfx rr_seg7_dl_07004A98[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(rr_seg7_dl_07004468),
    gsSPDisplayList(rr_seg7_dl_07004950),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
