/* Execute production air steering and drag over repeated updates. */
#include <math.h>
#include <stdint.h>
#include "test_support.h"

typedef int16_t s16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;

#define FALSE 0
#define TRUE 1
#define SLOPE_BUFFER
#define INPUT_A_PRESSED 4
#define INPUT_A_DOWN 8
#define ACT_BUTT_SLIDE 11
#define ACT_HOLD_BUTT_SLIDE 12
#define ACT_CROUCH_SLIDE 13
#define ACT_JUMP_LAND 14
#define ACT_QUICKSAND_JUMP_LAND 15
#define ACT_HOLD_QUICKSAND_JUMP_LAND 16
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define sqr(x) ((x) * (x))
#define absf(x) fabsf(x)
#define INPUT_NONZERO_ANALOG 1
#define INPUT_B_PRESSED 2
#define ACT_JUMP 1
#define ACT_LONG_JUMP 2
#define ACT_JUMP_KICK 3
#define ACT_HOLD_JUMP 9
#define ACT_STEEP_JUMP 10
#define ACT_DIVE 4
#define ACT_SLIDE_KICK 5
#define ACT_WALL_KICK_AIR 6
#define ACT_PUTTING_ON_CAP_AIR 7
#define ACT_SIDE_FLIP 8
#define ACT_ARG_CAP_PICKUP_CAPLESS 1
#define GO_NO_SPEED_LIMIT 1

#include "air_backward_definitions.inc"

struct Surface {
    struct {
        f32 x, y, z;
    } normal;
};
struct MarioState {
    s32 framesSinceA;
    s32 backwardSlopeSlideCount, backwardButtSlideJumpCount;
    f32 slopeJumpKickImpactSpeed;
    u32 action, prevAction, input, actionArg, actionState, actionTimer;
    f32 quicksandDepth;
    void *heldObj;
    s16 faceAngle[3], intendedYaw;
    f32 intendedMag, forwardVel, slideVelX, slideVelZ, vel[3];
    s32 wallKickChainCount;
    struct Surface *floor;
};

static f32 sins(s16 angle) {
    return sinf(angle * (6.28318530718f / 65536.0f));
}
static f32 coss(s16 angle) {
    return cosf(angle * (6.28318530718f / 65536.0f));
}
/* These fixtures cover ordinary air controls; Wing twirl has geometry coverage. */
static f32 mario_air_twirl_control_scale(struct MarioState *m) {
    return 1.0f;
}

static void mario_air_twirl_limit_control_speed(struct MarioState *m) {
}

/* Ordinary-control cases leave gameplay cheats disabled. */
static s32 game_options_cheat_active(s32 option) {
    return FALSE;
}

static s32 check_horizontal_wind(struct MarioState *m) {
    return 0;
}
static s32 mario_floor_is_slope(struct MarioState *m) {
    return m->floor->normal.y < 0.95f;
}
static s32 sSteep;
static s32 sDiveInput;
static s32 mario_has_dive_input(struct MarioState *m) {
    return sDiveInput;
}
static s32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
    return 1;
}

static s32 mario_floor_is_steep(struct MarioState *m) {
    return sSteep;
}
static void set_steep_jump_action(struct MarioState *m) {
    set_mario_action(m, ACT_STEEP_JUMP, 0);
}
static s32 check_crouch_slide_ledge_grab(struct MarioState *m) {
    return 0;
}
static s32 update_sliding(struct MarioState *m, f32 speed) {
    return 0;
}
static void common_slide_action(struct MarioState *m, u32 stop, u32 air, s32 anim) {
}
#include "air_backward_sources.inc"

#define RUN_CASE(name, fn)                                                                             \
    do {                                                                                               \
        if (test_selected(name))                                                                       \
            test_report(name, fn());                                                                   \
    } while (0)

static struct MarioState m;
static struct Surface slideFloor;

