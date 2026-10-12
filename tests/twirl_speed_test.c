/* Actual twirl speed and wall-rebound updates on a flat host fixture. */
#include <math.h>
#include <stdint.h>
#include "test_support.h"

typedef int16_t s16;
typedef int32_t s32;
typedef s32 Bool32;
typedef uint8_t u8;
typedef uint32_t u32;
typedef float f32;

#define TRUE 1
#define FALSE 0
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) MIN(MAX(x, lo), hi)
#define absf(x) fabsf(x)
#define DEGREES(x) ((s16) ((x) * 65536.0f / 360.0f))
#define INPUT_NONZERO_ANALOG 1
#define ACT_GROUND_TWIRL 1
#define ACT_WALKING 2
#define ACT_METAL_WATER_WALKING 3
#define ACT_CRAWLING 4
#define ACT_HOLD_WALKING 5
#define SURFACE_SLOW 1
#define SURFACE_NOT_SLIPPERY 2
#define SURFACE_CLASS_NOT_SLIPPERY 3

#include "twirl_speed_definitions.inc"

struct Surface {
    s32 type;
    struct {
        f32 x, y, z;
    } normal;
};
struct Object {
    struct {
        struct {
            s16 angle[3];
        } gfx;
    } header;
};
struct MarioState {
    const struct MarioMovementProfile *movementProfile;
    u32 action, input, actionArg, flags, particleFlags;
    s32 terrainSoundAddend;
    f32 pos[3], waterLevel;
    s32 actionTimer, actionState;
    s16 faceAngle[3], intendedYaw, wallYaw;
    f32 forwardVel, intendedMag, quicksandDepth;
    f32 slideVelX, slideVelZ, vel[3];
    struct Surface *floor, *wall;
    struct Object *marioObj;
    struct Object *heldObj;
};
static struct {
    f32 startSpeed, bounceBonus, appliedBonus, earlySpeedCap;
    s32 boostAge, hasBounced, earlySpeedCapActive;
    u8 wallBounceCount;
    s32 wallContact;
    s16 wallContactYaw;
    s32 bufferedInputFrames, bufferedInput, visualPhaseYaw, visualPrevFaceYaw, visualTargetOffset,
        visualWallBounce;
} sGroundTwirl;
static u8 gDebugFastWalk;
static inline s32 game_options_debug_active(void) {
    return sTestGameOptions[GO_DEBUG_MODE];
}
static s32 gCurrSaveFileNum = 3;
static s32 sHeThiccFile;
static s32 save_file_is_he_thicc(s32 file) {
    return file == 2 && sHeThiccFile;
}

static f32 sins(s16 angle) {
    return sinf(angle * (6.28318530718f / 65536.0f));
}
static f32 coss(s16 angle) {
    return cosf(angle * (6.28318530718f / 65536.0f));
}
static s32 abs_angle_diff(s16 a, s16 b) {
    s32 difference = (s16) (a - b);
    return difference < 0 ? -difference : difference;
}
static s32 mario_floor_is_slippery(struct MarioState *m) {
    return FALSE;
}
static s32 mario_floor_is_slope(struct MarioState *m) {
    return FALSE;
}
static s32 mario_get_floor_class(struct MarioState *m) {
    return m->floor->type == SURFACE_NOT_SLIPPERY ? SURFACE_CLASS_NOT_SLIPPERY : 0;
}
static s16 approach_s16_symmetric(s16 current, s16 target, s16 step) {
    return target;
}
static s32 approach_s32(s32 current, s32 target, s32 inc, s32 dec) {
    return target;
}
static f32 approach_f32(f32 current, f32 target, f32 inc, f32 dec) {
    return target;
}
static void apply_slope_accel(struct MarioState *m) {
    m->slideVelX = m->vel[0] = m->forwardVel * sins(m->faceAngle[1]);
    m->slideVelZ = m->vel[2] = m->forwardVel * coss(m->faceAngle[1]);
}
static void mario_set_forward_vel(struct MarioState *m, f32 speed) {
    m->forwardVel = speed;
    apply_slope_accel(m);
}
static void update_decelerating_speed(struct MarioState *m) {
    mario_set_forward_vel(m, MAX(0.0f, m->forwardVel - 1.0f));
}
static s32 sImpactRumbles, sImpactTime, sImpactStrength;
void queue_rumble_data(s32 time, s32 strength) {
    sImpactRumbles++;
    sImpactTime = time;
    sImpactStrength = strength;
}
static void mario_bonk_reflection(struct MarioState *m, s32 strong) {
}
static void mario_play_wall_bonk_sound(struct MarioState *m) {
}

