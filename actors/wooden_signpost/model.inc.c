// Signpost

// 0x0302C940

// 0x0302C958
static const Vtx wooden_signpost_seg3_vertex_0302C958[] = {
#include "assets/rom/generated/311a8add6316bc85ab47.inc.c"






};

// 0x0302C9C8
ALIGNED8 static const Texture wooden_signpost_seg3_texture_0302C9C8[] = {
#include "actors/wooden_signpost/wooden_signpost_back.rgba16.inc.c"
};

// 0x0302D1C8
ALIGNED8 static const Texture wooden_signpost_seg3_texture_0302D1C8[] = {
#include "actors/wooden_signpost/wooden_signpost_front.rgba16.inc.c"
};

// 0x0302D9C8 - 0x0302DA48
const Gfx wooden_signpost_seg3_dl_0302D9C8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, wooden_signpost_seg3_texture_0302C9C8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(wooden_signpost_seg3_vertex_0302C958, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 0,  3,  1, 0x0,  4,  1,  3, 0x0),
    gsSP2Triangles( 3,  5,  4, 0x0,  5,  3,  2, 0x0),
    gsSP2Triangles( 2,  6,  5, 0x0,  1,  4,  6, 0x0),
    gsSP1Triangle( 6,  2,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0302DA48 - 0x0302DAA8
const Gfx wooden_signpost_seg3_dl_0302DA48[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(wooden_signpost_seg3_dl_0302D9C8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0302DAA8

// 0x0302DAC0
static const Vtx wooden_signpost_seg3_vertex_0302DAC0[] = {
#include "assets/rom/generated/b84f8df9d37f75d37b3b.inc.c"















};

// 0x0302DBC0
static const Vtx wooden_signpost_seg3_vertex_0302DBC0[] = {
#include "assets/rom/generated/f7da86efdfa12fb508d7.inc.c"



};

// 0x0302DC00
static const Vtx wooden_signpost_seg3_vertex_0302DC00[] = {
#include "assets/rom/generated/dcdb605825c53b10acde.inc.c"



};

// 0x0302DC40 - 0x0302DCD0
const Gfx wooden_signpost_seg3_dl_0302DC40[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, wooden_signpost_seg3_texture_0302C9C8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(wooden_signpost_seg3_vertex_0302DAC0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles( 0, 10,  1, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles( 3,  5, 14, 0x0, 11, 13, 15, 0x0),
    gsSPVertex(wooden_signpost_seg3_vertex_0302DBC0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0302DCD0 - 0x0302DD08
const Gfx wooden_signpost_seg3_dl_0302DCD0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, wooden_signpost_seg3_texture_0302D1C8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(wooden_signpost_seg3_vertex_0302DC00, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0302DD08 - 0x0302DD80
const Gfx wooden_signpost_seg3_dl_0302DD08[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(wooden_signpost_seg3_dl_0302DC40),
    gsSPDisplayList(wooden_signpost_seg3_dl_0302DCD0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// Reuse the sign meshes and textures while allowing the encounter sign to fade.
const Gfx yoshi_credit_sign_dl[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADEA, G_CC_MODULATERGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(wooden_signpost_seg3_dl_0302D9C8),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsSPDisplayList(wooden_signpost_seg3_dl_0302DC40),
    gsSPDisplayList(wooden_signpost_seg3_dl_0302DCD0),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
