// Water Ring

// 0x06012368

// 0x06012380
ALIGNED8 static const Texture water_ring_seg6_texture_06012380[] = {
#include "actors/water_ring/water_ring.rgba16.inc.c"
};

// 0x06013380
static const Vtx water_ring_seg6_vertex_06013380[] = {
#include "assets/rom/generated/64fdaf0de6bb23410d52.inc.c"















};

// 0x06013480
static const Vtx water_ring_seg6_vertex_06013480[] = {
#include "assets/rom/generated/208c80552c14081e2f30.inc.c"















};

// 0x06013580
static const Vtx water_ring_seg6_vertex_06013580[] = {
#include "assets/rom/generated/3da43fd954c978cd8d35.inc.c"















};

// 0x06013680
static const Vtx water_ring_seg6_vertex_06013680[] = {
#include "assets/rom/generated/8cc816ba2d27b5c46e3d.inc.c"















};

// 0x06013780
static const Vtx water_ring_seg6_vertex_06013780[] = {
#include "assets/rom/generated/4645089647d1d00ffd68.inc.c"















};

// 0x06013880
static const Vtx water_ring_seg6_vertex_06013880[] = {
#include "assets/rom/generated/02e086ed3ccab00505d0.inc.c"















};

// 0x06013980
static const Vtx water_ring_seg6_vertex_06013980[] = {
#include "assets/rom/generated/049ac9ebd5c453ec1906.inc.c"















};

// 0x06013A80
static const Vtx water_ring_seg6_vertex_06013A80[] = {
#include "assets/rom/generated/91305b9518b4f879790d.inc.c"



};

// 0x06013AC0 - 0x06013DD8
const Gfx water_ring_seg6_dl_06013AC0[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_DECALFADE, G_CC_DECALFADE),
    gsDPLoadTextureBlock(water_ring_seg6_texture_06012380, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    gsSPTexture(0x07C0, 0x1800, 0, G_TX_RENDERTILE, G_ON),
    gsSPLightColor(LIGHT_1, 0xffffffff),
    gsSPLightColor(LIGHT_2, 0x3f3f3fff),
    gsSPVertex(water_ring_seg6_vertex_06013380, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13,  2, 0x0),
    gsSP2Triangles( 5, 14, 15, 0x0,  5, 15,  6, 0x0),
    gsSPVertex(water_ring_seg6_vertex_06013480, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13,  2, 0x0),
    gsSP2Triangles( 5, 14, 15, 0x0,  5, 15,  6, 0x0),
    gsSPVertex(water_ring_seg6_vertex_06013580, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13,  2, 0x0),
    gsSP2Triangles( 5, 14, 15, 0x0,  5, 15,  6, 0x0),
    gsSPVertex(water_ring_seg6_vertex_06013680, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13,  2, 0x0),
    gsSP2Triangles( 5, 14, 15, 0x0,  5, 15,  6, 0x0),
    gsSPVertex(water_ring_seg6_vertex_06013780, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13,  2, 0x0),
    gsSP2Triangles( 5, 14, 15, 0x0,  5, 15,  6, 0x0),
    gsSPVertex(water_ring_seg6_vertex_06013880, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13,  2, 0x0),
    gsSP2Triangles( 5, 14, 15, 0x0,  5, 15,  6, 0x0),
    gsSPVertex(water_ring_seg6_vertex_06013980, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles( 1, 12, 13, 0x0,  1, 13,  2, 0x0),
    gsSP2Triangles( 5, 14, 15, 0x0,  5, 15,  6, 0x0),
    gsSPVertex(water_ring_seg6_vertex_06013A80, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPTexture(0x07C0, 0x0F80, 0, G_TX_RENDERTILE, G_OFF),
    gsSPEndDisplayList(),
};
