#include <ultra64.h>

#include "sm64.h"
#include "game_init.h"
#include "memory.h"
#include "envfx_snow.h"
#include "envfx_bubbles.h"
#include "engine/surface_collision.h"
#include "engine/math_util.h"
#include "engine/behavior_script.h"
#include "audio/external.h"
#include "textures.h"
#include "level_geo.h"
#include "rendering_graph_node.h"
#include "level_table.h"
#include "area.h"

/**
 * This file implements environment effects that are not snow:
 * Flowers (unused), lava bubbles and jet stream/whirlpool bubbles.
 * Refer to 'envfx_snow.c' for more info about environment effects.
 * Note that the term 'bubbles' is used as a collective name for
 * effects in this file even though flowers aren't bubbles. For the
 * sake of concise naming, flowers fall under bubbles.
 */

s16 gEnvFxBubbleConfig[10];
static Gfx *sGfxCursor; // points to end of display list for bubble particles
static s32 sBubbleParticleCapacity;
static s32 sBubbleActiveCount;

/// Template for a bubble particle triangle
Vtx_t gBubbleTempVtx[3] = {
    { { 0, 0, 0 }, 0, { 1544, 964 }, { 0xFF, 0xFF, 0xFF, 0xFF } },
    { { 0, 0, 0 }, 0, { 522, -568 }, { 0xFF, 0xFF, 0xFF, 0xFF } },
    { { 0, 0, 0 }, 0, { -498, 964 }, { 0xFF, 0xFF, 0xFF, 0xFF } },
};

/**
 * Test XZ distance only, including the exact distance boundary.
 */
s32 particle_is_laterally_close(s32 index, s32 x, s32 z, s32 distance) {
    s32 xPos = (gEnvFxBuffer + index)->xPos;
    s32 zPos = (gEnvFxBuffer + index)->zPos;

    if (sqr(xPos - x) + sqr(zPos - z) > sqr(distance)) {
        return FALSE;
    }

    return TRUE;
}

/**
 * Create a gapped integer offset from one centered random sample.
 * Truncate before the sign test; integer zero follows the positive branch.
 */
s32 random_flower_offset(void) {
    s32 result = random_float() * 2000.0f - 1000.0f;
    if (result < 0) {
        result -= 1000;
    } else {
        result += 1000;
    }

    return result;
}

/**
 * Recycle flowers outside the camera target's 3000-unit XZ radius.
 * Use the returned floor height; survivors animate on globally aligned four-tick intervals.
 */
void envfx_update_flower(Vec3s centerPos) {
    s32 i;
    s32 globalTimer = gGlobalTimer;

    s16 centerX = centerPos[0];
    s16 centerZ = centerPos[2];

    for (i = 0; i < sBubbleActiveCount; i++) {
        (gEnvFxBuffer + i)->isAlive = particle_is_laterally_close(i, centerX, centerZ, 3000);
        if (!(gEnvFxBuffer + i)->isAlive) {
            (gEnvFxBuffer + i)->xPos = random_flower_offset() + centerX;
            (gEnvFxBuffer + i)->zPos = random_flower_offset() + centerZ;
            (gEnvFxBuffer + i)->yPos =
                find_floor_height((gEnvFxBuffer + i)->xPos, 10000.0f, (gEnvFxBuffer + i)->zPos);
            (gEnvFxBuffer + i)->isAlive = TRUE;
            (gEnvFxBuffer + i)->animFrame = random_float() * 5.0f;
        } else if (!(globalTimer & 3)) {
            (gEnvFxBuffer + i)->animFrame++;
            if ((gEnvFxBuffer + i)->animFrame > 5) {
                (gEnvFxBuffer + i)->animFrame = 0;
            }
        }
    }
}

/**
 * Sample XZ around the camera target, then reflect each axis at +/-8000.
 * Query the floor from centerY + 500 and narrow its height to s16 before checking
 * the surface. Only burning floors supply particle height; missing/non-burning
 * floors put the particle at FLOOR_LOWER_LIMIT_MISC without ending its animation.
 */
