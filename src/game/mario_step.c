#include "player_stats.h"
#include <ultra64.h>

#include "sm64.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "engine/surface_load.h"
#include "mario.h"
#include "mario_character.h"
#include "mario_actions_airborne.h"
#include "mario_collision.h"
#include "audio/external.h"
#include "game_init.h"
#include "game_options.h"
#include "game_options_runtime.h"
#include "interaction.h"
#include "mario_step.h"
#include "object_list_processor.h"
#include "behavior_actions.h"

#include "config.h"

#define MARIO_GROUND_STEP_MAX_FLOOR_DELTA 100.0f
#define MARIO_GROUND_LOW_STEP_MAX_HEIGHT 55.0f
#define MARIO_WATER_WALK_COOLDOWN 5
#define MARIO_WATER_WALK_MIN_SPEED 30.0f
#define MARIO_WATER_WALK_MIN_Y_VEL -60.0f

static s32 mario_water_walk_conditions_met(struct MarioState *m, f32 waterLevel, f32 ceilHeight) {
    s32 holdingWaterTwirl = m->waterWalkTwirl && m->action == ACT_GROUND_TWIRL
                            && (m->controller->buttonDown & L_TRIG)
                            && !(m->input & INPUT_NONZERO_ANALOG);
    if (game_options_get(GO_WING_CAP) != 0 || !(m->flags & MARIO_WING_CAP) || m->waterWalkDisabled
        || (m->action & ACT_FLAG_INTANGIBLE) || (m->action & ACT_GROUP_MASK) == ACT_GROUP_SUBMERGED
        || m->action == ACT_GROUND_POUND
        || (!holdingWaterTwirl && m->forwardVel < MARIO_WATER_WALK_MIN_SPEED)
        || m->vel[1] <= MARIO_WATER_WALK_MIN_Y_VEL || waterLevel <= FLOOR_LOWER_LIMIT
        || m->pos[1] < waterLevel - 100.0f || waterLevel + 160.0f >= ceilHeight) {
        return FALSE;
    }
    if (mario_would_take_fall_damage_on_water(m, waterLevel)) {
        return FALSE;
    }
    switch (m->action) {
        case ACT_FLYING:
        case ACT_FLYING_TRIPLE_JUMP:
        case ACT_DIVE:
        case ACT_DIVE_SLIDE:
        case ACT_DIVE_PICKING_UP:
        case ACT_STOMACH_SLIDE:
        case ACT_HOLD_STOMACH_SLIDE:
        case ACT_BUTT_SLIDE:
        case ACT_HOLD_BUTT_SLIDE:
        case ACT_SLIDE_KICK:
        case ACT_SLIDE_KICK_SLIDE:
        case ACT_HE_THICC_BELLY_SLIDE:
        case ACT_HE_THICC_WAVEDASH:
            return FALSE;
    }
    // A swimming jump must clear the surface before it can land on it.
    if ((m->action == ACT_WATER_JUMP || m->action == ACT_HOLD_WATER_JUMP) && m->pos[1] < waterLevel) {
        return FALSE;
    }
    return TRUE;
}

void mario_tick_water_walking_cooldown(struct MarioState *m) {
    if (m->waterWalkCooldown > 0) {
        m->waterWalkCooldown--;
    }
}

void mario_update_water_walking_state(struct MarioState *m, f32 waterLevel, f32 ceilHeight) {
    s32 grounded = m->floor != NULL && m->floor != &gWaterSurfacePseudoFloor
                   && absf(m->pos[1] - m->floorHeight) <= 0.01f && m->vel[1] <= 0.0f;
    // Shallow footing rearms surface support just like dry land.
    if (grounded && (waterLevel <= FLOOR_LOWER_LIMIT || m->floorHeight >= waterLevel - 100.0f)) {
        m->waterWalkDisabled = FALSE;
    } else if (waterLevel > FLOOR_LOWER_LIMIT && m->pos[1] < waterLevel) {
        m->waterWalkDisabled = TRUE;
    }
    // Check the restrictions independently of the timer, so it can expire.
    if (waterLevel > FLOOR_LOWER_LIMIT && !mario_water_walk_conditions_met(m, waterLevel, ceilHeight)) {
        m->waterWalkCooldown = MARIO_WATER_WALK_COOLDOWN;
    }
}

s32 mario_can_walk_on_water(struct MarioState *m, f32 waterLevel, f32 ceilHeight) {
    // This entry delay must not keep refreshing the shared five-frame cooldown.
    if ((m->action == ACT_FORWARD_ROLLOUT || m->action == ACT_BACKWARD_ROLLOUT)
        && m->rolloutFrames < ROLLOUT_WATER_WALK_DELAY_FRAMES) {
        return FALSE;
    }
    return m->waterWalkCooldown == 0 && mario_water_walk_conditions_met(m, waterLevel, ceilHeight);
}

f32 mario_get_movement_time_scale(struct MarioState *m) {
    if (!mario_has_current_metal_cap(m) || mario_has_current_vanish_cap(m)
        || m->waterLevel <= FLOOR_LOWER_LIMIT) {
        return 1.0f;
    }

    // Physical immersion keeps the same resistance across crouches, attacks,
    // held actions and Wing combinations, and eases it at the waterline.
    f32 immersion =
        CLAMP((m->waterLevel - m->pos[1] - METAL_WATER_RESISTANCE_START_DEPTH)
                  / (METAL_WATER_RESISTANCE_FULL_DEPTH - METAL_WATER_RESISTANCE_START_DEPTH),
              0.0f, 1.0f);
    return 1.0f - (1.0f - METAL_WATER_TIME_SCALE) * immersion;
}

static f32 mario_max_fall_speed(void) {
    return mario_character_palette() == MARIO_PALETTE_HE_THICC ? HE_THICC_MAX_FALL_SPEED
                                                               : MARIO_MAX_FALL_SPEED;
}

static void limit_metal_water_fall_speed(struct MarioState *m, f32 timeScale) {
    if (timeScale < 1.0f && !game_options_cheat_active(GO_NO_SPEED_LIMIT)) {
        f32 immersion = (1.0f - timeScale) / (1.0f - METAL_WATER_TIME_SCALE);
        f32 dryLimit = mario_max_fall_speed();
        f32 terminalSpeed = dryLimit + (METAL_WATER_MAX_FALL_SPEED / timeScale - dryLimit) * immersion;
        m->vel[1] = MAX(m->vel[1], -terminalSpeed);
    }
}

static s16 sMovingSandSpeeds[] = { 12, 8, 4, 0 };

struct Surface gWaterSurfacePseudoFloor = {
    SURFACE_VERY_SLIPPERY,      // type
    0x0,                        // force
    0x0,                        // flags
     0,                         // room
    -SURFACE_VERTICAL_BUFFER,   // lowerY
     SURFACE_VERTICAL_BUFFER,   // upperY
    { 0, 0, 0 },                // vertex1
    { 0, 0, 0 },                // vertex2
    { 0, 0, 0 },                // vertex3
    { 0.0f, 1.0f, 0.0f },       // normal
    0.0f,                       // originOffset
    NULL,                       // object
};

/**
 * Always returns zero. This may have been intended
 * to be used for the beta trampoline. Its return value
 * is used by set_mario_y_vel_based_on_fspeed as a constant
 * addition to Mario's Y velocity. Given the closeness of
 * this function to stub_mario_step_2, it is probable that this
 * was intended to check whether a trampoline had made itself
 * known through stub_mario_step_2 and whether Mario was on it,
 * and if so return a higher value than 0.
 */
f32 get_additive_y_vel_for_jumps(void) {
    return 0.0f;
}

/**
 * Does nothing, but takes in a MarioState. This is only ever
 * called by update_mario_inputs, which is called as part of Mario's
 * update routine. Due to its proximity to stub_mario_step_2, an
 * incomplete trampoline function, and get_additive_y_vel_for_jumps,
 * a potentially trampoline-related function, it is plausible that
 * this could be used for checking if Mario was on the trampoline.
 * It could, for example, make him bounce.
 */
void stub_mario_step_1(UNUSED struct MarioState *x) {
}

/**
 * Does nothing. This is only called by the beta trampoline.
 * Due to its proximity to get_additive_y_vel_for_jumps, another
 * currently-pointless function, it is probable that this was used
 * by the trampoline to make itself known to get_additive_y_vel_for_jumps,
 * or to set a variable with its intended additive Y vel.
 */
void stub_mario_step_2(void) {
}

void transfer_bully_speed(struct BullyCollisionData *obj1, struct BullyCollisionData *obj2) {
    f32 rx = obj2->posX - obj1->posX;
    f32 rz = obj2->posZ - obj1->posZ;

    //! Bully NaN crash
    f32 rzx = sqr(rx) + sqr(rz);
    f32 projectedV1 = (rx * obj1->velX + rz * obj1->velZ) / rzx;
    f32 projectedV2 = (-rx * obj2->velX - rz * obj2->velZ) / rzx;

    // Kill speed along r. Convert one object's speed along r and transfer it to
    // the other object.
    f32 p1x = projectedV1 * rx;
    f32 p1z = projectedV1 * rz;
    f32 p2x = projectedV2 * -rx;
    f32 p2z = projectedV2 * -rz;

    obj2->velX += obj2->conversionRatio * p1x - p2x;
    obj2->velZ += obj2->conversionRatio * p1z - p2z;

    obj1->velX += -p1x + obj1->conversionRatio * p2x;
    obj1->velZ += -p1z + obj1->conversionRatio * p2z;

    //! Bully battery
}

