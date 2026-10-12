// Birds

// 0x05000000

// 0x05000018

// 0x05000030

// 0x05000048
static const Vtx birds_seg5_vertex_05000048[] = {
#include "assets/rom/generated/48618f738422d178b15e.inc.c"








};

// 0x050000D8
static const Vtx birds_seg5_vertex_050000D8[] = {
#include "assets/rom/generated/78a91dd1b2290bbefd87.inc.c"


};

// 0x05000108
static const Vtx birds_seg5_vertex_05000108[] = {
#include "assets/rom/generated/81650e7c45c1d8eb8454.inc.c"


};

// 0x05000138
static const Vtx birds_seg5_vertex_05000138[] = {
#include "assets/rom/generated/0d0d7c64b958d2d9d03c.inc.c"


};

// 0x05000168
static const Vtx birds_seg5_vertex_05000168[] = {
#include "assets/rom/generated/f3ffbd1647124c6b02ab.inc.c"








};

// 0x050001F8
static const Vtx birds_seg5_vertex_050001F8[] = {
#include "assets/rom/generated/67786608109430ea5f69.inc.c"















};

// 0x050002F8
static const Vtx birds_seg5_vertex_050002F8[] = {
#include "assets/rom/generated/8df13df8eb553bb1bcb1.inc.c"















};

// 0x050003F8
static const Vtx birds_seg5_vertex_050003F8[] = {
#include "assets/rom/generated/ee23166fdf0b48ef82c5.inc.c"


};

// 0x05000428
static const Vtx birds_seg5_vertex_05000428[] = {
#include "assets/rom/generated/e15c0b31b0f6e9b91ea8.inc.c"









};

// 0x050004C8
static const Vtx birds_seg5_vertex_050004C8[] = {
#include "assets/rom/generated/6f0659992e8b968cdc11.inc.c"





};

// 0x05000528 - 0x05000598
const Gfx birds_seg5_dl_05000528[] = {
    gsSPLightColor(LIGHT_1, 0x1d91b0ff),
    gsSPLightColor(LIGHT_2, 0x7242cff),
    gsSPVertex(birds_seg5_vertex_05000048, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 4,  3,  2, 0x0,  5,  3,  6, 0x0),
    gsSP2Triangles( 4,  7,  8, 0x0,  4,  8,  6, 0x0),
    gsSP2Triangles( 6,  8,  5, 0x0,  6,  3,  4, 0x0),
    gsSP2Triangles( 2,  1,  7, 0x0,  2,  7,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x05000598 - 0x05000600
const Gfx birds_seg5_dl_05000598[] = {
    gsSPLightColor(LIGHT_1, 0x1d91b0ff),
    gsSPLightColor(LIGHT_2, 0x7242cff),
    gsSPVertex(birds_seg5_vertex_050000D8, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0x1d91b0ff),
    gsSPLightColor(LIGHT_2, 0x7242cff),
    gsSPVertex(birds_seg5_vertex_05000108, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0x1d91b0ff),
    gsSPLightColor(LIGHT_2, 0x7242cff),
    gsSPVertex(birds_seg5_vertex_05000138, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x05000600 - 0x05000670
const Gfx birds_seg5_dl_05000600[] = {
    gsSPLightColor(LIGHT_1, 0x1d91b0ff),
    gsSPLightColor(LIGHT_2, 0x7242cff),
    gsSPVertex(birds_seg5_vertex_05000168, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  5,  3,  6, 0x0),
    gsSP2Triangles( 5,  7,  8, 0x0,  5,  8,  4, 0x0),
    gsSP2Triangles( 6,  7,  5, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  8,  1, 0x0,  4,  1,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x05000670 - 0x050007E0
const Gfx birds_seg5_dl_05000670[] = {
    gsSPLightColor(LIGHT_1, 0x1d91b0ff),
    gsSPLightColor(LIGHT_2, 0x7242cff),
    gsSPVertex(birds_seg5_vertex_050001F8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  1,  9, 10, 0x0),
    gsSP2Triangles( 1,  0, 11, 0x0, 12,  3, 13, 0x0),
    gsSP2Triangles( 8,  5,  4, 0x0,  5, 14, 10, 0x0),
    gsSP2Triangles(13, 15, 12, 0x0, 11, 15,  1, 0x0),
    gsSPVertex(birds_seg5_vertex_050002F8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  7,  4, 0x0),
    gsSP2Triangles( 8,  5,  9, 0x0, 10,  7, 11, 0x0),
    gsSP2Triangles(11, 12, 10, 0x0, 11, 13,  2, 0x0),
    gsSP2Triangles( 2, 14, 11, 0x0, 11,  7,  3, 0x0),
    gsSP1Triangle( 7,  6, 15, 0x0),
    gsSPVertex(birds_seg5_vertex_050003F8, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(birds_seg5_vertex_05000428, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  0,  7, 0x0,  7,  8,  6, 0x0),
    gsSP2Triangles( 9,  7,  2, 0x0,  2,  7,  0, 0x0),
    gsSP2Triangles( 2,  3,  9, 0x0,  7,  9,  8, 0x0),
    gsSP1Triangle( 5,  4,  1, 0x0),
    gsSPLightColor(LIGHT_1, 0xce9d34ff),
    gsSPLightColor(LIGHT_2, 0x33270dff),
    gsSPVertex(birds_seg5_vertex_050004C8, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP1Triangle( 1,  3,  5, 0x0),
    gsSPEndDisplayList(),
};
