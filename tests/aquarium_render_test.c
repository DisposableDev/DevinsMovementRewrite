/* Production object rendering, batching, culling and matrix math. Display-list
 * commands are observed doubles; this does not run an RSP, rasterize, or time
 * the N64. Geometry/animation traversal outside the selected objects is supplied. */
#include <stdint.h>
#include <math.h>
#include "test_support.h"
typedef int8_t s8;
typedef uint8_t u8;
typedef u8 Alpha;
typedef int16_t TerrainData;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
typedef f32 Vec3f[3];
typedef s16 Vec3s[3];
typedef f32 Mat4[4][4];
typedef union {
    s16 values[32];
    s32 m[4][4];
    uint64_t align;
} Mtx;
typedef union {
    struct {
        s16 ob[3];
        u16 flag;
        s16 tc[2];
        u8 cn[4];
    } v;
    uint64_t align;
} Vtx;
typedef struct {
    int op, n, indices[4];
    const void *ptr;
} Gfx;
struct GraphNode;
typedef Gfx *(*GraphNodeFunc)(s32, struct GraphNode *, void *);
#define UNUSED __attribute__((unused))
#define TRUE 1
#define FALSE 0
#define SILHOUETTE 1
#define IA8_30FPS_COINS
#ifndef RENDER_TEST_SHADOW_LOD_DEFAULT
#define SHADOW_TEXTURE_REUSE
#endif
#define TREE_VERTICAL_CULLING
#define SHADOW_VIEW_CULLING
#define ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
#define AUTO_LOD
#define F3DEX_GBI_2
#define VISUAL_DEBUG
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define BITMASK(n) ((1U << (n)) - 1)
#define BIT(n) (1U << (n))
#define COND_BIT(cond, flags, bit) ((cond) ? ((flags) |= (bit)) : ((flags) &= ~(bit)))
#define MODEL_BREAKABLE_BOX 777
#define GET_HIGH_S16_OF_32(v) ((s16) ((u32) (v) >> 16))
#define SET_HIGH_S16_OF_32(v, h) ((v) = (s32) (((u32) (v) & 0xffff) | ((u32) (u16) (h) << 16)))
#define STATIC_ASSERT(c, m) typedef char bounds_mode_is_valid[(c) ? 1 : -1]
#define ALWAYS_INLINE static inline
#define OPTIMIZE_OS
#define PUPPYPRINT_ADD_COUNTER(x) (++(x))
#define PLAY_MODE_PAUSED 2
#define LAYER_FIRST LAYER_FORCE
#define LEVEL_PHYSICS_LAB 32
#define LEVEL_BITFS 19
#define LEVEL_LLL 22
#define LEVEL_JRB 12
#define LEVEL_BOWSER_3 34
#define INTERACT_HOOT (1u << 0)
#define INTERACT_GRABBABLE (1u << 1)
#define INTERACT_DAMAGE (1u << 3)
#define INTERACT_COIN (1u << 4)
#define ACTIVE_FLAG_DEACTIVATED 0
#define ACTIVE_FLAG_ACTIVE 1
#define ACTIVE_FLAG_DITHERED_ALPHA (1U << 7)
#define SPAWN_STAR_ARC_CUTSCENE_ACT_END 3
#define SPAWN_STAR_POS_CUTSCENE_ACT_SLOW_STAR_ROTATION 3
#define INTERACT_KOOPA (1u << 7)
#define INTERACT_SPINY_WALKING (1u << 8)
#define INTERACT_BOUNCE_TOP (1u << 15)
#define INTERACT_BULLY (1u << 17)
#define INTERACT_BOUNCE_TOP2 (1u << 20)
#define INTERACT_MR_BLIZZARD (1u << 21)
#define INTERACT_HIT_FROM_BELOW (1u << 22)
#define INTERACT_TEXT (1u << 23)
#define INTERACT_TORNADO (1u << 24)
#define INTERACT_CLAM_OR_BUBBA (1u << 26)
#define INTERACT_SHOCK (1u << 29)
#define INT_SUBTYPE_SIGN (1u << 12)
#define INT_SUBTYPE_HOLDABLE_NPC (1u << 4)
#define INT_SUBTYPE_GRABS_MARIO (1u << 2)
#define EXCLAMATION_BOX_BP_KOOPA_SHELL 3
#define VIRTUAL_TO_PHYSICAL(p) (p)
static int gCurrLevelNum, gCurrAreaIndex;
static int labObjectVisibility = -1;
static int labVisibilityCalls;
struct GraphNodeObject;
// Production lab bounds are tested separately; observe renderer handoff here.
static s32 physics_lab_object_in_view(struct GraphNodeObject *node) {
    (void) node;
    labVisibilityCalls++;
    return labObjectVisibility;
}
#include "render_definitions.inc"
typedef uintptr_t BehaviorScript;
struct Surface {
    TerrainData type;
    struct Object *object;
    struct {
        f32 x, y, z;
    } normal;
    f32 originOffset;
};
struct Object {
    struct {
        struct GraphNodeObject gfx;
    } header;
    struct Object *parentObj;
    u8 bowserArenaFade;
    const void *behavior;
    int timer;
    u32 oInteractType, oInteractionSubtype;
    s32 oBehParams2ndByte;
    s16 activeFlags, oIntangibleTimer;
    s32 oAction, oAnimState, oOpacity;
    s32 oTimer, oBreakableBoxSmallFramesSinceReleased, oPlatformOnTrackType;
    struct Surface *oFloor;
    f32 oFloorHeight;
    f32 oPosX, oPosY, oPosZ;
};
static struct Object *o;
static s32 sRespawners, sWithinRenderDistance;
static const BehaviorScript bhvBreakableBoxSmall[] = { 777 };
static void create_respawner(s32 model, const BehaviorScript *behavior, s32 distance) {
    sRespawners++;
}
static s32 is_point_within_radius_of_mario(f32 x, f32 y, f32 z, s32 distance) {
    return sWithinRenderDistance;
}
struct MarioState {
    Vec3f pos;
    f32 forwardVel;
    struct Surface *floor;
    f32 floorHeight;
    f32 quicksandDepth;
    u8 squishTimer;
    u32 flags;
    s16 invincTimer;
    u32 action;
    Vec3s faceAngle;
};
#define MARIO_VANISH_CAP (1U << 1)
#define MARIO_TELEPORTING (1U << 7)
#define GEO_CONTEXT_RENDER 1
#define GEO_CONTEXT_AREA_INIT 4
#define GEO_CONTEXT_HELD_OBJ 5
#define G_AC_DITHER 3
#define G_AC_NONE 0
#define G_AC_THRESHOLD 1
static struct MarioBodyState gBodyStates[2];
static struct GeoAnimState gGeoTempState;
static struct MarioState marioState, *gMarioState;
struct AllocOnlyPool {
    int unused;
};
static struct {
    int matrix;
} gPuppyCallCounter;
static int sCurrPlayMode, gMatStackIndex, gCurrAnimType, gEmulator, hitboxView;
static f32 sAspectRatio;
static s16 gCurrAnimFrame;
static u16 gAreaUpdateCounter, *gCurrAnimAttribute;
static u32 gGlobalTimer;
static s16 *gCurrAnimData;
static f32 gCurrAnimTranslationMultiplier;
static int gCurrAnimEnabled;
static Mat4 gMatStack[32], gCameraTransform;
void mtxf_mul(Mat4 dest, Mat4 a, Mat4 b);
static Mtx *gMatStackFixed[32];
static Vec3f gVec3fZero;
static struct GraphNodeRoot root;
static struct GraphNodePerspective frustum;
static struct GraphNodeCamera camera;
static struct GraphNodeMasterList master;
static struct GraphNodeRoot *gCurGraphNodeRoot;
static struct GraphNodePerspective *gCurGraphNodeCamFrustum;
static struct GraphNodeCamera *gCurGraphNodeCamera;
static struct GraphNodeMasterList *gCurGraphNodeMasterList;
static struct GraphNodeObject *gCurGraphNodeObject;
#define gCurGraphNodeObjectNode ((struct Object *) gCurGraphNodeObject)
static struct GraphNodeHeldObject *gCurGraphNodeHeldObject;
static struct GraphNodeObject gMirrorMario;
static struct Object *gMarioObject;
static u8 sCharacterPalette;
static int lodVisits;
static struct GraphNode *sLodChild;
static f32 get_dist_from_camera(Vec3f pos) {
    return -pos[2];
}
static u8 mario_character_palette(void) {
    return sCharacterPalette;
}
static struct Shadow gCurrShadow;
static struct Shadow *s = &gCurrShadow;
static int useNativeShadow, nativeFloorQueries, nativeWaterQueries;
static struct Surface nativeShadowFloor, *nativeFloorResult, *nativeWaterFloor;
static f32 nativeFloorHeight, nativeWaterHeight;
static f32 find_floor(f32 x, f32 y, f32 z, struct Surface **floor) {
    nativeFloorQueries++;
    *floor = nativeFloorResult;
    return nativeFloorHeight;
}
static f32 find_water_level_and_floor(f32 x, f32 y, f32 z, struct Surface **floor) {
    nativeWaterQueries++;
    *floor = nativeWaterFloor;
    return nativeWaterHeight;
}
static Gfx *real_create_shadow_below_xyz(Vec3f pos, s16 scale, u8 solidity, s8 type, s8 shifted);
Gfx *create_mario_wall_shadow(Vec3f pos, s16 scale, u8 solidity);
static struct Surface *wallShadowSurface;
static f32 wallShadowDistance;
static s32 wallShadowQueries, wallShadowFlags;
static Vec3f wallShadowOrigin, wallShadowRay;
static f32 find_surface_on_ray(Vec3f origin, Vec3f ray, struct Surface **wall, Vec3f hit, s32 flags) {
    wallShadowQueries++;
    wallShadowFlags = flags;
    vec3f_copy(wallShadowOrigin, origin);
    vec3f_copy(wallShadowRay, ray);
    *wall = wallShadowSurface;
    if (*wall != NULL) {
        s32 partition = RAYCAST_FIND_WALL;
        if ((*wall)->normal.y > 0.01f) {
            partition = RAYCAST_FIND_FLOOR;
        } else if ((*wall)->normal.y < -0.01f) {
            partition = RAYCAST_FIND_CEIL;
        }
        if (!(flags & partition)) {
            *wall = NULL;
        }
    }
    f32 range = sqrtf(ray[0] * ray[0] + ray[1] * ray[1] + ray[2] * ray[2]);
    for (s32 i = 0; i < 3; i++) {
        hit[i] = origin[i] + ray[i] * wallShadowDistance / range;
    }
    return wallShadowDistance;
}
static s16 atan2s(f32 z, f32 x) {
    return (s16) (atan2f(x, z) * (65536.0f / 6.283185307179586f));
}
static int shadowQueries, shadowMatrices, shadowOpacity, shadowVisible;
static const Gfx door_seg3_dl_polished_wooden_door_near[1];
static const Gfx door_seg3_dl_polished_wooden_door_far[1];
UNUSED static const Gfx dl_shadow_circle_32[1], dl_shadow_square_32[1];
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
static const Gfx dl_shadow_circle[1], dl_shadow_square[1], dl_shadow_begin[1], dl_shadow_end[1];
static Gfx shadowDl[4];
static Vec3f queriedShadowPos;
static Gfx *create_shadow_below_xyz(Vec3f pos, f32 scale, u8 solidity, u8 type, s8 shifted) {
    vec3f_copy(queriedShadowPos, pos);
    shadowQueries++;
    shadowOpacity = solidity;
    if (useNativeShadow) {
        return real_create_shadow_below_xyz(pos, scale, solidity, type, shifted);
    }
    return shadowVisible ? shadowDl : NULL;
}
static void mtxf_shadow(Mat4 dest, Vec3f normal, Vec3f pos, Vec3f scale, s16 yaw) {
    shadowMatrices++;
    memset(dest, 0, sizeof(Mat4));
    for (int i = 0; i < 4; i++) {
        dest[i][i] = 1;
    }
}
static struct AllocOnlyPool heap, *gDisplayListHeap = &heap;
static struct GraphNode *loaded[256], **gLoadedGraphNodes = loaded;
static const int bhvSmallParticleBubbles[1] = { 1 }, bhvBlueFish[1] = { 2 },
                 bhvExclamationBox[1] = { 3 };
static const BehaviorScript bhvPlatformOnTrack[1], bhvDoor[1], bhvDoorWarp[1];
static const int bhvBobomb[1] = { 4 }, bhvChuckya[1] = { 5 }, bhvKingBobomb[1] = { 6 },
                 bhvBowser[1] = { 7 };
static const BehaviorScript bhvRedCoin[1], bhv1Up[1], bhv1upWalking[1], bhv1upRunningAway[1];
static const BehaviorScript bhv1upSliding[1], bhv1upJumpOnApproach[1], bhvHidden1up[1],
    bhvHidden1upInPole[1];
static const BehaviorScript bhvStar[1], bhvStarSpawnCoordinates[1], bhvSpawnedStar[1],
    bhvSpawnedStarNoLevelExit[1];
static const Gfx white_particle_small_dl_begin[2], white_particle_small_dl_end[6];
static const Gfx white_particle_small_dl[3], otherDl[1], fishDl[1];
static const Gfx tree_seg3_dl_bubbly[1], tree_seg3_dl_bubbly_transparent[1];
static const Gfx tree_seg3_dl_spiky[1], tree_seg3_dl_spiky_transparent[1];
static const Gfx tree_seg3_dl_snowy_pine[1], tree_seg3_dl_snowy_pine_transparent[1];
static const Gfx tree_seg3_dl_palm[1], tree_seg3_dl_palm_transparent[1];
static const Gfx white_particle_small_unused_dl[3], lava_droplet_dl[3], cold_water_droplet_dl[3];
static const Gfx white_particle_dl[3], sand_seg3_dl_particle[3], pebble_seg3_dl_0301CB00[3];
static const Gfx mushroom_1up_seg3_dl_0302A660[1];
static const Gfx poison_mushroom_dl[1];
static s32 compactCount;
static Vec3f compactPositions[4], compactScales[4];
static s16 compactAreas[4];
static s32 compact_explosion_bubbles_count(void) {
    return compactCount;
}
static s32 compact_explosion_bubble_view(s32 index, Vec3f pos, Vec3f scale, s16 *areaIndex) {
    if (index < 0 || index >= compactCount) {
        return FALSE;
    }
    for (int i = 0; i < 3; i++) {
        pos[i] = compactPositions[index][i];
        scale[i] = compactScales[index][i];
    }
    *areaIndex = compactAreas[index];
    return TRUE;
}
static Gfx topCommands[1024], *gDisplayListHead;
static void *gCurLookAt;
static int animationCalls, nativeDraws, debugDraws, masterFlushes;
static void *allocations[1024];
static size_t allocationSizes[1024];
static int allocationCount, failNextAllocation;
static f32 absf(f32 x) {
    return fabsf(x);
}
static f32 sins(s16 x) {
    return sinf(x * (6.283185307179586f / 65536.0f));
}
static f32 coss(s16 x) {
    return cosf(x * (6.283185307179586f / 65536.0f));
}
static f32 construct_float(f32 x) {
    return x;
} // Equivalent value, without MIPS constant-load assembly.
static void *segmented_to_virtual(const void *ptr) {
    return (void *) ptr;
}
static void *alloc_display_list(u32 size) {
    if (failNextAllocation) {
        failNextAllocation = FALSE;
        return NULL;
    }
    if (allocationCount == 1024) {
        fputs("HARNESS ERROR: allocation capacity\n", stderr);
        exit(2);
    }
    void *ptr = calloc(1, size + 16);
    if (!ptr) {
        exit(2);
    }
    memset((u8 *) ptr + size, 0xA5, 16);
    allocations[allocationCount] = ptr;
    allocationSizes[allocationCount++] = size;
    return ptr;
}
static void *alloc_only_pool_alloc(struct AllocOnlyPool *pool, u32 size) {
    return alloc_display_list(size);
}
enum {
    CMD_MATERIAL = 1,
    CMD_VERTICES,
    CMD_QUAD,
    CMD_END,
    CMD_LOOKAT,
    CMD_PIPE_SYNC,
    CMD_RENDER_MODE,
    CMD_FOG_COLOR,
    CMD_PRIM_COLOR,
    CMD_COMBINE,
    CMD_MATRIX,
    CMD_GEOMETRY_SET,
    CMD_GEOMETRY_CLEAR,
    CMD_ALPHA_COMPARE,
    CMD_BLEND_COLOR
};
static void display(Gfx *cmd, const void *ptr) {
    *cmd = (Gfx){ .op = CMD_MATERIAL, .ptr = ptr };
}
static void vertices(Gfx *cmd, const void *ptr, int n, int offset) {
    if (offset) {
        exit(2);
    }
    *cmd = (Gfx){ .op = CMD_VERTICES, .ptr = ptr, .n = n };
}
static void quad(Gfx *cmd, int a, int b, int c, int d, int flags) {
    if (flags) {
        exit(2);
    }
    *cmd = (Gfx){ .op = CMD_QUAD, .indices = { a, b, c, d } };
}
static void end(Gfx *cmd) {
    *cmd = (Gfx){ .op = CMD_END };
}
#define gSPDisplayList(p, d) display((p), (d))
#define gSPVertex(p, v, n, o) vertices((p), (v), (n), (o))
#define gSP1Quadrangle(p, a, b, c, d, f) quad((p), (a), (b), (c), (d), (f))
#define gSPEndDisplayList(p) end((p))
#define gSPLookAt(p, l) (*(p) = (Gfx){ .op = CMD_LOOKAT, .ptr = (l) })
#define gDPPipeSync(p) (*(p) = (Gfx){ .op = CMD_PIPE_SYNC })
#define gDPSetRenderMode(p, m1, m2) (*(p) = (Gfx){ .op = CMD_RENDER_MODE, .indices = { (m1), (m2) } })
#define gDPSetFogColor(p, r, g, b, a)                                                                  \
    (*(p) = (Gfx){ .op = CMD_FOG_COLOR, .n = (a), .indices = { (r), (g), (b) } })
#define gDPSetAlphaCompare(p, a) (*(p) = (Gfx){ .op = CMD_ALPHA_COMPARE, .n = (a) })
#define gDPSetBlendColor(p, r, g, b, a)                                                                \
    (*(p) = (Gfx){ .op = CMD_BLEND_COLOR, .n = (a), .indices = { (r), (g), (b) } })
#define ENVIRONMENT 1
#define TEXEL0 2
#define gDPSetCombineLERP(p, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, q)                           \
    (*(p) = (Gfx){ .op = CMD_COMBINE, .indices = { (d), (e), (g), (h) } })
#define gDPSetEnvColor(p, r, g, b, a)                                                                  \
    (*(p) = (Gfx){ .op = CMD_FOG_COLOR, .n = (a), .indices = { (r), (g), (b), (a) } })
#define G_ZBUFFER 1
#define G_MTX_MODELVIEW 0
#define G_MTX_MUL 0
#define G_MTX_LOAD 2
#define G_MTX_NOPUSH 0
#define gSPMatrix(p, m, f) (*(p) = (Gfx){ .op = CMD_MATRIX, .ptr = (m), .n = (f) })
#define gSPSetGeometryMode(p, m) (*(p) = (Gfx){ .op = CMD_GEOMETRY_SET, .n = (m) })
#define gSPClearGeometryMode(p, m) (*(p) = (Gfx){ .op = CMD_GEOMETRY_CLEAR, .n = (m) })
// The master-list test supplies layer modes and observes the native draw order.
// Model geometry and the silhouette material lists are not rasterized here.
static struct RenderModeContainer renderModeTable_1Cycle[2], renderModeTable_2Cycle[2];
#define gDPSetPrimColor(p, m, l, r, g, b, a)                                                           \
    (*(p) = (Gfx){ .op = CMD_PRIM_COLOR, .n = (a), .indices = { (r), (g), (b) } })
#define G_LIGHTING 2
#define G_TEXTURE_GEN 4
#define G_OFF 0
#define G_TX_RENDERTILE 0
#define G_CC_PRIMITIVE 1
#define G_CC_SHADE 2
#define gsDPPipeSync()                                                                                 \
    { .op = CMD_PIPE_SYNC }
#define gsDPSetRenderMode(a, b)                                                                        \
    {                                                                                                  \
        .op = CMD_RENDER_MODE, .indices = {(a), (b) }                                                  \
    }
#define gsSPClearGeometryMode(m)                                                                       \
    { .op = CMD_GEOMETRY_CLEAR, .n = (m) }
#define gsSPSetGeometryMode(m)                                                                         \
    { .op = CMD_GEOMETRY_SET, .n = (m) }
#define gsSPTexture(s, t, l, tile, on)                                                                 \
    { .op = CMD_MATERIAL, .n = (on) }
#define gsDPSetCombineMode(a, b)                                                                       \
    {                                                                                                  \
        .op = CMD_COMBINE, .indices = {(a), (b) }                                                      \
    }
#define gsDPSetFogColor(r, g, b, a)                                                                    \
    { .op = CMD_FOG_COLOR, .n = (a) }
#define gsDPSetEnvColor(r, g, b, a)                                                                    \
    { .op = CMD_FOG_COLOR, .n = (a) }
#define gsDPSetAlphaCompare(a)                                                                         \
    { .op = CMD_ALPHA_COMPARE, .n = (a) }
#define gsDPSetBlendColor(r, g, b, a)                                                                  \
    { .op = CMD_BLEND_COLOR, .n = (a) }
#define gsDPSetPrimColor(m, l, r, g, b, a)                                                             \
    {                                                                                                  \
        .op = CMD_PRIM_COLOR, .n = (a), .indices = {(r), (g), (b) }                                    \
    }
#define gsSPEndDisplayList()                                                                           \
    { .op = CMD_END }
static Mtx identityMatrixWorldScale;
static s32 surfaceView;
static void visual_surface_loop(Gfx **head) {
    (void) head;
}
static void render_debug_boxes(Gfx **head) {
    (void) head;
}
static void geo_process_node_and_siblings(struct GraphNode *node);
void geo_process_shadow(struct GraphNodeShadow *node);
static void real_geo_set_animation_globals(struct AnimInfo *anim, s32 enabled);
static s32 heThiccPoseCalls;
static s32 floorPoseNative;
static s32 mario_he_thicc_render_pose(struct AnimInfo *info, struct MarioRenderPose *pose) {
    heThiccPoseCalls++;
    return FALSE;
}
// Mario-only visual blending is exercised by the wallpose suite.
static s32 mario_transition_pose_render(struct AnimInfo *anim, struct MarioRenderPose *pose) {
    return FALSE;
}
static void geo_set_animation_globals(struct AnimInfo *anim, int enabled) {
    if (anim->curAnim == &blue_fish_seg3_anim_0301C298 || floorPoseNative) {
        animationCalls++;
        real_geo_set_animation_globals(anim, enabled);
        return;
    }
    animationCalls++;
    if (enabled) {
        anim->animFrame++;
    }
    gCurrAnimType = 88;
}
static void visualise_object_hitbox(struct Object *obj) {
    debugDraws++;
}
static void geo_process_master_list_sub(struct GraphNodeMasterList *node) {
    if (sBubbleBatch.count) {
        fputs("HARNESS ERROR: unfinished display list\n", stderr);
        exit(2);
    }
    masterFlushes++;
}
static void geo_render_compact_explosion_bubbles(void);
#include "render_sources.inc"
static struct GraphNodeDisplayList bubbleGeo;
static struct GraphNode fishGeo, group;
static struct GraphNodeShadow objectShadowGeo;
static struct GraphNodeScale fishScaleGeo;
static struct GraphNodeDisplayList floorBodyGeo;
static struct GraphNodeAnimatedPart fishParts[3];
static struct Object objects[240];
static int programCount;
static struct GraphNode capBranches[4];
static int capDraws, heldDraws, capPoseMismatch;
static int capAlphas[2], capCases[2], capStates[2], capHands[2];
static u16 capAttributes[10];
static Gfx *cap_held_callback(s32 context, struct GraphNode *node, void *matrix) {
    heldDraws++;
    return NULL;
}
static void geo_process_node_and_siblings(struct GraphNode *node) {
    lodVisits++;
    if (node == sLodChild) {
        return;
    }
    if (node == &capBranches[0] || node == &capBranches[1] || node == &capBranches[2]
        || node == &capBranches[3]) {
        if (capDraws >= 2 || gCurrAnimType != ANIM_TYPE_TRANSLATION
            || gCurrAnimAttribute != capAttributes) {
            capPoseMismatch = TRUE;
        }
        capCases[capDraws] = node - capBranches;
        capStates[capDraws] = gBodyStates[0].capState;
        capHands[capDraws] = gBodyStates[0].handState;
        capAlphas[capDraws++] = (gBodyStates[0].modelState & MODEL_STATE_ALPHA)
                                    ? gBodyStates[0].modelState & MODEL_STATE_MASK
                                    : 255;
        // The child traversal consumes animation channels; no animation tick here.
        gCurrAnimType = ANIM_TYPE_ROTATION;
        gCurrAnimAttribute += 8;
        struct GraphNodeHeldObject held = { 0 };
        held.fnNode.func = cap_held_callback;
        geo_process_held_object(&held);
        return;
    }
    if (node == &group) {
        for (int i = 0; i < programCount; i++) {
            geo_process_object(&objects[i]);
        }
    } else if (node->type == GRAPH_NODE_TYPE_CULLING_RADIUS && node->children != NULL) {
        struct GraphNodeGenerated *gen = (struct GraphNodeGenerated *) node->children;
        Gfx *list = gen->fnNode.func(GEO_CONTEXT_RENDER, &gen->fnNode.node, NULL);
        geo_append_display_list(list, GET_GRAPH_NODE_LAYER(gen->fnNode.node.flags));
        geo_process_node_and_siblings(gen->fnNode.node.next);
    } else if (node->type == GRAPH_NODE_TYPE_START && node->children != NULL) {
        geo_process_node_and_siblings(node->children);
    } else if (node->type == GRAPH_NODE_TYPE_SWITCH_CASE) {
        struct GraphNodeSwitchCase *sw = (struct GraphNodeSwitchCase *) node;
        sw->fnNode.func(GEO_CONTEXT_RENDER, node, gMatStack[gMatStackIndex]);
        struct GraphNode *selected = node->children;
        for (s32 i = 0; i < sw->selectedCase && selected != NULL; i++) {
            selected = selected->next;
        }
        if (selected != NULL && sw->selectedCase >= 0) {
            if (selected->type != GRAPH_NODE_TYPE_DISPLAY_LIST) {
                fputs("HARNESS ERROR: non-leaf sprite frame\n", stderr);
                exit(2);
            }
            nativeDraws++;
            geo_append_display_list(((struct GraphNodeDisplayList *) selected)->displayList,
                                    GET_GRAPH_NODE_LAYER(selected->flags));
        }
    } else if (node->type == GRAPH_NODE_TYPE_DISPLAY_LIST
               && (((struct GraphNodeDisplayList *) node)->displayList == white_particle_small_dl
                   || ((struct GraphNodeDisplayList *) node)->displayList == cold_water_droplet_dl
                   || ((struct GraphNodeDisplayList *) node)->displayList == lava_droplet_dl
                   || ((struct GraphNodeDisplayList *) node)->displayList
                          == white_particle_small_unused_dl
                   || ((struct GraphNodeDisplayList *) node)->displayList == white_particle_dl
                   || ((struct GraphNodeDisplayList *) node)->displayList == sand_seg3_dl_particle
                   || ((struct GraphNodeDisplayList *) node)->displayList == pebble_seg3_dl_0301CB00)) {
        nativeDraws++;
        geo_append_display_list(((struct GraphNodeDisplayList *) node)->displayList,
                                LAYER_OCCLUDE_SILHOUETTE_ALPHA);
    } else if (node == &fishGeo) {
        nativeDraws++;
        geo_append_display_list((void *) fishDl, LAYER_OCCLUDE_SILHOUETTE_OPAQUE);
    } else if (node == &floorBodyGeo.node) {
        geo_append_display_list(floorBodyGeo.displayList, LAYER_OPAQUE);
    } else if (node == &objectShadowGeo.node) {
        geo_process_shadow(&objectShadowGeo);
    } else if (node == &fishScaleGeo.node) {
        geo_process_scale(&fishScaleGeo);
    } else if (node == &fishParts[0].node || node == &fishParts[1].node || node == &fishParts[2].node) {
        geo_process_animated_part((struct GraphNodeAnimatedPart *) node);
    } else {
        fputs("HARNESS ERROR: unknown geometry traversal\n", stderr);
        exit(2);
    }
}
static void begin(int n) {
    floorPoseNative = FALSE;
    sMarioFloorLift = 0.0f;
    marioState.quicksandDepth = 0.0f;
    marioState.squishTimer = 0;
    marioState.action = 0;
    marioState.forwardVel = 0;
    marioState.floor = NULL;
    marioState.floorHeight = 0;
    wallShadowSurface = NULL;
    wallShadowQueries = 0;
    marioState.invincTimer = 0;
    gGlobalTimer = 0;
    for (int i = 0; i < allocationCount; i++) {
        for (int j = 0; j < 16; j++) {
            if (((u8 *) allocations[i])[allocationSizes[i] + j] != 0xA5) {
                fputs("HARNESS ERROR: allocation overrun\n", stderr);
                exit(2);
            }
        }
        free(allocations[i]);
    }
    allocationCount = 0;
    failNextAllocation = 0;
    memset(&sBubbleBatch, 0, sizeof(sBubbleBatch));
    memset(sShadowTextureRuns, 0, sizeof(sShadowTextureRuns));
    memset(loaded, 0, sizeof(loaded));
    memset(&master, 0, sizeof(master));
    memset(&camera, 0, sizeof(camera));
    memset(&frustum, 0, sizeof(frustum));
    memset(objects, 0, sizeof(objects));
    memset(&bubbleGeo, 0, sizeof(bubbleGeo));
    memset(&fishGeo, 0, sizeof(fishGeo));
    root.areaIndex = 1;
    gCurGraphNodeRoot = &root;
    gCurGraphNodeCamera = &camera;
    gCurGraphNodeCamFrustum = &frustum;
    gCurGraphNodeMasterList = &master;
    gCurGraphNodeObject = NULL;
    gCurGraphNodeHeldObject = NULL;
    gMarioObject = NULL;
    gMarioState = &marioState;
    gCurrLevelNum = 0;
    gCurrAnimEnabled = 0;
    shadowQueries = shadowMatrices = 0;
    shadowVisible = TRUE;
    useNativeShadow = FALSE;
    nativeFloorQueries = nativeWaterQueries = 0;
    memset(&nativeShadowFloor, 0, sizeof(nativeShadowFloor));
    nativeShadowFloor.normal.y = 1;
    nativeFloorResult = &nativeShadowFloor;
    nativeWaterFloor = NULL;
    nativeFloorHeight = 0;
    nativeWaterHeight = FLOOR_LOWER_LIMIT_MISC;
    frustum.halfFovHorizontal = 0.7f;
    frustum.halfFovVertical = 0.5f;
    bubbleGeo.node.type = GRAPH_NODE_TYPE_DISPLAY_LIST;
    bubbleGeo.node.flags = GRAPH_RENDER_ACTIVE;
    SET_GRAPH_NODE_LAYER(bubbleGeo.node.flags, LAYER_OCCLUDE_SILHOUETTE_ALPHA);
    bubbleGeo.node.next = &bubbleGeo.node;
    bubbleGeo.displayList = (void *) white_particle_small_dl;
    loaded[MODEL_WHITE_PARTICLE_SMALL] = &bubbleGeo.node;
    compactCount = 0;
    fishGeo.type = GRAPH_NODE_TYPE_SCALE;
    loaded[MODEL_FISH] = &fishGeo;
    gMatStackIndex = 0;
    mtxf_identity(gMatStack[0]);
    mtxf_identity(gCameraTransform);
    memset(gMatStackFixed, 0, sizeof(gMatStackFixed));
    gDisplayListHead = topCommands;
    gEmulator = EMU_MUPEN;
    sAspectRatio = 16.0f / 9.0f; // Preserve the widescreen compatibility bypass by default.
    sCurrPlayMode = 0;
    gCurrAnimType = 0;
    hitboxView = 0;
    sJrbBayVisible = TRUE;
    sJrbCaveVisible = TRUE;
    animationCalls = nativeDraws = debugDraws = masterFlushes = 0;
    gPuppyCallCounter.matrix = 0;
    programCount = n;
    gAreaUpdateCounter = 1;
    for (int i = 0; i < n; i++) {
        objects[i].behavior = &bhvSmallParticleBubbles;
        objects[i].header.gfx.node.flags = GRAPH_RENDER_ACTIVE | GRAPH_RENDER_BILLBOARD;
        objects[i].header.gfx.areaIndex = 1;
        objects[i].header.gfx.sharedChild = &bubbleGeo.node;
        vec3f_set(objects[i].header.gfx.pos, i * 0.87f, i * 0.6f, -1000 + i * 0.31f);
        vec3f_set(objects[i].header.gfx.scale, 2.13f, 1.69f, 1.0f);
        objects[i].timer = 17 + i;
    }
}
static void fixed_transform(Vec3f out, const Mtx *matrix, const s16 point[3]) {
    for (int axis = 0; axis < 3; axis++) {
        out[axis] = 0;
        for (int row = 0; row < 4; row++) {
            int index = row * 4 + axis;
            f32 value =
                ((int32_t) matrix->values[index] * 65536 + (u16) matrix->values[index + 16]) / 65536.0f;
            out[axis] += value * (row < 3 ? point[row] : 1);
        }
        out[axis] *= WORLD_SCALE;
    }
}

