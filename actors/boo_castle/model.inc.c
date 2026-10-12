// Boo (Castle)

// 0x06015658

// 0x06015670
ALIGNED8 static const Texture boo_castle_seg6_texture_06015670[] = {
#include "actors/boo_castle/bbh_boo_eyes.rgba16.inc.c"
};

// 0x06016670
ALIGNED8 static const Texture boo_castle_seg6_texture_06016670[] = {
#include "actors/boo_castle/bbh_boo_mouth.rgba16.inc.c"
};

// 0x06016E70
static const Vtx boo_castle_seg6_vertex_06016E70[] = {
#include "assets/rom/generated/f5cf56e41f1f4a91ef87.inc.c"











};

// 0x06016F30
static const Vtx boo_castle_seg6_vertex_06016F30[] = {
#include "assets/rom/generated/1cdfedebd4388a457c94.inc.c"











};

// 0x06016FF0
static const Vtx boo_castle_seg6_vertex_06016FF0[] = {
#include "assets/rom/generated/570992d4652b05379b8a.inc.c"














};

// 0x060170E0
static const Vtx boo_castle_seg6_vertex_060170E0[] = {
#include "assets/rom/generated/78f239ee1f0daa8a362f.inc.c"














};

// 0x060171D0
static const Vtx boo_castle_seg6_vertex_060171D0[] = {
#include "assets/rom/generated/3e6298232b79fa55c2d6.inc.c"














};

// 0x060172C0
static const Vtx boo_castle_seg6_vertex_060172C0[] = {
#include "assets/rom/generated/14efcffa3941bab668e9.inc.c"














};

// 0x060173B0
static const Vtx boo_castle_seg6_vertex_060173B0[] = {
#include "assets/rom/generated/78be25201e5a6f880634.inc.c"














};

// 0x060174A0
static const Vtx boo_castle_seg6_vertex_060174A0[] = {
#include "assets/rom/generated/3b640df73493624e928f.inc.c"














};

// 0x06017590
static const Vtx boo_castle_seg6_vertex_06017590[] = {
#include "assets/rom/generated/64d4b05df27a9bb5c599.inc.c"














};

// 0x06017680
static const Vtx boo_castle_seg6_vertex_06017680[] = {
#include "assets/rom/generated/2da4f2b0e4bf7d5a151e.inc.c"














};

// 0x06017770
static const Vtx boo_castle_seg6_vertex_06017770[] = {
#include "assets/rom/generated/3c181451f02c0844e5e4.inc.c"














};

// 0x06017860
static const Vtx boo_castle_seg6_vertex_06017860[] = {
#include "assets/rom/generated/f7a1ac9db77fd06ac8c5.inc.c"














};

// 0x06017950
static const Vtx boo_castle_seg6_vertex_06017950[] = {
#include "assets/rom/generated/6c8a6760f35d30f08e94.inc.c"











};

// 0x06017A10 - 0x06017A78
const Gfx boo_castle_seg6_dl_06017A10[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, boo_castle_seg6_texture_06016670),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x979affff),
    gsSPVertex(boo_castle_seg6_vertex_06016E70, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x06017A78 - 0x06017AD0
const Gfx boo_castle_seg6_dl_06017A78[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, boo_castle_seg6_texture_06015670),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(boo_castle_seg6_vertex_06016F30, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x06017AD0 - 0x06017CE0
const Gfx boo_castle_seg6_dl_06017AD0[] = {
    gsSPVertex(boo_castle_seg6_vertex_06016FF0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_castle_seg6_vertex_060170E0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_castle_seg6_vertex_060171D0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_castle_seg6_vertex_060172C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_castle_seg6_vertex_060173B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_castle_seg6_vertex_060174A0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_castle_seg6_vertex_06017590, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_castle_seg6_vertex_06017680, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_castle_seg6_vertex_06017770, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_castle_seg6_vertex_06017860, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(boo_castle_seg6_vertex_06017950, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x06017CE0 - 0x06017D80
const Gfx boo_castle_seg6_dl_06017CE0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsSPNumLights(NUMLIGHTS_1),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(boo_castle_seg6_dl_06017A10),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(boo_castle_seg6_dl_06017A78),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(boo_castle_seg6_dl_06017AD0),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};
