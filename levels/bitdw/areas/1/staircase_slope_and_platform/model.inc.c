// 0x07009430 - 0x07009530
static const Vtx bitdw_seg7_vertex_07009430[] = {
#include "assets/rom/generated/c6c85b7cb8e1ba6fd22b.inc.c"















};

// 0x07009530 - 0x07009610
static const Vtx bitdw_seg7_vertex_07009530[] = {
#include "assets/rom/generated/b64b4ed2389d6fa20c86.inc.c"













};

// 0x07009610 - 0x07009690
static const Vtx bitdw_seg7_vertex_07009610[] = {
#include "assets/rom/generated/4c107e073930ab9a27c3.inc.c"







};

// 0x07009690 - 0x07009790
static const Vtx bitdw_seg7_vertex_07009690[] = {
#include "assets/rom/generated/a0f0266d707ccd1bb6d7.inc.c"















};

// 0x07009790 - 0x07009880
static const Vtx bitdw_seg7_vertex_07009790[] = {
#include "assets/rom/generated/f802e430323f8083ff9a.inc.c"














};

// 0x07009880 - 0x07009970
static const Vtx bitdw_seg7_vertex_07009880[] = {
#include "assets/rom/generated/09eac44aad63b9422e29.inc.c"














};

// 0x07009970 - 0x07009A70
static const Vtx bitdw_seg7_vertex_07009970[] = {
#include "assets/rom/generated/db2a0ee27b9925d3e6a7.inc.c"















};

// 0x07009A70 - 0x07009B60
static const Vtx bitdw_seg7_vertex_07009A70[] = {
#include "assets/rom/generated/3edd5d411089280c8fe5.inc.c"














};

// 0x07009B60 - 0x07009C50
static const Vtx bitdw_seg7_vertex_07009B60[] = {
#include "assets/rom/generated/68925e880f39ab5f3d78.inc.c"














};

// 0x07009C50 - 0x07009D50
static const Vtx bitdw_seg7_vertex_07009C50[] = {
#include "assets/rom/generated/a138fcb448c381ba02a4.inc.c"















};

// 0x07009D50 - 0x07009E30
static const Vtx bitdw_seg7_vertex_07009D50[] = {
#include "assets/rom/generated/8e676d8b9569f6e8906f.inc.c"













};

// 0x07009E30 - 0x07009E90
static const Vtx bitdw_seg7_vertex_07009E30[] = {
#include "assets/rom/generated/054135c29418f7ebf8d2.inc.c"





};

// 0x07009E90 - 0x07009F10
static const Vtx bitdw_seg7_vertex_07009E90[] = {
#include "assets/rom/generated/11968d6a786613bf37fd.inc.c"







};

// 0x07009F10 - 0x07009F90
static const Vtx bitdw_seg7_vertex_07009F10[] = {
#include "assets/rom/generated/575355563932c1e4801a.inc.c"







};

// 0x07009F90 - 0x0700A058
static const Gfx bitdw_seg7_dl_07009F90[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitdw_seg7_vertex_07009430, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  3,  0, 0x0,  4,  5,  3, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 11, 13, 0x0, 14, 15,  6, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07009530, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  7,  9, 13, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07009610, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700A058 - 0x0700A188
static const Gfx bitdw_seg7_dl_0700A058[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bitdw_seg7_texture_07001000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitdw_seg7_vertex_07009690, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0, 15,  1, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07009790, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(12, 14, 13, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07009880, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  4,  6, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07009970, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 13, 15, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700A188 - 0x0700A2B8
static const Gfx bitdw_seg7_dl_0700A188[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitdw_seg7_vertex_07009A70, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07009B60, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07009C50, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  0,  5, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 8,  9,  6, 0x0, 10, 11, 12, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07009D50, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07009E30, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700A2B8 - 0x0700A310
static const Gfx bitdw_seg7_dl_0700A2B8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09004800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitdw_seg7_vertex_07009E90, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  5,  6,  2, 0x0),
    gsSP2Triangles( 6,  3,  2, 0x0,  4,  7,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700A310 - 0x0700A368
static const Gfx bitdw_seg7_dl_0700A310[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitdw_seg7_vertex_07009F10, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  5,  6, 0x0,  0,  7,  1, 0x0),
    gsSP2Triangles( 0,  6,  7, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700A368 - 0x0700A3F8
const Gfx bitdw_seg7_dl_0700A368[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bitdw_seg7_dl_07009F90),
    gsSPDisplayList(bitdw_seg7_dl_0700A058),
    gsSPDisplayList(bitdw_seg7_dl_0700A188),
    gsSPDisplayList(bitdw_seg7_dl_0700A2B8),
    gsSPDisplayList(bitdw_seg7_dl_0700A310),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
