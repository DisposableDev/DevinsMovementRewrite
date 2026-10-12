// Door

// 0x03009CE0

// 0x03009CF8

// 0x03009D10
ALIGNED8 static const Texture door_seg3_texture_polished_wooden_door[] = {
#include "actors/door/polished_wooden_door.rgba16.inc.c"
};

// 0x0300AD10
ALIGNED8 static const Texture door_seg3_texture_polished_wooden_door_sides[] = {
#include "actors/door/polished_wooden_door_overlay.rgba16.inc.c"
};

// 0x0300BD10
ALIGNED8 static const Texture door_seg3_texture_rough_wooden_door[] = {
#include "actors/door/rough_wooden_door.rgba16.inc.c"
};

// 0x0300CD10
ALIGNED8 static const Texture door_seg3_texture_rough_wooden_door_sides[] = {
#include "actors/door/rough_wooden_door_overlay.rgba16.inc.c"
};

// 0x0300D510
ALIGNED8 static const Texture door_seg3_texture_metal_door[] = {
#include "actors/door/metal_door.rgba16.inc.c"
};

// 0x0300E510
ALIGNED8 static const Texture door_seg3_texture_metal_door_sides[] = {
#include "actors/door/metal_door_overlay.rgba16.inc.c"
};

// 0x0300ED10
ALIGNED8 static const Texture door_seg3_texture_cave_door[] = {
#include "actors/door/hmc_mural_door.rgba16.inc.c"
};

// 0x0300FD10
ALIGNED8 static const Texture door_seg3_texture_cave_door_sides[] = {
#include "actors/door/hmc_mural_door_overlay.rgba16.inc.c"
};

// 0x03010510
ALIGNED8 static const Texture door_seg3_texture_spooky_door[] = {
#include "actors/door/bbh_door.rgba16.inc.c"
};

// 0x03011510
ALIGNED8 static const Texture door_seg3_texture_spooky_door_sides[] = {
#include "actors/door/bbh_door_overlay.rgba16.inc.c"
};

// 0x03011D10
ALIGNED8 static const Texture door_seg3_texture_zero_star_door_sign[] = {
#include "actors/door/zero_star_door_sign.rgba16.inc.c"
};

// 0x03012510
ALIGNED8 static const Texture door_seg3_texture_one_star_door_sign[] = {
#include "actors/door/one_star_door_sign.rgba16.inc.c"
};

// 0x03012D10
ALIGNED8 static const Texture door_seg3_texture_three_star_door_sign[] = {
#include "actors/door/three_star_door_sign.rgba16.inc.c"
};

// 0x03013510
ALIGNED8 static const Texture door_seg3_texture_lock[] = {
#include "actors/door/door_lock.rgba16.inc.c"
};

// 0x03013910
static const Vtx door_seg3_vertex_polished_wooden_door_near_sides[] = {
#include "assets/rom/generated/b417038307af90b42fd0.inc.c"















};

// 0x03013A10
static const Vtx door_seg3_vertex_polished_wooden_door_near[] = {
#include "assets/rom/generated/eaeff6ec4d9fa11a0cc6.inc.c"







};

// 0x03013A90
static const Vtx door_seg3_vertex_polished_wooden_door_handle_1[] = {
#include "assets/rom/generated/619f30b7c55ef9798551.inc.c"











};

// 0x03013B50
static const Vtx door_seg3_vertex_polished_wooden_door_handle_2[] = {
#include "assets/rom/generated/9ec044779007b10e6551.inc.c"











};

