// cap.inc.c

// Cosmetic cap release: no pickup interaction or horizontal movement.
void bhv_released_power_up_cap_loop(void) {
    if (o->oTimer >= 30) {
        obj_mark_for_deletion(o);
        return;
    }

    o->oPosY += o->oVelY;
    o->oVelY -= 1.0f;
    o->oFaceAngleYaw += 0x800;
    if (o->oTimer >= 18) {
        s32 opacity = (o->oBehParams & MARIO_VANISH_CAP) ? 150 : 255;
        o->oOpacity = opacity * (30 - o->oTimer) / 12;
    }
}

static struct ObjectHitbox sCapHitbox = {
    /* interactType:      */ INTERACT_CAP,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 0,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 80,
    /* height:            */ 80,
    /* hurtboxRadius:     */ 90,
    /* hurtboxHeight:     */ 90,
};

s32 cap_set_hitbox(void) {
    obj_set_hitbox(o, &sCapHitbox);

    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        o->oInteractStatus = INT_STATUS_NONE;
        return TRUE;
    }

    return FALSE;
}

void cap_despawn(void) {
    if (o->oTimer > 300) {
        obj_flicker_and_disappear(o, 300);
    }
}

void cap_check_quicksand(void) {
    if (sObjFloor == NULL) {
        return;
    }

    switch (sObjFloor->type) {
        case SURFACE_DEATH_PLANE:
            o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
            break;

        case SURFACE_SHALLOW_QUICKSAND:
        case SURFACE_DEEP_QUICKSAND:
        case SURFACE_QUICKSAND:
            o->oAction = CAP_ACT_QUICKSAND;
            o->oForwardVel = 0.0f;
            break;

        case SURFACE_DEEP_MOVING_QUICKSAND:
        case SURFACE_SHALLOW_MOVING_QUICKSAND:
        case SURFACE_MOVING_QUICKSAND:
            o->oAction = CAP_ACT_MOVING_QUICKSAND;
            o->oMoveAngleYaw = (sObjFloor->force & 0xFF) << 8;
            o->oForwardVel = 8 + 2 * (0 - ((sObjFloor->force & 0xFF00) >> 8));
            break;

        case SURFACE_INSTANT_QUICKSAND:
            o->oAction = CAP_ACT_INSTANT_QUICKSAND;
            o->oForwardVel = 0.0f;
            break;

        case SURFACE_INSTANT_MOVING_QUICKSAND:
            o->oAction = CAP_ACT_INSTANT_MOVING_QUICKSAND;
            o->oMoveAngleYaw = (sObjFloor->force & 0xFF) << 8;
            o->oForwardVel = 8 + 2 * (0 - ((sObjFloor->force & 0xFF00) >> 8));
            break;
    }
}

void cap_sink_quicksand(void) {
    switch (o->oAction) {
        case CAP_ACT_QUICKSAND:
            if (o->oTimer < 10) {
                o->oGraphYOffset += -1.0f;
                o->oFaceAnglePitch = 0x2000;
            }
            break;

        case CAP_ACT_MOVING_QUICKSAND:
            if (o->oTimer < 10) {
                o->oGraphYOffset += -3.0f;
            }
            o->oFaceAnglePitch = 0x2000;
            break;

        case CAP_ACT_INSTANT_QUICKSAND:
            o->oGraphYOffset += -1.0f;
            if (o->oTimer > 20) {
                o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
            }
            break;

        case CAP_ACT_INSTANT_MOVING_QUICKSAND:
            o->oGraphYOffset += -6.0f;
            if (o->oTimer > 20) {
                o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
            }
            o->oFaceAnglePitch = 0x2000;
            break;
    }

    cap_check_quicksand();
}

void bhv_wing_cap_init(void) {
    o->oGravity = 1.2f;
    o->oFriction = 0.999f;
    o->oBuoyancy = 0.9f;
    o->oOpacity = 255;
}

