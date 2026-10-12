// Transparent Star

// 0x0302C488

// 0x0302C4A0
static const Vtx transparent_star_seg3_vertex_body[] = {
#include "assets/rom/generated/bdbdd409bd979c2e4ec9.inc.c"











};

// 0x0302C560 - 0x0302C620
const Gfx transparent_star_seg3_sub_dl_body[] = {
    gsSPLightColor(LIGHT_1, 0x1e32e6ff),
    gsSPLightColor(LIGHT_2, 0x30517ff),
    gsSPVertex(transparent_star_seg3_vertex_body, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 5,  3,  0, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 6,  7,  4, 0x0,  7,  2,  4, 0x0),
    gsSP2Triangles( 8,  6,  4, 0x0,  9,  4, 10, 0x0),
    gsSP2Triangles( 9, 11,  4, 0x0,  4,  5, 10, 0x0),
    gsSP2Triangles(11,  8,  4, 0x0,  0,  2,  7, 0x0),
    gsSP2Triangles( 0,  7,  6, 0x0,  0,  6,  8, 0x0),
    gsSP2Triangles( 0,  8, 11, 0x0,  0, 11,  9, 0x0),
    gsSP2Triangles(10,  5,  0, 0x0, 10,  0,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0302C620 - 0x0302C658
const Gfx transparent_star_seg3_dl_body[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsDPSetEnvColor(255, 255, 255, 120),
    gsSPDisplayList(transparent_star_seg3_sub_dl_body),
    gsDPPipeSync(),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
