// 0x07008CB8 - 0x07008D98
static const Vtx ddd_seg7_vertex_07008CB8[] = {
#include "assets/rom/generated/4688a7b97d82a85719dd.inc.c"













};

// 0x07008D98 - 0x07008E78
static const Vtx ddd_seg7_vertex_07008D98[] = {
#include "assets/rom/generated/8c723486f788da203a77.inc.c"













};

// 0x07008E78 - 0x07008EE8
static const Vtx ddd_seg7_vertex_07008E78[] = {
#include "assets/rom/generated/161733885fb9572a9dcc.inc.c"






};

// 0x07008EE8 - 0x07008F80
static const Gfx ddd_seg7_dl_07008EE8[] = {
    gsSPVertex(ddd_seg7_vertex_07008CB8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(ddd_seg7_vertex_07008D98, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(ddd_seg7_vertex_07008E78, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  2,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008F80 - 0x07008FD0
const Gfx ddd_seg7_dl_07008F80[] = {
    gsDPPipeSync(),
    gsDPSetEnvColor(255, 255, 255, 98),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsSPDisplayList(ddd_seg7_dl_07008EE8),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};
