// Hoot

// 0x05000900

// Unreferenced light group

// 0x05000930

// 0x05000948

// 0x05000960

// 0x05000978

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// 0x050009D8

// 0x050009F0

// 0x05000A08

// 0x05000A20
ALIGNED8 static const Texture hoot_seg5_texture_05000A20[] = {
#include "actors/hoot/hoot_eyes.rgba16.inc.c"
};

// 0x05001220
static const Vtx hoot_seg5_vertex_05001220[] = {
#include "assets/rom/generated/848d4d963fdb424df401.inc.c"














};

// 0x05001310
static const Vtx hoot_seg5_vertex_05001310[] = {
#include "assets/rom/generated/beff0113f48ee97f9323.inc.c"











};

// 0x050013D0
static const Vtx hoot_seg5_vertex_050013D0[] = {
#include "assets/rom/generated/629c117fd614ac0abe92.inc.c"


};

// 0x05001400
static const Vtx hoot_seg5_vertex_05001400[] = {
#include "assets/rom/generated/e5c97ea2d3ad653473ac.inc.c"














};

// 0x050014F0
static const Vtx hoot_seg5_vertex_050014F0[] = {
#include "assets/rom/generated/3c0fafe7b537d751ca02.inc.c"














};

// 0x050015E0
static const Vtx hoot_seg5_vertex_050015E0[] = {
#include "assets/rom/generated/e078383efdce849221b0.inc.c"














};

// 0x050016D0
static const Vtx hoot_seg5_vertex_050016D0[] = {
#include "assets/rom/generated/ec1d8eef348bee3d732c.inc.c"














};

// 0x050017C0
static const Vtx hoot_seg5_vertex_050017C0[] = {
#include "assets/rom/generated/cfed9b9f9a4493dd82ec.inc.c"














};

// 0x050018B0
static const Vtx hoot_seg5_vertex_050018B0[] = {
#include "assets/rom/generated/53464d471b03d5ee4fe0.inc.c"














};

// 0x050019A0
static const Vtx hoot_seg5_vertex_050019A0[] = {
#include "assets/rom/generated/82db1d9b09400467c24e.inc.c"














};

// 0x05001A90
static const Vtx hoot_seg5_vertex_05001A90[] = {
#include "assets/rom/generated/aaef564594be0658a849.inc.c"














};

