// Koopa (Small Koopa, Big Koopa [Koopa the Quick])


// Unreferenced light group

// 0x060025A0

// 0x060025B8

// 0x060025D0

// Unreferenced light group

// 0x06002600

// 0x06002618

// 0x06002630

// This light approximates the pink shorts that Koopas have, but is not exact
// because the original light had two different values for ambient and diffuse.
// However, defining it this way allows the light converter to run correctly.

// 0x06002648
ALIGNED8 static const Texture koopa_seg6_texture_06002648[] = {
#include "actors/koopa/koopa_shell_front.rgba16.inc.c"
};

// 0x06002E48
ALIGNED8 static const Texture koopa_seg6_texture_06002E48[] = {
#include "actors/koopa/koopa_shell_back.rgba16.inc.c"
};

// 0x06003648
ALIGNED8 static const Texture koopa_seg6_texture_06003648[] = {
#include "actors/koopa/koopa_shoe.rgba16.inc.c"
};

// 0x06003E48
ALIGNED8 static const Texture koopa_seg6_texture_06003E48[] = {
#include "actors/koopa/koopa_shell_front_top.rgba16.inc.c"
};

// 0x06004648
ALIGNED8 static const Texture koopa_seg6_texture_06004648[] = {
#include "actors/koopa/koopa_eyes_open.rgba16.inc.c"
};

// 0x06004E48
ALIGNED8 static const Texture koopa_seg6_texture_06004E48[] = {
#include "actors/koopa/koopa_eyes_closed.rgba16.inc.c"
};

// 0x06005648
ALIGNED8 static const Texture koopa_seg6_texture_06005648[] = {
#include "actors/koopa/koopa_eye_border.rgba16.inc.c"
};

// 0x06005E48
ALIGNED8 static const Texture koopa_seg6_texture_06005E48[] = {
#include "actors/koopa/koopa_nostrils.rgba16.inc.c"
};

// 0x06006E48

// 0x06006E60

// 0x06006E78
static const Vtx koopa_seg6_vertex_06006E78[] = {
#include "assets/rom/generated/1eed19d0516559f63dc7.inc.c"








};

// 0x06006F08
static const Vtx koopa_seg6_vertex_06006F08[] = {
#include "assets/rom/generated/5934511b3483f669b769.inc.c"














};

// 0x06006FF8
static const Vtx koopa_seg6_vertex_06006FF8[] = {
#include "assets/rom/generated/263922cf817d9344f035.inc.c"















};

// 0x060070F8
static const Vtx koopa_seg6_vertex_060070F8[] = {
#include "assets/rom/generated/840a926a8e74d1520e53.inc.c"














};

// 0x060071E8
static const Vtx koopa_seg6_vertex_060071E8[] = {
#include "assets/rom/generated/531b1204b13adac612b5.inc.c"







};

// 0x06007268
static const Vtx koopa_seg6_vertex_06007268[] = {
#include "assets/rom/generated/deee8266a25a8f3174fd.inc.c"














};

// 0x06007358
static const Vtx koopa_seg6_vertex_06007358[] = {
#include "assets/rom/generated/ec5846121a47c2841a8f.inc.c"















};

// 0x06007458
static const Vtx koopa_seg6_vertex_06007458[] = {
#include "assets/rom/generated/116fe0ebd499b204b08c.inc.c"


};

// 0x06007488
static const Vtx koopa_seg6_vertex_06007488[] = {
#include "assets/rom/generated/5e332a59940088bf4272.inc.c"


};

// 0x060074B8
static const Vtx koopa_seg6_vertex_060074B8[] = {
#include "assets/rom/generated/4b1c70cbdfe3c033d699.inc.c"















};

// 0x060075B8
static const Vtx koopa_seg6_vertex_060075B8[] = {
#include "assets/rom/generated/2890ce093b079babb798.inc.c"









};

