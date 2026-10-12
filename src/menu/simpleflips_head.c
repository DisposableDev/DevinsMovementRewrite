#include <ultra64.h>

#include "sm64.h"
#include "simpleflips_head.h"
#include "game/area.h"
#include "game/game_init.h"
#include "game/jrb_simpleflips.h"
#include "game/memory.h"
#include "game/main.h"
#include "level_table.h"
#include "game/sound_init.h"
#include "engine/math_util.h"
#include "levels/intro/header.h"

#define HEAD_GRAB_RADIUS_PIXELS 24.0f
#define HEAD_STRETCH_RADIUS_MODEL_UNITS 140.0f
#define HEAD_MAX_PULL_MODEL_UNITS 180.0f
#define HEAD_RELEASE_DECAY 0.8f

static Vec3f sModelGrabPoint;
static f32 sCursorScreenX;
static f32 sCursorScreenY;
static f32 sDragAnchorScreenX;
static f32 sDragAnchorScreenY;
static f32 sModelPullX;
static f32 sModelPullY;
static f32 sModelPullZ;
static s16 sHeadYaw;
static s16 sHeadPitch;
static f32 sYawSin, sYawCos, sPitchSin, sPitchCos;
static u8 sHeadShown;
static u8 sGrabbing;
static u8 sAWasDown;

static f32 clamp_head_value(f32 value, f32 minimum, f32 maximum) {
    if (value < minimum) {
        return minimum;
    }
    if (value > maximum) {
        return maximum;
    }
    return value;
}

// The cursor uses 320 screen units while the perspective stretches to 16:9.
static f32 head_horizontal_projection_scale(void) {
#ifdef WIDE
    if (gConfig.widescreen && gCurrLevelNum != LEVEL_UNKNOWN_1) {
        return 0.75f;
    }
#endif
    return 1.0f;
}

// Rotate both positions and lighting normals; stretching stays in model space.
static void rotate_head_point(f32 x, f32 y, f32 z, Vec3f result) {
    f32 depth = -x * sYawSin + z * sYawCos;

    result[0] = x * sYawCos + z * sYawSin;
    result[1] = y * sPitchCos - depth * sPitchSin;
    result[2] = y * sPitchSin + depth * sPitchCos;
}

// View-aligned pull uses model units; store its inverse rotation for deformation.
static void set_head_pull_from_view(f32 viewX, f32 viewY) {
    f32 depth = -viewY * sPitchSin;

    sModelPullX = viewX * sYawCos - depth * sYawSin;
    sModelPullY = viewY * sPitchCos;
    sModelPullZ = viewX * sYawSin + depth * sYawCos;
}

static f32 head_pull_weight(const Vtx *vertex) {
    f32 x = vertex->v.ob[0] - sModelGrabPoint[0];
    f32 y = vertex->v.ob[1] - sModelGrabPoint[1];
    f32 z = vertex->v.ob[2] - sModelGrabPoint[2];
    f32 weight =
        1.0f
        - (x * x + y * y + z * z) / (HEAD_STRETCH_RADIUS_MODEL_UNITS * HEAD_STRETCH_RADIUS_MODEL_UNITS);

    if (weight <= 0.0f) {
        return 0.0f;
    }
    return weight * weight;
}

