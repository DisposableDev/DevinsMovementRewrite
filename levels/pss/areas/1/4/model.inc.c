// 0x0700B4A8 - 0x0700B598
static const Vtx pss_seg7_vertex_0700B4A8[] = {
#include "assets/rom/generated/84855a94deee836d8920.inc.c"














};

// 0x0700B598 - 0x0700B688
static const Vtx pss_seg7_vertex_0700B598[] = {
#include "assets/rom/generated/54a51a94967162b638e0.inc.c"














};

// 0x0700B688 - 0x0700B778
static const Vtx pss_seg7_vertex_0700B688[] = {
#include "assets/rom/generated/4b5e370e7021a3c3c429.inc.c"














};

// 0x0700B778 - 0x0700B868
static const Vtx pss_seg7_vertex_0700B778[] = {
#include "assets/rom/generated/12a7b6d8a6475a0a96da.inc.c"














};

// 0x0700B868 - 0x0700B958
static const Vtx pss_seg7_vertex_0700B868[] = {
#include "assets/rom/generated/55b2f925d9f1e7c25210.inc.c"














};

// 0x0700B958 - 0x0700BA48
static const Vtx pss_seg7_vertex_0700B958[] = {
#include "assets/rom/generated/3bd42d2fd40d9d36e2b4.inc.c"














};

// 0x0700BA48 - 0x0700BB38
static const Vtx pss_seg7_vertex_0700BA48[] = {
#include "assets/rom/generated/acd0962d837428f1655a.inc.c"














};

// 0x0700BB38 - 0x0700BC28
static const Vtx pss_seg7_vertex_0700BB38[] = {
#include "assets/rom/generated/d240ac46291495fe9f18.inc.c"














};

// 0x0700BC28 - 0x0700BD18
static const Vtx pss_seg7_vertex_0700BC28[] = {
#include "assets/rom/generated/2989c72c9b615eb2b7c9.inc.c"














};

// 0x0700BD18 - 0x0700BDF8
static const Vtx pss_seg7_vertex_0700BD18[] = {
#include "assets/rom/generated/fec1a5f2fd6bc6eaa294.inc.c"













};

// 0x0700BDF8 - 0x0700BEE8
static const Vtx pss_seg7_vertex_0700BDF8[] = {
#include "assets/rom/generated/1bb8ab7058b698042bad.inc.c"














};

// 0x0700BEE8 - 0x0700BFD8
static const Vtx pss_seg7_vertex_0700BEE8[] = {
#include "assets/rom/generated/2bd50dffa7d232b1c36f.inc.c"














};

// 0x0700BFD8 - 0x0700C0C8
static const Vtx pss_seg7_vertex_0700BFD8[] = {
#include "assets/rom/generated/475a9623a342ac4d2489.inc.c"














};

// 0x0700C0C8 - 0x0700C1B8
static const Vtx pss_seg7_vertex_0700C0C8[] = {
#include "assets/rom/generated/577202a8c9eaaafa33a1.inc.c"














};

// 0x0700C1B8 - 0x0700C2A8
static const Vtx pss_seg7_vertex_0700C1B8[] = {
#include "assets/rom/generated/ce75fb78c30d22585f29.inc.c"














};

// 0x0700C2A8 - 0x0700C398
static const Vtx pss_seg7_vertex_0700C2A8[] = {
#include "assets/rom/generated/1d2994d94a16bee0d254.inc.c"














};

// 0x0700C398 - 0x0700C488
static const Vtx pss_seg7_vertex_0700C398[] = {
#include "assets/rom/generated/503bb200bdfb347fe652.inc.c"














};

// 0x0700C488 - 0x0700C578
static const Vtx pss_seg7_vertex_0700C488[] = {
#include "assets/rom/generated/f84cdbc3d802ac23da7c.inc.c"














};

// 0x0700C578 - 0x0700C668
static const Vtx pss_seg7_vertex_0700C578[] = {
#include "assets/rom/generated/0dcf47e2a368298de923.inc.c"














};

// 0x0700C668 - 0x0700C758
static const Vtx pss_seg7_vertex_0700C668[] = {
#include "assets/rom/generated/89e495af11bd0cc1b54c.inc.c"














};

// 0x0700C758 - 0x0700C848
static const Vtx pss_seg7_vertex_0700C758[] = {
#include "assets/rom/generated/dc34b86cd5b9a087a674.inc.c"














};

// 0x0700C848 - 0x0700C928
static const Vtx pss_seg7_vertex_0700C848[] = {
#include "assets/rom/generated/53a056a546431dafd475.inc.c"













};

// 0x0700C928 - 0x0700CA18
static const Vtx pss_seg7_vertex_0700C928[] = {
#include "assets/rom/generated/0573cb6ee1f17607e68e.inc.c"














};

// 0x0700CA18 - 0x0700CB08
static const Vtx pss_seg7_vertex_0700CA18[] = {
#include "assets/rom/generated/01bed6634b2ac3e29ef6.inc.c"














};

// 0x0700CB08 - 0x0700CBF8
static const Vtx pss_seg7_vertex_0700CB08[] = {
#include "assets/rom/generated/26e938155987aceda5f7.inc.c"














};

// 0x0700CBF8 - 0x0700CCA8
static const Vtx pss_seg7_vertex_0700CBF8[] = {
#include "assets/rom/generated/6b5ef11f714781b52fc6.inc.c"










};

// 0x0700CCA8 - 0x0700D338
static const Gfx pss_seg7_dl_0700CCA8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mountain_09007800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(pss_seg7_vertex_0700B4A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700B598, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700B688, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(pss_seg7_vertex_0700B778, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700B868, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(pss_seg7_vertex_0700B958, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700BA48, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(pss_seg7_vertex_0700BB38, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700BC28, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700BD18, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  8,  6, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9, 11, 12, 0x0),
    gsSP1Triangle( 0,  2, 13, 0x0),
    gsSPVertex(pss_seg7_vertex_0700BDF8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(pss_seg7_vertex_0700BEE8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700BFD8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(pss_seg7_vertex_0700C0C8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(pss_seg7_vertex_0700C1B8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(pss_seg7_vertex_0700C2A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700C398, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(pss_seg7_vertex_0700C488, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700C578, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700C668, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700C758, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(pss_seg7_vertex_0700C848, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  3,  1, 0x0),
    gsSPVertex(pss_seg7_vertex_0700C928, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700CA18, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(pss_seg7_vertex_0700CB08, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(pss_seg7_vertex_0700CBF8, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP1Triangle( 0,  2, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700D338 - 0x0700D3A8
const Gfx pss_seg7_dl_0700D338[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGB, G_CC_DECALRGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(pss_seg7_dl_0700CCA8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
