// Haunted Cage

// 0x0500C258

// 0x0500C270

// 0x0500C288
ALIGNED8 static const Texture haunted_cage_seg5_texture_0500C288[] = {
#include "actors/haunted_cage/bbh_cage_floor.rgba16.inc.c"
};

// 0x0500CA88
ALIGNED8 static const Texture haunted_cage_seg5_texture_0500CA88[] = {
#include "actors/haunted_cage/bbh_cage_double_ornament.rgba16.inc.c"
};

// 0x0500D288
ALIGNED8 static const Texture haunted_cage_seg5_texture_0500D288[] = {
#include "actors/haunted_cage/bbh_cage_ornament.rgba16.inc.c"
};

// 0x0500D688
ALIGNED8 static const Texture haunted_cage_seg5_texture_0500D688[] = {
#include "actors/haunted_cage/bbh_cage_wooden_base.rgba16.inc.c"
};

// 0x0500DA88
ALIGNED8 static const Texture haunted_cage_seg5_texture_0500DA88[] = {
#include "actors/haunted_cage/bbh_cage_bars.rgba16.inc.c"
};

// 0x0500EA88
static const Vtx haunted_cage_seg5_vertex_0500EA88[] = {
#include "assets/rom/generated/f7f4829aef3b4dd3f7ea.inc.c"














};

// 0x0500EB78
static const Vtx haunted_cage_seg5_vertex_0500EB78[] = {
#include "assets/rom/generated/b10ae931d7b610be37ca.inc.c"














};

// 0x0500EC68
static const Vtx haunted_cage_seg5_vertex_0500EC68[] = {
#include "assets/rom/generated/89eeb858ca190d1be57b.inc.c"





};

// 0x0500ECC8
static const Vtx haunted_cage_seg5_vertex_0500ECC8[] = {
#include "assets/rom/generated/2efb922d2e87377e54a1.inc.c"














};

// 0x0500EDB8
static const Vtx haunted_cage_seg5_vertex_0500EDB8[] = {
#include "assets/rom/generated/2f7f1214fe8fbe479574.inc.c"













};

// 0x0500EE98
static const Vtx haunted_cage_seg5_vertex_0500EE98[] = {
#include "assets/rom/generated/86722a7a347b449e3613.inc.c"














};

// 0x0500EF88
static const Vtx haunted_cage_seg5_vertex_0500EF88[] = {
#include "assets/rom/generated/97d151924f651a23142c.inc.c"







};

// 0x0500F008
static const Vtx haunted_cage_seg5_vertex_0500F008[] = {
#include "assets/rom/generated/29513061ca0c84b59903.inc.c"















};

// 0x0500F108
static const Vtx haunted_cage_seg5_vertex_0500F108[] = {
#include "assets/rom/generated/18ae20184534ce08e468.inc.c"















};

// 0x0500F208
static const Vtx haunted_cage_seg5_vertex_0500F208[] = {
#include "assets/rom/generated/be4fe92e1d85775c2a5e.inc.c"















};

// 0x0500F308
static const Vtx haunted_cage_seg5_vertex_0500F308[] = {
#include "assets/rom/generated/3d2fb72e791c4d6b6773.inc.c"















};

// 0x0500F408
static const Vtx haunted_cage_seg5_vertex_0500F408[] = {
#include "assets/rom/generated/f42756a48b38b08d904e.inc.c"











};