#include "twirl_speed_sources.inc"

#define RUN_CASE(name, fn)                                                                             \
    do {                                                                                               \
        if (test_selected(name))                                                                       \
            test_report(name, fn());                                                                   \
    } while (0)
static struct MarioState m;
static struct Surface floorSurface, wallSurface;
static struct Object mario;

static void begin(f32 speed, f32 intendedMag) {
    sImpactRumbles = sImpactTime = sImpactStrength = 0;
    memset(&m, 0, sizeof(m));
    mario_character_init();
    mario_character_commit(&m, FALSE);
    memset(&sGroundTwirl, 0, sizeof(sGroundTwirl));
    memset(&floorSurface, 0, sizeof(floorSurface));
    memset(&wallSurface, 0, sizeof(wallSurface));
    memset(&mario, 0, sizeof(mario));
    floorSurface.normal.y = 1.0f;
    wallSurface.normal.z = -1.0f;
    m.floor = &floorSurface;
    m.wall = &wallSurface;
    m.marioObj = &mario;
    m.waterLevel = -10000.0f;
    m.action = ACT_GROUND_TWIRL;
    m.input = INPUT_NONZERO_ANALOG;
    m.intendedMag = intendedMag;
    sGroundTwirl.startSpeed = speed;
    mario_set_forward_vel(&m, speed);
}

static int ordinary_walk_settles_at_forty(void) {
    begin(38.0f, 32.0f);
    m.action = ACT_WALKING;
    for (int frame = 0; frame < 30; frame++) {
        update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
        EXPECT_EQ(m.forwardVel <= WALK_TARGET_SPEED, TRUE);
    }
    EXPECT_EQ(m.forwardVel == WALK_TARGET_SPEED, TRUE);
    for (int frame = 0; frame < 60; frame++) {
        update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
        EXPECT_EQ(m.forwardVel == WALK_TARGET_SPEED, TRUE);
    }
    return TRUE;
}

static int punch_recovery_turn(void) {
    const f32 speeds[] = { 0.0f, 8.0f, -8.0f, 20.0f };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int i = 0; i < 4; i++) {
            for (int direction = -1; direction <= 1; direction += 2) {
                begin(speeds[i], 8.0f);
                m.action = ACT_WALKING;
                m.actionArg = ACT_ARG_WALKING_TURN_FROM_PUNCH;
                m.intendedYaw = direction * 0x4000;
                for (int frame = 0; frame < 16 && m.faceAngle[1] != m.intendedYaw; frame++) {
                    s16 facing = m.faceAngle[1];
                    update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
                    s16 turn = direction * (s16) (m.faceAngle[1] - facing);
                    EXPECT_EQ(turn > 0 && turn <= 0x2000, TRUE);
                    if (speeds[i] < 16.0f) {
                        EXPECT_EQ(turn <= 0x800, TRUE);
                    }
                }
                EXPECT_EQ(m.faceAngle[1], m.intendedYaw);
                EXPECT_EQ(m.actionArg, 0);

                // Once aligned, ordinary low-speed walking resumes immediately.
                m.forwardVel = 8.0f;
                m.intendedYaw -= direction * 0x1000;
                update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
                EXPECT_EQ(m.faceAngle[1], m.intendedYaw);
            }
        }
    }
    return TRUE;
}

