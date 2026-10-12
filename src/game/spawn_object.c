#include "player_stats.h"
#include <PR/ultratypes.h>

#include "audio/external.h"
#include "compact_explosion_bubbles.h"
#include "engine/geo_layout.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "engine/persistent_surfaces.h"
#include "engine/surface_collision.h"
#include "interaction.h"
#include "level_table.h"
#include "object_constants.h"
#include "object_fields.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "object_platform_motion.h"
#include "particle_budget.h"
#include "spawn_object.h"
#include "types.h"

/**
 * Attempt to allocate an object from freeList (singly linked) and append it
 * to the end of destList (doubly linked). Return the object, or NULL if
 * freeList is empty.
 */
struct Object *try_allocate_object(struct ObjectNode *destList, struct ObjectNode *freeList) {
    struct ObjectNode *nextObj;

    if ((nextObj = freeList->next) != NULL) {
        // Remove from free list
        freeList->next = nextObj->next;

        // Insert at end of destination list
        nextObj->prev = destList->prev;
        nextObj->next = destList;
        destList->prev->next = nextObj;
        destList->prev = nextObj;
    } else {
        return NULL;
    }

    particle_budget_allocated((struct Object *) nextObj);
    object_platform_motion_allocated((struct Object *) nextObj);
    geo_remove_child(&nextObj->gfx.node);
    geo_add_child(&gObjParentGraphNode, &nextObj->gfx.node);

    return (struct Object *) nextObj;
}

/**
 * Remove the given object from the object list that it's currently in, and
 * insert it at the beginning of the free list (singly linked).
 */
static void deallocate_object(struct ObjectNode *freeList, struct ObjectNode *obj) {
    particle_budget_freed((struct Object *) obj);
    persistent_surfaces_forget((struct Object *) obj);
    object_platform_motion_freed((struct Object *) obj);
    // Remove from object list
    obj->next->prev = obj->prev;
    obj->prev->next = obj->next;

    // Insert at beginning of free list
    obj->next = freeList->next;
    freeList->next = obj;
}

/**
 * Add every object in the pool to the free object list.
 */
void init_free_object_list(void) {
    s32 i;
    s32 poolLength = OBJECT_POOL_CAPACITY;

    particle_budget_reset();
    persistent_surfaces_reset();
    compact_explosion_bubbles_reset();

    // Add the first object in the pool to the free list
    struct Object *obj = &gObjectPool[0];
    gFreeObjectList.next = (struct ObjectNode *) obj;

    // Link each object in the pool to the following object
    for (i = 0; i < poolLength - 1; i++) {
        obj->header.next = &(obj + 1)->header;
        obj++;
    }

    // End the list
    obj->header.next = NULL;
}

/**
 * Clear each object list, without adding the objects back to the free list.
 */
void clear_object_lists(struct ObjectNode *objLists) {
    s32 i;

    for (i = 0; i < NUM_OBJ_LISTS; i++) {
        objLists[i].next = &objLists[i];
        objLists[i].prev = &objLists[i];
    }
}

/**
 * Free the given object.
 */
void unload_object(struct Object *obj) {
    mario_forget_attack_object(obj);
    mario_forget_pickup_object(obj);
    object_platform_motion_reset(obj);
    obj->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    obj->prevObj = NULL;
    obj->oFloor = NULL;
    obj->metalContactCooldown = 0;
    obj->metalContactPendingDefeat = FALSE;
    obj->bowserArenaOwner = NULL;
    obj->bowserArenaBooTimer = 0;
    obj->bowserArenaSpinyAge = 0;
    obj->bowserThrowAirFrames = 0;
    obj->bowserArenaCoinRolled = 0;
    obj->bowserArenaFade = 0;
    obj->bowserArenaCollapse = 0;
    obj->bowserArenaFireMask = 0;
    obj->bowserArenaStage = 0;
    obj->bowserArenaFallAllowed = FALSE;
    player_stats_object_reset(obj);

    obj->header.gfx.throwMatrix = NULL;
    stop_sounds_from_source(obj->header.gfx.cameraToObject);
    geo_remove_child(&obj->header.gfx.node);
    geo_add_child(&gObjParentGraphNode, &obj->header.gfx.node);

    obj->header.gfx.node.flags &= ~(GRAPH_RENDER_BILLBOARD | GRAPH_RENDER_ACTIVE);

    deallocate_object(&gFreeObjectList, &obj->header);
}

/**
 * Initialize both required objects and optional particles through the same path.
 */
