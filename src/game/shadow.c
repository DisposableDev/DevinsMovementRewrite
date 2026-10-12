#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "behavior_data.h"
#include "object_constants.h"
#include "geo_misc.h"
#include "level_table.h"
#include "memory.h"
#include "level_update.h"
#include "object_list_processor.h"
#include "rendering_graph_node.h"
#include "segment2.h"
#include "shadow.h"
#include "sm64.h"

/**
 * @file shadow.c
 * This file implements a self-contained subsystem used to draw shadows.
 */

/**
 * An array consisting of all the hardcoded rectangle shadows in the game.
 */
static ShadowRectangle sShadowRectangles[2] = {
    { 7.2f, 4.6f, TRUE }, // Spindel
    { 4.0f, 3.6f, TRUE }, // Whomp
};

struct Shadow gCurrShadow;
struct Shadow *s = &gCurrShadow;

// Separate Mario's shadow from coplanar geometry without disabling depth tests.
#define MARIO_SHADOW_SURFACE_OFFSET 2.0f
#define MARIO_SHADOW_MAX_SURFACE_OFFSET 6.0f
#define MARIO_SHADOW_OFFSET_NEAR_DEPTH 1000.0f
#define MARIO_SHADOW_OFFSET_FAR_DEPTH 4000.0f
#define MARIO_WALL_SHADOW_BASE_RANGE 600.0f
#define MARIO_WALL_SHADOW_LOOKAHEAD_FRAMES 12.0f

/**
 * Shrink a shadow when its parent object is further from the floor, given the
 * initial size of the shadow and the current distance.
 */
f32 scale_shadow_with_distance(f32 initial, f32 distFromFloor) {
    if (distFromFloor <= 0.0f) {
        return initial;
    } else if (distFromFloor >= 600.0f) {
        return initial * 0.5f;
    } else {
        return initial * (1.0f - ((distFromFloor * 0.5f) / 600.0f));
    }
}

/**
 * Dim a shadow when its parent object is further from the ground.
 */
s32 dim_shadow_with_distance(u8 solidity, f32 distFromFloor) {
    if (solidity < 121) {
        return solidity;
    } else if (distFromFloor <= 0.0f) {
        return solidity;
    } else if (distFromFloor >= 600.0f) {
        return 120;
    } else {
        return (((120 - solidity) * distFromFloor) / 600.0f) + (f32) solidity;
    }
}

/**
 * Initialize a shadow. Return 0 on success, 1 on failure.
 *
 * @param pos Position of the parent object (not the shadow)
 * @param shadowScale Diameter of the shadow
 * @param overwriteSolidity Flag for whether the existing shadow solidity should
 *                          be dimmed based on its distance to the floor
 */
s32 init_shadow(f32 distToShadow, s16 shadowScale, s8 shadowType, u8 overwriteSolidity) {
    f32 baseScale;

    if (shadowType != SHADOW_SQUARE_PERMANENT) {
        // Set solidity and scale based on distance.

        if (overwriteSolidity) {
            s->solidity = dim_shadow_with_distance(overwriteSolidity, distToShadow);
        }

        baseScale = scale_shadow_with_distance(shadowScale, distToShadow);
    } else {
        s->solidity = overwriteSolidity;
        baseScale = shadowScale;
    }
    vec3f_set(s->scale, baseScale, baseScale, baseScale);

    return !(s->solidity);
}

/**
 * Linearly interpolate a shadow's solidity between zero and finalSolidity
 * depending on curr's relation to start and end.
 */
void linearly_interpolate_solidity_positive(u8 finalSolidity, s16 curr, s16 start,
                                            s16 end) {
    if (curr >= 0 && curr < start) {
        s->solidity = 0;
    } else if (end < curr) {
        s->solidity = finalSolidity;
    } else {
        s->solidity = (f32) finalSolidity * (curr - start) / (end - start);
    }
}

/**
 * Linearly interpolate a shadow's solidity between initialSolidity and zero
 * depending on curr's relation to start and end. Note that if curr < start,
 * the solidity will be zero.
 */
void linearly_interpolate_solidity_negative(u8 initialSolidity, s16 curr, s16 start,
                                            s16 end) {
    // The curr < start case is not handled. Thus, if start != 0, this function
    // will have the surprising behavior of hiding the shadow until start.
    // This is not necessarily a bug, since this function is only used once,
    // with start == 0.
    if (curr >= start && end >= curr) {
        s->solidity = ((f32) initialSolidity * (1.0f - (f32)(curr - start) / (end - start)));
    } else {
        s->solidity = 0;
    }
}

