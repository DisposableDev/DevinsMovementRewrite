// 0x070056C0 - 0x070056D8

// 0x070056D8 - 0x070057C8
static const Vtx wf_seg7_vertex_070056D8[] = {
#include "assets/rom/generated/74a52d8c2a25daacd37d.inc.c"














};

// 0x070057C8 - 0x070058B8
static const Vtx wf_seg7_vertex_070057C8[] = {
#include "assets/rom/generated/5b391ce24ff70c3b3ebc.inc.c"














};

// 0x070058B8 - 0x070059A8
static const Vtx wf_seg7_vertex_070058B8[] = {
#include "assets/rom/generated/1eeebbd972d9531c3ac7.inc.c"














};

// 0x070059A8 - 0x07005A98
static const Vtx wf_seg7_vertex_070059A8[] = {
#include "assets/rom/generated/ba6886f25916dcf34d3b.inc.c"














};

// 0x07005A98 - 0x07005B88
static const Vtx wf_seg7_vertex_07005A98[] = {
#include "assets/rom/generated/09ede820da9d93e7d9a4.inc.c"














};

// 0x07005B88 - 0x07005C38
static const Vtx wf_seg7_vertex_07005B88[] = {
#include "assets/rom/generated/6866c89dcc376466ac5d.inc.c"










};

// 0x07005C38 - 0x07005DD8
static const Gfx wf_seg7_dl_07005C38[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, grass_09009800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(wf_seg7_vertex_070056D8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  9,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(13, 12, 14, 0x0),
    gsSPVertex(wf_seg7_vertex_070057C8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP1Triangle(14,  1,  0, 0x0),
    gsSPVertex(wf_seg7_vertex_070058B8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(13, 12, 14, 0x0),
    gsSPVertex(wf_seg7_vertex_070059A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP1Triangle(14,  1,  0, 0x0),
    gsSPVertex(wf_seg7_vertex_07005A98, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  4,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  9,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(14, 13, 12, 0x0),
    gsSPVertex(wf_seg7_vertex_07005B88, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP1Triangle(10,  1,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x07005DD8 - 0x07005E48
const Gfx wf_seg7_dl_07005DD8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(wf_seg7_dl_07005C38),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
