// Unagi (Eel)

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// 0x0500AF20
ALIGNED8 static const Texture unagi_seg5_texture_0500AF20[] = {
#include "actors/unagi/unagi_body.rgba16.inc.c"
};

// 0x0500B720
ALIGNED8 static const Texture unagi_seg5_texture_0500B720[] = {
#include "actors/unagi/unagi_eye.rgba16.inc.c"
};

// 0x0500B920
ALIGNED8 static const Texture unagi_seg5_texture_0500B920[] = {
#include "actors/unagi/unagi_head_base.rgba16.inc.c"
};

// 0x0500C120
ALIGNED8 static const Texture unagi_seg5_texture_0500C120[] = {
#include "actors/unagi/unagi_tooth.rgba16.inc.c"
};

// 0x0500C320
ALIGNED8 static const Texture unagi_seg5_texture_0500C320[] = {
#include "actors/unagi/unagi_mouth.rgba16.inc.c"
};

// 0x0500C3A0
ALIGNED8 static const Texture unagi_seg5_texture_0500C3A0[] = {
#include "actors/unagi/unagi_tail.rgba16.inc.c"
};

// 0x0500CBA0
static const Vtx unagi_seg5_vertex_0500CBA0[] = {
#include "assets/rom/generated/00141e2aaed5603950b2.inc.c"






};

// 0x0500CC10
static const Vtx unagi_seg5_vertex_0500CC10[] = {
#include "assets/rom/generated/c9d3f9bad2a010f8c8d1.inc.c"












};

