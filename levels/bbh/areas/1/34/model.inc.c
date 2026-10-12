// 0x0701A920 - 0x0701A938

// 0x0701A938 - 0x0701A950

// 0x0701A950 - 0x0701A968

// 0x0701A968 - 0x0701A9C8
static const Vtx bbh_seg7_vertex_0701A968[] = {
#include "assets/rom/generated/ead621d17ef03f7aa888.inc.c"





};

// 0x0701A9C8 - 0x0701AA08
static const Vtx bbh_seg7_vertex_0701A9C8[] = {
#include "assets/rom/generated/c1c2a005f4dde93482be.inc.c"



};

// 0x0701AA08 - 0x0701AB08
static const Vtx bbh_seg7_vertex_0701AA08[] = {
#include "assets/rom/generated/f47d660c886636cf17a5.inc.c"















};

// 0x0701AB08 - 0x0701ABE8
static const Vtx bbh_seg7_vertex_0701AB08[] = {
#include "assets/rom/generated/de549e6bf7e4bf48ecea.inc.c"













};

// 0x0701ABE8 - 0x0701ACD8
static const Vtx bbh_seg7_vertex_0701ABE8[] = {
#include "assets/rom/generated/1b2b333ee4bb127a9eb0.inc.c"














};

// 0x0701ACD8 - 0x0701ADD8
static const Vtx bbh_seg7_vertex_0701ACD8[] = {
#include "assets/rom/generated/3e424d44ca026fb99aea.inc.c"















};

// 0x0701ADD8 - 0x0701AED8
static const Vtx bbh_seg7_vertex_0701ADD8[] = {
#include "assets/rom/generated/edb4605a87fcf7b7ab53.inc.c"















};

// 0x0701AED8 - 0x0701AF48
static const Vtx bbh_seg7_vertex_0701AED8[] = {
#include "assets/rom/generated/88a4dd4d3555206cc11a.inc.c"






};

// 0x0701AF48 - 0x0701B048
static const Vtx bbh_seg7_vertex_0701AF48[] = {
#include "assets/rom/generated/15c0c5a73ee388a20f5b.inc.c"















};

// 0x0701B048 - 0x0701B148
static const Vtx bbh_seg7_vertex_0701B048[] = {
#include "assets/rom/generated/9ed2ebdaa739260babd6.inc.c"















};

// 0x0701B148 - 0x0701B1E8
static const Vtx bbh_seg7_vertex_0701B148[] = {
#include "assets/rom/generated/c047a93b64e7f5656d02.inc.c"









};

// 0x0701B1E8 - 0x0701B2A8
static const Vtx bbh_seg7_vertex_0701B1E8[] = {
#include "assets/rom/generated/7a619bc2fd39b3aabfac.inc.c"











};

// 0x0701B2A8 - 0x0701B328
static const Vtx bbh_seg7_vertex_0701B2A8[] = {
#include "assets/rom/generated/6fde75695b81dd79fb78.inc.c"







};

// 0x0701B328 - 0x0701B380
static const Gfx bbh_seg7_dl_0701B328[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09004800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x2a2c37ff),
    gsSPLightColor(LIGHT_2, 0x101116ff),
    gsSPVertex(bbh_seg7_vertex_0701A968, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  1,  5,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0701B380 - 0x0701B558
static const Gfx bbh_seg7_dl_0701B380[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09008800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x969696ff),
    gsSPLightColor(LIGHT_2, 0x3c3c3cff),
    gsSPVertex(bbh_seg7_vertex_0701A9C8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(bbh_seg7_vertex_0701AA08, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  2, 11,  0, 0x0),
    gsSP2Triangles( 0, 12,  1, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_0701AB08, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bbh_seg7_vertex_0701ABE8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9, 11, 12, 0x0),
    gsSP2Triangles( 9, 12,  7, 0x0,  7, 13,  8, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(bbh_seg7_vertex_0701ACD8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 8, 11,  9, 0x0,  8, 12, 11, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_0701ADD8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 4, 10,  3, 0x0, 11, 10,  4, 0x0),
    gsSP2Triangles(11, 12, 10, 0x0, 11,  4,  6, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_0701AED8, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 3,  5,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0701B558 - 0x0701B620
static const Gfx bbh_seg7_dl_0701B558[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_0701AF48, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  3,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_0701B048, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 12, 10, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_0701B148, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0701B620 - 0x0701B678
static const Gfx bbh_seg7_dl_0701B620[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_0701B1E8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0701B678 - 0x0701B6D0
static const Gfx bbh_seg7_dl_0701B678[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09009000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_0701B2A8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 2,  6,  3, 0x0,  2,  7,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0701B6D0 - 0x0701B7A8
const Gfx bbh_seg7_dl_0701B6D0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_0701B328),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_0701B380),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_0701B558),
    gsSPDisplayList(bbh_seg7_dl_0701B620),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_0701B678),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