static void grab_head(const struct SimpleFlipsHeadMesh *mesh) {
    const struct SimpleFlipsHeadVertexGroup *groups = segmented_to_virtual(mesh->groups);
    const Vtx *picked = NULL;
    f32 nearestDistanceSquared = HEAD_GRAB_RADIUS_PIXELS * HEAD_GRAB_RADIUS_PIXELS;
    s32 groupIndex;
    s32 vertexIndex;

    for (groupIndex = 0; groupIndex < mesh->groupCount; groupIndex++) {
        const Vtx *vertices = segmented_to_virtual(groups[groupIndex].vertices);

        for (vertexIndex = 0; vertexIndex < groups[groupIndex].count; vertexIndex++) {
            const Vtx *vertex = &vertices[vertexIndex];
            f32 pixelsPerModelUnit;
            f32 x;
            f32 y;
            f32 distanceSquared;
            f32 weight = head_pull_weight(vertex);
            Vec3f point;
            Vec3f normal;

            rotate_head_point((s8) vertex->v.cn[0], (s8) vertex->v.cn[1], (s8) vertex->v.cn[2], normal);
            if (normal[2] < 0) {
                continue;
            }
            rotate_head_point(vertex->v.ob[0] + sModelPullX * weight,
                              vertex->v.ob[1] + sModelPullY * weight,
                              vertex->v.ob[2] + sModelPullZ * weight, point);
            pixelsPerModelUnit =
                SIMPLEFLIPS_HEAD_SCALE * SIMPLEFLIPS_HEAD_FOCAL_LENGTH
                / (SIMPLEFLIPS_HEAD_CAMERA_DISTANCE - point[2] * SIMPLEFLIPS_HEAD_SCALE);
            x = SCREEN_CENTER_X + point[0] * pixelsPerModelUnit * head_horizontal_projection_scale();
            y = SCREEN_CENTER_Y + point[1] * pixelsPerModelUnit;
            distanceSquared = (x - sCursorScreenX) * (x - sCursorScreenX)
                              + (y - sCursorScreenY) * (y - sCursorScreenY);
            if (distanceSquared < nearestDistanceSquared) {
                nearestDistanceSquared = distanceSquared;
                picked = vertex;
            }
        }
    }
    if (picked != NULL) {
        f32 weight = head_pull_weight(picked);
        f32 pixelsPerModelUnit;
        Vec3f point;
        Vec3f pull;

        rotate_head_point(picked->v.ob[0] + sModelPullX * weight,
                          picked->v.ob[1] + sModelPullY * weight,
                          picked->v.ob[2] + sModelPullZ * weight, point);
        rotate_head_point(sModelPullX, sModelPullY, sModelPullZ, pull);
        pixelsPerModelUnit = SIMPLEFLIPS_HEAD_SCALE * SIMPLEFLIPS_HEAD_FOCAL_LENGTH
                             / (SIMPLEFLIPS_HEAD_CAMERA_DISTANCE - point[2] * SIMPLEFLIPS_HEAD_SCALE);
        // Account for existing deformation when placing the drag anchor.
        sDragAnchorScreenX =
            sCursorScreenX - pull[0] * weight * pixelsPerModelUnit * head_horizontal_projection_scale();
        sDragAnchorScreenY = sCursorScreenY - pull[1] * weight * pixelsPerModelUnit;
        sModelGrabPoint[0] = picked->v.ob[0];
        sModelGrabPoint[1] = picked->v.ob[1];
        sModelGrabPoint[2] = picked->v.ob[2];
        sGrabbing = TRUE;
        play_menu_sounds(SOUND_MENU_FLAG_PINCHMARIOFACE);
    }
}

