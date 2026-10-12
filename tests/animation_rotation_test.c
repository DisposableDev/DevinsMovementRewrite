/* Production running lean and bouncing-box action/air steering. A planar floor
 * and supplied air contacts replace level loading; no renderer or ROM. */
#include <math.h>
#include <stdint.h>
#include "test_support.h"

typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
typedef f32 Vec3f[3];
typedef s16 Vec3s[3];

#define TRUE 1
#define FALSE 0
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(v, lo, hi) MAX((lo), MIN((hi), (v)))
#define sqr(v) ((v) * (v))
#define absf(v) fabsf(v)
#define DEGREES(v) ((s16) ((v) * 65536.0f / 360.0f))
#define SOUND_GENERAL_CRAZY_BOX_BOING_SLOW 1
#define SOUND_GENERAL_CRAZY_BOX_BOING_FAST 2
#define SOUND_ACTION_TERRAIN_JUMP 3
#define SOUND_ACTION_TERRAIN_LANDING 4
#define SOUND_ACTION_HIT 5
#define INT_STATUS_STOP_RIDING 1

#include "rotation_definitions.inc"

struct Surface {
    struct {
        f32 x, y, z;
    } normal;
};
struct Controller {
    f32 stickX, stickY;
};
struct Object {
    u32 oInteractStatus;
    struct {
        struct {
            Vec3s angle;
            Vec3f pos, cameraToObject;
        } gfx;
    } header;
};
struct MarioState {
    u32 action, input, actionArg, particleFlags;
    s32 actionTimer, wallKickChainCount;
    unsigned char wallTwirlSoundTimer, airTwirlFlightRollFrame;
    Vec3f pos, vel;
    Vec3s faceAngle, angleVel;
    s16 intendedYaw, wallYaw;
    f32 forwardVel, floorHeight, slideVelX, slideVelZ, intendedMag;
    struct Surface *floor, *wall;
    struct Controller *controller;
    struct Object *marioObj, *heldObj;
};

static struct MarioState mario;
static struct Object object, box;
static struct Controller controller;
static struct Surface testFloor;
static f32 planeSlope;
static u32 airResult;
static s32 rumbleCount, bonkCount, lavaCount, selectedAnimation;
extern f32 gSineTable[];
static f32 gravity_with_fall_drag(f32 verticalSpeed, f32 gravity);
static s16 atan2s(f32 y, f32 x);
void mario_play_wall_bonk_sound(struct MarioState *m);
void mario_bonk_reflection(struct MarioState *m, u32 negateSpeed);

static f32 get_surface_height_at_location(f32 x, f32 z, struct Surface *surface) {
    return planeSlope * z;
}

static f32 find_floor(f32 x, f32 y, f32 z, struct Surface **surface) {
    *surface = &testFloor;
    return planeSlope * z;
}

static f32 approach_f32(f32 value, f32 target, f32 increment, f32 decrement) {
    return value < target ? MIN(value + increment, target) : MAX(value - decrement, target);
}

/* These fixtures cover ordinary air controls; Wing twirl has geometry coverage. */
static f32 mario_air_twirl_control_scale(struct MarioState *m) {
    return 1.0f;
}

static void mario_air_twirl_limit_control_speed(struct MarioState *m) {
}

static s32 check_horizontal_wind(struct MarioState *m) {
    return FALSE;
}

// These marker policies do not apply to the bouncing-box action. Reject another
// action rather than silently supplying its untested uphill/slide physics.
static void apply_backward_butt_slide_jump_drag(struct MarioState *m) {
    if (m->action != ACT_CRAZY_BOX_BOUNCE) {
        abort();
    }
}

static void backward_butt_slide_jump_restrict_uphill(struct MarioState *m) {
    if (m->action != ACT_CRAZY_BOX_BOUNCE) {
        abort();
    }
}

