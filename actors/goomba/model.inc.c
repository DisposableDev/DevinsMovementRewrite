// BEGIN GENERATED GOOMBA VERTEX GROUPS
#if defined(F3DEX_GBI_2) || defined(F3DEX_GBI) || defined(F3DLP_GBI)
ALIGNED16 static const Vtx sGoombaVertexGroup0[] = {
#include "assets/rom/generated/3f3045ced1a7e4beb7f7.inc.c"































};

ALIGNED16 static const Vtx sGoombaVertexGroup1[] = {
#include "assets/rom/generated/6e40e13d6cdb8eb9c7ca.inc.c"






























};

ALIGNED16 static const Vtx sGoombaVertexGroup2[] = {
#include "assets/rom/generated/6823751a30ef930834fa.inc.c"










};

ALIGNED16 static const Vtx sGoombaVertexGroup3[] = {
#include "assets/rom/generated/b998d76b327d3ca6f8dc.inc.c"



};

ALIGNED16 static const Vtx sGoombaVertexGroup4[] = {
#include "assets/rom/generated/44af45b4204651e1aaa4.inc.c"






























};

ALIGNED16 static const Vtx sGoombaVertexGroup5[] = {
#include "assets/rom/generated/5ef4f4fb3efc5d2044a9.inc.c"





};

ALIGNED16 static const Vtx sGoombaVertexGroup6[] = {
#include "assets/rom/generated/dffdac32e9f1cb7de5c0.inc.c"






























};

ALIGNED16 static const Vtx sGoombaVertexGroup7[] = {
#include "assets/rom/generated/a9a0e8e89dbe15a837f3.inc.c"







};

ALIGNED16 static const Vtx sGoombaVertexGroup8[] = {
#include "assets/rom/generated/acc8c9e93a78cd5e3592.inc.c"































};

ALIGNED16 static const Vtx sGoombaVertexGroup9[] = {
#include "assets/rom/generated/e2464b2c2cfce291d977.inc.c"































};

ALIGNED16 static const Vtx sGoombaVertexGroup10[] = {
#include "assets/rom/generated/a3a738f6efd422b3ef22.inc.c"






























};

ALIGNED16 static const Vtx sGoombaVertexGroup11[] = {
#include "assets/rom/generated/934a638d562435c958b4.inc.c"



























};

ALIGNED16 static const Vtx sGoombaVertexGroup12[] = {
#include "assets/rom/generated/bddfcad83e18a6d7e30e.inc.c"






























};

ALIGNED16 static const Vtx sGoombaVertexGroup13[] = {
#include "assets/rom/generated/bbcc54b5c02e08d18076.inc.c"































};

ALIGNED16 static const Vtx sGoombaVertexGroup14[] = {
#include "assets/rom/generated/edda21150225d53046ad.inc.c"































};

ALIGNED16 static const Vtx sGoombaVertexGroup15[] = {
#include "assets/rom/generated/674b1f369570cd8ed836.inc.c"




};
#endif
// END GENERATED GOOMBA VERTEX GROUPS
// Goomba

// Unreferenced light group

// Unreferenced light group

// 0x080194D0

// 0x080194E8

// 0x08019500

// 0x08019518

// 0x08019530
ALIGNED8 static const Texture goomba_seg8_texture_08019530[] = {
#include "actors/goomba/goomba_body.rgba16.inc.c"
};

// 0x08019D30
ALIGNED8 static const Texture goomba_seg8_texture_08019D30[] = {
#include "actors/goomba/goomba_face.rgba16.inc.c"
};

// 0x0801A530
ALIGNED8 static const Texture goomba_seg8_texture_0801A530[] = {
#include "actors/goomba/goomba_face_blink.rgba16.inc.c"
};

#ifdef FLOOMBAS
ALIGNED8 static const Texture floomba_seg8_texture_face[] = {
#include "actors/goomba/custom_floomba_face.rgba16.inc.c"
};

ALIGNED8 static const Texture floomba_seg8_texture_blink[] = {
#include "actors/goomba/custom_floomba_face_blink.rgba16.inc.c"
};
#endif

// 0x0801AD30

// 0x0801AD48
UNUSED static const Vtx goomba_seg8_vertex_0801AD48[] = {
#include "assets/rom/generated/55bd85e5e39848a875e2.inc.c"














};

// 0x0801AE38
UNUSED static const Vtx goomba_seg8_vertex_0801AE38[] = {
#include "assets/rom/generated/73b0f87e4604abae8b92.inc.c"















};