// 0x03013C10 - 0x03013CC8
const Gfx door_seg3_sub_dl_polished_wooden_door_near[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_polished_wooden_door_sides),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(door_seg3_vertex_polished_wooden_door_near_sides, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0,  2, 15, 0x0),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_polished_wooden_door),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(door_seg3_vertex_polished_wooden_door_near, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x03013CC8 - 0x03013D78
const Gfx door_seg3_sub_dl_polished_wooden_door_handle_1[] = {
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPVertex(door_seg3_vertex_polished_wooden_door_handle_1, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 2,  1,  3, 0x0,  5,  2,  4, 0x0),
    gsSP2Triangles( 5,  0,  2, 0x0,  6,  4,  3, 0x0),
    gsSP2Triangles( 7,  6,  3, 0x0,  7,  3,  1, 0x0),
    gsSP2Triangles( 6,  5,  4, 0x0,  0,  7,  1, 0x0),
    gsSP2Triangles( 8,  9,  7, 0x0,  9,  6,  7, 0x0),
    gsSP2Triangles( 8,  7,  0, 0x0,  9, 10,  6, 0x0),
    gsSP2Triangles(10,  5,  6, 0x0, 10, 11,  5, 0x0),
    gsSP2Triangles(11,  0,  5, 0x0, 11,  8,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x03013D78 - 0x03013E28
const Gfx door_seg3_sub_dl_polished_wooden_door_handle_2[] = {
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPVertex(door_seg3_vertex_polished_wooden_door_handle_2, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 5,  0,  4, 0x0,  2,  1,  6, 0x0),
    gsSP2Triangles( 1,  7,  6, 0x0,  1,  5,  7, 0x0),
    gsSP2Triangles( 2,  6,  3, 0x0,  5,  4,  7, 0x0),
    gsSP2Triangles( 6,  8,  9, 0x0,  3,  6,  9, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  4,  3, 10, 0x0),
    gsSP2Triangles( 3,  9, 10, 0x0,  7,  4, 11, 0x0),
    gsSP2Triangles( 7, 11,  8, 0x0,  4, 10, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x03013E28 - 0x03013EA8
const Gfx door_seg3_dl_polished_wooden_door_near[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(door_seg3_sub_dl_polished_wooden_door_near),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPDisplayList(door_seg3_sub_dl_polished_wooden_door_handle_1),
    gsSPDisplayList(door_seg3_sub_dl_polished_wooden_door_handle_2),
    gsSPEndDisplayList(),
};

// 0x03013EA8 - 0x03013F20
const Gfx door_seg3_dl_cabin_door_near[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(door_seg3_sub_dl_polished_wooden_door_near),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPDisplayList(door_seg3_sub_dl_polished_wooden_door_handle_1),
    gsSPEndDisplayList(),
};

// 0x03013F20
static const Vtx door_seg3_vertex_polished_wooden_door_far[] = {
#include "assets/rom/generated/b18ef39602f0a9388202.inc.c"







};

// 0x03013FA0
static const Vtx door_seg3_vertex_polished_wooden_door_far_handle[] = {
#include "assets/rom/generated/8e9a3d33e8572634fa5f.inc.c"







};

// 0x03014020 - 0x03014100
const Gfx door_seg3_sub_dl_polished_wooden_door_far[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_polished_wooden_door),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(door_seg3_vertex_polished_wooden_door_far, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPVertex(door_seg3_vertex_polished_wooden_door_far_handle, 8, 0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x03014100 - 0x03014128
const Gfx door_seg3_dl_polished_wooden_door_far[] = {
    gsSPDisplayList(door_seg3_sub_dl_polished_wooden_door_far),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x03014128 - 0x03014140
const Gfx door_seg3_dl_cabin_door_far[] = {
    gsSPDisplayList(door_seg3_sub_dl_polished_wooden_door_far),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x03014140
static const Vtx door_seg3_vertex_star_sign_near[] = {
#include "assets/rom/generated/d07735e4546523c1584a.inc.c"



};

// 0x03014180
static const Vtx door_seg3_vertex_star_sign_far[] = {
#include "assets/rom/generated/156d067145969d178768.inc.c"



};

// 0x030141C0 - 0x03014218
const Gfx door_seg3_vertex_star_sign_begin[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPEndDisplayList(),
};

// 0x03014218 - 0x03014250
const Gfx door_seg3_vertex_star_sign_end[] = {
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x03014250 - 0x03014280
const Gfx door_seg3_dl_zero_star_near[] = {
    gsSPDisplayList(door_seg3_vertex_star_sign_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_zero_star_door_sign),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(door_seg3_vertex_star_sign_near, 4, 0),
    gsSPBranchList(door_seg3_vertex_star_sign_end),
};

// 0x03014280 - 0x030142B0
const Gfx door_seg3_dl_zero_star_far[] = {
    gsSPDisplayList(door_seg3_vertex_star_sign_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_zero_star_door_sign),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(door_seg3_vertex_star_sign_far, 4, 0),
    gsSPBranchList(door_seg3_vertex_star_sign_end),
};

// 0x030142B0 - 0x030142E0
const Gfx door_seg3_dl_one_star_near[] = {
    gsSPDisplayList(door_seg3_vertex_star_sign_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_one_star_door_sign),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(door_seg3_vertex_star_sign_near, 4, 0),
    gsSPBranchList(door_seg3_vertex_star_sign_end),
};

// 0x030142E0 - 0x03014310
const Gfx door_seg3_dl_one_star_far[] = {
    gsSPDisplayList(door_seg3_vertex_star_sign_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_one_star_door_sign),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(door_seg3_vertex_star_sign_far, 4, 0),
    gsSPBranchList(door_seg3_vertex_star_sign_end),
};

// 0x03014310 - 0x03014340
const Gfx door_seg3_dl_three_stars_near[] = {
    gsSPDisplayList(door_seg3_vertex_star_sign_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_three_star_door_sign),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(door_seg3_vertex_star_sign_near, 4, 0),
    gsSPBranchList(door_seg3_vertex_star_sign_end),
};

// 0x03014340 - 0x03014370
const Gfx door_seg3_dl_three_stars_far[] = {
    gsSPDisplayList(door_seg3_vertex_star_sign_begin),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_three_star_door_sign),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(door_seg3_vertex_star_sign_far, 4, 0),
    gsSPBranchList(door_seg3_vertex_star_sign_end),
};

static const Vtx door_seg3_vertex_key_door_sides[] = {
#include "assets/rom/generated/6453d726b40f85e183f9.inc.c"


















};

const Gfx door_seg3_sub_dl_key_door_sides[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_polished_wooden_door_sides),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(door_seg3_vertex_key_door_sides, 16, 0),
    gsSP2Triangles( 0,  1,  3, 0x0,  1,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPEndDisplayList(),
};

static const Vtx door_seg3_vertex_key_door[] = {
#include "assets/rom/generated/2962dddfd702e8ab7294.inc.c"



















};

const Gfx door_seg3_sub_dl_key_door[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_polished_wooden_door),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(door_seg3_vertex_key_door, 16, 0),
    // right, lock on left
    gsSP2Triangles( 0,  1,  5, 0x0,  0,  5,  4, 0x0),
    gsSP2Triangles( 1,  2,  5, 0x0,  2,  3,  6, 0x0),
    gsSP2Triangles( 2,  6,  5, 0x0,  3,  7,  6, 0x0),
    // left, lock on right
    gsSP2Triangles( 8,  9, 13, 0x0,  8, 13, 11, 0x0),
    gsSP2Triangles( 9, 14, 13, 0x0, 10, 11, 15, 0x0),
    gsSP2Triangles(10, 15, 12, 0x0, 11, 13, 15, 0x0),
    gsSPEndDisplayList(),
};


// 0x03014370
static const Vtx door_seg3_vertex_lock[] = {
#include "assets/rom/generated/3250046733a9de3f2e72.inc.c"







};

// 0x03014528 - 0x03014540
const Gfx door_seg3_sub_dl_lock[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_lock),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 32 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(door_seg3_vertex_lock, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x03014528 - 0x03014540
const Gfx door_seg3_dl_key_door[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(door_seg3_sub_dl_key_door_sides),
    gsSPDisplayList(door_seg3_sub_dl_key_door),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPDisplayList(door_seg3_sub_dl_polished_wooden_door_handle_1),
    gsSPDisplayList(door_seg3_sub_dl_polished_wooden_door_handle_2),
    gsSPDisplayList(door_seg3_sub_dl_lock),
    gsSPEndDisplayList(),
};

// 0x03014558
static const Vtx door_seg3_vertex_themed_door_sides[] = {
#include "assets/rom/generated/e438cb895e81d214f01f.inc.c"















};

// 0x03014658
static const Vtx door_seg3_vertex_themed_door[] = {
#include "assets/rom/generated/e3415a134d12869ef9bc.inc.c"







};

// 0x030146D8
static const Vtx door_seg3_vertex_themed_handle_near_1[] = {
#include "assets/rom/generated/f792a2fc67e29d278e0c.inc.c"














};

// 0x030147C8
static const Vtx door_seg3_vertex_themed_handle_near_2[] = {
#include "assets/rom/generated/c19441f5afa3d82bb907.inc.c"











};

// 0x03014888 - 0x030149C0
const Gfx door_seg3_sub_dl_themed_handle_near[] = { // HACKERSM64_DO: Combine vertices
    gsSPVertex(door_seg3_vertex_themed_handle_near_1, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  6,  3, 0x0),
    gsSP2Triangles( 8,  3,  5, 0x0,  8,  7,  3, 0x0),
    gsSP2Triangles( 9,  5,  4, 0x0, 10,  9,  4, 0x0),
    gsSP2Triangles(10,  4,  6, 0x0,  9,  8,  5, 0x0),
    gsSP2Triangles( 7, 10,  6, 0x0, 11, 12, 10, 0x0),
    gsSP2Triangles(11, 10,  7, 0x0, 12,  9, 10, 0x0),
    gsSP2Triangles(12, 13,  9, 0x0, 13,  8,  9, 0x0),
    gsSP2Triangles(14,  7,  8, 0x0, 13, 14,  8, 0x0),
    gsSP1Triangle(14, 11,  7, 0x0),
    gsSPVertex(door_seg3_vertex_themed_handle_near_2, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  7, 0x0,  4,  3,  0, 0x0),
    gsSP2Triangles( 5,  4,  8, 0x0,  6,  5,  9, 0x0),
    gsSP2Triangles( 5,  8,  9, 0x0,  3,  5,  6, 0x0),
    gsSP2Triangles( 6,  9,  7, 0x0,  4,  0,  8, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles( 9,  8, 11, 0x0,  0,  7,  1, 0x0),
    gsSP2Triangles( 7, 10,  1, 0x0,  8,  0,  2, 0x0),
    gsSP1Triangle( 8,  2, 11, 0x0),
    gsSPEndDisplayList(),
};

// 0x030149C0 - 0x03014A20
const Gfx door_seg3_sub_dl_themed_door_sides[] = {
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(door_seg3_vertex_themed_door_sides, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0,  0,  2, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x03014A20 - 0x03014A50
const Gfx door_seg3_sub_dl_themed_door[] = {
    gsSPVertex(door_seg3_vertex_themed_door, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x03014A50 - 0x03014A80
const Gfx door_seg3_sub_dl_themed_door_begin[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

// 0x03014A80 - 0x03014B30
const Gfx door_seg3_dl_rough_wooden_door_near[] = {
    gsSPDisplayList(door_seg3_sub_dl_themed_door_begin),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_rough_wooden_door_sides),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door_sides),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_rough_wooden_door),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPBranchList(door_seg3_sub_dl_themed_handle_near),
};

// 0x03014B30 - 0x03014BE0
const Gfx door_seg3_dl_rough_wooden_door2_near[] = {
    gsSPDisplayList(door_seg3_sub_dl_themed_door_begin),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_rough_wooden_door_sides),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door_sides),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_rough_wooden_door),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPBranchList(door_seg3_sub_dl_themed_handle_near),
};

// 0x03014BE0 - 0x03014C90
const Gfx door_seg3_dl_metal_door_near[] = {
    gsSPDisplayList(door_seg3_sub_dl_themed_door_begin),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_metal_door_sides),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door_sides),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_metal_door),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPBranchList(door_seg3_sub_dl_themed_handle_near),
};

// 0x03014C90 - 0x03014D40
const Gfx door_seg3_dl_cave_door_near[] = {
    gsSPDisplayList(door_seg3_sub_dl_themed_door_begin),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_cave_door_sides),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door_sides),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_cave_door),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPBranchList(door_seg3_sub_dl_themed_handle_near),
};

// 0x03014D40 - 0x03014DF0
const Gfx door_seg3_dl_haunted_door_near[] = {
    gsSPDisplayList(door_seg3_sub_dl_themed_door_begin),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_spooky_door_sides),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door_sides),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_spooky_door),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPBranchList(door_seg3_sub_dl_themed_handle_near),
};

