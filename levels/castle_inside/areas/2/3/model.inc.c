// 0x070430F8 - 0x07043110

// 0x07043110 - 0x07043128

// 0x07043128 - 0x070431A8
static const Vtx inside_castle_seg7_vertex_07043128[] = {
#include "assets/rom/generated/c5addd84cfc10df1f04a.inc.c"







};

// 0x070431A8 - 0x07043298
static const Vtx inside_castle_seg7_vertex_070431A8[] = {
#include "assets/rom/generated/d2d709073b6ac983ea5e.inc.c"














};

// 0x07043298 - 0x07043398
static const Vtx inside_castle_seg7_vertex_07043298[] = {
#include "assets/rom/generated/0a6513ec4971018f2ff0.inc.c"















};

// 0x07043398 - 0x07043498
static const Vtx inside_castle_seg7_vertex_07043398[] = {
#include "assets/rom/generated/4247603810cdbed584ba.inc.c"















};

// 0x07043498 - 0x07043568
static const Vtx inside_castle_seg7_vertex_07043498[] = {
#include "assets/rom/generated/b940d7912161dcb3d0fe.inc.c"












};

// 0x07043568 - 0x070435C8
static const Vtx inside_castle_seg7_vertex_07043568[] = {
#include "assets/rom/generated/eed0bdbc85d449431454.inc.c"





};

// 0x070435C8 - 0x07043608
static const Vtx inside_castle_seg7_vertex_070435C8[] = {
#include "assets/rom/generated/15045f3cae287a4592ac.inc.c"



};

// 0x07043608 - 0x07043648
static const Vtx inside_castle_seg7_vertex_07043608[] = {
#include "assets/rom/generated/f00ef37e2e65a4188206.inc.c"



};

// 0x07043648 - 0x07043688
static const Vtx inside_castle_seg7_vertex_07043648[] = {
#include "assets/rom/generated/9a0f62b14c83796ca033.inc.c"



};

// 0x07043688 - 0x070436C8
static const Vtx inside_castle_seg7_vertex_07043688[] = {
#include "assets/rom/generated/63b52f03ac5e7999d705.inc.c"



};

// 0x070436C8 - 0x07043708
static const Vtx inside_castle_seg7_vertex_070436C8[] = {
#include "assets/rom/generated/bee23fba1b31b1c45bb5.inc.c"



};

// 0x07043708 - 0x07043748
static const Vtx inside_castle_seg7_vertex_07043708[] = {
#include "assets/rom/generated/a09e4dce6a2a8c0bf1be.inc.c"



};

// 0x07043748 - 0x07043788
static const Vtx inside_castle_seg7_vertex_07043748[] = {
#include "assets/rom/generated/2fe822b34670538c8705.inc.c"



};

// 0x07043788 - 0x070437C8
static const Vtx inside_castle_seg7_vertex_07043788[] = {
#include "assets/rom/generated/0097c36699d744642a1a.inc.c"



};

// 0x070437C8 - 0x07043820
static const Gfx inside_castle_seg7_dl_070437C8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09008000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x5f5f5fff),
    gsSPVertex(inside_castle_seg7_vertex_07043128, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x07043820 - 0x07043930
static const Gfx inside_castle_seg7_dl_07043820[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xac9d52ff),
    gsSPLightColor(LIGHT_2, 0x403a1eff),
    gsSPVertex(inside_castle_seg7_vertex_070431A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07043298, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07043398, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07043498, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP1Triangle(10, 11, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x07043930 - 0x07043988
static const Gfx inside_castle_seg7_dl_07043930[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09002000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x5f5f5fff),
    gsSPVertex(inside_castle_seg7_vertex_07043568, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  3,  0, 0x0,  4,  5,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07043988 - 0x070439C0
static const Gfx inside_castle_seg7_dl_07043988[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_castle_seg7_texture_07010800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_070435C8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x070439C0 - 0x070439F8
static const Gfx inside_castle_seg7_dl_070439C0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_castle_seg7_texture_07011800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07043608, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x070439F8 - 0x07043A30
static const Gfx inside_castle_seg7_dl_070439F8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_castle_seg7_texture_0700E800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07043648, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x07043A30 - 0x07043A68
static const Gfx inside_castle_seg7_dl_07043A30[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_castle_seg7_texture_0700F800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07043688, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x07043A68 - 0x07043AA0
static const Gfx inside_castle_seg7_dl_07043A68[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_castle_seg7_texture_0700C800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_070436C8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07043AA0 - 0x07043AD8
static const Gfx inside_castle_seg7_dl_07043AA0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_castle_seg7_texture_0700D800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07043708, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07043AD8 - 0x07043B10
static const Gfx inside_castle_seg7_dl_07043AD8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_castle_seg7_texture_0700A800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07043748, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07043B10 - 0x07043B48
static const Gfx inside_castle_seg7_dl_07043B10[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_castle_seg7_texture_0700B800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07043788, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07043B48 - 0x07043C38
const Gfx inside_castle_seg7_dl_07043B48[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_070437C8),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07043820),
    gsSPDisplayList(inside_castle_seg7_dl_07043930),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07043988),
    gsSPDisplayList(inside_castle_seg7_dl_070439C0),
    gsSPDisplayList(inside_castle_seg7_dl_070439F8),
    gsSPDisplayList(inside_castle_seg7_dl_07043A30),
    gsSPDisplayList(inside_castle_seg7_dl_07043A68),
    gsSPDisplayList(inside_castle_seg7_dl_07043AA0),
    gsSPDisplayList(inside_castle_seg7_dl_07043AD8),
    gsSPDisplayList(inside_castle_seg7_dl_07043B10),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
