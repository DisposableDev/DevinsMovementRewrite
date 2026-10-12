/* Execute the production update and pause gate; action handlers and world services are supplied. */
#include <stdint.h>
#include "test_support.h"
typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
typedef f32 Vec3f[3];
typedef void OSContStatus;
typedef void OSContPadEx;
#define TRUE 1
#define FALSE 0
#define jrb_simpleflips_room_active() 0
#define jrb_simpleflips_exit_locked() 0
#define jrb_simpleflips_is_game_over() 0
#define jrb_simpleflips_confine_mario(m) ((void) (m))
#define UNUSED
#define ENABLE_DEBUG_FREE_MOVE
#define A_BUTTON 1
#define B_BUTTON 2
#define Z_TRIG 4
#define L_TRIG 8
#define U_JPAD 16
#define START_BUTTON 32
#define GRAPH_RENDER_INVISIBLE 1
#define ACTIVE_PARTICLE_NONE 0
#define MARIO_SPAWN_FALL_PROTECTION_NONE 0
#define MARIO_SPAWN_FALL_PROTECTION_ACTIVE 1
#define MARIO_SPAWN_FALL_PROTECTION_LANDED 2
#define SURFACE_HORIZONTAL_WIND 1
#define SURFACE_VERTICAL_WIND 2
#define SURFACE_IS_UNSAFE(type) FALSE
#define SOUND_ENV_WIND2 1
#define CAMERA_MODE_8_DIRECTIONS 1
#define ACT_ARG_QUICKSAND_DEATH_SHALLOW_WATER 1
#define INT_STATUS_NONE 0
#define INT_STATUS_STOP_RIDING 0x400000
#include "zero_health_definitions.inc"
struct Surface {
    s32 type, force;
};
struct Object {
    struct {
        struct {
            struct {
                u32 flags;
            } node;
            Vec3f cameraToObject;
        } gfx;
    } header;
    u32 oInteractStatus;
    u32 oHootMarioReleaseTime;
};
struct Camera {
    s32 mode;
};
struct Area {
    struct Camera *camera;
};
struct MarioState {
    u8 objectRecoveryAFrames, objectRecoveryBFrames;
    u8 automaticAFrames, automaticBFrames, automaticZFrames, groundPoundInputFrames;
    u8 airTwirlInputFrames, landingJumpBufferedEdge;
    u8 metalFallGroundPoundImpact;
    struct Object *metalFallGroundPoundFloorObject;
    s16 health, faceAngle[3], intendedYaw, movePitch, moveYaw;
    u16 input, bullyKnockbackBuffer;
    u32 action, actionTimer, particleFlags;
    f32 intendedMag, forwardVel, moveSpeed, lateralSpeed, floorHeight, waterLevel, quicksandDepth;
    Vec3f prevPos, pos, lastSafePos;
    struct Controller *controller;
    struct Object *marioObj, *waterGroundPoundFloorObject;
    struct Object *riddenObj, *heldObj, *usedObj;
    struct Surface *floor;
    struct Area *area;
    u8 framesSinceA, framesSinceB, zHeldFrames, zCrouchBuffered, zBPressed, crouchSlideBackflip;
    u8 backwardLongJumpChain, backwardLongJumpCrouchFrames;
    u8 landingJumpInputFrames, wallKickInputFrames, wallKickForwardStickFrames;
    u8 wallKickBackwardStickFrames, burnCoinJumpPending, spawnFallProtection, waterGroundPoundImpact;
};
static struct Controller sController, *gPlayer1Controller = &sController;
static struct MarioState sMario, *gMarioState = &sMario;
// This suite uses gameplay levels; the ending suite exercises its distinct pause policy.
#define LEVEL_ENDING 99
static s32 gCurrLevelNum;
static struct Object sObject;
static struct Surface sFloor;
static struct Camera sCamera;
static struct Area sArea = { &sCamera };
static struct {
    s32 isActive;
} gWarpTransition;
static s32 sDelayedWarpOp, sDialog = -1;
static s32 sDispatches, sNewActions, sHealthUpdates, sGeometryUpdates, sBlockedErrors;
static s32 sKillOnContact, sKillOnDispatch, sTwirlQueued;
static s32 sRestoreHealth;
static s32 sLandDrown, sShallowDrown, sShellMusicStops;
static u32 gGlobalTimer;
static void stop_shell_music(void) {
    sShellMusicStops++;
}
static void mario_drop_held_object(struct MarioState *m) {
    m->heldObj = NULL;
}
static void mario_ground_twirl_cancel_queued_input(void) {
    sTwirlQueued = FALSE;
}
static void vec3f_copy(Vec3f dest, Vec3f src) {
    memcpy(dest, src, sizeof(Vec3f));
}
static void vec3f_get_dist_and_angle(Vec3f a, Vec3f b, f32 *dist, s16 *pitch, s16 *yaw) {
}
static void vec3f_get_lateral_dist(Vec3f a, Vec3f b, f32 *dist) {
}
static void set_camera_mode(struct Camera *camera, s32 mode, s32 frames) {
    camera->mode = mode;
}
static s32 set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->action = action;
    sNewActions++;
    return TRUE;
}
static void update_mario_inputs(struct MarioState *m) {
    m->input = INPUT_OFF_FLOOR | INPUT_STOMPED;
    if (m->controller->buttonPressed & A_BUTTON) {
        m->input |= INPUT_A_PRESSED;
    }
    if (m->controller->buttonDown & A_BUTTON) {
        m->input |= INPUT_A_DOWN;
    }
    if (m->controller->buttonPressed & B_BUTTON) {
        m->input |= INPUT_B_PRESSED;
    }
    if (m->controller->buttonDown & B_BUTTON) {
        m->input |= INPUT_B_DOWN;
    }
    if (m->controller->buttonPressed & Z_TRIG) {
        m->input |= INPUT_Z_PRESSED;
    }
    if (m->controller->buttonDown & Z_TRIG) {
        m->input |= INPUT_Z_DOWN;
    }
    if (m->controller->stickMag > 0) {
        m->input |= INPUT_NONZERO_ANALOG;
    }
    sGeometryUpdates++;
}
static void mario_process_interactions(struct MarioState *m) {
    if (sKillOnContact) {
        m->health = 0xFF;
    }
}
static void check_shortcut(struct MarioState *m) {
    if (m->controller->buttonPressed & L_TRIG) {
        sNewActions++;
    }
}
static void mario_try_ground_twirl(struct MarioState *m) {
    check_shortcut(m);
}
static s32 mario_try_air_twirl(struct MarioState *m) {
    (void) m;
    return FALSE;
}
static void mario_ground_twirl_update_cooldown(void) {
}
static s32 mario_dab_update_shortcut(struct MarioState *m) {
    check_shortcut(m);
    return FALSE;
}
static s32 dispatch(struct MarioState *m) {
    sDispatches++;
    m->actionTimer++;
    m->pos[0] += m->forwardVel; // Existing momentum still advances.
    if (sKillOnDispatch && sDispatches == 1) {
        m->health = 0xFF;
        m->action = ACT_HARD_BACKWARD_GROUND_KB;
        return TRUE; // Forced reaction must be allowed to redispatch.
    }
    if (m->health < 0x100) {
        if (m->controller->buttonDown || m->controller->buttonPressed || m->controller->buttonReleased
            || m->controller->stickMag || m->controller->stickX || m->controller->stickY
            || m->intendedMag || m->landingJumpInputFrames || m->wallKickInputFrames
            || m->wallKickForwardStickFrames || m->wallKickBackwardStickFrames || m->burnCoinJumpPending
            || m->bullyKnockbackBuffer || m->crouchSlideBackflip || m->zCrouchBuffered || m->zBPressed
            || m->framesSinceA != 0xFF || m->framesSinceB != 0xFF || sTwirlQueued
            || !(m->input & INPUT_OFF_FLOOR) || !(m->input & INPUT_STOMPED)) {
            sBlockedErrors++;
        }
    }
    if (m->input
        & (INPUT_A_PRESSED | INPUT_B_PRESSED | INPUT_Z_PRESSED | INPUT_A_DOWN | INPUT_B_DOWN
           | INPUT_Z_DOWN | INPUT_FIRST_PERSON | INPUT_NONZERO_ANALOG)) {
        sNewActions++;
    }
    return FALSE;
}
#define mario_execute_stationary_action dispatch
#define mario_execute_moving_action dispatch
#define mario_execute_airborne_action dispatch
#define mario_execute_submerged_action dispatch
#define mario_execute_cutscene_action dispatch
#define mario_execute_automatic_action dispatch
#define mario_execute_object_action dispatch
static s32 mario_should_drown_on_land(struct MarioState *m) {
    return sLandDrown && m->health < 0x100;
}
static s32 mario_has_shallow_water_death_pending(struct MarioState *m) {
    return sShallowDrown;
}
static s32 mario_pose_exposes_cap_to_damage(struct MarioState *m) {
    return FALSE;
}
static s32 mario_floor_is_slippery(struct MarioState *m) {
    return FALSE;
}
static void mario_blow_off_cap(struct MarioState *m, f32 speed) {
}
static void update_mario_health(struct MarioState *m) {
    sHealthUpdates++;
}
static s32 get_dialog_id(void) {
    return sDialog;
}
static void spawn_wind_particles(s32 type, s32 yaw) {
}
static void play_sound(s32 sound, Vec3f pos) {
}
static void play_infinite_stairs_music(void) {
}
static void mario_spin_dizzy_reset(void) {
}
#define SERVICE(name)                                                                                  \
    static void name(struct MarioState *m) {                                                           \
    }