void envfx_set_lava_bubble_position(s32 index, Vec3s centerPos) {
    struct Surface *surface;
    s16 floorY;

    s16 centerX = centerPos[0];
    s16 centerY = centerPos[1];
    s16 centerZ = centerPos[2];

    (gEnvFxBuffer + index)->xPos = random_float() * 6000.0f - 3000.0f + centerX;
    (gEnvFxBuffer + index)->zPos = random_float() * 6000.0f - 3000.0f + centerZ;

    if ((gEnvFxBuffer + index)->xPos > 8000) {
        (gEnvFxBuffer + index)->xPos = 16000 - (gEnvFxBuffer + index)->xPos;
    }
    if ((gEnvFxBuffer + index)->xPos < -8000) {
        (gEnvFxBuffer + index)->xPos = -16000 - (gEnvFxBuffer + index)->xPos;
    }

    if ((gEnvFxBuffer + index)->zPos > 8000) {
        (gEnvFxBuffer + index)->zPos = 16000 - (gEnvFxBuffer + index)->zPos;
    }
    if ((gEnvFxBuffer + index)->zPos < -8000) {
        (gEnvFxBuffer + index)->zPos = -16000 - (gEnvFxBuffer + index)->zPos;
    }

    floorY =
        find_floor((gEnvFxBuffer + index)->xPos, centerY + 500, (gEnvFxBuffer + index)->zPos, &surface);
    if (surface == NULL) {
        (gEnvFxBuffer + index)->yPos = FLOOR_LOWER_LIMIT_MISC;
        return;
    }

    if (surface->type == SURFACE_BURNING) {
        (gEnvFxBuffer + index)->yPos = floorY;
    } else {
        (gEnvFxBuffer + index)->yPos = FLOOR_LOWER_LIMIT_MISC;
    }
}

/**
 * Reposition dead particles, including those below the course, on the next call.
 * Live particles advance frames 0-8 on even global ticks; completing the animation
 * resets the frame and marks the particle dead. Newly positioned particles do
 * not advance their animation in the same call.
 */
void envfx_update_lava(Vec3s centerPos) {
    s32 i;
    s32 globalTimer = gGlobalTimer;

    for (i = 0; i < sBubbleActiveCount; i++) {
        if (!(gEnvFxBuffer + i)->isAlive) {
            envfx_set_lava_bubble_position(i, centerPos);
            (gEnvFxBuffer + i)->isAlive = TRUE;
        } else if (!(globalTimer & 1)) {
            (gEnvFxBuffer + i)->animFrame += 1;
            if ((gEnvFxBuffer + i)->animFrame > 8) {
                (gEnvFxBuffer + i)->isAlive = FALSE;
                (gEnvFxBuffer + i)->animFrame = 0;
            }
        }
    }

    // One ambient-sound random draw after all particle updates, regardless of floor validity.
    if (((s8)(s32)(random_float() * 16.0f)) == 8) {
        play_sound(SOUND_GENERAL_QUIET_BUBBLE2, gGlobalSoundSource);
    }
}

// Pitch and negative legacy-YAW trigonometry, shared by all particles this update.
struct EnvFxWhirlpoolRotation {
    f32 cosPitch;
    f32 sinPitch;
    f32 cosNegativeYaw;
    f32 sinNegativeYaw;
};

/**
 * Rotate world coordinates in place around the configured DEST origin.
 * Truncate each rotated offset before adding the integer origin back.
 */
static void envfx_rotate_around_whirlpool(s32 *worldX, s32 *worldY, s32 *worldZ,
                                          const struct EnvFxWhirlpoolRotation *rotation) {
    s32 offsetX = *worldX - gEnvFxBubbleConfig[ENVFX_STATE_DEST_X];
    s32 offsetY = *worldY - gEnvFxBubbleConfig[ENVFX_STATE_DEST_Y];
    s32 offsetZ = *worldZ - gEnvFxBubbleConfig[ENVFX_STATE_DEST_Z];

    f32 rotatedOffsetX = offsetX * rotation->cosNegativeYaw
                         - rotation->sinNegativeYaw * rotation->cosPitch * offsetY
                         - rotation->sinPitch * rotation->sinNegativeYaw * offsetZ;
    f32 rotatedOffsetY = offsetX * rotation->sinNegativeYaw
                         + rotation->cosPitch * rotation->cosNegativeYaw * offsetY
                         - rotation->sinPitch * rotation->cosNegativeYaw * offsetZ;
    f32 rotatedOffsetZ = offsetY * rotation->sinPitch + rotation->cosPitch * offsetZ;

    *worldX = gEnvFxBubbleConfig[ENVFX_STATE_DEST_X] + (s32) rotatedOffsetX;
    *worldY = gEnvFxBubbleConfig[ENVFX_STATE_DEST_Y] + (s32) rotatedOffsetY;
    *worldZ = gEnvFxBubbleConfig[ENVFX_STATE_DEST_Z] + (s32) rotatedOffsetZ;
}