void cap_scale_vertically(void) {
    o->oCapScaleAngle += 0x2000;
    o->header.gfx.scale[1] = coss(o->oCapScaleAngle) * 0.3f + 0.7f;
    if (o->oCapScaleAngle == 0x10000) {
        o->oCapScaleAngle = 0;
        o->oCapDoScaleVertically = FALSE;
    }
}

void wing_vanish_cap_act_0(void) {
    o->oFaceAngleYaw += o->oForwardVel * 128.0f;
    s16 collisionFlags = object_step();
    if (collisionFlags & OBJ_COL_FLAG_GROUNDED) {
        cap_check_quicksand();
        if (o->oVelY != 0.0f) {
            o->oCapDoScaleVertically = TRUE;
            o->oVelY = 0.0f;
        }
    }

    if (o->oCapDoScaleVertically) {
        cap_scale_vertically();
    }
}

void bhv_wing_vanish_cap_loop(void) {
    switch (o->oAction) {
        case CAP_ACT_MOVE:
            wing_vanish_cap_act_0();
            break;

        default:
            object_step();
            cap_sink_quicksand();
            break;
    }

    if (o->oTimer > 20) {
        cur_obj_become_tangible();
    }

    cap_despawn();
    cap_set_hitbox();
}

void bhv_metal_cap_init(void) {
    o->oGravity = 2.4f;
    o->oFriction = 0.999f;
    o->oBuoyancy = 1.5f;
    o->oOpacity = 255;
}

void metal_cap_act_0(void) {
    o->oFaceAngleYaw += (o->oForwardVel * 128.0f);

    s16 collisionFlags = object_step();

    if (collisionFlags & OBJ_COL_FLAG_GROUNDED) {
        cap_check_quicksand();
    }
}

void bhv_metal_cap_loop(void) {
    switch (o->oAction) {
        case CAP_ACT_MOVE:
            metal_cap_act_0();
            break;

        default:
            object_step();
            cap_sink_quicksand();
            break;
    }

    if (o->oTimer > 20) {
        cur_obj_become_tangible();
    }

    cap_set_hitbox();
    cap_despawn();
}

void bhv_normal_cap_init(void) {
    o->oGravity = 0.7f;
    o->oFriction = 0.89f;
    o->oBuoyancy = 0.9f;
    o->oOpacity = 255;

    save_file_set_cap_pos(o->oPosX, o->oPosY, o->oPosZ);
}

void normal_cap_set_save_flags(void) {
    save_file_clear_flags(SAVE_FLAG_CAP_ON_GROUND);

    switch (gCurrCourseNum) {
        case COURSE_SSL:
            save_file_set_flags(SAVE_FLAG_CAP_ON_KLEPTO);
            break;

        case COURSE_SL:
            save_file_set_flags(SAVE_FLAG_CAP_ON_MR_BLIZZARD);
            break;

        case COURSE_TTM:
            save_file_set_flags(SAVE_FLAG_CAP_ON_UKIKI);
            break;

        default:
            save_file_set_flags(SAVE_FLAG_CAP_ON_KLEPTO);
            break;
    }
}

