// bouncing_fireball.inc.c

static void bouncing_fireball_trail_loop(void) {
    if (o->oTimer == 0) {
        if (o->oAction == 0) {
            // Keep the native random draw so subsequent hazards spawn on time.
            o->oAnimState = random_float() * 10.0f;
            o->oVelY = 30.0f;
        } else {
            o->oVelY = 50.0f;
            o->oForwardVel = 30.0f;
        }
    }

    if (!(o->activeFlags & (ACTIVE_FLAG_FAR_AWAY | ACTIVE_FLAG_IN_DIFFERENT_ROOM))) {
        o->oPosX += o->oForwardVel * sins(o->oMoveAngleYaw);
        o->oPosZ += o->oForwardVel * coss(o->oMoveAngleYaw);
        o->oVelY += o->oGravity;
        o->oPosY += o->oVelY;
        // Intangible trails bounce against their emission plane, without queries.
        if (o->oPosY < o->oBouncingFireballTrailFloorY) {
            o->oPosY = o->oBouncingFireballTrailFloorY;
            if (o->oAction == 0) {
                o->oAction = 1;
            } else {
                o->oVelY *= o->oBounciness;
            }
        }
    }

    if (o->oTimer > 80) {
        cur_obj_scale(o->header.gfx.scale[0] * 0.9f);
    }
    if (o->oTimer > 100) {
        obj_mark_for_deletion(o);
    }
}

void bhv_bouncing_fireball_flame_loop(void) {
    o->activeFlags |= ACTIVE_FLAG_IGNORE_ENV_BOXES;
    if (o->oBouncingFireballIsTrail) {
        bouncing_fireball_trail_loop();
        o->oInteractStatus = INT_STATUS_NONE;
        return;
    }
    cur_obj_update_floor_and_walls();

    switch (o->oAction) {
        case 0:
            if (o->oTimer == 0) {
                o->oAnimState = random_float() * 10.0f;
                o->oVelY = 30.0f;
            }

            if (o->oMoveFlags & OBJ_MOVE_LANDED) {
                o->oAction++;
            }
            break;

        case 1:
            if (o->oTimer == 0) {
                o->oVelY = 50.0f;
                o->oForwardVel = 30.0f;
            }

            if (o->oMoveFlags
                & (OBJ_MOVE_ON_GROUND | OBJ_MOVE_AT_WATER_SURFACE | OBJ_MOVE_UNDERWATER_ON_GROUND)
                && o->oTimer > 100) {
                obj_mark_for_deletion(o);
            }
            break;
    }

    if (o->oTimer > 300) {
        obj_mark_for_deletion(o);
    }

    cur_obj_move_standard(78);

    o->oInteractStatus = INT_STATUS_NONE;
}

void bhv_bouncing_fireball_spawner_loop(void) {
    struct Object *flameObj;
    f32 scale;

    switch (o->oAction) {
        case 0:
            if (o->oDistanceToMario < 2000.0f) {
                o->oAction = 1;
            }
            break;

        case 1:
            flameObj = spawn_object(o, MODEL_RED_FLAME, bhvBouncingFireballFlame);
            scale = (10 - o->oTimer) * 0.5f;

            obj_scale(flameObj, scale);
            flameObj->oBouncingFireballIsTrail = (o->oTimer != 0);
            flameObj->oBouncingFireballTrailFloorY = flameObj->oPosY;
            if (o->oTimer == 0) {
                obj_become_tangible(flameObj);
            }

            if (o->oTimer > 10) {
                o->oAction++;
            }
            break;

        case 2:
            if (o->oTimer == 0) {
                o->oBouncingFireBallSpawnerRandomCooldown = random_float() * 100.0f;
            }
            if (o->oBouncingFireBallSpawnerRandomCooldown + 100 < o->oTimer) {
                o->oAction = 0;
            }
            break;
    }
}
