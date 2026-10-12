/* Real ground/air steps, gravity, ledge and wall-window functions. Rock queries
 * use the current mesh; other queries and action/animation services are doubles.
 * No level loading, emulator or performance measurement. */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
typedef uint8_t u8;
typedef int16_t s16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
typedef f32 Vec3f[3];
typedef s16 Vec3s[3];
#define TRUE 1
#define FALSE 0
#define UNUSED
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) MIN(MAX(x, lo), hi)
#define ABS(x) fabsf(x)
#define sqr(x) ((x) * (x))
#define DEGREES(x) ((s16) ((x) * 65536.0f / 360.0f))
#define SOUND_ACTION_TWIRL 16
#define SOUND_ACTION_FLYING_FAST 17
#define SOUND_MARIO_YAHOO_WAHA_YIPPEE 18
#define SOUND_MOVING_FLYING 19
#define A_BUTTON 0x8000
#define L_TRIG 0x40
#define Z_TRIG 0x2000
#define SOUND_ACTION_BONK 1
#define SOUND_ACTION_METAL_BONK 2
#define SOUND_ACTION_HIT 3
#define SOUND_ACTION_HANGING_STEP 15
#define SOUND_MARIO_WAAAOOOW 4
#define SOUND_ACTION_TERRAIN_JUMP 5
#define SOUND_MARIO_YAHOO 6
#define SOUND_MARIO_HERE_WE_GO 7
#define SOUND_MOVING_TERRAIN_SLIDE 8
#define SOUND_MARIO_HOOHOO 9
#define SOUND_ACTION_SPIN 10
#define SOUND_ACTION_TERRAIN_LANDING 11
#define SOUND_MARIO_PUNCH_HOO 13
#define SOUND_ACTION_WATER_JUMP 14
#define SOUND_MARIO_YAH_WAH_HOO 12
#define WARP_OP_WARP_FLOOR 1
#include "test_support.h"
#include "geometry_definitions.inc"
static int sPalette;
static int mario_character_palette(void) {
    return sPalette;
}

struct Surface {
    struct {
        f32 x, y, z;
    } normal;
    s32 type, force;
    f32 originOffset;
    s16 vertex1[3], vertex2[3], vertex3[3];
};
struct Object {
    struct {
        struct {
            Vec3f pos, cameraToObject;
            Vec3s angle;
            struct {
                s32 animID, animFrame;
            } animInfo;
        } gfx;
    } header;
    s32 oMarioLongJumpIsSlow, oMarioSteepJumpYaw;
};
struct Controller {
    f32 stickMag, stickX, stickY;
    u32 buttonDown, buttonPressed;
};
struct Camera {
    s32 mode, defMode;
};
struct Area {
    s32 terrainType;
    struct Camera *camera;
};
struct MarioBodyState {
    s32 wingFlutter, punchState;
};
struct PlayerCameraState {
    s32 cameraEvent;
};
struct MarioState;
void mario_set_wall_bonk_momentum(struct MarioState *m, f32 recoilSpeed, f32 minimumSpeed);
struct MarioState {
    u8 zBPressed;
    u8 framesSinceB;
    u8 landingJumpBufferedEdge;
    u32 bullyKnockbackBuffer;
    u8 objectRecoveryAFrames, objectRecoveryBFrames;
    u8 automaticAFrames, automaticBFrames, automaticZFrames, groundPoundInputFrames;

    u8 slideKickStartupFrames;
    u8 rolloutFrames;
    u8 waterWalkTwirl;
    u32 airTwirlSourceAction;
    s16 airTwirlStartAngle, airTwirlWallYaw, airTwirlCooldown;
    u8 airTwirlCooldownResetByCoin;
    u8 airTwirlInputFrames;
    u8 airTwirlJumpQueued;
    u8 airTwirlLPressed;
    u8 airTwirlFlightRollFrame;
    u8 airTwirlUsed, airTwirlWingResetUsed, airTwirlGravity, airTwirlContinue, airTwirlWallContact;
    u8 airTwirlWallBounceCount;
    u8 waterWalkCooldown;
    u8 waterWalkDisabled;
    u8 airTwirlWingControlTimer, wallTwirlSoundTimer;
    f32 airTwirlWingSpeedLimit;
    s32 wallKickFromDoubleJump;
    f32 quicksandDepth;
    int wallBonkMomentum, heaveHoThrowPendingLanding, heaveHoThrowRecoveryPending, hurtCounter;
    s32 backwardSlopeSlideCount, backwardButtSlideJumpCount;
    u8 windColumnActive, windDeathGraceTimer;
    f32 slopeJumpKickImpactSpeed;
    Vec3f pos, prevPos, vel, wallKickContactNormal;
    Vec3s faceAngle, angleVel;
    s16 intendedYaw, wallYaw, slideYaw, wallKickContactYaw;
    f32 floorHeight, ceilHeight, forwardVel, slideVelX, slideVelZ, peakHeight, windGravity,
        wallKickStartForwardVel, waterLevel;
    f32 intendedMag;
    u32 action, prevAction, actionArg, actionState, actionTimer, input, flags, particleFlags;
    u32 wallKickContactFrame;
    s32 windCatchActive;
    s32 wallKickTimer, wallKickChainCount, wallKickContactSoundPlayed, spawnFallProtection,
        terrainSoundAddend, health, framesSinceA;
    s32 landingJumpInputFrames, wallKickInputFrames, wallKickForwardStickFrames,
        wallKickBackwardStickFrames;
    struct Object *marioObj, *heldObj, *riddenObj;
    struct Surface *floor, *ceil, *wall;
    struct Controller *controller;
    struct MarioBodyState *marioBodyState;
    struct Area *area;
    struct PlayerCameraState *statusForCamera;
};
struct WallCollisionData {
    f32 x, y, z, radius, offsetY;
    s32 numWalls;
    struct Surface *walls[4];
};
static struct Surface ground, topA, topB, wallA, wallB, ceiling, gWaterSurfacePseudoFloor;
static struct Object marioObject;
static struct Controller controller;
static struct MarioBodyState bodyState;
static s32 mode, warps, bonks, twirlSounds, metalBonks, kickImpacts, poseBegins, stepCalls, animation,
    edgeWallEnabled;
static u32 gGlobalTimer;
static u32 activeBonk;
static s32 bonkOverlaps, bonkStopCalls;
static f32 *activeBonkSource;
static s32 slideStops, slideUpdateCalls;
static s32 gSpecialTripleJump;
static s32 stepFloorEnabled;
static f32 ceilingY;
static s32 ceilingAtBlockedFootprint;
static f32 sShellWaterLevel;
static s32 sRidingUnderwaterShell;
static struct Area sArea;
static struct Camera sCamera;
static u32 gAudioRandom;
static s32 sFlightTextCalls;
static s32 gCurrLevelNum, gCurrAreaIndex, sSlideBounceEffects, sSlideLandingSounds;
static struct Surface rockFaces[20];
static Vec3f rockWorldVertices[12];
static f32 rock_floor(f32 x, f32 y, f32 z, struct Surface **floor);
static f32 sins(s16 a) {
    return sinf(a * 6.28318530718f / 65536);
}
static f32 coss(s16 a) {
    return cosf(a * 6.28318530718f / 65536);
}
static s16 atan2s(f32 z, f32 x) {
    return (s16) (atan2f(x, z) * 65536 / 6.28318530718f);
}
#define SURFACE_YAW(s) atan2s((s)->normal.z, (s)->normal.x)
static s16 abss(s16 a) {
    return a < 0 ? -a : a;
}
static void vec3f_copy(Vec3f a, Vec3f b) {
    memcpy(a, b, sizeof(Vec3f));
}
static void vec3s_set(Vec3s a, s16 x, s16 y, s16 z) {
    a[0] = x;
    a[1] = y;
    a[2] = z;
}
static void set_mario_floor(struct MarioState *m, struct Surface *s, f32 y) {
    m->floor = s;
    m->floorHeight = y;
}
static void set_mario_ceil(struct MarioState *m, struct Surface *s, f32 y) {
    m->ceil = s;
    m->ceilHeight = y;
}
static Vec3f gVec3fZero;
#define approach_angle approach_s16_symmetric
static void set_mario_wall(struct MarioState *m, struct Surface *s) {
    m->wall = s;
    m->wallYaw = s ? SURFACE_YAW(s) : 0;
}
static s32 ledgeAnchorVariant, ledgeWallOrder;
static f32 ledgeAnchorHeight;
static f32 find_floor(f32 x, f32 y, f32 z, struct Surface **f) {
    if (mode == 17) {
        *f = &ground;
        return -(ground.normal.x * x + ground.normal.z * z + ground.originOffset) / ground.normal.y;
    }
    if (mode == 16) {
        if ((s32) z >= 0 && (s32) z <= 1 && y + FIND_FLOOR_BUFFER >= topA.originOffset) {
            *f = &topA;
            return topA.originOffset;
        }
        *f = &ground;
        return ground.originOffset;
    }
    if (mode == 14) {
        if (z <= 1.0f) {
            *f = &ground;
            return 0.0f;
        }
        if (topA.originOffset <= (s32) y + FIND_FLOOR_BUFFER) {
            *f = &topA;
            return topA.originOffset;
        }
        *f = NULL;
        return FLOOR_LOWER_LIMIT;
    }
    if (mode == 12) {
        if (z >= 40 && z <= 50) {
            *f = &ground;
            return 0;
        }
        if (z > 50 || ledgeAnchorVariant == 0) {
            *f = &topB;
            return -300;
        }
        *f = ledgeAnchorVariant == 1 ? NULL : &topA;
        return ledgeAnchorHeight;
    }
    if (mode == 13) {
        if (x < 40) {
            *f = &topA;
            return -12;
        }
        if (z < 40 && ledgeAnchorVariant) {
            *f = &topB;
            return 10;
        }
        *f = &ground;
        return -300;
    }
    if (mode == 11) {
        *f = x > 0 ? &topA : &ground;
        return x > 0 ? x * 4.0f / 3.0f : 0;
    }
    if (mode == 6) {
        return rock_floor(x, y, z, f);
    }
    if (mode == 7) {
        *f = NULL;
        return -10000;
    }
    if (mode == 10 && stepFloorEnabled && z <= 0) {
        *f = &topA;
        return topA.originOffset;
    }
    if (mode == 9) {
        *f = z <= 50 ? &ground : &topB;
        return z <= 50 ? 0 : topB.originOffset;
    }
    if (mode == 5) {
        *f = &topA;
        return topA.originOffset;
    }
    if (mode >= 1 && mode <= 3 && y >= 150) {
        if (z > 30) {
            if (mode == 3) {
                *f = NULL;
                return -10000;
            }
            *f = &topB;
            return 180;
        }
        *f = &topA;
        return 150;
    }
    *f = &ground;
    return ground.originOffset;
}
// The native rounded support correction runs in the swimming collision fixture.
static void get_air_step_floor_start(const Vec3f pos, struct Surface *floor, f32 height, Vec3f start) {
    memcpy(start, pos, sizeof(Vec3f));
}
static f32 find_floor_for_air_step(const Vec3f start, const Vec3f next, struct Surface **floor) {
    f32 height = find_floor(next[0], next[1], next[2], floor);
    if (mode == 16 && *floor == &topA && height > start[1]) {
        *floor = &ground;
        return ground.originOffset;
    }
    return height;
}
// Native riser detection runs in swimming; this suite supplies collision services.
static f32 find_floor_for_air_step_with_obstacle(const Vec3f start, const Vec3f next,
                                                 struct Surface **floor, struct Surface **obstacle) {
    *obstacle = NULL;
    return find_floor_for_air_step(start, next, floor);
}
static s32 mario_can_step_onto_air_floor(struct MarioState *m, Vec3f next, struct Surface *floor,
                                         f32 *height) {
    assert(floor == NULL);
    return FALSE;
}

static s16 gCollisionFlags;
// Enable analytic floor crossings for the rebound travel fixture only.
static s32 sFlatFloorContact;
static struct Surface *sCrossedFloor;
static Vec3f sFloorContact;
static s32 find_air_step_floor_contact(const Vec3f start, const Vec3f next, Vec3f contact,
                                       struct Surface **floor, s16 flags) {
    if (sFlatFloorContact && start[1] > ground.originOffset && next[1] <= ground.originOffset) {
        f32 fraction = (start[1] - ground.originOffset) / (start[1] - next[1]);
        for (int axis = 0; axis < 3; axis++) {
            contact[axis] = start[axis] + (next[axis] - start[axis]) * fraction;
        }
        *floor = &ground;
        return TRUE;
    }
    *floor = sCrossedFloor;
    if (*floor != NULL) {
        vec3f_copy(contact, sFloorContact);
    }
    return *floor != NULL;
}

static f32 find_mario_ceil(Vec3f p, f32 y, struct Surface **c) {
    if (ceilingAtBlockedFootprint && p[2] > 1.0f) {
        *c = NULL;
        return 10000.0f;
    }
    *c = ceilingY >= MAX(p[1], y) + 3 ? &ceiling : NULL;
    return *c ? ceilingY : 10000;
}
static f32 find_water_level(f32 x, f32 z) {
    return sShellWaterLevel;
}
static void resolve_and_return_wall_collisions(Vec3f p, f32 offset, f32 radius,
                                               struct WallCollisionData *walls) {
    walls->numWalls = 0;
    if (mode == 15 && p[2] >= 20.0f) {
        p[2] = 20.0f;
        walls->walls[0] = &wallB;
        walls->numWalls = 1;
    }
}
static void mario_resolve_air_step_walls(struct MarioState *m, Vec3f p, s32 cross,
                                         struct WallCollisionData *lo, struct WallCollisionData *hi) {
    lo->numWalls = hi->numWalls = 0;
    if (mode == 15 && p[2] >= 20.0f) {
        p[2] = 20.0f;
        lo->walls[0] = &wallB;
        lo->numWalls = 1;
    }
    if (mode >= 1 && mode <= 3) {
        lo->walls[0] = &wallA;
        lo->numWalls = 1;
        if (mode >= 2) {
            lo->walls[1] = &wallB;
            lo->numWalls = 2;
        }
        p[0] = p[2] = 0;
    }
    if (mode == 4) {
        hi->walls[0] = &wallB;
        hi->numWalls = 1;
        p[2] = 0;
    }
}
// Finite wall sweeps are supplied here; real mesh sweeps have separate coverage.
static s32 mario_air_step_crosses_wall(struct MarioState *m, Vec3f next) {
    return FALSE;
}
static s32 mario_resolve_air_step_floor_side(struct MarioState *m, Vec3f next,
                                             struct WallCollisionData *lower) {
    return FALSE;
}
static void vec3f_set(Vec3f p, f32 x, f32 y, f32 z) {
    p[0] = x;
    p[1] = y;
    p[2] = z;
}
s32 mario_is_low_land_pose(struct MarioState *m);
s32 mario_restrict_air_uphill(struct MarioState *m, struct Surface *floor);
static void mario_resolve_ground_step_walls(struct MarioState *m, Vec3f p, s32 crossed, s32 skipLower,
                                            struct WallCollisionData *lo,
                                            struct WallCollisionData *hi) {
    lo->numWalls = hi->numWalls = 0;
    if (mode == 8) {
        p[0] = m->pos[0];
        hi->walls[0] = &wallA;
        hi->numWalls = 1;
    }
    if (mode == 10 && crossed && p[2] < 50 && p[2] > -24) {
        if (m->pos[1] + 60 <= wallA.vertex1[1] + 5) {
            p[2] = 50;
            hi->walls[0] = &wallA;
            hi->numWalls = 1;
        } else if (!skipLower && m->pos[1] + 30 <= wallA.vertex1[1] + 5) {
            p[2] = 24;
            lo->walls[0] = &wallA;
            lo->numWalls = 1;
        }
    }
}
static s32 find_wall_collisions(struct WallCollisionData *wallCols) {
    wallCols->numWalls = 0;
    if (mode == 12) {
        wallCols->z = 51;
        wallCols->walls[0] = &wallB;
        wallCols->numWalls = 1;
        return 1;
    }
    if (mode == 13) {
        wallCols->x = wallCols->z = 51;
        wallCols->walls[0] = ledgeWallOrder ? &wallB : &wallA;
        wallCols->walls[1] = ledgeWallOrder ? &wallA : &wallB;
        wallCols->numWalls = 2;
        return 2;
    }
    if (mode == 9 && edgeWallEnabled && fabsf(wallCols->z - 50.0f) <= wallCols->radius) {
        wallCols->z = 51;
        wallCols->walls[0] = &wallB;
        wallCols->numWalls = 1;
    }
    return wallCols->numWalls;
}
static void stop_sound(u32 sound, Vec3f p) {
    if (sound == SOUND_ACTION_BONK || sound == SOUND_ACTION_METAL_BONK) {
        bonkStopCalls++;
    }
    if (activeBonk == sound && activeBonkSource == p) {
        activeBonk = 0;
    }
}
static void play_sound(s32 sound, Vec3f p) {
    if (sound == SOUND_ACTION_BONK || sound == SOUND_ACTION_METAL_BONK) {
        if (activeBonk && activeBonkSource == p) {
            bonkOverlaps++;
        }
        activeBonk = sound;
        activeBonkSource = p;
        bonks++;
    }
    if (sound == SOUND_ACTION_TWIRL) {
        twirlSounds++;
    }
    if (sound == SOUND_ACTION_METAL_BONK) {
        metalBonks++;
    }
    if (sound == SOUND_ACTION_HIT) {
        kickImpacts++;
    }
}
static void play_mario_sound(struct MarioState *m, s32 a, s32 b) {
}
static void play_mario_landing_sound(struct MarioState *m, s32 a) {
    sSlideLandingSounds++;
}
static void play_mario_landing_sound_once(struct MarioState *m, u32 sound) {
    if (!(m->flags & MARIO_ACTION_SOUND_PLAYED)) {
        play_mario_landing_sound(m, sound);
        m->flags |= MARIO_ACTION_SOUND_PLAYED;
    }
}
static void play_flip_sounds(struct MarioState *m, s16 a, s16 b, s16 c) {
}
static s32 check_kick_or_dive_in_air(struct MarioState *m) {
    return FALSE;
}
static void mario_reset_attack_hit_record(void) {
}
static void play_knockback_sound(struct MarioState *m) {
}
static void play_sound_if_no_flag(struct MarioState *m, s32 sound, u32 flag) {
}
static s32 mario_has_dive_input(struct MarioState *m) {
    return FALSE;
}
static s32 is_anim_at_end(struct MarioState *m) {
    return FALSE;
}
static s32 is_anim_past_end(struct MarioState *m) {
    return is_anim_at_end(m);
}

static s32 level_trigger_warp(struct MarioState *m, s32 op) {
    warps++;
    return 20;
}
void mario_air_twirl_transition(struct MarioState *m, u32 action);
static s32 set_mario_action(struct MarioState *m, u32 a, u32 arg) {
    mario_air_twirl_transition(m, a);
    if (a & ACT_FLAG_AIR) {
        m->peakHeight = m->pos[1];
        m->flags |= MARIO_JUMPING;
    }
    m->prevAction = m->action;
    m->action = a;
    m->actionArg = arg;
    m->actionState = m->actionTimer = 0;
    m->landingJumpInputFrames = m->wallKickInputFrames = 0;
    return TRUE;
}
static s32 set_jumping_action(struct MarioState *m, u32 a, u32 arg) {
    return set_mario_action(m, a, arg);
}
static void mario_set_forward_vel(struct MarioState *m, f32 v) {
    m->forwardVel = v;
    m->slideVelX = m->vel[0] = v * sins(m->faceAngle[1]);
    m->slideVelZ = m->vel[2] = v * coss(m->faceAngle[1]);
}
static f32 approach_f32(f32 value, f32 target, f32 inc, f32 dec) {
    return value < target ? MIN(value + inc, target) : MAX(value - dec, target);
}
static void set_mario_y_vel_based_on_fspeed(struct MarioState *m, f32 v, f32 mult) {
    m->vel[1] = v + mult * m->forwardVel;
}
static void mario_drop_held_object(struct MarioState *m) {
    m->heldObj = NULL;
}
static s32 drop_and_set_mario_action(struct MarioState *m, u32 a, u32 arg) {
    mario_drop_held_object(m);
    return set_mario_action(m, a, arg);
}
static void mario_limit_velocity(struct MarioState *m) {
    for (int i = 0; i < 3; i++) {
        m->vel[i] = MAX(-MARIO_GLOBAL_SPEED_CAP, MIN(MARIO_GLOBAL_SPEED_CAP, m->vel[i]));
    }
}
static s32 mario_wall_collision_begin(struct MarioState *m, s32 n, f32 lo, f32 hi, f32 y, f32 pad) {
    stepCalls++;
    return TRUE;
}
static s32 mario_get_terrain_sound_addend(struct MarioState *m) {
    return 0;
}
static s32 sCapSurfaceHits;
static u32 sCapSurfaceInteraction;
static struct Surface *sCapSurface;
static void mario_attack_cap_box_surface(struct MarioState *m, struct Surface *surface,
                                         u32 interaction) {
    sCapSurfaceHits++;
    sCapSurface = surface;
    sCapSurfaceInteraction = interaction;
}
static s32 sImpactRumbles, sImpactTime, sImpactStrength;
static void queue_rumble_data(s32 time, s32 strength) {
    sImpactRumbles++;
    sImpactTime = time;
    sImpactStrength = strength;
}
static void reset_rumble_timers_slip(void) {
}
static void adjust_sound_for_speed(struct MarioState *m) {
}
static void align_with_floor(struct MarioState *m) {
}
static s32 mario_floor_is_slippery(struct MarioState *m) {
    return FALSE;
}
static s32 mario_floor_is_slope(struct MarioState *m) {
    return m->floor && m->floor->normal.y < 0.95f;
}
static void slide_bonk(struct MarioState *m, u32 fast, u32 slow) {
    set_mario_action(m, slow, 0);
}
static s32 sAnimationAccel;
static s32 set_mario_animation(struct MarioState *m, s32 a) {
    animation = a;
    sAnimationAccel = 0;
    return 0;
}
static s32 set_mario_anim_with_accel(struct MarioState *m, s32 animationID, s32 accel) {
    animation = animationID;
    sAnimationAccel = accel;
    return 0;
}
static void mario_transition_pose_begin(struct MarioState *m, s32 a) {
    if (a != MARIO_ANIM_START_WALLKICK) {
        exit(2);
    }
    poseBegins++;
}
static void update_air_without_turn(struct MarioState *m) {
}
static s32 update_sliding(struct MarioState *m, f32 stopSpeed) {
    slideUpdateCalls++;
    return slideStops;
}
static s32 sAirTwirlLandingBlocked;
static s32 check_fall_damage_or_get_stuck(struct MarioState *m, u32 a) {
    if (sAirTwirlLandingBlocked) {
        set_mario_action(m, a, 0);
        return TRUE;
    }
    return FALSE;
}
static s32 mario_ground_twirl_is_active(struct MarioState *m) {
    return FALSE;
}
s32 set_water_plunge_action(struct MarioState *m);
static s32 mario_is_riding_underwater_shell(struct MarioState *m) {
    return sRidingUnderwaterShell;
}
static s32 mario_transition_underwater_shell_to_swimming(struct MarioState *m) {
    return FALSE;
}
static s32 lava_boost_on_wall(struct MarioState *m) {
    return set_mario_action(m, ACT_LAVA_BOOST, 0);
}
static s32 start_wall_kick_from_wall(struct MarioState *m);
s32 perform_air_step_with_remainder(struct MarioState *m, u32 arg, f32 *remaining);
s32 perform_air_step_with_fraction(struct MarioState *m, u32 arg, f32 fraction);
s32 perform_slide_kick_air_step(struct MarioState *m, f32 *remaining, s32 checkWater, f32 *waterLevel);
static void mario_attack_breakable_box_wall(struct MarioState *m) {
}
s32 mario_has_land_movement_underwater(struct MarioState *m);
static s32 physics_lab_is_cold_pool(s32 level, s32 area, f32 x, f32 z) {
    return FALSE;
}
static void spawn_slide_kick_liquid_particles(struct MarioState *m, f32 height,
                                              enum LiquidParticleMaterial material) {
}
void mario_he_thicc_slide_kick_bounce(struct MarioState *m) {
    sSlideBounceEffects++;
}
static s32 perform_air_step(struct MarioState *m, u32 arg) {
    return perform_air_step_with_remainder(m, arg, NULL);
}
static s32 sContactShakes, sContactShakeType;
static void set_camera_shake_from_hit(s16 shake) {
    sContactShakes++;
    sContactShakeType = shake;
}
void mario_he_thicc_contact_shake(struct MarioState *m);
void update_air_with_turn(struct MarioState *m);
void wing_update_air_without_turn(struct MarioState *m);
static void backward_butt_slide_jump_restrict_uphill(struct MarioState *m);
void set_vel_from_pitch_and_yaw(struct MarioState *m);
struct Surface *mario_find_air_twirl_entry_wall(struct MarioState *m);
static s32 mario_air_twirl_bounce_off_wall(struct MarioState *m);
void mario_play_bonk_sound(struct MarioState *m);
static void set_camera_mode(struct Camera *camera, s32 mode, s32 frames) {
    camera->mode = mode;
}
static void update_flying_camera_mode(struct MarioState *m, s32 flying) {
    m->area->camera->mode = flying ? FLYING_CAMERA_MODE : m->area->camera->defMode;
}
static void load_level_init_text(s32 text) {
    sFlightTextCalls++;
}
static void set_anim_to_frame(struct MarioState *m, s32 frame) {
    m->marioObj->header.gfx.animInfo.animFrame = frame;
}
static void finish_water_jump_pose(struct MarioState *m) {
    m->faceAngle[0] = m->faceAngle[2] = 0;
}
static void apply_backward_butt_slide_jump_drag(struct MarioState *m);
f32 mario_air_twirl_gravity_scale(struct MarioState *m);
s32 perform_air_step_with_remainder(struct MarioState *m, u32 arg, f32 *remaining);
s16 abs_angle_diff(s16 a0, s16 a1);
void mario_play_wall_bonk_sound(struct MarioState *m);
void mario_play_air_twirl_sound(struct MarioState *m);
void mario_bonk_reflection(struct MarioState *m, u32 negateSpeed);
static s32 sAirTwirlContacts;
static void mario_ground_twirl_cancel_queued_input(void) {
}
static void mario_twirl_begin_contacts(void) {
    sAirTwirlContacts++;
}
static void mario_twirl_push_enemies(struct MarioState *m) {
}
static s32 mario_start_ground_twirl(struct MarioState *m, s32 continuation) {
    m->airTwirlContinue = FALSE;
    return set_mario_action(m, ACT_GROUND_TWIRL, 0);
}
// The castle window callback is covered by the rooftop encounter fixture.
static void castle_secret_window_bonk(struct MarioState *m) {
    (void) m;
}

static void castle_secret_glass_check_air(struct MarioState *m) {
    (void) m;
}

#include "geometry_sources.inc"

// Match ground find_floor's integer coordinates and upward-facing triangle
// bounds. A supplied flat bottom lets a slide leave the actual rock mesh.
static f32 rock_floor(f32 x, f32 y, f32 z, struct Surface **floor) {
    s32 ix = x, iy = y, iz = z;
    f32 height = ground.originOffset;
    *floor = &ground;
    for (unsigned t = 0; t < sizeof(rockFaces) / sizeof(rockFaces[0]); t++) {
        struct Surface *f = &rockFaces[t];
        if (f->normal.y <= 0.01f) {
            continue;
        }
        int inside = TRUE;
        for (int edge = 0; edge < 3; edge++) {
            f32 *a = rockWorldVertices[rockTriangles[t][edge]],
                *b = rockWorldVertices[rockTriangles[t][(edge + 1) % 3]];
            if ((a[2] - iz) * (b[0] - a[0]) - (a[0] - ix) * (b[2] - a[2]) < 0) {
                inside = FALSE;
            }
        }
        if (!inside) {
            continue;
        }
        f32 h = -(f->normal.x * ix + f->normal.z * iz + f->originOffset) / f->normal.y;
        if (h <= iy + 78 && h > height) {
            height = h;
            *floor = f;
        }
    }
    return height;
}
static void place_rock(s32 x, s32 y, s32 z, s32 degrees) {
    s16 yaw = (s32) (degrees * 32768.0f / 180.0f);
    for (unsigned i = 0; i < sizeof(rockVertices) / sizeof(rockVertices[0]); i++) {
        rockWorldVertices[i][0] =
            (s16) (x + coss(yaw) * rockVertices[i][0] + sins(yaw) * rockVertices[i][2]);
        rockWorldVertices[i][1] = y + rockVertices[i][1];
        rockWorldVertices[i][2] =
            (s16) (z - sins(yaw) * rockVertices[i][0] + coss(yaw) * rockVertices[i][2]);
    }
    for (unsigned t = 0; t < sizeof(rockFaces) / sizeof(rockFaces[0]); t++) {
        f32 *p = rockWorldVertices[rockTriangles[t][0]], *q = rockWorldVertices[rockTriangles[t][1]],
            *r = rockWorldVertices[rockTriangles[t][2]];
        Vec3f a, b, n;
        for (int i = 0; i < 3; i++) {
            a[i] = q[i] - p[i];
            b[i] = r[i] - p[i];
        }
        n[0] = a[1] * b[2] - a[2] * b[1];
        n[1] = a[2] * b[0] - a[0] * b[2];
        n[2] = a[0] * b[1] - a[1] * b[0];
        f32 length = sqrtf(sqr(n[0]) + sqr(n[1]) + sqr(n[2]));
        for (int i = 0; i < 3; i++) {
            n[i] /= length;
        }
        rockFaces[t] = (struct Surface){ .normal = { n[0], n[1], n[2] },
                                         .originOffset = -(n[0] * p[0] + n[1] * p[1] + n[2] * p[2]) };
    }
    ground.originOffset = y - 872;
}

static struct MarioState fresh(void) {
    sArea.terrainType = 0;
    sArea.camera = &sCamera;
    sCamera.mode = sCamera.defMode = CAMERA_MODE_8_DIRECTIONS;
    sFlightTextCalls = 0;
    sSlideBounceEffects = sSlideLandingSounds = 0;
    sFlatFloorContact = FALSE;
    sPalette = MARIO_PALETTE_STANDARD;
    sContactShakes = 0;
    struct MarioState m = { 0 };
    m.marioObj = &marioObject;
    m.floor = &ground;
    m.controller = &controller;
    controller.stickMag = 64.0f;
    controller.stickX = controller.stickY = 0.0f;
    controller.buttonDown = controller.buttonPressed = 0;
    sAirTwirlContacts = sAirTwirlLandingBlocked = 0;
    m.action = ACT_JUMP;
    m.vel[0] = 20;
    m.vel[2] = 10;
    m.input = INPUT_NONZERO_ANALOG;
    m.intendedYaw = 0x2000;
    ground = (struct Surface){ .normal.y = 1, .originOffset = -200 };
    topA = (struct Surface){ .normal.y = 1 };
    topB = topA;
    wallA = (struct Surface){ .normal.x = -1 };
    wallB = (struct Surface){ .normal.z = -1 };
    m.area = &sArea;
    m.health = 0x880;
    m.waterLevel = -10000;
    m.marioBodyState = &bodyState;
    bodyState.wingFlutter = FALSE;
    ceilingAtBlockedFootprint = FALSE;
    gGlobalTimer = 10;
    ceilingY = 10000;
    sShellWaterLevel = -10000;
    sRidingUnderwaterShell = FALSE;
    poseBegins = kickImpacts = metalBonks = bonks = twirlSounds = warps = stepCalls = 0;
    sCapSurfaceHits = 0;
    sCapSurface = NULL;
    activeBonk = bonkOverlaps = bonkStopCalls = 0;
    activeBonkSource = NULL;
    return m;
}

