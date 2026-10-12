// 0x07009700 - 0x070097E0
static const Vtx bits_seg7_vertex_07009700[] = {
#include "assets/rom/generated/de54dbf8915020ffa7e4.inc.c"













};

// 0x070097E0 - 0x070098E0
static const Vtx bits_seg7_vertex_070097E0[] = {
#include "assets/rom/generated/f7baafbe8c3701f89737.inc.c"















};

// 0x070098E0 - 0x070099C0
static const Vtx bits_seg7_vertex_070098E0[] = {
#include "assets/rom/generated/a0727e9ceee01cf29ab0.inc.c"













};

// 0x070099C0 - 0x07009AA0
static const Vtx bits_seg7_vertex_070099C0[] = {
#include "assets/rom/generated/5dd851f90d1502896ca3.inc.c"













};

// 0x07009AA0 - 0x07009AE0
static const Vtx bits_seg7_vertex_07009AA0[] = {
#include "assets/rom/generated/92a2d2fe1374f3af27b5.inc.c"



};

// 0x07009AE0 - 0x07009BD0
static const Vtx bits_seg7_vertex_07009AE0[] = {
#include "assets/rom/generated/5be6231faae476ecda57.inc.c"














};

// 0x07009BD0 - 0x07009CD0
static const Vtx bits_seg7_vertex_07009BD0[] = {
#include "assets/rom/generated/aa78e1f53e1349c4c745.inc.c"















};

// 0x07009CD0 - 0x07009DC0
static const Vtx bits_seg7_vertex_07009CD0[] = {
#include "assets/rom/generated/de1d2f807c4a56d09e6b.inc.c"














};

// 0x07009DC0 - 0x07009EC0
static const Vtx bits_seg7_vertex_07009DC0[] = {
#include "assets/rom/generated/77c903cea42bf87591bb.inc.c"















};

// 0x07009EC0 - 0x07009FB0
static const Vtx bits_seg7_vertex_07009EC0[] = {
#include "assets/rom/generated/850626f75ae9a71238de.inc.c"














};

// 0x07009FB0 - 0x0700A090
static const Vtx bits_seg7_vertex_07009FB0[] = {
#include "assets/rom/generated/4c570e0d5dfac6918a71.inc.c"













};

// 0x0700A090 - 0x0700A170
static const Vtx bits_seg7_vertex_0700A090[] = {
#include "assets/rom/generated/d47f4bbe634c56211aea.inc.c"













};

// 0x0700A170 - 0x0700A250
static const Vtx bits_seg7_vertex_0700A170[] = {
#include "assets/rom/generated/0abb5a6a3b5c79abeb93.inc.c"













};

// 0x0700A250 - 0x0700A2C0
static const Vtx bits_seg7_vertex_0700A250[] = {
#include "assets/rom/generated/d43fe90c381bc69e660b.inc.c"






};

// 0x0700A2C0 - 0x0700A3A0
static const Vtx bits_seg7_vertex_0700A2C0[] = {
#include "assets/rom/generated/45f2f54b320e17050201.inc.c"













};

// 0x0700A3A0 - 0x0700A4A0
static const Vtx bits_seg7_vertex_0700A3A0[] = {
#include "assets/rom/generated/0d0fcf94255e11efbdcb.inc.c"















};

// 0x0700A4A0 - 0x0700A580
static const Vtx bits_seg7_vertex_0700A4A0[] = {
#include "assets/rom/generated/90622db8682c33230dc9.inc.c"













};

// 0x0700A580 - 0x0700A680
static const Vtx bits_seg7_vertex_0700A580[] = {
#include "assets/rom/generated/b4a84173afa74deb50c2.inc.c"















};

// 0x0700A680 - 0x0700A780
static const Vtx bits_seg7_vertex_0700A680[] = {
#include "assets/rom/generated/ba2524886fce0a36ed90.inc.c"















};

// 0x0700A780 - 0x0700A880
static const Vtx bits_seg7_vertex_0700A780[] = {
#include "assets/rom/generated/4c477e7aa3657697865b.inc.c"















};

// 0x0700A880 - 0x0700A970
static const Vtx bits_seg7_vertex_0700A880[] = {
#include "assets/rom/generated/866cf4c1fcd3e837b02c.inc.c"














};