// 0x0801AF38
UNUSED static const Vtx goomba_seg8_vertex_0801AF38[] = {
#include "assets/rom/generated/c42e431f62c8207ac3ad.inc.c"















};

// 0x0801B038
UNUSED static const Vtx goomba_seg8_vertex_0801B038[] = {
#include "assets/rom/generated/f0ec1a096cca41e0504b.inc.c"















};

// 0x0801B138
UNUSED static const Vtx goomba_seg8_vertex_0801B138[] = {
#include "assets/rom/generated/e987dbfbf806e2bf7722.inc.c"















};

// 0x0801B238
UNUSED static const Vtx goomba_seg8_vertex_0801B238[] = {
#include "assets/rom/generated/482b4c13d95159714639.inc.c"










};

// 0x0801B2E8 - 0x0801B560
const Gfx goomba_seg8_dl_0801B2E8[] = {
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x7f7f7fff),
#if defined(F3DEX_GBI_2) || defined(F3DEX_GBI) || defined(F3DLP_GBI)
    gsSPVertex(sGoombaVertexGroup0, 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 5, 4, 0, 5, 7, 3, 0),
    gsSP2Triangles(5, 6, 8, 0, 5, 8, 9, 0),
    gsSP2Triangles(7, 5, 9, 0, 3, 7, 10, 0),
    gsSP2Triangles(7, 9, 10, 0, 3, 11, 4, 0),
    gsSP2Triangles(3, 10, 12, 0, 3, 13, 11, 0),
    gsSP2Triangles(0, 2, 14, 0, 15, 16, 17, 0),
    gsSP2Triangles(2, 16, 15, 0, 16, 2, 1, 0),
    gsSP2Triangles(16, 1, 17, 0, 15, 17, 18, 0),
    gsSP2Triangles(19, 15, 18, 0, 20, 21, 22, 0),
    gsSP2Triangles(23, 22, 21, 0, 22, 24, 20, 0),
    gsSP2Triangles(25, 22, 26, 0, 25, 24, 22, 0),
    gsSP2Triangles(20, 24, 25, 0, 27, 17, 28, 0),
    gsSP2Triangles(20, 29, 21, 0, 20, 30, 29, 0),
    gsSP2Triangles(20, 25, 31, 0, 31, 30, 20, 0),
    gsSPVertex(sGoombaVertexGroup1, 31, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 0, 2, 0),
    gsSP2Triangles(4, 5, 0, 0, 1, 0, 5, 0),
    gsSP2Triangles(2, 1, 6, 0, 1, 5, 6, 0),
    gsSP2Triangles(2, 6, 7, 0, 3, 2, 8, 0),
    gsSP2Triangles(9, 10, 11, 0, 9, 11, 12, 0),
    gsSP2Triangles(11, 10, 13, 0, 14, 15, 16, 0),
    gsSP2Triangles(16, 17, 18, 0, 16, 18, 14, 0),
    gsSP2Triangles(17, 7, 18, 0, 19, 14, 18, 0),
    gsSP2Triangles(18, 6, 19, 0, 18, 7, 6, 0),
    gsSP2Triangles(20, 5, 4, 0, 14, 21, 15, 0),
    gsSP2Triangles(19, 22, 14, 0, 14, 23, 21, 0),
    gsSP2Triangles(14, 22, 23, 0, 24, 4, 25, 0),
    gsSP2Triangles(25, 26, 27, 0, 25, 27, 24, 0),
    gsSP2Triangles(23, 24, 27, 0, 24, 20, 4, 0),
    gsSP2Triangles(24, 19, 20, 0, 23, 22, 24, 0),
    gsSP2Triangles(24, 22, 19, 0, 6, 20, 19, 0),
    gsSP2Triangles(6, 5, 20, 0, 28, 29, 30, 0),
    gsSPVertex(sGoombaVertexGroup2, 11, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(3, 6, 4, 0, 7, 1, 8, 0),
    gsSP2Triangles(8, 1, 0, 0, 3, 9, 10, 0),
#else
    gsSPVertex(goomba_seg8_vertex_0801AD48, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  5,  4, 0x0,  5,  7,  3, 0x0),
    gsSP2Triangles( 5,  6,  8, 0x0,  5,  8,  9, 0x0),
    gsSP2Triangles( 7,  5,  9, 0x0,  3,  7, 10, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  3, 11,  4, 0x0),
    gsSP2Triangles( 3, 10, 12, 0x0,  3, 13, 11, 0x0),
    gsSP1Triangle( 0,  2, 14, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801AE38, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  1,  0, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  1,  4,  2, 0x0),
    gsSP2Triangles( 0,  2,  5, 0x0,  6,  0,  5, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0, 10,  9,  8, 0x0),
    gsSP2Triangles( 9, 11,  7, 0x0, 12,  9, 13, 0x0),
    gsSP2Triangles(12, 11,  9, 0x0,  7, 11, 12, 0x0),
    gsSP1Triangle(14,  2, 15, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801AF38, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 0,  4,  5, 0x0,  5,  3,  0, 0x0),
    gsSP2Triangles( 3,  6,  7, 0x0,  1,  3,  7, 0x0),
    gsSP2Triangles( 5,  8,  3, 0x0,  6,  3,  8, 0x0),
    gsSP2Triangles( 7,  6,  9, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  1,  7, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801B038, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 5,  6,  7, 0x0,  5,  7,  3, 0x0),
    gsSP2Triangles( 6,  8,  7, 0x0,  9,  3,  7, 0x0),
    gsSP2Triangles( 7, 10,  9, 0x0,  7,  8, 10, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0,  3, 14,  4, 0x0),
    gsSP1Triangle( 9, 15,  3, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801B138, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 6,  8,  4, 0x0,  1,  4,  8, 0x0),
    gsSP2Triangles( 4,  9,  5, 0x0,  4, 10,  9, 0x0),
    gsSP2Triangles( 1,  3,  4, 0x0,  4,  3, 10, 0x0),
    gsSP2Triangles(11,  9, 10, 0x0, 11, 12,  9, 0x0),
    gsSP1Triangle(13, 14, 15, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801B238, 11, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  1,  8, 0x0),
    gsSP2Triangles( 8,  1,  0, 0x0,  3,  9, 10, 0x0),
#endif

    gsSPEndDisplayList(),
};