/**
 * Change a shadow's solidity based on the player's current animation frame.
 */
s32 correct_shadow_solidity_for_animations(u8 initialSolidity) {
    s16 animFrame = gMarioObject->header.gfx.animInfo.animFrame;
    switch (gMarioObject->header.gfx.animInfo.animID) {
        case MARIO_ANIM_IDLE_ON_LEDGE:
            return SHADOW_SOLIDITY_NO_SHADOW;
        case MARIO_ANIM_FAST_LEDGE_GRAB:
            linearly_interpolate_solidity_positive(initialSolidity, animFrame,  5, 14);
            return SHADOW_SOILDITY_ALREADY_SET;
        case MARIO_ANIM_SLOW_LEDGE_GRAB:
            linearly_interpolate_solidity_positive(initialSolidity, animFrame, 21, 33);
            return SHADOW_SOILDITY_ALREADY_SET;
        case MARIO_ANIM_CLIMB_DOWN_LEDGE:
            linearly_interpolate_solidity_negative(initialSolidity, animFrame,  0,  5);
            return SHADOW_SOILDITY_ALREADY_SET;
        default:
            return SHADOW_SOLIDITY_NOT_YET_SET;
    }
}

#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
/**
 * Slightly change the height of a shadow in levels with lava.
 */
void correct_lava_shadow_height(f32 *floorHeight) {
    if (gCurrLevelNum == LEVEL_BITFS) {
        if (*floorHeight < -3000.0f) {
            *floorHeight = -3062.0f;
            s->isDecal = FALSE;
        } else if (*floorHeight > 3400.0f) {
            *floorHeight = 3492.0f;
            s->isDecal = FALSE;
        }
    } else if (gCurrLevelNum == LEVEL_LLL
               && gCurrAreaIndex == 1) {
        *floorHeight = 5.0f;
        s->isDecal = FALSE;
    }
}
#endif

/**
 * Add a shadow to the given display list.
 * shadowType 0 uses a circle texture, the rest use a square texture.
 * Uses environment alpha for shadow solidity.
 */
static void add_shadow_to_display_list(Gfx *displayListHead, s8 shadowType, s32 smallTexture,
                                       s32 tint) {
    const Gfx *material = shadowType == SHADOW_CIRCLE ? dl_shadow_circle : dl_shadow_square;
#if defined(HD_SHADOWS) && defined(SHADOW_TEXTURE_LOD) && !defined(SHADOW_TEXTURE_REUSE)
    if (smallTexture) {
        material = shadowType == SHADOW_CIRCLE ? dl_shadow_circle_32 : dl_shadow_square_32;
    }
#else
    (void) smallTexture;
#endif
    gSPDisplayList(displayListHead++, material);
    if (tint >= 0) {
        gDPSetCombineLERP(displayListHead++, 0, 0, 0, ENVIRONMENT, TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0,
                          ENVIRONMENT, TEXEL0, 0, ENVIRONMENT, 0);
    }
    s32 color = tint >= 0 ? tint : 255;
    gDPSetEnvColor(displayListHead++, color, color, color, s->solidity);
    gSPDisplayList(displayListHead++, dl_shadow_end);
    gSPEndDisplayList(displayListHead);
}

