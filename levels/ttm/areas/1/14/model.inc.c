// 0x0700D700 - 0x0700D718

// 0x0700D718 - 0x0700D730

// 0x0700D730 - 0x0700D748

// 0x0700D748 - 0x0700D848
static const Vtx ttm_seg7_vertex_0700D748[] = {
#include "assets/rom/generated/2bdb88e81b81f4fed6cf.inc.c"















};

// 0x0700D848 - 0x0700D938
static const Vtx ttm_seg7_vertex_0700D848[] = {
#include "assets/rom/generated/252d8276c9842f191dd2.inc.c"














};

// 0x0700D938 - 0x0700D9F8
static const Vtx ttm_seg7_vertex_0700D938[] = {
#include "assets/rom/generated/bd49410a183e0e7e859f.inc.c"











};

// 0x0700D9F8 - 0x0700DAE8
static const Vtx ttm_seg7_vertex_0700D9F8[] = {
#include "assets/rom/generated/e4224684c566a83e48f9.inc.c"














};

// 0x0700DAE8 - 0x0700DBD8
static const Vtx ttm_seg7_vertex_0700DAE8[] = {
#include "assets/rom/generated/368ae1ef15ec0d99757a.inc.c"














};

// 0x0700DBD8 - 0x0700DCC8
static const Vtx ttm_seg7_vertex_0700DBD8[] = {
#include "assets/rom/generated/fa0f35615e4e3d139554.inc.c"














};

// 0x0700DCC8 - 0x0700DD28
static const Vtx ttm_seg7_vertex_0700DCC8[] = {
#include "assets/rom/generated/8fdd91d67964440ad960.inc.c"





};

// 0x0700DD28 - 0x0700DD88
static const Vtx ttm_seg7_vertex_0700DD28[] = {
#include "assets/rom/generated/77942610075a50409647.inc.c"





};

// 0x0700DD88 - 0x0700DDB8
static const Vtx ttm_seg7_vertex_0700DD88[] = {
#include "assets/rom/generated/5d27d4eabba9f2294280.inc.c"


};

// 0x0700DDB8 - 0x0700DF78
static const Gfx ttm_seg7_dl_0700DDB8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ttm_seg7_vertex_0700D748, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700D848, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700D938, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPLightColor(LIGHT_1, 0x999999ff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(ttm_seg7_vertex_0700D9F8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700DAE8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700DBD8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(ttm_seg7_vertex_0700DCC8, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xbbbbbbff),
    gsSPLightColor(LIGHT_2, 0x5d5d5dff),
    gsSPVertex(ttm_seg7_vertex_0700DD28, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ttm_seg7_vertex_0700DD88, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700DF78 - 0x0700DFE8
const Gfx ttm_seg7_dl_0700DF78[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ttm_seg7_dl_0700DDB8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