// 0x03014DF0
static const Vtx door_seg3_vertex_themed_door_far[] = {
#include "assets/rom/generated/331b760108514ea28c8a.inc.c"







};

// 0x03014E70
static const Vtx door_seg3_vertex_themed_handle_far[] = {
#include "assets/rom/generated/385d204fe03eb4f46747.inc.c"







};

// 0x03014EF0 - 0x03014F30
const Gfx door_seg3_sub_dl_themed_door_far[] = {
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(door_seg3_vertex_themed_door_far, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x03014F30 - 0x03014F68
const Gfx door_seg3_sub_dl_themed_handle_far[] = {
    gsSPVertex(door_seg3_vertex_themed_handle_far, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x03014F68 - 0x03014F98
const Gfx door_seg3_sub_dl_themed_door_far_begin[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

// 0x03014F98 - 0x03015008
const Gfx door_seg3_dl_rough_wooden_door_far[] = {
    gsSPDisplayList(door_seg3_sub_dl_themed_door_far_begin),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_rough_wooden_door),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door_far),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPBranchList(door_seg3_sub_dl_themed_handle_far),
};

// 0x03015008 - 0x03015078
const Gfx door_seg3_dl_rough_wooden_door2_far[] = {
    gsSPDisplayList(door_seg3_sub_dl_themed_door_far_begin),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_rough_wooden_door),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door_far),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPBranchList(door_seg3_sub_dl_themed_handle_far),
};

// 0x03015078 - 0x030150E8
const Gfx door_seg3_dl_metal_door_far[] = {
    gsSPDisplayList(door_seg3_sub_dl_themed_door_far_begin),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_metal_door),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door_far),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPBranchList(door_seg3_sub_dl_themed_handle_far),
};

// 0x030150E8 - 0x03015158
const Gfx door_seg3_dl_cave_door_far[] = {
    gsSPDisplayList(door_seg3_sub_dl_themed_door_far_begin),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_cave_door),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door_far),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPBranchList(door_seg3_sub_dl_themed_handle_far),
};

// 0x03015158 - 0x030151C8
const Gfx door_seg3_dl_haunted_door_far[] = {
    gsSPDisplayList(door_seg3_sub_dl_themed_door_far_begin),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, door_seg3_texture_spooky_door),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(door_seg3_sub_dl_themed_door_far),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x3f3f00ff),
    gsSPBranchList(door_seg3_sub_dl_themed_handle_far),
};
