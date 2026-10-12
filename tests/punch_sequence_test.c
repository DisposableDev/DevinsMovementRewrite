/* Host boundary doubles for the extracted game functions. This checks action
 * and animation timing, not N64 rendering, real collision geometry or audio. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "test_support.h"

typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;
typedef float f32;
typedef f32 Vec3f[3];
typedef s32 ModelID32;
typedef u32 BehaviorScript;
#define SLOPE_BUFFER
#define TRUE 1
#define FALSE 0
#define BIT(n) (1U << (n))
#define GET_HIGH_S16_OF_32(v) ((s16) ((u32) (v) >> 16))
#define SET_HIGH_S16_OF_32(v, h) ((v) = (s32) (((u32) (v) & 0xffff) | ((u32) (u16) (h) << 16)))
#define UNUSED
#define FALL_THROUGH __attribute__((fallthrough));
#define COS73 0.29237169f
#define WATER_SURFACE_CAMERA_MODE 1
#define INT_STATUS_STOP_RIDING 1
#define sqr(x) ((x) * (x))
#define SOUND_MARIO_PUNCH_YAH 1
#define SOUND_MARIO_PUNCH_WAH 2
#define SOUND_MARIO_PUNCH_HOO 3
#define PUNCH_STATE_TYPE_FIRST_PUNCH (0 << 6)
#define PUNCH_STATE_TYPE_SECOND_PUNCH (1 << 6)
#define PUNCH_STATE_TYPE_KICK (2 << 6)
#define INT_SUBTYPE_GRABS_MARIO 1
#define INT_SUBTYPE_DROP_IMMEDIATELY (1 << 6)
#define CLAMP(value, min, max) ((value) < (min) ? (min) : (value) > (max) ? (max) : (value))

struct Animation {
    s16 flags, startFrame, loopStart, loopEnd;
};
#include "punch_definitions.inc"
struct AnimInfo {
    s16 animID, animFrame;
    u16 animTimer;
    s32 animAccel, animFrameAccelAssist;
    struct Animation *curAnim;
};
static u16 gAreaUpdateCounter;
struct Object {
    struct {
        struct {
            struct AnimInfo animInfo;
            f32 cameraToObject[3];
            s16 angle[3];
        } gfx;
    } header;
    const BehaviorScript *behavior;
    s32 oMoveAngleYaw, oBowserGrabCooldown, oHeldState, oAction;
    u32 oInteractionSubtype;
    u32 oBehParams, oInteractType, oInteractStatus;
    s32 oOpacity, oTimer, activeFlags, oFaceAnglePitch, oFaceAngleRoll, oFaceAngleYaw;
    f32 oPosX, oPosY, oPosZ, oVelY;
};
struct Surface {
    s16 type;
    struct {
        f32 x, y, z;
    } normal;
};
static struct Surface gWaterSurfacePseudoFloor;
struct Controller {
    f32 stickMag;
    u32 buttonDown;
};
struct Camera {
    s32 mode, defMode;
};
struct Area {
    struct Camera *camera;
};
struct MarioBodyState {
    s32 punchState;
};
struct MarioState {
    u32 forcedCapFlags, naturalCapFlags;
    u8 framesSinceA;
    u8 objectRecoveryAFrames, objectRecoveryBFrames;
    u8 automaticAFrames, automaticBFrames, automaticZFrames, groundPoundInputFrames;
    u8 airTwirlInputFrames, landingJumpBufferedEdge;
    u8 rolloutFrames;
    u8 waterWalkTwirl;
    u8 wallBonkMomentum;
    s32 backwardSlopeSlideCount, backwardButtSlideJumpCount;
    f32 slopeJumpKickImpactSpeed;
    u32 action, prevAction, actionArg, actionState, actionTimer, flags, input, particleFlags;
    s32 wallKickChainCount, wallKickFromDoubleJump, doubleJumpTimer;
    s32 bullyKnockbackActive, bullyKnockbackBuffer, bullyKnockbackImmunityFrames;
    s32 heaveHoThrowPendingLanding, zHeldFrames, zCrouchBuffered;
    s32 groundTakeoffThisFrame, windCatchActive, wallKickContactSoundPlayed;
    u8 backwardLongJumpChain, backwardLongJumpCrouchFrames;
    s32 landingJumpInputFrames, wallKickInputFrames, burnCoinJumpPending;
    s32 capTimer;
    s32 squishTimer, waterLevel, health;
    u8 crouchSlideNeutral, crouchSlideFrames, crouchSlideBackflip, rolloutWallKickFrames;
    f32 forwardVel, intendedMag, slideVelX, slideVelZ, pos[3], floorHeight, vel[3];
    s16 intendedYaw, slideYaw, faceAngle[3], angleVel[3];
    struct Object *marioObj;
    struct Object *heldObj;
    struct Area *area;
    struct Surface *floor;
    struct Controller *controller;
    struct MarioBodyState *marioBodyState;
};

u32 set_mario_action(struct MarioState *m, u32 action, u32 arg);
s32 mario_is_head_underwater(struct MarioState *m);
s32 check_common_idle_cancels(struct MarioState *m);
s32 set_water_plunge_action(struct MarioState *m);
s32 mario_has_land_movement_underwater(struct MarioState *m);
static s32 mario_is_riding_underwater_shell(struct MarioState *m) {
    return FALSE;
}
static s32 mario_transition_underwater_shell_to_swimming(struct MarioState *m) {
    return FALSE;
}
static void set_camera_mode(struct Camera *camera, s32 mode, s32 frames) {
    camera->mode = mode;
}
static void vec3_zero(s16 *v) {
    v[0] = v[1] = v[2] = 0;
}
static void unsupported(const char *service) {
    fprintf(stderr, "HARNESS ERROR: unsupported punch service: %s\n", service);
    exit(2);
}
static void load_level_init_text(s32 index) {
    unsupported("spawn landing");
}
static void update_mario_sound_and_camera(struct MarioState *m) {
}
static s32 mario_ground_twirl_is_active(struct MarioState *m) {
    unsupported("ground twirl");
    return FALSE;
}
void apply_punch_ground_drag(struct MarioState *m);
static const BehaviorScript bhvBowser[1] = { 1 };
static s32 obj_has_behavior(struct Object *obj, const BehaviorScript *behavior) {
    return obj->behavior == behavior;
}
static s16 mario_obj_angle_to_object(struct MarioState *m, struct Object *obj) {
    return (s16) (atan2f(obj->oPosX - m->pos[0], obj->oPosZ - m->pos[2]) * 32768.0f
                  / 3.14159265358979323846f);
}
u32 able_to_grab_object(struct MarioState *m, struct Object *obj);
static int sGroundResult, sGrabRequested, sGroundSteps;
static int sSimulateGroundTravel;
static int sEventCount, sEvents[128], sKickSoundCount;
static f32 sLastLunge, sLastDecel;
static const BehaviorScript bhvReleasedPowerUpCap[] = { 0 };
static struct Object sReleasedCap, *o;
static int sCapSpawns, sCapMusicStops, sCapSpawnFails;
static ModelID32 sCapModel;
static const BehaviorScript *sCapBehavior;

static struct Object *try_to_spawn_object(s16 offset, f32 scale, struct Object *parent, ModelID32 model,
                                          const BehaviorScript *behavior) {
    sCapSpawns++;
    sCapModel = model;
    sCapBehavior = behavior;
    memset(&sReleasedCap, 0, sizeof(sReleasedCap));
    sReleasedCap.activeFlags = 1;
    return sCapSpawnFails ? NULL : &sReleasedCap;
}
static int sPunchExplosions;
void mario_he_thicc_attack_impact(struct MarioState *m) {
    sPunchExplosions++;
}
static void stop_cap_music(void) {
    sCapMusicStops++;
}
static void obj_mark_for_deletion(struct Object *obj) {
    obj->activeFlags = 0;
}
static s16 set_mario_animation(struct MarioState *m, s32 id);
s32 mario_has_dive_input(struct MarioState *m);
static int sCrouchSteps;
static void stationary_ground_step(struct MarioState *m) {
    sCrouchSteps++;
    m->forwardVel = 0.0f;
}
static f32 coss(s16 a) {
    return cosf(a * (6.28318530718f / 65536));
}
static f32 sins(s16 a) {
    return sinf(a * (6.28318530718f / 65536));
}
static s16 atan2s(f32 z, f32 x) {
    return (s16) (atan2f(x, z) * (65536.0f / 6.28318530718f));
}
static int sRealSliding;
s32 real_update_sliding(struct MarioState *m, f32 stopSpeed);
static s32 update_sliding(struct MarioState *m, f32 stopSpeed) {
    return sRealSliding ? real_update_sliding(m, stopSpeed) : FALSE;
}
static s32 mario_get_floor_class(struct MarioState *m) {
    return SURFACE_CLASS_DEFAULT;
}
static u32 mario_update_moving_sand(struct MarioState *m) {
    return FALSE;
}
static u32 mario_update_windy_ground(struct MarioState *m) {
    return FALSE;
}
static void common_slide_action(struct MarioState *m, u32 stop, u32 air, s32 anim) {
    sCrouchSteps++;
    set_mario_animation(m, anim);
}
static void tilt_body_butt_slide(struct MarioState *m) {
}

static f32 absf(f32 value) {
    return fabsf(value);
}
static s32 abs_angle_diff(s16 a, s16 b) {
    s32 d = (s16) (a - b);
    return d < 0 ? -d : d;
}
static s16 set_mario_animation(struct MarioState *m, s32 id);
static void play_sound(s32 sound, f32 *pos) {
}
static void play_mario_action_sound(struct MarioState *m, s32 sound, s32 particle) {
    if (!(m->flags & MARIO_ACTION_SOUND_PLAYED)) {
        if (sound == SOUND_MARIO_PUNCH_HOO) {
            sKickSoundCount++;
        }
        m->flags |= MARIO_ACTION_SOUND_PLAYED;
    }
}
static void mario_ground_twirl_reset_wall_bounces(void) {
}
static int mario_is_knocked_back(u32 action) {
    return action == ACT_BACKWARD_AIR_KB;
}
static void mario_drop_held_object(struct MarioState *m) {
}
static void mario_set_forward_vel(struct MarioState *m, f32 vel) {
    m->forwardVel = sLastLunge = vel;
}
static void apply_slope_accel(struct MarioState *m) {
    if (sSimulateGroundTravel) {
        m->vel[0] = m->forwardVel * sins(m->faceAngle[1]);
        m->vel[2] = m->forwardVel * coss(m->faceAngle[1]);
    }
}
static void apply_slope_decel(struct MarioState *m, f32 decel) {
    sLastDecel = decel;
    if (sSimulateGroundTravel) {
        m->forwardVel = fmaxf(0.0f, m->forwardVel - 3.0f * decel);
        apply_slope_accel(m);
    }
}
static s32 perform_ground_step(struct MarioState *m) {
    sGroundSteps++;
    if (sSimulateGroundTravel) {
        m->pos[0] += m->vel[0];
        m->pos[2] += m->vel[2];
    }
    return sGroundResult;
}
static s32 coasting_ground_step(struct MarioState *m) {
    stationary_ground_step(m);
    return sGroundResult;
}
static s32 should_begin_sliding(struct MarioState *m) {
    return FALSE;
}
static s32 mario_floor_is_slope(struct MarioState *m) {
    return m->floor->normal.y < 0.95f;
}
static s32 check_ledge_climb_down(struct MarioState *m, s32 arg) {
    return FALSE;
}
static s32 check_crouch_slide_ledge_grab(struct MarioState *m) {
    return FALSE;
}
static void update_walking_speed(struct MarioState *m) {
}
static void align_with_floor(struct MarioState *m) {
}
static s16 set_mario_anim_with_accel(struct MarioState *m, s32 id, s32 accel) {
    struct AnimInfo *info = &m->marioObj->header.gfx.animInfo;
    s32 changed = info->animID != id;
    set_mario_animation(m, id);
    if (changed) {
        info->animFrameAccelAssist = info->curAnim->startFrame * 0x10000;
        if (!(info->curAnim->flags & ANIM_FLAG_NO_ACCEL)) {
            info->animFrameAccelAssist += (info->curAnim->flags & ANIM_FLAG_FORWARD) ? accel : -accel;
        }
        info->animFrame = GET_HIGH_S16_OF_32(info->animFrameAccelAssist);
    }
    info->animAccel = accel;
    return info->animFrame;
}
static void play_step_sound(struct MarioState *m, s32 a, s32 b) {
}
static s32 mario_check_punch_lunge_grab(struct MarioState *m) {
    return FALSE;
}
static u32 drop_and_set_mario_action(struct MarioState *m, u32 action, u32 arg) {
    m->heldObj = NULL;
    return set_mario_action(m, action, arg);
}
static u32 set_jumping_action(struct MarioState *m, u32 action, u32 arg) {
    return set_mario_action(m, action, arg);
}
static u32 mario_push_off_steep_floor(struct MarioState *m, u32 action, u32 arg) {
    return set_mario_action(m, action, arg);
}
#define ACTION_SETUP(name)                                                                             \
    static u32 name(struct MarioState *m, u32 action, u32 arg) {                                       \
        return action;                                                                                 \
    }
ACTION_SETUP(set_mario_action_moving)
ACTION_SETUP(set_mario_action_airborne)
ACTION_SETUP(set_mario_action_submerged)
ACTION_SETUP(set_mario_action_cutscene)

static s32 mario_check_object_grab(struct MarioState *m) {
    struct Object obj = { 0 };
    if (sGrabRequested && able_to_grab_object(m, &obj)) {
        return set_mario_action(m, ACT_PICKING_UP, 0);
    }
    return FALSE;
}

static s16 set_mario_animation(struct MarioState *m, s32 id) {
    struct AnimInfo *info = &m->marioObj->header.gfx.animInfo;
    if (info->animID != id) {
        info->animID = id;
        info->curAnim = &sAnimations[id];
        info->animAccel = 0;
        info->animFrame = info->curAnim->startFrame;
        if (!(info->curAnim->flags & 4)) {
            info->animFrame += (info->curAnim->flags & 2) ? 1 : -1;
        }
        if (id == MARIO_ANIM_FIRST_PUNCH || id == MARIO_ANIM_SECOND_PUNCH
            || id == MARIO_ANIM_GROUND_KICK) {
            if (sEventCount < 128) {
                sEvents[sEventCount++] = id;
            }
        }
    }
    return info->animFrame;
}

#include "punch_sources.inc"

static struct MarioState sMario;
static struct Object sMarioObject, sTarget;
static struct Controller sController;
static struct Camera sCamera;
static struct Area sArea = { &sCamera };
static struct MarioBodyState sBody;
static struct Surface sFloor;
static int sBWasDown, sTransitionCount;
static struct {
    u32 from, to;
    s16 frame;
} sTransitions[128];
static const char *sActiveCase;
static char sScenario[120];
static int sFrame, sFailurePrinted;
static struct {
    int frame;
    u32 input, action, arg, nextAction, nextArg;
    s16 anim, animFrame, nextAnim, nextFrame;
    f32 speed;
} sTrace[16];

static const char *animation_name(int id) {
    switch (id) {
        case MARIO_ANIM_FIRST_PUNCH:
            return "punch-1";
        case MARIO_ANIM_SECOND_PUNCH:
            return "punch-2";
        case MARIO_ANIM_FIRST_PUNCH_FAST:
            return "recovery-1";
        case MARIO_ANIM_SECOND_PUNCH_FAST:
            return "recovery-2";
        case MARIO_ANIM_GROUND_KICK:
            return "kick";
        default:
            return "other";
    }
}

static const char *action_name(u32 action) {
    switch (action) {
        case ACT_PUNCHING:
            return "standing-punch";
        case ACT_MOVE_PUNCHING:
            return "moving-punch";
        case ACT_IDLE:
            return "idle";
        case ACT_WALKING:
            return "walking";
        case ACT_JUMP:
            return "jump";
        case ACT_FREEFALL:
            return "freefall";
        case ACT_PICKING_UP:
            return "grab";
        default:
            return "other";
    }
}

static void failure_trace(void) {
    if (sFailurePrinted) {
        return;
    }
    sFailurePrinted = TRUE;
    struct AnimInfo *info = &sMarioObject.header.gfx.animInfo;
    fprintf(stderr, "Case %s (%s) at tick %d: %s arg=%u, %s frame=%d, speed=%.2f\n", sActiveCase,
            sScenario, sFrame, action_name(sMario.action), sMario.actionArg,
            animation_name(info->animID), info->animFrame, sMario.forwardVel);
    fprintf(stderr, "Recent ticks: B/down+press, input mask, action/arg, animation/frame -> result\n");
    for (int i = sFrame > 16 ? sFrame - 16 : 0; i < sFrame; i++) {
        int n = i % 16;
        fprintf(stderr, "  %d B=%d/%d input=%04x %s/%u %s/%d -> %s/%u %s/%d speed=%.2f\n",
                sTrace[n].frame, !!(sTrace[n].input & INPUT_B_DOWN),
                !!(sTrace[n].input & INPUT_B_PRESSED), sTrace[n].input, action_name(sTrace[n].action),
                sTrace[n].arg, animation_name(sTrace[n].anim), sTrace[n].animFrame,
                action_name(sTrace[n].nextAction), sTrace[n].nextArg,
                animation_name(sTrace[n].nextAnim), sTrace[n].nextFrame, sTrace[n].speed);
    }
}

#define CHECK(condition)                                                                               \
    do {                                                                                               \
        if (!(condition)) {                                                                            \
            fprintf(stderr, "%s:%d: expected %s\n", __func__, __LINE__, #condition);                   \
            failure_trace();                                                                           \
            return FALSE;                                                                              \
        }                                                                                              \
    } while (0)
#define CHECK_EQ(actual, expected)                                                                     \
    do {                                                                                               \
        long long got_ = (long long) (actual), want_ = (long long) (expected);                         \
        if (got_ != want_) {                                                                           \
            fprintf(stderr, "%s:%d: %s: expected %lld, observed %lld\n", __func__, __LINE__, #actual,  \
                    want_, got_);                                                                      \
            failure_trace();                                                                           \
            return FALSE;                                                                              \
        }                                                                                              \
    } while (0)

static int punching(void) {
    return sMario.action == ACT_PUNCHING || sMario.action == ACT_MOVE_PUNCHING;
}
static void begin(u32 action, f32 speed) {
    memset(&sMario, 0, sizeof(sMario));
    memset(&sMarioObject, 0, sizeof(sMarioObject));
    memset(&sController, 0, sizeof(sController));
    sFloor.normal.x = sFloor.normal.z = 0;
    sFloor.normal.y = 1;
    sMario.marioObj = &sMarioObject;
    sMario.marioBodyState = &sBody;
    sMario.floor = &sFloor;
    sMario.controller = &sController;
    sMario.area = &sArea;
    sCamera.mode = sCamera.defMode = 0;
    sMario.waterLevel = -10000;
    sMario.health = 0x880;
    sMario.forwardVel = speed;
    sMarioObject.header.gfx.animInfo.animID = -1;
    sMario.action = ACT_IDLE;
    sEventCount = sTransitionCount = sBWasDown = sGrabRequested = sKickSoundCount = 0;
    sCapSpawns = sCapMusicStops = sCapSpawnFails = sPunchExplosions = 0;
    sFrame = sFailurePrinted = 0;
    strcpy(sScenario, "default");
    sGroundResult = GROUND_STEP_NONE;
    sGroundSteps = 0;
    sCrouchSteps = 0;
    sSimulateGroundTravel = 0;
    sRealSliding = 0;
    mario_set_punch_hit_phase(0);
    set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_YAH);
}

static void tick(int bDown, u32 extraInput) {
    struct AnimInfo *info = &sMarioObject.header.gfx.animInfo;
    u32 beforeArg = sMario.actionArg;
    s16 beforeFrame = info->animFrame;
    sMario.input =
        extraInput | (bDown ? INPUT_B_DOWN : 0) | (bDown && !sBWasDown ? INPUT_B_PRESSED : 0);
    sBWasDown = bDown;
    int n = sFrame % 16;
    sTrace[n].frame = sFrame;
    sTrace[n].input = sMario.input;
    sTrace[n].action = sMario.action;
    sTrace[n].arg = beforeArg;
    sTrace[n].anim = info->animID;
    sTrace[n].animFrame = beforeFrame;
    sMario.flags &= ~(MARIO_PUNCHING | MARIO_KICKING | MARIO_TRIPPING);
    if (sMario.action == ACT_PUNCHING) {
        act_punching(&sMario);
    } else if (sMario.action == ACT_MOVE_PUNCHING) {
        act_move_punching(&sMario);
    }
    sTrace[n].nextAction = sMario.action;
    sTrace[n].nextArg = sMario.actionArg;
    sTrace[n].nextAnim = info->animID;
    sTrace[n].nextFrame = info->animFrame;
    sTrace[n].speed = sMario.forwardVel;
    sFrame++;
    if (punching() && beforeArg != sMario.actionArg && sTransitionCount < 128) {
        sTransitions[sTransitionCount].from = beforeArg;
        sTransitions[sTransitionCount].to = sMario.actionArg;
        sTransitions[sTransitionCount++].frame = beforeFrame;
    }
    // All six tested assets use ordinary forward, non-looping playback.
    if (info->curAnim && info->animFrame < info->curAnim->loopEnd - 1) {
        info->animFrame++;
    }
}

static int reach(u32 arg, int frame, int bDown) {
    for (int i = 0; i < 100 && punching(); i++) {
        if (sMario.actionArg == arg && sMarioObject.header.gfx.animInfo.animFrame == frame) {
            return TRUE;
        }
        tick(bDown, 0);
    }
    fprintf(stderr, "Could not reach punch phase %u at animation frame %d within 100 ticks\n", arg,
            frame);
    return FALSE;
}

static int arm_repeat(int pair) {
    u32 arg = pair ? ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH_FAST : ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST;
    CHECK(reach(arg, 1, TRUE));
    tick(FALSE, 0);
    tick(TRUE, 0);
    return TRUE;
}

static int expect_events(const int *events, int count) {
    for (int i = 0; i < 400 && punching() && sEventCount < count; i++) {
        tick(TRUE, 0);
    }
    int matches = sEventCount == count;
    for (int i = 0; i < count && i < sEventCount; i++) {
        matches &= sEvents[i] == events[i];
    }
    if (!matches) {
        fprintf(stderr, "Expected sequence:");
        for (int i = 0; i < count; i++) {
            fprintf(stderr, " %s", animation_name(events[i]));
        }
        fprintf(stderr, "\nObserved sequence:");
        for (int i = 0; i < sEventCount; i++) {
            fprintf(stderr, " %s", animation_name(sEvents[i]));
        }
        fprintf(stderr, "\n");
        failure_trace();
        return FALSE;
    }
    return TRUE;
}

static int combo(u32 action) {
    const int events[] = { MARIO_ANIM_FIRST_PUNCH, MARIO_ANIM_SECOND_PUNCH, MARIO_ANIM_GROUND_KICK,
                           MARIO_ANIM_FIRST_PUNCH, MARIO_ANIM_SECOND_PUNCH, MARIO_ANIM_GROUND_KICK,
                           MARIO_ANIM_FIRST_PUNCH, MARIO_ANIM_SECOND_PUNCH, MARIO_ANIM_GROUND_KICK };
    begin(action, 0);
    CHECK(expect_events(events, 9));
    CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK, sAnimations[MARIO_ANIM_GROUND_KICK].loopEnd - 1,
                TRUE));
    tick(FALSE, 0);
    CHECK(sMario.action == (action == ACT_PUNCHING ? ACT_IDLE : ACT_WALKING));
    CHECK(sEventCount == 9);
    CHECK_EQ(sKickSoundCount, 3);   // The first kick and both held-B repeats each speak once.
    CHECK(sMario.actionState == 0); // Finishing must not start another lunge.
    return TRUE;
}

static int singles(u32 action) {
    const int events[] = { MARIO_ANIM_FIRST_PUNCH, MARIO_ANIM_FIRST_PUNCH, MARIO_ANIM_FIRST_PUNCH,
                           MARIO_ANIM_FIRST_PUNCH };
    begin(action, 0);
    CHECK(arm_repeat(FALSE));
    CHECK(expect_events(events, 4));
    int boundaries = 0;
    for (int i = 0; i < sTransitionCount; i++) {
        if (sTransitions[i].from == ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST) {
            CHECK(sTransitions[i].frame == sAnimations[MARIO_ANIM_FIRST_PUNCH_FAST].loopEnd - 1);
            boundaries++;
        }
    }
    CHECK(boundaries == 3);
    return TRUE;
}

static int pairs(u32 action) {
    const int events[] = { MARIO_ANIM_FIRST_PUNCH,  MARIO_ANIM_SECOND_PUNCH, MARIO_ANIM_FIRST_PUNCH,
                           MARIO_ANIM_SECOND_PUNCH, MARIO_ANIM_FIRST_PUNCH,  MARIO_ANIM_SECOND_PUNCH,
                           MARIO_ANIM_FIRST_PUNCH };
    begin(action, 0);
    CHECK(arm_repeat(TRUE));
    CHECK(expect_events(events, 7));
    int boundaries = 0;
    for (int i = 0; i < sTransitionCount; i++) {
        if (sTransitions[i].from == ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH_FAST) {
            CHECK(sTransitions[i].frame == sAnimations[MARIO_ANIM_SECOND_PUNCH_FAST].loopEnd - 1);
            boundaries++;
        } else if (sTransitions[i].from == ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST) {
            CHECK(sTransitions[i].frame == sAnimations[MARIO_ANIM_FIRST_PUNCH_FAST].loopEnd - 7);
        }
    }
    CHECK(boundaries == 3);
    return TRUE;
}

static int release_and_restart(u32 action) {
    for (int pair = 0; pair <= 1; pair++) {
        begin(action, 0);
        CHECK(arm_repeat(pair));
        int count = sEventCount;
        for (int i = 0; i < 80 && punching(); i++) {
            tick(FALSE, 0);
        }
        CHECK(!punching());
        CHECK(sEventCount == count);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_YAH);
        sEventCount = 0;
        const int events[] = { MARIO_ANIM_FIRST_PUNCH, MARIO_ANIM_SECOND_PUNCH,
                               MARIO_ANIM_GROUND_KICK };
        CHECK(expect_events(events, 3));
    }
    return TRUE;
}

static int interruptions(u32 action) {
    const u32 targets[] = { ACT_BACKWARD_AIR_KB, ACT_FREEFALL, ACT_PICKING_UP, ACT_JUMP };
    for (unsigned int i = 0; i < sizeof(targets) / sizeof(targets[0]); i++) {
        begin(action, 0);
        CHECK(arm_repeat(TRUE));
        set_mario_action(&sMario, targets[i], 0);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_YAH);
        sEventCount = 0;
        const int events[] = { MARIO_ANIM_FIRST_PUNCH, MARIO_ANIM_SECOND_PUNCH,
                               MARIO_ANIM_GROUND_KICK };
        CHECK(expect_events(events, 3));
    }
    return TRUE;
}

static int cancel_and_stick(u32 action) {
    const u32 inputs[] = { INPUT_A_PRESSED, INPUT_Z_DOWN, INPUT_FIRST_PERSON };
    const u32 targets[] = { ACT_JUMP, ACT_CROUCH_SLIDE, ACT_FIRST_PERSON };
    for (int pair = 0; pair <= 1; pair++) {
        for (int i = 0; i < 3; i++) {
            begin(action, 0);
            CHECK(arm_repeat(pair));
            u32 arg = pair ? ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH_FAST
                           : ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST;
            int anim = pair ? MARIO_ANIM_SECOND_PUNCH_FAST : MARIO_ANIM_FIRST_PUNCH_FAST;
            CHECK(reach(arg, sAnimations[anim].loopEnd - 6, TRUE));
            sMario.zHeldFrames = 2;
            tick(TRUE, inputs[i]);
            CHECK(sMario.action == targets[i]);
        }
        begin(action, 0);
        CHECK(arm_repeat(pair));
        int count = sEventCount;
        for (int i = 0; i < 40 && sEventCount == count && punching(); i++) {
            tick(TRUE, INPUT_NONZERO_ANALOG);
        }
        CHECK(punching());
        CHECK(sEventCount == count + 1);
        CHECK(sEvents[count] == MARIO_ANIM_FIRST_PUNCH);
    }
    return TRUE;
}

static int lunge_and_grab(u32 action) {
    begin(action, 0);
    CHECK(arm_repeat(FALSE));
    CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH, 2, TRUE));
    CHECK(able_to_grab_object(&sMario, &sTarget));
    tick(TRUE, 0);
    CHECK(fabsf(sLastLunge - 15.0f) < 0.001f);
    remember_punch_hit_object(&sTarget);
    sPunchContacts.hitWallCount = 1;
    CHECK(punch_hit_object(&sTarget));
    CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST, 1, TRUE));
    CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH, 2, TRUE));
    CHECK(!punch_hit_object(&sTarget));
    CHECK(sPunchContacts.hitWallCount == 0);
    sGrabRequested = TRUE;
    tick(TRUE, 0);
    CHECK(sMario.action == ACT_PICKING_UP);

    begin(action, 20.0f);
    CHECK(arm_repeat(FALSE));
    CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH, 2, TRUE));
    tick(TRUE, 0);
    CHECK(sMario.actionState == ACT_STATE_PUNCH_LUNGE_INACTIVE);
    CHECK(sMario.forwardVel == 20.0f);

    begin(action, 0);
    CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH, 1, TRUE));
    CHECK(!able_to_grab_object(&sMario, &sTarget));
    tick(TRUE, 0);
    CHECK(fabsf(sLastLunge - 22.5f) < 0.001f);

    begin(action, 0);
    CHECK(arm_repeat(TRUE));
    CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH, 1, TRUE));
    tick(TRUE, 0);
    CHECK(fabsf(sLastLunge - 22.5f) < 0.001f);

    begin(action, 20.0f);
    CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH, 1, TRUE));
    tick(TRUE, 0);
    CHECK(sMario.actionState == ACT_STATE_PUNCH_LUNGE_INACTIVE);
    CHECK(sMario.forwardVel == 20.0f);

    begin(action, 0);
    CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK, 0, TRUE));
    tick(TRUE, 0);
    CHECK(fabsf(sLastLunge - 30.0f) < 0.001f);
    return TRUE;
}

static int moving_start_repeat_lunge(u32 action) {
    begin(action, 20.0f);
    CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK, sAnimations[MARIO_ANIM_GROUND_KICK].loopEnd - 1,
                TRUE));
    CHECK_EQ(sMario.actionState, ACT_STATE_PUNCH_LUNGE_INACTIVE);

    // The full combo has slowed Mario before its held-B restart.
    sMario.forwardVel = 0.0f;
    tick(TRUE, 0);
    CHECK_EQ(sMario.actionArg, ACT_ARG_PUNCH_SEQUENCE_YAH);
    CHECK_EQ(sMario.actionState, ACT_STATE_PUNCH_LUNGE_UNDECIDED);
    CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH, 2, TRUE));
    tick(TRUE, 0);
    CHECK_EQ(sMario.actionState, ACT_STATE_PUNCH_LUNGE_ACTIVE);
    CHECK(fabsf(sLastLunge - 15.0f) < 0.001f);
    return TRUE;
}

static int rebound_keeps_speed(u32 action) {
    begin(action, 0.0f);
    CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH, 3, TRUE));
    tick(TRUE, 0);
    CHECK_EQ(sMario.actionState, ACT_STATE_PUNCH_LUNGE_ACTIVE);

    // Wall contact ends this attack's lunge and supplies backward speed.
    sMario.actionTimer = 0;
    mario_set_forward_vel(&sMario, -16.0f);
    tick(TRUE, 0);
    CHECK(sMario.forwardVel < 0.0f);

    if (action == ACT_MOVE_PUNCHING) {
        begin(action, 0.0f);
        sMario.actionState = ACT_STATE_PUNCH_LUNGE_INACTIVE;
        tick(TRUE, 0);
        CHECK(!(sMario.particleFlags & PARTICLE_DUST));
    }
    return TRUE;
}

static int breakdance_completion_timing(u32 action) {
    const int length = sAnimations[MARIO_ANIM_BREAKDANCE].loopEnd;
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(action, 0.0f);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        tick(FALSE, 0);
        for (int frame = 0; frame < length; frame++) {
            CHECK_EQ(sMarioObject.header.gfx.animInfo.animFrame, frame);
            CHECK_EQ(sMario.action, action);
            tick(FALSE, 0);
            CHECK_EQ(!!(sMario.flags & MARIO_TRIPPING), frame >= 2 && frame < 12);
            if (frame < length - 1) {
                CHECK_EQ(sMario.action, action);
            } else {
                CHECK_EQ(sMario.action, action == ACT_MOVE_PUNCHING ? ACT_CROUCH_SLIDE : ACT_CROUCHING);
            }
        }
    }
    return TRUE;
}
static int breakdance_recovery_windows(u32 action) {
    for (int frame = 11; frame <= 13; frame++) {
        begin(action, 0.0f);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        tick(FALSE, 0);
        sMarioObject.header.gfx.animInfo.animFrame = frame;
        sMario.intendedMag = 32.0f;
        tick(FALSE, INPUT_NONZERO_ANALOG);
        CHECK_EQ(!!(sMario.flags & MARIO_TRIPPING), frame == 11);
        CHECK_EQ(sMario.action, frame == 13 ? ACT_WALKING : action);
    }
    return TRUE;
}

static int breakdance_no_lunge(u32 action) {
    begin(action, 0.0f);
    set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
    sLastLunge = -1.0f;
    for (int i = 0; i < 5; i++) {
        tick(FALSE, 0);
        CHECK(sMario.forwardVel == 0.0f);
        CHECK(sLastLunge == -1.0f);
    }
    CHECK(fabsf(sLastDecel - 0.10f) < 0.001f);

    begin(action, 20.0f);
    tick(FALSE, 0);
    CHECK(fabsf(sLastDecel - 0.7f) < 0.001f);

    begin(action, 20.0f);
    set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK);
    tick(FALSE, 0);
    CHECK(fabsf(sLastDecel - 0.6f) < 0.001f);

    if (action == ACT_MOVE_PUNCHING) {
        begin(action, 6.0f);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        sLastLunge = -1.0f;
        tick(FALSE, 0);
        CHECK(sMario.forwardVel == 6.0f);
        CHECK(sLastLunge == -1.0f);
        CHECK(fabsf(sLastDecel - 0.10f) < 0.001f);

        begin(action, -20.0f);
        tick(FALSE, 0);
        CHECK(fabsf(sMario.forwardVel - -8.8f) < 0.001f);

        begin(action, -20.0f);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK);
        tick(FALSE, 0);
        CHECK(fabsf(sMario.forwardVel - -10.4f) < 0.001f);

        begin(action, -20.0f);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        tick(FALSE, 0);
        CHECK(fabsf(sMario.forwardVel - -18.4f) < 0.001f);
    }
    return TRUE;
}

static int breakdance_steer_and_carry(u32 action) {
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(action, 40.0f);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        sSimulateGroundTravel = TRUE;
        for (int frame = 0; frame < 5; frame++) {
            tick(FALSE, 0);
        }
        CHECK_EQ(sMario.faceAngle[1], 0);
        CHECK(fabsf(sMario.pos[0]) < 0.01f);
        // Both actions use the settled 0.10 drag. This fixture supplies
        // three times that coefficient per travel update, not native physics.
        CHECK(fabsf(sMario.forwardVel - 38.5f) < 0.01f);
        CHECK(fabsf(sMario.pos[2] - 195.5f) < 0.01f);

        begin(action, 40.0f);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        sSimulateGroundTravel = TRUE;
        sMario.intendedMag = 32.0f;
        sMario.intendedYaw = 0x4000;
        for (int frame = 0; frame < 5; frame++) {
            tick(FALSE, INPUT_NONZERO_ANALOG);
        }
        if (action == ACT_MOVE_PUNCHING) {
            CHECK_EQ(sMario.faceAngle[1], 5 * BREAKDANCE_SLIDE_MAX_TURN);
            CHECK(sMario.pos[0] > 10.0f);
            CHECK(sMario.pos[2] > 180.0f);
        } else {
            CHECK_EQ(sMario.faceAngle[1], 0);
            CHECK(fabsf(sMario.pos[0]) < 0.01f);
        }

        // Early recovery into walking keeps the slide's facing. Standing
        // breakdances still use the ordinary idle-to-walk snap.
        begin(action, 40.0f);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        tick(FALSE, 0);
        sMarioObject.header.gfx.animInfo.animFrame =
            sAnimations[MARIO_ANIM_BREAKDANCE].loopEnd - GROUND_KICK_CANCEL_EARLY_FRAMES;
        sMario.intendedMag = 32.0f;
        sMario.intendedYaw = 0x4000;
        tick(FALSE, INPUT_NONZERO_ANALOG);
        CHECK_EQ(sMario.action, ACT_WALKING);
        CHECK_EQ(sMario.faceAngle[1], action == ACT_MOVE_PUNCHING ? 0 : 0x4000);
        CHECK(sMario.forwardVel > 16.0f);
    }

    if (action == ACT_MOVE_PUNCHING) {
        begin(action, 40.0f);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        sSimulateGroundTravel = TRUE;
        sMario.intendedMag = 16.0f;
        sMario.intendedYaw = 0x4000;
        for (int frame = 0; frame < 5; frame++) {
            tick(FALSE, INPUT_NONZERO_ANALOG);
        }
        CHECK_EQ(sMario.faceAngle[1], 5 * BREAKDANCE_SLIDE_MAX_TURN / 2);
        CHECK(sMario.pos[0] > 0.0f && sMario.pos[0] < 15.0f);

        begin(action, 40.0f);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        sSimulateGroundTravel = TRUE;
        sMario.intendedMag = 32.0f;
        sMario.intendedYaw = -0x4000;
        for (int frame = 0; frame < 5; frame++) {
            tick(FALSE, INPUT_NONZERO_ANALOG);
        }
        CHECK_EQ(sMario.faceAngle[1], -5 * BREAKDANCE_SLIDE_MAX_TURN);
        CHECK(sMario.pos[0] < -10.0f);

        begin(action, -40.0f);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        sSimulateGroundTravel = TRUE;
        sMario.intendedMag = 32.0f;
        sMario.intendedYaw = 0x4000;
        for (int frame = 0; frame < 5; frame++) {
            tick(FALSE, INPUT_NONZERO_ANALOG);
        }
        CHECK_EQ(sMario.faceAngle[1], -5 * BREAKDANCE_SLIDE_MAX_TURN);
        CHECK(sMario.pos[0] > 10.0f && sMario.pos[2] < 0.0f);
        CHECK(fabsf(sMario.forwardVel - -32.0f) < 0.01f);

        begin(action, 0.0f);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        sSimulateGroundTravel = TRUE;
        sMario.intendedMag = 32.0f;
        sMario.intendedYaw = 0x4000;
        for (int frame = 0; frame < 5; frame++) {
            tick(FALSE, INPUT_NONZERO_ANALOG);
        }
        CHECK_EQ(sMario.faceAngle[1], 0);
        CHECK(fabsf(sMario.pos[0]) < 0.01f);
    }
    return TRUE;
}

static int attack_steering(u32 action) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int direction = -1; direction <= 1; direction += 2) {
            for (int partial = 0; partial <= 1; partial++) {
                begin(action, 20.0f);
                sSimulateGroundTravel = TRUE;
                sMario.intendedMag = partial ? 16.0f : 32.0f;
                sMario.intendedYaw = direction * 0x4000;
                for (int frame = 0; frame < 100 && punching(); frame++) {
                    s16 facing = sMario.faceAngle[1];
                    tick(TRUE, INPUT_NONZERO_ANALOG);
                    s16 turn = direction * (s16) (sMario.faceAngle[1] - facing);
                    CHECK(turn >= 0 && turn <= 0x80);
                    if (punching()) {
                        CHECK_EQ(turn, partial ? 0x40 : 0x80);
                    }
                }
                CHECK_EQ(sMario.action, ACT_WALKING);
                CHECK_EQ(sMario.actionArg, ACT_ARG_WALKING_TURN_FROM_PUNCH);
                CHECK_EQ(sEventCount, 3);
                CHECK(direction * sMario.faceAngle[1] > 0);
                CHECK(direction * sMario.pos[0] > 0.0f);
                CHECK(sMario.pos[2] > 0.0f);
            }
        }
    }

    begin(action, 20.0f);
    sMario.faceAngle[1] = 0x2000;
    sMario.intendedYaw = -0x4000;
    sMario.intendedMag = 32.0f;
    tick(FALSE, 0);
    CHECK_EQ(sMario.faceAngle[1], 0x2000);

    // A stationary attack can aim before its lunge starts. Backward recoil
    // retains its sign while the attack turns toward the held direction.
    const f32 speeds[] = { 0.0f, -20.0f };
    for (int i = 0; i < 2; i++) {
        begin(action, speeds[i]);
        sSimulateGroundTravel = TRUE;
        sMario.intendedMag = 32.0f;
        sMario.intendedYaw = 0x4000;
        tick(FALSE, INPUT_NONZERO_ANALOG);
        CHECK_EQ(sMario.faceAngle[1], 0x80);
        CHECK(sMario.forwardVel <= 0.0f);
        if (i == 1) {
            CHECK(sMario.forwardVel < 0.0f && sMario.pos[0] < 0.0f);
        }
    }

    begin(action, 20.0f);
    sMario.faceAngle[1] = 0x7FC0;
    sMario.intendedYaw = (s16) 0x8040;
    sMario.intendedMag = 32.0f;
    tick(FALSE, INPUT_NONZERO_ANALOG);
    CHECK_EQ(sMario.faceAngle[1], (s16) 0x8040);
    return TRUE;
}

static int recovery_turn(u32 action) {
    const u32 args[] = { ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST,
                         ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH_FAST, ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK };
    const s32 anims[] = { MARIO_ANIM_FIRST_PUNCH_FAST, MARIO_ANIM_SECOND_PUNCH_FAST,
                          MARIO_ANIM_GROUND_KICK };
    for (int i = 0; i < 3; i++) {
        for (int cycle = 0; cycle < 3; cycle++) {
            begin(action, 0.0f);
            set_mario_action(&sMario, action, args[i]);
            tick(FALSE, 0);
            sMarioObject.header.gfx.animInfo.animFrame = sAnimations[anims[i]].loopEnd - 6;
            sMario.faceAngle[1] = 0x1000;
            sMario.intendedYaw = 0x4000;
            sMario.intendedMag = 32.0f;
            int groundSteps = sGroundSteps;
            tick(FALSE, INPUT_NONZERO_ANALOG);
            CHECK_EQ(sMario.action, ACT_WALKING);
            CHECK_EQ(sMario.actionArg, ACT_ARG_WALKING_TURN_FROM_PUNCH);
            CHECK_EQ(sMario.faceAngle[1], 0x1000);
            CHECK_EQ(sGroundSteps, groundSteps);
            set_mario_action(&sMario, ACT_JUMP, 0);
            CHECK_EQ(sMario.actionArg, 0);

            // Completing without a movement cancel still marks moving recovery.
            begin(action, 0.0f);
            set_mario_action(&sMario, action, args[i]);
            tick(FALSE, 0);
            sMarioObject.header.gfx.animInfo.animFrame = sAnimations[anims[i]].loopEnd - 1;
            tick(FALSE, 0);
            CHECK_EQ(sMario.action, action == ACT_MOVE_PUNCHING ? ACT_WALKING : ACT_IDLE);
            CHECK_EQ(sMario.actionArg,
                     action == ACT_MOVE_PUNCHING ? ACT_ARG_WALKING_TURN_FROM_PUNCH : 0);
        }
    }
    return TRUE;
}

static int breakdance_crawl_return(u32 action) {
    const u32 crawlActions[] = { ACT_START_CRAWLING, ACT_CRAWLING, ACT_STOP_CRAWLING };
    for (int i = 0; i < 3; i++) {
        begin(action, 9.0f);
        set_mario_action(&sMario, crawlActions[i], 0);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        sController.stickMag = MARIO_STRONG_STICK_THRESHOLD - 0.1f;
        for (int frame = 0; frame < 50 && punching(); frame++) {
            tick(FALSE, INPUT_Z_DOWN | INPUT_NONZERO_ANALOG);
        }
        CHECK_EQ(sMario.action, ACT_CRAWLING);

        begin(action, 9.0f);
        set_mario_action(&sMario, crawlActions[i], 0);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        sController.stickMag = MARIO_STRONG_STICK_THRESHOLD;
        for (int frame = 0; frame < 50 && punching(); frame++) {
            tick(FALSE, INPUT_Z_DOWN | INPUT_NONZERO_ANALOG);
        }
        CHECK_EQ(sMario.action, ACT_CRAWLING);
    }

    begin(action, 9.0f);
    set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
    sController.stickMag = MARIO_STRONG_STICK_THRESHOLD;
    for (int frame = 0; frame < 50 && punching(); frame++) {
        tick(FALSE, INPUT_Z_DOWN | INPUT_NONZERO_ANALOG);
    }
    CHECK_EQ(sMario.action, ACT_WALKING);

    begin(action, 9.0f);
    set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
    sMario.prevAction = ACT_CRAWLING;
    for (int frame = 0; frame < 50 && punching(); frame++) {
        tick(FALSE, 0);
    }
    CHECK_EQ(sMario.action, action == ACT_PUNCHING ? ACT_CROUCHING : ACT_CROUCH_SLIDE);
    return TRUE;
}

static int breakdance_crawl_repeat(u32 action) {
    const u32 crawlActions[] = { ACT_START_CRAWLING, ACT_CRAWLING, ACT_STOP_CRAWLING };
    for (int origin = 0; origin < 3; origin++) {
        begin(action, 9.0f);
        set_mario_action(&sMario, crawlActions[origin], 0);
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        tick(FALSE, 0);

        for (int cycle = 0; cycle < 3; cycle++) {
            struct AnimInfo *info = &sMarioObject.header.gfx.animInfo;
            info->animFrame = info->curAnim->loopEnd - 1 - GROUND_KICK_CANCEL_EARLY_FRAMES;
            sPunchContacts.hitWallCount = 1;
            sMario.input = INPUT_B_PRESSED | INPUT_B_DOWN | INPUT_Z_DOWN | INPUT_NONZERO_ANALOG;
            int transitions = 0;
            s32 again = TRUE;
            while (again && transitions < 8) {
                transitions++;
                if (sMario.action == ACT_CRAWLING) {
                    again = act_crawling(&sMario);
                } else if (sMario.action == ACT_PUNCHING) {
                    again = act_punching(&sMario);
                } else if (sMario.action == ACT_MOVE_PUNCHING) {
                    again = act_move_punching(&sMario);
                } else {
                    CHECK(FALSE);
                }
            }
            CHECK_EQ(transitions, 3);
            CHECK_EQ(sMario.action, ACT_MOVE_PUNCHING);
            CHECK_EQ(sMario.actionArg, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
            CHECK(info->animFrame < 2);
            CHECK_EQ(sPunchContacts.hitWallCount, 0);
        }
    }
    return TRUE;
}

static int input_boundaries(u32 action) {
    const int singlesExpected[] = { MARIO_ANIM_FIRST_PUNCH, MARIO_ANIM_FIRST_PUNCH,
                                    MARIO_ANIM_FIRST_PUNCH };
    const int pairsExpected[] = { MARIO_ANIM_FIRST_PUNCH, MARIO_ANIM_SECOND_PUNCH,
                                  MARIO_ANIM_FIRST_PUNCH, MARIO_ANIM_SECOND_PUNCH,
                                  MARIO_ANIM_FIRST_PUNCH };
    for (int pair = 0; pair <= 1; pair++) {
        int recovery = pair ? MARIO_ANIM_SECOND_PUNCH_FAST : MARIO_ANIM_FIRST_PUNCH_FAST;
        u32 recoveryArg =
            pair ? ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH_FAST : ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST;
        u32 attackArg = pair ? ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH : ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH;
        int last = sAnimations[recovery].loopEnd - 1;
        // Before/on/after the ordinary follow-up window, then the final repeat boundary.
        const int repressFrames[] = { last - 7, last - 6, last - 5, last - 1, last };
        for (int i = 0; i < 5; i++) {
            begin(action, 0);
            snprintf(sScenario, sizeof(sScenario), "punch %d re-press on recovery frame %d", pair + 1,
                     repressFrames[i]);
            CHECK(reach(recoveryArg, 0, TRUE));
            CHECK(reach(recoveryArg, repressFrames[i], FALSE));
            tick(TRUE, 0);
            CHECK(expect_events(pair ? pairsExpected : singlesExpected, pair ? 5 : 3));
        }
        for (int frame = 0; frame <= 1; frame++) {
            begin(action, 0);
            snprintf(sScenario, sizeof(sScenario), "punch %d release on attack frame %d", pair + 1,
                     frame);
            CHECK(reach(attackArg, frame, TRUE));
            tick(FALSE, 0);
            tick(TRUE, 0);
            CHECK(expect_events(pair ? pairsExpected : singlesExpected, pair ? 5 : 3));
        }
        // Releasing at either end of the extra wait must cancel a pending repeat.
        for (int frame = last - 1; frame <= last; frame++) {
            begin(action, 0);
            CHECK(arm_repeat(pair));
            snprintf(sScenario, sizeof(sScenario), "punch %d release on final recovery frame %d",
                     pair + 1, frame);
            CHECK(reach(recoveryArg, frame, TRUE));
            int count = sEventCount;
            for (int i = 0; i < 80 && punching(); i++) {
                tick(FALSE, 0);
            }
            CHECK(!punching());
            CHECK_EQ(sEventCount, count);
        }
        begin(action, 0);
        CHECK(arm_repeat(pair));
        CHECK(reach(recoveryArg, last, TRUE));
        tick(FALSE, 0); // The sequence has ended; a later B press starts an ordinary combo.
        CHECK(!punching());
        set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_YAH);
        sEventCount = 0;
        const int fresh[] = { MARIO_ANIM_FIRST_PUNCH, MARIO_ANIM_SECOND_PUNCH, MARIO_ANIM_GROUND_KICK };
        CHECK(expect_events(fresh, 3));
    }
    return TRUE;
}

static int cancel_boundaries(u32 action) {
    for (int pair = 0; pair <= 1; pair++) {
        int anim = pair ? MARIO_ANIM_SECOND_PUNCH_FAST : MARIO_ANIM_FIRST_PUNCH_FAST;
        u32 arg =
            pair ? ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH_FAST : ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST;
        int firstCancelFrame = sAnimations[anim].loopEnd - 6;
        for (int offset = -1; offset <= 1; offset++) {
            begin(action, 0);
            CHECK(arm_repeat(pair));
            snprintf(sScenario, sizeof(sScenario), "punch %d A cancel at window offset %+d", pair + 1,
                     offset);
            CHECK(reach(arg, firstCancelFrame + offset, TRUE));
            tick(TRUE, INPUT_A_PRESSED);
            CHECK_EQ(sMario.action, offset < 0 ? action : ACT_JUMP);
        }
    }
    return TRUE;
}

static int ground_exit(u32 action) {
    begin(action, 0);
    CHECK(arm_repeat(TRUE));
    strcpy(sScenario, "leave ground through action handler, then restart");
    sGroundResult = GROUND_STEP_LEFT_GROUND;
    tick(TRUE, action == ACT_PUNCHING ? INPUT_OFF_FLOOR : 0);
    CHECK_EQ(sMario.action, ACT_FREEFALL);
    sGroundResult = GROUND_STEP_NONE;
    set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_YAH);
    sEventCount = 0;
    const int fresh[] = { MARIO_ANIM_FIRST_PUNCH, MARIO_ANIM_SECOND_PUNCH, MARIO_ANIM_GROUND_KICK };
    CHECK(expect_events(fresh, 3));
    return TRUE;
}

static int breakdance_crouch_momentum(u32 action) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int backwards = 0; backwards < 2; backwards++) {
            begin(action, backwards ? -40.0f : 40.0f);
            set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
            tick(FALSE, 0);
            sMarioObject.header.gfx.animInfo.animFrame =
                sAnimations[MARIO_ANIM_BREAKDANCE].loopEnd - GROUND_KICK_CANCEL_EARLY_FRAMES;
            sMario.zHeldFrames = 2;
            sMario.zCrouchBuffered = 1;
            f32 incoming = sMario.forwardVel;
            tick(FALSE, INPUT_Z_DOWN);
            CHECK_EQ(sMario.action, ACT_CROUCH_SLIDE);
            CHECK(sMario.forwardVel * incoming > 0 && fabsf(sMario.forwardVel) > 1);
        }
    }
    begin(ACT_IDLE, 0);
    sMario.zHeldFrames = 2;
    sMario.input = INPUT_Z_DOWN;
    check_common_idle_cancels(&sMario);
    CHECK_EQ(sMario.action, ACT_START_CROUCHING);
    return TRUE;
}
static int forced_cap_release(u32 action) {
    for (u32 forced = MARIO_VANISH_CAP; forced <= MARIO_SPECIAL_CAPS; forced += 2) {
        for (u32 natural = 0; natural <= MARIO_SPECIAL_CAPS; natural += 2) {
            begin(action, 0);
            sMario.forcedCapFlags = forced;
            sMario.naturalCapFlags = natural;
            sMario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | forced | natural;
            sMario.capTimer = natural ? 123 : 0;
            sController.buttonDown = L_TRIG | Z_TRIG;
            release_breakdance_power_up_cap(&sMario);
            CHECK_EQ(sMario.flags & MARIO_SPECIAL_CAPS, forced);
            CHECK_EQ(sMario.naturalCapFlags, 0);
            CHECK_EQ(sCapSpawns, natural ? 1 : 0);
            CHECK_EQ(sCapMusicStops, natural ? 1 : 0);
            CHECK_EQ(sMario.capTimer, 0);
            release_breakdance_power_up_cap(&sMario);
            CHECK_EQ(sCapSpawns, natural ? 1 : 0);
        }
    }
    return TRUE;
}

static int breakdance_cap_release(u32 action) {
    const struct {
        u32 flags;
        ModelID32 model;
    } caps[] = {
        { MARIO_WING_CAP, MODEL_MARIOS_WING_CAP },
        { MARIO_METAL_CAP, MODEL_MARIOS_METAL_CAP },
        { MARIO_VANISH_CAP, MODEL_MARIOS_CAP },
        { MARIO_WING_CAP | MARIO_METAL_CAP, MODEL_MARIOS_WINGED_METAL_CAP },
        { MARIO_WING_CAP | MARIO_VANISH_CAP, MODEL_MARIOS_WING_CAP },
        { MARIO_METAL_CAP | MARIO_VANISH_CAP, MODEL_MARIOS_METAL_CAP },
        { MARIO_SPECIAL_CAPS, MODEL_MARIOS_WINGED_METAL_CAP },
    };
    for (unsigned int i = 0; i < sizeof(caps) / sizeof(caps[0]); i++) {
        for (int normal = 0; normal <= 1; normal++) {
            begin(action, 0);
            sMario.flags = caps[i].flags | MARIO_CAP_ON_HEAD | (normal ? MARIO_NORMAL_CAP : 0);
            sMario.capTimer = 600;
            sMario.pos[0] = 100;
            sMario.pos[1] = 200;
            sMario.pos[2] = -300;
            sController.buttonDown = L_TRIG | Z_TRIG;
            set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
            tick(TRUE, INPUT_Z_DOWN);
            CHECK_EQ(sMario.flags & MARIO_CAPS, normal ? MARIO_NORMAL_CAP : 0);
            CHECK_EQ(!!(sMario.flags & MARIO_CAP_ON_HEAD), normal);
            CHECK_EQ(sMario.capTimer, 0);
            CHECK_EQ(sCapMusicStops, 1);
            CHECK_EQ(sCapSpawns, 1);
            CHECK_EQ(sCapModel, caps[i].model);
            CHECK(sCapBehavior == bhvReleasedPowerUpCap);
            CHECK(sReleasedCap.oPosX == 100 && sReleasedCap.oPosZ == -300);
            CHECK(sReleasedCap.oPosY > sMario.pos[1] && sReleasedCap.oVelY > 0);
            CHECK_EQ(sReleasedCap.oOpacity, caps[i].flags & MARIO_VANISH_CAP ? 150 : 255);

            // The released visual stays in place horizontally and expires independently of Mario.
            o = &sReleasedCap;
            f32 startY = o->oPosY;
            s32 lastOpacity = o->oOpacity;
            set_mario_action(&sMario, ACT_FREEFALL, 0);
            for (o->oTimer = 0; o->oTimer <= 30; o->oTimer++) {
                bhv_released_power_up_cap_loop();
                CHECK(o->oPosX == 100 && o->oPosZ == -300);
                CHECK_EQ(o->oInteractType, 0);
                CHECK(o->oOpacity >= 0 && o->oOpacity <= lastOpacity);
                if (o->oTimer == 0) {
                    CHECK(o->oPosY > startY);
                }
                if (o->oTimer < 30) {
                    CHECK(o->activeFlags != 0);
                }
                lastOpacity = o->oOpacity;
            }
            CHECK_EQ(o->activeFlags, 0);
        }
    }
    // The cosmetic object pool being full must not prevent cancelling the power.
    begin(action, 0);
    sMario.flags = MARIO_NORMAL_CAP | MARIO_WING_CAP | MARIO_CAP_ON_HEAD;
    sController.buttonDown = L_TRIG | Z_TRIG;
    sCapSpawnFails = TRUE;
    set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
    tick(TRUE, INPUT_Z_DOWN);
    CHECK_EQ(sMario.flags & MARIO_CAPS, MARIO_NORMAL_CAP);
    CHECK_EQ(sCapMusicStops, 1);
    return TRUE;
}

// Bound the relevant same-update redispatch branches. Common water checks,
// crouch/punch handlers and the cap water-to-land transition are production
// functions; ordinary swim physics and unrelated dispatch branches are excluded.
static int underwater_breakdance_dispatch(void) {
    for (int i = 0; i < 12; i++) {
        switch (sMario.action & ACT_GROUP_MASK) {
            case ACT_GROUP_STATIONARY:
                if (check_common_stationary_cancels(&sMario)) {
                    continue;
                }
                if (sMario.action == ACT_START_CROUCHING) {
                    if (act_start_crouching(&sMario)) {
                        continue;
                    }
                } else if (sMario.action == ACT_CROUCHING) {
                    if (act_crouching(&sMario)) {
                        continue;
                    }
                } else if (sMario.action == ACT_IDLE) {
                    if (check_common_idle_cancels(&sMario)) {
                        continue;
                    }
                } else {
                    unsupported("underwater stationary action");
                }
                return TRUE;
            case ACT_GROUP_OBJECT:
                CHECK_EQ(sMario.action, ACT_PUNCHING);
                if (check_common_object_cancels(&sMario) || act_punching(&sMario)) {
                    continue;
                }
                return TRUE;
            case ACT_GROUP_MOVING:
                CHECK_EQ(sMario.action, ACT_MOVE_PUNCHING);
                if (check_common_moving_cancels(&sMario) || act_move_punching(&sMario)) {
                    continue;
                }
                return TRUE;
            case ACT_GROUP_SUBMERGED:
                if (transition_cap_swimming_to_land(&sMario)) {
                    continue;
                }
                CHECK_EQ(sMario.action, ACT_WATER_PLUNGE);
                return TRUE;
            default:
                unsupported("underwater action group");
        }
    }
    CHECK(!"land/water action loop exceeded twelve dispatches");
    return FALSE;
}

static int breakdance_cap_underwater(u32 action) {
    const s32 depths[] = { 100, 101, 500 };
    for (int wing = 0; wing < 2; wing++) {
        for (int normal = 0; normal < 2; normal++) {
            for (int failSpawn = 0; failSpawn < 2; failSpawn++) {
                for (int entry = 0; entry < 3; entry++) {
                    for (int depth = 0; depth < 3; depth++) {
                        begin(action, 0);
                        sMario.waterLevel = 1000;
                        sMario.pos[1] = sMario.floorHeight = sMario.waterLevel - depths[depth];
                        sMario.flags = MARIO_VANISH_CAP | MARIO_CAP_ON_HEAD
                                       | (wing ? MARIO_WING_CAP : 0) | (normal ? MARIO_NORMAL_CAP : 0);
                        sMario.capTimer = 600;
                        sMario.zHeldFrames = 1;
                        sController.buttonDown = L_TRIG | Z_TRIG;
                        sCapSpawnFails = failSpawn;
                        set_mario_action(&sMario,
                                         entry == 0   ? action
                                         : entry == 1 ? ACT_START_CROUCHING
                                                      : ACT_CROUCHING,
                                         entry == 0 ? ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE : 0);
                        mario_set_punch_hit_phase(4);
                        remember_punch_hit_object(&sTarget);
                        sMario.input = INPUT_B_DOWN | INPUT_B_PRESSED | INPUT_Z_DOWN | INPUT_IN_WATER;
                        CHECK(underwater_breakdance_dispatch());
                        CHECK_EQ(sMario.flags & MARIO_CAPS, normal ? MARIO_NORMAL_CAP : 0);
                        CHECK_EQ(!!(sMario.flags & MARIO_CAP_ON_HEAD), normal);
                        CHECK_EQ(sMario.capTimer, 0);
                        CHECK_EQ(sCapSpawns, 1);
                        CHECK_EQ(sCapMusicStops, 1);
                        if (depth > 0) {
                            CHECK_EQ(sMario.action, ACT_WATER_PLUNGE);
                            CHECK_EQ(sCamera.mode, WATER_SURFACE_CAMERA_MODE);
                            CHECK_EQ(sGroundSteps, 0);
                            CHECK_EQ(sMario.flags & (MARIO_PUNCHING | MARIO_KICKING | MARIO_TRIPPING),
                                     0);
                            CHECK_EQ(punch_hit_object(&sTarget), FALSE);
                            // Continued held B/L/Z cannot loop or spawn a second cap next update.
                            sMario.input &= ~INPUT_B_PRESSED;
                            CHECK(underwater_breakdance_dispatch());
                            CHECK_EQ(sMario.action, ACT_WATER_PLUNGE);
                            CHECK_EQ(sCapSpawns, 1);
                            CHECK_EQ(sCapMusicStops, 1);
                            // Reacquire Vanish while swimming, return to the crouch chord and
                            // remove it again, across three complete land/water cycles.
                            for (int cycle = 1; cycle < 3; cycle++) {
                                sMario.flags |=
                                    MARIO_VANISH_CAP | MARIO_CAP_ON_HEAD | (wing ? MARIO_WING_CAP : 0);
                                sMario.capTimer = 600;
                                set_mario_action(&sMario, ACT_WATER_IDLE, 0);
                                sMario.zCrouchBuffered = TRUE;
                                sMario.input =
                                    INPUT_B_DOWN | INPUT_B_PRESSED | INPUT_Z_DOWN | INPUT_IN_WATER;
                                CHECK(underwater_breakdance_dispatch());
                                CHECK_EQ(sMario.action, ACT_WATER_PLUNGE);
                                CHECK_EQ(sMario.capTimer, 0);
                                CHECK_EQ(sMario.flags & MARIO_CAPS, normal ? MARIO_NORMAL_CAP : 0);
                                CHECK_EQ(!!(sMario.flags & MARIO_CAP_ON_HEAD), normal);
                                CHECK_EQ(sCapSpawns, cycle + 1);
                                CHECK_EQ(sCapMusicStops, cycle + 1);
                                CHECK_EQ(sGroundSteps, 0);
                            }
                        } else {
                            CHECK_EQ(sMario.action, entry == 0 ? action : ACT_PUNCHING);
                            CHECK_EQ(sMario.actionArg, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
                            CHECK_EQ(sGroundSteps, 1);
                        }
                    }
                }
            }
        }
    }
    // Keeping Vanish must allow an ordinary underwater breakdance to finish
    // its current update. The shared object check also covers held-object actions.
    const u32 objects[] = { ACT_PUNCHING, ACT_PICKING_UP, ACT_THROWING, ACT_HOLDING_BOWSER };
    for (unsigned i = 0; i < sizeof(objects) / sizeof(objects[0]); i++) {
        begin(objects[i], 0);
        sMario.waterLevel = 500;
        sMario.flags = MARIO_VANISH_CAP;
        CHECK_EQ(check_common_object_cancels(&sMario), FALSE);
        CHECK_EQ(sMario.action, objects[i]);
        sMario.input = INPUT_SQUISHED;
        CHECK_EQ(check_common_object_cancels(&sMario), TRUE);
        CHECK_EQ(sMario.action, ACT_SQUISHED);
        set_mario_action(&sMario, objects[i], 0);
        sMario.input = 0;
        sMario.health = 0xFF;
        CHECK_EQ(check_common_object_cancels(&sMario), TRUE);
        CHECK_EQ(sMario.action, ACT_STANDING_DEATH);
    }
    for (int metal = 0; metal < 2; metal++) {
        begin(action, 0);
        sMario.waterLevel = 500;
        sMario.flags = metal ? MARIO_METAL_CAP : MARIO_NORMAL_CAP;
        CHECK_EQ(action == ACT_PUNCHING ? check_common_object_cancels(&sMario)
                                        : check_common_moving_cancels(&sMario),
                 !metal);
        CHECK_EQ(sMario.action, metal ? action : ACT_WATER_PLUNGE);
        CHECK_EQ(sCapSpawns, 0);
    }
    begin(action, 0);
    sMario.waterLevel = 500;
    sMario.flags = MARIO_VANISH_CAP;
    sMario.capTimer = 600;
    sController.buttonDown = Z_TRIG;
    set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
    sMario.input = INPUT_B_PRESSED | INPUT_Z_DOWN;
    CHECK(underwater_breakdance_dispatch());
    CHECK_EQ(sMario.action, action);
    CHECK_EQ(sCapSpawns, 0);
    CHECK_EQ(sMario.capTimer, 600);
    CHECK(sMario.flags & MARIO_VANISH_CAP);
    begin(action, 0);
    sMario.waterLevel = 500;
    sMario.flags = MARIO_VANISH_CAP;
    sMario.capTimer = 600;
    sController.buttonDown = L_TRIG | Z_TRIG;
    set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
    sMario.input = INPUT_STOMPED | INPUT_Z_DOWN;
    CHECK_EQ(action == ACT_PUNCHING ? act_punching(&sMario) : check_common_moving_cancels(&sMario),
             TRUE);
    CHECK_EQ(sMario.action, ACT_SHOCKWAVE_BOUNCE);
    CHECK_EQ(sCapSpawns, 0);
    CHECK_EQ(sCapMusicStops, 0);
    CHECK(sMario.flags & MARIO_VANISH_CAP);
    return TRUE;
}

static int cap_presets_underwater(u32 unused) {
    (void) unused;
    for (int metalOriginal = 0; metalOriginal < 2; metalOriginal++) {
        for (int vanishOriginal = 0; vanishOriginal < 2; vanishOriginal++) {
            sTestGameOptions[GO_METAL_CAP] = metalOriginal;
            sTestGameOptions[GO_VANISH_CAP] = vanishOriginal;
            for (int caps = 0; caps < 8; caps++) {
                for (int holding = 0; holding < 2; holding++) {
                    begin(holding ? ACT_HOLD_WATER_IDLE : ACT_WATER_IDLE, 0);
                    sMario.flags = (caps & 1 ? MARIO_WING_CAP : 0) | (caps & 2 ? MARIO_METAL_CAP : 0)
                                   | (caps & 4 ? MARIO_VANISH_CAP : 0);
                    sMario.waterLevel = 500;
                    sMario.capTimer = 90;
                    sMario.pos[1] = 20;
                    sMario.vel[0] = 6;
                    sMario.vel[1] = -3;
                    sMario.vel[2] = 8;
                    sMario.heldObj = holding ? &sTarget : NULL;
                    u32 powers = sMario.flags;
                    int landControls =
                        ((caps & 2) && !metalOriginal) || ((caps & 4) && !vanishOriginal);
                    CHECK_EQ(transition_cap_swimming_to_land(&sMario), landControls);
                    CHECK_EQ(sMario.action, landControls
                                                ? (holding ? ACT_HOLD_FREEFALL : ACT_FREEFALL)
                                                : (holding ? ACT_HOLD_WATER_IDLE : ACT_WATER_IDLE));
                    CHECK_EQ(sMario.flags, powers);
                    CHECK_EQ(sMario.capTimer, 90);
                    CHECK(sMario.heldObj == (holding ? &sTarget : NULL));
                    CHECK(fabsf(sMario.vel[0] - 6) < 0.01f);
                    CHECK(fabsf(sMario.vel[1] + 3) < 0.01f);
                    CHECK(fabsf(sMario.vel[2] - 8) < 0.01f);
                }
            }
        }
    }
    sTestGameOptions[GO_METAL_CAP] = sTestGameOptions[GO_VANISH_CAP] = 0;
    return TRUE;
}

static int cap_land_movement_underwater(u32 unused) {
    const u32 metalActions[] = {
        ACT_METAL_WATER_STANDING,       ACT_METAL_WATER_WALKING,      ACT_METAL_WATER_FALLING,
        ACT_METAL_WATER_FALL_LAND,      ACT_METAL_WATER_JUMP,         ACT_METAL_WATER_JUMP_LAND,
        ACT_HOLD_METAL_WATER_STANDING,  ACT_HOLD_METAL_WATER_WALKING, ACT_HOLD_METAL_WATER_FALLING,
        ACT_HOLD_METAL_WATER_FALL_LAND, ACT_HOLD_METAL_WATER_JUMP,    ACT_HOLD_METAL_WATER_JUMP_LAND,
    };
    (void) unused;
    const u32 caps[] = { MARIO_METAL_CAP, MARIO_VANISH_CAP, MARIO_METAL_CAP | MARIO_VANISH_CAP };
    for (unsigned cap = 0; cap < sizeof(caps) / sizeof(caps[0]); cap++) {
        for (unsigned i = 0; i < sizeof(metalActions) / sizeof(metalActions[0]); i++) {
            for (int airborne = 0; airborne < 2; airborne++) {
                int holding = i >= 6;
                begin(metalActions[i], 0);
                sMario.flags = caps[cap];
                sMario.waterLevel = 500;
                sMario.pos[1] = airborne ? 20.0f : 0.0f;
                sMario.vel[0] = 6.0f;
                sMario.vel[1] = airborne ? -3.0f : 0.0f;
                sMario.vel[2] = 8.0f;
                if (holding) {
                    sMario.heldObj = &sTarget;
                }
                CHECK(mario_has_land_movement_underwater(&sMario));
                CHECK(transition_cap_swimming_to_land(&sMario));
                u32 expectedAction;
                if (airborne) {
                    expectedAction = holding ? ACT_HOLD_FREEFALL : ACT_FREEFALL;
                } else {
                    expectedAction = holding ? ACT_HOLD_DECELERATING : ACT_DECELERATING;
                }
                CHECK_EQ(sMario.action, expectedAction);
                CHECK(fabsf(sMario.vel[0] - 6.0f) < 0.01f);
                CHECK(fabsf(sMario.vel[2] - 8.0f) < 0.01f);
                CHECK(sMario.forwardVel > 9.9f && sMario.forwardVel < 10.1f);
            }
        }
    }
    begin(ACT_METAL_WATER_STANDING, 0);
    sMario.flags = MARIO_METAL_CAP;
    CHECK(mario_has_land_movement_underwater(&sMario));
    CHECK(transition_cap_swimming_to_land(&sMario));
    CHECK_EQ(sMario.action, ACT_IDLE);
    begin(ACT_WATER_IDLE, 0);
    sMario.flags = MARIO_METAL_CAP | MARIO_VANISH_CAP;
    CHECK(transition_cap_swimming_to_land(&sMario));
    CHECK_EQ(sMario.action, ACT_IDLE);
    begin(ACT_HOLD_WATER_IDLE, 0);
    sMario.flags = MARIO_METAL_CAP | MARIO_VANISH_CAP;
    sMario.heldObj = &sTarget;
    CHECK(transition_cap_swimming_to_land(&sMario));
    CHECK_EQ(sMario.action, ACT_HOLD_IDLE);
    begin(ACT_WATER_SHOCKED, 0);
    sMario.flags = MARIO_METAL_CAP | MARIO_VANISH_CAP;
    CHECK(!transition_cap_swimming_to_land(&sMario));
    CHECK_EQ(sMario.action, ACT_WATER_SHOCKED);
    const u32 excluded[] = { ACT_WATER_DEATH, ACT_DROWNING, ACT_BACKWARD_WATER_KB,
                             ACT_STAR_DANCE_WATER };
    for (unsigned i = 0; i < sizeof(excluded) / sizeof(excluded[0]); i++) {
        begin(excluded[i], 0);
        sMario.flags = MARIO_METAL_CAP;
        CHECK(!transition_cap_swimming_to_land(&sMario));
        CHECK_EQ(sMario.action, excluded[i]);
    }
    const u32 landActions[] = { ACT_IDLE,       ACT_CROUCHING,        ACT_CRAWLING,
                                ACT_DIVE_SLIDE, ACT_SLIDE_KICK_SLIDE, ACT_BUTT_SLIDE,
                                ACT_WALKING,    ACT_PUNCHING,         ACT_PICKING_UP,
                                ACT_THROWING,   ACT_HOLD_IDLE,        ACT_HOLD_WALKING };
    for (unsigned cap = 0; cap < sizeof(caps) / sizeof(caps[0]); cap++) {
        for (unsigned i = 0; i < sizeof(landActions) / sizeof(landActions[0]); i++) {
            begin(landActions[i], 0);
            sMario.flags = caps[cap];
            sMario.waterLevel = 500;
            s32 group = sMario.action & ACT_GROUP_MASK;
            s32 cancel = group == ACT_GROUP_STATIONARY ? check_common_stationary_cancels(&sMario)
                         : group == ACT_GROUP_MOVING   ? check_common_moving_cancels(&sMario)
                                                       : check_common_object_cancels(&sMario);
            CHECK(!cancel);
            CHECK_EQ(sMario.action, landActions[i]);
            sMario.flags = 0;
            cancel = group == ACT_GROUP_STATIONARY ? check_common_stationary_cancels(&sMario)
                     : group == ACT_GROUP_MOVING   ? check_common_moving_cancels(&sMario)
                                                   : check_common_object_cancels(&sMario);
            CHECK(cancel);
            CHECK_EQ(sMario.action, ACT_WATER_PLUNGE);
        }
    }
    // Backward swim carry must remain backward, including the actual velocity.
    begin(ACT_WATER_PLUNGE, 0);
    sMario.flags = MARIO_METAL_CAP;
    sMario.pos[1] = 20;
    sMario.vel[0] = 6;
    sMario.vel[2] = -8;
    sMario.vel[1] = -3;
    CHECK(transition_cap_swimming_to_land(&sMario));
    CHECK_EQ(sMario.action, ACT_FREEFALL);
    CHECK(sMario.forwardVel < -9.9f);
    CHECK(fabsf(sMario.vel[0] - 6.0f) < 0.01f);
    CHECK(fabsf(sMario.vel[2] + 8.0f) < 0.01f);
    CHECK_EQ(sMario.vel[1], -3);
    return TRUE;
}

static int breakdance_cap_startup_only(u32 action) {
    const u32 buttons[] = { 0, L_TRIG, Z_TRIG, L_TRIG | Z_TRIG };
    const f32 speeds[] = { -20.0f, -19.99f, -10.0f, 0.0f, 10.0f, 19.99f, 20.0f };
    const f32 sticks[] = { 0.0f, 1.0f, 64.0f };
    for (int b = 0; b < 4; b++) {
        for (int v = 0; v < 7; v++) {
            for (int s = 0; s < 3; s++) {
                begin(action, speeds[v]);
                sMario.flags = MARIO_NORMAL_CAP | MARIO_WING_CAP | MARIO_CAP_ON_HEAD;
                sMario.capTimer = 600;
                sController.buttonDown = buttons[b];
                sController.stickMag = sticks[s];
                set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
                tick(TRUE, INPUT_Z_DOWN);
                int released = fabsf(speeds[v]) < 20.0f && s == 0 && b == 3;
                CHECK_EQ(sCapSpawns, released);
                CHECK_EQ(sMario.capTimer, released ? 0 : 600);
                CHECK_EQ(!!(sMario.flags & MARIO_WING_CAP), !released);
                CHECK(sMario.flags & MARIO_NORMAL_CAP);

                // Becoming idle, pressing L later, or picking up another cap cannot release mid-spin.
                sController.buttonDown = L_TRIG | Z_TRIG;
                sController.stickMag = 0;
                sMario.forwardVel = 0;
                sMario.flags |= MARIO_WING_CAP;
                for (int frame = 0; frame < 8; frame++) {
                    tick(TRUE, INPUT_Z_DOWN);
                }
                CHECK_EQ(sCapSpawns, released);
                CHECK(sMario.flags & MARIO_WING_CAP);
                set_mario_action(&sMario, action, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
                tick(TRUE, INPUT_Z_DOWN);
                CHECK_EQ(sCapSpawns, released + 1);
            }
        }
    }
    // Neither the regular cap alone nor a normal punch responds to this chord.
    for (int power = 0; power <= 1; power++) {
        begin(action, 0);
        sMario.flags = MARIO_NORMAL_CAP | MARIO_CAP_ON_HEAD | (power ? MARIO_WING_CAP : 0);
        sController.buttonDown = L_TRIG | Z_TRIG;
        set_mario_action(&sMario, action,
                         power ? ACT_ARG_PUNCH_SEQUENCE_YAH : ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        tick(TRUE, INPUT_Z_DOWN);
        CHECK_EQ(sCapSpawns, 0);
        CHECK_EQ(sCapMusicStops, 0);
        CHECK(sMario.flags & MARIO_NORMAL_CAP);
        CHECK(sMario.flags & MARIO_CAP_ON_HEAD);
    }
    return TRUE;
}

static int breakdance_cap_crouch_entry(u32 action) {
    for (int crouched = 0; crouched <= 1; crouched++) {
        begin(ACT_IDLE, 0);
        sMario.flags = MARIO_NORMAL_CAP | MARIO_METAL_CAP | MARIO_CAP_ON_HEAD;
        sMario.zHeldFrames = 1;
        sMario.input = INPUT_Z_DOWN;
        sController.buttonDown = L_TRIG | Z_TRIG;
        CHECK(check_common_idle_cancels(&sMario));
        CHECK_EQ(sMario.action, ACT_START_CROUCHING);
        if (crouched) {
            set_mario_action(&sMario, ACT_CROUCHING, 0);
        }
        sMario.input |= INPUT_B_PRESSED;
        CHECK(crouched ? act_crouching(&sMario) : act_start_crouching(&sMario));
        CHECK_EQ(sMario.actionArg, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
        CHECK_EQ(sCapSpawns, 0);
        tick(TRUE, INPUT_Z_DOWN);
        CHECK_EQ(sCapSpawns, 1);
        CHECK_EQ(sMario.flags & MARIO_CAPS, MARIO_NORMAL_CAP);
    }
    return TRUE;
}

/* Follow the actual action return value: cancels redispatch before movement. */
static int crouch_dispatch(void) {
    for (int i = 0; i < 16; i++) {
        int cancel = 0;
        switch (sMario.action) {
            case ACT_START_CROUCHING:
                cancel = act_start_crouching(&sMario);
                break;
            case ACT_CROUCHING:
                cancel = act_crouching(&sMario);
                break;
            case ACT_START_CRAWLING:
                cancel = act_start_crawling(&sMario);
                break;
            case ACT_STOP_CRAWLING:
                cancel = act_stop_crawling(&sMario);
                break;
            case ACT_STOP_CROUCHING:
                cancel = act_stop_crouching(&sMario);
                break;
            case ACT_IDLE:
                cancel = check_common_idle_cancels(&sMario);
                break;
            case ACT_CROUCH_SLIDE:
                cancel = act_crouch_slide(&sMario);
                break;
            case ACT_BUTT_SLIDE:
                cancel = act_butt_slide(&sMario);
                break;
            default:
                return TRUE;
        }
        if (!cancel) {
            return TRUE;
        }
    }
    return FALSE;
}
static int crawl_cancel_windows(u32 unused) {
    const u32 inputs[] = { INPUT_A_PRESSED, INPUT_B_PRESSED, INPUT_NONZERO_ANALOG, INPUT_FIRST_PERSON,
                           0 };
    const u32 actions[] = { ACT_JUMP, ACT_PUNCHING, ACT_WALKING, ACT_FIRST_PERSON, ACT_IDLE };
    for (int frame = 0; frame <= 3; frame++) {
        for (unsigned i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
            begin(ACT_STOP_CRAWLING, 0);
            set_mario_animation(&sMario, MARIO_ANIM_STOP_CRAWLING);
            sMarioObject.header.gfx.animInfo.animFrame = frame;
            sMario.input = inputs[i];
            CHECK(crouch_dispatch());
            CHECK_EQ(sMario.action, frame < 2 ? ACT_STOP_CRAWLING : actions[i]);
        }
        begin(ACT_STOP_CRAWLING, 0);
        set_mario_animation(&sMario, MARIO_ANIM_STOP_CRAWLING);
        sMarioObject.header.gfx.animInfo.animFrame = frame;
        sMario.input = INPUT_Z_DOWN | INPUT_A_PRESSED;
        CHECK(crouch_dispatch());
        CHECK_EQ(sMario.action, frame < 2 ? ACT_STOP_CRAWLING : ACT_BACKFLIP);
    }
    for (int frame = 0; frame <= 4; frame++) {
        begin(ACT_START_CRAWLING, 0);
        set_mario_animation(&sMario, MARIO_ANIM_START_CRAWLING);
        sMarioObject.header.gfx.animInfo.animFrame = frame;
        sController.stickMag = MARIO_STRONG_STICK_THRESHOLD;
        sMario.input = INPUT_Z_DOWN | INPUT_A_PRESSED | INPUT_NONZERO_ANALOG;
        act_start_crawling(&sMario);
        CHECK_EQ(sMario.action, frame < 4 ? ACT_LONG_JUMP : ACT_START_CRAWLING);
    }
    return TRUE;
}

