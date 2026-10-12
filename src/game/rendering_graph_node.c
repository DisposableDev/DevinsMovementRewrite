#include <PR/ultratypes.h>

#include "area.h"
#include "engine/math_util.h"
#include "game_init.h"
#include "game_options.h"
#include "gfx_dimensions.h"
#include "main.h"
#include "memory.h"
#include "print.h"
#include "rendering_graph_node.h"
#include "levels/jrb/header.h"
#include "shadow.h"
#include "sm64.h"
#include "game_init.h"
#include "puppyprint.h"
#include "debug_box.h"
#include "level_update.h"
#include "behavior_data.h"
#include "bowser_arena.h"
#include "string.h"
#include "color_presets.h"
#include "compact_explosion_bubbles.h"
#include "liquid_particles.h"
#include "emutest.h"
#include "mario_dither.h"
#include "actors/group0.h"
#include "actors/mario/silhouette.h"
#include "actors/mario/dither.h"
#include "actors/common1.h"
#include "actors/group4.h"
#include "actors/group13.h"
#include "model_ids.h"
#include "mario_transition_pose.h"
#include "mario_ground_pound_animation.h"
#include "mario_he_thicc.h"
#include "mario_character.h"
#include "mario_misc.h"
#include "physics_lab.h"
#include "level_table.h"
#include "geo_misc.h"
#include "object_list_processor.h"
#include "interaction.h"
#include "object_constants.h"
#include "object_helpers.h"

#include "config.h"
#include "config/config_world.h"

#include "mario_floor_lift.inc.c"

/**
 * This file contains the code that processes the scene graph for rendering.
 * The scene graph is responsible for drawing everything except the HUD / text boxes.
 * First the root of the scene graph is processed when geo_process_root
 * is called from level_script.c. The rest of the tree is traversed recursively
 * using the function geo_process_node_and_siblings, which switches over all
 * geo node types and calls a specialized function accordingly.
 * The types are defined in engine/graph_node.h
 *
 * The scene graph typically looks like:
 * - Root (viewport)
 *  - Master list
 *   - Ortho projection
 *    - Background (skybox)
 *  - Master list
 *   - Perspective
 *    - Camera
 *     - <area-specific display lists>
 *     - Object parent
 *      - <group with 240 object nodes>
 *  - Master list
 *   - Script node (Cannon overlay)
 *
 */

s16 gMatStackIndex = 0;
ALIGNED16 Mat4 gMatStack[32];
ALIGNED16 Mtx *gMatStackFixed[32];
f32 sAspectRatio;
static f32 sMarioFloorLift;

/**
 * Animation nodes have state in global variables, so this struct captures
 * the animation state so a 'context switch' can be made when rendering the
 * held object.
 */
struct GeoAnimState {
    /*0x00*/ u8 type;
    /*0x01*/ u8 enabled;
    /*0x02*/ s16 frame;
    /*0x04*/ f32 translationMultiplier;
    /*0x08*/ u16 *attribute;
    /*0x0C*/ s16 *data;
};

// For some reason, this is a GeoAnimState struct, but the current state consists
// of separate global variables. It won't match EU otherwise.
struct GeoAnimState gGeoTempState;

u8 gCurrAnimType;
u8 gCurrAnimEnabled;
s16 gCurrAnimFrame;
f32 gCurrAnimTranslationMultiplier;
u16 *gCurrAnimAttribute;
s16 *gCurrAnimData;

struct AllocOnlyPool *gDisplayListHeap;

// Aquarium particles keep their normal object updates. Only their drawing is
// batched, in original layer order, using the existing sprite and material.
#define BUBBLE_BATCH_CAPACITY 16
#define BUBBLE_VERTEX_PRECISION 16.0f
#define BUBBLE_BATCH_COMMANDS (BUBBLE_BATCH_CAPACITY + BUBBLE_BATCH_CAPACITY / 4 + 3)
struct BubbleBatchBuffer {
    Mtx transform;
    Vtx vertices[BUBBLE_BATCH_CAPACITY * 4];
    Gfx commands[BUBBLE_BATCH_COMMANDS];
};
static struct {
    struct BubbleBatchBuffer *buffer;
    Mat4 basis;
    Gfx *head;
    Gfx *vertexCommand;
    s32 count;
} sBubbleBatch;

static void geo_flush_bubble_batch(void) {
    if (sBubbleBatch.count != 0) {
        s32 remainder = sBubbleBatch.count % 4;
        if (remainder != 0) {
            gSPVertex(sBubbleBatch.vertexCommand,
                      &sBubbleBatch.buffer->vertices[(sBubbleBatch.count / 4) * 16], remainder * 4, 0);
        }
        // The first command of the legacy end list draws its quad. Everything
        // after it restores the material, and can be reused after our quads.
        gSPDisplayList(sBubbleBatch.head++, white_particle_small_dl_end + 1);
        gSPEndDisplayList(sBubbleBatch.head++);
        sBubbleBatch.count = 0;
    }
}

static void geo_render_compact_explosion_bubbles(void);

#if defined(SHADOW_TEXTURE_REUSE) && !defined(DISABLE_SHADOWS)
static struct {
    struct DisplayListNode *node;
    const Gfx *material;
} sShadowTextureRuns[LAYER_COUNT];
#endif

/* Rendermode settings for cycle 1 for all layers. */
struct RenderModeContainer renderModeTable_1Cycle[2] = {
    [RENDER_NO_ZB] = { {
        [LAYER_FORCE] = G_RM_OPA_SURF,
        [LAYER_OPAQUE] = G_RM_AA_OPA_SURF,
        [LAYER_OPAQUE_INTER] = G_RM_AA_OPA_SURF,
        [LAYER_OPAQUE_DECAL] = G_RM_AA_OPA_SURF,
        [LAYER_ALPHA] = G_RM_AA_TEX_EDGE,
#if SILHOUETTE
        [LAYER_ALPHA_DECAL] = G_RM_AA_TEX_EDGE | ZMODE_DEC,
        [LAYER_SILHOUETTE_OPAQUE] = G_RM_AA_OPA_SURF,
        [LAYER_SILHOUETTE_ALPHA] = G_RM_AA_TEX_EDGE,
        [LAYER_OCCLUDE_SILHOUETTE_OPAQUE] = G_RM_AA_OPA_SURF,
        [LAYER_OCCLUDE_SILHOUETTE_ALPHA] = G_RM_AA_TEX_EDGE,
#endif
        [LAYER_TRANSPARENT_DECAL] = G_RM_AA_XLU_SURF,
        [LAYER_TRANSPARENT] = G_RM_AA_XLU_SURF,
        [LAYER_TRANSPARENT_INTER] = G_RM_AA_XLU_SURF,
        [LAYER_MARIO_SHADOW] = G_RM_AA_XLU_SURF,
    } },
    [RENDER_ZB] = { {
        [LAYER_FORCE] = G_RM_ZB_OPA_SURF,
        [LAYER_OPAQUE] = G_RM_AA_ZB_OPA_SURF,
        [LAYER_OPAQUE_INTER] = G_RM_AA_ZB_OPA_INTER,
        [LAYER_OPAQUE_DECAL] = G_RM_AA_ZB_OPA_DECAL,
        [LAYER_ALPHA] = G_RM_AA_ZB_TEX_EDGE,
#if SILHOUETTE
        [LAYER_ALPHA_DECAL] = G_RM_AA_ZB_TEX_EDGE | ZMODE_DEC,
        [LAYER_SILHOUETTE_OPAQUE] = G_RM_AA_ZB_OPA_SURF,
        [LAYER_SILHOUETTE_ALPHA] = G_RM_AA_ZB_TEX_EDGE,
        [LAYER_OCCLUDE_SILHOUETTE_OPAQUE] = G_RM_AA_ZB_OPA_SURF,
        [LAYER_OCCLUDE_SILHOUETTE_ALPHA] = G_RM_AA_ZB_TEX_EDGE,
#endif
        [LAYER_TRANSPARENT_DECAL] = G_RM_AA_ZB_XLU_DECAL,
        [LAYER_TRANSPARENT] = G_RM_AA_ZB_XLU_SURF,
        [LAYER_TRANSPARENT_INTER] = G_RM_AA_ZB_XLU_INTER,
        // Decal depth tolerance keeps distant floors from clipping the shadow.
        [LAYER_MARIO_SHADOW] = G_RM_AA_ZB_XLU_DECAL,
    } }
};

/* Rendermode settings for cycle 2 for all layers. */
struct RenderModeContainer renderModeTable_2Cycle[2] = {
    [RENDER_NO_ZB] = { {
        [LAYER_FORCE] = G_RM_OPA_SURF2,
        [LAYER_OPAQUE] = G_RM_AA_OPA_SURF2,
        [LAYER_OPAQUE_INTER] = G_RM_AA_OPA_SURF2,
        [LAYER_OPAQUE_DECAL] = G_RM_AA_OPA_SURF2,
        [LAYER_ALPHA] = G_RM_AA_TEX_EDGE2,
#if SILHOUETTE
        [LAYER_ALPHA_DECAL] = G_RM_AA_TEX_EDGE2 | ZMODE_DEC,
        [LAYER_SILHOUETTE_OPAQUE] = G_RM_AA_OPA_SURF2,
        [LAYER_SILHOUETTE_ALPHA] = G_RM_AA_TEX_EDGE2,
        [LAYER_OCCLUDE_SILHOUETTE_OPAQUE] = G_RM_AA_OPA_SURF2,
        [LAYER_OCCLUDE_SILHOUETTE_ALPHA] = G_RM_AA_TEX_EDGE2,
#endif
        [LAYER_TRANSPARENT_DECAL] = G_RM_AA_XLU_SURF2,
        [LAYER_TRANSPARENT] = G_RM_AA_XLU_SURF2,
        [LAYER_TRANSPARENT_INTER] = G_RM_AA_XLU_SURF2,
        [LAYER_MARIO_SHADOW] = G_RM_AA_XLU_SURF2,
    } },
    [RENDER_ZB] = { {
        [LAYER_FORCE] = G_RM_ZB_OPA_SURF2,
        [LAYER_OPAQUE] = G_RM_AA_ZB_OPA_SURF2,
        [LAYER_OPAQUE_INTER] = G_RM_AA_ZB_OPA_INTER2,
        [LAYER_OPAQUE_DECAL] = G_RM_AA_ZB_OPA_DECAL2,
        [LAYER_ALPHA] = G_RM_AA_ZB_TEX_EDGE2,
#if SILHOUETTE
        [LAYER_ALPHA_DECAL] = G_RM_AA_ZB_TEX_EDGE2 | ZMODE_DEC,
        [LAYER_SILHOUETTE_OPAQUE] = G_RM_AA_ZB_OPA_SURF2,
        [LAYER_SILHOUETTE_ALPHA] = G_RM_AA_ZB_TEX_EDGE2,
        [LAYER_OCCLUDE_SILHOUETTE_OPAQUE] = G_RM_AA_ZB_OPA_SURF2,
        [LAYER_OCCLUDE_SILHOUETTE_ALPHA] = G_RM_AA_ZB_TEX_EDGE2,
#endif
        [LAYER_TRANSPARENT_DECAL] = G_RM_AA_ZB_XLU_DECAL2,
        [LAYER_TRANSPARENT] = G_RM_AA_ZB_XLU_SURF2,
        [LAYER_TRANSPARENT_INTER] = G_RM_AA_ZB_XLU_INTER2,
        [LAYER_MARIO_SHADOW] = G_RM_AA_ZB_XLU_DECAL2,
    } }
};

ALIGNED16 struct GraphNodeRoot *gCurGraphNodeRoot = NULL;
ALIGNED16 struct GraphNodeMasterList *gCurGraphNodeMasterList = NULL;
ALIGNED16 struct GraphNodePerspective *gCurGraphNodeCamFrustum = NULL;
ALIGNED16 struct GraphNodeCamera *gCurGraphNodeCamera = NULL;
ALIGNED16 struct GraphNodeObject *gCurGraphNodeObject = NULL;
ALIGNED16 struct GraphNodeHeldObject *gCurGraphNodeHeldObject = NULL;
u16 gAreaUpdateCounter = 0;
LookAt* gCurLookAt;

#if SILHOUETTE
#define MARIO_SILHOUETTE_TINT_GRAY 5

#define COLLECTIBLE_SILHOUETTE_TINT_NONE 0
#define COLLECTIBLE_SILHOUETTE_TINT_RED 1
#define COLLECTIBLE_SILHOUETTE_TINT_BLUE 2
#define COLLECTIBLE_SILHOUETTE_TINT_GREEN 3
#define COLLECTIBLE_SILHOUETTE_TINT_STAR 4
// Scale sprite silhouettes to 15/16 in their local plane. Their filtered edges
// stay inside the visible sprite; the shared matrix preserves position and depth.
static const Mtx sCollectibleSilhouetteInset = {
    .m = {
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        { 0x00000000, 0x00010000, 0x00000000, 0x00000001 },
        { (s32) 0xF0000000, 0x00000000, 0x0000F000, 0x00000000 },
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    },
};

// Inset articulated Mario parts to 31/32 around their existing joint pivots.
// This matrix is used only by the silhouette pass, including textured wings.
static const Mtx sMarioSilhouetteInset = {
    .m = {
        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
        { 0x00000000, 0x00000000, 0x00000000, 0x00000001 },
        { (s32) 0xF8000000, 0x00000000, 0x0000F800, 0x00000000 },
        { 0x00000000, (s32) 0xF8000000, 0x00000000, 0x00000000 },
    },
};

// AA_EN        Enable anti aliasing (not actually used for AA in this case).
// IM_RD        Enable reading coverage value.
// CLR_ON_CVG   Don't change the color unless coverage overflows. This helps prevent triangle overlap.
// CVG_DST_WRAP Wrap the coverage value on overflow.
// CVG_X_ALPHA  Coverage and alpha will be multiplied and both will be the same. This makes texture alpha work (eg. Wing Cap wings).
// FORCE_BL     Force Blending.
#define SIL_CVG_THRESHOLD    0x3F // 32..255, 63 seems to give best results
// Alpha dither adds 0..7 before comparison, even to a transparent texture texel.
#define SIL_ALPHA_THRESHOLD 8
#define SCHWA (AA_EN | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | CVG_X_ALPHA | FORCE_BL)
static const Gfx dl_silhouette_begin[] = {
    gsDPPipeSync(),
    // Set the render mode for the silhouette so that it gets its color and alpha from the fog register.
    gsDPSetRenderMode((SCHWA | GBL_c1(G_BL_CLR_FOG, G_BL_A_FOG, G_BL_CLR_MEM, G_BL_1MA)),
                      (SCHWA | GBL_c2(G_BL_CLR_FOG, G_BL_A_FOG, G_BL_CLR_MEM, G_BL_1MA))),
    // Set the silhouette's color & alpha.
    gsDPSetFogColor(255, 255, 255, SILHOUETTE),
    // Fog alpha supplies opacity, so reject transparent texels before blending.
    gsDPSetBlendColor(0, 0, 0, SIL_ALPHA_THRESHOLD),
    gsDPSetAlphaCompare(G_AC_THRESHOLD),
    // Hacky way to prevent triangle overlap. 32..255. 63 seems to give best results.
    gsDPSetEnvColor(0, 0, 0, 0x3F),
    gsSPEndDisplayList(),
};

static const Gfx dl_silhouette_end[] = {
    gsDPPipeSync(),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetBlendColor(0, 0, 0, 0),
    gsDPSetFogColor(0, 0, 0, 255),       // Reset fog color & alpha
    gsDPSetEnvColor(255, 255, 255, 255), // Reset env color & alpha
    gsSPEndDisplayList(),
};
#undef SCHWA

struct MarioSilhouetteMap {
    const Gfx *displayList;
    u8 geometry;
};
#include "actors/mario/silhouette_map.inc.c"
_Static_assert(ARRAY_COUNT(sMarioSilhouetteGeometry) <= 64, "Silhouette IDs exceed draw-node flags");

static struct {
    const void *displayList;
    u8 geometry;
} sMarioSilhouetteCache[64];

static u8 geo_mario_silhouette_geometry(const void *displayList) {
    u32 slot = ((uintptr_t) displayList >> 3) & (ARRAY_COUNT(sMarioSilhouetteCache) - 1);
    if (sMarioSilhouetteCache[slot].displayList == displayList) {
        return sMarioSilhouetteCache[slot].geometry;
    }
    u8 geometry = 0;
    for (u32 i = 0; i < ARRAY_COUNT(sMarioSilhouetteMap); i++) {
        if (displayList == sMarioSilhouetteMap[i].displayList) {
            geometry = sMarioSilhouetteMap[i].geometry;
            break;
        }
    }
    // Mario-owned segmented identities remain fixed across actor-bank reloads.
    sMarioSilhouetteCache[slot].displayList = displayList;
    sMarioSilhouetteCache[slot].geometry = geometry;
    return geometry;
}

struct MarioDitherMap {
    const Gfx *ordinary;
    const Gfx *ordered;
    const Gfx *noise;
};

#include "actors/mario/dither_map.inc.c"

static const Gfx *geo_mario_dither_material(const Gfx *displayList) {
    // Generated alpha commands live in the graphics pool, outside Mario's bank.
    if (((uintptr_t) displayList >> 24) != 4) {
        return displayList;
    }
    static struct MarioDitherMap cache[64];
    u32 slot = ((uintptr_t) displayList >> 3) & (ARRAY_COUNT(cache) - 1);
    if (cache[slot].ordinary == displayList) {
        return cache[slot].ordered;
    }
    const Gfx *ordered = displayList;
    for (u32 i = 0; i < ARRAY_COUNT(sMarioDitherMap); i++) {
        if (sMarioDitherMap[i].ordinary == displayList) {
            ordered = mario_dither_uses_noise() ? sMarioDitherMap[i].noise : sMarioDitherMap[i].ordered;
            break;
        }
    }
    cache[slot].ordinary = displayList;
    cache[slot].ordered = ordered;
    return ordered;
}

// Opaque body pixels need neither framebuffer reads nor overlap coverage blending.
static const Gfx dl_mario_silhouette_begin[] = {
    gsDPPipeSync(),
    gsDPSetRenderMode((G_RM_OPA_SURF) & ~FORCE_BL, (G_RM_OPA_SURF2) & ~FORCE_BL),
    gsSPClearGeometryMode(G_LIGHTING | G_TEXTURE_GEN),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
    gsSPEndDisplayList(),
};

// Ordered alpha supplies the transparent gaps. A fixed wall-copy colour makes
// overlapping triangles idempotent, without coverage rejection or extra masks.
static const Gfx dl_mario_dither_silhouette_begin[] = {
    gsDPPipeSync(),
    gsDPSetRenderMode(
        (CVG_DST_SAVE | FORCE_BL | GBL_c1(G_BL_CLR_FOG, G_BL_A_FOG, G_BL_CLR_FOG, G_BL_0)),
        (CVG_DST_SAVE | FORCE_BL | GBL_c2(G_BL_CLR_FOG, G_BL_A_FOG, G_BL_CLR_FOG, G_BL_0))),
    gsDPSetEnvColor(255, 255, 255, 255),
    // Keep the feather mask when an ordinary translucent wing uses this wall pass.
    gsDPSetBlendColor(0, 0, 0, SIL_ALPHA_THRESHOLD),
    gsDPSetAlphaCompare(G_AC_THRESHOLD),
    gsSPEndDisplayList(),
};

static const Gfx dl_mario_silhouette_end[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
    gsDPSetFogColor(0, 0, 0, 255),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsSPEndDisplayList(),
};
#endif

struct RenderPhase {
    u8 startLayer;
    u8 endLayer;
};

static struct RenderPhase sRenderPhases[] = {
#if SILHOUETTE
    [RENDER_PHASE_ZEX_BEFORE_SILHOUETTE]   = {
        .startLayer = LAYER_FIRST,
        .endLayer   = LAYER_LAST_BEFORE_SILHOUETTE,
    },

    [RENDER_PHASE_ZEX_SILHOUETTE]          = {
        .startLayer = LAYER_SILHOUETTE_FIRST,
        .endLayer   = LAYER_SILHOUETTE_LAST,
    },

    [RENDER_PHASE_ZEX_NON_SILHOUETTE]      = {
        .startLayer = LAYER_SILHOUETTE_FIRST,
        .endLayer   = LAYER_SILHOUETTE_LAST,
    },

    [RENDER_PHASE_ZEX_OCCLUDE_SILHOUETTE]  = {
        .startLayer = LAYER_OCCLUDE_SILHOUETTE_FIRST,
        .endLayer   = LAYER_OCCLUDE_SILHOUETTE_LAST,
    },

    [RENDER_PHASE_ZEX_AFTER_SILHOUETTE]    = {
        .startLayer = LAYER_NON_ZB_FIRST,
        .endLayer   = LAYER_LAST,
    },
#else
    [RENDER_PHASE_ZEX_ALL]                 = {
        .startLayer = LAYER_FIRST,
        .endLayer   = LAYER_LAST,
    },
#endif
};

extern const Gfx init_rsp[];

#define UPPER_FIXED(x) ((int)((unsigned int)((x) * 0x10000) >> 16))
#define LOWER_FIXED(x) ((int)((unsigned int)((x) * 0x10000) & 0xFFFF))

// Fixed-point identity matrix with the inverse of world scale
Mtx identityMatrixWorldScale = {{
    {UPPER_FIXED(1.0f / WORLD_SCALE) << 16, 0x00000000,
     UPPER_FIXED(1.0f / WORLD_SCALE) <<  0, 0x00000000},
    {0x00000000,                            UPPER_FIXED(1.0f / WORLD_SCALE) << 16,
     0x00000000,                            UPPER_FIXED(1.0f)               <<  0},
    {LOWER_FIXED(1.0f / WORLD_SCALE) << 16, 0x00000000,
     LOWER_FIXED(1.0f / WORLD_SCALE) <<  0, 0x00000000},
    {0x00000000,                            LOWER_FIXED(1.0f / WORLD_SCALE) << 16,
     0x00000000,                            LOWER_FIXED(1.0f)               <<  0}
}};

