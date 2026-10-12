// 0x07057FD8 - 0x07057FF0

// 0x07057FF0 - 0x07058008

// 0x07058008 - 0x07058108
static const Vtx inside_castle_seg7_vertex_07058008[] = {
#include "assets/rom/generated/567acb158c8fb19c704b.inc.c"















};

// 0x07058108 - 0x070581F8
static const Vtx inside_castle_seg7_vertex_07058108[] = {
#include "assets/rom/generated/03e8da5efec89e9dd98c.inc.c"














};

// 0x070581F8 - 0x070582F8
static const Vtx inside_castle_seg7_vertex_070581F8[] = {
#include "assets/rom/generated/dbb50a54f28b3b3855df.inc.c"















};

// 0x070582F8 - 0x070583E8
static const Vtx inside_castle_seg7_vertex_070582F8[] = {
#include "assets/rom/generated/fa0c5c449666e59cad3e.inc.c"














};

// 0x070583E8 - 0x070584E8
static const Vtx inside_castle_seg7_vertex_070583E8[] = {
#include "assets/rom/generated/b2c46739c6e5d281deb4.inc.c"















};

// 0x070584E8 - 0x070585A8
static const Vtx inside_castle_seg7_vertex_070584E8[] = {
#include "assets/rom/generated/7c6dc5cfe82b126a3e8f.inc.c"











};

// 0x070585A8 - 0x07058698
static const Vtx inside_castle_seg7_vertex_070585A8[] = {
#include "assets/rom/generated/3132a661f6f371b90cfa.inc.c"














};

// 0x07058698 - 0x07058718
static const Vtx inside_castle_seg7_vertex_07058698[] = {
#include "assets/rom/generated/5137f8858c861c64548e.inc.c"







};

// 0x07058718 - 0x07058950
static const Gfx inside_castle_seg7_dl_07058718[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, inside_09004800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffcc65ff),
    gsSPLightColor(LIGHT_2, 0x3f3319ff),
    gsSPVertex(inside_castle_seg7_vertex_07058008, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  4, 0x0,  6,  4,  3, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07058108, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070581F8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12,  1, 0x0),
    gsSP2Triangles(11,  1,  0, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070582F8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070583E8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_070584E8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 0, 10,  1, 0x0,  7, 11,  8, 0x0),
    gsSPLightColor(LIGHT_1, 0xff0000ff),
    gsSPLightColor(LIGHT_2, 0x3f0000ff),
    gsSPVertex(inside_castle_seg7_vertex_070585A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(inside_castle_seg7_vertex_07058698, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x07058950 - 0x070589C0
const Gfx inside_castle_seg7_dl_07058950[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(inside_castle_seg7_dl_07058718),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