static void begin(u32 action, f32 speed, f32 stickMagnitude) {
    memset(&m, 0, sizeof(m));
    sSteep = 0;
    m.action = action;
    m.forwardVel = speed;
    m.intendedYaw = 0x8000;
    m.intendedMag = stickMagnitude;
    m.input = stickMagnitude > 0.0f ? INPUT_NONZERO_ANALOG : 0;
}

static int held_back_eases_into_thirty_two(void) {
    for (int withTurn = 0; withTurn < 2; withTurn++) {
        begin(ACT_JUMP, -15.0f, 32.0f);
        for (int frame = 0; frame < 200; frame++) {
            f32 prior = m.forwardVel;
            if (withTurn) {
                update_air_with_turn(&m);
            } else {
                update_air_without_turn(&m);
            }
            EXPECT_EQ(m.forwardVel <= prior, 1);
            EXPECT_EQ(m.forwardVel >= -32.001f, 1);
        }
        EXPECT_EQ(m.forwardVel < -31.9f, 1);
        m.input = 0;
        if (withTurn) {
            update_air_with_turn(&m);
        } else {
            update_air_without_turn(&m);
        }
        EXPECT_EQ(m.forwardVel > -32.0f, 1);
        m.input = INPUT_NONZERO_ANALOG;
        for (int frame = 0; frame < 200; frame++) {
            if (withTurn) {
                update_air_with_turn(&m);
            } else {
                update_air_without_turn(&m);
            }
            EXPECT_EQ(m.forwardVel >= -32.001f, 1);
        }
        EXPECT_EQ(m.forwardVel < -31.9f, 1);
        begin(ACT_JUMP, -20.0f, 32.0f);
        if (withTurn) {
            update_air_with_turn(&m);
        } else {
            update_air_without_turn(&m);
        }
        EXPECT_EQ(fabsf(m.forwardVel - (-20.5f)) < 0.01f, 1);
        begin(ACT_JUMP, -25.0f, 32.0f);
        if (withTurn) {
            update_air_with_turn(&m);
        } else {
            update_air_without_turn(&m);
        }
        EXPECT_EQ(fabsf(m.forwardVel - (-25.291667f)) < 0.01f, 1);
        begin(ACT_JUMP, -32.0f, 32.0f);
        if (withTurn) {
            update_air_with_turn(&m);
        } else {
            update_air_without_turn(&m);
        }
        EXPECT_EQ(fabsf(m.forwardVel - (-32.0f)) < 0.01f, 1);
    }
    return 1;
}

static int released_stick_restores_drag(void) {
    for (int withTurn = 0; withTurn < 2; withTurn++) {
        begin(ACT_JUMP, -18.0f, 32.0f);
        if (withTurn) {
            update_air_with_turn(&m);
        } else {
            update_air_without_turn(&m);
        }
        m.input = 0;
        if (withTurn) {
            update_air_with_turn(&m);
        } else {
            update_air_without_turn(&m);
        }
        EXPECT_EQ(fabsf(m.forwardVel - (-16.8f)) < 0.01f, 1);
        m.forwardVel = -25.0f;
        if (withTurn) {
            update_air_with_turn(&m);
        } else {
            update_air_without_turn(&m);
        }
        EXPECT_EQ(fabsf(m.forwardVel - (-21.1f)) < 0.01f, 1);
    }
    return 1;
}

static int analog_strength_scales_drag(void) {
    EXPECT_EQ(fabsf(backward_air_extra_drag(-18.0f, -18.0f, 0.0f, 1.0f) - 2.0f) < 0.001f, 1);
    EXPECT_EQ(fabsf(backward_air_extra_drag(-18.0f, -18.0f, 0.5f, 1.0f) - 1.0f) < 0.001f, 1);
    EXPECT_EQ(fabsf(backward_air_extra_drag(-25.0f, -25.0f, 1.0f, 1.0f)) < 0.001f, 1);
    EXPECT_EQ(fabsf(backward_air_extra_drag(-35.0f, -35.0f, 0.0f, 1.0f) - 6.0f) < 0.001f, 1);
    return 1;
}