// 0x0801B560 - 0x0801B5A0
const Gfx goomba_seg8_dl_0801B560[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_MODULATERGB),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPEndDisplayList(),
};

// 0x0801B5A0 - 0x0801B5C8
const Gfx goomba_seg8_dl_0801B5A0[] = {
    gsSPDisplayList(goomba_seg8_dl_0801B2E8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

// 0x0801B5C8 - 0x0801B5F0
const Gfx goomba_seg8_dl_0801B5C8[] = {
    gsSPDisplayList(goomba_seg8_dl_0801B560),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, goomba_seg8_texture_08019D30),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(goomba_seg8_dl_0801B5A0),
};

// 0x0801B5F0 - 0x0801B618
const Gfx goomba_seg8_dl_0801B5F0[] = {
    gsSPDisplayList(goomba_seg8_dl_0801B560),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, goomba_seg8_texture_0801A530),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(goomba_seg8_dl_0801B5A0),
};

#ifdef FLOOMBAS
const Gfx floomba_seg8_dl_face[] = {
    gsSPDisplayList(goomba_seg8_dl_0801B560),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, floomba_seg8_texture_face),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(goomba_seg8_dl_0801B5A0),
};

const Gfx floomba_seg8_dl_blink[] = {
    gsSPDisplayList(goomba_seg8_dl_0801B560),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, floomba_seg8_texture_blink),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPBranchList(goomba_seg8_dl_0801B5A0),
};
#endif

// 0x0801B618
UNUSED static const Vtx goomba_seg8_vertex_0801B618[] = {
#include "assets/rom/generated/12413cc3c56cb0f7afb7.inc.c"



};

