#include "game_options_runtime.h"
#include "game/bowser_arena.h"
#include "player_stats.h"
#include <PR/ultratypes.h>

#include "sm64.h"
#include "small_data.h"
#include "area.h"
#include "audio/external.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "camera.h"
#include "compact_explosion_bubbles.h"
#include "course_table.h"
#include "dialog_ids.h"
#include "engine/behavior_script.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "envfx_bubbles.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "interaction.h"
#include "level_misc_macros.h"
#include "level_table.h"
#include "level_update.h"
#include "main.h"
#include "levels/bob/header.h"
#include "levels/ttm/header.h"
#include "mario.h"
#include "mario_he_thicc.h"
#include "mario_actions_cutscene.h"
#include "mario_misc.h"
#include "memory.h"
#include "obj_behaviors.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "object_platform_motion.h"
#include "rendering_graph_node.h"
#include "save_file.h"
#include "spawn_object.h"
#include "spawn_sound.h"
#include "rumble_init.h"

/**
 * @file obj_behaviors.c
 * This file contains a portion of the obj behaviors and many helper functions for those
 * specific behaviors. Few functions besides the bhv_ functions are used elsewhere in the repo.
 */

/**
 * Shared scratch for the last object_step floor query; each step also publishes to oFloor.
 */
static struct Surface *sObjFloor;

/**
 * Set to false when an object close to the floor should not be oriented in reference
 * to it. Happens with boulder, falling pillar, and the rolling snowman body.
 */
static s8 sOrientObjWithFloor = TRUE;

/**
 * Keeps track of Mario's previous non-zero room.
 * Helps keep track of room when Mario is over an object.
 */
s16 sPrevCheckMarioRoom = 0;

/**
 * Tracks whether or not Yoshi has walked/jumped off the roof.
 */
s8 sYoshiDead = FALSE;

extern const Trajectory ccm_seg7_trajectory_snowman[] SEGMENTED_DATA;
extern const Trajectory *const inside_castle_seg7_trajectory_mips[] SEGMENTED_DATA;

/**
 * Resets yoshi as spawned/despawned upon new file select.
 * Possibly a function with stubbed code.
 */
void set_yoshi_as_not_dead(void) {
    sYoshiDead = FALSE;
}

/**
 * An unused geo function. Bears strong similarity to geo_bits_bowser_coloring, and relates something
 * of the opacity of an object to something else. Perhaps like, giving a parent object the same
 * opacity?
 */
Gfx UNUSED *geo_obj_transparency_something(s32 callContext, struct GraphNode *node, UNUSED Mat4 *mtx) {
    Gfx *gfxHead = NULL;
    Gfx *gfx;

    if (callContext == GEO_CONTEXT_RENDER) {
        struct Object *heldObject = (struct Object *) gCurGraphNodeObject;
        struct Object *obj = (struct Object *) node;


        if (gCurGraphNodeHeldObject != NULL) {
            heldObject = gCurGraphNodeHeldObject->objNode;
        }

        gfxHead = alloc_display_list(3 * sizeof(Gfx));
        gfx = gfxHead;
        SET_GRAPH_NODE_LAYER(obj->header.gfx.node.flags, LAYER_TRANSPARENT);

        gDPSetEnvColor(gfx++, 255, 255, 255, heldObject->oOpacity);

        gSPEndDisplayList(gfx);
    }

    return gfxHead;
}

/**
 * Backwards compatibility, used to be a duplicate function
 */
#define absf_2 absf

/**
 * Turns an object away from floors/walls that it runs into.
 */
void turn_obj_away_from_surface(f32 velX, f32 velZ, f32 nX, UNUSED f32 nY, f32 nZ, f32 *objYawX,
                            f32 *objYawZ) {
    *objYawX = (nZ * nZ - nX * nX) * velX / (nX * nX + nZ * nZ)
               - 2 * velZ * (nX * nZ) / (nX * nX + nZ * nZ);

    *objYawZ = (nX * nX - nZ * nZ) * velZ / (nX * nX + nZ * nZ)
               - 2 * velX * (nX * nZ) / (nX * nX + nZ * nZ);
}

/**
 * Finds any wall collisions, applies them, and turns away from the surface.
 */