/** Project Mario's swimming/flight cue onto the nearest solid surface ahead. */
Gfx *create_mario_wall_shadow(Vec3f pos, s16 shadowScale, u8 shadowSolidity) {
    struct MarioState *m = gMarioState;
    if (m == NULL || gMarioObject == NULL || gCurGraphNodeHeldObject != NULL
        || gCurGraphNodeObject != (struct GraphNodeObject *) gMarioObject
        || !((m->action & ACT_FLAG_SWIMMING) || m->action == ACT_FLYING)
        || (m->action & ACT_FLAG_INTANGIBLE) || shadowSolidity == 0) {
        return NULL;
    }

    Vec3f origin, ray, hit;
    vec3f_copy(origin, m->pos);
    origin[1] += 80.0f;
    f32 range = MARIO_WALL_SHADOW_BASE_RANGE + absf(m->forwardVel) * MARIO_WALL_SHADOW_LOOKAHEAD_FRAMES;
    f32 horizontalRange = range * coss(m->faceAngle[0]);
    vec3f_set(ray, horizontalRange * sins(m->faceAngle[1]), range * sins(m->faceAngle[0]),
              horizontalRange * coss(m->faceAngle[1]));
    // Sloped terrain can belong to any solid-surface partition.
    s32 flags = RAYCAST_FIND_FLOOR | RAYCAST_FIND_WALL | RAYCAST_FIND_CEIL | RAYCAST_MARIO_WALL
                | RAYCAST_IGNORE_INTANGIBLE;
    if (m->flags & MARIO_VANISH_CAP) {
        flags |= RAYCAST_IGNORE_VANISH_CAP_WALL;
    }
    struct Surface *wall = NULL;
    f32 distance = find_surface_on_ray(origin, ray, &wall, hit, flags);
    if (wall == NULL || wall->type == SURFACE_DEATH_PLANE || distance >= range
        || wall->normal.x * ray[0] + wall->normal.y * ray[1] + wall->normal.z * ray[2] >= -1.0f) {
        return NULL;
    }

    u8 solidity = shadowSolidity * (1.0f - distance / range);
    if (solidity == 0) {
        return NULL;
    }
    Gfx *displayList = alloc_display_list(8 * sizeof(Gfx));
    if (displayList == NULL) {
        return NULL;
    }
    s->solidity = solidity;
    vec3f_set(s->floorNormal, wall->normal.x, wall->normal.y, wall->normal.z);
    f32 scale = shadowScale * (0.6f + 0.4f * distance / range);
    vec3f_set(s->scale, scale, scale, scale);
    for (s32 axis = 0; axis < 3; axis++) {
        pos[axis] = hit[axis] + s->floorNormal[axis] * MARIO_SHADOW_SURFACE_OFFSET;
    }
    s->isDecal = TRUE;
    s->hasPriority = TRUE;
    // Decal mode can reject this raised quad at close wall and slope views.
    // Keep depth comparison and the texture's soft edge, then restore the layer.
    s32 zBuffer = gCurGraphNodeMasterList->node.flags & GRAPH_RENDER_Z_BUFFER;
    gSPDisplayList(displayList, dl_shadow_circle);
    gDPSetEnvColor(displayList + 1, 255, 255, 255, s->solidity);
    gDPPipeSync(displayList + 2);
    gDPSetRenderMode(displayList + 3, (zBuffer ? G_RM_AA_ZB_XLU_SURF : G_RM_AA_XLU_SURF) & ~AA_EN,
                     (zBuffer ? G_RM_AA_ZB_XLU_SURF2 : G_RM_AA_XLU_SURF2) & ~AA_EN);
    gSPDisplayList(displayList + 4, dl_shadow_end);
    gDPPipeSync(displayList + 5);
    gDPSetRenderMode(displayList + 6, zBuffer ? G_RM_AA_ZB_XLU_DECAL : G_RM_AA_XLU_SURF,
                     zBuffer ? G_RM_AA_ZB_XLU_DECAL2 : G_RM_AA_XLU_SURF2);
    gSPEndDisplayList(displayList + 7);
    return displayList;
}

//! HACKERSM64_DO:
//      - Breakout create_shadow_below_xyz into multiple functions
/**
 * Create a shadow at the absolute position given, with the given parameters.
 * Return a pointer to the display list representing the shadow.
 */