void init_bully_collision_data(struct BullyCollisionData *data, f32 posX, f32 posZ,
                               f32 forwardVel, s16 yaw, f32 conversionRatio, f32 radius) {
    if (forwardVel < 0.0f) {
        forwardVel *= -1.0f;
        yaw += 0x8000;
    }

    data->radius = radius;
    data->conversionRatio = conversionRatio;
    data->posX = posX;
    data->posZ = posZ;
    data->velX = forwardVel * sins(yaw);
    data->velZ = forwardVel * coss(yaw);
}

void mario_play_bonk_sound(struct MarioState *m) {
    player_stats_wall_bonk(m);
    f32 *source = m->marioObj->header.gfx.cameraToObject;

    castle_secret_window_bonk(m);
    if (absf(m->forwardVel) >= 32.0f) {
        mario_he_thicc_contact_shake(m);
    }

    // Clear both variants, including requests not yet consumed by the audio thread.
    stop_sound(SOUND_ACTION_BONK, source);
    stop_sound(SOUND_ACTION_METAL_BONK, source);
    play_sound((m->flags & MARIO_METAL_CAP) ? SOUND_ACTION_METAL_BONK : SOUND_ACTION_BONK, source);
}

void mario_play_wall_bonk_sound(struct MarioState *m) {
    if (m->wallTwirlSoundTimer == 0) {
        player_stats_wall_bonk(m);
        mario_play_bonk_sound(m);
        m->wallTwirlSoundTimer = MARIO_WALL_TWIRL_SOUND_GAP_FRAMES;
    } else if (absf(m->forwardVel) >= 32.0f) {
        mario_he_thicc_contact_shake(m);
    }
}

void mario_play_air_twirl_sound(struct MarioState *m) {
    if (m->wallTwirlSoundTimer == 0) {
        play_sound(SOUND_ACTION_TWIRL, m->marioObj->header.gfx.cameraToObject);
        m->wallTwirlSoundTimer = MARIO_WALL_TWIRL_SOUND_GAP_FRAMES;
    }
}

void mario_bonk_reflection(struct MarioState *m, u32 negateSpeed) {
    if (m->wall != NULL) {
        s16 wallAngle = m->wallYaw;
        m->faceAngle[1] = wallAngle - (s16)(m->faceAngle[1] - wallAngle);

        if (m->action == ACT_L_AIR_TWIRL
            || (m->action == ACT_FLYING && m->airTwirlFlightRollFrame != 0)) {
            mario_play_wall_bonk_sound(m);
        } else {
            mario_play_bonk_sound(m);
        }
    } else {
        play_sound(SOUND_ACTION_HIT, m->marioObj->header.gfx.cameraToObject);
    }

    if (negateSpeed) {
        mario_set_forward_vel(m, -m->forwardVel);
    } else {
        m->faceAngle[1] += 0x8000;
    }
}

// Check only the incoming update's path; do not move Mario during the preview.
struct Surface *mario_find_air_twirl_entry_wall(struct MarioState *m) {
    f32 timeScale = mario_get_movement_time_scale(m);
    for (s32 step = 0; step <= 4; step++) {
        f32 fraction = timeScale * step / 4.0f;
        for (s32 height = 0; height < 2; height++) {
            struct WallCollisionData walls = {
                .x = m->pos[0] + m->vel[0] * fraction,
                .y = m->pos[1] + m->vel[1] * fraction,
                .z = m->pos[2] + m->vel[2] * fraction,
                .offsetY = height == 0 ? 150.0f : 30.0f,
                .radius = 50.0f,
            };
            find_wall_collisions(&walls);
            for (s32 wall = walls.numWalls - 1; wall >= 0; wall--) {
                struct Surface *surface = walls.walls[wall];
                f32 approach = m->vel[0] * surface->normal.x + m->vel[2] * surface->normal.z;
                if (approach < 0.0f && surface->type != SURFACE_BURNING) {
                    return surface;
                }
            }
        }
    }
    return NULL;
}

void mario_set_wall_bonk_momentum(struct MarioState *m, f32 recoilSpeed, f32 minimumSpeed) {
    player_stats_wall_bonk(m);
    // Call after entering recovery: ordinary action changes clear bonk ownership.
    f32 speed = MAX(absf(recoilSpeed) * WALL_BONK_MOMENTUM_RETENTION, minimumSpeed);
    mario_set_forward_vel(m, recoilSpeed > 0.0f ? speed : -speed);
    m->wallBonkMomentum = TRUE;
}

s32 mario_is_on_floor(struct MarioState *m) {
    return m->floor != NULL && !(m->action & ACT_FLAG_AIR) && absf(m->pos[1] - m->floorHeight) <= 4.0f;
}

u32 mario_update_quicksand(struct MarioState *m, f32 sinkingSpeed) {
    if (game_options_cheat_active(GO_IGNORE_SAND)) {
        m->quicksandDepth = 0.0f;
        return FALSE;
    }
    if (!mario_is_on_floor(m)) {
        m->quicksandDepth = 0.0f;
        return FALSE;
    }
    if (m->action & ACT_FLAG_RIDING_SHELL) {
        m->quicksandDepth = 0.0f;
    } else {
        if (m->quicksandDepth < 1.1f) {
            m->quicksandDepth = 1.1f;
        }

        switch (m->floor->type) {
            case SURFACE_SHALLOW_QUICKSAND:
                if ((m->quicksandDepth += sinkingSpeed) >= 10.0f) {
                    m->quicksandDepth = 10.0f;
                }
                break;

            case SURFACE_SHALLOW_MOVING_QUICKSAND:
                if ((m->quicksandDepth += sinkingSpeed) >= 25.0f) {
                    m->quicksandDepth = 25.0f;
                }
                break;

            case SURFACE_QUICKSAND:
            case SURFACE_MOVING_QUICKSAND:
                if ((m->quicksandDepth += sinkingSpeed) >= 60.0f) {
                    m->quicksandDepth = 60.0f;
                }
                break;

            case SURFACE_DEEP_QUICKSAND:
            case SURFACE_DEEP_MOVING_QUICKSAND:
                if ((m->quicksandDepth += sinkingSpeed) >= 160.0f) {
                    update_mario_sound_and_camera(m);
                    return drop_and_set_mario_action(m, ACT_QUICKSAND_DEATH, 0);
                }
                break;

            case SURFACE_INSTANT_QUICKSAND:
            case SURFACE_INSTANT_MOVING_QUICKSAND:
                update_mario_sound_and_camera(m);
                return drop_and_set_mario_action(m, ACT_QUICKSAND_DEATH, 0);
                break;

            default:
                m->quicksandDepth = 0.0f;
                break;
        }
    }

    return FALSE;
}

u32 mario_push_off_steep_floor(struct MarioState *m, u32 action, u32 actionArg) {
    s16 floorDYaw = m->floorYaw - m->faceAngle[1];

    if (floorDYaw > -0x4000 && floorDYaw < 0x4000) {
        m->forwardVel = 16.0f;
        m->faceAngle[1] = m->floorYaw;
    } else {
        m->forwardVel = -16.0f;
        m->faceAngle[1] = m->floorYaw + 0x8000;
    }

    return set_mario_action(m, action, actionArg);
}

u32 mario_update_moving_sand(struct MarioState *m) {
    if (game_options_cheat_active(GO_IGNORE_SAND)) {
        return FALSE;
    }
    struct Surface *floor = m->floor;
    s32 floorType = floor->type;

    if (floorType == SURFACE_DEEP_MOVING_QUICKSAND || floorType == SURFACE_SHALLOW_MOVING_QUICKSAND
        || floorType == SURFACE_MOVING_QUICKSAND || floorType == SURFACE_INSTANT_MOVING_QUICKSAND) {
        s16 pushAngle = floor->force << 8;
        f32 pushSpeed = sMovingSandSpeeds[floor->force >> 8];

        m->vel[0] += pushSpeed * sins(pushAngle);
        m->vel[2] += pushSpeed * coss(pushAngle);

        return TRUE;
    }

    return FALSE;
}

u32 mario_update_windy_ground(struct MarioState *m) {
    if (game_options_cheat_active(GO_IGNORE_WIND)) {
        return FALSE;
    }
    struct Surface *floor = m->floor;

    if (!mario_has_current_metal_cap(m) && floor->type == SURFACE_HORIZONTAL_WIND) {
        f32 pushSpeed;
        s16 pushAngle = floor->force << 8;

        if (m->action & ACT_FLAG_MOVING) {
            s16 pushDYaw = m->faceAngle[1] - pushAngle;

            pushSpeed = m->forwardVel > 0.0f ? -m->forwardVel * 0.5f : -8.0f;

            if (pushDYaw > -0x4000 && pushDYaw < 0x4000) {
                pushSpeed *= -1.0f;
            }

            pushSpeed *= coss(pushDYaw);
        } else {
            pushSpeed = 3.2f + (gGlobalTimer % 4);
        }

        m->vel[0] += pushSpeed * sins(pushAngle);
        m->vel[2] += pushSpeed * coss(pushAngle);

        return TRUE;
    }

    return FALSE;
}