s8 obj_find_wall(f32 objNewX, f32 objY, f32 objNewZ, f32 objVelX, f32 objVelZ) {
    struct WallCollisionData hitbox;
    f32 wall_nX, wall_nY, wall_nZ, objVelXCopy, objVelZCopy, objYawX, objYawZ;

    hitbox.x = objNewX;
    hitbox.y = objY;
    hitbox.z = objNewZ;
    hitbox.offsetY = o->hitboxHeight / 2;
    hitbox.radius = o->hitboxRadius;

    s32 wallCount = find_wall_collisions(&hitbox);
    // Edge and containment pushes can have no interactive wall reference.
    if (wallCount != 0 || hitbox.x != objNewX || hitbox.z != objNewZ) {
        o->oPosX = hitbox.x;
        o->oPosY = hitbox.y;
        o->oPosZ = hitbox.z;

        if (wallCount != 0) {
            wall_nX = hitbox.walls[0]->normal.x;
            wall_nY = hitbox.walls[0]->normal.y;
            wall_nZ = hitbox.walls[0]->normal.z;
        } else {
            wall_nX = hitbox.x - objNewX;
            wall_nY = 0.0f;
            wall_nZ = hitbox.z - objNewZ;
        }

        objVelXCopy = objVelX;
        objVelZCopy = objVelZ;

        // Keep first-wall reflection; an unreported push reflects only inward travel.
        if (wallCount != 0 || wall_nX * objVelXCopy + wall_nZ * objVelZCopy < 0.0f) {
            turn_obj_away_from_surface(objVelXCopy, objVelZCopy, wall_nX, wall_nY, wall_nZ, &objYawX,
                                       &objYawZ);
            o->oMoveAngleYaw = atan2s(objYawZ, objYawX);
        }
        return FALSE;
    }

    return TRUE;
}

/**
 * Turns an object away from steep floors, similarly to walls.
 */
s8 turn_obj_away_from_steep_floor(struct Surface *objFloor, f32 floorY, f32 objVelX, f32 objVelZ) {
    f32 floor_nX, floor_nY, floor_nZ, objVelXCopy, objVelZCopy, objYawX, objYawZ;

    if (objFloor == NULL) {
        o->oMoveAngleYaw += 0x8000;
        return FALSE;
    }

    floor_nX = objFloor->normal.x;
    floor_nY = objFloor->normal.y;
    floor_nZ = objFloor->normal.z;

    // If the floor is steep and we are below it (i.e. walking into it), turn away from the floor.
    if (floor_nY < 0.5f && floorY > o->oPosY) {
        objVelXCopy = objVelX;
        objVelZCopy = objVelZ;
        turn_obj_away_from_surface(objVelXCopy, objVelZCopy, floor_nX, floor_nY, floor_nZ, &objYawX, &objYawZ);
        o->oMoveAngleYaw = atan2s(objYawZ, objYawX);
        return FALSE;
    }

    return TRUE;
}

/**
 * Orients an object with the given normals, typically the surface under the object.
 */
void obj_orient_graph(struct Object *obj, f32 normalX, f32 normalY, f32 normalZ) {
    Vec3f objVisualPosition, surfaceNormals;

    // Passes on orienting certain objects that shouldn't be oriented, like boulders.
    if (!sOrientObjWithFloor) {
        return;
    }

    // Passes on orienting billboard objects, i.e. coins, trees, etc.
    if (obj->header.gfx.node.flags & GRAPH_RENDER_BILLBOARD) {
        return;
    }

    vec3f_copy_y_off(objVisualPosition, &obj->oPosVec, obj->oGraphYOffset);
    vec3f_set(surfaceNormals, normalX, normalY, normalZ);

    mtxf_align_terrain_normal(obj->transform, surfaceNormals, objVisualPosition, obj->oFaceAngleYaw);
    obj->header.gfx.throwMatrix = &obj->transform;
}

/**
 * Determines an object's forward speed multiplier.
 */
void calc_obj_friction(f32 *objFriction, f32 floor_nY) {
    if (floor_nY < 0.2f && o->oFriction < 0.9999f) {
        *objFriction = 0;
    } else {
        *objFriction = o->oFriction;
    }
}

