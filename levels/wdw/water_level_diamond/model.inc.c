// 0x07012F18 - 0x07012F30

// 0x07012F30 - 0x07012F48

// 0x07012F48 - 0x07012F60

// 0x07012F60 - 0x07012F78

// 0x07012F78 - 0x07012FD8
static const Vtx wdw_seg7_vertex_07012F78[] = {
#include "assets/rom/generated/06e2859cb4eff4fc5f7b.inc.c"





};

// 0x07012FD8 - 0x07013038
static const Vtx wdw_seg7_vertex_07012FD8[] = {
#include "assets/rom/generated/2c6d054fab52885de564.inc.c"





};

// 0x07013038 - 0x07013098
static const Vtx wdw_seg7_vertex_07013038[] = {
#include "assets/rom/generated/815f9fe79a8e5773511f.inc.c"





};

// 0x07013098 - 0x070130F8
static const Vtx wdw_seg7_vertex_07013098[] = {
#include "assets/rom/generated/3d526b4d95fd2715b403.inc.c"





};

// 0x070130F8 - 0x070131B8
static const Gfx wdw_seg7_dl_070130F8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, wdw_seg7_texture_07001000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x999900ff),
    gsSPVertex(wdw_seg7_vertex_07012F78, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xffff),
    gsSPLightColor(LIGHT_2, 0x99ff),
    gsSPVertex(wdw_seg7_vertex_07012FD8, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xff00ff),
    gsSPLightColor(LIGHT_2, 0x9900ff),
    gsSPVertex(wdw_seg7_vertex_07013038, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xff0000ff),
    gsSPLightColor(LIGHT_2, 0x990000ff),
    gsSPVertex(wdw_seg7_vertex_07013098, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x070131B8 - 0x07013238
const Gfx wdw_seg7_dl_070131B8[] = {
    gsDPPipeSync(),
    gsDPSetEnvColor(255, 255, 255, 200),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(wdw_seg7_dl_070130F8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};