// 0x0801B658 - 0x0801B690
const Gfx goomba_seg8_dl_0801B658[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, goomba_seg8_texture_08019530),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if defined(F3DEX_GBI_2) || defined(F3DEX_GBI) || defined(F3DLP_GBI)
    gsSPVertex(sGoombaVertexGroup3, 4, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
#else
    gsSPVertex(goomba_seg8_vertex_0801B618, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
#endif

    gsSPEndDisplayList(),
};

// 0x0801B690 - 0x0801B700
const Gfx goomba_seg8_dl_0801B690[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(goomba_seg8_dl_0801B658),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

// 0x0801B700
UNUSED static const Vtx goomba_seg8_vertex_0801B700[] = {
#include "assets/rom/generated/7960953227ba4d082e7b.inc.c"















};

// 0x0801B800
UNUSED static const Vtx goomba_seg8_vertex_0801B800[] = {
#include "assets/rom/generated/22b408635b09bedde654.inc.c"














};

// 0x0801B8F0
UNUSED static const Vtx goomba_seg8_vertex_0801B8F0[] = {
#include "assets/rom/generated/48fb8067a16d60dfa4bb.inc.c"













};

// 0x0801B9D0
UNUSED static const Vtx goomba_seg8_vertex_0801B9D0[] = {
#include "assets/rom/generated/d01060e93c8464ebaa05.inc.c"







};

// 0x0801BA50
UNUSED static const Vtx goomba_seg8_vertex_0801BA50[] = {
#include "assets/rom/generated/eccc5f960fefb61f41c5.inc.c"














};

// 0x0801BB40
UNUSED static const Vtx goomba_seg8_vertex_0801BB40[] = {
#include "assets/rom/generated/a138a54f85aa29c8ba4a.inc.c"















};

// 0x0801BC40
UNUSED static const Vtx goomba_seg8_vertex_0801BC40[] = {
#include "assets/rom/generated/0f24a78a2f558da1fa3d.inc.c"















};

// 0x0801BD40
UNUSED static const Vtx goomba_seg8_vertex_0801BD40[] = {
#include "assets/rom/generated/ffd083feb3a87be5d728.inc.c"







};

// 0x0801BDC0
UNUSED static const Vtx goomba_seg8_vertex_0801BDC0[] = {
#include "assets/rom/generated/b780393a1b2605c898cd.inc.c"















};

// 0x0801BEC0
UNUSED static const Vtx goomba_seg8_vertex_0801BEC0[] = {
#include "assets/rom/generated/ed9f69be84f3a29d3c00.inc.c"














};

// 0x0801BFB0
UNUSED static const Vtx goomba_seg8_vertex_0801BFB0[] = {
#include "assets/rom/generated/82de4a43dfa69d5c91ad.inc.c"














};

// 0x0801C0A0
UNUSED static const Vtx goomba_seg8_vertex_0801C0A0[] = {
#include "assets/rom/generated/fcdc785fe08e43d2add6.inc.c"















};

// 0x0801C1A0
UNUSED static const Vtx goomba_seg8_vertex_0801C1A0[] = {
#include "assets/rom/generated/0693c3d924b456bde837.inc.c"













};

// 0x0801C280
UNUSED static const Vtx goomba_seg8_vertex_0801C280[] = {
#include "assets/rom/generated/c9de5a9d7db85ac85bfe.inc.c"













};

// 0x0801C360
UNUSED static const Vtx goomba_seg8_vertex_0801C360[] = {
#include "assets/rom/generated/1bdeafd49ccf16ae10aa.inc.c"















};

// 0x0801C460
UNUSED static const Vtx goomba_seg8_vertex_0801C460[] = {
#include "assets/rom/generated/8c1cb7efb1305a518cff.inc.c"















};

// 0x0801C560
UNUSED static const Vtx goomba_seg8_vertex_0801C560[] = {
#include "assets/rom/generated/dea21fa8ed25536102aa.inc.c"











};

// 0x0801C620
UNUSED static const Vtx goomba_seg8_vertex_0801C620[] = {
#include "assets/rom/generated/123f74af113fa6ff3ce3.inc.c"















};

// 0x0801C720
UNUSED static const Vtx goomba_seg8_vertex_0801C720[] = {
#include "assets/rom/generated/f531e1ac18b8416678e9.inc.c"














};

// 0x0801C810
UNUSED static const Vtx goomba_seg8_vertex_0801C810[] = {
#include "assets/rom/generated/15e1f43ab420ec164daf.inc.c"















};

// 0x0801C910
UNUSED static const Vtx goomba_seg8_vertex_0801C910[] = {
#include "assets/rom/generated/4d5eb1a55bffcd3c2677.inc.c"














};

// 0x0801CA00
UNUSED static const Vtx goomba_seg8_vertex_0801CA00[] = {
#include "assets/rom/generated/d25e26cd8f7b3eb04628.inc.c"















};

// 0x0801CB00
UNUSED static const Vtx goomba_seg8_vertex_0801CB00[] = {
#include "assets/rom/generated/11a5090f170afcb76be9.inc.c"














};

// 0x0801CBF0
UNUSED static const Vtx goomba_seg8_vertex_0801CBF0[] = {
#include "assets/rom/generated/0e5384166cca44e9c10c.inc.c"















};

// 0x0801CCF0
UNUSED static const Vtx goomba_seg8_vertex_0801CCF0[] = {
#include "assets/rom/generated/b25022ae7e1668d13bac.inc.c"















};

// 0x0801CDF0
UNUSED static const Vtx goomba_seg8_vertex_0801CDF0[] = {
#include "assets/rom/generated/279b6c93fd260e773621.inc.c"


};

// 0x0801CE20 - 0x0801CF78
const Gfx goomba_seg8_dl_0801CE20[] = {
    gsSPLightColor(LIGHT_1, 0x542e10ff),
    gsSPLightColor(LIGHT_2, 0x150b04ff),
#if defined(F3DEX_GBI_2) || defined(F3DEX_GBI) || defined(F3DLP_GBI)
    gsSPVertex(sGoombaVertexGroup4, 31, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
    gsSP2Triangles(11, 6, 19, 0, 14, 7, 10, 0),
    gsSP2Triangles(9, 20, 15, 0, 18, 3, 8, 0),
    gsSP2Triangles(14, 10, 9, 0, 14, 9, 15, 0),
    gsSP2Triangles(18, 8, 11, 0, 18, 11, 19, 0),
    gsSP2Triangles(19, 6, 5, 0, 19, 5, 16, 0),
    gsSP2Triangles(10, 7, 6, 0, 10, 6, 11, 0),
    gsSP2Triangles(13, 21, 22, 0, 13, 22, 14, 0),
    gsSP2Triangles(15, 20, 23, 0, 15, 23, 12, 0),
    gsSP2Triangles(8, 3, 20, 0, 8, 20, 9, 0),
    gsSP2Triangles(24, 25, 3, 0, 24, 3, 26, 0),
    gsSP2Triangles(27, 28, 29, 0, 27, 29, 30, 0),
    gsSPVertex(sGoombaVertexGroup5, 6, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 0, 3, 0, 4, 3, 5, 0),
#else
    gsSPVertex(goomba_seg8_vertex_0801B700, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801B800, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  3, 0x0,  6,  7,  8, 0x0),
    gsSP2Triangles( 9, 10, 11, 0x0,  2, 12, 13, 0x0),
    gsSP2Triangles( 6,  8,  9, 0x0,  6,  9, 11, 0x0),
    gsSP2Triangles( 2, 13,  4, 0x0,  2,  4,  3, 0x0),
    gsSP2Triangles( 3,  5, 14, 0x0,  3, 14,  0, 0x0),
    gsSP2Triangles( 8,  7,  5, 0x0,  8,  5,  4, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801B8F0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9,  5, 0x0,  8,  5, 10, 0x0),
    gsSP2Triangles(11, 12,  9, 0x0, 11,  9, 13, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801B9D0, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 2,  4,  7, 0x0,  2,  7,  3, 0x0),
#endif

    gsSPEndDisplayList(),
};

// 0x0801CF78 - 0x0801D0D0
const Gfx goomba_seg8_dl_0801CF78[] = {
    gsSPLightColor(LIGHT_1, 0x613413ff),
    gsSPLightColor(LIGHT_2, 0x180d04ff),
#if defined(F3DEX_GBI_2) || defined(F3DEX_GBI) || defined(F3DLP_GBI)
    gsSPVertex(sGoombaVertexGroup6, 31, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(1, 4, 5, 0, 1, 5, 2, 0),
    gsSP2Triangles(0, 3, 6, 0, 0, 6, 7, 0),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSP2Triangles(12, 13, 9, 0, 12, 9, 14, 0),
    gsSP2Triangles(15, 16, 17, 0, 15, 17, 18, 0),
    gsSP2Triangles(19, 20, 21, 0, 19, 21, 22, 0),
    gsSP2Triangles(23, 24, 20, 0, 23, 20, 25, 0),
    gsSP2Triangles(26, 27, 24, 0, 26, 24, 28, 0),
    gsSP2Triangles(28, 23, 14, 0, 28, 14, 8, 0),
    gsSP2Triangles(18, 29, 30, 0, 18, 30, 19, 0),
    gsSP2Triangles(14, 9, 8, 0, 18, 13, 29, 0),
    gsSP2Triangles(30, 20, 19, 0, 28, 24, 23, 0),
    gsSP2Triangles(28, 8, 11, 0, 28, 11, 26, 0),
    gsSP2Triangles(18, 19, 22, 0, 18, 22, 15, 0),
    gsSP2Triangles(23, 30, 29, 0, 23, 29, 14, 0),
    gsSPVertex(sGoombaVertexGroup7, 8, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
#else
    gsSPVertex(goomba_seg8_vertex_0801BA50, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 1,  4,  5, 0x0,  1,  5,  2, 0x0),
    gsSP2Triangles( 0,  3,  6, 0x0,  0,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13,  9, 0x0, 12,  9, 14, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801BB40, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9,  5, 0x0,  8,  5, 10, 0x0),
    gsSP2Triangles(11, 12,  9, 0x0, 11,  9, 13, 0x0),
    gsSP2Triangles(13,  8, 14, 0x0, 13, 14, 15, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801BC40, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  0,  7,  1, 0x0),
    gsSP2Triangles( 2,  8,  3, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles( 9,  6, 12, 0x0,  9, 12, 13, 0x0),
    gsSP2Triangles( 0,  3, 14, 0x0,  0, 14, 15, 0x0),
    gsSP2Triangles(11,  2,  1, 0x0, 11,  1,  4, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801BD40, 8, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
#endif

    gsSPEndDisplayList(),
};

// 0x0801D0D0 - 0x0801D360
const Gfx goomba_seg8_dl_0801D0D0[] = {
    gsSPLightColor(LIGHT_1, 0x774220ff),
    gsSPLightColor(LIGHT_2, 0x1d1008ff),
#if defined(F3DEX_GBI_2) || defined(F3DEX_GBI) || defined(F3DLP_GBI)
    gsSPVertex(sGoombaVertexGroup8, 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
    gsSP2Triangles(20, 21, 22, 0, 20, 22, 23, 0),
    gsSP2Triangles(20, 24, 12, 0, 25, 26, 23, 0),
    gsSP2Triangles(27, 7, 28, 0, 4, 29, 30, 0),
    gsSP1Triangle(16, 31, 14, 0),
    gsSPVertex(sGoombaVertexGroup9, 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0),
    gsSP2Triangles(6, 7, 8, 0, 9, 10, 11, 0),
    gsSP2Triangles(9, 11, 12, 0, 13, 14, 15, 0),
    gsSP2Triangles(13, 15, 7, 0, 16, 0, 2, 0),
    gsSP2Triangles(16, 2, 17, 0, 18, 19, 20, 0),
    gsSP2Triangles(18, 20, 21, 0, 14, 22, 23, 0),
    gsSP2Triangles(14, 23, 24, 0, 20, 25, 22, 0),
    gsSP2Triangles(20, 22, 13, 0, 26, 27, 28, 0),
    gsSP2Triangles(26, 28, 29, 0, 30, 23, 27, 0),
    gsSP1Triangle(30, 27, 31, 0),
    gsSPVertex(sGoombaVertexGroup10, 31, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 1, 0, 4, 1, 6, 0),
    gsSP2Triangles(7, 8, 9, 0, 7, 9, 10, 0),
    gsSP2Triangles(11, 2, 8, 0, 11, 8, 12, 0),
    gsSP2Triangles(13, 14, 15, 0, 13, 15, 16, 0),
    gsSP2Triangles(17, 1, 5, 0, 17, 5, 9, 0),
    gsSP2Triangles(18, 19, 20, 0, 18, 20, 21, 0),
    gsSP2Triangles(22, 11, 12, 0, 22, 12, 23, 0),
    gsSP2Triangles(24, 25, 26, 0, 24, 26, 27, 0),
    gsSP2Triangles(26, 25, 28, 0, 26, 28, 29, 0),
    gsSP2Triangles(28, 25, 10, 0, 28, 10, 30, 0),
    gsSPVertex(sGoombaVertexGroup11, 28, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
    gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
    gsSP2Triangles(16, 17, 18, 0, 19, 20, 21, 0),
    gsSP2Triangles(22, 23, 24, 0, 25, 26, 27, 0),
#else
    gsSPVertex(goomba_seg8_vertex_0801BDC0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801BEC0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 4,  8,  9, 0x0, 10, 11,  7, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801BFB0, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0,  3,  5,  2, 0x0),
    gsSP1Triangle( 3,  2,  1, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801C0A0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13, 14, 0x0),
    gsSP2Triangles(10, 15, 12, 0x0, 10, 12,  0, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801C1A0, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  4,  1,  6, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  7,  9, 10, 0x0),
    gsSP2Triangles(11, 12,  8, 0x0, 11,  8, 13, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801C280, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  4,  1,  6, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  7,  9, 10, 0x0),
    gsSP2Triangles(11, 12, 13, 0x0, 11, 13,  2, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801C360, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(10,  9, 12, 0x0, 10, 12, 13, 0x0),
    gsSP2Triangles(12,  9, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801C460, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801C560, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  9, 10, 11, 0x0),
#endif

    gsSPEndDisplayList(),
};

// 0x0801D360 - 0x0801D760
const Gfx goomba_seg8_dl_0801D360[] = {
    gsSPLightColor(LIGHT_1, 0xdeb44eff),
    gsSPLightColor(LIGHT_2, 0x372d13ff),
#if defined(F3DEX_GBI_2) || defined(F3DEX_GBI) || defined(F3DLP_GBI)
    gsSPVertex(sGoombaVertexGroup12, 31, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 1, 0, 4, 1, 0, 0),
    gsSP2Triangles(6, 7, 5, 0, 6, 5, 4, 0),
    gsSP2Triangles(8, 9, 7, 0, 8, 7, 6, 0),
    gsSP2Triangles(10, 11, 9, 0, 10, 9, 8, 0),
    gsSP2Triangles(1, 12, 13, 0, 1, 13, 2, 0),
    gsSP2Triangles(5, 14, 12, 0, 5, 12, 1, 0),
    gsSP2Triangles(7, 15, 14, 0, 7, 14, 5, 0),
    gsSP2Triangles(9, 16, 17, 0, 9, 17, 7, 0),
    gsSP2Triangles(11, 18, 16, 0, 11, 16, 9, 0),
    gsSP2Triangles(12, 19, 20, 0, 12, 20, 13, 0),
    gsSP2Triangles(14, 21, 19, 0, 14, 19, 12, 0),
    gsSP2Triangles(17, 22, 21, 0, 17, 21, 14, 0),
    gsSP2Triangles(16, 23, 22, 0, 16, 22, 17, 0),
    gsSP2Triangles(18, 24, 23, 0, 18, 23, 16, 0),
    gsSP2Triangles(19, 25, 26, 0, 19, 26, 20, 0),
    gsSP2Triangles(21, 27, 25, 0, 21, 25, 19, 0),
    gsSP2Triangles(22, 28, 27, 0, 22, 27, 21, 0),
    gsSP2Triangles(23, 29, 28, 0, 23, 28, 22, 0),
    gsSP2Triangles(24, 30, 29, 0, 24, 29, 23, 0),
    gsSPVertex(sGoombaVertexGroup13, 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 1, 0, 4, 1, 0, 0),
    gsSP2Triangles(6, 7, 5, 0, 6, 5, 4, 0),
    gsSP2Triangles(8, 9, 10, 0, 8, 10, 6, 0),
    gsSP2Triangles(11, 12, 9, 0, 11, 9, 8, 0),
    gsSP2Triangles(1, 13, 14, 0, 1, 14, 2, 0),
    gsSP2Triangles(5, 15, 13, 0, 5, 13, 1, 0),
    gsSP2Triangles(10, 16, 15, 0, 10, 15, 5, 0),
    gsSP2Triangles(9, 17, 16, 0, 9, 16, 10, 0),
    gsSP2Triangles(12, 18, 17, 0, 12, 17, 9, 0),
    gsSP2Triangles(13, 19, 20, 0, 13, 20, 14, 0),
    gsSP2Triangles(15, 21, 19, 0, 15, 19, 13, 0),
    gsSP2Triangles(16, 22, 21, 0, 16, 21, 15, 0),
    gsSP2Triangles(17, 23, 22, 0, 17, 22, 16, 0),
    gsSP2Triangles(18, 24, 23, 0, 18, 23, 17, 0),
    gsSP2Triangles(19, 25, 26, 0, 19, 26, 20, 0),
    gsSP2Triangles(21, 27, 25, 0, 21, 25, 19, 0),
    gsSP2Triangles(22, 28, 27, 0, 22, 27, 21, 0),
    gsSP2Triangles(23, 29, 30, 0, 23, 30, 22, 0),
    gsSP2Triangles(24, 31, 29, 0, 24, 29, 23, 0),
    gsSPVertex(sGoombaVertexGroup14, 32, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
    gsSP2Triangles(4, 5, 1, 0, 4, 1, 0, 0),
    gsSP2Triangles(6, 7, 5, 0, 6, 5, 4, 0),
    gsSP2Triangles(8, 9, 7, 0, 8, 7, 6, 0),
    gsSP2Triangles(10, 11, 9, 0, 10, 9, 8, 0),
    gsSP2Triangles(1, 12, 13, 0, 1, 13, 2, 0),
    gsSP2Triangles(5, 14, 12, 0, 5, 12, 1, 0),
    gsSP2Triangles(7, 15, 14, 0, 7, 14, 5, 0),
    gsSP2Triangles(9, 16, 15, 0, 9, 15, 7, 0),
    gsSP2Triangles(11, 17, 16, 0, 11, 16, 9, 0),
    gsSP2Triangles(13, 18, 19, 0, 13, 19, 20, 0),
    gsSP2Triangles(13, 20, 21, 0, 22, 23, 24, 0),
    gsSP2Triangles(22, 24, 25, 0, 22, 25, 26, 0),
    gsSP2Triangles(22, 26, 3, 0, 22, 3, 2, 0),
    gsSP2Triangles(17, 11, 10, 0, 17, 10, 27, 0),
    gsSP2Triangles(17, 27, 28, 0, 17, 28, 29, 0),
    gsSP2Triangles(17, 29, 30, 0, 17, 30, 31, 0),
    gsSPVertex(sGoombaVertexGroup15, 5, 0),
    gsSP2Triangles(0, 1, 2, 0, 0, 3, 4, 0),
#else
    gsSPVertex(goomba_seg8_vertex_0801C620, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  4,  1,  0, 0x0),
    gsSP2Triangles( 6,  7,  5, 0x0,  6,  5,  4, 0x0),
    gsSP2Triangles( 8,  9,  7, 0x0,  8,  7,  6, 0x0),
    gsSP2Triangles(10, 11,  9, 0x0, 10,  9,  8, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13,  2, 0x0),
    gsSP2Triangles( 5, 14, 12, 0x0,  5, 12,  1, 0x0),
    gsSP2Triangles( 7, 15, 14, 0x0,  7, 14,  5, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801C720, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  4,  1,  0, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11,  7, 0x0, 10,  7,  6, 0x0),
    gsSP2Triangles( 2, 12, 11, 0x0,  2, 11, 10, 0x0),
    gsSP2Triangles( 1, 13, 12, 0x0,  1, 12,  2, 0x0),
    gsSP2Triangles( 5, 14, 13, 0x0,  5, 13,  1, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801C810, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  4,  1,  0, 0x0),
    gsSP2Triangles( 6,  7,  5, 0x0,  6,  5,  4, 0x0),
    gsSP2Triangles( 8,  9,  7, 0x0,  8,  7,  6, 0x0),
    gsSP2Triangles(10, 11,  9, 0x0, 10,  9,  8, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13,  2, 0x0),
    gsSP2Triangles( 5, 14, 12, 0x0,  5, 12,  1, 0x0),
    gsSP2Triangles( 7, 15, 14, 0x0,  7, 14,  5, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801C910, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  4,  1,  0, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11,  7, 0x0, 10,  7,  6, 0x0),
    gsSP2Triangles( 2, 12, 11, 0x0,  2, 11, 10, 0x0),
    gsSP2Triangles( 1, 13, 12, 0x0,  1, 12,  2, 0x0),
    gsSP2Triangles( 5, 14, 13, 0x0,  5, 13,  1, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801CA00, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  4,  1,  0, 0x0),
    gsSP2Triangles( 6,  7,  5, 0x0,  6,  5,  4, 0x0),
    gsSP2Triangles( 8,  9,  7, 0x0,  8,  7,  6, 0x0),
    gsSP2Triangles(10, 11,  9, 0x0, 10,  9,  8, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13,  2, 0x0),
    gsSP2Triangles( 5, 14, 12, 0x0,  5, 12,  1, 0x0),
    gsSP2Triangles( 7, 15, 14, 0x0,  7, 14,  5, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801CB00, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  4,  1,  0, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  8,  9, 0x0),
    gsSP2Triangles(10, 11,  7, 0x0, 10,  7,  6, 0x0),
    gsSP2Triangles( 2, 12, 11, 0x0,  2, 11, 10, 0x0),
    gsSP2Triangles( 1, 13, 12, 0x0,  1, 12,  2, 0x0),
    gsSP2Triangles( 5, 14, 13, 0x0,  5, 13,  1, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801CBF0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  1, 0x0,  4,  1,  0, 0x0),
    gsSP2Triangles( 6,  7,  5, 0x0,  6,  5,  4, 0x0),
    gsSP2Triangles( 8,  9,  7, 0x0,  8,  7,  6, 0x0),
    gsSP2Triangles(10, 11,  9, 0x0, 10,  9,  8, 0x0),
    gsSP2Triangles( 2, 12, 13, 0x0,  2, 13, 14, 0x0),
    gsSP1Triangle( 2, 14, 15, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801CCF0, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 0,  3,  4, 0x0,  0,  4,  5, 0x0),
    gsSP2Triangles( 0,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0,  7, 10, 11, 0x0),
    gsSP2Triangles( 7, 11, 12, 0x0,  7, 12, 13, 0x0),
    gsSP2Triangles( 7, 13, 14, 0x0,  7, 14, 15, 0x0),
    gsSPVertex(goomba_seg8_vertex_0801CDF0, 3, 0),
    gsSP1Triangle( 0,  1,  2, 0x0),
#endif

    gsSPEndDisplayList(),
};

// 0x0801D760 - 0x0801D770
const Gfx goomba_seg8_dl_0801D760[] = {
    gsSPNumLights(NUMLIGHTS_1),
    gsSPEndDisplayList(),
};