static int grippy_slope_returns_carried_speed_to_forty(void) {
    begin(52.0f, 32.0f);
    m.action = ACT_WALKING;
    floorSurface.type = SURFACE_NOT_SLIPPERY;
    floorSurface.normal.y = 0.8f;
    for (int frame = 0; frame < 12; frame++) {
        update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
        EXPECT_EQ(fabsf(m.forwardVel - (51.0f - frame)) < 0.001f, TRUE);
    }
    for (int frame = 0; frame < 30; frame++) {
        update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
        EXPECT_EQ(m.forwardVel == WALK_TARGET_SPEED, TRUE);
    }
    begin(52.0f, 32.0f);
    m.action = ACT_WALKING;
    floorSurface.normal.y = 0.8f;
    update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
    EXPECT_EQ(m.forwardVel, 52.0f); // Other slope classes retain their acceleration behavior.
    return TRUE;
}

static int gains_without_wall(void) {
    begin(24.0f, 32.0f);
    for (int frame = 0; frame < 12; frame++) {
        f32 previous = m.forwardVel;
        mario_ground_twirl_update_speed(&m, FALSE);
        EXPECT_EQ(m.forwardVel > previous, TRUE);
    }
    for (int frame = 0; frame < 80; frame++) {
        mario_ground_twirl_update_speed(&m, FALSE);
    }
    EXPECT_EQ(m.forwardVel >= 39.0f && m.forwardVel <= 41.0f, TRUE);
    return TRUE;
}

static int gains_after_wall(void) {
    for (int early = 0; early < 2; early++) {
        begin(early ? 12.0f : 32.0f, 32.0f);
        m.actionTimer = early ? 1 : 6;
        EXPECT_EQ(mario_ground_twirl_handle_wall(&m, FALSE), TRUE);
        EXPECT_EQ(sGroundTwirl.earlySpeedCapActive, early);
        if (early) {
            EXPECT_EQ(sGroundTwirl.earlySpeedCap >= 40.0f, TRUE);
        }
        for (int frame = 0; frame < 12; frame++) {
            f32 previous = m.forwardVel;
            mario_ground_twirl_update_speed(&m, TRUE);
            if (previous < 40.0f) {
                EXPECT_EQ(m.forwardVel > previous, TRUE);
            }
        }
        for (int frame = 0; frame < 80; frame++) {
            mario_ground_twirl_update_speed(&m, FALSE);
        }
        EXPECT_EQ(m.forwardVel >= 39.0f && m.forwardVel <= 41.0f, TRUE);
    }
    return TRUE;
}

