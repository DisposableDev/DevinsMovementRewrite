// 0x0702ACE8 - 0x0702AD00

// 0x0702AD00 - 0x0702AD18

// 0x0702AD18 - 0x0702AD30

// 0x0702AD30 - 0x0702B530
ALIGNED8 static const Texture ttm_seg7_texture_0702AD30[] = {
#include "levels/ttm/8.rgba16.inc.c"
};

// 0x0702B530 - 0x0702B570
static const Vtx ttm_seg7_vertex_0702B530[] = {
#include "assets/rom/generated/bd0ec1675ae937caffae.inc.c"



};

// 0x0702B570 - 0x0702B670
static const Vtx ttm_seg7_vertex_0702B570[] = {
#include "assets/rom/generated/e252bdf602e582caefc0.inc.c"















};

// 0x0702B670 - 0x0702B770
static const Vtx ttm_seg7_vertex_0702B670[] = {
#include "assets/rom/generated/f02e846577bb07439dec.inc.c"















};

// 0x0702B770 - 0x0702B870
static const Vtx ttm_seg7_vertex_0702B770[] = {
#include "assets/rom/generated/1b30f6243f36c34f2fcb.inc.c"















};

// 0x0702B870 - 0x0702B970
static const Vtx ttm_seg7_vertex_0702B870[] = {
#include "assets/rom/generated/bc32588799d0faeea57e.inc.c"















};

// 0x0702B970 - 0x0702B9D0
static const Vtx ttm_seg7_vertex_0702B970[] = {
#include "assets/rom/generated/47eaeb6c3cbeb041a08e.inc.c"





};

// 0x0702B9D0 - 0x0702BB60
static const Gfx ttm_seg7_dl_0702B9D0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ttm_seg7_texture_0702AD30),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x333333ff),
    gsSPLightColor(LIGHT_2, 0xc0c0cff),
    gsSPVertex(ttm_seg7_vertex_0702B530, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0x777777ff),
    gsSPLightColor(LIGHT_2, 0x1d1d1dff),
    gsSPVertex(ttm_seg7_vertex_0702B570, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(ttm_seg7_vertex_0702B670, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ttm_seg7_vertex_0702B770, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(ttm_seg7_vertex_0702B870, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0, 15,  1, 0x0),
    gsSPVertex(ttm_seg7_vertex_0702B970, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0702BB60 - 0x0702BBD0
const Gfx ttm_seg7_dl_0702BB60[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ttm_seg7_dl_0702B9D0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
