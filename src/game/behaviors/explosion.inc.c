// explosion.inc.c

void bhv_explosion_init(void) {
    s32 punchBlast = o->oBehParams2ndByte == HE_THICC_PUNCH_EXPLOSION && o->parentObj == gMarioObject;
    // Mario-triggered Bowser mines already applied their contact damage.
    if (punchBlast
        || (o->oBehParams2ndByte == 1 && o->parentObj != NULL
            && obj_has_behavior(o->parentObj, bhvBowserBomb))) {
        o->oInteractType = 0;
        cur_obj_become_intangible();
    }
    create_sound_spawner(SOUND_GENERAL2_BOBOMB_EXPLOSION);
    // The punch already owns its tuned HE THICC impact shake.
    if (!punchBlast) {
        set_environmental_camera_shake(SHAKE_ENV_EXPLOSION);
    }

    o->oOpacity = 255;
}

void bhv_explosion_loop(void) {
    s32 i;
    s32 count;

    if (o->oTimer == 9) {
        if (find_water_level(o->oPosX, o->oPosZ) > o->oPosY) {
            count = particle_budget_burst_count(PARTICLE_PRIORITY_EXPLOSION_BUBBLE, 40);
            for (i = 0; i < count; i++) {
                if (!compact_explosion_bubble_spawn(o)
                    && try_spawn_particle(o, MODEL_WHITE_PARTICLE_SMALL, bhvBobombExplosionBubble,
                                          PARTICLE_PRIORITY_EXPLOSION_BUBBLE)
                           == NULL) {
                    break;
                }
            }
        } else {
            try_spawn_particle(o, MODEL_SMOKE, bhvBobombBullyDeathSmoke, PARTICLE_PRIORITY_MIST);
        }

        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }

    o->oOpacity -= 14;

    cur_obj_scale((f32) o->oTimer / 9.0f + 1.0f);
}

void bhv_bobomb_bully_death_smoke_init(void) {
    o->oPosY -= 300.0f;
    cur_obj_scale(10.0f);
}

void bhv_bobomb_explosion_bubble_init(void) {
    obj_scale_xyz(o, 2.0f, 2.0f, 1.0f);

    o->oBobombExpBubGfxExpRateX = (s32)(random_float() * 2048.0f) + 0x800;
    o->oBobombExpBubGfxExpRateY = (s32)(random_float() * 2048.0f) + 0x800;
    o->oTimer = random_float() * 10.0f;
    o->oVelY = (s32)(random_float() * 4.0f) + 4;
}

void bhv_bobomb_explosion_bubble_loop(void) {
    f32 waterY = gMarioState->waterLevel;

    o->header.gfx.scale[0] = sins(o->oBobombExpBubGfxScaleFacX) * 0.5f + 2.0f;
    o->oBobombExpBubGfxScaleFacX += o->oBobombExpBubGfxExpRateX;

    o->header.gfx.scale[1] = sins(o->oBobombExpBubGfxScaleFacY) * 0.5f + 2.0f;
    o->oBobombExpBubGfxScaleFacY += o->oBobombExpBubGfxExpRateY;

    if (o->oPosY > waterY) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        o->oPosY += 5.0f;
        try_spawn_particle(o, MODEL_SMALL_WATER_SPLASH, bhvObjectWaterSplash,
                           PARTICLE_PRIORITY_DROPLET);
    }

    if (o->oTimer > 60) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
    o->oPosY += o->oVelY;
    o->oTimer++;
}
