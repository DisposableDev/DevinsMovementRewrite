// Yoshi

// 0x0501C458

// 0x0501C470

// 0x0501C488

// 0x0501C4A0
ALIGNED8 static const Texture yoshi_seg5_texture_0501C4A0[] = {
#include "actors/yoshi/yoshi_eye.rgba16.inc.c"
};

// 0x0501C6A0
ALIGNED8 static const Texture yoshi_seg5_texture_0501C6A0[] = {
#include "actors/yoshi/yoshi_eye_blink.rgba16.inc.c"
};

// 0x0501C8A0
ALIGNED8 static const Texture yoshi_seg5_texture_0501C8A0[] = {
#include "actors/yoshi/yoshi_nostril.rgba16.inc.c"
};

ALIGNED8 static const Texture yoshi_tank_top_texture[] = {
#include "actors/yoshi/tank_top.rgba16.inc.c"
};

// 0x0501CAA0
static const Vtx yoshi_seg5_vertex_0501CAA0[] = {
#include "assets/rom/generated/3c88a8a795cc6fae222c.inc.c"





};

// 0x0501CB00
static const Vtx yoshi_seg5_vertex_0501CB00[] = {
#include "assets/rom/generated/363378098e582d526c66.inc.c"










};

// 0x0501CBB0
static const Vtx yoshi_seg5_vertex_0501CBB0[] = {
#include "assets/rom/generated/d45e72babbe7832a0649.inc.c"




};

// 0x0501CC00
static const Vtx yoshi_seg5_vertex_0501CC00[] = {
#include "assets/rom/generated/20b0dcc7a40934700903.inc.c"














};

// 0x0501CCF0
static const Vtx yoshi_seg5_vertex_0501CCF0[] = {
#include "assets/rom/generated/4fdc5eb101a7caf5f5ea.inc.c"














};

// 0x0501CDE0
static const Vtx yoshi_seg5_vertex_0501CDE0[] = {
#include "assets/rom/generated/5a62d9335beb9bb63cf9.inc.c"















};

// 0x0501CEE0
static const Vtx yoshi_seg5_vertex_0501CEE0[] = {
#include "assets/rom/generated/5be22f4c38b097e3a050.inc.c"















};

// 0x0501CFE0
static const Vtx yoshi_seg5_vertex_0501CFE0[] = {
#include "assets/rom/generated/2edceb904b4aee9649ce.inc.c"















};

// 0x0501D0E0
static const Vtx yoshi_seg5_vertex_0501D0E0[] = {
#include "assets/rom/generated/b3efe00c67488bd224cd.inc.c"













};

// 0x0501D1C0
static const Vtx yoshi_seg5_vertex_0501D1C0[] = {
#include "assets/rom/generated/d94f1c2f50e6582d26e2.inc.c"















};

// 0x0501D2C0
static const Vtx yoshi_seg5_vertex_0501D2C0[] = {
#include "assets/rom/generated/a51cc84be950e680ba00.inc.c"






};

// 0x0501D330
static const Vtx yoshi_seg5_vertex_0501D330[] = {
#include "assets/rom/generated/f4242f5d14fafda0c377.inc.c"













};

// 0x0501D410
static const Vtx yoshi_seg5_vertex_0501D410[] = {
#include "assets/rom/generated/47c979aaf52db633a748.inc.c"


};

