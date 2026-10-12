#include <PR/ultratypes.h>

#include "sm64.h"
#include "debug.h"
#include "interaction.h"
#include "mario.h"
#include "object_list_processor.h"
#include "spawn_object.h"
#include "engine/math_util.h"

UNUSED struct Object *debug_print_obj_collision(struct Object *a) {
    struct Object *currCollidedObj;
    s32 i;

    for (i = 0; i < a->numCollidedObjs; i++) {
        print_debug_top_down_objectinfo("ON", 0);
        currCollidedObj = a->collidedObjs[i];
        if (currCollidedObj != gMarioObject) {
            return currCollidedObj;
        }
    }
    return NULL;
}

// Accepted overlap appends contacts to both objects and accumulates interaction masks.
// A full four-contact list rejects the pair, so FALSE need not mean geometric separation.
// Horizontal tangency is excluded; touching vertical extents count as overlap.
s32 detect_object_hitbox_overlap(struct Object *a, struct Object *b) {
    f32 dya_bottom = a->oPosY - a->hitboxDownOffset;
    f32 dyb_bottom = b->oPosY - b->hitboxDownOffset;
    f32 dx = a->oPosX - b->oPosX;
    f32 dz = a->oPosZ - b->oPosZ;
    f32 collisionRadius = a->hitboxRadius + b->hitboxRadius;
    f32 distance = sqr(dx) + sqr(dz);

    // Extend Mario's dialog reach without enlarging the objects' physical hitboxes.
    struct Object *dialogObj = NULL;
    if (a == gMarioObject) {
        dialogObj = b;
    } else if (b == gMarioObject) {
        dialogObj = a;
    }
    if (dialogObj != NULL && (dialogObj->oInteractType & INTERACT_TEXT)) {
        if (dialogObj->oInteractionSubtype & INT_SUBTYPE_SIGN) {
            collisionRadius *= 1.25f;
        } else if (dialogObj->oInteractionSubtype & INT_SUBTYPE_NPC) {
            collisionRadius *= 2.0f;
        }
    }

    if (sqr(collisionRadius) > distance) {
        f32 dya_top = a->hitboxHeight + dya_bottom;
        f32 dyb_top = b->hitboxHeight + dyb_bottom;

        if (dya_bottom > dyb_top
            || dya_top < dyb_bottom
            || a->numCollidedObjs >= 4
            || b->numCollidedObjs >= 4) {
            return FALSE;
        }
        a->collidedObjs[a->numCollidedObjs] = b;
        b->collidedObjs[b->numCollidedObjs] = a;
        a->collidedObjInteractTypes |= b->oInteractType;
        b->collidedObjInteractTypes |= a->oInteractType;
        a->numCollidedObjs++;
        b->numCollidedObjs++;
        return TRUE;
    }

    return FALSE;
}

// For Mario in slot a, set b's delay-invincibility flag before testing, even on a miss;
// an overlap clears it. The first object uses hitbox height, the second hurtbox height.
s32 detect_object_hurtbox_overlap(struct Object *a, struct Object *b) {
    f32 dya_bottom = a->oPosY - a->hitboxDownOffset;
    f32 dyb_bottom = b->oPosY - b->hitboxDownOffset;
    f32 dx = a->oPosX - b->oPosX;
    f32 dz = a->oPosZ - b->oPosZ;
    f32 collisionRadius = a->hurtboxRadius + b->hurtboxRadius;
    f32 distance = sqr(dx) + sqr(dz);

    if (a == gMarioObject) {
        b->oInteractionSubtype |= INT_SUBTYPE_DELAY_INVINCIBILITY;
    }

    if (sqr(collisionRadius) > distance) {
        f32 dya_top = a->hitboxHeight  + dya_bottom;
        f32 dyb_top = b->hurtboxHeight + dyb_bottom;

        if (dya_bottom > dyb_top || dya_top < dyb_bottom) {
            return FALSE;
        }
        if (a == gMarioObject) {
            b->oInteractionSubtype &= ~INT_SUBTYPE_DELAY_INVINCIBILITY;
        }
        return TRUE;
    }

    return FALSE;
}

// Reset circular list members, not its sentinel. Contact pointers remain; the count defines validity.
// Positive intangibility timers decrement unless the object is dormant.
void clear_object_collision(struct Object *listHead) {
    struct Object *nextObj = (struct Object *) listHead->header.next;

    while (nextObj != listHead) {
        nextObj->numCollidedObjs = 0;
        nextObj->collidedObjInteractTypes = 0;
        if (nextObj->oIntangibleTimer > 0 && !bbh_merry_go_round_object_is_dormant(nextObj)) {
            nextObj->oIntangibleTimer--;
        }
        nextObj = (struct Object *) nextObj->header.next;
    }
}

