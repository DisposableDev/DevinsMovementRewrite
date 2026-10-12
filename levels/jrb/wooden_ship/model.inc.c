// Share identical complete vertices in 32-entry batches. Geometry and texture
// events stay native; primitive regrouping permits tiny edge/shading differences.
static const Vtx jrb_ship_vertex_07008B88_0[] = {
#include "assets/rom/generated/2f53e1f9951a1c94867a.inc.c"































};

static const Vtx jrb_ship_vertex_07008B88_1[] = {
#include "assets/rom/generated/72c7eb1d1feda0e92d8e.inc.c"







};

static const Vtx jrb_ship_vertex_07008C90_0[] = {
#include "assets/rom/generated/37ea345902440d4fe954.inc.c"































};

static const Vtx jrb_ship_vertex_07008DD0_0[] = {
#include "assets/rom/generated/d6f528986f087bbe54c3.inc.c"






























};

static const Vtx jrb_ship_vertex_07008DD0_1[] = {
#include "assets/rom/generated/e99444e728572de8522a.inc.c"






















};

static const Vtx jrb_ship_vertex_07008F70_0[] = {
#include "assets/rom/generated/a552ee17f7dc941f6b1d.inc.c"











};

static const Vtx jrb_ship_vertex_07009798_0[] = {
#include "assets/rom/generated/6f7ab2a08b76bdb151be.inc.c"



};

static const Vtx jrb_ship_vertex_07009798_1[] = {
#include "assets/rom/generated/b41b9ed3af0b007ef7b9.inc.c"



};

static const Vtx jrb_ship_vertex_07009808_0[] = {
#include "assets/rom/generated/52ae10bc208039e99627.inc.c"




















};

static const Vtx jrb_ship_vertex_070098C0_0[] = {
#include "assets/rom/generated/6d099d62ca85f3d1320c.inc.c"































};

static const Vtx jrb_ship_vertex_070098C0_1[] = {
#include "assets/rom/generated/6977b0f1875bb8e5a536.inc.c"




















};

static const Vtx jrb_ship_vertex_07009A10_0[] = {
#include "assets/rom/generated/8cee946ab6e95354fb0a.inc.c"





};

static const Vtx jrb_ship_vertex_0700A2F8_0[] = {
#include "assets/rom/generated/37cf06cc483383aaeb89.inc.c"



























};

static const Vtx jrb_ship_vertex_0700A3A8_0[] = {
#include "assets/rom/generated/acdfc5af2cbeb6b8425a.inc.c"






















};

static const Vtx jrb_ship_vertex_0700A470_0[] = {
#include "assets/rom/generated/1ff65c2b69867a61ae60.inc.c"







};

static const Vtx jrb_ship_vertex_0700A470_1[] = {
#include "assets/rom/generated/5c0877993e6daa5d07f6.inc.c"































};

static const Vtx jrb_ship_vertex_0700A470_2[] = {
#include "assets/rom/generated/89756fafcc1c294037e3.inc.c"






};

static const Vtx jrb_ship_vertex_0700AA98_0[] = {
#include "assets/rom/generated/8694840b3a2e5576d0ee.inc.c"



};

static const Vtx jrb_ship_vertex_0700AAE0_0[] = {
#include "assets/rom/generated/deacff32bd5ceb41de85.inc.c"














};

static const Vtx jrb_ship_vertex_0700AB60_0[] = {
#include "assets/rom/generated/d44c3c35255d5cf8fee4.inc.c"





























};

static const Gfx jrb_seg7_dl_07008B88[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09005800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x79b2ebff),
    gsSPLightColor(LIGHT_2, 0x1e2c3aff),
    gsSPVertex(jrb_ship_vertex_07008B88_0, 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(3, 6, 4, 0, 7, 8, 9, 0),
    gsSP2Triangles(7, 10, 8, 0, 11, 12, 13, 0),
    gsSP2Triangles(11, 14, 12, 0, 15, 16, 17, 0),
    gsSP2Triangles(0, 18, 1, 0, 19, 20, 21, 0),
    gsSP2Triangles(19, 22, 20, 0, 23, 24, 25, 0),
    gsSP2Triangles(23, 25, 26, 0, 15, 27, 16, 0),
    gsSP2Triangles(28, 29, 30, 0, 28, 31, 29, 0),
    gsSPVertex(jrb_ship_vertex_07008B88_1, 8, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0),
    gsSP2Triangles(1, 4, 5, 0, 1, 5, 2, 0),
    gsSP2Triangles(4, 6, 7, 0, 4, 7, 5, 0),
    gsSP2Triangles(0, 7, 6, 0, 0, 6, 3, 0),
    gsSPEndDisplayList(),
};

