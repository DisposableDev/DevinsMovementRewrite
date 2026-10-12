static short gCollisionFlags;
/* Actual ground/air movement loops and ordinary landing handoff. Collision
 * substeps, action setup and damage outcomes are controlled doubles. No ROM,
 * triangle queries, animation playback, camera or performance measurements. */
#include <stdint.h>
#include <math.h>
#include "test_support.h"
typedef int32_t s32;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef int16_t s16;
typedef float f32;
typedef f32 Vec3f[3];
typedef s16 Vec3s[3];
#include "../include/sounds.h"
#define BIT(bit) (1u << (bit))
#define TRUE 1
#define FALSE 0
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, lo, hi) MIN(MAX(x, lo), hi)
#define sqr(x) ((x) * (x))
#define ABS(x) fabsf(x)
#include "landing_definitions.inc"
static int mario_character_palette(void) {
    return MARIO_PALETTE_STANDARD;
}

struct Animation {
    s16 flags;
};
struct AnimInfo {
    s16 animID, animFrame;
    s32 animAccel, animFrameAccelAssist;
    struct Animation *curAnim;
};
struct MarioStateForCamera {
    Vec3f pos;
};
struct Surface {
    struct {
        f32 x, y, z;
    } normal;
};
static struct Surface gWaterSurfacePseudoFloor;
struct Object {
    struct {
        struct {
            Vec3f pos, cameraToObject;
            struct AnimInfo animInfo;
            Vec3s angle;
        } gfx;
    } header;
    u32 oInteractStatus;
    f32 oForwardVel;
};
struct MarioState;
void play_sound_if_no_flag(struct MarioState *m, u32 sound, u32 flags);
void play_mario_landing_sound_once(struct MarioState *m, u32 sound);
void mario_set_wall_bonk_momentum(struct MarioState *m, f32 recoilSpeed, f32 minimumSpeed);
struct Controller {
    f32 stickMag;
};
static struct Controller sController;
struct MarioState {
    struct Controller *controller;
    u8 landingJumpBufferedEdge;
    u8 objectRecoveryAFrames, objectRecoveryBFrames;
    u8 automaticAFrames, automaticBFrames, automaticZFrames, groundPoundInputFrames;

    int wallBonkMomentum;
    Vec3f pos, vel;
    Vec3s faceAngle;
    f32 forwardVel, peakHeight, floorHeight, waterLevel, ceilHeight;
    u16 input;
    s16 intendedYaw;
    u8 hurtCounter, airTwirlUsed, airTwirlWingResetUsed;
    u8 metalFallGroundPoundImpact;
    u8 airTwirlWingControlTimer;
    s16 airTwirlCooldown;
    struct Surface *floor, *wall;
    struct Object *marioObj;
    struct Object *heldObj;
    struct MarioStateForCamera *statusForCamera;
    u32 action, actionArg, particleFlags, flags;
    s32 actionTimer, actionState, framesSinceA, wallKickChainCount, spawnFallProtection,
        terrainSoundAddend, windCatchActive;
    s32 landingJumpInputFrames, wallKickInputFrames;
};
static struct Surface sFlat = { { 0, 1, 0 } }, sSlope = { { -0.6f, 0.8f, 0 } }, sWall;
static struct MarioState sMario;
static struct Object sObject;
static int sAirChecks, sGroundChecks, sInputUpdates, sDamageChecks, sReaction, sAnimation;
static int sAirExit, sGravityUpdates, sTerrain, sWhoaCalls, sHrmmCalls, sUhCalls, sClimbEnded;
static int sThrowReleases;
static int sLandingSounds;
static u32 sLastLandingSound;
static f32 sLandingSoundX;
static f32 sWallX, sEdgeX, sSlopeX, sDamageX;

