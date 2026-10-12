// 0x07011F58 - 0x07011F70

// 0x07011F70 - 0x07011F88

// 0x07011F88 - 0x07011FA0

// 0x07011FA0 - 0x07012020
static const Vtx wdw_seg7_vertex_07011FA0[] = {
#include "assets/rom/generated/1bf6603db01266c6b58a.inc.c"







};

// 0x07012020 - 0x070120A0
static const Vtx wdw_seg7_vertex_07012020[] = {
#include "assets/rom/generated/186ac1fc02e06acdd79f.inc.c"







};

// 0x070120A0 - 0x07012160
static const Vtx wdw_seg7_vertex_070120A0[] = {
#include "assets/rom/generated/29e5f4fe7269ae242248.inc.c"











};

// 0x07012160 - 0x07012258
static const Gfx wdw_seg7_dl_07012160[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09000000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x8df0e3ff),
    gsSPLightColor(LIGHT_2, 0x549088ff),
    gsSPVertex(wdw_seg7_vertex_07011FA0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  0,  5,  6, 0x0),
    gsSP2Triangles( 0,  6,  7, 0x0,  0,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0x9092eeff),
    gsSPLightColor(LIGHT_2, 0x56578eff),
    gsSPVertex(wdw_seg7_vertex_07012020, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  5,  6, 0x0,  0,  6,  7, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x999999ff),
    gsSPVertex(wdw_seg7_vertex_070120A0, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  0,  7,  1, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x07012258 - 0x070122C8
const Gfx wdw_seg7_dl_07012258[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(wdw_seg7_dl_07012160),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
