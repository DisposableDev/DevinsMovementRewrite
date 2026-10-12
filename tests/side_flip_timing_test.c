/* Production turnaround and punch-recovery gates with controlled movement/animation. */
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "test_support.h"

typedef int16_t s16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;

#define TRUE 1
#define FALSE 0
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define COS73 0.29237169f
#define INPUT_ABOVE_SLIDE (1 << 0)
#define INPUT_A_PRESSED (1 << 1)
#define INPUT_IDLE (1 << 2)
#define INPUT_NONZERO_ANALOG (1 << 3)
#define INPUT_OFF_FLOOR (1 << 4)
#define INPUT_FIRST_PERSON (1 << 5)
#define INPUT_B_PRESSED (1 << 6)
#define INPUT_Z_DOWN (1 << 7)
#define PARTICLE_DUST 1
#define GROUND_STEP_LEFT_GROUND 1
#define GROUND_STEP_NONE 0
#define SOUND_MOVING_TERRAIN_SLIDE 1
#define MARIO_ANIM_TURNING_PART1 1
#define MARIO_ANIM_TURNING_PART2 2
#define ACT_TURNING_AROUND 1
#define ACT_FINISH_TURNING_AROUND 2
#define ACT_SIDE_FLIP 3
#define ACT_JUMP 4
#define ACT_WALKING 5
#define ACT_BEGIN_SLIDING 6
#define ACT_BRAKING 7
#define ACT_FREEFALL 8
#define ACT_MOVE_PUNCHING 9
#define ACT_PUNCHING 10
#define ACT_CROUCH_SLIDE 11
#define ACT_SLIDE_KICK 12
#define ACT_DIVE 13
#define ACT_JUMP_KICK 14
#define ACT_SIDE_FLIP_LAND 15
#define ACT_STEEP_JUMP 16
#define ACT_HOLD_JUMP 17
#define ACT_GROUND_POUND 18
#define INPUT_Z_PRESSED (1 << 8)
#define INPUT_A_DOWN (1 << 9)
#define ACT_BACKFLIP 19
#define AIR_STEP_CHECK_LEDGE_GRAB 1
#define AIR_STEP_GRABBED_LEDGE 2
#define MARIO_ANIM_SLIDEFLIP 3
#define SOUND_ACTION_TERRAIN_JUMP 2
#define SOUND_ACTION_SIDE_FLIP_UNK 3

#include "sideflip_definitions.inc"

struct Animation {
    s16 loopEnd;
};
struct AnimInfo {
    s16 animID, animFrame;
    struct Animation *curAnim;
};
struct Object {
    struct {
        struct {
            struct AnimInfo animInfo;
            s16 angle[3];
            f32 cameraToObject[3];
        } gfx;
    } header;
};
struct Surface {
    struct {
        f32 y;
    } normal;
};
struct Controller {
    f32 stickMag;
};
struct MarioState {
    u32 action, prevAction, actionArg, actionState, actionTimer, input, particleFlags;
    s16 faceAngle[3], intendedYaw;
    f32 forwardVel, vel[3];
    s32 zBPressed;
    struct Controller *controller;
    s32 terrainSoundAddend;
    struct Object *marioObj, *heldObj;
    struct Surface *floor;
};

static s32 sSlopeStop, sGroundStep, sAnimEnded;
static s32 abs_angle_diff(s16 a, s16 b) {
    s32 difference = (s16) (a - b);
    return difference < 0 ? -difference : difference;
}
static s32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
    m->actionState = 0;
    m->actionTimer = 0;
    return TRUE;
}
static s32 set_jumping_action(struct MarioState *m, u32 action, u32 arg) {
    return set_mario_action(m, action, arg);
}
static void mario_set_forward_vel(struct MarioState *m, f32 speed) {
    m->forwardVel = speed;
}
static void play_sound(u32 sound, f32 *position) {
}
static void adjust_sound_for_speed(struct MarioState *m) {
}
static s32 perform_ground_step(struct MarioState *m) {
    return sGroundStep;
}
static s16 set_mario_animation(struct MarioState *m, s32 anim) {
    m->marioObj->header.gfx.animInfo.animID = anim;
    return m->marioObj->header.gfx.animInfo.animFrame;
}
static s32 is_anim_at_end(struct MarioState *m) {
    return sAnimEnded;
}
static s32 apply_slope_decel(struct MarioState *m, f32 amount) {
    return sSlopeStop;
}
static void update_walking_speed(struct MarioState *m) {
}
static s32 check_common_idle_cancels(struct MarioState *m) {
    return FALSE;
}