static int bonk_sound_replacement(void) {
    struct MarioState m = fresh();
    m.wall = &wallA;
    for (int cycle = 0; cycle < 3; cycle++) {
        m.flags = 0;
        mario_bonk_reflection(&m, FALSE);
        EXPECT_EQ(activeBonk, SOUND_ACTION_BONK);
        m.flags = MARIO_METAL_CAP;
        mario_bonk_reflection(&m, FALSE);
        EXPECT_EQ(activeBonk, SOUND_ACTION_METAL_BONK);
    }
    EXPECT_EQ(bonks, 6);
    EXPECT_EQ(metalBonks, 3);
    EXPECT_EQ(bonkStopCalls, 12);
    EXPECT_EQ(bonkOverlaps, 0);
    return 1;
}

static int wall_kick_sound_requires_new_kick(void) {
    struct MarioState m = fresh();
    m.wall = &wallA;
    m.forwardVel = 40;
    begin_wall_kick_window(&m);
    EXPECT_EQ(bonks, 1);
    EXPECT_EQ(m.wallKickContactSoundPlayed, TRUE);

    // Leaving the window and contacting another wall without a kick is silent.
    m.action = ACT_FREEFALL;
    m.wall = &wallB;
    m.flags = MARIO_METAL_CAP;
    begin_wall_kick_window(&m);
    EXPECT_EQ(bonks, 1);
    EXPECT_EQ(metalBonks, 0);

    // An accepted kick rearms the next impact, including the Metal variant.
    start_wall_kick_from_wall(&m);
    EXPECT_EQ(m.wallKickContactSoundPlayed, FALSE);
    for (int frame = 0; frame < MARIO_WALL_TWIRL_SOUND_GAP_FRAMES; frame++) {
        mario_try_air_twirl(&m);
    }
    m.wall = &wallA;
    begin_wall_kick_window(&m);
    EXPECT_EQ(bonks, 2);
    EXPECT_EQ(metalBonks, 1);
    EXPECT_EQ(bonkOverlaps, 0);

    // Further contacts stay silent until another kick really starts.
    m.action = ACT_FREEFALL;
    m.wall = &wallB;
    begin_wall_kick_window(&m);
    EXPECT_EQ(bonks, 2);
    return 1;
}

static int wall_slide_edge_exit(void) {
    for (int timer = 1; timer <= WALL_KICK_GRACE_FRAMES; timer += WALL_KICK_GRACE_FRAMES - 1) {
        mode = 0;
        struct MarioState m = fresh();
        m.action = ACT_AIR_HIT_WALL;
        m.actionTimer = timer;
        m.wall = &wallB; // The previous update's contact is stale at this edge.
        m.wallKickContactFrame = gGlobalTimer - 1;
        m.wallKickContactNormal[2] = -1.0f;
        m.wallKickContactYaw = 0;
        m.wallKickTimer = 4;
        m.pos[1] = 1000.0f;
        mario_set_forward_vel(&m, 24.0f);
        EXPECT_EQ(act_air_hit_wall(&m), FALSE);
        EXPECT_EQ(m.action, ACT_FREEFALL);
        EXPECT_EQ(animation, MARIO_ANIM_GENERAL_FALL);
        EXPECT_EQ(m.wallKickTimer, 0);
        EXPECT_EQ(m.particleFlags & PARTICLE_VERTICAL_STAR, 0);
        EXPECT_EQ(bonks, 0);
        EXPECT_EQ(stepCalls, 1);
    }
    return 1;
}

#define NEAR(x, y)                                                                                     \
    do {                                                                                               \
        if (!(fabsf((x) - (y)) <= 0.01f)) {                                                            \
            fprintf(stderr, "%s: %s expected %.3f got %.3f\n", sCurrentCase, #x, (double) (y),         \
                    (double) (x));                                                                     \
            return 0;                                                                                  \
        }                                                                                              \
    } while (0)

static int slide_kick_bounce_travel(void) {
    const f32 speeds[] = { -100.0f, -40.0f, 20.0f, 40.0f, 100.0f };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int surface = 0; surface < 5; surface++) {
            for (int heldA = 0; heldA < 2; heldA++) {
                for (int late = 0; late < 3; late++) {
                    for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(speeds[0]); speed++) {
                        struct MarioState m = fresh();
                        mode = 0;
                        ground.originOffset = m.floorHeight = 0.0f;
                        ground.type = surface == 1 ? SURFACE_BURNING : SURFACE_DEFAULT;
                        sShellWaterLevel = surface == 2 || surface == 3 ? 10.0f : -10000.0f;
                        sArea.terrainType = surface == 3 ? TERRAIN_SNOW : TERRAIN_GRASS;
                        m.flags = surface == 4 ? MARIO_METAL_CAP : 0;
                        m.action = ACT_SLIDE_KICK;
                        m.actionTimer = 5;
                        m.actionArg = 3;
                        m.input = heldA ? INPUT_A_DOWN : 0;
                        m.marioObj->header.gfx.animInfo.animFrame = 12;
                        vec3f_set(m.vel, 0.0f, -40.0f, speeds[speed]);
                        m.forwardVel = speeds[speed];
                        s32 steps = get_mario_ground_air_collision_steps(
                            0.0f, -40.0f, speeds[speed], MARIO_GROUND_AIR_COLLISION_MIN_STEPS,
                            MARIO_GROUND_AIR_COLLISION_MAX_STEPS);
                        if (late == 2) {
                            m.pos[1] = 40.0f * (steps - 1) / steps + 0.5f;
                        } else if (late == 1) {
                            m.pos[1] = 40.0f * (steps - 2) / steps + 0.5f;
                        } else {
                            m.pos[1] = surface == 2 || surface == 3 ? 11.0f : 1.0f;
                        }
                        sFlatFloorContact = TRUE;
                        f32 contactY = surface == 2 || surface == 3 ? 10.0f : 0.0f;
                        f32 remaining = 1.0f - (m.pos[1] - contactY) / 40.0f;
                        f32 bounceFactor = heldA && surface >= 1 && surface <= 3 ? 0.6f : 0.5f;
                        struct MarioState reference = m;
                        reference.pos[1] = contactY;
                        apply_gravity(&reference);
                        f32 rebound = -reference.vel[1] * bounceFactor;
                        EXPECT_EQ(act_slide_kick(&m), FALSE);
                        NEAR(m.pos[2], speeds[speed]);
                        NEAR(m.pos[1], contactY + rebound * remaining);
                        NEAR(m.vel[1], rebound);
                        NEAR(m.forwardVel, speeds[speed]);
                        NEAR(m.marioObj->header.gfx.pos[1], m.pos[1]);
                        EXPECT_EQ(m.marioObj->header.gfx.animInfo.animFrame, 12);
                        EXPECT_EQ(m.actionState, surface == 2 || surface == 3
                                                     ? ACT_STATE_SLIDE_KICK_WATER_SKIPPED
                                                     : ACT_STATE_SLIDE_KICK_END);
                        EXPECT_EQ(m.actionTimer, 0);
                        EXPECT_EQ(m.actionArg, 2);
                        EXPECT_EQ(sSlideBounceEffects, 1);
                        EXPECT_EQ(sSlideLandingSounds, surface == 2 || surface == 3 ? 0 : 1);
                        for (int frame = 0; frame < 30 && m.action == ACT_SLIDE_KICK; frame++) {
                            act_slide_kick(&m);
                        }
                        EXPECT_EQ(m.action, ACT_SLIDE_KICK_SLIDE);
                        EXPECT_EQ(sSlideBounceEffects, 1);
                    }
                }
            }
        }
    }
    return TRUE;
}

static int swept_air_floor_landing(void) {
    const u32 actions[] = { ACT_DIVE, ACT_SIDE_FLIP, ACT_FREEFALL };
    for (unsigned action = 0; action < sizeof(actions) / sizeof(actions[0]); action++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            struct MarioState m = fresh();
            mode = 0;
            ground.originOffset = -300.0f;
            m.action = actions[action];
            vec3f_set(m.pos, -9398.0f, 141.2f, 304.0f);
            vec3f_set(m.vel, -64.0f, -64.0f, 0.0f);
            Vec3f next = { -9406.0f, 133.2f, 304.0f };
            vec3f_set(sFloorContact, -9402.0f, 137.2f, 304.0f);
            sCrossedFloor = &topA;
            EXPECT_EQ(perform_air_quarter_step(&m, next, 0, FALSE), AIR_STEP_LANDED);
            NEAR(m.pos[0], sFloorContact[0]);
            NEAR(m.pos[1], sFloorContact[1]);
            NEAR(m.pos[2], sFloorContact[2]);
            EXPECT_EQ(m.floor == &topA, TRUE);
            NEAR(m.floorHeight, sFloorContact[1]);
            sCrossedFloor = NULL;

            // An ordinary endpoint landing must keep its existing horizontal travel.
            m = fresh();
            ground.originOffset = 0.0f;
            m.action = actions[action];
            vec3f_set(m.pos, 0, 4, 0);
            vec3f_set(next, 8, -4, 0);
            vec3f_set(sFloorContact, 4, 0, 0);
            sCrossedFloor = &topA;
            EXPECT_EQ(perform_air_quarter_step(&m, next, 0, FALSE), AIR_STEP_LANDED);
            NEAR(m.pos[0], 8);
            NEAR(m.pos[1], ground.originOffset);
            EXPECT_EQ(m.floor == &ground, TRUE);
            sCrossedFloor = NULL;

            // The shipped ridge can also hide behind a lower endpoint landing.
            m = fresh();
            mode = 0;
            ground.originOffset = 1.350235f;
            m.action = actions[action];
            vec3f_set(m.pos, -9498, 6.8f, 104);
            vec3f_set(next, -9498, -1.2f, 96);
            vec3f_set(sFloorContact, -9498, 2.8f, 100);
            sCrossedFloor = &topA;
            EXPECT_EQ(perform_air_quarter_step(&m, next, 0, FALSE), AIR_STEP_LANDED);
            NEAR(m.pos[1], 2.8f);
            NEAR(m.pos[2], 100);
            EXPECT_EQ(m.floor == &topA, TRUE);
            sCrossedFloor = NULL;
        }
    }
    return TRUE;
}

static int fence_edge_grinding(void) {
    const u32 actions[] = { ACT_FREEFALL, ACT_HOLD_FREEFALL, ACT_DIVE, ACT_SLIDE_KICK };
    for (unsigned action = 0; action < sizeof(actions) / sizeof(actions[0]); action++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            struct MarioState m = fresh();
            mode = 16;
            topA.originOffset = 0.0f;
            ground.originOffset = -300.0f;
            m.floor = &topA;
            m.floorHeight = 0.0f;
            m.pos[1] = 0.0f;
            m.pos[2] = 0.5f;
            m.action = ACT_WALKING;
            vec3f_set(m.vel, 0.0f, 0.0f, 2.0f);
            EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_LEFT_GROUND);
            m.action = actions[action];
            vec3f_set(m.vel, 0.0f, -4.0f, 0.0f);
            EXPECT_EQ(perform_air_step(&m, 0), AIR_STEP_NONE);
            EXPECT_EQ(m.pos[1] < 0.0f, TRUE);
            f32 beforeY = m.pos[1];
            m.vel[2] = -2.0f;
            EXPECT_EQ(perform_air_step(&m, 0), AIR_STEP_NONE);
            EXPECT_EQ(m.pos[1] < beforeY, TRUE);
            EXPECT_EQ(m.floor == &ground, TRUE);
            for (int frame = 0; frame < 5; frame++) {
                beforeY = m.pos[1];
                m.vel[2] = 0.0f;
                EXPECT_EQ(perform_air_step(&m, 0), AIR_STEP_NONE);
                EXPECT_EQ(m.pos[1] < beforeY, TRUE);
            }
        }
    }
    // A missing-floor or ceiling fallback must not reuse the ledge above his feet.
    for (int lowCeiling = 0; lowCeiling < 2; lowCeiling++) {
        struct MarioState m = fresh();
        mode = 7;
        m.floorHeight = 0.0f;
        m.pos[1] = -1.0f;
        vec3f_set(m.vel, 0.0f, -4.0f, 0.0f);
        if (lowCeiling) {
            ceilingY = 100.0f;
        }
        EXPECT_EQ(perform_air_step(&m, 0) == AIR_STEP_LANDED, FALSE);
        EXPECT_EQ(m.pos[1] < -1.0f, TRUE);
    }
    return TRUE;
}

static int air_rebound_fraction(void) {
    const u32 caps[] = { 0, MARIO_METAL_CAP, MARIO_VANISH_CAP, MARIO_METAL_CAP | MARIO_VANISH_CAP };
    const f32 fractions[] = { -1.0f, 0.0f, 0.25f, 0.5f, 1.0f, 2.0f };
    for (unsigned cap = 0; cap < sizeof(caps) / sizeof(caps[0]); cap++) {
        for (unsigned fraction = 0; fraction < sizeof(fractions) / sizeof(fractions[0]); fraction++) {
            struct MarioState m = fresh();
            mode = 0;
            m.action = ACT_SLIDE_KICK;
            m.flags = caps[cap];
            m.pos[1] = 1000.0f;
            m.waterLevel = 2000.0f;
            m.actionTimer = 7;
            m.windCatchActive = TRUE;
            vec3f_set(m.vel, 0.0f, 22.0f, 40.0f);
            f32 scale = mario_get_movement_time_scale(&m);
            f32 fractionUsed = CLAMP(fractions[fraction], 0.0f, 1.0f);
            EXPECT_EQ(perform_air_step_with_fraction(&m, 0, fractions[fraction]), AIR_STEP_NONE);
            NEAR(m.pos[2], 40.0f * scale * fractionUsed);
            NEAR(m.pos[1], 1000.0f + 22.0f * scale * fractionUsed);
            NEAR(m.vel[1], 22.0f);
            EXPECT_EQ(m.actionTimer, 7);
            EXPECT_EQ(m.windCatchActive, TRUE);
        }
    }
    return TRUE;
}

static int slide_kick_uphill_bounce(void) {
    const f32 slopes[] = { 0.25f, 0.5f, 1.0f, 2.0f, 4.0f };
    const f32 speeds[] = { 100.0f, 40.0f, 20.0f };
    const f32 verticalSpeeds[] = { -40.0f, -8.0f, -0.01f, 0.0f, 0.01f, 2.0f, 8.0f, 20.0f };
    for (unsigned slope = 0; slope < sizeof(slopes) / sizeof(slopes[0]); slope++) {
        for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(speeds[0]); speed++) {
            for (unsigned vertical = 0; vertical < sizeof(verticalSpeeds) / sizeof(verticalSpeeds[0]);
                 vertical++) {
                for (int backward = 0; backward < 2; backward++) {
                    for (int lava = 0; lava < 2; lava++) {
                        struct MarioState m = fresh();
                        mode = 17;
                        f32 direction = backward ? -1.0f : 1.0f;
                        ground.normal.y = 1.0f / sqrtf(1.0f + sqr(slopes[slope]));
                        ground.normal.z = -direction * slopes[slope] * ground.normal.y;
                        ground.originOffset = 0.0f;
                        ground.type = lava ? SURFACE_BURNING : SURFACE_DEFAULT;
                        m.action = ACT_SLIDE_KICK;
                        m.actionTimer = 5;
                        m.input = INPUT_A_DOWN;
                        m.pos[1] = 0.1f;
                        m.forwardVel = direction * speeds[speed];
                        vec3f_set(m.vel, 0.0f, verticalSpeeds[vertical], m.forwardVel);
                        struct MarioState reference = m;
                        update_air_without_turn(&reference);
                        f32 floorRise = slopes[slope] * fabsf(reference.vel[2]);
                        if (reference.vel[1] >= floorRise) {
                            continue;
                        }
                        f32 remaining;
                        EXPECT_EQ(perform_air_step_with_remainder(&reference, 0, &remaining),
                                  AIR_STEP_LANDED);
                        f32 factor = lava ? 0.6f : 0.5f;
                        f32 rebound = MAX(0.0f, -reference.vel[1] * factor);
                        act_slide_kick(&m);
                        // A rising floor can consume the rebound immediately, but
                        // cannot turn horizontal approach into extra upward speed.
                        EXPECT_EQ(m.action == ACT_SLIDE_KICK || m.action == ACT_SLIDE_KICK_SLIDE, TRUE);
                        NEAR(m.vel[1], verticalSpeeds[vertical] < 0.0f ? rebound : reference.vel[1]);
                        NEAR(m.forwardVel, reference.forwardVel);
                        int bounceCount = verticalSpeeds[vertical] < 0.0f ? 1 : 0;
                        EXPECT_EQ(sSlideBounceEffects, bounceCount);
                        EXPECT_EQ(sSlideLandingSounds, 1);
                        if (verticalSpeeds[vertical] >= 0.0f) {
                            EXPECT_EQ(m.action, ACT_SLIDE_KICK_SLIDE);
                        }
                        f32 contactY = reference.pos[1];
                        for (int frame = 0; frame < 3 && m.action == ACT_SLIDE_KICK; frame++) {
                            EXPECT_EQ(m.vel[1] <= rebound + 0.001f, TRUE);
                            EXPECT_EQ(m.pos[1] <= contactY + rebound * (frame + 1) + 0.001f, TRUE);
                            act_slide_kick(&m);
                            EXPECT_EQ(sSlideBounceEffects, bounceCount);
                        }
                    }
                }
            }
        }
    }
    // Gravity may cross zero in free air without producing a contact bounce.
    for (unsigned vertical = 0; vertical < sizeof(verticalSpeeds) / sizeof(verticalSpeeds[0]);
         vertical++) {
        struct MarioState m = fresh();
        mode = 17;
        ground.normal.y = 1.0f / sqrtf(1.0f + sqr(0.25f));
        ground.normal.z = -0.25f * ground.normal.y;
        ground.originOffset = 0.0f;
        m.action = ACT_SLIDE_KICK;
        m.actionTimer = 5;
        m.pos[1] = 100.0f;
        m.forwardVel = 20.0f;
        vec3f_set(m.vel, 0.0f, verticalSpeeds[vertical], m.forwardVel);
        act_slide_kick(&m);
        EXPECT_EQ(m.action, ACT_SLIDE_KICK);
        EXPECT_EQ(m.actionState, ACT_STATE_SLIDE_KICK_SLIDING);
        EXPECT_EQ(sSlideBounceEffects, 0);
        EXPECT_EQ(sSlideLandingSounds, 0);
        NEAR(m.pos[1], 100.0f + verticalSpeeds[vertical]);
    }
    // Downhill contacts retain their original vertical bounce strength.
    for (unsigned slope = 0; slope < sizeof(slopes) / sizeof(slopes[0]); slope++) {
        if (slopes[slope] > 1.0f) {
            continue; // These steep downhill planes outrun the supplied descent.
        }
        struct MarioState m = fresh();
        mode = 17;
        ground.normal.y = 1.0f / sqrtf(1.0f + sqr(slopes[slope]));
        ground.normal.z = slopes[slope] * ground.normal.y;
        ground.originOffset = 0.0f;
        m.action = ACT_SLIDE_KICK;
        m.actionTimer = 5;
        m.pos[1] = 0.1f;
        m.forwardVel = 20.0f;
        vec3f_set(m.vel, 0.0f, -40.0f, 20.0f);
        struct MarioState reference = m;
        update_air_without_turn(&reference);
        f32 remaining;
        EXPECT_EQ(perform_air_step_with_remainder(&reference, 0, &remaining), AIR_STEP_LANDED);
        act_slide_kick(&m);
        EXPECT_EQ(m.action, ACT_SLIDE_KICK);
        NEAR(m.vel[1], -reference.vel[1] * SLIDE_KICK_NORMAL_BOUNCE_FACTOR);
        NEAR(m.forwardVel, reference.forwardVel);
        EXPECT_EQ(sSlideBounceEffects, 1);
    }
    return TRUE;
}

static int slide_kick_bounce_obstructions(void) {
    for (int obstruction = 0; obstruction < 3; obstruction++) {
        struct MarioState m = fresh();
        mode = obstruction == 0 ? 0 : 15;
        ground.originOffset = m.floorHeight = 0.0f;
        m.action = ACT_SLIDE_KICK;
        m.actionTimer = 3;
        m.slideKickStartupFrames = 3;
        m.pos[1] = 1.0f;
        m.forwardVel = 40.0f;
        vec3f_set(m.vel, 0.0f, -40.0f, 40.0f);
        if (obstruction == 0) {
            ceilingY = 170.0f;
        } else if (obstruction == 2) {
            wallB.type = SURFACE_BURNING;
        }
        act_slide_kick(&m);
        EXPECT_EQ(sSlideBounceEffects, 1);
        EXPECT_EQ(sSlideLandingSounds, 1);
        if (obstruction == 0) {
            EXPECT_EQ(m.action, ACT_SLIDE_KICK);
            EXPECT_EQ(m.pos[1] <= 10.0f, TRUE);
            NEAR(m.vel[1], 0.0f);
        } else {
            EXPECT_EQ(m.pos[2] <= 20.0f, TRUE);
            EXPECT_EQ(m.action, obstruction == 1 ? ACT_BACKWARD_AIR_KB : ACT_LAVA_BOOST);
        }
    }
    return TRUE;
}

static int slide_kick_fall_exit(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        struct MarioState m = fresh();
        mode = 0;
        m.action = ACT_SLIDE_KICK;
        m.actionTimer = 39;
        m.actionState = ACT_STATE_SLIDE_KICK_END;
        m.pos[1] = 1000.0f;
        m.floorHeight = 0.0f;
        m.vel[1] = -10.0f;
        m.input = 0;
        mario_set_forward_vel(&m, 40.0f);
        act_slide_kick(&m);
        EXPECT_EQ(m.action, ACT_SLIDE_KICK);
        EXPECT_EQ(m.actionTimer, 40);
        act_slide_kick(&m);
        EXPECT_EQ(m.action, ACT_FREEFALL);
        EXPECT_EQ(m.actionArg, ACT_ARG_FREEFALL_FROM_SLIDE_KICK);
        act_freefall(&m);
        EXPECT_EQ(animation, MARIO_ANIM_FALL_FROM_SLIDE_KICK);
        EXPECT_EQ(sAnimationAccel, 0x20000);
        act_freefall(&m);
        EXPECT_EQ(sAnimationAccel, 0x20000);
        m.actionArg = ACT_ARG_FREEFALL_GENERAL;
        act_freefall(&m);
        EXPECT_EQ(animation, MARIO_ANIM_GENERAL_FALL);
        EXPECT_EQ(sAnimationAccel, 0);
    }
    // Earlier ascent and near-floor kicks retain the original timeout behavior.
    for (int ascending = FALSE; ascending <= TRUE; ascending++) {
        struct MarioState m = fresh();
        mode = 0;
        m.action = ACT_SLIDE_KICK;
        m.actionTimer = ascending ? 20 : 40;
        m.actionState = ACT_STATE_SLIDE_KICK_END;
        m.pos[1] = ascending ? 1000.0f : 100.0f;
        m.floorHeight = 0.0f;
        m.vel[1] = ascending ? 10.0f : -10.0f;
        m.input = 0;
        act_slide_kick(&m);
        EXPECT_EQ(m.action, ACT_SLIDE_KICK);
    }
    return TRUE;
}

static int slide_kick_startup_wall_grace(void) {
    for (s32 cycle = 0; cycle < 3; cycle++) {
        struct MarioState m = fresh();
        mode = 15;
        ground.originOffset = m.floorHeight = 0;
        m.action = ACT_SLIDE_KICK;
        m.pos[1] = 100;
        m.forwardVel = 40;
        m.slideKickStartupFrames = 99; // Fresh entry must clear prior kick history.
        vec3f_set(m.vel, 0, 25, 40);
        for (s32 update = 1; update <= 4; update++) {
            act_slide_kick(&m);
            EXPECT_EQ(m.slideKickStartupFrames, update);
            EXPECT_EQ(m.pos[2] <= 20, TRUE);
            EXPECT_EQ(m.wall != NULL, TRUE);
            EXPECT_EQ(m.action, update <= 3 ? ACT_SLIDE_KICK : ACT_BACKWARD_AIR_KB);
            EXPECT_EQ((m.particleFlags & PARTICLE_VERTICAL_STAR) != 0, update == 4);
            if (update <= 3) {
                EXPECT_EQ(m.vel[1] > 0, TRUE);
            }
        }
        m = fresh();
        mode = 15;
        ground.originOffset = m.floorHeight = 0;
        m.action = ACT_SLIDE_KICK;
        m.pos[1] = 1;
        m.forwardVel = 40;
        vec3f_set(m.vel, 0, -40, 40);
        act_slide_kick(&m);
        EXPECT_EQ(sSlideBounceEffects, 1);
        EXPECT_EQ(m.action, ACT_SLIDE_KICK);
        EXPECT_EQ(m.slideKickStartupFrames, 1);
        EXPECT_EQ(m.actionTimer, 0);
        act_slide_kick(&m);
        EXPECT_EQ(m.slideKickStartupFrames, 2);
        wallB.type = SURFACE_BURNING;
        slide_kick_handle_wall(&m, AIR_STEP_HIT_LAVA_WALL);
        EXPECT_EQ(m.action, ACT_LAVA_BOOST);
    }
    return TRUE;
}

/* The elevated floor query models an exposed platform beyond the shared
 * lookup allowance. With no supporting floor or wall there, X/Z stay blocked. */
static int missing_floor_jump_height(void) {
    const u32 jumps[] = { ACT_JUMP, ACT_HOLD_JUMP, ACT_DOUBLE_JUMP, ACT_TRIPLE_JUMP, ACT_STEEP_JUMP };
    const f32 speeds[] = { 12.0f, 36.0f, 100.0f };
    for (unsigned jump = 0; jump < sizeof(jumps) / sizeof(jumps[0]); jump++) {
        for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(speeds[0]); speed++) {
            for (s32 held = 0; held < 2; held++) {
                for (s32 kick = 0; kick < 2; kick++) {
                    for (s32 cycle = 0; cycle < 3; cycle++) {
                        struct MarioState m = fresh();
                        ground.originOffset = 0.0f;
                        topA.originOffset = 156.0f;
                        m.action = kick ? ACT_JUMP_KICK : jumps[jump];
                        m.prevAction = jumps[jump];
                        m.flags = MARIO_JUMPING;
                        m.input = INPUT_A_DOWN;
                        m.vel[1] = kick ? 39.375f : 52.5f;
                        m.peakHeight = 0.0f;
                        m.faceAngle[1] = 0;
                        m.heldObj = jumps[jump] == ACT_HOLD_JUMP ? &marioObject : NULL;
                        mario_set_forward_vel(&m, speeds[speed]);
                        struct MarioState reference = m;
                        f32 peak = 0.0f;
                        s32 landed = FALSE;
                        for (s32 frame = 0; frame < 80 && !landed; frame++) {
                            m.input = reference.input = held || frame == 0 ? INPUT_A_DOWN : 0;
                            m.framesSinceA = reference.framesSinceA = frame;
                            mode = 0;
                            if (kick) {
                                act_jump_kick(&reference);
                            } else {
                                common_air_action_step(&reference, ACT_FREEFALL_LAND,
                                                       MARIO_ANIM_GENERAL_FALL, 0);
                            }
                            mode = 14;
                            if (kick) {
                                act_jump_kick(&m);
                            } else {
                                common_air_action_step(&m, ACT_FREEFALL_LAND, MARIO_ANIM_GENERAL_FALL,
                                                       0);
                            }
                            NEAR(m.pos[1], reference.pos[1]);
                            if (m.action != ACT_FREEFALL_LAND) {
                                NEAR(m.vel[1], reference.vel[1]);
                            }
                            EXPECT_EQ(m.action, reference.action);
                            EXPECT_EQ(m.heldObj == reference.heldObj, TRUE);
                            EXPECT_EQ(m.wall == NULL, TRUE);
                            EXPECT_EQ(m.faceAngle[1], 0);
                            NEAR(m.forwardVel, 0.0f);
                            NEAR(m.pos[0], 0.0f);
                            NEAR(m.pos[2], 0.0f);
                            peak = MAX(peak, m.pos[1]);
                            landed = m.action == ACT_FREEFALL_LAND;
                        }
                        EXPECT_EQ(landed, TRUE);
                        EXPECT_EQ(bonks, 0);
                        EXPECT_EQ(m.particleFlags & PARTICLE_VERTICAL_STAR, 0);
                        EXPECT_EQ(m.wallBonkMomentum, FALSE);
                        if (held) {
                            NEAR(peak, kick ? 213.75f : 371.0f);
                        }
                    }
                }
            }
        }
    }
    // A falling, fast no-floor step also keeps the existing descent and holding action.
    mode = 7;
    struct MarioState m = fresh();
    m.action = ACT_HOLD_FREEFALL;
    m.heldObj = &marioObject;
    m.pos[1] = 1000.0f;
    m.vel[1] = -20.0f;
    m.faceAngle[1] = 0;
    mario_set_forward_vel(&m, 100.0f);
    common_air_action_step(&m, ACT_HOLD_FREEFALL_LAND, MARIO_ANIM_GENERAL_FALL, 0);
    EXPECT_EQ(m.action, ACT_HOLD_FREEFALL);
    EXPECT_EQ(m.heldObj == &marioObject, TRUE);
    NEAR(m.pos[1], 980.0f);
    NEAR(m.vel[1], -24.0f);
    EXPECT_EQ(bonks, 0);
    return 1;
}

