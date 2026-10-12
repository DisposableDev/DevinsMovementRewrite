// 0x0700A910 - 0x0700A928

// 0x0700A928 - 0x0700A958
static const Vtx sl_seg7_vertex_0700A928[] = {
#include "assets/rom/generated/07498a2039e518f42bd6.inc.c"


};

// 0x0700A958 - 0x0700A980
static const Gfx sl_seg7_dl_0700A958[] = {
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(sl_seg7_vertex_0700A928, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700A980 - 0x0700A9B0
const Gfx sl_seg7_dl_0700A980[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPDisplayList(sl_seg7_dl_0700A958),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