/**
 * Check unrotated bubbleY and radial distance, rather than rotated world position.
 * A bubble recycles below the sink's height cutoff or inside its center radius.
 */
s32 envfx_is_whirlpool_bubble_alive(s32 index) {
    if ((gEnvFxBuffer + index)->bubbleY < gEnvFxBubbleConfig[ENVFX_STATE_DEST_Y] - 100) {
        return FALSE;
    }

    if ((gEnvFxBuffer + index)->angleAndDist[1] < 10) {
        return FALSE;
    }

    return TRUE;
}

/**
 * Update radius, angle and unrotated bubbleY, then rotate the world positions.
 * Respawning uses three random draws; surviving particles spiral inward without RNG.
 */
void envfx_update_whirlpool(void) {
    s32 i;
    struct EnvFxWhirlpoolRotation rotation = {
        coss(gEnvFxBubbleConfig[ENVFX_STATE_PITCH]),
        sins(gEnvFxBubbleConfig[ENVFX_STATE_PITCH]),
        coss(-gEnvFxBubbleConfig[ENVFX_STATE_YAW]),
        sins(-gEnvFxBubbleConfig[ENVFX_STATE_YAW]),
    };

    for (i = 0; i < sBubbleActiveCount; i++) {
        (gEnvFxBuffer + i)->isAlive = envfx_is_whirlpool_bubble_alive(i);
        if (!(gEnvFxBuffer + i)->isAlive) {
            (gEnvFxBuffer + i)->angleAndDist[1] = random_float() * 1000.0f;
            (gEnvFxBuffer + i)->angleAndDist[0] = random_float() * 65536.0f;
            (gEnvFxBuffer + i)->xPos =
                gEnvFxBubbleConfig[ENVFX_STATE_SRC_X]
                + sins((gEnvFxBuffer + i)->angleAndDist[0]) * (gEnvFxBuffer + i)->angleAndDist[1];
            (gEnvFxBuffer + i)->zPos =
                gEnvFxBubbleConfig[ENVFX_STATE_SRC_Z]
                + coss((gEnvFxBuffer + i)->angleAndDist[0]) * (gEnvFxBuffer + i)->angleAndDist[1];
            (gEnvFxBuffer + i)->bubbleY =
                gEnvFxBubbleConfig[ENVFX_STATE_SRC_Y] + (random_float() * 100.0f - 50.0f);
            (gEnvFxBuffer + i)->yPos = (i + gEnvFxBuffer)->bubbleY;
            (gEnvFxBuffer + i)->unusedBubbleVar = 0;
            (gEnvFxBuffer + i)->isAlive = TRUE;

            envfx_rotate_around_whirlpool(&(gEnvFxBuffer + i)->xPos, &(gEnvFxBuffer + i)->yPos,
                                          &(gEnvFxBuffer + i)->zPos, &rotation);
        } else {
            (gEnvFxBuffer + i)->angleAndDist[1] -= 40;
            (gEnvFxBuffer + i)->angleAndDist[0] +=
                (s16) (3000 - (gEnvFxBuffer + i)->angleAndDist[1] * 2) + 0x400;
            (gEnvFxBuffer + i)->xPos =
                gEnvFxBubbleConfig[ENVFX_STATE_SRC_X]
                + sins((gEnvFxBuffer + i)->angleAndDist[0]) * (gEnvFxBuffer + i)->angleAndDist[1];
            (gEnvFxBuffer + i)->zPos =
                gEnvFxBubbleConfig[ENVFX_STATE_SRC_Z]
                + coss((gEnvFxBuffer + i)->angleAndDist[0]) * (gEnvFxBuffer + i)->angleAndDist[1];
            (gEnvFxBuffer + i)->bubbleY -= 40 - ((s16) (gEnvFxBuffer + i)->angleAndDist[1] / 100);
            (gEnvFxBuffer + i)->yPos = (i + gEnvFxBuffer)->bubbleY;

            envfx_rotate_around_whirlpool(&(gEnvFxBuffer + i)->xPos, &(gEnvFxBuffer + i)->yPos,
                                          &(gEnvFxBuffer + i)->zPos, &rotation);
        }
    }
}
/**
 * Jet particles move up and outwards, recycling outside their native bounds.
 * JRB varies lifetime and spawn delay slightly, using otherwise unused fields.
 */