static int jump_geometry_preservation(void) {
    for (s32 kick = 0; kick < 2; kick++) {
        struct MarioState m = fresh();
        ground.originOffset = 0.0f;
        topA.originOffset = 117.0f;
        m.action = kick ? ACT_JUMP_KICK : ACT_JUMP;
        m.prevAction = ACT_JUMP;
        m.flags = MARIO_JUMPING;
        m.input = INPUT_A_DOWN;
        m.faceAngle[1] = 0;
        m.vel[1] = kick ? 39.375f : 52.5f;
        mario_set_forward_vel(&m, 36.0f);
        // The same step allowance still lifts both actions onto a reachable lip.
        mode = 14;
        for (s32 frame = 0; frame < 4 && m.action != ACT_FREEFALL_LAND; frame++) {
            if (kick) {
                act_jump_kick(&m);
            } else {
                common_air_action_step(&m, ACT_FREEFALL_LAND, MARIO_ANIM_GENERAL_FALL, 0);
            }
        }
        EXPECT_EQ(m.action, ACT_FREEFALL_LAND);
        NEAR(m.pos[1], 117.0f);
        EXPECT_EQ(m.floor == &topA, TRUE);

        m = fresh();
        ground.originOffset = 0.0f;
        m.action = kick ? ACT_JUMP_KICK : ACT_JUMP;
        m.prevAction = ACT_JUMP;
        m.flags = MARIO_JUMPING;
        m.input = INPUT_A_DOWN;
        m.pos[1] = 10.0f;
        m.vel[1] = 40.0f;
        m.faceAngle[1] = 0;
        mario_set_forward_vel(&m, 36.0f);
        mode = 0;
        ceilingY = 180.0f;
        if (kick) {
            act_jump_kick(&m);
        } else {
            common_air_action_step(&m, ACT_FREEFALL_LAND, MARIO_ANIM_GENERAL_FALL, 0);
        }
        EXPECT_EQ(m.vel[1] <= 0.0f, TRUE);
        EXPECT_EQ(m.action, kick ? ACT_JUMP_KICK : ACT_JUMP);
        EXPECT_EQ(sCapSurfaceHits > 0, TRUE);
        EXPECT_EQ(bonks, 0);
    }
    // A missing supporting floor must not bypass a ceiling over the retained footprint.
    for (s32 kick = 0; kick < 2; kick++) {
        mode = 7;
        struct MarioState m = fresh();
        m.action = kick ? ACT_JUMP_KICK : ACT_JUMP;
        m.prevAction = ACT_JUMP;
        m.flags = MARIO_JUMPING;
        m.input = INPUT_A_DOWN;
        m.pos[1] = 10.0f;
        m.vel[1] = 40.0f;
        m.faceAngle[1] = 0;
        mario_set_forward_vel(&m, 36.0f);
        ceilingY = 180.0f;
        ceilingAtBlockedFootprint = TRUE;
        if (kick) {
            act_jump_kick(&m);
        } else {
            common_air_action_step(&m, ACT_FREEFALL_LAND, MARIO_ANIM_GENERAL_FALL, 0);
        }
        EXPECT_EQ(m.vel[1] <= 0.0f, TRUE);
        EXPECT_EQ(m.pos[1] <= ceilingY - 160.0f + 0.01f, TRUE);
        NEAR(m.pos[2], 0.0f);
        NEAR(m.forwardVel, 0.0f);
        EXPECT_EQ(m.action, kick ? ACT_JUMP_KICK : ACT_JUMP);
        EXPECT_EQ(sCapSurfaceHits > 0, TRUE);
        EXPECT_EQ(bonks, 0);
    }
    // The shared ceiling response still permits a real hang and excludes a re-grab.
    for (s32 previousHang = 0; previousHang < 2; previousHang++) {
        mode = 7;
        struct MarioState m = fresh();
        m.prevAction = previousHang ? ACT_START_HANGING : ACT_JUMP;
        m.pos[1] = 10.0f;
        m.vel[1] = 40.0f;
        m.faceAngle[1] = 0;
        m.input = INPUT_A_DOWN;
        mario_set_forward_vel(&m, 36.0f);
        ceilingY = 180.0f;
        ceiling.type = SURFACE_HANGABLE;
        ceilingAtBlockedFootprint = TRUE;
        common_air_action_step(&m, ACT_FREEFALL_LAND, MARIO_ANIM_GENERAL_FALL, 0);
        EXPECT_EQ(m.action, previousHang ? ACT_JUMP : ACT_START_HANGING);
        EXPECT_EQ(m.vel[1] <= 0.0f, TRUE);
        NEAR(m.pos[2], 0.0f);
        EXPECT_EQ(bonks, 0);
        ceiling.type = SURFACE_DEFAULT;
    }
    // A real wall still opens the existing window and keeps upward speed.
    mode = 4;
    struct MarioState m = fresh();
    m.pos[1] = 1000.0f;
    m.vel[1] = 40.0f;
    m.input = INPUT_A_DOWN | INPUT_NONZERO_ANALOG;
    m.faceAngle[1] = m.intendedYaw = 0;
    mario_set_forward_vel(&m, 36.0f);
    mario_update_wall_kick_slide_input(&m);
    common_air_action_step(&m, ACT_FREEFALL_LAND, MARIO_ANIM_GENERAL_FALL, 0);
    EXPECT_EQ(m.wall == &wallB, TRUE);
    EXPECT_EQ(m.action, ACT_AIR_HIT_WALL);
    NEAR(m.vel[1], 36.0f);
    EXPECT_EQ(bonks, 1);
    return 1;
}

static int ledge_contacts(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (mode = 1; mode <= 3; mode++) {
            struct MarioState m = fresh();
            Vec3f p = { 10, 0, 10 };
            EXPECT_EQ(perform_air_quarter_step(&m, p, AIR_STEP_CHECK_LEDGE_GRAB, TRUE),
                      AIR_STEP_GRABBED_LEDGE);
            EXPECT_EQ(m.floor == &topA, TRUE);
            NEAR(m.pos[0], 60);
            NEAR(m.pos[1], 150);
            NEAR(m.pos[2], 0);
            EXPECT_EQ(m.faceAngle[1], (s16) (SURFACE_YAW(&wallA) + 0x8000));
        }
    }
    // A later candidate rejected by its stick direction must retain the winner.
    mode = 2;
    struct MarioState m = fresh();
    wallB.normal.z = 1;
    Vec3f p = { 10, 0, 10 };
    EXPECT_EQ(perform_air_quarter_step(&m, p, AIR_STEP_CHECK_LEDGE_GRAB, TRUE), AIR_STEP_GRABBED_LEDGE);
    EXPECT_EQ(m.floor == &topA, TRUE);
    NEAR(m.pos[2], 0);
    return 1;
}
static int forward_wall_kick_speed_gate(void) {
    const f32 speeds[] = { -1.0f, 0.0f, 9.99f, 10.0f, 10.01f, 16.0f, 16.01f };
    for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(speeds[0]); speed++) {
        for (int freshA = 0; freshA < 2; freshA++) {
            struct MarioState m = fresh();
            mode = 4;
            m.pos[1] = 1000.0f;
            m.vel[1] = 40.0f;
            m.faceAngle[1] = m.intendedYaw = 0;
            m.input = freshA ? INPUT_A_PRESSED : 0;
            m.wallKickForwardStickFrames = 1;
            mario_set_forward_vel(&m, speeds[speed]);
            common_air_action_step(&m, ACT_JUMP_LAND, MARIO_ANIM_SINGLE_JUMP, 0);
            u32 expectedAction = ACT_JUMP;
            if (speeds[speed] > 10.0f) {
                expectedAction = freshA ? ACT_WALL_KICK_AIR : ACT_AIR_HIT_WALL;
            }
            EXPECT_EQ(m.action, expectedAction);
            EXPECT_EQ(m.wallKickChainCount, speeds[speed] > 10.0f && freshA);
        }
    }
    return TRUE;
}

static int buffered_wall_kick_ledge_priority(void) {
    const f32 speeds[] = { 10.0f, 10.01f, 16.0f, 16.01f, 40.0f };
    const s16 coneLimit = 0x8000 - DEGREES(180 - WALL_KICK_DEGREES);
    const s16 offsets[] = { 0, coneLimit - 1, coneLimit, coneLimit + 1 };
    const u32 actions[] = { ACT_JUMP, ACT_DOUBLE_JUMP, ACT_FREEFALL, ACT_LONG_JUMP, ACT_WALL_KICK_AIR };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned action = 0; action < sizeof(actions) / sizeof(*actions); action++) {
            for (int buffer = 0; buffer <= 2; buffer++) {
                for (int stick = 0; stick < 3; stick++) {
                    for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(*speeds); speed++) {
                        for (unsigned angle = 0; angle < sizeof(offsets) / sizeof(*offsets); angle++) {
                            mode = 1;
                            struct MarioState m = fresh();
                            m.action = actions[action];
                            m.faceAngle[1] = m.intendedYaw = 0x4000 + offsets[angle];
                            m.input = INPUT_NONZERO_ANALOG;
                            m.vel[1] = -1.0f;
                            mario_set_forward_vel(&m, speeds[speed]);
                            controller.stickMag = stick ? MARIO_STRONG_STICK_THRESHOLD
                                                        : MARIO_STRONG_STICK_THRESHOLD - 0.01f;
                            mario_update_wall_kick_slide_input(&m);
                            if (stick == 2) {
                                // A still-live strong-stick sample also qualifies after release.
                                controller.stickMag = 20.0f;
                            }
                            m.wallKickInputFrames = buffer;
                            s32 kick = buffer != 0 && stick != 0 && speed != 0 && angle < 2;
                            u32 result =
                                common_air_action_step(&m, ACT_FREEFALL_LAND, MARIO_ANIM_GENERAL_FALL,
                                                       AIR_STEP_CHECK_LEDGE_GRAB);
                            if (result != (u32) (kick ? AIR_STEP_HIT_WALL : AIR_STEP_GRABBED_LEDGE)) {
                                fprintf(stderr,
                                        "cycle=%d action=%u buffer=%d stick=%d speed=%g angle=%d\n",
                                        cycle, actions[action], buffer, stick, speeds[speed],
                                        offsets[angle]);
                            }
                            EXPECT_EQ(result, kick ? AIR_STEP_HIT_WALL : AIR_STEP_GRABBED_LEDGE);
                            EXPECT_EQ(m.action, kick ? ACT_WALL_KICK_AIR : ACT_LEDGE_GRAB);
                            EXPECT_EQ(m.wallKickInputFrames, 0);
                            EXPECT_EQ(stepCalls, 1);
                            if (kick) {
                                EXPECT_EQ(m.wall == &wallA, TRUE);
                                EXPECT_EQ(m.floor == &ground, TRUE);
                                NEAR(m.pos[0], 0.0f);
                                NEAR(m.vel[1], WALL_KICK_VERTICAL_SPEED);
                                EXPECT_EQ(m.wallKickChainCount, 1);
                            } else {
                                EXPECT_EQ(m.floor == &topA, TRUE);
                                NEAR(m.pos[1], 150.0f);
                            }
                        }
                    }
                }
            }
        }
    }
    // A valid contact on a second corner wall must still beat the ledge candidate.
    mode = 2;
    struct MarioState m = fresh();
    m.faceAngle[1] = m.intendedYaw = 0;
    m.vel[1] = -1.0f;
    mario_set_forward_vel(&m, 40.0f);
    mario_update_wall_kick_slide_input(&m);
    m.wallKickInputFrames = 1;
    common_air_action_step(&m, ACT_FREEFALL_LAND, MARIO_ANIM_GENERAL_FALL, AIR_STEP_CHECK_LEDGE_GRAB);
    EXPECT_EQ(m.action, ACT_WALL_KICK_AIR);
    EXPECT_EQ(m.wall == &wallB, TRUE);
    EXPECT_EQ(m.floor == &ground, TRUE);
    // Ordinary upper-wall hits retain their full vertical travel, with either check mode.
    for (int ledgeCheck = 0; ledgeCheck < 2; ledgeCheck++) {
        mode = 4;
        m = fresh();
        m.pos[1] = 1000.0f;
        m.vel[1] = -20.0f;
        m.faceAngle[1] = m.intendedYaw = 0;
        mario_set_forward_vel(&m, 40.0f);
        mario_update_wall_kick_slide_input(&m);
        m.wallKickInputFrames = 1;
        common_air_action_step(&m, ACT_FREEFALL_LAND, MARIO_ANIM_GENERAL_FALL,
                               ledgeCheck ? AIR_STEP_CHECK_LEDGE_GRAB : AIR_STEP_CHECK_NONE);
        EXPECT_EQ(m.action, ACT_WALL_KICK_AIR);
        NEAR(m.pos[1], 980.0f);
        EXPECT_EQ(stepCalls, 1);
    }
    // Burning walls and floor/ceiling contacts retain their higher-priority reactions.
    for (int contact = 0; contact < 3; contact++) {
        mode = 1;
        m = fresh();
        m.faceAngle[1] = m.intendedYaw = 0x4000;
        m.vel[1] = -1.0f;
        mario_set_forward_vel(&m, 40.0f);
        mario_update_wall_kick_slide_input(&m);
        m.wallKickInputFrames = 2;
        if (contact == 0) {
            wallA.type = SURFACE_BURNING;
        } else if (contact == 1) {
            m.pos[1] = -200.0f;
        } else {
            ceilingY = 100.0f;
            m.vel[1] = 1.0f;
        }
        common_air_action_step(&m, ACT_FREEFALL_LAND, MARIO_ANIM_GENERAL_FALL,
                               AIR_STEP_CHECK_LEDGE_GRAB);
        EXPECT_EQ(m.action, contact == 0   ? ACT_LAVA_BOOST
                            : contact == 1 ? ACT_FREEFALL_LAND
                                           : ACT_JUMP);
        EXPECT_EQ(m.wallKickChainCount, 0);
    }
    return TRUE;
}
static int wall_slide_ledge_voice(void) {
    const f32 incoming[] = { -40.0f, -24.01f, -24.0f, -23.99f, -20.0f, -16.0f, -8.0f, -1.0f, 0.0f };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned i = 0; i < sizeof(incoming) / sizeof(incoming[0]); i++) {
            mode = 1;
            struct MarioState m = fresh();
            m.action = ACT_AIR_HIT_WALL;
            m.wallKickContactFrame = gGlobalTimer - 1;
            m.vel[1] = incoming[i];
            act_air_hit_wall(&m);
            EXPECT_EQ(m.action, ACT_LEDGE_GRAB);
            EXPECT_EQ(animation, MARIO_ANIM_GENERAL_FALL);
            EXPECT_EQ(m.actionArg, incoming[i] <= -24.0f ? 0 : ACT_ARG_LEDGE_GRAB_GENTLE);
        }
    }
    return 1;
}
static int wall_window(void) {
    const int speeds[] = { 20, 40, 100, 128 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int s = 0; s < 4; s++) {
            for (int kick = 1; kick <= 9; kick++) {
                for (int metal = 0; metal < 2; metal++) {
                    mode = 4;
                    struct MarioState m = fresh();
                    m.action = ACT_LONG_JUMP;
                    m.faceAngle[1] = 0;
                    m.intendedYaw = 0;
                    m.flags = metal ? MARIO_METAL_CAP : 0;
                    m.forwardVel = speeds[s];
                    m.vel[0] = 0;
                    m.vel[2] = speeds[s];
                    m.pos[1] = 1000;
                    m.input = INPUT_NONZERO_ANALOG | (kick == 1 ? INPUT_A_PRESSED : 0);
                    mario_update_wall_kick_slide_input(&m);
                    act_long_jump(&m);
                    EXPECT_EQ(stepCalls, 1);
                    EXPECT_EQ(bonks, 1);
                    EXPECT_EQ(metalBonks, metal);
                    EXPECT_EQ(kickImpacts, 0);
                    if (kick == 1) {
                        EXPECT_EQ(poseBegins, 1);
                        EXPECT_EQ(m.action, ACT_WALL_KICK_AIR);
                        NEAR(m.vel[1], WALL_KICK_VERTICAL_SPEED);
                        continue;
                    }
                    EXPECT_EQ(m.action, ACT_AIR_HIT_WALL);
                    EXPECT_EQ(animation, MARIO_ANIM_START_WALLKICK);
                    // Same-frame redispatch cannot spend movement or advance the timer.
                    EXPECT_EQ(act_air_hit_wall(&m), FALSE);
                    EXPECT_EQ(m.actionTimer, 1);
                    EXPECT_EQ(stepCalls, 1);
                    for (int update = 2; update <= MIN(kick, 7); update++) {
                        gGlobalTimer++;
                        m.input = update == kick ? INPUT_A_PRESSED : 0;
                        int moves = stepCalls;
                        act_air_hit_wall(&m);
                        if (update < kick && update <= 6) {
                            EXPECT_EQ(m.action, ACT_AIR_HIT_WALL);
                            EXPECT_EQ(bonks, 1);
                            EXPECT_EQ(stepCalls, moves + 1);
                        }
                    }
                    EXPECT_EQ(bonks, 1);
                    EXPECT_EQ(metalBonks, metal);
                    EXPECT_EQ(kickImpacts, 0);
                    if (kick <= 6) {
                        EXPECT_EQ(poseBegins, 1);
                        EXPECT_EQ(m.action, ACT_WALL_KICK_AIR);
                        NEAR(m.vel[1], 52);
                    } else {
                        EXPECT_EQ(poseBegins, 0);
                        EXPECT_EQ(kickImpacts, 0);
                        EXPECT_EQ(bonks, 1);
                        EXPECT_EQ(m.wallKickTimer, 0);
                        EXPECT_EQ(animation, MARIO_ANIM_GENERAL_FALL);
                        EXPECT_EQ(check_wall_kick(&m), FALSE);
                        NEAR(m.forwardVel, -speeds[s] * 0.5f);
                        EXPECT_EQ(m.wallBonkMomentum, TRUE);
                    }
                }
            }
        }
    }
    return 1;
}
static int wall_slide_stick_gate(void) {
    const u32 actions[] = { ACT_LONG_JUMP, ACT_BACKFLIP, ACT_TRIPLE_JUMP, ACT_SPECIAL_TRIPLE_JUMP };
    for (int action = 0; action < 4; action++) {
        for (int input = 0; input < 4; input++) {
            mode = 4;
            struct MarioState m = fresh();
            m.action = actions[action];
            m.pos[1] = 1000;
            m.vel[1] = 40;
            m.actionArg = action >= 2 ? ACT_ARG_TRIPLE_JUMP_BACKWARD : 0;
            m.actionTimer = action ? 13 : 0;
            m.input = INPUT_NONZERO_ANALOG | INPUT_A_PRESSED;
            wallB.normal.z = action ? 1 : -1;
            mario_set_forward_vel(&m, action ? -20.0f : 40.0f);
            // At threshold, below threshold, reversed direction, and neutral.
            controller.stickMag = input == 0   ? MARIO_STRONG_STICK_THRESHOLD
                                  : input == 1 ? MARIO_STRONG_STICK_THRESHOLD - 0.01f
                                               : 64.0f;
            if (input == 2) {
                m.intendedYaw = action ? 0 : 0x8000;
            } else {
                m.intendedYaw = action ? 0x8000 : 0;
            }
            if (input == 3) {
                m.input = INPUT_A_PRESSED;
            }
            mario_update_wall_kick_slide_input(&m);
            if (action == 0) {
                act_long_jump(&m);
            } else if (action == 1) {
                act_backflip(&m);
            } else if (action == 2) {
                act_triple_jump(&m);
            } else {
                act_special_triple_jump(&m);
            }
            if (input == 0) {
                EXPECT_EQ(m.action, ACT_WALL_KICK_AIR);
                EXPECT_EQ(poseBegins, 1);
            } else {
                EXPECT_EQ(m.action != ACT_AIR_HIT_WALL && m.action != ACT_WALL_KICK_AIR, TRUE);
                EXPECT_EQ(poseBegins, 0);
            }
        }
    }
    for (int action = 0; action < 4; action++) {
        for (int age = 0; age <= 4; age++) {
            mode = 4;
            struct MarioState m = fresh();
            m.action = actions[action];
            m.pos[1] = 1000;
            m.vel[1] = 40;
            m.actionArg = action >= 2 ? ACT_ARG_TRIPLE_JUMP_BACKWARD : 0;
            m.actionTimer = action ? 13 : 0;
            wallB.normal.z = action ? 1 : -1;
            mario_set_forward_vel(&m, action ? -20.0f : 40.0f);
            m.input = INPUT_NONZERO_ANALOG;
            m.intendedYaw = action ? 0x8000 : 0;
            mario_update_wall_kick_slide_input(&m);
            for (int elapsed = 0; elapsed < age; elapsed++) {
                m.input = 0;
                mario_update_wall_kick_slide_input(&m);
            }
            m.input = INPUT_A_PRESSED;
            if (action == 0) {
                act_long_jump(&m);
            } else if (action == 1) {
                act_backflip(&m);
            } else if (action == 2) {
                act_triple_jump(&m);
            } else {
                act_special_triple_jump(&m);
            }
            EXPECT_EQ(m.action == ACT_WALL_KICK_AIR, age <= WALL_KICK_STICK_LOOKBACK_FRAMES);
            EXPECT_EQ(poseBegins, age <= WALL_KICK_STICK_LOOKBACK_FRAMES);
        }
    }
    return 1;
}
static int backward_air_wall_speed(void) {
    for (int repeat = 0; repeat < 3; repeat++) {
        mode = 4;
        struct MarioState m = fresh();
        wallB.normal.z = 1;
        m.action = ACT_JUMP_KICK;
        m.pos[1] = 1000;
        m.vel[1] = 40;
        m.input = INPUT_NONZERO_ANALOG;
        m.intendedYaw = 0x8000;
        mario_set_forward_vel(&m, -28.0f);
        EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL), AIR_STEP_NONE);
        NEAR(m.forwardVel, 0);
        NEAR(m.vel[0], 0);
        NEAR(m.vel[2], 0);
        NEAR(m.slideVelX, 0);
        NEAR(m.slideVelZ, 0);
        EXPECT_EQ(m.wall == NULL, TRUE); // This contact has no bonk reference.
    }
    // A free-air step and a wall touched while moving away keep their speed.
    mode = 0;
    struct MarioState m = fresh();
    m.action = ACT_JUMP_KICK;
    m.pos[1] = 1000;
    m.vel[1] = 40;
    mario_set_forward_vel(&m, -28.0f);
    perform_air_step_with_remainder(&m, 0, NULL);
    NEAR(m.forwardVel, -28);
    mode = 4;
    m = fresh();
    wallB.normal.z = -1;
    m.action = ACT_JUMP_KICK;
    m.pos[1] = 1000;
    m.vel[1] = 40;
    mario_set_forward_vel(&m, -28.0f);
    perform_air_step_with_remainder(&m, 0, NULL);
    NEAR(m.forwardVel, -28);
    // Early backflip contact brakes, but a later fresh backward approach can kick.
    mode = 4;
    m = fresh();
    wallB.normal.z = 1;
    m.action = ACT_BACKFLIP;
    m.pos[1] = 1000;
    m.vel[1] = 40;
    mario_set_forward_vel(&m, -20.0f);
    act_backflip(&m);
    NEAR(m.forwardVel, 0);
    EXPECT_EQ(m.action, ACT_BACKFLIP);
    m.actionTimer = 13;
    m.input = INPUT_NONZERO_ANALOG | INPUT_A_PRESSED;
    m.intendedYaw = 0x8000;
    mario_set_forward_vel(&m, -20.0f);
    mario_update_wall_kick_slide_input(&m);
    act_backflip(&m);
    EXPECT_EQ(m.action, ACT_WALL_KICK_AIR);
    return 1;
}
static int backward_triple_wall_kick(void) {
    EXPECT_EQ(BACKWARD_JUMP_WALL_KICK_DELAY_FRAMES, 13);
    const u32 actions[] = { ACT_TRIPLE_JUMP, ACT_SPECIAL_TRIPLE_JUMP, ACT_BACKFLIP };
    const f32 speeds[] = { 2, 12, 20, 40, 100 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int special = 0; special < 3; special++) {
            for (int speed = 0; speed < 5; speed++) {
                for (int kick = 1; kick <= 9; kick++) {
                    mode = 4;
                    struct MarioState m = fresh();
                    wallB.normal.z = 1;
                    f32 incomingSpeed = special == 2 ? MAX(2.0f, 16.0f - 4.0f * speed) : speeds[speed];
                    m.action = actions[special];
                    m.actionArg = special == 2 ? 0 : ACT_ARG_TRIPLE_JUMP_BACKWARD;
                    m.pos[1] = 1000;
                    m.input = INPUT_NONZERO_ANALOG;
                    m.intendedYaw = 0x8000;
                    m.vel[1] = 40;
                    mario_set_forward_vel(&m, -incomingSpeed);
                    for (int frame = 1; frame <= 14; frame++) {
                        mode = frame == 14 ? 4 : 0; // First wall contact occurs when the kick opens.
                        gGlobalTimer++;
                        m.input =
                            INPUT_NONZERO_ANALOG | (frame == 14 && kick == 1 ? INPUT_A_PRESSED : 0);
                        mario_update_wall_kick_slide_input(&m);
                        if (special == 2) {
                            act_backflip(&m);
                        } else if (special) {
                            act_special_triple_jump(&m);
                        } else {
                            act_triple_jump(&m);
                        }
                        EXPECT_EQ(stepCalls, frame);
                        if (frame <= 13) {
                            EXPECT_EQ(m.action, actions[special]);
                            EXPECT_EQ(bonks, 0);
                            NEAR(m.forwardVel, -incomingSpeed);
                        }
                    }
                    EXPECT_EQ(bonks, 1);
                    EXPECT_EQ(animation, kick == 1 ? MARIO_ANIM_SLIDEJUMP : MARIO_ANIM_START_WALLKICK);
                    if (kick > 1) {
                        EXPECT_EQ(m.action, ACT_AIR_HIT_WALL);
                        act_air_hit_wall(&m);
                        EXPECT_EQ(m.actionTimer, 1);
                        EXPECT_EQ(stepCalls, 14);
                        for (int update = 2; update <= MIN(kick, 7); update++) {
                            gGlobalTimer++;
                            m.input = update == kick ? INPUT_A_PRESSED : 0;
                            act_air_hit_wall(&m);
                            if (update < kick && update <= 6) {
                                EXPECT_EQ(m.action, ACT_AIR_HIT_WALL);
                                EXPECT_EQ(animation, MARIO_ANIM_START_WALLKICK);
                            }
                        }
                    }
                    if (kick <= 6) {
                        EXPECT_EQ(m.action, ACT_WALL_KICK_AIR);
                        EXPECT_EQ(poseBegins, 1);
                        EXPECT_EQ(animation, MARIO_ANIM_SLIDEJUMP);
                        NEAR(m.vel[1], 52);
                        EXPECT_EQ(m.vel[2] > 0, TRUE);
                        NEAR(m.forwardVel, MAX(incomingSpeed, 28) * WALL_KICK_MOMENTUM_RETENTION);
                    } else {
                        EXPECT_EQ(m.action == ACT_SOFT_BONK || m.action == ACT_BACKWARD_AIR_KB, TRUE);
                        EXPECT_EQ(check_wall_kick(&m), FALSE);
                    }
                    EXPECT_EQ(bonks, 1);
                }
            }
        }
    }
    // An A press during the locked thirteenth update cannot kick or queue a kick.
    for (int action = 0; action < 3; action++) {
        mode = 4;
        struct MarioState m = fresh();
        wallB.normal.z = 1;
        m.action = actions[action];
        m.actionArg = action == 2 ? 0 : ACT_ARG_TRIPLE_JUMP_BACKWARD;
        m.pos[1] = 1000;
        m.vel[1] = 40;
        m.input = INPUT_NONZERO_ANALOG;
        m.intendedYaw = 0x8000;
        mario_set_forward_vel(&m, -20);
        for (int frame = 1; frame <= 14; frame++) {
            mode = frame == 14 ? 4 : 0;
            gGlobalTimer++;
            m.input = INPUT_NONZERO_ANALOG | (frame == 13 ? INPUT_A_PRESSED : 0);
            mario_update_wall_kick_slide_input(&m);
            if (action == 2) {
                act_backflip(&m);
            } else if (action) {
                act_special_triple_jump(&m);
            } else {
                act_triple_jump(&m);
            }
            if (frame <= 13) {
                EXPECT_EQ(m.action, actions[action]);
            }
        }
        EXPECT_EQ(m.action, ACT_AIR_HIT_WALL);
        EXPECT_EQ(m.actionTimer, 1);
        EXPECT_EQ(poseBegins, 0);
        gGlobalTimer++;
        m.input = INPUT_A_PRESSED;
        act_air_hit_wall(&m);
        EXPECT_EQ(m.action, ACT_WALL_KICK_AIR);
        EXPECT_EQ(poseBegins, 1);
    }
    // A fresh approach press from the preceding update kicks on contact once.
    for (int action = 0; action < 3; action++) {
        for (int buffered = 0; buffered < 2; buffered++) {
            mode = 4;
            struct MarioState m = fresh();
            wallB.normal.z = 1;
            m.action = actions[action];
            m.actionArg = action == 2 ? 0 : ACT_ARG_TRIPLE_JUMP_BACKWARD;
            m.actionTimer = 13;
            m.pos[1] = 1000;
            m.vel[1] = 40;
            m.input = INPUT_NONZERO_ANALOG;
            m.intendedYaw = 0x8000;
            m.wallKickInputFrames = buffered;
            mario_set_forward_vel(&m, -20);
            mario_update_wall_kick_slide_input(&m);
            if (action == 2) {
                act_backflip(&m);
            } else if (action) {
                act_special_triple_jump(&m);
            } else {
                act_triple_jump(&m);
            }
            EXPECT_EQ(m.action, buffered ? ACT_WALL_KICK_AIR : ACT_AIR_HIT_WALL);
            EXPECT_EQ(poseBegins, buffered);
            EXPECT_EQ(m.wallKickInputFrames, 0);
        }
    }
    // Keep the backward-travel and angle limits, and exclude other actions.
    const u32 excluded[] = { ACT_JUMP, ACT_DOUBLE_JUMP, ACT_LONG_JUMP, ACT_FLYING_TRIPLE_JUMP };
    for (unsigned i = 0; i < sizeof(excluded) / sizeof(excluded[0]); i++) {
        struct MarioState m = fresh();
        m.action = excluded[i];
        m.actionArg = ACT_ARG_TRIPLE_JUMP_BACKWARD;
        m.actionTimer = 14;
        m.forwardVel = -40;
        struct WallCollisionData walls = { .numWalls = 1, .walls = { &wallB } };
        wallB.normal.z = 1;
        EXPECT_EQ(bonk_or_hit_lava_wall(&m, &walls), AIR_STEP_NONE);
    }
    for (int angle = 0; angle <= 70; angle += 35) {
        struct MarioState m = fresh();
        m.action = ACT_TRIPLE_JUMP;
        m.actionArg = ACT_ARG_TRIPLE_JUMP_BACKWARD;
        m.actionTimer = 14;
        m.forwardVel = -40;
        wallB.normal.x = sins(DEGREES(angle));
        wallB.normal.z = coss(DEGREES(angle));
        struct WallCollisionData walls = { .numWalls = 1, .walls = { &wallB } };
        // Contact on update 13 must still be blocked, not just absent from the fixture.
        m.actionTimer = 13;
        EXPECT_EQ(bonk_or_hit_lava_wall(&m, &walls), AIR_STEP_NONE);
        m.wall = NULL;
        m.actionTimer = 14;
        EXPECT_EQ(bonk_or_hit_lava_wall(&m, &walls), angle < 60 ? AIR_STEP_HIT_WALL : AIR_STEP_NONE);
        m.wall = NULL;
        m.forwardVel = -16;
        EXPECT_EQ(bonk_or_hit_lava_wall(&m, &walls), angle < 60 ? AIR_STEP_HIT_WALL : AIR_STEP_NONE);
        m.wall = NULL;
        m.forwardVel = 0;
        EXPECT_EQ(bonk_or_hit_lava_wall(&m, &walls), AIR_STEP_NONE);
        m.wall = NULL;
        m.action = ACT_BACKFLIP;
        m.actionArg = 0;
        m.forwardVel = -16;
        EXPECT_EQ(bonk_or_hit_lava_wall(&m, &walls), angle < 60 ? AIR_STEP_HIT_WALL : AIR_STEP_NONE);
        m.wall = NULL;
        m.forwardVel = 0;
        EXPECT_EQ(bonk_or_hit_lava_wall(&m, &walls), AIR_STEP_NONE);
    }
    return 1;
}

