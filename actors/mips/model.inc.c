// Mips

// 0x0600FB80
ALIGNED8 static const Texture mips_seg6_texture_0600FB80[] = {
#include "actors/mips/mips_eyes.rgba16.inc.c"
};

// 0x06010380

// 0x06010398

// 0x060103B0
static const Vtx mips_seg6_vertex_060103B0[] = {
#include "assets/rom/generated/17e6b609276272f20a59.inc.c"















};

// 0x060104B0
static const Vtx mips_seg6_vertex_060104B0[] = {
#include "assets/rom/generated/c89481b4c2ce5048c3bf.inc.c"












};

// 0x06010580
static const Vtx mips_seg6_vertex_06010580[] = {
#include "assets/rom/generated/296b5e2cbff2b5405747.inc.c"



};

// 0x060105C0
static const Vtx mips_seg6_vertex_060105C0[] = {
#include "assets/rom/generated/eb5cd74d7d81f0f4306c.inc.c"



};

// 0x06010600 - 0x06010748
const Gfx mips_seg6_dl_06010600[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mips_seg6_texture_0600FB80),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(mips_seg6_vertex_060103B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  2,  5,  3, 0x0),
    gsSP2Triangles( 2,  6,  5, 0x0,  2,  4,  6, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  3,  5, 0x0),
    gsSP2Triangles( 5,  9,  8, 0x0,  9,  5,  7, 0x0),
    gsSP2Triangles( 3, 10,  0, 0x0,  8, 10,  3, 0x0),
    gsSP2Triangles( 6, 11,  7, 0x0,  4, 11,  6, 0x0),
    gsSP2Triangles(12, 11,  4, 0x0,  4,  1, 12, 0x0),
    gsSP2Triangles( 8,  9, 13, 0x0, 14, 10,  8, 0x0),
    gsSP2Triangles( 8, 15, 14, 0x0, 13, 15,  8, 0x0),
    gsSPVertex(mips_seg6_vertex_060104B0, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 2,  4,  0, 0x0,  5,  6,  1, 0x0),
    gsSP2Triangles( 5,  1,  7, 0x0,  7,  1,  3, 0x0),
    gsSP2Triangles( 5,  8,  6, 0x0,  6,  9,  2, 0x0),
    gsSP2Triangles( 2,  9, 10, 0x0,  2, 10,  4, 0x0),
    gsSP2Triangles(11,  5,  7, 0x0,  9, 12, 10, 0x0),
    gsSP1Triangle( 6,  8,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x06010748 - 0x060107B8
const Gfx mips_seg6_dl_06010748[] = {
    gsSPLightColor(LIGHT_1, 0x27210bff),
    gsSPLightColor(LIGHT_2, 0xf0d04ff),
    gsSPVertex(mips_seg6_vertex_06010580, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP1Triangle( 3,  2,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(mips_seg6_vertex_060105C0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  2,  0, 0x0,  2,  1,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x060107B8 - 0x06010820
const Gfx mips_seg6_dl_060107B8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mips_seg6_dl_06010600),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(mips_seg6_dl_06010748),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x06010820

// 0x06010838
static const Vtx mips_seg6_vertex_06010838[] = {
#include "assets/rom/generated/28f23ade9523eaf15c4c.inc.c"














};

// 0x06010928
static const Vtx mips_seg6_vertex_06010928[] = {
#include "assets/rom/generated/1d4b25b50a2226cd744c.inc.c"














};

// 0x06010A18
static const Vtx mips_seg6_vertex_06010A18[] = {
#include "assets/rom/generated/de00552d2e5534edeb1d.inc.c"







};

// 0x06010A98 - 0x06010C40
const Gfx mips_seg6_dl_06010A98[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mips_seg6_texture_0600FB80),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(mips_seg6_vertex_06010838, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 0,  5,  1, 0x0,  1,  6,  3, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  7,  1,  5, 0x0),
    gsSP2Triangles( 6,  1,  7, 0x0,  8,  6,  9, 0x0),
    gsSP2Triangles( 5,  9,  6, 0x0,  8,  3,  6, 0x0),
    gsSP2Triangles( 7, 10,  6, 0x0,  5,  6, 10, 0x0),
    gsSP2Triangles( 8, 11,  3, 0x0, 11,  4,  3, 0x0),
    gsSP2Triangles( 9,  5, 12, 0x0,  8,  9, 13, 0x0),
    gsSP2Triangles( 9, 12, 13, 0x0, 11,  2,  4, 0x0),
    gsSP1Triangle(12, 14, 13, 0x0),
    gsSPVertex(mips_seg6_vertex_06010928, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  4,  5,  6, 0x0),
    gsSP2Triangles( 7,  6,  5, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles(11, 12,  8, 0x0,  9,  8, 12, 0x0),
    gsSP2Triangles( 8, 13, 11, 0x0, 13,  8, 10, 0x0),
    gsSP2Triangles(13,  1,  3, 0x0, 10,  2, 13, 0x0),
    gsSP2Triangles( 3, 11, 13, 0x0,  2,  1, 13, 0x0),
    gsSP2Triangles(10,  9,  5, 0x0,  5,  4, 10, 0x0),
    gsSP2Triangles(10,  4,  2, 0x0,  5, 14,  7, 0x0),
    gsSP1Triangle(12, 14,  9, 0x0),
    gsSPVertex(mips_seg6_vertex_06010A18, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  2,  7,  3, 0x0),
    gsSP2Triangles( 1,  7,  2, 0x0,  3,  7,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x06010C40 - 0x06010CA0
const Gfx mips_seg6_dl_06010C40[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mips_seg6_dl_06010A98),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x06010CA0

// 0x06010CB8

// 0x06010CD0
static const Vtx mips_seg6_vertex_06010CD0[] = {
#include "assets/rom/generated/269cec802a2ec3f30ba1.inc.c"





};

// 0x06010D30
static const Vtx mips_seg6_vertex_06010D30[] = {
#include "assets/rom/generated/5ddf898468eed82cd765.inc.c"







};

// 0x06010DB0 - 0x06010E60
const Gfx mips_seg6_dl_06010DB0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mips_seg6_texture_0600FB80),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x969600ff),
    gsSPLightColor(LIGHT_2, 0x3c3c00ff),
    gsSPVertex(mips_seg6_vertex_06010CD0, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  4,  0,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(mips_seg6_vertex_06010D30, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  5,  6,  0, 0x0),
    gsSP2Triangles( 0,  6,  3, 0x0,  0,  4,  5, 0x0),
    gsSP2Triangles( 2,  7,  4, 0x0,  7,  5,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x06010E60 - 0x06010EC0
const Gfx mips_seg6_dl_06010E60[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mips_seg6_dl_06010DB0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x06010EC0

// 0x06010ED8

// 0x06010EF0

// 0x06010F08
static const Vtx mips_seg6_vertex_06010F08[] = {
#include "assets/rom/generated/8c9e2fae884fc155496f.inc.c"




};

// 0x06010F58
static const Vtx mips_seg6_vertex_06010F58[] = {
#include "assets/rom/generated/94e00d8ac6d0d73c895a.inc.c"


};

// 0x06010F88
static const Vtx mips_seg6_vertex_06010F88[] = {
#include "assets/rom/generated/b43ee7166ee53bb8ceb3.inc.c"






};

// 0x06010FF8 - 0x060110B0
const Gfx mips_seg6_dl_06010FF8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mips_seg6_texture_0600FB80),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x969600ff),
    gsSPLightColor(LIGHT_2, 0x3c3c00ff),
    gsSPVertex(mips_seg6_vertex_06010F08, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP1Triangle( 3,  2,  4, 0x0),
    gsSPLightColor(LIGHT_1, 0x858e00ff),
    gsSPLightColor(LIGHT_2, 0x353800ff),
    gsSPVertex(mips_seg6_vertex_06010F58, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(mips_seg6_vertex_06010F88, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  2,  1,  4, 0x0),
    gsSP2Triangles( 0,  5,  1, 0x0,  3,  6,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x060110B0 - 0x06011110
const Gfx mips_seg6_dl_060110B0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mips_seg6_dl_06010FF8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x06011110

// 0x06011128

// 0x06011140
static const Vtx mips_seg6_vertex_06011140[] = {
#include "assets/rom/generated/4b156bf653ad5a315c97.inc.c"





};

// 0x060111A0
static const Vtx mips_seg6_vertex_060111A0[] = {
#include "assets/rom/generated/3519d5aa5fe28330528e.inc.c"









};

// 0x06011240 - 0x06011310
const Gfx mips_seg6_dl_06011240[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mips_seg6_texture_0600FB80),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x969600ff),
    gsSPLightColor(LIGHT_2, 0x3c3c00ff),
    gsSPVertex(mips_seg6_vertex_06011140, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 2,  4,  3, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(mips_seg6_vertex_060111A0, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 0,  4,  3, 0x0,  5,  6,  1, 0x0),
    gsSP2Triangles( 1,  3,  5, 0x0,  2,  1,  6, 0x0),
    gsSP2Triangles( 6,  7,  2, 0x0,  5,  3,  4, 0x0),
    gsSP2Triangles( 4,  8,  5, 0x0,  8,  9,  5, 0x0),
    gsSP2Triangles( 9,  6,  5, 0x0,  9,  7,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011310 - 0x06011370
const Gfx mips_seg6_dl_06011310[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mips_seg6_dl_06011240),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x06011370

// 0x06011388

// 0x060113A0
static const Vtx mips_seg6_vertex_060113A0[] = {
#include "assets/rom/generated/7b86c3265d4258892622.inc.c"




};

// 0x060113F0
static const Vtx mips_seg6_vertex_060113F0[] = {
#include "assets/rom/generated/3b25db745de197c82e2d.inc.c"







};

// 0x06011470 - 0x06011520
const Gfx mips_seg6_dl_06011470[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mips_seg6_texture_0600FB80),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x969600ff),
    gsSPLightColor(LIGHT_2, 0x3c3c00ff),
    gsSPVertex(mips_seg6_vertex_060113A0, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP1Triangle( 3,  4,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(mips_seg6_vertex_060113F0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 3,  2,  1, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 6,  4,  3, 0x0,  1,  5,  3, 0x0),
    gsSP2Triangles( 6,  7,  4, 0x0,  4,  7,  0, 0x0),
    gsSP1Triangle( 0,  2,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011520 - 0x06011580
const Gfx mips_seg6_dl_06011520[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mips_seg6_dl_06011470),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x06011580

// 0x06011598

// 0x060115B0
static const Vtx mips_seg6_vertex_060115B0[] = {
#include "assets/rom/generated/06d2ae598464c1237359.inc.c"





};

// 0x06011610
static const Vtx mips_seg6_vertex_06011610[] = {
#include "assets/rom/generated/2b24bdea78ae2d2b4b05.inc.c"









};

// 0x060116B0 - 0x06011780
const Gfx mips_seg6_dl_060116B0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mips_seg6_texture_0600FB80),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x969600ff),
    gsSPLightColor(LIGHT_2, 0x3c3c00ff),
    gsSPVertex(mips_seg6_vertex_060115B0, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  3,  1, 0x0,  2,  5,  0, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(mips_seg6_vertex_06011610, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 2,  3,  0, 0x0,  3,  2,  5, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  5,  6,  3, 0x0),
    gsSP2Triangles( 6,  7,  4, 0x0,  7,  6,  5, 0x0),
    gsSP2Triangles( 5,  8,  7, 0x0,  5,  9,  8, 0x0),
    gsSP2Triangles( 5,  2,  9, 0x0,  2,  1,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011780 - 0x060117E0
const Gfx mips_seg6_dl_06011780[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mips_seg6_dl_060116B0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x060117E0

// 0x060117F8

// 0x06011810
static const Vtx mips_seg6_vertex_06011810[] = {
#include "assets/rom/generated/3fc089f762dfa265db3d.inc.c"




};

// 0x06011860
static const Vtx mips_seg6_vertex_06011860[] = {
#include "assets/rom/generated/30ae351310577dce26c0.inc.c"







};

// 0x060118E0 - 0x06011990
const Gfx mips_seg6_dl_060118E0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mips_seg6_texture_0600FB80),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x969600ff),
    gsSPLightColor(LIGHT_2, 0x3c3c00ff),
    gsSPVertex(mips_seg6_vertex_06011810, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  1, 0x0),
    gsSP1Triangle( 1,  4,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(mips_seg6_vertex_06011860, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  3, 0x0,  6,  7,  3, 0x0),
    gsSP2Triangles( 5,  4,  1, 0x0,  6,  5,  0, 0x0),
    gsSP2Triangles( 1,  0,  5, 0x0,  2,  7,  6, 0x0),
    gsSP1Triangle( 6,  0,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011990 - 0x060119F0
const Gfx mips_seg6_dl_06011990[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mips_seg6_dl_060118E0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x060119F0

// 0x06011A08

// 0x06011A20
static const Vtx mips_seg6_vertex_06011A20[] = {
#include "assets/rom/generated/c0f85b8e434ea48b8542.inc.c"





};

// 0x06011A80
static const Vtx mips_seg6_vertex_06011A80[] = {
#include "assets/rom/generated/03c4051e38d773290da8.inc.c"







};

// 0x06011B00 - 0x06011BB0
const Gfx mips_seg6_dl_06011B00[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mips_seg6_texture_0600FB80),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x969600ff),
    gsSPLightColor(LIGHT_2, 0x3c3c00ff),
    gsSPVertex(mips_seg6_vertex_06011A20, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  5,  2,  4, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(mips_seg6_vertex_06011A80, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  1,  3,  6, 0x0),
    gsSP2Triangles( 5,  2,  1, 0x0,  6,  4,  1, 0x0),
    gsSP2Triangles( 4,  7,  5, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011BB0 - 0x06011C10
const Gfx mips_seg6_dl_06011BB0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mips_seg6_dl_06011B00),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x06011C10

// 0x06011C28

// 0x06011C40
static const Vtx mips_seg6_vertex_06011C40[] = {
#include "assets/rom/generated/63707c7da97d94edc67d.inc.c"





};

// 0x06011CA0
static const Vtx mips_seg6_vertex_06011CA0[] = {
#include "assets/rom/generated/d13c5a0957ce014163cd.inc.c"






};

// 0x06011D10 - 0x06011DB0
const Gfx mips_seg6_dl_06011D10[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mips_seg6_texture_0600FB80),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x969600ff),
    gsSPLightColor(LIGHT_2, 0x3c3c00ff),
    gsSPVertex(mips_seg6_vertex_06011C40, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 1,  4,  3, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(mips_seg6_vertex_06011CA0, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  1,  3,  5, 0x0),
    gsSP2Triangles( 2,  1,  5, 0x0,  5,  6,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011DB0 - 0x06011E10
const Gfx mips_seg6_dl_06011DB0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_MODULATERGBFADE),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(mips_seg6_dl_06011D10),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x06011E10

// 0x06011E28
static const Vtx mips_seg6_vertex_06011E28[] = {
#include "assets/rom/generated/c98f54ac8babcbd9055d.inc.c"





};

// 0x06011E88 - 0x06011ED8
const Gfx mips_seg6_dl_06011E88[] = {
    gsSPLightColor(LIGHT_1, 0x826e26ff),
    gsSPLightColor(LIGHT_2, 0x342c0fff),
    gsSPVertex(mips_seg6_vertex_06011E28, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  2,  5,  4, 0x0),
    gsSP2Triangles( 4,  5,  3, 0x0,  1,  4,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011ED8 - 0x06011EF8
const Gfx mips_seg6_dl_06011ED8[] = {
    gsDPPipeSync(),
    gsSPDisplayList(mips_seg6_dl_06011E88),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

// 0x06011EF8

// 0x06011F10
static const Vtx mips_seg6_vertex_06011F10[] = {
#include "assets/rom/generated/5544aa8d74e7984b4b02.inc.c"





};

// 0x06011F70 - 0x06011FC0
const Gfx mips_seg6_dl_06011F70[] = {
    gsSPLightColor(LIGHT_1, 0x826e26ff),
    gsSPLightColor(LIGHT_2, 0x342c0fff),
    gsSPVertex(mips_seg6_vertex_06011F10, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  2, 0x0,  2,  1,  4, 0x0),
    gsSP2Triangles( 4,  3,  5, 0x0,  4,  0,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x06011FC0 - 0x06011FE0
const Gfx mips_seg6_dl_06011FC0[] = {
    gsDPPipeSync(),
    gsSPDisplayList(mips_seg6_dl_06011F70),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};