void stop_and_set_height_to_floor(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;

    mario_set_forward_vel(m, 0.0f);
    m->vel[1] = 0.0f;

    // HackerSM64 2.1: This check fixes the ledgegrab downwarp after being pushed off a ledge.
    if (m->pos[1] <= m->floorHeight + 160.0f) {
        m->pos[1] = m->floorHeight;
    }

    vec3f_copy(marioObj->header.gfx.pos, m->pos);
    vec3s_set(marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
}

s32 stationary_ground_step(struct MarioState *m) {
    struct Object *marioObj = m->marioObj;
    u32 stepResult = GROUND_STEP_NONE;

    mario_set_forward_vel(m, 0.0f);

    u32 takeStep = (mario_update_moving_sand(m) | mario_update_windy_ground(m));
    if (takeStep) {
        stepResult = perform_ground_step(m);
    } else {
        // HackerSM64 2.1: This check prevents the downwarps that plagued stationary actions.
        if (m->pos[1] <= m->floorHeight + 160.0f) {
            m->pos[1] = m->floorHeight;
        }

        vec3f_copy(marioObj->header.gfx.pos, m->pos);
        vec3s_set(marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);
    }

    return stepResult;
}

// The lower wall probe keeps Mario's center 24 units from a wall. A low
// platform can therefore push him back before his center reaches its floor.
// Look just across that wall for a supported top before ignoring the probe.
static s32 mario_can_step_past_lower_walls(struct MarioState *m, Vec3f nextPos,
                                           struct WallCollisionData *lowerWall) {
    s16 i;

    if (lowerWall->numWalls == 0) {
        return FALSE;
    }
    for (i = 0; i < lowerWall->numWalls; i++) {
        struct Surface *wall = lowerWall->walls[i];
        struct Surface *floor, *ceil;
        Vec3f previewPos;
        f32 wallTop = MAX(wall->vertex1[1], MAX(wall->vertex2[1], wall->vertex3[1]));
        f32 offset = wall->normal.x * nextPos[0] + wall->normal.z * nextPos[2] + wall->originOffset;
        f32 moveIntoWall =
            wall->normal.x * (nextPos[0] - m->pos[0]) + wall->normal.z * (nextPos[2] - m->pos[2]);

        if (ABS(wall->normal.y) > 0.01f || wallTop <= m->pos[1]
            || wallTop > m->pos[1] + MARIO_GROUND_LOW_STEP_MAX_HEIGHT || offset < -24.0f
            || offset > 24.0f || moveIntoWall >= 0.0f) {
            return FALSE;
        }
        vec3f_set(previewPos, nextPos[0] - wall->normal.x * (offset + 2.0f), nextPos[1],
                  nextPos[2] - wall->normal.z * (offset + 2.0f));
        f32 floorHeight = find_floor(previewPos[0], previewPos[1], previewPos[2], &floor);
        if (floor == NULL || floor->normal.y < COS73 || floorHeight <= m->pos[1]
            || floorHeight > m->pos[1] + MARIO_GROUND_LOW_STEP_MAX_HEIGHT
            || floorHeight < wallTop - 5.0f || floorHeight > wallTop + 5.0f
            || floorHeight + 160.0f >= find_mario_ceil(previewPos, floorHeight, &ceil)) {
            return FALSE;
        }
    }
    return TRUE;
}

static s32 perform_ground_quarter_step(struct MarioState *m, Vec3f nextPos, s32 checkCrossedWalls) {
    struct WallCollisionData lowerWall, upperWall;
    struct Surface *ceil, *floor;
    Vec3f intendedPos;

    s16 i;
    s16 wallDYaw;
    s32 oldWallDYaw;

    vec3f_copy(intendedPos, nextPos);
    mario_resolve_ground_step_walls(m, nextPos, checkCrossedWalls, FALSE, &lowerWall, &upperWall);
    if (checkCrossedWalls && upperWall.numWalls == 0
        && mario_can_step_past_lower_walls(m, intendedPos, &lowerWall)) {
        vec3f_copy(nextPos, intendedPos);
        mario_resolve_ground_step_walls(m, nextPos, checkCrossedWalls, TRUE, &lowerWall, &upperWall);
    }

    f32 floorHeight = find_floor(nextPos[0], nextPos[1], nextPos[2], &floor);
    f32 ceilHeight = find_mario_ceil(nextPos, floorHeight, &ceil);

    if (m->flags & (MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP)) {
        mario_update_water_walking_state(m, find_water_level(m->pos[0], m->pos[2]), ceilHeight);
    }

    if (floor == NULL) {
        return GROUND_STEP_HIT_WALL_STOP_QSTEPS;
    }

    if ((m->action & ACT_FLAG_RIDING_SHELL)
        || (m->flags & (MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP))) {
        f32 waterLevel = find_water_level(nextPos[0], nextPos[2]);
        if (!(m->action & ACT_FLAG_RIDING_SHELL)
            && !mario_can_walk_on_water(m, waterLevel, ceilHeight)) {
            waterLevel = floorHeight;
        }
        if (mario_is_riding_underwater_shell(m)) {
            if (mario_has_current_metal_cap(m) && !mario_has_current_vanish_cap(m)) {
                waterLevel = MIN(waterLevel, m->prevPos[1] + 60.0f);
                // The rising shell may only support Mario where his full
                // standing height fits below the ceiling.
                waterLevel = MIN(waterLevel, ceilHeight - 161.0f);
            } else {
                waterLevel = floorHeight;
            }
        }
        if (floorHeight < waterLevel) {
            floorHeight = waterLevel;
            floor = &gWaterSurfacePseudoFloor;
            floor->originOffset = -floorHeight;
        }
    }

    if (nextPos[1] > floorHeight + MARIO_GROUND_STEP_MAX_FLOOR_DELTA) {
        if (nextPos[1] + 160.0f >= ceilHeight) {
            return GROUND_STEP_HIT_WALL_STOP_QSTEPS;
        }

        vec3f_copy(m->pos, nextPos);
        set_mario_floor(m, floor, floorHeight);
        return GROUND_STEP_LEFT_GROUND;
    }

    // A tilted platform's side can enter the floor partition. Block upward
    // attachment in low poses, but let supported slides descend steep faces.
    // Handle real drops above so a steep floor below cannot prevent falling.
    if (mario_is_low_land_pose(m) && floor->normal.y < COS73 && floor->type != SURFACE_MARIO_ONLY_GRIPPY
        && (!mario_is_on_floor(m) || floorHeight > m->pos[1])) {
        return GROUND_STEP_HIT_WALL_STOP_QSTEPS;
    }

    if (floorHeight + 160.0f >= ceilHeight) {
        return GROUND_STEP_HIT_WALL_STOP_QSTEPS;
    }

    vec3f_set(m->pos, nextPos[0], floorHeight, nextPos[2]);

    set_mario_floor(m, floor, floorHeight);

    if (m->wall != NULL) {
        oldWallDYaw = abs_angle_diff(m->wallYaw, m->faceAngle[1]);
    } else {
        oldWallDYaw = 0x0;
    }
    for (i = 0; i < upperWall.numWalls; i++) {
        wallDYaw = abs_angle_diff(SURFACE_YAW(upperWall.walls[i]), m->faceAngle[1]);
        if (wallDYaw > oldWallDYaw || (m->action == ACT_GROUND_TWIRL && m->wall == NULL)) {
            oldWallDYaw = wallDYaw;
            set_mario_wall(m, upperWall.walls[i]);
        }

        if (m->action != ACT_GROUND_TWIRL && wallDYaw >= DEGREES(60) && wallDYaw <= DEGREES(120)) {
            continue;
        }

        return GROUND_STEP_HIT_WALL_CONTINUE_QSTEPS;
    }

    return GROUND_STEP_NONE;
}

static f32 get_mario_ground_vertical_travel(struct MarioState *m, f32 remaining) {
    return -(m->floor->normal.x * m->vel[0] + m->floor->normal.z * m->vel[2]) * remaining
           * mario_get_movement_time_scale(m);
}

static s32 get_mario_ground_air_collision_steps(f32 dx, f32 dy, f32 dz, s32 minSteps, s32 maxSteps) {
    f32 travelSquared = sqr(dx) + sqr(dy) + sqr(dz);
    s32 steps = minSteps;

    while (steps < maxSteps) {
        f32 coveredDistance = steps * MARIO_GROUND_AIR_COLLISION_STEP_DISTANCE;
        if (travelSquared <= sqr(coveredDistance)) {
            break;
        }
        steps++;
    }

    return steps;
}

// Ground movement is scaled by the current floor normal. Include its vertical
// rise so each check still covers at most the intended distance on a slope.
static s32 get_mario_ground_remaining_steps(struct MarioState *m, f32 remaining, s32 stepsDone,
                                            s32 currentSteps) {
    struct Surface *floor = m->floor;
    f32 timeScale = mario_get_movement_time_scale(m);
    f32 dx = floor->normal.y * m->vel[0] * remaining * timeScale;
    f32 dy = get_mario_ground_vertical_travel(m, remaining);
    f32 dz = floor->normal.y * m->vel[2] * remaining * timeScale;
    f32 travelSquared = sqr(dx) + sqr(dy) + sqr(dz);
    s32 minSteps = MAX(1, MARIO_GROUND_AIR_COLLISION_MIN_STEPS - stepsDone);
    s32 maxSteps = (game_options_cheat_active(GO_NO_SPEED_LIMIT) ? MARIO_UNLIMITED_COLLISION_MAX_STEPS
                                                                 : MARIO_GROUND_AIR_COLLISION_MAX_STEPS)
                   - stepsDone;
    s32 steps = CLAMP(currentSteps, minSteps, maxSteps);

    // When crossing many slope triangles, start near the previous count.
    while (steps > minSteps
           && travelSquared <= sqr((steps - 1) * MARIO_GROUND_AIR_COLLISION_STEP_DISTANCE)) {
        steps--;
    }
    while (steps < maxSteps && travelSquared > sqr(steps * MARIO_GROUND_AIR_COLLISION_STEP_DISTANCE)) {
        steps++;
    }
    return steps;
}

static s32 perform_ground_step_internal(struct MarioState *m, f32 fraction, s32 stopAtLedge) {
    s32 stepsDone = 0;
    s32 stepsRemaining;
    u32 stepResult = GROUND_STEP_NONE;
    Vec3f intendedPos;
    s32 checkCrossedWalls;
    f32 remaining = CLAMP(fraction, 0.0f, 1.0f);

    if (remaining == 0.0f) {
        return GROUND_STEP_NONE;
    }

    mario_limit_velocity(m);
    stepsRemaining =
        get_mario_ground_remaining_steps(m, remaining, stepsDone, MARIO_GROUND_AIR_COLLISION_MIN_STEPS);
    // The current floor bounds vertical travel until it changes. Allow for a
    // floor snap, then refresh the wall check after a floor transition.
    checkCrossedWalls = mario_wall_collision_begin(m, stepsRemaining, 30.0f, 60.0f,
                                                   get_mario_ground_vertical_travel(m, remaining),
                                                   MARIO_GROUND_STEP_MAX_FLOOR_DELTA);
    set_mario_wall(m, NULL);

    while (stepsRemaining > 0) {
        struct Surface *oldFloor = m->floor;
        f32 oldX = m->pos[0];
        f32 oldY = m->pos[1];
        f32 oldZ = m->pos[2];
        f32 oldFloorHeight = m->floorHeight;
        f32 stepFraction = remaining / (f32) stepsRemaining;
        f32 timeScale = mario_get_movement_time_scale(m);

        intendedPos[0] = oldX + oldFloor->normal.y * m->vel[0] * stepFraction * timeScale;
        intendedPos[2] = oldZ + oldFloor->normal.y * m->vel[2] * stepFraction * timeScale;
        intendedPos[1] = m->pos[1];

        stepResult = perform_ground_quarter_step(m, intendedPos, checkCrossedWalls);
        if (stopAtLedge && stepResult == GROUND_STEP_LEFT_GROUND) {
            // Keep every supported substep, but do not commit the one beyond
            // the lip. Crouch slides can then stop or grab from solid ground.
            vec3f_set(m->pos, oldX, oldY, oldZ);
            set_mario_floor(m, oldFloor, oldFloorHeight);
        }
        remaining -= stepFraction;
        stepsDone++;
        stepsRemaining--;
        if (m->wall != NULL) {
            checkCrossedWalls = TRUE;
        }
        if (stepResult == GROUND_STEP_LEFT_GROUND || stepResult == GROUND_STEP_HIT_WALL_STOP_QSTEPS) {
            break;
        }
        // A blocked step would ask the same wall and floor questions again.
        if (m->wall != NULL && m->floor == oldFloor
            && sqr(m->pos[0] - oldX) + sqr(m->pos[2] - oldZ) < 0.0001f
            && ABS(m->pos[1] - oldY) < 0.01f) {
            break;
        }
        // A new slope changes the distance covered by the remaining frame.
        if (m->floor != oldFloor && stepsRemaining > 0) {
            stepsRemaining = get_mario_ground_remaining_steps(m, remaining, stepsDone, stepsRemaining);
            if (!checkCrossedWalls) {
                checkCrossedWalls = mario_wall_collision_begin(
                    m, stepsRemaining, 30.0f, 60.0f, get_mario_ground_vertical_travel(m, remaining),
                    MARIO_GROUND_STEP_MAX_FLOOR_DELTA);
            }
        }
    }

    m->terrainSoundAddend = mario_get_terrain_sound_addend(m);
    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);

    if (stepResult == GROUND_STEP_HIT_WALL_CONTINUE_QSTEPS) {
        stepResult = GROUND_STEP_HIT_WALL;
    }
    return stepResult;
}

