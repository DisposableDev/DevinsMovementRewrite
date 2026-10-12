// 0x07005568 - 0x07005658
static const Vtx wf_seg7_vertex_07005568[] = {
#include "assets/rom/generated/53d15287eb9ca9eea91a.inc.c"














};

// 0x07005658 - 0x07005690
static const Gfx wf_seg7_dl_07005658[] = {
    gsSPVertex(wf_seg7_vertex_07005568, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPEndDisplayList(),
};

// 0x07005690 - 0x070056C0
const Gfx wf_seg7_dl_07005690[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsSPDisplayList(wf_seg7_dl_07005658),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
