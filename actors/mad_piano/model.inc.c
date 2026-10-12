// Mad Piano

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// 0x05006AF0
ALIGNED8 static const Texture mad_piano_seg5_texture_05006AF0[] = {
#include "actors/mad_piano/mad_piano_tooth.rgba16.inc.c"
};

// 0x050072F0
ALIGNED8 static const Texture mad_piano_seg5_texture_050072F0[] = {
#include "actors/mad_piano/mad_piano_body.rgba16.inc.c"
};

// 0x050076F0
ALIGNED8 static const Texture mad_piano_seg5_texture_050076F0[] = {
#include "actors/mad_piano/mad_piano_keys_corner.rgba16.inc.c"
};

// 0x05007AF0
ALIGNED8 static const Texture mad_piano_seg5_texture_05007AF0[] = {
#include "actors/mad_piano/mad_piano_mouth.rgba16.inc.c"
};

// 0x05007EF0
ALIGNED8 static const Texture mad_piano_seg5_texture_05007EF0[] = {
#include "actors/mad_piano/mad_piano_keys.rgba16.inc.c"
};

// 0x050082F0
ALIGNED8 static const Texture mad_piano_seg5_texture_050082F0[] = {
#include "actors/mad_piano/mad_piano_keys_edge.rgba16.inc.c"
};

// 0x050086F0

// 0x05008708
static const Vtx mad_piano_seg5_vertex_05008708[] = {
#include "assets/rom/generated/6a0518e4cfb367801259.inc.c"









};

// 0x050087A8
static const Vtx mad_piano_seg5_vertex_050087A8[] = {
#include "assets/rom/generated/fdb759ebc7bbe6c2fb85.inc.c"














};

// 0x05008898
static const Vtx mad_piano_seg5_vertex_05008898[] = {
#include "assets/rom/generated/4aaffd009dd85eca0c0d.inc.c"














};

// 0x05008988
static const Vtx mad_piano_seg5_vertex_05008988[] = {
#include "assets/rom/generated/296451745f4c5c343cd3.inc.c"














};

// 0x05008A78
static const Vtx mad_piano_seg5_vertex_05008A78[] = {
#include "assets/rom/generated/dcafeebf405b3fc27b7e.inc.c"














};