static int wind_catch(void) {
    const f32 floors[] = { -8089, -7065, -3789 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int i = 0; i < 3; i++) {
            mode = 0;
            struct MarioState m = fresh();
            ground.type = SURFACE_VERTICAL_WIND;
            ground.originOffset = m.floorHeight = floors[i];
            m.pos[1] = i == 2 ? -1700 : -4490;
            m.vel[0] = m.vel[2] = 0;
            m.vel[1] = -92;
            int caught = 0;
            for (int update = 0; update < 160; update++) {
                mario_update_wind_death_grace(&m);
                check_death_barrier(&m);
                EXPECT_EQ(warps, 0);
                perform_air_step_with_remainder(&m, 0, NULL);
                EXPECT_EQ(m.pos[1] >= floors[i] + 2048, TRUE);
                if (m.vel[1] >= 0) {
                    caught = 1;
                    break;
                }
            }
            EXPECT_EQ(caught, 1);
        }
    }
    return 1;
}
static int wind_boundaries(void) {
    mode = 0;
    struct MarioState m = fresh();
    ground.type = SURFACE_VERTICAL_WIND;
    ground.originOffset = m.floorHeight = -3789;
    m.pos[1] = -1742;
    m.vel[1] = -92;
    EXPECT_EQ(mario_is_in_vertical_wind(&m), FALSE);
    mario_update_wind_death_grace(&m);
    check_death_barrier(&m);
    EXPECT_EQ(warps, 0);
    m = fresh();
    ground.type = SURFACE_DEATH_PLANE;
    ground.originOffset = m.floorHeight = -3789;
    m.pos[1] = -1700;
    m.vel[0] = m.vel[2] = 0;
    m.vel[1] = -92;
    perform_air_step_with_remainder(&m, 0, NULL);
    check_death_barrier(&m);
    EXPECT_EQ(warps, 1);
    m = fresh();
    ground.type = SURFACE_VERTICAL_WIND;
    ground.originOffset = m.floorHeight = -3789;
    m.action = ACT_GROUND_POUND;
    m.pos[1] = -1700;
    m.vel[0] = m.vel[2] = 0;
    m.vel[1] = -92;
    mario_update_wind_death_grace(&m);
    perform_air_step_with_remainder(&m, 0, NULL);
    check_death_barrier(&m);
    EXPECT_EQ(warps, 0);
    m = fresh();
    ground.type = SURFACE_VERTICAL_WIND;
    ground.originOffset = m.floorHeight = -8089;
    m.pos[1] = 500;
    m.windCatchActive = TRUE;
    apply_vertical_wind(&m, -92);
    EXPECT_EQ(m.windCatchActive, FALSE);
    m.pos[1] = 0;
    m.floor = &topA;
    m.windCatchActive = TRUE;
    apply_vertical_wind(&m, -92);
    EXPECT_EQ(m.windCatchActive, FALSE);
    return 1;
}

static int ignore_wind_death_boundary(void) {
    struct MarioState m = fresh();
    ground.type = SURFACE_VERTICAL_WIND;
    ground.originOffset = m.floorHeight = -7065;
    m.pos[1] = m.floorHeight + 2047;
    m.windDeathGraceTimer = 30;
    m.windColumnActive = TRUE;
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    sTestGameOptions[GO_IGNORE_WIND] = TRUE;
    mario_update_wind_death_grace(&m);
    EXPECT_EQ(m.windDeathGraceTimer, 0);
    EXPECT_EQ(m.windColumnActive, FALSE);
    check_death_barrier(&m);
    EXPECT_EQ(warps, 1); // Ignoring force does not remove the death plane.
    sTestGameOptions[GO_IGNORE_WIND] = FALSE;
    warps = 0;
    mario_update_wind_death_grace(&m);
    EXPECT_EQ(m.windDeathGraceTimer, 30);
    check_death_barrier(&m);
    EXPECT_EQ(warps, 0);
    sTestGameOptions[GO_CHEATS_ENABLED] = FALSE;
    return TRUE;
}

static int wind_death_grace(void) {
    const u32 caps[] = { 0, MARIO_METAL_CAP, MARIO_VANISH_CAP, MARIO_WING_CAP };
    const f32 floors[] = { -8089, -7065, -3789 };
    for (unsigned cap = 0; cap < sizeof(caps) / sizeof(caps[0]); cap++) {
        for (unsigned column = 0; column < sizeof(floors) / sizeof(floors[0]); column++) {
            for (int cycle = 0; cycle < 3; cycle++) {
                struct MarioState m = fresh();
                ground.type = SURFACE_VERTICAL_WIND;
                ground.originOffset = m.floorHeight = floors[column];
                m.pos[1] = m.floorHeight + 2047;
                m.flags = caps[cap];
                for (int frame = 0; frame < 30; frame++) {
                    // Action changes and direct moves below the death boundary
                    // neither restart entry grace nor turn it into force immunity.
                    m.action = frame % 2 ? ACT_GROUND_POUND : ACT_FREEFALL;
                    mario_update_wind_death_grace(&m);
                    EXPECT_EQ(m.windDeathGraceTimer, 30 - frame);
                    check_death_barrier(&m);
                    EXPECT_EQ(warps, 0);
                }
                mario_update_wind_death_grace(&m);
                EXPECT_EQ(m.windDeathGraceTimer, 0);
                check_death_barrier(&m);
                EXPECT_EQ(warps, 1);
                for (int frame = 0; frame < 40; frame++) {
                    mario_update_wind_death_grace(&m);
                    EXPECT_EQ(m.windDeathGraceTimer, 0);
                }
                // Ordinary floor membership rearms a later genuine reentry.
                ground.type = SURFACE_DEATH_PLANE;
                mario_update_wind_death_grace(&m);
                EXPECT_EQ(m.windColumnActive, FALSE);
                ground.type = SURFACE_VERTICAL_WIND;
                warps = 0;
                mario_update_wind_death_grace(&m);
                EXPECT_EQ(m.windDeathGraceTimer, 30);
                check_death_barrier(&m);
                EXPECT_EQ(warps, 0);
            }
        }
    }
    return 1;
}

static int wind_grace_exit(void) {
    for (int exit = 0; exit < 4; exit++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            struct MarioState m = fresh();
            ground.type = SURFACE_VERTICAL_WIND;
            ground.originOffset = m.floorHeight = -3789;
            m.pos[1] = -1742;
            mario_update_wind_death_grace(&m);
            EXPECT_EQ(m.windDeathGraceTimer, 30);
            if (exit == 0) {
                ground.type = SURFACE_DEATH_PLANE;
                // Even before the next timer sample, leaving cancels protection.
                check_death_barrier(&m);
                EXPECT_EQ(warps, 1);
            } else if (exit == 1) {
                m.pos[1] = 500;
            } else if (exit == 2) {
                m.floor = NULL;
            } else {
                m.pos[1] = m.floorHeight - 1;
                check_death_barrier(&m);
                EXPECT_EQ(warps, 1);
            }
            mario_update_wind_death_grace(&m);
            EXPECT_EQ(m.windColumnActive, FALSE);
            EXPECT_EQ(m.windDeathGraceTimer, 0);
            // A fresh column entry earns a full second again.
            ground.type = SURFACE_VERTICAL_WIND;
            m.floor = &ground;
            m.pos[1] = -1742;
            warps = 0;
            mario_update_wind_death_grace(&m);
            EXPECT_EQ(m.windDeathGraceTimer, 30);
            check_death_barrier(&m);
            EXPECT_EQ(warps, 0);
        }
    }
    return 1;
}

static int crawl_platform_side(void) {
    // Actual BITFS inverted-pyramid side (vertices 2,1,4). At a 60-degree
    // weight tilt it becomes a nearly vertical floor instead of a ceiling.
    Vec3f a, b, n;
    for (int axis = 0; axis < 3; axis++) {
        a[axis] = tiltVertices[1][axis] - tiltVertices[2][axis];
        b[axis] = tiltVertices[4][axis] - tiltVertices[2][axis];
    }
    n[0] = a[1] * b[2] - a[2] * b[1];
    n[1] = a[2] * b[0] - a[0] * b[2];
    n[2] = a[0] * b[1] - a[1] * b[0];
    f32 length = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
    for (int axis = 0; axis < 3; axis++) {
        n[axis] /= length;
    }
    f32 tiltedY = n[1] * .5f - n[0] * .8660254f;
    if (tiltedY < 0) {
        tiltedY = n[1] * .5f + n[0] * .8660254f;
    }
    EXPECT_EQ(tiltedY > 0.01f && tiltedY < COS73, TRUE);
    const u32 actions[] = { ACT_START_CRAWLING, ACT_CRAWLING, ACT_STOP_CRAWLING, ACT_CROUCHING,
                            ACT_CROUCH_SLIDE };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned i = 0; i < sizeof(actions) / sizeof(actions[0]); i++) {
            struct MarioState m = fresh();
            mode = 5;
            m.action = actions[i];
            m.pos[1] = 0;
            m.floor = &ground;
            topA.normal.y = tiltedY;
            topA.originOffset = 40;
            Vec3f target = { 5, 0, 5 };
            EXPECT_EQ(perform_ground_quarter_step(&m, target, FALSE), GROUND_STEP_HIT_WALL_STOP_QSTEPS);
            NEAR(m.pos[1], 0);
            EXPECT_EQ(m.floor == &ground, TRUE);
            // The same tilted platform's genuine top remains usable.
            topA.normal.y = .8660254f;
            EXPECT_EQ(perform_ground_quarter_step(&m, target, FALSE), GROUND_STEP_NONE);
            NEAR(m.pos[1], 40);
            EXPECT_EQ(m.floor == &topA, TRUE);
        }
    }
    return 1;
}

static int rock_downhill(void) {
    // The three screenshots, using the corresponding JRB rock placements.
    const s32 positions[][3] = { { -5347, 1036, 2143 }, { -5488, 1168, 949 }, { -4109, 1313, -18 } };
    const u32 actions[] = { ACT_DIVE_SLIDE, ACT_STOMACH_SLIDE, ACT_HOLD_STOMACH_SLIDE,
                            ACT_SLIDE_KICK_SLIDE };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned a = 0; a < sizeof(actions) / sizeof(actions[0]); a++) {
            for (unsigned p = 0; p < sizeof(positions) / sizeof(positions[0]); p++) {
                for (int sign = -1; sign <= 1; sign += 2) {
                    struct MarioState m = fresh();
                    mode = 6;
                    const s32 *v = positions[p];
                    const s32 *placement = rockTestPlacements[p];
                    place_rock(placement[0], placement[1], placement[2], placement[3]);
                    m.action = actions[a];
                    m.waterLevel = 1024;
                    m.pos[0] = v[0];
                    m.pos[1] = v[1];
                    m.pos[2] = v[2];
                    m.floorHeight = find_floor(m.pos[0], m.pos[1], m.pos[2], &m.floor);
                    EXPECT_EQ(fabsf(m.pos[1] - m.floorHeight) < 1, TRUE);
                    EXPECT_EQ(m.floor->normal.y < COS73, TRUE);
                    m.pos[1] = m.floorHeight;
                    struct Surface *initial = m.floor;
                    int entered = FALSE;
                    for (int frame = 0; frame < 30; frame++) {
                        EXPECT_EQ(check_common_moving_cancels(&m), FALSE);
                        f32 horizontal = sqrtf(sqr(initial->normal.x) + sqr(initial->normal.z));
                        m.vel[0] = 86.6f * initial->normal.x / horizontal;
                        m.vel[2] = 86.6f * initial->normal.z / horizontal;
                        m.forwardVel = sign * 86.6f;
                        m.vel[1] = 0;
                        Vec3f old;
                        vec3f_copy(old, m.pos);
                        s32 step = perform_ground_step(&m);
                        EXPECT_EQ(step != GROUND_STEP_HIT_WALL, TRUE);
                        EXPECT_EQ(m.pos[1] < old[1], TRUE);
                        EXPECT_EQ(sqr(m.pos[0] - old[0]) + sqr(m.pos[2] - old[2]) > 0, TRUE);
                        EXPECT_EQ(m.action, actions[a]);
                        if (m.pos[1] < m.waterLevel - 100) {
                            EXPECT_EQ(check_common_moving_cancels(&m), TRUE);
                            EXPECT_EQ(m.action, ACT_WATER_PLUNGE);
                            entered = TRUE;
                            break;
                        }
                        if (step == GROUND_STEP_LEFT_GROUND) {
                            // The slide action's normal response is freefall. Exercise
                            // actual air movement before testing the water-entry gate.
                            set_mario_action(&m, ACT_FREEFALL, 0);
                            for (int airFrame = 0; airFrame < 30; airFrame++) {
                                perform_air_step(&m, 0);
                                if (check_common_moving_cancels(&m)) {
                                    EXPECT_EQ(m.action, ACT_WATER_PLUNGE);
                                    entered = TRUE;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    EXPECT_EQ(entered, TRUE);
                }
            }
        }
    }
    return 1;
}

static int rock_seams_and_exit(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        struct MarioState m = fresh();
        mode = 6;
        place_rock(0, 1500, 0, 0);
        m.action = ACT_DIVE_SLIDE;
        // Start within the upper sloped face, then descend across its seam
        // onto face (9,3,11) and finally leave onto the supplied lower floor.
        int face = 14;
        EXPECT_EQ(rockFaces[face].normal.y > COS73, TRUE);
        EXPECT_EQ(rockFaces[17].normal.y < COS73, TRUE);
        for (int i = 0; i < 3; i++) {
            m.pos[i] = 0;
        }
        for (int corner = 0; corner < 3; corner++) {
            for (int i = 0; i < 3; i++) {
                m.pos[i] += rockWorldVertices[rockTriangles[face][corner]][i] / 3;
            }
        }
        m.floorHeight = find_floor(m.pos[0], m.pos[1], m.pos[2], &m.floor);
        m.pos[1] = m.floorHeight;
        EXPECT_EQ(m.floor == &rockFaces[face], TRUE);
        int seams = 0, left = FALSE;
        for (int frame = 0; frame < 120; frame++) {
            f32 horizontal = sqrtf(sqr(m.floor->normal.x) + sqr(m.floor->normal.z));
            EXPECT_EQ(horizontal > 0, TRUE);
            m.vel[0] = 40 * m.floor->normal.x / horizontal;
            m.vel[2] = 40 * m.floor->normal.z / horizontal;
            struct Surface *previous = m.floor;
            f32 oldY = m.pos[1];
            s32 step = perform_ground_step(&m);
            EXPECT_EQ(step != GROUND_STEP_HIT_WALL, TRUE);
            EXPECT_EQ(m.pos[1] <= oldY, TRUE);
            if (m.floor != previous) {
                seams++;
            }
            if (step == GROUND_STEP_LEFT_GROUND) {
                left = TRUE;
                break;
            }
        }
        EXPECT_EQ(seams >= 2, TRUE);
        EXPECT_EQ(left, TRUE);
    }
    return 1;
}

static int steep_slide_protection(void) {
    const u32 actions[] = { ACT_DIVE_SLIDE,       ACT_STOMACH_SLIDE, ACT_HOLD_STOMACH_SLIDE,
                            ACT_SLIDE_KICK_SLIDE, ACT_CRAWLING,      ACT_CROUCH_SLIDE };
    for (unsigned a = 0; a < sizeof(actions) / sizeof(actions[0]); a++) {
        struct MarioState m = fresh();
        mode = 5;
        m.action = actions[a];
        m.pos[1] = m.floorHeight = 0;
        topA.normal.y = .283f;
        topA.originOffset = 1;
        Vec3f p = { 5, 0, 5 };
        EXPECT_EQ(perform_ground_quarter_step(&m, p, FALSE), GROUND_STEP_HIT_WALL);
        NEAR(m.pos[1], 0);
        m.floor = &topA; // Even the currently supporting face cannot snap upward.
        EXPECT_EQ(perform_ground_quarter_step(&m, p, FALSE), GROUND_STEP_HIT_WALL);
        NEAR(m.pos[1], 0);
        topA.originOffset = -10;
        m.floorHeight = -10; // Unsupported: must not acquire a steep face.
        EXPECT_EQ(perform_ground_quarter_step(&m, p, FALSE), GROUND_STEP_HIT_WALL);
        NEAR(m.pos[1], 0);
        m.floorHeight = 0;
        topA.originOffset = 0;
        EXPECT_EQ(perform_ground_quarter_step(&m, p, FALSE), GROUND_STEP_NONE);
        NEAR(m.pos[1], 0);
        m.pos[0] = m.pos[2] = 0;
        topA.originOffset = -10;
        ceilingY = 149;
        EXPECT_EQ(perform_ground_quarter_step(&m, p, FALSE), GROUND_STEP_HIT_WALL);
        NEAR(m.pos[1], 0);
        topA.originOffset = -101;
        ceilingY = 10000;
        EXPECT_EQ(perform_ground_quarter_step(&m, p, FALSE), GROUND_STEP_LEFT_GROUND);
        NEAR(m.pos[1], 0);
        m = fresh();
        m.action = actions[a];
        m.pos[1] = m.floorHeight = 0;
        mode = 7;
        EXPECT_EQ(perform_ground_quarter_step(&m, p, FALSE), GROUND_STEP_HIT_WALL);
        mode = 8;
        ground.originOffset = 0;
        m.faceAngle[1] = 0x4000;
        p[2] = 0;
        EXPECT_EQ(perform_ground_quarter_step(&m, p, TRUE), GROUND_STEP_HIT_WALL_CONTINUE_QSTEPS);
        NEAR(m.pos[0], 0);
        NEAR(m.pos[1], 0);
        mode = 5;
        m.action = ACT_WALKING;
        topA.normal.y = .283f;
        topA.originOffset = 40;
        EXPECT_EQ(perform_ground_quarter_step(&m, p, FALSE), GROUND_STEP_NONE);
        NEAR(m.pos[1], 40);
    }
    return 1;
}

static int backward_butt_slide_steep_jump(void) {
    struct MarioState m = fresh();
    mode = 0;
    ground.normal.y = 0.8f;
    ground.normal.z = -0.6f;
    m.action = ACT_STEEP_JUMP;
    m.actionArg = ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL;
    m.faceAngle[1] = (s16) 0x8000;
    m.forwardVel = -24.0f;
    m.vel[1] = 42.0f;
    EXPECT_EQ(act_steep_jump(&m), FALSE);
    EXPECT_EQ(m.vel[2] <= 0.001f, 1);
    EXPECT_EQ(m.forwardVel >= -0.001f, 1);

    m = fresh();
    mode = 0;
    ground.normal.y = 0.8f;
    ground.normal.z = -0.6f;
    m.action = ACT_STEEP_JUMP;
    m.faceAngle[1] = (s16) 0x8000;
    m.forwardVel = -24.0f;
    m.vel[1] = 42.0f;
    EXPECT_EQ(act_steep_jump(&m), FALSE);
    EXPECT_EQ(m.vel[2] > 0.0f, 1);
    return 1;
}

static int low_statue_step(void) {
    // Physics Lab's full plinth has a 40-unit rear face. Its lower wall
    // probe reaches 24 units out while its upper probe clears the face.
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int variant = 0; variant < 4; variant++) {
            mode = 10;
            struct MarioState m = fresh();
            ground.originOffset = m.floorHeight = 0;
            topA.originOffset = variant == 2 ? 80 : 40;
            stepFloorEnabled = variant != 1;
            wallA.normal.x = 0;
            wallA.normal.z = 1;
            wallA.originOffset = 0;
            wallA.vertex1[1] = wallA.vertex2[1] = wallA.vertex3[1] = topA.originOffset;
            m.pos[2] = 30;
            m.vel[0] = 0;
            m.vel[2] = -40;
            m.action = ACT_WALKING;
            if (variant == 3) {
                ceilingY = 180;
            }
            perform_ground_step(&m);
            if (variant == 0) {
                EXPECT_EQ(m.pos[2] <= 0, TRUE);
                NEAR(m.pos[1], 40);
                EXPECT_EQ(m.floor == &topA, TRUE);
            } else {
                EXPECT_EQ(m.pos[2] > 0, TRUE);
                NEAR(m.pos[1], 0);
                EXPECT_EQ(m.floor == &ground, TRUE);
            }
        }
    }
    return 1;
}
static int crouch_slide_edge_step(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        mode = 9;
        struct MarioState m = fresh();
        m.action = ACT_CROUCH_SLIDE;
        ground.originOffset = 0;
        topB.originOffset = -300;
        m.floorHeight = 0;
        m.pos[2] = 20;
        m.vel[0] = 0;
        m.vel[2] = 80;
        EXPECT_EQ(perform_ground_step_stop_at_ledge(&m), GROUND_STEP_LEFT_GROUND);
        EXPECT_EQ(m.pos[2] <= 50, TRUE);
        EXPECT_EQ(m.pos[2] > 20, TRUE);
        EXPECT_EQ(m.floor == &ground, TRUE);
        NEAR(m.floorHeight, 0);
        m = fresh();
        m.action = ACT_CROUCH_SLIDE;
        ground.originOffset = 0;
        topB.originOffset = -300;
        m.floorHeight = 0;
        m.pos[2] = 20;
        m.vel[0] = 0;
        m.vel[2] = 80;
        EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_LEFT_GROUND);
        EXPECT_EQ(m.pos[2] > 50, TRUE);
        EXPECT_EQ(m.floor == &topB, TRUE);
    }
    return 1;
}
static int grounded_ledge_anchors(void) {
    // A ten-unit-wide lip cannot support the anchor twenty units behind its wall.
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int route = 0; route < 3; route++) {
            for (int invalid = 0; invalid < 5; invalid++) {
                struct MarioState m = fresh();
                mode = 12;
                ledgeAnchorVariant = invalid < 2 ? invalid : 2;
                if (route == 0) {
                    m.action = ACT_WALKING;
                } else if (route == 1) {
                    m.action = ACT_CRAWLING;
                } else {
                    m.action = ACT_CROUCH_SLIDE;
                }
                m.pos[1] = m.floorHeight = 0;
                m.pos[2] = 49;
                m.floor = &ground;
                ground.originOffset = 0;
                wallB.normal.z = 1;
                m.faceAngle[1] = 0;
                m.vel[0] = 0;
                m.vel[2] = 2;
                m.forwardVel = 2;
                ledgeAnchorHeight = invalid == 3 ? -101 : 0;
                if (invalid == 2) {
                    topA.normal.y = COS25 - .01f;
                }
                if (invalid == 4) {
                    ceilingY = 159;
                }
                u32 originalAction = m.action;
                EXPECT_EQ(check_ledge_climb_down_internal(&m, route != 0, route == 2), FALSE);
                EXPECT_EQ(m.action, originalAction);
                NEAR(m.pos[2], 49);
                NEAR(m.pos[1], 0);
                EXPECT_EQ(m.floor == &ground, TRUE);
                NEAR(m.floorHeight, 0);
                EXPECT_EQ(m.faceAngle[1], 0);
            }
        }
    }
    // Gentle/slightly lower tops are committed with their actual floor and height.
    for (int route = 0; route < 3; route++) {
        for (int lower = 0; lower < 2; lower++) {
            struct MarioState m = fresh();
            mode = 12;
            ledgeAnchorVariant = 2;
            ledgeAnchorHeight = lower ? -100 : -10;
            m.pos[1] = m.floorHeight = 0;
            m.pos[2] = 49;
            ground.originOffset = 0;
            wallB.normal.z = 1;
            m.faceAngle[1] = 0;
            m.vel[0] = 0;
            m.vel[2] = 2;
            m.forwardVel = 2;
            topA.normal.y = COS25;
            ceilingY = ledgeAnchorHeight + 160;
            EXPECT_EQ(check_ledge_climb_down_internal(&m, route != 0, route == 2), TRUE);
            EXPECT_EQ(m.action, ACT_LEDGE_CLIMB_DOWN);
            NEAR(m.pos[2], route == 2 ? 23 : 31);
            EXPECT_EQ(m.floor == &topA, TRUE);
            NEAR(m.pos[1], ledgeAnchorHeight);
            NEAR(m.floorHeight, ledgeAnchorHeight);
        }
    }
    // Invalid seam candidates cannot erase a valid wall. Valid ties keep the first.
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int reverse = 0; reverse < 2; reverse++) {
            for (int bothValid = 0; bothValid < 2; bothValid++) {
                struct MarioState m = fresh();
                mode = 13;
                ledgeAnchorVariant = bothValid;
                ledgeWallOrder = reverse;
                m.pos[0] = m.pos[2] = 49;
                m.pos[1] = m.floorHeight = 0;
                m.faceAngle[1] = 0x2000;
                m.vel[0] = m.vel[2] = 2;
                wallA.normal.x = 1;
                wallB.normal.z = 1;
                EXPECT_EQ(check_ledge_climb_down_internal(&m, TRUE, TRUE), TRUE);
                int selectedB = bothValid && reverse;
                EXPECT_EQ(m.floor == (selectedB ? &topB : &topA), TRUE);
                NEAR(m.pos[0], selectedB ? 51 : 23);
                NEAR(m.pos[2], selectedB ? 23 : 51);
                NEAR(m.pos[1], selectedB ? 10 : -12);
                NEAR(m.floorHeight, m.pos[1]);
                EXPECT_EQ(m.faceAngle[1], (s16) (SURFACE_YAW(selectedB ? &wallB : &wallA) + 0x8000));
            }
        }
    }
    return 1;
}
static int crouch_slide_edge_actions(void) {
    // A guarded edge stop must honor the caller's unfinished entry action.
    for (int cycle = 0; cycle < 3; cycle++) {
        mode = 9;
        struct MarioState m = fresh();
        m.action = ACT_CROUCH_SLIDE;
        ground.originOffset = 0;
        topB.originOffset = -300;
        m.floorHeight = 0;
        m.pos[2] = 44;
        m.vel[0] = 0;
        m.vel[2] = 80;
        m.forwardVel = 80;
        edgeWallEnabled = FALSE;
        m.input = INPUT_Z_DOWN;
        common_slide_action(&m, ACT_START_CROUCHING, ACT_FREEFALL, MARIO_ANIM_START_CROUCHING);
        EXPECT_EQ(m.action, ACT_START_CROUCHING);
        EXPECT_EQ(m.forwardVel, 0);
        NEAR(m.slideVelX, 0);
        NEAR(m.slideVelZ, 0);
        EXPECT_EQ(m.pos[2] < 50, TRUE);
    }
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int inputMode = 0; inputMode < 3; inputMode++) {
            mode = 9;
            struct MarioState m = fresh();
            m.action = ACT_CROUCH_SLIDE;
            ground.originOffset = 0;
            topB.originOffset = -300;
            m.floorHeight = 0;
            m.pos[2] = 44;
            m.vel[0] = 0;
            m.vel[2] = 80;
            m.forwardVel = 80;
            edgeWallEnabled = TRUE;
            wallB.normal.z = 1;
            if (inputMode == 1) {
                m.input = INPUT_NONZERO_ANALOG;
                m.intendedYaw = 0x8000;
            } else if (inputMode == 2) {
                m.input = INPUT_NONZERO_ANALOG;
                m.intendedYaw = 0;
            } else {
                m.input = 0;
            }
            common_slide_action(&m, ACT_CROUCHING, ACT_FREEFALL, MARIO_ANIM_START_CROUCHING);
            EXPECT_EQ(m.action, inputMode == 1   ? ACT_LEDGE_CLIMB_DOWN
                                : inputMode == 2 ? ACT_FREEFALL
                                                 : ACT_CROUCHING);
            if (inputMode != 2) {
                EXPECT_EQ(m.forwardVel, 0);
                NEAR(m.slideVelX, 0);
                NEAR(m.slideVelZ, 0);
            }
            if (inputMode == 1) {
                EXPECT_EQ(m.pos[2] < 50, TRUE);
                EXPECT_EQ(m.floor == &ground, TRUE);
            }
            if (inputMode == 2) {
                EXPECT_EQ(m.pos[2] > 50, TRUE);
            }
        }
    }
    for (int wall = 0; wall < 2; wall++) {
        mode = 9;
        struct MarioState m = fresh();
        m.action = ACT_CROUCH_SLIDE;
        ground.originOffset = 0;
        topB.originOffset = wall ? -140 : -300;
        m.floorHeight = 0;
        m.pos[2] = 44;
        m.vel[0] = 0;
        m.vel[2] = 80;
        m.forwardVel = 80;
        m.input = INPUT_NONZERO_ANALOG;
        m.intendedYaw = 0x8000;
        edgeWallEnabled = wall;
        wallB.normal.z = 1;
        common_slide_action(&m, ACT_CROUCHING, ACT_FREEFALL, MARIO_ANIM_START_CROUCHING);
        EXPECT_EQ(m.action, ACT_CROUCHING);
    }
    for (int cycle = 0; cycle < 3; cycle++) {
        mode = 9;
        struct MarioState m = fresh();
        m.action = ACT_CROUCH_SLIDE;
        ground.originOffset = 0;
        topB.originOffset = -300;
        m.floorHeight = 0;
        m.pos[2] = 44;
        m.faceAngle[1] = 0x8000;
        m.intendedYaw = 0;
        m.input = INPUT_NONZERO_ANALOG;
        m.vel[2] = 80;
        m.forwardVel = -80;
        edgeWallEnabled = TRUE;
        wallB.normal.z = 1;
        common_slide_action(&m, ACT_CROUCHING, ACT_FREEFALL, MARIO_ANIM_START_CROUCHING);
        EXPECT_EQ(m.action, ACT_LEDGE_CLIMB_DOWN);
        EXPECT_EQ(m.pos[2] < 50, TRUE);
        EXPECT_EQ(m.forwardVel, 0);
        NEAR(m.slideVelX, 0);
        NEAR(m.slideVelZ, 0);
    }
    for (int variant = 0; variant < 5; variant++) {
        mode = 9;
        struct MarioState m = fresh();
        m.action = variant == 4 ? ACT_BUTT_SLIDE : ACT_CROUCH_SLIDE;
        ground.originOffset = 0;
        topB.originOffset = variant == 2 ? -140 : -300;
        m.floorHeight = 0;
        m.pos[2] = 44;
        m.vel[2] = variant == 3 ? -2 : 2;
        m.forwardVel = 2;
        m.input = INPUT_NONZERO_ANALOG;
        m.intendedYaw = variant == 1 ? 0 : 0x8000;
        edgeWallEnabled = variant != 2;
        wallB.normal.z = 1;
        slideStops = TRUE;
        slideUpdateCalls = 0;
        EXPECT_EQ(common_slide_action_with_jump(&m, variant == 4 ? ACT_BUTT_SLIDE_STOP : ACT_CROUCHING,
                                                ACT_JUMP, ACT_FREEFALL, MARIO_ANIM_START_CROUCHING,
                                                4.0f),
                  TRUE);
        EXPECT_EQ(m.action, variant == 0   ? ACT_LEDGE_CLIMB_DOWN
                            : variant == 4 ? ACT_BUTT_SLIDE_STOP
                                           : ACT_CROUCHING);
        EXPECT_EQ(slideUpdateCalls, variant == 0 ? 0 : 1);
        if (variant == 0) {
            EXPECT_EQ(m.pos[2] < 50, TRUE);
            EXPECT_EQ(m.floor == &ground, TRUE);
        }
    }
    return 1;
}
static int wing_water_walking(void) {
    const u32 unsupportedCaps[] = { 0, MARIO_METAL_CAP, MARIO_VANISH_CAP,
                                    MARIO_METAL_CAP | MARIO_VANISH_CAP };
    for (unsigned cap = 0; cap < sizeof(unsupportedCaps) / sizeof(*unsupportedCaps); cap++) {
        struct MarioState m = fresh();
        m.flags = unsupportedCaps[cap];
        m.action = ACT_WALKING;
        m.forwardVel = 48.0f;
        EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), FALSE);
    }

    const f32 depths[] = { 20.0f, 500.0f };
    const u32 caps[] = { MARIO_WING_CAP, MARIO_WING_CAP | MARIO_METAL_CAP,
                         MARIO_WING_CAP | MARIO_VANISH_CAP,
                         MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP };
    for (unsigned cap = 0; cap < sizeof(caps) / sizeof(*caps); cap++) {
        for (unsigned depth = 0; depth < sizeof(depths) / sizeof(*depths); depth++) {
            struct MarioState m = fresh();
            mode = 0;
            ground.originOffset = -depths[depth];
            m.floorHeight = -depths[depth];
            m.flags = caps[cap];
            m.action = ACT_WALKING;
            m.waterLevel = sShellWaterLevel = 0.0f;
            gWaterSurfacePseudoFloor.normal.y = 1.0f;
            m.faceAngle[1] = 0x4000;
            mario_set_forward_vel(&m, 48.0f);
            for (int frame = 0; frame < 20; frame++) {
                f32 before = m.pos[0];
                EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
                NEAR(m.pos[0] - before, 48.0f);
                NEAR(m.pos[1], 0.0f);
                NEAR(m.floorHeight, 0.0f);
                EXPECT_EQ(m.floor, &gWaterSurfacePseudoFloor);
            }
            // The real shore replaces water support without dropping movement.
            ground.originOffset = 0.0f;
            m.waterLevel = sShellWaterLevel = -10000.0f;
            EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
            EXPECT_EQ(m.floor, &ground);
            NEAR(m.pos[1], 0.0f);
        }
    }
    for (int pound = 0; pound < 2; pound++) {
        struct MarioState m = fresh();
        mode = 0;
        ground.originOffset = -500.0f;
        m.floorHeight = -500.0f;
        m.flags = MARIO_WING_CAP;
        m.action = pound ? ACT_GROUND_POUND : ACT_FREEFALL;
        m.pos[1] = 30.0f;
        m.waterLevel = sShellWaterLevel = 0.0f;
        mario_set_forward_vel(&m, 40.0f);
        vec3f_set(m.vel, 40.0f, pound ? -200.0f : -40.0f, 0.0f);
        EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL),
                  pound ? AIR_STEP_NONE : AIR_STEP_LANDED);
        if (pound) {
            for (int frame = 0; frame < 5 && m.pos[1] >= -100.0f; frame++) {
                EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL), AIR_STEP_NONE);
            }
            EXPECT_EQ(m.pos[1] < -100.0f, TRUE);
            EXPECT_EQ(mario_has_land_movement_underwater(&m), FALSE);
            EXPECT_EQ(set_water_plunge_action(&m), TRUE);
            EXPECT_EQ(m.action, ACT_WATER_DOWN_BURST);
            EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), FALSE);
        } else {
            NEAR(m.pos[1], 0.0f);
            EXPECT_EQ(m.floor, &gWaterSurfacePseudoFloor);
        }
    }
    struct MarioState m = fresh();
    mode = 0;
    ground.originOffset = -500.0f;
    m.floorHeight = -500.0f;
    m.floor = &gWaterSurfacePseudoFloor;
    m.flags = MARIO_WING_CAP;
    m.action = ACT_WALKING;
    m.waterLevel = sShellWaterLevel = 0.0f;
    mario_set_forward_vel(&m, 48.0f);
    m.flags &= ~MARIO_WING_CAP;
    EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_LEFT_GROUND);
    m.flags = MARIO_WING_CAP;
    m.action = ACT_WATER_IDLE;
    EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), FALSE);
    m.action = ACT_WATER_JUMP;
    m.pos[1] = -80.0f;
    EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), FALSE);
    m.pos[1] = 10.0f;
    EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), TRUE);
    m.action = ACT_WALKING;
    EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 150.0f), FALSE);
    EXPECT_EQ(mario_can_walk_on_water(&m, FLOOR_LOWER_LIMIT, 10000.0f), FALSE);
    const u32 divingActions[] = { ACT_BUTT_SLIDE,      ACT_HOLD_BUTT_SLIDE,   ACT_DIVE,
                                  ACT_FORWARD_ROLLOUT, ACT_BACKWARD_ROLLOUT,  ACT_SLIDE_KICK,
                                  ACT_FLYING,          ACT_FLYING_TRIPLE_JUMP };
    for (unsigned action = 0; action < sizeof(divingActions) / sizeof(*divingActions); action++) {
        m = fresh();
        mode = 0;
        m.flags = MARIO_WING_CAP;
        m.action = divingActions[action];
        ground.originOffset = m.floorHeight = -500.0f;
        m.waterLevel = sShellWaterLevel = 0.0f;
        m.pos[1] = 30.0f;
        mario_set_forward_vel(&m, 48.0f);
        vec3f_set(m.vel, 48.0f, -40.0f, 0.0f);
        EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL), AIR_STEP_NONE);
        EXPECT_EQ(m.pos[1] < 0.0f, TRUE);
        EXPECT_EQ(m.floor, &ground);
    }
    const u32 slidingActions[] = {
        ACT_BUTT_SLIDE,    ACT_HOLD_BUTT_SLIDE,    ACT_DIVE_SLIDE,       ACT_DIVE_PICKING_UP,
        ACT_STOMACH_SLIDE, ACT_HOLD_STOMACH_SLIDE, ACT_SLIDE_KICK_SLIDE,
    };
    for (unsigned action = 0; action < sizeof(slidingActions) / sizeof(*slidingActions); action++) {
        m = fresh();
        mode = 0;
        m.flags = MARIO_WING_CAP;
        m.action = slidingActions[action];
        ground.originOffset = m.floorHeight = m.pos[1] = -20.0f;
        m.waterLevel = sShellWaterLevel = 0.0f;
        mario_set_forward_vel(&m, 48.0f);
        EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
        NEAR(m.pos[1], -20.0f);
        EXPECT_EQ(m.floor, &ground);
    }
    const u32 stoppingActions[] = { ACT_BRAKING,
                                    ACT_BRAKING_STOP,
                                    ACT_DECELERATING,
                                    ACT_BUTT_SLIDE_STOP,
                                    ACT_HOLD_BUTT_SLIDE_STOP,
                                    ACT_STOMACH_SLIDE_STOP,
                                    ACT_SLIDE_KICK_SLIDE_STOP };
    for (unsigned action = 0; action < sizeof(stoppingActions) / sizeof(*stoppingActions); action++) {
        m = fresh();
        mode = 0;
        m.flags = MARIO_WING_CAP;
        m.action = stoppingActions[action];
        ground.originOffset = m.floorHeight = -500.0f;
        m.waterLevel = sShellWaterLevel = 0.0f;
        mario_set_forward_vel(&m, 48.0f);
        EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
        NEAR(m.pos[1], 0.0f);
        EXPECT_EQ(m.floor, &gWaterSurfacePseudoFloor);
    }
    const f32 speeds[] = { -30.0f, 0.0f, 29.99f, 30.0f, 30.01f };
    for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(*speeds); speed++) {
        m = fresh();
        mode = 0;
        m.flags = MARIO_WING_CAP;
        m.action = ACT_WALKING;
        ground.originOffset = m.floorHeight = -500.0f;
        m.waterLevel = sShellWaterLevel = 0.0f;
        mario_set_forward_vel(&m, speeds[speed]);
        EXPECT_EQ(perform_ground_step(&m),
                  speeds[speed] >= 30.0f ? GROUND_STEP_NONE : GROUND_STEP_LEFT_GROUND);
        NEAR(m.floorHeight, speeds[speed] >= 30.0f ? 0.0f : -500.0f);
    }
    const f32 verticalSpeeds[] = { -60.01f, -60.0f, -59.99f };
    for (unsigned speed = 0; speed < sizeof(verticalSpeeds) / sizeof(*verticalSpeeds); speed++) {
        m = fresh();
        mode = 0;
        m.flags = MARIO_WING_CAP;
        m.action = ACT_FREEFALL;
        ground.originOffset = m.floorHeight = -500.0f;
        m.waterLevel = sShellWaterLevel = 0.0f;
        m.pos[1] = 30.0f;
        mario_set_forward_vel(&m, 48.0f);
        m.vel[1] = verticalSpeeds[speed];
        EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL),
                  verticalSpeeds[speed] > -60.0f ? AIR_STEP_LANDED : AIR_STEP_NONE);
        if (verticalSpeeds[speed] > -60.0f) {
            NEAR(m.pos[1], 0.0f);
        } else {
            EXPECT_EQ(m.pos[1] < 0.0f, TRUE);
        }
    }
    return TRUE;
}