static int crouch_entry_exit(u32 unused) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int frame = -1; frame < 6; frame++) {
            begin(ACT_START_CROUCHING, 0);
            set_mario_animation(&sMario, MARIO_ANIM_START_CROUCHING);
            sMarioObject.header.gfx.animInfo.animFrame = frame;
            sMario.input = 0;
            sCrouchSteps = 0;
            CHECK(crouch_dispatch());
            CHECK_EQ(sMario.action, ACT_STOP_CROUCHING);
            CHECK_EQ(sCrouchSteps, 1);
            sMario.input = INPUT_Z_DOWN;
            CHECK(crouch_dispatch());
            CHECK_EQ(sMario.action, ACT_START_CROUCHING);
            sMario.input |= INPUT_NONZERO_ANALOG;
            CHECK(crouch_dispatch());
            CHECK_EQ(sMario.action, ACT_START_CROUCHING);
            sMarioObject.header.gfx.animInfo.animFrame =
                sAnimations[MARIO_ANIM_START_CROUCHING].loopEnd - 1;
            CHECK(crouch_dispatch());
            CHECK_EQ(sMario.action, ACT_START_CRAWLING);
            set_mario_action(&sMario, ACT_STOP_CROUCHING, 0);
            sMario.input = INPUT_B_PRESSED;
            CHECK_EQ(act_stop_crouching(&sMario), TRUE);
            CHECK_EQ(sMario.action, ACT_PUNCHING);
            set_mario_action(&sMario, ACT_START_CROUCHING, 0);
            sMario.input = INPUT_A_PRESSED | INPUT_Z_DOWN;
            CHECK_EQ(act_start_crouching(&sMario), TRUE);
            CHECK_EQ(sMario.action, ACT_BACKFLIP);
            set_mario_action(&sMario, ACT_START_CROUCHING, 0);
            sMario.input = INPUT_FIRST_PERSON | INPUT_Z_DOWN;
            CHECK(crouch_dispatch());
            CHECK_EQ(sMario.action, ACT_STOP_CROUCHING);
        }
    }
    const u32 inputs[] = { INPUT_A_PRESSED, INPUT_B_PRESSED, INPUT_NONZERO_ANALOG, INPUT_FIRST_PERSON,
                           INPUT_Z_DOWN };
    const u32 actions[] = { ACT_JUMP, ACT_PUNCHING, ACT_WALKING, ACT_FIRST_PERSON,
                            ACT_START_CROUCHING };
    int boundary = sAnimations[MARIO_ANIM_STOP_CROUCHING].loopEnd - 7;
    CHECK_EQ(boundary, 3);
    for (int frame = boundary - 1; frame <= boundary + 1; frame++) {
        for (unsigned i = 0; i < 5; i++) {
            begin(ACT_STOP_CROUCHING, 0);
            set_mario_animation(&sMario, MARIO_ANIM_STOP_CROUCHING);
            sMarioObject.header.gfx.animInfo.animFrame = frame;
            sMario.input = inputs[i];
            sMario.zHeldFrames = 2;
            sCrouchSteps = 0;
            CHECK(crouch_dispatch());
            u32 early[] = { ACT_BACKFLIP, ACT_PUNCHING, ACT_STOP_CROUCHING, ACT_STOP_CROUCHING,
                            ACT_START_CROUCHING };
            CHECK_EQ(sMario.action, frame < boundary ? early[i] : actions[i]);
            CHECK(sCrouchSteps <= 1);
        }
    }
    // Repeated Z presses must bypass idle's delayed Z handling at every exit pose.
    for (int speed = 0; speed < 2; speed++) {
        for (int frame = 0; frame < sAnimations[MARIO_ANIM_STOP_CROUCHING].loopEnd; frame++) {
            begin(ACT_STOP_CROUCHING, speed ? 40 : 0);
            for (int cycle = 0; cycle < 4; cycle++) {
                set_mario_animation(&sMario, MARIO_ANIM_STOP_CROUCHING);
                sMarioObject.header.gfx.animInfo.animFrame = frame;
                sMario.input = INPUT_Z_DOWN | INPUT_Z_PRESSED;
                sMario.zHeldFrames = 0;
                int steps = sCrouchSteps;
                CHECK(crouch_dispatch());
                CHECK_EQ(sMario.action, ACT_START_CROUCHING);
                CHECK_EQ(sMarioObject.header.gfx.animInfo.animID, MARIO_ANIM_START_CROUCHING);
                CHECK_EQ(sMarioObject.header.gfx.animInfo.animFrame, 1);
                CHECK_EQ(sCrouchSteps, steps + 1);
                sMario.input = 0;
                CHECK(crouch_dispatch());
                CHECK_EQ(sMario.action, ACT_STOP_CROUCHING);
                CHECK_EQ(sMarioObject.header.gfx.animInfo.animID, MARIO_ANIM_STOP_CROUCHING);
            }
        }
    }
    // Opening the input window must not cut short a neutral stand-up.
    int completion = sAnimations[MARIO_ANIM_STOP_CROUCHING].loopEnd - 2;
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int frame = boundary - 1; frame <= completion; frame++) {
            begin(ACT_STOP_CROUCHING, 0);
            set_mario_animation(&sMario, MARIO_ANIM_STOP_CROUCHING);
            sMarioObject.header.gfx.animInfo.animFrame = frame;
            sMario.input = 0;
            sCrouchSteps = 0;
            CHECK_EQ(act_stop_crouching(&sMario), frame >= completion);
            CHECK_EQ(sMario.action, frame < completion ? ACT_STOP_CROUCHING : ACT_IDLE);
            CHECK_EQ(sCrouchSteps, frame < completion);
        }
    }
    begin(ACT_STOP_CROUCHING, 0);
    sMario.input = INPUT_OFF_FLOOR | INPUT_A_PRESSED;
    CHECK_EQ(act_stop_crouching(&sMario), TRUE);
    CHECK_EQ(sMario.action, ACT_FREEFALL);
    return TRUE;
}
static int crouch_lowering_samples(u32 unused) {
    const s16 rootY[] = { 170, 163, 140, 112, 105, 105 };
    const s16 rootZ[] = { 0, -7, -19, -27, -32, -35 };
    const unsigned channelCount = sizeof(anim_97_indices) / (2 * sizeof(*anim_97_indices));
    const unsigned valueCount = sizeof(anim_97_values) / sizeof(*anim_97_values);
    CHECK_EQ(channelCount, 63);
    CHECK_EQ(sAnimations[MARIO_ANIM_START_CROUCHING].loopEnd, 11);
    for (unsigned channel = 0; channel < channelCount; channel++) {
        unsigned count = anim_97_indices[2 * channel];
        unsigned offset = anim_97_indices[2 * channel + 1];
        CHECK(count == 1 || count == 11);
        CHECK(offset + count <= valueCount);
        if (count == 1) {
            continue;
        }
        const s16 *samples = &anim_97_values[offset];
        for (unsigned frame = 1; frame < 10; frame += 2) {
            s32 delta = channel < 3 ? samples[frame + 1] - samples[frame - 1]
                                    : (s16) (samples[frame + 1] - samples[frame - 1]);
            s32 advance = channel < 3 ? samples[frame] - samples[frame - 1]
                                      : (s16) (samples[frame] - samples[frame - 1]);
            CHECK(abs(2 * advance - delta) <= 1);
        }
    }
    for (unsigned anchor = 0; anchor < 6; anchor++) {
        CHECK_EQ(anim_97_values[anim_97_indices[3] + 2 * anchor], rootY[anchor]);
        CHECK_EQ(anim_97_values[anim_97_indices[5] + 2 * anchor], rootZ[anchor]);
    }
    // Every update before the final hold must display a distinct complete pose.
    for (unsigned frame = 1; frame < 11; frame++) {
        s32 changed = FALSE;
        for (unsigned channel = 0; channel < channelCount; channel++) {
            if (anim_97_indices[2 * channel] == 11) {
                unsigned offset = anim_97_indices[2 * channel + 1];
                changed |= anim_97_values[offset + frame] != anim_97_values[offset + frame - 1];
            }
        }
        CHECK(changed);
    }
    return TRUE;
}
static int crouch_lowering_playback(u32 unused) {
    // Exercise the native frame clock through standing entry, an immediate
    // slide stop and a later stop that must retain the intermediate pose.
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int analog = 0; analog < 2; analog++) {
            for (int entry = 0; entry < 3; entry++) {
                begin(entry ? ACT_CROUCH_SLIDE : ACT_START_CROUCHING, entry == 2 ? 40 : 2);
                sRealSliding = TRUE;
                sMario.slideVelZ = sMario.forwardVel;
                sMario.input = INPUT_Z_DOWN | (analog ? INPUT_NONZERO_ANALOG : 0);
                struct AnimInfo *info = &sMarioObject.header.gfx.animInfo;
                int skip = entry ? 0 : 2;
                for (int tick = 0; tick < 12 - skip; tick++) {
                    if (entry == 2 && tick == 4) {
                        sMario.forwardVel = sMario.slideVelZ = 2;
                    }
                    CHECK(crouch_dispatch());
                    CHECK_EQ(info->animID, MARIO_ANIM_START_CROUCHING);
                    if (tick < 11 - skip) {
                        CHECK(sMario.action == ACT_START_CROUCHING
                              || sMario.action == ACT_CROUCH_SLIDE);
                    } else {
                        CHECK_EQ(sMario.action, analog ? ACT_START_CRAWLING : ACT_CROUCHING);
                    }
                    gAreaUpdateCounter++;
                    info->animFrame = geo_update_animation_frame(info, &info->animFrameAccelAssist);
                    info->animTimer = gAreaUpdateCounter;
                    CHECK_EQ(info->animFrame, tick < 11 - skip ? tick + skip : 10);
                }
                CHECK(crouch_dispatch());
                CHECK_EQ(info->animID, analog ? MARIO_ANIM_START_CRAWLING : MARIO_ANIM_CROUCHING);
            }
        }
    }
    return TRUE;
}
static int crouch_start_animation(u32 unused) {
    const f32 speeds[] = { 0, 1, -1, 3.9f, -3.9f, 4.2f, -4.2f };
    const int boundary = sAnimations[MARIO_ANIM_START_CROUCHING].loopEnd - 1;
    CHECK_EQ(boundary, 10);
    // Native friction stops these slides on their first update. The lowering
    // pose must survive the stop, including when it was already partly played.
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int analog = 0; analog < 2; analog++) {
            for (int frame = -1; frame <= boundary; frame++) {
                for (unsigned i = 0; i < sizeof(speeds) / sizeof(*speeds); i++) {
                    begin(ACT_CROUCH_SLIDE, speeds[i]);
                    sRealSliding = TRUE;
                    sMario.slideVelZ = speeds[i];
                    sMario.input = INPUT_Z_DOWN | (analog ? INPUT_NONZERO_ANALOG : 0);
                    set_mario_animation(&sMario, MARIO_ANIM_START_CROUCHING);
                    sMarioObject.header.gfx.animInfo.animFrame = frame;
                    sCrouchSteps = 0;
                    CHECK_EQ(act_crouch_slide(&sMario), TRUE);
                    CHECK_EQ(sMario.forwardVel, 0);
                    CHECK_EQ(sMario.action, frame < boundary ? ACT_START_CROUCHING : ACT_CROUCHING);
                    CHECK_EQ(sMarioObject.header.gfx.animInfo.animID, MARIO_ANIM_START_CROUCHING);
                    CHECK_EQ(sMarioObject.header.gfx.animInfo.animFrame, frame);
                    CHECK_EQ(sCrouchSteps, 0);
                    if (frame >= boundary) {
                        continue;
                    }
                    for (int f = frame; f <= boundary; f++) {
                        CHECK_EQ(sMario.action, ACT_START_CROUCHING);
                        sMarioObject.header.gfx.animInfo.animFrame = f;
                        CHECK_EQ(act_start_crouching(&sMario), FALSE);
                        CHECK_EQ(sMarioObject.header.gfx.animInfo.animFrame, f);
                        CHECK_EQ(sMario.action, f < boundary ? ACT_START_CROUCHING
                                                : analog     ? ACT_START_CRAWLING
                                                             : ACT_CROUCHING);
                    }
                    CHECK(crouch_dispatch());
                    CHECK_EQ(sMarioObject.header.gfx.animInfo.animID,
                             analog ? MARIO_ANIM_START_CRAWLING : MARIO_ANIM_CROUCHING);
                }
            }
        }
    }
    // From standing, weak and full stick both wait for the lowering pose.
    const f32 sticks[] = { 1, 20, MARIO_STRONG_STICK_THRESHOLD, 64 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned i = 0; i < sizeof(sticks) / sizeof(*sticks); i++) {
            begin(ACT_START_CROUCHING, 0);
            sController.stickMag = sticks[i];
            sMario.input = INPUT_Z_DOWN | INPUT_NONZERO_ANALOG;
            set_mario_animation(&sMario, MARIO_ANIM_START_CROUCHING);
            set_anim_to_frame(&sMario, 2);
            for (int frame = 1; frame <= boundary; frame++) {
                CHECK_EQ(act_start_crouching(&sMario), FALSE);
                CHECK_EQ(sMarioObject.header.gfx.animInfo.animID, MARIO_ANIM_START_CROUCHING);
                CHECK_EQ(sMarioObject.header.gfx.animInfo.animFrame, frame);
                CHECK_EQ(sMario.action, frame < boundary ? ACT_START_CROUCHING : ACT_START_CRAWLING);
                sMarioObject.header.gfx.animInfo.animFrame++;
            }
            CHECK_EQ(act_start_crawling(&sMario), FALSE);
            CHECK_EQ(sMarioObject.header.gfx.animInfo.animID, MARIO_ANIM_START_CRAWLING);
            CHECK_EQ(sMarioObject.header.gfx.animInfo.animFrame, -1);
            int crawlEnd = sAnimations[MARIO_ANIM_START_CRAWLING].loopEnd - 2;
            for (int frame = 0; frame <= crawlEnd; frame++) {
                sMarioObject.header.gfx.animInfo.animFrame = frame;
                CHECK_EQ(act_start_crawling(&sMario), FALSE);
                CHECK_EQ(sMario.action, frame < crawlEnd ? ACT_START_CRAWLING : ACT_CRAWLING);
            }
        }
    }
    const u32 inputs[] = {
        INPUT_Z_DOWN | INPUT_A_PRESSED,    INPUT_Z_DOWN | INPUT_B_PRESSED, 0,
        INPUT_Z_DOWN | INPUT_FIRST_PERSON, INPUT_Z_DOWN | INPUT_OFF_FLOOR, INPUT_Z_DOWN | INPUT_STOMPED
    };
    const u32 exits[] = { ACT_BACKFLIP,       ACT_PUNCHING, ACT_STOP_CROUCHING,
                          ACT_STOP_CROUCHING, ACT_FREEFALL, ACT_SHOCKWAVE_BOUNCE };
    for (int frame = -1; frame <= boundary; frame++) {
        for (unsigned i = 0; i < sizeof(inputs) / sizeof(*inputs); i++) {
            begin(ACT_START_CROUCHING, 0);
            set_mario_animation(&sMario, MARIO_ANIM_START_CROUCHING);
            sMarioObject.header.gfx.animInfo.animFrame = frame;
            sMario.input = inputs[i] | INPUT_NONZERO_ANALOG;
            sController.stickMag = 20;
            sCrouchSteps = 0;
            CHECK_EQ(act_start_crouching(&sMario), TRUE);
            CHECK_EQ(sMario.action, exits[i]);
            CHECK_EQ(sCrouchSteps, 0);
        }
    }
    return TRUE;
}
static int crouch_slide_cancel(u32 unused) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int direction = -1; direction <= 1; direction++) {
            for (int releaseFrame = 0; releaseFrame <= 7; releaseFrame++) {
                begin(ACT_CROUCH_SLIDE, 40);
                sCrouchSteps = 0;
                sMario.intendedYaw = direction < 0 ? (s16) 0x8000 : 0;
                for (int frame = 0; frame <= releaseFrame; frame++) {
                    sMario.input = (direction ? INPUT_NONZERO_ANALOG : 0)
                                   | (frame < releaseFrame ? INPUT_Z_DOWN : 0);
                    sMario.zCrouchBuffered = TRUE;
                    int steps = sCrouchSteps;
                    int cancel = frame == releaseFrame;
                    CHECK_EQ(act_crouch_slide(&sMario), cancel);
                    CHECK_EQ(sMario.action, cancel ? ACT_STOP_CROUCHING : ACT_CROUCH_SLIDE);
                    CHECK_EQ(sCrouchSteps, steps + !cancel);
                    CHECK(fabsf(sMario.forwardVel - 40) < 0.001f);
                    if (cancel) {
                        CHECK_EQ(sMario.zCrouchBuffered, FALSE);
                        CHECK_EQ(sMario.crouchSlideBackflip, FALSE);
                    }
                }
            }
        }
    }
    return TRUE;
}
static int crouch_slide_backflip_buffer(u32 unused) {
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int zHeld = 0; zHeld < 2; zHeld++) {
            for (int pressFrame = 0; pressFrame < 6; pressFrame++) {
                begin(ACT_CROUCH_SLIDE, 40);
                sCrouchSteps = 0;
                sController.stickMag = 20;
                int launchFrame = pressFrame == 0 ? 1 : pressFrame;
                for (int frame = 0; frame <= launchFrame; frame++) {
                    sMario.intendedYaw = frame <= pressFrame ? (s16) 0x8000 : 0;
                    sMario.input = INPUT_NONZERO_ANALOG | INPUT_A_DOWN
                                   | (frame < launchFrame || zHeld ? INPUT_Z_DOWN : 0)
                                   | (frame == pressFrame ? INPUT_A_PRESSED : 0);
                    CHECK_EQ(act_crouch_slide(&sMario), frame == launchFrame);
                    CHECK_EQ(sMario.action, frame == launchFrame ? ACT_BACKFLIP : ACT_CROUCH_SLIDE);
                    CHECK_EQ(sCrouchSteps, frame == launchFrame ? frame : frame + 1);
                    CHECK_EQ(sMario.crouchSlideBackflip, pressFrame == 0 && frame == 0);
                    CHECK(fabsf(sMario.forwardVel - 40) < 0.001f);
                }
                set_mario_action(&sMario, ACT_CROUCH_SLIDE, 0);
                sMario.input = INPUT_NONZERO_ANALOG | INPUT_A_DOWN | INPUT_Z_DOWN;
                CHECK_EQ(act_crouch_slide(&sMario), FALSE);
                CHECK_EQ(sMario.action, ACT_CROUCH_SLIDE);
            }
        }
    }
    // Releasing A on the second update cancels the buffered backflip.
    begin(ACT_CROUCH_SLIDE, 40);
    sController.stickMag = 20;
    sMario.intendedYaw = (s16) 0x8000;
    sMario.input = INPUT_A_PRESSED | INPUT_A_DOWN | INPUT_NONZERO_ANALOG | INPUT_Z_DOWN;
    CHECK_EQ(act_crouch_slide(&sMario), FALSE);
    CHECK_EQ(sMario.crouchSlideBackflip, TRUE);
    sMario.input = INPUT_NONZERO_ANALOG | INPUT_Z_DOWN;
    CHECK_EQ(act_crouch_slide(&sMario), FALSE);
    CHECK_EQ(sMario.crouchSlideBackflip, FALSE);
    sMario.input |= INPUT_A_DOWN;
    CHECK_EQ(act_crouch_slide(&sMario), FALSE);
    CHECK_EQ(sMario.action, ACT_CROUCH_SLIDE);
    return TRUE;
}
static int crouch_slide_a(u32 unused) {
    // B falls back to breakdancing for weak or backward input, at any slide speed.
    const s16 attackYaws[] = { 0, 0x4000, 0x471C, 0x471D, (s16) 0x8000 };
    const f32 attackSticks[] = { 0, MARIO_STRONG_STICK_THRESHOLD - 0.1f, MARIO_STRONG_STICK_THRESHOLD,
                                 64 };
    const f32 attackSpeeds[] = { -40, 0, 40 };
    for (unsigned yaw = 0; yaw < 5; yaw++) {
        for (unsigned stick = 0; stick < 4; stick++) {
            for (unsigned speed = 0; speed < 3; speed++) {
                begin(ACT_CROUCH_SLIDE, attackSpeeds[speed]);
                sMario.intendedYaw = attackYaws[yaw];
                sController.stickMag = attackSticks[stick];
                sMario.input = INPUT_B_PRESSED | INPUT_Z_DOWN;
                if (attackSticks[stick] > 0) {
                    sMario.input |= INPUT_NONZERO_ANALOG;
                }
                CHECK_EQ(act_crouch_slide(&sMario), TRUE);
                s32 kick = stick >= 2 && yaw <= 2;
                CHECK_EQ(sMario.action, kick ? ACT_SLIDE_KICK : ACT_MOVE_PUNCHING);
                CHECK_EQ(sMario.actionArg, kick ? 0 : ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
                CHECK_EQ(sCrouchSteps, 0);
            }
        }
    }

    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_BUTT_SLIDE, -20.0f);
        sMario.actionTimer = 5;
        sMario.input = INPUT_A_PRESSED | INPUT_A_DOWN;
        CHECK_EQ(act_butt_slide(&sMario), TRUE);
        CHECK_EQ(sMario.action, ACT_JUMP);
        CHECK_EQ(sMario.actionArg, ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE);
        begin(ACT_BUTT_SLIDE, -20.0f);
        sFloor.normal.y = 0.8f;
        sFloor.normal.z = -0.6f;
        sMario.slideVelZ = 20.0f; // Facing downhill while moving backward uphill.
        sMario.actionTimer = 9;
        sMario.input = INPUT_A_PRESSED | INPUT_A_DOWN;
        CHECK_EQ(act_butt_slide(&sMario), TRUE);
        CHECK_EQ(sMario.action, ACT_JUMP);
        CHECK_EQ(sMario.actionArg,
                 ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE | ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL);
        begin(ACT_BUTT_SLIDE, -20.0f);
        sFloor.normal.y = 0.8f;
        sFloor.normal.z = -0.6f;
        sMario.slideVelZ = -20.0f; // Downhill carry can reverse while airborne.
        sMario.actionTimer = 9;
        sMario.input = INPUT_A_PRESSED | INPUT_A_DOWN;
        CHECK_EQ(act_butt_slide(&sMario), TRUE);
        CHECK_EQ(sMario.actionArg,
                 ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE | ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL);
        begin(ACT_HOLD_BUTT_SLIDE, -20.0f);
        sFloor.normal.y = 0.8f;
        sFloor.normal.z = -0.6f;
        sMario.actionTimer = 9;
        sMario.input = INPUT_A_PRESSED | INPUT_A_DOWN;
        CHECK_EQ(common_slide_action_with_jump(&sMario, ACT_HOLD_BEGIN_SLIDING, ACT_HOLD_JUMP,
                                               ACT_HOLD_FREEFALL, MARIO_ANIM_SLIDE, 0.0f),
                 TRUE);
        CHECK_EQ(sMario.action, ACT_HOLD_JUMP);
        CHECK_EQ(sMario.actionArg,
                 ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE | ACT_ARG_JUMP_BACKWARD_BUTT_SLIDE_NO_UPHILL);
        begin(ACT_BUTT_SLIDE, 20.0f);
        sMario.actionTimer = 5;
        sMario.input = INPUT_A_PRESSED | INPUT_A_DOWN;
        CHECK_EQ(act_butt_slide(&sMario), FALSE);
        CHECK_EQ(sMario.actionTimer, 6);
        sMario.input = INPUT_A_PRESSED | INPUT_A_DOWN;
        CHECK_EQ(act_butt_slide(&sMario), TRUE);
        CHECK_EQ(sMario.actionArg, 0);
    }
    // Neutral Z+A backflips throughout entry, even with residual slide momentum.
    const f32 speeds[] = { -40.0f, 0.0f, 40.0f };
    for (unsigned speed = 0; speed < 3; speed++) {
        for (int frame = 0; frame < 6; frame++) {
            for (int heldZ = 0; heldZ < 2; heldZ++) {
                begin(ACT_CROUCH_SLIDE, speeds[speed]);
                sController.stickMag = 0;
                sMario.actionTimer = frame;
                sMario.crouchSlideFrames = frame;
                sMario.input = INPUT_A_PRESSED | INPUT_A_DOWN | (heldZ ? INPUT_Z_DOWN : 0);
                CHECK_EQ(act_crouch_slide(&sMario), TRUE);
                CHECK_EQ(sMario.action, heldZ ? ACT_BACKFLIP : ACT_JUMP);
                CHECK_EQ(sCrouchSteps, 0);
            }
        }
    }

    // One completed slide update unlocks backward A, with Z held or released.
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int zHeld = 0; zHeld < 2; zHeld++) {
            begin(ACT_CROUCH_SLIDE, 40);
            sController.stickMag = 20;
            sMario.intendedYaw = (s16) 0x8000;
            sMario.input = INPUT_A_PRESSED | INPUT_A_DOWN | INPUT_NONZERO_ANALOG | INPUT_Z_DOWN;
            CHECK_EQ(act_crouch_slide(&sMario), FALSE);
            sMario.input = INPUT_A_DOWN | INPUT_NONZERO_ANALOG | (zHeld ? INPUT_Z_DOWN : 0);
            CHECK_EQ(act_crouch_slide(&sMario), TRUE);
            CHECK_EQ(sMario.action, ACT_BACKFLIP);
        }
    }

    const f32 mags[] = { 0, 54.39f, 54.4f, 64 };
    const s16 yaws[] = { 0, 0x1000, 0x4000, (s16) 0x8000 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int late = 0; late < 2; late++) {
            for (unsigned i = 0; i < 4; i++) {
                for (unsigned j = 0; j < 4; j++) {
                    begin(ACT_CROUCH_SLIDE, 40);
                    sMario.actionTimer = late ? 30 : 0;
                    sMario.crouchSlideFrames = 5;
                    sController.stickMag = mags[i];
                    sMario.intendedYaw = yaws[j];
                    sMario.input = INPUT_A_PRESSED | INPUT_A_DOWN | (i ? INPUT_NONZERO_ANALOG : 0);
                    int strong = i >= 2 && j <= 1;
                    int back = i > 0 && j == 3;
                    CHECK_EQ(act_crouch_slide(&sMario), TRUE);
                    CHECK_EQ(sMario.action, back              ? ACT_BACKFLIP
                                            : strong && !late ? ACT_LONG_JUMP
                                                              : ACT_JUMP);
                }
            }
        }
    }
    // Slope entry remains automatic and consumes A so it cannot also jump.
    begin(ACT_CROUCH_SLIDE, 40);
    sController.stickMag = 20;
    sMario.input = INPUT_A_PRESSED | INPUT_A_DOWN | INPUT_ABOVE_SLIDE;
    CHECK(crouch_dispatch());
    CHECK_EQ(sMario.action, ACT_BUTT_SLIDE);
    CHECK_EQ(sMario.input & INPUT_A_PRESSED, 0);

    begin(ACT_CROUCH_SLIDE, 40);
    sController.stickMag = 20;
    sMario.input = INPUT_A_PRESSED | INPUT_B_PRESSED;
    CHECK_EQ(act_crouch_slide(&sMario), TRUE);
    CHECK_EQ(sMario.action, ACT_MOVE_PUNCHING);
    CHECK_EQ(sMario.actionArg, ACT_ARG_PUNCH_SEQUENCE_BREAKDANCE);
    return TRUE;
}

