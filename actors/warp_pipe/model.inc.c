// Warp Pipe

// 0x030079E8

// 0x03007A00
static const Vtx warp_pipe_seg3_vertex_03007A00[] = {
#include "assets/rom/generated/e48f0ab0ad22777b7480.inc.c"














};

// 0x03007AF0
static const Vtx warp_pipe_seg3_vertex_03007AF0[] = {
#include "assets/rom/generated/ecd3d99670b29898a937.inc.c"














};

// 0x03007BE0
static const Vtx warp_pipe_seg3_vertex_03007BE0[] = {
#include "assets/rom/generated/e23fef16d6c16e0606bf.inc.c"














};

// 0x03007CD0
static const Vtx warp_pipe_seg3_vertex_03007CD0[] = {
#include "assets/rom/generated/be3b2ea48ac0ab4adf77.inc.c"














};

// 0x03007DC0
static const Vtx warp_pipe_seg3_vertex_03007DC0[] = {
#include "assets/rom/generated/10bf06ce7cfb953be5e6.inc.c"







};

// 0x03007E40
ALIGNED8 static const Texture warp_pipe_seg3_texture_03007E40[] = {
#include "actors/warp_pipe/warp_pipe_side.custom.rgba16.inc.c"
};

// 0x03008E40 - 0x03008F98
const Gfx warp_pipe_seg3_dl_03008E40[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, warp_pipe_seg3_texture_03007E40),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(warp_pipe_seg3_vertex_03007A00, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(warp_pipe_seg3_vertex_03007AF0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(warp_pipe_seg3_vertex_03007BE0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(warp_pipe_seg3_vertex_03007CD0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(warp_pipe_seg3_vertex_03007DC0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x03008F98 - 0x03008FF8
const Gfx warp_pipe_seg3_dl_03008F98[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(warp_pipe_seg3_dl_03008E40),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x03008FF8

// 0x03009010

// 0x03009028
static const Vtx warp_pipe_seg3_vertex_03009028[] = {
#include "assets/rom/generated/b52866e1cc729c076af3.inc.c"















};

// 0x03009128
static const Vtx warp_pipe_seg3_vertex_03009128[] = {
#include "assets/rom/generated/cf5db5b7e6c4dd86dbad.inc.c"



};

// 0x03009168
ALIGNED8 static const Texture warp_pipe_seg3_texture_03009168[] = {
#include "actors/warp_pipe/warp_pipe_top.rgba16.inc.c"
};

// 0x03009968 - 0x03009A20
const Gfx warp_pipe_seg3_dl_03009968[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, warp_pipe_seg3_texture_03009168),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(warp_pipe_seg3_vertex_03009028, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  3,  5, 0x0),
    gsSP2Triangles( 8,  4,  6, 0x0,  8,  9,  4, 0x0),
    gsSP2Triangles( 7,  5, 10, 0x0, 11,  7, 10, 0x0),
    gsSP2Triangles(11, 10, 12, 0x0,  1, 11, 12, 0x0),
    gsSP2Triangles( 1, 12,  2, 0x0, 13, 14,  9, 0x0),
    gsSP2Triangles( 0,  2, 15, 0x0, 13,  0, 15, 0x0),
    gsSP2Triangles(13, 15, 14, 0x0, 13,  9,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x03009A20 - 0x03009A50
const Gfx warp_pipe_seg3_dl_03009A20[] = {
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(warp_pipe_seg3_vertex_03009128, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x03009A50 - 0x03009AC8
const Gfx warp_pipe_seg3_dl_03009A50[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(warp_pipe_seg3_dl_03009968),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(warp_pipe_seg3_dl_03009A20),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
