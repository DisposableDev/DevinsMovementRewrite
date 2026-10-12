/* Exercise the production jump-kick entry and shared wall-hit ledger. */
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "test_support.h"

typedef uint16_t u16;
typedef int16_t s16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
#define TRUE 1
#define FALSE 0
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define absf(x) fabsf(x)
#define MARIO_PUNCHING 1
#define MARIO_KICKING 2
#define MARIO_TRIPPING 4
#define MARIO_ACTION_SOUND_PLAYED 8
#define INPUT_A_DOWN 1
#define INPUT_B_DOWN 8
#define INPUT_B_PRESSED 2
#define INPUT_Z_PRESSED 4
#define ACT_FLAG_AIR 0x100
#define ACT_JUMP_KICK (ACT_FLAG_AIR | 1)
#define ACT_DIVE (ACT_FLAG_AIR | 2)
#define ACT_PUNCHING 3
#define ACT_MOVE_PUNCHING 4
#define ACT_FREEFALL_LAND 5
#define ACT_HARD_BACKWARD_GROUND_KB 6
#define ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK 7
#define ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE 8
#define MARIO_ANIM_AIR_KICK 1
#define MARIO_ANIM_GENERAL_FALL 2
#define MARIO_ANIM_GENERAL_LAND 3
#define PUNCH_STATE_TYPE_KICK 0x80
#define PARTICLE_TRIANGLE 1
#define SOUND_MARIO_PUNCH_HOO 1
#define SOUND_ACTION_HIT_2 2
#define AIR_STEP_NONE 0
#define AIR_STEP_LANDED 1
#define AIR_STEP_HIT_WALL 2

#include "jump_kick_wall_definitions.inc"

struct Object {
    struct {
        struct {
            struct {
                s32 animID, animFrame;
            } animInfo;
            f32 cameraToObject[3];
        } gfx;
    } header;
};
struct Surface {
    struct {
        f32 x, y, z;
    } normal;
    f32 originOffset;
    struct Object *object;
};
struct WallCollisionData {
    f32 x, y, z, offsetY, radius;
    s32 numWalls;
    struct Surface *walls[4];
};
struct MarioBodyState {
    s32 punchState;
};
struct MarioState {
    unsigned char framesSinceB;
    u32 action, actionArg, actionState, actionTimer, flags, input, particleFlags;
    f32 pos[3], forwardVel;
    s16 faceAngle[3];
    struct Object *marioObj;
    struct MarioBodyState *marioBodyState;
};

static struct Surface *sContactWalls[4];
static s32 sContactCount, sHitSounds, sStepResult, sAnimEnded;
static s32 sExplosions;
static void mario_he_thicc_attack_impact(struct MarioState *m) {
    sExplosions++;
}
static f32 sins(s16 angle) {
    return sinf(angle * (6.28318530718f / 65536.0f));
}
static f32 coss(s16 angle) {
    return cosf(angle * (6.28318530718f / 65536.0f));
}
static s32 find_wall_collisions(struct WallCollisionData *data) {
    data->numWalls = sContactCount;
    for (s32 i = 0; i < sContactCount; i++) {
        data->walls[i] = sContactWalls[i];
    }
    return sContactCount;
}
static s32 mario_is_breakdancing(struct MarioState *m) {
    return FALSE;
}
static void mario_set_forward_vel(struct MarioState *m, f32 speed) {
    m->forwardVel = speed;
}
static void play_sound(s32 sound, f32 *source) {
    if (sound == SOUND_ACTION_HIT_2) {
        sHitSounds++;
    }
}
static void play_sound_if_no_flag(struct MarioState *m, s32 sound, u32 flag) {
    m->flags |= flag;
}
static s32 set_mario_animation(struct MarioState *m, s32 anim) {
    m->marioObj->header.gfx.animInfo.animID = anim;
    return m->marioObj->header.gfx.animInfo.animFrame;
}
static s32 is_anim_at_end(struct MarioState *m) {
    return sAnimEnded;
}
static int sDiveInput;
static s32 mario_has_dive_input(struct MarioState *m) {
    return sDiveInput;
}
static void update_air_without_turn(struct MarioState *m) {
}
static s32 perform_air_step(struct MarioState *m, s32 arg) {
    return sStepResult;
}
static s32 check_fall_damage_or_get_stuck(struct MarioState *m, u32 action) {
    return FALSE;
}
static s32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    m->actionArg = arg;
    m->actionState = m->actionTimer = 0;
    m->flags &= ~MARIO_ACTION_SOUND_PLAYED;
    return TRUE;
}