static s32 sAirSteps, sAirLanded;
static f32 coss(s16 angle) {
    return cosf(angle * (6.28318530718f / 65536.0f));
}
static void play_mario_sound(struct MarioState *m, s32 sound, u32 flag) {
}
static s32 common_air_action_step(struct MarioState *m, u32 landAction, s32 anim, u32 flags) {
    sAirSteps++;
    set_mario_animation(m, anim);
    if (sAirLanded) {
        set_mario_action(m, landAction, 0);
    }
    return 0;
}

#include "sideflip_sources.inc"

#define RUN_CASE(name, fn)                                                                             \
    do {                                                                                               \
        if (test_selected(name))                                                                       \
            test_report(name, fn());                                                                   \
    } while (0)
static struct MarioState m;
static struct Controller controller;
static struct Object mario;
static struct Surface floorSurface;
static struct Animation turningAnimation;

static void begin(u32 action) {
    memset(&m, 0, sizeof(m));
    memset(&mario, 0, sizeof(mario));
    memset(&floorSurface, 0, sizeof(floorSurface));
    turningAnimation.loopEnd = 20;
    mario.header.gfx.animInfo.curAnim = &turningAnimation;
    mario.header.gfx.animInfo.animID = MARIO_ANIM_TURNING_PART1;
    floorSurface.normal.y = 1.0f;
    m.controller = &controller;
    controller.stickMag = 64;
    m.marioObj = &mario;
    m.floor = &floorSurface;
    m.action = action;
    m.input = INPUT_NONZERO_ANALOG;
    m.faceAngle[1] = 0;
    m.intendedYaw = 0x7fff;
    m.forwardVel = 20.0f;
    sSlopeStop = sGroundStep = sAnimEnded = 0;
    sAirSteps = sAirLanded = 0;
}

static int turn_frames(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int elapsed = 0; elapsed <= 2; elapsed++) {
            begin(ACT_TURNING_AROUND);
            for (int frame = 0; frame < elapsed; frame++) {
                EXPECT_EQ(act_turning_around(&m), FALSE);
            }
            EXPECT_EQ(m.actionTimer, 0);
            m.input |= INPUT_A_PRESSED;
            EXPECT_EQ(act_turning_around(&m), TRUE);
            EXPECT_EQ(m.action, ACT_SIDE_FLIP);
        }
    }
    begin(ACT_TURNING_AROUND);
    act_turning_around(&m);
    act_turning_around(&m);
    mario.header.gfx.animInfo.animID = MARIO_ANIM_TURNING_PART2;
    mario.header.gfx.animInfo.animFrame = 15;
    m.input |= INPUT_A_PRESSED;
    act_turning_around(&m);
    EXPECT_EQ(m.action, ACT_JUMP); // The existing late-animation cutoff still wins.
    return TRUE;
}

