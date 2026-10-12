// Test Platform (small tiny platform 1/4th the size of a box. Probably used in early modeling tests?)

// 0x08026008

// 0x08026020
static const Vtx unknown_seg8_vertex_08026020[] = {
#include "assets/rom/generated/8d0276b10313a1a15882.inc.c"














};

// 0x08026110
static const Vtx unknown_seg8_vertex_08026110[] = {
#include "assets/rom/generated/9e293c7a195a633f25c1.inc.c"














};

// 0x08026200
static const Vtx unknown_seg8_vertex_08026200[] = {
#include "assets/rom/generated/a004bccb127411e680c6.inc.c"





};

// 0x08026260 - 0x080262F8
const Gfx unknown_seg8_dl_08026260[] = {
    gsSPLightColor(LIGHT_1, 0xc8c81eff),
    gsSPLightColor(LIGHT_2, 0x323207ff),
    gsSPVertex(unknown_seg8_vertex_08026020, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(unknown_seg8_vertex_08026110, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(unknown_seg8_vertex_08026200, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};
