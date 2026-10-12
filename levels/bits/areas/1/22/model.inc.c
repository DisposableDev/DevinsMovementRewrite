// 0x07012DB0 - 0x07012DC8

// 0x07012DC8 - 0x07012DE0

// 0x07012DE0 - 0x07012DF8

// 0x07012DF8 - 0x07012E78
static const Vtx bits_seg7_vertex_07012DF8[] = {
#include "assets/rom/generated/e514543fd31473feac6b.inc.c"







};

// 0x07012E78 - 0x07012F58
static const Vtx bits_seg7_vertex_07012E78[] = {
#include "assets/rom/generated/16c43d67d35327c7f807.inc.c"













};

// 0x07012F58 - 0x07013058
static const Vtx bits_seg7_vertex_07012F58[] = {
#include "assets/rom/generated/60ade72ee3c54a4c0be9.inc.c"















};

// 0x07013058 - 0x070130F8
static const Vtx bits_seg7_vertex_07013058[] = {
#include "assets/rom/generated/6ba23ffd4c108c17fbfb.inc.c"









};

// 0x070130F8 - 0x070131E8
static const Vtx bits_seg7_vertex_070130F8[] = {
#include "assets/rom/generated/32d48c631cc87c90f7d2.inc.c"














};

// 0x070131E8 - 0x070132D8
static const Vtx bits_seg7_vertex_070131E8[] = {
#include "assets/rom/generated/17ffc2bd5300332816b4.inc.c"














};

// 0x070132D8 - 0x070133B8
static const Vtx bits_seg7_vertex_070132D8[] = {
#include "assets/rom/generated/ad7fa9954f05919f5f31.inc.c"













};

// 0x070133B8 - 0x070135A0
static const Gfx bits_seg7_dl_070133B8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xb1bffaff),
    gsSPLightColor(LIGHT_2, 0x585f7dff),
    gsSPVertex(bits_seg7_vertex_07012DF8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  0,  5,  6, 0x0),
    gsSP2Triangles( 0,  7,  1, 0x0,  0,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0x988cd8ff),
    gsSPLightColor(LIGHT_2, 0x4c466cff),
    gsSPVertex(bits_seg7_vertex_07012E78, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  3,  5,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 10, 12, 0x0),
    gsSPVertex(bits_seg7_vertex_07012F58, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_07013058, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSPLightColor(LIGHT_1, 0xb265ffff),
    gsSPLightColor(LIGHT_2, 0x59327fff),
    gsSPVertex(bits_seg7_vertex_070130F8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  7, 10, 11, 0x0),
    gsSP2Triangles( 7, 11, 12, 0x0,  7, 12, 13, 0x0),
    gsSP1Triangle( 7, 14,  8, 0x0),
    gsSPVertex(bits_seg7_vertex_070131E8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bits_seg7_vertex_070132D8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x070135A0 - 0x07013610
const Gfx bits_seg7_dl_070135A0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bits_seg7_dl_070133B8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