// Static N64 matrices pack integer/fraction pairs in big-endian words.
// Convert them to the fixture's native halves before applying vertices.
static void decode_n64_matrix(Mtx *host, const Mtx *native) {
    for (s32 i = 0; i < 16; i++) {
        s32 shift = (i & 1) ? 0 : 16;
        host->values[i] = (u32) native->m[i / 8][(i / 2) % 4] >> shift;
        host->values[i + 16] = (u32) native->m[2 + i / 8][(i / 2) % 4] >> shift;
    }
}

static int inspect(int n, int expectedGroups, int checkPositions) {
    struct DisplayListNode *list = master.listHeads[LAYER_OCCLUDE_SILHOUETTE_ALPHA];
    int groups = 0, sprites = 0, loads = 0, expectedLoads = 0;
    for (; list; list = list->next) {
        Gfx *commands = list->displayList;
        int firstSprite = sprites;
        EXPECT_EQ(commands[0].op, CMD_MATERIAL);
        EXPECT_EQ(commands[0].ptr == white_particle_small_dl_begin, TRUE);
        const Vtx *loadedVertices = NULL;
        int loadedCount = 0, usedVertices = 0, finished = FALSE, restored = FALSE;
        for (int c = 1; c < BUBBLE_BATCH_COMMANDS; c++) {
            Gfx *cmd = &commands[c];
            if (cmd->op == CMD_VERTICES) {
                EXPECT_EQ(usedVertices, loadedCount);
                loadedVertices = cmd->ptr;
                loadedCount = cmd->n;
                loads++;
                usedVertices = 0;
                EXPECT_EQ(loadedCount <= 16 && loadedCount > 0 && loadedCount % 4 == 0, TRUE);
            } else if (cmd->op == CMD_QUAD) {
                EXPECT_EQ(loadedVertices != NULL && !restored && sprites < n, TRUE);
                const Vtx *reference = white_particle_small_vertex;
                if (checkPositions) {
                    const Gfx *model =
                        ((struct GraphNodeDisplayList *) objects[sprites].header.gfx.sharedChild)
                            ->displayList;
                    if (model == cold_water_droplet_dl) {
                        reference = cold_water_droplet_vertex;
                    } else if (model == lava_droplet_dl) {
                        reference = lava_droplet_vertex;
                    }
                }
                for (int k = 0; k < 4; k++) {
                    EXPECT_EQ(cmd->indices[k] >= 0 && cmd->indices[k] < loadedCount, TRUE);
                    const Vtx *v = &loadedVertices[cmd->indices[k]];
                    EXPECT_EQ(v->v.flag, white_particle_small_vertex[k].v.flag);
                    for (int j = 0; j < 2; j++) {
                        EXPECT_EQ(v->v.tc[j], white_particle_small_vertex[k].v.tc[j]);
                    }
                    for (int j = 0; j < 4; j++) {
                        EXPECT_EQ(v->v.cn[j], reference[k].v.cn[j]);
                    }
                    if (checkPositions) {
                        Mat4 legacy;
                        Vec3f actual;
                        mtxf_billboard(legacy, gMatStack[0], objects[sprites].header.gfx.pos,
                                       objects[sprites].header.gfx.scale, camera.roll);
                        fixed_transform(actual, list->transform, v->v.ob);
                        for (int axis = 0; axis < 3; axis++) {
                            f32 expected = legacy[3][axis];
                            for (int row = 0; row < 3; row++) {
                                expected +=
                                    white_particle_small_vertex[k].v.ob[row] * legacy[row][axis];
                            }
                            if (fabsf(actual[axis] - expected) > 0.035f) {
                                fprintf(stderr, "sprite %d corner %d axis %d: %.5f vs %.5f\n", sprites,
                                        k, axis, actual[axis], expected);
                                return FALSE;
                            }
                        }
                    }
                }
                sprites++;
                usedVertices += 4;
            } else if (cmd->op == CMD_MATERIAL) {
                EXPECT_EQ(usedVertices, loadedCount);
                EXPECT_EQ(cmd->ptr == white_particle_small_dl_end + 1, TRUE);
                restored = TRUE;
            } else if (cmd->op == CMD_END) {
                finished = TRUE;
                break;
            } else {
                EXPECT_EQ(cmd->op, CMD_END);
            }
        }
        EXPECT_EQ(finished && restored, TRUE);
        groups++;
        expectedLoads += (sprites - firstSprite + 3) / 4;
    }
    EXPECT_EQ(groups, expectedGroups);
    EXPECT_EQ(sprites, n);
    EXPECT_EQ(loads, expectedLoads);
    EXPECT_EQ(sBubbleBatch.count, 0);
    EXPECT_EQ(gMatStackIndex, 0);
    return TRUE;
}
static int ordinary_sprite_policy(void) {
    const int counts[] = { 1, 2, 17 };
    for (unsigned i = 0; i < sizeof(counts) / sizeof(counts[0]); i++) {
        begin(counts[i]);
        for (int j = 0; j < counts[i]; j++) {
            geo_process_object(&objects[j]);
        }
        geo_flush_bubble_batch();
        if (ORDINARY_SPRITE_BATCHING) {
            EXPECT_EQ(nativeDraws, 0);
            EXPECT_EQ(inspect(counts[i], (counts[i] + 15) / 16, TRUE), TRUE);
        } else {
            EXPECT_EQ(nativeDraws, counts[i]);
            EXPECT_EQ(sBubbleBatch.count, 0);
            struct DisplayListNode *draw = master.listHeads[LAYER_OCCLUDE_SILHOUETTE_ALPHA];
            for (int j = 0; j < counts[i]; j++) {
                EXPECT_EQ(draw != NULL, TRUE);
                EXPECT_EQ(draw->displayList == white_particle_small_dl, TRUE);
                draw = draw->next;
            }
            EXPECT_EQ(draw == NULL, TRUE);
        }
    }
    begin(0);
    return TRUE;
}

static int batch_sizes(void) {
    const int counts[] = { 0, 1, 2, 3, 4, 5, 15, 16, 17, 35, 70, 240 };
    for (unsigned i = 0; i < sizeof(counts) / sizeof(counts[0]); i++) {
        begin(counts[i]);
        for (int j = 0; j < counts[i]; j++) {
            geo_process_object(&objects[j]);
        }
        geo_flush_bubble_batch();
        EXPECT_EQ(nativeDraws, 0);
        EXPECT_EQ(gPuppyCallCounter.matrix, counts[i] + 4 * ((counts[i] + 15) / 16));
        EXPECT_EQ(inspect(counts[i], (counts[i] + 15) / 16, TRUE), TRUE);
    }
    begin(0);
    return TRUE; // Check final canaries too.
}
static int batch_positions(void) {
    for (int pose = 0; pose < 40; pose++) {
        begin(35);
        Vec3s angles = { pose * 501, pose * 1597, pose * 73 };
        mtxf_rotate_zxy_and_translate(gCameraTransform, gVec3fZero, angles);
        camera.roll = pose * 1031;
        vec3f_set(gMatStack[0][3], 17.23f, -31.61f, 44.37f);
        for (int j = 0; j < 35; j++) {
            // A full 70-update bubble lifetime spans roughly 350 units vertically.
            objects[j].header.gfx.pos[0] = -100 + j * 5.73f;
            objects[j].header.gfx.pos[1] = j * 10.17f;
            objects[j].header.gfx.pos[2] = -1000 + (j % 11) * 14.37f;
            objects[j].header.gfx.scale[0] = (pose % 2 ? -1 : 1) * (0.23f + j * 0.08f);
            objects[j].header.gfx.scale[1] = 0.31f + j * 0.07f;
            Vec3f position;
            vec3f_sum(position, objects[j].header.gfx.pos, gMatStack[0][3]);
            EXPECT_EQ(geo_try_batch_bubble(&objects[j], position), TRUE);
        }
        geo_flush_bubble_batch();
        EXPECT_EQ(inspect(35, 3, TRUE), TRUE);
    }
    begin(0);
    return TRUE;
}
static int simple_mario_lod(void) {
    const GeoLayout *simpleBodies[] = { mario_geo_simple_body, he_thicc_mario_geo_simple_body };
    const GeoLayout *lowBodies[] = { mario_geo_load_low_poly_body,
                                     he_thicc_mario_geo_load_low_poly_body };
    EXPECT_EQ(sizeof(mario_geo_simple_body) / sizeof(GeoLayout), 5);
    EXPECT_EQ(sizeof(he_thicc_mario_geo_simple_body) / sizeof(GeoLayout), 5);
    for (int palette = 0; palette < 2; palette++) {
        EXPECT_EQ(simpleBodies[palette][2], lowBodies[palette]);
    }
    for (int console = 0; console < 2; console++) {
        for (int owner = 0; owner < 5; owner++) {
            for (int far = 0; far < 2; far++) {
                begin(1);
                gMarioObject = &objects[0];
                gEmulator = console ? EMU_CONSOLE : EMU_MUPEN;
                gCurGraphNodeObject = owner == 0                 ? NULL
                                      : owner == 1 || owner == 4 ? &objects[0].header.gfx
                                      : owner == 2               ? &gMirrorMario
                                                                 : &objects[1].header.gfx;
                gCurGraphNodeHeldObject = owner == 4 ? (void *) &objects[1] : NULL;
                gMatStack[gMatStackIndex][3][2] = far ? -1600 : -1599;
                struct GraphNode dummy = { 0 };
                sLodChild = &dummy;
                struct GraphNodeLevelOfDetail range = { 0 };
                range.node.children = &dummy;
                range.minDistance = 1600;
                range.maxDistance = 32767;
                lodVisits = 0;
                geo_process_level_of_detail(&range);
                EXPECT_EQ(lodVisits, far && console);
            }
        }
    }
    gCurGraphNodeHeldObject = NULL;
    sLodChild = NULL;
    return 1;
}
static int reflection_upload(void) {
    for (s32 frame = 0; frame < 3; frame++) {
        begin(0);
        gCurLookAt = (void *) otherDl;
        Gfx *beginCommands = gDisplayListHead;
        for (s32 i = 0; i < 12; i++) {
            geo_append_display_list((void *) otherDl, i % 2 ? LAYER_ALPHA : LAYER_OPAQUE);
        }
        EXPECT_EQ(gDisplayListHead == beginCommands, TRUE);
        real_geo_process_master_list_sub(&master);
        s32 lookAtLoads = 0;
        for (Gfx *cmd = beginCommands; cmd < gDisplayListHead; cmd++) {
            if (cmd->op == CMD_LOOKAT) {
                lookAtLoads++;
                EXPECT_EQ(cmd->ptr == gCurLookAt, TRUE);
            }
        }
        EXPECT_EQ(lookAtLoads, 1);
    }
    gCurLookAt = NULL;
    return TRUE;
}

static int mario_silhouette_option(void) {
    for (s32 enabled = 0; enabled <= 1; enabled++) {
        sTestGameOptions[GO_MARIO_SILHOUETTE] = enabled;
        for (s32 mirror = 0; mirror <= 1; mirror++) {
            for (s32 sourceLayer = LAYER_OPAQUE; sourceLayer <= LAYER_TRANSPARENT; sourceLayer++) {
                if (sourceLayer != LAYER_OPAQUE && sourceLayer != LAYER_ALPHA
                    && sourceLayer != LAYER_TRANSPARENT) {
                    continue;
                }
                begin(1);
                gMarioObject = &objects[0];
                gCurGraphNodeObject = mirror ? &gMirrorMario : &objects[0].header.gfx;
                gCurGraphNodeObject->node.flags |= GRAPH_RENDER_SILHOUETTE;
                gCurGraphNodeHeldObject = NULL;
                geo_append_display_list((void *) otherDl, sourceLayer);
                s32 layer = sourceLayer;
                if (enabled && sourceLayer == LAYER_OPAQUE) {
                    layer = LAYER_SILHOUETTE_OPAQUE;
                } else if (enabled && sourceLayer == LAYER_ALPHA) {
                    layer = LAYER_SILHOUETTE_ALPHA;
                }
                EXPECT_EQ(master.listHeads[layer] != NULL, TRUE);
                EXPECT_EQ(master.listHeads[layer]->displayList == (void *) otherDl, TRUE);
                if (sourceLayer == LAYER_TRANSPARENT) {
                    EXPECT_EQ(master.listHeads[LAYER_SILHOUETTE_ALPHA] != NULL, enabled);
                    EXPECT_EQ(master.listHeads[LAYER_TRANSPARENT]->next == NULL, TRUE);
                }
                if (!enabled) {
                    EXPECT_EQ(master.listHeads[LAYER_SILHOUETTE_OPAQUE] == NULL, TRUE);
                    EXPECT_EQ(master.listHeads[LAYER_SILHOUETTE_ALPHA] == NULL, TRUE);
                }
            }
        }
        begin(1);
        gMarioObject = &objects[0];
        gCurGraphNodeObject = &objects[1].header.gfx;
        gCurGraphNodeObject->node.flags |= GRAPH_RENDER_SILHOUETTE;
        geo_append_display_list((void *) otherDl, LAYER_OPAQUE);
        EXPECT_EQ(master.listHeads[LAYER_SILHOUETTE_OPAQUE] != NULL, TRUE);
    }
    sTestGameOptions[GO_MARIO_SILHOUETTE] = TRUE;
    return TRUE;
}

static int character_silhouette(void) {
    for (int palette = 0; palette < 2; palette++) {
        for (int owner = 0; owner < 5; owner++) {
            struct GraphNodeHeldObject heldNode = { 0 };
            begin(1);
            sCharacterPalette = palette;
            gMarioObject = &objects[0];
            gCurGraphNodeObject = owner == 0                 ? NULL
                                  : owner == 1 || owner == 4 ? &objects[0].header.gfx
                                  : owner == 2               ? &gMirrorMario
                                                             : &objects[1].header.gfx;
            heldNode.objNode = &objects[1];
            gCurGraphNodeHeldObject = owner == 4 ? &heldNode : NULL;
            geo_append_display_list((void *) otherDl, LAYER_OPAQUE);
            EXPECT_EQ(master.listTails[LAYER_OPAQUE]->cyanSilhouette,
                      palette == MARIO_PALETTE_HE_THICC && (owner == 1 || owner == 2));
        }
    }
    sCharacterPalette = 0;
    gCurGraphNodeHeldObject = NULL;
    return 1;
}
static int mario_wall_shadow(void) {
    struct Surface wall = { .normal = { 0.0f, 0.0f, -1.0f } };
    const u32 actions[] = { ACT_WATER_IDLE, ACT_BREASTSTROKE, ACT_FLYING,
                            ACT_IDLE,       ACT_TWIRLING,     ACT_DROWNING };
    for (u32 i = 0; i < ARRAY_COUNT(actions); i++) {
        begin(0);
        gMarioObject = &objects[0];
        gCurGraphNodeObject = &gMarioObject->header.gfx;
        gMarioState->action = actions[i];
        gMarioState->flags = MARIO_VANISH_CAP;
        vec3f_set(gMarioState->pos, 10, 20, 30);
        memset(gMarioState->faceAngle, 0, sizeof(gMarioState->faceAngle));
        wallShadowSurface = &wall;
        wallShadowDistance = 300;
        Vec3f pos;
        Gfx *list = create_mario_wall_shadow(pos, 50, 180);
        EXPECT_EQ(list != NULL, i < 3);
        EXPECT_EQ(wallShadowQueries, i < 3);
        if (list != NULL) {
            EXPECT_EQ(list[0].ptr == dl_shadow_circle, TRUE);
            EXPECT_EQ(list[4].ptr == dl_shadow_end, TRUE);
            EXPECT_EQ(list[7].op, CMD_END);
            EXPECT_EQ(list[3].op, CMD_RENDER_MODE);
            EXPECT_EQ(list[3].indices[0], (G_RM_AA_XLU_SURF) & ~AA_EN);
            EXPECT_EQ(list[6].indices[0], G_RM_AA_XLU_SURF);
            master.node.flags = GRAPH_RENDER_Z_BUFFER;
            Gfx *depthList = create_mario_wall_shadow(pos, 50, 180);
            EXPECT_EQ(depthList != NULL, TRUE);
            // Front cues retain depth comparison and do not write depth.
            EXPECT_EQ(depthList[3].op, CMD_RENDER_MODE);
            EXPECT_EQ(depthList[3].indices[0], (G_RM_AA_ZB_XLU_SURF) & ~AA_EN);
            EXPECT_EQ(depthList[3].indices[1], (G_RM_AA_ZB_XLU_SURF2) & ~AA_EN);
            EXPECT_EQ(depthList[3].indices[0] & (Z_CMP | Z_UPD | AA_EN), Z_CMP);
            EXPECT_EQ(depthList[6].op, CMD_RENDER_MODE);
            EXPECT_EQ(depthList[6].indices[0], G_RM_AA_ZB_XLU_DECAL);
            EXPECT_EQ(depthList[6].indices[1], G_RM_AA_ZB_XLU_DECAL2);
            master.node.flags = 0;
            EXPECT_EQ(pos[0], 10);
            EXPECT_EQ(pos[1], 100);
            EXPECT_EQ(pos[2], 328);
            EXPECT_EQ(gCurrShadow.solidity, 90);
            EXPECT_EQ(gCurrShadow.scale[0], 40);
            EXPECT_EQ(gCurrShadow.floorNormal[2], -1);
            EXPECT_EQ(wallShadowFlags, RAYCAST_FIND_FLOOR | RAYCAST_FIND_WALL | RAYCAST_FIND_CEIL
                                           | RAYCAST_MARIO_WALL | RAYCAST_IGNORE_INTANGIBLE
                                           | RAYCAST_IGNORE_VANISH_CAP_WALL);
            wall.type = SURFACE_DEATH_PLANE;
            s32 allocationsBefore = allocationCount;
            EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) == NULL, TRUE);
            EXPECT_EQ(allocationCount, allocationsBefore);
            wall.type = SURFACE_DEFAULT;
            EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) != NULL, TRUE);
            wallShadowDistance = 600;
            EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) == NULL, TRUE);
            wallShadowDistance = 30;
            EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) != NULL, TRUE);
            EXPECT_EQ(gCurrShadow.solidity > 90, TRUE);
            wall.normal.z = 1;
            EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) == NULL, TRUE);
            wall.normal.z = -1;
            failNextAllocation = 1;
            EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) == NULL, TRUE);
            wallShadowSurface = NULL;
            EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) == NULL, TRUE);
        }
    }
    begin(0);
    gMarioObject = &objects[0];
    gCurGraphNodeObject = &gMarioObject->header.gfx;
    vec3f_set(gCurGraphNodeObject->scale, 1, 1, 1);
    gMarioState->action = ACT_FLYING;
    gMarioState->flags = 0;
    gMarioState->faceAngle[0] = 0x2000;
    wallShadowSurface = &wall;
    wallShadowDistance = 100;
    struct GraphNodeShadow node = { 0 };
    node.shadowScale = 100;
    node.shadowSolidity = 180;
    node.shadowType = SHADOW_CIRCLE;
    geo_process_shadow(&node);
    EXPECT_EQ(wallShadowQueries, 1);
    EXPECT_EQ(wallShadowRay[1] > 200, TRUE);
    EXPECT_EQ(shadowQueries, 1);
    EXPECT_EQ(shadowMatrices, 2);
    EXPECT_EQ(gMatStackIndex, 0);
    EXPECT_EQ(master.listHeads[LAYER_MARIO_SHADOW] != NULL, TRUE);
    gCurGraphNodeObject = &gMirrorMario;
    Vec3f pos;
    EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) == NULL, TRUE);
    EXPECT_EQ(wallShadowQueries, 1);
    gMarioState->faceAngle[0] = 0;
    gMarioState->faceAngle[1] = 0x4000;
    gCurGraphNodeObject = &gMarioObject->header.gfx;
    wall.normal.x = -1;
    wall.normal.z = 0;
    EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) != NULL, TRUE);
    EXPECT_EQ(wallShadowRay[0], 600);
    EXPECT_EQ(gCurrShadow.floorNormal[0], -1);
    struct GraphNodeHeldObject held = { 0 };
    gCurGraphNodeHeldObject = &held;
    EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) == NULL, TRUE);
    EXPECT_EQ(wallShadowQueries, 2);
    gCurGraphNodeHeldObject = NULL;
    gMarioState->faceAngle[1] = 0;
    wall.normal.x = 0;
    wall.normal.z = -1;
    const f32 speeds[] = { 0.0f, 10.0f, 50.0f, -50.0f };
    for (u32 i = 0; i < ARRAY_COUNT(speeds); i++) {
        gMarioState->forwardVel = speeds[i];
        f32 range = 600.0f + fabsf(speeds[i]) * 12.0f;
        wallShadowDistance = 350.0f;
        EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) != NULL, TRUE);
        wallShadowDistance = 650.0f;
        EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) != NULL, i != 0);
        EXPECT_EQ(wallShadowRay[2], range);
        if (i != 0) {
            EXPECT_EQ(pos[2], gMarioState->pos[2] + 648.0f);
        }
        wallShadowDistance = range * 0.5f;
        EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) != NULL, TRUE);
        EXPECT_EQ(gCurrShadow.solidity, 90);
        EXPECT_EQ(gCurrShadow.scale[0], 40);
        wallShadowDistance = range;
        EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) == NULL, TRUE);
    }
    gMarioState->forwardVel = 0;
    const f32 slopeNormals[] = { -0.8f, -0.011f, -0.009f, 0.0f, 0.009f, 0.011f, 0.8f };
    for (u32 i = 0; i < ARRAY_COUNT(slopeNormals); i++) {
        wall.normal.y = slopeNormals[i];
        wall.normal.z = -sqrtf(1.0f - wall.normal.y * wall.normal.y);
        wallShadowDistance = 200;
        EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) != NULL, TRUE);
        EXPECT_EQ(gCurrShadow.floorNormal[1], wall.normal.y);
        EXPECT_EQ(gCurrShadow.floorNormal[2], wall.normal.z);
        EXPECT_EQ(pos[1], wallShadowOrigin[1] + wall.normal.y * 2.0f);
        EXPECT_EQ(pos[2], wallShadowOrigin[2] + 200.0f + wall.normal.z * 2.0f);
        wall.normal.z = -wall.normal.z;
        EXPECT_EQ(create_mario_wall_shadow(pos, 50, 180) == NULL, TRUE);
    }
    return TRUE;
}

static int mario_damage_blink_silhouette(void) {
    const s32 layers[] = { LAYER_OPAQUE, LAYER_ALPHA, LAYER_TRANSPARENT };
    for (s32 mirror = 0; mirror < 2; mirror++) {
        for (s32 held = 0; held < 2; held++) {
            for (s32 timer = 4; timer >= 0; timer--) {
                for (s32 phase = 0; phase < 20; phase++) {
                    for (u32 i = 0; i < ARRAY_COUNT(layers); i++) {
                        begin(0);
                        gMarioObject = &objects[0];
                        gCurGraphNodeObject = mirror ? &gMirrorMario : &gMarioObject->header.gfx;
                        gCurGraphNodeObject->node.flags = GRAPH_RENDER_SILHOUETTE;
                        struct GraphNodeHeldObject heldNode = { 0 };
                        gCurGraphNodeHeldObject = held ? &heldNode : NULL;
                        gMarioState->invincTimer = timer;
                        gGlobalTimer = phase % 10;
                        sTestGameOptions[GO_FLASHING] = phase >= 10;
                        gMarioState->flags = MARIO_VANISH_CAP;
                        geo_append_display_list((void *) 0x04012340, layers[i]);
                        s32 dim = !held && timer >= 3 && (phase >= 10 || phase % 5 >= 3);
                        s32 silhouetteLayer = layers[i] == LAYER_OPAQUE ? LAYER_SILHOUETTE_OPAQUE
                                                                        : LAYER_SILHOUETTE_ALPHA;
                        s32 wallCopy = !dim && (layers[i] != LAYER_TRANSPARENT || !held);
                        EXPECT_EQ(master.listHeads[silhouetteLayer] != NULL, wallCopy);
                        EXPECT_EQ(master.listHeads[layers[i]] != NULL,
                                  layers[i] == LAYER_TRANSPARENT || dim);
                        if (wallCopy) {
                            struct DisplayListNode *copy = master.listHeads[silhouetteLayer];
                            u8 expectedAlpha;
                            if (held) {
                                expectedAlpha = SILHOUETTE;
                            } else {
                                expectedAlpha = 255;
                            }
                            EXPECT_EQ(copy->silhouetteAlpha, expectedAlpha);
                        }
                        EXPECT_EQ(gCurGraphNodeObject->node.flags, GRAPH_RENDER_SILHOUETTE);
                        EXPECT_EQ(gMarioState->invincTimer, timer);
                    }
                }
            }
        }
    }
    gCurGraphNodeHeldObject = NULL;
    for (s32 mirror = 0; mirror < 2; mirror++) {
        for (s32 cutout = 0; cutout < 2; cutout++) {
            for (s32 phase = 0; phase < 4; phase++) {
                for (s32 palette = 0; palette < 2; palette++) {
                    begin(0);
                    sCharacterPalette = palette;
                    master.node.flags = GRAPH_RENDER_Z_BUFFER;
                    gMarioObject = &objects[0];
                    gCurGraphNodeObject = mirror ? &gMirrorMario : &gMarioObject->header.gfx;
                    gCurGraphNodeObject->node.flags = GRAPH_RENDER_SILHOUETTE;
                    gMarioState->invincTimer = 4;
                    gGlobalTimer = (phase & 1) ? 3 : 2;
                    sTestGameOptions[GO_FLASHING] = phase >= 2;
                    s32 dim = phase >= 2 || (phase & 1);
                    gMarioState->flags = cutout ? MARIO_VANISH_CAP : 0;
                    geo_append_display_list((void *) mario_butt, LAYER_TRANSPARENT);
                    real_geo_process_master_list_sub(&master);
                    s32 wallPass = FALSE, wallAlpha = -1;
                    const Gfx *wallMaterial = NULL;
                    for (Gfx *cmd = topCommands; cmd < gDisplayListHead; cmd++) {
                        if (cmd->op == CMD_MATERIAL
                            && (cmd->ptr == dl_silhouette_begin
                                || cmd->ptr == dl_mario_dither_silhouette_begin)) {
                            wallPass = TRUE;
                            wallMaterial = cmd->ptr;
                        } else if (cmd->op == CMD_MATERIAL && cmd->ptr == dl_silhouette_end) {
                            wallPass = FALSE;
                        } else if (cmd->op == CMD_FOG_COLOR && wallPass) {
                            wallAlpha = cmd->n;
                            EXPECT_EQ(cmd->indices[0], palette ? 0 : 208);
                            EXPECT_EQ(cmd->indices[1], 208);
                            EXPECT_EQ(cmd->indices[2], 208);
                        }
                    }
                    EXPECT_EQ(wallAlpha, dim ? -1 : 255);
                    EXPECT_EQ(wallMaterial == NULL, dim);
                    if (!dim) {
                        EXPECT_EQ(wallMaterial == dl_mario_dither_silhouette_begin, TRUE);
                    }
                }
            }
        }
    }
    sCharacterPalette = 0;
    return TRUE;
}