// 0x05001B80 - 0x05001C00
const Gfx hoot_seg5_dl_05001B80[] = {
    gsDPLoadTextureBlock(hoot_seg5_texture_05000A20, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPLightColor(LIGHT_1, 0x631e0fff),
    gsSPLightColor(LIGHT_2, 0x180703ff),
    gsSPVertex(hoot_seg5_vertex_05001220, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x05001C00 - 0x05001DF0
const Gfx hoot_seg5_dl_05001C00[] = {
    gsSPLightColor(LIGHT_1, 0xfeea26ff),
    gsSPLightColor(LIGHT_2, 0x3f3a09ff),
    gsSPVertex(hoot_seg5_vertex_05001310, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPLightColor(LIGHT_1, 0x4d352eff),
    gsSPLightColor(LIGHT_2, 0x130d0bff),
    gsSPVertex(hoot_seg5_vertex_050013D0, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0x631e0fff),
    gsSPLightColor(LIGHT_2, 0x180703ff),
    gsSPVertex(hoot_seg5_vertex_05001400, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_050014F0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_050015E0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_050016D0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_050017C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_050018B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_050019A0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_05001A90, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x05001DF0 - 0x05001E38
const Gfx hoot_seg5_dl_05001DF0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBA, G_CC_BLENDRGBA),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsSPDisplayList(hoot_seg5_dl_05001B80),
    gsSPTexture(0x8000, 0x8000, 1, G_TX_RENDERTILE + 1, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(hoot_seg5_dl_05001C00),
    gsSPEndDisplayList(),
};

// 0x05001E38

// 0x05001E50
ALIGNED8 static const Texture hoot_seg5_texture_05001E50[] = {
#include "actors/hoot/hoot_wing.rgba16.inc.c"
};

// 0x05002650
ALIGNED8 static const Texture hoot_seg5_texture_05002650[] = {
#include "actors/hoot/hoot_wing_tip.rgba16.inc.c"
};

// 0x05002E50
static const Vtx hoot_seg5_vertex_05002E50[] = {
#include "assets/rom/generated/b986651110b2fd14ea3c.inc.c"





};

// 0x05002EB0 - 0x05002F10
const Gfx hoot_seg5_dl_05002EB0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, hoot_seg5_texture_05002650),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(hoot_seg5_vertex_05002E50, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x05002F10 - 0x05002F60
const Gfx hoot_seg5_dl_05002F10[] = {
    gsDPPipeSync(),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPDisplayList(hoot_seg5_dl_05002EB0),
    gsSPTexture(0x8000, 0x8000, 1, G_TX_RENDERTILE + 1, G_OFF),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x05002F60

// 0x05002F78
static const Vtx hoot_seg5_vertex_05002F78[] = {
#include "assets/rom/generated/f2f0301df97a8f35c1d1.inc.c"





};

// 0x05002FD8 - 0x05003038
const Gfx hoot_seg5_dl_05002FD8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, hoot_seg5_texture_05001E50),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(hoot_seg5_vertex_05002F78, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x05003038 - 0x05003088
const Gfx hoot_seg5_dl_05003038[] = {
    gsDPPipeSync(),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPDisplayList(hoot_seg5_dl_05002FD8),
    gsSPTexture(0x8000, 0x8000, 1, G_TX_RENDERTILE + 1, G_OFF),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x05003088

// 0x050030A0
static const Vtx hoot_seg5_vertex_050030A0[] = {
#include "assets/rom/generated/d54976531a8e132a7a28.inc.c"





};

// 0x05003100 - 0x05003160
const Gfx hoot_seg5_dl_05003100[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, hoot_seg5_texture_05001E50),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(hoot_seg5_vertex_050030A0, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x05003160 - 0x050031B0
const Gfx hoot_seg5_dl_05003160[] = {
    gsDPPipeSync(),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPDisplayList(hoot_seg5_dl_05003100),
    gsSPTexture(0x8000, 0x8000, 1, G_TX_RENDERTILE + 1, G_OFF),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x050031B0

// 0x050031C8
static const Vtx hoot_seg5_vertex_050031C8[] = {
#include "assets/rom/generated/13ca29084f93bf799915.inc.c"





};

// 0x05003228 - 0x05003288
const Gfx hoot_seg5_dl_05003228[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, hoot_seg5_texture_05001E50),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(hoot_seg5_vertex_050031C8, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x05003288 - 0x050032D8
const Gfx hoot_seg5_dl_05003288[] = {
    gsDPPipeSync(),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPDisplayList(hoot_seg5_dl_05003228),
    gsSPTexture(0x8000, 0x8000, 1, G_TX_RENDERTILE + 1, G_OFF),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x050032D8

// 0x050032F0
static const Vtx hoot_seg5_vertex_050032F0[] = {
#include "assets/rom/generated/200da8315a27eb5f392a.inc.c"





};

// 0x05003350 - 0x050033B0
const Gfx hoot_seg5_dl_05003350[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, hoot_seg5_texture_05002650),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(hoot_seg5_vertex_050032F0, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x050033B0 - 0x05003400
const Gfx hoot_seg5_dl_050033B0[] = {
    gsDPPipeSync(),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPDisplayList(hoot_seg5_dl_05003350),
    gsSPTexture(0x8000, 0x8000, 1, G_TX_RENDERTILE + 1, G_OFF),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x05003400

// 0x05003418
static const Vtx hoot_seg5_vertex_05003418[] = {
#include "assets/rom/generated/871caa10a20566668dde.inc.c"





};

// 0x05003478 - 0x050034D8
const Gfx hoot_seg5_dl_05003478[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, hoot_seg5_texture_05001E50),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(hoot_seg5_vertex_05003418, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x050034D8 - 0x05003528
const Gfx hoot_seg5_dl_050034D8[] = {
    gsDPPipeSync(),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPDisplayList(hoot_seg5_dl_05003478),
    gsSPTexture(0x8000, 0x8000, 1, G_TX_RENDERTILE + 1, G_OFF),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x05003528

// 0x05003540
static const Vtx hoot_seg5_vertex_05003540[] = {
#include "assets/rom/generated/e08e16589c12c3f2f83a.inc.c"





};

// 0x050035A0 - 0x05003600
const Gfx hoot_seg5_dl_050035A0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, hoot_seg5_texture_05001E50),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(hoot_seg5_vertex_05003540, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x05003600 - 0x05003650
const Gfx hoot_seg5_dl_05003600[] = {
    gsDPPipeSync(),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPDisplayList(hoot_seg5_dl_050035A0),
    gsSPTexture(0x8000, 0x8000, 1, G_TX_RENDERTILE + 1, G_OFF),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x05003650

// 0x05003668
static const Vtx hoot_seg5_vertex_05003668[] = {
#include "assets/rom/generated/e4bbf870e1b660816a5d.inc.c"





};

// 0x050036C8 - 0x05003728
const Gfx hoot_seg5_dl_050036C8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, hoot_seg5_texture_05001E50),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(hoot_seg5_vertex_05003668, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x05003728 - 0x05003778
const Gfx hoot_seg5_dl_05003728[] = {
    gsDPPipeSync(),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPDisplayList(hoot_seg5_dl_050036C8),
    gsSPTexture(0x8000, 0x8000, 1, G_TX_RENDERTILE + 1, G_OFF),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x05003778
static const Vtx hoot_seg5_vertex_05003778[] = {
#include "assets/rom/generated/b774f50dc99519050e27.inc.c"














};

// 0x05003868
static const Vtx hoot_seg5_vertex_05003868[] = {
#include "assets/rom/generated/1127acfd6b312729c7a1.inc.c"














};

// 0x05003958
static const Vtx hoot_seg5_vertex_05003958[] = {
#include "assets/rom/generated/0a8c6e472ed382f82b89.inc.c"















};

// 0x05003A58
static const Vtx hoot_seg5_vertex_05003A58[] = {
#include "assets/rom/generated/1c0e337726959c14ae73.inc.c"














};

// 0x05003B48
static const Vtx hoot_seg5_vertex_05003B48[] = {
#include "assets/rom/generated/aa7d377d3e49dddc6d5c.inc.c"





};

// 0x05003BA8
static const Vtx hoot_seg5_vertex_05003BA8[] = {
#include "assets/rom/generated/be56aa5ee101e9437b6b.inc.c"














};

// 0x05003C98
static const Vtx hoot_seg5_vertex_05003C98[] = {
#include "assets/rom/generated/38c5dd00b3e5f5bc1e1a.inc.c"











};

// 0x05003D58
static const Vtx hoot_seg5_vertex_05003D58[] = {
#include "assets/rom/generated/50a6b4d71e3b90b99549.inc.c"















};

// 0x05003E58
static const Vtx hoot_seg5_vertex_05003E58[] = {
#include "assets/rom/generated/e5fc27973c2575b66764.inc.c"











};

// 0x05003F18
static const Vtx hoot_seg5_vertex_05003F18[] = {
#include "assets/rom/generated/7c2f9c51209f0143c3a8.inc.c"



};

// 0x05003F58
static const Vtx hoot_seg5_vertex_05003F58[] = {
#include "assets/rom/generated/5cdda7242ba39fea7f77.inc.c"














};

// 0x05004048
static const Vtx hoot_seg5_vertex_05004048[] = {
#include "assets/rom/generated/df3470610199139abd39.inc.c"











};

// 0x05004108
static const Vtx hoot_seg5_vertex_05004108[] = {
#include "assets/rom/generated/b22c656d08546a76d391.inc.c"














};

// 0x050041F8
static const Vtx hoot_seg5_vertex_050041F8[] = {
#include "assets/rom/generated/5fb61d483403281873c1.inc.c"













};

// 0x050042D8
static const Vtx hoot_seg5_vertex_050042D8[] = {
#include "assets/rom/generated/dc0a28363f564c285ae3.inc.c"













};

// 0x050043B8
static const Vtx hoot_seg5_vertex_050043B8[] = {
#include "assets/rom/generated/e67f50e2dae753ab87b2.inc.c"














};

// 0x050044A8
static const Vtx hoot_seg5_vertex_050044A8[] = {
#include "assets/rom/generated/09f8a5d1f0048fff8b81.inc.c"









};

// 0x05004548
static const Vtx hoot_seg5_vertex_05004548[] = {
#include "assets/rom/generated/fe594332cb70dd835d39.inc.c"















};

// 0x05004648
static const Vtx hoot_seg5_vertex_05004648[] = {
#include "assets/rom/generated/192763564f7393214254.inc.c"











};

// 0x05004708
static const Vtx hoot_seg5_vertex_05004708[] = {
#include "assets/rom/generated/7836b2db68196964c8c3.inc.c"



};

// 0x05004748
static const Vtx hoot_seg5_vertex_05004748[] = {
#include "assets/rom/generated/a6b7c3586c2a71294533.inc.c"













};

// 0x05004828
static const Vtx hoot_seg5_vertex_05004828[] = {
#include "assets/rom/generated/b032cbb0f34de120d123.inc.c"















};

// 0x05004928 - 0x05004A98
const Gfx hoot_seg5_dl_05004928[] = {
    gsSPLightColor(LIGHT_1, 0xdb9946ff),
    gsSPLightColor(LIGHT_2, 0x362611ff),
    gsSPVertex(hoot_seg5_vertex_05003778, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_05003868, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_05003958, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  9,  6, 0x0),
    gsSP2Triangles( 4, 10,  5, 0x0,  2, 11,  0, 0x0),
    gsSP2Triangles(12, 11,  2, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(hoot_seg5_vertex_05003A58, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_05003B48, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0x191919ff),
    gsSPLightColor(LIGHT_2, 0x60606ff),
    gsSPVertex(hoot_seg5_vertex_05003BA8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_05003C98, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x05004A98 - 0x05004B38
const Gfx hoot_seg5_dl_05004A98[] = {
    gsSPLightColor(LIGHT_1, 0x772a16ff),
    gsSPLightColor(LIGHT_2, 0x1d0a05ff),
    gsSPVertex(hoot_seg5_vertex_05003D58, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 15, 12, 0x0),
    gsSPVertex(hoot_seg5_vertex_05003E58, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPLightColor(LIGHT_1, 0x191919ff),
    gsSPLightColor(LIGHT_2, 0x60606ff),
    gsSPVertex(hoot_seg5_vertex_05003F18, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x05004B38 - 0x05004CA8
const Gfx hoot_seg5_dl_05004B38[] = {
    gsSPLightColor(LIGHT_1, 0x191919ff),
    gsSPLightColor(LIGHT_2, 0x60606ff),
    gsSPVertex(hoot_seg5_vertex_05003F58, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_05004048, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPLightColor(LIGHT_1, 0xdb9946ff),
    gsSPLightColor(LIGHT_2, 0x362611ff),
    gsSPVertex(hoot_seg5_vertex_05004108, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_050041F8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  9, 10, 0x0),
    gsSP1Triangle(11, 12, 13, 0x0),
    gsSPVertex(hoot_seg5_vertex_050042D8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  9,  6, 0x0),
    gsSP2Triangles( 0, 10,  1, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(hoot_seg5_vertex_050043B8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(hoot_seg5_vertex_050044A8, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x05004CA8 - 0x05004D48
const Gfx hoot_seg5_dl_05004CA8[] = {
    gsSPLightColor(LIGHT_1, 0x772a16ff),
    gsSPLightColor(LIGHT_2, 0x1d0a05ff),
    gsSPVertex(hoot_seg5_vertex_05004548, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(hoot_seg5_vertex_05004648, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPLightColor(LIGHT_1, 0x191919ff),
    gsSPLightColor(LIGHT_2, 0x60606ff),
    gsSPVertex(hoot_seg5_vertex_05004708, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x05004D48 - 0x05004EC0
const Gfx hoot_seg5_dl_05004D48[] = {
    gsSPLightColor(LIGHT_1, 0xc06e3dff),
    gsSPLightColor(LIGHT_2, 0x301b0fff),
    gsSPVertex(hoot_seg5_vertex_05004748, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  1,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  2, 11, 0x0,  0, 12,  5, 0x0),
    gsSP2Triangles( 6,  4,  3, 0x0, 12,  3,  5, 0x0),
    gsSP2Triangles( 1,  0,  5, 0x0,  8,  1,  6, 0x0),
    gsSP2Triangles(11,  2, 13, 0x0, 12,  0,  2, 0x0),
    gsSP2Triangles( 8,  7, 13, 0x0,  2,  8, 13, 0x0),
    gsSP2Triangles( 2, 10, 12, 0x0,  6,  9,  8, 0x0),
    gsSP2Triangles(11, 13,  7, 0x0,  5,  4,  1, 0x0),
    gsSP1Triangle( 1,  8,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0x772a16ff),
    gsSPLightColor(LIGHT_2, 0x1d0a05ff),
    gsSPVertex(hoot_seg5_vertex_05004828, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  4,  3, 0x0),
    gsSP2Triangles( 7,  6,  9, 0x0, 10, 11,  5, 0x0),
    gsSP2Triangles( 7,  1,  4, 0x0,  6,  8, 12, 0x0),
    gsSP2Triangles(13, 14,  0, 0x0,  7,  4,  8, 0x0),
    gsSP2Triangles( 2, 13,  0, 0x0, 13,  2,  9, 0x0),
    gsSP2Triangles( 5,  1,  0, 0x0,  3, 11, 12, 0x0),
    gsSP2Triangles(14, 10,  5, 0x0,  4,  1,  5, 0x0),
    gsSP2Triangles( 5,  0, 14, 0x0, 12,  8,  3, 0x0),
    gsSP2Triangles(11,  3,  5, 0x0,  7,  2,  1, 0x0),
    gsSP1Triangle(15,  2,  7, 0x0),
    gsSPEndDisplayList(),
};