void envfx_update_jetstream(void) {
    s32 sourceX = gEnvFxBubbleConfig[ENVFX_STATE_SRC_X];
    s32 sourceY = gEnvFxBubbleConfig[ENVFX_STATE_SRC_Y];
    s32 sourceZ = gEnvFxBubbleConfig[ENVFX_STATE_SRC_Z];
    s32 timedJet = gCurrLevelNum == LEVEL_JRB;

    for (s32 i = 0; i < sBubbleActiveCount; i++) {
        struct EnvFxParticle *particle = gEnvFxBuffer + i;

        if (timedJet && particle->unusedBubbleVar < 0) {
            particle->unusedBubbleVar++;
            continue;
        }
        s32 offsetX = particle->xPos - sourceX;
        s32 offsetZ = particle->zPos - sourceZ;

        // isAlive selects recycling; JRB draws newly spawned particles by positive age.
        particle->isAlive =
            (!timedJet
             || (particle->unusedBubbleVar > 0 && particle->unusedBubbleVar < particle->animFrame))
            && sqr(offsetX) + sqr(offsetZ) <= sqr(1000) && particle->yPos <= sourceY + 1500;
        if (!particle->isAlive) {
            if (timedJet && particle->unusedBubbleVar > 0 && particle->bubbleY > 0) {
                // This update is the first hidden update of the respawn delay.
                particle->unusedBubbleVar = 1 - particle->bubbleY;
                continue;
            }
            particle->angleAndDist[1] = random_float() * 300.0f;
            particle->angleAndDist[0] = random_u16();
            particle->xPos = sourceX + sins(particle->angleAndDist[0]) * particle->angleAndDist[1];
            particle->zPos = sourceZ + coss(particle->angleAndDist[0]) * particle->angleAndDist[1];
            particle->yPos = sourceY + (random_float() * 400.0f - 200.0f);
            if (timedJet) {
                particle->unusedBubbleVar = 1;
                // Scale eight angle bits to six lifetimes without extra random draws.
                particle->animFrame =
                    JRB_JETSTREAM_BUBBLE_LIFETIME
                    - (((particle->angleAndDist[0] & 0xFF) * (JRB_JETSTREAM_BUBBLE_LIFETIME_JITTER + 1))
                       >> 8);
                particle->bubbleY =
                    (particle->angleAndDist[0] >> 10) & JRB_JETSTREAM_BUBBLE_RESPAWN_DELAY;
            }
        } else {
            particle->angleAndDist[1] += 10;
            particle->xPos += sins(particle->angleAndDist[0]) * 10.0f;
            particle->zPos += coss(particle->angleAndDist[0]) * 10.0f;
            particle->yPos -= (particle->angleAndDist[1] / 30) - 50;
            if (timedJet) {
                particle->unusedBubbleVar++;
            }
        }
    }
}

/**
 * Allocate and reset bubble/flower storage, then publish the active mode.
 * NONE or allocation failure returns FALSE without activating the requested mode.
 * Flowers/lava use fixed active counts; whirlpool/jet counts come from emitters.
 */
s32 envfx_init_bubble(s32 mode) {
    s32 i;

    switch (mode) {
        case ENVFX_MODE_NONE:
            return FALSE;

        case ENVFX_FLOWERS:
            sBubbleParticleCapacity = 30;
            sBubbleActiveCount = 30;
            break;

        case ENVFX_LAVA_BUBBLES:
            sBubbleParticleCapacity = 15;
            sBubbleActiveCount = 15;
            break;

        case ENVFX_WHIRLPOOL_BUBBLES:
            sBubbleParticleCapacity = 60;
            break;

        case ENVFX_JETSTREAM_BUBBLES:
            sBubbleParticleCapacity = gCurrLevelNum == LEVEL_JRB ? JRB_JETSTREAM_BUBBLE_COUNT : 60;
            break;
    }

    gEnvFxBuffer =
        mem_pool_alloc(gEffectsMemoryPool, sBubbleParticleCapacity * sizeof(struct EnvFxParticle));
    if (gEnvFxBuffer == NULL) {
        return FALSE;
    }

    bzero(gEnvFxBuffer, sBubbleParticleCapacity * sizeof(struct EnvFxParticle));
    bzero(gEnvFxBubbleConfig, sizeof(gEnvFxBubbleConfig));

    switch (mode) {
        case ENVFX_JETSTREAM_BUBBLES:
            if (gCurrLevelNum == LEVEL_JRB) {
                for (i = 0; i < sBubbleParticleCapacity; i++) {
                    (gEnvFxBuffer + i)->unusedBubbleVar =
                        -((random_u16() * (JRB_JETSTREAM_BUBBLE_STARTUP_DELAY + 1)) >> 16);
                }
            }
            break;

        case ENVFX_LAVA_BUBBLES:
            for (i = 0; i < sBubbleParticleCapacity; i++) {
                (gEnvFxBuffer + i)->animFrame = random_float() * 7.0f;
            }
            break;
    }

    gEnvFxMode = mode;
    return TRUE;
}