// 0x05008B68 - 0x05008BD0
const Gfx mad_piano_seg5_dl_05008B68[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mad_piano_seg5_texture_050072F0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 32 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(mad_piano_seg5_vertex_05008708, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  2,  6, 0x0,  0,  7,  8, 0x0),
    gsSP2Triangles( 0,  8,  1, 0x0,  0,  9,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x05008BD0 - 0x05008CB0
const Gfx mad_piano_seg5_dl_05008BD0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mad_piano_seg5_texture_05006AF0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(mad_piano_seg5_vertex_050087A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(mad_piano_seg5_vertex_05008898, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(mad_piano_seg5_vertex_05008988, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(mad_piano_seg5_vertex_05008A78, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x05008CB0 - 0x05008D40
const Gfx mad_piano_seg5_dl_05008CB0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mad_piano_seg5_dl_05008B68),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mad_piano_seg5_dl_05008BD0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x05008D40

// 0x05008D58

// 0x05008D70

// 0x05008D88

// 0x05008DA0
static const Vtx mad_piano_seg5_vertex_05008DA0[] = {
#include "assets/rom/generated/70b8c79e6d05b947d94d.inc.c"












};

// 0x05008E70
static const Vtx mad_piano_seg5_vertex_05008E70[] = {
#include "assets/rom/generated/c2ac1451deed44387277.inc.c"














};

// 0x05008F60
static const Vtx mad_piano_seg5_vertex_05008F60[] = {
#include "assets/rom/generated/4e053fa655bbbeeaef5d.inc.c"














};

// 0x05009050
static const Vtx mad_piano_seg5_vertex_05009050[] = {
#include "assets/rom/generated/4d8390b226c1843e0c39.inc.c"















};

// 0x05009150
static const Vtx mad_piano_seg5_vertex_05009150[] = {
#include "assets/rom/generated/645660f53f63f6e70bf3.inc.c"














};

// 0x05009240
static const Vtx mad_piano_seg5_vertex_05009240[] = {
#include "assets/rom/generated/371547ef72ee2424fdea.inc.c"














};

// 0x05009330
static const Vtx mad_piano_seg5_vertex_05009330[] = {
#include "assets/rom/generated/f650ad3499023df815b9.inc.c"











};

// 0x050093F0
static const Vtx mad_piano_seg5_vertex_050093F0[] = {
#include "assets/rom/generated/078426b471e47492e30c.inc.c"



};

// 0x05009430
static const Vtx mad_piano_seg5_vertex_05009430[] = {
#include "assets/rom/generated/9aa4e630efd5089c6cea.inc.c"



};

// 0x05009470
static const Vtx mad_piano_seg5_vertex_05009470[] = {
#include "assets/rom/generated/ffbe3e230a8e38ec67f5.inc.c"







};

// 0x050094F0
static const Vtx mad_piano_seg5_vertex_050094F0[] = {
#include "assets/rom/generated/4bcc36f993a5b0586944.inc.c"









};

// 0x05009590 - 0x050096C8
const Gfx mad_piano_seg5_dl_05009590[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mad_piano_seg5_texture_050072F0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 32 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xccccccff),
    gsSPLightColor(LIGHT_2, 0x3d3d3dff),
    gsSPVertex(mad_piano_seg5_vertex_05008DA0, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11,  7,  9, 0x0),
    gsSP2Triangles(11,  9,  1, 0x0,  0, 12, 11, 0x0),
    gsSP1Triangle( 0, 11,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x4c4c4cff),
    gsSPVertex(mad_piano_seg5_vertex_05008E70, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(mad_piano_seg5_vertex_05008F60, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(mad_piano_seg5_vertex_05009050, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP2Triangles(10, 14, 13, 0x0,  0, 15,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x050096C8 - 0x050097B0
const Gfx mad_piano_seg5_dl_050096C8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mad_piano_seg5_texture_05007AF0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 32 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(mad_piano_seg5_vertex_05009150, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(mad_piano_seg5_vertex_05009240, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(mad_piano_seg5_vertex_05009330, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 4,  6,  8, 0x0,  4,  8,  9, 0x0),
    gsSP2Triangles( 4, 10, 11, 0x0,  4, 11,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x050097B0 - 0x050097F8
const Gfx mad_piano_seg5_dl_050097B0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mad_piano_seg5_texture_050082F0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x7f7f7fff),
    gsSPLightColor(LIGHT_2, 0x262626ff),
    gsSPVertex(mad_piano_seg5_vertex_050093F0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x050097F8 - 0x05009840
const Gfx mad_piano_seg5_dl_050097F8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mad_piano_seg5_texture_05007EF0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xccccccff),
    gsSPLightColor(LIGHT_2, 0x3d3d3dff),
    gsSPVertex(mad_piano_seg5_vertex_05009430, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x05009840 - 0x05009888
const Gfx mad_piano_seg5_dl_05009840[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mad_piano_seg5_texture_050076F0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(mad_piano_seg5_vertex_05009470, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x05009888 - 0x050098E8
const Gfx mad_piano_seg5_dl_05009888[] = {
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(mad_piano_seg5_vertex_050094F0, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 2,  4,  3, 0x0,  2,  5,  4, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  0,  7,  6, 0x0),
    gsSP2Triangles( 0,  8,  7, 0x0,  0,  9,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x050098E8 - 0x050099B8
const Gfx mad_piano_seg5_dl_050098E8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mad_piano_seg5_dl_05009590),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mad_piano_seg5_dl_050096C8),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mad_piano_seg5_dl_050097B0),
    gsSPDisplayList(mad_piano_seg5_dl_050097F8),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mad_piano_seg5_dl_05009840),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(mad_piano_seg5_dl_05009888),
    gsSPEndDisplayList(),
};
