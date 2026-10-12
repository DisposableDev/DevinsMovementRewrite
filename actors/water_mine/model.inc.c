// Water Mine (unused)

// 0x0600A4E0

// 0x0600A4F8
ALIGNED8 static const Texture water_mine_seg6_texture_0600A4F8[] = {
#include "actors/water_mine/water_mine_left_side_unused.rgba16.inc.c"
};

// 0x0600B4F8
ALIGNED8 static const Texture water_mine_seg6_texture_0600B4F8[] = {
#include "actors/water_mine/water_mine_right_side_unused.rgba16.inc.c"
};

// 0x0600C4F8
ALIGNED8 static const Texture water_mine_seg6_texture_0600C4F8[] = {
#include "actors/water_mine/water_mine_spike_unused.rgba16.inc.c"
};

// 0x0600CCF8
static const Vtx water_mine_seg6_vertex_0600CCF8[] = {
#include "assets/rom/generated/3bc621fb709a1ebdb79f.inc.c"



};

// 0x0600CD38
static const Vtx water_mine_seg6_vertex_0600CD38[] = {
#include "assets/rom/generated/d26d821b8242d5d5619f.inc.c"



};

// 0x0600CD78
static const Vtx water_mine_seg6_vertex_0600CD78[] = {
#include "assets/rom/generated/e2a99ace23da6fdb0cd1.inc.c"














};

// 0x0600CE68
static const Vtx water_mine_seg6_vertex_0600CE68[] = {
#include "assets/rom/generated/bbdf4b60d45332e72405.inc.c"














};

// 0x0600CF58
static const Vtx water_mine_seg6_vertex_0600CF58[] = {
#include "assets/rom/generated/d5f312ab122cd18e6d45.inc.c"














};

// 0x0600D048
static const Vtx water_mine_seg6_vertex_0600D048[] = {
#include "assets/rom/generated/0ba2725923073c34a40c.inc.c"














};

// 0x0600D138
static const Vtx water_mine_seg6_vertex_0600D138[] = {
#include "assets/rom/generated/fd69f49e0b9f9e4251bb.inc.c"











};

// 0x0600D1F8 - 0x0600D230
const Gfx water_mine_seg6_dl_0600D1F8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_mine_seg6_texture_0600A4F8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(water_mine_seg6_vertex_0600CCF8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600D230 - 0x0600D268
const Gfx water_mine_seg6_dl_0600D230[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_mine_seg6_texture_0600B4F8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(water_mine_seg6_vertex_0600CD38, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600D268 - 0x0600D2E0
const Gfx water_mine_seg6_dl_0600D268[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(water_mine_seg6_dl_0600D1F8),
    gsSPDisplayList(water_mine_seg6_dl_0600D230),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0600D2E0 - 0x0600D3F8
const Gfx water_mine_seg6_dl_0600D2E0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_mine_seg6_texture_0600C4F8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(water_mine_seg6_vertex_0600CD78, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(water_mine_seg6_vertex_0600CE68, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(water_mine_seg6_vertex_0600CF58, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(water_mine_seg6_vertex_0600D048, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(water_mine_seg6_vertex_0600D138, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600D3F8 - 0x0600D458
const Gfx water_mine_seg6_dl_0600D3F8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(water_mine_seg6_dl_0600D2E0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
