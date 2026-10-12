/* Production head controls and shipped vertices; supplied input and graphics allocation. */
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef int8_t s8;
typedef int32_t s32;
typedef uint32_t u32;
typedef int16_t s16;
typedef uint16_t u16;
typedef uint8_t u8;
typedef float f32;
typedef float Vec3f[3];
typedef union {
    struct {
        s16 ob[3];
        u16 flag;
        s16 tc[2];
        u8 cn[4];
    } v;
} Vtx;
typedef struct {
    struct {
        uintptr_t w0, w1;
    } words;
} Gfx;
typedef int Mtx;
struct GraphNode {
    s32 flags;
};
#define TRUE 1
#define FALSE 0
#define UNUSED __attribute__((unused))
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_CENTER_X 160
#define SCREEN_CENTER_Y 120
#define GEO_CONTEXT_CREATE 0
#define GEO_CONTEXT_RENDER 1
#define A_BUTTON 0x8000
#define R_CBUTTONS 1
#define L_CBUTTONS 2
#define D_CBUTTONS 4
#define U_CBUTTONS 8
#define SOUND_MENU_FLAG_HANDAPPEAR 1
#define SOUND_MENU_FLAG_PINCHMARIOFACE 8
#define SOUND_MENU_FLAG_LETGOMARIOFACE 32
static int handSounds, pinchSounds, releaseSounds;
static void play_menu_sounds(s16 flag) {
    if (flag == 1) {
        handSounds++;
    } else if (flag == 8) {
        pinchSounds++;
    } else {
        assert(flag == 32);
        releaseSounds++;
    }
}
static float sins(s16 angle) {
    return sinf(angle * 3.14159265f / 32768);
}
static float coss(s16 angle) {
    return cosf(angle * 3.14159265f / 32768);
}
#define G_VTX 1
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define VIRTUAL_TO_PHYSICAL(p) ((uintptr_t) (p))
#define SET_GRAPH_NODE_LAYER(flags, layer) ((flags) = (layer))
#define LAYER_OPAQUE 1
#define LAYER_ALPHA 4
#define gSPPerspNormalize(p, ...) ((void) (p))
#define gSPMatrix(p, ...) ((void) (p))
#define gSPClearGeometryMode(p, ...) ((void) (p))
#define gSPDisplayList(p, ...) ((void) (p))
#define gSPSetGeometryMode(p, ...) ((void) (p))
#define gSPEndDisplayList(p, ...) ((void) (p))
#define guOrtho(...) ((void) 0)
#define guTranslate(...) ((void) 0)
#define bcopy(source, dest, size) memcpy(dest, source, size)
#include "head_definitions.inc"
#include "head_vertices.inc"

static Gfx sCommands[ARRAY_COUNT(intro_simpleflips_head_groups) * 2 + 1];
static struct SimpleFlipsHeadMesh intro_simpleflips_head_mesh = {
    sCommands,
    intro_simpleflips_head_groups,
    ARRAY_COUNT(sCommands),
    ARRAY_COUNT(intro_simpleflips_head_groups),
    0,
};
static struct {
    s16 rawStickX, rawStickY;
    u16 buttonDown;
} sController, *gPlayer1Controller = &sController;
static struct {
    s32 isActive;
} gWarpTransition;
#ifndef HEAD_TEST_NO_WIDE
#define WIDE
#endif
#define LEVEL_UNKNOWN_1 1
static s32 gCurrLevelNum = 9;
static struct {
    s32 widescreen;
} gConfig;
static s32 sEnabled = TRUE;
static union {
    uint64_t align;
    u8 bytes[8192];
} sPool;
static size_t sAllocated;
static s32 jrb_simpleflips_is_game_over(void) {
    return sEnabled;
}
static void *segmented_to_virtual(const void *pointer) {
    return (void *) pointer;
}
static void *alloc_display_list(size_t size) {
    size = (size + 7) & ~(size_t) 7;
    assert(sAllocated + size <= sizeof(sPool));
    void *memory = sPool.bytes + sAllocated;
    sAllocated += size;
    return memory;
}
#include "head_actual.inc"

static Gfx *frame(struct GraphNode *node) {
    sAllocated = 0;
    return geo_simpleflips_head(GEO_CONTEXT_RENDER, node, NULL);
}