static int finish_phase(void) {
    begin(ACT_FINISH_TURNING_AROUND);
    m.input |= INPUT_A_PRESSED;
    act_finish_turning_around(&m);
    EXPECT_EQ(m.action, ACT_SIDE_FLIP);
    begin(ACT_FINISH_TURNING_AROUND);
    act_finish_turning_around(&m);
    m.input |= INPUT_A_PRESSED;
    act_finish_turning_around(&m);
    EXPECT_EQ(m.action, ACT_SIDE_FLIP);
    begin(ACT_FINISH_TURNING_AROUND);
    act_finish_turning_around(&m);
    act_finish_turning_around(&m);
    m.input |= INPUT_A_PRESSED;
    act_finish_turning_around(&m);
    EXPECT_EQ(m.action, ACT_SIDE_FLIP);

    begin(ACT_TURNING_AROUND);
    sSlopeStop = TRUE;
    EXPECT_EQ(act_turning_around(&m), TRUE);
    EXPECT_EQ(m.action, ACT_FINISH_TURNING_AROUND);
    EXPECT_EQ(m.actionArg, 0); // The finish dispatch shares this game update.
    sSlopeStop = FALSE;
    act_finish_turning_around(&m);
    EXPECT_EQ(m.actionTimer, 0);
    m.input |= INPUT_A_PRESSED;
    act_finish_turning_around(&m);
    EXPECT_EQ(m.action, ACT_SIDE_FLIP);

    begin(ACT_TURNING_AROUND);
    sSlopeStop = TRUE;
    act_turning_around(&m);
    sSlopeStop = FALSE;
    act_finish_turning_around(&m);
    act_finish_turning_around(&m);
    m.input |= INPUT_A_PRESSED;
    act_finish_turning_around(&m);
    EXPECT_EQ(m.action, ACT_SIDE_FLIP);
    return TRUE;
}

static int punch_entry(void) {
    begin(ACT_PUNCHING);
    mario.header.gfx.animInfo.animFrame = 15;
    m.input |= INPUT_A_PRESSED;
    EXPECT_EQ(cancel_punch_recovery(&m, 5), TRUE);
    EXPECT_EQ(m.action, ACT_JUMP);

    begin(ACT_PUNCHING);
    mario.header.gfx.animInfo.animFrame = 15;
    EXPECT_EQ(cancel_punch_recovery(&m, 5), TRUE);
    EXPECT_EQ(m.action, ACT_TURNING_AROUND);
    EXPECT_EQ(m.actionTimer, 0);
    m.input |= INPUT_A_PRESSED;
    act_turning_around(&m);
    EXPECT_EQ(m.action, ACT_SIDE_FLIP);
    return TRUE;
}

static int turn_controls(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int phase = 0; phase < 2; phase++) {
            for (int kind = 0; kind < 4; kind++) {
                for (int strong = 0; strong < 2; strong++) {
                    begin(phase ? ACT_FINISH_TURNING_AROUND : ACT_TURNING_AROUND);
                    controller.stickMag = strong ? 64 : 20;
                    m.input |= kind == 0   ? INPUT_B_PRESSED
                               : kind == 1 ? INPUT_Z_DOWN
                               : kind == 2 ? (INPUT_B_PRESSED | INPUT_Z_DOWN)
                                           : (INPUT_A_PRESSED | INPUT_Z_DOWN);
                    m.zBPressed = kind == 2;
                    EXPECT_EQ(phase ? act_finish_turning_around(&m) : act_turning_around(&m), TRUE);
                    EXPECT_EQ(m.action, kind == 0   ? ACT_MOVE_PUNCHING
                                        : kind == 1 ? ACT_CROUCH_SLIDE
                                        : kind == 2 ? ACT_MOVE_PUNCHING
                                                    : ACT_SIDE_FLIP);
                    EXPECT_EQ(m.forwardVel, 20);
                    EXPECT_EQ(m.faceAngle[1], 0);
                    EXPECT_EQ(mario.header.gfx.angle[1],
                              0); // No trailing turn-pose correction after cancellation.
                    if (kind == 2) {
                        EXPECT_EQ(m.actionArg, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
                    }
                }
            }
        }
    }
    for (int phase = 0; phase < 2; phase++) {
        begin(phase ? ACT_FINISH_TURNING_AROUND : ACT_TURNING_AROUND);
        m.input |= INPUT_ABOVE_SLIDE | INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_DOWN;
        m.zBPressed = TRUE;
        EXPECT_EQ(phase ? act_finish_turning_around(&m) : act_turning_around(&m), TRUE);
        EXPECT_EQ(m.action, ACT_BEGIN_SLIDING);
    }
    return TRUE;
}

