// 0x0700EA00 - 0x0700EA18

// 0x0700EA18 - 0x0700EA30

// 0x0700EA30 - 0x0700EA48

// 0x0700EA48 - 0x0700EAE8
static const Vtx ccm_seg7_vertex_0700EA48[] = {
#include "assets/rom/generated/32cb614c938aee7dd854.inc.c"









};

// 0x0700EAE8 - 0x0700EBD8
static const Vtx ccm_seg7_vertex_0700EAE8[] = {
#include "assets/rom/generated/c65e8e9fe772bbffb20c.inc.c"














};

// 0x0700EBD8 - 0x0700ECB8
static const Vtx ccm_seg7_vertex_0700EBD8[] = {
#include "assets/rom/generated/6bf76e6954ea57fe746b.inc.c"













};

// 0x0700ECB8 - 0x0700ED98
static const Vtx ccm_seg7_vertex_0700ECB8[] = {
#include "assets/rom/generated/4ac0a20cf92438b00c6c.inc.c"













};

// 0x0700ED98 - 0x0700EE38
static const Vtx ccm_seg7_vertex_0700ED98[] = {
#include "assets/rom/generated/03497ba7503160988017.inc.c"









};

// 0x0700EE38 - 0x0700EF38
static const Vtx ccm_seg7_vertex_0700EE38[] = {
#include "assets/rom/generated/1e3cf8d47a1e9fb1a4f1.inc.c"















};

// 0x0700EF38 - 0x0700EF98
static const Vtx ccm_seg7_vertex_0700EF38[] = {
#include "assets/rom/generated/8dfc3cb308dcb400fdca.inc.c"





};

// 0x0700EF98 - 0x0700F058
static const Vtx ccm_seg7_vertex_0700EF98[] = {
#include "assets/rom/generated/e9359d27c1445f8d9c4b.inc.c"











};

// 0x0700F058 - 0x0700F148
static const Vtx ccm_seg7_vertex_0700F058[] = {
#include "assets/rom/generated/94efe6a9f6c1b4132f43.inc.c"














};

// 0x0700F148 - 0x0700F1B8
static const Vtx ccm_seg7_vertex_0700F148[] = {
#include "assets/rom/generated/111e7f906adbdefdf16a.inc.c"






};

// 0x0700F1B8 - 0x0700F210
static const Gfx ccm_seg7_dl_0700F1B8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09008800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ccm_seg7_vertex_0700EA48, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700F210 - 0x0700F3B0
static const Gfx ccm_seg7_dl_0700F210[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ccm_seg7_vertex_0700EAE8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(12,  2, 13, 0x0,  2, 14, 13, 0x0),
    gsSP1Triangle(14,  2,  1, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700EBD8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  9,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  8,  9, 13, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700ECB8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 11, 13, 12, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700ED98, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSPLightColor(LIGHT_1, 0x666666ff),
    gsSPLightColor(LIGHT_2, 0x333333ff),
    gsSPVertex(ccm_seg7_vertex_0700EE38, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  9,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700EF38, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xbbbbbbff),
    gsSPLightColor(LIGHT_2, 0x5d5d5dff),
    gsSPVertex(ccm_seg7_vertex_0700EF98, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700F3B0 - 0x0700F440
static const Gfx ccm_seg7_dl_0700F3B0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09008000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x666666ff),
    gsSPLightColor(LIGHT_2, 0x333333ff),
    gsSPVertex(ccm_seg7_vertex_0700F058, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ccm_seg7_vertex_0700F148, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  6,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700F440 - 0x0700F4C0
const Gfx ccm_seg7_dl_0700F440[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ccm_seg7_dl_0700F1B8),
    gsSPDisplayList(ccm_seg7_dl_0700F210),
    gsSPDisplayList(ccm_seg7_dl_0700F3B0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
