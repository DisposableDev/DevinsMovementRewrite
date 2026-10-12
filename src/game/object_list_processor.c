#include "game/bowser_arena.h"
#include <PR/ultratypes.h>

#include "sm64.h"
#include "area.h"
#include "behavior_data.h"
#include "obj_behaviors_2.h"
#include "camera.h"
#include "compact_explosion_bubbles.h"
#include "debug.h"
#include "debugger/assert.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "engine/surface_collision.h"
#include "engine/surface_load.h"
#include "engine/math_util.h"
#include "interaction.h"
#include "level_update.h"
#include "mario.h"
#include "mario_actions_submerged.h"
#include "memory.h"
#include "object_collision.h"
#include "object_helpers.h"
#include "object_platform_motion.h"
#include "object_list_processor.h"
#include "platform_displacement.h"
#include "physics_lab.h"
#include "particle_budget.h"
#include "spawn_object.h"
#include "puppyprint.h"
#include "profiling.h"


/**
 * Flags controlling what debug info is displayed.
 */
s32 gDebugInfoFlags;

/**
 * The number of times per frame find_floor found no floor beneath an
 * object, and therefore either returned a dynamic floor or NULL.
 */
s32 gNumFindFloorMisses;

/**
 * An unused debug counter with the label "WALL".
 */
s32 gUnknownWallCount;

/**
 * Roughly the number of objects that have been processed this frame so far.
 * A bug in update_terrain_objects makes this count inaccurate.
 */
u32 gObjectCounter;

/**
 * The number of times find_floor, find_ceil, and find_wall_collisions have been called respectively.
 */
struct NumTimesCalled gNumCalls;

/**
 * An array of debug controls that could be used to tweak in-game parameters.
 * The only used rows are [4] and [5] (effectinfo and enemyinfo).
 */
s16 gDebugInfo[16][8];
s16 gDebugInfoOverwrite[16][8];

/**
 * A set of flags to control which objects are updated on a given frame.
 * This is used during dialog and cutscenes to freeze most objects in place.
 */
u32 gTimeStopState;

/**
 * The pool that objects are allocated from.
 */
struct Object gObjectPool[OBJECT_POOL_CAPACITY];

/**
 * A special object whose purpose is to act as a parent for macro objects.
 */
struct Object gMacroObjectDefaultParent;

/**
 * A pointer to gObjectListArray.
 * Given an object list index idx, gObjectLists[idx] is the head of a doubly
 * linked list of all currently spawned objects in the list.
 */
struct ObjectNode *gObjectLists;

/**
 * A singly linked list of available slots in the object pool.
 */
struct ObjectNode gFreeObjectList;

/**
 * The object representing Mario.
 */
struct Object *gMarioObject;

/**
 * An object variable that may have been used to represent the planned
 * second player. This is speculation, based on its position and its usage in
 * shadow.c.
 */
// struct Object *gLuigiObject;

/**
 * The object whose behavior script is currently being updated.
 * This object is used frequently in object behavior code, and so is often
 * aliased as "o".
 */
struct Object *gCurrentObject;

/**
 * The next object behavior command to be executed.
 */
const BehaviorScript *gCurBhvCommand;

/**
 * The number of objects that were processed last frame, which may miss some
 * objects that were spawned last frame and all objects that were spawned this
 * frame. It also includes objects that were unloaded last frame.
 * Besides this, a bug in update_terrain_objects makes this count inaccurate.
 */
s16 gPrevFrameObjectCount;

/**
 * The total number of surface nodes allocated (a node is allocated for each
 * spatial partition cell that a surface intersects).
 */
s32 gSurfaceNodesAllocated;

/**
 * The total number of surfaces allocated.
 */
s32 gSurfacesAllocated;

/**
 * The number of nodes that have been created for surfaces.
 */
s32 gNumStaticSurfaceNodes;

/**
 * The number of surfaces in the pool.
 */
s32 gNumStaticSurfaces;

/**
 * A pool used by chain chomp and wiggler to allocate their body parts.
 */
struct MemoryPool *gObjectMemoryPool;

s16 gCollisionFlags = COLLISION_FLAGS_NONE;
TerrainData *gEnvironmentRegions;
s32 gEnvironmentLevels[20];
struct TransitionRoomData gDoorAdjacentRooms[MAX_NUM_TRANSITION_ROOMS];
s16 gMarioCurrentRoom;
s16 gTHIWaterDrained;
s16 gTTCSpeedSetting;
s16 gMarioShotFromCannon;
s16 gCCMEnteredSlide;
s16 gNumRoomedObjectsInMarioRoom;
s16 gNumRoomedObjectsNotInMarioRoom;
s16 gWDWWaterLevelChanging;
s16 gMarioOnMerryGoRound;

/**
 * Nodes used to represent the doubly linked object lists.
 */
struct ObjectNode gObjectListArray[16];

/**
 * The order that object lists are processed in a frame.
 */
s8 sObjectListUpdateOrder[] = { OBJ_LIST_SPAWNER,
                                OBJ_LIST_SURFACE,
                                OBJ_LIST_POLELIKE,
                                OBJ_LIST_PLAYER,
                                OBJ_LIST_PUSHABLE,
                                OBJ_LIST_GENACTOR,
                                OBJ_LIST_DESTRUCTIVE,
                                OBJ_LIST_LEVEL,
                                OBJ_LIST_DEFAULT,
                                OBJ_LIST_UNIMPORTANT,
                                -1 };

/**
 * Info needed to spawn particles and keep track of which have been spawned for
 * an object.
 */
struct ParticleProperties {
    u32 particleFlag;
    u32 activeParticleFlag;
    ModelID16 model;
    const BehaviorScript *behavior;
};

/**
 * A table mapping particle flags to various properties use when spawning a particle.
 */
