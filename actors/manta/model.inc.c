// Manta Ray

// 0x05001758

// 0x05001770

// Unreferenced light group

// 0x050017A0
ALIGNED8 static const Texture manta_seg5_texture_050017A0[] = {
#include "actors/manta/manta_fin_corner.rgba16.inc.c"
};

// 0x05001FA0
ALIGNED8 static const Texture manta_seg5_texture_05001FA0[] = {
#include "actors/manta/manta_gills.rgba16.inc.c"
};

// 0x05002FA0
ALIGNED8 static const Texture manta_seg5_texture_05002FA0[] = {
#include "actors/manta/manta_eye.rgba16.inc.c"
};

// 0x050037A0
ALIGNED8 static const Texture manta_seg5_texture_050037A0[] = {
#include "actors/manta/manta_fin_edge.rgba16.inc.c"
};

// 0x050047A0
static const Vtx manta_seg5_vertex_050047A0[] = {
#include "assets/rom/generated/ea99742446f1ba1e9c46.inc.c"















};

// 0x050048A0
static const Vtx manta_seg5_vertex_050048A0[] = {
#include "assets/rom/generated/13981832bda9ac2b06bd.inc.c"











};

// 0x05004960 - 0x050049C8
const Gfx manta_seg5_dl_05004960[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_05002FA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_050047A0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 7,  6,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 0, 12,  1, 0x0, 13, 14, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x050049C8 - 0x05004A70
const Gfx manta_seg5_dl_050049C8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050017A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_050048A0, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  4, 0x0),
    gsSP2Triangles( 1,  0,  5, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 1,  5,  4, 0x0,  3,  4,  6, 0x0),
    gsSP2Triangles( 7,  8,  2, 0x0,  8,  0,  2, 0x0),
    gsSP2Triangles( 6,  4,  9, 0x0,  4,  5,  9, 0x0),
    gsSP2Triangles( 5, 10,  9, 0x0, 10, 11,  9, 0x0),
    gsSP2Triangles( 0, 10,  5, 0x0,  0,  8, 10, 0x0),
    gsSP2Triangles( 8,  7, 11, 0x0,  8, 11, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x05004A70 - 0x05004AE8
const Gfx manta_seg5_dl_05004A70[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_DECALRGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_05004960),
    gsSPDisplayList(manta_seg5_dl_050049C8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x05004AE8
static const Vtx manta_seg5_vertex_05004AE8[] = {
#include "assets/rom/generated/6b8e482130c5d4a3113b.inc.c"















};

// 0x05004BE8
static const Vtx manta_seg5_vertex_05004BE8[] = {
#include "assets/rom/generated/332efc769095f6820187.inc.c"











};

// 0x05004CA8 - 0x05004D10
const Gfx manta_seg5_dl_05004CA8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_05002FA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_05004AE8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  5,  4, 0x0,  7,  6,  4, 0x0),
    gsSP2Triangles( 8,  6,  7, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 1, 12,  2, 0x0, 13, 14, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x05004D10 - 0x05004DB8
const Gfx manta_seg5_dl_05004D10[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050017A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_05004BE8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 0,  4,  3, 0x0,  0,  5,  4, 0x0),
    gsSP2Triangles( 6,  7,  5, 0x0,  7,  4,  5, 0x0),
    gsSP2Triangles( 8,  3,  4, 0x0,  7,  8,  4, 0x0),
    gsSP2Triangles( 9,  7,  6, 0x0,  9,  8,  7, 0x0),
    gsSP2Triangles( 9, 10,  8, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles( 8, 10,  3, 0x0, 10,  1,  3, 0x0),
    gsSP2Triangles(11,  2,  1, 0x0, 10, 11,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x05004DB8 - 0x05004E30
const Gfx manta_seg5_dl_05004DB8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_DECALRGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_05004CA8),
    gsSPDisplayList(manta_seg5_dl_05004D10),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x05004E30
static const Vtx manta_seg5_vertex_05004E30[] = {
#include "assets/rom/generated/af98e10b768133514a6b.inc.c"


};

// 0x05004E60 - 0x05004E90
const Gfx manta_seg5_dl_05004E60[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050037A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_05004E30, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x05004E90 - 0x05004F00
const Gfx manta_seg5_dl_05004E90[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_05004E60),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x05004F00
static const Vtx manta_seg5_vertex_05004F00[] = {
#include "assets/rom/generated/2de02589f1436aae747b.inc.c"









};

// 0x05004FA0 - 0x05005038
const Gfx manta_seg5_dl_05004FA0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050017A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_05004F00, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 1,  0,  3, 0x0,  2,  1,  4, 0x0),
    gsSP2Triangles( 3,  0,  5, 0x0,  2,  5,  0, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  7,  4,  3, 0x0),
    gsSP2Triangles( 3,  6,  7, 0x0,  8,  5,  2, 0x0),
    gsSP2Triangles( 5,  8,  6, 0x0,  9,  2,  4, 0x0),
    gsSP2Triangles( 4,  7,  9, 0x0,  2,  9,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x05005038 - 0x050050A8
const Gfx manta_seg5_dl_05005038[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_05004FA0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x050050A8
static const Vtx manta_seg5_vertex_050050A8[] = {
#include "assets/rom/generated/376986954922a02fb6c6.inc.c"






};

// 0x05005118
static const Vtx manta_seg5_vertex_05005118[] = {
#include "assets/rom/generated/138b30390a1385fbbe8d.inc.c"









};

// 0x050051B8
static const Vtx manta_seg5_vertex_050051B8[] = {
#include "assets/rom/generated/a0486786f18451fd8ca6.inc.c"








};

// 0x05005248 - 0x05005288
const Gfx manta_seg5_dl_05005248[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050017A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_050050A8, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  3,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x05005288 - 0x05005308
const Gfx manta_seg5_dl_05005288[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050037A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_05005118, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 1,  0,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 5,  3,  6, 0x0,  3,  7,  6, 0x0),
    gsSP2Triangles( 7,  3,  0, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9,  8,  7, 0x0,  0,  9,  7, 0x0),
    gsSP1Triangle( 2,  9,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x05005308 - 0x05005358
const Gfx manta_seg5_dl_05005308[] = {
    gsSPVertex(manta_seg5_vertex_050051B8, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 0,  2,  5, 0x0,  2,  4,  5, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  7,  4,  6, 0x0),
    gsSP2Triangles( 8,  4,  7, 0x0,  5,  4,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x05005358 - 0x050053F0
const Gfx manta_seg5_dl_05005358[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_05005248),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_05005288),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(manta_seg5_dl_05005308),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x050053F0
static const Vtx manta_seg5_vertex_050053F0[] = {
#include "assets/rom/generated/4cb33cb4f7516b5b8cfa.inc.c"







};

// 0x05005470
static const Vtx manta_seg5_vertex_05005470[] = {
#include "assets/rom/generated/026e67f4b6aa59319a57.inc.c"







};

// 0x050054F0 - 0x05005558
const Gfx manta_seg5_dl_050054F0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050037A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_050053F0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 4,  0,  3, 0x0,  5,  4,  3, 0x0),
    gsSP2Triangles( 0,  4,  6, 0x0,  0,  6,  1, 0x0),
    gsSP2Triangles( 4,  5,  7, 0x0,  4,  7,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x05005558 - 0x050055A8
const Gfx manta_seg5_dl_05005558[] = {
    gsSPVertex(manta_seg5_vertex_05005470, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 5,  6,  0, 0x0,  6,  1,  0, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  0,  3,  5, 0x0),
    gsSP2Triangles( 7,  2,  1, 0x0,  7,  4,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x050055A8 - 0x05005620
const Gfx manta_seg5_dl_050055A8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_DECALRGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_050054F0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(manta_seg5_dl_05005558),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x05005620
static const Vtx manta_seg5_vertex_05005620[] = {
#include "assets/rom/generated/e2fa6d55215b89f2a181.inc.c"





};

// 0x05005680
static const Vtx manta_seg5_vertex_05005680[] = {
#include "assets/rom/generated/386284076fa41111dad1.inc.c"





};

// 0x050056E0 - 0x05005730
const Gfx manta_seg5_dl_050056E0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050037A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_05005620, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  4, 0x0),
    gsSP2Triangles( 4,  1,  0, 0x0,  1,  3,  5, 0x0),
    gsSP1Triangle( 5,  2,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x05005730 - 0x05005768
const Gfx manta_seg5_dl_05005730[] = {
    gsSPVertex(manta_seg5_vertex_05005680, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 3,  0,  2, 0x0,  4,  5,  0, 0x0),
    gsSP1Triangle( 5,  1,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x05005768 - 0x050057E0
const Gfx manta_seg5_dl_05005768[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_050056E0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(manta_seg5_dl_05005730),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x050057E0
static const Vtx manta_seg5_vertex_050057E0[] = {
#include "assets/rom/generated/160c2c60d15181619c2a.inc.c"









};

// 0x05005880 - 0x05005918
const Gfx manta_seg5_dl_05005880[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050017A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_050057E0, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 5,  3,  1, 0x0,  1,  6,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  4,  2,  1, 0x0),
    gsSP2Triangles( 7,  5,  6, 0x0,  6,  0,  7, 0x0),
    gsSP2Triangles( 3,  5,  7, 0x0,  7,  0,  8, 0x0),
    gsSP2Triangles( 2,  8,  0, 0x0,  3,  7,  9, 0x0),
    gsSP2Triangles( 9,  4,  3, 0x0,  8,  9,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x05005918 - 0x05005988
const Gfx manta_seg5_dl_05005918[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_05005880),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x05005988
static const Vtx manta_seg5_vertex_05005988[] = {
#include "assets/rom/generated/f0d581020af1770af885.inc.c"






};

// 0x050059F8
static const Vtx manta_seg5_vertex_050059F8[] = {
#include "assets/rom/generated/cbefd414315e00139a5e.inc.c"









};

// 0x05005A98
static const Vtx manta_seg5_vertex_05005A98[] = {
#include "assets/rom/generated/1efe8ce8157caf4e3708.inc.c"








};

// 0x05005B28 - 0x05005B68
const Gfx manta_seg5_dl_05005B28[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050017A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_05005988, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 3,  5,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x05005B68 - 0x05005BE8
const Gfx manta_seg5_dl_05005B68[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050037A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_050059F8, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  6,  4,  2, 0x0),
    gsSP2Triangles( 3,  7,  4, 0x0,  2,  4,  7, 0x0),
    gsSP2Triangles( 6,  2,  1, 0x0,  8,  7,  3, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  7,  9,  2, 0x0),
    gsSP1Triangle( 2,  9,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x05005BE8 - 0x05005C38
const Gfx manta_seg5_dl_05005BE8[] = {
    gsSPVertex(manta_seg5_vertex_05005A98, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  4,  5,  1, 0x0),
    gsSP2Triangles( 6,  5,  4, 0x0,  6,  4,  7, 0x0),
    gsSP2Triangles( 7,  4,  3, 0x0,  1,  8,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x05005C38 - 0x05005CD0
const Gfx manta_seg5_dl_05005C38[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_05005B28),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_05005B68),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(manta_seg5_dl_05005BE8),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x05005CD0
static const Vtx manta_seg5_vertex_05005CD0[] = {
#include "assets/rom/generated/9ff4cdba4c45c10256fc.inc.c"







};

// 0x05005D50
static const Vtx manta_seg5_vertex_05005D50[] = {
#include "assets/rom/generated/991aef73120f7d4055f2.inc.c"







};

// 0x05005DD0 - 0x05005E38
const Gfx manta_seg5_dl_05005DD0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050037A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_05005CD0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 2,  5,  3, 0x0,  5,  2,  1, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  5,  1,  6, 0x0),
    gsSP2Triangles( 7,  6,  1, 0x0,  0,  7,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x05005E38 - 0x05005E88
const Gfx manta_seg5_dl_05005E38[] = {
    gsSPVertex(manta_seg5_vertex_05005D50, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 0,  4,  3, 0x0,  4,  0,  2, 0x0),
    gsSP2Triangles( 2,  1,  5, 0x0,  4,  2,  6, 0x0),
    gsSP2Triangles( 2,  5,  7, 0x0,  7,  6,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x05005E88 - 0x05005F00
const Gfx manta_seg5_dl_05005E88[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_05005DD0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(manta_seg5_dl_05005E38),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x05005F00
static const Vtx manta_seg5_vertex_05005F00[] = {
#include "assets/rom/generated/2d7dde87067dabe3734b.inc.c"





};

// 0x05005F60
static const Vtx manta_seg5_vertex_05005F60[] = {
#include "assets/rom/generated/fe3e9f54fb3a858084a8.inc.c"





};

// 0x05005FC0 - 0x05006010
const Gfx manta_seg5_dl_05005FC0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050037A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_05005F00, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 1,  0,  3, 0x0,  4,  0,  5, 0x0),
    gsSP1Triangle( 2,  5,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x05006010 - 0x05006048
const Gfx manta_seg5_dl_05006010[] = {
    gsSPVertex(manta_seg5_vertex_05005F60, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 3,  0,  4, 0x0,  0,  2,  5, 0x0),
    gsSP1Triangle( 5,  4,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x05006048 - 0x050060C0
const Gfx manta_seg5_dl_05006048[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_05005FC0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(manta_seg5_dl_05006010),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x050060C0
static const Vtx manta_seg5_vertex_050060C0[] = {
#include "assets/rom/generated/5c631ba98431ee342c5a.inc.c"













};

// 0x050061A0
static const Vtx manta_seg5_vertex_050061A0[] = {
#include "assets/rom/generated/f8c5057a638c92e351bd.inc.c"














};

// 0x05006290
static const Vtx manta_seg5_vertex_05006290[] = {
#include "assets/rom/generated/a915b75cb862efc1a9b3.inc.c"













};

// 0x05006370
static const Vtx manta_seg5_vertex_05006370[] = {
#include "assets/rom/generated/6c17cb497fed9dd400a3.inc.c"










};

// 0x05006420
static const Vtx manta_seg5_vertex_05006420[] = {
#include "assets/rom/generated/635b4099d07e4af2e570.inc.c"














};

// 0x05006510 - 0x05006588
const Gfx manta_seg5_dl_05006510[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050017A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_050060C0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  3,  5,  7, 0x0),
    gsSP2Triangles( 7,  8,  3, 0x0,  6,  9, 10, 0x0),
    gsSP2Triangles( 4,  6, 10, 0x0, 11, 12,  8, 0x0),
    gsSP2Triangles( 8,  7, 11, 0x0, 13, 12, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x05006588 - 0x050065F0
const Gfx manta_seg5_dl_05006588[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_05001FA0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_050061A0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  5,  2,  4, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12,  7, 13, 0x0, 11, 10, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x050065F0 - 0x050066D0
const Gfx manta_seg5_dl_050065F0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, manta_seg5_texture_050037A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(manta_seg5_vertex_05006290, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  2,  5,  8, 0x0),
    gsSP2Triangles( 8,  5,  4, 0x0,  6,  5,  2, 0x0),
    gsSP2Triangles( 5,  7,  3, 0x0,  6,  2,  9, 0x0),
    gsSP2Triangles(10,  6,  9, 0x0,  7,  6, 11, 0x0),
    gsSP2Triangles(10, 11,  6, 0x0,  8, 12,  0, 0x0),
    gsSP2Triangles( 0,  2,  8, 0x0, 13,  1,  0, 0x0),
    gsSP1Triangle( 2,  1,  9, 0x0),
    gsSPVertex(manta_seg5_vertex_05006370, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  3,  7,  4, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9,  0, 0x0),
    gsSP1Triangle( 2, 10,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x050066D0 - 0x05006750
const Gfx manta_seg5_dl_050066D0[] = {
    gsSPVertex(manta_seg5_vertex_05006420, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  7,  6, 0x0,  8,  6,  9, 0x0),
    gsSP2Triangles( 3, 10,  6, 0x0, 11, 12,  1, 0x0),
    gsSP2Triangles(11,  1,  0, 0x0,  1, 10,  3, 0x0),
    gsSP2Triangles( 2,  1,  3, 0x0,  3, 13, 14, 0x0),
    gsSP2Triangles( 2,  3, 14, 0x0,  5, 13,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x05006750 - 0x05006808
const Gfx manta_seg5_dl_05006750[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_DECALRGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_05006510),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_05006588),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(manta_seg5_dl_050065F0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(manta_seg5_dl_050066D0),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x05006808
static const Vtx manta_seg5_vertex_05006808[] = {
#include "assets/rom/generated/e40f1b640660c0ae4f4d.inc.c"





};

// 0x05006868
static const Vtx manta_seg5_vertex_05006868[] = {
#include "assets/rom/generated/e68014916e4e7028989f.inc.c"





};

// 0x050068C8
static const Vtx manta_seg5_vertex_050068C8[] = {
#include "assets/rom/generated/2b25f4fa3304a5cecc57.inc.c"







};

// 0x05006948
static const Vtx manta_seg5_vertex_05006948[] = {
#include "assets/rom/generated/cbc71a395de6fbcc80c3.inc.c"







};

// 0x050069C8
static const Vtx manta_seg5_vertex_050069C8[] = {
#include "assets/rom/generated/0537752864eb645a00f4.inc.c"











};

// 0x05006A88
static const Vtx manta_seg5_vertex_05006A88[] = {
#include "assets/rom/generated/6857040b3386c02dc30a.inc.c"







};

// 0x05006B08 - 0x05006B70
const Gfx manta_seg5_dl_05006B08[] = {
    gsSPLightColor(LIGHT_1, 0x35b63ff),
    gsSPLightColor(LIGHT_2, 0x1618ff),
    gsSPVertex(manta_seg5_vertex_05006808, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  1,  0,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xc6eeedff),
    gsSPLightColor(LIGHT_2, 0x313b3bff),
    gsSPVertex(manta_seg5_vertex_05006868, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x05006B70 - 0x05006C08
const Gfx manta_seg5_dl_05006B70[] = {
    gsSPLightColor(LIGHT_1, 0x35b63ff),
    gsSPLightColor(LIGHT_2, 0x1618ff),
    gsSPVertex(manta_seg5_vertex_050068C8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  1,  7,  2, 0x0),
    gsSP2Triangles( 5,  1,  0, 0x0,  5,  0,  3, 0x0),
    gsSP2Triangles( 6,  7,  1, 0x0,  6,  1,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xc6eeedff),
    gsSPLightColor(LIGHT_2, 0x313b3bff),
    gsSPVertex(manta_seg5_vertex_05006948, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  7,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x05006C08 - 0x05006CA0
const Gfx manta_seg5_dl_05006C08[] = {
    gsSPLightColor(LIGHT_1, 0x35b63ff),
    gsSPLightColor(LIGHT_2, 0x1618ff),
    gsSPVertex(manta_seg5_vertex_050069C8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  1,  7,  2, 0x0),
    gsSP2Triangles( 5,  8,  9, 0x0,  5,  9,  3, 0x0),
    gsSP2Triangles( 6, 10, 11, 0x0,  6, 11,  5, 0x0),
    gsSPLightColor(LIGHT_1, 0xc6eeedff),
    gsSPLightColor(LIGHT_2, 0x313b3bff),
    gsSPVertex(manta_seg5_vertex_05006A88, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  7,  3, 0x0),
    gsSPEndDisplayList(),
};