/**
 * Advance the chosen effect, then set its unrotated billboard triangle offsets.
 * The caller rotates these offsets before translating them to particle positions.
 */
void envfx_bubbles_update_switch(s32 mode, Vec3s camTo, Vec3s vertex1, Vec3s vertex2, Vec3s vertex3) {
    switch (mode) {
        case ENVFX_FLOWERS:
            envfx_update_flower(camTo);
            vertex1[0] = 50;
            vertex1[1] = 0;
            vertex1[2] = 0;
            vertex2[0] = 0;
            vertex2[1] = 75;
            vertex2[2] = 0;
            vertex3[0] = -50;
            vertex3[1] = 0;
            vertex3[2] = 0;
            break;

        case ENVFX_LAVA_BUBBLES:
            envfx_update_lava(camTo);
            vertex1[0] = 100;
            vertex1[1] = 0;
            vertex1[2] = 0;
            vertex2[0] = 0;
            vertex2[1] = 150;
            vertex2[2] = 0;
            vertex3[0] = -100;
            vertex3[1] = 0;
            vertex3[2] = 0;
            break;

        case ENVFX_WHIRLPOOL_BUBBLES:
            envfx_update_whirlpool();
            vertex1[0] = 40;
            vertex1[1] = 0;
            vertex1[2] = 0;
            vertex2[0] = 0;
            vertex2[1] = 60;
            vertex2[2] = 0;
            vertex3[0] = -40;
            vertex3[1] = 0;
            vertex3[2] = 0;
            break;

        case ENVFX_JETSTREAM_BUBBLES:
            envfx_update_jetstream();
            vertex1[0] = 40;
            vertex1[1] = 0;
            vertex1[2] = 0;
            vertex2[0] = 0;
            vertex2[1] = 60;
            vertex2[2] = 0;
            vertex3[0] = -40;
            vertex3[1] = 0;
            vertex3[2] = 0;
            break;
    }
}

/**
 * Write 15 vertices for five particles starting at firstParticleIndex, then load them.
 * Copy each complete template vertex before replacing its position with the particle
 * position plus a rotated triangle offset; the sums narrow to the vertex's s16 fields.
 */
void append_bubble_vertex_buffer(Gfx *gfx, Vtx *vertBuf, s32 firstParticleIndex, Vec3s vertex1,
                                 Vec3s vertex2, Vec3s vertex3, Vtx *template) {
    s32 i = 0;

    for (i = 0; i < 15; i += 3) {
        vertBuf[i] = template[0];
        (vertBuf + i)->v.ob[0] = (gEnvFxBuffer + (firstParticleIndex + i / 3))->xPos + vertex1[0];
        (vertBuf + i)->v.ob[1] = (gEnvFxBuffer + (firstParticleIndex + i / 3))->yPos + vertex1[1];
        (vertBuf + i)->v.ob[2] = (gEnvFxBuffer + (firstParticleIndex + i / 3))->zPos + vertex1[2];

        vertBuf[i + 1] = template[1];
        (vertBuf + i + 1)->v.ob[0] = (gEnvFxBuffer + (firstParticleIndex + i / 3))->xPos + vertex2[0];
        (vertBuf + i + 1)->v.ob[1] = (gEnvFxBuffer + (firstParticleIndex + i / 3))->yPos + vertex2[1];
        (vertBuf + i + 1)->v.ob[2] = (gEnvFxBuffer + (firstParticleIndex + i / 3))->zPos + vertex2[2];

        vertBuf[i + 2] = template[2];
        (vertBuf + i + 2)->v.ob[0] = (gEnvFxBuffer + (firstParticleIndex + i / 3))->xPos + vertex3[0];
        (vertBuf + i + 2)->v.ob[1] = (gEnvFxBuffer + (firstParticleIndex + i / 3))->yPos + vertex3[1];
        (vertBuf + i + 2)->v.ob[2] = (gEnvFxBuffer + (firstParticleIndex + i / 3))->zPos + vertex3[2];
    }

    gSPVertex(gfx, VIRTUAL_TO_PHYSICAL(vertBuf), 15, 0);
}

