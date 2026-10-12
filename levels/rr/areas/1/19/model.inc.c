// 0x07015CE8 - 0x07015DE8
static const Vtx rr_seg7_vertex_07015CE8[] = {
#include "assets/rom/generated/1e805ab7dc74b5e5e0b0.inc.c"















};

// 0x07015DE8 - 0x07015ED8
static const Vtx rr_seg7_vertex_07015DE8[] = {
#include "assets/rom/generated/7d70f68651f0af521028.inc.c"














};

// 0x07015ED8 - 0x07015FD8
static const Vtx rr_seg7_vertex_07015ED8[] = {
#include "assets/rom/generated/8f046ee3acb34936b48e.inc.c"















};

// 0x07015FD8 - 0x070160B8
static const Vtx rr_seg7_vertex_07015FD8[] = {
#include "assets/rom/generated/4b153a54b9d1b49e1e7a.inc.c"













};

// 0x070160B8 - 0x07016198
static const Vtx rr_seg7_vertex_070160B8[] = {
#include "assets/rom/generated/d0b4dfc3cf3bacf68d9f.inc.c"













};

// 0x07016198 - 0x07016288
static const Vtx rr_seg7_vertex_07016198[] = {
#include "assets/rom/generated/bc095d9031fc445af4fd.inc.c"














};

// 0x07016288 - 0x07016368
static const Vtx rr_seg7_vertex_07016288[] = {
#include "assets/rom/generated/9bce4ab605410f9fe868.inc.c"













};

// 0x07016368 - 0x07016458
static const Vtx rr_seg7_vertex_07016368[] = {
#include "assets/rom/generated/cd2e52067ea414962af2.inc.c"














};

// 0x07016458 - 0x07016548
static const Vtx rr_seg7_vertex_07016458[] = {
#include "assets/rom/generated/3231004427e41b1ee85c.inc.c"














};

// 0x07016548 - 0x07016638
static const Vtx rr_seg7_vertex_07016548[] = {
#include "assets/rom/generated/d1bbb7f7600f314167dd.inc.c"














};

// 0x07016638 - 0x07016718
static const Vtx rr_seg7_vertex_07016638[] = {
#include "assets/rom/generated/aefb8868a8723db92bd2.inc.c"













};

// 0x07016718 - 0x07016808
static const Vtx rr_seg7_vertex_07016718[] = {
#include "assets/rom/generated/4f681fe409242294489d.inc.c"














};

// 0x07016808 - 0x070168E8
static const Vtx rr_seg7_vertex_07016808[] = {
#include "assets/rom/generated/3e37b3bfcc71fdb38026.inc.c"













};

// 0x070168E8 - 0x070169C8
static const Vtx rr_seg7_vertex_070168E8[] = {
#include "assets/rom/generated/4ef052ec1107b661b46e.inc.c"













};

// 0x070169C8 - 0x07016AB8
static const Vtx rr_seg7_vertex_070169C8[] = {
#include "assets/rom/generated/0c90a0bf51be79b28bdd.inc.c"














};

// 0x07016AB8 - 0x07016BB8
static const Vtx rr_seg7_vertex_07016AB8[] = {
#include "assets/rom/generated/744fb13848f44164767e.inc.c"















};

// 0x07016BB8 - 0x07016CB8
static const Vtx rr_seg7_vertex_07016BB8[] = {
#include "assets/rom/generated/47b2325ae781ec17ee5e.inc.c"















};

// 0x07016CB8 - 0x07016DB8
static const Vtx rr_seg7_vertex_07016CB8[] = {
#include "assets/rom/generated/b822564caf08f2adda89.inc.c"















};

// 0x07016DB8 - 0x07016EA8
static const Vtx rr_seg7_vertex_07016DB8[] = {
#include "assets/rom/generated/d3b525e1bee2830cdc36.inc.c"














};

// 0x07016EA8 - 0x07016F98
static const Vtx rr_seg7_vertex_07016EA8[] = {
#include "assets/rom/generated/89b37e4a45143c52b8d1.inc.c"














};

// 0x07016F98 - 0x07016FD8
static const Vtx rr_seg7_vertex_07016F98[] = {
#include "assets/rom/generated/70d0dada0f7b48c79a57.inc.c"



};

// 0x07016FD8 - 0x07017530
static const Gfx rr_seg7_dl_07016FD8[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, sky_09005800),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(rr_seg7_vertex_07015CE8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 4,  6,  5, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(12, 14, 13, 0x0,  1, 15,  2, 0x0),
    gsSPVertex(rr_seg7_vertex_07015DE8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle( 9, 14, 10, 0x0),
    gsSPVertex(rr_seg7_vertex_07015ED8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(10,  9, 14, 0x0, 10, 14, 15, 0x0),
    gsSPVertex(rr_seg7_vertex_07015FD8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_070160B8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07016198, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07016288, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07016368, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7, 10,  8, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(rr_seg7_vertex_07016458, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(rr_seg7_vertex_07016548, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  7,  4, 0x0,  8,  9,  7, 0x0),
    gsSP2Triangles(10, 11,  9, 0x0, 10,  9,  8, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07016638, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  0, 0x0,  5,  6,  0, 0x0),
    gsSP2Triangles( 7,  8,  9, 0x0,  8, 10,  9, 0x0),
    gsSP1Triangle(11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07016718, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  1, 0x0),
    gsSP2Triangles( 3,  1,  0, 0x0,  5,  6,  4, 0x0),
    gsSP2Triangles( 5,  4,  3, 0x0,  7,  6,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(11, 14, 12, 0x0),
    gsSPVertex(rr_seg7_vertex_07016808, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_070168E8, 14, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_070169C8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  7,  5, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 11,  9, 0x0),
    gsSP1Triangle(12, 13, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07016AB8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 13, 15, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07016BB8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  1,  3,  2, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  5,  7,  6, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  9, 11, 10, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 13, 15, 14, 0x0),
    gsSPVertex(rr_seg7_vertex_07016CB8, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 1,  6,  2, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 8, 10,  9, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(12, 14, 13, 0x0,  3, 15,  4, 0x0),
    gsSPVertex(rr_seg7_vertex_07016DB8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  6,  4, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP1Triangle(12, 14, 13, 0x0),
    gsSPVertex(rr_seg7_vertex_07016EA8, 15, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 6,  7,  8, 0x0,  6,  9,  7, 0x0),
    gsSP2Triangles(10, 11, 12, 0x0, 10, 13, 11, 0x0),
    gsSP1Triangle( 0, 14,  1, 0x0),
    gsSPVertex(rr_seg7_vertex_07016F98, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

// 0x07017530 - 0x070175B0
const Gfx rr_seg7_dl_07017530[] = {
    gsDPPipeSync(),
    gsDPSetEnvColor(255, 255, 255, 100),
    gsDPSetCombineMode(G_CC_MODULATERGBFADEA, G_CC_MODULATERGBFADEA),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(rr_seg7_dl_07016FD8),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};
