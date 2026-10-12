// 0x0700B0D8 - 0x0700B0F0

// 0x0700B0F0 - 0x0700B1D0
static const Vtx ddd_seg7_vertex_0700B0F0[] = {
#include "assets/rom/generated/5562cd1ce49607621f9e.inc.c"













};

// 0x0700B1D0 - 0x0700B2D0
static const Vtx ddd_seg7_vertex_0700B1D0[] = {
#include "assets/rom/generated/0758fbe23e2894fa9be3.inc.c"















};

// 0x0700B2D0 - 0x0700B3D0
static const Vtx ddd_seg7_vertex_0700B2D0[] = {
#include "assets/rom/generated/3bdf160d3df3b8288328.inc.c"















};

// 0x0700B3D0 - 0x0700B4D0
static const Vtx ddd_seg7_vertex_0700B3D0[] = {
#include "assets/rom/generated/de0fcc8f7efb4cd0aa13.inc.c"















};

// 0x0700B4D0 - 0x0700B5D0
static const Vtx ddd_seg7_vertex_0700B4D0[] = {
#include "assets/rom/generated/0c2411302c7afdf5a9a1.inc.c"















};

// 0x0700B5D0 - 0x0700B6D0
static const Vtx ddd_seg7_vertex_0700B5D0[] = {
#include "assets/rom/generated/905cb58be492db89f544.inc.c"















};

// 0x0700B6D0 - 0x0700B7D0
static const Vtx ddd_seg7_vertex_0700B6D0[] = {
#include "assets/rom/generated/aaf147db664a252eb999.inc.c"















};

// 0x0700B7D0 - 0x0700B890
static const Vtx ddd_seg7_vertex_0700B7D0[] = {
#include "assets/rom/generated/5e4988a91e11f3ca99f5.inc.c"











};

// 0x0700B890 - 0x0700BAE0
static const Gfx ddd_seg7_dl_0700B890[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ddd_seg7_texture_07002000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(ddd_seg7_vertex_0700B0F0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  0,  5, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 7,  9,  8, 0x0, 10, 11, 12, 0x0),
    gsSP1Triangle(11, 13, 12, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700B1D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 13, 15, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700B2D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 13, 15, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700B3D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 13, 15, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700B4D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  8,  6, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle(10, 15, 11, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700B5D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 2,  4,  3, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 13, 15, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700B6D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 13, 15, 14, 0x0),
    gsSPVertex(ddd_seg7_vertex_0700B7D0, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 1,  6,  2, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0, 11,  3,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700BAE0 - 0x0700BB50
const Gfx ddd_seg7_dl_0700BAE0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ddd_seg7_dl_0700B890),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};