static int water_twirl_support(void) {
    struct MarioState m = fresh();
    mode = 0;
    ground.originOffset = m.floorHeight = -500.0f;
    m.flags = MARIO_WING_CAP;
    m.action = ACT_GROUND_TWIRL;
    m.waterWalkTwirl = TRUE;
    m.input = 0;
    m.waterLevel = sShellWaterLevel = 0.0f;
    m.pos[1] = 0.0f;
    m.forwardVel = 0.0f;
    vec3f_set(m.vel, 0.0f, 0.0f, 0.0f);
    m.controller->buttonDown = L_TRIG;
    for (int frame = 0; frame < 1800; frame++) {
        mario_update_water_walking_state(&m, 0.0f, 10000.0f);
        EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), TRUE);
        EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
        EXPECT_EQ(m.floor, &gWaterSurfacePseudoFloor);
        NEAR(m.pos[1], 0.0f);
    }
    m.flags &= ~MARIO_WING_CAP;
    EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), FALSE);
    EXPECT_EQ(perform_ground_quarter_step(&m, m.pos, FALSE), GROUND_STEP_LEFT_GROUND);
    return TRUE;
}

static int water_walk_reentry(void) {
    const u32 caps[] = { MARIO_WING_CAP, MARIO_WING_CAP | MARIO_METAL_CAP,
                         MARIO_WING_CAP | MARIO_VANISH_CAP,
                         MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP };
    for (int cold = 0; cold < 2; cold++) {
        for (unsigned cap = 0; cap < sizeof(caps) / sizeof(*caps); cap++) {
            struct MarioState m = fresh();
            mode = 0;
            m.area->terrainType = cold ? TERRAIN_SNOW : TERRAIN_GRASS;
            m.flags = caps[cap];
            m.action = ACT_WALKING;
            m.floorHeight = ground.originOffset = -500.0f;
            m.waterLevel = sShellWaterLevel = 0.0f;
            m.pos[1] = -1.0f;
            mario_set_forward_vel(&m, 48.0f);
            mario_update_water_walking_state(&m, 0.0f, 10000.0f);
            EXPECT_EQ(m.waterWalkDisabled, TRUE);
            // Coming back above water, gaining speed or changing caps cannot rearm it.
            m.action = ACT_FREEFALL;
            m.pos[1] = 30.0f;
            m.flags = 0;
            mario_update_water_walking_state(&m, 0.0f, 10000.0f);
            m.flags = caps[cap];
            m.vel[1] = -40.0f;
            EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL), AIR_STEP_NONE);
            EXPECT_EQ(m.pos[1] < 0.0f, TRUE);
            EXPECT_EQ(m.waterWalkDisabled, TRUE);
            // Being airborne over a shore does not count as touching dry land.
            m.floorHeight = ground.originOffset = 0.0f;
            m.floor = &ground;
            m.waterLevel = sShellWaterLevel = -10000.0f;
            m.pos[1] = 0.5f;
            m.vel[1] = 0.0f;
            mario_update_water_walking_state(&m, -10000.0f, 10000.0f);
            EXPECT_EQ(m.waterWalkDisabled, TRUE);
            m.pos[1] = 0.0f;
            mario_update_water_walking_state(&m, -10000.0f, 10000.0f);
            EXPECT_EQ(m.waterWalkDisabled, FALSE);
            for (int frame = 0; frame < 5; frame++) {
                mario_tick_water_walking_cooldown(&m);
            }
            // A fresh run from dry land can land on water again, including cold water.
            m.action = ACT_WALKING;
            m.floorHeight = ground.originOffset = -500.0f;
            m.waterLevel = sShellWaterLevel = 0.0f;
            gWaterSurfacePseudoFloor.normal.y = 1.0f;
            EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
            EXPECT_EQ(m.floor, &gWaterSurfacePseudoFloor);
            NEAR(m.pos[1], 0.0f);
            EXPECT_EQ(m.waterWalkDisabled, FALSE);
        }
    }
    return TRUE;
}

static int water_walk_cooldown_and_shallow_reentry(void) {
    const u32 caps[] = { MARIO_WING_CAP, MARIO_WING_CAP | MARIO_METAL_CAP,
                         MARIO_WING_CAP | MARIO_VANISH_CAP,
                         MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP };
    for (int cold = 0; cold < 2; cold++) {
        for (unsigned cap = 0; cap < sizeof(caps) / sizeof(*caps); cap++) {
            struct MarioState m = fresh();
            mode = 0;
            m.area->terrainType = cold ? TERRAIN_SNOW : TERRAIN_GRASS;
            m.flags = caps[cap];
            m.action = ACT_WALKING;
            m.floorHeight = ground.originOffset = -500.0f;
            m.waterLevel = sShellWaterLevel = 0.0f;
            m.pos[1] = 0.0f;
            m.ceilHeight = 10000.0f;
            mario_set_forward_vel(&m, 29.0f);
            mario_update_water_walking_state(&m, 0.0f, 10000.0f);
            EXPECT_EQ(m.waterWalkCooldown, 5);
            mario_set_forward_vel(&m, 30.0f);
            for (int frame = 0; frame < 5; frame++) {
                for (int query = 0; query < 4; query++) {
                    mario_update_water_walking_state(&m, 0.0f, 10000.0f);
                    EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), FALSE);
                }
                mario_tick_water_walking_cooldown(&m);
            }
            EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), TRUE);
            // Deep entry stays latched, then shallow footing rearms it.
            m.pos[1] = -20.0f;
            mario_update_water_walking_state(&m, 0.0f, 10000.0f);
            EXPECT_EQ(m.waterWalkDisabled, TRUE);
            m.floor = &ground;
            m.floorHeight = ground.originOffset = -20.0f;
            mario_update_water_walking_state(&m, 0.0f, 10000.0f);
            EXPECT_EQ(m.waterWalkDisabled, FALSE);
            for (int frame = 0; frame < 5; frame++) {
                mario_tick_water_walking_cooldown(&m);
                mario_update_water_walking_state(&m, 0.0f, 10000.0f);
            }
            EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
            EXPECT_EQ(m.floor, &gWaterSurfacePseudoFloor);
            NEAR(m.pos[1], 0.0f);
            // With no pending timeout, shallow footing supports water immediately.
            m.floor = &ground;
            m.floorHeight = ground.originOffset = -100.0f;
            m.pos[1] = -100.0f;
            m.waterWalkDisabled = TRUE;
            mario_update_water_walking_state(&m, 0.0f, 10000.0f);
            EXPECT_EQ(m.waterWalkCooldown, 0);
            EXPECT_EQ(m.waterWalkDisabled, FALSE);
            EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
            EXPECT_EQ(m.floor, &gWaterSurfacePseudoFloor);
            NEAR(m.pos[1], 0.0f);
        }
    }
    return TRUE;
}

static int wing_water_fall_damage_prohibition(void) {
    const u32 caps[] = { MARIO_WING_CAP, MARIO_WING_CAP | MARIO_VANISH_CAP,
                         MARIO_WING_CAP | MARIO_METAL_CAP, MARIO_METAL_CAP, MARIO_VANISH_CAP };
    const f32 heights[] = { 1150.0f, 1150.01f, 3000.0f, 3000.01f };
    const f32 velocities[] = { -55.0f, -55.01f, -59.99f };
    for (int cold = 0; cold < 2; cold++) {
        for (unsigned cap = 0; cap < sizeof(caps) / sizeof(*caps); cap++) {
            for (unsigned height = 0; height < sizeof(heights) / sizeof(*heights); height++) {
                for (unsigned velocity = 0; velocity < sizeof(velocities) / sizeof(*velocities);
                     velocity++) {
                    for (int held = 0; held < 2; held++) {
                        struct MarioState m = fresh();
                        mode = 0;
                        m.area->terrainType = cold ? TERRAIN_SNOW : TERRAIN_GRASS;
                        m.action = ACT_FREEFALL;
                        m.flags = caps[cap];
                        m.floorHeight = ground.originOffset = -500.0f;
                        // The submerged floor must not grant slippery/burning exemptions
                        // to the hypothetical ordinary-friction water surface.
                        ground.type = SURFACE_BURNING;
                        m.pos[1] = 20.0f;
                        m.waterLevel = sShellWaterLevel = 0.0f;
                        m.peakHeight = heights[height];
                        m.vel[1] = velocities[velocity];
                        mario_set_forward_vel(&m, 40.0f);
                        controller.buttonDown = held ? A_BUTTON : 0;
                        s32 banned = !(caps[cap] & MARIO_WING_CAP)
                                     || (!(caps[cap] & MARIO_METAL_CAP) && !held
                                         && heights[height] > 1150.0f && velocities[velocity] < -55.0f);
                        Vec3f before;
                        vec3f_copy(before, m.pos);
                        EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), !banned);
                        NEAR(m.pos[1], before[1]);
                        EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL),
                                  banned ? AIR_STEP_NONE : AIR_STEP_LANDED);
                        EXPECT_EQ(m.floor == &gWaterSurfacePseudoFloor, !banned);
                        if (banned) {
                            EXPECT_EQ(m.waterWalkCooldown, 5);
                            EXPECT_EQ(m.pos[1] < 0.0f, TRUE);
                        } else {
                            NEAR(m.pos[1], 0.0f);
                        }
                    }
                }
            }
        }
    }
    const u32 protectedActions[] = { ACT_FREEFALL, ACT_TWIRLING, ACT_TOP_OF_POLE_JUMP };
    for (int protection = 0; protection < 3; protection++) {
        struct MarioState m = fresh();
        m.flags = MARIO_WING_CAP;
        m.action = protectedActions[protection];
        m.spawnFallProtection =
            protection == 0 ? MARIO_SPAWN_FALL_PROTECTION_ACTIVE : MARIO_SPAWN_FALL_PROTECTION_NONE;
        m.pos[1] = 10.0f;
        m.peakHeight = 4000.0f;
        m.vel[1] = -56.0f;
        m.forwardVel = 40.0f;
        EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), TRUE);
    }
    return TRUE;
}

static int underwater_shell_ascent(void) {
    mode = 0;
    struct MarioState m = fresh();
    ground.originOffset = 0;
    m.floorHeight = 0;
    m.action = ACT_RIDING_SHELL_GROUND;
    m.flags = MARIO_METAL_CAP;
    sShellWaterLevel = 150;
    sRidingUnderwaterShell = TRUE;
    for (int frame = 0; frame < 3; frame++) {
        m.prevPos[1] = m.pos[1];
        EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
        NEAR(m.pos[1], MIN(150.0f, (frame + 1) * 60.0f));
    }

    const u32 vanishCaps[] = { MARIO_VANISH_CAP, MARIO_VANISH_CAP | MARIO_METAL_CAP,
                               MARIO_VANISH_CAP | MARIO_METAL_CAP | MARIO_WING_CAP };
    for (unsigned cap = 0; cap < sizeof(vanishCaps) / sizeof(vanishCaps[0]); cap++) {
        m = fresh();
        ground.originOffset = 0;
        m.floorHeight = 0;
        m.action = ACT_RIDING_SHELL_GROUND;
        m.flags = vanishCaps[cap];
        sShellWaterLevel = 150;
        sRidingUnderwaterShell = TRUE;
        for (int frame = 0; frame < 3; frame++) {
            m.prevPos[1] = m.pos[1];
            EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
            NEAR(m.pos[1], 0);
        }
        // Losing only Vanish restores Metal's rise on the next movement step.
        if (m.flags & MARIO_METAL_CAP) {
            m.flags &= ~MARIO_VANISH_CAP;
            EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
            NEAR(m.pos[1], 60);
        }
        m = fresh();
        ground.originOffset = 0;
        m.floorHeight = 0;
        m.action = ACT_RIDING_SHELL_FALL;
        m.flags = vanishCaps[cap];
        m.pos[1] = m.prevPos[1] = 300;
        sShellWaterLevel = 500;
        sRidingUnderwaterShell = TRUE;
        Vec3f fallingPos = { 0, 250, 0 };
        EXPECT_EQ(perform_air_quarter_step(&m, fallingPos, AIR_STEP_CHECK_NONE, TRUE), AIR_STEP_NONE);
        NEAR(m.pos[1], 250);
        fallingPos[1] = -10;
        EXPECT_EQ(perform_air_quarter_step(&m, fallingPos, AIR_STEP_CHECK_NONE, TRUE), AIR_STEP_LANDED);
        NEAR(m.pos[1], 0);
    }

    m = fresh();
    ground.originOffset = 0;
    m.floorHeight = 0;
    m.action = ACT_RIDING_SHELL_GROUND;
    m.flags = MARIO_METAL_CAP;
    sShellWaterLevel = 150;
    sRidingUnderwaterShell = TRUE;
    ceilingY = 200;
    EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
    NEAR(m.pos[1], 39);

    m = fresh();
    ground.originOffset = 0;
    m.floorHeight = 0;
    m.action = ACT_RIDING_SHELL_FALL;
    m.flags = MARIO_METAL_CAP;
    m.pos[1] = m.prevPos[1] = 300;
    sShellWaterLevel = 500;
    sRidingUnderwaterShell = TRUE;
    Vec3f nextPos = { 0, 300, 0 };
    EXPECT_EQ(perform_air_quarter_step(&m, nextPos, AIR_STEP_CHECK_NONE, TRUE), AIR_STEP_LANDED);
    NEAR(m.pos[1], 360);

    m = fresh();
    ground.originOffset = 0;
    m.floorHeight = 0;
    m.action = ACT_RIDING_SHELL_FALL;
    m.flags = MARIO_METAL_CAP;
    m.pos[1] = m.prevPos[1] = 300;
    sShellWaterLevel = 500;
    sRidingUnderwaterShell = TRUE;
    ceilingY = 500;
    EXPECT_EQ(perform_air_quarter_step(&m, nextPos, AIR_STEP_CHECK_NONE, TRUE), AIR_STEP_LANDED);
    NEAR(m.pos[1], 339);

    m = fresh();
    ground.originOffset = 0;
    m.floorHeight = 0;
    m.action = ACT_RIDING_SHELL_JUMP;
    m.flags = MARIO_METAL_CAP;
    m.pos[1] = m.prevPos[1] = 300;
    m.vel[1] = 42;
    sShellWaterLevel = 500;
    sRidingUnderwaterShell = TRUE;
    nextPos[1] = 310;
    EXPECT_EQ(perform_air_quarter_step(&m, nextPos, AIR_STEP_CHECK_NONE, TRUE), AIR_STEP_NONE);
    NEAR(m.pos[1], 310);

    m = fresh();
    ground.originOffset = 0;
    m.floorHeight = 0;
    m.action = ACT_RIDING_SHELL_GROUND;
    sShellWaterLevel = 150;
    EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
    NEAR(m.pos[1], 150);
    return 1;
}

static int backward_jump_kick_slope_entry(void) {
    // Neither a floor underneath nor crossing onto a slope is contact.
    for (int marked = 0; marked < 2; marked++) {
        for (int onSlope = 0; onSlope < 2; onSlope++) {
            struct MarioState m = fresh();
            mode = 11;
            ground.originOffset = 0;
            topA.normal.x = -0.8f;
            topA.normal.y = 0.6f;
            m.action = ACT_JUMP_KICK;
            m.actionArg = marked ? ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL : 0;
            m.faceAngle[1] = -0x4000;
            m.pos[0] = onSlope ? 100 : -1;
            m.floor = onSlope ? &topA : &ground;
            m.floorHeight = onSlope ? 100 * 4.0f / 3.0f : 0;
            m.pos[1] = m.floorHeight + 500;
            m.forwardVel = -64;
            vec3f_set(m.vel, 64, 8, 0);
            f32 startX = m.pos[0];
            EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL), AIR_STEP_NONE);
            NEAR(m.pos[0], startX + 64);
            NEAR(m.vel[0], 64);
        }
    }
    // On real landing, remove 20 once, regardless of collision substeps.
    // Carry remaining speed into successive contacts rather than resetting it.
    f32 speed = 64;
    for (int contact = 0; contact < 5; contact++) {
        struct MarioState m = fresh();
        mode = 11;
        ground.originOffset = 0;
        topA.normal.x = -0.8f;
        topA.normal.y = 0.6f;
        m.action = ACT_JUMP_KICK;
        m.faceAngle[1] = -0x4000;
        m.pos[0] = -0.5f;
        m.pos[1] = 1;
        m.floor = &ground;
        m.floorHeight = 0;
        m.forwardVel = -speed;
        vec3f_set(m.vel, speed, -16, 0);
        if (speed == 0) {
            m.pos[0] = 100;
            m.pos[1] = 100 * 4.0f / 3.0f;
            m.floor = &topA;
        }
        EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL), AIR_STEP_LANDED);
        NEAR(m.vel[0], speed > 20 ? speed - 20 : 0);
        NEAR(m.pos[1], m.floorHeight);
        speed = m.vel[0];
    }
    // Actual contacts with three shipped steep JRB rock faces.
    const s32 starts[][3] = { { -5347, 1036, 2143 }, { -5488, 1168, 949 }, { -4109, 1313, -18 } };
    for (unsigned rock = 0; rock < 3; rock++) {
        struct MarioState m = fresh();
        mode = 6;
        const s32 *p = rockTestPlacements[rock];
        place_rock(p[0], p[1], p[2], p[3]);
        vec3f_set(m.pos, starts[rock][0], starts[rock][1], starts[rock][2]);
        m.floorHeight = find_floor(m.pos[0], m.pos[1], m.pos[2], &m.floor);
        m.pos[1] = m.floorHeight;
        struct Surface *initial = m.floor;
        f32 length = sqrtf(sqr(initial->normal.x) + sqr(initial->normal.z));
        m.faceAngle[1] = atan2s(initial->normal.z, initial->normal.x);
        m.action = ACT_JUMP_KICK;
        m.forwardVel = -24;
        vec3f_set(m.vel, -24 * initial->normal.x / length, -16, -24 * initial->normal.z / length);
        EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL), AIR_STEP_LANDED);
        NEAR(sqrtf(sqr(m.vel[0]) + sqr(m.vel[2])), 4);
        NEAR(m.slopeJumpKickImpactSpeed, -24);
        NEAR(m.pos[1], m.floorHeight);
    }
    // Backwards downhill and forward uphill contacts keep their momentum.
    for (int downhill = 0; downhill < 2; downhill++) {
        struct MarioState m = fresh();
        mode = 11;
        ground.originOffset = 0;
        topA.normal.x = -0.8f;
        topA.normal.y = 0.6f;
        m.floor = &topA;
        m.action = ACT_JUMP_KICK;
        m.pos[0] = 100;
        m.pos[1] = 100 * 4.0f / 3.0f;
        m.forwardVel = downhill ? -24 : 24;
        m.faceAngle[1] = 0x4000;
        vec3f_set(m.vel, downhill ? -24 : 24, -80, 0);
        EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL), AIR_STEP_LANDED);
        NEAR(m.vel[0], downhill ? -24 : 24);
    }
    return 1;
}

static int metal_water_movement(void) {
    const u32 actions[] = { ACT_WALKING,       ACT_HOLD_WALKING,       ACT_CRAWLING,
                            ACT_CROUCH_SLIDE,  ACT_BUTT_SLIDE,         ACT_DIVE_SLIDE,
                            ACT_STOMACH_SLIDE, ACT_HOLD_STOMACH_SLIDE, ACT_SLIDE_KICK_SLIDE,
                            ACT_MOVE_PUNCHING, ACT_RIDING_SHELL_GROUND };
    for (unsigned action = 0; action < sizeof(actions) / sizeof(actions[0]); action++) {
        for (int direction = -1; direction <= 1; direction += 2) {
            for (int cycle = 0; cycle < 3; cycle++) {
                struct MarioState m = fresh();
                mode = 0;
                ground.originOffset = m.floorHeight = 0;
                m.action = actions[action];
                m.flags = MARIO_METAL_CAP;
                m.waterLevel = 1000;
                m.faceAngle[1] = 0x4000;
                f32 previousTravel = 0;
                for (int frame = 0; frame < 20; frame++) {
                    // A supplied acceleration ramp isolates integration from each
                    // action's own controls. Stored speeds must not compound the scale.
                    f32 speed = direction * (frame + 1) * 2.0f;
                    mario_set_forward_vel(&m, speed);
                    f32 before = m.pos[0];
                    EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_NONE);
                    f32 travel = m.pos[0] - before;
                    NEAR(travel, speed * .65f);
                    NEAR(m.forwardVel, speed);
                    NEAR(m.vel[0], speed);
                    NEAR(travel - previousTravel, direction * 1.3f);
                    previousTravel = travel;
                    NEAR(m.pos[1], m.floorHeight);
                }
                // Vanish overrides Metal immediately; losing it restores resistance.
                m.flags |= MARIO_VANISH_CAP;
                f32 before = m.pos[0];
                perform_ground_step(&m);
                NEAR(m.pos[0] - before, m.forwardVel);
                m.flags &= ~MARIO_VANISH_CAP;
                before = m.pos[0];
                perform_ground_step(&m);
                NEAR(m.pos[0] - before, m.forwardVel * .65f);
                // Removing Metal, or leaving water, restores full movement immediately.
                for (int exit = 0; exit < 2; exit++) {
                    if (exit == 0) {
                        m.flags = MARIO_VANISH_CAP;
                    } else {
                        m.flags = MARIO_METAL_CAP;
                        m.waterLevel = -1000;
                    }
                    f32 before = m.pos[0];
                    perform_ground_step(&m);
                    NEAR(m.pos[0] - before, m.forwardVel);
                }
            }
        }
    }
    return 1;
}