static int wall_facing_boost(void) {
    const s16 yaws[] = { 0, DEGREES(60), DEGREES(90), DEGREES(120), (s16) 0x8000 };
    for (unsigned i = 0; i < sizeof(yaws) / sizeof(*yaws); i++) {
        begin(20, 32);
        m.wallYaw = yaws[i];
        wallSurface.normal.x = sins(m.wallYaw);
        wallSurface.normal.z = coss(m.wallYaw);
        s32 oldContact = m.vel[2] * wallSurface.normal.z < 0
                         && !(abs_angle_diff(m.wallYaw, m.faceAngle[1]) >= DEGREES(60)
                              && abs_angle_diff(m.wallYaw, m.faceAngle[1]) <= DEGREES(120));
        EXPECT_EQ(mario_ground_twirl_handle_wall(&m, FALSE), TRUE);
        EXPECT_EQ(m.particleFlags & PARTICLE_VERTICAL_STAR, PARTICLE_VERTICAL_STAR);
        EXPECT_EQ(sImpactRumbles, 1);
        EXPECT_EQ(sImpactTime, 5);
        EXPECT_EQ(sImpactStrength, 40);
        EXPECT_EQ(m.faceAngle[1], 0); // Reflection is a controlled service in this fixture.
        EXPECT_EQ(m.forwardVel > 20 && m.forwardVel <= 64, TRUE);
        if (!oldContact) {
            f32 speed = m.forwardVel;
            m.particleFlags = 0;
            EXPECT_EQ(mario_ground_twirl_handle_wall(&m, FALSE), FALSE);
            EXPECT_EQ(m.particleFlags, 0);
            EXPECT_EQ(sImpactRumbles, 1);
            EXPECT_EQ(m.forwardVel, speed);
            EXPECT_EQ(sGroundTwirl.wallBounceCount, 1);
            m.wall = NULL;
            mario_ground_twirl_handle_wall(&m, FALSE);
            m.wall = &wallSurface;
            mario_set_forward_vel(&m, 20);
            EXPECT_EQ(mario_ground_twirl_handle_wall(&m, FALSE), TRUE);
            EXPECT_EQ(sGroundTwirl.wallBounceCount, 2);
            EXPECT_EQ(m.particleFlags & PARTICLE_VERTICAL_STAR, PARTICLE_VERTICAL_STAR);
            EXPECT_EQ(sImpactRumbles, 2);
        }
    }
    return TRUE;
}
static int wall_bounce_speed_cap(void) {
    const f32 speeds[] = { 64.0f, 80.0f, 128.0f };
    for (unsigned i = 0; i < sizeof(speeds) / sizeof(speeds[0]); i++) {
        for (int backwards = 0; backwards < 2; backwards++) {
            begin(backwards ? -speeds[i] : speeds[i], 32.0f);
            wallSurface.normal.z = backwards ? 1.0f : -1.0f;
            EXPECT_EQ(mario_ground_twirl_handle_wall(&m, FALSE), TRUE);
            EXPECT_EQ(fabsf(m.forwardVel - 64.0f) < 0.001f, TRUE);
            for (int frame = 0; frame < 12; frame++) {
                mario_ground_twirl_update_speed(&m, FALSE);
                EXPECT_EQ(m.forwardVel <= 64.0f, TRUE);
            }
        }
    }
    begin(12.0f, 32.0f);
    m.actionTimer = 1;
    EXPECT_EQ(mario_ground_twirl_handle_wall(&m, FALSE), TRUE);
    EXPECT_EQ(sGroundTwirl.earlySpeedCap < 64.0f, TRUE);
    mario_set_forward_vel(&m, 64.0f);
    m.actionTimer = 6;
    EXPECT_EQ(mario_ground_twirl_handle_wall(&m, FALSE), TRUE);
    EXPECT_EQ(fabsf(m.forwardVel - 64.0f) < 0.001f, TRUE);
    EXPECT_EQ(sGroundTwirl.earlySpeedCap, 64.0f);
    return TRUE;
}

static int wall_bounce_retention(void) {
    for (int backwards = 0; backwards < 2; backwards++) {
        begin(30.0f, 32.0f);
        for (int bounce = 1; bounce <= 19; bounce++) {
            mario_set_forward_vel(&m, backwards ? -30.0f : 30.0f);
            wallSurface.normal.z = backwards ? 1.0f : -1.0f;
            EXPECT_EQ(mario_ground_twirl_handle_wall(&m, TRUE), TRUE);
            f32 retained = bounce >= 18 ? 0.0f : 1.0f - (bounce - 1) * 0.06f;
            EXPECT_EQ(fabsf(m.forwardVel - 30.0f * retained) < 0.001f, TRUE);
            EXPECT_EQ(sGroundTwirl.wallBounceCount, bounce >= 18 ? 18 : bounce);
            m.faceAngle[1] = 0;
        }
    }
    return TRUE;
}

static int repeat_bounce_and_release(void) {
    begin(30.0f, 32.0f);
    m.actionTimer = 6;
    EXPECT_EQ(mario_ground_twirl_handle_wall(&m, FALSE), TRUE);
    sGroundTwirl.wallBounceCount = 5;
    for (int frame = 0; frame < 8; frame++) {
        f32 previous = m.forwardVel;
        mario_ground_twirl_update_speed(&m, TRUE);
        if (previous < 40.0f) {
            EXPECT_EQ(m.forwardVel > previous, TRUE);
        }
    }
    m.input = 0;
    f32 previous = m.forwardVel;
    mario_ground_twirl_update_speed(&m, FALSE);
    EXPECT_EQ(m.forwardVel < previous, TRUE);
    return TRUE;
}

