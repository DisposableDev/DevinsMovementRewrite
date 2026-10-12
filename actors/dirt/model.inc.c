// Dirt

// 0x0302BD68

// 0x0302BD80

// 0x0302BD98

// 0x0302BDB0

// 0x0302BDC8
static const Vtx dirt_seg3_vertex_0302BDC8[] = {
#include "assets/rom/generated/5cf69c04bb75a844cc2a.inc.c"


};

// 0x0302BDF8
ALIGNED8 static const Texture dirt_seg3_texture_0302BDF8[] = {
#include "actors/dirt/dirt_particle.rgba16.inc.c"
};

// 0x0302BFF8 - 0x0302C028
const Gfx dirt_seg3_sub_dl_dirt_particle[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dirt_seg3_texture_0302BDF8),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 16 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(dirt_seg3_vertex_0302BDC8, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0302C028 - 0x0302C098
const Gfx dirt_seg3_dl_dirt_particle[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 4, G_TX_NOLOD, G_TX_CLAMP, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (16 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(dirt_seg3_sub_dl_dirt_particle),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x0302C098
static const Vtx dirt_seg3_vertex_tiny_particle[] = {
#include "assets/rom/generated/fcf8e7189e2bcb755548.inc.c"


};

// 0x0302C0C8
static const Vtx dirt_seg3_vertex_tiny_particle_billboard[] = {
#include "assets/rom/generated/316d63684176a56b56bf.inc.c"


};

// 0x0302C0F8
static const Vtx dirt_seg3_vertex_cartoon_star[] = {
#include "assets/rom/generated/1212dc1c2169254897e9.inc.c"









};

// 0x0302C198
static const Vtx dirt_seg3_vertex_cartoon_star_billboard[] = {
#include "assets/rom/generated/39d3797a42fbdbb21e98.inc.c"









};

// 0x0302C238 - 0x0302C298
const Gfx dirt_seg3_sub_dl_cartoon_star[] = {
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPVertex(dirt_seg3_vertex_cartoon_star, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 0,  5,  3, 0x0,  6,  7,  0, 0x0),
    gsSP2Triangles( 2,  8,  6, 0x0,  1,  9,  2, 0x0),
    gsSP2Triangles( 0,  3,  1, 0x0,  2,  6,  0, 0x0),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x0302C298 - 0x0302C2B8
const Gfx dirt_seg3_dl_cartoon_star_red[] = {
    gsSPLightColor(LIGHT_1, 0xff6464ff),
    gsSPLightColor(LIGHT_2, 0x3f1919ff),
    gsSPDisplayList(dirt_seg3_sub_dl_cartoon_star),
    gsSPEndDisplayList(),
};

// 0x0302C2B8 - 0x0302C2D8
const Gfx dirt_seg3_dl_cartoon_star_green[] = {
    gsSPLightColor(LIGHT_1, 0x64ff64ff),
    gsSPLightColor(LIGHT_2, 0x193f19ff),
    gsSPDisplayList(dirt_seg3_sub_dl_cartoon_star),
    gsSPEndDisplayList(),
};

// 0x0302C2D8 - 0x0302C2F8
const Gfx dirt_seg3_dl_cartoon_star_blue[] = {
    gsSPLightColor(LIGHT_1, 0x6464ffff),
    gsSPLightColor(LIGHT_2, 0x19193fff),
    gsSPDisplayList(dirt_seg3_sub_dl_cartoon_star),
    gsSPEndDisplayList(),
};

// 0x0302C2F8 - 0x0302C318
const Gfx dirt_seg3_dl_cartoon_star_yellow[] = {
    gsSPLightColor(LIGHT_1, 0xffff64ff),
    gsSPLightColor(LIGHT_2, 0x3f3f19ff),
    gsSPDisplayList(dirt_seg3_sub_dl_cartoon_star),
    gsSPEndDisplayList(),
};

// 0x0302C318 - 0x0302C378
const Gfx dirt_seg3_dl_cartoon_star_billboard[] = {
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPVertex(dirt_seg3_vertex_cartoon_star_billboard, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 0,  5,  3, 0x0,  6,  7,  0, 0x0),
    gsSP2Triangles( 2,  8,  6, 0x0,  1,  9,  2, 0x0),
    gsSP2Triangles( 0,  3,  1, 0x0,  2,  6,  0, 0x0),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};

const Gfx dirt_seg3_dl_cartoon_star_translucent[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(dirt_seg3_dl_cartoon_star_billboard),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};

// 0x0302C378 - 0x0302C3B0
const Gfx dirt_seg3_dl_tiny_particle_red[] = {
    gsSPLightColor(LIGHT_1, 0xff6464ff),
    gsSPLightColor(LIGHT_2, 0x3f1919ff),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPVertex(dirt_seg3_vertex_tiny_particle, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x0302C3B0 - 0x0302C3E8
const Gfx dirt_seg3_dl_tiny_particle_green[] = {
    gsSPLightColor(LIGHT_1, 0x64ff64ff),
    gsSPLightColor(LIGHT_2, 0x193f19ff),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPVertex(dirt_seg3_vertex_tiny_particle, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x0302C3E8 - 0x0302C420
const Gfx dirt_seg3_dl_tiny_particle_blue[] = {
    gsSPLightColor(LIGHT_1, 0x6464ffff),
    gsSPLightColor(LIGHT_2, 0x19193fff),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPVertex(dirt_seg3_vertex_tiny_particle, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x0302C420 - 0x0302C458
const Gfx dirt_seg3_dl_tiny_particle_yellow[] = {
    gsSPLightColor(LIGHT_1, 0xffff64ff),
    gsSPLightColor(LIGHT_2, 0x3f3f19ff),
    gsSPClearGeometryMode(G_CULL_BACK),
    gsSPVertex(dirt_seg3_vertex_tiny_particle, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};

// 0x0302C458 - 0x0302C480
const Gfx dirt_seg3_dl_tiny_particle_billboard[] = {
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPVertex(dirt_seg3_vertex_tiny_particle_billboard, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};