static int metal_water_gravity(void) {
    const u32 actions[] = { ACT_JUMP,
                            ACT_DOUBLE_JUMP,
                            ACT_TRIPLE_JUMP,
                            ACT_HOLD_JUMP,
                            ACT_BACKFLIP,
                            ACT_SIDE_FLIP,
                            ACT_LONG_JUMP,
                            ACT_DIVE,
                            ACT_FORWARD_ROLLOUT,
                            ACT_BACKWARD_ROLLOUT,
                            ACT_WALL_KICK_AIR,
                            ACT_JUMP_KICK,
                            ACT_SLIDE_KICK,
                            ACT_GROUND_POUND,
                            ACT_FREEFALL,
                            ACT_HOLD_FREEFALL,
                            ACT_TWIRLING,
                            ACT_SHOT_FROM_CANNON,
                            ACT_LAVA_BOOST,
                            ACT_FALL_AFTER_STAR_GRAB,
                            ACT_GETTING_BLOWN,
                            ACT_SOFT_BONK,
                            ACT_METAL_WATER_FALLING };
    const f32 speeds[] = { 40, 10, 0, -8, -50 };
    for (unsigned action = 0; action < sizeof(actions) / sizeof(actions[0]); action++) {
        for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(speeds[0]); speed++) {
            for (int release = 0; release < 2; release++) {
                for (int wing = 0; wing < 2; wing++) {
                    struct MarioState dry = fresh(), wet = dry;
                    mode = 0;
                    dry.action = wet.action = actions[action];
                    dry.windGravity = wet.windGravity = 1.0f;
                    dry.flags = MARIO_METAL_CAP | MARIO_JUMPING | (wing ? MARIO_WING_CAP : 0);
                    wet.flags = dry.flags;
                    dry.input = wet.input = release ? 0 : INPUT_A_DOWN;
                    dry.vel[1] = wet.vel[1] = speeds[speed];
                    wet.waterLevel = 1000;
                    struct MarioState vanish = wet;
                    vanish.flags |= MARIO_VANISH_CAP;
                    apply_gravity(&dry);
                    apply_gravity(&wet);
                    apply_gravity(&vanish);
                    NEAR(vanish.vel[1], dry.vel[1]);
                    f32 expected = speeds[speed] + (dry.vel[1] - speeds[speed]) * .65f;
                    NEAR(wet.vel[1], MAX(expected, -75.0f / .65f));
                    // Holding the state for many updates still sinks at a bounded speed.
                    for (int frame = 0; frame < 90; frame++) {
                        apply_gravity(&wet);
                    }
                    EXPECT_EQ(wet.vel[1] * .65f >= -75.001f, TRUE);
                }
            }
        }
    }
    // Ordinary descent reaches the requested world-speed cap despite resistance.
    struct MarioState falling = fresh();
    falling.flags = MARIO_METAL_CAP;
    falling.action = ACT_FREEFALL;
    falling.waterLevel = 1000;
    falling.vel[1] = -200;
    apply_gravity(&falling);
    NEAR(falling.vel[1] * .65f, -75);
    for (int frame = 0; frame < 120; frame++) {
        apply_gravity(&falling);
    }
    NEAR(falling.vel[1] * .65f, -75);
    // Reduced travel and gravity should make the arc take longer without
    // turning an ordinary underwater jump into an excessive-height launch.
    f32 peaks[2];
    int apexFrames[2];
    for (int water = 0; water < 2; water++) {
        struct MarioState m = fresh();
        mode = 0;
        m.flags = MARIO_METAL_CAP;
        m.pos[1] = 1000;
        m.waterLevel = water ? 5000 : -10000;
        m.input = INPUT_A_DOWN;
        vec3f_set(m.vel, 20, 40, 0);
        peaks[water] = 1000;
        apexFrames[water] = 0;
        for (int frame = 0; frame < 30; frame++) {
            EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL), AIR_STEP_NONE);
            if (m.pos[1] > peaks[water]) {
                peaks[water] = m.pos[1];
                apexFrames[water] = frame;
            }
        }
    }
    EXPECT_EQ(apexFrames[1] > apexFrames[0], TRUE);
    EXPECT_EQ(fabsf(peaks[1] - peaks[0]) < 12, TRUE);
    return 1;
}

static int metal_water_air_and_boundaries(void) {
    const u32 flags[] = { 0, MARIO_VANISH_CAP, MARIO_METAL_CAP, MARIO_METAL_CAP | MARIO_VANISH_CAP };
    const f32 depths[] = { 0, 50, 95, 140, 200 };
    struct MarioState absent = fresh();
    absent.flags = MARIO_METAL_CAP;
    absent.pos[1] = FLOOR_LOWER_LIMIT - 500;
    absent.waterLevel = FLOOR_LOWER_LIMIT;
    NEAR(mario_get_movement_time_scale(&absent), 1);
    for (unsigned cap = 0; cap < sizeof(flags) / sizeof(flags[0]); cap++) {
        for (unsigned depth = 0; depth < sizeof(depths) / sizeof(depths[0]); depth++) {
            for (int cycle = 0; cycle < 3; cycle++) {
                struct MarioState m = fresh();
                mode = 0;
                m.flags = flags[cap];
                m.pos[1] = 1000;
                m.waterLevel = m.pos[1] + depths[depth];
                m.action = ACT_GROUND_POUND;
                f32 scale = 1.0f;
                if ((flags[cap] & MARIO_METAL_CAP) && !(flags[cap] & MARIO_VANISH_CAP)) {
                    if (depth == 2) {
                        scale = .825f;
                    } else if (depth >= 3) {
                        scale = .65f;
                    }
                }
                NEAR(mario_get_movement_time_scale(&m), scale);
                vec3f_set(m.vel, 40, -50, 0);
                f32 before = m.pos[1];
                EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL), AIR_STEP_NONE);
                NEAR(m.pos[0], 40 * scale);
                if (depth >= 3 && scale < 1.0f) {
                    NEAR(m.pos[1] - before, -50.0f * scale);
                } else if (scale == 1.0f) {
                    NEAR(m.pos[1] - before, -50);
                }
                EXPECT_EQ(m.pos[1] > m.floorHeight, TRUE);
            }
        }
    }
    struct MarioState fastFall = fresh();
    mode = 0;
    fastFall.flags = MARIO_METAL_CAP;
    fastFall.action = ACT_GROUND_POUND;
    fastFall.pos[1] = 1000;
    fastFall.waterLevel = 2000;
    fastFall.vel[1] = -200;
    EXPECT_EQ(perform_air_step_with_remainder(&fastFall, 0, NULL), AIR_STEP_NONE);
    NEAR(fastFall.pos[1], 925);
    // A direct ground-pound velocity is limited before travel, then floor
    // contact and the remaining-frame handoff still use ordinary collision.
    struct MarioState m = fresh();
    mode = 0;
    ground.originOffset = m.floorHeight = 0;
    m.flags = MARIO_METAL_CAP;
    m.waterLevel = 1000;
    m.action = ACT_GROUND_POUND;
    m.pos[1] = 1;
    vec3f_set(m.vel, 40, -50, 0);
    f32 remaining = -1;
    EXPECT_EQ(perform_air_step_with_remainder(&m, 0, &remaining), AIR_STEP_LANDED);
    NEAR(m.pos[1], 0);
    EXPECT_EQ(remaining >= 0 && remaining < 1, TRUE);
    m.vel[1] = 0;
    f32 before = m.pos[0];
    perform_ground_step_with_fraction(&m, remaining);
    NEAR(m.pos[0] - before, 40 * .65f * remaining);
    // Ground walls continue to block the slowed movement.
    m = fresh();
    mode = 8;
    m.flags = MARIO_METAL_CAP;
    m.waterLevel = 1000;
    ground.originOffset = m.floorHeight = 0;
    m.faceAngle[1] = 0x4000;
    mario_set_forward_vel(&m, 64);
    vec3f_set(m.vel, 64, 0, 0);
    before = m.pos[0];
    EXPECT_EQ(perform_ground_step(&m), GROUND_STEP_HIT_WALL);
    NEAR(m.pos[0], before);
    return 1;
}

static int thicc_contact_shakes(void) {
    for (int palette = 0; palette < 2; palette++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            const f32 speeds[] = { -39.9f, -40, -70 };
            for (unsigned i = 0; i < 3; i++) {
                struct MarioState m = fresh();
                sPalette = palette;
                mode = 0;
                ground.originOffset = 0;
                m.pos[1] = 1;
                vec3f_set(m.vel, 0, speeds[i], 0);
                EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL), AIR_STEP_LANDED);
                EXPECT_EQ(sContactShakes, palette && i > 0 ? 1 : 0); // Once despite multiple substeps.
                if (sContactShakes) {
                    EXPECT_EQ(sContactShakeType, SHAKE_HE_THICC_CONTACT);
                }
            }
            struct MarioState m = fresh();
            sPalette = palette;
            mode = 0;
            ground.originOffset = 0;
            m.pos[1] = 1;
            m.action = ACT_GROUND_POUND;
            vec3f_set(m.vel, 0, -50, 0);
            EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL), AIR_STEP_LANDED);
            EXPECT_EQ(sContactShakes, 0); // Pound action owns the single stronger event.
            m = fresh();
            sPalette = palette;
            mode = 0;
            vec3f_set(m.vel, 0, -50, 0);
            m.pos[1] = 100;
            EXPECT_EQ(perform_air_step_with_remainder(&m, 0, NULL), AIR_STEP_NONE);
            EXPECT_EQ(sContactShakes, 0);
            m = fresh();
            sPalette = palette;
            m.forwardVel = 31.9f;
            mario_play_bonk_sound(&m);
            EXPECT_EQ(sContactShakes, 0);
            m.forwardVel = -32;
            mario_play_bonk_sound(&m);
            EXPECT_EQ(sContactShakes, palette ? 1 : 0);
        }
    }
    sPalette = MARIO_PALETTE_STANDARD;
    return TRUE;
}
static int underwater_box_surface_contacts(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        struct MarioState m = fresh();
        mode = 0;
        m.flags = MARIO_METAL_CAP;
        m.waterLevel = 1000;
        m.pos[1] = 0;
        m.vel[0] = m.vel[2] = 0;
        m.vel[1] = 20;
        ceilingY = 170;
        perform_air_step(&m, AIR_STEP_CHECK_NONE);
        EXPECT_EQ(sCapSurfaceHits, 1);
        EXPECT_EQ(sCapSurface, &ceiling);
        EXPECT_EQ(sCapSurfaceInteraction, INT_HIT_FROM_BELOW);
        m = fresh();
        mode = 0;
        m.flags = MARIO_METAL_CAP;
        m.waterLevel = 1000;
        m.action = ACT_GROUND_POUND;
        m.pos[1] = 10;
        m.vel[0] = m.vel[2] = 0;
        m.vel[1] = -50;
        ground.originOffset = 0;
        EXPECT_EQ(perform_air_step(&m, AIR_STEP_CHECK_NONE), AIR_STEP_LANDED);
        EXPECT_EQ(sCapSurfaceHits, 1);
        EXPECT_EQ(sCapSurface, &ground);
        EXPECT_EQ(sCapSurfaceInteraction, INT_GROUND_POUND_OR_TWIRL);
        m = fresh();
        mode = 0;
        m.pos[1] = 10;
        m.vel[0] = m.vel[2] = 0;
        m.vel[1] = -50;
        ground.originOffset = 0;
        perform_air_step(&m, AIR_STEP_CHECK_NONE);
        EXPECT_EQ(sCapSurfaceHits, 0);
    }
    return TRUE;
}

/* Follow TRUE into the actual destination, as Mario's dispatch loop does. */
static int wall_expiry_dispatch(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int hard = 0; hard < 2; hard++) {
            mode = 4;
            struct MarioState m = fresh();
            m.action = ACT_LONG_JUMP;
            m.forwardVel = hard ? 48 : 20;
            m.vel[2] = m.forwardVel;
            m.pos[1] = 1000;
            m.intendedYaw = 0;
            m.input = INPUT_NONZERO_ANALOG;
            mario_update_wall_kick_slide_input(&m);
            act_long_jump(&m);
            for (int frame = 2; frame <= 8; frame++) {
                gGlobalTimer++;
                m.input = 0;
                act_air_hit_wall(&m);
            }
            gGlobalTimer++;
            int before = stepCalls;
            f32 y = m.pos[1], vy = m.vel[1];
            int redispatch = act_air_hit_wall(&m);
            EXPECT_EQ(m.action, hard ? ACT_BACKWARD_AIR_KB : ACT_SOFT_BONK);
            if (redispatch) {
                if (hard) {
                    act_backward_air_kb(&m);
                } else {
                    act_soft_bonk(&m);
                }
            }
            EXPECT_EQ(stepCalls - before, 1);
            NEAR(m.pos[1], y + vy);
            NEAR(m.vel[1], vy - 4);
            NEAR(m.forwardVel, hard ? -24 : -10);
            gGlobalTimer++;
            if (hard) {
                act_backward_air_kb(&m);
            } else {
                act_soft_bonk(&m);
            }
            EXPECT_EQ(stepCalls - before, 2);
        }
    }
    return TRUE;
}

static int wall_post_step_exits(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int exit = 0; exit < 3; exit++) {
            if (exit == 0) {
                mode = 0;
            } else if (exit == 1) {
                mode = 1;
            } else {
                mode = 4;
            }
            struct MarioState m = fresh();
            m.action = ACT_AIR_HIT_WALL;
            m.wallKickContactFrame = gGlobalTimer - 1;
            m.input = exit == 1 ? INPUT_NONZERO_ANALOG : 0;
            m.pos[1] = exit == 2 ? 1000 : 0;
            m.vel[1] = exit == 1 ? -1 : -10;
            if (exit == 0) {
                ground.originOffset = 0;
            }
            if (exit == 2) {
                wallB.type = SURFACE_BURNING;
            }
            const u32 actions[] = { ACT_FREEFALL_LAND, ACT_LEDGE_GRAB, ACT_LAVA_BOOST };
            EXPECT_EQ(act_air_hit_wall(&m), FALSE);
            EXPECT_EQ(m.action, actions[exit]);
            EXPECT_EQ(animation, MARIO_ANIM_GENERAL_FALL);
            EXPECT_EQ(stepCalls, 1);
        }
    }
    return TRUE;
}

static struct MarioState hanging_start(f32 speed) {
    mode = 0;
    struct MarioState m = fresh();
    m.action = ACT_START_HANGING;
    m.input = INPUT_A_DOWN;
    ceilingY = 300;
    ceiling = (struct Surface){ .normal.y = -1, .type = SURFACE_HANGABLE };
    m.ceil = &ceiling;
    m.ceilHeight = ceilingY;
    m.pos[1] = ceilingY - HANG_DISTANCE;
    m.faceAngle[1] = 0;
    mario_set_forward_vel(&m, speed);
    return m;
}

static int hanging_grab_momentum(void) {
    const f32 speeds[] = { 0, 8, 40, 100, -40, -100 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned i = 0; i < sizeof(speeds) / sizeof(*speeds); i++) {
            struct MarioState m = hanging_start(speeds[i]);
            f32 retained = CLAMP(speeds[i] * .5f, -24, 24);
            act_start_hanging(&m);
            NEAR(m.pos[2], retained);
            NEAR(m.forwardVel, approach_f32(retained, 0, 2, 2));
            NEAR(m.pos[1], 156);
            f32 lastZ = m.pos[2];
            for (int frame = 0; frame < 16; frame++) {
                f32 before = m.forwardVel;
                act_start_hanging(&m);
                NEAR(m.pos[2] - lastZ, before);
                NEAR(m.forwardVel, approach_f32(before, 0, 2, 2));
                if (speeds[i] >= 0) {
                    EXPECT_EQ(m.pos[2] >= lastZ, TRUE);
                } else {
                    EXPECT_EQ(m.pos[2] <= lastZ, TRUE);
                }
                lastZ = m.pos[2];
            }
            NEAR(m.forwardVel, 0);
            NEAR(m.vel[0], 0);
            NEAR(m.vel[2], 0);
            f32 stopped = m.pos[2];
            act_start_hanging(&m);
            NEAR(m.pos[2], stopped);
        }
    }
    struct MarioState m = hanging_start(100);
    m.input |= INPUT_NONZERO_ANALOG;
    m.intendedMag = 32;
    m.intendedYaw = 0;
    act_start_hanging(&m);
    NEAR(m.forwardVel, 22);
    act_start_hanging(&m);
    EXPECT_EQ(m.action, ACT_HANGING);
    m.action = ACT_HANG_MOVING;
    update_hang_moving(&m);
    NEAR(m.forwardVel, 20);
    NEAR(m.pos[2], 44);
    ceiling.type = SURFACE_DEFAULT;
    return TRUE;
}

static int hanging_grab_boundaries(void) {
    struct MarioState m = hanging_start(40);
    mode = 15;
    act_start_hanging(&m);
    NEAR(m.pos[2], 20);
    NEAR(m.forwardVel, 0);
    act_start_hanging(&m);
    NEAR(m.pos[2], 20);
    m = hanging_start(40);
    ceilingAtBlockedFootprint = TRUE;
    act_start_hanging(&m);
    EXPECT_EQ(m.action, ACT_FREEFALL);
    NEAR(m.pos[2], 0);
    m = hanging_start(40);
    m.input |= INPUT_Z_PRESSED;
    act_start_hanging(&m);
    EXPECT_EQ(m.action, ACT_GROUND_POUND);
    NEAR(m.pos[2], 0);
    m = hanging_start(40);
    m.input |= INPUT_B_PRESSED;
    act_start_hanging(&m);
    EXPECT_EQ(m.action, ACT_FREEFALL);
    NEAR(m.pos[2], 0);
    m = hanging_start(40);
    m.ceil = NULL;
    act_start_hanging(&m);
    EXPECT_EQ(m.action, ACT_FREEFALL);
    m = hanging_start(40);
    m.flags = MARIO_METAL_CAP;
    m.waterLevel = m.pos[1] + 140;
    act_start_hanging(&m);
    NEAR(m.pos[2], 13);
    NEAR(m.forwardVel, 18);
    ceiling.type = SURFACE_DEFAULT;
    return TRUE;
}

static struct MarioState air_twirl_start(void) {
    struct MarioState m = fresh();
    mode = 0;
    m.action = ACT_FREEFALL;
    m.pos[1] = 5000.0f;
    m.peakHeight = 7000.0f;
    m.vel[1] = -10.0f;
    m.input = 0;
    mario_set_forward_vel(&m, 20.0f);
    controller.buttonDown = controller.buttonPressed = L_TRIG;
    mario_air_twirl_init(&m);
    return m;
}

static int jump_kick_twirl_cancel(void) {
    for (int pressedAt = 13; pressedAt <= 16; pressedAt++) {
        struct MarioState m = air_twirl_start();
        m.action = ACT_JUMP_KICK;
        m.actionState = ACT_STATE_JUMP_KICK_KICKING;
        for (int frame = 13; frame <= 16; frame++) {
            m.actionTimer = frame - 1;
            controller.buttonPressed = frame == pressedAt ? L_TRIG : 0;
            controller.buttonDown = 0;
            mario_try_air_twirl(&m);
            EXPECT_EQ(m.action, frame == 16 && pressedAt >= 14 ? ACT_L_AIR_TWIRL : ACT_JUMP_KICK);
        }
    }
    struct MarioState m = air_twirl_start();
    m.action = ACT_JUMP_KICK;
    m.actionTimer = 30;
    m.actionState = ACT_STATE_JUMP_KICK_RECOVERY;
    controller.buttonDown = 0;
    EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
    EXPECT_EQ(m.action, ACT_L_AIR_TWIRL);
    return TRUE;
}

static int wall_twirl_sound_gap(void) {
    struct MarioState m = air_twirl_start();
    mario_start_air_twirl(&m, FALSE);
    EXPECT_EQ(twirlSounds, 1);
    m.wall = &wallB;
    mario_bonk_reflection(&m, FALSE);
    EXPECT_EQ(bonks, 0);
    for (int frame = 0; frame < MARIO_WALL_TWIRL_SOUND_GAP_FRAMES - 1; frame++) {
        mario_try_air_twirl(&m);
        mario_bonk_reflection(&m, FALSE);
        EXPECT_EQ(bonks, 0);
    }
    mario_try_air_twirl(&m);
    mario_bonk_reflection(&m, FALSE);
    EXPECT_EQ(bonks, 1);
    mario_play_air_twirl_sound(&m);
    EXPECT_EQ(twirlSounds, 1);
    for (int frame = 0; frame < MARIO_WALL_TWIRL_SOUND_GAP_FRAMES; frame++) {
        mario_try_air_twirl(&m);
    }
    mario_play_air_twirl_sound(&m);
    EXPECT_EQ(twirlSounds, 2);
    // Ordinary impacts remain audible during the shared gap.
    mario_play_bonk_sound(&m);
    EXPECT_EQ(bonks, 2);

    m = air_twirl_start();
    m.wall = &wallB;
    controller.buttonDown = controller.buttonPressed = 0;
    begin_wall_kick_window(&m);
    EXPECT_EQ(bonks, 1);
    controller.buttonDown = L_TRIG;
    EXPECT_EQ(mario_try_wall_air_twirl(&m), TRUE);
    EXPECT_EQ(m.action, ACT_L_AIR_TWIRL);
    EXPECT_EQ(twirlSounds, 0);
    mario_bonk_reflection(&m, FALSE);
    EXPECT_EQ(bonks, 1);
    return TRUE;
}

static int air_twirl_coin_reset(void) {
    for (int flying = 0; flying < 2; flying++) {
        for (int interrupted = 0; interrupted < 2; interrupted++) {
            struct MarioState m = air_twirl_start();
            if (flying) {
                m.action = ACT_FLYING;
                m.flags = MARIO_WING_CAP;
            }
            EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
            m.airTwirlWingResetUsed = TRUE;
            mario_air_twirl_reset_on_coin(&m);
            EXPECT_EQ(mario_try_air_twirl(&m), FALSE); // A pickup cannot restart an active spin.
            EXPECT_EQ(m.airTwirlUsed, FALSE);
            EXPECT_EQ(m.airTwirlWingResetUsed, FALSE);
            controller.buttonPressed = 0;
            if (interrupted) {
                set_mario_action(&m, ACT_FREEFALL, 0);
            } else {
                for (int frame = 0; frame < (flying ? 16 : 8); frame++) {
                    if (flying) {
                        mario_air_twirl_flying_action(&m);
                    } else {
                        mario_air_twirl_action(&m);
                    }
                }
            }
            EXPECT_EQ(m.airTwirlCooldown, 0);
            controller.buttonPressed = L_TRIG;
            EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
            EXPECT_EQ(m.airTwirlCooldownResetByCoin, FALSE);
            EXPECT_EQ(m.airTwirlUsed, TRUE);
            EXPECT_EQ(m.airTwirlWingResetUsed, flying && interrupted);
            set_mario_action(&m, ACT_FREEFALL, 0);
            EXPECT_EQ(m.airTwirlCooldown, 30); // The earned reset applies to only one cycle.
            mario_air_twirl_init(&m);
            EXPECT_EQ(m.airTwirlCooldownResetByCoin, FALSE);
        }
    }
    // The next ordinary twirl gets its original braking or Wing boost again.
    for (int wing = 0; wing < 2; wing++) {
        struct MarioState m = air_twirl_start();
        m.flags = wing ? MARIO_WING_CAP : 0;
        m.airTwirlUsed = TRUE;
        m.airTwirlWingResetUsed = TRUE;
        m.airTwirlCooldown = 30;
        mario_air_twirl_reset_on_coin(&m);
        m.vel[1] = -10.0f;
        EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
        EXPECT_EQ(m.airTwirlGravity, TRUE);
        EXPECT_EQ(m.airTwirlUsed, TRUE);
        EXPECT_EQ(m.airTwirlWingResetUsed, wing);
        NEAR(m.vel[1], wing ? 20.0f : -3.0f);
        EXPECT_EQ(m.airTwirlWingControlTimer, wing ? 30 : 0);
        set_mario_action(&m, ACT_FREEFALL, 0);
        EXPECT_EQ(m.airTwirlCooldown, 30);
    }
    return TRUE;
}

static int air_twirl_cycle(void) {
    const u32 zHeldActions[] = { ACT_JUMP,     ACT_BACKFLIP,   ACT_LONG_JUMP,
                                 ACT_FREEFALL, ACT_WATER_JUMP, ACT_FLYING };
    for (u32 action = 0; action < sizeof(zHeldActions) / sizeof(*zHeldActions); action++) {
        for (s32 freshZ = FALSE; freshZ <= TRUE; freshZ++) {
            struct MarioState m = air_twirl_start();
            m.action = zHeldActions[action];
            m.actionTimer = WATER_JUMP_WALL_KICK_LOCK_FRAMES + 1;
            m.flags = m.action == ACT_FLYING ? MARIO_WING_CAP : 0;
            m.input = INPUT_Z_DOWN | (freshZ ? INPUT_Z_PRESSED : 0);
            controller.buttonDown = L_TRIG | Z_TRIG;
            controller.buttonPressed = L_TRIG | (freshZ ? Z_TRIG : 0);
            EXPECT_EQ(mario_try_air_twirl(&m), !freshZ);
            EXPECT_EQ(m.action, freshZ || zHeldActions[action] == ACT_FLYING ? zHeldActions[action]
                                                                             : ACT_L_AIR_TWIRL);
            EXPECT_EQ(m.input, INPUT_Z_DOWN | (freshZ ? INPUT_Z_PRESSED : 0));
            EXPECT_EQ(controller.buttonDown, L_TRIG | Z_TRIG);
            EXPECT_EQ(m.airTwirlContinue, !freshZ);
            if (!freshZ && zHeldActions[action] != ACT_FLYING) {
                controller.buttonPressed = 0;
                for (s32 frame = 0; frame < 8; frame++) {
                    EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
                    EXPECT_EQ(m.airTwirlContinue, TRUE);
                    EXPECT_EQ(mario_air_twirl_action(&m), FALSE);
                    EXPECT_EQ(m.action, frame < 7 ? ACT_L_AIR_TWIRL : ACT_FREEFALL);
                }
            }
        }
    }
    // L then A accepts exactly one update of separation, held or released.
    for (int delay = 1; delay <= 2; delay++) {
        for (int held = 0; held < 2; held++) {
            struct MarioState m = air_twirl_start();
            m.action = ACT_IDLE;
            m.pos[1] = m.floorHeight;
            EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
            controller.buttonPressed = 0;
            for (int frame = 1; frame < delay; frame++) {
                EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
            }
            controller.buttonDown = held ? L_TRIG : 0;
            m.input = INPUT_A_PRESSED;
            EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
            m.action = ACT_JUMP;
            m.input = 0;
            controller.buttonDown = 0; // A completed chord survives later release.
            EXPECT_EQ(mario_try_air_twirl(&m), delay == 1);
            EXPECT_EQ(m.airTwirlJumpQueued, FALSE);
            EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        }
    }
    for (int held = 0; held < 2; held++) {
        struct MarioState m = air_twirl_start();
        m.action = ACT_IDLE;
        m.pos[1] = m.floorHeight;
        m.input = INPUT_A_PRESSED;
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        m.action = ACT_JUMP; // Ground action dispatch launches after input sampling.
        m.vel[1] = 42;
        m.input = 0;
        controller.buttonPressed = 0;
        controller.buttonDown = held ? L_TRIG : 0;
        EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
        EXPECT_EQ(m.action, ACT_L_AIR_TWIRL);
        NEAR(m.vel[1], 42);
        EXPECT_EQ(m.airTwirlJumpQueued, FALSE);
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
    }
    for (int blocker = 0; blocker < 6; blocker++) {
        struct MarioState m = air_twirl_start();
        m.action = ACT_IDLE;
        m.pos[1] = m.floorHeight;
        m.input = INPUT_A_PRESSED;
        if (blocker == 0) {
            m.input |= INPUT_Z_DOWN;
        } else if (blocker == 1) {
            m.input |= INPUT_STOMPED;
        } else if (blocker == 2) {
            m.airTwirlCooldown = 5;
        } else if (blocker == 3) {
            m.heldObj = &marioObject;
        } else if (blocker == 4) {
            m.riddenObj = &marioObject;
        } else {
            m.input |= INPUT_OFF_FLOOR;
        }
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        EXPECT_EQ(m.airTwirlJumpQueued, FALSE);
        m.input = 0;
        m.heldObj = m.riddenObj = NULL;
        m.airTwirlCooldown = 0;
        m.action = ACT_JUMP;
        controller.buttonPressed = 0;
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
    }
    // Moving L can start a ground twirl before A completes the chord.
    for (int held = FALSE; held <= TRUE; held++) {
        struct MarioState m = air_twirl_start();
        m.action = ACT_WALKING;
        m.pos[1] = m.floorHeight;
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        m.action = ACT_GROUND_TWIRL;
        m.actionTimer = 1;
        controller.buttonPressed = 0;
        controller.buttonDown = held ? L_TRIG : 0;
        m.input = INPUT_A_PRESSED;
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        EXPECT_EQ(m.airTwirlJumpQueued, TRUE);
        m.action = ACT_JUMP; // The ground twirl handler accepts this early chord.
        m.input = 0;
        EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
    }
    // Restrictions on the A update still reject a previously eligible L edge.
    for (int blocker = 0; blocker < 6; blocker++) {
        struct MarioState m = air_twirl_start();
        m.action = ACT_IDLE;
        m.pos[1] = m.floorHeight;
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        controller.buttonPressed = 0;
        m.input = INPUT_A_PRESSED;
        if (blocker == 0) {
            m.input |= INPUT_Z_DOWN;
        } else if (blocker == 1) {
            m.input |= INPUT_STOMPED;
        } else if (blocker == 2) {
            m.airTwirlCooldown = 5;
        } else if (blocker == 3) {
            m.heldObj = &marioObject;
        } else if (blocker == 4) {
            m.input |= INPUT_OFF_FLOOR;
        } else {
            m.action = ACT_WATER_IDLE;
        }
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        m.input = 0;
        m.heldObj = NULL;
        m.airTwirlCooldown = 0;
        m.action = ACT_JUMP;
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
    }
    // An interrupted or rejected jump cannot carry the press into a later jump.
    for (int interrupted = 0; interrupted < 2; interrupted++) {
        struct MarioState m = air_twirl_start();
        m.action = ACT_IDLE;
        m.pos[1] = m.floorHeight;
        m.input = INPUT_A_PRESSED;
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        controller.buttonPressed = 0;
        m.input = interrupted ? INPUT_STOMPED : 0;
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        m.action = ACT_JUMP;
        m.input = 0;
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
    }

    for (int held = 0; held < 2; held++) {
        struct MarioState m = air_twirl_start();
        m.wallKickTimer = m.wallKickInputFrames = 5;
        m.wallKickFromDoubleJump = TRUE;
        EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
        NEAR(m.vel[1], -3);
        NEAR(m.peakHeight, 5000);
        EXPECT_EQ(m.flags & MARIO_JUMPING, 0);
        EXPECT_EQ(m.wallKickTimer, 0);
        EXPECT_EQ(m.wallKickInputFrames, 0);
        EXPECT_EQ(m.wallKickFromDoubleJump, FALSE);
        controller.buttonPressed = 0;
        controller.buttonDown = held ? L_TRIG : 0;
        for (int frame = 0; frame < 8; frame++) {
            EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
            EXPECT_EQ(mario_air_twirl_action(&m), FALSE);
            EXPECT_EQ(m.action, frame < 7 ? ACT_L_AIR_TWIRL : ACT_FREEFALL);
            if (frame == 0) {
                NEAR(m.vel[1], -3.08f); // Gravity runs once across the collision substeps.
            }
        }
        NEAR(m.peakHeight, 5000);
        EXPECT_EQ(m.marioObj->header.gfx.angle[1], m.faceAngle[1]);
        EXPECT_EQ(m.airTwirlContinue, held);
        EXPECT_EQ(m.airTwirlCooldown, 30);
        EXPECT_EQ(sAirTwirlContacts, 1);
        controller.buttonDown = L_TRIG;
        for (int wait = 0; wait < 30; wait++) {
            // An early press is discarded, not queued until cooldown expiry.
            controller.buttonPressed = wait == 0 ? L_TRIG : 0;
            EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        }
        controller.buttonPressed = L_TRIG;
        f32 repeatVelocity = m.vel[1];
        EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
        NEAR(m.vel[1], repeatVelocity);
        EXPECT_EQ(m.airTwirlGravity, FALSE);
        EXPECT_EQ(sAirTwirlContacts, 2);
    }
    struct MarioState m = air_twirl_start();
    m.action = ACT_FLYING;
    EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
    m.action = ACT_FREEFALL;
    controller.buttonPressed = 0;
    EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
    m = air_twirl_start();
    m.action = ACT_WATER_JUMP;
    controller.buttonPressed = L_TRIG;
    m.actionTimer = WATER_JUMP_WALL_KICK_LOCK_FRAMES;
    EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
    m.actionTimer++;
    EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
    for (int blocked = 0; blocked < 2; blocked++) {
        for (int age = 1; age <= 6; age++) {
            m = air_twirl_start();
            m.action = blocked ? ACT_FLYING : ACT_FREEFALL;
            m.airTwirlCooldown = blocked ? 0 : 10;
            controller.buttonPressed = L_TRIG;
            EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
            controller.buttonPressed = controller.buttonDown = 0;
            for (int frame = 1; frame < age; frame++) {
                EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
            }
            m.action = ACT_FREEFALL;
            m.airTwirlCooldown = 0;
            EXPECT_EQ(mario_try_air_twirl(&m), age < 6);
            EXPECT_EQ(m.airTwirlInputFrames, 0);
        }
    }
    return TRUE;
}

