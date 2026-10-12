// water_splashes_and_waves.inc.c

// Water droplets from Mario jumping in a pool of water.
struct WaterDropletParams sWaterSplashDropletParams = {
    /* Flags */ WATER_DROPLET_FLAG_RAND_ANGLE,
    /* Model */ MODEL_WHITE_PARTICLE_SMALL,
    /* Behavior */ bhvWaterDroplet,
    /* Unused (flag-specific) */ 0, 0,
    /* Random fvel offset, scale */ 5.0f, 3.0f,
    /* Random yvel offset, scale */ 30.0f, 20.0f,
    /* Random size offset, scale */ 0.5f, 1.0f
};

// Water droplets from Mario jumping in shallow water.
struct WaterDropletParams gShallowWaterSplashDropletParams = {
    /* Flags */ WATER_DROPLET_FLAG_RAND_ANGLE | WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL,
    /* Model */ MODEL_WHITE_PARTICLE_SMALL,
    /* Behavior */ bhvWaterDroplet,
    /* Unused (flag-specific) */ 0, 0,
    /* Random fvel offset, scale */ 2.0f, 3.0f,
    /* Random yvel offset, scale */ 20.0f, 20.0f,
    /* Random size offset, scale */ 0.5f, 1.0f
};

// The fish particle easter egg from Mario jumping in shallow water.
struct WaterDropletParams sWaterDropletFishParams = {
    /* Flags */ WATER_DROPLET_FLAG_RAND_ANGLE | WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL,
    /* Model */ MODEL_FISH,
    /* Behavior */ bhvWaterDroplet,
    /* Unused (flag-specific) */ 0, 0,
    /* Random fvel offset, scale */ 2.0f, 3.0f,
    /* Random yvel offset, scale */ 20.0f, 20.0f,
    /* Random size offset, scale */ 1.0f, 0.0f
};

// Water droplets from Mario running in shallow water.
struct WaterDropletParams gShallowWaterWaveDropletParams = {
    /* Flags */ WATER_DROPLET_FLAG_RAND_ANGLE_INCR_BACKWARD | WATER_DROPLET_FLAG_RAND_ANGLE | WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL,
    /* Model */ MODEL_WHITE_PARTICLE_SMALL,
    /* Behavior */ bhvWaterDroplet,
    /* Move angle range */ 0x6000,
    /* Unused (flag-specific) */ 0,
    /* Random fvel offset, scale */ 2.0f, 8.0f,
    /* Random yvel offset, scale */ 10.0f, 10.0f,
    /* Random size offset, scale */ 0.5f, 1.0f
};

static void spawn_shallow_water_droplets(struct WaterDropletParams *params, s32 count) {
    // The whole burst starts at the parent's X/Z. Each droplet still checks
    // its own moving position after spawning.
    s32 waterLevel = find_water_level(o->oPosX, o->oPosZ);
    count = particle_budget_burst_count(PARTICLE_PRIORITY_DROPLET, count);
    for (s32 i = 0; i < count; i++) {
        if (spawn_water_droplet_at_water_level(o, params, waterLevel) == NULL) {
            break;
        }
    }
}

void bhv_shallow_water_wave_spawn_droplets(void) {
    spawn_shallow_water_droplets(&gShallowWaterWaveDropletParams, 5);
}

void bhv_shallow_water_splash_spawn_droplets(void) {
    spawn_shallow_water_droplets(&gShallowWaterSplashDropletParams, 18);
}

void spawn_slide_kick_liquid_particles(struct MarioState *m, f32 contactHeight,
                                       enum LiquidParticleMaterial material) {
    struct WaterDropletParams params = gShallowWaterSplashDropletParams;
    // The contact is already resolved; lava must never query a water level here.
    params.flags &= ~WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL;
    if (material == LIQUID_PARTICLE_LAVA) {
        params.model = MODEL_LAVA_DROPLET;
        params.randSizeOffset *= LIQUID_PARTICLE_LAVA_DROPLET_SCALE;
        params.randSizeScale *= LIQUID_PARTICLE_LAVA_DROPLET_SCALE;
    } else if (material == LIQUID_PARTICLE_COLD_WATER) {
        params.model = MODEL_COLD_WATER_DROPLET;
    }

    s32 count = particle_budget_burst_count(PARTICLE_PRIORITY_DROPLET, SLIDE_KICK_LIQUID_DROPLET_COUNT);
    for (s32 i = 0; i < count; i++) {
        struct Object *drop = spawn_water_droplet(m->marioObj, &params);
        if (drop == NULL) {
            break;
        }
        drop->oPosX = m->pos[0];
        drop->oPosY = contactHeight;
        drop->oPosZ = m->pos[2];
        drop->oAction = WATER_DROPLET_ACT_SLIDE_KICK;
        drop->oBehParams2ndByte = material;
    }
}

