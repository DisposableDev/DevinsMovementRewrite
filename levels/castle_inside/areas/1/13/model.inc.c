// 0x070318A0 - 0x070318B8

// 0x070318B8 - 0x070318D0

// 0x070318D0 - 0x070319D0
static const Vtx inside_castle_seg7_vertex_070318D0[] = {
#include "assets/rom/generated/d7e8594af08e845a0bd7.inc.c"















};

// 0x070319D0 - 0x07031AB0
static const Vtx inside_castle_seg7_vertex_070319D0[] = {
#include "assets/rom/generated/55e462d4fd11dbf13546.inc.c"













};

// 0x07031AB0 - 0x07031B70
static const Vtx inside_castle_seg7_vertex_07031AB0[] = {
#include "assets/rom/generated/1a558e867df321997058.inc.c"











};

// 0x07031B70 - 0x07031C70
static const Vtx inside_castle_seg7_vertex_07031B70[] = {
#include "assets/rom/generated/a460ae226b2621e4fe81.inc.c"















};

// 0x07031C70 - 0x07031D70
static const Vtx inside_castle_seg7_vertex_07031C70[] = {
#include "assets/rom/generated/67c3c9c29445b5bb66d0.inc.c"















};

// 0x07031D70 - 0x07031E60
static const Vtx inside_castle_seg7_vertex_07031D70[] = {
#include "assets/rom/generated/87ca9d28783a6cf289db.inc.c"














};

// 0x07031E60 - 0x07031F10
static const Vtx inside_castle_seg7_vertex_07031E60[] = {
#include "assets/rom/generated/26e0caa36164343db6e0.inc.c"










};

// 0x07031F10 - 0x07032000
static const Vtx inside_castle_seg7_vertex_07031F10[] = {
#include "assets/rom/generated/3c1ed2d893998de58951.inc.c"














};

// 0x07032000 - 0x070320F0
static const Vtx inside_castle_seg7_vertex_07032000[] = {
#include "assets/rom/generated/fe48e267960f18f3beb5.inc.c"














};

// 0x070320F0 - 0x070321F0
static const Vtx inside_castle_seg7_vertex_070320F0[] = {
#include "assets/rom/generated/a15dff2b775c7bfafd8f.inc.c"















};

// 0x070321F0 - 0x070322F0
static const Vtx inside_castle_seg7_vertex_070321F0[] = {
#include "assets/rom/generated/eb235cd5b788ba6f1c5d.inc.c"















};

// 0x070322F0 - 0x070323F0
static const Vtx inside_castle_seg7_vertex_070322F0[] = {
#include "assets/rom/generated/71039db2aefcb0ecd6ef.inc.c"















};

// 0x070323F0 - 0x07032430
static const Vtx inside_castle_seg7_vertex_070323F0[] = {
#include "assets/rom/generated/c4438d025ebfed12838e.inc.c"



};

// 0x07032430 - 0x07032530
static const Vtx inside_castle_seg7_vertex_07032430[] = {
#include "assets/rom/generated/3887931565f5a368bbd4.inc.c"















};

// 0x07032530 - 0x07032630
static const Vtx inside_castle_seg7_vertex_07032530[] = {
#include "assets/rom/generated/8cf87bd94059f4fd0e68.inc.c"















};

// 0x07032630 - 0x070326E0
static const Vtx inside_castle_seg7_vertex_07032630[] = {
#include "assets/rom/generated/212626806879ff817ef6.inc.c"










};

// 0x070326E0 - 0x070327E0
static const Vtx inside_castle_seg7_vertex_070326E0[] = {
#include "assets/rom/generated/da6e89ad53c70cdfb189.inc.c"















};

// 0x070327E0 - 0x070328D0
static const Vtx inside_castle_seg7_vertex_070327E0[] = {
#include "assets/rom/generated/438bc9e5caad3bf0cdd6.inc.c"














};

// 0x070328D0 - 0x070329C0
static const Vtx inside_castle_seg7_vertex_070328D0[] = {
#include "assets/rom/generated/921a4424dccec1b6665e.inc.c"














};

// 0x070329C0 - 0x07032A00
static const Vtx inside_castle_seg7_vertex_070329C0[] = {
#include "assets/rom/generated/52945e7f4dc6371cdfc4.inc.c"



};

// 0x07032A00 - 0x07032B18
static const Gfx inside_castle_seg7_dl_07032A00[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xddddddff),
    gsSPLightColor(LIGHT_2, 0x525252ff),
    gsSPVertex(inside_castle_seg7_vertex_070318D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0,  2, 15, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x5f5f5fff),
    gsSPVertex(inside_castle_seg7_vertex_070319D0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  5,  3,  4, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 8,  5,  7, 0x0,  9,  8,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07031AB0, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x07032B18 - 0x07032C18
static const Gfx inside_castle_seg7_dl_07032B18[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07031B70, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  7, 10, 11, 0x0),
    gsSP2Triangles( 7, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07031C70, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07031D70, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07031E60, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP1Triangle( 7, 10,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x07032C18 - 0x07032DC8
static const Gfx inside_castle_seg7_dl_07032C18[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07031F10, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  3, 0x0,  3,  7,  8, 0x0),
    gsSP2Triangles( 3,  8,  4, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 12, 13, 0x0,  9, 11, 14, 0x0),
    gsSP1Triangle( 9, 14, 12, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07032000, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  7, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10, 11,  8, 0x0),
    gsSP2Triangles( 8, 12,  9, 0x0,  8, 13, 12, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070320F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  5,  4, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070321F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0,  6,  5, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070322F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070323F0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07032DC8 - 0x07032EC0
static const Gfx inside_castle_seg7_dl_07032DC8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_0900A000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07032430, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07032530, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  9, 11, 12, 0x0),
    gsSP2Triangles(10,  9, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07032630, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 8,  3,  9, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 6, 10,  7, 0x0,  3,  5,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x07032EC0 - 0x07032FC0
static const Gfx inside_castle_seg7_dl_07032EC0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_070326E0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070327E0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070328D0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070329C0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07032FC0 - 0x07033068
const Gfx inside_castle_seg7_dl_07032FC0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07032A00),
    gsSPDisplayList(inside_castle_seg7_dl_07032B18),
    gsSPDisplayList(inside_castle_seg7_dl_07032C18),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07032DC8),
    gsSPDisplayList(inside_castle_seg7_dl_07032EC0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
