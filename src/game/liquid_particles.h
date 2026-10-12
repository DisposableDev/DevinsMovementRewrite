#ifndef LIQUID_PARTICLES_H
#define LIQUID_PARTICLES_H

#include <PR/ultratypes.h>

struct MarioState;

enum LiquidParticleMaterial { LIQUID_PARTICLE_WATER, LIQUID_PARTICLE_COLD_WATER, LIQUID_PARTICLE_LAVA };

// Shared by droplets and their splash children. Only SLIDE_KICK uses the second
// behavior parameter as a LiquidParticleMaterial; ordinary/soaked splashes stay white.
enum WaterDropletAction {
    WATER_DROPLET_ACT_NORMAL,
    WATER_DROPLET_ACT_SOAKED,
    WATER_DROPLET_ACT_SLIDE_KICK
};

#define LIQUID_PARTICLE_COLD_R 128
#define LIQUID_PARTICLE_COLD_G 224
#define LIQUID_PARTICLE_COLD_B 255
#define LIQUID_PARTICLE_LAVA_R 255
#define LIQUID_PARTICLE_LAVA_G 192
#define LIQUID_PARTICLE_LAVA_B 64
#define LIQUID_PARTICLE_LAVA_DROPLET_SCALE 2.0f
#define LIQUID_PARTICLE_LAVA_SPLASH_SCALE 1.5f
#define SLIDE_KICK_LIQUID_DROPLET_COUNT 18

// Spawns at Mario's resolved X/Z and the contact height, in world units.
// Particle-budget or allocation failure affects only the visual burst.
void spawn_slide_kick_liquid_particles(struct MarioState *m, f32 contactHeight,
                                       enum LiquidParticleMaterial material);

#endif
