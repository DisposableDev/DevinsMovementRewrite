// clam.inc.c

struct ObjectHitbox sClamShellHitbox = {
    /* interactType:      */ INTERACT_CLAM_OR_BUBBA,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 2,
    /* health:            */ 99,
    /* numLootCoins:      */ 0,
    /* radius:            */ 150,
    /* height:            */ 80,
    /* hurtboxRadius:     */ 150,
    /* hurtboxHeight:     */ 80,
};

void clam_act_0(void) {
    s32 closingCue = cur_obj_init_anim_check_frame(CLAM_ANIM_CLOSING, 25);

    // Derive contact from the pose every update, including skipped frames/reloads.
    if (o->header.gfx.animInfo.animFrame >= CLAM_CLOSING_DAMAGE_FRAME) {
        cur_obj_become_tangible();
    } else {
        cur_obj_become_intangible();
    }

    if (closingCue) {
        cur_obj_play_sound_2(SOUND_GENERAL_CLAM_SHELL_CLOSE);
        spawn_mist_from_global();

        o->oClamShakeTimer = 10;
        o->oTimer = 0;
    } else if (o->oTimer > 150 && o->oDistanceToMario < 500.0f) {
        cur_obj_play_sound_2(SOUND_GENERAL_CLAM_SHELL_OPEN);
        o->oAction = CLAM_ACT_OPENING;
    } else if (o->oClamShakeTimer != 0) {
        o->oClamShakeTimer--;
        cur_obj_shake_y(3.0f);
    }
}

void clam_act_1(void) {
    s16 i;
    s16 bubblesX, bubblesZ;
    s32 openingCue = cur_obj_init_anim_check_frame(CLAM_ANIM_OPENING, 8);

    if (o->header.gfx.animInfo.animFrame >= CLAM_OPENING_SAFE_FRAME) {
        cur_obj_become_intangible();
    } else {
        cur_obj_become_tangible();
    }

    if (o->oTimer > 150) {
        o->oAction = CLAM_ACT_CLOSING;
    } else if (obj_is_rendering_enabled() && openingCue) {
        for (i = -0x2000; i < 0x2000; i += 0x555) {
            bubblesX = (s16)(100.0f * sins(i));
            bubblesZ = (s16)(100.0f * coss(i));

            spawn_object_relative(0, bubblesX, 30, bubblesZ, o, MODEL_BUBBLE, bhvBubbleMaybe);
        }
    }
}

void bhv_clam_loop(void) {
    o->header.gfx.scale[1] = 1.5f;
    // First-time hitbox setup makes the object tangible; do it before pose gating.
    if (!(o->oFlags & OBJ_FLAG_HITBOX_WAS_SET)) {
        obj_set_hitbox(o, &sClamShellHitbox);
    }

    switch (o->oAction) {
        case CLAM_ACT_CLOSING:
            clam_act_0();
            break;
        case CLAM_ACT_OPENING:
            clam_act_1();
            break;
    }

    obj_check_attacks(&sClamShellHitbox, o->oAction);
}
