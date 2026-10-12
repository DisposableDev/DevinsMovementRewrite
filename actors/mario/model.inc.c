// Shared Mario meshes, triangle lists, and palette-independent materials.

// 0x04000090
ALIGNED8 static const Texture mario_texture_metal[] = {
#include "actors/mario/mario_metal.rgba16.inc.c"
};

// 0x04001090
ALIGNED8 static const Texture mario_texture_yellow_button[] = {
#include "actors/mario/mario_overalls_button.rgba16.inc.c"
};

// 0x04008090
ALIGNED8 static const Texture mario_texture_wings_half_1[] = {
#include "actors/mario/mario_wing.rgba16.inc.c"
};

// 0x04009090
ALIGNED8 static const Texture mario_texture_wings_half_2[] = {
#include "actors/mario/mario_wing_tip.rgba16.inc.c"
};

// 0x0400A090
ALIGNED8 static const Texture mario_texture_metal_wings_half_1[] = {
#include "actors/mario/mario_metal_wing_unused.rgba16.inc.c"
};

// 0x0400B090
ALIGNED8 static const Texture mario_texture_metal_wings_half_2[] = {
#include "actors/mario/mario_metal_wing_tip_unused.rgba16.inc.c"
};

// 0x0400C090
static const Vtx mario_butt_dl_vertex_group1[] = {
#include "assets/rom/generated/10f764a8e01941b93a4d.inc.c"














};

// 0x0400C180
static const Vtx mario_butt_dl_vertex_group2[] = {
#include "assets/rom/generated/e5309ced9c7a4553497a.inc.c"













};

// 0x0400C260
static const Vtx mario_butt_dl_vertex_group3[] = {
#include "assets/rom/generated/64f4c4f631f329e8ae9b.inc.c"















};

// 0x0400C360
static const Vtx mario_butt_dl_vertex_group4[] = {
#include "assets/rom/generated/5be0131137614c6a450b.inc.c"














};

// 0x0400C450
static const Vtx mario_butt_dl_vertex_group5[] = {
#include "assets/rom/generated/ac47e05d075f50ab3038.inc.c"














};

// 0x0400C540
static const Vtx mario_butt_dl_vertex_group6[] = {
#include "assets/rom/generated/b3691cffdc7d0bd3b222.inc.c"














};

// 0x0400C630
static const Vtx mario_butt_dl_vertex_group7[] = {
#include "assets/rom/generated/015b2bf047f35cc88588.inc.c"














};

// 0x0400C720
static const Vtx mario_butt_dl_vertex_group8[] = {
#include "assets/rom/generated/ebe5308cfe731a51a5e2.inc.c"















};

// 0x0400C820
static const Vtx mario_butt_dl_vertex_group9[] = {
#include "assets/rom/generated/674a9957b6305df13daf.inc.c"















};

// 0x0400C920
static const Vtx mario_butt_dl_vertex_group10[] = {
#include "assets/rom/generated/38c92ef8bc05e5c0a37a.inc.c"













};

