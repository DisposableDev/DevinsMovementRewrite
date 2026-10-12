// Ukiki

// Unreferenced light group

// Unreferenced light group

// 0x05007BA0

// The hell?
UNUSED static const u64 ukiki_unused_1 = 1;

// 0x05007BC0
ALIGNED8 static const Texture ukiki_seg5_texture_05007BC0[] = {
#include "actors/ukiki/ukiki_face.rgba16.inc.c"
};

// 0x05008BC0
ALIGNED8 static const Texture ukiki_seg5_texture_05008BC0[] = {
#include "actors/ukiki/ukiki_face_blink.rgba16.inc.c"
};

// 0x05009BC0
ALIGNED8 static const Texture ukiki_seg5_texture_05009BC0[] = {
#include "actors/ukiki/ukiki_butt.rgba16.inc.c"
};

// 0x0500A3C0
ALIGNED8 static const Texture ukiki_seg5_texture_0500A3C0[] = {
#include "actors/ukiki/ukiki_fur.rgba16.inc.c"
};

// 0x0500ABC0

// 0x0500ABD8

// 0x0500ABF0
static const Vtx ukiki_seg5_vertex_0500ABF0[] = {
#include "assets/rom/generated/e36ff625b374e62926d4.inc.c"















};

// 0x0500ACF0
static const Vtx ukiki_seg5_vertex_0500ACF0[] = {
#include "assets/rom/generated/bbcf9d80bc054275f6b8.inc.c"















};

// 0x0500ADF0
static const Vtx ukiki_seg5_vertex_0500ADF0[] = {
#include "assets/rom/generated/b9c92aefbaccff83289e.inc.c"















};

// 0x0500AEF0
static const Vtx ukiki_seg5_vertex_0500AEF0[] = {
#include "assets/rom/generated/4c0d7cd90c2dc387d173.inc.c"






};

// 0x0500AF60
static const Vtx ukiki_seg5_vertex_0500AF60[] = {
#include "assets/rom/generated/56070e6ec624d6d019ba.inc.c"













};