static u32 perform_air_step(struct MarioState *m, u32 flags) {
    for (s32 axis = 0; axis < 3; axis++) {
        m->pos[axis] += m->vel[axis];
        m->marioObj->header.gfx.pos[axis] = m->pos[axis];
    }
    // Ordinary bouncing-box gravity; collision result is supplied separately.
    m->vel[1] = MAX(-MARIO_MAX_FALL_SPEED, m->vel[1] - gravity_with_fall_drag(m->vel[1], 4.0f));
    m->marioObj->header.gfx.angle[0] = 0;
    m->marioObj->header.gfx.angle[1] = m->faceAngle[1];
    m->marioObj->header.gfx.angle[2] = 0;
    return airResult;
}

static s32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
    m->actionTimer = 0;
    return TRUE;
}

static void set_mario_animation(struct MarioState *m, s32 animation) {
    selectedAnimation = animation;
}

static void play_sound(u32 sound, f32 *position) {
}
static void play_mario_sound(struct MarioState *m, u32 terrain, u32 voice) {
}

static void mario_he_thicc_contact_shake(struct MarioState *m) {
}

static void mario_play_bonk_sound(struct MarioState *m) {
    bonkCount++;
}

static void queue_rumble_data(s32 duration, s32 strength) {
    rumbleCount++;
}

static void lava_boost_on_wall(struct MarioState *m) {
    lavaCount++;
    set_mario_action(m, ACT_LAVA_BOOST, 0);
}

#include "rotation_sources.inc"

#define CHECK(value) EXPECT_EQ(!!(value), TRUE)

static void reset(void) {
    memset(&mario, 0, sizeof(mario));
    memset(&object, 0, sizeof(object));
    memset(&box, 0, sizeof(box));
    memset(&controller, 0, sizeof(controller));
    memset(&testFloor, 0, sizeof(testFloor));
    mario.marioObj = &object;
    mario.heldObj = &box;
    mario.controller = &controller;
    mario.floor = &testFloor;
    mario.action = ACT_CRAZY_BOX_BOUNCE;
    mario.pos[1] = 1000.0f;
    planeSlope = 0.0f;
    airResult = AIR_STEP_NONE;
    rumbleCount = bonkCount = lavaCount = 0;
    selectedAnimation = -1;
}

static int running_lean(void) {
    const f32 grades[] = { -70, -35, -10, 0, 10, 35, 70 };
    const f32 speeds[] = { -128, -120, -80, -40, 0, 8, 40, 80, 120, 128 };
    for (s32 slope = 0; slope < 7; slope++) {
        for (s32 speed = 0; speed < 10; speed++) {
            reset();
            mario.pos[1] = 0;
            mario.forwardVel = speeds[speed];
            planeSlope = tanf(grades[slope] * 0.01745329252f);
            struct MarioState before = mario;
            s16 pitch = tilt_body_running(&mario);
            CHECK(abs(pitch) <= DEGREES(60));
            CHECK(coss(pitch) >= 0.49f);
            f32 signedGrade = grades[slope] * speeds[speed];
            if (signedGrade != 0) {
                CHECK((pitch < 0) == (signedGrade > 0));
            } else {
                EXPECT_EQ(pitch, 0);
            }
            // Ordinary lean stays unchanged below the limit, including slow
            // floor-query sampling and negative carried speed.
            f32 nativePitch = -find_floor_slope(&mario, 0) * mario.forwardVel / 40.0f;
            if (absf(nativePitch) <= DEGREES(60)) {
                EXPECT_EQ(pitch, (s16) nativePitch);
            }
            EXPECT_EQ(memcmp(&before, &mario, sizeof(mario)), 0);
        }
    }
    return 1;
}