static void check_vertices(Gfx *list, s32 stretched) {
    s32 changed = 0;

    for (u32 group = 0; group < ARRAY_COUNT(intro_simpleflips_head_groups); group++) {
        const Vtx *original = intro_simpleflips_head_groups[group].vertices;
        const Vtx *drawn = (void *) list[group * 2 + 1].words.w1;
        assert(list[group * 2].words.w0 == sCommands[group * 2].words.w0);
        for (u32 index = 0; index < intro_simpleflips_head_groups[group].count; index++) {
            assert(drawn[index].v.flag == original[index].v.flag);
            assert(memcmp(drawn[index].v.tc, original[index].v.tc, 4) == 0);
            assert(drawn[index].v.cn[3] == original[index].v.cn[3]);
            if (sHeadYaw == 0 && sHeadPitch == 0) {
                assert(memcmp(drawn[index].v.cn, original[index].v.cn, 4) == 0);
                assert(drawn[index].v.ob[2] == original[index].v.ob[2]);
            }
            if (drawn[index].v.ob[0] != original[index].v.ob[0]
                || drawn[index].v.ob[1] != original[index].v.ob[1]) {
                changed++;
            }
            // Duplicated vertices at material boundaries must move identically.
            for (u32 otherGroup = 0; otherGroup <= group; otherGroup++) {
                const Vtx *otherOriginal = intro_simpleflips_head_groups[otherGroup].vertices;
                const Vtx *otherDrawn = (void *) list[otherGroup * 2 + 1].words.w1;
                for (u32 other = 0; other < intro_simpleflips_head_groups[otherGroup].count; other++) {
                    if (memcmp(original[index].v.ob, otherOriginal[other].v.ob, 6) == 0) {
                        assert(memcmp(drawn[index].v.ob, otherDrawn[other].v.ob, 6) == 0);
                    }
                }
            }
        }
    }
    assert((changed != 0) == stretched);
}

static void check_projected_head_grabs(struct GraphNode *node) {
    for (s32 mode = 0; mode < 3; mode++) {
        gConfig.widescreen = mode != 0;
        gCurrLevelNum = mode == 2 ? LEVEL_UNKNOWN_1 : 9;
        f32 aspect = 4.0f / 3.0f;
#ifdef WIDE
        if (mode == 1) {
            aspect = 16.0f / 9.0f;
        }
#endif
        f32 focalY = SCREEN_CENTER_Y / tanf(45.0f * 3.14159265f / 360.0f);
        f32 focalX = SCREEN_CENTER_X / (aspect * tanf(45.0f * 3.14159265f / 360.0f));
        for (s32 x = 80; x <= 240; x += 20) {
            geo_simpleflips_head(GEO_CONTEXT_CREATE, node, NULL);
            sController.rawStickX = sController.rawStickY = 0;
            sCursorScreenX = x;
            sCursorScreenY = 140;
            const Vtx *nearest = NULL;
            f32 distance = 24.0f * 24.0f;
            for (u32 group = 0; group < intro_simpleflips_head_mesh.groupCount; group++) {
                const struct SimpleFlipsHeadVertexGroup *part = &intro_simpleflips_head_groups[group];
                for (u32 index = 0; index < part->count; index++) {
                    const Vtx *vertex = &part->vertices[index];
                    if ((s8) vertex->v.cn[2] < 0) {
                        continue;
                    }
                    f32 depth = 1000.0f - vertex->v.ob[2] * 1.5f;
                    f32 drawnX = SCREEN_CENTER_X + vertex->v.ob[0] * 1.5f * focalX / depth;
                    f32 drawnY = SCREEN_CENTER_Y + vertex->v.ob[1] * 1.5f * focalY / depth;
                    f32 d = (drawnX - x) * (drawnX - x) + (drawnY - 140) * (drawnY - 140);
                    if (d < distance) {
                        distance = d;
                        nearest = vertex;
                    }
                }
            }
            sController.buttonDown = A_BUTTON;
            frame(node);
            if (sGrabbing != (nearest != NULL)) {
                fprintf(stderr, "Head pick mismatch: aspect=%g cursor=(%d,140)\n", aspect, x);
            }
            assert(sGrabbing == (nearest != NULL));
            if (nearest != NULL) {
                for (s32 axis = 0; axis < 3; axis++) {
                    assert(sModelGrabPoint[axis] == nearest->v.ob[axis]);
                }
                sController.rawStickX = 32;
                frame(node);
                f32 depth = 1000.0f - nearest->v.ob[2] * 1.5f;
                assert(fabsf(sModelPullX * 1.5f * focalX / depth - 4.0f) < 0.001f);
            }
        }
    }
    gConfig.widescreen = FALSE;
    gCurrLevelNum = 9;
}