static int mario_silhouette_geometry(void) {
    Mtx inset;
    decode_n64_matrix(&inset, &sMarioSilhouetteInset);
    const Vec3s points[] = { { -32, -64, -96 }, { 32, 64, 96 }, { 0, 0, 0 } };
    for (u32 i = 0; i < ARRAY_COUNT(points); i++) {
        Vec3f transformed;
        fixed_transform(transformed, &inset, points[i]);
        for (s32 axis = 0; axis < 3; axis++) {
            EXPECT_EQ(transformed[axis] == points[i][axis] * WORLD_SCALE * 0.96875f, TRUE);
        }
    }
    for (s32 mirror = 0; mirror < 2; mirror++) {
        for (s32 textured = 0; textured < 2; textured++) {
            begin(0);
            master.node.flags = GRAPH_RENDER_Z_BUFFER;
            gMarioObject = &objects[0];
            gCurGraphNodeObject = mirror ? &gMirrorMario : &gMarioObject->header.gfx;
            gCurGraphNodeObject->node.flags = GRAPH_RENDER_SILHOUETTE;
            s32 layer = textured ? LAYER_ALPHA : LAYER_TRANSPARENT;
            gMarioState->flags = textured ? 0 : MARIO_VANISH_CAP;
            const Gfx *material = textured ? otherDl : mario_butt;
            geo_append_display_list((void *) material, layer);
            s32 ordinaryLayer = textured ? LAYER_SILHOUETTE_ALPHA : layer;
            EXPECT_EQ(master.listHeads[ordinaryLayer] != NULL, TRUE);
            struct DisplayListNode *silhouette = master.listHeads[LAYER_SILHOUETTE_ALPHA];
            EXPECT_EQ(silhouette != NULL, TRUE);
            EXPECT_EQ(silhouette->silhouetteGeometry, 0);
            EXPECT_EQ(silhouette->marioSilhouette, TRUE);
            EXPECT_EQ(silhouette->silhouetteAlpha, 255);
            gDisplayListHead = topCommands;
            real_geo_process_master_list_sub(&master);
            s32 originalDraws = 0, insetCommands = 0;
            s32 silhouettePass = FALSE, marioInset = FALSE;
            for (Gfx *cmd = topCommands; cmd < gDisplayListHead; cmd++) {
                if (cmd->op == CMD_MATRIX) {
                    if (cmd->n & G_MTX_LOAD) {
                        marioInset = FALSE;
                    } else {
                        EXPECT_EQ(cmd->ptr == &sMarioSilhouetteInset, TRUE);
                        EXPECT_EQ(marioInset, FALSE);
                        marioInset = TRUE;
                        insetCommands++;
                    }
                } else if (cmd->op == CMD_FOG_COLOR && silhouettePass) {
                    EXPECT_EQ(cmd->n, 255);
                    for (s32 channel = 0; channel < 3; channel++) {
                        EXPECT_EQ(cmd->indices[channel], 208);
                    }
                } else if (cmd->op == CMD_MATERIAL) {
                    if (cmd->ptr == dl_silhouette_begin) {
                        silhouettePass = TRUE;
                    } else if (cmd->ptr == dl_silhouette_end) {
                        silhouettePass = FALSE;
                    } else if (cmd->ptr == material) {
                        originalDraws++;
                        EXPECT_EQ(marioInset, silhouettePass && textured);
                    }
                }
            }
            EXPECT_EQ(originalDraws, 2);
            EXPECT_EQ(insetCommands, textured ? 1 : 0);
        }
    }
    EXPECT_EQ(dl_silhouette_begin[3].op, CMD_BLEND_COLOR);
    EXPECT_EQ(dl_silhouette_begin[3].n, 8);
    EXPECT_EQ(dl_silhouette_begin[4].op, CMD_ALPHA_COMPARE);
    EXPECT_EQ(dl_silhouette_begin[4].n, G_AC_THRESHOLD);
    EXPECT_EQ(dl_mario_silhouette_begin[0].op, CMD_PIPE_SYNC);
    EXPECT_EQ(dl_mario_silhouette_begin[1].op, CMD_RENDER_MODE);
    for (s32 cycle = 0; cycle < 2; cycle++) {
        EXPECT_EQ(dl_mario_silhouette_begin[1].indices[cycle]
                      & (IM_RD | FORCE_BL | CLR_ON_CVG | CVG_X_ALPHA | Z_CMP | Z_UPD),
                  0);
    }
    EXPECT_EQ(dl_mario_silhouette_begin[4].op, CMD_COMBINE);
    EXPECT_EQ(dl_mario_silhouette_begin[4].indices[0], G_CC_PRIMITIVE);
    EXPECT_EQ(dl_mario_silhouette_end[3].op, CMD_PRIM_COLOR);
    EXPECT_EQ(dl_mario_silhouette_end[3].n, 255);
    for (s32 channel = 0; channel < 3; channel++) {
        EXPECT_EQ(dl_mario_silhouette_end[3].indices[channel], 255);
    }
    for (u32 i = 0; i < ARRAY_COUNT(sMarioSilhouetteMap); i++) {
        for (s32 small = FALSE; small <= TRUE; small++) {
            begin(0);
            root.height = SCREEN_HEIGHT / 2;
            gMarioObject = &objects[0];
            gCurGraphNodeObject = &gMarioObject->header.gfx;
            gCurGraphNodeObject->node.flags = GRAPH_RENDER_SILHOUETTE;
            vec3f_set(gCurGraphNodeObject->scale, 1, 1, 1);
            vec3f_set(gCurGraphNodeObject->cameraToObject, 0, 0, small ? -4000 : -500);
            frustum.fov = 45;
            geo_append_display_list((void *) sMarioSilhouetteMap[i].displayList, LAYER_OPAQUE);
            struct DisplayListNode *list = master.listHeads[LAYER_SILHOUETTE_OPAQUE];
            EXPECT_EQ(list->silhouetteGeometry, sMarioSilhouetteMap[i].geometry);
            EXPECT_EQ(list->displayList == sMarioSilhouetteMap[i].displayList, TRUE);
            EXPECT_EQ(list->silhouetteAlpha, 255);
            EXPECT_EQ(list->marioSilhouette, TRUE);
        }
    }
    begin(0);
    gMarioObject = &objects[0];
    gCurGraphNodeObject = &gMarioObject->header.gfx;
    gCurGraphNodeObject->node.flags = GRAPH_RENDER_SILHOUETTE;
    geo_append_display_list((void *) otherDl, LAYER_ALPHA);
    struct DisplayListNode *wing = master.listHeads[LAYER_SILHOUETTE_ALPHA];
    EXPECT_EQ(wing->silhouetteGeometry, 0);
    EXPECT_EQ(wing->silhouetteAlpha, 255);
    EXPECT_EQ(wing->displayList == otherDl, TRUE);
    for (s32 palette = 0; palette < 2; palette++) {
        for (s32 mirror = 0; mirror < 2; mirror++) {
            begin(0);
            master.node.flags = GRAPH_RENDER_Z_BUFFER;
            sCharacterPalette = palette;
            gMarioObject = &objects[0];
            gCurGraphNodeObject = mirror ? &gMirrorMario : &gMarioObject->header.gfx;
            gCurGraphNodeObject->node.flags = GRAPH_RENDER_SILHOUETTE;
            const Gfx *parts[] = {
                palette ? he_thicc_mario_butt : mario_butt,
                palette ? he_thicc_mario_torso : mario_torso,
                palette ? he_thicc_mario_left_arm : mario_left_arm,
            };
            geo_append_display_list((void *) parts[0], LAYER_OPAQUE);
            geo_append_display_list((void *) parts[1], LAYER_OPAQUE);
            // A held object interrupts the solid run and retains its original material.
            struct GraphNodeHeldObject held = { 0 };
            held.objNode = &objects[1];
            gCurGraphNodeHeldObject = &held;
            geo_append_display_list((void *) otherDl, LAYER_OPAQUE);
            EXPECT_EQ(master.listTails[LAYER_SILHOUETTE_OPAQUE]->silhouetteGeometry, 0);
            EXPECT_EQ(master.listTails[LAYER_SILHOUETTE_OPAQUE]->silhouetteAlpha, SILHOUETTE);
            EXPECT_EQ(master.listTails[LAYER_SILHOUETTE_OPAQUE]->marioSilhouette, FALSE);
            gCurGraphNodeHeldObject = NULL;
            geo_append_display_list((void *) parts[2], LAYER_OPAQUE);
            gDisplayListHead = topCommands;
            real_geo_process_master_list_sub(&master);
            s32 starts = 0, ends = 0, legacyStarts = 0, legacyEnds = 0;
            s32 solidDraws = 0, ordinaryDraws = 0, heldDraws = 0, matrices = 0, colours = 0;
            s32 insetCommands = 0, marioInset = FALSE;
            for (Gfx *cmd = topCommands; cmd < gDisplayListHead; cmd++) {
                if (cmd->op == CMD_MATRIX) {
                    if (cmd->n & G_MTX_LOAD) {
                        marioInset = FALSE;
                    } else {
                        EXPECT_EQ(cmd->ptr == &sMarioSilhouetteInset, TRUE);
                        EXPECT_EQ(marioInset, FALSE);
                        marioInset = TRUE;
                        insetCommands++;
                    }
                }
                if (cmd->op == CMD_PRIM_COLOR) {
                    colours++;
                    EXPECT_EQ(cmd->n, 255);
                    EXPECT_EQ(cmd->indices[0], palette ? 0 : 208);
                    EXPECT_EQ(cmd->indices[1], palette ? 255 : 208);
                    EXPECT_EQ(cmd->indices[2], palette ? 255 : 208);
                }
                matrices += cmd->op == CMD_MATRIX;
                if (cmd->op != CMD_MATERIAL) {
                    continue;
                }
                starts += cmd->ptr == dl_mario_silhouette_begin;
                ends += cmd->ptr == dl_mario_silhouette_end;
                legacyStarts += cmd->ptr == dl_silhouette_begin;
                legacyEnds += cmd->ptr == dl_silhouette_end;
                heldDraws += cmd->ptr == otherDl;
                if (cmd->ptr == otherDl) {
                    EXPECT_EQ(marioInset, FALSE);
                }
                for (s32 part = 0; part < 3; part++) {
                    u8 geometry = geo_mario_silhouette_geometry(parts[part]);
                    solidDraws += cmd->ptr == sMarioSilhouetteGeometry[geometry];
                    ordinaryDraws += cmd->ptr == parts[part];
                    if (cmd->ptr == sMarioSilhouetteGeometry[geometry]) {
                        EXPECT_EQ(marioInset, TRUE);
                    } else if (cmd->ptr == parts[part]) {
                        EXPECT_EQ(marioInset, FALSE);
                    }
                }
            }
            EXPECT_EQ(colours, 2);
            EXPECT_EQ(starts, 2);
            EXPECT_EQ(ends, 2);
            EXPECT_EQ(legacyStarts, 1);
            EXPECT_EQ(legacyEnds, 1);
            EXPECT_EQ(solidDraws, 3);
            EXPECT_EQ(ordinaryDraws, 3);
            EXPECT_EQ(heldDraws, 2);
            EXPECT_EQ(insetCommands, 2); // Shared parts reuse their already-inset matrix.
            EXPECT_EQ(matrices, 10);     // Includes the final debug identity-matrix reset.
        }
    }
    sCharacterPalette = 0;
    gCurGraphNodeHeldObject = NULL;
    return TRUE;
}

static int mario_silhouette_lod(void) {
    // Distinct visible detail levels must keep distinct silhouette meshes.
    const Gfx *tiers[] = { mario_torso, mario_medium_poly_torso, mario_low_poly_torso };
    for (s32 a = 0; a < 3; a++) {
        EXPECT_EQ(geo_mario_silhouette_geometry(tiers[a]) != 0, TRUE);
        for (s32 b = a + 1; b < 3; b++) {
            EXPECT_EQ(geo_mario_silhouette_geometry(tiers[a])
                          != geo_mario_silhouette_geometry(tiers[b]),
                      TRUE);
        }
    }
    for (s32 palette = 0; palette < 2; palette++) {
        for (s32 mirror = 0; mirror < 2; mirror++) {
            for (s32 far = 0; far < 2; far++) {
                begin(0);
                gMarioObject = &objects[0];
                gCurGraphNodeObject = mirror ? &gMirrorMario : &gMarioObject->header.gfx;
                vec3f_set(gCurGraphNodeObject->scale, 1, 1, 1);
                vec3f_set(gCurGraphNodeObject->cameraToObject, 0, 0, far ? -4000 : -500);
                gCurGraphNodeObject->node.flags = GRAPH_RENDER_SILHOUETTE;
                const Gfx *body = palette ? he_thicc_mario_torso : mario_torso;
                geo_append_display_list((void *) body, LAYER_OPAQUE);
                EXPECT_EQ(master.listTails[LAYER_SILHOUETTE_OPAQUE]->silhouetteGeometry,
                          geo_mario_silhouette_geometry(body));
                EXPECT_EQ(master.listTails[LAYER_SILHOUETTE_OPAQUE]->displayList == body, TRUE);
            }
        }
    }
    gCurGraphNodeObject = NULL;
    return TRUE;
}

static int mario_floor_lift(void) {
    begin(1);
    gMarioObject = &objects[0];
    struct GraphNodeObject *object = &gMarioObject->header.gfx;
    s16 values[63] = { 0 };
    u16 indices[126] = { 0 };
    for (s32 i = 0; i < 63; i++) {
        indices[i * 2] = 1;
    }
    struct Animation animation = {
        .animYTransDivisor = 189,
        .unusedBoneCount = 20,
        .values = values,
        .index = indices,
    };
    object->animInfo.curAnim = &animation;
    object->animInfo.animYTrans = 189;
    object->animInfo.animTimer = gAreaUpdateCounter;
    object->node.flags = GRAPH_RENDER_ACTIVE | GRAPH_RENDER_SILHOUETTE;
    vec3s_set(object->angle, 0, 0, 0);
    vec3f_set(object->scale, 1, 1, 1);
    vec3f_set(object->pos, 10, 20, -500);
    vec3f_copy(gMarioState->pos, object->pos);
    gMarioState->floorHeight = 20;
    gMarioState->floor = &nativeShadowFloor;
    gMarioState->action = ACT_WALKING;
    gMarioState->flags = 0;
    gBodyStates[0].punchState = 0;
    gCurrAnimData = values;
    gCurrAnimAttribute = indices;
    gCurrAnimTranslationMultiplier = 1;
    f32 peak = 0;
    s32 peakFrame = 0;
    for (u32 i = 0; i < ARRAY_COUNT(sMarioFloorLiftAnims); i++) {
        const struct MarioFloorLiftAnim *pose = &sMarioFloorLiftAnims[i];
        object->animInfo.animID = pose->animID;
        for (s32 frame = 0; frame < pose->frames; frame++) {
            gCurrAnimFrame = object->animInfo.animFrame = frame;
            f32 lift = geo_mario_floor_lift(object);
            EXPECT_EQ(lift >= 0 && lift <= 8, TRUE);
            EXPECT_EQ(lift, sMarioFloorLiftDepth[pose->offset + frame] * 0.0625f);
            if (pose->animID == MARIO_ANIM_WALKING && lift > peak) {
                peak = lift;
                peakFrame = frame;
            }
        }
    }
    EXPECT_EQ(peak > 7 && peak < 8, TRUE);
    object->animInfo.animID = MARIO_ANIM_WALKING;
    gCurrAnimFrame = object->animInfo.animFrame = peakFrame;
    EXPECT_EQ(geo_mario_floor_lift(object), peak);
    nativeShadowFloor.normal.y = COS10;
    EXPECT_EQ(geo_mario_floor_lift(object) > 0 && geo_mario_floor_lift(object) < peak, TRUE);
    nativeShadowFloor.normal.y = COS15;
    EXPECT_EQ(geo_mario_floor_lift(object), 0);
    nativeShadowFloor.normal.y = 1;
    gMarioState->action = ACT_JUMP;
    EXPECT_EQ(geo_mario_floor_lift(object), 0);
    gMarioState->action = ACT_WALKING;
    gMarioState->quicksandDepth = 1;
    EXPECT_EQ(geo_mario_floor_lift(object), 0);
    gMarioState->quicksandDepth = 0;
    gMarioState->squishTimer = 1;
    EXPECT_EQ(geo_mario_floor_lift(object), 0);
    gMarioState->squishTimer = 0;
    gMarioState->pos[1] += 1;
    EXPECT_EQ(geo_mario_floor_lift(object), 0);
    gMarioState->pos[1] -= 1;
    gCurrAnimData = values + 1;
    EXPECT_EQ(geo_mario_floor_lift(object), 0);
    gCurrAnimData = values;
    object->scale[1] = 0.4f;
    EXPECT_EQ(geo_mario_floor_lift(object), 0);
    object->scale[1] = 1;
    gBodyStates[0].punchState = 0x81;
    EXPECT_EQ(geo_mario_floor_lift(object), 0);
    gBodyStates[0].punchState = 0;
    object->animInfo.animID = MARIO_ANIM_DAB;
    EXPECT_EQ(geo_mario_floor_lift(object), 0);
    object->animInfo.animID = MARIO_ANIM_WALKING;
    gMirrorMario = *object;
    gMirrorMario.scale[0] = -1;
    EXPECT_EQ(geo_mario_floor_lift(&gMirrorMario), peak);

    // Traverse the real object/shadow/body-scale path without advancing the pose.
    floorPoseNative = TRUE;
    animation.loopEnd = 77;
    objectShadowGeo = (struct GraphNodeShadow){ 0 };
    objectShadowGeo.node.type = GRAPH_NODE_TYPE_SHADOW;
    objectShadowGeo.shadowScale = 100;
    objectShadowGeo.shadowSolidity = 180;
    objectShadowGeo.shadowType = SHADOW_CIRCLE;
    objectShadowGeo.node.children = &fishScaleGeo.node;
    fishScaleGeo = (struct GraphNodeScale){ 0 };
    fishScaleGeo.node.type = GRAPH_NODE_TYPE_SCALE;
    fishScaleGeo.node.parent = &objectShadowGeo.node;
    floorBodyGeo = (struct GraphNodeDisplayList){ 0 };
    floorBodyGeo.node.type = GRAPH_NODE_TYPE_DISPLAY_LIST;
    floorBodyGeo.displayList = (void *) mario_left_foot;
    fishScaleGeo.node.children = &floorBodyGeo.node;
    fishScaleGeo.scale = 0.25f;
    object->sharedChild = &objectShadowGeo.node;
    geo_process_object(gMarioObject);
    EXPECT_EQ(sMarioFloorLift, peak);
    EXPECT_EQ(queriedShadowPos[1], 20);
    EXPECT_EQ(object->pos[1], 20);
    EXPECT_EQ(gMarioState->pos[1], 20);
    EXPECT_EQ(gMarioState->floorHeight, 20);
    EXPECT_EQ(nativeFloorQueries, 0);
    EXPECT_EQ(nativeWaterQueries, 0);
    EXPECT_EQ(gMatStackIndex, 0);
    Mat4 expectedBody;
    Mtx expectedFixed;
    mtxf_identity(expectedBody);
    expectedBody[0][0] = expectedBody[1][1] = expectedBody[2][2] = 0.25f;
    vec3f_copy(expectedBody[3], object->pos);
    expectedBody[3][1] += peak;
    mtxf_to_mtx(&expectedFixed, expectedBody);
    EXPECT_EQ(memcmp(master.listTails[LAYER_SILHOUETTE_OPAQUE]->transform, &expectedFixed,
                     sizeof(expectedFixed)),
              0);
    EXPECT_EQ(master.listTails[LAYER_SILHOUETTE_OPAQUE]->silhouetteGeometry != 0, TRUE);

    Mat4 aligned;
    mtxf_identity(aligned);
    vec3f_copy(aligned[3], object->pos);
    object->throwMatrix = &aligned;
    geo_process_object(gMarioObject);
    EXPECT_EQ(sMarioFloorLift, 0);
    expectedBody[3][1] = 20;
    mtxf_to_mtx(&expectedFixed, expectedBody);
    EXPECT_EQ(memcmp(master.listTails[LAYER_SILHOUETTE_OPAQUE]->transform, &expectedFixed,
                     sizeof(expectedFixed)),
              0);

    // Mirror copies receive the same lift without changing their reflection.
    gCurGraphNodeObject = &gMirrorMario;
    sMarioFloorLift = peak;
    mtxf_identity(gMatStack[0]);
    gMatStack[0][0][0] = -1;
    vec3f_copy(gMatStack[0][3], object->pos);
    geo_process_scale(&fishScaleGeo);
    expectedBody[0][0] = -0.25f;
    expectedBody[3][1] = 20 + peak;
    mtxf_to_mtx(&expectedFixed, expectedBody);
    EXPECT_EQ(memcmp(master.listTails[LAYER_SILHOUETTE_OPAQUE]->transform, &expectedFixed,
                     sizeof(expectedFixed)),
              0);

    // A held object's own body scale must not apply its holder's lift twice.
    struct GraphNodeHeldObject held = { 0 };
    held.objNode = &objects[1];
    gCurGraphNodeHeldObject = &held;
    geo_process_scale(&fishScaleGeo);
    expectedBody[3][1] = 20;
    mtxf_to_mtx(&expectedFixed, expectedBody);
    EXPECT_EQ(memcmp(master.listTails[LAYER_SILHOUETTE_OPAQUE]->transform, &expectedFixed,
                     sizeof(expectedFixed)),
              0);
    gCurGraphNodeHeldObject = NULL;
    gCurGraphNodeObject = NULL;
    floorPoseNative = FALSE;
    return TRUE;
}

static int collectible_silhouette(void) {
    struct Object *obj;
    struct DisplayListNode *silhouette;
    u8 tint = COLLECTIBLE_SILHOUETTE_TINT_NONE;

    begin(0);
    obj = &objects[0];
    vec3f_set(gMarioState->pos, 0, 0, 0);
    loaded[MODEL_RED_COIN] = &fishGeo;
    loaded[MODEL_BLUE_COIN] = &bubbleGeo.node;
    loaded[MODEL_1UP] = &fishScaleGeo.node;
    loaded[MODEL_STAR] = &objectShadowGeo.node;
    obj->activeFlags = 1;
    obj->behavior = bhvRedCoin;
    obj->header.gfx.sharedChild = loaded[MODEL_RED_COIN];
    gCurGraphNodeObject = &obj->header.gfx;
    vec3f_set(obj->header.gfx.pos, 0, 80, 0);
    geo_append_display_list((void *) otherDl, LAYER_OCCLUDE_SILHOUETTE_ALPHA);
    silhouette = master.listHeads[LAYER_SILHOUETTE_ALPHA];
    EXPECT_EQ(silhouette != NULL, TRUE);
    EXPECT_EQ(silhouette->silhouetteAlpha, 127);
    EXPECT_EQ(silhouette->silhouetteTint, COLLECTIBLE_SILHOUETTE_TINT_RED);
    EXPECT_EQ(master.listHeads[LAYER_OCCLUDE_SILHOUETTE_ALPHA]->displayList == otherDl, TRUE);

    obj->header.gfx.pos[0] = 120;
    geo_append_display_list((void *) otherDl, LAYER_OCCLUDE_SILHOUETTE_ALPHA);
    EXPECT_EQ(silhouette->next->silhouetteAlpha, 127);
    obj->header.gfx.pos[0] = 300;
    geo_append_display_list((void *) otherDl, LAYER_OCCLUDE_SILHOUETTE_ALPHA);
    EXPECT_EQ(silhouette->next->next->silhouetteAlpha, 79);
    obj->header.gfx.pos[0] = 600;
    geo_append_display_list((void *) otherDl, LAYER_OCCLUDE_SILHOUETTE_ALPHA);
    EXPECT_EQ(silhouette->next->next->next == NULL, TRUE);
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 0);
    obj->header.gfx.pos[0] = 601;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 0);
    obj->header.gfx.pos[0] = 119;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 127);
    obj->header.gfx.pos[0] = 0;
    obj->header.gfx.pos[1] = 220;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 127);
    obj->header.gfx.pos[1] = 310;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 79);
    obj->header.gfx.pos[1] = 460;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 0);
    obj->header.gfx.pos[1] = 461;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 0);
    obj->header.gfx.pos[1] = -300;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 0);
    obj->header.gfx.pos[1] = -150;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 79);
    obj->header.gfx.pos[0] = 480;
    obj->header.gfx.pos[1] = 400;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 0);

    obj->header.gfx.pos[0] = 0;
    obj->header.gfx.pos[1] = 80;
    obj->header.gfx.sharedChild = loaded[MODEL_STAR];
    obj->behavior = bhvStar;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 127);
    EXPECT_EQ(tint, COLLECTIBLE_SILHOUETTE_TINT_STAR);
    obj->behavior = bhvStarSpawnCoordinates;
    obj->oAction = SPAWN_STAR_ARC_CUTSCENE_ACT_END - 1;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 0);
    obj->oAction++;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 127);
    obj->header.gfx.sharedChild = loaded[MODEL_BLUE_COIN];
    obj->oInteractType = INTERACT_COIN;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 127);
    EXPECT_EQ(tint, COLLECTIBLE_SILHOUETTE_TINT_BLUE);
    obj->oIntangibleTimer = 1;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 0);
    obj->oIntangibleTimer = 0;
    obj->header.gfx.sharedChild = loaded[MODEL_1UP];
    obj->behavior = bhv1Up;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 127);
    EXPECT_EQ(tint, COLLECTIBLE_SILHOUETTE_TINT_GREEN);
    obj->behavior = NULL;
    EXPECT_EQ(geo_collectible_silhouette_alpha(obj, &tint), 0);
    return TRUE;
}
static int one_up_cutout_variant(const Gfx *sprite) {
    Mtx matrix = { 0 };
    for (s32 zbuffer = 0; zbuffer < 2; zbuffer++) {
        for (s32 silhouette = 0; silhouette < 2; silhouette++) {
            for (s32 dim = 0; dim < 2; dim++) {
                begin(0);
                master.node.flags = zbuffer ? GRAPH_RENDER_Z_BUFFER : 0;
                u32 layerMode =
                    AA_EN | IM_RD | CVG_X_ALPHA | ALPHA_CVG_SEL | (zbuffer ? Z_CMP | Z_UPD : 0);
                for (s32 layer = 0; layer < LAYER_COUNT; layer++) {
                    renderModeTable_1Cycle[zbuffer].modes[layer] = layerMode;
                    renderModeTable_2Cycle[zbuffer].modes[layer] = layerMode;
                }
                struct DisplayListNode following = { .transform = &matrix,
                                                     .displayList = (void *) otherDl };
                struct DisplayListNode mushroom = {
                    .transform = &matrix,
                    .displayList = (void *) sprite,
                    .blinkAlpha = dim ? 51 : 0,
                    .silhouetteTint = silhouette ? COLLECTIBLE_SILHOUETTE_TINT_GREEN : 0,
                    .next = &following
                };
                struct DisplayListNode wallCopy = mushroom;
                wallCopy.next = NULL;
                wallCopy.blinkAlpha = 0;
                wallCopy.silhouetteAlpha = 160;
                s32 layer = dim ? LAYER_TRANSPARENT : LAYER_OCCLUDE_SILHOUETTE_ALPHA;
                master.listHeads[layer] = &mushroom;
                if (silhouette && zbuffer) {
                    master.listHeads[LAYER_SILHOUETTE_ALPHA] = &wallCopy;
                }
                real_geo_process_master_list_sub(&master);
                s32 alphaCompare = G_AC_NONE, threshold = 0, inWallCopy = FALSE;
                s32 ordinaryDraws = 0, followingDraws = 0, wallDraws = 0;
                u32 mode = 0;
                for (Gfx *cmd = topCommands; cmd < gDisplayListHead; cmd++) {
                    if (cmd->op == CMD_ALPHA_COMPARE) {
                        alphaCompare = cmd->n;
                    } else if (cmd->op == CMD_BLEND_COLOR) {
                        threshold = cmd->n;
                    } else if (cmd->op == CMD_RENDER_MODE) {
                        mode = cmd->indices[0] | cmd->indices[1];
                    } else if (cmd->op == CMD_MATERIAL) {
                        if (cmd->ptr == dl_silhouette_begin) {
                            inWallCopy = TRUE;
                        } else if (cmd->ptr == dl_silhouette_end) {
                            inWallCopy = FALSE;
                        } else if (cmd->ptr == sprite) {
                            if (inWallCopy) {
                                wallDraws++;
                                EXPECT_EQ(alphaCompare, G_AC_THRESHOLD);
                                EXPECT_EQ(threshold, 128);
                                EXPECT_EQ(mode
                                              & (AA_EN | CVG_X_ALPHA | ALPHA_CVG_SEL | CLR_ON_CVG
                                                 | Z_CMP | Z_UPD),
                                          0);
                                EXPECT_EQ(mode & CVG_DST_FULL, CVG_DST_FULL);
                                EXPECT_EQ(mode & (IM_RD | FORCE_BL), IM_RD | FORCE_BL);
                            } else {
                                ordinaryDraws++;
                                if (dim) {
                                    EXPECT_EQ(alphaCompare, G_AC_THRESHOLD);
                                    EXPECT_EQ(threshold, 1);
                                    EXPECT_EQ(mode & Z_UPD, 0);
                                    EXPECT_EQ(mode & CVG_X_ALPHA, CVG_X_ALPHA);
                                } else {
                                    EXPECT_EQ(alphaCompare, G_AC_THRESHOLD);
                                    EXPECT_EQ(threshold, 128);
                                    EXPECT_EQ(mode & (AA_EN | CVG_X_ALPHA | ALPHA_CVG_SEL), 0);
                                    EXPECT_EQ(mode & CVG_DST_FULL, CVG_DST_FULL);
                                    EXPECT_EQ(mode & (Z_CMP | Z_UPD), zbuffer ? Z_CMP | Z_UPD : 0);
                                }
                            }
                        } else if (cmd->ptr == otherDl) {
                            followingDraws++;
                            EXPECT_EQ(alphaCompare, G_AC_NONE);
                            EXPECT_EQ(threshold, 0);
                            EXPECT_EQ(mode, layerMode);
                        }
                    }
                }
                EXPECT_EQ(ordinaryDraws, 1);
                EXPECT_EQ(followingDraws, 1);
                EXPECT_EQ(wallDraws, silhouette && zbuffer ? 1 : 0);
            }
        }
    }
    return TRUE;
}

static int one_up_cutout(void) {
    return one_up_cutout_variant(mushroom_1up_seg3_dl_0302A660)
           && one_up_cutout_variant(poison_mushroom_dl);
}