struct ParticleProperties sParticleTypes[] = {
    { PARTICLE_DUST, ACTIVE_PARTICLE_DUST, MODEL_MIST, bhvMistParticleSpawner },
    { PARTICLE_VERTICAL_STAR, ACTIVE_PARTICLE_V_STAR, MODEL_NONE, bhvVertStarParticleSpawner },
    { PARTICLE_HORIZONTAL_STAR, ACTIVE_PARTICLE_H_STAR, MODEL_NONE, bhvHorStarParticleSpawner },
    { PARTICLE_SPARKLES, ACTIVE_PARTICLE_SPARKLES, MODEL_SPARKLES, bhvSparkleParticleSpawner },
    { PARTICLE_BUBBLE, ACTIVE_PARTICLE_BUBBLE, MODEL_BUBBLE, bhvBubbleParticleSpawner },
    { PARTICLE_WATER_SPLASH, ACTIVE_PARTICLE_WATER_SPLASH, MODEL_WATER_SPLASH, bhvWaterSplash },
    { PARTICLE_IDLE_WATER_WAVE, ACTIVE_PARTICLE_IDLE_WATER_WAVE, MODEL_IDLE_WATER_WAVE,
      bhvIdleWaterWave },
    { PARTICLE_PLUNGE_BUBBLE, ACTIVE_PARTICLE_PLUNGE_BUBBLE, MODEL_WHITE_PARTICLE_SMALL,
      bhvPlungeBubble },
    { PARTICLE_WATER_ENTRY_BUBBLE, ACTIVE_PARTICLE_PLUNGE_BUBBLE, MODEL_NONE, bhvWaterEntryBubble },
    { PARTICLE_WAVE_TRAIL, ACTIVE_PARTICLE_WAVE_TRAIL, MODEL_WAVE_TRAIL, bhvWaveTrail },
    { PARTICLE_FIRE, ACTIVE_PARTICLE_FIRE, MODEL_RED_FLAME, bhvFireParticleSpawner },
    { PARTICLE_SHALLOW_WATER_WAVE, ACTIVE_PARTICLE_SHALLOW_WATER_WAVE, MODEL_NONE,
      bhvShallowWaterWave },
    { PARTICLE_SHALLOW_WATER_SPLASH, ACTIVE_PARTICLE_SHALLOW_WATER_SPLASH, MODEL_NONE,
      bhvShallowWaterSplash },
    { PARTICLE_LEAF, ACTIVE_PARTICLE_LEAF, MODEL_NONE, bhvLeafParticleSpawner },
    { PARTICLE_SNOW, ACTIVE_PARTICLE_SNOW, MODEL_NONE, bhvSnowParticleSpawner },
    { PARTICLE_BREATH, ACTIVE_PARTICLE_BREATH, MODEL_NONE, bhvBreathParticleSpawner },
    { PARTICLE_DIRT, ACTIVE_PARTICLE_DIRT, MODEL_NONE, bhvDirtParticleSpawner },
    { PARTICLE_MIST_CIRCLE, ACTIVE_PARTICLE_MIST_CIRCLE, MODEL_NONE, bhvMistCircParticleSpawner },
    { PARTICLE_WATER_DOWN_MIST, ACTIVE_PARTICLE_WATER_DOWN_MIST, MODEL_NONE, bhvWaterDownMistSpawner },
    { PARTICLE_VANISH_WATER_RING, ACTIVE_PARTICLE_VANISH_WATER_RING, MODEL_NONE,
      bhvVanishWaterRingSpawner },
    { PARTICLE_TRIANGLE, ACTIVE_PARTICLE_TRIANGLE, MODEL_NONE, bhvTriangleParticleSpawner },
    { PARTICLE_NONE, ACTIVE_PARTICLE_NONE, MODEL_NONE, NULL },
};

/**
 * Copy position, velocity, and angle variables from MarioState to the Mario
 * object.
 */
void copy_mario_state_to_object(void) {
    s32 i = 0;
    // L is real
    if (gCurrentObject != gMarioObject) {
        i++;
    }

    gCurrentObject->oVelX = gMarioStates[i].vel[0];
    gCurrentObject->oVelY = gMarioStates[i].vel[1];
    gCurrentObject->oVelZ = gMarioStates[i].vel[2];

    gCurrentObject->oPosX = gMarioStates[i].pos[0];
    gCurrentObject->oPosY = gMarioStates[i].pos[1];
    gCurrentObject->oPosZ = gMarioStates[i].pos[2];

    gCurrentObject->oMoveAnglePitch = gCurrentObject->header.gfx.angle[0];
    gCurrentObject->oMoveAngleYaw = gCurrentObject->header.gfx.angle[1];
    gCurrentObject->oMoveAngleRoll = gCurrentObject->header.gfx.angle[2];

    gCurrentObject->oFaceAnglePitch = gCurrentObject->header.gfx.angle[0];
    gCurrentObject->oFaceAngleYaw = gCurrentObject->header.gfx.angle[1];
    gCurrentObject->oFaceAngleRoll = gCurrentObject->header.gfx.angle[2];

    gCurrentObject->oAngleVelPitch = gMarioStates[i].angleVel[0];
    gCurrentObject->oAngleVelYaw = gMarioStates[i].angleVel[1];
    gCurrentObject->oAngleVelRoll = gMarioStates[i].angleVel[2];
}

/**
 * Spawn a particle at gCurrentObject's location.
 */