static int partial_stick_target(void) {
    begin(8.0f, 16.0f);
    m.actionTimer = 1;
    EXPECT_EQ(mario_ground_twirl_handle_wall(&m, FALSE), TRUE);
    for (int frame = 0; frame < 80; frame++) {
        mario_ground_twirl_update_speed(&m, FALSE);
    }
    EXPECT_EQ(m.forwardVel >= 19.0f && m.forwardVel <= 21.0f, TRUE);
    return TRUE;
}

static int held_stick_cancels_negative_speed(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int action = ACT_WALKING; action <= ACT_HOLD_WALKING; action += 3) {
            begin(-18.0f, 32.0f);
            m.action = action;
            for (int frame = 0; frame < 3; frame++) {
                update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
                EXPECT_EQ(fabsf(m.forwardVel - (-12.0f + 6.0f * frame)) < 0.001f, TRUE);
                EXPECT_EQ(fabsf(m.vel[2] - m.forwardVel) < 0.001f, TRUE);
            }
            update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            EXPECT_EQ(m.forwardVel > 0.0f, TRUE);
        }
    }

    begin(-12.0f, 32.0f);
    m.action = ACT_WALKING;
    m.input = 0;
    update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
    EXPECT_EQ(m.forwardVel < -10.0f, TRUE);

    begin(-12.0f, 32.0f);
    m.action = ACT_WALKING;
    floorSurface.type = SURFACE_SLOW;
    update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
    EXPECT_EQ(m.forwardVel < -10.0f, TRUE);

    begin(-12.0f, 32.0f);
    m.action = ACT_GROUND_TWIRL;
    update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
    EXPECT_EQ(m.forwardVel < -10.0f, TRUE);

    for (int cycle = 0; cycle < 3; cycle++) {
        for (int action = ACT_WALKING; action <= ACT_HOLD_WALKING; action += 3) {
            begin(-18.0f, 32.0f);
            m.action = action;
            m.intendedYaw = DEGREES(180);
            update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            EXPECT_EQ(m.forwardVel, 0.0f);
            EXPECT_EQ(m.vel[0], 0.0f);
            EXPECT_EQ(m.vel[2], 0.0f);
            EXPECT_EQ(m.faceAngle[1], m.intendedYaw);
            update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            EXPECT_EQ(m.forwardVel > 0.0f, TRUE);
        }
    }
    begin(-18.0f, 32.0f);
    m.action = ACT_WALKING;
    m.intendedYaw = DEGREES(90);
    update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
    EXPECT_EQ(m.forwardVel, -12.0f);
    return TRUE;
}

static int character_walk_profiles(void) {
    sTestGameOptions[GO_DEBUG_MODE] = TRUE;
    for (int choice = 0; choice < MARIO_CHARACTER_COUNT; choice++) {
        for (int halfStick = 0; halfStick < 2; halfStick++) {
            begin(0.0f, halfStick ? 16.0f : 32.0f);
            sHeThiccFile = choice & 1;
            mario_character_request(choice);
            mario_character_commit(&m, FALSE);
            gDebugFastWalk = FALSE;
            m.action = ACT_WALKING;
            update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            EXPECT_EQ(fabsf(m.forwardVel - 1.375f) < 0.001f, TRUE);
            for (int frame = 0; frame < 180; frame++) {
                update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            }
            f32 target = 40.0f;
            if (halfStick) {
                target *= 0.5f;
            }
            EXPECT_EQ(fabsf(m.forwardVel - target) < 0.001f, TRUE);
        }
        gDebugFastWalk = TRUE;
        m.intendedMag = 32.0f;
        for (int frame = 0; frame < 500; frame++) {
            update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
        }
        EXPECT_EQ(fabsf(m.forwardVel - 127.0f) < 0.001f, TRUE);
        gDebugFastWalk = FALSE;
    }
    // A stale fast-walk toggle must stop taking effect as soon as consent is revoked.
    sTestGameOptions[GO_DEBUG_MODE] = FALSE;
    begin(0.0f, 32.0f);
    gDebugFastWalk = TRUE;
    m.action = ACT_WALKING;
    for (int frame = 0; frame < 500; frame++) {
        update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
    }
    EXPECT_EQ(fabsf(m.forwardVel - m.movementProfile->walkTarget) < 0.001f, TRUE);
    gDebugFastWalk = FALSE;
    return TRUE;
}

