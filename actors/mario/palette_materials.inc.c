// Included once per palette. Mesh and triangle data remain shared.

ALIGNED8 static const Texture MARIO_PALETTE_NAME(mario_texture_m_logo)[] = {
#if MARIO_PALETTE_VARIANT
#include "actors/mario/mario_he_thicc_logo.rgba16.inc.c"
#else
#include "actors/mario/mario_logo.rgba16.inc.c"
#endif
};

ALIGNED8 static const Texture MARIO_PALETTE_NAME(mario_texture_hair_sideburn)[] = {
#if MARIO_PALETTE_VARIANT
#include "actors/mario/mario_he_thicc_sideburn.rgba16.inc.c"
#else
#include "actors/mario/mario_sideburn.rgba16.inc.c"
#endif
};

ALIGNED8 static const Texture MARIO_PALETTE_NAME(mario_texture_mustache)[] = {
#if MARIO_PALETTE_VARIANT
#include "actors/mario/mario_he_thicc_mustache.rgba16.inc.c"
#else
#include "actors/mario/mario_mustache.rgba16.inc.c"
#endif
};

ALIGNED8 static const Texture MARIO_PALETTE_NAME(mario_texture_eyes_front)[] = {
#if MARIO_PALETTE_VARIANT
#include "actors/mario/mario_he_thicc_eyes_center.rgba16.inc.c"
#else
#include "actors/mario/mario_eyes_center.rgba16.inc.c"
#endif
};

ALIGNED8 static const Texture MARIO_PALETTE_NAME(mario_texture_eyes_half_closed)[] = {
#if MARIO_PALETTE_VARIANT
#include "actors/mario/mario_he_thicc_eyes_half_closed.rgba16.inc.c"
#else
#include "actors/mario/mario_eyes_half_closed.rgba16.inc.c"
#endif
};

ALIGNED8 static const Texture MARIO_PALETTE_NAME(mario_texture_eyes_closed)[] = {
#if MARIO_PALETTE_VARIANT
#include "actors/mario/mario_he_thicc_eyes_closed.rgba16.inc.c"
#else
#include "actors/mario/mario_eyes_closed.rgba16.inc.c"
#endif
};

UNUSED ALIGNED8 static const Texture MARIO_PALETTE_NAME(mario_texture_eyes_closed_unused1)[] = {
#if MARIO_PALETTE_VARIANT
#include "actors/mario/mario_he_thicc_eyes_closed_unused_0.rgba16.inc.c"
#else
#include "actors/mario/mario_eyes_closed_unused_0.rgba16.inc.c"
#endif
};

UNUSED ALIGNED8 static const Texture MARIO_PALETTE_NAME(mario_texture_eyes_closed_unused2)[] = {
#if MARIO_PALETTE_VARIANT
#include "actors/mario/mario_he_thicc_eyes_closed_unused_1.rgba16.inc.c"
#else
#include "actors/mario/mario_eyes_closed_unused_1.rgba16.inc.c"
#endif
};

ALIGNED8 static const Texture MARIO_PALETTE_NAME(mario_texture_eyes_right)[] = {
#if MARIO_PALETTE_VARIANT
#include "actors/mario/mario_he_thicc_eyes_left_unused.rgba16.inc.c"
#else
#include "actors/mario/mario_eyes_left_unused.rgba16.inc.c"
#endif
};

ALIGNED8 static const Texture MARIO_PALETTE_NAME(mario_texture_eyes_left)[] = {
#if MARIO_PALETTE_VARIANT
#include "actors/mario/mario_he_thicc_eyes_right_unused.rgba16.inc.c"
#else
#include "actors/mario/mario_eyes_right_unused.rgba16.inc.c"
#endif
};