/**
 * Updates an objects speed for gravity and updates Y position.
 */
void calc_new_obj_vel_and_pos_y(struct Surface *objFloor, f32 objFloorY, f32 objVelX, f32 objVelZ) {
    object_platform_motion_before_step(o);
    f32 floor_nX = objFloor->normal.x;
    f32 floor_nY = objFloor->normal.y;
    f32 floor_nZ = objFloor->normal.z;
    f32 objFriction;

    // Caps vertical speed with a "terminal velocity".
    o->oVelY -= o->oGravity;
    if (o->oVelY > 75.0) {
        o->oVelY = 75.0;
    }
    if (o->oVelY < -75.0) {
        o->oVelY = -75.0;
    }

    o->oPosY += o->oVelY;

    // Snap the object up to the floor.
    if (o->oPosY < objFloorY) {
        o->oPosY = objFloorY;

        // Bounces an object if the ground is hit fast enough.
        if (o->oVelY < -17.5f) {
            o->oVelY = -(o->oVelY / 2);
        } else {
            o->oVelY = 0;
        }
    }

    if ((o->oPosY >= objFloorY) && (o->oPosY < objFloorY + 37)) {
        // Adds horizontal component of gravity for horizontal speed.
        f32 nxz = sqr(floor_nX) + sqr(floor_nZ);
        f32 vel = ((nxz) / (nxz + sqr(floor_nY))) * o->oGravity * 2;
        objVelX += floor_nX * vel;
        objVelZ += floor_nZ * vel;

        if (objVelX < NEAR_ZERO && objVelX > -NEAR_ZERO) objVelX = 0;
        if (objVelZ < NEAR_ZERO && objVelZ > -NEAR_ZERO) objVelZ = 0;

        if (objVelX != 0 || objVelZ != 0) {
            o->oMoveAngleYaw = atan2s(objVelZ, objVelX);
        }

        calc_obj_friction(&objFriction, floor_nY);
        o->oForwardVel = sqrtf(sqr(objVelX) + sqr(objVelZ)) * objFriction;
    }
}

void calc_new_obj_vel_and_pos_y_underwater(struct Surface *objFloor, f32 floorY, f32 objVelX, f32 objVelZ, f32 waterY) {
    object_platform_motion_before_step(o);
    f32 floor_nX = objFloor->normal.x;
    f32 floor_nY = objFloor->normal.y;
    f32 floor_nZ = objFloor->normal.z;

    f32 netYAccel = (1.0f - o->oBuoyancy) * (-1.0f * o->oGravity);
    o->oVelY -= netYAccel;

    // Caps vertical speed with a "terminal velocity".
    if (o->oVelY > 75.0f) {
        o->oVelY = 75.0f;
    }
    if (o->oVelY < -75.0f) {
        o->oVelY = -75.0f;
    }

    o->oPosY += o->oVelY;

    // Snap the object up to the floor.
    if (o->oPosY < floorY) {
        o->oPosY = floorY;

        // Bounces an object if the ground is hit fast enough.
        if (o->oVelY < -17.5f) {
            o->oVelY = -(o->oVelY / 2);
        } else {
            o->oVelY = 0;
        }
    }

    // If moving fast near the surface of the water, flip vertical speed? To emulate skipping?
    if (o->oForwardVel > 12.5f && (waterY + 30.0f) > o->oPosY && (waterY - 30.0f) < o->oPosY) {
        o->oVelY = -o->oVelY;
    }

    if ((o->oPosY >= floorY) && (o->oPosY < floorY + 37)) {
        // Adds horizontal component of gravity for horizontal speed.
        f32 nxz = sqr(floor_nX) + sqr(floor_nZ);
        f32 velm = (nxz / (nxz + sqr(floor_nY))) * netYAccel * 2;
        objVelX += floor_nX * velm;
        objVelZ += floor_nZ * velm;
    }

    if (objVelX < NEAR_ZERO && objVelX > -NEAR_ZERO) objVelX = 0;
    if (objVelZ < NEAR_ZERO && objVelZ > -NEAR_ZERO) objVelZ = 0;

    if (o->oVelY < NEAR_ZERO && o->oVelY > -NEAR_ZERO) {
        o->oVelY = 0;
    }

    if (objVelX != 0 || objVelZ != 0) {
        o->oMoveAngleYaw = atan2s(objVelZ, objVelX);
    }

    // Decreases both vertical velocity and forward velocity. Likely so that skips above
    // don't loop infinitely.
    o->oForwardVel = sqrtf(sqr(objVelX) + sqr(objVelZ)) * 0.8f;
    o->oVelY *= 0.8f;
}

