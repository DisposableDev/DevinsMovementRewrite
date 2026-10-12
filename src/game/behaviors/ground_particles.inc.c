// ground_particles.inc.c

static struct SpawnParticlesInfo sGlobalMistParticles = {
    /* behParam:        */ WHITE_PUFF_EXPLODE_BP_SLOW_FADE,
    /* count:           */ 20,
    /* model:           */ MODEL_MIST,
    /* offsetY:         */ 20,
    /* forwardVelBase:  */ 10,
    /* forwardVelRange: */ 5,
    /* velYBase:        */ 0,
    /* velYRange:       */ 0,
    /* gravity:         */ 0,
    /* dragStrength:    */ 30,
    /* sizeBase:        */ 30.0f,
    /* sizeRange:       */ 1.5f,
};

void bhv_pound_white_puffs_init(void) {
    cur_obj_clear_particle_parent_flags(ACTIVE_PARTICLE_MIST_CIRCLE);
    s32 count = particle_budget_burst_count(PARTICLE_PRIORITY_MIST, 8);
    for (s32 i = 0; i < count; i++) {
        s16 yaw = (i * 65536) / count;
        struct Object *particle =
            try_spawn_particle(o, MODEL_MIST, bhvWhitePuffExplosion, PARTICLE_PRIORITY_MIST);
        if (particle == NULL) {
            break;
        }
        particle->oBehParams2ndByte = WHITE_PUFF_EXPLODE_BP_GROUND_POUND_IMPACT;
        particle->oMoveAngleYaw = yaw;
        particle->oPosX += 35.0f * sins(yaw);
        particle->oPosZ += 35.0f * coss(yaw);
        particle->oPosY += 20.0f;
        particle->oForwardVel = 10.0f;
        particle->oVelY = 0.0f;
        particle->oGravity = 0.0f;
        particle->oDragStrength = 30.0f;
        obj_scale(particle, 3.0f);
    }
}

// The impulse requests this effect even if Mario changes action before particles update.
void bhv_water_down_mist_init(void) {
    struct Object *particle;
    s32 i;
    s16 yaw;
    f32 downwardVel;
    s32 vanishBurst = o->oBehParams2ndByte == 1;
    s32 puffCount = vanishBurst ? 8 : 6;

    cur_obj_clear_particle_parent_flags(vanishBurst ? ACTIVE_PARTICLE_VANISH_WATER_RING
                                                    : ACTIVE_PARTICLE_WATER_DOWN_MIST);
    // Keep Vanish bursts at Mario's takeoff/impact height instead of carrying
    // a downward ground-pound velocity into the floor.
    downwardVel = !vanishBurst && o->parentObj->oVelY < 0.0f ? o->parentObj->oVelY * 0.2f : 0.0f;
    for (i = 0; i < puffCount; i++) {
        yaw = (i * 65536) / puffCount;
        particle = try_spawn_particle(o, MODEL_MIST, bhvWhitePuffExplosion, PARTICLE_PRIORITY_FEEDBACK);
        if (particle == NULL) {
            break;
        }
        particle->oBehParams2ndByte =
            vanishBurst ? WHITE_PUFF_EXPLODE_BP_VANISH_RING : WHITE_PUFF_EXPLODE_BP_WATER_DOWN_IMPULSE;
        particle->oMoveAngleYaw = yaw;
        // Equal spacing, height, size, and outward speed keep the ring thin.
        particle->oPosX += 35.0f * sins(yaw);
        particle->oPosZ += 35.0f * coss(yaw);
        particle->oPosY += 20.0f;
        particle->oForwardVel = 6.0f;
        particle->oVelY = downwardVel;
        particle->oGravity = 0.0f;
        particle->oDragStrength = 30.0f;
        obj_scale(particle, 0.75f);
    }

    if (vanishBurst) {
        // A few short-lived flecks burst from Mario with the ring. They never
        // snap to the water surface or spawn a second surface splash.
        for (i = 0; i < 3; i++) {
            yaw = gMarioState->faceAngle[1] + (i - 1) * 0x3000;
            particle = try_spawn_particle(o, MODEL_WHITE_PARTICLE_SMALL, bhvWhitePuffExplosion,
                                          PARTICLE_PRIORITY_FEEDBACK);
            if (particle == NULL) {
                break;
            }
            particle->oBehParams2ndByte = WHITE_PUFF_EXPLODE_BP_FAST_FADE;
            particle->oMoveAngleYaw = yaw;
            particle->oPosX += 12.0f * sins(yaw);
            particle->oPosZ += 12.0f * coss(yaw);
            particle->oPosY += 75.0f;
            particle->oForwardVel = 5.0f;
            particle->oVelY = 8.0f;
            particle->oGravity = 0.0f;
            particle->oDragStrength = 30.0f;
            obj_scale(particle, 1.5f);
        }
    }
}

void spawn_mist_from_global(void) {
    cur_obj_spawn_particles_with_priority(&sGlobalMistParticles, PARTICLE_PRIORITY_MIST);
}

static struct SpawnParticlesInfo sSandParticles = {
    /* behParam:        */ 0,
    /* count:           */ 5,
    /* model:           */ MODEL_SAND_DUST,
    /* offsetY:         */ 0,
    /* forwardVelBase:  */ 0,
    /* forwardVelRange: */ 20,
    /* velYBase:        */ 20,
    /* velYRange:       */ 0,
    /* gravity:         */ 252,
    /* dragStrength:    */ 30,
    /* sizeBase:        */ 5.0f,
    /* sizeRange:       */ 2.0f,
};

void bhv_ground_sand_init(void) {
    cur_obj_clear_particle_parent_flags(ACTIVE_PARTICLE_DIRT);
    cur_obj_spawn_particles_with_priority(&sSandParticles, PARTICLE_PRIORITY_AMBIENT);
}

static const struct {
    s16 forwardVel;
    s16 velY;
    s16 gravity;
    s16 jitterWidth;
} sSmokeMovementParams = {
    .forwardVel = 2,
    .velY = -8,
    .gravity = 1,
    .jitterWidth = 4,
};

void spawn_smoke_with_velocity(void) {
    struct Object *smoke =
        try_spawn_particle(o, MODEL_SMOKE, bhvWhitePuffSmoke2, PARTICLE_PRIORITY_AMBIENT);
    if (smoke == NULL) {
        return;
    }

    smoke->oForwardVel = sSmokeMovementParams.forwardVel;
    smoke->oVelY = sSmokeMovementParams.velY;
    smoke->oGravity = sSmokeMovementParams.gravity;

    obj_translate_xyz_random(smoke, sSmokeMovementParams.jitterWidth);
}

void cur_obj_clear_particle_parent_flags(u32 flagMask) {
    particle_budget_clear_parent_flags(o, flagMask);
}

static struct SpawnParticlesInfo sSnowParticles = {
    /* behParam:        */ 0,
    /* count:           */ 5,
    /* model:           */ MODEL_WHITE_PARTICLE_DL,
    /* offsetY:         */ 0,
    /* forwardVelBase:  */ 0,
    /* forwardVelRange: */ 20,
    /* velYBase:        */ 20,
    /* velYRange:       */ 0,
    /* gravity:         */ 252,
    /* dragStrength:    */ 30,
    /* sizeBase:        */ 2.0f,
    /* sizeRange:       */ 2.0f,
};

void bhv_ground_snow_init(void) {
    cur_obj_clear_particle_parent_flags(ACTIVE_PARTICLE_SNOW);
    cur_obj_spawn_particles_with_priority(&sSnowParticles, PARTICLE_PRIORITY_AMBIENT);
}