static void update_head_controls(const struct SimpleFlipsHeadMesh *mesh) {
    u8 wasGrabbing = sGrabbing;
    u8 aDown = (gPlayer1Controller->buttonDown & A_BUTTON) != 0;

    if (!gWarpTransition.isActive) {
        s16 stickX = gPlayer1Controller->rawStickX;
        s16 stickY = gPlayer1Controller->rawStickY;
        u16 buttons = gPlayer1Controller->buttonDown;

        if (!sGrabbing) {
            sHeadYaw += ((buttons & R_CBUTTONS) != 0) * 0x200 - ((buttons & L_CBUTTONS) != 0) * 0x200;
            sHeadPitch = clamp_head_value(sHeadPitch + ((buttons & D_CBUTTONS) != 0) * 0x200
                                              - ((buttons & U_CBUTTONS) != 0) * 0x200,
                                          -0x3800, 0x3800);
        }
        sYawSin = sins(sHeadYaw);
        sYawCos = coss(sHeadYaw);
        sPitchSin = sins(sHeadPitch);
        sPitchCos = coss(sHeadPitch);

        if (stickX > -2 && stickX < 2) {
            stickX = 0;
        }
        if (stickY > -2 && stickY < 2) {
            stickY = 0;
        }
        sCursorScreenX = clamp_head_value(sCursorScreenX + stickX / 8, 28.0f, SCREEN_WIDTH - 28.0f);
        sCursorScreenY = clamp_head_value(sCursorScreenY + stickY / 8, 30.0f, SCREEN_HEIGHT - 30.0f);
        if (aDown && !sAWasDown) {
            grab_head(mesh);
        }
        if (sGrabbing && aDown) {
            Vec3f point;
            f32 modelUnitsPerPixel;
            f32 viewPullX;
            f32 viewPullY;

            rotate_head_point(sModelGrabPoint[0], sModelGrabPoint[1], sModelGrabPoint[2], point);
            modelUnitsPerPixel = (SIMPLEFLIPS_HEAD_CAMERA_DISTANCE - point[2] * SIMPLEFLIPS_HEAD_SCALE)
                                 / (SIMPLEFLIPS_HEAD_SCALE * SIMPLEFLIPS_HEAD_FOCAL_LENGTH);
            viewPullX = clamp_head_value((sCursorScreenX - sDragAnchorScreenX) * modelUnitsPerPixel
                                             / head_horizontal_projection_scale(),
                                         -HEAD_MAX_PULL_MODEL_UNITS, HEAD_MAX_PULL_MODEL_UNITS);
            viewPullY = clamp_head_value((sCursorScreenY - sDragAnchorScreenY) * modelUnitsPerPixel,
                                         -HEAD_MAX_PULL_MODEL_UNITS, HEAD_MAX_PULL_MODEL_UNITS);
            set_head_pull_from_view(viewPullX, viewPullY);
        } else {
            sGrabbing = FALSE;
        }
    } else {
        sGrabbing = FALSE;
    }
    if (wasGrabbing && !sGrabbing) {
        play_menu_sounds(SOUND_MENU_FLAG_LETGOMARIOFACE);
    }
    sAWasDown = aDown;
    if (!sGrabbing) {
        sModelPullX *= HEAD_RELEASE_DECAY;
        sModelPullY *= HEAD_RELEASE_DECAY;
        sModelPullZ *= HEAD_RELEASE_DECAY;
        if (sModelPullX > -0.01f && sModelPullX < 0.01f) {
            sModelPullX = 0.0f;
        }
        if (sModelPullZ > -0.01f && sModelPullZ < 0.01f) {
            sModelPullZ = 0.0f;
        }
        if (sModelPullY > -0.01f && sModelPullY < 0.01f) {
            sModelPullY = 0.0f;
        }
    }
}

