// fire_spitter.inc.c

static void fire_spitter_act_idle(void) {
    approach_f32_ptr(&o->header.gfx.scale[0], 0.2f, 0.002f);

    if (o->oTimer > 150 && o->oDistanceToMario < 800.0f && !(o->oMoveFlags & OBJ_MOVE_MASK_IN_WATER)) {
        o->oAction = FIRE_SPITTER_ACT_SPIT_FIRE;
        o->oFireSpitterScaleVel = 0.05f;
    }
}

static void fire_spitter_act_spit_fire(void) {
    s32 scaleStatus;

    o->oMoveAngleYaw = o->oAngleToMario;

    // Increase scale by 0.05, 0.04, ..., -0.03. Then wait ~8 frames, then
    // starting moving scale by 0.05 each frame toward 0.1. The first time
    // it becomes below 0.15 during this latter portion, shoot fire.
    scaleStatus = obj_grow_then_shrink(&o->oFireSpitterScaleVel, 0.15f, 0.1f);

    if (scaleStatus != 0) {
        if (scaleStatus < 0) {
            o->oAction = FIRE_SPITTER_ACT_IDLE;
        } else {
            cur_obj_play_sound_2(SOUND_OBJ_FLAME_BLOWN);
            obj_spit_fire(0, 0, 0, 5.0f, MODEL_RED_FLAME_SHADOW, 20.0f, 15.0f, 0x1000);
        }
    }
}

#if FIRE_SPITTER_SLEEP_TRIAL
static s32 fire_spitter_physics_can_sleep(void) {
    if (gMarioObject == NULL || o->oAction != FIRE_SPITTER_ACT_IDLE || o->oPrevAction != o->oAction
        || o->oTimer <= 150 || o->oInteractStatus != INT_STATUS_NONE || o->numCollidedObjs != 0
        || o->oForwardVel != 0.0f || o->oVelX != 0.0f || o->oVelY != 0.0f || o->oVelZ != 0.0f
        || o->oGravity != 0.0f || o->oBuoyancy != 0.0f || o->oFloor == NULL
        || (o->oFloor->flags & SURFACE_FLAG_DYNAMIC)
        || (o->oMoveFlags
            & (OBJ_MOVE_MASK_IN_WATER | OBJ_MOVE_ABOVE_LAVA | OBJ_MOVE_ABOVE_DEATH_BARRIER))
        || o->oPosY < o->oFloorHeight) {
        return FALSE;
    }
    // Native Fire Spitters hover above the floor with zero gravity.
    f32 dx = o->oPosX - gMarioObject->oPosX;
    f32 dz = o->oPosZ - gMarioObject->oPosZ;
    if (dx * dx + dz * dz <= 2500.0f * 2500.0f && absf(o->oPosY - gMarioObject->oPosY) <= 1000.0f) {
        return FALSE;
    }
    // Rising water must resume native submersion handling before another attack.
    return find_water_level(o->oPosX, o->oPosZ) < o->oPosY;
}
#endif

void bhv_fire_spitter_update(void) {
    s32 sleepPhysics = FALSE;
#if FIRE_SPITTER_SLEEP_TRIAL
    sleepPhysics = fire_spitter_physics_can_sleep();
#endif
    cur_obj_scale(o->header.gfx.scale[0]);
    o->oGraphYOffset = 40.0f;
    if (!sleepPhysics) {
        cur_obj_update_floor_and_walls();
    }

    switch (o->oAction) {
        case FIRE_SPITTER_ACT_IDLE:
            fire_spitter_act_idle();
            break;
        case FIRE_SPITTER_ACT_SPIT_FIRE:
            fire_spitter_act_spit_fire();
            break;
    }

    if (!sleepPhysics) {
        cur_obj_move_standard(78);
    }
}
