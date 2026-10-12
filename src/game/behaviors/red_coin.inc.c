
/**
 * This file contains the initialization and behavior for red coins.
 * Behavior controls audio and the orange number spawned, as well as interacting with
 * the course's red coin star.
 */

/**
 * Red coin's hitbox details.
 */
static struct ObjectHitbox sRedCoinHitbox = {
    /* interactType:      */ INTERACT_COIN,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 2,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 100,
    /* height:            */ 64,
    /* hurtboxRadius:     */ 0,
    /* hurtboxHeight:     */ 0,
};

/**
 * Red coin initialization function. Sets the coin's hitbox and parent object.
 */
void bhv_red_coin_init(void) {
    // Prefer the nearest ordinary spawner; search Bowser spawners only if none was found.
    struct Object *redCoinSpawner = cur_obj_nearest_object_with_behavior(bhvHiddenRedCoinStar);
    if (redCoinSpawner != NULL) {
        o->parentObj = redCoinSpawner;
    } else if ((redCoinSpawner = cur_obj_nearest_object_with_behavior(bhvBowserCourseRedCoinStar))
               != NULL) {
        o->parentObj = redCoinSpawner;
    } else {
        o->parentObj = NULL;
    }

    obj_set_hitbox(o, &sRedCoinHitbox);
}

/**
 * Main behavior for red coins. Primarily controls coin collection noise and spawning
 * the orange number counter.
 */
void bhv_red_coin_loop(void) {
    // Interaction code already awarded the coin; this loop updates feedback and cleanup.
    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        if (o->parentObj != NULL) {
            // The parent is the trigger spawner, which owns progress toward the red-coin star.
            o->parentObj->oHiddenStarTriggerCounter++;

            // Spawn the orange number counter, as long as it isn't the last coin.
            if (o->parentObj->oHiddenStarTriggerCounter != o->parentObj->oHiddenStarTriggerTotal) {
                // Cap only the visible count to 99; the spawner's real counter is unchanged.
                if (o->parentObj->oHiddenStarTriggerCounter > 99) {
                    spawn_orange_number(9, 28, 0, 0);
                    spawn_orange_number(9, -28, 0, 0);
                }
                else if (o->parentObj->oHiddenStarTriggerCounter >= 10) {
                    spawn_orange_number(o->parentObj->oHiddenStarTriggerCounter % 10, 28, 0, 0);
                    spawn_orange_number(o->parentObj->oHiddenStarTriggerCounter / 10, -28, 0, 0);
                }
                else {
                    spawn_orange_number(o->parentObj->oHiddenStarTriggerCounter, 0, 0, 0);
                }
            }

#ifdef JP_RED_COIN_SOUND
            // For JP version, play an identical sound for all coins.
            create_sound_spawner(SOUND_GENERAL_RED_COIN);
#else
            if (o->parentObj->oHiddenStarTriggerTotal - o->parentObj->oHiddenStarTriggerCounter > 7) {
                // Play the first red coin sound until it gets to the final 8
                play_sound(SOUND_MENU_COLLECT_RED_COIN, gGlobalSoundSource);
            }
            else {
                // Select successive sound variants from the number still needed (the final eight).
                play_sound(SOUND_MENU_COLLECT_RED_COIN
                        + (((u8) 7 - (o->parentObj->oHiddenStarTriggerTotal - o->parentObj->oHiddenStarTriggerCounter)) << 16),
                        gGlobalSoundSource);
            }
#endif
        }

        // Standalone coins also get pickup sparkles and deactivation from this shared cleanup.
        coin_collected();
        o->oInteractStatus = INT_STATUS_NONE;
    }
}