static int running_water_and_sand_targets(void) {
    const u32 caps[] = { 0,
                         MARIO_METAL_CAP,
                         MARIO_VANISH_CAP,
                         MARIO_METAL_CAP | MARIO_VANISH_CAP,
                         MARIO_WING_CAP,
                         MARIO_WING_CAP | MARIO_METAL_CAP,
                         MARIO_WING_CAP | MARIO_VANISH_CAP,
                         MARIO_WING_CAP | MARIO_METAL_CAP | MARIO_VANISH_CAP };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int sand = 0; sand < 2; sand++) {
            for (int halfStick = 0; halfStick < 2; halfStick++) {
                for (unsigned cap = 0; cap < sizeof(caps) / sizeof(*caps); cap++) {
                    for (int debug = 0; debug < 2; debug++) {
                        begin(0.0f, halfStick ? 16.0f : 32.0f);
                        gDebugFastWalk = debug;
                        sTestGameOptions[GO_DEBUG_MODE] = debug;
                        m.action = ACT_WALKING;
                        m.flags = caps[cap];
                        m.waterLevel = sand ? -10000.0f : 40.0f;
                        m.terrainSoundAddend = (sand ? SOUND_TERRAIN_SAND : SOUND_TERRAIN_WATER) << 16;
                        for (int frame = 0; frame < 500; frame++) {
                            update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
                        }
                        f32 target = 32.0f;
                        if (!sand && (caps[cap] & (MARIO_VANISH_CAP | MARIO_WING_CAP))) {
                            target = (caps[cap] & MARIO_WING_CAP) ? 48.0f : 40.0f;
                            if (debug) {
                                target = 127.0f;
                            }
                        }
                        if (halfStick) {
                            target *= 0.5f;
                        }
                        EXPECT_EQ(fabsf(m.forwardVel - target) < 0.001f, TRUE);
                    }
                }
            }
        }
    }
    gDebugFastWalk = FALSE;
    sTestGameOptions[GO_DEBUG_MODE] = 0;
    for (int action = 0; action < 2; action++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            begin(40.0f, 32.0f);
            m.action = action ? ACT_METAL_WATER_WALKING : ACT_WALKING;
            m.flags = MARIO_WING_CAP | MARIO_METAL_CAP;
            for (int frame = 0; frame < 100; frame++) {
                update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            }
            EXPECT_EQ(m.forwardVel, 48.0f);
            m.waterLevel = 40.0f;
            for (int frame = 0; frame < 100; frame++) {
                update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            }
            EXPECT_EQ(m.forwardVel, 48.0f);
            m.flags = MARIO_METAL_CAP;
            update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            EXPECT_EQ(m.forwardVel, 47.0f);
            for (int frame = 0; frame < 100; frame++) {
                update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            }
            EXPECT_EQ(m.forwardVel, 32.0f);
            m.flags |= MARIO_WING_CAP;
            for (int frame = 0; frame < 100; frame++) {
                update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            }
            EXPECT_EQ(m.forwardVel, 48.0f);
        }
    }
    // Held-object walking and twirl keep their own targets.
    begin(0.0f, 20.0f);
    m.action = ACT_HOLD_WALKING;
    m.waterLevel = 40.0f;
    for (int frame = 0; frame < 100; frame++) {
        update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
    }
    EXPECT_EQ(m.forwardVel, 20.0f);
    begin(0.0f, 32.0f);
    m.waterLevel = 40.0f;
    for (int frame = 0; frame < 100; frame++) {
        update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
    }
    EXPECT_EQ(m.forwardVel, GROUND_TWIRL_TARGET_SPEED);
    return TRUE;
}