/**
 * Flowers/lava use the first particle's animation frame for the whole group;
 * whirlpools/jets use frame zero. Convert the table address and return its entry
 * as stored. Unsupported modes return NULL.
 */
static void *envfx_get_bubble_texture(s32 mode, s16 firstParticleIndex) {
    void **textureFrames;
    s16 frame = (gEnvFxBuffer + firstParticleIndex)->animFrame;

    switch (mode) {
        case ENVFX_FLOWERS:
            textureFrames = segmented_to_virtual(&flower_bubbles_textures_ptr_0B002008);
            frame = (gEnvFxBuffer + firstParticleIndex)->animFrame;
            break;

        case ENVFX_LAVA_BUBBLES:
            textureFrames = segmented_to_virtual(&lava_bubble_ptr_0B006020);
            frame = (gEnvFxBuffer + firstParticleIndex)->animFrame;
            break;

        case ENVFX_WHIRLPOOL_BUBBLES:
        case ENVFX_JETSTREAM_BUBBLES:
            textureFrames = segmented_to_virtual(&bubble_ptr_0B006848);
            frame = 0;
            break;
        default:
            return NULL;
    }

    return *(textureFrames + frame);
}

/**
 * Conservatively test one complete five-particle JRB group, ignoring hidden ages.
 * Keep groups whose vertex coordinates may wrap when narrowed to s16.
 */
static s32 envfx_jetstream_group_in_view(s32 firstParticleIndex) {
    Vec3f minimum = { 32767.0f, 32767.0f, 32767.0f };
    Vec3f maximum = { -32767.0f, -32767.0f, -32767.0f };
    s32 hasActiveParticle = FALSE;

    for (s32 i = 0; i < 5; i++) {
        struct EnvFxParticle *particle = &gEnvFxBuffer[firstParticleIndex + i];
        if (particle->unusedBubbleVar <= 0) {
            continue;
        }
        hasActiveParticle = TRUE;
        Vec3f position = { particle->xPos, particle->yPos, particle->zPos };
        for (s32 axis = 0; axis < 3; axis++) {
            // Bubble vertices narrow to s16. Keep any possible wrapping case.
            if (!(absf(position[axis]) < 32000.0f)) {
                return TRUE;
            }
            minimum[axis] = MIN(minimum[axis], position[axis] - 128.0f);
            maximum[axis] = MAX(maximum[axis], position[axis] + 128.0f);
        }
    }
    // Rotating the native 60-unit triangle cannot leave this padded box.
    return hasActiveParticle && geo_camera_box_in_view(minimum, maximum);
}

/**
 * Allocate frame buffers, advance particles, then emit complete five-particle groups.
 * The active count must be a multiple of five. Allocation failure returns NULL
 * before advancement; JRB visibility rejection skips draw commands after advancement.
 */