// 0x0400CA00 - 0x0400CC98
const Gfx mario_butt_dl[] = {
    gsSPVertex(mario_butt_dl_vertex_group1, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  5, 10, 0x0),
    gsSP2Triangles( 4, 11, 10, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(mario_butt_dl_vertex_group2, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12,  3, 13, 0x0,  6,  8, 12, 0x0),
    gsSPVertex(mario_butt_dl_vertex_group3, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  1,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles(11,  9, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(mario_butt_dl_vertex_group4, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  8,  2, 0x0),
    gsSP2Triangles( 9,  7, 10, 0x0, 11, 12,  7, 0x0),
    gsSP1Triangle(13, 11, 14, 0x0),
    gsSPVertex(mario_butt_dl_vertex_group5, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  2,  7,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(mario_butt_dl_vertex_group6, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10,  7, 0x0),
    gsSP2Triangles(11, 10,  9, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle( 0, 13, 12, 0x0),
    gsSPVertex(mario_butt_dl_vertex_group7, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  2,  3, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9,  8, 11, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 13, 14,  1, 0x0),
    gsSPVertex(mario_butt_dl_vertex_group8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10,  5, 0x0),
    gsSP2Triangles( 4, 11, 12, 0x0,  6, 11, 13, 0x0),
    gsSP2Triangles(13,  3, 14, 0x0, 15,  2,  1, 0x0),
    gsSPVertex(mario_butt_dl_vertex_group9, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 1,  0,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  0, 13, 14, 0x0),
    gsSP1Triangle(15,  0, 14, 0x0),
    gsSPVertex(mario_butt_dl_vertex_group10, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 3,  4,  0, 0x0,  0,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  7,  0, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles( 9,  1, 11, 0x0, 11, 12, 10, 0x0),
    gsSP1Triangle(10, 13,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400CCC8 - 0x0400CD40
const Gfx mario_metal_butt[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_DECALFADE, G_CC_DECALFADE),
    gsDPLoadTextureBlock(mario_texture_metal, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0x07C0, 0x0F80, 0, G_TX_RENDERTILE, G_ON),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPDisplayList(mario_butt_dl),
    gsSPEndDisplayList(),
};

// 0x0400CD40
static const Vtx mario_left_arm_shared_dl_vertex_group1[] = {
#include "assets/rom/generated/dc24d9476ea03e48871e.inc.c"














};

// 0x0400CE30
static const Vtx mario_left_arm_shared_dl_vertex_group2[] = {
#include "assets/rom/generated/92a2be1b3f6a6b72abf9.inc.c"














};

// 0x0400CF20
static const Vtx mario_left_arm_shared_dl_vertex_group3[] = {
#include "assets/rom/generated/02357a9c3c461931213f.inc.c"













};

// 0x0400D000
static const Vtx mario_left_arm_shared_dl_vertex_group4[] = {
#include "assets/rom/generated/c807b626a50269716066.inc.c"








};

// 0x0400D090 - 0x0400D1D8
const Gfx mario_left_arm_shared_dl[] = {
    gsSPVertex(mario_left_arm_shared_dl_vertex_group1, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 5,  6,  4, 0x0,  7,  8,  6, 0x0),
    gsSP2Triangles( 9,  2,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 7, 12, 10, 0x0,  5, 13, 12, 0x0),
    gsSP2Triangles( 3, 14, 13, 0x0,  0, 11, 14, 0x0),
    gsSPVertex(mario_left_arm_shared_dl_vertex_group2, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 5,  4,  3, 0x0,  6,  7,  5, 0x0),
    gsSP2Triangles( 1,  7,  6, 0x0,  2,  1,  6, 0x0),
    gsSP2Triangles( 6,  5,  8, 0x0,  8,  5,  3, 0x0),
    gsSP2Triangles( 3,  0,  9, 0x0,  9,  0,  2, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(mario_left_arm_shared_dl_vertex_group3, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  2,  4,  7, 0x0),
    gsSP2Triangles( 0,  2,  8, 0x0,  9,  0, 10, 0x0),
    gsSP2Triangles( 6,  9, 11, 0x0,  4,  6, 12, 0x0),
    gsSP1Triangle(13,  5,  3, 0x0),
    gsSPVertex(mario_left_arm_shared_dl_vertex_group4, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 3,  4,  1, 0x0,  5,  6,  7, 0x0),
    gsSP1Triangle( 6,  8,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400D1F8
static const Vtx mario_left_forearm_shared_dl_vertex[] = {
#include "assets/rom/generated/e7f1bdc421dbb75d40a2.inc.c"















};

// 0x0400D2F8 - 0x0400D3E8
const Gfx mario_left_forearm_shared_dl[] = {
    gsSPVertex(mario_left_forearm_shared_dl_vertex, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 5,  6,  4, 0x0,  7,  8,  6, 0x0),
    gsSP2Triangles( 9,  2,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 7, 12, 10, 0x0,  5, 13, 12, 0x0),
    gsSP2Triangles( 3, 14, 13, 0x0,  0, 11, 14, 0x0),
    gsSP2Triangles( 8, 15,  6, 0x0,  6, 15,  4, 0x0),
    gsSP2Triangles( 4, 15,  1, 0x0,  1, 15,  2, 0x0),
    gsSP2Triangles( 2, 15,  8, 0x0,  0, 14,  3, 0x0),
    gsSP2Triangles( 3, 13,  5, 0x0,  5, 12,  7, 0x0),
    gsSP2Triangles( 7, 10,  9, 0x0,  9, 11,  0, 0x0),
    gsSP2Triangles( 7,  9,  8, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 3,  5,  4, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 9,  0,  2, 0x0, 12, 13, 10, 0x0),
    gsSP2Triangles(13, 11, 10, 0x0, 13, 14, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400D3E8
static const Vtx mario_left_hand_closed_shared_dl_vertex_group1[] = {
#include "assets/rom/generated/82e3bbdce1d0073003a2.inc.c"















};

// 0x0400D4E8
static const Vtx mario_left_hand_closed_shared_dl_vertex_group2[] = {
#include "assets/rom/generated/07f65eeaf0b99dff2055.inc.c"













};

// 0x0400D5C8
static const Vtx mario_left_hand_closed_shared_dl_vertex_group3[] = {
#include "assets/rom/generated/fd94b5e209b71bb81c42.inc.c"














};

// 0x0400D6B8
static const Vtx mario_left_hand_closed_shared_dl_vertex_group4[] = {
#include "assets/rom/generated/18e3aad72e0ed0de9597.inc.c"









};

// 0x0400D758 - 0x0400D8F0
const Gfx mario_left_hand_closed_shared_dl[] = {
    gsSPVertex(mario_left_hand_closed_shared_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  1,  0, 0x0,  2,  1,  7, 0x0),
    gsSP2Triangles( 7,  1,  6, 0x0,  6,  4,  8, 0x0),
    gsSP2Triangles( 8,  4,  3, 0x0,  9,  3,  2, 0x0),
    gsSP2Triangles( 0, 10, 11, 0x0, 12, 10,  0, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(mario_left_hand_closed_shared_dl_vertex_group2, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  1,  3, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9, 10,  3, 0x0,  1,  0,  9, 0x0),
    gsSP2Triangles( 9,  3,  1, 0x0,  5,  2,  1, 0x0),
    gsSP2Triangles( 0,  2,  7, 0x0,  7,  2,  8, 0x0),
    gsSP2Triangles( 8,  2, 11, 0x0, 11,  2,  5, 0x0),
    gsSP2Triangles( 9,  7,  6, 0x0,  6,  8, 12, 0x0),
    gsSP2Triangles(12,  8, 13, 0x0, 13,  5,  4, 0x0),
    gsSPVertex(mario_left_hand_closed_shared_dl_vertex_group3, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 3,  1,  4, 0x0,  4,  1,  5, 0x0),
    gsSP2Triangles( 5,  1,  0, 0x0,  6,  7,  2, 0x0),
    gsSP2Triangles( 8,  2,  3, 0x0,  3,  9, 10, 0x0),
    gsSP2Triangles(10,  9, 11, 0x0, 11,  7,  6, 0x0),
    gsSP2Triangles(10, 12, 13, 0x0,  6, 14, 12, 0x0),
    gsSPVertex(mario_left_hand_closed_shared_dl_vertex_group4, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 1,  5,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  9,  6, 0x0),
    gsSP1Triangle( 7,  1,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400D910
static const Vtx mario_right_arm_shared_dl_vertex_group1[] = {
#include "assets/rom/generated/34bd62de7199edc612bc.inc.c"















};

// 0x0400DA10
static const Vtx mario_right_arm_shared_dl_vertex_group2[] = {
#include "assets/rom/generated/a3771b65282925e4d2cc.inc.c"















};

// 0x0400DB10
static const Vtx mario_right_arm_shared_dl_vertex_group3[] = {
#include "assets/rom/generated/facb432cae02e94164ba.inc.c"















};

// 0x0400DC10
static const Vtx mario_right_arm_shared_dl_vertex_group4[] = {
#include "assets/rom/generated/edeea85e87de05da228d.inc.c"








};

// 0x0400DCA0 - 0x0400DDE8
const Gfx mario_right_arm_shared_dl[] = {
    gsSPVertex(mario_right_arm_shared_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 5,  6,  4, 0x0,  7,  8,  6, 0x0),
    gsSP2Triangles( 9,  2,  8, 0x0,  1, 10,  2, 0x0),
    gsSP2Triangles( 2, 11,  8, 0x0,  8, 12,  6, 0x0),
    gsSP2Triangles( 6, 13,  4, 0x0,  4, 14,  1, 0x0),
    gsSP2Triangles( 9, 15,  0, 0x0,  0, 15,  3, 0x0),
    gsSPVertex(mario_right_arm_shared_dl_vertex_group2, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  3,  6,  4, 0x0),
    gsSP2Triangles( 1,  6,  3, 0x0,  0,  7,  1, 0x0),
    gsSP2Triangles( 8,  7,  0, 0x0,  5,  4,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 12,  9, 13, 0x0),
    gsSP1Triangle(14, 12, 15, 0x0),
    gsSPVertex(mario_right_arm_shared_dl_vertex_group3, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 5,  6,  4, 0x0,  7,  5,  2, 0x0),
    gsSP2Triangles( 8,  7,  9, 0x0, 10,  8, 11, 0x0),
    gsSP2Triangles( 6, 10, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(mario_right_arm_shared_dl_vertex_group4, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 2,  4,  3, 0x0,  5,  6,  7, 0x0),
    gsSP1Triangle( 5,  8,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400DE08
static const Vtx mario_right_forearm_shared_dl_vertex[] = {
#include "assets/rom/generated/84fd2bab38cc8d40c3ba.inc.c"















};

// 0x0400DF08 - 0x0400DFF8
const Gfx mario_right_forearm_shared_dl[] = {
    gsSPVertex(mario_right_forearm_shared_dl_vertex, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 5,  6,  4, 0x0,  7,  8,  6, 0x0),
    gsSP2Triangles( 9,  2,  8, 0x0,  1, 10,  2, 0x0),
    gsSP2Triangles( 2, 11,  8, 0x0,  8, 12,  6, 0x0),
    gsSP2Triangles( 6, 13,  4, 0x0,  4, 14,  1, 0x0),
    gsSP2Triangles( 9, 15,  0, 0x0,  0, 15,  3, 0x0),
    gsSP2Triangles( 3, 15,  5, 0x0,  5, 15,  7, 0x0),
    gsSP2Triangles( 7, 15,  9, 0x0, 14, 10,  1, 0x0),
    gsSP2Triangles(13, 14,  4, 0x0, 12, 13,  6, 0x0),
    gsSP2Triangles(11, 12,  8, 0x0, 10, 11,  2, 0x0),
    gsSP2Triangles( 9,  0,  2, 0x0,  7,  9,  8, 0x0),
    gsSP2Triangles( 5,  7,  6, 0x0,  3,  5,  4, 0x0),
    gsSP2Triangles( 0,  3,  1, 0x0, 11, 13, 12, 0x0),
    gsSP2Triangles(11, 10, 13, 0x0, 10, 14, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400DFF8
static const Vtx mario_right_hand_closed_dl_vertex_group1[] = {
#include "assets/rom/generated/e25b3b44f378808e1cb6.inc.c"















};

// 0x0400E0F8
static const Vtx mario_right_hand_closed_dl_vertex_group2[] = {
#include "assets/rom/generated/22d9acb4797867a10961.inc.c"













};

// 0x0400E1D8
static const Vtx mario_right_hand_closed_dl_vertex_group3[] = {
#include "assets/rom/generated/dbd74a5efd01b44a09f2.inc.c"














};

// 0x0400E2C8 - 0x0400E458
const Gfx mario_right_hand_closed_dl[] = {
    gsSPVertex(mario_right_hand_closed_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 2,  5,  6, 0x0,  6,  7,  4, 0x0),
    gsSP2Triangles( 4,  7,  8, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 9,  5,  2, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(13, 12, 14, 0x0, 14, 12,  8, 0x0),
    gsSP2Triangles( 8, 12,  9, 0x0,  9, 12, 11, 0x0),
    gsSP1Triangle( 3, 15,  0, 0x0),
    gsSPVertex(mario_right_hand_closed_dl_vertex_group2, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 8,  7,  1, 0x0,  1,  7,  4, 0x0),
    gsSP2Triangles( 4,  7,  9, 0x0, 10,  7,  6, 0x0),
    gsSP2Triangles( 9, 10,  5, 0x0, 10, 11,  5, 0x0),
    gsSP2Triangles(11, 12,  5, 0x0,  1,  4,  3, 0x0),
    gsSP2Triangles(11, 10,  6, 0x0,  6, 13, 11, 0x0),
    gsSP2Triangles( 7, 10,  9, 0x0,  4,  9,  5, 0x0),
    gsSP2Triangles( 0,  8,  1, 0x0,  6,  8,  0, 0x0),
    gsSPVertex(mario_right_hand_closed_dl_vertex_group3, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 4,  3,  5, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 7,  6,  0, 0x0,  8,  6,  5, 0x0),
    gsSP2Triangles( 9,  3,  1, 0x0,  0,  6,  8, 0x0),
    gsSP2Triangles(10, 11,  1, 0x0, 11,  9,  1, 0x0),
    gsSP2Triangles(11, 12,  9, 0x0,  1,  0, 10, 0x0),
    gsSP2Triangles(13,  5,  3, 0x0,  3,  9, 14, 0x0),
    gsSP1Triangle(14, 13,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400E478 - 0x0400E4A8
const Gfx mario_metal_right_hand_closed[] = {
    gsSPDisplayList(mario_right_hand_closed_dl),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPTexture(0x0F80, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsSPEndDisplayList(),
};

// 0x0400E4A8
static const Vtx mario_left_thigh_dl_vertex_group1[] = {
#include "assets/rom/generated/e1aa1b3348ceb84f800e.inc.c"















};

// 0x0400E5A8
static const Vtx mario_left_thigh_dl_vertex_group2[] = {
#include "assets/rom/generated/769f9b166fdf0f58003a.inc.c"















};

// 0x0400E6A8 - 0x0400E7B0
const Gfx mario_left_thigh_dl[] = {
    gsSPVertex(mario_left_thigh_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 3,  1,  4, 0x0,  4,  1,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 9,  7, 10, 0x0, 10,  7, 11, 0x0),
    gsSP2Triangles(11,  7,  6, 0x0, 12,  3, 13, 0x0),
    gsSP2Triangles(13,  4, 14, 0x0, 14,  4, 15, 0x0),
    gsSPVertex(mario_left_thigh_dl_vertex_group2, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  0,  2,  6, 0x0),
    gsSP2Triangles( 7,  0,  8, 0x0,  9,  7, 10, 0x0),
    gsSP2Triangles( 3,  9, 10, 0x0,  2,  3,  6, 0x0),
    gsSP2Triangles( 3, 11,  6, 0x0, 10, 11,  3, 0x0),
    gsSP2Triangles( 7,  8, 10, 0x0,  0, 12,  8, 0x0),
    gsSP2Triangles( 6, 12,  0, 0x0,  1, 13,  3, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0, 14,  4,  0, 0x0),
    gsSP2Triangles( 9, 15, 14, 0x0,  3, 13, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400E7E0 - 0x0400E858
const Gfx mario_metal_left_thigh[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_DECALFADE, G_CC_DECALFADE),
    gsDPLoadTextureBlock(mario_texture_metal, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0x07C0, 0x0F80, 0, G_TX_RENDERTILE, G_ON),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPDisplayList(mario_left_thigh_dl),
    gsSPEndDisplayList(),
};

// 0x0400E858
static const Vtx mario_left_leg_shared_dl_vertex[] = {
#include "assets/rom/generated/abb80efa3cee246ee4dc.inc.c"











};

// 0x0400E918 - 0x0400E9C8
const Gfx mario_left_leg_shared_dl[] = {
    gsSPVertex(mario_left_leg_shared_dl_vertex, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 0,  7,  1, 0x0,  0,  8,  7, 0x0),
    gsSP2Triangles( 7,  8,  6, 0x0,  5,  6,  3, 0x0),
    gsSP2Triangles( 3,  9,  2, 0x0,  2,  9,  0, 0x0),
    gsSP2Triangles( 3, 10,  9, 0x0,  5, 11,  7, 0x0),
    gsSP2Triangles( 7, 11,  1, 0x0,  1, 11,  2, 0x0),
    gsSP2Triangles( 2, 11,  4, 0x0,  4, 11,  5, 0x0),
    gsSP2Triangles( 9, 10,  0, 0x0,  0, 10,  8, 0x0),
    gsSP2Triangles( 8, 10,  6, 0x0,  6, 10,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400E9C8
static const Vtx mario_left_foot_shared_dl_vertex_group1[] = {
#include "assets/rom/generated/8492c55e02d83b709ec4.inc.c"















};

// 0x0400EAC8
static const Vtx mario_left_foot_shared_dl_vertex_group2[] = {
#include "assets/rom/generated/55c06bb38401cd82316c.inc.c"














};

// 0x0400EBB8 - 0x0400ECA0
const Gfx mario_left_foot_shared_dl[] = {
    gsSPVertex(mario_left_foot_shared_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  5,  3,  4, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(14,  8, 10, 0x0, 11, 13, 15, 0x0),
    gsSPVertex(mario_left_foot_shared_dl_vertex_group2, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 2,  3,  0, 0x0,  4,  5,  6, 0x0),
    gsSP2Triangles( 6,  3,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP2Triangles(12,  9, 10, 0x0,  9, 12,  7, 0x0),
    gsSP2Triangles(14,  0,  3, 0x0, 11,  0, 14, 0x0),
    gsSP2Triangles( 3,  6, 14, 0x0, 14,  6,  7, 0x0),
    gsSP2Triangles(14,  7, 12, 0x0, 12, 11, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400ECC0
static const Vtx mario_right_thigh_shared_dl_vertex_group1[] = {
#include "assets/rom/generated/7318c5882ec9197534be.inc.c"















};

// 0x0400EDC0
static const Vtx mario_right_thigh_shared_dl_vertex_group2[] = {
#include "assets/rom/generated/3f8437c9a91509c20f72.inc.c"














};

// 0x0400EEB0 - 0x0400EFB8
const Gfx mario_right_thigh_shared_dl[] = {
    gsSPVertex(mario_right_thigh_shared_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 3,  5,  4, 0x0,  6,  7,  5, 0x0),
    gsSP2Triangles( 0,  2,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 1,  8,  2, 0x0,  2,  8,  7, 0x0),
    gsSP2Triangles( 7, 11,  5, 0x0,  5, 11,  4, 0x0),
    gsSP2Triangles( 4, 12,  1, 0x0, 13, 14,  0, 0x0),
    gsSP2Triangles( 0, 14, 15, 0x0, 15, 14,  3, 0x0),
    gsSP2Triangles( 3, 14,  6, 0x0,  6, 14, 13, 0x0),
    gsSP2Triangles(10,  9, 11, 0x0, 11,  9, 12, 0x0),
    gsSPVertex(mario_right_thigh_shared_dl_vertex_group2, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  5,  0,  6, 0x0),
    gsSP2Triangles( 7,  8,  5, 0x0,  3,  8,  7, 0x0),
    gsSP2Triangles( 4,  2,  3, 0x0,  7,  9, 10, 0x0),
    gsSP2Triangles(11,  9,  7, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles( 4, 14, 12, 0x0, 10, 14,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400EFD8
static const Vtx mario_right_leg_shared_dl_vertex_group1[] = {
#include "assets/rom/generated/cec2c83e9845e34f8745.inc.c"















};

// 0x0400F0D8
static const Vtx mario_right_leg_shared_dl_vertex_group2[] = {
#include "assets/rom/generated/17ebe05416a15cf1ed7e.inc.c"















};

// 0x0400F1D8 - 0x0400F290
const Gfx mario_right_leg_shared_dl[] = {
    gsSPVertex(mario_right_leg_shared_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10,  8, 0x0),
    gsSP2Triangles(11, 12,  2, 0x0, 13, 14,  2, 0x0),
    gsSP2Triangles( 2, 14, 15, 0x0, 15, 14,  8, 0x0),
    gsSP2Triangles( 8, 14,  5, 0x0,  5, 14, 13, 0x0),
    gsSPVertex(mario_right_leg_shared_dl_vertex_group2, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 3,  1,  4, 0x0,  4,  1,  5, 0x0),
    gsSP2Triangles( 5,  1,  0, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9,  7, 10, 0x0, 10, 11, 12, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0, 15, 13,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400F290
static const Vtx mario_right_foot_dl_vertex_group1[] = {
#include "assets/rom/generated/3dafb3e0242cdeba3534.inc.c"














};

// 0x0400F380
static const Vtx mario_right_foot_dl_vertex_group2[] = {
#include "assets/rom/generated/e212a534c5294f60355a.inc.c"







};

// 0x0400F400 - 0x0400F4E8
const Gfx mario_right_foot_dl[] = {
    gsSPVertex(mario_right_foot_dl_vertex_group1, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 3,  4,  0, 0x0,  0,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  5,  6,  0, 0x0),
    gsSP2Triangles( 3,  2,  7, 0x0,  8,  7,  2, 0x0),
    gsSP2Triangles( 1,  9,  8, 0x0,  2,  1,  8, 0x0),
    gsSP2Triangles( 7, 10,  3, 0x0, 11,  5,  4, 0x0),
    gsSP2Triangles( 4, 12, 11, 0x0,  6,  5, 13, 0x0),
    gsSP2Triangles(11, 13,  5, 0x0, 13, 14,  6, 0x0),
    gsSP2Triangles(14,  9,  6, 0x0, 10,  4,  3, 0x0),
    gsSP2Triangles( 9,  1,  6, 0x0, 10, 12,  4, 0x0),
    gsSPVertex(mario_right_foot_dl_vertex_group2, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 3,  4,  2, 0x0,  3,  5,  4, 0x0),
    gsSP2Triangles( 5,  6,  4, 0x0,  5,  7,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400F528 - 0x0400F568
const Gfx mario_metal_right_foot[] = {
    gsSPDisplayList(mario_right_foot_dl),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPTexture(0x0F80, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsSPEndDisplayList(),
};

// 0x0400F568
static const Vtx mario_yellow_button_dl_vertex[] = {
#include "assets/rom/generated/aa11448f665e4d7693fa.inc.c"













};

// 0x0400F648
static const Vtx mario_tshirt_shared_dl_vertex_group1[] = {
#include "assets/rom/generated/77a39a1378ef434677e1.inc.c"















};

// 0x0400F748
static const Vtx mario_tshirt_shared_dl_vertex_group2[] = {
#include "assets/rom/generated/94e902d50148ec13344d.inc.c"













};

// 0x0400F828
static const Vtx mario_tshirt_shared_dl_vertex_group3[] = {
#include "assets/rom/generated/a191f07c99a5173820ad.inc.c"




};

// 0x0400F878
static const Vtx mario_pants_overalls_shared_dl_vertex_group1[] = {
#include "assets/rom/generated/9adbcd5d15be95bbf7f7.inc.c"














};

// 0x0400F968
static const Vtx mario_pants_overalls_shared_dl_vertex_group2[] = {
#include "assets/rom/generated/2a77fae57bb632085eb3.inc.c"















};

// 0x0400FA68
static const Vtx mario_pants_overalls_shared_dl_vertex_group3[] = {
#include "assets/rom/generated/82f8e7d445780f953e0c.inc.c"















};

// 0x0400FB68
static const Vtx mario_pants_overalls_shared_dl_vertex_group4[] = {
#include "assets/rom/generated/dba158ee04a2d153cbae.inc.c"















};

// 0x0400FC68
static const Vtx mario_pants_overalls_shared_dl_vertex_group5[] = {
#include "assets/rom/generated/cd84ec70594c5bc509ea.inc.c"













};

// 0x0400FD48
static const Vtx mario_pants_overalls_shared_dl_vertex_group6[] = {
#include "assets/rom/generated/24de9fa663290617a216.inc.c"














};

// 0x0400FE38
static const Vtx mario_pants_overalls_shared_dl_vertex_group7[] = {
#include "assets/rom/generated/5c3c77674fbeb297c968.inc.c"














};

// 0x0400FF28 - 0x0400FF88
const Gfx mario_yellow_button_dl[] = {
    gsSPVertex(mario_yellow_button_dl_vertex, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  5,  6,  2, 0x0),
    gsSP2Triangles( 4,  5,  2, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  7,  9, 0x0,  9, 11, 12, 0x0),
    gsSP2Triangles(12, 13,  9, 0x0, 13, 10,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0400FF88 - 0x04010260
const Gfx mario_pants_overalls_shared_dl[] = {
    gsSPVertex(mario_pants_overalls_shared_dl_vertex_group1, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  1,  5, 0x0,  1,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 7,  5,  8, 0x0,  9,  6,  7, 0x0),
    gsSP2Triangles( 7,  8, 10, 0x0, 10, 11,  7, 0x0),
    gsSP2Triangles( 9,  7, 11, 0x0, 12,  6, 13, 0x0),
    gsSP2Triangles( 9, 13,  6, 0x0,  6, 12, 14, 0x0),
    gsSPVertex(mario_pants_overalls_shared_dl_vertex_group2, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10,  7, 0x0),
    gsSP2Triangles( 7, 10, 11, 0x0, 11,  8,  7, 0x0),
    gsSP2Triangles(12,  8, 11, 0x0, 13,  0,  2, 0x0),
    gsSP2Triangles( 2,  1, 14, 0x0,  2, 15, 13, 0x0),
    gsSPVertex(mario_pants_overalls_shared_dl_vertex_group3, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  7, 0x0,  3,  8,  6, 0x0),
    gsSP2Triangles( 5,  9,  3, 0x0,  8,  3,  9, 0x0),
    gsSP2Triangles( 6,  8, 10, 0x0, 11, 10,  8, 0x0),
    gsSP2Triangles( 9, 11,  8, 0x0,  6, 12,  7, 0x0),
    gsSP2Triangles(12,  6, 10, 0x0,  7, 12, 13, 0x0),
    gsSP2Triangles(14,  5, 15, 0x0, 14,  9,  5, 0x0),
    gsSPVertex(mario_pants_overalls_shared_dl_vertex_group4, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 1,  4,  3, 0x0,  5,  4,  6, 0x0),
    gsSP2Triangles( 4,  5,  3, 0x0,  2,  3,  7, 0x0),
    gsSP2Triangles( 7,  3,  5, 0x0,  8,  2,  7, 0x0),
    gsSP2Triangles( 2,  8,  0, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 11, 10, 0x0, 11,  7, 13, 0x0),
    gsSP1Triangle(14, 15, 11, 0x0),
    gsSPVertex(mario_pants_overalls_shared_dl_vertex_group5, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 2,  5,  0, 0x0,  6,  0,  7, 0x0),
    gsSP2Triangles( 4,  3,  8, 0x0,  9,  7,  4, 0x0),
    gsSP2Triangles( 7, 10, 11, 0x0, 11,  6,  7, 0x0),
    gsSP2Triangles( 9, 12, 13, 0x0, 10,  9, 13, 0x0),
    gsSP1Triangle( 7,  9, 10, 0x0),
    gsSPVertex(mario_pants_overalls_shared_dl_vertex_group6, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  8,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(14, 12, 13, 0x0, 11, 10,  9, 0x0),
    gsSPVertex(mario_pants_overalls_shared_dl_vertex_group7, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  7,  9, 0x0,  9,  8,  5, 0x0),
    gsSP2Triangles( 5,  8,  6, 0x0, 11,  1,  0, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0,  6, 14,  4, 0x0),
    gsSP1Triangle(11,  0, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x04010260 - 0x04010348
const Gfx mario_tshirt_shared_dl[] = {
    gsSPVertex(mario_tshirt_shared_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  3,  2, 0x0,  4,  5,  3, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 6,  8, 10, 0x0, 11,  8, 12, 0x0),
    gsSP2Triangles(12,  8,  7, 0x0,  8, 11, 10, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(mario_tshirt_shared_dl_vertex_group2, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 3,  1,  4, 0x0,  4,  1,  5, 0x0),
    gsSP2Triangles( 6,  2,  3, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 10,  9, 0x0),
    gsSP2Triangles(12,  3,  4, 0x0, 13,  3, 12, 0x0),
    gsSP1Triangle( 6,  3, 13, 0x0),
    gsSPVertex(mario_tshirt_shared_dl_vertex_group3, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP1Triangle( 1,  4,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x040103F0 - 0x04010410
const Gfx mario_metal_torso_shared_dl[] = {
    gsSPDisplayList(mario_yellow_button_dl),
    gsSPDisplayList(mario_pants_overalls_shared_dl),
    gsSPDisplayList(mario_tshirt_shared_dl),
    gsSPEndDisplayList(),
};

// 0x04010410
static const Vtx mario_m_logo_dl_vertex[] = {
#include "assets/rom/generated/fbb7ac3933c950c850f7.inc.c"






};

// 0x04010480
static const Vtx mario_eyes_cap_on_dl_vertex[] = {
#include "assets/rom/generated/96e9a992d2cd4699e95a.inc.c"











};

// 0x04010540
static const Vtx mario_hair_sideburn_cap_on_dl_vertex[] = {
#include "assets/rom/generated/d50769eea375f98b75f9.inc.c"











};

// 0x04010600
static const Vtx mario_mustache_cap_on_dl_vertex_group1[] = {
#include "assets/rom/generated/a94a93bc29544e21ea2a.inc.c"















};

// 0x04010700
static const Vtx mario_mustache_cap_on_dl_vertex_group2[] = {
#include "assets/rom/generated/5ff2ce38b1349ecb113c.inc.c"




};

// 0x04010750
static const Vtx mario_face_cap_dl_vertex_group1[] = {
#include "assets/rom/generated/9556cb1917280f8c6c69.inc.c"















};

// 0x04010850
static const Vtx mario_face_cap_dl_vertex_group2[] = {
#include "assets/rom/generated/a9157688a5f8600e750c.inc.c"














};

// 0x04010940
static const Vtx mario_face_cap_dl_vertex_group3[] = {
#include "assets/rom/generated/feae80a57d50c81a5890.inc.c"














};

// 0x04010A30
static const Vtx mario_face_part_cap_on_dl_vertex_group1[] = {
#include "assets/rom/generated/f7cf6b505e8de7790714.inc.c"














};

// 0x04010B20
static const Vtx mario_face_part_cap_on_dl_vertex_group2[] = {
#include "assets/rom/generated/ce698aae31f09608aadf.inc.c"















};

// 0x04010C20
static const Vtx mario_face_part_cap_on_dl_vertex_group3[] = {
#include "assets/rom/generated/092afca4ab3c77ad04de.inc.c"














};

// 0x04010D10
static const Vtx mario_face_part_cap_on_dl_vertex_group4[] = {
#include "assets/rom/generated/0896a19f0151e117c064.inc.c"















};

// 0x04010E10
static const Vtx mario_face_part_cap_on_dl_vertex_group5[] = {
#include "assets/rom/generated/4e5d6973da8986c6680e.inc.c"














};

// 0x04010F00
static const Vtx mario_face_part_cap_on_dl_vertex_group6[] = {
#include "assets/rom/generated/ce204fc71dc754b9aae5.inc.c"















};

// 0x04011000
static const Vtx mario_face_part_cap_on_dl_vertex_group7[] = {
#include "assets/rom/generated/f004a2cb7fbca3723a8f.inc.c"












};

// 0x040110D0
static const Vtx mario_face_back_hair_cap_on_dl_vertex_group1[] = {
#include "assets/rom/generated/9ae91d2161cbc44cbba3.inc.c"















};

// 0x040111D0
static const Vtx mario_face_back_hair_cap_on_dl_vertex_group2[] = {
#include "assets/rom/generated/4591479a6cfe3894b9a7.inc.c"













};

// 0x040112B0 - 0x040112E8
const Gfx mario_m_logo_dl[] = {
    gsSPVertex(mario_m_logo_dl_vertex, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 2,  3,  0, 0x0,  3,  5,  0, 0x0),
    gsSP1Triangle( 4,  6,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x040112E8 - 0x04011350
const Gfx mario_eyes_cap_on_dl[] = {
    gsSPVertex(mario_eyes_cap_on_dl_vertex, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  0,  4, 0x0,  6,  3,  5, 0x0),
    gsSP2Triangles( 3,  1,  0, 0x0,  4,  0,  7, 0x0),
    gsSP2Triangles( 5,  4,  8, 0x0,  2,  9, 10, 0x0),
    gsSP2Triangles(10,  7,  2, 0x0, 11,  7, 10, 0x0),
    gsSP1Triangle( 7,  0,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x04011350 - 0x040113A0
const Gfx mario_hair_sideburn_cap_on_dl[] = {
    gsSPVertex(mario_hair_sideburn_cap_on_dl_vertex, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  9,  6, 0x0),
    gsSP2Triangles( 9, 10,  6, 0x0, 11,  6, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x040113A0 - 0x04011438
const Gfx mario_mustache_cap_on_dl[] = {
    gsSPVertex(mario_mustache_cap_on_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 0,  4,  3, 0x0,  5,  3,  4, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  7,  3,  8, 0x0),
    gsSP2Triangles( 8,  3,  5, 0x0,  3,  7,  1, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 12, 10, 13, 0x0),
    gsSP2Triangles(11, 10, 12, 0x0, 10,  9, 14, 0x0),
    gsSP1Triangle(15,  9, 11, 0x0),
    gsSPVertex(mario_mustache_cap_on_dl_vertex_group2, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP1Triangle( 2,  4,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x04011438 - 0x040116F8
const Gfx mario_face_part_cap_on_dl[] = {
    gsSPVertex(mario_face_part_cap_on_dl_vertex_group1, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 3,  4,  2, 0x0,  1,  5,  3, 0x0),
    gsSP2Triangles( 6,  7,  3, 0x0,  7,  4,  3, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  4,  7,  8, 0x0),
    gsSP2Triangles( 9,  4,  8, 0x0, 10,  6,  5, 0x0),
    gsSP2Triangles(10,  5, 11, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(mario_face_part_cap_on_dl_vertex_group2, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  5,  6,  8, 0x0),
    gsSP2Triangles( 3,  8,  7, 0x0,  9,  3,  7, 0x0),
    gsSP2Triangles( 3,  9,  4, 0x0, 10, 11, 12, 0x0),
    gsSP2Triangles( 0, 10,  1, 0x0, 10,  0, 11, 0x0),
    gsSP2Triangles(11,  0,  2, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(mario_face_part_cap_on_dl_vertex_group3, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  3,  0,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11,  9,  8, 0x0),
    gsSP2Triangles(12, 13,  8, 0x0,  8, 13, 11, 0x0),
    gsSP1Triangle( 9, 11, 14, 0x0),
    gsSPVertex(mario_face_part_cap_on_dl_vertex_group4, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 5,  2,  4, 0x0,  2,  6,  3, 0x0),
    gsSP2Triangles( 3,  6,  7, 0x0,  8,  9,  3, 0x0),
    gsSP2Triangles( 9,  4,  3, 0x0,  7, 10,  3, 0x0),
    gsSP2Triangles( 3, 10,  8, 0x0, 11,  4,  9, 0x0),
    gsSP2Triangles(12, 11,  9, 0x0, 13,  9,  8, 0x0),
    gsSP2Triangles( 9, 13, 12, 0x0,  4, 11,  5, 0x0),
    gsSP1Triangle(14, 15, 11, 0x0),
    gsSPVertex(mario_face_part_cap_on_dl_vertex_group5, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 1,  5,  6, 0x0,  1,  3,  5, 0x0),
    gsSP2Triangles( 2,  1,  6, 0x0,  2,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  6,  5, 11, 0x0),
    gsSP2Triangles(12, 13,  8, 0x0, 10, 12,  8, 0x0),
    gsSP1Triangle(14,  8, 13, 0x0),
    gsSPVertex(mario_face_part_cap_on_dl_vertex_group6, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  6,  5,  8, 0x0),
    gsSP2Triangles( 5,  7,  3, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 6, 12,  7, 0x0, 13, 10,  9, 0x0),
    gsSP2Triangles(14, 13,  9, 0x0, 15, 10, 13, 0x0),
    gsSPVertex(mario_face_part_cap_on_dl_vertex_group7, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 1,  5,  3, 0x0,  2,  1,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  7,  6,  8, 0x0),
    gsSP2Triangles( 7,  9,  3, 0x0,  5,  7,  3, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12,  6, 10, 0x0),
    gsSP1Triangle(12,  8,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x040116F8 - 0x04011870
const Gfx mario_face_cap_dl[] = {
    gsSPVertex(mario_face_cap_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 4,  2,  5, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 2,  1,  6, 0x0,  6,  7,  2, 0x0),
    gsSP2Triangles( 5,  2,  7, 0x0,  6,  1,  8, 0x0),
    gsSP2Triangles( 8,  1,  0, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10,  7,  6, 0x0,  9, 10,  6, 0x0),
    gsSP2Triangles(11, 10,  9, 0x0,  9,  8, 11, 0x0),
    gsSP2Triangles(11,  8, 12, 0x0,  0, 12,  8, 0x0),
    gsSP2Triangles(13, 10, 11, 0x0, 11, 12, 14, 0x0),
    gsSP2Triangles(15, 11, 14, 0x0, 13, 11, 15, 0x0),
    gsSPVertex(mario_face_cap_dl_vertex_group2, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 4,  5,  0, 0x0,  0,  6,  1, 0x0),
    gsSP2Triangles( 7,  1,  8, 0x0,  8,  1,  6, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9, 12, 10, 0x0),
    gsSP2Triangles(12,  9, 13, 0x0, 13,  9, 14, 0x0),
    gsSPVertex(mario_face_cap_dl_vertex_group3, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9,  5, 0x0,  0, 10,  1, 0x0),
    gsSP2Triangles( 1, 10,  3, 0x0, 11, 10,  0, 0x0),
    gsSP2Triangles(12,  3, 10, 0x0, 12, 10, 13, 0x0),
    gsSP2Triangles(13, 10, 11, 0x0,  5,  9,  7, 0x0),
    gsSP1Triangle( 7,  9, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x04011870 - 0x04011960
const Gfx mario_face_back_hair_cap_on_dl[] = {
    gsSPVertex(mario_face_back_hair_cap_on_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  8,  6, 0x0),
    gsSP2Triangles( 5,  4,  8, 0x0,  7,  3,  5, 0x0),
    gsSP2Triangles( 0,  2,  6, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 0,  6,  8, 0x0,  3,  7, 10, 0x0),
    gsSP2Triangles( 1, 11, 12, 0x0, 12, 13, 14, 0x0),
    gsSP2Triangles(11, 13, 12, 0x0,  1, 12,  2, 0x0),
    gsSP2Triangles(14,  2, 12, 0x0, 15, 14, 13, 0x0),
    gsSPVertex(mario_face_back_hair_cap_on_dl_vertex_group2, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  3,  2, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  9, 10,  7, 0x0),
    gsSP2Triangles( 2,  1, 11, 0x0,  2, 11,  7, 0x0),
    gsSP2Triangles( 9, 12, 13, 0x0, 10,  9, 13, 0x0),
    gsSP1Triangle( 8, 12,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x04012120 - 0x04012160
const Gfx mario_metal_cap_on_shared_dl[] = {
    gsSPDisplayList(mario_m_logo_dl),
    gsSPDisplayList(mario_eyes_cap_on_dl),
    gsSPDisplayList(mario_hair_sideburn_cap_on_dl),
    gsSPDisplayList(mario_mustache_cap_on_dl),
    gsSPDisplayList(mario_face_part_cap_on_dl),
    gsSPDisplayList(mario_face_cap_dl),
    gsSPDisplayList(mario_face_back_hair_cap_on_dl),
    gsSPEndDisplayList(),
};

// 0x04012190
static const Vtx mario_eyes_cap_off_dl_vertex[] = {
#include "assets/rom/generated/469a3a8f8d903647eb53.inc.c"











};

// 0x04012250
static const Vtx mario_hair_sideburn_cap_off_dl_vertex[] = {
#include "assets/rom/generated/e63e5ef18736e079e7f0.inc.c"











};

// 0x04012310
static const Vtx mario_mustache_cap_off_dl_vertex_group1[] = {
#include "assets/rom/generated/384df426db8cfe4beb11.inc.c"















};

// 0x04012410
static const Vtx mario_mustache_cap_off_dl_vertex_group2[] = {
#include "assets/rom/generated/3a0212d32e683ca1e8fb.inc.c"




};

// 0x04012460
static const Vtx mario_face_part_cap_off_dl_vertex_group1[] = {
#include "assets/rom/generated/b665af83c9e8eb38dbed.inc.c"














};

// 0x04012550
static const Vtx mario_face_part_cap_off_dl_vertex_group2[] = {
#include "assets/rom/generated/ab89a639c10eab0710d2.inc.c"















};

// 0x04012650
static const Vtx mario_face_part_cap_off_dl_vertex_group3[] = {
#include "assets/rom/generated/97ec3966f81f46018d9a.inc.c"














};

// 0x04012740
static const Vtx mario_face_part_cap_off_dl_vertex_group4[] = {
#include "assets/rom/generated/65987a8f356aef9b6694.inc.c"














};

// 0x04012830
static const Vtx mario_face_part_cap_off_dl_vertex_group5[] = {
#include "assets/rom/generated/3e9db9ec510dd925bd2f.inc.c"















};

// 0x04012930
static const Vtx mario_face_part_cap_off_dl_vertex_group6[] = {
#include "assets/rom/generated/94d452d1b584dd7df82e.inc.c"















};

// 0x04012A30
static const Vtx mario_face_part_cap_off_dl_vertex_group7[] = {
#include "assets/rom/generated/94fef35a36f80b86d30e.inc.c"















};

// 0x04012B30
static const Vtx mario_face_part_cap_off_dl_vertex_group8[] = {
#include "assets/rom/generated/a727c716780d75bc3c59.inc.c"








};

// 0x04012BC0
static const Vtx mario_face_hair_cap_off_dl_vertex_group1[] = {
#include "assets/rom/generated/f95662ddbeabaf16c936.inc.c"















};

// 0x04012CC0
static const Vtx mario_face_hair_cap_off_dl_vertex_group2[] = {
#include "assets/rom/generated/eaf35310d17622b2e988.inc.c"














};

// 0x04012DB0
static const Vtx mario_face_hair_cap_off_dl_vertex_group3[] = {
#include "assets/rom/generated/8fec0de08a75d3191626.inc.c"















};

// 0x04012EB0
static const Vtx mario_face_hair_cap_off_dl_vertex_group4[] = {
#include "assets/rom/generated/710490d49bde1004df97.inc.c"















};

// 0x04012FB0
static const Vtx mario_face_hair_cap_off_dl_vertex_group5[] = {
#include "assets/rom/generated/39cd5aeecdbc9203d53b.inc.c"















};

// 0x040130B0
static const Vtx mario_face_hair_cap_off_dl_vertex_group6[] = {
#include "assets/rom/generated/9dced0cf8bcc8e462367.inc.c"















};

// 0x040131B0
static const Vtx mario_face_hair_cap_off_dl_vertex_group7[] = {
#include "assets/rom/generated/f528ca38f2677d6e43e1.inc.c"












};

// 0x04013280
static const Vtx mario_face_part_cap_off_dl_vertex_group9[] = {
#include "assets/rom/generated/4b8563d2c9cf9bfd26a0.inc.c"


};

// 0x040132B0 - 0x04013318
const Gfx mario_eyes_cap_off_dl[] = {
    gsSPVertex(mario_eyes_cap_off_dl_vertex, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  8,  6,  4, 0x0),
    gsSP2Triangles( 5,  4,  7, 0x0,  7,  9,  5, 0x0),
    gsSP2Triangles( 2,  8, 10, 0x0,  1,  6,  8, 0x0),
    gsSP2Triangles( 1,  8,  2, 0x0,  6, 11,  7, 0x0),
    gsSP1Triangle( 1, 11,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x04013318 - 0x040133A8
const Gfx mario_mustache_cap_off_dl[] = {
    gsSPVertex(mario_mustache_cap_off_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 0,  4,  3, 0x0,  3,  5,  1, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  6,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0,  8, 11,  6, 0x0),
    gsSP2Triangles(12,  8, 13, 0x0, 13,  8, 10, 0x0),
    gsSP2Triangles(14,  3,  4, 0x0, 13, 10, 15, 0x0),
    gsSPVertex(mario_mustache_cap_off_dl_vertex_group2, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP1Triangle( 2,  3,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x040133A8 - 0x040133F8
const Gfx mario_hair_sideburn_cap_off_dl[] = {
    gsSPVertex(mario_hair_sideburn_cap_off_dl_vertex, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 4,  2,  5, 0x0,  1,  5,  2, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  9,  6, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0, 10,  8, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x040133F8 - 0x040136D0
const Gfx mario_face_part_cap_off_dl[] = {
    gsSPVertex(mario_face_part_cap_off_dl_vertex_group1, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  3,  6,  7, 0x0),
    gsSP2Triangles( 4,  8,  2, 0x0,  8,  4,  9, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  6,  5, 10, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 14, 13, 12, 0x0),
    gsSP1Triangle(13, 14,  0, 0x0),
    gsSPVertex(mario_face_part_cap_off_dl_vertex_group2, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  8,  7, 0x0),
    gsSP2Triangles(10,  1, 11, 0x0,  1, 12, 11, 0x0),
    gsSP2Triangles(12,  1,  0, 0x0, 13, 11, 14, 0x0),
    gsSP2Triangles(12, 14, 11, 0x0, 11, 13, 15, 0x0),
    gsSPVertex(mario_face_part_cap_off_dl_vertex_group3, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  3,  5,  7, 0x0),
    gsSP2Triangles( 8,  7,  5, 0x0,  4,  8,  5, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(mario_face_part_cap_off_dl_vertex_group4, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  1,  0, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 8,  3,  9, 0x0, 10, 11, 12, 0x0),
    gsSP2Triangles(13, 11, 10, 0x0, 11, 13, 14, 0x0),
    gsSPVertex(mario_face_part_cap_off_dl_vertex_group5, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  5, 0x0,  8,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8,  6,  9, 0x0),
    gsSP2Triangles( 9, 11, 10, 0x0,  9, 12, 11, 0x0),
    gsSP2Triangles( 0, 13,  1, 0x0, 13, 14,  1, 0x0),
    gsSP1Triangle( 1, 14, 15, 0x0),
    gsSPVertex(mario_face_part_cap_off_dl_vertex_group6, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 2,  4,  0, 0x0,  1,  0,  5, 0x0),
    gsSP2Triangles( 1,  6,  7, 0x0,  1,  5,  6, 0x0),
    gsSP2Triangles( 7,  8,  1, 0x0,  3,  9,  4, 0x0),
    gsSP2Triangles( 9,  3, 10, 0x0,  4, 11,  0, 0x0),
    gsSP2Triangles(11,  4, 12, 0x0,  4,  9, 12, 0x0),
    gsSP2Triangles(10, 13, 14, 0x0,  0, 11, 15, 0x0),
    gsSP2Triangles( 5,  0, 15, 0x0, 14,  9, 10, 0x0),
    gsSPVertex(mario_face_part_cap_off_dl_vertex_group7, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 8,  5,  4, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 2, 12,  0, 0x0, 13,  4,  6, 0x0),
    gsSP2Triangles( 6, 14, 15, 0x0,  6,  7, 14, 0x0),
    gsSP1Triangle(13,  6, 15, 0x0),
    gsSPVertex(mario_face_part_cap_off_dl_vertex_group8, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  6,  5, 0x0),
    gsSP1Triangle( 8,  5,  4, 0x0),
    gsSPVertex(mario_face_part_cap_off_dl_vertex_group9, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x040136D0 - 0x040139C0
const Gfx mario_face_hair_cap_off_dl[] = {
    gsSPVertex(mario_face_hair_cap_off_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  7, 0x0,  3,  7,  4, 0x0),
    gsSP2Triangles( 8,  4,  9, 0x0,  7, 10,  4, 0x0),
    gsSP2Triangles( 4, 10,  9, 0x0,  8,  5,  4, 0x0),
    gsSP2Triangles(11,  1, 12, 0x0,  1,  0, 12, 0x0),
    gsSP2Triangles( 1, 13,  2, 0x0,  1, 11, 14, 0x0),
    gsSP2Triangles(13,  1, 14, 0x0, 15,  2, 13, 0x0),
    gsSPVertex(mario_face_hair_cap_off_dl_vertex_group2, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  3,  7,  6, 0x0),
    gsSP2Triangles( 8,  7,  3, 0x0,  5,  8,  3, 0x0),
    gsSP2Triangles( 9, 10,  4, 0x0, 11,  4,  6, 0x0),
    gsSP2Triangles(11,  9,  4, 0x0, 10,  5,  4, 0x0),
    gsSP2Triangles(10, 12,  5, 0x0,  5, 13, 14, 0x0),
    gsSP2Triangles( 8,  5, 14, 0x0,  9,  2, 10, 0x0),
    gsSPVertex(mario_face_hair_cap_off_dl_vertex_group3, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  4,  6,  5, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0, 10,  7,  9, 0x0),
    gsSP2Triangles( 8,  7, 11, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(14, 11, 13, 0x0, 14,  8, 11, 0x0),
    gsSP1Triangle(15, 13, 12, 0x0),
    gsSPVertex(mario_face_hair_cap_off_dl_vertex_group4, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  8,  5, 0x0),
    gsSP2Triangles( 8,  3,  5, 0x0,  9,  7,  5, 0x0),
    gsSP2Triangles(10,  6,  4, 0x0,  4,  3, 11, 0x0),
    gsSP2Triangles( 3, 12, 11, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(mario_face_hair_cap_off_dl_vertex_group5, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  3, 0x0,  7,  6,  8, 0x0),
    gsSP2Triangles( 9,  6,  3, 0x0,  6,  9,  8, 0x0),
    gsSP2Triangles(10, 11,  0, 0x0, 12,  0,  2, 0x0),
    gsSP2Triangles(10,  0, 12, 0x0, 10,  5, 11, 0x0),
    gsSP2Triangles(13, 14, 15, 0x0, 13,  3, 10, 0x0),
    gsSPVertex(mario_face_hair_cap_off_dl_vertex_group6, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 5,  0,  2, 0x0,  5,  4,  0, 0x0),
    gsSP2Triangles( 3,  6,  7, 0x0,  8,  9,  7, 0x0),
    gsSP2Triangles( 6, 10,  7, 0x0,  8,  7, 10, 0x0),
    gsSP2Triangles( 3, 11, 12, 0x0, 13, 14,  3, 0x0),
    gsSP2Triangles(12,  6,  3, 0x0,  6, 15, 10, 0x0),
    gsSP1Triangle(12, 15,  6, 0x0),
    gsSPVertex(mario_face_hair_cap_off_dl_vertex_group7, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  4,  7, 0x0),
    gsSP2Triangles( 6,  5,  8, 0x0,  9,  8,  5, 0x0),
    gsSP2Triangles( 5,  7,  9, 0x0, 10, 11, 12, 0x0),
    gsSP2Triangles(11, 10,  3, 0x0, 12,  2, 10, 0x0),
    gsSP2Triangles( 2,  1, 10, 0x0, 10,  1,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x04014068 - 0x04014098
const Gfx mario_metal_cap_off_shared_dl[] = {
    gsSPDisplayList(mario_eyes_cap_off_dl),
    gsSPDisplayList(mario_mustache_cap_off_dl),
    gsSPDisplayList(mario_hair_sideburn_cap_off_dl),
    gsSPDisplayList(mario_face_part_cap_off_dl),
    gsSPDisplayList(mario_face_hair_cap_off_dl),
    gsSPEndDisplayList(),
};

// 0x04014098
static const Vtx mario_medium_poly_butt_dl_vertex_group1[] = {
#include "assets/rom/generated/2d3715d9be55164049fc.inc.c"















};

// 0x04014198
static const Vtx mario_medium_poly_butt_dl_vertex_group2[] = {
#include "assets/rom/generated/92d1ed5c44f96a8b9571.inc.c"















};

// 0x04014298
static const Vtx mario_medium_poly_butt_dl_vertex_group3[] = {
#include "assets/rom/generated/0a003a07c3298da876fd.inc.c"















};

// 0x04014398
static const Vtx mario_medium_poly_butt_dl_vertex_group4[] = {
#include "assets/rom/generated/37a078944427e3a96701.inc.c"















};

// 0x04014498
static const Vtx mario_medium_poly_butt_dl_vertex_group5[] = {
#include "assets/rom/generated/9390f2c10df365ea2058.inc.c"



};

// 0x040144D8 - 0x04014638
const Gfx mario_medium_poly_butt_dl[] = {
    gsSPVertex(mario_medium_poly_butt_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15,  6, 0x0),
    gsSP1Triangle( 3, 10,  7, 0x0),
    gsSPVertex(mario_medium_poly_butt_dl_vertex_group2, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  2,  6, 0x0,  7,  5,  4, 0x0),
    gsSP2Triangles( 7,  8,  6, 0x0,  9, 10,  8, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0,  2,  1,  5, 0x0),
    gsSP1Triangle(14, 15,  0, 0x0),
    gsSPVertex(mario_medium_poly_butt_dl_vertex_group3, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  3,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 10, 14, 0x0,  0, 12, 15, 0x0),
    gsSP2Triangles(14,  9,  6, 0x0,  2, 13, 12, 0x0),
    gsSPVertex(mario_medium_poly_butt_dl_vertex_group4, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 12, 13, 0x0, 13, 12, 14, 0x0),
    gsSP2Triangles(14, 12,  8, 0x0,  8, 12, 15, 0x0),
    gsSP2Triangles(15, 12,  1, 0x0,  1, 12,  2, 0x0),
    gsSPVertex(mario_medium_poly_butt_dl_vertex_group5, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x04014668 - 0x040146E0
const Gfx mario_metal_medium_poly_butt[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_DECALFADE, G_CC_DECALFADE),
    gsDPLoadTextureBlock(mario_texture_metal, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0x07C0, 0x0F80, 0, G_TX_RENDERTILE, G_ON),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPDisplayList(mario_medium_poly_butt_dl),
    gsSPEndDisplayList(),
};

// 0x040146E0
static const Vtx mario_medium_poly_left_arm_shared_dl_vertex[] = {
#include "assets/rom/generated/64f79c48dc9b394a6742.inc.c"














};

// 0x040147D0 - 0x04014840
const Gfx mario_medium_poly_left_arm_shared_dl[] = {
    gsSPVertex(mario_medium_poly_left_arm_shared_dl_vertex, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  6,  7,  5, 0x0),
    gsSP2Triangles( 8,  2,  7, 0x0,  6,  8,  7, 0x0),
    gsSP2Triangles( 9,  6,  5, 0x0, 10,  0,  2, 0x0),
    gsSP2Triangles( 5,  2,  1, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(13, 14, 11, 0x0,  5,  7,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x04014860
static const Vtx mario_medium_poly_left_forearm_shared_dl_vertex[] = {
#include "assets/rom/generated/6ef976096dc82e1913bd.inc.c"














};

// 0x04014950 - 0x040149C0
const Gfx mario_medium_poly_left_forearm_shared_dl[] = {
    gsSPVertex(mario_medium_poly_left_forearm_shared_dl_vertex, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  6,  7,  5, 0x0),
    gsSP2Triangles( 8,  2,  7, 0x0,  6,  8,  7, 0x0),
    gsSP2Triangles( 9,  6,  5, 0x0, 10,  0,  2, 0x0),
    gsSP2Triangles( 1,  7,  2, 0x0,  1,  5,  7, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 13, 14, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x040149C0
static const Vtx mario_medium_poly_left_hand_closed_shared_dl_vertex_group1[] = {
#include "assets/rom/generated/e07111c164b3f4169e56.inc.c"















};

// 0x04014AC0
static const Vtx mario_medium_poly_left_hand_closed_shared_dl_vertex_group2[] = {
#include "assets/rom/generated/0b91dd272b7417761ffc.inc.c"














};

// 0x04014BB0
static const Vtx mario_medium_poly_left_hand_closed_shared_dl_vertex_group3[] = {
#include "assets/rom/generated/18f2f52f59bb14e8ff0e.inc.c"













};

// 0x04014C90 - 0x04014DC0
const Gfx mario_medium_poly_left_hand_closed_shared_dl[] = {
    gsSPVertex(mario_medium_poly_left_hand_closed_shared_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 5,  3,  6, 0x0,  6,  1,  0, 0x0),
    gsSP2Triangles( 2,  1,  7, 0x0,  7,  1,  6, 0x0),
    gsSP2Triangles( 6,  3,  8, 0x0,  3,  2,  8, 0x0),
    gsSP2Triangles( 0,  9, 10, 0x0, 11,  9,  0, 0x0),
    gsSP2Triangles(10, 12, 13, 0x0, 14, 12, 11, 0x0),
    gsSP2Triangles(11, 12,  9, 0x0,  9, 12, 10, 0x0),
    gsSP1Triangle(15, 11,  0, 0x0),
    gsSPVertex(mario_medium_poly_left_hand_closed_shared_dl_vertex_group2, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 9, 10,  3, 0x0,  7,  0,  2, 0x0),
    gsSP2Triangles( 7,  2, 11, 0x0,  3, 12, 13, 0x0),
    gsSP2Triangles( 3, 13,  0, 0x0, 11,  2, 14, 0x0),
    gsSPVertex(mario_medium_poly_left_hand_closed_shared_dl_vertex_group3, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 5,  2,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  2,  8, 0x0,  7,  0,  2, 0x0),
    gsSP2Triangles( 5,  8,  2, 0x0, 10, 11, 12, 0x0),
    gsSP1Triangle(10, 13, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x04014DE0
static const Vtx mario_medium_poly_right_arm_shared_dl_vertex[] = {
#include "assets/rom/generated/9a9679e413eea060cbfb.inc.c"














};

// 0x04014ED0 - 0x04014F40
const Gfx mario_medium_poly_right_arm_shared_dl[] = {
    gsSPVertex(mario_medium_poly_right_arm_shared_dl_vertex, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 2,  5,  6, 0x0,  7,  8,  5, 0x0),
    gsSP2Triangles( 7,  1,  8, 0x0,  2,  7,  5, 0x0),
    gsSP2Triangles( 0,  2,  9, 0x0,  0,  9, 10, 0x0),
    gsSP2Triangles( 1,  0,  3, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(13, 14, 11, 0x0,  1,  7,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x04014F60
static const Vtx mario_medium_poly_right_forearm_shared_dl_vertex[] = {
#include "assets/rom/generated/5f6a50ffc5ed44849f77.inc.c"














};

// 0x04015050 - 0x040150C0
const Gfx mario_medium_poly_right_forearm_shared_dl[] = {
    gsSPVertex(mario_medium_poly_right_forearm_shared_dl_vertex, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 4,  9, 10, 0x0,  3, 11,  9, 0x0),
    gsSP2Triangles( 3,  6, 11, 0x0,  4,  3,  9, 0x0),
    gsSP2Triangles( 5,  4, 12, 0x0,  5, 12, 13, 0x0),
    gsSP2Triangles( 6,  5,  7, 0x0,  2, 14,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x040150C0
static const Vtx mario_medium_poly_right_hand_closed_dl_vertex_group1[] = {
#include "assets/rom/generated/55321b646d45cbe9c8a7.inc.c"















};

// 0x040151C0
static const Vtx mario_medium_poly_right_hand_closed_dl_vertex_group2[] = {
#include "assets/rom/generated/12c66091395a3a2c817b.inc.c"















};

// 0x040152C0
static const Vtx mario_medium_poly_right_hand_closed_dl_vertex_group3[] = {
#include "assets/rom/generated/9180d158c7f29e7b6426.inc.c"














};

// 0x040153B0 - 0x040154E0
const Gfx mario_medium_poly_right_hand_closed_dl[] = {
    gsSPVertex(mario_medium_poly_right_hand_closed_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  8, 0x0),
    gsSP2Triangles( 5,  4,  8, 0x0,  9,  5,  8, 0x0),
    gsSP2Triangles(10,  4,  6, 0x0,  7, 11, 12, 0x0),
    gsSP2Triangles( 4, 13,  7, 0x0,  7, 12,  8, 0x0),
    gsSP2Triangles(14,  6, 15, 0x0,  6,  5, 15, 0x0),
    gsSPVertex(mario_medium_poly_right_hand_closed_dl_vertex_group2, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  7,  2, 0x0),
    gsSP2Triangles( 2,  8,  9, 0x0,  9, 10,  4, 0x0),
    gsSP2Triangles( 1, 11,  3, 0x0,  3, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 15, 0x0, 15, 14, 12, 0x0),
    gsSPVertex(mario_medium_poly_right_hand_closed_dl_vertex_group3, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  4, 0x0),
    gsSP2Triangles( 5,  6,  0, 0x0,  4,  6,  5, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  7,  9, 10, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11,  8, 0x0),
    gsSP2Triangles( 5, 11, 10, 0x0, 10,  9, 13, 0x0),
    gsSP2Triangles(14,  9,  8, 0x0,  8, 11,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x04015500 - 0x04015530
const Gfx mario_metal_medium_poly_right_hand_closed[] = {
    gsSPDisplayList(mario_medium_poly_right_hand_closed_dl),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPTexture(0x0F80, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsSPEndDisplayList(),
};

// 0x04015530
static const Vtx mario_medium_poly_left_thigh_dl_vertex[] = {
#include "assets/rom/generated/7a4ec5612ddc776690ac.inc.c"














};

// 0x04015620 - 0x040156B0
const Gfx mario_medium_poly_left_thigh_dl[] = {
    gsSPVertex(mario_medium_poly_left_thigh_dl_vertex, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 5,  3,  6, 0x0,  7,  5,  6, 0x0),
    gsSP2Triangles( 1,  7,  2, 0x0,  7,  8,  2, 0x0),
    gsSP2Triangles( 6,  8,  7, 0x0,  3,  4,  6, 0x0),
    gsSP2Triangles( 0,  9,  4, 0x0,  2,  9,  0, 0x0),
    gsSP2Triangles( 9,  2,  8, 0x0, 10, 11, 12, 0x0),
    gsSP2Triangles(12, 13, 10, 0x0, 13, 14, 10, 0x0),
    gsSP2Triangles( 8,  6,  9, 0x0,  6,  4,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x040156E0 - 0x04015758
const Gfx mario_metal_medium_poly_left_thigh[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_DECALFADE, G_CC_DECALFADE),
    gsDPLoadTextureBlock(mario_texture_metal, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0x07C0, 0x0F80, 0, G_TX_RENDERTILE, G_ON),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPDisplayList(mario_medium_poly_left_thigh_dl),
    gsSPEndDisplayList(),
};

// 0x04015758
static const Vtx mario_medium_poly_left_leg_shared_dl_vertex[] = {
#include "assets/rom/generated/76e3494d776847485dfa.inc.c"














};

// 0x04015848 - 0x040158D8
const Gfx mario_medium_poly_left_leg_shared_dl[] = {
    gsSPVertex(mario_medium_poly_left_leg_shared_dl_vertex, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  4, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 0,  7,  1, 0x0,  0,  8,  7, 0x0),
    gsSP2Triangles( 7,  8,  6, 0x0,  5,  6,  3, 0x0),
    gsSP2Triangles( 3,  9,  2, 0x0,  2,  9,  0, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  8,  0,  9, 0x0),
    gsSP2Triangles( 9,  6,  8, 0x0,  9,  3,  6, 0x0),
    gsSP2Triangles(12, 13, 10, 0x0, 13, 14, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x040158D8
static const Vtx mario_medium_poly_left_foot_shared_dl_vertex_group1[] = {
#include "assets/rom/generated/af4c3fb832dbc1453a55.inc.c"















};

// 0x040159D8
static const Vtx mario_medium_poly_left_foot_shared_dl_vertex_group2[] = {
#include "assets/rom/generated/54b4e9fa556d4b86c8a8.inc.c"











};

// 0x04015A98 - 0x04015B60
const Gfx mario_medium_poly_left_foot_shared_dl[] = {
    gsSPVertex(mario_medium_poly_left_foot_shared_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  5,  3,  4, 0x0),
    gsSP2Triangles( 6,  5,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13,  7,  9, 0x0),
    gsSP2Triangles(10, 12, 14, 0x0, 10, 14, 15, 0x0),
    gsSPVertex(mario_medium_poly_left_foot_shared_dl_vertex_group2, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  5,  0,  2, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles(11,  3,  0, 0x0, 10,  3, 11, 0x0),
    gsSP2Triangles( 0,  5, 11, 0x0, 11,  5,  6, 0x0),
    gsSP2Triangles(11,  6, 10, 0x0,  6,  8, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x04015B80
static const Vtx mario_medium_poly_right_thigh_shared_dl_vertex[] = {
#include "assets/rom/generated/524693925417bf016036.inc.c"














};

// 0x04015C70 - 0x04015D00
const Gfx mario_medium_poly_right_thigh_shared_dl[] = {
    gsSPVertex(mario_medium_poly_right_thigh_shared_dl_vertex, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  5,  4, 0x0,  7,  5,  6, 0x0),
    gsSP2Triangles( 8,  7,  9, 0x0, 10,  3,  8, 0x0),
    gsSP2Triangles( 4,  3, 10, 0x0,  4, 10, 11, 0x0),
    gsSP2Triangles( 8, 12, 10, 0x0,  8,  9, 12, 0x0),
    gsSP2Triangles( 7,  6,  9, 0x0,  4, 11,  6, 0x0),
    gsSP2Triangles( 5,  8,  3, 0x0,  5,  7,  8, 0x0),
    gsSP2Triangles( 2, 13,  0, 0x0,  2, 14, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x04015D20
static const Vtx mario_medium_poly_right_leg_shared_dl_vertex[] = {
#include "assets/rom/generated/560bf74efa83ba3f6208.inc.c"














};

// 0x04015E10 - 0x04015EA0
const Gfx mario_medium_poly_right_leg_shared_dl[] = {
    gsSPVertex(mario_medium_poly_right_leg_shared_dl_vertex, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 1,  0,  6, 0x0,  6,  0,  7, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  8,  2, 10, 0x0),
    gsSP2Triangles(10,  2,  1, 0x0, 11, 10,  1, 0x0),
    gsSP2Triangles(10,  9,  8, 0x0,  9, 12,  7, 0x0),
    gsSP2Triangles(12,  6,  7, 0x0,  6, 11,  1, 0x0),
    gsSP2Triangles( 5, 13,  3, 0x0,  5, 14, 13, 0x0),
    gsSP2Triangles( 2,  8,  0, 0x0,  8,  7,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x04015EA0
static const Vtx mario_medium_poly_right_foot_dl_vertex_group1[] = {
#include "assets/rom/generated/a83b587284b8ff0e391f.inc.c"















};

// 0x04015FA0
static const Vtx mario_medium_poly_right_foot_dl_vertex_group2[] = {
#include "assets/rom/generated/11c8c11099ef8f30dce3.inc.c"





};

// 0x04016000 - 0x040160C8
const Gfx mario_medium_poly_right_foot_dl[] = {
    gsSPVertex(mario_medium_poly_right_foot_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 2,  4,  3, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  0, 0x0,  5,  6,  3, 0x0),
    gsSP2Triangles( 0,  7,  1, 0x0,  1,  8,  2, 0x0),
    gsSP2Triangles( 9,  5,  4, 0x0,  4, 10,  9, 0x0),
    gsSP2Triangles( 6,  5, 11, 0x0,  9, 11,  5, 0x0),
    gsSP2Triangles(11, 12,  6, 0x0, 12,  7,  6, 0x0),
    gsSP2Triangles( 8,  4,  2, 0x0,  7,  0,  6, 0x0),
    gsSP2Triangles( 8, 10,  4, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(mario_medium_poly_right_foot_dl_vertex_group2, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 3,  4,  1, 0x0,  3,  5,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x04016108 - 0x04016148
const Gfx mario_metal_medium_poly_right_foot[] = {
    gsSPDisplayList(mario_medium_poly_right_foot_dl),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPTexture(0x0F80, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsSPEndDisplayList(),
};

// 0x04016148
static const Vtx mario_seg4_vertex_04016148[] = {
#include "assets/rom/generated/4fb44b7eac20435fd34c.inc.c"











};

// 0x04016208
static const Vtx mario_seg4_vertex_04016208[] = {
#include "assets/rom/generated/c67d0e7bc31427c6d527.inc.c"















};

// 0x04016308
static const Vtx mario_seg4_vertex_04016308[] = {
#include "assets/rom/generated/6c011335953466c11601.inc.c"








};

// 0x04016398
static const Vtx mario_seg4_vertex_04016398[] = {
#include "assets/rom/generated/1ab1682c51cf8132bc37.inc.c"















};

// 0x04016498
static const Vtx mario_seg4_vertex_04016498[] = {
#include "assets/rom/generated/9b606dbffdd8dd0b52fe.inc.c"















};

// 0x04016598
static const Vtx mario_seg4_vertex_04016598[] = {
#include "assets/rom/generated/a8aafbe64923128f160e.inc.c"












};

// 0x04016668 - 0x040166B8
const Gfx mario_medium_poly_yellow_button_dl[] = {
    gsSPVertex(mario_seg4_vertex_04016148, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  4,  5,  2, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10,  6, 0x0),
    gsSP2Triangles( 6, 10,  7, 0x0,  7, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x040166B8 - 0x04016800
const Gfx mario_medium_poly_pants_overalls_shared_dl[] = {
    gsSPVertex(mario_seg4_vertex_04016398, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 3,  4,  1, 0x0,  2,  5,  0, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10, 11, 0x0,  8,  7, 11, 0x0),
    gsSP2Triangles(11, 10,  2, 0x0, 10, 12,  2, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(mario_seg4_vertex_04016498, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  2,  1, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 5,  2,  4, 0x0,  6,  2,  5, 0x0),
    gsSP2Triangles( 4,  3,  7, 0x0,  8,  9,  7, 0x0),
    gsSP2Triangles( 5,  4,  7, 0x0,  9, 10,  7, 0x0),
    gsSP2Triangles(11,  8,  7, 0x0,  3, 11,  7, 0x0),
    gsSP2Triangles( 7, 10,  5, 0x0, 10,  9, 12, 0x0),
    gsSP2Triangles(10, 13, 14, 0x0, 12, 13, 10, 0x0),
    gsSP2Triangles( 5, 10, 14, 0x0,  9,  8, 15, 0x0),
    gsSPVertex(mario_seg4_vertex_04016598, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  3, 0x0,  6,  3,  7, 0x0),
    gsSP2Triangles( 8,  9,  6, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0, 11, 10, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x04016800 - 0x040168A0
const Gfx mario_medium_poly_tshirt_shared_dl[] = {
    gsSPVertex(mario_seg4_vertex_04016208, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 1,  0,  4, 0x0,  0,  5,  4, 0x0),
    gsSP2Triangles( 6,  4,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 9, 10,  7, 0x0,  7, 10, 11, 0x0),
    gsSP2Triangles( 7, 12,  8, 0x0, 11, 12,  7, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(mario_seg4_vertex_04016308, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  2,  4,  5, 0x0),
    gsSP2Triangles( 5,  3,  2, 0x0,  6,  7,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x04016948 - 0x04016968
const Gfx mario_metal_medium_poly_torso[] = {
    gsSPDisplayList(mario_medium_poly_yellow_button_dl),
    gsSPDisplayList(mario_medium_poly_pants_overalls_shared_dl),
    gsSPDisplayList(mario_medium_poly_tshirt_shared_dl),
    gsSPEndDisplayList(),
};

// 0x04016968
static const Vtx mario_low_poly_butt_dl_vertex[] = {
#include "assets/rom/generated/e87415a40ae174793896.inc.c"










};

// 0x04016A18 - 0x04016AB8
const Gfx mario_low_poly_butt_dl[] = {
    gsSPVertex(mario_low_poly_butt_dl_vertex, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  0,  2,  6, 0x0),
    gsSP2Triangles( 6,  5,  0, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 7,  8,  5, 0x0,  3,  9,  4, 0x0),
    gsSP2Triangles( 8,  7, 10, 0x0,  4,  9,  8, 0x0),
    gsSP2Triangles( 5,  9,  3, 0x0,  3,  0,  5, 0x0),
    gsSP2Triangles( 5,  8,  9, 0x0, 10,  7,  6, 0x0),
    gsSP2Triangles(10,  4,  8, 0x0,  2,  1, 10, 0x0),
    gsSP2Triangles( 6,  2, 10, 0x0,  1,  4, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x04016AE8 - 0x04016B60
const Gfx mario_metal_low_poly_butt[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_DECALFADE, G_CC_DECALFADE),
    gsDPLoadTextureBlock(mario_texture_metal, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0x07C0, 0x0F80, 0, G_TX_RENDERTILE, G_ON),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPDisplayList(mario_low_poly_butt_dl),
    gsSPEndDisplayList(),
};

// 0x04016B60
static const Vtx mario_low_poly_left_arm_shared_dl_vertex[] = {
#include "assets/rom/generated/00ad0cc581219a9a5212.inc.c"











};

// 0x04016C20 - 0x04016C70
const Gfx mario_low_poly_left_arm_shared_dl[] = {
    gsSPVertex(mario_low_poly_left_arm_shared_dl_vertex, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 5,  3,  1, 0x0,  6,  0,  2, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  2,  1,  4, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  7, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x04016C90
static const Vtx mario_low_poly_left_forearm_shared_dl_vertex[] = {
#include "assets/rom/generated/a0ebb1ffda896ff78bc4.inc.c"











};

// 0x04016D50 - 0x04016DA0
const Gfx mario_low_poly_left_forearm_shared_dl[] = {
    gsSPVertex(mario_low_poly_left_forearm_shared_dl_vertex, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 5,  3,  1, 0x0,  6,  0,  2, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  2,  1,  4, 0x0),
    gsSP2Triangles(10,  7,  9, 0x0, 10,  9, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x04016DA0
static const Vtx mario_low_poly_left_hand_closed_shared_dl_vertex[] = {
#include "assets/rom/generated/c52c90a1950e14220233.inc.c"







};

// 0x04016E20 - 0x04016E80
const Gfx mario_low_poly_left_hand_closed_shared_dl[] = {
    gsSPVertex(mario_low_poly_left_hand_closed_shared_dl_vertex, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  3, 0x0,  0,  6,  7, 0x0),
    gsSP2Triangles( 0,  2,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 3,  1,  4, 0x0,  2,  3,  5, 0x0),
    gsSP2Triangles( 0,  7,  4, 0x0,  4,  1,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x04016EA0
static const Vtx mario_low_poly_right_arm_shared_dl_vertex[] = {
#include "assets/rom/generated/6279a7b2fdaece193d35.inc.c"











};

// 0x04016F60 - 0x04016FB0
const Gfx mario_low_poly_right_arm_shared_dl[] = {
    gsSPVertex(mario_low_poly_right_arm_shared_dl_vertex, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 2,  6,  7, 0x0,  1,  8,  9, 0x0),
    gsSP2Triangles( 1,  0,  8, 0x0,  2,  1,  6, 0x0),
    gsSP2Triangles( 5, 10, 11, 0x0,  5, 11,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x04016FD0
static const Vtx mario_low_poly_right_forearm_shared_dl_vertex[] = {
#include "assets/rom/generated/bf401c5eff1b1ea01cbd.inc.c"











};

// 0x04017090 - 0x040170E0
const Gfx mario_low_poly_right_forearm_shared_dl[] = {
    gsSPVertex(mario_low_poly_right_forearm_shared_dl_vertex, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  2, 10, 11, 0x0),
    gsSP2Triangles( 2, 11,  0, 0x0,  9,  6,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x040170E0
static const Vtx mario_low_poly_right_hand_closed_dl_vertex[] = {
#include "assets/rom/generated/8e29448082cd1bda5f86.inc.c"







};

// 0x04017160 - 0x040171C0
const Gfx mario_low_poly_right_hand_closed_dl[] = {
    gsSPVertex(mario_low_poly_right_hand_closed_dl_vertex, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  0,  3, 0x0,  4,  5,  1, 0x0),
    gsSP2Triangles( 6,  3,  7, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  3,  2,  7, 0x0),
    gsSP2Triangles( 1,  5,  7, 0x0,  7,  2,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x040171E0 - 0x04017210
const Gfx mario_metal_low_poly_right_hand_closed[] = {
    gsSPDisplayList(mario_low_poly_right_hand_closed_dl),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPTexture(0x0F80, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsSPEndDisplayList(),
};

// 0x04017210
static const Vtx mario_low_poly_left_thigh_dl_vertex[] = {
#include "assets/rom/generated/765550cbe1f003201aa6.inc.c"













};

// 0x040172F0 - 0x04017360
const Gfx mario_low_poly_left_thigh_dl[] = {
    gsSPVertex(mario_low_poly_left_thigh_dl_vertex, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 5,  3,  4, 0x0,  4,  6,  5, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  1,  7,  2, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles( 6,  4,  7, 0x0,  4,  2,  7, 0x0),
    gsSP2Triangles(12,  7, 13, 0x0, 12,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x04017390 - 0x04017408
const Gfx mario_metal_low_poly_left_thigh[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_DECALFADE, G_CC_DECALFADE),
    gsDPLoadTextureBlock(mario_texture_metal, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0x07C0, 0x0F80, 0, G_TX_RENDERTILE, G_ON),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPDisplayList(mario_low_poly_left_thigh_dl),
    gsSPEndDisplayList(),
};

// 0x04017408
static const Vtx mario_low_poly_left_leg_shared_dl_vertex[] = {
#include "assets/rom/generated/9cd0bd37f7f0c28010b3.inc.c"













};

// 0x040174E8 - 0x04017558
const Gfx mario_low_poly_left_leg_shared_dl[] = {
    gsSPVertex(mario_low_poly_left_leg_shared_dl_vertex, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  3,  5, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 3,  4,  0, 0x0,  0,  7,  1, 0x0),
    gsSP2Triangles( 7,  4,  6, 0x0,  7,  0,  4, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles( 6, 12, 13, 0x0, 13,  7,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x04017558
static const Vtx mario_low_poly_left_foot_shared_dl_vertex[] = {
#include "assets/rom/generated/c0d3709e7d401fc817f5.inc.c"













};

// 0x04017638 - 0x040176A8
const Gfx mario_low_poly_left_foot_shared_dl[] = {
    gsSPVertex(mario_low_poly_left_foot_shared_dl_vertex, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  4,  6, 0x0,  7,  9, 11, 0x0),
    gsSP2Triangles(12, 10, 13, 0x0,  7, 10,  8, 0x0),
    gsSP2Triangles( 7, 11,  5, 0x0,  5,  4,  7, 0x0),
    gsSP2Triangles( 7,  4, 10, 0x0, 12,  8, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x040176C8
static const Vtx mario_low_poly_right_thigh_shared_dl_vertex[] = {
#include "assets/rom/generated/7a104134930c3c0d793b.inc.c"













};

// 0x040177A8 - 0x04017818
const Gfx mario_low_poly_right_thigh_shared_dl[] = {
    gsSPVertex(mario_low_poly_right_thigh_shared_dl_vertex, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles( 4,  0, 10, 0x0,  5,  4, 11, 0x0),
    gsSP2Triangles(12,  1,  5, 0x0,  5, 13, 12, 0x0),
    gsSP2Triangles( 5, 11, 13, 0x0,  4, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x04017838
static const Vtx mario_low_poly_right_leg_shared_dl_vertex[] = {
#include "assets/rom/generated/0456bb9c390324a1eda3.inc.c"













};

// 0x04017918 - 0x04017988
const Gfx mario_low_poly_right_leg_shared_dl[] = {
    gsSPVertex(mario_low_poly_right_leg_shared_dl_vertex, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9,  1, 0x0,  0,  8,  1, 0x0),
    gsSP2Triangles(10,  1,  9, 0x0,  9,  8, 11, 0x0),
    gsSP2Triangles( 8,  0, 12, 0x0, 12, 11,  8, 0x0),
    gsSP2Triangles(11, 13,  9, 0x0, 13, 10,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x04017988
static const Vtx mario_low_poly_right_foot_dl_vertex[] = {
#include "assets/rom/generated/0a86bd82e9334ad2cd34.inc.c"













};

// 0x04017A68 - 0x04017AD8
const Gfx mario_low_poly_right_foot_dl[] = {
    gsSPVertex(mario_low_poly_right_foot_dl_vertex, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 4,  3,  5, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 1,  0,  4, 0x0,  7,  0,  2, 0x0),
    gsSP2Triangles( 6,  8,  4, 0x0,  9,  3,  0, 0x0),
    gsSP2Triangles( 8,  1,  4, 0x0,  9,  5,  3, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x04017B18 - 0x04017B58
const Gfx mario_metal_low_poly_right_foot[] = {
    gsSPDisplayList(mario_low_poly_right_foot_dl),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPTexture(0x0F80, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsSPEndDisplayList(),
};

// 0x04017B58
static const Vtx mario_seg4_vertex_04017B58[] = {
#include "assets/rom/generated/03e517650e30c643cdc9.inc.c"







};

// 0x04017BD8
static const Vtx mario_seg4_vertex_04017BD8[] = {
#include "assets/rom/generated/60186eefb8af4d1182e2.inc.c"














};

// 0x04017CC8
static const Vtx mario_seg4_vertex_04017CC8[] = {
#include "assets/rom/generated/1f5fc3b9a1e5668bf6af.inc.c"









};

// 0x04017D68 - 0x04017D98
const Gfx mario_low_poly_yellow_button_dl[] = {
    gsSPVertex(mario_seg4_vertex_04017B58, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x04017D98 - 0x04017E20
const Gfx mario_low_poly_pants_overalls_shared_dl[] = {
    gsSPVertex(mario_seg4_vertex_04017BD8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  3,  6,  0, 0x0),
    gsSP2Triangles( 4,  0,  6, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 7,  4,  6, 0x0,  5,  4,  8, 0x0),
    gsSP2Triangles( 8,  1,  5, 0x0,  3,  1,  9, 0x0),
    gsSP2Triangles( 3,  2,  1, 0x0,  6,  3, 10, 0x0),
    gsSP2Triangles(10, 11,  6, 0x0, 12,  9, 13, 0x0),
    gsSP1Triangle(13,  7, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x04017E20 - 0x04017E78
const Gfx mario_low_poly_tshirt_shared_dl[] = {
    gsSPVertex(mario_seg4_vertex_04017CC8, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  3, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 6,  5,  1, 0x0,  7,  8,  0, 0x0),
    gsSP2Triangles( 7,  0,  9, 0x0,  8,  6,  1, 0x0),
    gsSP1Triangle( 1,  0,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x04017F20 - 0x04017F40
const Gfx mario_metal_low_poly_torso[] = {
    gsSPDisplayList(mario_low_poly_yellow_button_dl),
    gsSPDisplayList(mario_low_poly_pants_overalls_shared_dl),
    gsSPDisplayList(mario_low_poly_tshirt_shared_dl),
    gsSPEndDisplayList(),
};

// 0x04017F40
static const Vtx mario_low_poly_mario_m_logo_dl_vertex[] = {
#include "assets/rom/generated/0a08b179f5ff60a91835.inc.c"




};

// 0x04017F90
static const Vtx mario_low_poly_eyes_cap_on_dl_vertex[] = {
#include "assets/rom/generated/a870fd9557ff8b27c1bc.inc.c"




};

// 0x04017FE0
static const Vtx mario_low_poly_mustache_cap_on_dl_vertex[] = {
#include "assets/rom/generated/629c651717679ebd7705.inc.c"









};

// 0x04018080
static const Vtx mario_low_poly_face_cap_dl_vertex[] = {
#include "assets/rom/generated/c3d6a0fe9ee77bf91187.inc.c"











};

// 0x04018140
static const Vtx mario_low_poly_face_part_cap_on_dl_vertex[] = {
#include "assets/rom/generated/4ee2c3873c24b960d2bd.inc.c"












};

// 0x04018210
static const Vtx mario_low_poly_face_back_hair_cap_on_dl_vertex[] = {
#include "assets/rom/generated/f6b2692376488f1fafdd.inc.c"





};

// 0x04018270 - 0x04018298
const Gfx mario_low_poly_mario_m_logo_dl[] = {
    gsSPVertex(mario_low_poly_mario_m_logo_dl_vertex, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP1Triangle( 3,  1,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x04018298 - 0x040182C0
const Gfx mario_low_poly_eyes_cap_on_dl[] = {
    gsSPVertex(mario_low_poly_eyes_cap_on_dl_vertex, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  4, 0x0),
    gsSP1Triangle( 3,  2,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x040182C0 - 0x04018300
const Gfx mario_low_poly_mustache_cap_on_dl[] = {
    gsSPVertex(mario_low_poly_mustache_cap_on_dl_vertex, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 2,  4,  3, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 7,  8,  5, 0x0,  6,  9,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x04018300 - 0x04018370
const Gfx mario_low_poly_face_part_cap_on_dl[] = {
    gsSPVertex(mario_low_poly_face_part_cap_on_dl_vertex, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  2,  4,  5, 0x0),
    gsSP2Triangles( 4,  1,  0, 0x0,  4,  0,  3, 0x0),
    gsSP2Triangles( 5,  4,  3, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9,  6,  8, 0x0,  8,  7, 10, 0x0),
    gsSP2Triangles(10,  7, 11, 0x0, 10, 11, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x04018370 - 0x040183F0
const Gfx mario_low_poly_face_cap_dl[] = {
    gsSPVertex(mario_low_poly_face_cap_dl_vertex, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 6,  2,  5, 0x0,  6,  7,  2, 0x0),
    gsSP2Triangles( 7,  4,  2, 0x0,  3,  4,  8, 0x0),
    gsSP2Triangles( 4,  7,  8, 0x0,  9, 10,  3, 0x0),
    gsSP2Triangles(10, 11,  3, 0x0,  8,  7,  6, 0x0),
    gsSP2Triangles(10,  5, 11, 0x0, 10,  9,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x040183F0 - 0x04018420
const Gfx mario_low_poly_face_back_hair_cap_on_dl[] = {
    gsSPVertex(mario_low_poly_face_back_hair_cap_on_dl_vertex, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  1,  0, 0x0,  3,  2,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x04018AE0 - 0x04018B18
const Gfx mario_metal_low_poly_cap_on[] = {
    gsSPDisplayList(mario_low_poly_mario_m_logo_dl),
    gsSPDisplayList(mario_low_poly_eyes_cap_on_dl),
    gsSPDisplayList(mario_low_poly_mustache_cap_on_dl),
    gsSPDisplayList(mario_low_poly_face_part_cap_on_dl),
    gsSPDisplayList(mario_low_poly_face_cap_dl),
    gsSPDisplayList(mario_low_poly_face_back_hair_cap_on_dl),
    gsSPEndDisplayList(),
};

// 0x04018B18
static const Vtx mario_low_poly_mario_eyes_cap_off_dl_vertex[] = {
#include "assets/rom/generated/471d7b1b8c249c7b281a.inc.c"




};

// 0x04018B68
static const Vtx mario_low_poly_mustache_cap_off_dl_vertex[] = {
#include "assets/rom/generated/b94df719dcbbaaa78a6b.inc.c"









};

// 0x04018C08
static const Vtx mario_low_poly_face_part_cap_off_dl_vertex[] = {
#include "assets/rom/generated/6084efb0b57b16c79c93.inc.c"












};

// 0x04018CD8
static const Vtx mario_low_poly_face_hair_cap_off_dl_vertex[] = {
#include "assets/rom/generated/a4a71b3ae650881d2f1c.inc.c"














};

// 0x04018DC8 - 0x04018DF0
const Gfx mario_low_poly_mario_eyes_cap_off_dl[] = {
    gsSPVertex(mario_low_poly_mario_eyes_cap_off_dl_vertex, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  4, 0x0),
    gsSP1Triangle( 3,  2,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x04018DF0 - 0x04018E30
const Gfx mario_low_poly_mustache_cap_off_dl[] = {
    gsSPVertex(mario_low_poly_mustache_cap_off_dl_vertex, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 2,  4,  0, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 6,  8,  7, 0x0,  7,  9,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x04018E30 - 0x04018EA0
const Gfx mario_low_poly_face_part_cap_off_dl[] = {
    gsSPVertex(mario_low_poly_face_part_cap_off_dl_vertex, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  0, 0x0,  5,  3,  0, 0x0),
    gsSP2Triangles( 1,  3,  5, 0x0,  2,  1,  4, 0x0),
    gsSP2Triangles( 1,  5,  4, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 7,  9,  8, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles(10,  9, 11, 0x0, 10, 11, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x04018EA0 - 0x04018F68
const Gfx mario_low_poly_face_hair_cap_off_dl[] = {
    gsSPVertex(mario_low_poly_face_hair_cap_off_dl_vertex, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 0,  4,  3, 0x0,  0,  2,  5, 0x0),
    gsSP2Triangles( 5,  6,  0, 0x0,  0,  6,  4, 0x0),
    gsSP2Triangles( 5,  2,  7, 0x0,  7,  8,  5, 0x0),
    gsSP2Triangles( 8,  6,  5, 0x0,  9,  7, 10, 0x0),
    gsSP2Triangles( 7,  9, 11, 0x0,  7, 11,  8, 0x0),
    gsSP2Triangles( 6,  8,  4, 0x0,  8, 11, 12, 0x0),
    gsSP2Triangles( 4,  8, 12, 0x0, 11,  9, 12, 0x0),
    gsSP2Triangles(13, 14, 12, 0x0, 14,  4, 12, 0x0),
    gsSP2Triangles( 4, 14,  3, 0x0, 13, 10,  3, 0x0),
    gsSP2Triangles(14, 13,  3, 0x0,  9, 10, 13, 0x0),
    gsSP1Triangle( 9, 13, 12, 0x0),
    gsSPEndDisplayList(),
};

// 0x04019510 - 0x04019538
const Gfx mario_metal_low_poly_cap_off[] = {
    gsSPDisplayList(mario_low_poly_mario_eyes_cap_off_dl),
    gsSPDisplayList(mario_low_poly_mustache_cap_off_dl),
    gsSPDisplayList(mario_low_poly_face_part_cap_off_dl),
    gsSPDisplayList(mario_low_poly_face_hair_cap_off_dl),
    gsSPEndDisplayList(),
};

// 0x04019538
static const Vtx mario_left_hand_open_shared_dl_vertex_group1[] = {
#include "assets/rom/generated/7830427368a734fc0fcf.inc.c"













};

// 0x04019618
static const Vtx mario_left_hand_open_shared_dl_vertex_group2[] = {
#include "assets/rom/generated/328bd348cc219dee7e94.inc.c"















};

// 0x04019718
static const Vtx mario_left_hand_open_shared_dl_vertex_group3[] = {
#include "assets/rom/generated/a52d4f4a5561266f4252.inc.c"















};

// 0x04019818
static const Vtx mario_left_hand_open_shared_dl_vertex_group4[] = {
#include "assets/rom/generated/3e605fe56873c3062e83.inc.c"















};

// 0x04019918
static const Vtx mario_left_hand_open_shared_dl_vertex_group5[] = {
#include "assets/rom/generated/61c3fb731156778c841d.inc.c"















};

// 0x04019A18
static const Vtx mario_left_hand_open_shared_dl_vertex_group6[] = {
#include "assets/rom/generated/056a38928548c61ce929.inc.c"




};

// 0x04019A68 - 0x04019CA0
const Gfx mario_left_hand_open_shared_dl[] = {
    gsSPVertex(mario_left_hand_open_shared_dl_vertex_group1, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  3,  7, 0x0),
    gsSP2Triangles( 1,  0,  5, 0x0,  5,  4,  1, 0x0),
    gsSP2Triangles( 5,  3,  6, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 8, 11,  9, 0x0, 12, 13, 11, 0x0),
    gsSPVertex(mario_left_hand_open_shared_dl_vertex_group2, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 0, 10,  7, 0x0, 11, 10,  0, 0x0),
    gsSP2Triangles(12, 13, 11, 0x0,  9,  8, 13, 0x0),
    gsSP2Triangles( 7,  5,  8, 0x0, 10, 14,  7, 0x0),
    gsSP2Triangles(11, 14, 10, 0x0, 13,  4, 11, 0x0),
    gsSP2Triangles( 8,  5, 13, 0x0,  3, 15,  1, 0x0),
    gsSPVertex(mario_left_hand_open_shared_dl_vertex_group3, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  5,  3, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 1,  9, 10, 0x0,  5, 11,  1, 0x0),
    gsSP2Triangles(12, 13,  5, 0x0, 14,  9, 15, 0x0),
    gsSP1Triangle( 4,  7, 12, 0x0),
    gsSPVertex(mario_left_hand_open_shared_dl_vertex_group4, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  7,  0,  2, 0x0),
    gsSP2Triangles( 8,  6,  5, 0x0,  5,  9, 10, 0x0),
    gsSP2Triangles(11,  3,  2, 0x0,  2, 12, 11, 0x0),
    gsSP2Triangles( 3, 13,  9, 0x0,  6,  0,  7, 0x0),
    gsSP2Triangles( 7,  4,  9, 0x0, 10, 14, 15, 0x0),
    gsSP1Triangle(15, 14, 11, 0x0),
    gsSPVertex(mario_left_hand_open_shared_dl_vertex_group5, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 4,  3,  5, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 7,  6,  0, 0x0,  2,  8,  9, 0x0),
    gsSP2Triangles( 4, 10,  8, 0x0, 11, 10,  4, 0x0),
    gsSP2Triangles( 7, 12, 11, 0x0,  9, 12,  7, 0x0),
    gsSP2Triangles( 6,  3,  1, 0x0,  6,  5,  3, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 14, 10, 11, 0x0),
    gsSP2Triangles( 8, 10, 15, 0x0, 10, 14, 15, 0x0),
    gsSPVertex(mario_left_hand_open_shared_dl_vertex_group6, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x04019CC0
static const Vtx mario_right_hand_open_dl_vertex_group1[] = {
#include "assets/rom/generated/7c4d2a581d3cc8dd70b3.inc.c"













};

// 0x04019DA0
static const Vtx mario_right_hand_open_dl_vertex_group2[] = {
#include "assets/rom/generated/977d5137ba8476eda9e1.inc.c"















};

// 0x04019EA0
static const Vtx mario_right_hand_open_dl_vertex_group3[] = {
#include "assets/rom/generated/f41901c5786c080871ce.inc.c"















};

// 0x04019FA0
static const Vtx mario_right_hand_open_dl_vertex_group4[] = {
#include "assets/rom/generated/2f171eec124d614871a5.inc.c"















};

// 0x0401A0A0
static const Vtx mario_right_hand_open_dl_vertex_group5[] = {
#include "assets/rom/generated/1d79a32250b853e0b375.inc.c"















};

// 0x0401A1A0
static const Vtx mario_right_hand_open_dl_vertex_group6[] = {
#include "assets/rom/generated/26717eec1f7ee405739e.inc.c"




};

// 0x0401A1F0 - 0x0401A428
const Gfx mario_right_hand_open_dl[] = {
    gsSPVertex(mario_right_hand_open_dl_vertex_group1, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  3,  4, 0x0),
    gsSP2Triangles( 5,  2,  1, 0x0,  1,  6,  5, 0x0),
    gsSP2Triangles( 4,  3,  5, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 9, 11, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(mario_right_hand_open_dl_vertex_group2, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 9, 10,  2, 0x0,  2, 10, 11, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 12,  8,  7, 0x0),
    gsSP2Triangles( 8,  5,  9, 0x0,  9, 14, 10, 0x0),
    gsSP2Triangles(10, 14, 11, 0x0, 11,  6, 12, 0x0),
    gsSP2Triangles(12,  5,  8, 0x0,  1, 15,  3, 0x0),
    gsSPVertex(mario_right_hand_open_dl_vertex_group3, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9, 10,  1, 0x0,  1, 11,  4, 0x0),
    gsSP2Triangles( 4, 12, 13, 0x0, 14, 10, 15, 0x0),
    gsSP1Triangle(13,  7,  5, 0x0),
    gsSPVertex(mario_right_hand_open_dl_vertex_group4, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  0,  2,  5, 0x0),
    gsSP2Triangles( 7,  6,  8, 0x0,  9, 10,  7, 0x0),
    gsSP2Triangles( 0,  4, 11, 0x0, 11, 12,  0, 0x0),
    gsSP2Triangles(10, 13,  4, 0x0,  5,  2,  6, 0x0),
    gsSP2Triangles(10,  3,  5, 0x0, 14, 15,  9, 0x0),
    gsSP1Triangle(11, 15, 14, 0x0),
    gsSPVertex(mario_right_hand_open_dl_vertex_group5, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 5,  4,  3, 0x0,  6,  7,  5, 0x0),
    gsSP2Triangles( 2,  7,  6, 0x0,  8,  9,  0, 0x0),
    gsSP2Triangles( 9, 10,  3, 0x0,  3, 10, 11, 0x0),
    gsSP2Triangles(11, 12,  6, 0x0,  6, 12,  8, 0x0),
    gsSP2Triangles( 1,  4,  7, 0x0,  4,  5,  7, 0x0),
    gsSP2Triangles(13, 12, 11, 0x0, 11, 10, 14, 0x0),
    gsSP2Triangles(15, 10,  9, 0x0, 15, 14, 10, 0x0),
    gsSPVertex(mario_right_hand_open_dl_vertex_group6, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401A448 - 0x0401A478
const Gfx mario_metal_right_hand_open[] = {
    gsSPDisplayList(mario_right_hand_open_dl),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPTexture(0x0F80, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsSPEndDisplayList(),
};

// 0x0401A478
static const Vtx mario_right_hand_cap_m_logo_dl_vertex[] = {
#include "assets/rom/generated/0b1254d82ec47d8ac10f.inc.c"




};

// 0x0401A4C8
static const Vtx mario_right_hand_cap_hand_position_dl_vertex_group1[] = {
#include "assets/rom/generated/790d5dcf341db05f6781.inc.c"














};

// 0x0401A5B8
static const Vtx mario_right_hand_cap_hand_position_dl_vertex_group2[] = {
#include "assets/rom/generated/6be1d8e824f7f592202f.inc.c"















};

// 0x0401A6B8
static const Vtx mario_right_hand_cap_hand_position_dl_vertex_group3[] = {
#include "assets/rom/generated/f3b9fe0ad56ef1cbbf75.inc.c"













};

// 0x0401A798
static const Vtx mario_right_hand_cap_top_dl_vertex_group1[] = {
#include "assets/rom/generated/e3515ff1c4508f15f82d.inc.c"














};

// 0x0401A888
static const Vtx mario_right_hand_cap_top_dl_vertex_group2[] = {
#include "assets/rom/generated/ff14e9086dff3d15dda5.inc.c"















};

// 0x0401A988
static const Vtx mario_right_hand_cap_top_dl_vertex_group3[] = {
#include "assets/rom/generated/5582ee9c4d1d4d29d4fb.inc.c"















};

// 0x0401AA88
static const Vtx mario_right_hand_cap_top_dl_vertex_group4[] = {
#include "assets/rom/generated/83575a660c2495361348.inc.c"







};

// 0x0401AB08
static const Vtx mario_right_hand_cap_bottom_dl_vertex[] = {
#include "assets/rom/generated/9acdac14d934a3b7a382.inc.c"









};

// 0x0401ABA8 - 0x0401ABD0
const Gfx mario_right_hand_cap_m_logo_dl[] = {
    gsSPVertex(mario_right_hand_cap_m_logo_dl_vertex, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP1Triangle( 1,  4,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401ABD0 - 0x0401AD40
const Gfx mario_right_hand_cap_top_dl[] = {
    gsSPVertex(mario_right_hand_cap_top_dl_vertex_group1, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 2,  4,  5, 0x0,  6,  2,  1, 0x0),
    gsSP2Triangles( 2,  6,  4, 0x0,  7,  4,  8, 0x0),
    gsSP2Triangles( 8,  4,  6, 0x0,  4,  9,  5, 0x0),
    gsSP2Triangles( 4,  7,  9, 0x0,  5,  9, 10, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 13, 14, 11, 0x0),
    gsSPVertex(mario_right_hand_cap_top_dl_vertex_group2, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 0,  3,  1, 0x0,  5,  2,  1, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  0,  9,  3, 0x0),
    gsSP2Triangles(10,  9,  0, 0x0, 11, 12,  2, 0x0),
    gsSP2Triangles( 2,  5, 11, 0x0,  3,  9,  4, 0x0),
    gsSP2Triangles(13, 14, 15, 0x0, 11,  5, 14, 0x0),
    gsSP1Triangle( 7, 14,  5, 0x0),
    gsSPVertex(mario_right_hand_cap_top_dl_vertex_group3, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  7,  6,  8, 0x0),
    gsSP2Triangles(10,  7,  9, 0x0, 11, 12,  9, 0x0),
    gsSP2Triangles(10, 13, 14, 0x0, 12, 13, 10, 0x0),
    gsSP1Triangle(13, 12, 15, 0x0),
    gsSPVertex(mario_right_hand_cap_top_dl_vertex_group4, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 3,  4,  5, 0x0,  3,  5,  6, 0x0),
    gsSP1Triangle( 7,  0,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401AD40 - 0x0401AED0
const Gfx mario_right_hand_cap_hand_position_dl[] = {
    gsSPVertex(mario_right_hand_cap_hand_position_dl_vertex_group1, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  2,  5,  6, 0x0),
    gsSP2Triangles( 6,  3,  2, 0x0,  5,  2,  4, 0x0),
    gsSP2Triangles( 3,  7,  0, 0x0,  3,  6,  8, 0x0),
    gsSP2Triangles( 3,  8,  7, 0x0,  9,  0,  7, 0x0),
    gsSP2Triangles(10,  0,  9, 0x0,  1,  0, 10, 0x0),
    gsSP2Triangles(11,  1, 10, 0x0, 11,  4,  1, 0x0),
    gsSP2Triangles( 7, 12, 13, 0x0,  8, 12,  7, 0x0),
    gsSP2Triangles(13, 14,  7, 0x0,  7, 14,  9, 0x0),
    gsSPVertex(mario_right_hand_cap_hand_position_dl_vertex_group2, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  6,  2,  4, 0x0),
    gsSP2Triangles( 7,  5,  4, 0x0,  8,  9,  3, 0x0),
    gsSP2Triangles( 3,  9, 10, 0x0, 10,  6,  3, 0x0),
    gsSP2Triangles( 5, 11,  3, 0x0,  3, 11,  8, 0x0),
    gsSP2Triangles(10,  0,  6, 0x0,  6,  0,  2, 0x0),
    gsSP2Triangles( 2,  1, 12, 0x0,  5, 13, 11, 0x0),
    gsSP2Triangles( 1, 14, 12, 0x0, 14, 15, 12, 0x0),
    gsSPVertex(mario_right_hand_cap_hand_position_dl_vertex_group3, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 7,  5,  8, 0x0,  8,  5,  9, 0x0),
    gsSP2Triangles( 3, 10,  1, 0x0, 11, 10,  3, 0x0),
    gsSP2Triangles( 1, 10,  2, 0x0,  2, 10, 12, 0x0),
    gsSP2Triangles(12, 10, 13, 0x0, 10, 11, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401AED0 - 0x0401AF20
const Gfx mario_right_hand_cap_bottom_dl[] = {
    gsSPVertex(mario_right_hand_cap_bottom_dl_vertex, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  3,  2, 0x0,  5,  4,  2, 0x0),
    gsSP2Triangles( 6,  5,  2, 0x0,  6,  2,  7, 0x0),
    gsSP2Triangles( 8,  9,  6, 0x0,  6,  7,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401AF60
static const Vtx mario_right_hand_cap_wings_half_1_dl_vertex[] = {
#include "assets/rom/generated/50c160c61a63ceb23ac8.inc.c"









};

// 0x0401B000
static const Vtx mario_right_hand_cap_wings_half_2_dl_vertex[] = {
#include "assets/rom/generated/4f27e6ef3ee51f396c90.inc.c"







};

// 0x0401B080 - 0x0401B0B0
const Gfx mario_right_hand_cap_wings_half_1_dl[] = {
    gsSPVertex(mario_right_hand_cap_wings_half_1_dl_vertex, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  9,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401B0B0 - 0x0401B0E0
const Gfx mario_right_hand_cap_wings_half_2_dl[] = {
    gsSPVertex(mario_right_hand_cap_wings_half_2_dl_vertex, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  4,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401B0E0 - 0x0401B138
const Gfx mario_right_hand_cap_wings_initial_dl[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBFADEA, G_CC_MODULATERGBFADEA),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPEndDisplayList(),
};

// 0x0401B138 - 0x0401B158
const Gfx mario_right_hand_cap_wings_end_dl[] = {
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x0401B1D8 - 0x0401B230
const Gfx mario_right_hand_cap_wings[] = {
    gsSPDisplayList(mario_right_hand_cap_wings_initial_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_wings_half_1),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_right_hand_cap_wings_half_1_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_wings_half_2),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_right_hand_cap_wings_half_2_dl),
    gsSPDisplayList(mario_right_hand_cap_wings_end_dl),
    gsSPEndDisplayList(),
};

// 0x0401B278 - 0x0401B2D0
const Gfx mario_metal_right_hand_cap_shared_dl_wings[] = {
    gsSPDisplayList(mario_right_hand_cap_wings_initial_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_metal_wings_half_1),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_right_hand_cap_wings_half_1_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_metal_wings_half_2),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_right_hand_cap_wings_half_2_dl),
    gsSPDisplayList(mario_right_hand_cap_wings_end_dl),
    gsSPEndDisplayList(),
};

// 0x0401B2D0
static const Vtx mario_right_hand_peace_shared_dl_vertex_group1[] = {
#include "assets/rom/generated/5ca442dba6e0c682965f.inc.c"















};

// 0x0401B3D0
static const Vtx mario_right_hand_peace_shared_dl_vertex_group2[] = {
#include "assets/rom/generated/2371de377002a5bbf7f0.inc.c"













};

// 0x0401B4B0
static const Vtx mario_right_hand_peace_shared_dl_vertex_group3[] = {
#include "assets/rom/generated/b9cc10863919cb97b890.inc.c"














};

// 0x0401B5A0
static const Vtx mario_right_hand_peace_shared_dl_vertex_group4[] = {
#include "assets/rom/generated/20fcd7b0a2f1adbd45ad.inc.c"















};

// 0x0401B6A0
static const Vtx mario_right_hand_peace_shared_dl_vertex_group5[] = {
#include "assets/rom/generated/2c9aedd46b9a2e3adb53.inc.c"















};

// 0x0401B7A0
static const Vtx mario_right_hand_peace_shared_dl_vertex_group6[] = {
#include "assets/rom/generated/db8fadb4e01ba5f9b997.inc.c"














};

// 0x0401B890
static const Vtx mario_right_hand_peace_shared_dl_vertex_group7[] = {
#include "assets/rom/generated/fa11629c586d142864ec.inc.c"














};

// 0x0401B980
static const Vtx mario_right_hand_peace_shared_dl_vertex_group8[] = {
#include "assets/rom/generated/a320386d83a617dd2a1f.inc.c"














};

// 0x0401BA70
static const Vtx mario_right_hand_peace_shared_dl_vertex_group9[] = {
#include "assets/rom/generated/834fad7d94f2c1813125.inc.c"















};

// 0x0401BB70
static const Vtx mario_right_hand_peace_shared_dl_vertex_group10[] = {
#include "assets/rom/generated/bc3402f08e8c10b28699.inc.c"













};

// 0x0401BC50
static const Vtx mario_right_hand_peace_shared_dl_vertex_group11[] = {
#include "assets/rom/generated/4c9f3e196e2835af9bc6.inc.c"


};

// 0x0401BC80 - 0x0401BF30
const Gfx mario_right_hand_peace_shared_dl[] = {
    gsSPVertex(mario_right_hand_peace_shared_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 2,  4,  3, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 8,  7,  9, 0x0,  7,  6, 10, 0x0),
    gsSP2Triangles( 7, 10,  9, 0x0, 11,  8,  9, 0x0),
    gsSP2Triangles( 7, 12,  5, 0x0, 10, 13, 14, 0x0),
    gsSP2Triangles( 6, 15, 10, 0x0, 10, 14,  9, 0x0),
    gsSPVertex(mario_right_hand_peace_shared_dl_vertex_group2, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  0,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(mario_right_hand_peace_shared_dl_vertex_group3, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 7,  6,  8, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(mario_right_hand_peace_shared_dl_vertex_group4, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 2, 12, 13, 0x0,  3,  5, 12, 0x0),
    gsSP1Triangle(12, 14, 15, 0x0),
    gsSPVertex(mario_right_hand_peace_shared_dl_vertex_group5, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(mario_right_hand_peace_shared_dl_vertex_group6, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 4, 10, 11, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(mario_right_hand_peace_shared_dl_vertex_group7, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10,  7, 0x0),
    gsSP2Triangles(10,  9, 11, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(mario_right_hand_peace_shared_dl_vertex_group8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  3,  2,  7, 0x0),
    gsSP2Triangles( 8,  3,  7, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(mario_right_hand_peace_shared_dl_vertex_group9, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(mario_right_hand_peace_shared_dl_vertex_group10, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13,  9, 0x0, 12,  9,  8, 0x0),
    gsSPVertex(mario_right_hand_peace_shared_dl_vertex_group11, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401BF50
static const Vtx mario_cap_unused_m_logo_dl_vertex[] = {
#include "assets/rom/generated/e2e7bc4f54f66916e917.inc.c"






};

// 0x0401BFC0
static const Vtx mario_cap_unused_base_top_dl_vertex_group1[] = {
#include "assets/rom/generated/1e4ce2e8e7f6e9eab466.inc.c"















};

// 0x0401C0C0
static const Vtx mario_cap_unused_base_top_dl_vertex_group2[] = {
#include "assets/rom/generated/b1fd2f0729d11d09edb8.inc.c"














};

// 0x0401C1B0
static const Vtx mario_cap_unused_base_top_dl_vertex_group3[] = {
#include "assets/rom/generated/930f949a59821384e42a.inc.c"














};

// 0x0401C2A0
static const Vtx mario_cap_unused_base_bottom_dl_vertex[] = {
#include "assets/rom/generated/cd749c77e83b9bc17ede.inc.c"








};

// 0x0401C330 - 0x0401C368
const Gfx mario_cap_unused_m_logo_dl[] = {
    gsSPVertex(mario_cap_unused_m_logo_dl_vertex, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 3,  5,  4, 0x0,  2,  3,  0, 0x0),
    gsSP1Triangle( 5,  6,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401C368 - 0x0401C4C8
const Gfx mario_cap_unused_base_top_dl[] = {
    gsSPVertex(mario_cap_unused_base_top_dl_vertex_group1, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 3,  1,  0, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 3,  0,  5, 0x0,  5,  7,  8, 0x0),
    gsSP2Triangles( 5,  8,  6, 0x0,  0,  2,  5, 0x0),
    gsSP2Triangles( 6,  9, 10, 0x0,  8,  9,  6, 0x0),
    gsSP2Triangles(11,  8,  7, 0x0,  8, 12,  9, 0x0),
    gsSP2Triangles(12,  8, 11, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(mario_cap_unused_base_top_dl_vertex_group2, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 4,  2,  1, 0x0,  5,  1,  0, 0x0),
    gsSP2Triangles( 1,  6,  3, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9,  7,  6, 0x0, 10,  4,  3, 0x0),
    gsSP2Triangles( 9, 11,  7, 0x0,  7, 11,  8, 0x0),
    gsSP2Triangles(12, 11,  9, 0x0, 13, 14,  4, 0x0),
    gsSPVertex(mario_cap_unused_base_top_dl_vertex_group3, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 5,  3,  1, 0x0,  1,  4,  2, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  0,  2,  7, 0x0),
    gsSP2Triangles( 8,  7,  2, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 10,  9, 0x0,  7, 10,  0, 0x0),
    gsSP2Triangles(10,  7, 11, 0x0,  0, 10, 12, 0x0),
    gsSP2Triangles(11,  8, 13, 0x0, 14, 12,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401C4C8 - 0x0401C510
const Gfx mario_cap_unused_base_bottom_dl[] = {
    gsSPVertex(mario_cap_unused_base_bottom_dl_vertex, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  7,  6,  5, 0x0),
    gsSP2Triangles( 0,  7,  5, 0x0,  0,  5,  1, 0x0),
    gsSP1Triangle( 2,  8,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401C538
static const Vtx mario_wings_half_1_unused_dl_vertex[] = {
#include "assets/rom/generated/7279e792bcc676f59868.inc.c"











};

// 0x0401C5F8
static const Vtx mario_wings_half_2_unused_dl_vertex[] = {
#include "assets/rom/generated/4fce0df9f1868541ed6c.inc.c"







};

// 0x0401C678 - 0x0401C6A8
const Gfx mario_wings_half_1_unused_dl[] = {
    gsSPVertex(mario_wings_half_1_unused_dl_vertex, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401C6A8 - 0x0401C6D8
const Gfx mario_wings_half_2_unused_dl[] = {
    gsSPVertex(mario_wings_half_2_unused_dl_vertex, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401C6D8 - 0x0401C730
const Gfx mario_cap_wings_unused_initial_dl[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPEndDisplayList(),
};

// 0x0401C730 - 0x0401C758
const Gfx mario_cap_wings_unused_end_dl[] = {
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x0401C7E8 - 0x0401C890 # Unreferenced, possibly an early duplicate of the cap object used in segment 03
const Gfx mario_metal_cap_unused_dl[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPLoadTextureBlock(mario_texture_metal, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0x07C0, 0x0F80, 0, G_TX_RENDERTILE, G_ON),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPDisplayList(mario_cap_unused_m_logo_dl),
    gsSPDisplayList(mario_cap_unused_base_top_dl),
    gsSPDisplayList(mario_cap_unused_base_bottom_dl),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPTexture(0x0F80, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsSPEndDisplayList(),
};

// 0x0401C890 - 0x0401C8E8 # Unreferenced, unused like the duplicated cap, same case
const Gfx mario_cap_wings_unused[] = {
    gsSPDisplayList(mario_cap_wings_unused_initial_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_wings_half_1),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_wings_half_1_unused_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_wings_half_2),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_wings_half_2_unused_dl),
    gsSPDisplayList(mario_cap_wings_unused_end_dl),
    gsSPEndDisplayList(),
};

// 0x0401C8E8 - 0x0401C940 # Unreferenced, unused like the duplicated cap, same case
const Gfx mario_metal_cap_wings_unused[] = {
    gsSPDisplayList(mario_cap_wings_unused_initial_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_metal_wings_half_1),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_wings_half_1_unused_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_metal_wings_half_2),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_wings_half_2_unused_dl),
    gsSPDisplayList(mario_cap_wings_unused_end_dl),
    gsSPEndDisplayList(),
};

// 0x0401C940
static const Vtx mario_wings_half_1_dl_vertex[] = {
#include "assets/rom/generated/c11e8d3c3b6e95614fee.inc.c"



};

// 0x0401C980
static const Vtx mario_wings_half_2_dl_vertex[] = {
#include "assets/rom/generated/3543979187927c5d7df7.inc.c"



};

// 0x0401C9C0 - 0x0401C9E0
const Gfx mario_wings_half_1_dl[] = {
    gsSPVertex(mario_wings_half_1_dl_vertex, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401C9E0 - 0x0401CA00
const Gfx mario_wings_half_2_dl[] = {
    gsSPVertex(mario_wings_half_2_dl_vertex, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0401CA00 - 0x0401CAB8
const Gfx mario_cap_wings[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_wings_half_1),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_wings_half_1_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_wings_half_2),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_wings_half_2_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x0401CAB8 - 0x0401CB70
const Gfx mario_cap_wings_transparent[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALFADEA, G_CC_DECALFADEA),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_wings_half_1),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_wings_half_1_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_wings_half_2),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_wings_half_2_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x0401CB70 - 0x0401CC28
const Gfx mario_metal_cap_wings[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_metal_wings_half_1),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_wings_half_1_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_metal_wings_half_2),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_wings_half_2_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x0401CC28 - 0x0401CD20
const Gfx mario_metal_cap_wings_transparent[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsSPTexture(0x0F80, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetCombineMode(G_CC_DECALFADEA, G_CC_DECALFADEA),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_metal_wings_half_1),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_wings_half_1_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_metal_wings_half_2),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_wings_half_2_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN | G_CULL_BACK | G_SHADING_SMOOTH),
    gsDPSetCombineMode(G_CC_DECALFADE, G_CC_DECALFADE),
    gsDPLoadTextureBlock(mario_texture_metal, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0x07C0, 0x0F80, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

#include "actors/mario/he_thicc_body.inc.c"

#define MARIO_PALETTE_VARIANT 0
#define MARIO_BODY_NAME(name) name
#define MARIO_PALETTE_NAME(name) name
#include "actors/mario/palette.h"
#include "actors/mario/palette_materials.inc.c"
#include "actors/mario/palette_undef.h"
#undef MARIO_PALETTE_NAME
#undef MARIO_PALETTE_VARIANT
#undef MARIO_BODY_NAME


#include "actors/mario/silhouette.inc.c"

#define MARIO_PALETTE_VARIANT 1
#define MARIO_BODY_NAME(name) he_thicc_body_##name
#define MARIO_PALETTE_NAME(name) he_thicc_##name
#include "actors/mario/palette.h"
#include "actors/mario/palette_materials.inc.c"
#include "actors/mario/palette_undef.h"
#undef MARIO_PALETTE_NAME
#undef MARIO_PALETTE_VARIANT
#undef MARIO_BODY_NAME

#include "actors/mario/dither.inc.c"