static void unsupported(const char *service) {
    fprintf(stderr, "HARNESS ERROR: landing test entered unmodeled %s\n", service);
    exit(2);
}
static void vec3f_copy(Vec3f dst, Vec3f src) {
    memcpy(dst, src, sizeof(Vec3f));
}
static void vec3f_set(Vec3f dst, f32 x, f32 y, f32 z) {
    dst[0] = x;
    dst[1] = y;
    dst[2] = z;
}
static void vec3s_set(Vec3s dst, s16 x, s16 y, s16 z) {
    dst[0] = x;
    dst[1] = y;
    dst[2] = z;
}
static void set_mario_floor(struct MarioState *m, struct Surface *floor, f32 height) {
    m->floor = floor;
    m->floorHeight = height;
}
static void mario_limit_velocity(struct MarioState *m) {
}
static int mario_wall_collision_begin(struct MarioState *m, int count, f32 lo, f32 hi, f32 dy,
                                      f32 padding) {
    return FALSE;
}
static void set_mario_wall(struct MarioState *m, struct Surface *wall) {
    m->wall = wall;
}
static int mario_get_terrain_sound_addend(struct MarioState *m) {
    return sTerrain;
}
static void apply_gravity(struct MarioState *m) {
    sGravityUpdates++;
    m->vel[1] -= 4.0f;
}
static void apply_vertical_wind(struct MarioState *m, f32 beforeGravity) {
}
static f32 find_water_level(f32 x, f32 z) {
    return sMario.waterLevel;
}
static f32 find_floor(f32 x, f32 y, f32 z, struct Surface **floor) {
    *floor = &sFlat;
    return 0.0f;
}
static int perform_air_quarter_step(struct MarioState *m, Vec3f next, u32 arg, int walls) {
    sAirChecks++;
    vec3f_copy(m->pos, next);
    if (sAirExit != AIR_STEP_NONE) {
        return sAirExit;
    }
    if (next[1] <= 0.0f) {
        m->pos[1] = 0.0f;
        m->floor = &sFlat;
        return AIR_STEP_LANDED;
    }
    return AIR_STEP_NONE;
}
static int perform_ground_quarter_step(struct MarioState *m, Vec3f next, int walls) {
    sGroundChecks++;
    if (next[0] >= sWallX) {
        m->pos[0] = sWallX;
        m->wall = &sWall;
        return GROUND_STEP_HIT_WALL_STOP_QSTEPS;
    }
    vec3f_copy(m->pos, next);
    if (next[0] >= sEdgeX) {
        return GROUND_STEP_LEFT_GROUND;
    }
    if (next[0] >= sSlopeX) {
        m->floor = &sSlope;
    }
    m->pos[1] = m->floor == &sSlope ? 0.75f * (m->pos[0] - sSlopeX) : 0.0f;
    return GROUND_STEP_NONE;
}
static void update_air_without_turn(struct MarioState *m) {
    sInputUpdates++;
}
static int check_fall_damage_or_get_stuck(struct MarioState *m, u32 action) {
    sDamageChecks++;
    sDamageX = m->pos[0];
    if (sReaction) {
        m->action = sReaction;
    }
    return sReaction != 0;
}
static int set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
    m->actionState = m->actionTimer = 0;
    m->landingJumpInputFrames = 0;
    // Match the production action setter's per-action sound reset.
    m->flags &= ~(MARIO_ACTION_SOUND_PLAYED | MARIO_MARIO_SOUND_PLAYED);
    return TRUE;
}
static void set_anim_to_frame(struct MarioState *m, s16 frame);
static struct Animation sClimbAnimation;
static struct MarioStateForCamera sCamera;
static void set_mario_animation(struct MarioState *m, int animation) {
    sAnimation = animation;
    struct AnimInfo *info = &m->marioObj->header.gfx.animInfo;
    if (info->animID != animation) {
        info->animID = animation;
        info->animFrame = -1;
        info->curAnim = &sClimbAnimation;
    }
}
// This fixture checks landing poses, not animation playback rates.
static void set_mario_anim_with_accel(struct MarioState *m, s32 animation, s32 accel) {
    (void) accel;
    set_mario_animation(m, animation);
}
static f32 sins(s16 angle) {
    return sinf(angle * 3.14159265f / 32768.0f);
}
static f32 coss(s16 angle) {
    return cosf(angle * 3.14159265f / 32768.0f);
}
static s32 check_common_action_exits(struct MarioState *m) {
    return set_mario_action(m, ACT_WALKING, 0);
}
static void play_mario_landing_sound(struct MarioState *m, u32 sound) {
    sLandingSounds++;
}
static void play_mario_jump_sound(struct MarioState *m) {
}
static s32 check_kick_or_dive_in_air(struct MarioState *m) {
    return FALSE;
}
static inline __attribute__((unused)) void queue_rumble_data(int a, int b) {
    unsupported("rumble");
}
static inline __attribute__((unused)) void mario_bonk_reflection(struct MarioState *m, int negate) {
    unsupported("bonk");
}
static int mario_has_wall_kick_slide_input(struct MarioState *m) {
    unsupported("wall-kick input");
    return FALSE;
}
static void begin_wall_kick_window(struct MarioState *m) {
    unsupported("wall-kick window");
}
static void mario_set_forward_vel(struct MarioState *m, f32 speed) {
    m->forwardVel = m->vel[0] = speed;
}
static void mario_throw_held_object(struct MarioState *m) {
    if (m->heldObj != NULL) {
        sThrowReleases++;
        m->heldObj = NULL;
    }
}
static int sLandingCancelRequested;
static s32 check_common_landing_cancels(struct MarioState *m, u32 action) {
    if (sLandingCancelRequested) {
        return set_mario_action(m, ACT_JUMP, 0);
    }
    return FALSE;
}
static u32 mario_update_moving_sand(struct MarioState *m) {
    return FALSE;
}
static u32 mario_update_windy_ground(struct MarioState *m) {
    return FALSE;
}
static void drop_and_set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    set_mario_action(m, action, arg);
}
static void lava_boost_on_wall(struct MarioState *m) {
    unsupported("lava wall");
}
static void mario_he_thicc_contact_shake(struct MarioState *m) {
}
static inline __attribute__((unused)) void mario_ground_pound_impact(struct MarioState *m) {
    unsupported("ground-pound impact");
}
static void play_sound(u32 sound, Vec3f position) {
    if ((sound >= SOUND_ACTION_TERRAIN_LANDING
         && sound <= SOUND_ACTION_TERRAIN_LANDING + (SOUND_TERRAIN_SAND << 16))
        || sound == SOUND_ACTION_METAL_LANDING) {
        sLandingSounds++;
        sLastLandingSound = sound;
        sLandingSoundX = sMario.pos[0];
    } else if (sound == SOUND_MARIO_WHOA) {
        sWhoaCalls++;
    } else if (sound == SOUND_MARIO_UH || sound == SOUND_MARIO_UH_LEDGE_CLIMB_FAST) {
        sUhCalls++;
    } else if (sound == SOUND_MARIO_HRMM) {
        sHrmmCalls++;
    } else if (sound != SOUND_MARIO_WAH2) {
        unsupported("voice sample");
    }
}
static f32 find_floor_height_relative_polar(struct MarioState *m, s16 yaw, f32 distance) {
    return m->floorHeight - 200.0f;
}
static s32 let_go_of_ledge(struct MarioState *m) {
    return set_mario_action(m, ACT_SOFT_BONK, 0);
}
static u32 mario_bowser_impact_knockback(struct MarioState *m, s32 inAir) {
    unsupported("Bowser impact; covered by the Metal suite");
    return FALSE;
}
static void stop_and_set_height_to_floor(struct MarioState *m) {
    m->pos[1] = m->floorHeight;
}
static s32 is_anim_at_end(struct MarioState *m) {
    return sClimbEnded;
}
static int sClimbCompletions;
static void climb_up_ledge(struct MarioState *m) {
    sClimbCompletions++;
}
static void castle_secret_glass_check_air(struct MarioState *m) {
    (void) m;
}

#include "landing_sources.inc"