static int crouch_slide_no_manual_butt(u32 unused) {
    // Every fresh weak forward A press jumps, including during slide startup.
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int zHeld = 0; zHeld < 2; zHeld++) {
            for (int slideFrame = 0; slideFrame <= 6; slideFrame++) {
                begin(ACT_CROUCH_SLIDE, 40);
                sController.stickMag = 20;
                sMario.input = INPUT_NONZERO_ANALOG | INPUT_Z_DOWN;
                for (int frame = 0; frame < slideFrame; frame++) {
                    CHECK_EQ(act_crouch_slide(&sMario), FALSE);
                    CHECK_EQ(sMario.action, ACT_CROUCH_SLIDE);
                }
                sMario.input =
                    INPUT_NONZERO_ANALOG | INPUT_A_PRESSED | INPUT_A_DOWN | (zHeld ? INPUT_Z_DOWN : 0);
                CHECK_EQ(act_crouch_slide(&sMario), TRUE);
                CHECK_EQ(sMario.action, ACT_JUMP);
            }
        }
    }
    // Holding A without a new press cannot create a delayed butt slide.
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(ACT_CROUCH_SLIDE, 40);
        sMario.input = INPUT_Z_DOWN | INPUT_A_DOWN;
        for (int frame = 0; frame < 8; frame++) {
            CHECK_EQ(act_crouch_slide(&sMario), FALSE);
            CHECK_EQ(sMario.action, ACT_CROUCH_SLIDE);
        }
    }
    return TRUE;
}
static int dive_grab_recovery(u32 action) {
    (void) action;
    const u32 inputs[] = { INPUT_A_PRESSED, INPUT_B_PRESSED, INPUT_Z_DOWN, INPUT_IDLE };
    const u32 exits[] = { ACT_HOLD_JUMP, ACT_THROWING, ACT_START_CROUCHING, ACT_HOLD_WALKING };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (int type = 0; type < 4; type++) {
            begin(ACT_DIVE_PICKING_UP, 12.0f);
            sMario.heldObj = &sTarget;
            sMario.faceAngle[1] = sMario.intendedYaw = 0x2000;
            set_mario_animation(&sMario, MARIO_ANIM_STOP_SLIDE_LIGHT_OBJ);
            CHECK_EQ(sMarioObject.header.gfx.animInfo.curAnim->loopEnd, 37);
            sMarioObject.header.gfx.animInfo.animFrame = 25;
            sMario.input = inputs[type];
            sController.stickMag = type == 3 ? 64.0f : 0.0f;
            sGroundSteps = 0;
            CHECK_EQ(act_dive_picking_up(&sMario), FALSE);
            CHECK_EQ(sMario.action, ACT_DIVE_PICKING_UP);
            CHECK_EQ(sGroundSteps, 1);

            sMarioObject.header.gfx.animInfo.animFrame = 26;
            sGroundSteps = 0;
            CHECK_EQ(act_dive_picking_up(&sMario), TRUE);
            CHECK_EQ(sMario.action, exits[type]);
            CHECK_EQ(sGroundSteps, 0);
            CHECK_EQ(sMario.faceAngle[1], 0x2000);
            if (type == 3) {
                CHECK(sMario.input & INPUT_NONZERO_ANALOG);
                CHECK(!(sMario.input & INPUT_IDLE));
                CHECK_EQ(sMario.intendedYaw, 0x2000);
                CHECK_EQ(sMario.intendedMag, 0);
            } else {
                CHECK_EQ(sMario.heldObj, type == 2 ? NULL : &sTarget);
            }
        }
    }
    begin(ACT_DIVE_PICKING_UP, 12.0f);
    sMario.heldObj = &sTarget;
    set_mario_animation(&sMario, MARIO_ANIM_STOP_SLIDE_LIGHT_OBJ);
    sMarioObject.header.gfx.animInfo.animFrame = 26;
    sMario.input = 0;
    sGroundSteps = 0;
    CHECK_EQ(act_dive_picking_up(&sMario), FALSE);
    CHECK_EQ(sMario.action, ACT_DIVE_PICKING_UP);
    CHECK_EQ(sGroundSteps, 1);

    begin(ACT_DIVE_PICKING_UP, 12.0f);
    set_mario_animation(&sMario, MARIO_ANIM_STOP_SLIDE_LIGHT_OBJ);
    sMarioObject.header.gfx.animInfo.animFrame = 26;
    sMario.input = INPUT_B_PRESSED;
    CHECK_EQ(act_dive_picking_up(&sMario), FALSE);
    CHECK_EQ(sMario.action, ACT_DIVE_PICKING_UP);

    const f32 finishSpeeds[] = { 12, -12, 0.5f, -0.5f, 0 };
    for (int cycle = 0; cycle < 3; cycle++) {
        for (unsigned i = 0; i < sizeof(finishSpeeds) / sizeof(*finishSpeeds); i++) {
            begin(ACT_DIVE_PICKING_UP, finishSpeeds[i]);
            sMario.heldObj = &sTarget;
            set_mario_animation(&sMario, MARIO_ANIM_STOP_SLIDE_LIGHT_OBJ);
            sMarioObject.header.gfx.animInfo.animFrame = 36;
            CHECK_EQ(act_dive_picking_up(&sMario), TRUE);
            CHECK_EQ(sMario.action, finishSpeeds[i] != 0 ? ACT_HOLD_DECELERATING : ACT_HOLD_IDLE);
            CHECK_EQ(sMario.forwardVel, finishSpeeds[i]);
            CHECK_EQ(sMario.heldObj, &sTarget);
        }
    }
    for (int type = 0; type < 2; type++) {
        for (int age = 0; age <= 6; age++) {
            begin(ACT_DIVE_PICKING_UP, 12.0f);
            sMario.heldObj = &sTarget;
            set_mario_animation(&sMario, MARIO_ANIM_STOP_SLIDE_LIGHT_OBJ);
            sMario.objectRecoveryAFrames = type == 0 && age < 6 ? 6 - age : 0;
            sMario.objectRecoveryBFrames = type == 1 && age < 6 ? 6 - age : 0;
            sMarioObject.header.gfx.animInfo.animFrame = 25;
            sMario.input = 0;
            CHECK_EQ(act_dive_picking_up(&sMario), FALSE);
            CHECK_EQ(sMario.input, 0);
            sMarioObject.header.gfx.animInfo.animFrame = 26;
            CHECK_EQ(act_dive_picking_up(&sMario), age < 6);
            if (age < 6) {
                CHECK_EQ(sMario.action, type == 0 ? ACT_HOLD_JUMP : ACT_THROWING);
                CHECK_EQ(sMario.objectRecoveryAFrames | sMario.objectRecoveryBFrames, 0);
            }
        }
    }
    return TRUE;
}

