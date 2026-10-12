// 0x07005DC8 - 0x07005EB8
static const Vtx totwc_seg7_vertex_07005DC8[] = {
#include "assets/rom/generated/0d69e12cf1ae1e534b1e.inc.c"














};

// 0x07005EB8 - 0x07005FA8
static const Vtx totwc_seg7_vertex_07005EB8[] = {
#include "assets/rom/generated/d0c605965961c7f20292.inc.c"














};

// 0x07005FA8 - 0x07006098
static const Vtx totwc_seg7_vertex_07005FA8[] = {
#include "assets/rom/generated/ba0de1cb54a0e63ea5a3.inc.c"














};

// 0x07006098 - 0x07006188
static const Vtx totwc_seg7_vertex_07006098[] = {
#include "assets/rom/generated/c7646541ec86243f7789.inc.c"














};

// 0x07006188 - 0x07006278
static const Vtx totwc_seg7_vertex_07006188[] = {
#include "assets/rom/generated/0467251bc5ab1c6eb299.inc.c"














};

// 0x07006278 - 0x07006368
static const Vtx totwc_seg7_vertex_07006278[] = {
#include "assets/rom/generated/a10daf139920c74eb246.inc.c"














};

// 0x07006368 - 0x07006458
static const Vtx totwc_seg7_vertex_07006368[] = {
#include "assets/rom/generated/79a7a87b0054cb31238c.inc.c"














};

// 0x07006458 - 0x07006548
static const Vtx totwc_seg7_vertex_07006458[] = {
#include "assets/rom/generated/ee5f4f1d29abfc3d6239.inc.c"














};

// 0x07006548 - 0x07006638
static const Vtx totwc_seg7_vertex_07006548[] = {
#include "assets/rom/generated/ad35c687718a1986dece.inc.c"














};

// 0x07006638 - 0x07006728
static const Vtx totwc_seg7_vertex_07006638[] = {
#include "assets/rom/generated/08b960c0d2855f708153.inc.c"














};

// 0x07006728 - 0x07006818
static const Vtx totwc_seg7_vertex_07006728[] = {
#include "assets/rom/generated/f3c849f897bbdd66145b.inc.c"














};

// 0x07006818 - 0x07006908
static const Vtx totwc_seg7_vertex_07006818[] = {
#include "assets/rom/generated/7e8aaabf33219c1f9f33.inc.c"














};

// 0x07006908 - 0x070069F8
static const Vtx totwc_seg7_vertex_07006908[] = {
#include "assets/rom/generated/1eb7ca98dd1168612d0e.inc.c"














};

// 0x070069F8 - 0x07006AE8
static const Vtx totwc_seg7_vertex_070069F8[] = {
#include "assets/rom/generated/08b9c64e446b3aebb8b9.inc.c"














};

// 0x07006AE8 - 0x07006BD8
static const Vtx totwc_seg7_vertex_07006AE8[] = {
#include "assets/rom/generated/b038eb407231d2b11fc7.inc.c"














};

// 0x07006BD8 - 0x07006C48
static const Vtx totwc_seg7_vertex_07006BD8[] = {
#include "assets/rom/generated/b4ce632fcc94330684c9.inc.c"






};

// 0x07006C48 - 0x07007048
static const Gfx totwc_seg7_dl_07006C48[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09005800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(totwc_seg7_vertex_07005DC8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(totwc_seg7_vertex_07005EB8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(totwc_seg7_vertex_07005FA8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(totwc_seg7_vertex_07006098, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(totwc_seg7_vertex_07006188, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(totwc_seg7_vertex_07006278, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(totwc_seg7_vertex_07006368, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(totwc_seg7_vertex_07006458, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(totwc_seg7_vertex_07006548, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(totwc_seg7_vertex_07006638, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(totwc_seg7_vertex_07006728, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(totwc_seg7_vertex_07006818, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(totwc_seg7_vertex_07006908, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(totwc_seg7_vertex_070069F8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(totwc_seg7_vertex_07006AE8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(totwc_seg7_vertex_07006BD8, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 0,  2,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x07007048 - 0x070070C8
const Gfx totwc_seg7_dl_07007048[] = {
    gsDPPipeSync(),
    gsDPSetEnvColor(255, 255, 255, 150),
    gsDPSetCombineMode(G_CC_MODULATERGBFADEA, G_CC_MODULATERGBFADEA),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(totwc_seg7_dl_07006C48),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};