static int turn_backflip(void) {
    const f32 magnitudes[] = { 0.0f, 20.0f, MARIO_STRONG_STICK_THRESHOLD - 0.1f,
                               MARIO_STRONG_STICK_THRESHOLD, 64.0f };
    for (int phase = 0; phase < 2; phase++) {
        for (int kind = 0; kind < 7; kind++) {
            for (int stick = 0; stick < 5; stick++) {
                begin(phase ? ACT_FINISH_TURNING_AROUND : ACT_TURNING_AROUND);
                controller.stickMag = magnitudes[stick];
                m.input = stick == 0 ? 0 : INPUT_NONZERO_ANALOG;
                m.input |= INPUT_A_DOWN | INPUT_Z_DOWN;
                if (kind == 1) {
                    m.input |= INPUT_A_PRESSED;
                }
                if (kind == 2) {
                    m.heldObj = &mario;
                }
                if (kind == 3) {
                    m.input &= ~INPUT_Z_DOWN;
                }
                if (kind == 4) {
                    m.input &= ~INPUT_A_DOWN;
                }
                if (kind == 5) {
                    m.input |= INPUT_ABOVE_SLIDE;
                }
                if (kind == 6) {
                    mario.header.gfx.animInfo.animID = MARIO_ANIM_TURNING_PART2;
                    mario.header.gfx.animInfo.animFrame = 15;
                    m.input |= INPUT_A_PRESSED;
                }
                s32 backflip = stick < 3 && kind < 2;
                EXPECT_EQ(phase ? act_finish_turning_around(&m) : act_turning_around(&m),
                          kind == 3 ? FALSE : TRUE);
                u32 expectedAction;
                if (backflip) {
                    expectedAction = ACT_BACKFLIP;
                } else if (kind == 1) {
                    expectedAction = ACT_SIDE_FLIP;
                } else if (kind == 5) {
                    expectedAction = ACT_BEGIN_SLIDING;
                } else if (kind == 6) {
                    expectedAction = ACT_JUMP;
                } else if (kind == 3) {
                    expectedAction = phase ? ACT_FINISH_TURNING_AROUND : ACT_TURNING_AROUND;
                } else {
                    expectedAction = ACT_CROUCH_SLIDE;
                }
                EXPECT_EQ(m.action, expectedAction);
                EXPECT_EQ(m.actionArg, 0);
                EXPECT_EQ(m.forwardVel, 20.0f);
                EXPECT_EQ(m.faceAngle[1], 0);
                if (kind != 3) {
                    EXPECT_EQ(mario.header.gfx.angle[1], 0);
                }
            }
        }
    }
    return TRUE;
}

static int kick_startup_lock(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_SIDE_FLIP);
        m.input = 0;
        for (int frame = 0; frame < 1; frame++) {
            EXPECT_EQ(act_side_flip(&m), FALSE);
            EXPECT_EQ(m.action, ACT_SIDE_FLIP);
            EXPECT_EQ(m.actionTimer, frame + 1);
            EXPECT_EQ(sAirSteps, frame + 1);
        }
        EXPECT_EQ(act_side_flip(&m), FALSE);
        EXPECT_EQ(m.action, ACT_SIDE_FLIP);
        EXPECT_EQ(m.actionTimer, 1);
        m.input = INPUT_B_PRESSED;
        EXPECT_EQ(act_side_flip(&m), TRUE);
        EXPECT_EQ(m.action, ACT_JUMP_KICK);
        EXPECT_EQ(m.actionTimer, 0);
    }
    begin(ACT_SIDE_FLIP);
    m.input = 0;
    act_side_flip(&m);
    m.input = INPUT_B_PRESSED;
    EXPECT_EQ(act_side_flip(&m), TRUE); // Update two is the exact boundary.
    EXPECT_EQ(m.action, ACT_JUMP_KICK);

    for (int frame = 0; frame < 1; frame++) {
        begin(ACT_SIDE_FLIP);
        m.actionTimer = frame;
        m.intendedYaw = m.faceAngle[1];
        m.input = INPUT_NONZERO_ANALOG | INPUT_B_PRESSED;
        EXPECT_EQ(act_side_flip(&m), TRUE);
        EXPECT_EQ(m.action, ACT_DIVE);
        EXPECT_EQ(sAirSteps, 0);

        begin(ACT_SIDE_FLIP);
        m.actionTimer = frame;
        m.input = INPUT_B_PRESSED | INPUT_Z_PRESSED;
        EXPECT_EQ(act_side_flip(&m), TRUE);
        EXPECT_EQ(m.action, ACT_GROUND_POUND);
        EXPECT_EQ(m.actionTimer, 0);

        begin(ACT_SIDE_FLIP);
        m.actionTimer = frame;
        m.input = INPUT_B_PRESSED;
        sAirLanded = TRUE;
        EXPECT_EQ(act_side_flip(&m), FALSE);
        EXPECT_EQ(m.action, ACT_SIDE_FLIP_LAND);
        EXPECT_EQ(m.actionTimer, 0);
        EXPECT_EQ(m.actionState, 0);
    }
    return TRUE;
}