// 0x0501D440 - 0x0501D488
const Gfx yoshi_seg5_dl_0501D440[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, yoshi_seg5_texture_0501C8A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 16 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xf900ff),
    gsSPLightColor(LIGHT_2, 0x7c00ff),
    gsSPVertex(yoshi_seg5_vertex_0501CAA0, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0501D488 - 0x0501D4E0
const Gfx yoshi_seg5_dl_0501D488[] = {
    gsSPLightColor(LIGHT_1, 0x00f900ff),
    gsSPLightColor(LIGHT_2, 0x007c00ff),
    gsSPVertex(yoshi_seg5_vertex_0501CB00, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  5,  0,  3, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP1Triangle( 7, 10,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0501D4E0 - 0x0501D8F8
const Gfx yoshi_seg5_dl_0501D4E0[] = {
    gsSPLightColor(LIGHT_1, 0xff0000ff),
    gsSPLightColor(LIGHT_2, 0x7f0000ff),
    gsSPVertex(yoshi_seg5_vertex_0501CBB0, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  1,  3, 0x0,  2,  1,  4, 0x0),
    gsSPLightColor(LIGHT_1, 0xf900ff),
    gsSPLightColor(LIGHT_2, 0x7c00ff),
    gsSPVertex(yoshi_seg5_vertex_0501CC00, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  3, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 8,  4,  3, 0x0,  3,  7,  9, 0x0),
    gsSP2Triangles(10,  7,  6, 0x0,  7, 10, 11, 0x0),
    gsSP2Triangles( 9,  7, 11, 0x0, 12,  9, 11, 0x0),
    gsSP2Triangles( 4,  8,  0, 0x0,  0,  8, 12, 0x0),
    gsSP2Triangles(13,  4,  0, 0x0,  5,  4, 13, 0x0),
    gsSP2Triangles(12, 14,  0, 0x0,  1,  0, 14, 0x0),
    gsSP1Triangle( 2, 13,  0, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501CCF0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  2,  5,  4, 0x0),
    gsSP2Triangles( 3,  1,  6, 0x0,  7,  3,  6, 0x0),
    gsSP2Triangles( 8,  3,  7, 0x0,  3,  8,  9, 0x0),
    gsSP2Triangles( 9, 10,  3, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(12, 10, 11, 0x0, 13, 10, 12, 0x0),
    gsSP1Triangle(14, 13, 12, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501CDE0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 2,  5,  6, 0x0,  2,  1,  7, 0x0),
    gsSP2Triangles( 3,  2,  7, 0x0,  2,  6,  0, 0x0),
    gsSP2Triangles( 8,  6,  9, 0x0, 10,  6,  8, 0x0),
    gsSP2Triangles(10,  0,  6, 0x0,  9,  6,  5, 0x0),
    gsSP2Triangles( 4,  3, 11, 0x0,  5, 12,  9, 0x0),
    gsSP2Triangles( 9, 13,  8, 0x0, 11, 14, 15, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501CEE0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  2,  5,  3, 0x0),
    gsSP2Triangles( 1,  7,  2, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 6,  5, 11, 0x0, 11,  5,  2, 0x0),
    gsSP2Triangles( 2, 12, 11, 0x0, 13, 11, 12, 0x0),
    gsSP1Triangle(14,  8, 15, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501CFE0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  4, 0x0),
    gsSP2Triangles( 5,  1,  6, 0x0,  1,  7,  2, 0x0),
    gsSP2Triangles( 3,  7,  1, 0x0,  8,  9,  3, 0x0),
    gsSP2Triangles( 2,  7, 10, 0x0,  0,  2, 11, 0x0),
    gsSP2Triangles(10, 12,  2, 0x0, 13, 12, 14, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 12, 10, 14, 0x0),
    gsSP2Triangles(14, 10,  3, 0x0,  9, 15, 14, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501D0E0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  0, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  8, 12,  5, 0x0),
    gsSP2Triangles(13,  8,  4, 0x0,  5, 12,  1, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501D1C0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 2,  4,  0, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 4,  6,  0, 0x0,  3,  7,  4, 0x0),
    gsSP2Triangles( 4,  8,  6, 0x0,  0,  9, 10, 0x0),
    gsSP2Triangles(11,  1,  0, 0x0,  0,  6,  9, 0x0),
    gsSP2Triangles( 0, 10, 11, 0x0, 12,  7,  3, 0x0),
    gsSP2Triangles( 5,  1, 11, 0x0, 13, 11, 10, 0x0),
    gsSP2Triangles(10, 14, 15, 0x0, 15, 13, 10, 0x0),
    gsSP1Triangle(10,  9, 14, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501D2C0, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  3,  5,  6, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(yoshi_seg5_vertex_0501D330, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  8, 0x0),
    gsSP2Triangles( 4,  7,  5, 0x0,  7,  9, 10, 0x0),
    gsSP2Triangles( 8,  9,  7, 0x0,  6,  5, 11, 0x0),
    gsSP2Triangles(12, 13,  5, 0x0,  5, 13, 11, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501D410, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0501D8F8 - 0x0501D940
const Gfx yoshi_seg5_dl_0501D8F8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_CLAMP, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(yoshi_seg5_dl_0501D440),
    gsSPEndDisplayList(),
};

// Opaque sunglasses follow the head through walking, blinking and the departure jump.
static const Vtx yoshi_sunglasses_vertices_0[] = {
    {{{305, 126, -4}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{350, 70, -132}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, 80, -114}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{305, 126, -4}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, 80, -114}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{205, 138, -95}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{305, 126, -4}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{205, 138, -95}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{205, 148, -4}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, 80, -114}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{350, 70, -132}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, -28, -108}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, 80, -114}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, -28, -108}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{240, -28, -108}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, 80, -114}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{240, -28, -108}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{205, 138, -95}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{293, 136, -35}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{292, 135, -47}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{213, 149, -63}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{293, 136, -35}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{213, 149, -63}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{213, 150, -51}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{291, 134, -65}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{300, 133, -70}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{213, 147, -86}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{291, 134, -65}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{213, 147, -86}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{213, 148, -81}, 0, {0, 0}, {225, 225, 225, 255}}},
};

static const Vtx yoshi_sunglasses_vertices_1[] = {
    {{{305, 126, 4}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{350, 70, 132}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, 80, 114}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{305, 126, 4}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, 80, 114}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{205, 138, 95}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{305, 126, 4}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{205, 138, 95}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{205, 148, 4}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, 80, 114}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{350, 70, 132}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, -28, 108}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, 80, 114}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, -28, 108}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{240, -28, 108}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{300, 80, 114}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{240, -28, 108}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{205, 138, 95}, 0, {0, 0}, {10, 10, 10, 255}}},
    {{{293, 136, 35}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{292, 135, 47}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{213, 149, 63}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{293, 136, 35}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{213, 149, 63}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{213, 150, 51}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{291, 134, 65}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{300, 133, 70}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{213, 147, 86}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{291, 134, 65}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{213, 147, 86}, 0, {0, 0}, {225, 225, 225, 255}}},
    {{{213, 148, 81}, 0, {0, 0}, {225, 225, 225, 255}}},
};