// Resolve once on the second draw of a run; later draws reuse the resolved pointer.
static void *geo_texture_reuse_display_list(void *displayList) {
    static const struct {
        const Gfx *full;
        const Gfx *reuse;
    } materials[] = {
#ifdef IA8_30FPS_COINS
        { coin_seg3_dl_yellow_0, coin_seg3_dl_yellow_0_reuse },
        { coin_seg3_dl_yellow_22_5, coin_seg3_dl_yellow_22_5_reuse },
        { coin_seg3_dl_yellow_45, coin_seg3_dl_yellow_45_reuse },
        { coin_seg3_dl_yellow_67_5, coin_seg3_dl_yellow_67_5_reuse },
        { coin_seg3_dl_yellow_90, coin_seg3_dl_yellow_90_reuse },
        { coin_seg3_dl_yellow_67_5_r, coin_seg3_dl_yellow_67_5_r_reuse },
        { coin_seg3_dl_yellow_45_r, coin_seg3_dl_yellow_45_r_reuse },
        { coin_seg3_dl_yellow_22_5_r, coin_seg3_dl_yellow_22_5_r_reuse },
        { coin_seg3_dl_blue_0, coin_seg3_dl_blue_0_reuse },
        { coin_seg3_dl_blue_22_5, coin_seg3_dl_blue_22_5_reuse },
        { coin_seg3_dl_blue_45, coin_seg3_dl_blue_45_reuse },
        { coin_seg3_dl_blue_67_5, coin_seg3_dl_blue_67_5_reuse },
        { coin_seg3_dl_blue_90, coin_seg3_dl_blue_90_reuse },
        { coin_seg3_dl_blue_67_5_r, coin_seg3_dl_blue_67_5_r_reuse },
        { coin_seg3_dl_blue_22_5_r, coin_seg3_dl_blue_22_5_r_reuse },
        { coin_seg3_dl_blue_45_r, coin_seg3_dl_blue_45_r_reuse },
        { coin_seg3_dl_red_0, coin_seg3_dl_red_0_reuse },
        { coin_seg3_dl_red_22_5, coin_seg3_dl_red_22_5_reuse },
        { coin_seg3_dl_red_45, coin_seg3_dl_red_45_reuse },
        { coin_seg3_dl_red_67_5, coin_seg3_dl_red_67_5_reuse },
        { coin_seg3_dl_red_90, coin_seg3_dl_red_90_reuse },
        { coin_seg3_dl_red_67_5_r, coin_seg3_dl_red_67_5_r_reuse },
        { coin_seg3_dl_red_45_r, coin_seg3_dl_red_45_r_reuse },
        { coin_seg3_dl_red_22_5_r, coin_seg3_dl_red_22_5_r_reuse },
        { coin_seg3_dl_secret_0, coin_seg3_dl_secret_0_reuse },
        { coin_seg3_dl_secret_22_5, coin_seg3_dl_secret_22_5_reuse },
        { coin_seg3_dl_secret_45, coin_seg3_dl_secret_45_reuse },
        { coin_seg3_dl_secret_67_5, coin_seg3_dl_secret_67_5_reuse },
        { coin_seg3_dl_secret_90, coin_seg3_dl_secret_90_reuse },
        { coin_seg3_dl_secret_67_5_r, coin_seg3_dl_secret_67_5_r_reuse },
        { coin_seg3_dl_secret_45_r, coin_seg3_dl_secret_45_r_reuse },
        { coin_seg3_dl_secret_22_5_r, coin_seg3_dl_secret_22_5_r_reuse },

#endif
        { water_splash_seg4_dl_04032640, water_splash_seg4_dl_04032640_reuse },
        { water_splash_seg4_dl_04032658, water_splash_seg4_dl_04032658_reuse },
        { water_splash_seg4_dl_04032670, water_splash_seg4_dl_04032670_reuse },
        { water_splash_seg4_dl_04032688, water_splash_seg4_dl_04032688_reuse },
        { water_splash_seg4_dl_040326A0, water_splash_seg4_dl_040326A0_reuse },
        { water_splash_seg4_dl_040326B8, water_splash_seg4_dl_040326B8_reuse },
        { water_splash_seg4_dl_040326D0, water_splash_seg4_dl_040326D0_reuse },
        { water_splash_seg4_dl_040326E8, water_splash_seg4_dl_040326E8_reuse },
        { mist_seg3_dl_03000880, mist_seg3_dl_03000880_reuse },
        { mist_seg3_dl_03000920, mist_seg3_dl_03000920_reuse },
        { sparkles_seg4_dl_0402A4F8, sparkles_seg4_dl_reuse },
        { sparkles_seg4_dl_0402A510, sparkles_seg4_dl_reuse },
        { sparkles_seg4_dl_0402A528, sparkles_seg4_dl_reuse },
        { sparkles_seg4_dl_0402A540, sparkles_seg4_dl_reuse },
        { sparkles_seg4_dl_0402A558, sparkles_seg4_dl_reuse },
        { sparkles_seg4_dl_0402A570, sparkles_seg4_dl_reuse },
        { small_water_splash_seg4_dl_0, small_water_splash_seg4_dl_reuse },
        { small_water_splash_seg4_dl_1, small_water_splash_seg4_dl_reuse },
        { small_water_splash_seg4_dl_2, small_water_splash_seg4_dl_reuse },
        { small_water_splash_seg4_dl_3, small_water_splash_seg4_dl_reuse },
        { small_water_splash_seg4_dl_4, small_water_splash_seg4_dl_reuse },
        { small_water_splash_seg4_dl_5, small_water_splash_seg4_dl_reuse },
    };

    for (s32 i = 0; i < ARRAY_COUNT(materials); i++) {
        if (displayList == materials[i].full) {
            return (void *) materials[i].reuse;
        }
    }
    return displayList;
}

static Gfx *geo_draw_texture_reuse_display_list(Gfx *head, struct DisplayListNode *list,
                                                void **previous, void **reuse) {
    void *displayList = list->displayList;
    if (list->textureReuse && list->textureReuseEnvironment) {
        // Opacity/tint still executes for each draw without evicting its texture.
        gSPDisplayList(head++, displayList);
        return head;
    }
    if (list->textureReuse && displayList == *previous) {
        if (*reuse == NULL) {
            *reuse = geo_texture_reuse_display_list(displayList);
        }
        displayList = *reuse;
    } else {
        // Misses keep the empty reuse state without repeatedly storing it.
        if (*reuse != NULL) {
            *reuse = NULL;
        }
    }
    void *nextPrevious = list->textureReuse ? list->displayList : NULL;
    if (*previous != nextPrevious) {
        *previous = nextPrevious;
    }
    gSPDisplayList(head++, displayList);
    return head;
}

static u8 geo_object_allows_texture_reuse(void) {
    if (gCurGraphNodeObject == NULL || gCurGraphNodeHeldObject != NULL) {
        return FALSE;
    }
    struct GraphNode *model = gCurGraphNodeObject->sharedChild;
    if (model == NULL) {
        return FALSE;
    }
    // Capture provenance before submission: other actor banks also use segment 4.
    return model == gLoadedGraphNodes[MODEL_WATER_SPLASH]
           || model == gLoadedGraphNodes[MODEL_YELLOW_COIN]
           || model == gLoadedGraphNodes[MODEL_YELLOW_COIN_NO_SHADOW]
           || model == gLoadedGraphNodes[MODEL_BLUE_COIN]
           || model == gLoadedGraphNodes[MODEL_BLUE_COIN_NO_SHADOW]
           || model == gLoadedGraphNodes[MODEL_RED_COIN]
           || model == gLoadedGraphNodes[MODEL_RED_COIN_NO_SHADOW]
#ifdef IA8_30FPS_COINS
           || model == gLoadedGraphNodes[MODEL_SILVER_COIN]
           || model == gLoadedGraphNodes[MODEL_SILVER_COIN_NO_SHADOW]
#endif
           || model == gLoadedGraphNodes[MODEL_MIST] || model == gLoadedGraphNodes[MODEL_WHITE_PUFF]
           || model == gLoadedGraphNodes[MODEL_SPARKLES]
           || model == gLoadedGraphNodes[MODEL_SMALL_WATER_SPLASH];
}

static s32 geo_is_1up_sprite_display_list(const void *displayList) {
    return displayList == mushroom_1up_seg3_dl_0302A660 || displayList == poison_mushroom_dl;
}

static Gfx *geo_draw_1up_cutout_display_list(Gfx *head, struct DisplayListNode *list, s32 enableZBuffer,
                                             u32 mode1, u32 mode2) {
    // Use texture alpha for rejection, without leaving partial coverage for VI.
    // Coverage-derived alpha would accept transparent texels in this non-AA mode.
    u32 cutoutMode1 =
        enableZBuffer ? (G_RM_ZB_OPA_SURF) & ~ALPHA_CVG_SEL : (G_RM_OPA_SURF) | CVG_DST_FULL;
    u32 cutoutMode2 =
        enableZBuffer ? (G_RM_ZB_OPA_SURF2) & ~ALPHA_CVG_SEL : (G_RM_OPA_SURF2) | CVG_DST_FULL;
    gDPPipeSync(head++);
    gDPSetRenderMode(head++, cutoutMode1, cutoutMode2);
    gDPSetBlendColor(head++, 0, 0, 0, 0x80);
    gDPSetAlphaCompare(head++, G_AC_THRESHOLD);
    gSPDisplayList(head++, list->displayList);
    gDPPipeSync(head++);
    gDPSetAlphaCompare(head++, G_AC_NONE);
    gDPSetBlendColor(head++, 0, 0, 0, 0);
    gDPSetRenderMode(head++, mode1, mode2);
    return head;
}

#if SILHOUETTE
static Gfx *geo_draw_1up_silhouette_display_list(Gfx *head, struct DisplayListNode *list, u32 mode1,
                                                 u32 mode2) {
    // This billboard needs no triangle-overlap coverage trick. Reject transparent
    // texels explicitly and blend the green fog tint with full pixel coverage.
    u32 flags = IM_RD | CVG_DST_FULL | FORCE_BL | ZMODE_OPA;
    gDPPipeSync(head++);
    gDPSetRenderMode(head++, flags | GBL_c1(G_BL_CLR_FOG, G_BL_A_FOG, G_BL_CLR_MEM, G_BL_1MA),
                     flags | GBL_c2(G_BL_CLR_FOG, G_BL_A_FOG, G_BL_CLR_MEM, G_BL_1MA));
    gDPSetBlendColor(head++, 0, 0, 0, 0x80);
    gDPSetAlphaCompare(head++, G_AC_THRESHOLD);
    gSPDisplayList(head++, list->displayList);
    gDPPipeSync(head++);
    gDPSetAlphaCompare(head++, G_AC_NONE);
    gDPSetBlendColor(head++, 0, 0, 0, 0);
    gDPSetRenderMode(head++, mode1, mode2);
    return head;
}
#endif

static Gfx *geo_draw_blinking_display_list(Gfx *head, struct DisplayListNode *list, s32 enableZBuffer,
                                           u32 mode1, u32 mode2) {
    // Fog alpha controls blending independently of the model's combiner.
    // Solid materials can have zero vertex alpha, ignored by their normal layer.
    // Only alpha layers need model alpha in coverage to retain transparent edges.
    u32 flags = IM_RD | CVG_DST_WRAP | FORCE_BL | ZMODE_XLU;
    if (!list->blinkOpaque) {
        flags |= CVG_X_ALPHA;
    }
#ifndef DISABLE_AA
    flags |= AA_EN;
#endif
    s32 blendAlpha = list->blinkAlpha;
#if SILHOUETTE
    if (list->marioSilhouette && (gMarioState->flags & (MARIO_VANISH_CAP | MARIO_TELEPORTING))) {
        // The dim phase is already encoded in the shared cutout threshold.
        // A second fade or partial coverage would reveal the white wall copy.
        flags = (flags & ~(AA_EN | IM_RD)) | CVG_X_ALPHA;
        blendAlpha = 255;
    }
#endif
    if (enableZBuffer) {
        flags |= Z_CMP;
    }
    gDPPipeSync(head++);
    gDPSetRenderMode(head++, flags | GBL_c1(G_BL_CLR_IN, G_BL_A_FOG, G_BL_CLR_MEM, G_BL_1MA),
                     flags | GBL_c2(G_BL_CLR_IN, G_BL_A_FOG, G_BL_CLR_MEM, G_BL_1MA));
    gDPSetFogColor(head++, 0, 0, 0, blendAlpha);
    // Alpha-layer and ordered Mario cutouts must still reject transparent texels.
    gDPSetBlendColor(head++, 0, 0, 0, 1);
    gDPSetAlphaCompare(head++, flags & CVG_X_ALPHA ? G_AC_THRESHOLD : G_AC_NONE);
    gSPDisplayList(head++, list->displayList);
    gDPPipeSync(head++);
    gDPSetAlphaCompare(head++, G_AC_NONE);
    gDPSetBlendColor(head++, 0, 0, 0, 0);
    // The next dim draw sets its own state before drawing. Restore only when
    // leaving the run; never assume a model preserves the blend mode or fog.
    if (list->next == NULL || list->next->blinkAlpha == 0) {
        gDPPipeSync(head++);
        gDPSetFogColor(head++, 0, 0, 0, 255);
        gDPSetRenderMode(head++, mode1, mode2);
    }
    return head;
}

/**
 * Process a master list node. This has been modified, so now it runs twice, for each microcode.
 * It iterates through the first 5 layers of if the first index using F3DLX2.Rej, then it switches
 * to F3DZEX and iterates through all layers, then switches back to F3DLX2.Rej and finishes the last
 * 3. It does this, because layers 5-7 are non zbuffered, and just doing 0-7 of ZEX, then 0-7 of REJ
 * would make the ZEX 0-4 render on top of Rej's 5-7.
 */