static int master_list_effects(void) {
    // The sprite inset shrinks X/Y without moving the sprite in Z.
    Mtx inset;
    decode_n64_matrix(&inset, &sCollectibleSilhouetteInset);
    const Vec3s points[] = { { -32, -64, 0 }, { 32, 64, 0 }, { 0, 0, -35 }, { 0, 0, 35 } };
    for (u32 i = 0; i < ARRAY_COUNT(points); i++) {
        Vec3f transformed;
        fixed_transform(transformed, &inset, points[i]);
        EXPECT_EQ(transformed[0] == points[i][0] * WORLD_SCALE * 0.9375f, TRUE);
        EXPECT_EQ(transformed[1] == points[i][1] * WORLD_SCALE * 0.9375f, TRUE);
        EXPECT_EQ(transformed[2] == points[i][2] * WORLD_SCALE, TRUE);
    }
    const s32 modelIds[] = { MODEL_RED_COIN, MODEL_BLUE_COIN, MODEL_1UP, MODEL_STAR };
    const BehaviorScript *behaviors[] = { bhvRedCoin, bhvRedCoin, bhv1Up, bhvStar };
    const u8 colors[][3] = {
        { 255, 190, 190 }, { 190, 214, 255 }, { 195, 255, 195 }, { 255, 239, 185 }
    };
    for (s32 kind = 0; kind < 4; kind++) {
        for (s32 dim = 0; dim < 2; dim++) {
            for (s32 zbuffer = 0; zbuffer < 2; zbuffer++) {
                for (s32 palette = 0; palette < 2; palette++) {
                    begin(0);
                    sCharacterPalette = palette;
                    master.node.flags = zbuffer ? GRAPH_RENDER_Z_BUFFER : 0;
                    // Layer modes are inputs; observe exact per-draw changes and restoration.
                    for (s32 layer = 0; layer < LAYER_COUNT; layer++) {
                        renderModeTable_1Cycle[zbuffer].modes[layer] =
                            IM_RD | AA_EN | CVG_X_ALPHA | (zbuffer ? Z_CMP | Z_UPD : 0);
                        renderModeTable_2Cycle[zbuffer].modes[layer] =
                            renderModeTable_1Cycle[zbuffer].modes[layer];
                    }
                    gMarioObject = &objects[1];
                    gCurGraphNodeObject = &gMarioObject->header.gfx;
                    gCurGraphNodeObject->node.flags = GRAPH_RENDER_SILHOUETTE;
                    geo_append_display_list((void *) fishDl, LAYER_OPAQUE);

                    struct Object *obj = &objects[0];
                    for (s32 model = 0; model < 4; model++) {
                        loaded[modelIds[model]] = NULL;
                    }
                    loaded[modelIds[kind]] = &bubbleGeo.node;
                    obj->header.gfx.sharedChild = &bubbleGeo.node;
                    obj->activeFlags = ACTIVE_FLAG_ACTIVE | (dim ? ACTIVE_FLAG_BLINKING : 0);
                    obj->behavior = behaviors[kind];
                    obj->oInteractType = INTERACT_COIN;
                    vec3f_set(obj->header.gfx.pos, 0, 80, 0);
                    vec3f_set(gMarioState->pos, 0, 0, 0);
                    gCurGraphNodeObject = &obj->header.gfx;
                    s32 layer =
                        kind == 3 ? LAYER_OCCLUDE_SILHOUETTE_OPAQUE : LAYER_OCCLUDE_SILHOUETTE_ALPHA;
                    geo_append_display_list((void *) otherDl, layer);
                    if (kind == 3) {
                        geo_append_display_list((void *) shadowDl, LAYER_OCCLUDE_SILHOUETTE_ALPHA);
                    }
                    gCurGraphNodeObject = NULL;
                    geo_append_display_list((void *) white_particle_small_dl, layer);

                    for (s32 repeat = 0; repeat < 2; repeat++) {
                        gDisplayListHead = topCommands;
                        real_geo_process_master_list_sub(&master);
                        s32 collectibleDraws = 0, marioDraws = 0, ordinaryDraws = 0;
                        s32 silhouettes = 0;
                        s32 spriteInset = FALSE, marioInset = FALSE;
                        s32 insetCommands = 0, marioInsetCommands = 0;
                        u32 mode = 0;
                        u8 alpha = 255;
                        s32 red = 255, green = 255, blue = 255;
                        for (Gfx *cmd = topCommands; cmd < gDisplayListHead; cmd++) {
                            if (cmd->op == CMD_MATRIX) {
                                if (cmd->n & G_MTX_LOAD) {
                                    spriteInset = FALSE;
                                    marioInset = FALSE;
                                } else if (cmd->ptr == &sMarioSilhouetteInset) {
                                    EXPECT_EQ(marioInset, FALSE);
                                    EXPECT_EQ(spriteInset, FALSE);
                                    marioInset = TRUE;
                                    marioInsetCommands++;
                                } else {
                                    EXPECT_EQ(cmd->ptr == &sCollectibleSilhouetteInset, TRUE);
                                    EXPECT_EQ(marioInset, FALSE);
                                    spriteInset = TRUE;
                                    insetCommands++;
                                }
                            } else if (cmd->op == CMD_RENDER_MODE) {
                                mode = cmd->indices[0];
                            } else if (cmd->op == CMD_FOG_COLOR) {
                                alpha = cmd->n;
                                red = cmd->indices[0];
                                green = cmd->indices[1];
                                blue = cmd->indices[2];
                            } else if (cmd->op == CMD_MATERIAL) {
                                if (cmd->ptr == dl_silhouette_begin) {
                                    silhouettes = TRUE;
                                } else if (cmd->ptr == dl_silhouette_end) {
                                    silhouettes = FALSE;
                                } else if (cmd->ptr == otherDl || cmd->ptr == shadowDl) {
                                    collectibleDraws++;
                                    EXPECT_EQ(spriteInset, silhouettes && kind != 3);
                                    EXPECT_EQ(marioInset, FALSE);
                                    if (silhouettes) {
                                        EXPECT_EQ(alpha, dim ? 51 : 127);
                                        EXPECT_EQ(red, colors[kind][0]);
                                        EXPECT_EQ(green, colors[kind][1]);
                                        EXPECT_EQ(blue, colors[kind][2]);
                                    } else if (dim) {
                                        EXPECT_EQ(alpha, 51);
                                        EXPECT_EQ(mode & Z_UPD, 0);
                                        EXPECT_EQ(mode & IM_RD, IM_RD);
                                        EXPECT_EQ(mode & CVG_X_ALPHA, CVG_X_ALPHA);
                                    } else {
                                        EXPECT_EQ(mode & IM_RD, zbuffer && kind != 2 ? 0 : IM_RD);
                                        EXPECT_EQ(mode & AA_EN, AA_EN);
                                        EXPECT_EQ(mode & (Z_CMP | Z_UPD), zbuffer ? Z_CMP | Z_UPD : 0);
                                    }
                                } else if (cmd->ptr == fishDl) {
                                    marioDraws++;
                                    EXPECT_EQ(spriteInset, FALSE);
                                    EXPECT_EQ(marioInset, silhouettes);
                                    if (silhouettes) {
                                        EXPECT_EQ(red, palette == MARIO_PALETTE_HE_THICC ? 0 : 208);
                                    }
                                } else if (cmd->ptr == white_particle_small_dl) {
                                    ordinaryDraws++;
                                    EXPECT_EQ(spriteInset, FALSE);
                                    EXPECT_EQ(marioInset, FALSE);
                                    EXPECT_EQ(mode, renderModeTable_1Cycle[zbuffer].modes[layer]);
                                }
                            }
                        }
                        EXPECT_EQ(collectibleDraws, (kind == 3 ? 2 : 1) * (zbuffer ? 2 : 1));
                        EXPECT_EQ(marioDraws, zbuffer ? 2 : 1);
                        EXPECT_EQ(ordinaryDraws, 1);
                        EXPECT_EQ(insetCommands, kind != 3 && zbuffer);
                        EXPECT_EQ(marioInsetCommands, zbuffer);
                    }
                }
            }
        }
    }
    sCharacterPalette = 0;
    return TRUE;
}

static Gfx *unreviewed_particle_callback(s32 context, struct GraphNode *node, void *matrix) {
    (void) context;
    (void) node;
    (void) matrix;
    return (Gfx *) otherDl;
}

static int particle_texture_callbacks(void) {
    const s32 models[] = { MODEL_MIST, MODEL_WHITE_PUFF, MODEL_SMALL_WATER_SPLASH };
    for (u32 i = 0; i < sizeof(models) / sizeof(models[0]); i++) {
        begin(0);
        loaded[models[i]] = &bubbleGeo.node;
        objects[0].header.gfx.sharedChild = &bubbleGeo.node;
        objects[0].oOpacity = 127;
        gCurGraphNodeObject = &objects[0].header.gfx;
        struct GraphNodeGenerated generated = { 0 };
        generated.fnNode.func = models[i] == MODEL_SMALL_WATER_SPLASH
                                    ? geo_update_small_water_splash_opacity
                                    : geo_update_layer_transparency;
        geo_process_generated_list(&generated);
        struct DisplayListNode *alpha = master.listTails[LAYER_TRANSPARENT];
        EXPECT_EQ(alpha != NULL, TRUE);
        EXPECT_EQ(alpha->textureReuse, TRUE);
        EXPECT_EQ(alpha->textureReuseEnvironment, TRUE);
        EXPECT_EQ(sTextureReuseEnvironment, FALSE);
        Gfx *command = alpha->displayList;
        EXPECT_EQ(command->op, CMD_FOG_COLOR);
        EXPECT_EQ(command->n, 127);
        EXPECT_EQ(command->indices[0], 255);
        EXPECT_EQ(command->indices[1], 255);
        EXPECT_EQ(command->indices[2], 255);
        generated.fnNode.func = unreviewed_particle_callback;
        SET_GRAPH_NODE_LAYER(generated.fnNode.node.flags, LAYER_TRANSPARENT);
        geo_process_generated_list(&generated);
        EXPECT_EQ(master.listTails[LAYER_TRANSPARENT]->textureReuseEnvironment, FALSE);
        EXPECT_EQ(sTextureReuseEnvironment, FALSE);
        geo_append_display_list((void *) mist_seg3_dl_03000880, LAYER_TRANSPARENT);
        EXPECT_EQ(master.listTails[LAYER_TRANSPARENT]->textureReuseEnvironment, FALSE);
    }
    return TRUE;
}

static int sprite_texture_runs(void) {
    Mtx matrix = { 0 };
    // Each native coin/splash frame must reload after interruption, dim drawing,
    // or a layer boundary, even when its segmented display-list pointer repeats.
    for (u32 material = 0; material < sizeof(textureReuseMaterials) / sizeof(textureReuseMaterials[0]);
         material++) {
        const Gfx *full = textureReuseMaterials[material].full;
        const Gfx *reuse = textureReuseMaterials[material].reuse;
        for (s32 zbuffer = 0; zbuffer < 2; zbuffer++) {
            begin(0);
            master.node.flags = zbuffer ? GRAPH_RENDER_Z_BUFFER : 0;
            struct DisplayListNode lists[10] = { 0 };
            const Gfx *expected[] = {
                full, reuse, otherDl, full, reuse, full, full, reuse, full, reuse
            };
            for (s32 i = 0; i < 10; i++) {
                lists[i].displayList = (void *) (i == 2 ? otherDl : full);
                lists[i].textureReuse = i != 2;
                lists[i].transform = &matrix;
                lists[i].next = i == 7 || i == 9 ? NULL : &lists[i + 1];
            }
            lists[5].blinkAlpha = 51;
            master.listHeads[LAYER_TRANSPARENT] = &lists[0];
            master.listHeads[LAYER_TRANSPARENT_INTER] = &lists[8];
            real_geo_process_master_list_sub(&master);
            s32 draws = 0;
            for (Gfx *command = topCommands; command < gDisplayListHead; command++) {
                if (command->op == CMD_MATERIAL
                    && (command->ptr == full || command->ptr == reuse || command->ptr == otherDl)) {
                    EXPECT_EQ(command->ptr == expected[draws], TRUE);
                    draws++;
                }
            }
            EXPECT_EQ(draws, 10);
        }
        begin(0);
        master.node.flags = GRAPH_RENDER_Z_BUFFER;
        struct DisplayListNode silhouettes[2] = { 0 };
        for (s32 i = 0; i < 2; i++) {
            silhouettes[i].displayList = (void *) full;
            silhouettes[i].textureReuse = TRUE;
            silhouettes[i].transform = &matrix;
            silhouettes[i].next = i == 0 ? &silhouettes[1] : NULL;
        }
        master.listHeads[LAYER_SILHOUETTE_ALPHA] = silhouettes;
        real_geo_process_master_list_sub(&master);
        s32 silhouettePass = FALSE;
        s32 silhouetteDraws = 0;
        for (Gfx *command = topCommands; command < gDisplayListHead; command++) {
            if (command->ptr == dl_silhouette_begin) {
                silhouettePass = TRUE;
            } else if (command->ptr == dl_silhouette_end) {
                silhouettePass = FALSE;
            } else if (silhouettePass && command->op == CMD_MATERIAL) {
                EXPECT_EQ(command->ptr == full, TRUE);
                silhouetteDraws++;
            }
        }
        EXPECT_EQ(silhouetteDraws, 2);
    }

    // Approved alpha/tint lists preserve the resident frame; arbitrary generated
    // lists must end the run. Alpha commands remain in their original order.
    const Gfx *particleFrames[] = { mist_seg3_dl_03000880,        mist_seg3_dl_03000920,
                                    sparkles_seg4_dl_0402A4F8,    sparkles_seg4_dl_0402A510,
                                    small_water_splash_seg4_dl_0, small_water_splash_seg4_dl_1 };
    for (u32 frame = 0; frame < sizeof(particleFrames) / sizeof(particleFrames[0]); frame++) {
        begin(0);
        void *previous = NULL;
        void *reuse = NULL;
        struct DisplayListNode mesh = { 0 };
        struct DisplayListNode alpha = { 0 };
        mesh.textureReuse = TRUE;
        mesh.displayList = (void *) particleFrames[frame];
        alpha.textureReuse = TRUE;
        alpha.textureReuseEnvironment = TRUE;
        alpha.displayList = (void *) otherDl;
        Gfx *head = geo_draw_texture_reuse_display_list(topCommands, &mesh, &previous, &reuse);
        head = geo_draw_texture_reuse_display_list(head, &alpha, &previous, &reuse);
        head = geo_draw_texture_reuse_display_list(head, &mesh, &previous, &reuse);
        EXPECT_EQ(head - topCommands, 3);
        EXPECT_EQ(topCommands[0].ptr == mesh.displayList, TRUE);
        EXPECT_EQ(topCommands[1].ptr == alpha.displayList, TRUE);
        EXPECT_EQ(topCommands[2].ptr == geo_texture_reuse_display_list(mesh.displayList), TRUE);
        alpha.textureReuseEnvironment = FALSE;
        head = geo_draw_texture_reuse_display_list(head, &alpha, &previous, &reuse);
        geo_draw_texture_reuse_display_list(head, &mesh, &previous, &reuse);
        EXPECT_EQ(topCommands[4].ptr == mesh.displayList, TRUE);
    }

    begin(0);
    loaded[MODEL_WATER_SPLASH] = &bubbleGeo.node;
    objects[0].header.gfx.sharedChild = &bubbleGeo.node;
    gCurGraphNodeObject = &objects[0].header.gfx;
    geo_append_display_list((void *) water_splash_seg4_dl_04032640, LAYER_TRANSPARENT);
    EXPECT_EQ(master.listTails[LAYER_TRANSPARENT]->textureReuse, TRUE);
    // A different actor bank may use the same segment/address. Its owner is ineligible.
    objects[0].header.gfx.sharedChild = &fishGeo;
    geo_append_display_list((void *) water_splash_seg4_dl_04032640, LAYER_TRANSPARENT);
    EXPECT_EQ(master.listTails[LAYER_TRANSPARENT]->textureReuse, FALSE);
    objects[0].header.gfx.sharedChild = &bubbleGeo.node;
    struct GraphNodeHeldObject held = { 0 };
    gCurGraphNodeHeldObject = &held;
    EXPECT_EQ(geo_object_allows_texture_reuse(), FALSE);
    gCurGraphNodeHeldObject = NULL;
    gCurGraphNodeObject = NULL;
    EXPECT_EQ(geo_object_allows_texture_reuse(), FALSE);
    return TRUE;
}

static int shadow_texture_runs(void) {
    begin(1);
    Gfx shadows[8][4] = { 0 };
    const s32 layer = LAYER_TRANSPARENT_DECAL;
    for (s32 i = 0; i < 8; i++) {
        gSPDisplayList(&shadows[i][0], dl_shadow_circle);
        gDPSetEnvColor(&shadows[i][1], 255, 255, 255, 40 + i);
        gSPDisplayList(&shadows[i][2], shadowDl);
        gSPEndDisplayList(&shadows[i][3]);
    }
    geo_append_shadow_display_list(shadows[0], layer, SHADOW_CIRCLE);
    geo_append_shadow_display_list(shadows[1], layer, SHADOW_CIRCLE);
    EXPECT_EQ(shadows[0][0].ptr == dl_shadow_circle, TRUE);
    EXPECT_EQ(shadows[1][0].ptr == dl_shadow_begin, TRUE);
    EXPECT_EQ(shadows[1][1].n, 41);
    EXPECT_EQ(shadows[1][2].ptr == shadowDl, TRUE);
    EXPECT_EQ(shadows[1][3].op, CMD_END);

    // Other layers do not interrupt the order within this layer.
    geo_append_display_list((void *) otherDl, LAYER_OPAQUE);
    geo_append_shadow_display_list(shadows[2], layer, SHADOW_CIRCLE);
    EXPECT_EQ(shadows[2][0].ptr == dl_shadow_begin, TRUE);
    geo_append_display_list((void *) otherDl, layer);
    geo_append_shadow_display_list(shadows[3], layer, SHADOW_CIRCLE);
    EXPECT_EQ(shadows[3][0].ptr == dl_shadow_circle, TRUE);

    gSPDisplayList(&shadows[4][0], dl_shadow_square);
    geo_append_shadow_display_list(shadows[4], layer, SHADOW_CIRCLE + 1);
    EXPECT_EQ(shadows[4][0].ptr == dl_shadow_square, TRUE);
    geo_append_shadow_display_list(shadows[5], layer, SHADOW_CIRCLE);
    EXPECT_EQ(shadows[5][0].ptr == dl_shadow_circle, TRUE);

    gCurGraphNodeObject = &objects[0].header.gfx;
    objects[0].activeFlags = ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_BLINKING;
    geo_append_shadow_display_list(shadows[6], layer, SHADOW_CIRCLE);
    EXPECT_EQ(shadows[6][0].ptr == dl_shadow_circle, TRUE);
    objects[0].activeFlags = ACTIVE_FLAG_ACTIVE;
    geo_append_shadow_display_list(shadows[7], layer, SHADOW_CIRCLE);
    EXPECT_EQ(shadows[7][0].ptr == dl_shadow_circle, TRUE);
    EXPECT_EQ(master.listHeads[layer]->next->displayList == shadows[1], TRUE);
    EXPECT_EQ(master.listTails[layer]->displayList == shadows[7], TRUE);

    // A new master-list traversal resets reused arena pointers from the old frame.
    master.node.children = &group;
    gCurGraphNodeMasterList = NULL;
    programCount = 0;
    geo_process_master_list(&master);
    EXPECT_EQ(sShadowTextureRuns[layer].node == NULL, TRUE);
    EXPECT_EQ(sShadowTextureRuns[layer].material == NULL, TRUE);
    return TRUE;
}

static int blink_runs(void) {
    const s32 counts[] = { 1, 2, 9 };
    for (u32 pass = 0; pass < sizeof(counts) / sizeof(counts[0]); pass++) {
        for (s32 ordinaryFollows = 0; ordinaryFollows < 2; ordinaryFollows++) {
            struct DisplayListNode lists[10] = { 0 };
            Gfx commands[100], models[9];
            Gfx *head = commands;
            s32 count = counts[pass];
            for (s32 i = 0; i < count; i++) {
                lists[i].displayList = &models[i];
                lists[i].blinkAlpha = i & 1 ? 23 : 51;
                lists[i].blinkOpaque = i & 1;
                lists[i].next = i + 1 < count || ordinaryFollows ? &lists[i + 1] : NULL;
            }
            for (s32 i = 0; i < count; i++) {
                Gfx *start = head;
                head = geo_draw_blinking_display_list(head, &lists[i], TRUE, 0x12340000, 0x23450000);
                EXPECT_EQ(start[0].op, CMD_PIPE_SYNC);
                EXPECT_EQ(start[1].op, CMD_RENDER_MODE);
                EXPECT_EQ(start[1].indices[0] & Z_UPD, 0);
                EXPECT_EQ(start[1].indices[0] & CVG_X_ALPHA, lists[i].blinkOpaque ? 0 : CVG_X_ALPHA);
                EXPECT_EQ(start[4].n, lists[i].blinkOpaque ? G_AC_NONE : G_AC_THRESHOLD);
                EXPECT_EQ(start[2].n, lists[i].blinkAlpha);
                EXPECT_EQ(start[5].ptr, &models[i]);
            }
            EXPECT_EQ(head - commands, count * 9 + 3);
            EXPECT_EQ(head[-3].op, CMD_PIPE_SYNC);
            EXPECT_EQ(head[-2].op, CMD_FOG_COLOR);
            EXPECT_EQ(head[-2].n, 255);
            EXPECT_EQ(head[-1].indices[0], 0x12340000);
            EXPECT_EQ(head[-1].indices[1], 0x23450000);
        }
    }
    return TRUE;
}

static int draw_order(void) {
    begin(3);
    geo_process_object(&objects[0]);
    geo_process_object(&objects[1]);
    geo_append_display_list((void *) otherDl, LAYER_OCCLUDE_SILHOUETTE_ALPHA);
    EXPECT_EQ(sBubbleBatch.count, 0);
    geo_process_object(&objects[2]);
    geo_flush_bubble_batch();
    struct DisplayListNode *a = master.listHeads[LAYER_OCCLUDE_SILHOUETTE_ALPHA];
    EXPECT_EQ(a->displayList != otherDl, TRUE);
    EXPECT_EQ(a->next->displayList == otherDl, TRUE);
    EXPECT_EQ(a->next->next->displayList != otherDl, TRUE);
    EXPECT_EQ(a->next->next->next == NULL, TRUE);
    EXPECT_EQ(((Gfx *) a->displayList)[2].op, CMD_QUAD);
    EXPECT_EQ(((Gfx *) a->displayList)[3].op, CMD_QUAD);
    EXPECT_EQ(((Gfx *) a->displayList)[4].ptr == white_particle_small_dl_end + 1, TRUE);
    return TRUE;
}
static int culling_edges(void) {
    begin(1);
    struct GraphNodeObject *node = &objects[0].header.gfx;
    for (int depth = -50; depth >= -10000; depth -= 199) {
        for (int dimension = 0; dimension < 2; dimension++) {
            for (int sign = -1; sign <= 1; sign += 2) {
                for (int wide = 0; wide < 2; wide++) {
                    frustum.halfFovHorizontal = wide ? 1.1f : 0.7f;
                    f32 slope = dimension ? frustum.halfFovVertical : frustum.halfFovHorizontal;
                    vec3f_set(node->scale, 1, 1, 1);
                    vec3f_set(node->cameraToObject, 0, 0, depth);
                    node->cameraToObject[dimension] =
                        sign * (-depth * slope + 300 * sqrtf(1 + slope * slope) - 0.01f);
                    EXPECT_EQ(geo_aquarium_decoration_in_view(node),
                              TRUE); // Sphere still intersects the plane.
                    node->cameraToObject[dimension] =
                        sign * (-depth * slope + 300 * (1 + fabsf(slope)) + 1);
                    EXPECT_EQ(geo_aquarium_decoration_in_view(node), FALSE);
                    node->scale[dimension] = 2;
                    EXPECT_EQ(geo_aquarium_decoration_in_view(node), TRUE);
                }
            }
        }
    }
    vec3f_set(node->cameraToObject, 50000, 50000, -1000);
    camera.rollScreen = 1234;
    EXPECT_EQ(geo_aquarium_decoration_in_view(node), TRUE);
    camera.rollScreen = 0;
    gCurGraphNodeCamFrustum = NULL;
    EXPECT_EQ(geo_aquarium_decoration_in_view(node), TRUE);
    node->cameraToObject[2] = -21000;
    EXPECT_EQ(geo_aquarium_decoration_in_view(node), FALSE);
    gCurGraphNodeCamFrustum = &frustum;
    frustum.halfFovHorizontal = 0.7f;
    const int emulators[] = { EMU_MUPEN, EMU_PROJECT64, EMU_PARALLEL_LAUNCHER, EMU_CONSOLE };
    for (u32 i = 0; i < sizeof(emulators) / sizeof(emulators[0]); i++) {
        gEmulator = emulators[i];
        for (int cycle = 0; cycle < 3; cycle++) {
            sAspectRatio = 4.0f / 3.0f;
            vec3f_set(node->cameraToObject, 1000, 0, -1000);
            EXPECT_EQ(obj_is_in_view(node), TRUE); // Exact horizontal boundary.
            node->cameraToObject[0] = 1001;
            EXPECT_EQ(obj_is_in_view(node), FALSE);
            sAspectRatio = 16.0f / 9.0f;
            EXPECT_EQ(obj_is_in_view(node), emulators[i] != EMU_CONSOLE);
            node->cameraToObject[2] = -21000;
            EXPECT_EQ(obj_is_in_view(node), FALSE); // Depth protection applies in both modes.
        }
    }
    return TRUE;
}
static s32 particle_outside_vertical_view(struct GraphNodeObject *node) {
    s32 cameraPositionReady = FALSE;
    return geo_particle_outside_vertical_view(node, &cameraPositionReady);
}

static int vertical_sprite_switches(void) {
    const s32 models[] = { MODEL_BUBBLE, MODEL_PURPLE_MARBLE, MODEL_SPARKLES, MODEL_EXPLOSION };
    const s16 frameCounts[] = { 1, 1, 12, 9 };
    const f32 radii[] = { 36, 36, 72, 182 };
    for (s32 kind = 0; kind < 4; kind++) {
        begin(1);
        sAspectRatio = 4.0f / 3.0f;
        objects[0].behavior = NULL;
        struct GraphNodeObject *node = &objects[0].header.gfx;
        struct GraphNode start = { .type = GRAPH_NODE_TYPE_START, .flags = GRAPH_RENDER_ACTIVE };
        struct GraphNodeSwitchCase sw = { 0 };
        struct GraphNodeDisplayList frames[12] = { 0 };
        sw.fnNode.node.type = GRAPH_NODE_TYPE_SWITCH_CASE;
        sw.fnNode.node.flags = GRAPH_RENDER_ACTIVE;
        sw.fnNode.node.next = &sw.fnNode.node;
        sw.fnNode.node.children = &frames[0].node;
        sw.fnNode.func = geo_switch_anim_state;
        sw.numCases = frameCounts[kind];
        for (s32 i = 0; i < sw.numCases; i++) {
            frames[i].node.type = GRAPH_NODE_TYPE_DISPLAY_LIST;
            frames[i].node.flags = GRAPH_RENDER_ACTIVE;
            SET_GRAPH_NODE_LAYER(frames[i].node.flags, LAYER_TRANSPARENT);
            frames[i].node.next = &frames[(i + 1) % sw.numCases].node;
            frames[i].displayList = (void *) otherDl;
        }
        start.next = &start;
        start.children = &sw.fnNode.node;
        node->sharedChild = kind == 3 ? &start : &sw.fnNode.node;
        loaded[models[kind]] = node->sharedChild;
        EXPECT_EQ(verticalSwitchRadii[kind] <= radii[kind], TRUE);
        for (s32 frame = 0; frame < sw.numCases; frame++) {
            objects[0].oAnimState = frame;
            for (s32 scale = 1; scale <= 20; scale += 19) {
                vec3f_set(node->scale, -scale, scale, 1);
                for (s32 sign = -1; sign <= 1; sign += 2) {
                    f32 edge = 500 + MAX(300, radii[kind] * scale) * 1.5f;
                    vec3f_set(node->pos, 0, sign * edge, -1000);
                    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
                    node->pos[1] += sign;
                    sw.selectedCase = -1;
                    int draws = nativeDraws;
                    geo_process_object(&objects[0]);
                    EXPECT_EQ(nativeDraws, draws);
                    EXPECT_EQ(objects[0].oAnimState, frame);
                    EXPECT_EQ(sw.selectedCase, frame);
                    EXPECT_EQ(node->throwMatrix == NULL && gMatStackIndex == 0, TRUE);
                    node->pos[1] = 0;
                    geo_process_object(&objects[0]);
                    EXPECT_EQ(nativeDraws, draws + 1);
                }
            }
        }
        vec3f_set(node->pos, 0, 10000, -1000);
        vec3f_set(node->scale, 1, 1, 1);
        objects[0].oAnimState = sw.numCases;
        EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
        geo_process_object(&objects[0]);
        EXPECT_EQ(objects[0].oAnimState, 0);
        EXPECT_EQ(sw.selectedCase, 0);
        objects[0].oAnimState = -1;
        EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
        objects[0].oAnimState = 0;
        loaded[models[kind]] = NULL;
        EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
        loaded[models[kind]] = node->sharedChild;
        sw.fnNode.func = NULL;
        EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
        sw.fnNode.func = geo_switch_anim_state;
        camera.rollScreen = 1;
        EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
        camera.rollScreen = 0;
        node->node.children = &frames[0].node;
        EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
        node->node.children = NULL;
        gMatStack[0][3][1] = -10000;
        EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    }
    return TRUE;
}