static const Gfx yoshi_sunglasses[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPVertex(yoshi_sunglasses_vertices_0, 30, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSP1Triangle(3, 4, 5, 0),
    gsSP1Triangle(6, 7, 8, 0),
    gsSP1Triangle(9, 10, 11, 0),
    gsSP1Triangle(12, 13, 14, 0),
    gsSP1Triangle(15, 16, 17, 0),
    gsSP1Triangle(18, 19, 20, 0),
    gsSP1Triangle(21, 22, 23, 0),
    gsSP1Triangle(24, 25, 26, 0),
    gsSP1Triangle(27, 28, 29, 0),
    gsSPVertex(yoshi_sunglasses_vertices_1, 30, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSP1Triangle(3, 4, 5, 0),
    gsSP1Triangle(6, 7, 8, 0),
    gsSP1Triangle(9, 10, 11, 0),
    gsSP1Triangle(12, 13, 14, 0),
    gsSP1Triangle(15, 16, 17, 0),
    gsSP1Triangle(18, 19, 20, 0),
    gsSP1Triangle(21, 22, 23, 0),
    gsSP1Triangle(24, 25, 26, 0),
    gsSP1Triangle(27, 28, 29, 0),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x0501D940 - 0x0501D970
const Gfx yoshi_seg5_dl_0501D940[] = {
    gsSPDisplayList(yoshi_seg5_dl_0501D488),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(yoshi_seg5_dl_0501D4E0),
    gsSPDisplayList(yoshi_sunglasses),
    gsSPEndDisplayList(),
};

// 0x0501D970 - 0x0501D998
const Gfx yoshi_seg5_dl_0501D970[] = {
    gsSPDisplayList(yoshi_seg5_dl_0501D8F8),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, yoshi_seg5_texture_0501C4A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 16 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(yoshi_seg5_dl_0501D940),
};

// 0x0501D998 - 0x0501D9C0
const Gfx yoshi_seg5_dl_0501D998[] = {
    gsSPDisplayList(yoshi_seg5_dl_0501D8F8),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, yoshi_seg5_texture_0501C6A0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 16 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(yoshi_seg5_dl_0501D940),
};

// 0x0501D9C0

// 0x0501D9D8

// 0x0501D9F0

// Unreferenced light group

// Unreferenced light group

// 0x0501DA38

// 0x0501DA50
static const Vtx yoshi_seg5_vertex_0501DA50[] = {
#include "assets/rom/generated/bcefe410a04055b0c298.inc.c"














};

// 0x0501DB40
static const Vtx yoshi_seg5_vertex_0501DB40[] = {
#include "assets/rom/generated/534e18a16731aa6db3d1.inc.c"














};

// 0x0501DC30
static const Vtx yoshi_seg5_vertex_0501DC30[] = {
#include "assets/rom/generated/ab7b27de12fc4136d1a9.inc.c"













};

// 0x0501DD10
static const Vtx yoshi_seg5_vertex_0501DD10[] = {
#include "assets/rom/generated/7f70da9608f9c720ff36.inc.c"















};

// 0x0501DE10
static const Vtx yoshi_seg5_vertex_0501DE10[] = {
#include "assets/rom/generated/c2dab484ef172d05fdf0.inc.c"














};

// 0x0501DF00
static const Vtx yoshi_seg5_vertex_0501DF00[] = {
#include "assets/rom/generated/e6a304a8cfb28da0830f.inc.c"














};

// 0x0501DFF0
static const Vtx yoshi_seg5_vertex_0501DFF0[] = {
#include "assets/rom/generated/7dbb769692ed8beb2c91.inc.c"













};

// 0x0501E0D0
static const Vtx yoshi_seg5_vertex_0501E0D0[] = {
#include "assets/rom/generated/0b19f60d4facb7a578c7.inc.c"













};

// 0x0501E1B0
static const Vtx yoshi_seg5_vertex_0501E1B0[] = {
#include "assets/rom/generated/99c2e45ab551c1ccf74b.inc.c"


};

// 0x0501E1E0
static const Vtx yoshi_seg5_vertex_0501E1E0[] = {
#include "assets/rom/generated/4a2db2f03db2149b228e.inc.c"













};

// 0x0501E2C0
static const Vtx yoshi_seg5_vertex_0501E2C0[] = {
#include "assets/rom/generated/d64f43b8e365f8f2ef67.inc.c"















};

// 0x0501E3C0
static const Vtx yoshi_seg5_vertex_0501E3C0[] = {
#include "assets/rom/generated/726acafbbea84bcfc876.inc.c"














};

// 0x0501E4B0
static const Vtx yoshi_seg5_vertex_0501E4B0[] = {
#include "assets/rom/generated/9bab519610738f79c937.inc.c"







};

// 0x0501E530
static const Vtx yoshi_seg5_vertex_0501E530[] = {
#include "assets/rom/generated/b183929c470339bd81d3.inc.c"









};

// 0x0501E5D0
static const Vtx yoshi_seg5_vertex_0501E5D0[] = {
#include "assets/rom/generated/0f56c26ee97669f4f6f0.inc.c"










};

// 0x0501E680
static const Vtx yoshi_seg5_vertex_0501E680[] = {
#include "assets/rom/generated/fa45a8a25f06eb826311.inc.c"













};

// 0x0501E760
static const Vtx yoshi_seg5_vertex_0501E760[] = {
#include "assets/rom/generated/ae0e2102695b69ec8397.inc.c"















};

// 0x0501E860
static const Vtx yoshi_seg5_vertex_0501E860[] = {
#include "assets/rom/generated/9e8e210332e35808def3.inc.c"














};

// 0x0501E950
static const Vtx yoshi_seg5_vertex_0501E950[] = {
#include "assets/rom/generated/d7105716df200894cd4a.inc.c"







};

// 0x0501E9D0
static const Vtx yoshi_seg5_vertex_0501E9D0[] = {
#include "assets/rom/generated/22c6800e5c458a183f37.inc.c"









};

// 0x0501EA70
static const Vtx yoshi_seg5_vertex_0501EA70[] = {
#include "assets/rom/generated/17a7f14a5352781b7e1e.inc.c"










};

// 0x0501EB20
static const Vtx yoshi_seg5_vertex_0501EB20[] = {
#include "assets/rom/generated/70e3cbca13c17d9ab319.inc.c"














};

// 0x0501EC10
static const Vtx yoshi_seg5_vertex_0501EC10[] = {
#include "assets/rom/generated/f93371cdfd142e5ca939.inc.c"















};

// 0x0501ED10
static const Vtx yoshi_seg5_vertex_0501ED10[] = {
#include "assets/rom/generated/bd34da82543c4e2d641a.inc.c"















};

// 0x0501EE10
static const Vtx yoshi_seg5_vertex_0501EE10[] = {
#include "assets/rom/generated/1c4d793e8b356914304d.inc.c"















};

// 0x0501EF10
static const Vtx yoshi_seg5_vertex_0501EF10[] = {
#include "assets/rom/generated/8d5fcb2ad504719ea73b.inc.c"














};

// 0x0501F000
static const Vtx yoshi_seg5_vertex_0501F000[] = {
#include "assets/rom/generated/900b1dbbdff284f4ef84.inc.c"















};

// 0x0501F100
static const Vtx yoshi_seg5_vertex_0501F100[] = {
#include "assets/rom/generated/5a1f27439b3273e4df6a.inc.c"







};

// 0x0501F180
static const Vtx yoshi_seg5_vertex_0501F180[] = {
#include "assets/rom/generated/0991317b02c5a6fdf9c4.inc.c"















};

// 0x0501F280
static const Vtx yoshi_seg5_vertex_0501F280[] = {
#include "assets/rom/generated/a2335631e09d95e30760.inc.c"















};

// 0x0501F380
static const Vtx yoshi_seg5_vertex_0501F380[] = {
#include "assets/rom/generated/ca30706b7f9e7916b7dd.inc.c"















};

// 0x0501F480
static const Vtx yoshi_seg5_vertex_0501F480[] = {
#include "assets/rom/generated/ccfcd28b07aaa855fd13.inc.c"











};

// 0x0501F540
static const Vtx yoshi_seg5_vertex_0501F540[] = {
#include "assets/rom/generated/86f97e340c8b7fb56085.inc.c"















};

// 0x0501F640
static const Vtx yoshi_seg5_vertex_0501F640[] = {
#include "assets/rom/generated/9dbe403f1e19beab4226.inc.c"














};

// 0x0501F730
static const Vtx yoshi_seg5_vertex_0501F730[] = {
#include "assets/rom/generated/8f4cb1418783c1e21f64.inc.c"














};

// 0x0501F820
static const Vtx yoshi_seg5_vertex_0501F820[] = {
#include "assets/rom/generated/bed33db4eb89b166a53d.inc.c"














};

// 0x0501F910
static const Vtx yoshi_seg5_vertex_0501F910[] = {
#include "assets/rom/generated/bd75eb9cf7a7c39ce092.inc.c"














};

// 0x0501FA00
static const Vtx yoshi_seg5_vertex_0501FA00[] = {
#include "assets/rom/generated/77b22c033b486a6587f4.inc.c"














};

// 0x0501FAF0
static const Vtx yoshi_seg5_vertex_0501FAF0[] = {
#include "assets/rom/generated/f038b4f186ce47f92365.inc.c"










};

// 0x0501FBA0
static const Vtx yoshi_seg5_vertex_0501FBA0[] = {
#include "assets/rom/generated/cee70bfec1b23cbf73e9.inc.c"














};

// 0x0501FC90
static const Vtx yoshi_seg5_vertex_0501FC90[] = {
#include "assets/rom/generated/037ad3d3b12607c3aa2d.inc.c"















};

// 0x0501FD90
static const Vtx yoshi_seg5_vertex_0501FD90[] = {
#include "assets/rom/generated/ad02ba874cce50dbf17e.inc.c"















};

// 0x0501FE90
static const Vtx yoshi_seg5_vertex_0501FE90[] = {
#include "assets/rom/generated/ef66e582a0d9f96af0b6.inc.c"















};

// 0x0501FF90
static const Vtx yoshi_seg5_vertex_0501FF90[] = {
#include "assets/rom/generated/a8b45799a8af959076a4.inc.c"














};

// 0x05020080
static const Vtx yoshi_seg5_vertex_05020080[] = {
#include "assets/rom/generated/e799a83bc1d7200dc616.inc.c"















};

// 0x05020180
static const Vtx yoshi_seg5_vertex_05020180[] = {
#include "assets/rom/generated/dd4cae052b7b4a57ae88.inc.c"







};

// 0x05020200
static const Vtx yoshi_seg5_vertex_05020200[] = {
#include "assets/rom/generated/55f3fff4cec5ca1537cb.inc.c"















};

// 0x05020300
static const Vtx yoshi_seg5_vertex_05020300[] = {
#include "assets/rom/generated/3ccdfd9322355f5fce5a.inc.c"















};

// 0x05020400
static const Vtx yoshi_seg5_vertex_05020400[] = {
#include "assets/rom/generated/002eaa11b68cd5f7f0d7.inc.c"















};

// 0x05020500
static const Vtx yoshi_seg5_vertex_05020500[] = {
#include "assets/rom/generated/1364d315ff429b9e7e47.inc.c"











};

// 0x050205C0
static const Vtx yoshi_seg5_vertex_050205C0[] = {
#include "assets/rom/generated/a81b3ea2f8d29a3723f1.inc.c"















};

// 0x050206C0
static const Vtx yoshi_seg5_vertex_050206C0[] = {
#include "assets/rom/generated/cfeb9570416ccba9ddb7.inc.c"














};

// 0x050207B0
static const Vtx yoshi_seg5_vertex_050207B0[] = {
#include "assets/rom/generated/5a900a462431954a8574.inc.c"














};

// 0x050208A0
static const Vtx yoshi_seg5_vertex_050208A0[] = {
#include "assets/rom/generated/e5ca925845c774da38ab.inc.c"














};

// 0x05020990
static const Vtx yoshi_seg5_vertex_05020990[] = {
#include "assets/rom/generated/d5d80e43bccbd6b31a9d.inc.c"














};

// 0x05020A80
static const Vtx yoshi_seg5_vertex_05020A80[] = {
#include "assets/rom/generated/2a40f80fe151ada6a00f.inc.c"















};

// 0x05020B80
static const Vtx yoshi_seg5_vertex_05020B80[] = {
#include "assets/rom/generated/824ac71980dfea6ad8bc.inc.c"















};

// 0x05020C80
static const Vtx yoshi_seg5_vertex_05020C80[] = {
#include "assets/rom/generated/145bc0e9a4d05d99f91b.inc.c"













};

// 0x05020D60
static const Vtx yoshi_seg5_vertex_05020D60[] = {
#include "assets/rom/generated/7e3d5430a5aa92c36c77.inc.c"















};

// 0x05020E60
static const Vtx yoshi_seg5_vertex_05020E60[] = {
#include "assets/rom/generated/f55bab79db2ecf38448f.inc.c"


};

// 0x05020E90
static const Vtx yoshi_seg5_vertex_05020E90[] = {
#include "assets/rom/generated/6e032105706eb465c1a7.inc.c"















};

// 0x05020F90
static const Vtx yoshi_seg5_vertex_05020F90[] = {
#include "assets/rom/generated/41955223bc206c5e1ac9.inc.c"







};

// 0x05021010
static const Vtx yoshi_seg5_vertex_05021010[] = {
#include "assets/rom/generated/d27ad684514ccaff92aa.inc.c"















};

// 0x05021110
static const Vtx yoshi_seg5_vertex_05021110[] = {
#include "assets/rom/generated/418007e117b64bb499dc.inc.c"











};

// 0x050211D0
static const Vtx yoshi_seg5_vertex_050211D0[] = {
#include "assets/rom/generated/cad5cd29e41d9e32e01c.inc.c"













};

// 0x050212B0 - 0x05021490
const Gfx yoshi_seg5_dl_050212B0[] = {
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(yoshi_seg5_vertex_0501DA50, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501DB40, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501DC30, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle( 2, 12, 13, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501DD10, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 8, 12,  0, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501DE10, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles(11, 12,  9, 0x0, 13,  0, 14, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501DF00, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  9,  5, 0x0),
    gsSP2Triangles(10,  3,  2, 0x0,  1, 11,  2, 0x0),
    gsSP2Triangles(12, 10, 11, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle( 9,  8, 13, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501DFF0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 10, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501E0D0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  8, 10, 0x0),
    gsSP2Triangles( 1,  4,  3, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501E1B0, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x05021490 - 0x050215D8
const Gfx yoshi_seg5_dl_05021490[] = {
    gsSPLightColor(LIGHT_1, 0xf700ff),
    gsSPLightColor(LIGHT_2, 0x7b00ff),
    gsSPVertex(yoshi_seg5_vertex_0501E1E0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle( 6, 12, 13, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501E2C0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  3,  2, 11, 0x0),
    gsSP2Triangles( 3, 11,  9, 0x0, 12, 13, 14, 0x0),
    gsSP2Triangles(12, 14, 11, 0x0,  2,  1, 15, 0x0),
    gsSP1Triangle( 2, 15, 12, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501E3C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  0, 0x0),
    gsSP2Triangles( 7,  8,  6, 0x0,  9,  8,  7, 0x0),
    gsSP2Triangles( 5, 10,  7, 0x0, 11,  7, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  7,  6,  5, 0x0),
    gsSP1Triangle( 0,  3,  4, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501E4B0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 5,  6,  3, 0x0,  5,  3,  0, 0x0),
    gsSP1Triangle( 7,  6,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x050215D8 - 0x05021678
const Gfx yoshi_seg5_dl_050215D8[] = {
    gsSPLightColor(LIGHT_1, 0xf700ff),
    gsSPLightColor(LIGHT_2, 0x7b00ff),
    gsSPVertex(yoshi_seg5_vertex_0501E530, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  2,  6,  7, 0x0),
    gsSP2Triangles( 5,  8,  6, 0x0,  2,  9,  0, 0x0),
    gsSP2Triangles( 9,  7,  4, 0x0,  2,  5,  6, 0x0),
    gsSP2Triangles( 6,  8,  7, 0x0,  7,  8,  4, 0x0),
    gsSP2Triangles( 4,  8,  5, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 4,  3,  9, 0x0,  9,  3,  0, 0x0),
    gsSP2Triangles( 7,  9,  2, 0x0,  1,  3,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x05021678 - 0x05021718
const Gfx yoshi_seg5_dl_05021678[] = {
    gsSPLightColor(LIGHT_1, 0xf700ff),
    gsSPLightColor(LIGHT_2, 0x7b00ff),
    gsSPVertex(yoshi_seg5_vertex_0501E5D0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  7, 0x0,  4,  8,  1, 0x0),
    gsSP2Triangles( 2,  9, 10, 0x0,  4,  0,  7, 0x0),
    gsSP2Triangles( 1,  8,  2, 0x0,  2,  8,  3, 0x0),
    gsSP2Triangles( 3,  8,  4, 0x0,  5,  6,  9, 0x0),
    gsSP2Triangles( 9,  6, 10, 0x0, 10,  6,  0, 0x0),
    gsSP2Triangles( 4,  1,  0, 0x0,  2,  3,  9, 0x0),
    gsSP2Triangles( 5,  9,  3, 0x0,  2, 10,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x05021718 - 0x05021860
const Gfx yoshi_seg5_dl_05021718[] = {
    gsSPLightColor(LIGHT_1, 0xf700ff),
    gsSPLightColor(LIGHT_2, 0x7b00ff),
    gsSPVertex(yoshi_seg5_vertex_0501E680, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13,  5, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501E760, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11,  1, 0x0),
    gsSP2Triangles( 9,  1,  0, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0, 14, 15,  2, 0x0),
    gsSP1Triangle(14,  2,  1, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501E860, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 4,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8, 10,  5, 0x0, 10,  8, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  5,  4,  8, 0x0),
    gsSP1Triangle( 6,  0,  3, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501E950, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  2,  5,  6, 0x0),
    gsSP1Triangle( 6,  5,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x05021860 - 0x05021900
const Gfx yoshi_seg5_dl_05021860[] = {
    gsSPLightColor(LIGHT_1, 0xf700ff),
    gsSPLightColor(LIGHT_2, 0x7b00ff),
    gsSPVertex(yoshi_seg5_vertex_0501E9D0, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  6,  7,  0, 0x0),
    gsSP2Triangles( 7,  8,  4, 0x0,  2,  9,  0, 0x0),
    gsSP2Triangles( 5,  6,  9, 0x0,  7,  4,  0, 0x0),
    gsSP2Triangles( 6,  8,  7, 0x0,  5,  8,  6, 0x0),
    gsSP2Triangles( 4,  8,  5, 0x0,  4,  1,  0, 0x0),
    gsSP2Triangles( 9,  3,  5, 0x0,  2,  3,  9, 0x0),
    gsSP2Triangles( 0,  9,  6, 0x0,  5,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x05021900 - 0x050219A0
const Gfx yoshi_seg5_dl_05021900[] = {
    gsSPLightColor(LIGHT_1, 0xf700ff),
    gsSPLightColor(LIGHT_2, 0x7b00ff),
    gsSPVertex(yoshi_seg5_vertex_0501EA70, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  2, 0x0,  1,  8,  4, 0x0),
    gsSP2Triangles( 9, 10,  0, 0x0,  6,  2,  4, 0x0),
    gsSP2Triangles( 0,  8,  1, 0x0,  5,  8,  0, 0x0),
    gsSP2Triangles( 4,  8,  5, 0x0, 10,  7,  3, 0x0),
    gsSP2Triangles( 9,  7, 10, 0x0,  2,  7,  9, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0, 10,  5,  0, 0x0),
    gsSP2Triangles( 5, 10,  3, 0x0,  2,  9,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x050219A0 - 0x05021B90
const Gfx yoshi_seg5_dl_050219A0[] = {
    gsSPLightColor(LIGHT_1, 0xff0000ff),
    gsSPLightColor(LIGHT_2, 0x7f0000ff),
    gsSPVertex(yoshi_seg5_vertex_0501EB20, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(10, 14,  8, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501EC10, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  4,  9,  5, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  8, 13, 14, 0x0),
    gsSP1Triangle(15, 10, 12, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501ED10, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  0,  2,  9, 0x0),
    gsSP2Triangles(10,  3,  5, 0x0, 11, 10,  5, 0x0),
    gsSP2Triangles( 5, 12, 11, 0x0, 13, 14,  6, 0x0),
    gsSP1Triangle(14, 15,  6, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501EE10, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  4, 0x0,  2,  8,  0, 0x0),
    gsSP2Triangles( 4,  0,  5, 0x0,  4,  9,  6, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501EF10, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 2,  7,  0, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle(10,  3,  5, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501F000, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 12, 10, 0x0,  7, 13,  8, 0x0),
    gsSP2Triangles( 5, 14,  3, 0x0,  2, 15,  0, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501F100, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  0,  4,  5, 0x0),
    gsSP2Triangles( 0,  5,  6, 0x0,  0,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x05021B90 - 0x05021CD8
const Gfx yoshi_seg5_dl_05021B90[] = {
    gsSPLightColor(LIGHT_1, 0xf700ff),
    gsSPLightColor(LIGHT_2, 0x7b00ff),
    gsSPVertex(yoshi_seg5_vertex_0501F180, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  3,  7,  4, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12,  4, 0x0),
    gsSP2Triangles( 7,  3, 13, 0x0, 14,  5,  4, 0x0),
    gsSP2Triangles( 9,  5, 14, 0x0, 13,  3, 15, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501F280, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 2, 12, 13, 0x0, 14, 12, 11, 0x0),
    gsSP1Triangle(15,  3,  5, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501F380, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  7, 12,  0, 0x0),
    gsSP2Triangles( 0, 15,  7, 0x0, 14,  1, 12, 0x0),
    gsSP2Triangles( 9, 13, 10, 0x0,  6, 10,  7, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501F480, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  4,  6, 0x0,  5,  7,  8, 0x0),
    gsSP2Triangles( 9,  1,  0, 0x0, 10,  8,  7, 0x0),
    gsSP1Triangle( 1,  9, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x05021CD8 - 0x05021E38
const Gfx yoshi_seg5_dl_05021CD8[] = {
    gsSPLightColor(LIGHT_1, 0xf700ff),
    gsSPLightColor(LIGHT_2, 0x7b00ff),
    gsSPVertex(yoshi_seg5_vertex_0501F540, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  3, 15,  4, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501F640, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  7, 0x0,  2,  1,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501F730, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  7,  5, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  0,  2, 13, 0x0),
    gsSP2Triangles( 8, 14, 10, 0x0,  6,  3,  5, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501F820, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 6, 10,  5, 0x0,  2, 11,  3, 0x0),
    gsSP2Triangles(12,  7, 13, 0x0,  8, 14,  6, 0x0),
    gsSP1Triangle(11,  1, 13, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501F910, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 2,  1,  6, 0x0,  4,  3,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12,  2, 0x0),
    gsSP1Triangle(13, 14,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x05021E38 - 0x05021F70
const Gfx yoshi_seg5_dl_05021E38[] = {
    gsSPLightColor(LIGHT_1, 0xf700ff),
    gsSPLightColor(LIGHT_2, 0x7b00ff),
    gsSPVertex(yoshi_seg5_vertex_0501FA00, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 4,  5,  3, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 2,  9,  3, 0x0,  8, 10,  2, 0x0),
    gsSP2Triangles( 8,  2, 11, 0x0,  3, 12,  4, 0x0),
    gsSP2Triangles( 2, 10,  9, 0x0,  7, 13, 10, 0x0),
    gsSP2Triangles( 5, 14,  0, 0x0,  0, 14,  1, 0x0),
    gsSP2Triangles( 1, 14, 11, 0x0, 11, 14,  6, 0x0),
    gsSP2Triangles(10,  8,  7, 0x0,  3,  9, 12, 0x0),
    gsSP2Triangles( 2,  1, 11, 0x0,  8, 11,  6, 0x0),
    gsSP1Triangle( 5,  0,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(yoshi_seg5_vertex_0501FAF0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  7,  6,  8, 0x0),
    gsSP2Triangles( 1,  9,  2, 0x0,  3,  4,  8, 0x0),
    gsSP2Triangles( 2,  4,  0, 0x0,  8,  6, 10, 0x0),
    gsSP2Triangles( 9,  5,  7, 0x0,  8, 10,  3, 0x0),
    gsSP2Triangles( 7,  2,  9, 0x0,  7,  4,  2, 0x0),
    gsSP1Triangle( 7,  8,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x05021F70 - 0x05022160
const Gfx yoshi_seg5_dl_05021F70[] = {
    gsSPLightColor(LIGHT_1, 0xff0000ff),
    gsSPLightColor(LIGHT_2, 0x7f0000ff),
    gsSPVertex(yoshi_seg5_vertex_0501FBA0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(10, 14,  8, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501FC90, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  3,  9,  4, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14,  6, 0x0),
    gsSP1Triangle(10, 12, 15, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501FD90, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  0,  2, 0x0),
    gsSP2Triangles( 3,  5, 10, 0x0,  3, 10, 11, 0x0),
    gsSP2Triangles(11, 12,  3, 0x0,  8, 13, 14, 0x0),
    gsSP1Triangle( 8, 15, 13, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501FE90, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  2,  8,  0, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  7,  9,  4, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(yoshi_seg5_vertex_0501FF90, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 2,  7,  0, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle( 3,  5, 10, 0x0),
    gsSPVertex(yoshi_seg5_vertex_05020080, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(10, 12, 11, 0x0,  6, 13,  7, 0x0),
    gsSP2Triangles( 5, 14,  3, 0x0,  2, 15,  0, 0x0),
    gsSPVertex(yoshi_seg5_vertex_05020180, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  3,  2, 0x0,  5,  4,  2, 0x0),
    gsSP2Triangles( 6,  5,  2, 0x0,  7,  6,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x05022160 - 0x050222A8
const Gfx yoshi_seg5_dl_05022160[] = {
    gsSPLightColor(LIGHT_1, 0xf700ff),
    gsSPLightColor(LIGHT_2, 0x7b00ff),
    gsSPVertex(yoshi_seg5_vertex_05020200, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 1,  6,  2, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  4, 11, 12, 0x0),
    gsSP2Triangles(13,  5,  7, 0x0,  4,  3, 14, 0x0),
    gsSP2Triangles(14,  3,  9, 0x0, 15,  5, 13, 0x0),
    gsSPVertex(yoshi_seg5_vertex_05020300, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13,  0, 0x0,  9, 13, 14, 0x0),
    gsSP1Triangle( 3,  5, 15, 0x0),
    gsSPVertex(yoshi_seg5_vertex_05020400, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  2, 14,  7, 0x0),
    gsSP2Triangles( 7, 15,  2, 0x0, 14,  1, 12, 0x0),
    gsSP2Triangles(10, 13, 11, 0x0,  7, 10,  8, 0x0),
    gsSPVertex(yoshi_seg5_vertex_05020500, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  2, 0x0,  7,  8,  3, 0x0),
    gsSP2Triangles( 2,  1,  9, 0x0,  8,  7, 10, 0x0),
    gsSP1Triangle(11,  9,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x050222A8 - 0x05022408
const Gfx yoshi_seg5_dl_050222A8[] = {
    gsSPLightColor(LIGHT_1, 0xf700ff),
    gsSPLightColor(LIGHT_2, 0x7b00ff),
    gsSPVertex(yoshi_seg5_vertex_050205C0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  4, 15,  5, 0x0),
    gsSPVertex(yoshi_seg5_vertex_050206C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  7, 0x0,  8,  1,  0, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(yoshi_seg5_vertex_050207B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  3,  7,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13,  0,  2, 0x0),
    gsSP2Triangles(12, 14,  6, 0x0,  3,  5,  8, 0x0),
    gsSPVertex(yoshi_seg5_vertex_050208A0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 5,  3,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 6, 10,  5, 0x0,  4, 11,  0, 0x0),
    gsSP2Triangles(12,  9, 13, 0x0,  5, 14,  8, 0x0),
    gsSP1Triangle(12,  1, 11, 0x0),
    gsSPVertex(yoshi_seg5_vertex_05020990, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  1,  0, 0x0,  7,  5,  4, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  0, 11, 12, 0x0),
    gsSP1Triangle( 4, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x05022408 - 0x05022608
const Gfx yoshi_seg5_dl_05022408[] = {
    // Lighting is baked at vertex load; reload the same batch when its material changes.
    gsSPLightColor(LIGHT_1, 0x00f700ff),
    gsSPLightColor(LIGHT_2, 0x007b00ff),
    gsSPVertex(yoshi_seg5_vertex_05020A80, 16, 0),
    gsSP2Triangles(0, 1, 2, 0, 6, 7, 8, 0),
    gsSP1Triangle(9, 10, 11, 0),
    gsSPLightColor(LIGHT_1, 0x18181aff),
    gsSPLightColor(LIGHT_2, 0x0b0b0cff),
    gsSPVertex(yoshi_seg5_vertex_05020A80, 16, 0),
    gsSP2Triangles(3, 4, 5, 0, 12, 4, 13, 0),
    gsSP1Triangle(14, 5, 15, 0),
    gsSPLightColor(LIGHT_1, 0x18181aff),
    gsSPLightColor(LIGHT_2, 0x0b0b0cff),
    gsSPVertex(yoshi_seg5_vertex_05020B80, 16, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 7, 2, 0, 3, 8, 9, 0),
    gsSP2Triangles(9, 10, 11, 0, 7, 10, 8, 0),
    gsSP1Triangle(12, 13, 6, 0),
    gsSPLightColor(LIGHT_1, 0x00f700ff),
    gsSPLightColor(LIGHT_2, 0x007b00ff),
    gsSPVertex(yoshi_seg5_vertex_05020B80, 16, 0),
    gsSP1Triangle(1, 14, 15, 0),
    gsSPLightColor(LIGHT_1, 0x00f700ff),
    gsSPLightColor(LIGHT_2, 0x007b00ff),
    gsSPVertex(yoshi_seg5_vertex_05020C80, 14, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSPLightColor(LIGHT_1, 0x18181aff),
    gsSPLightColor(LIGHT_2, 0x0b0b0cff),
    gsSPVertex(yoshi_seg5_vertex_05020C80, 14, 0),
    gsSP2Triangles(3, 4, 5, 0, 6, 7, 8, 0),
    gsSP2Triangles(9, 10, 11, 0, 11, 12, 9, 0),
    gsSP1Triangle(2, 13, 0, 0),
    gsSPLightColor(LIGHT_1, 0x18181aff),
    gsSPLightColor(LIGHT_2, 0x0b0b0cff),
    gsSPVertex(yoshi_seg5_vertex_05020D60, 16, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 7, 8, 0, 3, 9, 1, 0),
    gsSP1Triangle(4, 14, 15, 0),
    gsSPLightColor(LIGHT_1, 0x00f700ff),
    gsSPLightColor(LIGHT_2, 0x007b00ff),
    gsSPVertex(yoshi_seg5_vertex_05020D60, 16, 0),
    gsSP2Triangles(6, 10, 11, 0, 12, 13, 1, 0),
    gsSPLightColor(LIGHT_1, 0x00f700ff),
    gsSPLightColor(LIGHT_2, 0x007b00ff),
    gsSPVertex(yoshi_seg5_vertex_05020E60, 3, 0),
    gsSP1Triangle(0, 1, 2, 0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPLoadTextureBlock(yoshi_tank_top_texture, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0,
                         G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(yoshi_seg5_vertex_05020E90, 16, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 7, 8, 0, 5, 9, 3, 0),
    gsSP2Triangles(7, 6, 4, 0, 1, 9, 2, 0),
    gsSP2Triangles(10, 11, 12, 0, 8, 1, 6, 0),
    gsSP2Triangles(11, 13, 14, 0, 6, 5, 4, 0),
    gsSP2Triangles(12, 11, 14, 0, 13, 15, 14, 0),
    gsSP2Triangles(14, 15, 8, 0, 3, 10, 12, 0),
    gsSP2Triangles(12, 4, 3, 0, 12, 14, 7, 0),
    gsSP2Triangles(8, 7, 14, 0, 7, 4, 12, 0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPLightColor(LIGHT_1, 0xff0000ff),
    gsSPLightColor(LIGHT_2, 0x7f0000ff),
    gsSPVertex(yoshi_seg5_vertex_05020F90, 8, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 3, 5, 0, 7, 6, 5, 0),
    gsSPEndDisplayList(),
};

// 0x05022608 - 0x050227D8
const Gfx yoshi_seg5_dl_05022608[] = {
    gsSPLightColor(LIGHT_1, 0xf700ff),
    gsSPLightColor(LIGHT_2, 0x7b00ff),
    gsSPVertex(yoshi_seg5_vertex_05021010, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12,  4, 13, 0x0,  3,  5, 14, 0x0),
    gsSP2Triangles( 8,  1,  6, 0x0, 10,  0, 11, 0x0),
    gsSP2Triangles( 0,  2, 11, 0x0,  8, 15,  1, 0x0),
    gsSP2Triangles(14,  7,  3, 0x0, 13,  9, 12, 0x0),
    gsSP2Triangles(11, 12,  9, 0x0,  7, 14,  8, 0x0),
    gsSP2Triangles( 3, 13,  4, 0x0,  1, 15,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0x8b0000ff),
    gsSPLightColor(LIGHT_2, 0x450000ff),
    gsSPVertex(yoshi_seg5_vertex_05021110, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  2,  1, 0x0,  6,  7,  4, 0x0),
    gsSP2Triangles( 3,  8,  9, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  9,  2,  3, 0x0),
    gsSP2Triangles( 4,  1,  6, 0x0,  2,  9, 10, 0x0),
    gsSP2Triangles( 5, 11,  3, 0x0,  2, 10,  0, 0x0),
    gsSP2Triangles(11,  8,  3, 0x0,  1,  4,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(yoshi_seg5_vertex_050211D0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  7,  6, 0x0),
    gsSP2Triangles(10,  3,  5, 0x0,  5,  6, 11, 0x0),
    gsSP2Triangles( 5,  2, 12, 0x0,  2,  1, 12, 0x0),
    gsSP2Triangles( 2, 11, 13, 0x0,  9,  6,  5, 0x0),
    gsSP2Triangles(12, 10,  5, 0x0,  8, 13, 11, 0x0),
    gsSP2Triangles(11,  2,  5, 0x0,  6,  8, 11, 0x0),
    gsSP2Triangles( 9,  5,  4, 0x0,  3, 10,  1, 0x0),
    gsSP2Triangles(10, 12,  1, 0x0,  0,  2, 13, 0x0),
    gsSPEndDisplayList(),
};
