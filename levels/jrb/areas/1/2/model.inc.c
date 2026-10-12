// 0x070030A8 - 0x070030C0

// 0x070030C0 - 0x070030D8

// 0x070030D8 - 0x070030F0

// 0x070030F0 - 0x070031F0
static const Vtx jrb_seg7_vertex_070030F0[] = {
#include "assets/rom/generated/16c82f10ad0bbde3d506.inc.c"















};

// 0x070031F0 - 0x07003250
static const Vtx jrb_seg7_vertex_070031F0[] = {
#include "assets/rom/generated/4391ba8fd8d1c0ac4adf.inc.c"





};

// 0x07003250 - 0x07003350
static const Vtx jrb_seg7_vertex_07003250[] = {
#include "assets/rom/generated/58df69ae34b436f8df4f.inc.c"















};

// 0x07003350 - 0x07003450
static const Vtx jrb_seg7_vertex_07003350[] = {
#include "assets/rom/generated/15d138fb66d4de94c661.inc.c"















};

// 0x07003450 - 0x07003540
static const Vtx jrb_seg7_vertex_07003450[] = {
#include "assets/rom/generated/5a5fd7162fb7d7b2e75a.inc.c"














};

// 0x07003540 - 0x07003640
static const Vtx jrb_seg7_vertex_07003540[] = {
#include "assets/rom/generated/b73999d23324b9a6e487.inc.c"















};

// 0x07003640 - 0x07003700
static const Vtx jrb_seg7_vertex_07003640[] = {
#include "assets/rom/generated/226f823506efecb63615.inc.c"











};

// 0x07003700 - 0x07003800
static const Vtx jrb_seg7_vertex_07003700[] = {
#include "assets/rom/generated/df768c199603f6373685.inc.c"















};

// 0x07003800 - 0x070038F0
static const Vtx jrb_seg7_vertex_07003800[] = {
#include "assets/rom/generated/63dbcd8cd0471de6ac78.inc.c"














};

// 0x070038F0 - 0x070039E0
static const Vtx jrb_seg7_vertex_070038F0[] = {
#include "assets/rom/generated/eb144a769d8c631f09d9.inc.c"














};

// 0x070039E0 - 0x07003A90
static const Vtx jrb_seg7_vertex_070039E0[] = {
#include "assets/rom/generated/3552c72bf11ba9259ed0.inc.c"










};

// 0x07003A90 - 0x07003B10
static const Vtx jrb_seg7_vertex_07003A90[] = {
#include "assets/rom/generated/ae0baa7024a21e876891.inc.c"







};

// 0x07003B10 - 0x07003C10
static const Vtx jrb_seg7_vertex_07003B10[] = {
#include "assets/rom/generated/2f08e8abe0e3363969ec.inc.c"















};

// 0x07003C10 - 0x07003D10
static const Vtx jrb_seg7_vertex_07003C10[] = {
#include "assets/rom/generated/ce97b9cf8b57c9704f17.inc.c"















};

// 0x07003D10 - 0x07003E00
static const Vtx jrb_seg7_vertex_07003D10[] = {
#include "assets/rom/generated/585938002113920bad40.inc.c"














};

// 0x07003E00 - 0x07003EF0
static const Vtx jrb_seg7_vertex_07003E00[] = {
#include "assets/rom/generated/7d68abb1038c804f4b1f.inc.c"














};

// 0x07003EF0 - 0x07003FD0
static const Vtx jrb_seg7_vertex_07003EF0[] = {
#include "assets/rom/generated/bcdbce2c50507d1214e5.inc.c"













};

// 0x07003FD0 - 0x070040C0
static const Vtx jrb_seg7_vertex_07003FD0[] = {
#include "assets/rom/generated/e02df7fe07be6a6cabea.inc.c"














};

// 0x070040C0 - 0x070041C0
static const Vtx jrb_seg7_vertex_070040C0[] = {
#include "assets/rom/generated/3e2ff0355de09b596653.inc.c"















};

// 0x070041C0 - 0x070041F0
static const Vtx jrb_seg7_vertex_070041C0[] = {
#include "assets/rom/generated/476fd000b39dfaf0e7b6.inc.c"


};