// 0x0700A970 - 0x0700AA70
static const Vtx bits_seg7_vertex_0700A970[] = {
#include "assets/rom/generated/aa469545162ef642ab83.inc.c"















};

// 0x0700AA70 - 0x0700AB70
static const Vtx bits_seg7_vertex_0700AA70[] = {
#include "assets/rom/generated/e83aeda8c05a940633d5.inc.c"















};

// 0x0700AB70 - 0x0700AC20
static const Vtx bits_seg7_vertex_0700AB70[] = {
#include "assets/rom/generated/8aabb0bf9359a2d8555a.inc.c"










};

// 0x0700AC20 - 0x0700AD20
static const Vtx bits_seg7_vertex_0700AC20[] = {
#include "assets/rom/generated/7c6c3d47dc335370a79b.inc.c"















};

// 0x0700AD20 - 0x0700AD60
static const Vtx bits_seg7_vertex_0700AD20[] = {
#include "assets/rom/generated/2b8a09fd74770cdcb91c.inc.c"



};

// 0x0700AD60 - 0x0700AE98
static const Gfx bits_seg7_dl_0700AD60[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_07009700, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  4,  6, 0x0,  8,  6,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(bits_seg7_vertex_070097E0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 7,  8,  5, 0x0,  7,  9,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_070098E0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  7,  9, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_070099C0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 10, 12, 0x0),
    gsSPVertex(bits_seg7_vertex_07009AA0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700AE98 - 0x0700B0E0
static const Gfx bits_seg7_dl_0700AE98[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_07009AE0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bits_seg7_vertex_07009BD0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 0,  8,  9, 0x0,  0,  9,  6, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  4, 13,  2, 0x0),
    gsSP1Triangle(10, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_07009CD0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  0,  7,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle( 3, 12, 14, 0x0),
    gsSPVertex(bits_seg7_vertex_07009DC0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 4,  8,  9, 0x0,  0, 10, 11, 0x0),
    gsSP2Triangles( 4,  9,  2, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle(12, 15, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_07009EC0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14, 11, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_07009FB0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_0700A090, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_0700A170, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_0700A250, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP1Triangle( 4,  5,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700B0E0 - 0x0700B420
static const Gfx bits_seg7_dl_0700B0E0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_0700A2C0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSP2Triangles( 6,  5,  8, 0x0,  5,  9,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(bits_seg7_vertex_0700A3A0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  1,  0, 0x0,  4,  5,  1, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10,  6, 11, 0x0, 10,  7,  6, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700A4A0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(11, 10, 12, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_0700A580, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  3,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_0700A680, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 3, 11, 10, 0x0,  3,  2, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700A780, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  3, 0x0,  4,  3,  2, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  6,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP2Triangles(14, 15, 10, 0x0, 15, 11, 10, 0x0),
    gsSPVertex(bits_seg7_vertex_0700A880, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0,  1,  0, 11, 0x0),
    gsSP2Triangles( 1, 11, 12, 0x0, 13,  6,  5, 0x0),
    gsSP1Triangle(13, 14,  6, 0x0),
    gsSPVertex(bits_seg7_vertex_0700A970, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles( 0, 10,  1, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0, 11, 15, 12, 0x0),
    gsSPVertex(bits_seg7_vertex_0700AA70, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  9,  6,  4, 0x0),
    gsSP2Triangles(10,  9, 11, 0x0,  9,  4, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700AB70, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  6,  5, 0x0),
    gsSP2Triangles( 7,  8,  6, 0x0,  7,  9,  8, 0x0),
    gsSP1Triangle( 7, 10,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700B420 - 0x0700B4A0
static const Gfx bits_seg7_dl_0700B420[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09008000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_0700AC20, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_0700AD20, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700B4A0 - 0x0700B528
const Gfx bits_seg7_dl_0700B4A0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bits_seg7_dl_0700AD60),
    gsSPDisplayList(bits_seg7_dl_0700AE98),
    gsSPDisplayList(bits_seg7_dl_0700B0E0),
    gsSPDisplayList(bits_seg7_dl_0700B420),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