static int carried_speed_slows_without_snap(void) {
    for (int withTurn = 0; withTurn < 2; withTurn++) {
        begin(ACT_LONG_JUMP, -40.0f, 32.0f);
        if (withTurn) {
            update_air_with_turn(&m);
        } else {
            update_air_without_turn(&m);
        }
        EXPECT_EQ(m.forwardVel > -40.0f && m.forwardVel < -32.0f, 1);
        for (int frame = 0; frame < 10; frame++) {
            if (withTurn) {
                update_air_with_turn(&m);
            } else {
                update_air_without_turn(&m);
            }
        }
        EXPECT_EQ(m.forwardVel >= -32.001f, 1);
    }
    return 1;
}

static int backward_butt_slide_jump_has_extra_drag(void) {
    begin(ACT_JUMP, -12.0f, 0.0f);
    update_air_without_turn(&m);
    f32 ordinary = m.forwardVel;
    begin(ACT_JUMP, -12.0f, 0.0f);
    m.actionArg = ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE;
    update_air_without_turn(&m);
    EXPECT_EQ(fabsf(m.forwardVel - (ordinary + 2.0f)) < 0.001f, 1);
    begin(ACT_JUMP_KICK, -12.0f, 0.0f);
    update_air_without_turn(&m);
    ordinary = m.forwardVel;
    begin(ACT_JUMP_KICK, -12.0f, 0.0f);
    m.actionArg = ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE;
    update_air_without_turn(&m);
    EXPECT_EQ(fabsf(m.forwardVel - (ordinary + 2.0f)) < 0.001f, 1);
    begin(ACT_JUMP, 12.0f, 0.0f);
    m.actionArg = ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE;
    update_air_without_turn(&m);
    EXPECT_EQ(fabsf(m.forwardVel - 11.65f) < 0.001f, 1);
    return 1;
}

static int backward_butt_slide_marker_survives_jump_kicks(void) {
    begin(ACT_JUMP, -12.0f, 0.0f);
    m.actionArg = ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE | ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL;
    m.input = INPUT_B_PRESSED;
    sDiveInput = 0;
    EXPECT_EQ(check_kick_or_dive_in_air(&m), 1);
    EXPECT_EQ(m.action, ACT_JUMP_KICK);
    EXPECT_EQ(m.actionArg,
              ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE | ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL);
    m.input = INPUT_B_PRESSED;
    EXPECT_EQ(check_kick_or_dive_in_air(&m), 1);
    EXPECT_EQ(m.actionArg,
              ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE | ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL);
    sDiveInput = 1;
    m.input = INPUT_B_PRESSED;
    EXPECT_EQ(check_kick_or_dive_in_air(&m), 1);
    EXPECT_EQ(m.action, ACT_DIVE);
    EXPECT_EQ(m.actionArg, 0);
    begin(ACT_JUMP, -12.0f, 0.0f);
    m.input = INPUT_B_PRESSED;
    sDiveInput = 0;
    EXPECT_EQ(check_kick_or_dive_in_air(&m), 1);
    EXPECT_EQ(m.actionArg, 0);
    return 1;
}

static int unmarked_backward_jump_kick(void) {
    for (int marked = 0; marked < 2; marked++) {
        begin(ACT_JUMP_KICK, -24, 32);
        slideFloor.normal.x = 0;
        slideFloor.normal.y = 0.6f;
        slideFloor.normal.z = -0.8f;
        m.floor = &slideFloor;
        m.faceAngle[1] = (s16) 0x8000;
        m.intendedYaw = 0;
        m.actionArg = marked ? ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL : 0;
        struct MarioState control = m;
        control.floor = NULL;
        update_air_without_turn(&control);
        update_air_without_turn(&m);
        EXPECT_EQ(fabsf(m.vel[2] - control.vel[2]) < 0.001f, 1);
        EXPECT_EQ(m.vel[2] > 20, 1);
    }
    begin(ACT_JUMP_KICK, -64, 0);
    slideFloor.normal.x = 0;
    slideFloor.normal.y = 0.6f;
    slideFloor.normal.z = -0.8f;
    m.floor = &slideFloor;
    m.faceAngle[1] = (s16) 0x8000;
    m.vel[0] = 7;
    m.vel[2] = 64;
    for (int contact = 1; contact <= 5; contact++) {
        mario_restrict_air_uphill(&m, m.floor);
        EXPECT_EQ(fabsf(m.vel[2] - (contact < 4 ? 64 - 20 * contact : 0)) < 0.001f, 1);
        EXPECT_EQ(fabsf(m.vel[0] - 7) < 0.001f, 1);
        EXPECT_EQ(!!(m.actionArg & ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL), 1);
        EXPECT_EQ(!!(m.actionArg & ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE), 0);
    }
    m.input = INPUT_B_PRESSED;
    sDiveInput = FALSE;
    check_kick_or_dive_in_air(&m);
    EXPECT_EQ(!!(m.actionArg & ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL), 1);
    return 1;
}