/**
 * Updates an objects position from oForwardVel and oMoveAngleYaw.
 */
void obj_update_pos_vel_xz(void) {
    o->oPosX += o->oForwardVel * sins(o->oMoveAngleYaw);
    o->oPosZ += o->oForwardVel * coss(o->oMoveAngleYaw);
}

/**
 * Generates splashes if at surface of water, entering water, or bubbles
 * if underwater.
 */
void obj_splash(s32 waterY, s32 objY) {
    u32 globalTimer = gGlobalTimer;

    // Spawns waves if near surface of water and plays a noise if entering.
    if ((f32)(waterY + 30) > o->oPosY && o->oPosY > (f32)(waterY - 30)) {
        // A cap can rest on the surface for many updates; its generic object
        // step should not leave a continuous trail of surface waves.
        if (!obj_has_behavior(o, bhvNormalCap) && !obj_has_behavior(o, bhvWingCap)
            && !obj_has_behavior(o, bhvMetalCap) && !obj_has_behavior(o, bhvVanishCap)) {
            try_spawn_particle(o, MODEL_IDLE_WATER_WAVE, bhvObjectWaterWave, PARTICLE_PRIORITY_AMBIENT);
        }

        if (o->oVelY < -20.0f) {
            cur_obj_play_sound_2(SOUND_OBJ_DIVING_INTO_WATER);
        }
    }

    // Spawns bubbles if underwater.
    if ((objY + 50) < waterY && !(globalTimer & 31)) {
        try_spawn_particle(o, MODEL_WHITE_PARTICLE_SMALL, bhvObjectBubble, PARTICLE_PRIORITY_AMBIENT);
    }
}

/**
 * Move the current object through walls, water, floors and gravity; return ObjCollisionFlags.
 * Publish the attempted destination's floor to oFloor/oFloorHeight and shared sObjFloor.
 * GROUNDED and NO_Y_VEL compare s32-truncated values; the floor pointer may be NULL.
 * Callers apply floor hazards separately with obj_check_floor_death where needed.
 */
s16 object_step(void) {
    f32 objX = o->oPosX;
    f32 objY = o->oPosY;
    f32 objZ = o->oPosZ;

    f32 floorY;
    f32 waterY = FLOOR_LOWER_LIMIT_MISC;

    f32 objVelX = o->oForwardVel * sins(o->oMoveAngleYaw);
    f32 objVelZ = o->oForwardVel * coss(o->oMoveAngleYaw);

    s16 collisionFlags = 0;

    // Find any wall collisions, receive the push, and set the flag.
    if (obj_find_wall(objX + objVelX, objY, objZ + objVelZ, objVelX, objVelZ) == 0) {
        collisionFlags += OBJ_COL_FLAG_HIT_WALL;
    }

    floorY = find_floor(objX + objVelX, objY, objZ + objVelZ, &sObjFloor);

    o->oFloor       = sObjFloor;
    o->oFloorHeight = floorY;

    if (turn_obj_away_from_steep_floor(sObjFloor, floorY, objVelX, objVelZ) == 1) {
        waterY = find_water_level(objX + objVelX, objZ + objVelZ);
        if (waterY > objY) {
            calc_new_obj_vel_and_pos_y_underwater(sObjFloor, floorY, objVelX, objVelZ, waterY);
            collisionFlags += OBJ_COL_FLAG_UNDERWATER;
        } else {
            calc_new_obj_vel_and_pos_y(sObjFloor, floorY, objVelX, objVelZ);
        }
    } else {
        // Treat any awkward floors similar to a wall.
        collisionFlags +=
            ((collisionFlags & OBJ_COL_FLAG_HIT_WALL) ^ OBJ_COL_FLAG_HIT_WALL);
    }

    obj_update_pos_vel_xz();

    if (sObjFloor && (o->oPosY >= floorY) && (o->oPosY < floorY + 37)) {
        obj_orient_graph(o, sObjFloor->normal.x, sObjFloor->normal.y, sObjFloor->normal.z);
    }

    if ((s32) o->oPosY == (s32) floorY) {
        collisionFlags += OBJ_COL_FLAG_GROUNDED;
    }

    if ((s32) o->oVelY == 0) {
        collisionFlags += OBJ_COL_FLAG_NO_Y_VEL;
    }

    // Generate a splash if in water.
    obj_splash(waterY, o->oPosY);
    return collisionFlags;
}