static int mario_shadow_distance_bias(void) {
    const f32 depths[] = { 500, 1000, 2500, 4000, 8000 };
    const f32 offsets[] = { 2, 2, 4, 6, 6 };
    for (s32 mirror = 0; mirror < 2; mirror++) {
        for (u32 i = 0; i < ARRAY_COUNT(depths); i++) {
            begin(1);
            gMarioObject = &objects[0];
            objects[0].header.gfx.animInfo.animID = MARIO_ANIM_WALKING;
            gCurGraphNodeObject = mirror ? &gMirrorMario : &objects[0].header.gfx;
            nativeShadowFloor.normal.x = 0.6f;
            nativeShadowFloor.normal.y = 0.8f;
            Vec3f position = { 0, 400, -depths[i] };
            EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) != NULL,
                      TRUE);
            EXPECT_EQ(absf(position[0] - 0.6f * offsets[i]) < 0.001f, TRUE);
            EXPECT_EQ(absf(position[1] - 0.8f * offsets[i]) < 0.001f, TRUE);
            EXPECT_EQ(position[2], -depths[i]);
        }
    }
    // Cached and queried player floors must both reject invisible death surfaces.
    for (s32 mirror = 0; mirror < 2; mirror++) {
        for (s32 cached = 0; cached < 2; cached++) {
            begin(1);
            gMarioObject = &objects[0];
            objects[0].header.gfx.animInfo.animID = MARIO_ANIM_WALKING;
            gCurGraphNodeObject = mirror ? &gMirrorMario : &objects[0].header.gfx;
            nativeShadowFloor.type = SURFACE_DEATH_PLANE;
            gMarioState->floor = cached ? &nativeShadowFloor : NULL;
            Vec3f position = { 0, 400, -1000 };
            s32 allocationsBefore = allocationCount;
            EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) == NULL,
                      TRUE);
            EXPECT_EQ(allocationCount, allocationsBefore);
            // Water above the plane is a visible receiving surface, including environment boxes.
            nativeWaterHeight = 30;
            for (s32 waterSurface = 0; waterSurface < 2; waterSurface++) {
                struct Surface water = { .type = SURFACE_DEFAULT, .normal = { 0, 1, 0 } };
                nativeWaterFloor = waterSurface ? &water : NULL;
                vec3f_set(position, 0, 400, -1000);
                EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE)
                              != NULL,
                          TRUE);
                EXPECT_EQ(position[1], 32);
            }
        }
    }
    begin(1);
    gMarioObject = &objects[0];
    objects[0].header.gfx.animInfo.animID = MARIO_ANIM_WALKING;
    gCurGraphNodeObject = &objects[0].header.gfx;
    gCurGraphNodeCamera = NULL;
    Vec3f position = { 0, 400, -8000 };
    EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) != NULL, TRUE);
    EXPECT_EQ(position[1], 2);
    begin(1);
    gCurGraphNodeObject = &objects[0].header.gfx;
    vec3f_set(position, 0, 400, -8000);
    EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) != NULL, TRUE);
    EXPECT_EQ(position[1], 0);
    // The player-only exclusion does not change ordinary or held-object shadows.
    nativeShadowFloor.type = SURFACE_DEATH_PLANE;
    vec3f_set(position, 0, 400, -1000);
    EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) != NULL, TRUE);
    gMarioObject = &objects[0];
    objects[0].header.gfx.animInfo.animID = MARIO_ANIM_WALKING;
    struct GraphNodeHeldObject held = { 0 };
    gCurGraphNodeHeldObject = &held;
    vec3f_set(position, 0, 400, -1000);
    EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) != NULL, TRUE);
    gCurGraphNodeHeldObject = NULL;
    return TRUE;
}

static int shadow_footprint_culling(void) {
    begin(1);
    sAspectRatio = 4.0f / 3.0f;
    objects[0].behavior = NULL;
    gCurGraphNodeObject = &objects[0].header.gfx;
    Vec3f position = { 0, 900, -1000 };
    // An offscreen body still has a visible ground footprint.
    Gfx *list = real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE);
    EXPECT_EQ(list != NULL, TRUE);
    EXPECT_EQ(position[1], 0);
    EXPECT_EQ(nativeFloorQueries, 1);
    EXPECT_EQ(nativeWaterQueries, 1);
    EXPECT_EQ(list[1].n, gCurrShadow.solidity);
    EXPECT_EQ(list[2].ptr == dl_shadow_end, TRUE);
    // An onscreen body can have a wholly offscreen shadow below it.
    nativeFloorHeight = -1000;
    int allocationsBefore = allocationCount;
    vec3f_set(position, 0, 0, -1000);
    EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) == NULL, TRUE);
    EXPECT_EQ(allocationCount, allocationsBefore);
    EXPECT_EQ(nativeFloorQueries, 2);
    EXPECT_EQ(nativeWaterQueries, 2);
    EXPECT_EQ(position[1], 0); // Rejection retains the ordinary failure contract.

    // Every actual quad corner is inside the rounded rotation-independent radius.
    for (u32 i = 0; i < sizeof(shadowFootprintVertices) / sizeof(shadowFootprintVertices[0]); i++) {
        f32 norm = 0;
        for (s32 axis = 0; axis < 3; axis++) {
            norm += shadowFootprintVertices[i][axis] * shadowFootprintVertices[i][axis];
        }
        EXPECT_EQ(sqrtf(norm) <= 1.5f, TRUE);
    }
    for (s32 sign = -1; sign <= 1; sign += 2) {
        f32 edge = 500 + (100 * 1.5f + 2) * 1.5f;
        nativeFloorHeight = sign * edge;
        vec3f_set(position, 0, nativeFloorHeight, -1000);
        EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) != NULL, TRUE);
        nativeFloorHeight += sign;
        vec3f_set(position, 0, nativeFloorHeight, -1000);
        EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) == NULL, TRUE);
    }
    // Cached, shifted sloped floors use the resolved plane height.
    objects[0].oFloor = &nativeShadowFloor;
    objects[0].oFloorHeight = 0;
    nativeShadowFloor.normal.x = 0.6f;
    nativeShadowFloor.normal.y = 0.8f;
    nativeShadowFloor.originOffset = 800;
    vec3f_set(position, 0, 0, -1000);
    EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_SQUARE, TRUE) == NULL, TRUE);
    nativeShadowFloor.originOffset = 0;
    vec3f_set(position, 0, 900, -1000);
    EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_SQUARE, TRUE) != NULL, TRUE);
    EXPECT_EQ(position[1], 0);
    nativeShadowFloor.normal.x = 0;
    nativeShadowFloor.normal.y = 1;
    objects[0].oFloorHeight = -1000;
    // Environment-box water replaces the offscreen floor with visible water.
    nativeWaterHeight = 0;
    vec3f_set(position, 0, 0, -1000);
    EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) != NULL, TRUE);
    EXPECT_EQ(gCurrShadow.isDecal, FALSE);
    nativeWaterFloor = &nativeShadowFloor;
    vec3f_set(position, 0, 0, -1000);
    EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) != NULL, TRUE);
    nativeWaterHeight = FLOOR_LOWER_LIMIT_MISC;
    nativeWaterFloor = NULL;
    nativeShadowFloor.type = SURFACE_BURNING;
    gCurrLevelNum = LEVEL_LLL;
    gCurrAreaIndex = 1;
    vec3f_set(position, 0, 100, -1000);
    EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) != NULL, TRUE);
    EXPECT_EQ(position[1], 5);
    gCurrLevelNum = 0;
    nativeShadowFloor.type = SURFACE_ICE;
    vec3f_set(position, 0, 0, -1000);
    EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) == NULL, TRUE);
    EXPECT_EQ(gCurrShadow.isDecal, FALSE);
    nativeShadowFloor.type = SURFACE_DEFAULT;
    nativeShadowFloor.object = &objects[1];
    objects[1].behavior = bhvPlatformOnTrack;
    objects[1].oPlatformOnTrackType = PLATFORM_ON_TRACK_TYPE_CARPET;
    objects[0].oFloorHeight = -5;
    vec3f_set(position, 0, 0, -1000);
    EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE) != NULL, TRUE);
    EXPECT_EQ(position[1], 0);
    nativeShadowFloor.object = NULL;
    objects[0].oFloorHeight = -1000;
    nativeFloorHeight = -1000;
    // Roll, emulator aspect policy, debug views and unusual types keep fallback.
    objects[0].header.gfx.animInfo.animID = MARIO_ANIM_WALKING;
    for (s32 fallbackKind = 0; fallbackKind < 7; fallbackKind++) {
        camera.rollScreen = fallbackKind == 0;
        sAspectRatio = fallbackKind == 1 ? 16.0f / 9.0f : 4.0f / 3.0f;
        hitboxView = fallbackKind == 2;
        struct GraphNodeHeldObject held = { 0 };
        gCurGraphNodeHeldObject = fallbackKind == 3 ? &held : NULL;
        gMarioObject = fallbackKind == 4 ? &objects[0] : NULL;
        gCurGraphNodeObject = fallbackKind == 5 ? &gMirrorMario : &objects[0].header.gfx;
        vec3f_set(position, 0, 0, -1000);
        s8 type = fallbackKind == 6 ? SHADOW_RECTANGLE_WHOMP : SHADOW_CIRCLE;
        EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, type, FALSE) != NULL, TRUE);
    }
    gCurGraphNodeObject = &objects[0].header.gfx;
    gCurGraphNodeHeldObject = NULL;
    gCurGraphNodeCamFrustum = NULL;
    vec3f_set(position, 0, 0, -1000);
    EXPECT_EQ(real_create_shadow_below_xyz(position, 100, 180, SHADOW_SQUARE_PERMANENT, FALSE) != NULL,
              TRUE);
    return TRUE;
}

static int shadow_culling_child_state(void) {
    for (s32 outside = 0; outside < 2; outside++) {
        begin(1);
        sAspectRatio = 4.0f / 3.0f;
        useNativeShadow = TRUE;
        objects[0].behavior = NULL;
        objects[0].header.gfx.sharedChild = &objectShadowGeo.node;
        objects[0].oFloor = &nativeShadowFloor;
        objects[0].oFloorHeight = outside ? -1000 : 0;
        vec3f_set(objects[0].header.gfx.pos, 0, outside ? 0 : 900, -1000);
        objectShadowGeo.node.type = GRAPH_NODE_TYPE_SHADOW;
        objectShadowGeo.node.flags = GRAPH_RENDER_ACTIVE;
        objectShadowGeo.node.next = &objectShadowGeo.node;
        objectShadowGeo.node.children = &bubbleGeo.node;
        objectShadowGeo.shadowType = SHADOW_CIRCLE;
        objectShadowGeo.shadowScale = 100;
        objectShadowGeo.shadowSolidity = 180;
        geo_process_object(&objects[0]);
        EXPECT_EQ(shadowQueries, 1);
        EXPECT_EQ(nativeWaterQueries, 1);
        EXPECT_EQ(shadowMatrices, outside ? 0 : 1);
        EXPECT_EQ(nativeDraws, 1); // Body children survive shadow-only rejection.
        EXPECT_EQ(gCurGraphNodeObject == NULL, TRUE);
        EXPECT_EQ(gMatStackIndex, 0);
        EXPECT_EQ(gCurrAnimType, ANIM_TYPE_NONE);
        EXPECT_EQ(objects[0].header.gfx.throwMatrix == NULL, TRUE);
        objectShadowGeo.node.children = NULL;
    }
    return TRUE;
}

static int vertical_trees(void) {
    const Gfx *opaque[] = { tree_seg3_dl_bubbly, tree_seg3_dl_spiky, tree_seg3_dl_snowy_pine,
                            tree_seg3_dl_palm };
    const Gfx *faded[] = { tree_seg3_dl_bubbly_transparent, tree_seg3_dl_spiky_transparent,
                           tree_seg3_dl_snowy_pine_transparent, tree_seg3_dl_palm_transparent };
    const s32 opacities[] = { 0, 96, 255 };
    for (s32 kind = 0; kind < 4; kind++) {
        for (s32 opacity = 0; opacity < 3; opacity++) {
            begin(1);
            sAspectRatio = 4.0f / 3.0f;
            objects[0].behavior = NULL;
            objects[0].oOpacity = opacities[opacity];
            objects[0].oAnimState = 77;
            objects[0].activeFlags |= ACTIVE_FLAG_DITHERED_ALPHA | ACTIVE_FLAG_BLINKING;
            struct GraphNodeObject *obj = &objects[0].header.gfx;
            struct GraphNodeCullingRadius tree = { 0 };
            struct GraphNodeGenerated gen = { 0 };
            struct GraphNodeSwitchCase sw = { 0 };
            struct GraphNodeDisplayList frames[2] = { 0 };
            tree.node.type = GRAPH_NODE_TYPE_CULLING_RADIUS;
            tree.node.flags = GRAPH_RENDER_ACTIVE;
            tree.node.next = &tree.node;
            tree.cullingRadius = 800;
            tree.node.children = &gen.fnNode.node;
            gen.fnNode.node.type = GRAPH_NODE_TYPE_GENERATED_LIST;
            gen.fnNode.node.flags = GRAPH_RENDER_ACTIVE;
            gen.fnNode.node.next = &sw.fnNode.node;
            gen.fnNode.func = geo_update_layer_transparency;
            gen.parameter = GEO_TRANSPARENCY_MODE_INTER;
            sw.fnNode.node.type = GRAPH_NODE_TYPE_SWITCH_CASE;
            sw.fnNode.node.flags = GRAPH_RENDER_ACTIVE;
            sw.fnNode.node.next = &gen.fnNode.node;
            sw.fnNode.node.children = &frames[0].node;
            sw.fnNode.func = geo_switch_anim_state;
            sw.numCases = 2;
            for (s32 frame = 0; frame < 2; frame++) {
                frames[frame].node.type = GRAPH_NODE_TYPE_DISPLAY_LIST;
                frames[frame].node.flags = GRAPH_RENDER_ACTIVE;
                SET_GRAPH_NODE_LAYER(frames[frame].node.flags,
                                     frame == 0 ? LAYER_ALPHA : LAYER_TRANSPARENT_INTER);
                frames[frame].node.next = &frames[1 - frame].node;
                frames[frame].displayList = (void *) (frame == 0 ? opaque[kind] : faded[kind]);
            }
            obj->sharedChild = &tree.node;
            EXPECT_EQ(verticalTreeRadii[kind] <= 800, TRUE);
            for (s32 scale = 1; scale <= 3; scale += 2) {
                vec3f_set(obj->scale, -scale, scale, 1);
                for (s32 sign = -1; sign <= 1; sign += 2) {
                    f32 edge = 500 + 800 * scale * 1.5f;
                    vec3f_set(obj->pos, 0, sign * edge, -1000);
                    EXPECT_EQ(particle_outside_vertical_view(obj), FALSE);
                    obj->pos[1] += sign;
                    objects[0].oAnimState = 77;
                    sw.selectedCase = -1;
                    int draws = nativeDraws;
                    geo_process_object(&objects[0]);
                    EXPECT_EQ(nativeDraws, draws);
                    EXPECT_EQ(objects[0].oAnimState, opacity == 2 ? 0 : 1);
                    EXPECT_EQ(sw.selectedCase, objects[0].oAnimState);
                    EXPECT_EQ(GET_GRAPH_NODE_LAYER(gen.fnNode.node.flags),
                              opacity == 2 ? LAYER_OPAQUE : LAYER_TRANSPARENT_INTER);
                    EXPECT_EQ(gCurGraphNodeObject == NULL, TRUE);
                    EXPECT_EQ(obj->throwMatrix == NULL, TRUE);
                    EXPECT_EQ(gMatStackIndex, 0);
                    vec3f_set(obj->pos, 0, 0, -1000);
                    geo_process_object(&objects[0]);
                    EXPECT_EQ(nativeDraws, draws + 1);
                }
            }
            vec3f_set(obj->pos, 10000, 10000, -1000);
            objects[0].oAnimState = 77;
            geo_process_object(&objects[0]);
            EXPECT_EQ(objects[0].oAnimState, 77); // Native horizontal rejection skips callbacks.
            obj->pos[0] = 0;
            gen.fnNode.func = NULL;
            EXPECT_EQ(particle_outside_vertical_view(obj), FALSE);
            gen.fnNode.func = geo_update_layer_transparency;
            frames[1].displayList = (void *) otherDl;
            EXPECT_EQ(particle_outside_vertical_view(obj), FALSE);
        }
    }
    return TRUE;
}

static int particle_camera_position_reuse(void) {
    for (s32 enabled = 0; enabled < 2; enabled++) {
        begin(1);
        objects[0].behavior = NULL;
        bubbleGeo.displayList = (void *) pebble_seg3_dl_0301CB00;
        sAspectRatio = 4.0f / 3.0f;
        if (!enabled) {
            camera.rollScreen = 1;
        }
        vec3f_set(objects[0].header.gfx.pos, 10, 20, -1000);
        vec3f_set(gMatStack[0][3], 30, 70, 0);
        geo_process_object(&objects[0]);
        EXPECT_EQ(nativeDraws, 1);
        EXPECT_EQ(gPuppyCallCounter.matrix, 3);
        EXPECT_EQ(objects[0].header.gfx.cameraToObject[0], 40);
        EXPECT_EQ(objects[0].header.gfx.cameraToObject[1], 90);
        EXPECT_EQ(objects[0].header.gfx.cameraToObject[2], -1000);
        EXPECT_EQ(objects[0].header.gfx.throwMatrix == NULL, TRUE);
        EXPECT_EQ(gMatStackIndex, 0);
    }
    return TRUE;
}

static int vertical_particles(void) {
    begin(1);
    struct GraphNodeObject *node = &objects[0].header.gfx;
    objects[0].behavior = NULL;
    sAspectRatio = 4.0f / 3.0f;
    const Gfx *models[] = {
        white_particle_small_dl, white_particle_small_unused_dl,
        lava_droplet_dl,         cold_water_droplet_dl,
        white_particle_dl,       sand_seg3_dl_particle,
        pebble_seg3_dl_0301CB00,
    };
    // Check the bound against current asset vertices, rather than mock geometry.
    for (u32 i = 0; i < sizeof(verticalParticleVertices) / sizeof(verticalParticleVertices[0]); i++) {
        const f32 *v = verticalParticleVertices[i];
        EXPECT_EQ(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] <= 32 * 32, TRUE);
    }
    for (u32 i = 0; i < sizeof(models) / sizeof(models[0]); i++) {
        bubbleGeo.displayList = (void *) models[i];
        for (int sign = -1; sign <= 1; sign += 2) {
            for (int scale = 1; scale <= 20; scale += 19) {
                vec3f_set(node->scale, -scale, scale, 1);
                f32 edge = 500 + MAX(300, 32 * scale) * 1.5f;
                vec3f_set(node->pos, 0, sign * edge, -1000);
                EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
                node->pos[1] += sign;
                EXPECT_EQ(particle_outside_vertical_view(node), TRUE);
            }
        }
    }
    bubbleGeo.displayList = (void *) pebble_seg3_dl_0301CB00;
    vec3f_set(node->scale, 1, 1, 1);
    // Repeated departures and returns preserve sound coordinates and cleanup.
    for (int cycle = 0; cycle < 3; cycle++) {
        vec3f_set(node->pos, 0, cycle % 2 ? -1000 : 1000, -1000);
        int matrices = gPuppyCallCounter.matrix;
        geo_process_object(&objects[0]);
        EXPECT_EQ(nativeDraws, cycle);
        EXPECT_EQ(gPuppyCallCounter.matrix, matrices + 1); // Sound-space origin only.
        EXPECT_EQ(node->cameraToObject[1], node->pos[1]);
        EXPECT_EQ(node->throwMatrix == NULL && gMatStackIndex == 0, TRUE);
        EXPECT_EQ(objects[0].timer, 17);
        node->pos[1] = 0;
        geo_process_object(&objects[0]);
        EXPECT_EQ(nativeDraws, cycle + 1);
    }
    vec3f_set(node->pos, 0, 1000, -1000);
    gMatStack[0][3][1] = -1000;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    gMatStack[0][3][1] = 0;
    mtxf_translate(gCameraTransform, (Vec3f){ 0, -1000, 0 });
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    mtxf_identity(gCameraTransform);

    // Unknown layouts and anything capable of drawing a separate shadow stay live.
    bubbleGeo.node.type = GRAPH_NODE_TYPE_SHADOW;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    bubbleGeo.node.type = GRAPH_NODE_TYPE_DISPLAY_LIST;
    struct GraphNode child = { 0 };
    node->node.children = &child;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    node->node.children = NULL;
    bubbleGeo.node.children = &child;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    bubbleGeo.node.children = NULL;
    bubbleGeo.node.next = &child;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    bubbleGeo.node.next = &bubbleGeo.node;
    bubbleGeo.displayList = (void *) otherDl;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    bubbleGeo.displayList = (void *) pebble_seg3_dl_0301CB00;
    node->animInfo.curAnim = (struct Animation *) &fishGeo;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    node->animInfo.curAnim = NULL;
    Mat4 custom;
    mtxf_translate(custom, node->pos);
    node->throwMatrix = &custom;
    sCurrPlayMode = PLAY_MODE_PAUSED;
    geo_process_object(&objects[0]);
    EXPECT_EQ(node->throwMatrix == &custom, TRUE);
    EXPECT_EQ(nativeDraws, 4);
    node->throwMatrix = NULL;
    sCurrPlayMode = 0;
    node->node.flags |= GRAPH_RENDER_SILHOUETTE;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    node->node.flags &= ~GRAPH_RENDER_SILHOUETTE;
    node->node.flags &= ~GRAPH_RENDER_BILLBOARD;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    node->node.flags |= GRAPH_RENDER_BILLBOARD;
    hitboxView = 1;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    hitboxView = 0;
    camera.rollScreen = 1;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    camera.rollScreen = 0;
    gCurGraphNodeCamFrustum = NULL;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    gCurGraphNodeCamFrustum = &frustum;
    node->scale[0] = NAN;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    node->scale[0] = 1;
    frustum.halfFovVertical = NAN;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    frustum.halfFovVertical = 0.5f;
    sAspectRatio = 16.0f / 9.0f;
    EXPECT_EQ(particle_outside_vertical_view(node), FALSE);
    gEmulator = EMU_CONSOLE;
    EXPECT_EQ(particle_outside_vertical_view(node), TRUE);
    return TRUE;
}

static int offscreen_state(void) {
    begin(2);
    struct GraphNodeObject *fish = &objects[1].header.gfx;
    objects[1].behavior = &bhvBlueFish;
    fish->sharedChild = &fishGeo;
    fish->node.flags = GRAPH_RENDER_ACTIVE | GRAPH_RENDER_HAS_ANIMATION;
    fish->animInfo.curAnim = (struct Animation *) &fishGeo;
    for (int frame = 0; frame < 70; frame++) {
        objects[0].header.gfx.pos[0] = fish->pos[0] = (frame % 2 ? -1 : 1) * 4000;
        geo_process_object(&objects[0]);
        geo_process_object(&objects[1]);
        EXPECT_EQ(animationCalls, frame + 1);
        EXPECT_EQ(fish->animInfo.animFrame, frame + 1);
        EXPECT_EQ(gMatStackIndex, 0);
        EXPECT_EQ(gCurrAnimType, ANIM_TYPE_NONE);
        EXPECT_EQ(fish->throwMatrix == NULL, TRUE);
        EXPECT_EQ(fish->cameraToObject[0], fish->pos[0]);
        EXPECT_EQ(nativeDraws, 0);
        EXPECT_EQ(gPuppyCallCounter.matrix, 2 * (frame + 1));
        EXPECT_EQ(objects[0].timer, 17);
        EXPECT_EQ(objects[1].timer, 18);
    }
    // Below/above culling works on the emulator too; ordinary/ shadowed objects keep the bypass.
    fish->pos[0] = 0;
    fish->pos[1] = 4000;
    geo_process_object(&objects[1]);
    EXPECT_EQ(nativeDraws, 0);
    fish->pos[1] = -4000;
    geo_process_object(&objects[1]);
    EXPECT_EQ(nativeDraws, 0);
    fishGeo.type = GRAPH_NODE_TYPE_SHADOW;
    EXPECT_EQ(geo_is_aquarium_decoration(&objects[1]), FALSE);
    EXPECT_EQ(obj_is_in_view(fish), TRUE);
    fish->node.flags &= ~GRAPH_RENDER_HAS_ANIMATION;
    fishGeo.type = GRAPH_NODE_TYPE_SCALE;
    geo_process_object(&objects[1]);
    EXPECT_EQ(fish->animInfo.animFrame, 72);
    // Lab rejection must preserve the shared update/cleanup path on every
    // cycle, including sound position, animation advancement and pause.
    objects[1].behavior = NULL;
    fish->node.flags |= GRAPH_RENDER_HAS_ANIMATION;
    gCurrLevelNum = LEVEL_PHYSICS_LAB;
    gEmulator = 0;
    fish->pos[1] = 0;
    int expectedDraws = nativeDraws, expectedFrame = fish->animInfo.animFrame;
    for (int frame = 0; frame < 24; frame++) {
        labObjectVisibility = frame % 3 == 0 ? -1 : 0;
        int matrices = gPuppyCallCounter.matrix, queries = labVisibilityCalls;
        geo_process_object(&objects[1]);
        EXPECT_EQ(labVisibilityCalls, queries + 1); // No repeated late check.
        // Sound-space translation remains; rotation and scale matrices disappear.
        if (!labObjectVisibility) {
            EXPECT_EQ(gPuppyCallCounter.matrix, matrices + 1);
        }
        expectedDraws += labObjectVisibility != 0;
        expectedFrame++;
        EXPECT_EQ(nativeDraws, expectedDraws);
        EXPECT_EQ(fish->animInfo.animFrame, expectedFrame);
        EXPECT_EQ(gMatStackIndex, 0);
        EXPECT_EQ(gCurrAnimType, ANIM_TYPE_NONE);
        EXPECT_EQ(fish->throwMatrix == NULL, TRUE);
        EXPECT_EQ(fish->cameraToObject[2], fish->pos[2]);
        EXPECT_EQ(objects[1].timer, 18);
    }
    fish->node.flags &= ~GRAPH_RENDER_HAS_ANIMATION;
    /* Ordinary objects outside the Lab use the identical early rejection too.
     * Keep their sound position and animation clock while doing no transform. */
    gCurrLevelNum = 0;
    gEmulator = 0;
    fish->node.flags |= GRAPH_RENDER_HAS_ANIMATION;
    fish->pos[0] = 10000;
    int beforeMatrices = gPuppyCallCounter.matrix;
    int beforeFrame = fish->animInfo.animFrame;
    int beforeDraws = nativeDraws;
    geo_process_object(&objects[1]);
    EXPECT_EQ(gPuppyCallCounter.matrix, beforeMatrices + 1);
    EXPECT_EQ(fish->animInfo.animFrame, beforeFrame + 1);
    expectedFrame++;
    EXPECT_EQ(nativeDraws, beforeDraws);
    EXPECT_EQ(fish->cameraToObject[0], fish->pos[0]);
    EXPECT_EQ(gMatStackIndex, 0);
    EXPECT_EQ(fish->throwMatrix == NULL, TRUE);
    fish->pos[0] = 0;
    gCurrLevelNum = LEVEL_PHYSICS_LAB;
    fish->node.flags &= ~GRAPH_RENDER_HAS_ANIMATION;
    labObjectVisibility = 0;
    sCurrPlayMode = PLAY_MODE_PAUSED;
    Mat4 paused;
    mtxf_identity(paused);
    paused[3][2] = -1000;
    fish->throwMatrix = &paused;
    geo_process_object(&objects[1]);
    EXPECT_EQ(fish->throwMatrix == &paused, TRUE);
    EXPECT_EQ(fish->animInfo.animFrame, expectedFrame);
    EXPECT_EQ(nativeDraws, expectedDraws);
    sCurrPlayMode = 0;
    gCurrLevelNum = 0;
    labObjectVisibility = -1;
    return TRUE;
}
static int fallback(void) {
    begin(1);
    struct GraphNodeObject *node = &objects[0].header.gfx;
    struct GraphNode child = { 0 };
    node->node.children = &child;
    EXPECT_EQ(geo_is_simple_small_sprite(&objects[0]), FALSE);
    node->node.children = NULL;
    bubbleGeo.node.children = &child;
    EXPECT_EQ(geo_is_simple_small_sprite(&objects[0]), FALSE);
    bubbleGeo.node.children = NULL;
    node->node.flags |= GRAPH_RENDER_SILHOUETTE;
    EXPECT_EQ(geo_is_simple_small_sprite(&objects[0]), FALSE);
    node->node.flags &= ~GRAPH_RENDER_SILHOUETTE;
    node->areaIndex = 2;
    geo_process_object(&objects[0]);
    EXPECT_EQ(allocationCount, 0);
    node->areaIndex = 1;
    hitboxView = 1;
    geo_process_object(&objects[0]);
    EXPECT_EQ(nativeDraws, 1);
    EXPECT_EQ(debugDraws, 1);
    begin(1);
    node = &objects[0].header.gfx;
    failNextAllocation = TRUE;
    geo_process_object(&objects[0]);
    EXPECT_EQ(nativeDraws, 1);
    EXPECT_EQ(sBubbleBatch.count, 0);
    begin(1);
    node = &objects[0].header.gfx;
    node->scale[0] = 10000;
    geo_process_object(&objects[0]);
    EXPECT_EQ(nativeDraws, 1);
    EXPECT_EQ(sBubbleBatch.count, 0);
    begin(1);
    node = &objects[0].header.gfx;
    Mat4 custom;
    mtxf_translate(custom, node->pos);
    node->throwMatrix = &custom;
    sCurrPlayMode = PLAY_MODE_PAUSED;
    geo_process_object(&objects[0]);
    EXPECT_EQ(nativeDraws, 1);
    EXPECT_EQ(node->throwMatrix == &custom, TRUE);
    EXPECT_EQ(gMatStackIndex, 0);
    begin(1);
    node = &objects[0].header.gfx;
    vec3f_set(gMatStack[0][3], 50, 60, 70);
    node->node.flags |= GRAPH_RENDER_INVISIBLE;
    geo_process_object(&objects[0]);
    EXPECT_EQ(nativeDraws, 0);
    EXPECT_EQ(allocationCount, 0);
    for (int axis = 0; axis < 3; axis++) {
        EXPECT_EQ(node->cameraToObject[axis], node->pos[axis]);
    }
    return TRUE;
}
static int distant_rebase(void) {
    begin(2);
    objects[1].header.gfx.pos[0] = 3000.37f;
    for (int i = 0; i < 2; i++) {
        EXPECT_EQ(geo_try_batch_bubble(&objects[i], objects[i].header.gfx.pos), TRUE);
    }
    geo_flush_bubble_batch();
    // Each sprite is on a different reference origin; the second is never lost.
    EXPECT_EQ(inspect(2, 2, TRUE), TRUE);
    return TRUE;
}
static int other_small_sprites_batch(void) {
    static const BehaviorScript otherParticle[] = { 42 };
    begin(3);
    for (int i = 0; i < 3; i++) {
        objects[i].behavior = otherParticle;
    }
    for (int i = 0; i < 3; i++) {
        geo_process_object(&objects[i]);
    }
    geo_flush_bubble_batch();
    EXPECT_EQ(nativeDraws, 0);
    EXPECT_EQ(inspect(3, 1, TRUE), TRUE);
    return TRUE;
}
static int coloured_droplets_batch(void) {
    const Gfx *displayLists[] = { white_particle_small_dl, cold_water_droplet_dl, lava_droplet_dl };
    const int counts[] = { 1, 18, 35 };
    for (u32 pass = 0; pass < sizeof(counts) / sizeof(counts[0]); pass++) {
        for (int pattern = 0; pattern < 4; pattern++) {
            int count = counts[pass];
            begin(count);
            struct GraphNodeDisplayList models[3];
            for (u32 model = 0; model < sizeof(models) / sizeof(models[0]); model++) {
                models[model] = bubbleGeo;
                models[model].node.next = &models[model].node;
                models[model].displayList = (void *) displayLists[model];
            }
            for (int i = 0; i < count; i++) {
                int model = pattern == 3 ? i % 3 : pattern;
                objects[i].header.gfx.sharedChild = &models[model].node;
                geo_process_object(&objects[i]);
            }
            geo_flush_bubble_batch();
            EXPECT_EQ(nativeDraws, 0);
            EXPECT_EQ(inspect(count, (count + 15) / 16, TRUE), TRUE);
        }
    }
    begin(1);
    bubbleGeo.displayList = (void *) cold_water_droplet_dl;
    objects[0].activeFlags = ACTIVE_FLAG_BLINKING;
    geo_process_object(&objects[0]);
    geo_flush_bubble_batch();
    EXPECT_EQ(nativeDraws, 1);
    EXPECT_EQ(sBubbleBatch.count, 0);
    EXPECT_EQ(master.listHeads[LAYER_TRANSPARENT]->displayList == cold_water_droplet_dl, TRUE);
    EXPECT_EQ(master.listHeads[LAYER_TRANSPARENT]->blinkAlpha, OBJECT_BLINK_MIN_ALPHA);
    return TRUE;
}
static int compact_bubbles_batch(void) {
    begin(0);
    compactCount = 3;
    for (int i = 0; i < compactCount; i++) {
        vec3f_set(compactPositions[i], i * 4.0f, i * 5.0f, -1000.0f);
        vec3f_set(compactScales[i], 2.0f, 2.0f, 1.0f);
        compactAreas[i] = 1;
    }
    geo_render_compact_explosion_bubbles();
    geo_flush_bubble_batch();
    EXPECT_EQ(nativeDraws, 0);
    EXPECT_EQ(inspect(3, 1, FALSE), TRUE);
    begin(0);
    compactCount = 1;
    vec3f_set(compactPositions[0], 0, 0, -1000);
    vec3f_set(compactScales[0], 2, 2, 1);
    compactAreas[0] = 2;
    geo_render_compact_explosion_bubbles();
    EXPECT_EQ(sBubbleBatch.count, 0);
    return TRUE;
}
static int pause_and_flush(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(35);
        sCurrPlayMode = PLAY_MODE_PAUSED;
        struct GraphNodeObjectParent parent = { 0 };
        parent.sharedChild = &group;
        geo_process_object_parent(&parent);
        EXPECT_EQ(inspect(35, 3, TRUE), TRUE);
        for (int i = 0; i < 35; i++) {
            EXPECT_EQ(objects[i].header.gfx.throwMatrix == NULL, TRUE);
            EXPECT_EQ(objects[i].timer, 17 + i);
        }
        begin(5);
        master.node.children = &group;
        gCurGraphNodeMasterList = NULL;
        geo_process_master_list(&master);
        EXPECT_EQ(inspect(5, 1, TRUE), TRUE);
        EXPECT_EQ(masterFlushes, 1);
        EXPECT_EQ(gCurGraphNodeMasterList == NULL, TRUE);
    }
    return TRUE;
}