#define EXPECT_NEAR(actual, expected)                                                                  \
    do {                                                                                               \
        f32 got_ = (actual), want_ = (expected);                                                       \
        if (!(fabsf(got_ - want_) <= 0.001f)) {                                                        \
            fprintf(stderr, "%s:%d: %s: %s: expected %.4f, observed %.4f\n", __FILE__, __LINE__,       \
                    sCurrentCase, #actual, want_, got_);                                               \
            return FALSE;                                                                              \
        }                                                                                              \
    } while (0)

static void begin(f32 speed, f32 down, f32 height) {
    memset(&sMario, 0, sizeof(sMario));
    memset(&sObject, 0, sizeof(sObject));
    sObject.header.gfx.animInfo.animID = -1;
    sController.stickMag = 0;
    sMario.controller = &sController;
    sMario.statusForCamera = &sCamera;
    sMario.marioObj = &sObject;
    sMario.floor = &sFlat;
    sMario.action = ACT_JUMP;
    sMario.pos[1] = height;
    sMario.vel[0] = sMario.forwardVel = speed;
    sMario.vel[1] = -down;
    sAirChecks = sGroundChecks = sInputUpdates = sDamageChecks = sGravityUpdates = 0;
    sReaction = sTerrain = sWhoaCalls = sHrmmCalls = sUhCalls = sClimbEnded = 0;
    sMario.ceilHeight = 1000;
    sAirExit = AIR_STEP_NONE;
    sAnimation = -1;
    sThrowReleases = 0;
    sLandingSounds = 0;
    sLastLandingSound = 0;
    sLandingSoundX = -1.0f;
    sWallX = sEdgeX = sSlopeX = INFINITY;
    sDamageX = -1;
}
static int land(u32 action) {
    return common_air_action_step(&sMario, action, MARIO_ANIM_SINGLE_JUMP, 0);
}
static int contact_sound(void) {
    const u32 actions[] = { ACT_JUMP_LAND,     ACT_DOUBLE_JUMP_LAND, ACT_TRIPLE_JUMP_LAND,
                            ACT_BACKFLIP_LAND, ACT_SIDE_FLIP_LAND,   ACT_LONG_JUMP_LAND,
                            ACT_FREEFALL_LAND, ACT_HOLD_JUMP_LAND,   ACT_HOLD_FREEFALL_LAND };
    for (unsigned action = 0; action < sizeof(actions) / sizeof(actions[0]); action++) {
        for (int terrain = SOUND_TERRAIN_DEFAULT; terrain <= SOUND_TERRAIN_SAND; terrain++) {
            for (int metal = 0; metal < 2; metal++) {
                begin(40, 40, 1);
                sTerrain = terrain << 16;
                sMario.flags = MARIO_ACTION_SOUND_PLAYED | (metal ? MARIO_METAL_CAP : 0);
                EXPECT_EQ(land(actions[action]), AIR_STEP_LANDED);
                EXPECT_EQ(sLandingSounds, 1);
                EXPECT_NEAR(sLandingSoundX, 10.0f);
                EXPECT_NEAR(sMario.pos[0], 40.0f);
                EXPECT_EQ(sLastLandingSound,
                          metal ? SOUND_ACTION_METAL_LANDING : SOUND_ACTION_TERRAIN_LANDING + sTerrain);
                EXPECT_EQ((sMario.flags & MARIO_ACTION_SOUND_PLAYED) != 0, TRUE);
                // The normal recovery's once helper cannot replay the contact.
                play_mario_landing_sound_once(&sMario, SOUND_ACTION_TERRAIN_LANDING);
                EXPECT_EQ(sLandingSounds, 1);
                // A recovery cancel keeps the already emitted sound.
                set_mario_action(&sMario, ACT_JUMP, 0);
                EXPECT_EQ(sLandingSounds, 1);
            }
        }
    }
    begin(40, 40, 1);
    sEdgeX = 15;
    EXPECT_EQ(land(ACT_JUMP_LAND), AIR_STEP_LANDED);
    EXPECT_EQ(sMario.action, ACT_FREEFALL);
    EXPECT_EQ(sLandingSounds, 1);
    EXPECT_NEAR(sLandingSoundX, 10.0f);
    begin(40, 40, 1);
    sReaction = ACT_HARD_BACKWARD_GROUND_KB;
    EXPECT_EQ(land(ACT_JUMP_LAND), AIR_STEP_LANDED);
    EXPECT_EQ(sLandingSounds, 0);
    begin(40, 40, 100);
    EXPECT_EQ(land(ACT_JUMP_LAND), AIR_STEP_NONE);
    EXPECT_EQ(sLandingSounds, 0);
    return TRUE;
}
static int jump_buffer_handoff(void) {
    for (int jump = 0; jump < 2; jump++) {
        for (int age = 0; age <= 6; age++) {
            begin(20, 20, 1);
            sMario.action = jump ? ACT_DOUBLE_JUMP : ACT_JUMP;
            sMario.actionState = 2;
            sMario.framesSinceA = age;
            EXPECT_EQ(land(jump ? ACT_DOUBLE_JUMP_LAND : ACT_JUMP_LAND), AIR_STEP_LANDED);
            EXPECT_EQ(sMario.actionArg, age < 6 ? 6 - age : 0);
        }
    }
    begin(20, 20, 1);
    sMario.action = ACT_JUMP;
    sMario.actionState = 1;
    sMario.framesSinceA = 0;
    EXPECT_EQ(land(ACT_JUMP_LAND), AIR_STEP_LANDED);
    EXPECT_EQ(sMario.actionArg, 0);
    begin(20, 20, 1);
    sMario.action = ACT_JUMP;
    sMario.actionState = 2;
    sMario.framesSinceA = 0;
    sReaction = ACT_HARD_BACKWARD_GROUND_KB;
    EXPECT_EQ(land(ACT_JUMP_LAND), AIR_STEP_LANDED);
    EXPECT_EQ(sMario.actionArg, 0);
    const u32 airActions[] = { ACT_TRIPLE_JUMP,   ACT_SIDE_FLIP,  ACT_LONG_JUMP,
                               ACT_WALL_KICK_AIR, ACT_FREEFALL,   ACT_HOLD_FREEFALL,
                               ACT_HOLD_JUMP,     ACT_STEEP_JUMP, ACT_WATER_JUMP };
    const u32 landActions[] = { ACT_TRIPLE_JUMP_LAND, ACT_SIDE_FLIP_LAND, ACT_LONG_JUMP_LAND,
                                ACT_JUMP_LAND,        ACT_FREEFALL_LAND,  ACT_HOLD_FREEFALL_LAND,
                                ACT_HOLD_JUMP_LAND,   ACT_JUMP_LAND,      ACT_JUMP_LAND };
    for (unsigned jump = 0; jump < sizeof(airActions) / sizeof(*airActions); jump++) {
        for (int remaining = 0; remaining <= 6; remaining++) {
            begin(20, 20, 1);
            sMario.action = airActions[jump];
            sMario.landingJumpInputFrames = remaining;
            EXPECT_EQ(land(landActions[jump]), AIR_STEP_LANDED);
            EXPECT_EQ(sMario.actionArg, remaining);
            EXPECT_EQ(sMario.landingJumpInputFrames, 0);
        }
    }
    return TRUE;
}
static int travel(void) {
    const f32 speeds[] = { 0, 5, 40, 100, 127, -40, -100 };
    const f32 descents[] = { 10, 40, 92 };
    const u32 actions[] = { ACT_JUMP_LAND,      ACT_DOUBLE_JUMP_LAND, ACT_TRIPLE_JUMP_LAND,
                            ACT_BACKFLIP_LAND,  ACT_SIDE_FLIP_LAND,   ACT_FREEFALL_LAND,
                            ACT_LONG_JUMP_LAND, ACT_HOLD_JUMP_LAND,   ACT_HOLD_FREEFALL_LAND };
    for (unsigned a = 0; a < sizeof(actions) / sizeof(actions[0]); a++) {
        for (unsigned v = 0; v < sizeof(speeds) / sizeof(speeds[0]); v++) {
            for (unsigned d = 0; d < sizeof(descents) / sizeof(descents[0]); d++) {
                for (int h = 1; h <= 10; h++) {
                    // Stay just below the endpoint: repeated float additions
                    // can leave an exact endpoint a few ulps above the plane.
                    begin(speeds[v], descents[d], descents[d] * h / 10.0f - 0.001f);
                    sMario.vel[2] = speeds[v] * 0.5f;
                    sTerrain = a & 1; // Same movement budget on dry/wet terrain.
                    EXPECT_EQ(land(actions[a]), AIR_STEP_LANDED);
                    EXPECT_NEAR(sMario.pos[0], speeds[v]);
                    EXPECT_NEAR(sMario.pos[2], speeds[v] * 0.5f);
                    EXPECT_NEAR(sObject.header.gfx.pos[0], sMario.pos[0]);
                    EXPECT_NEAR(sMario.forwardVel, speeds[v]);
                    EXPECT_EQ(sMario.action, actions[a]);
                    EXPECT_EQ(sMario.actionTimer, 0);
                    EXPECT_EQ(sMario.actionState, 0);
                    EXPECT_EQ(sInputUpdates, 1);
                    EXPECT_EQ(sGravityUpdates, 1);
                    EXPECT_EQ(sDamageChecks, 1);
                }
            }
        }
    }
    return TRUE;
}
static int repeated(void) {
    begin(40, 40, 1);
    for (int i = 1; i <= 12; i++) {
        sMario.pos[1] = 1;
        sMario.vel[1] = -40;
        sMario.action = ACT_JUMP;
        EXPECT_EQ(land(ACT_JUMP_LAND), AIR_STEP_LANDED);
        EXPECT_NEAR(sMario.pos[0], i * 40);
    }
    EXPECT_EQ(sInputUpdates, 12);
    EXPECT_EQ(sDamageChecks, 12);
    return TRUE;
}
static int wall(void) {
    begin(40, 40, 1);
    sWallX = 20;
    EXPECT_EQ(land(ACT_JUMP_LAND), AIR_STEP_LANDED);
    EXPECT_NEAR(sMario.pos[0], 20);
    EXPECT_EQ(sMario.wall == &sWall, TRUE);
    EXPECT_NEAR(sObject.header.gfx.pos[0], 20);
    return TRUE;
}
static int floor_change(void) {
    begin(40, 40, 1);
    sSlopeX = 15;
    EXPECT_EQ(land(ACT_JUMP_LAND), AIR_STEP_LANDED);
    EXPECT_EQ(sMario.floor == &sSlope, TRUE);
    EXPECT_NEAR(sMario.pos[0], 35.5f);
    EXPECT_NEAR(sMario.pos[1], 15.375f);
    EXPECT_NEAR(sObject.header.gfx.pos[1], sMario.pos[1]);
    return TRUE;
}
static int leave_floor(void) {
    const u32 landActions[] = { ACT_JUMP_LAND, ACT_HOLD_JUMP_LAND, ACT_HOLD_FREEFALL_LAND };
    for (unsigned i = 0; i < sizeof(landActions) / sizeof(landActions[0]); i++) {
        begin(40, 40, 1);
        sEdgeX = 20;
        EXPECT_EQ(land(landActions[i]), AIR_STEP_LANDED);
        EXPECT_EQ(sMario.action, i ? ACT_HOLD_FREEFALL : ACT_FREEFALL);
        EXPECT_EQ(sAnimation, i ? MARIO_ANIM_FALL_WITH_LIGHT_OBJ : MARIO_ANIM_GENERAL_FALL);
        EXPECT_NEAR(sMario.vel[1], 0);
        EXPECT_EQ(sGroundChecks, 2);
    }
    return TRUE;
}
static int reactions(void) {
    const u32 actions[] = { ACT_HARD_BACKWARD_GROUND_KB, ACT_FEET_STUCK_IN_GROUND };
    for (unsigned i = 0; i < sizeof(actions) / sizeof(actions[0]); i++) {
        begin(40, 40, 1);
        sReaction = actions[i];
        EXPECT_EQ(land(ACT_JUMP_LAND), AIR_STEP_LANDED);
        EXPECT_EQ(sMario.action, actions[i]);
        EXPECT_EQ(sGroundChecks, 0);
        EXPECT_NEAR(sDamageX, 10);
        EXPECT_NEAR(sMario.pos[0], 10);
    }
    begin(40, 40, 1);
    EXPECT_EQ(land(ACT_JUMP_LAND), AIR_STEP_LANDED);
    EXPECT_NEAR(sDamageX, 10);
    EXPECT_NEAR(sMario.pos[0], 40);
    return TRUE;
}
static int legacy_step(void) {
    begin(40, 40, 1);
    sMario.action = ACT_SLIDE_KICK;
    EXPECT_EQ(perform_air_step(&sMario, 0), AIR_STEP_LANDED);
    EXPECT_NEAR(sMario.pos[0], 10);
    EXPECT_EQ(sGroundChecks, 0);
    begin(40, 40, 0);
    EXPECT_EQ(perform_ground_step(&sMario), GROUND_STEP_NONE);
    EXPECT_NEAR(sMario.pos[0], 40);
    return TRUE;
}
static int no_landing(void) {
    begin(40, 40, 100);
    EXPECT_EQ(land(ACT_JUMP_LAND), AIR_STEP_NONE);
    EXPECT_NEAR(sMario.pos[0], 40);
    EXPECT_EQ(sGroundChecks, 0);
    EXPECT_EQ(sDamageChecks, 0);
    EXPECT_EQ(sAnimation, MARIO_ANIM_SINGLE_JUMP);
    begin(40, 40, 1);
    sAirExit = AIR_STEP_GRABBED_CEILING;
    f32 remaining = 123;
    EXPECT_EQ(perform_air_step_with_remainder(&sMario, 0, &remaining), AIR_STEP_GRABBED_CEILING);
    EXPECT_NEAR(remaining, 0);
    EXPECT_EQ(sGroundChecks, 0);
    return TRUE;
}
static int final_substep(void) {
    begin(40, 40, 40);
    EXPECT_EQ(land(ACT_JUMP_LAND), AIR_STEP_LANDED);
    EXPECT_EQ(sGroundChecks, 0);
    EXPECT_NEAR(sMario.pos[0], 40);
    EXPECT_EQ(perform_ground_step_with_fraction(&sMario, 0), GROUND_STEP_NONE);
    EXPECT_EQ(sGroundChecks, 0);
    return TRUE;
}
static int ledge_voice(void) {
    const f32 incoming[] = { -40.0f, -24.01f, -24.0f, -23.99f, -20.0f, -16.0f,
                             -8.0f,  -0.01f,  0.0f,   2.0f,    20.0f };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned i = 0; i < sizeof(incoming) / sizeof(incoming[0]); i++) {
            begin(20, 0, 200);
            sMario.vel[1] = incoming[i];
            sAirExit = AIR_STEP_GRABBED_LEDGE;
            EXPECT_EQ(land(ACT_JUMP_LAND), AIR_STEP_GRABBED_LEDGE);
            EXPECT_EQ(sMario.action, ACT_LEDGE_GRAB);
            EXPECT_EQ(sObject.header.gfx.animInfo.animFrame, 0); // First rendered catch pose is 1.
            EXPECT_EQ(sMario.actionArg, incoming[i] <= -24.0f ? 0 : ACT_ARG_LEDGE_GRAB_GENTLE);
            EXPECT_NEAR(sMario.vel[1], incoming[i] - 4.0f);
            for (int update = 0; update < 4; update++) {
                act_ledge_grab(&sMario);
                EXPECT_EQ(sWhoaCalls, incoming[i] <= -24.0f ? 1 : 0);
                EXPECT_EQ(sHrmmCalls, incoming[i] > -24.0f ? 1 : 0);
                EXPECT_EQ(sUhCalls, 0);
            }
        }
    }
    return TRUE;
}
static int ledge_climb_skip(void) {
    const u32 actions[] = { ACT_LEDGE_CLIMB_SLOW_1, ACT_LEDGE_CLIMB_FAST };
    const s32 animations[] = { MARIO_ANIM_SLOW_LEDGE_GRAB, MARIO_ANIM_FAST_LEDGE_GRAB };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned i = 0; i < 2; i++) {
            begin(0, 0, 0);
            set_mario_action(&sMario, actions[i], 0);
            update_ledge_climb(&sMario, animations[i], ACT_IDLE);
            EXPECT_EQ(sObject.header.gfx.animInfo.animFrame, 2); // Next rendered pose is 3.
            sObject.header.gfx.animInfo.animFrame = 9;
            update_ledge_climb(&sMario, animations[i], ACT_IDLE);
            EXPECT_EQ(sObject.header.gfx.animInfo.animFrame, 9); // Never seek twice.
            sClimbEnded = TRUE;
            update_ledge_climb(&sMario, animations[i], ACT_IDLE);
            EXPECT_EQ(sMario.action, ACT_IDLE);
        }
    }
    begin(0, 0, 0);
    set_mario_action(&sMario, ACT_LEDGE_CLIMB_DOWN, 0);
    update_ledge_climb(&sMario, MARIO_ANIM_CLIMB_DOWN_LEDGE, ACT_LEDGE_GRAB);
    EXPECT_EQ(sObject.header.gfx.animInfo.animFrame, -1);
    begin(0, 0, 0);
    set_mario_action(&sMario, ACT_LEDGE_CLIMB_SLOW_1, 0);
    sMario.actionTimer = 23;
    sMario.input = INPUT_NONZERO_ANALOG;
    EXPECT_EQ(act_ledge_climb_slow(&sMario), FALSE);
    EXPECT_EQ(sMario.actionTimer, 24);
    EXPECT_EQ(act_ledge_climb_slow(&sMario), TRUE);
    EXPECT_EQ(sMario.action, ACT_WALKING);
    return TRUE;
}
static int ledge_climb_down_voice(void) {
    const int animationUpdates[] = { 1, 4 };
    for (unsigned length = 0; length < sizeof(animationUpdates) / sizeof(animationUpdates[0]);
         length++) {
        begin(0, 0, 0);
        sMario.flags |= MARIO_MARIO_SOUND_PLAYED;
        for (int cycle = 0; cycle < 3; cycle++) {
            set_mario_action(&sMario, ACT_LEDGE_CLIMB_DOWN, 0);
            for (int update = 0; update < animationUpdates[length]; update++) {
                sClimbEnded = update == animationUpdates[length] - 1;
                EXPECT_EQ(act_ledge_climb_down(&sMario), FALSE);
                EXPECT_EQ(sHrmmCalls, cycle + 1);
                EXPECT_EQ(sUhCalls, 0);
                EXPECT_EQ(sWhoaCalls, 0);
                EXPECT_EQ(sAnimation, MARIO_ANIM_CLIMB_DOWN_LEDGE);
                EXPECT_NEAR(sMario.pos[1], sMario.floorHeight);
            }
            EXPECT_EQ(sMario.action, ACT_LEDGE_GRAB);
            EXPECT_EQ(sMario.actionArg, ACT_ARG_LEDGE_GRAB_SILENT);
            EXPECT_EQ(sMario.flags & MARIO_MARIO_SOUND_PLAYED, 0);
            for (int update = 0; update < 20; update++) {
                EXPECT_EQ(act_ledge_grab(&sMario), FALSE);
                EXPECT_EQ(sMario.action, ACT_LEDGE_GRAB);
                EXPECT_EQ(sHrmmCalls, cycle + 1);
                EXPECT_EQ(sUhCalls, 0);
                EXPECT_EQ(sWhoaCalls, 0);
            }
            sMario.input = INPUT_Z_PRESSED;
            EXPECT_EQ(act_ledge_grab(&sMario), TRUE);
            EXPECT_EQ(sMario.action, ACT_SOFT_BONK);
            sMario.input = 0;
        }
    }

    // An invalid entry drops immediately without a grab cue.
    begin(0, 0, 0);
    set_mario_action(&sMario, ACT_LEDGE_CLIMB_DOWN, 0);
    sMario.input = INPUT_OFF_FLOOR;
    EXPECT_EQ(act_ledge_climb_down(&sMario), TRUE);
    EXPECT_EQ(sMario.action, ACT_SOFT_BONK);
    EXPECT_EQ(sHrmmCalls, 0);

    // Losing support during the animation must not replay its entry cue.
    sMario.input = 0;
    set_mario_action(&sMario, ACT_LEDGE_CLIMB_DOWN, 0);
    EXPECT_EQ(act_ledge_climb_down(&sMario), FALSE);
    EXPECT_EQ(sHrmmCalls, 1);
    sMario.input = INPUT_OFF_FLOOR;
    EXPECT_EQ(act_ledge_climb_down(&sMario), TRUE);
    EXPECT_EQ(sMario.action, ACT_SOFT_BONK);
    EXPECT_EQ(sHrmmCalls, 1);
    EXPECT_EQ(sUhCalls, 0);
    EXPECT_EQ(sWhoaCalls, 0);
    return TRUE;
}
static int ledge_release_controls(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(0, 0, 0);
        set_mario_action(&sMario, ACT_LEDGE_GRAB, 0);
        sMario.ceilHeight = 200.0f;
        sMario.input = INPUT_NONZERO_ANALOG;
        for (int update = 1; update < 8; update++) {
            EXPECT_EQ(act_ledge_grab(&sMario), FALSE);
            EXPECT_EQ(sMario.action, ACT_LEDGE_GRAB);
            EXPECT_EQ(sMario.actionTimer, update);
            if (update == 1) {
                EXPECT_EQ(sObject.header.gfx.animInfo.animFrame, 0); // Next pose is frame 1.
                sObject.header.gfx.animInfo.animFrame = 5;
            } else {
                EXPECT_EQ(sObject.header.gfx.animInfo.animFrame, 5); // Never seek twice.
            }
        }
        EXPECT_EQ(act_ledge_grab(&sMario), TRUE);
        EXPECT_EQ(sMario.action, ACT_LEDGE_CLIMB_SLOW_1);
    }

    // Airborne catches already seek the pose before the first hanging update.
    begin(0, 0, 0);
    set_mario_action(&sMario, ACT_LEDGE_GRAB, 0);
    set_mario_animation(&sMario, MARIO_ANIM_IDLE_ON_LEDGE);
    sObject.header.gfx.animInfo.animFrame = 5;
    sMario.ceilHeight = 200.0f;
    EXPECT_EQ(act_ledge_grab(&sMario), FALSE);
    EXPECT_EQ(sObject.header.gfx.animInfo.animFrame, 5);

    const s16 hangingYaw[] = { 0x4001, -0x4001, -0x8000 };
    const int timers[] = { 0, 9, 10 };

    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned i = 0; i < sizeof(hangingYaw) / sizeof(hangingYaw[0]); i++) {
            begin(0, 0, 0);
            sMario.action = ACT_LEDGE_GRAB;
            sMario.ceilHeight = 200.0f;
            sMario.input = INPUT_NONZERO_ANALOG;
            sMario.intendedYaw = hangingYaw[i];
            for (int update = 0; update < 20; update++) {
                EXPECT_EQ(act_ledge_grab(&sMario), FALSE);
                EXPECT_EQ(sMario.action, ACT_LEDGE_GRAB);
                EXPECT_EQ(sAnimation, MARIO_ANIM_IDLE_ON_LEDGE);
            }
            sMario.input = 0;
            EXPECT_EQ(act_ledge_grab(&sMario), FALSE);
            EXPECT_EQ(sMario.action, ACT_LEDGE_GRAB);
            sMario.input = INPUT_Z_PRESSED | INPUT_NONZERO_ANALOG;
            EXPECT_EQ(act_ledge_grab(&sMario), TRUE);
            EXPECT_EQ(sMario.action, ACT_SOFT_BONK);
        }

        for (unsigned i = 0; i < sizeof(timers) / sizeof(timers[0]); i++) {
            begin(0, 0, 0);
            sMario.action = ACT_LEDGE_GRAB;
            sMario.actionTimer = timers[i];
            sMario.ceilHeight = 200.0f;
            sMario.input = INPUT_Z_PRESSED | INPUT_A_PRESSED | INPUT_NONZERO_ANALOG;
            EXPECT_EQ(act_ledge_grab(&sMario), TRUE);
            EXPECT_EQ(sMario.action, timers[i] + 1 < 8 ? ACT_LEDGE_CLIMB_FAST : ACT_SOFT_BONK);
        }

        // Backward stick does not prevent an A climb, even during the initial hang.
        begin(0, 0, 0);
        sMario.action = ACT_LEDGE_GRAB;
        sMario.ceilHeight = 200.0f;
        sMario.intendedYaw = -0x8000;
        sMario.input = INPUT_A_PRESSED | INPUT_NONZERO_ANALOG;
        EXPECT_EQ(act_ledge_grab(&sMario), TRUE);
        EXPECT_EQ(sMario.action, ACT_LEDGE_CLIMB_FAST);

        begin(0, 0, 0);
        sMario.action = ACT_LEDGE_GRAB;
        sMario.ceilHeight = 200.0f;
        sMario.input = INPUT_NONZERO_ANALOG;
        sMario.actionTimer = 6;
        EXPECT_EQ(act_ledge_grab(&sMario), FALSE);
        EXPECT_EQ(sMario.action, ACT_LEDGE_GRAB);
        EXPECT_EQ(act_ledge_grab(&sMario), TRUE);
        EXPECT_EQ(sMario.action, ACT_LEDGE_CLIMB_SLOW_1);

        const u16 forcedRelease[] = { INPUT_OFF_FLOOR, INPUT_STOMPED };
        for (unsigned i = 0; i < sizeof(forcedRelease) / sizeof(forcedRelease[0]); i++) {
            begin(0, 0, 0);
            sMario.action = ACT_LEDGE_GRAB;
            sMario.input = forcedRelease[i];
            EXPECT_EQ(act_ledge_grab(&sMario), TRUE);
            EXPECT_EQ(sMario.action, ACT_SOFT_BONK);
        }
        begin(0, 0, 0);
        sMario.action = ACT_LEDGE_GRAB;
        sMario.floor = &sSlope;
        EXPECT_EQ(act_ledge_grab(&sMario), TRUE);
        EXPECT_EQ(sMario.action, ACT_SOFT_BONK);
    }
    for (int timer = 6; timer <= 8; timer++) {
        begin(0, 0, 0);
        sMario.action = ACT_LEDGE_GRAB;
        sMario.actionTimer = timer;
        sMario.ceilHeight = 200.0f;
        sMario.input = INPUT_Z_PRESSED;
        EXPECT_EQ(act_ledge_grab(&sMario), timer + 1 >= 8);
        EXPECT_EQ(sMario.action, timer + 1 >= 8 ? ACT_SOFT_BONK : ACT_LEDGE_GRAB);
    }
    return TRUE;
}