static int running_terrain_transitions(void) {
    gDebugFastWalk = FALSE;
    const f32 depths[] = { 0.0f, 10.0f, 10.01f, 80.0f };
    for (unsigned depth = 0; depth < sizeof(depths) / sizeof(*depths); depth++) {
        begin(40.0f, 32.0f);
        m.action = ACT_WALKING;
        m.waterLevel = depths[depth];
        m.flags = MARIO_METAL_CAP;
        update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
        EXPECT_EQ(m.forwardVel, depths[depth] > 10.0f ? 39.0f : 40.0f);
    }
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int sand = 0; sand < 2; sand++) {
            begin(72.0f, 32.0f);
            m.action = ACT_WALKING;
            m.waterLevel = sand ? -10000.0f : 40.0f;
            m.terrainSoundAddend = sand ? SOUND_TERRAIN_SAND << 16 : 0;
            update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            EXPECT_EQ(m.forwardVel, 71.0f);
            for (int frame = 0; frame < 80; frame++) {
                update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            }
            EXPECT_EQ(m.forwardVel, 32.0f);
            m.waterLevel = -10000.0f;
            m.terrainSoundAddend = SOUND_TERRAIN_STONE << 16;
            for (int frame = 0; frame < 80; frame++) {
                update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            }
            EXPECT_EQ(m.forwardVel, 40.0f);
        }
    }
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(32.0f, 32.0f);
        m.action = ACT_WALKING;
        m.waterLevel = 40.0f;
        m.flags = MARIO_VANISH_CAP;
        for (int frame = 0; frame < 80; frame++) {
            update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
        }
        EXPECT_EQ(m.forwardVel, 40.0f);
        m.flags = 0;
        update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
        EXPECT_EQ(m.forwardVel, 39.0f);
        for (int frame = 0; frame < 80; frame++) {
            update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
        }
        EXPECT_EQ(m.forwardVel, 32.0f);
    }
    begin(24.0f, 32.0f);
    m.action = ACT_WALKING;
    m.waterLevel = 40.0f;
    m.flags = MARIO_WING_CAP;
    floorSurface.type = SURFACE_SLOW;
    update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
    EXPECT_EQ(m.forwardVel, 24.0f);
    begin(0.0f, 32.0f);
    m.action = ACT_WALKING;
    m.terrainSoundAddend = SOUND_TERRAIN_SAND << 16;
    m.quicksandDepth = 25.0f;
    for (int frame = 0; frame < 100; frame++) {
        update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
    }
    EXPECT_EQ(m.forwardVel, 8.0f);
    return TRUE;
}

static int wing_preset(void) {
    for (s32 original = 0; original < 2; original++) {
        sTestGameOptions[GO_WING_CAP] = original;
        for (s32 wet = 0; wet < 2; wet++) {
            begin(0.0f, 32.0f);
            m.action = ACT_WALKING;
            m.flags = MARIO_WING_CAP;
            m.waterLevel = wet ? 100.0f : -10000.0f;
            for (s32 frame = 0; frame < 100; frame++) {
                update_walking_speed_with_minimum(&m, 0.0f, 100.0f);
            }
            EXPECT_EQ(m.forwardVel, original ? (wet ? 32.0f : WALK_TARGET_SPEED) : 48.0f);
        }
    }
    sTestGameOptions[GO_WING_CAP] = 0;
    return TRUE;
}

