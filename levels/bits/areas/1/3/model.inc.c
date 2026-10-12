// 0x070036F8 - 0x070037F8
static const Vtx bits_seg7_vertex_070036F8[] = {
#include "assets/rom/generated/8cbb9f69d9689d2de0ff.inc.c"















};

// 0x070037F8 - 0x070038F8
static const Vtx bits_seg7_vertex_070037F8[] = {
#include "assets/rom/generated/feb5bbc93986d970073f.inc.c"















};

// 0x070038F8 - 0x070039D8
static const Vtx bits_seg7_vertex_070038F8[] = {
#include "assets/rom/generated/77face4e08e18cf44d08.inc.c"













};

// 0x070039D8 - 0x07003AC8
static const Vtx bits_seg7_vertex_070039D8[] = {
#include "assets/rom/generated/bc962c61ae24536c2ebb.inc.c"














};

// 0x07003AC8 - 0x07003BC8
static const Vtx bits_seg7_vertex_07003AC8[] = {
#include "assets/rom/generated/cee87c4584b26d9caa3a.inc.c"















};

// 0x07003BC8 - 0x07003C88
static const Vtx bits_seg7_vertex_07003BC8[] = {
#include "assets/rom/generated/5a57121e625513cdc1e1.inc.c"











};

// 0x07003C88 - 0x07003D88
static const Vtx bits_seg7_vertex_07003C88[] = {
#include "assets/rom/generated/989a382878754567a5ce.inc.c"















};

// 0x07003D88 - 0x07003E68
static const Vtx bits_seg7_vertex_07003D88[] = {
#include "assets/rom/generated/4e58ef5f51c01bc62224.inc.c"













};

// 0x07003E68 - 0x07003F68
static const Vtx bits_seg7_vertex_07003E68[] = {
#include "assets/rom/generated/23fc8db89a1452f0bc71.inc.c"















};

// 0x07003F68 - 0x07004048
static const Vtx bits_seg7_vertex_07003F68[] = {
#include "assets/rom/generated/bb074cb280170e196a22.inc.c"













};

// 0x07004048 - 0x07004138
static const Vtx bits_seg7_vertex_07004048[] = {
#include "assets/rom/generated/22559491785344462324.inc.c"














};

// 0x07004138 - 0x070041A8
static const Vtx bits_seg7_vertex_07004138[] = {
#include "assets/rom/generated/af6016b538164586085f.inc.c"






};

// 0x070041A8 - 0x070042A8
static const Vtx bits_seg7_vertex_070041A8[] = {
#include "assets/rom/generated/1b387ce02adf53cca475.inc.c"















};

// 0x070042A8 - 0x070043A8
static const Vtx bits_seg7_vertex_070042A8[] = {
#include "assets/rom/generated/09de536baba1b91ae9cd.inc.c"















};

// 0x070043A8 - 0x070043E8
static const Vtx bits_seg7_vertex_070043A8[] = {
#include "assets/rom/generated/c5d3267e90a10c79330e.inc.c"



};

// 0x070043E8 - 0x07004590
static const Gfx bits_seg7_dl_070043E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_070036F8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11, 12, 0x0),
    gsSP2Triangles( 8, 13, 14, 0x0,  8, 14, 11, 0x0),
    gsSP2Triangles(15,  8, 12, 0x0,  9,  8, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_070037F8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 4,  3,  5, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9, 11, 12, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_070038F8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_070039D8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(bits_seg7_vertex_07003AC8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_07003BC8, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  5,  6, 0x0,  0,  7,  1, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x07004590 - 0x070045F8
static const Gfx bits_seg7_dl_07004590[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09008000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_07003C88, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 15, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x070045F8 - 0x07004720
static const Gfx bits_seg7_dl_070045F8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09007000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_07003D88, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(bits_seg7_vertex_07003E68, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9, 12, 10, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_07003F68, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSPVertex(bits_seg7_vertex_07004048, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 13, 14, 0x0),
    gsSPVertex(bits_seg7_vertex_07004138, 7, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP1Triangle( 4,  5,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x07004720 - 0x070047D0
static const Gfx bits_seg7_dl_07004720[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09001000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bits_seg7_vertex_070041A8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(bits_seg7_vertex_070042A8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0,  2, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x070047D0 - 0x070047F0
static const Gfx bits_seg7_dl_070047D0[] = {
    gsSPVertex(bits_seg7_vertex_070043A8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x070047F0 - 0x07004880
const Gfx bits_seg7_dl_070047F0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bits_seg7_dl_070043E8),
    gsSPDisplayList(bits_seg7_dl_07004590),
    gsSPDisplayList(bits_seg7_dl_070045F8),
    gsSPDisplayList(bits_seg7_dl_07004720),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(bits_seg7_dl_070047D0),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