static int handstand_touchdown(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int state = HANDSTAND_FALL_ANIMATING; state <= HANDSTAND_FALL_NORMAL; state++) {
            begin(24, 20, 1);
            sMario.action = ACT_TOP_OF_POLE_JUMP;
            sMario.actionState = state;
            sAnimation = MARIO_ANIM_HANDSTAND_JUMP;
            EXPECT_EQ(act_top_of_pole_jump(&sMario), FALSE);
            EXPECT_EQ(sMario.action, ACT_FREEFALL_LAND);
            EXPECT_EQ(sAnimation, MARIO_ANIM_GENERAL_LAND);
            EXPECT_EQ(sMario.actionTimer, 0);
            EXPECT_EQ(sInputUpdates, 1);
            EXPECT_EQ(sDamageChecks, 1);
            EXPECT_EQ(sGravityUpdates, 1);
            EXPECT_NEAR(sMario.pos[0], 24);

            begin(24, 20, 1);
            sMario.action = ACT_TOP_OF_POLE_JUMP;
            sReaction = ACT_HARD_BACKWARD_GROUND_KB;
            sAnimation = MARIO_ANIM_HANDSTAND_JUMP;
            act_top_of_pole_jump(&sMario);
            EXPECT_EQ(sMario.action, sReaction);
            EXPECT_EQ(sAnimation, MARIO_ANIM_HANDSTAND_JUMP);

            begin(24, 20, 1);
            sMario.action = ACT_TOP_OF_POLE_JUMP;
            sEdgeX = 10;
            act_top_of_pole_jump(&sMario);
            EXPECT_EQ(sMario.action, ACT_FREEFALL);
            EXPECT_EQ(sAnimation, MARIO_ANIM_GENERAL_FALL);
        }
    }
    return TRUE;
}

