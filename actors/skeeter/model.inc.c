// Skeeter

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// Unreferenced light group

// 0x06000090
ALIGNED8 static const Texture skeeter_seg6_texture_06000090[] = {
#include "actors/skeeter/skeeter_eye.rgba16.inc.c"
};

// 0x06000890
ALIGNED8 static const Texture skeeter_seg6_texture_06000890[] = {
#include "actors/skeeter/skeeter_iris.rgba16.inc.c"
};

// 0x06000990
static const Vtx skeeter_seg6_vertex_06000990[] = {
#include "assets/rom/generated/0181895f9327258b6e12.inc.c"



};

// 0x060009D0 - 0x06000A08
const Gfx skeeter_seg6_dl_060009D0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, skeeter_seg6_texture_06000090),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(skeeter_seg6_vertex_06000990, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x06000A08 - 0x06000A78
const Gfx skeeter_seg6_dl_06000A08[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(skeeter_seg6_dl_060009D0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x06000A78
static const Vtx skeeter_seg6_vertex_06000A78[] = {
#include "assets/rom/generated/722e963a4b4cc0e0d388.inc.c"



};

// 0x06000AB8 - 0x06000AF0
const Gfx skeeter_seg6_dl_06000AB8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, skeeter_seg6_texture_06000090),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(skeeter_seg6_vertex_06000A78, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x06000AF0 - 0x06000B60
const Gfx skeeter_seg6_dl_06000AF0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(skeeter_seg6_dl_06000AB8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x06000B60
static const Vtx skeeter_seg6_vertex_06000B60[] = {
#include "assets/rom/generated/52a323779633bb918c95.inc.c"



};

// 0x06000BA0 - 0x06000BD8
const Gfx skeeter_seg6_dl_06000BA0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, skeeter_seg6_texture_06000090),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(skeeter_seg6_vertex_06000B60, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x06000BD8 - 0x06000C48
const Gfx skeeter_seg6_dl_06000BD8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(skeeter_seg6_dl_06000BA0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x06000C48
static const Vtx skeeter_seg6_vertex_06000C48[] = {
#include "assets/rom/generated/d7c15351146a1a0c36a3.inc.c"


};

// 0x06000C78 - 0x06000CA8
const Gfx skeeter_seg6_dl_06000C78[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, skeeter_seg6_texture_06000890),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 8 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(skeeter_seg6_vertex_06000C48, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x06000CA8 - 0x06000D18
const Gfx skeeter_seg6_dl_06000CA8[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 3, G_TX_NOLOD, G_TX_CLAMP, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (8 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(skeeter_seg6_dl_06000C78),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x06000D18
static const Vtx skeeter_seg6_vertex_06000D18[] = {
#include "assets/rom/generated/a66d79ed495bc092dc53.inc.c"


};

// 0x06000D48 - 0x06000D78
const Gfx skeeter_seg6_dl_06000D48[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, skeeter_seg6_texture_06000890),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 16 * 8 - 1, CALC_DXT(16, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(skeeter_seg6_vertex_06000D18, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x06000D78 - 0x06000DE8
const Gfx skeeter_seg6_dl_06000D78[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 4, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 3, G_TX_NOLOD, G_TX_CLAMP, 4, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (16 - 1) << G_TEXTURE_IMAGE_FRAC, (8 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(skeeter_seg6_dl_06000D48),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x06000DE8

// 0x06000E00
static const Vtx skeeter_seg6_vertex_06000E00[] = {
#include "assets/rom/generated/b4af564dda63b5af69e7.inc.c"





};

// 0x06000E60 - 0x06000EC0
const Gfx skeeter_seg6_dl_06000E60[] = {
    gsSPLightColor(LIGHT_1, 0xffaa00ff),
    gsSPLightColor(LIGHT_2, 0x7f5500ff),
    gsSPVertex(skeeter_seg6_vertex_06000E00, 6, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 3,  0,  4, 0x0,  5,  2,  4, 0x0),
    gsSP2Triangles( 0,  5,  4, 0x0,  5,  0,  2, 0x0),
    gsSPEndDisplayList(),
};

// 0x06000EC0 - 0x06000EF0
const Gfx skeeter_seg6_dl_06000EC0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(skeeter_seg6_dl_06000E60),
    gsSPEndDisplayList(),
};

// 0x06000EF0
static const Vtx skeeter_seg6_vertex_06000EF0[] = {
#include "assets/rom/generated/b2c465f827fe87d68e48.inc.c"










};

// 0x06000FA0
static const Vtx skeeter_seg6_vertex_06000FA0[] = {
#include "assets/rom/generated/6974cdece636acf3b688.inc.c"



};

// 0x06000FE0
static const Vtx skeeter_seg6_vertex_06000FE0[] = {
#include "assets/rom/generated/c0943f299635d0201736.inc.c"



};

// 0x06001020
static const Vtx skeeter_seg6_vertex_06001020[] = {
#include "assets/rom/generated/9c7004c17c5598d731ff.inc.c"










};

// 0x060010D0
static const Vtx skeeter_seg6_vertex_060010D0[] = {
#include "assets/rom/generated/c6aa70189aedd0394ad0.inc.c"



};

// 0x06001110
static const Vtx skeeter_seg6_vertex_06001110[] = {
#include "assets/rom/generated/011e27f921aa9a0d53cc.inc.c"



};

// 0x06001150
static const Vtx skeeter_seg6_vertex_06001150[] = {
#include "assets/rom/generated/6f51d9a743e78613e432.inc.c"










};

// 0x06001200
static const Vtx skeeter_seg6_vertex_06001200[] = {
#include "assets/rom/generated/f6fa4a208ef91f48e20c.inc.c"



};

// 0x06001240
static const Vtx skeeter_seg6_vertex_06001240[] = {
#include "assets/rom/generated/19b29223c2b325f3d1ba.inc.c"



};

// 0x06001280
static const Vtx skeeter_seg6_vertex_06001280[] = {
#include "assets/rom/generated/87af4f47d37b9da3aef9.inc.c"















};

// 0x06001380
static const Vtx skeeter_seg6_vertex_06001380[] = {
#include "assets/rom/generated/d17ad9b15f6eaa8d7aa3.inc.c"















};

// 0x06001480
static const Vtx skeeter_seg6_vertex_06001480[] = {
#include "assets/rom/generated/ee096232441b53a72336.inc.c"















};

// 0x06001580
static const Vtx skeeter_seg6_vertex_06001580[] = {
#include "assets/rom/generated/a05fda2e440e29833154.inc.c"















};

// 0x06001680
static const Vtx skeeter_seg6_vertex_06001680[] = {
#include "assets/rom/generated/50972b9efa5bedf62fd1.inc.c"















};

// 0x06001780
static const Vtx skeeter_seg6_vertex_06001780[] = {
#include "assets/rom/generated/d95caae10ab48548376f.inc.c"















};

// 0x06001880
static const Vtx skeeter_seg6_vertex_06001880[] = {
#include "assets/rom/generated/925de05bb1fc1657eea0.inc.c"















};

// 0x06001980
static const Vtx skeeter_seg6_vertex_06001980[] = {
#include "assets/rom/generated/bf08d9034c6a11b1915e.inc.c"















};

// 0x06001A80
static const Vtx skeeter_seg6_vertex_06001A80[] = {
#include "assets/rom/generated/ca7a523c522c0f147407.inc.c"










};

// 0x06001B30
static const Vtx skeeter_seg6_vertex_06001B30[] = {
#include "assets/rom/generated/b6b17a2c9235a17faf79.inc.c"



};

// 0x06001B70
static const Vtx skeeter_seg6_vertex_06001B70[] = {
#include "assets/rom/generated/82283e7398ac3a8c7f52.inc.c"



};

// 0x06001BB0
static const Vtx skeeter_seg6_vertex_06001BB0[] = {
#include "assets/rom/generated/38edc12e4e3c3b60c0f3.inc.c"















};

// 0x06001CB0
static const Vtx skeeter_seg6_vertex_06001CB0[] = {
#include "assets/rom/generated/0cc0bd1745a1f5aea83f.inc.c"















};

// 0x06001DB0
static const Vtx skeeter_seg6_vertex_06001DB0[] = {
#include "assets/rom/generated/807ce0acb93ab395e4ec.inc.c"















};

// 0x06001EB0
static const Vtx skeeter_seg6_vertex_06001EB0[] = {
#include "assets/rom/generated/e59910d69cfc047e85da.inc.c"















};

// 0x06001FB0
static const Vtx skeeter_seg6_vertex_06001FB0[] = {
#include "assets/rom/generated/6b7151a540f2938bc3b1.inc.c"















};

// 0x060020B0
static const Vtx skeeter_seg6_vertex_060020B0[] = {
#include "assets/rom/generated/dc0e5f45d3cfd24d415b.inc.c"















};

// 0x060021B0
static const Vtx skeeter_seg6_vertex_060021B0[] = {
#include "assets/rom/generated/e9d889791d56c12636cf.inc.c"















};

// 0x060022B0
static const Vtx skeeter_seg6_vertex_060022B0[] = {
#include "assets/rom/generated/6bf52d88a32da704e549.inc.c"















};

// 0x060023B0
static const Vtx skeeter_seg6_vertex_060023B0[] = {
#include "assets/rom/generated/d49d064815e7b032f676.inc.c"















};

// 0x060024B0
static const Vtx skeeter_seg6_vertex_060024B0[] = {
#include "assets/rom/generated/0e211a4274681047dba9.inc.c"















};

// 0x060025B0
static const Vtx skeeter_seg6_vertex_060025B0[] = {
#include "assets/rom/generated/8fce4bf0b4d52139b0f6.inc.c"















};

// 0x060026B0
static const Vtx skeeter_seg6_vertex_060026B0[] = {
#include "assets/rom/generated/c3c615bc73faee89962d.inc.c"















};

// 0x060027B0
static const Vtx skeeter_seg6_vertex_060027B0[] = {
#include "assets/rom/generated/9c9e4d1f820f2930f8ee.inc.c"















};

// 0x060028B0
static const Vtx skeeter_seg6_vertex_060028B0[] = {
#include "assets/rom/generated/cabf489cb237190f399a.inc.c"















};

// 0x060029B0
static const Vtx skeeter_seg6_vertex_060029B0[] = {
#include "assets/rom/generated/908c8356cc826c308c02.inc.c"















};

// 0x06002AB0
static const Vtx skeeter_seg6_vertex_06002AB0[] = {
#include "assets/rom/generated/6dcc9d4608af68dfd356.inc.c"















};

// 0x06002BB0
static const Vtx skeeter_seg6_vertex_06002BB0[] = {
#include "assets/rom/generated/7b57bc87508992f3a76a.inc.c"















};

// 0x06002CB0
static const Vtx skeeter_seg6_vertex_06002CB0[] = {
#include "assets/rom/generated/797b826ec3e9f2c338b5.inc.c"















};

// 0x06002DB0
static const Vtx skeeter_seg6_vertex_06002DB0[] = {
#include "assets/rom/generated/526d1c5a7607143b290c.inc.c"















};

// 0x06002EB0
static const Vtx skeeter_seg6_vertex_06002EB0[] = {
#include "assets/rom/generated/54343c230a7f3adc0c44.inc.c"















};

// 0x06002FB0
static const Vtx skeeter_seg6_vertex_06002FB0[] = {
#include "assets/rom/generated/ae5d8710cfc5cd6bbd42.inc.c"















};

// 0x060030B0
static const Vtx skeeter_seg6_vertex_060030B0[] = {
#include "assets/rom/generated/46e969e05d0e86cd744f.inc.c"















};

// 0x060031B0
static const Vtx skeeter_seg6_vertex_060031B0[] = {
#include "assets/rom/generated/72f45e322cb05e2c9064.inc.c"















};

// 0x060032B0
static const Vtx skeeter_seg6_vertex_060032B0[] = {
#include "assets/rom/generated/a20731b7647a1ac050b7.inc.c"















};

// 0x060033B0
static const Vtx skeeter_seg6_vertex_060033B0[] = {
#include "assets/rom/generated/d3fb87fe013011630207.inc.c"















};

// 0x060034B0
static const Vtx skeeter_seg6_vertex_060034B0[] = {
#include "assets/rom/generated/9af73fccc18ada49d0fc.inc.c"















};

// 0x060035B0
static const Vtx skeeter_seg6_vertex_060035B0[] = {
#include "assets/rom/generated/a9e3cbcf2829130d134f.inc.c"















};

// 0x060036B0
static const Vtx skeeter_seg6_vertex_060036B0[] = {
#include "assets/rom/generated/8ca1bfb723cb91a1be97.inc.c"















};

// 0x060037B0
static const Vtx skeeter_seg6_vertex_060037B0[] = {
#include "assets/rom/generated/727da3b20dba1cf67c7f.inc.c"



};

// 0x060037F0
static const Vtx skeeter_seg6_vertex_060037F0[] = {
#include "assets/rom/generated/84fc4f4e33e3eacd1dd9.inc.c"















};

// 0x060038F0
static const Vtx skeeter_seg6_vertex_060038F0[] = {
#include "assets/rom/generated/5d015bfe0771c4cb8f97.inc.c"















};

// 0x060039F0
static const Vtx skeeter_seg6_vertex_060039F0[] = {
#include "assets/rom/generated/c0c2904baf9acfa5690e.inc.c"















};

// 0x06003AF0
static const Vtx skeeter_seg6_vertex_06003AF0[] = {
#include "assets/rom/generated/7596d7a1e17410875e6e.inc.c"















};

// 0x06003BF0
static const Vtx skeeter_seg6_vertex_06003BF0[] = {
#include "assets/rom/generated/6f6b5ddaffcafd0a59d5.inc.c"















};

// 0x06003CF0
static const Vtx skeeter_seg6_vertex_06003CF0[] = {
#include "assets/rom/generated/42bc275cde22617012ec.inc.c"















};

// 0x06003DF0
static const Vtx skeeter_seg6_vertex_06003DF0[] = {
#include "assets/rom/generated/08aed5e7094952c461a0.inc.c"















};

// 0x06003EF0
static const Vtx skeeter_seg6_vertex_06003EF0[] = {
#include "assets/rom/generated/f6c436728225efe69612.inc.c"















};

// 0x06003FF0 - 0x06004040
const Gfx skeeter_seg6_dl_06003FF0[] = {
    gsSPVertex(skeeter_seg6_vertex_06000EF0, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 0,  2,  3, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9, 10,  7, 0x0,  6,  9,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x06004040 - 0x06004070
const Gfx skeeter_seg6_dl_06004040[] = {
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPVertex(skeeter_seg6_vertex_06000FA0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x06004070 - 0x060040A0
const Gfx skeeter_seg6_dl_06004070[] = {
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPVertex(skeeter_seg6_vertex_06000FE0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x060040A0 - 0x060040F0
const Gfx skeeter_seg6_dl_060040A0[] = {
    gsSPVertex(skeeter_seg6_vertex_06001020, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  4, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  0,  5,  1, 0x0),
    gsSP2Triangles( 0,  2,  3, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9, 10,  7, 0x0,  6,  9,  7, 0x0),
    gsSPEndDisplayList(),
};

// 0x060040F0 - 0x06004120
const Gfx skeeter_seg6_dl_060040F0[] = {
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPVertex(skeeter_seg6_vertex_060010D0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x06004120 - 0x06004150
const Gfx skeeter_seg6_dl_06004120[] = {
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPVertex(skeeter_seg6_vertex_06001110, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  0,  2, 0x0),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x06004150 - 0x060041A0
const Gfx skeeter_seg6_dl_06004150[] = {
    gsSPVertex(skeeter_seg6_vertex_06001150, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 5,  3,  2, 0x0,  1,  5,  2, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  7, 10,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x060041A0 - 0x060041D0
const Gfx skeeter_seg6_dl_060041A0[] = {
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPVertex(skeeter_seg6_vertex_06001200, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x060041D0 - 0x06004200
const Gfx skeeter_seg6_dl_060041D0[] = {
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPVertex(skeeter_seg6_vertex_06001240, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x06004200 - 0x060045C8
const Gfx skeeter_seg6_dl_06004200[] = {
    gsSPVertex(skeeter_seg6_vertex_06001280, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06001380, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06001480, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06001580, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06001680, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06001780, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06001880, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06001980, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x060045C8 - 0x06004618
const Gfx skeeter_seg6_dl_060045C8[] = {
    gsSPVertex(skeeter_seg6_vertex_06001A80, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 5,  3,  2, 0x0,  1,  5,  2, 0x0),
    gsSP2Triangles( 4,  0,  2, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  7, 10,  8, 0x0),
    gsSPEndDisplayList(),
};

// 0x06004618 - 0x06004648
const Gfx skeeter_seg6_dl_06004618[] = {
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPVertex(skeeter_seg6_vertex_06001B30, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x06004648 - 0x06004678
const Gfx skeeter_seg6_dl_06004648[] = {
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPVertex(skeeter_seg6_vertex_06001B70, 4, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
    gsSP1Triangle( 0,  2,  3, 0x0),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x06004678 - 0x06004A40
const Gfx skeeter_seg6_dl_06004678[] = {
    gsSPVertex(skeeter_seg6_vertex_06001BB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 3,  2,  1, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 5,  4,  3, 0x0,  7,  8,  6, 0x0),
    gsSP2Triangles( 7,  6,  5, 0x0,  9, 10,  8, 0x0),
    gsSP2Triangles( 9,  8,  7, 0x0, 11, 12, 10, 0x0),
    gsSP2Triangles(11, 10,  9, 0x0, 13, 14, 12, 0x0),
    gsSP2Triangles(13, 12, 11, 0x0, 15, 14, 13, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06001CB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 3,  2,  1, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 5,  4,  3, 0x0,  7,  8,  6, 0x0),
    gsSP2Triangles( 7,  6,  5, 0x0,  9, 10,  8, 0x0),
    gsSP2Triangles( 9,  8,  7, 0x0, 11, 12, 10, 0x0),
    gsSP2Triangles(11, 10,  9, 0x0, 13, 14, 12, 0x0),
    gsSP2Triangles(13, 12, 11, 0x0, 15, 14, 13, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06001DB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 3,  2,  1, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 5,  4,  3, 0x0,  7,  8,  6, 0x0),
    gsSP2Triangles( 7,  6,  5, 0x0,  9, 10,  8, 0x0),
    gsSP2Triangles( 9,  8,  7, 0x0, 11, 12, 10, 0x0),
    gsSP2Triangles(11, 10,  9, 0x0, 13, 14, 12, 0x0),
    gsSP2Triangles(13, 12, 11, 0x0, 15, 14, 13, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06001EB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 3,  2,  1, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 5,  4,  3, 0x0,  7,  8,  6, 0x0),
    gsSP2Triangles( 7,  6,  5, 0x0,  9, 10,  8, 0x0),
    gsSP2Triangles( 9,  8,  7, 0x0, 11, 12, 10, 0x0),
    gsSP2Triangles(11, 10,  9, 0x0, 13, 14, 12, 0x0),
    gsSP2Triangles(13, 12, 11, 0x0, 15, 14, 13, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06001FB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 3,  2,  1, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 5,  4,  3, 0x0,  7,  8,  6, 0x0),
    gsSP2Triangles( 7,  6,  5, 0x0,  9, 10,  8, 0x0),
    gsSP2Triangles( 9,  8,  7, 0x0, 11, 12, 10, 0x0),
    gsSP2Triangles(11, 10,  9, 0x0, 13, 14, 12, 0x0),
    gsSP2Triangles(13, 12, 11, 0x0, 15, 14, 13, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060020B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 3,  2,  1, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 5,  4,  3, 0x0,  7,  8,  6, 0x0),
    gsSP2Triangles( 7,  6,  5, 0x0,  9, 10,  8, 0x0),
    gsSP2Triangles( 9,  8,  7, 0x0, 11, 12, 10, 0x0),
    gsSP2Triangles(11, 10,  9, 0x0, 13, 14, 12, 0x0),
    gsSP2Triangles(13, 12, 11, 0x0, 15, 14, 13, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060021B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 3,  2,  1, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 5,  4,  3, 0x0,  7,  8,  6, 0x0),
    gsSP2Triangles( 7,  6,  5, 0x0,  9, 10,  8, 0x0),
    gsSP2Triangles( 9,  8,  7, 0x0, 11, 12, 10, 0x0),
    gsSP2Triangles(11, 10,  9, 0x0, 13, 14, 12, 0x0),
    gsSP2Triangles(13, 12, 11, 0x0, 15, 14, 13, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060022B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  2, 0x0),
    gsSP2Triangles( 3,  2,  1, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 5,  4,  3, 0x0,  7,  8,  6, 0x0),
    gsSP2Triangles( 7,  6,  5, 0x0,  9, 10,  8, 0x0),
    gsSP2Triangles( 9,  8,  7, 0x0, 11, 12, 10, 0x0),
    gsSP2Triangles(11, 10,  9, 0x0, 13, 14, 12, 0x0),
    gsSP2Triangles(13, 12, 11, 0x0, 15, 14, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x06004A40 - 0x06005328
const Gfx skeeter_seg6_dl_06004A40[] = {
    gsSPVertex(skeeter_seg6_vertex_060023B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060024B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 11, 10, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060025B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060026B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  6,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060027B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060028B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060029B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 11, 10, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06002AB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06002BB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  6,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06002CB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06002DB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06002EB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 11, 10, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06002FB0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060030B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  6,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060031B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060032B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060033B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 11, 10, 13, 0x0),
    gsSP2Triangles(11, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060034B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060035B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  7,  6,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060036B0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  2,  1,  3, 0x0),
    gsSP2Triangles( 2,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPEndDisplayList(),
};

// 0x06005328 - 0x06005358
const Gfx skeeter_seg6_dl_06005328[] = {
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPVertex(skeeter_seg6_vertex_060037B0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK | G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};

// 0x06005358 - 0x06005720
const Gfx skeeter_seg6_dl_06005358[] = {
    gsSPVertex(skeeter_seg6_vertex_060037F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060038F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_060039F0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06003AF0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06003BF0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06003CF0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06003DF0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPVertex(skeeter_seg6_vertex_06003EF0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  0,  3, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  5,  7, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  8,  7,  9, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 10,  9, 11, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 12, 11, 13, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 14, 13, 15, 0x0),
    gsSPEndDisplayList(),
};
