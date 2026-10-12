// 0x07013C58 - 0x07013C70

// 0x07013C70 - 0x07013C88

// 0x07013C88 - 0x07013D08
static const Vtx bbh_seg7_vertex_07013C88[] = {
#include "assets/rom/generated/8743310e2a9becd5aaac.inc.c"







};

// 0x07013D08 - 0x07013DC8
static const Vtx bbh_seg7_vertex_07013D08[] = {
#include "assets/rom/generated/e9146132495b1094134f.inc.c"











};

// 0x07013DC8 - 0x07013E08
static const Vtx bbh_seg7_vertex_07013DC8[] = {
#include "assets/rom/generated/2e045229c12b47e8f26f.inc.c"



};

// 0x07013E08 - 0x07013E48
static const Vtx bbh_seg7_vertex_07013E08[] = {
#include "assets/rom/generated/436b4699cb4df2c3c234.inc.c"



};

// 0x07013E48 - 0x07013F38
static const Vtx bbh_seg7_vertex_07013E48[] = {
#include "assets/rom/generated/d5a898ca7c3024a5af31.inc.c"














};

// 0x07013F38 - 0x07014028
static const Vtx bbh_seg7_vertex_07013F38[] = {
#include "assets/rom/generated/47e89c7e12a75ba7afe0.inc.c"














};

// 0x07014028 - 0x07014088
static const Vtx bbh_seg7_vertex_07014028[] = {
#include "assets/rom/generated/18d661dbbf082b3d6765.inc.c"





};

// 0x07014088 - 0x07014178
static const Vtx bbh_seg7_vertex_07014088[] = {
#include "assets/rom/generated/427f93c5c65076260f4a.inc.c"














};

// 0x07014178 - 0x07014268
static const Vtx bbh_seg7_vertex_07014178[] = {
#include "assets/rom/generated/c59ff21ff5707f9d9e61.inc.c"














};

// 0x07014268 - 0x07014368
static const Vtx bbh_seg7_vertex_07014268[] = {
#include "assets/rom/generated/43c9a39f47d4ccdd9293.inc.c"















};

// 0x07014368 - 0x07014458
static const Vtx bbh_seg7_vertex_07014368[] = {
#include "assets/rom/generated/79803ea57c367d6fbcf5.inc.c"














};

// 0x07014458 - 0x07014538
static const Vtx bbh_seg7_vertex_07014458[] = {
#include "assets/rom/generated/ae09b86dfa904fbc6b9a.inc.c"













};

// 0x07014538 - 0x07014638
static const Vtx bbh_seg7_vertex_07014538[] = {
#include "assets/rom/generated/6f2156c43dda4c87912f.inc.c"















};

// 0x07014638 - 0x07014738
static const Vtx bbh_seg7_vertex_07014638[] = {
#include "assets/rom/generated/e888d825b84d620fffb2.inc.c"















};

// 0x07014738 - 0x07014838
static const Vtx bbh_seg7_vertex_07014738[] = {
#include "assets/rom/generated/75871445b78a4f3c259a.inc.c"















};

// 0x07014838 - 0x07014938
static const Vtx bbh_seg7_vertex_07014838[] = {
#include "assets/rom/generated/f3572af2ee0655a3ffdc.inc.c"















};

// 0x07014938 - 0x07014A28
static const Vtx bbh_seg7_vertex_07014938[] = {
#include "assets/rom/generated/0e6f671419150b3f9a02.inc.c"














};

// 0x07014A28 - 0x07014B28
static const Vtx bbh_seg7_vertex_07014A28[] = {
#include "assets/rom/generated/0937a2fa22e656ba9f24.inc.c"















};

// 0x07014B28 - 0x07014BC8
static const Gfx bbh_seg7_dl_07014B28[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09008800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x505050ff),
    gsSPLightColor(LIGHT_2, 0x202020ff),
    gsSPVertex(bbh_seg7_vertex_07013C88, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(bbh_seg7_vertex_07013D08, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x07014BC8 - 0x07014C10
static const Gfx bbh_seg7_dl_07014BC8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_0900A000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x505050ff),
    gsSPLightColor(LIGHT_2, 0x202020ff),
    gsSPVertex(bbh_seg7_vertex_07013DC8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07014C10 - 0x07014C58
static const Gfx bbh_seg7_dl_07014C10[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09004800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(bbh_seg7_vertex_07013E08, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07014C58 - 0x07014D10
static const Gfx bbh_seg7_dl_07014C58[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_07013E48, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_07013F38, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_07014028, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x07014D10 - 0x07014FD8
static const Gfx bbh_seg7_dl_07014D10[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_07014088, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_07014178, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_07014268, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  1, 15,  2, 0x0),
    gsSPVertex(bbh_seg7_vertex_07014368, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_07014458, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(bbh_seg7_vertex_07014538, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  0,  9,  1, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_07014638, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  9, 11, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_07014738, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_07014838, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  9, 15, 10, 0x0),
    gsSPVertex(bbh_seg7_vertex_07014938, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  0,  2, 13, 0x0),
    gsSP1Triangle(10, 12, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_07014A28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x07014FD8 - 0x07015098
const Gfx bbh_seg7_dl_07014FD8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_07014B28),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_07014BC8),
    gsSPDisplayList(bbh_seg7_dl_07014C10),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_07014C58),
    gsSPDisplayList(bbh_seg7_dl_07014D10),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