static int uphill_slide_jump_cannot_gain_ground(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_JUMP, -24.0f, 32.0f);
        memset(&slideFloor, 0, sizeof(slideFloor));
        slideFloor.normal.y = 0.8f;
        slideFloor.normal.z = -0.6f;
        m.floor = &slideFloor;
        m.faceAngle[1] = (s16) 0x8000; // Facing downhill, moving backward uphill.
        m.intendedYaw = 0;
        m.actionArg = ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE | ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL;
        f32 uphillDistance = 0.0f;
        for (int frame = 0; frame < 12; frame++) {
            if (frame == 4) {
                m.action = ACT_JUMP_KICK;
                m.actionArg = ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE
                              | ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL;
            }
            update_air_without_turn(&m);
            if (m.action == ACT_JUMP_KICK) {
                mario_restrict_air_uphill(&m, m.floor);
            }
            uphillDistance += m.vel[2];
            EXPECT_EQ(m.vel[2] <= 0.001f, 1);
            EXPECT_EQ(m.slideVelZ <= 0.001f, 1);
        }
        EXPECT_EQ(uphillDistance <= 0.001f, 1);
    }
    // The same marked jump can still steer downhill, and ordinary jumps stay free.
    begin(ACT_JUMP, 12.0f, 0.0f);
    m.floor = &slideFloor;
    m.faceAngle[1] = (s16) 0x8000;
    m.actionArg = ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE | ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL;
    update_air_without_turn(&m);
    EXPECT_EQ(m.vel[2] < 0.0f, 1);
    begin(ACT_JUMP, -12.0f, 0.0f);
    m.floor = &slideFloor;
    m.faceAngle[1] = (s16) 0x8000;
    update_air_without_turn(&m);
    EXPECT_EQ(m.vel[2] > 0.0f, 1);
    // A downhill start can reverse with held backward input; the slope still blocks ascent.
    begin(ACT_JUMP, 12.0f, 32.0f);
    m.floor = &slideFloor;
    m.faceAngle[1] = (s16) 0x8000;
    m.intendedYaw = 0;
    m.actionArg = ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE | ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL;
    for (int frame = 0; frame < 40; frame++) {
        update_air_without_turn(&m);
        EXPECT_EQ(m.vel[2] <= 0.001f, 1);
    }
    begin(ACT_HOLD_JUMP, -24.0f, 32.0f);
    m.floor = &slideFloor;
    m.faceAngle[1] = (s16) 0x8000;
    m.intendedYaw = 0;
    m.actionArg = ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE | ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL;
    for (int frame = 0; frame < 12; frame++) {
        update_air_without_turn(&m);
        EXPECT_EQ(m.vel[2] <= 0.001f, 1);
    }
    begin(ACT_STEEP_JUMP, -24.0f, 0.0f);
    m.floor = &slideFloor;
    m.faceAngle[1] = (s16) 0x8000;
    m.actionArg = ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL;
    m.vel[2] = 24.0f;
    backward_butt_slide_jump_restrict_uphill(&m);
    EXPECT_EQ(m.vel[2] <= 0.001f, 1);
    return 1;
}