static int air_twirl_enemy_bounce_interrupt(void) {
    for (int phase = 0; phase < 2; phase++) {
        struct MarioState m = air_twirl_start();
        m.action = ACT_TWIRLING;
        m.actionArg = phase;
        m.vel[1] = 50.0f;
        controller.buttonPressed = 0;
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        EXPECT_EQ(m.action, ACT_TWIRLING);
        controller.buttonPressed = L_TRIG;
        m.airTwirlCooldown = 2;
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        EXPECT_EQ(m.action, ACT_TWIRLING);
        m.airTwirlCooldown = 0;
        m.input = INPUT_Z_DOWN | INPUT_SQUISHED;
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        EXPECT_EQ(m.action, ACT_TWIRLING);
        m.input = INPUT_Z_DOWN;
        controller.buttonDown |= Z_TRIG;
        EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
        EXPECT_EQ(m.action, ACT_L_AIR_TWIRL);
        EXPECT_EQ(m.actionArg, 0);
        NEAR(m.vel[1], 50.0f);
        NEAR(m.forwardVel, 20.0f);
        NEAR(m.peakHeight, 7000.0f);
        EXPECT_EQ(sAirTwirlContacts, 1);
        controller.buttonDown = controller.buttonPressed = 0;
        for (int frame = 0; frame < 8; frame++) {
            mario_air_twirl_action(&m);
        }
        EXPECT_EQ(m.action, ACT_FREEFALL);
        EXPECT_EQ(m.airTwirlCooldown, MARIO_TWIRL_COOLDOWN_FRAMES);
    }
    return TRUE;
}

static int air_twirl_gravity(void) {
    struct MarioState m = air_twirl_start();
    const f32 fallingSpeeds[] = { -1.0f, -40.0f, -75.0f };
    for (unsigned i = 0; i < sizeof(fallingSpeeds) / sizeof(fallingSpeeds[0]); i++) {
        m = air_twirl_start();
        m.vel[1] = fallingSpeeds[i];
        EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
        NEAR(m.vel[1], fallingSpeeds[i] * 0.3f);
    }
    m = air_twirl_start();
    mario_try_air_twirl(&m);
    apply_gravity(&m);
    NEAR(m.vel[1], -3.08f);
    m.airTwirlGravity = FALSE;
    m.vel[1] = -10;
    apply_gravity(&m);
    NEAR(m.vel[1], -14);
    m = air_twirl_start();
    m.action = ACT_LONG_JUMP;
    struct MarioState baselineFall = m;
    mario_try_air_twirl(&m);
    apply_gravity(&baselineFall);
    apply_gravity(&m);
    NEAR(m.vel[1] + 3, (baselineFall.vel[1] + 10) * 0.02f);
    const u32 jumps[] = { ACT_JUMP, ACT_DOUBLE_JUMP, ACT_TRIPLE_JUMP, ACT_BACKFLIP, ACT_LONG_JUMP };
    for (unsigned i = 0; i < sizeof(jumps) / sizeof(jumps[0]); i++) {
        for (int held = 0; held < 2; held++) {
            m = air_twirl_start();
            m.action = jumps[i];
            m.flags = MARIO_JUMPING;
            m.vel[1] = 40;
            m.input = held ? INPUT_A_DOWN : 0;
            struct MarioState baseline = m;
            mario_try_air_twirl(&m);
            NEAR(m.vel[1], 40);
            apply_gravity(&baseline);
            apply_gravity(&m);
            NEAR(m.vel[1], baseline.vel[1]);
        }
    }
    m = air_twirl_start();
    m.flags = MARIO_WING_CAP;
    mario_try_air_twirl(&m);
    NEAR(m.vel[1], 20);
    EXPECT_EQ(m.airTwirlWingResetUsed, TRUE);
    set_mario_action(&m, ACT_FREEFALL, 0);
    m.vel[1] = -20;
    controller.buttonPressed = 0;
    for (int wait = 0; wait < 30; wait++) {
        mario_try_air_twirl(&m);
    }
    controller.buttonPressed = L_TRIG;
    EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
    NEAR(m.vel[1], -20);
    m = air_twirl_start();
    m.flags = MARIO_WING_CAP;
    m.vel[1] = 40;
    mario_try_air_twirl(&m);
    NEAR(m.vel[1], 60);
    EXPECT_EQ(m.airTwirlWingResetUsed, TRUE);
    set_mario_action(&m, ACT_FREEFALL, 0);
    m.vel[1] = -20;
    controller.buttonPressed = 0;
    for (int wait = 0; wait < 30; wait++) {
        mario_try_air_twirl(&m);
    }
    controller.buttonPressed = L_TRIG;
    EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
    NEAR(m.vel[1], -20);
    return TRUE;
}

static int air_twirl_wall_and_landing(void) {
    for (int cooldown = 0; cooldown <= 30; cooldown++) {
        struct MarioState landing = air_twirl_start();
        mario_try_air_twirl(&landing);
        landing.airTwirlCooldown = cooldown;
        landing.pos[1] = ground.originOffset + 1;
        controller.buttonDown = 0;
        mario_air_twirl_action(&landing);
        EXPECT_EQ(landing.action, ACT_FREEFALL_LAND);
        EXPECT_EQ(landing.airTwirlCooldown, cooldown);
    }

    struct MarioState m = air_twirl_start();
    m.airTwirlUsed = m.airTwirlWingResetUsed = TRUE;
    mario_try_air_twirl(&m);
    EXPECT_EQ(m.airTwirlGravity, FALSE);
    m.actionTimer = 7;
    m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
    m.marioObj->header.gfx.angle[1] = 0x1234;
    mode = 4;
    mario_air_twirl_action(&m);
    EXPECT_EQ(m.action, ACT_L_AIR_TWIRL);
    EXPECT_EQ(m.actionTimer, 0);
    EXPECT_EQ(m.airTwirlGravity, TRUE);
    EXPECT_EQ(m.airTwirlWingResetUsed, TRUE);
    EXPECT_EQ(m.wallKickTimer, 0);
    EXPECT_EQ(m.vel[2] < 0, TRUE);
    EXPECT_EQ(m.marioObj->header.gfx.angle[1], 0x1234);
    EXPECT_EQ(m.vel[1] < -10, TRUE);
    m.faceAngle[1] = 0;
    mario_air_twirl_action(&m);
    EXPECT_EQ(m.actionTimer, 1); // Persistent wall overlap cannot refresh.
    mode = 0;
    mario_air_twirl_action(&m);
    EXPECT_EQ(m.airTwirlWallContact, FALSE);
    mode = 4;
    m.faceAngle[1] = 0;
    mario_air_twirl_action(&m);
    EXPECT_EQ(m.actionTimer, 0);
    wallB.type = SURFACE_BURNING;
    m.faceAngle[1] = 0;
    mario_air_twirl_action(&m);
    EXPECT_EQ(m.action, ACT_LAVA_BOOST);
    for (int held = 0; held < 2; held++) {
        m = air_twirl_start();
        m.airTwirlWingResetUsed = TRUE;
        mario_try_air_twirl(&m);
        m.pos[1] = ground.originOffset + 1;
        controller.buttonDown = held ? L_TRIG : 0;
        mario_air_twirl_action(&m);
        EXPECT_EQ(m.action, held ? ACT_GROUND_TWIRL : ACT_FREEFALL_LAND);
        NEAR(m.pos[1], ground.originOffset);
        EXPECT_EQ(m.airTwirlUsed, FALSE);
        EXPECT_EQ(m.airTwirlWingResetUsed, FALSE);
    }
    m = air_twirl_start();
    mario_try_air_twirl(&m);
    m.pos[1] = ground.originOffset + 1;
    sAirTwirlLandingBlocked = TRUE;
    mario_air_twirl_action(&m);
    EXPECT_EQ(m.action, ACT_HARD_BACKWARD_GROUND_KB);
    EXPECT_EQ(m.airTwirlContinue, FALSE);
    m = air_twirl_start();
    mario_try_air_twirl(&m);
    set_mario_action(&m, ACT_WATER_PLUNGE, 0);
    EXPECT_EQ(m.airTwirlUsed, FALSE);
    EXPECT_EQ(m.airTwirlWingResetUsed, FALSE);
    EXPECT_EQ(m.airTwirlContinue, FALSE);
    EXPECT_EQ(m.airTwirlCooldown, 30); // Water restores allowances, not either cooldown.
    return TRUE;
}

static int wing_air_twirl_entry_direction(void) {
    const s16 directions[] = { 0x4000, -0x4000, -0x8000 };
    for (unsigned direction = 0; direction < sizeof(directions) / sizeof(directions[0]); direction++) {
        for (int wing = 0; wing < 2; wing++) {
            for (int stick = 0; stick < 2; stick++) {
                for (int used = 0; used < 2; used++) {
                    struct MarioState m = air_twirl_start();
                    m.flags = wing ? MARIO_WING_CAP : 0;
                    m.input = stick ? INPUT_NONZERO_ANALOG : 0;
                    m.airTwirlUsed = used;
                    m.intendedYaw = directions[direction];
                    m.intendedMag = 32;
                    m.vel[0] = m.slideVelX = 12;
                    m.vel[2] = m.slideVelZ = 16;
                    m.vel[1] = 10;
                    mario_start_air_twirl(&m, FALSE);
                    if (wing && stick) {
                        EXPECT_EQ(m.faceAngle[1], directions[direction]);
                        NEAR(m.vel[0], 20 * sins(m.intendedYaw));
                        NEAR(m.vel[2], 20 * coss(m.intendedYaw));
                        NEAR(m.forwardVel, 20);
                        m.intendedYaw = m.faceAngle[1] + 0x4000;
                        s16 entryYaw = m.faceAngle[1];
                        update_air_with_turn(&m);
                        EXPECT_EQ(m.faceAngle[1] != m.intendedYaw, TRUE);
                        EXPECT_EQ(m.faceAngle[1] != entryYaw, TRUE);
                    } else {
                        EXPECT_EQ(m.faceAngle[1], 0);
                        NEAR(m.vel[0], 12);
                        NEAR(m.vel[2], 16);
                    }
                    NEAR(m.vel[1], wing ? 30 : 10);
                }
            }
        }
    }
    struct MarioState m = air_twirl_start();
    m.flags = MARIO_WING_CAP;
    m.action = ACT_FLYING;
    m.input = INPUT_NONZERO_ANALOG;
    m.intendedYaw = 0x4000;
    mario_start_air_twirl(&m, FALSE);
    EXPECT_EQ(m.action, ACT_FLYING);
    EXPECT_EQ(m.faceAngle[1], 0);
    NEAR(m.vel[2], 20);
    return TRUE;
}

static int wing_air_twirl_upward_boost(void) {
    const f32 velocities[] = { -75.0f, -1.0f, 0.0f, 40.0f };
    for (unsigned i = 0; i < sizeof(velocities) / sizeof(velocities[0]); i++) {
        struct MarioState m = air_twirl_start();
        m.flags = MARIO_WING_CAP;
        m.vel[1] = velocities[i];
        mario_try_air_twirl(&m);
        NEAR(m.vel[1], MAX(0.0f, velocities[i]) + 20.0f);
        NEAR(m.peakHeight, velocities[i] < 0.0f ? m.pos[1] : 7000.0f);
        EXPECT_EQ(m.airTwirlWingResetUsed, TRUE);
        NEAR(m.forwardVel, 20.0f);
        set_mario_action(&m, ACT_FREEFALL, 0);
        m.vel[1] = -30.0f;
        controller.buttonPressed = 0;
        for (int frame = 0; frame < 30; frame++) {
            mario_try_air_twirl(&m);
        }
        controller.buttonPressed = L_TRIG;
        EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
        NEAR(m.vel[1], -30.0f);
    }
    return TRUE;
}

static int wing_air_twirl_entry_wall_boost(void) {
    const s16 directions[] = { 0, 0x4000, -0x8000 };
    for (unsigned i = 0; i < sizeof(directions) / sizeof(directions[0]); i++) {
        struct MarioState m = air_twirl_start();
        mode = 9;
        edgeWallEnabled = TRUE;
        m.flags = MARIO_WING_CAP;
        m.input = INPUT_NONZERO_ANALOG;
        m.intendedYaw = directions[i];
        m.pos[2] = -10; // Incoming movement reaches the wall radius; current position does not.
        mario_set_forward_vel(&m, 20);
        mario_start_air_twirl(&m, FALSE);
        f32 boost = (20.0f + 9.0f) * 1.3f;
        NEAR(m.forwardVel, boost);
        NEAR(m.vel[0], boost * sins(m.intendedYaw));
        NEAR(m.vel[2], boost * coss(m.intendedYaw));
        NEAR(m.airTwirlWingSpeedLimit, boost);
        NEAR(m.pos[2], -10);
        EXPECT_EQ(m.airTwirlWallBounceCount, 1);
        EXPECT_EQ(mario_air_twirl_bounce_off_wall(&m), FALSE);
    }
    for (int excluded = 0; excluded < 4; excluded++) {
        struct MarioState m = air_twirl_start();
        mode = 9;
        edgeWallEnabled = TRUE;
        m.flags = excluded == 0 ? 0 : MARIO_WING_CAP;
        m.input = INPUT_NONZERO_ANALOG;
        m.intendedYaw = -0x8000;
        m.pos[2] = excluded == 1 ? -100 : -10;
        mario_set_forward_vel(&m, excluded == 2 ? -20 : 20);
        if (excluded == 3) {
            wallB.type = SURFACE_BURNING;
        }
        mario_start_air_twirl(&m, FALSE);
        EXPECT_EQ(m.airTwirlWallBounceCount, 0);
        NEAR(fabsf(m.forwardVel), 20);
    }
    edgeWallEnabled = FALSE;
    return TRUE;
}

static int wing_air_twirl_control(void) {
    struct MarioState m = air_twirl_start();
    m.flags = MARIO_WING_CAP;
    mario_try_air_twirl(&m);
    NEAR(m.vel[1], 20.0f);
    EXPECT_EQ(m.airTwirlWingControlTimer, 30);
    NEAR(m.airTwirlWingSpeedLimit, 20.0f);
    apply_gravity(&m);
    NEAR(m.vel[1], 16.0f);
    m.vel[1] = 0.0f;
    apply_gravity(&m);
    NEAR(m.vel[1], -0.04f);
    for (int turning = 0; turning < 2; turning++) {
        struct MarioState normal = m;
        struct MarioState boosted = m;
        normal.airTwirlWingControlTimer = 0;
        normal.input = boosted.input = INPUT_NONZERO_ANALOG;
        normal.intendedMag = boosted.intendedMag = 32;
        normal.intendedYaw = boosted.intendedYaw = turning ? 0x4000 : 0;
        normal.faceAngle[1] = boosted.faceAngle[1] = 0;
        normal.forwardVel = boosted.forwardVel = 10;
        update_air_with_turn(&normal);
        update_air_with_turn(&boosted);
        if (turning) {
            EXPECT_EQ(boosted.faceAngle[1], normal.faceAngle[1] * 4);
        } else {
            NEAR(boosted.forwardVel, 10.0f);
        }
        NEAR(sqrtf(sqr(boosted.vel[0]) + sqr(boosted.vel[2])), 10.0f);
    }
    set_mario_action(&m, ACT_FREEFALL, 0);
    NEAR(mario_air_twirl_control_scale(&m), 4.0f);
    controller.buttonPressed = 0;
    for (int frame = 0; frame < 30; frame++) {
        mario_try_air_twirl(&m);
    }
    EXPECT_EQ(m.airTwirlWingControlTimer, 0);
    NEAR(mario_air_twirl_control_scale(&m), 1.0f);
    mario_start_air_twirl(&m, FALSE);
    EXPECT_EQ(m.airTwirlWingControlTimer, 0);
    m = air_twirl_start();
    m.flags = MARIO_WING_CAP;
    mario_start_air_twirl(&m, FALSE);
    m.airTwirlWingControlTimer = 12;
    m.wall = &wallB;
    wallB.normal.x = 0;
    wallB.normal.z = -1;
    m.wallYaw = m.faceAngle[1] = 0;
    mario_set_forward_vel(&m, 20);
    mario_air_twirl_bounce_off_wall(&m);
    EXPECT_EQ(m.airTwirlWingControlTimer, 12);
    f32 wallBoostCeiling = m.airTwirlWingSpeedLimit;
    EXPECT_EQ(wallBoostCeiling > 20.0f, TRUE);
    NEAR(sqrtf(sqr(m.vel[0]) + sqr(m.vel[2])), wallBoostCeiling);
    NEAR(m.forwardVel, wallBoostCeiling);
    m.faceAngle[1] = 0;
    mario_set_forward_vel(&m, m.forwardVel);
    EXPECT_EQ(mario_air_twirl_bounce_off_wall(&m), FALSE);
    NEAR(m.airTwirlWingSpeedLimit, wallBoostCeiling);
    m.input = INPUT_NONZERO_ANALOG;
    m.intendedMag = 32;
    m.intendedYaw = m.faceAngle[1];
    update_air_with_turn(&m);
    NEAR(m.forwardVel, wallBoostCeiling);
    m.flags = 0;
    mario_try_air_twirl(&m);
    EXPECT_EQ(m.airTwirlWingControlTimer, 0);
    m = air_twirl_start();
    m.flags = MARIO_WING_CAP;
    mario_start_air_twirl(&m, FALSE);
    set_mario_action(&m, ACT_WATER_PLUNGE, 0);
    EXPECT_EQ(m.airTwirlWingControlTimer, 0);
    m = air_twirl_start();
    m.flags = MARIO_WING_CAP;
    mario_start_air_twirl(&m, FALSE);
    m.pos[1] = ground.originOffset + 1;
    m.vel[1] = -2.0f;
    controller.buttonDown = 0;
    mario_air_twirl_action(&m);
    EXPECT_EQ(m.airTwirlWingControlTimer, 0);
    const f32 entrySpeeds[] = { 0.0f, 5.0f, 40.0f, -20.0f };
    for (unsigned i = 0; i < sizeof(entrySpeeds) / sizeof(entrySpeeds[0]); i++) {
        m = air_twirl_start();
        m.flags = MARIO_WING_CAP;
        mario_set_forward_vel(&m, entrySpeeds[i]);
        mario_start_air_twirl(&m, FALSE);
        NEAR(m.airTwirlWingSpeedLimit, fabsf(entrySpeeds[i]));
        mario_set_forward_vel(&m, entrySpeeds[i] < 0.0f ? -100.0f : 100.0f);
        m.vel[1] = -3.0f;
        mario_air_twirl_limit_control_speed(&m);
        NEAR(fabsf(m.forwardVel), fabsf(entrySpeeds[i]));
        NEAR(sqrtf(sqr(m.vel[0]) + sqr(m.vel[2])), fabsf(entrySpeeds[i]));
        NEAR(m.vel[1], -3.0f);
        m.airTwirlWingControlTimer = 0;
        mario_set_forward_vel(&m, 100.0f);
        mario_air_twirl_limit_control_speed(&m);
        NEAR(m.forwardVel, 100.0f);
    }
    return TRUE;
}

static int wing_air_twirl_momentum(void) {
    const f32 speeds[] = { 0.0f, 20.0f, 80.0f, -80.0f };
    for (unsigned speed = 0; speed < sizeof(speeds) / sizeof(speeds[0]); speed++) {
        for (int turning = 0; turning < 2; turning++) {
            for (int stick = 0; stick < 2; stick++) {
                struct MarioState m = air_twirl_start();
                m.flags = MARIO_WING_CAP;
                m.input = stick ? INPUT_NONZERO_ANALOG : 0;
                m.intendedMag = 32;
                m.intendedYaw = 0x4000;
                mario_set_forward_vel(&m, speeds[speed]);
                mario_start_air_twirl(&m, FALSE);
                f32 entrySpeed = m.forwardVel;
                for (int frame = 0; frame < 30; frame++) {
                    // Exercise the twirl-to-freefall handoff during the control period.
                    if (frame == 8) {
                        set_mario_action(&m, ACT_FREEFALL, 0);
                    }
                    m.intendedYaw = m.faceAngle[1] + 0x4000;
                    if (turning) {
                        update_air_with_turn(&m);
                    } else {
                        wing_update_air_without_turn(&m);
                    }
                    NEAR(m.forwardVel, entrySpeed);
                    NEAR(sqrtf(sqr(m.vel[0]) + sqr(m.vel[2])), fabsf(speeds[speed]));
                    mario_try_air_twirl(&m);
                }
                EXPECT_EQ(m.airTwirlWingControlTimer, 0);
                m.input = 0;
                wing_update_air_without_turn(&m);
                if (entrySpeed >= 0.0f) {
                    f32 drag = entrySpeed > 32.0f ? 1.35f : 0.35f;
                    if (entrySpeed - 0.35f > 64.0f) {
                        drag = 1.85f;
                    }
                    NEAR(m.forwardVel, MAX(0.0f, entrySpeed - drag));
                } else {
                    EXPECT_EQ(fabsf(m.forwardVel) < fabsf(entrySpeed), TRUE);
                }
            }
        }
    }
    // Losing the cap ends momentum preservation immediately.
    struct MarioState m = air_twirl_start();
    m.flags = MARIO_WING_CAP;
    mario_set_forward_vel(&m, 80);
    mario_start_air_twirl(&m, FALSE);
    m.flags = 0;
    m.input = 0;
    update_air_with_turn(&m);
    NEAR(m.forwardVel, 78.15f);
    return TRUE;
}

static int air_twirl_fall_damage(void) {
    for (int wing = 0; wing < 2; wing++) {
        struct MarioState m = air_twirl_start();
        m.flags = wing ? MARIO_WING_CAP : 0;
        m.vel[1] = -75.0f;
        EXPECT_EQ(fall_damage_severity(&m), FALL_DAMAGE_SMALL);
        mario_try_air_twirl(&m);
        NEAR(m.peakHeight, 5000.0f);
        // Even after speed returns, the pre-twirl drop must not count.
        set_mario_action(&m, ACT_FREEFALL, 0);
        m.pos[1] = 4500.0f;
        m.vel[1] = -56.0f;
        EXPECT_EQ(fall_damage_severity(&m), FALL_DAMAGE_NONE);
        m.pos[1] = 5000.0f - (wing ? 3001.0f : FALL_DAMAGE_HEIGHT_LARGE + 1.0f);
        EXPECT_EQ(fall_damage_severity(&m), FALL_DAMAGE_LARGE);
        // Re-entering without another velocity reduction cannot erase this fall.
        mario_start_air_twirl(&m, FALSE);
        NEAR(m.peakHeight, 5000.0f);
        EXPECT_EQ(fall_damage_severity(&m), FALL_DAMAGE_LARGE);
    }
    struct MarioState m = air_twirl_start();
    m.vel[1] = 20.0f;
    mario_try_air_twirl(&m);
    NEAR(m.peakHeight, 7000.0f);
    m = air_twirl_start();
    m.action = ACT_FLYING;
    m.flags = MARIO_WING_CAP;
    mario_try_air_twirl(&m);
    NEAR(m.peakHeight, 7000.0f);
    return TRUE;
}

static int twirl_wall_all_directions(void) {
    for (int angle = 0; angle < 360; angle += 15) {
        for (int flying = 0; flying < 2; flying++) {
            sImpactRumbles = sImpactTime = sImpactStrength = 0;
            struct MarioState m = air_twirl_start();
            m.action = flying ? ACT_FLYING : ACT_FREEFALL;
            m.flags = flying ? MARIO_WING_CAP : 0;
            mario_try_air_twirl(&m);
            m.faceAngle[0] = 0;
            m.faceAngle[1] = DEGREES(angle);
            wallB.normal.x = 0;
            wallB.normal.z = -1;
            struct WallCollisionData walls = { .numWalls = 1, .walls = { &wallB } };
            set_mario_wall(&m, NULL);
            mario_set_forward_vel(&m, 20);
            s16 facing = m.faceAngle[1];
            s32 oldContact =
                m.vel[2] > 0
                && abs_angle_diff(SURFACE_YAW(&wallB), facing) > DEGREES(180 - WALL_KICK_DEGREES);
            EXPECT_EQ(bonk_or_hit_lava_wall(&m, &walls), AIR_STEP_HIT_WALL);
            NEAR(m.forwardVel, 20);
            EXPECT_EQ(m.wall == &wallB, TRUE);
            EXPECT_EQ(mario_air_twirl_bounce_off_wall(&m), TRUE);
            NEAR(m.forwardVel, 37.7f);
            EXPECT_EQ(m.particleFlags & PARTICLE_VERTICAL_STAR, PARTICLE_VERTICAL_STAR);
            EXPECT_EQ(sImpactRumbles, 1);
            EXPECT_EQ(sImpactTime, 5);
            EXPECT_EQ(sImpactStrength, 40);
            EXPECT_EQ(m.faceAngle[1], oldContact ? (s16) (-facing + 0x8000) : facing);
            m.particleFlags = 0;
            EXPECT_EQ(mario_air_twirl_bounce_off_wall(&m), FALSE);
            EXPECT_EQ(m.particleFlags, 0);
            EXPECT_EQ(sImpactRumbles, 1);
            EXPECT_EQ(m.airTwirlWallBounceCount, 1);
            m.airTwirlWallContact = FALSE;
            EXPECT_EQ(mario_air_twirl_bounce_off_wall(&m), TRUE);
            EXPECT_EQ(m.airTwirlWallBounceCount, 2);
            EXPECT_EQ(m.particleFlags & PARTICLE_VERTICAL_STAR, PARTICLE_VERTICAL_STAR);
            EXPECT_EQ(sImpactRumbles, 2);
        }
        mode = 8;
        struct MarioState m = fresh();
        m.action = ACT_GROUND_TWIRL;
        m.faceAngle[1] = DEGREES(angle);
        m.pos[1] = ground.originOffset;
        Vec3f next = { 0, ground.originOffset, 0 };
        EXPECT_EQ(perform_ground_quarter_step(&m, next, FALSE), GROUND_STEP_HIT_WALL_CONTINUE_QSTEPS);
        EXPECT_EQ(m.wall == &wallA, TRUE);
    }
    return TRUE;
}
static int air_twirl_wall_speed(void) {
    const f32 entrySpeeds[] = { 20.0f, 40.0f, 40.2f, 64.0f, 128.0f };
    for (int form = 0; form < 3; form++) {
        int flying = form == 2;
        for (unsigned speed = 0; speed < sizeof(entrySpeeds) / sizeof(entrySpeeds[0]); speed++) {
            for (int backwards = 0; backwards < 2; backwards++) {
                struct MarioState m = air_twirl_start();
                m.action = flying ? ACT_FLYING : ACT_FREEFALL;
                m.flags = form != 0 ? MARIO_WING_CAP : 0;
                mario_try_air_twirl(&m);
                m.wall = &wallB;
                m.wallYaw = (s16) 0x8000;
                wallB.normal.x = 0;
                wallB.normal.z = -1;
                m.faceAngle[0] = flying ? 0x1000 : 0;
                m.faceAngle[1] = backwards ? (s16) 0x8000 : 0;
                mario_set_forward_vel(&m, backwards ? -entrySpeeds[speed] : entrySpeeds[speed]);
                f32 verticalSpeed = m.vel[1];
                EXPECT_EQ(mario_air_twirl_bounce_off_wall(&m), TRUE);
                f32 rebound = MIN((entrySpeeds[speed] + 9.0f) * 1.3f, 64.0f);
                NEAR(fabsf(m.forwardVel), rebound);
                EXPECT_EQ(m.vel[2] < 0.0f, TRUE);
                if (flying) {
                    NEAR(sqrtf(sqr(m.vel[0]) + sqr(m.vel[1]) + sqr(m.vel[2])), rebound);
                } else {
                    NEAR(sqrtf(sqr(m.vel[0]) + sqr(m.vel[2])), rebound);
                    NEAR(m.vel[1], verticalSpeed);
                }
            }
        }
    }
    for (int flying = 0; flying < 2; flying++) {
        for (int frame = 0; frame < (flying ? 16 : 8); frame++) {
            for (int backwards = 0; backwards < 2; backwards++) {
                struct MarioState m = air_twirl_start();
                m.action = flying ? ACT_FLYING : ACT_FREEFALL;
                m.flags = flying ? MARIO_WING_CAP : 0;
                mario_try_air_twirl(&m);
                m.actionTimer = frame;
                m.airTwirlFlightRollFrame = flying ? frame + 1 : 0;
                m.wall = &wallB;
                m.wallYaw = (s16) 0x8000;
                wallB.normal.x = 0;
                wallB.normal.z = -1;
                m.faceAngle[0] = 0;
                m.faceAngle[1] = backwards ? (s16) 0x8000 : 0;
                mario_set_forward_vel(&m, backwards ? -20.0f : 20.0f);
                f32 verticalSpeed = m.vel[1];

                EXPECT_EQ(mario_air_twirl_bounce_off_wall(&m), TRUE);
                f32 firstRebound = (20.0f + (frame < 3 ? 9.0f : 8.0f)) * 1.3f;
                NEAR(fabsf(m.forwardVel), firstRebound);
                EXPECT_EQ(m.airTwirlWallBounceCount, 1);
                EXPECT_EQ(m.vel[2] < 0, TRUE);
                NEAR(m.vel[1], flying ? 0.0f : verticalSpeed);
                // Even an incoming overlap must not multiply speed again.
                m.faceAngle[1] = backwards ? (s16) 0x8000 : 0;
                mario_set_forward_vel(&m, m.forwardVel);
                EXPECT_EQ(mario_air_twirl_bounce_off_wall(&m), FALSE);
                NEAR(fabsf(m.forwardVel), firstRebound);
                EXPECT_EQ(m.airTwirlWallBounceCount, 1);
                // Separation rearms a later impact within the same twirl.
                m.airTwirlWallContact = FALSE;
                EXPECT_EQ(mario_air_twirl_bounce_off_wall(&m), TRUE);
                NEAR(fabsf(m.forwardVel), (firstRebound + 8.0f) * 1.3f * 0.94f);
                EXPECT_EQ(m.airTwirlWallBounceCount, 2);
            }
        }
    }
    for (int flying = 0; flying < 2; flying++) {
        struct MarioState m = air_twirl_start();
        m.action = flying ? ACT_FLYING : ACT_FREEFALL;
        m.flags = flying ? MARIO_WING_CAP : 0;
        mario_try_air_twirl(&m);
        m.wall = &wallB;
        m.wallYaw = (s16) 0x8000;
        wallB.normal.x = 0;
        wallB.normal.z = -1;
        m.faceAngle[0] = 0;
        for (int bounce = 1; bounce <= 19; bounce++) {
            m.airTwirlWallContact = FALSE;
            m.faceAngle[1] = 0;
            mario_set_forward_vel(&m, 40.0f);
            EXPECT_EQ(mario_air_twirl_bounce_off_wall(&m), TRUE);
            f32 retained = bounce >= 18 ? 0.0f : 1.0f - (bounce - 1) * 0.06f;
            f32 rebound = MIN((40.0f + (bounce == 1 ? 9.0f : 8.0f)) * 1.3f * retained, 64.0f);
            NEAR(fabsf(m.forwardVel), rebound);
            NEAR(fabsf(m.vel[2]), rebound);
            EXPECT_EQ(m.airTwirlWallBounceCount, bounce >= 18 ? 18 : bounce);
        }
        mario_start_air_twirl(&m, FALSE);
        EXPECT_EQ(m.airTwirlWallBounceCount, 0);
        m.airTwirlWallBounceCount = 5;
        mario_air_twirl_init(&m);
        EXPECT_EQ(m.airTwirlWallBounceCount, 0);
    }
    return TRUE;
}