static const Gfx jrb_seg7_dl_07008C90[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09000000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(jrb_ship_vertex_07008C90_0, 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 2, 3, 0, 0),
    gsSP2Triangles(2, 4, 3, 0, 5, 6, 7, 0),
    gsSP2Triangles(5, 8, 6, 0, 5, 9, 8, 0),
    gsSP2Triangles(5, 7, 1, 0, 6, 10, 11, 0),
    gsSP2Triangles(8, 10, 6, 0, 6, 11, 7, 0),
    gsSP2Triangles(7, 12, 13, 0, 7, 11, 12, 0),
    gsSP2Triangles(7, 2, 1, 0, 7, 13, 2, 0),
    gsSP2Triangles(13, 4, 2, 0, 13, 12, 4, 0),
    gsSP2Triangles(9, 14, 8, 0, 9, 15, 14, 0),
    gsSP2Triangles(8, 16, 10, 0, 16, 8, 14, 0),
    gsSP2Triangles(17, 18, 19, 0, 16, 14, 20, 0),
    gsSP2Triangles(21, 22, 23, 0, 21, 23, 24, 0),
    gsSP2Triangles(25, 17, 26, 0, 25, 26, 27, 0),
    gsSP2Triangles(17, 19, 26, 0, 18, 28, 29, 0),
    gsSP2Triangles(18, 29, 19, 0, 28, 30, 29, 0),
    gsSP1Triangle(28, 31, 30, 0),
    gsSPEndDisplayList(),
};

static const Gfx jrb_seg7_dl_07008DD0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(jrb_ship_vertex_07008DD0_0, 31, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(0, 4, 1, 0, 4, 5, 1, 0),
    gsSP2Triangles(4, 6, 7, 0, 4, 7, 5, 0),
    gsSP2Triangles(6, 8, 7, 0, 6, 3, 9, 0),
    gsSP2Triangles(6, 9, 8, 0, 10, 11, 9, 0),
    gsSP2Triangles(3, 10, 9, 0, 12, 13, 14, 0),
    gsSP2Triangles(15, 16, 17, 0, 12, 18, 13, 0),
    gsSP2Triangles(19, 20, 21, 0, 19, 22, 20, 0),
    gsSP2Triangles(21, 20, 23, 0, 21, 23, 24, 0),
    gsSP2Triangles(24, 23, 25, 0, 24, 25, 26, 0),
    gsSP2Triangles(15, 27, 16, 0, 28, 29, 30, 0),
    gsSPVertex(jrb_ship_vertex_07008DD0_1, 23, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(0, 4, 1, 0, 5, 0, 3, 0),
    gsSP2Triangles(5, 3, 6, 0, 7, 8, 9, 0),
    gsSP2Triangles(7, 10, 8, 0, 8, 11, 12, 0),
    gsSP2Triangles(7, 13, 10, 0, 13, 14, 15, 0),
    gsSP2Triangles(13, 15, 16, 0, 6, 14, 13, 0),
    gsSP2Triangles(13, 16, 10, 0, 9, 13, 7, 0),
    gsSP2Triangles(9, 6, 13, 0, 6, 3, 14, 0),
    gsSP2Triangles(12, 6, 9, 0, 12, 5, 6, 0),
    gsSP2Triangles(17, 18, 19, 0, 17, 19, 20, 0),
    gsSP1Triangle(21, 22, 19, 0),
    gsSPEndDisplayList(),
};

static const Gfx jrb_seg7_dl_07008F70[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_0900A800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(jrb_ship_vertex_07008F70_0, 12, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 6, 0, 4, 7, 5, 0),
    gsSP2Triangles(8, 9, 10, 0, 9, 6, 5, 0),
    gsSP2Triangles(9, 11, 6, 0, 8, 11, 9, 0),
    gsSPEndDisplayList(),
};

const Gfx jrb_seg7_dl_07008FD8[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR,
                G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5,
                G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_07008B88),
    gsSPDisplayList(jrb_seg7_dl_07008C90),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5,
                G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_07008DD0),
    gsSPDisplayList(jrb_seg7_dl_07008F70),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

const Gfx jrb_seg7_dl_070090B0[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR,
                G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5,
                G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_07008B88),
    gsSPDisplayList(jrb_seg7_dl_07008C90),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5,
                G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_07008DD0),
    gsSPDisplayList(jrb_seg7_dl_07008F70),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_XLU_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};