static void initialize_object(struct Object *obj) {
    object_platform_motion_reset(obj);
    s32 i;
    // Initialize object fields

    obj->activeFlags = ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_ALLOCATED;
    obj->parentObj = obj;
    obj->prevObj = NULL;
    obj->collidedObjInteractTypes = 0;
    obj->numCollidedObjs = 0;

#if IS_64_BIT
    for (i = 0; i < MAX_OBJECT_FIELDS; i++) {
        obj->rawData.asS32[i] = 0;
        obj->ptrData.asVoidPtr[i] = NULL;
    }
#else
    for (i = 0; i < MAX_OBJECT_FIELDS; i++) {
        obj->rawData.asS32[i] = 0;
    }
#endif

    obj->cameraDistanceCulled = TRUE;
    obj->bhvStackIndex = 0;
    obj->bhvDelayTimer = 0;

    obj->hitboxRadius = 50.0f;
    obj->hitboxHeight = 100.0f;
    obj->hurtboxRadius = 0.0f;
    obj->hurtboxHeight = 0.0f;
    obj->hitboxDownOffset = 0.0f;
    obj->metalContactCooldown = 0;
    obj->metalContactPendingDefeat = FALSE;
    obj->bowserArenaOwner = NULL;
    obj->bowserArenaBooTimer = 0;
    obj->bowserArenaSpinyAge = 0;
    obj->bowserThrowAirFrames = 0;
    obj->bowserArenaCoinRolled = 0;
    obj->bowserArenaFade = 0;
    obj->bowserArenaCollapse = 0;
    obj->bowserArenaFireMask = 0;
    obj->bowserArenaStage = 0;
    obj->bowserArenaFallAllowed = FALSE;
    player_stats_object_reset(obj);

    obj->platform = NULL;
    obj->collisionData = NULL;
    obj->oIntangibleTimer = -1;
    obj->oDamageOrCoinValue = 0;
    obj->oHealth = 2048;

    obj->oCollisionDistance = 1000.0f;
    obj->oDrawingDistance = 4000.0f;

    mtxf_identity(obj->transform);

    obj->respawnInfoType = RESPAWN_INFO_TYPE_NULL;
    obj->respawnInfo = NULL;

    obj->oDistanceToMario = 19000.0f;
    obj->oRoom = -1;

    obj->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
    vec3_same(obj->header.gfx.pos, -10000.0f);
    obj->header.gfx.throwMatrix = NULL;
}

static struct Object *allocate_object_with_policy(struct ObjectNode *objList, struct Object *parent,
                                                  enum ParticlePriority priority) {
    struct Object *obj;
    s32 isOptionalSpawn = priority != PARTICLE_PRIORITY_REQUIRED;
    if ((isOptionalSpawn && particle_budget_free_slots() <= PARTICLE_SLOT_RESERVE)
        || gFreeObjectList.next == NULL) {
        struct Object *victim = particle_budget_select(priority, parent);
        if (victim == NULL) {
            if (isOptionalSpawn) {
                return NULL;
            }
            // Retain the required-spawn contract when only protected objects remain.
            while (TRUE) {
            }
        }
        unload_object(victim);
    }
    obj = try_allocate_object(objList, &gFreeObjectList);
    initialize_object(obj);
    return obj;
}

/**
 * Spawn an object at the origin with the behavior script at virtual address bhvScript.
 */
static struct Object *create_object_with_policy(const BehaviorScript *bhvScript, struct Object *parent,
                                                enum ParticlePriority priority) {
    s32 objListIndex;
    struct Object *obj;
    struct ObjectNode *objList;

    // If the first behavior script command is "begin <object list>", then
    // extract the object list from it
    if ((bhvScript[0] >> 24) == 0) {
        objListIndex = (bhvScript[0] >> 16) & 0xFFFF;
    } else {
        objListIndex = OBJ_LIST_DEFAULT;
    }

    objList = &gObjectLists[objListIndex];
    obj = allocate_object_with_policy(objList, parent, priority);
    if (obj == NULL) {
        return NULL;
    }

    obj->curBhvCommand = bhvScript;
    obj->behavior = bhvScript;

    if (objListIndex == OBJ_LIST_UNIMPORTANT) {
        obj->activeFlags |= ACTIVE_FLAG_UNIMPORTANT;
    }

    particle_budget_init_priority(obj, priority);
    return obj;
}

struct Object *create_object(const BehaviorScript *bhvScript) {
    return create_object_with_policy(bhvScript, NULL, PARTICLE_PRIORITY_REQUIRED);
}

struct Object *create_object_for_parent(const BehaviorScript *bhvScript, struct Object *parent) {
    return create_object_with_policy(bhvScript, parent, PARTICLE_PRIORITY_REQUIRED);
}

struct Object *try_create_particle(const BehaviorScript *bhvScript, struct Object *parent,
                                   enum ParticlePriority priority) {
    if (priority <= PARTICLE_PRIORITY_PROTECTED || priority >= PARTICLE_PRIORITY_REQUIRED) {
        return NULL;
    }
    return create_object_with_policy(bhvScript, parent, priority);
}
