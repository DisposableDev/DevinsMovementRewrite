// Scuttlebug

// Unreferenced light group

// Unreferenced light group

// 0x06010108
ALIGNED8 static const Texture scuttlebug_seg6_texture_06010108[] = {
#include "actors/scuttlebug/scuttlebug_eye.rgba16.inc.c"
};

// 0x06010908
ALIGNED8 static const Texture scuttlebug_seg6_texture_06010908[] = {
#include "actors/scuttlebug/scuttlebug_left_side.rgba16.inc.c"
};

// 0x06011908
ALIGNED8 static const Texture scuttlebug_seg6_texture_06011908[] = {
#include "actors/scuttlebug/scuttlebug_right_side.rgba16.inc.c"
};

// 0x06012908
ALIGNED8 static const Texture scuttlebug_seg6_texture_06012908[] = {
#include "actors/scuttlebug/scuttlebug_iris.rgba16.inc.c"
};

// 0x06013108
ALIGNED8 static const Texture scuttlebug_seg6_texture_06013108[] = {
#include "actors/scuttlebug/scuttlebug_leg.rgba16.inc.c"
};

// 0x06013908
static const Vtx scuttlebug_seg6_vertex_06013908[] = {
#include "assets/rom/generated/44153844e03313258021.inc.c"



};

// 0x06013948
static const Vtx scuttlebug_seg6_vertex_06013948[] = {
#include "assets/rom/generated/9f0a5fd0668f03cb2f7e.inc.c"



};

