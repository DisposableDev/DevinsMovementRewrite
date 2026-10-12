// 0x0700A8D0 - 0x0700A9C0
static const Vtx castle_grounds_seg7_vertex_0700A8D0[] = {
#include "assets/rom/generated/b690a5db16cb6d3eb8a8.inc.c"














};

// 0x0700A9C0 - 0x0700AAB0
static const Vtx castle_grounds_seg7_vertex_0700A9C0[] = {
#include "assets/rom/generated/76abf34d5b8d60abd7b0.inc.c"














};

// 0x0700AAB0 - 0x0700ABA0
static const Vtx castle_grounds_seg7_vertex_0700AAB0[] = {
#include "assets/rom/generated/efc7148e59ea841176df.inc.c"














};

// 0x0700ABA0 - 0x0700AC90
static const Vtx castle_grounds_seg7_vertex_0700ABA0[] = {
#include "assets/rom/generated/27b42f2727fbd34b9560.inc.c"














};

// 0x0700AC90 - 0x0700AD80
static const Vtx castle_grounds_seg7_vertex_0700AC90[] = {
#include "assets/rom/generated/69ae014baa55a44abd72.inc.c"














};

// 0x0700AD80 - 0x0700AE70
static const Vtx castle_grounds_seg7_vertex_0700AD80[] = {
#include "assets/rom/generated/0ef165ad59ee34a1fc49.inc.c"














};

// 0x0700AE70 - 0x0700AF60
static const Vtx castle_grounds_seg7_vertex_0700AE70[] = {
#include "assets/rom/generated/cd5c0bdf15d1eac10f58.inc.c"














};

// 0x0700AF60 - 0x0700AFD0
static const Vtx castle_grounds_seg7_vertex_0700AF60[] = {
#include "assets/rom/generated/048ba2f9a2c5d1d77af2.inc.c"






};

// 0x0700AFD0 - 0x0700B1D0
static const Gfx castle_grounds_seg7_dl_0700AFD0[] = {
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, outside_0900BC00),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(castle_grounds_seg7_vertex_0700A8D0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(castle_grounds_seg7_vertex_0700A9C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(castle_grounds_seg7_vertex_0700AAB0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(castle_grounds_seg7_vertex_0700ABA0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(castle_grounds_seg7_vertex_0700AC90, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(castle_grounds_seg7_vertex_0700AD80, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(castle_grounds_seg7_vertex_0700AE70, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(castle_grounds_seg7_vertex_0700AF60, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  6,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700B1D0 - 0x0700B240
const Gfx castle_grounds_seg7_dl_0700B1D0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(castle_grounds_seg7_dl_0700AFD0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