void spawn_particle(u32 activeParticleFlag, ModelID16 model, const BehaviorScript *behavior) {
    if (!(gCurrentObject->oActiveParticleFlags & activeParticleFlag)) {
        struct Object *particle;
        enum ParticlePriority priority = particle_budget_priority(segmented_to_virtual(behavior));
        if (priority == PARTICLE_PRIORITY_PROTECTED) {
            priority = PARTICLE_PRIORITY_FEEDBACK; // Protected visual controller, optional creation.
        }
        particle = try_spawn_particle(gCurrentObject, model, behavior, priority);
        if (particle != NULL) {
            if (activeParticleFlag == ACTIVE_PARTICLE_V_STAR
                && (mario_ground_twirl_is_active(gMarioState) || mario_air_twirl_is_active(gMarioState)
                    || mario_swimming_roll_is_active())) {
                // Capture the impact's twirl state before the spawner's next update.
                particle->oBehParams2ndByte = 3;
            }
            particle_budget_own_flags(particle, gCurrentObject, activeParticleFlag);
        }
    }
}

/**
 * Mario's primary behavior update function.
 */
void bhv_mario_update(void) {
    u32 particleFlags = 0;
    s32 i;

    particleFlags = execute_mario_action(gCurrentObject);
    gCurrentObject->oMarioParticleFlags = particleFlags;

    // Mario code updates MarioState's versions of position etc, so we need
    // to sync it with the Mario object
    copy_mario_state_to_object();

    i = 0;
    while (sParticleTypes[i].particleFlag != 0) {
        if (particleFlags & sParticleTypes[i].particleFlag) {
            spawn_particle(sParticleTypes[i].activeParticleFlag, sParticleTypes[i].model,
                           sParticleTypes[i].behavior);
        }

        i++;
    }
}

// A twirl-hit enemy is launched without running its behavior, then pauses briefly after landing.
#define GROUND_TWIRL_KNOCKBACK_GRAVITY 4.0f

void bhv_ground_twirl_stun_star_loop(void) {
    struct Object *enemy = o->parentObj;
    f32 size;
    f32 radius;
    s16 angle;

    if (enemy == NULL || enemy->activeFlags == ACTIVE_FLAG_DEACTIVATED
        || enemy->oGroundTwirlStunTimer == 0 || enemy->oGroundTwirlContactRebound != 0
        || enemy->oGroundTwirlLastHitId != o->oGroundTwirlLastHitId) {
        obj_mark_for_deletion(o);
        return;
    }

    size = CLAMP(MAX(enemy->hitboxRadius / 65.0f, enemy->hitboxHeight / 113.0f), 0.5f, 2.5f);
    radius = MAX(28.0f, enemy->hitboxRadius * 0.65f);
    angle = o->oTimer * 0x800 + o->oBehParams2ndByte * 0x5555;
    o->oPosX = enemy->oPosX + radius * sins(angle);
    o->oPosY = enemy->oPosY - enemy->hitboxDownOffset + enemy->hitboxHeight + 25.0f * size;
    o->oPosZ = enemy->oPosZ + radius * coss(angle);
    obj_scale(o, 0.14f * size);
}

static s32 ground_twirl_enemy_needs_water_update(struct Object *obj) {
    if ((obj->activeFlags & ACTIVE_FLAG_IGNORE_ENV_BOXES)
        || find_water_level(obj->oPosX, obj->oPosZ) < obj->oPosY) {
        return FALSE;
    }
    if (obj_has_behavior(obj, bhvChuckya) && obj->oAction == 2) {
        return TRUE;
    }
    if (obj->oHealth > 0 || obj->oGravity + obj->oBuoyancy > 0.0f
        || find_water_level(obj->oPosX, obj->oPosZ) - obj->oPosY < 10.0f) {
        return FALSE;
    }
    // These enemies use the shared water-death check in their normal update.
    return obj_has_behavior(obj, bhvGoomba)
#ifdef FLOOMBAS
           || obj_has_behavior(obj, bhvFloomba)
#endif
           || obj_has_behavior(obj, bhvKoopa) || obj_has_behavior(obj, bhvSpiny)
           || obj_has_behavior(obj, bhvMrBlizzard) || obj_has_behavior(obj, bhvSkeeter)
           || obj_has_behavior(obj, bhvFlyGuy) || obj_has_behavior(obj, bhvMontyMole)
           || obj_has_behavior(obj, bhvSwoop) || obj_has_behavior(obj, bhvKlepto)
           || obj_has_behavior(obj, bhvSnufit) || obj_has_behavior(obj, bhvEnemyLakitu);
}

static void stop_ground_twirl_stunned_enemy(struct Object *obj) {
    obj->oForwardVel = 0.0f;
    obj->oVelX = 0.0f;
    obj->oVelY = 0.0f;
    obj->oVelZ = 0.0f;
}

// Monty Mole's exposed pose and Snufit's centered body need raised floor support.
// Keep the real floor metadata so native burial and hazard checks still work.
static s32 ground_twirl_support_raised_enemy(struct Object *obj, s32 descending) {
    if ((!obj_has_behavior(obj, bhvMontyMole) && !obj_has_behavior(obj, bhvSnufit)) || !descending
        || obj->oFloor == NULL || obj->oFloorType == SURFACE_BURNING
        || obj->oFloorType == SURFACE_DEATH_PLANE || obj->oFloorType == SURFACE_VERTICAL_WIND
        || (obj->oMoveFlags & OBJ_MOVE_MASK_IN_WATER) || obj->oPosY > obj->oFloorHeight + 50.0f) {
        return FALSE;
    }
    obj->oPosY = obj->oFloorHeight + 50.0f;
    obj->oVelY = 0.0f;
    // This is exposed-pose support, not contact with the real floor. In
    // particular, HIDE must still sink to the floor after the stun ends.
    obj->oMoveFlags &= ~(OBJ_MOVE_MASK_ON_GROUND | OBJ_MOVE_BOUNCE | OBJ_MOVE_LEFT_GROUND
                         | OBJ_MOVE_UNDERWATER_ON_GROUND);
    return TRUE;
}

