// 0x07051A38 - 0x07051A50

// 0x07051A50 - 0x07051B10
static const Vtx inside_castle_seg7_vertex_07051A50[] = {
#include "assets/rom/generated/179ab009b4c9b0bcff91.inc.c"











};

// 0x07051B10 - 0x07051B60
static const Gfx inside_castle_seg7_dl_07051B10[] = {
    gsSPLightColor(LIGHT_1, 0xbfaa3fff),
    gsSPLightColor(LIGHT_2, 0x473f17ff),
    gsSPVertex(inside_castle_seg7_vertex_07051A50, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x07051B60 - 0x07051B88
const Gfx inside_castle_seg7_dl_07051B60[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPDisplayList(inside_castle_seg7_dl_07051B10),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
