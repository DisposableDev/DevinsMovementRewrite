// 0x070127E8 - 0x070128D8
static const Vtx rr_seg7_vertex_070127E8[] = {
#include "assets/rom/generated/90d385f8f70d39690085.inc.c"














};

// 0x070128D8 - 0x070129B8
static const Vtx rr_seg7_vertex_070128D8[] = {
#include "assets/rom/generated/9faf164d97200e74174b.inc.c"













};

// 0x070129B8 - 0x07012AA8
static const Vtx rr_seg7_vertex_070129B8[] = {
#include "assets/rom/generated/42e6b2b19941092cd16a.inc.c"














};

// 0x07012AA8 - 0x07012B98
static const Vtx rr_seg7_vertex_07012AA8[] = {
#include "assets/rom/generated/541df00532f748334f64.inc.c"














};

// 0x07012B98 - 0x07012C98
static const Vtx rr_seg7_vertex_07012B98[] = {
#include "assets/rom/generated/a60068eb3bccd31cea3f.inc.c"















};

// 0x07012C98 - 0x07012D78
static const Vtx rr_seg7_vertex_07012C98[] = {
#include "assets/rom/generated/e0a863cc0f0b412c0e63.inc.c"













};

// 0x07012D78 - 0x07012E68
static const Vtx rr_seg7_vertex_07012D78[] = {
#include "assets/rom/generated/084f03b5b287276bf7ac.inc.c"














};

// 0x07012E68 - 0x07012F58
static const Vtx rr_seg7_vertex_07012E68[] = {
#include "assets/rom/generated/5b10425d75b78a2e4550.inc.c"














};

// 0x07012F58 - 0x07013038
static const Vtx rr_seg7_vertex_07012F58[] = {
#include "assets/rom/generated/45978e99ac91829dc691.inc.c"













};

// 0x07013038 - 0x07013128
static const Vtx rr_seg7_vertex_07013038[] = {
#include "assets/rom/generated/c4f9fed5b5bfa0d51304.inc.c"














};

// 0x07013128 - 0x07013208
static const Vtx rr_seg7_vertex_07013128[] = {
#include "assets/rom/generated/1243fc32cccf9868f098.inc.c"













};

// 0x07013208 - 0x070132F8
static const Vtx rr_seg7_vertex_07013208[] = {
#include "assets/rom/generated/b7bc546e773f09f0907a.inc.c"














};

// 0x070132F8 - 0x070133E8
static const Vtx rr_seg7_vertex_070132F8[] = {
#include "assets/rom/generated/acde1dce607af7449f62.inc.c"














};

// 0x070133E8 - 0x070134C8
static const Vtx rr_seg7_vertex_070133E8[] = {
#include "assets/rom/generated/85b9a947551777ccfc35.inc.c"













};

// 0x070134C8 - 0x070135C8
static const Vtx rr_seg7_vertex_070134C8[] = {
#include "assets/rom/generated/d3d515b06d50739adb77.inc.c"















};

// 0x070135C8 - 0x070136A8
static const Vtx rr_seg7_vertex_070135C8[] = {
#include "assets/rom/generated/27d9a1db87146cd0131c.inc.c"













};

// 0x070136A8 - 0x07013728
static const Vtx rr_seg7_vertex_070136A8[] = {
#include "assets/rom/generated/976e31b60a0ea474cbd7.inc.c"







};

// 0x07013728 - 0x07013828
static const Vtx rr_seg7_vertex_07013728[] = {
#include "assets/rom/generated/6a8c710436a3716c4f24.inc.c"















};

// 0x07013828 - 0x070138A8
static const Vtx rr_seg7_vertex_07013828[] = {
#include "assets/rom/generated/b0c0583aacd5fd825bac.inc.c"







};

// 0x070138A8 - 0x07013998
static const Vtx rr_seg7_vertex_070138A8[] = {
#include "assets/rom/generated/760483444d687b6c51b8.inc.c"














};

// 0x07013998 - 0x07013A88
static const Vtx rr_seg7_vertex_07013998[] = {
#include "assets/rom/generated/02d4ac53a725875dd318.inc.c"














};

// 0x07013A88 - 0x07013B88
static const Vtx rr_seg7_vertex_07013A88[] = {
#include "assets/rom/generated/076f0c47ca3b3a881294.inc.c"















};

// 0x07013B88 - 0x07013C88
static const Vtx rr_seg7_vertex_07013B88[] = {
#include "assets/rom/generated/28d7ffd696ab58635dcb.inc.c"















};

// 0x07013C88 - 0x07013D78
static const Vtx rr_seg7_vertex_07013C88[] = {
#include "assets/rom/generated/ee8f011be8a8ce1e8ce9.inc.c"





};

// 0x07013E68 - 0x070142C0
static const Gfx rr_seg7_dl_07013E68[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_070127E8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(rr_seg7_vertex_070128D8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(rr_seg7_vertex_070129B8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(12, 14, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07012AA8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07012B98, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10,  2, 11, 0x0),
    gsSP2Triangles( 0,  2, 10, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_07012C98, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07012D78, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07012E68, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07012F58, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07013038, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07013128, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07013208, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(rr_seg7_vertex_070132F8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_070133E8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12,  0, 0x0),
    gsSP2Triangles(11,  0,  2, 0x0,  0, 13,  1, 0x0),
    gsSPVertex(rr_seg7_vertex_070134C8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_070135C8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  4,  0, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 9, 10,  7, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles( 7, 12, 13, 0x0,  7, 13,  8, 0x0),
    gsSPVertex(rr_seg7_vertex_070136A8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x070142C0 - 0x07014350
static const Gfx rr_seg7_dl_070142C0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09000800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_07013728, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_07013828, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x07014350 - 0x07014490
static const Gfx rr_seg7_dl_07014350[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09008000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_070138A8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 0,  4,  3, 0x0,  0,  2,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles( 6,  9, 10, 0x0,  6, 11,  7, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07013998, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07013A88, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0, 15,  1, 0x0),
    gsSPVertex(rr_seg7_vertex_07013B88, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0, 15,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x07014490 - 0x07014508
static const Gfx rr_seg7_dl_07014490[] = {
    gsSPVertex(rr_seg7_vertex_07013C88, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 0,  4,  3, 0x0,  0,  5,  4, 0x0),
    gsSPEndDisplayList(),
};

// 0x07014508 - 0x07014590
const Gfx rr_seg7_dl_07014508[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(rr_seg7_dl_07013E68),
    gsSPDisplayList(rr_seg7_dl_070142C0),
    gsSPDisplayList(rr_seg7_dl_07014350),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(rr_seg7_dl_07014490),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