// Restore native gravity and wall radius before any hazard or landing decision.
static void update_ground_twirl_stun_motion(s32 fallingStun) {
    f32 gravity = gCurrentObject->oGravity;
    f32 wallRadius = gCurrentObject->oWallHitboxRadius;
    s32 contactRebound = gCurrentObject->oGroundTwirlContactRebound > 0;
    f32 startY = gCurrentObject->oPosY;
    s32 descending;
    s32 raisedEnemyLanded;

    gCurrentObject->header.gfx.node.flags &= ~GRAPH_RENDER_HAS_ANIMATION;
    // cur_obj_move_standard expects negative gravity; some enemies use positive gravity.
    gCurrentObject->oGravity = -MAX(absf(gravity), GROUND_TWIRL_KNOCKBACK_GRAVITY);
    gCurrentObject->oWallHitboxRadius =
        MAX(wallRadius, MIN(gCurrentObject->hitboxRadius * 0.7f, 90.0f));
    cur_obj_update_floor_and_walls();
    if (gCurrentObject->oMoveFlags & OBJ_MOVE_HIT_WALL) {
        gCurrentObject->oForwardVel = 0.0f;
    }
    descending = gCurrentObject->oVelY + gCurrentObject->oGravity <= 0.0f;
    cur_obj_move_stunned();
    // A native floor bounce can erase the incoming descent on a deep step.
    raisedEnemyLanded =
        ground_twirl_support_raised_enemy(gCurrentObject, descending || gCurrentObject->oPosY < startY);
    gCurrentObject->oGravity = gravity;
    gCurrentObject->oWallHitboxRadius = wallRadius;
    vec3_copy_y_off(gCurrentObject->header.gfx.pos, &gCurrentObject->oPosVec,
                    gCurrentObject->oGraphYOffset);
    gCurrentObject->oInteractStatus = INT_STATUS_NONE;

    if (((gCurrentObject->oMoveFlags & OBJ_MOVE_MASK_IN_WATER)
         && ground_twirl_enemy_needs_water_update(gCurrentObject))
        || ((gCurrentObject->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND)
            && (gCurrentObject->oMoveFlags & (OBJ_MOVE_ABOVE_LAVA | OBJ_MOVE_ABOVE_DEATH_BARRIER)))) {
        // Run native death and hazard handling on the next update.
        gCurrentObject->oGroundTwirlStunTimer = 0;
        gCurrentObject->oGroundTwirlContactRebound = 0;
        return;
    }

    if (fallingStun) {
        gCurrentObject->oGroundTwirlStunTimer--;
    }
    if (contactRebound) {
        gCurrentObject->oGroundTwirlContactRebound--;
    } else if (!fallingStun) {
        gCurrentObject->oGroundTwirlStunTimer++;
    }
    if (raisedEnemyLanded
        || ((gCurrentObject->oMoveFlags & (OBJ_MOVE_MASK_ON_GROUND | OBJ_MOVE_UNDERWATER_ON_GROUND))
            && gCurrentObject->oVelY <= 5.0f)) {
        if (contactRebound) {
            gCurrentObject->oGroundTwirlContactRebound = 0;
        } else if (!fallingStun) {
            gCurrentObject->oGroundTwirlStunTimer = MAX(gCurrentObject->oGroundTwirlStunDuration,
                                                        (s32) gCurrentObject->metalContactCooldown + 1);
        }
        stop_ground_twirl_stunned_enemy(gCurrentObject);
    } else if (fallingStun && gCurrentObject->oFloor == NULL
               && gCurrentObject->oPosY <= FLOOR_LOWER_LIMIT_MISC) {
        gCurrentObject->oGroundTwirlStunTimer = 0;
        stop_ground_twirl_stunned_enemy(gCurrentObject);
    } else if (!fallingStun && !contactRebound && gCurrentObject->oGroundTwirlStunTimer == 0) {
        if (gCurrentObject->oFloor != NULL || gCurrentObject->oPosY > FLOOR_LOWER_LIMIT_MISC) {
            // Keep the launch active until landing, including for flying enemies.
            gCurrentObject->oGroundTwirlStunTimer = -1;
        } else {
            // No floor and out of bounds: return control to the enemy behavior.
            stop_ground_twirl_stunned_enemy(gCurrentObject);
        }
    }
}

