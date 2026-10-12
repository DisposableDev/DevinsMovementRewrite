// 0x0700E378 - 0x0700E390

// 0x0700E390 - 0x0700E3A8

// 0x0700E3A8 - 0x0700E3C0

// 0x0700E3C0 - 0x0700E4B0
static const Vtx ttm_seg7_vertex_0700E3C0[] = {
#include "assets/rom/generated/7d5778bd22d474535495.inc.c"














};

// 0x0700E4B0 - 0x0700E590
static const Vtx ttm_seg7_vertex_0700E4B0[] = {
#include "assets/rom/generated/fcd86bfb40444916748c.inc.c"













};

// 0x0700E590 - 0x0700E680
static const Vtx ttm_seg7_vertex_0700E590[] = {
#include "assets/rom/generated/3022c35b9f9fe9cab40c.inc.c"














};

// 0x0700E680 - 0x0700E6E0
static const Vtx ttm_seg7_vertex_0700E680[] = {
#include "assets/rom/generated/098a5f4f78b9ce77832f.inc.c"





};

// 0x0700E6E0 - 0x0700E720
static const Vtx ttm_seg7_vertex_0700E6E0[] = {
#include "assets/rom/generated/e8332b9187c267d4ef4e.inc.c"



};

// 0x0700E720 - 0x0700E820
static const Vtx ttm_seg7_vertex_0700E720[] = {
#include "assets/rom/generated/398e24af3acfb28ca221.inc.c"















};

// 0x0700E820 - 0x0700E8B0
static const Vtx ttm_seg7_vertex_0700E820[] = {
#include "assets/rom/generated/d074e44c68b3ea2759c8.inc.c"








};

// 0x0700E8B0 - 0x0700E910
static const Vtx ttm_seg7_vertex_0700E8B0[] = {
#include "assets/rom/generated/86687e9345a06b7aed28.inc.c"





};

// 0x0700E910 - 0x0700EA28
static const Gfx ttm_seg7_dl_0700E910[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ttm_seg7_vertex_0700E3C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700E4B0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  6,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700E590, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPLightColor(LIGHT_1, 0xbbbbbbff),
    gsSPLightColor(LIGHT_2, 0x5d5d5dff),
    gsSPVertex(ttm_seg7_vertex_0700E680, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ttm_seg7_vertex_0700E6E0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700EA28 - 0x0700EAC8
static const Gfx ttm_seg7_dl_0700EA28[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ttm_seg7_vertex_0700E720, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  4,  3,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700E820, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  7,  8, 0x0),
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(ttm_seg7_vertex_0700E8B0, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700EAC8 - 0x0700EB40
const Gfx ttm_seg7_dl_0700EAC8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ttm_seg7_dl_0700E910),
    gsSPDisplayList(ttm_seg7_dl_0700EA28),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