Gfx *create_shadow_below_xyz(Vec3f pos, s16 shadowScale, u8 shadowSolidity, s8 shadowType, s8 shifted) {
    struct Object *obj = gCurGraphNodeObjectNode;
    // Check if the object exists.
    if (obj == NULL) {
        return NULL;
    }

    // The floor underneath the object.
    struct Surface *floor = NULL;
    // The y-position of the floor (or water or lava) underneath the object.
    f32 floorHeight = FLOOR_LOWER_LIMIT_MISC;
    f32 x = pos[0];
    f32 y = pos[1];
    f32 z = pos[2];
    s8 isPlayer = (obj == gMarioObject);
    s8 notHeldObj = (gCurGraphNodeHeldObject == NULL);

    // Attempt to use existing floors before finding a new one.
    if (notHeldObj && isPlayer && gMarioState->floor) {
        // The object is Mario and has a referenced floor.
        floor = gMarioState->floor;
        floorHeight = gMarioState->floorHeight;
    } else if (notHeldObj && (gCurGraphNodeObject != &gMirrorMario) && obj->oFloor) {
        // The object is not Mario but has a referenced floor.
        //! Some objects only get their oFloor from bhv_init_room, which skips dynamic floors.
        floor = obj->oFloor;
        floorHeight = obj->oFloorHeight;
    } else {
        // The object has no referenced floor, so find a new one.
        // gCollisionFlags |= COLLISION_FLAG_RETURN_FIRST;
        floorHeight = find_floor(x, y, z, &floor);

        // No shadow if the position is OOB.
        if (floor == NULL) {
            return NULL;
        }

        // Skip shifting the shadow height later, since the find_floor call above uses the already
        // shifted position.
        shifted = FALSE;
    }

    // The shadow is a decal by default.
    s->isDecal = TRUE;

    // Check for water under the shadow.
    struct Surface *waterFloor = NULL;
    f32 waterLevel = find_water_level_and_floor(x, y, z, &waterFloor);

    // Whether the floor is an environment box rather than an actual surface.
    s32 isEnvBox = FALSE;

    if (waterLevel > FLOOR_LOWER_LIMIT_MISC && y >= waterLevel && floorHeight <= waterLevel) {
        // Skip shifting the shadow height later, since the find_water_level_and_floor call above uses
        // the already shifted position.
        shifted = FALSE;

        // If there is water under the shadow, put the shadow on the water.
        floorHeight = waterLevel;

        // Don't use the decal layer, since water is transparent.
        s->isDecal = FALSE;

        // Check whether the water is an environment box or a water surface.
        if (waterFloor != NULL) { // Water surfaces:
            floor = waterFloor;
        } else { // Environment boxes:
            isEnvBox = TRUE;
        }
    } else { // Normal surfaces:
        TerrainData type = floor->type;
        if (type == SURFACE_DEATH_PLANE && notHeldObj
            && (isPlayer || gCurGraphNodeObject == &gMirrorMario)) {
            return NULL;
        }
        if (type == SURFACE_ICE) {
            // Stock ice is usually transparent. The lab's ice lanes, rims and
            // sculptures are opaque, so their shadows need the decal layer.
            s->isDecal = (gCurrLevelNum == LEVEL_PHYSICS_LAB);
#ifdef ENABLE_VANILLA_LEVEL_SPECIFIC_CHECKS
        } else if (type == SURFACE_BURNING) {
            // Set the shadow height to the lava height in specific areas.
            correct_lava_shadow_height(&floorHeight);
#endif
        } else if (floor->object != NULL
                   && floor->object->behavior == segmented_to_virtual(bhvPlatformOnTrack)
                   && floor->object->oPlatformOnTrackType == PLATFORM_ON_TRACK_TYPE_CARPET) {
            // Raise the shadow 5 units so the shadow doesn't clip into the flying carpet.
            floorHeight += 5;
            // The flying carpet is transparent.
            s->isDecal = FALSE;
        }
    }

    f32 nx, ny, nz;
    if (isEnvBox) {
        // Assume the floor is flat.
        nx = 0.0f;
        ny = 1.0f;
        nz = 0.0f;
    } else {
        // Read the floor's normals.
        nx = floor->normal.x;
        ny = floor->normal.y;
        nz = floor->normal.z;

        // No shadow if the y-normal is negative (an unexpected result).
        if (ny <= 0.0f) {
            return NULL;
        }

        // If the animation changes the shadow position, move its height to the new position.
        if (shifted) {
            floorHeight = -((x * nx) + (z * nz) + floor->originOffset) / ny;
        }
    }

    // No shadow if the floor is lower than expected possible,
    if (floorHeight < FLOOR_LOWER_LIMIT_MISC) {
        return NULL;
    }

    // Get the vertical distance to the shadow, now that the final shadow height is set.
    f32 distToShadow = (y - floorHeight);

    // No shadow if the object is below it.
    if (distToShadow < -80.0f) {
        return NULL;
    }

    // No shadow if the non-Mario object is too high.
    if (!isPlayer && distToShadow > 1024.0f) {
        return NULL;
    }

    vec3f_set(s->floorNormal, nx, ny, nz);

    if (isPlayer) {
        // Set the shadow solidity manually for certain Mario animations.
        s32 solidityAction = correct_shadow_solidity_for_animations(shadowSolidity);
        switch (solidityAction) {
            case SHADOW_SOLIDITY_NO_SHADOW:
                return NULL;
            case SHADOW_SOILDITY_ALREADY_SET:
                if (init_shadow(distToShadow, shadowScale, shadowType, /* overwriteSolidity */ 0)) {
                    return NULL;
                }
                break;
            case SHADOW_SOLIDITY_NOT_YET_SET:
                if (init_shadow(distToShadow, shadowScale, shadowType, shadowSolidity)) {
                    return NULL;
                }
                break;
            default:
                return NULL;
        }
    } else {
        if (init_shadow(distToShadow, shadowScale, shadowType, shadowSolidity)) {
            return NULL;
        }

        // Get the scaling modifiers for rectangular shadows (Whomp and Spindel).
        if (shadowType >= SHADOW_RECTANGLE_HARDCODED_OFFSET) {
            s8 idx = shadowType - SHADOW_RECTANGLE_HARDCODED_OFFSET;
            s->scale[0] *= sShadowRectangles[idx].scaleX;
            s->scale[2] *= sShadowRectangles[idx].scaleZ;
            if (sShadowRectangles[idx].scaleWithDistance) {
                scale_shadow_with_distance(s->scale[0], distToShadow);
                scale_shadow_with_distance(s->scale[2], distToShadow);
            }
        }
    }

#if defined(VERTICAL_CULLING) && defined(SHADOW_VIEW_CULLING)
    if (notHeldObj && !isPlayer && gCurGraphNodeObject != &gMirrorMario
        && (shadowType == SHADOW_CIRCLE || shadowType == SHADOW_SQUARE
            || shadowType == SHADOW_SQUARE_PERMANENT)) {
        Vec3f shadowPosition = { x, floorHeight, z };
        // The regular quad has (+/-1, 0, +/-1) corners. Its normalized plane
        // rotation fits within 1.5 times the final uniform scale; pad rounding.
        f32 radius = absf(s->scale[0]) * 1.5f + 2.0f;
        if (geo_shadow_outside_vertical_view(shadowPosition, radius)) {
            return NULL;
        }
    }
#endif

    s32 tint = -1;
    if (notHeldObj && obj->behavior == segmented_to_virtual(bhvBowser)
        && obj->oAction == BOWSER_ACT_TELEPORT) {
        tint = 255 - obj->oOpacity;
    }
    Gfx *displayList = alloc_display_list((tint >= 0 ? 5 : 4) * sizeof(Gfx));

    if (displayList == NULL) {
        return NULL;
    }

    // Generate the shadow display list with type and solidity.
    s32 smallTexture = FALSE;
#if defined(HD_SHADOWS) && defined(SHADOW_TEXTURE_LOD) && !defined(SHADOW_TEXTURE_REUSE)
    if (notHeldObj && !isPlayer && gCurGraphNodeObject != &gMirrorMario
        && (shadowType == SHADOW_CIRCLE || shadowType == SHADOW_SQUARE
            || shadowType == SHADOW_SQUARE_PERMANENT)) {
        Vec3f shadowPosition = { x, floorHeight, z };
        smallTexture = geo_shadow_uses_small_texture(shadowPosition, absf(s->scale[0]) * 1.5f + 2.0f);
    }
#endif
    add_shadow_to_display_list(displayList, shadowType, smallTexture, tint);

    // Move the shadow position to the floor height.
    pos[1] = floorHeight;

    s->hasPriority = notHeldObj && (isPlayer || gCurGraphNodeObject == &gMirrorMario);
    if (s->hasPriority) {
        f32 surfaceOffset = MARIO_SHADOW_SURFACE_OFFSET;
        if (gCurGraphNodeCamera != NULL) {
            Vec3f cameraPosition;
            linear_mtxf_mul_vec3f_and_translate(gCameraTransform, cameraPosition, pos);
            f32 distanceFraction = (-cameraPosition[2] - MARIO_SHADOW_OFFSET_NEAR_DEPTH)
                                   / (MARIO_SHADOW_OFFSET_FAR_DEPTH - MARIO_SHADOW_OFFSET_NEAR_DEPTH);
            distanceFraction = MAX(0.0f, MIN(1.0f, distanceFraction));
            surfaceOffset +=
                distanceFraction * (MARIO_SHADOW_MAX_SURFACE_OFFSET - MARIO_SHADOW_SURFACE_OFFSET);
        }
        // Use the floor normal so steep slopes get the same separation as flat
        // floors. Apply this after sizing/fading to preserve the actual height.
        pos[0] += nx * surfaceOffset;
        pos[1] += ny * surfaceOffset;
        pos[2] += nz * surfaceOffset;
    }

    return displayList;
}