static int air_throw_touchdown(void) {
    const f32 speeds[] = { -40.0f, -5.0f, 0.0f, 5.0f, 40.0f, 100.0f };
    const f32 heights[] = { 1.0f, 10.0f, 40.0f };
    const int clocks[] = { 0, 1, 2, 3, 8 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned v = 0; v < sizeof(speeds) / sizeof(speeds[0]); v++) {
            for (unsigned h = 0; h < sizeof(heights) / sizeof(heights[0]); h++) {
                for (unsigned t = 0; t < sizeof(clocks) / sizeof(clocks[0]); t++) {
                    begin(speeds[v], 40, heights[h]);
                    sMario.action = ACT_AIR_THROW;
                    sMario.actionTimer = clocks[t];
                    sMario.heldObj = clocks[t] < 4 ? &sObject : NULL;
                    EXPECT_EQ(act_air_throw(&sMario), FALSE);
                    EXPECT_EQ(sMario.action, ACT_AIR_THROW_LAND);
                    EXPECT_EQ(sMario.actionTimer, clocks[t] + 1);
                    EXPECT_NEAR(sMario.forwardVel, speeds[v]);
                    EXPECT_NEAR(sMario.pos[0], speeds[v]);
                    EXPECT_EQ(sGravityUpdates, 1);
                    EXPECT_EQ(sInputUpdates, 1);
                    EXPECT_EQ(sDamageChecks, 1);
                    f32 expectedSpeed = speeds[v];
                    for (int update = 0; update < 9; update++) {
                        f32 before = sMario.pos[0];
                        expectedSpeed = approach_f32(expectedSpeed, 0.0f, GROUND_NEUTRAL_COAST_DECEL,
                                                     GROUND_NEUTRAL_COAST_DECEL);
                        sClimbEnded = update == 8;
                        EXPECT_EQ(act_air_throw_land(&sMario), FALSE);
                        EXPECT_NEAR(sMario.forwardVel, expectedSpeed);
                        EXPECT_NEAR(sMario.pos[0] - before, expectedSpeed);
                        EXPECT_EQ(sAnimation, MARIO_ANIM_THROW_LIGHT_OBJECT);
                    }
                    EXPECT_EQ(sMario.heldObj == NULL, TRUE);
                    EXPECT_EQ(sThrowReleases, clocks[t] < 4 ? 1 : 0);
                    EXPECT_EQ(sMario.action,
                              fabsf(expectedSpeed) >= 1.0f ? ACT_DECELERATING : ACT_IDLE);
                    EXPECT_EQ(sGravityUpdates, 1);
                }
            }
        }
    }
    // Damage sees the contact before continuation; it keeps ownership of the reaction.
    begin(40, 40, 1);
    sMario.action = ACT_AIR_THROW;
    sReaction = ACT_HARD_BACKWARD_GROUND_KB;
    act_air_throw(&sMario);
    EXPECT_EQ(sMario.action, sReaction);
    EXPECT_EQ(sGroundChecks, 0);
    // A brief touchdown on a tiny platform resumes the same throw clock.
    begin(40, 40, 1);
    sMario.action = ACT_AIR_THROW;
    sMario.heldObj = &sObject;
    sEdgeX = 10;
    act_air_throw(&sMario);
    EXPECT_EQ(sMario.action, ACT_AIR_THROW);
    EXPECT_EQ(sMario.actionTimer, 1);
    EXPECT_EQ(sThrowReleases, 0);
    sEdgeX = INFINITY;
    for (int update = 0; update < 3; update++) {
        if (sMario.action == ACT_AIR_THROW) {
            act_air_throw(&sMario);
        } else {
            act_air_throw_land(&sMario);
        }
    }
    EXPECT_EQ(sThrowReleases, 1);
    return TRUE;
}

