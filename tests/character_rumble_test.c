/* Real character action policy and VI rumble state progression; motor I/O is recorded. */
#include <stdint.h>
#include <math.h>
#include <stddef.h>
#include "test_support.h"
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int16_t s16;
typedef int32_t s32;
typedef float f32;
typedef s32 OSIntMask;
#define FALSE 0
#define TRUE 1
#define OS_IM_NONE 0
#define NO_SOUND 0
#define absf(x) fabsf(x)
#include "rumble_definitions.inc"
static struct RumbleSettings gCurrRumbleSettings;
static struct RumbleData gRumbleDataQueue[3];
static s32 sCharacterMovementRumbleVblanks, sCharacterMovementRumbleLevel;
static s32 gResetTimer, gNumVblanks, gRumblePakTimer, sRumblePakActive;
static s32 sRumblePakResetStopped, sRumblePakThreadActive, sRumblePakErrorCount;
static s32 sLockBusy, sInitCalls, sDataUpdates, gRumbleThreadVIMesgQueue;
static s32 sResetMotorPending, sReleases;
typedef void *OSMesg;
#define OS_MESG_BLOCK 1
#define osSyncPrintf(...) ((void) 0)
#include <setjmp.h>
#include <assert.h>
#define UNUSED __attribute__((unused))
static jmp_buf sShutdown;
static void osRecvMesg(s32 *queue, OSMesg *message, s32 block) {
    (void) message;
    assert(queue == &gRumbleThreadVIMesgQueue && block == OS_MESG_BLOCK);
    gResetTimer = 1;
    gNumVblanks = 60;
    sRumblePakActive = FALSE;
}
static void osStopThread(void *thread) {
    assert(thread == NULL && gResetTimer && !sRumblePakThreadActive);
    longjmp(sShutdown, 1);
}
static void update_rumble_data_queue(void) {
    sDataUpdates++;
}
static s32 try_acquire_rumble_pak(void) {
    return !sLockBusy;
}