/**
 * Takes an object step but does not orient with the object's floor.
 * Used for boulders, falling pillars, and the rolling snowman body.
 * Floor orientation is always enabled again before returning the step flags.
 */
s16 object_step_without_floor_orient(void) {
    sOrientObjWithFloor = FALSE;
    s16 collisionFlags = object_step();
    sOrientObjWithFloor = TRUE;

    return collisionFlags;
}

/**
 * Uses an object's forward velocity and yaw to move its X, Y, and Z positions.
 * This does accept an object as an argument, though it is always called with `o`.
 */
void obj_move_xyz_using_fvel_and_yaw(struct Object *obj) {
    obj->oVelX = obj->oForwardVel * sins(obj->oMoveAngleYaw);
    obj->oVelZ = obj->oForwardVel * coss(obj->oMoveAngleYaw);

    vec3f_add(&obj->oPosVec, &obj->oVelVec);
}

/**
 * Test an exclusive 3D radius in world units around Mario's graphical position.
 */
s32 is_point_within_radius_of_mario(f32 x, f32 y, f32 z, s32 radius) {
    f32 dx = x - gMarioObject->header.gfx.pos[0];
    f32 dy = y - gMarioObject->header.gfx.pos[1];
    f32 dz = z - gMarioObject->header.gfx.pos[2];

    return sqr(dx) + sqr(dy) + sqr(dz) < (f32) sqr(radius);
}

/**
 * Test an exclusive 3D radius in world units around the object's logical oPos position.
 */
s32 is_point_close_to_object(struct Object *obj, f32 x, f32 y, f32 z, s32 radius) {
    f32 dx = x - obj->oPosX;
    f32 dy = y - obj->oPosY;
    f32 dz = z - obj->oPosZ;

    return sqr(dx) + sqr(dy) + sqr(dz) < (f32) sqr(radius);
}

/**
 * Set GRAPH_RENDER_INVISIBLE from the exclusive radius around graphical Mario.
 * Hiding also clears ACTIVE_FLAG_BLINKING; each call owns the invisibility flag.
 */
void set_object_visibility(struct Object *obj, s32 radius) {
    s32 hidden = !is_point_within_radius_of_mario(obj->oPosX, obj->oPosY, obj->oPosZ, radius);
    if (hidden) {
        obj->activeFlags &= ~ACTIVE_FLAG_BLINKING;
    }
    COND_BIT(hidden, obj->header.gfx.node.flags, GRAPH_RENDER_INVISIBLE);
}

/**
 * Return TRUE when graphical Mario is near the supplied home point, leaving yaw unchanged.
 * Otherwise turn toward home X/Z by up to 320 binary-angle units and return FALSE.
 * homeCheckY affects only proximity; callers may supply current Y instead of home Y.
 */
s32 obj_return_home_if_safe(struct Object *obj, f32 homeX, f32 homeCheckY, f32 homeZ, s32 marioRadius) {
    f32 homeDistX = homeX - obj->oPosX;
    f32 homeDistZ = homeZ - obj->oPosZ;
    s16 angleTowardsHome = atan2s(homeDistZ, homeDistX);

    if (is_point_within_radius_of_mario(homeX, homeCheckY, homeZ, marioRadius)) {
        return TRUE;
    } else {
        obj->oMoveAngleYaw = approach_s16_symmetric(obj->oMoveAngleYaw, angleTowardsHome, 320);
    }

    return FALSE;
}