// Keep the rare stun path out of the ordinary update's register/stack usage.
static void update_ground_twirl_stunned_object(void) __attribute__((noinline));
static void update_ground_twirl_stunned_object(void) {
    if (gCurrentObject->metalContactPendingDefeat) {
        // Consume the committed contact once, before native AI can erase it or move home.
        gCurrentObject->metalContactPendingDefeat = FALSE;
        gCurrentObject->metalContactCooldown = 0;
        gCurrentObject->oGroundTwirlStunTimer = 0;
        gCurrentObject->oGroundTwirlContactRebound = 0;
        gCurrentObject->oGroundTwirlStunDuration = 0;
        gCurrentObject->header.gfx.node.flags |= GRAPH_RENDER_HAS_ANIMATION;
        cur_obj_defeat_metal_contact_enemy();
        return;
    }
    s32 isEnemy = mario_ground_twirl_is_enemy(gCurrentObject);
    if (isEnemy && gCurrentObject->metalContactCooldown != 0) {
        gCurrentObject->metalContactCooldown--;
    }
    // These slots are shared object fields. Only twirl targets may use them to
    // suppress their own update.
    if ((gCurrentObject->oGroundTwirlStunTimer == 0 && gCurrentObject->oGroundTwirlContactRebound == 0
         && gCurrentObject->oGroundTwirlStunDuration == 0)
        || !isEnemy) {
        cur_obj_update();
        return;
    }

    // A real attack still reaches the enemy's normal damage handler immediately.
    if ((gCurrentObject->oGroundTwirlStunTimer != 0 || gCurrentObject->oGroundTwirlContactRebound != 0)
        && (gCurrentObject->oInteractStatus & INT_STATUS_WAS_ATTACKED)) {
        gCurrentObject->oGroundTwirlStunTimer = 0;
        gCurrentObject->oGroundTwirlContactRebound = 0;
    }

    s32 fallingStun = FALSE;
    if (gCurrentObject->oGroundTwirlStunTimer > 0) {
        cur_obj_update_floor_and_walls();
        // Resting stun skips movement, so rising water may not refresh move flags.
        if (ground_twirl_enemy_needs_water_update(gCurrentObject)) {
            gCurrentObject->oGroundTwirlStunTimer = 0;
            gCurrentObject->oGroundTwirlContactRebound = 0;
            gCurrentObject->oMoveFlags |= OBJ_MOVE_ENTERED_WATER;
        } else if (obj_has_behavior(gCurrentObject, bhvMontyMole)
                   || obj_has_behavior(gCurrentObject, bhvSnufit)) {
            fallingStun = !ground_twirl_support_raised_enemy(gCurrentObject, TRUE);
        } else {
            fallingStun = !object_platform_motion_supported(gCurrentObject);
        }
        vec3_copy_y_off(gCurrentObject->header.gfx.pos, &gCurrentObject->oPosVec,
                        gCurrentObject->oGraphYOffset);
    }

    if (gCurrentObject->oGroundTwirlStunTimer < 0 || gCurrentObject->oGroundTwirlContactRebound > 0
        || fallingStun) {
        update_ground_twirl_stun_motion(fallingStun);
        return;
    }

    if (gCurrentObject->oGroundTwirlStunTimer > 0) {
        gCurrentObject->oGroundTwirlStunTimer--;
        stop_ground_twirl_stunned_enemy(gCurrentObject);
        gCurrentObject->oInteractStatus = INT_STATUS_NONE;
        gCurrentObject->header.gfx.node.flags &= ~GRAPH_RENDER_HAS_ANIMATION;
        return;
    }

    if (obj_has_behavior(gCurrentObject, bhvHeaveHo) && gCurrentObject->oGroundTwirlStunDuration != 0) {
        // Face yaw stayed fixed during stun; resume walking from that angle instead of the launch
        // angle.
        gCurrentObject->oMoveAngleYaw = gCurrentObject->oFaceAngleYaw;
        gCurrentObject->oGroundTwirlStunDuration = 0;
    }
    gCurrentObject->header.gfx.node.flags |= GRAPH_RENDER_HAS_ANIMATION;
    cur_obj_update();
}

static void update_object_with_ground_twirl_stun(void) {
    bowser_arena_recover_enemy(gCurrentObject);
    if (bowser_arena_phase_in(gCurrentObject)) {
        return;
    }
    if (!gCurrentObject->metalContactPendingDefeat && gCurrentObject->metalContactCooldown == 0
        && gCurrentObject->oGroundTwirlStunTimer == 0 && gCurrentObject->oGroundTwirlContactRebound == 0
        && gCurrentObject->oGroundTwirlStunDuration == 0) {
        cur_obj_update();
    } else {
        update_ground_twirl_stunned_object();
        if (obj_has_behavior(gCurrentObject, bhvSwoop)
            && !(gCurrentObject->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION)
            && gCurrentObject->oFloor != NULL && (gCurrentObject->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND)
            && !(gCurrentObject->oMoveFlags
                 & (OBJ_MOVE_MASK_IN_WATER | OBJ_MOVE_ABOVE_LAVA | OBJ_MOVE_ABOVE_DEATH_BARRIER))) {
            // A frozen flight tilt puts the wings below the origin on landing.
            // Level only the rendered resting pose; its mesh extends 7.5 units below it.
            gCurrentObject->header.gfx.angle[0] = 0;
            gCurrentObject->header.gfx.angle[2] = 0;
            gCurrentObject->header.gfx.pos[1] =
                MAX(gCurrentObject->header.gfx.pos[1],
                    gCurrentObject->oFloorHeight + 10.0f * gCurrentObject->header.gfx.scale[1]);
        }
    }
    bowser_arena_recover_enemy(gCurrentObject);
}