static int box_reverse(void) {
    for (s32 cycle = 0; cycle < 3; cycle++) {
        for (s32 bounce = 0; bounce < 3; bounce++) {
            reset();
            mario.actionArg = bounce;
            mario.input = INPUT_NONZERO_ANALOG;
            mario.intendedMag = 32.0f;
            mario.intendedYaw = (s16) 0x8000;
            s32 backwards = FALSE;
            for (s32 frame = 0; frame < 32; frame++) {
                EXPECT_EQ(act_crazy_box_bounce(&mario), FALSE);
                CHECK(abs(object.header.gfx.angle[0]) <= 0x4000);
                EXPECT_EQ(mario.faceAngle[0], 0);
                EXPECT_EQ(mario.faceAngle[1], 0);
                EXPECT_EQ(mario.heldObj, &box);
                EXPECT_EQ(mario.action, ACT_CRAZY_BOX_BOUNCE);
                EXPECT_EQ(selectedAnimation, MARIO_ANIM_DIVE);
                if (mario.forwardVel < 0) {
                    backwards = TRUE;
                    CHECK(mario.vel[2] < 0);
                }
            }
            CHECK(backwards);
            EXPECT_EQ(rumbleCount, 0);
        }
    }
    return 1;
}

static int box_forward_pose(void) {
    const f32 speeds[] = { 0, 8, 32, 48, 128 };
    const f32 vertical[] = { -92, -4, 0, 4, 100 };
    for (s32 speed = 0; speed < 5; speed++) {
        for (s32 height = 0; height < 5; height++) {
            reset();
            mario.actionTimer = 1;
            mario.forwardVel = speeds[speed];
            mario.vel[1] = vertical[height];
            EXPECT_EQ(act_crazy_box_bounce(&mario), FALSE);
            CHECK(mario.forwardVel >= 0);
            EXPECT_EQ(object.header.gfx.angle[0], atan2s(mario.forwardVel, -mario.vel[1]));
            EXPECT_EQ(mario.actionTimer, 1);
            EXPECT_EQ(mario.particleFlags, 0);
        }
    }
    return 1;
}

static int box_contacts(void) {
    for (s32 bounce = 0; bounce < 3; bounce++) {
        reset();
        mario.actionArg = bounce;
        mario.actionTimer = 1;
        mario.forwardVel = -8;
        mario.vel[1] = -10;
        airResult = AIR_STEP_LANDED;
        EXPECT_EQ(act_crazy_box_bounce(&mario), FALSE);
        EXPECT_EQ(mario.action, bounce < 2 ? ACT_CRAZY_BOX_BOUNCE : ACT_STOMACH_SLIDE);
        EXPECT_EQ(mario.actionArg, bounce < 2 ? bounce + 1 : 0);
        EXPECT_EQ(mario.heldObj, bounce < 2 ? &box : NULL);
        EXPECT_EQ(box.oInteractStatus, bounce < 2 ? 0 : INT_STATUS_STOP_RIDING);
        EXPECT_EQ(rumbleCount, 1);
        EXPECT_EQ(mario.particleFlags, PARTICLE_MIST_CIRCLE);
        CHECK(mario.forwardVel < 0);
    }
    reset();
    mario.actionTimer = 1;
    mario.forwardVel = -8;
    mario.wall = &testFloor;
    airResult = AIR_STEP_HIT_WALL;
    act_crazy_box_bounce(&mario);
    EXPECT_EQ(mario.faceAngle[1], (s16) 0x8000);
    EXPECT_EQ(mario.heldObj, &box);
    EXPECT_EQ(bonkCount, 1);
    CHECK(mario.forwardVel < 0);
    reset();
    mario.actionTimer = 1;
    airResult = AIR_STEP_HIT_LAVA_WALL;
    act_crazy_box_bounce(&mario);
    EXPECT_EQ(lavaCount, 1);
    EXPECT_EQ(mario.action, ACT_LAVA_BOOST);
    return 1;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    if (test_selected("rotation.running_lean")) {
        test_report("rotation.running_lean", running_lean());
    }
    if (test_selected("rotation.box_reverse")) {
        test_report("rotation.box_reverse", box_reverse());
    }
    if (test_selected("rotation.box_forward_pose")) {
        test_report("rotation.box_forward_pose", box_forward_pose());
    }
    if (test_selected("rotation.box_contacts")) {
        test_report("rotation.box_contacts", box_contacts());
    }
    return test_finish();
}