static s32 normal_cap_return_to_bowser_arena(void) {
    if (gCurrLevelNum != LEVEL_BOWSER_1 && gCurrLevelNum != LEVEL_BOWSER_2
        && gCurrLevelNum != LEVEL_BOWSER_3) {
        return FALSE;
    }
    s32 fightOne = gCurrLevelNum == LEVEL_BOWSER_1;
    s32 fightThree = gCurrLevelNum == LEVEL_BOWSER_3;
    if (sObjFloor != NULL && sObjFloor->type != SURFACE_BURNING
        && sObjFloor->type != SURFACE_DEATH_PLANE) {
        if (fightThree) {
            struct Object *support = sObjFloor->object;
            if (support != NULL
                && support->behavior == segmented_to_virtual(bhvFallingBowserPlatform)) {
                if (!support->bowserArenaCollapse && support->oAction != BOWSER_BITS_PLAT_ACT_FALL) {
                    return FALSE;
                }
            } else if (ABS(o->oPosX) < 2500 && ABS(o->oPosZ) < 2600 && sObjFloor->normal.y > 0.5f
                       && sObjFloor->type != SURFACE_MARIO_ONLY_GRIPPY) {
                return FALSE;
            }
        } else if (fightOne) {
            // Fight 1's main arena is static.
            if (sObjFloor->object == NULL && sObjFloor->normal.y > 0.5f) {
                return FALSE;
            }
        } else if (sObjFloor->object != NULL
                   && sObjFloor->object->behavior
                          == segmented_to_virtual(bhvTiltingBowserLavaPlatform)) {
            return FALSE;
        }
    }
    if (sObjFloor == NULL && o->oPosY >= 0) {
        return FALSE;
    }
    struct Surface *floor;
    f32 height = find_floor(800.0f, 10000.0f, 0.0f, &floor);
    if (floor == NULL || floor->type == SURFACE_BURNING || floor->type == SURFACE_DEATH_PLANE
        || (fightOne && (floor->object != NULL || floor->normal.y <= 0.5f))
        || (gCurrLevelNum == LEVEL_BOWSER_2
            && (floor->object == NULL
                || floor->object->behavior != segmented_to_virtual(bhvTiltingBowserLavaPlatform)))) {
        return FALSE;
    }
    o->oPosX = 800.0f;
    o->oPosY = height;
    o->oPosZ = 0.0f;
    o->oForwardVel = 0.0f;
    o->oVelX = 0.0f;
    o->oVelY = 0.0f;
    o->oVelZ = 0.0f;
    o->oFaceAnglePitch = 0;
    o->oFloor = floor;
    o->oFloorHeight = height;
    sObjFloor = floor;
    save_file_set_cap_pos(o->oPosX, o->oPosY, o->oPosZ);
    return TRUE;
}

void normal_cap_act_0(void) {
    o->oFaceAngleYaw += o->oForwardVel * 128.0f;
    o->oFaceAnglePitch += o->oForwardVel * 80.0f;
    s16 collisionFlags = object_step();

    if (((collisionFlags & OBJ_COL_FLAG_GROUNDED) || o->oPosY < 0)
        && normal_cap_return_to_bowser_arena()) {
        return;
    }
    if (collisionFlags & OBJ_COL_FLAG_GROUNDED) {
        cap_check_quicksand();

        if (o->oVelY != 0.0f) {
            o->oCapDoScaleVertically = TRUE;
            o->oVelY = 0.0f;
            o->oFaceAnglePitch = 0;
        }
    }

    if (o->oCapDoScaleVertically) {
        cap_scale_vertically();
    }
}

void bhv_normal_cap_loop(void) {
    // A power-up pickup may have removed this lost cap earlier in the same frame.
    if (o->activeFlags == ACTIVE_FLAG_DEACTIVATED) {
        return;
    }

    // Resolve collection before movement can sink the cap and save it as lost again.
    if (cap_set_hitbox()) {
        save_file_clear_flags(SAVE_FLAG_CAP_ON_GROUND);
        return;
    }

    switch (o->oAction) {
        case CAP_ACT_MOVE:
            normal_cap_act_0();
            break;

        default:
            object_step();
            cap_sink_quicksand();
            break;
    }

    if ((s32) o->oForwardVel != 0) {
        save_file_set_cap_pos(o->oPosX, o->oPosY, o->oPosZ);
    }

    if (o->activeFlags == ACTIVE_FLAG_DEACTIVATED) {
        normal_cap_set_save_flags();
    }

}

void bhv_vanish_cap_init(void) {
    o->oGravity = 1.2f;
    o->oFriction = 0.999f;
    o->oBuoyancy = 0.9f;
    o->oOpacity = 150;
}
