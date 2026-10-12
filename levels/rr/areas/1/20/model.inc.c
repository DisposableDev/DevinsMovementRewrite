// 0x070175B0 - 0x070176A0
static const Vtx rr_seg7_vertex_070175B0[] = {
#include "assets/rom/generated/080b6ff50f7288668e4b.inc.c"














};

// 0x070176A0 - 0x07017790
static const Vtx rr_seg7_vertex_070176A0[] = {
#include "assets/rom/generated/d54a24eef00aa878dd96.inc.c"














};

// 0x07017790 - 0x07017880
static const Vtx rr_seg7_vertex_07017790[] = {
#include "assets/rom/generated/3c608df3cdd3c4eeb65c.inc.c"














};

// 0x07017880 - 0x07017980
static const Vtx rr_seg7_vertex_07017880[] = {
#include "assets/rom/generated/d7a6ec0442c23e609dff.inc.c"















};

// 0x07017980 - 0x07017A60
static const Vtx rr_seg7_vertex_07017980[] = {
#include "assets/rom/generated/038d5f40e97f745deb91.inc.c"













};

// 0x07017A60 - 0x07017B40
static const Vtx rr_seg7_vertex_07017A60[] = {
#include "assets/rom/generated/b8e74376bfacdeafc58f.inc.c"













};

// 0x07017B40 - 0x07017C20
static const Vtx rr_seg7_vertex_07017B40[] = {
#include "assets/rom/generated/31253c95f8451347deca.inc.c"













};

// 0x07017C20 - 0x07017D00
static const Vtx rr_seg7_vertex_07017C20[] = {
#include "assets/rom/generated/a2705a010c5ff678d512.inc.c"













};

// 0x07017D00 - 0x07017DE0
static const Vtx rr_seg7_vertex_07017D00[] = {
#include "assets/rom/generated/096a7c8dd6e6e2dde87f.inc.c"













};

// 0x07017DE0 - 0x07017EE0
static const Vtx rr_seg7_vertex_07017DE0[] = {
#include "assets/rom/generated/8a6022c545b8f8a59712.inc.c"















};

// 0x07017EE0 - 0x07017F90
static const Vtx rr_seg7_vertex_07017EE0[] = {
#include "assets/rom/generated/354fcb8c329f34c0097c.inc.c"










};

// 0x07017F90 - 0x07018010
static const Vtx rr_seg7_vertex_07017F90[] = {
#include "assets/rom/generated/31768a3d4817c9f55a78.inc.c"







};

// 0x07018010 - 0x07018100
static const Vtx rr_seg7_vertex_07018010[] = {
#include "assets/rom/generated/7a944cd17ca42e006ea0.inc.c"














};

// 0x07018100 - 0x070181F0
static const Vtx rr_seg7_vertex_07018100[] = {
#include "assets/rom/generated/71ab1eac9243a51cf13c.inc.c"














};

// 0x070181F0 - 0x070182F0
static const Vtx rr_seg7_vertex_070181F0[] = {
#include "assets/rom/generated/fb5fd36f81625a47e853.inc.c"















};

// 0x070182F0 - 0x070183E0
static const Vtx rr_seg7_vertex_070182F0[] = {
#include "assets/rom/generated/c93ea231a3fe97ea7ad3.inc.c"














};

// 0x070183E0 - 0x070184D0
static const Vtx rr_seg7_vertex_070183E0[] = {
#include "assets/rom/generated/fb32bb4880c423b843eb.inc.c"














};

// 0x070184D0 - 0x070185D0
static const Vtx rr_seg7_vertex_070184D0[] = {
#include "assets/rom/generated/b95129e015d657d7cf03.inc.c"















};

// 0x070185D0 - 0x07018650
static const Vtx rr_seg7_vertex_070185D0[] = {
#include "assets/rom/generated/cea39f7129c621eade1f.inc.c"







};

// 0x07018650 - 0x07018938
static const Gfx rr_seg7_dl_07018650[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09005800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_070175B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  7,  9, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(rr_seg7_vertex_070176A0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(rr_seg7_vertex_07017790, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14, 11, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07017880, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10,  2,  7, 0x0, 10,  7,  6, 0x0),
    gsSP2Triangles( 0,  2, 10, 0x0, 11,  1,  0, 0x0),
    gsSP2Triangles(11, 12,  1, 0x0, 13, 14, 12, 0x0),
    gsSP2Triangles(13, 12, 11, 0x0, 15, 14, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07017980, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 3,  4,  1, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 8,  4,  3, 0x0,  8,  9,  4, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(rr_seg7_vertex_07017A60, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07017B40, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07017C20, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07017D00, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07017DE0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  0, 0x0,  5,  3,  0, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  9,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_07017EE0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP1Triangle( 8,  9, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x07018938 - 0x07018980
static const Gfx rr_seg7_dl_07018938[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09006000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_07017F90, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x07018980 - 0x07018B58
static const Gfx rr_seg7_dl_07018980[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, rr_seg7_texture_07001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_07018010, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(12, 14, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07018100, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  9,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 11, 13, 12, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_070181F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_070182F0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(rr_seg7_vertex_070183E0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(rr_seg7_vertex_070184D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 1,  6,  2, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(12, 14, 13, 0x0,  3,  5, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_070185D0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x07018B58 - 0x07018C18
const Gfx rr_seg7_dl_07018B58[] = {
    gsDPPipeSync(),
    gsDPSetEnvColor(255, 255, 255, 100),
    gsDPSetCombineMode(G_CC_MODULATERGBFADEA, G_CC_MODULATERGBFADEA),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(rr_seg7_dl_07018650),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(rr_seg7_dl_07018938),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(rr_seg7_dl_07018980),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};