/**
 * On a random selection, replace persistent oHomeX/Z with jitter around the supplied center.
 * Each axis uses homeJitterHalfWidth in world units; homeY is unused.
 * Always steer toward the stored home by up to 320 binary-angle units per call.
 */
void obj_return_and_displace_home(struct Object *obj, f32 homeX, UNUSED f32 homeY, f32 homeZ,
                                  s32 homeJitterHalfWidth) {
    s16 angleToNewHome;
    f32 homeDistX, homeDistZ;

    if ((s32)(random_float() * 50.0f) == 0) {
        obj->oHomeX =
            (f32) (homeJitterHalfWidth * 2) * random_float() - (f32) homeJitterHalfWidth + homeX;
        obj->oHomeZ =
            (f32) (homeJitterHalfWidth * 2) * random_float() - (f32) homeJitterHalfWidth + homeZ;
    }

    homeDistX = obj->oHomeX - obj->oPosX;
    homeDistZ = obj->oHomeZ - obj->oPosZ;
    angleToNewHome = atan2s(homeDistZ, homeDistX);
    obj->oMoveAngleYaw = approach_s16_symmetric(obj->oMoveAngleYaw, angleToNewHome, 320);
}

/**
 * Test facing with a signed 16-bit angle delta, strict sine bounds and a positive cosine.
 * currentAngle/targetAngle contribute their low 16 bits; angleRange uses binary-angle units.
 */
s32 obj_check_if_facing_toward_angle(u32 currentAngle, u32 targetAngle, s16 angleRange) {
    s16 angleDelta = (u16) targetAngle - (u16) currentAngle;

    if (((f32) sins(-angleRange) < (f32) sins(angleDelta))
        && ((f32) sins(angleDelta) < (f32) sins(angleRange)) && (coss(angleDelta) > 0)) {
        return TRUE;
    }

    return FALSE;
}

/**
 * Query walls at a world-space point with radius and a 10-unit vertical offset.
 * TRUE means a reported hit and writes corrected-minus-input XYZ to outDisplacement.
 * FALSE leaves the output unchanged; solver corrections without reported hits are omitted.
 */
