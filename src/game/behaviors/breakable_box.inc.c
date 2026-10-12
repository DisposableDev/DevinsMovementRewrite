// breakable_box.inc.c

struct ObjectHitbox sBreakableBoxHitbox = {
    /* interactType:      */ INTERACT_BREAKABLE,
    /* downOffset:        */  20,
    /* damageOrCoinValue: */   0,
    /* health:            */   1,
    /* numLootCoins:      */   0,
    /* radius:            */ 150,
    /* height:            */ 200,
    /* hurtboxRadius:     */ 150,
    /* hurtboxHeight:     */ 200,
};

void breakable_box_init(void) {
    o->oHiddenObjectSwitchObj = NULL;
    o->oAnimState = BREAKABLE_BOX_ANIM_STATE_CORK_BOX;
    switch (o->oBehParams2ndByte) {
        case BREAKABLE_BOX_BP_NO_COINS: o->oNumLootCoins = 0; break;
        case BREAKABLE_BOX_BP_3_COINS:  o->oNumLootCoins = 3; break;
        case BREAKABLE_BOX_BP_5_COINS:  o->oNumLootCoins = 5; break;
        case BREAKABLE_BOX_BP_LARGE:    cur_obj_scale(1.5f);  break;
    }
}

static s32 hidden_box_switch_is_valid(struct Object *switchObj) {
    return switchObj != NULL && switchObj->activeFlags != ACTIVE_FLAG_DEACTIVATED
           && switchObj->oBehParams2ndByte == PURPLE_SWITCH_BP_REVEAL_HIDDEN
           && (obj_has_behavior(switchObj, bhvFloorSwitchHiddenObjects)
               || obj_has_behavior(switchObj, bhvPurpleSwitchHiddenBoxes)
               || obj_has_behavior(switchObj, bhvFloorSwitchHardcodedModel));
}

static struct Object *find_hidden_box_switch(void) {
    struct ObjectNode *list = &gObjectLists[OBJ_LIST_SURFACE];
    struct Object *candidate = NULL;
    f32 nearest = 0x20000;

    for (struct Object *switchObj = (struct Object *) list->next; switchObj != (struct Object *) list;
         switchObj = (struct Object *) switchObj->header.next) {
        if (hidden_box_switch_is_valid(switchObj)) {
            f32 distance = dist_between_objects(o, switchObj);
            if (distance < nearest) {
                nearest = distance;
                candidate = switchObj;
            }
        }
    }
    return candidate;
}

static s32 hidden_box_new_switch_press(void) {
    struct Object *switchObj = o->oHiddenObjectSwitchObj;
    if (!hidden_box_switch_is_valid(switchObj)) {
        o->oHiddenObjectSwitchObj = find_hidden_box_switch();
        switchObj = o->oHiddenObjectSwitchObj;
        o->oHiddenObjectLastSwitchPress = 0;
    }
    if (switchObj != NULL
        && o->oHiddenObjectLastSwitchPress != switchObj->oPurpleSwitchActivationSerial) {
        o->oHiddenObjectLastSwitchPress = switchObj->oPurpleSwitchActivationSerial;
        return switchObj->oPurpleSwitchEffectTimer > 0;
    }
    return FALSE;
}

static s32 hidden_box_update_lifetime(void) {
    s32 warningAge =
        o->oTimer - (PURPLE_SWITCH_HIDDEN_ACTIVE_FRAMES - PURPLE_SWITCH_HIDDEN_BLINK_FRAMES);

    if (o->oTimer >= PURPLE_SWITCH_HIDDEN_ACTIVE_FRAMES) {
        o->oAction = BREAKABLE_BOX_ACT_HIDDEN;
        cur_obj_disable_rendering();
        cur_obj_become_intangible();
        return TRUE;
    }

    // Keep collision during all 90 warning updates, including dim ones.
    obj_set_blink_phase(o, warningAge >= 0 && (warningAge & 1));
    return FALSE;
}

void hidden_breakable_box_actions(void) {
    obj_set_hitbox(o, &sBreakableBoxHitbox);
    cur_obj_set_model(MODEL_BREAKABLE_BOX);
    if (!o->oHiddenObjectInitialized) {
        breakable_box_init();
        o->oHiddenObjectInitialized = TRUE;
    }
    if (hidden_box_new_switch_press()) {
        o->oAction = BREAKABLE_BOX_ACT_ACTIVE;
        o->oTimer = 0;
        o->oInteractStatus = INT_STATUS_NONE;
        cur_obj_enable_rendering();
        cur_obj_unhide();
        cur_obj_become_tangible();
    }
    switch (o->oAction) {
        case BREAKABLE_BOX_ACT_HIDDEN:
            cur_obj_disable_rendering();
            cur_obj_become_intangible();
            break;
        case BREAKABLE_BOX_ACT_ACTIVE:
            if (hidden_box_update_lifetime()) {
                break;
            }
            cur_obj_become_tangible();
            if (cur_obj_was_attacked_or_ground_pounded()) {
                spawn_mist_particles();
                spawn_triangle_break_particles(30, MODEL_DIRT_ANIMATION, 3.0f, TINY_DIRT_PARTICLE_ANIM_STATE_YELLOW);
                o->oAction = BREAKABLE_BOX_ACT_BROKEN;
                cur_obj_play_sound_2(SOUND_GENERAL_BREAK_BOX);
                cur_obj_become_intangible();
                cur_obj_disable_rendering();
                break;
            }
            load_object_collision_model();
            break;
        case BREAKABLE_BOX_ACT_BROKEN:
            cur_obj_become_intangible();
            cur_obj_disable_rendering();
            o->oInteractStatus = INT_STATUS_NONE;
            break;
    }
}

void hidden_unbreakable_box_actions(void) {
    obj_set_collision_data(o, wdw_seg7_collision_hidden_platform);
    if (hidden_box_new_switch_press()) {
        o->oAction = BREAKABLE_BOX_ACT_ACTIVE;
        o->oTimer = 0;
        cur_obj_enable_rendering();
        cur_obj_unhide();
    }
    switch (o->oAction) {
        case BREAKABLE_BOX_ACT_HIDDEN:
            cur_obj_disable_rendering();
            cur_obj_become_intangible();
            break;
        case BREAKABLE_BOX_ACT_ACTIVE:
            if (hidden_box_update_lifetime()) {
                break;
            }
            cur_obj_become_tangible();
            load_object_collision_model();
            break;
    }
}

void bhv_hidden_object_loop(void) {
    if (o->oBehParams2ndByte == BREAKABLE_BOX_BP_NO_COINS) {
        hidden_breakable_box_actions();
    } else {
        hidden_unbreakable_box_actions();
    }
}

void bhv_breakable_box_loop(void) {
    obj_set_hitbox(o, &sBreakableBoxHitbox);
    cur_obj_set_model(MODEL_BREAKABLE_BOX);
    if (o->oTimer == 0) breakable_box_init();
    if (cur_obj_was_attacked_or_ground_pounded()) {
        obj_explode_and_spawn_coins(46.0f, COIN_TYPE_YELLOW);
        create_sound_spawner(SOUND_GENERAL_BREAK_BOX);
    }
}