void geo_process_master_list_sub(struct GraphNodeMasterList *node) {
    struct RenderPhase *renderPhase;
    struct DisplayListNode *currList;
    s32 currLayer     = LAYER_FIRST;
    s32 startLayer    = LAYER_FIRST;
    s32 endLayer      = LAYER_LAST;
    s32 phaseIndex    = RENDER_PHASE_FIRST;
    s32 enableZBuffer = (node->node.flags & GRAPH_RENDER_Z_BUFFER) != 0;
    s32 finalPhase    = enableZBuffer ? RENDER_PHASE_END : 1;
    struct RenderModeContainer *mode1List = &renderModeTable_1Cycle[enableZBuffer];
    struct RenderModeContainer *mode2List = &renderModeTable_2Cycle[enableZBuffer];
    Gfx *tempGfxHead = gDisplayListHead;

#ifdef F3DEX_GBI_2
    // All draws in this master list use the same camera reflection vectors.
    // Load them once, after traversal, instead of once for every appended part.
    gSPLookAt(tempGfxHead++, gCurLookAt);
#endif

    // Loop through the render phases
    for (phaseIndex = RENDER_PHASE_FIRST; phaseIndex < finalPhase; phaseIndex++) {
        if (enableZBuffer) {
            // Get the render phase information.
            renderPhase = &sRenderPhases[phaseIndex];
            startLayer  = renderPhase->startLayer;
            endLayer    = renderPhase->endLayer;
            // Enable z buffer.
            gDPPipeSync(tempGfxHead++);
            gSPSetGeometryMode(tempGfxHead++, G_ZBUFFER);
        } else {
            startLayer = LAYER_FORCE;
            endLayer = LAYER_LAST;
        }
        // Iterate through the layers on the current render phase.
        for (currLayer = startLayer; currLayer <= endLayer; currLayer++) {
            void *previousTexture = NULL;
            void *reuseDisplayList = NULL;
#if SILHOUETTE
            Mtx *silhouetteMatrix = NULL;
#endif
            u32 mode1 = mode1List->modes[currLayer];
            u32 mode2 = mode2List->modes[currLayer];
#if SILHOUETTE
            struct DisplayListNode *silhouetteRun = NULL;
#endif
            // Set 'currList' to the first DisplayListNode on the current layer.
            currList = node->listHeads[currLayer];
#if defined(DISABLE_AA) || !SILHOUETTE
            // Set the render mode for the current layer.
            gDPSetRenderMode(tempGfxHead++, mode1, mode2);
#else
            if (phaseIndex == RENDER_PHASE_NON_SILHOUETTE) {
                // To properly cover the silhouette, disable AA.
                // The silhouette model does not have AA due to the hack used to prevent triangle overlap.
                mode1 &= ~IM_RD;
                mode2 &= ~IM_RD;
                gDPSetRenderMode(tempGfxHead++, mode1, mode2);
            } else {
                // Set the render mode for the current dl.
                gDPSetRenderMode(tempGfxHead++, mode1, mode2);
            }
#endif
            // Iterate through all the displaylists on the current layer.
            while (currList != NULL) {
#if SILHOUETTE
                // Collectible copies belong only to the silhouette pass. Their
                // original nodes still draw in the ordinary occlusion layers.
                if (phaseIndex != RENDER_PHASE_SILHOUETTE && currLayer >= LAYER_SILHOUETTE_FIRST
                    && currLayer <= LAYER_SILHOUETTE_LAST
                    && currList->silhouetteTint != COLLECTIBLE_SILHOUETTE_TINT_NONE) {
                    currList = currList->next;
                    continue;
                }
#endif
                // Add the display list's transformation to the master list.
#if SILHOUETTE
                // Generated silhouette geometry contains only vertices/triangles.
                // It cannot modify the matrix, so consecutive pieces may reuse it.
                s32 geometryOnly =
                    phaseIndex == RENDER_PHASE_SILHOUETTE && currList->silhouetteGeometry != 0;
                if (!geometryOnly || silhouetteMatrix != currList->transform) {
#endif
                    gSPMatrix(tempGfxHead++, VIRTUAL_TO_PHYSICAL(currList->transform),
                              (G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH));
#if SILHOUETTE
                    if (phaseIndex == RENDER_PHASE_SILHOUETTE && currList->marioSilhouette
                        && (currList->silhouetteTint != MARIO_SILHOUETTE_TINT_GRAY
                            || !(gMarioState->flags & (MARIO_VANISH_CAP | MARIO_TELEPORTING)))) {
                        // Vanish cutouts must rasterize at exactly the same
                        // coordinates. Shrinking around a joint can move an
                        // off-center limb outside its ordinary silhouette.
                        gSPMatrix(tempGfxHead++, VIRTUAL_TO_PHYSICAL(&sMarioSilhouetteInset),
                                  G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
                    }
                }
                silhouetteMatrix = geometryOnly ? currList->transform : NULL;
#endif
#if SILHOUETTE
                if (phaseIndex == RENDER_PHASE_SILHOUETTE) {
                    previousTexture = NULL;
                    reuseDisplayList = NULL;
                    u8 red = 255, green = 255, blue = 255;
                    if (currList->cyanSilhouette) {
                        red = 0;
                        if (currList->silhouetteTint == MARIO_SILHOUETTE_TINT_GRAY) {
                            green = 208;
                            blue = 208;
                        }
                    } else if (currList->marioSilhouette
                               && currList->silhouetteTint == COLLECTIBLE_SILHOUETTE_TINT_NONE) {
                        // Blinking changes opacity, not Mario's silhouette colour.
                        red = 208;
                        green = 208;
                        blue = 208;
                    } else {
                        switch (currList->silhouetteTint) {
                            case COLLECTIBLE_SILHOUETTE_TINT_RED:
                                green = 190;
                                blue = 190;
                                break;
                            case COLLECTIBLE_SILHOUETTE_TINT_BLUE:
                                red = 190;
                                green = 214;
                                break;
                            case COLLECTIBLE_SILHOUETTE_TINT_GREEN:
                                red = 195;
                                blue = 195;
                                break;
                            case COLLECTIBLE_SILHOUETTE_TINT_STAR:
                                green = 239;
                                blue = 185;
                                break;
                            case MARIO_SILHOUETTE_TINT_GRAY:
                                // Keep accepted wall-copy pixels uniform where body parts overlap.
                                red = 208;
                                green = 208;
                                blue = 208;
                                break;
                        }
                    }
                    if (currList->silhouetteGeometry != 0) {
                        if (silhouetteRun == NULL
                            || silhouetteRun->cyanSilhouette != currList->cyanSilhouette
                            || silhouetteRun->silhouetteAlpha != currList->silhouetteAlpha) {
                            if (silhouetteRun != NULL) {
                                gSPDisplayList(tempGfxHead++, dl_mario_silhouette_end);
                            }
                            gSPDisplayList(tempGfxHead++, dl_mario_silhouette_begin);
                            gDPSetPrimColor(tempGfxHead++, 0, 0, red, green, blue, 255);
                        }
                        gSPDisplayList(tempGfxHead++,
                                       sMarioSilhouetteGeometry[currList->silhouetteGeometry]);
                        silhouetteRun = currList;
                    } else {
                        // Textured cutouts and unknown models keep their complete material.
                        if (silhouetteRun != NULL) {
                            gSPDisplayList(tempGfxHead++, dl_mario_silhouette_end);
                            silhouetteRun = NULL;
                        }
                        if (currList->silhouetteTint == COLLECTIBLE_SILHOUETTE_TINT_RED
                            || currList->silhouetteTint == COLLECTIBLE_SILHOUETTE_TINT_BLUE
                            || currList->silhouetteTint == COLLECTIBLE_SILHOUETTE_TINT_GREEN) {
                            gSPMatrix(tempGfxHead++, VIRTUAL_TO_PHYSICAL(&sCollectibleSilhouetteInset),
                                      G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
                        }
                        s32 marioCutout =
                            currList->marioSilhouette
                            && currList->silhouetteTint == MARIO_SILHOUETTE_TINT_GRAY
                            && (gMarioState->flags & (MARIO_VANISH_CAP | MARIO_TELEPORTING));
                        gSPDisplayList(tempGfxHead++,
                                       currList->marioSilhouette
                                               && currList->silhouetteTint == MARIO_SILHOUETTE_TINT_GRAY
                                               && (currList->silhouetteAlpha == 255 || marioCutout)
                                           ? dl_mario_dither_silhouette_begin
                                           : dl_silhouette_begin);
                        // Ordered cutouts already carry the visible model's opacity.
                        gDPSetFogColor(tempGfxHead++, red, green, blue,
                                       marioCutout ? 255 : currList->silhouetteAlpha);
                        if (geo_is_1up_sprite_display_list(currList->displayList)) {
                            tempGfxHead = geo_draw_1up_silhouette_display_list(tempGfxHead, currList,
                                                                               mode1, mode2);
                        } else {
                            gSPDisplayList(tempGfxHead++, currList->displayList);
                        }
                        gSPDisplayList(tempGfxHead++, dl_silhouette_end);
                    }
                } else {
                    // Add the current display list to the master list.
                    if (currList->blinkAlpha != 0) {
                        previousTexture = NULL;
                        reuseDisplayList = NULL;
                        tempGfxHead = geo_draw_blinking_display_list(tempGfxHead, currList,
                                                                     enableZBuffer, mode1, mode2);
                    } else if (geo_is_1up_sprite_display_list(currList->displayList)) {
                        previousTexture = NULL;
                        reuseDisplayList = NULL;
                        tempGfxHead = geo_draw_1up_cutout_display_list(tempGfxHead, currList,
                                                                       enableZBuffer, mode1, mode2);
                    } else {
#ifndef DISABLE_AA
                        // Keep the same coverage mode that covers Mario's
                        // silhouette, on this collectible's one ordinary draw.
                        s32 marioCutout =
                            currList->marioSilhouette && currLayer == LAYER_TRANSPARENT
                            && (gMarioState->flags & (MARIO_VANISH_CAP | MARIO_TELEPORTING));
                        s32 coverSilhouette =
                            marioCutout
                            || (enableZBuffer
                                && currList->silhouetteTint != COLLECTIBLE_SILHOUETTE_TINT_NONE
                                // Solid 1-ups use their own alpha-cutout draw above.
                                && currList->silhouetteTint != COLLECTIBLE_SILHOUETTE_TINT_GREEN);
                        if (coverSilhouette) {
                            // Partial triangle coverage must not blend the white
                            // wall copy into an accepted Vanish pixel.
                            u32 coverageMask = IM_RD | (marioCutout ? AA_EN : 0);
                            gDPPipeSync(tempGfxHead++);
                            gDPSetRenderMode(tempGfxHead++, mode1 & ~coverageMask,
                                             mode2 & ~coverageMask);
                        }
#endif
                        if (currList->textureReuse) {
                            tempGfxHead = geo_draw_texture_reuse_display_list(
                                tempGfxHead, currList, &previousTexture, &reuseDisplayList);
                        } else {
                            if (previousTexture != NULL) {
                                previousTexture = NULL;
                                reuseDisplayList = NULL;
                            }
                            gSPDisplayList(tempGfxHead++, currList->displayList);
                        }
#ifndef DISABLE_AA
                        if (coverSilhouette && !marioCutout) {
                            gDPPipeSync(tempGfxHead++);
                            gDPSetRenderMode(tempGfxHead++, mode1, mode2);
                        }
#endif
                    }
                }
                if (currList->marioSilhouette && currLayer == LAYER_TRANSPARENT
                    && currList->blinkAlpha == 0) {
                    // The ordered material temporarily changes cycle one's
                    // blender. Restore this layer before the next actor draws.
                    gDPPipeSync(tempGfxHead++);
                    gDPSetRenderMode(tempGfxHead++, mode1, mode2);
                }
#else
                // Add the current display list to the master list.
                if (currList->blinkAlpha != 0) {
                    previousTexture = NULL;
                    reuseDisplayList = NULL;
                    tempGfxHead = geo_draw_blinking_display_list(tempGfxHead, currList, enableZBuffer,
                                                                 mode1, mode2);
                } else if (geo_is_1up_sprite_display_list(currList->displayList)) {
                    previousTexture = NULL;
                    reuseDisplayList = NULL;
                    tempGfxHead = geo_draw_1up_cutout_display_list(tempGfxHead, currList, enableZBuffer,
                                                                   mode1, mode2);
                } else {
                    if (currList->textureReuse) {
                        tempGfxHead = geo_draw_texture_reuse_display_list(
                            tempGfxHead, currList, &previousTexture, &reuseDisplayList);
                    } else {
                        if (previousTexture != NULL) {
                            previousTexture = NULL;
                            reuseDisplayList = NULL;
                        }
                        gSPDisplayList(tempGfxHead++, currList->displayList);
                    }
                }
#endif
                // Move to the next DisplayListNode.
                currList = currList->next;
            }
#if SILHOUETTE
            if (silhouetteRun != NULL) {
                gSPDisplayList(tempGfxHead++, dl_mario_silhouette_end);
            }
#endif
        }
    }

    if (enableZBuffer) {
        // Disable z buffer.
        gDPPipeSync(tempGfxHead++);
        gSPClearGeometryMode(tempGfxHead++, G_ZBUFFER);
#ifdef VISUAL_DEBUG
        // Load the world scale identity matrix
        gSPMatrix(tempGfxHead++, &identityMatrixWorldScale, G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        if (surfaceView) visual_surface_loop(&tempGfxHead);
        render_debug_boxes(&tempGfxHead);
#endif
    }

    gDisplayListHead = tempGfxHead;
}

/**
 * Appends the display list to one of the master lists based on the layer
 * parameter. Look at the RenderModeContainer struct to see the corresponding
 * render modes of layers.
 */
#if SILHOUETTE
#define COLLECTIBLE_SILHOUETTE_HORIZONTAL_RADIUS 600.0f
#define COLLECTIBLE_SILHOUETTE_VERTICAL_RADIUS 300.0f
#define COLLECTIBLE_SILHOUETTE_FULL_ALPHA_FRACTION 0.2f
#define COLLECTIBLE_SILHOUETTE_MAX_ALPHA 127 // 49.8% of 255, never above 50%.
static u8 geo_collectible_silhouette_alpha(struct Object *obj, u8 *tint) {
    const BehaviorScript *behavior;
    struct GraphNode *model;
    f32 dx, dy, dz, scaledDistSq;

    if (gMarioState == NULL || !(obj->activeFlags & ACTIVE_FLAG_ACTIVE)) {
        return 0;
    }

    behavior = obj->behavior;
    model = obj->header.gfx.sharedChild;
    if (model == gLoadedGraphNodes[MODEL_RED_COIN]
        || model == gLoadedGraphNodes[MODEL_RED_COIN_NO_SHADOW]) {
        if (behavior != segmented_to_virtual(bhvRedCoin)) {
            return 0;
        }
        *tint = COLLECTIBLE_SILHOUETTE_TINT_RED;
    } else if (model == gLoadedGraphNodes[MODEL_BLUE_COIN]
               || model == gLoadedGraphNodes[MODEL_BLUE_COIN_NO_SHADOW]) {
        if (obj->oInteractType != INTERACT_COIN || obj->oIntangibleTimer != 0) {
            return 0;
        }
        *tint = COLLECTIBLE_SILHOUETTE_TINT_BLUE;
    } else if (model == gLoadedGraphNodes[MODEL_1UP]) {
        if (behavior != segmented_to_virtual(bhv1Up) && behavior != segmented_to_virtual(bhv1upWalking)
            && behavior != segmented_to_virtual(bhv1upRunningAway)
            && behavior != segmented_to_virtual(bhv1upSliding)
            && behavior != segmented_to_virtual(bhv1upJumpOnApproach)
            && behavior != segmented_to_virtual(bhvHidden1up)
            && behavior != segmented_to_virtual(bhvHidden1upInPole)) {
            return 0;
        }
        *tint = COLLECTIBLE_SILHOUETTE_TINT_GREEN;
    } else if (model == gLoadedGraphNodes[MODEL_STAR]) {
        if (behavior != segmented_to_virtual(bhvStar)
            && behavior != segmented_to_virtual(bhvStarSpawnCoordinates)
            && behavior != segmented_to_virtual(bhvSpawnedStar)
            && behavior != segmented_to_virtual(bhvSpawnedStarNoLevelExit)) {
            return 0;
        }
        if (behavior == segmented_to_virtual(bhvStarSpawnCoordinates)
            && obj->oAction < SPAWN_STAR_ARC_CUTSCENE_ACT_END) {
            return 0;
        }
        if ((behavior == segmented_to_virtual(bhvSpawnedStar)
             || behavior == segmented_to_virtual(bhvSpawnedStarNoLevelExit))
            && obj->oAction < SPAWN_STAR_POS_CUTSCENE_ACT_SLOW_STAR_ROTATION) {
            return 0;
        }
        if (obj->oIntangibleTimer != 0) {
            return 0;
        }
        *tint = COLLECTIBLE_SILHOUETTE_TINT_STAR;
    } else {
        return 0;
    }

    dx = obj->header.gfx.pos[0] - gMarioState->pos[0];
    dy = obj->header.gfx.pos[1] - gMarioState->pos[1];
    dz = obj->header.gfx.pos[2] - gMarioState->pos[2];
    // Measure to Mario's standing body, rather than only to his feet.
    if (dy < 0.0f) {
        dy = -dy;
    } else if (dy > 160.0f) {
        dy -= 160.0f;
    } else {
        dy = 0.0f;
    }
    scaledDistSq =
        (dx * dx + dz * dz)
            / (COLLECTIBLE_SILHOUETTE_HORIZONTAL_RADIUS * COLLECTIBLE_SILHOUETTE_HORIZONTAL_RADIUS)
        + dy * dy / (COLLECTIBLE_SILHOUETTE_VERTICAL_RADIUS * COLLECTIBLE_SILHOUETTE_VERTICAL_RADIUS);
    if (scaledDistSq >= 1.0f) {
        return 0;
    }
    if (scaledDistSq
        <= COLLECTIBLE_SILHOUETTE_FULL_ALPHA_FRACTION * COLLECTIBLE_SILHOUETTE_FULL_ALPHA_FRACTION) {
        return COLLECTIBLE_SILHOUETTE_MAX_ALPHA;
    }
    return (u8) (0.5f
                 + COLLECTIBLE_SILHOUETTE_MAX_ALPHA * (1.0f - sqrtf(scaledDistSq))
                       / (1.0f - COLLECTIBLE_SILHOUETTE_FULL_ALPHA_FRACTION));
}
#endif

static u8 geo_object_blend_alpha(void) {
    struct Object *obj;
    if (gCurGraphNodeHeldObject != NULL) {
        obj = gCurGraphNodeHeldObject->objNode;
        if (obj == NULL) {
            return 0;
        }
    } else if (gCurGraphNodeObject != NULL && gCurGraphNodeObject != &gMirrorMario) {
        obj = (struct Object *) gCurGraphNodeObject;
    } else {
        return 0;
    }
    // Child meshes (such as Mr. I's iris) share their spawning body's fade.
    struct Object *body = obj;
    if (obj->parentObj != NULL && obj->parentObj != obj && obj->parentObj->bowserArenaFade != 0) {
        body = obj->parentObj;
    }
    if (body->bowserArenaFade != 0) {
        // Zero denotes an ordinary draw in this pipeline; the initial 1/255
        // opacity keeps newly spawned models on the blending path.
        return MAX(1,
                   (BOWSER_ARENA_FADE_FRAMES - body->bowserArenaFade) * 255 / BOWSER_ARENA_FADE_FRAMES);
    }
    return (obj->activeFlags & ACTIVE_FLAG_BLINKING) ? OBJECT_BLINK_MIN_ALPHA : 0;
}

static u8 sTextureReuseEnvironment;

void geo_append_display_list(void *displayList, s32 layer) {
    u8 blinkAlpha = geo_object_blend_alpha();
#if SILHOUETTE
    s32 marioSilhouette = gCurGraphNodeObject != NULL && gCurGraphNodeHeldObject == NULL
                          && (gCurGraphNodeObject == (struct GraphNodeObject *) gMarioObject
                              || gCurGraphNodeObject == &gMirrorMario);
    // Damage blinking dims the visible body without leaving a wall-visible copy.
    s32 dimMarioSilhouette = marioSilhouette && gMarioState->invincTimer >= 3
                             && (game_options_get(GO_FLASHING) || gGlobalTimer % 5 >= 3);
    s32 showMarioSilhouette = game_options_get(GO_MARIO_SILHOUETTE) && !dimMarioSilhouette;
#endif
    // Another draw ends the run, so batching cannot reorder alpha sprites
    // around other objects, shadows, or generated geometry on this layer.
    geo_flush_bubble_batch();
#if SILHOUETTE
    if (gCurGraphNodeObject != NULL) {
        if ((gCurGraphNodeObject->node.flags & GRAPH_RENDER_SILHOUETTE)
            && (!marioSilhouette || showMarioSilhouette)) {
            switch (layer) {
                case LAYER_OPAQUE: layer = LAYER_SILHOUETTE_OPAQUE; break;
                case LAYER_ALPHA:  layer = LAYER_SILHOUETTE_ALPHA;  break;
            }
        }
        if (gCurGraphNodeObject->node.flags & GRAPH_RENDER_OCCLUDE_SILHOUETTE) {
            switch (layer) {
                case LAYER_OPAQUE: layer = LAYER_OCCLUDE_SILHOUETTE_OPAQUE; break;
                case LAYER_ALPHA:  layer = LAYER_OCCLUDE_SILHOUETTE_ALPHA;  break;
            }
        }
    }
#endif // F3DEX_GBI_2 || SILHOUETTE
#if SILHOUETTE
    s32 silhouetteSourceLayer = layer;
#endif
    u8 blinkOpaque =
        layer == LAYER_OPAQUE || layer == LAYER_OPAQUE_DECAL || layer == LAYER_OPAQUE_INTER;
#if SILHOUETTE
    blinkOpaque |= layer == LAYER_SILHOUETTE_OPAQUE || layer == LAYER_OCCLUDE_SILHOUETTE_OPAQUE;
#endif
    if (blinkAlpha != 0) {
        // Draw after solid geometry, without writing depth for the dim phase.
        switch (layer) {
            case LAYER_OPAQUE:
            case LAYER_ALPHA:
#if SILHOUETTE
            case LAYER_OCCLUDE_SILHOUETTE_OPAQUE:
            case LAYER_OCCLUDE_SILHOUETTE_ALPHA:
#endif
                layer = LAYER_TRANSPARENT;
                break;
            case LAYER_OPAQUE_DECAL:
#if SILHOUETTE
            case LAYER_ALPHA_DECAL:
#endif
                layer = LAYER_TRANSPARENT_DECAL;
                break;
            case LAYER_OPAQUE_INTER:
                layer = LAYER_TRANSPARENT_INTER;
                break;
        }
    }
    if (gCurGraphNodeMasterList != NULL) {
        struct DisplayListNode *listNode =
            alloc_only_pool_alloc(gDisplayListHeap, sizeof(struct DisplayListNode));

        // Transform-only nodes need the float matrix for their children, but
        // no fixed matrix until a display list actually draws at this depth.
        // Reuse it for subsequent draws at the same unchanged stack entry.
        if (gMatStackFixed[gMatStackIndex] == NULL) {
            Mtx *mtx = alloc_display_list(sizeof(*mtx));
            mtxf_to_mtx(mtx, gMatStack[gMatStackIndex]);
            gMatStackFixed[gMatStackIndex] = mtx;
        }
        listNode->transform = gMatStackFixed[gMatStackIndex];
        listNode->displayList = displayList;
        listNode->blinkAlpha = blinkAlpha;
        listNode->blinkOpaque = blinkOpaque;
        listNode->textureReuse = geo_object_allows_texture_reuse();
        listNode->textureReuseEnvironment = sTextureReuseEnvironment;
#if SILHOUETTE
        // Capture the owner while traversing: the master-list pass runs later.
        // Held objects retain their own silhouette colour.
        listNode->marioSilhouette = marioSilhouette;
        if (marioSilhouette && silhouetteSourceLayer == LAYER_TRANSPARENT
            && (gMarioState->flags & (MARIO_VANISH_CAP | MARIO_TELEPORTING))) {
            listNode->displayList = (void *) geo_mario_dither_material(displayList);
        }
        listNode->cyanSilhouette =
            marioSilhouette && mario_character_palette() == MARIO_PALETTE_HE_THICC;
        if (marioSilhouette) {
            listNode->silhouetteAlpha = dimMarioSilhouette ? 64 : 255;
        } else {
            listNode->silhouetteAlpha = SILHOUETTE;
        }
        listNode->silhouetteTint = COLLECTIBLE_SILHOUETTE_TINT_NONE;
        listNode->silhouetteGeometry = 0;
        if (layer == LAYER_SILHOUETTE_OPAQUE && !dimMarioSilhouette && gCurGraphNodeObject != NULL
            && gCurGraphNodeHeldObject == NULL
            && (gCurGraphNodeObject == (struct GraphNodeObject *) gMarioObject
                || gCurGraphNodeObject == &gMirrorMario)) {
            listNode->silhouetteGeometry = geo_mario_silhouette_geometry(displayList);
        }
#endif
        listNode->next = NULL;
        if (gCurGraphNodeMasterList->listHeads[layer] == NULL) {
            gCurGraphNodeMasterList->listHeads[layer] = listNode;
        } else {
            gCurGraphNodeMasterList->listTails[layer]->next = listNode;
        }
        gCurGraphNodeMasterList->listTails[layer] = listNode;
#if SILHOUETTE
        if (marioSilhouette && showMarioSilhouette && silhouetteSourceLayer == LAYER_TRANSPARENT
            && (gCurGraphNodeObject->node.flags & GRAPH_RENDER_SILHOUETTE)) {
            // Vanish/warp parts keep their ordinary translucent draw, plus a
            // separate wall-visible copy that never draws in the ordinary pass.
            struct DisplayListNode *silhouetteNode =
                alloc_only_pool_alloc(gDisplayListHeap, sizeof(*silhouetteNode));
            *silhouetteNode = *listNode;
            silhouetteNode->silhouetteTint = MARIO_SILHOUETTE_TINT_GRAY;
            // The ordered material retains the visible draw's transparent gaps.
            // Fixed wall-copy colour prevents overlapping triangles from
            // brightening the same accepted pixel repeatedly.
            silhouetteNode->silhouetteGeometry = 0;
            silhouetteNode->silhouetteAlpha = listNode->silhouetteAlpha;
            silhouetteNode->blinkAlpha = 0;
            s32 silhouetteLayer = LAYER_SILHOUETTE_ALPHA;
            if (gCurGraphNodeMasterList->listHeads[silhouetteLayer] == NULL) {
                gCurGraphNodeMasterList->listHeads[silhouetteLayer] = silhouetteNode;
            } else {
                gCurGraphNodeMasterList->listTails[silhouetteLayer]->next = silhouetteNode;
            }
            gCurGraphNodeMasterList->listTails[silhouetteLayer] = silhouetteNode;
        } else if (gCurGraphNodeObject != NULL && gCurGraphNodeHeldObject == NULL
                   && gCurGraphNodeObject != (struct GraphNodeObject *) gMarioObject
                   && gCurGraphNodeObject != &gMirrorMario
                   && (silhouetteSourceLayer == LAYER_OCCLUDE_SILHOUETTE_OPAQUE
                       || silhouetteSourceLayer == LAYER_OCCLUDE_SILHOUETTE_ALPHA)) {
            u8 tint = COLLECTIBLE_SILHOUETTE_TINT_NONE;
            u8 alpha = geo_collectible_silhouette_alpha((struct Object *) gCurGraphNodeObject, &tint);
            if (alpha != 0) {
                s32 silhouetteLayer = silhouetteSourceLayer == LAYER_OCCLUDE_SILHOUETTE_OPAQUE
                                          ? LAYER_SILHOUETTE_OPAQUE
                                          : LAYER_SILHOUETTE_ALPHA;
                struct DisplayListNode *silhouetteNode =
                    alloc_only_pool_alloc(gDisplayListHeap, sizeof(*silhouetteNode));
                listNode->silhouetteTint = tint; // Mark its ordinary draw for silhouette coverage.
                *silhouetteNode = *listNode;
                silhouetteNode->cyanSilhouette = FALSE;
                silhouetteNode->silhouetteAlpha = blinkAlpha ? MIN(alpha, blinkAlpha) : alpha;
                silhouetteNode->silhouetteTint = tint;
                silhouetteNode->blinkAlpha = 0; // The silhouette pass owns its blending.
                if (gCurGraphNodeMasterList->listHeads[silhouetteLayer] == NULL) {
                    gCurGraphNodeMasterList->listHeads[silhouetteLayer] = silhouetteNode;
                } else {
                    gCurGraphNodeMasterList->listTails[silhouetteLayer]->next = silhouetteNode;
                }
                gCurGraphNodeMasterList->listTails[silhouetteLayer] = silhouetteNode;
            }
        }
#endif
    }
}

static void inc_mat_stack() {
    gMatStackIndex++;
    // A sibling may reuse this depth with a different transform. Never reuse
    // its previous fixed matrix; geo_append_display_list materializes this one.
    gMatStackFixed[gMatStackIndex] = NULL;
}

static void append_dl_and_return(struct GraphNodeDisplayList *node) {
    if (node->displayList != NULL) {
        geo_append_display_list(node->displayList, GET_GRAPH_NODE_LAYER(node->node.flags));
    }
    if (node->node.children != NULL) {
        geo_process_node_and_siblings(node->node.children);
    }
    gMatStackIndex--;
}

/**
 * Process the master list node.
 */
void geo_process_master_list(struct GraphNodeMasterList *node) {
    s32 layer;

    if (gCurGraphNodeMasterList == NULL && node->node.children != NULL) {
        gCurGraphNodeMasterList = node;
        for (layer = LAYER_FIRST; layer < LAYER_COUNT; layer++) {
            node->listHeads[layer] = NULL;
#if defined(SHADOW_TEXTURE_REUSE) && !defined(DISABLE_SHADOWS)
            sShadowTextureRuns[layer].node = NULL;
            sShadowTextureRuns[layer].material = NULL;
#endif
        }
        geo_process_node_and_siblings(node->node.children);
        geo_flush_bubble_batch();
        geo_process_master_list_sub(gCurGraphNodeMasterList);
        gCurGraphNodeMasterList = NULL;
    }
}

/**
 * Process an orthographic projection node.
 */
 void geo_process_ortho_projection(struct GraphNodeOrthoProjection *node) {
    if (node->node.children != NULL) {
        Mtx *mtx = alloc_display_list(sizeof(*mtx));
        f32 scale = node->scale / 2.0f;
        f32 left = (gCurGraphNodeRoot->x - gCurGraphNodeRoot->width) * scale;
        f32 right = (gCurGraphNodeRoot->x + gCurGraphNodeRoot->width) * scale;
        f32 top = (gCurGraphNodeRoot->y - gCurGraphNodeRoot->height) * scale;
        f32 bottom = (gCurGraphNodeRoot->y + gCurGraphNodeRoot->height) * scale;

        guOrtho(mtx, left, right, bottom, top, -2.0f, 2.0f, 1.0f);
        gSPPerspNormalize(gDisplayListHead++, 0xFFFF);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);

        geo_process_node_and_siblings(node->node.children);
    }
}

/**
 * Process a perspective projection node.
 */
void geo_process_perspective(struct GraphNodePerspective *node) {
    if (node->fnNode.func != NULL) {
        node->fnNode.func(GEO_CONTEXT_RENDER, &node->fnNode.node, gMatStack[gMatStackIndex]);
    }
    if (node->fnNode.node.children != NULL) {
        u16 perspNorm;
        Mtx *mtx = alloc_display_list(sizeof(*mtx));
#ifdef WIDE
        gConfig.widescreen = game_options_get(GO_ASPECT_RATIO);
        if (gConfig.widescreen && gCurrLevelNum != 0x01) {
            sAspectRatio = 16.0f / 9.0f; // 1.775f
        } else {
            sAspectRatio = 4.0f / 3.0f; // 1.33333f
        }
#else
        sAspectRatio = 4.0f / 3.0f; // 1.33333f
#endif

        f32 vHalfFov = ( ((node->fov * 4096.f) + 8192.f) ) / 45.f;

        // We need to account for aspect ratio changes by multiplying by the widescreen horizontal
        // stretch (normally 1.775).
        node->halfFovHorizontal = tans(vHalfFov * sAspectRatio);

        node->halfFovVertical = tans(vHalfFov);

        // With low fovs, coordinate overflow can occur more easily. This slightly reduces precision only while zoomed in.
        f32 scale = node->fov < 28.0f ? remap(MAX(node->fov, 15), 15, 28, 0.5f, 1.0f): 1.0f;
        guPerspective(mtx, &perspNorm, node->fov, sAspectRatio, node->near / WORLD_SCALE, node->far / WORLD_SCALE, scale);

        gSPPerspNormalize(gDisplayListHead++, perspNorm);

        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);

        gCurGraphNodeCamFrustum = node;
        geo_process_node_and_siblings(node->fnNode.node.children);
        gCurGraphNodeCamFrustum = NULL;
    }
}

static f32 get_dist_from_camera(Vec3f pos) {
    return -((gCameraTransform[0][2] * pos[0])
           + (gCameraTransform[1][2] * pos[1])
           + (gCameraTransform[2][2] * pos[2])
           +  gCameraTransform[3][2]);
}

/**
 * Process a level of detail node. From the current transformation matrix,
 * the perpendicular distance to the camera is extracted and the children
 * of this node are only processed if that distance is within the render
 * range of this node.
 */
#define DOOR_SCREEN_LOD_MIN_DEPTH 2500.0f
#define DOOR_SCREEN_LOD_MAX_PIXELS 40.0f

#ifdef DOOR_SCREEN_LOD
static s32 geo_door_uses_far_mesh(struct GraphNodeLevelOfDetail *node) {
#ifdef AUTO_LOD
    if (!(gEmulator & EMU_CONSOLE)) {
        return FALSE;
    }
#endif
    struct GraphNode *child = node->node.children;
    if (child == NULL || child->type != GRAPH_NODE_TYPE_ANIMATED_PART || child->children != NULL
        || child->next != child || gCurGraphNodeRoot == NULL || gCurGraphNodeCamera == NULL
        || gCurGraphNodeCamFrustum == NULL || gCurGraphNodeCamera->rollScreen != 0
        || gCurGraphNodeRoot->width != SCREEN_WIDTH / 2
        || gCurGraphNodeRoot->height != SCREEN_HEIGHT / 2 || !(sAspectRatio >= 4.0f / 3.0f)
        || gCurGraphNodeObject == NULL || gCurGraphNodeHeldObject != NULL
        || gCurGraphNodeObject == &gMirrorMario
        || gCurGraphNodeObject == (struct GraphNodeObject *) gMarioObject) {
        return FALSE;
    }
#ifdef VISUAL_DEBUG
    if (hitboxView) {
        return FALSE;
    }
#endif
    // The projected bounds describe the closed pose. Keep animated opening
    // doors on their original ranges; the choice stays stable across both branches.
    if (gCurGraphNodeObject->animInfo.curAnim != NULL
        && (gCurGraphNodeObject->animInfo.animID != DOOR_ANIM_CLOSED
            || (gCurGraphNodeObjectNode->behavior != segmented_to_virtual(bhvDoor)
                && gCurGraphNodeObjectNode->behavior != segmented_to_virtual(bhvDoorWarp)))) {
        return FALSE;
    }
    struct GraphNodeAnimatedPart *part = (struct GraphNodeAnimatedPart *) child;
    if (part->translation[0] != 0 || part->translation[1] != 0 || part->translation[2] != 0) {
        return FALSE;
    }
    // Audit only the paired ordinary polished-door ranges, excluding star signs.
    if (!((node->minDistance == -2048 && node->maxDistance == 3500
           && part->displayList == door_seg3_dl_polished_wooden_door_near)
          || (node->minDistance == 3500 && node->maxDistance == 32767
              && part->displayList == door_seg3_dl_polished_wooden_door_far))) {
        return FALSE;
    }
    f32 fov = gCurGraphNodeCamFrustum->fov;
    if (!(fov > 0.0f && fov < 180.0f)) {
        return FALSE;
    }
    Mat4 cameraModel;
    mtxf_mul(cameraModel, gMatStack[gMatStackIndex], gCameraTransform);
    // The detailed body and handles fit [-16,600] x [0,1024] x [-160,160].
    Vec3f center = { 292.0f, 512.0f, 0.0f };
    Vec3f cameraCenter;
    Vec3f extent;
    linear_mtxf_mul_vec3f_and_translate(cameraModel, cameraCenter, center);
    for (s32 axis = 0; axis < 3; axis++) {
        extent[axis] = absf(cameraModel[0][axis]) * 308.0f + absf(cameraModel[1][axis]) * 512.0f
                       + absf(cameraModel[2][axis]) * 160.0f;
        if (!(absf(cameraCenter[axis]) <= 1.0e6f && extent[axis] <= 1.0e6f)) {
            return FALSE;
        }
    }
    f32 depth = -cameraCenter[2];
    if (!(depth >= DOOR_SCREEN_LOD_MIN_DEPTH && depth > extent[2])) {
        return FALSE;
    }
    s16 halfAngle = fov * (65536.0f / 720.0f);
    f32 vertical = sins(halfAngle) / coss(halfAngle);
    if (!(vertical > 0.0f)) {
        return FALSE;
    }
    f32 radius = MAX(extent[0], extent[1]);
    f32 offset = MAX(absf(cameraCenter[0]), absf(cameraCenter[1]));
    f32 pixels =
        SCREEN_HEIGHT * (radius + offset * extent[2] / depth) / (vertical * (depth - extent[2]));
    return pixels <= DOOR_SCREEN_LOD_MAX_PIXELS;
}
#endif

void geo_process_level_of_detail(struct GraphNodeLevelOfDetail *node) {
#ifdef AUTO_LOD
    f32 distanceFromCam =
        (gEmulator & EMU_CONSOLE) ? get_dist_from_camera(gMatStack[gMatStackIndex][3]) : 50.0f;
#else
    f32 distanceFromCam = get_dist_from_camera(gMatStack[gMatStackIndex][3]);
#endif

#ifdef DOOR_SCREEN_LOD
    if (distanceFromCam >= DOOR_SCREEN_LOD_MIN_DEPTH && distanceFromCam < 3500.0f
        && geo_door_uses_far_mesh(node)) {
        distanceFromCam = 3500.0f;
    }
#endif

    if ((f32) node->minDistance <= distanceFromCam && distanceFromCam < (f32) node->maxDistance
        && node->node.children != 0) {
        geo_process_node_and_siblings(node->node.children);
    }
}

// The extra appearance pass must not render Mario's held object twice.
static s32 sMarioCapBlinkOverlay;

void geo_process_switch(struct GraphNodeSwitchCase *node);

static void geo_append_cap_blink_alpha(Gfx *gfx, s32 alpha, s32 layer) {
    s32 blendAlpha = alpha;
    gDPPipeSync(&gfx[0]);
#if SILHOUETTE
    gDPSetAlphaCompare(&gfx[1], G_AC_NONE);
    if (gMarioState->flags & (MARIO_VANISH_CAP | MARIO_TELEPORTING)) {
        blendAlpha = 255;
    }
#else
    gDPSetAlphaCompare(&gfx[1], (gMarioState->flags & (MARIO_VANISH_CAP | MARIO_TELEPORTING))
                                    ? G_AC_DITHER
                                    : G_AC_NONE);
#endif
    gDPSetEnvColor(&gfx[2], 255, 255, 255, blendAlpha);
    gDPSetPrimColor(&gfx[3], 0, 0, 255, 255, 255, mario_dither_alpha(alpha));
    gSPEndDisplayList(&gfx[4]);
    geo_append_display_list(gfx, layer);
}

static void geo_process_cap_blink(struct GraphNodeSwitchCase *node) {
    struct MarioBodyState *body = &gBodyStates[node->numCases];
    s16 poweredModel = body->modelState;
    s8 poweredCap = body->capState;
    s8 poweredHand = body->handState;
    u8 animType = gCurrAnimType;
    u16 *animAttribute = gCurrAnimAttribute;
    s32 poweredAlpha =
        (poweredModel & MODEL_STATE_ALPHA) ? poweredModel & MODEL_STATE_MASK : MODEL_STATE_MASK;
    s32 ordinaryAlpha = (body->capBlinkModelState & MODEL_STATE_ALPHA)
                            ? body->capBlinkModelState & MODEL_STATE_MASK
                            : MODEL_STATE_MASK;
    s32 overlayAlpha;
    s32 baseAlpha;
    Gfx *alphaLists = alloc_display_list(15 * sizeof(*alphaLists));

    if (alphaLists == NULL) {
        geo_process_switch(node);
        return;
    }
    overlayAlpha = (poweredAlpha + 1) / 2;
    // Compensate for source-over composition so fades and damage blinks stay dark.
    baseAlpha = MIN(255, (ordinaryAlpha * 255 + (255 - overlayAlpha)) / (2 * (255 - overlayAlpha)));
    body->modelState = (body->capBlinkModelState & ~MODEL_STATE_MASK)
                       | (baseAlpha < 255 ? MODEL_STATE_ALPHA | baseAlpha : 0);
    body->capState = body->capBlinkCapState;
    body->handState = body->capBlinkHandState;
    geo_append_cap_blink_alpha(alphaLists, baseAlpha,
                               baseAlpha == 255 ? LAYER_OPAQUE : LAYER_TRANSPARENT);
    geo_process_switch(node);

    // Reuse the exact pose, without updating the animation clock or held object twice.
    gCurrAnimType = animType;
    gCurrAnimAttribute = animAttribute;
    body->modelState = (poweredModel & ~MODEL_STATE_MASK) | MODEL_STATE_ALPHA | overlayAlpha;
    body->capState = poweredCap;
    body->handState = poweredHand;
    geo_append_cap_blink_alpha(alphaLists + 5, overlayAlpha, LAYER_TRANSPARENT);
    sMarioCapBlinkOverlay = TRUE;
    geo_process_switch(node);
    sMarioCapBlinkOverlay = FALSE;

    body->modelState = poweredModel;
    geo_append_cap_blink_alpha(alphaLists + 10, poweredAlpha, LAYER_TRANSPARENT);
}

/**
 * Process a switch case node. The node's selection function is called
 * if it is 0, and among the node's children, only the selected child is
 * processed next.
 */
void geo_process_switch(struct GraphNodeSwitchCase *node) {
    struct GraphNode *selectedChild = node->fnNode.node.children;
    s32 i;

    if (node->fnNode.func == (GraphNodeFunc) geo_switch_mario_cap_effect
        && gBodyStates[node->numCases].capBlinkBlend == MARIO_CAP_BLINK_GEOMETRY) {
        // Disable recursion only for these two traversals; mirror draws can blend too.
        gBodyStates[node->numCases].capBlinkBlend = MARIO_CAP_BLINK_NONE;
        geo_process_cap_blink(node);
        gBodyStates[node->numCases].capBlinkBlend = MARIO_CAP_BLINK_GEOMETRY;
        return;
    }
    if (node->fnNode.func != NULL) {
        node->fnNode.func(GEO_CONTEXT_RENDER, &node->fnNode.node, gMatStack[gMatStackIndex]);
    }
    for (i = 0; selectedChild != NULL && node->selectedCase > i; i++) {
        selectedChild = selectedChild->next;
    }
    if (selectedChild != NULL) {
        geo_process_node_and_siblings(selectedChild);
    }
}

Mat4 gCameraTransform;

Lights1 defaultLight = gdSPDefLights1(
    0x3F, 0x3F, 0x3F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00
);

Vec3f globalLightDirection = { 0x28, 0x28, 0x28 };

void setup_global_light() {
    Lights1* curLight = (Lights1*)alloc_display_list(sizeof(Lights1));
    bcopy(&defaultLight, curLight, sizeof(Lights1));

#ifdef WORLDSPACE_LIGHTING
    curLight->l->l.dir[0] = (s8)(globalLightDirection[0]);
    curLight->l->l.dir[1] = (s8)(globalLightDirection[1]);
    curLight->l->l.dir[2] = (s8)(globalLightDirection[2]);
#else
    Vec3f transformedLightDirection;
    linear_mtxf_transpose_mul_vec3f(gCameraTransform, transformedLightDirection, globalLightDirection);
    curLight->l->l.dir[0] = (s8)(transformedLightDirection[0]);
    curLight->l->l.dir[1] = (s8)(transformedLightDirection[1]);
    curLight->l->l.dir[2] = (s8)(transformedLightDirection[2]);
#endif

    gSPSetLights1(gDisplayListHead++, (*curLight));
}

/**
 * Process a camera node.
 */
void geo_process_camera(struct GraphNodeCamera *node) {
    Mtx *rollMtx = alloc_display_list(sizeof(*rollMtx));
    Mtx *viewMtx = alloc_display_list(sizeof(Mtx));

    if (node->fnNode.func != NULL) {
        node->fnNode.func(GEO_CONTEXT_RENDER, &node->fnNode.node, gMatStack[gMatStackIndex]);
    }
    mtxf_rotate_xy(rollMtx, node->rollScreen);

    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(rollMtx), G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);

    mtxf_lookat(gCameraTransform, node->pos, node->focus, node->roll);

    // Calculate the lookAt
#ifdef F3DEX_GBI_2
    // @bug This is where the LookAt values should be calculated but aren't.
    // As a result, environment mapping is broken on Fast3DEX2 without the
    // changes below.
    Mat4* cameraMatrix = &gCameraTransform;
    /**
    * HackerSM64 2.1: Now uses the correct "up" vector for the guLookAtReflect call in geo_process_master_list_sub.
    * It was originally sideways in vanilla, with vanilla's environment map textures sideways to accommodate, but those
    * textures are now rotated automatically on extraction to allow for this to be fixed.
    */
    gCurLookAt->l[0].l.dir[0] = (s8)(127.0f * (*cameraMatrix)[0][0]);
    gCurLookAt->l[0].l.dir[1] = (s8)(127.0f * (*cameraMatrix)[1][0]);
    gCurLookAt->l[0].l.dir[2] = (s8)(127.0f * (*cameraMatrix)[2][0]);
    gCurLookAt->l[1].l.dir[0] = (s8)(127.0f * -(*cameraMatrix)[0][1]);
    gCurLookAt->l[1].l.dir[1] = (s8)(127.0f * -(*cameraMatrix)[1][1]);
    gCurLookAt->l[1].l.dir[2] = (s8)(127.0f * -(*cameraMatrix)[2][1]);
#endif // F3DEX_GBI_2

#if WORLD_SCALE > 1
    // Make a copy of the view matrix and scale its translation based on WORLD_SCALE
    Mat4 scaledCamera;
    mtxf_copy(scaledCamera, gCameraTransform);
    for (int i = 0; i < 3; i++) {
        scaledCamera[3][i] /= WORLD_SCALE;
    }

    // Convert the scaled matrix to fixed-point and integrate it into the projection matrix stack
    guMtxF2L(scaledCamera, viewMtx);
#else
    guMtxF2L(gCameraTransform, viewMtx);
#endif
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(viewMtx), G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
    setup_global_light();

    if ((gCurrLevelNum == LEVEL_PHYSICS_LAB || gWaterEffectViewRequested)
        && gCurGraphNodeCamFrustum != NULL) {
        physics_lab_prepare_view(gCameraTransform, gCurGraphNodeCamFrustum->halfFovHorizontal,
                                 gCurGraphNodeCamFrustum->halfFovVertical, sAspectRatio,
                                 node->rollScreen);
    }

    if (node->fnNode.node.children != 0) {
        gCurGraphNodeCamera = node;
        node->matrixPtr = &gCameraTransform;
        geo_process_node_and_siblings(node->fnNode.node.children);
        geo_render_compact_explosion_bubbles();
        gCurGraphNodeCamera = NULL;
    }
}

