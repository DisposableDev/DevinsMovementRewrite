// Penguin

// 0x05002D80

// 0x05002D98

// 0x05002DB0

// 0x05002DC8

// 0x05002DE0
ALIGNED8 static const Texture penguin_seg5_texture_eye_open[] = {
#include "actors/penguin/penguin_eye_open.rgba16.inc.c"
};

// 0x050035E0
ALIGNED8 static const Texture penguin_seg5_texture_eye_half_closed[] = {
#include "actors/penguin/penguin_eye_half_closed.rgba16.inc.c"
};

// 0x05003DE0
ALIGNED8 static const Texture penguin_seg5_texture_eye_closed[] = {
#include "actors/penguin/penguin_eye_closed.rgba16.inc.c"
};

// 0x050045E0
ALIGNED8 static const Texture penguin_seg5_texture_eye_angry[] = {
#include "actors/penguin/penguin_eye_angry.rgba16.inc.c"
};

// 0x05004DE0
ALIGNED8 static const Texture penguin_seg5_texture_eye_sad[] = {
#include "actors/penguin/penguin_eye_angry_unused.rgba16.inc.c"
};

// 0x050055E0
ALIGNED8 static const Texture penguin_seg5_texture_beak[] = {
#include "actors/penguin/penguin_beak.rgba16.inc.c"
};

// 0x05005DE0

// 0x05005DF8

// 0x05005E10

// 0x05005E28
static const Vtx penguin_seg5_vertex_eyes[] = {
#include "assets/rom/generated/d9b36d34a29a6c846da2.inc.c"







};

// 0x05005EA8
static const Vtx penguin_seg5_vertex_beak_top[] = {
#include "assets/rom/generated/ad0038b74354b87f88e1.inc.c"



};

// 0x05005EE8
static const Vtx penguin_seg5_vertex_05005EE8[] = {
#include "assets/rom/generated/d63e5cdf55398af0dbef.inc.c"















};

// 0x05005FE8
static const Vtx penguin_seg5_vertex_05005FE8[] = {
#include "assets/rom/generated/68df810e3bb487beab39.inc.c"















};

// 0x050060E8
static const Vtx penguin_seg5_vertex_050060E8[] = {
#include "assets/rom/generated/0f84aac24b5bde057ed6.inc.c"






};

// 0x05006158
static const Vtx penguin_seg5_vertex_beak_bottom[] = {
#include "assets/rom/generated/e47e9214e538cd518f1e.inc.c"


};

