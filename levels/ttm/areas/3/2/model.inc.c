// 0x07024AB0 - 0x07024B90
static const Vtx ttm_seg7_vertex_07024AB0[] = {
#include "assets/rom/generated/63a3716e650194117765.inc.c"













};

// 0x07024B90 - 0x07024C00
static const Vtx ttm_seg7_vertex_07024B90[] = {
#include "assets/rom/generated/be433929a5069129d314.inc.c"






};

// 0x07024C00 - 0x07024C78
static const Gfx ttm_seg7_dl_07024C00[] = {
    gsSPVertex(ttm_seg7_vertex_07024AB0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  5,  3, 0x0,  2,  6,  0, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 11, 12, 0x0,  9, 13,  7, 0x0),
    gsSPVertex(ttm_seg7_vertex_07024B90, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  5,  1, 0x0,  4,  6,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x07024C78 - 0x07024CA8
const Gfx ttm_seg7_dl_07024C78[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsSPDisplayList(ttm_seg7_dl_07024C00),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