ALIGNED8 static const Texture MARIO_PALETTE_NAME(mario_texture_eyes_up)[] = {
#if MARIO_PALETTE_VARIANT
#include "actors/mario/mario_he_thicc_eyes_up_unused.rgba16.inc.c"
#else
#include "actors/mario/mario_eyes_up_unused.rgba16.inc.c"
#endif
};

ALIGNED8 static const Texture MARIO_PALETTE_NAME(mario_texture_eyes_down)[] = {
#if MARIO_PALETTE_VARIANT
#include "actors/mario/mario_he_thicc_eyes_down_unused.rgba16.inc.c"
#else
#include "actors/mario/mario_eyes_down_unused.rgba16.inc.c"
#endif
};

ALIGNED8 static const Texture MARIO_PALETTE_NAME(mario_texture_eyes_dead)[] = {
#if MARIO_PALETTE_VARIANT
#include "actors/mario/mario_he_thicc_eyes_dead.rgba16.inc.c"
#else
#include "actors/mario/mario_eyes_dead.rgba16.inc.c"
#endif
};

const Gfx MARIO_PALETTE_NAME(mario_butt)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_OVERALLS_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_OVERALLS_AMBIENT),
    gsSPDisplayList(MARIO_BODY_NAME(mario_butt_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_left_arm)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHIRT_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHIRT_AMBIENT),
    gsSPDisplayList(mario_left_arm_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_left_hand_closed)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_GLOVES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_GLOVES_AMBIENT),
    gsSPDisplayList(mario_left_hand_closed_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_right_arm)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHIRT_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHIRT_AMBIENT),
    gsSPDisplayList(mario_right_arm_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_right_hand_closed)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_GLOVES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_GLOVES_AMBIENT),
    gsSPDisplayList(mario_right_hand_closed_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_left_thigh)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_OVERALLS_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_OVERALLS_AMBIENT),
    gsSPDisplayList(MARIO_BODY_NAME(mario_left_thigh_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_left_foot)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHOES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHOES_AMBIENT),
    gsSPDisplayList(mario_left_foot_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_right_thigh)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_OVERALLS_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_OVERALLS_AMBIENT),
    gsSPDisplayList(MARIO_BODY_NAME(mario_right_thigh_shared_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_right_foot)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHOES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHOES_AMBIENT),
    gsSPDisplayList(mario_right_foot_dl),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_torso_dl)[] = {
    gsSPDisplayList(MARIO_BODY_NAME(mario_pants_overalls_shared_dl)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHIRT_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHIRT_AMBIENT),
    gsSPDisplayList(MARIO_BODY_NAME(mario_tshirt_shared_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_torso)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_yellow_button),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(MARIO_BODY_NAME(mario_yellow_button_dl)),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_torso_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_face_cap_on_dl)[] = {
    gsSPDisplayList(mario_face_part_cap_on_dl),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
    gsSPDisplayList(mario_face_cap_dl),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_HAIR_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_HAIR_AMBIENT),
    gsSPDisplayList(mario_face_back_hair_cap_on_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_on_eyes_front)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_front)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_on_eyes_half_closed)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_half_closed)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_on_eyes_closed)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_closed)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_on_eyes_right)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_right)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_on_eyes_left)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_left)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_on_eyes_up)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_up)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_on_eyes_down)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_down)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_on_eyes_dead)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_dead)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_face_cap_off_dl)[] = {
    gsSPDisplayList(mario_face_part_cap_off_dl),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_HAIR_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_HAIR_AMBIENT),
    gsSPDisplayList(mario_face_hair_cap_off_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_off_eyes_front)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_front)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_off_eyes_half_closed)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_half_closed)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_off_eyes_closed)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_closed)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_off_eyes_right)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_right)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_off_eyes_left)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_left)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_off_eyes_up)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_up)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_off_eyes_down)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_down)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_off_eyes_dead)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_dead)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_mustache_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_hair_sideburn)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_hair_sideburn_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_medium_poly_butt)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_OVERALLS_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_OVERALLS_AMBIENT),
    gsSPDisplayList(MARIO_BODY_NAME(mario_medium_poly_butt_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_medium_poly_left_arm)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHIRT_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHIRT_AMBIENT),
    gsSPDisplayList(mario_medium_poly_left_arm_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_medium_poly_left_hand_closed)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_GLOVES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_GLOVES_AMBIENT),
    gsSPDisplayList(mario_medium_poly_left_hand_closed_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_medium_poly_right_arm)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHIRT_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHIRT_AMBIENT),
    gsSPDisplayList(mario_medium_poly_right_arm_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_medium_poly_right_hand_closed)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_GLOVES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_GLOVES_AMBIENT),
    gsSPDisplayList(mario_medium_poly_right_hand_closed_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_medium_poly_left_thigh)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_OVERALLS_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_OVERALLS_AMBIENT),
    gsSPDisplayList(MARIO_BODY_NAME(mario_medium_poly_left_thigh_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_medium_poly_left_foot)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHOES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHOES_AMBIENT),
    gsSPDisplayList(mario_medium_poly_left_foot_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_medium_poly_right_thigh)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_OVERALLS_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_OVERALLS_AMBIENT),
    gsSPDisplayList(MARIO_BODY_NAME(mario_medium_poly_right_thigh_shared_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_medium_poly_right_foot)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHOES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHOES_AMBIENT),
    gsSPDisplayList(mario_medium_poly_right_foot_dl),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_medium_poly_torso_dl)[] = {
    gsSPDisplayList(MARIO_BODY_NAME(mario_medium_poly_pants_overalls_shared_dl)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHIRT_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHIRT_AMBIENT),
    gsSPDisplayList(MARIO_BODY_NAME(mario_medium_poly_tshirt_shared_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_medium_poly_torso)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_yellow_button),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(MARIO_BODY_NAME(mario_medium_poly_yellow_button_dl)),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_medium_poly_torso_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_butt)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_OVERALLS_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_OVERALLS_AMBIENT),
    gsSPDisplayList(MARIO_BODY_NAME(mario_low_poly_butt_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_left_arm)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHIRT_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHIRT_AMBIENT),
    gsSPDisplayList(mario_low_poly_left_arm_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_left_hand_closed)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_GLOVES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_GLOVES_AMBIENT),
    gsSPDisplayList(mario_low_poly_left_hand_closed_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_right_arm)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHIRT_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHIRT_AMBIENT),
    gsSPDisplayList(mario_low_poly_right_arm_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_right_hand_closed)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_GLOVES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_GLOVES_AMBIENT),
    gsSPDisplayList(mario_low_poly_right_hand_closed_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_left_thigh)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_OVERALLS_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_OVERALLS_AMBIENT),
    gsSPDisplayList(MARIO_BODY_NAME(mario_low_poly_left_thigh_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_left_foot)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHOES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHOES_AMBIENT),
    gsSPDisplayList(mario_low_poly_left_foot_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_right_thigh)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_OVERALLS_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_OVERALLS_AMBIENT),
    gsSPDisplayList(MARIO_BODY_NAME(mario_low_poly_right_thigh_shared_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_right_foot)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHOES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHOES_AMBIENT),
    gsSPDisplayList(mario_low_poly_right_foot_dl),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_torso_dl)[] = {
    gsSPDisplayList(MARIO_BODY_NAME(mario_low_poly_pants_overalls_shared_dl)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SHIRT_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SHIRT_AMBIENT),
    gsSPDisplayList(MARIO_BODY_NAME(mario_low_poly_tshirt_shared_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_torso)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, mario_texture_yellow_button),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(MARIO_BODY_NAME(mario_low_poly_yellow_button_dl)),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_torso_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_face_cap_on_dl)[] = {
    gsSPDisplayList(mario_low_poly_face_part_cap_on_dl),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
    gsSPDisplayList(mario_low_poly_face_cap_dl),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_HAIR_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_HAIR_AMBIENT),
    gsSPDisplayList(mario_low_poly_face_back_hair_cap_on_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_on_eyes_front)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_low_poly_mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_front)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_on_eyes_half_closed)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_low_poly_mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_half_closed)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_on_eyes_closed)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_low_poly_mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_closed)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_on_eyes_right)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_low_poly_mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_right)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_on_eyes_left)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_low_poly_mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_left)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_on_eyes_up)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_low_poly_mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_up)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_on_eyes_down)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_low_poly_mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_down)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_on_eyes_dead)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_low_poly_mario_m_logo_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_dead)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_eyes_cap_on_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_on_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_on_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_face_cap_off_dl)[] = {
    gsSPDisplayList(mario_low_poly_face_part_cap_off_dl),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_HAIR_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_HAIR_AMBIENT),
    gsSPDisplayList(mario_low_poly_face_hair_cap_off_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_off_eyes_front)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_front)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_off_eyes_half_closed)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_half_closed)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_off_eyes_closed)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_closed)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_off_eyes_right)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_right)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_off_eyes_left)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_left)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_off_eyes_up)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_up)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_off_eyes_down)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_down)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_low_poly_cap_off_eyes_dead)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_eyes_dead)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_SKIN_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_SKIN_AMBIENT),
    gsSPDisplayList(mario_low_poly_mario_eyes_cap_off_dl),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_mustache)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(mario_low_poly_mustache_cap_off_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_low_poly_face_cap_off_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_left_hand_open)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_GLOVES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_GLOVES_AMBIENT),
    gsSPDisplayList(mario_left_hand_open_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_right_hand_open)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_GLOVES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_GLOVES_AMBIENT),
    gsSPDisplayList(mario_right_hand_open_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_right_hand_cap_dl)[] = {
    gsSPDisplayList(mario_right_hand_cap_top_dl),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_GLOVES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_GLOVES_AMBIENT),
    gsSPDisplayList(mario_right_hand_cap_hand_position_dl),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_UNDERSIDE_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_UNDERSIDE_AMBIENT),
    gsSPDisplayList(mario_right_hand_cap_bottom_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_right_hand_cap)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBFADEA, G_CC_BLENDRGBFADEA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_right_hand_cap_m_logo_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADEFADEA, G_CC_SHADEFADEA),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_right_hand_cap_dl)),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_metal_right_hand_cap_shared_dl)[] = {
#if MARIO_PALETTE_VARIANT
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
#endif
    gsSPDisplayList(mario_right_hand_cap_m_logo_dl),
    gsSPDisplayList(mario_right_hand_cap_top_dl),
    gsSPDisplayList(mario_right_hand_cap_hand_position_dl),
    gsSPDisplayList(mario_right_hand_cap_bottom_dl),
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPTexture(0x0F80, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_right_hand_peace)[] = {
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_GLOVES_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_GLOVES_AMBIENT),
    gsSPDisplayList(mario_right_hand_peace_shared_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_unused_base_dl)[] = {
    gsSPDisplayList(mario_cap_unused_base_top_dl),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_UNDERSIDE_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_UNDERSIDE_AMBIENT),
    gsSPDisplayList(mario_cap_unused_base_bottom_dl),
    gsSPEndDisplayList(),
};

const Gfx MARIO_PALETTE_NAME(mario_cap_unused_dl)[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_BLENDRGBA, G_CC_BLENDRGBA),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, MARIO_PALETTE_NAME(mario_texture_m_logo)),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLightColor(LIGHT_1, MARIO_PALETTE_CAP_LIGHT),
    gsSPLightColor(LIGHT_2, MARIO_PALETTE_CAP_AMBIENT),
    gsSPDisplayList(mario_cap_unused_m_logo_dl),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPDisplayList(MARIO_PALETTE_NAME(mario_cap_unused_base_dl)),
    gsSPEndDisplayList(),
};