SERVICE(mario_tick_cap_pickup_lock)
SERVICE(mario_reset_bodystate)
SERVICE(mario_update_wind_death_grace)
SERVICE(update_bully_knockback_buffer)
SERVICE(mario_handle_special_floors)
SERVICE(mario_spin_dizzy_update)
SERVICE(mario_swimming_update)
SERVICE(mario_limit_velocity)
SERVICE(update_pole_handstand_facing)
SERVICE(mario_transition_pose_update_gfx)
SERVICE(sink_mario_in_quicksand)
SERVICE(squish_mario_model)
SERVICE(mario_he_thicc_update)
SERVICE(mario_refresh_soaked_after_movement)
SERVICE(mario_emit_soaked_drips)
SERVICE(update_mario_submerged_effects)
SERVICE(update_submerged_camera)
SERVICE(update_mario_info_for_cam)
SERVICE(mario_update_hitbox_and_cap_model)
SERVICE(queue_rumble_particles)
SERVICE(mario_update_character_rumble)
/* Ordinary zero-health actions run outside the protected secret encounter. */
static s32 castle_secret_lobby_active(void) {
    return FALSE;
}
#include "zero_health_sources.inc"

static void begin(u32 action, s16 health) {
    memset(&sMario, 0, sizeof(sMario));
    memset(&sController, 0, sizeof(sController));
    sMario.action = action;
    sMario.health = health;
    sMario.controller = &sController;
    sMario.marioObj = &sObject;
    sMario.floor = &sFloor;
    sMario.area = &sArea;
    sMario.forwardVel = 12.0f;
    sMario.intendedMag = 32.0f;
    sMario.faceAngle[1] = 1234;
    sMario.landingJumpInputFrames = sMario.wallKickInputFrames = 2;
    sMario.wallKickForwardStickFrames = sMario.wallKickBackwardStickFrames = 3;
    sMario.burnCoinJumpPending = sMario.crouchSlideBackflip = TRUE;
    sMario.zBPressed = sMario.zCrouchBuffered = TRUE;
    sMario.bullyKnockbackBuffer = INPUT_A_PRESSED | INPUT_B_PRESSED;
    sController.buttonDown = sController.buttonPressed =
        A_BUTTON | B_BUTTON | Z_TRIG | L_TRIG | START_BUTTON;
    sController.buttonReleased = B_BUTTON;
    sController.stickX = sController.stickY = sController.stickMag = 64.0f;
    sTwirlQueued = TRUE;
    sDispatches = sNewActions = sHealthUpdates = sGeometryUpdates = sBlockedErrors = 0;
    sKillOnContact = sKillOnDispatch = FALSE;
    sRestoreHealth = FALSE;
    sLandDrown = sShallowDrown = sShellMusicStops = FALSE;
    sDialog = -1;
    sDelayedWarpOp = WARP_OP_NONE;
    gWarpTransition.isActive = FALSE;
}
static int dead_controls(void) {
    const u32 actions[] = { ACT_IDLE,           ACT_WALKING,      ACT_FREEFALL, ACT_WATER_IDLE,
                            ACT_STANDING_DEATH, ACT_HOLDING_POLE, ACT_PUNCHING };
    const s16 health[] = { -1, 0, 0x80, 0xFF };
    for (u32 a = 0; a < sizeof(actions) / sizeof(actions[0]); a++) {
        for (u32 h = 0; h < sizeof(health) / sizeof(health[0]); h++) {
            begin(actions[a], health[h]);
            sController.buttonDown |= U_JPAD; // Debug movement is also a new action.
            struct Controller saved = sController;
            for (s32 cycle = 0; cycle < 3; cycle++) {
                execute_mario_action(NULL);
                EXPECT_EQ(sNewActions, 0);
                EXPECT_EQ(sBlockedErrors, 0);
                EXPECT_EQ(sMario.controller == &sController, TRUE);
                EXPECT_EQ(memcmp(&saved, &sController, sizeof(saved)), 0);
                EXPECT_EQ(sMario.action, actions[a]);
                EXPECT_EQ(sMario.actionTimer, cycle + 1);
                EXPECT_EQ(sMario.pos[0], 12 * (cycle + 1));
                EXPECT_EQ(sHealthUpdates, cycle + 1);
                EXPECT_EQ(sGeometryUpdates, cycle + 1);
            }
        }
    }
    begin(ACT_FREEFALL, 0x100);
    execute_mario_action(NULL);
    EXPECT_EQ(sNewActions > 0, TRUE);
    EXPECT_EQ(sMario.controller == &sController, TRUE);
    return TRUE;
}
static int health_restored_controls(void) {
    begin(ACT_IDLE, 0xFF);
    sRestoreHealth = TRUE;
    struct Controller saved = sController;
    execute_mario_action(NULL);
    EXPECT_EQ(sMario.health, 0x880);
    EXPECT_EQ(sNewActions > 0, TRUE);
    EXPECT_EQ(sMario.input & INPUT_A_PRESSED, INPUT_A_PRESSED);
    EXPECT_EQ(sMario.landingJumpInputFrames, 2);
    EXPECT_EQ(sMario.controller == &sController, TRUE);
    EXPECT_EQ(memcmp(&saved, &sController, sizeof(saved)), 0);
    return TRUE;
}

