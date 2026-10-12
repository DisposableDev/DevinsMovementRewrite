// 0x07010930 - 0x07010948

// 0x07010948 - 0x07010A28
static const Vtx ttc_seg7_vertex_07010948[] = {
#include "assets/rom/generated/cfef1a83ed0f3afcc5ca.inc.c"













};

// 0x07010A28 - 0x07010B28
static const Vtx ttc_seg7_vertex_07010A28[] = {
#include "assets/rom/generated/8beb75a8fbc9a3787b7a.inc.c"















};

// 0x07010B28 - 0x07010B78
static const Vtx ttc_seg7_vertex_07010B28[] = {
#include "assets/rom/generated/483776ad1e9f8c93a9e1.inc.c"




};

// 0x07010B78 - 0x07010BF8
static const Vtx ttc_seg7_vertex_07010B78[] = {
#include "assets/rom/generated/51a9c4ccf206f2851e6a.inc.c"







};

// 0x07010BF8 - 0x07010CF0
static const Gfx ttc_seg7_dl_07010BF8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, machine_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ttc_seg7_vertex_07010948, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  3,  5,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10, 11,  8, 0x0),
    gsSP2Triangles( 9, 12, 10, 0x0, 11, 10, 12, 0x0),
    gsSP1Triangle( 1,  0, 13, 0x0),
    gsSPVertex(ttc_seg7_vertex_07010A28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  3, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  9, 11, 12, 0x0),
    gsSP2Triangles( 9, 12, 10, 0x0,  1, 13,  2, 0x0),
    gsSP2Triangles( 1, 14, 13, 0x0,  0, 15,  1, 0x0),
    gsSPVertex(ttc_seg7_vertex_07010B28, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP1Triangle( 0,  2,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x07010CF0 - 0x07010D38
static const Gfx ttc_seg7_dl_07010CF0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, machine_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ttc_seg7_vertex_07010B78, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x07010D38 - 0x07010DE8
const Gfx ttc_seg7_dl_07010D38[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(200, 255, 255, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ttc_seg7_dl_07010BF8),
    gsSPDisplayList(ttc_seg7_dl_07010CF0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
