// break_particles.inc.c

void spawn_triangle_break_particles_with_priority(s16 numTris, s16 triModel, f32 scale,
                                                  s16 triAnimState, enum ParticlePriority priority) {
    struct Object *triangle;
    s32 i;

    s16 burstCount = particle_budget_burst_count(priority, numTris);
    for (i = 0; i < burstCount; i++) {
        triangle = try_spawn_particle(o, triModel, bhvBreakBoxTriangle, priority);
        if (triangle == NULL) {
            break;
        }
        triangle->oAnimState = triAnimState;
        triangle->oPosY += 100.0f;
        triangle->oMoveAngleYaw = random_u16();
        triangle->oFaceAngleYaw = triangle->oMoveAngleYaw;
        triangle->oFaceAnglePitch = random_u16();
        triangle->oVelY = random_f32_around_zero(50.0f);

        if (triModel == MODEL_DIRT_ANIMATION || triModel == MODEL_SL_CRACKED_ICE_CHUNK) {
            triangle->oAngleVelPitch = 0xF00;
            triangle->oAngleVelYaw = 0x500;
            triangle->oForwardVel = 30.0f;
        } else {
            triangle->oAngleVelPitch = 0x80 * (s32)(random_float() + 50.0f);
            triangle->oForwardVel = 30.0f;
        }

        obj_scale(triangle, scale);
    }
}

void spawn_triangle_break_particles(s16 numTris, s16 triModel, f32 scale, s16 triAnimState) {
    spawn_triangle_break_particles_with_priority(numTris, triModel, scale, triAnimState,
                                                 PARTICLE_PRIORITY_DEBRIS);
}