static void use_real_fish(int count) {
    memset(&fishScaleGeo, 0, sizeof(fishScaleGeo));
    memset(fishParts, 0, sizeof(fishParts));
    fishScaleGeo.node.type = GRAPH_NODE_TYPE_SCALE;
    fishScaleGeo.node.flags = GRAPH_RENDER_ACTIVE;
    fishScaleGeo.scale = fishModelScale;
    fishScaleGeo.node.children = &fishParts[0].node;
    for (int i = 0; i < 3; i++) {
        fishParts[i].node.type = GRAPH_NODE_TYPE_ANIMATED_PART;
        fishParts[i].node.flags = GRAPH_RENDER_ACTIVE;
        SET_GRAPH_NODE_LAYER(fishParts[i].node.flags, LAYER_OCCLUDE_SILHOUETTE_OPAQUE);
        vec3s_copy(fishParts[i].translation, fishModelOffsets[i]);
        fishParts[i].node.children = i < 2 ? &fishParts[i + 1].node : NULL;
        fishParts[i].displayList = i == 0 ? NULL : (void *) fishDl;
    }
    loaded[MODEL_FISH] = &fishScaleGeo.node;
    for (int i = 0; i < count; i++) {
        objects[i].behavior = bhvBlueFish;
        objects[i].header.gfx.sharedChild = &fishScaleGeo.node;
        objects[i].header.gfx.node.flags = GRAPH_RENDER_ACTIVE | GRAPH_RENDER_HAS_ANIMATION;
        objects[i].header.gfx.animInfo.curAnim = (struct Animation *) &blue_fish_seg3_anim_0301C298;
    }
}

static int fixed_matrix_count(void) {
    int count = 0;
    for (int i = 0; i < allocationCount; i++) {
        count += allocationSizes[i] == sizeof(Mtx);
    }
    return count;
}

static int visible_fish_matrices(void) {
    // Execute every frame of the actual animation with the actual geolayout's
    // scale and bone offsets, over repeated cycles and nonuniform object scales.
    // Build an independent eager reference from those inputs; compare every
    // fixed-matrix byte submitted for the body/tail, not just a visual tolerance.
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int frame = 0; frame < blue_fish_seg3_anim_0301C298.loopEnd; frame++) {
            begin(60);
            use_real_fish(60);
            for (int i = 0; i < 60; i++) {
                struct GraphNodeObject *fish = &objects[i].header.gfx;
                fish->angle[0] = frame * 131;
                fish->angle[1] = i * 1091;
                fish->angle[2] = cycle * 997;
                fish->scale[0] = 0.7f + i * 0.013f;
                fish->scale[1] = 0.6f + (i % 7) * 0.08f;
                fish->scale[2] = (cycle % 2 ? -1 : 1) * 1.17f;
                fish->animInfo.animFrame = (frame + i) % blue_fish_seg3_anim_0301C298.loopEnd;
                fish->animInfo.animTimer =
                    gAreaUpdateCounter; // Hold the requested pose for comparison.
                geo_process_object(&objects[i]);
            }
            EXPECT_EQ(fixed_matrix_count(), 120); // Two draws, versus five matrices per fish before.
            EXPECT_EQ(animationCalls, 60);
            EXPECT_EQ(gMatStackIndex, 0);
            EXPECT_EQ(gCurrAnimType, ANIM_TYPE_NONE);
            struct DisplayListNode *draw = master.listHeads[LAYER_OCCLUDE_SILHOUETTE_OPAQUE];
            for (int i = 0; i < 60; i++) {
                struct GraphNodeObject *fish = &objects[i].header.gfx;
                Mat4 matrix[5];
                Vec3f modelScale = { fishModelScale, fishModelScale, fishModelScale };
                mtxf_rotate_zxy_and_translate(matrix[0], fish->pos, fish->angle);
                mtxf_scale_vec3f(matrix[0], matrix[0], fish->scale);
                mtxf_scale_vec3f(matrix[1], matrix[0], modelScale);
                u16 *attribute = (u16 *) blue_fish_seg3_anim_0301C298.index;
                const s16 *values = blue_fish_seg3_anim_0301C298.values;
                for (int bone = 0; bone < 3; bone++) {
                    Vec3s rotation;
                    Vec3f translation;
                    vec3s_to_vec3f(translation, fishModelOffsets[bone]);
                    if (bone == 0) {
                        for (int axis = 0; axis < 3; axis++) {
                            translation[axis] +=
                                values[retrieve_animation_index(fish->animInfo.animFrame, &attribute)];
                        }
                    }
                    for (int axis = 0; axis < 3; axis++) {
                        rotation[axis] =
                            values[retrieve_animation_index(fish->animInfo.animFrame, &attribute)];
                    }
                    mtxf_rotate_xyz_and_translate_and_mul(rotation, translation, matrix[bone + 2],
                                                          matrix[bone + 1]);
                    if (bone > 0) {
                        Mtx expected;
                        mtxf_to_mtx(&expected, matrix[bone + 2]);
                        EXPECT_EQ(draw != NULL, TRUE);
                        EXPECT_EQ(memcmp(draw->transform, &expected, sizeof(expected)), 0);
                        draw = draw->next;
                    }
                }
                EXPECT_EQ(fish->throwMatrix == NULL, TRUE);
            }
            EXPECT_EQ(draw == NULL, TRUE);
        }
    }
    // Animation advancement is still handled by the real engine while drawing
    // once per update, at both fish speeds, and while flags freeze it in pause.
    for (int speed = 1; speed <= 2; speed++) {
        begin(1);
        use_real_fish(1);
        struct GraphNodeObject *fish = &objects[0].header.gfx;
        fish->animInfo.animAccel = speed << 16;
        fish->animInfo.animFrameAccelAssist = 28 << 16;
        gAreaUpdateCounter = 2;
        geo_process_object(&objects[0]);
        EXPECT_EQ(fish->animInfo.animFrame, speed == 1 ? 29 : 0);
        geo_process_object(&objects[0]);
        EXPECT_EQ(fish->animInfo.animFrame, speed == 1 ? 29 : 0); // Same update.
        int lastFrame = fish->animInfo.animFrame;
        fish->node.flags &= ~GRAPH_RENDER_HAS_ANIMATION;
        sCurrPlayMode = PLAY_MODE_PAUSED;
        gAreaUpdateCounter++;
        geo_process_object(&objects[0]);
        EXPECT_EQ(fish->animInfo.animFrame, lastFrame);
        fish->node.flags |= GRAPH_RENDER_HAS_ANIMATION;
        sCurrPlayMode = 0;
        gAreaUpdateCounter++;
        geo_process_object(&objects[0]);
        EXPECT_EQ(fish->animInfo.animFrame, speed == 1 ? 0 : 2);
    }
    return TRUE;
}

static int matrix_stack_reuse(void) {
    begin(0);
    Vec3f first = { 10.5f, -14.25f, 30 }, second = { -400, 215.75f, 71.25f };
    mtxf_translate(gMatStack[1], first);
    inc_mat_stack();
    EXPECT_EQ(fixed_matrix_count(), 0);
    geo_append_display_list((void *) otherDl, LAYER_OPAQUE);
    Mtx *parent = gMatStackFixed[1];
    EXPECT_EQ(fixed_matrix_count(), 1);
    geo_append_display_list((void *) otherDl, LAYER_ALPHA);
    EXPECT_EQ(fixed_matrix_count(), 1);
    EXPECT_EQ(gMatStackFixed[1] == parent, TRUE);
    // A child draw cannot invalidate its parent's conversion.
    mtxf_translate(gMatStack[2], second);
    inc_mat_stack();
    geo_append_display_list((void *) otherDl, LAYER_OPAQUE);
    EXPECT_EQ(fixed_matrix_count(), 2);
    gMatStackIndex--;
    geo_append_display_list((void *) otherDl, LAYER_OPAQUE);
    EXPECT_EQ(fixed_matrix_count(), 2);
    EXPECT_EQ(gMatStackFixed[1] == parent, TRUE);
    // A sibling uses the same slot with a different matrix: the previous fixed
    // snapshot must remain valid in its list, and must not be reused here.
    gMatStackIndex--;
    mtxf_translate(gMatStack[1], second);
    inc_mat_stack();
    geo_append_display_list((void *) otherDl, LAYER_OPAQUE);
    Mtx expected;
    mtxf_to_mtx(&expected, gMatStack[1]);
    EXPECT_EQ(memcmp(gMatStackFixed[1], &expected, sizeof(expected)), 0);
    EXPECT_EQ(gMatStackFixed[1] != parent, TRUE);
    mtxf_to_mtx(&expected, gMatStack[2]); // The earlier child still owns its snapshot.
    EXPECT_EQ(memcmp(master.listHeads[LAYER_OPAQUE]->next->transform, &expected, sizeof(expected)), 0);
    mtxf_translate(gMatStack[2], first);
    mtxf_to_mtx(&expected, gMatStack[2]);
    EXPECT_EQ(memcmp(parent, &expected, sizeof(expected)), 0);
    gMatStackIndex--;
    return TRUE;
}

static int undrawn_matrices(void) {
    begin(1);
    use_real_fish(1);
    gCurGraphNodeMasterList = NULL;
    geo_process_object(&objects[0]);
    EXPECT_EQ(fixed_matrix_count(), 0);
    EXPECT_EQ(allocationCount, 0);
    EXPECT_EQ(gMatStackIndex, 0);
    EXPECT_EQ(gCurrAnimType, ANIM_TYPE_NONE);
    // A parent with no own draw may draw later, after a child popped the stack.
    gCurGraphNodeMasterList = &master;
    Vec3f a = { 13.75f, 42, -91 }, b = { -10, 36, 250.5f };
    mtxf_translate(gMatStack[1], a);
    inc_mat_stack();
    mtxf_translate(gMatStack[2], b);
    inc_mat_stack();
    geo_append_display_list((void *) otherDl, LAYER_OPAQUE);
    gMatStackIndex--;
    geo_append_display_list((void *) otherDl, LAYER_ALPHA);
    Mtx expected;
    mtxf_to_mtx(&expected, gMatStack[1]);
    EXPECT_EQ(memcmp(gMatStackFixed[1], &expected, sizeof(expected)), 0);
    EXPECT_EQ(gMatStackFixed[1] != gMatStackFixed[2], TRUE);
    EXPECT_EQ(fixed_matrix_count(), 2);
    gMatStackIndex--;
    return TRUE;
}
static int lab_shadow_distance(void) {
    const int depths[] = { 1000, 2000, 2500, 2999, 3000, 9000 };
    const int opacities[] = { 200, 200, 100, 0, 0, 0 };
    u16 indices[] = { 1, 0, 1, 1, 1, 2 };
    s16 values[] = { 10, 20, 30 };
    for (unsigned i = 0; i < sizeof(depths) / sizeof(*depths); i++) {
        begin(1);
        gCurrLevelNum = LEVEL_PHYSICS_LAB;
        gCurGraphNodeObject = &objects[0].header.gfx;
        gCurGraphNodeObject->cameraToObject[2] = -depths[i];
        struct GraphNodeShadow node = { .shadowSolidity = 200, .shadowScale = 100 };
        node.node.children = &fishGeo;
        gCurrAnimEnabled = 1;
        gCurrAnimType = ANIM_TYPE_TRANSLATION;
        gCurrAnimAttribute = indices;
        gCurrAnimData = values;
        gCurrAnimFrame = 0;
        gCurrAnimTranslationMultiplier = 1;
        geo_process_shadow(&node);
        EXPECT_EQ(shadowQueries, opacities[i] != 0);
        EXPECT_EQ(shadowMatrices, opacities[i] != 0);
        if (opacities[i]) {
            EXPECT_EQ(shadowOpacity, opacities[i]);
        }
        EXPECT_EQ(nativeDraws, 1);
        EXPECT_EQ(gMatStackIndex, 0);
        EXPECT_EQ(gCurrAnimAttribute == indices,
                  TRUE); // Shadow offsets never consume animation channels.
    }
    for (int protection = 0; protection < 4; protection++) {
        begin(1);
        gCurrLevelNum = LEVEL_PHYSICS_LAB;
        struct GraphNodeHeldObject held = { .objNode = &objects[0] };
        gCurGraphNodeObject = &objects[0].header.gfx;
        if (protection == 0) {
            gMarioObject = &objects[0];
        }
        if (protection == 1) {
            gCurGraphNodeObject = &gMirrorMario;
        }
        if (protection == 2) {
            gCurGraphNodeHeldObject = &held;
        }
        if (protection == 3) {
            gCurrLevelNum = 1;
        }
        gCurGraphNodeObject->cameraToObject[2] = -9000;
        struct GraphNodeShadow node = { .shadowSolidity = 200, .shadowScale = 100 };
        geo_process_shadow(&node);
        EXPECT_EQ(shadowQueries, 1);
        EXPECT_EQ(shadowOpacity, 200);
        EXPECT_EQ(gMatStackIndex, 0);
    }
    for (int actor = 0; actor < 11; actor++) {
        begin(1);
        gCurrLevelNum = LEVEL_PHYSICS_LAB;
        gCurGraphNodeObject = &objects[0].header.gfx;
        gCurGraphNodeObject->cameraToObject[2] = -9000;
        if (actor == 0) {
            objects[0].oInteractType = INTERACT_DAMAGE; // Enemy.
        }
        if (actor == 1) {
            objects[0].oInteractType = INTERACT_TEXT; // Talking NPC.
        }
        if (actor == 2) {
            objects[0].oInteractType = INTERACT_GRABBABLE; // Holdable NPC.
            objects[0].oInteractionSubtype = INT_SUBTYPE_HOLDABLE_NPC;
        }
        if (actor >= 3 && actor <= 5) {
            objects[0].behavior = bhvExclamationBox;
            objects[0].oBehParams2ndByte = actor == 5   ? 4
                                           : actor == 4 ? EXCLAMATION_BOX_BP_KOOPA_SHELL
                                                        : 0;
        }
        if (actor == 6) {
            objects[0].oInteractType = INTERACT_TEXT;
            objects[0].oInteractionSubtype = INT_SUBTYPE_SIGN;
        }
        if (actor == 7) {
            objects[0].oInteractType = INTERACT_GRABBABLE; // Ordinary small box.
        }
        if (actor == 8) {
            objects[0].oInteractType = INTERACT_GRABBABLE; // Bob-omb enemy.
            objects[0].behavior = bhvBobomb;
        }
        if (actor == 9) {
            objects[0].oInteractType = INTERACT_BOUNCE_TOP; // Goomba.
        }
        if (actor == 10) {
            objects[0].oInteractType = INTERACT_KOOPA;
        }
        struct GraphNodeShadow node = { .shadowSolidity = 200, .shadowScale = 100 };
        geo_process_shadow(&node);
        int keepsShadow = actor < 5 || actor >= 8;
        EXPECT_EQ(shadowQueries, keepsShadow);
        if (keepsShadow) {
            EXPECT_EQ(shadowOpacity, 200);
        }
        EXPECT_EQ(gMatStackIndex, 0);
    }
    for (int kind = 0; kind < 2; kind++) {
        for (int visibility = 0; visibility < 4; visibility++) {
            begin(1);
            gCurrLevelNum = LEVEL_PHYSICS_LAB;
            gEmulator = 0;
            labObjectVisibility = visibility == 1 ? 0 : -1;
            if (kind == 0) {
                objects[0].oInteractType = INTERACT_DAMAGE;
            } else {
                objects[0].behavior = bhvExclamationBox;
            }
            objects[0].header.gfx.node.flags = GRAPH_RENDER_ACTIVE;
            objects[0].header.gfx.pos[2] = -9000;
            objects[0].header.gfx.sharedChild = &objectShadowGeo.node;
            objectShadowGeo.shadowSolidity = 200;
            objectShadowGeo.shadowScale = 100;
            objectShadowGeo.node.children = &fishGeo;
            if (visibility == 2) {
                objects[0].header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
            }
            if (visibility == 3) {
                objects[0].header.gfx.areaIndex = 2;
            }
            geo_process_object(&objects[0]);
            EXPECT_EQ(shadowQueries, visibility == 0);
            EXPECT_EQ(nativeDraws, visibility == 0);
            EXPECT_EQ(gMatStackIndex, 0);
        }
    }
    return TRUE;
}
/* Native fish geometry exercises the real object traversal; pose appearance
 * remains the responsibility of he_thicc.action_and_render_hooks. */
static int pose_owner(void) {
    for (int paused = 0; paused < 2; paused++) {
        for (int throwing = 0; throwing < 2; throwing++) {
            begin(2);
            use_real_fish(2);
            gMarioObject = &objects[0];
            objects[0].behavior = objects[1].behavior = NULL;
            sCurrPlayMode = paused ? PLAY_MODE_PAUSED : 0;
            Mat4 throwMatrix;
            mtxf_identity(throwMatrix);
            if (throwing) {
                objects[0].header.gfx.throwMatrix = &throwMatrix;
            }
            heThiccPoseCalls = 0;
            geo_process_object(&objects[0]);
            EXPECT_EQ(animationCalls, 1);
            // Two fish draws plus Mario's post-model dither-state cleanup.
            EXPECT_EQ(fixed_matrix_count(), 3);
            EXPECT_EQ(heThiccPoseCalls, 1);
            EXPECT_EQ(gCurGraphNodeObject == NULL, TRUE);
            EXPECT_EQ(objects[0].header.gfx.animInfo.animFrame, 1);
            geo_process_object(&objects[1]);
            EXPECT_EQ(heThiccPoseCalls, 1);
            gMirrorMario.animInfo = objects[0].header.gfx.animInfo;
            real_geo_set_animation_globals(&gMirrorMario.animInfo, FALSE);
            EXPECT_EQ(heThiccPoseCalls, 2);
            gCurGraphNodeHeldObject = (void *) &objects[0];
            real_geo_set_animation_globals(&objects[0].header.gfx.animInfo, FALSE);
            EXPECT_EQ(heThiccPoseCalls, 2);
            gCurGraphNodeHeldObject = NULL;
        }
    }
    return TRUE;
}

static int mario_alpha_state_cleanup(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(1);
        gMarioObject = &objects[0];
        gMarioState->flags = MARIO_VANISH_CAP;
        gBodyStates[0].modelState = MODEL_STATE_ALPHA | 128;
        struct GraphNodeGenerated alpha = { 0 };
        alpha.fnNode.func = (GraphNodeFunc) geo_mirror_mario_set_alpha;
        struct GraphNodeCullingRadius owner = { 0 };
        owner.node.type = GRAPH_NODE_TYPE_CULLING_RADIUS;
        owner.node.flags = GRAPH_RENDER_ACTIVE;
        owner.node.children = &alpha.fnNode.node;
        alpha.fnNode.node.next = &bubbleGeo.node;
        objects[0].header.gfx.sharedChild = &owner.node;
        geo_process_object(&objects[0]);
        struct DisplayListNode *head = master.listHeads[LAYER_TRANSPARENT];
        EXPECT_EQ(head != NULL, TRUE);
        EXPECT_EQ(((Gfx *) head->displayList)[0].n, G_AC_NONE);
        EXPECT_EQ(((Gfx *) head->displayList)[2].op, CMD_PRIM_COLOR);
        // The default Mupen fixture uses full-opacity noise cutouts.
        EXPECT_EQ(((Gfx *) head->displayList)[2].n, 128);
        struct DisplayListNode *reset = master.listTails[LAYER_TRANSPARENT];
        EXPECT_EQ(((Gfx *) reset->displayList)[0].op, CMD_PIPE_SYNC);
        EXPECT_EQ(((Gfx *) reset->displayList)[1].n, G_AC_NONE);
        EXPECT_EQ(((Gfx *) reset->displayList)[2].op, CMD_PRIM_COLOR);
        EXPECT_EQ(((Gfx *) reset->displayList)[2].n, 255);
        EXPECT_EQ(((Gfx *) reset->displayList)[3].op, CMD_END);
        // A later translucent volume must follow the reset, not inherit Mario's dither.
        gCurGraphNodeObject = NULL;
        geo_append_display_list((void *) otherDl, LAYER_TRANSPARENT);
        EXPECT_EQ(reset->next->displayList == otherDl, TRUE);
    }
    gMarioState->flags = 0;
    memset(gBodyStates, 0, sizeof(gBodyStates));
    return TRUE;
}

static int mario_aligned_dither(void) {
    // The wall copy must draw every accepted dither pixel, even where its
    // triangle coverage cannot overflow the background's coverage value.
    for (s32 cycle = 0; cycle < 2; cycle++) {
        u32 mode = dl_mario_dither_silhouette_begin[1].indices[cycle];
        EXPECT_EQ(mode & (IM_RD | CLR_ON_CVG | CVG_X_ALPHA | AA_EN | Z_CMP | Z_UPD), 0);
        EXPECT_EQ(mode & FORCE_BL, FORCE_BL);
    }
    // Transparent wing texels must stay rejected after the RDP adds alpha dither.
    const Gfx *wingSilhouettes[] = { dl_silhouette_begin, dl_mario_dither_silhouette_begin };
    const u32 commandCounts[] = { ARRAY_COUNT(dl_silhouette_begin),
                                  ARRAY_COUNT(dl_mario_dither_silhouette_begin) };
    s32 alphaCompare = G_AC_NONE, threshold = 0;
    for (u32 material = 0; material < ARRAY_COUNT(wingSilhouettes); material++) {
        alphaCompare = G_AC_NONE;
        threshold = 0;
        for (u32 i = 0; i < commandCounts[material]; i++) {
            const Gfx *cmd = &wingSilhouettes[material][i];
            if (cmd->op == CMD_ALPHA_COMPARE) {
                alphaCompare = cmd->n;
            }
            if (cmd->op == CMD_BLEND_COLOR) {
                threshold = cmd->n;
            }
        }
        EXPECT_EQ(alphaCompare, G_AC_THRESHOLD);
        for (s32 dither = 0; dither < 8; dither++) {
            EXPECT_EQ(dither >= threshold, FALSE);
            // Feather pixels remain accepted in dim and half-opacity draws.
            EXPECT_EQ(32 + dither >= threshold, TRUE);
            EXPECT_EQ(64 + dither >= threshold, TRUE);
            EXPECT_EQ(128 + dither >= threshold, TRUE);
            EXPECT_EQ(255 + dither >= threshold, TRUE);
        }
    }
    for (u32 i = 0; i < ARRAY_COUNT(dl_silhouette_end); i++) {
        const Gfx *cmd = &dl_silhouette_end[i];
        if (cmd->op == CMD_ALPHA_COMPARE) {
            alphaCompare = cmd->n;
        }
        if (cmd->op == CMD_BLEND_COLOR) {
            threshold = cmd->n;
        }
    }
    EXPECT_EQ(alphaCompare, G_AC_NONE);
    EXPECT_EQ(threshold, 0);
    const Gfx *ordinary = (const Gfx *) 0x04012340;
    const Gfx *ordered = (const Gfx *) 0x80200000;
    const u32 powers[] = { 0, MARIO_VANISH_CAP, MARIO_TELEPORTING };
    for (int emulator = 0; emulator < 2; emulator++) {
        for (u32 powered = 0; powered < ARRAY_COUNT(powers); powered++) {
            for (s32 alpha = 0; alpha < 255; alpha++) {
                begin(0);
                gEmulator = emulator ? EMU_MUPEN : EMU_CONSOLE;
                gMarioState->flags = powers[powered];
                struct GraphNodeGenerated node = { 0 };
                Gfx *gfx = make_gfx_mario_alpha(&node, alpha);
                EXPECT_EQ(gfx[0].n, G_AC_NONE);
                EXPECT_EQ(gfx[1].n, powered ? 255 : alpha);
                EXPECT_EQ(gfx[2].op, CMD_PRIM_COLOR);
                EXPECT_EQ(gfx[2].n, emulator ? alpha : (alpha + 16) >> 5);
                // Both passes use this cutoff. Neither the fog blend alpha nor
                // silhouette coverage can introduce a pixel outside that pattern.
                if (!emulator) {
                    s32 pixels = 0;
                    for (s32 pattern = 0; pattern < 8; pattern++) {
                        pixels += gfx[2].n + pattern >= 8;
                    }
                    EXPECT_EQ(pixels, gfx[2].n);
                }
                Gfx capAlpha[5];
                geo_append_cap_blink_alpha(capAlpha, alpha, LAYER_TRANSPARENT);
                EXPECT_EQ(capAlpha[2].n, powered ? 255 : alpha);
                EXPECT_EQ(capAlpha[3].n, emulator ? alpha : (alpha + 16) >> 5);
            }
        }
    }
    // A particle following Mario must receive its layer's original blender,
    // including cycle one, rather than the temporary two-cycle pass mode.
    begin(0);
    gEmulator = EMU_CONSOLE;
    gMarioObject = &objects[0];
    gCurGraphNodeObject = &gMarioObject->header.gfx;
    gCurGraphNodeHeldObject = NULL;
    gMarioState->flags = MARIO_VANISH_CAP;
    geo_append_display_list((void *) ordinary, LAYER_TRANSPARENT);
    gCurGraphNodeObject = NULL;
    geo_append_display_list((void *) otherDl, LAYER_TRANSPARENT);
    renderModeTable_1Cycle[0].modes[LAYER_TRANSPARENT] = 0x12345678;
    renderModeTable_2Cycle[0].modes[LAYER_TRANSPARENT] = 0x87654321;
    Gfx *start = gDisplayListHead;
    real_geo_process_master_list_sub(&master);
    s32 sawMario = FALSE;
    s32 restored = FALSE;
    s32 sawParticle = FALSE;
    u32 drawMode = 0;
    for (Gfx *cmd = start; cmd < gDisplayListHead; cmd++) {
        if (cmd->op == CMD_RENDER_MODE) {
            drawMode = cmd->indices[0];
        }
        if (cmd->op == CMD_MATERIAL && cmd->ptr == ordered) {
            EXPECT_EQ(drawMode & (AA_EN | IM_RD), 0);
            EXPECT_EQ(drawMode & Z_CMP, 0x12345678 & Z_CMP);
            sawMario = TRUE;
        } else if (sawMario && cmd->op == CMD_RENDER_MODE) {
            EXPECT_EQ(cmd->indices[0], 0x12345678);
            EXPECT_EQ((u32) cmd->indices[1], 0x87654321);
            restored = TRUE;
        } else if (cmd->op == CMD_MATERIAL && cmd->ptr == otherDl) {
            EXPECT_EQ(restored, TRUE);
            sawParticle = TRUE;
        }
        if (sawParticle) {
            break;
        }
    }
    EXPECT_EQ(sawMario && sawParticle, TRUE);
    for (u32 powered = 0; powered < ARRAY_COUNT(powers); powered++) {
        gMarioState->flags = powers[powered];
        for (s32 mario = 0; mario < 2; mario++) {
            for (s32 opaque = 0; opaque < 2; opaque++) {
                struct DisplayListNode dim = {
                    .displayList = (void *) otherDl,
                    .blinkAlpha = 51,
                    .marioSilhouette = mario,
                    .blinkOpaque = opaque,
                };
                Gfx commands[12];
                geo_draw_blinking_display_list(commands, &dim, TRUE, 0x12345678, 0x87654321);
                s32 cutout = mario && powered;
                EXPECT_EQ(commands[1].indices[0] & (AA_EN | IM_RD), cutout ? 0 : AA_EN | IM_RD);
                EXPECT_EQ(commands[1].indices[0] & Z_CMP, Z_CMP);
                EXPECT_EQ(commands[2].n, cutout ? 255 : 51);
                s32 alphaCutout = !opaque || cutout;
                EXPECT_EQ(commands[1].indices[0] & CVG_X_ALPHA, alphaCutout ? CVG_X_ALPHA : 0);
                EXPECT_EQ(commands[4].n, alphaCutout ? G_AC_THRESHOLD : G_AC_NONE);
                EXPECT_EQ((u32) commands[11].indices[1], 0x87654321);
            }
        }
    }
    for (s32 mirror = 0; mirror < 2; mirror++) {
        for (s32 powered = 0; powered < 3; powered++) {
            for (s32 held = 0; held < 2; held++) {
                begin(0);
                gMarioObject = &objects[0];
                gCurGraphNodeObject = mirror ? &gMirrorMario : &gMarioObject->header.gfx;
                gCurGraphNodeObject->node.flags = GRAPH_RENDER_SILHOUETTE;
                struct GraphNodeHeldObject heldNode = { 0 };
                gCurGraphNodeHeldObject = held ? &heldNode : NULL;
                gMarioState->flags = powered == 1   ? MARIO_VANISH_CAP
                                     : powered == 2 ? MARIO_TELEPORTING
                                                    : 0;
                geo_append_display_list((void *) ordinary, LAYER_TRANSPARENT);
                const Gfx *expected = powered && !held ? ordered : ordinary;
                EXPECT_EQ(master.listHeads[LAYER_TRANSPARENT]->displayList == expected, TRUE);
                if (!held) {
                    EXPECT_EQ(master.listHeads[LAYER_SILHOUETTE_ALPHA]->displayList == expected, TRUE);
                    if (powered) {
                        master.node.flags = GRAPH_RENDER_Z_BUFFER;
                        gDisplayListHead = topCommands;
                        real_geo_process_master_list_sub(&master);
                        s32 wallSetups = 0;
                        for (Gfx *cmd = topCommands; cmd < gDisplayListHead; cmd++) {
                            wallSetups +=
                                cmd->op == CMD_MATERIAL && cmd->ptr == dl_mario_dither_silhouette_begin;
                        }
                        EXPECT_EQ(wallSetups, 1);
                    }
                }
                // Generated setup commands and foreign models retain their own material.
                geo_append_display_list((void *) otherDl, LAYER_TRANSPARENT);
                EXPECT_EQ(master.listTails[LAYER_TRANSPARENT]->displayList == otherDl, TRUE);
            }
        }
    }
    gMarioState->flags = 0;
    gCurGraphNodeHeldObject = NULL;
    return TRUE;
}