// A saved dynamic support must publish before its dependent body runs.
static void update_object_with_platform_motion(void) {
    struct Object *obj = gCurrentObject;
    s32 riderAtStart;
    u8 *updateStatus = object_platform_motion_start_update(obj, &riderAtStart);
    if (updateStatus == NULL) {
        return;
    }
    if (bbh_merry_go_round_update_dormancy(obj)) {
        profiler_count_object(TRUE);
        *updateStatus = ENEMY_PLATFORM_UPDATE_COMPLETE;
        return;
    }
    if (physics_lab_discard_distant_effect(obj)) {
        profiler_count_object(TRUE);
        *updateStatus = ENEMY_PLATFORM_UPDATE_COMPLETE;
        return;
    }
    // Refresh owner history before Bob-omb/Chomp sleep guards read it.
    physics_lab_update_enemy_hidden_state(obj);
    // Eligible distant enemies sleep; the Lab also permits active-state freezing.
    // Surfaces still publish collision; scripts and animation clocks remain paused.
    if ((obj->oFlags & OBJ_FLAG_ALLOW_ENEMY_SLEEP) && enemy_should_sleep(obj)) {
        obj->activeFlags &= ~ACTIVE_FLAG_BLINKING;
        obj->header.gfx.node.flags &= ~GRAPH_RENDER_HAS_ANIMATION;
        cur_obj_update_sleeping_visibility();
        // Resting hazards retain collision publication and drawing-range updates.
        if (obj->collisionData != NULL) {
            load_object_collision_model();
        }
        // Collision publication can restore rendering; hide enemy models afterward.
        if (obj->oFlags & OBJ_FLAG_HIDE_WHEN_SLEEPING) {
            obj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
        }
        profiler_count_object(TRUE);
        *updateStatus = ENEMY_PLATFORM_UPDATE_COMPLETE;
        return;
    }
    profiler_count_object(FALSE);
    const BehaviorScript *behaviorBeforeUpdate = obj->behavior;
    // Publish a saved support first, even when its rider precedes it in SURFACE.
    // Cyclic/stale support is rejected by the provider query.
    if (riderAtStart) {
        struct Object *provider = object_platform_motion_provider(obj);
        if (provider != NULL && !(gTimeStopState & TIME_STOP_ACTIVE)) {
            gCurrentObject = provider;
            update_object_with_platform_motion();
            gCurrentObject = obj;
        }
    }
    // Restore a sleeping enemy before native behavior applies its visibility rules.
    if ((obj->oFlags & OBJ_FLAG_HIDE_WHEN_SLEEPING)
        && !(obj->header.gfx.node.flags & GRAPH_RENDER_HAS_ANIMATION)) {
        obj->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    }
    // Restore animation only for an object actually updating, never on a revisit.
    gCurrentObject->header.gfx.node.flags |= GRAPH_RENDER_HAS_ANIMATION;
    if (riderAtStart) {
        object_platform_motion_apply(obj);
    }
    update_object_with_ground_twirl_stun();
    // Mandatory behavior updates do not override the Lab's stable display range.
    if (physics_lab_update_enemy_hidden_state(obj)) {
        obj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
    }
    // A native script may become a rider during this very update.
    if (riderAtStart || obj->behavior != behaviorBeforeUpdate) {
        object_platform_motion_finish(obj);
    }
    *updateStatus = ENEMY_PLATFORM_UPDATE_COMPLETE;
}

/**
 * Update every object that occurs after firstObj in the given object list,
 * including firstObj itself. Return the number of objects that were updated.
 */
s32 update_objects_starting_at(struct ObjectNode *objList, struct ObjectNode *firstObj) {
    s32 count = 0;

    while (objList != firstObj) {
        gCurrentObject = (struct Object *) firstObj;

        update_object_with_platform_motion();

        firstObj = firstObj->next;
        count++;
    }

    return count;
}

/**
 * Update objects in objList starting with firstObj while time stop is active.
 * This means that only certain select objects will be updated, such as Mario,
 * doors, unimportant objects, and the object that initiated time stop.
 * The exact set of objects that are updated depends on which flags are set
 * in gTimeStopState.
 * Return the total number of objects in the list (including those that weren't
 * updated)
 */
s32 update_objects_during_time_stop(struct ObjectNode *objList, struct ObjectNode *firstObj) {
    s32 count = 0;
    s32 unfrozen;

    while (objList != firstObj) {
        gCurrentObject = (struct Object *) firstObj;

        unfrozen = FALSE;

        // Selectively unfreeze certain objects
        if (!(gTimeStopState & TIME_STOP_ALL_OBJECTS)) {
            if (gCurrentObject == gMarioObject && !(gTimeStopState & TIME_STOP_MARIO_AND_DOORS)) {
                unfrozen = TRUE;
            }

            if ((gCurrentObject->oInteractType & (INTERACT_DOOR | INTERACT_WARP_DOOR))
                && !(gTimeStopState & TIME_STOP_MARIO_AND_DOORS)) {
                unfrozen = TRUE;
            }

            if (gCurrentObject->activeFlags
                & (ACTIVE_FLAG_UNIMPORTANT | ACTIVE_FLAG_INITIATED_TIME_STOP)) {
                unfrozen = TRUE;
            }
        }

        // Only update if unfrozen
        if (unfrozen) {
            update_object_with_platform_motion();
        } else {
            gCurrentObject->header.gfx.node.flags &= ~GRAPH_RENDER_HAS_ANIMATION;
        }

        firstObj = firstObj->next;
        count++;
    }

    return count;
}

/**
 * Update every object in the given list. Return the total number of objects in
 * the list.
 */
s32 update_objects_in_list(struct ObjectNode *objList) {
    s32 count;
    struct ObjectNode *firstObj = objList->next;

    if (!(gTimeStopState & TIME_STOP_ACTIVE)) {
        count = update_objects_starting_at(objList, firstObj);
    } else {
        count = update_objects_during_time_stop(objList, firstObj);
    }

    return count;
}

/**
 * Unload any objects in the list that have been deactivated.
 */
s32 unload_deactivated_objects_in_list(struct ObjectNode *objList) {
    struct ObjectNode *obj = objList->next;

    while (objList != obj) {
        gCurrentObject = (struct Object *) obj;

        obj = obj->next;

        if ((gCurrentObject->activeFlags & ACTIVE_FLAG_ACTIVE) != ACTIVE_FLAG_ACTIVE) {
            // Prevent object from respawning after exiting and re-entering the
            // area
            if (!(gCurrentObject->oFlags & OBJ_FLAG_PERSISTENT_RESPAWN)) {
                set_object_respawn_info_bits(gCurrentObject, RESPAWN_INFO_DONT_RESPAWN);
            }

            unload_object(gCurrentObject);
        }
    }

    return 0;
}

/**
 * OR the object's respawn info with bits << 8. If bits = 0xFF, this prevents
 * the object from respawning after leaving and re-entering the area.
 * For macro objects, respawnInfo points to the 16 bit entry in the macro object
 * list. For other objects, it points to the 32 bit behaviorArg in the
 * SpawnInfo.
 */