s32 obj_find_wall_displacement(Vec3f outDisplacement, f32 x, f32 y, f32 z, f32 radius) {
    struct WallCollisionData hitbox;
    hitbox.x = x;
    hitbox.y = y;
    hitbox.z = z;
    hitbox.offsetY = 10.0f;
    hitbox.radius = radius;

    if (find_wall_collisions(&hitbox) != 0) {
        outDisplacement[0] = hitbox.x - x;
        outDisplacement[1] = hitbox.y - y;
        outDisplacement[2] = hitbox.z - z;
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * Emit yellow rewards at the object; callers own ordinary stored-loot accounting.
 * An arena override may replace the reward. Bundles use the configured ring; a single
 * uses fixed forward speed, randomized vertical speed and a launch aimed at Mario.
 */
void obj_spawn_yellow_coins(struct Object *obj, s8 nCoins) {
    if (bowser_arena_try_drop_coin(obj)) {
        return;
    }
    struct Object *coin;
    s8 count;

    if (nCoins > 1) {
        obj_spawn_coin_ring(obj, nCoins, 0.0f);
        return;
    }

    for (count = 0; count < nCoins; count++) {
        coin = spawn_object(obj, MODEL_YELLOW_COIN, bhvMovingYellowCoin);
        coin->oForwardVel = 10.0f;
        coin->oVelY = random_float() * 40 + 20;
        coin->oMoveAngleYaw = obj_angle_to_object(coin, gMarioObject);
    }
}

/**
 * Use the existing action timer: flicker for 40 updates starting at flickerStartTimer,
 * then deactivate and return TRUE. Earlier calls return FALSE.
 * Blink phase follows the absolute oTimer parity; this helper leaves the timer unchanged.
 */
s32 obj_flicker_and_disappear(struct Object *obj, s16 flickerStartTimer) {
    if (obj->oTimer < flickerStartTimer) {
        return FALSE;
    }

    if (obj->oTimer < flickerStartTimer + 40) {
        obj_set_blink_phase(obj, obj->oTimer & 0x1);
    } else {
        obj->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        return TRUE;
    }

    return FALSE;
}

/**
 * Checks if a given room is Mario's current room, even if on an object.
 */
s32 current_mario_room_check(RoomData room) {
    s32 result;

    // Since object surfaces have room 0, this tests if the surface is an
    // object first and uses the last room if so.
    if (gMarioCurrentRoom == 0) {
        return room == sPrevCheckMarioRoom;
    } else {
        result = room == gMarioCurrentRoom;

        sPrevCheckMarioRoom = gMarioCurrentRoom;
    }

    return result;
}

/**
 * Latch a request after distance and both facing checks; an existing TRUE latch permits
 * continuation even when those entry checks fail. dialogRadius is truncated to s32 world units.
 * marioDialogAction selects Mario's entry pose. Return DIALOG_RESPONSE_NONE while pending
 * or not started; completion stops Mario's dialog, clears the caller's latch and returns the response.
 */
s32 trigger_obj_dialog_when_facing(s32 *dialogLatched, s16 dialogID, f32 dialogRadius,
                                   s32 marioDialogAction) {
    if ((is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, (s32) dialogRadius)
         && obj_check_if_facing_toward_angle(o->oFaceAngleYaw,
                                             gMarioObject->header.gfx.angle[1] + 0x8000, 0x1000)
         && obj_check_if_facing_toward_angle(o->oMoveAngleYaw, o->oAngleToMario, 0x1000))
        || (*dialogLatched == TRUE)) {
        *dialogLatched = TRUE;

        if (set_mario_npc_dialog(marioDialogAction) == MARIO_DIALOG_STATUS_SPEAK) {
            s16 dialogResponse = cutscene_object_with_dialog(CUTSCENE_DIALOG, o, dialogID);
            if (dialogResponse != DIALOG_RESPONSE_NONE) {
                set_mario_npc_dialog(MARIO_DIALOG_STOP);
                *dialogLatched = FALSE;
                return dialogResponse;
            }
            return DIALOG_RESPONSE_NONE;
        }
    }

    return DIALOG_RESPONSE_NONE;
}

/**
 * A grounded, non-NULL floor selects lava/death-plane action.
 * Callers own handling that action and deleting the object.
 */
void obj_check_floor_death(s16 collisionFlags, struct Surface *floor) {
    if (floor == NULL) {
        return;
    }

    if ((collisionFlags & OBJ_COL_FLAG_GROUNDED) == OBJ_COL_FLAG_GROUNDED) {
        switch (floor->type) {
            case SURFACE_BURNING:
                o->oAction = OBJ_ACT_LAVA_DEATH;
                break;
            case SURFACE_VERTICAL_WIND:
            case SURFACE_DEATH_PLANE:
                o->oAction = OBJ_ACT_DEATH_PLANE_DEATH;
                break;
            default:
                break;
        }
    }
}

/**
 * Controls an object dying in lava by creating smoke, sinking the object, playing
 * audio, and eventually despawning it. Returns TRUE when the obj is dead.
 */
s32 obj_lava_death(void) {
    struct Object *deathSmoke;

    if (o->oTimer > 30) {
        player_stats_enemy_defeated(o);
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        return TRUE;
    } else {
        // Sinking effect
        o->oPosY -= 10.0f;
    }

    if ((o->oTimer % 8) == 0) {
        cur_obj_play_sound_2(SOUND_OBJ_BULLY_EXPLODE_LAVA);
        deathSmoke = spawn_object(o, MODEL_SMOKE, bhvBobombBullyDeathSmoke);
        deathSmoke->oPosX += random_float() * 20.0f;
        deathSmoke->oPosY += random_float() * 20.0f;
        deathSmoke->oPosZ += random_float() * 20.0f;
        deathSmoke->oForwardVel = random_float() * 10.0f;
    }

    return FALSE;
}

/**
 * Spawns an orange number object relatively, such as those that count up for secrets.
 */
void spawn_orange_number(s8 behParam, s16 relX, s16 relY, s16 relZ) {
#ifdef DIALOG_INDICATOR
    if (behParam > ORANGE_NUMBER_F) return;
#else
    if (behParam > ORANGE_NUMBER_9) return;
#endif

    struct Object *orangeNumber = spawn_object_relative(behParam, relX, relY, relZ, o, MODEL_NUMBER, bhvOrangeNumber);
    orangeNumber->oPosY += 25.0f;
    orangeNumber->oOrangeNumberOffset = relX;
    orangeNumber->oHomeX = o->oPosX;
    orangeNumber->oHomeZ = o->oPosZ;
}

/**
 * Unused variables for debug_sequence_tracker.
 */
s8 sDebugSequenceTracker = 0;
s8 sDebugTimer = 0;

/**
 * Unused presumably debug function that tracks for a sequence of inputs.
 */
UNUSED s32 debug_sequence_tracker(s16 debugInputSequence[]) {
    // If end of sequence reached, return true.
    if (debugInputSequence[sDebugSequenceTracker] == 0) {
        sDebugSequenceTracker = 0;
        return TRUE;
    }

    // If the button pressed is next in sequence, reset timer and progress to next value.
    if (debugInputSequence[sDebugSequenceTracker] & gPlayer1Controller->buttonPressed) {
        sDebugSequenceTracker++;
        sDebugTimer = 0;
    // If wrong input or timer reaches 10, reset sequence progress.
    } else if (sDebugTimer == 10 || gPlayer1Controller->buttonPressed != 0) {
        sDebugSequenceTracker = 0;
        sDebugTimer = 0;
        return FALSE;
    }
    sDebugTimer++;

    return FALSE;
}

#include "behaviors/moving_coin.inc.c"
#include "behaviors/seaweed.inc.c"
#include "behaviors/bobomb.inc.c"
#include "behaviors/cannon_door.inc.c"
#include "behaviors/whirlpool.inc.c"
#include "behaviors/amp.inc.c"
#include "behaviors/butterfly.inc.c"
#include "behaviors/hoot.inc.c"
#include "behaviors/beta_holdable_object.inc.c"
#include "behaviors/bubble.inc.c"
#include "behaviors/water_wave.inc.c"
#include "behaviors/explosion.inc.c"
#include "behaviors/respawner.inc.c"
#include "behaviors/bully.inc.c"
#include "behaviors/water_ring.inc.c"
#include "behaviors/bowser_bomb.inc.c"
#include "behaviors/celebration_star.inc.c"
#include "behaviors/drawbridge.inc.c"
#include "behaviors/bomp.inc.c"
#include "behaviors/sliding_platform.inc.c"
#include "behaviors/moneybag.inc.c"
#include "behaviors/bowling_ball.inc.c"
#include "behaviors/cruiser.inc.c"
#include "behaviors/spindel.inc.c"
#include "behaviors/pyramid_wall.inc.c"
#include "behaviors/pyramid_elevator.inc.c"
#include "behaviors/pyramid_top.inc.c"
#include "behaviors/sound_waterfall.inc.c"
#include "behaviors/sound_volcano.inc.c"
#include "behaviors/castle_flag.inc.c"
#include "behaviors/sound_birds.inc.c"
#include "behaviors/sound_ambient.inc.c"
#include "behaviors/sound_sand.inc.c"
#include "behaviors/castle_cannon_grate.inc.c"
#include "behaviors/snowman.inc.c"
#include "behaviors/boulder.inc.c"
#include "behaviors/cap.inc.c"
#include "behaviors/koopa_shell.inc.c"
#include "behaviors/spawn_star.inc.c"
#include "behaviors/red_coin.inc.c"
#include "behaviors/hidden_star.inc.c"
#include "behaviors/rolling_log.inc.c"
#include "behaviors/mushroom_1up.inc.c"
#include "behaviors/controllable_platform.inc.c"
#include "behaviors/breakable_box_small.inc.c"
#include "behaviors/snow_mound.inc.c"
#include "behaviors/floating_platform.inc.c"
#include "behaviors/arrow_lift.inc.c"
#include "behaviors/orange_number.inc.c"
#include "behaviors/manta_ray.inc.c"
#include "behaviors/falling_pillar.inc.c"
#include "behaviors/floating_box.inc.c"
#include "behaviors/decorative_pendulum.inc.c"
#include "behaviors/treasure_chest.inc.c"
#include "behaviors/mips.inc.c"
#include "behaviors/yoshi.inc.c"