static int cap_blink_opacity(void) {
    const int alphas[][2] = {
        { 128, 255 }, { 128, 150 }, { 90, 90 }, { 51, 51 }, { 25, 25 }, { 0, 0 }
    };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned fade = 0; fade < sizeof(alphas) / sizeof(*alphas); fade++) {
            begin(1);
            int onAlpha = alphas[fade][0];
            int offAlpha = alphas[fade][1];
            gBodyStates[0] =
                (struct MarioBodyState){ .modelState = MODEL_STATE_ALPHA | onAlpha,
                                         .capBlinkModelState =
                                             offAlpha == 255 ? 0 : MODEL_STATE_ALPHA | offAlpha,
                                         .capBlinkBlend = MARIO_CAP_BLINK_OPACITY };
            struct GraphNodeGenerated alphaNode = { 0 };
            for (int render = 0; render < 3; render++) {
                struct MarioBodyState before = gBodyStates[0];
                Gfx *dl = geo_mirror_mario_set_alpha(GEO_CONTEXT_RENDER, &alphaNode.fnNode.node, NULL);
                EXPECT_EQ(dl[1].n, (onAlpha + offAlpha + 1) / 2);
                EXPECT_EQ(memcmp(&before, &gBodyStates[0], sizeof(before)), 0);
                struct GraphNodeSwitchCase sw = { 0 };
                sw.fnNode.func = (GraphNodeFunc) geo_switch_mario_cap_effect;
                sw.fnNode.node.children = &capBranches[0];
                for (int i = 0; i < 4; i++) {
                    capBranches[i].next = &capBranches[(i + 1) % 4];
                }
                capDraws = heldDraws = capPoseMismatch = 0;
                gCurrAnimType = ANIM_TYPE_TRANSLATION;
                gCurrAnimAttribute = capAttributes;
                geo_process_switch(&sw);
                EXPECT_EQ(capDraws, 1);
                EXPECT_EQ(capCases[0], 1);
                EXPECT_EQ(heldDraws, 1);
                EXPECT_EQ(capPoseMismatch, FALSE);
            }
        }
    }
    memset(gBodyStates, 0, sizeof(gBodyStates));
    return TRUE;
}

static int cap_blink_rendering(void) {
    const int alphas[][2] = {
        { 255, 255 }, { 255, 128 }, { 64, 255 }, { 51, 51 }, { 25, 25 }, { 0, 0 }
    };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned fade = 0; fade < sizeof(alphas) / sizeof(*alphas); fade++) {
            for (int mirror = 0; mirror < 2; mirror++) {
                begin(1);
                gCurGraphNodeObject = mirror ? &gMirrorMario : &objects[0].header.gfx;
                struct GraphNodeSwitchCase sw = { 0 };
                sw.fnNode.func = (GraphNodeFunc) geo_switch_mario_cap_effect;
                sw.fnNode.node.children = &capBranches[0];
                for (int i = 0; i < 4; i++) {
                    capBranches[i].next = &capBranches[(i + 1) % 4];
                }
                gBodyStates[0] = (struct MarioBodyState){
                    .modelState = MODEL_STATE_METAL
                                  | (alphas[fade][1] < 255 ? MODEL_STATE_ALPHA | alphas[fade][1] : 0),
                    .capState = 2,
                    .handState = 5,
                    .capBlinkBlend = MARIO_CAP_BLINK_GEOMETRY,
                    .capBlinkModelState =
                        alphas[fade][0] < 255 ? MODEL_STATE_ALPHA | alphas[fade][0] : 0,
                    .capBlinkCapState = 1,
                    .capBlinkHandState = 4
                };
                struct MarioBodyState before = gBodyStates[0];
                for (int render = 0; render < 3; render++) {
                    capDraws = heldDraws = capPoseMismatch = 0;
                    gCurrAnimType = ANIM_TYPE_TRANSLATION;
                    gCurrAnimAttribute = capAttributes;
                    gCurrAnimFrame = 17;
                    gCurrAnimData = NULL;
                    gCurrAnimEnabled = TRUE;
                    geo_process_switch(&sw);
                    EXPECT_EQ(capDraws, 2);
                    EXPECT_EQ(capPoseMismatch, FALSE);
                    EXPECT_EQ(heldDraws, 1);
                    EXPECT_EQ(capCases[0], capAlphas[0] < 255 ? 1 : 0);
                    EXPECT_EQ(capCases[1], 3);
                    EXPECT_EQ(capStates[0], 1);
                    EXPECT_EQ(capStates[1], 2);
                    EXPECT_EQ(capHands[0], 4);
                    EXPECT_EQ(capHands[1], 5);
                    int poweredAlpha = alphas[fade][1];
                    int ordinaryAlpha = alphas[fade][0];
                    EXPECT_EQ(capAlphas[1], (poweredAlpha + 1) / 2);
                    float combined = capAlphas[1] + capAlphas[0] * (255 - capAlphas[1]) / 255.0f;
                    EXPECT_EQ(fabsf(combined - (ordinaryAlpha + poweredAlpha) / 2.0f) < 1, TRUE);
                    EXPECT_EQ(memcmp(&before, &gBodyStates[0], sizeof(before)), 0);
                    EXPECT_EQ(gCurrAnimType, ANIM_TYPE_ROTATION);
                    EXPECT_EQ(gCurrAnimAttribute == capAttributes + 8, TRUE);
                    EXPECT_EQ(gCurrAnimFrame, 17);
                    EXPECT_EQ(gCurrAnimEnabled, TRUE);
                    struct DisplayListNode *last = master.listHeads[LAYER_TRANSPARENT];
                    while (last->next != NULL) {
                        last = last->next;
                    }
                    const Gfx *restore = last->displayList;
                    EXPECT_EQ(restore[2].n, poweredAlpha);
                }
                // Allocation failure falls back to one intact powered draw.
                failNextAllocation = TRUE;
                capDraws = heldDraws = capPoseMismatch = 0;
                gCurrAnimType = ANIM_TYPE_TRANSLATION;
                gCurrAnimAttribute = capAttributes;
                geo_process_switch(&sw);
                EXPECT_EQ(capDraws, 1);
                EXPECT_EQ(heldDraws, 1);
                EXPECT_EQ(capCases[0], alphas[fade][1] < 255 ? 3 : 2);
                EXPECT_EQ(capAlphas[0], alphas[fade][1]);
                EXPECT_EQ(memcmp(&before, &gBodyStates[0], sizeof(before)), 0);
            }
        }
    }
    gCurGraphNodeObject = NULL;
    memset(gBodyStates, 0, sizeof(gBodyStates));
    return TRUE;
}

static int object_blink_phases(void) {
    begin(0);
    struct Object obj = { 0 };
    o = &obj;
    for (s32 cycle = 0; cycle < 3; cycle++) {
        obj.activeFlags = ACTIVE_FLAG_ACTIVE;
        obj.header.gfx.node.flags = GRAPH_RENDER_ACTIVE;
        for (s32 timer = 0; timer <= 403; timer++) {
            obj.oTimer = timer;
            // Behavior-script frame setup is a supplied service; helpers below are production.
            obj.activeFlags &= ~ACTIVE_FLAG_BLINKING;
            EXPECT_EQ(cur_obj_wait_then_blink(360, 20), timer == 403);
            EXPECT_EQ((obj.activeFlags & ACTIVE_FLAG_BLINKING) != 0,
                      timer >= 360 && ((timer - 360) & 1));
            EXPECT_EQ((obj.header.gfx.node.flags & GRAPH_RENDER_INVISIBLE) != 0,
                      timer >= 360 && ((timer - 360) & 1));
        }
        cur_obj_hide();
        EXPECT_EQ(obj.activeFlags & ACTIVE_FLAG_BLINKING, 0);
        EXPECT_EQ((obj.header.gfx.node.flags & GRAPH_RENDER_INVISIBLE) != 0, TRUE);
        cur_obj_unhide();
        EXPECT_EQ(obj.header.gfx.node.flags & GRAPH_RENDER_INVISIBLE, 0);

        for (s32 timer = 0; timer <= 340; timer++) {
            obj.oTimer = timer;
            obj.activeFlags &= ~ACTIVE_FLAG_BLINKING;
            EXPECT_EQ(obj_flicker_and_disappear(&obj, 300), timer == 340);
            EXPECT_EQ((obj.activeFlags & ACTIVE_FLAG_BLINKING) != 0,
                      timer >= 300 && timer < 340 && (timer & 1));
        }
        EXPECT_EQ(obj.activeFlags, ACTIVE_FLAG_DEACTIVATED);
        obj_set_blink_phase(&obj, TRUE);
        EXPECT_EQ(obj.activeFlags, ACTIVE_FLAG_DEACTIVATED);

        obj.activeFlags = ACTIVE_FLAG_ACTIVE;
        obj.oBreakableBoxSmallFramesSinceReleased = 809;
        sRespawners = 0;
        for (s32 frame = 810; frame <= 901; frame++) {
            obj.activeFlags &= ~ACTIVE_FLAG_BLINKING;
            breakable_box_small_released_loop();
            EXPECT_EQ(obj.oBreakableBoxSmallFramesSinceReleased, frame);
            EXPECT_EQ((obj.activeFlags & ACTIVE_FLAG_BLINKING) != 0,
                      frame > 810 && frame <= 900 && (frame & 1));
        }
        EXPECT_EQ(sRespawners, 1);
        EXPECT_EQ(obj.activeFlags, ACTIVE_FLAG_DEACTIVATED);

        obj.activeFlags = ACTIVE_FLAG_ACTIVE;
        obj_set_blink_phase(&obj, TRUE);
        sWithinRenderDistance = TRUE;
        set_object_visibility(&obj, 3000);
        EXPECT_EQ((obj.activeFlags & ACTIVE_FLAG_BLINKING) != 0, TRUE);
        sWithinRenderDistance = FALSE;
        set_object_visibility(&obj, 3000);
        EXPECT_EQ(obj.activeFlags & ACTIVE_FLAG_BLINKING, 0);
        EXPECT_EQ((obj.header.gfx.node.flags & GRAPH_RENDER_INVISIBLE) != 0, TRUE);
    }
    o = NULL;
    return TRUE;
}

static int bowser_flame_billboards(void) {
    const s32 models[] = { MODEL_RED_FLAME, MODEL_BLUE_FLAME };
    static const Gfx frames[8];
    for (u32 model = 0; model < ARRAY_COUNT(models); model++) {
        for (s32 frame = 0; frame < 9; frame++) {
            struct GraphNode geometry = { .type = GRAPH_NODE_TYPE_START };
            struct GraphNodeSwitchCase selection = { 0 };
            struct GraphNodeDisplayList displays[8] = { 0 };
            geometry.children = &selection.fnNode.node;
            selection.fnNode.node.type = GRAPH_NODE_TYPE_SWITCH_CASE;
            selection.fnNode.func = geo_switch_anim_state;
            selection.numCases = 8;
            selection.fnNode.node.children = &displays[0].node;
            for (s32 i = 0; i < 8; i++) {
                displays[i].node.type = GRAPH_NODE_TYPE_DISPLAY_LIST;
                displays[i].node.flags = GRAPH_RENDER_ACTIVE;
                SET_GRAPH_NODE_LAYER(displays[i].node.flags, LAYER_TRANSPARENT_INTER);
                displays[i].node.next = &displays[(i + 1) % 8].node;
                displays[i].displayList = (void *) &frames[i];
            }
            Mtx nativeMatrix;
            Vec3f nativePosition;
            for (s32 fast = 0; fast < 2; fast++) {
                begin(1);
                gCurrLevelNum = fast ? LEVEL_BOWSER_3 : 0;
                loaded[models[model]] = &geometry;
                objects[0].header.gfx.sharedChild = &geometry;
                objects[0].oAnimState = frame;
                vec3f_set(objects[0].header.gfx.pos, -123.25f, 444.5f, -1000.75f);
                vec3f_set(objects[0].header.gfx.scale, 3.14f, 2.73f, 1);
                vec3f_set(gMatStack[0][3], 13.25f, -25.5f, 76.125f);
                camera.roll = 0x1234;
                geo_process_object(&objects[0]);
                struct DisplayListNode *draw = master.listHeads[LAYER_TRANSPARENT_INTER];
                EXPECT_EQ(draw != NULL && draw->next == NULL, TRUE);
                EXPECT_EQ(draw->displayList == &frames[frame < 8 ? frame : 0], TRUE);
                EXPECT_EQ(gMatStackIndex, 0);
                EXPECT_EQ(gCurGraphNodeObject == NULL, TRUE);
                if (!fast) {
                    nativeMatrix = *draw->transform;
                    vec3f_copy(nativePosition, objects[0].header.gfx.cameraToObject);
                } else {
                    EXPECT_EQ(memcmp(&nativeMatrix, draw->transform, sizeof(Mtx)), 0);
                    EXPECT_EQ(
                        memcmp(nativePosition, objects[0].header.gfx.cameraToObject, sizeof(Vec3f)), 0);
                    EXPECT_EQ(nativeDraws, 0);
                }
            }
            // Exceptional rendering states retain the native graph traversal.
            objects[0].activeFlags |= ACTIVE_FLAG_BLINKING;
            EXPECT_EQ(geo_try_draw_bowser_flame(&objects[0]), FALSE);
            objects[0].activeFlags &= ~ACTIVE_FLAG_BLINKING;
            objects[0].header.gfx.throwMatrix = &gMatStack[0];
            EXPECT_EQ(geo_try_draw_bowser_flame(&objects[0]), FALSE);
            objects[0].header.gfx.throwMatrix = NULL;
            objects[0].header.gfx.node.children = &group;
            EXPECT_EQ(geo_try_draw_bowser_flame(&objects[0]), FALSE);
            objects[0].header.gfx.node.children = NULL;
            objects[0].header.gfx.animInfo.curAnim = (struct Animation *) &blue_fish_seg3_anim_0301C298;
            EXPECT_EQ(geo_try_draw_bowser_flame(&objects[0]), FALSE);
            objects[0].header.gfx.animInfo.curAnim = NULL;
            hitboxView = 1;
            EXPECT_EQ(geo_try_draw_bowser_flame(&objects[0]), FALSE);
            hitboxView = 0;
            objects[0].oAnimState = -1;
            EXPECT_EQ(geo_try_draw_bowser_flame(&objects[0]), FALSE);
            objects[0].oAnimState = 0;
            failNextAllocation = 1;
            EXPECT_EQ(geo_try_draw_bowser_flame(&objects[0]), FALSE);
            EXPECT_EQ(gCurGraphNodeObject == NULL, TRUE);
            objects[0].header.gfx.sharedChild = NULL;
            EXPECT_EQ(geo_try_draw_bowser_flame(&objects[0]), FALSE);
        }
    }
    begin(0);
    return TRUE;
}

static int arena_enemy_fade_rendering(void) {
    const Vtx *solidVertices[] = { sGoombaVertexGroup4,        bobomb_seg8_vertex_08022E30,
                                   bully_seg5_vertex_05002C68, lakitu_enemy_seg5_vertex_05013790,
                                   spiny_seg5_vertex_05015888, spiny_egg_seg5_vertex_050149A8 };
    // Native zero-alpha solid triangles must retain coverage during the fade.
    // These are command-state checks, not an RSP/RDP or pixel reproduction.
    for (u32 actor = 0; actor < ARRAY_COUNT(solidVertices); actor++) {
        for (s32 vertex = 0; vertex < 3; vertex++) {
            EXPECT_EQ(solidVertices[actor][vertex].v.cn[3], 0);
        }
        begin(2);
        objects[0].bowserArenaFade = BOWSER_ARENA_FADE_FRAMES / 2;
        gCurGraphNodeObject = &objects[0].header.gfx;
        geo_append_display_list((void *) otherDl, LAYER_OPAQUE);
        Gfx commands[12];
        geo_draw_blinking_display_list(commands, master.listHeads[LAYER_TRANSPARENT], TRUE, 0x12340000,
                                       0x23450000);
        EXPECT_EQ(commands[1].indices[0] & CVG_X_ALPHA, 0);
        EXPECT_EQ(commands[4].n, G_AC_NONE);
        // Texture cutouts keep the native alpha mask despite sharing the faded layer.
        geo_append_display_list((void *) otherDl, LAYER_ALPHA);
        geo_draw_blinking_display_list(commands, master.listTails[LAYER_TRANSPARENT], TRUE, 0x12340000,
                                       0x23450000);
        EXPECT_EQ(commands[1].indices[0] & CVG_X_ALPHA, CVG_X_ALPHA);
        EXPECT_EQ(commands[4].n, G_AC_THRESHOLD);
    }
    for (int child = 0; child < 2; child++) {
        for (int remaining = BOWSER_ARENA_FADE_FRAMES; remaining >= 0; remaining--) {
            begin(2);
            objects[0].bowserArenaFade = remaining;
            objects[1].parentObj = &objects[0];
            gCurGraphNodeObject = &objects[child].header.gfx;
            geo_append_display_list((void *) otherDl, LAYER_OPAQUE);
            int layer = remaining ? LAYER_TRANSPARENT : LAYER_OPAQUE;
            struct DisplayListNode *draw = master.listHeads[layer];
            EXPECT_EQ(draw != NULL, TRUE);
            int alpha =
                remaining
                    ? MAX(1, (BOWSER_ARENA_FADE_FRAMES - remaining) * 255 / BOWSER_ARENA_FADE_FRAMES)
                    : 0;
            EXPECT_EQ(draw->blinkAlpha, alpha);
            EXPECT_EQ(draw->blinkOpaque, TRUE);
            if (remaining) {
                Gfx commands[12];
                geo_draw_blinking_display_list(commands, draw, TRUE, 0x12340000, 0x23450000);
                EXPECT_EQ(commands[2].op, CMD_FOG_COLOR);
                EXPECT_EQ(commands[2].n, alpha);
            }
        }
    }
    return TRUE;
}

static int object_blink_rendering(void) {
    const s32 layers[] = { LAYER_OPAQUE,
                           LAYER_ALPHA,
                           LAYER_OCCLUDE_SILHOUETTE_OPAQUE,
                           LAYER_OCCLUDE_SILHOUETTE_ALPHA,
                           LAYER_OPAQUE_DECAL,
                           LAYER_OPAQUE_INTER,
                           LAYER_ALPHA_DECAL,
                           LAYER_TRANSPARENT,
                           LAYER_MARIO_SHADOW };
    for (s32 cycle = 0; cycle < 3; cycle++) {
        for (u32 i = 0; i < sizeof(layers) / sizeof(layers[0]); i++) {
            begin(1);
            objects[0].activeFlags = ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_BLINKING;
            gCurGraphNodeObject = &objects[0].header.gfx;
            s32 layer = LAYER_TRANSPARENT;
            if (layers[i] == LAYER_OPAQUE_DECAL || layers[i] == LAYER_ALPHA_DECAL) {
                layer = LAYER_TRANSPARENT_DECAL;
            } else if (layers[i] == LAYER_OPAQUE_INTER) {
                layer = LAYER_TRANSPARENT_INTER;
            } else if (layers[i] == LAYER_MARIO_SHADOW) {
                layer = LAYER_MARIO_SHADOW;
            }
            geo_append_display_list((void *) otherDl, layers[i]);
            struct DisplayListNode *draw = master.listHeads[layer];
            EXPECT_EQ(draw != NULL, TRUE);
            EXPECT_EQ(draw->blinkAlpha, 51);
            s32 opaque = layers[i] == LAYER_OPAQUE || layers[i] == LAYER_OPAQUE_DECAL
                         || layers[i] == LAYER_OPAQUE_INTER
                         || layers[i] == LAYER_OCCLUDE_SILHOUETTE_OPAQUE;
            EXPECT_EQ(draw->blinkOpaque, opaque);
            // The queued draw owns its phase even if the live object's state changes.
            objects[0].activeFlags = ACTIVE_FLAG_ACTIVE;
            for (s32 zbuffer = 0; zbuffer < 2; zbuffer++) {
                Gfx commands[12];
                Gfx *end =
                    geo_draw_blinking_display_list(commands, draw, zbuffer, 0x12340000, 0x23450000);
                EXPECT_EQ(end - commands, 12);
                EXPECT_EQ(commands[0].op, CMD_PIPE_SYNC);
                EXPECT_EQ(commands[1].op, CMD_RENDER_MODE);
                EXPECT_EQ(commands[1].indices[0] & Z_CMP, zbuffer ? Z_CMP : 0);
                EXPECT_EQ(commands[1].indices[0] & Z_UPD, 0);
                EXPECT_EQ(commands[1].indices[0] & CVG_X_ALPHA, opaque ? 0 : CVG_X_ALPHA);
                EXPECT_EQ(commands[2].op, CMD_FOG_COLOR);
                EXPECT_EQ(commands[2].n, 51);
                EXPECT_EQ(commands[3].op, CMD_BLEND_COLOR);
                EXPECT_EQ(commands[3].n, 1);
                EXPECT_EQ(commands[4].op, CMD_ALPHA_COMPARE);
                EXPECT_EQ(commands[4].n, opaque ? G_AC_NONE : G_AC_THRESHOLD);
                EXPECT_EQ(commands[5].ptr == otherDl, TRUE);
                EXPECT_EQ(commands[7].n, G_AC_NONE);
                EXPECT_EQ(commands[8].n, 0);
                EXPECT_EQ(commands[9].op, CMD_PIPE_SYNC);
                EXPECT_EQ(commands[10].n, 255);
                EXPECT_EQ(commands[11].indices[0], 0x12340000);
                EXPECT_EQ(commands[11].indices[1], 0x23450000);
            }
            geo_append_display_list((void *) otherDl, LAYER_OPAQUE);
            EXPECT_EQ(master.listHeads[LAYER_OPAQUE]->blinkAlpha, 0);
        }
        begin(1);
        objects[0].activeFlags = ACTIVE_FLAG_ACTIVE;
        obj_set_blink_phase(&objects[0], TRUE);
        objects[0].behavior = &bhvSpawnedStar;
        geo_process_object(&objects[0]);
        EXPECT_EQ(nativeDraws, 1);
        EXPECT_EQ(master.listHeads[LAYER_TRANSPARENT]->blinkAlpha, 51);
        begin(1);
        objects[0].header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
        geo_process_object(&objects[0]);
        EXPECT_EQ(nativeDraws, 0);
    }
    begin(1);
    struct Object held = { 0 };
    struct GraphNodeHeldObject heldNode = { 0 };
    held.activeFlags = ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_BLINKING;
    heldNode.objNode = &held;
    gCurGraphNodeObject = &objects[0].header.gfx;
    gCurGraphNodeHeldObject = &heldNode;
    EXPECT_EQ(geo_object_blend_alpha(), 51);
    held.activeFlags = ACTIVE_FLAG_ACTIVE;
    objects[0].activeFlags = ACTIVE_FLAG_BLINKING | ACTIVE_FLAG_ACTIVE;
    EXPECT_EQ(geo_object_blend_alpha(), 0);
    gCurGraphNodeHeldObject = NULL;
    gCurGraphNodeObject = &gMirrorMario;
    EXPECT_EQ(geo_object_blend_alpha(), 0);
    gCurGraphNodeObject = NULL;
    begin(0);
    loaded[MODEL_RED_COIN] = &fishGeo;
    objects[0].activeFlags = ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_BLINKING;
    objects[0].behavior = bhvRedCoin;
    objects[0].header.gfx.sharedChild = &fishGeo;
    vec3f_set(objects[0].header.gfx.pos, 0, 80, 0);
    vec3f_set(gMarioState->pos, 0, 0, 0);
    gCurGraphNodeObject = &objects[0].header.gfx;
    geo_append_display_list((void *) otherDl, LAYER_OCCLUDE_SILHOUETTE_ALPHA);
    EXPECT_EQ(master.listHeads[LAYER_TRANSPARENT]->blinkAlpha, 51);
    EXPECT_EQ(master.listHeads[LAYER_SILHOUETTE_ALPHA]->blinkAlpha, 0);
    EXPECT_EQ(master.listHeads[LAYER_SILHOUETTE_ALPHA]->silhouetteAlpha, 51);
    gCurGraphNodeObject = NULL;
    return TRUE;
}

static int door_screen_lod(void) {
    begin(0);
    root.width = SCREEN_WIDTH / 2;
    root.height = SCREEN_HEIGHT / 2;
    frustum.fov = 45;
    gEmulator = EMU_CONSOLE;
    gCurGraphNodeObject = &objects[0].header.gfx;
    Mat4 *matrix = &gMatStack[gMatStackIndex];
    (*matrix)[0][0] = (*matrix)[1][1] = (*matrix)[2][2] = 0.25f;
    (*matrix)[3][2] = -2600;
    struct GraphNodeAnimatedPart part = { 0 };
    part.node.type = GRAPH_NODE_TYPE_ANIMATED_PART;
    part.node.next = &part.node;
    part.displayList = (void *) door_seg3_dl_polished_wooden_door_near;
    struct GraphNodeLevelOfDetail range = { 0 };
    range.node.children = &part.node;
    range.minDistance = -2048;
    range.maxDistance = 3500;
    sLodChild = &part.node;
    lodVisits = 0;
    geo_process_level_of_detail(&range);
    EXPECT_EQ(lodVisits, 0);
    range.minDistance = 3500;
    range.maxDistance = 32767;
    part.displayList = (void *) door_seg3_dl_polished_wooden_door_far;
    geo_process_level_of_detail(&range);
    EXPECT_EQ(lodVisits, 1);
    (*matrix)[3][2] = -2499;
    EXPECT_EQ(geo_door_uses_far_mesh(&range), FALSE);
    (*matrix)[3][2] = -2600;
    (*matrix)[0][0] = (*matrix)[1][1] = (*matrix)[2][2] = 1;
    EXPECT_EQ(geo_door_uses_far_mesh(&range), FALSE);
    (*matrix)[0][0] = (*matrix)[1][1] = (*matrix)[2][2] = 0.25f;
    (*matrix)[3][0] = 10000;
    EXPECT_EQ(geo_door_uses_far_mesh(&range), FALSE);
    (*matrix)[3][0] = 0;
    frustum.fov = 0.001f;
    EXPECT_EQ(geo_door_uses_far_mesh(&range), FALSE);
    frustum.fov = 45;
    part.translation[1] = 10;
    EXPECT_EQ(geo_door_uses_far_mesh(&range), FALSE);
    part.translation[1] = 0;
    objects[0].header.gfx.animInfo.curAnim = (void *) otherDl;
    objects[0].behavior = bhvDoor;
    objects[0].header.gfx.animInfo.animID = DOOR_ANIM_CLOSED;
    EXPECT_EQ(geo_door_uses_far_mesh(&range), TRUE);
    objects[0].header.gfx.animInfo.animID = DOOR_ANIM_PULLED;
    EXPECT_EQ(geo_door_uses_far_mesh(&range), FALSE);
    // Both paired ranges must retain their original choice while opening.
    lodVisits = 0;
    geo_process_level_of_detail(&range);
    EXPECT_EQ(lodVisits, 0);
    range.minDistance = -2048;
    range.maxDistance = 3500;
    part.displayList = (void *) door_seg3_dl_polished_wooden_door_near;
    geo_process_level_of_detail(&range);
    EXPECT_EQ(lodVisits, 1);
    range.minDistance = 3500;
    range.maxDistance = 32767;
    part.displayList = (void *) door_seg3_dl_polished_wooden_door_far;
    objects[0].header.gfx.animInfo.animID = DOOR_ANIM_CLOSED;
    objects[0].behavior = NULL;
    EXPECT_EQ(geo_door_uses_far_mesh(&range), FALSE);
    objects[0].header.gfx.animInfo.curAnim = NULL;
    camera.rollScreen = 1;
    EXPECT_EQ(geo_door_uses_far_mesh(&range), FALSE);
    camera.rollScreen = 0;
    gEmulator = EMU_MUPEN;
    EXPECT_EQ(geo_door_uses_far_mesh(&range), FALSE);
    gEmulator = EMU_CONSOLE;
    part.displayList = (void *) otherDl;
    EXPECT_EQ(geo_door_uses_far_mesh(&range), FALSE);
    part.displayList = (void *) door_seg3_dl_polished_wooden_door_far;
    range.minDistance = 1000;
    EXPECT_EQ(geo_door_uses_far_mesh(&range), FALSE);
    range.minDistance = 3500;
    gCurGraphNodeHeldObject = (void *) &objects[1];
    EXPECT_EQ(geo_door_uses_far_mesh(&range), FALSE);
    gCurGraphNodeHeldObject = NULL;
    sLodChild = NULL;
    return TRUE;
}

