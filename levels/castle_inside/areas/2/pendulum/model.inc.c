// 0x07050A78 - 0x07050A90

// 0x07050A90 - 0x07050AA8

// 0x07050AA8 - 0x07050B68
static const Vtx inside_castle_seg7_vertex_07050AA8[] = {
#include "assets/rom/generated/10d23ba9be3aa59989f9.inc.c"











};

// 0x07050B68 - 0x07050C58
static const Vtx inside_castle_seg7_vertex_07050B68[] = {
#include "assets/rom/generated/804539b0772ae8d069cd.inc.c"














};

// 0x07050C58 - 0x07050D58
static const Vtx inside_castle_seg7_vertex_07050C58[] = {
#include "assets/rom/generated/9955df88fb198a2bb79f.inc.c"















};

// 0x07050D58 - 0x07050E58
static const Vtx inside_castle_seg7_vertex_07050D58[] = {
#include "assets/rom/generated/6b6cd0d4e85de6dc717c.inc.c"















};

// 0x07050E58 - 0x07050F58
static const Vtx inside_castle_seg7_vertex_07050E58[] = {
#include "assets/rom/generated/f9880e8acf63e76577a7.inc.c"















};

// 0x07050F58 - 0x07051048
static const Vtx inside_castle_seg7_vertex_07050F58[] = {
#include "assets/rom/generated/ef4ba83203396139e20d.inc.c"














};

// 0x07051048 - 0x07051108
static const Vtx inside_castle_seg7_vertex_07051048[] = {
#include "assets/rom/generated/51c230cdbfc560579555.inc.c"











};

// 0x07051108 - 0x07051170
static const Gfx inside_castle_seg7_dl_07051108[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xac7f52ff),
    gsSPLightColor(LIGHT_2, 0x402f1eff),
    gsSPVertex(inside_castle_seg7_vertex_07050AA8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x07051170 - 0x070512F8
static const Gfx inside_castle_seg7_dl_07051170[] = {
    gsSPLightColor(LIGHT_1, 0xac9d52ff),
    gsSPLightColor(LIGHT_2, 0x403a1eff),
    gsSPVertex(inside_castle_seg7_vertex_07050B68, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  7, 0x0,  0,  2,  8, 0x0),
    gsSP2Triangles( 0,  8,  9, 0x0,  0,  9, 10, 0x0),
    gsSP2Triangles( 0, 10,  6, 0x0,  3,  5, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07050C58, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07050D58, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07050E58, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  6, 15,  7, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07050F58, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07051048, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x070512F8 - 0x07051370
const Gfx inside_castle_seg7_dl_070512F8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07051108),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(inside_castle_seg7_dl_07051170),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