static void *gCurrDemoInput, *gCurrCreditsEntry;
static s32 sPalette, sMotor, sCalls, gSIEventMesgQueue;
static struct {
    s32 channel;
} gRumblePakPfs;
static struct {
    s32 port;
} sController, *gPlayer1Controller = &sController;
typedef int BehaviorScript;
static const BehaviorScript bhvBobomb[] = { 1 };
struct Object {
    const BehaviorScript *behavior;
};
struct MarioState {
    u32 action;
    s32 actionState;
    f32 forwardVel, vel[3], pos[3], floorHeight;
    void *floor;
    struct Object *heldObj;
};
static struct MarioState sMario, *gMarioState = &sMario;
static const void *segmented_to_virtual(const void *p) {
    return p;
}
static int floorSurface;
s32 mario_has_rumble_contact(struct MarioState *m);
static s32 sHeldPose;
static s32 mario_ground_twirl_is_held_pose(struct MarioState *m) {
    return sHeldPose;
}
static u8 mario_character_palette(void) {
    return sPalette;
}
static OSIntMask osSetIntMask(OSIntMask mask) {
    return mask;
}
static void start_rumble(void) {
    sMotor = 1;
    sCalls++;
}
static void stop_rumble(void) {
    sMotor = 0;
    sCalls++;
}
static void block_until_rumble_pak_free(void) {
}
static void release_rumble_pak_control(void) {
    sReleases++;
}
static s32 osMotorInitEx(void *queue, void *pak, s32 port) {
    assert(port >= 0 && port < 4);
    gRumblePakPfs.channel = port;
    sInitCalls++;
    return 0;
}
static s32 osMotorStop(void *pak) {
    stop_rumble();
    return 0;
}
static s32 osMotorStopExForReset(void *pak) {
    (void) pak;
    stop_rumble();
    return !sResetMotorPending;
}
#include "rumble_sources.inc"
static void begin(void) {
    game_options_init(NULL, NULL);
    game_options_select_file(0);
    sController.port = gRumblePakPfs.channel = 0;
    memset(&gCurrRumbleSettings, 0, sizeof(gCurrRumbleSettings));
    memset(gRumbleDataQueue, 0, sizeof(gRumbleDataQueue));
    sCharacterMovementRumbleVblanks = sCharacterMovementRumbleLevel = 0;
    sRumblePakResetStopped = sRumblePakThreadActive = sRumblePakErrorCount = 0;
    sLockBusy = sInitCalls = sDataUpdates = 0;
    sResetMotorPending = sReleases = 0;
    gResetTimer = gNumVblanks = sMotor = sCalls = 0;
    gCurrDemoInput = gCurrCreditsEntry = NULL;
    sPalette = MARIO_PALETTE_HE_THICC;
    sHeldPose = 0;
    memset(&sMario, 0, sizeof(sMario));
}
static int actions(void) {
    const struct {
        u32 action;
        s32 level;
    } cases[] = { { ACT_WALKING, 2 },
                  { ACT_HOLD_WALKING, 2 },
                  { ACT_HOLD_HEAVY_WALKING, 2 },
                  { ACT_TURNING_AROUND, 2 },
                  { ACT_FINISH_TURNING_AROUND, 2 },
                  { ACT_BRAKING, 2 },
                  { ACT_BRAKING_STOP, 2 },
                  { ACT_DECELERATING, 2 },
                  { ACT_HOLD_DECELERATING, 2 },
                  { ACT_METAL_WATER_WALKING, 2 },
                  { ACT_HOLD_METAL_WATER_WALKING, 2 },
                  { ACT_JUMP, 1 },
                  { ACT_DOUBLE_JUMP, 1 },
                  { ACT_TRIPLE_JUMP, 1 },
                  { ACT_BACKFLIP, 1 },
                  { ACT_DAB, 4 },
                  { ACT_GROUND_TWIRL, 5 },
                  { ACT_GROUND_POUND, 4 },
                  { ACT_DIVE, 5 },
                  { ACT_PUNCHING, 4 },
                  { ACT_MOVE_PUNCHING, 4 },
                  { ACT_SLIDE_KICK, 5 },
                  { ACT_CROUCHING, 1 },
                  { ACT_CRAWLING, 2 },
                  { ACT_BREASTSTROKE, 3 },
                  { ACT_FLUTTER_KICK, 3 },
                  { ACT_CLIMBING_POLE, 2 },
                  { ACT_HANG_MOVING, 2 },
                  { ACT_JUMP_LAND, 6 },
                  { ACT_HOLD_JUMP_LAND, 6 },
                  { ACT_TRIPLE_JUMP_LAND, 6 },
                  { ACT_GROUND_POUND_LAND, 8 },
                  { ACT_GROUND_BONK, 8 },
                  { ACT_HARD_FORWARD_GROUND_KB, 8 },
                  { ACT_UNINITIALIZED, 0 },
                  { ACT_IDLE, 0 },
                  { ACT_PANTING, 0 },
                  { ACT_START_SLEEPING, 0 },
                  { ACT_SLEEPING, 0 },
                  { ACT_HOLD_IDLE, 0 },
                  { ACT_HOLD_HEAVY_IDLE, 0 },
                  { ACT_HOLD_PANTING_UNUSED, 0 },
                  { ACT_SHIVERING, 0 },
                  { ACT_FIRST_PERSON, 0 },
                  { ACT_WATER_IDLE, 0 },
                  { ACT_HOLD_WATER_IDLE, 0 },
                  { ACT_METAL_WATER_STANDING, 0 },
                  { ACT_HOLD_METAL_WATER_STANDING, 0 },
                  { ACT_INTRO_CUTSCENE, 0 },
                  { ACT_READING_SIGN, 0 },
                  { ACT_QUICKSAND_DEATH, 0 },
                  { ACT_HOLDING_POLE, 0 },
                  { ACT_TOP_OF_POLE, 0 },
                  { ACT_HANGING, 0 },
                  { ACT_LEDGE_GRAB, 0 } };
    for (int palette = 0; palette < 2; palette++) {
        begin();
        sPalette = palette;
        for (size_t i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
            struct MarioState m = { .action = cases[i].action, .floor = &floorSurface };
            mario_update_character_rumble(&m);
            EXPECT_EQ(sCharacterMovementRumbleLevel, palette ? cases[i].level : 0);
            EXPECT_EQ(sCharacterMovementRumbleVblanks, palette && cases[i].level ? 6 : 0);
        }
    }
    begin();
    struct MarioState m = { .action = ACT_WALKING, .floor = &floorSurface };
    const f32 speeds[] = { 0, 23.9f, 24, 40, -24, 8 };
    const s32 strengths[] = { 2, 2, 3, 3, 3, 2 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int i = 0; i < 6; i++) {
            m.forwardVel = speeds[i];
            mario_update_character_rumble(&m);
            EXPECT_EQ(sCharacterMovementRumbleLevel, strengths[i]);
        }
    }
    m.action = ACT_JUMP;
    m.vel[1] = 30;
    mario_update_character_rumble(&m);
    EXPECT_EQ(sCharacterMovementRumbleLevel, 4);
    m.vel[1] = -10;
    mario_update_character_rumble(&m);
    EXPECT_EQ(sCharacterMovementRumbleLevel, 1);
    m.action = ACT_JUMP_LAND;
    mario_update_character_rumble(&m);
    EXPECT_EQ(sCharacterMovementRumbleLevel, 6);
    m.action = ACT_DAB;
    m.actionState = 1;
    mario_update_character_rumble(&m);
    EXPECT_EQ(sCharacterMovementRumbleLevel, 1);
    m.action = ACT_GROUND_POUND;
    mario_update_character_rumble(&m);
    EXPECT_EQ(sCharacterMovementRumbleLevel, 6);
    m.action = ACT_GROUND_POUND_LAND;
    mario_update_character_rumble(&m);
    EXPECT_EQ(sCharacterMovementRumbleLevel, 8);
    m.action = ACT_GROUND_TWIRL;
    sHeldPose = 1;
    mario_update_character_rumble(&m);
    EXPECT_EQ(sCharacterMovementRumbleLevel, 1);
    sHeldPose = 0;
    mario_update_character_rumble(&m);
    EXPECT_EQ(sCharacterMovementRumbleLevel, 5);
    gCurrDemoInput = &m;
    mario_update_character_rumble(&m);
    EXPECT_EQ(sCharacterMovementRumbleVblanks, 0);
    gCurrDemoInput = NULL;
    gCurrCreditsEntry = &m;
    mario_update_character_rumble(&m);
    EXPECT_EQ(sCharacterMovementRumbleVblanks, 0);
    return 1;
}
static int ground_air_contacts(void) {
    begin();
    for (int cycle = 0; cycle < 3; cycle++) {
        sMario.action = ACT_WALKING;
        sMario.floor = &floorSurface;
        sMario.pos[1] = 0;
        mario_update_character_rumble(gMarioState);
        EXPECT_EQ(sCharacterMovementRumbleLevel, 2);
        // Travel feedback also continues between floor contacts.
        sMario.pos[1] = 5;
        mario_update_character_rumble(gMarioState);
        EXPECT_EQ(sCharacterMovementRumbleLevel, 2);
        const u32 air[] = { ACT_JUMP,       ACT_TWIRLING, ACT_GROUND_POUND, ACT_DIVE,
                            ACT_SLIDE_KICK, ACT_FLYING,   ACT_BREASTSTROKE };
        for (unsigned i = 0; i < sizeof(air) / sizeof(*air); i++) {
            sMario.action = air[i];
            sMario.floor = NULL;
            memset(&gCurrRumbleSettings, 0, sizeof(gCurrRumbleSettings));
            mario_update_character_rumble(gMarioState);
            EXPECT_EQ(sCharacterMovementRumbleLevel > 0, 1);
            reset_rumble_timers_slip();
            queue_rumble_submerged();
            EXPECT_EQ(gCurrRumbleSettings.slip, 4);
            EXPECT_EQ(gCurrRumbleSettings.vibrate, 4);
        }
        // A queued airborne contact survives movement-overlay cancellation.
        gCurrRumbleSettings.start = 2;
        mario_update_character_rumble(gMarioState);
        update_rumble_pak();
        EXPECT_EQ(sMotor, 1);
        EXPECT_EQ(gCurrRumbleSettings.start, 1);
        memset(&gCurrRumbleSettings, 0, sizeof(gCurrRumbleSettings));
        sMario.action = ACT_CLIMBING_POLE;
        mario_update_character_rumble(gMarioState);
        EXPECT_EQ(sCharacterMovementRumbleLevel, 2);
        reset_rumble_timers_slip();
        EXPECT_EQ(gCurrRumbleSettings.slip, 7);
        memset(&gCurrRumbleSettings, 0, sizeof(gCurrRumbleSettings));
        sMario.action = ACT_JUMP;
        sMario.heldObj = NULL;
        struct Object bomb = { .behavior = bhvBobomb };
        sMario.heldObj = &bomb;
        reset_rumble_timers_slip();
        EXPECT_EQ(gCurrRumbleSettings.slip, 7);
        sMario.heldObj = NULL;
        memset(&gCurrRumbleSettings, 0, sizeof(gCurrRumbleSettings));
        sPalette = MARIO_PALETTE_STANDARD;
        reset_rumble_timers_slip();
        EXPECT_EQ(gCurrRumbleSettings.slip, 7);
        sPalette = MARIO_PALETTE_HE_THICC;
        memset(&gCurrRumbleSettings, 0, sizeof(gCurrRumbleSettings));
        sMario.action = ACT_GROUND_POUND_LAND;
        sMario.floor = &floorSurface;
        sMario.pos[1] = 0;
        mario_update_character_rumble(gMarioState);
        EXPECT_EQ(sCharacterMovementRumbleLevel, 8);
    }
    return 1;
}
static int motor(void) {
    for (int scenario = 0; scenario < 8; scenario++) {
        begin();
        gNumVblanks = 2;
        switch (scenario) {
            case 1:
                gCurrRumbleSettings.delay = 2;
                break;
            case 2:
                gCurrRumbleSettings.start = 2;
                break;
            case 3:
                gCurrRumbleSettings.timer = 3;
                gCurrRumbleSettings.event = 1;
                break;
            case 4:
                gCurrRumbleSettings.timer = 3;
                gCurrRumbleSettings.level = 10;
                break;
            case 5:
                gCurrRumbleSettings.timer = 3;
                gCurrRumbleSettings.count = 256;
                break;
            case 6:
                gCurrRumbleSettings.slip = 7;
                break;
            case 7:
                gCurrRumbleSettings.slip = 3;
                gCurrRumbleSettings.vibrate = 4;
                break;
        }
        struct RumbleSettings before = gCurrRumbleSettings;
        update_rumble_pak();
        EXPECT_EQ(sMotor, scenario == 2 || scenario == 3 || scenario == 5 || scenario == 6);
        EXPECT_EQ(sCalls, 1);
        struct RumbleSettings after = gCurrRumbleSettings;
        gCurrRumbleSettings = before;
        sCalls = 0;
        set_character_movement_rumble(CHARACTER_RUMBLE_HEAVY);
        update_rumble_pak();
        EXPECT_EQ(sMotor, 1);
        EXPECT_EQ(sCalls, 1);
        EXPECT_EQ(memcmp(&after, &gCurrRumbleSettings, sizeof(after)), 0);
    }
    for (int level = 0; level <= 8; level++) {
        begin();
        int motorOn = 0;
        for (int vi = 0; vi < 32; vi++) {
            gNumVblanks = vi;
            set_character_movement_rumble(level);
            update_rumble_pak();
            motorOn += sMotor;
        }
        EXPECT_EQ(motorOn, level * 4);
    }
    EXPECT_EQ(gRumbleDataQueue[0].comm | gRumbleDataQueue[1].comm | gRumbleDataQueue[2].comm, 0);
    return 1;
}
static int lifecycle(void) {
    begin();
    set_character_movement_rumble(CHARACTER_RUMBLE_HEAVY);
    for (int vi = 0; vi < 6; vi++) {
        update_rumble_pak();
        EXPECT_EQ(sMotor, 1);
    }
    update_rumble_pak();
    EXPECT_EQ(sMotor, 0);
    for (int frame = 0; frame < 120; frame++) {
        set_character_movement_rumble(CHARACTER_RUMBLE_HEAVY);
        update_rumble_pak();
        update_rumble_pak();
        EXPECT_EQ(sMotor, 1);
    }
    set_character_movement_rumble(CHARACTER_RUMBLE_NONE);
    update_rumble_pak();
    EXPECT_EQ(sMotor, 0);
    set_character_movement_rumble(CHARACTER_RUMBLE_HEAVY);
    cancel_rumble();
    EXPECT_EQ(sCharacterMovementRumbleVblanks, 0);
    update_rumble_pak();
    EXPECT_EQ(sMotor, 0);
    set_character_movement_rumble(CHARACTER_RUMBLE_HEAVY);
    gResetTimer = 1;
    update_rumble_pak();
    EXPECT_EQ(sMotor, 0);
    EXPECT_EQ(sCharacterMovementRumbleVblanks, 0);
    return 1;
}
static int reset_shutdown(void) {
    begin();
    sRumblePakActive = TRUE;
    gResetTimer = 1;
    for (int vi = 0; vi < 4; vi++) {
        update_rumble_pak();
    }
    EXPECT_EQ(sCalls, 1);
    EXPECT_EQ(sMotor, 0);
    EXPECT_EQ(sRumblePakResetStopped, TRUE);
    begin();
    sRumblePakActive = TRUE;
    sLockBusy = 1;
    gResetTimer = 1;
    for (int vi = 0; vi < 4; vi++) {
        update_rumble_pak();
    }
    EXPECT_EQ(sCalls, 0);
    EXPECT_EQ(sRumblePakResetStopped, TRUE);
    begin();
    sRumblePakActive = TRUE;
    sResetMotorPending = TRUE;
    gResetTimer = 1;
    stop_rumble_for_reset();
    stop_rumble_for_reset();
    EXPECT_EQ(sCalls, 1);
    EXPECT_EQ(sReleases, 0);
    begin();
    if (setjmp(sShutdown) == 0) {
        thread6_rumble_loop(NULL);
    }
    EXPECT_EQ(sInitCalls, 1);
    EXPECT_EQ(sDataUpdates, 0);
    EXPECT_EQ(sRumblePakThreadActive, FALSE);
    EXPECT_EQ(sRumblePakResetStopped, TRUE);
    return 1;
}
static int options(void) {
    for (s32 setting = 0; setting < 3; setting++) {
        begin();
        sRumblePakActive = TRUE;
        game_options_set(GO_RUMBLE, setting);
        gCurrRumbleSettings.start = 8;
        s32 active = 0;
        for (gNumVblanks = 0; gNumVblanks < 8; gNumVblanks++) {
            update_rumble_pak();
            active += sMotor;
        }
        EXPECT_EQ(gCurrRumbleSettings.start, 0);
        EXPECT_EQ(active, setting == 0 ? 0 : setting == 1 ? 4 : 8);
    }
    begin();
    sRumblePakActive = TRUE;
    sController.port = -1;
    update_rumble_pak();
    EXPECT_EQ(sRumblePakActive, FALSE);
    EXPECT_EQ(sMotor, 0);
    EXPECT_EQ(sCalls, 2); // Stop old port, then request Off; actual motor I/O ignores inactive paks.
    cancel_rumble();
    EXPECT_EQ(sInitCalls, 0);
    sController.port = 3;
    cancel_rumble();
    EXPECT_EQ(gRumblePakPfs.channel, 3);
    EXPECT_EQ(sInitCalls, 1);
    return TRUE;
}
int main(int argc, char **argv) {
    test_init(argc, argv);
#define RUN(name, fn)                                                                                  \
    if (test_selected("rumble." name))                                                                 \
    test_report("rumble." name, fn())
    RUN("actions", actions);
    RUN("ground_air_contacts", ground_air_contacts);
    RUN("motor", motor);
    RUN("lifecycle", lifecycle);
    RUN("reset_shutdown", reset_shutdown);
    RUN("options", options);
    return test_finish();
}