static void mario_attack_cap_box_surface(struct MarioState *m, struct Surface *surface,
                                         u32 interaction) {
}
#include "jump_kick_wall_sources.inc"

#define RUN_CASE(name, fn)                                                                             \
    do {                                                                                               \
        if (test_selected(name))                                                                       \
            test_report(name, fn());                                                                   \
    } while (0)

static struct MarioState m;
static struct Object marioObject;
static struct MarioBodyState body;
static struct Surface wallA, wallACoplanar, wallB;

static void begin(void) {
    memset(&m, 0, sizeof(m));
    memset(&marioObject, 0, sizeof(marioObject));
    memset(&body, 0, sizeof(body));
    memset(&wallA, 0, sizeof(wallA));
    memset(&wallACoplanar, 0, sizeof(wallACoplanar));
    memset(&wallB, 0, sizeof(wallB));
    m.marioObj = &marioObject;
    m.marioBodyState = &body;
    m.action = ACT_JUMP_KICK;
    m.forwardVel = 30.0f;
    wallA.normal.z = wallACoplanar.normal.z = 1.0f;
    wallB.normal.x = 1.0f;
    sContactWalls[0] = &wallA;
    sContactCount = sHitSounds = sStepResult = sAnimEnded = 0;
    sExplosions = 0;
    mario_reset_attack_hit_record();
    act_jump_kick(&m);
}

static void contact(struct Surface *wall) {
    sContactWalls[0] = wall;
    sContactCount = 1;
    m.particleFlags = 0;
    check_kick_or_punch_wall(&m);
    m.flags &= ~MARIO_KICKING;
}

static s32 tick_jump_kick(void) {
    if (m.input & INPUT_B_PRESSED) {
        m.framesSinceB = 0;
    } else if (m.framesSinceB < 255) {
        m.framesSinceB++;
    }
    return act_jump_kick(&m);
}

static int one_hit_per_wall_per_jump_kick(void) {
    begin();
    contact(&wallA);
    EXPECT_EQ(sHitSounds, 1);
    EXPECT_EQ(m.particleFlags & PARTICLE_TRIANGLE, PARTICLE_TRIANGLE);
    EXPECT_EQ(m.forwardVel, -30.0f);
    for (int frame = 1; frame < 3; frame++) {
        m.marioObj->header.gfx.animInfo.animFrame = frame;
        tick_jump_kick();
        m.forwardVel = 30.0f;
        contact(frame & 1 ? &wallACoplanar : &wallA);
        EXPECT_EQ(sHitSounds, 1);
        EXPECT_EQ(m.particleFlags & PARTICLE_TRIANGLE, 0);
        EXPECT_EQ(m.forwardVel, 30.0f);
    }
    m.marioObj->header.gfx.animInfo.animFrame = 3;
    tick_jump_kick();
    contact(&wallB);
    EXPECT_EQ(sHitSounds, 2);
    EXPECT_EQ(m.particleFlags & PARTICLE_TRIANGLE, PARTICLE_TRIANGLE);
    EXPECT_EQ(m.forwardVel, -30.0f);
    return 1;
}

