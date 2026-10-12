// Pebble

// 0x0301C2C0
static const Vtx pebble_seg3_vertex_0301C2C0[] = {
#include "assets/rom/generated/5b50d35c38327c4ced70.inc.c"



};

// 0x0301C300
ALIGNED8 static const Texture pebble_seg3_texture_0301C300[] = {
#include "actors/pebble/pebble.rgba16.inc.c"
};

// 0x0301CB00 - 0x0301CB98
const Gfx pebble_seg3_dl_0301CB00[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsDPLoadTextureBlock(pebble_seg3_texture_0301C300, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPVertex(pebble_seg3_vertex_0301C2C0, 4, 0),
    gsSP1Quadrangle(0, 1, 2, 3, 0x0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