Gfx *envfx_update_bubble_particles(s32 mode, UNUSED Vec3s marioPos, Vec3s camFrom, Vec3s camTo) {
    s32 i;
    s16 radius, pitch, yaw;

    Vec3s vertex1;
    Vec3s vertex2;
    Vec3s vertex3;
    Vtx *vertices;
    void *lastTexture = NULL;
    s32 timedJet = mode == ENVFX_JETSTREAM_BUBBLES && gCurrLevelNum == LEVEL_JRB;

    // At most nine commands per group, plus setup and cleanup.
    Gfx *gfxStart = alloc_display_list(((sBubbleActiveCount / 5) * 9 + 3) * sizeof(Gfx));
    if (gfxStart == NULL) {
        return NULL;
    }

    vertices = alloc_display_list(sBubbleActiveCount * 3 * sizeof(Vtx));
    if (vertices == NULL) {
        return NULL;
    }

    sGfxCursor = gfxStart;

    orbit_from_positions(camTo, camFrom, &radius, &pitch, &yaw);
    envfx_bubbles_update_switch(mode, camTo, vertex1, vertex2, vertex3);
    rotate_triangle_vertices(vertex1, vertex2, vertex3, pitch, yaw);

    gSPDisplayList(sGfxCursor++, &tiny_bubble_dl_0B006D38);

    for (i = 0; i < sBubbleActiveCount; i += 5) {
        if (timedJet && !envfx_jetstream_group_in_view(i)) {
            continue;
        }
        // Jet groups all use frame zero; resolve its segmented texture once.
        void *texture = mode == ENVFX_JETSTREAM_BUBBLES && lastTexture != NULL
                            ? lastTexture
                            : envfx_get_bubble_texture(mode, i);

        // Vertex and triangle commands leave the loaded texture in TMEM.
        if (texture != lastTexture) {
            gDPPipeSync(sGfxCursor++);
            gDPSetTextureImage(sGfxCursor++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture);
            gSPDisplayList(sGfxCursor++, &tiny_bubble_dl_0B006D68);
            lastTexture = texture;
        }
        append_bubble_vertex_buffer(sGfxCursor++, vertices + i * 3, i, vertex1, vertex2, vertex3,
                                    (Vtx *) gBubbleTempVtx);
        if (timedJet) {
            for (s32 j = 0; j < 5; j++) {
                if (gEnvFxBuffer[i + j].unusedBubbleVar > 0) {
                    gSP1Triangle(sGfxCursor++, j * 3, j * 3 + 1, j * 3 + 2, 0);
                }
            }
        } else {
            gSP1Triangle(sGfxCursor++, 0, 1, 2, 0);
            gSP1Triangle(sGfxCursor++, 3, 4, 5, 0);
            gSP1Triangle(sGfxCursor++, 6, 7, 8, 0);
            gSP1Triangle(sGfxCursor++, 9, 10, 11, 0);
            gSP1Triangle(sGfxCursor++, 12, 13, 14, 0);
        }
    }

    gSPDisplayList(sGfxCursor++, &tiny_bubble_dl_0B006AB0);
    gSPEndDisplayList(sGfxCursor++);

    return gfxStart;
}

/**
 * Refresh the active loop/draw count from shared whirlpool or jet emitter config.
 * Flowers/lava retain the fixed count selected during initialization.
 */
void envfx_set_max_bubble_particles(s32 mode) {
    switch (mode) {
        case ENVFX_WHIRLPOOL_BUBBLES:
            sBubbleActiveCount = gEnvFxBubbleConfig[ENVFX_STATE_PARTICLECOUNT];
            break;
        case ENVFX_JETSTREAM_BUBBLES:
            sBubbleActiveCount = gEnvFxBubbleConfig[ENVFX_STATE_PARTICLECOUNT];
            break;
    }
}

/**
 * Update supported bubble modes; the outer particle dispatcher owns mode changes
 * and buffer cleanup. A zero active count retains particle state without advancing it.
 * Return NULL for initialization failure, zero count, unsupported mode or failed
 * frame allocation; otherwise return the particle display list.
 */
Gfx *envfx_update_bubbles(s32 mode, Vec3s marioPos, Vec3s camTo, Vec3s camFrom) {
    Gfx *gfx;

    if (gEnvFxMode == ENVFX_MODE_NONE && !envfx_init_bubble(mode)) {
        return NULL;
    }

    envfx_set_max_bubble_particles(mode);

    if (sBubbleActiveCount == 0) {
        return NULL;
    }

    switch (mode) {
        case ENVFX_FLOWERS:
            gfx = envfx_update_bubble_particles(ENVFX_FLOWERS, marioPos, camFrom, camTo);
            break;

        case ENVFX_LAVA_BUBBLES:
            gfx = envfx_update_bubble_particles(ENVFX_LAVA_BUBBLES, marioPos, camFrom, camTo);
            break;

        case ENVFX_WHIRLPOOL_BUBBLES:
            gfx = envfx_update_bubble_particles(ENVFX_WHIRLPOOL_BUBBLES, marioPos, camFrom, camTo);
            break;

        case ENVFX_JETSTREAM_BUBBLES:
            gfx = envfx_update_bubble_particles(ENVFX_JETSTREAM_BUBBLES, marioPos, camFrom, camTo);
            break;

        default:
            return NULL;
    }

    return gfx;
}
