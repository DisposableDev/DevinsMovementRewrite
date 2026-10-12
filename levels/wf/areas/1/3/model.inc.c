// 0x07005328 - 0x07005418
static const Vtx wf_seg7_vertex_07005328[] = {
#include "assets/rom/generated/ec46343d7dcae9ef75e7.inc.c"














};

// 0x07005418 - 0x070054D8
static const Vtx wf_seg7_vertex_07005418[] = {
#include "assets/rom/generated/9abf834964ae6362d29c.inc.c"











};

// 0x070054D8 - 0x07005538
static const Gfx wf_seg7_dl_070054D8[] = {
    gsSPVertex(wf_seg7_vertex_07005328, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(wf_seg7_vertex_07005418, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x07005538 - 0x07005568
const Gfx wf_seg7_dl_07005538[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsSPDisplayList(wf_seg7_dl_070054D8),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
