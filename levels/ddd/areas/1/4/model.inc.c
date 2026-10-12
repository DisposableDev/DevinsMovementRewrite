// 0x07005CB0 - 0x07005D90
static const Vtx ddd_seg7_vertex_07005CB0[] = {
#include "assets/rom/generated/1a4c1e7912c50d993a26.inc.c"













};

// 0x07005D90 - 0x07005E70
static const Vtx ddd_seg7_vertex_07005D90[] = {
#include "assets/rom/generated/7bb3db5da45de50cb761.inc.c"













};

// 0x07005E70 - 0x07005EE0
static const Vtx ddd_seg7_vertex_07005E70[] = {
#include "assets/rom/generated/ebda19f77f9be17799e0.inc.c"






};

// 0x07005EE0 - 0x07005F78
static const Gfx ddd_seg7_dl_07005EE0[] = {
    gsSPVertex(ddd_seg7_vertex_07005CB0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(ddd_seg7_vertex_07005D90, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(ddd_seg7_vertex_07005E70, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  2,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x07005F78 - 0x07005FC8
const Gfx ddd_seg7_dl_07005F78[] = {
    gsDPPipeSync(),
    gsDPSetEnvColor(255, 255, 255, 98),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(ddd_seg7_dl_07005EE0),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};