// 0x0500CCE0 - 0x0500CD30
const Gfx unagi_seg5_dl_0500CCE0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, unagi_seg5_texture_0500C320),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 8 * 8 - 1, CALC_DXT(8, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(unagi_seg5_vertex_0500CBA0, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  3,  5,  4, 0x0),
    gsSP1Triangle( 6,  1,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500CD30 - 0x0500CDD0
const Gfx unagi_seg5_dl_0500CD30[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, unagi_seg5_texture_0500B920),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(unagi_seg5_vertex_0500CC10, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  7,  8,  5, 0x0),
    gsSP2Triangles( 5,  8,  3, 0x0,  9,  7,  6, 0x0),
    gsSP2Triangles( 8,  7,  1, 0x0,  1,  7,  9, 0x0),
    gsSP2Triangles( 4,  3, 10, 0x0, 10,  3, 11, 0x0),
    gsSP2Triangles(12, 11,  3, 0x0,  8, 12,  3, 0x0),
    gsSP2Triangles( 1,  9,  2, 0x0, 12,  8,  1, 0x0),
    gsSP1Triangle( 1,  0, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500CDD0 - 0x0500CEA8
const Gfx unagi_seg5_dl_0500CDD0[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_PASS2),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 2, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 3, G_TX_NOLOD, G_TX_CLAMP, 3, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (8 - 1) << G_TEXTURE_IMAGE_FRAC, (8 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(unagi_seg5_dl_0500CCE0),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(unagi_seg5_dl_0500CD30),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0500CEA8
static const Vtx unagi_seg5_vertex_0500CEA8[] = {
#include "assets/rom/generated/016c0b4dd19da26d60b0.inc.c"










};

// 0x0500CF58
static const Vtx unagi_seg5_vertex_0500CF58[] = {
#include "assets/rom/generated/5bedbb739f7d15e3991c.inc.c"


};

// 0x0500CF88 - 0x0500D038
const Gfx unagi_seg5_dl_0500CF88[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, unagi_seg5_texture_0500AF20),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(unagi_seg5_vertex_0500CEA8, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 5,  0,  4, 0x0,  6,  7,  4, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  7,  6,  8, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  1,  0,  5, 0x0),
    gsSP2Triangles( 1,  5,  9, 0x0,  9,  5,  8, 0x0),
    gsSP2Triangles( 6,  9,  8, 0x0, 10,  9,  6, 0x0),
    gsSP2Triangles( 6,  3, 10, 0x0, 10,  3,  2, 0x0),
    gsSP1Triangle( 2,  1, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500D038 - 0x0500D050
const Gfx unagi_seg5_dl_0500D038[] = {
    gsSPVertex(unagi_seg5_vertex_0500CF58, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500D050 - 0x0500D120
const Gfx unagi_seg5_dl_0500D050[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_PASS2),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(unagi_seg5_dl_0500CF88),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_PASS2),
    gsSPDisplayList(unagi_seg5_dl_0500D038),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0500D120
static const Vtx unagi_seg5_vertex_0500D120[] = {
#include "assets/rom/generated/bd42003b966677ef1567.inc.c"









};

// Body material begins at D268 and ends at D828, in unagi_geo traversal order.
// The middle joints reuse the loaded skin texture across their own matrices.
// 0x0500D1C0 - 0x0500D268
const Gfx unagi_seg5_dl_0500D1C0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, unagi_seg5_texture_0500AF20),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(unagi_seg5_vertex_0500D120, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 5,  6,  2, 0x0,  2,  4,  5, 0x0),
    gsSP2Triangles( 2,  1,  3, 0x0,  2,  6,  0, 0x0),
    gsSP2Triangles( 4,  3,  7, 0x0,  5,  4,  7, 0x0),
    gsSP2Triangles( 8,  5,  9, 0x0,  8,  6,  5, 0x0),
    gsSP2Triangles( 9,  5,  7, 0x0,  0,  8,  1, 0x0),
    gsSP2Triangles( 1,  8,  3, 0x0,  8,  9,  3, 0x0),
    gsSP2Triangles( 6,  8,  0, 0x0,  3,  9,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500D268 - 0x0500D320
const Gfx unagi_seg5_dl_0500D268[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_PASS2),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(unagi_seg5_dl_0500D1C0),
    gsSPEndDisplayList(),
};

// 0x0500D320
static const Vtx unagi_seg5_vertex_0500D320[] = {
#include "assets/rom/generated/05886c794489e406d5b3.inc.c"









};

// 0x0500D3C0 - 0x0500D468
const Gfx unagi_seg5_dl_0500D3C0[] = {
    gsSPVertex(unagi_seg5_vertex_0500D320, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 5,  6,  2, 0x0,  2,  4,  5, 0x0),
    gsSP2Triangles( 2,  1,  3, 0x0,  2,  6,  0, 0x0),
    gsSP2Triangles( 4,  3,  7, 0x0,  5,  4,  7, 0x0),
    gsSP2Triangles( 8,  5,  9, 0x0,  8,  6,  5, 0x0),
    gsSP2Triangles( 9,  5,  7, 0x0,  0,  8,  1, 0x0),
    gsSP2Triangles( 1,  8,  3, 0x0,  8,  9,  3, 0x0),
    gsSP2Triangles( 6,  8,  0, 0x0,  3,  9,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500D468 - 0x0500D520
const Gfx unagi_seg5_dl_0500D468[] = {
    gsSPDisplayList(unagi_seg5_dl_0500D3C0),
    gsSPEndDisplayList(),
};

// 0x0500D520
static const Vtx unagi_seg5_vertex_0500D520[] = {
#include "assets/rom/generated/8896282b0ee83edf1cb1.inc.c"









};

// 0x0500D5C0 - 0x0500D668
const Gfx unagi_seg5_dl_0500D5C0[] = {
    gsSPVertex(unagi_seg5_vertex_0500D520, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 1,  4,  3, 0x0,  0,  4,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  3, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  5,  4,  0, 0x0),
    gsSP2Triangles( 3,  8,  2, 0x0,  3,  7,  9, 0x0),
    gsSP2Triangles( 8,  3,  9, 0x0,  2,  8,  6, 0x0),
    gsSP2Triangles( 6,  8,  9, 0x0,  7,  6,  9, 0x0),
    gsSP2Triangles( 6,  5,  2, 0x0,  2,  5,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500D668 - 0x0500D720
const Gfx unagi_seg5_dl_0500D668[] = {
    gsSPDisplayList(unagi_seg5_dl_0500D5C0),
    gsSPEndDisplayList(),
};

// 0x0500D720
static const Vtx unagi_seg5_vertex_0500D720[] = {
#include "assets/rom/generated/352ee904d96a7eccd5d5.inc.c"







};

// 0x0500D7A0 - 0x0500D828
const Gfx unagi_seg5_dl_0500D7A0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, unagi_seg5_texture_0500C3A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(unagi_seg5_vertex_0500D720, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 6,  2,  5, 0x0,  2,  6,  0, 0x0),
    gsSP2Triangles( 4,  7,  5, 0x0,  7,  4,  3, 0x0),
    gsSP2Triangles( 7,  6,  5, 0x0,  7,  1,  0, 0x0),
    gsSP2Triangles( 3,  1,  7, 0x0,  0,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500D828 - 0x0500D8E0
const Gfx unagi_seg5_dl_0500D828[] = {
    gsDPPipeSync(),
    gsSPDisplayList(unagi_seg5_dl_0500D7A0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0500D8E0
static const Vtx unagi_seg5_vertex_0500D8E0[] = {
#include "assets/rom/generated/2e8b0c3ac91a128cc9cc.inc.c"









};

// 0x0500D980
static const Vtx unagi_seg5_vertex_0500D980[] = {
#include "assets/rom/generated/54cca8b6ff0f0bfbc4dc.inc.c"














};

// 0x0500DA70
static const Vtx unagi_seg5_vertex_0500DA70[] = {
#include "assets/rom/generated/5ac70d68afd73f72559e.inc.c"














};

// 0x0500DB60 - 0x0500DBC0
const Gfx unagi_seg5_dl_0500DB60[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, unagi_seg5_texture_0500C320),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 8 * 8 - 1, CALC_DXT(8, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(unagi_seg5_vertex_0500D8E0, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  5,  4,  7, 0x0),
    gsSP2Triangles( 0,  7,  4, 0x0,  0,  8,  7, 0x0),
    gsSP1Triangle( 0,  9,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500DBC0 - 0x0500DD08
const Gfx unagi_seg5_dl_0500DBC0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, unagi_seg5_texture_0500B920),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(unagi_seg5_vertex_0500D980, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  2,  6,  3, 0x0),
    gsSP2Triangles( 5,  6,  2, 0x0,  7,  4,  2, 0x0),
    gsSP2Triangles( 1,  7,  2, 0x0,  8,  6,  5, 0x0),
    gsSP2Triangles( 9,  6,  8, 0x0,  6,  9,  3, 0x0),
    gsSP2Triangles( 3,  9, 10, 0x0, 10,  0,  3, 0x0),
    gsSP2Triangles( 9, 11, 10, 0x0,  8, 12, 13, 0x0),
    gsSP2Triangles(14, 13, 12, 0x0,  5,  4, 12, 0x0),
    gsSP2Triangles( 5, 12,  8, 0x0,  8, 11,  9, 0x0),
    gsSPVertex(unagi_seg5_vertex_0500DA70, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 5,  6,  3, 0x0,  6,  4,  3, 0x0),
    gsSP2Triangles( 2,  3,  0, 0x0,  1,  0,  7, 0x0),
    gsSP2Triangles( 0,  4,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 8,  2,  9, 0x0,  7,  4, 11, 0x0),
    gsSP2Triangles(11,  4,  6, 0x0,  7, 11, 12, 0x0),
    gsSP2Triangles( 7, 13,  1, 0x0, 12, 13,  7, 0x0),
    gsSP2Triangles(14,  6,  5, 0x0, 14, 12, 11, 0x0),
    gsSP1Triangle(11,  6, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500DD08 - 0x0500DDE0
const Gfx unagi_seg5_dl_0500DD08[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_PASS2),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 2, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 3, G_TX_NOLOD, G_TX_CLAMP, 3, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (8 - 1) << G_TEXTURE_IMAGE_FRAC, (8 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(unagi_seg5_dl_0500DB60),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(unagi_seg5_dl_0500DBC0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0500DDE0
static const Vtx unagi_seg5_vertex_0500DDE0[] = {
#include "assets/rom/generated/19b6d13da70f0e9b8100.inc.c"








};

// 0x0500DE70 - 0x0500DEB8
const Gfx unagi_seg5_dl_0500DE70[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, unagi_seg5_texture_0500B720),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 16 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(unagi_seg5_vertex_0500DDE0, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  7,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500DEB8 - 0x0500DF70
const Gfx unagi_seg5_dl_0500DEB8[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_TEX_EDGE2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_CLAMP, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(unagi_seg5_dl_0500DE70),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_TEX_EDGE, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0500DF70
static const Vtx unagi_seg5_vertex_0500DF70[] = {
#include "assets/rom/generated/286beba83caf96885019.inc.c"











};

// 0x0500E030 - 0x0500E088
const Gfx unagi_seg5_dl_0500E030[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, unagi_seg5_texture_0500C120),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 16 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(unagi_seg5_vertex_0500DF70, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11,  2, 0x0,  2,  1, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500E088 - 0x0500E140
const Gfx unagi_seg5_dl_0500E088[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_TEX_EDGE2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(unagi_seg5_dl_0500E030),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_TEX_EDGE, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x0500E140
static const Vtx unagi_seg5_vertex_0500E140[] = {
#include "assets/rom/generated/ca606fb5dcb4f6ed9576.inc.c"











};

// 0x0500E200 - 0x0500E258
const Gfx unagi_seg5_dl_0500E200[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, unagi_seg5_texture_0500C120),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 16 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(unagi_seg5_vertex_0500E140, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  9,  6, 0x0),
    gsSP2Triangles(10,  9,  8, 0x0,  8, 11, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500E258 - 0x0500E310
const Gfx unagi_seg5_dl_0500E258[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_TEX_EDGE2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(unagi_seg5_dl_0500E200),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_TEX_EDGE, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};