/**
 * Process a translation / rotation node. A transformation matrix based
 * on the node's translation and rotation is created and pushed on both
 * the float and fixed point matrix stacks.
 * For the rest it acts as a normal display list node.
 */
void geo_process_translation_rotation(struct GraphNodeTranslationRotation *node) {
    Vec3f translation;

    vec3s_to_vec3f(translation, node->translation);
    mtxf_rotate_zxy_and_translate_and_mul(node->rotation, translation, gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex]);

    inc_mat_stack();
    append_dl_and_return((struct GraphNodeDisplayList *)node);
}

/**
 * Process a translation node. A transformation matrix based on the node's
 * translation is created and pushed on both the float and fixed point matrix stacks.
 * For the rest it acts as a normal display list node.
 */
void geo_process_translation(struct GraphNodeTranslation *node) {
    Vec3f translation;

    vec3s_to_vec3f(translation, node->translation);
    mtxf_rotate_zxy_and_translate_and_mul(gVec3sZero, translation, gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex]);

    inc_mat_stack();
    append_dl_and_return((struct GraphNodeDisplayList *)node);
}

/**
 * Process a rotation node. A transformation matrix based on the node's
 * rotation is created and pushed on both the float and fixed point matrix stacks.
 * For the rest it acts as a normal display list node.
 */
void geo_process_rotation(struct GraphNodeRotation *node) {
    mtxf_rotate_zxy_and_translate_and_mul(node->rotation, gVec3fZero, gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex]);

    inc_mat_stack();
    append_dl_and_return(((struct GraphNodeDisplayList *)node));
}

// Native shoe burial only; movement and terrain support remain authoritative.
static f32 geo_mario_floor_lift(struct GraphNodeObject *object) {
    struct MarioState *m = gMarioState;
    struct AnimInfo *info = &object->animInfo;
    struct Animation *anim = info->curAnim;

    if (m == NULL || m->floor == NULL || anim == NULL || m->quicksandDepth != 0.0f
        || m->squishTimer != 0 || ABS(m->pos[1] - m->floorHeight) > 0.1f || m->floor->normal.y <= COS15
        || object->angle[0] != 0 || object->angle[2] != 0 || ABS(object->scale[0]) != 1.0f
        || object->scale[1] != 1.0f || object->scale[2] != 1.0f || info->animYTrans != 189
        || anim->animYTransDivisor != 189 || anim->unusedBoneCount != 20
        || (anim->flags & ANIM_FLAG_DISABLED)
        || (gBodyStates[0].punchState >> 6 == 2
            && (gBodyStates[0].punchState & PUNCH_STATE_TIMER_MASK) != 0)) {
        return 0.0f;
    }
    switch (m->action) {
        case ACT_IDLE:
        case ACT_STANDING_AGAINST_WALL:
        case ACT_FIRST_PERSON:
        case ACT_WALKING:
        case ACT_CROUCHING:
        case ACT_START_CROUCHING:
        case ACT_STOP_CROUCHING:
        case ACT_START_CRAWLING:
        case ACT_STOP_CRAWLING:
        case ACT_CRAWLING:
            break;
        default:
            return 0.0f;
    }
    // Tables describe native displayed poses. Blends and custom character poses
    // have different roots/bones, even when their gameplay animation ID matches.
    if (gCurrAnimData != segmented_to_virtual((void *) anim->values)
        || gCurrAnimAttribute != segmented_to_virtual((void *) anim->index)
        || gCurrAnimFrame != info->animFrame || gCurrAnimTranslationMultiplier != 1.0f) {
        return 0.0f;
    }
    for (u32 i = 0; i < ARRAY_COUNT(sMarioFloorLiftAnims); i++) {
        const struct MarioFloorLiftAnim *pose = &sMarioFloorLiftAnims[i];
        if (info->animID == pose->animID) {
            s32 frame = CLAMP(gCurrAnimFrame, 0, pose->frames - 1);
            // Correct animation burial, not the entire sloping footprint. Fade
            // the correction continuously to zero by 15 degrees to limit hover.
            f32 slope = CLAMP((m->floor->normal.y - COS15) / (1.0f - COS15), 0.0f, 1.0f);
            return sMarioFloorLiftDepth[pose->offset + frame] * 0.0625f * slope;
        }
    }
    return 0.0f;
}

/**
 * Process a scaling node. A transformation matrix based on the node's
 * scale is created and pushed on both the float and fixed point matrix stacks.
 * For the rest it acts as a normal display list node.
 */
void geo_process_scale(struct GraphNodeScale *node) {
    Vec3f scaleVec;

    vec3f_set(scaleVec, node->scale, node->scale, node->scale);
    mtxf_scale_vec3f(gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex], scaleVec);

    if (sMarioFloorLift != 0.0f && gCurGraphNodeObject != NULL && node->scale == 0.25f
        && node->node.parent != NULL && node->node.parent->type == GRAPH_NODE_TYPE_SHADOW
        && gCurGraphNodeHeldObject == NULL
        && (gCurGraphNodeObject == (struct GraphNodeObject *) gMarioObject
            || gCurGraphNodeObject == &gMirrorMario)) {
        // Mario's body scale follows his shadow. Move only the body matrix;
        // both visible/silhouette passes and hand attachments inherit this lift.
        gMatStack[gMatStackIndex + 1][3][1] += sMarioFloorLift;
    }

    inc_mat_stack();
    append_dl_and_return((struct GraphNodeDisplayList *)node);
}

/**
 * Process a billboard node. A transformation matrix is created that makes its
 * children face the camera, and it is pushed on the floating point and fixed
 * point matrix stacks.
 * For the rest it acts as a normal display list node.
 */