s32 perform_ground_step_with_fraction(struct MarioState *m, f32 fraction) {
    return perform_ground_step_internal(m, fraction, FALSE);
}

s32 perform_ground_step(struct MarioState *m) {
    return perform_ground_step_with_fraction(m, 1.0f);
}

s32 perform_ground_step_stop_at_ledge(struct MarioState *m) {
    return perform_ground_step_internal(m, 1.0f, TRUE);
}

// Horizontal dot product of surface normal
#define hdot_surf(surf, vec) (((surf)->normal.x * (vec)[0]) + ((surf)->normal.z * (vec)[2]))

struct Surface *check_ledge_grab(struct MarioState *m, struct Surface *prevWall, struct Surface *wall, Vec3f intendedPos, Vec3f nextPos, Vec3f ledgePos, struct Surface **ledgeFloor) {
    Vec3f candidatePos;
    struct Surface *candidateFloor;
    if (m->vel[1] > 0.0f || wall == NULL || !(m->input & INPUT_NONZERO_ANALOG)
        || abs_angle_diff(m->intendedYaw, m->faceAngle[1]) > 0x4000
        || sins(m->intendedYaw) * wall->normal.x + coss(m->intendedYaw) * wall->normal.z >= 0.0f) {
        return prevWall;
    }

    // Keep the entire winning contact together, including its floor and anchor.
    // Equal approach speeds retain the first candidate rather than oscillating.
    if (prevWall != NULL && hdot_surf(prevWall, m->vel) <= hdot_surf(wall, m->vel)) {
        return prevWall;
    }

    // Only ledge grab if the wall displaced Mario in the opposite direction of his velocity.
    // hdot(displacement, vel).
    if (
        ((nextPos[0] - intendedPos[0]) * m->vel[0]) + ((nextPos[2] - intendedPos[2]) * m->vel[2]) > 0.0f
    ) {
        return prevWall;
    }

    candidatePos[0] = nextPos[0] - (wall->normal.x * 60.0f);
    candidatePos[2] = nextPos[2] - (wall->normal.z * 60.0f);
    candidatePos[1] =
        find_floor(candidatePos[0], nextPos[1] + 160.0f, candidatePos[2], &candidateFloor);

    if (candidateFloor == NULL || candidatePos[1] < nextPos[1] + 100.0f
#ifdef DONT_LEDGE_GRAB_STEEP_SLOPES
        || candidateFloor->normal.y < COS25 // HACKERSM64_DO: check if floor is actually slippery
#endif
    ) {
        return prevWall;
    }
    vec3f_copy(ledgePos, candidatePos);
    *ledgeFloor = candidateFloor;
    return wall;
}

#undef hdot_surf

#define WIND_CATCH_ACCEL 8.0f
#define WIND_CATCH_MARGIN 128.0f
#define WIND_DEATH_CLEARANCE 1.0f
#define WIND_DEATH_GRACE_FRAMES 30

s32 mario_is_inside_wind_column(struct MarioState *m) {
    // The wind surface also acts as a death plane above its actual floor.
    // Entry grace can extend below that boundary, but never outside the column.
    return m->floor != NULL && m->floor->type == SURFACE_VERTICAL_WIND && m->pos[1] >= m->floorHeight
           && m->pos[1] < 500.0f;
}

void mario_update_wind_death_grace(struct MarioState *m) {
    if (game_options_cheat_active(GO_IGNORE_WIND)) {
        m->windColumnActive = FALSE;
        m->windDeathGraceTimer = 0;
        return;
    }
    s32 inside = mario_is_inside_wind_column(m);
    if (!inside) {
        m->windDeathGraceTimer = 0;
    } else if (!m->windColumnActive) {
        m->windDeathGraceTimer = WIND_DEATH_GRACE_FRAMES;
    } else if (m->windDeathGraceTimer != 0) {
        m->windDeathGraceTimer--;
    }
    m->windColumnActive = inside;
}

