// Snowman

// ???
UNUSED static const u64 snowman_unused_1 = 1;

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// 0x05008C70
ALIGNED8 static const Texture snowman_seg5_texture_05008C70[] = {
#include "actors/snowman/mr_blizzard_mitten.rgba16.inc.c"
};

// 0x05009470
ALIGNED8 static const Texture snowman_seg5_texture_05009470[] = {
#include "actors/snowman/mr_blizzard_left_side.rgba16.inc.c"
};

// 0x0500A470
ALIGNED8 static const Texture snowman_seg5_texture_0500A470[] = {
#include "actors/snowman/mr_blizzard_right_side.rgba16.inc.c"
};

// 0x0500B470
ALIGNED8 static const Texture snowman_seg5_texture_0500B470[] = {
#include "actors/snowman/mr_blizzard_eye.rgba16.inc.c"
};

// 0x0500BC70
ALIGNED8 static const Texture snowman_seg5_texture_0500BC70[] = {
#include "actors/snowman/mr_blizzard_mouth.rgba16.inc.c"
};

// 0x0500C470

// 0x0500C488
static const Vtx snowman_seg5_vertex_0500C488[] = {
#include "assets/rom/generated/190c3046d8252406402e.inc.c"



};

// 0x0500C4C8 - 0x0500C500
const Gfx snowman_seg5_dl_0500C4C8[] = {
    gsSPLightColor(LIGHT_1, 0x381e0eff),
    gsSPLightColor(LIGHT_2, 0x1c0f07ff),
    gsSPVertex(snowman_seg5_vertex_0500C488, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP1Triangle( 0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C500 - 0x0500C530
const Gfx snowman_seg5_dl_0500C500[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(snowman_seg5_dl_0500C4C8),
    gsSPEndDisplayList(),
};

// 0x0500C530
static const Vtx snowman_seg5_vertex_0500C530[] = {
#include "assets/rom/generated/f6d446fad0c2bed5c2f3.inc.c"



};

// 0x0500C570
static const Vtx snowman_seg5_vertex_0500C570[] = {
#include "assets/rom/generated/9d03ecd8a8988e398a2a.inc.c"



};

// 0x0500C5B0 - 0x0500C5E8
const Gfx snowman_seg5_dl_0500C5B0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snowman_seg5_texture_05009470),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(snowman_seg5_vertex_0500C530, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C5E8 - 0x0500C620
const Gfx snowman_seg5_dl_0500C5E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snowman_seg5_texture_0500A470),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(snowman_seg5_vertex_0500C570, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C620 - 0x0500C698
const Gfx snowman_seg5_dl_0500C620[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(snowman_seg5_dl_0500C5B0),
    gsSPDisplayList(snowman_seg5_dl_0500C5E8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0500C698
static const Vtx snowman_seg5_vertex_0500C698[] = {
#include "assets/rom/generated/a12b4c2c0c111e93b864.inc.c"







};

// 0x0500C718 - 0x0500C760
const Gfx snowman_seg5_dl_0500C718[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snowman_seg5_texture_0500B470),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(snowman_seg5_vertex_0500C698, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C760 - 0x0500C7D0
const Gfx snowman_seg5_dl_0500C760[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(snowman_seg5_dl_0500C718),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0500C7D0

// 0x0500C7E8
static const Vtx snowman_seg5_vertex_0500C7E8[] = {
#include "assets/rom/generated/4ec7987c22a75b3396ee.inc.c"















};

// 0x0500C8E8
static const Vtx snowman_seg5_vertex_0500C8E8[] = {
#include "assets/rom/generated/3c4c913832def9d37b04.inc.c"








};

// 0x0500C978 - 0x0500CAA8
const Gfx snowman_seg5_dl_0500C978[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snowman_seg5_texture_05008C70),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(snowman_seg5_vertex_0500C7E8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 4,  1,  0, 0x0,  5,  4,  3, 0x0),
    gsSP2Triangles( 5,  6,  4, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 7,  1,  4, 0x0,  7,  6,  8, 0x0),
    gsSP2Triangles( 9,  8,  6, 0x0,  9,  6, 10, 0x0),
    gsSP2Triangles( 5, 10,  6, 0x0,  7,  2,  1, 0x0),
    gsSP2Triangles( 8, 11,  7, 0x0, 11,  2,  7, 0x0),
    gsSP2Triangles( 5, 12, 13, 0x0, 13, 14,  5, 0x0),
    gsSP2Triangles( 5,  3, 12, 0x0,  5, 14, 10, 0x0),
    gsSP2Triangles(10, 14,  9, 0x0, 13,  9, 14, 0x0),
    gsSP2Triangles( 8,  9, 13, 0x0,  2, 15,  0, 0x0),
    gsSPVertex(snowman_seg5_vertex_0500C8E8, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  1, 0x0),
    gsSP2Triangles( 4,  5,  2, 0x0,  2,  6,  4, 0x0),
    gsSP2Triangles( 2,  5,  7, 0x0,  0,  2,  7, 0x0),
    gsSP2Triangles( 3,  6,  2, 0x0,  7,  8,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500CAA8 - 0x0500CB08
const Gfx snowman_seg5_dl_0500CAA8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(snowman_seg5_dl_0500C978),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0500CB08
static const Vtx snowman_seg5_vertex_0500CB08[] = {
#include "assets/rom/generated/2ba048375b58828214e8.inc.c"



};

// 0x0500CB48
static const Vtx snowman_seg5_vertex_0500CB48[] = {
#include "assets/rom/generated/a9f6b4bf560e8eed4352.inc.c"



};

// 0x0500CB88 - 0x0500CBC0
const Gfx snowman_seg5_dl_0500CB88[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snowman_seg5_texture_05009470),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(snowman_seg5_vertex_0500CB08, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500CBC0 - 0x0500CBF8
const Gfx snowman_seg5_dl_0500CBC0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snowman_seg5_texture_0500A470),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(snowman_seg5_vertex_0500CB48, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500CBF8 - 0x0500CC70
const Gfx snowman_seg5_dl_0500CBF8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(snowman_seg5_dl_0500CB88),
    gsSPDisplayList(snowman_seg5_dl_0500CBC0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0500CC70
static const Vtx snowman_seg5_vertex_0500CC70[] = {
#include "assets/rom/generated/4e3f494a10c82dbd9b3d.inc.c"



};

// 0x0500CCB0 - 0x0500CCE8
const Gfx snowman_seg5_dl_0500CCB0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snowman_seg5_texture_0500BC70),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(snowman_seg5_vertex_0500CC70, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500CCE8 - 0x0500CD58
const Gfx snowman_seg5_dl_0500CCE8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(snowman_seg5_dl_0500CCB0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