void geo_process_billboard(struct GraphNodeBillboard *node) {
    Vec3f translation;
    Vec3f axis;
    Vec3f camera;
    Vec3f scale = { 1.0f, 1.0f, 1.0f };

    vec3s_to_vec3f(translation, node->translation);
    linear_mtxf_mul_vec3(gMatStack[gMatStackIndex], axis, node->axis);
    vec3f_diff(camera, gCurGraphNodeCamera->focus, gCurGraphNodeCamera->pos);

    if (gCurGraphNodeHeldObject != NULL) {
        vec3f_copy(scale, gCurGraphNodeHeldObject->objNode->header.gfx.scale);
    } else if (gCurGraphNodeObject != NULL) {
        vec3f_copy(scale, gCurGraphNodeObject->scale);
    }

    if (node->isCylindrical) {
        mtxf_billboard_generic(gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex], camera, axis, translation, scale, gCurGraphNodeCamera->roll, TRUE);
    } else {
        mtxf_billboard(gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex], translation, scale, gCurGraphNodeCamera->roll);
    }

    inc_mat_stack();
    append_dl_and_return((struct GraphNodeDisplayList *)node);
}

/**
 * Process a display list node. It draws a display list without first pushing
 * a transformation on the stack, so all transformations are inherited from the
 * parent node. It processes its children if it has them.
 */
#define NO_CULLING_EMULATOR_WHITELIST (EMU_PROJECT64 | EMU_PARALLEL_LAUNCHER | EMU_MUPEN)

// Reject only boxes wholly outside a camera plane. Keep uncertain inputs.
s32 geo_camera_box_in_view(const Vec3f minimum, const Vec3f maximum) {
    if (gCurGraphNodeCamera == NULL || gCurGraphNodeCamFrustum == NULL) {
        return TRUE;
    }
    // Screen roll is applied to the projection, outside gCameraTransform.
    // Keep the box when this unrotated frustum cannot prove it is hidden.
    if (gCurGraphNodeCamera->rollScreen != 0) {
        return TRUE;
    }
#ifndef CULLING_ON_EMULATOR
    if ((gEmulator & NO_CULLING_EMULATOR_WHITELIST) && sAspectRatio > 4.0f / 3.0f) {
        return TRUE;
    }
#endif
#ifdef VISUAL_DEBUG
    if (hitboxView) {
        return TRUE;
    }
#endif
    f32 horizontal = gCurGraphNodeCamFrustum->halfFovHorizontal;
    f32 vertical = gCurGraphNodeCamFrustum->halfFovVertical;
    if (!(horizontal > 0.0f && horizontal < 4.0f && vertical > 0.0f && vertical < 4.0f)) {
        return TRUE;
    }
    Vec3f center, extent, camera;
    Vec3f bounds = { 0.0f, 0.0f, 0.0f };
    for (s32 axis = 0; axis < 3; axis++) {
        if (!(absf(minimum[axis]) < 100000.0f && absf(maximum[axis]) < 100000.0f
              && minimum[axis] <= maximum[axis])) {
            return TRUE;
        }
        center[axis] = (minimum[axis] + maximum[axis]) * 0.5f;
        extent[axis] = (maximum[axis] - minimum[axis]) * 0.5f;
    }
    linear_mtxf_mul_vec3f_and_translate(gCameraTransform, camera, center);
    for (s32 axis = 0; axis < 3; axis++) {
        for (s32 world = 0; world < 3; world++) {
            bounds[axis] += absf(gCameraTransform[world][axis]) * extent[world];
        }
        if (!(absf(camera[axis]) < 1000000.0f && bounds[axis] < 1000000.0f)) {
            return TRUE;
        }
    }
    // Four units retain clipping boundaries and fixed-point rounding margins.
    return !(camera[2] - bounds[2] > 4.0f
             || absf(camera[0]) > -camera[2] * horizontal + bounds[0] + bounds[2] * horizontal + 4.0f
             || absf(camera[1]) > -camera[2] * vertical + bounds[1] + bounds[2] * vertical + 4.0f);
}

static s32 geo_jrb_terrain_in_view(const Gfx *displayList) {
    // Bounds include every loaded native vertex; material order is unchanged.
    if (displayList == jrb_seg7_dl_07004940_culled) {
        const Vec3f minimum = { -6081.0f, -5635.0f, -673.0f };
        const Vec3f maximum = { 7996.0f, 1028.0f, 7713.0f };
        return geo_camera_box_in_view(minimum, maximum);
    }
    if (displayList == jrb_seg7_dl_07004C78_culled) {
        const Vec3f minimum = { -5260.0f, -4.0f, 1672.0f };
        const Vec3f maximum = { -701.0f, 1028.0f, 7215.0f };
        return geo_camera_box_in_view(minimum, maximum);
    }
    if (displayList == jrb_seg7_dl_070058C8_culled) {
        const Vec3f minimum = { 4634.0f, -4918.0f, -5007.0f };
        const Vec3f maximum = { 8170.0f, -1938.0f, 2436.0f };
        return geo_camera_box_in_view(minimum, maximum);
    }
    if (displayList == jrb_seg7_dl_070069B0_culled) {
        const Vec3f minimum = { -7822.0f, 733.0f, -1129.0f };
        const Vec3f maximum = { 5088.0f, 2974.0f, 7818.0f };
        return geo_camera_box_in_view(minimum, maximum);
    }
    if (displayList == jrb_seg7_dl_07007570_culled) {
        const Vec3f minimum = { -4274.0f, -2970.0f, -7567.0f };
        const Vec3f maximum = { 6485.0f, -914.0f, -903.0f };
        return geo_camera_box_in_view(minimum, maximum);
    }
    return TRUE;
}

void geo_process_display_list(struct GraphNodeDisplayList *node) {
    if (gCurrLevelNum == LEVEL_JRB && gCurGraphNodeObject == NULL
        && !geo_jrb_terrain_in_view(node->displayList)) {
        if (node->node.children != NULL) {
            geo_process_node_and_siblings(node->node.children);
        }
        return;
    }
    append_dl_and_return((struct GraphNodeDisplayList *)node);

    gMatStackIndex++;
}

/**
 * Process a generated list. Instead of storing a pointer to a display list,
 * the list is generated on the fly by a function.
 */
void geo_process_generated_list(struct GraphNodeGenerated *node) {
    if (node->fnNode.func != NULL) {
        Gfx *list = node->fnNode.func(GEO_CONTEXT_RENDER, &node->fnNode.node, (struct AllocOnlyPool *) gMatStack[gMatStackIndex]);

        if (list != NULL) {
            // Only these callbacks change alpha/tint without touching tiles or TMEM.
            u8 previousEnvironment = sTextureReuseEnvironment;
            sTextureReuseEnvironment = node->fnNode.func == geo_update_layer_transparency
                                       || node->fnNode.func == geo_update_small_water_splash_opacity;
            geo_append_display_list((void *) VIRTUAL_TO_PHYSICAL(list), GET_GRAPH_NODE_LAYER(node->fnNode.node.flags));
            sTextureReuseEnvironment = previousEnvironment;
        }
    }
    if (node->fnNode.node.children != NULL) {
        geo_process_node_and_siblings(node->fnNode.node.children);
    }
}

/**
 * Process a background node. Tries to retrieve a background display list from
 * the function of the node. If that function is null or returns null, a black
 * rectangle is drawn instead.
 */
void geo_process_background(struct GraphNodeBackground *node) {
    Gfx *list = NULL;

    if (node->fnNode.func != NULL) {
        list = node->fnNode.func(GEO_CONTEXT_RENDER, &node->fnNode.node,
                                 (struct AllocOnlyPool *) gMatStack[gMatStackIndex]);
    }
    if (list != NULL) {
        geo_append_display_list((void *) VIRTUAL_TO_PHYSICAL(list), GET_GRAPH_NODE_LAYER(node->fnNode.node.flags));
    } else if (gCurGraphNodeMasterList != NULL) {
#ifndef F3DEX_GBI_2E
        Gfx *gfxStart = alloc_display_list(sizeof(Gfx) * 7);
#else
        Gfx *gfxStart = alloc_display_list(sizeof(Gfx) * 8);
#endif
        Gfx *gfx = gfxStart;

        gDPPipeSync(gfx++);
        gDPSetCycleType(gfx++, G_CYC_FILL);
        gDPSetFillColor(gfx++, node->background);
        gDPFillRectangle(gfx++, GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(0), gBorderHeight,
        GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(0) - 1, SCREEN_HEIGHT - gBorderHeight - 1);
        gDPPipeSync(gfx++);
        gDPSetCycleType(gfx++, G_CYC_1CYCLE);
        gSPEndDisplayList(gfx++);

        geo_append_display_list((void *) VIRTUAL_TO_PHYSICAL(gfxStart), LAYER_FORCE);
    }
    if (node->fnNode.node.children != NULL) {
        geo_process_node_and_siblings(node->fnNode.node.children);
    }
}

/**
 * Render an animated part. The current animation state is not part of the node
 * but set in global variables. If an animated part is skipped, everything afterwards desyncs.
 */
void geo_process_animated_part(struct GraphNodeAnimatedPart *node) {
    Vec3s rotation = { 0, 0, 0 };
    Vec3f translation = { node->translation[0], node->translation[1], node->translation[2] };

    if (gCurrAnimType == ANIM_TYPE_TRANSLATION) {
        translation[0] += gCurrAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                          * gCurrAnimTranslationMultiplier;
        translation[1] += gCurrAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                          * gCurrAnimTranslationMultiplier;
        translation[2] += gCurrAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                          * gCurrAnimTranslationMultiplier;
        gCurrAnimType = ANIM_TYPE_ROTATION;
    } else {
        if (gCurrAnimType == ANIM_TYPE_LATERAL_TRANSLATION) {
            translation[0] +=
                gCurrAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                * gCurrAnimTranslationMultiplier;
            gCurrAnimAttribute += 2;
            translation[2] +=
                gCurrAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                * gCurrAnimTranslationMultiplier;
            gCurrAnimType = ANIM_TYPE_ROTATION;
        } else {
            if (gCurrAnimType == ANIM_TYPE_VERTICAL_TRANSLATION) {
                gCurrAnimAttribute += 2;
                translation[1] +=
                    gCurrAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)]
                    * gCurrAnimTranslationMultiplier;
                gCurrAnimAttribute += 2;
                gCurrAnimType = ANIM_TYPE_ROTATION;
            } else if (gCurrAnimType == ANIM_TYPE_NO_TRANSLATION) {
                gCurrAnimAttribute += 6;
                gCurrAnimType = ANIM_TYPE_ROTATION;
            }
        }
    }

    if (gCurrAnimType == ANIM_TYPE_ROTATION) {
        rotation[0] = gCurrAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)];
        rotation[1] = gCurrAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)];
        rotation[2] = gCurrAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)];
    }

    mtxf_rotate_xyz_and_translate_and_mul(rotation, translation, gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex]);

    inc_mat_stack();
    append_dl_and_return(((struct GraphNodeDisplayList *)node));
}

/**
 * Initialize the animation-related global variables for the currently drawn
 * object's animation.
 */
void geo_set_animation_globals(struct AnimInfo *node, s32 hasAnimation) {
    struct Animation *anim = node->curAnim;

    if (hasAnimation) {
        node->animFrame = geo_update_animation_frame(node, &node->animFrameAccelAssist);
    }
    node->animTimer = gAreaUpdateCounter;
    if (anim->flags & ANIM_FLAG_HOR_TRANS) {
        gCurrAnimType = ANIM_TYPE_VERTICAL_TRANSLATION;
    } else if (anim->flags & ANIM_FLAG_VERT_TRANS) {
        gCurrAnimType = ANIM_TYPE_LATERAL_TRANSLATION;
    } else if (anim->flags & ANIM_FLAG_NO_TRANS) {
        gCurrAnimType = ANIM_TYPE_NO_TRANSLATION;
    } else {
        gCurrAnimType = ANIM_TYPE_TRANSLATION;
    }

    gCurrAnimFrame = node->animFrame;
    gCurrAnimEnabled = (anim->flags & ANIM_FLAG_DISABLED) == 0;
    gCurrAnimAttribute = segmented_to_virtual((void *) anim->index);
    gCurrAnimData = segmented_to_virtual((void *) anim->values);

    if (anim->animYTransDivisor == 0) {
        gCurrAnimTranslationMultiplier = 1.0f;
    } else {
        gCurrAnimTranslationMultiplier = (f32) node->animYTrans / (f32) anim->animYTransDivisor;
    }

    struct MarioRenderPose pose = {
        gCurrAnimData,          gCurrAnimAttribute, gCurrAnimFrame, gCurrAnimTranslationMultiplier,
        MARIO_POSE_NATIVE_ROOT,
    };
    if ((gMarioObject != NULL && node == &gMarioObject->header.gfx.animInfo)
        || node == &gMirrorMario.animInfo) {
        mario_ground_pound_animation_render(node, gMarioState, &pose);
    }
    mario_transition_pose_render(node, &pose);
    // Object traversal initializes animation before installing its child-render context.
    if (gCurGraphNodeHeldObject == NULL
        && ((gMarioObject != NULL && node == &gMarioObject->header.gfx.animInfo)
            || node == &gMirrorMario.animInfo)) {
        mario_he_thicc_render_pose(node, &pose);
    }
    gCurrAnimData = pose.values;
    gCurrAnimAttribute = pose.index;
    gCurrAnimFrame = pose.frame;
    gCurrAnimTranslationMultiplier = pose.rootScale;
    if (pose.rootSpace == MARIO_POSE_RENDER_ROOT) {
        gCurrAnimType = ANIM_TYPE_TRANSLATION;
    }
}

/**
 * Process a shadow node. Renders a shadow under an object offset by the
 * translation of the first animated component and rotated according to
 * the floor below it.
 */
#ifndef DISABLE_SHADOWS
#ifdef SHADOW_TEXTURE_REUSE
static void geo_append_shadow_display_list(Gfx *displayList, s32 layer, s8 shadowType) {
    struct DisplayListNode *previous = NULL;
    const Gfx *material = shadowType == SHADOW_CIRCLE ? dl_shadow_circle : dl_shadow_square;
    if (gCurGraphNodeMasterList != NULL && gCurGraphNodeMasterList->listHeads[layer] != NULL) {
        previous = gCurGraphNodeMasterList->listTails[layer];
    }
    geo_append_display_list((void *) VIRTUAL_TO_PHYSICAL(displayList), layer);
    if (gCurGraphNodeMasterList != NULL) {
        struct DisplayListNode *current = gCurGraphNodeMasterList->listTails[layer];
        if (current->blinkAlpha == 0 && previous == sShadowTextureRuns[layer].node
            && material == sShadowTextureRuns[layer].material) {
            // The preceding shadow restored its material without overwriting TMEM.
            // Keep setup, synchronization, per-shadow alpha and the original quad.
            gSPDisplayList(displayList, dl_shadow_begin);
        }
        sShadowTextureRuns[layer].node = current;
        sShadowTextureRuns[layer].material = current->blinkAlpha == 0 ? material : NULL;
    }
}
#endif

static s32 geo_shadow_keeps_distance(void) {
    struct Object *obj = gCurGraphNodeObjectNode;
    u32 actorInteractions = INTERACT_HOOT | INTERACT_DAMAGE | INTERACT_KOOPA | INTERACT_SPINY_WALKING
                            | INTERACT_BOUNCE_TOP | INTERACT_BULLY | INTERACT_BOUNCE_TOP2
                            | INTERACT_MR_BLIZZARD | INTERACT_HIT_FROM_BELOW | INTERACT_TORNADO
                            | INTERACT_CLAM_OR_BUBBA | INTERACT_SHOCK;

    // Cap and shell boxes, including their inactive outlines, are supplies.
    if (obj->behavior == segmented_to_virtual(bhvExclamationBox)
        && obj->oBehParams2ndByte <= EXCLAMATION_BOX_BP_KOOPA_SHELL) {
        return TRUE;
    }

    // Keep shadows with visible characters, including enemies and talkable NPCs.
    // Signs and other scenery still use the lab's distant-shadow fade.
    return (obj->oInteractType & actorInteractions)
           || ((obj->oInteractType & INTERACT_GRABBABLE)
               && ((obj->oInteractionSubtype & (INT_SUBTYPE_HOLDABLE_NPC | INT_SUBTYPE_GRABS_MARIO))
                   || obj->behavior == segmented_to_virtual(bhvBobomb)
                   || obj->behavior == segmented_to_virtual(bhvChuckya)
                   || obj->behavior == segmented_to_virtual(bhvKingBobomb)
                   || obj->behavior == segmented_to_virtual(bhvBowser)))
           || ((obj->oInteractType & INTERACT_TEXT) && !(obj->oInteractionSubtype & INT_SUBTYPE_SIGN));
}

static u8 geo_shadow_solidity(u8 solidity) {
    if (gCurrLevelNum != LEVEL_PHYSICS_LAB || gCurGraphNodeObject == NULL
        || gCurGraphNodeHeldObject != NULL
        || gCurGraphNodeObject == (struct GraphNodeObject *) gMarioObject
        || gCurGraphNodeObject == &gMirrorMario) {
        return solidity;
    }
    // Camera depth is already available in unscaled world units. Reject before
    // animation offsets, floor/water queries, allocations and matrix work.
    f32 depth = -gCurGraphNodeObject->cameraToObject[2];
    if (depth <= 2000.0f || geo_shadow_keeps_distance()) {
        return solidity;
    }
    if (depth >= 3000.0f) {
        return 0;
    }
    if (depth > 2000.0f) {
        return (u8) (solidity * ((3000.0f - depth) * 0.001f));
    }
    return solidity;
}
#endif

void geo_process_shadow(struct GraphNodeShadow *node) {
#ifndef DISABLE_SHADOWS
    u8 solidity = geo_shadow_solidity(node->shadowSolidity);
    if ((solidity != 0 || node->shadowSolidity == 0) && gCurGraphNodeCamera != NULL
        && gCurGraphNodeObject != NULL) {
        Vec3f shadowPos;
        f32 shadowScale;

        if (gCurGraphNodeHeldObject != NULL) {
            vec3f_copy(shadowPos, gMatStack[gMatStackIndex][3]);
            shadowScale = node->shadowScale * gCurGraphNodeHeldObject->objNode->header.gfx.scale[0];
        } else {
            vec3f_copy(shadowPos, gCurGraphNodeObject->pos);
            shadowScale = node->shadowScale * gCurGraphNodeObject->scale[0];
        }

        s8 shifted = (gCurrAnimEnabled
                      && (gCurrAnimType == ANIM_TYPE_TRANSLATION
                       || gCurrAnimType == ANIM_TYPE_LATERAL_TRANSLATION)
        );

        if (shifted) {
            struct GraphNode *geo = node->node.children;
            f32 objScale = 1.0f;
            if (geo != NULL && geo->type == GRAPH_NODE_TYPE_SCALE) {
                objScale = ((struct GraphNodeScale *) geo)->scale;
            }

            f32 animScale = gCurrAnimTranslationMultiplier * objScale;
            Vec3f animOffset;
            animOffset[0] = gCurrAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)] * animScale;
            animOffset[1] = 0.0f;
            gCurrAnimAttribute += 2;
            animOffset[2] = gCurrAnimData[retrieve_animation_index(gCurrAnimFrame, &gCurrAnimAttribute)] * animScale;
            gCurrAnimAttribute -= 6;

            // simple matrix rotation so the shadow offset rotates along with the object
            f32 sinAng = sins(gCurGraphNodeObject->angle[1]);
            f32 cosAng = coss(gCurGraphNodeObject->angle[1]);

            shadowPos[0] += animOffset[0] * cosAng + animOffset[2] * sinAng;
            shadowPos[2] += -animOffset[0] * sinAng + animOffset[2] * cosAng;
        }

        Gfx *shadowList =
            create_shadow_below_xyz(shadowPos, shadowScale * 0.5f, solidity, node->shadowType, shifted);

        if (shadowList != NULL) {
            mtxf_shadow(gMatStack[gMatStackIndex + 1],
                gCurrShadow.floorNormal, shadowPos, gCurrShadow.scale, gCurGraphNodeObject->angle[1]);

            inc_mat_stack();
            s32 shadowLayer;
            if (gCurrShadow.hasPriority) {
                shadowLayer = LAYER_MARIO_SHADOW;
            } else {
                shadowLayer = gCurrShadow.isDecal ? LAYER_TRANSPARENT_DECAL : LAYER_TRANSPARENT;
            }
#ifdef SHADOW_TEXTURE_REUSE
            geo_append_shadow_display_list(shadowList, shadowLayer, node->shadowType);
#else
            geo_append_display_list((void *) VIRTUAL_TO_PHYSICAL(shadowList), shadowLayer);
#endif

            gMatStackIndex--;
        }
        shadowList = create_mario_wall_shadow(shadowPos, shadowScale * 0.5f, solidity);
        if (shadowList != NULL) {
            // A tangent yaw keeps the wall normal and shadow basis independent.
            s16 wallYaw = atan2s(-gCurrShadow.floorNormal[0], gCurrShadow.floorNormal[2]);
            mtxf_shadow(gMatStack[gMatStackIndex + 1], gCurrShadow.floorNormal, shadowPos,
                        gCurrShadow.scale, wallYaw);
            inc_mat_stack();
#ifdef SHADOW_TEXTURE_REUSE
            geo_append_shadow_display_list(shadowList, LAYER_MARIO_SHADOW, SHADOW_CIRCLE);
#else
            geo_append_display_list((void *) VIRTUAL_TO_PHYSICAL(shadowList), LAYER_MARIO_SHADOW);
#endif
            gMatStackIndex--;
        }
    }
#endif
    if (node->node.children != NULL) {
        geo_process_node_and_siblings(node->node.children);
    }
}

