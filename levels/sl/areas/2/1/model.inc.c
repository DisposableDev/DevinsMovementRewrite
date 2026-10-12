// 0x0700A9B0 - 0x0700A9C8

// 0x0700A9C8 - 0x0700A9E0

// 0x0700A9E0 - 0x0700AAE0
static const Vtx sl_seg7_vertex_0700A9E0[] = {
#include "assets/rom/generated/ac23f015a2c1a9cbfa4b.inc.c"















};

// 0x0700AAE0 - 0x0700ABE0
static const Vtx sl_seg7_vertex_0700AAE0[] = {
#include "assets/rom/generated/c9f924d3e9cb91c9c037.inc.c"















};

// 0x0700ABE0 - 0x0700ACC0
static const Vtx sl_seg7_vertex_0700ABE0[] = {
#include "assets/rom/generated/c07d72f2ba17a56ed45c.inc.c"













};

// 0x0700ACC0 - 0x0700ADB0
static const Vtx sl_seg7_vertex_0700ACC0[] = {
#include "assets/rom/generated/c00f4976dc0d05a6f3d9.inc.c"














};

// 0x0700ADB0 - 0x0700AEA0
static const Vtx sl_seg7_vertex_0700ADB0[] = {
#include "assets/rom/generated/5deabafc3d48bbebf53b.inc.c"














};

// 0x0700AEA0 - 0x0700AFA0
static const Vtx sl_seg7_vertex_0700AEA0[] = {
#include "assets/rom/generated/0a121a72356eaa15706e.inc.c"















};

// 0x0700AFA0 - 0x0700B080
static const Vtx sl_seg7_vertex_0700AFA0[] = {
#include "assets/rom/generated/e12a3c3a0cdd7b74c9be.inc.c"













};

// 0x0700B080 - 0x0700B170
static const Vtx sl_seg7_vertex_0700B080[] = {
#include "assets/rom/generated/256510e08985a4524fd9.inc.c"














};

// 0x0700B170 - 0x0700B250
static const Vtx sl_seg7_vertex_0700B170[] = {
#include "assets/rom/generated/6c1496ec2ce108ef2d89.inc.c"













};

// 0x0700B250 - 0x0700B330
static const Vtx sl_seg7_vertex_0700B250[] = {
#include "assets/rom/generated/467034b834bbcfa049cf.inc.c"













};

// 0x0700B330 - 0x0700B420
static const Vtx sl_seg7_vertex_0700B330[] = {
#include "assets/rom/generated/262d85fdd2e56ce993d9.inc.c"














};

// 0x0700B420 - 0x0700B510
static const Vtx sl_seg7_vertex_0700B420[] = {
#include "assets/rom/generated/e59b347467d726d55b7b.inc.c"














};

// 0x0700B510 - 0x0700B5D0
static const Vtx sl_seg7_vertex_0700B510[] = {
#include "assets/rom/generated/142f445dc1499d3fb184.inc.c"











};

// 0x0700B5D0 - 0x0700B620
static const Vtx sl_seg7_vertex_0700B5D0[] = {
#include "assets/rom/generated/e767060afb9e2102ec8e.inc.c"




};

// 0x0700B620 - 0x0700B6C0
static const Vtx sl_seg7_vertex_0700B620[] = {
#include "assets/rom/generated/1ca06e3ca9969b0c99bb.inc.c"









};

// 0x0700B6C0 - 0x0700BA70
static const Gfx sl_seg7_dl_0700B6C0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09008800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(sl_seg7_vertex_0700A9E0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0, 15,  1, 0x0),
    gsSPVertex(sl_seg7_vertex_0700AAE0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  2,  4, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(sl_seg7_vertex_0700ABE0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(sl_seg7_vertex_0700ACC0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(sl_seg7_vertex_0700ADB0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(sl_seg7_vertex_0700AEA0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11,  6,  3, 0x0),
    gsSP2Triangles(11, 12,  6, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(sl_seg7_vertex_0700AFA0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(sl_seg7_vertex_0700B080, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(sl_seg7_vertex_0700B170, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9, 12, 10, 0x0),
    gsSP1Triangle( 0, 13,  1, 0x0),
    gsSPVertex(sl_seg7_vertex_0700B250, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(sl_seg7_vertex_0700B330, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  4,  7,  6, 0x0),
    gsSP2Triangles( 3,  5,  8, 0x0,  3,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(sl_seg7_vertex_0700B420, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(sl_seg7_vertex_0700B510, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  7, 0x0,  3,  7,  8, 0x0),
    gsSP2Triangles( 0,  9, 10, 0x0,  0, 11,  9, 0x0),
    gsSP1Triangle( 0,  2, 11, 0x0),
    gsSPLightColor(LIGHT_1, 0xff),
    gsSPLightColor(LIGHT_2, 0xff),
    gsSPVertex(sl_seg7_vertex_0700B5D0, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP1Triangle( 0,  4,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700BA70 - 0x0700BAE8
static const Gfx sl_seg7_dl_0700BA70[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, snow_09001000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(sl_seg7_vertex_0700B620, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  2,  1, 0x0,  4,  1,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles( 8,  3,  0, 0x0,  8,  7,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700BAE8 - 0x0700BB68
const Gfx sl_seg7_dl_0700BAE8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(sl_seg7_dl_0700B6C0),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(sl_seg7_dl_0700BA70),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
