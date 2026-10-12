// Clam Shell

// Unreferenced light group

// Unreferenced light group

// 0x05000030
ALIGNED8 static const Texture clam_shell_seg5_texture_05000030[] = {
#include "actors/clam_shell/clam_shell.rgba16.inc.c"
};

// 0x05000830
ALIGNED8 static const Texture clam_shell_seg5_texture_05000830[] = {
#include "actors/clam_shell/clam_shell_mouth.rgba16.inc.c"
};

// 0x05001030

// 0x05001048
static const Vtx clam_shell_seg5_vertex_05001048[] = {
#include "assets/rom/generated/190bc68927fad106f9f0.inc.c"










};

// 0x050010F8
static const Vtx clam_shell_seg5_vertex_050010F8[] = {
#include "assets/rom/generated/22460e8e4b482c7305cf.inc.c"










};

// 0x050011A8 - 0x05001238
const Gfx clam_shell_seg5_dl_050011A8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, clam_shell_seg5_texture_05000030),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(clam_shell_seg5_vertex_05001048, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  4, 0x0),
    gsSP2Triangles( 4,  5,  3, 0x0,  4,  0,  6, 0x0),
    gsSP2Triangles( 6,  7,  4, 0x0,  3,  8,  0, 0x0),
    gsSP2Triangles( 6,  0,  9, 0x0,  2, 10,  9, 0x0),
    gsSP2Triangles( 9,  7,  6, 0x0,  9,  0,  2, 0x0),
    gsSP1Triangle( 8,  1,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x05001238 - 0x050012B8
const Gfx clam_shell_seg5_dl_05001238[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, clam_shell_seg5_texture_05000830),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(clam_shell_seg5_vertex_050010F8, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 0,  3,  6, 0x0,  6,  1,  0, 0x0),
    gsSP2Triangles( 2,  8,  9, 0x0,  9, 10,  3, 0x0),
    gsSP2Triangles( 9,  3,  2, 0x0,  3, 10,  4, 0x0),
    gsSP1Triangle( 2,  3,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x050012B8 - 0x05001320
const Gfx clam_shell_seg5_dl_050012B8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(clam_shell_seg5_dl_050011A8),
    gsSPDisplayList(clam_shell_seg5_dl_05001238),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x05001320

// 0x05001338
static const Vtx clam_shell_seg5_vertex_05001338[] = {
#include "assets/rom/generated/9579121278200abbe57c.inc.c"









};

// 0x050013D8
static const Vtx clam_shell_seg5_vertex_050013D8[] = {
#include "assets/rom/generated/b8f66a7f2f65083bb54a.inc.c"









};

// 0x05001478 - 0x050014F8
const Gfx clam_shell_seg5_dl_05001478[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, clam_shell_seg5_texture_05000030),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(clam_shell_seg5_vertex_05001338, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 4,  5,  0, 0x0,  0,  5,  6, 0x0),
    gsSP2Triangles( 0,  7,  8, 0x0,  2,  7,  0, 0x0),
    gsSP2Triangles( 8,  3,  0, 0x0,  0,  9,  1, 0x0),
    gsSP1Triangle( 6,  9,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x050014F8 - 0x05001568
const Gfx clam_shell_seg5_dl_050014F8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, clam_shell_seg5_texture_05000830),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(clam_shell_seg5_vertex_050013D8, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 2,  4,  3, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 6,  7,  2, 0x0,  2,  7,  4, 0x0),
    gsSP2Triangles( 8,  9,  2, 0x0,  2,  9,  6, 0x0),
    gsSP1Triangle( 5,  8,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x05001568 - 0x050015D0
const Gfx clam_shell_seg5_dl_05001568[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(clam_shell_seg5_dl_05001478),
    gsSPDisplayList(clam_shell_seg5_dl_050014F8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