static int fatal_redispatch(void) {
    begin(ACT_FREEFALL, 0x100);
    sKillOnContact = TRUE;
    execute_mario_action(NULL);
    // The shortcut before contact was live; the shortcut after fatal contact must be blocked.
    EXPECT_EQ(sNewActions, 1);
    EXPECT_EQ(sBlockedErrors, 0);
    begin(ACT_FREEFALL, 0x100);
    sController.buttonPressed &= ~L_TRIG;
    sKillOnDispatch = TRUE;
    execute_mario_action(NULL);
    EXPECT_EQ(sDispatches, 2);
    EXPECT_EQ(sNewActions, 0);
    EXPECT_EQ(sBlockedErrors, 0);
    EXPECT_EQ(sMario.action, ACT_HARD_BACKWARD_GROUND_KB);
    EXPECT_EQ(sMario.controller == &sController, TRUE);
    begin(ACT_FREEFALL, 0xFF);
    sMario.floor = NULL;
    execute_mario_action(NULL);
    EXPECT_EQ(sMario.controller == &sController, TRUE);
    begin(ACT_UNINITIALIZED, 0xFF);
    execute_mario_action(NULL);
    EXPECT_EQ(sMario.controller == &sController, TRUE);
    return TRUE;
}
static int death_pause(void) {
    begin(ACT_STANDING_DEATH, 0xFF);
    execute_mario_action(NULL);
    EXPECT_EQ(pressed_pause(), TRUE);
    sDelayedWarpOp = WARP_OP_DEATH;
    EXPECT_EQ(pressed_pause(), TRUE);
    gWarpTransition.isActive = TRUE;
    EXPECT_EQ(pressed_pause(), FALSE);
    gWarpTransition.isActive = FALSE;
    sDialog = 1;
    EXPECT_EQ(pressed_pause(), FALSE);
    sDialog = -1;
    sDelayedWarpOp = WARP_OP_WARP_DOOR;
    EXPECT_EQ(pressed_pause(), FALSE);
    sDelayedWarpOp = WARP_OP_NONE;
    sController.buttonPressed = 0;
    EXPECT_EQ(pressed_pause(), FALSE);
    begin(ACT_STANDING_DEATH, 0x100);
    EXPECT_EQ(pressed_pause(), FALSE);
    begin(ACT_WALKING, 0x100);
    EXPECT_EQ(pressed_pause(), TRUE);
    return TRUE;
}
static int drowning_releases_shell(void) {
    for (s32 shallow = FALSE; shallow <= TRUE; shallow++) {
        begin(ACT_RIDING_SHELL_GROUND, 0xFF);
        struct Object shell = { 0 };
        sMario.riddenObj = &shell;
        sLandDrown = TRUE;
        sShallowDrown = shallow;
        execute_mario_action(NULL);
        EXPECT_EQ(sMario.action, shallow ? ACT_QUICKSAND_DEATH : ACT_DROWNING);
        EXPECT_EQ(sMario.riddenObj == NULL, TRUE);
        EXPECT_EQ(shell.oInteractStatus, INT_STATUS_STOP_RIDING);
        EXPECT_EQ(sShellMusicStops, 1);
        EXPECT_EQ(sBlockedErrors, 0);
    }
    begin(ACT_WALKING, 0xFF);
    sLandDrown = TRUE;
    execute_mario_action(NULL);
    EXPECT_EQ(sMario.action, ACT_DROWNING);
    EXPECT_EQ(sShellMusicStops, 0);
    EXPECT_EQ(sBlockedErrors, 0);
    begin(ACT_RIDING_SHELL_GROUND, 0x100);
    struct Object shell = { 0 };
    sMario.riddenObj = &shell;
    sLandDrown = TRUE;
    execute_mario_action(NULL);
    EXPECT_EQ(sMario.action, ACT_RIDING_SHELL_GROUND);
    EXPECT_EQ(sMario.riddenObj == &shell, TRUE);
    EXPECT_EQ(shell.oInteractStatus, INT_STATUS_NONE);
    EXPECT_EQ(sShellMusicStops, 0);
    return TRUE;
}
int main(int argc, char **argv) {
    test_init(argc, argv);
    if (test_selected("zero_health.dead_controls")) {
        test_report("zero_health.dead_controls", dead_controls());
    }
    if (test_selected("zero_health.fatal_redispatch")) {
        test_report("zero_health.fatal_redispatch", fatal_redispatch());
    }
    if (test_selected("zero_health.health_restored_controls")) {
        test_report("zero_health.health_restored_controls", health_restored_controls());
    }
    if (test_selected("zero_health.death_pause")) {
        test_report("zero_health.death_pause", death_pause());
    }
    if (test_selected("zero_health.drowning_releases_shell")) {
        test_report("zero_health.drowning_releases_shell", drowning_releases_shell());
    }
    return test_finish();
}
