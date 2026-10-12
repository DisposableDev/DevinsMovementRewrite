#ifndef ENVFX_SNOW_H
#define ENVFX_SNOW_H

#include <PR/ultratypes.h>
#include "types.h"

struct EnvFxParticle {
    s8 isAlive;
    s16 animFrame; // animation frame for lava/flowers; lifetime for JRB jets
    s32 xPos;
    s32 yPos;
    s32 zPos;
    s32 angleAndDist[2]; // whirlpool/jet polar offset: [0] = angle, [1] = radius
    s32 unusedBubbleVar; // JRB jet age; negative values count down a spawn delay
    s32 bubbleY;         // whirlpool height; respawn delay for JRB jets
    u8 filler[24];
};

extern s8 gEnvFxMode;

extern struct EnvFxParticle *gEnvFxBuffer;
extern Vec3i gSnowCylinderLastPos;
extern s16 gSnowParticleCount;

Gfx *envfx_update_particles(s32 mode, Vec3s marioPos, Vec3s camTo, Vec3s camFrom);
void orbit_from_positions(Vec3s from, Vec3s to, s16 *radius, s16 *pitch, s16 *yaw);
void rotate_triangle_vertices(Vec3s vertex1, Vec3s vertex2, Vec3s vertex3, s16 pitch, s16 yaw);

#endif // ENVFX_SNOW_H