s32 mario_is_in_vertical_wind(struct MarioState *m) {
    return !game_options_cheat_active(GO_IGNORE_WIND) && !mario_has_current_metal_cap(m)
           && m->action != ACT_GROUND_POUND && m->floor != NULL
           && m->floor->type == SURFACE_VERTICAL_WIND && m->pos[1] >= m->floorHeight + 2048.0f
           && m->pos[1] < 500.0f;
}

static f32 wind_stopping_distance(f32 speed) {
    s32 updates = (s32) (speed / WIND_CATCH_ACCEL);
    if (updates * WIND_CATCH_ACCEL < speed) {
        updates++;
    }
    return updates * speed - 0.5f * WIND_CATCH_ACCEL * updates * (updates - 1);
}

static f32 wind_safe_downward_speed(f32 distance) {
    s32 updates = 1;
    distance = MAX(distance, 0.0f);
    while (distance > 0.5f * WIND_CATCH_ACCEL * updates * (updates + 1)) {
        updates++;
    }
    return (distance + 0.5f * WIND_CATCH_ACCEL * updates * (updates - 1)) / updates;
}

static void constrain_wind_descent(struct MarioState *m, Vec3f nextPos, struct Surface *floor,
                                   f32 floorHeight) {
    f32 clearance, speed, fraction;
    // A side entry below the real death boundary is still fatal. Only a
    // downward path starting in the live column receives catch protection.
    if (game_options_cheat_active(GO_IGNORE_WIND) || mario_has_current_metal_cap(m) || floor == NULL
        || floor->type != SURFACE_VERTICAL_WIND || m->action == ACT_GROUND_POUND || m->vel[1] >= 0.0f
        || m->pos[1] < floorHeight + 2048.0f || m->pos[1] >= 500.0f) {
        return;
    }
    clearance = m->pos[1] - (floorHeight + 2048.0f + WIND_DEATH_CLEARANCE);
    speed = -m->vel[1];
    if (clearance <= wind_stopping_distance(speed) + WIND_CATCH_MARGIN) {
        m->windCatchActive = TRUE;
        fraction = MAX(0.0f, (m->pos[1] - nextPos[1]) / speed);
        speed = MIN(speed, wind_safe_downward_speed(clearance));
        m->vel[1] = -speed;
        nextPos[1] = MAX(nextPos[1], m->pos[1] - fraction * speed);
    }
}

s32 bonk_or_hit_lava_wall(struct MarioState *m, struct WallCollisionData *wallData) {
    s16 i;
    s16 wallDYaw;
    s32 oldWallDYaw;
    s32 result = AIR_STEP_NONE;
    s32 backwardContact = FALSE;
    s32 twirlContact =
        m->action == ACT_L_AIR_TWIRL || (m->action == ACT_FLYING && m->airTwirlFlightRollFrame != 0);
    s16 approachYaw = m->faceAngle[1];

    if (mario_can_wall_kick_backward_jump(m)) {
        approachYaw += 0x8000;
    }

    if (m->wall != NULL) {
        oldWallDYaw = abs_angle_diff(m->wallYaw, approachYaw);
    } else {
        oldWallDYaw = 0x0;
    }

    for (i = 0; i < wallData->numWalls; i++) {
        if (wallData->walls[i] != NULL) {
            if (wallData->walls[i]->type == SURFACE_BURNING) {
                set_mario_wall(m, wallData->walls[i]);
                return AIR_STEP_HIT_LAVA_WALL;
            }

            if (m->forwardVel < 0.0f
                && m->vel[0] * wallData->walls[i]->normal.x + m->vel[2] * wallData->walls[i]->normal.z
                       < 0.0f) {
                backwardContact = TRUE;
            }

            // Update wall reference (bonked wall) only if the new wall has a better facing angle
            wallDYaw = abs_angle_diff(SURFACE_YAW(wallData->walls[i]), approachYaw);
            if (wallDYaw > oldWallDYaw || (twirlContact && m->wall == NULL)) {
                oldWallDYaw = wallDYaw;
                set_mario_wall(m, wallData->walls[i]);

                if (twirlContact || wallDYaw > DEGREES(180 - WALL_KICK_DEGREES)) {
                    m->flags |= MARIO_AIR_HIT_WALL;
                    result = AIR_STEP_HIT_WALL;
                }
            }
        }
    }

    // A backward hit often has no bonk result because Mario faces away from
    // the wall. Clear the speed here so held back input cannot accumulate it.
    // A valid backward-jump wall kick still needs its incoming speed first.
    if (backwardContact && !twirlContact
        && !(mario_can_wall_kick_backward_jump(m) && m->wall != NULL
             && abs_angle_diff(m->wallYaw, approachYaw) > DEGREES(180 - WALL_KICK_DEGREES))) {
        mario_set_forward_vel(m, 0.0f);
    }

    return result;
}