static void slide_kick_liquid_droplet_land(f32 stepStartY) {
    // The lava floor query uses height before this update's gravity/movement.
    f32 contactHeight;
    if (o->oBehParams2ndByte == LIQUID_PARTICLE_LAVA) {
        struct Surface *floor = NULL;
        contactHeight = find_floor(o->oPosX, stepStartY + 50.0f, o->oPosZ, &floor);
        if (floor == NULL) {
            return;
        }
    } else {
        contactHeight = find_water_level(o->oPosX, o->oPosZ);
        if (contactHeight < FLOOR_LOWER_LIMIT_MISC) {
            obj_mark_for_deletion(o);
            return;
        }
    }
    if (o->oVelY < 0.0f && o->oPosY <= contactHeight) {
        struct Object *splash =
            try_to_spawn_object(0, 1.0f, o, MODEL_SMALL_WATER_SPLASH, bhvWaterDropletSplash);
        if (splash != NULL) {
            splash->oPosY = contactHeight;
            splash->oAction = WATER_DROPLET_ACT_SLIDE_KICK;
            splash->oBehParams2ndByte = o->oBehParams2ndByte;
        }
        obj_mark_for_deletion(o);
    }
}

void bhv_water_splash_spawn_droplets(void) {
    s32 i;
    // Nine droplets over five updates: two, two, one, two, two.
    if (o->oTimer >= 5) {
        return;
    }
    if (o->oTimer == 0) {
        o->oPosY = find_water_level(o->oPosX, o->oPosZ);
    }

    if (o->oPosY > FLOOR_LOWER_LIMIT_MISC) { // Make sure it is not at the default water level
        s32 requested = o->oTimer == 2 ? 1 : 2;
        s32 count = particle_budget_burst_count(PARTICLE_PRIORITY_DROPLET, requested);
        for (i = 0; i < count; i++) {
            if (spawn_water_droplet(o, &sWaterSplashDropletParams) == NULL) {
                break;
            }
        }
    }
}

void bhv_water_droplet_loop(void) {
    if (o->oTimer == 0) {
        if (cur_obj_has_model(MODEL_FISH)) {
            o->header.gfx.node.flags &= ~GRAPH_RENDER_BILLBOARD;
        } else {
            o->header.gfx.node.flags |= GRAPH_RENDER_BILLBOARD;
        }
        o->oFaceAngleYaw = random_u16();
    }
    if (o->oAction == WATER_DROPLET_ACT_SLIDE_KICK) {
        if (o->oTimer > 20) {
            obj_mark_for_deletion(o);
            return;
        }
        f32 stepStartY = o->oPosY;
        o->oVelY -= 4.0f;
        o->oPosY += o->oVelY;
        if (o->oVelY < 0.0f || (o->oTimer == 0 && o->oBehParams2ndByte != LIQUID_PARTICLE_LAVA)) {
            slide_kick_liquid_droplet_land(stepStartY);
        }
        return;
    }
    if (o->oAction == WATER_DROPLET_ACT_SOAKED) {
        struct Surface *floor = NULL;
        struct Object *mario = o->parentObj;

        if (mario == NULL) {
            obj_mark_for_deletion(o);
            return;
        }
        o->oVelY -= 4.0f;
        o->oPosX += (mario->oPosX - o->oHomeX) * 0.8f;
        o->oPosY += o->oVelY;
        o->oPosZ += (mario->oPosZ - o->oHomeZ) * 0.8f;
        o->oHomeX = mario->oPosX;
        o->oHomeZ = mario->oPosZ;
        f32 floorHeight = find_floor(o->oPosX, o->oPosY + 50.0f, o->oPosZ, &floor);
        if (floor != NULL && o->oPosY <= floorHeight) {
            struct Object *splash =
                try_to_spawn_object(0, 1.0f, o, MODEL_SMALL_WATER_SPLASH, bhvWaterDropletSplash);
            if (splash != NULL) {
                splash->oPosY = floorHeight;
                splash->oAction = WATER_DROPLET_ACT_SOAKED; // Soaked ground splash only.
            }
            obj_mark_for_deletion(o);
        } else if (o->oTimer > 20) {
            obj_mark_for_deletion(o);
        }
        return;
    }
    // Apply gravity to ordinary, world-space water droplets.
    o->oVelY -= 4.0f;
    o->oPosY += o->oVelY;
    // A rising droplet cannot meet the water surface. Validate its water on
    // the first update, then query again only when it begins falling.
    if (o->oTimer == 0 || o->oVelY < 0.0f) {
        f32 waterLevel = find_water_level(o->oPosX, o->oPosZ);
        if (waterLevel < FLOOR_LOWER_LIMIT_MISC) {
            obj_mark_for_deletion(o);
            return;
        }
        // Check if fallen back into the water.
        if (o->oVelY < 0.0f) {
            if (waterLevel > o->oPosY) {
                // Create the smaller splash
                try_to_spawn_object(0, 1.0f, o, MODEL_SMALL_WATER_SPLASH, bhvWaterDropletSplash);
                obj_mark_for_deletion(o);
            } else if (o->oTimer > 20) {
                obj_mark_for_deletion(o);
            }
        }
    }
}