static const Gfx jrb_seg7_dl_07009798[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09005800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x79b2ebff),
    gsSPLightColor(LIGHT_2, 0x1e2c3aff),
    gsSPVertex(jrb_ship_vertex_07009798_0, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0),
    gsSPLightColor(LIGHT_1, 0x65b2ffff),
    gsSPLightColor(LIGHT_2, 0x192c3fff),
    gsSPVertex(jrb_ship_vertex_07009798_1, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSPEndDisplayList(),
};

static const Gfx jrb_seg7_dl_07009808[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09000000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(jrb_ship_vertex_07009808_0, 21, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 3, 7, 0, 3, 5, 7, 0),
    gsSP2Triangles(6, 7, 8, 0, 9, 6, 8, 0),
    gsSP2Triangles(9, 8, 10, 0, 11, 1, 12, 0),
    gsSP2Triangles(11, 13, 1, 0, 14, 1, 0, 0),
    gsSP2Triangles(14, 12, 1, 0, 0, 2, 15, 0),
    gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
    gsSP1Triangle(18, 20, 19, 0),
    gsSPEndDisplayList(),
};

static const Gfx jrb_seg7_dl_070098C0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(jrb_ship_vertex_070098C0_0, 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0),
    gsSP2Triangles(4, 5, 6, 0, 6, 5, 7, 0),
    gsSP2Triangles(6, 7, 8, 0, 4, 9, 5, 0),
    gsSP2Triangles(10, 11, 12, 0, 10, 12, 13, 0),
    gsSP2Triangles(14, 15, 16, 0, 17, 18, 19, 0),
    gsSP2Triangles(19, 18, 20, 0, 18, 21, 20, 0),
    gsSP2Triangles(22, 21, 18, 0, 23, 18, 17, 0),
    gsSP2Triangles(22, 18, 23, 0, 24, 19, 20, 0),
    gsSP2Triangles(25, 14, 16, 0, 26, 27, 28, 0),
    gsSP2Triangles(27, 16, 29, 0, 30, 31, 19, 0),
    gsSP1Triangle(31, 17, 19, 0),
    gsSPVertex(jrb_ship_vertex_070098C0_1, 21, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 1, 0, 0),
    gsSP2Triangles(4, 5, 6, 0, 7, 8, 9, 0),
    gsSP2Triangles(10, 4, 11, 0, 12, 13, 14, 0),
    gsSP2Triangles(13, 12, 15, 0, 13, 16, 14, 0),
    gsSP2Triangles(16, 13, 17, 0, 13, 15, 17, 0),
    gsSP2Triangles(17, 15, 18, 0, 17, 18, 19, 0),
    gsSP1Triangle(8, 20, 9, 0),
    gsSPEndDisplayList(),
};

static const Gfx jrb_seg7_dl_07009A10[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_0900A800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(jrb_ship_vertex_07009A10_0, 6, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(0, 4, 5, 0, 0, 3, 4, 0),
    gsSPEndDisplayList(),
};

const Gfx jrb_seg7_dl_07009A58[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR,
                G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5,
                G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_07009798),
    gsSPDisplayList(jrb_seg7_dl_07009808),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5,
                G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_070098C0),
    gsSPDisplayList(jrb_seg7_dl_07009A10),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

const Gfx jrb_seg7_dl_07009B30[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGBFADE, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR,
                G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5,
                G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_07009798),
    gsSPDisplayList(jrb_seg7_dl_07009808),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5,
                G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_070098C0),
    gsSPDisplayList(jrb_seg7_dl_07009A10),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_XLU_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};

static const Gfx jrb_seg7_dl_0700A2F8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09005800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x79b2ebff),
    gsSPLightColor(LIGHT_2, 0x1e2c3aff),
    gsSPVertex(jrb_ship_vertex_0700A2F8_0, 28, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(3, 5, 6, 0, 7, 8, 9, 0),
    gsSP2Triangles(7, 10, 8, 0, 11, 12, 13, 0),
    gsSP2Triangles(11, 14, 12, 0, 15, 16, 17, 0),
    gsSP2Triangles(0, 2, 18, 0, 19, 20, 21, 0),
    gsSP2Triangles(19, 22, 20, 0, 23, 24, 25, 0),
    gsSP2Triangles(23, 26, 24, 0, 15, 17, 27, 0),
    gsSPEndDisplayList(),
};