// Visit candidates up to the exclusive list end, preserving contact insertion order.
void check_collision_in_list(struct Object *obj, struct Object *candidate, struct Object *listEnd) {
    if (obj->oIntangibleTimer == 0 && !bbh_merry_go_round_object_is_dormant(obj)) {
        while (candidate != listEnd) {
            if (candidate->oIntangibleTimer == 0 && !bbh_merry_go_round_object_is_dormant(candidate)) {
                if (detect_object_hitbox_overlap(obj, candidate) && candidate->hurtboxRadius != 0.0f) {
                    detect_object_hurtbox_overlap(obj, candidate);
                }
            }
            candidate = (struct Object *) candidate->header.next;
        }
    }
}

// Same-list scans start after the source object, avoiding self-pairs and duplicate pairs.
void check_player_object_collision(void) {
    struct Object *playerListHead = (struct Object *) &gObjectLists[OBJ_LIST_PLAYER];
    struct Object *nextObj = (struct Object *) playerListHead->header.next;

    while (nextObj != playerListHead) {
        check_collision_in_list(nextObj, (struct Object *) nextObj->header.next, playerListHead);
        check_collision_in_list(nextObj, (struct Object *) gObjectLists[OBJ_LIST_POLELIKE].next,
                                (struct Object *) &gObjectLists[OBJ_LIST_POLELIKE]);
        check_collision_in_list(nextObj, (struct Object *) gObjectLists[OBJ_LIST_LEVEL].next,
                                (struct Object *) &gObjectLists[OBJ_LIST_LEVEL]);
        check_collision_in_list(nextObj, (struct Object *) gObjectLists[OBJ_LIST_GENACTOR].next,
                                (struct Object *) &gObjectLists[OBJ_LIST_GENACTOR]);
        check_collision_in_list(nextObj, (struct Object *) gObjectLists[OBJ_LIST_PUSHABLE].next,
                                (struct Object *) &gObjectLists[OBJ_LIST_PUSHABLE]);
        check_collision_in_list(nextObj, (struct Object *) gObjectLists[OBJ_LIST_SURFACE].next,
                                (struct Object *) &gObjectLists[OBJ_LIST_SURFACE]);
        check_collision_in_list(nextObj, (struct Object *) gObjectLists[OBJ_LIST_DESTRUCTIVE].next,
                                (struct Object *) &gObjectLists[OBJ_LIST_DESTRUCTIVE]);
        nextObj = (struct Object *) nextObj->header.next;
    }
}

void check_pushable_object_collision(void) {
    struct Object *pushableListHead = (struct Object *) &gObjectLists[OBJ_LIST_PUSHABLE];
    struct Object *nextObj = (struct Object *) pushableListHead->header.next;

    while (nextObj != pushableListHead) {
        check_collision_in_list(nextObj, (struct Object *) nextObj->header.next, pushableListHead);
        nextObj = (struct Object *) nextObj->header.next;
    }
}

// The distance/flag filter gates this object as a destructive scan source.
// Other scans may still record contact with it.
void check_destructive_object_collision(void) {
    struct Object *destructiveListHead = (struct Object *) &gObjectLists[OBJ_LIST_DESTRUCTIVE];
    struct Object *nextObj = (struct Object *) destructiveListHead->header.next;

    while (nextObj != destructiveListHead) {
        if (nextObj->oDistanceToMario < 2000.0f
            && !(nextObj->activeFlags & ACTIVE_FLAG_DESTRUCTIVE_OBJ_DONT_DESTROY)) {
            check_collision_in_list(nextObj, (struct Object *) nextObj->header.next,
                                    destructiveListHead);
            check_collision_in_list(nextObj, (struct Object *) gObjectLists[OBJ_LIST_GENACTOR].next,
                                    (struct Object *) &gObjectLists[OBJ_LIST_GENACTOR]);
            check_collision_in_list(nextObj, (struct Object *) gObjectLists[OBJ_LIST_PUSHABLE].next,
                                    (struct Object *) &gObjectLists[OBJ_LIST_PUSHABLE]);
            check_collision_in_list(nextObj, (struct Object *) gObjectLists[OBJ_LIST_SURFACE].next,
                                    (struct Object *) &gObjectLists[OBJ_LIST_SURFACE]);
        }
        nextObj = (struct Object *) nextObj->header.next;
    }
}

void detect_object_collisions(void) {
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_POLELIKE]);
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_PLAYER]);
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_PUSHABLE]);
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_GENACTOR]);
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_LEVEL]);
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_SURFACE]);
    clear_object_collision((struct Object *) &gObjectLists[OBJ_LIST_DESTRUCTIVE]);
    // Four-entry contact lists can fill; pass order affects which later pairs are recorded.
    check_player_object_collision();
    check_destructive_object_collision();
    check_pushable_object_collision();
}
