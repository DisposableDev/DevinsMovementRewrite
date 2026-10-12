// Spindrift

// 0x050006D0
ALIGNED8 static const Texture spindrift_seg5_texture_050006D0[] = {
#include "actors/spindrift/spindrift_face.rgba16.inc.c"
};

// 0x05000ED0
ALIGNED8 static const Texture spindrift_seg5_texture_05000ED0[] = {
#include "actors/spindrift/spindrift_petal.rgba16.inc.c"
};

// 0x050016D0
ALIGNED8 static const Texture spindrift_seg5_texture_050016D0[] = {
#include "actors/spindrift/spindrift_leaf.rgba16.inc.c"
};

// 0x05001ED0
ALIGNED8 static const Texture spindrift_seg5_texture_05001ED0[] = {
#include "actors/spindrift/spindrift_head.rgba16.inc.c"
};

// 0x050026D0
static const Vtx spindrift_seg5_vertex_050026D0[] = {
#include "assets/rom/generated/9ee088259f1eb4fe8dec.inc.c"



};

// 0x05002710 - 0x05002748
const Gfx spindrift_seg5_dl_05002710[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spindrift_seg5_texture_05001ED0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(spindrift_seg5_vertex_050026D0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x05002748 - 0x050027B8
const Gfx spindrift_seg5_dl_05002748[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(spindrift_seg5_dl_05002710),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x050027B8
static const Vtx spindrift_seg5_vertex_050027B8[] = {
#include "assets/rom/generated/b0236397bb6b5efed653.inc.c"



};

// 0x050027F8 - 0x05002830
const Gfx spindrift_seg5_dl_050027F8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spindrift_seg5_texture_050006D0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(spindrift_seg5_vertex_050027B8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x05002830 - 0x050028A0
const Gfx spindrift_seg5_dl_05002830[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(spindrift_seg5_dl_050027F8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x050028A0
static const Vtx spindrift_seg5_vertex_050028A0[] = {
#include "assets/rom/generated/e0958cb163ade3ba8781.inc.c"


};

// 0x050028D0 - 0x05002900
const Gfx spindrift_seg5_dl_050028D0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spindrift_seg5_texture_050006D0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(spindrift_seg5_vertex_050028A0, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x05002900 - 0x05002970
const Gfx spindrift_seg5_dl_05002900[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(spindrift_seg5_dl_050028D0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x05002970

// 0x05002988
static const Vtx spindrift_seg5_vertex_05002988[] = {
#include "assets/rom/generated/ecf4a270e52e68be49e4.inc.c"



};

// 0x050029C8 - 0x05002A20
const Gfx spindrift_seg5_dl_050029C8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spindrift_seg5_texture_050016D0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(spindrift_seg5_vertex_05002988, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x05002A20 - 0x05002A80
const Gfx spindrift_seg5_dl_05002A20[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(spindrift_seg5_dl_050029C8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x05002A80

// 0x05002A98
static const Vtx spindrift_seg5_vertex_05002A98[] = {
#include "assets/rom/generated/8d8403d2d5094c910ebe.inc.c"



};

// 0x05002AD8 - 0x05002B30
const Gfx spindrift_seg5_dl_05002AD8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spindrift_seg5_texture_050016D0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(spindrift_seg5_vertex_05002A98, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x05002B30 - 0x05002B90
const Gfx spindrift_seg5_dl_05002B30[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(spindrift_seg5_dl_05002AD8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x05002B90

// 0x05002BA8
static const Vtx spindrift_seg5_vertex_05002BA8[] = {
#include "assets/rom/generated/b41d0ebcba0e82729ea3.inc.c"














};

// 0x05002C98 - 0x05002D08
const Gfx spindrift_seg5_dl_05002C98[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spindrift_seg5_texture_05000ED0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(spindrift_seg5_vertex_05002BA8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x05002D08 - 0x05002D68
const Gfx spindrift_seg5_dl_05002D08[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(spindrift_seg5_dl_05002C98),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x05000030

// 0x05000048

// 0x05000078

// 0x05000090

// 0x050000C0
static const Vtx spindrift_seg5_vertex_050000C0[] = {
#include "assets/rom/generated/e4429c4ffe1f329c2c7c.inc.c"





};

// 0x05000120
static const Vtx spindrift_seg5_vertex_05000120[] = {
#include "assets/rom/generated/163fee54baf47e715422.inc.c"





};

// 0x05000180
static const Vtx spindrift_seg5_vertex_05000180[] = {
#include "assets/rom/generated/3556528f2e3856155106.inc.c"








};

// 0x05000210
static const Vtx spindrift_seg5_vertex_05000210[] = {
#include "assets/rom/generated/dc9a226bb3505c026dae.inc.c"








};

// 0x050002A0 - 0x05000328
const Gfx spindrift_seg5_dl_050002A0[] = {
    gsSPLightColor(LIGHT_1, 0xddff01ff),
    gsSPLightColor(LIGHT_2, 0x373f00ff),
    gsSPVertex(spindrift_seg5_vertex_050000C0, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 1,  3,  2, 0x0,  4,  5,  2, 0x0),
    gsSP1Triangle( 5,  0,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0x49b200ff),
    gsSPLightColor(LIGHT_2, 0x122c00ff),
    gsSPVertex(spindrift_seg5_vertex_05000120, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 1,  0,  4, 0x0,  4,  0,  5, 0x0),
    gsSP1Triangle( 5,  0,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x05000328 - 0x050003D8
const Gfx spindrift_seg5_dl_05000328[] = {
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPLightColor(LIGHT_1, 0x3f00ff),
    gsSPLightColor(LIGHT_2, 0xf00ff),
    gsSPVertex(spindrift_seg5_vertex_05000180, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 0,  4,  3, 0x0,  1,  5,  6, 0x0),
    gsSP2Triangles( 7,  4,  0, 0x0,  5,  4,  8, 0x0),
    gsSP2Triangles( 5,  1,  3, 0x0,  4,  5,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0xffe200ff),
    gsSPLightColor(LIGHT_2, 0x3f3800ff),
    gsSPVertex(spindrift_seg5_vertex_05000210, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 1,  5,  6, 0x0,  5,  7,  8, 0x0),
    gsSP1Triangle( 8,  7,  3, 0x0),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};
