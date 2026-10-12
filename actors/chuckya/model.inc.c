// Chuckya

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// 0x08006778
ALIGNED8 static const Texture chuckya_seg8_texture_08006778[] = {
#include "actors/chuckya/chuckya_eyes.rgba16.inc.c"
};

// 0x08007778
ALIGNED8 static const Texture chuckya_seg8_texture_08007778[] = {
#include "actors/chuckya/chuckya_hand_antenna.rgba16.inc.c"
};

// 0x08007F78
ALIGNED8 static const Texture chuckya_seg8_texture_08007F78[] = {
#include "actors/chuckya/chuckya_body_arm_left_side.rgba16.inc.c"
};

// 0x08008F78
ALIGNED8 static const Texture chuckya_seg8_texture_08008F78[] = {
#include "actors/chuckya/chuckya_body_arm_right_side.rgba16.inc.c"
};

// 0x08009F78
static const Vtx chuckya_seg8_vertex_08009F78[] = {
#include "assets/rom/generated/7dd386e6dca9026ac27a.inc.c"



};

// 0x08009FB8
static const Vtx chuckya_seg8_vertex_08009FB8[] = {
#include "assets/rom/generated/412f7fe528e1cba7efaa.inc.c"



};

// 0x08009FF8 - 0x0800A030
const Gfx chuckya_seg8_dl_08009FF8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, chuckya_seg8_texture_08007F78),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(chuckya_seg8_vertex_08009F78, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0800A030 - 0x0800A068
const Gfx chuckya_seg8_dl_0800A030[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, chuckya_seg8_texture_08008F78),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(chuckya_seg8_vertex_08009FB8, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0800A068 - 0x0800A0E0
const Gfx chuckya_seg8_dl_0800A068[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(chuckya_seg8_dl_08009FF8),
    gsSPDisplayList(chuckya_seg8_dl_0800A030),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0800A0E0
static const Vtx chuckya_seg8_vertex_0800A0E0[] = {
#include "assets/rom/generated/9419e886d1cab37cdf89.inc.c"



};

// 0x0800A120
static const Vtx chuckya_seg8_vertex_0800A120[] = {
#include "assets/rom/generated/8b5c4cefa41d9a06e428.inc.c"



};

// 0x0800A160 - 0x0800A198
const Gfx chuckya_seg8_dl_0800A160[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, chuckya_seg8_texture_08007F78),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(chuckya_seg8_vertex_0800A0E0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0800A198 - 0x0800A1D0
const Gfx chuckya_seg8_dl_0800A198[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, chuckya_seg8_texture_08008F78),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(chuckya_seg8_vertex_0800A120, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0800A1D0 - 0x0800A248
const Gfx chuckya_seg8_dl_0800A1D0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(chuckya_seg8_dl_0800A160),
    gsSPDisplayList(chuckya_seg8_dl_0800A198),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0800A248
static const Vtx chuckya_seg8_vertex_0800A248[] = {
#include "assets/rom/generated/0058defb935d6f198b82.inc.c"



};

// 0x0800A288
static const Vtx chuckya_seg8_vertex_0800A288[] = {
#include "assets/rom/generated/cd527d35234e8c2ee9cd.inc.c"



};

// 0x0800A2C8 - 0x0800A300
const Gfx chuckya_seg8_dl_0800A2C8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, chuckya_seg8_texture_08007F78),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(chuckya_seg8_vertex_0800A248, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0800A300 - 0x0800A338
const Gfx chuckya_seg8_dl_0800A300[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, chuckya_seg8_texture_08008F78),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 64 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(chuckya_seg8_vertex_0800A288, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0800A338 - 0x0800A3B0
const Gfx chuckya_seg8_dl_0800A338[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 6, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (64 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(chuckya_seg8_dl_0800A2C8),
    gsSPDisplayList(chuckya_seg8_dl_0800A300),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0800A3B0
static const Vtx chuckya_seg8_vertex_0800A3B0[] = {
#include "assets/rom/generated/e8fc1895eb6a7e340b6c.inc.c"



};

// 0x0800A3F0 - 0x0800A428
const Gfx chuckya_seg8_dl_0800A3F0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, chuckya_seg8_texture_08007778),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(chuckya_seg8_vertex_0800A3B0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0800A428 - 0x0800A498
const Gfx chuckya_seg8_dl_0800A428[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(chuckya_seg8_dl_0800A3F0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0800A498
static const Vtx chuckya_seg8_vertex_0800A498[] = {
#include "assets/rom/generated/4c59203bd17dfdafb621.inc.c"



};

// 0x0800A4D8 - 0x0800A510
const Gfx chuckya_seg8_dl_0800A4D8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, chuckya_seg8_texture_08007778),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(chuckya_seg8_vertex_0800A498, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0800A510 - 0x0800A580
const Gfx chuckya_seg8_dl_0800A510[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(chuckya_seg8_dl_0800A4D8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0800A580
static const Vtx chuckya_seg8_vertex_0800A580[] = {
#include "assets/rom/generated/4176d3e32f7865c35e25.inc.c"



};

// 0x0800A5C0 - 0x0800A5F8
const Gfx chuckya_seg8_dl_0800A5C0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, chuckya_seg8_texture_08007778),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(chuckya_seg8_vertex_0800A580, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x0800A5F8 - 0x0800A668
const Gfx chuckya_seg8_dl_0800A5F8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(chuckya_seg8_dl_0800A5C0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0800A668

// 0x0800A680
static const Vtx chuckya_seg8_vertex_0800A680[] = {
#include "assets/rom/generated/d0c170a7b64e93a4b6b9.inc.c"







};

// 0x0800A700 - 0x0800A758
const Gfx chuckya_seg8_dl_0800A700[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, chuckya_seg8_texture_08006778),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x666666ff),
    gsSPVertex(chuckya_seg8_vertex_0800A680, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSPEndDisplayList(),
};

// 0x0800A758 - 0x0800A7C8
const Gfx chuckya_seg8_dl_0800A758[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(chuckya_seg8_dl_0800A700),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x0800A7C8

// 0x0800A7E0
static const Vtx chuckya_seg8_vertex_0800A7E0[] = {
#include "assets/rom/generated/592e0fe783268d556ca5.inc.c"








};

// 0x0800A870 - 0x0800A8D0
const Gfx chuckya_seg8_dl_0800A870[] = {
    gsSPLightColor(LIGHT_1, 0x89898aff),
    gsSPLightColor(LIGHT_2, 0x292929ff),
    gsSPVertex(chuckya_seg8_vertex_0800A7E0, 9, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSP2Triangles( 1,  4,  2, 0x0,  5,  6,  2, 0x0),
    gsSP2Triangles( 6,  7,  2, 0x0,  8,  5,  2, 0x0),
    gsSP2Triangles( 7,  3,  2, 0x0,  4,  8,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x0800A8D0 - 0x0800A8F0
const Gfx chuckya_seg8_dl_0800A8D0[] = {
    gsDPPipeSync(),
    gsSPDisplayList(chuckya_seg8_dl_0800A870),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

// 0x0800A8F0

// 0x0800A908
static const Vtx chuckya_seg8_vertex_0800A908[] = {
#include "assets/rom/generated/6355171f42f5da686d1f.inc.c"




};

// 0x0800A958 - 0x0800A998
const Gfx chuckya_seg8_dl_0800A958[] = {
    gsSPLightColor(LIGHT_1, 0xffff00ff),
    gsSPLightColor(LIGHT_2, 0x4c4c00ff),
    gsSPVertex(chuckya_seg8_vertex_0800A908, 5, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  2,  3, 0x0,  0,  4,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0800A998 - 0x0800A9B8
const Gfx chuckya_seg8_dl_0800A998[] = {
    gsDPPipeSync(),
    gsSPDisplayList(chuckya_seg8_dl_0800A958),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

// 0x0800A9B8

// 0x0800A9D0
static const Vtx chuckya_seg8_vertex_0800A9D0[] = {
#include "assets/rom/generated/516f534b3e4ad00c564f.inc.c"














};

// 0x0800AAC0
static const Vtx chuckya_seg8_vertex_0800AAC0[] = {
#include "assets/rom/generated/9923e7256019adefde4c.inc.c"










};

// 0x0800AB70 - 0x0800ABE8
const Gfx chuckya_seg8_dl_0800AB70[] = {
    gsSPLightColor(LIGHT_1, 0x323232ff),
    gsSPLightColor(LIGHT_2, 0xc0c0cff),
    gsSPVertex(chuckya_seg8_vertex_0800A9D0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(chuckya_seg8_vertex_0800AAC0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP1Triangle( 7, 10,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x0800ABE8 - 0x0800AC18
const Gfx chuckya_seg8_dl_0800ABE8[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsSPDisplayList(chuckya_seg8_dl_0800AB70),
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
