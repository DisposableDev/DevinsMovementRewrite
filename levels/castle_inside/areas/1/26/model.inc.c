// 0x0703A878 - 0x0703A890

// 0x0703A890 - 0x0703A8A8

// 0x0703A8A8 - 0x0703A998
static const Vtx inside_castle_seg7_vertex_0703A8A8[] = {
#include "assets/rom/generated/a655a79af8ae6aad410a.inc.c"














};

// 0x0703A998 - 0x0703AA98
static const Vtx inside_castle_seg7_vertex_0703A998[] = {
#include "assets/rom/generated/b8ef7d960c542519cc16.inc.c"















};

// 0x0703AA98 - 0x0703AB88
static const Vtx inside_castle_seg7_vertex_0703AA98[] = {
#include "assets/rom/generated/510cb5b2f65bbbbb95c1.inc.c"














};

// 0x0703AB88 - 0x0703ABC8
static const Vtx inside_castle_seg7_vertex_0703AB88[] = {
#include "assets/rom/generated/c163bd1e5bdce4de1c10.inc.c"



};

// 0x0703ABC8 - 0x0703ACC8
static const Vtx inside_castle_seg7_vertex_0703ABC8[] = {
#include "assets/rom/generated/6786e02eb77fb9324009.inc.c"















};

// 0x0703ACC8 - 0x0703ADB8
static const Vtx inside_castle_seg7_vertex_0703ACC8[] = {
#include "assets/rom/generated/a137fefade6658256e8c.inc.c"














};

// 0x0703ADB8 - 0x0703AEB8
static const Vtx inside_castle_seg7_vertex_0703ADB8[] = {
#include "assets/rom/generated/35257034bb5a8335e0ce.inc.c"















};

// 0x0703AEB8 - 0x0703AFA8
static const Vtx inside_castle_seg7_vertex_0703AEB8[] = {
#include "assets/rom/generated/296ef2978f24ab199d3c.inc.c"














};

// 0x0703AFA8 - 0x0703B0A8
static const Vtx inside_castle_seg7_vertex_0703AFA8[] = {
#include "assets/rom/generated/c3a11af5ac3bade85a0b.inc.c"















};

// 0x0703B0A8 - 0x0703B1A8
static const Vtx inside_castle_seg7_vertex_0703B0A8[] = {
#include "assets/rom/generated/2b5f687c1ccc9c905e88.inc.c"















};

// 0x0703B1A8 - 0x0703B298
static const Vtx inside_castle_seg7_vertex_0703B1A8[] = {
#include "assets/rom/generated/581ed344a212fe1dda7a.inc.c"














};

// 0x0703B298 - 0x0703B398
static const Vtx inside_castle_seg7_vertex_0703B298[] = {
#include "assets/rom/generated/7a1fa2e87cbe04c33402.inc.c"















};

// 0x0703B398 - 0x0703B458
static const Vtx inside_castle_seg7_vertex_0703B398[] = {
#include "assets/rom/generated/3fc164d87fa3157457fa.inc.c"











};

// 0x0703B458 - 0x0703B548
static const Vtx inside_castle_seg7_vertex_0703B458[] = {
#include "assets/rom/generated/e32b0ade92cd89194e53.inc.c"














};

// 0x0703B548 - 0x0703B5B8
static const Vtx inside_castle_seg7_vertex_0703B548[] = {
#include "assets/rom/generated/c3656d796abe6de4ed15.inc.c"






};

// 0x0703B5B8 - 0x0703B6C8
static const Gfx inside_castle_seg7_dl_0703B5B8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_0900B000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xccccccff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(inside_castle_seg7_vertex_0703A8A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  7,  9, 0x0,  4,  6, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0703A998, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0703AA98, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x5f5f5fff),
    gsSPVertex(inside_castle_seg7_vertex_0703AB88, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0703B6C8 - 0x0703B740
static const Gfx inside_castle_seg7_dl_0703B6C8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_0703ABC8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  1,  5,  3, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 10, 12, 0x0),
    gsSP2Triangles(13, 12, 14, 0x0, 12, 15, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0703B740 - 0x0703B808
static const Gfx inside_castle_seg7_dl_0703B740[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_0703ACC8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0703ADB8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0703AEB8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0703B808 - 0x0703B988
static const Gfx inside_castle_seg7_dl_0703B808[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09003000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_0703AFA8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  3,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11,  8, 10, 0x0),
    gsSP2Triangles(12, 11, 10, 0x0, 12, 10,  1, 0x0),
    gsSP2Triangles( 0, 12,  1, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0703B0A8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0703B1A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 1, 14,  2, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0703B298, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0703B398, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0703B988 - 0x0703BA08
static const Gfx inside_castle_seg7_dl_0703B988[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_0703B458, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_0703B548, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP1Triangle( 4,  5,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0703BA08 - 0x0703BAB0
const Gfx inside_castle_seg7_dl_0703BA08[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_0703B5B8),
    gsSPDisplayList(inside_castle_seg7_dl_0703B6C8),
    gsSPDisplayList(inside_castle_seg7_dl_0703B740),
    gsSPDisplayList(inside_castle_seg7_dl_0703B808),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_0703B988),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
