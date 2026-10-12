// water_objs.inc.c
// HACKERSM64_DO: Better name, please

void bhv_water_air_bubble_init(void) {
    cur_obj_scale(4.0f);
}

// Native scaling reads these fields as binary-angle phases and increments.
// BubbleMaybe/SmallWaterWave scripts also write float offsets into the same slots;
// preserve their shared storage and initialization order when changing either side.

// Ordinary bubbles heal through interact_water_ring; chest bubbles already granted the benefit.
void bhv_water_air_bubble_loop(void) {
    s32 i;
    if (o->oBehParams2ndByte == TREASURE_CHEST_BUBBLE_VISUAL_ONLY) {
        o->oInteractType = INTERACT_NONE; // The chest already gave Mario this bubble's healing.
    }
    o->header.gfx.scale[0] = sins(o->oWaterObjScaleXAngle) * 0.5f + 4.0f;
    o->header.gfx.scale[1] = -sins(o->oWaterObjScaleXAngle) * 0.5f + 4.0f;
    o->oWaterObjScaleXAngle += 0x400;

    if (o->oTimer < 30) {
        cur_obj_become_intangible();
        o->oPosY += 3.0f;
    } else {
        cur_obj_become_tangible();
        cur_obj_forward_vel_approach_upward(2.0f, 10.0f);
        o->oMoveAngleYaw = obj_angle_to_object(o, gMarioObject);
        cur_obj_move_using_fvel_and_gravity();
    }

    o->oPosX += random_float() * 4.0f - 2.0f;
    o->oPosZ += random_float() * 4.0f - 2.0f;

    if (o->oInteractStatus & INT_STATUS_INTERACTED || o->oTimer > 200) {
        cur_obj_play_sound_2(SOUND_GENERAL_QUIET_BUBBLE);
        obj_mark_for_deletion(o);
        for (i = 0; i < 30; i++) {
            spawn_object(o, MODEL_BUBBLE, bhvBubbleMaybe);
        }
    }

    if (find_water_level(o->oPosX, o->oPosZ) < o->oPosY) {
        obj_mark_for_deletion(o);
    }

    o->oInteractStatus = INT_STATUS_NONE;
}

void bhv_spawn_mario_breath_bubble(void) {
    s32 isVanishBreathBubble =
        (gMarioState->flags & MARIO_VANISH_CAP) && mario_is_head_underwater(gMarioState);

    // The spawner waits several updates. Place Vanish bubbles and their sound
    // at Mario's current position, not where he was when the wait began.
    if (isVanishBreathBubble) {
        obj_copy_pos(o, gMarioObject);
    }
    struct Object *bubble =
        try_spawn_particle(o, MODEL_BUBBLE, bhvSmallWaterWave, PARTICLE_PRIORITY_FEEDBACK);
    if (bubble != NULL) {
        // Retain the birth classification even if Mario's cap or immersion changes later.
        bubble->oBehParams2ndByte = isVanishBreathBubble;
    }
}

void bhv_bubble_wave_init(void) {
    o->oWaterObjScaleXAngleVel = 0x800 + (s32)(random_float() * 2048.0f);
    o->oWaterObjScaleYAngleVel = 0x800 + (s32)(random_float() * 2048.0f);
    cur_obj_play_sound_2(SOUND_GENERAL_QUIET_BUBBLE);
}

void scale_bubble_random(void) {
    cur_obj_scale(random_float() + 1.0f);
}

void bhv_bubble_maybe_loop(void) {
    o->oPosY += random_float() * 3.0f + 6.0f;
    o->oPosX += random_float() * 10.0f - 5.0f;
    o->oPosZ += random_float() * 10.0f - 5.0f;

    o->header.gfx.scale[0] = sins(o->oWaterObjScaleXAngle) * 0.2f + 1.0f;
    o->oWaterObjScaleXAngle += o->oWaterObjScaleXAngleVel;
    o->header.gfx.scale[1] = sins(o->oWaterObjScaleYAngle) * 0.2f + 1.0f;
    o->oWaterObjScaleYAngle += o->oWaterObjScaleYAngleVel;
}

void bhv_small_water_wave_loop(void) {
    o->header.gfx.scale[0] = sins(o->oWaterObjScaleXAngle) * 0.2f + 1.0f;
    o->oWaterObjScaleXAngle += o->oWaterObjScaleXAngleVel;
    o->header.gfx.scale[1] = sins(o->oWaterObjScaleYAngle) * 0.2f + 1.0f;
    o->oWaterObjScaleYAngle += o->oWaterObjScaleYAngleVel;

    if (o->oPosY > find_water_level(o->oPosX, o->oPosZ)) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        o->oPosY += 5.0f;
        // Both the birth flag and Mario's current Vanish/head-underwater state suppress splashes.
        if (o->oBehParams2ndByte == 0
            && !((gMarioState->flags & MARIO_VANISH_CAP) && mario_is_head_underwater(gMarioState))
            && gFreeObjectList.next != NULL) {
            spawn_object(o, MODEL_SMALL_WATER_SPLASH, bhvObjectWaterSplash);
        }
    }

    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        obj_mark_for_deletion(o);
    }
}

void scale_bubble_sin(void) {
    o->header.gfx.scale[0] = sins(o->oWaterObjScaleXAngle) * 0.5f + 2.0f;
    o->oWaterObjScaleXAngle += o->oWaterObjScaleXAngleVel;
    o->header.gfx.scale[1] = sins(o->oWaterObjScaleYAngle) * 0.5f + 2.0f;
    o->oWaterObjScaleYAngle += o->oWaterObjScaleYAngleVel;
}

void bhv_particle_init(void) {
    obj_scale_xyz(o, 2.0f, 2.0f, 1.0f);
    o->oWaterObjScaleXAngleVel = 0x800 + (s32)(random_float() * 2048.0f);
    o->oWaterObjScaleYAngleVel = 0x800 + (s32)(random_float() * 2048.0f);
    obj_translate_xyz_random(o, 100.0f);
}

void bhv_particle_loop() {
    o->oPosY += 5.0f;
    obj_translate_xz_random(o, 4.0f);
    scale_bubble_sin();
    if (o->oTimer != 0 && o->oPosY > find_water_level(o->oPosX, o->oPosZ)) {
        obj_mark_for_deletion(o);
        try_to_spawn_object(5, 0, o, MODEL_SMALL_WATER_SPLASH, bhvObjectWaterSplash);
    }
}

void bhv_small_bubbles_loop(void) {
    o->oPosY += 5.0f;
    obj_translate_xz_random(o, 4.0f);
    scale_bubble_sin();
}

void bhv_fish_group_loop(void) {
    if ((gMarioCurrentRoom == 15 || gMarioCurrentRoom == 7) && (gGlobalTimer & 1)) {
        try_spawn_particle(o, MODEL_WHITE_PARTICLE_SMALL, bhvSmallParticleBubbles,
                           PARTICLE_PRIORITY_AMBIENT);
    }
}

void bhv_water_waves_init(void) {
    s32 i;
    s32 requested = o->oBehParams2ndByte == 2 ? 2 : 3;
    s32 count = particle_budget_burst_count(PARTICLE_PRIORITY_AMBIENT, requested);
    for (i = 0; i < count; i++) {
        if (try_spawn_particle(o, MODEL_WHITE_PARTICLE_SMALL, bhvSmallParticle,
                               PARTICLE_PRIORITY_AMBIENT)
            == NULL) {
            break;
        }
    }
}