static s32 perform_air_step_ceiling_collision(struct MarioState *m, Vec3f nextPos, struct Surface *ceil,
                                              UNUSED u32 stepArg) {
    if (m->vel[1] >= 0.0f) {
        if (m->vel[1] > 0.0f) {
            mario_attack_cap_box_surface(m, ceil, INT_HIT_FROM_BELOW);
        }
        m->vel[1] = 0.0f;

#ifdef HANGING_FIX
        // Grab ceiling unless they just were grabbing a ceiling
        if (!(m->prevAction & ACT_FLAG_HANGING) && ceil != NULL && ceil->type == SURFACE_HANGABLE) {
#else
        if ((stepArg & AIR_STEP_CHECK_HANG) && ceil != NULL && ceil->type == SURFACE_HANGABLE) {
#endif
            return AIR_STEP_GRABBED_CEILING;
        }

        return AIR_STEP_NONE;
    }

    // A remembered floor only counts when this descent crosses it from above.
    if (nextPos[1] <= m->floorHeight && m->pos[1] >= m->floorHeight) {
        m->pos[1] = m->floorHeight;
        return AIR_STEP_LANDED;
    }

    m->pos[1] = nextPos[1];
    return AIR_STEP_HIT_CEILING;
}

// A wall probe 30 units above the feet can miss the last few units of a
// riser. Accept its low, walkable top only after verifying the actual side
// crossing and space beyond it. Thin fence tops must not become step support.
static s32 mario_can_step_onto_air_floor(struct MarioState *m, Vec3f nextPos, struct Surface *floor,
                                         f32 *height) {
    struct Surface *wall, *previewFloor, *ceil;
    Vec3f movement, contact, preview;
    if (floor == NULL || floor->normal.y < COS73) {
        return FALSE;
    }
    *height = get_surface_height_at_location(nextPos[0], nextPos[2], floor);
    f32 rise = *height - m->pos[1];
    if (rise <= 0.0f || rise > MARIO_GROUND_LOW_STEP_MAX_HEIGHT || nextPos[1] > *height) {
        return FALSE;
    }
    vec3f_diff(movement, nextPos, m->pos);
    find_surface_on_ray(m->pos, movement, &wall, contact, RAYCAST_FIND_WALL | RAYCAST_MARIO_WALL);
    if (wall == NULL || ABS(wall->normal.y) > 0.01f || wall->object != floor->object) {
        return FALSE;
    }
    f32 wallTop = MAX(wall->vertex1[1], MAX(wall->vertex2[1], wall->vertex3[1]));
    if (ABS(wallTop - *height) > 5.0f) {
        return FALSE;
    }
    vec3f_set(preview, contact[0] - wall->normal.x * 50.0f, *height,
              contact[2] - wall->normal.z * 50.0f);
    f32 previewHeight = find_floor(preview[0], preview[1], preview[2], &previewFloor);
    return previewFloor != NULL && previewFloor->normal.y >= COS73
           && previewFloor->object == floor->object && ABS(previewHeight - *height) <= 5.0f
           && *height + 160.0f < find_mario_ceil(nextPos, *height, &ceil);
}

s32 perform_air_quarter_step(struct MarioState *m, Vec3f intendedPos, u32 stepArg,
                             s32 checkCrossedWalls) {
    s16 i;
    s32 stepResult = AIR_STEP_NONE;

    Vec3f nextPos, ledgePos;
    struct WallCollisionData upperWall, lowerWall;
    struct Surface *ceil, *floor, *obstacle, *ledgeFloor = NULL;
    struct Surface *grabbedWall = NULL;

    vec3f_copy(nextPos, intendedPos);

    mario_resolve_air_step_walls(m, nextPos, checkCrossedWalls, &lowerWall, &upperWall);

    Vec3f floorStart;
    if (m->floor == &gWaterSurfacePseudoFloor) {
        // Synthetic water support stores its height in originOffset, not vertices.
        vec3f_copy(floorStart, m->pos);
    } else {
        get_air_step_floor_start(m->pos, m->floor, m->floorHeight, floorStart);
    }
    s16 floorQueryFlags = gCollisionFlags;
    if (intendedPos[1] < floorStart[1]
        && (nextPos[0] != intendedPos[0] || nextPos[2] != intendedPos[2])) {
        Vec3f topContact, checkedContact;
        struct Surface *topFloor;
        if (find_air_step_floor_contact(floorStart, intendedPos, topContact, &topFloor,
                                        floorQueryFlags)) {
            struct WallCollisionData contactLowerWall, contactUpperWall;
            vec3f_copy(checkedContact, topContact);
            // A later side push must not erase an earlier top crossing. Check
            // walls at that crossing too, so an intervening side still blocks it.
            mario_resolve_air_step_walls(m, checkedContact, checkCrossedWalls, &contactLowerWall,
                                         &contactUpperWall);
            if (checkedContact[0] == topContact[0] && checkedContact[2] == topContact[2]
                && contactLowerWall.numWalls == 0 && contactUpperWall.numWalls == 0) {
                vec3f_copy(nextPos, topContact);
                lowerWall = contactLowerWall;
                upperWall = contactUpperWall;
            } else if (topFloor->normal.x * (checkedContact[0] - topContact[0])
                               + topFloor->normal.z * (checkedContact[2] - topContact[2])
                           < -0.01f
                       && !mario_air_step_crosses_wall(m, topContact)) {
                // Radius pushes from nearby sides can throw a valid top landing
                // across a narrow slope. Keep the first contact when the approach
                // crosses no real wall, rather than committing that later push.
                vec3f_copy(nextPos, topContact);
                lowerWall.numWalls = 0;
                upperWall.numWalls = 0;
            }
        }
    }
    f32 floorHeight = find_floor_for_air_step_with_obstacle(floorStart, nextPos, &floor, &obstacle);
    f32 stepHeight;
    if (obstacle != NULL && mario_can_step_onto_air_floor(m, nextPos, obstacle, &stepHeight)) {
        floor = obstacle;
        floorHeight = stepHeight;
    } else if (obstacle != NULL && mario_resolve_air_step_floor_side(m, nextPos, &lowerWall)) {
        // Re-query the corrected footprint with the original one-shot filters.
        gCollisionFlags = floorQueryFlags;
        floorHeight = find_floor_for_air_step(floorStart, nextPos, &floor);
    }
    if (m->action != ACT_JUMP_KICK && mario_restrict_air_uphill(m, floor)
        && (nextPos[0] - m->pos[0]) * floor->normal.x + (nextPos[2] - m->pos[2]) * floor->normal.z
               < 0.0f) {
        // Enforce at the destination, before a landing can snap Mario uphill.
        // The next substep uses the clipped sideways/downhill velocity. Keep
        // this footprint rather than introducing an unchecked sideways push.
        nextPos[0] = m->pos[0];
        nextPos[2] = m->pos[2];
        floorHeight = find_floor_for_air_step(floorStart, nextPos, &floor);
    }
    Vec3f contactPos;
    struct Surface *contactFloor;
    if (find_air_step_floor_contact(floorStart, nextPos, contactPos, &contactFloor, floorQueryFlags)) {
        // A lower endpoint floor must not hide an earlier ridge crossing. Ignore
        // coplanar seams and small rounding differences to keep ordinary landing travel.
        if ((stepArg & AIR_STEP_CHECK_PRECISE_LANDING) || floor == NULL || nextPos[1] > floorHeight
            || (floor != contactFloor
                && floor->normal.x * (contactPos[0] - floor->vertex1[0])
                           + floor->normal.y * (contactPos[1] - floor->vertex1[1])
                           + floor->normal.z * (contactPos[2] - floor->vertex1[2])
                       > 0.01f)) {
            vec3f_copy(nextPos, contactPos);
            floor = contactFloor;
            floorHeight = contactPos[1];
        }
    }
    // Wall resolution can move the footprint into a different wind column.
    // Revalidate against that actual floor before committing downward travel.
    constrain_wind_descent(m, nextPos, floor, floorHeight);
    f32 ceilHeight = find_mario_ceil(nextPos, floorHeight, &ceil);

    if (m->flags & (MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP)) {
        mario_update_water_walking_state(m, find_water_level(m->pos[0], m->pos[2]), ceilHeight);
    }

    //! The water pseudo floor is not referenced when your intended qstep is
    // out of bounds, so it won't detect you as landing.

    if (floor == NULL) {
        // Do not catch a remembered ledge Mario has already fallen below.
        if (nextPos[1] <= m->floorHeight && m->pos[1] >= m->floorHeight) {
            m->pos[1] = m->floorHeight;
            return AIR_STEP_LANDED;
        }

        // Keep the blocked footprint, but still collide vertically with its ceiling.
        Vec3f blockedPos;
        vec3f_set(blockedPos, m->pos[0], nextPos[1], m->pos[2]);
        ceilHeight = find_mario_ceil(blockedPos, m->floorHeight, &ceil);
        if (blockedPos[1] + 160.0f > ceilHeight) {
            s32 ceilingResult = perform_air_step_ceiling_collision(m, blockedPos, ceil, stepArg);
            if (ceilingResult == AIR_STEP_NONE || ceilingResult == AIR_STEP_HIT_CEILING) {
                // The ceiling response still leaves horizontal travel blocked.
                return AIR_STEP_HIT_WALL;
            }
            return ceilingResult;
        }

        m->pos[1] = nextPos[1];
        return AIR_STEP_HIT_WALL;
    }

    if ((m->action & ACT_FLAG_RIDING_SHELL)
        || (m->flags & (MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP))) {
        f32 waterLevel = find_water_level(nextPos[0], nextPos[2]);
        if (!(m->action & ACT_FLAG_RIDING_SHELL)
            && !mario_can_walk_on_water(m, waterLevel, ceilHeight)) {
            waterLevel = floorHeight;
        }
        if (mario_is_riding_underwater_shell(m)) {
            if (m->action == ACT_RIDING_SHELL_JUMP && m->vel[1] > 0.0f) {
                waterLevel = floorHeight;
            } else if (mario_has_current_metal_cap(m) && !mario_has_current_vanish_cap(m)) {
                waterLevel = MIN(waterLevel, m->prevPos[1] + 60.0f);
                waterLevel = MIN(waterLevel, ceilHeight - 161.0f);
            } else {
                waterLevel = floorHeight;
            }
        }
        if (floorHeight < waterLevel) {
            floorHeight = waterLevel;
            floor = &gWaterSurfacePseudoFloor;
            floor->originOffset = -floorHeight;
        }
    }

    //! This check uses f32, but findFloor uses short (overflow jumps)
    if (nextPos[1] <= floorHeight) {
        if (ceilHeight - floorHeight > 160.0f) {
            m->pos[0] = nextPos[0];
            m->pos[2] = nextPos[2];
            set_mario_floor(m, floor, floorHeight);
            if (m->action == ACT_GROUND_POUND) {
                mario_attack_cap_box_surface(m, floor, INT_GROUND_POUND_OR_TWIRL);
            }
        }

        //! When ceilHeight - floorHeight <= 160, the step result says that
        // Mario landed, but his movement is cancelled and his referenced floor
        // isn't updated (pedro spots)
        m->pos[1] = floorHeight;
        if (m->action == ACT_JUMP_KICK) {
            // Brake against the contacted surface, never a floor below Mario.
            // Landing ends this air update, so this applies only once.
            mario_restrict_air_uphill(m, floor);
        }
        return AIR_STEP_LANDED;
    }

    if (nextPos[1] + 160.0f > ceilHeight) {
        return perform_air_step_ceiling_collision(m, nextPos, ceil, stepArg);
    }

    //! When the wall is not completely vertical or there is a slight wall
    // misalignment, you can activate these conditions in unexpected situations

    if ((stepArg & AIR_STEP_CHECK_LEDGE_GRAB) && upperWall.numWalls == 0 && lowerWall.numWalls != 0) {
        if (stepArg & AIR_STEP_CHECK_WALL_KICK) {
            // Test the real contact cone before committing a ledge anchor.
            // Oblique contacts still get their ordinary ledge opportunity.
            stepResult = bonk_or_hit_lava_wall(m, &lowerWall);
            if (stepResult != AIR_STEP_NONE) {
                vec3f_copy(m->pos, nextPos);
                set_mario_floor(m, floor, floorHeight);
                return stepResult == AIR_STEP_HIT_WALL ? AIR_STEP_HIT_WALL_STOP_QSTEPS : stepResult;
            }
        }
        for (i = 0; i < lowerWall.numWalls; i++) {
            grabbedWall = check_ledge_grab(m, grabbedWall, lowerWall.walls[i], intendedPos, nextPos, ledgePos, &ledgeFloor);
            if (grabbedWall != NULL) {
                stepResult = AIR_STEP_GRABBED_LEDGE;
            }
        }

        if (stepResult == AIR_STEP_GRABBED_LEDGE && grabbedWall != NULL && ledgeFloor != NULL) {
            vec3f_copy(m->pos, ledgePos);
            set_mario_floor(m, ledgeFloor, ledgePos[1]);
            m->faceAngle[0] = 0x0;
            m->faceAngle[1] = SURFACE_YAW(grabbedWall) + 0x8000;
        } else {
            vec3f_copy(m->pos, nextPos);
            set_mario_floor(m, floor, floorHeight);
        }
        return stepResult;
    }

    vec3f_copy(m->pos, nextPos);
    set_mario_floor(m, floor, floorHeight);

    if (upperWall.numWalls > 0) {
        stepResult  = bonk_or_hit_lava_wall(m, &upperWall);
        if (stepResult != AIR_STEP_NONE) {
            return stepResult;
        }
    }

    return (lowerWall.numWalls > 0) ? bonk_or_hit_lava_wall(m, &lowerWall) : AIR_STEP_NONE;
}

static f32 gravity_with_fall_drag(f32 verticalSpeed, f32 gravity) {
    // Keep the existing drag ceiling so HE THICC can continue accelerating
    // beyond Standard's limit instead of turning drag into upward acceleration.
    if (verticalSpeed < -FALL_DRAG_START_SPEED) {
        f32 excessSpeed = MIN(-verticalSpeed - FALL_DRAG_START_SPEED, 22.0f);
        gravity *= 1.0f - excessSpeed * FALL_DRAG_GRAVITY_REDUCTION_PER_UNIT;
    }
    return gravity;
}

// Metal/Vanish swimming uses land actions, so weight must also reach their gravity path.
static f32 water_gravity_scale(struct MarioState *m) {
    if (mario_character_palette() == MARIO_PALETTE_HE_THICC && mario_has_land_movement_underwater(m)
        && !(m->action & ACT_FLAG_INVULNERABLE) && m->pos[1] < m->waterLevel - 80.0f) {
        return HE_THICC_WATER_GRAVITY_SCALE;
    }
    return 1.0f;
}

void apply_twirl_gravity(struct MarioState *m) {
#ifdef Z_TWIRL
    f32 Zmodifier = m->input & INPUT_Z_DOWN ? 4.0f : 1.0f;
#endif
    f32 heaviness = 1.0f;
    f32 gravityScale = water_gravity_scale(m);
    f32 maxFallSpeed =
        game_options_cheat_active(GO_NO_SPEED_LIMIT) ? MARIO_SPEED_SAFETY_CAP : mario_max_fall_speed();

    if (m->angleVel[1] > 1024) {
        heaviness = 1024.0f / m->angleVel[1];
    }

#ifdef Z_TWIRL
    f32 terminalVelocity = -maxFallSpeed * heaviness * Zmodifier;
    m->vel[1] -= gravity_with_fall_drag(m->vel[1], 4.0f * heaviness * Zmodifier * gravityScale);
#else
    f32 terminalVelocity = -maxFallSpeed * heaviness;

    m->vel[1] -= gravity_with_fall_drag(m->vel[1], 4.0f * heaviness * gravityScale);
#endif
    if (m->vel[1] < terminalVelocity) {
        m->vel[1] = terminalVelocity;
    }
}

u32 should_strengthen_gravity_for_jump_ascent(struct MarioState *m) {
    if (!(m->flags & MARIO_JUMPING)) {
        return FALSE;
    }

    u32 jumpAction = m->action == ACT_L_AIR_TWIRL ? m->airTwirlSourceAction : m->action;
    if (m->action & (ACT_FLAG_INTANGIBLE | ACT_FLAG_INVULNERABLE)) {
        return FALSE;
    }

    if (!(m->input & INPUT_A_DOWN) && m->vel[1] > 20.0f) {
        if (m->action == ACT_JUMP_KICK && m->prevAction != ACT_LONG_JUMP) {
            // Kicking right after a jump must not bypass its short-hop gravity.
            return (m->prevAction & ACT_FLAG_CONTROL_JUMP_HEIGHT) != 0;
        }
        return (jumpAction & ACT_FLAG_CONTROL_JUMP_HEIGHT) != 0;
    }

    return FALSE;
}

void apply_gravity(struct MarioState *m) {
    f32 beforeGravity = m->vel[1];
    // Death-return warps keep their native descent instead of gameplay's faster fall.
    f32 maxFallSpeed = m->action == ACT_FALLING_DEATH_EXIT ? 75.0f : mario_max_fall_speed();
    if (game_options_cheat_active(GO_NO_SPEED_LIMIT) && !(m->action & ACT_FLAG_INTANGIBLE)) {
        maxFallSpeed = MARIO_SPEED_SAFETY_CAP;
    }
    f32 timeScale = mario_get_movement_time_scale(m);
    if (timeScale < 1.0f) {
        maxFallSpeed = MAX(maxFallSpeed, METAL_WATER_MAX_FALL_SPEED / timeScale);
    }
    f32 gravityScale = water_gravity_scale(m) * mario_air_twirl_gravity_scale(m);
    u32 gravityAction = m->action == ACT_L_AIR_TWIRL ? m->airTwirlSourceAction : m->action;

    if (m->action == ACT_TWIRLING && m->vel[1] < 0.0f) {
        apply_twirl_gravity(m);
    } else if (m->action == ACT_SHOT_FROM_CANNON) {
        m->vel[1] -= gravity_with_fall_drag(m->vel[1], 1.0f * gravityScale);
        if (m->vel[1] < -maxFallSpeed) {
            m->vel[1] = -maxFallSpeed;
        }
    } else if (gravityAction == ACT_LONG_JUMP || gravityAction == ACT_SLIDE_KICK
               || gravityAction == ACT_BBH_ENTER_SPIN) {
        m->vel[1] -= gravity_with_fall_drag(m->vel[1],
                                            (m->action == ACT_SLIDE_KICK ? 2.4f : 2.0f) * gravityScale);
        if (m->vel[1] < -maxFallSpeed) {
            m->vel[1] = -maxFallSpeed;
        }
    } else if (m->action == ACT_LAVA_BOOST || m->action == ACT_FALL_AFTER_STAR_GRAB) {
        m->vel[1] -= 3.2f * gravityScale;
        if (m->vel[1] < -65.0f
            && (!game_options_cheat_active(GO_NO_SPEED_LIMIT)
                || m->action == ACT_FALL_AFTER_STAR_GRAB)) {
            m->vel[1] = -65.0f;
        }
    } else if (m->action == ACT_GETTING_BLOWN) {
        m->vel[1] -= gravity_with_fall_drag(m->vel[1], m->windGravity * gravityScale);
        if (m->vel[1] < -maxFallSpeed) {
            m->vel[1] = -maxFallSpeed;
        }
    } else if (should_strengthen_gravity_for_jump_ascent(m)) {
        u32 jumpAction = m->action == ACT_JUMP_KICK ? m->prevAction : gravityAction;
        if (jumpAction == ACT_JUMP || jumpAction == ACT_DOUBLE_JUMP || jumpAction == ACT_TRIPLE_JUMP
            || jumpAction == ACT_HOLD_JUMP) {
            // Keep variable height without cutting upward speed in one frame.
            m->vel[1] -= JUMP_RELEASE_UPWARD_DECEL * gravityScale;
        } else {
            m->vel[1] /= 4.0f;
        }
    } else if (m->action & ACT_FLAG_METAL_WATER) {
        m->vel[1] -= 1.6f * gravityScale;
        if (m->vel[1] < -maxFallSpeed) {
            m->vel[1] = -maxFallSpeed;
        }
    } else if ((m->flags & MARIO_WING_CAP) && m->vel[1] < 0.0f && (m->input & INPUT_A_DOWN)) {
        m->marioBodyState->wingFlutter = TRUE;

        m->vel[1] -= 2.0f * gravityScale;
        if (m->vel[1] < -37.5f) {
            if ((m->vel[1] += 4.0f) > -37.5f) {
                m->vel[1] = -37.5f;
            }
        }
    } else {
        m->vel[1] -= gravity_with_fall_drag(m->vel[1], 4.0f * gravityScale);
        if (m->vel[1] < -maxFallSpeed) {
            m->vel[1] = -maxFallSpeed;
        }
    }
    // Each action keeps its own gravity and short-hop rule, integrated over
    // the same slower interval as movement. Collision stops are never eased.
    if (timeScale < 1.0f) {
        m->vel[1] = beforeGravity + (m->vel[1] - beforeGravity) * timeScale;
        limit_metal_water_fall_speed(m, timeScale);
    }
}

void apply_vertical_wind(struct MarioState *m, f32 beforeGravity) {
    f32 maxVelY;

    if (mario_is_in_vertical_wind(m)) {
        f32 offsetY = m->pos[1] - -1500.0f;

        {
            if (offsetY >= 0.0f) {
                maxVelY = 10000.0f / (offsetY + 200.0f);
            } else {
                maxVelY = 50.0f;
            }

            if (m->vel[1] < maxVelY) {
                if ((m->vel[1] += maxVelY / 8.0f) > maxVelY) {
                    m->vel[1] = maxVelY;
                }
            }
            if (m->windCatchActive) {
                m->vel[1] = MAX(m->vel[1], MIN(maxVelY, beforeGravity + WIND_CATCH_ACCEL));
                if (m->vel[1] >= 0.0f) {
                    m->windCatchActive = FALSE;
                }
            }
        }
    } else {
        m->windCatchActive = FALSE;
    }
}

static s32 perform_air_step_internal(struct MarioState *m, u32 stepArg, f32 *remaining, f32 fraction,
                                     s32 applyForces, f32 *waterContact) {
    Vec3f intendedPos;
    s32 i;
    s32 numSteps;
    s32 quarterStepResult;
    s32 stepResult = AIR_STEP_NONE;
    s32 checkCrossedWalls;
    f32 beforeGravity;
    f32 impactVelY;
    f32 timeScale = mario_get_movement_time_scale(m);
    f32 stepScale = timeScale * fraction;

    if (remaining != NULL) {
        *remaining = 0.0f;
    }
    if (waterContact != NULL) {
        *waterContact = FLOOR_LOWER_LIMIT;
    }

    mario_limit_velocity(m);
    // Also limit direct action velocities, such as a ground pound, before
    // collision movement rather than waiting until the following frame.
    limit_metal_water_fall_speed(m, timeScale);
    impactVelY = m->vel[1] * timeScale;
    numSteps = get_mario_ground_air_collision_steps(
        m->vel[0] * stepScale, m->vel[1] * stepScale, m->vel[2] * stepScale,
        MARIO_GROUND_AIR_COLLISION_MIN_STEPS,
        game_options_cheat_active(GO_NO_SPEED_LIMIT) ? MARIO_UNLIMITED_COLLISION_MAX_STEPS
                                                     : MARIO_GROUND_AIR_COLLISION_MAX_STEPS);
    checkCrossedWalls =
        mario_wall_collision_begin(m, numSteps, 30.0f, 150.0f, m->vel[1] * stepScale, 0.0f);
    set_mario_wall(m, NULL);

    for (i = 0; i < numSteps; i++) {
        f32 oldX = m->pos[0];
        f32 oldY = m->pos[1];
        f32 oldZ = m->pos[2];
        f32 oldVelY = m->vel[1];

        intendedPos[0] = m->pos[0] + m->vel[0] * stepScale / (f32) numSteps;
        intendedPos[1] = m->pos[1] + m->vel[1] * stepScale / (f32) numSteps;
        intendedPos[2] = m->pos[2] + m->vel[2] * stepScale / (f32) numSteps;

        // Slide-kick skips stop at the liquid crossing, before spending rebound time.
        f32 waterLevel = FLOOR_LOWER_LIMIT;
        f32 waterFraction = 1.0f;
        if (waterContact != NULL && intendedPos[1] < oldY) {
            waterLevel = find_water_level(intendedPos[0], intendedPos[2]);
            if (oldY >= waterLevel && intendedPos[1] <= waterLevel) {
                waterFraction = (oldY - waterLevel) / (oldY - intendedPos[1]);
                Vec3f crossing = {
                    oldX + (intendedPos[0] - oldX) * waterFraction,
                    waterLevel,
                    oldZ + (intendedPos[2] - oldZ) * waterFraction,
                };
                struct Surface *waterFloor;
                s16 queryFlags = gCollisionFlags;
                f32 waterFloorHeight = find_floor(crossing[0], crossing[1], crossing[2], &waterFloor);
                gCollisionFlags = queryFlags;
                if (waterFloor != NULL && waterLevel > waterFloorHeight
                    && find_water_level(crossing[0], crossing[2]) == waterLevel) {
                    vec3f_copy(intendedPos, crossing);
                } else {
                    waterLevel = FLOOR_LOWER_LIMIT;
                }
            } else {
                waterLevel = FLOOR_LOWER_LIMIT;
            }
        }
        quarterStepResult = perform_air_quarter_step(m, intendedPos, stepArg, checkCrossedWalls);
        if (waterLevel != FLOOR_LOWER_LIMIT && quarterStepResult == AIR_STEP_NONE
            && stepResult == AIR_STEP_NONE && ABS(m->pos[1] - waterLevel) < 0.01f
            && m->floorHeight < waterLevel) {
            *waterContact = waterLevel;
            if (remaining != NULL) {
                *remaining = (numSteps - i - waterFraction) / (f32) numSteps;
            }
            break;
        }
        if (m->wall != NULL) {
            checkCrossedWalls = TRUE;
        }

        if (quarterStepResult == AIR_STEP_HIT_WALL_STOP_QSTEPS) {
            // A buffered kick won a lower-wall/ledge conflict. Keep that contact
            // without shortening ordinary wall hits that have no competing grab.
            stepResult = AIR_STEP_HIT_WALL;
            break;
        }
        if (quarterStepResult != AIR_STEP_NONE) {
            stepResult = quarterStepResult;
        }

        if (quarterStepResult == AIR_STEP_LANDED || quarterStepResult == AIR_STEP_GRABBED_LEDGE
            || quarterStepResult == AIR_STEP_GRABBED_CEILING
            || quarterStepResult == AIR_STEP_HIT_LAVA_WALL) {
            if (quarterStepResult == AIR_STEP_LANDED && remaining != NULL) {
                f32 consumed = 1.0f;
                if ((stepArg & AIR_STEP_CHECK_PRECISE_LANDING) && oldVelY < 0.0f) {
                    consumed =
                        CLAMP((oldY - m->pos[1]) / (-oldVelY * stepScale / (f32) numSteps), 0.0f, 1.0f);
                }
                *remaining = (numSteps - i - consumed) / (f32) numSteps;
            }
            break;
        }
        // With no vertical travel, a blocked wall step would repeat unchanged.
        if ((m->wall != NULL || quarterStepResult == AIR_STEP_HIT_WALL) && oldVelY == 0.0f
            && m->vel[1] == 0.0f && sqr(m->pos[0] - oldX) + sqr(m->pos[2] - oldZ) < 0.0001f
            && ABS(m->pos[1] - oldY) < 0.01f) {
            break;
        }
    }

    if (stepResult == AIR_STEP_LANDED) {
        player_stats_ground_contact();
        m->airTwirlUsed = FALSE;
        m->airTwirlWingControlTimer = 0;
        m->airTwirlWingResetUsed = FALSE;
        if (impactVelY <= -40.0f && m->action != ACT_GROUND_POUND) {
            mario_he_thicc_contact_shake(m);
        }
        m->wallKickChainCount = 0;
        if (m->spawnFallProtection == MARIO_SPAWN_FALL_PROTECTION_ACTIVE) {
            m->spawnFallProtection = MARIO_SPAWN_FALL_PROTECTION_LANDED;
        }
    }

    if (m->vel[1] >= 0.0f) {
        m->peakHeight = m->pos[1];
    }

    m->terrainSoundAddend = mario_get_terrain_sound_addend(m);

    beforeGravity = m->vel[1];
    if (applyForces && m->action != ACT_FLYING) {
        apply_gravity(m);
    }
    if (stepResult == AIR_STEP_LANDED || stepResult == AIR_STEP_GRABBED_LEDGE
        || stepResult == AIR_STEP_GRABBED_CEILING || stepResult == AIR_STEP_HIT_CEILING) {
        m->windCatchActive = FALSE;
    }
    if (applyForces) {
        apply_vertical_wind(m, beforeGravity);
    }
    mario_limit_velocity(m);

    vec3f_copy(m->marioObj->header.gfx.pos, m->pos);
    vec3s_set(m->marioObj->header.gfx.angle, 0, m->faceAngle[1], 0);

    castle_secret_glass_check_air(m);
    return stepResult;
}

s32 perform_air_step_with_remainder(struct MarioState *m, u32 stepArg, f32 *remaining) {
    return perform_air_step_internal(m, stepArg, remaining, 1.0f, TRUE, NULL);
}

// Rebounds use the exact contact fraction; ordinary land actions keep substep timing.
s32 perform_slide_kick_air_step(struct MarioState *m, f32 *remaining, s32 checkWater, f32 *waterLevel) {
    *waterLevel = FLOOR_LOWER_LIMIT;
    return perform_air_step_internal(m, AIR_STEP_CHECK_PRECISE_LANDING, remaining, 1.0f, TRUE,
                                     checkWater ? waterLevel : NULL);
}

s32 perform_air_step_with_fraction(struct MarioState *m, u32 stepArg, f32 fraction) {
    if (fraction <= 0.0f) {
        return AIR_STEP_NONE;
    }
    return perform_air_step_internal(m, stepArg, NULL, MIN(fraction, 1.0f), FALSE, NULL);
}

s32 perform_air_step(struct MarioState *m, u32 stepArg) {
    return perform_air_step_with_remainder(m, stepArg, NULL);
}

// They had these functions the whole time and never used them? Lol

void set_vel_from_pitch_and_yaw(struct MarioState *m) {
    m->vel[0] = m->forwardVel * coss(m->faceAngle[0]) * sins(m->faceAngle[1]);
    m->vel[1] = m->forwardVel * sins(m->faceAngle[0]);
    m->vel[2] = m->forwardVel * coss(m->faceAngle[0]) * coss(m->faceAngle[1]);
}

void set_vel_from_yaw(struct MarioState *m) {
    m->vel[0] = m->slideVelX = m->forwardVel * sins(m->faceAngle[1]);
    m->vel[1] = 0.0f;
    m->vel[2] = m->slideVelZ = m->forwardVel * coss(m->faceAngle[1]);
}
