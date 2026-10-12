// Lakitu (Cameraman)

// 0x06000000
UNUSED ALIGNED8 static const Texture lakitu_seg6_texture_06000000[] = {
#include "actors/lakitu_cameraman/lakitu_cameraman_cloud_face_unused.rgba16.inc.c"
};

// 0x06000800
ALIGNED8 static const Texture lakitu_seg6_texture_06000800[] = {
#include "actors/lakitu_cameraman/lakitu_cameraman_eyes_open.rgba16.inc.c"
};

// 0x06001800
ALIGNED8 static const Texture lakitu_seg6_texture_06001800[] = {
#include "actors/lakitu_cameraman/lakitu_cameraman_eyes_closed.rgba16.inc.c"
};

// 0x06002800
ALIGNED8 static const Texture lakitu_seg6_texture_06002800[] = {
#include "actors/lakitu_cameraman/lakitu_cameraman_shell.rgba16.inc.c"
};

// 0x06003000
ALIGNED8 static const Texture lakitu_seg6_texture_06003000[] = {
#include "actors/lakitu_cameraman/lakitu_cameraman_frown.rgba16.inc.c"
};

// 0x06003800
ALIGNED8 static const Texture lakitu_seg6_texture_06003800[] = {
#include "actors/lakitu_cameraman/lakitu_camera_lens.rgba16.inc.c"
};

// 0x06003A00

// 0x06003A18

// 0x06003A30
static const Vtx lakitu_seg6_vertex_06003A30[] = {
#include "assets/rom/generated/50d5b74a6304ee24cdc0.inc.c"















};

// 0x06003B30
static const Vtx lakitu_seg6_vertex_06003B30[] = {
#include "assets/rom/generated/8107dde00fa840206326.inc.c"









};

// 0x06003BD0
static const Vtx lakitu_seg6_vertex_06003BD0[] = {
#include "assets/rom/generated/d2dc1c8e6742957732a4.inc.c"










};