void bhv_idle_water_wave_loop(void) {
    obj_copy_pos(o, gMarioObject);
    o->oPosY = gMarioStates[0].waterLevel + 5;
    // The requested Mario flag controls lifetime; the parent flag prevents duplicate spawns.
    if (!(gMarioObject->oMarioParticleFlags & ACTIVE_PARTICLE_IDLE_WATER_WAVE)) {
        particle_budget_clear_parent_flags(o, ACTIVE_PARTICLE_IDLE_WATER_WAVE);
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}

void bhv_water_droplet_splash_init(void) {
    if (o->oAction == WATER_DROPLET_ACT_SOAKED) {
        cur_obj_scale(0.9f);
        o->oOpacity = 216;
    } else {
        f32 scale = random_float() + 1.5f;
        if (o->oAction == WATER_DROPLET_ACT_SLIDE_KICK
            && o->oBehParams2ndByte == LIQUID_PARTICLE_LAVA) {
            scale *= LIQUID_PARTICLE_LAVA_SPLASH_SCALE;
        }
        cur_obj_scale(scale);
        o->oOpacity = 255;
    }
}

void bhv_water_droplet_splash_fade(void) {
    if (o->oAction == WATER_DROPLET_ACT_SOAKED) {
        o->oOpacity = MAX(o->oOpacity - 30, 0);
    } else if (o->oTimer >= 5) {
        // Ordinary splashes still last six updates despite the longer dry fade.
        obj_mark_for_deletion(o);
    }
}

void bhv_bubble_splash_init(void) {
    f32 waterLevel = find_water_level(o->oPosX, o->oPosZ);
    obj_scale_xyz(o, 0.5f, 1.0f, 0.5f);
    o->oPosY = waterLevel + 5.0f;
}

void bhv_shallow_water_splash_init(void) {
    // Have a 1 in 256 chance to spawn the fish particle easter egg.
    if ((random_u16() & 0xFF) <= 0 && !game_options_secrets_blocked()) {
        struct Object *fishObj = spawn_water_droplet(o, &sWaterDropletFishParams);
        if (fishObj != NULL) {
            obj_init_animation_with_sound(fishObj, blue_fish_seg3_anims_0301C2B0, 0);
        }
    }
}

void bhv_wave_trail_shrink(void) {
    f32 waterLevel;
    f32 shrinkPerUpdate = 0.0f;

    waterLevel = find_water_level(o->oPosX, o->oPosZ);
    if (o->oTimer == 0) {
        o->oSubAction = 0;
        // Cache the birth-depth classification before moving the trail to the water surface.
        // A submerged trail starts broad and tapers toward nothing.
        if (o->parentObj == gMarioObject && o->oPosY < waterLevel - 130.0f) {
            cur_obj_scale(1.15f);
            o->oSubAction = 1;
        }
        // Submerged trails are already spaced by distance; only surface trails use frame gating.
        s32 skipSurfaceTrail =
            o->parentObj == gMarioObject ? gGlobalTimer % 3 != 0 : (gGlobalTimer & 1);
        if (o->oSubAction == 0 && skipSurfaceTrail) {
            obj_mark_for_deletion(o);
            return;
        }
        o->oWaveTrailSize = o->header.gfx.scale[0];
    }
    o->oPosY = waterLevel + 5.0f;

    if (o->oSubAction == 1) {
        if (o->oTimer > 0) {
            shrinkPerUpdate = 0.075f;
        }
    } else if (o->oAnimState > 3) {
        shrinkPerUpdate = 0.1f;
    }
    // Shrinking affects X/Z only; the behavior script owns animation and final deletion.
    if (shrinkPerUpdate > 0.0f) {
        o->oWaveTrailSize -= shrinkPerUpdate;
        if (o->oWaveTrailSize < 0.0f) {
            o->oWaveTrailSize = 0.0f;
        }
        o->header.gfx.scale[0] = o->oWaveTrailSize;
        o->header.gfx.scale[2] = o->oWaveTrailSize;
    }
}