static int shadow_texture_lod(void) {
    begin(0);
    sAspectRatio = 4.0f / 3.0f;
    root.width = SCREEN_WIDTH / 2;
    root.height = SCREEN_HEIGHT / 2;
    frustum.fov = 45;
    Vec3f position = { 0, 0, -1000 };
    EXPECT_EQ(geo_shadow_uses_small_texture(position, 40), FALSE);
    EXPECT_EQ(geo_shadow_uses_small_texture(position, 39), TRUE);
    position[0] = 1000;
    EXPECT_EQ(geo_shadow_uses_small_texture(position, 39), FALSE);
    position[0] = 0;
    position[2] = -20;
    EXPECT_EQ(geo_shadow_uses_small_texture(position, 39), FALSE);
    position[2] = -1000;
    hitboxView = TRUE;
    EXPECT_EQ(geo_shadow_uses_small_texture(position, 39), FALSE);
    hitboxView = FALSE;
    sAspectRatio = 16.0f / 9.0f;
    EXPECT_EQ(geo_shadow_uses_small_texture(position, 39), FALSE);
    sAspectRatio = 4.0f / 3.0f;
    gCurGraphNodeCamera = NULL;
    EXPECT_EQ(geo_shadow_uses_small_texture(position, 39), FALSE);
    gCurGraphNodeCamera = &camera;
    camera.rollScreen = 1;
    EXPECT_EQ(geo_shadow_uses_small_texture(position, 39), FALSE);
    camera.rollScreen = 0;
    root.height = 60;
    EXPECT_EQ(geo_shadow_uses_small_texture(position, 39), FALSE);
    root.height = SCREEN_HEIGHT / 2;
    frustum.fov = 0;
    EXPECT_EQ(geo_shadow_uses_small_texture(position, 39), FALSE);
    frustum.fov = 45;
    gCurGraphNodeObject = &objects[0].header.gfx;
    Gfx *dl = real_create_shadow_below_xyz(position, 10, 180, SHADOW_CIRCLE, FALSE);
    EXPECT_EQ(dl != NULL, TRUE);
    objects[0].behavior = bhvBowser;
    objects[0].oAction = BOWSER_ACT_TELEPORT;
    const s32 opacities[] = { 255, 192, 128, 64, 0, 64, 128, 192, 255 };
    for (unsigned fade = 0; fade < ARRAY_COUNT(opacities); fade++) {
        objects[0].oOpacity = opacities[fade];
        position[1] = 0;
        dl = real_create_shadow_below_xyz(position, 10, 180, SHADOW_CIRCLE, FALSE);
        EXPECT_EQ(dl != NULL, TRUE);
        EXPECT_EQ(dl[1].op, CMD_COMBINE);
        EXPECT_EQ(dl[1].indices[0], ENVIRONMENT);
        EXPECT_EQ(dl[1].indices[1], TEXEL0);
        EXPECT_EQ(dl[1].indices[2], ENVIRONMENT);
        for (int channel = 0; channel < 3; channel++) {
            EXPECT_EQ(dl[2].indices[channel], 255 - opacities[fade]);
        }
    }
    objects[0].oAction = BOWSER_ACT_DEFAULT;
    position[1] = 0;
    dl = real_create_shadow_below_xyz(position, 10, 180, SHADOW_CIRCLE, FALSE);
    EXPECT_EQ(dl[1].op, CMD_FOG_COLOR);
    objects[0].behavior = NULL;

#ifdef SHADOW_TEXTURE_REUSE
    EXPECT_EQ(dl[0].ptr == dl_shadow_circle, TRUE);
#else
    EXPECT_EQ(dl[0].ptr == dl_shadow_circle_32, TRUE);
    position[1] = 0;
    dl = real_create_shadow_below_xyz(position, 10, 180, SHADOW_SQUARE, FALSE);
    EXPECT_EQ(dl[0].ptr == dl_shadow_square_32, TRUE);
    gMarioObject = &objects[0];
    objects[0].header.gfx.animInfo.animID = MARIO_ANIM_WALKING;
    position[1] = 0;
    dl = real_create_shadow_below_xyz(position, 10, 180, SHADOW_CIRCLE, FALSE);
    EXPECT_EQ(dl[0].ptr == dl_shadow_circle, TRUE);
    gMarioObject = NULL;
    gCurGraphNodeObject = &gMirrorMario;
    position[1] = 0;
    dl = real_create_shadow_below_xyz(position, 10, 180, SHADOW_CIRCLE, FALSE);
    EXPECT_EQ(dl != NULL, TRUE);
    EXPECT_EQ(dl[0].ptr == dl_shadow_circle, TRUE);
    gCurGraphNodeObject = &objects[0].header.gfx;
    struct GraphNodeHeldObject held = { 0 };
    gCurGraphNodeHeldObject = &held;
    position[1] = 0;
    dl = real_create_shadow_below_xyz(position, 10, 180, SHADOW_CIRCLE, FALSE);
    EXPECT_EQ(dl != NULL, TRUE);
    EXPECT_EQ(dl[0].ptr == dl_shadow_circle, TRUE);
    gCurGraphNodeHeldObject = NULL;
    position[1] = 0;
    dl = real_create_shadow_below_xyz(position, 100, 180, SHADOW_CIRCLE, FALSE);
    EXPECT_EQ(dl != NULL, TRUE);
    EXPECT_EQ(dl[0].ptr == dl_shadow_circle, TRUE);
#endif
    return TRUE;
}

static int jrb_vertical_models(void) {
    begin(1);
    sAspectRatio = 4.0f / 3.0f;
    gCurrLevelNum = LEVEL_JRB;
    loaded[MODEL_SEAWEED] = &fishGeo;
    struct GraphNodeObject *node = &objects[0].header.gfx;
    node->sharedChild = &fishGeo;
    node->node.flags = GRAPH_RENDER_ACTIVE | GRAPH_RENDER_HAS_ANIMATION;
    node->animInfo.curAnim = (struct Animation *) &seaweed_seg6_anim_0600A4BC;
    vec3f_set(node->scale, 1, 1, 1);
    vec3f_set(node->cameraToObject, 0, 952, -1000);
    EXPECT_EQ(geo_jrb_object_outside_vertical_view(&objects[0]), FALSE);
    node->cameraToObject[1] = 953;
    EXPECT_EQ(geo_jrb_object_outside_vertical_view(&objects[0]), TRUE);
    node->cameraToObject[1] = -953;
    EXPECT_EQ(geo_jrb_object_outside_vertical_view(&objects[0]), TRUE);
    node->scale[1] = -2;
    EXPECT_EQ(geo_jrb_object_outside_vertical_view(&objects[0]), FALSE);
    node->scale[1] = 1;
    camera.rollScreen = 1;
    EXPECT_EQ(geo_jrb_object_outside_vertical_view(&objects[0]), FALSE);
    camera.rollScreen = 0;
    node->node.flags |= GRAPH_RENDER_SILHOUETTE;
    EXPECT_EQ(geo_jrb_object_outside_vertical_view(&objects[0]), FALSE);
    node->node.flags &= ~GRAPH_RENDER_SILHOUETTE;
    node->node.children = &fishGeo;
    EXPECT_EQ(geo_jrb_object_outside_vertical_view(&objects[0]), FALSE);
    node->node.children = NULL;
    struct Animation unknown = seaweed_seg6_anim_0600A4BC;
    node->animInfo.curAnim = &unknown;
    EXPECT_EQ(geo_jrb_object_outside_vertical_view(&objects[0]), FALSE);
    node->animInfo.curAnim = (struct Animation *) &seaweed_seg6_anim_0600A4BC;
    vec3f_set(node->pos, 0, 2000, -1000);
    int before = animationCalls;
    geo_process_object(&objects[0]);
    EXPECT_EQ(animationCalls, before);
    EXPECT_EQ(node->animInfo.animTimer, gAreaUpdateCounter);
    EXPECT_EQ(node->animInfo.animFrame, 1);
    EXPECT_EQ(nativeDraws, 0);
    EXPECT_EQ(gMatStackIndex, 0);
    EXPECT_EQ(node->throwMatrix == NULL, TRUE);
    EXPECT_EQ(gCurrAnimType, ANIM_TYPE_NONE);
    loaded[MODEL_SEAWEED] = NULL;
    loaded[MODEL_CLAM_SHELL] = &fishGeo;
    node->animInfo.curAnim = (struct Animation *) &clam_shell_seg5_anim_05001654;
    vec3f_set(node->cameraToObject, 0, 1174, -1000);
    EXPECT_EQ(geo_jrb_object_outside_vertical_view(&objects[0]), FALSE);
    node->cameraToObject[1] = 1175;
    EXPECT_EQ(geo_jrb_object_outside_vertical_view(&objects[0]), TRUE);
    node->animInfo.curAnim = (struct Animation *) &clam_shell_seg5_anim_0500172C;
    EXPECT_EQ(geo_jrb_object_outside_vertical_view(&objects[0]), TRUE);
    gCurrLevelNum = 0;
    EXPECT_EQ(geo_jrb_object_outside_vertical_view(&objects[0]), FALSE);
    return 1;
}

static int jrb_cave_visibility(void) {
    begin(1);
    struct GraphNodeSwitchCase selection = { 0 };
    gCurrLevelNum = LEVEL_JRB;
    vec3f_set(gMarioState->pos, -2263, 1331, 6468);
    vec3f_set(camera.pos, -2263, 1950, 7778);
    geo_switch_jrb_cave(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    struct GraphNodeObject *object = &objects[0].header.gfx;
    object->sharedChild = &fishGeo;
    object->node.flags = GRAPH_RENDER_ACTIVE | GRAPH_RENDER_HAS_ANIMATION;
    object->animInfo.curAnim = (struct Animation *) &seaweed_seg6_anim_0600A4BC;
    vec3f_set(object->pos, 0, -2966, -5000);
    geo_process_object(&objects[0]);
    EXPECT_EQ(nativeDraws, 0);
    EXPECT_EQ(animationCalls, 0);
    EXPECT_EQ(object->animInfo.animTimer, gAreaUpdateCounter);
    EXPECT_EQ(object->animInfo.animFrame, 1);
    EXPECT_EQ(gCurrAnimType, ANIM_TYPE_NONE);
    EXPECT_EQ(gMatStackIndex, 0);
    EXPECT_EQ(object->throwMatrix == NULL, TRUE);
    object->node.flags |= GRAPH_RENDER_BILLBOARD;
    geo_process_object(&objects[0]);
    EXPECT_EQ(nativeDraws, 0);
    EXPECT_EQ(animationCalls, 1);
    EXPECT_EQ(gMatStackIndex, 0);
    EXPECT_EQ(object->throwMatrix == NULL, TRUE);

    // Mario enables the room before the mouth; the camera keeps it on while leaving.
    vec3f_set(gMarioState->pos, 5060, -4420, 1600);
    geo_switch_jrb_cave(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    gMarioState->pos[2] = 1599;
    geo_switch_jrb_cave(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);
    object->node.flags &= ~GRAPH_RENDER_BILLBOARD;
    geo_process_object(&objects[0]);
    EXPECT_EQ(nativeDraws, 1);
    vec3f_copy(camera.pos, gMarioState->pos);
    vec3f_set(gMarioState->pos, -2263, 1331, 6468);
    geo_switch_jrb_cave(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);
    vec3f_set(camera.pos, -2263, 1950, 7778);
    vec3f_set(gMarioState->pos, -1800, -2812, -2100);
    geo_switch_jrb_cave(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);

    // Reloads and unknown camera contexts fail open; other areas/levels keep drawing.
    vec3f_set(gMarioState->pos, -2263, 1331, 6468);
    geo_switch_jrb_cave(GEO_CONTEXT_AREA_INIT, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);
    gCurGraphNodeCamera = NULL;
    geo_switch_jrb_cave(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);
    gCurGraphNodeCamera = &camera;
    geo_switch_jrb_cave(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    gEmulator = EMU_PROJECT64;
    EXPECT_EQ(obj_is_in_view(object), FALSE);
    object->areaIndex = 2;
    EXPECT_EQ(obj_is_in_view(object), TRUE);
    object->areaIndex = 1;
    gCurrLevelNum = 0;
    EXPECT_EQ(obj_is_in_view(object), TRUE);
    return 1;
}

static int jrb_offscreen_animation(void) {
    begin(1);
    gCurrLevelNum = LEVEL_JRB;
    gEmulator = EMU_CONSOLE;
    sAspectRatio = 4.0f / 3.0f;
    struct GraphNodeObject *node = &objects[0].header.gfx;
    node->node.flags = GRAPH_RENDER_ACTIVE | GRAPH_RENDER_HAS_ANIMATION;
    node->sharedChild = &fishGeo;
    node->animInfo.curAnim = (struct Animation *) &seaweed_seg6_anim_0600A4BC;
    vec3f_set(node->pos, 9000, 0, -1000);
    for (s32 frame = 1; frame <= 180; frame++) {
        gAreaUpdateCounter = frame;
        s32 paused = frame >= 50 && frame < 70;
        if (paused) {
            node->node.flags &= ~GRAPH_RENDER_HAS_ANIMATION;
        } else {
            node->node.flags |= GRAPH_RENDER_HAS_ANIMATION;
        }
        struct AnimInfo reference = node->animInfo;
        s32 expected = paused ? reference.animFrame
                              : geo_update_animation_frame(&reference, &reference.animFrameAccelAssist);
        geo_process_object(&objects[0]);
        EXPECT_EQ(node->animInfo.animFrame, expected);
        EXPECT_EQ(node->animInfo.animFrameAccelAssist, reference.animFrameAccelAssist);
        EXPECT_EQ(node->animInfo.animTimer, gAreaUpdateCounter);
        EXPECT_EQ(animationCalls, 0);
        EXPECT_EQ(nativeDraws, 0);
        EXPECT_EQ(node->throwMatrix == NULL, TRUE);
        // A second render in the same update must not advance animation twice.
        geo_process_object(&objects[0]);
        EXPECT_EQ(node->animInfo.animFrame, expected);
    }
    return TRUE;
}

static int jrb_camera_boxes(void) {
    begin(1);
    gEmulator = EMU_CONSOLE;
    sAspectRatio = 4.0f / 3.0f;
    Vec3f minimum = { -10, -10, -1010 };
    Vec3f maximum = { 10, 10, -990 };
    EXPECT_EQ(geo_camera_box_in_view(minimum, maximum), TRUE);
    minimum[0] = 800;
    maximum[0] = 820;
    EXPECT_EQ(geo_camera_box_in_view(minimum, maximum), FALSE);
    minimum[0] = 690;
    maximum[0] = 720;
    EXPECT_EQ(geo_camera_box_in_view(minimum, maximum), TRUE);
    minimum[0] = -10;
    maximum[0] = 10;
    minimum[1] = 600;
    maximum[1] = 620;
    EXPECT_EQ(geo_camera_box_in_view(minimum, maximum), FALSE);
    // Projection rolls separately from gCameraTransform. A quarter turn
    // moves this box into the wider horizontal field of view.
    for (s32 roll = -0x4000; roll <= 0x4000; roll += 0x8000) {
        camera.rollScreen = roll;
        EXPECT_EQ(geo_camera_box_in_view(minimum, maximum), TRUE);
    }
    camera.rollScreen = 0;
    EXPECT_EQ(geo_camera_box_in_view(minimum, maximum), FALSE);
    minimum[1] = -10;
    maximum[1] = 10;
    minimum[2] = 100;
    maximum[2] = 200;
    EXPECT_EQ(geo_camera_box_in_view(minimum, maximum), FALSE);
    minimum[2] = -200;
    EXPECT_EQ(geo_camera_box_in_view(minimum, maximum), TRUE);
    minimum[0] = NAN;
    EXPECT_EQ(geo_camera_box_in_view(minimum, maximum), TRUE);
    minimum[0] = -10;
    frustum.halfFovVertical = NAN;
    EXPECT_EQ(geo_camera_box_in_view(minimum, maximum), TRUE);
    frustum.halfFovVertical = 0.5f;
    gCurGraphNodeCamera = NULL;
    EXPECT_EQ(geo_camera_box_in_view(minimum, maximum), TRUE);
    return TRUE;
}

static int jrb_terrain_occlusion(void) {
    begin(1);
    gCurrLevelNum = LEVEL_JRB;
    loaded[MODEL_SEAWEED] = &fishGeo;
    struct GraphNodeObject *object = &objects[0].header.gfx;
    object->sharedChild = &fishGeo;
    object->node.flags = GRAPH_RENDER_ACTIVE | GRAPH_RENDER_HAS_ANIMATION;
    object->animInfo.curAnim = (struct Animation *) &seaweed_seg6_anim_0600A4BC;
    vec3f_set(camera.pos, -2263, 1950, 7778);
    vec3f_set(object->scale, 1, 1, 1);
    vec3f_set(object->pos, 3551, -5120, 2744);
    geo_jrb_prepare_occluders();
    EXPECT_EQ(geo_jrb_object_behind_terrain(object), TRUE);
    geo_process_object(&objects[0]);
    EXPECT_EQ(nativeDraws, 0);
    EXPECT_EQ(animationCalls, 0);
    EXPECT_EQ(object->animInfo.animTimer, gAreaUpdateCounter);
    EXPECT_EQ(object->animInfo.animFrame, 1);
    EXPECT_EQ(object->throwMatrix == NULL, TRUE);
    EXPECT_EQ(gMatStackIndex, 0);
    // Partial cover, attachments, unknown animation and invalid scale fail open.
    object->pos[1] = 400;
    EXPECT_EQ(geo_jrb_object_behind_terrain(object), FALSE);
    vec3f_set(object->pos, 3551, -5120, 2744);
    object->node.children = &fishGeo;
    EXPECT_EQ(geo_jrb_object_behind_terrain(object), FALSE);
    object->node.children = NULL;
    object->animInfo.curAnim = NULL;
    EXPECT_EQ(geo_jrb_object_behind_terrain(object), FALSE);
    object->animInfo.curAnim = (struct Animation *) &seaweed_seg6_anim_0600A4BC;
    object->scale[0] = NAN;
    EXPECT_EQ(geo_jrb_object_behind_terrain(object), FALSE);
    object->scale[0] = 1;
    vec3f_set(camera.pos, -2263, -1500, 7778);
    geo_jrb_prepare_occluders();
    EXPECT_EQ(geo_jrb_object_behind_terrain(object), FALSE);
    struct GraphNodeSwitchCase selection = { 0 };
    geo_switch_jrb_bay(GEO_CONTEXT_AREA_INIT, &selection.fnNode.node, NULL);
    EXPECT_EQ(geo_jrb_object_behind_terrain(object), FALSE);
    return 1;
}

static int jrb_bay_visibility(void) {
    begin(1);
    gCurrLevelNum = LEVEL_JRB;
    struct GraphNodeSwitchCase selection = { 0 };
    vec3f_set(gMarioState->pos, 5200, -2966, -6000);
    vec3f_set(camera.pos, 5200, -2450, -4300);
    // Looking deeper into the cave puts the padded exit behind the camera.
    mtxf_identity(gCameraTransform);
    gCameraTransform[3][0] = -5200;
    gCameraTransform[3][1] = 2450;
    gCameraTransform[3][2] = 4300;
    geo_switch_jrb_bay(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    struct GraphNodeObject *object = &objects[0].header.gfx;
    object->sharedChild = &fishGeo;
    vec3f_set(object->pos, 0, -1000, 4000);
    vec3f_set(object->cameraToObject, 0, 0, -4000);
    EXPECT_EQ(obj_is_in_view(object), FALSE);
    vec3f_set(object->pos, 0, -2966, -5000);
    EXPECT_EQ(obj_is_in_view(object), TRUE);
    // Turning back toward the exit restores the bay before it can be seen.
    gCameraTransform[2][2] = -1;
    gCameraTransform[3][2] = -4300;
    geo_switch_jrb_bay(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);
    camera.pos[2] = -1900;
    geo_switch_jrb_bay(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);
    gCurGraphNodeCamera = NULL;
    geo_switch_jrb_bay(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);
    geo_switch_jrb_bay(GEO_CONTEXT_AREA_INIT, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);
    return 1;
}

static int jrb_bay_terrain(void) {
    begin(1);
    gCurrLevelNum = LEVEL_JRB;
    root.height = SCREEN_HEIGHT / 2;
    struct GraphNodeSwitchCase bay = { 0 };
    struct GraphNodeSwitchCase terrain = { 0 };
    vec3f_set(gMarioState->pos, 5200, -2966, -6000);
    const f32 positions[][3] = { { 5200, -2450, -4300 }, { 5200, -3300, -2500 }, { 5200, -4100, -1200 },
                                 { 5200, -4100, -800 },  { 5200, -3300, -1900 }, { 4950, -2450, -4300 },
                                 { 5350, -2450, -4300 }, { 5200, -2300, -4300 }, { 5200, -2500, -1200 },
                                 { 5200, -4750, -800 },  { 5200, -3500, -3100 }, { 5200, -2900, -4300 },
                                 { 5200, -2450, -7500 }, { 5200, -2450, -4300 } };
    const s32 expected[] = { 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    for (u32 i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        vec3f_set(camera.pos, positions[i][0], positions[i][1], positions[i][2]);
        geo_switch_jrb_bay(GEO_CONTEXT_RENDER, &bay.fnNode.node, NULL);
        // All three roots reuse the current pass's selection, even in one frame.
        for (s32 part = 0; part < 3; part++) {
            geo_switch_jrb_bay_terrain(GEO_CONTEXT_RENDER, &terrain.fnNode.node, NULL);
            EXPECT_EQ(terrain.selectedCase, expected[i]);
        }
    }
    camera.rollScreen = 1;
    geo_switch_jrb_bay(GEO_CONTEXT_RENDER, &bay.fnNode.node, NULL);
    geo_switch_jrb_bay_terrain(GEO_CONTEXT_RENDER, &terrain.fnNode.node, NULL);
    EXPECT_EQ(terrain.selectedCase, 0);
    camera.rollScreen = 0;
    root.areaIndex = 2;
    geo_switch_jrb_bay(GEO_CONTEXT_RENDER, &bay.fnNode.node, NULL);
    EXPECT_EQ(sJrbBayTerrain, 0);
    root.areaIndex = 1;
    gCurrLevelNum = 0;
    geo_switch_jrb_bay(GEO_CONTEXT_RENDER, &bay.fnNode.node, NULL);
    EXPECT_EQ(sJrbBayTerrain, 0);
    gCurrLevelNum = LEVEL_JRB;
    frustum.halfFovVertical = NAN;
    geo_switch_jrb_bay(GEO_CONTEXT_RENDER, &bay.fnNode.node, NULL);
    EXPECT_EQ(sJrbBayTerrain, 0);
    frustum.halfFovVertical = 0.5f;
    gMarioState->pos[1] = -1942;
    geo_switch_jrb_bay(GEO_CONTEXT_RENDER, &bay.fnNode.node, NULL);
    EXPECT_EQ(sJrbBayTerrain, 0);
    gMarioState->pos[1] = -2966;
    geo_switch_jrb_bay(GEO_CONTEXT_RENDER, &bay.fnNode.node, NULL);
    EXPECT_EQ(sJrbBayTerrain, 1);
    geo_switch_jrb_bay_terrain(GEO_CONTEXT_AREA_INIT, &terrain.fnNode.node, NULL);
    EXPECT_EQ(terrain.selectedCase, 0);
    geo_switch_jrb_bay(GEO_CONTEXT_AREA_INIT, &bay.fnNode.node, NULL);
    geo_switch_jrb_bay_terrain(GEO_CONTEXT_RENDER, &terrain.fnNode.node, NULL);
    EXPECT_EQ(terrain.selectedCase, 0);
    gCurGraphNodeCamera = NULL;
    geo_switch_jrb_bay(GEO_CONTEXT_RENDER, &bay.fnNode.node, NULL);
    EXPECT_EQ(sJrbBayTerrain, 0);
    return 1;
}

static int jrb_ship_lod(void) {
    begin(1);
    struct GraphNodeSwitchCase selection = { 0 };
    struct GraphNodeObject *object = &objects[0].header.gfx;
    gCurGraphNodeObject = object;
    gCurrLevelNum = LEVEL_JRB;
    object->areaIndex = 1;
    object->node.flags = GRAPH_RENDER_ACTIVE;
    root.height = SCREEN_HEIGHT / 2;
    frustum.fov = 45;
    vec3f_set(object->scale, 1, 1, 1);
    vec3f_set(object->cameraToObject, 0, 0, -7499);
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    object->cameraToObject[2] = -7500;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);
    object->cameraToObject[2] = -9999;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);
    object->cameraToObject[2] = -10000;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 2);
    object->cameraToObject[2] = -19999;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 2);
    object->cameraToObject[2] = -20000;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 3);
    object->cameraToObject[2] = -10000;
    object->scale[0] = -2;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    object->cameraToObject[2] = -20000;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 2);
    frustum.fov = 44;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    frustum.fov = 45;
    object->node.children = &fishGeo;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    object->node.children = NULL;
    object->animInfo.curAnim = (void *) otherDl;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    object->animInfo.curAnim = NULL;
    object->scale[0] = NAN;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    object->scale[0] = 1;
    object->areaIndex = 2;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    object->areaIndex = 1;
    camera.rollScreen = 1;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    camera.rollScreen = 0;
    geo_switch_jrb_ship_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 3);
    geo_switch_jrb_ship_lod(GEO_CONTEXT_AREA_INIT, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    return 1;
}

static int seaweed_screen_lod(void) {
    begin(1);
    struct GraphNodeSwitchCase selection = { 0 };
    struct GraphNodeObject *object = &objects[0].header.gfx;
    gCurGraphNodeObject = object;
    root.height = SCREEN_HEIGHT / 2;
    object->node.flags = GRAPH_RENDER_ACTIVE | GRAPH_RENDER_HAS_ANIMATION;
    object->node.children = NULL;
    object->animInfo.curAnim = (struct Animation *) &seaweed_seg6_anim_0600A4BC;
    vec3f_set(object->scale, 1, 1, 1);
    // At slope 0.5, the 300-unit conservative sphere is 32 pixels at depth 4800.
    vec3f_set(object->cameraToObject, 0, 0, -6000);
    geo_switch_seaweed_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    object->cameraToObject[2] = -6001;
    geo_switch_seaweed_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);
    object->scale[0] = -2;
    geo_switch_seaweed_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    object->cameraToObject[2] = -9600;
    geo_switch_seaweed_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);
    object->cameraToObject[2] = -9599;
    geo_switch_seaweed_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    object->cameraToObject[2] = -12000;
    object->node.children = &fishGeo;
    geo_switch_seaweed_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    object->node.children = NULL;
    struct Animation unknown = seaweed_seg6_anim_0600A4BC;
    object->animInfo.curAnim = &unknown;
    geo_switch_seaweed_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    object->animInfo.curAnim = (struct Animation *) &seaweed_seg6_anim_0600A4BC;
    camera.rollScreen = 1;
    geo_switch_seaweed_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    camera.rollScreen = 0;
    object->scale[0] = NAN;
    geo_switch_seaweed_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    object->scale[0] = 1;
    geo_switch_seaweed_lod(GEO_CONTEXT_RENDER, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 1);
    geo_switch_seaweed_lod(GEO_CONTEXT_AREA_INIT, &selection.fnNode.node, NULL);
    EXPECT_EQ(selection.selectedCase, 0);
    return 1;
}

int main(int argc, char **argv) {
    const struct {
        const char *name;
        int (*run)(void);
    } tests[] = {
        { "render.batch_sizes", batch_sizes },
        { "render.ordinary_sprite_policy", ordinary_sprite_policy },
        { "render.batch_positions", batch_positions },
        { "render.simple_mario_lod", simple_mario_lod },
        { "render.door_screen_lod", door_screen_lod },
        { "render.character_silhouette", character_silhouette },
        { "render.mario_silhouette_geometry", mario_silhouette_geometry },
        { "render.mario_damage_blink_silhouette", mario_damage_blink_silhouette },
        { "render.mario_silhouette_option", mario_silhouette_option },
        { "render.mario_wall_shadow", mario_wall_shadow },
        { "render.mario_silhouette_lod", mario_silhouette_lod },
        { "render.mario_floor_lift", mario_floor_lift },
        { "render.collectible_silhouette", collectible_silhouette },
        { "render.master_list_effects", master_list_effects },
        { "render.one_up_cutout", one_up_cutout },
        { "render.blink_runs", blink_runs },
        { "render.shadow_texture_runs", shadow_texture_runs },
        { "render.sprite_texture_runs", sprite_texture_runs },
        { "render.particle_texture_callbacks", particle_texture_callbacks },
        { "render.shadow_texture_lod", shadow_texture_lod },
        { "render.draw_order", draw_order },
        { "render.reflection_upload", reflection_upload },
        { "render.culling_edges", culling_edges },
        { "render.vertical_particles", vertical_particles },
        { "render.particle_camera_position_reuse", particle_camera_position_reuse },
        { "render.vertical_sprite_switches", vertical_sprite_switches },
        { "render.vertical_trees", vertical_trees },
        { "render.jrb_vertical_models", jrb_vertical_models },
        { "render.jrb_cave_visibility", jrb_cave_visibility },
        { "render.jrb_camera_boxes", jrb_camera_boxes },
        { "render.jrb_offscreen_animation", jrb_offscreen_animation },
        { "render.jrb_terrain_occlusion", jrb_terrain_occlusion },
        { "render.jrb_bay_visibility", jrb_bay_visibility },
        { "render.jrb_bay_terrain", jrb_bay_terrain },
        { "render.jrb_ship_lod", jrb_ship_lod },
        { "render.seaweed_screen_lod", seaweed_screen_lod },
        { "render.mario_shadow_distance_bias", mario_shadow_distance_bias },
        { "render.shadow_footprint_culling", shadow_footprint_culling },
        { "render.shadow_culling_child_state", shadow_culling_child_state },
        { "render.offscreen_state", offscreen_state },
        { "render.fallback", fallback },
        { "render.distant_rebase", distant_rebase },
        { "render.pause_and_flush", pause_and_flush },
        { "render.other_small_sprites_batch", other_small_sprites_batch },
        { "render.coloured_droplets_batch", coloured_droplets_batch },
        { "render.compact_bubbles_batch", compact_bubbles_batch },
        { "render.visible_fish_matrices", visible_fish_matrices },
        { "render.matrix_stack_reuse", matrix_stack_reuse },
        { "render.undrawn_matrices", undrawn_matrices },
        { "render.lab_shadow_distance", lab_shadow_distance },
        { "render.object_blink_phases", object_blink_phases },
        { "render.cap_blink_rendering", cap_blink_rendering },
        { "render.mario_alpha_state_cleanup", mario_alpha_state_cleanup },
        { "render.mario_aligned_dither", mario_aligned_dither },
        { "render.cap_blink_opacity", cap_blink_opacity },
        { "render.object_blink_rendering", object_blink_rendering },
        { "render.arena_enemy_fade", arena_enemy_fade_rendering },
        { "render.bowser_flame_billboards", bowser_flame_billboards },
    };
    test_init(argc, argv);
    if (test_selected("render.pose_owner")) {
        test_report("render.pose_owner", pose_owner());
    }
    for (unsigned i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        if (test_selected(tests[i].name)) {
            test_report(tests[i].name, tests[i].run());
        }
    }
    begin(0);
    return test_finish();
}