static int air_throw_cancel_window(void) {
    for (int frame = 5; frame <= 7; frame++) {
        begin(0, 0, 0);
        sMario.action = ACT_AIR_THROW_LAND;
        sMario.actionTimer = 4;
        sMario.heldObj = NULL;
        sObject.header.gfx.animInfo.animID = MARIO_ANIM_THROW_LIGHT_OBJECT;
        sObject.header.gfx.animInfo.animFrame = frame;
        sLandingCancelRequested = TRUE;
        EXPECT_EQ(act_air_throw_land(&sMario), frame >= 6);
        EXPECT_EQ(sMario.action, frame >= 6 ? ACT_JUMP : ACT_AIR_THROW_LAND);
    }
    sLandingCancelRequested = FALSE;
    return TRUE;
}

static int fast_ledge_control_window(void) {
    for (int frame = 4; frame <= 6; frame++) {
        begin(0, 0, 0);
        sMario.action = ACT_LEDGE_CLIMB_FAST;
        sClimbCompletions = 0;
        sObject.header.gfx.animInfo.animID = MARIO_ANIM_FAST_LEDGE_GRAB;
        sObject.header.gfx.animInfo.animFrame = frame;
        EXPECT_EQ(act_ledge_climb_fast(&sMario), frame >= 5);
        EXPECT_EQ(sMario.action, frame >= 5 ? ACT_IDLE : ACT_LEDGE_CLIMB_FAST);
        EXPECT_EQ(sLandingSounds, frame >= 5 ? 1 : 0);
        EXPECT_EQ(sClimbCompletions, frame >= 5 ? 1 : 0);
    }
    const f32 magnitudes[] = { 0, 54.3f, 54.4f, 64 };
    const s16 directions[] = { 0, 0x1000, 0x4000, (s16) 0x8000 };
    for (unsigned magnitude = 0; magnitude < 4; magnitude++) {
        for (unsigned direction = 0; direction < 4; direction++) {
            begin(3, 0, 0);
            sMario.action = ACT_LEDGE_CLIMB_FAST;
            sMario.input = INPUT_NONZERO_ANALOG;
            sController.stickMag = magnitudes[magnitude];
            sMario.intendedYaw = directions[direction];
            sObject.header.gfx.animInfo.animID = MARIO_ANIM_FAST_LEDGE_GRAB;
            sObject.header.gfx.animInfo.animFrame = 5;
            sMario.automaticAFrames = sMario.automaticBFrames = 1;
            EXPECT_EQ(act_ledge_climb_fast(&sMario), TRUE);
            int boost = magnitudes[magnitude] >= 64.0f * 0.85f
                        && magnitudes[magnitude] * coss(directions[direction]) >= 64.0f * 0.78f;
            EXPECT_EQ(sMario.forwardVel, boost ? 23 : 3);
            EXPECT_EQ(sMario.input & (INPUT_A_PRESSED | INPUT_B_PRESSED),
                      INPUT_A_PRESSED | INPUT_B_PRESSED);
        }
    }
    begin(3, 0, 0);
    sMario.action = ACT_LEDGE_CLIMB_FAST;
    sObject.header.gfx.animInfo.animID = MARIO_ANIM_FAST_LEDGE_GRAB;
    sObject.header.gfx.animInfo.animFrame = 4;
    sMario.automaticAFrames = sMario.automaticBFrames = 1;
    EXPECT_EQ(act_ledge_climb_fast(&sMario), FALSE);
    EXPECT_EQ(sMario.input, 0);
    const u16 releases[] = { INPUT_Z_PRESSED, INPUT_OFF_FLOOR };
    for (unsigned i = 0; i < sizeof(releases) / sizeof(releases[0]); i++) {
        begin(0, 0, 0);
        sMario.action = ACT_LEDGE_CLIMB_FAST;
        sMario.input = releases[i];
        EXPECT_EQ(act_ledge_climb_fast(&sMario), TRUE);
        EXPECT_EQ(sMario.action, ACT_SOFT_BONK);
        EXPECT_EQ(sLandingSounds, 0);
    }
    return TRUE;
}

