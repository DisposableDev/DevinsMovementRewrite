// 0x07024CD8 - 0x07024DC8
static const Vtx lll_seg7_vertex_07024CD8[] = {
#include "assets/rom/generated/7bbad051955e1f48f125.inc.c"














};

// 0x07024DC8 - 0x07024EB8
static const Vtx lll_seg7_vertex_07024DC8[] = {
#include "assets/rom/generated/a7d25239de49c79cc192.inc.c"














};

// 0x07024EB8 - 0x07024FA8
static const Vtx lll_seg7_vertex_07024EB8[] = {
#include "assets/rom/generated/da081cf815594930794d.inc.c"














};

// 0x07024FA8 - 0x07025098
static const Vtx lll_seg7_vertex_07024FA8[] = {
#include "assets/rom/generated/c3399afaf795e542fe57.inc.c"














};

// 0x07025098 - 0x07025188
static const Vtx lll_seg7_vertex_07025098[] = {
#include "assets/rom/generated/275c551bcc234e4194ad.inc.c"














};

// 0x07025188 - 0x07025278
static const Vtx lll_seg7_vertex_07025188[] = {
#include "assets/rom/generated/26926e48e52d0d8ae843.inc.c"














};

// 0x07025278 - 0x07025378
static const Vtx lll_seg7_vertex_07025278[] = {
#include "assets/rom/generated/3a30238e255c12f5eb5f.inc.c"















};

// 0x07025378 - 0x070253D8
static const Vtx lll_seg7_vertex_07025378[] = {
#include "assets/rom/generated/29d0ffb64f01fe023908.inc.c"





};

// 0x070253D8 - 0x070255D8
static const Gfx lll_seg7_dl_070253D8[] = {
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, lll_seg7_texture_0700D200),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 16 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(lll_seg7_vertex_07024CD8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(lll_seg7_vertex_07024DC8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(lll_seg7_vertex_07024EB8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(lll_seg7_vertex_07024FA8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  6,  8, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(lll_seg7_vertex_07025098, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(lll_seg7_vertex_07025188, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(lll_seg7_vertex_07025278, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  0, 11,  1, 0x0),
    gsSP2Triangles(12, 11,  0, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(lll_seg7_vertex_07025378, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x070255D8 - 0x07025648
const Gfx lll_seg7_dl_070255D8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(lll_seg7_dl_070253D8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
