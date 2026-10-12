// 0x07014C68 - 0x07014CE8
static const Vtx lll_seg7_vertex_07014C68[] = {
#include "assets/rom/generated/2e48d90e8dd8f900c385.inc.c"







};

// 0x07014CE8 - 0x07014DD8
static const Vtx lll_seg7_vertex_07014CE8[] = {
#include "assets/rom/generated/93b32fdf89aab48e7c1f.inc.c"














};

// 0x07014DD8 - 0x07014EC8
static const Vtx lll_seg7_vertex_07014DD8[] = {
#include "assets/rom/generated/e5b0a7eb0921ab8addf8.inc.c"














};

// 0x07014EC8 - 0x07014F08
static const Vtx lll_seg7_vertex_07014EC8[] = {
#include "assets/rom/generated/4196786773df05119f03.inc.c"



};

// 0x07014F08 - 0x07014FE8
static const Vtx lll_seg7_vertex_07014F08[] = {
#include "assets/rom/generated/ee9d196fd032405caaaa.inc.c"













};

// 0x07014FE8 - 0x070150C8
static const Vtx lll_seg7_vertex_07014FE8[] = {
#include "assets/rom/generated/80a5770478ac493e91d8.inc.c"













};

// 0x070150C8 - 0x07015108
static const Vtx lll_seg7_vertex_070150C8[] = {
#include "assets/rom/generated/77dcac7e48a5b560cf0b.inc.c"



};

// 0x07015108 - 0x07015208
static const Vtx lll_seg7_vertex_07015108[] = {
#include "assets/rom/generated/8f1a4a2ee763dcd55b9e.inc.c"















};

// 0x07015208 - 0x07015270
static const Gfx lll_seg7_dl_07015208[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lll_seg7_texture_07004800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(lll_seg7_vertex_07014C68, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  0,  5,  6, 0x0),
    gsSP2Triangles( 0,  7,  5, 0x0,  0,  6,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07015270 - 0x07015328
static const Gfx lll_seg7_dl_07015270[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fire_09007800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(lll_seg7_vertex_07014CE8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(lll_seg7_vertex_07014DD8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(lll_seg7_vertex_07014EC8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07015328 - 0x070153F0
static const Gfx lll_seg7_dl_07015328[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fire_09006800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(lll_seg7_vertex_07014F08, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  4, 0x0),
    gsSP2Triangles( 3,  2,  1, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  5,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(lll_seg7_vertex_07014FE8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  5,  9,  6, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(lll_seg7_vertex_070150C8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x070153F0 - 0x07015458
static const Gfx lll_seg7_dl_070153F0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, fire_09006000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(lll_seg7_vertex_07015108, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0,  2, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x07015458 - 0x070154F8
const Gfx lll_seg7_dl_07015458[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(lll_seg7_dl_07015208),
    gsSPDisplayList(lll_seg7_dl_07015328),
    gsSPDisplayList(lll_seg7_dl_070153F0),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(lll_seg7_dl_07015270),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