/**
 * Check whether an object is in view to determine whether if it should be drawn.
 * This is known as frustum culling.
 * It checks whether the object is far away, very close or behind the camera and
 * vertically or horizontally out of view.
 * The radius used is specified in DEFAULT_CULLING_RADIUS unless the object
 * has a culling radius node that specifies another value.
 *
 * The matrix parameter should be the top of the matrix stack, which is the
 * object's transformation matrix times the camera 'look-at' matrix. The math
 * is counter-intuitive, but it checks column 3 (translation vector) of this
 * matrix to determine where the origin (0,0,0) in object space will be once
 * transformed to camera space (x+ = right, y+ = up, z = 'coming out the screen').
 *
 * In 3D graphics, you typically model the world as being moved in front of a
 * static camera instead of a moving camera through a static world, which in
 * this case simplifies calculations. Note that the perspective matrix is not
 * on the matrix stack, so there are still calculations with the fov to compute
 * the slope of the lines of the frustum, these are done once during geo_process_perspective.
 *
 *        z-
 *
 *  \     |     /
 *   \    |    /
 *    \   |   /
 *     \  |  /
 *      \ | /
 *       \|/
 *        C       x+
 *
 * Since (0,0,0) is unaffected by rotation, columns 0, 1 and 2 are ignored.
 */

#if defined(VERTICAL_CULLING) && defined(SHADOW_VIEW_CULLING)
s32 geo_shadow_outside_vertical_view(Vec3f position, f32 radius) {
    if (gCurGraphNodeCamera == NULL || gCurGraphNodeCamFrustum == NULL
        || gCurGraphNodeCamera->rollScreen != 0 || !(radius >= 0.0f && radius <= 1.0e6f)) {
        return FALSE;
    }
#ifndef CULLING_ON_EMULATOR
    if ((gEmulator & NO_CULLING_EMULATOR_WHITELIST) && sAspectRatio > 4.0f / 3.0f) {
        return FALSE;
    }
#endif
#ifdef VISUAL_DEBUG
    if (hitboxView) {
        return FALSE;
    }
#endif
    f32 vertical = gCurGraphNodeCamFrustum->halfFovVertical;
    Vec3f cameraPosition;
    linear_mtxf_mul_vec3f_and_translate(gCameraTransform, cameraPosition, position);
    if (!(vertical > 0.0f && vertical <= 1.0e6f && absf(cameraPosition[2]) <= 1.0e30f)) {
        return FALSE;
    }
    // The radius includes the resolved quad on its receiving plane, not the body.
    return absf(cameraPosition[1]) > -cameraPosition[2] * vertical + radius * (1.0f + vertical);
}
#endif

#define SHADOW_TEXTURE_LOD_MAX_PIXELS 24.0f

#if defined(HD_SHADOWS) && defined(SHADOW_TEXTURE_LOD)
s32 geo_shadow_uses_small_texture(Vec3f position, f32 radius) {
    if (gCurGraphNodeRoot == NULL || gCurGraphNodeCamera == NULL || gCurGraphNodeCamFrustum == NULL
        || gCurGraphNodeCamera->rollScreen != 0 || gCurGraphNodeRoot->width != SCREEN_WIDTH / 2
        || gCurGraphNodeRoot->height != SCREEN_HEIGHT / 2 || !(radius > 0.0f && radius <= 1.0e6f)) {
        return FALSE;
    }
#ifndef CULLING_ON_EMULATOR
    if ((gEmulator & NO_CULLING_EMULATOR_WHITELIST) && sAspectRatio > 4.0f / 3.0f) {
        return FALSE;
    }
#endif
#ifdef VISUAL_DEBUG
    if (hitboxView) {
        return FALSE;
    }
#endif
    f32 fov = gCurGraphNodeCamFrustum->fov;
    if (!(fov > 0.0f && fov < 180.0f)) {
        return FALSE;
    }
    s16 halfAngle = fov * (65536.0f / 720.0f);
    f32 vertical = sins(halfAngle) / coss(halfAngle);
    Vec3f cameraPosition;
    linear_mtxf_mul_vec3f_and_translate(gCameraTransform, cameraPosition, position);
    f32 depth = -cameraPosition[2];
    f32 offset = MAX(absf(cameraPosition[0]), absf(cameraPosition[1]));
    if (!(vertical > 0.0f && depth > radius && depth <= 1.0e6f && absf(cameraPosition[0]) <= 1.0e6f
          && absf(cameraPosition[1]) <= 1.0e6f)) {
        return FALSE;
    }
    // Bound the projected sphere using its nearest depth, including off-axis
    // perspective expansion. Root dimensions store half the viewport size.
    f32 pixels = (2.0f * gCurGraphNodeRoot->height) * radius * (1.0f + offset / depth)
                 / (vertical * (depth - radius));
    return pixels <= SHADOW_TEXTURE_LOD_MAX_PIXELS;
}
#endif

static s32 geo_is_simple_small_sprite(struct Object *node) {
    struct GraphNode *geo = node->header.gfx.sharedChild;
    return geo != NULL && geo->type == GRAPH_NODE_TYPE_DISPLAY_LIST
           && (geo->flags & (GRAPH_RENDER_ACTIVE | GRAPH_RENDER_CHILDREN_FIRST)) == GRAPH_RENDER_ACTIVE
           && geo->children == NULL && geo->next == geo
           && (((struct GraphNodeDisplayList *) geo)->displayList == white_particle_small_dl
               || ((struct GraphNodeDisplayList *) geo)->displayList == cold_water_droplet_dl
               || ((struct GraphNodeDisplayList *) geo)->displayList == lava_droplet_dl)
           && GET_GRAPH_NODE_LAYER(geo->flags) == LAYER_OCCLUDE_SILHOUETTE_ALPHA
           && node->header.gfx.node.children == NULL && node->header.gfx.animInfo.curAnim == NULL
           && (node->header.gfx.node.flags & GRAPH_RENDER_BILLBOARD)
           && !(node->header.gfx.node.flags & GRAPH_RENDER_SILHOUETTE);
}

static s32 geo_is_aquarium_decoration(struct Object *node) {
    // The shadowed fish model must retain the usual shadow-safe culling.
    return (node->behavior == segmented_to_virtual(bhvSmallParticleBubbles)
            && geo_is_simple_small_sprite(node))
           || (node->behavior == segmented_to_virtual(bhvBlueFish)
               && node->header.gfx.sharedChild != NULL
               && node->header.gfx.sharedChild == gLoadedGraphNodes[MODEL_FISH]
               && node->header.gfx.sharedChild->type == GRAPH_NODE_TYPE_SCALE
               && node->header.gfx.node.children == NULL);
}

static s32 geo_aquarium_decoration_in_view(struct GraphNodeObject *node) {
    f32 depth = node->cameraToObject[2];
    f32 radius = DEFAULT_CULLING_RADIUS;
    f32 scale = MAX(1.0f, MAX(absf(node->scale[0]), MAX(absf(node->scale[1]), absf(node->scale[2]))));

    // Retain the legacy depth limits, including the near-plane margin.
    if (absf(depth + 10050.0f) >= 9950.0f + radius) {
        return FALSE;
    }
    // Screen roll rotates the projection after the camera transform. Keep the
    // depth-only fallback in that case, or without a perspective context.
    if (gCurGraphNodeCamFrustum == NULL || gCurGraphNodeCamera == NULL
        || gCurGraphNodeCamera->rollScreen != 0) {
        return TRUE;
    }
    radius *= scale;
    f32 horizontal = gCurGraphNodeCamFrustum->halfFovHorizontal;
    f32 vertical = gCurGraphNodeCamFrustum->halfFovVertical;
    // A sphere intersects a sloping plane beyond radius alone. The sum below
    // conservatively bounds radius * sqrt(1 + slope squared), including edges
    // near/behind the camera. Use the actual configured aspect ratio and FOV.
    return absf(node->cameraToObject[0]) <= -depth * horizontal + radius * (1.0f + absf(horizontal))
           && absf(node->cameraToObject[1]) <= -depth * vertical + radius * (1.0f + absf(vertical));
}

static s32 geo_bubble_batch_positions(struct Object *node, Vec3f position, Vec3s vertices[4]) {
    Vec3f delta;
    s32 i, axis;
    f32 width = 4.0f * node->header.gfx.scale[0] * BUBBLE_VERTEX_PRECISION;
    f32 height = 8.0f * node->header.gfx.scale[1] * BUBBLE_VERTEX_PRECISION;
    vec3f_diff(delta, position, sBubbleBatch.basis[3]);
    for (axis = 0; axis < 3; axis++) {
        f32 center = delta[axis] * BUBBLE_VERTEX_PRECISION;
        f32 right = width * sBubbleBatch.basis[0][axis];
        f32 up = height * sBubbleBatch.basis[1][axis];
        f32 coordinate[4] = {
            center - right,
            center + right,
            center + right + up,
            center - right + up,
        };
        for (i = 0; i < 4; i++) {
            // Reject large coordinates and NaNs before the s16 conversion.
            if (!(coordinate[i] >= -32767.0f && coordinate[i] <= 32767.0f)) {
                return FALSE;
            }
            vertices[i][axis] = coordinate[i] + ((coordinate[i] < 0.0f) ? -0.5f : 0.5f);
        }
    }
    return TRUE;
}

static s32 geo_try_batch_bubble(struct Object *node, Vec3f position) {
    // A dim blink needs its own blending state instead of the shared sprite material.
    if (node->activeFlags & ACTIVE_FLAG_BLINKING) {
        return FALSE;
    }
    Vec3s positions[4];
    s32 i, index;
    if (!geo_is_simple_small_sprite(node) || gCurGraphNodeMasterList == NULL) {
        return FALSE;
    }
    const Gfx *displayList =
        ((struct GraphNodeDisplayList *) node->header.gfx.sharedChild)->displayList;
    u8 red = 255, green = 255, blue = 255;
    if (displayList == lava_droplet_dl) {
        red = LIQUID_PARTICLE_LAVA_R;
        green = LIQUID_PARTICLE_LAVA_G;
        blue = LIQUID_PARTICLE_LAVA_B;
    } else if (displayList == cold_water_droplet_dl) {
        red = LIQUID_PARTICLE_COLD_R;
        green = LIQUID_PARTICLE_COLD_G;
        blue = LIQUID_PARTICLE_COLD_B;
    }
#ifdef VISUAL_DEBUG
    if (hitboxView) {
        return FALSE;
    }
#endif
    if (sBubbleBatch.count == 0) {
        Vec3f unitScale = { 1.0f, 1.0f, 1.0f };
        mtxf_billboard(sBubbleBatch.basis, gMatStack[gMatStackIndex], node->header.gfx.pos, unitScale,
                       gCurGraphNodeCamera->roll);
    }
    if (!geo_bubble_batch_positions(node, position, positions)) {
        if (sBubbleBatch.count == 0) {
            return FALSE; // Unusual sprite scale: use its ordinary model.
        }
        geo_flush_bubble_batch();
        return geo_try_batch_bubble(node, position); // Start near the next sprite.
    }
    if (sBubbleBatch.count == 0) {
        Mat4 matrix;
        Mtx *previous = gMatStackFixed[gMatStackIndex];
        Vec3f scale = { 1.0f / BUBBLE_VERTEX_PRECISION, 1.0f / BUBBLE_VERTEX_PRECISION,
                        1.0f / BUBBLE_VERTEX_PRECISION };
        sBubbleBatch.buffer = alloc_display_list(sizeof(struct BubbleBatchBuffer));
        if (sBubbleBatch.buffer == NULL) {
            return FALSE;
        }
        // Bake orientation into the vertices. The shared transform then has
        // exact power-of-two scale coefficients, avoiding fixed-point rotation
        // error growing with the distance between bubbles in a batch.
        mtxf_translate(matrix, sBubbleBatch.basis[3]);
        mtxf_scale_vec3f(matrix, matrix, scale);
        mtxf_to_mtx(&sBubbleBatch.buffer->transform, matrix);
        sBubbleBatch.head = sBubbleBatch.buffer->commands;
        gSPDisplayList(sBubbleBatch.head++, white_particle_small_dl_begin);
        gMatStackFixed[gMatStackIndex] = &sBubbleBatch.buffer->transform;
        geo_append_display_list(sBubbleBatch.buffer->commands, LAYER_OCCLUDE_SILHOUETTE_ALPHA);
        gMatStackFixed[gMatStackIndex] = previous;
    }
    index = sBubbleBatch.count % 4;
    if (index == 0) {
        sBubbleBatch.vertexCommand = sBubbleBatch.head;
        gSPVertex(sBubbleBatch.head++, &sBubbleBatch.buffer->vertices[sBubbleBatch.count * 4], 16, 0);
    }
    for (i = 0; i < 4; i++) {
        Vtx *vertex = &sBubbleBatch.buffer->vertices[sBubbleBatch.count * 4 + i];
        vec3s_copy(vertex->v.ob, positions[i]);
        vertex->v.flag = 0;
        vertex->v.tc[0] = (i == 1 || i == 2) ? 960 : 0;
        vertex->v.tc[1] = (i < 2) ? 960 : 0;
        vertex->v.cn[0] = red;
        vertex->v.cn[1] = green;
        vertex->v.cn[2] = blue;
        vertex->v.cn[3] = 255;
    }
    index *= 4;
    gSP1Quadrangle(sBubbleBatch.head++, index, index + 1, index + 2, index + 3, 0);
    if (++sBubbleBatch.count == BUBBLE_BATCH_CAPACITY) {
        geo_flush_bubble_batch();
    }
    return TRUE;
}

static void geo_render_compact_explosion_bubbles(void) {
    static struct Object sprite;
    s32 remaining = compact_explosion_bubbles_count();
    if (remaining == 0 || gCurGraphNodeCamera == NULL || gCurGraphNodeRoot == NULL
        || gLoadedGraphNodes[MODEL_WHITE_PARTICLE_SMALL] == NULL) {
        return;
    }
    sprite.behavior = segmented_to_virtual(bhvSmallParticleBubbles);
    sprite.header.gfx.sharedChild = gLoadedGraphNodes[MODEL_WHITE_PARTICLE_SMALL];
    sprite.header.gfx.node.flags = GRAPH_RENDER_BILLBOARD;
    for (s32 i = 0; i < COMPACT_EXPLOSION_BUBBLE_CAPACITY && remaining > 0; i++) {
        s16 areaIndex;
        Vec3f position;
        Vec3f scale;
        if (!compact_explosion_bubble_view(i, position, scale, &areaIndex)) {
            continue;
        }
        remaining--;
        if (areaIndex != gCurGraphNodeRoot->areaIndex) {
            continue;
        }
        vec3_copy(sprite.header.gfx.pos, position);
        vec3_copy(sprite.header.gfx.scale, scale);
        linear_mtxf_mul_vec3f_and_translate(gCameraTransform, sprite.header.gfx.cameraToObject,
                                            position);
        if (geo_aquarium_decoration_in_view(&sprite.header.gfx)) {
            geo_try_batch_bubble(&sprite, position);
        }
    }
}

#ifdef VERTICAL_CULLING
#ifdef TREE_VERTICAL_CULLING
static struct GraphNodeGenerated *geo_tree_transparency_node(struct GraphNode *root) {
    if (root->type != GRAPH_NODE_TYPE_CULLING_RADIUS
        || ((struct GraphNodeCullingRadius *) root)->cullingRadius != 800) {
        return NULL;
    }
    struct GraphNode *generated = root->children;
    if (generated == NULL || generated->type != GRAPH_NODE_TYPE_GENERATED_LIST
        || generated->children != NULL
        || (generated->flags & (GRAPH_RENDER_ACTIVE | GRAPH_RENDER_CHILDREN_FIRST))
               != GRAPH_RENDER_ACTIVE) {
        return NULL;
    }
    struct GraphNodeGenerated *transparency = (struct GraphNodeGenerated *) generated;
    struct GraphNode *next = generated->next;
    if (transparency->fnNode.func != geo_update_layer_transparency
        || transparency->parameter != GEO_TRANSPARENCY_MODE_INTER || next == NULL
        || next->type != GRAPH_NODE_TYPE_SWITCH_CASE || next->next != generated
        || (next->flags & (GRAPH_RENDER_ACTIVE | GRAPH_RENDER_CHILDREN_FIRST)) != GRAPH_RENDER_ACTIVE) {
        return NULL;
    }
    struct GraphNodeSwitchCase *frames = (struct GraphNodeSwitchCase *) next;
    struct GraphNode *opaque = next->children;
    if (frames->fnNode.func != geo_switch_anim_state || frames->numCases != 2 || opaque == NULL
        || opaque->type != GRAPH_NODE_TYPE_DISPLAY_LIST || opaque->children != NULL
        || opaque->next == NULL || opaque->next == opaque) {
        return NULL;
    }
    struct GraphNode *faded = opaque->next;
    if (faded->type != GRAPH_NODE_TYPE_DISPLAY_LIST || faded->children != NULL
        || faded->next != opaque) {
        return NULL;
    }
    const Gfx *a = ((struct GraphNodeDisplayList *) opaque)->displayList;
    const Gfx *b = ((struct GraphNodeDisplayList *) faded)->displayList;
    // These exact drawing alternatives fit the existing 800-unit tree sphere.
    if (!((a == tree_seg3_dl_bubbly && b == tree_seg3_dl_bubbly_transparent)
          || (a == tree_seg3_dl_spiky && b == tree_seg3_dl_spiky_transparent)
          || (a == tree_seg3_dl_snowy_pine && b == tree_seg3_dl_snowy_pine_transparent)
          || (a == tree_seg3_dl_palm && b == tree_seg3_dl_palm_transparent))) {
        return NULL;
    }
    return transparency;
}
#endif

static s32 geo_particle_outside_vertical_view(struct GraphNodeObject *node, s32 *cameraPositionReady) {
    struct GraphNode *geo = node->sharedChild;
    struct GraphNodeSwitchCase *switchCase = NULL;
    f32 meshRadius = 32.0f;
    // Only audited shadow-free sprites and trees without bones or attachments.
    if (geo == NULL
        || (geo->flags & (GRAPH_RENDER_ACTIVE | GRAPH_RENDER_CHILDREN_FIRST)) != GRAPH_RENDER_ACTIVE
        || geo->next != geo || node->node.children != NULL || node->animInfo.curAnim != NULL
        || node->throwMatrix != NULL || !(node->node.flags & GRAPH_RENDER_BILLBOARD)
        || (node->node.flags & GRAPH_RENDER_SILHOUETTE)) {
        return FALSE;
    }
    if (geo->type == GRAPH_NODE_TYPE_DISPLAY_LIST) {
        const Gfx *displayList = ((struct GraphNodeDisplayList *) geo)->displayList;
        if (geo->children != NULL
            || (displayList != white_particle_small_dl && displayList != white_particle_small_unused_dl
                && displayList != lava_droplet_dl && displayList != cold_water_droplet_dl
                && displayList != white_particle_dl && displayList != sand_seg3_dl_particle
                && displayList != pebble_seg3_dl_0301CB00)) {
            return FALSE;
        }
    }
#ifdef TREE_VERTICAL_CULLING
    else if (geo->type == GRAPH_NODE_TYPE_CULLING_RADIUS) {
        if (gCurGraphNodeHeldObject != NULL || geo_tree_transparency_node(geo) == NULL) {
            return FALSE;
        }
        meshRadius = 800.0f;
    }
#endif
    else {
        s16 frameCount;
        if (geo == gLoadedGraphNodes[MODEL_BUBBLE] || geo == gLoadedGraphNodes[MODEL_PURPLE_MARBLE]) {
            meshRadius = 36.0f;
            frameCount = 1;
        } else if (geo == gLoadedGraphNodes[MODEL_SPARKLES]) {
            meshRadius = 72.0f;
            frameCount = 12;
        } else if (geo == gLoadedGraphNodes[MODEL_EXPLOSION] && geo->type == GRAPH_NODE_TYPE_START) {
            meshRadius = 182.0f;
            frameCount = 9;
            geo = geo->children;
        } else {
            return FALSE;
        }
        if (geo == NULL || geo->type != GRAPH_NODE_TYPE_SWITCH_CASE || geo->next != geo
            || (geo->flags & (GRAPH_RENDER_ACTIVE | GRAPH_RENDER_CHILDREN_FIRST)) != GRAPH_RENDER_ACTIVE
            || gCurGraphNodeHeldObject != NULL) {
            return FALSE;
        }
        switchCase = (struct GraphNodeSwitchCase *) geo;
        s32 frame = ((struct Object *) node)->oAnimState;
        if (switchCase->fnNode.func != geo_switch_anim_state || switchCase->numCases != frameCount
            || frame < 0 || frame >= frameCount) {
            // Invalid frames retain the callback's native reset behavior.
            return FALSE;
        }
    }
    if (gCurGraphNodeCamera == NULL || gCurGraphNodeCamFrustum == NULL
        || gCurGraphNodeCamera->rollScreen != 0) {
        return FALSE;
    }
#ifndef CULLING_ON_EMULATOR
    if ((gEmulator & NO_CULLING_EMULATOR_WHITELIST) && sAspectRatio > 4.0f / 3.0f) {
        return FALSE;
    }
#endif
#ifdef VISUAL_DEBUG
    if (hitboxView) {
        return FALSE;
    }
#endif
    f32 scale = 1.0f;
    for (s32 axis = 0; axis < 3; axis++) {
        f32 magnitude = absf(node->scale[axis]);
        if (!(magnitude <= 1.0e6f)) {
            return FALSE;
        }
        scale = MAX(scale, magnitude);
    }
    f32 vertical = gCurGraphNodeCamFrustum->halfFovVertical;
    Vec3f position;
    vec3f_copy(position, node->pos);
    vec3f_add(position, gMatStack[gMatStackIndex][3]);
    linear_mtxf_mul_vec3f_and_translate(gCameraTransform, node->cameraToObject, position);
    *cameraPositionReady = TRUE;
    f32 depth = node->cameraToObject[2];
    if (!(vertical > 0.0f && vertical <= 1.0e6f && absf(depth) <= 1.0e30f)) {
        return FALSE;
    }
    // Bounds include every drawing alternative; retain the ordinary margin.
    f32 radius = MAX(DEFAULT_CULLING_RADIUS, meshRadius * scale);
    s32 outside = absf(node->cameraToObject[1]) > -depth * vertical + radius * (1.0f + vertical);
    if (outside && switchCase != NULL) {
        // Valid frame selection is the callback's only remaining state change.
        switchCase->selectedCase = ((struct Object *) node)->oAnimState;
    }
    return outside;
}
#endif