static int punch_explosion_windows(u32 action) {
    begin(action, 0);
    for (int frame = 0; frame < 120 && punching(); frame++) {
        tick(frame < 60, 0);
        CHECK_EQ(sPunchExplosions, 0); // Complete missed combo emits no blasts.
    }
    for (int cycle = 0; cycle < 3; cycle++) {
        begin(action, 0);
        CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH, 2, TRUE));
        CHECK_EQ(sPunchExplosions, 0);
        tick(TRUE, 0);
        CHECK_EQ(sPunchExplosions, 0);
        he_thicc_attack_contact(&sMario, INT_PUNCH);
        he_thicc_attack_contact(&sMario, INT_PUNCH);
        CHECK_EQ(sPunchExplosions, 1);
        CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH, 1, TRUE));
        CHECK_EQ(sPunchExplosions, 1);
        tick(TRUE, 0);
        CHECK_EQ(sPunchExplosions, 1);
        he_thicc_attack_contact(&sMario, INT_PUNCH);
        CHECK_EQ(sPunchExplosions, 2);
        CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_GROUND_KICK, 1, TRUE));
        CHECK_EQ(sPunchExplosions, 2);
        he_thicc_attack_contact(&sMario, INT_KICK);
        he_thicc_attack_contact(&sMario, INT_KICK);
        CHECK_EQ(sPunchExplosions, 3);
        for (int frame = 0; frame < 80 && punching(); frame++) {
            tick(FALSE, 0);
        }
        CHECK_EQ(sPunchExplosions, 3);
        CHECK(!punching());
    }
    for (int pair = 0; pair < 2; pair++) {
        begin(action, 0);
        CHECK(arm_repeat(pair));
        int phase =
            pair ? ACT_ARG_PUNCH_SEQUENCE_SECOND_PUNCH_FAST : ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH_FAST;
        CHECK(reach(phase, 1, TRUE));
        for (int cycle = 0; cycle < 3; cycle++) {
            int count = sPunchExplosions;
            CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH, 2, TRUE));
            CHECK_EQ(sPunchExplosions, count);
            tick(TRUE, 0);
            CHECK_EQ(sPunchExplosions, count);
            he_thicc_attack_contact(&sMario, INT_PUNCH);
            CHECK_EQ(sPunchExplosions, count + 1);
            CHECK(reach(phase, 1, TRUE));
            if (pair) {
                he_thicc_attack_contact(&sMario, INT_PUNCH);
            }
            CHECK_EQ(sPunchExplosions, count + (pair ? 2 : 1));
        }
    }
    begin(action, 0);
    CHECK(reach(ACT_ARG_PUNCH_SEQUENCE_FIRST_PUNCH, 2, TRUE));
    sGrabRequested = TRUE;
    tick(TRUE, 0);
    CHECK_EQ(sMario.action, ACT_PICKING_UP);
    CHECK_EQ(sPunchExplosions, 0);
    return TRUE;
}