// 0x06013988 - 0x060139C0
const Gfx scuttlebug_seg6_dl_06013988[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, scuttlebug_seg6_texture_06010908),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(scuttlebug_seg6_vertex_06013908, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x060139C0 - 0x060139F8
const Gfx scuttlebug_seg6_dl_060139C0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, scuttlebug_seg6_texture_06011908),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(scuttlebug_seg6_vertex_06013948, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x060139F8 - 0x06013A70
const Gfx scuttlebug_seg6_dl_060139F8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(scuttlebug_seg6_dl_06013988),
    gsSPDisplayList(scuttlebug_seg6_dl_060139C0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x06013A70
static const Vtx scuttlebug_seg6_vertex_06013A70[] = {
#include "assets/rom/generated/de0378e14a11c6042c1e.inc.c"



};

// 0x06013AB0 - 0x06013AE8
const Gfx scuttlebug_seg6_dl_06013AB0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, scuttlebug_seg6_texture_06010108),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(scuttlebug_seg6_vertex_06013A70, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x06013AE8 - 0x06013B58
const Gfx scuttlebug_seg6_dl_06013AE8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(scuttlebug_seg6_dl_06013AB0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x06013B58
static const Vtx scuttlebug_seg6_vertex_06013B58[] = {
#include "assets/rom/generated/3f300044b998f7ae4a07.inc.c"



};

// 0x06013B98 - 0x06013BD0
const Gfx scuttlebug_seg6_dl_06013B98[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, scuttlebug_seg6_texture_06010108),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(scuttlebug_seg6_vertex_06013B58, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x06013BD0 - 0x06013C40
const Gfx scuttlebug_seg6_dl_06013BD0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(scuttlebug_seg6_dl_06013B98),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x06013C40
static const Vtx scuttlebug_seg6_vertex_06013C40[] = {
#include "assets/rom/generated/4636ff67ee975706d24a.inc.c"



};

// 0x06013C80 - 0x06013CB8
const Gfx scuttlebug_seg6_dl_06013C80[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, scuttlebug_seg6_texture_06012908),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(scuttlebug_seg6_vertex_06013C40, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x06013CB8 - 0x06013D28
const Gfx scuttlebug_seg6_dl_06013CB8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(scuttlebug_seg6_dl_06013C80),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x06013D28
static const Vtx scuttlebug_seg6_vertex_06013D28[] = {
#include "assets/rom/generated/b50ade063d59f5945d7c.inc.c"



};

// 0x06013D68 - 0x06013DA0
const Gfx scuttlebug_seg6_dl_06013D68[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, scuttlebug_seg6_texture_06012908),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(scuttlebug_seg6_vertex_06013D28, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x06013DA0 - 0x06013E10
const Gfx scuttlebug_seg6_dl_06013DA0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(scuttlebug_seg6_dl_06013D68),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x06013E10

// 0x06013E28
static const Vtx scuttlebug_seg6_vertex_06013E28[] = {
#include "assets/rom/generated/3b01cffe89cb3cdb15b3.inc.c"















};

// 0x06013F28
static const Vtx scuttlebug_seg6_vertex_06013F28[] = {
#include "assets/rom/generated/e5193f63b8932a310403.inc.c"









};

// 0x06013FC8 - 0x060140F0
const Gfx scuttlebug_seg6_dl_06013FC8[] = {
    gsSPLightColor(LIGHT_1, 0xcf8906ff),
    gsSPLightColor(LIGHT_2, 0x332201ff),
    gsSPVertex(scuttlebug_seg6_vertex_06013E28, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  4,  6, 0x0),
    gsSP2Triangles( 8,  5,  7, 0x0,  3,  5,  8, 0x0),
    gsSP2Triangles( 9, 10,  4, 0x0,  3,  9,  4, 0x0),
    gsSP2Triangles(10,  6,  4, 0x0, 10, 11,  6, 0x0),
    gsSP2Triangles(11,  7,  6, 0x0, 12,  7, 11, 0x0),
    gsSP2Triangles(12,  8,  7, 0x0, 12,  9,  3, 0x0),
    gsSP2Triangles( 3,  8, 12, 0x0, 11, 10,  9, 0x0),
    gsSP2Triangles(11,  9, 12, 0x0, 13, 14,  0, 0x0),
    gsSP2Triangles( 0, 14,  1, 0x0, 15, 13,  0, 0x0),
    gsSPVertex(scuttlebug_seg6_vertex_06013F28, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  1, 0x0),
    gsSP2Triangles( 4,  2,  3, 0x0,  5,  4,  6, 0x0),
    gsSP2Triangles( 6,  4,  7, 0x0,  4,  3,  7, 0x0),
    gsSP2Triangles( 1,  8,  3, 0x0,  8,  7,  3, 0x0),
    gsSP2Triangles( 8,  1,  0, 0x0,  8,  0,  9, 0x0),
    gsSP2Triangles( 9,  7,  8, 0x0,  7,  9,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x060140F0 - 0x06014110
const Gfx scuttlebug_seg6_dl_060140F0[] = {
    gsDPPipeSync(),
    gsSPDisplayList(scuttlebug_seg6_dl_06013FC8),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

// 0x06014110
static const Vtx scuttlebug_seg6_vertex_06014110[] = {
#include "assets/rom/generated/4537303404d007b89414.inc.c"



};

// 0x06014150 - 0x06014188
const Gfx scuttlebug_seg6_dl_06014150[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, scuttlebug_seg6_texture_06013108),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(scuttlebug_seg6_vertex_06014110, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x06014188 - 0x060141F8
const Gfx scuttlebug_seg6_dl_06014188[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(scuttlebug_seg6_dl_06014150),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x060141F8
static const Vtx scuttlebug_seg6_vertex_060141F8[] = {
#include "assets/rom/generated/1742aeccda967de31e5a.inc.c"



};

// 0x06014238 - 0x06014270
const Gfx scuttlebug_seg6_dl_06014238[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, scuttlebug_seg6_texture_06013108),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(scuttlebug_seg6_vertex_060141F8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x06014270 - 0x060142E0
const Gfx scuttlebug_seg6_dl_06014270[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(scuttlebug_seg6_dl_06014238),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x060142E0

// 0x060142F8
static const Vtx scuttlebug_seg6_vertex_060142F8[] = {
#include "assets/rom/generated/9779c1daf99e4d80e205.inc.c"



};

// 0x06014338 - 0x06014378
const Gfx scuttlebug_seg6_dl_06014338[] = {
    gsSPLightColor(LIGHT_1, 0x991616ff),
    gsSPLightColor(LIGHT_2, 0x260505ff),
    gsSPVertex(scuttlebug_seg6_vertex_060142F8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  1, 0x0),
    gsSP2Triangles( 2,  3,  0, 0x0,  1,  0,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x06014378 - 0x06014398
const Gfx scuttlebug_seg6_dl_06014378[] = {
    gsDPPipeSync(),
    gsSPDisplayList(scuttlebug_seg6_dl_06014338),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};