// 0x070041F0 - 0x070044C8
static const Gfx jrb_seg7_dl_070041F0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x419678ff),
    gsSPLightColor(LIGHT_2, 0x10251eff),
    gsSPVertex(jrb_seg7_vertex_070030F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 5,  3,  2, 0x0,  4,  3,  6, 0x0),
    gsSP2Triangles( 7,  3,  8, 0x0,  6,  3,  9, 0x0),
    gsSP2Triangles( 8,  3,  5, 0x0,  9,  3, 10, 0x0),
    gsSP2Triangles( 3,  7, 10, 0x0, 11,  0,  4, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0, 12, 10,  7, 0x0),
    gsSP2Triangles(13,  5,  1, 0x0,  5,  2,  1, 0x0),
    gsSP2Triangles(11,  4,  6, 0x0, 14, 11,  6, 0x0),
    gsSP2Triangles(14,  6,  9, 0x0, 15, 14,  9, 0x0),
    gsSP2Triangles(15,  9, 10, 0x0, 12, 15, 10, 0x0),
    gsSPVertex(jrb_seg7_vertex_070031F0, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  0,  3, 0x0,  4,  3,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(jrb_seg7_vertex_07003250, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  2,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9, 10, 0x0),
    gsSP2Triangles( 6,  8, 11, 0x0,  6, 11, 12, 0x0),
    gsSP2Triangles( 6, 12, 13, 0x0,  6, 10,  7, 0x0),
    gsSP1Triangle( 8, 14, 15, 0x0),
    gsSPVertex(jrb_seg7_vertex_07003350, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  0,  3, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 0,  6,  5, 0x0,  7,  3,  2, 0x0),
    gsSP2Triangles( 4,  3,  8, 0x0,  7,  8,  3, 0x0),
    gsSP2Triangles( 9,  6, 10, 0x0,  9,  5,  6, 0x0),
    gsSP2Triangles(10, 11,  9, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(14, 12, 15, 0x0, 12, 13, 15, 0x0),
    gsSPVertex(jrb_seg7_vertex_07003450, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  5,  4, 0x0,  6,  4,  7, 0x0),
    gsSP2Triangles( 8,  3,  9, 0x0,  3,  5,  9, 0x0),
    gsSP2Triangles(10, 11,  0, 0x0, 12,  0, 11, 0x0),
    gsSP2Triangles(13,  1,  0, 0x0, 14, 13,  0, 0x0),
    gsSP1Triangle(12, 14,  0, 0x0),
    gsSPVertex(jrb_seg7_vertex_07003540, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  6,  7,  4, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles( 0,  2, 12, 0x0, 13,  0, 12, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(jrb_seg7_vertex_07003640, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 1,  5,  3, 0x0,  0,  2,  6, 0x0),
    gsSP2Triangles( 0,  7,  8, 0x0,  0,  6,  7, 0x0),
    gsSP1Triangle( 9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x070044C8 - 0x07004658
static const Gfx jrb_seg7_dl_070044C8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x799febff),
    gsSPLightColor(LIGHT_2, 0x1e273aff),
    gsSPVertex(jrb_seg7_vertex_07003700, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  5,  4, 0x0),
    gsSP2Triangles( 7,  2,  5, 0x0,  6,  3,  0, 0x0),
    gsSP2Triangles( 3,  1,  0, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 0,  2,  7, 0x0, 11, 12,  8, 0x0),
    gsSP2Triangles(12, 11, 13, 0x0, 12, 13, 14, 0x0),
    gsSP2Triangles(12, 15,  8, 0x0, 10, 14, 13, 0x0),
    gsSP2Triangles(10,  9, 14, 0x0,  8, 15,  9, 0x0),
    gsSPVertex(jrb_seg7_vertex_07003800, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  7,  6, 0x0,  8,  9,  7, 0x0),
    gsSP2Triangles( 3,  5, 10, 0x0, 10,  5,  8, 0x0),
    gsSP2Triangles( 5,  9,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(jrb_seg7_vertex_070038F0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11,  8, 12, 0x0),
    gsSP2Triangles(13,  4,  6, 0x0, 13,  6, 14, 0x0),
    gsSPVertex(jrb_seg7_vertex_070039E0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP1Triangle( 8,  9, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x07004658 - 0x070046C0
static const Gfx jrb_seg7_dl_07004658[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09002800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(jrb_seg7_vertex_07003A90, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 1,  4,  3, 0x0,  4,  5,  3, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  6,  0,  7, 0x0),
    gsSP2Triangles( 6,  7,  5, 0x0,  0,  2,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x070046C0 - 0x07004940
static const Gfx jrb_seg7_dl_070046C0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09004800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(jrb_seg7_vertex_07003B10, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  0,  3, 0x0,  1,  5,  2, 0x0),
    gsSP2Triangles( 5,  6,  2, 0x0,  2,  7,  3, 0x0),
    gsSP2Triangles( 2,  8,  9, 0x0,  2,  9,  7, 0x0),
    gsSP2Triangles(10,  8,  2, 0x0, 10,  2, 11, 0x0),
    gsSP2Triangles(11,  2,  6, 0x0, 12,  4,  3, 0x0),
    gsSP2Triangles(12,  3, 13, 0x0,  3,  7, 14, 0x0),
    gsSP1Triangle(15, 12, 13, 0x0),
    gsSPVertex(jrb_seg7_vertex_07003C10, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  5,  4,  2, 0x0),
    gsSP2Triangles( 6,  2,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 9,  8, 11, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle( 9, 11, 15, 0x0),
    gsSPVertex(jrb_seg7_vertex_07003D10, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 3,  4,  1, 0x0,  1,  4,  5, 0x0),
    gsSP2Triangles( 1,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 10, 12, 0x0),
    gsSP1Triangle(10, 14, 11, 0x0),
    gsSPVertex(jrb_seg7_vertex_07003E00, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  3, 0x0,  4,  6,  5, 0x0),
    gsSP2Triangles( 4,  7,  8, 0x0,  4,  8,  6, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(jrb_seg7_vertex_07003EF0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle( 6, 12, 13, 0x0),
    gsSPVertex(jrb_seg7_vertex_07003FD0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 2,  4,  3, 0x0,  2,  5,  4, 0x0),
    gsSP2Triangles( 6,  3,  4, 0x0,  6,  4,  7, 0x0),
    gsSP2Triangles( 3,  8,  0, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(jrb_seg7_vertex_070040C0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 3,  8,  4, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(jrb_seg7_vertex_070041C0, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x07004940 - 0x07004A18
const Gfx jrb_seg7_dl_07004940[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_070041F0),
    gsSPDisplayList(jrb_seg7_dl_070044C8),
    gsSPDisplayList(jrb_seg7_dl_07004658),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_070046C0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
