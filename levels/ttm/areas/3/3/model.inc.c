// 0x07024CA8 - 0x07024CC0

// 0x07024CC0 - 0x07024DC0
static const Vtx ttm_seg7_vertex_07024CC0[] = {
#include "assets/rom/generated/299cc9f9d8c2c94959c0.inc.c"















};

// 0x07024DC0 - 0x07024EA0
static const Vtx ttm_seg7_vertex_07024DC0[] = {
#include "assets/rom/generated/87f245e5351314ca3d78.inc.c"













};

// 0x07024EA0 - 0x07024F80
static const Vtx ttm_seg7_vertex_07024EA0[] = {
#include "assets/rom/generated/7dc5f93515ad8a3fd68b.inc.c"













};

// 0x07024F80 - 0x07025070
static const Vtx ttm_seg7_vertex_07024F80[] = {
#include "assets/rom/generated/4cc127df055b74e93e9f.inc.c"














};

// 0x07025070 - 0x07025150
static const Vtx ttm_seg7_vertex_07025070[] = {
#include "assets/rom/generated/d0563306712460322e64.inc.c"













};

// 0x07025150 - 0x07025240
static const Vtx ttm_seg7_vertex_07025150[] = {
#include "assets/rom/generated/b65ef512db1556b8d726.inc.c"














};

// 0x07025240 - 0x07025330
static const Vtx ttm_seg7_vertex_07025240[] = {
#include "assets/rom/generated/6f9f2125cbf6bb08b900.inc.c"














};

// 0x07025330 - 0x07025410
static const Vtx ttm_seg7_vertex_07025330[] = {
#include "assets/rom/generated/f0fc34bae5f3de749c38.inc.c"













};

// 0x07025410 - 0x07025500
static const Vtx ttm_seg7_vertex_07025410[] = {
#include "assets/rom/generated/3373fe860b0334505f66.inc.c"














};

// 0x07025500 - 0x070255F0
static const Vtx ttm_seg7_vertex_07025500[] = {
#include "assets/rom/generated/132fd8ca0e6f72587688.inc.c"














};

// 0x070255F0 - 0x070256F0
static const Vtx ttm_seg7_vertex_070255F0[] = {
#include "assets/rom/generated/8e761e8d20284375ffc4.inc.c"















};

// 0x070256F0 - 0x070257F0
static const Vtx ttm_seg7_vertex_070256F0[] = {
#include "assets/rom/generated/c103a8e74c9a377e871f.inc.c"















};

// 0x070257F0 - 0x070258E0
static const Vtx ttm_seg7_vertex_070257F0[] = {
#include "assets/rom/generated/f09eccbe8fed03602dcd.inc.c"














};

// 0x070258E0 - 0x070259C0
static const Vtx ttm_seg7_vertex_070258E0[] = {
#include "assets/rom/generated/383710ba20b33fbf4f10.inc.c"













};

// 0x070259C0 - 0x07025AB0
static const Vtx ttm_seg7_vertex_070259C0[] = {
#include "assets/rom/generated/1f3fcf69987ae0a650e0.inc.c"














};

// 0x07025AB0 - 0x07025B30
static const Vtx ttm_seg7_vertex_07025AB0[] = {
#include "assets/rom/generated/0e69a7a203f44b883226.inc.c"







};

// 0x07025B30 - 0x07025F48
static const Gfx ttm_seg7_dl_07025B30[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(ttm_seg7_vertex_07024CC0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 1,  4,  3, 0x0,  5,  0,  6, 0x0),
    gsSP2Triangles( 0,  2,  6, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 8,  5,  7, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ttm_seg7_vertex_07024DC0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(ttm_seg7_vertex_07024EA0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(ttm_seg7_vertex_07024F80, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_07025070, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 2, 10,  0, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(ttm_seg7_vertex_07025150, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 6,  8,  7, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_07025240, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_07025330, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(ttm_seg7_vertex_07025410, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ttm_seg7_vertex_07025500, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_070255F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0, 15,  1, 0x0),
    gsSPVertex(ttm_seg7_vertex_070256F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9,  8, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(ttm_seg7_vertex_070257F0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  4,  6, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(ttm_seg7_vertex_070258E0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(ttm_seg7_vertex_070259C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_07025AB0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x07025F48 - 0x07025FF0
const Gfx ttm_seg7_dl_07025F48[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(0, 0, 0, 255),
    gsSPFogPosition(980, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ttm_seg7_dl_07025B30),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