int main(int argc, char **argv) {
    const struct {
        const char *name;
        int (*run)(u32);
    } tests[] = {
        { "punch_explosion_windows", punch_explosion_windows },
        { "combo", combo },
        { "singles", singles },
        { "pairs", pairs },
        { "release_restart", release_and_restart },
        { "interruptions", interruptions },
        { "cancel_stick", cancel_and_stick },
        { "lunge_grab_hits", lunge_and_grab },
        { "moving_start_repeat_lunge", moving_start_repeat_lunge },
        { "rebound_keeps_speed", rebound_keeps_speed },
        { "attack_steering", attack_steering },
        { "recovery_turn", recovery_turn },
        { "breakdance_no_lunge", breakdance_no_lunge },
        { "breakdance_completion_timing", breakdance_completion_timing },
        { "breakdance_recovery_windows", breakdance_recovery_windows },
        { "breakdance_steer_and_carry", breakdance_steer_and_carry },
        { "breakdance_crouch_momentum", breakdance_crouch_momentum },
        { "breakdance_crawl_return", breakdance_crawl_return },
        { "breakdance_crawl_repeat", breakdance_crawl_repeat },
        { "breakdance_cap_release", breakdance_cap_release },
        { "forced_cap_release", forced_cap_release },
        { "breakdance_cap_underwater", breakdance_cap_underwater },
        { "cap_land_movement_underwater", cap_land_movement_underwater },
        { "cap_presets_underwater", cap_presets_underwater },
        { "breakdance_cap_startup_only", breakdance_cap_startup_only },
        { "breakdance_cap_crouch_entry", breakdance_cap_crouch_entry },
        { "input_boundaries", input_boundaries },
        { "cancel_boundaries", cancel_boundaries },
        { "ground_exit", ground_exit },
        { "crouch_entry_exit", crouch_entry_exit },
        { "crawl_cancel_windows", crawl_cancel_windows },
        { "crouch_slide_cancel", crouch_slide_cancel },
        { "crouch_start_animation", crouch_start_animation },
        { "crouch_lowering_samples", crouch_lowering_samples },
        { "crouch_lowering_playback", crouch_lowering_playback },
        { "crouch_slide_a", crouch_slide_a },
        { "crouch_slide_no_manual_butt", crouch_slide_no_manual_butt },
        { "crouch_slide_backflip_buffer", crouch_slide_backflip_buffer },
        { "dive_grab_recovery", dive_grab_recovery },
    };
    test_init(argc, argv);
    for (int moving = 0; moving <= 1; moving++) {
        for (unsigned int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
            char name[120];
            snprintf(name, sizeof(name), "punch.%s.%s", moving ? "moving" : "standing", tests[i].name);
            if (test_selected(name)) {
                sActiveCase = name;
                test_report(name, tests[i].run(moving ? ACT_MOVE_PUNCHING : ACT_PUNCHING));
            }
        }
    }
    return test_finish();
}