// 0x0500B040 - 0x0500B1D8
const Gfx ukiki_seg5_dl_0500B040[] = {
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ukiki_seg5_vertex_0500ABF0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  5,  2,  4, 0x0),
    gsSP2Triangles( 2,  6,  0, 0x0,  7,  3,  8, 0x0),
    gsSP2Triangles( 3,  0,  9, 0x0,  3,  9,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9, 11,  8, 0x0),
    gsSP2Triangles( 0, 10,  9, 0x0, 12,  7,  8, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(ukiki_seg5_vertex_0500ACF0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  3, 0x0,  5,  4,  2, 0x0),
    gsSP2Triangles( 2,  7,  0, 0x0,  0,  8,  9, 0x0),
    gsSP2Triangles( 0, 10,  8, 0x0,  0,  7, 10, 0x0),
    gsSP2Triangles( 7,  2,  4, 0x0, 11, 12,  6, 0x0),
    gsSP2Triangles( 7,  6, 12, 0x0,  6,  7,  4, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(ukiki_seg5_vertex_0500ADF0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  7, 10, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 14, 11, 13, 0x0),
    gsSP2Triangles(15, 12, 11, 0x0, 13, 12,  3, 0x0),
    gsSP2Triangles( 5,  3, 12, 0x0, 12, 15,  5, 0x0),
    gsSPVertex(ukiki_seg5_vertex_0500AEF0, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  0,  5,  3, 0x0),
    gsSP2Triangles( 5,  0,  6, 0x0,  0,  2,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500B1D8 - 0x0500B278
const Gfx ukiki_seg5_dl_0500B1D8[] = {
    gsSPLightColor(LIGHT_1, 0xeecd8aff),
    gsSPLightColor(LIGHT_2, 0x776645ff),
    gsSPVertex(ukiki_seg5_vertex_0500AF60, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  4,  0, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 0,  4,  6, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 3,  0,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  8,  7, 0x0,  7, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 11, 0x0, 11, 13, 10, 0x0),
    gsSP2Triangles(10, 12,  9, 0x0, 10, 13, 12, 0x0),
    gsSP2Triangles(10,  9,  8, 0x0,  3,  6,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500B278 - 0x0500B2B8
const Gfx ukiki_seg5_dl_0500B278[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPEndDisplayList(),
};

// 0x0500B2B8 - 0x0500B2E8
const Gfx ukiki_seg5_dl_0500B2B8[] = {
    gsSPDisplayList(ukiki_seg5_dl_0500B040),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(ukiki_seg5_dl_0500B1D8),
    gsSPEndDisplayList(),
};

// 0x0500B2E8 - 0x0500B310
const Gfx ukiki_seg5_dl_0500B2E8[] = {
    gsSPDisplayList(ukiki_seg5_dl_0500B278),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_05007BC0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(ukiki_seg5_dl_0500B2B8),
};

// 0x0500B310 - 0x0500B338
const Gfx ukiki_seg5_dl_0500B310[] = {
    gsSPDisplayList(ukiki_seg5_dl_0500B278),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_05008BC0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(ukiki_seg5_dl_0500B2B8),
};

// 0x0500B338

// 0x0500B350
static const Vtx ukiki_seg5_vertex_0500B350[] = {
#include "assets/rom/generated/60d69d0dd5129335038e.inc.c"














};

// 0x0500B440
static const Vtx ukiki_seg5_vertex_0500B440[] = {
#include "assets/rom/generated/bc738ccb5ba18b1db880.inc.c"















};

// 0x0500B540
static const Vtx ukiki_seg5_vertex_0500B540[] = {
#include "assets/rom/generated/aa0a23620652431a001b.inc.c"













};

// 0x0500B620
static const Vtx ukiki_seg5_vertex_0500B620[] = {
#include "assets/rom/generated/67509f8cb2c76636b772.inc.c"



};

// 0x0500B660 - 0x0500B7E8
const Gfx ukiki_seg5_dl_0500B660[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_0500A3C0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ukiki_seg5_vertex_0500B350, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 3,  0,  4, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 3,  7,  5, 0x0,  3,  4,  7, 0x0),
    gsSP2Triangles( 8,  1,  3, 0x0,  3,  6,  8, 0x0),
    gsSP2Triangles( 9,  8,  6, 0x0,  5,  9,  6, 0x0),
    gsSP2Triangles( 2,  8, 10, 0x0,  2,  1,  8, 0x0),
    gsSP2Triangles(11, 10,  8, 0x0, 11,  8,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 11, 12, 14, 0x0),
    gsSP1Triangle(11,  9, 12, 0x0),
    gsSPVertex(ukiki_seg5_vertex_0500B440, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 4,  3,  5, 0x0,  3,  6,  5, 0x0),
    gsSP2Triangles( 0,  2,  3, 0x0,  2,  6,  3, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  7,  9, 10, 0x0),
    gsSP2Triangles(11,  8,  7, 0x0, 12,  2,  1, 0x0),
    gsSP2Triangles( 2, 12,  6, 0x0, 13, 12,  1, 0x0),
    gsSP2Triangles(14, 15,  1, 0x0,  0, 14,  1, 0x0),
    gsSPVertex(ukiki_seg5_vertex_0500B540, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  6,  5, 0x0),
    gsSP2Triangles( 7,  8,  6, 0x0,  9,  6,  8, 0x0),
    gsSP2Triangles(10,  8, 11, 0x0,  9,  8, 10, 0x0),
    gsSP1Triangle(11, 12, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500B7E8 - 0x0500B820
const Gfx ukiki_seg5_dl_0500B7E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_05009BC0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(ukiki_seg5_vertex_0500B620, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500B820 - 0x0500B888
const Gfx ukiki_seg5_dl_0500B820[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ukiki_seg5_dl_0500B660),
    gsSPDisplayList(ukiki_seg5_dl_0500B7E8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0500B888

// 0x0500B8A0
static const Vtx ukiki_seg5_vertex_0500B8A0[] = {
#include "assets/rom/generated/93d7ba006304bb8b8e73.inc.c"







};

// 0x0500B920 - 0x0500B988
const Gfx ukiki_seg5_dl_0500B920[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_0500A3C0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ukiki_seg5_vertex_0500B8A0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  4,  6,  5, 0x0),
    gsSP2Triangles( 0,  7,  3, 0x0,  7,  0,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500B988 - 0x0500B9E8
const Gfx ukiki_seg5_dl_0500B988[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ukiki_seg5_dl_0500B920),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0500B9E8

// 0x0500BA00
static const Vtx ukiki_seg5_vertex_0500BA00[] = {
#include "assets/rom/generated/598928a011c5169772a5.inc.c"








};

// 0x0500BA90 - 0x0500BB28
const Gfx ukiki_seg5_dl_0500BA90[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_0500A3C0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ukiki_seg5_vertex_0500BA00, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  4, 0x0),
    gsSP2Triangles( 1,  5,  6, 0x0,  1,  6,  2, 0x0),
    gsSP2Triangles( 3,  5,  1, 0x0,  4,  2,  7, 0x0),
    gsSP2Triangles( 7,  2,  6, 0x0,  3,  4,  7, 0x0),
    gsSP2Triangles( 3,  7,  8, 0x0,  7,  6,  8, 0x0),
    gsSP2Triangles( 5,  3,  8, 0x0,  5,  8,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500BB28 - 0x0500BB88
const Gfx ukiki_seg5_dl_0500BB28[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ukiki_seg5_dl_0500BA90),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0500BB88

// 0x0500BBA0
static const Vtx ukiki_seg5_vertex_0500BBA0[] = {
#include "assets/rom/generated/abf61faf8a086d834942.inc.c"






};

// 0x0500BC10 - 0x0500BC78
const Gfx ukiki_seg5_dl_0500BC10[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_0500A3C0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ukiki_seg5_vertex_0500BBA0, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 2,  4,  3, 0x0,  2,  5,  4, 0x0),
    gsSP2Triangles( 6,  3,  4, 0x0,  6,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500BC78 - 0x0500BCD8
const Gfx ukiki_seg5_dl_0500BC78[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ukiki_seg5_dl_0500BC10),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0500BCD8

// 0x0500BCF0
static const Vtx ukiki_seg5_vertex_0500BCF0[] = {
#include "assets/rom/generated/1c165bf75285708e1af8.inc.c"










};

// 0x0500BDA0 - 0x0500BE38
const Gfx ukiki_seg5_dl_0500BDA0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_0500A3C0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ukiki_seg5_vertex_0500BCF0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 0,  3,  5, 0x0,  0,  2,  6, 0x0),
    gsSP2Triangles( 0,  6,  4, 0x0,  2,  1,  7, 0x0),
    gsSP2Triangles( 2,  7,  6, 0x0,  3,  4,  8, 0x0),
    gsSP2Triangles( 9,  8,  4, 0x0,  5,  3,  8, 0x0),
    gsSP2Triangles( 5,  8, 10, 0x0,  9, 10,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500BE38 - 0x0500BE98
const Gfx ukiki_seg5_dl_0500BE38[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ukiki_seg5_dl_0500BDA0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0500BE98

// 0x0500BEB0
static const Vtx ukiki_seg5_vertex_0500BEB0[] = {
#include "assets/rom/generated/9f95847611b5c85b08f1.inc.c"










};

// 0x0500BF60 - 0x0500BFF8
const Gfx ukiki_seg5_dl_0500BF60[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_0500A3C0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ukiki_seg5_vertex_0500BEB0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  2,  5,  3, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 3,  5,  7, 0x0,  0,  3,  8, 0x0),
    gsSP2Triangles( 3,  9,  8, 0x0,  0, 10,  1, 0x0),
    gsSP2Triangles(10,  0,  8, 0x0, 10,  8,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500BFF8 - 0x0500C058
const Gfx ukiki_seg5_dl_0500BFF8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ukiki_seg5_dl_0500BF60),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0500C058

// 0x0500C070
static const Vtx ukiki_seg5_vertex_0500C070[] = {
#include "assets/rom/generated/aa4bbb994a5991c08381.inc.c"










};

// 0x0500C120 - 0x0500C1B8
const Gfx ukiki_seg5_dl_0500C120[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_0500A3C0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ukiki_seg5_vertex_0500C070, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 2,  1,  5, 0x0,  2,  5,  4, 0x0),
    gsSP2Triangles( 1,  6,  7, 0x0,  1,  7,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  4,  5,  7, 0x0),
    gsSP2Triangles( 3,  4,  8, 0x0,  4,  9,  8, 0x0),
    gsSP2Triangles(10,  3,  8, 0x0, 10,  8,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C1B8 - 0x0500C218
const Gfx ukiki_seg5_dl_0500C1B8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ukiki_seg5_dl_0500C120),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0500C218

// 0x0500C230
static const Vtx ukiki_seg5_vertex_0500C230[] = {
#include "assets/rom/generated/0d04375575b7ad57df0b.inc.c"









};

// 0x0500C2D0 - 0x0500C368
const Gfx ukiki_seg5_dl_0500C2D0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_0500A3C0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ukiki_seg5_vertex_0500C230, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 0,  3,  1, 0x0,  0,  2,  5, 0x0),
    gsSP2Triangles( 0,  5,  4, 0x0,  2,  6,  7, 0x0),
    gsSP2Triangles( 2,  7,  5, 0x0,  5,  7,  8, 0x0),
    gsSP2Triangles( 5,  9,  4, 0x0,  3,  4,  9, 0x0),
    gsSP2Triangles( 1,  3,  9, 0x0,  6,  8,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C368 - 0x0500C3C8
const Gfx ukiki_seg5_dl_0500C368[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ukiki_seg5_dl_0500C2D0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0500C3C8

// 0x0500C3E0
static const Vtx ukiki_seg5_vertex_0500C3E0[] = {
#include "assets/rom/generated/dce855c0974b6abcab41.inc.c"










};

// 0x0500C490 - 0x0500C528
const Gfx ukiki_seg5_dl_0500C490[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_0500A3C0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ukiki_seg5_vertex_0500C3E0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 2,  5,  3, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  1, 0x0),
    gsSP2Triangles( 1,  8,  9, 0x0,  1,  9,  5, 0x0),
    gsSP2Triangles( 8,  7, 10, 0x0,  8, 10,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C528 - 0x0500C588
const Gfx ukiki_seg5_dl_0500C528[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ukiki_seg5_dl_0500C490),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0500C588

// 0x0500C5A0
static const Vtx ukiki_seg5_vertex_0500C5A0[] = {
#include "assets/rom/generated/1ce31a5397b8759b4700.inc.c"










};

// 0x0500C650 - 0x0500C6E8
const Gfx ukiki_seg5_dl_0500C650[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_0500A3C0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ukiki_seg5_vertex_0500C5A0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 1,  5,  6, 0x0,  1,  6,  3, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  5,  7,  8, 0x0),
    gsSP2Triangles( 5,  8,  6, 0x0,  9,  6,  8, 0x0),
    gsSP2Triangles( 7,  9,  8, 0x0,  7, 10,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C6E8 - 0x0500C748
const Gfx ukiki_seg5_dl_0500C6E8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ukiki_seg5_dl_0500C650),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0500C748

// 0x0500C760
static const Vtx ukiki_seg5_vertex_0500C760[] = {
#include "assets/rom/generated/7683539655d8844c6646.inc.c"









};

// 0x0500C800 - 0x0500C898
const Gfx ukiki_seg5_dl_0500C800[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_0500A3C0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ukiki_seg5_vertex_0500C760, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  2,  4,  3, 0x0),
    gsSP2Triangles( 5,  6,  1, 0x0,  1,  6,  7, 0x0),
    gsSP2Triangles( 1,  7,  4, 0x0,  3,  4,  7, 0x0),
    gsSP2Triangles( 0,  3,  8, 0x0,  3,  7,  9, 0x0),
    gsSP2Triangles( 6,  5,  9, 0x0,  6,  9,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C898 - 0x0500C8F8
const Gfx ukiki_seg5_dl_0500C898[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ukiki_seg5_dl_0500C800),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0500C8F8

// 0x0500C910
static const Vtx ukiki_seg5_vertex_0500C910[] = {
#include "assets/rom/generated/148b34c7c06bf87ef64c.inc.c"









};

// 0x0500C9B0 - 0x0500CA48
const Gfx ukiki_seg5_dl_0500C9B0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ukiki_seg5_texture_0500A3C0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(ukiki_seg5_vertex_0500C910, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  2,  5,  3, 0x0),
    gsSP2Triangles( 0,  3,  6, 0x0,  3,  7,  8, 0x0),
    gsSP2Triangles( 3,  8,  6, 0x0,  0,  9,  1, 0x0),
    gsSP2Triangles( 1,  9,  8, 0x0,  1,  8,  7, 0x0),
    gsSP2Triangles( 9,  0,  6, 0x0,  9,  6,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500CA48 - 0x0500CAA8
const Gfx ukiki_seg5_dl_0500CA48[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(ukiki_seg5_dl_0500C9B0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0500CAA8
static const Vtx ukiki_seg5_vertex_0500CAA8[] = {
#include "assets/rom/generated/97ebf2e0e1032bccc73a.inc.c"















};

// 0x0500CBA8
static const Vtx ukiki_seg5_vertex_0500CBA8[] = {
#include "assets/rom/generated/5f9b2bab663499172b09.inc.c"







};

// 0x0500CC28
static const Vtx ukiki_seg5_vertex_0500CC28[] = {
#include "assets/rom/generated/7fab4caad647abcb37e1.inc.c"















};

// 0x0500CD28
static const Vtx ukiki_seg5_vertex_0500CD28[] = {
#include "assets/rom/generated/830dcd4cd3617a865e18.inc.c"







};

// 0x0500CDA8
static const Vtx ukiki_seg5_vertex_0500CDA8[] = {
#include "assets/rom/generated/7a9f3e7aec9c994c3d7e.inc.c"














};

// 0x0500CE98
static const Vtx ukiki_seg5_vertex_0500CE98[] = {
#include "assets/rom/generated/5be28e9133c1a23bd4c5.inc.c"












};

// 0x0500CF68 - 0x0500CFF0
const Gfx ukiki_seg5_dl_0500CF68[] = {
    gsSPLightColor(LIGHT_1, 0xe79361ff),
    gsSPLightColor(LIGHT_2, 0x392418ff),
    gsSPVertex(ukiki_seg5_vertex_0500CAA8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(ukiki_seg5_vertex_0500CBA8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500CFF0 - 0x0500D078
const Gfx ukiki_seg5_dl_0500CFF0[] = {
    gsSPLightColor(LIGHT_1, 0xe79361ff),
    gsSPLightColor(LIGHT_2, 0x392418ff),
    gsSPVertex(ukiki_seg5_vertex_0500CC28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(ukiki_seg5_vertex_0500CD28, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500D078 - 0x0500D108
const Gfx ukiki_seg5_dl_0500D078[] = {
    gsSPLightColor(LIGHT_1, 0xe79361ff),
    gsSPLightColor(LIGHT_2, 0x392418ff),
    gsSPVertex(ukiki_seg5_vertex_0500CDA8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  2,  7,  6, 0x0),
    gsSP2Triangles( 2,  6,  5, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 8, 10, 11, 0x0,  8, 11, 12, 0x0),
    gsSP2Triangles(13,  3,  6, 0x0, 13,  6,  7, 0x0),
    gsSP2Triangles( 4, 14,  2, 0x0,  4,  2,  5, 0x0),
    gsSP2Triangles( 1, 13,  7, 0x0,  1,  7,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500D108 - 0x0500D198
const Gfx ukiki_seg5_dl_0500D108[] = {
    gsSPLightColor(LIGHT_1, 0xe79361ff),
    gsSPLightColor(LIGHT_2, 0x392418ff),
    gsSPVertex(ukiki_seg5_vertex_0500CE98, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  0,  5, 0x0,  4,  5,  6, 0x0),
    gsSP2Triangles( 1,  7,  8, 0x0,  1,  8,  2, 0x0),
    gsSP2Triangles( 9, 10,  2, 0x0,  9,  2,  8, 0x0),
    gsSP2Triangles( 9,  8, 11, 0x0,  4,  7,  1, 0x0),
    gsSP2Triangles( 4,  1,  0, 0x0,  7,  4,  6, 0x0),
    gsSP2Triangles( 7,  6,  8, 0x0,  0,  3, 12, 0x0),
    gsSPEndDisplayList(),
};