static int slope_rejump_delay(void) {
    for (int held = 0; held < 2; held++) {
        for (int age = 0; age <= 6; age++) {
            begin(ACT_BUTT_SLIDE, -24, 0);
            m.actionState = 1;
            m.actionTimer = 9;
            m.framesSinceA = age;
            m.input = held ? INPUT_A_DOWN : 0;
            EXPECT_EQ(common_slide_action_with_jump(&m, ACT_JUMP_LAND, ACT_JUMP, ACT_JUMP, 0, 4),
                      age < 6);
            EXPECT_EQ(m.action, age < 6 ? ACT_JUMP : ACT_BUTT_SLIDE);
        }
    }
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int held = 0; held < 2; held++) {
            for (int slope = 0; slope < 2; slope++) {
                for (int release = 0; release < 2; release++) {
                    u32 slide = held ? ACT_HOLD_BUTT_SLIDE : ACT_BUTT_SLIDE;
                    u32 jump = held ? ACT_HOLD_JUMP : ACT_JUMP;
                    begin(slide, -24, 0);
                    slideFloor.normal.y = slope ? 0.8f : 1.0f;
                    m.floor = &slideFloor;
                    int delay = 3;
                    for (int frame = 0; frame < delay; frame++) {
                        m.input = frame == 0 ? INPUT_A_PRESSED | INPUT_A_DOWN
                                  : release  ? 0
                                             : INPUT_A_DOWN;
                        EXPECT_EQ(
                            common_slide_action_with_jump(&m, ACT_JUMP_LAND, jump, ACT_JUMP, 0, 4),
                            FALSE);
                        EXPECT_EQ(m.action, slide);
                    }
                    m.input = release ? 0 : INPUT_A_DOWN;
                    EXPECT_EQ(common_slide_action_with_jump(&m, ACT_JUMP_LAND, jump, ACT_JUMP, 0, 4),
                              TRUE);
                    EXPECT_EQ(m.action, jump);
                    EXPECT_EQ(m.backwardButtSlideJumpCount, 1);
                    if (release) {
                        m.action = slide;
                        m.actionState = 0;
                        m.actionTimer = 9;
                        // Waiting longer, including sliding onto flat ground, cannot lock the gate.
                        slideFloor.normal.y = 1;
                        m.input = INPUT_A_PRESSED | INPUT_A_DOWN;
                        m.backwardSlopeSlideCount = 1;
                        EXPECT_EQ(
                            common_slide_action_with_jump(&m, ACT_JUMP_LAND, jump, ACT_JUMP, 0, 4),
                            TRUE);
                        EXPECT_EQ(m.backwardSlopeSlideCount, 0);
                    }
                }
            }
        }
    }
    for (int held = 0; held < 2; held++) {
        for (int slope = 0; slope < 2; slope++) {
            u32 slide = held ? ACT_HOLD_BUTT_SLIDE : ACT_BUTT_SLIDE;
            u32 jump = held ? ACT_HOLD_JUMP : ACT_JUMP;
            begin(slide, -24, 0);
            slideFloor.normal.y = slope ? 0.8f : 1.0f;
            m.floor = &slideFloor;
            for (int attempt = 0; attempt < 4; attempt++) {
                int delay;
                if (attempt == 0) {
                    delay = 3;
                } else if (attempt == 1) {
                    delay = 6;
                } else {
                    delay = 9;
                }
                m.action = slide;
                m.actionState = m.actionTimer = 0;
                m.forwardVel = -24;
                for (int frame = 0; frame < delay; frame++) {
                    m.input = frame == 0 ? INPUT_A_PRESSED | INPUT_A_DOWN : INPUT_A_DOWN;
                    EXPECT_EQ(common_slide_action_with_jump(&m, ACT_JUMP_LAND, jump, ACT_JUMP, 0, 4),
                              FALSE);
                }
                m.input = INPUT_A_DOWN;
                EXPECT_EQ(common_slide_action_with_jump(&m, ACT_JUMP_LAND, jump, ACT_JUMP, 0, 4), TRUE);
                EXPECT_EQ(m.action, jump);
                EXPECT_EQ(m.backwardButtSlideJumpCount, attempt < 2 ? attempt + 1 : 3);
            }
            // Forward butt-slide jumps retain their previous flat/slope delay.
            m.action = slide;
            m.actionState = m.actionTimer = 0;
            m.forwardVel = 24;
            for (int frame = 0; frame < (slope ? 9 : 6); frame++) {
                m.input = frame == 0 ? INPUT_A_PRESSED | INPUT_A_DOWN : INPUT_A_DOWN;
                EXPECT_EQ(common_slide_action_with_jump(&m, ACT_JUMP_LAND, jump, ACT_JUMP, 0, 4),
                          FALSE);
            }
            m.input = INPUT_A_DOWN;
            EXPECT_EQ(common_slide_action_with_jump(&m, ACT_JUMP_LAND, jump, ACT_JUMP, 0, 4), TRUE);
            EXPECT_EQ(m.backwardButtSlideJumpCount, 3);
        }
    }
    return TRUE;
}