Gfx *geo_simpleflips_head(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    const struct SimpleFlipsHeadMesh *mesh;
    const struct SimpleFlipsHeadVertexGroup *groups;
    Gfx *displayList;
    Vtx *vertices;
    s32 groupIndex = 0;
    s32 commandIndex;
    s32 vertexIndex;

    if (callContext == GEO_CONTEXT_CREATE) {
        sCursorScreenX = SCREEN_CENTER_X;
        sCursorScreenY = SCREEN_CENTER_Y;
        sModelPullX = 0.0f;
        sModelPullY = 0.0f;
        sModelPullZ = 0.0f;
        sHeadPitch = 0;
        sHeadYaw = 0;
        sPitchSin = 0.0f;
        sYawSin = 0.0f;
        sPitchCos = 1.0f;
        sYawCos = sPitchCos;
        sHeadShown = FALSE;
        sGrabbing = FALSE;
        sAWasDown = FALSE;
    }
    if (callContext != GEO_CONTEXT_RENDER || !jrb_simpleflips_is_game_over()) {
        return NULL;
    }
    SET_GRAPH_NODE_LAYER(node->flags, LAYER_OPAQUE);
    mesh = segmented_to_virtual(&intro_simpleflips_head_mesh);
    if (!sHeadShown) {
        sHeadShown = TRUE;
        play_menu_sounds(SOUND_MENU_FLAG_HANDAPPEAR);
    }
    update_head_controls(mesh);
    if (sModelPullX == 0.0f && sModelPullY == 0.0f && sModelPullZ == 0.0f && sHeadYaw == 0
        && sHeadPitch == 0) {
        return segmented_to_virtual(mesh->displayList);
    }
    displayList = alloc_display_list(mesh->commandCount * sizeof(Gfx));
    vertices = alloc_display_list(mesh->vertexCount * sizeof(Vtx));
    if (displayList == NULL || vertices == NULL) {
        return segmented_to_virtual(mesh->displayList);
    }
    bcopy(segmented_to_virtual(mesh->displayList), displayList, mesh->commandCount * sizeof(Gfx));
    groups = segmented_to_virtual(mesh->groups);
    for (commandIndex = 0; commandIndex < mesh->commandCount; commandIndex++) {
        const Vtx *original;

        if ((displayList[commandIndex].words.w0 >> 24) != G_VTX) {
            continue;
        }
        original = segmented_to_virtual(groups[groupIndex].vertices);

        bcopy(original, vertices, groups[groupIndex].count * sizeof(Vtx));
        for (vertexIndex = 0; vertexIndex < groups[groupIndex].count; vertexIndex++) {
            f32 weight = head_pull_weight(&original[vertexIndex]);

            Vec3f point;
            Vec3f normal;
            s32 axis;

            rotate_head_point(original[vertexIndex].v.ob[0] + sModelPullX * weight,
                              original[vertexIndex].v.ob[1] + sModelPullY * weight,
                              original[vertexIndex].v.ob[2] + sModelPullZ * weight, point);
            rotate_head_point((s8) original[vertexIndex].v.cn[0], (s8) original[vertexIndex].v.cn[1],
                              (s8) original[vertexIndex].v.cn[2], normal);
            for (axis = 0; axis < 3; axis++) {
                vertices[vertexIndex].v.ob[axis] = point[axis];
                vertices[vertexIndex].v.cn[axis] = (s8) normal[axis];
            }
        }
        // Frame-owned copies keep deformation from touching a previous RSP task's vertices.
        displayList[commandIndex].words.w1 = VIRTUAL_TO_PHYSICAL(vertices);
        vertices += groups[groupIndex].count;
        groupIndex++;
    }
    return displayList;
}

Gfx *geo_simpleflips_head_cursor(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    Gfx *displayList;
    Gfx *head;
    Mtx *matrices;

    if (callContext != GEO_CONTEXT_RENDER || !jrb_simpleflips_is_game_over()) {
        return NULL;
    }
    displayList = alloc_display_list(8 * sizeof(Gfx));
    matrices = alloc_display_list(2 * sizeof(Mtx));
    if (displayList == NULL || matrices == NULL) {
        return NULL;
    }
    head = displayList;
    SET_GRAPH_NODE_LAYER(node->flags, LAYER_ALPHA);
    guOrtho(&matrices[0], 0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -10.0f, 10.0f, 1.0f);
    guTranslate(&matrices[1], sCursorScreenX - 5.0f, sCursorScreenY - 25.0f, 0.0f);
    gSPPerspNormalize(head++, 0xFFFF);
    gSPMatrix(head++, VIRTUAL_TO_PHYSICAL(&matrices[0]), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(head++, VIRTUAL_TO_PHYSICAL(&matrices[1]), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPClearGeometryMode(head++, G_LIGHTING);
    gSPDisplayList(head++,
                   sGrabbing ? intro_simpleflips_grabbing_hand_dl : intro_simpleflips_idle_hand_dl);
    gSPSetGeometryMode(head++, G_LIGHTING);
    gSPEndDisplayList(head++);
    return displayList;
}