static int active_window(void) {
    begin();
    m.flags &= ~MARIO_KICKING;
    m.actionState = ACT_STATE_JUMP_KICK_PLAY_SOUND_AND_ANIM;
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int frame = 0; frame <= 20; frame++) {
            // Contacts consume the flag from the previous action update, then clear it.
            wallA.originOffset = cycle * 1000 + frame * 100;
            contact(&wallA);
            int activeContact = frame >= 1 && frame <= 7;
            EXPECT_EQ(sHitSounds, cycle * 7 + (frame < 7 ? frame : 7));
            EXPECT_EQ(m.particleFlags & PARTICLE_TRIANGLE, activeContact ? PARTICLE_TRIANGLE : 0);

            m.marioObj->header.gfx.animInfo.animFrame = frame;
            m.input = INPUT_A_DOWN | (frame == 7 ? INPUT_B_PRESSED : 0);
            sAnimEnded = frame == 20;
            m.forwardVel = 30.0f;
            tick_jump_kick();
            EXPECT_EQ(m.flags & MARIO_KICKING, frame < 7 ? MARIO_KICKING : 0);
            EXPECT_EQ(m.marioObj->header.gfx.animInfo.animID, MARIO_ANIM_AIR_KICK);
            EXPECT_EQ(m.forwardVel, 30.0f);
            EXPECT_EQ(m.actionState,
                      frame == 20 ? ACT_STATE_JUMP_KICK_RECOVERY : ACT_STATE_JUMP_KICK_KICKING);
        }
        m.input = INPUT_B_PRESSED;
        contact(&wallA);
        tick_jump_kick();
        EXPECT_EQ(sHitSounds, (cycle + 1) * 7);
        EXPECT_EQ(m.flags & MARIO_KICKING, 0);
        EXPECT_EQ(m.actionState, ACT_STATE_JUMP_KICK_RECOVERY);
        m.actionTimer = 9;
        m.input = INPUT_B_PRESSED;
        tick_jump_kick();
        EXPECT_EQ(m.actionState, ACT_STATE_JUMP_KICK_PLAY_SOUND_AND_ANIM);
    }
    return TRUE;
}

static int followup_resets_wall_hits(void) {
    begin();
    contact(&wallA);
    EXPECT_EQ(sHitSounds, 1);
    m.input = INPUT_A_DOWN | INPUT_B_PRESSED;
    sAnimEnded = 1;
    tick_jump_kick();
    EXPECT_EQ(m.actionState, ACT_STATE_JUMP_KICK_RECOVERY);
    m.input = INPUT_A_DOWN;
    m.actionTimer = 9;
    m.input = INPUT_B_PRESSED;
    tick_jump_kick();
    EXPECT_EQ(m.action, ACT_JUMP_KICK);
    EXPECT_EQ(m.actionState, ACT_STATE_JUMP_KICK_PLAY_SOUND_AND_ANIM);
    sContactCount = 0;
    sAnimEnded = 0;
    tick_jump_kick();
    contact(&wallA);
    EXPECT_EQ(sHitSounds, 2);
    EXPECT_EQ(m.particleFlags & PARTICLE_TRIANGLE, PARTICLE_TRIANGLE);
    tick_jump_kick();
    contact(&wallA);
    EXPECT_EQ(sHitSounds, 2);
    EXPECT_EQ(m.particleFlags & PARTICLE_TRIANGLE, 0);
    return 1;
}

static int backward_butt_slide_drag_survives_queued_kicks(void) {
    begin();
    for (int cycle = 0; cycle < 3; cycle++) {
        m.actionState = ACT_STATE_JUMP_KICK_RECOVERY;
        m.actionTimer = 9;
        m.actionArg =
            ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE | ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL;
        m.input = INPUT_B_PRESSED;
        tick_jump_kick();
        EXPECT_EQ(m.actionState, ACT_STATE_JUMP_KICK_PLAY_SOUND_AND_ANIM);
        EXPECT_EQ(m.actionArg, ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE
                                   | ACT_ARG_JUMP_KICK_BACKWARD_BUTT_SLIDE_NO_UPHILL);
    }
    return TRUE;
}