static int butt_slide_takeoff_drag_sequence(void) {
    const float speeds[] = { -1.0f, -12.0f, -24.0f, -40.0f };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int held = 0; held < 2; held++) {
            for (int steep = 0; steep < 2; steep++) {
                for (int speed = 0; speed < 4; speed++) {
                    begin(held ? ACT_HOLD_BUTT_SLIDE : ACT_BUTT_SLIDE, speeds[speed], 32.0f);
                    slideFloor.normal.x = 0;
                    slideFloor.normal.y = steep ? 0.8f : 1.0f;
                    slideFloor.normal.z = steep ? 0.6f : 0;
                    m.floor = &slideFloor;
                    sSteep = steep;
                    m.actionTimer = steep ? 9 : 5;
                    m.input |= INPUT_A_PRESSED | INPUT_A_DOWN;
                    EXPECT_EQ(common_slide_action_with_jump(&m, ACT_JUMP_LAND,
                                                            held ? ACT_HOLD_JUMP : ACT_JUMP, ACT_JUMP,
                                                            0, 4.0f),
                              TRUE);
                    u32 expectedAction;
                    if (steep) {
                        expectedAction = ACT_STEEP_JUMP;
                    } else {
                        expectedAction = held ? ACT_HOLD_JUMP : ACT_JUMP;
                    }
                    EXPECT_EQ(m.action, expectedAction);
                    if (steep) {
                        EXPECT_EQ(m.actionArg, ACT_ARG_STEEP_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL);
                    } else {
                        EXPECT_EQ(m.actionArg, ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE);
                    }
                    // Exercise the real air attack transition, including repeat and dive exit.
                    if (!held || steep) {
                        m.input = INPUT_B_PRESSED;
                        sDiveInput = 0;
                        EXPECT_EQ(check_kick_or_dive_in_air(&m), TRUE);
                        EXPECT_EQ(m.action, ACT_JUMP_KICK);
                    }
                    m.floor = NULL; // Continue beyond the launch slope, where its uphill guard no
                                    // longer applies.
                    for (int frame = 0; frame < 100; frame++) {
                        if ((!held || steep) && (frame == 5 || frame == 15)) {
                            m.input = INPUT_B_PRESSED;
                            check_kick_or_dive_in_air(&m);
                        }
                        m.input = INPUT_NONZERO_ANALOG;
                        float before = m.forwardVel;
                        update_air_without_turn(&m);
                        EXPECT_EQ(m.forwardVel >= before, TRUE);
                        EXPECT_EQ(m.forwardVel <= 0, TRUE);
                        EXPECT_EQ(fabsf(m.vel[2] - m.forwardVel) < 0.001f, TRUE);
                    }
                    EXPECT_EQ(m.forwardVel, 0);
                    if (!held || steep) {
                        m.input = INPUT_B_PRESSED;
                        sDiveInput = 1;
                        check_kick_or_dive_in_air(&m);
                        EXPECT_EQ(m.action, ACT_DIVE);
                        EXPECT_EQ(m.actionArg, 0);
                    }
                }
            }
        }
    }
    // Each air steering path must settle while backward input remains held.
    for (int withTurn = 0; withTurn < 2; withTurn++) {
        for (int kick = 0; kick < 2; kick++) {
            begin(kick ? ACT_JUMP_KICK : ACT_JUMP, -12.0f, 32.0f);
            m.actionArg =
                kick ? ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE : ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE;
            for (int frame = 0; frame < 50; frame++) {
                if (withTurn) {
                    update_air_with_turn(&m);
                } else {
                    update_air_without_turn(&m);
                }
            }
            EXPECT_EQ(m.forwardVel, 0);
        }
    }
    return TRUE;
}