static const Gfx jrb_seg7_dl_0700A3A8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09000000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(jrb_ship_vertex_0700A3A8_0, 23, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 3, 5, 0, 6, 5, 7, 0),
    gsSP2Triangles(6, 8, 3, 0, 6, 7, 9, 0),
    gsSP2Triangles(0, 10, 11, 0, 0, 12, 10, 0),
    gsSP2Triangles(0, 2, 12, 0, 1, 13, 2, 0),
    gsSP2Triangles(1, 14, 13, 0, 7, 15, 9, 0),
    gsSP2Triangles(7, 16, 15, 0, 16, 17, 15, 0),
    gsSP2Triangles(16, 18, 17, 0, 19, 20, 21, 0),
    gsSP1Triangle(19, 21, 22, 0),
    gsSPEndDisplayList(),
};

static const Gfx jrb_seg7_dl_0700A470[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(jrb_ship_vertex_0700A470_0, 8, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0),
    gsSP2Triangles(0, 4, 3, 0, 4, 5, 6, 0),
    gsSP2Triangles(4, 0, 5, 0, 7, 1, 3, 0),
    gsSP2Triangles(7, 2, 1, 0, 6, 7, 3, 0),
    gsSP2Triangles(0, 2, 5, 0, 6, 3, 4, 0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(jrb_ship_vertex_0700A470_1, 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0),
    gsSP2Triangles(4, 5, 6, 0, 4, 7, 5, 0),
    gsSP2Triangles(8, 6, 5, 0, 5, 7, 9, 0),
    gsSP2Triangles(10, 11, 12, 0, 12, 11, 13, 0),
    gsSP2Triangles(14, 15, 16, 0, 12, 13, 17, 0),
    gsSP2Triangles(10, 18, 11, 0, 19, 18, 10, 0),
    gsSP2Triangles(19, 20, 18, 0, 21, 22, 23, 0),
    gsSP2Triangles(21, 23, 24, 0, 15, 25, 16, 0),
    gsSP2Triangles(26, 14, 16, 0, 27, 16, 28, 0),
    gsSP2Triangles(27, 26, 16, 0, 29, 30, 26, 0),
    gsSP2Triangles(29, 26, 27, 0, 8, 31, 29, 0),
    gsSP2Triangles(8, 29, 6, 0, 31, 30, 29, 0),
    gsSP2Triangles(6, 29, 27, 0, 6, 27, 4, 0),
    gsSPVertex(jrb_ship_vertex_0700A470_2, 7, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP1Triangle(4, 6, 5, 0),
    gsSPEndDisplayList(),
};

const Gfx jrb_seg7_dl_0700A608[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR,
                G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5,
                G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_0700A2F8),
    gsSPDisplayList(jrb_seg7_dl_0700A3A8),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5,
                G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_0700A470),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

static const Gfx jrb_seg7_dl_0700AA98[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09005800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0x79b2ebff),
    gsSPLightColor(LIGHT_2, 0x1e2c3aff),
    gsSPVertex(jrb_ship_vertex_0700AA98_0, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 1, 3, 2, 0),
    gsSPEndDisplayList(),
};

static const Gfx jrb_seg7_dl_0700AAE0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09000000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(jrb_ship_vertex_0700AAE0_0, 15, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(4, 6, 5, 0, 4, 7, 6, 0),
    gsSP2Triangles(3, 5, 8, 0, 9, 10, 11, 0),
    gsSP2Triangles(10, 12, 11, 0, 10, 13, 12, 0),
    gsSP1Triangle(9, 11, 14, 0),
    gsSPEndDisplayList(),
};

static const Gfx jrb_seg7_dl_0700AB60[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(jrb_ship_vertex_0700AB60_0, 30, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 4, 3, 0, 3, 7, 2, 0),
    gsSP2Triangles(3, 2, 1, 0, 3, 1, 8, 0),
    gsSP2Triangles(6, 3, 8, 0, 5, 7, 3, 0),
    gsSP2Triangles(7, 5, 9, 0, 10, 11, 12, 0),
    gsSP2Triangles(10, 13, 11, 0, 14, 8, 1, 0),
    gsSP2Triangles(15, 14, 0, 0, 0, 14, 1, 0),
    gsSP2Triangles(16, 15, 0, 0, 17, 11, 18, 0),
    gsSP2Triangles(17, 12, 11, 0, 19, 20, 21, 0),
    gsSP2Triangles(20, 4, 6, 0, 9, 5, 4, 0),
    gsSP2Triangles(9, 4, 19, 0, 19, 4, 20, 0),
    gsSP2Triangles(22, 23, 24, 0, 22, 24, 25, 0),
    gsSP2Triangles(26, 27, 28, 0, 26, 28, 29, 0),
    gsSPEndDisplayList(),
};

const Gfx jrb_seg7_dl_0700AC68[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR,
                G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5,
                G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_0700AA98),
    gsSPDisplayList(jrb_seg7_dl_0700AAE0),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5,
                G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_0700AB60),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