void set_object_respawn_info_bits(struct Object *obj, u8 bits) {
    u32 *info32;
    u16 *info16;

    switch (obj->respawnInfoType) {
        case RESPAWN_INFO_TYPE_NORMAL:
            info32 = (u32 *) obj->respawnInfo;
            *info32 |= bits << 8;
            break;

        case RESPAWN_INFO_TYPE_MACRO_OBJECT:
            info16 = (u16 *) obj->respawnInfo;
            *info16 |= bits << 8;
            break;
    }
}

/**
 * Unload all objects whose activeAreaIndex is areaIndex.
 */
void unload_objects_from_area(UNUSED s32 unused, s32 areaIndex) {
    struct Object *obj;
    struct ObjectNode *node;
    struct ObjectNode *list;
    s32 i;
    gObjectLists = gObjectListArray;

    for (i = 0; i < NUM_OBJ_LISTS; i++) {
        list = gObjectLists + i;
        node = list->next;

        while (node != list) {
            obj = (struct Object *) node;
            node = node->next;

            if (obj->header.gfx.activeAreaIndex == areaIndex) {
                unload_object(obj);
            }
        }
    }
}

/**
 * Spawn objects given a list of SpawnInfos. Called when loading an area.
 */
void spawn_objects_from_info(UNUSED s32 unused, struct SpawnInfo *spawnInfo) {
    gObjectLists = gObjectListArray;
    gTimeStopState = 0;

    gWDWWaterLevelChanging = FALSE;
    gMarioOnMerryGoRound = FALSE;

    clear_mario_platform();

    if (gCurrAreaIndex == 2) {
        gCCMEnteredSlide |= 1;
    }

    while (spawnInfo != NULL) {
        struct Object *object;
        const BehaviorScript *script;

        script = segmented_to_virtual(spawnInfo->behaviorScript);

        // If the object was previously killed/collected, don't respawn it
        if ((spawnInfo->behaviorArg & (RESPAWN_INFO_DONT_RESPAWN << 8))
            != (RESPAWN_INFO_DONT_RESPAWN << 8)) {
            object = create_object(script);

            // Behavior parameters are often treated as four separate bytes, but
            // are stored as an s32.
            object->oBehParams = spawnInfo->behaviorArg;
            // The second byte of the behavior parameters is copied over to a special field
            // as it is the most frequently used by objects.
            object->oBehParams2ndByte = GET_BPARAM2(spawnInfo->behaviorArg);

            object->behavior = script;
            object->cameraDistanceCulled = TRUE;

            // Record death/collection in the SpawnInfo
            object->respawnInfoType = RESPAWN_INFO_TYPE_NORMAL;
            object->respawnInfo = &spawnInfo->behaviorArg;

            // Usually this checks if bparam4 is 1 to decide if this is mario
            // This change allows any object to use that param
            if (object->behavior == segmented_to_virtual(bhvMario)) {
                gMarioObject = object;
                geo_make_first_child(&object->header.gfx.node);
            }

            geo_obj_init_spawninfo(&object->header.gfx, spawnInfo);

            vec3s_to_vec3f(&object->oPosVec, spawnInfo->startPos);

            vec3s_to_vec3i(&object->oFaceAngleVec, spawnInfo->startAngle);

            vec3s_to_vec3i(&object->oMoveAngleVec, spawnInfo->startAngle);

            object->oFloorHeight = find_floor(object->oPosX, object->oPosY, object->oPosZ, &object->oFloor);
        }

        spawnInfo = spawnInfo->next;
    }
}

/**
 * Clear objects, dynamic surfaces, and some miscellaneous level data used by objects.
 */
void clear_objects(void) {
    s32 i;

    object_platform_motion_init();
    gTHIWaterDrained = 0;
    gTimeStopState = 0;
    gMarioObject = NULL;
    gMarioCurrentRoom = 0;

    bzero(gDoorAdjacentRooms, sizeof(gDoorAdjacentRooms));

    debug_unknown_level_select_check();

    init_free_object_list();
    clear_object_lists(gObjectListArray);

    for (i = 0; i < OBJECT_POOL_CAPACITY; i++) {
        gObjectPool[i].activeFlags = ACTIVE_FLAG_DEACTIVATED;
        geo_reset_object_node(&gObjectPool[i].header.gfx);
    }

    gObjectMemoryPool = mem_pool_init(OBJECT_MEMORY_POOL, MEMORY_POOL_LEFT);
    gObjectLists = gObjectListArray;

    clear_dynamic_surfaces();
}

/**
 * Update spawner and surface objects.
 */
void update_terrain_objects(void) {
    PROFILER_GET_SNAPSHOT_TYPE(PROFILER_DELTA_COLLISION);
    gObjectCounter = update_objects_in_list(&gObjectLists[OBJ_LIST_SPAWNER]);
    profiler_update(PROFILER_TIME_SPAWNER, profiler_get_delta(PROFILER_DELTA_COLLISION) - first);

#ifdef PUPPYPRINT_DEBUG
    first = profiler_get_delta(PROFILER_DELTA_COLLISION);
#endif
    gObjectCounter += update_objects_in_list(&gObjectLists[OBJ_LIST_SURFACE]);
    profiler_update(PROFILER_TIME_DYNAMIC, profiler_get_delta(PROFILER_DELTA_COLLISION) - first);

    // If the dynamic surface pool has overflowed, throw an error.
    assertf((uintptr_t)gDynamicSurfacePoolEnd <= (uintptr_t)gDynamicSurfacePool + DYNAMIC_SURFACE_POOL_SIZE, "Dynamic surface pool size exceeded!");
}

/**
 * Update all other object lists besides spawner and surface objects, using
 * the order specified by sObjectListUpdateOrder.
 */