// The audited JRB layouts have no shadows or attachments. Seaweed's selector
// only chooses immutable geometry and does not move its conservative bounds.
// Their native animations have zero root translation and a zero divisor.
// Keep model-specific checks out of the hot object traversal's instruction footprint.
static s32 geo_jrb_object_outside_vertical_view(struct Object *object) __attribute__((noinline));
static s32 geo_jrb_object_outside_vertical_view(struct Object *object) {
    struct GraphNodeObject *node = &object->header.gfx;
    const struct Animation *const *animations;
    f32 radius;
    s32 animationCount;

    if (gCurrLevelNum != LEVEL_JRB || node->areaIndex != 1 || node->sharedChild == NULL
        || node->node.children != NULL || node->throwMatrix != NULL
        || (node->node.flags & (GRAPH_RENDER_BILLBOARD | GRAPH_RENDER_SILHOUETTE))
        || gCurGraphNodeHeldObject != NULL || node->animInfo.curAnim == NULL
        || gCurGraphNodeCamera == NULL || gCurGraphNodeCamFrustum == NULL
        || gCurGraphNodeCamera->rollScreen != 0) {
        return FALSE;
    }
#ifndef CULLING_ON_EMULATOR
    if ((gEmulator & NO_CULLING_EMULATOR_WHITELIST) && sAspectRatio > 4.0f / 3.0f) {
        return FALSE;
    }
#endif
#ifdef VISUAL_DEBUG
    if (hitboxView) {
        return FALSE;
    }
#endif
    if (node->sharedChild == gLoadedGraphNodes[MODEL_SEAWEED]) {
        animations = segmented_to_virtual(seaweed_seg6_anims_0600A4D4);
        animationCount = 1;
        radius = 300.0f;
    } else if (node->sharedChild == gLoadedGraphNodes[MODEL_CLAM_SHELL]) {
        animations = segmented_to_virtual(clam_shell_seg5_anims_05001744);
        animationCount = 2;
        radius = 448.0f;
    } else {
        return FALSE;
    }
    s32 knownAnimation = FALSE;
    for (s32 i = 0; i < animationCount; i++) {
        knownAnimation |= node->animInfo.curAnim == segmented_to_virtual(animations[i]);
    }
    if (!knownAnimation || node->animInfo.curAnim->animYTransDivisor != 0) {
        return FALSE;
    }
    f32 scale = 1.0f;
    for (s32 axis = 0; axis < 3; axis++) {
        f32 magnitude = absf(node->scale[axis]);
        if (!(magnitude <= 1.0e6f)) {
            return FALSE;
        }
        scale = MAX(scale, magnitude);
    }
    f32 vertical = gCurGraphNodeCamFrustum->halfFovVertical;
    return absf(node->cameraToObject[1])
           > -node->cameraToObject[2] * vertical + radius * scale * (1.0f + vertical) + 2.0f;
}

static s32 sJrbBayVisible = TRUE;
static s32 sJrbBayTerrain;
// Strict boundaries restore full terrain. The two tunnel regions share a mask.
static const f32 sJrbBayTerrainRegions[3][2][3] = {
    { { 4700.0f, -2900.0f, -7500.0f }, { 5600.0f, -2000.0f, -3100.0f } },
    { { 4700.0f, -4100.0f, -3100.0f }, { 5600.0f, -2400.0f, -1900.0f } },
    { { 4700.0f, -4800.0f, -1900.0f }, { 5600.0f, -3200.0f, -800.0f } }
};
// These three opaque floor triangles are audited against the native terrain.
static const Vec3f sJrbOccluderVertices[][3] = {
    { { -219.0f, 512.0f, 5643.0f }, { -1619.0f, 512.0f, 6015.0f }, { -805.0f, 512.0f, 7672.0f } },
    { { -1662.0f, -1023.0f, 3030.0f },
      { -2876.0f, -1023.0f, 4101.0f },
      { -2690.0f, -1023.0f, 4801.0f } },
    { { -1662.0f, -1023.0f, 3030.0f },
      { -2362.0f, -1023.0f, 3215.0f },
      { -2876.0f, -1023.0f, 4101.0f } }
};
struct JrbOccluder {
    Vec3f edge[3];
    f32 edgeLengthSquared[3];
    s32 active;
};
static struct JrbOccluder sJrbOccluders[3];
static Vec3f sJrbOcclusionCamera;

static void geo_jrb_prepare_occluders(void) {
    for (s32 i = 0; i < 3; i++) {
        struct JrbOccluder *occluder = &sJrbOccluders[i];
        occluder->active = FALSE;
        if (gCurGraphNodeCamera == NULL) {
            continue;
        }
        Vec3f relative[3];
        for (s32 vertex = 0; vertex < 3; vertex++) {
            for (s32 axis = 0; axis < 3; axis++) {
                relative[vertex][axis] =
                    sJrbOccluderVertices[i][vertex][axis] - gCurGraphNodeCamera->pos[axis];
            }
        }
        // Native winding faces upward; the underside cannot occlude anything.
        if (!(gCurGraphNodeCamera->pos[1] > sJrbOccluderVertices[i][0][1] + 4.0f)) {
            continue;
        }
        occluder->active = TRUE;
        for (s32 edge = 0; edge < 3; edge++) {
            const f32 *a = relative[edge];
            const f32 *b = relative[(edge + 1) % 3];
            f32 *normal = occluder->edge[edge];
            normal[0] = (a[1] * b[2] - a[2] * b[1]) * 0.000001f;
            normal[1] = (a[2] * b[0] - a[0] * b[2]) * 0.000001f;
            normal[2] = (a[0] * b[1] - a[1] * b[0]) * 0.000001f;
            const f32 *inside = relative[(edge + 2) % 3];
            if (normal[0] * inside[0] + normal[1] * inside[1] + normal[2] * inside[2] < 0.0f) {
                normal[0] = -normal[0];
                normal[1] = -normal[1];
                normal[2] = -normal[2];
            }
            occluder->edgeLengthSquared[edge] =
                normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2];
        }
    }
    if (gCurGraphNodeCamera != NULL) {
        vec3f_copy(sJrbOcclusionCamera, gCurGraphNodeCamera->pos);
    }
}

static s32 geo_jrb_object_behind_terrain(struct GraphNodeObject *node) __attribute__((noinline));
static s32 geo_jrb_object_behind_terrain(struct GraphNodeObject *node) {
    if (node->sharedChild == NULL || node->node.children != NULL || node->throwMatrix != NULL
        || gCurGraphNodeHeldObject != NULL || gCurGraphNodeCamera == NULL
        || (node->node.flags & (GRAPH_RENDER_BILLBOARD | GRAPH_RENDER_SILHOUETTE))) {
        return FALSE;
    }
#ifdef VISUAL_DEBUG
    if (hitboxView) {
        return FALSE;
    }
#endif
    f32 radius;
    if (node->sharedChild == gLoadedGraphNodes[MODEL_SEAWEED]) {
        const struct Animation *const *animations = segmented_to_virtual(seaweed_seg6_anims_0600A4D4);
        if (node->animInfo.curAnim != segmented_to_virtual(animations[0])) {
            return FALSE;
        }
        radius = 300.0f;
    } else if (node->sharedChild == gLoadedGraphNodes[MODEL_JRB_ROCK]
               && node->animInfo.curAnim == NULL) {
        radius = 1100.0f;
    } else {
        return FALSE;
    }
    f32 scale = 1.0f;
    Vec3f relative;
    for (s32 axis = 0; axis < 3; axis++) {
        f32 magnitude = absf(node->scale[axis]);
        if (!(magnitude <= 64.0f) || !(absf(node->pos[axis]) <= 20000.0f)
            || !(absf(sJrbOcclusionCamera[axis]) <= 20000.0f)) {
            return FALSE;
        }
        scale = MAX(scale, magnitude);
        relative[axis] = node->pos[axis] - sJrbOcclusionCamera[axis];
    }
    radius = radius * scale + 4.0f;
    for (s32 i = 0; i < 3; i++) {
        const struct JrbOccluder *occluder = &sJrbOccluders[i];
        if (!occluder->active || !(node->pos[1] + radius < sJrbOccluderVertices[i][0][1])) {
            continue;
        }
        s32 inside = TRUE;
        for (s32 edge = 0; edge < 3; edge++) {
            const f32 *normal = occluder->edge[edge];
            f32 distance = normal[0] * relative[0] + normal[1] * relative[1] + normal[2] * relative[2];
            if (!(distance > 0.0f)
                || !(distance * distance > radius * radius * occluder->edgeLengthSquared[edge])) {
                inside = FALSE;
                break;
            }
        }
        if (inside) {
            return TRUE;
        }
    }
    return FALSE;
}

static s32 geo_jrb_tunnel_exit_in_view(void) {
    // A padded box encloses the only opening from the bay into the tunnel.
    // Retain the bay if any part of this box might enter the camera frustum.
    Vec3f center = { 5060.0f, -3531.0f, 675.0f };
    Vec3f camera;
    linear_mtxf_mul_vec3f_and_translate(gCameraTransform, camera, center);
    const f32 extent[3] = { 600.0f, 1700.0f, 100.0f };
    Vec3f bounds = { 0.0f, 0.0f, 0.0f };
    for (s32 axis = 0; axis < 3; axis++) {
        for (s32 world = 0; world < 3; world++) {
            bounds[axis] += absf(gCameraTransform[world][axis]) * extent[world];
        }
    }
    f32 horizontal = gCurGraphNodeCamFrustum->halfFovHorizontal;
    f32 vertical = gCurGraphNodeCamFrustum->halfFovVertical;
    if (!(horizontal > 0.0f && horizontal < 4.0f && vertical > 0.0f && vertical < 4.0f)) {
        return TRUE;
    }
    return !(camera[2] - bounds[2] > 4.0f
             || absf(camera[0]) > -camera[2] * horizontal + bounds[0] + bounds[2] * horizontal + 4.0f
             || absf(camera[1]) > -camera[2] * vertical + bounds[1] + bounds[2] * vertical + 4.0f);
}

static s32 geo_jrb_bay_terrain_region(s32 callContext) {
    if (callContext != GEO_CONTEXT_RENDER || gCurrLevelNum != LEVEL_JRB || gMarioState == NULL
        || gCurGraphNodeCamera == NULL || gCurGraphNodeCamFrustum == NULL || gCurGraphNodeRoot == NULL
        || gCurGraphNodeRoot->areaIndex != 1 || gCurGraphNodeRoot->height != SCREEN_HEIGHT / 2
        || gCurGraphNodeCamera->rollScreen != 0 || gCurGraphNodeObject != NULL
        || gCurGraphNodeHeldObject != NULL
        || !(gMarioState->pos[1] < -1942.0f && gMarioState->pos[2] < 675.0f)) {
        return 0;
    }
#ifdef VISUAL_DEBUG
    if (hitboxView) {
        return 0;
    }
#endif
    f32 horizontal = gCurGraphNodeCamFrustum->halfFovHorizontal;
    f32 vertical = gCurGraphNodeCamFrustum->halfFovVertical;
    if (!(horizontal > 0.0f && horizontal < 4.0f && vertical > 0.0f && vertical < 4.0f)) {
        return 0;
    }
    const f32 *position = gCurGraphNodeCamera->pos;
    // Stay inside the native cavity, including its sloping floor and ceiling.
    // The portal proof alone cannot justify cameras on the far side of a wall.
    if (!(position[0] > 4950.0f && position[0] < 5350.0f)) {
        return 0;
    }
    for (s32 region = 0; region < 3; region++) {
        f32 slopeHeight = position[1] + 0.6f * position[2];
        s32 inside =
            region == 0 ? position[1] < -2300.0f : slopeHeight > -5050.0f && slopeHeight < -4550.0f;
        if (inside && position[0] > sJrbBayTerrainRegions[region][0][0]
            && position[0] < sJrbBayTerrainRegions[region][1][0]
            && position[1] > sJrbBayTerrainRegions[region][0][1]
            && position[1] < sJrbBayTerrainRegions[region][1][1]
            && position[2] > sJrbBayTerrainRegions[region][0][2]
            && position[2] < sJrbBayTerrainRegions[region][1][2]) {
            return region == 0 ? 1 : 2;
        }
    }
    return 0;
}

// The outer bay switch computes the mask once for all three terrain roots.
Gfx *geo_switch_jrb_bay_terrain(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    ((struct GraphNodeSwitchCase *) node)->selectedCase =
        callContext == GEO_CONTEXT_RENDER ? sJrbBayTerrain : 0;
    return NULL;
}

Gfx *geo_switch_jrb_bay(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    sJrbBayVisible = TRUE;
    sJrbBayTerrain = geo_jrb_bay_terrain_region(callContext);
    geo_jrb_prepare_occluders();
    if (callContext == GEO_CONTEXT_RENDER && gMarioState != NULL && gCurGraphNodeCamera != NULL
        && gCurGraphNodeCamFrustum != NULL && gCurGraphNodeCamera->rollScreen == 0
        && gMarioState->pos[1] < -900.0f && gMarioState->pos[2] < -1900.0f
        && gCurGraphNodeCamera->pos[1] < -900.0f && gCurGraphNodeCamera->pos[2] < -1900.0f) {
        sJrbBayVisible = geo_jrb_tunnel_exit_in_view();
#ifdef VISUAL_DEBUG
        sJrbBayVisible |= hitboxView;
#endif
    }
    if (callContext != GEO_CONTEXT_RENDER) {
        for (s32 i = 0; i < 3; i++) {
            sJrbOccluders[i].active = FALSE;
        }
    }
    ((struct GraphNodeSwitchCase *) node)->selectedCase = sJrbBayVisible;
    return NULL;
}

static s32 sJrbCaveVisible = TRUE;

static s32 geo_jrb_near_cave(Vec3f position) {
    // The tunnel mouth is x=4638..5662, z=675, below y=-1942.
    // Include its approach and the entire dry cave, with ample camera margin.
    return position[1] < 0.0f && position[2] < 1600.0f
           && (position[0] > 3500.0f || position[2] < -900.0f);
}

Gfx *geo_switch_jrb_cave(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    struct GraphNodeSwitchCase *selection = (struct GraphNodeSwitchCase *) node;
    sJrbCaveVisible = TRUE;
    if (callContext == GEO_CONTEXT_RENDER && gMarioState != NULL && gCurGraphNodeCamera != NULL) {
        sJrbCaveVisible =
            geo_jrb_near_cave(gMarioState->pos) || geo_jrb_near_cave(gCurGraphNodeCamera->pos);
#ifdef VISUAL_DEBUG
        sJrbCaveVisible |= hitboxView;
#endif
    }
    selection->selectedCase = sJrbCaveVisible;
    return NULL;
}

#define JRB_SHIP_LOD_MEDIUM_DEPTH 7500.0f
#define JRB_SHIP_LOD_FAR_DEPTH 10000.0f
#define JRB_SHIP_LOD_VERY_FAR_DEPTH 20000.0f

Gfx *geo_switch_jrb_ship_lod(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    struct GraphNodeSwitchCase *selection = (struct GraphNodeSwitchCase *) node;
    struct GraphNodeObject *object = gCurGraphNodeObject;
    selection->selectedCase = 0;
    if (callContext != GEO_CONTEXT_RENDER || gCurrLevelNum != LEVEL_JRB || object == NULL
        || object->areaIndex != 1 || object->animInfo.curAnim != NULL || object->node.children != NULL
        || gCurGraphNodeHeldObject != NULL
        || (object->node.flags & (GRAPH_RENDER_BILLBOARD | GRAPH_RENDER_SILHOUETTE))
        || gCurGraphNodeCamera == NULL || gCurGraphNodeCamFrustum == NULL || gCurGraphNodeRoot == NULL
        || gCurGraphNodeRoot->height != SCREEN_HEIGHT / 2 || gCurGraphNodeCamera->rollScreen != 0) {
        return NULL;
    }
#ifdef VISUAL_DEBUG
    if (hitboxView) {
        return NULL;
    }
#endif
    // All parts share their native origin and rocking pose, so they switch together.
    // A zoomed-in camera retains the detailed mesh even at a long distance.
    f32 fov = gCurGraphNodeCamFrustum->fov;
    if (!(fov >= 45.0f && fov < 120.0f)) {
        return NULL;
    }
    f32 scale = 1.0f;
    for (s32 axis = 0; axis < 3; axis++) {
        f32 magnitude = absf(object->scale[axis]);
        if (!(magnitude <= 64.0f && absf(object->cameraToObject[axis]) <= 1.0e6f)) {
            return NULL;
        }
        scale = MAX(scale, magnitude);
    }
    f32 depth = -object->cameraToObject[2];
    if (depth >= JRB_SHIP_LOD_VERY_FAR_DEPTH * scale) {
        selection->selectedCase = 3;
    } else if (depth >= JRB_SHIP_LOD_FAR_DEPTH * scale) {
        selection->selectedCase = 2;
    } else if (depth >= JRB_SHIP_LOD_MEDIUM_DEPTH * scale) {
        selection->selectedCase = 1;
    }
    return NULL;
}

#define SEAWEED_LOD_MAX_PIXELS 32.0f
#define SEAWEED_LOD_MIN_DEPTH 6000.0f

Gfx *geo_switch_seaweed_lod(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    struct GraphNodeSwitchCase *selection = (struct GraphNodeSwitchCase *) node;
    struct GraphNodeObject *object = gCurGraphNodeObject;
    selection->selectedCase = 0;
    if (callContext != GEO_CONTEXT_RENDER || object == NULL || gCurGraphNodeHeldObject != NULL
        || object->node.children != NULL
        || (object->node.flags & (GRAPH_RENDER_BILLBOARD | GRAPH_RENDER_SILHOUETTE))
        || gCurGraphNodeCamera == NULL || gCurGraphNodeCamFrustum == NULL || gCurGraphNodeRoot == NULL
        || gCurGraphNodeRoot->height != SCREEN_HEIGHT / 2 || gCurGraphNodeCamera->rollScreen != 0) {
        return NULL;
    }
#ifdef VISUAL_DEBUG
    if (hitboxView) {
        return NULL;
    }
#endif
    const struct Animation *const *animations = segmented_to_virtual(seaweed_seg6_anims_0600A4D4);
    if (object->animInfo.curAnim != segmented_to_virtual(animations[0])) {
        return NULL;
    }
    f32 scale = 1.0f;
    for (s32 axis = 0; axis < 3; axis++) {
        f32 magnitude = absf(object->scale[axis]);
        if (!(magnitude <= 1.0e6f)) {
            return NULL;
        }
        scale = MAX(scale, magnitude);
    }
    f32 radius = 300.0f * scale;
    f32 depth = -object->cameraToObject[2];
    f32 vertical = gCurGraphNodeCamFrustum->halfFovVertical;
    if (depth > SEAWEED_LOD_MIN_DEPTH && depth > radius && depth <= 1.0e30f && vertical > 0.0f
        && vertical <= 1.0e6f
        && radius * SCREEN_HEIGHT <= SEAWEED_LOD_MAX_PIXELS * vertical * (depth - radius)) {
        selection->selectedCase = 1;
    }
    return NULL;
}

s32 obj_is_in_view(struct GraphNodeObject *node) {
    struct GraphNode *geo = node->sharedChild;

    s16 cullingRadius;

    if (geo != NULL && geo->type == GRAPH_NODE_TYPE_CULLING_RADIUS) {
        cullingRadius = ((struct GraphNodeCullingRadius *) geo)->cullingRadius;
    } else {
        cullingRadius = DEFAULT_CULLING_RADIUS;
    }

    // Check whether the object is not too far away or too close / behind the camera.
    // This makes the HOLP not update when the camera is far away, and it
    // makes PU travel safe when the camera is locked on the main map.
    // If Mario were rendered with a depth over 65536 it would cause overflow
    // when converting the transformation matrix to a fixed point matrix.
    f32 cameraToObjectDepth = node->cameraToObject[2];

    #define VALID_DEPTH_MIDDLE (-20100.f / 2.f)
    #define VALID_DEPTH_RANGE (19900 / 2.f)
    if (absf(cameraToObjectDepth - VALID_DEPTH_MIDDLE) >= VALID_DEPTH_RANGE + cullingRadius) {
        return FALSE;
    }

    if (gCurrLevelNum == LEVEL_JRB && node->areaIndex == 1) {
        if (!sJrbBayVisible && (node->pos[1] >= -900.0f || node->pos[2] >= -900.0f)) {
            return FALSE;
        }
        if (geo_jrb_object_behind_terrain(node)) {
            return FALSE;
        }
    }

    // Cave actors use the same gate as its terrain. Ordinary offscreen cleanup
    // preserves animation clocks and camera-relative positions used by audio.
    if (gCurrLevelNum == LEVEL_JRB && node->areaIndex == 1 && !sJrbCaveVisible && node->pos[1] < -900.0f
        && node->pos[2] < -900.0f) {
        return FALSE;
    }

#ifndef CULLING_ON_EMULATOR
    // Keep the widescreen compatibility bypass; 4:3 uses ordinary object culling.
    if ((gEmulator & NO_CULLING_EMULATOR_WHITELIST) && sAspectRatio > 4.0f / 3.0f) {
        return TRUE;
    }
#endif

    if (gCurrLevelNum == LEVEL_PHYSICS_LAB
#ifdef VISUAL_DEBUG
        && !hitboxView
#endif
    ) {
        s32 visible = physics_lab_object_in_view(node);
        if (visible >= 0) {
            return visible;
        }
    }

    f32 hScreenEdge = -cameraToObjectDepth * gCurGraphNodeCamFrustum->halfFovHorizontal;

    if (absf(node->cameraToObject[0]) > hScreenEdge + cullingRadius) {
        return FALSE;
    }
    return TRUE;
}

#ifdef VISUAL_DEBUG
void visualise_object_hitbox(struct Object *node) {
    Vec3f bnds1, bnds2;
    // This will create a cylinder that visualises their hitbox.
    // If they do not have a hitbox, it will be a small white cube instead.
    if (node->oIntangibleTimer != -1) {
        vec3f_set(bnds1, node->oPosX, (node->oPosY - node->hitboxDownOffset), node->oPosZ);
        vec3f_set(bnds2, node->hitboxRadius, node->hitboxHeight-node->hitboxDownOffset, node->hitboxRadius);
        if (node->behavior == segmented_to_virtual(bhvWarp)
            || node->behavior == segmented_to_virtual(bhvDoorWarp)
            || node->behavior == segmented_to_virtual(bhvFadingWarp)) {
            debug_box_color(COLOR_RGBA32_DEBUG_WARP);
        } else {
            debug_box_color(COLOR_RGBA32_DEBUG_HITBOX);
        }

        debug_box(bnds1, bnds2, (DEBUG_SHAPE_CYLINDER));
        vec3f_set(bnds1, node->oPosX, (node->oPosY - node->hitboxDownOffset), node->oPosZ);
        vec3f_set(bnds2, node->hurtboxRadius, node->hurtboxHeight, node->hurtboxRadius);
        debug_box_color(COLOR_RGBA32_DEBUG_HURTBOX);
        debug_box(bnds1, bnds2, (DEBUG_SHAPE_CYLINDER));
    } else {
        vec3f_set(bnds1, node->oPosX, (node->oPosY - 15), node->oPosZ);
        vec3f_set(bnds2, 30, 30, 30);
        debug_box_color(COLOR_RGBA32_DEBUG_POSITION);
        debug_box(bnds1, bnds2, (DEBUG_SHAPE_BOX));
    }
}
#endif