int main(void) {
    struct GraphNode node = { 0 };
    Gfx *list;

    for (u32 group = 0; group < ARRAY_COUNT(intro_simpleflips_head_groups); group++) {
        sCommands[group * 2].words.w0 = 0xAB000000;
        sCommands[group * 2 + 1].words.w0 = G_VTX << 24;
        sCommands[group * 2 + 1].words.w1 = (uintptr_t) intro_simpleflips_head_groups[group].vertices;
        intro_simpleflips_head_mesh.vertexCount += intro_simpleflips_head_groups[group].count;
    }
    geo_simpleflips_head(GEO_CONTEXT_CREATE, &node, NULL);
    assert(frame(&node) == sCommands && sAllocated == 0);
    for (s32 cycle = 0; cycle < 3; cycle++) {
        sCursorScreenX = SCREEN_CENTER_X;
        sCursorScreenY = 70;
        sController.buttonDown = A_BUTTON;
        frame(&node);
        assert(sGrabbing);
        sController.rawStickX = 64;
        sController.rawStickY = 16;
        for (s32 step = 0; step < 12; step++) {
            list = frame(&node);
        }
        assert(sCursorScreenX <= SCREEN_WIDTH - 28 && sModelPullX <= HEAD_MAX_PULL_MODEL_UNITS);
        check_vertices(list, TRUE);
        check_vertices(sCommands, FALSE);
        sController.rawStickX = sController.rawStickY = 0;
        sController.buttonDown = 0;
        for (s32 step = 0; step < 60; step++) {
            list = frame(&node);
        }
        assert(!sGrabbing && list == sCommands);
        check_vertices(list, FALSE);
    }
    sCursorScreenX = 28;
    sCursorScreenY = 30;
    sController.buttonDown = A_BUTTON;
    assert(frame(&node) == sCommands && !sGrabbing);
    sController.buttonDown = 0;
    frame(&node);
    sCursorScreenX = SCREEN_CENTER_X;
    sCursorScreenY = 70;
    gWarpTransition.isActive = TRUE;
    sController.buttonDown = A_BUTTON;
    assert(frame(&node) == sCommands && !sGrabbing);
    gWarpTransition.isActive = FALSE;
    assert(frame(&node) == sCommands && !sGrabbing);
    assert(handSounds == 1 && pinchSounds == 3 && releaseSounds == 3);
    sController.buttonDown = R_CBUTTONS | U_CBUTTONS;
    for (s32 step = 0; step < 40; step++) {
        list = frame(&node);
    }
    assert(sHeadYaw == 0x5000 && sHeadPitch == -0x3800);
    check_vertices(list, TRUE);
    check_vertices(sCommands, FALSE);
    sController.buttonDown = 0;
    frame(&node);
    // Choose a visible rotated vertex; screen-space picking must still grab it.
    s32 found = FALSE;
    for (u32 group = 0; group < ARRAY_COUNT(intro_simpleflips_head_groups) && !found; group++) {
        const Vtx *vertices = intro_simpleflips_head_groups[group].vertices;
        for (u32 index = 0; index < intro_simpleflips_head_groups[group].count; index++) {
            Vec3f point, normal;
            rotate_head_point(vertices[index].v.ob[0], vertices[index].v.ob[1], vertices[index].v.ob[2],
                              point);
            rotate_head_point((s8) vertices[index].v.cn[0], (s8) vertices[index].v.cn[1],
                              (s8) vertices[index].v.cn[2], normal);
            f32 scale = SIMPLEFLIPS_HEAD_SCALE * SIMPLEFLIPS_HEAD_FOCAL_LENGTH
                        / (SIMPLEFLIPS_HEAD_CAMERA_DISTANCE - point[2] * SIMPLEFLIPS_HEAD_SCALE);
            f32 x = SCREEN_CENTER_X + point[0] * scale;
            f32 y = SCREEN_CENTER_Y + point[1] * scale;
            if (normal[2] > 0 && x > 30 && x < 280 && y > 32 && y < 200) {
                sCursorScreenX = x;
                sCursorScreenY = y;
                found = TRUE;
                break;
            }
        }
    }
    assert(found);
    sController.buttonDown = A_BUTTON;
    frame(&node);
    assert(sGrabbing && pinchSounds == 4);
    sController.rawStickX = 32;
    for (s32 step = 0; step < 8; step++) {
        list = frame(&node);
    }
    assert(fabsf(sModelPullZ) > 1 && sHeadYaw == 0x5000);
    check_vertices(list, TRUE);
    sController.buttonDown = 0;
    sController.rawStickX = 0;
    for (s32 step = 0; step < 70; step++) {
        frame(&node);
    }
    assert(!sGrabbing && sModelPullZ == 0 && releaseSounds == 4);
    geo_simpleflips_head(GEO_CONTEXT_CREATE, &node, NULL);
    assert(frame(&node) == sCommands && sHeadYaw == 0 && sHeadPitch == 0);
    check_projected_head_grabs(&node);
    sEnabled = FALSE;
    assert(frame(&node) == NULL && sAllocated == 0);
    assert(geo_simpleflips_head_cursor(GEO_CONTEXT_RENDER, &node, NULL) == NULL);
    puts("PASS Toad head: rotation, rotated picking, stretch recovery, mesh seams and sound edges");
    return 0;
}