static int ignore_sand_speed(void) {
    gDebugFastWalk = FALSE;
    sTestGameOptions[GO_CHEATS_ENABLED] = TRUE;
    sTestGameOptions[GO_IGNORE_SAND] = TRUE;
    for (int slow = 0; slow < 2; slow++) {
        for (int half = 0; half < 2; half++) {
            begin(0, half ? 16 : 32);
            m.action = ACT_WALKING;
            m.terrainSoundAddend = SOUND_TERRAIN_SAND << 16;
            floorSurface.type = slow ? SURFACE_SLOW : 0;
            m.quicksandDepth = 25;
            for (int frame = 0; frame < 150; frame++) {
                update_walking_speed_with_minimum(&m, 0, 48);
            }
            EXPECT_EQ(m.forwardVel, half ? 20 : 40);
            m.waterLevel = 40;
            for (int frame = 0; frame < 150; frame++) {
                update_walking_speed_with_minimum(&m, 0, 48);
            }
            EXPECT_EQ(m.forwardVel, half ? 16 : 32);
        }
    }
    begin(40, 32);
    m.action = ACT_WALKING;
    m.terrainSoundAddend = SOUND_TERRAIN_SAND << 16;
    sTestGameOptions[GO_CHEATS_ENABLED] = FALSE;
    for (int frame = 0; frame < 150; frame++) {
        update_walking_speed_with_minimum(&m, 0, 48);
    }
    EXPECT_EQ(m.forwardVel, 32);
    sTestGameOptions[GO_IGNORE_SAND] = FALSE;
    return TRUE;
}

static int unlimited_speed(void) {
    for (int master = 0; master < 2; master++) {
        for (int enabled = 0; enabled < 2; enabled++) {
            sTestGameOptions[GO_CHEATS_ENABLED] = master;
            sTestGameOptions[GO_NO_SPEED_LIMIT] = enabled;
            begin(300.0f, 32.0f);
            m.action = ACT_HOLD_WALKING;
            update_walking_speed_with_minimum(&m, 0.0f, 48.0f);
            EXPECT_EQ(m.forwardVel, master && enabled ? 299.0f : 48.0f);
            begin(300.0f, 32.0f);
            EXPECT_EQ(mario_ground_twirl_handle_wall(&m, FALSE), TRUE);
            EXPECT_EQ(m.forwardVel > 300.0f, master && enabled);
            if (!(master && enabled)) {
                EXPECT_EQ(fabsf(m.forwardVel - 64.0f) < 0.001f, TRUE);
            }
            mario_ground_twirl_update_speed(&m, FALSE);
            EXPECT_EQ(m.forwardVel > 64.0f, master && enabled);
        }
    }
    sTestGameOptions[GO_CHEATS_ENABLED] = sTestGameOptions[GO_NO_SPEED_LIMIT] = 0;
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    RUN_CASE("twirl_speed.unlimited_speed", unlimited_speed);
    RUN_CASE("twirl_speed.ignore_sand", ignore_sand_speed);
    RUN_CASE("twirl_speed.wing_preset", wing_preset);
    RUN_CASE("twirl_speed.wall_bounce_speed_cap", wall_bounce_speed_cap);
    RUN_CASE("twirl_speed.wall_facing_boost", wall_facing_boost);
    RUN_CASE("twirl_speed.wall_bounce_retention", wall_bounce_retention);
    RUN_CASE("twirl_speed.running_water_and_sand", running_water_and_sand_targets);
    RUN_CASE("twirl_speed.running_terrain_transitions", running_terrain_transitions);
    RUN_CASE("twirl_speed.character_profiles", character_walk_profiles);
    RUN_CASE("twirl_speed.walk_forty", ordinary_walk_settles_at_forty);
    RUN_CASE("twirl_speed.punch_recovery_turn", punch_recovery_turn);
    RUN_CASE("twirl_speed.grippy_slope", grippy_slope_returns_carried_speed_to_forty);
    RUN_CASE("twirl_speed.no_wall", gains_without_wall);
    RUN_CASE("twirl_speed.wall_recovery", gains_after_wall);
    RUN_CASE("twirl_speed.repeat_and_release", repeat_bounce_and_release);
    RUN_CASE("twirl_speed.partial_stick", partial_stick_target);
    RUN_CASE("twirl_speed.held_stick_negative_speed", held_stick_cancels_negative_speed);
    return test_finish();
}