int main(int argc, char **argv) {
    const struct {
        const char *name;
        int (*run)(void);
    } cases[] = {
        { "landing.travel", travel },
        { "landing.repeated", repeated },
        { "landing.wall", wall },
        { "landing.floor_change", floor_change },
        { "landing.leave_floor", leave_floor },
        { "landing.reactions", reactions },
        { "landing.legacy_step", legacy_step },
        { "landing.no_landing", no_landing },
        { "landing.final_substep", final_substep },
        { "landing.jump_buffer_handoff", jump_buffer_handoff },
        { "landing.contact_sound", contact_sound },
        { "landing.ledge_voice", ledge_voice },
        { "landing.ledge_climb_skip", ledge_climb_skip },
        { "landing.fast_ledge_control_window", fast_ledge_control_window },
        { "landing.ledge_climb_down_voice", ledge_climb_down_voice },
        { "landing.ledge_release_controls", ledge_release_controls },
        { "landing.handstand_touchdown", handstand_touchdown },
        { "landing.air_throw_touchdown", air_throw_touchdown },
        { "landing.air_throw_cancel_window", air_throw_cancel_window },
    };
    test_init(argc, argv);
    for (unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        if (test_selected(cases[i].name)) {
            test_report(cases[i].name, cases[i].run());
        }
    }
    return test_finish();
}
