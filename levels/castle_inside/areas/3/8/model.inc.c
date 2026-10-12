// 0x07064DC8 - 0x07064DE0

// 0x07064DE0 - 0x07064EE0
static const Vtx inside_castle_seg7_vertex_07064DE0[] = {
#include "assets/rom/generated/2d6830b1645f87017c86.inc.c"















};

// 0x07064EE0 - 0x07064FD0
static const Vtx inside_castle_seg7_vertex_07064EE0[] = {
#include "assets/rom/generated/193e0f80f8ee61855c54.inc.c"














};

// 0x07064FD0 - 0x070650D0
static const Vtx inside_castle_seg7_vertex_07064FD0[] = {
#include "assets/rom/generated/460619638ef4c86415ac.inc.c"















};

// 0x070650D0 - 0x070651C0
static const Vtx inside_castle_seg7_vertex_070650D0[] = {
#include "assets/rom/generated/033de889e1e2964cc0ba.inc.c"














};

// 0x070651C0 - 0x070652B0
static const Vtx inside_castle_seg7_vertex_070651C0[] = {
#include "assets/rom/generated/f6dfa796c5c3d3dc33e0.inc.c"














};

// 0x070652B0 - 0x070653A0
static const Vtx inside_castle_seg7_vertex_070652B0[] = {
#include "assets/rom/generated/b298ba860e21ce42a5d9.inc.c"














};

// 0x070653A0 - 0x07065460
static const Vtx inside_castle_seg7_vertex_070653A0[] = {
#include "assets/rom/generated/6925d8389de696cbe589.inc.c"











};

// 0x07065460 - 0x07065560
static const Vtx inside_castle_seg7_vertex_07065460[] = {
#include "assets/rom/generated/b354a319c63824774522.inc.c"















};

// 0x07065560 - 0x07065620
static const Vtx inside_castle_seg7_vertex_07065560[] = {
#include "assets/rom/generated/e935bc319e5f683a2a28.inc.c"











};

// 0x07065620 - 0x07065710
static const Vtx inside_castle_seg7_vertex_07065620[] = {
#include "assets/rom/generated/0263ba38517a1807535e.inc.c"














};

// 0x07065710 - 0x07065810
static const Vtx inside_castle_seg7_vertex_07065710[] = {
#include "assets/rom/generated/2d3e278df0a5da576a9a.inc.c"















};

// 0x07065810 - 0x070658C0
static const Vtx inside_castle_seg7_vertex_07065810[] = {
#include "assets/rom/generated/214c147954372911531c.inc.c"










};

// 0x070658C0 - 0x070659C0
static const Vtx inside_castle_seg7_vertex_070658C0[] = {
#include "assets/rom/generated/48b48cf5943afedc9a3a.inc.c"















};

// 0x070659C0 - 0x07065AB0
static const Vtx inside_castle_seg7_vertex_070659C0[] = {
#include "assets/rom/generated/df1854dfc89e557524a9.inc.c"














};

// 0x07065AB0 - 0x07065BB0
static const Vtx inside_castle_seg7_vertex_07065AB0[] = {
#include "assets/rom/generated/361a005dedd5b94ff63a.inc.c"















};

// 0x07065BB0 - 0x07065C90
static const Vtx inside_castle_seg7_vertex_07065BB0[] = {
#include "assets/rom/generated/204b16fb4343e54dad77.inc.c"













};

// 0x07065C90 - 0x07065D80
static const Vtx inside_castle_seg7_vertex_07065C90[] = {
#include "assets/rom/generated/3863afa1c8c955cb7a96.inc.c"














};

// 0x07065D80 - 0x07065E70
static const Vtx inside_castle_seg7_vertex_07065D80[] = {
#include "assets/rom/generated/61346af04f607bc6cfbc.inc.c"














};

// 0x07065E70 - 0x07065F50
static const Vtx inside_castle_seg7_vertex_07065E70[] = {
#include "assets/rom/generated/987ee6ffa646e7a5ffa7.inc.c"













};

// 0x07065F50 - 0x07066030
static const Vtx inside_castle_seg7_vertex_07065F50[] = {
#include "assets/rom/generated/e82f735b608d3f8d7991.inc.c"













};

