// Cannon Base

// 0x08004988

// 0x080049A0

// 0x080049B8
ALIGNED8 static const Texture cannon_base_seg8_texture_080049B8[] = {
#include "actors/cannon_base/cannon_base.rgba16.inc.c"
};

// 0x080051B8
static const Vtx cannon_base_seg8_vertex_080051B8[] = {
#include "assets/rom/generated/b8e3d6cbe38649875414.inc.c"











};

// 0x08005278
static const Vtx cannon_base_seg8_vertex_08005278[] = {
#include "assets/rom/generated/ed2d227c429fa1cb8182.inc.c"















};

// 0x08005378
static const Vtx cannon_base_seg8_vertex_08005378[] = {
#include "assets/rom/generated/2342e8100ae0b0a64c48.inc.c"














};

// 0x08005468
static const Vtx cannon_base_seg8_vertex_08005468[] = {
#include "assets/rom/generated/15c5d817efbb89e3d176.inc.c"














};

// 0x08005558
static const Vtx cannon_base_seg8_vertex_08005558[] = {
#include "assets/rom/generated/0e9aec265a0b6c69ee10.inc.c"















};

// 0x08005658 - 0x080056D0
const Gfx cannon_base_seg8_dl_08005658[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, cannon_base_seg8_texture_080049B8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(cannon_base_seg8_vertex_080051B8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  2,  5, 0x0,  0,  5,  3, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9, 10, 0x0),
    gsSP2Triangles( 6, 10, 11, 0x0,  6, 11,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x080056D0 - 0x080057F8
const Gfx cannon_base_seg8_dl_080056D0[] = {
    gsSPLightColor(LIGHT_1, 0x3037ffff),
    gsSPLightColor(LIGHT_2, 0xe104cff),
    gsSPVertex(cannon_base_seg8_vertex_08005278, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10, 11, 0x0,  7, 12, 10, 0x0),
    gsSP2Triangles( 7,  9, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(cannon_base_seg8_vertex_08005378, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(cannon_base_seg8_vertex_08005468, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  3, 13,  4, 0x0),
    gsSP1Triangle(10, 12, 14, 0x0),
    gsSPVertex(cannon_base_seg8_vertex_08005558, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x080057F8 - 0x08005870
const Gfx cannon_base_seg8_dl_080057F8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(cannon_base_seg8_dl_08005658),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(cannon_base_seg8_dl_080056D0),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
