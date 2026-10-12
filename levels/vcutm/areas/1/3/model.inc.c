// 0x07008140 - 0x07008220
static const Vtx vcutm_seg7_vertex_07008140[] = {
#include "assets/rom/generated/9710973f16207f8890e0.inc.c"













};

// 0x07008220 - 0x07008300
static const Vtx vcutm_seg7_vertex_07008220[] = {
#include "assets/rom/generated/8fae6c054b93b68f2bd0.inc.c"













};

// 0x07008300 - 0x070083F0
static const Vtx vcutm_seg7_vertex_07008300[] = {
#include "assets/rom/generated/773db0897359c3c535b1.inc.c"














};

// 0x070083F0 - 0x070084E0
static const Vtx vcutm_seg7_vertex_070083F0[] = {
#include "assets/rom/generated/d567beaed6310e3c719d.inc.c"














};

// 0x070084E0 - 0x070085D0
static const Vtx vcutm_seg7_vertex_070084E0[] = {
#include "assets/rom/generated/24a0df9a58a9ee1d9b3c.inc.c"














};

// 0x070085D0 - 0x070086D0
static const Vtx vcutm_seg7_vertex_070085D0[] = {
#include "assets/rom/generated/f88ed398ec6bfe21454d.inc.c"















};

// 0x070086D0 - 0x070087D0
static const Vtx vcutm_seg7_vertex_070086D0[] = {
#include "assets/rom/generated/c0ae87db93224dfa524c.inc.c"















};

// 0x070087D0 - 0x070088D0
static const Vtx vcutm_seg7_vertex_070087D0[] = {
#include "assets/rom/generated/fec0ff8e0d74fda902ff.inc.c"















};

// 0x070088D0 - 0x070089D0
static const Vtx vcutm_seg7_vertex_070088D0[] = {
#include "assets/rom/generated/8140db044d128e251804.inc.c"















};

// 0x070089D0 - 0x07008AC0
static const Vtx vcutm_seg7_vertex_070089D0[] = {
#include "assets/rom/generated/f29b90bd95d0f615e158.inc.c"














};

// 0x07008AC0 - 0x07008B20
static const Vtx vcutm_seg7_vertex_07008AC0[] = {
#include "assets/rom/generated/901c6d9f8d70a4b5b6e5.inc.c"





};

// 0x07008B20 - 0x07008E10
static const Gfx vcutm_seg7_dl_07008B20[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, vcutm_seg7_texture_07000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(vcutm_seg7_vertex_07008140, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  1,  5,  4, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(vcutm_seg7_vertex_07008220, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  5, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9, 11, 12, 0x0),
    gsSP1Triangle( 0, 13,  1, 0x0),
    gsSPVertex(vcutm_seg7_vertex_07008300, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(vcutm_seg7_vertex_070083F0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  6,  8, 0x0),
    gsSP2Triangles( 8,  7, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 9,  8, 12, 0x0,  0, 13, 14, 0x0),
    gsSP1Triangle( 0,  2, 13, 0x0),
    gsSPVertex(vcutm_seg7_vertex_070084E0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  3,  8, 0x0),
    gsSP2Triangles( 7,  6,  3, 0x0,  9,  0, 10, 0x0),
    gsSP2Triangles( 9,  1,  0, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(vcutm_seg7_vertex_070085D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  2,  5, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSP1Triangle(13,  6, 14, 0x0),
    gsSPVertex(vcutm_seg7_vertex_070086D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(vcutm_seg7_vertex_070087D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  8,  3, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(vcutm_seg7_vertex_070088D0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  0, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 6,  9,  7, 0x0, 10, 11, 12, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(vcutm_seg7_vertex_070089D0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(vcutm_seg7_vertex_07008AC0, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008E10 - 0x07008E80
const Gfx vcutm_seg7_dl_07008E10[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(vcutm_seg7_dl_07008B20),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