// 0x06003C80 - 0x06003DB0
const Gfx lakitu_seg6_dl_06003C80[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lakitu_seg6_texture_06002800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(lakitu_seg6_vertex_06003A30, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  8,  7,  1, 0x0),
    gsSP2Triangles( 6,  1,  7, 0x0,  5,  8,  9, 0x0),
    gsSP2Triangles( 5,  9, 10, 0x0,  3,  5, 10, 0x0),
    gsSP2Triangles( 6,  2,  1, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles( 8, 12, 11, 0x0,  8,  1, 12, 0x0),
    gsSP2Triangles(12,  1,  0, 0x0, 13, 11, 12, 0x0),
    gsSP2Triangles(13, 12,  0, 0x0, 14, 13,  0, 0x0),
    gsSP2Triangles( 0,  2, 14, 0x0, 14, 15, 13, 0x0),
    gsSPVertex(lakitu_seg6_vertex_06003B30, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  4,  2,  5, 0x0),
    gsSP2Triangles( 1,  5,  2, 0x0,  1,  6,  5, 0x0),
    gsSP2Triangles( 4,  5,  7, 0x0,  7,  5,  8, 0x0),
    gsSP2Triangles( 8,  5,  6, 0x0,  8,  6,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x06003DB0 - 0x06003E30
const Gfx lakitu_seg6_dl_06003DB0[] = {
    gsSPLightColor(LIGHT_1, 0xf2ab00ff),
    gsSPLightColor(LIGHT_2, 0x795500ff),
    gsSPVertex(lakitu_seg6_vertex_06003BD0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 1,  5,  2, 0x0,  4,  0,  2, 0x0),
    gsSP2Triangles( 5,  3,  2, 0x0,  4,  6,  0, 0x0),
    gsSP2Triangles( 7,  8,  1, 0x0,  9,  7,  0, 0x0),
    gsSP2Triangles( 1,  0,  7, 0x0,  8, 10,  1, 0x0),
    gsSP2Triangles( 1, 10,  5, 0x0,  0,  6,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x06003E30 - 0x06003E98
const Gfx lakitu_seg6_dl_06003E30[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(lakitu_seg6_dl_06003C80),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(lakitu_seg6_dl_06003DB0),
    gsSPEndDisplayList(),
};

// 0x06003E98

// 0x06003EB0
static const Vtx lakitu_seg6_vertex_06003EB0[] = {
#include "assets/rom/generated/d94c522c91bbc43a7385.inc.c"















};

// 0x06003FB0
static const Vtx lakitu_seg6_vertex_06003FB0[] = {
#include "assets/rom/generated/e3b880e2cf2626de288d.inc.c"














};

// 0x060040A0
static const Vtx lakitu_seg6_vertex_060040A0[] = {
#include "assets/rom/generated/a5e9fdbea89a8ca9b25b.inc.c"















};

// 0x060041A0
static const Vtx lakitu_seg6_vertex_060041A0[] = {
#include "assets/rom/generated/75c24485bb87d5107b0a.inc.c"














};

// 0x06004290
static const Vtx lakitu_seg6_vertex_06004290[] = {
#include "assets/rom/generated/f1eec70dccfe86583c96.inc.c"















};

// 0x06004390
static const Vtx lakitu_seg6_vertex_06004390[] = {
#include "assets/rom/generated/9e71c1a772a62d75381e.inc.c"







};

// 0x06004410 - 0x06004680
const Gfx lakitu_seg6_dl_06004410[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lakitu_seg6_texture_06003000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(lakitu_seg6_vertex_06003EB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  4, 0x0),
    gsSP2Triangles( 3,  5,  1, 0x0,  1,  6,  7, 0x0),
    gsSP2Triangles( 1,  5,  6, 0x0,  1,  0,  4, 0x0),
    gsSP2Triangles( 1,  7,  2, 0x0,  8,  7,  6, 0x0),
    gsSP2Triangles( 9,  7,  8, 0x0,  2,  7,  9, 0x0),
    gsSP2Triangles(10,  0,  2, 0x0,  2, 11, 10, 0x0),
    gsSP2Triangles( 2, 12, 11, 0x0,  2,  9, 12, 0x0),
    gsSP2Triangles( 9, 13, 12, 0x0,  9,  8, 13, 0x0),
    gsSP2Triangles(14,  5,  3, 0x0,  5, 15,  6, 0x0),
    gsSP1Triangle( 5, 14, 15, 0x0),
    gsSPVertex(lakitu_seg6_vertex_06003FB0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  6,  0,  3, 0x0),
    gsSP2Triangles( 0,  5,  1, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  6, 11, 0x0, 11,  6,  3, 0x0),
    gsSP2Triangles( 2, 11,  3, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(lakitu_seg6_vertex_060040A0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 2,  1,  5, 0x0,  4,  6,  2, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10,  7,  9, 0x0,  7, 12,  8, 0x0),
    gsSP2Triangles(13,  8, 12, 0x0,  9,  8, 14, 0x0),
    gsSP1Triangle(15,  8, 13, 0x0),
    gsSPVertex(lakitu_seg6_vertex_060041A0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 5,  6,  2, 0x0,  2,  6,  4, 0x0),
    gsSP2Triangles( 5,  2,  1, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle(14, 10, 13, 0x0),
    gsSPVertex(lakitu_seg6_vertex_06004290, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 1,  0,  5, 0x0,  0,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10,  7, 0x0),
    gsSP2Triangles( 3,  7, 10, 0x0,  9,  7,  6, 0x0),
    gsSP2Triangles(11, 12, 10, 0x0, 10, 13, 11, 0x0),
    gsSP2Triangles( 3, 10, 12, 0x0,  1, 14,  2, 0x0),
    gsSP1Triangle(13,  9, 15, 0x0),
    gsSPVertex(lakitu_seg6_vertex_06004390, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x06004680 - 0x060046E0
const Gfx lakitu_seg6_dl_06004680[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(lakitu_seg6_dl_06004410),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x060046E0

// 0x060046F8
static const Vtx lakitu_seg6_vertex_060046F8[] = {
#include "assets/rom/generated/c7a110cdf15fa81ff2ce.inc.c"














};

// 0x060047E8 - 0x060048D8
const Gfx lakitu_seg6_dl_060047E8[] = {
    gsSPLightColor(LIGHT_1, 0xf2ab00ff),
    gsSPLightColor(LIGHT_2, 0x795500ff),
    gsSPVertex(lakitu_seg6_vertex_060046F8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 6,  4,  8, 0x0,  3,  8,  4, 0x0),
    gsSP2Triangles( 9,  7,  6, 0x0, 10,  7,  9, 0x0),
    gsSP2Triangles( 5,  7, 10, 0x0,  3,  5, 11, 0x0),
    gsSP2Triangles( 5, 10, 11, 0x0,  1,  6,  2, 0x0),
    gsSP2Triangles( 2,  6,  8, 0x0,  9,  6,  1, 0x0),
    gsSP2Triangles( 1, 12,  9, 0x0, 12, 10,  9, 0x0),
    gsSP2Triangles( 1,  0, 12, 0x0, 11, 13,  3, 0x0),
    gsSP2Triangles(14,  0,  2, 0x0, 14,  2,  8, 0x0),
    gsSP2Triangles( 3, 14,  8, 0x0, 13,  0,  3, 0x0),
    gsSP2Triangles( 3,  0, 14, 0x0,  0, 13, 12, 0x0),
    gsSP2Triangles(10, 13, 11, 0x0, 12, 13, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x060048D8

// 0x060048F0
static const Vtx lakitu_seg6_vertex_060048F0[] = {
#include "assets/rom/generated/a3fbff5cf8d33e13e8b6.inc.c"














};

// 0x060049E0 - 0x06004AD0
const Gfx lakitu_seg6_dl_060049E0[] = {
    gsSPLightColor(LIGHT_1, 0xf2ab00ff),
    gsSPLightColor(LIGHT_2, 0x795500ff),
    gsSPVertex(lakitu_seg6_vertex_060048F0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  3,  7,  4, 0x0),
    gsSP2Triangles( 6,  1,  3, 0x0,  1,  7,  3, 0x0),
    gsSP2Triangles( 8,  7,  0, 0x0,  7,  8,  4, 0x0),
    gsSP2Triangles( 0,  7,  1, 0x0,  9,  4,  8, 0x0),
    gsSP2Triangles( 4,  9,  5, 0x0, 10,  8, 11, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0, 11,  8,  0, 0x0),
    gsSP2Triangles( 0,  2, 11, 0x0,  2, 12, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 12,  5, 0x0),
    gsSP2Triangles( 9, 10, 13, 0x0,  9, 13,  5, 0x0),
    gsSP2Triangles(10, 12, 13, 0x0,  1, 14,  2, 0x0),
    gsSP2Triangles( 2, 14, 12, 0x0,  6, 14,  1, 0x0),
    gsSP2Triangles( 5, 14,  6, 0x0,  5, 12, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x06004AD0

// 0x06004AE8
static const Vtx lakitu_seg6_vertex_06004AE8[] = {
#include "assets/rom/generated/268a08ad67bd16fee5b4.inc.c"











};

// 0x06004BA8 - 0x06004BE8
const Gfx lakitu_seg6_dl_06004BA8[] = {
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(lakitu_seg6_vertex_06004AE8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x06004BE8 - 0x06004C30
const Gfx lakitu_seg6_dl_06004BE8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPEndDisplayList(),
};

// 0x06004C30 - 0x06004C60
const Gfx lakitu_seg6_dl_06004C30[] = {
    gsSPDisplayList(lakitu_seg6_dl_06004BA8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x06004C60 - 0x06004C88
const Gfx lakitu_seg6_dl_06004C60[] = {
    gsSPDisplayList(lakitu_seg6_dl_06004BE8),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lakitu_seg6_texture_06000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(lakitu_seg6_dl_06004C30),
};

// 0x06004C88 - 0x06004CB0
const Gfx lakitu_seg6_dl_06004C88[] = {
    gsSPDisplayList(lakitu_seg6_dl_06004BE8),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lakitu_seg6_texture_06001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(lakitu_seg6_dl_06004C30),
};

// 0x06004CB0

// 0x06004CC8

// 0x06004CE0

// 0x06004CF8

// 0x06004D10
static const Vtx lakitu_seg6_vertex_06004D10[] = {
#include "assets/rom/generated/e95e31d6f6108ea42189.inc.c"



};

// 0x06004D50
static const Vtx lakitu_seg6_vertex_06004D50[] = {
#include "assets/rom/generated/0ab714552d594e049f6f.inc.c"














};

// 0x06004E40
static const Vtx lakitu_seg6_vertex_06004E40[] = {
#include "assets/rom/generated/12175a5377e797a67151.inc.c"














};

// 0x06004F30
static const Vtx lakitu_seg6_vertex_06004F30[] = {
#include "assets/rom/generated/71ca3b229ea7ea127b20.inc.c"





};

// 0x06004F90
static const Vtx lakitu_seg6_vertex_06004F90[] = {
#include "assets/rom/generated/99dba9cbca5698adf998.inc.c"













};

// 0x06005070
static const Vtx lakitu_seg6_vertex_06005070[] = {
#include "assets/rom/generated/853991163dc26e1a6d61.inc.c"













};

// 0x06005150
static const Vtx lakitu_seg6_vertex_06005150[] = {
#include "assets/rom/generated/df28eec2f816c6cf5fd8.inc.c"







};

// 0x060051D0 - 0x06005218
const Gfx lakitu_seg6_dl_060051D0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, lakitu_seg6_texture_06003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 16 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(lakitu_seg6_vertex_06004D10, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x06005218 - 0x06005360
const Gfx lakitu_seg6_dl_06005218[] = {
    gsSPLightColor(LIGHT_1, 0x191919ff),
    gsSPLightColor(LIGHT_2, 0xc0c0cff),
    gsSPVertex(lakitu_seg6_vertex_06004D50, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(lakitu_seg6_vertex_06004E40, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(lakitu_seg6_vertex_06004F30, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0x324440ff),
    gsSPLightColor(LIGHT_2, 0x192220ff),
    gsSPVertex(lakitu_seg6_vertex_06004F90, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  0,  7,  1, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(lakitu_seg6_vertex_06005070, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPLightColor(LIGHT_1, 0x303030ff),
    gsSPLightColor(LIGHT_2, 0x181818ff),
    gsSPVertex(lakitu_seg6_vertex_06005150, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  1,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x06005360 - 0x060053D8
const Gfx lakitu_seg6_dl_06005360[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_CLAMP, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(lakitu_seg6_dl_060051D0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(lakitu_seg6_dl_06005218),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// Unreferenced light group

// 0x060053F0

// Unreferenced light group

// Unreferenced light group

// 0x06005438

// Unreferenced light group

// 0x06005468
static const Vtx lakitu_seg6_vertex_06005468[] = {
#include "assets/rom/generated/7272f175bef0f796cff9.inc.c"












};

// 0x06005538
static const Vtx lakitu_seg6_vertex_06005538[] = {
#include "assets/rom/generated/02b1271943a99e6db511.inc.c"


};

// 0x06005568
static const Vtx lakitu_seg6_vertex_06005568[] = {
#include "assets/rom/generated/bb7d56020f98ef632a88.inc.c"


};

// 0x06005598 - 0x060055E8
const Gfx lakitu_seg6_dl_06005598[] = {
    gsSPLightColor(LIGHT_1, 0xa54f1bff),
    gsSPLightColor(LIGHT_2, 0x291306ff),
    gsSPVertex(lakitu_seg6_vertex_06005468, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  0,  2, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 1,  6,  2, 0x0, 10, 11, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x060055E8 - 0x06005610
const Gfx lakitu_seg6_dl_060055E8[] = {
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(lakitu_seg6_vertex_06005538, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x06005610 - 0x06005638
const Gfx lakitu_seg6_dl_06005610[] = {
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(lakitu_seg6_vertex_06005568, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};
