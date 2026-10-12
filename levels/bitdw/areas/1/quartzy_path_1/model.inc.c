// 0x07003688 - 0x07003778
static const Vtx bitdw_seg7_vertex_07003688[] = {
#include "assets/rom/generated/2be2e782e6c065efb75d.inc.c"














};

// 0x07003778 - 0x07003868
static const Vtx bitdw_seg7_vertex_07003778[] = {
#include "assets/rom/generated/0d6ad43c196f8990fe00.inc.c"














};

// 0x07003868 - 0x07003968
static const Vtx bitdw_seg7_vertex_07003868[] = {
#include "assets/rom/generated/463d0100c901d886576f.inc.c"















};

// 0x07003968 - 0x07003A68
static const Vtx bitdw_seg7_vertex_07003968[] = {
#include "assets/rom/generated/1c1784a7fdcb15d70f05.inc.c"















};

// 0x07003A68 - 0x07003A98
static const Vtx bitdw_seg7_vertex_07003A68[] = {
#include "assets/rom/generated/0dbda24c70e28e460f4d.inc.c"


};

// 0x07003A98 - 0x07003BF0
static const Gfx bitdw_seg7_dl_07003A98[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, bitdw_seg7_texture_07000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bitdw_seg7_vertex_07003688, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07003778, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07003868, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(14, 11, 13, 0x0, 11, 15, 12, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07003968, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  4,  6,  5, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  8, 10,  9, 0x0),
    gsSP2Triangles(11,  7,  9, 0x0, 11,  9, 12, 0x0),
    gsSP2Triangles(13, 14, 11, 0x0, 12, 13, 11, 0x0),
    gsSP1Triangle(13, 15, 14, 0x0),
    gsSPVertex(bitdw_seg7_vertex_07003A68, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x07003BF0 - 0x07003C60
const Gfx bitdw_seg7_dl_07003BF0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bitdw_seg7_dl_07003A98),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