// 0x05006188 - 0x050061C8
const Gfx penguin_seg5_sub_dl_eyes_model[] = {
    gsSPLightColor(LIGHT_1, 0xb26a4ff),
    gsSPLightColor(LIGHT_2, 0x40f41ff),
    gsSPVertex(penguin_seg5_vertex_eyes, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  6,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x050061C8 - 0x050061F8
const Gfx penguin_seg5_sub_dl_beak_model[] = {
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(penguin_seg5_vertex_beak_top, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x050061F8 - 0x05006380
const Gfx penguin_seg5_sub_dl_head_model[] = {
    gsSPLightColor(LIGHT_1, 0xb26a4ff),
    gsSPLightColor(LIGHT_2, 0x40f41ff),
    gsSPVertex(penguin_seg5_vertex_05005EE8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  1,  5, 0x0,  1,  4,  6, 0x0),
    gsSP2Triangles( 6,  3,  1, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 7,  8,  4, 0x0,  4,  5,  9, 0x0),
    gsSP2Triangles( 4,  8,  6, 0x0,  9,  7,  4, 0x0),
    gsSP2Triangles( 6,  8, 10, 0x0, 10,  3,  6, 0x0),
    gsSP2Triangles(11,  8,  7, 0x0, 12,  2,  3, 0x0),
    gsSP2Triangles(13, 11,  7, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(penguin_seg5_vertex_05005FE8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  5,  6,  2, 0x0),
    gsSP2Triangles( 2,  6,  3, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 3,  6,  7, 0x0,  8,  9,  3, 0x0),
    gsSP2Triangles( 7,  8,  3, 0x0,  0,  3,  9, 0x0),
    gsSP2Triangles( 5,  4, 10, 0x0, 11,  0,  9, 0x0),
    gsSP2Triangles(11,  1,  0, 0x0, 12,  1, 11, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0, 11, 14, 12, 0x0),
    gsSP1Triangle(15, 12, 14, 0x0),
    gsSPVertex(penguin_seg5_vertex_050060E8, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  5,  2,  3, 0x0),
    gsSP2Triangles( 2,  5,  4, 0x0,  4,  5,  6, 0x0),
    gsSPLightColor(LIGHT_1, 0xf2d51cff),
    gsSPLightColor(LIGHT_2, 0x60550bff),
    gsSPVertex(penguin_seg5_vertex_beak_bottom, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x05006380 - 0x050063C8
const Gfx penguin_seg5_sub_dl_head_begin[] = {
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsSPEndDisplayList(),
};

// 0x050063C8 - 0x05006428
const Gfx penguin_seg5_dl_head[] = {
    gsSPDisplayList(penguin_seg5_sub_dl_eyes_model),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, penguin_seg5_texture_beak),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(penguin_seg5_sub_dl_beak_model),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(penguin_seg5_sub_dl_head_model),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x05006428 - 0x05006458
const Gfx penguin_seg5_dl_head_eyes_open[] = {
    gsSPDisplayList(penguin_seg5_sub_dl_head_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, penguin_seg5_texture_eye_open),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(penguin_seg5_dl_head),
    gsSPEndDisplayList(),
};

// 0x05006458 - 0x05006488
const Gfx penguin_seg5_dl_head_eyes_half_closed[] = {
    gsSPDisplayList(penguin_seg5_sub_dl_head_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, penguin_seg5_texture_eye_half_closed),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(penguin_seg5_dl_head),
    gsSPEndDisplayList(),
};

// 0x05006488 - 0x050064B8
const Gfx penguin_seg5_dl_head_eyes_closed[] = {
    gsSPDisplayList(penguin_seg5_sub_dl_head_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, penguin_seg5_texture_eye_closed),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(penguin_seg5_dl_head),
    gsSPEndDisplayList(),
};

// 0x050064B8 - 0x050064E8
const Gfx penguin_seg5_dl_head_eyes_angry[] = {
    gsSPDisplayList(penguin_seg5_sub_dl_head_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, penguin_seg5_texture_eye_angry),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(penguin_seg5_dl_head),
    gsSPEndDisplayList(),
};

// 0x050064E8 - 0x05006518
const Gfx penguin_seg5_dl_head_eyes_sad[] = {
    gsSPDisplayList(penguin_seg5_sub_dl_head_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, penguin_seg5_texture_eye_sad),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(penguin_seg5_dl_head),
    gsSPEndDisplayList(),
};

// 0x05006518
static const Vtx penguin_seg5_vertex_05006518[] = {
#include "assets/rom/generated/3ad792eb2025dabbdd7a.inc.c"









};

// 0x050065B8
static const Vtx penguin_seg5_vertex_050065B8[] = {
#include "assets/rom/generated/41c8ec4cdcf2aecb502a.inc.c"









};

// 0x05006658
static const Vtx penguin_seg5_vertex_05006658[] = {
#include "assets/rom/generated/0af1f1489ae8d3cde4ff.inc.c"














};

// 0x05006748
static const Vtx penguin_seg5_vertex_05006748[] = {
#include "assets/rom/generated/d72efc1409c94385411f.inc.c"














};

// 0x05006838
static const Vtx penguin_seg5_vertex_05006838[] = {
#include "assets/rom/generated/7f308e63c2c5d38cc440.inc.c"




};

// 0x05006888
static const Vtx penguin_seg5_vertex_05006888[] = {
#include "assets/rom/generated/dbc02d2fcfc426c8f40d.inc.c"















};

// 0x05006988
static const Vtx penguin_seg5_vertex_05006988[] = {
#include "assets/rom/generated/436b28c38d9a71d1d8eb.inc.c"














};

// 0x05006A78
static const Vtx penguin_seg5_vertex_05006A78[] = {
#include "assets/rom/generated/8313b07e86dccad0063a.inc.c"














};

// 0x05006B68
static const Vtx penguin_seg5_vertex_05006B68[] = {
#include "assets/rom/generated/cb6b9f57eb09b4f0ecd6.inc.c"















};

// 0x05006C68
static const Vtx penguin_seg5_vertex_05006C68[] = {
#include "assets/rom/generated/d4408402c3832287521c.inc.c"















};

// 0x05006D68
static const Vtx penguin_seg5_vertex_05006D68[] = {
#include "assets/rom/generated/556f5a7d0a8924145db3.inc.c"







};

// 0x05006DE8
static const Vtx penguin_seg5_vertex_05006DE8[] = {
#include "assets/rom/generated/f44de11e5aaee75454da.inc.c"



};

// 0x05006E28
static const Vtx penguin_seg5_vertex_05006E28[] = {
#include "assets/rom/generated/ea8993443808aa3d8fca.inc.c"














};

// 0x05006F18
static const Vtx penguin_seg5_vertex_05006F18[] = {
#include "assets/rom/generated/b31ada32d1cb023adb2d.inc.c"















};

// 0x05007018
static const Vtx penguin_seg5_vertex_05007018[] = {
#include "assets/rom/generated/3a9b927e1281033e013e.inc.c"








};

// 0x050070A8
static const Vtx penguin_seg5_vertex_050070A8[] = {
#include "assets/rom/generated/3bfe642ca9aaa881c661.inc.c"














};

// 0x05007198 - 0x050071E8
const Gfx penguin_seg5_dl_05007198[] = {
    gsSPLightColor(LIGHT_1, 0xf0f90ff),
    gsSPLightColor(LIGHT_2, 0x60639ff),
    gsSPVertex(penguin_seg5_vertex_05006518, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  3, 0x0,  3,  6,  7, 0x0),
    gsSP2Triangles( 8,  0,  2, 0x0,  0,  9,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x050071E8 - 0x05007238
const Gfx penguin_seg5_dl_050071E8[] = {
    gsSPLightColor(LIGHT_1, 0xf0f90ff),
    gsSPLightColor(LIGHT_2, 0x60639ff),
    gsSPVertex(penguin_seg5_vertex_050065B8, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  7,  6,  8, 0x0),
    gsSP2Triangles( 8,  9,  7, 0x0,  3,  0,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x05007238 - 0x050072C8
const Gfx penguin_seg5_dl_05007238[] = {
    gsSPLightColor(LIGHT_1, 0xce841aff),
    gsSPLightColor(LIGHT_2, 0x52340aff),
    gsSPVertex(penguin_seg5_vertex_05006658, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  7,  2, 0x0),
    gsSP2Triangles( 1,  4,  6, 0x0,  1,  6,  2, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  1,  0,  4, 0x0),
    gsSP2Triangles( 8, 10, 11, 0x0,  2, 12,  0, 0x0),
    gsSP2Triangles( 0, 12,  3, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles(10, 13, 11, 0x0,  9, 14, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x050072C8 - 0x05007358
const Gfx penguin_seg5_dl_050072C8[] = {
    gsSPLightColor(LIGHT_1, 0xce841aff),
    gsSPLightColor(LIGHT_2, 0x52340aff),
    gsSPVertex(penguin_seg5_vertex_05006748, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  5, 11, 0x0,  8, 12,  9, 0x0),
    gsSP2Triangles( 6,  2, 11, 0x0,  2, 10, 11, 0x0),
    gsSP2Triangles( 6,  0,  2, 0x0,  2,  1, 10, 0x0),
    gsSP2Triangles(10,  3,  5, 0x0,  6, 11,  5, 0x0),
    gsSP2Triangles( 8, 13, 12, 0x0,  7, 14,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x05007358 - 0x05007540
const Gfx penguin_seg5_dl_05007358[] = {
    gsSPLightColor(LIGHT_1, 0xdfa734ff),
    gsSPLightColor(LIGHT_2, 0x594214ff),
    gsSPVertex(penguin_seg5_vertex_05006838, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP1Triangle( 1,  4,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0xf0f90ff),
    gsSPLightColor(LIGHT_2, 0x60639ff),
    gsSPVertex(penguin_seg5_vertex_05006888, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  0, 0x0),
    gsSP2Triangles(10, 11,  8, 0x0, 12, 13, 10, 0x0),
    gsSP2Triangles( 9,  4,  1, 0x0,  3, 14, 15, 0x0),
    gsSPVertex(penguin_seg5_vertex_05006988, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13,  7, 0x0, 14,  0,  2, 0x0),
    gsSPVertex(penguin_seg5_vertex_05006A78, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  7, 0x0,  8,  4,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 10, 14, 0x0),
    gsSPVertex(penguin_seg5_vertex_05006B68, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles(11,  1,  6, 0x0, 12,  2, 13, 0x0),
    gsSP2Triangles( 3,  7,  4, 0x0, 14, 15,  1, 0x0),
    gsSPVertex(penguin_seg5_vertex_05006C68, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  2, 10, 0x0),
    gsSP2Triangles( 4,  7, 11, 0x0,  8,  4,  3, 0x0),
    gsSP2Triangles( 0,  9,  6, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle(15, 12,  1, 0x0),
    gsSPVertex(penguin_seg5_vertex_05006D68, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  7,  4, 0x0),
    gsSPLightColor(LIGHT_1, 0xf0f90ff),
    gsSPLightColor(LIGHT_2, 0x60639ff),
    gsSPVertex(penguin_seg5_vertex_05006DE8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x05007540 - 0x05007708
const Gfx penguin_seg5_dl_05007540[] = {
    gsSPLightColor(LIGHT_1, 0xf0f90ff),
    gsSPLightColor(LIGHT_2, 0x60639ff),
    gsSPVertex(penguin_seg5_vertex_05006E28, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  2,  9,  6, 0x0),
    gsSP2Triangles( 5, 10, 11, 0x0,  4, 10,  5, 0x0),
    gsSP2Triangles( 8, 12, 13, 0x0,  5, 11,  1, 0x0),
    gsSP2Triangles( 6,  0,  2, 0x0, 10,  9,  2, 0x0),
    gsSP1Triangle( 3, 14,  4, 0x0),
    gsSPVertex(penguin_seg5_vertex_05006F18, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10,  1, 0x0),
    gsSP2Triangles(11,  8, 12, 0x0, 13,  5,  4, 0x0),
    gsSP2Triangles( 0, 14, 15, 0x0,  2, 11, 12, 0x0),
    gsSP2Triangles( 6, 11,  2, 0x0,  1,  0,  5, 0x0),
    gsSP2Triangles( 1, 10,  6, 0x0,  6,  2,  1, 0x0),
    gsSP2Triangles( 5, 13,  1, 0x0,  2, 12, 14, 0x0),
    gsSPVertex(penguin_seg5_vertex_05007018, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  2,  8, 0x0),
    gsSPLightColor(LIGHT_1, 0xfffffdff),
    gsSPLightColor(LIGHT_2, 0x666665ff),
    gsSPVertex(penguin_seg5_vertex_050070A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  4, 0x0),
    gsSP2Triangles( 5,  2,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0,  9,  2,  7, 0x0),
    gsSP2Triangles( 4,  1,  0, 0x0,  7,  3,  8, 0x0),
    gsSP2Triangles( 8, 11, 12, 0x0, 13,  9, 10, 0x0),
    gsSP2Triangles( 2,  1,  7, 0x0, 13, 10, 14, 0x0),
    gsSP2Triangles(14, 10,  8, 0x0,  2,  5,  0, 0x0),
    gsSP2Triangles(13,  6,  2, 0x0,  3,  7,  1, 0x0),
    gsSP2Triangles(11,  8,  3, 0x0, 12, 14,  8, 0x0),
    gsSP1Triangle( 2,  9, 13, 0x0),
    gsSPEndDisplayList(),
};