void update_non_terrain_objects(void) {
    s32 listIndex;

    PROFILER_GET_SNAPSHOT_TYPE(PROFILER_DELTA_COLLISION);
    s32 i = 2;
    while ((listIndex = sObjectListUpdateOrder[i]) != -1) {
        if (listIndex == OBJ_LIST_PLAYER) {
            profiler_update(PROFILER_TIME_BEHAVIOR_BEFORE_MARIO, profiler_get_delta(PROFILER_DELTA_COLLISION) - first);
#ifdef USE_PROFILER
            first = profiler_get_delta(PROFILER_DELTA_COLLISION);
#endif
        }
        gObjectCounter += update_objects_in_list(&gObjectLists[listIndex]);
        if (listIndex == OBJ_LIST_DEFAULT && !(gTimeStopState & TIME_STOP_ACTIVE)) {
            compact_explosion_bubbles_update();
        }
        if (listIndex == OBJ_LIST_PLAYER) {
            profiler_update(PROFILER_TIME_MARIO, profiler_get_delta(PROFILER_DELTA_COLLISION) - first);
#ifdef USE_PROFILER
            first = profiler_get_delta(PROFILER_DELTA_COLLISION);
#endif
        }
        i++;
    }
    profiler_update(PROFILER_TIME_BEHAVIOR_AFTER_MARIO,
                    profiler_get_delta(PROFILER_DELTA_COLLISION) - first);
}

/**
 * Unload deactivated objects in any object list.
 */
void unload_deactivated_objects(void) {
    s32 listIndex;

    s32 i = 0;
    while ((listIndex = sObjectListUpdateOrder[i]) != -1) {
        unload_deactivated_objects_in_list(&gObjectLists[listIndex]);
        i++;
    }

    // TIME_STOP_UNKNOWN_0 was most likely intended to be used to track whether
    // any objects had been deactivated
    gTimeStopState &= ~TIME_STOP_UNKNOWN_0;
}

/**
 * Unused profiling function.
 */
UNUSED static u16 unused_get_elapsed_time(u64 *cycleCounts, s32 index) {
    u16 time;
    f64 cycles;

    cycles = cycleCounts[index] - cycleCounts[index - 1];
    if (cycles < 0) {
        cycles = 0;
    }

    time = (u16)(((u64) cycles * 1000000 / osClockRate) / 16667.0 * 1000.0);
    if (time > 999) {
        time = 999;
    }

    return time;
}

/**
 * Clear all floors tied to dynamic collision, as they become invalid once the dynamic
 * surfaces are cleared.
 * 
 * NOTE: Checking over the full object pool is slower than checking against the linked lists of active
 * objects until the active object pool is around half capacity, after which, this is faster.
 * Change this function to use a linked list instead if you add any additional logic here whatsoever.
 */
void clear_dynamic_surface_references(void) {
    for (s32 i = 0; i < OBJECT_POOL_CAPACITY; i++) {
        if (gObjectPool[i].oFloor && gObjectPool[i].oFloor->flags & SURFACE_FLAG_DYNAMIC) {
            gObjectPool[i].oFloor = NULL;
        }
    }
}

/**
 * Update all objects. This includes script execution, object collision detection,
 * and object surface management.
 */
void update_objects(UNUSED s32 unused) {

    gTimeStopState &= ~TIME_STOP_MARIO_OPENED_DOOR;

    gNumRoomedObjectsInMarioRoom = 0;
    gNumRoomedObjectsNotInMarioRoom = 0;
    gCollisionFlags &= ~COLLISION_FLAG_CAMERA;

    reset_debug_objectinfo();
    stub_debug_control();

    gObjectLists = gObjectListArray;

    PROFILER_GET_SNAPSHOT_TYPE(PROFILER_DELTA_COLLISION);
    object_platform_motion_begin_frame();
    profiler_update(PROFILER_TIME_OBJECT_PLATFORM_SETUP,
                    profiler_get_delta(PROFILER_DELTA_COLLISION) - first);

    // If time stop is not active, unload object surfaces
    clear_dynamic_surfaces();

    // Update spawners and objects with surfaces
    update_terrain_objects();
#ifdef USE_PROFILER
    first = profiler_get_delta(PROFILER_DELTA_COLLISION);
#endif

    // If Mario was touching a moving platform at the end of last frame, apply
    // displacement now
    //! If the platform object unloaded and a different object took its place,
    //  displacement could be applied incorrectly
    apply_mario_platform_displacement();

    object_platform_motion_apply_all();
    profiler_update(PROFILER_TIME_OBJECT_PLATFORM_PREPARE,
                    profiler_get_delta(PROFILER_DELTA_COLLISION) - first);

    // Detect which objects are intersecting
    detect_object_collisions();
    profiler_update(PROFILER_TIME_OBJECT_COLLISION, 0);

    // Update all other objects that haven't been updated yet
    update_non_terrain_objects();
    
    // Take a snapshot of the current collision processing time.
    UNUSED u32 firstPoint = profiler_get_delta(PROFILER_DELTA_COLLISION); 

    // Unload any objects that have been deactivated
    unload_deactivated_objects();

    // Check if Mario is on a platform object and save this object
    update_mario_platform();

    try_print_debug_mario_object_info();

    // If time stop was enabled this frame, activate it now so that it will
    // take effect next frame
    if (gTimeStopState & TIME_STOP_ENABLED) {
        gTimeStopState |= TIME_STOP_ACTIVE;
    } else {
        gTimeStopState &= ~TIME_STOP_ACTIVE;
    }

    gPrevFrameObjectCount = gObjectCounter;
    profiler_update(PROFILER_TIME_OBJECT_CLEANUP,
                    profiler_get_delta(PROFILER_DELTA_COLLISION) - firstPoint);
}