static int rebuilt_wall(void) {
    struct Object carrierA = { 0 }, carrierB = { 0 };
    for (int cycle = 0; cycle < 3; cycle++) {
        begin();
        wallA.object = &carrierA;
        contact(&wallA);
        wallA.object = &carrierB; // A different provider reuses the same surface slot.
        tick_jump_kick();
        contact(&wallA);
        EXPECT_EQ(sHitSounds, 2);
        begin();
        wallA.object = &carrierA;
        contact(&wallA);
        wallA.object = &carrierB;
        wallB.object = &carrierA; // Already-hit A moves to another surface slot.
        tick_jump_kick();
        contact(&wallB);
        EXPECT_EQ(sHitSounds, 1);
        EXPECT_EQ(m.particleFlags & PARTICLE_TRIANGLE, 0);
        begin();
        contact(&wallA);
        wallA.originOffset = 100; // A changed static plane is a fresh wall too.
        tick_jump_kick();
        contact(&wallA);
        EXPECT_EQ(sHitSounds, 2);
    }
    return TRUE;
}

static int forgotten_provider(void) {
    struct Object carrierA = { 0 }, carrierB = { 0 };
    begin();
    wallA.object = &carrierA;
    wallB.object = &carrierB;
    contact(&wallA);
    tick_jump_kick();
    contact(&wallB);
    remember_punch_hit_object(&carrierA);
    remember_punch_hit_object(&carrierB);
    mario_forget_attack_object(&carrierA);
    EXPECT_EQ(punch_hit_object(&carrierA), FALSE);
    EXPECT_EQ(punch_hit_object(&carrierB), TRUE);
    tick_jump_kick();
    contact(&wallB);
    EXPECT_EQ(sHitSounds, 2);
    tick_jump_kick();
    contact(&wallA); // A reused object slot can be hit again.
    EXPECT_EQ(sHitSounds, 3);
    mario_forget_attack_object(&carrierA);
    mario_forget_attack_object(&carrierA);
    EXPECT_EQ(sPunchContacts.hitWallCount, 1);
    return TRUE;
}

static int contact_blast(void) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin();
        check_kick_or_punch_wall(&m);
        EXPECT_EQ(sExplosions, 0);
        contact(&wallA);
        EXPECT_EQ(sExplosions, 1);
        tick_jump_kick();
        contact(&wallB);
        EXPECT_EQ(sExplosions, 1); // Another wall is still part of this strike.
        m.actionState = ACT_STATE_JUMP_KICK_PLAY_SOUND_AND_ANIM;
        tick_jump_kick();
        contact(&wallA);
        EXPECT_EQ(sExplosions, 2);
        m.action = ACT_MOVE_PUNCHING;
        m.actionArg = ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE;
        m.flags = MARIO_TRIPPING;
        mario_reset_attack_hit_record();
        check_kick_or_punch_wall(&m);
        EXPECT_EQ(sExplosions, 2);
    }
    return TRUE;
}

static int followup_buffer(void) {
    for (int held = 0; held < 4; held++) {
        for (int dive = 0; dive < 2; dive++) {
            for (int age = 0; age <= 3; age++) {
                begin();
                sDiveInput = dive;
                m.actionState = ACT_STATE_JUMP_KICK_RECOVERY;
                m.actionTimer = 9;
                m.actionArg = ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED;
                m.framesSinceB = age;
                m.input = (held & 1 ? INPUT_A_DOWN : 0) | (held & 2 ? INPUT_B_DOWN : 0);
                act_jump_kick(&m);
                EXPECT_EQ(m.action, age < 3 && dive ? ACT_DIVE : ACT_JUMP_KICK);
                EXPECT_EQ(m.actionState, age < 3 ? ACT_STATE_JUMP_KICK_PLAY_SOUND_AND_ANIM
                                                 : ACT_STATE_JUMP_KICK_RECOVERY);
                EXPECT_EQ(m.actionArg & ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED, 0);
            }
        }
    }
    sDiveInput = 0;
    return TRUE;
}

