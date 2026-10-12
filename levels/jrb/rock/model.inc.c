// 0x070077F0 - 0x07007808

// 0x07007808 - 0x07007908
static const Vtx jrb_seg7_vertex_07007808[] = {
#include "assets/rom/generated/23eab4e92e3d76a32053.inc.c"















};

// 0x07007908 - 0x070079E8
static const Vtx jrb_seg7_vertex_07007908[] = {
#include "assets/rom/generated/6808afee264baa6cb1d0.inc.c"













};

// 0x070079E8 - 0x07007AC8
static const Gfx jrb_seg7_dl_070079E8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, water_09001800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 64 * 32 - 1, CALC_DXT(64, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, 0xc8e1ffff),
    gsSPLightColor(LIGHT_2, 0x32383fff),
    gsSPVertex(jrb_seg7_vertex_07007808, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 0,  4,  3, 0x0,  3,  6,  0, 0x0),
    gsSP2Triangles( 7,  0,  6, 0x0,  8,  7,  6, 0x0),
    gsSP2Triangles( 9,  4,  0, 0x0,  9,  0,  2, 0x0),
    gsSP2Triangles( 7,  1,  0, 0x0, 10, 11, 12, 0x0),
    gsSP2Triangles(13, 14,  4, 0x0, 13,  4,  9, 0x0),
    gsSP2Triangles( 4, 14,  5, 0x0,  5, 14, 15, 0x0),
    gsSPVertex(jrb_seg7_vertex_07007908, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 1,  0,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 12, 13, 0x0),
    gsSPEndDisplayList(),
};

// 0x07007AC8 - 0x07007B70
const Gfx jrb_seg7_dl_07007AC8[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_2CYCLE),
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPSetFogColor(5, 80, 75, 255),
    gsSPFogPosition(900, 1000),
    gsSPSetGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_MODULATERGB, G_CC_PASS2),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 6, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (64 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(jrb_seg7_dl_070079E8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_NOOP2),
    gsSPClearGeometryMode(G_FOG),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};
