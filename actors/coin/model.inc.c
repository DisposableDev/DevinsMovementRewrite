#include "config.h"

// Coin

#ifdef IA8_30FPS_COINS

// YELLOW
// 0x030056C0
static const Vtx coin_seg3_vertex_yellow[] = {
#include "assets/rom/generated/5c3dc8edae3d5585e4a7.inc.c"



};

// BLUE
// 0x03005700
static const Vtx coin_seg3_vertex_blue[] = {
#include "assets/rom/generated/f1bb062317d849eb6773.inc.c"



};

// RED
// 0x03005740
static const Vtx coin_seg3_vertex_red[] = {
#include "assets/rom/generated/841a0efefe9819635c54.inc.c"



};

// SECRET
// 0x03005780
static const Vtx coin_seg3_vertex_secret[] = {
#include "assets/rom/generated/6e57b39a7c05c54e227b.inc.c"



};

// YELLOW REV
// 0x030056C0
static const Vtx coin_seg3_vertex_yellow_r[] = {
#include "assets/rom/generated/06b6eca8926f52846446.inc.c"



};

// BLUE REV
// 0x03005700
static const Vtx coin_seg3_vertex_blue_r[] = {
#include "assets/rom/generated/23c137564ed95dc7627a.inc.c"



};

// RED REV
// 0x03005740
static const Vtx coin_seg3_vertex_red_r[] = {
#include "assets/rom/generated/09e0bbb83278474ebfa4.inc.c"



};

// SECRET REV
// 0x03005780
static const Vtx coin_seg3_vertex_secret_r[] = {
#include "assets/rom/generated/d1d5ff02b18843722085.inc.c"



};

// 0x03005780
ALIGNED8 static const Texture coin_seg3_texture_0_ia8[] = {
#include "actors/coin/coin_0.ia8.inc.c"
};

// 0x03005F80
ALIGNED8 static const Texture coin_seg3_texture_22_5_ia8[] = {
#include "actors/coin/coin_22_5.ia8.inc.c"
};

// 0x03006780
ALIGNED8 static const Texture coin_seg3_texture_45_ia8[] = {
#include "actors/coin/coin_45.ia8.inc.c"
};

// 0x03006F80
ALIGNED8 static const Texture coin_seg3_texture_67_5_ia8[] = {
#include "actors/coin/coin_67_5.ia8.inc.c"
};

// 0x03007
ALIGNED8 static const Texture coin_seg3_texture_90_ia8[] = {
#include "actors/coin/coin_90.ia8.inc.c"
};

// set geo
// 0x03007780 - 0x030077D0
const Gfx coin_seg3_dl_start[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsSPTexture(32767, 32767, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0, G_TX_LOADTILE, 0, (G_TX_CLAMP | G_TX_NOMIRROR), 6, G_TX_NOLOD, (G_TX_CLAMP | G_TX_NOMIRROR), 6, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadTile(G_TX_LOADTILE, 0, 0, 252, 252),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0, G_TX_RENDERTILE, 0, (G_TX_CLAMP | G_TX_NOMIRROR), 6, G_TX_NOLOD, (G_TX_CLAMP | G_TX_NOMIRROR), 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, 252, 252),
    gsSPEndDisplayList(),
};

// The preceding compatible coin already populated TMEM. Restore all drawing state.
const Gfx coin_seg3_dl_start_reuse[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsSPTexture(32767, 32767, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0, G_TX_LOADTILE, 0, (G_TX_CLAMP | G_TX_NOMIRROR), 6, G_TX_NOLOD, (G_TX_CLAMP | G_TX_NOMIRROR), 6, G_TX_NOLOD),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0, G_TX_RENDERTILE, 0, (G_TX_CLAMP | G_TX_NOMIRROR), 6, G_TX_NOLOD, (G_TX_CLAMP | G_TX_NOMIRROR), 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, 252, 252),
    gsSPEndDisplayList(),
};