// 0x0500F4C8 - 0x0500F660
const Gfx haunted_cage_seg5_dl_0500F4C8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, haunted_cage_seg5_texture_0500D288),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x901e25ff),
    gsSPLightColor(LIGHT_2, 0x390c0eff),
    gsSPVertex(haunted_cage_seg5_vertex_0500EA88, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(haunted_cage_seg5_vertex_0500EB78, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(haunted_cage_seg5_vertex_0500EC68, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xb2b2b2ff),
    gsSPLightColor(LIGHT_2, 0x474747ff),
    gsSPVertex(haunted_cage_seg5_vertex_0500ECC8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  3,  9,  4, 0x0),
    gsSP2Triangles( 6, 10,  7, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(haunted_cage_seg5_vertex_0500EDB8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(haunted_cage_seg5_vertex_0500EE98, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(haunted_cage_seg5_vertex_0500EF88, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500F660 - 0x0500F760
const Gfx haunted_cage_seg5_dl_0500F660[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, haunted_cage_seg5_texture_0500CA88),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(haunted_cage_seg5_vertex_0500F008, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  3,  9,  4, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(haunted_cage_seg5_vertex_0500F108, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 13, 15, 14, 0x0),
    gsSPVertex(haunted_cage_seg5_vertex_0500F208, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  6, 15,  7, 0x0),
    gsSPVertex(haunted_cage_seg5_vertex_0500F308, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 15, 12, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500F760 - 0x0500F7D8
const Gfx haunted_cage_seg5_dl_0500F760[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, haunted_cage_seg5_texture_0500C288),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(haunted_cage_seg5_vertex_0500F408, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  0,  5,  3, 0x0),
    gsSP2Triangles( 0,  6,  7, 0x0,  0,  8,  6, 0x0),
    gsSP2Triangles( 0,  7,  9, 0x0,  0, 10,  8, 0x0),
    gsSP2Triangles( 0, 11, 10, 0x0,  0,  2, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500F7D8 - 0x0500F888
const Gfx haunted_cage_seg5_dl_0500F7D8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 4, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(haunted_cage_seg5_dl_0500F4C8),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(haunted_cage_seg5_dl_0500F660),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(haunted_cage_seg5_dl_0500F760),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x0500F888

// 0x0500F8A0
static const Vtx haunted_cage_seg5_vertex_0500F8A0[] = {
#include "assets/rom/generated/2ad7518d03ec9a368c13.inc.c"














};

// 0x0500F990
static const Vtx haunted_cage_seg5_vertex_0500F990[] = {
#include "assets/rom/generated/2c0ac462a3c846ecf531.inc.c"














};

// 0x0500FA80
static const Vtx haunted_cage_seg5_vertex_0500FA80[] = {
#include "assets/rom/generated/465a18d71f6587150416.inc.c"











};

// 0x0500FB40 - 0x0500FC28
const Gfx haunted_cage_seg5_dl_0500FB40[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, haunted_cage_seg5_texture_0500D688),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(haunted_cage_seg5_vertex_0500F8A0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(haunted_cage_seg5_vertex_0500F990, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(haunted_cage_seg5_vertex_0500FA80, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500FC28 - 0x0500FC98
const Gfx haunted_cage_seg5_dl_0500FC28[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 4, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(haunted_cage_seg5_dl_0500FB40),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x0500FC98

// 0x0500FCB0
static const Vtx haunted_cage_seg5_vertex_0500FCB0[] = {
#include "assets/rom/generated/e798f177fc3acfea90ab.inc.c"














};

// 0x0500FDA0
static const Vtx haunted_cage_seg5_vertex_0500FDA0[] = {
#include "assets/rom/generated/44e2d201b6d7edf70ca7.inc.c"













};

// 0x0500FE80
static const Vtx haunted_cage_seg5_vertex_0500FE80[] = {
#include "assets/rom/generated/c817963b62dad56613ac.inc.c"















};

// 0x0500FF80
static const Vtx haunted_cage_seg5_vertex_0500FF80[] = {
#include "assets/rom/generated/dc04dfd7badbc057fecc.inc.c"






};

// 0x0500FFF0 - 0x05010100
const Gfx haunted_cage_seg5_dl_0500FFF0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, haunted_cage_seg5_texture_0500DA88),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xb2b2b2ff),
    gsSPLightColor(LIGHT_2, 0x2c2c2cff),
    gsSPVertex(haunted_cage_seg5_vertex_0500FCB0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(haunted_cage_seg5_vertex_0500FDA0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(haunted_cage_seg5_vertex_0500FE80, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(haunted_cage_seg5_vertex_0500FF80, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 3,  5,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x05010100 - 0x05010170
const Gfx haunted_cage_seg5_dl_05010100[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(haunted_cage_seg5_dl_0500FFF0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