static int no_self_cancel_and_recovery_delay(void) {
    for (s32 recovering = 0; recovering < 2; recovering++) {
        for (s32 timer = 0; timer <= 30; timer++) {
            begin();
            sDiveInput = 0;
            m.actionState = recovering ? ACT_STATE_JUMP_KICK_RECOVERY : ACT_STATE_JUMP_KICK_KICKING;
            m.actionTimer = timer;
            m.input = INPUT_B_PRESSED;
            tick_jump_kick();
            EXPECT_EQ(m.actionState, recovering && timer >= 9 ? ACT_STATE_JUMP_KICK_PLAY_SOUND_AND_ANIM
                                     : recovering             ? ACT_STATE_JUMP_KICK_RECOVERY
                                                              : ACT_STATE_JUMP_KICK_KICKING);
        }
    }
    return TRUE;
}

static int animation_cancel(void) {
    for (int dive = 0; dive < 2; dive++) {
        for (int pressedAt = 13; pressedAt <= 16; pressedAt++) {
            begin();
            sDiveInput = dive;
            m.actionState = ACT_STATE_JUMP_KICK_PLAY_SOUND_AND_ANIM;
            m.actionTimer = 0;
            for (int frame = 1; frame <= 16; frame++) {
                m.marioObj->header.gfx.animInfo.animFrame = frame - 2;
                m.input = frame == pressedAt ? INPUT_B_PRESSED : 0;
                tick_jump_kick();
                EXPECT_EQ(m.actionState, frame == 16 && pressedAt >= 14 && dive
                                             ? ACT_STATE_JUMP_KICK_PLAY_SOUND_AND_ANIM
                                             : ACT_STATE_JUMP_KICK_KICKING);
            }
            EXPECT_EQ(m.action, pressedAt >= 14 && dive ? ACT_DIVE : ACT_JUMP_KICK);
        }
    }
    sDiveInput = 0;
    for (int timer = 14; timer <= 15; timer++) {
        begin();
        m.actionTimer = timer;
        m.input = INPUT_Z_PRESSED;
        act_jump_kick(&m);
        EXPECT_EQ(m.action, timer == 15 ? ACT_GROUND_POUND : ACT_JUMP_KICK);
    }
    begin();
    m.actionTimer = 15;
    m.actionArg = ACT_ARG_JUMP_KICK_FOLLOWUP_QUEUED;
    m.input = INPUT_Z_PRESSED | INPUT_B_PRESSED;
    act_jump_kick(&m);
    EXPECT_EQ(m.action, ACT_GROUND_POUND);
    begin();
    m.actionTimer = 21;
    sAnimEnded = TRUE;
    act_jump_kick(&m);
    EXPECT_EQ(m.actionState, ACT_STATE_JUMP_KICK_RECOVERY);
    m.input = INPUT_B_PRESSED;
    sAnimEnded = FALSE;
    act_jump_kick(&m);
    EXPECT_EQ(m.actionState, ACT_STATE_JUMP_KICK_RECOVERY);
    return TRUE;
}

int main(int argc, char **argv) {
    test_init(argc, argv);
    RUN_CASE("jump_kick_wall.contact_blast", contact_blast);
    RUN_CASE("jump_kick_wall.rebuilt_wall", rebuilt_wall);
    RUN_CASE("jump_kick_wall.forgotten_provider", forgotten_provider);
    RUN_CASE("jump_kick_wall.same_wall", one_hit_per_wall_per_jump_kick);
    RUN_CASE("jump_kick_wall.active_window", active_window);
    RUN_CASE("jump_kick_wall.followup", followup_resets_wall_hits);
    RUN_CASE("jump_kick_wall.butt_slide_drag", backward_butt_slide_drag_survives_queued_kicks);
    RUN_CASE("jump_kick_wall.followup_buffer", followup_buffer);
    RUN_CASE("jump_kick_wall.animation_cancel", animation_cancel);
    RUN_CASE("jump_kick_wall.recovery_delay", no_self_cancel_and_recovery_delay);
    return test_finish();
}