/**
 * Process an object node.
 */
static void geo_advance_offscreen_animation(struct AnimInfo *node, s32 hasAnimation) {
    if (hasAnimation) {
        node->animFrame = geo_update_animation_frame(node, &node->animFrameAccelAssist);
    }
    node->animTimer = gAreaUpdateCounter;
}

static s32 geo_try_draw_bowser_flame(struct Object *node) {
    if (gCurrLevelNum != LEVEL_BOWSER_3 || gCurGraphNodeHeldObject != NULL
        || gCurGraphNodeCamera == NULL || gCurGraphNodeMasterList == NULL
        || node->header.gfx.sharedChild == NULL || node->header.gfx.throwMatrix != NULL
        || node->header.gfx.node.children != NULL || node->header.gfx.animInfo.curAnim != NULL
        || !(node->header.gfx.node.flags & GRAPH_RENDER_BILLBOARD)
        || (node->header.gfx.node.flags & GRAPH_RENDER_INVISIBLE)
        || (node->activeFlags & ACTIVE_FLAG_BLINKING)) {
        return FALSE;
    }
#ifdef VISUAL_DEBUG
    if (hitboxView) {
        return FALSE;
    }
#endif
    if (node->header.gfx.sharedChild != gLoadedGraphNodes[MODEL_RED_FLAME]
        && node->header.gfx.sharedChild != gLoadedGraphNodes[MODEL_BLUE_FLAME]) {
        return FALSE;
    }
    Vec3f position;
    vec3f_sum(position, node->header.gfx.pos, gMatStack[gMatStackIndex][3]);
    linear_mtxf_mul_vec3f_and_translate(gCameraTransform, node->header.gfx.cameraToObject, position);
    if (!obj_is_in_view(&node->header.gfx)) {
        gCurrAnimType = ANIM_TYPE_NONE;
        return TRUE;
    }
    struct GraphNodeSwitchCase *selection =
        (struct GraphNodeSwitchCase *) node->header.gfx.sharedChild->children;
    gCurGraphNodeObject = &node->header.gfx;
    geo_switch_anim_state(GEO_CONTEXT_RENDER, &selection->fnNode.node, NULL);
    if (selection->selectedCase < 0) {
        gCurGraphNodeObject = NULL;
        return FALSE;
    }
    struct GraphNode *child = selection->fnNode.node.children;
    for (s32 i = 0; i < selection->selectedCase; i++) {
        child = child->next;
    }
    // Camera projection already applies the view transform. Keep the original
    // world-space billboard and skip only the simple geometry-node traversal.
    Mat4 matrix;
    mtxf_billboard(matrix, gMatStack[gMatStackIndex], node->header.gfx.pos, node->header.gfx.scale,
                   gCurGraphNodeCamera->roll);
    Mtx *fixed = alloc_display_list(sizeof(*fixed));
    if (fixed == NULL) {
        gCurGraphNodeObject = NULL;
        return FALSE;
    }
    mtxf_to_mtx(fixed, matrix);
    Mtx *previous = gMatStackFixed[gMatStackIndex];
    gMatStackFixed[gMatStackIndex] = fixed;
    struct GraphNodeDisplayList *display = (struct GraphNodeDisplayList *) child;
    geo_append_display_list(display->displayList, GET_GRAPH_NODE_LAYER(display->node.flags));
    gMatStackFixed[gMatStackIndex] = previous;
    gCurGraphNodeObject = NULL;
    gCurrAnimType = ANIM_TYPE_NONE;
    return TRUE;
}

void geo_process_object(struct Object *node) {
    if (node->header.gfx.areaIndex == gCurGraphNodeRoot->areaIndex && geo_try_draw_bowser_flame(node)) {
        return;
    }
    if (node->header.gfx.areaIndex == gCurGraphNodeRoot->areaIndex) {
        s32 isInvisible = (node->header.gfx.node.flags & GRAPH_RENDER_INVISIBLE)
                          && !(node->activeFlags & ACTIVE_FLAG_BLINKING);
        s32 noThrowMatrix = (node->header.gfx.throwMatrix == NULL);
        s32 cameraPositionReady = FALSE;
        // Maintain throw matrix pointer if the game is paused as it won't be updated.
        Mat4 *oldThrowMatrix = (sCurrPlayMode == PLAY_MODE_PAUSED) ? node->header.gfx.throwMatrix : NULL;
#ifdef VERTICAL_CULLING
        if (geo_particle_outside_vertical_view(&node->header.gfx, &cameraPositionReady)) {
#ifdef TREE_VERTICAL_CULLING
            if (node->header.gfx.sharedChild->type == GRAPH_NODE_TYPE_CULLING_RADIUS && !isInvisible
                && obj_is_in_view(&node->header.gfx)) {
                // Preserve native callback state only when ordinary traversal
                // would have reached this tree. Their matrix argument is unused.
                struct GraphNodeGenerated *transparency =
                    (struct GraphNodeGenerated *) node->header.gfx.sharedChild->children;
                struct GraphNodeObject *oldObject = gCurGraphNodeObject;
                gCurGraphNodeObject = &node->header.gfx;
                geo_update_layer_transparency(GEO_CONTEXT_RENDER, &transparency->fnNode.node, NULL);
                geo_switch_anim_state(GEO_CONTEXT_RENDER, transparency->fnNode.node.next, NULL);
                gCurGraphNodeObject = oldObject;
            }
#endif
            gCurrAnimType = ANIM_TYPE_NONE;
            node->header.gfx.throwMatrix = oldThrowMatrix;
            return;
        }
#endif
        // Invisible billboards use translation without the parent's offset.
        if (isInvisible) {
            cameraPositionReady = FALSE;
        }
        s32 decoration = noThrowMatrix && geo_is_aquarium_decoration(node);
        s32 simpleSprite = noThrowMatrix && geo_is_simple_small_sprite(node);
        s32 earlyObject =
            noThrowMatrix && !(node->header.gfx.node.flags & GRAPH_RENDER_BILLBOARD) && !decoration;
        /* Rotation and scale do not change this object's origin. Perform the
         * same visibility test before building a matrix for an offscreen object. */
        if (earlyObject) {
            linear_mtxf_mul_vec3f_and_translate(gCameraTransform, node->header.gfx.cameraToObject,
                                                node->header.gfx.pos);
            cameraPositionReady = TRUE;
            if (isInvisible || !obj_is_in_view(&node->header.gfx)
                || geo_jrb_object_outside_vertical_view(node)) {
                if (node->header.gfx.animInfo.curAnim != NULL) {
                    if (gCurrLevelNum == LEVEL_JRB && node != gMarioObject
                        && &node->header.gfx != &gMirrorMario) {
                        geo_advance_offscreen_animation(
                            &node->header.gfx.animInfo,
                            (node->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION) != 0);
                    } else {
                        geo_set_animation_globals(
                            &node->header.gfx.animInfo,
                            (node->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION) != 0);
                    }
                }
                gCurrAnimType = ANIM_TYPE_NONE;
                node->header.gfx.throwMatrix = oldThrowMatrix;
                return;
            }
        }
        if (earlyObject && node->header.gfx.animInfo.curAnim != NULL) {
            geo_set_animation_globals(&node->header.gfx.animInfo,
                                      (node->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION) != 0);
        }
        if (decoration || simpleSprite) {
            Vec3f position;
            vec3f_copy(position, node->header.gfx.pos);
            if (!isInvisible && (node->header.gfx.node.flags & GRAPH_RENDER_BILLBOARD)) {
                vec3f_add(position, gMatStack[gMatStackIndex][3]);
            }
            if (!cameraPositionReady) {
                linear_mtxf_mul_vec3f_and_translate(gCameraTransform, node->header.gfx.cameraToObject,
                                                    position);
                cameraPositionReady = TRUE;
            }
            if (isInvisible
                || !(decoration ? geo_aquarium_decoration_in_view(&node->header.gfx)
                                : obj_is_in_view(&node->header.gfx))) {
                // Animation clocks and sound positions still advance while
                // offscreen, just as they do through the ordinary renderer.
                if (node->header.gfx.animInfo.curAnim != NULL) {
                    geo_set_animation_globals(&node->header.gfx.animInfo,
                                              (node->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION)
                                                  != 0);
                }
                gCurrAnimType = ANIM_TYPE_NONE;
                node->header.gfx.throwMatrix = oldThrowMatrix;
                return;
            }
            if (ORDINARY_SPRITE_BATCHING && geo_try_batch_bubble(node, position)) {
                gCurrAnimType = ANIM_TYPE_NONE;
                node->header.gfx.throwMatrix = oldThrowMatrix;
                return;
            }
        }

        // If the throw matrix is null and the object is invisible, there is no need
        // to update billboarding, scale, rotation, etc.
        // This still updates translation since it is needed for sound.
        if (isInvisible && noThrowMatrix) {
            mtxf_translate(gMatStack[gMatStackIndex + 1], node->header.gfx.pos);
        }
        else{
            if (!noThrowMatrix) {
                mtxf_scale_vec3f(gMatStack[gMatStackIndex + 1], *node->header.gfx.throwMatrix, node->header.gfx.scale);
            } else if (node->header.gfx.node.flags & GRAPH_RENDER_BILLBOARD) {
                mtxf_billboard(gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex],
                            node->header.gfx.pos, node->header.gfx.scale, gCurGraphNodeCamera->roll);
            } else {
                mtxf_rotate_zxy_and_translate(gMatStack[gMatStackIndex + 1], node->header.gfx.pos, node->header.gfx.angle);
                mtxf_scale_vec3f(gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex + 1], node->header.gfx.scale);
            }
        }

        node->header.gfx.throwMatrix = &gMatStack[++gMatStackIndex];
        if (!cameraPositionReady) {
            linear_mtxf_mul_vec3f_and_translate(gCameraTransform, node->header.gfx.cameraToObject,
                                                (*node->header.gfx.throwMatrix)[3]);
        }

        // FIXME: correct types
        if (!earlyObject && node->header.gfx.animInfo.curAnim != NULL) {
            geo_set_animation_globals(&node->header.gfx.animInfo, (node->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION) != 0);
        }

        if (!isInvisible && (earlyObject || decoration || obj_is_in_view(&node->header.gfx))) {
            gMatStackIndex--;
            inc_mat_stack();

            if (node->header.gfx.sharedChild != NULL) {
#ifdef VISUAL_DEBUG
                if (hitboxView) visualise_object_hitbox(node);
#endif
                if (node == gMarioObject || &node->header.gfx == &gMirrorMario) {
                    // A supplied throw matrix already owns terrain alignment.
                    sMarioFloorLift = noThrowMatrix ? geo_mario_floor_lift(&node->header.gfx) : 0.0f;
                }
                gCurGraphNodeObject = (struct GraphNodeObject *) node;
                node->header.gfx.sharedChild->parent = &node->header.gfx.node;
                geo_process_node_and_siblings(node->header.gfx.sharedChild);
                node->header.gfx.sharedChild->parent = NULL;
                gCurGraphNodeObject = NULL;
            }
            if (node->header.gfx.node.children != NULL) {
                geo_process_node_and_siblings(node->header.gfx.node.children);
            }
            if (node == gMarioObject || &node->header.gfx == &gMirrorMario) {
                // Vanish/teleport dithering belongs to Mario, not the next translucent actor.
                Gfx *gfx = alloc_display_list(4 * sizeof(*gfx));
                gDPPipeSync(&gfx[0]);
                gDPSetAlphaCompare(&gfx[1], G_AC_NONE);
                gDPSetPrimColor(&gfx[2], 0, 0, 255, 255, 255, 255);
                gSPEndDisplayList(&gfx[3]);
                geo_append_display_list(gfx, LAYER_TRANSPARENT);
            }
        }

        gMatStackIndex--;
        gCurrAnimType = ANIM_TYPE_NONE;
        node->header.gfx.throwMatrix = oldThrowMatrix;
    }
}

/**
 * Process an object parent node. Temporarily assigns itself as the parent of
 * the subtree rooted at 'sharedChild' and processes the subtree, after which the
 * actual children are be processed. (in practice they are null though)
 */
void geo_process_object_parent(struct GraphNodeObjectParent *node) {
    if (node->sharedChild != NULL) {
        node->sharedChild->parent = (struct GraphNode *) node;
        geo_process_node_and_siblings(node->sharedChild);
        node->sharedChild->parent = NULL;
    }
    if (node->node.children != NULL) {
        geo_process_node_and_siblings(node->node.children);
    }
    geo_flush_bubble_batch();
}

/**
 * Process a held object node.
 */
void geo_process_held_object(struct GraphNodeHeldObject *node) {
    Mat4 mat;
    Vec3f translation;
    Mat4 tempMtx;

    if (sMarioCapBlinkOverlay) {
        return;
    }

    if (node->fnNode.func != NULL) {
        node->fnNode.func(GEO_CONTEXT_RENDER, &node->fnNode.node, gMatStack[gMatStackIndex]);
    }
    if (node->objNode != NULL && node->objNode->header.gfx.sharedChild != NULL) {
        vec3_scale_dest(translation, node->translation, 0.25f);

        mtxf_translate(mat, translation);
        mtxf_copy(gMatStack[gMatStackIndex + 1], *gCurGraphNodeObject->throwMatrix);
        vec3f_copy(gMatStack[gMatStackIndex + 1][3], gMatStack[gMatStackIndex][3]);
        mtxf_copy(tempMtx, gMatStack[gMatStackIndex + 1]);
        mtxf_mul(gMatStack[gMatStackIndex + 1], mat, tempMtx);
        mtxf_scale_vec3f(gMatStack[gMatStackIndex + 1], gMatStack[gMatStackIndex + 1], node->objNode->header.gfx.scale);

        if (node->fnNode.func != NULL) {
            node->fnNode.func(GEO_CONTEXT_HELD_OBJ, &node->fnNode.node, (struct AllocOnlyPool *) gMatStack[gMatStackIndex + 1]);
        }

        inc_mat_stack();
        gGeoTempState.type = gCurrAnimType;
        gGeoTempState.enabled = gCurrAnimEnabled;
        gGeoTempState.frame = gCurrAnimFrame;
        gGeoTempState.translationMultiplier = gCurrAnimTranslationMultiplier;
        gGeoTempState.attribute = gCurrAnimAttribute;
        gGeoTempState.data = gCurrAnimData;
        gCurrAnimType = ANIM_TYPE_NONE;
        gCurGraphNodeHeldObject = (void *) node;
        if (node->objNode->header.gfx.animInfo.curAnim != NULL) {
            geo_set_animation_globals(&node->objNode->header.gfx.animInfo, (node->objNode->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION) != 0);
        }

        geo_process_node_and_siblings(node->objNode->header.gfx.sharedChild);
        gCurGraphNodeHeldObject = NULL;
        gCurrAnimType = gGeoTempState.type;
        gCurrAnimEnabled = gGeoTempState.enabled;
        gCurrAnimFrame = gGeoTempState.frame;
        gCurrAnimTranslationMultiplier = gGeoTempState.translationMultiplier;
        gCurrAnimAttribute = gGeoTempState.attribute;
        gCurrAnimData = gGeoTempState.data;
        gMatStackIndex--;
    }

    if (node->fnNode.node.children != NULL) {
        geo_process_node_and_siblings(node->fnNode.node.children);
    }
}

/**
 * Processes the children of the given GraphNode if it has any
 */
void geo_try_process_children(struct GraphNode *node) {
    if (node->children != NULL) {
        geo_process_node_and_siblings(node->children);
    }
}

typedef void (*GeoProcessFunc)(struct GraphNode *);

// See enum 'GraphNodeTypes' in 'graph_node.h'.
static GeoProcessFunc GeoProcessJumpTable[] = {
    [GRAPH_NODE_TYPE_ORTHO_PROJECTION    ] = (GeoProcessFunc) geo_process_ortho_projection,
    [GRAPH_NODE_TYPE_PERSPECTIVE         ] = (GeoProcessFunc) geo_process_perspective,
    [GRAPH_NODE_TYPE_MASTER_LIST         ] = (GeoProcessFunc) geo_process_master_list,
    [GRAPH_NODE_TYPE_LEVEL_OF_DETAIL     ] = (GeoProcessFunc) geo_process_level_of_detail,
    [GRAPH_NODE_TYPE_SWITCH_CASE         ] = (GeoProcessFunc) geo_process_switch,
    [GRAPH_NODE_TYPE_CAMERA              ] = (GeoProcessFunc) geo_process_camera,
    [GRAPH_NODE_TYPE_TRANSLATION_ROTATION] = (GeoProcessFunc) geo_process_translation_rotation,
    [GRAPH_NODE_TYPE_TRANSLATION         ] = (GeoProcessFunc) geo_process_translation,
    [GRAPH_NODE_TYPE_ROTATION            ] = (GeoProcessFunc) geo_process_rotation,
    [GRAPH_NODE_TYPE_OBJECT              ] = (GeoProcessFunc) geo_process_object,
    [GRAPH_NODE_TYPE_ANIMATED_PART       ] = (GeoProcessFunc) geo_process_animated_part,
    [GRAPH_NODE_TYPE_BILLBOARD           ] = (GeoProcessFunc) geo_process_billboard,
    [GRAPH_NODE_TYPE_DISPLAY_LIST        ] = (GeoProcessFunc) geo_process_display_list,
    [GRAPH_NODE_TYPE_SCALE               ] = (GeoProcessFunc) geo_process_scale,
    [GRAPH_NODE_TYPE_SHADOW              ] = (GeoProcessFunc) geo_process_shadow,
    [GRAPH_NODE_TYPE_OBJECT_PARENT       ] = (GeoProcessFunc) geo_process_object_parent,
    [GRAPH_NODE_TYPE_GENERATED_LIST      ] = (GeoProcessFunc) geo_process_generated_list,
    [GRAPH_NODE_TYPE_BACKGROUND          ] = (GeoProcessFunc) geo_process_background,
    [GRAPH_NODE_TYPE_HELD_OBJ            ] = (GeoProcessFunc) geo_process_held_object,
    [GRAPH_NODE_TYPE_CULLING_RADIUS      ] = (GeoProcessFunc) geo_try_process_children,
    [GRAPH_NODE_TYPE_ROOT                ] = (GeoProcessFunc) geo_try_process_children,
    [GRAPH_NODE_TYPE_START               ] = (GeoProcessFunc) geo_try_process_children,
};

/**
 * Process a generic geo node and its siblings.
 * The first argument is the start node, and all its siblings will
 * be iterated over.
 */
void geo_process_node_and_siblings(struct GraphNode *firstNode) {
    s32 iterateChildren = TRUE;
    struct GraphNode *curGraphNode = firstNode;
    struct GraphNode *parent = curGraphNode->parent;

    // In the case of a switch node, exactly one of the children of the node is
    // processed instead of all children like usual
    if (parent != NULL) {
        iterateChildren = (parent->type != GRAPH_NODE_TYPE_SWITCH_CASE);
    }

    do {
        if (curGraphNode->flags & GRAPH_RENDER_ACTIVE) {
            if (curGraphNode->flags & GRAPH_RENDER_CHILDREN_FIRST) {
                geo_try_process_children(curGraphNode);
            } else {
                GeoProcessJumpTable[curGraphNode->type](curGraphNode);
            }
        } else {
            if (curGraphNode->type == GRAPH_NODE_TYPE_OBJECT) {
                ((struct GraphNodeObject *) curGraphNode)->throwMatrix = NULL;
            }
        }
    } while (iterateChildren && (curGraphNode = curGraphNode->next) != firstNode);
}

/**
 * Process a root node. This is the entry point for processing the scene graph.
 * The root node itself sets up the viewport, then all its children are processed
 * to set up the projection and draw display lists.
 */
void geo_process_root(struct GraphNodeRoot *node, Vp *b, Vp *c, s32 clearColor) {
    if (node->node.flags & GRAPH_RENDER_ACTIVE) {
        Mtx *initialMatrix;
        Vp *viewport = alloc_display_list(sizeof(*viewport));

        gDisplayListHeap = alloc_only_pool_init(main_pool_available() - sizeof(struct AllocOnlyPool), MEMORY_POOL_LEFT);
        initialMatrix = alloc_display_list(sizeof(*initialMatrix));
        gCurLookAt = (LookAt*)alloc_display_list(sizeof(LookAt));
        bzero(gCurLookAt, sizeof(LookAt));

        gMatStackIndex = 0;
        gCurrAnimType = ANIM_TYPE_NONE;
        vec3s_set(viewport->vp.vtrans, node->x * 4, node->y * 4, 511);
        vec3s_set(viewport->vp.vscale, node->width * 4, node->height * 4, 511);

        if (b != NULL) {
            clear_framebuffer(clearColor);
            make_viewport_clip_rect(b);
            *viewport = *b;
        }

        else if (c != NULL) {
            clear_framebuffer(clearColor);
            make_viewport_clip_rect(c);
        }

        mtxf_identity(gMatStack[gMatStackIndex]);
        mtxf_to_mtx(initialMatrix, gMatStack[gMatStackIndex]);
        gMatStackFixed[gMatStackIndex] = initialMatrix;
        gSPViewport(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(viewport));
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(gMatStackFixed[gMatStackIndex]),
                  G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gCurGraphNodeRoot = node;
        if (node->node.children != NULL) {
            geo_process_node_and_siblings(node->node.children);
        }
        gCurGraphNodeRoot = NULL;
#ifdef VANILLA_DEBUG
        if (gShowDebugText) {
            print_text_fmt_int(180, 36, "MEM %d", gDisplayListHeap->totalSpace - gDisplayListHeap->usedSpace);
        }
#endif
        main_pool_free(gDisplayListHeap);
    }
}
