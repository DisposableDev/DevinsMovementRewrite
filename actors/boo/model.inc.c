// Boo

// 0x05009B28

// 0x05009B40
ALIGNED8 static const Texture boo_seg5_texture_05009B40[] = {
#include "actors/boo/boo_eyes.rgba16.inc.c"
};

// 0x0500AB40
ALIGNED8 static const Texture boo_seg5_texture_0500AB40[] = {
#include "actors/boo/boo_mouth.rgba16.inc.c"
};

// 0x0500B340
static const Vtx boo_seg5_vertex_0500B340[] = {
#include "assets/rom/generated/3678b4853455f379adde.inc.c"











};

// 0x0500B400
static const Vtx boo_seg5_vertex_0500B400[] = {
#include "assets/rom/generated/142f06e5957f32141f31.inc.c"











};

// 0x0500B4C0
static const Vtx boo_seg5_vertex_0500B4C0[] = {
#include "assets/rom/generated/10fa4a522d36033a224e.inc.c"














};

// 0x0500B5B0
static const Vtx boo_seg5_vertex_0500B5B0[] = {
#include "assets/rom/generated/1aad1ae9df1fe97c9495.inc.c"














};

// 0x0500B6A0
static const Vtx boo_seg5_vertex_0500B6A0[] = {
#include "assets/rom/generated/242572ccd98526f9b222.inc.c"














};

// 0x0500B790
static const Vtx boo_seg5_vertex_0500B790[] = {
#include "assets/rom/generated/47efa896a299122f59c7.inc.c"














};

// 0x0500B880
static const Vtx boo_seg5_vertex_0500B880[] = {
#include "assets/rom/generated/15d91f473746fc1dc3af.inc.c"














};

// 0x0500B970
static const Vtx boo_seg5_vertex_0500B970[] = {
#include "assets/rom/generated/7db46a3c808d914b14f9.inc.c"














};

// 0x0500BA60
static const Vtx boo_seg5_vertex_0500BA60[] = {
#include "assets/rom/generated/fd7c2e56eaf96a01ebd7.inc.c"














};

// 0x0500BB50
static const Vtx boo_seg5_vertex_0500BB50[] = {
#include "assets/rom/generated/f3407a0264e4c68fec61.inc.c"














};

// 0x0500BC40
static const Vtx boo_seg5_vertex_0500BC40[] = {
#include "assets/rom/generated/331c8914f30b3ac2d193.inc.c"














};

// 0x0500BD30
static const Vtx boo_seg5_vertex_0500BD30[] = {
#include "assets/rom/generated/34d739337c286637690a.inc.c"














};

// 0x0500BE20
static const Vtx boo_seg5_vertex_0500BE20[] = {
#include "assets/rom/generated/1b2d271ca2e1d3108436.inc.c"











};

// 0x0500BEE0 - 0x0500BF48
const Gfx boo_seg5_dl_0500BEE0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, boo_seg5_texture_0500AB40),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x979affff),
    gsSPVertex(boo_seg5_vertex_0500B340, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500BF48 - 0x0500BFA0
const Gfx boo_seg5_dl_0500BF48[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, boo_seg5_texture_05009B40),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(boo_seg5_vertex_0500B400, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500BFA0 - 0x0500C1B0
const Gfx boo_seg5_dl_0500BFA0[] = {
    gsSPVertex(boo_seg5_vertex_0500B4C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_seg5_vertex_0500B5B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_seg5_vertex_0500B6A0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_seg5_vertex_0500B790, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_seg5_vertex_0500B880, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_seg5_vertex_0500B970, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_seg5_vertex_0500BA60, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_seg5_vertex_0500BB50, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_seg5_vertex_0500BC40, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_seg5_vertex_0500BD30, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_seg5_vertex_0500BE20, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x0500C1B0 - 0x0500C250
const Gfx boo_seg5_dl_0500C1B0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsSPNumLights(NUMLIGHTS_1),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(boo_seg5_dl_0500BEE0),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(boo_seg5_dl_0500BF48),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(boo_seg5_dl_0500BFA0),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};
