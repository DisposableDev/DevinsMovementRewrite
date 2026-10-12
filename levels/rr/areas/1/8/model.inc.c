// 0x070082C8 - 0x070083C8
static const Vtx rr_seg7_vertex_070082C8[] = {
#include "assets/rom/generated/2ae4ddfd428e3a07219f.inc.c"















};

// 0x070083C8 - 0x070084C8
static const Vtx rr_seg7_vertex_070083C8[] = {
#include "assets/rom/generated/25e62d337531df071874.inc.c"















};

// 0x070084C8 - 0x070085B8
static const Vtx rr_seg7_vertex_070084C8[] = {
#include "assets/rom/generated/5e1236e3aab69048a9c7.inc.c"














};

// 0x070085B8 - 0x070086B8
static const Vtx rr_seg7_vertex_070085B8[] = {
#include "assets/rom/generated/732af713b17479a54f67.inc.c"















};

// 0x070086B8 - 0x07008798
static const Vtx rr_seg7_vertex_070086B8[] = {
#include "assets/rom/generated/d4abce8db2e350639643.inc.c"













};

// 0x07008798 - 0x07008858
static const Vtx rr_seg7_vertex_07008798[] = {
#include "assets/rom/generated/65ff87ffc241f37ff635.inc.c"











};

// 0x07008858 - 0x07008958
static const Vtx rr_seg7_vertex_07008858[] = {
#include "assets/rom/generated/5f82ec1b23464ed9930b.inc.c"















};

// 0x07008958 - 0x07008A58
static const Vtx rr_seg7_vertex_07008958[] = {
#include "assets/rom/generated/c875c1dfecd49a9aa507.inc.c"















};

// 0x07008A58 - 0x07008B58
static const Vtx rr_seg7_vertex_07008A58[] = {
#include "assets/rom/generated/34bffcc96986572e5e28.inc.c"















};

// 0x07008B58 - 0x07008D28
static const Gfx rr_seg7_dl_07008B58[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_070082C8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0,  2, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_070083C8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11, 12, 0x0),
    gsSP2Triangles( 8, 13, 14, 0x0,  8, 15, 11, 0x0),
    gsSP2Triangles( 8, 10, 13, 0x0,  8, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_070084C8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_070085B8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 0,  8,  6, 0x0,  0,  9,  8, 0x0),
    gsSP2Triangles(10, 11,  3, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle(13, 15, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_070086B8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 3,  4,  2, 0x0,  5,  0,  2, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(rr_seg7_vertex_07008798, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008D28 - 0x07008E20
static const Gfx rr_seg7_dl_07008D28[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_07008858, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_07008958, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_07008A58, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x07008E20 - 0x07008E98
const Gfx rr_seg7_dl_07008E20[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(rr_seg7_dl_07008B58),
    gsSPDisplayList(rr_seg7_dl_07008D28),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