static int air_twirl_wall_input(void) {
    for (int timer = 0; timer <= WALL_KICK_GRACE_FRAMES; timer++) {
        struct MarioState m = air_twirl_start();
        m.action = ACT_AIR_HIT_WALL;
        m.actionTimer = timer;
        controller.buttonPressed = 0; // Held L, even without a new press.
        m.input = INPUT_A_PRESSED | INPUT_A_DOWN | INPUT_Z_DOWN;
        controller.buttonDown |= Z_TRIG;
        EXPECT_EQ(mario_try_air_twirl(&m), timer < WALL_KICK_GRACE_FRAMES);
        EXPECT_EQ(m.action, timer < WALL_KICK_GRACE_FRAMES ? ACT_L_AIR_TWIRL : ACT_AIR_HIT_WALL);
        NEAR(m.vel[1], timer < WALL_KICK_GRACE_FRAMES ? -3 : -10); // No wall-kick launch.
    }
    struct MarioState m = air_twirl_start();
    m.wall = &wallB;
    m.wallKickInputFrames = 3;
    m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
    m.airTwirlCooldown = 0;
    begin_wall_kick_window(&m); // The actual collision entry chooses L before fresh/buffered A.
    EXPECT_EQ(m.action, ACT_L_AIR_TWIRL);
    EXPECT_EQ(poseBegins, 0);
    EXPECT_EQ(m.wallKickInputFrames, 0);
    NEAR(m.vel[1], -3);
    m = air_twirl_start();
    m.wall = &wallB;
    m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
    m.airTwirlCooldown = 5;
    begin_wall_kick_window(&m);
    EXPECT_EQ(m.action, ACT_WALL_KICK_AIR); // Cooldown does not block ordinary A.
    EXPECT_EQ(m.airTwirlCooldown, 5);       // Collision entry must not age the clock twice.
    const u32 bonks[] = { ACT_SOFT_BONK, ACT_BACKWARD_AIR_KB, ACT_FORWARD_AIR_KB };
    for (unsigned i = 0; i < sizeof(bonks) / sizeof(bonks[0]); i++) {
        m = air_twirl_start();
        m.action = bonks[i];
        m.prevAction = ACT_AIR_HIT_WALL;
        m.wallKickTimer = 1;
        controller.buttonPressed = 0;
        EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
        EXPECT_EQ(m.action, ACT_L_AIR_TWIRL);
        EXPECT_EQ(m.wallKickTimer, 0);
    }
    // Wing entry must survive expired windows and compete with fresh/buffered A.
    const u32 wallStates[] = { ACT_AIR_HIT_WALL, ACT_SOFT_BONK, ACT_BACKWARD_AIR_KB,
                               ACT_FORWARD_AIR_KB };
    for (unsigned state = 0; state < sizeof(wallStates) / sizeof(*wallStates); state++) {
        for (int age = 0; age <= WALL_KICK_GRACE_FRAMES + 1; age++) {
            m = air_twirl_start();
            m.flags = MARIO_WING_CAP;
            m.action = wallStates[state];
            m.prevAction = ACT_AIR_HIT_WALL;
            m.actionTimer = age;
            m.wallKickTimer = 0;
            m.wallKickInputFrames = 2;
            m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
            EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
            EXPECT_EQ(m.action, ACT_L_AIR_TWIRL);
            EXPECT_EQ(m.wallKickInputFrames, 0);
            NEAR(m.vel[1], 20);
        }
    }
    m = air_twirl_start();
    m.flags = MARIO_WING_CAP;
    m.action = ACT_AIR_HIT_WALL;
    m.actionTimer = WALL_KICK_GRACE_FRAMES;
    m.wallKickContactFrame = gGlobalTimer - 1;
    m.input = INPUT_A_PRESSED;
    EXPECT_EQ(act_air_hit_wall(&m), TRUE);
    EXPECT_EQ(m.action, ACT_L_AIR_TWIRL);
    NEAR(m.vel[1], 20);
    m = air_twirl_start();
    m.flags = MARIO_WING_CAP;
    m.action = ACT_SOFT_BONK;
    m.prevAction = ACT_AIR_HIT_WALL;
    m.wallKickTimer = 1;
    m.input = INPUT_A_PRESSED;
    EXPECT_EQ(check_wall_kick(&m), TRUE);
    EXPECT_EQ(m.action, ACT_L_AIR_TWIRL);
    NEAR(m.vel[1], 20);
    // Fresh Wing input outside a kick window still respects cooldown and damage origins.
    m = air_twirl_start();
    m.flags = MARIO_WING_CAP;
    m.action = ACT_AIR_HIT_WALL;
    m.actionTimer = WALL_KICK_GRACE_FRAMES;
    m.airTwirlCooldown = 5;
    EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
    m = air_twirl_start();
    m.flags = MARIO_WING_CAP;
    m.action = ACT_BACKWARD_AIR_KB;
    m.prevAction = ACT_FREEFALL;
    EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
    return TRUE;
}

static int air_twirl_flying_roll(void) {
    for (int held = 0; held < 2; held++) {
        struct MarioState m = air_twirl_start();
        m.action = ACT_FLYING;
        m.actionState = ACT_STATE_FLYING_FLY;
        m.flags = MARIO_WING_CAP;
        m.forwardVel = 48;
        m.faceAngle[0] = -0x800;
        controller.stickX = 20;
        controller.stickY = -15;
        set_vel_from_pitch_and_yaw(&m);
        struct MarioState baseline = m;
        struct Object baselineObject = *m.marioObj;
        baseline.marioObj = &baselineObject;
        EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
        EXPECT_EQ(m.action, ACT_FLYING);
        EXPECT_EQ(m.airTwirlWingResetUsed, FALSE);
        NEAR(m.vel[1], baseline.vel[1]); // A descending flight roll keeps flight's momentum.
        controller.buttonPressed = 0;
        controller.buttonDown = held ? L_TRIG : 0;
        for (int frame = 0; frame < 16; frame++) {
            mario_try_air_twirl(&m);
            act_flying(&baseline);
            act_flying(&m);
            EXPECT_EQ(m.action, ACT_FLYING);
            EXPECT_EQ(m.airTwirlFlightRollFrame != 0, frame < 15);
            if (frame == 7) {
                EXPECT_EQ((s16) (m.marioObj->header.gfx.angle[2] - m.faceAngle[2]), -0x8000);
            }
            for (int axis = 0; axis < 3; axis++) {
                NEAR(m.pos[axis], baseline.pos[axis]);
                NEAR(m.vel[axis], baseline.vel[axis]);
                EXPECT_EQ(m.faceAngle[axis], baseline.faceAngle[axis]);
            }
            NEAR(m.forwardVel, baseline.forwardVel);
            EXPECT_EQ(m.marioObj->header.gfx.angle[2] == m.faceAngle[2], frame == 15);
        }
        EXPECT_EQ(m.airTwirlCooldown, 30);
        EXPECT_EQ(sCamera.mode, FLYING_CAMERA_MODE);
        act_flying(&m);
        EXPECT_EQ(m.marioObj->header.gfx.angle[2], m.faceAngle[2]);
    }
    struct MarioState m = air_twirl_start();
    m.action = ACT_FLYING;
    m.flags = MARIO_WING_CAP;
    m.actionState = ACT_STATE_FLYING_SPIN;
    m.actionArg = ACT_ARG_FLYING_TOTWC;
    mario_try_air_twirl(&m);
    act_flying(&m);
    EXPECT_EQ(m.actionState, ACT_STATE_FLYING_FLY);
    EXPECT_EQ(sFlightTextCalls, 1);
    m.input = INPUT_Z_PRESSED;
    act_flying(&m);
    EXPECT_EQ(m.action, ACT_FREEFALL);
    EXPECT_EQ(m.input & INPUT_Z_PRESSED, 0);
    EXPECT_EQ(m.airTwirlFlightRollFrame, 0);
    EXPECT_EQ(m.airTwirlCooldown, 30);
    m = air_twirl_start();
    m.action = ACT_FLYING;
    m.flags = MARIO_WING_CAP;
    mario_try_air_twirl(&m);
    m.flags = 0;
    EXPECT_EQ(mario_air_twirl_is_active(&m), FALSE);
    act_flying(&m);
    EXPECT_EQ(m.action, ACT_FREEFALL);
    EXPECT_EQ(m.airTwirlFlightRollFrame, 0);
    return TRUE;
}

static int cannon_boundary_recovery(void) {
    for (int wing = FALSE; wing <= TRUE; wing++) {
        for (int direction = -1; direction <= 1; direction += 2) {
            struct MarioState m = fresh();
            struct PlayerCameraState cameraState = { 0 };
            m.statusForCamera = &cameraState;
            mode = 14;
            topA.originOffset = 10000.0f; // No floor beyond z = 1.
            m.action = ACT_SHOT_FROM_CANNON;
            m.flags = wing ? MARIO_WING_CAP : 0;
            m.input = 0;
            m.pos[1] = 1000.0f;
            m.faceAngle[1] = direction > 0 ? 0 : (s16) 0x8000;
            // Start inside the blocked footprint, moving further outside it.
            m.pos[2] = direction > 0 ? 0.0f : 1000.0f;
            mario_set_forward_vel(&m, 100.0f);
            m.vel[1] = -10.0f;
            // The negative direction uses real wall contact rather than the map edge.
            if (direction < 0) {
                mode = 1;
                m.pos[0] = 0.0f;
                m.faceAngle[1] = 0x4000;
                mario_set_forward_vel(&m, 100.0f);
            }
            act_shot_from_cannon(&m);
            EXPECT_EQ(m.action, ACT_BACKWARD_AIR_KB);
            EXPECT_EQ(m.wallBonkMomentum, TRUE);
            NEAR(m.forwardVel, -50.0f);
            for (int frame = 0; frame < 4; frame++) {
                mode = direction > 0 ? 14 : 0;
                common_air_knockback_step(&m, ACT_BACKWARD_GROUND_KB, ACT_HARD_BACKWARD_GROUND_KB,
                                          MARIO_ANIM_BACKWARD_AIR_KB, -16.0f);
                EXPECT_EQ(m.action, ACT_BACKWARD_AIR_KB);
                EXPECT_EQ(m.wallBonkMomentum, TRUE);
                NEAR(m.forwardVel, -50.0f);
            }
        }
    }
    struct MarioState m = fresh();
    struct PlayerCameraState cameraState = { 0 };
    m.statusForCamera = &cameraState;
    mode = 0;
    m.action = ACT_SHOT_FROM_CANNON;
    m.flags = MARIO_WING_CAP;
    m.pos[1] = 1000.0f;
    m.vel[1] = -10.0f;
    mario_set_forward_vel(&m, 100.0f);
    act_shot_from_cannon(&m);
    EXPECT_EQ(m.action, ACT_FLYING);
    return TRUE;
}

static int air_twirl_flying_contacts(void) {
    // Landing during and after the roll needs held L; release uses flight's ordinary slide.
    for (int expired = 0; expired < 2; expired++) {
        for (int held = 0; held < 2; held++) {
            struct MarioState m = air_twirl_start();
            m.action = ACT_FLYING;
            m.flags = MARIO_WING_CAP;
            m.actionState = ACT_STATE_FLYING_FLY;
            m.faceAngle[0] = -0x800;
            mario_try_air_twirl(&m);
            if (expired) {
                controller.buttonPressed = 0;
                for (int frame = 0; frame < 16; frame++) {
                    mario_try_air_twirl(&m);
                    act_flying(&m);
                }
                EXPECT_EQ(m.airTwirlFlightRollFrame, 0);
            }
            m.pos[1] = ground.originOffset + 1;
            if (!expired && held) {
                controller.buttonDown = 0;
                mario_try_air_twirl(&m);
                EXPECT_EQ(m.airTwirlContinue, FALSE);
            }
            controller.buttonPressed = 0;
            controller.buttonDown = held ? L_TRIG : 0;
            mario_try_air_twirl(&m);
            act_flying(&m);
            EXPECT_EQ(m.action, held ? ACT_GROUND_TWIRL : ACT_DIVE_SLIDE);
            EXPECT_EQ(m.airTwirlFlightRollFrame, 0);
            EXPECT_EQ(sCamera.mode, sCamera.defMode);

            NEAR(m.pos[1], ground.originOffset);
        }
    }
    struct MarioState m = air_twirl_start();
    m.action = ACT_FLYING;
    m.flags = MARIO_WING_CAP;
    m.actionState = ACT_STATE_FLYING_FLY;
    m.faceAngle[0] = -0x800;
    mario_try_air_twirl(&m);
    controller.buttonPressed = 0;
    for (int frame = 0; frame < 5; frame++) {
        act_flying(&m);
    }
    mode = 4;
    act_flying(&m);
    EXPECT_EQ(m.action, ACT_FLYING);
    EXPECT_EQ(m.airTwirlFlightRollFrame, 2); // Impact restarts the complete roll.
    EXPECT_EQ(m.vel[2] < 0, TRUE);
    EXPECT_EQ(m.vel[1] < 0, TRUE);
    m.faceAngle[1] = 0;
    act_flying(&m);
    EXPECT_EQ(m.airTwirlFlightRollFrame, 3); // Persistent overlap cannot restart it.
    mode = 0;
    act_flying(&m);
    EXPECT_EQ(m.airTwirlWallContact, FALSE);
    mode = 4;
    m.faceAngle[1] = 0;
    wallB.type = SURFACE_BURNING;
    act_flying(&m);
    EXPECT_EQ(m.action, ACT_LAVA_BOOST);
    EXPECT_EQ(m.airTwirlFlightRollFrame, 0);
    return TRUE;
}

static int twirl_option_entry(void) {
    const u32 actions[] = { ACT_JUMP, ACT_FREEFALL, ACT_AIR_HIT_WALL, ACT_FLYING };
    sTestGameOptions[GO_TWIRLS] = FALSE;
    for (unsigned a = 0; a < sizeof(actions) / sizeof(*actions); a++) {
        struct MarioState m = air_twirl_start();
        m.action = actions[a];
        m.flags |= MARIO_WING_CAP;
        m.airTwirlInputFrames = 6;
        m.airTwirlJumpQueued = m.airTwirlLPressed = TRUE;
        u32 action = m.action;
        f32 speed = m.forwardVel;
        EXPECT_EQ(mario_start_air_twirl(&m, TRUE), FALSE);
        EXPECT_EQ(mario_try_wall_air_twirl(&m), FALSE);
        EXPECT_EQ(mario_try_air_twirl(&m), FALSE);
        EXPECT_EQ(m.action, action);
        NEAR(m.forwardVel, speed);
        EXPECT_EQ(m.airTwirlInputFrames | m.airTwirlJumpQueued | m.airTwirlLPressed, 0);
        m.airTwirlGravity = TRUE;
        m.airTwirlWingControlTimer = 30;
        m.action = ACT_L_AIR_TWIRL;
        m.vel[1] = -30;
        EXPECT_EQ(mario_air_twirl_is_active(&m), FALSE);
        NEAR(mario_air_twirl_gravity_scale(&m), 1);
        NEAR(mario_air_twirl_control_scale(&m), 1);
    }
    sTestGameOptions[GO_TWIRLS] = TRUE;
    struct MarioState m = air_twirl_start();
    EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
    return TRUE;
}

static int wing_preset(void) {
    for (s32 original = 0; original < 2; original++) {
        sTestGameOptions[GO_WING_CAP] = original;
        for (s32 otherCaps = 0; otherCaps < 4; otherCaps++) {
            struct MarioState m = fresh();
            m.flags = MARIO_WING_CAP | (otherCaps & 1 ? MARIO_METAL_CAP : 0)
                      | (otherCaps & 2 ? MARIO_VANISH_CAP : 0);
            m.action = ACT_WALKING;
            m.forwardVel = 40.0f;
            m.vel[1] = 0.0f;
            m.pos[1] = 0.0f;
            EXPECT_EQ(mario_can_walk_on_water(&m, 0.0f, 10000.0f), !original);

            m.action = ACT_FLYING;
            m.forwardVel = 32.0f;
            vec3_zero(m.faceAngle);
            vec3_zero(m.angleVel);
            update_flying(&m);
            NEAR(m.forwardVel, original ? 31.9f : 33.0f);
            EXPECT_EQ(m.faceAngle[0], 0);
            m.forwardVel = 4.0f;
            vec3_zero(m.faceAngle);
            vec3_zero(m.angleVel);
            update_flying(&m);
            NEAR(m.forwardVel, original ? 3.9f : 5.0f);
            if (original) {
                EXPECT_EQ(m.faceAngle[0], -0x400);
            }

            // Original keeps the separately enabled L roll.
            m.input = 0;
            m.action = ACT_FLYING;
            m.forwardVel = 32.0f;
            controller.buttonPressed = L_TRIG;
            EXPECT_EQ(mario_try_air_twirl(&m), TRUE);
            EXPECT_EQ(m.airTwirlFlightRollFrame, 1);

            m.input = INPUT_Z_PRESSED;
            sCamera.mode = FLYING_CAMERA_MODE;
            EXPECT_EQ(act_flying(&m), TRUE);
            EXPECT_EQ(m.action, original ? ACT_GROUND_POUND : ACT_FREEFALL);
            EXPECT_EQ(m.actionArg, original ? 1 : 0);
            EXPECT_EQ(m.airTwirlFlightRollFrame, 0);
            EXPECT_EQ(sCamera.mode, sCamera.defMode);
            m.action = ACT_FLYING_TRIPLE_JUMP;
            m.input = INPUT_Z_PRESSED;
            sCamera.mode = FLYING_CAMERA_MODE;
            EXPECT_EQ(act_flying_triple_jump(&m), TRUE);
            EXPECT_EQ(m.action, original ? ACT_GROUND_POUND : ACT_FREEFALL);
            EXPECT_EQ(m.actionArg, 0);
            EXPECT_EQ(sCamera.mode, sCamera.defMode);
            if (original) {
                m.action = ACT_FLYING_TRIPLE_JUMP;
                m.input = INPUT_B_PRESSED | INPUT_Z_PRESSED;
                EXPECT_EQ(act_flying_triple_jump(&m), TRUE);
                EXPECT_EQ(m.action, ACT_DIVE);
                // The shared input stage preserves simultaneous Z+B in this
                // field while removing INPUT_B_PRESSED from the action input.
                m.action = ACT_FLYING_TRIPLE_JUMP;
                m.input = INPUT_Z_PRESSED | INPUT_Z_DOWN;
                m.zBPressed = TRUE;
                EXPECT_EQ(act_flying_triple_jump(&m), TRUE);
                EXPECT_EQ(m.action, ACT_DIVE);
                m.zBPressed = FALSE;
            }
        }
    }
    sTestGameOptions[GO_WING_CAP] = 0;
    return TRUE;
}

static int unlimited_speed(void) {
    const u32 actions[] = { ACT_FREEFALL,   ACT_LONG_JUMP, ACT_SHOT_FROM_CANNON,
                            ACT_LAVA_BOOST, ACT_TWIRLING,  ACT_METAL_WATER_FALLING };
    for (int master = 0; master < 2; master++) {
        for (int enabled = 0; enabled < 2; enabled++) {
            sTestGameOptions[GO_CHEATS_ENABLED] = master;
            sTestGameOptions[GO_NO_SPEED_LIMIT] = enabled;
            for (unsigned i = 0; i < sizeof(actions) / sizeof(*actions); i++) {
                struct MarioState m = fresh();
                m.action = actions[i];
                m.input = 0;
                m.vel[1] = -200.0f;
                m.angleVel[1] = 2048;
                apply_gravity(&m);
                if (master && enabled) {
                    EXPECT_EQ(m.vel[1] < -200.0f, TRUE);
                } else {
                    EXPECT_EQ(m.vel[1] >= -MARIO_MAX_FALL_SPEED, TRUE);
                }
            }
            struct MarioState twirl = air_twirl_start();
            twirl.action = ACT_FREEFALL;
            mario_try_air_twirl(&twirl);
            twirl.wall = &wallB;
            twirl.wallYaw = (s16) 0x8000;
            twirl.faceAngle[1] = 0;
            mario_set_forward_vel(&twirl, 300.0f);
            EXPECT_EQ(mario_air_twirl_bounce_off_wall(&twirl), TRUE);
            if (master && enabled) {
                EXPECT_EQ(absf(twirl.forwardVel) > 300.0f, TRUE);
            } else {
                NEAR(absf(twirl.forwardVel), 64.0f);
            }
            for (int original = 0; original < 2; original++) {
                struct MarioState flying = fresh();
                sTestGameOptions[GO_WING_CAP] = original;
                flying.action = ACT_FLYING;
                flying.forwardVel = MARIO_SPEED_SAFETY_CAP;
                flying.controller->stickX = flying.controller->stickY = -64.0f;
                update_flying(&flying);
                EXPECT_EQ(flying.faceAngle[0] <= DEGREES(60), TRUE);
                EXPECT_EQ(flying.faceAngle[0] >= -DEGREES(60), TRUE);
                EXPECT_EQ(flying.angleVel[0] > 0, TRUE);
                EXPECT_EQ(flying.angleVel[1] > 0, TRUE);
            }
            sTestGameOptions[GO_WING_CAP] = 0;
            struct MarioState death = fresh();
            death.action = ACT_FALLING_DEATH_EXIT;
            death.vel[1] = -200.0f;
            apply_gravity(&death);
            NEAR(death.vel[1], -75.0f);
            struct MarioState star = fresh();
            star.action = ACT_FALL_AFTER_STAR_GRAB;
            star.vel[1] = -200.0f;
            apply_gravity(&star);
            NEAR(star.vel[1], -65.0f);
            struct MarioState groundStep = fresh();
            groundStep.vel[0] = 640.0f;
            groundStep.vel[1] = 0.0f;
            groundStep.vel[2] = 0.0f;
            groundStep.floor->normal.y = 1.0f;
            groundStep.floor->normal.x = groundStep.floor->normal.z = 0.0f;
            s32 steps = get_mario_ground_remaining_steps(&groundStep, 1.0f, 0, 4);
            EXPECT_EQ(steps, master && enabled ? 40 : 4);
            if (master && enabled) {
                EXPECT_EQ(get_mario_ground_air_collision_steps(640, 640, 640, 4,
                                                               MARIO_UNLIMITED_COLLISION_MAX_STEPS)
                                  * 16
                              >= sqrtf(3.0f * 640 * 640),
                          TRUE);
            }
        }
    }
    sTestGameOptions[GO_CHEATS_ENABLED] = sTestGameOptions[GO_NO_SPEED_LIMIT] = 0;
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    struct {
        const char *name;
        int (*run)(void);
    } cases[] = { { "geometry.unlimited_speed", unlimited_speed },
                  { "geometry.wing_preset", wing_preset },
                  { "geometry.twirl_option_entry", twirl_option_entry },
                  { "geometry.air_twirl_cycle", air_twirl_cycle },
                  { "geometry.jump_kick_twirl_cancel", jump_kick_twirl_cancel },
                  { "geometry.air_twirl_enemy_bounce_interrupt", air_twirl_enemy_bounce_interrupt },
                  { "geometry.air_twirl_coin_reset", air_twirl_coin_reset },
                  { "geometry.air_twirl_wall_input", air_twirl_wall_input },
                  { "geometry.air_twirl_wall_speed", air_twirl_wall_speed },
                  { "geometry.twirl_wall_all_directions", twirl_wall_all_directions },
                  { "geometry.air_twirl_flying_roll", air_twirl_flying_roll },
                  { "geometry.air_twirl_flying_contacts", air_twirl_flying_contacts },
                  { "geometry.air_twirl_gravity", air_twirl_gravity },
                  { "geometry.air_twirl_fall_damage", air_twirl_fall_damage },
                  { "geometry.wing_air_twirl_control", wing_air_twirl_control },
                  { "geometry.wing_air_twirl_entry_wall_boost", wing_air_twirl_entry_wall_boost },
                  { "geometry.wing_air_twirl_upward_boost", wing_air_twirl_upward_boost },
                  { "geometry.wing_air_twirl_momentum", wing_air_twirl_momentum },
                  { "geometry.wing_air_twirl_entry_direction", wing_air_twirl_entry_direction },
                  { "geometry.air_twirl_wall_and_landing", air_twirl_wall_and_landing },
                  { "geometry.hanging_grab_momentum", hanging_grab_momentum },
                  { "geometry.hanging_grab_boundaries", hanging_grab_boundaries },
                  { "geometry.swept_air_floor_landing", swept_air_floor_landing },
                  { "geometry.fence_edge_grinding", fence_edge_grinding },
                  { "geometry.air_rebound_fraction", air_rebound_fraction },
                  { "geometry.slide_kick_bounce_travel", slide_kick_bounce_travel },
                  { "geometry.slide_kick_uphill_bounce", slide_kick_uphill_bounce },
                  { "geometry.slide_kick_bounce_obstructions", slide_kick_bounce_obstructions },
                  { "geometry.slide_kick_startup_wall_grace", slide_kick_startup_wall_grace },
                  { "geometry.slide_kick_fall_exit", slide_kick_fall_exit },
                  { "geometry.thicc_contact_shakes", thicc_contact_shakes },
                  { "geometry.wing_water_walking", wing_water_walking },
                  { "geometry.water_walk_reentry", water_walk_reentry },
                  { "geometry.water_twirl_support", water_twirl_support },
                  { "geometry.wing_water_fall_damage_prohibition", wing_water_fall_damage_prohibition },
                  { "geometry.water_walk_cooldown_and_shallow_reentry",
                    water_walk_cooldown_and_shallow_reentry },
                  { "geometry.metal_water_movement", metal_water_movement },
                  { "geometry.metal_water_gravity", metal_water_gravity },
                  { "geometry.metal_water_air_and_boundaries", metal_water_air_and_boundaries },
                  { "geometry.backward_jump_kick_slope_entry", backward_jump_kick_slope_entry },
                  { "geometry.rock_downhill", rock_downhill },
                  { "geometry.rock_seams_and_exit", rock_seams_and_exit },
                  { "geometry.steep_slide_protection", steep_slide_protection },
                  { "geometry.backward_butt_slide_steep_jump", backward_butt_slide_steep_jump },
                  { "geometry.low_statue_step", low_statue_step },
                  { "geometry.crouch_slide_edge_step", crouch_slide_edge_step },
                  { "geometry.crouch_slide_edge_actions", crouch_slide_edge_actions },
                  { "geometry.grounded_ledge_anchors", grounded_ledge_anchors },
                  { "geometry.crawl_platform_side", crawl_platform_side },
                  { "geometry.ledge_contacts", ledge_contacts },
                  { "geometry.forward_wall_kick_speed_gate", forward_wall_kick_speed_gate },
                  { "geometry.buffered_wall_kick_ledge_priority", buffered_wall_kick_ledge_priority },
                  { "geometry.wall_slide_ledge_voice", wall_slide_ledge_voice },
                  { "geometry.wall_window", wall_window },
                  { "geometry.wall_expiry_dispatch", wall_expiry_dispatch },
                  { "geometry.wall_post_step_exits", wall_post_step_exits },
                  { "geometry.bonk_sound_replacement", bonk_sound_replacement },
                  { "geometry.wall_kick_sound_requires_new_kick", wall_kick_sound_requires_new_kick },
                  { "geometry.wall_twirl_sound_gap", wall_twirl_sound_gap },
                  { "geometry.missing_floor_jump_height", missing_floor_jump_height },
                  { "geometry.cannon_boundary_recovery", cannon_boundary_recovery },
                  { "geometry.jump_geometry_preservation", jump_geometry_preservation },
                  { "geometry.wall_slide_edge_exit", wall_slide_edge_exit },
                  { "geometry.wall_slide_stick_gate", wall_slide_stick_gate },
                  { "geometry.backward_air_wall_speed", backward_air_wall_speed },
                  { "geometry.backward_triple_wall_kick", backward_triple_wall_kick },
                  { "geometry.wind_catch", wind_catch },
                  { "geometry.wind_boundaries", wind_boundaries },
                  { "geometry.ignore_wind_death_boundary", ignore_wind_death_boundary },
                  { "geometry.wind_death_grace", wind_death_grace },
                  { "geometry.wind_grace_exit", wind_grace_exit },
                  { "geometry.underwater_shell_ascent", underwater_shell_ascent },
                  { "geometry.underwater_box_surface_contacts", underwater_box_surface_contacts } };
    for (unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        if (test_selected(cases[i].name)) {
            test_report(cases[i].name, cases[i].run());
        }
    }
    return test_finish();
}