// clear geo
// 0x030077D0 - 0x03007800
const Gfx coin_seg3_dl_end[] = {
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPTexture(0x0001, 0x0001, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// YELLOW
const Gfx coin_seg3_dl_yellow_0[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_0_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_22_5[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_45[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_67_5[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_90[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_90_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_67_5_r[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_yellow_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_45_r[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_yellow_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_22_5_r[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_yellow_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

// BLUE
const Gfx coin_seg3_dl_blue_0[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_0_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_22_5[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_45[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_67_5[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_90[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_90_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_67_5_r[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_blue_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_22_5_r[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_blue_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_45_r[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_blue_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

// RED
const Gfx coin_seg3_dl_red_0[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_0_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_22_5[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_45[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_67_5[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_90[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_90_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_67_5_r[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_red_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_45_r[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_red_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};


const Gfx coin_seg3_dl_red_22_5_r[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_red_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};
// SECRET
const Gfx coin_seg3_dl_secret_0[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_0_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_secret, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_22_5[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_secret, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_45[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_secret, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_67_5[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_secret, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_90[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_90_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_secret, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_67_5_r[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_secret_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_45_r[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_secret_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};


const Gfx coin_seg3_dl_secret_22_5_r[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start),
    gsSPVertex(coin_seg3_vertex_secret_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

#else
#ifdef IA8_COINS
// 0x030056C0
static const Vtx coin_seg3_vertex_yellow[] = {
#include "assets/rom/generated/9fbfa6780a1a019affce.inc.c"



};

// 0x03005700
static const Vtx coin_seg3_vertex_blue[] = {
#include "assets/rom/generated/8c11f1aac8abaed05828.inc.c"



};

// 0x03005740
static const Vtx coin_seg3_vertex_red[] = {
#include "assets/rom/generated/8832598dad61a48eb36d.inc.c"



};
#else
static const Vtx coin_seg3_vertex_yellow[] = {
#include "assets/rom/generated/5a796c1ad7fea0c20ff8.inc.c"



};

// 0x03005700
static const Vtx coin_seg3_vertex_blue[] = {
#include "assets/rom/generated/d0264d71adb49788849b.inc.c"



};

// 0x03005740
static const Vtx coin_seg3_vertex_red[] = {
#include "assets/rom/generated/bf4db06a43973521f20d.inc.c"



};
#endif

// 0x03005780
#ifdef IA8_COINS
ALIGNED8 static const Texture coin_seg3_texture_front[] = {
#include "actors/coin/coin_front.ia8.inc.c"
};

// 0x03005F80
ALIGNED8 static const Texture coin_seg3_texture_tilt_right[] = {
#include "actors/coin/coin_tilt_right.ia8.inc.c"
};

// 0x03006780
ALIGNED8 static const Texture coin_seg3_texture_side[] = {
#include "actors/coin/coin_side.ia8.inc.c"
};

// 0x03006F80
ALIGNED8 static const Texture coin_seg3_texture_tilt_left[] = {
#include "actors/coin/coin_tilt_left.ia8.inc.c"
};
#else
ALIGNED8 static const Texture coin_seg3_texture_front[] = {
#include "actors/coin/coin_front.ia16.inc.c"
};

ALIGNED8 static const Texture coin_seg3_texture_tilt_right[] = {
#include "actors/coin/coin_tilt_right.ia16.inc.c"
};

ALIGNED8 static const Texture coin_seg3_texture_side[] = {
#include "actors/coin/coin_side.ia16.inc.c"
};

ALIGNED8 static const Texture coin_seg3_texture_tilt_left[] = {
#include "actors/coin/coin_tilt_left.ia16.inc.c"
};
#endif

// 0x03007780 - 0x030077D0
const Gfx coin_seg3_sub_dl_begin[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
#ifdef IA8_COINS
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64/2 * 64 - 1, CALC_DXT(64/2, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_8b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
#else
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
#endif
    gsSPEndDisplayList(),
};

// 0x030077D0 - 0x03007800
const Gfx coin_seg3_sub_dl_end[] = {
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPTexture(0x0001, 0x0001, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x03007800 - 0x03007828
const Gfx coin_seg3_dl_yellow_front[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_front),
    gsSPDisplayList(coin_seg3_sub_dl_begin),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_sub_dl_end),
};

// 0x03007828 - 0x03007850
const Gfx coin_seg3_dl_yellow_tilt_right[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_tilt_right),
    gsSPDisplayList(coin_seg3_sub_dl_begin),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_sub_dl_end),
};

// 0x03007850 - 0x03007878
const Gfx coin_seg3_dl_yellow_side[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_side),
    gsSPDisplayList(coin_seg3_sub_dl_begin),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_sub_dl_end),
};

// 0x03007878 - 0x030078A0
const Gfx coin_seg3_dl_yellow_tilt_left[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_tilt_left),
    gsSPDisplayList(coin_seg3_sub_dl_begin),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_sub_dl_end),
};

// 0x030078A0 - 0x030078C8
const Gfx coin_seg3_dl_blue_front[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_front),
    gsSPDisplayList(coin_seg3_sub_dl_begin),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_sub_dl_end),
};

// 0x030078C8 - 0x030078F0
const Gfx coin_seg3_dl_blue_tilt_right[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_tilt_right),
    gsSPDisplayList(coin_seg3_sub_dl_begin),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_sub_dl_end),
};

// 0x030078F0 - 0x03007918
const Gfx coin_seg3_dl_blue_side[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_side),
    gsSPDisplayList(coin_seg3_sub_dl_begin),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_sub_dl_end),
};

// 0x03007918 - 0x03007940
const Gfx coin_seg3_dl_blue_tilt_left[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_tilt_left),
    gsSPDisplayList(coin_seg3_sub_dl_begin),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_sub_dl_end),
};

// 0x03007940 - 0x03007968
const Gfx coin_seg3_dl_red_front[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_front),
    gsSPDisplayList(coin_seg3_sub_dl_begin),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_sub_dl_end),
};

// 0x03007968 - 0x03007990
const Gfx coin_seg3_dl_red_tilt_right[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_tilt_right),
    gsSPDisplayList(coin_seg3_sub_dl_begin),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_sub_dl_end),
};

// 0x03007990 - 0x030079B8
const Gfx coin_seg3_dl_red_side[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_side),
    gsSPDisplayList(coin_seg3_sub_dl_begin),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_sub_dl_end),
};

// 0x030079B8 - 0x030079E0
const Gfx coin_seg3_dl_red_tilt_left[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, coin_seg3_texture_tilt_left),
    gsSPDisplayList(coin_seg3_sub_dl_begin),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_sub_dl_end),
};

#endif

// Pause-menu copies use environment alpha. World coins keep their original
// shade-alpha material, textures, vertices and display lists.
#ifdef IA8_30FPS_COINS
#define PAUSE_COIN_TEX_SIZE G_IM_SIZ_8b
#define PAUSE_COIN_TEX_WIDTH 64
#define PAUSE_COIN_BEGIN coin_seg3_dl_start
#define PAUSE_COIN_END coin_seg3_dl_end
#else
#define PAUSE_COIN_TEX_SIZE G_IM_SIZ_16b
#define PAUSE_COIN_TEX_WIDTH 1
#define PAUSE_COIN_BEGIN coin_seg3_sub_dl_begin
#define PAUSE_COIN_END coin_seg3_sub_dl_end
#endif

#define DEFINE_PAUSE_RED_COIN(frame, texture, vertices) \
const Gfx coin_seg3_dl_red_pause_##frame[] = { \
    gsDPPipeSync(), \
    gsDPSetTextureImage(G_IM_FMT_IA, PAUSE_COIN_TEX_SIZE, PAUSE_COIN_TEX_WIDTH, texture), \
    gsSPDisplayList(PAUSE_COIN_BEGIN), \
    gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, ENVIRONMENT, 0, \
                      TEXEL0, 0, SHADE, 0, TEXEL0, 0, ENVIRONMENT, 0), \
    gsSPVertex(vertices, 4, 0), \
    gsSPBranchList(PAUSE_COIN_END), \
};

#ifdef IA8_30FPS_COINS
DEFINE_PAUSE_RED_COIN(0,      coin_seg3_texture_0_ia8,    coin_seg3_vertex_red)
DEFINE_PAUSE_RED_COIN(22_5,   coin_seg3_texture_22_5_ia8, coin_seg3_vertex_red)
DEFINE_PAUSE_RED_COIN(45,     coin_seg3_texture_45_ia8,   coin_seg3_vertex_red)
DEFINE_PAUSE_RED_COIN(67_5,   coin_seg3_texture_67_5_ia8, coin_seg3_vertex_red)
DEFINE_PAUSE_RED_COIN(90,     coin_seg3_texture_90_ia8,   coin_seg3_vertex_red)
DEFINE_PAUSE_RED_COIN(67_5_r, coin_seg3_texture_67_5_ia8, coin_seg3_vertex_red_r)
DEFINE_PAUSE_RED_COIN(45_r,   coin_seg3_texture_45_ia8,   coin_seg3_vertex_red_r)
DEFINE_PAUSE_RED_COIN(22_5_r, coin_seg3_texture_22_5_ia8, coin_seg3_vertex_red_r)
#else
DEFINE_PAUSE_RED_COIN(front,      coin_seg3_texture_front,      coin_seg3_vertex_red)
DEFINE_PAUSE_RED_COIN(tilt_right, coin_seg3_texture_tilt_right, coin_seg3_vertex_red)
DEFINE_PAUSE_RED_COIN(side,       coin_seg3_texture_side,       coin_seg3_vertex_red)
DEFINE_PAUSE_RED_COIN(tilt_left,  coin_seg3_texture_tilt_left,  coin_seg3_vertex_red)
#endif

#undef DEFINE_PAUSE_RED_COIN
#undef PAUSE_COIN_TEX_SIZE
#undef PAUSE_COIN_TEX_WIDTH
#undef PAUSE_COIN_BEGIN
#undef PAUSE_COIN_END

// Consecutive-draw alternatives selected by the master-list renderer.
#ifdef IA8_30FPS_COINS
const Gfx coin_seg3_dl_yellow_0_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_0_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_22_5_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_45_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_67_5_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_90_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_90_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_yellow, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_67_5_r_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_yellow_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_45_r_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_yellow_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_yellow_22_5_r_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_yellow_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_0_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_0_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_22_5_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_45_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_67_5_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_90_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_90_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_blue, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_67_5_r_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_blue_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_22_5_r_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_blue_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_blue_45_r_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_blue_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_0_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_0_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_22_5_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_45_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_67_5_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_90_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_90_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_red, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_67_5_r_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_red_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_45_r_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_red_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_red_22_5_r_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_red_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_0_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_0_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_secret, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_22_5_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_secret, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_45_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_secret, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_67_5_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_secret, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_90_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_90_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_secret, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_67_5_r_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_67_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_secret_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_45_r_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_45_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_secret_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

const Gfx coin_seg3_dl_secret_22_5_r_reuse[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_8b, 64, coin_seg3_texture_22_5_ia8),
    gsSPDisplayList(coin_seg3_dl_start_reuse),
    gsSPVertex(coin_seg3_vertex_secret_r, 4, 0),
    gsSPBranchList(coin_seg3_dl_end),
};

#endif