// 0x07066030 - 0x07066110
static const Vtx inside_castle_seg7_vertex_07066030[] = {
#include "assets/rom/generated/744d543a2a0ffd34dac7.inc.c"













};

// 0x07066110 - 0x070661F0
static const Vtx inside_castle_seg7_vertex_07066110[] = {
#include "assets/rom/generated/8889ad101efbe0fa3a22.inc.c"













};

// 0x070661F0 - 0x07066260
static const Vtx inside_castle_seg7_vertex_070661F0[] = {
#include "assets/rom/generated/7d8f1c1e6e0e9b80ceb1.inc.c"






};

// 0x07066260 - 0x07066360
static const Vtx inside_castle_seg7_vertex_07066260[] = {
#include "assets/rom/generated/24950d4a64895629251e.inc.c"















};

// 0x07066360 - 0x07066460
static const Vtx inside_castle_seg7_vertex_07066360[] = {
#include "assets/rom/generated/b438bdba943d30d69f8b.inc.c"















};

// 0x07066460 - 0x07066560
static const Vtx inside_castle_seg7_vertex_07066460[] = {
#include "assets/rom/generated/f170abb640bd9fe9afe9.inc.c"















};

// 0x07066560 - 0x07066768
static const Gfx inside_castle_seg7_dl_07066560[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_0900B000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x5f5f5fff),
    gsSPVertex(inside_castle_seg7_vertex_07064DE0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  5,  4, 0x0),
    gsSP2Triangles( 7,  8,  5, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07064EE0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  7, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07064FD0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  5, 15,  1, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070650D0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  0,  2,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070651C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070652B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  4,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 12, 0x0),
    gsSP2Triangles( 9, 13, 11, 0x0,  9, 12, 10, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070653A0, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x07066768 - 0x07066838
static const Gfx inside_castle_seg7_dl_07066768[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09004000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07065460, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0, 15,  1, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07065560, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 5,  6,  1, 0x0,  6,  7,  1, 0x0),
    gsSP2Triangles( 8,  4,  3, 0x0,  8,  3,  9, 0x0),
    gsSP2Triangles( 6,  8,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 8, 11,  4, 0x0,  6, 11,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x07066838 - 0x07066910
static const Gfx inside_castle_seg7_dl_07066838[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09003000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07065620, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07065710, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  1, 15,  2, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07065810, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  7,  8,  9, 0x0),
    gsSP1Triangle( 7,  9, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x07066910 - 0x07066BE8
static const Gfx inside_castle_seg7_dl_07066910[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09003800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_070658C0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 12, 10, 0x0,  9, 13, 12, 0x0),
    gsSP2Triangles( 0,  2, 14, 0x0,  0, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070659C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(12, 14, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07065AB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  5,  0,  4, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  6,  8, 0x0),
    gsSP2Triangles( 8, 10, 11, 0x0,  9,  8, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 15,  9, 11, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07065BB0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 1,  4,  3, 0x0,  0,  2,  5, 0x0),
    gsSP2Triangles( 6,  7,  0, 0x0,  6,  0,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07065C90, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07065D80, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  3,  9,  4, 0x0),
    gsSP2Triangles( 6, 10,  7, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07065E70, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07065F50, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07066030, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07066110, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 1,  4,  3, 0x0,  0,  2,  5, 0x0),
    gsSP2Triangles( 6,  0,  5, 0x0,  6,  7,  0, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070661F0, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  6,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07066BE8 - 0x07066CE0
static const Gfx inside_castle_seg7_dl_07066BE8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(inside_castle_seg7_vertex_07066260, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07066360, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  8, 15,  9, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07066460, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0,  4, 11,  5, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x07066CE0 - 0x07066D88
const Gfx inside_castle_seg7_dl_07066CE0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07066560),
    gsSPDisplayList(inside_castle_seg7_dl_07066768),
    gsSPDisplayList(inside_castle_seg7_dl_07066838),
    gsSPDisplayList(inside_castle_seg7_dl_07066910),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07066BE8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