static int kick_buffer(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int pressFrame = 0; pressFrame < 1; pressFrame++) {
            begin(ACT_SIDE_FLIP);
            for (int frame = 0; frame < 1; frame++) {
                m.input = frame == pressFrame ? INPUT_B_PRESSED : 0;
                EXPECT_EQ(act_side_flip(&m), FALSE);
                EXPECT_EQ(m.action, ACT_SIDE_FLIP);
                EXPECT_EQ(sAirSteps, frame + 1);
            }
            // Releasing B and changing the stick cannot turn the queued kick into a dive.
            m.input = INPUT_NONZERO_ANALOG;
            m.intendedYaw = m.faceAngle[1];
            // The game refreshes graphical facing before dispatching each update.
            mario.header.gfx.angle[1] = m.faceAngle[1];
            EXPECT_EQ(act_side_flip(&m), TRUE);
            EXPECT_EQ(m.action, ACT_JUMP_KICK);
            EXPECT_EQ(m.actionArg, 0);
            EXPECT_EQ(m.actionState, 0);
            EXPECT_EQ(sAirSteps, 1);
            EXPECT_EQ(mario.header.gfx.angle[1], (s16) 0x8000);

            // Re-entering side flip must not inherit a previous queued attack.
            set_mario_action(&m, ACT_SIDE_FLIP, 0);
            m.input = 0;
            for (int frame = 0; frame < 3; frame++) {
                EXPECT_EQ(act_side_flip(&m), FALSE);
                EXPECT_EQ(m.action, ACT_SIDE_FLIP);
            }
        }
    }
    for (int exit = 0; exit < 3; exit++) {
        begin(ACT_SIDE_FLIP);
        m.input = INPUT_B_PRESSED;
        sAirLanded = exit == 0;
        EXPECT_EQ(act_side_flip(&m), FALSE);
        if (exit == 0) {
            EXPECT_EQ(m.action, ACT_SIDE_FLIP_LAND);
        } else {
            m.input = exit == 1 ? INPUT_Z_PRESSED : INPUT_B_PRESSED | INPUT_NONZERO_ANALOG;
            m.intendedYaw = m.faceAngle[1];
            EXPECT_EQ(act_side_flip(&m), TRUE);
            EXPECT_EQ(m.action, exit == 1 ? ACT_GROUND_POUND : ACT_DIVE);
        }
        EXPECT_EQ(m.actionState, 0);
        set_mario_action(&m, ACT_SIDE_FLIP, 0);
        sAirLanded = FALSE;
        m.input = 0;
        for (int frame = 0; frame < 3; frame++) {
            EXPECT_EQ(act_side_flip(&m), FALSE);
            EXPECT_EQ(m.action, ACT_SIDE_FLIP);
        }
    }
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    RUN_CASE("sideflip.kick_startup_lock", kick_startup_lock);
    RUN_CASE("sideflip.kick_buffer", kick_buffer);
    RUN_CASE("sideflip.turn_controls", turn_controls);
    RUN_CASE("sideflip.turn_backflip", turn_backflip);
    RUN_CASE("sideflip.turn_frames", turn_frames);
    RUN_CASE("sideflip.finish_phase", finish_phase);
    RUN_CASE("sideflip.punch_entry", punch_entry);
    return test_finish();
}