static int high_forward_speed_drag(void) {
    const f32 speeds[] = { 63.0f, 64.35f, 64.36f, 80.0f };
    const f32 expected[] = { 61.65f, 63.0f, 62.51f, 78.15f };
    for (int withTurn = 0; withTurn < 2; withTurn++) {
        for (unsigned i = 0; i < sizeof(speeds) / sizeof(speeds[0]); i++) {
            begin(ACT_JUMP, speeds[i], 0.0f);
            if (withTurn) {
                update_air_with_turn(&m);
            } else {
                update_air_without_turn(&m);
            }
            EXPECT_EQ(fabsf(m.forwardVel - expected[i]) < 0.001f, TRUE);
            EXPECT_EQ(fabsf(m.vel[2] - expected[i]) < 0.001f, TRUE);
        }
        begin(ACT_JUMP, 80.0f, 32.0f);
        m.intendedYaw = 0;
        if (withTurn) {
            update_air_with_turn(&m);
        } else {
            update_air_without_turn(&m);
        }
        EXPECT_EQ(fabsf(m.forwardVel - 79.65f) < 0.001f, TRUE);
    }
    begin(ACT_LONG_JUMP, 80.0f, 0.0f);
    update_air_without_turn(&m);
    EXPECT_EQ(fabsf(m.forwardVel - 78.15f) < 0.001f, TRUE);
    begin(ACT_JUMP_KICK, 80.0f, 0.0f);
    update_air_without_turn(&m);
    EXPECT_EQ(fabsf(m.forwardVel - 78.20833f) < 0.001f, TRUE);
    begin(ACT_WALL_KICK_AIR, 80.0f, 0.0f);
    update_air_without_turn(&m);
    EXPECT_EQ(fabsf(m.forwardVel - 76.45f) < 0.001f, TRUE);
    return TRUE;
}

static int jump_kick_keeps_retention(void) {
    begin(ACT_JUMP_KICK, -25.0f, 0.0f);
    update_air_without_turn(&m);
    EXPECT_EQ(fabsf(m.forwardVel - (-21.733797f)) < 0.01f, 1);
    return 1;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    RUN_CASE("air_backward.high_forward_speed", high_forward_speed_drag);
    RUN_CASE("air_backward.slope_rejump_delay", slope_rejump_delay);
    RUN_CASE("air_backward.butt_slide_takeoff_sequence", butt_slide_takeoff_drag_sequence);
    RUN_CASE("air_backward.held_back", held_back_eases_into_thirty_two);
    RUN_CASE("air_backward.release", released_stick_restores_drag);
    RUN_CASE("air_backward.analog", analog_strength_scales_drag);
    RUN_CASE("air_backward.jump_kick", jump_kick_keeps_retention);
    RUN_CASE("air_backward.carried_speed", carried_speed_slows_without_snap);
    RUN_CASE("air_backward.butt_slide_jump", backward_butt_slide_jump_has_extra_drag);
    RUN_CASE("air_backward.butt_slide_jump_kick", backward_butt_slide_marker_survives_jump_kicks);
    RUN_CASE("air_backward.unmarked_jump_kick", unmarked_backward_jump_kick);
    RUN_CASE("air_backward.uphill_slide_jump", uphill_slide_jump_cannot_gain_ground);
    return test_finish();
}