// 0x06007658 - 0x060076B0
const Gfx koopa_seg6_dl_06007658[] = {
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(koopa_seg6_vertex_06006E78, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 0,  5,  3, 0x0,  4,  6,  0, 0x0),
    gsSP2Triangles( 7,  5,  0, 0x0,  8,  1,  0, 0x0),
    gsSP1Triangle( 0,  6,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x060076B0 - 0x06007850
const Gfx koopa_seg6_dl_060076B0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, koopa_seg6_texture_06005648),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(koopa_seg6_vertex_06006F08, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  5,  7, 0x0,  5,  6,  8, 0x0),
    gsSP2Triangles( 3,  5,  8, 0x0,  7,  5,  4, 0x0),
    gsSP2Triangles( 9,  6, 10, 0x0,  7, 10,  6, 0x0),
    gsSP2Triangles( 9,  8,  6, 0x0,  8, 11,  3, 0x0),
    gsSP2Triangles(11,  8,  9, 0x0,  3, 11, 12, 0x0),
    gsSP2Triangles( 2,  3, 12, 0x0,  3, 13,  4, 0x0),
    gsSP2Triangles(13,  3,  2, 0x0,  2,  1, 13, 0x0),
    gsSP1Triangle(14,  0,  2, 0x0),
    gsSPVertex(koopa_seg6_vertex_06006FF8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 3,  5,  4, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 7,  8,  5, 0x0,  4,  5,  8, 0x0),
    gsSP2Triangles( 6,  5,  3, 0x0,  0,  4,  1, 0x0),
    gsSP2Triangles( 8,  1,  4, 0x0,  2,  9,  0, 0x0),
    gsSP2Triangles( 2, 10,  9, 0x0,  9, 11, 12, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(koopa_seg6_vertex_060070F8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  3,  7,  6, 0x0),
    gsSP2Triangles( 6,  8,  9, 0x0, 10,  8,  7, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0,  0, 11,  1, 0x0),
    gsSP1Triangle(14,  8, 10, 0x0),
    gsSPVertex(koopa_seg6_vertex_060071E8, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 1,  0,  4, 0x0,  5,  6,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x06007850 - 0x06007970
const Gfx koopa_seg6_dl_06007850[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, koopa_seg6_texture_06005E48),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(koopa_seg6_vertex_06007268, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  2,  1, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  1,  0,  6, 0x0),
    gsSP2Triangles( 6,  4,  1, 0x0,  5,  3,  1, 0x0),
    gsSP2Triangles( 7,  4,  6, 0x0,  7,  5,  4, 0x0),
    gsSP2Triangles( 7,  8,  5, 0x0,  3,  5,  9, 0x0),
    gsSP2Triangles( 9,  5,  8, 0x0,  7,  6, 10, 0x0),
    gsSP2Triangles(11,  7, 10, 0x0,  8,  7, 11, 0x0),
    gsSP2Triangles(12,  9,  8, 0x0,  8, 11, 12, 0x0),
    gsSP1Triangle(13,  2, 14, 0x0),
    gsSPVertex(koopa_seg6_vertex_06007358, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  3, 0x0),
    gsSP2Triangles( 9,  1,  3, 0x0,  9,  3,  8, 0x0),
    gsSP2Triangles( 4, 10, 11, 0x0,  4, 12,  5, 0x0),
    gsSP2Triangles(13, 14, 12, 0x0, 14, 15, 12, 0x0),
    gsSP1Triangle(12, 11, 13, 0x0),
    gsSPVertex(koopa_seg6_vertex_06007458, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x06007970 - 0x06007A60
const Gfx koopa_seg6_dl_06007970[] = {
    gsSPLightColor(LIGHT_1, 0xce1635ff),
    gsSPLightColor(LIGHT_2, 0x670b1aff),
    gsSPVertex(koopa_seg6_vertex_06007488, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(koopa_seg6_vertex_060074B8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  4,  7, 0x0,  4,  6,  5, 0x0),
    gsSP2Triangles( 6,  8,  5, 0x0,  8,  6,  9, 0x0),
    gsSP2Triangles( 5, 10,  3, 0x0,  8, 10,  5, 0x0),
    gsSP2Triangles(11, 12,  1, 0x0, 12,  2,  1, 0x0),
    gsSP2Triangles( 1,  7, 11, 0x0, 13, 14, 12, 0x0),
    gsSP2Triangles(12, 15,  2, 0x0, 12, 14, 15, 0x0),
    gsSP1Triangle( 0,  2, 15, 0x0),
    gsSPVertex(koopa_seg6_vertex_060075B8, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  5,  4, 0x0),
    gsSP2Triangles( 6,  8,  4, 0x0,  9,  6,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x06007A60 - 0x06007AA0
const Gfx koopa_seg6_dl_06007A60[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPEndDisplayList(),
};

// 0x06007AA0 - 0x06007AF8
const Gfx koopa_seg6_dl_06007AA0[] = {
    gsSPDisplayList(koopa_seg6_dl_06007658),
    gsSPDisplayList(koopa_seg6_dl_060076B0),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(koopa_seg6_dl_06007850),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(koopa_seg6_dl_06007970),
    gsSPEndDisplayList(),
};

// 0x06007AF8 - 0x06007B20
const Gfx koopa_seg6_dl_06007AF8[] = {
    gsSPDisplayList(koopa_seg6_dl_06007A60),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, koopa_seg6_texture_06004648),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(koopa_seg6_dl_06007AA0),
};

// 0x06007B20 - 0x06007B48
const Gfx koopa_seg6_dl_06007B20[] = {
    gsSPDisplayList(koopa_seg6_dl_06007A60),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, koopa_seg6_texture_06004E48),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(koopa_seg6_dl_06007AA0),
};

// 0x06007B48

// 0x06007B60

// 0x06007B78
static const Vtx koopa_seg6_vertex_06007B78[] = {
#include "assets/rom/generated/c2126b94706aec5da712.inc.c"









};

// 0x06007C18
static const Vtx koopa_seg6_vertex_06007C18[] = {
#include "assets/rom/generated/2e7a8d5049f52d8badcc.inc.c"













};

// 0x06007CF8
static const Vtx koopa_seg6_vertex_06007CF8[] = {
#include "assets/rom/generated/0b2411292c16779d65b4.inc.c"




};

// 0x06007D48
static const Vtx koopa_seg6_vertex_06007D48[] = {
#include "assets/rom/generated/fb23fd8dfb3a81fc5161.inc.c"














};

// 0x06007E38
static const Vtx koopa_seg6_vertex_06007E38[] = {
#include "assets/rom/generated/db7b836f582578f0799b.inc.c"














};

// 0x06007F28
static const Vtx koopa_seg6_vertex_06007F28[] = {
#include "assets/rom/generated/73c3ae04cf40e6a5bbfd.inc.c"








};

// 0x06007FB8 - 0x06008050
const Gfx koopa_seg6_dl_06007FB8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, koopa_seg6_texture_06002E48),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(koopa_seg6_vertex_06007B78, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 0,  4,  1, 0x0,  0,  5,  4, 0x0),
    gsSP2Triangles( 0,  3,  5, 0x0,  1,  4,  6, 0x0),
    gsSP2Triangles( 6,  4,  5, 0x0,  3,  2,  7, 0x0),
    gsSP2Triangles( 3,  8,  5, 0x0,  8,  3,  7, 0x0),
    gsSP2Triangles( 9,  5,  8, 0x0,  9,  6,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x06008050 - 0x060080F8
const Gfx koopa_seg6_dl_06008050[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, koopa_seg6_texture_06002648),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(koopa_seg6_vertex_06007C18, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 0,  2,  3, 0x0,  1,  0,  5, 0x0),
    gsSP2Triangles( 4,  6,  0, 0x0,  6,  5,  0, 0x0),
    gsSP2Triangles( 3,  2,  7, 0x0,  8,  2,  1, 0x0),
    gsSP2Triangles( 2,  8,  7, 0x0,  9,  4,  3, 0x0),
    gsSP2Triangles( 3, 10,  9, 0x0,  7, 11,  3, 0x0),
    gsSP2Triangles(11, 10,  3, 0x0, 10, 12,  9, 0x0),
    gsSP2Triangles( 4,  9, 13, 0x0,  4, 13,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x060080F8 - 0x06008250
const Gfx koopa_seg6_dl_060080F8[] = {
    gsSPLightColor(LIGHT_1, 0xe0ae00ff),
    gsSPLightColor(LIGHT_2, 0x705700ff),
    gsSPVertex(koopa_seg6_vertex_06007CF8, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP1Triangle( 2,  4,  3, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(koopa_seg6_vertex_06007D48, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 4,  1,  3, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  4,  3,  6, 0x0),
    gsSP2Triangles( 7,  4,  6, 0x0,  2,  5,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 11, 12,  9, 0x0),
    gsSP2Triangles(12, 11, 13, 0x0, 11, 10, 14, 0x0),
    gsSP2Triangles( 9,  2, 10, 0x0,  2,  9,  0, 0x0),
    gsSPVertex(koopa_seg6_vertex_06007E38, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  3, 0x0,  6,  7,  3, 0x0),
    gsSP2Triangles( 0,  8,  1, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 2, 12,  0, 0x0, 13, 14, 11, 0x0),
    gsSP1Triangle(11, 14,  9, 0x0),
    gsSPVertex(koopa_seg6_vertex_06007F28, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  4,  5,  2, 0x0),
    gsSP2Triangles( 6,  2,  7, 0x0,  6,  3,  2, 0x0),
    gsSP2Triangles( 7,  2,  5, 0x0,  6,  7,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x06008250 - 0x060082C0
const Gfx koopa_seg6_dl_06008250[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(koopa_seg6_dl_06007FB8),
    gsSPDisplayList(koopa_seg6_dl_06008050),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(koopa_seg6_dl_060080F8),
    gsSPEndDisplayList(),
};

// 0x060082C0

// 0x060082D8

// 0x060082F0
static const Vtx koopa_seg6_vertex_060082F0[] = {
#include "assets/rom/generated/565c06e3325eb9ad499e.inc.c"















};

// 0x060083F0
static const Vtx koopa_seg6_vertex_060083F0[] = {
#include "assets/rom/generated/c7f72ceebea223b13598.inc.c"







};

// 0x06008470
static const Vtx koopa_seg6_vertex_06008470[] = {
#include "assets/rom/generated/f821b4768a082e231cf0.inc.c"











};

// 0x06008530 - 0x060085E8
const Gfx koopa_seg6_dl_06008530[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, koopa_seg6_texture_06003648),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(koopa_seg6_vertex_060082F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 1,  4,  3, 0x0,  4,  5,  3, 0x0),
    gsSP2Triangles( 0,  2,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  5,  7, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 9, 10,  8, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 11, 13, 10, 0x0),
    gsSP2Triangles(12, 14, 13, 0x0, 12, 15, 14, 0x0),
    gsSP2Triangles( 6, 14, 15, 0x0,  6, 15,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x060085E8 - 0x06008640
const Gfx koopa_seg6_dl_060085E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, koopa_seg6_texture_06003E48),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(koopa_seg6_vertex_060083F0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 2,  4,  0, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 5,  6,  1, 0x0,  5,  7,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x06008640 - 0x060086D0
const Gfx koopa_seg6_dl_06008640[] = {
    gsSPLightColor(LIGHT_1, 0xc800ff),
    gsSPLightColor(LIGHT_2, 0x6400ff),
    gsSPVertex(koopa_seg6_vertex_06008470, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 3,  4,  0, 0x0,  1,  5,  2, 0x0),
    gsSP2Triangles( 3,  2,  6, 0x0,  7,  6,  2, 0x0),
    gsSP2Triangles( 7,  2,  5, 0x0,  3,  6,  8, 0x0),
    gsSP2Triangles( 4,  3,  8, 0x0,  4,  8,  9, 0x0),
    gsSP2Triangles( 6, 10, 11, 0x0,  7, 10,  6, 0x0),
    gsSP2Triangles( 6, 11,  8, 0x0,  8, 11,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x060086D0 - 0x06008740
const Gfx koopa_seg6_dl_060086D0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(koopa_seg6_dl_06008530),
    gsSPDisplayList(koopa_seg6_dl_060085E8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(koopa_seg6_dl_06008640),
    gsSPEndDisplayList(),
};

// 0x06008740

// 0x06008758

// 0x06008770
static const Vtx koopa_seg6_vertex_06008770[] = {
#include "assets/rom/generated/eb853e9b379b5cd67633.inc.c"















};

// 0x06008870
static const Vtx koopa_seg6_vertex_06008870[] = {
#include "assets/rom/generated/4f57f292e4cc5cce6661.inc.c"







};

// 0x060088F0
static const Vtx koopa_seg6_vertex_060088F0[] = {
#include "assets/rom/generated/053d2344fbfa33cb328e.inc.c"











};

// 0x060089B0 - 0x06008A68
const Gfx koopa_seg6_dl_060089B0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, koopa_seg6_texture_06003648),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(koopa_seg6_vertex_06008770, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 1,  4,  3, 0x0,  4,  5,  3, 0x0),
    gsSP2Triangles( 0,  2,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  5,  7, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 9, 10,  8, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 11, 13, 10, 0x0),
    gsSP2Triangles(12, 14, 13, 0x0, 12, 15, 14, 0x0),
    gsSP2Triangles( 6, 14, 15, 0x0,  6, 15,  0, 0x0),
    gsSPEndDisplayList(),
};

// 0x06008A68 - 0x06008AC0
const Gfx koopa_seg6_dl_06008A68[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, koopa_seg6_texture_06003E48),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(koopa_seg6_vertex_06008870, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 2,  4,  0, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 5,  6,  1, 0x0,  5,  7,  6, 0x0),
    gsSPEndDisplayList(),
};

// 0x06008AC0 - 0x06008B50
const Gfx koopa_seg6_dl_06008AC0[] = {
    gsSPLightColor(LIGHT_1, 0xc800ff),
    gsSPLightColor(LIGHT_2, 0x6400ff),
    gsSPVertex(koopa_seg6_vertex_060088F0, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 0,  2,  4, 0x0,  5,  0,  6, 0x0),
    gsSP2Triangles( 5,  3,  0, 0x0,  4,  6,  0, 0x0),
    gsSP2Triangles( 1,  7,  2, 0x0,  3,  8,  1, 0x0),
    gsSP2Triangles( 7,  1,  8, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 3, 10, 11, 0x0,  3, 11,  8, 0x0),
    gsSP2Triangles( 5, 10,  3, 0x0,  8, 11,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x06008B50 - 0x06008BC0
const Gfx koopa_seg6_dl_06008B50[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(koopa_seg6_dl_060089B0),
    gsSPDisplayList(koopa_seg6_dl_06008A68),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(koopa_seg6_dl_06008AC0),
    gsSPEndDisplayList(),
};

// 0x06008BC0
static const Vtx koopa_seg6_vertex_06008BC0[] = {
#include "assets/rom/generated/8a960558eb9cb9eb861e.inc.c"















};

// 0x06008CC0
static const Vtx koopa_seg6_vertex_06008CC0[] = {
#include "assets/rom/generated/773b4be5be7c07bd2c5a.inc.c"













};

// 0x06008DA0
static const Vtx koopa_seg6_vertex_06008DA0[] = {
#include "assets/rom/generated/d8140c3afc8d78f71772.inc.c"













};

// 0x06008E80
static const Vtx koopa_seg6_vertex_06008E80[] = {
#include "assets/rom/generated/ac11260a7fd432b60cca.inc.c"













};

// 0x06008F60
static const Vtx koopa_seg6_vertex_06008F60[] = {
#include "assets/rom/generated/2a19b5b00addc0634d2e.inc.c"













};

// 0x06009040
static const Vtx koopa_seg6_vertex_06009040[] = {
#include "assets/rom/generated/5db4f779bc535340aa5a.inc.c"















};

// 0x06009140
static const Vtx koopa_seg6_vertex_06009140[] = {
#include "assets/rom/generated/84db01bef96977f752fb.inc.c"















};

// 0x06009240
static const Vtx koopa_seg6_vertex_06009240[] = {
#include "assets/rom/generated/5a0af307f7a201a3db18.inc.c"















};

// 0x06009340
static const Vtx koopa_seg6_vertex_06009340[] = {
#include "assets/rom/generated/7225a26bc417a97e02c8.inc.c"












};

// 0x06009410
static const Vtx koopa_seg6_vertex_06009410[] = {
#include "assets/rom/generated/97507179998806d1ad05.inc.c"













};

// 0x060094F0
static const Vtx koopa_seg6_vertex_060094F0[] = {
#include "assets/rom/generated/0e4022d96d0645ac1954.inc.c"













};

// 0x060095D0
static const Vtx koopa_seg6_vertex_060095D0[] = {
#include "assets/rom/generated/a0790d28cdaae9f76e8f.inc.c"














};

// 0x060096C0
static const Vtx koopa_seg6_vertex_060096C0[] = {
#include "assets/rom/generated/c1545b9c005272072363.inc.c"














};

// 0x060097B0
static const Vtx koopa_seg6_vertex_060097B0[] = {
#include "assets/rom/generated/da3101f6418bb63a241f.inc.c"














};

// 0x060098A0
static const Vtx koopa_seg6_vertex_060098A0[] = {
#include "assets/rom/generated/e4db3ba44de34731afe2.inc.c"















};

// 0x060099A0
static const Vtx koopa_seg6_vertex_060099A0[] = {
#include "assets/rom/generated/c603a25abfe9f0214426.inc.c"








};

// 0x06009A30
static const Vtx koopa_seg6_vertex_06009A30[] = {
#include "assets/rom/generated/aa32031266749c0c475a.inc.c"









};

// 0x06009AD0
static const Vtx koopa_seg6_vertex_06009AD0[] = {
#include "assets/rom/generated/b37cd8cbf3dd6eb11871.inc.c"













};

// 0x06009BB0
static const Vtx koopa_seg6_vertex_06009BB0[] = {
#include "assets/rom/generated/f812940618b7b07614ea.inc.c"














};

// 0x06009CA0
static const Vtx koopa_seg6_vertex_06009CA0[] = {
#include "assets/rom/generated/a73a98fea71f220e3a94.inc.c"














};

// 0x06009D90
static const Vtx koopa_seg6_vertex_06009D90[] = {
#include "assets/rom/generated/5e1da9e89560ef2ee6a0.inc.c"














};

// 0x06009E80
static const Vtx koopa_seg6_vertex_06009E80[] = {
#include "assets/rom/generated/9337b7a4a89e412f6ea1.inc.c"















};

// 0x06009F80
static const Vtx koopa_seg6_vertex_06009F80[] = {
#include "assets/rom/generated/a19e83fac4d04dc2461b.inc.c"








};

// 0x0600A010
static const Vtx koopa_seg6_vertex_0600A010[] = {
#include "assets/rom/generated/996269b911339ed57f66.inc.c"









};

// 0x0600A0B0
static const Vtx koopa_seg6_vertex_0600A0B0[] = {
#include "assets/rom/generated/f46e23fe73caddb1e2fc.inc.c"














};

// 0x0600A1A0
static const Vtx koopa_seg6_vertex_0600A1A0[] = {
#include "assets/rom/generated/d0d3368a0c1aca6aec97.inc.c"















};

// 0x0600A2A0
static const Vtx koopa_seg6_vertex_0600A2A0[] = {
#include "assets/rom/generated/92f757540ed66a962743.inc.c"















};

// 0x0600A3A0
static const Vtx koopa_seg6_vertex_0600A3A0[] = {
#include "assets/rom/generated/558c490fc0b5e76fb0c8.inc.c"















};

// 0x0600A4A0
static const Vtx koopa_seg6_vertex_0600A4A0[] = {
#include "assets/rom/generated/6858d401608b12c810a8.inc.c"















};

// 0x0600A5A0
static const Vtx koopa_seg6_vertex_0600A5A0[] = {
#include "assets/rom/generated/be36a26e5b4c610f0d7e.inc.c"














};

// 0x0600A690
static const Vtx koopa_seg6_vertex_0600A690[] = {
#include "assets/rom/generated/34dbf0716596cb5a9f9f.inc.c"















};

// 0x0600A790
static const Vtx koopa_seg6_vertex_0600A790[] = {
#include "assets/rom/generated/01534a1d85a65b8a995a.inc.c"















};

// 0x0600A890
static const Vtx koopa_seg6_vertex_0600A890[] = {
#include "assets/rom/generated/9bea11c35c4f4b0c10a3.inc.c"















};

// 0x0600A990
static const Vtx koopa_seg6_vertex_0600A990[] = {
#include "assets/rom/generated/94256d81fa177fec158c.inc.c"


};

// 0x0600A9C0
static const Vtx koopa_seg6_vertex_0600A9C0[] = {
#include "assets/rom/generated/0928bf345e998cad2d4b.inc.c"















};

// 0x0600AAC0
static const Vtx koopa_seg6_vertex_0600AAC0[] = {
#include "assets/rom/generated/8b80a51660034a98818d.inc.c"














};

// 0x0600ABB0
static const Vtx koopa_seg6_vertex_0600ABB0[] = {
#include "assets/rom/generated/ecbbbb8795ef91a35d55.inc.c"













};

// 0x0600AC90
static const Vtx koopa_seg6_vertex_0600AC90[] = {
#include "assets/rom/generated/708da19313f971896a2b.inc.c"















};

// 0x0600AD90
static const Vtx koopa_seg6_vertex_0600AD90[] = {
#include "assets/rom/generated/722159c61e3fa1d561b0.inc.c"















};

// 0x0600AE90
static const Vtx koopa_seg6_vertex_0600AE90[] = {
#include "assets/rom/generated/565164e3b8fb95842216.inc.c"












};

// 0x0600AF60
static const Vtx koopa_seg6_vertex_0600AF60[] = {
#include "assets/rom/generated/31765db22e21dab1d7d4.inc.c"









};

// 0x0600B000
static const Vtx koopa_seg6_vertex_0600B000[] = {
#include "assets/rom/generated/f945873f0dced83d434a.inc.c"













};

// 0x0600B0E0
static const Vtx koopa_seg6_vertex_0600B0E0[] = {
#include "assets/rom/generated/f2ec29054cbc0816033f.inc.c"










};

// 0x0600B190
static const Vtx koopa_seg6_vertex_0600B190[] = {
#include "assets/rom/generated/27de03d7163f17d9744e.inc.c"













};

// 0x0600B270
static const Vtx koopa_seg6_vertex_0600B270[] = {
#include "assets/rom/generated/75035d0679ea4c5959ac.inc.c"















};

// 0x0600B370
static const Vtx koopa_seg6_vertex_0600B370[] = {
#include "assets/rom/generated/c06a9047745a3fcadad7.inc.c"















};

// 0x0600B470
static const Vtx koopa_seg6_vertex_0600B470[] = {
#include "assets/rom/generated/d965bdfd8284c931a355.inc.c"














};

// 0x0600B560
static const Vtx koopa_seg6_vertex_0600B560[] = {
#include "assets/rom/generated/80ae25ba074e6fb37daa.inc.c"








};

// 0x0600B5F0
static const Vtx koopa_seg6_vertex_0600B5F0[] = {
#include "assets/rom/generated/47aa1e3ce2c1887f7d2b.inc.c"














};

// 0x0600B6E0 - 0x0600B838
const Gfx koopa_seg6_dl_0600B6E0[] = {
    gsSPLightColor(LIGHT_1, 0xb200ff),
    gsSPLightColor(LIGHT_2, 0x5900ff),
    gsSPVertex(koopa_seg6_vertex_06008BC0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(koopa_seg6_vertex_06008CC0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  2,  7,  8, 0x0),
    gsSP2Triangles( 9, 10,  4, 0x0,  9,  4,  6, 0x0),
    gsSP2Triangles( 2,  8, 11, 0x0,  2, 11,  3, 0x0),
    gsSP2Triangles( 3, 12, 13, 0x0,  3, 13,  0, 0x0),
    gsSPVertex(koopa_seg6_vertex_06008DA0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9,  5, 0x0,  8,  5, 10, 0x0),
    gsSP2Triangles(11, 12,  9, 0x0, 11,  9, 13, 0x0),
    gsSPVertex(koopa_seg6_vertex_06008E80, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 6,  7,  9, 0x0,  6,  9,  4, 0x0),
    gsSP2Triangles(10,  0,  3, 0x0, 10,  3, 11, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 11, 13, 10, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600B838 - 0x0600B8B8
const Gfx koopa_seg6_dl_0600B838[] = {
    gsSPLightColor(LIGHT_1, 0xe0ae00ff),
    gsSPLightColor(LIGHT_2, 0x705700ff),
    gsSPVertex(koopa_seg6_vertex_06008F60, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  7, 10, 11, 0x0),
    gsSP2Triangles(12,  4,  6, 0x0, 12,  6, 13, 0x0),
    gsSP2Triangles( 5,  0,  3, 0x0,  5,  3,  6, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600B8B8 - 0x0600BA10
const Gfx koopa_seg6_dl_0600B8B8[] = {
    gsSPLightColor(LIGHT_1, 0xb200ff),
    gsSPLightColor(LIGHT_2, 0x5900ff),
    gsSPVertex(koopa_seg6_vertex_06009040, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(koopa_seg6_vertex_06009140, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  1, 0x0),
    gsSP2Triangles( 4,  6,  9, 0x0,  4,  9, 10, 0x0),
    gsSP2Triangles( 0, 11,  7, 0x0,  0,  7,  1, 0x0),
    gsSP2Triangles( 3, 12, 13, 0x0,  3, 13,  0, 0x0),
    gsSP1Triangle(10, 14, 15, 0x0),
    gsSPVertex(koopa_seg6_vertex_06009240, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  5,  8, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0, 10,  8, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(koopa_seg6_vertex_06009340, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  5,  6,  8, 0x0),
    gsSP2Triangles( 5,  8,  3, 0x0,  9,  0,  2, 0x0),
    gsSP2Triangles( 9,  2, 10, 0x0, 10, 11, 12, 0x0),
    gsSP1Triangle(10, 12,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600BA10 - 0x0600BA90
const Gfx koopa_seg6_dl_0600BA10[] = {
    gsSPLightColor(LIGHT_1, 0xe0ae00ff),
    gsSPLightColor(LIGHT_2, 0x705700ff),
    gsSPVertex(koopa_seg6_vertex_06009410, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  7, 10, 11, 0x0),
    gsSP2Triangles(12,  4,  6, 0x0, 12,  6, 13, 0x0),
    gsSP2Triangles( 4,  0,  3, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600BA90 - 0x0600BC18
const Gfx koopa_seg6_dl_0600BA90[] = {
    gsSPLightColor(LIGHT_1, 0xe0ae00ff),
    gsSPLightColor(LIGHT_2, 0x705700ff),
    gsSPVertex(koopa_seg6_vertex_060094F0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(koopa_seg6_vertex_060095D0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  3,  9, 10, 0x0),
    gsSP2Triangles(10,  9,  0, 0x0, 11,  1, 12, 0x0),
    gsSP1Triangle( 1, 13, 14, 0x0),
    gsSPVertex(koopa_seg6_vertex_060096C0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  9,  8, 0x0),
    gsSP2Triangles( 2, 10, 11, 0x0, 12,  3,  5, 0x0),
    gsSP2Triangles(12,  8, 13, 0x0, 14,  0, 11, 0x0),
    gsSPVertex(koopa_seg6_vertex_060097B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  5,  8, 0x0),
    gsSP2Triangles( 9,  8, 10, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(10,  4, 12, 0x0, 12, 13, 10, 0x0),
    gsSP1Triangle( 8, 14,  7, 0x0),
    gsSPVertex(koopa_seg6_vertex_060098A0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10,  0, 0x0),
    gsSP2Triangles(11, 12,  7, 0x0, 11,  7, 13, 0x0),
    gsSP1Triangle(14, 15,  6, 0x0),
    gsSPVertex(koopa_seg6_vertex_060099A0, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  7,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600BC18 - 0x0600BC88
const Gfx koopa_seg6_dl_0600BC18[] = {
    gsSPLightColor(LIGHT_1, 0xe0ae00ff),
    gsSPLightColor(LIGHT_2, 0x705700ff),
    gsSPVertex(koopa_seg6_vertex_06009A30, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  1,  5,  2, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 6,  8,  9, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 8,  0,  3, 0x0,  8,  3,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600BC88 - 0x0600BE10
const Gfx koopa_seg6_dl_0600BC88[] = {
    gsSPLightColor(LIGHT_1, 0xe0ae00ff),
    gsSPLightColor(LIGHT_2, 0x705700ff),
    gsSPVertex(koopa_seg6_vertex_06009AD0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(koopa_seg6_vertex_06009BB0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10,  5, 0x0),
    gsSP2Triangles( 2, 10,  9, 0x0, 11,  1, 12, 0x0),
    gsSP1Triangle(13, 14,  1, 0x0),
    gsSPVertex(koopa_seg6_vertex_06009CA0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11,  0, 0x0,  3,  5, 12, 0x0),
    gsSP2Triangles(13,  6, 12, 0x0, 10,  2, 14, 0x0),
    gsSPVertex(koopa_seg6_vertex_06009D90, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  4,  8, 0x0),
    gsSP2Triangles( 9, 10,  7, 0x0,  9,  7, 11, 0x0),
    gsSP2Triangles(12,  5, 10, 0x0, 10, 13, 12, 0x0),
    gsSP1Triangle( 8, 14,  7, 0x0),
    gsSPVertex(koopa_seg6_vertex_06009E80, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  2,  9, 10, 0x0),
    gsSP2Triangles(11,  7, 12, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle( 8, 14, 15, 0x0),
    gsSPVertex(koopa_seg6_vertex_06009F80, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  7,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600BE10 - 0x0600BE80
const Gfx koopa_seg6_dl_0600BE10[] = {
    gsSPLightColor(LIGHT_1, 0xe0ae00ff),
    gsSPLightColor(LIGHT_2, 0x705700ff),
    gsSPVertex(koopa_seg6_vertex_0600A010, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  1,  5,  2, 0x0),
    gsSP2Triangles( 4,  6,  7, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 6,  8,  9, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles( 8,  0,  3, 0x0,  8,  3,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600BE80 - 0x0600C240
const Gfx koopa_seg6_dl_0600BE80[] = {
    gsSPLightColor(LIGHT_1, 0xe0ae00ff),
    gsSPLightColor(LIGHT_2, 0x705700ff),
    gsSPVertex(koopa_seg6_vertex_0600A0B0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 5,  9,  2, 0x0,  3,  2,  9, 0x0),
    gsSP2Triangles(10,  6,  8, 0x0, 11,  5,  4, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600A1A0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  3,  8, 0x0),
    gsSP2Triangles( 1,  9,  8, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  2, 14, 15, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600A2A0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle(12, 14, 15, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600A3A0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 9, 11,  6, 0x0, 12, 13, 14, 0x0),
    gsSP2Triangles(14, 13,  9, 0x0,  7, 11, 15, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600A4A0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  6,  1, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 5, 10, 11, 0x0, 12, 13, 14, 0x0),
    gsSP1Triangle(15,  7, 14, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600A5A0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles(11, 12, 10, 0x0, 12, 13, 14, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600A690, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  6,  5, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  9, 12, 10, 0x0),
    gsSP2Triangles(13, 14,  2, 0x0, 13, 15, 14, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600A790, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 4,  5,  3, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9, 10,  8, 0x0, 11,  7,  6, 0x0),
    gsSP2Triangles(11,  9,  7, 0x0, 10,  9, 12, 0x0),
    gsSP2Triangles(12,  9, 13, 0x0,  2, 10, 12, 0x0),
    gsSP2Triangles( 2,  1, 10, 0x0, 13, 14, 15, 0x0),
    gsSP1Triangle(15, 12, 13, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600A890, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0, 10, 12, 11, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600A990, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(koopa_seg6_vertex_0600A9C0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  2,  9, 10, 0x0),
    gsSP2Triangles( 4,  0, 11, 0x0, 12, 13,  5, 0x0),
    gsSP2Triangles( 6,  8, 12, 0x0, 11,  0, 14, 0x0),
    gsSP2Triangles( 6, 15,  7, 0x0, 10, 14,  2, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600AAC0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  8,  9, 10, 0x0),
    gsSP2Triangles( 0,  2, 10, 0x0, 11,  1,  0, 0x0),
    gsSP2Triangles(12,  8, 10, 0x0,  3, 13, 14, 0x0),
    gsSP2Triangles(10,  2, 12, 0x0,  7,  3,  5, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600ABB0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  4, 0x0),
    gsSP2Triangles( 5,  6,  2, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10,  3, 11, 0x0,  0, 12, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600C240 - 0x0600C498
const Gfx koopa_seg6_dl_0600C240[] = {
    gsSPLightColor(LIGHT_1, 0xb2b2b2ff),
    gsSPLightColor(LIGHT_2, 0x595959ff),
    gsSPVertex(koopa_seg6_vertex_0600AC90, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  5,  4, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  8,  3,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0,  9, 13, 12, 0x0),
    gsSP2Triangles( 5,  9, 14, 0x0,  6,  5, 14, 0x0),
    gsSP2Triangles(14, 15,  6, 0x0, 12, 14,  9, 0x0),
    gsSP2Triangles(12, 13, 10, 0x0,  7,  9,  5, 0x0),
    gsSP1Triangle( 3,  2,  9, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600AD90, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9,  7, 10, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0,  0, 14,  1, 0x0),
    gsSP1Triangle( 9, 15,  1, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600AE90, 13, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  3,  0, 0x0),
    gsSP2Triangles( 2,  4,  3, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 4,  7,  8, 0x0,  4,  8,  9, 0x0),
    gsSP2Triangles( 3,  4,  9, 0x0,  0,  3, 10, 0x0),
    gsSP1Triangle(11, 12,  7, 0x0),
    gsSPLightColor(LIGHT_1, 0xa900ff),
    gsSPLightColor(LIGHT_2, 0x5400ff),
    gsSPVertex(koopa_seg6_vertex_0600AF60, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  3,  5, 0x0,  6,  5,  1, 0x0),
    gsSP2Triangles( 7,  5,  4, 0x0,  2,  1,  5, 0x0),
    gsSP2Triangles( 3,  8,  9, 0x0,  4,  3,  9, 0x0),
    gsSP2Triangles( 7,  2,  5, 0x0,  6,  0,  8, 0x0),
    gsSP2Triangles( 8,  3,  6, 0x0,  6,  1,  0, 0x0),
    gsSPLightColor(LIGHT_1, 0xe0ae00ff),
    gsSPLightColor(LIGHT_2, 0x705700ff),
    gsSPVertex(koopa_seg6_vertex_0600B000, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  3,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 1, 10,  2, 0x0,  0, 11, 12, 0x0),
    gsSP2Triangles( 6,  7,  9, 0x0,  0, 12,  1, 0x0),
    gsSP2Triangles( 2,  5,  4, 0x0,  9,  4,  6, 0x0),
    gsSP2Triangles( 5,  2, 10, 0x0,  9,  0,  2, 0x0),
    gsSP1Triangle( 8, 13, 11, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600B0E0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  0, 0x0),
    gsSP2Triangles( 2,  3,  0, 0x0,  5,  6,  7, 0x0),
    gsSP2Triangles( 7,  8,  5, 0x0,  9, 10,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x0600C498 - 0x0600C6A0
const Gfx koopa_seg6_dl_0600C498[] = {
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
    gsSPVertex(koopa_seg6_vertex_0600B190, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600B270, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 2,  1,  9, 0x0,  2,  9, 12, 0x0),
    gsSP2Triangles( 7, 13,  8, 0x0, 14, 15, 12, 0x0),
    gsSP2Triangles(14, 12,  8, 0x0,  3,  2, 15, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600B370, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0, 12,  1, 13, 0x0),
    gsSP2Triangles(14,  9, 11, 0x0, 14, 11, 15, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600B470, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  1, 0x0),
    gsSP2Triangles( 7,  1,  0, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(koopa_seg6_vertex_0600B560, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP1Triangle( 6,  7,  8, 0x0),
#ifdef KOOPA_KEEP_PINK_SHORTS
    gsSPLightColor(LIGHT_1, 0xde01deff),
    gsSPLightColor(LIGHT_2, 0xdec1deff),
#endif
    gsSPVertex(koopa_seg6_vertex_0600B5F0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  0, 0x0,  8,  5,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  3,  5,  8, 0x0),
    gsSP2Triangles( 6,  0,  4, 0x0,  6,  4,  3, 0x0),
    gsSP2Triangles( 6,  3, 11, 0x0,  9,  5,  4, 0x0),
    gsSP2Triangles( 9,  4,  0, 0x0, 12, 13,  7, 0x0),
    gsSP2Triangles(12,  7,  6, 0x0,  1,  7, 13, 0x0),
    gsSP2Triangles(10,  9,  2, 0x0,  0,  2,  9, 0x0),
    gsSP2Triangles( 0,  7,  1, 0x0,  6, 14, 12, 0x0),
    gsSPEndDisplayList(),
};
