// 0x0700D850 - 0x0700D868

// 0x0700D868 - 0x0700D880

// 0x0700D880 - 0x0700D898

// 0x0700D898 - 0x0700D918
static const Vtx bbh_seg7_vertex_0700D898[] = {
#include "assets/rom/generated/d95de063e221d1576e65.inc.c"







};

// 0x0700D918 - 0x0700D9B8
static const Vtx bbh_seg7_vertex_0700D918[] = {
#include "assets/rom/generated/fa39361e996499ed19e7.inc.c"









};

// 0x0700D9B8 - 0x0700DA98
static const Vtx bbh_seg7_vertex_0700D9B8[] = {
#include "assets/rom/generated/7a574f7833270ea5a1a7.inc.c"













};

// 0x0700DA98 - 0x0700DB98
static const Vtx bbh_seg7_vertex_0700DA98[] = {
#include "assets/rom/generated/80874a836c6219e2423e.inc.c"















};

// 0x0700DB98 - 0x0700DC88
static const Vtx bbh_seg7_vertex_0700DB98[] = {
#include "assets/rom/generated/cdbeb941e6afb203ba59.inc.c"














};

// 0x0700DC88 - 0x0700DD78
static const Vtx bbh_seg7_vertex_0700DC88[] = {
#include "assets/rom/generated/2d7193b122b0bad4045d.inc.c"














};

// 0x0700DD78 - 0x0700DE68
static const Vtx bbh_seg7_vertex_0700DD78[] = {
#include "assets/rom/generated/07ddbeede607c0910683.inc.c"














};

// 0x0700DE68 - 0x0700DF58
static const Vtx bbh_seg7_vertex_0700DE68[] = {
#include "assets/rom/generated/79a01737ba841cdccc13.inc.c"














};

// 0x0700DF58 - 0x0700E058
static const Vtx bbh_seg7_vertex_0700DF58[] = {
#include "assets/rom/generated/dd2024b3f02a03b9c088.inc.c"















};

// 0x0700E058 - 0x0700E158
static const Vtx bbh_seg7_vertex_0700E058[] = {
#include "assets/rom/generated/e843a3c614a4c038d342.inc.c"















};

// 0x0700E158 - 0x0700E258
static const Vtx bbh_seg7_vertex_0700E158[] = {
#include "assets/rom/generated/420eef616f0a93ca2a31.inc.c"















};

// 0x0700E258 - 0x0700E338
static const Vtx bbh_seg7_vertex_0700E258[] = {
#include "assets/rom/generated/00d124f0a1f71c0b5d99.inc.c"













};

// 0x0700E338 - 0x0700E418
static const Vtx bbh_seg7_vertex_0700E338[] = {
#include "assets/rom/generated/bec963b263b6c2ffef50.inc.c"













};

// 0x0700E418 - 0x0700E508
static const Vtx bbh_seg7_vertex_0700E418[] = {
#include "assets/rom/generated/7865c7753317c5a08196.inc.c"














};

// 0x0700E508 - 0x0700E5F8
static const Vtx bbh_seg7_vertex_0700E508[] = {
#include "assets/rom/generated/74204c3c8e34a9c3cf71.inc.c"














};

// 0x0700E5F8 - 0x0700E6B8
static const Vtx bbh_seg7_vertex_0700E5F8[] = {
#include "assets/rom/generated/3225e39f6100d16d9403.inc.c"











};

// 0x0700E6B8 - 0x0700E7A8
static const Vtx bbh_seg7_vertex_0700E6B8[] = {
#include "assets/rom/generated/fd084ac8f2a71c02eb81.inc.c"














};

// 0x0700E7A8 - 0x0700E898
static const Vtx bbh_seg7_vertex_0700E7A8[] = {
#include "assets/rom/generated/82171d87cc6ff7b4d590.inc.c"














};

// 0x0700E898 - 0x0700E988
static const Vtx bbh_seg7_vertex_0700E898[] = {
#include "assets/rom/generated/00953d969c95dba1aaa7.inc.c"














};

// 0x0700E988 - 0x0700EA78
static const Vtx bbh_seg7_vertex_0700E988[] = {
#include "assets/rom/generated/9546eac455f0d02c1259.inc.c"














};

// 0x0700EA78 - 0x0700EB68
static const Vtx bbh_seg7_vertex_0700EA78[] = {
#include "assets/rom/generated/1a264c69579b498de2fc.inc.c"














};

// 0x0700EB68 - 0x0700EC48
static const Vtx bbh_seg7_vertex_0700EB68[] = {
#include "assets/rom/generated/3d7198059250c2abf08f.inc.c"













};

// 0x0700EC48 - 0x0700ED28
static const Vtx bbh_seg7_vertex_0700EC48[] = {
#include "assets/rom/generated/52ddc07ccb98ecfdd63a.inc.c"













};

// 0x0700ED28 - 0x0700EE28
static const Vtx bbh_seg7_vertex_0700ED28[] = {
#include "assets/rom/generated/75d9ae952d43d3b785ca.inc.c"















};

// 0x0700EE28 - 0x0700EE98
static const Vtx bbh_seg7_vertex_0700EE28[] = {
#include "assets/rom/generated/9ba5492b51bdfb0fb0a4.inc.c"






};

// 0x0700EE98 - 0x0700EFD8
static const Gfx bbh_seg7_dl_0700EE98[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09008800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x505050ff),
    gsSPLightColor(LIGHT_2, 0x202020ff),
    gsSPVertex(bbh_seg7_vertex_0700D898, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0x969696ff),
    gsSPLightColor(LIGHT_2, 0x3c3c3cff),
    gsSPVertex(bbh_seg7_vertex_0700D918, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  5,  3, 0x0,  0,  2,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(bbh_seg7_vertex_0700D9B8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 4,  5,  0, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700DA98, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0,  2, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700EFD8 - 0x0700F2B8
static const Gfx bbh_seg7_dl_0700EFD8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_0700DB98, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700DC88, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700DD78, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700DE68, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700DF58, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13,  9, 0x0, 12,  9, 11, 0x0),
    gsSP2Triangles( 9, 14, 10, 0x0, 15, 10, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700E058, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700E158, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0, 13,  9, 12, 0x0),
    gsSP2Triangles(14, 11, 10, 0x0, 14, 15, 11, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700E258, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700E338, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700E418, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0,  6,  8, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700E508, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700E5F8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700F2B8 - 0x0700F3D8
static const Gfx bbh_seg7_dl_0700F2B8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09005000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_0700E6B8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700E7A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700E898, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 7, 14,  8, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700E988, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 4,  6, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700F3D8 - 0x0700F510
static const Gfx bbh_seg7_dl_0700F3D8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_0700EA78, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 3, 14,  4, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700EB68, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  0,  9,  1, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700EC48, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700ED28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  0,  7,  1, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(bbh_seg7_vertex_0700EE28, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  2,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700F510 - 0x0700F5C8
const Gfx bbh_seg7_dl_0700F510[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_0700EE98),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_0700EFD8),
    gsSPDisplayList(bbh_seg7_dl_0700F2B8),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_0700F3D8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
