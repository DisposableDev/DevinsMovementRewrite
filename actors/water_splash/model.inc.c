// Water Splash

// 0x0402A588
static const Vtx water_splash_seg4_vertex_0402A588[] = {
#include "assets/rom/generated/18e63f96fa54f0800a8f.inc.c"



};

// Lossless CI8 indices and a separate RGBA16 palette for each animation frame.
ALIGNED8 static const Texture water_splash_indices_0[] = {
#include "actors/water_splash/water_splash_0.ci8.inc.c"
};

ALIGNED8 static const Texture water_splash_palette_0[] = {
#include "actors/water_splash/water_splash_0.ci8.pal"
};

ALIGNED8 static const Texture water_splash_indices_1[] = {
#include "actors/water_splash/water_splash_1.ci8.inc.c"
};

ALIGNED8 static const Texture water_splash_palette_1[] = {
#include "actors/water_splash/water_splash_1.ci8.pal"
};

ALIGNED8 static const Texture water_splash_indices_2[] = {
#include "actors/water_splash/water_splash_2.ci8.inc.c"
};

ALIGNED8 static const Texture water_splash_palette_2[] = {
#include "actors/water_splash/water_splash_2.ci8.pal"
};

ALIGNED8 static const Texture water_splash_indices_3[] = {
#include "actors/water_splash/water_splash_3.ci8.inc.c"
};

ALIGNED8 static const Texture water_splash_palette_3[] = {
#include "actors/water_splash/water_splash_3.ci8.pal"
};

ALIGNED8 static const Texture water_splash_indices_4[] = {
#include "actors/water_splash/water_splash_4.ci8.inc.c"
};

ALIGNED8 static const Texture water_splash_palette_4[] = {
#include "actors/water_splash/water_splash_4.ci8.pal"
};

ALIGNED8 static const Texture water_splash_indices_5[] = {
#include "actors/water_splash/water_splash_5.ci8.inc.c"
};

ALIGNED8 static const Texture water_splash_palette_5[] = {
#include "actors/water_splash/water_splash_5.ci8.pal"
};

ALIGNED8 static const Texture water_splash_indices_6[] = {
#include "actors/water_splash/water_splash_6.ci8.inc.c"
};

ALIGNED8 static const Texture water_splash_palette_6[] = {
#include "actors/water_splash/water_splash_6.ci8.pal"
};

ALIGNED8 static const Texture water_splash_indices_7[] = {
#include "actors/water_splash/water_splash_7.ci8.inc.c"
};

ALIGNED8 static const Texture water_splash_palette_7[] = {
#include "actors/water_splash/water_splash_7.ci8.pal"
};

// Only the lower half of TMEM is replaced; the preceding frame's palette can persist.
const Gfx water_splash_seg4_dl_040325C8[] = {
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetTextureLUT(G_TT_RGBA16),
    gsSPVertex(water_splash_seg4_vertex_0402A588, 4, 0),
    gsSP1Quadrangle(0, 1, 2, 3, 0x0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetTextureLUT(G_TT_NONE),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

const Gfx water_splash_seg4_dl_04032640[] = {
    gsDPPipeSync(),
    gsDPLoadTLUT_pal256(water_splash_palette_0),
    gsDPLoadTextureBlock(water_splash_indices_0, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_04032640_reuse[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(water_splash_indices_0, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_04032658[] = {
    gsDPPipeSync(),
    gsDPLoadTLUT_pal256(water_splash_palette_1),
    gsDPLoadTextureBlock(water_splash_indices_1, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_04032658_reuse[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(water_splash_indices_1, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_04032670[] = {
    gsDPPipeSync(),
    gsDPLoadTLUT_pal256(water_splash_palette_2),
    gsDPLoadTextureBlock(water_splash_indices_2, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_04032670_reuse[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(water_splash_indices_2, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_04032688[] = {
    gsDPPipeSync(),
    gsDPLoadTLUT_pal256(water_splash_palette_3),
    gsDPLoadTextureBlock(water_splash_indices_3, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_04032688_reuse[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(water_splash_indices_3, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_040326A0[] = {
    gsDPPipeSync(),
    gsDPLoadTLUT_pal256(water_splash_palette_4),
    gsDPLoadTextureBlock(water_splash_indices_4, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_040326A0_reuse[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(water_splash_indices_4, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_040326B8[] = {
    gsDPPipeSync(),
    gsDPLoadTLUT_pal256(water_splash_palette_5),
    gsDPLoadTextureBlock(water_splash_indices_5, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_040326B8_reuse[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(water_splash_indices_5, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_040326D0[] = {
    gsDPPipeSync(),
    gsDPLoadTLUT_pal256(water_splash_palette_6),
    gsDPLoadTextureBlock(water_splash_indices_6, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_040326D0_reuse[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(water_splash_indices_6, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_040326E8[] = {
    gsDPPipeSync(),
    gsDPLoadTLUT_pal256(water_splash_palette_7),
    gsDPLoadTextureBlock(water_splash_indices_7, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};

const Gfx water_splash_seg4_dl_040326E8_reuse[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(water_splash_indices_7, G_IM_FMT_CI, G_IM_SIZ_8b,
                         32, 64, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPBranchList(water_splash_seg4_dl_040325C8),
};
